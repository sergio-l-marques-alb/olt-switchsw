
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimGroupMappingTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  09 May 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimGroupMappingTable_obj.h"
#include "usmdb_mib_pimsm_api.h"
/*******************************************************************************
* @function fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingOrigin
*
* @purpose Get 'pimGroupMappingOrigin'
*
* @description [pimGroupMappingOrigin] The mechanism by which this group mapping was learned.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingOrigin (void *wap, void *bufp)
{

  xLibU32_t objpimGroupMappingOriginValue;
  xLibU32_t nextObjpimGroupMappingOriginValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimGroupMappingOrigin */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingOrigin,
                          (xLibU8_t *) & objpimGroupMappingOriginValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimGroupMappingOriginValue = L7_XUI_PIM_GRP_MAPPING_ORIGIN_TYPE_FIXED;
    owa.l7rc =L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimGroupMappingOriginValue, owa.len);

    if (objpimGroupMappingOriginValue == L7_XUI_PIM_GRP_MAPPING_ORIGIN_TYPE_OTHER)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjpimGroupMappingOriginValue = objpimGroupMappingOriginValue + 1;
      owa.l7rc =L7_SUCCESS;
    }
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimGroupMappingOriginValue, owa.len);

  /* return the object value: pimGroupMappingOrigin */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimGroupMappingOriginValue,
                           sizeof (objpimGroupMappingOriginValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingAddressType
*
* @purpose Get 'pimGroupMappingAddressType'
*
* @description [pimGroupMappingAddressType] The address type of the IP multicast group prefix.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimGroupMappingAddressTypeValue;
  xLibU32_t nextObjpimGroupMappingAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimGroupMappingAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingAddressType,
                          (xLibU8_t *) & objpimGroupMappingAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimGroupMappingAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimGroupMappingAddressTypeValue, owa.len);

    if (objpimGroupMappingAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjpimGroupMappingAddressTypeValue = objpimGroupMappingAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimGroupMappingAddressTypeValue, owa.len);

  /* return the object value: pimGroupMappingAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimGroupMappingAddressTypeValue,
                           sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingGrpAddress
*
* @purpose Get 'pimGroupMappingGrpAddress'
*
* @description [pimGroupMappingGrpAddress] The IP multicast group address which, when combined 
*   with pimSmGroupMappingGrpPrefixLength, gives the group prefix for this mapping. The InetAddressType 
*   is given by the pimSmGroupMappingAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingGrpAddress (void *wap, void *bufp)
{
  xLibU32_t objpimGroupMappingOriginValue;
  xLibU32_t nextObjpimGroupMappingOriginValue=0;

  xLibU32_t objpimGroupMappingAddressTypeValue;

  L7_inet_addr_t objpimGroupMappingGrpAddressValue;
  L7_inet_addr_t nextObjpimGroupMappingGrpAddressValue;

  xLibU8_t objpimGroupMappingGrpPrefixLengthValue;
  xLibU8_t nextObjpimGroupMappingGrpPrefixLengthValue;

  L7_inet_addr_t objpimGroupMappingRPAddressValue;
  L7_inet_addr_t nextObjpimGroupMappingRPAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimGroupMappingOrigin */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingOrigin,
                          (xLibU8_t *) & objpimGroupMappingOriginValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimGroupMappingOriginValue, owa.len);

  /* retrieve key: pimGroupMappingAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingAddressType,
                          (xLibU8_t *) & objpimGroupMappingAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimGroupMappingAddressTypeValue, owa.len);


  /* retrieve key: pimGroupMappingGrpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingGrpAddress,
                          (xLibU8_t *) &objpimGroupMappingGrpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    memset(&objpimGroupMappingGrpAddressValue, 0, sizeof(L7_inet_addr_t));
    memset(&nextObjpimGroupMappingGrpAddressValue, 0, sizeof(L7_inet_addr_t));
    memset(&objpimGroupMappingRPAddressValue, 0, sizeof(L7_inet_addr_t));
    memset(&nextObjpimGroupMappingRPAddressValue,0, sizeof(L7_inet_addr_t));

    objpimGroupMappingGrpPrefixLengthValue = nextObjpimGroupMappingGrpPrefixLengthValue = 0;
      
    owa.l7rc =
      usmDbPimsmGroupMappingEntryNextGet ((xLibU8_t*)&objpimGroupMappingOriginValue, objpimGroupMappingAddressTypeValue,
                                                                    &nextObjpimGroupMappingGrpAddressValue, &objpimGroupMappingGrpPrefixLengthValue,
                                                                    &nextObjpimGroupMappingRPAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimGroupMappingGrpAddressValue, owa.len);

    objpimGroupMappingGrpPrefixLengthValue = nextObjpimGroupMappingGrpPrefixLengthValue = 0;
    memset(&objpimGroupMappingRPAddressValue, 0, sizeof(L7_inet_addr_t));
    memset(&nextObjpimGroupMappingRPAddressValue, 0, sizeof(L7_inet_addr_t));
    
    memcpy(&nextObjpimGroupMappingGrpAddressValue, &objpimGroupMappingGrpAddressValue, sizeof(L7_inet_addr_t));
    
    do
    {
      owa.l7rc =
        usmDbPimsmGroupMappingEntryNextGet ((xLibU8_t*)&objpimGroupMappingOriginValue, objpimGroupMappingAddressTypeValue,
                                                                    &nextObjpimGroupMappingGrpAddressValue, &objpimGroupMappingGrpPrefixLengthValue,
                                                                    &nextObjpimGroupMappingRPAddressValue);
    }
    while ((objpimGroupMappingOriginValue == nextObjpimGroupMappingOriginValue)
           && (!memcmp(&objpimGroupMappingGrpAddressValue, &nextObjpimGroupMappingGrpAddressValue,sizeof(L7_inet_addr_t)))
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objpimGroupMappingOriginValue != nextObjpimGroupMappingOriginValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimGroupMappingGrpAddressValue, owa.len);

  /* return the object value: pimGroupMappingGrpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimGroupMappingGrpAddressValue,
                                       sizeof(L7_inet_addr_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingGrpPrefixLength
*
* @purpose Get 'pimGroupMappingGrpPrefixLength'
*
* @description [pimGroupMappingGrpPrefixLength] The IP multicast group address which, when combined with pimSmGroupMappingGrpPrefixLength, gives the group prefix for this mapping. The InetAddressType is given by the pimSmGroupMappingAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingGrpPrefixLength (void *wap, void *bufp)
{
  xLibU32_t objpimGroupMappingGrpPrefixLengthValue;
  xLibU32_t nextObjpimGroupMappingGrpPrefixLengthValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimGroupMappingGrpPrefixLength */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingGrpPrefixLength,
                          (xLibU8_t *) & objpimGroupMappingGrpPrefixLengthValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = L7_FAILURE;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimGroupMappingGrpPrefixLengthValue, owa.len);
    nextObjpimGroupMappingGrpPrefixLengthValue = objpimGroupMappingGrpPrefixLengthValue;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimGroupMappingGrpPrefixLengthValue, owa.len);

  /* return the object value: pimGroupMappingGrpPrefixLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimGroupMappingGrpPrefixLengthValue,
                           sizeof (objpimGroupMappingGrpPrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingRPAddressType
*
* @purpose Get 'pimGroupMappingRPAddressType'
*
* @description [pimGroupMappingRPAddressType] The address type of the RP to be used for groups within this group prefix, or unknown(0) if no RP is to be used or if the RP address is unknown. This object must be unknown(0) if pimGroupMappingPimMode is ssm(2), or if pimGroupMappingOrigin is embedded(6).
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingRPAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimGroupMappingRPAddressTypeValue;
  xLibU32_t nextObjpimGroupMappingRPAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimGroupMappingRPAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingRPAddressType,
                          (xLibU8_t *) & objpimGroupMappingRPAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimGroupMappingRPAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimGroupMappingRPAddressTypeValue, owa.len);

    if (objpimGroupMappingRPAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjpimGroupMappingRPAddressTypeValue = objpimGroupMappingRPAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimGroupMappingRPAddressTypeValue, owa.len);

  /* return the object value: pimGroupMappingAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimGroupMappingRPAddressTypeValue,
                           sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingRPAddress
*
* @purpose Get 'pimGroupMappingRPAddress'
*
* @description [pimGroupMappingRPAddress] The IP address of the RP to be used for groups within this group prefix, or zero if no RP is to be used or if the RP address is unknown. The InetAddressType is given by the pimSmGroupMappingAddressType object. This object must be zero if pimSmGroupM
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingRPAddress (void *wap, void *bufp)
{
  xLibU32_t objpimGroupMappingOriginValue;
  xLibU32_t nextObjpimGroupMappingOriginValue=0;

  xLibU32_t objpimGroupMappingAddressTypeValue;
  
  L7_inet_addr_t objpimGroupMappingGrpAddressValue;
  L7_inet_addr_t nextObjpimGroupMappingGrpAddressValue;
  
  xLibU8_t objpimGroupMappingGrpPrefixLengthValue;
  xLibU8_t nextObjpimGroupMappingGrpPrefixLengthValue=0;

  L7_inet_addr_t objpimGroupMappingRPAddressValue;
  L7_inet_addr_t nextObjpimGroupMappingRPAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimGroupMappingOrigin */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingOrigin,
                          (xLibU8_t *) & objpimGroupMappingOriginValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimGroupMappingOriginValue, owa.len);

  /* retrieve key: pimGroupMappingGrpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingGrpAddress,
                          (xLibU8_t *) &objpimGroupMappingGrpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimGroupMappingGrpAddressValue, owa.len);
  /* retrieve key: pimGroupMappingGrpPrefixLength */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingGrpPrefixLength,
                          (xLibU8_t *) & objpimGroupMappingGrpPrefixLengthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimGroupMappingGrpPrefixLengthValue, owa.len);

  /* retrieve key: pimGroupMappingAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingAddressType,
                          (xLibU8_t *) & objpimGroupMappingAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimGroupMappingAddressTypeValue, owa.len);

  /* retrieve key: pimGroupMappingRPAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingRPAddress,
                          (xLibU8_t *)&objpimGroupMappingRPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    memset(&nextObjpimGroupMappingRPAddressValue, 0, sizeof(L7_inet_addr_t));
    
    owa.l7rc =
      usmDbPimsmGroupMappingEntryNextGet ((xLibU8_t*)&objpimGroupMappingOriginValue, objpimGroupMappingAddressTypeValue,
                                                                  &nextObjpimGroupMappingGrpAddressValue, &objpimGroupMappingGrpPrefixLengthValue,
                                                                  &nextObjpimGroupMappingRPAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimGroupMappingRPAddressValue, owa.len);
    memcpy(&nextObjpimGroupMappingRPAddressValue, &objpimGroupMappingRPAddressValue, sizeof(L7_inet_addr_t));

    owa.l7rc =
      usmDbPimsmGroupMappingEntryNextGet ((xLibU8_t*)&objpimGroupMappingOriginValue, objpimGroupMappingAddressTypeValue,
                                                                  &nextObjpimGroupMappingGrpAddressValue, &objpimGroupMappingGrpPrefixLengthValue,
                                                                  &nextObjpimGroupMappingRPAddressValue);
  }

  if ((objpimGroupMappingOriginValue != nextObjpimGroupMappingOriginValue)
      || (memcmp(&objpimGroupMappingGrpAddressValue, &nextObjpimGroupMappingGrpAddressValue, sizeof(L7_inet_addr_t)))
      || (objpimGroupMappingGrpPrefixLengthValue != nextObjpimGroupMappingGrpPrefixLengthValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimGroupMappingRPAddressValue, owa.len);

  /* return the object value: pimGroupMappingRPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimGroupMappingRPAddressValue,
                           sizeof (objpimGroupMappingRPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingPimMode
*
* @purpose Get 'pimGroupMappingPimMode'
*
* @description [pimGroupMappingPimMode] The PIM mode to be used for groups in this group prefix.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingPimMode (void *wap, void *bufp)
{

  fpObjWa_t kwapimGroupMappingOrigin = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimGroupMappingOriginValue;

  fpObjWa_t kwapimGroupMappingAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimGroupMappingAddressTypeValue;

  fpObjWa_t kwapimGroupMappingGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimGroupMappingGrpAddressValue;

  fpObjWa_t kwapimGroupMappingGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimGroupMappingGrpPrefixLengthValue;

  fpObjWa_t kwapimGroupMappingRPAddressType = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimGroupMappingRPAddressTypeValue;

  fpObjWa_t kwapimGroupMappingRPAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimGroupMappingRPAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimGroupMappingPimModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimGroupMappingOrigin */
  kwapimGroupMappingOrigin.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingOrigin,
                   (xLibU8_t *) & keypimGroupMappingOriginValue, &kwapimGroupMappingOrigin.len);
  if (kwapimGroupMappingOrigin.rc != XLIBRC_SUCCESS)
  {
    kwapimGroupMappingOrigin.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimGroupMappingOrigin);
    return kwapimGroupMappingOrigin.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimGroupMappingOriginValue, kwapimGroupMappingOrigin.len);

  /* retrieve key: pimGroupMappingAddressType */
  kwapimGroupMappingAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingAddressType,
                   (xLibU8_t *) & keypimGroupMappingAddressTypeValue,
                   &kwapimGroupMappingAddressType.len);
  if (kwapimGroupMappingAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimGroupMappingAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimGroupMappingAddressType);
    return kwapimGroupMappingAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimGroupMappingAddressTypeValue,
                           kwapimGroupMappingAddressType.len);

  /* retrieve key: pimGroupMappingGrpAddress */
  kwapimGroupMappingGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingGrpAddress,
                   (xLibU8_t *) &keypimGroupMappingGrpAddressValue,
                   &kwapimGroupMappingGrpAddress.len);
  if (kwapimGroupMappingGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimGroupMappingGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimGroupMappingGrpAddress);
    return kwapimGroupMappingGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimGroupMappingGrpAddressValue,
                           kwapimGroupMappingGrpAddress.len);

  /* retrieve key: pimGroupMappingGrpPrefixLength */
  kwapimGroupMappingGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingGrpPrefixLength,
                   (xLibU8_t *) & keypimGroupMappingGrpPrefixLengthValue,
                   &kwapimGroupMappingGrpPrefixLength.len);
  if (kwapimGroupMappingGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimGroupMappingGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimGroupMappingGrpPrefixLength);
    return kwapimGroupMappingGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimGroupMappingGrpPrefixLengthValue,
                           kwapimGroupMappingGrpPrefixLength.len);

  /* retrieve key: pimGroupMappingRPAddressType */
  kwapimGroupMappingRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingRPAddressType,
                   (xLibU8_t *) keypimGroupMappingRPAddressTypeValue,
                   &kwapimGroupMappingRPAddressType.len);
  if (kwapimGroupMappingRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimGroupMappingRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimGroupMappingRPAddressType);
    return kwapimGroupMappingRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimGroupMappingRPAddressTypeValue,
                           kwapimGroupMappingRPAddressType.len);

  /* retrieve key: pimGroupMappingRPAddress */
  kwapimGroupMappingRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingRPAddress,
                   (xLibU8_t *) & keypimGroupMappingRPAddressValue,
                   &kwapimGroupMappingRPAddress.len);
  if (kwapimGroupMappingRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimGroupMappingRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimGroupMappingRPAddress);
    return kwapimGroupMappingRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimGroupMappingRPAddressValue,
                           kwapimGroupMappingRPAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmGroupMappingPimModeGet (keypimGroupMappingOriginValue,
                                                                              keypimGroupMappingAddressTypeValue,
                                                                              &keypimGroupMappingGrpAddressValue,
                                                                              keypimGroupMappingGrpPrefixLengthValue,
                                                                              &keypimGroupMappingRPAddressValue, &objpimGroupMappingPimModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimGroupMappingPimMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimGroupMappingPimModeValue,
                           sizeof (objpimGroupMappingPimModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingPrecedence
*
* @purpose Get 'pimGroupMappingPrecedence'
*
* @description [pimGroupMappingPrecedence] The precedence of this row, used in the algorithm that determines which row applies to a given group address.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimGroupMappingTable_pimGroupMappingPrecedence (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimGroupMappingOrigin = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimGroupMappingOriginValue;
  fpObjWa_t kwapimGroupMappingAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimGroupMappingAddressTypeValue;
  fpObjWa_t kwapimGroupMappingGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimGroupMappingGrpAddressValue;
  fpObjWa_t kwapimGroupMappingGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimGroupMappingGrpPrefixLengthValue;
  fpObjWa_t kwapimGroupMappingRPAddressType = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimGroupMappingRPAddressTypeValue;
  fpObjWa_t kwapimGroupMappingRPAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimGroupMappingRPAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimGroupMappingPrecedenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimGroupMappingOrigin */
  kwapimGroupMappingOrigin.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingOrigin,
                   (xLibU8_t *) & keypimGroupMappingOriginValue, &kwapimGroupMappingOrigin.len);
  if (kwapimGroupMappingOrigin.rc != XLIBRC_SUCCESS)
  {
    kwapimGroupMappingOrigin.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimGroupMappingOrigin);
    return kwapimGroupMappingOrigin.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimGroupMappingOriginValue, kwapimGroupMappingOrigin.len);

  /* retrieve key: pimGroupMappingAddressType */
  kwapimGroupMappingAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingAddressType,
                   (xLibU8_t *) & keypimGroupMappingAddressTypeValue,
                   &kwapimGroupMappingAddressType.len);
  if (kwapimGroupMappingAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimGroupMappingAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimGroupMappingAddressType);
    return kwapimGroupMappingAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimGroupMappingAddressTypeValue,
                           kwapimGroupMappingAddressType.len);

  /* retrieve key: pimGroupMappingGrpAddress */
  kwapimGroupMappingGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingGrpAddress,
                   (xLibU8_t *) keypimGroupMappingGrpAddressValue,
                   &kwapimGroupMappingGrpAddress.len);
  if (kwapimGroupMappingGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimGroupMappingGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimGroupMappingGrpAddress);
    return kwapimGroupMappingGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimGroupMappingGrpAddressValue,
                           kwapimGroupMappingGrpAddress.len);

  /* retrieve key: pimGroupMappingGrpPrefixLength */
  kwapimGroupMappingGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingGrpPrefixLength,
                   (xLibU8_t *) & keypimGroupMappingGrpPrefixLengthValue,
                   &kwapimGroupMappingGrpPrefixLength.len);
  if (kwapimGroupMappingGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimGroupMappingGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimGroupMappingGrpPrefixLength);
    return kwapimGroupMappingGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimGroupMappingGrpPrefixLengthValue,
                           kwapimGroupMappingGrpPrefixLength.len);

  /* retrieve key: pimGroupMappingRPAddressType */
  kwapimGroupMappingRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingRPAddressType,
                   (xLibU8_t *) keypimGroupMappingRPAddressTypeValue,
                   &kwapimGroupMappingRPAddressType.len);
  if (kwapimGroupMappingRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimGroupMappingRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimGroupMappingRPAddressType);
    return kwapimGroupMappingRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimGroupMappingRPAddressTypeValue,
                           kwapimGroupMappingRPAddressType.len);

  /* retrieve key: pimGroupMappingRPAddress */
  kwapimGroupMappingRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimGroupMappingTable_pimGroupMappingRPAddress,
                   (xLibU8_t *) & keypimGroupMappingRPAddressValue,
                   &kwapimGroupMappingRPAddress.len);
  if (kwapimGroupMappingRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimGroupMappingRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimGroupMappingRPAddress);
    return kwapimGroupMappingRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimGroupMappingRPAddressValue,
                           kwapimGroupMappingRPAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimGroupMappingOriginValue,
                              keypimGroupMappingAddressTypeValue,
                              keypimGroupMappingGrpAddressValue,
                              keypimGroupMappingGrpPrefixLengthValue,
                              keypimGroupMappingRPAddressTypeValue,
                              keypimGroupMappingRPAddressValue, &objpimGroupMappingPrecedenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimGroupMappingPrecedence */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimGroupMappingPrecedenceValue,
                           sizeof (objpimGroupMappingPrecedenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
