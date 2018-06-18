
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastmgmdHostInterfaceTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  01 May 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastmgmdHostInterfaceTable_obj.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_igmp_api.h"
#include "usmdb_mib_igmp_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceIfIndex
*
* @purpose Get 'mgmdHostInterfaceIfIndex'
*
* @description [mgmdHostInterfaceIfIndex] The ifIndex value of the interface for which IGMP or MLD is enabled. The table is indexed by the ifIndex value and the InetAddressType to allow for interfaces which may be configured in both IPv4 and IPv6 modes.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceIfIndex (void *wap, void *bufp)
{

  xLibU32_t objmgmdHostInterfaceQuerierTypeValue;
  xLibU32_t objmgmdHostInterfaceIfIndexValue;
  xLibU32_t nextObjmgmdHostInterfaceIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostInterfaceQuerierType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerierType,
                          (xLibU8_t *) & objmgmdHostInterfaceQuerierTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostInterfaceQuerierTypeValue, owa.len);

  /* retrieve key: mgmdHostInterfaceIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceIfIndex,
                          (xLibU8_t *) & objmgmdHostInterfaceIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdHostInterfaceIfIndexValue = 0;
    owa.l7rc = usmDbMgmdHostInterfaceEntryNextGet (L7_UNIT_CURRENT,
                                     objmgmdHostInterfaceQuerierTypeValue,
                                     &nextObjmgmdHostInterfaceIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostInterfaceIfIndexValue, owa.len);

    owa.l7rc = usmDbMgmdHostInterfaceEntryNextGet (L7_UNIT_CURRENT,
                                     objmgmdHostInterfaceQuerierTypeValue,
                                     &nextObjmgmdHostInterfaceIfIndexValue);

  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdHostInterfaceIfIndexValue, owa.len);

  /* return the object value: mgmdHostInterfaceIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdHostInterfaceIfIndexValue,
                           sizeof (objmgmdHostInterfaceIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerierType
*
* @purpose Get 'mgmdHostInterfaceQuerierType'
*
* @description [mgmdHostInterfaceQuerierType] The address type of this interface. This entry along with the ifIndex value acts as an index to the mgmdHostInterface table. A physical interface may be configured in multiple modes concurrently, e.g. in IPv4 and IPv6
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerierType (void *wap, void *bufp)
{

  xLibU32_t objmgmdHostInterfaceQuerierTypeValue;
  xLibU32_t nextObjmgmdHostInterfaceQuerierTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostInterfaceQuerierType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerierType,
                          (xLibU8_t *) & objmgmdHostInterfaceQuerierTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdHostInterfaceQuerierTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;    
  }
  else if(objmgmdHostInterfaceQuerierTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostInterfaceQuerierTypeValue, owa.len);
    nextObjmgmdHostInterfaceQuerierTypeValue = L7_INET_ADDR_TYPE_IPV6;
    owa.l7rc = L7_SUCCESS;    
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostInterfaceQuerierTypeValue, owa.len);
    owa.l7rc = L7_FAILURE;    
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdHostInterfaceQuerierTypeValue, owa.len);

  /* return the object value: mgmdHostInterfaceQuerierType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdHostInterfaceQuerierTypeValue,
                           sizeof (objmgmdHostInterfaceQuerierTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerier
*
* @purpose Get 'mgmdHostInterfaceQuerier'
*
* @description [mgmdHostInterfaceQuerier] The address of the IGMP or MLD Querier on the IP subnet to which this interface is attached. The InetAddressType, e.g. IPv4 or IPv6, is identified by the mgmdHostInterfaceQuerierType variable in the mgmdHostInterface table.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerier (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdHostInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceIfIndexValue;
  fpObjWa_t kwamgmdHostInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objmgmdHostInterfaceQuerierValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostInterfaceIfIndex */
  kwamgmdHostInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdHostInterfaceIfIndexValue,
                   &kwamgmdHostInterfaceIfIndex.len);
  if (kwamgmdHostInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceIfIndex);
    return kwamgmdHostInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceIfIndexValue,
                           kwamgmdHostInterfaceIfIndex.len);

  /* retrieve key: mgmdHostInterfaceQuerierType */
  kwamgmdHostInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdHostInterfaceQuerierTypeValue,
                   &kwamgmdHostInterfaceQuerierType.len);
  if (kwamgmdHostInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceQuerierType);
    return kwamgmdHostInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceQuerierTypeValue,
                           kwamgmdHostInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdHostInterfaceQuerierGet (L7_UNIT_CURRENT, keymgmdHostInterfaceQuerierTypeValue,
                               keymgmdHostInterfaceIfIndexValue,
                               &objmgmdHostInterfaceQuerierValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdHostInterfaceQuerier */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objmgmdHostInterfaceQuerierValue,
                           sizeof (objmgmdHostInterfaceQuerierValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceStatus
*
* @purpose Get 'mgmdHostInterfaceStatus'
*
* @description [mgmdHostInterfaceStatus] The activation of a row enables the host side of IGMP or MLD on the interface. The destruction of a row disables the host side of IGMP or MLD on the interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceStatus (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdHostInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceIfIndexValue;
  fpObjWa_t kwamgmdHostInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdHostInterfaceStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostInterfaceIfIndex */
  kwamgmdHostInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdHostInterfaceIfIndexValue,
                   &kwamgmdHostInterfaceIfIndex.len);
  if (kwamgmdHostInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceIfIndex);
    return kwamgmdHostInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceIfIndexValue,
                           kwamgmdHostInterfaceIfIndex.len);

  /* retrieve key: mgmdHostInterfaceQuerierType */
  kwamgmdHostInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdHostInterfaceQuerierTypeValue,
                   &kwamgmdHostInterfaceQuerierType.len);
  if (kwamgmdHostInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceQuerierType);
    return kwamgmdHostInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceQuerierTypeValue,
                           kwamgmdHostInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdHostInterfaceStatusGet (L7_UNIT_CURRENT, keymgmdHostInterfaceQuerierTypeValue,
                              keymgmdHostInterfaceIfIndexValue,
                              &objmgmdHostInterfaceStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdHostInterfaceStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdHostInterfaceStatusValue,
                           sizeof (objmgmdHostInterfaceStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceStatus
*
* @purpose Set 'mgmdHostInterfaceStatus'
*
* @description [mgmdHostInterfaceStatus] The activation of a row enables the host side of IGMP or MLD on the interface. The destruction of a row disables the host side of IGMP or MLD on the interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdHostInterfaceStatusValue;

  fpObjWa_t kwamgmdHostInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceIfIndexValue;
  fpObjWa_t kwamgmdHostInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceQuerierTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdHostInterfaceStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmgmdHostInterfaceStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdHostInterfaceStatusValue, owa.len);

  /* retrieve key: mgmdHostInterfaceIfIndex */
  kwamgmdHostInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdHostInterfaceIfIndexValue,
                   &kwamgmdHostInterfaceIfIndex.len);
  if (kwamgmdHostInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceIfIndex);
    return kwamgmdHostInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceIfIndexValue,
                           kwamgmdHostInterfaceIfIndex.len);

  /* retrieve key: mgmdHostInterfaceQuerierType */
  kwamgmdHostInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdHostInterfaceQuerierTypeValue,
                   &kwamgmdHostInterfaceQuerierType.len);
  if (kwamgmdHostInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceQuerierType);
    return kwamgmdHostInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceQuerierTypeValue,
                           kwamgmdHostInterfaceQuerierType.len);

  owa.l7rc = L7_SUCCESS;
  if (objmgmdHostInterfaceStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbMgmdHostInterfaceStatusSet (L7_UNIT_CURRENT, keymgmdHostInterfaceQuerierTypeValue,
                                keymgmdHostInterfaceIfIndexValue,
                                objmgmdHostInterfaceStatusValue,MGMD_PROXY_INTERFACE);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceVersion
*
* @purpose Get 'mgmdHostInterfaceVersion'
*
* @description [mgmdHostInterfaceVersion] The maximum version of MGMD which the host can run on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceVersion (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdHostInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceIfIndexValue;
  fpObjWa_t kwamgmdHostInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdHostInterfaceVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostInterfaceIfIndex */
  kwamgmdHostInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdHostInterfaceIfIndexValue,
                   &kwamgmdHostInterfaceIfIndex.len);
  if (kwamgmdHostInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceIfIndex);
    return kwamgmdHostInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceIfIndexValue,
                           kwamgmdHostInterfaceIfIndex.len);

  /* retrieve key: mgmdHostInterfaceQuerierType */
  kwamgmdHostInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdHostInterfaceQuerierTypeValue,
                   &kwamgmdHostInterfaceQuerierType.len);
  if (kwamgmdHostInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceQuerierType);
    return kwamgmdHostInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceQuerierTypeValue,
                           kwamgmdHostInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdHostInterfaceVersionGet (L7_UNIT_CURRENT,keymgmdHostInterfaceQuerierTypeValue,
  keymgmdHostInterfaceIfIndexValue,&objmgmdHostInterfaceVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdHostInterfaceVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdHostInterfaceVersionValue,
                           sizeof (objmgmdHostInterfaceVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceVersion
*
* @purpose Set 'mgmdHostInterfaceVersion'
*
* @description [mgmdHostInterfaceVersion] The maximum version of MGMD which the host can run on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceVersion (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdHostInterfaceVersionValue;

  fpObjWa_t kwamgmdHostInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceIfIndexValue;
  fpObjWa_t kwamgmdHostInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceQuerierTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdHostInterfaceVersion */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmgmdHostInterfaceVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdHostInterfaceVersionValue, owa.len);

  /* retrieve key: mgmdHostInterfaceIfIndex */
  kwamgmdHostInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdHostInterfaceIfIndexValue,
                   &kwamgmdHostInterfaceIfIndex.len);
  if (kwamgmdHostInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceIfIndex);
    return kwamgmdHostInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceIfIndexValue,
                           kwamgmdHostInterfaceIfIndex.len);

  /* retrieve key: mgmdHostInterfaceQuerierType */
  kwamgmdHostInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdHostInterfaceQuerierTypeValue,
                   &kwamgmdHostInterfaceQuerierType.len);
  if (kwamgmdHostInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceQuerierType);
    return kwamgmdHostInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceQuerierTypeValue,
                           kwamgmdHostInterfaceQuerierType.len);

  /* set the value in application - NOT IMPLEMENTED */

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceVersion1QuerierTimer
*
* @purpose Get 'mgmdHostInterfaceVersion1QuerierTimer'
*
* @description [mgmdHostInterfaceVersion1QuerierTimer] The time remaining until the host assumes that there are no MGMDv1 routers present on the interface. While this is non-zero, the host will reply to all queries with version 1 membership reports.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceVersion1QuerierTimer (void *wap,
                                                                                       void *bufp)
{

  fpObjWa_t kwamgmdHostInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceIfIndexValue;
  fpObjWa_t kwamgmdHostInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdHostInterfaceVersion1QuerierTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostInterfaceIfIndex */
  kwamgmdHostInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdHostInterfaceIfIndexValue,
                   &kwamgmdHostInterfaceIfIndex.len);
  if (kwamgmdHostInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceIfIndex);
    return kwamgmdHostInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceIfIndexValue,
                           kwamgmdHostInterfaceIfIndex.len);

  /* retrieve key: mgmdHostInterfaceQuerierType */
  kwamgmdHostInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdHostInterfaceQuerierTypeValue,
                   &kwamgmdHostInterfaceQuerierType.len);
  if (kwamgmdHostInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceQuerierType);
    return kwamgmdHostInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceQuerierTypeValue,
                           kwamgmdHostInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdHostInterfaceVer1QuerierTimerGet (L7_UNIT_CURRENT, keymgmdHostInterfaceQuerierTypeValue,
                              keymgmdHostInterfaceIfIndexValue,
                              &objmgmdHostInterfaceVersion1QuerierTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdHostInterfaceVersion1QuerierTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdHostInterfaceVersion1QuerierTimerValue,
                           sizeof (objmgmdHostInterfaceVersion1QuerierTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceVersion2QuerierTimer
*
* @purpose Get 'mgmdHostInterfaceVersion2QuerierTimer'
*
* @description [mgmdHostInterfaceVersion2QuerierTimer] The time remaining until the host assumes that there are no MGMDv2 routers present on the interface. While this is non-zero, the host will reply to all queries with version 1 or 2 membership reports.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceVersion2QuerierTimer (void *wap,
                                                                                       void *bufp)
{

  fpObjWa_t kwamgmdHostInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceIfIndexValue;
  fpObjWa_t kwamgmdHostInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdHostInterfaceVersion2QuerierTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostInterfaceIfIndex */
  kwamgmdHostInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdHostInterfaceIfIndexValue,
                   &kwamgmdHostInterfaceIfIndex.len);
  if (kwamgmdHostInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceIfIndex);
    return kwamgmdHostInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceIfIndexValue,
                           kwamgmdHostInterfaceIfIndex.len);

  /* retrieve key: mgmdHostInterfaceQuerierType */
  kwamgmdHostInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostInterfaceTable_mgmdHostInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdHostInterfaceQuerierTypeValue,
                   &kwamgmdHostInterfaceQuerierType.len);
  if (kwamgmdHostInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostInterfaceQuerierType);
    return kwamgmdHostInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostInterfaceQuerierTypeValue,
                           kwamgmdHostInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdHostInterfaceVer2QuerierTimerGet (L7_UNIT_CURRENT, keymgmdHostInterfaceQuerierTypeValue,
                              keymgmdHostInterfaceIfIndexValue,
                              &objmgmdHostInterfaceVersion2QuerierTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdHostInterfaceVersion2QuerierTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdHostInterfaceVersion2QuerierTimerValue,
                           sizeof (objmgmdHostInterfaceVersion2QuerierTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
