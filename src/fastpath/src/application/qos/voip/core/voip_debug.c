/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    voip_debug.c
* @purpose    
* @component   
* @comments    none
* @create      
* @author      pmurali
*              
* @end
*             
*********************************************************************/
#include "l7_common.h"
#include "voip_debug.h"
#include "voip_exports.h"
#include "sysapi.h"
#include "voip_sip.h"
#include "voip_sccp.h"
#include "voip_h323.h"

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/

static L7_uint32   voipH323TraceFlag = L7_DISABLE;
static L7_uint32   voipMGCPTraceFlag = L7_DISABLE;
static L7_uint32   voipSCCPTraceFlag = L7_DISABLE;
static L7_uint32   voipSIPTraceFlag = L7_DISABLE;
/*********************************************************************
*
* @purpose
*
* @param    flag   {(input)} Trace Level
* @param    family {(input)} Snoop Instance
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL
voipDebugTraceFlagCheck (L7_uint32 traceFlag, L7_uint32 protocol)
{
  L7_BOOL rc = L7_DISABLE;
  
  if (traceFlag > VOIP_DEBUG_NONE)
  {
    switch (protocol)
    {
      case VOIP_PROTO_H323:
        rc = voipH323TraceFlag;
        break;
        
      case VOIP_PROTO_MGCP:
        rc = voipMGCPTraceFlag;
        break;
        
      case VOIP_PROTO_SCCP:
        rc = voipSCCPTraceFlag;
        break;
        
      case VOIP_PROTO_SIP:
        rc = voipSIPTraceFlag;
        break;
        
      default:
        break;
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Sets the VOIP Protocol Trace Flag
*
* @param  L7_uint32 Protocol  (input)  VoIP Protocol
* @param  L7_uint32 Flag      (input)  Flag value
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
voipDebugTraceFlagSet (L7_uint32 Protocol,L7_uint32 Flag)
{
  L7_RC_t rc = L7_SUCCESS;
  
  switch (Protocol)
  {
    case VOIP_PROTO_H323:
      voipH323TraceFlag = Flag;
      break;
      
    case VOIP_PROTO_MGCP:
      voipMGCPTraceFlag = Flag;
      break;
      
    case VOIP_PROTO_SCCP:
      voipSCCPTraceFlag = Flag;
      break;
      
    case VOIP_PROTO_SIP:
      voipSIPTraceFlag =  Flag;
      break;
      
    case VOIP_PROTO_ALL:
      voipH323TraceFlag = Flag;
      voipMGCPTraceFlag = Flag;
      voipSCCPTraceFlag = Flag;
      voipSIPTraceFlag  = Flag;
      break;

    default:
      rc = L7_FAILURE;
      break;
  }

  return rc;
}


/*********************************************************************
*
* @purpose  Gets the VOIP Protocol Trace Flag
*
* @param  L7_uint32 Protocol  (input)  VoIP Protocol
* @param  L7_uint32 Flag      (input)  Flag value
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
voipDebugTraceFlagGet (L7_uint32 Protocol,L7_uint32 *Flag)
{
  L7_RC_t rc = L7_SUCCESS;
  
  switch (Protocol)
  {
    case VOIP_PROTO_H323:
      *Flag = voipH323TraceFlag;
      break;
      
    case VOIP_PROTO_MGCP:
      *Flag = voipMGCPTraceFlag;
      break;
      
    case VOIP_PROTO_SCCP:
      *Flag = voipSCCPTraceFlag;
      break;
      
    case VOIP_PROTO_SIP:
      *Flag = voipSIPTraceFlag;
      break;
      
    default:
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the VOIP Protocol String
*
* @param  L7_uint32 Protocol  (input)  VoIP Protocol
* @param  L7_uchar8 *Str      (output) VoIP Protocol String
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
voipDebugVoIPProtoGet (L7_uint32 Protocol,L7_char8*Str)
{
  L7_RC_t rc = L7_SUCCESS;

  if (!Str)
  {
    rc = L7_FAILURE;
  }
  else
  {
    switch (Protocol)
    {
      case VOIP_PROTO_H323:
        osapiStrncpy(Str, "H323", VOIP_MAX_DEBUG_MSG_SIZE);
        break;
        
      case VOIP_PROTO_MGCP:
        osapiStrncpy(Str, "MGCP", VOIP_MAX_DEBUG_MSG_SIZE);
        break;
        
      case VOIP_PROTO_SCCP:
        osapiStrncpy(Str, "SCCP", VOIP_MAX_DEBUG_MSG_SIZE);
        break;
        
      case VOIP_PROTO_SIP:
        osapiStrncpy(Str, "SIP", VOIP_MAX_DEBUG_MSG_SIZE);
        break;
        
      default:
        rc = L7_FAILURE;
        break;
    }
  }
  return rc;
}


/************************ DEBUG FUNCTION ****************************/
void voipCallDBDump()
{
  voipSipCallDump();
  voipSccpCallDump();
  voipH323CallDump();
}

