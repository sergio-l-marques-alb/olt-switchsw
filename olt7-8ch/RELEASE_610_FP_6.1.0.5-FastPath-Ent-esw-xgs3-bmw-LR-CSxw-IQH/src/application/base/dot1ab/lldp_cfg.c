/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename lldp_cfg.c
*
* @purpose 802.1AB configuration functions
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
#include "nvstoreapi.h"
#include "nimapi.h"
#include "sysapi.h"
#include "l7_product.h"
#include "registry.h"
#include "log.h"
#include "defaultconfig.h"
#include "lldp_api.h"
#include "lldp.h"
#include "lldp_util.h"
#include "platform_config.h"

lldpCfgData_t         *lldpCfgData     = L7_NULLPTR;
L7_uint32             *lldpMapTbl      = L7_NULLPTR;
void                  *lldpQueue       = L7_NULLPTR;
lldpStats_t           *lldpStats       = L7_NULLPTR;
lldpIntfOprData_t     *lldpIntfTbl     = L7_NULLPTR;
L7_uchar8             *lldpPDUBuf      = L7_NULLPTR;
void                  *lldpSemaphore   = L7_NULLPTR;

L7_uint32              lldpMgmtAddrPoolId = 0;
L7_uint32              lldpUnknownTLVPoolId = 0;
L7_uint32              lldpOrgDefInfoPoolId = 0;

extern lldpCnfgrState_t  lldpCnfgrState;
extern void             *lldpSemaphore;


/*********************************************************************
* @purpose  Saves 802.1AB file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpSave(void)
{
  if (lldpCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    lldpCfgData->cfgHdr.dataChanged = L7_FALSE;

    lldpCfgData->checkSum = nvStoreCrc32((L7_char8 *)lldpCfgData,
                                         (sizeof (lldpCfgData_t) - sizeof (lldpCfgData->checkSum)));

    if (sysapiCfgFileWrite(L7_LLDP_COMPONENT_ID, LLDP_CFG_FILENAME,
                           (L7_char8 *)lldpCfgData, sizeof(lldpCfgData_t)) != L7_SUCCESS)
    {
      LOG_MSG("lldpSave: Error on call to sysapiCfgFileWrite file %s\n", LLDP_CFG_FILENAME);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if 802.1AB user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL lldpHasDataChanged(void)
{
  return lldpCfgData->cfgHdr.dataChanged;
}
void lldpResetDataChanged(void)
{
  lldpCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Apply 802.1AB Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpApplyConfigData(void)
{
  L7_uint32     index;
  L7_uint32     intIfNum;
  nimConfigID_t cfgIdNull;

  memset(&cfgIdNull, 0, sizeof(nimConfigID_t));

  /* Check for interfaces that have been enabled for LLDP. All non-zero
     values in the cfg structure are configured and need to be applied. */
  if (LLDP_IS_READY)
  {
    for (index = 1; index < L7_LLDP_INTF_MAX_COUNT; index++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&lldpCfgData->intfCfgData[index].cfgId, &cfgIdNull))
      {
        continue;
      }
      if (nimIntIfFromConfigIDGet(&(lldpCfgData->intfCfgData[index].cfgId),
                                  &intIfNum) != L7_SUCCESS)
      {
        continue;
      }
      lldpIntfConfigDataApply(index);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default 802.1AB Interface config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void lldpBuildDefaultIntfConfigData(nimConfigID_t *cfgId,
                                    lldpIntfCfgData_t *pCfg)
{
  NIM_CONFIG_ID_COPY(&pCfg->cfgId, cfgId);
  pCfg->txEnabled = FD_LLDP_PORT_TX_ADMIN_MODE;
  pCfg->rxEnabled = FD_LLDP_PORT_RX_ADMIN_MODE;
  pCfg->optionalTLVsEnabled = FD_LLDP_PORT_TRANSMIT_TLVS_ENABLE;
  pCfg->mgmtAddrTxEnabled = FD_LLDP_PORT_TRANSMIT_MGMT_ADDR_ENABLE;
  pCfg->notificationEnabled = FD_LLDP_PORT_NOTIFICATION_ADMIN_MODE;
  /* MED Default Config Data */
  pCfg->medEnabled = FD_LLDP_MED_PORT_ADMIN_MODE;
  pCfg->medNotifyEnabled = FD_LLDP_MED_PORT_NOTIFY_MODE;
  pCfg->medTLVsSupported.bitmap[0] = 0x00;
  pCfg->medTLVsSupported.bitmap[1] = (L7_uchar8)(FD_LLDP_MED_PORT_TVL_TX);
  /* Rest of the bits, if any are to be set, will be set on L7_CREATE of the port */
  pCfg->medTLVsEnabled.bitmap[0] = 0x00;
  pCfg->medTLVsEnabled.bitmap[1] = (L7_uchar8)(FD_LLDP_MED_PORT_TVL_TX);
  return;
}

/*********************************************************************
* @purpose  Build default 802.1AB config data
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes    default is NO Multicast MAC entries
*
* @end
*********************************************************************/
void lldpBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 index;
  nimConfigID_t cfgId[L7_LLDP_INTF_MAX_COUNT];
  
  /* save the config id's */
  memset(&cfgId[0], 0, sizeof(nimConfigID_t) * L7_LLDP_INTF_MAX_COUNT);

  for (index = 1; index < L7_LLDP_INTF_MAX_COUNT; index++)
  {
    NIM_CONFIG_ID_COPY(&cfgId[index],
                       &lldpCfgData->intfCfgData[index].cfgId);
  }

  memset((void *)lldpCfgData, 0, sizeof(lldpCfgData_t));

  for (index = 1; index < L7_LLDP_INTF_MAX_COUNT; index++)
  {
    lldpBuildDefaultIntfConfigData(&cfgId[index],
                                   &lldpCfgData->intfCfgData[index]);
  }

  strcpy(lldpCfgData->cfgHdr.filename, LLDP_CFG_FILENAME);

  lldpCfgData->cfgHdr.version = ver;
  lldpCfgData->cfgHdr.componentID = L7_LLDP_COMPONENT_ID;
  lldpCfgData->cfgHdr.type = L7_CFG_DATA;
  lldpCfgData->cfgHdr.length = sizeof(lldpCfgData_t);
  lldpCfgData->cfgHdr.dataChanged = L7_FALSE;

  lldpCfgData->notifyInterval = FD_LLDP_NOTIFICATION_INTERVAL;
  lldpCfgData->reinitDelay = FD_LLDP_REINIT_DELAY;
  lldpCfgData->txHoldMultiplier = FD_LLDP_TX_HOLD_MULTIPLIER;
  lldpCfgData->txInterval = FD_LLDP_TX_INTERVAL;
  lldpCfgData->txDelay= FD_LLDP_TX_DELAY;
  /* MED Default Config Data */
  lldpCfgData->fastStartRepeatCount = FD_LLDP_MED_FASTSTART_REPEAT_COUNT;
  lldpCfgData->localDeviceClass = FD_LLDP_MED_LOCAL_DEVICE_CLASS;

  return;
}
