
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename fpobj_util.h
*
* @purpose
*
* @component object handlers
*
* @comments
*
* @create 04/17/2007
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#ifndef FPOBJ_UTIL_H
#define FPOBJ_UTIL_H
#define FPOBJLIB_MAX_VAL_LEN  1024

#include <stdio.h>
#include <string.h>

#include "l7_common.h"
#include "fastpath_enum.h"
#include "xlib.h"

#ifdef L7_PRODUCT_SMARTPATH
#define USMWEB_BUFFER_SIZE_256 256
#endif
typedef struct
{
  xLibRC_t rc;
  L7_RC_t l7rc;
  xLibU16_t len;
}fpObjWa_t;

typedef struct
{
  xLibU8_t value[FPOBJLIB_MAX_VAL_LEN];
  xLibU16_t valen;
  xLibBool_t valid;
  xLibId_t oid;
}fpObjLibBuf_t;

#define FPOBJ_INIT_WA(len) {XLIBRC_SUCCESS, L7_SUCCESS, len}
#define FPOBJ_INIT_WA2() {rc : XLIBRC_SUCCESS, l7rc : L7_SUCCESS}

#define FPOBJ_CHECK_AND_RETURN_FAILURE(a,b,c,d) if(a != b) return c;

typedef enum
{
  FPOBJ_TRC_DBG,
  FPOBJ_TRC_WARN,
  FPOBJ_TRC_ERR,
}fpobj_trc_t;

#define usmDbGetUnknown(x,y...) L7_ERROR
#define usmDbSetUnknown(x,y...) L7_ERROR
#define usmDbGetFirstUnknown(x,y...) L7_ERROR
#define usmDbGetNextUnknown(x,y...) L7_ERROR
#define usmDbAddUnknown(x,y...) L7_ERROR
#define usmDbDeleteUnknown(x,y...) L7_ERROR

typedef enum
{
  FPOBJ_TRACE_TYPE_ENTER = 100,
  FPOBJ_TRACE_TYPE_EXIT,
  FPOBJ_TRACE_TYPE_CURRENT_KEY,
  FPOBJ_TRACE_TYPE_NEW_KEY,
}fpObjTraceType_t;

void fpObjDebugTrace (fpObjTraceType_t which, void *bufp, void *owcp, int *index, int len, const char *func, int line);

#define FPOBJ_TRACE_ENTER(bufp) fpObjDebugTrace(FPOBJ_TRACE_TYPE_ENTER, bufp, NULL, (int *)0, 0, __FUNCTION__, __LINE__)
#define FPOBJ_TRACE_EXIT(bufp,owc) fpObjDebugTrace(FPOBJ_TRACE_TYPE_EXIT, bufp, &owc, (int *)0, 0, __FUNCTION__, __LINE__)
#define FPOBJ_TRACE_CURRENT_KEY(bufp,indx,len) fpObjDebugTrace(FPOBJ_TRACE_TYPE_CURRENT_KEY, bufp, NULL, (int *)indx, len, __FUNCTION__, __LINE__)
#define FPOBJ_TRACE_NEW_KEY(bufp,indx,len) fpObjDebugTrace(FPOBJ_TRACE_TYPE_NEW_KEY, bufp, NULL, (int *)indx, len, __FUNCTION__, __LINE__)
#define FPOBJ_TRACE_VALUE(...)

xLibRC_t fpObjAppGetKey_PhyicalInterface (void *wap, void *bufp, xLibId_t oid);
xLibRC_t fpObjAppGetKey_PhysicalLagInterface (void *wap, void *bufp, xLibId_t oid);
xLibRC_t fpObjAppGetKey_Dot1sInstance (void *wap, void *bufp, xLibId_t oid);
xLibRC_t fpObjAppGetKey_VlanInstance (void *wap, void *bufp, xLibId_t oid);
xLibRC_t fpObjAppGetKey_U32Number (void *wap, void *bufp, xLibId_t oid, L7_uint32 min,
                                   L7_uint32 max, L7_uint32 inc);

typedef L7_RC_t (*usmDbU32KeyU32ValGetWithUnit_t) (L7_uint32, L7_uint32, L7_uint32 *);
typedef L7_RC_t (*usmDbU32KeyU32ValSetWithUnit_t) (L7_uint32, L7_uint32, L7_uint32);
xLibRC_t fpObjAppGetU32KeyU32ValueWithUnit (void *wap, void *bufp, xLibId_t keyId,
                                            usmDbU32KeyU32ValGetWithUnit_t usmdb, L7_uint32 unit);
xLibRC_t fpObjAppSetU32KeyU32ValueWithUnit (void *wap, void *bufp, xLibId_t keyId,
                                            usmDbU32KeyU32ValSetWithUnit_t usmdb, L7_uint32 unit);

typedef L7_RC_t (*usmDbU32KeyU32ValGet_t) (L7_uint32, L7_uint32 *);
typedef L7_RC_t (*usmDbU32KeyU32ValSet_t) (L7_uint32, L7_uint32);
xLibRC_t fpObjAppGetU32KeyU32Value (void *wap, void *bufp, xLibId_t keyId, usmDbU32KeyU32ValGet_t usmdb);
xLibRC_t fpObjAppSetU32KeyU32Value (void *wap, void *bufp, xLibId_t keyId, usmDbU32KeyU32ValSet_t usmdb);

typedef L7_RC_t (*usmDbU32ValGetWithUnit_t) (L7_uint32, L7_uint32 *);
typedef L7_RC_t (*usmDbU32ValSetWithUnit_t) (L7_uint32, L7_uint32);
xLibRC_t fpObjAppGetU32ValueWithUnit (void *wap, void *bufp, usmDbU32ValGetWithUnit_t usmdbFunc, L7_uint32 unit);
xLibRC_t fpObjAppSetU32ValueWithUnit (void *wap, void *bufp, usmDbU32ValSetWithUnit_t usmdbFunc, L7_uint32 unit);

typedef L7_RC_t (*usmDbU32ValGet_t) (L7_uint32 *);
typedef L7_RC_t (*usmDbU32ValSet_t) (L7_uint32);
xLibRC_t fpObjAppGetU32Value (void *wap, void *bufp, usmDbU32ValGet_t usmdbFunc);
xLibRC_t fpObjAppSetU32Value (void *wap, void *bufp, usmDbU32ValSet_t usmdbFunc);

#define XLIB_HWARN(fmt,arg...) xLibTrace(1, __FILE__, __LINE__, fmt,##arg)

extern xLibIpV6_t fpObjBlankIpV6_g;

#define FPOBJ_CLR_IPV4(x) memset((void *)&x, 0, sizeof(x))
#define FPOBJ_CLR_IPV6(x) memset((void *)&x, 0, sizeof(x))
#define FPOBJ_CLR_MAC(x) memset((void *)&x, 0, sizeof(x))
#define FPOBJ_CLR_S32(x) memset((void *)&x, 0, sizeof(x))
#define FPOBJ_CLR_STR256(x) memset((void *)&x, 0, sizeof(x))
#define FPOBJ_CLR_U32(x) memset((void *)&x, 0, sizeof(x))

#define FPOBJ_CMP_IPV4(x,y) (x == y)
#define FPOBJ_CMP_IPV6(x,y) memcmp(&x,&y,sizeof(x))
#define FPOBJ_CMP_MAC(x,y)  memcmp(x,y,sizeof(x))
#define FPOBJ_CMP_S32(x,y)  (x == y)
#define FPOBJ_CMP_STR256(x,y) strncmp(x,y,sizeof(x))
#define FPOBJ_CMP_U32(x,y)  (x == y)

#define FPOBJ_CPY_IPV4(x,y) x = y
#define FPOBJ_CPY_IPV6(x,y) memcpy(&x, &y, sizeof(x))
#define FPOBJ_CPY_MAC(x,y) memcpy(x, y, sizeof(x))
#define FPOBJ_CPY_S32(x,y) x = y
#define FPOBJ_CPY_STR256(x,y) memcpy(x, y, sizeof(x))
#define FPOBJ_CPY_U32(x,y) x = y

int fpObjPortFromMaskGet (L7_INTF_MASK_t intfmask);
int fpObjNextIntfGet (L7_INTF_MASK_t * intfMask, xLibS32_t prevPort);

L7_BOOL fpObjIsStackingSupported (void);
L7_RC_t fpObjScanUSP (char *str, L7_uint32 * unitp, L7_uint32 * slotp, L7_uint32 * portp);
L7_RC_t fpObjPrintUSP (char *str, L7_uint32 size, L7_uint32 unit, L7_uint32 slot, L7_uint32 port);

L7_RC_t configScriptCopy(L7_char8 *sourceFile);

/* parsing routines for Buffered and Persistent Logs */
L7_RC_t extractTime(L7_uchar8 *logBuff,L7_uchar8 *buf,L7_uint32 buffMsgLen);
L7_RC_t extractComponent(L7_uchar8 *logBuff,L7_uchar8 *destBuff);
L7_RC_t extractLogDesc(L7_uchar8 *logBuff,L7_uchar8 *buffMsg,L7_uint32 buffMsgLen);
L7_RC_t extractSeverity(L7_uchar8 *logBuff,L7_uint32 *severity);
L7_RC_t stripQuotes(L7_uchar8 *inputString);
L7_RC_t fpObjUtil__ValidateSummerTime(L7_uint32 stMonth,
                              L7_uint32 stDay, L7_uint32 stYear, L7_uint32 stWeek,
                              L7_uint32 endMonth, L7_uint32 endDay,
                              L7_uint32 endYear, L7_uint32 endWeek, L7_BOOL recurFlag);

#endif
