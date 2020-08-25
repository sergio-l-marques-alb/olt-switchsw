/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename llpf_util.h
*
* @purpose Link Local Protocol Filtering Utility 
*
* @component LLPF
*
* @comments none
*
* @create 10/09/2009
*
* @author Vijayanand K(kvijayan)
* @end
*
**********************************************************************/
#ifndef LLPF_UTIL_H
#define LLPF_UTIL_H

#include "comm_mask.h"
#include "nimapi.h"
#include "comm_structs.h"

#include "llpf_exports.h"
#include "llpf_cfg.h"

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    intfNum        @{{input}}The Current Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t llpfIntfCreate(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose To process the call back for L7_ATTACH
*
* @param    intfNum        @{{input}}The Current Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t llpfIntfApplyConfigData(L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Note that configuration data has changed
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void llpfCfgDataChange(void);
/*********************************************************************
* @purpose  Note that configuration data is now in sync with persistent
*           storage.
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void llpfCfgDataNotChanged(void);
/*********************************************************************
* @purpose  Set LLPF Protocol Filter Blocking Mode
*           for a given interface in the config
*
* @param    intIfNum   internal interface number
* @param    L7_LLPF_BLOCK_TYPE_t   protocolType  @{{input}}  Type of Protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t llpfIntfFilterModeSet(L7_uint32 intIfNum,
                 L7_LLPF_BLOCK_TYPE_t protocolType);
/*********************************************************************
* @purpose  Clear LLPF Protocol Filter Blocking Mode
*           for a given interface in the config
*
* @param    intfNum        @{{input}}The Current Interface Number
* @param    protocolType   @{{input}}  Type of Protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t llpfIntfFilterModeClear(L7_uint32 intIfNum,
                 L7_LLPF_BLOCK_TYPE_t protocolType);
/*********************************************************************
* @purpose  Get LLPF Protocol Filter Blocking Mode
*           for a given interface in the config
*    
* @param    intfNum        @{{input}}The Current Interface Number
* @param    protocolType   @{{input}}  Type of Protocol
*       
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*     
* @end
*********************************************************************/
L7_RC_t llpfIntfFilterModeGet(L7_uint32 intIfNum,
                 L7_LLPF_BLOCK_TYPE_t protocolType,L7_BOOL *mode);


/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH
*
* @param    intfNum        @{{input}}The Current Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t llpfIntfDetach(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Propogate Interface notifications to llpf
*
* @param    intIfNum   @{{input}} The Current Interface Number
* @param    event      @{{input}} Event type
* @param    correlator @{{input}} Correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t llpfIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, 
                    NIM_CORRELATOR_t correlator);
/*********************************************************************
* @purpose  Set LLPF Protocol Filter Blocking Mode.
*
* @param    intIfNum      @{{input}} Interface number
* @param    protocolType  @{{input}}  Type of Protocol
* @param    mode          @{{input}}  Enable/Disable LLPF
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t llpfIntfBlockModeApply(L7_uint32 intifnum,
                 L7_LLPF_BLOCK_TYPE_t protocoltype, L7_uint32 mode);

/*********************************************************************
* @purpose  Propogate Startup notifications to llpf  
*
* @param    startupPhase  @{{input}}NIM phase of operation
*
* @notes
*
* @end
*********************************************************************/
void llpfStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase);
/*********************************************************************
* @purpose  Build default LLPF config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void llpfBuildDefaultConfigData(L7_uint32 ver);
/*********************************************************************
* @purpose  Build default llpf Interface config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void llpfBuildIntfDefaultConfigData(L7_uint32 intfNum);
/*********************************************************************
* @purpose  Saves LLPF user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t llpfSave(void);

/*********************************************************************
* @purpose  Checks if LLPF user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL llpfHasDataChanged(void);
/*********************************************************************
* @purpose  Restores  LLPF user config data change Flag
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/

L7_BOOL llpfResetDataChanged(void);

/*********************************************************************
* @purpose  Checks to see if the interface is valid for LLPF
*
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE or L7_FALSE
*       
* @comments
*
* @end
*********************************************************************/
L7_BOOL llpfIntfIsValid(L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Handle Interface notifications
* 
* @param    intIfNum   @{{input}} Interface number
* @param    event      @{{input}} Event type
* @param    correlator @{{input}} Correlator for event
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE            
*   
* @notes    
* 
* @end
*********************************************************************/
L7_RC_t llpfIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);
/*********************************************************************
* @purpose  Build default llpf Interface config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void llpfBuildIntfDefaultConfigData(L7_uint32 intfNum);
#endif

