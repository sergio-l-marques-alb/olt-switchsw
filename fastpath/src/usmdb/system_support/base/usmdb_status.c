/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\usmdb\usmdb_status.c
*
* @purpose Provide interface to status component
*
* @component usmdb
*
* @comments none
*
* @create 09/19/2002
*
* @author Jill Flanagan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#include <math.h>
#include "l7_common.h"
#include "usmdb_nim_api.h"
#include "cable_status.h"
#include "dtlapi.h"
#include "usmdb_nim_api.h"
#include "usmdb_status.h"

/*********************************************************************
*
* @purpose Test the status of the cable
* 
* @param L7_uint32 intIfNum @b((input))  
* @param L7_CABLE_STATUS_t *cableStatus        @b((output))  
* @param L7_BOOL   *lengthKnown @b((output))  
* @param L7_uint32 *shortestLength @b((output))  
* @param L7_uint32 *longestLength @b((output))  
* @param L7_uint32 *cableFailureLength @b((output))  
* @param L7_clocktime      *timestamp          @b((output))  
*
* @returns L7_SUCCESS
*          L7_ERROR - the cable is not of the proper type for the test
*                      or the interface does not exist
*          L7_FAILURE
*
* @notes Only valid for copper cables
*
* @end
*********************************************************************/
extern L7_RC_t usmDbStatusCableGet(L7_uint32          intIfNum, 
                                   L7_CABLE_STATUS_t *cableStatus,
                                   L7_BOOL           *lengthKnown, 
                                   L7_uint32         *shortestLength,
						           L7_uint32         *longestLength, 
                                   L7_uint32         *cableFailureLength,
                                   L7_clocktime      *timestamp)
{
  return statusCableGet(intIfNum, cableStatus,lengthKnown, shortestLength,
                        longestLength, cableFailureLength, timestamp);
}

/*********************************************************************
* @purpose  Returns the result of the latest test, if any.
*
* @param L7_uint32          intIfNum           @b((input))  
* @param L7_CABLE_STATUS_t *cableStatus        @b((output))  
* @param L7_BOOL           *lengthKnown        @b((output))  
* @param L7_uint32         *shortestLength     @b((output))  
* @param L7_uint32         *longestLength      @b((output))  
* @param L7_uint32         *cableFailureLength @b((output))  
* @param L7_clocktime      *timestamp          @b((output))  
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
* @returns  L7_NOT_EXIST  if not copper
*
* @notes    Is only valid for copper cables
*           If the link is not tested, then all other outputs are undefined
*
* @end
*********************************************************************/
extern L7_RC_t usmDbCableStatusCopperResult( L7_uint32               intIfNum,
                                             L7_CABLE_STATUS_t      *cableStatus,
                                             L7_BOOL                *lengthKnown, 
                                             L7_uint32              *shortestLength,
                                             L7_uint32              *longestLength, 
                                             L7_uint32              *cableFailureLength,
                                             L7_clocktime           *timestamp)
{
  return cableStatusCopperResult(intIfNum,
                                 cableStatus,
                                 lengthKnown,
                                 shortestLength,
                                 longestLength,
                                 cableFailureLength,
                                 timestamp);
}

/*********************************************************************
* @purpose  Tests the Fiber cable and returns the result
*
* @param    L7_uint32     intIfNum    @b((input))  Internal interface number
* @param    L7_int32      temperature @b{(output)} millidegrees C
* @param    L7_uint32     voltage     @b{(output)} milliVolts
* @param    L7_uint32     current     @b{(output)} milliAmps
* @param    L7_double64   powerOut    @b{(output)} decibals relative to 1mW
* @param    L7_double64   powerIn     @b{(output)} decibals relative to 1mW
* @param    L7_uint32     txFault     @b((output)) true or false
* @param    L7_uint32     los         @b((output)) true or false
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist or is not copper
* @returns  L7_FAILURE  if other failure
*
* @notes    Is only valid for copper cables
*           If the link is not tested, then all other outputs are undefined
*
* @end
*********************************************************************/
L7_RC_t usmDbCableStatusFiberTest( L7_uint32  intIfNum,
                                   L7_int32  *temperature,
                                   L7_uint32 *voltage,
                                   L7_uint32 *current,
                                   L7_double64 *powerOut,
                                   L7_double64 *powerIn,
                                   L7_BOOL   *txFault,
                                   L7_BOOL   *los )
{
  L7_NIM_QUERY_DATA_t queryData;
  L7_uint32 txPower;
  L7_uint32 rxPower;
  L7_uint32 adminState;
  L7_RC_t   rc;

  /* make sure this port is attached */
  /* get the interface state */
  queryData.intIfNum = intIfNum;
  queryData.request = L7_NIM_QRY_RQST_STATE;

  if (nimIntfQuery(&queryData) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (queryData.data.state != L7_INTF_ATTACHED)
  {
    return L7_FAILURE;
  }

  rc = cableStatusFiberTest( intIfNum, temperature, voltage, current, &txPower,
                             &rxPower, txFault, los );

  *powerOut = -40;            /* -40 is the spec absolute minimum */
  *powerIn  = -40;            /* -40 is the spec absolute minimum */

  if (L7_SUCCESS == rc)
  {
      /* Will calibrate powerOut only when port is not administratively disabled*/
    if ((nimGetIntfMgmtAdminState(intIfNum, &adminState) == L7_SUCCESS) && (adminState != L7_DISABLE))
    {
      if (0 != txPower)
      {
        *powerOut = 10 * log10((L7_double64)txPower/10000);
      }
    }
    if (0 != rxPower)
    {
      *powerIn  = 10 * log10((L7_double64)rxPower/10000);
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose   Returns the result of the latest test, if any.
*
* @param    L7_uint32     intIfNum    @b((input))  Internal interface number
* @param    L7_int32      temperature @b{(output)} millidegrees C
* @param    L7_uint32     voltage     @b{(output)} milliVolts
* @param    L7_uint32     current     @b{(output)} milliAmps
* @param    L7_double64   powerOut    @b{(output)} decibals relative to 1mW
* @param    L7_double64   powerIn     @b{(output)} decibals relative to 1mW
* @param    L7_uint32     txFault     @b((output)) true or false
* @param    L7_uint32     los         @b((output)) true or false
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist or is not copper
* @returns  L7_FAILURE  if other failure
*
* @notes    Is only valid for copper cables
*           If the link is not tested, then all other outputs are undefined
*
* @end
*********************************************************************/
L7_RC_t usmDbCableStatusFiberResult( L7_uint32  intIfNum,
                                     L7_int32 *temperature,
                                     L7_uint32 *voltage,
                                     L7_uint32 *current,
                                     L7_double64 *powerOut,
                                     L7_double64 *powerIn,
                                     L7_BOOL   *txFault,
                                     L7_BOOL   *los )
{
  L7_NIM_QUERY_DATA_t queryData;
  L7_uint32 txPower;
  L7_uint32 rxPower;
  L7_RC_t   rc;

  /* make sure this port is attached */
  /* get the interface state */
  queryData.intIfNum = intIfNum;
  queryData.request = L7_NIM_QRY_RQST_STATE;

  if (nimIntfQuery(&queryData) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (queryData.data.state != L7_INTF_ATTACHED)
  {
    return L7_FAILURE;
  }

  rc = cableStatusFiberResult( intIfNum, temperature, voltage, current, &txPower, 
                               &rxPower, txFault, los );

  *powerOut = -40;            /* -40 is the spec absolute minimum */
  *powerIn  = -40;            /* -40 is the spec absolute minimum */

  if (L7_SUCCESS == rc)
  {
    if (0 != txPower) 
    {
      *powerOut = 10 * log10((L7_double64)txPower/10000);
    }

    if (0 != rxPower) 
    {
      *powerIn  = 10 * log10((L7_double64)rxPower/10000);
    }
  }

  return(rc);

}

/*********************************************************************
*
* @purpose Test the link of the SFP on a combo port
* 
* @param L7_uint32 UnitIndex @b((input))         
* @param L7_uint32 intIfNum @b((input))  
* @param L7_BOOL  *sfpLinkUp @b((output))  
*
* @returns L7_SUCCESS
*          L7_ERROR - the port is not a combo port
*                      or the interface does not exist
*          L7_FAILURE
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t usmDbStatusSFPLinkUpGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_BOOL *sfpLinkUp)
{
    return dtlintfSfpLinkUpGet( intIfNum, sfpLinkUp );
}
