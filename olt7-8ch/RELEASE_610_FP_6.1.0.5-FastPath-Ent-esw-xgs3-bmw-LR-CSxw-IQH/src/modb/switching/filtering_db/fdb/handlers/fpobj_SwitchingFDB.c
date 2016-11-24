
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingFDB.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  26 May 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingFDB_obj.h"
#include "usmdb_mib_bridge_api.h"
#include "fdb_exports.h"
#include "usmdb_common.h"


xLibRC_t fpObjGet_SwitchingFDB_FdbFilter (void *wap, void *bufp)
{
  xLibU32_t objFdbFilter;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof(xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
   
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFDB_FdbFilter, (xLibU8_t *)&objFdbFilter, &owa.len);
  if( owa.rc != XLIBRC_SUCCESS )
  {
    objFdbFilter = 0;
  }
  else
  {
    if(objFdbFilter)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    objFdbFilter = 1; /* Get all */
  } 
  /* return the object value: FdbFilter */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objFdbFilter, sizeof(objFdbFilter));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjGet_SwitchingFDB_FdbMac (void *wap, void *bufp)
{

  xLibStr256_t objFdbMacValue;
  xLibStr256_t nextObjFdbMacValue;
  xLibU32_t keyObjFdbFilter = 0;
  fpObjWa_t owaFdbFilter = FPOBJ_INIT_WA (sizeof(xLibU32_t));
  usmdbFdbEntry_t entry;
  fpObjWa_t owa = FPOBJ_INIT_WA (L7_FDB_KEY_SIZE);
  xLibU32_t flag = 0;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objFdbMacValue, 0x00, sizeof(objFdbMacValue));
  memset(nextObjFdbMacValue, 0x00, sizeof(nextObjFdbMacValue));

  owaFdbFilter.rc = xLibFilterGet (wap, XOBJ_SwitchingFDB_FdbFilter, (xLibU8_t *)&keyObjFdbFilter , &owaFdbFilter.len);
  /* retrieve key: FdbMac */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFDB_FdbMac, (xLibU8_t *) objFdbMacValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(&entry, 0, sizeof(entry));	
    do
    {
      flag = 0;
      owa.l7rc = usmDbFDBEntryNextGet(L7_UNIT_CURRENT, &entry);
      if(keyObjFdbFilter)
      {
        if(entry.usmdbFdbEntryType != L7_FDB_ADDR_FLAG_LEARNED)
            flag = 1;
        memset(&entry.usmdbFdbIntIfNum, 0, sizeof(entry.usmdbFdbIntIfNum));
        memset(&entry.usmdbFdbEntryType, 0, sizeof(entry.usmdbFdbEntryType));

      } 
    }while(flag &&(owa.l7rc == L7_SUCCESS));  
		
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objFdbMacValue, owa.len);
    memcpy(entry.usmdbFdbMac, objFdbMacValue, L7_FDB_KEY_SIZE);
    memset(&entry.usmdbFdbIntIfNum, 0, sizeof(entry.usmdbFdbIntIfNum));
    memset(&entry.usmdbFdbEntryType, 0, sizeof(entry.usmdbFdbEntryType));
    do
    {
      flag = 0;
      owa.l7rc = usmDbFDBEntryNextGet(L7_UNIT_CURRENT, &entry);
      if(keyObjFdbFilter)
      {
        if(entry.usmdbFdbEntryType != L7_FDB_ADDR_FLAG_LEARNED)
            flag = 1;
        memset(&entry.usmdbFdbIntIfNum, 0, sizeof(entry.usmdbFdbIntIfNum));
        memset(&entry.usmdbFdbEntryType, 0, sizeof(entry.usmdbFdbEntryType));

      } 
    }while(flag && (owa.l7rc == L7_SUCCESS));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memcpy(nextObjFdbMacValue, entry.usmdbFdbMac, L7_FDB_KEY_SIZE);

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjFdbMacValue, owa.len);

  /* return the object value: FdbMac */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjFdbMacValue, L7_FDB_KEY_SIZE);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFDB_FdbIntIfNum
*
* @purpose Get 'FdbIntIfNum'
 *@description  [FdbIntIfNum] Entry from the forwarding database   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFDB_FdbIntIfNum (void *wap, void *bufp)
{

  xLibU32_t objFdbIntIfNumValue;
  usmdbFdbEntry_t entry;


  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  char kwaFdbMacValue[L7_FDB_KEY_SIZE];
  fpObjWa_t kwa = FPOBJ_INIT_WA (L7_FDB_KEY_SIZE);
  FPOBJ_TRACE_ENTER (bufp);
  memset(kwaFdbMacValue, 0x00, sizeof(kwaFdbMacValue));

  /* retrieve key: FdbMac */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingFDB_FdbMac, (xLibU8_t *) kwaFdbMacValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  memcpy(entry.usmdbFdbMac, kwaFdbMacValue, L7_FDB_KEY_SIZE); 
  memset(&entry.usmdbFdbIntIfNum, 0, sizeof(entry.usmdbFdbIntIfNum));
  memset(&entry.usmdbFdbEntryType, 0, sizeof(entry.usmdbFdbEntryType));

  FPOBJ_TRACE_CURRENT_KEY (bufp, kwaFdbMacValue, kwa.len);

  /* retrieve key: FdbIntIfNum */
  owa.l7rc = usmDbFDBEntryGet(L7_UNIT_CURRENT, &entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objFdbIntIfNumValue = entry.usmdbFdbIntIfNum;


  FPOBJ_TRACE_NEW_KEY (bufp, &objFdbIntIfNumValue, sizeof(objFdbIntIfNumValue));

  /* return the object value: FdbIntIfNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFdbIntIfNumValue,
                           sizeof (objFdbIntIfNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFDB_FdbIfIndex
*
* @purpose Get 'FdbIfIndex'
 *@description  [FdbIfIndex] Entry from the forwarding database   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFDB_FdbIfIndex (void *wap, void *bufp)
{

  xLibU32_t objFdbIfIndexValue;
  usmdbFdbEntry_t entry;


  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  char kwaFdbMacValue[L7_FDB_KEY_SIZE];
  fpObjWa_t kwa = FPOBJ_INIT_WA (L7_FDB_KEY_SIZE);
  FPOBJ_TRACE_ENTER (bufp);
  memset(kwaFdbMacValue, 0x00, sizeof(kwaFdbMacValue));

  /* retrieve key: FdbMac */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingFDB_FdbMac, (xLibU8_t *) kwaFdbMacValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  memcpy(entry.usmdbFdbMac, kwaFdbMacValue, L7_FDB_KEY_SIZE); 
  memset(&entry.usmdbFdbIntIfNum, 0, sizeof(entry.usmdbFdbIntIfNum));
  memset(&entry.usmdbFdbEntryType, 0, sizeof(entry.usmdbFdbEntryType));

  FPOBJ_TRACE_CURRENT_KEY (bufp, kwaFdbMacValue, kwa.len);

  /* retrieve key: FdbIfIndex */
  owa.l7rc = usmDbFDBEntryGet(L7_UNIT_CURRENT, &entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objFdbIfIndexValue = entry.usmdbFdbIntIfNum;


  FPOBJ_TRACE_NEW_KEY (bufp, &objFdbIfIndexValue, sizeof(objFdbIfIndexValue));

  /* return the object value: FdbIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFdbIfIndexValue,
                           sizeof (objFdbIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFDB_FdbEntryType
*
* @purpose Get 'FdbEntryType'
 *@description  [FdbEntryType] Entry from the forwarding database   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFDB_FdbEntryType (void *wap, void *bufp)
{

  usmdbFdbEntry_t entry;

  xLibStr256_t objFdbEntryTypeValue={0};
	
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  char kwaFdbMacValue[L7_FDB_KEY_SIZE];
  fpObjWa_t kwa = FPOBJ_INIT_WA (L7_FDB_KEY_SIZE);
  FPOBJ_TRACE_ENTER (bufp);

  memset(kwaFdbMacValue, 0x00, sizeof(kwaFdbMacValue));
	

  /* retrieve key: FdbMac */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingFDB_FdbMac, (xLibU8_t *) kwaFdbMacValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  memcpy(entry.usmdbFdbMac, kwaFdbMacValue,L7_FDB_KEY_SIZE); 
  memset(&entry.usmdbFdbIntIfNum, 0, sizeof(entry.usmdbFdbIntIfNum));
  memset(&entry.usmdbFdbEntryType, 0, sizeof(entry.usmdbFdbEntryType));

  FPOBJ_TRACE_CURRENT_KEY (bufp, kwaFdbMacValue, kwa.len);

  /* retrieve key: FdbEntryType */
  owa.l7rc = usmDbFDBEntryGet(L7_UNIT_CURRENT, &entry);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  switch (entry.usmdbFdbEntryType)
  {
    case L7_FDB_ADDR_FLAG_STATIC:
        strcpy(objFdbEntryTypeValue, "Static");
        break;
    case L7_FDB_ADDR_FLAG_LEARNED:
        strcpy(objFdbEntryTypeValue, "Learned");
        break;
    case L7_FDB_ADDR_FLAG_SELF:
        strcpy(objFdbEntryTypeValue, "Self");
        break;
    case L7_FDB_ADDR_FLAG_MANAGEMENT:
        strcpy(objFdbEntryTypeValue, "Management");
        break;
    case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
        strcpy(objFdbEntryTypeValue, "GMRP Learned");
        break;
    default:
        strcpy(objFdbEntryTypeValue, "Other");
        break;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objFdbEntryTypeValue, owa.len);

  /* return the object value: FdbEntryType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objFdbEntryTypeValue,
                           strlen (objFdbEntryTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_SwitchingFDB_TypeOfVL
*
* @purpose Get 'TypeOfVL'
 *@description  [TypeOfVL] Type of supported VL   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFDB_TypeOfVL (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeOfVLValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbFDBTypeOfVLGet(L7_UNIT_CURRENT, &objTypeOfVLValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTypeOfVLValue, sizeof (objTypeOfVLValue));

  /* return the object value: TypeOfVL */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTypeOfVLValue, sizeof (objTypeOfVLValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjSet_SwitchingFDB_FdbFlush
*
* @purpose Set 'FdbFlush'
 *@description  [FdbFlush] <HTML>Flush all learned entries from the L2FDB   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingFDB_FdbFlush (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbFlushValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: FdbFlush */
  owa.len = sizeof (objFdbFlushValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objFdbFlushValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objFdbFlushValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbFdbFlush ();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
