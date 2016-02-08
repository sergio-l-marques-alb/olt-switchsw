/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1s_cnfgr.h
*
* @purpose   Multiple Spanning tree configurator header
*
* @component dot1s
*
* @comments 
*
* @create    8/5/2003
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1S_CNFGR_H
#define INCLUDE_DOT1S_CNFGR_H
L7_RC_t dot1sCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t dot1sCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t dot1sCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );
void    dot1sCnfgrFiniPhase1Process();
void    dot1sCnfgrFiniPhase2Process();
void    dot1sCnfgrFiniPhase3Process();
L7_RC_t dot1sCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t dot1sCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason );
void    dot1sCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t dot1sCfgDump();
L7_RC_t dot1sCfgInstanceDefaultPopulate(DOT1S_INSTANCE_INFO_CFG_t *instance, 
                                        L7_uint32 instIndex, 
                                        L7_uint32 instId);
L7_RC_t dot1sCfgInstanceMacAddrPopulate(DOT1S_INSTANCE_INFO_CFG_t *instance, 
                                        L7_uint32 instIndex);
void dot1sCnfgrInstApply();
DOT1S_PORT_COMMON_CFG_t *dot1sIntfCfgEntryGet(L7_uint32 intIfNum);


#endif /*INCLUDE_DOT1S_CNFGR_H*/
