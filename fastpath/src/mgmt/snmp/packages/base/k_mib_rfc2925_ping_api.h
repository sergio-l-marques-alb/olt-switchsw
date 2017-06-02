/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_rfc2925_ping_api.h
*
* @purpose  Provide interface to TraceRoute MIB
*
* @component SNMP
*
* @comments
*
* @create 17/11/2006
*
* @author Sowjanya 
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include "sr_ip.h"
#include <usmdb_ping_api.h>
#include "l7_common.h"
#include "usmdb_ping_api.h"
#include "usmdb_util_api.h"


/*********************************************************************
*  Set the Row Status field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    pingRowStatus   @b{(input)} Row Status
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
snmpPingRowStatusSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 pingRowStatus,
                    ContextInfo *contextInfo)

{
  L7_RC_t rc = L7_SUCCESS;
  L7_ushort16 handle;
  L7_BOOL syncFlag = L7_FALSE;

  switch (pingRowStatus)
  {
    case D_pingCtlRowStatus_createAndGo:
      rc = usmDbPingSessionAlloc( index, testName,
          syncFlag, NULL, NULL, contextInfo, &handle );
      break;

    case D_pingCtlRowStatus_destroy:
      rc = usmDbPingSessionHandleFromIndexTestNameGet( index, testName, &handle);
      
      if(rc == L7_SUCCESS)
        rc = usmDbPingSessionFree( handle ); 
      break;

    case D_pingCtlRowStatus_active:
      break;

    default:
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

/*********************************************************************
*  Get the Row Status field
*
* @param    handle   @b{(input)} handle for the session
* @param    pingRowStatus   @b{(output)} Row Status
*
* @returns  L7_SUCCESS  
*
* @comments 
*            
*
* @end
*********************************************************************/
L7_RC_t
snmpPingRowStatusGet(L7_ushort16 handle,int *pingRowStatus)
{
  L7_uint32 ipDa;
    
  if(usmDbPingQueryTargetAddrs(handle, &ipDa)== L7_SUCCESS && ipDa != 0)
  {
    *pingRowStatus =  D_pingCtlRowStatus_active;
  } 
  else
  {
    *pingRowStatus =  D_pingCtlRowStatus_createAndGo;
  }
  
  return L7_SUCCESS;
}

/*********************************************************************
*  Get the Admin Status field
*
* @param    handle   @b{(input)} handle for the session
* @param    pingAdminStatus   @b{(output)} Admin Status
*
* @returns  L7_SUCCESS 
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpPingAdminStatusGet(L7_ushort16 handle,L7_uint32 *pingAdminStatus)
{
  L7_BOOL     operStatus;
  
  if(usmDbPingQueryOperStatus(handle, &operStatus) == L7_SUCCESS )
  {
    if(operStatus == L7_TRUE)
    {
      *pingAdminStatus = D_pingCtlAdminStatus_enabled ;
    } 
    else
    { 
      *pingAdminStatus = D_pingCtlAdminStatus_disabled;
    }  
  }

  return L7_SUCCESS;
}

/*********************************************************************
*  Set the Admin Status field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    pingAdminStatus   @b{(input)} Admin Status
*
* @returns  L7_SUCCESS if ping operation starts
*           L7_FAILURE if Row Status is not active or session is in progress. 
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpPingAdminStatusSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 pingAdminStatus)
{
  L7_uint32 rowStatus;
  L7_RC_t rc = L7_SUCCESS;
  L7_ushort16 handle;
  
  switch (pingAdminStatus)
  {
    case D_pingCtlAdminStatus_enabled:

      rc= usmDbPingSessionHandleFromIndexTestNameGet( index,testName, &handle);

      if (rc == L7_SUCCESS)
      {
        rc = snmpPingRowStatusGet(handle,&rowStatus);

        if ((rc == L7_SUCCESS) && (rowStatus == D_pingCtlRowStatus_active))
        {
          rc = usmDbPingSessionStart(handle); 
        }
        else
        {
          rc = L7_FAILURE;
        }
      }
      break;

    case D_pingCtlAdminStatus_disabled:
      
      rc= usmDbPingSessionHandleFromIndexTestNameGet( index,testName, &handle);

      if (rc == L7_SUCCESS)
      {
        rc = snmpPingRowStatusGet(handle,&rowStatus);

        if ((rc == L7_SUCCESS) && (rowStatus == D_pingCtlRowStatus_active))
        {
          rc = usmDbPingSessionEnd(handle);
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
snmpPingTargetAddressSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uchar8 *targetAddress)
{
   L7_RC_t rc = L7_FAILURE;
   L7_uint32 ipDa=0;
   L7_ushort16 handle;

  rc= usmDbPingSessionHandleFromIndexTestNameGet( index,testName, &handle);
  
    if (rc == L7_SUCCESS)
    {
     rc = usmDbInetAton(targetAddress, &ipDa);
      if ( (rc == L7_SUCCESS) && (ipDa != 0))
      rc = usmDbPingDestAddrSet(handle,ipDa);
    }
   
   return rc; 
}

/*********************************************************************
*  Set the Probe Count field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    probeCount @b{(input)} Probe Count
*
* @returns  L7_SUCCESS if Probe Count is set
*           L7_FAILURE if Probe Count is not set
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpPingProbeCountSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 probeCount)
{ 
  L7_ushort16 handle;
  L7_RC_t rc;

  rc = usmDbPingSessionHandleFromIndexTestNameGet( index,testName,&handle);

  if (rc  == L7_SUCCESS)
  {
    rc = usmDbPingProbeCountSet(handle,probeCount);
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
snmpPingTimeOutSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 timeOut)
{
  L7_ushort16 handle;
  L7_RC_t rc;
  
  rc = usmDbPingSessionHandleFromIndexTestNameGet( index,testName,&handle);

  if (rc  == L7_SUCCESS)
  {
    rc = usmDbPingProbeIntervalSet(handle,timeOut);
  }

  return rc;
}

/*********************************************************************
*  Set the Data Size field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    dataSize @b{(input)} Probe Size
*
* @returns  L7_SUCCESS if probe size is set
*           L7_FAILURE if pobe Size is not set
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpPingDataSizeSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 dataSize)
{
  L7_ushort16 handle;
  L7_RC_t rc;
  
  rc = usmDbPingSessionHandleFromIndexTestNameGet( index,testName,&handle);

  if (rc  == L7_SUCCESS)
  {
    rc = usmDbPingProbeSizeSet(handle,dataSize);
  }

  return rc;
}

