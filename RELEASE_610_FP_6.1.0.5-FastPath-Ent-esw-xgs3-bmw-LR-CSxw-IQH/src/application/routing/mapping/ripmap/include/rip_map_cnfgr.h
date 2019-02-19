/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  rtrdisc_cnfgr.h
*
* @purpose   To discover router protocols
*
* @component 
*
* @comments  none
*
* @create    11/02/2001
*
* @author    ksdesai
*
* @end
*             
**********************************************************************/

/*************************************************************
		
*************************************************************/

#ifndef INCLUDE_RIP_MAP_CNFGR_H
#define INCLUDE_RIP_MAP_CNFGR_H

                                      
                                      
/****************************************
*
*  OSPF Cnfgr State
*
*****************************************/
typedef enum {
  RIPMAP_PHASE_INIT_0 = 0,
  RIPMAP_PHASE_INIT_1,
  RIPMAP_PHASE_INIT_2,
  RIPMAP_PHASE_WMU,
  RIPMAP_PHASE_INIT_3,
  RIPMAP_PHASE_EXECUTE,
  RIPMAP_PHASE_UNCONFIG_1,
  RIPMAP_PHASE_UNCONFIG_2,
} ripMapCnfgrState_t;


/****************************************
*
*  RIP Deregistrations
*
*****************************************/
typedef struct {
  L7_BOOL ripMapSave;
  L7_BOOL ripMapRestore;
  L7_BOOL ripMapHasDataChanged;
  L7_BOOL ripMapExtenRouteCallback;
  L7_BOOL ripMapIntfChangeCallback;  /* WPJ_TBD:  Complete deregistration in phase 2 */
} ripMapDeregister_t;

#define RIPMAP_IS_READY (((ripMapCnfgrState == RIPMAP_PHASE_INIT_3) || \
					   (ripMapCnfgrState == RIPMAP_PHASE_EXECUTE) || \
					   (ripMapCnfgrState == RIPMAP_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

void ripMapApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData );
L7_RC_t ripMapCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t ripMapCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t ripMapCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t ripMapCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse, 
                                 L7_CNFGR_ERR_RC_t   *pReason );
void ripMapCnfgrFiniPhase1Process();
void ripMapCnfgrFiniPhase2Process();
void ripMapCnfgrFiniPhase3Process();
L7_RC_t ripMapCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason );

#endif /* INCLUDE_RIP_MAP_CNFGR_H */
