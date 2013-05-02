/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\mgmt\snmp\packages\base\k_mib_rfc2925_traceroute_api.h
*
* @purpose Provide interface to  TraceRoute MIB
*
* @component SNMP
*
* @comments
*
* @create 16/11/2006
*
* @author sowjanya
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include "sr_ip.h"
#include <usmdb_traceroute_api.h>
#include "l7_common.h"
#include "usmdb_traceroute_api.h"
#include "usmdb_util_api.h"

/*********************************************************************
*  Set the Row Status field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    TraceRouteRowStatus   @b{(input)} Row Status
* @param    contextInfo  @b{(input)} Context information
*
* @returns  L7_SUCCESS  row-status is set
* @returns  L7_FAILURE  Failed to set row-status
*
* @comments  Row-status field has been partially implemented.
*            If user sets it to destroy, the session is deleted.
*
* @end
*********************************************************************/
L7_RC_t
snmpTraceRouteRowStatusSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 traceRouteRowStatus,ContextInfo *contextInfo)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_ushort16 handle;
  L7_BOOL syncFlag = L7_FALSE;

  
  switch (traceRouteRowStatus)
  {
    case D_traceRouteCtlRowStatus_createAndGo:
      rc = usmDbTraceRouteAlloc( index,testName,
          syncFlag, NULL, contextInfo, &handle );
      break;
      
    case D_traceRouteCtlRowStatus_destroy:
      rc =usmDbTraceRouteHandleGet( index,testName, &handle);
      if(rc == L7_SUCCESS)
        rc = usmDbTraceRouteFree( handle );
      break;
      
    case D_traceRouteCtlRowStatus_active:
      break;
      
    default:
      rc = L7_FAILURE;
  }

  return rc;

}

/*********************************************************************
*  Get the Row Status field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    traceRouteRowStatus   @b{(output)} Row Status
*
* @returns  L7_SUCCESS
*
* @comments
*
*
* @end
*********************************************************************/
 L7_RC_t
snmpTraceRouteRowStatusGet(L7_ushort16 handle,int *traceRouteRowStatus)
{
  L7_uint32 ipDa;
  
  
  if(usmDbTraceRouteDestAddrGet(handle, &ipDa)== L7_SUCCESS && ipDa != 0)
  {
    *traceRouteRowStatus =  D_traceRouteCtlRowStatus_active;
  }
  else
  {
    *traceRouteRowStatus =  D_traceRouteCtlRowStatus_createAndGo;
  }
 
  return L7_SUCCESS;
}

/*********************************************************************
*  Set the Admin Status field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    traceRouteAdminStatus   @b{(input)} Admin Status
*
* @returns  L7_SUCCESS if traceroute operation starts
*           L7_FAILURE if Row Status is not active or session is in progress.
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTraceRouteAdminStatusSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 traceRouteAdminStatus)
{
  L7_uint32 rowStatus;
  L7_RC_t rc = L7_SUCCESS;
  L7_ushort16 handle;

  
  switch (traceRouteAdminStatus)
  {
    case D_traceRouteCtlAdminStatus_enabled:

      rc = usmDbTraceRouteHandleGet( index,testName, &handle);

      if (rc == L7_SUCCESS)
      {
        rc = snmpTraceRouteRowStatusGet(handle,&rowStatus);

        if ((rc == L7_SUCCESS) && (rowStatus == D_traceRouteCtlRowStatus_active))
        {
          rc = usmDbTraceRouteStart(handle);
        }
        else
        {
          rc = L7_FAILURE;
        }
      }
      break;

    case D_traceRouteCtlAdminStatus_disabled:

      rc = usmDbTraceRouteHandleGet( index,testName, &handle);

      if (rc == L7_SUCCESS)
      {
        rc = snmpTraceRouteRowStatusGet(handle,&rowStatus);

        if ((rc == L7_SUCCESS) && (rowStatus == D_traceRouteCtlRowStatus_active))
        {
          rc = usmDbTraceRouteSessionEnd(handle);
        }
      }
      break;

    default :
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

/*********************************************************************
*  Set the Target Address field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    targetAddress   @b{(input)} Target Address
*
* @returns  L7_SUCCESS if Target Address is set
*           L7_FAIlURE Target Address is not set
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTraceRouteTargetAddressSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uchar8 *targetAddress)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 ipDa=0;
  L7_ushort16 handle;

 
  rc = usmDbTraceRouteHandleGet( index,testName, &handle); 
  if (rc == L7_SUCCESS)
  {
    rc = usmDbInetAton(targetAddress, &ipDa);
    if ( (rc == L7_SUCCESS) && (ipDa!= 0) )
    {
      rc = usmDbTraceRouteDestAddrSet(handle,ipDa);
    }
  }

  return rc;
}

/*********************************************************************
*  Set the Probe Count field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    dataSize @b{(input)} Probe Size
*
* @returns  L7_SUCCESS if Probe Size is set
*           L7_FAILURE if Probe Size is not set
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTraceRouteDataSizeSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 dataSize)
{
  L7_ushort16 handle;
  L7_RC_t rc;
  
  rc = usmDbTraceRouteHandleGet( index,testName, &handle);
  if (rc  == L7_SUCCESS)
  {
    rc = usmDbTraceRouteProbeSizeSet(handle,dataSize);
  }
 
  return rc;
}                                                                                             

/*********************************************************************
*  Set the TimeOut field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    timeOut @b{(input)} Probe Interval
*
* @returns  L7_SUCCESS if Time-Out is set
*           L7_FAILURE  Time-Out is not set
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTraceRouteTimeOutSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 timeOut)
{
  L7_ushort16 handle;
  L7_RC_t rc;
  
  rc = usmDbTraceRouteHandleGet( index,testName,&handle);

  if (rc  == L7_SUCCESS)
  {
    rc = usmDbTraceRouteProbeIntervalSet(handle,timeOut);
  }

  return rc;
}

/*********************************************************************
*  Set the Probes per Hop field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    probePerHop @b{(input)} Probes per Hop
*
* @returns  L7_SUCCESS if Probes per Hop is set
*           L7_FAILURE  Probes per Hop is not set
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTraceRouteProbePerHopSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 probePerHop)
{
  L7_ushort16 handle;
  L7_RC_t rc;
  
  rc = usmDbTraceRouteHandleGet( index,testName,&handle);

  if (rc  == L7_SUCCESS)
  {
    rc = usmDbTraceRouteProbePerHopSet(handle,probePerHop);
  }

  return rc;
}

/*********************************************************************
*  Set the Port field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    port @b{(input)}Destination Port
*
* @returns  L7_SUCCESS if port is set
*           L7_FAILURE if port is not set
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTraceRoutePortSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 port)
{
  L7_ushort16 handle;
  L7_RC_t rc;
  
  rc = usmDbTraceRouteHandleGet( index,testName,&handle);

  if (rc  == L7_SUCCESS)
  {
    rc = usmDbTraceRouteDestPortSet(handle,port);
  }
 
  return rc;
}

/*********************************************************************
*  Set the MaxTtl field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    maxTtl @b{(input)} MaxTtl
*
* @returns  L7_SUCCESS if maxTtl is set
*           L7_FAILURE if maxTtl is not set
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTraceRouteMaxTtlSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 maxTtl)
{
  L7_ushort16 handle;
  L7_RC_t rc;
  
  rc = usmDbTraceRouteHandleGet( index,testName,&handle);

  if (rc  == L7_SUCCESS)
  {
    rc = usmDbTraceRouteMaxTtlSet(handle,maxTtl);
  }
 
  return rc;
}

/*********************************************************************
*  Set the MaxFail field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    maxFail @b{(input)} MaxFail
*
* @returns  L7_SUCCESS if MaxFail is set
*           L7_FAILURE if MaxFail is not set
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTraceRouteMaxFailSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 maxFail)
{
  L7_ushort16 handle;
  L7_RC_t rc;
  
  rc = usmDbTraceRouteHandleGet( index,testName,&handle);

  if (rc  == L7_SUCCESS)
  {
    rc = usmDbTraceRouteMaxFailSet(handle,maxFail);
  }
 
  return rc;
}

/*********************************************************************
*  Set the DontFragment field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    dontFrag @b{(input)} DontFragment
*
* @returns  L7_SUCCESS if DontFragment is set
*           L7_FAILURE if DontFragment is not set
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTraceRouteDontFragmentSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 dontFrag)
{
  L7_ushort16 handle;
  L7_RC_t rc;
  
  rc = usmDbTraceRouteHandleGet( index,testName,&handle);

  if (rc  == L7_SUCCESS)
  {
    rc = usmDbTraceRouteDontFragSet(handle,dontFrag);
  }
 
  return rc;
}

/*********************************************************************
*  Set the InitTtl  field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    initTtl @b{(input)} initial Ttl 
*
* @returns  L7_SUCCESS if initTtl is set
*           L7_FAILURE if initTtl is not set
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTraceRouteInitTtlSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 initTtl)
{
  L7_ushort16 handle;
  L7_RC_t rc;
  
  rc = usmDbTraceRouteHandleGet( index,testName,&handle);

  if (rc  == L7_SUCCESS)
  {
    rc = usmDbTraceRouteInitTtlSet(handle,initTtl);
  }
 
  return rc;
}

