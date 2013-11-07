
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_basesFlowRcvrTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  05 June 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_basesFlowRcvrTable_obj.h"
#include "usmdb_sflow.h"

/*******************************************************************************
* @function fpObjGet_basesFlowRcvrTable_RcvrIndex
*
* @purpose Get 'RcvrIndex'
 *@description  [RcvrIndex] Index into sFlowReceiverTable.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowRcvrTable_RcvrIndex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvrIndexValue;
  xLibU32_t nextObjRcvrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RcvrIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                          (xLibU8_t *) & objRcvrIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjRcvrIndexValue = 0;
    owa.l7rc = usmdbsFlowRcvrEntryNextGet (L7_UNIT_CURRENT, &nextObjRcvrIndexValue);
  }
  else
  {
    nextObjRcvrIndexValue = objRcvrIndexValue;
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRcvrIndexValue, owa.len);
    owa.l7rc = usmdbsFlowRcvrEntryNextGet (L7_UNIT_CURRENT, &nextObjRcvrIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjRcvrIndexValue, owa.len);

  /* return the object value: RcvrIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjRcvrIndexValue,
                           sizeof (nextObjRcvrIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_basesFlowRcvrTable_RcvrOwner
*
* @purpose Get 'RcvrOwner'
 *@description  [RcvrOwner] The entity making use of this sFlowRcvrTable entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowRcvrTable_RcvrOwner (void *wap, void *bufp)
{

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRcvrOwnerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  /* get the value from application */
  owa.l7rc = usmdbsFlowRcvrOwnerGet (L7_UNIT_CURRENT, keyRcvrIndexValue, objRcvrOwnerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RcvrOwner */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objRcvrOwnerValue, strlen (objRcvrOwnerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesFlowRcvrTable_RcvrOwner
*
* @purpose Set 'RcvrOwner'
 *@description  [RcvrOwner] The entity making use of this sFlowRcvrTable entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesFlowRcvrTable_RcvrOwner (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t firstObjRcvrOwnerValue;
  xLibStr256_t objRcvrOwnerValue;
  xLibU32_t objRcvrTimeoutValue;
  fpObjWa_t kwaRcvrTimeout = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RcvrOwner */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objRcvrOwnerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRcvrOwnerValue, owa.len);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  kwaRcvrTimeout.len = sizeof(objRcvrTimeoutValue);
  kwaRcvrTimeout.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrTimeout,
                                   (xLibU8_t *)  &objRcvrTimeoutValue, &kwaRcvrTimeout.len);
  if (kwaRcvrTimeout.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrTimeout.rc = XLIBRC_SFLOW_INVALID_RCVRTIMEOUT;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrTimeout);
    return kwaRcvrTimeout.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  owa.l7rc = usmdbsFlowRcvrOwnerGet (L7_UNIT_CURRENT, keyRcvrIndexValue, firstObjRcvrOwnerValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  if(strlen(objRcvrOwnerValue) == 0)
  {
     if((strlen(firstObjRcvrOwnerValue)==0) && (objRcvrTimeoutValue!=0))
     {
        owa.rc= XLIBRC_FAILURE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
     }
     else
     {
        if(objRcvrTimeoutValue==0)
        { 
         owa.rc = XLIBRC_SUCCESS;
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
        }
     }
  }

  else if (strlen(objRcvrOwnerValue) != 0)
  {
     if((strlen(firstObjRcvrOwnerValue)==0) &&(objRcvrTimeoutValue==0))
     {
          owa.rc = XLIBRC_FAILURE;
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        
     }
    else
     {
       if  (objRcvrTimeoutValue==0) 
       { 
        owa.rc=XLIBRC_SUCCESS;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
       } 
 
     } 
  } 



  owa.l7rc = usmdbsFlowRcvrOwnerSet (L7_UNIT_CURRENT, keyRcvrIndexValue, 
                                                          objRcvrOwnerValue, objRcvrTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesFlowRcvrTable_RcvrTimeout
*
* @purpose Get 'RcvrTimeout'
 *@description  [RcvrTimeout] The time (in seconds) remaining before the sampler
* is released and stops sampling.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowRcvrTable_RcvrTimeout (void *wap, void *bufp)
{

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvrTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  /* get the value from application */
  owa.l7rc = usmdbsFlowRcvrTimeoutGet (L7_UNIT_CURRENT, keyRcvrIndexValue, &objRcvrTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RcvrTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRcvrTimeoutValue, sizeof (objRcvrTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesFlowRcvrTable_RcvrTimeout
*
* @purpose Set 'RcvrTimeout'
 *@description  [RcvrTimeout] The time (in seconds) remaining before the sampler
* is released and stops sampling.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesFlowRcvrTable_RcvrTimeout (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvrTimeoutValue;
  xLibStr256_t EnteredObjRcvrOwnerValue;

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  fpObjWa_t kwaRcvrOwner = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRcvrOwnerValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RcvrTimeout */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRcvrTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRcvrTimeoutValue, owa.len);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  kwaRcvrOwner.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrOwner,
                                (xLibU8_t *) EnteredObjRcvrOwnerValue, &kwaRcvrOwner.len);
  if (kwaRcvrOwner.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrOwner.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrOwner.rc);
    return kwaRcvrOwner.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &EnteredObjRcvrOwnerValue, kwaRcvrOwner.len);

  owa.l7rc = usmdbsFlowRcvrOwnerGet (L7_UNIT_CURRENT, keyRcvrIndexValue, objRcvrOwnerValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

 if(strlen(EnteredObjRcvrOwnerValue) == 0)
  {
     owa.rc = XLIBRC_SUCCESS;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  } 
 
  else if  (strcmp(EnteredObjRcvrOwnerValue,objRcvrOwnerValue)==0)
  {
      /* set the value in application */
     owa.l7rc = usmdbsFlowRcvrTimeoutSet (L7_UNIT_CURRENT, keyRcvrIndexValue, objRcvrTimeoutValue);

     if (owa.l7rc == L7_SUCCESS)
     {
        owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
  }
     else
     {
        owa.rc = XLIBRC_FAILURE;
     }
  }

  else
  {
     owa.rc = XLIBRC_FAILURE;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesFlowRcvrTable_RcvrMaximumDatagramSize
*
* @purpose Get 'RcvrMaximumDatagramSize'
 *@description  [RcvrMaximumDatagramSize] The maximum number of data bytes that
* can be sent in a single sample datagram.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowRcvrTable_RcvrMaximumDatagramSize (void *wap, void *bufp)
{

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvrMaximumDatagramSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  /* get the value from application */
  owa.l7rc = usmdbSflowRcvrMaxDatagramSizeGet (L7_UNIT_CURRENT, keyRcvrIndexValue, &objRcvrMaximumDatagramSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RcvrMaximumDatagramSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRcvrMaximumDatagramSizeValue,
                           sizeof (objRcvrMaximumDatagramSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesFlowRcvrTable_RcvrMaximumDatagramSize
*
* @purpose Set 'RcvrMaximumDatagramSize'
 *@description  [RcvrMaximumDatagramSize] The maximum number of data bytes that
* can be sent in a single sample datagram.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesFlowRcvrTable_RcvrMaximumDatagramSize (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvrMaximumDatagramSizeValue;
  xLibStr256_t EnteredObjRcvrOwnerValue;

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  fpObjWa_t kwaRcvrOwner = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRcvrOwnerValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RcvrMaximumDatagramSize */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRcvrMaximumDatagramSizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRcvrMaximumDatagramSizeValue, owa.len);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

   kwaRcvrOwner.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrOwner,
                                (xLibU8_t *) EnteredObjRcvrOwnerValue, &kwaRcvrOwner.len);
  if (kwaRcvrOwner.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrOwner.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrOwner.rc);
    return kwaRcvrOwner.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &EnteredObjRcvrOwnerValue, kwaRcvrOwner.len);

  owa.l7rc = usmdbsFlowRcvrOwnerGet (L7_UNIT_CURRENT, keyRcvrIndexValue, objRcvrOwnerValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (strlen(objRcvrOwnerValue)==0)
  {
   owa.rc = XLIBRC_SUCCESS;
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
  } 
  else  if(strcmp(EnteredObjRcvrOwnerValue,objRcvrOwnerValue)==0)
  {

     /* set the value in application */
     owa.l7rc = usmdbsFlowRcvrMaximumDatagramSizeSet (L7_UNIT_CURRENT, keyRcvrIndexValue, objRcvrMaximumDatagramSizeValue);

     if (owa.l7rc == L7_SUCCESS)
     { 
        owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
     }
     else
     {
        owa.rc = XLIBRC_FAILURE;
     }
  }

  else
  {
     owa.rc = XLIBRC_FAILURE;   
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesFlowRcvrTable_RcvrAddressType
*
* @purpose Get 'RcvrAddressType'
 *@description  [RcvrAddressType] The type of sFlowRcvrCollectorAddress.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowRcvrTable_RcvrAddressType (void *wap, void *bufp)
{

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvrAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  /* get the value from application */
  owa.l7rc = usmdbsFlowRcvrAddressTypeGet (L7_UNIT_CURRENT, keyRcvrIndexValue, &objRcvrAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RcvrAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRcvrAddressTypeValue,
                           sizeof (objRcvrAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesFlowRcvrTable_RcvrAddressType
*
* @purpose Set 'RcvrAddressType'
 *@description  [RcvrAddressType] The type of sFlowRcvrCollectorAddress.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesFlowRcvrTable_RcvrAddressType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvrAddressTypeValue;

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RcvrAddressType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRcvrAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRcvrAddressTypeValue, owa.len);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  /* set the value in application */
  owa.l7rc = usmdbsFlowRcvrAddressTypeSet (L7_UNIT_CURRENT, keyRcvrIndexValue, objRcvrAddressTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
     owa.rc = XLIBRC_FAILURE;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesFlowRcvrTable_RcvrAddress
*
* @purpose Get 'RcvrAddress'
 *@description  [RcvrAddress] The IP address of the sFlow collector.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowRcvrTable_RcvrAddress (void *wap, void *bufp)
{

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objRcvrAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  /* get the value from application */
  owa.l7rc = usmdbsFlowRcvrAddressGet (L7_UNIT_CURRENT, keyRcvrIndexValue, &objRcvrAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RcvrAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objRcvrAddressValue, sizeof (objRcvrAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesFlowRcvrTable_RcvrAddress
*
* @purpose Set 'RcvrAddress'
 *@description  [RcvrAddress] The IP address of the sFlow collector.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesFlowRcvrTable_RcvrAddress (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objRcvrAddressValue;
  xLibStr256_t EnteredObjRcvrOwnerValue;
  xLibStr256_t RcvrAddressStrValue;

  fpObjWa_t kwaRcvrOwner = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRcvrOwnerValue;

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RcvrAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objRcvrAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRcvrAddressValue, owa.len);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  kwaRcvrOwner.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrOwner,
                                   (xLibU8_t *) EnteredObjRcvrOwnerValue, &kwaRcvrOwner.len);
  if (kwaRcvrOwner.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrOwner.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrOwner);
    return kwaRcvrOwner.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &EnteredObjRcvrOwnerValue, kwaRcvrOwner.len);

  owa.l7rc = usmdbsFlowRcvrOwnerGet (L7_UNIT_CURRENT, keyRcvrIndexValue, objRcvrOwnerValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (strlen(objRcvrOwnerValue)==0)
  {
   owa.rc = XLIBRC_SUCCESS;
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
  } 

  else if(strcmp(EnteredObjRcvrOwnerValue,objRcvrOwnerValue)==0)
  {
  /* set the value in application */
  inetAddrPrint(&objRcvrAddressValue, RcvrAddressStrValue);
  
  owa.l7rc = usmdbsFlowRcvrAddressSet (L7_UNIT_CURRENT, keyRcvrIndexValue, RcvrAddressStrValue);

    if (owa.l7rc == L7_SUCCESS)
  {
       owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    }
    else
    {
       owa.rc = XLIBRC_FAILURE;
    }

  }
  else
  {
     owa.rc = XLIBRC_FAILURE;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesFlowRcvrTable_RcvrPort
*
* @purpose Get 'RcvrPort'
 *@description  [RcvrPort] The destination port for sFlow datagrams.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowRcvrTable_RcvrPort (void *wap, void *bufp)
{

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvrPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  /* get the value from application */
  owa.l7rc = usmdbsFlowRcvrPortGet (L7_UNIT_CURRENT, keyRcvrIndexValue, &objRcvrPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RcvrPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRcvrPortValue, sizeof (objRcvrPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesFlowRcvrTable_RcvrPort
*
* @purpose Set 'RcvrPort'
 *@description  [RcvrPort] The destination port for sFlow datagrams.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesFlowRcvrTable_RcvrPort (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvrPortValue;

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaRcvrOwner = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
 
  xLibU32_t keyRcvrIndexValue;
  xLibStr256_t objRcvrOwnerValue;
  xLibStr256_t EnteredObjRcvrOwnerValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RcvrPort */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRcvrPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRcvrPortValue, owa.len);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  kwaRcvrOwner.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrOwner,
                                   (xLibU8_t *) EnteredObjRcvrOwnerValue, &kwaRcvrOwner.len);
  if (kwaRcvrOwner.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrOwner.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrOwner);
    return kwaRcvrOwner.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &EnteredObjRcvrOwnerValue, kwaRcvrOwner.len);

  owa.l7rc = usmdbsFlowRcvrOwnerGet (L7_UNIT_CURRENT, keyRcvrIndexValue, objRcvrOwnerValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (strlen(objRcvrOwnerValue)==0)
  {
   owa.rc = XLIBRC_SUCCESS;
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
  } 

  else if(strcmp(EnteredObjRcvrOwnerValue,objRcvrOwnerValue)==0)
  {
     /* set the value in application */
     owa.l7rc = usmdbsFlowRcvrPortSet (L7_UNIT_CURRENT, keyRcvrIndexValue, objRcvrPortValue);

     if (owa.l7rc == L7_SUCCESS)
     {
        owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
     }
     else
     {
        owa.rc = XLIBRC_FAILURE;
  }

  }
  else
  {
     owa.rc = XLIBRC_FAILURE;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesFlowRcvrTable_RcvrDatagramVersion
*
* @purpose Get 'RcvrDatagramVersion'
 *@description  [RcvrDatagramVersion] The version of sFlow datagrams that should
* be sent.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesFlowRcvrTable_RcvrDatagramVersion (void *wap, void *bufp)
{

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvrDatagramVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  /* get the value from application */
  owa.l7rc = usmdbsFlowRcvrDatagramVersionGet (L7_UNIT_CURRENT, keyRcvrIndexValue, &objRcvrDatagramVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RcvrDatagramVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRcvrDatagramVersionValue,
                           sizeof (objRcvrDatagramVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesFlowRcvrTable_RcvrDatagramVersion
*
* @purpose Set 'RcvrDatagramVersion'
 *@description  [RcvrDatagramVersion] The version of sFlow datagrams that should
* be sent.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesFlowRcvrTable_RcvrDatagramVersion (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvrDatagramVersionValue;

  fpObjWa_t kwaRcvrIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRcvrIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RcvrDatagramVersion */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRcvrDatagramVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRcvrDatagramVersionValue, owa.len);

  /* retrieve key: RcvrIndex */
  kwaRcvrIndex.rc = xLibFilterGet (wap, XOBJ_basesFlowRcvrTable_RcvrIndex,
                                   (xLibU8_t *) & keyRcvrIndexValue, &kwaRcvrIndex.len);
  if (kwaRcvrIndex.rc != XLIBRC_SUCCESS)
  {
    kwaRcvrIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRcvrIndex);
    return kwaRcvrIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRcvrIndexValue, kwaRcvrIndex.len);

  /* set the value in application */
  owa.l7rc = usmdbsFlowRcvrDatagramVersionSet (L7_UNIT_CURRENT, keyRcvrIndexValue, objRcvrDatagramVersionValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
