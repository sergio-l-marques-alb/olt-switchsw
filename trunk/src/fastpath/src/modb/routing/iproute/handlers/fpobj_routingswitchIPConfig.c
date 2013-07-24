/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingswitchIPConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ipconfig-object.xml
*
* @create  6 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingswitchIPConfig_obj.h"
#include "usmdb_1213_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_util_api.h"
#include "usmdb_nim_api.h"

L7_RC_t usmDbIpRtrIntfSecondaryAddressEntryGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *netMask)
{
  L7_int32 temp_val;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_uint32 i; /* address loop index */

  /* check to see if this is a valid routing interface */
  if ( (usmDbValidateRtrIntf(UnitIndex, intIfNum) == L7_SUCCESS) &&
       (usmDbVisibleInterfaceCheck(UnitIndex, intIfNum, &temp_val) == L7_SUCCESS) &&
       (usmDbIpRtrIntfCfgIpAddrListGet(UnitIndex, intIfNum, ipAddrList) == L7_SUCCESS))
  {
    for (i=1; i<L7_L3_NUM_IP_ADDRS; i++)
    {
      if (ipAddrList[i].ipAddr == ipAddress)
      {
        *netMask = ipAddrList[i].ipMask;
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

L7_RC_t usmDbIpRtrIntfSecondaryAddressEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *ifIndex, L7_uint32 *ipAddress, L7_uint32 *netMask)
{
  L7_int32 temp_val;
  L7_int32 mode;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_rtrIntfIpAddr_t tempIpAddr;
  L7_uint32 i; /* address loop index */

  do
  {
    /* clear the temp value */
    tempIpAddr.ipAddr = 0;
    tempIpAddr.ipMask = 0;

    /* check to see if this is a valid routing interface */
    if (usmDbIpRtrIntfModeGet(UnitIndex, *ifIndex, &mode) == L7_SUCCESS &&
        usmDbValidateRtrIntf(UnitIndex, *ifIndex) == L7_SUCCESS &&
        usmDbVisibleInterfaceCheck(UnitIndex, *ifIndex, &temp_val) == L7_SUCCESS &&
        usmDbIpRtrIntfCfgIpAddrListGet(UnitIndex, *ifIndex, ipAddrList) == L7_SUCCESS)
    {
      /* find the next greater address for this interface */
      for (i=1; i<L7_L3_NUM_IP_ADDRS; i++)
      {
        /* if greater than requested address */
        if (ipAddrList[i].ipAddr > *ipAddress)
        {
          /* if first greater, or less than first found greater address */
          if (tempIpAddr.ipAddr == 0 ||
              ipAddrList[i].ipAddr < tempIpAddr.ipAddr)
          {
            tempIpAddr.ipAddr = ipAddrList[i].ipAddr;
            tempIpAddr.ipMask = ipAddrList[i].ipMask;
          }
        }
      }

      /* if an address is found, return */
      if (tempIpAddr.ipAddr != 0)
      {
        *ipAddress = tempIpAddr.ipAddr;
        *netMask = tempIpAddr.ipMask;
        return L7_SUCCESS;
      }
    }
  }
  /* loop through available external interface numbers */
  while (usmDbValidIntIfNumNext(*ifIndex, ifIndex) == L7_SUCCESS);

  return L7_FAILURE;
}

/*******************************************************************************
* @function fpObjGet_routingswitchIPConfig_IfIndex
*
* @purpose Get 'IfIndex'
 *@description  [IfIndex] The IfIndex associated with this instance combo-key
* needs to be removed   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPConfig_IfIndex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfIndexValue;
  xLibU32_t nextObjIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPConfig_IfIndex,
                          (xLibU8_t *) & objIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfIndexValue, owa.len);
  }
  
  owa.l7rc = usmDbValidIntIfNumNext(objIfIndexValue, &nextObjIfIndexValue);
 
    
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIfIndexValue, owa.len);

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIfIndexValue, sizeof (objIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchIPConfig_secondaryIpAddress
*
* @purpose Get 'secondaryIpAddress'
 *@description  [secondaryIpAddress] The Secondary IP Address assigned to this
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPConfig_secondaryIpAddress (void *wap, void *bufp)
{

  fpObjWa_t kwaIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t objsecondaryIpAddressValue;
  xLibIpV4_t nextObjsecondaryIpAddressValue;
  xLibS32_t temp_val;
  xLibS32_t mode;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  xLibU32_t i,j; /* address loop index */
  xLibBool_t firstItrFlag = XLIB_FALSE;
  
  FPOBJ_TRACE_ENTER (bufp);

  memset(&objsecondaryIpAddressValue,0,sizeof(objsecondaryIpAddressValue));
  memset(&nextObjsecondaryIpAddressValue,0,sizeof(nextObjsecondaryIpAddressValue));

  /* retrieve key: IfIndex */
  kwaIfIndex.rc = xLibFilterGet (wap, XOBJ_routingswitchIPConfig_IfIndex,
                                 (xLibU8_t *) & keyIfIndexValue, &kwaIfIndex.len);
  if (kwaIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIfIndex);
    return kwaIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwaIfIndex.len);

  /* retrieve key: secondaryIpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPConfig_secondaryIpAddress,
                          (xLibU8_t *) & objsecondaryIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objsecondaryIpAddressValue = 0;
    firstItrFlag = XLIB_TRUE;
  }

  nextObjsecondaryIpAddressValue = objsecondaryIpAddressValue;
  memset(ipAddrList, 0, sizeof(L7_rtrIntfIpAddr_t) * L7_L3_NUM_IP_ADDRS);
  if (usmDbIpRtrIntfModeGet(L7_UNIT_CURRENT, keyIfIndexValue, &mode) == L7_SUCCESS &&
      usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyIfIndexValue) == L7_SUCCESS &&
      usmDbVisibleInterfaceCheck(L7_UNIT_CURRENT, keyIfIndexValue, &temp_val) == L7_SUCCESS &&
      usmDbIpRtrIntfCfgIpAddrListGet(L7_UNIT_CURRENT, keyIfIndexValue, ipAddrList) == L7_SUCCESS)
  {
    if (firstItrFlag == XLIB_TRUE)
    {
      i=1;
      while (i<L7_L3_NUM_IP_ADDRS)
      {
        if (ipAddrList[i].ipAddr != 0)
        {
          nextObjsecondaryIpAddressValue = ipAddrList[i].ipAddr;
          owa.l7rc = L7_SUCCESS;
          break;
        }
        else if (i == (L7_L3_NUM_IP_ADDRS-1))
        {
          owa.l7rc = L7_FAILURE;
          break;
        }
        i++;
      }
    }
    else
    {
      i = 1;
      while (i<L7_L3_NUM_IP_ADDRS)
      {
        if (nextObjsecondaryIpAddressValue == ipAddrList[i].ipAddr)
        {
          break;
        }
        i++;
      }

      if (i == (L7_L3_NUM_IP_ADDRS-1))
      {
        owa.l7rc = L7_FAILURE;
      }
      else
      {
        for (j=i+1; j<L7_L3_NUM_IP_ADDRS; j++)
        {
          if (ipAddrList[j].ipAddr != 0)
          {
            nextObjsecondaryIpAddressValue = ipAddrList[j].ipAddr;
            owa.l7rc = L7_SUCCESS;
            break;
          }
        }
        if (j == L7_L3_NUM_IP_ADDRS)
        {
          owa.l7rc = L7_FAILURE;
        }
      }
    }
  }

 
  if ((owa.l7rc != L7_SUCCESS) || (nextObjsecondaryIpAddressValue == 0))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjsecondaryIpAddressValue, owa.len);

  /* return the object value: secondaryIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjsecondaryIpAddressValue,
                           sizeof (objsecondaryIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchIPConfig_secondaryNetMask
*
* @purpose Get 'secondaryNetMask'
*
* @description [secondaryNetMask]: The Secondary Net Mask assigned to this
*              interface. This value must be set during creation, and can
*              not be changed. 
*
* @return
*******************************************************************************/

xLibRC_t fpObjGet_routingswitchIPConfig_secondaryNetMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibIpV4_t objsecondaryNetMaskValue;

  xLibU32_t keyIfIndexValue;
  xLibIpV4_t keysecondaryIpAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.len = sizeof (keyIfIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPConfig_IfIndex,
                          (xLibU8_t *) & keyIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, owa.len);

  /* retrieve key: secondaryIpAddress */
  owa.len = sizeof (keysecondaryIpAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchIPConfig_secondaryIpAddress,
                          (xLibU8_t *) & keysecondaryIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysecondaryIpAddressValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRtrIntfSecondaryAddressEntryGet (L7_UNIT_CURRENT, keyIfIndexValue,
                              keysecondaryIpAddressValue, &objsecondaryNetMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objsecondaryNetMaskValue, sizeof (objsecondaryNetMaskValue));

  /* return the object value: secondaryNetMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsecondaryNetMaskValue,
                           sizeof (objsecondaryNetMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_routingswitchIPConfig_secondaryNetMask
*
* @purpose Set 'secondaryNetMask'
*
* @description [secondaryNetMask]: The Secondary Net Mask assigned to this
*              interface. This value must be set during creation, and can
*              not be changed. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPConfig_secondaryNetMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsecondaryNetMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: secondaryNetMask */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsecondaryNetMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsecondaryNetMaskValue, owa.len);
  return XLIBRC_SUCCESS;    /* TODO: Change if required */
}


/*******************************************************************************
* @function fpObjGet_routingswitchIPConfig_secondaryStatus
*
* @purpose Get 'secondaryStatus'
*
* @description [secondaryStatus]: Creates a new entry in the Secondary Address
*              table. Allowed values are: createAndGo(4) - Creates an
*              entry in this table, associating the address with a giv 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPConfig_secondaryStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keysecondaryIpAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsecondaryStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingswitchIPConfig_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: secondaryIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingswitchIPConfig_secondaryIpAddress,
                           (xLibU8_t *) & keysecondaryIpAddressValue,
                           &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysecondaryIpAddressValue, kwa2.len);

  /* get the value from application */
  objsecondaryStatusValue = L7_ROW_STATUS_ACTIVE; 

  
  /* return the object value: secondaryStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsecondaryStatusValue,
                           sizeof (objsecondaryStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_routingswitchIPConfig_secondaryStatus
*
* @purpose Set 'secondaryStatus'
*
* @description [secondaryStatus]: Creates a new entry in the Secondary Address
*              table. Allowed values are: createAndGo(4) - Creates an
*              entry in this table, associating the address with a giv 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIPConfig_secondaryStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsecondaryStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keysecondaryIpAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keysecondaryNetMaskValue;
  xLibU32_t intfType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: secondaryStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsecondaryStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsecondaryStatusValue, owa.len);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingswitchIPConfig_IfIndex,
                           (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: secondaryIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingswitchIPConfig_secondaryIpAddress,
                           (xLibU8_t *) & keysecondaryIpAddressValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysecondaryIpAddressValue, kwa2.len);

   /* retrieve key: secondaryNetMaskValue */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingswitchIPConfig_secondaryNetMask,
                           (xLibU8_t *) & keysecondaryNetMaskValue,
                           &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysecondaryNetMaskValue, kwa3.len);

  /* call the usmdb only for add and delete */
  if (objsecondaryStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    if ((usmDbIntfTypeGet(keyIfIndexValue, &intfType) == L7_SUCCESS) &&
            (intfType == L7_LOOPBACK_INTF))
    {
       if (usmDbNetmaskValidate32(keysecondaryNetMaskValue) != L7_SUCCESS)
       {
         /*Subnet mask must have contiguous ones and be no longer than 32 bits. */
         owa.rc = XLIBRC_INVALID_SUBNET_MASK32;
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;

       }
    }
    else if (usmDbNetmaskValidate(keysecondaryNetMaskValue) != L7_SUCCESS)
    {

      /* Subnet mask must have contiguous ones and be no longer than 30 bits. */
      owa.rc = XLIBRC_INVALID_SUBNET_MASK30;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;

    }

    /* Create a row */
    owa.l7rc =
      usmDbIpRtrIntfSecondaryIpAddrAdd(L7_UNIT_CURRENT, keyIfIndexValue,
                                           keysecondaryIpAddressValue,
                                           keysecondaryNetMaskValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      switch (owa.l7rc)
      {
        case L7_ERROR:
           owa.rc = XLIBRC_IP_ADDR_CONFLICT;
           break;
        case L7_NOT_SUPPORTED:
          owa.rc = XLIBRC_SUPPORTON_LOGICALINTF;
          break;
        case L7_TABLE_IS_FULL:
          owa.rc = XLIBRC_IPV4_INTF_ADDRESS_TABLE_FULL;
          break;
        case L7_ALREADY_CONFIGURED:
          owa.rc = XLIBRC_ADDRESS_ALREADY_CONFIGURED;
          break;
        case L7_REQUEST_DENIED:
          owa.rc = XLIBRC_NEXTHOPOF_STATIC_ROUTE_OR_ARP_ENTRY_EXIST;
          break;
        case L7_NOT_EXIST:
          owa.rc = XLIBRC_PRIMARY_ADDRESS_NOT_EXIST;
          break;
        default:
          owa.rc = XLIBRC_FAILURE;
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  }
  else if (objsecondaryStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc =
      usmDbIpRtrIntfSecondaryIpAddrRemove (L7_UNIT_CURRENT, keyIfIndexValue,
                                           keysecondaryIpAddressValue,
                                           keysecondaryNetMaskValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_routingswitchIPConfig_MaxSecAddressConfigured
*
* @purpose Get 'MaxSecAddressConfigured'
 *@description  [MaxSecAddressConfigured] <HTML>To check whether Max Secondary
* Addresses Configured
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIPConfig_MaxSecAddressConfigured (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objMaxSecAddressConfiguredValue;
  xLibU32_t keyIfIndexValue;
  xLibS32_t temp_val, mode, i;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];

  FPOBJ_TRACE_ENTER(bufp);


  /* retrieve key: IfIndex */
  owa.len = sizeof(keyIfIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_routingswitchIPConfig_IfIndex,
                              (xLibU8_t *) &keyIfIndexValue,
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyIfIndexValue, owa.len);

  /* get the value from application */
  memset(ipAddrList, 0, sizeof(L7_rtrIntfIpAddr_t) * L7_L3_NUM_IP_ADDRS);
  if (usmDbIpRtrIntfModeGet(L7_UNIT_CURRENT, keyIfIndexValue, &mode) == L7_SUCCESS &&
    usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyIfIndexValue) == L7_SUCCESS &&
    usmDbVisibleInterfaceCheck(L7_UNIT_CURRENT, keyIfIndexValue, &temp_val) == L7_SUCCESS &&
    usmDbIpRtrIntfCfgIpAddrListGet(L7_UNIT_CURRENT, keyIfIndexValue, ipAddrList) == L7_SUCCESS)
  {
    i=0;
    while (i<L7_L3_NUM_IP_ADDRS)
    {
      if (ipAddrList[i].ipAddr == 0)
        break;
      else
        i++;
    }

    if (i == L7_L3_NUM_IP_ADDRS)
      objMaxSecAddressConfiguredValue = XLIB_TRUE;
    else
      objMaxSecAddressConfiguredValue = XLIB_FALSE;

    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    objMaxSecAddressConfiguredValue = XLIB_FALSE;
    owa.l7rc = L7_FAILURE;
  }


  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objMaxSecAddressConfiguredValue, sizeof(objMaxSecAddressConfiguredValue));

  /* return the object value: MaxSecAddressConfigured */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objMaxSecAddressConfiguredValue,
                      sizeof(objMaxSecAddressConfiguredValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

