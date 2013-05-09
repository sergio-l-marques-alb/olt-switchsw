/*
 * $Id: telecom.c 1.1.2.6 Broadcom SDK $
 * 
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */
#if defined(INCLUDE_PTP)

#include <bcm/ptp.h>
#include <bcm_int/esw/ptp.h>
#include <bcm/error.h>

#include <sal/core/dpc.h>
#include <sal/core/time.h>

/* Definitions. */
#define PTP_TELECOM_CONTROLLER_DPC_TIME_USEC_DEFAULT              (1000000)
#define PTP_TELECOM_CONTROLLER_DPC_TIME_USEC_IDLE                 (3000000)
#define PTP_TELECOM_MUTEX_TIMEOUT_USEC                              (50000)

#define PTP_TELECOM_MESSAGE_ELAPSED_TIME_MSEC_MAX                 (3600000)
#define PTP_TELECOM_MESSAGE_ELAPSED_TIME_MSEC_UNAVAIL          ((uint32)-1)

#define PACKET_MASTER(index)                            (telecom.GM[index])
#define PACKET_MASTER_BEST               PACKET_MASTER(telecom.selected_GM)

/* Macros. */
#define PTP_TELECOM_MUTEX_TAKE()                                                         \
    do {                                                                                 \
        int __rv__ = _bcm_ptp_mutex_take(telecom_mutex, PTP_TELECOM_MUTEX_TIMEOUT_USEC); \
        if (BCM_FAILURE(__rv__)) {                                                       \
            soc_cm_print("%s() failed _bcm_ptp_mutex_take() returned %d : %s\n",         \
                         __func__, __rv__, bcm_errmsg(__rv__));                          \
            return BCM_E_INTERNAL;                                                       \
        }                                                                                \
    } while (0)

#define PTP_TELECOM_MUTEX_RELEASE_RETURN(__rv__) \
    do {                                         \
        _bcm_ptp_mutex_give(telecom_mutex);      \
        return __rv__;                           \
    } while (0)


/* Constants and variables. */
static int telecom_init = 0;
static _bcm_ptp_mutex_t telecom_mutex = 0x0;

static const _bcm_ptp_telecom_pktmaster_t pktmaster_zero;
static _bcm_ptp_telecom_profile_t telecom;

/* Static functions. */
static int _bcm_ptp_telecom_profile_enabled_set(
    int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, uint8 enabled);

static void _bcm_ptp_telecom_controller(
    void *owner, void *arg_unit, void *arg_ptp_id,
    void *arg_clock_num, void *unused);

static int _bcm_ptp_telecom_AMT_set(
    int unit, bcm_ptp_stack_id_t ptp_id, int clock_num, uint32 clock_port);

static int _bcm_ptp_telecom_master_selector(
    void);

static int _bcm_ptp_telecom_PTSF_set(
    void);

static int _bcm_ptp_telecom_packet_master_PTSF_set(
    _bcm_ptp_telecom_pktmaster_t *pktmaster);

static int _bcm_ptp_telecom_degradeQL_set(
    void);

static int _bcm_ptp_telecom_packet_master_degradeQL_set(
    _bcm_ptp_telecom_pktmaster_t *pktmaster);

static int _bcm_ptp_telecom_wait_to_restore_set(
    void);

static int _bcm_ptp_telecom_packet_master_wait_to_restore_set(
    _bcm_ptp_telecom_pktmaster_t *pktmaster);

static int _bcm_ptp_telecom_packet_master_fmds_processor(
    int unit, bcm_ptp_stack_id_t ptp_id, int clock_num);

static int _bcm_ptp_telecom_packet_master_priority_calculator(
    const uint8 gmPriority1, const uint8 gmPriority2,
    _bcm_ptp_telecom_pktmaster_t *pktmaster);

static int _bcm_ptp_telecom_packet_master_manager(
    bcm_ptp_clock_port_address_t *address, int *index);

static int _bcm_ptp_telecom_packet_master_lookup(
    bcm_ptp_clock_port_address_t *address, int *index);

static int _bcm_ptp_telecom_packet_master_make(
    bcm_ptp_clock_port_address_t *address,
    _bcm_ptp_telecom_pktmaster_t *pktmaster);


/*
 * Function:
 *      _bcm_ptp_telecom_init
 * Purpose:
 *      Initialize the telecom profile.
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      ptp_id    - (IN)  PTP stack ID.
 *      clock_num - (IN)  PTP clock number.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_init(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rv;
    int el;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id,
            clock_num, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    if (!telecom_mutex) {
        telecom_mutex = _bcm_ptp_mutex_create("telecom_packet_master_array");
    }

    /* Set ITU-T G.781 synchronization networking option. */
    telecom.network_option = _bcm_ptp_telecom_disable;

    /*
     * Set PTP message receipt timeouts.
     * NOTE : announceReceiptTimeout is not referenced in current implementation.
     *        PTSF lossAnnounce is handled in the firmware foreignMasterDS logic.
     */
    telecom.receipt_timeouts.announce = PTP_TELECOM_ANNOUNCE_RECEIPT_TIMEOUT_MSEC_DEFAULT;
    telecom.receipt_timeouts.sync = PTP_TELECOM_SYNC_RECEIPT_TIMEOUT_MSEC_DEFAULT;
    telecom.receipt_timeouts.delayResp = PTP_TELECOM_DELAYRESP_RECEIPT_TIMEOUT_MSEC_DEFAULT;

    /* Set packet statistics thresholds. */
    COMPILER_64_SET(telecom.thresholds.pdv_scaled_allan_var, 
        COMPILER_64_HI((uint64)PTP_TELECOM_PDV_SCALED_ALLAN_VARIANCE_THRESHOLD_NSECSQ_DEFAULT),
        COMPILER_64_LO((uint64)PTP_TELECOM_PDV_SCALED_ALLAN_VARIANCE_THRESHOLD_NSECSQ_DEFAULT));

    /* Set selected packet master. */
    telecom.previous_GM = -1;
    telecom.selected_GM = 0;

    /* Initialize packet master array entries. */
    for (el = 0; el < PTP_TELECOM_MAX_NUMBER_PKTMASTERS; ++el) {
        PACKET_MASTER(el) = pktmaster_zero;
        PACKET_MASTER(el).selector.non_reversion = PTP_TELECOM_NON_REVERSION_MODE_DEFAULT;
        PACKET_MASTER(el).selector.wait_sec = PTP_TELECOM_WAIT_TO_RESTORE_SEC_DEFAULT;
        PACKET_MASTER(el).state = _bcm_ptp_telecom_pktmaster_state_UNUSED;
    }

    telecom_init = 1;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_shutdown
 * Purpose:
 *      Shut down the telecom profile.
 * Parameters:
 *      NONE
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_shutdown(
    void)
{
    /* Cancel telecom controller delayed procedure call. */
    sal_dpc_cancel(INT_TO_PTR(&_bcm_ptp_telecom_controller));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_network_option_get
 * Purpose:
 *      Get telecom profile ITU-T G.781 network option.
 * Parameters:
 *      unit           - (IN)  Unit number.
 *      ptp_id         - (IN)  PTP stack ID.
 *      clock_num      - (IN)  PTP clock number.
 *      network_option - (OUT) Telecom profile ITU-T G.781 network option.
 *                             |Disable |Option I |Option II |Option III |
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_network_option_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    _bcm_ptp_telecom_network_option_t *network_option)
{
    *network_option = telecom.network_option;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_network_option_set
 * Purpose:
 *      Set telecom profile ITU-T G.781 network option.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      ptp_id         - (IN) PTP stack ID.
 *      clock_num      - (IN) PTP clock number.
 *      network_option - (IN) Telecom profile ITU-T G.781 network option.
 *                            |Disable |Option I |Option II |Option III |
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_network_option_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    _bcm_ptp_telecom_network_option_t network_option)
{
    int rv;
    uint8 enabled;

    telecom.network_option = network_option;

    enabled = (_bcm_ptp_telecom_disable == network_option) ? (0):(1);
    if (BCM_FAILURE(rv = _bcm_ptp_telecom_profile_enabled_set(unit, ptp_id,
            clock_num, enabled))){
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_quality_level_set
 * Purpose:
 *      Set the ITU-T G.781 quality level (QL) of local clock.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      ptp_id     - (IN) PTP stack ID.
 *      clock_num  - (IN) PTP clock number.
 *      QL         - (IN) Quality level.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Ref. ITU-T G.781 and ITU-T G.8265.1.
 *      Quality level is mapped to PTP clockClass.
 *      Quality level is used to infer ITU-T G.781 synchronization network
 *      option. ITU-T G.781 synchronization network option must be uniform
 *      among slave and its packet masters.
 */
int
_bcm_ptp_telecom_quality_level_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    _bcm_ptp_telecom_QL_t QL)
{
    int rv = BCM_E_NONE;
    uint8 clockClass;
    bcm_ptp_clock_info_t ci;

    if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_info_get(unit, ptp_id,
            clock_num, &ci))) {
        return rv;
    }

    if (ci.slaveonly) {
        /* Packet slave clock. Supersede caller-provided QL. */
        QL = _bcm_ptp_telecom_QL_NA_SLV;
    } else {
        /* Set ITU-T G.781 synchronization networking option. */
        if (telecom.network_option != (QL /0x10)) {
            soc_cm_print("ITU-T G.781 synchronization networking option changed.\n");
        }
        telecom.network_option = QL / 0x10;
    }

    /* Set PTP clockClass. */
    if (BCM_FAILURE(rv = _bcm_ptp_telecom_map_QL_clockClass(QL, &clockClass))) {
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_clock_class_set(unit, ptp_id, clock_num, clockClass))) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_receipt_timeout_set
 * Purpose:
 *      Set PTP |announce|sync|delayResp| message receipt timeout req'd for
 *      packet timing signal fail (PTSF) analysis.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      ptp_id         - (IN) PTP stack ID.
 *      clock_num      - (IN) PTP clock number.
 *      receiptTimeout - (IN) PTP |announce|sync|delayResp| message receipt timeout.
 *      messageType    - (IN) PTP message type.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_receipt_timeout_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 receiptTimeout,
    _bcmPTPmessageType_t messageType)
{
    switch (messageType) {
    case _bcmPTPmessageType_announce:
        telecom.receipt_timeouts.announce = receiptTimeout;
        break;

    case _bcmPTPmessageType_sync:
        telecom.receipt_timeouts.sync = receiptTimeout;
        break;

    case _bcmPTPmessageType_delay_resp:
        telecom.receipt_timeouts.delayResp = receiptTimeout;
        break;

    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* Function:
 *      _bcm_ptp_telecom_pktstats_thresholds_set
 * Purpose:
 *      Set packet timing and PDV statistics thresholds.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      ptp_id     - (IN) PTP stack ID.
 *      clock_num  - (IN) PTP clock number.
 *      thresholds - (IN) Packet timing and PDV statistics thresholds.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_pktstats_thresholds_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    _bcm_ptp_telecom_pktstats_t thresholds)
{
    COMPILER_64_SET(telecom.thresholds.pdv_scaled_allan_var,
        COMPILER_64_HI(thresholds.pdv_scaled_allan_var),
        COMPILER_64_LO(thresholds.pdv_scaled_allan_var));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_list
 * Purpose:
 *      Get list of in-use packet masters.
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      ptp_id      - (IN)  PTP stack ID.
 *      clock_num   - (IN)  PTP clock number.
 *      num_masters - (OUT) Number of in-use packet masters.
 *      best_master - (OUT) Best packet master list index.
 *      pktmaster   - (OUT) Packet masters.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_packet_master_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *num_masters,
    int *best_master,
    _bcm_ptp_telecom_pktmaster_t *pktmaster)
{
    int el;

    *num_masters = 0;
    *best_master = -1;

    PTP_TELECOM_MUTEX_TAKE();

    for (el = 0; el < PTP_TELECOM_MAX_NUMBER_PKTMASTERS; ++el) {
        if (_bcm_ptp_telecom_pktmaster_state_UNUSED != PACKET_MASTER(el).state) {
            if (telecom.selected_GM == el) {
                *best_master = *num_masters;
            }
            pktmaster[(*num_masters)++] = PACKET_MASTER(el);
        }
    }

    PTP_TELECOM_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_best_get
 * Purpose:
 *      Get current best packet master clock as defined by telecom profile
 *      master selection logic. 
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      ptp_id    - (IN)  PTP stack ID.
 *      clock_num - (IN)  PTP clock number.
 *      pktmaster - (OUT) Best packet master clock.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_packet_master_best_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    _bcm_ptp_telecom_pktmaster_t *pktmaster)
{
    PTP_TELECOM_MUTEX_TAKE();

    if (_bcm_ptp_telecom_pktmaster_state_UNUSED != PACKET_MASTER_BEST.state) {
        *pktmaster = PACKET_MASTER_BEST;
        PTP_TELECOM_MUTEX_RELEASE_RETURN(BCM_E_NONE);
    }

    *pktmaster = pktmaster_zero;
    PTP_TELECOM_MUTEX_RELEASE_RETURN(BCM_E_NOT_FOUND);
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_get
 * Purpose:
 *      Get packet master by address. 
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      ptp_id    - (IN)  PTP stack ID.
 *      clock_num - (IN)  PTP clock number.
 *      address   - (IN)  Packet master address.
 *      pktmaster - (OUT) Packet master.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_packet_master_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_port_address_t *address,
    _bcm_ptp_telecom_pktmaster_t *pktmaster)
{
    int rv;
    int el;

    PTP_TELECOM_MUTEX_TAKE();

    if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_lookup(address, &el))) {
        *pktmaster = pktmaster_zero;
        PTP_ERROR_FUNC("_bcm_ptp_telecom_packet_master_lookup()");
        PTP_TELECOM_MUTEX_RELEASE_RETURN(rv);
    }

    *pktmaster = PACKET_MASTER(el);
    PTP_TELECOM_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_add
 * Purpose:
 *      Add a packet master.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      address   - (IN) Packet master address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_packet_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_port_address_t *address)
{
    int rv;
    int el;

    PTP_TELECOM_MUTEX_TAKE();

    
    if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_manager(address, &el))) {
        PTP_ERROR_FUNC("_bcm_ptp_telecom_packet_master_manager()");
        PTP_TELECOM_MUTEX_RELEASE_RETURN(rv);
    }

    /* Initialize packet master data. */
    _bcm_ptp_telecom_packet_master_make(address, &PACKET_MASTER(el));

    PTP_TELECOM_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_remove
 * Purpose:
 *      Remove a packet master.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      address   - (IN) Packet master address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_packet_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_port_address_t *address)
{
    int rv;
    int el;

    PTP_TELECOM_MUTEX_TAKE();

    
    if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_manager(address, &el))) {
        PTP_ERROR_FUNC("_bcm_ptp_telecom_packet_master_manager()");
        PTP_TELECOM_MUTEX_RELEASE_RETURN(rv);
    }

    /* Clear packet master data. */
    PACKET_MASTER(el) = pktmaster_zero;
    PACKET_MASTER(el).selector.non_reversion = PTP_TELECOM_NON_REVERSION_MODE_DEFAULT;
    PACKET_MASTER(el).selector.wait_sec = PTP_TELECOM_WAIT_TO_RESTORE_SEC_DEFAULT;
    PACKET_MASTER(el).state = _bcm_ptp_telecom_pktmaster_state_UNUSED;

    /* Call telecom controller (so changes take effect immediately). */
    if (_bcm_ptp_telecom_disable != telecom.network_option) {
        sal_dpc_cancel(INT_TO_PTR(&_bcm_ptp_telecom_controller));
        sal_dpc_time(0, &_bcm_ptp_telecom_controller, INT_TO_PTR(&_bcm_ptp_telecom_controller),
                     INT_TO_PTR(unit), INT_TO_PTR(ptp_id), INT_TO_PTR(clock_num), 0);
    }

    PTP_TELECOM_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_lockout_set
 * Purpose:
 *      Set packet master lockout to exclude from master selection process.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      lockout   - (IN) Packet master lockout Boolean.
 *      address   - (IN) Packet master address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_packet_master_lockout_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 lockout,
    bcm_ptp_clock_port_address_t *address)
{
    int rv;
    int el;

    PTP_TELECOM_MUTEX_TAKE();

    if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_lookup(address, &el))) {
        PTP_ERROR_FUNC("_bcm_ptp_telecom_packet_master_lookup()");
        PTP_TELECOM_MUTEX_RELEASE_RETURN(rv);
    }
    PACKET_MASTER(el).selector.lockout = lockout ? 1:0;

    PTP_TELECOM_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_non_reversion_set
 * Purpose:
 *      Set packet master non-reversion to control master selection process
 *      if master fails or is unavailable.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      nonrev    - (IN) Packet master non-reversion Boolean.
 *      address   - (IN) Packet master address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_packet_master_non_reversion_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 nonrev,
    bcm_ptp_clock_port_address_t *address)
{
    int rv;
    int el;

    PTP_TELECOM_MUTEX_TAKE();

    if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_lookup(address, &el))) {
        PTP_ERROR_FUNC("_bcm_ptp_telecom_packet_master_lookup()");
        PTP_TELECOM_MUTEX_RELEASE_RETURN(rv);
    }
    PACKET_MASTER(el).selector.non_reversion = nonrev ? 1:0;

    if (nonrev) {
        /*
         * Clear historical information so non-revertive behavior applies
         * from this point forward.
         */
        PACKET_MASTER(el).PTSF.counter = 0;
        PACKET_MASTER(el).PTSF.timestamp = 0;

        PACKET_MASTER(el).hQL.counter = 0;
        PACKET_MASTER(el).hQL.timestamp = 0;
    }

    PTP_TELECOM_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_wait_duration_set
 * Purpose:
 *      Set packet master wait-to-restore duration to control master selection
 *      process if master fails or is unavailable.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      wait_sec  - (IN) Packet master wait-to-restore duration.
 *      address   - (IN) Packet master address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_telecom_packet_master_wait_duration_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    sal_time_t wait_sec,
    bcm_ptp_clock_port_address_t *address)
{
    int rv;
    int el;

    PTP_TELECOM_MUTEX_TAKE();

    if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_lookup(address, &el))) {
        PTP_ERROR_FUNC("_bcm_ptp_telecom_packet_master_lookup()");
        PTP_TELECOM_MUTEX_RELEASE_RETURN(rv);
    }
    PACKET_MASTER(el).selector.wait_sec = wait_sec;

    PTP_TELECOM_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_priority_override
 * Purpose:
 *      Set priority override of packet master.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      override  - (IN) Packet master priority override Boolean.
 *      address   - (IN) Packet master address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Priority override controls whether a packet master's priority value
 *      is set automatically using grandmaster priority1 / priority2 in PTP
 *      announce message (i.e., override equals TRUE) or via host API calls
 *      (i.e., override equals FALSE).
 */
int
_bcm_ptp_telecom_packet_master_priority_override(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 override,
    bcm_ptp_clock_port_address_t *address)
{
    int rv;
    int el;

    PTP_TELECOM_MUTEX_TAKE();

    if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_lookup(address, &el))) {
        PTP_ERROR_FUNC("_bcm_ptp_telecom_packet_master_lookup()");
        PTP_TELECOM_MUTEX_RELEASE_RETURN(rv);
    }
    PACKET_MASTER(el).priority.override = override;

    PTP_TELECOM_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_priority_set
 * Purpose:
 *      Set priority of packet master.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      ptp_id     - (IN) PTP stack ID.
 *      clock_num  - (IN) PTP clock number.
 *      priority   - (IN) Packet master priority.
 *      address    - (IN) Packet master address.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Ref. ITU-T G.8265.1. 
 */
int
_bcm_ptp_telecom_packet_master_priority_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 priority,
    bcm_ptp_clock_port_address_t *address)
{
    int rv;
    int el;

    PTP_TELECOM_MUTEX_TAKE();

    if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_lookup(address, &el))) {
        PTP_ERROR_FUNC("_bcm_ptp_telecom_packet_master_lookup()");
        PTP_TELECOM_MUTEX_RELEASE_RETURN(rv);
    }
    PACKET_MASTER(el).priority.value = priority;

    PTP_TELECOM_MUTEX_RELEASE_RETURN(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_ptp_telecom_map_QL_clockClass
 * Purpose:
 *      Map ITU-T G.781 quality level to PTP clock class.
 * Parameters:
 *      QL         - (IN)  Quality level.
 *      clockClass - (OUT) PTP clock class.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Ref. ITU-T G.8265.1, Clause 6.7.3.1. 
 */
int
_bcm_ptp_telecom_map_QL_clockClass(
    const _bcm_ptp_telecom_QL_t QL,
    uint8 *clockClass)
{
    switch (QL) {
    case _bcm_ptp_telecom_QL_II_PRS:
        *clockClass = 80;
        break;
    case _bcm_ptp_telecom_QL_II_STU:
        /* Fall through. */
    case _bcm_ptp_telecom_QL_III_UNK:
        *clockClass = 82;
        break;
    case _bcm_ptp_telecom_QL_I_PRC:
        *clockClass = 84;
        break;
    case _bcm_ptp_telecom_QL_II_ST2:
        *clockClass = 86;
        break;
    case _bcm_ptp_telecom_QL_I_SSUA:
        /* Fall through. */
    case _bcm_ptp_telecom_QL_II_TNC:
        *clockClass = 90;
        break;
    case _bcm_ptp_telecom_QL_I_SSUB:
        *clockClass = 96;
        break;
    case _bcm_ptp_telecom_QL_II_ST3E:
        *clockClass = 100;
        break;
    case _bcm_ptp_telecom_QL_II_ST3:
        *clockClass = 102;
        break;
    case _bcm_ptp_telecom_QL_I_SEC:
        /* Fall through. */
    case _bcm_ptp_telecom_QL_III_SEC:
        *clockClass = 104;
        break;
    case _bcm_ptp_telecom_QL_II_SMC:
        *clockClass = 106;
        break;
    case _bcm_ptp_telecom_QL_II_PROV:
        *clockClass = 108;
        break;
    case _bcm_ptp_telecom_QL_I_DNU:
        /* Fall through. */
    case _bcm_ptp_telecom_QL_II_DUS:
        *clockClass = 110;
        break;
    case _bcm_ptp_telecom_QL_NA_SLV:
        /* Not applicable (packet slave). */
        *clockClass = 255;
        break;
    default:
        /*
         * Mapping unsuccessful.
         * Maximum alternate PTP profile clockClass value in range [68,122]
         * used by telecom profile. Ref. IEEE Std. 1588-2008, Clause 7.6.2.4.
         */
        *clockClass = 122;
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_map_clockClass_QL
 * Purpose:
 *      Map PTP clock class to ITU-T G.781 quality level.
 * Parameters:
 *      clockClass - (IN)  PTP clock class.
 *      QL         - (OUT) Quality level.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Transformation requires ITU-T G.781 network option.
 */
int
_bcm_ptp_telecom_map_clockClass_QL(
    const uint8 clockClass,
    _bcm_ptp_telecom_QL_t *QL)
{
    if (clockClass == 255) {
        *QL = _bcm_ptp_telecom_QL_NA_SLV;
        return BCM_E_NONE;
    }

    switch (telecom.network_option) {
    case (_bcm_ptp_telecom_network_option_I):
        switch (clockClass) {
        case 84:
            *QL = _bcm_ptp_telecom_QL_I_PRC;
            break;
        case 90:
            *QL = _bcm_ptp_telecom_QL_I_SSUA;
            break;
        case 96:
            *QL = _bcm_ptp_telecom_QL_I_SSUB;
            break;
        case 104:
            *QL = _bcm_ptp_telecom_QL_I_SEC;
            break;
        case 110:
            *QL = _bcm_ptp_telecom_QL_I_DNU;
            break;
        default:
            *QL = _bcm_ptp_telecom_QL_INVALID;
            return BCM_E_PARAM;
        }
        break;
    case (_bcm_ptp_telecom_network_option_II):
        switch (clockClass) {
        case 80:
            *QL = _bcm_ptp_telecom_QL_II_PRS;
            break;
        case 82:
            *QL = _bcm_ptp_telecom_QL_II_STU;
            break;
        case 86:
            *QL = _bcm_ptp_telecom_QL_II_ST2;
            break;
        case 90:
            *QL = _bcm_ptp_telecom_QL_II_TNC;
            break;
        case 100:
            *QL = _bcm_ptp_telecom_QL_II_ST3E;
            break;
        case 102:
            *QL = _bcm_ptp_telecom_QL_II_ST3;
            break;
        case 106:
            *QL = _bcm_ptp_telecom_QL_II_SMC;
            break;
        case 108:
            *QL = _bcm_ptp_telecom_QL_II_PROV;
            break;
        case 110:
            *QL = _bcm_ptp_telecom_QL_II_DUS;
            break;
        default:
            *QL = _bcm_ptp_telecom_QL_INVALID;
            return BCM_E_PARAM;
        }
        break;
    case (_bcm_ptp_telecom_network_option_III):
        switch (clockClass) {
        case 82:
            *QL = _bcm_ptp_telecom_QL_III_UNK;
            break;
        case 104:
            *QL = _bcm_ptp_telecom_QL_III_SEC;
            break;
        default:
            *QL = _bcm_ptp_telecom_QL_INVALID;
            return BCM_E_PARAM;
        }
        break;
    default:
        *QL = _bcm_ptp_telecom_QL_INVALID;
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_profile_enabled_set
 * Purpose:
 *      Set telecom profile enabled Boolean.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      enabled   - (IN) Telecom profile enabled Boolean.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_profile_enabled_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 enabled)
{
    int rv;
    int i;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_TELECOM_PROFILE_ENABLED_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;

    bcm_ptp_port_identity_t portid;
    bcm_ptp_clock_info_t ci;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    /*
     * Make payload.
     *    Octet 0...5 : Custom management message key/identifier.
     *                  BCM<null><null><null>.
     *    Octet 6     : Telecom profile enabled Boolean.
     *    Octet 7     : Reserved.
     */
    sal_memcpy(payload, "BCM\0\0\0", 6);
    i = 6;
    payload[i] = enabled ? 1:0;

    if (BCM_FAILURE(rv = bcm_esw_ptp_clock_port_identity_get(unit, ptp_id,
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_esw_ptp_clock_port_identity_get()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_TELECOM_PROFILE_ENABLED,
            payload, PTP_MGMTMSG_PAYLOAD_TELECOM_PROFILE_ENABLED_SIZE_OCTETS,
            resp, &resp_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
        return rv;
    }

    /*
     * Start telecom controller DPC iff telecom profile is enabled.
     * NB: Cancel prior telecom controller DPC process (if any).
     */
    sal_dpc_cancel(INT_TO_PTR(&_bcm_ptp_telecom_controller));
    if (enabled) {
        sal_dpc_time(PTP_TELECOM_CONTROLLER_DPC_TIME_USEC_DEFAULT,
                     &_bcm_ptp_telecom_controller, INT_TO_PTR(&_bcm_ptp_telecom_controller),
                     INT_TO_PTR(unit), INT_TO_PTR(ptp_id), INT_TO_PTR(clock_num), 0);
    }

    /* Update clock cache. */
    if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_info_get(unit, ptp_id,
            clock_num, &ci))) {
        PTP_ERROR_FUNC("_bcm_ptp_clock_cache_info_get()");
        return rv;
    }

    if (enabled) {
        SHR_BITSET(&ci.flags, PTP_CLOCK_FLAGS_TELECOM_PROFILE_BIT);
    } else {
        SHR_BITCLR(&ci.flags, PTP_CLOCK_FLAGS_TELECOM_PROFILE_BIT);
    }

    if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_info_set(unit, ptp_id,
            clock_num, ci))) {
        PTP_ERROR_FUNC("_bcm_ptp_clock_cache_info_set()");
        return rv;
    }

    /*
     * Manage signaling module behavior(s) and configurable PTP attributes
     * based on the active PTP profile.
     */
    if (BCM_FAILURE(rv = _bcm_ptp_signaling_manager(unit, ptp_id, clock_num,
            SHR_BITGET(&ci.flags, PTP_CLOCK_FLAGS_TELECOM_PROFILE_BIT)))) {
        PTP_ERROR_FUNC("_bcm_ptp_signaling_manager()");
        return rv;
    }

    

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_controller
 * Purpose:
 *      Run telecom profile controller (as DPC).
 * Parameters:
 *      owner         - (IN) DPC owner.
 *      arg_unit      - (IN) Unit number (as void*).
 *      arg_ptp_id    - (IN) PTP stack ID (as void*).
 *      arg_clock_num - (IN) PTP clock number (as void*).
 *      unused        - (IN) Unused.
 * Returns:
 *      None.
 * Notes:
 */
static void
_bcm_ptp_telecom_controller(
    void *owner,
    void *arg_unit,
    void *arg_ptp_id,
    void *arg_clock_num,
    void *unused)
{
    int rv;

    int unit = (size_t)arg_unit;
    bcm_ptp_stack_id_t ptp_id = (bcm_ptp_stack_id_t)(size_t)arg_ptp_id;
    int clock_num = (size_t)arg_clock_num;
    bcm_ptp_clock_info_t ci;

    sal_usecs_t dpc_recall_usec = PTP_TELECOM_CONTROLLER_DPC_TIME_USEC_DEFAULT;

    if (_bcm_ptp_telecom_disable == telecom.network_option) {
        /*
         * Telecom controller DPC should not be running.
         * (Shutdown with data consistency enforcement.)
         */
        if (BCM_FAILURE(rv = _bcm_ptp_telecom_network_option_set(unit, ptp_id, clock_num,
                _bcm_ptp_telecom_disable))) {
            PTP_ERROR_FUNC("_bcm_ptp_telecom_network_option_set()");
        }
        return;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id,
            clock_num, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        dpc_recall_usec = PTP_TELECOM_CONTROLLER_DPC_TIME_USEC_IDLE;
        goto telecom_dpc_recall;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_info_get(unit, ptp_id,
            clock_num, &ci))) {
        PTP_ERROR_FUNC("_bcm_ptp_clock_cache_info_get()");
        dpc_recall_usec = PTP_TELECOM_CONTROLLER_DPC_TIME_USEC_IDLE;
        goto telecom_dpc_recall;
    }

    if (0 == ci.slaveonly) {
        dpc_recall_usec = PTP_TELECOM_CONTROLLER_DPC_TIME_USEC_IDLE;
        goto telecom_dpc_recall;
    }

    if ((rv = _bcm_ptp_mutex_take(telecom_mutex, PTP_TELECOM_MUTEX_TIMEOUT_USEC))) {
        PTP_ERROR_FUNC("_bcm_ptp_mutex_take()");
        goto telecom_dpc_recall;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_fmds_processor(unit,
            ptp_id, clock_num))) {
        PTP_ERROR_FUNC("_bcm_ptp_telecom_packet_master_fmds_processor()");
        goto telecom_dpc_release;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_telecom_master_selector())) {
        /* PTP_ERROR_FUNC("_bcm_ptp_telecom_master_selector()"); */
        goto telecom_dpc_release;
    }

    if (telecom.previous_GM != telecom.selected_GM) {
        /* GM changed - update ToP acceptable master table (AMT). */
        if (BCM_FAILURE(rv = _bcm_ptp_telecom_AMT_set(unit, ptp_id, clock_num,
                PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
            PTP_ERROR_FUNC("_bcm_ptp_telecom_AMT_set()");
            goto telecom_dpc_release;
        }
        telecom.previous_GM = telecom.selected_GM;
    }

telecom_dpc_release:
    _bcm_ptp_mutex_give(telecom_mutex);

telecom_dpc_recall:
    sal_dpc_time(dpc_recall_usec, &_bcm_ptp_telecom_controller,
                 INT_TO_PTR(&_bcm_ptp_telecom_controller),
                 arg_unit, arg_ptp_id, arg_clock_num, 0);

    return;
}

/*
 * Function:
 *      _bcm_ptp_telecom_AMT_set
 * Purpose:
 *      Set acceptable master table (AMT), which provides the mechanism to use
 *      the best packet master for frequency synchronization.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      ptp_id     - (IN) PTP stack ID.
 *      clock_num  - (IN) PTP clock number.
 *      clock_port - (IN) PTP clock port number.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_AMT_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port)
{
    int rv;
    int i;

    uint8 payload[128] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS; 

    bcm_ptp_port_identity_t portid;
    int addr_len = 0;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            clock_port))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    

    /* Set AMT. */

    /*
     * Make payload.
     *    0...1   : Number of acceptable master table entries.
     *    2...3   : AMT#0 network protocol.
     *    4...5   : AMT#0 address length.
     *    6...N-1 : AMT#0 address.
     *    N       : AMT#0 alternatePriority1.
     *    N+1     : Reserved.
     */
    i = 0;
    _bcm_ptp_uint16_write(payload, 1);
    i += sizeof(uint16);

    _bcm_ptp_uint16_write(payload + i, PACKET_MASTER_BEST.address.addr_type);
    i += sizeof(uint16);

    addr_len = _bcm_ptp_addr_len((int)PACKET_MASTER_BEST.address.addr_type);
    if (0 == addr_len) {
        return BCM_E_PARAM;
    }

    _bcm_ptp_uint16_write(payload + i, addr_len);
    i += sizeof(uint16);
    sal_memcpy(payload + i, PACKET_MASTER_BEST.address.address, addr_len);
    i += addr_len;
    
    payload[i++] = PACKET_MASTER_BEST.grandmasterPriority1;
    payload[i++] = 0x00;

    if (BCM_FAILURE(rv = bcm_esw_ptp_clock_port_identity_get(unit, ptp_id, 
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_esw_ptp_clock_port_identity_get()");
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num,
            &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_ACCEPTABLE_MASTER_TABLE,
            payload, i, resp, &resp_len))) {
        return rv;
    }

    /* Enable AMT. */

    /*
     * Make payload.
     *    Octet 0 : Acceptable Master table enabled (EN) Boolean.
     *              |B7|B6|B5|B4|B3|B2|B1|B0| = |0|0|0|0|0|0|0|EN|.
     *    Octet 1 : Reserved.
     */
    payload[0] = 0x01;
    payload[1] = 0x00;

    if (BCM_FAILURE(rv = bcm_esw_ptp_clock_port_identity_get(unit, ptp_id, 
            clock_num, clock_port, &portid))) {
        PTP_ERROR_FUNC("bcm_esw_ptp_clock_port_identity_get()");
        return rv;
    }

    rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num, &portid,
            PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_ACCEPTABLE_MASTER_TABLE_ENABLED,
            payload, PTP_MGMTMSG_PAYLOAD_MIN_SIZE_OCTETS,
            resp, &resp_len);

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_telecom_master_selector
 * Purpose:
 *      Choose a master clock via telecom profile master selection process.
 * Parameters:
 *      NONE
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Ref. ITU-T G.8265.1, Clause 6.7.3.
 */
static int
_bcm_ptp_telecom_master_selector(
    void)
{
    int el;
    uint8 unknownGM = 0;

    /* Update PTSF properties. */
    _bcm_ptp_telecom_PTSF_set();

    /* Update QL-degradation event properties. */
    _bcm_ptp_telecom_degradeQL_set();

    /* Update wait-to-restore. */
    _bcm_ptp_telecom_wait_to_restore_set();

    if (_bcm_ptp_telecom_pktmaster_state_UNUSED == PACKET_MASTER_BEST.state ||
        1 == PACKET_MASTER_BEST.selector.lockout ||
        1 == PACKET_MASTER_BEST.selector.wait_to_restore ||
        1 == PACKET_MASTER_BEST.PTSF.lossAnnounce ||
        1 == PACKET_MASTER_BEST.PTSF.lossSync ||
        1 == PACKET_MASTER_BEST.PTSF.unusable ||
        1 == PACKET_MASTER_BEST.hQL.degradeQL) {
        /* Prior best master is no longer valid. */
        unknownGM = 1;
    }

    for (el = 0; el < PTP_TELECOM_MAX_NUMBER_PKTMASTERS; ++el) {
        if (_bcm_ptp_telecom_pktmaster_state_UNUSED == PACKET_MASTER(el).state ||
            1 == PACKET_MASTER(el).selector.lockout ||
            1 == PACKET_MASTER(el).selector.wait_to_restore ||
            1 == PACKET_MASTER(el).PTSF.lossAnnounce ||
            1 == PACKET_MASTER(el).PTSF.lossSync ||
            1 == PACKET_MASTER(el).PTSF.unusable ||
            1 == PACKET_MASTER(el).hQL.degradeQL) {
            /* Packet master does not meet master selector criteria. */
            continue;
        }

        if (1 == unknownGM) {
            unknownGM = 0;
            telecom.selected_GM = el;
        }

        if (PACKET_MASTER(el).clockClass < PACKET_MASTER_BEST.clockClass) {
            /*
             * Higher quality level (QL) than currently selected packet master.
             * NOTE : ITU-T G.781 quality level (QL) is inversely proportional
             *        to PTP clockClass attribute. Ref. ITU-T G.8265.1, Clause
             *        6.7.3.1, Table 1.
             */
            telecom.selected_GM = el;
        } else if (PACKET_MASTER(el).clockClass == PACKET_MASTER_BEST.clockClass) {
            if (PACKET_MASTER(el).priority.value >
                PACKET_MASTER_BEST.priority.value) {
                /* Higher priority than currently selected packet master. */
                telecom.selected_GM = el;
            }
        }

    }

    if (1 == unknownGM) {
        return BCM_E_UNAVAIL;
    } else {
        return BCM_E_NONE;
    }

}

/*
 * Function:
 *      _bcm_ptp_telecom_PTSF_set
 * Purpose:
 *      Set packet timing signal fail (PTSF) properties of in-use packet
 *      master array entries.
 * Parameters:
 *      NONE
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_PTSF_set(
    void)
{
    int rv;
    int el;

    for (el = 0; el < PTP_TELECOM_MAX_NUMBER_PKTMASTERS; ++el) {
        if (_bcm_ptp_telecom_pktmaster_state_UNUSED == PACKET_MASTER(el).state) {
            continue;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_PTSF_set(
                &PACKET_MASTER(el)))) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_PTSF_set
 * Purpose:
 *      Set packet timing signal fail (PTSF) properties.
 * Parameters:
 *      pktmaster - (IN/OUT) Packet master.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_packet_master_PTSF_set(
    _bcm_ptp_telecom_pktmaster_t *pktmaster)
{
    if (NULL == pktmaster ||
        _bcm_ptp_telecom_pktmaster_state_UNUSED == pktmaster->state) {
        return BCM_E_UNAVAIL;
    }

    /*
     * PTSF lossAnnounce?
     * Test ITU-T G.8265.1 criteria on non-receipt of PTP announce messages,
     * indicating loss of channel traceability information. 
     */
    pktmaster->PTSF.lossAnnounce =
        (pktmaster->elapsed_times.announce > telecom.receipt_timeouts.announce) ? 1:0;

    /*
     * PTSF lossSync?
     * Test ITU-T G.8265.1 criteria on non-receipt of PTP timing messages,
     * indicating loss of packet timing signal.
     */
    pktmaster->PTSF.lossTimingSync =
        (pktmaster->elapsed_times.sync > telecom.receipt_timeouts.sync) ? 1:0;

    pktmaster->PTSF.lossTimingSyncTS =
        (pktmaster->elapsed_times.syncTS > telecom.receipt_timeouts.sync) ? 1:0;

    pktmaster->PTSF.lossTimingDelay =
        (pktmaster->elapsed_times.delayResp > telecom.receipt_timeouts.delayResp) ? 1:0;

    /*
     * One-way operation.
     * NOTE: Bypass PTSF analysis of delayResp messages.
     * Delay request and delay response messages are not exchanged.
     * Firmware overwrites time since last delayResp with all-ones.
     */
    if (pktmaster->elapsed_times.delayResp == PTP_TELECOM_MESSAGE_ELAPSED_TIME_MSEC_UNAVAIL) {
        pktmaster->PTSF.lossTimingDelay = 0;
    }

    pktmaster->PTSF.lossSync = pktmaster->PTSF.lossTimingSync |
                               pktmaster->PTSF.lossTimingSyncTS |
                               pktmaster->PTSF.lossTimingDelay;

    /*
     * PTSF unusable?
     * Reference implementation: apply packet timing statistics criteria,
     * incl. thresholds on packet delay variation (PDV) statistical metrics.
     */
    pktmaster->PTSF.unusable = COMPILER_64_GT(pktmaster->stats.pdv_scaled_allan_var,
                                              telecom.thresholds.pdv_scaled_allan_var) ? 1:0;

    if (0 == pktmaster->fmds_counter) {
        /*
         * Startup/discovery dynamics.
         * Bypass PTSF criteria for wait-to-restore/non-reversion classifications
         * until a packet master is recognized in foreignMasterDS.
         */
         pktmaster->PTSF.timestamp = 0;
         pktmaster->PTSF.counter = 0;
    } else if (pktmaster->PTSF.lossAnnounce || pktmaster->PTSF.lossSync || pktmaster->PTSF.unusable) {
        /*
         * PTSF criteria met?
         * Set PTSF timestamp and PTSF occurence counter.
         */
        pktmaster->PTSF.timestamp = sal_time();
        if (pktmaster->PTSF.counter < ((uint32)-1)) {
            ++pktmaster->PTSF.counter;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_degradeQL_set
 * Purpose:
 *      Set QL-degradation event properties of in-use packet master array entries.
 * Parameters:
 *      NONE
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_degradeQL_set(
    void)
{
    int rv;
    int el;

    for (el = 0; el < PTP_TELECOM_MAX_NUMBER_PKTMASTERS; ++el) {
        if (_bcm_ptp_telecom_pktmaster_state_UNUSED == PACKET_MASTER(el).state) {
            continue;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_degradeQL_set(
                &PACKET_MASTER(el)))) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_degradeQL_set
 * Purpose:
 *      Set QL-degradation event properties.
 * Parameters:
 *      pktmaster - (IN/OUT) Packet master.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_packet_master_degradeQL_set(
    _bcm_ptp_telecom_pktmaster_t *pktmaster)
{
    if (NULL == pktmaster ||
        _bcm_ptp_telecom_pktmaster_state_UNUSED == pktmaster->state) {
        return BCM_E_UNAVAIL;
    }

    /* 
     * QL degradation?
     * NOTE: An increase of PTP clockClass over consecutive queries of foreign
     *       master dataset is only condition presently considered. Additional
     *       criteria might be worthwhile in subsequent implementations.
     */
    pktmaster->hQL.degradeQL =
        (pktmaster->hQL.clockClass_window.riseEvents) ? 1:0;

    if (0 == pktmaster->fmds_counter) {
        /*
         * Startup/discovery dynamics.
         * Bypass QL degradation criteria for wait-to-restore/non-reversion classifications
         * until a packet master is recognized in foreignMasterDS.
         */
        pktmaster->hQL.timestamp = 0;
        pktmaster->hQL.counter = 0;
    } else if (pktmaster->hQL.degradeQL) {
        /*
         * QL degradation criteria met?
         * Set QL-degradation event timestamp and occurence counter.
         */
        pktmaster->hQL.timestamp = sal_time();
        if (pktmaster->hQL.counter < ((uint32)-1)) {
            ++pktmaster->hQL.counter;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_wait_to_restore_set
 * Purpose:
 *      Set wait-to-restore attribute of in-use packet master array entries.
 * Parameters:
 *      NONE
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_wait_to_restore_set(
    void)
{
    int rv;
    int el;

    for (el = 0; el < PTP_TELECOM_MAX_NUMBER_PKTMASTERS; ++el) {
        if (_bcm_ptp_telecom_pktmaster_state_UNUSED == PACKET_MASTER(el).state) {
            continue;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_wait_to_restore_set(
                &PACKET_MASTER(el)))) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_wait_to_restore_set
 * Purpose:
 *      Set wait-to-restore attribute.
 * Parameters:
 *      pktmaster - (IN/OUT) Packet master.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_packet_master_wait_to_restore_set(
    _bcm_ptp_telecom_pktmaster_t *pktmaster)
{
    sal_time_t t = sal_time();

    if (NULL == pktmaster || _bcm_ptp_telecom_pktmaster_state_UNUSED == pktmaster->state) {
        return BCM_E_UNAVAIL;
    }

    if (pktmaster->selector.non_reversion && (pktmaster->PTSF.counter || pktmaster->hQL.counter)) {
        /*
         * Non-reversion mode.
         * Packet-master PTSF or QL degradation has occurred at least once.
         * Prevent selection of master via TRUE wait-to-restore setting.
         */
        pktmaster->selector.wait_to_restore = 1;
    } else if ((t - pktmaster->PTSF.timestamp < pktmaster->selector.wait_sec) ||
               (t - pktmaster->hQL.timestamp < pktmaster->selector.wait_sec)) {
        /* Elapsed-time criterion.
         * Time since most recent PTSF or QL-degradation event is less than
         * prescribed wait-to-restore duration. Prevent selection of master
         * via TRUE wait-to-restore setting.
         */
        pktmaster->selector.wait_to_restore = 1;
    } else {
        pktmaster->selector.wait_to_restore = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_fmds_processor
 * Purpose:
 *      Query foreign master dataset and assign attributes, data, and derived
 *      quantities to corresponding packet master entries.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_packet_master_fmds_processor(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rv;
    int i, el;

    _bcm_ptp_foreign_master_dataset_t FMds;
    _bcm_ptp_foreign_master_entry_t *fm;
    _bcm_ptp_port_state_t portState;

    if (BCM_FAILURE(rv = _bcm_esw_ptp_foreign_master_dataset_get(unit, ptp_id,
            clock_num, PTP_CLOCK_PORT_NUMBER_DEFAULT, &FMds))) {
        return rv;
    }

    /*
     * "Clear" packet master state, message receipt times, etc. to account for
     * the foreignMasterDS firmware logic to dynamically remove foreign master
     * entries.
     */
    for (el = 0; el < PTP_TELECOM_MAX_NUMBER_PKTMASTERS; ++el) {
        if (_bcm_ptp_telecom_pktmaster_state_UNUSED != PACKET_MASTER(el).state) {
            PACKET_MASTER(el).state = _bcm_ptp_telecom_pktmaster_state_INIT;

            PACKET_MASTER(el).elapsed_times.announce = PTP_TELECOM_MESSAGE_ELAPSED_TIME_MSEC_MAX;
            PACKET_MASTER(el).elapsed_times.sync = PTP_TELECOM_MESSAGE_ELAPSED_TIME_MSEC_MAX;
            PACKET_MASTER(el).elapsed_times.syncTS = PTP_TELECOM_MESSAGE_ELAPSED_TIME_MSEC_MAX;
            PACKET_MASTER(el).elapsed_times.delayResp = PTP_TELECOM_MESSAGE_ELAPSED_TIME_MSEC_MAX;

            COMPILER_64_ALLONES(PACKET_MASTER(el).stats.pdv_scaled_allan_var);
        }
    }

    /* Update packet master attributes. */
    for (i = 0; i < FMds.num_foreign_masters; ++i) {
        fm = &FMds.foreign_master[i]; 
        if (BCM_FAILURE(rv = _bcm_ptp_telecom_packet_master_lookup(&fm->address, &el))) {
            PTP_ERROR_FUNC("_bcm_ptp_telecom_packet_master_lookup()");
            continue;
        }

        /*
         * Update foreignMasterDS entry counter for this packet master to keep
         * track of number of times it has shown up in foreignMasterDS queries.
         * Do not rollover.
         */
        if (PACKET_MASTER(el).fmds_counter < ((uint32)-1)) {
            ++PACKET_MASTER(el).fmds_counter;
        }

        PACKET_MASTER(el).portIdentity = fm->port_identity;

        PACKET_MASTER(el).clockClass = fm->clockClass;
        _bcm_ptp_telecom_map_clockClass_QL(PACKET_MASTER(el).clockClass, &PACKET_MASTER(el).QL);

        _bcm_ptp_telecom_packet_master_priority_calculator(
            fm->grandmasterPriority1, fm->grandmasterPriority2, &PACKET_MASTER(el));

        /*
         * Extract basic clockClass statistics accummulated over foreignMasterDs
         * query window to facilitate an appraisal of reduction in QL conditions.
         */
        PACKET_MASTER(el).hQL.clockClass_window = fm->clockClass_window;

        /*
         * Extract elapsed times since receipt of messages from foreignMasterDS.
         * NOTE: A master in foreignMasterDS implies PTSF lossAnnounce is false.
         *       Set elapsed time of last announce message to zero to guarantee.
         */
        PACKET_MASTER(el).elapsed_times.announce = 0;
        PACKET_MASTER(el).elapsed_times.sync = fm->ms_since_sync;
        PACKET_MASTER(el).elapsed_times.syncTS = fm->ms_since_sync_ts;
        PACKET_MASTER(el).elapsed_times.delayResp = fm->ms_since_del_resp;

        /*
         * Zero elapsed times since receipt of Sync, Follow_Up (Sync timestamp), and Delay_Resp
         * if portState is neither slave nor uncalibrated for consistency with firmware Rx/Tx.
         */
        if (BCM_SUCCESS(_bcm_ptp_clock_cache_port_state_get(unit, ptp_id, clock_num,
                PTP_CLOCK_PORT_NUMBER_DEFAULT, &portState)) &&
                (portState != _bcm_ptp_state_uncalibrated) && (portState != _bcm_ptp_state_slave)) {
            PACKET_MASTER(el).elapsed_times.sync = 0;
            PACKET_MASTER(el).elapsed_times.syncTS = 0;
            PACKET_MASTER(el).elapsed_times.delayResp = 0;
        }

        COMPILER_64_SET(PACKET_MASTER(el).stats.pdv_scaled_allan_var,
            COMPILER_64_HI(fm->pdv_scaled_allan_var),
            COMPILER_64_LO(fm->pdv_scaled_allan_var));

        PACKET_MASTER(el).state = _bcm_ptp_telecom_pktmaster_state_VALID;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_priority_calculator
 * Purpose:
 *      Calculate packet master priority based on associated grandmaster's
 *      priority1 and priority2 attributes.
 * Parameters:
 *      gmPriority1 - (IN) PTP grandmaster clock priority1.
 *      gmPriority2 - (IN) PTP grandmaster clock priority2.
 *      pktmaster   - (IN/OUT) Packet master clock.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_packet_master_priority_calculator(
    const uint8 gmPriority1,
    const uint8 gmPriority2,
    _bcm_ptp_telecom_pktmaster_t *pktmaster)
{
    pktmaster->grandmasterPriority1 = gmPriority1;
    pktmaster->grandmasterPriority2 = gmPriority2;

    if (1 == pktmaster->priority.override) {
        /*
         * Packet master priority calculated automatically from PTP priority1
         * and priority2 attributes, which are available in announce messages
         * for example. Otherwise (i.e., override equals FALSE), the priority
         * is prescribed via host API calls.
         */
        pktmaster->priority.value = 0xffff - (gmPriority1 << 8) - gmPriority2;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_manager
 * Purpose:
 *      Manage/assign packet master array resources and identify/lookup the
 *      entry with matching address.
 * Parameters:
 *      address - (IN)  Packet master address.
 *      index   - (OUT) Packet master index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_packet_master_manager(
    bcm_ptp_clock_port_address_t *address,
    int *index)
{
    int rv;
    int el;

    /* Existing packet master array entry. */
    if (BCM_SUCCESS(rv = _bcm_ptp_telecom_packet_master_lookup(address, index))) {
        return BCM_E_NONE;
    }

    /* New packet master array entry. */
    for (el = 0; el < PTP_TELECOM_MAX_NUMBER_PKTMASTERS; ++el) {
        if (_bcm_ptp_telecom_pktmaster_state_UNUSED == PACKET_MASTER(el).state) {
            *index = el;
            return BCM_E_NONE;
        }
    }

    /* No available packet master array entries. */
    *index = -1;
    return BCM_E_FULL;
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_lookup
 * Purpose:
 *      Look up a packet master in the packet master array based on address.
 * Parameters:
 *      address - (IN)  Packet master address.
 *      index   - (OUT) Packet master array index.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_packet_master_lookup(
    bcm_ptp_clock_port_address_t *address,
    int *index)
{
    int el;

    /* Existing packet master array entry. */
    for (el = 0; el < PTP_TELECOM_MAX_NUMBER_PKTMASTERS; ++el) {
        if (_bcm_ptp_telecom_pktmaster_state_UNUSED != PACKET_MASTER(el).state) {
            /* Scan packet master array for entry with matching address. */
            if (_bcm_ptp_port_address_compare(address, &PACKET_MASTER(el).address)) {
                /* Select corresponding packet master. */
                *index = el;
                return BCM_E_NONE;
            }
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_ptp_telecom_packet_master_make
 * Purpose:
 *      Initialize new packet master.
 * Parameters:
 *      address   - (IN)  Packet master address.
 *      pktmaster - (OUT) Packet master.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
_bcm_ptp_telecom_packet_master_make(
    bcm_ptp_clock_port_address_t *address,
    _bcm_ptp_telecom_pktmaster_t *pktmaster)
{
    if (NULL == pktmaster) {
        return BCM_E_FAIL;
    }

    /* Set address. */
    pktmaster->address = *address;

    /* Set state. */
    pktmaster->state = _bcm_ptp_telecom_pktmaster_state_INIT;

    /* Set attributes. */
    pktmaster->priority.override = 1;
    _bcm_ptp_telecom_packet_master_priority_calculator(
        PTP_CLOCK_PRESETS_PRIORITY1_MAXIMUM, PTP_CLOCK_PRESETS_PRIORITY2_MAXIMUM,
        pktmaster);

    return BCM_E_NONE;
}

#endif /* defined(INCLUDE_PTP) */
