/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  sflow_cfg.h
*
* @purpose   sFlow configuration
*
* @component sFlow
*
* @create 
*
* @author 
*
* @end
*             
**********************************************************************/
#ifndef SFLOW_CFG_H
#define SFLOW_CFG_H


#include "comm_mask.h"
#include "comm_structs.h"
#include "l7_common.h"
#include "l3_addrdefs.h"

#include "sflow_mib.h"

/* Configuration routines */
L7_RC_t sFlowSave(void);
L7_RC_t sFlowDebugSave(void);
L7_BOOL sFlowHasDataChanged(void);
void sFlowResetDataChanged(void);
L7_BOOL sFlowDebugHasDataChanged(void);
L7_RC_t sFlowApplyConfigData(void);
L7_RC_t sFlowApplyDebugConfigData(void);
L7_RC_t sFlowCfgDataNotChanged(void);
L7_RC_t sFlowRestoreProcess(void);
L7_RC_t sFlowCfgDump();

/* Default Configuration build routines */
void    sFlowBuildDefaultConfigData(L7_uint32 ver);
void sFlowDebugBuildDefaultConfigData(L7_uint32 ver);
void    sFlowBuildDefaultPollerConfigData(SFLOW_poller_mib_t *pPollerCfg);
void    sFlowBuildDefaultSamplerConfigData(SFLOW_sampler_mib_t *pSamplerCfg);
void    sFlowBuildDefaultReceiverConfigData(L7_uint32 rcvrIndx);
#endif /* SFLOW_CFG_H */
