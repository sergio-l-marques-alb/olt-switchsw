/*******************************************************************************

*

* (C) Copyright Broadcom Corporation 2000-2007

*

********************************************************************************

*

* @filename fpobj_SwitchingLLDPRemoteSystemData.c

*

* @purpose

*

* @component object handlers

*

* @comments  Refer to LLDP-object.xml

*

* @create  6 February 2008

*

* @author Radha K 

* @end

*

********************************************************************************/

#include "fpobj_util.h"

#include "_xe_SwitchingLLDPRemoteSystemData_obj.h"

#include "usmdb_lldp_api.h"

#include "usmdb_util_api.h"

#include "osapi_support.h"

#include "l3_commdefs.h"





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_TimeMark

*

* @purpose Get 'TimeMark'

 *@description  [TimeMark] A TimeFilter for this entry. See the TimeFilter

* textual convention in IETF RFC 2021 and

* http://www.ietf.org/IESG/Implementations/RFC2021-Implementation.txt to see how TimeFilter works.   

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_TimeMark (void *wap, void *bufp)

{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTimeMarkValue;
  xLibU32_t objTimeMarkValueNext;
  xLibU32_t KeyRemIndexValueNext;
  xLibU32_t KeyRemIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,
                          (xLibU8_t *) & keyLocalPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa.len);

  /* retrieve key: RemIndexValue */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,
                           (xLibU8_t *) &KeyRemIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; 
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: TimeMark */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,
                          (xLibU8_t *) & objTimeMarkValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    /* First time to get the time mark */ 
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objTimeMarkValue, owa.len);
    KeyRemIndexValueNext = 0;
    objTimeMarkValueNext = 0;

    do
    {
      owa.l7rc = usmDbLldpRemEntryGetNext (keyLocalPortNumValue, &KeyRemIndexValueNext, &objTimeMarkValueNext);
      if ((owa.l7rc == L7_SUCCESS) && (KeyRemIndexValue == KeyRemIndexValueNext))
      {
        break;
      }
    } while (owa.l7rc == L7_SUCCESS); 
  } 
  else 
  { 
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &objTimeMarkValueNext, owa.len);

  /* return the object value: TimeMark */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objTimeMarkValueNext,
                           sizeof (objTimeMarkValueNext));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;
}



/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_LocalPortNum

*

* @purpose Get 'LocalPortNum'

 *@description  [LocalPortNum] The index value used to identify the port

* component (contained in the local chassis with the LLDP agent)

* associated with this entry. The LocalPortNum identifies the port on which

* the remote system information is received.    

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_LocalPortNum (void *wap, void *bufp)

{



  xLibU32_t objLocalPortNumValue;

  xLibU32_t nextObjLocalPortNumValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LocalPortNum */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                          (xLibU8_t *) & objLocalPortNumValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    owa.l7rc = usmDbLldpRemoteIntfFirstGet ( &nextObjLocalPortNumValue);

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLocalPortNumValue, owa.len);

    owa.l7rc = usmDbLldpRemoteIntfNextGet (objLocalPortNumValue,

                                           &nextObjLocalPortNumValue);

  }

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLocalPortNumValue, owa.len);

  /* return the object value: LocalPortNum */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLocalPortNumValue,

                           sizeof (nextObjLocalPortNumValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}
/*******************************************************************************
* @function fpObjGet_SwitchingLLDPRemoteSystemData_UnitIndex
*
* @purpose Get 'UnitIndex'
 *@description  [UnitIndex] Index for Supported Unit Types
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_UnitIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objUnitIndexValue;
  xLibU32_t nextObjUnitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UnitIndex */
  owa.len = sizeof (objUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_UnitIndex,
                          (xLibU8_t *) & objUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
   objUnitIndexValue = 0;
   nextObjUnitIndexValue = 0;
  }

  owa.l7rc = usmDbUnitMgrStackMemberGetNext (objUnitIndexValue,
                                             &nextObjUnitIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjUnitIndexValue, owa.len);

  /* return the object value: UnitIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjUnitIndexValue,
                           sizeof (nextObjUnitIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_LocalPortNumPerUnit

*

* @purpose Get 'LocalPortNumPerUnit'

 *@description  [LocalPortNumPerUnit] The index value used to identify the port

* component (contained in the local chassis with the LLDP agent)

* associated with this entry. The LocalPortNum identifies the port on which

* the remote system information is received.

* @notes

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_LocalPortNumPerUnit (void *wap, void *bufp)

{



  xLibU32_t objLocalPortNumValue;
  xLibU32_t objUnitIndexValue;
  L7_uint32 u, s, p;

  xLibU32_t nextObjLocalPortNumValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: UnitIndex */
  owa.len = sizeof (objUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_UnitIndex,
                          (xLibU8_t *) & objUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }
  /* retrieve key: LocalPortNum */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNumPerUnit,

                          (xLibU8_t *) & objLocalPortNumValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    owa.l7rc = usmDbLldpRemoteIntfFirstGet ( &nextObjLocalPortNumValue);
    while((owa.l7rc == L7_SUCCESS) &&((owa.l7rc=usmDbUnitSlotPortGet(nextObjLocalPortNumValue, &u, &s, &p))== L7_SUCCESS))
    {
       if (u != objUnitIndexValue)
      {
        owa.l7rc = usmDbLldpRemoteIntfNextGet (nextObjLocalPortNumValue,

                                           &nextObjLocalPortNumValue);
      }
      else
      {
        break;
      }

    }

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLocalPortNumValue, owa.len);

    owa.l7rc = usmDbLldpRemoteIntfNextGet (objLocalPortNumValue,

                                           &nextObjLocalPortNumValue);
    if((owa.l7rc == L7_SUCCESS) &&((owa.l7rc=usmDbUnitSlotPortGet(nextObjLocalPortNumValue, &u, &s, &p))== L7_SUCCESS))
    {
       if (u != objUnitIndexValue)
      {
        owa.l7rc = L7_FAILURE;
      }

    }


  }
  

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLocalPortNumValue, owa.len);

  /* return the object value: LocalPortNum */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLocalPortNumValue,

                           sizeof (nextObjLocalPortNumValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}


/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_RemIndex

*

* @purpose Get 'RemIndex'

 *@description  [RemIndex] This object represents an arbitrary local integer

* value used by this agent to identify a particular connection

* instance, unique only for the indicated remote system. An agent is

* encouraged to assign monotonically increasing index values to new

* entries, s   

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_RemIndex (void *wap, void *bufp)

{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRemIndexValue;
  xLibU32_t TimeMarkValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,
                          (xLibU8_t *) & keyLocalPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa.len);

  /* retrieve key: RemIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,
                          (xLibU8_t *) &objRemIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objRemIndexValue = 0;
    TimeMarkValue    = 0; 
    owa.l7rc = usmDbLldpRemEntryGetNext (keyLocalPortNumValue,
                                       &objRemIndexValue, &TimeMarkValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRemIndexValue, owa.len);
    owa.l7rc = usmDbLldpRemEntryGetNext (keyLocalPortNumValue,
                                       &objRemIndexValue, &TimeMarkValue);
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &objRemIndexValue, owa.len);

  /* return the object value: RemIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRemIndexValue,
                           sizeof (objRemIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_ManAddrSubtype

*

* @purpose Get 'ManAddrSubtype'

*

* @description [ManAddrSubtype]: The type of management address identifier

*              encoding used in the associated 'lldpRemManagmentAddr' object.

*              

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_ManAddrSubtype (void *wap,

                                                                void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objManAddrSubtypeValue;

  xLibU32_t nextObjManAddrSubtypeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: ManAddrSubtype */

  owa.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_ManAddrSubtype,

                   (xLibU8_t *) & objManAddrSubtypeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objManAddrSubtypeValue, owa.len);

  }

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjManAddrSubtypeValue, owa.len);



  /* return the object value: ManAddrSubtype */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjManAddrSubtypeValue,

                           sizeof (objManAddrSubtypeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_ManAddr

*

* @purpose Get 'ManAddr'

*

* @description [ManAddr]: The string value used to identify the management

*              address component associated with the remote system. The purpose

*              of this address is to contact the management entity.

*              

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_ManAddr (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objManAddrValue;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  

  /* RAMA: made the below two variables static

     HACK

     HACK

     HACK

     HACK

     WHEN THE LENGTH IS ZERO THE INITIAL ADDRESS IS FETCHED ALONG WITH THE FAMILY AND LENGTH

     WE NEED TO PUSH THE FAMILY AND LENGTH BETWEEN THE REQUESTS IN ORDER TO FECTCH THE NEXT ENTRY.

     HACK

     HACK

     HACK

     HACK

     Inital thoughts on the clean fix are - add xLibFilterContextCreate function by passing the 

     length os space required and in each subsequent call call xLibFilterContextGet

     this allocated contexts should be cleared by xLib once the serve is completed

     need more thoughts ????????????????

   */

  static lldpIANAAddrFamilyNumber_t family;

  static xLibU8_t length;

  FPOBJ_TRACE_ENTER (bufp);



   /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* retrieve key: ManAddr */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_ManAddr,

                          (xLibU8_t *) objManAddrValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);

    length = 0; /* length == 0 returns first management address */

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, objManAddrValue, owa.len);

  }



  owa.l7rc = usmDbLldpRemManAddrEntryNextGet (keyLocalPortNumValue,

                                                keyRemIndexValue,

                                                keyTimeMarkValue,

                                                &family,

                                                objManAddrValue,

                                                (xLibU8_t *)&length);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_NEW_KEY (bufp, objManAddrValue, owa.len);



  /* return the object value: ManAddr */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objManAddrValue,

                           strlen (objManAddrValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_ChassisIdSubtype

*

* @purpose Get 'ChassisIdSubtype'

*

* @description [ChassisIdSubtype]: The type of encoding used to identify the

*              chassis associated with the remote system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_ChassisIdSubtype (void *wap,

                                                                  void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibStr256_t objChassisIdSubtypeValue;

  xLibU32_t subtype; 

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* get the value from application */

  owa.l7rc = usmDbLldpRemChassisIdSubtypeGet (keyLocalPortNumValue,

                                              keyRemIndexValue,

                                              keyTimeMarkValue,

                                              &subtype);

  

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  osapiStrncpySafe(objChassisIdSubtypeValue, (L7_char8*)usmDbLldpChassisIdSubtypeString(subtype),sizeof(objChassisIdSubtypeValue));

  /* return the object value: ChassisIdSubtype */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objChassisIdSubtypeValue,

                           strlen (objChassisIdSubtypeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_ChassisId

*

* @purpose Get 'ChassisId'

*

* @description [ChassisId]: The string value used to identify the chassis

*              component associated with the remote system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_ChassisId (void *wap,

                                                           void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objChassisIdValue={0};
  xLibStr256_t chassisId={0};
  xLibU16_t length;
  lldpChassisIdSubtype_t subtype;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  { 
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);
  /* get the value from application */
  owa.l7rc = usmDbLldpRemChassisIdGet (keyLocalPortNumValue,
                                       keyRemIndexValue,
                                       keyTimeMarkValue,
                                       chassisId,
                                       &length);

  if (owa.l7rc != L7_SUCCESS)
  { 
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbLldpRemChassisIdSubtypeGet (keyLocalPortNumValue,
                                              keyRemIndexValue,
                                              keyTimeMarkValue,
                                              &subtype);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  usmDbLldpChassisIdFormat(objChassisIdValue, sizeof(objChassisIdValue), subtype, chassisId, length);

  /* return the object value: ChassisId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objChassisIdValue,
                           strlen (objChassisIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_PortIdSubtype

*

* @purpose Get 'PortIdSubtype'

*

* @description [PortIdSubtype]: The type of port identifier encoding used

*              in the associated 'PortId' object. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_PortIdSubtype (void *wap,

                                                               void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibStr256_t objPortIdSubtypeValue;

  xLibU32_t portIdType;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* get the value from application */

  owa.l7rc = usmDbLldpRemPortIdSubtypeGet (keyLocalPortNumValue,

                                           keyRemIndexValue,

                                           keyTimeMarkValue,

                                           &portIdType);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  osapiStrncpySafe(objPortIdSubtypeValue, (L7_char8*)usmDbLldpPortIdSubtypeString(portIdType),sizeof(objPortIdSubtypeValue));



  /* return the object value: PortIdSubtype */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortIdSubtypeValue,

                           sizeof (objPortIdSubtypeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_PortId

*

* @purpose Get 'PortId'

*

* @description [PortId]: The string value used to identify the port component

*              associated with the remote system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_PortId (void *wap, void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objPortIdValue={0};

  xLibStr256_t portId={0};

  xLibU32_t portIdType;

  xLibU16_t length;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* get the value from application */

  owa.l7rc = usmDbLldpRemPortIdGet ( keyLocalPortNumValue,

                                     keyRemIndexValue, 

                                     keyTimeMarkValue,

                                     portId, &length);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  owa.l7rc = usmDbLldpRemPortIdSubtypeGet (keyLocalPortNumValue,

                                           keyRemIndexValue,

                                           keyTimeMarkValue,

                                           &portIdType);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  usmDbLldpPortIdFormat(objPortIdValue, sizeof(objPortIdValue), 

                         portIdType, portId, length);



  /* return the object value: PortId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPortIdValue,

                           strlen (objPortIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_PortDesc

*

* @purpose Get 'PortDesc'

*

* @description [PortDesc]: The string value used to identify the description

*              of the given port associated with the remote system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_PortDesc (void *wap, void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objPortDescValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* get the value from application */

  owa.l7rc = usmDbLldpRemPortDescGet ( keyLocalPortNumValue,

                                       keyRemIndexValue, 

                                       keyTimeMarkValue,

                                       objPortDescValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: PortDesc */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPortDescValue,

                           strlen (objPortDescValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_SysName

*

* @purpose Get 'SysName'

*

* @description [SysName]: The string value used to identify the system name

*              of the remote system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_SysName (void *wap, void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objSysNameValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* get the value from application */

  owa.l7rc = usmDbLldpRemSysNameGet ( keyLocalPortNumValue,

                                      keyRemIndexValue, 

                                      keyTimeMarkValue,

                                      objSysNameValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: SysName */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSysNameValue,

                           strlen (objSysNameValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_SysDesc

*

* @purpose Get 'SysDesc'

*

* @description [SysDesc]: The string value used to identify the system description

*              of the remote system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_SysDesc (void *wap, void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objSysDescValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* get the value from application */

  owa.l7rc = usmDbLldpRemSysDescGet ( keyLocalPortNumValue,

                                      keyRemIndexValue, 

                                      keyTimeMarkValue,

                                      objSysDescValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: SysDesc */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSysDescValue,

                           strlen (objSysDescValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_SysCapSupported

*

* @purpose Get 'SysCapSupported'

*

* @description [SysCapSupported]: The bitmap value used to identify which

*              system capabilities are supported on the remote system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_SysCapSupported (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objSysCapSupportedValue;

  xLibU16_t sysCap;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* get the value from application */

  owa.l7rc = usmDbLldpRemSysCapSupportedGet ( keyLocalPortNumValue,

                                              keyRemIndexValue,

                                              keyTimeMarkValue,

                                              &sysCap);



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  usmDbLldpSysCapFormat(objSysCapSupportedValue, sizeof(objSysCapSupportedValue), sysCap);



  /* return the object value: SysCapSupported */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSysCapSupportedValue,

                           strlen (objSysCapSupportedValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_SysCapEnabled

*

* @purpose Get 'SysCapEnabled'

*

* @description [SysCapEnabled]: The bitmap value used to identify which system

*              capabilities are enabled on the remote system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_SysCapEnabled (void *wap,

                                                               void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objSysCapEnabledValue;

  xLibU16_t val;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* get the value from application */

  owa.l7rc = usmDbLldpRemSysCapEnabledGet ( keyLocalPortNumValue,

                                            keyRemIndexValue,

                                            keyTimeMarkValue,

                                            &val);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  usmDbLldpSysCapFormat(objSysCapEnabledValue, sizeof(objSysCapEnabledValue), val);



  /* return the object value: SysCapEnabled */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSysCapEnabledValue,

                           strlen (objSysCapEnabledValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_LldpRemTTL

*

* @purpose Get 'LldpRemTTL'

*

* @description [LldpRemTTL]: The time to live for a remote entry. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_LldpRemTTL(void *wap, void *bufp) 

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibU32_t objLldpRemTTLValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* get the value from application */

  owa.l7rc = usmDbLldpRemTTLGet (keyLocalPortNumValue,

                                 keyRemIndexValue,

                                 keyTimeMarkValue,

                                 &objLldpRemTTLValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: SysCapEnabled */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objLldpRemTTLValue,

                           sizeof (objLldpRemTTLValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_ManAddrIfSubtype

*

* @purpose Get 'ManAddrIfSubtype'

*

* @description [ManAddrIfSubtype]: The enumeration value that identifies the

*              interface numbering method used for defining the interface

*              number, associated with the remote system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_ManAddrIfSubtype (void *wap,

                                                                  void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t keyManAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objManAddrIfSubtypeValue;

  lldpIANAAddrFamilyNumber_t family;

  xLibU16_t length;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* retrieve key: ManAddr */

  kwa4.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_ManAddr,

                           (xLibU8_t *) keyManAddrValue, &kwa4.len);

  if (kwa4.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyManAddrValue, kwa1.len);



  /* get the value from application */

  length = 4; /* TODO: CHECK */

  family = LLDP_IANA_ADDR_FAMILY_NUMBER_IPV4; /* TODO: CHECK */

  owa.l7rc = usmDbLldpRemManAddrIfSubtypeGet ( keyLocalPortNumValue,

                                               keyRemIndexValue,

                                               keyTimeMarkValue,

                                               family,

                                               keyManAddrValue,

                                               length,

                                               &objManAddrIfSubtypeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: ManAddrIfSubtype */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objManAddrIfSubtypeValue,

                           sizeof (objManAddrIfSubtypeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_ManAddrIfId

*

* @purpose Get 'ManAddrIfId'

*

* @description [ManAddrIfId]: The integer value used to identify the interface

*              number regarding the management address component associated

*              with the remote system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_ManAddrIfId (void *wap,

                                                             void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t keyManAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objManAddrIfIdValue;

  xLibU16_t length;

  lldpIANAAddrFamilyNumber_t family;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* retrieve key: ManAddr */

  kwa4.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_ManAddr,

                           (xLibU8_t *) keyManAddrValue, &kwa4.len);

  if (kwa4.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyManAddrValue, kwa4.len);



  /* get the value from application */

  length = 4; /* TODO: CHECK */

  family = LLDP_IANA_ADDR_FAMILY_NUMBER_IPV4; /* TODO: CHECK */

  owa.l7rc = usmDbLldpRemManAddrIfIdGet ( keyLocalPortNumValue,

                                          keyRemIndexValue,

                                          keyTimeMarkValue,

                                          family,

                                          keyManAddrValue, 

                                          length,

                                          &objManAddrIfIdValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: ManAddrIfId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objManAddrIfIdValue,

                           sizeof (objManAddrIfIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_ManAddrOID

*

* @purpose Get 'ManAddrOID'

*

* @description [ManAddrOID]: The OID value used to identify the type of hardware

*              component or protocol entity associated with the management

*              address advertised by the remote system agent. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_ManAddrOID (void *wap,

                                                            void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t keyManAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objManAddrOIDValue;

  lldpIANAAddrFamilyNumber_t family;

  xLibU16_t length;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* retrieve key: ManAddr */

  kwa4.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_ManAddr,

                           (xLibU8_t *) keyManAddrValue, &kwa4.len);

  if (kwa4.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyManAddrValue, kwa4.len);



  /* get the value from application */

  length = 4; /* TODO: CHECK */

  family = LLDP_IANA_ADDR_FAMILY_NUMBER_IPV4; /* TODO: CHECK */

  owa.l7rc = usmDbLldpRemManAddrOIDGet ( keyLocalPortNumValue,

                                         keyRemIndexValue,

                                         keyTimeMarkValue,

                                         family,

                                         keyManAddrValue,

                                         length,

                                         objManAddrOIDValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: ManAddrOID */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objManAddrOIDValue,

                           strlen (objManAddrOIDValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_UnknownTLVType

*

* @purpose Get 'UnknownTLVType'

*

* @description [UnknownTLVType]: This object represents the value extracted

*              from the type field of the TLV 

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_UnknownTLVType (void *wap,

                                                                void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objUnknownTLVTypeValue;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* retrieve key: UnknownTLVType */

  owa.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_UnknownTLVType,

                   (xLibU8_t *) & objUnknownTLVTypeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);

    owa.l7rc = usmDbLldpRemUnknownTLVEntryGet ( keyLocalPortNumValue,

                                                keyRemIndexValue,

                                                keyTimeMarkValue,

                                                objUnknownTLVTypeValue);

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objUnknownTLVTypeValue, owa.len);

    owa.l7rc =

      usmDbLldpRemUnknownTLVEntryNextGet (keyLocalPortNumValue,

                                          keyRemIndexValue,

                                          keyTimeMarkValue,

                                          &objUnknownTLVTypeValue);

  }

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* return the object value: UnknownTLVType */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnknownTLVTypeValue,

                           sizeof (objUnknownTLVTypeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_UnknownTLVInfo

*

* @purpose Get 'UnknownTLVInfo'

*

* @description [UnknownTLVInfo]:  This object represents the value extracted

*              from the value field of the TLV. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_UnknownTLVInfo (void *wap,

                                                                void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyUnknownTLVTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objUnknownTLVInfoValue;

  xLibU16_t length;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* retrieve key: UnknownTLVType */

  kwa4.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_UnknownTLVType,

                   (xLibU8_t *) & keyUnknownTLVTypeValue, &kwa4.len);

  if (kwa4.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUnknownTLVTypeValue, kwa4.len);



  /* get the value from application */

  owa.l7rc = usmDbLldpRemUnknownTLVInfoGet ( keyLocalPortNumValue,

                                             keyRemIndexValue,

                                             keyTimeMarkValue,

                                             keyUnknownTLVTypeValue,

                                             objUnknownTLVInfoValue,

                                             &length);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: UnknownTLVInfo */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUnknownTLVInfoValue,

                           strlen (objUnknownTLVInfoValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_OrgDefInfoOUI

*

* @purpose Get 'OrgDefInfoOUI'

*

* @description [OrgDefInfoOUI]: The Organizationally Unique Identifier (OUI),

*              as defined in IEEE std 802-2001, is a 24 bit (three octets)

*              globally unique assigned number referenced by various

*              standards,of the information received from the remote system.

*              

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_OrgDefInfoOUI (void *wap,

                                                               void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objOrgDefInfoOUIValue;

  xLibStr256_t nextObjOrgDefInfoOUIValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: OrgDefInfoOUI */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_OrgDefInfoOUI,

                          (xLibU8_t *) objOrgDefInfoOUIValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

#if 0

    owa.l7rc =  usmDbLldpRemOrgDefInfoEntryGet (L7_UNIT_CURRENT,

                                                nextObjOrgDefInfoOUIValue);

#endif

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, objOrgDefInfoOUIValue, owa.len);

#if 0

    owa.l7rc =

      usmDbLldpRemOrgDefInfoEntryNextGet (L7_UNIT_CURRENT,

                                          objOrgDefInfoOUIValue,

                                          nextObjOrgDefInfoOUIValue);

#endif

  }

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjOrgDefInfoOUIValue, owa.len);



  /* return the object value: OrgDefInfoOUI */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjOrgDefInfoOUIValue,

                           strlen (objOrgDefInfoOUIValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_OrgDefInfoSubtype

*

* @purpose Get 'OrgDefInfoSubtype'

*

* @description [OrgDefInfoSubtype]: The integer value used to identify the

*              subtype of the organizationally defined information received

*              from the remote system. The subtype value is required to

*              identify different instances of organizationally defined information

*              that could not be 

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_OrgDefInfoSubtype (void *wap,

                                                                   void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objOrgDefInfoSubtypeValue;

  xLibU32_t nextObjOrgDefInfoSubtypeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: OrgDefInfoSubtype */

  owa.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_OrgDefInfoSubtype,

                   (xLibU8_t *) & objOrgDefInfoSubtypeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

#if 0

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT,

                                     &nextObjOrgDefInfoSubtypeValue);

#endif

  }

  else

  {

#if 0

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objOrgDefInfoSubtypeValue, owa.len);

    owa.l7rc =

      usmDbLldpRemOrgDefInfoEntryNextGet (L7_UNIT_CURRENT,

                                          objOrgDefInfoSubtypeValue,

                                          &nextObjOrgDefInfoSubtypeValue);

#endif

  }

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjOrgDefInfoSubtypeValue, owa.len);



  /* return the object value: OrgDefInfoSubtype */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjOrgDefInfoSubtypeValue,

                           sizeof (objOrgDefInfoSubtypeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_OrgDefInfoIndex

*

* @purpose Get 'OrgDefInfoIndex'

*

* @description [OrgDefInfoIndex]: This object represents an arbitrary local

*              integer value used by this agent to identify a particular

*              unrecognized organizationally defined information instance,

*              unique only for the lldpRemOrgDefInfoOUI and lldpRemOrgDefInfoSubtype

*              from the same remote syst 

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_OrgDefInfoIndex (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objOrgDefInfoIndexValue;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa2.len);



  /* retrieve key: OrgDefInfoIndex */

  owa.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_OrgDefInfoIndex,

                   (xLibU8_t *) & objOrgDefInfoIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);

    owa.l7rc = usmDbLldpRemOrgDefInfoEntryGet(keyLocalPortNumValue,

                                              keyRemIndexValue,

                                              keyTimeMarkValue, 

                                              objOrgDefInfoIndexValue);

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objOrgDefInfoIndexValue, owa.len);

    owa.l7rc =

      usmDbLldpRemOrgDefInfoEntryNextGet (keyLocalPortNumValue,

                                          keyRemIndexValue,

                                          keyTimeMarkValue,

                                          &objOrgDefInfoIndexValue);

  }

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_NEW_KEY (bufp, &objOrgDefInfoIndexValue, owa.len);



  /* return the object value: OrgDefInfoIndex */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objOrgDefInfoIndexValue,

                           sizeof (objOrgDefInfoIndexValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_OrgDefInfo

*

* @purpose Get 'OrgDefInfo'

*

* @description [OrgDefInfo]: The string value used to identify the organizationally

*              defined information of the remote system. The encoding

*              for this object should be as defined for SnmpAdminString

*              TC. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_OrgDefInfo (void *wap,

                                                            void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyOrgDefInfoIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objOrgDefInfoValue;

  xLibU16_t length;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa2.len);



  /* retrieve key: OrgDefInfoIndex */

  kwa4.rc =

    xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_OrgDefInfoIndex,

                   (xLibU8_t *) & keyOrgDefInfoIndexValue, &kwa4.len);

  if (kwa4.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOrgDefInfoIndexValue, kwa1.len);



  /* get the value from application */

  owa.l7rc = usmDbLldpRemOrgDefInfoGet ( keyLocalPortNumValue,

                                         keyRemIndexValue,

                                         keyTimeMarkValue,

                                         keyOrgDefInfoIndexValue,

                                         objOrgDefInfoValue,

                                         &length );

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: OrgDefInfo */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objOrgDefInfoValue,

                           strlen (objOrgDefInfoValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjSet_SwitchingLLDPRemoteSystemData_ClearAllRemoteData

*

* @purpose Set 'ClearAllRemoteData'

*

* @description [ClearAllRemoteData] Clears all 802.1AB remote data

*

* @notes  

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingLLDPRemoteSystemData_ClearAllRemoteData (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objClearAllRemoteDataValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: ClearAllRemoteData */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objClearAllRemoteDataValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objClearAllRemoteDataValue, owa.len);



  /* if row status object is specified and eual to delete return success */



  /* set the value in application */

  owa.l7rc = usmDbLldpRemTableClear ();

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_LLDPRemoteDataPresent

*

* @purpose Get 'LLDPRemoteDataPresent'

* @description  [LLDPRemoteDataPresent] This object tells whether there is any 

* remote entry for the local port number.

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_LLDPRemoteDataPresent (void *wap, void *bufp)

{

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objLLDPRemoteDataPresentValue= L7_FALSE;

  xLibU32_t TimeMarkValue = 0;

  xLibU32_t objRemIndexValue = 0;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: LocalPortNum */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                          (xLibU8_t *) & keyLocalPortNumValue, &owa.len);

  if (owa.rc == XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, owa.len);



    if (usmDbLldpRemEntryGet(keyLocalPortNumValue, &objRemIndexValue, &TimeMarkValue) == L7_SUCCESS)

     objLLDPRemoteDataPresentValue = L7_TRUE;

  }



  /* return the object value: RemIndex */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objLLDPRemoteDataPresentValue,

                           sizeof (objLLDPRemoteDataPresentValue));



  FPOBJ_TRACE_EXIT (bufp, owa);

  return XLIBRC_SUCCESS;

}



/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_RomManAddFamily

*

* @purpose Get 'RomManAddFamily'

*

* @description [RomManAddFamily]: The string value used to identify the mgmt address

*              associated with the remote system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_RomManAddFamily (void *wap, void *bufp)

{

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibU32_t objRomManAddFamilyValue;



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: objRomManAddFamilyValue */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RomManAddFamily,

                           (xLibU8_t *) &objRomManAddFamilyValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    objRomManAddFamilyValue = LLDP_IANA_ADDR_FAMILY_NUMBER_IPV4; 

  }

  else 

  {

    if(objRomManAddFamilyValue == LLDP_IANA_ADDR_FAMILY_NUMBER_802) 

    {

      owa.l7rc = L7_FAILURE;

    }

    else

    { 

      objRomManAddFamilyValue += 1;

    }

  }



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE; 

    FPOBJ_TRACE_EXIT (bufp, owa); 

    return owa.rc;

  }



  /* return the object value: PortId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objRomManAddFamilyValue,

                           sizeof (objRomManAddFamilyValue));

  FPOBJ_TRACE_EXIT (bufp, owa);



  return owa.rc;

}



/* Rebuild the address based on its family type from ascii to raw bytes */

static L7_RC_t rebuildAddress(xLibStr256_t keyRomManAddrValue, 

                       lldpIANAAddrFamilyNumber_t family, 

                       xLibStr256_t addressString, 

                       xLibU8_t *length)

{

  xLibU32_t  ipAddrTemp;

  L7_ulong32 ip6AddrTemp;



 

  switch (family)

  {

    case LLDP_IANA_ADDR_FAMILY_NUMBER_802: 

      { 

        if (usmDbMacAddrStringToHex (keyRomManAddrValue, addressString) != L7_SUCCESS)

        {

          return L7_FAILURE;

        }

        *length = L7_MAC_ADDR_LEN; 



        return L7_SUCCESS;

      }

    case LLDP_IANA_ADDR_FAMILY_NUMBER_IPV4:

      {

        if (osapiInetPton (L7_AF_INET, (xLibU8_t *)keyRomManAddrValue, 

                                   addressString) != L7_SUCCESS)

        {

          return L7_FAILURE;

        }

        memcpy (&ipAddrTemp, addressString, sizeof (ipAddrTemp));

        ipAddrTemp = osapiHtonl (ipAddrTemp);

        memcpy (addressString, &ipAddrTemp, sizeof (ipAddrTemp));

        *length = sizeof(xLibU32_t);

       

        return L7_SUCCESS;

      }

    case LLDP_IANA_ADDR_FAMILY_NUMBER_IPV6:

      {

        if (osapiInetPton (L7_AF_INET6, (xLibU8_t *)keyRomManAddrValue, 

                                   addressString) != L7_SUCCESS)

        {

          return L7_FAILURE;

        }

        memcpy (&ip6AddrTemp, addressString, sizeof (ip6AddrTemp));

        ip6AddrTemp = osapiHtonl (ip6AddrTemp);

        memcpy (addressString, &ip6AddrTemp, sizeof (ip6AddrTemp));

        *length =  strlen(addressString);

        return L7_SUCCESS;

      }

    default:

          return L7_FAILURE;

  }

}



/*******************************************************************************

* @function fpObjGet_SwitchingLLDPRemoteSystemData_RomManAddr

*

* @purpose Get 'RomManAddr'

*

* @description [RomManAddr]: The string value used to identify the mgmt address

*              associated with the remote system. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingLLDPRemoteSystemData_RomManAddr (void *wap, void *bufp)

{

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyTimeMarkValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyLocalPortNumValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRemIndexValue;

  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyRomManAddFamilyValue;

  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t keyRomManAddrValue;

  xLibStr256_t objRomManAddrValue;

  xLibStr256_t addressString;

  xLibU8_t length;

  lldpIANAAddrFamilyNumber_t  family;



  memset(keyRomManAddrValue, 0, sizeof(keyRomManAddrValue)); 

  memset(objRomManAddrValue, 0, sizeof(objRomManAddrValue)); 

  memset(addressString, 0, sizeof(addressString)); 



  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: TimeMark */

  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_TimeMark,

                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);

  if (kwa1.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);



  /* retrieve key: LocalPortNum */

  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_LocalPortNum,

                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);

  if (kwa2.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);



  /* retrieve key: RemIndex */

  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RemIndex,

                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);



  /* retrieve key: RomManAddFamilyValue */

  kwa4.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RomManAddFamily,

                           (xLibU8_t *) & keyRomManAddFamilyValue, &kwa4.len);

  if (kwa4.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRomManAddFamilyValue, kwa3.len);



  /* retrieve key: RomManAddr */

  kwa5.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPRemoteSystemData_RomManAddr,

                           (xLibU8_t *) &keyRomManAddrValue, &kwa5.len);



  if (kwa5.rc != XLIBRC_SUCCESS)

  { /* Get the first entry */ 

    length = 0;

  }

  else 

  { 

    /* next element */

    family = keyRomManAddFamilyValue;

    owa.l7rc = rebuildAddress(keyRomManAddrValue, keyRomManAddFamilyValue, addressString, &length);

    if (owa.l7rc != L7_SUCCESS)

    { 

      owa.rc = XLIBRC_FAILURE;

      FPOBJ_TRACE_EXIT (bufp, owa);

      return owa.rc;

    }

  }



  while((owa.l7rc = usmDbLldpRemManAddrEntryNextGet (keyLocalPortNumValue,

                                               keyRemIndexValue,

                                               keyTimeMarkValue,

                                               &family, addressString,  

                                               &length)) == L7_SUCCESS)

  {

      if (keyRomManAddFamilyValue == family)

      { 

        usmDbLldpManAddrFormat(objRomManAddrValue, strlen(objRomManAddrValue), family, 

                                   addressString, length);

        owa.rc = XLIBRC_SUCCESS; 

        break;

      }

  }



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objRomManAddrValue,

                           strlen (objRomManAddrValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

