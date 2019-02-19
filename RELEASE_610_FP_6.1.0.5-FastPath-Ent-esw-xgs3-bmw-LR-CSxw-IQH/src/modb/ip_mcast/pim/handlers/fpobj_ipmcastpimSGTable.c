
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimSGTable.c
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
#include "_xe_ipmcastpimSGTable_obj.h"
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_mib_pim_rfc5060_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGAddressType
*
* @purpose Get 'pimSGAddressType'
*
* @description [pimSGAddressType] The address type of the source and multicast group for this entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGAddressTypeValue;
  xLibU32_t nextObjpimSGAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                          (xLibU8_t *) & objpimSGAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimSGAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGAddressTypeValue, owa.len);

    if (objpimSGAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjpimSGAddressTypeValue = objpimSGAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimSGAddressTypeValue, owa.len);

  /* return the object value: pimSGAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimSGAddressTypeValue,
                           sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGGrpAddress
*
* @purpose Get 'pimSGGrpAddress'
*
* @description [pimSGGrpAddress] The multicast group address for this entry. The InetAddressType is given by the pimSmSGAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGGrpAddress (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;

  L7_inet_addr_t objpimSGGrpAddressValue;
  L7_inet_addr_t nextObjpimSGGrpAddressValue;
  L7_inet_addr_t nextObjpimSGSrcAddressValue;

  L7_int32 IntIfNum;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                          (xLibU8_t *) &objpimSGGrpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    memset(&nextObjpimSGGrpAddressValue, 0, sizeof(L7_inet_addr_t));
    memset(&nextObjpimSGSrcAddressValue, 0, sizeof(L7_inet_addr_t));

    owa.l7rc = usmDbPimSGIEntryNextGet (keypimSGAddressTypeValue,
                                                                 &nextObjpimSGGrpAddressValue,
                                                                 &nextObjpimSGSrcAddressValue, &IntIfNum);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGGrpAddressValue, owa.len);

    memcpy(&nextObjpimSGGrpAddressValue, &objpimSGGrpAddressValue, sizeof(L7_inet_addr_t));
    memset(&nextObjpimSGSrcAddressValue, 0, sizeof(L7_inet_addr_t));

    do
    {
      owa.l7rc = usmDbPimSGIEntryNextGet (keypimSGAddressTypeValue,
                                                                   &nextObjpimSGGrpAddressValue,
                                                                   &nextObjpimSGSrcAddressValue, &IntIfNum);
    }
    while (!memcmp(&objpimSGGrpAddressValue, &nextObjpimSGGrpAddressValue, sizeof(L7_inet_addr_t)) 
              && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimSGGrpAddressValue, owa.len);

  /* return the object value: pimSGGrpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimSGGrpAddressValue,
                           sizeof (objpimSGGrpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGSrcAddress
*
* @purpose Get 'pimSGSrcAddress'
*
* @description [pimSGSrcAddress] The source address for this entry. The InetAddressType is given by the pimSmSGAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGSrcAddress (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;

  L7_inet_addr_t objpimSGGrpAddressValue;
  L7_inet_addr_t nextObjpimSGGrpAddressValue;
  L7_inet_addr_t objpimSGSrcAddressValue;
  L7_inet_addr_t nextObjpimSGSrcAddressValue;

  L7_int32 IntIfNum;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                          (xLibU8_t *) &objpimSGGrpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGGrpAddressValue, owa.len);

  /* retrieve key: pimSGSrcAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                          (xLibU8_t *) &objpimSGSrcAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(&nextObjpimSGSrcAddressValue, 0, sizeof(L7_inet_addr_t));

    owa.l7rc = usmDbPimSGIEntryNextGet (keypimSGAddressTypeValue,
                                                                 &nextObjpimSGGrpAddressValue,
                                                                 &nextObjpimSGSrcAddressValue, &IntIfNum);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGSrcAddressValue, owa.len);

    memcpy(&nextObjpimSGSrcAddressValue, &objpimSGSrcAddressValue, sizeof(L7_inet_addr_t));
    
    owa.l7rc = usmDbPimSGIEntryNextGet (keypimSGAddressTypeValue,
                                                                 &nextObjpimSGGrpAddressValue,
                                                                 &nextObjpimSGSrcAddressValue, &IntIfNum);
  }

  if (!memcmp(&objpimSGGrpAddressValue, &nextObjpimSGGrpAddressValue, sizeof(L7_inet_addr_t)) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimSGSrcAddressValue, owa.len);

  /* return the object value: pimSGSrcAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimSGSrcAddressValue,
                           sizeof (objpimSGSrcAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGUpTime
*
* @purpose Get 'pimSGUpTime'
*
* @description [pimSGUpTime] The time since this entry was created by the local router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGUpTime (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;
  
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGUpTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGUpTimeGet (keypimSGAddressTypeValue,
                                                            &keypimSGGrpAddressValue,
                                                            &keypimSGSrcAddressValue, &objpimSGUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGUpTimeValue, sizeof (objpimSGUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGPimMode
*
* @purpose Get 'pimSGPimMode'
*
* @description [pimSGPimMode] Whether pimSmSGGrpAddress is an SSM (Source Specific Multicast, used with PIM-SM) or ASM (Any Source Multicast, used with PIM-SM) group.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGPimMode (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;

  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGPimModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGPimModeGet (keypimSGAddressTypeValue,
                                                              &keypimSGGrpAddressValue,
                                                              &keypimSGSrcAddressValue, &objpimSGPimModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGPimMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGPimModeValue,
                           sizeof (objpimSGPimModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGUpstreamJoinState
*
* @purpose Get 'pimSGUpstreamJoinState'
*
* @description [pimSGUpstreamJoinState] Whether the local router should join the shortest-path treefor the source and group represented by this entry. This corresponds to the state of the upstream (S,G) state machinein the PIM-SM specification [I-D.ietf-pim-sm-v2-new].
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGUpstreamJoinState (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;
  
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGUpstreamJoinStateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGUpstreamJoinStateGet (keypimSGAddressTypeValue,
                                                                            &keypimSGGrpAddressValue,
                                                                            &keypimSGSrcAddressValue, &objpimSGUpstreamJoinStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGUpstreamJoinState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGUpstreamJoinStateValue,
                           sizeof (objpimSGUpstreamJoinStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGUpstreamJoinTimer
*
* @purpose Get 'pimSGUpstreamJoinTimer'
*
* @description [pimSGUpstreamJoinTimer] The time remaining before the local router next sends aperiodic (S,G) Join message on pimSmSGRPFIfIndex. This timeris called the (S,G) Upstream Join Timer in the PIM-SMspecification [I-D.ietf-pim-sm-v2-new].
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGUpstreamJoinTimer (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;
  
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGUpstreamJoinTimerValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGUpstreamJoinTimerGet (keypimSGAddressTypeValue,
                                                                            &keypimSGGrpAddressValue,
                                                                            &keypimSGSrcAddressValue, &objpimSGUpstreamJoinTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGUpstreamJoinTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGUpstreamJoinTimerValue,
                           sizeof (objpimSGUpstreamJoinTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGUpstreamNeighbor
*
* @purpose Get 'pimSGUpstreamNeighbor'
*
* @description [pimSGUpstreamNeighbor] The primary address of the neighbor on pimSmSGRPFIfIndex that the local router is sending periodic (S,G) Join messages to. This is zero if the the RPF next hop is unknown or is not a PIM neighbor.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGUpstreamNeighbor (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;
  
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimSGUpstreamNeighborValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGUpstreamNeighborGet (keypimSGAddressTypeValue,
                                                                            &keypimSGGrpAddressValue,
                                                                            &keypimSGSrcAddressValue, &objpimSGUpstreamNeighborValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGUpstreamNeighbor */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objpimSGUpstreamNeighborValue,
                           sizeof(objpimSGUpstreamNeighborValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGRPFIfIndex
*
* @purpose Get 'pimSGRPFIfIndex'
*
* @description [pimSGRPFIfIndex] The value of ifIndex for the RPF interface towards the source, or zero if the RPF interface is unknown.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGRPFIfIndex (void *wap, void *bufp)
{
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRPFIfIndexValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGRPFIfIndexGet (keypimSGAddressTypeValue,
                                                                &keypimSGGrpAddressValue,
                                                                &keypimSGSrcAddressValue, &objpimSGRPFIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRPFIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRPFIfIndexValue,
                           sizeof (objpimSGRPFIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGRPFNextHopType
*
* @purpose Get 'pimSGRPFNextHopType'
*
* @description [pimSGRPFNextHopType] The address type of the RPF next hop towards the source, or unknown(0) if the RPF next hop is unknown.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGRPFNextHopType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGGrpAddressValue;
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRPFNextHopTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimSGAddressTypeValue,
                              keypimSGGrpAddressValue,
                              keypimSGSrcAddressValue, &objpimSGRPFNextHopTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRPFNextHopType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRPFNextHopTypeValue,
                           sizeof (objpimSGRPFNextHopTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGRPFNextHop
*
* @purpose Get 'pimSGRPFNextHop'
*
* @description [pimSGRPFNextHop] The address of the RPF next hop towards the source, or zero if the the RPF next hop is unknown. The InetAddressType is given by the pimSmSGAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGRPFNextHop (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimSGRPFNextHopValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGRPFNextHopGet (keypimSGAddressTypeValue,
                                                                  &keypimSGGrpAddressValue,
                                                                  &keypimSGSrcAddressValue, &objpimSGRPFNextHopValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRPFNextHop */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objpimSGRPFNextHopValue,
                           sizeof (objpimSGRPFNextHopValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGRPFRouteProtocol
*
* @purpose Get 'pimSGRPFRouteProtocol'
*
* @description [pimSGRPFRouteProtocol] The routing mechanism via which the route used to find the RPF interface towards the source was learned.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGRPFRouteProtocol (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRPFRouteProtocolValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGRPFRouteProtocolGet (keypimSGAddressTypeValue,
                                                                          &keypimSGGrpAddressValue,
                                                                          &keypimSGSrcAddressValue, &objpimSGRPFRouteProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRPFRouteProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRPFRouteProtocolValue,
                           sizeof (objpimSGRPFRouteProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGRPFRouteAddress
*
* @purpose Get 'pimSGRPFRouteAddress'
*
* @description [pimSGRPFRouteAddress] The IP address which when combined with the corresponding value of pimSmSGRPFRoutePrefixLength identifies the route used to find the RPF interface towards the source.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGRPFRouteAddress (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimSGRPFRouteAddressValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGRPFRouteAddressGet (keypimSGAddressTypeValue,
                                                                          &keypimSGGrpAddressValue,
                                                                          &keypimSGSrcAddressValue, &objpimSGRPFRouteAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRPFRouteAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objpimSGRPFRouteAddressValue,
                           sizeof (objpimSGRPFRouteAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGRPFRoutePrefixLength
*
* @purpose Get 'pimSGRPFRoutePrefixLength'
*
* @description [pimSGRPFRoutePrefixLength] The prefix length which when combined with the corresponding value of pimSmSGRPFRouteAddress identifies the route used to find the RPF interface towards the source.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGRPFRoutePrefixLength (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRPFRoutePrefixLengthValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGRPFRoutePrefixLengthGet (keypimSGAddressTypeValue,
                                                                                &keypimSGGrpAddressValue,
                                                                                &keypimSGSrcAddressValue, &objpimSGRPFRoutePrefixLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRPFRoutePrefixLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRPFRoutePrefixLengthValue,
                           sizeof (objpimSGRPFRoutePrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGRPFRouteMetricPref
*
* @purpose Get 'pimSGRPFRouteMetricPref'
*
* @description [pimSGRPFRouteMetricPref] The metric preference of the route used to find the RPF interface towards the source.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGRPFRouteMetricPref (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRPFRouteMetricPrefValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGRPFRouteMetricPrefGet (keypimSGAddressTypeValue,
                                                                             &keypimSGGrpAddressValue,
                                                                             &keypimSGSrcAddressValue, &objpimSGRPFRouteMetricPrefValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRPFRouteMetricPref */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRPFRouteMetricPrefValue,
                           sizeof (objpimSGRPFRouteMetricPrefValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGRPFRouteMetric
*
* @purpose Get 'pimSGRPFRouteMetric'
*
* @description [pimSGRPFRouteMetric] The metric preference of the route used to find the RPF interface towards the source.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGRPFRouteMetric (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRPFRouteMetricValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGRPFRouteMetricPrefGet (keypimSGAddressTypeValue,
                                                                            &keypimSGGrpAddressValue,
                                                                            &keypimSGSrcAddressValue, &objpimSGRPFRouteMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRPFRouteMetric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRPFRouteMetricValue,
                           sizeof (objpimSGRPFRouteMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGSPTBit
*
* @purpose Get 'pimSGSPTBit'
*
* @description [pimSGSPTBit] Whether the SPT bit is set; and therefore whether forwarding is taking place on the shortest-path tree.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGSPTBit (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGSPTBitValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGSPTBitGet (keypimSGAddressTypeValue,
                                                          &keypimSGGrpAddressValue,
                                                          &keypimSGSrcAddressValue, &objpimSGSPTBitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGSPTBit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGSPTBitValue, sizeof (objpimSGSPTBitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGKeepaliveTimer
*
* @purpose Get 'pimSGKeepaliveTimer'
*
* @description [pimSGKeepaliveTimer] The time remaining before, in the absence of explicit (S,G) local membership or (S,G) Join messages received to maintain it, this (S,G) state expires.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGKeepaliveTimer (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGKeepaliveTimerValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmSGKeepaliveTimerGet (keypimSGAddressTypeValue,
                                                &keypimSGGrpAddressValue,
                                                &keypimSGSrcAddressValue, &objpimSGKeepaliveTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGKeepaliveTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGKeepaliveTimerValue,
                           sizeof (objpimSGKeepaliveTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGDRRegisterState
*
* @purpose Get 'pimSGDRRegisterState'
*
* @description [pimSGDRRegisterState] Whether the local router should encapsulate (S,G) datapackets in Register messages and send them to the RP. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGDRRegisterState (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGDRRegisterStateValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmSGDRRegisterStateGet (keypimSGAddressTypeValue,
                                                                        &keypimSGGrpAddressValue,
                                                                        &keypimSGSrcAddressValue, &objpimSGDRRegisterStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGDRRegisterState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGDRRegisterStateValue,
                           sizeof (objpimSGDRRegisterStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGDRRegisterStopTimer
*
* @purpose Get 'pimSGDRRegisterStopTimer'
*
* @description [pimSGDRRegisterStopTimer] If pimSmSGDRRegisterState is 'prune', this is the timeremaining before the local router sends a Null-Registermessage to the RP.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGDRRegisterStopTimer (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGDRRegisterStopTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmSGDRRegisterStopTimerGet (keypimSGAddressTypeValue,
                                                                                &keypimSGGrpAddressValue,
                                                                                &keypimSGSrcAddressValue, &objpimSGDRRegisterStopTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGDRRegisterStopTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGDRRegisterStopTimerValue,
                           sizeof (objpimSGDRRegisterStopTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGRPRegisterPMBRAddressType
*
* @purpose Get 'pimSGRPRegisterPMBRAddressType'
*
* @description [pimSGRPRegisterPMBRAddressType] The address type of the first PIM Multicast Border Router to send a Register message with the Border bit set. This object is unknown(0) if the local router is not the RP for the group.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGRPRegisterPMBRAddressType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGGrpAddressValue;
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRPRegisterPMBRAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimSGAddressTypeValue,
                              keypimSGGrpAddressValue,
                              keypimSGSrcAddressValue, &objpimSGRPRegisterPMBRAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRPRegisterPMBRAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRPRegisterPMBRAddressTypeValue,
                           sizeof (objpimSGRPRegisterPMBRAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGRPRegisterPMBRAddress
*
* @purpose Get 'pimSGRPRegisterPMBRAddress'
*
* @description [pimSGRPRegisterPMBRAddress] The IP address of the first PIM Multicast Border Router tosend a Register message with the Border bit set. Thisobject is zero if the local router is not the RP for thegroup. The InetAddressType is given by the pimSmSGAddressTypeobject.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGRPRegisterPMBRAddress (void *wap, void *bufp)
{
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimSGRPRegisterPMBRAddressValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGRPRegisterPMBRAddressGet (keypimSGAddressTypeValue,
                                                                                    &keypimSGGrpAddressValue,
                                                                                    &keypimSGSrcAddressValue, &objpimSGRPRegisterPMBRAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRPRegisterPMBRAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objpimSGRPRegisterPMBRAddressValue,
                           sizeof (objpimSGRPRegisterPMBRAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGUpstreamPruneState
*
* @purpose Get 'pimSGUpstreamPruneState'
*
* @description [pimSGUpstreamPruneState] Whether the local router has pruned itself from the tree.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGUpstreamPruneState (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGGrpAddressValue;
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGUpstreamPruneStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimSGAddressTypeValue,
                              keypimSGGrpAddressValue,
                              keypimSGSrcAddressValue, &objpimSGUpstreamPruneStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGUpstreamPruneState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGUpstreamPruneStateValue,
                           sizeof (objpimSGUpstreamPruneStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGUpstreamPruneLimitTimer
*
* @purpose Get 'pimSGUpstreamPruneLimitTimer'
*
* @description [pimSGUpstreamPruneLimitTimer] The time remaining before the local router may send a (S,G) Prune message on pimSGRPFIfIndex.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGUpstreamPruneLimitTimer (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGGrpAddressValue;
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGUpstreamPruneLimitTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimSGAddressTypeValue,
                              keypimSGGrpAddressValue,
                              keypimSGSrcAddressValue, &objpimSGUpstreamPruneLimitTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGUpstreamPruneLimitTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGUpstreamPruneLimitTimerValue,
                           sizeof (objpimSGUpstreamPruneLimitTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGOriginatorState
*
* @purpose Get 'pimSGOriginatorState'
*
* @description [pimSGOriginatorState] Whether the router is an originator for an (S,G) message flow.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGOriginatorState (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGGrpAddressValue;
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGOriginatorStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimSGAddressTypeValue,
                              keypimSGGrpAddressValue,
                              keypimSGSrcAddressValue, &objpimSGOriginatorStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGOriginatorState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGOriginatorStateValue,
                           sizeof (objpimSGOriginatorStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGSourceActiveTimer
*
* @purpose Get 'pimSGSourceActiveTimer'
*
* @description [pimSGSourceActiveTimer] If pimSGOriginatorState is 'originator', this is the time remaining before the local router reverts to a notOriginator state.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGSourceActiveTimer (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGGrpAddressValue;
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGSourceActiveTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimSGAddressTypeValue,
                              keypimSGGrpAddressValue,
                              keypimSGSrcAddressValue, &objpimSGSourceActiveTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGSourceActiveTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGSourceActiveTimerValue,
                           sizeof (objpimSGSourceActiveTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGTable_pimSGStateRefreshTimer
*
* @purpose Get 'pimSGStateRefreshTimer'
*
* @description [pimSGStateRefreshTimer] If pimSGOriginatorState is 'originator', this is the time remaining before the local router sends a State Refresh message.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGTable_pimSGStateRefreshTimer (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGGrpAddressValue;
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGStateRefreshTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGGrpAddress,
                                         (xLibU8_t *) keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGTable_pimSGSrcAddress,
                                         (xLibU8_t *) keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimSGAddressTypeValue,
                              keypimSGGrpAddressValue,
                              keypimSGSrcAddressValue, &objpimSGStateRefreshTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGStateRefreshTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGStateRefreshTimerValue,
                           sizeof (objpimSGStateRefreshTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
