/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basePortMirroring.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  15 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basePortMirroring_obj.h"
#include "usmdb_mirror_api.h"


int getPortFromMask(L7_INTF_MASK_t intfmask)
{
  int i = 0;
  int j = 0;

  for(i=0;i< sizeof(intfmask); i++)
  {
    for(j = 0; j<sizeof(char)*8;j++)
    {
      if((intfmask.value[i] & (0x01<<j)) != 0x0)
        return i*8+j+1;
    }
  }
  return -1;
}

L7_BOOL isSet(L7_INTF_MASK_t* intfMask,int pos)
{
  if((intfMask->value[pos/8] & (1 << pos)) !=0)
  {
    return L7_TRUE;
  }
  else
    return L7_FALSE;
}

int getNextIntf(L7_INTF_MASK_t* intfMask,xLibS32_t prevPort)
{
  int i = 0;
  int j = 0;
  for( i = (prevPort)/8;i< sizeof(L7_INTF_MASK_t);i++)
  {
    for(j = (prevPort)%8,prevPort=0; j< sizeof(char)*8;j++)
    {
      if((intfMask->value[i] & (0x01 <<j)) !=0)
        return i*8+j+1;
    }
  }
  return -1;

}


/*******************************************************************************
* @function fpObjGet_basePortMirroring_PortMirrorSessionID
*
* @purpose Get 'PortMirrorSessionID'
*
* @description The Session number of this mirroring entry. The number of sessions 
*              is fixed, and is platform dependant. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortMirroring_PortMirrorSessionID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortMirrorSessionIDValue;
  xLibU32_t nextObjPortMirrorSessionIDValue;

  xLibU32_t sessionCount;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortMirrorSessionID */
  owa.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
                          (xLibU8_t *) & objPortMirrorSessionIDValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjPortMirrorSessionIDValue = L7_DEFAULT_MIRROR_SESSION_ID;    
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortMirrorSessionIDValue, owa.len);	
    sessionCount = usmDbSwPortMonitorSessionCount(L7_UNIT_CURRENT);
    nextObjPortMirrorSessionIDValue = objPortMirrorSessionIDValue+1;
		
    if (nextObjPortMirrorSessionIDValue >= 1 &&
                nextObjPortMirrorSessionIDValue <= sessionCount)
    {
       owa.l7rc = L7_SUCCESS;
    }
    else
    {
  	   owa.l7rc = L7_FAILURE;
    }
  }
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPortMirrorSessionIDValue, owa.len);

  /* return the object value: PortMirrorSessionID */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPortMirrorSessionIDValue,
                    sizeof (objPortMirrorSessionIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basePortMirroring_PortMirrorDestinationPort
*
* @purpose Get 'PortMirrorDestinationPort'
*
* @description The port which traffic from the mirrored ports will be sent to. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortMirroring_PortMirrorDestinationPort (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortMirrorSessionIDValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortMirrorDestinationPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortMirrorSessionID */
  kwa.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
                          (xLibU8_t *) & keyPortMirrorSessionIDValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortMirrorSessionIDValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSwPortMonitorDestPortGet (L7_UNIT_CURRENT,
                                   keyPortMirrorSessionIDValue,
                                   &objPortMirrorDestinationPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PortMirrorDestinationPort */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPortMirrorDestinationPortValue,
                    sizeof (objPortMirrorDestinationPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_TestuspRange_PortMirrorDestinationPort
*
* @purpose Set 'PortMirrorDestinationPort'
*
* @description The port which traffic from the mirrored ports will be sent to. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjList_basePortMirroring_PortMirrorDestinationPort (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortMirrorDestinationPortValue;
  xLibU32_t nextObjPortMirrorDestinationPortValue;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortMirrorSessionID */
  owa.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorDestinationPort,
                          (xLibU8_t *) &objPortMirrorDestinationPortValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	nextObjPortMirrorDestinationPortValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortMirrorDestinationPortValue, owa.len);
	owa.l7rc = usmDbSwPortMonitorValidDestIntfNextGet (
		                   objPortMirrorDestinationPortValue, 
		                   &nextObjPortMirrorDestinationPortValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPortMirrorDestinationPortValue, owa.len);

  /* return the object value: PortMirrorSessionID */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPortMirrorDestinationPortValue,
                    sizeof (objPortMirrorDestinationPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_basePortMirroring_PortMirrorDestinationPort
*
* @purpose Set 'PortMirrorDestinationPort'
*
* @description The port which traffic from the mirrored ports will be sent to. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basePortMirroring_PortMirrorDestinationPort(void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortMirrorDestinationPortValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortMirrorSessionIDValue;
  xLibU32_t tempIfNum;
  L7_INTF_MASK_t intfMask;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PortMirrorDestinationPort */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPortMirrorDestinationPortValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortMirrorDestinationPortValue, owa.len);

  /* retrieve key: PortMirrorSessionID */
  kwa.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
                          (xLibU8_t *) & keyPortMirrorSessionIDValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortMirrorSessionIDValue, kwa.len);
  owa.l7rc = usmDbSwPortMonitorSourcePortsGet(L7_UNIT_CURRENT, keyPortMirrorSessionIDValue, &intfMask);

  if((objPortMirrorDestinationPortValue !=0 ) &&(isSet(&intfMask,objPortMirrorDestinationPortValue-1) == L7_TRUE))
  {
    return XLIBRC_PORTMIRROR_SOURCE_DEST_SAME;
  }
  
  /* set the value in application */
  if(usmDbSwPortMonitorDestPortGet(L7_UNIT_CURRENT,
		               keyPortMirrorSessionIDValue,
		               &tempIfNum)==L7_SUCCESS)
 {
 	usmDbSwPortMonitorDestPortRemove(L7_UNIT_CURRENT,
		              keyPortMirrorSessionIDValue);
 }
  if(objPortMirrorDestinationPortValue != 0)
  {		
    owa.l7rc = usmDbSwPortMonitorDestPortSet (L7_UNIT_CURRENT, keyPortMirrorSessionIDValue,
                                   objPortMirrorDestinationPortValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjGet_basePortMirroring_SourcePort
*
* @purpose Get 'SourcePort listing'
*
* @description The source port list. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjList_basePortMirroring_PortMirrorSourcePort(void *wap, void *bufp)
{
  L7_RC_t rv;
  L7_INTF_MASK_t sourcePortMask;
  L7_uint32 sourcePort; 
  L7_uint32 nextPort;
  L7_INTF_MASK_t mask;
  
  xLibU16_t bufLen = sizeof (sourcePortMask);
  memset(&sourcePortMask,0x0,sizeof(sourcePortMask)); 
  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSourcePort,
        (xLibU8_t *) &sourcePortMask, &bufLen))
  {
    rv = usmDbSwPortMonitorValidSrcIntfFirstGet (&nextPort);
    if( rv == L7_SUCCESS )
    {
      if(usmDbSwPortMonitorIsSrcConfigured(L7_UNIT_CURRENT,nextPort) == L7_TRUE)
      {
         sourcePort = nextPort;
         while((rv=usmDbSwPortMonitorValidSrcIntfNextGet (sourcePort, &nextPort)) != L7_FAILURE) 
         {
           if( usmDbSwPortMonitorIsSrcConfigured(L7_UNIT_CURRENT,nextPort) == L7_FALSE)
           {
              break;
           }
           sourcePort = nextPort;
         };
         if(rv==L7_FAILURE)
         {
           rv = L7_FAILURE; 
         }
      }
    }
    else
    {
      rv = L7_FAILURE;
    }
  }
  else
  {
    sourcePort = getPortFromMask(sourcePortMask);   
    if(sourcePort != -1)
    {
      while((rv=usmDbSwPortMonitorValidSrcIntfNextGet (sourcePort, &nextPort)) != L7_FAILURE)
      {
        if( usmDbSwPortMonitorIsSrcConfigured(L7_UNIT_CURRENT,nextPort) == L7_FALSE)
        {
           break;
        }
        sourcePort = nextPort;
      };
      if(rv==L7_FAILURE) 
      {
        rv = L7_FAILURE; 
      }
    }
    else
    {
      rv = L7_FAILURE;
    }
  }

  if (rv != L7_SUCCESS)
  {
    memset(&mask,0x0,sizeof(mask));
    xLibBufDataSet (bufp, (xLibU8_t *) &mask, sizeof (mask));
    return XLIBRC_ENDOF_TABLE;
  }
  if(0 != nextPort)
  {
    memset(&mask,0x0,sizeof(mask));
    L7_INTF_SETMASKBIT(mask,nextPort);
    xLibBufDataSet (bufp, (xLibU8_t *) &mask, sizeof (mask));
    return XLIBRC_SUCCESS;
  }
  return XLIBRC_FAILURE;
}

/*******************************************************************************
* @function fpObjGet_basePortMirroring_PortMirrorSourcePort
*
* @purpose Get 'PortMirrorSourcePort'
*
* @description The ports from which traffic will be sent to the destination port. 
*              The destination port can not be included in this list of 
*              ports. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortMirroring_PortMirrorSourcePort (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwaSessionId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaSourcePort= FPOBJ_INIT_WA (sizeof (L7_INTF_MASK_t));
  xLibS32_t  keySessionIdValue;
  xLibS32_t sourcePortFirst;
  xLibS32_t keySourcePortValue;
  xLibS32_t nextSourcePort;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  L7_INTF_MASK_t srcPortMask;
  L7_INTF_MASK_t returnPortMask;
  L7_INTF_MASK_t filterPortMask;

  memset(&srcPortMask,0x0,sizeof(srcPortMask));
  memset(&returnPortMask,0x0,sizeof(returnPortMask));
  memset(&filterPortMask,0x0,sizeof(filterPortMask));
  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: SessionId */
  kwaSessionId.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
      (xLibU8_t *) & keySessionIdValue, &kwaSessionId.len);
  if (kwaSessionId.rc != XLIBRC_SUCCESS)
  {
    kwaSessionId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSessionId);
    return kwaSessionId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIdValue, kwaSessionId.len);

  kwaSourcePort.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSourcePort,
      (xLibU8_t *) &filterPortMask, &kwaSourcePort.len);
  if (kwaSourcePort.rc != XLIBRC_SUCCESS)
  {

    owa.l7rc = usmDbSwPortMonitorSourcePortsGet(L7_UNIT_CURRENT, keySessionIdValue,
        &srcPortMask);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    sourcePortFirst = getNextIntf(&srcPortMask,0);
    if(sourcePortFirst > -1) 
    {
      L7_INTF_SETMASKBIT(returnPortMask,sourcePortFirst);
      owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)&returnPortMask,sizeof(returnPortMask));
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
/*
here we need to loop through the all port in the mask and 
return one by one value.This logic can be moved to usmDb too
*/
  keySourcePortValue = getPortFromMask(filterPortMask);
  if(keySourcePortValue == -1)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;

  }

  owa.l7rc = usmDbSwPortMonitorSourcePortsGet(L7_UNIT_CURRENT, keySessionIdValue,
      &srcPortMask);
  if (owa.l7rc != L7_SUCCESS)
  {

    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextSourcePort = getNextIntf(&srcPortMask,keySourcePortValue);
  if(nextSourcePort > -1 )
  {

    L7_INTF_SETMASKBIT(returnPortMask,nextSourcePort);
    owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)&returnPortMask,sizeof(returnPortMask));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_basePortMirroring_PortMirrorAdminMode
*
* @purpose Get 'PortMirrorAdminMode'
*
* @description The status of this port mirroring session. enable(1) - This session 
*              is active and all traffic from the source ports will be 
*              mirrored to the destination port. disable(2) - This session 
*              is not active. delete(3) - Remove the configuration for this 
*              Session 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortMirroring_PortMirrorAdminMode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortMirrorSessionIDValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortMirrorAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortMirrorSessionID */
  kwa.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
                          (xLibU8_t *) & keyPortMirrorSessionIDValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortMirrorSessionIDValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbSwPortMonitorModeGet (L7_UNIT_CURRENT, keyPortMirrorSessionIDValue,
                              &objPortMirrorAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PortMirrorAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortMirrorAdminModeValue,
                           sizeof (objPortMirrorAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basePortMirroring_PortMirrorAdminMode
*
* @purpose Set 'PortMirrorAdminMode'
*
* @description The status of this port mirroring session. enable(1) - This session 
*              is active and all traffic from the source ports will be 
*              mirrored to the destination port. disable(2) - This session 
*              is not active. delete(3) - Remove the configuration for this 
*              Session 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basePortMirroring_PortMirrorAdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortMirrorAdminModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortMirrorSessionIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PortMirrorAdminMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPortMirrorAdminModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortMirrorAdminModeValue, owa.len);

  /* retrieve key: PortMirrorSessionID */
  kwa.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
                          (xLibU8_t *) & keyPortMirrorSessionIDValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortMirrorSessionIDValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSwPortMonitorModeSet(L7_UNIT_CURRENT,
                                                                keyPortMirrorSessionIDValue,
                                                                objPortMirrorAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basePortMirroring_Direction
*
* @purpose Get 'PortMirrorDirection'
*
* @description The direction of traffic to be mirrored which 
*                     can be ingress or egress or both 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortMirroring_PortMirrorDirection (void *wap, void *bufp)
{
  xLibRC_t rc;
  L7_RC_t rv;
  L7_int32 intIfNum;
  L7_uint32 sessionId;
  xLibU16_t len;
  xLibU16_t sidLen;
  L7_MIRROR_DIRECTION_t direction;
  L7_INTF_MASK_t intfMask;
  memset(&intfMask,0x0,sizeof(intfMask));
  sidLen = sizeof (sessionId);
  rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID, (xLibU8_t *) & sessionId, &sidLen);
  if (rc != XLIBRC_SUCCESS)
  {
    return XLIBRC_FILTER_MISSING;
  }

  len = sizeof (intfMask);
  rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSourcePort, (xLibU8_t *) & intfMask, &len);
  if (rc != XLIBRC_SUCCESS)
  {
    return XLIBRC_FILTER_MISSING;
  }
  intIfNum = getPortFromMask(intfMask); 

  if(intIfNum == -1)
  {
    return XLIBRC_FILTER_MISSING;
  } 
  rv = usmDbMirrorSourcePortDirectionGet (sessionId, intIfNum, &direction);
  return xLibBufDataSet (bufp, (xLibU8_t *) & direction, sizeof (direction));
}

/*******************************************************************************
* @function fpObjSet_basePortMirroring_PortMirrorDirection
*
* @purpose Set 'PortMirrorDirection'
*
* @description The direction of traffic to be mirrored which 
*                     can be ingress or egress or both 
*              
* @return
*******************************************************************************/


xLibRC_t fpObjSet_basePortMirroring_PortMirrorDirection (void *wap, void *bufp)
{
  xLibRC_t rc;
  L7_RC_t rv;
  L7_int32 intfNum;
  L7_int32 tempIfNum;
  L7_uint32 sessionId;
  xLibU16_t len;
  xLibU16_t sidLen;
  L7_MIRROR_DIRECTION_t direction;
  L7_INTF_MASK_t intfMask;

  sidLen = sizeof (sessionId);
  memset(&intfMask,0x0,sizeof(intfMask));
  rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID, (xLibU8_t *) & sessionId, &sidLen);
  if (rc != XLIBRC_SUCCESS)
  {
    return XLIBRC_FILTER_MISSING;
  }

  len = sizeof (intfMask);
  rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSourcePort, (xLibU8_t *) & intfMask, &len);
  if (rc != XLIBRC_SUCCESS)
  {
    return XLIBRC_FILTER_MISSING;
  }  

  usmDbSwPortMonitorDestPortGet(L7_UNIT_CURRENT, sessionId, &tempIfNum);
  if((tempIfNum !=0) && (isSet(&intfMask,tempIfNum-1) == L7_TRUE))
  {
    return XLIBRC_PORTMIRROR_SOURCE_DEST_SAME;
  }
  len = sizeof (direction);
  rc = xLibBufDataGet (bufp, (xLibU8_t *) & direction, &len);
  if (rc != XLIBRC_SUCCESS)
  {
    return XLIBRC_FILTER_MISSING;
  }
  intfNum = getNextIntf(&intfMask,0);
  if(intfNum == -1)
  {
    return XLIBRC_FILTER_MISSING;

  }
  while(intfNum != -1)
  {
    if(direction==L7_MIRROR_UNCONFIGURED)
      rv=usmDbSwPortMonitorSourcePortRemove(L7_USMDB_UNIT_ZERO, sessionId,intfNum);
    else
      rv = usmDbSwPortMonitorSourcePortAdd (L7_USMDB_UNIT_ZERO, sessionId, intfNum, direction);

    if(rv!=L7_SUCCESS)
      return XLIBRC_FAILURE;
    intfNum = getNextIntf(&intfMask,intfNum);
  }
  return XLIBRC_SUCCESS;
}



/*******************************************************************************
* @function fpObjSet_basePortMirroring_PortMonitorDestPortRemove
*
* @purpose Set 'PortMonitorDestPortRemove'
*
* @description Remove Destination Port for session sessionNum 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basePortMirroring_PortMonitorDestPortRemove (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortMonitorDestPortRemoveValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortMirrorSessionIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PortMonitorDestPortRemove */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPortMonitorDestPortRemoveValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortMonitorDestPortRemoveValue, owa.len);

  /* retrieve key: PortMirrorSessionID */
  kwa.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
                          (xLibU8_t *) & keyPortMirrorSessionIDValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortMirrorSessionIDValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSwPortMonitorDestPortRemove (L7_UNIT_CURRENT,
                                      keyPortMirrorSessionIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjGet_basePortMirroring_PortMirrorDirectionKey (void *wap,
                                                               void *bufp)
{  
  fpObjWa_t kwaSessionId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaDirectionKey = FPOBJ_INIT_WA (sizeof (L7_MIRROR_DIRECTION_t));
  xLibU32_t  keySessionIdValue;
  L7_MIRROR_DIRECTION_t directionKeyFirst;
  L7_MIRROR_DIRECTION_t directionKeyValue;
  L7_MIRROR_DIRECTION_t directionKeyNext;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: SessionId */
  kwaSessionId.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
      (xLibU8_t *) & keySessionIdValue, &kwaSessionId.len);
  if (kwaSessionId.rc != XLIBRC_SUCCESS)
  {
    kwaSessionId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSessionId);
    return kwaSessionId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIdValue, kwaSessionId.len);

  kwaDirectionKey.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorDirectionKey,
      (xLibU8_t *) &directionKeyValue, &kwaDirectionKey.len);
  if (kwaDirectionKey.rc != XLIBRC_SUCCESS)
  {
    directionKeyFirst = L7_MIRROR_BIDIRECTIONAL;
    owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)&directionKeyFirst,sizeof(directionKeyFirst));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
/*
here we need to loop through the all port in the mask and 
return one by one value.This logic can be moved to usmDb too
*/
  owa.rc = XLIBRC_SUCCESS;
  switch(directionKeyValue)
  {
    case L7_MIRROR_UNCONFIGURED:
       directionKeyNext = L7_MIRROR_BIDIRECTIONAL;
    break;
    case L7_MIRROR_BIDIRECTIONAL:
       directionKeyNext = L7_MIRROR_INGRESS;
    break;
    case L7_MIRROR_INGRESS:
       directionKeyNext = L7_MIRROR_EGRESS;
    break;
    case L7_MIRROR_EGRESS:
    default:
       owa.rc = XLIBRC_ENDOF_TABLE;
       break;
  }
  if(owa.rc != XLIBRC_ENDOF_TABLE)
  {
    owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)&directionKeyNext,sizeof(directionKeyNext));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
}

xLibRC_t fpObjList_basePortMirroring_PortMirrorSourcePortConfigured (void *wap,
                                                               void *bufp)
{  

  L7_RC_t rv;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaSessionId = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t  keySessionIdValue;
  L7_INTF_MASK_t configuredSrcPortMask;
  L7_INTF_MASK_t filterSourcePortMask;
  L7_INTF_MASK_t returnMask;
  xLibS32_t sourcePort; 
  xLibS32_t keySourcePortValue;

  xLibU16_t bufLen = sizeof (filterSourcePortMask);
  memset(&filterSourcePortMask,0x0,sizeof(filterSourcePortMask)); 
  memset(&returnMask,0x0,sizeof(returnMask)); 

  kwaSessionId.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
      (xLibU8_t *) & keySessionIdValue, &kwaSessionId.len);
  if (kwaSessionId.rc != XLIBRC_SUCCESS)
  {
    return kwaSessionId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIdValue, kwaSessionId.len);

  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSourcePortConfigured,
        (xLibU8_t *) &filterSourcePortMask, &bufLen))
  {    
    owa.l7rc = usmDbSwPortMonitorSourcePortsGet(L7_UNIT_CURRENT, keySessionIdValue, &configuredSrcPortMask);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    sourcePort = getNextIntf(&configuredSrcPortMask,0);
    if( sourcePort != -1)
    {
      rv = L7_SUCCESS;
    }
    else
    {
      rv = L7_FAILURE;
    }
  }
  else
  {
     keySourcePortValue = getPortFromMask(filterSourcePortMask);
     if(keySourcePortValue == -1)
     {
       owa.rc = XLIBRC_ENDOF_TABLE;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }

    owa.l7rc = usmDbSwPortMonitorSourcePortsGet(L7_UNIT_CURRENT, keySessionIdValue, &configuredSrcPortMask);
    sourcePort = getNextIntf(&configuredSrcPortMask,keySourcePortValue);
    if(sourcePort != -1)
    { 
      rv = L7_SUCCESS;
    }
    else
    {
      rv = L7_FAILURE;
    }
  }

  if (rv != L7_SUCCESS)
  {
    return XLIBRC_ENDOF_TABLE;
  }
  
  L7_INTF_SETMASKBIT(returnMask,sourcePort);
  owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)&returnMask,sizeof(returnMask));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjGet_basePortMirroring_PortMirrorSourcePortConfigured (void *wap,
                                                               void *bufp)
{  
  fpObjWa_t kwaSessionId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaDirectionKey = FPOBJ_INIT_WA (sizeof (L7_MIRROR_DIRECTION_t));
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t  keySessionIdValue;
  L7_MIRROR_DIRECTION_t directionKeyValue;
  L7_MIRROR_DIRECTION_t direction;
  L7_INTF_MASK_t configuredSrcPortMask;
  L7_INTF_MASK_t returnPortMask;
  L7_INTF_MASK_t configuredFilterPortMask;

  xLibS32_t sourcePort;
  xLibS32_t sourcePortPrev=0;

  L7_RC_t rv;

  memset(&configuredSrcPortMask,0x0,sizeof(configuredSrcPortMask));
  memset(&returnPortMask,0x0,sizeof(returnPortMask));
  memset(&configuredFilterPortMask,0x0,sizeof(configuredFilterPortMask));

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: SessionId */
  kwaSessionId.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
      (xLibU8_t *) & keySessionIdValue, &kwaSessionId.len);
  if (kwaSessionId.rc != XLIBRC_SUCCESS)
  {    
    return kwaSessionId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIdValue, kwaSessionId.len);

  kwaDirectionKey.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorDirectionKey,
      (xLibU8_t *) & directionKeyValue, &kwaDirectionKey.len);
  if (kwaDirectionKey.rc != XLIBRC_SUCCESS)
  {
    directionKeyValue  = L7_MIRROR_BIDIRECTIONAL;    
    owa.rc = xLibFilterSet(wap,XOBJ_basePortMirroring_PortMirrorDirectionKey,0,(xLibU8_t *) &directionKeyValue,sizeof(directionKeyValue));
    return XLIBRC_SUCCESS;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &directionKeyValue, kwaDirectionKey.len);
  if(directionKeyValue == L7_MIRROR_UNCONFIGURED)
  {
    kwaDirectionKey.rc = XLIBRC_FAILURE;
    return kwaDirectionKey.rc;
  }

  owa.l7rc = usmDbSwPortMonitorSourcePortsGet(L7_UNIT_CURRENT, keySessionIdValue, &configuredSrcPortMask);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  do
  {
    sourcePort = getNextIntf(&configuredSrcPortMask,sourcePortPrev);
    if(sourcePort != -1)
    {
       rv = usmDbMirrorSourcePortDirectionGet (keySessionIdValue, sourcePort, &direction);
       if(direction == directionKeyValue)
       { 
         L7_INTF_SETMASKBIT(returnPortMask,sourcePort);
         sourcePortPrev = sourcePort;
       }
       else
       {
          sourcePortPrev = sourcePort;
       }
    }
    else
    {
      break;
    }
  }while(1);
  owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)&returnPortMask,sizeof(returnPortMask));
  FPOBJ_TRACE_EXIT (bufp, owa);
  owa.rc = XLIBRC_SUCCESS;
  return owa.rc;
}

xLibRC_t fpObjSet_basePortMirroring_PortMirrorSourcePortConfigured (void *wap,
                                                               void *bufp)
{  
  xLibRC_t rc;
  L7_RC_t rv;
  L7_int32 intfNum;
  L7_uint32 sessionId;
  xLibU16_t len;
  xLibU16_t sidLen;
  L7_INTF_MASK_t intfMask;

  sidLen = sizeof (sessionId);
  memset(&intfMask,0x0,sizeof(intfMask));
  rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID, (xLibU8_t *) & sessionId, &sidLen);
  if (rc != XLIBRC_SUCCESS)
  {
    return XLIBRC_FILTER_MISSING;
  }

  len = sizeof (intfMask);
  rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSourcePortConfigured, (xLibU8_t *) & intfMask, &len);
  if (rc != XLIBRC_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  intfNum = getNextIntf(&intfMask,0);
  if(intfNum == -1)
  {
    return XLIBRC_FAILURE;

  }
  while(intfNum != -1)
  {
    rv=usmDbSwPortMonitorSourcePortRemove(L7_UNIT_CURRENT, sessionId,intfNum);
    if(rv!=L7_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }
    intfNum = getNextIntf(&intfMask,intfNum);
  }
  return XLIBRC_SUCCESS;
}

xLibRC_t fpObjGet_basePortMirroring_PortMirrorIsConfigured (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortMirrorSessionIDValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_INTF_MASK_t configuredSrcPortMask;
  xLibS32_t sourcePort;
  xLibU32_t objPortMirrorIsConfigured = L7_FALSE;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortMirrorSessionID */
  kwa.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
                          (xLibU8_t *) & keyPortMirrorSessionIDValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortMirrorSessionIDValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbSwPortMonitorSourcePortsGet(L7_UNIT_CURRENT, keyPortMirrorSessionIDValue, &configuredSrcPortMask);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  sourcePort = getNextIntf(&configuredSrcPortMask,0);
  if(sourcePort != -1)
  {
    objPortMirrorIsConfigured = L7_TRUE;
  } 
  /* return the object value: PortMirrorAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortMirrorIsConfigured,
                           sizeof (objPortMirrorIsConfigured));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjSet_basePortMirroring_PortMirrorRemoveSession(void *wap,
                                                               void *bufp)
{  
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortMirrorSessionIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortMirrorSessionID */
  kwa.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
                          (xLibU8_t *) & keyPortMirrorSessionIDValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortMirrorSessionIDValue, kwa.len);

  kwa.rc = XLIBRC_SUCCESS;
  /* set the value in application */
  kwa.l7rc = usmDbSwPortMonitorSessionRemove(L7_UNIT_CURRENT,keyPortMirrorSessionIDValue);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
	
  FPOBJ_TRACE_EXIT (bufp, kwa);
  return kwa.rc;
}

xLibRC_t fpObjGet_basePortMirroring_PortMirrorRemoveSession(void *wap,
                                                               void *bufp)
{
   /* 
      THIS IS A WEB ONLY OBJECT, NOT USED BY CLI or SNMP.
      This is a empty function as this object is used with a button.
      It is always write-only NO GET will be called.
   */
   return XLIBRC_SUCCESS;
}
/*******************************************************************************
* @function fpObjSet_basePortMirroring_PortMirrorSrcPortDelete
*
* @purpose Set 'PortMirrorSrcPortDelete'
 *@description  [PortMirrorSrcPortDelete]    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basePortMirroring_PortMirrorSrcPortDelete (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objPortMirrorSrcPortDeleteValue;

  xLibU32_t keyPortMirrorSessionIDValue;
  
  L7_INTF_MASK_t intfMask;
  xLibU32_t intfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PortMirrorSrcPortDelete */
  owa.len = sizeof (objPortMirrorSrcPortDeleteValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPortMirrorSrcPortDeleteValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortMirrorSrcPortDeleteValue, owa.len);

  /* retrieve key: PortMirrorSessionID */
  owa.len = sizeof (keyPortMirrorSessionIDValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID,
                          (xLibU8_t *) & keyPortMirrorSessionIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortMirrorSessionIDValue, owa.len);

  /* retrieve key: PortMirrorSourcePort */
  owa.len = sizeof (intfMask);
  owa.rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSourcePort, (xLibU8_t *) & intfMask, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  intfNum = getPortFromMask(intfMask);
  /* set the value in application */ 
  if(intfNum != -1)
  {
    owa.l7rc = usmDbSwPortMonitorSourcePortRemove (L7_UNIT_CURRENT, keyPortMirrorSessionIDValue,
                              intfNum);
  }
  else
  {
    owa.l7rc = L7_FAILURE;  
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/********************************************************************************
* @function fpObjGet_basePortMirroring_SourcePort
*
* @purpose Get 'PortMirroring_SourcePort'
 *@description  [PortMirroring_SourcePort]
* @notes
*
* @return
*******************************************************************************/

xLibRC_t fpObjGet_basePortMirroring_SourcePort (void *wap, void *bufp)
{
  L7_RC_t rv;
  L7_uint32 sourcePort;
  L7_uint32 nextPort;
  xLibU16_t bufLen = sizeof (sourcePort);

  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, XOBJ_basePortMirroring_SourcePort,
                                         (xLibU8_t *) & sourcePort, &bufLen))
  {
    rv = usmDbSwPortMonitorValidSrcIntfFirstGet (&nextPort);
  }
  else
  {
    rv = usmDbSwPortMonitorValidSrcIntfNextGet (sourcePort, &nextPort);
  }

  if (rv != L7_SUCCESS)
  {
    return XLIBRC_ENDOF_TABLE;
  }

  return xLibBufDataSet (bufp, (xLibU8_t *) & nextPort, sizeof (nextPort));
}
/**************************************************************************
* @function fpObjGet_basePortMirroring_Direction
*
* @purpose Get 'basePortMirroring_Direction'
 *@description  [basePortMirroring_Direction]
* @notes
*
* @return
***************************************************************************/

xLibRC_t fpObjGet_basePortMirroring_Direction (void *wap, void *bufp)
{
  xLibRC_t rc;
  L7_RC_t rv;
  L7_uint32 intIfNum;
  L7_uint32 sessionId;
  xLibU16_t len;
  xLibU16_t sidLen;
  L7_MIRROR_DIRECTION_t direction;

  sidLen = sizeof (sessionId);
  rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID, (xLibU8_t *) & sessionId, &sidLen);
  if (rc != XLIBRC_SUCCESS)
  {
    XLIB_HWARN ("Filter Missing");
    return XLIBRC_FILTER_MISSING;
  }

  len = sizeof (intIfNum);
  rc = xLibFilterGet (wap, XOBJ_basePortMirroring_SourcePort, (xLibU8_t *) & intIfNum, &len);
  if (rc != XLIBRC_SUCCESS)
  {
    XLIB_HWARN ("Filter Missing");
    return XLIBRC_FILTER_MISSING;
  }

  rv = usmDbMirrorSourcePortDirectionGet (sessionId, intIfNum, &direction);
  return xLibBufDataSet (bufp, (xLibU8_t *) & direction, sizeof (direction));
}

/**********************************************************************************
* @function fpObjSet_basePortMirroring_Direction
*
* @purpose Get 'basePortMirroring_Direction'
 *@description  [basePortMirroring_Direction]
* @notes
*
* @return
***********************************************************************************/

xLibRC_t fpObjSet_basePortMirroring_Direction (void *wap, void *bufp)
{
  xLibRC_t rc;
  L7_RC_t rv;
  L7_uint32 intIfNum;
  L7_uint32 sessionId;
  xLibU16_t len;
  xLibU16_t sidLen;
  L7_MIRROR_DIRECTION_t direction;

  sidLen = sizeof (sessionId);
  rc = xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSessionID, (xLibU8_t *) & sessionId, &sidLen);
  if (rc != XLIBRC_SUCCESS)
  {
    XLIB_HWARN ("Filter Missing");
    return XLIBRC_FILTER_MISSING;
  }

  len = sizeof (intIfNum);
  rc = xLibFilterGet (wap, XOBJ_basePortMirroring_SourcePort, (xLibU8_t *) & intIfNum, &len);
  if (rc != XLIBRC_SUCCESS)
  {
    XLIB_HWARN ("Filter Missing");
    return XLIBRC_FILTER_MISSING;
  }

  len = sizeof (direction);
  rc=xLibBufDataGet (bufp, (xLibU8_t *) & direction, &len);
  if(rc != XLIBRC_SUCCESS)
  {
     return XLIBRC_FAILURE;
  }     

  if(direction==L7_MIRROR_UNCONFIGURED)
    rv=usmDbSwPortMonitorSourcePortRemove(L7_USMDB_UNIT_ZERO, sessionId,intIfNum);
  else
    rv = usmDbSwPortMonitorSourcePortAdd (L7_USMDB_UNIT_ZERO, sessionId, intIfNum, direction);

  if(rv!=L7_FAILURE)
  {
   return XLIBRC_SUCCESS;
  }
  return XLIBRC_FAILURE;
}

/*******************************************************************************
* @function fpObjGet_basePortMirroring_SourcePortRange
*
* @purpose Get 'SourcePort listing'
*
* @description The source port list. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortMirroring_PortMirrorSourcePortRange(void *wap, void *bufp)
{
  L7_RC_t rv;
  L7_INTF_MASK_t sourcePortMask;
  L7_uint32 sourcePort; 
  L7_uint32 nextPort;
  L7_INTF_MASK_t mask;
  
  xLibU16_t bufLen = sizeof (sourcePortMask);
  memset(&sourcePortMask,0x0,sizeof(sourcePortMask)); 
  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, XOBJ_basePortMirroring_PortMirrorSourcePortRange,
        (xLibU8_t *) &sourcePortMask, &bufLen))
  {
    rv = usmDbSwPortMonitorValidSrcIntfFirstGet (&nextPort);
    if( rv == L7_SUCCESS )
    {
      if(usmDbSwPortMonitorIsSrcConfigured(L7_UNIT_CURRENT,nextPort) == L7_TRUE)
      {
         sourcePort = nextPort;
         while((rv=usmDbSwPortMonitorValidSrcIntfNextGet (sourcePort, &nextPort)) != L7_FAILURE) 
         {
           if( usmDbSwPortMonitorIsSrcConfigured(L7_UNIT_CURRENT,nextPort) == L7_FALSE)
           {
              break;
           }
           sourcePort = nextPort;
         };
         if(rv==L7_FAILURE)
         {
           rv = L7_FAILURE; 
         }
      }
    }
    else
    {
      rv = L7_FAILURE;
    }
  }
  else
  {
    sourcePort = getPortFromMask(sourcePortMask);   
    if(sourcePort != -1)
    {
      while((rv=usmDbSwPortMonitorValidSrcIntfNextGet (sourcePort, &nextPort)) != L7_FAILURE)
      {
        if( usmDbSwPortMonitorIsSrcConfigured(L7_UNIT_CURRENT,nextPort) == L7_FALSE)
        {
           break;
        }
        sourcePort = nextPort;
      };
      if(rv==L7_FAILURE) 
      {
        rv = L7_FAILURE; 
      }
    }
    else
    {
      rv = L7_FAILURE;
    }
  }

  if (rv != L7_SUCCESS)
  {
    memset(&mask,0x0,sizeof(mask));
    xLibBufDataSet (bufp, (xLibU8_t *) &mask, sizeof (mask));
    return XLIBRC_ENDOF_TABLE;
  }
  if(0 != nextPort)
  {
    memset(&mask,0x0,sizeof(mask));
    L7_INTF_SETMASKBIT(mask,nextPort);
    xLibBufDataSet (bufp, (xLibU8_t *) &mask, sizeof (mask));
    return XLIBRC_SUCCESS;
  }
  return XLIBRC_FAILURE;
}

