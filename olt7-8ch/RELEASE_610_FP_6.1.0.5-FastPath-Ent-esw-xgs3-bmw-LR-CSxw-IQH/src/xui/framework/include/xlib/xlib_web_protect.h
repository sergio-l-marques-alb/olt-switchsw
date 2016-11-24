/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename xlib_web_protect.h
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

#ifndef XLIB_WEB_PROTECT_H
#define XLIB_WEB_PROTECT_H

#include <stdlib.h>
#include <string.h>

#include "xlib_protect.h"
#include "ew_proto.h"
#include "fastpath_enum.h"

#include "_xeobj_xtype.h"
#include "_xe_obj.h"

#define XE_RET_SELECT_STR(func)\
  extern char *func(void *);\
  return func(ewsContext);

char *xLibWeb2FinishForm (void *servContext);
void xLibWeb2LoadError (void *cntx, xLibS8_t ** eval, xLibU8_t * estat,
                        xLibU32_t * fval, xLibU8_t * fstat);
void xLibWeb2RecordError (void *cntx, xLibId_t oid, xLibRC_t rc, xLibS8_t * name);
void xLibWeb2VarServeSet (char **var, unsigned char *status, const char *name);
void xLibWeb2VarSubmitSet (char **var, unsigned char *status, const char *name);
void xLibWeb2FileNameSet (char **var, unsigned char *status, void *handle);

void xLibWeb2SetSubmitType (void *cntx, unsigned int type);
char *xLibWeb2ErrorMessageGet (void *servContext, xLibId_t oid, xLibU32_t nlsId, xLibRC_t rc);

void *xLibWeb2Init (void *webSrvCntx);
void xLibWeb2AddKey (void *cntx, xLibId_t oid, xLibU16_t type,xLibId_t cpoid,xLibU32_t flags);
void xLibWeb2AddObj (void *cntx, xLibId_t oid, xLibId_t cpoid, xLibU16_t type, xLibU16_t flag,
                     xLibS8_t ** var, xLibU8_t * status, xLibS8_t * name, xLibS8_t * clazz);
void xLibWeb2AddPrivObj (void *cntx, xLibId_t oid, xLibId_t cpoid, xLibU16_t type, xLibU16_t flag,
                         xLibS8_t ** var, xLibU8_t * status, xLibS8_t * name, xLibU32_t offset, xLibS8_t * clazz);

void xLibWeb2AddPseudoKeyObj (void *cntx, xLibId_t oid, xLibId_t cpoid, xLibId_t keyoid,xLibU16_t type, xLibU16_t flag,
                     xLibS8_t * *var, xLibU8_t * status, xLibS8_t * name, xLibS8_t * clazz);
void xLibWeb2RegisterLoadFunc(void *cntx, xLibId_t oid, objFunc_t func);

void xLibWeb2Get (void *cntx);
void xLibWeb2RepeatGet (void *cntx, xLibU32_t pageMax, xLibU16_t rep,
                        xLibU16_t varSize, xLibU16_t statSize);
xLibBool_t xLibWeb2IsIterate( void *cntx,xLibIterate_t init);
char *xLibWeb2IterateGet(void *cntx, xLibCS8_t * clazz);
void xLibWeb2AddObjForIterate (void *cntx, xLibId_t oid, xLibId_t cpoid, xLibU16_t type, xLibU16_t flag, xLibS8_t * clazz);

void xLibWeb2Set (void *cntx);
void xLibWeb2RepeatSet (void *cntx, xLibU16_t flagVal, xLibU16_t rep,
                        xLibU16_t varSize, xLibU16_t statSize);
/*xLibRC_t  xLibSetKeyForPseudoKey (xLibWa_t *wap, xLibObj_t *pseudokeyobjp);*/
void xLibWeb2FilterSet (void *cntx, xLibId_t oid, xLibU16_t type, char *value);

void *xLibWeb2AddErrMap (void *cntx, xLibRC_t err1, xLibRC_t err2);
void xLibWeb2FinishServe (void *cntx, char **var, unsigned char *status);
char *xLibWeb2FinishSubmit (void *cntx, char *file, char *filterDump);
void xLibWeb2FinishServe (void *cntx, char **var, unsigned char *status);
char *xLibWeb2RepeatCount (void *cntx, xLibU32_t pageMax, xLibU32_t split);
char *xLibWeb2KeyOrDynamicString (void *cntx, xLibId_t oid, xLibId_t cpoid, xLibU16_t type,
                                  objFunc_t func);
char *xLibWeb2LoadPseudoKey(void *cntx, xLibId_t oid, xLibU16_t type, xLibId_t keyoid,
                            xLibU16_t keytype);
void xLibWeb2ObjUIEleValSet (void *cntx, xLibU32_t objID, xLibU32_t uiWebID, xLibU32_t uiCliID,
                             xLibU32_t eleID);
void xLibWeb2RowStatusDepListSet (void *cntx, xLibId_t rowOid, xLibId_t depOid);

xLibRC_t xLibWeb2ValidateIntMinMax (xLibId_t oid, xLibU16_t type, xLibS8_t * value);

char *xLibWeb2PrintString (void *context, int oid, int nlsId, ...);

#define XLIB_WEB2_ACCESS_LESSER 0
#define XLIB_WEB2_ACCESS_EQUAL 1
#define XLIB_WEB2_ACCESS_GREATER 2

char *xLibWeb2AllowAccess (void *srvCntx, int which);
void xLibWeb2SetSubmitType (void *cntx, unsigned int type);
void xLibWeb2InitSubmitType (void *cntx, unsigned long *var, unsigned char *status);

char *xLibWeb2NumberGet (void *servContext);
char *xLibWeb2RepeatIsFirst (void *context);
char *xLibWeb2RepeatIsLast (void *context);

void xLibWeb2ErrorMap (void *cntx, xLibId_t oid, xLibU32_t code, xLibU32_t mCode);

char* xLibWeb2GetMaximumInt(xLibId_t oid);
char* xLibWeb2GetMinimumInt(xLibId_t oid);
#endif
