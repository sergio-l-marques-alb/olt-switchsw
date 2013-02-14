/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip.h
*
* @purpose VOIP definitions
*
* @component VOIP
*
* @comments none
*
* @create 05/10/2007
*
* @author parora
* @end
*
**********************************************************************/
#ifndef VOIP_H
#define VOIP_H

#include "nimapi.h"
#include "defaultconfig.h"
#include "avl_api.h"
#include "sysnet_api.h"
#include "l7_product.h"
#include "comm_mask.h"

#define VOIP_CFG_FILENAME    "voipCfgData.cfg"
#define VOIP_CFG_VER_1       0x1
#define VOIP_CFG_VER_CURRENT VOIP_CFG_VER_1

typedef struct
{
  nimConfigID_t          configId;
  L7_uint32              voipProfile;
  L7_uint32              voipBandwidth;     
}voipIntfCfgData_t;

typedef struct
{
  L7_fileHdr_t      cfgHdr;
  voipIntfCfgData_t voipIntfCfgData[L7_VOIP_MAX_INTF];
  L7_uint32         checkSum;    /* check sum of config file NOTE: needs to be last entry */
} voipCfgData_t;

#define L7_VOIP_CFG_DATA_SIZE  sizeof(voipCfgData_t)
/*********************************************************************
* @purpose  Saves VOIP file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSave(void);

/*********************************************************************
* @purpose  Restores VOIP user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipRestore(void);

/*********************************************************************
* @purpose  Checks if VOIP user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL voipHasDataChanged(void);
void voipResetDataChanged(void);
/*********************************************************************
* @purpose  Apply VOIP Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipApplyConfigData(void);

/*********************************************************************
* @purpose  Build default VOIP Interface config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void voipBuildDefaultIntfConfigData(nimConfigID_t *cfgId,
                                    voipIntfCfgData_t *pCfg);

/*********************************************************************
* @purpose  Build default VOIP config data
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes   
*
* @end
*********************************************************************/
void voipBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
*
* @purpose task to handle all VOIP messages
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void voipTask();

/*********************************************************************
* @purpose  Start VOIP TASk
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipStartTask();


SYSNET_PDU_RC_t voipPktIntercept(L7_uint32 hookId,
                                 L7_netBufHandle bufHandle,
                                 sysnet_pdu_info_t *pduInfo,
                                 L7_FUNCPTR_t continueFunc);

#endif
