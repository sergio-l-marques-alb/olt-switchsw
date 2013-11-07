/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseSPM.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  16 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseSPM_obj.h"
#include "usmdb_spm_api.h"
#include "usmdb_util_api.h"
#include "usmdb_nim_api.h"
#include <ctype.h>
#include <stdlib.h>
/* For stack manager unit util routine */
#include "usmdb_unitmgr_api.h"
#include "fpobj_stackingRunifUtils.h"

/* Run if routine for stack port diag Utils */
L7_BOOL usmUtilStackMemberGet()
{
   /* To check whether the unit has any members in stack or not */
   xLibU32_t selected_unit,nxtUnit;

  if(usmDbUnitMgrStackMemberGetFirst(&selected_unit) != L7_SUCCESS)
            return L7_FALSE;

  if(usmDbUnitMgrStackMemberGetNext(selected_unit,&nxtUnit) != L7_SUCCESS)
            return L7_FALSE;

  return L7_TRUE;
} 

/********************************************************************
*
* @purpose  displays stack port diagnostics
*
* @returns  usmWebBigBuf
* @end
* @ notes "RBYT:%x RPKT:%x TBYT:%x TPKT:%x" is the typical format
           refered from broad_hpc_stacking.c
*********************************************************************/
L7_RC_t fpObjUtil_baseSPM_ParseSPMDiagString(L7_uchar8 *infoStr , L7_char8 *searchParam , L7_uchar8 *buf)
 {
  L7_char8 *strPtr , *low , *high;

  if(infoStr == L7_NULL)
    return L7_FAILURE;

  if(buf == L7_NULL)
    return L7_FAILURE;

  if(searchParam == L7_NULL)
    return L7_FAILURE;

  strPtr = (L7_char8 *)strstr(infoStr,searchParam);
 
  if ( strPtr == L7_NULL )
   return L7_FAILURE;

  strPtr = strPtr+strlen(searchParam)+1;

  low = strPtr;
  while(*strPtr!= '\0' && isspace((int )*(strPtr++))==0);
  high = strPtr-1;
  if (*strPtr == '\0')
    memcpy(buf,low,(high-low)+1);
  else
    memcpy(buf,low,(high-low));

  return L7_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_spmIndex
*
* @purpose Get 'spmIndex'
*
* @description Index for a given USP 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_spmIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objspmIndexValue;
  xLibU32_t nextObjspmIndexValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & objspmIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(&temp_entry,0x00,sizeof(temp_entry));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objspmIndexValue, owa.len);
    /* clear data in temp_entry */
   memset(&temp_entry, 0, sizeof(temp_entry));
   temp_entry.snmp_index = objspmIndexValue;
  }
  
   if (usmdbSpmStackingCapablePortNextGet(&temp_entry) == L7_SUCCESS)
   {
      nextObjspmIndexValue = temp_entry.snmp_index;
      owa.l7rc = L7_SUCCESS;
   }
   else
   {
      owa.l7rc = L7_FAILURE;
   }


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjspmIndexValue, owa.len);

  /* return the object value: spmIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjspmIndexValue,
                           sizeof (objspmIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSPM_Unit
*
* @purpose Get 'Unit'
*
* @description Stack port unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_Unit (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  xLibU32_t objUnitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objUnitValue = temp_entry.unit;

  /* return the object value: Unit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitValue,
                           sizeof (objUnitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSPM_Tag
*
* @purpose Get 'Tag'
*
* @description Name of the port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_Tag (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTagValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet ( keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  strcpy(objTagValue,temp_entry.port_tag);

  /* return the object value: Tag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTagValue,
                           strlen (objTagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSPM_ConfiguredStackMode
*
* @purpose Get 'ConfiguredStackMode'
*
* @description config mode of the stack port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_ConfiguredStackMode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfiguredStackModeValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (temp_entry.fps_port == L7_TRUE)  /* Front Panel */
  {
    switch (temp_entry.port_info.config_mode)
    {
      case L7_TRUE:
           objConfiguredStackModeValue = L7_XUI_STACK;
           break;
      case L7_FALSE:
           objConfiguredStackModeValue = L7_XUI_ETHERNET;
           break;
      default:
           objConfiguredStackModeValue = L7_XUI_STACK;
           break;
    }/*End-of-Switch*/
  }
  else
  {
    objConfiguredStackModeValue = L7_XUI_NOT_APPLICABLE;
  }

  /* return the object value: ConfiguredStackMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objConfiguredStackModeValue,
                           sizeof (objConfiguredStackModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSPM_ConfiguredStackMode
*
* @purpose Set 'ConfiguredStackMode'
*
* @description config mode of the stack port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSPM_ConfiguredStackMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfiguredStackModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  xLibU32_t temp_val = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ConfiguredStackMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objConfiguredStackModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objConfiguredStackModeValue, owa.len);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  switch (objConfiguredStackModeValue)
  {
      case L7_XUI_ETHERNET:
        temp_val = L7_FALSE;
        break;
  
      case L7_XUI_STACK:
        temp_val = L7_TRUE;
        break;
  
      default:
        /* unknown case */
        owa.l7rc = L7_FAILURE;
        break;
   }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmdbSpmFpsConfigStackingModeSet (keyspmIndexValue,
                                      temp_val);
  /* Waiting for 2 seconds to update the configured stack mode value before a get is performed immediately. 
    * Keeping wait in get call will lead to wait for a longer period. Instead placing the wait here */
  osapiSleep(2);
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
* @function fpObjGet_baseSPM_RunningStackMode
*
* @purpose Get 'RunningStackMode'
*
* @description runtime mod eof the port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_RunningStackMode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRunningStackModeValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue, &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (temp_entry.fps_port == L7_TRUE)  /* Front Panel */
  {
    switch (temp_entry.port_info.runtime_mode)
    {
      case L7_TRUE:
           objRunningStackModeValue = L7_XUI_STACK;
           break;
      case L7_FALSE:
           objRunningStackModeValue = L7_XUI_ETHERNET;
           break;
      default:
           objRunningStackModeValue = L7_XUI_STACK;
           break;
    }/*End-of-Switch*/
  }
  else
  {
    objRunningStackModeValue = L7_XUI_STACK;
  }

  /* return the object value: RunningStackMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRunningStackModeValue,
                           sizeof (objRunningStackModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSPM_LinkStatus
*
* @purpose Get 'LinkStatus'
*
* @description Link status of th port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_LinkStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLinkStatusValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  L7_uint32 val, intIfNum;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* ETHERNET TYPE */

  if ((L7_TRUE  == temp_entry.fps_port) &&
      (L7_FALSE == temp_entry.port_info.runtime_mode))
  {
    if (L7_SUCCESS == usmDbIntIfNumFromUSPGet(temp_entry.unit, temp_entry.slot, temp_entry.port, &intIfNum))
    {
      if (L7_SUCCESS != usmDbIntfStatusGet(intIfNum, &val))
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      else
      {
        if (L7_INTF_ATTACHED == val)
        {
          if (L7_SUCCESS == usmDbIfOperStatusGet(temp_entry.unit, intIfNum, &val))
          {
            switch (val) /* val = up or down */
            {
              case L7_DOWN:
                objLinkStatusValue = L7_XUI_LINK_DOWN;
                break;
              case L7_UP:
                objLinkStatusValue = L7_XUI_LINK_UP;
                break;
              default:
                objLinkStatusValue = L7_XUI_LINK_DOWN;
                break;
            }
          }
        }
        else
        {
          objLinkStatusValue = L7_XUI_LINK_DOWN;
        }
      }
    }
  }
  else
  {
    switch (temp_entry.port_info.link_status)
    {
      case L7_TRUE:
          objLinkStatusValue = L7_XUI_LINK_UP;
          break;
      case L7_FALSE:
          objLinkStatusValue = L7_XUI_LINK_DOWN;
          break;
      default:
          objLinkStatusValue = L7_XUI_LINK_DOWN;
          break;
    }/*End-of-Switch*/
  }

  /* return the object value: LinkStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLinkStatusValue,
                           sizeof (objLinkStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSPM_LinkSpeed
*
* @purpose Get 'LinkSpeed'
*
* @description Link speed of the port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_LinkSpeed (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objLinkSpeedValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  L7_uint32 intIfNum;
  L7_uint32 val;
  L7_uint32 speed = 0;
 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet ( keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

    if ((L7_TRUE  == temp_entry.fps_port) &&
        (L7_FALSE == temp_entry.port_info.runtime_mode))
    {
      if (L7_SUCCESS == usmDbIntIfNumFromUSPGet(temp_entry.unit, temp_entry.slot, temp_entry.port, &intIfNum))
      {
        if (L7_SUCCESS == usmDbIfSpeedGet(temp_entry.unit, intIfNum, &val))
        {
          switch (val)
          {
            case L7_PORTCTRL_PORTSPEED_HALF_100TX:
            case L7_PORTCTRL_PORTSPEED_FULL_100TX:
            case L7_PORTCTRL_PORTSPEED_FULL_100FX:
              speed = 100;
              break;
            case L7_PORTCTRL_PORTSPEED_HALF_10T:
            case L7_PORTCTRL_PORTSPEED_FULL_10T:
              speed = 10;
              break;
            case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
              speed = 1000;
              break;
            /* PTin added: Speed 2.5G */
            case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
              speed = 2500;
              break;
            /* PTin end */
            case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
              speed = 10000;
              break;
            /* PTin added: Speed 40G */
            case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
              speed = 40000;
              break;
            /* PTin added: Speed 100G */
            case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
              speed = 100000;
              break;
            /* PTin end */
            default:
              break;
          }
          speed /= 1000;
        }
      }
    }
    else
    {
      speed = temp_entry.port_info.speed;
    } 

   if(speed > 0)
   {
    switch(speed)
    {
      case 2:
        osapiSnprintf(objLinkSpeedValue, sizeof(objLinkSpeedValue), "2.5");
        break;
      default:
        osapiSnprintf(objLinkSpeedValue, sizeof(objLinkSpeedValue), "%d", speed);
        break;
    }
   }
   else
   {
     osapiSnprintf(objLinkSpeedValue, sizeof(objLinkSpeedValue), 0);
   }

   /* return the object value: LinkSpeed */
   owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLinkSpeedValue,
                           strlen(objLinkSpeedValue));
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSPM_DataRate
*
* @purpose Get 'DataRate'
*
* @description Data rate of the port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_DataRate (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDataRateValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objDataRateValue = temp_entry.port_info.rx_data_rate + temp_entry.port_info.tx_data_rate;

  /* return the object value: DataRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDataRateValue,
                           sizeof (objDataRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_TxDataRate
*
* @purpose Get 'TxDataRate'
*
* @description Data rate of the port
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_TxDataRate (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTxDataRateValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objTxDataRateValue = temp_entry.port_info.tx_data_rate;

  /* return the object value: DataRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxDataRateValue,
                           sizeof (objTxDataRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_RxDataRate
*
* @purpose Get 'RxDataRate'
*
* @description Data rate of the port
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_RxDataRate (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRxDataRateValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objRxDataRateValue = temp_entry.port_info.rx_data_rate;

  /* return the object value: DataRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxDataRateValue,
                           sizeof (objRxDataRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_ErrorRate
*
* @purpose Get 'ErrorRate'
*
* @description Error rate of the port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_ErrorRate (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objErrorRateValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet ( keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objErrorRateValue = temp_entry.port_info.rx_error_rate + temp_entry.port_info.tx_error_rate;

  /* return the object value: ErrorRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objErrorRateValue,
                           sizeof (objErrorRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_TxErrorRate
*
* @purpose Get 'TxErrorRate'
*
* @description Error rate of the port
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_TxErrorRate (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTxErrorRateValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet ( keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objTxErrorRateValue = temp_entry.port_info.tx_error_rate;

  /* return the object value: ErrorRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxErrorRateValue,
                           sizeof (objTxErrorRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_RxErrorRate
*
* @purpose Get 'RxErrorRate'
*
* @description Error rate of the port
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_RxErrorRate (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRxErrorRateValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet ( keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objRxErrorRateValue = temp_entry.port_info.rx_error_rate;

  /* return the object value: ErrorRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxErrorRateValue,
                           sizeof (objRxErrorRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_TotalErrors
*
* @purpose Get 'TotalErrors'
*
* @description Total number of errors 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_TotalErrors (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTotalErrorsValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objTotalErrorsValue = temp_entry.port_info.rx_total_errors + temp_entry.port_info.tx_total_errors;

  /* return the object value: TotalErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTotalErrorsValue,
                           sizeof (objTotalErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_TxTotalErrors
*
* @purpose Get 'TxTotalErrors'
*
* @description Total number of errors
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_TxTotalErrors (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTxTotalErrorsValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objTxTotalErrorsValue = temp_entry.port_info.tx_total_errors;

  /* return the object value: TotalErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxTotalErrorsValue,
                           sizeof (objTxTotalErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_RxTotalErrors
*
* @purpose Get 'RxTotalErrors'
*
* @description Total number of errors
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_RxTotalErrors (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRxTotalErrorsValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objRxTotalErrorsValue = temp_entry.port_info.rx_total_errors;

  /* return the object value: TotalErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxTotalErrorsValue,
                           sizeof (objRxTotalErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_ConfigQosMode
*
* @purpose Get 'ConfigQosMode'
*
* @description Configurability of QOS Mode 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_ConfigQosMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfigQosModeValue;
  xLibU32_t temp_val = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbSpmFpsConfigQosModeGet ( &temp_val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objConfigQosModeValue,
                     sizeof (objConfigQosModeValue));
  switch(temp_val)
  {
     case L7_TRUE:
   objConfigQosModeValue = L7_ENABLE;
   break;
     case L7_FALSE:
   objConfigQosModeValue = L7_DISABLE;
   break;
     default:
   owa.l7rc  = L7_FAILURE;
  break;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ConfigQosMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objConfigQosModeValue,
                           sizeof (objConfigQosModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSPM_ConfigQosMode
*
* @purpose Set 'ConfigQosMode'
*
* @description Configurability of QOS Mode 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSPM_ConfigQosMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfigQosModeValue;
  L7_uint32 temp_val = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ConfigQosMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objConfigQosModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objConfigQosModeValue, owa.len);

  switch (objConfigQosModeValue)
  {
  case L7_ENABLE:
    temp_val = L7_TRUE;
    break;

  case L7_DISABLE:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    owa.l7rc = L7_FAILURE;
    break;
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmdbSpmFpsConfigQosModeSet (temp_val);
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
* @function fpObjGet_baseSPM_info1
*
* @purpose Get 'info1'
*
* @description 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_info1 (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objinfo1Value;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  strcpy(objinfo1Value,temp_entry.port_info.info1);

  /* return the object value: TotalErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinfo1Value,
                           sizeof (objinfo1Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_info2
*
* @purpose Get 'info2'
*
* @description
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_info2 (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objinfo2Value;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  strcpy(objinfo2Value, temp_entry.port_info.info2);

  /* return the object value: TotalErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinfo2Value,
                           sizeof (objinfo2Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_info3
*
* @purpose Get 'info3'
*
* @description
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_info3 (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objinfo3Value;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  strcpy(objinfo3Value, temp_entry.port_info.info3);

  /* return the object value: TotalErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objinfo3Value,
                           sizeof (objinfo3Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_baseSPM_StackIndex
*
* @purpose Get 'StackIndex`
*
* @description UnitID of the StackIndex
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_StackIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStackMemberValue;
  xLibU32_t nextObjStackMemberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StackMember */
  owa.rc = xLibFilterGet (wap, XOBJ_baseSPM_StackIndex,
                          (xLibU8_t *) & objStackMemberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjStackMemberValue = 0;
    objStackMemberValue = 0;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objStackMemberValue, owa.len);
  owa.l7rc = usmDbUnitMgrStackMemberGetNext (objStackMemberValue,
                                             &nextObjStackMemberValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjStackMemberValue, owa.len);

  /* return the object value: StackMember */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjStackMemberValue,
                           sizeof (objStackMemberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_spmStackIndex
*
* @purpose Get 'spmStackIndex'
*
* @description Index for a given USP 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_spmStackIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objspmIndexValue;
  xLibU32_t nextObjspmIndexValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t objStackMemberValue;

  /* retrieve key: objStackMemberValue */
  owa.rc = xLibFilterGet (wap, XOBJ_baseSPM_StackIndex,
                          (xLibU8_t *) & objStackMemberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* retrieve key: spmIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & objspmIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(&temp_entry,0x00,sizeof(temp_entry));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objspmIndexValue, owa.len);
    /* clear data in temp_entry */
   memset(&temp_entry, 0, sizeof(temp_entry));
   temp_entry.snmp_index = objspmIndexValue;
  }
  
   owa.l7rc = usmdbSpmStackingCapablePortNextGet(&temp_entry);
 
  /* Loop through all the interfaces and skip interfaces other than Stacking */
   while(owa.l7rc == L7_SUCCESS)
  {
   if((L7_TRUE  == temp_entry.fps_port) &&
        (L7_FALSE == temp_entry.port_info.runtime_mode))
   {
      nextObjspmIndexValue = temp_entry.snmp_index;
   }
   else
   {
      nextObjspmIndexValue = temp_entry.snmp_index;
      break;
   }
   owa.l7rc = usmdbSpmStackingCapablePortNextGet(&temp_entry); 
  }
  #if 0
  /* Return E_O_T if it is another stack member */ 
  if ((owa.l7rc != L7_SUCCESS) ||(objStackMemberValue != temp_entry.unit)) 
  {
  #endif
  if (owa.l7rc != L7_SUCCESS) 
  { 
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjspmIndexValue, owa.len);

  /* return the object value: spmIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjspmIndexValue,
                           sizeof (objspmIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_spmStackUnit
*
* @purpose Get 'spmStackUnit'
*
* @description Stack port unit 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_spmStackUnit (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  xLibU32_t objUnitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objUnitValue = temp_entry.unit;

  /* return the object value: Unit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnitValue,
                           sizeof (objUnitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseSPM_RBYT
*
* @purpose Get 'RBYT'
 *@description  [RBYT] <HTML>Received Bytes for a given stack port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_RBYT (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objValue;

  memset(&temp_entry,0x00,sizeof(temp_entry));
  memset(objValue,0x00,sizeof(objValue));

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Call the parsing utility routine to parse and get the actual object */
  if(strcmp(temp_entry.port_info.info1,"") != 0)
  {
    owa.l7rc = fpObjUtil_baseSPM_ParseSPMDiagString (temp_entry.port_info.info1,
                                        "RBYT",objValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }

  FPOBJ_TRACE_VALUE (bufp, &objValue, sizeof (objValue));

  /* return the object value: RBYT */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objValue, strlen (objValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSPM_RPKT
*
* @purpose Get 'RPKT'
 *@description  [RPKT] <HTML>Received Packets of the given stack port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_RPKT (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objValue;

  memset(&temp_entry,0x00,sizeof(temp_entry));
  memset(objValue,0x00,sizeof(objValue));

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Call the parsing utility routine to parse and get the actual object */
  if(strcmp(temp_entry.port_info.info1,"") != 0)
  {
    owa.l7rc = fpObjUtil_baseSPM_ParseSPMDiagString (temp_entry.port_info.info1,
                                        "RPKT",objValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }

  FPOBJ_TRACE_VALUE (bufp, &objValue, sizeof (objValue));

  /* return the object value: RBYT */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objValue, strlen (objValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSPM_TBYT
*
* @purpose Get 'TBYT'
 *@description  [TBYT] <HTML>Transmitted Bytes of the given stack port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_TBYT (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objValue;

  memset(&temp_entry,0x00,sizeof(temp_entry));
  memset(objValue,0x00,sizeof(objValue));

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Call the parsing utility routine to parse and get the actual object */
  if(strcmp(temp_entry.port_info.info1,"") != 0)
  {
    owa.l7rc = fpObjUtil_baseSPM_ParseSPMDiagString (temp_entry.port_info.info1,
                                        "TBYT",objValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }

  FPOBJ_TRACE_VALUE (bufp, &objValue, sizeof (objValue));

  /* return the object value: RBYT */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objValue, strlen (objValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_TPKT
*
* @purpose Get 'TPKT'
 *@description  [TPKT] <HTML>Transmitted packets for a given stack port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_TPKT (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objValue;

  memset(&temp_entry,0x00,sizeof(temp_entry));
  memset(objValue,0x00,sizeof(objValue));

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Call the parsing utility routine to parse and get the actual object */
  if(strcmp(temp_entry.port_info.info1,"") != 0)
  {
    owa.l7rc = fpObjUtil_baseSPM_ParseSPMDiagString (temp_entry.port_info.info1,
                                        "TPKT",objValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }

  FPOBJ_TRACE_VALUE (bufp, &objValue, sizeof (objValue));

  /* return the object value: RBYT */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objValue, strlen (objValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_RFCS
*
* @purpose Get 'RFCS'
 *@description  [RFCS] <HTML>Received Frame Check Sequence Errors   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_RFCS (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objValue;

  memset(&temp_entry,0x00,sizeof(temp_entry));
  memset(objValue,0x00,sizeof(objValue));

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Call the parsing utility routine to parse and get the actual object */
  if(strcmp(temp_entry.port_info.info2,"") != 0)
  {
    owa.l7rc = fpObjUtil_baseSPM_ParseSPMDiagString (temp_entry.port_info.info2,
                                        "RFCS",objValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }

  FPOBJ_TRACE_VALUE (bufp, &objValue, sizeof (objValue));

  /* return the object value: RBYT */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objValue, strlen (objValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_RFRG
*
* @purpose Get 'RFRG'
 *@description  [RFRG] <HTML>Received Fragment Errors   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_RFRG (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objValue;

  memset(&temp_entry,0x00,sizeof(temp_entry));
  memset(objValue,0x00,sizeof(objValue));

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Call the parsing utility routine to parse and get the actual object */
  if(strcmp(temp_entry.port_info.info2,"") != 0)
  {
    owa.l7rc = fpObjUtil_baseSPM_ParseSPMDiagString (temp_entry.port_info.info2,
                                        "RFRG",objValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }

  FPOBJ_TRACE_VALUE (bufp, &objValue, sizeof (objValue));

  /* return the object value: RBYT */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objValue, strlen (objValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_RJBR
*
* @purpose Get 'RJBR'
 *@description  [RJBR] <HTML>Received Jabber Errors   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_RJBR (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objValue;

  memset(&temp_entry,0x00,sizeof(temp_entry));
  memset(objValue,0x00,sizeof(objValue));

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Call the parsing utility routine to parse and get the actual object */
  if(strcmp(temp_entry.port_info.info2,"") != 0)
  {
    owa.l7rc = fpObjUtil_baseSPM_ParseSPMDiagString (temp_entry.port_info.info2,
                                        "RJBR",objValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }

  FPOBJ_TRACE_VALUE (bufp, &objValue, sizeof (objValue));

  /* return the object value: RBYT */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objValue, strlen (objValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_RUND
*
* @purpose Get 'RUND'
 *@description  [RUND] <HTML>Received Under Run Errors   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_RUND (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objValue;

  memset(&temp_entry,0x00,sizeof(temp_entry));
  memset(objValue,0x00,sizeof(objValue));

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Call the parsing utility routine to parse and get the actual object */
  if(strcmp(temp_entry.port_info.info2,"") != 0)
  {
    owa.l7rc = fpObjUtil_baseSPM_ParseSPMDiagString (temp_entry.port_info.info2,
                                        "RUND",objValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }

  FPOBJ_TRACE_VALUE (bufp, &objValue, sizeof (objValue));

  /* return the object value: RBYT */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objValue, strlen (objValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_ROVR
*
* @purpose Get 'ROVR'
 *@description  [ROVR] <HTML>Received Overrun Errors   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_ROVR (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objValue;

  memset(&temp_entry,0x00,sizeof(temp_entry));
  memset(objValue,0x00,sizeof(objValue));

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Call the parsing utility routine to parse and get the actual object */
  if(strcmp(temp_entry.port_info.info2,"") != 0)
  {
    owa.l7rc = fpObjUtil_baseSPM_ParseSPMDiagString (temp_entry.port_info.info2,
                                        "ROVR",objValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }

  FPOBJ_TRACE_VALUE (bufp, &objValue, sizeof (objValue));

  /* return the object value: RBYT */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objValue, strlen (objValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_TFCS
*
* @purpose Get 'TFCS'
 *@description  [TFCS] <HTML>Tranmit Frame Check Sequence Errors   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_TFCS (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objValue;

  memset(&temp_entry,0x00,sizeof(temp_entry));
  memset(objValue,0x00,sizeof(objValue));

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Call the parsing utility routine to parse and get the actual object */
  if(strcmp(temp_entry.port_info.info3,"") != 0)
  {
    owa.l7rc = fpObjUtil_baseSPM_ParseSPMDiagString (temp_entry.port_info.info3,
                                        "TFCS",objValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }

  FPOBJ_TRACE_VALUE (bufp, &objValue, sizeof (objValue));

  /* return the object value: RBYT */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objValue, strlen (objValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_TERR
*
* @purpose Get 'TERR'
 *@description  [TERR] <HTML>Transmit Errors   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_TERR (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyspmIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t objValue;

  memset(&temp_entry,0x00,sizeof(temp_entry));
  memset(objValue,0x00,sizeof(objValue));

  /* retrieve key: spmIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseSPM_spmStackIndex,
                          (xLibU8_t *) & keyspmIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyspmIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbSpmStackingCapablePortGet (keyspmIndexValue,
                                             &temp_entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Call the parsing utility routine to parse and get the actual object */
  if(strcmp(temp_entry.port_info.info3,"") != 0)
  {
    owa.l7rc = fpObjUtil_baseSPM_ParseSPMDiagString (temp_entry.port_info.info3,
                                        "TERR",objValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }

  FPOBJ_TRACE_VALUE (bufp, &objValue, sizeof (objValue));

  /* return the object value: RBYT */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objValue, strlen (objValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseSPM_stackPortIndex
*
* @purpose Get 'stackPortIndex'
*
* @description Index for a given USP 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSPM_stackPortIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objstackPortIndexValue;
  xLibU32_t nextObjstackPortIndexValue;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: spmIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseSPM_stackPortIndex,
                          (xLibU8_t *) & objstackPortIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(&temp_entry,0x00,sizeof(temp_entry));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objstackPortIndexValue, owa.len);
    /* clear data in temp_entry */
   memset(&temp_entry, 0, sizeof(temp_entry));
   temp_entry.snmp_index = objstackPortIndexValue;
  }
  
   do
   {
     if (usmdbSpmStackingCapablePortNextGet(&temp_entry) == L7_SUCCESS)
     {
        nextObjstackPortIndexValue = temp_entry.snmp_index;
        owa.l7rc = L7_SUCCESS;
     }
     else
     {
        owa.l7rc = L7_FAILURE;
     }
   }while ((temp_entry.port_info.runtime_mode != L7_XUI_STACK) && (owa.l7rc == L7_SUCCESS));
   
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjstackPortIndexValue, owa.len);

  /* return the object value: spmIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjstackPortIndexValue,
                           sizeof (objstackPortIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

