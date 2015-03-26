/*******************************************************************************

*

* (C) Copyright Broadcom Corporation 2000-2007

*

********************************************************************************

*

* @filename fpobj_SwitchingLagConfig.c

*

* @purpose

*

* @component object handlers

*

* @comments  Refer to PortChannel-object.xml

*

* @create  6 February 2008

*

* @author   

* @end

*

********************************************************************************/

#include "fpobj_util.h"

#include "_xe_SwitchingLagConfig_obj.h"

#include "_xe_baseport_obj.h"

#include "usmdb_dot3ad_api.h"

#include "usmdb_nim_api.h"

#include "usmdb_dot1s_api.h"

#include "defaultconfig.h"

#include "xlib_protect.h"

#include "usmdb_util_api.h"

#include "dot3ad_exports.h"



L7_RC_t usmDbUnitSlotPortGet(L7_uint32 intIfNum,

                             L7_uint32 *UnitIndex,

                             L7_uint32 *SlotIndex,

                             L7_uint32 *Index);

L7_RC_t usmDbIfOperStatusGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

xLibRC_t xLibFilterSet (void *arg, xLibId_t oid, xLibU16_t type, xLibU8_t * val, xLibU16_t len);





/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagCreate

*

* @purpose Set 'LagCreate'

*

* @description [LagCreate]: When this object is set with a non-empty string,

*              a new LAG will be created if possible with the entered string

*              as its name. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagRowStatus (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagCreateValue;

  xLibU32_t keyObjLagValue;

  xLibStr256_t objLagNameValue;



	

  L7_uint32 ifIndx;

  L7_uint32 member[L7_MAX_MEMBERS_PER_LAG];

  FPOBJ_TRACE_ENTER (bufp);



  memset(objLagNameValue,0x0,sizeof(objLagNameValue));

  owa.len = sizeof(objLagCreateValue);

  

  /* retrieve object: LagCreate */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objLagCreateValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objLagCreateValue, owa.len);

  

  memset ((L7_char8 *)member, 0, sizeof(L7_uint32)*L7_MAX_MEMBERS_PER_LAG);

  

  

  if(objLagCreateValue == L7_ROW_STATUS_CREATE_AND_GO)

  {

   kwa.len = sizeof(objLagNameValue); 

   kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagName,

                          (xLibU8_t *) & objLagNameValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    memset(objLagNameValue,0x0,sizeof(objLagNameValue));

    strcpy(objLagNameValue,"default");

    

  }

   /* set the value in application */

   owa.l7rc = usmDbDot3adCreateSet (L7_UNIT_CURRENT, -1 /*PTin added*/, objLagNameValue, FD_DOT3AD_ADMIN_MODE, 

                          FD_DOT3AD_LINK_TRAP_MODE, FD_DOT3AD_MEMBER_UNIT, USMDB_FD_DOT3AD_HASH_MODE, member, &ifIndx);

   if (owa.l7rc != L7_SUCCESS)

   {

     owa.rc = XLIBRC_LAG_CREATE_FAILURE;    /* TODO: Change if required */

     FPOBJ_TRACE_EXIT (bufp, owa);

     return owa.rc;

   }

    xLibFilterSet(wap,XOBJ_SwitchingLagConfig_LagIndex,0,(xLibU8_t *) &ifIndx,sizeof(ifIndx));



  }

  else if(objLagCreateValue == L7_ROW_STATUS_DESTROY)

  {
  
   kwa1.len = sizeof(keyObjLagValue);

   kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyObjLagValue, &kwa1.len);
  
   if (kwa1.rc != XLIBRC_SUCCESS)

   {

     kwa1.rc = XLIBRC_LAG_DEL_FAILURE;

     FPOBJ_TRACE_EXIT (bufp, kwa1);

     return kwa1.rc;

    } 
    owa.l7rc = usmDbDot3adRemoveSet(L7_UNIT_CURRENT,keyObjLagValue);

    if (owa.l7rc != L7_SUCCESS)

    {

      owa.rc = XLIBRC_LAG_DEL_FAILURE;    /* TODO: Change if required */

    }

    /*
      it may take some time to delete the lag by lag application.
      so try to check if the lag is deleted or not. if not wait for a second
      and proceed. sleep of 1 second is used randomly.
       
    */

      osapiSleep(1); 
  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagDeletePort

*

* @purpose Set 'LagDeletePort'

 *@description  [LagDeletePort] Set to a non 0 value to remove a port from the

* LAG

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagDelete (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objLagDeletePortValue;



  xLibU32_t keyLagIndexValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: LagDeletePort */

  owa.len = sizeof (objLagDeletePortValue);

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLagDeletePortValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objLagDeletePortValue, owa.len);



  /* retrieve key: LagIndex */

  owa.len = sizeof (keyLagIndexValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, owa.len);



  /* set the value in application */

  if(objLagDeletePortValue)

  {

    owa.l7rc = usmDbDot3adRemoveSet(L7_UNIT_CURRENT,keyLagIndexValue);



    if (owa.l7rc != L7_SUCCESS)

    {

      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    }

  }

  

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}





/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagIndex

*

* @purpose Get 'LagIndex'

*

* @description [LagIndex]: Index for LAG interfaces 

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagIndex (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagIndexValue;

  xLibU32_t nextObjLagIndexValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LagIndex */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & objLagIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);

    owa.l7rc = usmDbDot3adIfIndexGetFirst(L7_UNIT_CURRENT,

                                          &nextObjLagIndexValue);

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLagIndexValue, owa.len);

    owa.l7rc = usmDbDot3adIfIndexGetNext(L7_UNIT_CURRENT, 

                                         objLagIndexValue,

                                         &nextObjLagIndexValue);

  }

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLagIndexValue, owa.len);



  /* return the object value: LagIndex */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLagIndexValue,

                           sizeof (nextObjLagIndexValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagName

*

* @purpose Get 'LagName'

*

* @description [LagName]: The associated name of the LAG used during creation.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagName (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objLagNameValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot3adNameGet (L7_UNIT_CURRENT, keyLagIndexValue,

                                 objLagNameValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: LagName */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLagNameValue,

                           strlen (objLagNameValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagName

*

* @purpose Set 'LagName'

*

* @description [LagName]: The associated name of the LAG used during creation.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagName (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objLagNameValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: LagName */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objLagNameValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objLagNameValue, owa.len);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot3adNameSet (L7_UNIT_CURRENT, keyLagIndexValue,

                                 objLagNameValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagLinkTrapMode

*

* @purpose Get 'LagLinkTrapMode'

*

* @description [LagLinkTrapMode]: Configures sending Link Up/Down traps when

*              the LAG interface goes Up or Down. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagLinkTrapMode (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagLinkTrapModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot3adLinkTrapGet (L7_UNIT_CURRENT, keyLagIndexValue,

                                     &objLagLinkTrapModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: LagLinkTrapMode */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLagLinkTrapModeValue,

                           sizeof (objLagLinkTrapModeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagLinkTrapMode

*

* @purpose Set 'LagLinkTrapMode'

*

* @description [LagLinkTrapMode]: Configures sending Link Up/Down traps when

*              the LAG interface goes Up or Down. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagLinkTrapMode (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagLinkTrapModeValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: LagLinkTrapMode */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objLagLinkTrapModeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objLagLinkTrapModeValue, owa.len);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot3adLinkTrapSet (L7_UNIT_CURRENT, keyLagIndexValue,

                                     objLagLinkTrapModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagAdminMode

*

* @purpose Get 'LagAdminMode'

*

* @description [LagAdminMode]: Administratively enables or disables this LAG

*              interface 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagAdminMode (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot3adAdminModeGet (L7_UNIT_CURRENT, keyLagIndexValue,

                                      &objLagAdminModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: LagAdminMode */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLagAdminModeValue,

                           sizeof (objLagAdminModeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagAdminMode

*

* @purpose Set 'LagAdminMode'

*

* @description [LagAdminMode]: Administratively enables or disables this LAG

*              interface 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagAdminMode (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagAdminModeValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: LagAdminMode */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLagAdminModeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objLagAdminModeValue, owa.len);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot3adAdminModeSet ( L7_UNIT_CURRENT, keyLagIndexValue,

                                       objLagAdminModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* Application handles admin mode setting asynchronously. The following delay 

   * is required, because the set handler for the objects that gets called

   * later in Port Channel Configuration Page, depend on the successful setting 

   * of admin mode. 

   */ 

  osapiSleepUSec(1);

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagStpMode

*

* @purpose Get 'LagStpMode'

*

* @description [LagStpMode]: If Dot1d is enabled, the valid values are: dot1d(1),

*              fast(2), and off(3) If Dot1s is enabled, the valid

*              values are: off(3) and dot1s(4) 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagStpMode (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagStpModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot1sPortStateGet (L7_UNIT_CURRENT, keyLagIndexValue,

                                     &objLagStpModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: LagStpMode */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLagStpModeValue,

                           sizeof (objLagStpModeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagStpMode

*

* @purpose Set 'LagStpMode'

*

* @description [LagStpMode]: If Dot1d is enabled, the valid values are: dot1d(1),

*              fast(2), and off(3) If Dot1s is enabled, the valid

*              values are: off(3) and dot1s(4) 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagStpMode (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagStpModeValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: LagStpMode */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLagStpModeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objLagStpModeValue, owa.len);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot1sPortStateSet (L7_UNIT_CURRENT, keyLagIndexValue,

                                     objLagStpModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagAddPort

*

* @purpose Set 'LagAddPort'

*

* @description [LagAddPort]: Set to a non 0 value to add a port to the LAG

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagAddPort (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagAddPortValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: LagAddPort */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLagAddPortValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objLagAddPortValue, owa.len);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot3adMemberAddSet (L7_UNIT_CURRENT, keyLagIndexValue,

                                      objLagAddPortValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagDeletePort

*

* @purpose Set 'LagDeletePort'

*

* @description [LagDeletePort]: Set to a non 0 value to remove a port from

*              the LAG 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagDeletePort (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagDeletePortValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: LagDeletePort */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objLagDeletePortValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objLagDeletePortValue, owa.len);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot3adMemberDeleteSet (L7_UNIT_CURRENT, keyLagIndexValue,

                                         objLagDeletePortValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagType

*

* @purpose Get 'LagType'

*

* @description [LagType]: Agent LAG Type.static(1) - This LAG is staticly

*              maintained. dynamic(2) - This LAG is dynamicly maintained.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagType (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagTypeValue=0;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot3adIsStaticLag (L7_UNIT_CURRENT, keyLagIndexValue,

                                     &objLagTypeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: LagType */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLagTypeValue,

                           sizeof (objLagTypeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagStaticCapability

*

* @purpose Get 'LagStaticCapability'

*

* @description [LagStaticCapability]: Agent LAG Static Capability enable(1)

*              - Static capability is enabled for this LAG interface .disable(2)

*              - Static capability is disabled for this LAG interface

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagStaticCapability (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagStaticCapabilityValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);


  osapiSleepUSec(1);
  /* get the value from application */

  owa.l7rc = usmDbDot3adIsStaticLag (L7_UNIT_CURRENT, keyLagIndexValue,

                                     &objLagStaticCapabilityValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: LagStaticCapability */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLagStaticCapabilityValue,

                           sizeof (objLagStaticCapabilityValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagStaticCapability

*

* @purpose Set 'LagStaticCapability'

*

* @description [LagStaticCapability]: Agent LAG Static Capability enable(1)

*              - Static capability is enabled for this LAG interface .disable(2)

*              - Static capability is disabled for this LAG interface

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagStaticCapability (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagStaticCapabilityValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: LagStaticCapability */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objLagStaticCapabilityValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objLagStaticCapabilityValue, owa.len);



  /* retrieve key: LagIndex */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot3adLagStaticModeSet (L7_UNIT_CURRENT, keyLagIndexValue,

                                          objLagStaticCapabilityValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagMemberPortList

*

* @purpose Get 'LagMemberPortList'

 *@description  [LagMemberPortList] This object gets member ports for a

* purticular LAG index

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagMemberPortList (void *wap, void *bufp)

{



  fpObjWa_t kwaLagIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue, flag, i;



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objLagMemberPortListValue, stat;

  L7_uint32 count = L7_MAX_MEMBERS_PER_LAG;

  L7_uint32 portChannelMembers[L7_MAX_MEMBERS_PER_LAG];



  FPOBJ_TRACE_ENTER (bufp);

  flag = 1;



  /* retrieve key: LagIndex */

  kwaLagIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                                  (xLibU8_t *) & keyLagIndexValue, &kwaLagIndex.len);

  if (kwaLagIndex.rc != XLIBRC_SUCCESS)

  {

    kwaLagIndex.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaLagIndex);

    return kwaLagIndex.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwaLagIndex.len);



  memset(objLagMemberPortListValue, 0x0, sizeof(objLagMemberPortListValue));



  /* get the value from application */

  owa.l7rc = usmDbDot3adMemberListGet(L7_UNIT_CURRENT, keyLagIndexValue, &count, portChannelMembers);



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

   

  if(count != 0)

  {

     for (i = 0; i < count; i++)

     {

        memset(stat, 0x0, sizeof(stat));

        osapiSnprintf(stat, sizeof(stat), "%d", portChannelMembers[i]);

        if(flag == 1)

        {

          OSAPI_STRNCAT(objLagMemberPortListValue, stat);

          flag = 0;

        }

        else

        {

          OSAPI_STRNCAT(objLagMemberPortListValue, ",");

          OSAPI_STRNCAT(objLagMemberPortListValue, stat);

        }

      }

  }

 

  /* return the object value: LagMemberPortList */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLagMemberPortListValue,

                           strlen (objLagMemberPortListValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}

/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagStatus

*

* @purpose Set 'LagStatus'

 *@description  [LagStatus] LAG Status

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagStatus (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagStatusValue;



  fpObjWa_t kwaLagIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: LagStatus */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLagStatusValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objLagStatusValue, owa.len);



  /* retrieve key: LagIndex */

  kwaLagIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                                  (xLibU8_t *) & keyLagIndexValue, &kwaLagIndex.len);

  if (kwaLagIndex.rc != XLIBRC_SUCCESS)

  {

    kwaLagIndex.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaLagIndex);

    return kwaLagIndex.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwaLagIndex.len);



  owa.l7rc = L7_SUCCESS;

  if (objLagStatusValue == L7_ROW_STATUS_DESTROY)

  {

    owa.l7rc = usmDbDot3adRemoveSet(L7_UNIT_CURRENT, keyLagIndexValue);

  }



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagStatus

*

* @purpose Get 'LagStatus'

 *@description  [LagStatus] LAG Status

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagStatus (void *wap, void *bufp)

{



  fpObjWa_t kwaLagIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagStatusValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LagIndex */

  kwaLagIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                                  (xLibU8_t *) & keyLagIndexValue, &kwaLagIndex.len);

  if (kwaLagIndex.rc != XLIBRC_SUCCESS)

  {

    kwaLagIndex.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaLagIndex);

    return kwaLagIndex.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwaLagIndex.len);



  /* get the value from application */

  owa.l7rc = L7_SUCCESS;



  objLagStatusValue = L7_ROW_STATUS_ACTIVE;



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: LagStatus */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLagStatusValue, sizeof (objLagStatusValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}

/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagSummaryHashOption

*

* @purpose Get 'LagSummaryHashOption'

 *@description  [LagSummaryHashOption] LAG Hashing Option.   

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagSummaryHashOption (void *wap, void *bufp)

{



  fpObjWa_t kwaLagIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagSummaryHashOptionValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LagIndex */

  kwaLagIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                                  (xLibU8_t *) & keyLagIndexValue, &kwaLagIndex.len);

  if (kwaLagIndex.rc != XLIBRC_SUCCESS)

  {

    kwaLagIndex.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaLagIndex);

    return kwaLagIndex.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwaLagIndex.len);



  /* get the value from application */

  owa.l7rc = usmDbDot3adLagHashModeGet (L7_UNIT_CURRENT, keyLagIndexValue, &objLagSummaryHashOptionValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODusmDbGetUnknownO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: LagSummaryHashOption */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLagSummaryHashOptionValue,

                           sizeof (objLagSummaryHashOptionValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagSummaryHashOption

*

* @purpose Set 'LagSummaryHashOption'

 *@description  [LagSummaryHashOption] LAG Hashing Option.   

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagSummaryHashOption (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagSummaryHashOptionValue;



  fpObjWa_t kwaLagIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLagIndexValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: LagSummaryHashOption */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLagSummaryHashOptionValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objLagSummaryHashOptionValue, owa.len);



  /* retrieve key: LagIndex */

  kwaLagIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                                  (xLibU8_t *) & keyLagIndexValue, &kwaLagIndex.len);

  if (kwaLagIndex.rc != XLIBRC_SUCCESS)

  {

    kwaLagIndex.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaLagIndex);

    return kwaLagIndex.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwaLagIndex.len);



  /* set the value in application */

  owa.l7rc = usmDbDot3adLagHashModeSet (L7_UNIT_CURRENT, keyLagIndexValue, objLagSummaryHashOptionValue);



  if (owa.l7rc != L7_SUCCESS)

  {
    if (owa.l7rc == L7_DEPENDENCY_NOT_MET)
    {
      owa.rc = XLIBRC_LAG_HASH_MODE_APPLY_DUE_TO_ADMIN_DOWN_FAILURE;
    }
    else
    {
      owa.rc = XLIBRC_LAG_HASH_MODE_APPLY_FAILURE;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}

/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagGlobalHashOption

*

* @purpose Get 'LagGlobalHashOption'

 *@description  [LagGlobalHashOption] Global LAG Hashing Option.

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagGlobalHashOption (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagGlobalHashOptionValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  /*owa.l7rc = usmDbDot3adSystemHashModeGet (L7_UNIT_CURRENT, &objLagGlobalHashOptionValue);*/

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODusmDbGetUnknownO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: LagSummaryHashOption */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLagGlobalHashOptionValue,

                           sizeof (objLagGlobalHashOptionValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}

/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagGlobalHashOption

*

* @purpose Set 'LagGlobalHashOption'

 *@description  [LagGlobalHashOption] Gloabl LAG Hashing Option.

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagGlobalHashOption (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLagGlobalHashOptionValue;



  /* retrieve object: LagSummaryHashOption */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLagGlobalHashOptionValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objLagGlobalHashOptionValue, owa.len);



  /* set the value in application */

  /*owa.l7rc = usmDbDot3adSystemHashModeSet (L7_UNIT_CURRENT, objLagGlobalHashOptionValue);*/



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagParticipation

*

* @purpose Get 'LagParticipation'

 *@description  [LagParticipation] ToDO: Add Help

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagParticipation (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objLagParticipationValue;



  xLibU32_t keyLagIndexValue;

  xLibU32_t keyInterfaceValue,whichLag;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LagIndex */

  owa.len = sizeof (keyLagIndexValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, owa.len);



  /* retrieve key: Interface */

  owa.len = sizeof (keyInterfaceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT,keyInterfaceValue,&whichLag);

  if(owa.l7rc == L7_SUCCESS && whichLag == keyLagIndexValue)

  {

    objLagParticipationValue = L7_TRUE;

  }

  else

  {

    objLagParticipationValue = L7_FALSE;

  }

  FPOBJ_TRACE_VALUE (bufp, &objLagParticipationValue, sizeof (objLagParticipationValue));



  /* return the object value: LagParticipation */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLagParticipationValue,

                           sizeof (objLagParticipationValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagParticipation

*

* @purpose Set 'LagParticipation'

 *@description  [LagParticipation] ToDO: Add Help

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagParticipation (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objLagParticipationValue;



  xLibU32_t keyLagIndexValue;

  xLibU32_t keyInterfaceValue,whichLag;

  

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: LagParticipation */

  owa.len = sizeof (objLagParticipationValue);

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLagParticipationValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* retrieve key: LagIndex */

  owa.len = sizeof (keyLagIndexValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, owa.len);



  /* retrieve key: Interface */

  owa.len = sizeof (keyInterfaceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);



  owa.l7rc = usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT,keyInterfaceValue,&whichLag);



  if(owa.l7rc == L7_SUCCESS) /* interface is a member of a lag */

  {

    if(objLagParticipationValue == L7_TRUE)

    {

      if(whichLag != keyLagIndexValue) /* is a member of some other lag */

        owa.rc = XLIBRC_FAILURE;

    }

    else

    {

      /* Remove the interface from the LAG, if the interface is member of this lag only */

      if(whichLag == keyLagIndexValue)

      {

        owa.l7rc = usmDbDot3adMemberDeleteSet(L7_UNIT_CURRENT,keyLagIndexValue,keyInterfaceValue);

        if(owa.l7rc != L7_SUCCESS)

          owa.rc = XLIBRC_FAILURE;

      }

    }

  }

  else /* interface is not a member of lag */

  {

    if(objLagParticipationValue == L7_TRUE)

    {

       /* Add the interface to the LAG */

       owa.l7rc = usmDbDot3adMemberAddSet(L7_UNIT_CURRENT,keyLagIndexValue,keyInterfaceValue);

       if(owa.l7rc != L7_SUCCESS)

         owa.rc = XLIBRC_FAILURE;

    }

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}

/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_MemberShipConflicts

*

* @purpose Get 'MemberShipConflicts'

 *@description  [MemberShipConflicts] ToDO: Add Help

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_MemberShipConflicts (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibStr256_t objMemberShipConflictsValue;

  xLibStr256_t tempStr;

  

  xLibU32_t keyLagIndexValue;

  xLibU32_t keyInterfaceValue;

  L7_uint32 UnitIndex,SlotIndex,Index,whichLag=0;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LagIndex */

  owa.len = sizeof (keyLagIndexValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, owa.len);



  /* retrieve key: Interface */

  owa.len = sizeof (keyInterfaceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);



  /* get the value from application */

 

 owa.l7rc = usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT,keyInterfaceValue,&whichLag);

 memset(tempStr,0x0,sizeof(tempStr));

 strcpy(tempStr,"  ");

 

if(owa.l7rc == L7_SUCCESS  )

{

  if(whichLag != keyLagIndexValue)

  	{

  	  (void)usmDbUnitSlotPortGet(whichLag,&UnitIndex,&SlotIndex,&Index);

	   memset(tempStr,0x0,sizeof(tempStr));

#ifdef L7_STACKING_PACKAGE

    osapiSnprintf(tempStr, sizeof(tempStr), "Member of %d/%d/%d",UnitIndex,SlotIndex,Index);

#else

    osapiSnprintf(tempStr, sizeof(tempStr), "Member of %d/%d",SlotIndex,Index);

#endif

	}

}

memset(objMemberShipConflictsValue,0x0,sizeof(objMemberShipConflictsValue));

memcpy(objMemberShipConflictsValue,tempStr,strlen(tempStr));

  /* return the object value: MemberShipConflicts */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMemberShipConflictsValue,

                           strlen (objMemberShipConflictsValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LagParticipationAll

*

* @purpose Get 'LagParticipationAll'

 *@description  [LagParticipationAll] ToDO: Add Help

* @notes   This object should not use key "LagIndex"

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LagParticipationAll (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objLagParticipationValue;



  xLibU32_t keyInterfaceValue,whichLag;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  owa.len = sizeof (keyInterfaceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT,keyInterfaceValue,&whichLag);

  if(owa.l7rc == L7_SUCCESS)

  {

    objLagParticipationValue = L7_TRUE;

  }

  else

  {

    objLagParticipationValue = L7_FALSE;

  }

  FPOBJ_TRACE_VALUE (bufp, &objLagParticipationValue, sizeof (objLagParticipationValue));



  /* return the object value: LagParticipation */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLagParticipationValue,

                           sizeof (objLagParticipationValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjSet_SwitchingLagConfig_LagParticipationAll

*

* @purpose Set 'LagParticipationAll'

 *@description  [LagParticipationAll] ToDO: Add Help

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLagConfig_LagParticipationAll (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objLagParticipationValue;



  xLibU32_t keyLagIndexValue;

  xLibU32_t keyInterfaceValue,whichLag;

  

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: LagParticipation */

  owa.len = sizeof (objLagParticipationValue);

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLagParticipationValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* retrieve key: LagIndex */

  owa.len = sizeof (keyLagIndexValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, owa.len);



  /* retrieve key: Interface */

  owa.len = sizeof (keyInterfaceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);



  owa.l7rc = usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT,keyInterfaceValue,&whichLag);



  if(owa.l7rc == L7_SUCCESS) /* interface is a member of a lag */

  {

    if(objLagParticipationValue == L7_TRUE)

    {

      if(whichLag != keyLagIndexValue) /* is a member of some other lag */

        owa.rc = XLIBRC_FAILURE;

    }

    else

    {

      /* Remove the interface from the LAG, if the interface is member of this lag only */

      if(whichLag == keyLagIndexValue)

      {

        owa.l7rc = usmDbDot3adMemberDeleteSet(L7_UNIT_CURRENT,keyLagIndexValue,keyInterfaceValue);

        if(owa.l7rc != L7_SUCCESS)

          owa.rc = XLIBRC_FAILURE;

      }

    }

  }

  else /* interface is not a member of lag */

  {

    if(objLagParticipationValue == L7_TRUE)

    {

       /* Add the interface to the LAG */

       owa.l7rc = usmDbDot3adMemberAddSet(L7_UNIT_CURRENT,keyLagIndexValue,keyInterfaceValue);

       if(owa.l7rc != L7_SUCCESS)

         owa.rc = XLIBRC_FAILURE;

    }

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}

/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_MemberShipConflictsAll

*

* @purpose Get 'MemberShipConflictsAll'

 *@description  [MemberShipConflictsAll] ToDO: Add Help

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_MemberShipConflictsAll (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibStr256_t objMemberShipConflictsValue;

  xLibStr256_t tempStr;



  xLibU32_t keyInterfaceValue;

  L7_uint32 UnitIndex,SlotIndex,Index,whichLag=0;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  owa.len = sizeof (keyInterfaceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);



  /* get the value from application */

 

  owa.l7rc = usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT,keyInterfaceValue,&whichLag);

  memset(tempStr,0x0,sizeof(tempStr));

  strcpy(tempStr,"  ");



  if(owa.l7rc == L7_SUCCESS  )

  {

    (void)usmDbUnitSlotPortGet(whichLag,&UnitIndex,&SlotIndex,&Index);

    memset(tempStr,0x0,sizeof(tempStr));

#ifdef L7_STACKING_PACKAGE

    osapiSnprintf(tempStr, sizeof(tempStr), "Member of %d/%d/%d",UnitIndex,SlotIndex,Index);

#else

    osapiSnprintf(tempStr, sizeof(tempStr), "Member of %d/%d",SlotIndex,Index);

#endif

  }

  memset(objMemberShipConflictsValue,0x0,sizeof(objMemberShipConflictsValue));

  memcpy(objMemberShipConflictsValue,tempStr,strlen(tempStr));

  /* return the object value: MemberShipConflicts */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMemberShipConflictsValue,

      strlen (objMemberShipConflictsValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}





/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_Interface

*

* @purpose Get 'Interface'

 *@description  [Interface] Port of the system

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_Interface (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objInterfaceValue;

  xLibU32_t nextObjInterfaceValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  owa.len = sizeof (objInterfaceValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_Interface,

                          (xLibU8_t *) & objInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    owa.l7rc = usmDbValidIntIfNumFirstGet(&nextObjInterfaceValue);

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);

    owa.l7rc = usmDbGetNextPhysicalIntIfNumber (objInterfaceValue, &nextObjInterfaceValue);

  }



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);



  /* return the object value: Interface */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,

                           sizeof (nextObjInterfaceValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjGet_SwitchingLagConfig_LinkStatus

*

* @purpose Get 'LinkStatus'

 *@description  [LinkStatus] LAG Status

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_LinkStatus (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t objLinkStatusValue;



  xLibU32_t keyLagIndexValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LagIndex */

  owa.len = sizeof (keyLagIndexValue);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,

                          (xLibU8_t *) & keyLagIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, owa.len);



  /* get the value from application */

  owa.l7rc = usmDbIfOperStatusGet(L7_UNIT_CURRENT, keyLagIndexValue, &objLinkStatusValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_VALUE (bufp, &objLinkStatusValue, sizeof (objLinkStatusValue));



  /* return the object value: LinkStatus */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLinkStatusValue, sizeof (objLinkStatusValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}

/****************************************************************
 * @brief        This function returns the number of Lags created
 *
 * @param wap   context information
 * @param bufp  input/output data buffer
 *
 * @return      returns SUCCESS always
 ***************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_NumLags (void *wap, void *bufp)
{
  L7_uint32 numLags = 0;

  usmDbDot3adLagCountGet (&numLags);

  return xLibBufDataSet (bufp, (xLibU8_t *) & numLags, sizeof (numLags));
}

/****************************************************************
 * @brief        This function returns the Maximum ports per LAG
 *
 * @param wap   context information
 * @param bufp  input/output data buffer
 *
 * @return      returns SUCCESS always
 ***************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_MaxPortsPerLAG (void *wap, void *bufp)
{
  L7_uint32 maxPorts = L7_MAX_MEMBERS_PER_LAG;
  return xLibBufDataSet (bufp, (xLibU8_t *) & maxPorts, sizeof (maxPorts));
}

/****************************************************************
 * @brief        This function returns the Maximum Lags
 *
 * @param wap   context information
 * @param bufp  input/output data buffer
 *
 * @return      returns SUCCESS always
 ***************************************************************/

xLibRC_t fpObjGet_SwitchingLagConfig_MaxLAGS (void *wap, void *bufp)
{
  L7_uint32 numLags = L7_MAX_NUM_LAG_INTF;
  return xLibBufDataSet (bufp, (xLibU8_t *) & numLags, sizeof (numLags));
}



xLibRC_t fpObjGet_SwitchingLagConfig_ActivePorts(void *wap, void *bufp)
{
   fpObjWa_t kwaLagIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
   xLibU32_t keyLagIndexValue;
   char lagMemberList[L7_MAX_MEMBERS_PER_LAG * 5];
   L7_uint32 maxMembers = L7_MAX_MEMBERS_PER_LAG;
   L7_uint32 lagMembers[L7_MAX_MEMBERS_PER_LAG];
   char pTemp[10];
    int i = 0;


   /* retrieve key: LagIndex */

  kwaLagIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_LagIndex,
                                  (xLibU8_t *) & keyLagIndexValue, &kwaLagIndex.len);

  if (kwaLagIndex.rc != XLIBRC_SUCCESS)

  {

    kwaLagIndex.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaLagIndex);

    return kwaLagIndex.rc;

  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLagIndexValue, kwaLagIndex.len);
  memset (lagMemberList, 0, sizeof (lagMemberList));  

  if (usmDbDot3adActiveMemberListGet (1,  keyLagIndexValue, &maxMembers, &lagMembers[0]) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  /* convert this int array to a read-able string */

  for (i = 0; i < maxMembers; i++)
  {
    memset(pTemp, 0, sizeof (pTemp));
    if (i < maxMembers-1)
     osapiSnprintf (pTemp,sizeof(pTemp), "%d,", lagMembers[i]);
    else
     osapiSnprintf (pTemp, sizeof(pTemp), "%d", lagMembers[i]);
    OSAPI_STRNCAT (lagMemberList, pTemp);
  }

  return xLibBufDataSet (bufp, (xLibU8_t *) lagMemberList, sizeof (lagMemberList));
}

  
/*******************************************************************************
* @function fpObjGet_SwitchingLagConfig_IntfLagMember
*
* @purpose Get 'IntfLagMember'
 *@description  [IntfLagMember] Returns LAG Member to which this interface is
* part of
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLagConfig_IntfLagMember (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objIntfLagMemberValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT, keyInterfaceValue, &objIntfLagMemberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objIntfLagMemberValue, sizeof (objIntfLagMemberValue));

  /* return the object value: IntfLagMember */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfLagMemberValue,
                           sizeof (objIntfLagMemberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_SwitchingLagConfig_IntfLagMember
*
* @purpose Set 'IntfLagMember'
 *@description  [IntfLagMember] Returns LAG Member to which this interface is
* part of
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLagConfig_IntfLagMember (void *wap, void *bufp)
{

  fpObjWa_t  owa = FPOBJ_INIT_WA2 ();
  xLibU32_t  objIntfLagMemberValue;
  xLibU32_t  objLACPModeValue;
  xLibU32_t  objLagStaticCapabilityValue;
  xLibU32_t  tempVal;
  xLibBool_t addFlag = XLIB_FALSE;
  xLibBool_t delFlag = XLIB_FALSE;

  xLibU32_t keyInterfaceValue;
  L7_RC_t rc;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IntfLagMember */
  owa.len = sizeof (objIntfLagMemberValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIntfLagMemberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIntfLagMemberValue, owa.len);

  /* retrieve key: LACPMode */
  owa.len = sizeof (objLACPModeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseport_LACPMode, (xLibU8_t *) & objLACPModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLACPModeValue, owa.len);

  if (objLACPModeValue == L7_ENABLE)
  {
    objLagStaticCapabilityValue = L7_FALSE;
  } 
  else
  {
    objLagStaticCapabilityValue = L7_TRUE;
  }

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLagConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  if (usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT, keyInterfaceValue, &tempVal) == L7_SUCCESS) /* Modify only */
  {
    delFlag = XLIB_TRUE;
  }

  if (objIntfLagMemberValue != 0) /* Add only */
  {
    addFlag = XLIB_TRUE;
  }

  if (delFlag == XLIB_TRUE)
  {
    rc = usmDbDot3adMemberDeleteSet(L7_UNIT_CURRENT, tempVal, keyInterfaceValue);  
    osapiSleepUSec(1);
  }

  if (addFlag == XLIB_TRUE)
  {
    owa.l7rc = usmDbDot3adLagStaticModeSet (L7_UNIT_CURRENT, objIntfLagMemberValue, objLagStaticCapabilityValue);

    if (owa.l7rc == L7_SUCCESS)
    {
      rc = usmDbDot3adMemberAddSet(L7_UNIT_CURRENT, objIntfLagMemberValue, keyInterfaceValue);
    }
    else if (owa.l7rc == L7_TABLE_IS_FULL)
    {
      owa.rc = XLIBRC_TABLE_IS_FULL;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
