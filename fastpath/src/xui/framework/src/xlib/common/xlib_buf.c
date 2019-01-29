/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename xlib_buf.c
 *
 * @purpose Utility functions to manipulate the XLIB buffer
 *
 * @component  XLIB
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
 * @param arg
 *
 * @return
 */
xLibRC_t xLibBufDataInit (void *wap, void *arg, xLibId_t oid)
{
  xLibBuf_t *bufp = (xLibBuf_t *) arg;
  XLIB_ASSERT (arg);
  memset (bufp, 0, sizeof (*bufp));
  bufp->valid = XLIB_FALSE;
  bufp->oid = oid;
  bufp->wap = wap;
  return XLIBRC_SUCCESS;
}

/**
 * @brief Copies data into the buffer context
 *
 * @param arg  buffer context
 * @param data data pointer to copy from
 * @param len  length of the data to copy
 *
 * @return
 */
xLibRC_t xLibBufDataSet (void *arg, const xLibU8_t * data, const xLibU16_t len)
{
  xLibBuf_t *bufp = (xLibBuf_t *) arg;

  XLIB_ASSERT (arg);
  XLIB_ASSERT (data);

  if (len > sizeof (bufp->value))
  {
    XLIB_TRACE ("XLIBRC_CROSS_BOUNDS");
    return XLIBRC_CROSS_BOUNDS;
  }

  memset (bufp->value, 0, sizeof (bufp->value));
  memcpy (bufp->value, data, len);
  bufp->valen = len;
  bufp->valid = XLIB_TRUE;
  return XLIBRC_SUCCESS;
}

/**
 *  * @brief
 *  *
 *  * @param arg
 *  * @purpose : Decode the value and set it into filter.
 *  * @return
 *  */
 

xLibRC_t xLibBufDataSet2 (void *arg, xLibId_t oid,xLibU16_t type,const xLibU8_t * data, const xLibU16_t len)
{
   xLibU8_t tmp[1024];
   xLibU16_t tmpLen = sizeof(tmp);
   xLibRC_t rc;

   /* do not over-write the filter if it is in reserved mode */
   if(xLibIsFilterReserved(arg,oid)){
      return XLIBRC_SUCCESS;
   }

   memset(tmp,0,sizeof(tmp));

   rc = xLibTypeDecode(XLIB_IF_WEB, type, (xLibS8_t *)data, tmp,
                         &tmpLen, XLIB_TRUE);

   if(rc == XLIBRC_SUCCESS)
      rc = xLibFilterSet2(arg, oid, type, tmp, tmpLen);

   if(rc == XLIBRC_SUCCESS)
      rc = xLibFilterReserve(arg,oid);
  

  return rc;
}

/**
 * @brief Copies data from buffer context to caller memory
 *
 * @param arg      buffer context
 * @param data     data pointer to copy to
 * @param sizeLen  size of supplied buffer/ length of data copied
 *
 * @return
 */
xLibRC_t xLibBufDataGet (void *arg, xLibU8_t * data, xLibU16_t * sizeLen)
{
  xLibRC_t rc = XLIBRC_SUCCESS;
  xLibBuf_t *bufp = (xLibBuf_t *) arg;

  XLIB_ASSERT (arg);
  XLIB_ASSERT (data);
  XLIB_ASSERT (sizeLen);

  if (bufp->valen > *sizeLen)
  {
    XLIB_TRACE ("XLIBRC_TRUNCATED %d %d", bufp->valen, *sizeLen);
    return XLIBRC_TRUNCATED;
  }
  *sizeLen = bufp->valen;
  memcpy (data, bufp->value, *sizeLen);
  return rc;
}

/**
 * @brief gives back pointer to data in buffer context to caller memory
 *
 * @param arg    buffer context
 * @param data   pointer to give back the data location in buffer context
 * @param len    length of data available
 *
 * @return
 */
xLibRC_t xLibBufDataPtrGet (void *arg, xLibU8_t * *data, xLibU16_t * len)
{
  xLibBuf_t *bufp = (xLibBuf_t *) arg;

  XLIB_ASSERT (arg);
  XLIB_ASSERT (data);
  XLIB_ASSERT (len);

  *len = bufp->valen;
  *data = bufp->value;
  return XLIBRC_SUCCESS;
}

xLibRC_t xLibBufDataFileSet (void *arg, char *name)
{
  xLibBuf_t *bufp = (xLibBuf_t *) arg;
  xLibWa_t *wap = (xLibWa_t *) bufp->wap;
  if (wap->fileNameSet == NULL)
  {
    return XLIBRC_FAILURE;
  }
  return wap->fileNameSet (wap, name);
}

xLibU16_t xLibBufFileDataGet (void *arg, xLibU8_t ** data, void **repeat)
{
  return XLIBRC_NOT_SUPPORTED;
}

xLibRC_t xLibBufFileDataSet (void *arg, char *name, xLibU8_t * data, xLibU16_t * sizeLen)
{
  return XLIBRC_NOT_SUPPORTED;
}

