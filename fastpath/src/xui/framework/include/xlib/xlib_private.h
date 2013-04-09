/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename xlib_private.h
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

#ifndef XLIB_PRIVATE_H
#define XLIB_PRIVATE_H

/* to be included in the xlib only */

#include <stdlib.h>
#include <string.h>
#include "xlib_regex.h"
#include "xlib_protect.h"
#include "_xe_constants.h"

/* TODO: some how get rid of this include to make xlib independent of fastpath */
#include "fastpath_inc.h"
/* xLib types are needed for xLib Web functions */
#include "_xeobj_xtype.h"
#include "emweb_if.h"
/* utility functions and macros */
xLibU16_t xLibUtilHexDecode (xLibS8_t * in, xLibU8_t * out, xLibU16_t maxCount, xLibS8_t sep);
xLibRC_t xLibUtil64BitsToString (xLibU64_t num, xLibS8_t * buf);

/* standard library functions */
char *xLibStrncpy (char *dest, const char *src, int n);

extern int xLibTraceLevel;
#define XLIB_ASSERT(x) \
  if (!(x))\
{\
  xLibTrace(0, __FILE__, __LINE__, "XLIB_ASSERT:");\
  exit(0);\
}

#define XLIB_TRACE(fmt,arg...) xLibTrace(2, __FILE__, __LINE__, fmt,##arg)
#define XLIB_WARN(fmt,arg...) xLibTrace(1, __FILE__, __LINE__, fmt,##arg)
#define XLIB_WARN2(err,fmt,arg...) if(err == XLIB_TRUE) xLibTrace(1, __FILE__, __LINE__, fmt,##arg)
#define XLIB_ERROR(fmt,arg...) xLibTrace(0, __FILE__, __LINE__, fmt,##arg)

void xLibDumpValue (char *file, int line, xLibU16_t type, xLibU8_t * value, xLibU16_t len);
#define XLIB_DUMP_VALUE(t,v,l) xLibDumpValue(__FUNCTION__, __LINE__, t, v, l)

/* TODO: These constants should be comming from branding xml file so that we don't have platform dependencies here */

#ifdef L7_PRODUCT_SMARTPATH
/* 1 is added to accomidate EOS */
#define XLIB_MAX_VAL_LEN  512+1
#define XLIB_MAX_OBJ 24
#define XLIB_MAX_ERROR_CODE 8
#define XLIB_MAX_KEY 16
#define MAX_WAP_BUFFER_SIZE 16 * 1024
#else
#define XLIB_MAX_KEY_LEN  1024
/* 1 is added to accomidate EOS*/
#define XLIB_MAX_VAL_LEN  1024+1
#define XLIB_MAX_OBJ 128
#define XLIB_MAX_ERROR_CODE 32
#define XLIB_MAX_KEY 128
#define MAX_WAP_BUFFER_SIZE 64 * 1024
#endif

#define XLIB_MAX_REPEAT_COUNT 20000
#define XLIB_BUF_SIZE_2048 2048
#define XLIB_BUF_SIZE_100 100
#define XLIB_BUF_SIZE_256 256

typedef struct xLibCliVar_s
{
  struct xLibCliVar_s *next;
  char *var;
  int len;
} xLibCliVar_t;

typedef struct
{
  xLibId_t oid;
  xLibId_t cpoid;
  xLibId_t keyoid;
  /* instead of isKey now flag bit gives information on key.validation information */
  xLibU16_t flag;
  xLibU16_t valen;
  xLibU8_t value[XLIB_MAX_VAL_LEN];
  xLibRC_t rc;
  xLibU16_t type;
  /* added for error Map */
  xLibU16_t mCount;
  xLibU32_t code[XLIB_MAX_ERROR_CODE];
  xLibU32_t mCode[XLIB_MAX_ERROR_CODE];
  /* objValID to indicate the validation to be used at object level */
  xLibU16_t objValID;
  /* added dependent list, depCount for row-status object */
  xLibU16_t depCount;
  xLibId_t depList[XLIB_MAX_OBJ];

/* Added for UI Objects module load function reference */
  objFunc_t loadFunc; 
  

  union
  {
    struct
    {
      xLibS8_t **var;
      xLibU8_t *status;
      xLibS8_t *name;
      xLibS8_t *clazz;
      /* webValID to indicate the validation to be used at UI level:Web */
      xLibU16_t webValID;
      /* eleValID to indicate the validation to be used at element level per page */
      xLibU16_t eleValID;
    } web;
    struct
    {
      xLibU32_t len;
      xLibS8_t *label;
      xLibCliVar_t *varHead;
      xLibCliVar_t *varTail;
      /* cliValID to indicate the validation to be used at UI level:Cli */
      xLibU16_t cliValID;
    } cli;
  } ui;
} xLibObj_t;

typedef struct
{
  xLibU8_t value[XLIB_MAX_VAL_LEN];
  xLibU16_t valen;
  xLibBool_t valid;
  xLibId_t oid;
  void *wap;
} xLibBuf_t;

typedef struct
{
  xLibId_t oid;
  xLibU8_t value[XLIB_MAX_VAL_LEN];
  xLibU16_t valen;
  xLibU16_t type;
  xLibId_t cpoid;
  xLibU32_t flags;
} xLibKey_t;

typedef struct xLibFilter_s
{
  struct xLibFilter_s *next;
  xLibId_t oid;
  xLibU8_t depth;
  /* flag added to mark the filters in reserve/unreserve mode 
     to have the flexibility to decide upon filter over-writing */

  xLibFilterFlags_t flag;
  xLibU16_t type;
  xLibS16_t valen;
  xLibU8_t value[XLIB_MAX_VAL_LEN];
  void *context;
} xLibFilter_t;

typedef struct
{
  long magic;
#define XLIB_WA_MAGIC (long)((long)'X' << 24) + ((long)'L' << 16) + ((long)'I' << 8) + 'W'
  void **filters;
  xLibObj_t objs[XLIB_MAX_OBJ];
  xLibU16_t objCount;
  xLibU16_t keyCount;
  xLibKey_t keys[XLIB_MAX_KEY];
  xLibS8_t encBuf[XLIB_MAX_VAL_LEN];
  xLibU16_t encLen;  
  /* this feild is introduced to recognize the type of interface for which WAP is intialized(CLI or WEB) */
  xLibIf_t intf;

  void *fileHandler;
    xLibRC_t (*fileNameSet) (void *, char *);

  union
  {
    struct
    {
      void *webCntxt;
      xLibS8_t htmlBuffer[MAX_WAP_BUFFER_SIZE];
      xLibU32_t htmlBufferOffset;
      char buf256[256];
      char buf1024[1024];
      char buf2048[2048];
    } web;
    struct
    {
      void *cliCntxt;
      void *printFunc;          /* custom print function for CLI */
      xLibU16_t dispType;       /* default display type which is used in default display function */
    } cli;
  } ui;
} xLibWa_t;

/* encode/decode */
typedef struct
{
  xLibS8_t *name;
  xLibU16_t type;
  xLibEncodeFunc_t encode;
  xLibDecodeFunc_t decode;
} xLibEncDec_t;

void xLibTypeInfoInit (void);

void xLibWaInit (xLibWa_t * wap);
void *xLibFileHandlerGet (void);
void xLibWaAddKey (xLibWa_t * wap, xLibId_t oid, xLibU16_t type,xLibId_t cpoid,xLibU32_t flags);
xLibObj_t *xLibWaAddObj (xLibWa_t * wap, xLibId_t oid, xLibId_t cpoid, xLibU16_t type,
                         xLibU16_t flag);
xLibObj_t *xLibWaAddObjWithKey (xLibWa_t * wap, xLibId_t oid,xLibId_t cpoid,  xLibId_t keyoid , xLibU16_t type, xLibU16_t flag);
void xLibWaErrorMap (xLibWa_t * wap, xLibId_t oid, xLibU32_t code, xLibU32_t mCode);
void xLibWaObjUIEleValSet (xLibWa_t * wap, xLibU32_t objID, xLibU32_t uiWebID, xLibU32_t uiCliID,
                           xLibU32_t eleID);
void xLibWaRowStatusDepListSet (xLibWa_t * wap, xLibId_t oid, xLibId_t depOid);
xLibS8_t *xLibRcStrErr (xLibRC_t rc);

char *xLibFilterDumpBuffer (void *cntx, int intf);
void xLibFilterDumpLoad (void *arg, char *dump);
xLibRC_t xLibFilterSet (void *arg, xLibId_t oid, xLibU16_t type, xLibU8_t * val, xLibU16_t len);
xLibRC_t xLibFilterRemoveAll (void **arg);
xLibRC_t xLibFilterClearAll (void *arg);
xLibRC_t xLibFilterAttributeSet(xLibFilter_t *fp, xLibId_t oid, xLibU16_t type, xLibU8_t * val, xLibU16_t len);
xLibRC_t xLibFilterPopFromKeys (void *arg);
xLibRC_t xLibFilterPop (void *arg, xLibId_t oid);
xLibRC_t xLibFilterPushFromKeys (void *arg);
xLibRC_t xLibFilterPush (void *arg, xLibId_t oid);
xLibRC_t xLibFilterCheckOutPush (void *arg, xLibId_t oid);
void xLibClearReserveUnreserveFilters (void *arg);

xLibEncDec_t *xLibEncDecInfoGet (xLibU16_t * size);
/*xLibRC_t xLibTypeEncode (xLibIf_t intf, xLibU16_t type, xLibU8_t * inBuf, xLibU16_t inLen,
                         xLibS8_t * outBuf, xLibU16_t * outSizeLen, xLibBool_t dyn);
xLibRC_t xLibTypeDecode (xLibIf_t intf, xLibU16_t type, xLibS8_t * inBuf, xLibU8_t * outBuf,
                         xLibU16_t * outSizeLen, xLibBool_t dyn);*/
xLibRC_t xLibTypeColate (xLibIf_t intf, xLibU16_t type, xLibS8_t * inBuf, xLibU16_t inLen,
                         void **colWap);
char *xLibTypeColatedGet (void **colWap);
void *xLibTypeColateFinish (void **colWap);
xLibRC_t xLibObjGet (void *wap, xLibId_t oid, xLibId_t cpoid, xLibU8_t * inBuf,
                     xLibU16_t * sizeLen);
xLibRC_t xLibObjSet (void *wap, xLibId_t oid, xLibU8_t * inBuf, xLibU16_t len);
xLibBool_t xLibIsPvtObj (xLibWa_t * wap, xLibId_t oid);



/* load handler for ui module */
xLibRC_t xLibObjInvokeLoad(void *cntx,xLibObj_t *objp);

xLibRC_t xLibObjLoadKey (xLibWa_t * wap);
xLibU16_t xLibObjCountKey (xLibWa_t * wap, xLibU32_t max, xLibBool_t * more);
xLibRC_t xLibObjGetMayBeKey (xLibWa_t * wap, xLibObj_t * objp);
void xLibCliInit (void);
/*Moving this to xlib_protect.h to be available for  generated code*/
/*
xLibRC_t xLibRowInfoCheck (xLibId_t oid, xLibU16_t type, xLibU8_t * val);
*/

xLibRC_t xLibBufDataInit (void *wap, void *arg, xLibId_t oid);
xLibBool_t xLibWeb2IsDisable (xLibObj_t * objp);
xLibRC_t  xLibSetKeyForPseudoKey(xLibWa_t * wap, xLibObj_t * pseudokeyobjp);
L7_RC_t usmWebConvertHexToUnicode(L7_char8 *in, L7_char8 *out);
/* validation functions */

typedef struct
{
  xLibS8_t *output;
  xLibU32_t level;
  xLibU32_t type;
  xLibU32_t parentType;
  xLibU32_t error;
  xLibRC_t rc;
} xLibVal_t;

typedef enum
{
  XLIB_VALIDATION = 1,
  XLIB_AND,
  XLIB_OR,
  XLIB_REG_AND,
  XLIB_REG_OR,
  XLIB_CLOSE_OR,
  XLIB_CLOSE_AND
} validation_t;

void xLibValInfoSet (xLibVal_t * val, xLibS8_t * output, xLibU32_t level, xLibU32_t type,
                     xLibU32_t parentType, xLibU32_t error);
xLibRC_t xLibFinalValidate (xLibVal_t opStk[], xLibU32_t opCount);
xLibRC_t xLibValidateTravList (xLibVal_t val[], xLibU32_t size, xLibS8_t * value);
xLibRC_t xLibValidate (xLibWa_t * wap, xLibU16_t index, xLibId_t oid, xLibU16_t type,
                       xLibS8_t * value);
xLibRC_t xLibValidateMapIDtoVal (xLibU16_t id, xLibS8_t * value);

void xLibAllocWorkAreaMemory (xLibWa_t * wap);
void xLibFreeWorkAreaMemory (xLibWa_t * wap, int freeKey);

#endif
