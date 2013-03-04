
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_IPMCASTdvmrpRouteNextHopTable.c
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
#include "_xe_IPMCASTdvmrpRouteNextHopTable_obj.h"
#include "usmdb_mib_dvmrp_api.h"

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopIfIndex
*
* @purpose Get 'dvmrpRouteNextHopIfIndex'
*
* @description [dvmrpRouteNextHopIfIndex] The ifIndex value of the interface for the outgoing interface for this next hop.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopIfIndex (void *wap, void *bufp)
{

  xLibU32_t objdvmrpRouteNextHopIfIndexValue;
  xLibU32_t nextObjdvmrpRouteNextHopIfIndexValue;
  
  L7_inet_addr_t nextObjdvmrpRouteNextHopSourceValue;
  L7_inet_addr_t objdvmrpRouteNextHopSourceValue;

  L7_inet_addr_t nextObjdvmrpRouteNextHopSourceMaskValue;
  L7_inet_addr_t objdvmrpRouteNextHopSourceMaskValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpRouteNextHopSource */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopSource,
                          (xLibU8_t *) & objdvmrpRouteNextHopSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpRouteNextHopSourceValue, owa.len);

  /* retrieve key: dvmrpRouteNextHopSourceMask */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopSourceMask,
                          (xLibU8_t *) & objdvmrpRouteNextHopSourceMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpRouteNextHopSourceMaskValue, owa.len);

  /* retrieve key: dvmrpRouteNextHopIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopIfIndex,
                          (xLibU8_t *) & objdvmrpRouteNextHopIfIndexValue, &owa.len);
  inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpRouteNextHopSourceValue);
  inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpRouteNextHopSourceMaskValue);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjdvmrpRouteNextHopIfIndexValue = 0;
    do
    {
      owa.l7rc = usmDbDvmrpRouteNextHopEntryNextGet (L7_UNIT_CURRENT, 
                              &nextObjdvmrpRouteNextHopSourceValue,
                              &nextObjdvmrpRouteNextHopSourceMaskValue,
                              &nextObjdvmrpRouteNextHopIfIndexValue  );
    }while(((memcmp(&nextObjdvmrpRouteNextHopSourceValue, &objdvmrpRouteNextHopSourceValue, sizeof(L7_inet_addr_t))) ||
            (memcmp(&nextObjdvmrpRouteNextHopSourceMaskValue, &objdvmrpRouteNextHopSourceMaskValue, sizeof(L7_inet_addr_t)))             ) && (owa.l7rc == L7_SUCCESS));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpRouteNextHopIfIndexValue, owa.len);
    memcpy(&nextObjdvmrpRouteNextHopSourceValue, &objdvmrpRouteNextHopSourceValue, sizeof (L7_inet_addr_t));
    memcpy(&nextObjdvmrpRouteNextHopSourceMaskValue, &objdvmrpRouteNextHopSourceMaskValue, sizeof (L7_inet_addr_t));
    nextObjdvmrpRouteNextHopIfIndexValue = objdvmrpRouteNextHopIfIndexValue;
    
    owa.l7rc = usmDbDvmrpRouteNextHopEntryNextGet (L7_UNIT_CURRENT,
                            &nextObjdvmrpRouteNextHopSourceValue,
                            &nextObjdvmrpRouteNextHopSourceMaskValue,
                            &nextObjdvmrpRouteNextHopIfIndexValue  );
  }

  if ((memcmp(&nextObjdvmrpRouteNextHopSourceValue, &objdvmrpRouteNextHopSourceValue, sizeof(L7_inet_addr_t)))
      || (memcmp(&nextObjdvmrpRouteNextHopSourceMaskValue, &nextObjdvmrpRouteNextHopSourceMaskValue, sizeof(L7_inet_addr_t)))
       ||(owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdvmrpRouteNextHopIfIndexValue, owa.len);

  /* return the object value: dvmrpRouteNextHopIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdvmrpRouteNextHopIfIndexValue,
                           sizeof (objdvmrpRouteNextHopIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopSource
*
* @purpose Get 'dvmrpRouteNextHopSource'
*
* @description [dvmrpRouteNextHopSource] The network address which when combined with the corresponding value of dvmrpRouteNextHopSourceMask identifies the sources for which this entry specifies a next hop on an outgoing interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopSource (void *wap, void *bufp)
{

  xLibU32_t nextObjdvmrpRouteNextHopIfIndexValue;
  
  L7_inet_addr_t objdvmrpRouteNextHopSourceValue;
  L7_inet_addr_t nextObjdvmrpRouteNextHopSourceValue;

  L7_inet_addr_t nextObjdvmrpRouteNextHopSourceMaskValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpRouteNextHopSourceValue);
  inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpRouteNextHopSourceMaskValue);

  /* retrieve key: dvmrpRouteNextHopSource */
  owa.len = sizeof (L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopSource,
                          (xLibU8_t *) & objdvmrpRouteNextHopSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjdvmrpRouteNextHopIfIndexValue = 0;

    owa.l7rc = usmDbDvmrpRouteNextHopEntryNextGet (L7_UNIT_CURRENT,
                            &nextObjdvmrpRouteNextHopSourceValue,
                            &nextObjdvmrpRouteNextHopSourceMaskValue,
                            &nextObjdvmrpRouteNextHopIfIndexValue  );
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpRouteNextHopSourceValue, owa.len);

    do
    {  
       owa.l7rc = usmDbDvmrpRouteNextHopEntryNextGet (L7_UNIT_CURRENT,
                            &nextObjdvmrpRouteNextHopSourceValue,
                            &nextObjdvmrpRouteNextHopSourceMaskValue,
                            &nextObjdvmrpRouteNextHopIfIndexValue  );

    }
    while (((memcmp(&objdvmrpRouteNextHopSourceValue, &nextObjdvmrpRouteNextHopSourceValue,sizeof(L7_inet_addr_t))))
           && (owa.l7rc == L7_SUCCESS));
    if (owa.l7rc == L7_SUCCESS)
    {
      do 
      {
        owa.l7rc = usmDbDvmrpRouteNextHopEntryNextGet (L7_UNIT_CURRENT,
                             &nextObjdvmrpRouteNextHopSourceValue,
                             &nextObjdvmrpRouteNextHopSourceMaskValue,
                             &nextObjdvmrpRouteNextHopIfIndexValue  );
      }while(((!memcmp(&objdvmrpRouteNextHopSourceValue, &nextObjdvmrpRouteNextHopSourceValue,sizeof(L7_inet_addr_t))))
             && (owa.l7rc == L7_SUCCESS));
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdvmrpRouteNextHopSourceValue, owa.len);

  /* return the object value: dvmrpRouteNextHopSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdvmrpRouteNextHopSourceValue,
                           sizeof (objdvmrpRouteNextHopSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopSourceMask
*
* @purpose Get 'dvmrpRouteNextHopSourceMask'
*
* @description [dvmrpRouteNextHopSourceMask] The network mask which when combined with the corresponding value of dvmrpRouteNextHopSource identifies the sources for which this entry specifies a next hop on an outgoing interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopSourceMask (void *wap, void *bufp)
{

  xLibU32_t nextObjdvmrpRouteNextHopIfIndexValue;

  L7_inet_addr_t objdvmrpRouteNextHopSourceValue;
  L7_inet_addr_t nextObjdvmrpRouteNextHopSourceValue;

  L7_inet_addr_t objdvmrpRouteNextHopSourceMaskValue;
  L7_inet_addr_t nextObjdvmrpRouteNextHopSourceMaskValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (L7_inet_addr_t);
  /* retrieve key: dvmrpRouteNextHopSource */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopSource,
                          (xLibU8_t *) & objdvmrpRouteNextHopSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpRouteNextHopSourceValue, owa.len);

  /* retrieve key: dvmrpRouteNextHopSourceMask */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopSourceMask,
                          (xLibU8_t *) & objdvmrpRouteNextHopSourceMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpRouteNextHopSourceValue);
    inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpRouteNextHopSourceMaskValue);

    do 
    {
      owa.l7rc = usmDbDvmrpRouteNextHopEntryNextGet (L7_UNIT_CURRENT,
                              &nextObjdvmrpRouteNextHopSourceValue,
                              &nextObjdvmrpRouteNextHopSourceMaskValue,
                              &nextObjdvmrpRouteNextHopIfIndexValue);
    }while( memcmp(&objdvmrpRouteNextHopSourceValue, &nextObjdvmrpRouteNextHopSourceValue, sizeof (L7_inet_addr_t)) && 
            (owa.l7rc == L7_SUCCESS));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpRouteNextHopSourceMaskValue, owa.len);

    memcpy(&nextObjdvmrpRouteNextHopSourceValue, &objdvmrpRouteNextHopSourceValue, sizeof(L7_inet_addr_t));
    memcpy(&nextObjdvmrpRouteNextHopSourceMaskValue, &objdvmrpRouteNextHopSourceMaskValue, sizeof(L7_inet_addr_t));
    nextObjdvmrpRouteNextHopIfIndexValue = 0;
   
    do 
    { 
      owa.l7rc = usmDbDvmrpRouteNextHopEntryNextGet (L7_UNIT_CURRENT,
                              &nextObjdvmrpRouteNextHopSourceValue,
                              &nextObjdvmrpRouteNextHopSourceMaskValue,
                              &nextObjdvmrpRouteNextHopIfIndexValue);
    } while(((memcmp(&nextObjdvmrpRouteNextHopSourceValue, &objdvmrpRouteNextHopSourceValue,  sizeof(L7_inet_addr_t))
            || (memcmp(&nextObjdvmrpRouteNextHopSourceMaskValue, &objdvmrpRouteNextHopSourceMaskValue, sizeof(L7_inet_addr_t))))) && (owa.l7rc == L7_SUCCESS));

    if (owa.l7rc == L7_SUCCESS)
    {
      owa.l7rc = usmDbDvmrpRouteNextHopEntryNextGet (L7_UNIT_CURRENT,
                              &nextObjdvmrpRouteNextHopSourceValue,
                              &nextObjdvmrpRouteNextHopSourceMaskValue,
                              &nextObjdvmrpRouteNextHopIfIndexValue);
    }
    else
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  if ((memcmp(&objdvmrpRouteNextHopSourceValue, &nextObjdvmrpRouteNextHopSourceValue,sizeof(L7_inet_addr_t)))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdvmrpRouteNextHopSourceMaskValue, owa.len);

  /* return the object value: dvmrpRouteNextHopSourceMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdvmrpRouteNextHopSourceMaskValue,
                           sizeof (objdvmrpRouteNextHopSourceMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopType
*
* @purpose Get 'dvmrpRouteNextHopType'
*
* @description [dvmrpRouteNextHopType] Type is leaf if no downstream dependent neighbors exist on the outgoing virtual interface. Otherwise, type is branch.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopType (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpRouteNextHopSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpRouteNextHopSourceValue;
  fpObjWa_t kwadvmrpRouteNextHopSourceMask = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpRouteNextHopSourceMaskValue;
  fpObjWa_t kwadvmrpRouteNextHopIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpRouteNextHopIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdvmrpRouteNextHopTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpRouteNextHopSource */
  kwadvmrpRouteNextHopSource.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopSource,
                   (xLibU8_t *) & keydvmrpRouteNextHopSourceValue, &kwadvmrpRouteNextHopSource.len);
  if (kwadvmrpRouteNextHopSource.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteNextHopSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteNextHopSource);
    return kwadvmrpRouteNextHopSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteNextHopSourceValue, kwadvmrpRouteNextHopSource.len);

  /* retrieve key: dvmrpRouteNextHopSourceMask */
  kwadvmrpRouteNextHopSourceMask.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopSourceMask,
                   (xLibU8_t *) & keydvmrpRouteNextHopSourceMaskValue,
                   &kwadvmrpRouteNextHopSourceMask.len);
  if (kwadvmrpRouteNextHopSourceMask.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteNextHopSourceMask.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteNextHopSourceMask);
    return kwadvmrpRouteNextHopSourceMask.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteNextHopSourceMaskValue,
                           kwadvmrpRouteNextHopSourceMask.len);

  /* retrieve key: dvmrpRouteNextHopIfIndex */
  kwadvmrpRouteNextHopIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteNextHopTable_dvmrpRouteNextHopIfIndex,
                   (xLibU8_t *) & keydvmrpRouteNextHopIfIndexValue,
                   &kwadvmrpRouteNextHopIfIndex.len);
  if (kwadvmrpRouteNextHopIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteNextHopIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteNextHopIfIndex);
    return kwadvmrpRouteNextHopIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteNextHopIfIndexValue,
                           kwadvmrpRouteNextHopIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpNextHopTypeGet (L7_UNIT_CURRENT, &keydvmrpRouteNextHopSourceValue,
                              &keydvmrpRouteNextHopSourceMaskValue,
                              keydvmrpRouteNextHopIfIndexValue, &objdvmrpRouteNextHopTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpRouteNextHopType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpRouteNextHopTypeValue,
                           sizeof (objdvmrpRouteNextHopTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
