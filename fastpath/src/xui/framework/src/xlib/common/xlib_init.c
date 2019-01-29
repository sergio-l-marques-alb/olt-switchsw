/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename xlib_init.c
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
#include "xlib_protect.h"

/* 
 * 0 - Errors
 * 1 - Errors and Warnings
 * 2 - Errors, Warnings, Trace
 */
int xLibTraceLevel = 0;

int xLibTraceCurrentLine = 0;
char *xLibTraceCurrentFile = NULL;

static void (*xLibAppInitFunc) (void) = NULL;
static void (*xLibFileHandlerFunc) (void) = NULL;

void xLibAppInitRegister (void (*func) (void))
{
  xLibAppInitFunc = func;
}

void xLibInit (void)
{
  static int done = 0;
  if (done == 0)
  {
    xLibTypeInfoInit ();
	  xLibEnumInfoInit(); 
    xLibAppInit ();
    if (xLibAppInitFunc != NULL)
    {
      xLibAppInitFunc ();
    }
    done = 1;
  }
}

void xLibFileHandlerRegister(void *func)
{
  xLibFileHandlerFunc = func;
}

void *xLibFileHandlerGet(void)
{
  return xLibFileHandlerFunc;
}

void xLibWaInit (xLibWa_t * wap)
{
  memset (wap, 0, sizeof (*wap));
  wap->fileHandler = xLibFileHandlerFunc;
}

void *xLibWaCreate(void)
{
  xLibWa_t *wap = (xLibWa_t *)xLibMalloc(sizeof(xLibWa_t));

  if(wap != NULL)
  {
    xLibWaInit(wap);
    XLIB_TRACE("xlib work area created\r\n");
  }
  else
  {
    XLIB_TRACE("xlib work area not created\r\n");
  }

  return wap;
}

void xLibWaDelete(void *wap)
{
  XLIB_TRACE("xlib work area deleted\r\n");
  xLibFilterClearAll(wap);
  return  xLibFree(wap);
}

void xLibWaAddKey (xLibWa_t * wap, xLibId_t oid, xLibU16_t type, xLibId_t cpoid, xLibU32_t flags)
{
  int i;
  if(wap->keyCount < XLIB_MAX_KEY)
  {
     for(i=0; i<wap->keyCount; i++)
     {
        if(wap->keys[i].oid == oid)
        {
           return;
        }
    }

    wap->keys[wap->keyCount].oid = oid;
    wap->keys[wap->keyCount].type = type;
    wap->keys[wap->keyCount].cpoid = cpoid;
    wap->keys[wap->keyCount].flags = flags;
    memset(wap->keys[wap->keyCount].value,0,sizeof(wap->keys[wap->keyCount].value));
    wap->keys[wap->keyCount].valen = 0;
    wap->keyCount++;
  }
  else
  {
     XLIB_ERROR ("Error! xLibWaAddKey : Failed to add Key 0x%08x. Maximum number of keys are already added.\r\n",oid);
     return;
  }
}

xLibObj_t *xLibWaAddObj (xLibWa_t * wap, xLibId_t oid, xLibId_t cpoid, xLibU16_t type, xLibU16_t flag)
{
  xLibObj_t *objp; 
  if(wap->objCount < XLIB_MAX_OBJ)
  {
    objp = &wap->objs[wap->objCount++];  
    memset(objp,0,sizeof(*objp));
    objp->oid = oid;
    objp->cpoid = cpoid;
    objp->type = type;
    objp->flag = flag;
    objp->mCount = 0;
    objp->depCount = 0;
    return objp;
  }
  else
  {
     XLIB_ERROR ("Error! xLibWaAddObj : Failed to add object 0x%08x. Maximum number of objects are already added.\r\n",oid);
     return NULL;
  }

}

xLibObj_t *xLibWaAddObjWithKey (xLibWa_t * wap, xLibId_t oid,xLibId_t cpoid,  xLibId_t keyoid , xLibU16_t type, xLibU16_t flag)
{
  xLibObj_t *objp;
  if(wap->objCount < XLIB_MAX_OBJ)
  {
      objp = &wap->objs[wap->objCount++];
      memset(objp,0,sizeof(*objp));
      objp->oid = oid;
      objp->cpoid = cpoid;
      objp->keyoid= keyoid;
      objp->type = type;
      objp->flag = flag;
      objp->mCount = 0;
      objp->depCount = 0;
      return objp;
  }
  else
  {
     XLIB_ERROR ("Error! xLibWaAddObjWithKey : Failed to add object 0x%08x. Maximum number of objects are already added.\r\n",oid);
     return NULL;
  }


}

void xLibWaErrorMap (xLibWa_t * wap, xLibId_t oid, xLibU32_t code, xLibU32_t mCode)
{
  xLibU32_t i = 0, mCount;
  for (i = 0; i < wap->objCount; i++)
  {
    if (wap->objs[i].oid == oid)
    {
      mCount = wap->objs[i].mCount++;
      wap->objs[i].code[mCount] = code;
      wap->objs[i].mCode[mCount] = mCode;
    }
  }
}

void xLibWaObjUIEleValSet(xLibWa_t *wap, xLibU32_t objID,xLibU32_t uiWebID,xLibU32_t uiCliID,xLibU32_t eleID)
{
  xLibU32_t count =  wap->objCount;
  count--;
  wap->objs[count].objValID = objID;
  wap->objs[count].ui.cli.cliValID = uiCliID; 
  wap->objs[count].ui.web.eleValID = eleID;
  wap->objs[count].ui.web.webValID = uiWebID; 
}

void xLibWaRowStatusDepListSet(xLibWa_t *wap, xLibId_t oid,  xLibId_t depOid)
{
  xLibU32_t depCount;
  xLibU32_t count = wap->objCount;
  count--;
  if(wap->objs[count].oid == oid)
  {
    depCount = wap->objs[count].depCount;
    wap->objs[count].depList[depCount]  = depOid;
    wap->objs[count].depCount++;
  }
}

void xLibAllocWorkAreaMemory (xLibWa_t * wap)
{
#if 0
  int i;

  /* Allocate the memory to hold values */
  for (i = 0; i < wap->objCount; i++)
  {
    if(wap->objs[i].value != NULL)
    {
      printf("xLibAllocWorkAreaMemory: not freed obj mem 0x%lx\r\n", wap->objs[i].value);
    }
    else
    {
      wap->objs[i].value = xLibMalloc (XLIB_MAX_VAL_LEN);
      wap->objs[i].valen = XLIB_MAX_VAL_LEN;
      printf("xLibAllocWorkAreaMemory: obj mem 0x%lx\r\n", wap->objs[i].value);
    }
  }

  /* Allocate the memory to hold keys */
  for (i = 0; i < wap->keyCount; i++)
  {
    if(wap->keys[i].value != NULL)
    {
      printf("xLibAllocWorkAreaMemory: not freed key mem 0x%lx\r\n", wap->keys[i].value);
    }
    else
    {
      wap->keys[i].value = xLibMalloc (XLIB_MAX_VAL_LEN);
      wap->keys[i].valen = XLIB_MAX_VAL_LEN;
      printf("xLibAllocWorkAreaMemory: key mem 0x%lx\r\n", wap->keys[i].value);
    }
  }
#endif
}

void xLibFreeWorkAreaMemory (xLibWa_t * wap, int freeKey)
{
#if 0
  int i;

  /* free the values memory */
  for (i = 0; i < wap->objCount && freeKey == 0; i++)
  {
    printf("xLibFreeWorkAreaMemory: freed obj mem 0x%lx\r\n", wap->objs[i].value);
    xLibFree (wap->objs[i].value);
    wap->objs[i].value = NULL;
    wap->objs[i].valen = 0;
  }

  /* free the keys memory */
  for (i = 0; i < wap->keyCount && freeKey != 0; i++)
  {
    printf("xLibFreeWorkAreaMemory: freed key mem 0x%lx\r\n", wap->keys[i].value);
    xLibFree (wap->keys[i].value);
    wap->keys[i].value = NULL;
    wap->keys[i].valen = 0;
  }
#endif
}

void *xLibGetServerContext (void *arg)
{
  xLibWa_t *wap = (xLibWa_t *) arg;
  return wap->ui.web.webCntxt;
}

xLibRC_t xLibDownLoadUrlSet(void *arg, char *name)
{
   emwebDownloadUrlSet(xLibGetServerContext(arg), name);
   return XLIBRC_SUCCESS;
}


