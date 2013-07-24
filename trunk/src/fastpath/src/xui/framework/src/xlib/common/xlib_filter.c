/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename xlib_filter.c
*
* @purpose Utility functions to read and manipulate the filter info
*           Object handlers calls only xLibFilterGet function 
*           XLIB interface specific (CLI/WEB/SNMP) can set the filters
*           to be used by the object handlers by calling xLibFilterSet
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

static xLibFilter_t *xLibFilterNew (xLibFilter_t ** head)
{
  xLibFilter_t *start;
  xLibFilter_t *fp;

  fp = (xLibFilter_t *) xLibMalloc (sizeof (*fp));
  XLIB_ASSERT (fp);
  fp->next = NULL;

  if (*head == NULL)
  {
    *head = fp;
  }
  else
  {
    for (start = *head; start->next; start = start->next);
    start->next = fp;
  }
  return fp;
}

/** 
* @brief Stores filter data into work area
* 
* @param arg  Work Area
* @param oid  filter id
* @param type  filter type
* @param val  filter data to store
* @param len  length of filter data
* 
* @return 
*/
xLibRC_t xLibFilterSet (void *arg, xLibId_t oid, xLibU16_t type, xLibU8_t * val, xLibU16_t len)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  xLibFilter_t *fp;
  xLibU32_t i = 0;
  xLibBool_t flag = XLIB_FALSE;

  /* when filters are set from object handlers there is no type information passed, hence get the type info from wap */
  if (type == 0)
  {
    for (i = 0; i < wap->keyCount; i++)
    {
      if (wap->keys[i].oid == oid)
      {
        type = wap->keys[i].type;
        flag = XLIB_TRUE;
        break;
      }
    }
    if (flag == XLIB_FALSE)
    {
      for (i = 0; i < wap->objCount; i++)
      {
        if (wap->objs[i].oid == oid)
        {
          type = wap->objs[i].type;
          flag = XLIB_TRUE;
          break;
        }
      }
    }
    if (flag == XLIB_FALSE)
    {
      return XLIBRC_FAILURE;
    }
  }

  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    /* we can override the filter only if depth is zero */
    if (fp->oid == oid && fp->depth == 0)
    {
      break;
    }
  }

  /* use exiting or create filter space */
  fp = fp ? fp : xLibFilterNew ((xLibFilter_t **) wap->filters);
  if (fp == NULL)
  {
    XLIB_TRACE ("xLibMalloc failed");
    return XLIBRC_NO_MEMORY;
  }

  fp->oid = oid;
  fp->type = type;
  fp->valen = len;
  fp->depth=0;

  XLIB_ASSERT (len <= sizeof (fp->value));
  memcpy (fp->value, val, len);
  XLIB_TRACE ("xLibFilterSet: oid = 0x%08x type = %d len = %d", fp->oid, fp->type, fp->valen);
  return XLIBRC_SUCCESS;
}

/**
 *  @brief Stores filter data into work area
 *   will not search for oid in objects or keys in wap
 *
 *  @param arg  Work Area
 *  @param oid  filter id
 *  @param type  filter type
 *  @param val  filter data to store
 *  @param len  length of filter data
 * 
 *  @return
 */

xLibRC_t xLibFilterSet2 (void *arg, xLibId_t oid, xLibU16_t type, xLibU8_t * val, xLibU16_t len)
{
   xLibWa_t *wap = (xLibWa_t *) arg;
   xLibFilter_t *fp;


   for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    /* we can override the filter only if depth is zero */
    if (fp->oid == oid && fp->depth == 0)
    {
      break;
    }
  }

  /* use exiting or create filter space */
  fp = fp ? fp : xLibFilterNew ((xLibFilter_t **) wap->filters);

  if (fp == NULL)
  {
    XLIB_TRACE ("xLibMalloc failed");
    return XLIBRC_NO_MEMORY;
  }

  fp->oid = oid;
  fp->type = type;
  fp->valen = len;
  fp->depth=0;
  XLIB_ASSERT (len <= sizeof (fp->value));
  memcpy (fp->value, val, len);
  XLIB_TRACE ("xLibFilterSet2: oid = 0x%08x type = %d len = %d", fp->oid, fp->type, fp->valen);
  return XLIBRC_SUCCESS;
}


/** 
* @brief 
* 
* @param arg 
* @param oid 
* 
* @return 
*/
xLibRC_t xLibFilterClear (void *arg, xLibId_t oid)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  xLibFilter_t *fp, *prev;

  XLIB_TRACE ("xLibFilterClear: oid = 0x%08x", oid);

  for (fp = *wap->filters, prev = NULL; fp != NULL; prev = fp, fp = fp->next)
  {
    if (fp->oid == oid && fp->depth == 0)
    {
      break;
    }
  }

  if (fp != NULL)
  {
    if (prev == NULL)
    {
      *wap->filters = fp->next;
    }
    else
    {
      prev->next = fp->next;
    }
    xLibFree (fp);
  }
  return XLIBRC_SUCCESS;
}

/** 
* @brief This functions finds the filter and increases the depth
*         so that the filter is not available for object handlers
*         This is mainly used for temporarly hiding the filter and
*         getting it back later
* 
* @param arg 
* @param oid 
* 
* @return 
*/
xLibRC_t xLibFilterPush (void *arg, xLibId_t oid)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  xLibFilter_t *fp, *prev;

  XLIB_TRACE ("xLibFilterPush: oid = 0x%08x", oid);

  for (fp = *wap->filters, prev = NULL; fp != NULL; prev = fp, fp = fp->next)
  {
    if (fp->oid == oid && fp->depth == 0)
    {
      fp->depth++;
    }
  }
  return XLIBRC_SUCCESS;
}
/**
* @brief This functions finds the filter and increases the depth only when falg is not
          XLIB_FILTER_CHECKED_OUT || XLIB_FILTER_CHECKED_IN when filter is available 
*         so that the filter is not available for object handlers
*         This is mainly used for temporarly hiding the filter and
*         getting it back later
*
* @param arg
* @param oid
*
* @return
*/
xLibRC_t xLibFilterCheckOutPush (void *arg, xLibId_t oid)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  xLibFilter_t *fp, *prev;

  XLIB_TRACE ("xLibFilterPush: oid = 0x%08x", oid);

  for (fp = *wap->filters, prev = NULL; fp != NULL; prev = fp, fp = fp->next)
  {
    if (fp->oid == oid && fp->depth == 0  && (fp->flag != XLIB_FILTER_CHECKED_OUT && fp->flag != XLIB_FILTER_CHECKED_IN)  )
    {
      fp->depth++;
    }
  }
  return XLIBRC_SUCCESS;
}

/**
* @brief This functions finds the filter with depth zero and removed it
*         it also tries to find if there is a filter with non zero depth
*         and decreases it depth so that it can be available for object
*         handlers
*
* @param arg
* @param oid
*
* @return
*/
xLibRC_t xLibFilterPop (void *arg, xLibId_t oid)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  xLibFilter_t *fp, *prev;

  /* clear the filter at depth zero */
  xLibFilterClear (arg, oid);

  for (fp = *wap->filters, prev = NULL; fp != NULL; prev = fp, fp = fp->next)
  {
    if (fp->oid == oid && fp->depth != 0)
    {
      fp->depth--;
    }
  }
  return XLIBRC_SUCCESS;
}
xLibRC_t xLibFilterPop2 (void *arg, xLibId_t oid)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  xLibFilter_t *fp, *prev;
 /* clear the filters with depth zero  and oid matching and flag set to other than XLIB_FILTER_CHECKED_OUT, XLIB_FILTER_CHECKED_IN*/
  XLIB_TRACE ("xLibFilterClear: oid = 0x%08x", oid);

  for (fp = *wap->filters, prev = NULL; fp != NULL; prev = fp, fp = fp->next)
  {
    if (fp->oid == oid && fp->depth == 0  && (fp->flag != XLIB_FILTER_CHECKED_OUT && fp->flag != XLIB_FILTER_CHECKED_IN))
    {
      break;
    }
  }

  if (fp != NULL)
  {
    if (prev == NULL)
    {
      *wap->filters = fp->next;
    }
    else
    {
      prev->next = fp->next;
    }
    xLibFree (fp);
  }
  
 /* decrement the depth for filters with matching oid  and flag set to other than XLIB_FILTER_CHECKED_OUT, XLIB_FILTER_CHECKED_IN */
  for (fp = *wap->filters, prev = NULL; fp != NULL; prev = fp, fp = fp->next)
  {
    if (fp->oid == oid && fp->depth != 0 && (fp->flag != XLIB_FILTER_CHECKED_OUT && fp->flag != XLIB_FILTER_CHECKED_IN) )
    {
      fp->depth--;
    }
  }
  return XLIBRC_SUCCESS;

}

xLibRC_t xLibFilterPushFromKeys (void *arg)
{
  int i;
  xLibWa_t *wap = (xLibWa_t *) arg;

  for (i = 0; i < wap->keyCount; i++)
  {
    if(wap->keys[i].flags & XLIB_IS_KEY_ITERATE)
    {
       xLibFilterPush (wap, wap->keys[i].oid);
    }
  }
  return XLIBRC_SUCCESS;
}

xLibRC_t xLibFilterPopFromKeys (void *arg)
{
  int i;
  xLibWa_t *wap = (xLibWa_t *) arg;

  for (i = 0; i < wap->keyCount; i++)
  {
    if(wap->keys[i].flags & XLIB_IS_KEY_ITERATE)
    {
       xLibFilterPop (wap, wap->keys[i].oid);
    }
  }
  return XLIBRC_SUCCESS;
}

/** 
* @brief 
* 
* @param arg 
* 
* @return 
*/
xLibRC_t xLibFilterRemoveAll (void **arg)
{
  xLibFilter_t *fp, *next;

  if(arg == NULL)
  {
      return XLIBRC_SUCCESS;
  }
		
  for (fp = *arg; fp != NULL;)
  {
    next = fp->next;
    fp->next = NULL;
    XLIB_TRACE ("xLibFilterRemoveAll: clearing filter of 0x%08x len %d", fp->oid, fp->valen);
    xLibFree (fp);
    fp = next;
  }

  *arg = NULL;
  return XLIBRC_SUCCESS;
}

/** 
* @brief 
* 
* @param arg 
* 
* @return 
*/
xLibRC_t xLibFilterClearAll (void *arg)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  return xLibFilterRemoveAll (wap->filters);
}

/** 
* @brief Retrives filter data by copy to caller memory
* 
* @param arg      work area
* @param oid      filter object id
* @param val      caller memory to copy the filter data
* @param sizeLen  size of supplied buffer/ length of data copied
* 
* @return 
*/
xLibRC_t xLibFilterGet (void *arg, xLibId_t oid, xLibU8_t * val, xLibU16_t * sizeLen)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  xLibFilter_t *fp;

  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    /* Always use from depth zero */
    if (fp->oid == oid && fp->depth == 0)
    {
      if (*sizeLen < fp->valen)
      {
        XLIB_WARN ("Insufficient memory (%d): to return filter value for oid = 0x%08x len = %d",
                   *sizeLen, fp->oid, fp->valen);
        return XLIBRC_CROSS_BOUNDS;
      }
      memcpy (val, fp->value, fp->valen);
      if(*sizeLen != fp->valen)
      {
        fp->value[fp->valen] = 0;
        *sizeLen = fp->valen;
      }
      XLIB_TRACE ("xLibFilterGet: oid = 0x%08x len = %d", fp->oid, fp->valen);
      return XLIBRC_SUCCESS;
    }
  }

  *sizeLen = -1;
  return XLIBRC_NO_FILTER;
}

/** 
* @brief Retrives filter data by reference
* 
* @param arg  work area
* @param oid  filter object id
* @param val  pointer to retrive filter data address
* @param len  pointer to retrive filter data lenegth
* 
* @return 
*/
xLibRC_t xLibFilterPtrGet (void *arg, xLibId_t oid, xLibU8_t ** val, xLibU16_t * len)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  xLibFilter_t *fp;

  *len = -1;
  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    /* Always use from depth zero */
    if (fp->oid == oid && fp->depth == 0)
    {
      *len = fp->valen;
      *val = fp->value;
      return XLIBRC_SUCCESS;
    }
  }

  return XLIBRC_NO_FILTER;
}

/**
* @brief Retrives caller association to filter context
*
* @param arg      filter context
* @param oid      object id
* @param context  caller suppiled association value
*
* @return
*/
xLibRC_t xLibFilterContextGet (void *arg, xLibId_t oid, void **context)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  xLibFilter_t *fp;

  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    /* Always use from depth zero */
    if (fp->oid == oid && fp->depth == 0)
    {
      *context = fp->context;
      return XLIBRC_SUCCESS;
    }
  }

  return XLIBRC_NO_FILTER;
}

/** 
* @brief  Stores caller association to filter context 
* 
* @param arg      filter context
* @param oid      object id
* @param context  caller suppiled association value
* 
* @return 
*/
xLibRC_t xLibFilterContextSet (void *arg, xLibId_t oid, void *context)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  xLibFilter_t *fp;

  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    /* Always use from depth zero */
    if (fp->oid == oid && fp->depth == 0)
    {
      fp->context = context;
      return XLIBRC_SUCCESS;
    }
  }

  return XLIBRC_NO_FILTER;
}

char *xLibFilterDumpBuffer (void *arg, int intf)
{
  xLibFilter_t *fp;
  xLibWa_t *wap = (xLibWa_t *) arg;
  static char dumpBuffer[10 * 1024];
  int dumpOffset = 0;
  int i;

  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    for (i = 0; i < wap->keyCount; i++)
    {
      if (wap->keys[i].oid == fp->oid && fp->depth == 0)
      {
        wap->encLen = sizeof (wap->encBuf);
        xLibTypeEncode (intf, fp->type, fp->value, fp->valen, wap->encBuf, &wap->encLen, XLIB_TRUE);
        sprintf (&dumpBuffer[dumpOffset], "0x%08x %d %d %s ", fp->oid, fp->type, wap->encLen,
                 wap->encBuf);
        dumpOffset = strlen (dumpBuffer);
        break;
      }
    }
  }
  dumpBuffer[dumpOffset] = 0;
  return dumpBuffer;
}

/**
 * * @brief  Reserves the filter value 
 * *
 * * @param arg      filter context
 * * @param oid      object id
 * *
 * * @return
 * */

xLibRC_t xLibFilterCheckOut(void *arg,xLibId_t oid){
  xLibWa_t *wap = (xLibWa_t *)arg;
  xLibFilter_t *fp=NULL ;

  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
     if (fp->oid == oid &&  fp->depth == 0 )
     {

          break;
    }
  }
  fp =  (fp != NULL)? fp: xLibFilterNew((xLibFilter_t **) wap->filters);
  fp->oid=oid;
  fp->flag=XLIB_FILTER_CHECKED_OUT;

  return XLIBRC_SUCCESS;

}

/**
 *        @brief  checks whether reserve flag is set to filter 
 *        @param arg      filter context
 *        @param oid      object id
 *        @return
 *        * */

xLibBool_t xLibIsFilterCheckedout(void *arg,xLibId_t oid){

   xLibWa_t *wap = (xLibWa_t *)arg;
   xLibFilter_t *fp ;

   for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    /* we can override the filter only if depth is zero */
    if (fp->oid == oid && fp->flag == XLIB_FILTER_CHECKED_OUT && fp->depth == 0)
    {
       return XLIB_TRUE;
    }
  }

  return XLIB_FALSE;

}
/**
 *        @brief  checks whether check in flag is set to filter
 *        @param arg      filter context
 *        @param oid      object id
 *        @return
 *        * */

xLibBool_t xLibIsFilterCheckedin(void *arg,xLibId_t oid){

   xLibWa_t *wap = (xLibWa_t *)arg;
   xLibFilter_t *fp ;

   for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    /* we can override the filter only if depth is zero */
    if (fp->oid == oid && fp->flag == XLIB_FILTER_CHECKED_IN && fp->depth == 0)
    {
       return XLIB_TRUE;
    }
  }

  return XLIB_FALSE;

}


/**
 *      @brief  releases the reserve flag to make it vailable for over-riding
 *      @param arg      filter context
 *      @param oid      object id
 *      @return
 *        * */

xLibRC_t xLibFilterCheckIn(void *arg,xLibId_t oid){

   xLibWa_t *wap = (xLibWa_t *)arg;
   xLibFilter_t *fp ;

   for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    /* we can override the filter only if depth is zero */
    if (fp->oid == oid && fp->depth == 0)
    {
       fp->flag = XLIB_FILTER_CHECKED_IN;
    }
  }

  return XLIBRC_SUCCESS;

}

/**
 *  * @brief  Reserves the filter value
 *  *
 *  * @param arg      filter context
 *  * @param oid      object id
 *  *
 *  * @return
 *  */

xLibRC_t xLibFilterReserve(void *arg,xLibId_t oid){
  xLibWa_t *wap = (xLibWa_t *)arg;
  xLibFilter_t *fp=NULL ;
  xLibFilter_t *newFp= NULL;
  xLibFilter_t *oldFp= NULL;  


  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    if (fp->oid == oid && fp->flag == XLIB_FILTER_RESERVED)
    {                    
        newFp = fp;
        
    }
    else if(fp->oid == oid && fp->depth == 0){
       oldFp = fp;
    }
  }
  if(oldFp == NULL){
     return XLIBRC_SUCCESS;
  }

  fp =  (newFp != NULL)? newFp: xLibFilterNew((xLibFilter_t **) wap->filters);
  fp->oid=oid;
  fp->flag=XLIB_FILTER_RESERVED;
  
  fp->valen = oldFp->valen;
  fp->depth = 0;

  memcpy (fp->value, oldFp->value, oldFp->valen);
  XLIB_TRACE ("xLibFilterSet: oid = 0x%08x type = %d len = %d", fp->oid, fp->type, fp->valen);
  return XLIBRC_SUCCESS;


}

/**
 ** @brief  checks whether the filter is reserved or not
 **
 ** @param arg      filter context
 ** @param oid      object id
 **
 ** @return
 **/

xLibBool_t xLibIsFilterReserved(void *arg,xLibId_t oid){
  xLibWa_t *wap = (xLibWa_t *)arg;
  xLibFilter_t *fp=NULL ;

  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    if (fp->oid == oid && fp->flag == XLIB_FILTER_RESERVED)
    {
         return XLIB_TRUE;
    }
  }

  return XLIB_FALSE;

}


/**
 *        @brief cache a value into filter 
 *        @param arg      filter context
 *        @param oid      object id
 *        @return
 * */

xLibRC_t xLibFilterCache(void *arg,xLibId_t oid,xLibU16_t type,xLibU8_t *val,xLibU16_t sizeLen,xLibBool_t noFilter){


 xLibRC_t rc = XLIBRC_FAILURE;
 xLibU8_t tmp[1024];

 if(noFilter){
    xLibFilterPush(arg,oid);
    return XLIBRC_SUCCESS;
 }

   rc = xLibFilterCheckOutPush (arg,oid);

/* store the value as a filter also, so that other obj handler can use if needed */

   rc =   xLibTypeDecode (XLIB_IF_WEB, type, val, tmp,
                         &sizeLen, XLIB_TRUE);

   if(rc != XLIBRC_SUCCESS) return rc;

   rc = xLibFilterSet2 (arg, oid, 0, tmp,sizeLen);

   if(rc != XLIBRC_SUCCESS){
       XLIB_ERROR ("xLibCacheVal : xLibFilterSet2() failed for 0x%08x: rc = %s\r\n", oid,
              rc);
       return rc;
   }



return XLIBRC_SUCCESS;

}
/**
 ** @brief  Clear reserved/unreserved filters for uiobjects
 **
 ** @return
 **/

void  xLibClearReserveUnreserveFilters(void *arg){

  xLibWa_t *wap = (xLibWa_t *)arg;
  xLibFilter_t *fp=NULL ;
  xLibFilter_t *prev;

  for (fp = *wap->filters, prev = NULL; fp != NULL; prev = fp, fp = fp->next)
  {
    if (fp->flag == XLIB_FILTER_UNRESERVED ||  fp->flag == XLIB_FILTER_RESERVED)
    {
        break;
    }
  }


  if (fp != NULL)
  {
    if (prev == NULL)
    {
      *wap->filters = fp->next;
    }
    else
    {
      prev->next = fp->next;
    }
    xLibFree (fp);
  }
}


void xLibClearFiltersInvoke(void *arg){

   xLibWa_t *wap = (xLibWa_t *)arg;
   xLibFilter_t *fp=NULL ;

   for (fp = *wap->filters; fp != NULL; fp = fp->next)
   {
     if (fp->flag == XLIB_FILTER_UNRESERVED ||  fp->flag == XLIB_FILTER_RESERVED){
         xLibClearReserveUnreserveFilters(wap);
         fp = *wap->filters;
     }
   }
}

/**
 ** @brief  Reserve the filters that are in unreserved mode
 **
 ** @param arg      filter context
 ** @param oid      object id
 **
 ** @return
 **/

void  xLibReserveUnreservedFilters(void *arg){

  xLibWa_t *wap = (xLibWa_t *)arg;
  xLibFilter_t *fp=NULL ;

  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    if (fp->flag == XLIB_FILTER_UNRESERVED)
    {
        fp->flag = XLIB_FILTER_RESERVED;         
    }
  }



}

/**
 ** @brief  Unreserve the filters that are in reserved mode
 **
 ** @param arg      filter context
 ** @param oid      object id
 **
 ** @return
 **/

void  xLibUnreserveFilters(void *arg){

  xLibWa_t *wap = (xLibWa_t *)arg;
  xLibFilter_t *fp=NULL ;

  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    if (fp->flag == XLIB_FILTER_RESERVED)
    {
        fp->flag = XLIB_FILTER_UNRESERVED;
    }
  }

}
/**
 ** @brief  set the flag back to zero for the filters that are in reserved mode
 **
 ** @param arg     filter context
 ** @param oid      object id
 **
 ** @return
 **/

void  xLibUndoReserveFilters(void *arg){

  xLibWa_t *wap = (xLibWa_t *)arg;
  xLibFilter_t *fp=NULL ;

  for (fp = *wap->filters; fp != NULL; fp = fp->next)
  {
    if (fp->flag == XLIB_FILTER_RESERVED)
    {
        fp->flag = 0;
    }
  }

}

