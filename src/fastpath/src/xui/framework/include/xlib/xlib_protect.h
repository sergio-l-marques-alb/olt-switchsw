/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename xlib_protect.h
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

#ifndef XLIB_PROTECT_H
#define XLIB_PROTECT_H

/* To be included in the generated code */

#include "xlib.h"
#include "xlib_rc.h"

typedef enum
{
  XLIB_SUBMIT_FLAG_REFRESH = 1,
  XLIB_SUBMIT_FLAG_NEXT = 2,
  XLIB_SUBMIT_FLAG_SUBMIT = 8,
} xLibSubmitFlag_t;

typedef enum
{
  XLIB_NO_FLAG = 0,
  XLIB_IS_PVT = 1,
  XLIB_IS_KEY = 2,
  XLIB_IS_COLLATE = 4,
  XLIB_IS_VALIDATE = 8,
  XLIB_IS_FRESH = 16,
  XLIB_IS_PSEUDO_KEY = 32,
  XLIB_IS_KEY_FOR_PSEUDOKEY = 64,
  XLIB_IS_KEY_ITERATE = 128,
  XLIB_WO_FLAG= 256,
  XLIB_IS_UIOBJ_ROW_STATUS= 512,
  XLIB_IS_HIDDEN= 1024,
  XLIB_IS_GLOBAL_EDIT = 2048,
  XLIB_IS_SELECTOR = 4096,
} xLibWebFlag_t;

/* enum decalred to identify global/hybrid/inline edits for repeat */
typedef enum
{
  XLIB_NO_EDIT =0,
  XLIB_GLOBAL_EDIT =1,
  XLIB_INLINE_EDIT =2,
} xLibWebSelector_t;


extern int xLibTraceLevel;
extern char *xLibTraceCurrentFile;
extern int xLibTraceCurrentLine;

#ifdef XUI_DEBUG
#define XLIB_TRACE_LINE() \
 {\
   xLibTraceCurrentFile = __FILE__;\
   xLibTraceCurrentLine = __LINE__;\
   xLibTrace(2, xLibTraceCurrentFile, xLibTraceCurrentLine, "");\
 }
#else
#define XLIB_TRACE_LINE() \
{\
  xLibTraceCurrentFile = __FILE__;\
  xLibTraceCurrentLine = __LINE__;\
}
#endif

/* object nodes */
/*! \var typedef xLibRC_t (*getFunc_t) (void *wap, xLibId_t oid, xLibU8_t * buf, xLibU16_t * len)
    \brief A type definition for get function
*/
typedef xLibRC_t (*modFunc_t) (void *wap, xLibId_t oid, void *buf);
typedef xLibRC_t (*objFunc_t) (void *wap, void *buf);

/*! \var typedef xLibObjNode_t
    \brief A type definition to hold get/set functions and object id
*/
typedef struct
{
  xLibId_t oid;
  modFunc_t getFunc;
  modFunc_t setFunc;
} xLibObjNode_t;
xLibObjNode_t *xLibObjGetNodes (xLibU16_t * size);

typedef struct
{
  xLibU16_t type;
  xLibId_t id;
} xLibObjInfo_t;

void xLibLog (char *file, int line, const char *func);
int xLibDebugEnable (int dbg);

char *xLibObjBufPrintAlloc (void *context, int oid, char *msg);
char *xLibObjBufPrint (xLibId_t oid, xLibS8_t * name, xLibS8_t * value, xLibS8_t * fmt,
                       xLibS8_t * dst, xLibU16_t size);


/* NLS */
const char *xLibOEMStringGet (int token);
const char *xLibOEMOidNameGet (int oid);

/*Error Level */
const char *xLibOEMErrLevelGet (int token);

/* range */
const char *xLibNumberGet (int token, char *buf16);
const char *xLibObjMinGet (int oid, char *buf16);
const char *xLibObjMaxGet (int oid, char *buf16);
const char *xLibObjDefaultGet (int oid, char *buf16);
int xLibObjMinIntGet (int oid,char *buf16);
int xLibObjMaxIntGet (int oid,char *buf16);
xLibRC_t xLibObjectOidToTypeGet(void *arg, xLibId_t oid, xLibU16_t *type);

const char *xLibOEMErrorOutputNameGet (xLibU32_t rowindex,xLibId_t oid,xLibS8_t * name,  xLibS8_t * value, xLibRC_t rc,
                                       xLibS8_t * buf, xLibU16_t size);

xLibRC_t xLibGlobalMapGet (xLibRC_t rc);

#define XLIB_WEB2_ACCESS_LESSER 0
#define XLIB_WEB2_ACCESS_EQUAL 1
#define XLIB_WEB2_ACCESS_GREATER 2

/* types */
void xLibTypeInfoDataInit (int max);
void xLibEnumInfoInit(); 

xLibRC_t xLibTypeEncode (xLibIf_t intf, xLibU16_t type, xLibU8_t * inBuf, xLibU16_t inLen,
                         xLibS8_t * outBuf, xLibU16_t * outSizeLen, xLibBool_t dyn);
xLibRC_t xLibTypeDecode (xLibIf_t intf, xLibU16_t type, xLibS8_t * inBuf, xLibU8_t * outBuf,
                         xLibU16_t * outSizeLen, xLibBool_t dyn);

void xLibTypeInfoSet (char *type, int id, xLibEncodeFunc_t encode, xLibDecodeFunc_t decode);
xLibRC_t xLibDecode_int (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outLen);
xLibRC_t xLibEncode_int (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, xLibS8_t * out,
                         xLibU16_t * outLen);
xLibRC_t xLibDecode_uint (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outLen);
xLibRC_t xLibEncode_uint (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, xLibS8_t * out,
                          xLibU16_t * outLen);
xLibRC_t xLibDecode_string (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outLen);
xLibRC_t xLibEncode_string (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, xLibS8_t * out,
                            xLibU16_t * outLen);
xLibRC_t xLibDecode_ipv6 (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outLen);
xLibRC_t xLibEncode_ipv6 (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, xLibS8_t * out,
                          xLibU16_t * outLen);
xLibRC_t xLibDecode_ipv4 (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outLen);
xLibRC_t xLibEncode_ipv4 (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, xLibS8_t * out,
                          xLibU16_t * outLen);
xLibRC_t xLibDecode_mac (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outLen);
xLibRC_t xLibEncode_mac (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, xLibS8_t * out,
                         xLibU16_t * outLen);

xLibRC_t xLibDecode_uint64 (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outLen);
xLibRC_t xLibEncode_uint64 (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, xLibS8_t * out,
                         xLibU16_t * outLen);

int xLibTypeGetMinData (void);
int xLibTypeGetMaxData (void);
int xLibTypeGetMinUser (void);
int xLibTypeGetMaxUser (void);
int xLibTypeGetMinEnum (void);
int xLibTypeGetMaxEnum (void);

/* enums */
typedef struct
{
  xLibU16_t type;
  xLibU32_t appVal;
  xLibU32_t ewVal;
  xLibCS8_t *cliName;
  xLibCS8_t *webName;
  xLibCS8_t *optType;
} xLibEnumInfo_t;
xLibEnumInfo_t *xLibEnumInfoGet (xLibU16_t * size);
xLibEnumInfo_t *xLibEnumInfoFind (xLibU16_t type, xLibU16_t * left);
xLibBool_t xLibIsEnumType (xLibU16_t type);
xLibBool_t xLibIsRowStatusType (xLibU16_t type);
xLibRC_t xLibRowInfoCheck (xLibId_t oid, xLibU16_t type, xLibU8_t * val);
void xLibEnumInfoSet (xLibEnumInfo_t * infop, xLibU16_t type, xLibU32_t ewVal,
                      xLibU32_t appVal, xLibCS8_t * webName, xLibCS8_t * cliName,
                      xLibCS8_t * optType);

/*
 * SNMP
 */
typedef struct
{
  xLibObjInfo_t xobj;
  char value[1];
} xLibSnmpIndexInfo_t;

typedef struct
{
  xLibId_t obj;
  xLibU16_t type;
  xLibS8_t *snmpOid;
} xLibSnmpNode_t;
xLibSnmpNode_t *xLibSnmpGetNodes (xLibU16_t * size);

xLibId_t xLibSnmpFindOid (const char *name, xLibU16_t * type);
xLibRC_t xLibSnmpGetScalar (xLibId_t id, xLibU16_t type, xLibS8_t * bufp, xLibU16_t * sizeLen);
xLibRC_t xLibSnmpSetScalar (xLibId_t id, xLibU16_t type, xLibS8_t * bufp, xLibU16_t sizeLen);
xLibRC_t xLibSnmpGetKey (xLibId_t id, xLibU16_t type, xLibS8_t * in, xLibS8_t * out,
                         xLibU16_t * sizeLen);
xLibRC_t xLibSnmpGetInstance (int keyCount, xLibId_t * keys, int *keyTypes, xLibS8_t ** keyVals,
                              xLibId_t id, xLibU16_t type, xLibS8_t * out, xLibU16_t * sizeLen);

char *xLibMalloc (int size);
void xLibFree (void *ptr);
char *xLibStrDup (char *str);

#endif
