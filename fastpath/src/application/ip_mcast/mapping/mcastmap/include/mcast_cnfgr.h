/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename mcast_cnfgr.h
*
* @purpose Contains prototypes to support the new configurator API
*
* @component  MCAST Mapping component
*
* @comments
*
* @create 06/23/2003
*
* @author jeffr
* @end
*
**********************************************************************/


#ifndef INCLUDE_MCAST_CNFGR_H
#define INCLUDE_MCAST_CNFGR_H

#include "l7_common.h"
#include "l7_cnfgr_api.h"

typedef enum {
  MCAST_PHASE_INIT_0 = 0,
  MCAST_PHASE_INIT_1,
  MCAST_PHASE_INIT_2,
  MCAST_PHASE_WMU,
  MCAST_PHASE_INIT_3,
  MCAST_PHASE_EXECUTE,
  MCAST_PHASE_UNCONFIG_1,
  MCAST_PHASE_UNCONFIG_2,
} mcastCnfgrState_t;

#define MCAST_IS_READY (mcastGblVariables_g.mcastCnfgrState == MCAST_PHASE_INIT_3) ? (L7_TRUE) : ((mcastGblVariables_g.mcastCnfgrState == MCAST_PHASE_EXECUTE) ? (L7_TRUE) : ((mcastGblVariables_g.mcastCnfgrState == MCAST_PHASE_UNCONFIG_1) ? (L7_TRUE) : (L7_FALSE)))


/********************************************************************
*                     Function Prototypes
*********************************************************************/

extern void mcastApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

extern void mcastCnfgrCommandProcess(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif /* INCLUDE_MCAST_CNFGR_H */
