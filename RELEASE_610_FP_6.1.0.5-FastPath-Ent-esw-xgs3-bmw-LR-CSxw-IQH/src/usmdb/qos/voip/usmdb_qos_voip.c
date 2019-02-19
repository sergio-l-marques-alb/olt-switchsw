/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    usmdb_qos_voip.c
* @purpose     usmdb AUTOVOIP functions
* @component   base BP
* @comments    none
* @create      
* @author      Murali
* @end
*             
**********************************************************************/


#include <string.h>
#include "l7_common.h"
#include "nimapi.h"
#include "l7_cos_api.h"
#include "voip_exports.h"
#include "voip_api.h"
#include "usmdb_qos_voip_api.h"
#include "usmdb_util_api.h"
#include "voip_debug.h"
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
                                    L7_uint32 *CosQueue)
{
  if (interface == L7_ALL_INTERFACES)
  {
    interface = 0;
  }

  return voipIntfVoipProfileGet(interface, mode, bandwidth,CosQueue);
}


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
L7_RC_t usmDbQosVoIPIntfModeSet(L7_uint32 unit,L7_uint32 interface, L7_uint32 mode)
{
  L7_BOOL countFailure=L7_FALSE;
  L7_RC_t rc=L7_FAILURE;

  if (interface == L7_ALL_INTERFACES)
  {
    voipIntfVoipModeSet(0, mode);
    rc = usmDbQoSVoIPValidIntfFirstGet(&interface);
    while(rc == L7_SUCCESS)
    {
      if(voipIntfVoipModeSet(interface, mode) != L7_SUCCESS)
      {
        countFailure = L7_TRUE;
      }
      rc = usmDbQoSVoIPValidIntfNextGet(interface, &interface);
    }

    if(countFailure)
    {
      rc = L7_FAILURE;
    }
    else
    {
      rc = L7_SUCCESS;      
    }
  }
  else
  {
    if (voipIsValidIntf(interface) == L7_TRUE)
    {
      rc = voipIntfVoipModeSet(interface, mode);
    }
    else
    {
      rc = L7_FAILURE;      
    }
  }
  return rc;
}

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
L7_RC_t usmDbQosVoIPIntfModeGet(L7_uint32 unit,L7_uint32 interface, L7_uint32 *mode)
{
  if (interface == L7_ALL_INTERFACES)
  {
    interface = 0;
  }
  return voipIntfVoipModeGet(interface, mode);
}

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
L7_RC_t usmDbQosVoIPIntfMinBandwidthSet(L7_uint32 unit,L7_uint32 interface, L7_uint32 bandwidth)
{
  L7_BOOL countFailure=L7_FALSE;
  L7_RC_t rc=L7_FAILURE;

  if (interface == L7_ALL_INTERFACES)
  {
    voipIntfVoipMinBandwidthSet(0, bandwidth);
    rc = usmDbQoSVoIPValidIntfFirstGet(&interface);
    while(rc == L7_SUCCESS)
    {
      if(voipIntfVoipMinBandwidthSet(interface, bandwidth) != L7_SUCCESS)
      {
        countFailure = L7_TRUE;
      }
      rc = usmDbQoSVoIPValidIntfNextGet(interface, &interface);
    }

    if(countFailure)
    {
      rc = L7_FAILURE;
    }
    else
    {
      rc = L7_SUCCESS;      
    }
  }
  else
  {
    if (voipIsValidIntf(interface) == L7_TRUE)
    {
      rc = voipIntfVoipMinBandwidthSet(interface, bandwidth);
    }
    else
    {
      rc = L7_FAILURE;      
    }
  }
  return rc;
}

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
L7_RC_t usmDbQosVoIPIntfMinBandwidthGet(L7_uint32 unit,L7_uint32 interface, L7_uint32 *bandwidth)
{
  if (interface == L7_ALL_INTERFACES)
  {
    interface = 0;
  }
  return voipIntfVoipMinBandwidthGet(interface, bandwidth);
}

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
L7_RC_t usmDbQosVoIPIntfCosQueueGet(L7_uint32 unit,L7_uint32 interface, L7_uint32 *CosQueue)
{
  *CosQueue = FD_VOIP_COS_QUEUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine if the interface is valid for VOIP configuration
*
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      enabled or disabled
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbQoSVoIPIsValidIntf(L7_uint32 interface)
{
  return voipIsValidIntf(interface);
}

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
L7_RC_t usmDbQoSVoIPValidIntfNextGet(L7_uint32 prevIfNum, L7_uint32 *intIfNum)
{
  L7_RC_t   rc;
  L7_uint32 interface;

  rc = usmDbValidIntIfNumNext(prevIfNum, &interface); 
  while (rc == L7_SUCCESS)
  {
    if (voipIsValidIntf(interface) == L7_TRUE)
    {
      *intIfNum = interface;
      return L7_SUCCESS;
    }
    else
    {
      rc = usmDbValidIntIfNumNext(interface, &interface); 
    }
  }
  return L7_FAILURE;
}

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
L7_RC_t usmDbQoSVoIPValidIntfFirstGet(L7_uint32 *intIfNum)
{
  L7_RC_t   rc;
  L7_uint32 interface;

  rc = usmDbValidIntIfNumFirstGet(&interface);
  while (rc == L7_SUCCESS)
  {
    if (voipIsValidIntf(interface) == L7_TRUE)
    {
      *intIfNum = interface;
      return L7_SUCCESS;
    }
    else
    {
      rc = usmDbValidIntIfNumNext(interface, &interface); 
    }
  }
  return L7_FAILURE;
}

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
L7_RC_t usmDbQosVoIPDebugTraceFlagSet(L7_uint32 Protocol,L7_uint32 Flag)
{
  return voipDebugTraceFlagSet(Protocol, Flag);
}

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
                                 L7_uint32 *SCCPTraceFlag, L7_uint32 *MGCPTraceFlag)
{
  voipDebugTraceFlagGet(VOIP_PROTO_SIP, SIPTraceFlag);
  voipDebugTraceFlagGet(VOIP_PROTO_H323, H323TraceFlag);
  voipDebugTraceFlagGet(VOIP_PROTO_SCCP, SCCPTraceFlag);
  voipDebugTraceFlagGet(VOIP_PROTO_MGCP, MGCPTraceFlag);

  return L7_SUCCESS;
}
