
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingARPACL.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  19 May 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingARPACL_obj.h"
#include "usmdb_dai_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingARPACL_ARPACL
*
* @purpose Get 'ARPACL'
 *@description  [ARPACL] ARP ACL entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingARPACL_ARPACL (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objARPACLValue;
  xLibStr256_t nextObjARPACLValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ARPACL */
  memset(nextObjARPACLValue, 0x0, sizeof(nextObjARPACLValue));
  memset(objARPACLValue, 0x0, sizeof(objARPACLValue));
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingARPACL_ARPACL, (xLibU8_t *) objARPACLValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    memset(objARPACLValue, 0x0, sizeof(objARPACLValue));
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbArpAclNextGet(objARPACLValue, nextObjARPACLValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objARPACLValue, owa.len);
    owa.l7rc = usmDbArpAclNextGet(objARPACLValue, nextObjARPACLValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjARPACLValue, owa.len);

  /* return the object value: ARPACL */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjARPACLValue, strlen (nextObjARPACLValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingARPACL_ROWSTATUS
*
* @purpose Get 'ROWSTATUS'
 *@description  [ROWSTATUS] Create and Delete an ARP ACL   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingARPACL_ROWSTATUS (void *wap, void *bufp)
{

  fpObjWa_t kwaARPACL = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyARPACLValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objROWSTATUSValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ARPACL */
  kwaARPACL.rc = xLibFilterGet (wap, XOBJ_SwitchingARPACL_ARPACL,
                                (xLibU8_t *) keyARPACLValue, &kwaARPACL.len);
  if (kwaARPACL.rc != XLIBRC_SUCCESS)
  {
    kwaARPACL.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaARPACL);
    return kwaARPACL.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyARPACLValue, kwaARPACL.len);

  owa.l7rc  = usmDbArpAclGet(keyARPACLValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objROWSTATUSValue = L7_ROW_STATUS_ACTIVE;
  /* return the object value: ROWSTATUS */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objROWSTATUSValue, sizeof (objROWSTATUSValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingARPACL_ROWSTATUS
*
* @purpose Set 'ROWSTATUS'
 *@description  [ROWSTATUS] Create and Delete an ARP ACL   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingARPACL_ROWSTATUS (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objROWSTATUSValue;

  fpObjWa_t kwaARPACL = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyARPACLValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ROWSTATUS */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objROWSTATUSValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objROWSTATUSValue, owa.len);

  /* retrieve key: ARPACL */
  kwaARPACL.rc = xLibFilterGet (wap, XOBJ_SwitchingARPACL_ARPACL,
                                (xLibU8_t *) keyARPACLValue, &kwaARPACL.len);
  if (kwaARPACL.rc != XLIBRC_SUCCESS)
  {
    kwaARPACL.rc = XLIBRC_ARP_ACL_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaARPACL);
    return kwaARPACL.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyARPACLValue, kwaARPACL.len);

  if (objROWSTATUSValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbArpAclCreate(keyARPACLValue);
     if (owa.l7rc != L7_SUCCESS)
     {
        owa.rc = XLIBRC_ACL_CREATE_FAILURE;  
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;  
     }
  }
  else if (objROWSTATUSValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbArpAclDelete(keyARPACLValue);
     if (owa.l7rc != L7_SUCCESS)
     {
        owa.rc = XLIBRC_ACL_DEL_FAILURE; 
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

