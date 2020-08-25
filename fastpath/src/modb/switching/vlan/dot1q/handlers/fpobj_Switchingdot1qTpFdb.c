
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_Switchingdot1qTpFdb.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  27 May 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_Switchingdot1qTpFdb_obj.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_Switchingdot1qTpFdb_Id
*
* @purpose Get 'Id'
 *@description  [Id] The identity of this Filtering Database.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qTpFdb_Id (void *wap, void *bufp)
{

  xLibU32_t objIdValue;
  xLibU32_t nextObjIdValue;
  xLibStr256_t objAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpFdb_Id, (xLibU8_t *) & objIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objIdValue = 0;
    nextObjIdValue = 0;
    memset (objAddressValue, 0, sizeof (objAddressValue));
    owa.l7rc = usmDbDot1qTpFdbEntryNextGet(L7_UNIT_CURRENT,
                                     &objIdValue,
                                     objAddressValue );
   nextObjIdValue = objIdValue;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIdValue, owa.len);
    memset (objAddressValue, 0, sizeof (objAddressValue));
    nextObjIdValue = objIdValue;
    do
    {
      owa.l7rc = usmDbDot1qTpFdbEntryNextGet (L7_UNIT_CURRENT,
                                      &nextObjIdValue,
                                      objAddressValue );
    }
    while ((objIdValue == nextObjIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIdValue, owa.len);

  /* return the object value: Id */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIdValue, sizeof (objIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1qTpFdb_Address
*
* @purpose Get 'Address'
 *@description  [Address] A unicast MAC address for which the device has
* forwarding and/or filtering information.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qTpFdb_Address (void *wap, void *bufp)
{

  xLibU32_t objIdValue;
  xLibU32_t nextObjIdValue;
  xLibStr256_t objAddressValue;
  xLibStr256_t nextObjAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpFdb_Id, (xLibU8_t *) & objIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIdValue, owa.len);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpFdb_Address,
                          (xLibU8_t *) objAddressValue, &owa.len);
  nextObjIdValue = objIdValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (objAddressValue, 0, sizeof (objAddressValue));
    memset(nextObjAddressValue,0,sizeof(nextObjAddressValue));
    owa.l7rc = usmDbDot1qTpFdbEntryNextGet(L7_UNIT_CURRENT,
                                     &objIdValue,
                                     nextObjAddressValue );
   
   
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objAddressValue, owa.len);

    owa.l7rc = usmDbDot1qTpFdbEntryNextGet(L7_UNIT_CURRENT,
                                    &objIdValue,
                                    objAddressValue );
    strcpy(nextObjAddressValue,objAddressValue);
  }

  if ((objIdValue != nextObjIdValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjAddressValue, owa.len);

  /* return the object value: Address */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjAddressValue, strlen (objAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1qTpFdb_Port
*
* @purpose Get 'Port'
 *@description  [Port] Either the value '0', or the port number of the port on
* which a frame having a source address equal to the value of the
* corresponding instance of dot1qTpFdbAddress has been seen. A value
* of '0' indicates that the port number has not been learned but
* that the device does have some forwarding/filtering information
* about this address (e.g. in the dot1qStaticUnicastTable).
* Implementors are encouraged to assign the port value to this object
* whenever it is learned even for addresses for which the corresponding
* value of dot1qTpFdbStatus is not learned(3).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qTpFdb_Port (void *wap, void *bufp)
{

  fpObjWa_t kwaId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t kwaAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwaId.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpFdb_Id,
                            (xLibU8_t *) & keyIdValue, &kwaId.len);
  if (kwaId.rc != XLIBRC_SUCCESS)
  {
    kwaId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaId);
    return kwaId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwaId.len);

  /* retrieve key: Address */
  kwaAddress.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpFdb_Address,
                                 (xLibU8_t *) keyAddressValue, &kwaAddress.len);
  if (kwaAddress.rc != XLIBRC_SUCCESS)
  {
    kwaAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaAddress);
    return kwaAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwaAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qTpFdbPortGet (L7_UNIT_CURRENT, keyIdValue, keyAddressValue, &objPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Port */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortValue, sizeof (objPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1qTpFdb_Status
*
* @purpose Get 'Status'
 *@description  [Status] The status of this entry. The meanings of the values
* are: other(1) - none of the following. This may include the case
* where some other MIB object (not the corresponding instance of
* dot1qTpFdbPort, nor an entry in the dot1qStaticUnicastTable) is being
* used to determine if and how frames addressed to the value of
* the corresponding instance of dot1qTpFdbAddress are being
* forwarded. invalid(2) - this entry is no longer valid (e.g., it was
* learned but has since aged out), but has not yet been flushed from the
* table. learned(3) - the value of the corresponding instance of
* dot1qTpFdbPort was learned and is being used. self(4) - the value
* of the corresponding instance of dot1qTpFdbAddress represents one
* of the device's addresses. The corresponding instance of
* dot1qTpFdbPort indicates which of the device's ports has this address.
* mgmt(5) - the value of the corresponding instance of
* dot1qTpFdbAddress is also the value of an existing instance of
* dot1qStaticAddress.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qTpFdb_Status (void *wap, void *bufp)
{

  fpObjWa_t kwaId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t kwaAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwaId.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpFdb_Id,
                            (xLibU8_t *) & keyIdValue, &kwaId.len);
  if (kwaId.rc != XLIBRC_SUCCESS)
  {
    kwaId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaId);
    return kwaId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwaId.len);

  /* retrieve key: Address */
  kwaAddress.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qTpFdb_Address,
                                 (xLibU8_t *) keyAddressValue, &kwaAddress.len);
  if (kwaAddress.rc != XLIBRC_SUCCESS)
  {
    kwaAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaAddress);
    return kwaAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyAddressValue, kwaAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qTpFdbStatusGet(L7_UNIT_CURRENT, keyIdValue, keyAddressValue, &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue, sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
