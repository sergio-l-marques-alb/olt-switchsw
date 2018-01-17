/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename xlib_web.c
 *
 * @purpose  interfaces the web content (generated) with xlib objects
 *
 * @component XLIB
 *
 * @comments
 *
 * @create 04/17/2007
 *
 * @author Rama Sasthri, Kristipati
 * @end
 *
 **********************************************************************/

/*! \file */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "xlib_private.h"


#define xLibWebAddOffset(x,y) (x = (void *)((char *)x + y))

void *xLibWaCreate(void );
static void xLib2WebDebugDump (xLibWa_t * wap, char *msg);
static void xLibWeb2FilterDumpLoad (xLibWa_t * wap);
static xLibBool_t xLibWeb2RecordError (xLibU32_t rowindex,void *cntx, xLibId_t oid, xLibRC_t rc, xLibS8_t * value);
static xLibRC_t xLibWeb2ValidateIntMinMax (xLibId_t oid, xLibU16_t type, xLibS8_t * value);
void emwebTransferProgressCheckSet(int (*callback)(void *), void *arg, char *url1, char *url2);
/* variable to maintain alternate colors in plain table */
static xLibU16_t mod;
static xLibRC_t xLibWeb2DownloadUrlSet (void *arg, char *name)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  emwebDownloadUrlSet (wap->ui.web.webCntxt, name);
  return XLIBRC_SUCCESS;
}

xLibWa_t *xLibWeb2AppWaGet(void *webSrvCntx)
{
  xLibWa_t *wap = (xLibWa_t *) emwebAppWaGet (webSrvCntx, sizeof (xLibWa_t));
  if(wap == NULL)
  {
    wap = xLibWaCreate();
    emwebAppWaSet(webSrvCntx, wap);
  }
  return wap;
}

void *xLibWeb2WorkAreaGet (void *webSrvCntx)
{
  xLibWa_t *wap = xLibWeb2AppWaGet(webSrvCntx);
  if(wap != NULL)
  {
    wap->magic = XLIB_WA_MAGIC;
    wap->ui.web.webCntxt = webSrvCntx;
    wap->filters = emwebGetFilters (webSrvCntx);
    wap->fileNameSet = xLibWeb2DownloadUrlSet;
    emwebFileHandlerSet (webSrvCntx, xLibFileHandlerGet ());
  }
  return wap;
}

void *xLibWeb2ObjInit (void *webSrvCntx)
{
  xLibWa_t *wap = xLibWeb2WorkAreaGet (webSrvCntx);
  if(wap != NULL)
  {
    wap->ui.web.htmlBufferOffset = 0;
    wap->ui.web.htmlBuffer[0] = 0;
    xLibFreeWorkAreaMemory (wap, 0);
    wap->objCount = 0;
    wap->keyCount = 0;
  }
  return wap;
}

/**
 * @brief
 *
 * @param oid
 *
 * @return
 */
static xLibObjNode_t *xLibObjNodeExists (xLibId_t oid)
{
  xLibU16_t size;
  xLibObjNode_t * node;
  xLibObjNode_t * root = xLibObjGetNodes (&size);
  xLibU32_t mod_oid = (oid & 0xFFFF0000);

  for (node = root; node->oid != 0; node++)
  {
    if (node->oid == mod_oid)
    {
      return node;
    }
  }
  return NULL;
}



void *xLibWeb2Init (void *webSrvCntx)
{
  xLibWa_t *wap = (xLibWa_t *) xLibWeb2ObjInit (webSrvCntx);
  if(wap != NULL)
  {
    xLibFreeWorkAreaMemory (wap, 1);
    wap->keyCount = 0;
    wap->intf=XLIB_IF_WEB;
  }
  emwebFileClose(webSrvCntx);
  return wap;
}

void *xLibWebLiveObjGet (void *wap, xLibId_t oid, xLibU16_t type)
{

  xLibRC_t rv;
  xLibObjNode_t * node;
  xLibBuf_t bd;
  static xLibS8_t retVal[XLIB_MAX_VAL_LEN];
  xLibU16_t outSizeLen;

  /* find the module */
  node = xLibObjNodeExists (oid);

  if (node == NULL)
  {
    XLIB_WARN ("xLibWebLiveObjGet: The given live object doesnot exist\r\n");
    return "";
  }

  /* get the value from object handler */
  xLibBufDataInit (wap, &bd, oid);
  rv = node->getFunc (wap, oid, &bd);
  if (rv != XLIBRC_SUCCESS && rv != XLIBRC_ENDOF_TABLE)
  {
    XLIB_WARN ("xLibWebLiveObjGet: The object handler for given live object returned error.\r\n");
    return "";
  }

  if (rv == XLIBRC_SUCCESS)
  {
    if (bd.valid != XLIB_TRUE)
    {
       XLIB_WARN ("xLibWebLiveObjGet: Failed to validate the data returned by the object handler.\r\n");
       return "";
    }
    else
    {
      memset(retVal,0x00,XLIB_MAX_VAL_LEN);
	  outSizeLen = XLIB_MAX_VAL_LEN;
      rv= xLibTypeEncode (XLIB_IF_WEB, type, bd.value, bd.valen,
                         retVal, &outSizeLen, XLIB_TRUE);
	  if(rv== XLIBRC_SUCCESS)
	  {
	    return retVal;
	  }
    }
  }
 XLIB_WARN ("xLibWebLiveObjGet: Unknown error returned.\r\n");
 return "";

}
static xLibWa_t *xLibWeb2ReqInit (void *cntx, char *msg)
{
  xLibWa_t *wap = (xLibWa_t *) cntx;
  xLib2WebDebugDump (wap, msg);
  xLibAllocWorkAreaMemory (wap);
  return wap;
}

static void xLibWeb2ReqFinish (xLibWa_t * wap, char *msg)
{
  xLib2WebDebugDump (wap, msg);
  xLibWeb2ObjInit (wap->ui.web.webCntxt);
}

void xLibWeb2AddKey (void *cntx, xLibId_t oid, xLibU16_t type, xLibId_t cpoid, xLibU32_t flags)
{
  xLibWaAddKey ((xLibWa_t *) cntx, oid, type, cpoid,flags);
}

void xLibWeb2AddObj (void *cntx, xLibId_t oid, xLibId_t cpoid, xLibU16_t type, xLibU16_t flag,
    xLibS8_t * *var, xLibU8_t * status, xLibS8_t * name, xLibS8_t * clazz)
{
  xLibObj_t *objp = xLibWaAddObj ((xLibWa_t *) cntx, oid, cpoid, type, flag);
  if(objp != NULL)
  {
     objp->ui.web.var = var;
     objp->ui.web.status = status;
     objp->ui.web.name = name;
     objp->ui.web.clazz = clazz;
  objp->loadFunc=NULL;
}
}
void xLibWeb2AddPseudoKeyObj (void *cntx, xLibId_t oid, xLibId_t cpoid, xLibId_t keyoid,
    xLibU16_t type, xLibU16_t flag, xLibS8_t * *var, xLibU8_t * status,
    xLibS8_t * name, xLibS8_t * clazz)
{
  xLibObj_t *objp = xLibWaAddObjWithKey ((xLibWa_t *) cntx, oid, cpoid, keyoid, type, flag);
  if(objp != NULL)
  {
     objp->ui.web.var = var;
     objp->ui.web.status = status;
     objp->ui.web.name = name;
     objp->ui.web.clazz = clazz;
  }

}
void xLibWeb2AddObjForIterate (void *cntx, xLibId_t oid, xLibId_t cpoid, xLibU16_t type, xLibU16_t flag, xLibS8_t * clazz)
{
  xLibObj_t *objp = xLibWaAddObj ((xLibWa_t *) cntx, oid, cpoid, type, flag);
  if(objp != NULL)
  {
      objp->ui.web.clazz = clazz;
  }
}

void xLibWeb2RegisterLoadFunc(void *cntx, xLibId_t oid, objFunc_t func)
{
  xLibWa_t *wap = (xLibWa_t *)cntx;
  xLibU16_t i;
  xLibObj_t *objp;

   for (i = 0; i < wap->objCount; i++)
   {
      objp = &wap->objs[i];
      if(objp->oid == oid)
      {
        objp->loadFunc = func;
      }
   }

}

void xLibWeb2AddPrivObj (void *cntx, xLibId_t oid, xLibId_t cpoid, xLibU16_t type, xLibU16_t flag,
    xLibS8_t * *var, xLibU8_t * status, xLibS8_t * name, xLibU32_t offset,  xLibS8_t * clazz)
{
  xLibWeb2AddObj (cntx, oid + offset, cpoid, type, flag, var, status, name, clazz);
}

void xLibWeb2ErrorMap (void *cntx, xLibId_t oid, xLibU32_t code, xLibU32_t mCode)
{
  return xLibWaErrorMap ((xLibWa_t *) cntx, oid, code, mCode);
}

void xLibWeb2ObjUIEleValSet (void *cntx, xLibU32_t objID, xLibU32_t uiWebID, xLibU32_t uiCliID,
    xLibU32_t eleID)
{
  return xLibWaObjUIEleValSet ((xLibWa_t *) cntx, objID, uiWebID, uiCliID, eleID);
}

void xLibWeb2RowStatusDepListSet (void *cntx, xLibId_t rowOid, xLibId_t depOid)
{
  return xLibWaRowStatusDepListSet ((xLibWa_t *) cntx, rowOid, depOid);
}

/**
 * @brief  Helper function to determine if the corresponding web element
 *         is disabled, we should not set value from disabled elements
 *
 * @param objp
 *
 * @return boolean
 */
xLibBool_t xLibWeb2IsDisable (xLibObj_t * objp)
{
  return (*objp->ui.web.status == 0) ? XLIB_TRUE : XLIB_FALSE;
}


xLibRC_t xLibObjInvokeLoad(void *cntx,xLibObj_t *objp){

   xLibBuf_t bd;
   xLibWa_t *wap=(xLibWa_t *)cntx;
   xLibRC_t rc = XLIBRC_SUCCESS;
   xLibU8_t tmp[1024];
   xLibU16_t tmpLen = sizeof(tmp);

   memset(tmp,0,sizeof(tmp));
   if(objp->loadFunc != NULL ){

      rc = xLibTypeEncode (XLIB_IF_WEB, objp->type, objp->value, objp->valen,
           tmp, &tmpLen, XLIB_TRUE);

      if (rc != XLIBRC_SUCCESS)
      {
          XLIB_WARN ("xLibTypeEncode: failed for 0x%08x: rc = %s\r\n", objp->oid,
          xLibRcStrErr (rc));
          return rc;
      }

      rc = xLibFilterSet2(wap,objp->oid,objp->type,tmp,tmpLen);
      if(rc != XLIBRC_SUCCESS){
          XLIB_WARN ("xLibFilterSet2: failed for 0x%08x: rc = %s\r\n", objp->oid,
          xLibRcStrErr (rc));
          return rc;
      }
      xLibBufDataInit (wap, &bd, objp->oid);
      rc= objp->loadFunc (wap, &bd);
   }

   return rc;
}

void xLibWeb2Get (void *cntx)
{
  int i;
  char *retval;
  xLibObj_t *objp = NULL;
  xLibWa_t *wap = xLibWeb2ReqInit (cntx, "xLibWeb2Get start");

  /* All the required fileters are set - get the object values now */
  for (i = 0; i < wap->objCount; i++)
  {
    void *colateWap = NULL;

    objp = &wap->objs[i];

    /* initially we can take the value either from filter or object handler
       but when we are collating, fresh flag should be set so that we take
       only new values */
    objp->flag = objp->flag & ~XLIB_IS_FRESH;

    /* we need to loop here to support collate, excercise caution and make
       sure that we don't go into infinete loop */
    while (1)
    {
      /* Get the object value from the handler */
      objp->rc = xLibObjGetMayBeKey (wap, objp);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        if (objp->rc != XLIBRC_ENDOF_TABLE && objp->rc != XLIBRC_IGNORE)
        {
          XLIB_WARN ("xLibObjGetMayBeKey: failed for 0x%08x: rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
        }
        break;
      }
      /* store the value as a filter also, so that other obj handler can use if needed */
      xLibFilterSet (wap, objp->oid, objp->type, objp->value, objp->valen);

      /*invoke the load handler for ui objects */
           xLibObjInvokeLoad(wap,objp);

      /* encode the value */
      wap->encLen = sizeof (wap->encBuf);
      objp->rc = xLibTypeEncode (XLIB_IF_WEB, objp->type, objp->value, objp->valen,
          wap->encBuf, &wap->encLen, XLIB_TRUE);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        XLIB_WARN ("xLibTypeEncode: failed for 0x%08x: rc = %s\r\n", objp->oid,
            xLibRcStrErr (objp->rc));
        break;
      }
      else
      {
        XLIB_TRACE ("xLibTypeEncode: success for 0x%08x to %s\r\n", objp->oid, wap->encBuf);
      }

      /* if colation is not required we don;t need to loop for more data */
      if (objp->flag & XLIB_IS_COLLATE && objp->flag & XLIB_IS_KEY)
      {
        objp->rc = xLibTypeColate (XLIB_IF_WEB, objp->type, wap->encBuf, wap->encLen, &colateWap);
        if (objp->rc != XLIBRC_SUCCESS)
        {
          XLIB_WARN ("xLibTypeColate: failed for 0x%08x: rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
          break;
        }

        /* we don't want to get stuck infinite loop looking at same value from filter
           so set the flag to FRESH to see some new blood */
        objp->flag = objp->flag | XLIB_IS_FRESH;
      }
      else
      {
        break;
      }
    }

    if (objp->rc == XLIBRC_SUCCESS)
    {
      /* if there is no colation return the currently encoded data otherwise retutn the colated info */
      retval = (colateWap == NULL) ? wap->encBuf : xLibTypeColatedGet (&colateWap);

      /* set the value and status in emweb structures */
      emwebFormValueSet (objp->ui.web.var, objp->ui.web.status, retval, XLIB_TRUE);
    }
    else if (colateWap != NULL)
    {
      /* if there is no colation return the currently encoded data otherwise retutn the colated info */
      retval = xLibTypeColatedGet (&colateWap);

      /* set the value and status in emweb structures */
      emwebFormValueSet (objp->ui.web.var, objp->ui.web.status, retval, XLIB_TRUE);
    }
    else
    {
      /* clear the value and status in emweb structures */
      emwebFormValueSet (objp->ui.web.var, objp->ui.web.status, NULL, XLIB_TRUE);
    }

    xLibTypeColateFinish (&colateWap);
  }
  xLibWeb2ReqFinish (wap, "xLibWeb2Get end");
}

void xLibWeb2RepeatGet (void *cntx, xLibU32_t pageMax, xLibU16_t rep,
    xLibU16_t varSize, xLibU16_t statSize)
{
  xLibRC_t rc;
  xLibU16_t i;
  xLibObj_t *objp = NULL;
  xLibWa_t *wap = xLibWeb2ReqInit (cntx, "xLibWeb2RepeatGet start");

  /* preserve the filter keys */
  xLibFilterPushFromKeys (wap);

  /* Load the filters from the dump for the pagination */
  if (pageMax != 0)
  {
    if (emwebFormSubmitGet (wap->ui.web.webCntxt) == XLIB_SUBMIT_FLAG_NEXT)
    {
      xLibWeb2FilterDumpLoad (wap);
    }
  }


  /* repeat only as many times asked for */
  while (rep-- > 0)
  {
    rc = xLibObjLoadKey (wap);

    /*unreserve the filters that are reserved by ui objects
      otherwise, we cannot update the filters with next entries in table */
      xLibUnreserveFilters(wap);

    if (rc != XLIBRC_SUCCESS)
    {
      /* no more keys */
      /* TODO: Do we need to clear other repeats left ?? */
      XLIB_TRACE ("no more keys");
      break;
    }

    /* get all object values */
    for (i = 0; i < wap->objCount; xLibWebAddOffset (wap->objs[i].ui.web.var, varSize),
        xLibWebAddOffset (wap->objs[i].ui.web.status, statSize), i++)
    {
      objp = &wap->objs[i];

      objp->rc = xLibObjGetMayBeKey (wap, objp);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        if (objp->rc != XLIBRC_ENDOF_TABLE)
        {
          /* TODO: How to report this error to UI */
          XLIB_WARN ("xLibObjGetMayBeKey failed: for 0x%08x: rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
        }
        continue;
      }

      /* store the value as a filter also, so that other obj handler can use if needed */
      if (objp->flag & XLIB_IS_PSEUDO_KEY)
      {

        /*xLibSetKeyForPseudoKey(wap, objp); */
      }
      else
      {
        xLibFilterSet (wap, objp->oid, objp->type, objp->value, objp->valen);

        /*invoke the load handler for ui objects */
           xLibObjInvokeLoad(wap,objp);

      }


      /* encode the result */
      wap->encLen = sizeof (wap->encBuf);
      objp->rc = xLibTypeEncode (XLIB_IF_WEB, objp->type, objp->value, objp->valen,
          wap->encBuf, &wap->encLen, XLIB_TRUE);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        XLIB_WARN ("xLibTypeEncode failed: for 0x%08x: rc = %s\r\n", objp->oid,
            xLibRcStrErr (objp->rc));
        /* clear the value and status in emweb structures */
        emwebFormValueSet (objp->ui.web.var, objp->ui.web.status, NULL, XLIB_TRUE);
      }
      else
      {
        XLIB_TRACE ("xLibTypeEncode: success for 0x%08x to %s\r\n", objp->oid, wap->encBuf);
        /* set the value and status in emweb structures */
        emwebFormValueSet (objp->ui.web.var, objp->ui.web.status, wap->encBuf, XLIB_TRUE);
      }
    }
  }

   /*Reserve the filters that are unreserved */
   xLibReserveUnreservedFilters(wap);

  /* Generate the filter dump for pagination */
  if (pageMax != 0)
  {
    emwebFilterDumpAppend (wap->ui.web.webCntxt, xLibFilterDumpBuffer (wap, XLIB_IF_WEB),
        XLIB_FALSE);
  }

  /* get the pushed values back */
  xLibFilterPopFromKeys (wap);

  xLibWeb2ReqFinish (wap, "xLibWeb2RepeatGet end");
}
/* Reads object values for each iteration and formats buffer and return  formatted string*/

char *xLibWeb2IterateGet(void *cntx, xLibCS8_t * clazz)
{
  xLibObj_t *objp;
  xLibWa_t *wap = xLibWeb2ReqInit (cntx, "xLibWeb2IterateGet start");
  xLibS8_t buf[XLIB_BUF_SIZE_2048];
  /* Add one to buffer size to handle EOS */
  xLibS8_t outBuf[XLIB_MAX_VAL_LEN+1];
  xLibS8_t clazzTemp[XLIB_BUF_SIZE_100+1];
  xLibU16_t outBufLen,i;
  xLibBool_t isEncode;

  memset(wap->ui.web.buf2048,0,sizeof(wap->ui.web.buf2048));
  memset(buf,0,sizeof(buf));

  /* Read the element values based on objCount in wap */
  strcpy(buf, "<tr>");
  for(i=0;i<wap->objCount; i++)
  {
     isEncode = XLIB_TRUE;
     memset(outBuf,0,sizeof(outBuf));
     memset(clazzTemp,0,sizeof(clazzTemp));
     objp = &wap->objs[i];
     objp->rc = xLibObjGetMayBeKey (wap, objp);
     if (objp->rc != XLIBRC_SUCCESS)
      {
        if (objp->rc == XLIBRC_ENDOF_TABLE)
        {
          /* Code cannot come to this loop as condition is checked in fucntion xLibWeb2IsIterate and returns from there */
          XLIB_WARN ("Debug Statement END OF TABLE from xLibWeb2Iterate: for 0x%08x: rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
        }
        else
        {
            XLIB_WARN ("xLibObjGetMayBeKey failed: for 0x%08x: rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
            /* objp->value is set to blank string for display purpose */

        }
        isEncode = XLIB_FALSE;
      }

      if( isEncode == XLIB_TRUE)
      {
        /* store the value as a filter also, so that other obj handler can use if needed */
        xLibFilterSet (wap, objp->oid, objp->type, objp->value, objp->valen);
        /* encode the result */
        outBufLen = sizeof(outBuf);
        objp->rc = xLibTypeEncode (XLIB_IF_WEB, objp->type, objp->value, objp->valen,
          outBuf, &outBufLen, XLIB_TRUE);
        if (objp->rc != XLIBRC_SUCCESS)
        {
          XLIB_WARN ("xLibTypeEncode failed: for 0x%08x: rc = %s\r\n", objp->oid,
            xLibRcStrErr (objp->rc));
          /* clear the value and status in emweb structures */
        }
      }
      if(objp->flag & XLIB_IS_HIDDEN)
      {
        if(objp->ui.web.clazz== NULL)
        {
            if(mod%2==0)
            strcat(clazzTemp,"<td CLASS=\"tabledata0\" style=\"display: none;\">");
            else
            strcat(clazzTemp,"<td CLASS=\"tabledata1\" style=\"display: none;\">");
        }
        else
        {
          if(mod%2==0)
          sprintf(clazzTemp,"<td CLASS=\"tabledata0 %s\" style=\"display: none;\">",objp->ui.web.clazz);
          else
          sprintf(clazzTemp,"<td CLASS=\"tabledata1 %s\" style=\"display: none;\">",objp->ui.web.clazz);
        }
      }
      else
      {
        if(objp->ui.web.clazz== NULL)
        {
            if(mod%2==0)
            strcat(clazzTemp,"<td CLASS=\"tabledata0\">");
            else
            strcat(clazzTemp,"<td CLASS=\"tabledata1\">");
        }
        else
        {
          if(mod%2==0)
          sprintf(clazzTemp,"<td CLASS=\"tabledata0 %s\">",objp->ui.web.clazz);
          else
          sprintf(clazzTemp,"<td CLASS=\"tabledata1 %s\">",objp->ui.web.clazz);
        }
      }
      strcat(buf,clazzTemp);
      strcat(buf,outBuf);
      strcat(buf,"</td>");
   }
   strcat(buf,"</tr>");
   mod++;/* increment the variable for alternate color*/
   strcpy(wap->ui.web.buf2048,buf);
   xLibWeb2ReqFinish (wap, "xLibWeb2IterateGet end");
   return wap->ui.web.buf2048;
}

void xLibWeb2SetFinish(void* cntx)
{

  /* TODO : Add the functionality needed after submit   */

}

void xLibWeb2ClearRangeObjFilter(void* cntx)
{
  xLibWa_t *wap  = ( xLibWa_t *) cntx;
  xLibObj_t *objp = NULL;
  xLibU16_t i,j;
  /*
     if it is a range object, we need to clear the filter for this object and all its
     dependencies.This is to ensure that we will get all the values from the usmdb,
     instead only those set during the set command. i.e there can be few values set
     during previous set which will not appear during this serve
   */
  for (i = 0; i < wap->objCount; i++)
  {
    objp = &wap->objs[i];
    if (objp->flag & XLIB_IS_COLLATE && objp->flag & XLIB_IS_KEY)
    {
      for (j = 0; j < objp->depCount; j++)
      {
        xLibFilterClear (wap, objp->depList[j]);
      }
      xLibFilterClear (wap, objp->oid);
    }
  }

}

void xLibWeb2Set (void *cntx)
{
  xLibBool_t err;
  xLibU16_t i, j;
  xLibObj_t *objp = NULL;
  xLibWa_t *wap = xLibWeb2ReqInit (cntx, "xLibWeb2Set start");

  /* For all the objects in the request
   *  - decode the value
   *  - set the filter
   */
  for (i = 0; i < wap->objCount; i++)
  {
    objp = &wap->objs[i];

    /* init error code */
    objp->rc = XLIBRC_SUCCESS;


    /* skip disabled elements and  decode failed elements */
    if (objp->rc != XLIBRC_SUCCESS || XLIB_TRUE == xLibWeb2IsDisable (objp))
    {
      continue;
    }
    /*skip key elements whose value will be set from xLibSetKeyForPseudoKey */
    if ((emwebFormSubmitGet (wap->ui.web.webCntxt) == XLIB_SUBMIT_FLAG_REFRESH) &&
        (objp->flag & XLIB_IS_KEY_FOR_PSEUDOKEY))
    {

      continue;
    }

    if ((emwebFormSubmitGet (wap->ui.web.webCntxt) == XLIB_SUBMIT_FLAG_SUBMIT)
        && (objp->flag & XLIB_IS_VALIDATE))
    {
      /* validate the data before decoding */
      objp->rc = xLibValidate (wap, i, objp->oid, objp->type, *objp->ui.web.var);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        err = xLibWeb2RecordError (0,wap, objp->oid, objp->rc, *objp->ui.web.var);
        XLIB_WARN2 (err, "xLibValidate (%s): failed for 0x%08x rc = %s\r\n",
            *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
        continue;
      }
      objp->rc = xLibWeb2ValidateIntMinMax (objp->oid,objp->type,*objp->ui.web.var);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        err = xLibWeb2RecordError (0,wap, objp->oid, objp->rc, *objp->ui.web.var);
        XLIB_WARN2 (err, "xLibValidate: failed for 0x%08x rc = %s\r\n", objp->oid,
            xLibRcStrErr (objp->rc));
        continue;
      }
    }

    /* decode the value */
    objp->valen = XLIB_MAX_VAL_LEN;
    objp->rc = xLibTypeDecode (XLIB_IF_WEB, objp->type, *objp->ui.web.var,
        objp->value, (xLibU16_t *) & objp->valen, XLIB_TRUE);
    XLIB_TRACE ("xLibTypeDecode (%s): retval for 0x%08x rc = %s\r\n",
        *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
    if (objp->rc != XLIBRC_SUCCESS)
    {
      XLIB_WARN ("xLibTypeDecode (%s): failed for 0x%08x rc = %s\r\n",
          *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
      continue;
    }

    /* store the value as a filter also, so that obj handler can use if needed */
    xLibFilterSet (wap, objp->oid, objp->type, objp->value, objp->valen);

     /* invoke the load func for ui module */
         xLibObjInvokeLoad(wap,objp);


    /* set key object if this object is pseudokey */

    if ((emwebFormSubmitGet (wap->ui.web.webCntxt) == XLIB_SUBMIT_FLAG_REFRESH) &&
        /*         objp->keyoid != 0) */
      (objp->flag & XLIB_IS_PSEUDO_KEY))
      {

        xLibSetKeyForPseudoKey (wap, objp);
      }

    /* set the row status flag */
    if (xLibIsRowStatusType (objp->type) == XLIB_TRUE)
    {
      objp->rc = xLibRowInfoCheck (objp->oid, objp->type, objp->value);
    }
  }

  /* just a refresh */
  if (emwebFormSubmitGet (wap->ui.web.webCntxt) != XLIB_SUBMIT_FLAG_SUBMIT)
  {
    xLibWeb2ReqFinish (wap, "xLibWeb2Set end - just a refresh");
    return;
  }

  /* now we have all the info in the work area, set now - row status add */
  for (i = 0; i < wap->objCount; i++)
  {
    objp = &wap->objs[i];

    if (objp->flag & XLIB_IS_UIOBJ_ROW_STATUS){
       continue;
    }

    if (objp->rc == XLIBRC_ROW_STATUS_ADD)
    {
      objp->rc = xLibObjSet (wap, objp->oid, objp->value, objp->valen);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        err = xLibWeb2RecordError (0,wap, objp->oid, objp->rc, *objp->ui.web.var);
        XLIB_WARN2 (err, "xLibWeb2Set (%s): failed for 0x%08x rc = %s\r\n",
            *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));

        /* clear the filters  for dependent list for row status object  */
        for (j = 0; j < objp->depCount; j++)
        {
          xLibFilterClear (wap, objp->depList[j]);
        }

        xLibWeb2ReqFinish (wap, "xLibWeb2Set end");
        return;

      }
      else
      {
        objp->rc = XLIBRC_ROW_STATUS_ADD;
      }
    }
  }

  /* now we have all the info in the work area, set now - non row-status */
  for (i = 0; i < wap->objCount; i++)
  {
    objp = &wap->objs[i];
    /* skip disabled elements, decode failed elements and row status objects */
    if (objp->rc != XLIBRC_SUCCESS || XLIB_TRUE == xLibWeb2IsDisable (objp))
    {
      continue;
    }
    if (objp->flag & XLIB_IS_UIOBJ_ROW_STATUS){
       continue;
    }

    /* TODO: Optimize by checking if it is a key and don;t call set as we don;t have set handlers anyway */
    objp->rc = xLibObjSet (wap, objp->oid, objp->value, objp->valen);
    if (objp->rc != XLIBRC_SUCCESS)
    {
      err = xLibWeb2RecordError (0,wap, objp->oid, objp->rc, *objp->ui.web.var);
      XLIB_WARN2 (err, "xLibWeb2Set: failed for 0x%08x rc = %s\r\n", objp->oid,
          xLibRcStrErr (objp->rc));
    }
  }

  /* now we have all the info in the work area, set now - row status delete */
  for (i = 0; i < wap->objCount; i++)
  {
    objp = &wap->objs[i];

    if (objp->flag & XLIB_IS_UIOBJ_ROW_STATUS){
       continue;
    }

    if (objp->rc == XLIBRC_ROW_STATUS_DELETE)
    {
      objp->rc = xLibObjSet (wap, objp->oid, objp->value, objp->valen);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        err = xLibWeb2RecordError (0,wap, objp->oid, objp->rc, *objp->ui.web.var);
        XLIB_WARN2 (err, "xLibWeb2Set: failed for 0x%08x rc = %s\r\n", objp->oid,
            xLibRcStrErr (objp->rc));
      }
      else
      {
        /* clear the filters  for dependent list for row status object when delete */
        for (j = 0; j < objp->depCount; j++)
        {
          xLibFilterClear (wap, objp->depList[j]);
        }
      }
    }
  }


/* finally, check with ui objects row-status */
  for (i = 0; i < wap->objCount; i++)
  {
    objp = &wap->objs[i];
    if(XLIB_TRUE == xLibWeb2IsDisable (objp)){
       continue;
    }
    if (objp->flag & XLIB_IS_UIOBJ_ROW_STATUS)
    {
      if (objp->rc == XLIBRC_ROW_STATUS_DELETE) {
        xLibClearFiltersInvoke(wap);
      }
      objp->rc = xLibObjSet (wap, objp->oid, objp->value, objp->valen);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        err = xLibWeb2RecordError (0,wap, objp->oid, objp->rc, *objp->ui.web.var);
        XLIB_WARN2 (err, "xLibWeb2Set (%s): failed for 0x%08x rc = %s\r\n",
            *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
      }
      break;
    }
  }

  xLibWeb2SetFinish(wap);
  xLibWeb2ReqFinish (wap, "xLibWeb2Set end");
}

void xLibWeb2RepeatSet (void *cntx, xLibU16_t flagVal, xLibU16_t rep,
    xLibU16_t varSize, xLibU16_t statSize)
{
  xLibBool_t first = XLIB_TRUE;
  xLibBool_t err;
  xLibU16_t i, j, k;
  xLibU32_t cnt = 0;
  xLibObj_t *objp = NULL;
  xLibObj_t *repobj = NULL;
  xLibWa_t *wap = xLibWeb2ReqInit (cntx, "xLibWeb2RepeatSet start");

  /* preserve the filter keys */
  xLibFilterPushFromKeys (wap);

  /* repeat only as many times asked for */
  while (rep-- > 0)
  {
    cnt = cnt+1;

    /* get the next set of values - except first time */
    for (i = 0; i < wap->objCount && first == XLIB_FALSE; i++)
    {
      xLibWebAddOffset (wap->objs[i].ui.web.var, varSize);
      xLibWebAddOffset (wap->objs[i].ui.web.status, statSize);
    }
    first = XLIB_FALSE;

    /* For all the objects in the request
     *  - decode the value
     *  - set the filter
     */

    for (i = 0; i < wap->objCount; i++)
    {
      objp = &wap->objs[i];

      /* init error code */
      objp->rc = XLIBRC_SUCCESS;

      /* skip disabled elements and decode failed elements */
      if (objp->rc != XLIBRC_SUCCESS || XLIB_TRUE == xLibWeb2IsDisable (objp))
      {
        continue;
      }
      if ((emwebFormSubmitGet (wap->ui.web.webCntxt) == XLIB_SUBMIT_FLAG_SUBMIT)
          && (objp->flag & XLIB_IS_VALIDATE))
      {
        /* validate the data before decoding */
        objp->rc = xLibValidate (wap, i, objp->oid, objp->type, *objp->ui.web.var);
        if (objp->rc != XLIBRC_SUCCESS)
        {
          err = xLibWeb2RecordError (cnt,wap, objp->oid, objp->rc, *objp->ui.web.var);
          XLIB_WARN2 (err, "xLibValidate: failed for 0x%08x rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
          continue;
        }
        objp->rc = xLibWeb2ValidateIntMinMax (objp->oid, objp->type, *objp->ui.web.var);
        if (objp->rc != XLIBRC_SUCCESS)
        {
          err = xLibWeb2RecordError (cnt,wap, objp->oid, objp->rc, *objp->ui.web.var);
          XLIB_WARN2 (err, "xLibValidate: failed for 0x%08x rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
          continue;
        }
      }

      /* decode the value */
      objp->valen = XLIB_MAX_VAL_LEN;
      objp->rc = xLibTypeDecode (XLIB_IF_WEB, objp->type, *objp->ui.web.var,
          objp->value, (xLibU16_t *) & objp->valen, XLIB_TRUE);
      XLIB_TRACE ("xLibTypeDecode (%s): retval for 0x%08x rc = %s\r\n",
          *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
      if (objp->rc != XLIBRC_SUCCESS)
      {
        XLIB_WARN ("xLibTypeDecode (%s): failed for 0x%08x rc = %s\r\n",
            *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
        continue;
      }

      /* store the value as a filter also, so that obj handler can use if needed */
      xLibFilterSet (wap, objp->oid, objp->type, objp->value, objp->valen);

      /*invoke the load handler for ui objects */
           xLibObjInvokeLoad(wap,objp);


      /* set the row status flag */
      if (xLibIsRowStatusType (objp->type) == XLIB_TRUE)
      {
        objp->rc = xLibRowInfoCheck (objp->oid, objp->type, objp->value);
      }
    }

    xLib2WebDebugDump (wap, "xLibWeb2RepeatSet iter");
    /* now we have all the info in the work area, set now - row status add */
    for (i = 0; i < wap->objCount; i++)
    {
      objp = &wap->objs[i];

      if (objp->rc == XLIBRC_ROW_STATUS_ADD)
      {
        if (emwebFormSubmitGet (wap->ui.web.webCntxt) != XLIB_SUBMIT_FLAG_SUBMIT)
        {
           continue;
        }

        objp->rc = xLibObjSet (wap, objp->oid, objp->value, objp->valen);
        if (objp->rc != XLIBRC_SUCCESS)
        {
          err = xLibWeb2RecordError (cnt,wap, objp->oid, objp->rc, *objp->ui.web.var);
          XLIB_WARN2 (err, "xLibWeb2Set: failed for 0x%08x rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));

          /* clear the filters  for dependent list for row status object */
          for (j = 0; j < objp->depCount; j++)
          {
             for(k=0; k < wap->objCount; k++)
                 {
                  /* clear the filter only if the key is present in repeat */
                   repobj = &wap->objs[k];
                   if(repobj->oid == objp->depList[j])
                      xLibFilterClear (wap, objp->depList[j]);
                 }
          }

          xLibWeb2ReqFinish (wap, "xLibWeb2Set end");
          return;

        }
        else
        {
          objp->rc = XLIBRC_ROW_STATUS_ADD;
        }
      }
    }

    /* now we have all the info in the work area, set now */
    for (i = 0; i < wap->objCount; i++)
    {
      objp = &wap->objs[i];

      /* skip disabled elements and decode failed elements */
      if (objp->rc != XLIBRC_SUCCESS || XLIB_TRUE == xLibWeb2IsDisable (objp))
      {
        continue;
      }

      /* just a refresh - Is this valid for repeat - NEED TO CHECK */
      if (emwebFormSubmitGet (wap->ui.web.webCntxt) != XLIB_SUBMIT_FLAG_SUBMIT)
      {
        continue;
      }

      /* set the object value */
      objp->rc = xLibObjSet (wap, objp->oid, objp->value, objp->valen);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        err = xLibWeb2RecordError (cnt,wap, objp->oid, objp->rc, *objp->ui.web.var);
        XLIB_WARN2 (err, "xLibWeb2RepeatSet: failed for 0x%08x rc = %s\r\n", objp->oid,
            xLibRcStrErr (objp->rc));
      }
    }

    /* when object is row status type and value is delete */
    for (i = 0; i < wap->objCount; i++)
    {
      objp = &wap->objs[i];
      if (objp->rc == XLIBRC_ROW_STATUS_DELETE)
      {
        /* set the object value */
        if (emwebFormSubmitGet (wap->ui.web.webCntxt) != XLIB_SUBMIT_FLAG_SUBMIT)
        {
           continue;
        }
        objp->rc = xLibObjSet (wap, objp->oid, objp->value, objp->valen);
        if (objp->rc != XLIBRC_SUCCESS)
        {
          err = xLibWeb2RecordError (cnt,wap, objp->oid, objp->rc, *objp->ui.web.var);
          XLIB_WARN2 (err, "xLibWeb2RepeatSet: failed for 0x%08x rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
        }
        else
        {
          /* clear the filters  for dependent list for row status object when delete */
          for (j = 0; j < objp->depCount; j++)
          {
            for(k=0; k < wap->objCount; k++)
               {
                /* clear the filter only if the key is present in repeat */
                 repobj = &wap->objs[k];
                 if(repobj->oid == objp->depList[j])
            xLibFilterClear (wap, objp->depList[j]);
               }
          }
        }
      }
    }
    /* set the flag to zero for the filters that are reserved by ui objects inside a repeat
       otherwise, we cannot update the filters with next entries in table.This call calong with tge below call
       xLibReserveUnreservedFilters shall be removed when creation of duplicate filters is removed
     */
      xLibUndoReserveFilters(wap);

  }

  /*Reserve the filters that are unreserved */
  /* Check why this is needed */
  xLibReserveUnreservedFilters(wap);

  /* get the pushed values back */
  xLibFilterPopFromKeys (wap);

  xLibWeb2SetFinish(wap);
  xLibWeb2ReqFinish (wap, "xLibWeb2RepeatSet end");

}

void xLibWeb2FilterSet (void *cntx, xLibId_t oid, xLibU16_t type, char *value)
{
  xLibWa_t *wap = xLibWeb2ReqInit (cntx, "xLibWeb2FilterSet start");
  xLibObj_t *objp = &wap->objs[0];

  objp->rc = xLibTypeDecode (XLIB_IF_WEB, type, value, objp->value,
      (xLibU16_t *) & objp->valen, XLIB_TRUE);
  XLIB_TRACE ("xLibTypeDecode (%s): retval for 0x%08x rc = %s\r\n",
      value, oid, xLibRcStrErr (objp->rc));
  if (objp->rc != XLIBRC_SUCCESS)
  {
    XLIB_WARN ("xLibTypeDecode (%s): failed for 0x%08x rc = %s\r\n",
        value, oid, xLibRcStrErr (objp->rc));
  }
  else
  {
    xLibFilterSet (wap, oid, type, objp->value, objp->valen);
  }
  xLibWeb2ReqFinish (wap, "xLibWeb2FilterSet end");
}

void *xLibWeb2AddErrMap (void *cntx, xLibRC_t err1, xLibRC_t err2)
{
  return cntx;
}

char *xLibWeb2FinishSubmit (void *cntx, char *file, char *filterDump)
{
  xLibWa_t *wap = (xLibWa_t *) cntx;
  xLibWeb2ClearRangeObjFilter(wap);
  emwebServeAfterSubmitSet (wap->ui.web.webCntxt, XLIB_TRUE);
  emwebFilterDumpSet (wap->ui.web.webCntxt, NULL, XLIB_TRUE);
  emwebFilterDumpAppend (wap->ui.web.webCntxt, filterDump, XLIB_TRUE);
  return emwebContextSendReply (wap->ui.web.webCntxt, file);
}

void xLibWeb2FinishServe (void *cntx, char **var, unsigned char *status)
{
  xLibWa_t *wap = (xLibWa_t *) cntx;
  if (var != NULL && status != NULL)
  {
    char *datap = emwebFilterDumpGet (wap->ui.web.webCntxt, XLIB_FALSE);
    emwebFormValueSet (var, status, datap, XLIB_TRUE);
  }
  emwebFilterDumpSet (wap->ui.web.webCntxt, NULL, XLIB_FALSE);
  emwebServeAfterSubmitSet (wap->ui.web.webCntxt, XLIB_FALSE);
}

char *xLibWeb2FinishForm (void *servContext)
{
  xLibU16_t size;
  xLibWa_t *wap = xLibWeb2AppWaGet(servContext);

  char *ptr = emwebErrorBufGet (servContext, &size);

  if (wap != NULL)
  {
    wap->ui.web.htmlBufferOffset = 0;
    wap->ui.web.htmlBuffer[0] = 0;
  }

  emwebFilterDumpSet (servContext, NULL, XLIB_TRUE);
  emwebFilterDumpSet (servContext, NULL, XLIB_FALSE);

  /* The form processing is finished, we need to clear all the
   * filters so that we don't interfear with other pages
   */
  xLibFilterRemoveAll (emwebGetFilters (servContext));

  emwebDownloadUrlSet (servContext, NULL);
  memset (ptr, 0, size);
  return "";
}

char *xLibWeb2RepeatCount (void *cntx, xLibU16_t rptIndex, xLibU32_t pageMax, xLibU32_t split)
{
  xLibU32_t count;
  xLibBool_t more;
  char *retval;
  xLibWa_t *wap = xLibWeb2ReqInit (cntx, "xLibWeb2RepeatCount start");

  xLibFilterPushFromKeys (wap);

  /* Load the filters from the dump for the pagination */
  if (pageMax != 0)
  {
    if (emwebFormSubmitGet (wap->ui.web.webCntxt) == XLIB_SUBMIT_FLAG_NEXT)
    {
      xLibWeb2FilterDumpLoad (wap);
    }
  }

  count = xLibObjCountKey (wap, pageMax, &more);

  xLibFilterPopFromKeys (wap);

  retval = emwebRepeatInitDetails (wap->ui.web.webCntxt, rptIndex, count, split, pageMax,
      (more == XLIB_TRUE) ? XLIB_TRUE : XLIB_FALSE);
  xLibWeb2ReqFinish (wap, "xLibWeb2RepeatCount end");

  return retval;
}
xLibBool_t xLibWeb2IsIterate( void *cntx,xLibIterate_t init)
{
  xLibRC_t rc;
  xLibWa_t *wap = xLibWeb2ReqInit (cntx, "xLibWeb2RepeatCount start");
  if(init != XLIB_ITERATE_NEXT)
  {
    /*  variable to maintain alternate colors in plain table
        reset the varible when trying to get first key instance in plain tables
    */
     mod =0;
     xLibFilterPushFromKeys(wap);
  }
   rc = xLibObjLoadKey (wap);
   if (rc != XLIBRC_SUCCESS)
   {
       /* no more keys
         clear keys from filters
         return XLIB_FALSE so that we do not iterate any more
       */

       XLIB_TRACE ("no more keys");
       xLibFilterPopFromKeys (wap);
       return XLIB_FALSE;
   }
   else
   {
       /* do not clear the filters instead return XLIB_TRUE*/
       return XLIB_TRUE;
  }

}
static char *xLibWeb2KeyAdd (char *str, int *off, int *max, char *newstr)
{
  int len1 = 0;
  int len2 = strlen (newstr);
  if (str == NULL)
  {
    return str;
  }
  len1 = strlen (str);
  if (*max - len1 <= len2)
  {
    XLIB_ERROR ("Memory exhaust in xLibWeb2KeyAdd Function\r\n");
  }
  else
  {
    sprintf (&str[*off], "\"%s\",", newstr);
    *off = strlen (str);
    str[*off] = 0;
  }
  return str;
}

#define MAX_KEYS_COUNT 10000

char *xLibWeb2KeyOrDynamicString (void *cntx, xLibId_t oid, xLibId_t cpoid, xLibU16_t type,
    objFunc_t func)
{
  char *retval = NULL;
  int max = MAX_KEYS_COUNT;
  int len = 0;
  xLibWa_t *wap = (xLibWa_t *) cntx;
  xLibObj_t *objp = &wap->objs[0];
  int size = 0;
#if 0
  /* Allocate the memory to hold values */
  objp->value = xLibMalloc (XLIB_MAX_VAL_LEN);
#endif

  retval = (char *) &(wap->ui.web.htmlBuffer[wap->ui.web.htmlBufferOffset]);
  size = MAX_WAP_BUFFER_SIZE;

  objp->oid = cpoid ? cpoid : oid;
  objp->cpoid = 0;
  objp->type = type;

  xLibFilterPush (wap, oid);

  if (cpoid != 0)
  {
    xLibFilterPush (wap, cpoid);
  }

  while (max-- > 0)
  {
    /* Get the object value from the handler */
    objp->valen = sizeof (XLIB_MAX_VAL_LEN);
    if (func == NULL)
    {
      objp->rc = xLibObjGet (wap, objp->oid, objp->cpoid, objp->value, (xLibU16_t *) & objp->valen);
    }
    else
    {
      xLibBuf_t bd;
      xLibBufDataInit (wap, &bd, oid);
      objp->rc = func (wap, &bd);
      if (objp->rc == XLIBRC_SUCCESS)
      {
        memcpy (objp->value, bd.value, bd.valen);
        objp->valen = bd.valen;
      }
    }

    if (objp->rc != XLIBRC_SUCCESS)
    {
      if (objp->rc != XLIBRC_ENDOF_TABLE && objp->rc != XLIBRC_IGNORE)
      {
        XLIB_WARN ("xLibObjGetMayBeKey: failed for 0x%08x: rc = %s\r\n", objp->oid,
            xLibRcStrErr (objp->rc));
      }
      break;
    }

    /* store the value as a filter also, so that other obj handler can use if needed */
    xLibFilterSet (wap, objp->oid, objp->type, objp->value, objp->valen);


    /* encode the value */
    wap->encLen = sizeof (wap->encBuf);
    objp->rc = xLibTypeEncode (XLIB_IF_WEB, objp->type, objp->value, objp->valen,
        wap->encBuf, &wap->encLen, XLIB_TRUE);
    if (objp->rc != XLIBRC_SUCCESS)
    {
      XLIB_WARN ("xLibTypeEncode: failed for 0x%08x: rc = %s\r\n", objp->oid,
          xLibRcStrErr (objp->rc));
      break;
    }
    else
    {
      XLIB_TRACE ("xLibTypeEncode: success for 0x%08x to %s\r\n", objp->oid, wap->encBuf);
    }

    retval = xLibWeb2KeyAdd (retval, &len, &size, wap->encBuf);
  }

  if (max <= 0)
  {
    XLIB_ERROR ("xLibWeb2KeyOrDynamicString infinite loop\r\n");
  }

  if (cpoid != 0)
  {
    xLibFilterPop (wap, cpoid);
  }
  xLibFilterPop (wap, oid);

  if (retval != NULL)
  {
    retval[strlen (retval) - 1] = 0;    /* remove the last comma */
    if (wap->ui.web.htmlBufferOffset + len >= MAX_WAP_BUFFER_SIZE)
    {
      XLIB_ERROR ("XLIB ERROR:ui.web.htmlBuffer offset greater than 64K!!!");
    }
    else
    {
      wap->ui.web.htmlBufferOffset += len;
    }
  }
  else
  {
    retval = "";
  }

#if 0
  xLibFree (objp->value);
#endif

  return retval;
}

char *xLibWeb2LoadPseudoKey (void *cntx, xLibId_t oid, xLibU16_t type, xLibId_t keyoid,
    xLibU16_t keytype)
{
  char *retval = NULL;
  int max = MAX_KEYS_COUNT;
  int len = 0;
  int size = 0;
  xLibWa_t *wap = (xLibWa_t *) cntx;
  xLibObj_t *objp = &wap->objs[0];
  xLibObj_t *keyobjp = &wap->objs[1];

  objp->oid = oid;
  objp->cpoid = 0;
  objp->type = type;

  keyobjp->oid = keyoid;
  keyobjp->cpoid = 0;
  keyobjp->type = keytype;

  xLibFilterPush (wap, oid);
  xLibFilterPush (wap, keyoid);

  retval = (char *) &(wap->ui.web.htmlBuffer[wap->ui.web.htmlBufferOffset]);
  size = sizeof (wap->ui.web.htmlBuffer);

  if (size != MAX_WAP_BUFFER_SIZE)
  {
    XLIB_ERROR ("\nWap buffer could not be allocated!!!\r\n");
  }


  while (max-- > 0)
  {
    /* Get the object value from the handler */
    objp->valen = sizeof (objp->value);
    keyobjp->valen = sizeof (keyobjp->value);
    /* iterate through all the possible values of key */

    keyobjp->rc = xLibObjGet (wap, keyobjp->oid, keyobjp->cpoid, keyobjp->value, &keyobjp->valen);

    if (keyobjp->rc != XLIBRC_SUCCESS && keyobjp->rc != XLIBRC_ENDOF_TABLE)
    {
      XLIB_WARN ("xLibObjGetMayBeKey: failed for 0x%08x: rc = %s\r\n", keyobjp->oid,
          xLibRcStrErr (keyobjp->rc));
      break;
    }
    if (keyobjp->rc == XLIBRC_ENDOF_TABLE)
    {
      break;
    }
    /* set the key value to filter */

    xLibFilterSet (wap, keyobjp->oid, keyobjp->type, keyobjp->value, keyobjp->valen);

    objp->rc = xLibObjGet (wap, objp->oid, objp->cpoid, objp->value, &objp->valen);

    if (objp->rc != XLIBRC_SUCCESS)
    {
      continue;
    }


    /* store the value as a filter also, so that other obj handler can use if needed */
    xLibFilterSet (wap, objp->oid, objp->type, objp->value, objp->valen);


    /*invoke the load handler for ui objects */
           xLibObjInvokeLoad(wap,objp);


    /* encode the value */
    wap->encLen = sizeof (wap->encBuf);
    objp->rc = xLibTypeEncode (XLIB_IF_WEB, objp->type, objp->value, objp->valen,
        wap->encBuf, &wap->encLen, XLIB_TRUE);
    if (objp->rc != XLIBRC_SUCCESS)
    {
      XLIB_WARN ("xLibTypeEncode: failed for 0x%08x: rc = %s\r\n", objp->oid,
          xLibRcStrErr (objp->rc));
      break;
    }
    else
    {
      XLIB_TRACE ("xLibTypeEncode: success for 0x%08x to %s\r\n", objp->oid, wap->encBuf);
    }

    retval = xLibWeb2KeyAdd (retval, &len, &size, wap->encBuf);

  }                             /* end while */

  if (max <= 0)
  {
    XLIB_ERROR ("xLibWeb2LoadPsuedoKey infinite loop\r\n");
  }

  xLibFilterPop (wap, oid);
  xLibFilterPop (wap, keyoid);

  if (retval == NULL)
  {
    return "";
  }

  retval[strlen (retval) - 1] = 0;      /* remove the last comma */
  return retval;
}


/* This function maps the error code to error message
 * if the error need to be ignored than the error code
 * need to get mapped to SUCCESS
 * this function retuns if the error is effective or not
 */
static xLibBool_t xLibWeb2RecordError (xLibU32_t rowindex,void *cntx, xLibId_t oid, xLibRC_t rc, xLibS8_t * value)
{
  char buf[1024];
  void *name;
  xLibRC_t  gmap;
  int i, j;
  xLibWa_t *wap = (xLibWa_t *) cntx;
  xLibU16_t size;
  char *ptr = emwebErrorBufGet (wap->ui.web.webCntxt, &size);
  for (i = 0; i < wap->objCount; i++)
  {
    if (wap->objs[i].oid == oid)
    {
      for (j = 0; j < wap->objs[i].mCount; j++)
      {
        if (wap->objs[i].code[j] == rc)
        {
          rc = wap->objs[i].mCode[j];
          break;
        }
      }
      break;
    }
  }
  if (rc == XLIBRC_SUCCESS)
  {
    /* error ignored using error map */
    return XLIB_FALSE;
  }
  /* XE_XLIB_ERROR_USE_WEB_NAME will be deined in auto generated code of web names are mentioned to be used in branding.xml */
  #ifdef XE_XLIB_ERROR_USE_ELEMENT_NAME
     name = (xLibS8_t *)wap->objs[i].ui.web.name;
  #else
     name = NULL;
  #endif


  gmap = xLibGlobalMapGet(rc);
 /* if globalmap exists */
  if(gmap != XLIBRC_SUCCESS)
  {
   rc = gmap;
  }

  xLibOEMErrorOutputNameGet (rowindex,oid, (xLibS8_t *)name,value, rc, buf, sizeof (buf));
  if (strlen (ptr) + strlen (buf) < size)
  {
    strcat (ptr, buf);
    strcat (ptr, "\r\n");
  }
  return XLIB_TRUE;
}

void xLibWeb2LoadError (void *cntx, xLibS8_t * *eval, xLibU8_t * estat, xLibU32_t * fval,
    xLibU8_t * fstat)
{
  xLibU16_t size;
  xLibWa_t *wap = (xLibWa_t *) cntx;
  char *ptr = emwebErrorBufGet (wap->ui.web.webCntxt, &size);

  *fstat |= EW_FORM_INITIALIZED;
  *estat |= EW_FORM_INITIALIZED;
  *fval = 0;
  *eval = NULL;

  if (strlen (ptr) > 0)
  {
    *fval = 1;
    *eval = (char *) emwebStrdup (ptr);
    *estat |= EW_FORM_DYNAMIC;
    XLIB_WARN ("%s", ptr);
  }
}

void xLibWeb2SetSubmitType (void *cntx, unsigned int type)
{
  xLibWa_t *wap = (xLibWa_t *) cntx;
  if(wap != NULL)
  {
    emwebFormSubmitSet (wap->ui.web.webCntxt, type);
  }
}

char *xLibWeb2AllowAccess (void *srvCntx, int which)
{
  int usrLevel;
  int level;
  char strLevel[32];
  char *retval = emwebLevelCheckBuffer (srvCntx);

  usrLevel = emwebUserAccessGet (srvCntx);
  memset (strLevel, 0x0, 32);
  osapiStrncpySafe (strLevel, emwebContextGetValue (srvCntx), sizeof(strLevel));
  level = atoi (strLevel);
  strcpy (retval, "TRUE");

  switch (which)
  {
    case XLIB_WEB2_ACCESS_LESSER:
      return (usrLevel < level) ? retval : NULL;
    case XLIB_WEB2_ACCESS_EQUAL:
      return (usrLevel == level) ? retval : NULL;
    case XLIB_WEB2_ACCESS_GREATER:
      return (usrLevel > level) ? retval : NULL;
    default:
      break;
  }
  return NULL;
}

static xLibRC_t xLibWeb2ValidateIntMinMax (xLibId_t oid,xLibU16_t type,xLibS8_t * value)
{
  xLibUL64_t uintval = 0;
  xLibSL64_t intval = 0;
  xLibS8_t minBuf[16];
  xLibS8_t maxBuf[16];
  xLibS8_t *endptr;
  xLibSL32_t intmin = 0;
  xLibUL32_t uintmin = 0;

  xLibSL32_t intmax = 0;
  xLibUL32_t uintmax = 0;

  memset(minBuf,0x0,sizeof(minBuf));
  memset(maxBuf,0x0,sizeof(maxBuf));

  if(type == XLIB_DTYPE_int)
  {
     intval = strtol(value, &endptr, 10);
     errno = 0;
     if (errno == ERANGE)
      {
        if(intval == LONG_MIN)
          return XLIBRC_ERROR_MIN;
        if(intval == LONG_MAX)
          return XLIBRC_ERROR_MAX;
      }
     if(xLibObjMinIntGet(oid,minBuf) != -1)
      {
          sscanf(minBuf,"%ld",&intmin);
      }
      else
      {
           /*Failed to get min. so return success for now.*/
          return XLIBRC_SUCCESS;
      }

      if(xLibObjMaxIntGet(oid,maxBuf) != -1)
      {
          sscanf(maxBuf,"%ld",&intmax);
      }
      else
      {
           /*Failed to get max. so return success*/
          return XLIBRC_SUCCESS;
      }
      if (intval < intmin)
      {
           return XLIBRC_ERROR_MIN;
      }
      if ( intval > intmax)
      {
           return XLIBRC_ERROR_MAX;
      }
  }
  else if( type == XLIB_DTYPE_uint)
  {

      uintval = strtoul(value,&endptr,10);
      errno = 0;
      if (errno == ERANGE)
      {
        if (uintval == LONG_MIN)
          return XLIBRC_ERROR_MIN;
        if (uintval == LONG_MAX)
          return XLIBRC_ERROR_MAX;
      }
      if(xLibObjMinIntGet(oid,minBuf) != -1)
      {
           sscanf(minBuf,"%lu",&uintmin);
      }
      else
      {
            /*Failed to get min. so return success for now.*/
           return XLIBRC_SUCCESS;
      }

      if(xLibObjMaxIntGet(oid,maxBuf) != -1)
      {
          sscanf(maxBuf,"%lu",&uintmax);
      }
      else
      {
           /*Failed to get max. so return success*/
          return XLIBRC_SUCCESS;
      }
      if (uintval < uintmin)
      {
           return XLIBRC_ERROR_MIN;
      }
      if ( uintval > uintmax)
      {
           return XLIBRC_ERROR_MAX;
      }

  }


  return XLIBRC_SUCCESS;
}

char *xLibWeb2BufPrint (void *context, int oid, char *msg)
{
  xLibWa_t *wap = xLibWeb2WorkAreaGet (context);
  if(wap != NULL)
  {
    xLibObjBufPrint (oid, NULL,"", msg, wap->ui.web.buf1024, sizeof (wap->ui.web.buf1024));
    return wap->ui.web.buf1024;
  }
  return "";
}

char *xLibWeb2PrintString (void *context, int oid, int nlsId, ...)
{
  va_list args;
  char *fmt = (char *) xLibOEMStringGet (nlsId);
  char *argStr = xLibWeb2BufPrint (context, oid, fmt);
  xLibWa_t *wap = xLibWeb2WorkAreaGet (context);
  if(wap != NULL)
  {
    va_start (args, nlsId);
    memset (wap->ui.web.buf256, 0x0, sizeof (wap->ui.web.buf256));
    vsprintf (wap->ui.web.buf256, argStr, args);
    va_end (args);
    return wap->ui.web.buf256;
  }
  return "";
}

void xLibWeb2InitSubmitType (void *cntx, unsigned long *var, unsigned char *status)
{
  unsigned int val = 0;
  emwebFormValueSet ((char **) var, status, (char *) &val, 0);
}

void xLibWeb2VarServeSet (char **var, unsigned char *status, const char *name)
{
  emwebFormValueSet (var, status, (char *) name, 1);
}

void xLibWeb2VarSubmitSet (char **var, unsigned char *status, const char *name)
{
  if (*status != 0)
  {
    emwebFormValueSet (var, status, (char *) name, 1);
  }
}

void xLibWeb2FileNameSet (char **var, unsigned char *status, void *handle)
{
  if (handle != NULL)
  {
    char *name = emwebFileNameGet (handle);
    emwebFormValueSet (var, status, (char *) name, 1);
  }
}

static void xLib2WebDebugDump (xLibWa_t * wap, char *msg)
{
  int i;
  xLibS8_t encBuf[256];
  xLibU16_t encLen;
  xLibFilter_t *fp;

  if (xLibTraceLevel < 2)
  {
    return;
  }

  sysapiPrintf ("=DBG=> %s : KEYS = ", msg);
  for (i = 0; i < wap->keyCount; i++)
  {
    sysapiPrintf ("%x ", wap->keys[i].oid);
  }
  sysapiPrintf (":: OBJECTS = ");
  for (i = 0; i < wap->objCount; i++)
  {
    sysapiPrintf ("%x ", wap->objs[i].oid);
  }

  sysapiPrintf (":: FILTERS = ");
  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    encLen = sizeof (encBuf);
    memset (encBuf, 0, sizeof (encBuf));
    xLibTypeEncode (XLIB_IF_WEB, fp->type, fp->value, fp->valen, encBuf, &encLen, XLIB_TRUE);
    sysapiPrintf ("(0x%08x %s) ", fp->oid, encBuf);
  }

  sysapiPrintf ("\r\n");
}

static void xLibWeb2FilterDumpLoad (xLibWa_t * wap)
{
  char tmpBuf[64];
  int i, j, k, oid, dlen, type, len;
  char *filterDump;

  if ((filterDump = emwebFilterDumpGet (wap->ui.web.webCntxt, XLIB_TRUE)) == NULL)
  {
    return;
  }
  len = strlen (filterDump);

  for (i = 0, k = 0; i < len; i += k)
  {
    sscanf (&filterDump[i], "0x%08x %d %d ", &oid, &type, &dlen);
    sprintf (tmpBuf, "0x%08x %d %d ", oid, dlen, type);
    k = strlen (tmpBuf);
    filterDump[i + k + dlen] = 0;
    for (j = 0; j < wap->keyCount; j++)
    {
      if (wap->keys[j].oid == oid)
      {
        xLibObj_t *objp = &wap->objs[0];

        objp->valen = XLIB_MAX_VAL_LEN;
        memset (objp->value, 0, objp->valen);
        objp->rc = xLibTypeDecode (XLIB_IF_WEB, type, &filterDump[i + k], objp->value,
            (xLibU16_t *) & objp->valen, XLIB_TRUE);
        if (objp->rc != XLIBRC_SUCCESS)
        {
          XLIB_WARN ("xLibWebDecode: failed for 0x%08x rc = 0x%x", oid, objp->rc);
        }
        else
        {
          xLibFilterSet (wap, oid, type, objp->value, objp->valen);
        }
        break;
      }
    }
    k = k + dlen + 1;
  }
}

char *xLibWeb2ErrorMessageGet (void *servContext, xLibId_t oid, xLibU32_t nlsId, xLibRC_t rc)
{
  static char ptr[1024];
  char value[20];
  char *temp;

  #ifdef XE_XLIB_ERROR_USE_ELEMENT_NAME
     char name[20];
     strcpy(name,"$$objName$$");
  #else
     void *name;
     name = NULL;
  #endif

  strcpy (value, "$$objValue$$");
  memset (ptr, 0, sizeof (ptr));
  temp = (char *) xLibOEMErrorOutputNameGet (0,oid, name, value, rc, ptr, sizeof (ptr));
  if (NULL != temp)
  {
    if (*temp == '\r')
    {
      if (temp + 2 != NULL)
      {
        return temp + 2;
      }
    }
    else if (*temp == '\n')
    {
      if (temp + 1 != NULL)
      {
        return temp + 1;
      }
    }
  }
  return temp;
}

char *xLibWeb2NumberGet (void *servContext)
{
  char *buf16;
  int tok = emwebGetReqIntValue (servContext, &buf16);
  return (char *) xLibNumberGet (tok, buf16);
}

char *xLibWeb2RepeatIsFirst (void *context, short index)
{
  return emwebRepeatIsFirst (context, index);
}

char *xLibWeb2RepeatIsLast (void *context, short index)
{
  return emwebRepeatIsLast (context, index);
}

char *xLibWebGetRepeatCount (void *context, short index)
{
  extern int  emwebGetRepeatCount (void* context, short index);
  int count = emwebGetRepeatCount (context, index);
  if( count == 0)
  {
    return "TRUE";
  }
  else
  {
    return NULL;
  }
}

/* TO BE REMOVED AFTER TOOL CHANGE */
char *xLibObjBufPrintAlloc (void *context, int oid, char *msg)
{
  return xLibWeb2BufPrint (context, oid, msg);
}

char* xLibWeb2GetMaximumInt(xLibId_t oid)
{
 static char buf[20];
 memset(buf,0x0,sizeof(buf));
 if(xLibObjMaxIntGet(oid,buf) == -1)
 {
   return "";
 }
 return buf;
}


char* xLibWeb2GetMinimumInt(xLibId_t oid)
{
 static char buf[20];
 memset(buf,0x0,sizeof(buf));
 if(xLibObjMinIntGet(oid,buf) == -1)
 {
   return "";
 }
 return buf;
}

xLibRC_t xLibTransferProgressCheckSet(int (*callback)(void *), void *arg, char *url1, char *url2)
{
  emwebTransferProgressCheckSet(callback, arg, url1, url2);
  return XLIBRC_SUCCESS;
}

void xLibWeb2RepeatSetOnSelect (void *cntx, xLibU16_t flagVal, xLibU16_t rep,
    xLibU16_t varSize, xLibU16_t statSize)
{
  xLibBool_t first = XLIB_TRUE;
  xLibBool_t err;
  xLibU16_t i, j, k;
  xLibU32_t rownum = 0;
  xLibObj_t *objp = NULL;
  xLibObj_t *repobj = NULL;
  xLibWa_t *wap = xLibWeb2ReqInit (cntx, "xLibWeb2RepeatSetOnSelect start");
  xLibS16_t select_col_index = -1;

  /* preserve the filter keys */
  xLibFilterPushFromKeys (wap);

  for (i = 0; i < wap->objCount; i++)
  {
    if(wap->objs[i].flag & XLIB_IS_SELECTOR)
    {
      select_col_index = i;
      break;
    }
  }

  /* repeat only as many times asked for */
  while (rep-- > 0)
  {
    rownum++;

    /* get the next set of values - except first time */
    for (i = 0; i < wap->objCount && first == XLIB_FALSE; i++)
    {
      xLibWebAddOffset (wap->objs[i].ui.web.var, varSize);
      xLibWebAddOffset (wap->objs[i].ui.web.status, statSize);
    }
    first = XLIB_FALSE;

    /* For all the objects in the request
     *  - decode the value
     *  - set the filter
     */

    for (i = 0; i < wap->objCount; i++)
    {
      objp = &wap->objs[i];
      if ((objp->flag & XLIB_IS_SELECTOR) || \
          ((select_col_index!= -1) && \
           ((*(wap->objs[select_col_index].ui.web.var) == NULL) || \
            (strlen(*(wap->objs[select_col_index].ui.web.var)) <= 0) || \
            *(wap->objs[select_col_index].ui.web.var)[0] == 'D')))
      {
        continue;
      }

      /* init error code */
      objp->rc = XLIBRC_SUCCESS;

      /* skip disabled elements and decode failed elements */
      if (objp->rc != XLIBRC_SUCCESS || XLIB_TRUE == xLibWeb2IsDisable (objp))
      {
        continue;
      }
      if ((emwebFormSubmitGet (wap->ui.web.webCntxt) == XLIB_SUBMIT_FLAG_SUBMIT)
           && (objp->flag & XLIB_IS_VALIDATE))
      {
        /* validate the data before decoding */
        objp->rc = xLibValidate (wap, i, objp->oid, objp->type, *objp->ui.web.var);
        if (objp->rc != XLIBRC_SUCCESS)
        {
          err = xLibWeb2RecordError (rownum, wap, objp->oid, objp->rc, *objp->ui.web.var);
          XLIB_WARN2 (err, "xLibValidate: failed for 0x%08x rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
          continue;
        }
        objp->rc = xLibWeb2ValidateIntMinMax (objp->oid, objp->type, *objp->ui.web.var);
        if (objp->rc != XLIBRC_SUCCESS)
        {
          err = xLibWeb2RecordError (rownum, wap, objp->oid, objp->rc, *objp->ui.web.var);
          XLIB_WARN2 (err, "xLibValidate: failed for 0x%08x rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
          continue;
        }
      }

      /* decode the value */
      objp->valen = XLIB_MAX_VAL_LEN;
      objp->rc = xLibTypeDecode (XLIB_IF_WEB, objp->type, *objp->ui.web.var,
      objp->value, (xLibU16_t *) & objp->valen, XLIB_TRUE);
      XLIB_TRACE ("xLibTypeDecode (%s): retval for 0x%08x rc = %s\r\n",
      *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
      if (objp->rc != XLIBRC_SUCCESS)
      {
        XLIB_WARN ("xLibTypeDecode (%s): failed for 0x%08x rc = %s\r\n",
        *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
        continue;
      }

      /* store the value as a filter also, so that obj handler can use if needed */
      xLibFilterSet (wap, objp->oid, objp->type, objp->value, objp->valen);

      /*invoke the load handler for ui objects */
      xLibObjInvokeLoad(wap,objp);


      /* set the row status flag */
      if (xLibIsRowStatusType (objp->type) == XLIB_TRUE)
      {
        objp->rc = xLibRowInfoCheck (objp->oid, objp->type, objp->value);
      }
    }

    xLib2WebDebugDump (wap, "xLibWeb2RepeatSetOnSelect iter");

    /* now we have all the info in the work area, set now - row status add */
    for (i = 0; i < wap->objCount; i++)
    {
      objp = &wap->objs[i];

      if ((objp->flag & XLIB_IS_SELECTOR) || \
          ((select_col_index!= -1) && \
           ((*(wap->objs[select_col_index].ui.web.var) == NULL) || \
            (strlen(*(wap->objs[select_col_index].ui.web.var)) <= 0) || \
            *(wap->objs[select_col_index].ui.web.var)[0] == 'D')))
      {
        continue;
      }

      if (objp->rc == XLIBRC_ROW_STATUS_ADD)
      {
        if (emwebFormSubmitGet (wap->ui.web.webCntxt) != XLIB_SUBMIT_FLAG_SUBMIT)
        {
         continue;
        }

        objp->rc = xLibObjSet (wap, objp->oid, objp->value, objp->valen);
        if (objp->rc != XLIBRC_SUCCESS)
        {
          err = xLibWeb2RecordError (rownum, wap, objp->oid, objp->rc, *objp->ui.web.var);
          XLIB_WARN2 (err, "xLibWeb2Set: failed for 0x%08x rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));

          /* clear the filters  for dependent list for row status object */
          for (j = 0; j < objp->depCount; j++)
          {
             for(k=0; k < wap->objCount; k++)
             {
               /* clear the filter only if the key is present in repeat */
               repobj = &wap->objs[k];
               if(repobj->oid == objp->depList[j])
                  xLibFilterClear (wap, objp->depList[j]);
               }
          }

          xLibWeb2ReqFinish (wap, "xLibWeb2Set end");
          return;
        }
        else
        {
          objp->rc = XLIBRC_ROW_STATUS_ADD;
        }
      }
    }

    /* now we have all the info in the work area, set now */
    for (i = 0; i < wap->objCount; i++)
    {
      objp = &wap->objs[i];
      if ((objp->flag & XLIB_IS_SELECTOR) || \
          ((select_col_index!= -1) && \
           ((*(wap->objs[select_col_index].ui.web.var) == NULL) || \
            (strlen(*(wap->objs[select_col_index].ui.web.var)) <= 0) || \
            *(wap->objs[select_col_index].ui.web.var)[0] == 'D')))
      {
        continue;
      }

      /* skip disabled elements and decode failed elements */
      if (objp->rc != XLIBRC_SUCCESS || XLIB_TRUE == xLibWeb2IsDisable (objp))
      {
        continue;
      }

      /* just a refresh - Is this valid for repeat - NEED TO CHECK */
      if (emwebFormSubmitGet (wap->ui.web.webCntxt) != XLIB_SUBMIT_FLAG_SUBMIT)
      {
        continue;
      }

      /* set the object value */
      objp->rc = xLibObjSet (wap, objp->oid, objp->value, objp->valen);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        err = xLibWeb2RecordError (rownum, wap, objp->oid, objp->rc, *objp->ui.web.var);
        XLIB_WARN2 (err, "xLibWeb2RepeatSetOnSelect: failed for 0x%08x rc = %s\r\n", objp->oid,
            xLibRcStrErr (objp->rc));
      }
    }

    /* when object is row status type and value is delete */
    for (i = 0; i < wap->objCount; i++)
    {
      objp = &wap->objs[i];
      if ((objp->flag & XLIB_IS_SELECTOR) || \
          ((select_col_index!= -1) && \
           ((*(wap->objs[select_col_index].ui.web.var) == NULL) || \
            (strlen(*(wap->objs[select_col_index].ui.web.var)) <= 0) || \
            *(wap->objs[select_col_index].ui.web.var)[0] == 'D')))
      {
        continue;
      }

      if (objp->rc == XLIBRC_ROW_STATUS_DELETE)
      {
        /* set the object value */
        if (emwebFormSubmitGet (wap->ui.web.webCntxt) != XLIB_SUBMIT_FLAG_SUBMIT)
        {
          continue;
        }
        objp->rc = xLibObjSet (wap, objp->oid, objp->value, objp->valen);
        if (objp->rc != XLIBRC_SUCCESS)
        {
          err = xLibWeb2RecordError (rownum, wap, objp->oid, objp->rc, *objp->ui.web.var);
          XLIB_WARN2 (err, "xLibWeb2RepeatSetOnSelect: failed for 0x%08x rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
        }
        else
        {
          /* clear the filters  for dependent list for row status object when delete */
          for (j = 0; j < objp->depCount; j++)
          {
            for(k=0; k < wap->objCount; k++)
            {
              /* clear the filter only if the key is present in repeat */
              repobj = &wap->objs[k];
              if(repobj->oid == objp->depList[j])
              xLibFilterClear (wap, objp->depList[j]);
            }
          }
        }
      }
    }

    /* set the flag to zero for the filters that are reserved by ui objects inside a repeat
     otherwise, we cannot update the filters with next entries in table.This call calong with tge below call
     xLibReserveUnreservedFilters shall be removed when creation of duplicate filters is removed
     */
    xLibUndoReserveFilters(wap);
  }

  /*Reserve the filters that are unreserved */
  /* Check why this is needed */
  xLibReserveUnreservedFilters(wap);

  /* get the pushed values back */
  xLibFilterPopFromKeys (wap);

  xLibWeb2SetFinish(wap);
  xLibWeb2ReqFinish (wap, "xLibWeb2RepeatSetOnSelect end");

}

void xLibWeb2RepeatGlobalHybridEditSet (void *cntx, xLibU16_t flagVal, xLibU16_t rep,
    xLibU16_t varSize, xLibU16_t statSize)
{
  xLibBool_t first = XLIB_TRUE;
  xLibBool_t err, isglbele;
  xLibU16_t i, j;
  xLibObj_t *objp = NULL;
  xLibObj_t *glObjp = NULL;
  xLibWa_t *wap = xLibWeb2ReqInit (cntx, "xLibWeb2RepeatGlobalHybridEditSet start");
  xLibS16_t selectColIndex = -1;
  xLibU16_t localStart, localEnd , globalStart, globalEnd;
  xLibU32_t rownum = 0;
  globalStart = 0;
  globalEnd = 0;
  localStart = 0;
  localEnd = wap->objCount;


  /* preserve the filter keys */
  xLibFilterPushFromKeys (wap);
  if (rep > 0)
  {
    /* Find local and global object indexes. */
    for (i = 0; i < wap->objCount; i++)
    {
      if((wap->objs[i].flag & XLIB_IS_GLOBAL_EDIT) == 0)
      {
        break;
      }
      localStart++;
      globalEnd++;
    }

  /* find selector index in local rows. */
    for (i = localStart; i < wap->objCount; i++)
    {
      if(wap->objs[i].flag & XLIB_IS_SELECTOR)
      {
        selectColIndex = i;
        break;
      }
    }
  }

  /* Validate global data and decode the values for the objects. */
  for (i = globalStart; i < globalEnd; i++)
  {
    objp = &wap->objs[i];
    if (objp->flag & XLIB_IS_SELECTOR)
    {
      continue;
    }

    /* init error code */
    objp->rc = XLIBRC_SUCCESS;

    /* skip disabled elements and decode failed elements */
    if (objp->rc != XLIBRC_SUCCESS || XLIB_TRUE == xLibWeb2IsDisable (objp))
    {
      continue;
    }
    if ((emwebFormSubmitGet (wap->ui.web.webCntxt) == XLIB_SUBMIT_FLAG_SUBMIT)
        && (objp->flag & XLIB_IS_VALIDATE))
    {
      /* validate the data before decoding */
      objp->rc = xLibValidate (wap, i, objp->oid, objp->type, *objp->ui.web.var);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        err = xLibWeb2RecordError (0, wap, objp->oid, objp->rc, *objp->ui.web.var);
        XLIB_WARN2 (err, "xLibValidate: failed for 0x%08x rc = %s\r\n", objp->oid,
               xLibRcStrErr (objp->rc));
        continue;
      }
      objp->rc = xLibWeb2ValidateIntMinMax (objp->oid, objp->type, *objp->ui.web.var);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        err = xLibWeb2RecordError (0, wap, objp->oid, objp->rc, *objp->ui.web.var);
        XLIB_WARN2 (err, "xLibValidate: failed for 0x%08x rc = %s\r\n", objp->oid,
             xLibRcStrErr (objp->rc));
        continue;
      }
    }

    /* decode the value */
    objp->valen = XLIB_MAX_VAL_LEN;
    objp->rc = xLibTypeDecode (XLIB_IF_WEB, objp->type, *(objp->ui.web.var),
                  objp->value, (xLibU16_t *) & objp->valen, XLIB_TRUE);
    XLIB_TRACE ("xLibTypeDecode (%s): retval for 0x%08x rc = %s\r\n",
         *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
    if (objp->rc != XLIBRC_SUCCESS)
    {
      XLIB_WARN ("xLibTypeDecode (%s): failed for 0x%08x rc = %s\r\n",
          *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
      continue;
    }
  }

  /* repeat only as many times asked for */
  while (rep-- > 0)
  {
    rownum++;

    /* get the next set of values - except first time */
    for (i = localStart; i < localEnd && first == XLIB_FALSE; i++)
    {
      xLibWebAddOffset (wap->objs[i].ui.web.var, varSize);
      xLibWebAddOffset (wap->objs[i].ui.web.status, statSize);
    }
    first = XLIB_FALSE;
    if ((selectColIndex!= -1) && \
        ((*(wap->objs[selectColIndex].ui.web.var) == NULL) || \
         (strlen(*(wap->objs[selectColIndex].ui.web.var)) <= 0) || \
         *(wap->objs[selectColIndex].ui.web.var)[0] == 'D'))
    {
      continue;
    }

    /* For all the objects in the request
     *  - decode the global value and set it in object value
     *  - set the filter
    */
    xLib2WebDebugDump (wap, "xLibWeb2RepeatGlobalHybridEditSet iter");

    /* now we have all the info in the work area, set now */
    for (i = localStart; i < localEnd; i++)
    {
      objp = &wap->objs[i];
      if (objp->flag & XLIB_IS_SELECTOR)
      {
        continue;
      }

      /* skip disabled elements and decode failed elements */
      if (XLIB_TRUE == xLibWeb2IsDisable (objp))
      {
        continue;
      }
      objp->rc = XLIBRC_SUCCESS;
      objp->valen = XLIB_MAX_VAL_LEN;
      isglbele = XLIB_FALSE;

      for(j=globalStart; j < globalEnd; j++)
      {
        glObjp = &wap->objs[j];
        if((glObjp->oid == objp->oid) && (glObjp->valen > 0))
        {
          isglbele = XLIB_TRUE;
          break;
        }
      }

      /* match found for an element in global row */
      /* decode the global object value and set it in local object */
      if (isglbele == XLIB_TRUE)
      {
        /* decode the global value and set it in local object. */
        objp->rc = xLibTypeDecode (XLIB_IF_WEB, objp->type, *(glObjp->ui.web.var),
        objp->value, (xLibU16_t *) & objp->valen, XLIB_TRUE);
        XLIB_TRACE ("xLibTypeDecode (%s): retval for 0x%08x rc = %s\r\n",
              *glObjp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
        if (objp->rc != XLIBRC_SUCCESS)
        {
          XLIB_WARN ("xLibTypeDecode (%s): failed for 0x%08x rc = %s\r\n",
              *glObjp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
          continue;
        }
      }
      else
      {
        /* decode the local value */
        objp->rc = xLibTypeDecode (XLIB_IF_WEB, objp->type, *(objp->ui.web.var),
        objp->value, (xLibU16_t *) & objp->valen, XLIB_TRUE);
        XLIB_TRACE ("xLibTypeDecode (%s): retval for 0x%08x rc = %s\r\n",
              *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
        if (objp->rc != XLIBRC_SUCCESS)
        {
          XLIB_WARN ("xLibTypeDecode (%s): failed for 0x%08x rc = %s\r\n",
              *objp->ui.web.var, objp->oid, xLibRcStrErr (objp->rc));
          continue;
        }
      }

      /* store the value as a filter also, so that obj handler can use if needed */
      xLibFilterSet (wap, objp->oid, objp->type, objp->value, objp->valen);

      /*invoke the load handler for ui objects */
      xLibObjInvokeLoad(wap,objp);

      /* set the row status flag */
      if (xLibIsRowStatusType (objp->type) == XLIB_TRUE)
      {
        objp->rc = xLibRowInfoCheck (objp->oid, objp->type, objp->value);
      }
    }

    /* Peform submit operation. */
    for (i = localStart; i < localEnd; i++)
    {
      objp = &wap->objs[i];
      if (objp->flag & XLIB_IS_SELECTOR)
      {
        continue;
      }

      /* skip disabled elements and decode failed elements */
      if (XLIB_TRUE == xLibWeb2IsDisable (objp))
      {
        continue;
      }

      if (objp->rc != XLIBRC_SUCCESS)
      {
        continue;
      }

      /* just a refresh - Is this valid for repeat - NEED TO CHECK */
      if (emwebFormSubmitGet (wap->ui.web.webCntxt) != XLIB_SUBMIT_FLAG_SUBMIT)
      {
        continue;
      }

      for(j=globalStart; j < globalEnd; j++)
      {
        glObjp = &wap->objs[j];
        if(glObjp->oid == objp->oid)
        {
          /* match found for an element in global row*/
          /* set the object value */
          if (glObjp->valen > 0)
          {
            objp->rc = xLibObjSet (wap, objp->oid, glObjp->value, glObjp->valen);
            /* set the object value */
            if (objp->rc != XLIBRC_SUCCESS)
            {
              err = xLibWeb2RecordError (rownum, wap, objp->oid, objp->rc, *objp->ui.web.var);
              XLIB_WARN2 (err, "xLibWeb2RepeatGlobalHybridEditSet: failed for 0x%08x rc = %s\r\n", objp->oid,
                  xLibRcStrErr (objp->rc));
            }
          }
        }
      }
    }

    /* set the flag to zero for the filters that are reserved by ui objects inside a repeat
       otherwise, we cannot update the filters with next entries in table.This call calong with tge below call
       xLibReserveUnreservedFilters shall be removed when creation of duplicate filters is removed
    */
    xLibUndoReserveFilters(wap);
  }

  /*Reserve the filters that are unreserved */
  /* Check why this is needed */
  xLibReserveUnreservedFilters(wap);

  /* get the pushed values back */
  xLibFilterPopFromKeys (wap);

  xLibWeb2SetFinish(wap);
  xLibWeb2ReqFinish (wap, "xLibWeb2RepeatGlobalHybridEditSet end");
}

char *xLibWeb2RepeatCountZero (void *cntx, xLibU16_t rptIndex, xLibU32_t pageMax, xLibU32_t split)
{
  xLibU32_t count;
  char *retval;
  xLibWa_t *wap = xLibWeb2ReqInit (cntx, "xLibWeb2RepeatCount start");

  xLibFilterPushFromKeys (wap);

  /* Load the filters from the dump for the pagination */
  if (pageMax != 0)
  {
    if (emwebFormSubmitGet (wap->ui.web.webCntxt) == XLIB_SUBMIT_FLAG_NEXT)
    {
      xLibWeb2FilterDumpLoad (wap);
    }
  }

  count = 0;

  xLibFilterPopFromKeys (wap);

  retval = emwebRepeatInitDetails (wap->ui.web.webCntxt, rptIndex, count, split, pageMax, XLIB_FALSE);
  xLibWeb2ReqFinish (wap, "xLibWeb2RepeatCountZero end");

  return retval;
}

char *xLibWeb2GetRow(void *cntx, short index, xLibS8_t *rowStart, xLibS8_t **cellStarts,
        xLibS8_t * rowClose, xLibS8_t * cellClose)
{
  xLibObj_t *objp;
  xLibWa_t *wap = xLibWeb2ReqInit (cntx, "xLibWeb2GetRow start");
  xLibS8_t *buf = wap->ui.web.buf2048;
  xLibS8_t outBuf[XLIB_MAX_VAL_LEN+1];
  xLibU16_t outBufLen,i;
  xLibBool_t isEncode;

  /* memset(wap->ui.web.buf2048,0,sizeof(wap->ui.web.buf2048));*/
  memset(buf,0,sizeof(wap->ui.web.buf2048));
  if (rowStart != NULL)
  {
    osapiStrncpySafe(buf, rowStart, sizeof(wap->ui.web.buf2048));
  }
  /* Read the element values based on objCount in wap */
  for(i=0;i<wap->objCount; i++)
  {
    isEncode = XLIB_TRUE;
    memset(outBuf,0,sizeof(outBuf));
    objp = &wap->objs[i];
    objp->rc = xLibObjGetMayBeKey (wap, objp);
    if (objp->rc != XLIBRC_SUCCESS)
    {
      if (objp->rc == XLIBRC_ENDOF_TABLE)
      {
        /* Code cannot come to this loop as condition is checked in fucntion xLibWeb2IsIterate and returns from there */
        XLIB_WARN ("Debug Statement END OF TABLE from xLibWeb2Iterate: for 0x%08x: rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
      }
      else
      {
        XLIB_WARN ("xLibObjGetMayBeKey failed: for 0x%08x: rc = %s\r\n", objp->oid,
            xLibRcStrErr (objp->rc));
        /* objp->value is set to blank string for display purpose */

      }
      isEncode = XLIB_FALSE;
    }

    if( isEncode == XLIB_TRUE)
    {
      /* store the value as a filter also, so that other obj handler can use if needed */
      xLibFilterSet (wap, objp->oid, objp->type, objp->value, objp->valen);
    }
  }

  /* Read the element values based on objCount in wap */
  for(i=0;i<wap->objCount; i++)
  {
    isEncode = XLIB_TRUE;
    memset(outBuf,0,sizeof(outBuf));
    objp = &wap->objs[i];
    objp->rc = xLibObjGetMayBeKey (wap, objp);
    if (objp->rc != XLIBRC_SUCCESS)
    {
      if (objp->rc == XLIBRC_ENDOF_TABLE)
      {
        /* Code cannot come to this loop as condition is checked in fucntion xLibWeb2IsIterate and returns from there */
        XLIB_WARN ("Debug Statement END OF TABLE from xLibWeb2Iterate: for 0x%08x: rc = %s\r\n", objp->oid,
              xLibRcStrErr (objp->rc));
      }
      else
      {
        XLIB_WARN ("xLibObjGetMayBeKey failed: for 0x%08x: rc = %s\r\n", objp->oid,
            xLibRcStrErr (objp->rc));
        /* objp->value is set to blank string for display purpose */

      }
      isEncode = XLIB_FALSE;
    }

    if( isEncode == XLIB_TRUE)
    {
      /* store the value as a filter also, so that other obj handler can use if needed */
      xLibFilterSet (wap, objp->oid, objp->type, objp->value, objp->valen);
      /* encode the result */
      outBufLen = sizeof(outBuf);
      objp->rc = xLibTypeEncode (XLIB_IF_WEB, objp->type, objp->value, objp->valen,
            outBuf, &outBufLen, XLIB_TRUE);
      if (objp->rc != XLIBRC_SUCCESS)
      {
        XLIB_WARN ("xLibTypeEncode failed: for 0x%08x: rc = %s\r\n", objp->oid,
        xLibRcStrErr (objp->rc));
       /* clear the value and status in emweb structures */
      }
    }
    if (cellStarts[i] != NULL)
    {
      osapiStrncat(buf,cellStarts[i],(sizeof(wap->ui.web.buf2048)- strlen(buf)-1));
    }
    osapiStrncat(buf,outBuf,(sizeof(wap->ui.web.buf2048)- strlen(buf)-1));

    if (cellClose != NULL)
    {
      osapiStrncat(buf,cellClose,(sizeof(wap->ui.web.buf2048)- strlen(buf)-1));
    }
  }
  if (cellClose != NULL)
  {
    osapiStrncat(buf,rowClose,(sizeof(wap->ui.web.buf2048)- strlen(buf)-1));
  }
  /* strcpy(wap->ui.web.buf2048,buf); */
  xLibWeb2ReqFinish (wap, "xLibWeb2GetRow end");
  return wap->ui.web.buf2048;
}

xLibBool_t xLibWeb2CheckUserAllowAccess (void *srvCntx, int which, int level)
{
  int usrLevel;

  usrLevel = emwebUserAccessGet (srvCntx);

  switch (which)
  {
    case XLIB_WEB2_ACCESS_LESSER:
      return (usrLevel < level) ? XLIB_TRUE: XLIB_FALSE;
    case XLIB_WEB2_ACCESS_EQUAL:
      return (usrLevel == level) ? XLIB_TRUE: XLIB_FALSE;
    case XLIB_WEB2_ACCESS_GREATER:
      return (usrLevel > level) ? XLIB_TRUE: XLIB_FALSE;
    default:
      break;
  }
  return XLIB_FALSE;
}
