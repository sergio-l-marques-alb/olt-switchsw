
/********************************************************************
  Function: snmpSmonPortCopyEntryGet
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/

#include "usmdb_mib_rmon_api.h"
#include "usmdb_mirror_api.h"
#include "usmdb_util_api.h"


/* The session number. */
#define SESSION_NUMBER 1

typedef enum
{
  PORTCOPY_INGRESS = 1,
  PORTCOPY_EGRESS,
  PORTCOPY_BIDIRECTIONAL
} portCopyDirection_t;


L7_RC_t
snmpSmonPortCopyEntryGet(L7_uint32 UnitIndex,
                         L7_uint32 portCopySource,
                         L7_uint32 portCopyDestn)
{
  /* */
  L7_uint32 sessionNum = SESSION_NUMBER;
  L7_BOOL rc;

  if(portCopySource == 0 || portCopyDestn == 0)
    return L7_FAILURE;

  /* Verify if the source port exists. */
  rc = usmDbSwPortMonitorIsSrcConfigured(UnitIndex, portCopySource);
  if (rc == L7_FALSE)
     return L7_FAILURE;

  /* Verify if the destination port exists. */
  rc = usmDbSwPortMonitorIsDestConfigured(UnitIndex, portCopyDestn,
                                                      &sessionNum);
  if (rc == L7_FALSE)
     return L7_FAILURE;

  /* Source and destination ports both found. */
  return L7_SUCCESS;

}

/*********************************************************************
  Function: snmpSmonPortCopyEntryNextGet
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpSmonPortCopyEntryNextGet(L7_uint32 UnitIndex,
                             L7_uint32 *portCopySource,
                             L7_uint32 *portCopyDestn)
{
  /* */
  L7_RC_t        rc;
  L7_uint32      sessionNum = SESSION_NUMBER;
  L7_uint32      listSrcPorts[256];
  L7_uint32      numPorts;
  L7_INTF_MASK_t srcIfMask;
  L7_uint32      index = 0;
  L7_uchar8      srcFoundBeforeLastEntry = L7_FALSE;
  L7_uint32      source, destn;


  if((*portCopySource > 0) && (*portCopyDestn > 0))
  {
    if(nimGetNextExtIfNumber(*portCopySource, &source) != L7_SUCCESS)
      return L7_FAILURE;
    if(nimGetNextExtIfNumber(*portCopyDestn, &destn) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* Get the data for port mirroring at this point in time. */
  rc = usmDbSwPortMonitorSourcePortsGet(UnitIndex, sessionNum, &srcIfMask);
  if (rc != L7_SUCCESS)
     return L7_FAILURE;

  rc = usmDbConvertMaskToList(&srcIfMask, listSrcPorts, &numPorts);
  if (rc != L7_SUCCESS)
     return L7_FAILURE;

  /*  Search if the source port exists in the listSrcPorts 
   *  array only if numPorts >= 1. */
  if (numPorts < 1)
  {
    return L7_FAILURE;
  }
  if((*portCopySource == 1) && (*portCopyDestn == 1) &&
    (usmDbSwPortMonitorIsSrcConfigured(UnitIndex, *portCopySource)
                                                      == L7_TRUE))
  {
    srcFoundBeforeLastEntry = L7_TRUE;
  }
  else
  {
    for (index = 1; index <= numPorts; index++)
    {
      if(listSrcPorts[index] == *portCopySource)
      {
        if(usmDbSwPortMonitorDestPortGet(UnitIndex, sessionNum, &destn)
            == L7_FAILURE)
        {
          return L7_FAILURE;
        }
        /* It is possible that the request might have a non-existing index */
        if(destn >= *portCopyDestn)
        {
          *portCopyDestn = destn;
          return L7_SUCCESS;
        }
      }
      else if(listSrcPorts[index] > *portCopySource)
      {
        *portCopySource = listSrcPorts[index];
        srcFoundBeforeLastEntry = L7_TRUE;
        break;
      }
    }
  }
  /* Verify Destination if the Source found before last entry */
  if (srcFoundBeforeLastEntry == L7_TRUE)
  {
    /* Verfiy the given destination port is same as the one existing. */
    if(usmDbSwPortMonitorDestPortGet(UnitIndex, sessionNum, portCopyDestn)
        == L7_FAILURE)
    {
       return L7_FAILURE;
    }

    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }

}

/*********************************************************************
  Function: snmpSmonPortCopyDirectionGet
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpSmonPortCopyDirectionGet(L7_uint32 UnitIndex,
                             L7_uint32 portCopySource,
                             L7_uint32 portCopyDestn,
                             L7_uint32 *portCopyDirection)
{
  L7_uint32              sessionNum = SESSION_NUMBER;
  L7_MIRROR_DIRECTION_t  probeType; 

  if((usmDbSwPortMonitorIsSrcConfigured(UnitIndex, portCopySource)
                                                       == L7_TRUE) &&
     (usmDbSwPortMonitorIsDestConfigured(UnitIndex, portCopyDestn, 
                                          &sessionNum) == L7_TRUE))
  {
    /*
     * No API as of now to Get/Set the Direction, so sending default
     */
    if(usmDbMirrorSourcePortDirectionGet(sessionNum, portCopySource, &probeType)
                                                      == L7_SUCCESS)
    {
      *portCopyDirection = (((L7_uint32)probeType == L7_MIRROR_INGRESS)?
           PORTCOPY_INGRESS:(((L7_uint32)probeType == L7_MIRROR_EGRESS)? 
           PORTCOPY_EGRESS:(((L7_uint32)probeType == L7_MIRROR_BIDIRECTIONAL)?
           PORTCOPY_BIDIRECTIONAL:(L7_uint32)NULL)));

      return L7_SUCCESS;
    }
    return L7_FAILURE;
  }
  else
  {
    return L7_FAILURE;
  }

}


/*********************************************************************
  Function: snmpSmonPortCopyDestDropEventsGet
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpSmonPortCopyDestDropEventsGet(L7_uint32 UnitIndex,
                                  L7_uint32 portCopySource,
                                  L7_uint32 portCopyDestn,
                                  L7_uint32 *portCopyDestDropEvents)
{
  L7_RC_t        rc;
  L7_uint32      sessionNum = SESSION_NUMBER;

  /*
   * <TBD> Currently no plans to support this functionality
   *  so just returning value '0' 
   */
  if((usmDbSwPortMonitorIsSrcConfigured(UnitIndex, 
                                        portCopySource) == L7_TRUE) &&
     (usmDbSwPortMonitorIsDestConfigured(UnitIndex, 
                                         portCopyDestn, 
                                         &sessionNum) == L7_TRUE))
  {
    
    rc = usmDbEtherStatsDropEventsGet(UnitIndex, portCopySource, 
                                      portCopyDestDropEvents);
    /* 
     * Return value has to be checked. Currently API is not fully implemented
     * and the return is always L7_FAILURE.
     */
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }

}

/*********************************************************************
  Function: snmpSmonPortCopyStatusGet
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpSmonPortCopyStatusGet(L7_uint32 UnitIndex,
                          L7_uint32 portCopySource,
                          L7_uint32 portCopyDestn,
                          L7_uint32 *portCopyStatus)
{
  L7_BOOL        rc;
  L7_uint32      sessionNum = SESSION_NUMBER;

  rc = usmDbSwPortMonitorIsSrcConfigured(UnitIndex, portCopySource);
  if (rc == L7_FALSE)
     return L7_FAILURE;

  /* Verify if the destination port exists. */
  rc = usmDbSwPortMonitorIsDestConfigured(UnitIndex,portCopyDestn,&sessionNum);
  if (rc == L7_FALSE)
     return L7_FAILURE;

  /* Source and destination ports both found. */
  *portCopyStatus = D_portCopyStatus_active;
  return L7_SUCCESS;
}

/*********************************************************************
  Function: snmpSmonPortCopyDestDropEventsGet
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpSmonPortCopyStatusSet(L7_uint32 UnitIndex,
                          L7_uint32 portCopySource,
                          L7_uint32 portCopyDestn,
                          L7_uint32 portCopyStatus)
{
  L7_RC_t        rc;
  L7_uint32      sessionNum = SESSION_NUMBER;
  L7_uint32      listSrcPorts[256];
  L7_uint32      numPorts;
  L7_INTF_MASK_t srcIfMask;

  if (portCopyStatus == D_portCopyStatus_createAndGo)
  {
    rc = usmDbSwPortMonitorSourcePortAdd(UnitIndex, sessionNum, portCopySource,
                                         L7_MIRROR_BIDIRECTIONAL);
    if(rc != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if(usmDbSwPortMonitorIsDestConfigured(UnitIndex, portCopyDestn, 
                                          &sessionNum) == L7_FALSE)
    {
      rc = usmDbSwPortMonitorDestPortSet(UnitIndex, sessionNum, portCopyDestn);
      if(rc != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
  }
  else if(portCopyStatus == D_portCopyStatus_destroy)
  {
    rc = usmDbSwPortMonitorSourcePortRemove(UnitIndex, sessionNum,
                                                  portCopySource);
    if(rc != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    rc = usmDbSwPortMonitorSourcePortsGet(UnitIndex, sessionNum, &srcIfMask);
    if (rc != L7_SUCCESS)
      return L7_FAILURE;

    rc = usmDbConvertMaskToList(&srcIfMask, listSrcPorts, &numPorts);
    if (rc != L7_SUCCESS)
      return L7_FAILURE;

    if(numPorts == 0)
    {
      rc = usmDbSwPortMonitorDestPortRemove(UnitIndex, sessionNum);
      if(rc != L7_SUCCESS)
        return L7_FAILURE;
    }
  }
  else
  /* 
   * D_portCopyStatus_notReady, D_portCopyStatus_createAndWait, 
   * D_portCopyStatus_notInService are currently not supported, 
   * and this case can be extended to support these at later point of time.
   */
  {
     return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
  Function: snmpSmonPortCopyDestDropEventsGet
  Purpose:  Support the port copy functionality.
  Arguments:
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpSmonPortCopyDirectionSet(L7_uint32 UnitIndex,
                             L7_uint32 portCopySource,
                             L7_uint32 portCopyDestn,
                             L7_uint32 portCopyDirection)
{
  L7_RC_t        rc, rc1;
  L7_uint32      sessionNum = SESSION_NUMBER;
  L7_MIRROR_DIRECTION_t probeType; 

  if((usmDbSwPortMonitorIsDestConfigured(UnitIndex, portCopyDestn, &sessionNum)
                                                                 == L7_TRUE) &&
     (usmDbSwPortMonitorIsSrcConfigured(UnitIndex, portCopySource) == L7_TRUE))
  {
    if((usmDbMirrorSourcePortDirectionGet(sessionNum, portCopySource, &probeType) == L7_SUCCESS) &&
      ((portCopyDirection == PORTCOPY_INGRESS) ||
       (portCopyDirection == PORTCOPY_EGRESS) ||
       (portCopyDirection == PORTCOPY_BIDIRECTIONAL)))
    {
      probeType = ((probeType == L7_MIRROR_INGRESS)? 
            PORTCOPY_INGRESS:((probeType == L7_MIRROR_EGRESS)? 
            PORTCOPY_EGRESS:((probeType == L7_MIRROR_BIDIRECTIONAL)?
            PORTCOPY_BIDIRECTIONAL:(L7_uint32)NULL)));

      if((L7_uint32)probeType != portCopyDirection)
      {
        portCopyDirection = ((portCopyDirection == PORTCOPY_INGRESS) ?
            L7_MIRROR_INGRESS : ((portCopyDirection == PORTCOPY_EGRESS) ?
            L7_MIRROR_EGRESS : L7_MIRROR_BIDIRECTIONAL));

        rc = usmDbSwPortMonitorSourcePortRemove(UnitIndex, sessionNum,
                                                      portCopySource);
        rc1 = usmDbSwPortMonitorSourcePortAdd(UnitIndex, sessionNum, portCopySource,
                                              portCopyDirection);
        if((rc == L7_SUCCESS) && (rc1 == L7_SUCCESS))
          return L7_SUCCESS;
      }
      return L7_SUCCESS;
    }
    return L7_FAILURE;
  }
  else
  {
     return L7_FAILURE;
  }

}

