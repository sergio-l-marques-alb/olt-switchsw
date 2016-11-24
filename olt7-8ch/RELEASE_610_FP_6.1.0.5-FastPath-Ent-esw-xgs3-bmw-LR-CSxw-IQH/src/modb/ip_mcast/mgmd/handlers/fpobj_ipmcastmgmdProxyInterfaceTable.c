
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastmgmdProxyInterfaceTable.c
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
#include "_xe_ipmcastmgmdProxyInterfaceTable_obj.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_igmp_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_mib_igmp_api.h"
#include "usmdb_util_api.h"



L7_RC_t getValidIntfCandidate_forProxy( xLibU32_t intIfNum, xLibU32_t *nextIntfNum, xLibU32_t protoType)
{
  L7_RC_t retValue;
  
  xLibU32_t mode = 0;
  xLibU32_t rtr_intf_mode = 0;
  
  if(intIfNum == 0)
  {
     retValue = usmDbValidIntIfNumFirstGet(nextIntfNum); 
  }
  else
  {
     retValue = usmDbValidIntIfNumNext( intIfNum, nextIntfNum);  
  }
  if( retValue == L7_SUCCESS)
  {
    do
    {
       if (usmDbIpRtrIntfModeGet(L7_UNIT_CURRENT, *nextIntfNum,
                                                  &rtr_intf_mode)  != L7_SUCCESS)
       {
          rtr_intf_mode = L7_DISABLE;
       }

       if (rtr_intf_mode == L7_ENABLE )
       {
          if (usmDbMgmdInterfaceModeGet(L7_UNIT_CURRENT, protoType, *nextIntfNum, &mode) != L7_SUCCESS)
          {
            mode  = L7_DISABLE;
          }
          if (mode == L7_DISABLE)
          {
            return L7_SUCCESS;
          }
       }
       intIfNum = *nextIntfNum; 
    }while(usmDbValidIntIfNumNext(intIfNum, nextIntfNum) == L7_SUCCESS);
  }
  return L7_FAILURE;
} 
  

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType
*
* @purpose Get 'mgmdProxyIntfType'
*
* @description [mgmdProxyIntfType] Proxy Interface Type - V4 or V6
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType (void *wap, void *bufp)
{

  xLibU32_t objmgmdProxyIntfTypeValue;
  xLibU32_t nextObjmgmdProxyIntfTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                          (xLibU8_t *) & objmgmdProxyIntfTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdProxyIntfTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else if (objmgmdProxyIntfTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdProxyIntfTypeValue, owa.len);
    nextObjmgmdProxyIntfTypeValue = L7_INET_ADDR_TYPE_IPV6;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdProxyIntfTypeValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdProxyIntfTypeValue, owa.len);

  /* return the object value: mgmdProxyIntfType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdProxyIntfTypeValue,
                           sizeof (objmgmdProxyIntfTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex
*
* @purpose Get 'mgmdProxyIntfIndex'
*
* @description [mgmdProxyIntfIndex] Valid MGMD Proxy interface number
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex (void *wap, void *bufp)
{

  xLibU32_t objmgmdProxyIntfTypeValue = 0;
  xLibU32_t objmgmdProxyIntfIndexValue = 0;
  xLibU32_t nextObjmgmdProxyIntfIndexValue = 0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t currentProxyInterface = L7_NULL;
  L7_RC_t  rc = L7_FAILURE;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                          (xLibU8_t *) & objmgmdProxyIntfTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  rc = usmDbMgmdProxyInterfaceGet(L7_UNIT_CURRENT, objmgmdProxyIntfTypeValue, 
                                  &currentProxyInterface);

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdProxyIntfTypeValue, owa.len);

  /* retrieve key: mgmdProxyIntfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                          (xLibU8_t *) & objmgmdProxyIntfIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    if (rc == L7_SUCCESS && currentProxyInterface != L7_NULL)
    {
      nextObjmgmdProxyIntfIndexValue = currentProxyInterface;
      owa.l7rc = L7_SUCCESS;
    }
    else
    {
      memset (&objmgmdProxyIntfTypeValue, 0, sizeof (objmgmdProxyIntfTypeValue));
      nextObjmgmdProxyIntfIndexValue = objmgmdProxyIntfIndexValue;
      owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_VLAN_INTF,
                                              0, &nextObjmgmdProxyIntfIndexValue);
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdProxyIntfIndexValue, owa.len);
    if (objmgmdProxyIntfIndexValue == currentProxyInterface)
    {
      owa.l7rc = L7_FAILURE;
    } 
    else
    {
      nextObjmgmdProxyIntfIndexValue = 0;
      owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_VLAN_INTF, 0,
                      objmgmdProxyIntfIndexValue, &nextObjmgmdProxyIntfIndexValue);
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdProxyIntfIndexValue, owa.len);

  /* return the object value: mgmdProxyIntfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdProxyIntfIndexValue,
                           sizeof (objmgmdProxyIntfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfMode
*
* @purpose Get 'mgmdProxyIntfMode'
*
* @description [mgmdProxyIntfMode] Administrative status of MGMD Proxy on the selected interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfMode (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyIntfModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyInterfaceModeGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyIntfModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyIntfMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyIntfModeValue,
                           sizeof (objmgmdProxyIntfModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfMode
*
* @purpose Set 'mgmdProxyIntfMode'
 *@description  [mgmdProxyIntfMode] Administrative status of MGMD Proxy on the
* selected interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyIntfModeValue;

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;
  xLibU32_t errCode;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdProxyIntfMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmgmdProxyIntfModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdProxyIntfModeValue, owa.len);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbMgmdInterfaceModeSet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, objmgmdProxyIntfModeValue,
                              MGMD_PROXY_INTERFACE, &errCode);

  if (owa.l7rc != L7_SUCCESS)
  {
    if (errCode == L7_MGMD_ERR_OTHER_MCAST_ROUTING_PROTOCOL_CONFIGURED)
    {
      owa.rc = XLIBRC_MGMD_PROXY_MROUTE_ALREADY_SET;
    }
    else if (errCode == L7_MGMD_ERR_MGMD_INTF)
    {
      if (keymgmdProxyIntfTypeValue == L7_AF_INET)
      {
        owa.rc = XLIBRC_MGMD_PROXY_IGMP_ROUTER_ALREADY_SET;
      }
      else if (keymgmdProxyIntfTypeValue == L7_AF_INET6)
      {
        owa.rc = XLIBRC_MGMD_PROXY_MLD_ROUTER_ALREADY_SET;
      }
    }
    else if (errCode == L7_MGMD_ERR_MCAST_FWD_DISABLED)
    {
      owa.rc = XLIBRC_MGMD_PROXY_MULTICAST_NOT_SET;
    }
    else if (errCode == L7_MGMD_ERR_ROUTING_NOT_ENABLED)
    {
      owa.rc = XLIBRC_MGMD_PROXY_ROUTING_NOT_SET;
    }
    else if (errCode == L7_MGMD_ERR_MGMD_DISABLED_ON_SYSTEM)
    {
      if (keymgmdProxyIntfTypeValue == L7_AF_INET)
      {
        owa.rc = XLIBRC_MGMD_PROXY_IGMP_NOT_SET;
      }
      else if (keymgmdProxyIntfTypeValue == L7_AF_INET6)
      {
        owa.rc = XLIBRC_MGMD_PROXY_MLD_NOT_SET;
      }
    }
    else if (errCode == L7_MGMD_PROXY_ALREADY_CONFIGURED)
    {
      owa.rc = XLIBRC_MGMD_PROXY_PROXY_OTHER_INTF_ALREADY_SET;
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
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfOperStatus
*
* @purpose Get 'mgmdProxyIntfOperStatus'
*
* @description [mgmdProxyIntfOperStatus] MGMD Proxy interface status
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfOperStatus (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyIntfOperStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  objmgmdProxyIntfOperStatusValue = usmDbMgmdProxyInterfaceOperationalStateGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue);

  /* return the object value: mgmdProxyIntfOperStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyIntfOperStatusValue,
                           sizeof (objmgmdProxyIntfOperStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIpAddress
*
* @purpose Get 'mgmdProxyIntfIpAddress'
*
* @description [mgmdProxyIntfIpAddress] The IP address of the MGMD proxy interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIpAddress (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyIntfIpAddressValue;
  xLibU32_t objmgmdProxyIntfIpMaskValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRtrIntfIpAddressGet (L7_UNIT_CURRENT,
                              keymgmdProxyIntfIndexValue, 
                              &objmgmdProxyIntfIpAddressValue, 
                              &objmgmdProxyIntfIpMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyIntfIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objmgmdProxyIntfIpAddressValue,
                           sizeof (&objmgmdProxyIntfIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIpMask
*
* @purpose Get 'mgmdProxyIntfIpMask'
*
* @description [mgmdProxyIntfIpMask] The mask of the MGMD proxy interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIpMask (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyIntfIpMaskValue;
  xLibU32_t objmgmdProxyIntfIpAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRtrIntfIpAddressGet (L7_UNIT_CURRENT,
                              keymgmdProxyIntfIndexValue, 
                              &objmgmdProxyIntfIpAddressValue, 
                              &objmgmdProxyIntfIpMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyIntfIpMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objmgmdProxyIntfIpMaskValue,
                           sizeof (objmgmdProxyIntfIpMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfUnsolicitedInterval
*
* @purpose Get 'mgmdProxyIntfUnsolicitedInterval'
*
* @description [mgmdProxyIntfUnsolicitedInterval] The Unsolicited Report Interval is the time between repetitions of a host's initial report of membership in a group
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfUnsolicitedInterval (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyIntfUnsolicitedIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyInterfaceUnsolicitedIntervalGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue,
                              &objmgmdProxyIntfUnsolicitedIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyIntfUnsolicitedInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyIntfUnsolicitedIntervalValue,
                           sizeof (objmgmdProxyIntfUnsolicitedIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfUnsolicitedInterval
*
* @purpose Set 'mgmdProxyIntfUnsolicitedInterval'
*
* @description [mgmdProxyIntfUnsolicitedInterval] The Unsolicited Report Interval is the time between repetitions of a host's initial report of membership in a group
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfUnsolicitedInterval (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyIntfUnsolicitedIntervalValue;

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: mgmdProxyIntfUnsolicitedInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmgmdProxyIntfUnsolicitedIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmgmdProxyIntfUnsolicitedIntervalValue, owa.len);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbMgmdProxyInterfaceUnsolicitedIntervalSet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, objmgmdProxyIntfUnsolicitedIntervalValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyInterfaceNumMcastGroups
*
* @purpose Get 'mgmdProxyInterfaceNumMcastGroups'
*
* @description [mgmdProxyInterfaceNumMcastGroups] The current number of multicast group entries for the IGMP Proxy interface in the cache table
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyInterfaceNumMcastGroups (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyInterfaceNumMcastGroupsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyInterfaceNumMcastGroupsGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue,
                              &objmgmdProxyInterfaceNumMcastGroupsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyInterfaceNumMcastGroups */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyInterfaceNumMcastGroupsValue,
                           sizeof (objmgmdProxyInterfaceNumMcastGroupsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyInterfaceRestartCount
*
* @purpose Get 'mgmdProxyInterfaceRestartCount'
*
* @description [mgmdProxyInterfaceRestartCount] The number of times the proxy was brought up
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyInterfaceRestartCount (void *wap,
                                                                                 void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyInterfaceRestartCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyInterfaceRestartCountGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyInterfaceRestartCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyInterfaceRestartCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyInterfaceRestartCountValue,
                           sizeof (objmgmdProxyInterfaceRestartCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV1QueriesRecvd
*
* @purpose Get 'mgmdProxyV1QueriesRecvd'
*
* @description [mgmdProxyV1QueriesRecvd] MGMD Proxy V1 Queries received
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV1QueriesRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyV1QueriesRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyV1QueriesRecvdGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyV1QueriesRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyV1QueriesRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyV1QueriesRecvdValue,
                           sizeof (objmgmdProxyV1QueriesRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV1ReportsRecvd
*
* @purpose Get 'mgmdProxyV1ReportsRecvd'
*
* @description [mgmdProxyV1ReportsRecvd] MGMD Proxy V1 Reports received
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV1ReportsRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyV1ReportsRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyV1ReportsRecvdGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyV1ReportsRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyV1ReportsRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyV1ReportsRecvdValue,
                           sizeof (objmgmdProxyV1ReportsRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV1ReportsSent
*
* @purpose Get 'mgmdProxyV1ReportsSent'
*
* @description [mgmdProxyV1ReportsSent] MGMD Proxy V1 Reports sent
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV1ReportsSent (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyV1ReportsSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyV1ReportsSentGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyV1ReportsSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyV1ReportsSent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyV1ReportsSentValue,
                           sizeof (objmgmdProxyV1ReportsSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV2QueriesRecvd
*
* @purpose Get 'mgmdProxyV2QueriesRecvd'
*
* @description [mgmdProxyV2QueriesRecvd] MGMD Proxy V2 Queries Received
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV2QueriesRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyV2QueriesRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyV2QueriesRecvdGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyV2QueriesRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyV2QueriesRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyV2QueriesRecvdValue,
                           sizeof (objmgmdProxyV2QueriesRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV2ReportsRecvd
*
* @purpose Get 'mgmdProxyV2ReportsRecvd'
*
* @description [mgmdProxyV2ReportsRecvd] MGMD Proxy V2 Reports Received
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV2ReportsRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyV2ReportsRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyV2ReportsRecvdGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyV2ReportsRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyV2ReportsRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyV2ReportsRecvdValue,
                           sizeof (objmgmdProxyV2ReportsRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV2ReportsSent
*
* @purpose Get 'mgmdProxyV2ReportsSent'
*
* @description [mgmdProxyV2ReportsSent] MGMD Proxy V2 Reports Sent
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV2ReportsSent (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyV2ReportsSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyV2ReportsSentGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyV2ReportsSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyV2ReportsSent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyV2ReportsSentValue,
                           sizeof (objmgmdProxyV2ReportsSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV2LeavesRecvd
*
* @purpose Get 'mgmdProxyV2LeavesRecvd'
*
* @description [mgmdProxyV2LeavesRecvd] MGMD Proxy V2 leaves received
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV2LeavesRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyV2LeavesRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyV2LeavesRecvdGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyV2LeavesRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyV2LeavesRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyV2LeavesRecvdValue,
                           sizeof (objmgmdProxyV2LeavesRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV2LeavesSent
*
* @purpose Get 'mgmdProxyV2LeavesSent'
*
* @description [mgmdProxyV2LeavesSent] MGMD Proxy V2 leaves sent
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV2LeavesSent (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyV2LeavesSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyV2LeavesSentGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyV2LeavesSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyV2LeavesSent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyV2LeavesSentValue,
                           sizeof (objmgmdProxyV2LeavesSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV3QueriesRecvd
*
* @purpose Get 'mgmdProxyV3QueriesRecvd'
*
* @description [mgmdProxyV3QueriesRecvd] MGMD Proxy V3 queries received
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV3QueriesRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyV3QueriesRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyV3QueriesRecvdGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyV3QueriesRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyV3QueriesRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyV3QueriesRecvdValue,
                           sizeof (objmgmdProxyV3QueriesRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV3ReportsRecvd
*
* @purpose Get 'mgmdProxyV3ReportsRecvd'
*
* @description [mgmdProxyV3ReportsRecvd] MGMD Proxy V3 reports received
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV3ReportsRecvd (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyV3ReportsRecvdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyV3ReportsRecvdGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyV3ReportsRecvdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyV3ReportsRecvd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyV3ReportsRecvdValue,
                           sizeof (objmgmdProxyV3ReportsRecvdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV3ReportsSent
*
* @purpose Get 'mgmdProxyV3ReportsSent'
*
* @description [mgmdProxyV3ReportsSent] MGMD Proxy V3 reports sent
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyV3ReportsSent (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdProxyV3ReportsSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdProxyV3ReportsSentGet (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue, &objmgmdProxyV3ReportsSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdProxyV3ReportsSent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdProxyV3ReportsSentValue,
                           sizeof (objmgmdProxyV3ReportsSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_ipmcastmgmdProxyInterfaceTable_mgmdProxyResetStatus
*
* @purpose Set 'mgmdProxyResetStatus'
*
* @description [mgmdProxyResetStatus] Resets the mgmd-proxy statistics parameters
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastmgmdProxyInterfaceTable_mgmdProxyResetStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibS32_t));

  fpObjWa_t kwamgmdProxyIntfType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfTypeValue;
  fpObjWa_t kwamgmdProxyIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdProxyIntfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdProxyIntfType */
  kwamgmdProxyIntfType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                   (xLibU8_t *) & keymgmdProxyIntfTypeValue, &kwamgmdProxyIntfType.len);
  if (kwamgmdProxyIntfType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfType);
    return kwamgmdProxyIntfType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfTypeValue, kwamgmdProxyIntfType.len);

  /* retrieve key: mgmdProxyIntfIndex */
  kwamgmdProxyIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfIndex,
                   (xLibU8_t *) & keymgmdProxyIntfIndexValue, &kwamgmdProxyIntfIndex.len);
  if (kwamgmdProxyIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdProxyIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdProxyIntfIndex);
    return kwamgmdProxyIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdProxyIntfIndexValue, kwamgmdProxyIntfIndex.len);

  if (usmDbMgmdProxyInterfaceOperationalStateGet (L7_UNIT_CURRENT,
                                     keymgmdProxyIntfTypeValue,
                                     keymgmdProxyIntfIndexValue) != L7_TRUE)
  {
    owa.rc = XLIBRC_MGMD_PROXY_NOT_OPERATIONAL_CLEAR_STATS;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbMgmdProxyResetHostStatus (L7_UNIT_CURRENT, keymgmdProxyIntfTypeValue,
                              keymgmdProxyIntfIndexValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyVlanIntfIndex
*
* @purpose Get 'mgmdProxyVlanIntfIndex'
*
* @description [mgmdProxyVlanIntfIndex] Valid MGMD Proxy Routing interface number
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdProxyInterfaceTable_mgmdProxyVlanIntfIndex (void *wap, void *bufp)
{
  xLibU32_t objmgmdHostCacheAddressTypeValue;
  xLibU32_t objmgmdHostCacheIfIndexValue;
  xLibU32_t nextObjmgmdHostCacheIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostCacheAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyIntfType,
                          (xLibU8_t *) & objmgmdHostCacheAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostCacheAddressTypeValue, owa.len);

  /* retrieve key: mgmdHostCacheIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdProxyInterfaceTable_mgmdProxyVlanIntfIndex,
                          (xLibU8_t *) & objmgmdHostCacheIfIndexValue, &owa.len);

  if (owa.rc == XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  owa.l7rc = usmDbMgmdProxyInterfaceGet(L7_UNIT_CURRENT,
                                   objmgmdHostCacheAddressTypeValue,
                                   &nextObjmgmdHostCacheIfIndexValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_MGMD_PROXY_NOT_ENBLD;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdHostCacheIfIndexValue, owa.len);

  /* return the object value: mgmdHostCacheIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdHostCacheIfIndexValue,
                           sizeof (objmgmdHostCacheIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



