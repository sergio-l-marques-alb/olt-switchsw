/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename cable_status.c
*
* @purpose Pass status test commands through to dtl
*
* @component system
*
* @comments none
*
* @create 09/19/2002
*
* @author Jill Flanagan
*
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#include <l7_common.h>
#include <dtlapi.h>

typedef struct cableStatusIntfData_s
{
  L7_BOOL                tested;
  L7_PORT_CABLE_MEDIUM_t medium;
    struct
    {
      L7_CABLE_STATUS_t      status;
      L7_BOOL                lengthKnown;
      L7_uint32              shortestLength;
      L7_uint32              longestLength;
      L7_uint32              cableFailureLength;
      L7_clocktime           timestamp;
    } copper;
} cableStatusIntfData_t;

typedef struct fiberDiagData_s
{
  L7_uint32   intIfNum;
  L7_RC_t     retCode;
      L7_int32  temperature;
      L7_uint32 voltage;
      L7_uint32 current;
      L7_uint32 powerOut;
      L7_uint32 powerIn;
      L7_BOOL   txFault;
      L7_BOOL   los;
} fiberDiagData_t;

static fiberDiagData_t       fiberDiagData;
static cableStatusIntfData_t cableStatusIntfData[L7_MAX_PORT_COUNT];
static L7_BOOL               cableStatusDataInit = L7_FALSE;

static void checkGlobalData( void )
{
  if ( L7_FALSE == cableStatusDataInit )
  {
    bzero( (char *)cableStatusIntfData, sizeof( cableStatusIntfData ) );
    bzero( (char *)&fiberDiagData,      sizeof( fiberDiagData ) );
    fiberDiagData.retCode   = L7_NO_VALUE;
    fiberDiagData.intIfNum  = -1;
    cableStatusDataInit = L7_TRUE;
  }
}

/*********************************************************************
* @purpose  Returns the cable status
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
*
* @notes    Is only valid for copper cables
*
* @end
*********************************************************************/
L7_RC_t statusCableGet(L7_uint32               intIfNum,
                       L7_CABLE_STATUS_t      *cableStatus,
                       L7_BOOL                *lengthKnown,
                       L7_uint32              *shortestLength,
                       L7_uint32              *longestLength,
                       L7_uint32              *cableFailureLength,
                       L7_clocktime           *timestamp)
{
  L7_uint32 connType;
  L7_RC_t rc;

  checkGlobalData();

  if ( (L7_SUCCESS != nimCheckIfNumber( intIfNum ))  || (intIfNum >= L7_MAX_PORT_COUNT))
  {
    return( L7_ERROR );
  }

  if ( L7_SUCCESS != nimGetIntfConnectorType( intIfNum, &connType ))
  {
    return( L7_ERROR );
  }

  /*
   * Verify that the cable is copper.
   */
  if ( L7_RJ45 != connType )
  {
    return( L7_NOT_EXIST );
  }

  rc = dtlintfCableStatusGet(intIfNum,
                             &cableStatusIntfData[intIfNum].copper.status, 
                             &cableStatusIntfData[intIfNum].copper.lengthKnown, 
                             &cableStatusIntfData[intIfNum].copper.shortestLength, 
                             &cableStatusIntfData[intIfNum].copper.longestLength, 
                             &cableStatusIntfData[intIfNum].copper.cableFailureLength );

  if ( L7_SUCCESS != rc )
  {
    return( rc );
  }

  osapiUTCTimeGet( &cableStatusIntfData[intIfNum].copper.timestamp );
  cableStatusIntfData[intIfNum].tested = L7_TRUE;

  *cableStatus        = cableStatusIntfData[intIfNum].copper.status;
  *lengthKnown        = cableStatusIntfData[intIfNum].copper.lengthKnown;
  *shortestLength     = cableStatusIntfData[intIfNum].copper.shortestLength;
  *longestLength      = cableStatusIntfData[intIfNum].copper.longestLength;
  *cableFailureLength = cableStatusIntfData[intIfNum].copper.cableFailureLength;
  *timestamp          = cableStatusIntfData[intIfNum].copper.timestamp;

  return rc;
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
L7_RC_t cableStatusCopperResult( L7_uint32               intIfNum,
                                 L7_CABLE_STATUS_t      *cableStatus,
                                 L7_BOOL                *lengthKnown,
                                 L7_uint32              *shortestLength,
                                 L7_uint32              *longestLength,
                                 L7_uint32              *cableFailureLength,
                                 L7_clocktime           *timestamp)
{
  L7_uint32 connType;

  checkGlobalData();

  if ( (L7_SUCCESS != nimCheckIfNumber( intIfNum ))  || (intIfNum >= L7_MAX_PORT_COUNT))
  {
    return( L7_ERROR );
  }

  if ( L7_SUCCESS != nimGetIntfConnectorType( intIfNum, &connType ))
  {
    return( L7_ERROR );
  }

  /*
   * Verify that the cable is copper.
   */
  if ( L7_RJ45 != connType )
  {
    return( L7_NOT_EXIST );
  }

  if ( cableStatusIntfData[intIfNum].tested )
  {
    *cableStatus        = cableStatusIntfData[intIfNum].copper.status;
    *lengthKnown        = cableStatusIntfData[intIfNum].copper.lengthKnown;
    *shortestLength     = cableStatusIntfData[intIfNum].copper.shortestLength;
    *longestLength      = cableStatusIntfData[intIfNum].copper.longestLength;
    *cableFailureLength = cableStatusIntfData[intIfNum].copper.cableFailureLength;
    *timestamp          = cableStatusIntfData[intIfNum].copper.timestamp;
  }
  else
  {
    *cableStatus = L7_CABLE_UNTESTED;
  }

  return( L7_SUCCESS );
}


/*********************************************************************
* @purpose  Get the fiber diagnostics values and returns the result
*
* @param    L7_uint32     intIfNum    @b((input))  Internal interface number
* @param    L7_int32      temperature @b{(output)} degrees C
* @param    L7_uint32     voltage     @b{(output)} milliVolts
* @param    L7_uint32     current     @b{(output)} milliAmps
* @param    L7_uint32     powerOut    @b{(output)} microWatts
* @param    L7_uint32     powerIn     @b{(output)} microWatts
* @param    L7_uint32     txFault     @b((output)) true or false
* @param    L7_uint32     LOS         @b((output)) true or false
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist or is not copper
* @returns  L7_FAILURE  if other failure
*
* @notes    Is only valid for Fiber cables
*           If the link is not tested, then all other outputs are undefined
*
* @end
*********************************************************************/
L7_RC_t cableStatusFiberTest( L7_int32  intIfNum,
                              L7_int32 *temperature,
                              L7_uint32 *voltage,
                              L7_uint32 *current,
                              L7_uint32 *powerOut,
                              L7_uint32 *powerIn,
                              L7_BOOL   *txFault,
                              L7_BOOL   *los )
{
  L7_PORT_CABLE_MEDIUM_t medium;

  checkGlobalData();

  if ( L7_SUCCESS != nimCheckIfNumber( intIfNum ) )
  {
    return( L7_ERROR );
  }

  fiberDiagData.retCode = dtlIntfCableFiberDiagGet(intIfNum, 
                                                   &medium,
                                                   &fiberDiagData.temperature,
                                                   &fiberDiagData.voltage,
                                                   &fiberDiagData.current,
                                                   &fiberDiagData.powerOut,
                                                   &fiberDiagData.powerIn,
                                                   &fiberDiagData.txFault,
                                                   &fiberDiagData.los );

  fiberDiagData.intIfNum = intIfNum;

  if ( L7_SUCCESS != fiberDiagData.retCode )
  {
    fiberDiagData.temperature = 0;
    fiberDiagData.voltage     = 0;
    fiberDiagData.current     = 0;
    fiberDiagData.powerOut    = 0;
    fiberDiagData.powerIn     = 0;
    fiberDiagData.txFault     = L7_TRUE;
    fiberDiagData.los         = L7_TRUE;
  }

  *temperature = fiberDiagData.temperature;
  *voltage     = fiberDiagData.voltage;
  *current     = fiberDiagData.current;
  *powerOut    = fiberDiagData.powerOut;
  *powerIn     = fiberDiagData.powerIn;
  *txFault     = fiberDiagData.txFault;
  *los         = fiberDiagData.los;

  return( fiberDiagData.retCode );
}

/*********************************************************************
* @purpose  Get the fiber diagnostics values and returns the result
*
* @param    L7_uint32     intIfNum    @b((input))  Internal interface number
* @param    L7_int32      temperature @b{(output)} degrees C
* @param    L7_uint32     voltage     @b{(output)} milliVolts
* @param    L7_uint32     current     @b{(output)} milliAmps
* @param    L7_uint32     powerOut    @b{(output)} microWatts
* @param    L7_uint32     powerIn     @b{(output)} microWatts
* @param    L7_uint32     txFault     @b((output)) true or false
* @param    L7_uint32     los         @b((output)) true or false
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist or is not copper
* @returns  L7_FAILURE  if other failure
*
* @notes    Is only valid for Fiber cables
*           If the link is not tested, then all other outputs are undefined
*
* @end
*********************************************************************/
L7_RC_t cableStatusFiberResult( L7_uint32  intIfNum,
                                L7_int32  *temperature,
                                L7_uint32 *voltage,
                                L7_uint32 *current,
                                L7_uint32 *powerOut,
                                L7_uint32 *powerIn,
                                L7_BOOL   *txFault,
                                L7_BOOL   *los )
{
  static  L7_RC_t     rc = L7_ERROR;

  checkGlobalData();

  rc = nimCheckIfNumber( intIfNum );
  if ( L7_SUCCESS == rc)
  {
    if (intIfNum != fiberDiagData.intIfNum)
    {
      rc = cableStatusFiberTest(intIfNum,
                                temperature,
                                voltage,
                                current,
                                powerOut,
                                powerIn,
                                txFault,
                                los);
    }
    else 
    {
      *temperature = fiberDiagData.temperature;
      *voltage     = fiberDiagData.voltage;    
      *current     = fiberDiagData.current;    
      *powerOut    = fiberDiagData.powerOut;   
      *powerIn     = fiberDiagData.powerIn;    
      *txFault     = fiberDiagData.txFault;    
      *los         = fiberDiagData.los;        
      rc           = fiberDiagData.retCode;
    }
  }

  return( rc );
}
