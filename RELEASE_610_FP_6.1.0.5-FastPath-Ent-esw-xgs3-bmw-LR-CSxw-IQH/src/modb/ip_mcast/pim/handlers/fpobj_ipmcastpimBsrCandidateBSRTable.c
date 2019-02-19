
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimBsrCandidateBSRTable.c

* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  23 May 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimBsrCandidateBSRTable_obj.h"
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_1213_api.h"
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_pimsm_api.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_iputil_api.h"
#include "usmdb_util_api.h"

extern L7_RC_t usmDbValidIntIfNumNext(L7_uint32 interface, L7_uint32 *intIfNum);

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex
*
* @purpose Get 'pimBsrCandidateBSRZoneIndex'
 *@description  [pimBsrCandidateBSRZoneIndex] The zone index uniquely identifies
* the zone on a device to which this Candidate BSR is attached.
* There is one entry for each zone in ipMcastZoneTable. Scope-level
* information for this zone can be extracted from ipMcastZoneTable
* in IP Multicast MIB [RFC5132]. Zero is a special value used to
* request the default zone for a given scope. Zero is not a valid
* value for this object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex (void *wap, void *bufp)
{

#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRZoneIndexValue;
  xLibU32_t nextObjpimBsrCandidateBSRZoneIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  xLibU32_t objpimBsrCandidateBSRAddressTypeValue;

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddressTypeValue,
                          (xLibU8_t *) & objpimBsrCandidateBSRZoneIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0); 
    owa.l7rc = usmDbPimsmXUICandBSREntryGet (objpimBsrCandidateBSRZoneIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrCandidateBSRZoneIndexValue, owa.len);
    owa.l7rc = usmDbPimsmXUICandBSREntryNextGet (&nextObjpimBsrCandidateBSRZoneIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimBsrCandidateBSRZoneIndexValue, owa.len);

  /* return the object value: pimBsrCandidateBSRZoneIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimBsrCandidateBSRZoneIndexValue,
                           sizeof (objpimBsrCandidateBSRZoneIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

return XLIBRC_NOT_IMPLEMENTED;
}
/*******************************************************************************************/


static L7_RC_t
usmDbIpInterfaceNextGet(L7_uint32 UnitIndex, L7_uint32 *intIfNum)
{
  L7_int32 temp_val;
  /* loop through available external interface numbers */
  while (usmDbValidIntIfNumNext(*intIfNum, intIfNum) == L7_SUCCESS)
  {
    /* check to see if this is a valid routing interface */
    if ( (usmDbValidateRtrIntf(UnitIndex, *intIfNum) == L7_SUCCESS) &&
         (usmDbVisibleInterfaceCheck(UnitIndex, *intIfNum, &temp_val) == L7_SUCCESS) )
    return L7_SUCCESS;

  }

  return L7_FAILURE;
}

/*******************************************************************************
* @function fpObjGet_pimBsrCandidateBSRTablepimBsrCandidateBSRInterface
*
* @purpose Get 'IfIndex'
*
* @description [IfIndex]: The IfIndex associated with this instance.
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRInterface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBSRInterfaceValue;
  xLibU32_t nextObjBSRInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);
  L7_RC_t bsrConfigured = L7_FAILURE;
  xLibU32_t  bsrIntf = L7_NULL;
  xLibU32_t objpimBsrCandidateBSRAddressType;

    /* retrieve key: pimInterfaceIPVersion */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddressType,
                          (xLibU8_t *) & objpimBsrCandidateBSRAddressType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  bsrConfigured = usmDbPimsmXUICandBSRInterfaceGet(objpimBsrCandidateBSRAddressType, &bsrIntf);
  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRInterface,
                          (xLibU8_t *) & objBSRInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjBSRInterfaceValue = 0;
    if (bsrConfigured == L7_SUCCESS && bsrIntf != L7_NULL)
    {
      nextObjBSRInterfaceValue = bsrIntf;
    }
    else
    {
      owa.l7rc = usmDbIpInterfaceNextGet(L7_UNIT_CURRENT, &nextObjBSRInterfaceValue);
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objBSRInterfaceValue, owa.len);
    nextObjBSRInterfaceValue = objBSRInterfaceValue;
    if (objBSRInterfaceValue == bsrIntf)
    {
       owa.l7rc = L7_FAILURE;
    }
    else
    {
      owa.l7rc = usmDbIpInterfaceNextGet(L7_UNIT_CURRENT, &nextObjBSRInterfaceValue);
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjBSRInterfaceValue, owa.len);

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjBSRInterfaceValue,
                           sizeof (objBSRInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddressType
*
* @purpose Get 'pimBsrCandidateBSRAddressType'
 *@description  [pimBsrCandidateBSRAddressType] The address type of the
* Candidate-BSR.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddressType (void *wap,
                                                                                void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  fpObjWa_t kwapimBsrCandidateBSRInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len);

  /* retrieve key: pimBsrCandidateBSRInterface */
  kwapimBsrCandidateBSRInterface.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRInterface,
                   (xLibU8_t *) & keypimBsrCandidateBSRInterfaceValue,
                   &kwapimBsrCandidateBSRInterface.len);
  if (kwapimBsrCandidateBSRInterface.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRInterface);
    return kwapimBsrCandidateBSRInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRInterfaceValue,
                           kwapimBsrCandidateBSRInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateBSRZoneIndexValue,
                              &objpimBsrCandidateBSRAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateBSRAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRAddressTypeValue,
                           sizeof (objpimBsrCandidateBSRAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddressType
*
* @purpose Set 'pimBsrCandidateBSRAddressType'
 *@description  [pimBsrCandidateBSRAddressType] The address type of the
* Candidate-BSR.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddressType (void *wap,
                                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRAddressTypeValue;

  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimBsrCandidateBSRAddressType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimBsrCandidateBSRAddressTypeValue, owa.len);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateBSRZoneIndexValue,
                              objpimBsrCandidateBSRAddressTypeValue);

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
* @function fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddress
*
* @purpose Get 'pimBsrCandidateBSRAddress'
 *@description  [pimBsrCandidateBSRAddress] The (unicast) address that the local
* router will use to advertise itself as a Candidate-BSR. The
* InetAddressType is given by the pimBsrCandidateBSRAddressType object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddress (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimBsrCandidateBSRAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmXUIElectedBSRAddressGet (keypimBsrCandidateBSRZoneIndexValue,
                              objpimBsrCandidateBSRAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateBSRAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpimBsrCandidateBSRAddressValue,
                           strlen (objpimBsrCandidateBSRAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddress
*
* @purpose Set 'pimBsrCandidateBSRAddress'
 *@description  [pimBsrCandidateBSRAddress] The (unicast) address that the local
* router will use to advertise itself as a Candidate-BSR. The
* InetAddressType is given by the pimBsrCandidateBSRAddressType object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddress (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimBsrCandidateBSRAddressValue;

  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimBsrCandidateBSRAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objpimBsrCandidateBSRAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objpimBsrCandidateBSRAddressValue, owa.len);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbPimsmbsrXUIElectedBSRPriorityGet (keypimBsrCandidateBSRZoneIndexValue,
                              objpimBsrCandidateBSRAddressValue);

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
* @function fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRPriority
*
* @purpose Get 'pimBsrCandidateBSRPriority'
 *@description  [pimBsrCandidateBSRPriority] The priority value for the local
* router as a Candidate-BSR for this zone. Numerically higher values
* for this object indicate higher priorities.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRPriority (void *wap, void *bufp)
{

  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRPriorityValue;

  fpObjWa_t kwapimBsrCandidateBSRInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRInterfaceValue;

  xLibU32_t configuredpimBsrCandidateBSRInterfaceValue;
  xLibU32_t objpimBsrCandidateBSRAddressType;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len); 

    /* retrieve address type: pimBsrCandidateBSRAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddressType,
                          (xLibU8_t *) & objpimBsrCandidateBSRAddressType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: pimBsrCandidateBSRInterface */
  kwapimBsrCandidateBSRInterface.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRInterface,
                   (xLibU8_t *) & keypimBsrCandidateBSRInterfaceValue,
                   &kwapimBsrCandidateBSRInterface.len);
  if (kwapimBsrCandidateBSRInterface.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRInterface);
    return kwapimBsrCandidateBSRInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRInterfaceValue,
                           kwapimBsrCandidateBSRInterface.len);

  /*get the configured Interface value from the application*/
  owa.l7rc = usmDbPimsmXUICandBSRInterfaceGet(objpimBsrCandidateBSRAddressType, &configuredpimBsrCandidateBSRInterfaceValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if(keypimBsrCandidateBSRInterfaceValue == configuredpimBsrCandidateBSRInterfaceValue ) 
  {
    
    /* get the value from application */
    owa.l7rc = usmDbPimsmXUICandBSRPriorityGet (objpimBsrCandidateBSRAddressType,
                              &objpimBsrCandidateBSRPriorityValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  
  /* return the object value: pimBsrCandidateBSRPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRPriorityValue,
                           sizeof (objpimBsrCandidateBSRPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
  }
  /* return the object value: pimBsrCandidateBSRPriority */
  objpimBsrCandidateBSRPriorityValue = L7_PIMSM_CBSR_PRIORITY;
  
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRPriorityValue,
                           sizeof (objpimBsrCandidateBSRPriorityValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRPriority
*
* @purpose Set 'pimBsrCandidateBSRPriority'
 *@description  [pimBsrCandidateBSRPriority] The priority value for the local
* router as a Candidate-BSR for this zone. Numerically higher values
* for this object indicate higher priorities.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRPriority (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRPriorityValue;

  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  fpObjWa_t kwapimBsrCandidateBSRInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRInterfaceValue;

  fpObjWa_t kwapimBsrCandidateBSRHashMaskLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRHashMaskLengthValue;

  xLibU32_t objpimBsrCandidateBSRAddressType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimBsrCandidateBSRPriority */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimBsrCandidateBSRPriorityValue, owa.len);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len); 

/*  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &keypimBsrCandidateBSRZoneIndexValue, owa.len); */

    /* retrieve address type: pimBsrCandidateBSRAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddressType,
                          (xLibU8_t *) & objpimBsrCandidateBSRAddressType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: pimBsrCandidateBSRInterface */
  kwapimBsrCandidateBSRInterface.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRInterface,
                   (xLibU8_t *) & keypimBsrCandidateBSRInterfaceValue,
                   &kwapimBsrCandidateBSRInterface.len);
  if (kwapimBsrCandidateBSRInterface.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRInterface);
    return kwapimBsrCandidateBSRInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRInterfaceValue,
                           kwapimBsrCandidateBSRInterface.len);

   /* retrieve object: pimBsrCandidateBSRHashMaskLength */
   kwapimBsrCandidateBSRHashMaskLength.rc = 
     xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRHashMaskLength,
                    (xLibU8_t *) & objpimBsrCandidateBSRHashMaskLengthValue, 
                    &kwapimBsrCandidateBSRHashMaskLength.len);
  if (kwapimBsrCandidateBSRHashMaskLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRHashMaskLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRHashMaskLength);
    return kwapimBsrCandidateBSRHashMaskLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrCandidateBSRHashMaskLengthValue,
                           kwapimBsrCandidateBSRHashMaskLength.len);



  if (objpimBsrCandidateBSRAddressType == L7_AF_INET)
  {
    if (objpimBsrCandidateBSRHashMaskLengthValue < L7_PIMSM_INTERFACE_CBSR_HASH_MASK_LENGTH_MIN ||
        objpimBsrCandidateBSRHashMaskLengthValue > L7_PIMSM_INTERFACE_CBSR_HASH_MASK_LENGTH_MAX)
    {
      /* The appropriate error is sent by the hash mask length object handler.*/
      owa.rc = XLIBRC_SUCCESS;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else if (objpimBsrCandidateBSRAddressType == L7_AF_INET6)
  {
    if (objpimBsrCandidateBSRHashMaskLengthValue < L7_PIMSM6_INTERFACE_CBSR_HASH_MASK_LENGTH_MIN ||
        objpimBsrCandidateBSRHashMaskLengthValue > L7_PIMSM6_INTERFACE_CBSR_HASH_MASK_LENGTH_MAX)
    {
      /* The appropriate error is sent by the hash mask length object handler.*/
      owa.rc = XLIBRC_SUCCESS;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }


  /* set the value in application */
  owa.l7rc = usmDbPimsmXUICandBSRSet (objpimBsrCandidateBSRAddressType, keypimBsrCandidateBSRInterfaceValue, 
                              objpimBsrCandidateBSRPriorityValue, L7_ENABLE, objpimBsrCandidateBSRHashMaskLengthValue, 0);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRHashMaskLength
*
* @purpose Get 'pimBsrCandidateBSRHashMaskLength'
 *@description  [pimBsrCandidateBSRHashMaskLength] The hash mask length (used in
* the RP hash function) that the local router will advertise in
* its Bootstrap messages for this zone. This object defaults to 30 if
* pimBsrCandidateBSRAddressType is 'ipv4' or 'ipv4z' , and
* defaults to 126 if pimBsrCandidateBSRAddressType is 'ipv6' or 'ipv6z'.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRHashMaskLength (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRHashMaskLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  fpObjWa_t kwapimBsrCandidateBSRInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRInterfaceValue;

   xLibU32_t configuredpimBsrCandidateBSRInterfaceValue;
   xLibU32_t objpimBsrCandidateBSRAddressType;

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len); 

    /* retrieve address type: pimBsrCandidateBSRAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddressType,
                          (xLibU8_t *) & objpimBsrCandidateBSRAddressType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: pimBsrCandidateBSRInterface */
  kwapimBsrCandidateBSRInterface.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRInterface,
                   (xLibU8_t *) & keypimBsrCandidateBSRInterfaceValue,
                   &kwapimBsrCandidateBSRInterface.len);
  if (kwapimBsrCandidateBSRInterface.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRInterface);
    return kwapimBsrCandidateBSRInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRInterfaceValue,
                           kwapimBsrCandidateBSRInterface.len);

  /*get the configured Interface value from the application*/
  owa.l7rc = usmDbPimsmXUICandBSRInterfaceGet(objpimBsrCandidateBSRAddressType, &configuredpimBsrCandidateBSRInterfaceValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(keypimBsrCandidateBSRInterfaceValue == configuredpimBsrCandidateBSRInterfaceValue )
  {
    /* get the value from application */
    owa.l7rc = usmDbPimsmXUICandBSRHashMaskLengthGet (objpimBsrCandidateBSRAddressType,
                              &objpimBsrCandidateBSRHashMaskLengthValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  
  /* return the object value: pimBsrCandidateBSRHashMaskLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRHashMaskLengthValue,
                           sizeof (objpimBsrCandidateBSRHashMaskLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

  }

  /* return the object value: pimBsrCandidateBSRHashMaskLength */
  if (objpimBsrCandidateBSRAddressType == L7_AF_INET)
  {
    objpimBsrCandidateBSRHashMaskLengthValue = L7_PIMSM_CBSR_HASH_MASK_LENGTH;
  }
  else if (objpimBsrCandidateBSRAddressType == L7_AF_INET6)
  {
    objpimBsrCandidateBSRHashMaskLengthValue = L7_PIMSM6_CBSR_HASH_MASK_LENGTH;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRHashMaskLengthValue,
                           sizeof (objpimBsrCandidateBSRHashMaskLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRHashMaskLength
*
* @purpose Set 'pimBsrCandidateBSRHashMaskLength'
 *@description  [pimBsrCandidateBSRHashMaskLength] The hash mask length (used in
* the RP hash function) that the local router will advertise in
* its Bootstrap messages for this zone. This object defaults to 30 if
* pimBsrCandidateBSRAddressType is 'ipv4' or 'ipv4z' , and
* defaults to 126 if pimBsrCandidateBSRAddressType is 'ipv6' or 'ipv6z'.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRHashMaskLength (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRHashMaskLengthValue;

  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  fpObjWa_t kwapimBsrCandidateBSRInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRInterfaceValue;

  fpObjWa_t kwapimBsrCandidateBSRPriority = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRPriorityValue;
  xLibU32_t objpimBsrCandidateBSRAddressType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimBsrCandidateBSRHashMaskLength */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRHashMaskLengthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimBsrCandidateBSRHashMaskLengthValue, owa.len);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len);

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len);

    /* retrieve address type: pimBsrCandidateBSRAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddressType,
                          (xLibU8_t *) & objpimBsrCandidateBSRAddressType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }



  if (objpimBsrCandidateBSRAddressType == L7_AF_INET)
  {
    if (objpimBsrCandidateBSRHashMaskLengthValue < L7_PIMSM_INTERFACE_CBSR_HASH_MASK_LENGTH_MIN ||
        objpimBsrCandidateBSRHashMaskLengthValue > L7_PIMSM_INTERFACE_CBSR_HASH_MASK_LENGTH_MAX)
    {
      owa.rc = XLIBRC_PIM4_BSR_HASHMASKLEN_OUT_OF_RANGE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else if (objpimBsrCandidateBSRAddressType == L7_AF_INET6)
  {
    if (objpimBsrCandidateBSRHashMaskLengthValue < L7_PIMSM6_INTERFACE_CBSR_HASH_MASK_LENGTH_MIN ||
        objpimBsrCandidateBSRHashMaskLengthValue > L7_PIMSM6_INTERFACE_CBSR_HASH_MASK_LENGTH_MAX)
    {
      owa.rc = XLIBRC_PIM6_BSR_HASHMASKLEN_OUT_OF_RANGE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  /* retrieve key: pimBsrCandidateBSRInterface */
  kwapimBsrCandidateBSRInterface.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRInterface,
                   (xLibU8_t *) & keypimBsrCandidateBSRInterfaceValue,
                   &kwapimBsrCandidateBSRInterface.len);
  if (kwapimBsrCandidateBSRInterface.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRInterface);
    return kwapimBsrCandidateBSRInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRInterfaceValue,
                           kwapimBsrCandidateBSRInterface.len);

  /* retrieve object: pimBsrCandidateBSRPriority */
  kwapimBsrCandidateBSRPriority.rc =
     xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRPriority,
                    (xLibU8_t *) & objpimBsrCandidateBSRPriorityValue,
                    &kwapimBsrCandidateBSRPriority.len);
  if (kwapimBsrCandidateBSRPriority.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRPriority.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRPriority);
    return kwapimBsrCandidateBSRPriority.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrCandidateBSRPriorityValue,
                           kwapimBsrCandidateBSRPriority.len);

  /* set the value in application */
  owa.l7rc = usmDbPimsmXUICandBSRSet (objpimBsrCandidateBSRAddressType, keypimBsrCandidateBSRInterfaceValue,
                              objpimBsrCandidateBSRPriorityValue, L7_ENABLE, objpimBsrCandidateBSRHashMaskLengthValue, 0);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRElectedBSR
*
* @purpose Get 'pimBsrCandidateBSRElectedBSR'
 *@description  [pimBsrCandidateBSRElectedBSR] Whether the local router is the
* elected BSR for this zone.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRElectedBSR (void *wap,
                                                                               void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRElectedBSRValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateBSRZoneIndexValue,
                              &objpimBsrCandidateBSRElectedBSRValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateBSRElectedBSR */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRElectedBSRValue,
                           sizeof (objpimBsrCandidateBSRElectedBSRValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRBootstrapTimer
*
* @purpose Get 'pimBsrCandidateBSRBootstrapTimer'
 *@description  [pimBsrCandidateBSRBootstrapTimer] The time remaining before the
* local router next originates a Bootstrap message for this zone.
* Value of this object is zero if pimBsrCandidateBSRElectedBSR is
* 'FALSE'.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRBootstrapTimer (void *wap,
                                                                                   void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRBootstrapTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmXUICandBSRBootstrapTimerGet (keypimBsrCandidateBSRZoneIndexValue,
                              &objpimBsrCandidateBSRBootstrapTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateBSRBootstrapTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRBootstrapTimerValue,
                           sizeof (objpimBsrCandidateBSRBootstrapTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRStatus
*
* @purpose Get 'pimBsrCandidateBSRStatus'
 *@description  [pimBsrCandidateBSRStatus] The status of this row, by which new
* entries may be created, or old entries deleted from this table.
* This status object can be set to active(1) without setting any
* other columnar objects in this entry. All writable objects in this
* entry can be modified when the status of this entry is active(1).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRStatus (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateBSRZoneIndexValue,
                              &objpimBsrCandidateBSRStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateBSRStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRStatusValue,
                           sizeof (objpimBsrCandidateBSRStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRStatus
*
* @purpose Set 'pimBsrCandidateBSRStatus'
 *@description  [pimBsrCandidateBSRStatus] The status of this row, by which new
* entries may be created, or old entries deleted from this table.
* This status object can be set to active(1) without setting any
* other columnar objects in this entry. All writable objects in this
* entry can be modified when the status of this entry is active(1).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRStatusValue;

  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  fpObjWa_t kwapimBsrCandidateBSRInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRAddressType;
  xLibU32_t objpimBsrCandidateBSRInterfaceValue;
  xLibU32_t objpimBsrCandidateBSRPriority;
  xLibU32_t objpimBsrCandidateBSRHashMaskLength;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimBsrCandidateBSRStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimBsrCandidateBSRStatusValue, owa.len);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len);

    /* retrieve address type: pimBsrCandidateBSRAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRAddressType,
                          (xLibU8_t *) & objpimBsrCandidateBSRAddressType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: pimBsrCandidateBSRInterface */
  kwapimBsrCandidateBSRInterface.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRInterface,
                   (xLibU8_t *) & objpimBsrCandidateBSRInterfaceValue,
                   &kwapimBsrCandidateBSRInterface.len);
  if (kwapimBsrCandidateBSRInterface.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRInterface);
    return kwapimBsrCandidateBSRInterface.rc;
  }

  /* retrieve address type: pimBsrCandidateBSRPriority */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRPriority,
                          (xLibU8_t *) & objpimBsrCandidateBSRPriority, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve address type: pimBsrCandidateBSRHashMaskLength */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRHashMaskLength,
                          (xLibU8_t *) & objpimBsrCandidateBSRHashMaskLength, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  if (objpimBsrCandidateBSRAddressType == L7_AF_INET)
  {
    if (objpimBsrCandidateBSRHashMaskLength < L7_PIMSM_INTERFACE_CBSR_HASH_MASK_LENGTH_MIN ||
        objpimBsrCandidateBSRHashMaskLength > L7_PIMSM_INTERFACE_CBSR_HASH_MASK_LENGTH_MAX)
    {
      /* The appropriate error is sent by the hash mask length object handler.*/
      owa.rc = XLIBRC_SUCCESS;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else if (objpimBsrCandidateBSRAddressType == L7_AF_INET6)
  {
    if (objpimBsrCandidateBSRHashMaskLength < L7_PIMSM6_INTERFACE_CBSR_HASH_MASK_LENGTH_MIN ||
        objpimBsrCandidateBSRHashMaskLength > L7_PIMSM6_INTERFACE_CBSR_HASH_MASK_LENGTH_MAX)
    {
      /* The appropriate error is sent by the hash mask length object handler.*/
      owa.rc = XLIBRC_SUCCESS;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  /* set the value in application */
  if (objpimBsrCandidateBSRStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbPimsmCandBSRSet (L7_UNIT_CURRENT, objpimBsrCandidateBSRAddressType,
                                objpimBsrCandidateBSRInterfaceValue, objpimBsrCandidateBSRPriority,
                                     L7_DISABLE, objpimBsrCandidateBSRHashMaskLength,
                                    keypimBsrCandidateBSRZoneIndexValue );
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;


/*  return XLIBRC_NOT_IMPLEMENTED; */
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRStorageType
*
* @purpose Get 'pimBsrCandidateBSRStorageType'
 *@description  [pimBsrCandidateBSRStorageType] The storage type for this row.
* Rows having the value 'permanent' need not allow write-access to
* any columnar objects in the row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRStorageType (void *wap,
                                                                                void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateBSRZoneIndexValue,
                              &objpimBsrCandidateBSRStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateBSRStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRStorageTypeValue,
                           sizeof (objpimBsrCandidateBSRStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRStorageType
*
* @purpose Set 'pimBsrCandidateBSRStorageType'
 *@description  [pimBsrCandidateBSRStorageType] The storage type for this row.
* Rows having the value 'permanent' need not allow write-access to
* any columnar objects in the row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRStorageType (void *wap,
                                                                                void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateBSRStorageTypeValue;

  fpObjWa_t kwapimBsrCandidateBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateBSRZoneIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimBsrCandidateBSRStorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimBsrCandidateBSRStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimBsrCandidateBSRStorageTypeValue, owa.len);

  /* retrieve key: pimBsrCandidateBSRZoneIndex */
  kwapimBsrCandidateBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateBSRTable_pimBsrCandidateBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrCandidateBSRZoneIndexValue,
                   &kwapimBsrCandidateBSRZoneIndex.len);
  if (kwapimBsrCandidateBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateBSRZoneIndex);
    return kwapimBsrCandidateBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateBSRZoneIndexValue,
                           kwapimBsrCandidateBSRZoneIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateBSRZoneIndexValue,
                              objpimBsrCandidateBSRStorageTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
