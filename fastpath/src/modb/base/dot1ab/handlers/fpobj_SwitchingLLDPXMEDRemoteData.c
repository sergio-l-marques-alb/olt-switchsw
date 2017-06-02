/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingLLDPXMEDRemoteData.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to LLDP-object.xml
*
* @create  11 February 2008
*
* @author  
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingLLDPXMEDRemoteData_obj.h"
#include "usmdb_lldp_api.h"
#include "usmdb_util_api.h"
#include "lldp_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_TimeMark
*
* @purpose Get 'TimeMark'
*
* @description [TimeMark]: A TimeFilter for this entry. See the TimeFilter
*              textual convention in IETF RFC 2021 and http://www.ietf.org/IESG/Implementations/RFC2021-Implementation.txt
*              to see how
*              TimeFilter works. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_TimeMark (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTimeMarkValue;
  xLibU32_t objRemIndexValue;
  xLibU32_t nextObjRemIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                          (xLibU8_t *) & keyLocalPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa.len);

  owa.len = sizeof (objRemIndexValue);
  
  /* retrieve key: RemIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                          (xLibU8_t *) & objRemIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
     FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }  

  /* retrieve key: TimeMark */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                          (xLibU8_t *) & objTimeMarkValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objTimeMarkValue = 0;
    nextObjRemIndexValue = 0;
    do 
    {
       owa.l7rc = usmDbLldpRemEntryGetNext(keyLocalPortNumValue, &nextObjRemIndexValue, &objTimeMarkValue);
       if (nextObjRemIndexValue == objRemIndexValue)
       {
          break;
       }
    }while (owa.l7rc == L7_SUCCESS);
  }
  else
  {
    /* For each rem index and local intf time mark is unique. Hence returnig failure for next entry of  timemark */
    owa.l7rc = L7_FAILURE;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &objTimeMarkValue, owa.len);

  /* return the object value: TimeMark */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTimeMarkValue,
                           sizeof (objTimeMarkValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_LocalPortNum
*
* @purpose Get 'LocalPortNum'
*
* @description [LocalPortNum]: The index value used to identify the port component
*              (contained in the local chassis with the LLDP agent)
*              associated with this entry. The LocalPortNum identifies
*              the port on which the remote system information is received.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_LocalPortNum (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLocalPortNumValue;
  xLibU32_t nextObjLocalPortNumValue;
  xLibU32_t mode = L7_DISABLE;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                          (xLibU8_t *) & objLocalPortNumValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbLldpRemoteIntfFirstGet ( &nextObjLocalPortNumValue);
    while (owa.l7rc == L7_SUCCESS)
    {
       mode = L7_DISABLE;
       objLocalPortNumValue = nextObjLocalPortNumValue;
       if((usmDbLldpXMedPortAdminModeGet(objLocalPortNumValue, &mode) == L7_SUCCESS) &&( mode == L7_ENABLE))
       {
         break;
       }
       else
       {
         owa.l7rc = usmDbLldpRemoteIntfNextGet (objLocalPortNumValue, &nextObjLocalPortNumValue);
       }
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLocalPortNumValue, owa.len);
    owa.l7rc = L7_SUCCESS;
    while (owa.l7rc == L7_SUCCESS)
    {
       mode = L7_DISABLE;
       
       owa.l7rc = usmDbLldpRemoteIntfNextGet (objLocalPortNumValue, &nextObjLocalPortNumValue);
       objLocalPortNumValue = nextObjLocalPortNumValue;
       if((usmDbLldpXMedPortAdminModeGet(objLocalPortNumValue, &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
       {
         break;
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
                           sizeof (objLocalPortNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_RemIndex
*
* @purpose Get 'RemIndex'
*
* @description [RemIndex]: This object represents an arbitrary local integer
*              value used by this agent to identify a particular connection
*              instance, unique only for the indicated remote system.
*              An agent is encouraged to assign monotonically increasing
*              index values to new entries, s 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_RemIndex (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRemIndexValue;
  xLibU32_t TimeMarkValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                          (xLibU8_t *) & keyLocalPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa.len);

  /* retrieve key: RemIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                          (xLibU8_t *) & objRemIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objRemIndexValue = 0;
    TimeMarkValue = 0;
    owa.l7rc = usmDbLldpRemEntryGet (keyLocalPortNumValue,
                                     &objRemIndexValue,&TimeMarkValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRemIndexValue, owa.len);
    TimeMarkValue = 0;
    owa.l7rc = usmDbLldpRemEntryGetNext(keyLocalPortNumValue, &objRemIndexValue, &TimeMarkValue);
  }
  if (owa.l7rc != L7_SUCCESS)
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
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PolicyAppType
*
* @purpose Get 'PolicyAppType'
*
* @description [PolicyAppType]: The media type that defines the primary function
*              of the application for the policy advertised by the
*              endpoint connected remotely to this port. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PolicyAppType (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyAppTypeValue,vlanId=0;
  xLibU32_t nextObjPolicyAppTypeValue;
  xLibU32_t keyLocalPortNumValue;
  xLibU32_t keyRemIndexValue;
  xLibU32_t keyTimeMarkValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                          (xLibU8_t *) & keyLocalPortNumValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa1.len);

  /* retrieve key: RemIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                          (xLibU8_t *) & keyRemIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa2.len);


  /* retrieve key: TimeMark */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa3.len);


  /* retrieve key: PolicyAppType */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_PolicyAppType,
                          (xLibU8_t *) & objPolicyAppTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjPolicyAppTypeValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyAppTypeValue, owa.len);
    if (objPolicyAppTypeValue < videoSignaling_policyapptype)
    {
      nextObjPolicyAppTypeValue = objPolicyAppTypeValue + 1;
    }
    else
    {
      owa.l7rc = L7_FAILURE;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  do
  {
    owa.l7rc = usmDbLldpXMedRemMediaPolicyVlanIdGet(keyLocalPortNumValue, keyRemIndexValue, 
                               keyTimeMarkValue, nextObjPolicyAppTypeValue, &vlanId);
    if (owa.l7rc == L7_SUCCESS)
    {
      break; 
    }
    nextObjPolicyAppTypeValue += 1;    
  }while( nextObjPolicyAppTypeValue <= videoSignaling_policyapptype);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPolicyAppTypeValue, owa.len);

  /* return the object value: PolicyAppType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPolicyAppTypeValue,
                           sizeof (objPolicyAppTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_LocationSubtype
*
* @purpose Get 'LocationSubtype'
*
* @description [LocationSubtype]: The location subtype advertised by the remote
*              endpoint. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_LocationSubtype (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLocationSubtypeValue;
  xLibU32_t nextObjLocationSubtypeValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t keyLocalPortNumValue;
  xLibU32_t keyRemIndexValue;
  xLibU32_t keyTimeMarkValue;
  xLibStr256_t buf;

  /* retrieve key: LocalPortNum */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                          (xLibU8_t *) & keyLocalPortNumValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa1.len);

  /* retrieve key: RemIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                          (xLibU8_t *) & keyRemIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa2.len);


  /* retrieve key: TimeMark */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa3.len);


  /* retrieve key: LocationSubtype */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocationSubtype,
                          (xLibU8_t *) & objLocationSubtypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjLocationSubtypeValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLocationSubtypeValue, owa.len);
    if (objLocationSubtypeValue < elin_locsubtype)
    {
      nextObjLocationSubtypeValue = objLocationSubtypeValue + 1;
    }
    else
    {
      owa.l7rc = L7_FAILURE; 
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  do
  {
    memset(buf, 0x00, sizeof(buf));
    owa.l7rc = usmDbLldpXMedRemLocationInfoGet(keyLocalPortNumValue, keyRemIndexValue, keyTimeMarkValue, 
                       nextObjLocationSubtypeValue, buf);
    if ((owa.l7rc  == L7_SUCCESS) && (strcmp(buf, " ") !=0 ))
    {
      break;
    }
    nextObjLocationSubtypeValue += 1;
  }while(nextObjLocationSubtypeValue <= elin_locsubtype);


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLocationSubtypeValue, owa.len);

  /* return the object value: LocationSubtype */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLocationSubtypeValue,
                           sizeof (objLocationSubtypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_HardwareRev
*
* @purpose Get 'HardwareRev'
*
* @description [HardwareRev]: The vendor-specific hardware revision string
*              as advertised by the remote endpoint 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_HardwareRev (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objHardwareRevValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedRemHardwareRevGet (keyLocalPortNumValue,
                                             keyRemIndexValue,
                                             keyTimeMarkValue,
                                             objHardwareRevValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: HardwareRev */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objHardwareRevValue,
                           strlen (objHardwareRevValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_FirmwareRev
*
* @purpose Get 'FirmwareRev'
*
* @description [FirmwareRev]: The vendor-specific firmware revision string
*              as advertised by the remote endpoint. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_FirmwareRev (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objFirmwareRevValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedRemFirmwareRevGet (keyLocalPortNumValue,
                                             keyRemIndexValue,
                                             keyTimeMarkValue,
                                             objFirmwareRevValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FirmwareRev */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objFirmwareRevValue,
                           strlen (objFirmwareRevValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_SoftwareRev
*
* @purpose Get 'SoftwareRev'
*
* @description [SoftwareRev]: The vendor-specific software revision string
*              as advertised by the remote endpoint 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_SoftwareRev (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSoftwareRevValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedRemSoftwareRevGet (keyLocalPortNumValue,
                                             keyRemIndexValue,
                                             keyTimeMarkValue, 
                                             objSoftwareRevValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SoftwareRev */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSoftwareRevValue,
                           strlen (objSoftwareRevValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_SerialNum
*
* @purpose Get 'SerialNum'
*
* @description [SerialNum]: The vendor-specific serial number as advertised
*              by the remote endpoint. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_SerialNum (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSerialNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedRemSerialNumGet (keyLocalPortNumValue,
                                           keyRemIndexValue, 
                                           keyTimeMarkValue,
                                           objSerialNumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SerialNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSerialNumValue,
                           strlen (objSerialNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_MfgName
*
* @purpose Get 'MfgName'
*
* @description [MfgName]: The vendor-specific manufacturer name as advertised
*              by the remote endpoint. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_MfgName (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMfgNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedRemMfgNameGet (keyLocalPortNumValue,
                                         keyRemIndexValue, 
                                         keyTimeMarkValue,
                                         objMfgNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MfgName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMfgNameValue,
                           strlen (objMfgNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_ModelName
*
* @purpose Get 'ModelName'
*
* @description [ModelName]: The vendor-specific model name as advertised by
*              the remote endpoint. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_ModelName (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objModelNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedRemModelNameGet (keyLocalPortNumValue,
                                           keyRemIndexValue,
                                           keyTimeMarkValue,
                                           objModelNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ModelName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objModelNameValue,
                           strlen (objModelNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_AssetID
*
* @purpose Get 'AssetID'
*
* @description [AssetID]: The vendor-specific asset tracking identifier as
*              advertised by the remote endpoint 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_AssetID (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAssetIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedRemAssetIdGet (keyLocalPortNumValue,
                                         keyRemIndexValue,
                                         keyTimeMarkValue,
                                         objAssetIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AssetID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAssetIDValue,
                           strlen (objAssetIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PoEDeviceType
*
* @purpose Get 'PoEDeviceType'
*
* @description [PoEDeviceType]: Defines the type of Power-via-MDI (Power over
*              Ethernet) advertised by the remote device. A value pseDevice(2)
*              indicates that the device is advertised as a Power
*              Sourcing Entity (PSE). A value pdDevice(3) indicates that
*              the device is advertised as a Power 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PoEDeviceType (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoEDeviceTypeValue={0};
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedRemXPoeDeviceTypeGet (keyLocalPortNumValue, 
                                       keyRemIndexValue,
                                       keyTimeMarkValue,
                                       &objPoEDeviceTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoEDeviceType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objPoEDeviceTypeValue,
                           sizeof(objPoEDeviceTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PoEPDPowerSource
*
* @purpose Get 'PoEPDPowerSource'
*
* @description [PoEPDPowerSource]: Defines the type of Power Source advertised
*              as being used by the device connected remotely to the
*              port. A value fromPSE(2) indicates that the device advertises
*              its power source as received from a PSE. A value local(3)
*              indicates that the device advertise 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PoEPDPowerSource (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoEPDPowerSourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedRemXPoePDPowerSrcGet (keyLocalPortNumValue, 
                                        keyRemIndexValue,
                                        keyTimeMarkValue,
                                        &objPoEPDPowerSourceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoEPDPowerSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoEPDPowerSourceValue,
                           sizeof (objPoEPDPowerSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PoEPDPowerPriority
*
* @purpose Get 'PoEPDPowerPriority'
*
* @description [PoEPDPowerPriority]: Defines the priority advertised as being
*              required by the PD connected remotely to the port. A value
*              critical(2) indicates that the device advertises its power
*              Priority as critical, as per RFC 3621. A value high(3)
*              indicates that the device advertises it 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PoEPDPowerPriority (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoEPDPowerPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedRemXPoePDPowerPriGet (keyLocalPortNumValue, 
                                        keyRemIndexValue,
                                        keyTimeMarkValue,
                                        &objPoEPDPowerPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoEPDPowerPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoEPDPowerPriorityValue,
                           sizeof (objPoEPDPowerPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PoEPDPowerReq
*
* @purpose Get 'PoEPDPowerReq'
*
* @description [PoEPDPowerReq]: This object contains the value of the power
*              required by a PD expressed in units of 0.1 watts. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PoEPDPowerReq(void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPoEPDPowerReqValue;
  xLibU32_t   tempPoEPDPowerReqValue;
  xLibU32_t   tempPoEPDPowerReqValueFraction;
  FPOBJ_TRACE_ENTER (bufp);

  memset(&objPoEPDPowerReqValue,0,sizeof(objPoEPDPowerReqValue));

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedRemXPoePDPowerReqGet (keyLocalPortNumValue, 
                                        keyRemIndexValue,
                                        keyTimeMarkValue,
                                        &tempPoEPDPowerReqValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  tempPoEPDPowerReqValueFraction = (tempPoEPDPowerReqValue-(tempPoEPDPowerReqValue/10)*10);
  tempPoEPDPowerReqValue = tempPoEPDPowerReqValue/10;

  osapiSnprintf(objPoEPDPowerReqValue, sizeof(objPoEPDPowerReqValue),
                  "%d.%02d Watts",tempPoEPDPowerReqValue,tempPoEPDPowerReqValueFraction);
  /* return the object value: PoEPDPowerPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoEPDPowerReqValue,
                           strlen(objPoEPDPowerReqValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PoEPSEPowerAv
*
* @purpose Get 'PoEPSEPowerAv'
*
* @description [PoEPSEPowerAv]: This object contains the value of the power
*              available from the PSE via this port expressed in units of
*              0.1 watts on the remote device. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PoEPSEPowerAv (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPoEPSEPowerAvValue;
  xLibU32_t tempPoEPSEPowerAvValue;
  xLibU32_t tempPoEPSEPowerAvValueFraction;
  FPOBJ_TRACE_ENTER (bufp);

  memset(&objPoEPSEPowerAvValue,0,sizeof(objPoEPSEPowerAvValue));

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedRemXPoePSEPowerAvGet (keyLocalPortNumValue, 
                                       keyRemIndexValue,
                                       keyTimeMarkValue, 
                                       &tempPoEPSEPowerAvValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  tempPoEPSEPowerAvValueFraction = (tempPoEPSEPowerAvValue-(tempPoEPSEPowerAvValue/10)*10);
  tempPoEPSEPowerAvValue = tempPoEPSEPowerAvValue/10;

  osapiSnprintf(objPoEPSEPowerAvValue, sizeof(objPoEPSEPowerAvValue),
                  "%d.%02d Watts",tempPoEPSEPowerAvValue,tempPoEPSEPowerAvValueFraction);
 
  /* return the object value: PoEPSEPowerAv */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoEPSEPowerAvValue,
                           strlen (objPoEPSEPowerAvValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PoEPSEPowerSource
*
* @purpose Get 'PoEPSEPowerSource'
*
* @description [PoEPSEPowerSource]: Defines the type of PSE Power Source advertised
*              by the remote device. A value primary(2) indicates
*              that the device advertises its power source as primary. A
*              value backup(3) indicates that the device advertises its power
*              Source as backup. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PoEPSEPowerSource (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoEPSEPowerSourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedRemXPoePSEPowerSrcGet (keyLocalPortNumValue, 
                                        keyRemIndexValue,
                                        keyTimeMarkValue,
                                        &objPoEPSEPowerSourceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoEPSEPowerSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoEPSEPowerSourceValue,
                           sizeof (objPoEPSEPowerSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PoEPSEPowerPriority
*
* @purpose Get 'PoEPSEPowerPriority'
*
* @description [PoEPSEPowerPriority]: This object contains the value of the
*              PSE power priority advertised by the remote device. A value
*              critical(2) indicates that the device advertises its power
*              priority as critical, as per RFC 3621. A value high(3) indicates
*              that the device advertises its 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PoEPSEPowerPriority (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoEPSEPowerPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedRemXPoePSEPowerPriGet (keyLocalPortNumValue, 
                                        keyRemIndexValue,
                                        keyTimeMarkValue,
                                        &objPoEPSEPowerPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoEPSEPowerPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoEPSEPowerPriorityValue,
                           sizeof (objPoEPSEPowerPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_CapSupported
*
* @purpose Get 'CapSupported'
*
* @description [CapSupported]: The bitmap includes the MED organizationally
*              defined set of LLDP TLVs whose transmission is possible on
*              the LLDP agent of the remote device connected to this port.
*              Each bit in the bitmap corresponds to an LLDP-MED subtype
*              associated with a specific TIA TR 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_CapSupported (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objCapSupportedValue;
  lldpXMedCapabilities_t capGet;
  L7_BOOL  commaFlag = L7_FALSE;
  FPOBJ_TRACE_ENTER (bufp);

  memset(&capGet, 0, sizeof(capGet));
  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedRemCapSupportedGet (keyLocalPortNumValue,
                                              keyRemIndexValue,
                                              keyTimeMarkValue,
                                              &capGet);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CapSupported */
  if(capGet.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK)
  {
    osapiStrncpySafe(objCapSupportedValue, usmDbLldpXMedMediaCapabilitiesString(0), sizeof(objCapSupportedValue));
    commaFlag = L7_TRUE;
  }
  if(capGet.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
  {
    if(commaFlag == L7_TRUE)
    {
      osapiStrncat(objCapSupportedValue, ", ", sizeof(objCapSupportedValue));
    }
    osapiStrncat(objCapSupportedValue, usmDbLldpXMedMediaCapabilitiesString(1), sizeof(objCapSupportedValue));
    commaFlag = L7_TRUE;
  }
  if(capGet.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)
  {
    if(commaFlag == L7_TRUE)
    {
      osapiStrncat(objCapSupportedValue, ", ", sizeof(objCapSupportedValue));
    }
    osapiStrncat(objCapSupportedValue, usmDbLldpXMedMediaCapabilitiesString(2), sizeof(objCapSupportedValue));
    commaFlag = L7_TRUE;
  }
  if(capGet.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)
  {
    if(commaFlag == L7_TRUE)
    {
      osapiStrncat(objCapSupportedValue, ", ", sizeof(objCapSupportedValue));
    }
    osapiStrncat(objCapSupportedValue, usmDbLldpXMedMediaCapabilitiesString(3), sizeof(objCapSupportedValue));
    commaFlag = L7_TRUE;
  }
  if(capGet.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)
  {
    if(commaFlag == L7_TRUE)
    {
      osapiStrncat(objCapSupportedValue, ", ", sizeof(objCapSupportedValue));
    }
    osapiStrncat(objCapSupportedValue, usmDbLldpXMedMediaCapabilitiesString(4), sizeof(objCapSupportedValue));
    commaFlag = L7_TRUE;
  }
  if(capGet.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)
  {
    if(commaFlag == L7_TRUE)
    {
      osapiStrncat(objCapSupportedValue, ", ", sizeof(objCapSupportedValue));
    }
    osapiStrncat(objCapSupportedValue, usmDbLldpXMedMediaCapabilitiesString(5), sizeof(objCapSupportedValue));
  }
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objCapSupportedValue,
                           strlen (objCapSupportedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_CapCurrent
*
* @purpose Get 'CapCurrent'
*
* @description [CapCurrent]: The bitmap includes the MED organizationally
*              defined set of LLDP TLVs whose transmission is possible on
*              the LLDP agent of the remote device connected to this port.
*              Each bit in the bitmap corresponds to an LLDP-MED subtype
*              associated with a specific TIA TR 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_CapCurrent (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objCapCurrentValue;
  lldpXMedCapabilities_t capGet;
  L7_BOOL  commaFlag = L7_FALSE;
  FPOBJ_TRACE_ENTER (bufp);
  
  memset(&capGet, 0, sizeof(capGet));
  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedRemCapCurrentGet (keyLocalPortNumValue,
                                            keyRemIndexValue,
                                            keyTimeMarkValue,
                                            &capGet);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CapCurrent */
  if(capGet.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK)
  {
    osapiStrncpySafe(objCapCurrentValue, usmDbLldpXMedMediaCapabilitiesString(0), sizeof(objCapCurrentValue));
    commaFlag = L7_TRUE;
  }
  if(capGet.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
  {
    if(commaFlag == L7_TRUE)
    {
      osapiStrncat(objCapCurrentValue, ", ", sizeof(objCapCurrentValue));
    }
    osapiStrncat(objCapCurrentValue, usmDbLldpXMedMediaCapabilitiesString(1), sizeof(objCapCurrentValue));
    commaFlag = L7_TRUE; 
  }
  if(capGet.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)
  {
    if(commaFlag == L7_TRUE)
    {
      osapiStrncat(objCapCurrentValue, ", ", sizeof(objCapCurrentValue));
    }
    osapiStrncat(objCapCurrentValue, usmDbLldpXMedMediaCapabilitiesString(2), sizeof(objCapCurrentValue));
    commaFlag = L7_TRUE;
  }
  if(capGet.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)
  {
    if(commaFlag == L7_TRUE)
    {
      osapiStrncat(objCapCurrentValue, ", ", sizeof(objCapCurrentValue));
    }
    osapiStrncat(objCapCurrentValue, usmDbLldpXMedMediaCapabilitiesString(3), sizeof(objCapCurrentValue));
    commaFlag = L7_TRUE;
  }
  if(capGet.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)
  {
    if(commaFlag == L7_TRUE)
    {
      osapiStrncat(objCapCurrentValue, ", ", sizeof(objCapCurrentValue));
    }
    osapiStrncat(objCapCurrentValue, usmDbLldpXMedMediaCapabilitiesString(4), sizeof(objCapCurrentValue));
    commaFlag = L7_TRUE;
  }
  if(capGet.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)
  {
    if(commaFlag == L7_TRUE)
    {
      osapiStrncat(objCapCurrentValue, ", ", sizeof(objCapCurrentValue));
    }
    osapiStrncat(objCapCurrentValue, usmDbLldpXMedMediaCapabilitiesString(5), sizeof(objCapCurrentValue));
  }
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objCapCurrentValue,
                           strlen (objCapCurrentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_DeviceClass
*
* @purpose Get 'DeviceClass'
*
* @description [DeviceClass]: Device Class as advertised by the device remotely
*              connected to the port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_DeviceClass (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t type;
  xLibStr256_t objDeviceClassValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedRemDeviceClassGet (keyLocalPortNumValue,
                                             keyRemIndexValue,
                                             keyTimeMarkValue,
                                             &type);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  switch (type)
  {
    case notDefined:
      osapiStrncpy(objDeviceClassValue, "Not Defined", sizeof(objDeviceClassValue));
      break;
    case endpointClass1:
      osapiStrncpy(objDeviceClassValue,"Endpoint Class I", sizeof(objDeviceClassValue));
      break;
    case endpointClass2:
      osapiStrncpy(objDeviceClassValue,"Endpoint Class II", sizeof(objDeviceClassValue));
      break;
    case endpointClass3:
      osapiStrncpy(objDeviceClassValue,"Endpoint Class III", sizeof(objDeviceClassValue));
      break;
    case networkConnectivity:
      osapiStrncpy(objDeviceClassValue,"Network Connectivity", sizeof(objDeviceClassValue));
      break;
    default:
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* return the object value: DeviceClass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDeviceClassValue,
                           strlen (objDeviceClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PolicyVlanID
*
* @purpose Get 'PolicyVlanID'
*
* @description [PolicyVlanID]: An extension of the VLAN Identifier for the
*              remote system connected to this port, as defined in IEEE 802.1P-1998.
*              A value of 1 through 4094 is used to define a valid
*              PVID. A value of 0 shall be used if the device is using
*              priority tagged frames, meaning t 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PolicyVlanID (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAppTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyVlanIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* retrieve key: PolicyAppType */
  kwa4.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_PolicyAppType,
                           (xLibU8_t *) & keyPolicyAppTypeValue, &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedRemMediaPolicyVlanIdGet (keyLocalPortNumValue,
                                          keyRemIndexValue,
                                          keyTimeMarkValue,
                                          keyPolicyAppTypeValue,
                                          &objPolicyVlanIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyVlanID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyVlanIDValue,
                           sizeof (objPolicyVlanIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PolicyPriority
*
* @purpose Get 'PolicyPriority'
*
* @description [PolicyPriority]: This object contains the value of the 802.1p
*              priority which is associated with the remote system connected
*              at given port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PolicyPriority (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAppTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* retrieve key: PolicyAppType */
  kwa4.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_PolicyAppType,
                           (xLibU8_t *) & keyPolicyAppTypeValue, &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedRemMediaPolicyPriorityGet (keyLocalPortNumValue,
                                            keyRemIndexValue,
                                            keyTimeMarkValue,
                                            keyPolicyAppTypeValue,
                                            &objPolicyPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyPriorityValue,
                           sizeof (objPolicyPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PolicyDscp
*
* @purpose Get 'PolicyDscp'
*
* @description [PolicyDscp]: This object contains the value of the Differentiated
*              Service Code Point (DSCP) as defined in IETF RFC 2474
*              and RFC 2475 which is associated with the remote system
*              connected at a given port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PolicyDscp (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAppTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyDscpValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* retrieve key: PolicyAppType */
  kwa4.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_PolicyAppType,
                           (xLibU8_t *) & keyPolicyAppTypeValue, &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedRemMediaPolicyDscpGet (keyLocalPortNumValue, 
                                        keyRemIndexValue,
                                        keyTimeMarkValue,
                                        keyPolicyAppTypeValue,
                                        &objPolicyDscpValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyDscp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyDscpValue,
                           sizeof (objPolicyDscpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PolicyUnknown
*
* @purpose Get 'PolicyUnknown'
*
* @description [PolicyUnknown]: A value of 'true' indicates that the network
*              policy for the specified application type is currently unknown.
*              In this case, the VLAN ID, the layer 2 priority and
*              the DSCP value fields are ignored. A value of 'false' indicates
*              that this network policy is 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PolicyUnknown (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAppTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyUnknownValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* retrieve key: PolicyAppType */
  kwa4.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_PolicyAppType,
                           (xLibU8_t *) & keyPolicyAppTypeValue, &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedRemMediaPolicyUnknownGet (keyLocalPortNumValue,
                                           keyRemIndexValue,
                                           keyTimeMarkValue,
                                           keyPolicyAppTypeValue,
                                           &objPolicyUnknownValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyUnknown */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyUnknownValue,
                           sizeof (objPolicyUnknownValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_PolicyTagged
*
* @purpose Get 'PolicyTagged'
*
* @description [PolicyTagged]: A value of 'true' indicates that the application
*              is using a tagged VLAN. A value of 'false' indicates
*              that for the specific application the device either is using
*              an untagged VLAN or does not support port based VLAN operation.
*              In this case, both the VLAN 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_PolicyTagged (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAppTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyTaggedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* retrieve key: PolicyAppType */
  kwa4.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_PolicyAppType,
                           (xLibU8_t *) & keyPolicyAppTypeValue, &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedRemMediaPolicyTaggedGet (keyLocalPortNumValue,
                                          keyRemIndexValue,
                                          keyTimeMarkValue,
                                          keyPolicyAppTypeValue,
                                          &objPolicyTaggedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyTagged */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyTaggedValue,
                           sizeof (objPolicyTaggedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_LocationInfo
*
* @purpose Get 'LocationInfo'
*
* @description [LocationInfo]: The location information advertised by the
*              remote endpoint. Parsing of this information is dependent upon
*              the location subtype, as defined by the value of the corresponding
*              lldpXMedRemLocationSubType object 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDRemoteData_LocationInfo (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocationSubtypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objLocationInfoValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* retrieve key: LocationSubtype */
  kwa4.rc =
    xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDRemoteData_LocationSubtype,
                   (xLibU8_t *) & keyLocationSubtypeValue, &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedRemLocationInfoGet (keyLocalPortNumValue,
                                              keyRemIndexValue,
                                              keyTimeMarkValue,
                                              keyLocationSubtypeValue,
                                              objLocationInfoValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LocationInfo */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLocationInfoValue,
                           strlen (objLocationInfoValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/***********************************************************************
* @function fpObjGet_SwitchingLLDPXMEDRemoteData_LocationInfo
*
* @purpose Get 'LocationInfo'
*
* @description [LocationInfo]: The location information advertised by the
*              remote endpoint. Parsing of this information is dependent upon
*              the location subtype, as defined by the value of the corresponding
*              lldpXMedRemLocationSubType object
*
* @return
***********************************************************************/
xLibRC_t
fpObjGet_SwitchingLLDPXMEDRemoteData_MediaApplicationType (void *wap, void *bufp)
{
  L7_uint32 policyAppType=VOICE_POLICYAPPTYPE;
  xLibU16_t len= sizeof(policyAppType);

  if(XLIBRC_NO_FILTER == xLibFilterGet (wap,XOBJ_SwitchingLLDPXMEDRemoteData_MediaApplicationType,
        (xLibU8_t *)&policyAppType,&len))
  {
     return xLibBufDataSet(bufp,(xLibU8_t *)&policyAppType, sizeof(policyAppType));
  }
  else
  {
     policyAppType++;
     if (policyAppType <= VIDEOSIGNALLING_POLICYAPPTYPE)
     {
       return xLibBufDataSet(bufp,(xLibU8_t *)&policyAppType, sizeof(policyAppType));
     }
     return XLIBRC_ENDOF_TABLE;
  }
  return XLIBRC_SUCCESS;
}


