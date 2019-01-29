/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_util.h
*
* @purpose    
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#ifndef _PIMDM_UTIL_H
#define _PIMDM_UTIL_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "pimdm_main.h"
#include "heap_api.h"
#include "l7_mcast_api.h"

/*******************************************************************************
**                             General Definitions                            **
*******************************************************************************/
#define PIMDM_BIT_SET(X,n)    ((X) |= 1 << (n))
#define PIMDM_BIT_CLR(X,n)    ((X) &= ~(1 << (n)))
#define PIMDM_BIT_TST(X,n)    ((X) & 1 << (n))

#define PIMDM_ALLOC(family,size) heapAlloc(mcastMapHeapIdGet(family),(size),__FILE__,__LINE__)
#define PIMDM_FREE(family,pMem) heapFree(mcastMapHeapIdGet(family), (pMem))

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/

extern L7_RC_t pimdmV4RouteChangeBufMemAlloc(pimdmCB_t *pimdmCB);

extern L7_RC_t pimdmUtilAppTimerSet(pimdmCB_t*            pimdmCB,
                                    L7_app_tmr_fn         pFunc,
                                    void                  *pParam,
                                    L7_uint32             timeOut,
                                    L7_uchar8             *errMessage,
                                    L7_uchar8             *successMessage,
                                    L7_APP_TMR_HNDL_t     *handle,
                                    L7_uchar8 *timerName);

extern void pimdmUtilAppTimerCancel(pimdmCB_t*            pimdmCB,
                                    L7_APP_TMR_HNDL_t     *handle);

extern L7_RC_t pimdmV6RouteChangeBufMemAlloc(pimdmCB_t *pimdmCB);

extern L7_uint32 pimdmUtilRandomGet(L7_uint32 minVal, L7_uint32 maxVal);

extern L7_RC_t
pimdmUtilIntfTTLThresholdGet (pimdmCB_t *pimdmCB, L7_uint32 rtrIfNum,
                              L7_uint32 *ttlThreshold);

extern L7_RC_t pimdmV4BestRoutesGet(pimdmCB_t *pimdmCB);

extern L7_RC_t pimdmV6BestRoutesGet(pimdmCB_t *pimdmCB);

extern L7_RC_t pimdmSocketInit (pimdmCB_t *pimdmCB);

extern L7_RC_t pimdmV6SocketOptionsSet(pimdmCB_t *pimdmCB,L7_int32 sockFd);

extern L7_RC_t
pimdmRTORouteChangeBufferAllocate (pimdmCB_t *pimdmCB);

extern L7_RC_t
pimdmUtilMemoryInit (L7_uint32 addrFamily);

extern L7_RC_t
pimdmUtilCtrlPktBufferFree (L7_uchar8 addrFamily,
                            L7_uchar8 *pBuffer);

extern L7_RC_t
pimdmSocketDeInit (pimdmCB_t *pimdmCB);

extern heapBuffPool_t *pimdmV6HeapPoolListGet(void);

#endif
