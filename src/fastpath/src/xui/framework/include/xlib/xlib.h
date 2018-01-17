/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename xlib.h
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

#ifndef XLIB_H
#define XLIB_H

#include <stdio.h>
#include <string.h>

/* to be included by object handlers */
#include "xlib_rc.h"

typedef unsigned int xLibId_t;
typedef unsigned char xLibU8_t;
typedef char xLibS8_t;
typedef const char xLibCS8_t;
typedef unsigned short xLibU16_t;
typedef signed short xLibS16_t;
typedef unsigned int xLibU32_t;
typedef unsigned long xLibUL32_t;
typedef signed int xLibS32_t;
typedef signed long xLibSL32_t;
typedef unsigned long long xLibUL64_t;
typedef signed long long xLibSL64_t;
typedef char xLibStr256_t[256];
typedef char xLibStr6_t[6];
typedef unsigned int xLibIpV4_t;
typedef char xLibU32Range_t[256];
typedef char xLibMac_t[6];
typedef double xLibDouble64_t ;
 
typedef struct
{
  xLibU32_t   high;
  xLibU32_t   low;
} xLibU64_t;

typedef struct
{
  union
  {
    xLibU8_t addr8[16];
    xLibU16_t addr16[8];
    xLibU32_t addr32[4];
  } in6;
} xLibIpV6_t;

typedef struct
{
  xLibU8_t family;
  xLibU32_t   addr;
} xLibV4V6Mask_t;

typedef enum
{
  XLIB_FALSE,
  XLIB_TRUE,
} xLibBool_t;

typedef enum
{
  XLIB_FILTER_CHECKED_OUT=10,
  XLIB_FILTER_CHECKED_IN,
  XLIB_FILTER_RESERVED,
  XLIB_FILTER_UNRESERVED,
}xLibFilterFlags_t;

typedef enum
{
  XLIB_IF_WEB = 10,
  XLIB_IF_CLI,
  XLIB_IF_SNMP
} xLibIf_t;

typedef enum
{
  XLIB_ITERATE_FIRST =1,
  XLIB_ITERATE_NEXT
} xLibIterate_t;
typedef struct
{
  xLibU32_t iList[100];
  xLibU32_t count;
} xLibIntRange_t;

typedef struct
{
  xLibU8_t value[1024];
  xLibU16_t valen;
}xlibValen_t;

typedef struct
{
  xLibRC_t rc;
  xLibU16_t len;
}xLibObjWa_t;

typedef xLibRC_t (*xLibEncodeFunc_t) (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
                                      xLibS8_t * outBuf, xLibU16_t * outSizeLen);
typedef xLibRC_t (*xLibDecodeFunc_t) (xLibIf_t intf, xLibS8_t * inBuf,
                                      xLibU8_t * outBuf, xLibU16_t * outSizeLen);
typedef xLibRC_t (*objGetFunc_t) (void *wap, void *buf);

#define XLIB_INIT_VALEN(valen) xlibAllocValen(valen)
#define XLIB_INIT_WA(len) {XLIBRC_SUCCESS, len}

xLibRC_t xLibDataMapRegister (char *name, xLibEncodeFunc_t encode, xLibDecodeFunc_t decode);
void xLibFileHandlerRegister (void *func);

/* xLibAppInit: xLib expects this function in appl code */
void xLibAppInit (void);
void xLibAppInitRegister (void (*func) (void));
xLibRC_t xLibTransferProgressCheckSet(int (*callback)(void *), void *arg, char *url1, char *url2);

/* Buffer manipulation routines */
xLibRC_t xLibBufDataSet (void *arg, const xLibU8_t * data, const xLibU16_t len);
xLibRC_t xLibBufDataGet (void *arg, xLibU8_t * data, xLibU16_t * sizeLen);
xLibRC_t xLibBufDataPtrGet (void *arg, xLibU8_t ** data, xLibU16_t * len);
xLibRC_t xLibBufDataFileSet (void *arg, char *file);
xLibU16_t xLibBufFileDataGet (void *arg, xLibU8_t ** data, void **repeat);
xLibRC_t xLibBufFileDataSet (void *arg, char *name, xLibU8_t * data, xLibU16_t * sizeLen);
xLibRC_t xLibBufDataSet2 (void *arg, xLibId_t oid,xLibU16_t type,
                          const xLibU8_t * data, const xLibU16_t len);


/* Filter manipulation routines */
xLibRC_t xLibFilterGet (void *arg, xLibId_t oid, xLibU8_t * val, xLibU16_t * sizeLen);
xLibRC_t xLibFilterClear (void *arg, xLibId_t oid);
xLibRC_t xLibFilterPtrGet (void *arg, xLibId_t oid, xLibU8_t ** val, xLibU16_t * len);
xLibRC_t xLibFilterContextGet (void *arg, xLibId_t oid, void **context);
xLibRC_t xLibFilterContextSet (void *arg, xLibId_t oid, void *context);
xLibRC_t xLibFilterSet (void *arg, xLibId_t oid, xLibU16_t type, xLibU8_t * val, xLibU16_t len);
xLibRC_t xLibFilterSet2 (void *arg, xLibId_t oid, xLibU16_t type, xLibU8_t * val, xLibU16_t len);
xLibRC_t xLibFilterCheckOut(void *arg,xLibId_t oid);
xLibRC_t xLibFilterCheckIn(void *arg,xLibId_t oid);
xLibRC_t xLibFilterCache(void *arg,xLibId_t oid,xLibU16_t type,xLibU8_t *val,
                         xLibU16_t sizeLen,xLibBool_t noFilter);
xLibRC_t xLibFilterCheckIn(void *arg,xLibId_t oid);
xLibRC_t xLibFilterCheckOut(void *arg,xLibId_t oid);
xLibBool_t xLibIsFilterCheckedout(void *arg,xLibId_t oid);
xLibBool_t xLibIsFilterCheckedin(void *arg,xLibId_t oid);
xLibRC_t xLibFilterPop2 (void *arg, xLibId_t oid);
xLibBool_t xLibIsFilterReserved(void *arg,xLibId_t oid);
xLibRC_t xLibFilterReserve(void *arg,xLibId_t oid);
void xLibReserveUnreservedFilters(void *arg);
void xLibUnreserveFilters(void *arg);
void xLibUndoReserveFilters(void *arg);
xLibRC_t  xLibGetNextObjVal2(void *arg,xLibId_t oid, objGetFunc_t func, xLibU16_t type,
                          xLibU8_t *nextVal,xLibU16_t *len,xLibBool_t setFilter);
xLibBool_t xLibHasNext(void *arg,xLibId_t oid,xLibU16_t type,objGetFunc_t func);

xLibRC_t xLibUtiStart (void **cntxp);
xLibRC_t xLibUtiGet (void *cntx, xLibId_t oid, xLibU16_t type, xLibS8_t * out,
                     xLibU16_t * outSizeLen);
xLibRC_t xLibUtiSet (void *cntx, xLibId_t oid, xLibU16_t type, xLibS8_t * val);
xLibRC_t xLibUtiPush (void *cntx, xLibId_t oid, xLibU16_t type, xLibS8_t * val);
xLibRC_t xLibUtPop (void *cntx, xLibId_t oid);
xLibRC_t xLibUtiEnd (void *cntx);
xLibRC_t xLibUtScalar (xLibU16_t num, xLibId_t oid, xLibU16_t type, char *str);

void xLibTrace (int lvl, const char *file, int line, const char *fmt, ...);
xLibRC_t xLibDownLoadUrlSet(void *arg, char *name);
void *xLibGetServerContext (void *arg); 

xlibValen_t xlibAllocValen(xLibU16_t valen);

void xLibClearFiltersInvoke(void *arg);

#endif
