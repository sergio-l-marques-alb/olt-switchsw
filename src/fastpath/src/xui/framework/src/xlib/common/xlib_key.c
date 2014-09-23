/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename xlib_key.c
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

static xLibRC_t xLibObjLoadKeyWithCount (xLibWa_t * wap, xLibU16_t * count);

xLibRC_t xLibObjLoadKey (xLibWa_t * wap)
{
  xLibU16_t instCount = 0;
  return xLibObjLoadKeyWithCount (wap, &instCount);
}

xLibU16_t xLibObjCountKey (xLibWa_t * wap, xLibU32_t max, xLibBool_t * more)
{
  xLibU16_t instCount = 0;

  *more = XLIB_FALSE;
  while (XLIBRC_SUCCESS == xLibObjLoadKeyWithCount (wap, &instCount))
  {
    if (instCount > XLIB_MAX_REPEAT_COUNT)
    {
      XLIB_WARN ("Fairly large instance count");
    }
    if (max != 0 && instCount > max)
    {
      *more = XLIB_TRUE;
      instCount = max;
      break;
    }
  }
  XLIB_TRACE ("instCount = %d more = %d", instCount, *more);
  return instCount;
}

static xLibRC_t xLibObjGetMayBeKeyCopy (xLibWa_t * wap, xLibId_t obj, xLibId_t cpobj,
    xLibU8_t * value, xLibU16_t * valen)
{
  xLibU16_t i;
  xLibRC_t rc = XLIBRC_FAILURE;
  xLibU16_t tmpValen = *valen;

  /* search in the keys */
  for (i = 0; i < wap->keyCount; i++)
  {
    if (wap->keys[i].oid == obj)
    {
      /* match with key get the value from filter */
      rc = xLibFilterGet (wap, cpobj, value, valen);
      if (rc == XLIBRC_SUCCESS)
      {
        return rc;
      }
      break;
    }
  }

  /* TODO:: Need to check why the below loop is required */
  for (i = 0; i < wap->objCount; i++)
  {
    if (wap->objs[i].oid == obj)
    {
      if (wap->objs[i].flag & XLIB_IS_KEY)
      {
        /* match with key get the value from filter */
        rc = xLibFilterGet (wap, cpobj, value, valen);
        if (rc == XLIBRC_SUCCESS)
        {
          return rc;
        }
        *valen = tmpValen;
        break;
      }
    }
  }

  return rc;
}

xLibRC_t  xLibSetKeyForPseudoKey(xLibWa_t * wap, xLibObj_t * pseudokeyobjp)
{
  xLibU16_t i;
  xLibU8_t *pseudokey_val = NULL;
  xLibU16_t pseudokey_size = 0;
  xLibObj_t * keyobjp = NULL;

  for (i = 0; i < wap->objCount; i++)
  {
    if (wap->objs[i].oid == pseudokeyobjp->keyoid)
    {
      keyobjp = &wap->objs[i];
      if(keyobjp != NULL && xLibWeb2IsDisable (keyobjp)){
         keyobjp= NULL;
      }
      if(keyobjp != NULL){
         break;
      }
    }

  }


  if(keyobjp == NULL){
    return XLIBRC_FAILURE;
  } 
  if( XLIB_TRUE == xLibWeb2IsDisable (keyobjp))
  {
    return XLIBRC_SUCCESS;
  }

  if (keyobjp->cpoid != 0)
  {
    keyobjp->valen = sizeof (keyobjp->value);
    return xLibObjGetMayBeKeyCopy (wap, keyobjp->oid, keyobjp->cpoid, keyobjp->value, &keyobjp->valen);
  }

  keyobjp->flag = keyobjp->flag | XLIB_IS_KEY_FOR_PSEUDOKEY;
  pseudokey_size = (pseudokeyobjp->type == XLIB_DTYPE_string)?pseudokeyobjp->valen-1:pseudokeyobjp->valen;
  pseudokey_val = (xLibU8_t *)xLibMalloc(pseudokeyobjp->valen);
  if(pseudokey_val == NULL)
  {
    return XLIBRC_FAILURE;
  }
  memcpy( pseudokey_val,pseudokeyobjp->value, pseudokeyobjp->valen);

  /* search in the keys 
     for (i = 0; i < wap->keyCount; i++)
     {*/
  if (keyobjp->flag & XLIB_IS_FRESH)
  {
    xLibFree(pseudokey_val);
    return XLIBRC_FAILURE;

  }
  /*    if (wap->keys[i].oid == keyobjp->oid)
        {*/
  keyobjp->valen = sizeof (keyobjp->value);
  /* clear the filter to get keyobj values from it's first value */
  /*   xLibFilterPush(wap,pseudokeyobjp->oid);
       xLibFilterPush(wap,pseudokeyobjp->keyoid);*/

  xLibFilterClear(wap,keyobjp->oid);
  xLibFilterClear(wap,pseudokeyobjp->oid);
  bzero(keyobjp->value,sizeof(keyobjp->value));
  /* iterate through all the possible values of key */
  while(1){

    keyobjp->rc = xLibObjGet (wap, keyobjp->oid, keyobjp->cpoid, keyobjp->value, &keyobjp->valen);

    if (keyobjp->rc != XLIBRC_SUCCESS && keyobjp->rc != XLIBRC_ENDOF_TABLE)
    {
      /*             xLibFilterPop(wap,keyobjp->oid);*/
      xLibFree(pseudokey_val);
      return keyobjp->rc;
    }
    if(keyobjp->rc == XLIBRC_ENDOF_TABLE){
      break;
    }   
    /* set the key value to filter */
    /*  xLibFilterPop(wap,wap->objs[i].oid);*/
    xLibFilterSet(wap,  keyobjp->oid,  keyobjp->type,  keyobjp->value,  keyobjp->valen);
    pseudokeyobjp->rc = xLibObjGet (wap, pseudokeyobjp->oid, pseudokeyobjp->cpoid, pseudokeyobjp->value, &pseudokeyobjp->valen);

    if (pseudokeyobjp->rc != XLIBRC_SUCCESS)
    {   
      continue; 
    }
    /* set the pseudo key value to filter */
    /*xLibFilterPop(wap,wap->objs[i].oid);*/

    xLibFilterSet(wap,pseudokeyobjp->oid, pseudokeyobjp->type,  pseudokeyobjp->value,  pseudokeyobjp->valen);

    /* matched with pseudo key value */ 
    if (pseudokey_size == pseudokeyobjp->valen)
    {
      if(!memcmp(pseudokey_val, pseudokeyobjp->value,pseudokey_size)){
        /* key value corresponding to pseudokey is in filter , so return from here */
        break;
      } 
    }
  }/* end while */


  /*  for (i = 0; i < wap->objCount; i++)
      {
      if(wap->objs[i].oid == pseudokeyobjp->keyoid || wap->objs[i].oid == pseudokeyobjp->oid){
      xLibFilterPop(wap,wap->objs[i].oid);
      }

      }*/
  /*  xLibFilterPop(wap,pseudokeyobjp->oid);
      xLibFilterPop(wap,pseudokeyobjp->keyoid);*/

  xLibFree(pseudokey_val);
  return XLIBRC_FAILURE;
}


xLibRC_t xLibObjGetMayBeKey (xLibWa_t * wap, xLibObj_t * objp)
{
  xLibU16_t i;
  xLibBool_t isKey = XLIB_FALSE;

  if (objp->cpoid != 0)
  {
    objp->valen = XLIB_MAX_VAL_LEN;
    return xLibObjGetMayBeKeyCopy (wap, objp->oid, objp->cpoid, objp->value, &objp->valen);
  }

  /* search in the keys */
  for (i = 0; i < wap->keyCount; i++)
  {
    if (objp->flag & XLIB_IS_FRESH)
    {
      break;
    }
    if (wap->keys[i].oid == objp->oid)
    {
      /* match with key get the value from filter */
      objp->valen = XLIB_MAX_VAL_LEN;
      objp->rc = xLibFilterGet (wap, objp->oid, objp->value, &objp->valen);
      if (objp->rc == XLIBRC_SUCCESS)
      {
        return objp->rc;
      }
      break;
    }
  }

  /* TODO:: Need to check why the below loop is required */
  for (i = 0; i < wap->objCount; i++)
  {
    if (objp->flag & XLIB_IS_FRESH)
    {
      break;
    }
    if (wap->objs[i].oid == objp->oid)
    {
      if (wap->objs[i].flag & XLIB_IS_KEY)
      {
        /* match with key get the value from filter */
        objp->valen = XLIB_MAX_VAL_LEN;
        objp->rc = xLibFilterGet (wap, objp->oid, objp->value, &objp->valen);
        if (objp->rc == XLIBRC_SUCCESS)
        {
          return objp->rc;
        }
        break;
      }
    }
  }
   /* for non-key, whose value is reserved by  ui object, get the value from filter */
  
  for (i = 0; i < wap->objCount; i++)
  {
    if (objp->flag & XLIB_IS_FRESH)
    {
      break;
    }
    if (wap->objs[i].oid == objp->oid)
    {
      if (wap->objs[i].flag == XLIB_IS_KEY){
          isKey = XLIB_TRUE;
          break;
      }
    }
  }

   if(xLibIsFilterReserved(wap,objp->oid) && !isKey){
    objp->valen = XLIB_MAX_VAL_LEN;
    objp->rc = xLibFilterGet (wap, objp->oid, objp->value, &objp->valen);
    return objp->rc;
  }


  /* not found in keys - get the object value */
  objp->valen = XLIB_MAX_VAL_LEN;
  objp->rc = xLibObjGet (wap, objp->oid, objp->cpoid, objp->value, &objp->valen);
  if (objp->rc != XLIBRC_SUCCESS && objp->rc != XLIBRC_ENDOF_TABLE)
  {
    XLIB_TRACE ("xLibObjGet failed: for 0x%08x: rc = 0x%x", objp->oid, objp->rc);
  }
  return objp->rc;
}

void xLibDumpValue (char *file, int line, xLibU16_t type, xLibU8_t * value, xLibU16_t len)
{
  char encBuf[1024];
  xLibU16_t encLen = sizeof (encBuf);

  xLibTypeEncode (XLIB_IF_WEB, type, value, len, encBuf, &encLen, XLIB_TRUE);
  sysapiPrintf ("[%s:%d] %s", file, line, encBuf);
}

/*This function is used to identify if XLIB_IS_KEY_ITERATE flag is set for a key or not*/
static xLibBool_t xLibIsIterate(xLibWa_t * wap,int iterator)
{
  if(wap->keyCount <= iterator)
    return XLIB_FALSE;
  if(wap->keys[iterator].flags & XLIB_IS_KEY_ITERATE)
  {
    return XLIB_TRUE;
  }
  return XLIB_FALSE;
}
#if 0
/*Compiler is throwing a warning because this function is not used. since this can be useful later,
i have commented it out instead of removing it*/

/*This function will return the next iterator present in the wap*/
static xLibRC_t xLibGetNextIterator(xLibWa_t * wap,int current,int* next)
{
  int i;
  if(next == NULL)
  {
    return XLIBRC_FAILURE;
  }
  for(i = current + 1;i < wap->keyCount;i++)
  {
    if(xLibIsIterate(wap,i) == XLIB_TRUE)
    {
      *next = i;
      return XLIBRC_SUCCESS;
    }
  } 
  return XLIBRC_FAILURE;
}
#endif
/*This function will return the previous iterator present in the wap*/
static xLibRC_t xLibGetPrevIterator(xLibWa_t * wap,int current,int* prev)
{
  int i = 0;

  if(prev == NULL)
    return XLIBRC_FAILURE; 
  for(i = current - 1 ; i >= 0; i--  )
  {
    if(xLibIsIterate(wap,i) == XLIB_TRUE)
    {
      *prev = i;
      return XLIBRC_SUCCESS;
    }
  }
  return XLIBRC_FAILURE;
}

/*This function checks whether the given iterator index is the last iterator in the wap or not*/
static xLibBool_t xLibIsLastIterator(xLibWa_t * wap,int iterator)
{
  int i = 0;
  /*if the iterator is not "iterator" then return false*/
  if( xLibIsIterate(wap,iterator) != XLIB_TRUE)
  {
    return XLIB_FALSE;
  } 
 
  for(i = iterator + 1 ;i < wap->keyCount;i++)
  {
    if(xLibIsIterate(wap,i) == XLIB_TRUE)
    {
      return XLIB_FALSE; 
    }
  }
  return XLIB_TRUE;
}

static xLibRC_t xLibObjLoadKeyWithCount (xLibWa_t * wap, xLibU16_t * count)
{
  xLibU32_t keyExts[XLIB_MAX_KEY];
  xLibBool_t flag = XLIB_FALSE;
  xLibU32_t i, j, k;
  xLibS32_t firstIterator = -1;
  xLibU32_t iteratorCount = 0;
  xLibRC_t rc;

  if(wap->keyCount == 0){
    *count = 0;
    return XLIBRC_FAILURE;
  }

/*keyExts array is used to identify the iterators and keys with cpval.
  Only these keys should be iterated*/

  for (i = 0; i < wap->keyCount; i++)
  {
    if(xLibIsIterate(wap,i) == XLIB_TRUE)
    {
      keyExts[i] = 1;
      iteratorCount++; 
      if(firstIterator == -1)
      {
        firstIterator = i; 
      }  
    }else if(wap->keys[i].cpoid != 0)
    {
      keyExts[i] = 1;
    }
    else
    {
      keyExts[i] = 0;
    }
  }

/*if filter exists for any key, dont iterate through it except for the last iterator.
we should always get the next value for the last iterator*/

  for (i = 0; i < wap->keyCount  ; i++)
  {
    /* check if we have already read the key */
    if(xLibIsLastIterator(wap,i) != XLIB_TRUE)
    { 
      wap->keys[i].valen = XLIB_MAX_VAL_LEN;

      if( wap->keys[i].cpoid != 0){
        rc = xLibFilterGet (wap, wap->keys[i].cpoid, wap->keys[i].value, &wap->keys[i].valen);
        if(rc == XLIBRC_SUCCESS)  
             xLibFilterSet (wap, wap->keys[i].oid, wap->keys[i].type, wap->keys[i].value,
              wap->keys[i].valen);

      }
      else{
      rc = xLibFilterGet (wap, wap->keys[i].oid, wap->keys[i].value, &wap->keys[i].valen);
      }

      if(rc != XLIBRC_SUCCESS)  
      {
        /*if key is not iterate and keyExts is 0, then try to get the value from object for this key*/
        if(keyExts[i] == 0)
        {
          wap->keys[i].valen = XLIB_MAX_VAL_LEN;
          rc = xLibObjGet (wap, wap->keys[i].oid, 0, wap->keys[i].value, &wap->keys[i].valen);
          if(rc != XLIBRC_SUCCESS)
          {
            /*value not in filter and also obj get failed.something wrong*/
            *count = 0;
            return XLIBRC_FAILURE;
          }
          xLibFilterSet (wap, wap->keys[i].oid, wap->keys[i].type, wap->keys[i].value,
              wap->keys[i].valen);

        }
        else
        {
       break;    
      }
      }
      keyExts[i] = 0;
    }
  }

/*

Main logic of this while loop is as follows:
1.if the key has cpval, then get the value from the filter of cpval oid.
2.if any key other than the first iterator returns end of table,go to the previous iterator and get its next value. 
  do this until you reach the first iterator.
3. if first iterator obj get is not successful then return as we are done with all the combinations

*/
  if(iteratorCount == 0)
  {
    *count = 0;
    return XLIBRC_FAILURE;
  }
  while (1)
  {
    for (i = 0; i < wap->keyCount; i++)
    {
      flag = XLIB_FALSE;
      if (keyExts[i])
      {
        for (j = i; j < wap->keyCount; j++)
        {
          wap->keys[j].valen = XLIB_MAX_VAL_LEN;
          if(wap->keys[j].cpoid != 0)
          {
            rc = xLibFilterGet(wap, wap->keys[j].cpoid, wap->keys[j].value, &wap->keys[j].valen);
          }
          else
          {
            /* Key value not set in filter */
            rc = xLibObjGet (wap, wap->keys[j].oid, 0, wap->keys[j].value, &wap->keys[j].valen);
          }
          /* For key other then first */
          if (j != firstIterator)
          {
            if (rc == XLIBRC_ENDOF_TABLE)
            {
              rc = xLibGetPrevIterator(wap,j,&j); 
              if(rc == XLIBRC_SUCCESS)
              {   
                keyExts[j] = 1;
                flag = XLIB_TRUE;
                break;
              }
              else
              {
                /*we should not reach here as this is not the first iterator. so we shd 
                  always get the previous iterator
                */
                return rc;  
              }  
            }
            else if (rc != XLIBRC_SUCCESS)
            {
              XLIB_TRACE ("failed to get key %x", wap->keys[j].oid);
              return rc;
            }

            /* set the current filter */
            xLibFilterSet (wap, wap->keys[j].oid, wap->keys[j].type, wap->keys[j].value,
                wap->keys[j].valen);
            if (XLIB_TRUE == xLibIsLastIterator(wap,j) || (wap->keys[i].cpoid != 0 && j == wap->keyCount - 1))
            {
                *count = *count + 1;
                return rc;
            }
            else
            {
              /* clear next dependent key */
              k = j + 1;
              xLibFilterClear (wap, wap->keys[k].oid);
            }
          }
          /* For first iterator */
          else
          {
            if (rc != XLIBRC_SUCCESS)
            {
              XLIB_TRACE ("failed to get key %x", wap->keys[j].oid);
              return rc;
            }
            xLibFilterSet (wap, wap->keys[j].oid, wap->keys[j].type, wap->keys[j].value,
                wap->keys[j].valen);
            /* If first key is  only key */
            if (XLIB_TRUE == xLibIsLastIterator(wap,j) || (wap->keys[i].cpoid != 0 && j == wap->keyCount - 1))
            {
              *count = *count + 1;
              return rc;
            }
            else
            {
              /* clear next dependent key */
              k = j + 1; 
              xLibFilterClear (wap, wap->keys[k].oid);
            }
          }
        }
      }
      if (flag == XLIB_TRUE)
      {
        break;
      }
    }
  }
  return XLIBRC_SUCCESS;
}

