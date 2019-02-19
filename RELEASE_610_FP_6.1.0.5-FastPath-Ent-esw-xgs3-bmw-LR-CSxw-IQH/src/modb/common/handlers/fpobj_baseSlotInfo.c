
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseSlotInfo.c
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
#include "_xe_baseSlotInfo_obj.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_cardmgr_api.h"


L7_RC_t fpObjUtil_baseSlotInfo_UnitNumber_SlotIndexNextGet(L7_uint32 *agentInventoryUnitNumber, L7_int32 *agentInventorySlotNumber)
{
  L7_uint32 unit_id;
  L7_uint32 ptrSlots[L7_MAX_PHYSICAL_SLOTS_PER_UNIT];
  L7_uint32 numSlots;
  L7_uint32 i;
/*  L7_BOOL matchFirst = L7_FALSE; */

    if (usmDbUnitMgrUnitTypeGet(*agentInventoryUnitNumber, &unit_id) == L7_SUCCESS)
    {
      if (usmDbUnitDbEntrySlotsGet(unit_id, &numSlots, ptrSlots) == L7_SUCCESS)
      {
        for (i = 0; i < numSlots; i++)
        {
          if (*agentInventorySlotNumber == -1)
          {
            *agentInventorySlotNumber = ptrSlots[i];
            return L7_SUCCESS;
          }

          if (ptrSlots[i] == *agentInventorySlotNumber)
          {
            if (i+1 < numSlots)
            {
              *agentInventorySlotNumber = ptrSlots[i+1];
              return L7_SUCCESS;
            }
            else
            {
              return L7_FAILURE;
            }
          }
        }
      }
    }

  return L7_FAILURE;
}
xLibRC_t fpObjGet_CardDescription_Utility (L7_uint32 cardType, L7_uchar8 *cardDescription)
{

   L7_uint32 cardIndex;   
   L7_uint32 cardType2;
   xLibBool_t flag = XLIB_FALSE;
   L7_RC_t rc= L7_FAILURE;
  

   if(cardType != 0)
   {
      cardIndex =0;

      while (usmDbCardSupportedIndexNextGet(&cardIndex) == L7_SUCCESS)
      {
         if ((usmDbCardSupportedTypeIdGet(cardIndex, &cardType2) == L7_SUCCESS)
            && (cardType2 == cardType))
         {
            flag = XLIB_TRUE;
            break;
         }
      }
      if (flag == XLIB_TRUE)
      {
        rc = usmDbCardSupportedCardDescriptionGet(cardIndex, cardDescription);

      }
  }
  if(rc !=  L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }
  return XLIBRC_SUCCESS;
}

xLibRC_t fpObjGet_CardModel_Utility (L7_uint32 cardType, L7_uchar8 *cardModel)
{

   L7_uint32 cardIndex;
   L7_uint32 cardType2;
   xLibBool_t flag = XLIB_FALSE;
   L7_RC_t rc= L7_FAILURE;


   if(cardType != 0)
   {
      cardIndex =0;

      while (usmDbCardSupportedIndexNextGet(&cardIndex) == L7_SUCCESS)
      {
         if ((usmDbCardSupportedTypeIdGet(cardIndex, &cardType2) == L7_SUCCESS)
            && (cardType2 == cardType))
         {
            flag = XLIB_TRUE;
            break;
         }
      }
      if (flag == XLIB_TRUE)
      {
        rc = usmDbCardSupportedCardModelIdGet(cardIndex, cardModel);

      }
  }
  if(rc !=  L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }
  return XLIBRC_SUCCESS;
}

#if 0
/*******************************************************************************
* @function fpObjGet_baseSlotInfo_UnitNumber_SlotIndex
*
* @purpose Get 'UnitNumber+SlotIndex+'
*
* @description  [UnitNumber] Unit Number.   
[SlotIndex] Slot Number.   

*
* @notes  Get Object Handler for Combo Key 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_UnitNumber_SlotIndex (void *wap, void *bufp[], xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  fpObjWa_t owaUnitNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitNumberValue;
  xLibU32_t nextObjUnitNumberValue;
  void *outUnitNumber = (void *) bufp[--keyCount];

  fpObjWa_t owaSlotIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSlotIndexValue;
  xLibU32_t nextObjSlotIndexValue;
  void *outSlotIndex = (void *) bufp[--keyCount];

  FPOBJ_TRACE_ENTER (outUnitNumber);
  FPOBJ_TRACE_ENTER (outSlotIndex);

  /* retrieve key: UnitNumber  */
  owaUnitNumber.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                                    (xLibU8_t *) & objUnitNumberValue, &owaUnitNumber.len);
  if (owaUnitNumber.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: SlotIndex  */
    owaSlotIndex.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                                     (xLibU8_t *) & objSlotIndexValue, &owaSlotIndex.len);
  }
  else
  {
       usmDbUnitMgrStackMemberGetFirst(&objUnitNumberValue);
	objSlotIndexValue = 0;   
  }

  FPOBJ_TRACE_CURRENT_KEY (outUnitNumber, &objSlotIndexValue, owaUnitNumber.len);
  FPOBJ_TRACE_CURRENT_KEY (outSlotIndex, &objSlotIndexValue, owaSlotIndex.len);

  owa.l7rc = fpObjUtil_baseSlotInfo_UnitNumber_SlotIndexNextGet (
                                  &objUnitNumberValue,
                                  &objSlotIndexValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outUnitNumber, owaUnitNumber);
    FPOBJ_TRACE_EXIT (outSlotIndex, owaSlotIndex);
    return owa.rc;
  }

  
  nextObjUnitNumberValue = objUnitNumberValue;
  nextObjSlotIndexValue = objSlotIndexValue;
  FPOBJ_TRACE_CURRENT_KEY (outUnitNumber, &nextObjSlotIndexValue, owaUnitNumber.len);
  FPOBJ_TRACE_CURRENT_KEY (outSlotIndex, &nextObjSlotIndexValue, owaSlotIndex.len);

  /* Set the object value: UnitNumber */
  owaUnitNumber.rc = xLibBufDataSet (outUnitNumber, (xLibU8_t *) & nextObjUnitNumberValue,
                                     sizeof (nextObjUnitNumberValue));
  FPOBJ_TRACE_EXIT (outUnitNumber, owaUnitNumber);

  /* Set the object value: SlotIndex */
  owaSlotIndex.rc = xLibBufDataSet (outSlotIndex, (xLibU8_t *) & nextObjSlotIndexValue,
                                    sizeof (nextObjSlotIndexValue));
  FPOBJ_TRACE_EXIT (outSlotIndex, owaSlotIndex);

  return XLIBRC_SUCCESS;
}


#endif


/*******************************************************************************
* @function fpObjGet_baseSlotInfo_UnitNumber
*
* @purpose Get 'UnitNumber'
*
* @description [UnitNumber] Unit Number. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_UnitNumber (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnitNumberValue;
  xLibU32_t nextObjUnitNumberValue = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitNumber */
  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                          (xLibU8_t *) & objUnitNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbUnitMgrStackMemberGetFirst(&objUnitNumberValue);
    nextObjUnitNumberValue = objUnitNumberValue;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objUnitNumberValue, owa.len);
    owa.l7rc = usmDbUnitMgrStackMemberGetNext(objUnitNumberValue, &nextObjUnitNumberValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjUnitNumberValue, owa.len);

  /* return the object value: UnitNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjUnitNumberValue,
                           sizeof (objUnitNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSlotInfo_SlotIndex
*
* @purpose Get 'SlotIndex'
*
* @description [SlotIndex] Slot Number. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_SlotIndex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSlotIndexValue;
  xLibU32_t nextObjSlotIndexValue;
  xLibU32_t objUnitNumberValue;
  xLibS32_t slotNum = -1;
  FPOBJ_TRACE_ENTER (bufp);

  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                          (xLibU8_t *) &objUnitNumberValue, &owa.len);
  /* retrieve key: SlotIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                          (xLibU8_t *) & objSlotIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    slotNum = -1;
    owa.l7rc = fpObjUtil_baseSlotInfo_UnitNumber_SlotIndexNextGet(&objUnitNumberValue, &slotNum);
    nextObjSlotIndexValue = slotNum;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objSlotIndexValue, owa.len);
    slotNum = objSlotIndexValue; 
    owa.l7rc = fpObjUtil_baseSlotInfo_UnitNumber_SlotIndexNextGet(&objUnitNumberValue, &slotNum);
    nextObjSlotIndexValue = slotNum;  
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSlotIndexValue, owa.len);

  /* return the object value: SlotIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjSlotIndexValue, sizeof (objSlotIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSlotInfo_SlotStatus
*
* @purpose Get 'SlotStatus'
*
* @description [SlotStatus] Slot Status. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_SlotStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaUnitNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUnitNumberValue;
  fpObjWa_t kwaSlotIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySlotIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSlotStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitNumber */
  kwaUnitNumber.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                                    (xLibU8_t *) & keyUnitNumberValue, &kwaUnitNumber.len);
  if (kwaUnitNumber.rc != XLIBRC_SUCCESS)
  {
    kwaUnitNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUnitNumber);
    return kwaUnitNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, kwaUnitNumber.len);

  /* retrieve key: SlotIndex */
  kwaSlotIndex.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                                   (xLibU8_t *) & keySlotIndexValue, &kwaSlotIndex.len);
  if (kwaSlotIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSlotIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSlotIndex);
    return kwaSlotIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, kwaSlotIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbSlotIsFullGet ( keyUnitNumberValue,
                              keySlotIndexValue, &objSlotStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SlotStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSlotStatusValue, sizeof (objSlotStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSlotInfo_SlotPowerMode
*
* @purpose Get 'SlotPowerMode'
*
* @description [SlotPowerMode] Slot Powerr Mode. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_SlotPowerMode (void *wap, void *bufp)
{

  fpObjWa_t kwaUnitNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUnitNumberValue;
  fpObjWa_t kwaSlotIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySlotIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSlotPowerModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitNumber */
  kwaUnitNumber.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                                    (xLibU8_t *) & keyUnitNumberValue, &kwaUnitNumber.len);
  if (kwaUnitNumber.rc != XLIBRC_SUCCESS)
  {
    kwaUnitNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUnitNumber);
    return kwaUnitNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, kwaUnitNumber.len);

  /* retrieve key: SlotIndex */
  kwaSlotIndex.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                                   (xLibU8_t *) & keySlotIndexValue, &kwaSlotIndex.len);
  if (kwaSlotIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSlotIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSlotIndex);
    return kwaSlotIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, kwaSlotIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbSlotPowerModeGet ( keyUnitNumberValue,
                              keySlotIndexValue, &objSlotPowerModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SlotPowerMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSlotPowerModeValue,
                           sizeof (objSlotPowerModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSlotInfo_SlotPowerMode
*
* @purpose Set 'SlotPowerMode'
*
* @description [SlotPowerMode] Slot Powerr Mode. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSlotInfo_SlotPowerMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSlotPowerModeValue;

  fpObjWa_t kwaUnitNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUnitNumberValue;
  fpObjWa_t kwaSlotIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySlotIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SlotPowerMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSlotPowerModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSlotPowerModeValue, owa.len);

  /* retrieve key: UnitNumber */
  kwaUnitNumber.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                                    (xLibU8_t *) & keyUnitNumberValue, &kwaUnitNumber.len);
  if (kwaUnitNumber.rc != XLIBRC_SUCCESS)
  {
    kwaUnitNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUnitNumber);
    return kwaUnitNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, kwaUnitNumber.len);

  /* retrieve key: SlotIndex */
  kwaSlotIndex.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                                   (xLibU8_t *) & keySlotIndexValue, &kwaSlotIndex.len);
  if (kwaSlotIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSlotIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSlotIndex);
    return kwaSlotIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, kwaSlotIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbSlotPowerModeSet ( keyUnitNumberValue,
                              keySlotIndexValue, objSlotPowerModeValue);

  if (owa.l7rc == L7_ERROR || owa.l7rc == L7_FAILURE)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSlotInfo_SlotAdminMode
*
* @purpose Get 'SlotAdminMode'
*
* @description [SlotAdminMode] Slot Admin Mode. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_SlotAdminMode (void *wap, void *bufp)
{

  fpObjWa_t kwaUnitNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUnitNumberValue;
  fpObjWa_t kwaSlotIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySlotIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSlotAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitNumber */
  kwaUnitNumber.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                                    (xLibU8_t *) & keyUnitNumberValue, &kwaUnitNumber.len);
  if (kwaUnitNumber.rc != XLIBRC_SUCCESS)
  {
    kwaUnitNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUnitNumber);
    return kwaUnitNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, kwaUnitNumber.len);

  /* retrieve key: SlotIndex */
  kwaSlotIndex.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                                   (xLibU8_t *) & keySlotIndexValue, &kwaSlotIndex.len);
  if (kwaSlotIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSlotIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSlotIndex);
    return kwaSlotIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, kwaSlotIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbSlotAdminModeGet ( keyUnitNumberValue,
                              keySlotIndexValue, &objSlotAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SlotAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSlotAdminModeValue,
                           sizeof (objSlotAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSlotInfo_SlotAdminMode
*
* @purpose Set 'SlotAdminMode'
*
* @description [SlotAdminMode] Slot Admin Mode. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSlotInfo_SlotAdminMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSlotAdminModeValue;

  fpObjWa_t kwaUnitNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUnitNumberValue;
  fpObjWa_t kwaSlotIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySlotIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SlotAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSlotAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSlotAdminModeValue, owa.len);

  /* retrieve key: UnitNumber */
  kwaUnitNumber.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                                    (xLibU8_t *) & keyUnitNumberValue, &kwaUnitNumber.len);
  if (kwaUnitNumber.rc != XLIBRC_SUCCESS)
  {
    kwaUnitNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUnitNumber);
    return kwaUnitNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, kwaUnitNumber.len);

  /* retrieve key: SlotIndex */
  kwaSlotIndex.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                                   (xLibU8_t *) & keySlotIndexValue, &kwaSlotIndex.len);
  if (kwaSlotIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSlotIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSlotIndex);
    return kwaSlotIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, kwaSlotIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbSlotAdminModeSet ( keyUnitNumberValue,
                              keySlotIndexValue, objSlotAdminModeValue);

  if (owa.l7rc == L7_ERROR || owa.l7rc == L7_FAILURE)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSlotInfo_InsertedCardType
*
* @purpose Get 'InsertedCardType'
*
* @description [InsertedCardType] Slot Inserted Card Type. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_InsertedCardType (void *wap, void *bufp)
{

  fpObjWa_t kwaUnitNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUnitNumberValue;
  fpObjWa_t kwaSlotIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySlotIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInsertedCardTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitNumber */
  kwaUnitNumber.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                                    (xLibU8_t *) & keyUnitNumberValue, &kwaUnitNumber.len);
  if (kwaUnitNumber.rc != XLIBRC_SUCCESS)
  {
    kwaUnitNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUnitNumber);
    return kwaUnitNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, kwaUnitNumber.len);

  /* retrieve key: SlotIndex */
  kwaSlotIndex.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                                   (xLibU8_t *) & keySlotIndexValue, &kwaSlotIndex.len);
  if (kwaSlotIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSlotIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSlotIndex);
    return kwaSlotIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, kwaSlotIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbCardInsertedCardTypeGet ( keyUnitNumberValue,
                              keySlotIndexValue, &objInsertedCardTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: InsertedCardType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objInsertedCardTypeValue,
                           sizeof (objInsertedCardTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSlotInfo_ConfiguredCardType
*
* @purpose Get 'ConfiguredCardType'
*
* @description [ConfiguredCardType] Slot Configured Card Type. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_ConfiguredCardType (void *wap, void *bufp)
{

  fpObjWa_t kwaUnitNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUnitNumberValue;
  fpObjWa_t kwaSlotIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySlotIndexValue;
  xLibStr256_t objCardTypeValueHexStr;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfiguredCardTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objCardTypeValueHexStr, L7_EOS, sizeof(objCardTypeValueHexStr));
  /* retrieve key: UnitNumber */
  kwaUnitNumber.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                                    (xLibU8_t *) & keyUnitNumberValue, &kwaUnitNumber.len);
  if (kwaUnitNumber.rc != XLIBRC_SUCCESS)
  {
    kwaUnitNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUnitNumber);
    return kwaUnitNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, kwaUnitNumber.len);

  /* retrieve key: SlotIndex */
  kwaSlotIndex.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                                   (xLibU8_t *) & keySlotIndexValue, &kwaSlotIndex.len);
  if (kwaSlotIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSlotIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSlotIndex);
    return kwaSlotIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, kwaSlotIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbCardConfiguredCardTypeGet ( keyUnitNumberValue,
                              keySlotIndexValue, &objConfiguredCardTypeValue);
  if(owa.l7rc == L7_SUCCESS)
  {
    osapiSnprintf(objCardTypeValueHexStr, sizeof(objCardTypeValueHexStr), "0x%x",objConfiguredCardTypeValue);
  }
 
  else 
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ConfiguredCardType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCardTypeValueHexStr,
                           sizeof (objCardTypeValueHexStr));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSlotInfo_SlotCapabilities
*
* @purpose Get 'SlotCapabilities'
*
* @description [SlotCapabilities] Slot Capabilities. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_SlotCapabilities (void *wap, void *bufp)
{

  fpObjWa_t kwaUnitNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUnitNumberValue;
  xLibU32_t unit_id;
  xLibU32_t ux;
  fpObjWa_t kwaSlotIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySlotIndexValue;
  xLibU32_t sx;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSlotCapabilitiesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitNumber */
  kwaUnitNumber.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                                    (xLibU8_t *) & keyUnitNumberValue, &kwaUnitNumber.len);
  if (kwaUnitNumber.rc != XLIBRC_SUCCESS)
  {
    kwaUnitNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUnitNumber);
    return kwaUnitNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, kwaUnitNumber.len);

  /* retrieve key: SlotIndex */
  kwaSlotIndex.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                                   (xLibU8_t *) & keySlotIndexValue, &kwaSlotIndex.len);
  if (kwaSlotIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSlotIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSlotIndex);
    return kwaSlotIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, kwaSlotIndex.len);
  owa.l7rc = L7_FAILURE;
  if (usmDbUnitMgrUnitTypeGet(keyUnitNumberValue, &unit_id) == L7_SUCCESS)

  {
    /* obtain unit index from unit id */
    if (usmDbUnitIndexFromIDGet(unit_id, &ux) == L7_SUCCESS)

    {
      /* get slot index from slot */
      if (usmDbSlotIndexFromNumGet(ux, keySlotIndexValue, &sx) == L7_SUCCESS)
      {
         /* get the value from application */
        owa.l7rc = usmDbSlotSupportedPluggableGet ( ux, sx, &objSlotCapabilitiesValue);
      }
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SlotCapabilities */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSlotCapabilitiesValue,
                           sizeof (objSlotCapabilitiesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSlotInfo_SlotPowerDown
*
* @purpose Get 'SlotPowerDown'
*
* @description [SlotPowerDown] Slot Power Down Status. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_SlotPowerDown (void *wap, void *bufp)
{

  fpObjWa_t kwaUnitNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUnitNumberValue;
  xLibU32_t unit_id;
  xLibU32_t ux;
  fpObjWa_t kwaSlotIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySlotIndexValue;
  xLibU32_t sx;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSlotPowerDownValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitNumber */
  kwaUnitNumber.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                                    (xLibU8_t *) & keyUnitNumberValue, &kwaUnitNumber.len);
  if (kwaUnitNumber.rc != XLIBRC_SUCCESS)
  {
    kwaUnitNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUnitNumber);
    return kwaUnitNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, kwaUnitNumber.len);

  /* retrieve key: SlotIndex */
  kwaSlotIndex.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                                   (xLibU8_t *) & keySlotIndexValue, &kwaSlotIndex.len);
  if (kwaSlotIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSlotIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSlotIndex);
    return kwaSlotIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, kwaSlotIndex.len);

  owa.l7rc = L7_FAILURE;
  if (usmDbUnitMgrUnitTypeGet(keyUnitNumberValue, &unit_id) == L7_SUCCESS)

  {
    /* obtain unit index from unit id */
    if (usmDbUnitIndexFromIDGet(unit_id, &ux) == L7_SUCCESS)

    {
      /* get slot index from slot */
      if (usmDbSlotIndexFromNumGet(ux, keySlotIndexValue, &sx) == L7_SUCCESS)
      {
        /* get the value from application */
        owa.l7rc = usmDbSlotSupportedPowerdownGet ( ux, sx, &objSlotPowerDownValue);
      }
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SlotPowerDown */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSlotPowerDownValue,
                           sizeof (objSlotPowerDownValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSlotInfo_SlotConfigClear
*
* @purpose Set 'SlotConfigClear'
*
* @description [SlotConfigClear] Clear Card Configuration. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSlotInfo_SlotConfigClear (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSlotConfigClearValue;

  fpObjWa_t kwaUnitNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUnitNumberValue;
  fpObjWa_t kwaSlotIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySlotIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SlotConfigClear */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSlotConfigClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSlotConfigClearValue, owa.len);

  /* retrieve key: UnitNumber */
  kwaUnitNumber.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                                    (xLibU8_t *) & keyUnitNumberValue, &kwaUnitNumber.len);
  if (kwaUnitNumber.rc != XLIBRC_SUCCESS)
  {
    kwaUnitNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUnitNumber);
    return kwaUnitNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, kwaUnitNumber.len);

  /* retrieve key: SlotIndex */
  kwaSlotIndex.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                                   (xLibU8_t *) & keySlotIndexValue, &kwaSlotIndex.len);
  if (kwaSlotIndex.rc != XLIBRC_SUCCESS)
  {
    kwaSlotIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSlotIndex);
    return kwaSlotIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, kwaSlotIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbSlotCardConfigClear ( keyUnitNumberValue,
                              keySlotIndexValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSlotInfo_InsertedCardDescription
*
* @purpose Get 'InsertedCardDescription'
 *@description  [InsertedCardDescription] Card Description.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_InsertedCardDescription (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objInsertedCardDescriptionValue;
  
  xLibU32_t keyUnitNumberValue;
  xLibU32_t keySlotIndexValue;
  xLibU32_t cardType;
  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitNumber */
  owa.len = sizeof (keyUnitNumberValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                          (xLibU8_t *) & keyUnitNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, owa.len);

/* retrieve key: SlotIndex */
  owa.len = sizeof (keySlotIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                          (xLibU8_t *) & keySlotIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, owa.len);
  
   owa.l7rc = usmDbCardInsertedCardTypeGet(keyUnitNumberValue, keySlotIndexValue, &cardType);
   if(owa.l7rc != L7_SUCCESS)
  {
   /* TODO: Change if required */
    owa.rc= XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   owa.rc = fpObjGet_CardDescription_Utility(cardType, objInsertedCardDescriptionValue);
#if 0
   if(owa.rc  == XLIBRC_SUCCESS && cardType != 0)      
   {
      cardIndex =0;
      
      while (usmDbCardSupportedIndexNextGet(&cardIndex) == L7_SUCCESS)
      {
         if ((usmDbCardSupportedTypeIdGet(cardIndex, &cardType) == L7_SUCCESS)
            && (cardType2 == cardType))
         {
            flag = L7_TRUE;
            break;
         }
      }
      if (flag == L7_TRUE)
      {      
        owa.l7rc = usmDbCardSupportedCardDescriptionGet(cardIndex, objInsertedCardDescriptionValue);
      
      }
      else
        owa.l7rc = L7_FAILURE;
  }  
#endif     
  if (owa.rc != XLIBRC_SUCCESS)
  {
   /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objInsertedCardDescriptionValue,
                     strlen (objInsertedCardDescriptionValue));

  /* return the object value: InsertedCardDescription */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objInsertedCardDescriptionValue,
                           strlen (objInsertedCardDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#if 0
xLibRC_t fpObjGet_CardDescription_Utility (xLibU32_t unit, xLibU32_t slot, xLibStr256_t cardDescription)
{
   
   xLibU32_t cardIndex;
   xLibU32_t cardType;
   xLibU32_t cardType2;
   xLibBool_t flag;
   L7_RC_t rc;

  
   rc = usmDbCardConfiguredCardTypeGet(unit, slot, &cardType);
   if(rc  == L7_SUCCESS && cardType != 0)
   {
      cardIndex =0;

      while (usmDbCardSupportedIndexNextGet(&cardIndex) == L7_SUCCESS)
      {
         if ((usmDbCardSupportedTypeIdGet(cardIndex, &cardType2) == L7_SUCCESS)
            && (cardType2 == cardType))
         {
            flag = XLIB_TRUE;
            break;
         }
      }
      if (flag == XLIB_TRUE)
      {
        rc = usmDbCardSupportedCardDescriptionGet(cardIndex, cardDescription);

      }
  }
  if(rc !=  L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }
  return XLIBRC_SUCCESS; 
}
#endif
/*******************************************************************************
* @function fpObjGet_baseSlotInfo_ConfiguredCardDescription
*
* @purpose Get 'ConfiguredCardDescription'
 *@description  [ConfiguredCardDescription] Card Description.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_ConfiguredCardDescription (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objConfiguredCardDescriptionValue;

  xLibU32_t keyUnitNumberValue;
  xLibU32_t keySlotIndexValue;
  xLibU32_t cardType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitNumber */
  owa.len = sizeof (keyUnitNumberValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                          (xLibU8_t *) & keyUnitNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, owa.len);

  /* retrieve key: SlotIndex */
  owa.len = sizeof (keySlotIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                          (xLibU8_t *) & keySlotIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, owa.len);
 /* get the value from application */
  owa.l7rc = usmDbCardConfiguredCardTypeGet(keyUnitNumberValue, keySlotIndexValue, &cardType);
   if(owa.l7rc != L7_SUCCESS)
  {
   /* TODO: Change if required */
    owa.rc= XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
   
  owa.rc = fpObjGet_CardDescription_Utility( cardType, objConfiguredCardDescriptionValue);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objConfiguredCardDescriptionValue,
                     strlen (objConfiguredCardDescriptionValue));

  /* return the object value: ConfiguredCardDescription */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objConfiguredCardDescriptionValue,
                           strlen (objConfiguredCardDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSlotInfo_InsertedCardModel
*
* @purpose Get 'InsertedCardModel'
 *@description  [InsertedCardModel] Slot Inserted Card Model.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_InsertedCardModel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objInsertedCardModelValue;

  xLibU32_t keyUnitNumberValue;
  xLibU32_t keySlotIndexValue;
  xLibU32_t cardType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitNumber */
  owa.len = sizeof (keyUnitNumberValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                          (xLibU8_t *) & keyUnitNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, owa.len);

  /* retrieve key: SlotIndex */
  owa.len = sizeof (keySlotIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                          (xLibU8_t *) & keySlotIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCardInsertedCardTypeGet(keyUnitNumberValue, keySlotIndexValue, &cardType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.rc = fpObjGet_CardModel_Utility(cardType, objInsertedCardModelValue);
  if (owa.rc != XLIBRC_SUCCESS)
  {
   /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_VALUE (bufp, objInsertedCardModelValue, sizeof (objInsertedCardModelValue));

  /* return the object value: InsertedCardModel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objInsertedCardModelValue,
                           sizeof (objInsertedCardModelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSlotInfo_ConfiguredCardModel
*
* @purpose Get 'ConfiguredCardModel'
 *@description  [ConfiguredCardModel] Slot Configured Card Model.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_ConfiguredCardModel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objConfiguredCardModelValue;

  xLibU32_t keyUnitNumberValue;
  xLibU32_t keySlotIndexValue;
  xLibU32_t cardType;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitNumber */
  owa.len = sizeof (keyUnitNumberValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                          (xLibU8_t *) & keyUnitNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, owa.len);

  /* retrieve key: SlotIndex */
  owa.len = sizeof (keySlotIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                          (xLibU8_t *) & keySlotIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCardConfiguredCardTypeGet(keyUnitNumberValue, keySlotIndexValue, &cardType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  owa.rc = fpObjGet_CardModel_Utility( cardType, objConfiguredCardModelValue);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objConfiguredCardModelValue, sizeof (objConfiguredCardModelValue));

  /* return the object value: ConfiguredCardModel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objConfiguredCardModelValue,
                           sizeof (objConfiguredCardModelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
i* @function fpObjGet_baseSlotInfo_Slot
*
* @purpose Get 'Slot'
 *@description  [Slot] Object used display unit/slot format in Slot Summary Page
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSlotInfo_Slot (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objUnitSlotValue;  
  xLibU32_t keyUnitNumberValue;
  xLibU32_t keySlotIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  memset(objUnitSlotValue, L7_EOS, sizeof(objUnitSlotValue));
  
  /* retrieve key: UnitNumber */
  owa.len = sizeof (keyUnitNumberValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_UnitNumber,
                          (xLibU8_t *) & keyUnitNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnitNumberValue, owa.len);   

  /* retrieve key: SlotIndex */
  owa.len = sizeof (keySlotIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSlotInfo_SlotIndex,
                          (xLibU8_t *) & keySlotIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySlotIndexValue, owa.len);
  
#ifdef L7_STACKING_PACKAGE
    osapiSnprintf(objUnitSlotValue, sizeof(objUnitSlotValue), "%u/%u", keyUnitNumberValue, keySlotIndexValue);
#else
    osapiSnprintf(objUnitSlotValue, sizeof(objUnitSlotValue), "%u", keySlotIndexValue);
#endif

  /* return the object value: Slot */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)objUnitSlotValue,
                           strlen(objUnitSlotValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

