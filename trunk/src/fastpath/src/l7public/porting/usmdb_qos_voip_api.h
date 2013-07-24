/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    usmdb_qos_voip_api.h
* @purpose     usmdb AUTOVOIP functions
* @component   base BP
* @comments    none
* @create
* @author      Murali
* @end
*
**********************************************************************/

#ifndef USMDB_VOIP_API_H
#define USMDB_VOIP_API_H

/*********************************************************************
* @purpose  Enables or disables the VOIP profile on interface
*
* @param  L7_uint32 unit      (input)  Unit number
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      enabled or disabled
* @param  L7_uint32 mode      (output)  Mode value
*
* @param  L7_uint32 bandwidth (output)  Bandwidth value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbVoipIntfVoipProfileGet(L7_uint32 unit,L7_uint32 interface, 
                                    L7_uint32 *mode, L7_uint32 *bandwidth,
                                    L7_uint32 *CosQueue);

/*********************************************************************
* @purpose  Enables or disables the VOIP profile on interface
*
* @param  L7_uint32 unit      (input)  Unit number
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      enabled or disabled
* @param  L7_uint32 mode      (input)  Mode value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbQosVoIPIntfModeSet(L7_uint32 unit,L7_uint32 interface, L7_uint32 mode);

/*********************************************************************
* @purpose  Gets the VOIP profile mode on interface
*
* @param  L7_uint32 unit      (input)  Unit number
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      enabled or disabled
* @param  L7_uint32 mode      (output)  Mode value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbQosVoIPIntfModeGet(L7_uint32 unit,L7_uint32 interface, L7_uint32 *mode);

/*********************************************************************
* @purpose  Sets the VOIP profile bandwidth on interface
*
* @param  L7_uint32 unit      (input)  Unit number
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      enabled or disabled
* @param  L7_uint32 bandwidth (input)  Bandwidth value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbQosVoIPIntfMinBandwidthSet(L7_uint32 unit,L7_uint32 interface, L7_uint32 bandwidth);

/*********************************************************************
* @purpose  Gets the VOIP profile Bandwidth on interface
*
* @param  L7_uint32 unit      (input)  Unit number
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      enabled or disabled
* @param  L7_uint32 bandwidth (output)  Bandwidth value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbQosVoIPIntfMinBandwidthGet(L7_uint32 unit,L7_uint32 interface, L7_uint32 *bandwidth);

/*********************************************************************
* @purpose  Gets the VOIP profile Cos Queue on interface
*
* @param  L7_uint32 unit      (input)  Unit number
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      enabled or disabled
* @param  L7_uint32 CosQueue (output)  Cos Queue
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbQosVoIPIntfCosQueueGet(L7_uint32 unit,L7_uint32 interface, L7_uint32 *CosQueue);

/*********************************************************************
* @purpose  Determine if the interface is valid for VOIP configuration
*
* @param  L7_uint32 unit      (input)  Unit number
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      enabled or disabled
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL usmdbQoSVoIPIsValidIntf(L7_uint32 interface);

/*********************************************************************
* @purpose  Get the next interface that is valid for VoIP
*
* @param     prevIfNum  @b{(input)} internal interface number
* @param    *intIfNum  @b{(output)} internal interface number
*
* @returns  L7_SUCCESS, if a valid interface is found
* @returns  L7_FAILURE, otherwise
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbQoSVoIPValidIntfNextGet(L7_uint32 prevIfNum, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Get the first interface that is valid for 802.1AB
*
* @param    *intIfNum  @b{(output)} internal interface number
*
* @returns  L7_SUCCESS, if a valid interface is found
* @returns  L7_FAILURE, otherwise
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbQoSVoIPValidIntfFirstGet(L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Sets the VOIP Protocol Trace Flag
*
* @param  L7_uint32 Protocol  (input)  VoIP Protocol
* @param  L7_uint32 Flag      (input)  Flag value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbQosVoIPDebugTraceFlagSet(L7_uint32 Protocol,L7_uint32 Flag);

/*********************************************************************
* @purpose  Gets All the VOIP Protocol Trace Flags
*
* @param  L7_uint32 SIPTraceFlag  (output)  SIP Protocol Trace Flag
* @param  L7_uint32 H323TraceFlag  (output) H323 Protocol Trace Flag
* @param  L7_uint32 SCCPTraceFlag  (output) SCCP Protocol Trace Flag
* @param  L7_uint32 MGCPTraceFlag  (output) MGCP Protocol Trace Flag
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
usmDbQosVoIPDebugTraceAllFlagGet(L7_uint32 *SIPTraceFlag, L7_uint32 *H323TraceFlag,
                                 L7_uint32 *SCCPTraceFlag, L7_uint32 *MGCPTraceFlag);


#endif

