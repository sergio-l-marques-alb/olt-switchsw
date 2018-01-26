/*
 * $Id: cmix_schan.c,v 1.00 Broadcom SDK $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * S-Channel (internal command bus) support
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/error.h>
#include <sal/core/sync.h>
#include <soc/util.h>
#include <soc/cmic.h>

#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#include <soc/intr_cmicx.h>
#include <soc/schanmsg_internal.h>

#define SOC_SCHAN_POLL_WAIT_TIMEOUT 100


typedef struct intr_data_s {
    sal_sem_t   schanIntr;
    int         ch;
}intr_data_t;

typedef struct soc_cmicx_schan_s {
    sal_spinlock_t   lock;
    int              timeout;
    int              ch_map;
    intr_data_t      intr[CMIC_SCHAN_NUM_MAX];
}soc_cmicx_schan_t;


STATIC soc_cmicx_schan_t _soc_cmicx_schan[SOC_MAX_NUM_DEVICES];


/*******************************************
* @function _soc_cmicx_schan_reset
* purposeResets the CMICX S-Channel interface.
*
* @param unit [in] unit #
* @param cmc [in] cmc number 0
* @param ch [in] channel number 0-4
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
********************************************/

STATIC void
_soc_cmicx_schan_reset(int unit, int cmc, int ch)
{
    uint32 val;

    COMPILER_REFERENCE(cmc);

    val = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch));
    /* Toggle S-Channel abort bit in CMIC_SCHAN_CTRL register */
    soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch),
                                       val | SC_CHx_SCHAN_ABORT);
    SDK_CONFIG_MEMORY_BARRIER;
    soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch), val);
    SDK_CONFIG_MEMORY_BARRIER;

    if (SAL_BOOT_QUICKTURN) {
        /* Give Quickturn at least 2 cycles */
        sal_usleep(10 * MILLISECOND_USEC);
    }
}


/*******************************************
* @function _soc_cmicx_schan_get
* purpose get idle channel
*
* @param unit [in] unit #
* @param ch [out] channel number 0-4
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
********************************************/
STATIC int _soc_cmicx_schan_get(int unit, int *ch)
{
    int i;
    int rv = SOC_E_BUSY;

    soc_timeout_t to;

    soc_timeout_init(&to,  _soc_cmicx_schan[unit].timeout, 100);

    do {
       sal_spinlock_lock(_soc_cmicx_schan[unit].lock);
       for ( i = 0; i < CMIC_SCHAN_NUM_MAX; i++) {
           if (_soc_cmicx_schan[unit].ch_map & 0x01 << i) {
               rv = SOC_E_NONE;
               *ch = i;
               _soc_cmicx_schan[unit].ch_map &= ~(0x01 << i);
               break;
           }
       }
       sal_spinlock_unlock(_soc_cmicx_schan[unit].lock);
       if (rv == SOC_E_NONE)
          break;

    } while (!soc_timeout_check(&to));
    return rv;
}


/*******************************************
* @function _soc_cmicx_schan_put
* purpose get idle channel
*
* @param unit [in] unit #
* @param ch [in] channel number 0-4
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE

*
* @end
********************************************/
STATIC int _soc_cmicx_schan_put(int unit, int ch)
{
   int rv = SOC_E_NONE;

   if ((ch < 0) || (ch > CMIC_SCHAN_NUM_MAX -1)) {
       rv = SOC_E_PARAM;
       return rv;
   }
   sal_spinlock_lock(_soc_cmicx_schan[unit].lock);
   _soc_cmicx_schan[unit].ch_map |= (0x01 << ch);
   sal_spinlock_unlock(_soc_cmicx_schan[unit].lock);

   return rv;
}

/*******************************************
* @function _soc_cmicx_schan_cmic_err_handle
* purpose Handle schan cimicx error
*
* @param unit [in] unit
* @param schanCtrl [in] schan control word
* @param ch [in] channel
*
* @returns SOC_E_XXX
* @returns SOC_E_NONE
*
* @end
********************************************/

STATIC int
_soc_cmicx_schan_err_handle(int unit, int schanCtrl, int ch)
{
    int rv = SOC_E_NONE;
    uint32 schan_err = 0;

    if (schanCtrl & SC_CHx_MSG_NAK) {
        LOG_ERROR(BSL_LS_SOC_SCHAN,
                  (BSL_META_U(unit,
                   "NAK received from SCHAN.\n")));
        rv = SOC_E_FAIL;
    }

    if (schanCtrl & SC_CHx_MSG_SER_CHECK_FAIL) {
        LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                  "SER Parity Check Error.\n")));
        rv = SOC_E_FAIL;
    }

    if (soc_feature(unit, soc_feature_schan_hw_timeout)) {
        if (schanCtrl & SC_CHx_MSG_TIMEOUT_TST) {
            LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                      "Hardware Timeout Error.\n")));
            rv = SOC_E_TIMEOUT;
        }
    }

    if (soc_feature(unit, soc_feature_schan_err_check)) {
        schan_err = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_ERR(ch));

        if (schan_err & SC_CHx_MSG_ERR_BIT) {
            rv = SOC_E_FAIL;
            LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
                "  ERRBIT received in CMIC_SCHAN_ERR.\n")));

        }
        soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch), SC_CHx_MSG_CLR);
    }

    return rv;
}

/*******************************************
* @function _soc_cmicx_schan_intr_wait
* purpose On CMICX chips with schan interrupts
*
* @param unit [in] unit
* @param ch [in] channel number 0-4
*
* @returns SOC_E_XXX
* @returns SOC_E_XXX
*
* Commnet: Called when CMICX chips with schan interrupts
* enabled, wait on a schan interrupt.
* When schan interrupts are disabled,
* _soc_cmicx_schan_poll_wait is called instead.

* @end
********************************************/
STATIC int
_soc_cmicx_schan_intr_wait(int unit, int ch)
{
    int rv = SOC_E_NONE;
    uint32 schanCtrl;
    intr_data_t    *int_data;

    soc_cmic_intr_enable(unit, INTR_SCHAN(ch));
    int_data = &_soc_cmicx_schan[unit].intr[ch];
    if (sal_sem_take(int_data->schanIntr,
                     SOC_CONTROL(unit)->schanTimeout) != 0) {
        soc_cmic_intr_disable(unit, INTR_SCHAN(ch));
        rv = SOC_E_TIMEOUT;
    }

    if (rv != SOC_E_TIMEOUT) {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Interrupt Done\n")));
        schanCtrl = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch));
        rv = _soc_cmicx_schan_err_handle(unit, schanCtrl, ch);
    }
    return rv;
}

/*******************************************
* @function _soc_cmicx_schan_poll_wait
* purpose On CMICX chips with no schan interrupts
*
* @param unit [in] unit
* @param msg [in] message <schan_msg_t>
* @param ch [in] channel number 0-4
*
* @returns SOC_E_XXX
* @returns SOC_E_XXX
*
* Commnet: Called when CMICX chips with schan interrupts
* disabled, wait on a schan interrupt.
* When schan interrupts are enabled,
* _soc_cmicx_schan_intr_wait is called instead.

* @end
********************************************/

STATIC int
_soc_cmicx_schan_poll_wait(int unit, schan_msg_t *msg, int ch)
{
    int rv = SOC_E_NONE;
    soc_timeout_t to;
    uint32 schanCtrl;

    soc_timeout_init(&to, SOC_CONTROL(unit)->schanTimeout,
                     SOC_SCHAN_POLL_WAIT_TIMEOUT);

    LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Timeout= %d usec\n"),
                             SOC_CONTROL(unit)->schanTimeout));

    while (((schanCtrl = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch))) &
            SC_CHx_MSG_DONE) == 0) {
        if (soc_timeout_check(&to)) {
            rv = SOC_E_TIMEOUT;
            LOG_VERBOSE(BSL_LS_SOC_SCHAN, (BSL_META_U(unit,
              " schan control value 0x%x after timeout in %d polls\n"), schanCtrl, to.polls));
            break;
        }
    }
    /* Polling finished no timeout , check other errors */
    if (rv == SOC_E_NONE) {
        LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Done in %d polls\n"), to.polls));
        rv = _soc_cmicx_schan_err_handle(unit, schanCtrl, ch);
    }

    return rv;
}

/*******************************************
* @function _soc_cmicx_schan_wait
* purpose This waits for either interrupt or poll
*
* @param handle [in] unit
* @param msg [in] SCHAN message pointer
* @param dwc_write [in] Number of messages to write
* @param dwc_read [in] Number of messages to read
* @param intr [in] Interrupt
* @param ch [in] channel number 0-4
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/

STATIC int _soc_cmicx_schan_wait(int unit,
                                 schan_msg_t *msg,
                                 int dwc_write,
                                 int dwc_read,
                                 int intr,
                                 int ch)
{
    int i, rv;

    LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Enter\n")));
    do {

        /* Wait for completion using either the interrupt or polling method */
        if (intr && *(SOC_SCHAN_CONTROL(unit).schanIntrEnb)) {
            rv = _soc_cmicx_schan_intr_wait(unit, ch);
        } else {
            rv = _soc_cmicx_schan_poll_wait(unit, msg, ch);
        }

        if (soc_schan_timeout_check(unit, &rv, msg, -1, ch) == TRUE) {
            break;
        }

        /* Read in data from S-Channel buffer space, if any */
        for (i = 0; i < dwc_read; i++) {
            msg->dwords[i] =
                 soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(ch, i));
        }

        if (LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_VERBOSE)) {
            soc_schan_dump(unit, msg, dwc_read);
        }

    } while(0);

    if (rv == SOC_E_TIMEOUT) {
        if (LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_ERROR)) {
            LOG_ERROR(BSL_LS_SOC_SCHAN,
                      (BSL_META_U(unit,
                       "soc_cmicx_schan_op operation timed out\n")));
            soc_schan_dump(unit, msg, dwc_read);
        }
    }

    return rv;
}

/*******************************************
* @function soc_cmicx_schan_op
* purpose SDK-6 API used to initiate the schan operation
*
* @param handle [in] unit
* @param msg [in] SCHAN message pointer
* @param dwc_write [in] Number of messages to write
* @param dwc_read [in] Number of messages to read
* @param intr [in] Interrupt
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
STATIC int
_soc_cmicx_schan_op(
         int unit,
         schan_msg_t *msg,
         int dwc_write,
         int dwc_read,
         int intr)

{
    int i, rv;
    int ch;

    LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Enter\n")));

    if (soc_schan_op_sanity_check(unit, msg,
                   dwc_write, dwc_read, &rv) == TRUE) {
        return rv;
    }

    /* Get the free channel */
    rv = _soc_cmicx_schan_get(unit, &ch);

    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "  Unable to assign channel\n")));
        return rv;

    }

    LOG_VERBOSE(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             " Assign channel = %d\n"), ch));

    do {
        /* Write raw S-Channel Data: dwc_write words */
        for (i = 0; i < dwc_write; i++) {
            soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_MESSAGEn(ch, i),
                                            msg->dwords[i]);
        }

        /* Tell CMIC to start */
        soc_pci_write(unit, CMIC_COMMON_POOL_SCHAN_CHx_CTRL(ch),
                                            SC_CHx_MSG_START);
    } while(0);

    rv = _soc_cmicx_schan_wait(unit, msg, dwc_write, dwc_read, intr, ch);

    _soc_cmicx_schan_put(unit, ch);

   return rv;
}

/*******************************************
* @function _cmicx_schan_ch_done
* Interrupt handler for schan channel done
*
* @param unit [in] unit
* @param data [in] pointer provided during registration
*
*
* @end
********************************************/
STATIC void
_cmicx_schan_ch_done(int unit, void *data)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    intr_data_t *in_d = (intr_data_t *)data;

    if (soc->schanIntrEnb) {
        soc_cmic_intr_disable(unit, INTR_SCHAN(in_d->ch));
        sal_sem_give(in_d->schanIntr);
    }
}

/*******************************************
* @function _cmicx_schan_intr_deinit
* purpose API to de-Initialize SCHAN interrupts
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
STATIC int
_cmicx_schan_intr_deinit(int unit)
{
    int ch;
    intr_data_t    *int_data;
    for (ch = 0; ch < CMIC_SCHAN_NUM_MAX; ch++) {
        int_data = &_soc_cmicx_schan[unit].intr[ch];
        if (int_data->schanIntr) {
             sal_sem_destroy(int_data->schanIntr);
        }
    }

    return SOC_E_NONE;
}

/*******************************************
* @function _cmicx_schan_intr_init
* purpose API to Initialize SCHAN interrupts
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
STATIC int
_cmicx_schan_intr_init(int unit)
{
    int ch;
    soc_cmic_intr_handler_t *handle;
    soc_cmic_intr_handler_t *hitr;
    intr_data_t    *int_data;
    int rv = SOC_E_NONE;


    /* Register interrupts */
    handle = sal_alloc(CMIC_SCHAN_NUM_MAX * sizeof(soc_cmic_intr_handler_t),
                        "schan_interrupt");
    if (handle == NULL) {
        return SOC_E_MEMORY;
    }

    hitr = handle;
    for (ch = 0; ch < CMIC_SCHAN_NUM_MAX; ch++) {
         int_data = &_soc_cmicx_schan[unit].intr[ch];
         int_data->schanIntr =
                         sal_sem_create("SCHAN interrupt",
                                        sal_sem_BINARY, 0);
         if (int_data->schanIntr == NULL) {
             _cmicx_schan_intr_deinit(unit);
             rv = SOC_E_MEMORY;
             goto error;
         }
         int_data->ch = ch;
         hitr->num = INTR_SCHAN(ch);
         hitr->intr_fn = _cmicx_schan_ch_done;
         hitr->intr_data = int_data;
         hitr++;
    }
    rv = soc_cmic_intr_register(unit, handle, CMIC_SCHAN_NUM_MAX);
error:
    sal_free(handle);

    return rv;
}


/*******************************************
* @function _soc_cmicx_schan_deinit
* purpose API to de-Initialize SCHAN
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
STATIC int
_soc_cmicx_schan_deinit(int unit)
{
   int ch;

   _cmicx_schan_intr_deinit(unit);

   /* reset all th schan channels */
   for (ch = 0 ; ch < CMIC_SCHAN_NUM_MAX; ch++) {
       _soc_cmicx_schan_reset(unit, -1, ch);
   }
   sal_spinlock_destroy(_soc_cmicx_schan[unit].lock);

   return SOC_E_NONE;
}

/*******************************************
* @function soc_cmicx_schan_init
* purpose API to Initialize cmicx SCHAN
*
* @param unit [in] unit
* @param drv [out] soc_cmic_schan_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
int soc_cmicx_schan_init(int unit, soc_cmic_schan_drv_t *drv)

{
   int rv = SOC_E_NONE;

#if defined(PLISIM) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
        if (SAL_BOOT_PLISIM && (SOC_IS_DNX(unit)|| SOC_IS_DNXF(unit))) {
            /* Use CMICM simulation instead of CMICX simulation */
            return soc_cmicm_schan_init(unit, drv);
        }
#endif

   sal_memset(&_soc_cmicx_schan[unit], 0, sizeof(soc_cmicx_schan_t));
   _soc_cmicx_schan[unit].lock = sal_spinlock_create("schan Lock");

   if (_soc_cmicx_schan[unit].lock == NULL) {
        return SOC_E_MEMORY;
   }

   _soc_cmicx_schan[unit].ch_map = (0x01 << CMIC_SCHAN_NUM_MAX) -1;
   _soc_cmicx_schan[unit].timeout = SOC_CONTROL(unit)->schanTimeout;
   /* program SBUS_RING_MAP registers */

   /* Program SCHAN ARB Weighted priority
    * CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_BASE  12'h04c */

   /* Configure CMIC_SBUS_TIMEOUT */
   rv = _cmicx_schan_intr_init(unit);

   if (SOC_FAILURE(rv)) {
       _soc_cmicx_schan_deinit(unit);
       return rv;
   }

   drv->soc_schan_deinit = _soc_cmicx_schan_deinit;
   drv->soc_schan_op = _soc_cmicx_schan_op;
   drv->soc_schan_reset = _soc_cmicx_schan_reset;

   return rv;
}


#endif /* BCM_CMICX_SUPPORT  */
