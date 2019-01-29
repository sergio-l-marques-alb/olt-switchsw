/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingSwitchSnoopingGroup.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to Snooping-object.xml
*
* @create  13 February 2008
*
* @author  Radha K
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingSwitchSnoopingGroup_obj.h"
#include "usmdb_snooping_api.h"
#include "filter_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingGroup_SnoopingProtocol
*
* @purpose Get 'SnoopingProtocol'
*
* @description [SnoopingProtocol]: The protocol type of network protocol in
*              use 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingGroup_SnoopingProtocol (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingProtocolValue =0;
  xLibU32_t nextObjSnoopingProtocolValue=0;
  L7_uchar8 tmpFamily;
  FPOBJ_TRACE_ENTER (bufp);

  memset(&nextObjSnoopingProtocolValue,0, sizeof(nextObjSnoopingProtocolValue));
  /* retrieve key: SnoopingProtocol */
  owa.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingGroup_SnoopingProtocol,
                   (xLibU8_t *) & objSnoopingProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    tmpFamily = (L7_uchar8)objSnoopingProtocolValue;
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objSnoopingProtocolValue = 0;
    if(usmDbSnoopProtocolNextGet(tmpFamily, &tmpFamily) == L7_TRUE)
    {
       nextObjSnoopingProtocolValue = tmpFamily;
       owa.l7rc = L7_SUCCESS;
    }
    else      
       owa.l7rc = L7_FAILURE;      
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objSnoopingProtocolValue, owa.len);
    tmpFamily = (L7_uchar8)objSnoopingProtocolValue;
    if(usmDbSnoopProtocolNextGet (tmpFamily,
                                          &tmpFamily)== L7_TRUE)
    {
       nextObjSnoopingProtocolValue = tmpFamily;
       owa.l7rc = L7_SUCCESS;
    }
    else 
       owa.l7rc = L7_FAILURE;

  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSnoopingProtocolValue, owa.len);

  /* return the object value: SnoopingProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjSnoopingProtocolValue,
                           sizeof (nextObjSnoopingProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
 

/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingGroup_SnoopingAdminMode
*
* @purpose Get 'SnoopingAdminMode'
*
* @description [SnoopingAdminMode]: This enables or disables Snooping on the
*              system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingGroup_SnoopingAdminMode (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbSnoopAdminModeGet (L7_UNIT_CURRENT,
                                     &objSnoopingAdminModeValue,
                                     keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopingAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSnoopingAdminModeValue,
                           sizeof (objSnoopingAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingSwitchSnoopingGroup_SnoopingAdminMode
*
* @purpose Set 'SnoopingAdminMode'
*
* @description [SnoopingAdminMode]: This enables or disables Snooping on the
*              system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSwitchSnoopingGroup_SnoopingAdminMode (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingAdminModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingAdminMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objSnoopingAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnoopingAdminModeValue, owa.len);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbSnoopAdminModeSet (L7_UNIT_CURRENT, 
                                     objSnoopingAdminModeValue,
                                     (xLibU8_t)keySnoopingProtocolValue);
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
* @function fpObjGet_SwitchingSwitchSnoopingGroup_SnoopingPortMask
*
* @purpose Get 'SnoopingPortMask'
*
* @description [SnoopingPortMask]: IGMP/MLD Snooping Port Mask. This allows
*              configuration of IGMP/MLD Snooping on selected ports.IGMP
*              Snooping cannot be enabled on an interface that has routing
*              enabled, or is a member of a LAG. If a port which has IGMP
*              Snooping enabled 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingGroup_SnoopingPortMask (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSnoopingPortMaskValue;
  L7_INTF_MASK_t intfMask;
  L7_INTF_MASK_t conIntfMask;
  xLibU32_t intfList[L7_FILTER_MAX_INTF];
  xLibU32_t numPorts = 0,i,flag=1;
  xLibU32_t length;
  xLibStr256_t stat;  
  FPOBJ_TRACE_ENTER (bufp);
    
  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  memset (&intfMask, 0x0, sizeof (intfMask));
  memset (&conIntfMask, 0x0, sizeof (conIntfMask));
  memset(stat,0x0,sizeof(stat));
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* get the value from application */
  length = NIM_INTF_INDICES ;
  owa.l7rc =
    usmDbSnoopIntfEnabledMaskGet (L7_UNIT_CURRENT, 
                                  (L7_uchar8 *) &intfMask,
                                  &length,
                                  (xLibU8_t)keySnoopingProtocolValue);

   if(owa.l7rc == L7_SUCCESS)
  {
   if((owa.l7rc = usmDbReverseMask(intfMask,&conIntfMask))== L7_SUCCESS)
    {
        memset(objSnoopingPortMaskValue,0,sizeof(objSnoopingPortMaskValue));
        if (usmDbConvertMaskToList (&conIntfMask, intfList, &numPorts) != L7_SUCCESS)
        {
           return XLIBRC_FAILURE;
        }
        if(numPorts !=0)
        {
             for (i = 1; i <= numPorts; i++)
             {
                memset(stat, 0x0, sizeof(stat));
                sprintf(stat, "%d",intfList[i]);
                if(flag == 1)
                {
                    OSAPI_STRNCAT(objSnoopingPortMaskValue,stat);
                    flag =0;
                }
                else
                {
                    OSAPI_STRNCAT(objSnoopingPortMaskValue,",");
                    OSAPI_STRNCAT(objSnoopingPortMaskValue, stat);
                }
              }
 
        }
        else
        {
           owa.l7rc =L7_SUCCESS;
        }

  }
}
  
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopingPortMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSnoopingPortMaskValue,
                           strlen(objSnoopingPortMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingSwitchSnoopingGroup_SnoopingPortMask
*
* @purpose Set 'SnoopingPortMask'
*
* @description [SnoopingPortMask]: IGMP/MLD Snooping Port Mask. This allows
*              configuration of IGMP/MLD Snooping on selected ports.IGMP
*              Snooping cannot be enabled on an interface that has routing
*              enabled, or is a member of a LAG. If a port which has IGMP
*              Snooping enabled 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSwitchSnoopingGroup_SnoopingPortMask (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSnoopingPortMaskValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  xLibU32_t length = sizeof(objSnoopingPortMaskValue);
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SnoopingPortMask */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objSnoopingPortMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objSnoopingPortMaskValue, owa.len);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSnoopIntfEnabledMaskSet (L7_UNIT_CURRENT, 
                                  objSnoopingPortMaskValue,
                                  &length,
                                  (xLibU8_t)keySnoopingProtocolValue);
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
* @function fpObjGet_SwitchingSwitchSnoopingGroup_SnoopingMulticastControlFramesProcessed
*
* @purpose Get 'SnoopingMulticastControlFramesProcessed'
*
* @description [SnoopingMulticastControlFramesProcessed]: Multicast Control
*              Frames Processed by CPU. The number of multicast control
*              frames that have been processed by the CPU. 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_SwitchingSwitchSnoopingGroup_SnoopingMulticastControlFramesProcessed
(void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingMulticastControlFramesProcessedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_SwitchingSwitchSnoopingGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSnoopControlFramesProcessed (L7_UNIT_CURRENT, 
                                      &objSnoopingMulticastControlFramesProcessedValue,
                                      (xLibU8_t)keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnoopingMulticastControlFramesProcessed */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objSnoopingMulticastControlFramesProcessedValue,
                    sizeof (objSnoopingMulticastControlFramesProcessedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
