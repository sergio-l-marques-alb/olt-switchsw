/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename xlib_obj.c
 *
 * @purpose
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
#include <string.h>

#include "xlib_private.h"

/**
 * @brief
 *
 * @param oid
 *
 * @return
 */
static xLibObjNode_t *xLibObjNodeFind (xLibId_t oid)
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

xLibBool_t xLibIsPvtObj (xLibWa_t * wap, xLibId_t oid)
{
  int i = 0;
  for (i = 0; i < wap->objCount; i++)
  {
    if (wap->objs[i].oid == oid)
    {
      if (wap->objs[i].flag & XLIB_IS_PVT)
      {
        return XLIB_TRUE;
      }
    }
  }
  return XLIB_FALSE;
}

/**
 * @brief
 *
 * @param wap
 * @param oid
 * @param buf
 * @param len
 *
 * @return
 */
xLibRC_t xLibObjGet (void * wap, xLibId_t oid, xLibId_t cpoid, xLibU8_t * buf, xLibU16_t * len)
{
  xLibRC_t rv;
  xLibObjNode_t * node;
  xLibBuf_t bd;                 /* TEMP */

  /* in case of private objects return the value from the filter */
  if (XLIB_TRUE == xLibIsPvtObj (wap, oid))
  {
    memset (buf, 0, *len);
    return xLibFilterGet (wap, oid, buf, len);
  }

  /* in case of copy value return the other object value from filter */
  if (cpoid != 0)
  {
    memset (buf, 0, *len);
    return xLibFilterGet (wap, cpoid, buf, len);
  }

  /* find the module */
  node = xLibObjNodeFind (oid);


  if (node == NULL)
  {

    XLIB_TRACE ("Fail to find module for OID 0x%08x\r\n", oid);
    return XLIBRC_UNKNOWN_MOD;
  }

  /* get the value from object handler */
  xLibBufDataInit (wap, &bd, oid);
  rv = node->getFunc (wap, oid, &bd);
  if (rv != XLIBRC_SUCCESS && rv != XLIBRC_ENDOF_TABLE)
  {
    XLIB_TRACE ("Get handler error for 0x%08x 0x%x\r\n", oid, rv);

    return rv;
  }

  if (rv == XLIBRC_SUCCESS)
  {
    if (bd.valid != XLIB_TRUE)
    {

      rv = XLIBRC_BAD_APP;
    }
    else
    {
      memset (buf, 0, *len);
      memcpy (buf, bd.value, bd.valen);
      *len = bd.valen;
    }
  }

  return rv;
}

/**
 * @brief
 *
 * @param wap
 * @param oid
 * @param buf
 * @param len
 *
 * @return
 */
xLibRC_t xLibObjSet (void * wap, xLibId_t oid, xLibU8_t * buf, xLibU16_t len)
{
  xLibRC_t rv;
  xLibObjNode_t * node;
  xLibBuf_t bd;                 /* TEMP */

  /* we don;t need to set any thing for the private objects */
  if (XLIB_TRUE == xLibIsPvtObj (wap, oid))
  {
    return XLIBRC_SUCCESS;
  }

  /* find the module */
  node = xLibObjNodeFind (oid);
  if (node == NULL)
  {
    XLIB_TRACE ("Fail to find module for OID 0x%08x\r\n", oid);
    return XLIBRC_UNKNOWN_MOD;
  }

  xLibBufDataInit (wap, &bd, oid);
  memcpy (bd.value, buf, len);
  bd.valen = len;

  /* set the value from object handler */
  rv = node->setFunc (wap, oid, &bd);
  if (rv != XLIBRC_SUCCESS)
  {
    XLIB_TRACE ("Set handler error for 0x%08x %s\r\n", oid, xLibRcStrErr (rv));

    return rv;
  }

  return rv;
}

char *xLibObjBufPrint (xLibId_t oid, xLibS8_t * name, xLibS8_t * value, xLibS8_t * fmt, xLibS8_t * dst,
                       xLibU16_t size)
{
  char * fmtPtr = (char *) fmt;
  char * ret = NULL;
  char buf[50];
  char * temp = NULL;
  char *lf = "";
  char *cr = "";
  char tempBuf[265];
  char dot[10] = "....";
  memset (tempBuf, 0x0, sizeof (tempBuf));
  ret = dst;
  while (*fmtPtr)
  {
    if (fmtPtr[0] == '\r')
    {
      strcpy (dst, lf);
      dst += strlen (lf);
      fmtPtr += 1;
    }
    else if (fmtPtr[0] == '\n')
    {
      strcpy (dst, cr);
      dst += strlen (cr);
      fmtPtr += 1;
    }
    else if (!strncmp (fmtPtr, "$$objName$$", 11))
    {
      /* name here corresponds to the webName if present will be used instead of object name */
      if(name  == NULL)
      {
      temp = (char *) xLibOEMOidNameGet (oid);
      }
      else
      { 
         temp = (char *) name;
      }
      strcpy (dst, temp);
      dst += strlen (temp);
      fmtPtr += 11;
    }
    else if (!strncmp (fmtPtr, "$$objValue$$", 12))
    {
      if(strlen(value) <= 256)
      {
       strcpy (dst, value);
       dst += strlen (value);
       fmtPtr += 12;
      }
  /* Limiting the error value to 256 characters*/
       else
      {
       strncpy(tempBuf,value,256);
       strcat(tempBuf,dot);
       strcpy (dst, tempBuf);
       dst += strlen (tempBuf);
       fmtPtr += 12;
      }

    }
    else if (!strncmp (fmtPtr, "$$objMin$$", 10))
    {
      memset (buf, 0x0, sizeof (buf));
      xLibObjMinGet (oid, buf);
      strcpy (dst, buf);
      dst += strlen (buf);
      fmtPtr += 10;
    }
    else if (!strncmp (fmtPtr, "$$objMax$$", 10))
    {
      memset (buf, 0x0, sizeof (buf));
      xLibObjMaxGet (oid, buf);
      strcpy (dst, buf);
      dst += strlen (buf);
      fmtPtr += 10;
    }
    else if (!strncmp (fmtPtr, "$$objDefault$$", 14))
    {
      memset (buf, 0x0, sizeof (buf));
      xLibObjDefaultGet (oid, buf);
      strcpy (dst, buf);
      dst += strlen (buf);

      fmtPtr += 14;
    }
    else
    {
      *dst++ = *fmtPtr++;
    }
  }
  *dst = 0;
  dst = ret;
  return ret;
}

/* return XLIB_TRUE if object has next possible value
 *        XLIB_FALSE otherwise
 *
 */
        
xLibBool_t xLibHasNext(void *arg,xLibId_t oid,xLibU16_t type,objGetFunc_t func){
   xLibU8_t tmp[1024];
   xLibU16_t len=0;

   if(xLibGetNextObjVal2(arg,oid,func,type,(xLibU8_t *)tmp,&len,XLIB_FALSE) == XLIBRC_SUCCESS)
      return XLIB_TRUE;

   return XLIB_FALSE;


}

/* 
 * purpose  Get the next possible value for the object
 * return XLIBRC_SUCCESS 
 *        XLIBRC_FAILURE
 *
 */

xLibRC_t  xLibGetNextObjVal2(void *arg,xLibId_t oid, objGetFunc_t func, xLibU16_t type,
                          xLibU8_t *nextVal,xLibU16_t *len,xLibBool_t setFilter){


 xLibWa_t *wap = (xLibWa_t *)arg;
 xLibRC_t rc=XLIBRC_FAILURE;
 xLibU8_t tmp[1024];
 xLibU16_t tmpLen = sizeof(tmp);

  if( xLibIsFilterCheckedout(wap,oid)) return XLIBRC_SUCCESS;

  memset(nextVal,0,*len);
  memset(tmp,0,tmpLen);


 if(func != NULL)  {
      xLibBuf_t bd;
      xLibBufDataInit (wap, &bd, oid);
      rc = func (wap, &bd);
      if (rc == XLIBRC_SUCCESS)
      {
        memcpy (tmp, bd.value, bd.valen);
        tmpLen = bd.valen;

      }
  }
  else{

      rc = xLibObjGet(wap, oid,0,tmp,&tmpLen);
  }

 if (rc != XLIBRC_SUCCESS)
 {
        if (rc != XLIBRC_ENDOF_TABLE && rc != XLIBRC_IGNORE)
        {
          XLIB_TRACE ("xLibObjGet: failed for 0x%08x: rc = \r\n", oid);
        }
        return XLIBRC_FAILURE;
  }


 if(setFilter == XLIB_TRUE)
    xLibFilterSet2(arg,oid,0,tmp,tmpLen);


 rc = xLibTypeEncode (XLIB_IF_WEB, type, tmp,tmpLen,
          nextVal, len, XLIB_TRUE);

 return rc;



}


