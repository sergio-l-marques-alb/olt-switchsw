/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename lldp_api.c
*
* @purpose 802.1AB API functions
*
* @component 802.1AB
*
* @comments none
*
* @create 02/01/2005
*
* @author dfowler
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "osapi_support.h"
#include "lldp.h"
#include "lldp_api.h"
#include "lldp_util.h"
#include "osapi.h"
#include "simapi.h"
#include "usmdb_common.h"
#include "usmdb_1213_api.h"
#include "usmdb_2233_stats_api.h"
#include "lldp_exports.h"

#ifdef L7_ROUTING_PACKAGE
#include "l7_ip_api.h"
#endif

extern lldpCfgData_t      *lldpCfgData;
extern lldpStats_t        *lldpStats;
extern void               *lldpSemaphore;
extern lldpIntfOprData_t  *lldpIntfTbl;
extern const L7_uchar8 LLDP_MED_TIA_OUI_STRING[LLDP_TIA_OUI_STRING_LEN];


/*********************************************************************
 *                802.1AB Configuration APIs
 *********************************************************************/

/*********************************************************************
*
* @purpose  Sets the 802.1AB global transmit interval
*
* @param    L7_uint32  interval  @b((input))  interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxIntervalSet(L7_uint32 interval)
{
  if (interval < LLDP_TX_INTERVAL_MIN ||
      interval > LLDP_TX_INTERVAL_MAX)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpCfgData->txInterval != interval)
  {
    lldpCfgData->txInterval = interval;
    lldpCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Gets the 802.1AB global transmit interval
*
* @param    L7_uint32  *interval @b((output)) interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxIntervalGet(L7_uint32 *interval)
{
  if (interval == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  *interval = lldpCfgData->txInterval;
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Sets the 802.1AB global transmit delay
*
* @param    L7_uint32  delay  @b((input))  delay in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxDelaySet(L7_uint32 delay)
{
  if (delay < LLDP_TX_DELAY_MIN ||
      delay > LLDP_TX_DELAY_MAX)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  /* The other condition that has to be met is w.r.t txInterval */
  if (delay > (lldpCfgData->txInterval/4))
  {
	  (void)osapiSemaGive(lldpSemaphore);
	  return L7_FAILURE;
  }
  
  if (lldpCfgData->txDelay != delay)
  {
    lldpCfgData->txDelay = delay;
    lldpCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Gets the 802.1AB global transmit delay
*
* @param    L7_uint32  *delay @b((output)) interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxDelayGet(L7_uint32 *delay)
{
  if (delay == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  *delay = lldpCfgData->txDelay;
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB global transmit hold muliplier
*
* @param    L7_uint32  hold      @b((input))  hold multiplier
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid hold value
*
* @notes    hold mulitplier must be between 2-10 seconds
*
* @end
*********************************************************************/
L7_RC_t lldpTxHoldSet(L7_uint32 hold)
{
  if (hold < LLDP_TX_HOLD_MULTIPLIER_MIN ||
      hold > LLDP_TX_HOLD_MULTIPLIER_MAX)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpCfgData->txHoldMultiplier != hold)
  {
    lldpCfgData->txHoldMultiplier = hold;
    lldpCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB global transmit hold muliplier
*
* @param    L7_uint32  *hold     @b((output)) hold multiplier
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxHoldGet(L7_uint32 *hold)
{
  if (hold == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  *hold = lldpCfgData->txHoldMultiplier;
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB global transmit reinit delay
*
* @param    L7_uint32  delay     @b((input))  delay in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid delay value
*
* @notes    delay must be between 1-10 seconds
*
* @end
*********************************************************************/
L7_RC_t lldpTxReinitDelaySet(L7_uint32 delay)
{
  if (delay < LLDP_REINIT_DELAY_MIN ||
      delay > LLDP_REINIT_DELAY_MAX)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpCfgData->reinitDelay != delay)
  {
    lldpCfgData->reinitDelay = delay;
    lldpCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB global transmit reinit delay
*
* @param    L7_uint32  *delay     @b((output)) delay in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTxReinitDelayGet(L7_uint32 *delay)
{
  if (delay == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  *delay = lldpCfgData->reinitDelay;
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB global notification interval limit
*
* @param    L7_uint32  interval  @b((input))  interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpNotificationIntervalSet(L7_uint32 interval)
{
  if (interval < LLDP_NOTIFY_INTERVAL_MIN ||
      interval > LLDP_NOTIFY_INTERVAL_MAX)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpCfgData->notifyInterval != interval)
  {
    lldpCfgData->notifyInterval = interval;
    lldpCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB global notification interval limit
*
* @param    L7_uint32  *interval @b((output)) interval in seconds
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpNotificationIntervalGet(L7_uint32 *interval)
{
  if (interval == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  *interval = lldpCfgData->notifyInterval;
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Sets the 802.1AB transmit mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) internal interface number
* @param    L7_uint32  mode      @b((input))  802.1AB transmit mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  lldpIntfCfgData_t *pCfg  = L7_NULLPTR;
  L7_uint32          index = 0;

  if (mode != L7_ENABLE && mode != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_NOT_SUPPORTED;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  pCfg = &lldpCfgData->intfCfgData[index];
  if ((mode == L7_ENABLE && pCfg->txEnabled == L7_TRUE) ||
      (mode == L7_DISABLE && pCfg->txEnabled == L7_FALSE))
  {
    osapiSemaGive(lldpSemaphore);
    return L7_SUCCESS;
  }

  pCfg->txEnabled = (mode == L7_ENABLE) ? L7_TRUE : L7_FALSE;
  /* If this port has disabled tx function then medEnabled has to be disabled */
  if (pCfg->txEnabled == L7_FALSE)
  {
    pCfg->medEnabled = L7_FALSE;
  }

  /* apply the config change */
  lldpIntfModeApplyPostMsg(intIfNum,L7_FALSE,L7_TRUE); /* Tx Apply only*/

  if (mode == L7_DISABLE)
  {
    /* clear the operational tx data for this port */
    lldpStats->intfStats[index].txFramesTotal = 0;
  }

  lldpCfgData->cfgHdr.dataChanged = L7_TRUE;
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB transmit mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_uint32  *mode     @b((output)) configured 802.1AB intf mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  lldpIntfCfgData_t *pCfg  = L7_NULLPTR;
  L7_uint32          index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  pCfg = &lldpCfgData->intfCfgData[index];
  *mode = (pCfg->txEnabled == L7_TRUE) ? L7_ENABLE : L7_DISABLE;

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB receive mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_uint32  mode      @b((input))  intf receive mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfRxModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  lldpIntfCfgData_t *pCfg  = L7_NULLPTR;
  L7_uint32          index = 0;

  if (mode != L7_ENABLE && mode != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_NOT_SUPPORTED;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  pCfg = &lldpCfgData->intfCfgData[index];
  if ((mode == L7_ENABLE && pCfg->rxEnabled == L7_TRUE) ||
      (mode == L7_DISABLE && pCfg->rxEnabled == L7_FALSE))
  {
    osapiSemaGive(lldpSemaphore);
    return L7_SUCCESS;
  }

  pCfg->rxEnabled = (mode == L7_ENABLE) ? L7_TRUE : L7_FALSE;
  /* If this port has disabled rx function then medEnabled has to be disabled */
  if (pCfg->rxEnabled == L7_FALSE)
  {
    pCfg->medEnabled = L7_FALSE;
  }

  /* apply the config change, this will clear remote data if required */
  lldpIntfModeApplyPostMsg(intIfNum,L7_TRUE,L7_FALSE); /* Rx Apply only */

  if (mode == L7_DISABLE)
  {
    /* clear the operational rx data for this port */
    lldpStats->intfStats[index].rxFramesDiscarded = 0;
    lldpStats->intfStats[index].rxFramesErrors = 0;
    lldpStats->intfStats[index].rxFramesTotal = 0;
    lldpStats->intfStats[index].rxTLVsDiscarded = 0;
    lldpStats->intfStats[index].rxTLVsUnrecognized = 0;
    lldpStats->intfStats[index].rxAgeouts = 0;
    lldpStats->intfStats[index].rxTLVs8021 = 0;
    lldpStats->intfStats[index].rxTLVs8023 = 0;
    lldpStats->intfStats[index].rxTLVsMED = 0;
  }

  lldpCfgData->cfgHdr.dataChanged = L7_TRUE;
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB receive mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_uint32  *mode     @b((output)) configured intf receive mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfRxModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  lldpIntfCfgData_t *pCfg  = L7_NULLPTR;
  L7_uint32          index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  pCfg = &lldpCfgData->intfCfgData[index];
  *mode = (pCfg->rxEnabled == L7_TRUE) ? L7_ENABLE : L7_DISABLE;

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB notify mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) internal interface number
* @param    L7_uint32  mode      @b((input)) intf notify mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfNotificationModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  lldpIntfCfgData_t *pCfg  = L7_NULLPTR;
  L7_uint32          index = 0;

  if (mode != L7_ENABLE && mode != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_NOT_SUPPORTED;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  pCfg = &lldpCfgData->intfCfgData[index];
  if ((mode == L7_ENABLE && pCfg->notificationEnabled == L7_TRUE) ||
      (mode == L7_DISABLE && pCfg->notificationEnabled == L7_FALSE))
  {
    osapiSemaGive(lldpSemaphore);
    return L7_SUCCESS;
  }

  pCfg->notificationEnabled = (mode == L7_ENABLE) ? L7_TRUE : L7_FALSE;


  lldpCfgData->cfgHdr.dataChanged = L7_TRUE;
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB notify mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_uint32  *mode     @b((output)) intf notify mode
*
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfNotificationModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  lldpIntfCfgData_t *pCfg  = L7_NULLPTR;
  L7_uint32          index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  pCfg = &lldpCfgData->intfCfgData[index];
  *mode = (pCfg->notificationEnabled == L7_TRUE) ? L7_ENABLE : L7_DISABLE;

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB optional transmit TLVs for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_BOOL    portDesc  @b((input))  transmit port desc TLV
* @param    L7_BOOL    sysName   @b((input))  transmit system name TLV
* @param    L7_BOOL    sysDesc   @b((input))  transmit system desc TLV
* @param    L7_BOOL    sysCap    @b((input))  transmit system capability TLV
*
* @returns  L7_SUCCESS, if optional TLVs are set successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxTLVsSet(L7_uint32 intIfNum,
                          L7_BOOL   portDesc,
                          L7_BOOL   sysName,
                          L7_BOOL   sysDesc,
                          L7_BOOL   sysCap)
{
  lldpIntfCfgData_t *pCfg  = L7_NULLPTR;
  L7_uint32          index = 0;
  L7_uint32          optionalTLVsEnabled = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_NOT_SUPPORTED;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  pCfg = &lldpCfgData->intfCfgData[index];

  if (portDesc == L7_TRUE)
  {
    optionalTLVsEnabled |= LLDP_TX_TLV_PORT_DESC_BIT;
  }
  if (sysName == L7_TRUE)
  {
    optionalTLVsEnabled |= LLDP_TX_TLV_SYS_NAME_BIT;
  }
  if (sysDesc == L7_TRUE)
  {
    optionalTLVsEnabled |= LLDP_TX_TLV_SYS_DESC_BIT;
  }
  if (sysCap == L7_TRUE)
  {
    optionalTLVsEnabled |= LLDP_TX_TLV_SYS_CAP_BIT;
  }

  if (pCfg->optionalTLVsEnabled != optionalTLVsEnabled)
  {
    pCfg->optionalTLVsEnabled = optionalTLVsEnabled;
    lldpCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB optional transmit TLVs for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) internal interface number
* @param    L7_BOOL    portDesc  @b((output))  transmit port desc TLV
* @param    L7_BOOL    sysName   @b((output))  transmit system name TLV
* @param    L7_BOOL    sysDesc   @b((output))  transmit system desc TLV
* @param    L7_BOOL    sysCap    @b((output))  transmit system capability TLV
*
* @returns  L7_SUCCESS, if values retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxTLVsGet(L7_uint32 intIfNum,
                          L7_BOOL   *portDesc,
                          L7_BOOL   *sysName,
                          L7_BOOL   *sysDesc,
                          L7_BOOL   *sysCap)
{
  lldpIntfCfgData_t *pCfg  = L7_NULLPTR;
  L7_uint32          index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  pCfg = &lldpCfgData->intfCfgData[index];

  *portDesc = (pCfg->optionalTLVsEnabled & LLDP_TX_TLV_PORT_DESC_BIT) ? L7_TRUE : L7_FALSE;
  *sysName  = (pCfg->optionalTLVsEnabled & LLDP_TX_TLV_SYS_NAME_BIT) ? L7_TRUE : L7_FALSE;
  *sysDesc  = (pCfg->optionalTLVsEnabled & LLDP_TX_TLV_SYS_DESC_BIT) ? L7_TRUE : L7_FALSE;
  *sysCap   = (pCfg->optionalTLVsEnabled & LLDP_TX_TLV_SYS_CAP_BIT) ? L7_TRUE : L7_FALSE;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Sets the 802.1AB mgmt addr transmit mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  internal interface number
* @param    L7_BOOL    enabled   @b((input))  transmit mgmt addr TLV
*
* @returns  L7_SUCCESS, if mgmt addr transmit is set successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxMgmtAddrSet(L7_uint32 intIfNum, L7_BOOL enabled)
{
  lldpIntfCfgData_t *pCfg  = L7_NULLPTR;
  L7_uint32          index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_NOT_SUPPORTED;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  pCfg = &lldpCfgData->intfCfgData[index];

  if (pCfg->mgmtAddrTxEnabled != enabled)
  {
    pCfg->mgmtAddrTxEnabled = enabled;
    lldpCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB mgmt addr transmit mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) internal interface number
* @param    L7_BOOL    enabled   @b((output))  transmit mgmt addr TLV
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpIntfTxMgmtAddrGet(L7_uint32 intIfNum, L7_BOOL *enabled)
{
  L7_uint32 index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *enabled = lldpCfgData->intfCfgData[index].mgmtAddrTxEnabled;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
 *                802.1AB Statistics APIs
 *********************************************************************/

/*********************************************************************
*
* @purpose  Clears all 802.1AB statistics
*
* @param    none
*
* @returns  L7_SUCCESS, if statistics cleared successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsClear(void)
{
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  memset(lldpStats, 0, sizeof(lldpStats_t));
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the last update time for 802.1AB remote data table
*
* @param    L7_uint32  lastUpate   @b((output))  time of last update
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    lastUpdate is expressed in seconds, system up time.
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRemoteLastUpdateGet(L7_uint32 *lastUpdate)
{
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);

  *lastUpdate = lldpStats->remTblLastChangeTime;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the insert count for 802.1AB remote data table
*
* @param    L7_uint32   inserts   @b((output))  insert count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRemoteInsertsGet(L7_uint32 *inserts)
{
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);

  *inserts = lldpStats->remTblInserts;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the delete count for 802.1AB remote data table
*
* @param    L7_uint32   deletes   @b((output))  delete count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRemoteDeletesGet(L7_uint32 *deletes)
{
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);

  *deletes = lldpStats->remTblDeletes;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the drop count for 802.1AB remote data table
*
* @param    L7_uint32   drops   @b((output))  drop count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRemoteDropsGet(L7_uint32 *drops)
{
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);

  *drops = lldpStats->remTblDrops;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the ageout count for 802.1AB remote data table
*
* @param    L7_uint32   ageouts   @b((output))  ageout count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRemoteAgeoutsGet(L7_uint32 *ageouts)
{
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);

  *ageouts = lldpStats->remTblAgeouts;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the LLDP transmit frame count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  transmit frame count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsTxPortFramesTotalGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  L7_uint32 index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *total = lldpStats->intfStats[index].txFramesTotal;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the LLDP receive frame count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  receive frame count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortFramesTotalGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  L7_uint32 index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *total = lldpStats->intfStats[index].rxFramesTotal;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the LLDP discarded frame count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  discarded frame count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortFramesDiscardedGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  L7_uint32 index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *total = lldpStats->intfStats[index].rxFramesDiscarded;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the LLDP errors frame count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  errors frame count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortFramesErrorsGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  L7_uint32 index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *total = lldpStats->intfStats[index].rxFramesErrors;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the LLDP ageouts count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  ageouts count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortAgeoutsGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  L7_uint32 index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *total = lldpStats->intfStats[index].rxAgeouts;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the LLDP TLV discarded count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  discarded count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortTLVsDiscardedGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  L7_uint32 index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *total = lldpStats->intfStats[index].rxTLVsDiscarded;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the LLDP TLV unrecognized count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  unrecognized count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortTLVsUnrecognizedGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  L7_uint32 index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *total = lldpStats->intfStats[index].rxTLVsUnrecognized;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Gets the LLDP TLV MED count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  unrecognized count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortTLVsMEDGet(L7_uint32 intIfNum, L7_uint32 *total)
{
  L7_uint32 index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *total = lldpStats->intfStats[index].rxTLVsMED;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Gets the LLDP TLV 8023 count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  unrecognized count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortTLVs8023Get(L7_uint32 intIfNum, L7_uint32 *total)
{
  L7_uint32 index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *total = lldpStats->intfStats[index].rxTLVs8023;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Gets the LLDP TLV 8021 count for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))   internal interface number
* @param    L7_uint32  total     @b((output))  unrecognized count
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpStatsRxPortTLVs8021Get(L7_uint32 intIfNum, L7_uint32 *total)
{
  L7_uint32 index = 0;

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *total = lldpStats->intfStats[index].rxTLVs8021;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
 *                802.1AB Local Data APIs
 *********************************************************************/

/*********************************************************************
*
* @purpose  Gets the 802.1AB local chassis id subtype
*
* @param    lldpChassisIdSubtype_t  subtype  @b((output))  Chassis ID Subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocChassisIdSubtypeGet(lldpChassisIdSubtype_t *subtype)
{
  if (subtype == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *subtype = LLDP_CHASSIS_ID_SUBTYPE_MAC_ADDR;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local chassis id
*
* @param    L7_uchar8   *chassisId  @b((output))  Chassis ID
* @param    L7_ushort16 *length     @b((output))  Chassis ID length
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    chassisId buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*           chassisId is encoded based on the ChassisIdSubtype
*
* @end
*********************************************************************/
L7_RC_t lldpLocChassisIdGet(L7_uchar8 *chassisId, L7_ushort16 *length)
{
  if (chassisId == L7_NULLPTR ||
      length == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
  {
    simGetSystemIPBurnedInMac(chassisId);
  }
  else
  {
    simGetSystemIPLocalAdminMac(chassisId);
  }
  *length = L7_MAC_ADDR_LEN;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local system name
*
* @param    L7_char8  sysName  @b((output))  system name
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    sysName buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpLocSysNameGet(L7_char8 *sysName)
{
  if (sysName == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  simGetSystemName(sysName);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local system description
*
* @param    L7_char8  sysDesc  @b((output))  system description
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    sysDesc buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpLocSysDescGet(L7_char8 *sysDesc)
{
  if (sysDesc == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  return usmDb1213SysDescrGet(USMDB_UNIT_CURRENT, sysDesc);
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local system capabilities supported
*
* @param    L7_ushort16  sysCap  @b((output))  bit mask of lldpSysCapMask_t
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocSysCapSupportedGet(L7_ushort16 *sysCap)
{
  if (sysCap == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  *sysCap = LLDP_SYS_CAP_MASK_BRIDGE;

#ifdef L7_ROUTING_PACKAGE
  *sysCap |= LLDP_SYS_CAP_MASK_ROUTER;
#endif

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local system capabilities enabled
*
* @param    L7_ushort16  sysCap  @b((output))  bit mask of lldpSysCapMask_t
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocSysCapEnabledGet(L7_ushort16 *sysCap)
{
  if (sysCap == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  *sysCap = LLDP_SYS_CAP_MASK_BRIDGE;

#ifdef L7_ROUTING_PACKAGE
  if (ipMapRtrAdminModeGet() == L7_TRUE)
  {
    *sysCap |= LLDP_SYS_CAP_MASK_ROUTER;
  }
#endif

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local port subtype for the specified interface
*
* @param    L7_uint32            intIfNum @b((input))   internal interface number
* @param    lldpPortIdSubtype_t  subtype  @b((output))  Port ID Subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocPortIdSubtypeGet(L7_uint32 intIfNum,
                                lldpPortIdSubtype_t *subtype)
{
  if (subtype == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  *subtype = LLDP_PORTID_SUBTYPE_SUPPORTED;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local port id for the specified interface
*
* @param    L7_uint32   intIfNum  @b((input))   internal interface number
* @param    L7_uchar8   *portId   @b((output))  Port ID
* @param    L7_ushort16 *length   @b((output))  Port ID length
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    portId buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*           portId is encoded based on the ChassisIdSubtype
*
* @end
*********************************************************************/
L7_RC_t lldpLocPortIdGet(L7_uint32   intIfNum,
                         L7_uchar8   *portId,
                         L7_ushort16 *length)
{
  L7_uint32 addrType = 0;

  if (portId == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  switch (LLDP_PORTID_SUBTYPE_SUPPORTED)
  {
    case LLDP_PORT_ID_SUBTYPE_MAC_ADDR:
  if (nimGetIntfAddrType(intIfNum, &addrType) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (nimGetIntfAddress(intIfNum, addrType, portId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  *length = L7_MAC_ADDR_LEN;
      break;
    case LLDP_PORT_ID_SUBTYPE_INTF_NAME:
      if (usmDbIfNameGet(0, intIfNum, portId) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      *length = strlen(portId);
      break;
    default:
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB local port description for the specified interface
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_char8  *portDesc  @b((output))  Port Description
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    portDesc buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpLocPortDescGet(L7_uint32 intIfNum,
                           L7_char8 *portDesc)
{
  if (portDesc == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  /* there is a numGetIntfDesc, but no API to set the description,
     we'll use the alias which may have descriptive information */
  if (nimGetIntfName(intIfNum, L7_ALIASNAME, (L7_char8 *)portDesc) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB management address
*
* @param    lldpIANAAddrFamilyNumber_t *family   @b((output))   IANA address family
* @param    L7_uchar8                  *address  @b((output))   management address
* @param    L7_uchar8                  *length   @b((output))   address length
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    address is encoded based on family
*
* @end
*********************************************************************/
L7_RC_t lldpLocManAddrGet(lldpIANAAddrFamilyNumber_t *family,
                          L7_uchar8                  *address,
                          L7_uchar8                  *length)
{
  L7_uint32 ip = 0;

  if (family == L7_NULLPTR ||
      address == L7_NULLPTR ||
      length == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* return IP address if defined, otherwise MAC */
  ip = simGetSystemIPAddr();
  if (ip != 0)
  {
    memcpy(address, &ip, sizeof(L7_uint32));
    *length = sizeof(L7_uint32);
    *family = LLDP_IANA_ADDR_FAMILY_NUMBER_IPV4;
    return L7_SUCCESS;
  }

  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
  {
    simGetSystemIPBurnedInMac(address);
  }
  else
  {
    simGetSystemIPLocalAdminMac(address);
  }
  *length = L7_MAC_ADDR_LEN;
  *family = LLDP_IANA_ADDR_FAMILY_NUMBER_802;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Gets the 802.1AB management address interface subtype
*
* @param    lldpManAddrIfSubtype_t *subtype @b((output))  interface subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocManAddrIfSubtypeGet(lldpManAddrIfSubtype_t *subtype)
{
  if (subtype == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  *subtype = LLDP_MAN_ADDR_IF_SUBTYPE_IF_INDEX;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB management address interface id
*
* @param    L7_uint32  *ifId @b((output))  interface id
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpLocManAddrIfIdGet(L7_uint32 *ifId)
{
  L7_uint32 intIfNum;

  if (ifId == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (nimFirstValidIntfNumberByType(L7_CPU_INTF, &intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (nimGetIntfIfIndex(intIfNum, ifId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets the 802.1AB management address OID
*
* @param    L7_char8  *addrOID @b((output))  management address OID
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    addrOID buffer should be at least LLDP_MGMT_ADDR_OID_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpLocManAddrOIDGet(L7_char8 *addrOID)
{
  if (addrOID == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  /* OID returned is a string, not binary,
     the API doesn't give us a length anyway */
  simGetSystemOID((L7_char8 *)addrOID);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets a flag indicating a change in the Local 802.31AB Database.
*
* @param    intIfNum    Internal    Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t lldpLocDbChangeSet(L7_uint32 intIfNum)
{
  L7_uint32          index = 0;
  if (lldpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if (lldpMapIntfIndexGet(intIfNum, &index) != L7_TRUE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }
  lldpIntfTbl[index].localDbChanged = L7_TRUE;
  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}
/*********************************************************************
 *                802.1AB Remote Data APIs
 *********************************************************************/


/*********************************************************************
*
* @purpose  Clears all 802.1AB remote data
*
* @param    none
*
* @returns  L7_SUCCESS, if datacleared successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpRemTableClear(void)
{
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);

  lldpRemoteDBClearPostMsg();
  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the unique values to identify an interface remote entry
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 *remIndex  @b((output))  remote data index
* @param    L7_uint32 *timestamp @b((output))  remote data timestamp
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    each interface can only have one remote entry.
*
* @end
*********************************************************************/
L7_RC_t lldpRemEntryGet(L7_uint32 intIfNum,
                        L7_uint32 *remIndex,
                        L7_uint32 *timestamp)
{
  lldpRemDataEntry_t *remEntry;

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);

  remEntry = lldpRemDataNeighborGetNext(intIfNum,L7_NULLPTR);
  /* Check if there is a valid remote entry for this interface */
  if (remEntry == L7_NULLPTR)
  {
    *remIndex = 0;
    *timestamp = 0;
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  /* intIfNum, index, timestamp uniquely identify the remote entry,
     if entry is deleted or modified, these will not be valid */
  *remIndex = remEntry->remIndex;
  *timestamp = remEntry->timestamp;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the unique values to identify an interface remote entry
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 *remIndex  @b((output))  remote data index
* @param    L7_uint32 *timestamp @b((output))  remote data timestamp
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    each interface can only have one remote entry.
*
* @end
*********************************************************************/
L7_RC_t lldpRemEntryGetNext(L7_uint32 intIfNum,
                            L7_uint32 *remIndex,
                            L7_uint32 *timestamp)
{
  lldpRemDataEntry_t *remEntry,*prevEntry=L7_NULLPTR;

  if(*remIndex == 0)
  {
    return lldpRemEntryGet(intIfNum,remIndex,timestamp);
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  while((remEntry = lldpRemDataNeighborGetNext(intIfNum,prevEntry))!=L7_NULLPTR)
  {
    if(remEntry->remIndex == *remIndex)
    {
      prevEntry = remEntry;
      remEntry = lldpRemDataNeighborGetNext(intIfNum,prevEntry);
      break;
    }
    prevEntry = remEntry;
  }
  /* Check if there is a valid remote entry for this interface */
  if (remEntry == L7_NULLPTR)
  {
    *remIndex = 0;
    *timestamp = 0;
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  /* intIfNum, index, timestamp uniquely identify the remote entry,
     if entry is deleted or modified, these will not be valid */
  *remIndex = remEntry->remIndex;
  *timestamp = remEntry->timestamp;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the unique values to identify an interface remote entry
*
* @param    L7_uint32 timestamp @b((input))  remote data timestamp
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex  @b((input))  remote data index
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpRemTimestampIndexEntryGet(L7_uint32 timestamp,
                                      L7_uint32 intIfNum,
                                      L7_uint32 remIndex)
{
  lldpRemDataEntry_t *remEntry;

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);

  remEntry = lldpRemDataEntryTimestampIndexGet(timestamp,intIfNum,remIndex);
    
  /* Check if there is a valid remote entry for this interface */
  if (remEntry == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }
  osapiSemaGive(lldpSemaphore);
  
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the unique values to identify an interface remote entry
*
* @param    L7_uint32 timestamp @b((output))  remote data timestamp
* @param    L7_uint32 intIfNum   @b((output))   internal interface number
* @param    L7_uint32 remIndex  @b((output))  remote data index
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t lldpRemTimestampIndexEntryGetNext(L7_uint32 *timestamp,
                                          L7_uint32 *intIfNum,
                                          L7_uint32 *remIndex)
{
  L7_RC_t rc;
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  rc = lldpRemDataEntryTimestampIndexGetNext(timestamp,intIfNum,remIndex);
  osapiSemaGive(lldpSemaphore);
  return rc;
}

/*********************************************************************
*
* @purpose  Get the time to live for a remote entry
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 *ttl       @b((output)) Time to Live seconds
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpRemTTLGet(L7_uint32 intIfNum,
                      L7_uint32 remIndex,
                      L7_uint32 timestamp,
                      L7_uint32 *ttl)
{
  lldpRemDataEntry_t *remEntry;

  if (ttl == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *ttl = remEntry->rxTTL;

  osapiSemaGive(lldpSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the chassis subtype ID for a remote entry
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    lldpChassisIdSubtype_t subtype  @b((output))  remote chassis subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpRemChassisIdSubtypeGet(L7_uint32 intIfNum,
                                   L7_uint32 remIndex,
                                   L7_uint32 timestamp,
                                   lldpChassisIdSubtype_t *subtype)
{
  lldpRemDataEntry_t *remEntry;
  if (subtype == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *subtype = remEntry->chassisIdSubtype;

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the chassis ID for a remote entry
*
* @param    L7_uint32    intIfNum    @b((input))   internal interface number
* @param    L7_uint32    remIndex    @b((input))   remote data index
* @param    L7_uint32    timestamp   @b((input))   remote data timestamp
* @param    L7_uchar8    *chassisId  @b((output))  buffer to store chassis id
* @param    L7_ushort16  *length     @b((output))  length of chassis id
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    chassisId buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*           chassisId is encoded based on ChassisIdSubtype
*
* @end
*********************************************************************/
L7_RC_t lldpRemChassisIdGet(L7_uint32    intIfNum,
                            L7_uint32    remIndex,
                            L7_uint32    timestamp,
                            L7_uchar8   *chassisId,
                            L7_ushort16 *length)
{
  lldpRemDataEntry_t *remEntry;
  if (chassisId == L7_NULLPTR ||
      length == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  memcpy(chassisId, remEntry->chassisId, remEntry->chassisIdLength);
  *length = remEntry->chassisIdLength;

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the port id subtype for a remote entry
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex   @b((input))   remote data index
* @param    L7_uint32 timestamp  @b((input))   remote data timestamp
* @param    lldpPortIdSubtype_t *subtype @b((output))  remote port id subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpRemPortIdSubtypeGet(L7_uint32 intIfNum,
                                L7_uint32 remIndex,
                                L7_uint32 timestamp,
                                lldpPortIdSubtype_t *subtype)
{
  lldpRemDataEntry_t *remEntry;
  if (subtype == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *subtype = remEntry->portIdSubtype;

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the port ID for a remote entry
*
* @param    L7_uint32    intIfNum   @b((input))   internal interface number
* @param    L7_uint32    remIndex   @b((input))   remote data index
* @param    L7_uint32    timestamp  @b((input))   remote data timestamp
* @param    L7_uchar8    *portId    @b((output))  remote port id
* @param    L7_ushort16  *length    @b((output))  length of port id
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    portId buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*           portId buffer is encoded based on portIdSubtype
*
* @end
*********************************************************************/
L7_RC_t lldpRemPortIdGet(L7_uint32    intIfNum,
                         L7_uint32    remIndex,
                         L7_uint32    timestamp,
                         L7_uchar8   *portId,
                         L7_ushort16 *length)
{
  lldpRemDataEntry_t *remEntry;
  if (portId == L7_NULLPTR ||
      length == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  memcpy(portId, remEntry->portId, remEntry->portIdLength);
  *length = remEntry->portIdLength;

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the port description for a remote entry
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_char8  *portDesc  @b((output)) remote port description
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    portDesc buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemPortDescGet(L7_uint32 intIfNum,
                           L7_uint32 remIndex,
                           L7_uint32 timestamp,
                           L7_char8 *portDesc)
{
  lldpRemDataEntry_t *remEntry;
  if (portDesc == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  strcpy(portDesc, remEntry->portDesc);

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the system name for the remote entry
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_char8  *sysName   @b((output)) remote system name
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    sysName buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemSysNameGet(L7_uint32 intIfNum,
                          L7_uint32 remIndex,
                          L7_uint32 timestamp,
                          L7_char8 *sysName)
{
  lldpRemDataEntry_t *remEntry;
  if (sysName == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  strcpy(sysName, remEntry->sysName);

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the system description for the remote entry
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex   @b((input))   remote data index
* @param    L7_uint32 timestamp  @b((input))   remote data timestamp
* @param    L7_char8  *sysDesc   @b((output))  remote system description
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    sysDesc buffer must be at least LLDP_MGMT_STRING_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemSysDescGet(L7_uint32 intIfNum,
                          L7_uint32 remIndex,
                          L7_uint32 timestamp,
                          L7_char8 *sysDesc)
{
  lldpRemDataEntry_t *remEntry;
  if (sysDesc == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  strcpy(sysDesc, remEntry->sysDesc);

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the system capabilities supported for a remote entry
*
* @param    L7_uint32    intIfNum   @b((input))   internal interface number
* @param    L7_uint32    remIndex   @b((input))   remote data index
* @param    L7_uint32    timestamp  @b((input))   remote data timestamp
* @param    L7_ushort16  sysCap  @b((output))  bit mask of lldpSysCapMask_t
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemSysCapSupportedGet(L7_uint32    intIfNum,
                                  L7_uint32    remIndex,
                                  L7_uint32    timestamp,
                                  L7_ushort16 *sysCap)
{
  lldpRemDataEntry_t *remEntry;
  if (sysCap == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *sysCap = remEntry->sysCapSupported;

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the system capabilities enabled for a remote entry
*
* @param    L7_uint32    intIfNum   @b((input))  internal interface number
* @param    L7_uint32    remIndex   @b((input))  remote data index
* @param    L7_uint32    timestamp  @b((input))  remote data timestamp
* @param    L7_ushort16  sysCap     @b((output)) bit mask of lldpSysCapMask_t
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemSysCapEnabledGet(L7_uint32    intIfNum,
                                L7_uint32    remIndex,
                                L7_uint32    timestamp,
                                L7_ushort16 *sysCap)
{
  lldpRemDataEntry_t *remEntry;
  if (sysCap == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  *sysCap = remEntry->sysCapEnabled;

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the next valid management address entry for the
*           indicated remote data entry.
*
* @param    L7_uint32                   intIfNum   @b((input))  internal interface number
* @param    L7_uint32                   remIndex   @b((input))  remote data index
* @param    L7_uint32                   timestamp  @b((input))  remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t *family     @b((input/output))   IANA address family
* @param    L7_uchar8                  *addr       @b((input/output))   remote mgmt address
* @param    L7_uchar8                  *length     @b((input/output))   address length
*
* @returns  L7_SUCCESS, entry found, addrIndex is valid
* @returns  L7_FAILURE, no more entries
*
* @notes    if *length == 0, this will return first entry.
*           addr buffer must be at least LLDP_MGMT_ADDR_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemManAddrEntryNextGet(L7_uint32                    intIfNum,
                                   L7_uint32                    remIndex,
                                   L7_uint32                    timestamp,
                                   lldpIANAAddrFamilyNumber_t  *family,
                                   L7_uchar8                   *addr,
                                   L7_uchar8                   *length)
{
  lldpRemDataEntry_t *remEntry;
  lldpMgmtAddrEntry_t *entry = L7_NULLPTR;

  if ((family == L7_NULLPTR) ||
      (addr == L7_NULLPTR) ||
      (length == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->mgmtAddrList;
  if (((*length) == 0) &&
      (entry != L7_NULLPTR))
  {
    *family = entry->family;
    memcpy(addr, entry->address, entry->length);
    *length = entry->length;
    osapiSemaGive(lldpSemaphore);
    return L7_SUCCESS;
  }

  while (entry != L7_NULLPTR)
  {
    if ((entry->family == (*family)) &&
        (entry->length == (*length)) &&
        (memcmp(entry->address, addr, (*length)) == 0))
    {
      if (entry->next != L7_NULLPTR)
      {
        *family = entry->next->family;
        memcpy(addr, entry->next->address, entry->next->length);
        *length = entry->next->length;
        osapiSemaGive(lldpSemaphore);
        return L7_SUCCESS;
      } else
      {
        break;
      }
    }
    entry = entry->next;
  }

  *length = 0;
  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Determine if a remote management address entry exists
*
* @param    L7_uint32                  intIfNum  @b((input))  internal interface number
* @param    L7_uint32                  remIndex  @b((input))  remote data index
* @param    L7_uint32                  timestamp @b((input))  remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t family    @b((input))   IANA address family
* @param    L7_uchar8                  *addr     @b((input))   remote mgmt address
* @param    L7_uchar8                  length    @b((input))   address length
*
* @returns  L7_SUCCESS, entry exists
* @returns  L7_FAILURE, entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemManAddrEntryGet(L7_uint32                    intIfNum,
                               L7_uint32                    remIndex,
                               L7_uint32                    timestamp,
                               lldpIANAAddrFamilyNumber_t   family,
                               L7_uchar8                   *addr,
                               L7_uchar8                    length)
{
  lldpRemDataEntry_t *remEntry;
  lldpMgmtAddrEntry_t *entry = L7_NULLPTR;

  if ((addr == L7_NULLPTR) ||
      (length == 0) ||
      (length > LLDP_MGMT_ADDR_SIZE_MAX))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->mgmtAddrList;
  while (entry != L7_NULLPTR)
  {
    if ((entry->family == family) &&
        (entry->length == length) &&
        (memcmp(entry->address, addr, length) == 0))
    {
      osapiSemaGive(lldpSemaphore);
      return L7_SUCCESS;
    }
    entry = entry->next;
  }

  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get interface subtype for a remote management address entry
*
* @param    L7_uint32                  intIfNum   @b((input))  internal interface number
* @param    L7_uint32                  remIndex   @b((input))  remote data index
* @param    L7_uint32                  timestamp  @b((input))  remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t family     @b((input))   IANA address family
* @param    L7_uchar8                  *addr      @b((input))   remote mgmt address
* @param    L7_uchar8                  length     @b((input))   address length
* @param    lldpManAddrIfSubtype_t     *subtype   @b((output))  interface subtype
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemManAddrIfSubtypeGet(L7_uint32                    intIfNum,
                                   L7_uint32                    remIndex,
                                   L7_uint32                    timestamp,
                                   lldpIANAAddrFamilyNumber_t   family,
                                   L7_uchar8                   *addr,
                                   L7_uchar8                    length,
                                   lldpManAddrIfSubtype_t      *subtype)
{
  lldpRemDataEntry_t *remEntry;
  lldpMgmtAddrEntry_t *entry = L7_NULLPTR;

  if ((addr == L7_NULLPTR) ||
      (length == 0) ||
      (length > LLDP_MGMT_ADDR_SIZE_MAX) ||
      (subtype == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->mgmtAddrList;
  while (entry != L7_NULLPTR)
  {
    if ((entry->family == family) &&
        (entry->length == length) &&
        (memcmp(entry->address, addr, length) == 0))
    {
      *subtype = entry->ifSubtype;
      osapiSemaGive(lldpSemaphore);
      return L7_SUCCESS;
    }
    entry = entry->next;
  }

  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get interface id for a remote management address entry
*
* @param    L7_uint32                  intIfNum   @b((input))  internal interface number
* @param    L7_uint32                  remIndex   @b((input))  remote data index
* @param    L7_uint32                  timestamp  @b((input))  remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t family     @b((input))   IANA address family
* @param    L7_uchar8                  *addr      @b((input))   remote mgmt address
* @param    L7_uchar8                  length     @b((input))   address length
* @param    L7_uint32                  *id        @b((output)) interface id
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemManAddrIfIdGet(L7_uint32                    intIfNum,
                              L7_uint32                    remIndex,
                              L7_uint32                    timestamp,
                              lldpIANAAddrFamilyNumber_t   family,
                              L7_uchar8                   *addr,
                              L7_uchar8                    length,
                              L7_uint32                   *id)
{
  lldpRemDataEntry_t *remEntry;
  lldpMgmtAddrEntry_t *entry = L7_NULLPTR;

  if ((addr == L7_NULLPTR) ||
      (length == 0) ||
      (length > LLDP_MGMT_ADDR_SIZE_MAX) ||
      (id == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->mgmtAddrList;
  while (entry != L7_NULLPTR)
  {
    if ((entry->family == family) &&
        (entry->length == length) &&
        (memcmp(entry->address, addr, length) == 0))
    {
      *id = entry->ifId;
      osapiSemaGive(lldpSemaphore);
      return L7_SUCCESS;
    }
    entry = entry->next;
  }

  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get OID for the remote management address entry
*
* @param    L7_uint32                  intIfNum   @b((input))   internal interface number
* @param    L7_uint32                  remIndex   @b((input))   remote data index
* @param    L7_uint32                  timestamp  @b((input))   remote data timestamp
* @param    lldpIANAAddrFamilyNumber_t family     @b((input))   IANA address family
* @param    L7_uchar8                  *addr      @b((input))   remote mgmt address
* @param    L7_uchar8                  length     @b((input))   address length
* @param    L7_char8                   *oid       @b((output))  management address OID
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    oid buffer must be at least LLDP_MGMT_ADDR_OID_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemManAddrOIDGet(L7_uint32                    intIfNum,
                             L7_uint32                    remIndex,
                             L7_uint32                    timestamp,
                             lldpIANAAddrFamilyNumber_t   family,
                             L7_uchar8                   *addr,
                             L7_uchar8                    length,
                             L7_char8                    *oid)
{
  lldpRemDataEntry_t *remEntry;
  lldpMgmtAddrEntry_t *entry = L7_NULLPTR;

  if ((addr == L7_NULLPTR) ||
      (length == 0) ||
      (length > LLDP_MGMT_ADDR_SIZE_MAX) ||
      (oid == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->mgmtAddrList;
  while (entry != L7_NULLPTR)
  {
    if ((entry->family == family) &&
        (entry->length == length) &&
        (memcmp(entry->address, addr, length) == 0))
    {
      strcpy(oid, entry->oid);
      osapiSemaGive(lldpSemaphore);
      return L7_SUCCESS;
    }
    entry = entry->next;
  }

  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the next unknown TLV entry for the indicated remote data entry.
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 *tlvType   @b((input/output)) tlv type
*
* @returns  L7_SUCCESS, next entry found, tlvType is valid
* @returns  L7_FAILURE, no more entries
*
* @notes    if *tlvType == 0, this will return first entry.
*
* @end
*********************************************************************/
L7_RC_t lldpRemUnknownTLVEntryNextGet(L7_uint32  intIfNum,
                                      L7_uint32  remIndex,
                                      L7_uint32  timestamp,
                                      L7_uint32 *tlvType)
{
  lldpRemDataEntry_t *remEntry;
  lldpUnknownTLVEntry_t *entry = L7_NULLPTR;

  if (tlvType == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->unknownTLVList;
  if (((*tlvType) == 0) &&
      (entry != L7_NULLPTR))
  {
    *tlvType = entry->type;
    osapiSemaGive(lldpSemaphore);
    return L7_SUCCESS;
  }
  while (entry != L7_NULLPTR)
  {
    if (entry->type == (*tlvType))
    {
      if (entry->next != L7_NULLPTR)
      {
        *tlvType = entry->next->type;
        osapiSemaGive(lldpSemaphore);
        return L7_SUCCESS;
      } else
      {
        break;
      }
    }
    entry = entry->next;
  }

  *tlvType = 0;
  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Determine if unknown TLV entry exists.
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 tlvType    @b((input))  tlv type
*
* @returns  L7_SUCCESS, entry valid
* @returns  L7_FAILURE, entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemUnknownTLVEntryGet(L7_uint32  intIfNum,
                                  L7_uint32  remIndex,
                                  L7_uint32  timestamp,
                                  L7_uint32  tlvType)
{
  lldpRemDataEntry_t *remEntry;
  lldpUnknownTLVEntry_t *entry = L7_NULLPTR;

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->unknownTLVList;
  while (entry != L7_NULLPTR)
  {
    if (entry->type == tlvType)
    {
      osapiSemaGive(lldpSemaphore);
      return L7_SUCCESS;
    }
    entry = entry->next;
  }

  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the TLV info for the specified remote entry
*
* @param    L7_uint32   intIfNum   @b((input))  internal interface number
* @param    L7_uint32   remIndex   @b((input))  remote data index
* @param    L7_uint32   timestamp  @b((input))  remote data timestamp
* @param    L7_uint32   tlvType    @b((input))  tlv type
* @param    L7_uchar8   *info      @b((output))  organization defined info
* @param    L7_ushort16 *length    @b((output))  length of info field
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    tlvInfo buffer must be at least LLDP_UNKNOWN_TLV_INFO_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemUnknownTLVInfoGet(L7_uint32    intIfNum,
                                 L7_uint32    remIndex,
                                 L7_uint32    timestamp,
                                 L7_uint32    tlvType,
                                 L7_uchar8   *info,
                                 L7_ushort16 *length)
{
  lldpRemDataEntry_t *remEntry;
  lldpUnknownTLVEntry_t *entry = L7_NULLPTR;

  if ((info == L7_NULLPTR) ||
      (length == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->unknownTLVList;
  while (entry != L7_NULLPTR)
  {
    if (entry->type == tlvType)
    {
      memcpy(info, entry->info, entry->length);
      *length = entry->length;
      osapiSemaGive(lldpSemaphore);
      return L7_SUCCESS;
    }
    entry = entry->next;
  }

  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the next organizationally defined info entry for
*           the indicated remote data entry.
*
* @param    L7_uint32 intIfNum   @b((input))  internal interface number
* @param    L7_uint32 remIndex   @b((input))  remote data index
* @param    L7_uint32 timestamp  @b((input))  remote data timestamp
* @param    L7_uint32 *infoIndex @b((output)) info index
*
* @returns  L7_SUCCESS, entry found, tlvIndex is valid
* @returns  L7_FAILURE, no more entries
*
* @notes    if *infoIndex = 0, this will return first index.
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefInfoEntryNextGet(L7_uint32  intIfNum,
                                      L7_uint32  remIndex,
                                      L7_uint32  timestamp,
                                      L7_uint32 *infoIndex)
{
  lldpRemDataEntry_t *remEntry;
  L7_uint32 i = 0;
  lldpOrgDefInfoEntry_t *entry = L7_NULLPTR;

  if (infoIndex == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  /* this is a linked list, "indexes" are 1 based */
  entry = remEntry->orgDefInfoList;
  while (entry != L7_NULLPTR)
  {
    if (++i == (*infoIndex)+1)
    {
      *infoIndex = i;
      osapiSemaGive(lldpSemaphore);
      return L7_SUCCESS;
    }
    entry = entry->next;
  }

  *infoIndex = 0;
  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Determine if an organizationally defined info entry exists.
*
* @param    L7_uint32   intIfNum    @b((input))  internal interface number
* @param    L7_uint32   remIndex    @b((input))  remote data index
* @param    L7_uint32   timestamp   @b((input))  remote data timestamp
* @param    L7_uint32   infoIndex   @b((input))  info index
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE, if entry is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefInfoEntryGet(L7_uint32    intIfNum,
                                  L7_uint32    remIndex,
                                  L7_uint32    timestamp,
                                  L7_uint32    infoIndex)
{
  L7_uint32 i = 0;
  lldpRemDataEntry_t *remEntry;
  lldpOrgDefInfoEntry_t *entry = L7_NULLPTR;

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->orgDefInfoList;
  while (entry != L7_NULLPTR)
  {
    if (++i == infoIndex)
    {
      osapiSemaGive(lldpSemaphore);
      return L7_SUCCESS;
    }
    entry = entry->next;
  }

  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the organizationally defined info for specified entry.
*
* @param    L7_uint32   intIfNum    @b((input))  internal interface number
* @param    L7_uint32   remIndex    @b((input))  remote data index
* @param    L7_uint32   timestamp   @b((input))  remote data timestamp
* @param    L7_uint32   infoIndex   @b((input))  info index
* @param    L7_uchar8   *info       @b((output))  organization defined info
* @param    L7_ushort16 *length     @b((output))  length of info field
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    orgDefInfo buffer must be at least LLDP_ORG_DEF_INFO_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefInfoGet(L7_uint32    intIfNum,
                             L7_uint32    remIndex,
                             L7_uint32    timestamp,
                             L7_uint32    infoIndex,
                             L7_uchar8   *info,
                             L7_ushort16 *length)
{
  L7_uint32 i = 0;
  lldpRemDataEntry_t *remEntry;
  lldpOrgDefInfoEntry_t *entry = L7_NULLPTR;

  if (info == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->orgDefInfoList;
  while (entry != L7_NULLPTR)
  {
    if (++i == infoIndex)
    {
      memcpy(info, entry->info, entry->length);
      *length = entry->length;
      osapiSemaGive(lldpSemaphore);
      return L7_SUCCESS;
    }
    entry = entry->next;
  }

  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}
/*********************************************************************
*
* @purpose  Get the organizationally defined OUI for specified entry.
*
* @param    L7_uint32   intIfNum    @b((input))  internal interface number
* @param    L7_uint32   remIndex    @b((input))  remote data index
* @param    L7_uint32   timestamp   @b((input))  remote data timestamp
* @param    L7_uint32   infoIndex   @b((input))  info index
* @param    L7_uchar8   *oui       @b((output))  organization defined info
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    oui buffer must be at least LLDP_ORG_DEF_INFO_OUI_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefOUIGet(L7_uint32    intIfNum,
                             L7_uint32    remIndex,
                             L7_uint32    timestamp,
                             L7_uint32    infoIndex,
                             L7_uchar8   *oui)
{
  L7_uint32 i = 0;
  lldpRemDataEntry_t *remEntry;
  lldpOrgDefInfoEntry_t *entry = L7_NULLPTR;

  if (oui == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->orgDefInfoList;
  while (entry != L7_NULLPTR)
  {
    if (++i == infoIndex)
    {
      memcpy(oui, entry->oui, LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1);
      osapiSemaGive(lldpSemaphore);
      return L7_SUCCESS;
    }
    entry = entry->next;
  }

  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}
/*********************************************************************
*
* @purpose  Get the organizationally defined subtype for specified entry.
*
* @param    L7_uint32   intIfNum    @b((input))  internal interface number
* @param    L7_uint32   remIndex    @b((input))  remote data index
* @param    L7_uint32   timestamp   @b((input))  remote data timestamp
* @param    L7_uint32   infoIndex   @b((input))  info index
* @param    L7_uchar8   *subtype    @b((output))  organization defined info
*
* @returns  L7_SUCCESS, if value was retrieved successfully
* @returns  L7_FAILURE, if entry is invalid
*
* @notes    orgDefInfo buffer must be at least LLDP_ORG_DEF_INFO_SIZE_MAX
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefSubtypeGet(L7_uint32    intIfNum,
                             L7_uint32    remIndex,
                             L7_uint32    timestamp,
                             L7_uint32    infoIndex,
                             L7_uint32   *subtype)
{
  L7_uint32 i = 0;
  lldpRemDataEntry_t *remEntry;
  lldpOrgDefInfoEntry_t *entry = L7_NULLPTR;

  if (subtype == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->orgDefInfoList;
  while (entry != L7_NULLPTR)
  {
    if (++i == infoIndex)
    {
      *subtype = (L7_uint32)entry->subtype;
      osapiSemaGive(lldpSemaphore);
      return L7_SUCCESS;
    }
    entry = entry->next;
  }

  osapiSemaGive(lldpSemaphore);
  return L7_FAILURE;
}
/*********************************************************************
*
* @purpose  Get the Next voip device address on an interface 
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    This is call that should be called within semaphore unless 
*           it is called in the lldp's context.
*
* @end
*********************************************************************/
L7_RC_t lldpRemoteDeviceGetNext(lldpRemDeviceInfoParms_t *deviceInfo)
{
  lldpRemDataEntry_t *remEntry = L7_NULLPTR,*prevEntry=L7_NULLPTR;
  lldpMedCapTLV_t info;
  L7_uint32 intIfNum,remIndex;

  intIfNum = deviceInfo->intIfNum;
  remIndex = deviceInfo->remIndex;

  if(remIndex != 0)
  {
    while((remEntry = lldpRemDataNeighborGetNext(intIfNum,prevEntry))!=L7_NULLPTR)
    {
      if(remEntry->remIndex == remIndex)
      {
        prevEntry = remEntry;
        break;
      }
      prevEntry = remEntry;
    }
  }

  /* Check if there is a valid remote entry for this interface */
  if (remEntry != L7_NULLPTR || remIndex == 0)
  {
    while((remEntry = lldpRemDataNeighborGetNext(intIfNum,prevEntry))!=L7_NULLPTR)
    {
      memset(&info,  0x00,  sizeof(lldpMedCapTLV_t));
      if (lldpOrgDefTLVGet(remEntry,
                       (L7_uchar8*)LLDP_MED_TIA_OUI_STRING, 
                       LLDP_MED_SUBTYPE_CAP, 
                       (L7_uchar8*)&info, 
                       sizeof(lldpMedCapTLV_t)) == L7_SUCCESS)
      {
        if ((L7_uint32)info.deviceType[0] == endpointClass3)
        {
          /* found a voip device, give back the mac info */
          deviceInfo->remIndex = remEntry->remIndex;
          memcpy(deviceInfo->data.macAddr, remEntry->srcMac, L7_MAC_ADDR_LEN);
          return L7_SUCCESS;
        }
      }
      prevEntry = remEntry;
    }
  }
  deviceInfo->remIndex = 0;
  return L7_FAILURE;
}
/*********************************************************************
*
* @purpose  Get Info associated with this port with a specific OUI and Subtype 
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex   @b((input))   Remote Index
* @param    L7_uint32 timestamp  @b((input))   Timestamp for this entry
* @param    L7_uint32 infoIndex  @b((input))   InfoIndex for this entry
* @param    L7_uchar8     *oui   @b((input))   Pointer to the OUI
* @param    L7_uint32 *subtype   @b((input))   Pointer to the Subtype
* @param    L7_uchar8    *info   @b((output))   Pointer To the Info
* @param    L7_uint32 *length    @b((output))   Pointer To the Length
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    If incoming OUI and subtype is zero then this routine gets the 
*           numerically smallest OUI/Subtype combination.  
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefEntryInfoGet(L7_uint32    intIfNum,
                                  L7_uint32    remIndex,
                                  L7_uint32    timestamp,
                                  L7_uint32    infoIndex,
                                  L7_uchar8     *oui,
                                  L7_uint32    *subtype,
                                  L7_uchar8    *info,
                                  L7_uint32  *length)
{
  lldpRemDataEntry_t *remEntry;
  lldpOrgDefInfoEntry_t *entry = L7_NULLPTR;
  lldpOrgDefInfoEntry_t firstEntry;
  L7_BOOL getFirst = L7_FALSE;
  L7_BOOL exactMatchFound = L7_FALSE;

  if (info == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  memset(&firstEntry, 0x00, sizeof(lldpOrgDefInfoEntry_t));
  if (memcmp(firstEntry.oui, oui, LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1)== 0 && *subtype == 0)
  {
    getFirst = L7_TRUE;
  }
  if (infoIndex != 1)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  entry = remEntry->orgDefInfoList;
  if (entry!=L7_NULLPTR)
  {
    memcpy(&firstEntry,entry, sizeof(lldpOrgDefInfoEntry_t));
  }

  while (entry != L7_NULLPTR)
  {
    if (memcmp((void*)entry->oui, (void*)firstEntry.oui, LLDP_ORG_DEF_INFO_OUI_SIZE_MAX) < 0)
    {
      memcpy(&firstEntry,entry, sizeof(lldpOrgDefInfoEntry_t));
    }
    entry = entry->next;
  }
  entry = remEntry->orgDefInfoList;
  if (entry != L7_NULLPTR)
  {
    while (entry != L7_NULLPTR)
    {
      if (getFirst == L7_TRUE)
      {
        if (memcmp(entry->oui,firstEntry.oui,LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1)==0 && (L7_uint32)entry->subtype<(L7_uint32)firstEntry.subtype)
        {
          memcpy(&firstEntry,entry, sizeof(lldpOrgDefInfoEntry_t));
        }
      }
      else
      {
        /*Get the exact match*/
        if (memcmp(oui, entry->oui,LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1)==0 && *subtype == (L7_uint32)entry->subtype)
        {
          memcpy(&firstEntry,entry, sizeof(lldpOrgDefInfoEntry_t));
          exactMatchFound = L7_TRUE;
        }
      }
      entry = entry->next;
    }
  }
  if (getFirst == L7_FALSE && exactMatchFound == L7_FALSE)
  {
    /*Exact mat was requested but not found*/
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }
  memcpy(oui,firstEntry.oui,LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1);
  memcpy(info, firstEntry.info, firstEntry.length);
  *length = (L7_uint32)firstEntry.length;
  *subtype = (L7_uint32)firstEntry.subtype;
  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Get next Info associated with this port given a specific OUI and Subtype 
*
* @param    L7_uint32 intIfNum   @b((input))   internal interface number
* @param    L7_uint32 remIndex   @b((input))   Remote Index
* @param    L7_uint32 timestamp  @b((input))   Timestamp for this entry
* @param    L7_uint32 *infoIndex  @b((input))  Pointer to InfoIndex for this entry
* @param    L7_uchar8     *oui   @b((input))   Pointer to the OUI
* @param    L7_uint32 *subtype   @b((input))   Pointer to the Subtype
* @param    L7_uchar8    *info   @b((output))   Pointer To the Info
* @param    L7_uint32 *length    @b((output))   Pointer To the Length
*
* @returns  L7_SUCCESS, if entry was retrieved successfully
* @returns  L7_FAILURE, if interface out of range or no entry exists
*
* @notes    Starting with the incoming OUI and Subtype this routine returns the next 
*           numerically largest OUI/Subtype combination.  
*
* @end
*********************************************************************/
L7_RC_t lldpRemOrgDefEntryInfoGetNext(L7_uint32    intIfNum,
                                      L7_uint32    remIndex,
                                      L7_uint32    timestamp,
                                      L7_uint32    *infoIndex,
                                      L7_uchar8     *oui,
                                      L7_uint32    *subtype,
                                      L7_uchar8   *info,
                                      L7_uint32 *length)
{
  lldpRemDataEntry_t *remEntry;
  lldpOrgDefInfoEntry_t *entry = L7_NULLPTR;
  lldpOrgDefInfoEntry_t nextEntry;
  L7_RC_t rc = L7_SUCCESS;
  L7_int32 compare = 0, currenttype = 0;
  L7_BOOL nextPresent = L7_FALSE;
  
  if (info == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *infoIndex = 1;

  memset(&nextEntry, 0x00, sizeof(lldpOrgDefInfoEntry_t));
  if (memcmp(nextEntry.oui, oui, LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1)== 0 && *subtype == 0)
  {
    rc = lldpRemOrgDefEntryInfoGet(intIfNum,remIndex,timestamp,*infoIndex,oui,subtype,info,length);
    return rc;
  }

  osapiSemaTake(lldpSemaphore, L7_WAIT_FOREVER);
  if ((remEntry = lldpIsRemEntryValid(intIfNum, remIndex, timestamp)) == L7_NULLPTR)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  memcpy(nextEntry.oui,oui,LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1);
  nextEntry.subtype = (L7_uchar8)*subtype;
  currenttype  = 255; /*Set this to the max possible subtype, which is max number stored in a char*/

  entry = remEntry->orgDefInfoList;

  while (entry != L7_NULLPTR)
  {
    compare = memcmp(entry->oui, nextEntry.oui, LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1);
    if (compare == 0 && nextEntry.subtype == entry->subtype)
    {
      /*Same entry*/
      entry = entry->next;
      continue;
    }
    else if (compare == 0 && entry->subtype > nextEntry.subtype && (entry->subtype - nextEntry.subtype) < currenttype)
    {
      /*We found a numerical larger entry so return*/
      /*
      memcpy(oui,entry->oui,LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1);
      memcpy(info, entry->info, entry->length);
      *length = entry->length;
      *subtype = (L7_uint32)entry->subtype;
      osapiSemaGive(lldpSemaphore);
      return L7_SUCCESS;*/
      currenttype = entry->subtype - nextEntry.subtype;
      
    }
    entry = entry->next;
  }
  if (currenttype != 255)
  {
    entry = remEntry->orgDefInfoList;
    while (entry != L7_NULLPTR)
    {
      compare = memcmp(entry->oui, nextEntry.oui, LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1);
      if (compare == 0 && entry->subtype == (nextEntry.subtype+ currenttype) )
      {
        memcpy(oui,entry->oui,LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1);
        memcpy(info, entry->info, entry->length);
        *length = (L7_uint32)entry->length;
        *subtype = (L7_uint32)entry->subtype;
        osapiSemaGive(lldpSemaphore);
        return L7_SUCCESS;
      }
      entry = entry->next;
    }
  }


  /*If we get here that means , with the same oui we did not find a larger subtype in the list*/
  /*So now get the next larger oui*/
  entry = remEntry->orgDefInfoList;
  if (entry != L7_NULLPTR)
  {
    while (entry != L7_NULLPTR)
    {
      compare = memcmp(entry->oui,nextEntry.oui,LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1);
      if (compare > 0)
      {
        memcpy(&nextEntry, entry, sizeof(lldpOrgDefInfoEntry_t));
        nextPresent = L7_TRUE;
        break;
      }
      entry = entry->next;
    }
  }

  if (nextPresent == L7_FALSE)
  {
    osapiSemaGive(lldpSemaphore);
    return L7_FAILURE;
  }

  /*If we get here that means a higher oui exists and now we need to find the smallest subtype in that higher oui*/
  entry = remEntry->orgDefInfoList;
  if (entry != L7_NULLPTR)
  {
    while (entry != L7_NULLPTR)
    {
      compare = memcmp(entry->oui,nextEntry.oui,LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1);
      if (compare == 0 && nextEntry.subtype > entry->subtype)
      {
        /*We found a smaller subtype */
        memcpy(&nextEntry, entry, sizeof(lldpOrgDefInfoEntry_t));
      }
      entry = entry->next;
    }
  }

  memcpy(oui,nextEntry.oui,LLDP_ORG_DEF_INFO_OUI_SIZE_MAX-1);
  memcpy(info, nextEntry.info, nextEntry.length);
  *length = (L7_uint32)nextEntry.length;
  *subtype = (L7_uint32)nextEntry.subtype;

  osapiSemaGive(lldpSemaphore);
  return L7_SUCCESS;
}

void lldpRemIntfOrgDefDump(L7_uint32 intIfNum)
{
  lldpRemDataEntry_t *remEntry,*prevEntry=L7_NULLPTR;
  L7_uint32 subtype=0;
  L7_uint32 infoindex = 0;
  L7_uint32 length = 0;
  
  printf("\n");
  printf("802.1AB Remote Data for interface %d\n", intIfNum);
  printf("=====================\n\n");

  while((remEntry=lldpRemDataNeighborGetNext(intIfNum,prevEntry))!=L7_NULLPTR)
  {
    lldpOrgDefInfoEntry_t orgDefEntry;
    L7_uint32 i;
    memset(&orgDefEntry, 0x00, sizeof(lldpOrgDefInfoEntry_t));
    while( lldpRemOrgDefEntryInfoGetNext(intIfNum,
                                         remEntry->remIndex, 
                                         remEntry->timestamp,
                                         &infoindex,
                                         orgDefEntry.oui,
                                         &subtype,
                                         orgDefEntry.info,
                                         &length) == L7_SUCCESS)
    {
      orgDefEntry.subtype = (L7_uchar8)subtype;
      orgDefEntry.length = (L7_ushort16)length;
      printf("Org Def OUI 0x%2x %2x %2x\n", orgDefEntry.oui[0],orgDefEntry.oui[1],orgDefEntry.oui[2]);
      printf("Subtype: 0x%2x\n",orgDefEntry.subtype);
      printf("Info Length is %d\n", orgDefEntry.length);
      printf("Info is:"); 
      for (i = 0; i < orgDefEntry.length; i++)
      {
        printf(" %2x", orgDefEntry.info[i]);
      }
      printf("\n");

    }
    prevEntry = remEntry;
  }

}
