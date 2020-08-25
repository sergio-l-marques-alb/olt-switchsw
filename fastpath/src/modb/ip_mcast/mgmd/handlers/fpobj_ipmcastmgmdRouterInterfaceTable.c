
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastmgmdRouterInterfaceTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  02 May 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastmgmdRouterInterfaceTable_obj.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_igmp_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_mib_igmp_api.h"
#include "usmdb_util_api.h"


/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex
*
* @purpose Get 'mgmdRouterInterfaceIfIndex'
*
* @description [mgmdRouterInterfaceIfIndex] The ifIndex value of the interface for which IGMP or MLD is enabled. The table is indexed by the ifIndex value and the InetAddressType to allow for interfaces which may be configured in both IPv4 and IPv6 modes.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex (void *wap, void *bufp)
{

  xLibU32_t objmgmdRouterInterfaceQuerierTypeValue;
  xLibU32_t objmgmdRouterInterfaceIfIndexValue;
  xLibU32_t nextObjmgmdRouterInterfaceIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                          (xLibU8_t *) & objmgmdRouterInterfaceQuerierTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterInterfaceQuerierTypeValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                          (xLibU8_t *) & objmgmdRouterInterfaceIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdRouterInterfaceIfIndexValue = 0;
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LOGICAL_VLAN_INTF,
                                            0, &nextObjmgmdRouterInterfaceIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterInterfaceIfIndexValue, owa.len);
    nextObjmgmdRouterInterfaceIfIndexValue = 0;
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LOGICAL_VLAN_INTF, 0,
                    objmgmdRouterInterfaceIfIndexValue, &nextObjmgmdRouterInterfaceIfIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdRouterInterfaceIfIndexValue, owa.len);

  /* return the object value: mgmdRouterInterfaceIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdRouterInterfaceIfIndexValue,
                           sizeof (objmgmdRouterInterfaceIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType
*
* @purpose Get 'mgmdRouterInterfaceQuerierType'
*
* @description [mgmdRouterInterfaceQuerierType] The address type of this interface. This entry along with the ifIndex value acts as the index to the mgmdRouterInterface table. A physical interface may be configured in multiple modes concurrently, e.g. in IPv4 and IPv6 mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType (void *wap,
                                                                                  void *bufp)
{

  xLibU32_t objmgmdRouterInterfaceQuerierTypeValue;
  xLibU32_t nextObjmgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                          (xLibU8_t *) & objmgmdRouterInterfaceQuerierTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdRouterInterfaceQuerierTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;    
  }
  else if (objmgmdRouterInterfaceQuerierTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterInterfaceQuerierTypeValue, owa.len);
    nextObjmgmdRouterInterfaceQuerierTypeValue = L7_INET_ADDR_TYPE_IPV6;
    owa.l7rc = L7_SUCCESS;    
  }
  else
  {
    owa.l7rc = L7_FAILURE;
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdRouterInterfaceQuerierTypeValue, owa.len);

  /* return the object value: mgmdRouterInterfaceQuerierType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdRouterInterfaceQuerierTypeValue,
                           sizeof (objmgmdRouterInterfaceQuerierTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerier
*
* @purpose Get 'mgmdRouterInterfaceQuerier'
*
* @description [mgmdRouterInterfaceQuerier] The address of the IGMP or MLD Querier on the IP subnet to which this interface is attached. The InetAddressType, e.g. IPv4 or IPv6, is identified by the mgmdRouterInterfaceQuerierType variable
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerier (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  L7_inet_addr_t objmgmdRouterInterfaceQuerierValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  inetAddressZeroSet(keymgmdRouterInterfaceQuerierTypeValue, &objmgmdRouterInterfaceQuerierValue);
  if(usmDbMgmdInterfaceOperationalStateGet(L7_UNIT_CURRENT,keymgmdRouterInterfaceQuerierTypeValue,
                                           keymgmdRouterInterfaceIfIndexValue) == L7_TRUE)
  {
    /* get the value from application */
    owa.l7rc = usmDbMgmdInterfaceQuerierGet (L7_UNIT_CURRENT,
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              (L7_inet_addr_t *)&objmgmdRouterInterfaceQuerierValue);

  }
  else
  { /* Interface is not operational */
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceQuerier */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objmgmdRouterInterfaceQuerierValue,
                           sizeof(objmgmdRouterInterfaceQuerierValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdQuerierStatus
*
* @purpose Get 'mgmdQuerierStatus'
*
* @description [mgmdQuerierStatus] Gets Querier / Non-Querier Status of MGMD on this interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdQuerierStatus (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdQuerierStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* This parmeter is valid only when the MGMD interface is operational.*/
  if(usmDbMgmdInterfaceOperationalStateGet(L7_UNIT_CURRENT,
                                           keymgmdRouterInterfaceQuerierTypeValue,
                                           keymgmdRouterInterfaceIfIndexValue) == L7_FALSE)
  {
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceQuerierStatusGet (L7_UNIT_CURRENT, keymgmdRouterInterfaceQuerierTypeValue, 
                                                      keymgmdRouterInterfaceIfIndexValue,
                                                      &objmgmdQuerierStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdQuerierStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdQuerierStatusValue,
                           sizeof (objmgmdQuerierStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQueryInterval
*
* @purpose Get 'mgmdRouterInterfaceQueryInterval'
*
* @description [mgmdRouterInterfaceQueryInterval] The frequency at which IGMP or MLD Host-Query packets are transmitted on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQueryInterval (void *wap,
                                                                                    void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceQueryIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceQueryIntervalGet (L7_UNIT_CURRENT,
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceQueryIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceQueryInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceQueryIntervalValue,
                           sizeof (objmgmdRouterInterfaceQueryIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQueryInterval
*
* @purpose Set 'mgmdRouterInterfaceQueryInterval'
*
* @description [mgmdRouterInterfaceQueryInterval] The frequency at which IGMP or MLD Host-Query packets are transmitted on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQueryInterval (void *wap,
                                                                                    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceQueryIntervalValue;

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;
  xLibU32_t objmgmdRouterInterfaceQueryMaxResponseTimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdRouterInterfaceQueryInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objmgmdRouterInterfaceQueryIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterInterfaceQueryIntervalValue, owa.len);

  /* retrieve object: objmgmdRouterInterfaceQueryMaxResponseTimeValue */
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQueryMaxResponseTime,
                   (xLibU8_t *) &objmgmdRouterInterfaceQueryMaxResponseTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_ERROR_IGMP_COULDNOT_SET_QUERYNTVL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterInterfaceQueryMaxResponseTimeValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* For IGMP, the max response time is factor of one-tenth seconds.*/
  if (keymgmdRouterInterfaceQuerierTypeValue == L7_AF_INET)
  {
    if ((objmgmdRouterInterfaceQueryIntervalValue * 10) <= objmgmdRouterInterfaceQueryMaxResponseTimeValue)
    {
      owa.rc = XLIBRC_ERROR_IGMP_MAXRESPTIME_LESSTHAN_QUERYNTVL;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  /* For MLD, the max response time is factor of milli-seconds.*/
  else if (keymgmdRouterInterfaceQuerierTypeValue == L7_AF_INET6)
  {
    if ((objmgmdRouterInterfaceQueryIntervalValue *  MLD_RESP_TIME_FACTOR) <= objmgmdRouterInterfaceQueryMaxResponseTimeValue)
    {
      owa.rc = XLIBRC_ERROR_IGMP_MAXRESPTIME_LESSTHAN_QUERYNTVL;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  } 

  /* set the value in application */
  owa.l7rc = usmDbMgmdInterfaceQueryIntervalSet (L7_UNIT_CURRENT, 
                            keymgmdRouterInterfaceQuerierTypeValue,
                            keymgmdRouterInterfaceIfIndexValue,
                            objmgmdRouterInterfaceQueryIntervalValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceStatus
*
* @purpose Get 'mgmdRouterInterfaceStatus'
*
* @description [mgmdRouterInterfaceStatus] The activation of a row enables the router side of IGMP or MLD on the interface. The destruction of a row disables the router side of IGMP or MLD on the interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceStatus (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;
  if (usmDbMgmdInterfaceOperationalStateGet(L7_UNIT_CURRENT,keymgmdRouterInterfaceQuerierTypeValue,
                                                                   keymgmdRouterInterfaceIfIndexValue) == L7_TRUE)
  { 
    objmgmdRouterInterfaceStatusValue = L7_ROW_STATUS_ACTIVE;
  }
  else
  {
    objmgmdRouterInterfaceStatusValue = L7_ROW_STATUS_NOT_IN_SERVICE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceStatusValue,
                           sizeof (objmgmdRouterInterfaceStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceStatus
*
* @purpose Set 'mgmdRouterInterfaceStatus'
*
* @description [mgmdRouterInterfaceStatus] The activation of a row enables the router side of IGMP or MLD on the interface. The destruction of a row disables the router side of IGMP or MLD on the interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceStatusValue;

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdRouterInterfaceStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterInterfaceStatusValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  owa.l7rc = L7_SUCCESS;
  if ( (objmgmdRouterInterfaceStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
    ||(objmgmdRouterInterfaceStatusValue == L7_ROW_STATUS_ACTIVE))
  {
    owa.l7rc = usmDbMgmdInterfaceStatusSet (L7_UNIT_CURRENT, keymgmdRouterInterfaceQuerierTypeValue,
                                keymgmdRouterInterfaceIfIndexValue,
                                L7_ENABLE, MGMD_ROUTER_INTERFACE);
  }
  else if (objmgmdRouterInterfaceStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbMgmdInterfaceStatusSet (L7_UNIT_CURRENT, keymgmdRouterInterfaceQuerierTypeValue,
                                keymgmdRouterInterfaceIfIndexValue,
                                L7_DISABLE, MGMD_ROUTER_INTERFACE);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceVersion
*
* @purpose Get 'mgmdRouterInterfaceVersion'
*
* @description [mgmdRouterInterfaceVersion] The version of MGMD which is running on this interface. Value 1 applies to IGMPv1 routers only. Value 2 applies to IGMPv2 and MLDv1 routers, and value 3 applies to IGMPv3 and MLDv2 routers.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceVersion (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceVersionGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* return the object value: mgmdRouterInterfaceVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceVersionValue,
                           sizeof (objmgmdRouterInterfaceVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceVersion
*
* @purpose Set 'mgmdRouterInterfaceVersion'
*
* @description [mgmdRouterInterfaceVersion] The version of MGMD which is running on this interface. Value 1 applies to IGMPv1 routers only. Value 2 applies to IGMPv2 and MLDv1 routers, and value 3 applies to IGMPv3 and MLDv2 routers.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceVersion (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceVersionValue;

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdRouterInterfaceVersion */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterInterfaceVersionValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* set the value in application */
  owa.l7rc = usmDbMgmdInterfaceVersionSet(L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              objmgmdRouterInterfaceVersionValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQueryMaxResponseTime
*
* @purpose Get 'mgmdRouterInterfaceQueryMaxResponseTime'
*
* @description [mgmdRouterInterfaceQueryMaxResponseTime] The maximum query response time advertised in MGMDv2 or v3 queries on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQueryMaxResponseTime (void
                                                                                           *wap,
                                                                                           void
                                                                                           *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceQueryMaxResponseTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceQueryMaxResponseTimeGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceQueryMaxResponseTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceQueryMaxResponseTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceQueryMaxResponseTimeValue,
                           sizeof (objmgmdRouterInterfaceQueryMaxResponseTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQueryMaxResponseTime
*
* @purpose Set 'mgmdRouterInterfaceQueryMaxResponseTime'
*
* @description [mgmdRouterInterfaceQueryMaxResponseTime] The maximum query response time advertised in MGMDv2 or v3 queries on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQueryMaxResponseTime (void
                                                                                           *wap,
                                                                                           void
                                                                                           *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceQueryMaxResponseTimeValue;
  xLibU32_t objmgmdRouterInterfaceQueryIntervalValue;
  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdRouterInterfaceQueryMaxResponseTime */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objmgmdRouterInterfaceQueryMaxResponseTimeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterInterfaceQueryMaxResponseTimeValue, owa.len);

  /* retrieve object: objmgmdRouterInterfaceQueryMaxResponseTimeValue */
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQueryInterval,
                   (xLibU8_t *) &objmgmdRouterInterfaceQueryIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterInterfaceQueryIntervalValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);


  /* For IGMP, the max response time is factor of one-tenth seconds.*/
  if( keymgmdRouterInterfaceQuerierTypeValue == L7_AF_INET)
  { 
    if ((objmgmdRouterInterfaceQueryIntervalValue * 10) <= objmgmdRouterInterfaceQueryMaxResponseTimeValue)
    {
      /* Error is not shown instead shown while configuring QueryInterval. as both are executed when the form 
       * is submitted so error is shown once. Here the value is not passed to the lower layer. Simply return
       * success.
       */
      owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  } 
  /* For MLD, the max response time is factor of milli-seconds.*/
  else if (keymgmdRouterInterfaceQuerierTypeValue == L7_AF_INET6)
  {
    if ((objmgmdRouterInterfaceQueryIntervalValue *  MLD_RESP_TIME_FACTOR) <= objmgmdRouterInterfaceQueryMaxResponseTimeValue)
    {
      /* Error is not shown instead shown while configuring QueryInterval. as both are executed when the form
       * is submitted so error is shown once. Here the value is not passed to the lower layer. Simply return
       * success.
       */
      owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  /* set the value in application */
  owa.l7rc = usmDbMgmdInterfaceQueryMaxResponseTimeSet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              objmgmdRouterInterfaceQueryMaxResponseTimeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierUpTime
*
* @purpose Get 'mgmdRouterInterfaceQuerierUpTime'
*
* @description [mgmdRouterInterfaceQuerierUpTime] The time since mgmdRouterInterfaceQuerier was last changed.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierUpTime (void *wap,
                                                                                    void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceQuerierUpTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* This parmeter is valid only when the MGMD interface is operational.*/
  if(usmDbMgmdInterfaceOperationalStateGet(L7_UNIT_CURRENT,
                                           keymgmdRouterInterfaceQuerierTypeValue,
                                           keymgmdRouterInterfaceIfIndexValue) == L7_FALSE)
  {
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceQuerierUpTimeGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceQuerierUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceQuerierUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceQuerierUpTimeValue,
                           sizeof (objmgmdRouterInterfaceQuerierUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierExpiryTime
*
* @purpose Get 'mgmdRouterInterfaceQuerierExpiryTime'
*
* @description [mgmdRouterInterfaceQuerierExpiryTime] The amount of time remaining before the Other Querier Present Timer expires. If the local system is the querier, the value of this object is zero.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierExpiryTime (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceQuerierExpiryTimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* This parmeter is valid only when the MGMD interface is operational.*/
  if(usmDbMgmdInterfaceOperationalStateGet(L7_UNIT_CURRENT,
                                           keymgmdRouterInterfaceQuerierTypeValue,
                                           keymgmdRouterInterfaceIfIndexValue) == L7_FALSE)
  {
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceQuerierExpiryTimeGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceQuerierExpiryTimeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceQuerierExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceQuerierExpiryTimeValue,
                           sizeof (objmgmdRouterInterfaceQuerierExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceWrongVersionQueries
*
* @purpose Get 'mgmdRouterInterfaceWrongVersionQueries'
*
* @description [mgmdRouterInterfaceWrongVersionQueries] The number of general queries received whose IGMP or MLD version does not match the equivalent mgmdRouterInterfaceVersion, over the lifetime of the row entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceWrongVersionQueries (void *wap,
                                                                                          void
                                                                                          *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceWrongVersionQueriesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* This parmeter is valid only when the MGMD interface is operational.*/
  if(usmDbMgmdInterfaceOperationalStateGet(L7_UNIT_CURRENT,
                                           keymgmdRouterInterfaceQuerierTypeValue,
                                           keymgmdRouterInterfaceIfIndexValue) == L7_FALSE)
  {
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceWrongVersionQueriesGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceWrongVersionQueriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceWrongVersionQueries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceWrongVersionQueriesValue,
                           sizeof (objmgmdRouterInterfaceWrongVersionQueriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceJoins
*
* @purpose Get 'mgmdRouterInterfaceJoins'
*
* @description [mgmdRouterInterfaceJoins] The number of times a group membership has been added on this interface; that is, the number of times an entry for this interface has been added to the Cache Table.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceJoins (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceJoinsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceJoinsGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceJoinsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceJoins */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceJoinsValue,
                           sizeof (objmgmdRouterInterfaceJoinsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceProxyIfIndex
*
* @purpose Get 'mgmdRouterInterfaceProxyIfIndex'
*
* @description [mgmdRouterInterfaceProxyIfIndex] Some devices implement a form of IGMP or MLD proxying whereby memberships learned on the interface represented by this row, cause Host Membership Reports to be sent on the interface whose ifIndex value is given by this object
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceProxyIfIndex (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceProxyIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyInterfaceGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              &objmgmdRouterInterfaceProxyIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_MGMD_PROXY_NOT_ENBLD;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceProxyIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceProxyIfIndexValue,
                           sizeof (objmgmdRouterInterfaceProxyIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceGroups
*
* @purpose Get 'mgmdRouterInterfaceGroups'
*
* @description [mgmdRouterInterfaceGroups] The current number of entries for this interface in the RouterCache Table.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceGroups (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceGroupsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* This parmeter is valid only when the MGMD interface is operational.*/
  if(usmDbMgmdInterfaceOperationalStateGet(L7_UNIT_CURRENT,
                                           keymgmdRouterInterfaceQuerierTypeValue,
                                           keymgmdRouterInterfaceIfIndexValue) == L7_FALSE)
  {
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceGroupsGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceGroupsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceGroups */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceGroupsValue,
                           sizeof (objmgmdRouterInterfaceGroupsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceRobustness
*
* @purpose Get 'mgmdRouterInterfaceRobustness'
*
* @description [mgmdRouterInterfaceRobustness] The Robustness Variable allows tuning for the expected packet loss on a subnet. If a subnet is expected to be lossy, the Robustness Variable may be increased.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceRobustness (void *wap,
                                                                                 void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceRobustnessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceRobustnessGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceRobustnessValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceRobustness */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceRobustnessValue,
                           sizeof (objmgmdRouterInterfaceRobustnessValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceRobustness
*
* @purpose Set 'mgmdRouterInterfaceRobustness'
*
* @description [mgmdRouterInterfaceRobustness] The Robustness Variable allows tuning for the expected packet loss on a subnet. If a subnet is expected to be lossy, the Robustness Variable may be increased.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceRobustness (void *wap,
                                                                                 void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceRobustnessValue;

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdRouterInterfaceRobustness */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceRobustnessValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterInterfaceRobustnessValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* set the value in application */
  owa.l7rc = usmDbMgmdInterfaceRobustnessSet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              objmgmdRouterInterfaceRobustnessValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceLastMembQueryIntvl
*
* @purpose Get 'mgmdRouterInterfaceLastMembQueryIntvl'
*
* @description [mgmdRouterInterfaceLastMembQueryIntvl] The Last Member Query Interval is the Max Response Time inserted into Group-Specific Queries sent in response to Leave Group messages, and is also the amount of time between Group-Specific Query messages.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceLastMembQueryIntvl (void *wap,
                                                                                         void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceLastMembQueryIntvlValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceLastMembQueryIntervalGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceLastMembQueryIntvlValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceLastMembQueryIntvl */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceLastMembQueryIntvlValue,
                           sizeof (objmgmdRouterInterfaceLastMembQueryIntvlValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceLastMembQueryIntvl
*
* @purpose Set 'mgmdRouterInterfaceLastMembQueryIntvl'
*
* @description [mgmdRouterInterfaceLastMembQueryIntvl] The Last Member Query Interval is the Max Response Time inserted into Group-Specific Queries sent in response to Leave Group messages, and is also the amount of time between Group-Specific Query messages.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceLastMembQueryIntvl (void *wap,
                                                                                         void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceLastMembQueryIntvlValue;

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdRouterInterfaceLastMembQueryIntvl */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objmgmdRouterInterfaceLastMembQueryIntvlValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterInterfaceLastMembQueryIntvlValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* set the value in application */
  owa.l7rc = usmDbMgmdInterfaceLastMembQueryIntervalSet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              objmgmdRouterInterfaceLastMembQueryIntvlValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceLastMemberQuerycount
*
* @purpose Get 'mgmdRouterInterfaceLastMemberQuerycount'
*
* @description [mgmdRouterInterfaceLastMemberQuerycount] MGMD Interface last member Query count
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceLastMemberQuerycount (void
                                                                                           *wap,
                                                                                           void
                                                                                           *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceLastMemberQuerycountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdLastMembQueryCountGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceLastMemberQuerycountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceLastMemberQuerycount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceLastMemberQuerycountValue,
                           sizeof (objmgmdRouterInterfaceLastMemberQuerycountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceLastMemberQuerycount
*
* @purpose Set 'mgmdRouterInterfaceLastMemberQuerycount'
*
* @description [mgmdRouterInterfaceLastMemberQuerycount] MGMD Interface last member Query count
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceLastMemberQuerycount (void
                                                                                           *wap,
                                                                                           void
                                                                                           *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceLastMemberQuerycountValue;

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdRouterInterfaceLastMemberQuerycount */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objmgmdRouterInterfaceLastMemberQuerycountValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterInterfaceLastMemberQuerycountValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* set the value in application */
  owa.l7rc = usmDbMgmdLastMembQueryCountSet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              objmgmdRouterInterfaceLastMemberQuerycountValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceStartupQueryCount
*
* @purpose Get 'mgmdRouterInterfaceStartupQueryCount'
*
* @description [mgmdRouterInterfaceStartupQueryCount] MGMD Interface Startup Query count
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceStartupQueryCount (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceStartupQueryCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceStartupQueryCountGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceStartupQueryCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceStartupQueryCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceStartupQueryCountValue,
                           sizeof (objmgmdRouterInterfaceStartupQueryCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceStartupQueryCount
*
* @purpose Set 'mgmdRouterInterfaceStartupQueryCount'
*
* @description [mgmdRouterInterfaceStartupQueryCount] MGMD Interface Startup Query count
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceStartupQueryCount (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceStartupQueryCountValue;

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdRouterInterfaceStartupQueryCount */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objmgmdRouterInterfaceStartupQueryCountValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterInterfaceStartupQueryCountValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* set the value in application */
  owa.l7rc = usmDbMgmdInterfaceStartupQueryCountSet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              objmgmdRouterInterfaceStartupQueryCountValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceStartupQueryIntvl
*
* @purpose Get 'mgmdRouterInterfaceStartupQueryIntvl'
*
* @description [mgmdRouterInterfaceStartupQueryIntvl] MGMD Interface Startup Query interval
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceStartupQueryIntvl (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceStartupQueryIntvlValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceStartupQueryIntervalGet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              &objmgmdRouterInterfaceStartupQueryIntvlValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceStartupQueryIntvl */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceStartupQueryIntvlValue,
                           sizeof (objmgmdRouterInterfaceStartupQueryIntvlValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceStartupQueryIntvl
*
* @purpose Set 'mgmdRouterInterfaceStartupQueryIntvl'
*
* @description [mgmdRouterInterfaceStartupQueryIntvl] MGMD Interface Startup Query interval
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceStartupQueryIntvl (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceStartupQueryIntvlValue;

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdRouterInterfaceStartupQueryIntvl */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objmgmdRouterInterfaceStartupQueryIntvlValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterInterfaceStartupQueryIntvlValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* set the value in application */
  owa.l7rc = usmDbMgmdInterfaceStartupQueryIntervalSet (L7_UNIT_CURRENT, 
                              keymgmdRouterInterfaceQuerierTypeValue,
                              keymgmdRouterInterfaceIfIndexValue,
                              objmgmdRouterInterfaceStartupQueryIntvlValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceMode
*
* @purpose Get 'mgmdRouterInterfaceMode'
 *@description  [mgmdRouterInterfaceMode] Interface IGMP Mode   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceMode (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdInterfaceModeGet (L7_UNIT_CURRENT,
                                                                keymgmdRouterInterfaceQuerierTypeValue,
                                                                keymgmdRouterInterfaceIfIndexValue,
                                                                &objmgmdRouterInterfaceModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterInterfaceMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceModeValue,
                           sizeof (objmgmdRouterInterfaceModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceMode
*
* @purpose Set 'mgmdRouterInterfaceMode'
 *@description  [mgmdRouterInterfaceMode] Interface IGMP Mode   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterInterfaceModeValue;

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;
  xLibU32_t errCode;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdRouterInterfaceMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmgmdRouterInterfaceModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdRouterInterfaceModeValue, owa.len);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* set the value in application */
  owa.l7rc = usmDbMgmdInterfaceModeSet (L7_UNIT_CURRENT, 
                                                                keymgmdRouterInterfaceQuerierTypeValue,
                                                                keymgmdRouterInterfaceIfIndexValue,
                                                                objmgmdRouterInterfaceModeValue,
                                                                MGMD_ROUTER_INTERFACE, &errCode);

  if (owa.l7rc != L7_SUCCESS)
  {
    if (errCode == L7_MGMD_ERR_PROXY_INTF)
    {
      owa.rc = XLIBRC_MGMD_ROUTER_PROXY_ALREADY_SET;
    }
    else
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceClearCounters
*
* @purpose Set 'mgmdRouterInterfaceClearCounters'
*
* @description [mgmdRouterInterfaceClearCounters] Clear Counters
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceClearCounters (void *wap,
                                                                                    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  /* set the value in application */
  owa.l7rc = usmDbMldCountersClear (L7_UNIT_CURRENT, keymgmdRouterInterfaceIfIndexValue,
                              keymgmdRouterInterfaceQuerierTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceOperationalStatus
*
* @purpose Get 'mgmdRouterInterfaceOperationalStatus'
*
* @description [mgmdRouterInterfaceOperationalStatus] The activation of a row enables the router side of IGMP or MLD on the interface. The destruction of a row disables the router side of IGMP or MLD on the interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceOperationalStatus (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceIfIndexValue;
  fpObjWa_t kwamgmdRouterInterfaceQuerierType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterInterfaceQuerierTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdInterfaceOpStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterInterfaceIfIndex */
  kwamgmdRouterInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceIfIndex,
                   (xLibU8_t *) & keymgmdRouterInterfaceIfIndexValue,
                   &kwamgmdRouterInterfaceIfIndex.len);
  if (kwamgmdRouterInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceIfIndex);
    return kwamgmdRouterInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceIfIndexValue,
                           kwamgmdRouterInterfaceIfIndex.len);

  /* retrieve key: mgmdRouterInterfaceQuerierType */
  kwamgmdRouterInterfaceQuerierType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterInterfaceTable_mgmdRouterInterfaceQuerierType,
                   (xLibU8_t *) & keymgmdRouterInterfaceQuerierTypeValue,
                   &kwamgmdRouterInterfaceQuerierType.len);
  if (kwamgmdRouterInterfaceQuerierType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterInterfaceQuerierType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterInterfaceQuerierType);
    return kwamgmdRouterInterfaceQuerierType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterInterfaceQuerierTypeValue,
                           kwamgmdRouterInterfaceQuerierType.len);

  objmgmdInterfaceOpStatusValue = (xLibU32_t) usmDbMgmdInterfaceOperationalStateGet(L7_UNIT_CURRENT,
                                     keymgmdRouterInterfaceQuerierTypeValue,
                                     keymgmdRouterInterfaceIfIndexValue); 

  /* return the object value: objmgmdInterfaceOpStatusValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objmgmdInterfaceOpStatusValue ,
                           sizeof (objmgmdInterfaceOpStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


