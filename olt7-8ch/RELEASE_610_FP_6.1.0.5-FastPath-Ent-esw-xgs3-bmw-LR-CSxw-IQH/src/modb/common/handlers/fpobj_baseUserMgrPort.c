/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseUserMgrPort.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to common-object.xml
*
* @create  9 April 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseUserMgrPort_obj.h"
#include "usmdb_port_user.h"
#include "usmdb_util_api.h"
#include "usmdb_user_mgmt_api.h"
/*******************************************************************************
* @function fpObjGet_baseUserMgrPort_ifIndex
*
* @purpose Get 'ifIndex'
 *@description  [ifIndex] interfaceIndex.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUserMgrPort_ifIndex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseUserMgrPort_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objifIndexValue = 0;
    owa.l7rc =  usmDbGetNextPhysicalIntIfNumber(objifIndexValue, &nextObjifIndexValue);
     
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
    owa.l7rc =  usmDbGetNextPhysicalIntIfNumber(objifIndexValue, &nextObjifIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjifIndexValue, owa.len);

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue, sizeof (nextObjifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUserMgrPort_portUserName
*
* @purpose Get 'portUserName'
 *@description  [portUserName] the name of the first user with access to the
* port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseUserMgrPort_portUserName (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  xLibU32_t index = 0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objportUserNameValue;
  xLibStr256_t nextObjportUserNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  memset (objportUserNameValue,0,sizeof(objportUserNameValue));

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_baseUserMgrPort_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: portUserName */
  owa.rc = xLibFilterGet (wap, XOBJ_baseUserMgrPort_portUserName,
                          (xLibU8_t *) objportUserNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.l7rc = usmDbLoginsGet (L7_UNIT_CURRENT, index, nextObjportUserNameValue);
  }
  
  else
  {

    owa1.l7rc = usmDbUserLoginIndexGet(L7_UNIT_CURRENT,
                                        objportUserNameValue , &index);
    if ( owa1.l7rc != L7_SUCCESS)
   {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
   }
   else
   {
    owa.l7rc = usmDbLoginsGet (L7_UNIT_CURRENT, ++index, nextObjportUserNameValue);
   }

  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjportUserNameValue, owa.len);

  /* return the object value: portUserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjportUserNameValue,
                           strlen (nextObjportUserNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
 
}

xLibRC_t fpObjSet_baseUserMgrPort_portUserName (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
}
/*get the list of all configured users*/

xLibRC_t fpObjGet_baseUserMgrPort_portUserName (void *wap, void *bufp)
{

#define REM_STR_LEN(a) (sizeof(a)-strlen(a))
  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objportUserNameValue,concatString;
  xLibStr256_t nextObjportUserNameValue;
  xLibU32_t flag;
  FPOBJ_TRACE_ENTER (bufp);

  memset (objportUserNameValue,0,sizeof(objportUserNameValue));
  memset (nextObjportUserNameValue,0,sizeof(nextObjportUserNameValue));
  memset (concatString,0,sizeof(concatString));

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_baseUserMgrPort_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  if (kwaifIndex.rc == XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc=usmDbPortUserGetFirst (L7_UNIT_CURRENT, keyifIndexValue, nextObjportUserNameValue);
    while(owa.l7rc == L7_SUCCESS) 
    {
       owa1.rc = usmDbPortUserAccessGet(L7_UNIT_CURRENT, keyifIndexValue, nextObjportUserNameValue, &flag );
       if(owa1.rc == L7_SUCCESS && flag == L7_TRUE)
       {
        osapiStrncat(concatString,nextObjportUserNameValue, REM_STR_LEN(concatString));
        osapiStrncat(concatString,",",REM_STR_LEN(concatString));
        owa.l7rc = L7_SUCCESS;
       }
       osapiStrncpySafe(objportUserNameValue, nextObjportUserNameValue, sizeof(objportUserNameValue));
       owa.l7rc = usmDbPortUserGetNext (L7_UNIT_CURRENT,keyifIndexValue,
                     objportUserNameValue,nextObjportUserNameValue);
    }
  }
  else
  {
  owa.rc = XLIBRC_ENDOF_TABLE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, concatString, owa.len);

 /*  return the object value: portUserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)concatString,
                           sizeof(concatString));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*get the list of all configured users this is only for displaying users as a tabular form*/

xLibRC_t fpObjGet_baseUserMgrPort_portUserNameSummary (void *wap, void *bufp)
{

#define REM_STR_LEN(a) (sizeof(a)-strlen(a))
  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objportUserNameValue,concatString;
  xLibStr256_t nextObjportUserNameValue;
  xLibU32_t flag;
  FPOBJ_TRACE_ENTER (bufp);


  memset (objportUserNameValue,0,sizeof(objportUserNameValue));
  memset (nextObjportUserNameValue,0,sizeof(nextObjportUserNameValue));
  memset (concatString,0,sizeof(concatString));

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_baseUserMgrPort_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  if (kwaifIndex.rc == XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc=usmDbPortUserGetFirst (L7_UNIT_CURRENT, keyifIndexValue, nextObjportUserNameValue);
    while(owa.l7rc == L7_SUCCESS)
    {
       owa1.rc = usmDbPortUserAccessGet(L7_UNIT_CURRENT, keyifIndexValue, nextObjportUserNameValue, &flag );
       if(owa1.rc == L7_SUCCESS && flag == L7_TRUE)
       {
        
        osapiStrncat(concatString,nextObjportUserNameValue,REM_STR_LEN(concatString));
        osapiStrncat(concatString,"<br>", REM_STR_LEN(concatString));
        owa.l7rc = L7_SUCCESS;
       }
       osapiStrncpySafe(objportUserNameValue, nextObjportUserNameValue, sizeof(objportUserNameValue));
       owa.l7rc = usmDbPortUserGetNext (L7_UNIT_CURRENT,keyifIndexValue,
                     objportUserNameValue,nextObjportUserNameValue);
    }
  }
  else
  {
  owa.rc = XLIBRC_ENDOF_TABLE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, concatString, owa.len);

 /*  return the object value: portUserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)concatString,
                                             sizeof(concatString));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


  


/*******************************************************************************
* @function fpObjGet_baseUserMgrPort_portUserAccess
*
* @purpose Get 'portUserAccess'
* @description  [portUserAccess] the port access for the specified user on the
* specified port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUserMgrPort_portUserAccess (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaportUserName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyportUserNameValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objportUserAccessValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_baseUserMgrPort_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: portUserName */
  kwaportUserName.rc = xLibFilterGet (wap, XOBJ_baseUserMgrPort_portUserName,
                                      (xLibU8_t *) keyportUserNameValue, &kwaportUserName.len);
  if (kwaportUserName.rc != XLIBRC_SUCCESS)
  {
    kwaportUserName.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaportUserName);
    return kwaportUserName.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyportUserNameValue, kwaportUserName.len);

  /* get the value from application */
 
  owa.l7rc = usmDbPortUserAccessGet (L7_UNIT_CURRENT, keyifIndexValue,
                              keyportUserNameValue, &objportUserAccessValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: portUserAccess */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objportUserAccessValue,
                           sizeof (objportUserAccessValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUserMgrPort_portUserAccess
*
* @purpose Set 'portUserAccess'
* @description  [portUserAccess] the port access for the specified user on the
* specified port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUserMgrPort_portUserAccess (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objportUserAccessValue = 1;
  xLibU32_t index = 0, status;
  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwaportUserName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyportUserNameValue;
  xLibS8_t *buf = NULL;
  xLibStr256_t  nextObjportUserNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: portUserAccess */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objportUserAccessValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objportUserAccessValue, owa.len);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_baseUserMgrPort_ifIndex,(xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);

  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* retrieve key: portUserName */
  kwaportUserName.rc = xLibFilterGet (wap, XOBJ_baseUserMgrPort_portUserName,
                                      (xLibU8_t *) keyportUserNameValue, &kwaportUserName.len);
  if (kwaportUserName.rc != XLIBRC_SUCCESS)
  {
    kwaportUserName.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaportUserName);
    return kwaportUserName.rc;
  }
   
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyportUserNameValue, kwaportUserName.len);
  if(keyifIndexValue > 0 )
  {

    for(index=0; index<L7_MAX_LOGINS; index++)
 
    { 
      memset(nextObjportUserNameValue,0x00,sizeof(nextObjportUserNameValue));  
      owa.l7rc = usmDbLoginStatusGet(L7_UNIT_CURRENT, index, &status);
      if (status != L7_ENABLE || usmDbLoginsGet(L7_UNIT_CURRENT,
                                                index, nextObjportUserNameValue) != L7_SUCCESS)
      {
        continue;
      }
      /*find the string*/
      buf =  strstr(keyportUserNameValue, nextObjportUserNameValue);
      if (buf != NULL)

      {
        objportUserAccessValue = L7_TRUE;
      }
      else
     {
      objportUserAccessValue = L7_FALSE;
     }
     owa1.l7rc = usmDbPortUserAccessSet (L7_UNIT_CURRENT, keyifIndexValue,
                                        nextObjportUserNameValue, objportUserAccessValue);
     if(owa1.l7rc != L7_SUCCESS)
        owa.rc = XLIBRC_FAILURE;

   }
 }
 else
 {
   owa.rc = XLIBRC_UNKNOWN;
      
 }
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseUserMgrPort_portUserAccessGlobal
*
* @purpose Set 'portUserAccess'
*@description  [portUserAccess] the port access for the specified user on the
* all ports
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUserMgrPort_portUserAccessGlobal (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objportUserAccessValue = 1;
  xLibU32_t index = 0;
  fpObjWa_t kwaportUserName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyportUserNameValue;
  xLibS8_t *buf = NULL;
  xLibStr256_t  nextObjportUserNameValue;
  L7_RC_t rc= L7_FAILURE;
  xLibU32_t prevIntf, intIfNum, status;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: portUserAccess */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objportUserAccessValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objportUserAccessValue, owa.len);

  /* retrieve key: portUserName */
  kwaportUserName.rc = xLibFilterGet (wap, XOBJ_baseUserMgrPort_portUserName,
                                      (xLibU8_t *) keyportUserNameValue, &kwaportUserName.len);
  if (kwaportUserName.rc != XLIBRC_SUCCESS)
  {
    kwaportUserName.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaportUserName);
    return kwaportUserName.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyportUserNameValue, kwaportUserName.len);
  /* apply users for all ports */
  {
    rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF, 0,
                                  &intIfNum);
    while (rc == L7_SUCCESS)
    {
      for(index=0; index<L7_MAX_LOGINS; index++)

      {
        memset(nextObjportUserNameValue,0x00,sizeof(nextObjportUserNameValue));
        owa.l7rc = usmDbLoginStatusGet(L7_UNIT_CURRENT, index, &status);
        if (status != L7_ENABLE || usmDbLoginsGet(L7_UNIT_CURRENT,
                                                index, nextObjportUserNameValue) != L7_SUCCESS)
        {
          continue;
        }

        if (owa.l7rc!=L7_SUCCESS)
        {
           break;
        }
        /*find the string*/
        buf =  strstr(keyportUserNameValue, nextObjportUserNameValue);
        if (buf != NULL)

        {
          objportUserAccessValue = L7_TRUE;
        }
        else
        {
          objportUserAccessValue = L7_FALSE;
        }
        owa1.l7rc = usmDbPortUserAccessSet (L7_UNIT_CURRENT, intIfNum,
                                        nextObjportUserNameValue, objportUserAccessValue);
        if(owa1.l7rc != L7_SUCCESS)
          owa.rc = XLIBRC_FAILURE;

      }
      prevIntf = intIfNum;
      rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT,  USM_PHYSICAL_INTF, 0,
                                      prevIntf, &intIfNum);
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
