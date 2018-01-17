/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dos_util.h
*
* @purpose DOS utility function prototypes
*
* @component DENIAL OF SERVICE
*
* @comments none
*
* @create 04/13/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#ifndef DOS_UTIL_H
#define DOS_UTIL_H

#include "comm_mask.h"
#include "nimapi.h"
#include "dos.h"

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to dos interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL dosMapIntfIsConfigurable(L7_uint32 intIfNum, dosIntfCfgData_t **pCfg);

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to dos interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL dosMapIntfConfigEntryGet(L7_uint32 intIfNum, dosIntfCfgData_t **pCfg);

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dosIntfCreate(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dosApplyIntfConfigData(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dosIntfDetach(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dosIntfDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Propogate Interface notifications to dos Task
*
* @param    L7_uint32  intIfNum   Interface number
* @param    L7_uint32  event      Event type
* @param    NIM_CORRELATOR_t  correlator  Correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dosIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, 
NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Handle Interface notifications
*
* @param    L7_uint32  intIfNum   Interface number
* @param    L7_uint32  event      Event type
* @param    NIM_CORRELATOR_t  correlator  Correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dosIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);

/*********************************************************************
*
* @purpose  Applies the Ping flooding mode for an interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uint32  mode      @b((input)) Ping flodding intf mode
* @param    L7_uint32  param     @b((input)) Param value
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes    This function assumes mode parameter is valid
*
* @end
*********************************************************************/
L7_RC_t dosIntfPingFloodingApply(L7_uint32 intIfNum, L7_uint32 mode,L7_uint32 param);

/*********************************************************************
*
* @purpose  Applies the Smurf Attack mode for an interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uint32  mode      @b((input)) Smurf Attack intf mode
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes    This function assumes mode parameter is valid
*
* @end
*********************************************************************/
L7_RC_t dosIntfSmurfAttackApply(L7_uint32 intIfNum,L7_uint32 mode);

/*********************************************************************
*
* @purpose  Applies the SynAck flooding mode for an interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uint32  mode      @b((input)) Syn Ack flodding intf mode
* @param    L7_uint32  param     @b((input)) Param value
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes    This function assumes mode parameter is valid
*
* @end
*********************************************************************/
L7_RC_t dosIntfSynAckFloodingApply(L7_uint32 intIfNum, L7_uint32 mode, L7_uint32 param);

/*********************************************************************
* @purpose  Propogate Startup notifications to dos Task
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void dosStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase);

#endif

