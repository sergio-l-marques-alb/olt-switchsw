/*
 * $Id: gdpll.h,  Exp $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        gdpll.h
 * Purpose:     
 */

#ifndef __BCM_INT_GDPLL_H__
#define __BCM_INT_GDPLL_H__

#if defined(INCLUDE_GDPLL)

#include <bcm_int/esw/gdpll_shared.h>

/* Number of GDPLL input array locations */
#define BCM_GDPLL_NUM_IA_ENTRIES    256
#define BCM_GDPLL_NUM_INPUT_EVENTS  256

#define GDPLL_DEBUGBUFF_LOCAL_COPY
#ifdef GDPLL_DEBUGBUFF_LOCAL_COPY
#define GDPLL_NUM_DEBUG_BUFFERS     2
#endif


int bcm_esw_gdpll_debug (int unit, int enable);

/* M7 DTCM region references
 */
#define BCM_M7_DTCM_BASE                (0x3264000)

/* Internal datastructure for event destination config */
typedef struct gdpll_event_dest_cfg_s {
    bcm_gdpll_event_dest_t event_dest;
    int                 ts_counter;       /* 0: TS0     1:TS1*/
    int                 ppm_check_enable; /* PPM check enable */
} gdpll_event_dest_cfg_t;

/* Internal datastructure for gdpll channel */
typedef struct dpll_chan_s {
    uint32 flag;        /* Channel flags */
    uint32 eventId_ref; /* Channel reference event */
    uint32 eventId_fb;  /* Channel feedback event */
    int    phaseConterRef;
}dpll_chan_t;

/* Internal datastructure for gdpll context */
typedef struct gdpll_context_s {
    /* Per dpll channel parameters */
    dpll_chan_t dpll_chan[BCM_GDPLL_NUM_CHANNELS];

    /* event_id array holds the Input array location for that event */
    int          eventId[BCM_GDPLL_NUM_INPUT_EVENTS];
    sal_mutex_t  mutex;

    /* User callback for debug registry */
    bcm_gdpll_debug_cb debug_cb;
    void         *pUserData;
#ifdef GDPLL_DEBUGBUFF_LOCAL_COPY
    /* Debug buffer local copy */
    uint32       *pSwDebugBuff[GDPLL_NUM_DEBUG_BUFFERS];
    int          debugFillCnt;
    int          debugWrPtr;  /* Pointers for buffer fill */
    int          debugRdPtr;  /* Pointers for buffer drain */
    sal_sem_t    debug_sem;
#endif

}gdpll_context_t;

#endif /* INCLUDE_GDPLL */
#endif /* __BCM_INT_GDPLL_H__ */
