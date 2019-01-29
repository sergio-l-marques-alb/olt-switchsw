/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_util.h
*
* @purpose VOIP utility function prototypes
*
* @component VOIP
*
* @comments none
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#ifndef VOIP_UTIL_H
#define VOIP_UTIL_H

#include "comm_mask.h"
#include "nimapi.h"
#include "voip.h"

typedef struct voipIntfInfo_s
{
  /* Bit mask indicating the components which have acquired
   * the interface */
  AcquiredMask        acquiredList;
} voipIntfInfo_t;

/*********************************************************************
* @purpose  check if the interface is attached
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_BOOL voipMapIntfIsAttached(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to voip interface config structure
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
L7_BOOL voipMapIntfIsConfigurable(L7_uint32 intIfNum, voipIntfCfgData_t **pCfg);

/*********************************************************************
* @purpose  check if the interface is acquired by another component
*           and is therefore unavailable to VOIP
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_BOOL voipMapIntfIsAcquired(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to voip interface config structure
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
L7_BOOL voipMapIntfConfigEntryGet(L7_uint32 intIfNum, voipIntfCfgData_t **pCfg);

/*********************************************************************
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
L7_RC_t voipIntfCreate(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Applies the interface configuration data
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
L7_RC_t voipApplyIntfConfigData(L7_uint32 intIfNum);

/*********************************************************************
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
L7_RC_t voipIntfDetach(L7_uint32 intIfNum);

/*********************************************************************
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
L7_RC_t voipIntfDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Propogate Interface notifications to voip Task
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
L7_RC_t voipIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, 
NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Propogate Startup notifications to VOIP component
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void voipStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase);

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
L7_RC_t voipIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Propogate Startup notifications to VOIP component
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void voipStartupNotifyProcess(NIM_STARTUP_PHASE_t startupPhase);

/*********************************************************************
* @purpose  Applies the voip Profile for an interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uint32  mode      @b((input)) voip Profile intf mode
* @param    L7_uint32  bandwidth @b((input)) bandwidth
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t voipProfileIntfModeApply(L7_uint32 intIfNum,L7_uint32 mode, L7_uint32 bandwidth);

#endif

