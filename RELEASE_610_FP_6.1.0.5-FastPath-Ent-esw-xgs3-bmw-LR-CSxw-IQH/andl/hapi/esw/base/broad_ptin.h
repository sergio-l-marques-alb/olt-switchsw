/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_system.h
*
* @purpose   This file contains the hapi interface for system parameters
*
* @component hapi
*
* @comments
*
* @create    2/22/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_BROAD_PTIN_H
#define INCLUDE_BROAD_PTIN_H

#include "dapi.h"

L7_RC_t hapiBroadPtinStart(void);

L7_RC_t hapiBroadPtinInit(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

L7_RC_t hapiBroadPtinPhySet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

L7_RC_t hapiBroadPtinGetHwResources(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

L7_RC_t hapiBroadPtinVlanDefs(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

L7_RC_t hapiBroadPtinVlanTranslate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

L7_RC_t hapiBroadPtinFPentry(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

L7_RC_t hapiBroadPtinFlowCounters(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

L7_RC_t hapiBroadPtinBcastPktLimit(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

L7_RC_t hapiBroadPtinBitstreamUpFwd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

L7_RC_t hapiBroadPtinBitstreamLagRecheck(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

//L7_RC_t hapiBroadPtinSetPhyConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);
//
//L7_RC_t hapiBroadPtinGetPhyConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);
//
//L7_RC_t hapiBroadPtinGetPhyState(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

L7_RC_t hapiBroadPtinGetCounters(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

L7_RC_t hapiBroadPtinClearCounters(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

L7_RC_t hapiBroadPtinStatisticStatus(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

//L7_RC_t hapiBroadPtinFlowCreate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);
//
//L7_RC_t hapiBroadPtinFlowKill(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);
//
//L7_RC_t hapiBroadPtinSetProfile(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

#endif

