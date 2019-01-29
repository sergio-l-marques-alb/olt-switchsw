/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
********************************************************************************
*
* @filename  broad_l3_debug.h
*
* @purpose   This file contains debug functions and performance tests for
*            Layer3 module
*
* @component HAPI
*
* @comments  This file is internal to L3 module in HAPI and must not be
*            included in other components
*
* @create    10/1/2007
*
* @author    sdoke
*
* @end
*
*******************************************************************************/

#include "dapi_trace.h"
/* Debug flags */
#define BROAD_L3_DBG_NHOP       0x0001
#define BROAD_L3_DBG_HOST       0x0002
#define BROAD_L3_DBG_ROUTE      0x0004
#define BROAD_L3_DBG_ECMP       0x0008
#define BROAD_L3_DBG_TUNNEL     0x0010
#define BROAD_L3_DBG_L2_EVENTS  0x0020

#define BROAD_L3_DBG_DAPI_CMD   0x0100 
#define BROAD_L3_DBG_BCMX_ERR   0x0200
#define BROAD_L3_DBG_BCMX_ALL   0x0400
extern L7_BOOL broadL3DebugTrace;

#define HAPI_BROAD_L3_DEBUG(cond, format, args...)                  \
 do                                                                 \
 {                                                                  \
   if ((cond))                                                      \
   {                                                                \
     osapiSemaTake(broadL3DebugSema, L7_WAIT_FOREVER);              \
     if (broadL3DebugTrace == L7_TRUE)                              \
     {                                                              \
       dapiTraceGeneric(format, ##args);                            \
     }                                                              \
     else                                                           \
     {                                                              \
       sysapiPrintf("\n"format, ##args);                            \
     }                                                              \
     osapiSemaGive(broadL3DebugSema);                               \
   }                                                                \
 } while(0)

#define HAPI_BROAD_L3_ASYNC_DBG(format, args...)                    \
 do                                                                 \
 {                                                                  \
   if ((broadL3AsyncDebug))                                         \
   {                                                                \
     osapiSemaTake(broadL3DebugSema, L7_WAIT_FOREVER);              \
     if (broadL3DebugTrace == L7_TRUE)                              \
     {                                                              \
       dapiTraceGeneric(format, ##args);                            \
     }                                                              \
     else                                                           \
     {                                                              \
       sysapiPrintf("\n"format, ##args);                            \
     }                                                              \
     osapiSemaGive(broadL3DebugSema);                               \
   }                                                                \
 } while(0)

#define HAPI_BROAD_L3_BCMX_DBG(rv, format, args...)                 \
 do                                                                 \
 {                                                                  \
   if ((broadL3BcmxDebug & 0x1) && (rv != BCM_E_NONE))              \
   {                                                                \
     osapiSemaTake(broadL3DebugSema, L7_WAIT_FOREVER);              \
     if (broadL3DebugTrace == L7_TRUE)                              \
     {                                                              \
       dapiTraceGeneric(format, ##args);                            \
     }                                                              \
     else                                                           \
     {                                                              \
       sysapiPrintf("\n"format, ##args);                            \
     }                                                              \
     osapiSemaGive(broadL3DebugSema);                               \
   }                                                                \
   else if (broadL3BcmxDebug & 0x2)                                 \
   {                                                                \
     osapiSemaTake(broadL3DebugSema, L7_WAIT_FOREVER);              \
     if (broadL3DebugTrace == L7_TRUE)                              \
     {                                                              \
       dapiTraceGeneric(format, ##args);                            \
     }                                                              \
     else                                                           \
     {                                                              \
       sysapiPrintf("\n"format, ##args);                            \
     }                                                              \
     osapiSemaGive(broadL3DebugSema);                               \
   }                                                                \
 } while (0)


#define HAPI_BROAD_L3_DEBUG_DECL(d) d

/* This should be sizeof("01:02:03:04:05:06:07:08:09:10:11:12:13:14:15:16") */
#define BROAD_L3_ADDR_STR_LEN  48

extern L7_BOOL broadL3Debug;
extern L7_BOOL broadL3AsyncDebug;
extern L7_BOOL broadL3BcmxDebug;
extern L7_BOOL broadL3WaitDebug;
extern L7_BOOL broadL3SemaDebug;
extern L7_BOOL broadL3ArpDebug;

extern void *broadL3DebugSema;

extern L7_uint32 broadL3NhopWlistCurrDepth;
extern L7_uint32 broadL3HostWlistCurrDepth;
extern L7_uint32 broadL3RouteWlistCurrDepth;
extern L7_uint32 broadL3NhopWlistMaxDepth;
extern L7_uint32 broadL3HostWlistMaxDepth;
extern L7_uint32 broadL3RouteWlistMaxDepth;


void hapiBroadL3DebugDapiCmdPrint(const L7_char8 *func, DAPI_USP_t *usp,
                                  DAPI_CMD_t cmd, void *data);
void hapiBroadL3DebugArpQueryPrint(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data);

void hapiBroadL3DebugTaskIdSet(void);

void hapiBroadL3StatsShow(void);

void hapiBroadL3DebugStackTrace(void);
