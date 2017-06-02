/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_sm.h
*
* @purpose   Multiple Spanning tree State Machine header file
*
* @component dot1s
*
* @comments 
*
* @create    9/19/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1S_SM_H
#define INCLUDE_DOT1S_SM_H

#include "dot1s_txrx.h"
#include "dot1s_states.h"

/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */
L7_RC_t dot1sForwardingDisable(L7_uint32 intIfNum, L7_uint32 instIndex);
L7_RC_t dot1sLearningDisable(L7_uint32 intIfNum, L7_uint32 instIndex);
L7_RC_t dot1sForwardingEnable(L7_uint32 intIfNum, L7_uint32 instIndex);                  
L7_RC_t dot1sLearningEnable(L7_uint32 intIfNum, L7_uint32 instIndex);
L7_RC_t dot1sFlush(L7_uint32 intIfNum, L7_uint32 instIndex);
L7_RC_t dot1sBPDUVersionUpdate(DOT1S_PORT_COMMON_t *p, 
							   DOT1S_MSTP_ENCAPS_t 	*bpdu);
L7_RC_t dot1sReselectTreeClear(L7_uint32 instIndex);
L7_RC_t dot1sReselectTreeSet(L7_uint32 instIndex);
L7_uint32 dot1sTcWhileNew(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sTcWhileZero(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
DOT1S_RCVD_INFO_t dot1sInfoCistRcv(DOT1S_PORT_COMMON_t *p, DOT1S_MSTP_ENCAPS_t 	*bpdu);
DOT1S_RCVD_INFO_t dot1sInfoMstiRcv(DOT1S_PORT_COMMON_t *p, 
								   L7_uint32 instIndex, 
								   DOT1S_MSTP_ENCAPS_t 	*bpdu);
L7_RC_t dot1sProposalCistRecord(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sProposalMstiRecord(DOT1S_PORT_COMMON_t *p, 
								L7_uint32 instIndex);
L7_RC_t dot1sReRootTreeSet(L7_uint32 instIndex);
L7_RC_t dot1sSelectedTreeSet(L7_uint32 instIndex);
L7_RC_t dot1sSelectedTreeClear(L7_uint32 instIndex);
L7_RC_t dot1sSyncTreeSet(L7_uint32 instIndex);
L7_RC_t dot1sTcFlagsSet(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sTcPropTreeSet(L7_uint32 intIfNum, L7_uint32 instIndex);
L7_RC_t dot1sRcvdInfoWhileUpdt(DOT1S_PORT_COMMON_t *p,
								   L7_uint32 instIndex );
L7_RC_t dot1sRolesCistUpdt();
L7_RC_t dot1sRolesMstiUpdt(L7_uint32 instIndex);
L7_RC_t dot1sRolesDisabledTreeUpdt(L7_uint32 instIndex);
L7_BOOL dot1sBetterOrSameInfoCist(DOT1S_PORT_COMMON_t *p, 
								  DOT1S_CIST_PRIORITY_VECTOR_t *cistMsgPrio,
								  DOT1S_INFO_t newInfoIs);
L7_BOOL dot1sBetterOrSameInfoMsti(DOT1S_PORT_COMMON_t *p, 
								  L7_uint32 instIndex, 
								  DOT1S_MSTI_PRIORITY_VECTOR_t *mstiMsgPrio);
L7_RC_t dot1sRcvdMsgsClearAll();
L7_BOOL dot1sFromSameRegion(DOT1S_PORT_COMMON_t *p, DOT1S_MSTP_ENCAPS_t 	*bpdu);
L7_RC_t dot1sAgreementCistRecord(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sAgreementMstiRecord(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sRecordDispute(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sMasteredCistRecord(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sMasteredMstiRecord(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sRcvdMsgsSet(DOT1S_PORT_COMMON_t *p, DOT1S_MSTP_ENCAPS_t 	*bpdu);
L7_RC_t dot1sPimMachine(L7_uint32 dot1sEvent, 
						DOT1S_PORT_COMMON_t *p, 
						L7_uint32 instIndex,
						DOT1S_MSTP_ENCAPS_t 	*bpdu);
L7_RC_t dot1sPimDisabledAction(DOT1S_PORT_COMMON_t *p, 
							   L7_uint32 instIndex);
L7_RC_t dot1sPimEnabledAction(DOT1S_PORT_COMMON_t *p, 
							  L7_uint32 instIndex);
L7_RC_t dot1sPimAgedAction(DOT1S_PORT_COMMON_t *p, 
						   L7_uint32 instIndex);
L7_RC_t dot1sPimUpdateAction(DOT1S_PORT_COMMON_t *p, 
							 L7_uint32 instIndex);
L7_RC_t dot1sPimCurrentAction(DOT1S_PORT_COMMON_t *p, 
							  L7_uint32 instIndex);
L7_RC_t dot1sPimOtherAction(DOT1S_PORT_COMMON_t *p, 
							L7_uint32 instIndex);
L7_RC_t dot1sPimNotDesignatedAction(DOT1S_PORT_COMMON_t *p, 
						   L7_uint32 instIndex);
L7_RC_t dot1sPimReceiveAction(DOT1S_PORT_COMMON_t *p, 
							  L7_uint32 instIndex, 
							  DOT1S_MSTP_ENCAPS_t 	*bpdu);
L7_RC_t dot1sPimRepDesigAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPimInferiorDesigAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPimSupDesigAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrsMachine(L7_uint32 dot1sEvent, L7_uint32 instIndex);
L7_RC_t dot1sPrsInitAction(L7_uint32 instIndex);
L7_RC_t dot1sPrsReceiveAction(L7_uint32 instIndex);



L7_RC_t dot1sPrtInitPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtBackupPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtDisablePortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtBlockPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtDisabledPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtMasterPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtDesigPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtRootPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtAlternatePortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtMasterRetiredAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtDesigRetiredAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtRerootedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtMasterDiscardAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtDesigDiscardAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtMasterLearnAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtDesigLearnAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtRootLearnAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtMasterForwardAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtDesigForwardAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtRootForwardAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtRerootAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtMasterSyncedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtDesigSyncedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtRootSyncedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtMasterAgreedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtDesigAgreedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtRootAgreedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtAlternateAgreedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtDesigProposeAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtMasterProposedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtRootProposedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrtAlternateProposedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);

L7_RC_t dot1sPrtMachine(L7_uint32 dot1sEvent,
						DOT1S_PORT_COMMON_t *p, 
						L7_uint32 instIndex);
L7_RC_t dot1sPrtDisabledMachine(L7_uint32 dot1sEvent,
						DOT1S_PORT_COMMON_t *p, 
						L7_uint32 instIndex);
L7_RC_t dot1sPrtMasterMachine(L7_uint32 dot1sEvent,
						DOT1S_PORT_COMMON_t *p, 
						L7_uint32 instIndex);
L7_RC_t dot1sPrtDesigMachine(L7_uint32 dot1sEvent,
						DOT1S_PORT_COMMON_t *p, 
						L7_uint32 instIndex);
L7_RC_t dot1sPrtRootMachine(L7_uint32 dot1sEvent,
						DOT1S_PORT_COMMON_t *p, 
						L7_uint32 instIndex);
L7_RC_t dot1sPrtAltBkupMachine(L7_uint32 dot1sEvent,
						DOT1S_PORT_COMMON_t *p, 
						L7_uint32 instIndex);


L7_RC_t dot1sPortStateMachineInit(DOT1S_PORT_COMMON_t *p, L7_BOOL portEnabled);
L7_RC_t dot1sResetBpduGuardEffect(DOT1S_PORT_COMMON_t *p);

L7_RC_t dot1sPrxMachine(L7_uint32 dot1sEvent,
						DOT1S_PORT_COMMON_t *p, 
						DOT1S_MSTP_ENCAPS_t 	*bpdu);
L7_RC_t dot1sPrxDiscardAction(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPrxReceiveAction(DOT1S_PORT_COMMON_t *p, DOT1S_MSTP_ENCAPS_t 	*bpdu);
L7_RC_t dot1sPstMachine(L7_uint32 dot1sEvent, 
						DOT1S_PORT_COMMON_t *p, 
						L7_uint32 instIndex);
L7_RC_t dot1sPstDiscardingAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPstLearningAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPstForwardingAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sTcmMachine(L7_uint32 dot1sEvent,
						DOT1S_PORT_COMMON_t *p, 
						L7_uint32 instIndex);
L7_RC_t dot1sTcmInitAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sTcmInactiveAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sTcmDetectedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sTcmActiveAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sTcmNotifiedTcnAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sTcmNotifiedTcAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sTcmPropagatingAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sTcmAcknowledgedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPpmMachine(L7_uint32 dot1sEvent, DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPpmCheckingRstpAction(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPpmSelectingStpAction(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPpmSensingAction(DOT1S_PORT_COMMON_t *p);

L7_RC_t dot1sPtxMachine(L7_uint32 dot1sEvent, DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPtxInitAction(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPtxPeriodicAction(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPtxConfigAction(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPtxTcnAction(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPtxRstpAction(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPtxIdleAction(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPtxGenerateEvents(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sNeedForBpduTxStopCheck(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPtiMachine();
L7_RC_t dot1sPtiTickAction();
L7_RC_t dot1sPtiOneSecondAction();
L7_RC_t dot1sBdmMachine(L7_uint32 dot1sEvent, DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sBdmEdgeAction(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sBdmNotEdgeAction(DOT1S_PORT_COMMON_t *p);
DOT1S_PRI_COMP_t dot1sMstiPriVecCompare(DOT1S_MSTI_PRIORITY_VECTOR_t *pv1, 
										DOT1S_MSTI_PRIORITY_VECTOR_t *pv2);
DOT1S_PRI_COMP_t dot1sCistPriVecCompare(DOT1S_CIST_PRIORITY_VECTOR_t *pv1, 
										DOT1S_CIST_PRIORITY_VECTOR_t *pv2);
L7_BOOL dot1sIdNotEqual(DOT1S_BRIDGEID_t id1, DOT1S_BRIDGEID_t id2);
L7_BOOL dot1sIdEqual(DOT1S_BRIDGEID_t id1, DOT1S_BRIDGEID_t id2);
L7_BOOL dot1sIdLesser(DOT1S_BRIDGEID_t id1, DOT1S_BRIDGEID_t id2);
L7_BOOL dot1sIsConfigBPDU(DOT1S_MSTP_ENCAPS_t 	*bpdu);
L7_BOOL dot1sIsTcnBPDU(DOT1S_MSTP_ENCAPS_t 	*bpdu);
dot1s_bpdu_types_t dot1sBPDUTypeGet(DOT1S_MSTP_ENCAPS_t *bpdu);
L7_BOOL dot1sIsRstBPDU(DOT1S_MSTP_ENCAPS_t 	*bpdu);
L7_BOOL dot1sIsMstBPDU(DOT1S_MSTP_ENCAPS_t 	*bpdu);
L7_BOOL dot1sCistRootPriVecDerived(DOT1S_CIST_PRIORITY_VECTOR_t *rpv,
								   DOT1S_CIST_PRIORITY_VECTOR_t *ppv,
								   L7_uint32 epc,
								   L7_uint32 ipc,
								   L7_BOOL rcvdInternal);
L7_BOOL dot1sMstiRootPriVecDerived(DOT1S_MSTI_PRIORITY_VECTOR_t *rpv,
								   DOT1S_MSTI_PRIORITY_VECTOR_t *ppv,
								   L7_uint32 ipc);
L7_RC_t dot1sStateMachineClassifier(dot1sEvents_t dot1sEvent, 
									DOT1S_PORT_COMMON_t *p, 
									L7_uint32 instIndex,
									DOT1S_MSTP_ENCAPS_t 	*bpdu,
									void *msg);
L7_RC_t dot1sPrxGenerateEvents(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPrtGenerateEvents(DOT1S_PORT_COMMON_t *p, 
							   L7_uint32 instIndex);
L7_RC_t dot1sPpmGenerateEvents(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sBdmGenerateEvents(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sPimGenerateEvents(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sPrsGenerateEvents(L7_uint32 instIndex);
L7_RC_t dot1sPstGenerateEvents(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sTcmGenerateEvents(DOT1S_PORT_COMMON_t *p, 
							   L7_uint32 instIndex);
L7_RC_t generateEventsForBDM(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1sTimerGenerateEvents(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sNewRootCheck(DOT1S_BRIDGEID_t bridgeId,
                          DOT1S_BRIDGEID_t rootId,
                          DOT1S_BRIDGEID_t bestId,
                          L7_uint32 instIndex);
L7_RC_t dot1sAgreeFlagCompute(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
L7_RC_t dot1sMstiMasterFlagCompute(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
DOT1S_PRI_COMP_t dot1sCistPriVecAllCompare(DOT1S_CIST_PRIORITY_VECTOR_t *pv1, 
										   DOT1S_CIST_PRIORITY_VECTOR_t *pv2);
DOT1S_PRI_COMP_t dot1sMstiPriVecAllCompare(DOT1S_MSTI_PRIORITY_VECTOR_t *pv1, 
										   DOT1S_MSTI_PRIORITY_VECTOR_t *pv2);



void dot1sLoopInconsistentSet(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex, L7_BOOL mode);
void dot1sCallCkPtServiceIfNeeded(L7_uint32 event);







  

#endif /*INCLUDE_DOT1S_SM_H*/
