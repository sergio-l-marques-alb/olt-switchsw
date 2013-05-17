
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_IPMCASTdvmrpPruneTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to IPMCAST-object.xml
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
#include "_xe_IPMCASTdvmrpPruneTable_obj.h"
#include "usmdb_mib_dvmrp_api.h"


/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpPruneTable_dvmrpPruneGroup
*
* @purpose Get 'dvmrpPruneGroup'
*
* @description [dvmrpPruneGroup] The group address which has been pruned.
*
* @notes  
*
* @return
*******************************************************************************/
#define IPV4_ADDRESS(g,s,m) g.addr.ipv4.s_addr, s.addr.ipv4.s_addr, m.addr.ipv4.s_addr
xLibRC_t fpObjGet_IPMCASTdvmrpPruneTable_dvmrpPruneGroup (void *wap, void *bufp)
{

  L7_inet_addr_t objdvmrpPruneGroupValue;
  L7_inet_addr_t nextObjdvmrpPruneGroupValue;
  L7_inet_addr_t nextObjdvmrpPruneSourceValue;
  L7_inet_addr_t nextObjdvmrpPruneSourceMaskValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpPruneGroup */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpPruneTable_dvmrpPruneGroup,
                          (xLibU8_t *) & objdvmrpPruneGroupValue, &owa.len);

  inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpPruneGroupValue);
  inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpPruneSourceValue);
  inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpPruneSourceMaskValue);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    owa.l7rc = usmDbDvmrpPruneEntryNextGet (L7_UNIT_CURRENT,
                                     &nextObjdvmrpPruneGroupValue,
                                     &nextObjdvmrpPruneSourceValue,
                                     &nextObjdvmrpPruneSourceMaskValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpPruneGroupValue, owa.len);
    do
    {
      owa.l7rc = usmDbDvmrpPruneEntryNextGet (L7_UNIT_CURRENT,
                                      &nextObjdvmrpPruneGroupValue,
                                      &nextObjdvmrpPruneSourceValue,
                                      &nextObjdvmrpPruneSourceMaskValue);
    } while (memcmp(&objdvmrpPruneGroupValue, &nextObjdvmrpPruneGroupValue, sizeof(L7_inet_addr_t)) && (owa.l7rc == L7_SUCCESS));
    if (owa.l7rc == L7_SUCCESS)
    {
      do 
      {
        owa.l7rc = usmDbDvmrpPruneEntryNextGet (L7_UNIT_CURRENT,
                                        &nextObjdvmrpPruneGroupValue,
                                        &nextObjdvmrpPruneSourceValue,
                                        &nextObjdvmrpPruneSourceMaskValue);
      } while(!memcmp(&objdvmrpPruneGroupValue, &nextObjdvmrpPruneGroupValue, sizeof(L7_inet_addr_t)) && (owa.l7rc == L7_SUCCESS));
    }
    else
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdvmrpPruneGroupValue, owa.len);

  /* return the object value: dvmrpPruneGroup */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdvmrpPruneGroupValue,
                           sizeof (objdvmrpPruneGroupValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpPruneTable_dvmrpPruneSource
*
* @purpose Get 'dvmrpPruneSource'
*
* @description [dvmrpPruneSource] The address of the source or source network which has been pruned.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpPruneTable_dvmrpPruneSource (void *wap, void *bufp)
{

  L7_inet_addr_t objdvmrpPruneGroupValue;
  L7_inet_addr_t nextObjdvmrpPruneGroupValue;
  L7_inet_addr_t objdvmrpPruneSourceValue;
  L7_inet_addr_t nextObjdvmrpPruneSourceValue;
  L7_inet_addr_t nextObjdvmrpPruneSourceMaskValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpPruneGroup */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpPruneTable_dvmrpPruneGroup,
                          (xLibU8_t *) & objdvmrpPruneGroupValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpPruneGroupValue, owa.len);

  /* retrieve key: dvmrpPruneSource */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpPruneTable_dvmrpPruneSource,
                          (xLibU8_t *) & objdvmrpPruneSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memcpy(&nextObjdvmrpPruneGroupValue, &objdvmrpPruneGroupValue, sizeof(L7_inet_addr_t));
    inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpPruneSourceValue);
    inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpPruneSourceMaskValue);
   
    do
    { 
      owa.l7rc = usmDbDvmrpPruneEntryNextGet (L7_UNIT_CURRENT,
                                       &nextObjdvmrpPruneGroupValue,
                                       &nextObjdvmrpPruneSourceValue,
                                       &nextObjdvmrpPruneSourceMaskValue);
    } while((memcmp(&nextObjdvmrpPruneGroupValue, &objdvmrpPruneGroupValue, sizeof(L7_inet_addr_t))) && (owa.l7rc == L7_SUCCESS));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpPruneSourceValue, owa.len);
    inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpPruneSourceMaskValue);
    inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpPruneGroupValue);
    inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpPruneSourceValue);
    do
    {
    
      owa.l7rc = usmDbDvmrpPruneEntryNextGet (L7_UNIT_CURRENT,
                                      &nextObjdvmrpPruneGroupValue,
                                      &nextObjdvmrpPruneSourceValue,
                                      &nextObjdvmrpPruneSourceMaskValue);
    }
    while ((memcmp(&objdvmrpPruneGroupValue, &nextObjdvmrpPruneGroupValue, sizeof(L7_inet_addr_t))
           || (memcmp(&objdvmrpPruneSourceValue, &nextObjdvmrpPruneSourceValue, sizeof(L7_inet_addr_t))))
           && (owa.l7rc == L7_SUCCESS));
    if (owa.l7rc == L7_SUCCESS)
    {
      owa.l7rc = usmDbDvmrpPruneEntryNextGet (L7_UNIT_CURRENT,
                                      &nextObjdvmrpPruneGroupValue,
                                      &nextObjdvmrpPruneSourceValue,
                                      &nextObjdvmrpPruneSourceMaskValue);
    }
    else
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  if (memcmp(&objdvmrpPruneGroupValue, &nextObjdvmrpPruneGroupValue, sizeof(L7_inet_addr_t)) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdvmrpPruneSourceValue, owa.len);

  /* return the object value: dvmrpPruneSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdvmrpPruneSourceValue,
                           sizeof (objdvmrpPruneSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpPruneTable_dvmrpPruneSourceMask
*
* @purpose Get 'dvmrpPruneSourceMask'
*
* @description [dvmrpPruneSourceMask] The address of the source or source network which has been pruned. The mask must either be all 1's, or else dvmrpPruneSource and dvmrpPruneSourceMask must match dvmrpRouteSource and dvmrpRouteSourceMask for some entry in 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpPruneTable_dvmrpPruneSourceMask (void *wap, void *bufp)
{

  L7_inet_addr_t objdvmrpPruneGroupValue;
  L7_inet_addr_t nextObjdvmrpPruneGroupValue;
  L7_inet_addr_t objdvmrpPruneSourceValue;
  L7_inet_addr_t nextObjdvmrpPruneSourceValue;
  L7_inet_addr_t objdvmrpPruneSourceMaskValue;
  L7_inet_addr_t nextObjdvmrpPruneSourceMaskValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpPruneGroup */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpPruneTable_dvmrpPruneGroup,
                          (xLibU8_t *) & objdvmrpPruneGroupValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpPruneGroupValue, owa.len);

  /* retrieve key: dvmrpPruneSource */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpPruneTable_dvmrpPruneSource,
                          (xLibU8_t *) & objdvmrpPruneSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpPruneSourceValue, owa.len);

  /* retrieve key: dvmrpPruneSourceMask */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpPruneTable_dvmrpPruneSourceMask,
                          (xLibU8_t *) & objdvmrpPruneSourceMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpPruneSourceMaskValue);
    inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpPruneGroupValue);
    inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpPruneSourceValue);
   do 
   {
 
     owa.l7rc = usmDbDvmrpPruneEntryNextGet (L7_UNIT_CURRENT,
                                       &nextObjdvmrpPruneGroupValue,
                                       &nextObjdvmrpPruneSourceValue,
                                       &nextObjdvmrpPruneSourceMaskValue);
    }while((memcmp(&objdvmrpPruneGroupValue, &nextObjdvmrpPruneGroupValue, sizeof(L7_inet_addr_t)) ||
            memcmp(&objdvmrpPruneSourceValue, &nextObjdvmrpPruneSourceValue, sizeof(L7_inet_addr_t))) && 
           (owa.l7rc == L7_SUCCESS));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpPruneSourceMaskValue, owa.len);

    memcpy(&nextObjdvmrpPruneGroupValue, &objdvmrpPruneGroupValue, sizeof(L7_inet_addr_t));
    memcpy(&nextObjdvmrpPruneSourceValue, &objdvmrpPruneSourceValue, sizeof(L7_inet_addr_t));
    memcpy(&nextObjdvmrpPruneSourceMaskValue, &objdvmrpPruneSourceMaskValue, sizeof(L7_inet_addr_t));
      
    owa.l7rc = usmDbDvmrpPruneEntryNextGet (L7_UNIT_CURRENT,
                                    &nextObjdvmrpPruneGroupValue,
                                    &nextObjdvmrpPruneSourceValue,
                                    &nextObjdvmrpPruneSourceMaskValue);

  }

  if (memcmp(&objdvmrpPruneGroupValue, &nextObjdvmrpPruneGroupValue, sizeof(L7_inet_addr_t))
      || (memcmp(&objdvmrpPruneSourceValue, &nextObjdvmrpPruneSourceValue, sizeof(L7_inet_addr_t))) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdvmrpPruneSourceMaskValue, owa.len);

  /* return the object value: dvmrpPruneSourceMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdvmrpPruneSourceMaskValue,
                           sizeof (objdvmrpPruneSourceMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpPruneTable_dvmrpPruneExpiryTime
*
* @purpose Get 'dvmrpPruneExpiryTime'
*
* @description [dvmrpPruneExpiryTime] The amount of time remaining before this prune should expire at the upstream neighbor. This value should be the minimum of the default prune lifetime and the remaining prune lifetimes of the local router's downstream neig
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpPruneTable_dvmrpPruneExpiryTime (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpPruneGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpPruneGroupValue;
  fpObjWa_t kwadvmrpPruneSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpPruneSourceValue;
  fpObjWa_t kwadvmrpPruneSourceMask = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpPruneSourceMaskValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibStr256_t objdvmrpPruneExpiryTimeValue;
  xLibUL32_t PruneExpiryTimeValue;
  L7_timespec timeSpec;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpPruneGroup */
  kwadvmrpPruneGroup.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpPruneTable_dvmrpPruneGroup,
                                         (xLibU8_t *) & keydvmrpPruneGroupValue,
                                         &kwadvmrpPruneGroup.len);
  if (kwadvmrpPruneGroup.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpPruneGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpPruneGroup);
    return kwadvmrpPruneGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpPruneGroupValue, kwadvmrpPruneGroup.len);

  /* retrieve key: dvmrpPruneSource */
  kwadvmrpPruneSource.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpPruneTable_dvmrpPruneSource,
                                          (xLibU8_t *) & keydvmrpPruneSourceValue,
                                          &kwadvmrpPruneSource.len);
  if (kwadvmrpPruneSource.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpPruneSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpPruneSource);
    return kwadvmrpPruneSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpPruneSourceValue, kwadvmrpPruneSource.len);

  /* retrieve key: dvmrpPruneSourceMask */
  kwadvmrpPruneSourceMask.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpPruneTable_dvmrpPruneSourceMask,
                                              (xLibU8_t *) & keydvmrpPruneSourceMaskValue,
                                              &kwadvmrpPruneSourceMask.len);
  if (kwadvmrpPruneSourceMask.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpPruneSourceMask.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpPruneSourceMask);
    return kwadvmrpPruneSourceMask.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpPruneSourceMaskValue, kwadvmrpPruneSourceMask.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpPruneExpiryTimeGet (L7_UNIT_CURRENT, &keydvmrpPruneGroupValue,
                              &keydvmrpPruneSourceValue,
                              &keydvmrpPruneSourceMaskValue, &PruneExpiryTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiConvertRawUpTime(PruneExpiryTimeValue,(L7_timespec *)&timeSpec);
  osapiSnprintf(objdvmrpPruneExpiryTimeValue, sizeof(objdvmrpPruneExpiryTimeValue), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                               timeSpec.minutes,timeSpec.seconds);
  /* return the object value: dvmrpPruneExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpPruneExpiryTimeValue,
                           sizeof (objdvmrpPruneExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
