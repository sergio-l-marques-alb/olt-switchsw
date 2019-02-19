/*
 * $Id: gdpll.c,v 0.1 2017/11/09 Asheefikbal Exp $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:       gdpll.c
 *
 * Remarks:    Broadcom StrataSwitch Time GDPLL API
 *
 * Functions:
 *   Private Interface functions
 *      _bcm_esw_gdpll_event_divisor_misc_set
 *      _bcm_esw_gdpll_event_divisor_misc_get
 *      _bcm_esw_gdpll_event_divisor_port_set
 *      _bcm_esw_gdpll_event_divisor_port_get
 *      _bcm_esw_gdpll_event_dest_misc_set
 *      _bcm_esw_gdpll_event_dest_misc_get
 *      _bcm_esw_gdpll_event_dest_port_set
 *      _bcm_esw_gdpll_event_dest_port_get
 *      _bcm_esw_gdpll_event_enable_misc_set
 *      _bcm_esw_gdpll_event_enable_misc_get
 *      _bcm_esw_gdpll_event_enable_port_set
 *      _bcm_esw_gdpll_event_enable_port_get
 *      _bcm_esw_gdpll_event_roe_52b_set
 *      _bcm_esw_gdpll_event_roe_52b_get
 *      _bcm_esw_gdpll_event_config_set
 *
 *      _bcm_esw_gdpll_capture_enable_m7_set
 *      _bcm_esw_gdpll_capture_enable_m7_get
 *      _bcm_esw_gdpll_capture_enable_cpu_set
 *      _bcm_esw_gdpll_capture_enable_cpu_get
 *
 *      _bcm_esw_gdpll_chan_update_set
 *      _bcm_esw_gdpll_chan_update_get
 *      _bcm_esw_gdpll_chan_enable_set
 *      _bcm_esw_gdpll_chan_enable_get
 *      _bcm_esw_gdpll_chan_out_txpi_set
 *      _bcm_esw_gdpll_chan_out_txpi_get
 *      _bcm_esw_gdpll_chan_out_misc_set
 *      _bcm_esw_gdpll_chan_out_misc_get
 *      _bcm_esw_gdpll_chan_out_enable_set
 *      _bcm_esw_gdpll_chan_out_enable_get
 *      _bcm_esw_gdpll_chan_priority_set
 *
 *      _bcm_esw_gdpll_chan_config_dpll_set
 *      _bcm_esw_gdpll_chan_config_dpll_get
 *      _bcm_esw_gdpll_chan_debug_mode_set
 *      _bcm_esw_gdpll_chan_debug_mode_get
 *      _bcm_esw_gdpll_debug_enable_set
 *
 *      _bcm_esw_gdpll_flush
 *      _bcm_esw_gdpll_init
 *      _bcm_esw_gdpll_deinit
 *      _bcm_esw_gdpll_input_eventId_get
 *      _bcm_esw_gdpll_input_array_alloc
 *      _bcm_esw_gdpll_input_array_free
 *      _bcm_esw_gdpll_chan_alloc
 *      _bcm_esw_gdpll_chan_free
 *
 *   Public Interface functions
 *      bcm_esw_gdpll_chan_create
 *      bcm_esw_gdpll_chan_destroy
 *      bcm_esw_gdpll_chan_enable
 *      bcm_esw_gdpll_chan_state_get
 *      bcm_esw_gdpll_chan_debug_enable
 *      bcm_esw_gdpll_debug_cb_register
 *      bcm_esw_gdpll_flush
 *
 */
#include <shared/util.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/iproc.h>

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_internal.h>
#endif

#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm_int/esw/switch.h>

#if defined(INCLUDE_GDPLL)
#include <bcm/gdpll.h>
#include <bcm_int/esw/gdpll.h>


/****************************************************************************/
/*                      MACRO definitions                                   */
/****************************************************************************/

/* TBD - As per section 6.6, port events are from 0 to 191 and misc events
 * start from 192
 */
#define BCM_GDPLL_IA_START_MISC          192
#define BCM_GDPLL_IA_START_TXPI_TXSOF    128

#define BCM_GDPLL_MISC_EVENT_EN_CPU     (1<<0)
#define BCM_GDPLL_MISC_EVENT_EN_BS0HB   (1<<1)
#define BCM_GDPLL_MISC_EVENT_EN_BS1HB   (1<<2)
#define BCM_GDPLL_MISC_EVENT_EN_IPDM0   (1<<3)
#define BCM_GDPLL_MISC_EVENT_EN_IPDM1   (1<<4)
#define BCM_GDPLL_MISC_EVENT_EN_TS1     (1<<5)
#define BCM_GDPLL_MISC_EVENT_EN_RP1IF   (1<<6)
#define BCM_GDPLL_MISC_EVENT_EN_RP1RF   (1<<7)
#define BCM_GDPLL_MISC_EVENT_EN_BS0CONV (1<<8)
#define BCM_GDPLL_MISC_EVENT_EN_BS1CONV (1<<9)

/* GDPLL debug buffer thresholds */
#define BCM_GDPLL_DEBUG_BUFFER_START    (0x3274000)
#define BCM_GDPLL_DEBUG_BUFFER_SIZE     (1024*40)
#define BCM_GDPLL_DEBUG_THRESHOLD_2K    2048
#define BCM_GDPLL_DEBUG_THRESHOLD_8K    8196
#define BCM_GDPLL_DEBUG_THRESHOLD       (BCM_GDPLL_DEBUG_THRESHOLD_2K)
#define BCM_GDPLL_DEBUG_SW_SIZE         (BCM_GDPLL_DEBUG_THRESHOLD)

/* Debug buffer interrupts */
#define BCM_GDPLL_DEBUG_INT_THR         (1<<0)
#define BCM_GDPLL_DEBUG_INT_OF          (1<<1)
#define BCM_GDPLL_DEBUG_INT_ECC0        (1<<2)
#define BCM_GDPLL_DEBUG_INT_ECC1        (1<<3)
#define BCM_GDPLL_DEBUG_INT             (BCM_GDPLL_DEBUG_INT_THR  | \
                                         BCM_GDPLL_DEBUG_INT_OF   | \
                                         BCM_GDPLL_DEBUG_INT_ECC0 | \
                                         BCM_GDPLL_DEBUG_INT_ECC1)

#define READ_NS_REGr(unit, reg, idx, rvp) \
    soc_iproc_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, idx), rvp)
#define WRITE_NS_REGr(unit, reg, idx, rv) \
    soc_iproc_setreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, idx), rv)


/****************************************************************************/
/*                      LOCAL VARIABLES DECLARATION                         */
/****************************************************************************/
/* Like context structure, etc */
STATIC
soc_reg_t  gpio_regs[6] = { NS_TIMESYNC_GPIO_0_CTRLr,
                            NS_TIMESYNC_GPIO_1_CTRLr,
                            NS_TIMESYNC_GPIO_2_CTRLr,
                            NS_TIMESYNC_GPIO_3_CTRLr,
                            NS_TIMESYNC_GPIO_4_CTRLr,
                            NS_TIMESYNC_GPIO_5_CTRLr };

STATIC
soc_reg_t  bs_pll_regs[2] = { NS_BROADSYNC0_CLK_EVENT_CTRLr,
                            NS_BROADSYNC1_CLK_EVENT_CTRLr };

STATIC
soc_reg_t mapper_port_enable_regs[3] = { NS_TIMESYNC_MAPPER_PORT_ENABLE_0r,
                                         NS_TIMESYNC_MAPPER_PORT_ENABLE_1r,
                                         NS_TIMESYNC_MAPPER_PORT_ENABLE_2r
                                       };

STATIC
soc_reg_t mapper_port_enable[32] = { PORT0_TS_ENABLEf, PORT1_TS_ENABLEf,
                                     PORT2_TS_ENABLEf, PORT3_TS_ENABLEf,
                                     PORT4_TS_ENABLEf, PORT5_TS_ENABLEf,
                                     PORT6_TS_ENABLEf, PORT7_TS_ENABLEf,

                                     PORT8_TS_ENABLEf, PORT9_TS_ENABLEf,
                                     PORT10_TS_ENABLEf, PORT11_TS_ENABLEf,
                                     PORT12_TS_ENABLEf, PORT13_TS_ENABLEf,
                                     PORT14_TS_ENABLEf, PORT15_TS_ENABLEf,

                                     PORT16_TS_ENABLEf, PORT17_TS_ENABLEf,
                                     PORT18_TS_ENABLEf, PORT19_TS_ENABLEf,
                                     PORT20_TS_ENABLEf, PORT21_TS_ENABLEf,
                                     PORT22_TS_ENABLEf, PORT23_TS_ENABLEf,

                                     PORT24_TS_ENABLEf, PORT25_TS_ENABLEf,
                                     PORT26_TS_ENABLEf, PORT27_TS_ENABLEf,
                                     PORT28_TS_ENABLEf, PORT29_TS_ENABLEf,
                                     PORT30_TS_ENABLEf, PORT31_TS_ENABLEf
                                  };

STATIC gdpll_context_t *pGdpllCtx = NULL;

/****************************************************************************/
/*                     Internal functions implementation                    */
/****************************************************************************/
STATIC int
_bcm_esw_gdpll_event_divisor_misc_set(int unit, bcm_gdpll_input_event_t event_misc,
                            uint32 divisor)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;

    switch (event_misc) {
        case bcmGdpllInputEventCpu:
        case bcmGdpllInputEventBS0HB:
        case bcmGdpllInputEventBS1HB:
        case bcmGdpllInputEventIPDM0:
        case bcmGdpllInputEventIPDM1:
        case bcmGdpllInputEventTS1TS:
        case bcmGdpllInputEventRP1IF:
        case bcmGdpllInputEventRP1RF:
        case bcmGdpllInputEventBS0ConvTC:
        case bcmGdpllInputEventBS1ConvTC:
            /* No divider */
            rv = BCM_E_INTERNAL;
            break;

        case bcmGdpllInputEventGPIO0:
        case bcmGdpllInputEventGPIO1:
        case bcmGdpllInputEventGPIO2:
        case bcmGdpllInputEventGPIO3:
        case bcmGdpllInputEventGPIO4:
        case bcmGdpllInputEventGPIO5:
            idx = event_misc - bcmGdpllInputEventGPIO0;
            READ_NS_REGr(unit, gpio_regs[idx], 0, &regVal);
            soc_reg_field_set(unit,  gpio_regs[idx], &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, gpio_regs[idx], 0, regVal);
            break;

        case bcmGdpllInputEventBS0PLL:
        case bcmGdpllInputEventBS1PLL:
            idx = event_misc - bcmGdpllInputEventBS0PLL;
            READ_NS_REGr(unit, bs_pll_regs[idx], 0, &regVal);
            soc_reg_field_set(unit, bs_pll_regs[idx], &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, bs_pll_regs[idx], 0, regVal);
            break;

        case bcmGdpllInputEventLCPLL0:
        case bcmGdpllInputEventLCPLL1:
        case bcmGdpllInputEventLCPLL2:
        case bcmGdpllInputEventLCPLL3:
            idx = event_misc - bcmGdpllInputEventLCPLL0;
            READ_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, regVal);
            break;

        default:
           rv = BCM_E_PARAM;
           goto exit;
    }

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_divisor_misc_get(int unit, bcm_gdpll_input_event_t event_misc,
                            uint32 *pDivisor)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_divisor_port_set(int unit, bcm_port_t port,
    bcm_gdpll_port_event_t port_event_type, uint32 divisor)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;

    if (port > BCM_GDPLL_NUM_PORTS) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    idx = port;
    switch (port_event_type) {
        case bcmGdpllPortEventRXSOF:
        case bcmGdpllPortEventTXSOF:
        case bcmGdpllPortEventROE:
            rv = BCM_E_UNAVAIL;
            break;

        case bcmGdpllPortEventRXCDR:
            READ_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, idx, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, idx, regVal);
            break;

        case bcmGdpllPortEventTXPI:
            READ_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, idx, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, idx, regVal);
            break;

        default:
           rv = BCM_E_PARAM;
    }

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_divisor_port_get(int unit, bcm_port_t port,
                     bcm_gdpll_port_event_t port_event_type,
                     uint32 *pDivisor)
{
    int rv  = BCM_E_NONE;
    return (rv);
}

/* Mapping of input events to eventId will be device specific */
STATIC int
_bcm_esw_gdpll_input_eventId_get(int unit,
                    bcm_gdpll_input_event_t event_misc,
                    bcm_gdpll_port_event_t port_event_type,
                    bcm_port_t port,
                    uint32 *pEventId)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    if ((pEventId==NULL) || (pGdpllContext==NULL) ||
        (event_misc < bcmGdpllInputEventCpu) ||
        (event_misc > bcmGdpllInputEventPORT)) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Events 0 to 191 for port events, from 192 onwards for
     * miscellaneous events
     */
    switch (event_misc) {
        case bcmGdpllInputEventPORT:
            if (port < 0 || port > BCM_GDPLL_NUM_PORTS){
                rv = BCM_E_PARAM;
                goto exit;
            }

            if ((port_event_type == bcmGdpllPortEventRXCDR) ||
                (port_event_type == bcmGdpllPortEventRXSOF)){
                /* Event could be TXSOF or TXPI based on port type */
                idx = port*2;
            } else if (port_event_type == bcmGdpllPortEventROE) {
                idx = port*2 + 1;
            } else if ((port_event_type == bcmGdpllPortEventTXSOF) ||
                       (port_event_type == bcmGdpllPortEventTXPI)){
                /* Event could be TXSOF or TXPI based on port type */
                idx = BCM_GDPLL_IA_START_TXPI_TXSOF + port;
            } else {
                rv = BCM_E_PARAM;
                goto exit;
            }
            break;

        default:
            idx = BCM_GDPLL_IA_START_MISC + event_misc;
    }

    *pEventId = idx;

exit:
    return rv;
}

/* Get the event based on the eventId */
STATIC int
_bcm_esw_gdpll_input_event_get(int unit,
                    uint32 eventId,
                    bcm_gdpll_input_event_t *pEvent_misc,
                    bcm_gdpll_port_event_t *pPort_event_type,
                    bcm_port_t *pPort)
{
    int rv  = BCM_E_NONE;
    int idx = 0;

    if (eventId >= BCM_GDPLL_NUM_INPUT_EVENTS){
        rv = BCM_E_PARAM;
        goto exit;
    }

    if (eventId >= 192) {
        /* Miscellaneous events */
        *pEvent_misc = eventId - BCM_GDPLL_IA_START_MISC;

    } else if (eventId >= BCM_GDPLL_IA_START_TXPI_TXSOF) {
        /* Its a TXPI or TXSOF event based on the port */
        *pEvent_misc = bcmGdpllInputEventPORT;
        *pPort = eventId - BCM_GDPLL_IA_START_TXPI_TXSOF;

        *pPort_event_type = IS_CPRI_PORT(unit, *pPort) ? bcmGdpllPortEventTXSOF : bcmGdpllPortEventTXPI;

    } else {
        /* Its a TXPI or TXSOF event based on the port */
        *pEvent_misc = bcmGdpllInputEventPORT;
        *pPort = eventId/2;
        idx = eventId%2;

         if (IS_CPRI_PORT(unit, *pPort)) {
            *pPort_event_type = idx ? bcmGdpllPortEventROE : bcmGdpllPortEventRXSOF;
         } else {
            if (idx) {
                /* Unused event ID */
                rv = BCM_E_PARAM;
                goto exit;
            }
            *pPort_event_type = bcmGdpllPortEventRXCDR;
         }
    }

exit:
    return rv;
}

STATIC int
_bcm_esw_gdpll_input_eventId_inUse(int unit,
                    uint32 eventId, int *pIsUse)
{
    int rv  = BCM_E_NONE;
    int chan;
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    /* Check if this event is ref or fb for any of the active channels */
    for (chan=0; chan<BCM_GDPLL_NUM_CHANNELS; chan++) {
        if (pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_CHAN_ALLOC) {
            if ((pGdpllContext->dpll_chan[chan].eventId_ref == eventId) ||
                (pGdpllContext->dpll_chan[chan].eventId_fb == eventId)) {
                /* This event is already used by chan */
                *pIsUse = 1;
                goto exit;
            }
        }
    }

    *pIsUse = 0;

exit:
    return rv;
}

STATIC int
_bcm_esw_gdpll_input_array_alloc(int unit, uint32 eventId, uint32 *pIaAddr)
{
    int rv  = BCM_E_NONE;

    /*
     * Findout if this event is already under use by active channel
     * If not, findout the free ia address from the pool and allocate,
     *
     * Monterey have event id's less than the input array size.
     * Hence let the event id be the input array address for now
     * HW team recommends the eventId to be same as iaAddr
     */
    *pIaAddr = eventId;

    return rv;
}

STATIC int
_bcm_esw_gdpll_input_array_free(int unit, uint32 eventId)
{
    int rv  = BCM_E_NONE;
    /*
     * Figure out if this eventId is under use by any active channel
     * If not, free the input array address assocuated with this event
     */
    return rv;
}


STATIC int
_bcm_esw_gdpll_event_dest_misc_set(int unit, bcm_gdpll_input_event_t event_misc,
                            gdpll_event_dest_cfg_t *pEventDest,
                            uint32 eventId, uint32 *pIaAddr)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;
    uint32 dest;
    uint32 chkProfile;
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    if (event_misc < bcmGdpllInputEventCpu ||
        event_misc > bcmGdpllInputEventLCPLL3) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Check if the event is already enabled, that may indicate
     * that it will be used by some active GDPLL channel.
     * if so, return error
     */
    /* Check if this event is forwared to any of IA location */
    if ((pGdpllContext->eventId[eventId] >= 0) ||
        (pGdpllContext->eventId[eventId] < BCM_GDPLL_NUM_IA_ENTRIES)) {
        /* This event is already forwarded to IA location and could
         * be in use by an active GDPLL channel.
         * Hence it cant be forwarded
         */
        rv = BCM_E_BUSY;
        goto exit;
    }

    /* Get the free Input array address */
    BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_input_array_alloc(unit,
        eventId, pIaAddr));

    /* Since we have 150 numels of NS_TIMESYNC_TS_EVENT_FWD_CFG,
     * the miscellaneous events starts from index 128 per table 6.4 in uArch
     */
    idx = 128 + event_misc;

    if (pEventDest->event_dest == bcmGdpllEventDestCPU) {
        dest = 0;
    }else if (pEventDest->event_dest == bcmGdpllEventDestM7) {
        dest = 1;
    }else if (pEventDest->event_dest == bcmGdpllEventDestALL) {
        dest = 2;
    }else {
        rv = BCM_E_PARAM;
        goto exit;
    }

    
    chkProfile = 0;

    READ_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, &regVal);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              EVENT_IDf, eventId);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DESTf, dest);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DPLL_ADDRf, *pIaAddr);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              GDPLL_TSf, pEventDest->ts_counter ? 1:0);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              SRC_TS_COUNTERf, pEventDest->ts_counter ? 1:0);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              CHK_PROFILEf, chkProfile);
    WRITE_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, regVal);

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_dest_misc_get (int unit, bcm_gdpll_input_event_t  event_misc,
                             gdpll_event_dest_cfg_t *pEventDest)
{
    int rv  = BCM_E_NONE;
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_dest_port_set (int unit, bcm_port_t port,
                bcm_gdpll_port_event_t port_event_type,
                gdpll_event_dest_cfg_t *pEventDest,
                uint32 eventId, uint32 *pIaAddr)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;
    uint32 dest;
    uint32 chkProfile;
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    if (port >= BCM_GDPLL_NUM_PORTS) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Check if the event is already enabled, that may indicate
     * it will be used by some active GDPLL channel.
     * if so, return error
     */
    if ((pGdpllContext->eventId[eventId] >= 0) ||
        (pGdpllContext->eventId[eventId] < BCM_GDPLL_NUM_IA_ENTRIES)) {
        /* This event is already forwarded IA location and could be in use
         * by an active GDPLL channel. Hence it cant be forwarded
         */
        rv = BCM_E_BUSY;
        goto exit;
    }

    /* Get the free Input array address */
    BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_input_array_alloc(unit,
        eventId, pIaAddr));

    if (pEventDest->event_dest == bcmGdpllEventDestCPU) {
        dest = 0;
    }else if (pEventDest->event_dest == bcmGdpllEventDestM7) {
        dest = 1;
    }else if (pEventDest->event_dest == bcmGdpllEventDestALL) {
        dest = 2;
    }else {
        rv = BCM_E_PARAM;
        goto exit;
    }

    
    chkProfile = 0;

    switch(port_event_type) {
        case bcmGdpllPortEventRXCDR:
        case bcmGdpllPortEventTXPI:
            if (port_event_type == bcmGdpllPortEventRXCDR){
                idx = port;
            }else if (port_event_type == bcmGdpllPortEventTXPI) {
                idx = port + BCM_GDPLL_NUM_PORTS;
            }

            READ_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              EVENT_IDf, eventId);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DESTf, dest);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DPLL_ADDRf, *pIaAddr);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              GDPLL_TSf, pEventDest->ts_counter ? 1:0);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              SRC_TS_COUNTERf, pEventDest->ts_counter ? 1:0);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              CHK_PROFILEf, chkProfile);
            WRITE_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, regVal);

            break;

        case bcmGdpllPortEventRXSOF:    /* Type-0 */
        case bcmGdpllPortEventTXSOF:    /* Type-1 */
        case bcmGdpllPortEventROE:      /* Type-2 */
            /*
             * Having 128 Numels, register map is
             * Port-0: type-0, type-1, type-2, unused
             * Port-1: type-0, type-1, type-2, unused
             */
            if ((port >= BCM_GDPLL_NUM_CPRI_PORTS) ||
                (IS_CPRI_PORT(unit, port) == 0)) {
                rv = BCM_E_PARAM;
                goto exit;
            }

            idx = port*4;
            if (port_event_type == bcmGdpllPortEventTXSOF) {
                idx = idx + 1;
            }else if (port_event_type == bcmGdpllPortEventROE) {
                idx = idx + 2;
            }

            READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, idx, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              EVENT_IDf, eventId);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              DESTf, dest);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              DPLL_ADDRf, *pIaAddr);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              SRC_TS_COUNTERf, 1);  /* It is TS1 counter */
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              CHK_PROFILEf, chkProfile);
            WRITE_NS_REGr(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, idx, regVal);
            break;

        default:
            rv = BCM_E_PARAM;
            goto exit;
    }

    /* eventId got set the forwarding to iaAddr */
    pGdpllContext->eventId[eventId] = *pIaAddr;

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_dest_port_get(int unit, bcm_port_t port, bcm_gdpll_port_event_t port_event_type,
                             gdpll_event_dest_cfg_t  *pEventDest)
{
    int rv = BCM_E_NONE;
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_enable_misc_set(int unit, bcm_gdpll_input_event_t  event_misc,
                               int enable)
{
    int rv = BCM_E_NONE;
    uint32 idx = 0;
    uint32 enable_field = 0;
    uint32 regVal = 0;
    uint32 reg_enable;

    /*
    NS_MISC_CLK_EVENT_CTRL
    NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRL
    NS_BROADSYNC0_CLK_EVENT_CTRL
    NS_BROADSYNC1_CLK_EVENT_CTRL
    NS_TIMESYNC_GPIO_x_CTRL
     */
    switch (event_misc){
        case bcmGdpllInputEventCpu:
            if (event_misc == bcmGdpllInputEventCpu) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_CPU;
            }
        case bcmGdpllInputEventBS0HB:
            if (event_misc == bcmGdpllInputEventBS0HB) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS0HB;
            }
        case bcmGdpllInputEventBS1HB:
            if (event_misc == bcmGdpllInputEventBS1HB) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS1HB;
            }
        case bcmGdpllInputEventIPDM0:
            if (event_misc == bcmGdpllInputEventIPDM0) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_IPDM0;
            }
        case bcmGdpllInputEventIPDM1:
            if (event_misc == bcmGdpllInputEventIPDM1) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_IPDM1;
            }
        case bcmGdpllInputEventTS1TS:
            if (event_misc == bcmGdpllInputEventTS1TS) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_TS1;
            }
        case bcmGdpllInputEventRP1IF:
            if (event_misc == bcmGdpllInputEventRP1IF) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_RP1IF;
            }
        case bcmGdpllInputEventRP1RF:
            if (event_misc == bcmGdpllInputEventRP1RF) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_RP1RF;
            }
        case bcmGdpllInputEventBS0ConvTC:
            if (event_misc == bcmGdpllInputEventBS0ConvTC) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS0CONV;
            }
        case bcmGdpllInputEventBS1ConvTC:
            if (event_misc == bcmGdpllInputEventBS1ConvTC) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS1CONV;
            }

            /* Write the enable flag */
            READ_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, &regVal);
            reg_enable = soc_reg_field_get(unit, NS_MISC_CLK_EVENT_CTRLr, regVal, ENABLEf);

            reg_enable = enable ? (reg_enable | enable_field) : (reg_enable & (~enable_field));

            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regVal,
                              ENABLEf, reg_enable);
            WRITE_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, regVal);
            break;

        case bcmGdpllInputEventGPIO0:
        case bcmGdpllInputEventGPIO1:
        case bcmGdpllInputEventGPIO2:
        case bcmGdpllInputEventGPIO3:
        case bcmGdpllInputEventGPIO4:
        case bcmGdpllInputEventGPIO5:
            idx = event_misc - bcmGdpllInputEventGPIO0;
            READ_NS_REGr(unit, gpio_regs[idx], 0, &regVal);
            soc_reg_field_set(unit,  gpio_regs[idx], &regVal,
                              CAPTURE_ENABLEf, enable ? 1:0);
            WRITE_NS_REGr(unit, gpio_regs[idx], 0, regVal);
            break;

        case bcmGdpllInputEventBS0PLL:
        case bcmGdpllInputEventBS1PLL:
            idx = event_misc - bcmGdpllInputEventBS0PLL;
            READ_NS_REGr(unit, bs_pll_regs[idx], 0, &regVal);
            soc_reg_field_set(unit, bs_pll_regs[idx], &regVal,
                              ENABLEf, enable ? 1:0);
            WRITE_NS_REGr(unit, bs_pll_regs[idx], 0, regVal);
            break;

        case bcmGdpllInputEventLCPLL0:
        case bcmGdpllInputEventLCPLL1:
        case bcmGdpllInputEventLCPLL2:
        case bcmGdpllInputEventLCPLL3:
            idx = event_misc - bcmGdpllInputEventLCPLL0;
            READ_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, &regVal,
                              ENABLEf, enable ? 1:0);
            WRITE_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, regVal);
            break;

        case bcmGdpllInputEventPORT:
        default: 
            rv = BCM_E_PARAM;
            goto exit;
    }

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_enable_misc_get(int unit, bcm_gdpll_input_event_t  event_misc,
                              int *pEnable)
{
    int rv = BCM_E_NONE;
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_enable_port_set(int unit, bcm_port_t port, bcm_gdpll_port_event_t port_event_type,
                                 int enable)
{
    int rv = BCM_E_NONE;
    uint32 mapper_reg;
    uint32 regVal = 0;
    uint32 enable_field = 0;
    uint32 reg_enable = 0;

    if (port >= BCM_GDPLL_NUM_PORTS) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    switch (port_event_type) {
        case bcmGdpllPortEventRXSOF:
        case bcmGdpllPortEventTXSOF:
        case bcmGdpllPortEventROE:
            /* GDPLL has support to 32 CPRI ports */
            if ((port >= BCM_GDPLL_NUM_CPRI_PORTS) ||
                (IS_CPRI_PORT(unit, port) == 0)){
                rv = BCM_E_PARAM;
                goto exit;
            }

            /* Program NS_TIMESYNC_MAPPER_PORT_ENABLE_x */
            enable_field = 1 << (port_event_type - bcmGdpllPortEventRXSOF);
            mapper_reg = port/8;

            READ_NS_REGr(unit, mapper_port_enable_regs[mapper_reg], 0, &regVal);
            reg_enable = soc_reg_field_get(unit, mapper_port_enable_regs[mapper_reg], regVal, mapper_port_enable[port]);

            reg_enable = enable ? (reg_enable | enable_field) : (reg_enable & (~enable_field));

            soc_reg_field_set(unit, mapper_port_enable_regs[mapper_reg], &regVal, mapper_port_enable[port], reg_enable);
            WRITE_NS_REGr(unit, mapper_port_enable_regs[mapper_reg], 0, regVal);

            break;

        case bcmGdpllPortEventRXCDR:
            /* Program NS_TIMESYNC_RX_CDR_EVENT_CTRL */
            READ_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, port, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, &regVal, ENABLEf, enable ? 1:0);
            WRITE_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, port, regVal);
            break;

        case bcmGdpllPortEventTXPI:
            /* Program NS_TIMESYNC_TX_PI_CLK_EVENT_CTRL */
            READ_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, port, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, &regVal, ENABLEf, enable ? 1:0);
            WRITE_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, port, regVal);
            break;

        default:
            rv = BCM_E_PARAM;
          goto exit;
    }

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_enable_port_get(int unit, bcm_port_t port, bcm_gdpll_port_event_t port_event_type,
                              int *pEnable)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_roe_52b_set(int unit, bcm_port_t port, int enable)
{
    int rv = BCM_E_NONE;
    uint32 enable_field = 0;
    uint32 regVal = 0;
    uint32 reg_enable = 0;

    /* Program NS_TIMESYNC_MAPPER_TYPE2_FORMAT */

    enable_field = 1 << port;

    READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, port, &regVal);
    reg_enable = soc_reg_field_get(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, regVal, CONV_ENf);

    reg_enable = enable ? (reg_enable | enable_field) : (reg_enable & (~enable_field));

    soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, &regVal, CONV_ENf, reg_enable);
    WRITE_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, port, regVal);

    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_roe_52b_get(int unit, bcm_port_t port, int *pEnable)
{
    int rv = BCM_E_NONE;
    /* NS_TIMESYNC_MAPPER_TYPE2_FORMAT */

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_update_set(int unit, int chan)
{
    /* Update NS_GDPLL_IA_UPDATE_CONFIG */
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx;
    uint32 eventId_fb;
    uint32 eventId_ref;

    if (chan >= BCM_GDPLL_NUM_CHANNELS) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    eventId_fb = pGdpllContext->dpll_chan[chan].eventId_fb;
    eventId_ref = pGdpllContext->dpll_chan[chan].eventId_ref;

    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal);

    /* Set the feedback for the channel */
    if ((eventId_fb < BCM_GDPLL_NUM_INPUT_EVENTS) &&
        (pGdpllContext->eventId[eventId_fb] < BCM_GDPLL_NUM_IA_ENTRIES)){
        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, FEEDBACK_IDf,
                    pGdpllContext->eventId[eventId_fb]);
    } else {
        rv = BCM_E_NOT_FOUND;
        goto exit;
    }

    /* Set the reference for the channel */
    if (pGdpllContext->dpll_chan[chan].phaseConterRef == 0){
        if ((eventId_ref < BCM_GDPLL_NUM_INPUT_EVENTS) &&
            (pGdpllContext->eventId[eventId_ref] < BCM_GDPLL_NUM_IA_ENTRIES)){
            soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, REFERENCE_IDf,
                    pGdpllContext->eventId[eventId_ref]);
        } else {
            rv = BCM_E_NOT_FOUND;
            goto exit;
        }
    }

    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, regVal);

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_update_get(int unit, int chan,
                           bcm_gdpll_chan_t *pGdpllChan)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_enable_set(int unit, int chan, int enable)
{
   /* NS_GDPLL_IA_UPDATE_CONFIG */
    int rv = BCM_E_NONE;
    uint32 regVal = 0;

    if (chan >= BCM_GDPLL_NUM_CHANNELS) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ENABLEf, enable ? 1:0);
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, regVal);

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_enable_get(int unit, int chan, int *pEnable)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_capture_enable_m7_set(int unit, int enable)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    /* NS_MISC_CLK_EVENT_CTRL */

    READ_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, &regVal);
    soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regVal, GDPLL_CAPTURE_ENABLEf, enable ? 1:0);
    WRITE_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, regVal);

    return (rv);
}

STATIC int
_bcm_esw_gdpll_capture_enable_m7_get(int unit, int *pEnable)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_capture_enable_cpu_set(int unit, int enable)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;

    READ_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, &regVal);
    soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regVal, TIME_CAPTURE_ENABLEf, enable ? 1:0);
    WRITE_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, regVal);

    return (rv);
}

STATIC int
_bcm_esw_gdpll_capture_enable_cpu_get(int unit, int *pEnable)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_flush(int unit)
{
    int rv = BCM_E_NONE;
    /* NS_GDPLL_GDPLL_COMMON_CTRL */

    WRITE_NS_REGr(unit, NS_GDPLL_GDPLL_COMMON_CTRLr, 0, 1);
    sal_usleep(100);
    WRITE_NS_REGr(unit, NS_GDPLL_GDPLL_COMMON_CTRLr, 0, 0);

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_out_txpi_set(int unit, int chan, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;

    if ((chan >= BCM_GDPLL_NUM_CHANNELS) &&
        (port >= BCM_GDPLL_NUM_PORTS)) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* NS_GDPLL_NCO_UPDATE_CONTROL */
    READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, chan, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_IDf, port);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_TYPEf, 1);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_TYPEf, 0);
    WRITE_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, chan, regVal);

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_out_txpi_get(int unit, int chan, bcm_port_t *pPort)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_out_misc_set(int unit, int chan,
                        bcm_gdpll_output_event_t event)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    uint32 dest_hw_id = 0;
    uint32 dest_hw_type = 0;

    if (chan >= BCM_GDPLL_NUM_CHANNELS) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    switch (event) {
        case bcmGdpllOutputEventBSPLL0:
        case bcmGdpllOutputEventBSPLL1:
        case bcmGdpllOutputEventLCPLL0:
        case bcmGdpllOutputEventLCPLL1:
        case bcmGdpllOutputEventLCPLL2:
        case bcmGdpllOutputEventLCPLL3:
            dest_hw_id = BCM_GDPLL_NUM_PORTS +
                    (event - bcmGdpllOutputEventBSPLL0);
            dest_hw_type = 0;
            break;

        case bcmGdpllOutputEventTS0:
        case bcmGdpllOutputEventTS1:
            dest_hw_id = event - bcmGdpllOutputEventTS0;
            dest_hw_type = 1;

        default:
            rv = BCM_E_PARAM;
            goto exit;
    }

    READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, chan, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_IDf, dest_hw_id);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_TYPEf, 1);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_TYPEf, dest_hw_type);
    WRITE_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, chan, regVal);

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_out_misc_get(int unit, int chan,
                        bcm_gdpll_output_event_t *pOutEvent)
{
    int rv = BCM_E_NONE;

    return (rv);
}

#if 0  
STATIC int
_bcm_esw_gdpll_chan_out_enable_set(int unit, int chan, int enable)
{
    int rv = BCM_E_NONE;
    /* NS_GDPLL_NCO_UPDATE_COMMON_CONTROL */

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_out_enable_get(int unit, int chan,
                               int *pEnable)
{
    int rv = BCM_E_NONE;

    return (rv);
}
#endif

STATIC int
soc_iproc_m7_write(int unit, uint32 addr, uint32 data)
{
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        soc_cm_iproc_write(unit, addr, data);
        return SOC_E_NONE;
    }
    return SOC_E_NONE;
}

STATIC int
soc_iproc_m7_read(int unit, uint32 addr)
{
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        return soc_cm_iproc_read(unit, addr);
    }
    return SOC_E_NONE;
}

STATIC int
_bcm_esw_gdpll_chan_config_dpll_set(int unit, int chan,
                        dpll_param_t *pDpllParam)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    uint32 count;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 *pData = (uint32 *)pDpllParam;

    if ((chan >= BCM_GDPLL_NUM_CHANNELS) ||
        (pDpllParam == NULL)) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Info - Copy to ITCM location for the given channel here
     * Verify that M7 also reads with proper offsets
     * Every field can also be accessed using
     * SAL_OFFSETOF(mos_msg_area_t, data[0].words[0])
     */
    for (count = 0; count < DPLL_PARAM_SIZE;) {
        soc_iproc_m7_write(unit, dpllAddr, *pData);
        pData++;
        count += sizeof(uint32);
        dpllAddr += sizeof(uint32);
    }

    /* Update the channel for phase counter reference */
    if (pDpllParam->dpll_config.phase_counter_ref == 1) {
        READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, PHASECOUNTERREFf, 1);
        WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, regVal);
    }

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_config_dpll_get(int unit, int chan,
                        bcm_gdpll_chan_dpll_config_t *pDpllConfig)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_debug_enable_set(int unit, int enable)
{
    /* This API may not be required as the debugging is
     * always enabled during the init API
     */
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_debug_mode_set(int unit, int chan,
            bcm_gdpll_debug_mode_t debugMode)
{
    int rv = BCM_E_NONE;
    uint32 dpll_debugMode;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);


    /* Info - debugEnable debugMode addresses visible by M7 */
    dpll_debugMode = dpllAddr + SAL_OFFSETOF(dpll_param_t, debugMode);

    /* M7 ITCM configuration, for the desired debug mode */
    soc_iproc_m7_write(unit, dpll_debugMode, debugMode);

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_debug_mode_get(int unit, int chan,
            bcm_gdpll_debug_mode_t *pDebugMode)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_deinit(int unit)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    /* Destroy protection mutex. */
    if (NULL != pGdpllContext->mutex) {
        sal_mutex_destroy(pGdpllContext->mutex);
        pGdpllContext->mutex = NULL;
    }

    return (rv);
}

void
gdpll_debug_buffer_send_thread(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    if (pGdpllContext == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "gdpll_debug_buffer_send_thread: context not setup\n")));
          return;

    }

    /* Info
     * Wait on the signal from the interrupt routine
     * Check the buffer which is filled and call the user callback
     */
    while(sal_sem_take(pGdpllContext->debug_sem, sal_sem_FOREVER)) {
        if (pGdpllContext->debug_cb) {
            /* Return of this cb confirms the buffer consumption */
            pGdpllContext->debug_cb(unit, pGdpllContext->pUserData,
                    pGdpllContext->pSwDebugBuff[pGdpllContext->debugRdPtr],
                    BCM_GDPLL_DEBUG_SW_SIZE);
        }
        pGdpllContext->debugFillCnt--;

        if (++pGdpllContext->debugRdPtr >= GDPLL_NUM_DEBUG_BUFFERS) {
            pGdpllContext->debugRdPtr = 0;
        }
    }
}

void
gdpll_debug_buffer_intr(int unit, void *data)
{
    

    /* Info
     * Read the RD_PTR, WR_PTR
     * Initiate the DMA tranfer or CPU-read to local buffer
     * Signal the thread that is waiting on the debug data
     *
     * call the user callback passing the read buffer
     * Re-enable the debug buffer interrupt
     *
     * NS_GDPLL_DEBUG_M7DG_THRESHOLD    - Threshold set
     * NS_GDPLL_DEBUG_M7DG_ENABLE       - Enable debug
     * NS_GDPLL_DEBUG_M7DG_RSTATE       - RDPTR
     * NS_GDPLL_DEBUG_M7DG_BSTATE       - OCCUPANCY 14 bits for debug
     *                                    EMPTY     1 bit for debug
     *                                    WDPTR     14 bits
     * NS_GDPLL_DEBUG_M7DG_INT_STATUS   - THRESHOLD_REACHED
     *                                    OVERFLOW  1-bit for full condition
     *                                    ECC_ERR_MEM0
     *                                    ECC_ERR_MEM1
     * NS_GDPLL_DEBUG_M7DG_INT_ENABLE   - INTEN 4-bits for above
     */

    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    uint32 thrVal = 0;  /* Threshold size   */
    uint32 rdptr = 0;   /* Read pointer     */
    uint32 int_status = 0;  /* Interrupt status */
    uint32 int_enable = 0;  /* Interrupt enable */
    uint32  dmaCount = 0;
#ifdef PTRS_ARE_64BITS
    uint32 *pGdpllSrcBuff = (unsigned int *)((uint64)BCM_GDPLL_DEBUG_BUFFER_START);
#else
    uint32 *pGdpllSrcBuff = (unsigned int *)(BCM_GDPLL_DEBUG_BUFFER_START);
#endif
    uint32  *pGdpllDstBuff = NULL;
    gdpll_context_t *pGdpllContext = pGdpllCtx;
    int dmaFlags;
    int dmaChan = 0;

    /* Read the GDPLL interrupt status */
    READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_INT_STATUSr, 0, &int_status);

    /* Process threshold interrupt */
    if (soc_reg_field_get(unit, NS_GDPLL_DEBUG_M7DG_INT_STATUSr, int_status, THRESHOLD_REACHEDf)) {
        int_enable |= BCM_GDPLL_DEBUG_INT_THR;

        /* Read the threshold value set */
        READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_THRESHOLDr, 0, &thrVal);

        /* Read the Rd-pointer */
        READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_RSTATEr, 0, &rdptr);

        /*
         * DMA Flags 1 -> 32 bit endian swap
         *           2 -> read from PCI (i.e, Manual copy )
         *  other(Ex: 4, 0) -> write to PCI
         */
        dmaFlags = 0x2;

        if (pGdpllContext->debugFillCnt < GDPLL_NUM_DEBUG_BUFFERS) {
            /* Copy only if there are free buffers available */
            if ((rdptr + thrVal) < BCM_GDPLL_DEBUG_BUFFER_SIZE) {
                /* Initiate the transfer from "rdptr" to "rdptr + thrVal" */
                pGdpllSrcBuff += rdptr;
                pGdpllDstBuff = pGdpllContext->pSwDebugBuff[pGdpllContext->debugWrPtr];
                dmaCount = thrVal;

                rv = soc_ccmdma_copy(unit, pGdpllSrcBuff, pGdpllDstBuff, 1, 0, dmaCount, dmaFlags, dmaChan);
                if (rv != SOC_E_NONE) {
                    LOG_ERROR(BSL_LS_SOC_SCHANFIFO,
                              (BSL_META_U(unit,
                                "CCM DMA failed with error 0x%x\n"), rv));
                    /* Info: Manual copy required ? */
                }

                rdptr = rdptr + thrVal;
            } else {
                /*
                 * - Info - Need two DMA transactions here, back to back
                 *   1. rdptr to "BCM_GDPLL_DEBUG_BUFFER_SIZE - 1"
                 *   2. 0 to thrVal - (BCM_GDPLL_DEBUG_BUFFER_SIZE - rdptr)
                 */
                pGdpllSrcBuff += rdptr;
                pGdpllDstBuff = pGdpllContext->pSwDebugBuff[pGdpllContext->debugWrPtr];
                dmaCount = BCM_GDPLL_DEBUG_BUFFER_SIZE - rdptr;

                rv = soc_ccmdma_copy(unit, pGdpllSrcBuff, pGdpllDstBuff, 1, 0, dmaCount, dmaFlags, dmaChan);
                if (rv != SOC_E_NONE) {
                    LOG_ERROR(BSL_LS_SOC_SCHANFIFO,
                              (BSL_META_U(unit,
                                "CCM DMA failed with error 0x%x\n"), rv));
                    /* Info: Manual copy ? */
                }

                pGdpllDstBuff += dmaCount;
                dmaCount = thrVal - dmaCount;
#ifdef PTRS_ARE_64BITS
                pGdpllSrcBuff = (unsigned int *)((uint64)BCM_GDPLL_DEBUG_BUFFER_START);
#else
                pGdpllSrcBuff = (unsigned int *)(BCM_GDPLL_DEBUG_BUFFER_START);
#endif
                rv = soc_ccmdma_copy(unit, pGdpllSrcBuff, pGdpllDstBuff, 1, 0, dmaCount, dmaFlags, dmaChan);
                if (rv != SOC_E_NONE) {
                    LOG_ERROR(BSL_LS_SOC_SCHANFIFO,
                              (BSL_META_U(unit,
                                "CCM DMA failed with error 0x%x\n"), rv));
                    /* Info: Manual copy ? */
                }

                rdptr = dmaCount;
            }
            pGdpllContext->debugFillCnt++;

            if(++pGdpllContext->debugWrPtr >= GDPLL_NUM_DEBUG_BUFFERS) {
                pGdpllContext->debugWrPtr = 0;
            }

            /* Update the HW read pointer */
            if (rdptr >= BCM_GDPLL_DEBUG_BUFFER_SIZE) {
                rdptr = 0;
            }

            soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_RSTATEr, &regVal, RDPTRf, rdptr);
            WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_RSTATEr, 0, regVal);

            /* Signal the thread for buffer read */
            if (pGdpllContext->debug_sem) {
                sal_sem_give(pGdpllContext->debug_sem);
            }
        } else {
            /* Info - The debug containt is still not drained out !!! */
        }
    }

    if (soc_reg_field_get(unit, NS_GDPLL_DEBUG_M7DG_INT_STATUSr, int_status, OVERFLOWf)) {
        int_enable |= BCM_GDPLL_DEBUG_INT_OF;
    }

    if (soc_reg_field_get(unit, NS_GDPLL_DEBUG_M7DG_INT_STATUSr, int_status, ECC_ERR_MEM0f)) {
        int_enable |= BCM_GDPLL_DEBUG_INT_ECC0;
    }

    if (soc_reg_field_get(unit, NS_GDPLL_DEBUG_M7DG_INT_STATUSr, int_status, ECC_ERR_MEM1f)) {
        int_enable |= BCM_GDPLL_DEBUG_INT_ECC1;
    }

    /* Clear the interrupt status, which is W1TC */
    WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_INT_STATUSr, 0, int_status);

    /* Re-enable the interrupts */
    READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_INT_ENABLEr, 0, &regVal);
    int_enable |= soc_reg_field_get(unit, NS_GDPLL_DEBUG_M7DG_INT_ENABLEr, regVal, INTENf);
    soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_INT_ENABLEr, &regVal, INTENf, int_enable);
    WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_INT_ENABLEr, 0, int_enable);
}

static void
clear_ext_memory(unsigned char* start_addr, unsigned int num_bytes)
{
    unsigned int i;

    for (i = 0; i < num_bytes; i++) {
        start_addr[i] = 0x0;
    }
}

STATIC int
_bcm_esw_gdpll_init(int unit)
{
    
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    uint32 alloc_sz = 0;
    uint32 count = 0;
    gdpll_context_t *pGdpllContext = NULL;
    sal_thread_t thread_id_debug;

    /* Allocate time config structure. */
    alloc_sz = sizeof(gdpll_context_t);
    pGdpllContext = sal_alloc(alloc_sz, "GDPLL module");
    if (NULL == pGdpllContext) {
        rv = BCM_E_MEMORY;
        goto exit;
    }
    sal_memset(pGdpllContext, 0, alloc_sz);

    /* Create protection mutex. */
    pGdpllContext->mutex = NULL;
    pGdpllContext->mutex = sal_mutex_create("GDPLL mutex");
    if (NULL == pGdpllContext->mutex) {
        _bcm_esw_gdpll_deinit(unit);
        return (BCM_E_MEMORY);
    }

    pGdpllContext->debug_cb = NULL;

    

    /* Configure debug threshold level */
    READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_THRESHOLDr, 0, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_THRESHOLDr, &regVal, THRESHOLDf, BCM_GDPLL_DEBUG_THRESHOLD);
    WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_THRESHOLDr, 0, regVal);

    /* Enable the GDPLL debug feature */
    READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, 0, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, &regVal, ENABLEf, 1);
    WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, 0, regVal);

#ifdef GDPLL_DEBUGBUFF_LOCAL_COPY
    for (count = 0; count < GDPLL_NUM_DEBUG_BUFFERS; count++) {
        pGdpllContext->pSwDebugBuff[count] = sal_dma_alloc(BCM_GDPLL_DEBUG_SW_SIZE*
                                sizeof(unsigned int), "Host_Buffer");
        clear_ext_memory((unsigned char*)pGdpllContext->pSwDebugBuff[count],
                                BCM_GDPLL_DEBUG_SW_SIZE * sizeof(uint32));

        /* Clear the read and write pointers */
        pGdpllContext->debugWrPtr = 0;
        pGdpllContext->debugRdPtr = 0;
    }
#endif
    pGdpllCtx = pGdpllContext;

    /* Create the debug buffer semaphore */
    pGdpllContext->debug_sem = sal_sem_create("gdpll_debug_buff", sal_sem_COUNTING, 0);
    if (pGdpllContext->debug_sem == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "_bcm_esw_gdpll_init: failed to create the sem\n")));
          return (SOC_E_MEMORY);
    }

    /* Create the debug thread */
    thread_id_debug = sal_thread_create("GDPLL Debug thread", SAL_THREAD_STKSZ, 100,
                    gdpll_debug_buffer_send_thread, INT_TO_PTR(unit));
    if (thread_id_debug == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "GDPLL debug thread create failed\n")));
        return SOC_E_INTERNAL;
    }

    /* Enable the GDPLL interrupts */
    WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_INT_ENABLEr, 0, BCM_GDPLL_DEBUG_INT);

    
exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_alloc(int unit, int *pChan)
{
    int rv = BCM_E_NONE;
    uint32 idx = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    if (pGdpllContext == NULL) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    for (idx = 0; idx < BCM_GDPLL_NUM_CHANNELS; idx++) {
        if (!(pGdpllContext->dpll_chan[idx].flag & BCM_GDPLL_CHAN_ALLOC)) {
            *pChan = idx;
            goto exit;
        }
    }

    rv = BCM_E_FULL;

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_free(int unit, int chan)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    if ((pGdpllContext == NULL) ||
        (chan >= BCM_GDPLL_NUM_CHANNELS)){
        rv = BCM_E_PARAM;
        goto exit;
    }

    pGdpllContext->dpll_chan[chan].flag &= ~BCM_GDPLL_CHAN_ALLOC;

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_config_set(int unit, bcm_gdpll_chan_input_t *pChanInput, uint32 eventId, uint32 *pIaAddr)
{
    int rv = BCM_E_NONE;
    gdpll_event_dest_cfg_t eventDest;

    /* Configure the event */
    eventDest.event_dest = pChanInput->event_dest;
    eventDest.ts_counter = pChanInput->ts_counter;
    eventDest.ppm_check_enable = pChanInput->ppm_check_enable;

    if ((pChanInput->input_event >= bcmGdpllInputEventCpu) ||
        (pChanInput->input_event <= bcmGdpllInputEventLCPLL3)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_event_divisor_misc_set(unit,
                            pChanInput->input_event,
                            pChanInput->event_divisor));
        BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_event_dest_misc_set(unit,
                            pChanInput->input_event,
                            &eventDest, eventId, pIaAddr));

    } else if(pChanInput->input_event == bcmGdpllInputEventPORT) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_event_divisor_port_set(unit,
                            pChanInput->port, pChanInput->port_event_type,
                            pChanInput->event_divisor));
        BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_event_dest_port_set(unit, pChanInput->port,
                            pChanInput->port_event_type,
                            &eventDest, eventId, pIaAddr));

        if (pChanInput->port_event_type == bcmGdpllPortEventROE)
            BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_event_roe_52b_set(unit,
                            pChanInput->port, 1));
    } else {
        rv = BCM_E_PARAM;
    }

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_priority_set(int unit, int chan, bcm_gdpll_chan_priority_t chan_prio)
{
    /* NS_GDPLL_IA_UPDATE_CONFIG */
    int rv = BCM_E_NONE;
    uint32 regVal = 0;

    if (chan >= BCM_GDPLL_NUM_CHANNELS) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, SPEED_BINf, chan_prio);
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, regVal);

exit:
    return (rv);
}

/****************************************************************************/
/*                      API functions implementation                        */
/****************************************************************************/

/*
 * Function:
 *      bcm_esw_gdpll_chan_enable
 * Purpose:
 *      Channel enable
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      chan - (IN) Channel Number
 *      enable   - (IN) Enable flag
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_esw_gdpll_chan_enable(int unit, int chan, int enable)
{
    int rv = BCM_E_NONE;
    bcm_gdpll_input_event_t event_misc;
    bcm_gdpll_port_event_t port_event_type;
    bcm_port_t port;
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    if((chan >= BCM_GDPLL_NUM_CHANNELS) ||
       (pGdpllContext == NULL)) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Channel is not allocated and feedbak is configured */
    if (!(pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_CHAN_ALLOC) ||
        !(pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_EVENT_CONFIG_FB)) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    /* Check if phaseCounterRef and event-ref is not configured */
    if (!pGdpllContext->dpll_chan[chan].phaseConterRef &&
        !(pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_EVENT_CONFIG_REF)) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* enable=1, Enable the events in sequence */
    if (enable) {
        /* set enable for feedback event */
        _bcm_esw_gdpll_input_event_get(unit, pGdpllContext->dpll_chan[chan].eventId_fb,
                    &event_misc, &port_event_type, &port);
        if (event_misc == bcmGdpllInputEventPORT) {
            _bcm_esw_gdpll_event_enable_port_set(unit, port, port_event_type, enable);
        } else {
            _bcm_esw_gdpll_event_enable_misc_set(unit, event_misc, enable);
        }

        /* set enable for reference event */
        if (!pGdpllContext->dpll_chan[chan].phaseConterRef) {
            if (!(pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_EVENT_CONFIG_REF)) {
                /* Phase reference is not set, return error */
                rv = BCM_E_PARAM;
                goto exit;
            }

            _bcm_esw_gdpll_input_event_get(unit, pGdpllContext->dpll_chan[chan].eventId_ref,
                        &event_misc, &port_event_type, &port);
            if (event_misc == bcmGdpllInputEventPORT) {
                _bcm_esw_gdpll_event_enable_port_set(unit, port, port_event_type, enable);
            } else {
                _bcm_esw_gdpll_event_enable_misc_set(unit, event_misc, enable);
            }
        }

        /* Enable GDPLL input array capture */
        _bcm_esw_gdpll_capture_enable_m7_set(unit, enable);
    }

    /* Enable the GDPLL channel */
    _bcm_esw_gdpll_chan_enable_set(unit, chan, enable);

exit:
    sal_mutex_give(pGdpllContext->mutex);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_chan_create
 * Purpose:
 *      Initialize time module
 * Parameters:
 *      unit  - (IN) StrataSwitch Unit #.
 *      flags - (IN) Flags for channel create
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_esw_gdpll_chan_create(int unit, uint32 flags,
                          bcm_gdpll_chan_t *pGdpllChan, int *pChan)
{
    int rv = BCM_E_NONE;
    uint32 iaAddr_fb    = BCM_GDPLL_NUM_IA_ENTRIES;   /* Indicating free */
    uint32 iaAddr_ref   = BCM_GDPLL_NUM_IA_ENTRIES;   /* Indicating free */
    uint32 eventId_fb   = BCM_GDPLL_NUM_INPUT_EVENTS; /* Indicating invalid event */
    uint32 eventId_ref  = BCM_GDPLL_NUM_INPUT_EVENTS; /* Indicating invalid event */
    int phaseCounterRef = 0;

    int chan;
    int inUse = 0;
    dpll_param_t   dpllParam;
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    if ((pChan == NULL) || (pGdpllChan == NULL) ||
        (pGdpllContext == NULL)) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Lock the channel access */
    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    /* Get the free channel
     * If the channel is allocated earlier, then pChan carries the
     * allocated channel number, otherwise flag should need to be
     * with BCM_GDPLL_CHAN_ALLOC
     */
    if(flags & BCM_GDPLL_CHAN_ALLOC) {
        /* Alloc new channel */
        if(_bcm_esw_gdpll_chan_alloc(unit, &chan) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        *pChan = chan;
    } else if((*pChan < BCM_GDPLL_NUM_CHANNELS) &&
              (pGdpllContext->dpll_chan[*pChan].flag & BCM_GDPLL_CHAN_ALLOC)){
        /* Channel is already allocated earlier and channel number
         * passed by the user
         */
        chan = *pChan;
    } else {
        /* Channel is not allocated earlier either */
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Feedback event forward configuration */
    if(flags & BCM_GDPLL_EVENT_CONFIG_FB) {
        if(_bcm_esw_gdpll_input_eventId_get(unit,
                    pGdpllChan->event_fb.input_event,
                    pGdpllChan->event_fb.port,
                    pGdpllChan->event_fb.port_event_type,
                    &eventId_fb) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }

        /* Check if the event is already under use */
        if(_bcm_esw_gdpll_input_eventId_inUse(unit,
                    eventId_fb, &inUse) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        if (inUse) {
            /* Event is under use by other channel */
            rv = BCM_E_PARAM;
            goto exit;
        }

        /* Configure the input event and get iaAddr_fb */
        if(_bcm_esw_gdpll_event_config_set(unit, &(pGdpllChan->event_fb), eventId_fb, &iaAddr_fb) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        pGdpllContext->eventId[eventId_fb] = iaAddr_fb;
    }

    /* Reference event forward configuration */
    if(flags & BCM_GDPLL_EVENT_CONFIG_REF) {
        if (_bcm_esw_gdpll_input_eventId_get(unit,
                    pGdpllChan->event_ref.input_event,
                    pGdpllChan->event_ref.port,
                    pGdpllChan->event_ref.port_event_type,
                    &eventId_ref) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }

        /* Check if the event is already under use */
        if(_bcm_esw_gdpll_input_eventId_inUse(unit,
                    eventId_ref, &inUse) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        if (inUse) {
            /* Event is under use by other channel */
            rv = BCM_E_PARAM;
            goto exit;
        }

        /* Configure the input event and get iaAddr_ref */
        if(_bcm_esw_gdpll_event_config_set(unit, &(pGdpllChan->event_ref), eventId_ref, &iaAddr_ref) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        pGdpllContext->eventId[eventId_ref] = iaAddr_ref;
    }

    /* Channel priority set */
    if(flags & BCM_GDPLL_CHAN_SET_PRIORITY) {
        if(_bcm_esw_gdpll_chan_priority_set(unit, chan,
                                pGdpllChan->chan_prio) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
    }

    /* Channel DPLL configuration */
    if(flags & BCM_GDPLL_CONF_DPLL) {
        dpllParam.dpll_config = pGdpllChan->chan_dpll_config;
        dpllParam.dpll_state  = pGdpllChan->chan_dpll_state;
        if(_bcm_esw_gdpll_chan_config_dpll_set(unit, chan,
                    &dpllParam) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        if (pGdpllChan->chan_dpll_config.phase_counter_ref) {
            phaseCounterRef = 1;
        }
    }

    /* Channel output configurations */
    if(flags & BCM_GDPLL_CHAN_OUTPUT_CONF) {
        if (pGdpllChan->out_event == bcmGdpllOutputEventTXPI) {
            if(_bcm_esw_gdpll_chan_out_txpi_set(unit, chan, pGdpllChan->port) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto exit;
            }
        } else {
            if(_bcm_esw_gdpll_chan_out_misc_set(unit, chan, pGdpllChan->out_event) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto exit;
            }
        }
    }

    /* Set the channel soft state data structures */
    pGdpllContext->dpll_chan[chan].flag |= flags;
    if (phaseCounterRef) {
        pGdpllContext->dpll_chan[chan].phaseConterRef = 1;
        pGdpllContext->dpll_chan[chan].eventId_ref = BCM_GDPLL_NUM_INPUT_EVENTS; /* Indicating invalid event */;
    }

    if((flags & BCM_GDPLL_EVENT_CONFIG_REF) &&
       (pGdpllContext->dpll_chan[chan].phaseConterRef = 0)) {
        pGdpllContext->dpll_chan[chan].eventId_ref = eventId_ref;
    }

    if(flags & BCM_GDPLL_EVENT_CONFIG_FB) {
        pGdpllContext->dpll_chan[chan].eventId_fb = eventId_fb;
    }

    /* Upadet the channel with ref and fb event */
    _bcm_esw_gdpll_chan_update_set(unit, chan);

exit:
    sal_mutex_give(pGdpllContext->mutex);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_chan_destroy
 * Purpose:
 *      Destroys the GDPLL channel
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      chan - (IN) Channel number
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_esw_gdpll_chan_destroy(int unit, int chan)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    if((chan >= BCM_GDPLL_NUM_CHANNELS) ||
       (pGdpllContext == NULL)) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    /* Disable the channel */
    bcm_esw_gdpll_chan_enable(unit, chan, 0);

    pGdpllContext->dpll_chan[chan].flag = 0;

    _bcm_esw_gdpll_input_array_free(unit,
        pGdpllContext->dpll_chan[chan].eventId_ref);
    pGdpllContext->dpll_chan[chan].eventId_ref = BCM_GDPLL_NUM_INPUT_EVENTS;

    _bcm_esw_gdpll_input_array_free(unit,
        pGdpllContext->dpll_chan[chan].eventId_fb);
    pGdpllContext->dpll_chan[chan].eventId_fb = BCM_GDPLL_NUM_INPUT_EVENTS;
    pGdpllContext->dpll_chan[chan].phaseConterRef = BCM_GDPLL_NUM_INPUT_EVENTS;

exit:
    sal_mutex_give(pGdpllContext->mutex);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_chan_state_get
 * Purpose:
 *      Get Channel status
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      chan  - (IN) Channel Number
 *      dpllState - (OUT) DPLL State
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_esw_gdpll_chan_state_get(int unit, int chan, uint32 *dpllState)
{
    int rv = BCM_E_NONE;

    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_chan_debug_enable
 * Purpose:
 *      Enable Channel debugging
 * Parameters:
 *      unit     - (IN) StrataSwitch Unit #.
 *      chan - (IN) Channel Number
 *      debug_mode  - (IN) Debug mode
 *      enable      - (IN) Enable flag
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_esw_gdpll_chan_debug_enable(int unit, int chan,
                    bcm_gdpll_debug_mode_t debug_mode, int enable)
{
    int rv = BCM_E_NONE;

    if(chan >= BCM_GDPLL_NUM_CHANNELS) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_chan_debug_mode_set(unit, chan,
                        debug_mode));
exit:
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_debug_cb_register
 * Purpose:
 *      Register Channel debug callback
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      user_data - (IN) Users private data
 *      cb      - (IN) Callback handler
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_esw_gdpll_debug_cb_register(int unit, void *user_data,
                            bcm_gdpll_debug_cb cb)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    if(pGdpllContext == NULL) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    pGdpllContext->debug_cb = cb;
    pGdpllContext->pUserData = user_data;

exit:
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_flush
 * Purpose:
 *      Flush the GDPLL pipeline
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_esw_gdpll_flush (int unit)
{
    int rv = BCM_E_NONE;
    int chan = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx;

    if(pGdpllContext == NULL) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    for(chan = 0; chan < BCM_GDPLL_NUM_CHANNELS; chan++) {
        bcm_esw_gdpll_chan_destroy(unit, chan);
    }

    pGdpllContext->debug_cb = NULL;
    pGdpllContext->pUserData = NULL;

    _bcm_esw_gdpll_flush(unit);

exit:
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_debug
 * Purpose:
 *      Enable the GDPLL global debug feature
 *      This will set the flag for M7 to start pushing the debug
 *      content to debug buffers
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int
bcm_esw_gdpll_debug (int unit, int enable)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    uint32 dpllDebugFlag = BCM_M7_DTCM_BASE + DPLL_DEBUG_ENABLE_OFFSET;

    if (enable) {
        /* Check if its already enabled */
        if(soc_iproc_m7_read (unit, dpllDebugFlag)) {
            /* Already enabled. Return error */
            rv = BCM_E_PARAM;
            goto exit;
        }

        /* Reset the debug pointers and re-enable */
        READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, 0, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, &regVal, ENABLEf, 0);
        WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, 0, regVal);

        soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, &regVal, ENABLEf, 1);
        WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, 0, regVal);
    }

    soc_iproc_m7_write(unit, dpllDebugFlag, enable);

exit:
    return (rv);
}

#endif
