
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseCardTypeDetails.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  17 April 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseCardTypeDetails_obj.h"
#include "usmdb_cardmgr_api.h"

/*******************************************************************************
* @function fpObjGet_baseStacking_SupportedUnitIndex
*
* @purpose Get 'SupportedUnitIndex'
*
* @description Index for Supported Unit Types 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCardTypeDetails_SwitchIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSwitchIndexValue;
  xLibU32_t nextSwitchIndexValue;
  xLibU32_t tempUnitType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SwitchIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseCardTypeDetails_SwitchIndex,
                          (xLibU8_t *) & objSwitchIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextSwitchIndexValue = 0;
    objSwitchIndexValue = 0;
    tempUnitType =0;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objSwitchIndexValue, owa.len);
  
  owa.l7rc = usmDbUnitMgrSupportedUnitIdNextGet (&objSwitchIndexValue,
                                                 &tempUnitType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextSwitchIndexValue = objSwitchIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextSwitchIndexValue, owa.len);

  /* return the object value: SwitchIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextSwitchIndexValue,
                           sizeof (nextSwitchIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseCardTypeDetails_CardIndex
*
* @purpose Get 'CardIndex'
*
* @description [CardIndex] Card Number. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCardTypeDetails_CardIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSwitchIndexValue = 0;
  xLibU32_t objCardIndexValue = 0;
  xLibU32_t nextSwitchIndexValue = 0;
  xLibU32_t nextObjCardIndexValue = 0;
  xLibU32_t SlotIndex = 0;
  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: SwitchIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseCardTypeDetails_SwitchIndex,
                          (xLibU8_t *) & objSwitchIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    /* If SwitchIndex filter is missing (as in case of SNMP code
     * that thinks CardIndex is the only key according to MIB)
     * while calling these handler functions, iterate and return the
     * card indices of all supported card types in the system */ 
  
    /* retrieve key: CardIndex */
    owa.rc = xLibFilterGet (wap, XOBJ_baseCardTypeDetails_CardIndex,
                            (xLibU8_t *) & objCardIndexValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
      FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
      objCardIndexValue = 0;
    }
  
    owa.l7rc  =  usmDbCardSupportedPhysicalIndexNextGet(&objCardIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  
    FPOBJ_TRACE_NEW_KEY (bufp, &objCardIndexValue, owa.len);
  
    /* return the object value: CardIndex */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCardIndexValue, sizeof (objCardIndexValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    /* If SwitchIndex filter is not missing (as in case of WEB code)
     * while calling these handler functions, iterate and return the
     * card indices of all supported card types in the Supported SwitchType
     * corresponding to the Switch Index */ 
  
    /* retrieve key: CardIndex */
    owa.rc = xLibFilterGet (wap, XOBJ_baseCardTypeDetails_CardIndex,
                            (xLibU8_t *) & objCardIndexValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
      FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
      objCardIndexValue = 0;
    }
    SlotIndex = 0;
    nextSwitchIndexValue = objSwitchIndexValue; 
    nextObjCardIndexValue = objCardIndexValue;
    do
    {
      owa.l7rc  =  usmDbCardInSlotSupportedNextGet(&nextSwitchIndexValue, &SlotIndex, &nextObjCardIndexValue);
    }
    while((owa.l7rc == L7_SUCCESS) && (nextSwitchIndexValue == objSwitchIndexValue) &&
          (objCardIndexValue == nextObjCardIndexValue));

    if ((owa.l7rc != L7_SUCCESS) || (nextSwitchIndexValue != objSwitchIndexValue))
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  
    FPOBJ_TRACE_NEW_KEY (bufp, &nextObjCardIndexValue, owa.len);
  
    /* return the object value: CardIndex */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjCardIndexValue, sizeof (nextObjCardIndexValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
}

/*******************************************************************************
* @function fpObjGet_baseCardTypeDetails_SupportedCard
*
* @purpose Get 'Supported Card'
*
* @description [CardIndex] Card Number.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCardTypeDetails_SupportedCard (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCardIndexValue;
  xLibStr256_t objSupportCardValue;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objSupportCardValue, L7_EOS, sizeof(objSupportCardValue));
  /* retrieve key: CardIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseCardTypeDetails_CardIndex,
                          (xLibU8_t *) &objCardIndexValue, &owa.len);
  
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objCardIndexValue = 0;
  }

  owa.l7rc = usmDbCardSupportedCardModelIdGet(objCardIndexValue, objSupportCardValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, objSupportCardValue, owa.len);

  /* return the object value: CardIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSupportCardValue, strlen(objSupportCardValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjSet_baseCardTypeDetails_SupportedCard (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseCardTypeDetails_CardType
*
* @purpose Get 'CardType'
*
* @description [CardType] Card Type. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCardTypeDetails_CardType (void *wap, void *bufp)
{

  fpObjWa_t kwaCardIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCardIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCardTypeValue;
  xLibStr256_t objCardTypeValueHexStr;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objCardTypeValueHexStr, L7_EOS, sizeof(objCardTypeValueHexStr));
  /* retrieve key: CardIndex */
  kwaCardIndex.rc = xLibFilterGet (wap, XOBJ_baseCardTypeDetails_CardIndex,
                                   (xLibU8_t *) & keyCardIndexValue, &kwaCardIndex.len);
  if (kwaCardIndex.rc != XLIBRC_SUCCESS)
  {
    kwaCardIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaCardIndex);
    return kwaCardIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCardIndexValue, kwaCardIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbCardSupportedTypeIdGet ( keyCardIndexValue, &objCardTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiSnprintf(objCardTypeValueHexStr, sizeof(objCardTypeValueHexStr), "0x%x",objCardTypeValue);
  

  /* return the object value: CardType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)objCardTypeValueHexStr, sizeof (objCardTypeValueHexStr));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseCardTypeDetails_CardModelIdentifier
*
* @purpose Get 'CardModelIdentifier'
*
* @description [CardModelIdentifier] Card Model Identifier. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCardTypeDetails_CardModelIdentifier (void *wap, void *bufp)
{

  fpObjWa_t kwaCardIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCardIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objCardModelIdentifierValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CardIndex */
  memset(objCardModelIdentifierValue, L7_EOS, sizeof(objCardModelIdentifierValue));
  kwaCardIndex.rc = xLibFilterGet (wap, XOBJ_baseCardTypeDetails_CardIndex,
                                   (xLibU8_t *) & keyCardIndexValue, &kwaCardIndex.len);
  if (kwaCardIndex.rc != XLIBRC_SUCCESS)
  {
    kwaCardIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaCardIndex);
    return kwaCardIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCardIndexValue, kwaCardIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbCardSupportedCardModelIdGet ( keyCardIndexValue, objCardModelIdentifierValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CardModelIdentifier */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objCardModelIdentifierValue,
                           strlen (objCardModelIdentifierValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseCardTypeDetails_CardDescription
*
* @purpose Get 'CardDescription'
*
* @description [CardDescription] Card Description. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCardTypeDetails_CardDescription (void *wap, void *bufp)
{

  fpObjWa_t kwaCardIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCardIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objCardDescriptionValue;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objCardDescriptionValue, L7_EOS, sizeof(objCardDescriptionValue));
  /* retrieve key: CardIndex */
  kwaCardIndex.rc = xLibFilterGet (wap, XOBJ_baseCardTypeDetails_CardIndex,
                                   (xLibU8_t *) & keyCardIndexValue, &kwaCardIndex.len);
  if (kwaCardIndex.rc != XLIBRC_SUCCESS)
  {
    kwaCardIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaCardIndex);
    return kwaCardIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCardIndexValue, kwaCardIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbCardSupportedCardDescriptionGet ( keyCardIndexValue, objCardDescriptionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CardDescription */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objCardDescriptionValue,
                           strlen (objCardDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
