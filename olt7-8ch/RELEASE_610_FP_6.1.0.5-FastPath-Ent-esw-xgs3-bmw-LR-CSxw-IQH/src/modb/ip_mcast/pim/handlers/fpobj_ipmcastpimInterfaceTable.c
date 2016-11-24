
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimInterfaceTable.c
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
#include "_xe_ipmcastpimInterfaceTable_obj.h"
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_common.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pim_api.h"
#endif

#include "usmdb_mib_pim_rfc5060_api.h"
#include "usmdb_ip_api.h"
#if defined (L7_IPV6_PACKAGE)
#include "usmdb_ip6_api.h"
#endif

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceIfIndex
*
* @purpose Get 'pimInterfaceIfIndex'
*
* @description [pimInterfaceIfIndex] The ifIndex value of this PIMSm interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceIfIndex (void *wap, void *bufp)
{

  xLibU32_t objpimInterfaceIPVersionValue;
  xLibU32_t objpimInterfaceIfIndexValue;
  xLibU32_t nextObjpimInterfaceIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIPVersion */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                          (xLibU8_t *) & objpimInterfaceIPVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimInterfaceIPVersionValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                          (xLibU8_t *) & objpimInterfaceIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimInterfaceIfIndexValue = 0;
	 if(objpimInterfaceIPVersionValue == L7_XUI_INET_VER_IPv4)
 	{
 	   owa.l7rc = usmDbIpMapValidIntfFirstGet(&nextObjpimInterfaceIfIndexValue);
 	}
#if defined (L7_IPV6_PACKAGE)
	 else if(objpimInterfaceIPVersionValue == L7_XUI_INET_VER_IPv6)
 	{
 	   owa.l7rc = usmDbIp6MapIntfValidFirstGet(&nextObjpimInterfaceIfIndexValue); 	
 	}
#endif
	 else
 	{
 	   owa.l7rc = L7_FAILURE;
 	}
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimInterfaceIfIndexValue, owa.len);

    nextObjpimInterfaceIfIndexValue = 0;
		
	if(objpimInterfaceIPVersionValue == L7_XUI_INET_VER_IPv4)
 	{
 	   owa.l7rc = usmDbIpMapValidIntfNextGet (objpimInterfaceIfIndexValue,
                                                                         &nextObjpimInterfaceIfIndexValue);
 	}
#if defined (L7_IPV6_PACKAGE)
	 else if(objpimInterfaceIPVersionValue == L7_XUI_INET_VER_IPv6)
 	{ 
 	   owa.l7rc = usmDbIp6MapIntfValidNextGet (objpimInterfaceIfIndexValue,
                                                                         &nextObjpimInterfaceIfIndexValue); 	
 	}
#endif
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimInterfaceIfIndexValue, owa.len);

  /* return the object value: pimInterfaceIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimInterfaceIfIndexValue,
                           sizeof (objpimInterfaceIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceIPVersion
*
* @purpose Get 'pimInterfaceIPVersion'
*
* @description [pimInterfaceIPVersion] The IP version of this PIM interface. A physical interface 
* may be configured in multiple modes concurrently, e.g. IPv4 and IPv6, however the traffic is considered to be logically separate.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceIPVersion (void *wap, void *bufp)
{
  xLibU32_t objpimInterfaceIPVersionValue;
  xLibU32_t nextObjpimInterfaceIPVersionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIPVersion */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                          (xLibU8_t *) & objpimInterfaceIPVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjpimInterfaceIPVersionValue = L7_XUI_INET_VER_IPv4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimInterfaceIPVersionValue, owa.len);
#if defined (L7_IPV6_PACKAGE)
    if (objpimInterfaceIPVersionValue == L7_XUI_INET_VER_IPv6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjpimInterfaceIPVersionValue = L7_XUI_INET_VER_IPv6;
      owa.l7rc = L7_SUCCESS;
    }
#else
    owa.l7rc = L7_FAILURE;
#endif
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimInterfaceIPVersionValue, owa.len);

  /* return the object value: pimInterfaceIPVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimInterfaceIPVersionValue,
                           sizeof (objpimInterfaceIPVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceAddressType
*
* @purpose Get 'pimInterfaceAddressType'
*
* @description [pimInterfaceAddressType] The address type of this PIM interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceAddressType (void *wap, void *bufp)
{

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);


  xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceAddressType,
                  (xLibU8_t *) & objpimInterfaceAddressTypeValue, &owa.len);

  /* get the value from application */
  if (owa.rc != XLIBRC_SUCCESS)
  {

    objpimInterfaceAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else if (objpimInterfaceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    objpimInterfaceAddressTypeValue = L7_INET_ADDR_TYPE_IPV6;
    owa.l7rc = L7_SUCCESS;
  }
  else if(objpimInterfaceAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceAddressTypeValue,
                           sizeof (objpimInterfaceAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceAddress
*
* @purpose Get 'pimInterfaceAddress'
*
* @description [pimInterfaceAddress] The primary IP address of this router on this PIM interface. The InetAddressType is given by the pimSmInterfaceAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceAddress (void *wap, void *bufp)
{

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimInterfaceAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbPimInterfaceIPAddressGet (L7_UNIT_CURRENT, keypimInterfaceIPVersionValue,
                                                                        keypimInterfaceIfIndexValue, &objpimInterfaceAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceAddressValue,
                           sizeof (objpimInterfaceAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceGenerationIDValue
*
* @purpose Get 'pimInterfaceGenerationIDValue'
*
* @description [pimInterfaceGenerationIDValue] The value of the Generation ID this router inserted in the last PIM Hello message it sent on this interface. This object is 0 if pimSmInterfaceUseGenerationID is set to FALSE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceGenerationIDValue (void *wap, void *bufp)
{

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceGenerationIDValueValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbPimInterfaceGenerationIDValueGet (keypimInterfaceIfIndexValue,
                                                                                    keypimInterfaceIPVersionValue,
                                                                                    &objpimInterfaceGenerationIDValueValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceGenerationIDValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceGenerationIDValueValue,
                           sizeof (objpimInterfaceGenerationIDValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceDR
*
* @purpose Get 'pimInterfaceDR'
*
* @description [pimInterfaceDR] The primary IP address of the Designated Router on this PIM interface. The InetAddressType is given by the pimSmInterfaceAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceDR (void *wap, void *bufp)
{

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimInterfaceDRValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  memset(&objpimInterfaceDRValue, 0x00, sizeof(objpimInterfaceDRValue));
  /* get the value from application */
  owa.l7rc = usmDbPimsmInterfaceDRGet (L7_UNIT_CURRENT, keypimInterfaceIPVersionValue, 
                                                              keypimInterfaceIfIndexValue, &objpimInterfaceDRValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceDR */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceDRValue,
                           sizeof (objpimInterfaceDRValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceDRPriority
*
* @purpose Get 'pimInterfaceDRPriority'
*
* @description [pimInterfaceDRPriority] Whether or not this router includes the DR Priority option in the PIM Hello messages it sends on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceDRPriority (void *wap, void *bufp)
{

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceDRPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmInterfaceDRPriorityGet (L7_UNIT_CURRENT, keypimInterfaceIPVersionValue,
                                                                        keypimInterfaceIfIndexValue, &objpimInterfaceDRPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceDRPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceDRPriorityValue,
                           sizeof (objpimInterfaceDRPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceDRPriority
*
* @purpose Set 'pimInterfaceDRPriority'
*
* @description [pimInterfaceDRPriority] Whether or not this router includes the DR Priority option in the PIM Hello messages it sends on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceDRPriority (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceDRPriorityValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceDRPriority */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceDRPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceDRPriorityValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbPimsmInterfaceDRPrioritySet (L7_UNIT_CURRENT,keypimInterfaceIPVersionValue,
                                                                        keypimInterfaceIfIndexValue, objpimInterfaceDRPriorityValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceDRPriorityEnabled
*
* @purpose Get 'pimInterfaceDRPriorityEnabled'
*
* @description [pimInterfaceDRPriorityEnabled] Whether or not this router includes the DR Priority option in the PIM Hello messages it sends on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceDRPriorityEnabled (void *wap, void *bufp)
{

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceDRPriorityEnabledValue = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbPimInterfaceUseDRPriorityGet (keypimInterfaceIfIndexValue,
                                                                              keypimInterfaceIPVersionValue,
                                                                              &objpimInterfaceDRPriorityEnabledValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceDRPriorityEnabled */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceDRPriorityEnabledValue,
                           sizeof (objpimInterfaceDRPriorityEnabledValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceHelloInterval
*
* @purpose Get 'pimInterfaceHelloInterval'
*
* @description [pimInterfaceHelloInterval] The frequency at which PIM Hello messages are transmitted on this interface. This object corresponds to the Hello_Period timer value defined in the PIM-SM specification [I-D.ietf-pim-sm-v2-new]. A value of 0 represents an 'infinite' interval, and in
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceHelloInterval (void *wap, void *bufp)
{

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceHelloIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbPimIntfHelloIntervalGet (L7_UNIT_CURRENT, keypimInterfaceIPVersionValue, 
                                                               keypimInterfaceIfIndexValue, &objpimInterfaceHelloIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceHelloInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceHelloIntervalValue,
                           sizeof (objpimInterfaceHelloIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceHelloInterval
*
* @purpose Set 'pimInterfaceHelloInterval'
*
* @description [pimInterfaceHelloInterval] The frequency at which PIM Hello messages are transmitted on this interface. This object corresponds to the Hello_Period timer value defined in the PIM-SM specification [I-D.ietf-pim-sm-v2-new]. A value of 0 represents an 'infinite' interval, and in
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceHelloInterval (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceHelloIntervalValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceHelloInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceHelloIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceHelloIntervalValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbPimIntfHelloIntervalSet (L7_UNIT_CURRENT, keypimInterfaceIPVersionValue,
                                                               keypimInterfaceIfIndexValue, objpimInterfaceHelloIntervalValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceTrigHelloInterval
*
* @purpose Get 'pimInterfaceTrigHelloInterval'
*
* @description [pimInterfaceTrigHelloInterval] The maximum time before this router sends a triggered PIM Hello message on this interface. This object corresponds to the 'Trigered_Hello_Delay' timer value defined in the PIM-SM specification [I-D.ietf-pim-sm-v2-new]. A value of 0 has no special me
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceTrigHelloInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceTrigHelloIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue,
                              &objpimInterfaceTrigHelloIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceTrigHelloInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceTrigHelloIntervalValue,
                           sizeof (objpimInterfaceTrigHelloIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceTrigHelloInterval
*
* @purpose Set 'pimInterfaceTrigHelloInterval'
*
* @description [pimInterfaceTrigHelloInterval] The maximum time before this router sends a triggered PIM Hello message on this interface. This object corresponds to the 'Trigered_Hello_Delay' timer value defined in the PIM-SM specification [I-D.ietf-pim-sm-v2-new]. A value of 0 has no special me
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceTrigHelloInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceTrigHelloIntervalValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceTrigHelloInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceTrigHelloIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceTrigHelloIntervalValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, objpimInterfaceTrigHelloIntervalValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceHelloHoldtime
*
* @purpose Get 'pimInterfaceHelloHoldtime'
*
* @description [pimInterfaceHelloHoldtime] The value set in the Holdtime field of PIM Hello messages transmitted on this interface. A value of 65535 represents an 'infinite' holdtime. Implementations are recommended to use a holdtime that is 3.5 times the value of pimSmInterfaceHelloInterval
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceHelloHoldtime (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceHelloHoldtimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, &objpimInterfaceHelloHoldtimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceHelloHoldtime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceHelloHoldtimeValue,
                           sizeof (objpimInterfaceHelloHoldtimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceHelloHoldtime
*
* @purpose Set 'pimInterfaceHelloHoldtime'
*
* @description [pimInterfaceHelloHoldtime] The value set in the Holdtime field of PIM Hello messages transmitted on this interface. A value of 65535 represents an 'infinite' holdtime. Implementations are recommended to use a holdtime that is 3.5 times the value of pimSmInterfaceHelloInterval
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceHelloHoldtime (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceHelloHoldtimeValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceHelloHoldtime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceHelloHoldtimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceHelloHoldtimeValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, objpimInterfaceHelloHoldtimeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceJoinPruneInterval
*
* @purpose Get 'pimInterfaceJoinPruneInterval'
*
* @description [pimInterfaceJoinPruneInterval] The frequency at which this router sends PIM Join/Prune messages on this PIM interface. This object corresponds to the 't_periodic' timer value defined in the PIM-SM specification [I-D.ietf-pim-sm-v2-new]. A value of 0 represents an 'infinite' inter
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceJoinPruneInterval (void *wap, void *bufp)
{

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceJoinPruneIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbPimInterfaceJoinPruneIntervalGet (L7_UNIT_CURRENT, 
                                                                                  keypimInterfaceIPVersionValue, keypimInterfaceIfIndexValue,
                                                                                  &objpimInterfaceJoinPruneIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceJoinPruneInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceJoinPruneIntervalValue,
                           sizeof (objpimInterfaceJoinPruneIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceJoinPruneInterval
*
* @purpose Set 'pimInterfaceJoinPruneInterval'
*
* @description [pimInterfaceJoinPruneInterval] The frequency at which this router sends PIM Join/Prune messages on this PIM interface. This object corresponds to the 't_periodic' timer value defined in the PIM-SM specification [I-D.ietf-pim-sm-v2-new]. A value of 0 represents an 'infinite' inter
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceJoinPruneInterval (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceJoinPruneIntervalValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceJoinPruneInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceJoinPruneIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceJoinPruneIntervalValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbPimsmInterfaceJoinPruneIntervalSet (L7_UNIT_CURRENT, keypimInterfaceIPVersionValue,
                                                                                  keypimInterfaceIfIndexValue, objpimInterfaceJoinPruneIntervalValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceJoinPruneHoldtime
*
* @purpose Get 'pimInterfaceJoinPruneHoldtime'
*
* @description [pimInterfaceJoinPruneHoldtime] The value inserted into the Holdtime field of a PIM Join/Prune message sent on this interface. A value of 65535 represents an 'infinite' holdtime. Implementations are recommended to use a holdtime that is 3.5 times the value of pimSmInterfaceJoinPruneHoldtime
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceJoinPruneHoldtime (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceJoinPruneHoldtimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue,
                              &objpimInterfaceJoinPruneHoldtimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceJoinPruneHoldtime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceJoinPruneHoldtimeValue,
                           sizeof (objpimInterfaceJoinPruneHoldtimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceJoinPruneHoldtime
*
* @purpose Set 'pimInterfaceJoinPruneHoldtime'
*
* @description [pimInterfaceJoinPruneHoldtime] The value inserted into the Holdtime field of a PIM Join/Prune message sent on this interface. A value of 65535 represents an 'infinite' holdtime. Implementations are recommended to use a holdtime that is 3.5 times the value of pimSmInterfaceJoinPruneHoldtime
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceJoinPruneHoldtime (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceJoinPruneHoldtimeValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceJoinPruneHoldtime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceJoinPruneHoldtimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceJoinPruneHoldtimeValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, objpimInterfaceJoinPruneHoldtimeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceDFElectionRobustness
*
* @purpose Get 'pimInterfaceDFElectionRobustness'
*
* @description [pimInterfaceDFElectionRobustness] The minimum number of PIM DF-Election messages that must be lost in order for DF election on this interface to fail.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceDFElectionRobustness (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceDFElectionRobustnessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue,
                              &objpimInterfaceDFElectionRobustnessValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceDFElectionRobustness */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceDFElectionRobustnessValue,
                           sizeof (objpimInterfaceDFElectionRobustnessValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceDFElectionRobustness
*
* @purpose Set 'pimInterfaceDFElectionRobustness'
*
* @description [pimInterfaceDFElectionRobustness] The minimum number of PIM DF-Election messages that must be lost in order for DF election on this interface to fail.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceDFElectionRobustness (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceDFElectionRobustnessValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceDFElectionRobustness */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceDFElectionRobustnessValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceDFElectionRobustnessValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue,
                              objpimInterfaceDFElectionRobustnessValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceLanDelayEnabled
*
* @purpose Get 'pimInterfaceLanDelayEnabled'
*
* @description [pimInterfaceLanDelayEnabled] Evaluates to TRUE if all routers on this interface are using the LAN Prune Delay option.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceLanDelayEnabled (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceLanDelayEnabledValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, &objpimInterfaceLanDelayEnabledValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceLanDelayEnabled */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceLanDelayEnabledValue,
                           sizeof (objpimInterfaceLanDelayEnabledValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfacePropagationDelay
*
* @purpose Get 'pimInterfacePropagationDelay'
*
* @description [pimInterfacePropagationDelay] The value this router inserts into the Propagation_Delay field of the LAN Prune Delay option in the PIM Hello messages it sends on this interface. This object is only used if pimSmInterfaceUseLanPruneDelay is set to TRUE. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfacePropagationDelay (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfacePropagationDelayValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, &objpimInterfacePropagationDelayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfacePropagationDelay */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfacePropagationDelayValue,
                           sizeof (objpimInterfacePropagationDelayValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfacePropagationDelay
*
* @purpose Set 'pimInterfacePropagationDelay'
*
* @description [pimInterfacePropagationDelay] The value this router inserts into the Propagation_Delay field of the LAN Prune Delay option in the PIM Hello messages it sends on this interface. This object is only used if pimSmInterfaceUseLanPruneDelay is set to TRUE. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfacePropagationDelay (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfacePropagationDelayValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfacePropagationDelay */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfacePropagationDelayValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfacePropagationDelayValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, objpimInterfacePropagationDelayValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceOverrideInterval
*
* @purpose Get 'pimInterfaceOverrideInterval'
*
* @description [pimInterfaceOverrideInterval] The value this router inserts into the Override_Interval field of the LAN Prune Delay option in the PIM Hello messages it sends on this interface. This object is only used if pimSmInterfaceUseLanPruneDelay is set to TRUE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceOverrideInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceOverrideIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, &objpimInterfaceOverrideIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceOverrideInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceOverrideIntervalValue,
                           sizeof (objpimInterfaceOverrideIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceOverrideInterval
*
* @purpose Set 'pimInterfaceOverrideInterval'
*
* @description [pimInterfaceOverrideInterval] The value this router inserts into the Override_Interval field of the LAN Prune Delay option in the PIM Hello messages it sends on this interface. This object is only used if pimSmInterfaceUseLanPruneDelay is set to TRUE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceOverrideInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceOverrideIntervalValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceOverrideInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceOverrideIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceOverrideIntervalValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, objpimInterfaceOverrideIntervalValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceEffectPropagDelay
*
* @purpose Get 'pimInterfaceEffectPropagDelay'
*
* @description [pimInterfaceEffectPropagDelay] The Effective Propagation Delay on this interface. This object is always 500 if pimSmInterfaceLanDelayEnabled is FALSE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceEffectPropagDelay (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceEffectPropagDelayValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue,
                              &objpimInterfaceEffectPropagDelayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceEffectPropagDelay */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceEffectPropagDelayValue,
                           sizeof (objpimInterfaceEffectPropagDelayValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceEffectOverrideIvl
*
* @purpose Get 'pimInterfaceEffectOverrideIvl'
*
* @description [pimInterfaceEffectOverrideIvl] The Effective Override Interval on this interface. This object is always 2500 if pimSmInterfaceLanDelayEnabled is FALSE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceEffectOverrideIvl (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceEffectOverrideIvlValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue,
                              &objpimInterfaceEffectOverrideIvlValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceEffectOverrideIvl */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceEffectOverrideIvlValue,
                           sizeof (objpimInterfaceEffectOverrideIvlValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceSuppressionEnabled
*
* @purpose Get 'pimInterfaceSuppressionEnabled'
*
* @description [pimInterfaceSuppressionEnabled] Whether join suppression is enabled on this interface. This object is always TRUE if pimSmInterfaceLanDelayEnabled is FALSE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceSuppressionEnabled (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceSuppressionEnabledValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue,
                              &objpimInterfaceSuppressionEnabledValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceSuppressionEnabled */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceSuppressionEnabledValue,
                           sizeof (objpimInterfaceSuppressionEnabledValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceBidirCapable
*
* @purpose Get 'pimInterfaceBidirCapable'
*
* @description [pimInterfaceBidirCapable] Evaluates to TRUE if all routers on this interface are using the Bidirectional-PIM Capable option.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceBidirCapable (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceBidirCapableValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, &objpimInterfaceBidirCapableValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceBidirCapable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceBidirCapableValue,
                           sizeof (objpimInterfaceBidirCapableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceDomainBorder
*
* @purpose Get 'pimInterfaceDomainBorder'
*
* @description [pimInterfaceDomainBorder] Whether or not this interface acts as a border for all PIM Bootstrap messages.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceDomainBorder (void *wap, void *bufp)
{

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceDomainBorderValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmInterfaceBsrBorderGet (L7_UNIT_CURRENT, keypimInterfaceIPVersionValue,
                                                                        keypimInterfaceIfIndexValue, &objpimInterfaceDomainBorderValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceDomainBorder */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceDomainBorderValue,
                           sizeof (objpimInterfaceDomainBorderValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceDomainBorder
*
* @purpose Set 'pimInterfaceDomainBorder'
*
* @description [pimInterfaceDomainBorder] Whether or not this interface acts as a border for all PIM Bootstrap messages.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceDomainBorder (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceDomainBorderValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceDomainBorder */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceDomainBorderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceDomainBorderValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbPimsmInterfaceBsrBorderSet (L7_UNIT_CURRENT, keypimInterfaceIPVersionValue, 
                                                                        keypimInterfaceIfIndexValue, objpimInterfaceDomainBorderValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceStubInterface
*
* @purpose Get 'pimInterfaceStubInterface'
*
* @description [pimInterfaceStubInterface] Whether this interface is a 'stub interface'.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceStubInterface (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceStubInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, &objpimInterfaceStubInterfaceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceStubInterface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceStubInterfaceValue,
                           sizeof (objpimInterfaceStubInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceStubInterface
*
* @purpose Set 'pimInterfaceStubInterface'
*
* @description [pimInterfaceStubInterface] Whether this interface is a 'stub interface'.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceStubInterface (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceStubInterfaceValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceStubInterface */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceStubInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceStubInterfaceValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, objpimInterfaceStubInterfaceValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfacePruneLimitInterval
*
* @purpose Get 'pimInterfacePruneLimitInterval'
*
* @description [pimInterfacePruneLimitInterval] The minimum interval that must transpire between two
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfacePruneLimitInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfacePruneLimitIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue,
                              &objpimInterfacePruneLimitIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfacePruneLimitInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfacePruneLimitIntervalValue,
                           sizeof (objpimInterfacePruneLimitIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfacePruneLimitInterval
*
* @purpose Set 'pimInterfacePruneLimitInterval'
*
* @description [pimInterfacePruneLimitInterval] The minimum interval that must transpire between two
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfacePruneLimitInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfacePruneLimitIntervalValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfacePruneLimitInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfacePruneLimitIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfacePruneLimitIntervalValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue,
                              objpimInterfacePruneLimitIntervalValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceGraftRetryInterval
*
* @purpose Get 'pimInterfaceGraftRetryInterval'
*
* @description [pimInterfaceGraftRetryInterval] The minimum interval that must transpire between two successive Grafts sent by a router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceGraftRetryInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceGraftRetryIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue,
                              &objpimInterfaceGraftRetryIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceGraftRetryInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceGraftRetryIntervalValue,
                           sizeof (objpimInterfaceGraftRetryIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceGraftRetryInterval
*
* @purpose Set 'pimInterfaceGraftRetryInterval'
*
* @description [pimInterfaceGraftRetryInterval] The minimum interval that must transpire between two successive Grafts sent by a router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceGraftRetryInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceGraftRetryIntervalValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceGraftRetryInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceGraftRetryIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceGraftRetryIntervalValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue,
                              objpimInterfaceGraftRetryIntervalValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceSRPriorityEnabled
*
* @purpose Get 'pimInterfaceSRPriorityEnabled'
*
* @description [pimInterfaceSRPriorityEnabled] Evaluates to TRUE if all routers on this interface are using the State Refresh option.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceSRPriorityEnabled (void *wap, void *bufp)
{

#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceSRPriorityEnabledValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue,
                              &objpimInterfaceSRPriorityEnabledValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceSRPriorityEnabled */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceSRPriorityEnabledValue,
                           sizeof (objpimInterfaceSRPriorityEnabledValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceStatus
*
* @purpose Get 'pimInterfaceStatus'
*
* @description [pimInterfaceStatus] The status of this entry. Creating the entry enables PIM on the interface; destroying the entry disables PIM on the interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceStatus (void *wap, void *bufp)
{

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceStatusValue;

  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  if((owa.l7rc = usmDbPimInterfaceModeGet(USMDB_UNIT_CURRENT,keypimInterfaceIPVersionValue,
                                                                    keypimInterfaceIfIndexValue, &mode))==L7_SUCCESS)
  {
    if(mode==L7_ENABLE)
    {
      objpimInterfaceStatusValue = L7_ADMIN_MODE_ENABLE;
    }
    else
    {
      objpimInterfaceStatusValue = L7_ADMIN_MODE_DISABLE;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceStatusValue,
                           sizeof (objpimInterfaceStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceStatus
*
* @purpose Set 'pimInterfaceStatus'
*
* @description [pimInterfaceStatus] The status of this entry. Creating the entry enables PIM on the interface; destroying the entry disables PIM on the interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceStatusValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceStatusValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  owa.l7rc = L7_SUCCESS;
  owa.l7rc = usmDbPimInterfaceModeSet(L7_UNIT_CURRENT, keypimInterfaceIPVersionValue,
                                      keypimInterfaceIfIndexValue, objpimInterfaceStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceOperStatus
*
* @purpose Get 'pimInterfaceOperStatus'
 *@description  [pimInterfaceOperStatus] The Operational status of this entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceOperStatus (void *wap, void *bufp)
{

  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceOperStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* get the value from application */
/*  if((owa.l7rc = usmDbPimInterfaceModeGet(USMDB_UNIT_CURRENT,keypimInterfaceIPVersionValue,
                                                                    keypimInterfaceIfIndexValue, &mode))==L7_SUCCESS) */

  {
    if (usmDbPimInterfaceOperationalStateGet(USMDB_UNIT_CURRENT, keypimInterfaceIPVersionValue,
                                                                       keypimInterfaceIfIndexValue) == L7_TRUE)
    {
      objpimInterfaceOperStatusValue = L7_ENABLE;
    }
    else
    {
      objpimInterfaceOperStatusValue = L7_DISABLE;
    }
  }

  /* return the object value: pimInterfaceOperStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceOperStatusValue,
                           sizeof (objpimInterfaceOperStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceStorageType
*
* @purpose Get 'pimInterfaceStorageType'
*
* @description [pimInterfaceStorageType] The storage type for this row. Rows having the value 'permanent' need not allow write-access to any columnar objects in the row.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, &objpimInterfaceStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceStorageTypeValue,
                           sizeof (objpimInterfaceStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimInterfaceTable_pimInterfaceStorageType
*
* @purpose Set 'pimInterfaceStorageType'
*
* @description [pimInterfaceStorageType] The storage type for this row. Rows having the value 'permanent' need not allow write-access to any columnar objects in the row.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimInterfaceTable_pimInterfaceStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceStorageTypeValue;

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimInterfaceStorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimInterfaceStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimInterfaceStorageTypeValue, owa.len);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimInterfaceIfIndexValue,
                              keypimInterfaceIPVersionValue, objpimInterfaceStorageTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}


/*******************************************************************************
* @function fpObjGet_ipmcastpimInterfaceTable_pimInterfaceNeighborCount
*
* @purpose Get 'pimInterfaceNeighborCount'
 *@description  [pimInterfaceNeighborCount] Neighbor Count for the specified
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimInterfaceTable_pimInterfaceNeighborCount (void *wap, void *bufp)
{

  fpObjWa_t kwapimInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIfIndexValue;
  fpObjWa_t kwapimInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimInterfaceIPVersionValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimInterfaceNeighborCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimInterfaceIfIndex */
  kwapimInterfaceIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIfIndex,
                                             (xLibU8_t *) & keypimInterfaceIfIndexValue,
                                             &kwapimInterfaceIfIndex.len);
  if (kwapimInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIfIndex);
    return kwapimInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIfIndexValue, kwapimInterfaceIfIndex.len);

  /* retrieve key: pimInterfaceIPVersion */
  kwapimInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimInterfaceTable_pimInterfaceIPVersion,
                   (xLibU8_t *) & keypimInterfaceIPVersionValue, &kwapimInterfaceIPVersion.len);
  if (kwapimInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwapimInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimInterfaceIPVersion);
    return kwapimInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimInterfaceIPVersionValue, kwapimInterfaceIPVersion.len);

  /* get the value from application */
  owa.l7rc = usmDbPimNeighborCountGet (L7_UNIT_CURRENT, keypimInterfaceIPVersionValue, 
                                                              keypimInterfaceIfIndexValue, &objpimInterfaceNeighborCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimInterfaceNeighborCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimInterfaceNeighborCountValue,
                           sizeof (objpimInterfaceNeighborCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
