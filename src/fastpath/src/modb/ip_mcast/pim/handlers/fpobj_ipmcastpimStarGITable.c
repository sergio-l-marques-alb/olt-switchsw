
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimStarGITable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  10 May 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimStarGITable_obj.h"
#include "usmdb_mib_pimsm_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGAddressType
*
* @purpose Get 'pimStarGAddressType'
*
* @description [pimStarGAddressType] The address type of this multicast group.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGAddressTypeValue;
  xLibU32_t nextObjpimStarGAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                          (xLibU8_t *) & objpimStarGAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimStarGAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimStarGAddressTypeValue, owa.len);

    if (objpimStarGAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjpimStarGAddressTypeValue = objpimStarGAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimStarGAddressTypeValue, owa.len);

  /* return the object value: pimStarGAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimStarGAddressTypeValue,
                           sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGGrpAddress
*
* @purpose Get 'pimStarGGrpAddress'
*
* @description [pimStarGGrpAddress] The multicast group address. The InetAddressType is given by the pimSmStarGAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGGrpAddress (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimStarGGrpAddressValue;
  L7_inet_addr_t nextObjpimStarGGrpAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                          (xLibU8_t *) &objpimStarGGrpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(&nextObjpimStarGGrpAddressValue, 0, sizeof(L7_inet_addr_t));

    owa.l7rc = usmDbPimsmStarGEntryNextGet (keypimStarGAddressTypeValue,&nextObjpimStarGGrpAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimStarGGrpAddressValue, owa.len);
    memcpy(&nextObjpimStarGGrpAddressValue, &objpimStarGGrpAddressValue, sizeof(L7_inet_addr_t));

    owa.l7rc = usmDbPimsmStarGEntryNextGet (keypimStarGAddressTypeValue,&nextObjpimStarGGrpAddressValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimStarGGrpAddressValue, owa.len);

  /* return the object value: pimStarGGrpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimStarGGrpAddressValue,
                           sizeof (objpimStarGGrpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGIIfIndex
*
* @purpose Get 'pimStarGIIfIndex'
*
* @description [pimStarGIIfIndex] The ifIndex of the interface that this entry corresponds to.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGIIfIndex (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;

  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objpimStarGIIfIndexValue;
  xLibU32_t nextObjpimStarGIIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);

  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGIIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGIIfIndex,
                          (xLibU8_t *) & objpimStarGIIfIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimStarGIIfIndexValue = 0;

    owa.l7rc = usmDbPimsmStarGIEntryNextGet (keypimStarGAddressTypeValue,
                                       &keypimStarGGrpAddressValue, &nextObjpimStarGIIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimStarGIIfIndexValue, owa.len);

    nextObjpimStarGIIfIndexValue = objpimStarGIIfIndexValue;
    owa.l7rc = usmDbPimsmStarGIEntryNextGet (keypimStarGAddressTypeValue,
                                       &keypimStarGGrpAddressValue, &nextObjpimStarGIIfIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimStarGIIfIndexValue, owa.len);

  /* return the object value: pimStarGIIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimStarGIIfIndexValue,
                           sizeof (objpimStarGIIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGIUpTime
*
* @purpose Get 'pimStarGIUpTime'
*
* @description [pimStarGIUpTime] The time since this entry was created by the local router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGIUpTime (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;

  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimStarGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGIUpTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGIIfIndex */
  kwapimStarGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGIIfIndex,
                                          (xLibU8_t *) & keypimStarGIIfIndexValue,
                                          &kwapimStarGIIfIndex.len);
  if (kwapimStarGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGIIfIndex);
    return kwapimStarGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGIIfIndexValue, kwapimStarGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGIUpTimeGet (keypimStarGAddressTypeValue,
                                                                 &keypimStarGGrpAddressValue,
                                                                 keypimStarGIIfIndexValue, &objpimStarGIUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGIUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGIUpTimeValue,
                           sizeof (objpimStarGIUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGILocalMembership
*
* @purpose Get 'pimStarGILocalMembership'
*
* @description [pimStarGILocalMembership] Whether the local router has (*,G) local membership on thisinterface (resulting from a mechanism such as IGMP or MLD).This corresponds to local_receiver_include(*,G,I) in thePIM-SM specification [I-D.ietf-pim-sm-v2-new].
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGILocalMembership (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimStarGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGILocalMembershipValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGIIfIndex */
  kwapimStarGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGIIfIndex,
                                          (xLibU8_t *) & keypimStarGIIfIndexValue,
                                          &kwapimStarGIIfIndex.len);
  if (kwapimStarGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGIIfIndex);
    return kwapimStarGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGIIfIndexValue, kwapimStarGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGILocalMembershipGet (keypimStarGAddressTypeValue,
                                                                              &keypimStarGGrpAddressValue,
                                                                              keypimStarGIIfIndexValue, &objpimStarGILocalMembershipValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGILocalMembership */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGILocalMembershipValue,
                           sizeof (objpimStarGILocalMembershipValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGIJoinPruneState
*
* @purpose Get 'pimStarGIJoinPruneState'
*
* @description [pimStarGIJoinPruneState] The state resulting from (*,G) Join/Prune messagesreceived on this interface. This corresponds to the stateof the downstream per-interface (*,G) state machine in thePIM-SM specification [I-D.ietf-pim-sm-v2-new].
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGIJoinPruneState (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimStarGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGIJoinPruneStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGIIfIndex */
  kwapimStarGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGIIfIndex,
                                          (xLibU8_t *) & keypimStarGIIfIndexValue,
                                          &kwapimStarGIIfIndex.len);
  if (kwapimStarGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGIIfIndex);
    return kwapimStarGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGIIfIndexValue, kwapimStarGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGIJoinPruneStateGet (keypimStarGAddressTypeValue,
                                                                            &keypimStarGGrpAddressValue,
                                                                            keypimStarGIIfIndexValue, &objpimStarGIJoinPruneStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGIJoinPruneState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGIJoinPruneStateValue,
                           sizeof (objpimStarGIJoinPruneStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGIPrunePendingTimer
*
* @purpose Get 'pimStarGIPrunePendingTimer'
*
* @description [pimStarGIPrunePendingTimer] The time remaining before the local router acts on a (*,G)Prune message received on this interface, during which therouter is waiting to see whether another downstream routerwill override the Prune message. This timer is called the(*,G) Prune-Pending Tim
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGIPrunePendingTimer (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimStarGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGIPrunePendingTimerValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGIIfIndex */
  kwapimStarGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGIIfIndex,
                                          (xLibU8_t *) & keypimStarGIIfIndexValue,
                                          &kwapimStarGIIfIndex.len);
  if (kwapimStarGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGIIfIndex);
    return kwapimStarGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGIIfIndexValue, kwapimStarGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGIPrunePendingTimerGet (keypimStarGAddressTypeValue,
                                                                                  &keypimStarGGrpAddressValue,
                                                                                  keypimStarGIIfIndexValue, &objpimStarGIPrunePendingTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGIPrunePendingTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGIPrunePendingTimerValue,
                           sizeof (objpimStarGIPrunePendingTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGIJoinExpiryTimer
*
* @purpose Get 'pimStarGIJoinExpiryTimer'
*
* @description [pimStarGIJoinExpiryTimer] The time remaining before (*,G) Join state for thisinterface expires. This timer is called the (*,G) JoinExpiry Timer in the PIM-SM specification[I-D.ietf-pim-sm-v2-new]. This object is zero if the timeris not running. A value of 'FFFFFFFF'h indicates 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGIJoinExpiryTimer (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimStarGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGIJoinExpiryTimerValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGIIfIndex */
  kwapimStarGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGIIfIndex,
                                          (xLibU8_t *) & keypimStarGIIfIndexValue,
                                          &kwapimStarGIIfIndex.len);
  if (kwapimStarGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGIIfIndex);
    return kwapimStarGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGIIfIndexValue, kwapimStarGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGIJoinExpiryTimerGet (keypimStarGAddressTypeValue,
                                                                            &keypimStarGGrpAddressValue,
                                                                            keypimStarGIIfIndexValue, &objpimStarGIJoinExpiryTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGIJoinExpiryTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGIJoinExpiryTimerValue,
                           sizeof (objpimStarGIJoinExpiryTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGIAssertState
*
* @purpose Get 'pimStarGIAssertState'
*
* @description [pimStarGIAssertState] The (*,G) Assert state for this interface. Thiscorresponds to the state of the per-interface (*,G) Assertstate machine in the PIM-SM specification[I-D.ietf-pim-sm-v2-new]. If pimSmStarGPimSmMode is 'bidir', this object must be 'noInfo'.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGIAssertState (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimStarGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGIAssertStateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGIIfIndex */
  kwapimStarGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGIIfIndex,
                                          (xLibU8_t *) & keypimStarGIIfIndexValue,
                                          &kwapimStarGIIfIndex.len);
  if (kwapimStarGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGIIfIndex);
    return kwapimStarGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGIIfIndexValue, kwapimStarGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGIAssertStateGet (keypimStarGAddressTypeValue,
                                                                      &keypimStarGGrpAddressValue,
                                                                      keypimStarGIIfIndexValue, &objpimStarGIAssertStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGIAssertState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGIAssertStateValue,
                           sizeof (objpimStarGIAssertStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGIAssertTimer
*
* @purpose Get 'pimStarGIAssertTimer'
*
* @description [pimStarGIAssertTimer] If pimSmStarGIAssertState is 'iAmAssertWinner', this is the time remaining before the local router next sends a (*,G) Assert message on this interface. If pimSmStarGIAssertState is 'iAmAssertLoser', this is the time remaining before the (*,G) Assert message on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGIAssertTimer (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimStarGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGIAssertTimerValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGIIfIndex */
  kwapimStarGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGIIfIndex,
                                          (xLibU8_t *) & keypimStarGIIfIndexValue,
                                          &kwapimStarGIIfIndex.len);
  if (kwapimStarGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGIIfIndex);
    return kwapimStarGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGIIfIndexValue, kwapimStarGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGIAssertTimerGet (keypimStarGAddressTypeValue,
                                                                      &keypimStarGGrpAddressValue,
                                                                      keypimStarGIIfIndexValue, &objpimStarGIAssertTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGIAssertTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGIAssertTimerValue,
                           sizeof (objpimStarGIAssertTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGIAssertWinnerAddressType
*
* @purpose Get 'pimStarGIAssertWinnerAddressType'
*
* @description [pimStarGIAssertWinnerAddressType] If pimStarGIAssertState is 'iAmAssertLoser', this is the address type of the assert winner; otherwise, this object is unknown(0).
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGIAssertWinnerAddressType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimStarGGrpAddressValue;
  fpObjWa_t kwapimStarGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGIAssertWinnerAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                                            (xLibU8_t *) keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGIIfIndex */
  kwapimStarGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGIIfIndex,
                                          (xLibU8_t *) & keypimStarGIIfIndexValue,
                                          &kwapimStarGIIfIndex.len);
  if (kwapimStarGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGIIfIndex);
    return kwapimStarGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGIIfIndexValue, kwapimStarGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimStarGAddressTypeValue,
                              keypimStarGGrpAddressValue,
                              keypimStarGIIfIndexValue, &objpimStarGIAssertWinnerAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGIAssertWinnerAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGIAssertWinnerAddressTypeValue,
                           sizeof (objpimStarGIAssertWinnerAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGIAssertWinnerAddress
*
* @purpose Get 'pimStarGIAssertWinnerAddress'
*
* @description [pimStarGIAssertWinnerAddress] If pimStarGIAssertState is 'iAmAssertLoser', this is the address of the assert winner. The InetAddressType is given by the pimStarGIAssertWinnerAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGIAssertWinnerAddress (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimStarGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimStarGIAssertWinnerAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGIIfIndex */
  kwapimStarGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGIIfIndex,
                                          (xLibU8_t *) & keypimStarGIIfIndexValue,
                                          &kwapimStarGIIfIndex.len);
  if (kwapimStarGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGIIfIndex);
    return kwapimStarGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGIIfIndexValue, kwapimStarGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGIAssertWinnerAddrGet (keypimStarGAddressTypeValue,
                                                                                &keypimStarGGrpAddressValue,
                                                                                keypimStarGIIfIndexValue, &objpimStarGIAssertWinnerAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGIAssertWinnerAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objpimStarGIAssertWinnerAddressValue,
                           sizeof (objpimStarGIAssertWinnerAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGIAssertWinnerMetricPref
*
* @purpose Get 'pimStarGIAssertWinnerMetricPref'
*
* @description [pimStarGIAssertWinnerMetricPref] If pimStarGIAssertState is 'iAmAssertLoser', this is the metric preference of the route to the RP advertised by the assert winner; otherwise, this object is zero.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGIAssertWinnerMetricPref (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimStarGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGIAssertWinnerMetricPrefValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGIIfIndex */
  kwapimStarGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGIIfIndex,
                                          (xLibU8_t *) & keypimStarGIIfIndexValue,
                                          &kwapimStarGIIfIndex.len);
  if (kwapimStarGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGIIfIndex);
    return kwapimStarGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGIIfIndexValue, kwapimStarGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGIRPFRouteMetricPrefGet (keypimStarGAddressTypeValue,
                                                                                  &keypimStarGGrpAddressValue,
                                                                                  keypimStarGIIfIndexValue, &objpimStarGIAssertWinnerMetricPrefValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGIAssertWinnerMetricPref */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGIAssertWinnerMetricPrefValue,
                           sizeof (objpimStarGIAssertWinnerMetricPrefValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGITable_pimStarGIAssertWinnerMetric
*
* @purpose Get 'pimStarGIAssertWinnerMetric'
*
* @description [pimStarGIAssertWinnerMetric] If pimStarGIAssertState is 'iAmAssertLoser', this is the routing metric of the route to the RP advertised by the assert winner; otherwise, this object is zero.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGITable_pimStarGIAssertWinnerMetric (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimStarGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGIAssertWinnerMetricValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGIIfIndex */
  kwapimStarGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGITable_pimStarGIIfIndex,
                                          (xLibU8_t *) & keypimStarGIIfIndexValue,
                                          &kwapimStarGIIfIndex.len);
  if (kwapimStarGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGIIfIndex);
    return kwapimStarGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGIIfIndexValue, kwapimStarGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGIRPFRouteMetricPrefGet (keypimStarGAddressTypeValue,
                                                                                  &keypimStarGGrpAddressValue,
                                                                                  keypimStarGIIfIndexValue, &objpimStarGIAssertWinnerMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGIAssertWinnerMetric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGIAssertWinnerMetricValue,
                           sizeof (objpimStarGIAssertWinnerMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
