/*********************************************************************
*
  * (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_cnfgr.h
*
* @purpose   isdp configurator file
*
* @component isdp
*
* @comments 
*
* @create    8/11/2007
*
* @author    dgaryachy
*
* @end
*             
**********************************************************************/
#ifndef ISDP_CNFGR_H
#define ISDP_CNFGR_H

#include "isdp_util.h"
#include "avl_api.h"

typedef enum
{
  ISDP_PHASE_INIT_0 = 0,
  ISDP_PHASE_INIT_1,
  ISDP_PHASE_INIT_2,
  ISDP_PHASE_WMU,
  ISDP_PHASE_INIT_3,
  ISDP_PHASE_EXECUTE,
  ISDP_PHASE_UNCONFIG_1,
  ISDP_PHASE_UNCONFIG_2,
} isdpCnfgrState_t;

typedef struct isdpEntryTree_s
{
  avlTree_t       treeData;
  avlTreeTables_t treeHeap[L7_ISDP_MAX_NEIGHBORS];
  isdpEntry_t     dataHeap[L7_ISDP_MAX_NEIGHBORS];
} isdpEntryTree_t;

#define ISDP_IS_READY (((isdpCnfgrState == ISDP_PHASE_WMU) ||        \
                         (isdpCnfgrState == ISDP_PHASE_INIT_3) ||     \
                         (isdpCnfgrState == ISDP_PHASE_EXECUTE) ||    \
                         (isdpCnfgrState == ISDP_PHASE_UNCONFIG_1) || \
                         (isdpCnfgrState == ISDP_PHASE_UNCONFIG_2)) ? (L7_TRUE) : (L7_FALSE))

#define ISDP_IP_ADDR_BUF_DESC "ISDPIpAddrBufs"  /* 16 char max */

/* Begin Function Prototypes */
L7_RC_t isdpCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t isdpCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t isdpCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );
void    isdpCnfgrFiniPhase1Process();
void    isdpCnfgrFiniPhase2Process();
void    isdpCnfgrFiniPhase3Process();
L7_RC_t isdpCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t isdpCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason );

void    isdpCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_BOOL isdpCnfgrStateCheck (void);
/* End Function Prototypes */

#endif /*ISDP_CNFGR_H*/
