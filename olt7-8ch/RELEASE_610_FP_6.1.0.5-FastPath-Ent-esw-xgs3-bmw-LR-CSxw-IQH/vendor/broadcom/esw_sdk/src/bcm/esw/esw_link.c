/*
 * $Id: esw_link.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 *
 * Link Scan
 *
 * Linkscan should always run for all chips in a system. It manages
 * the current chip LINK state (EPC_LINK or equivalent), and programs
 * MACs to match auto-negotiated links. 
 *
 * Linkscan is also responsible for determining the link up/down
 * status for a port. Since link down events signaled by most PHYs are
 * latched low and cleared on read, it is important that accesses to
 * to the PHY itself be managed carefully. Linkscan is intended to be
 * the only process that reads the actual PHY (using
 * _bcm_port_link_get). All other calls to retrieve link status
 * results in calls to _bcm_link_get which returns the linkscan view
 * of the link status. This ensures linkscan is the only process that
 * reads the actual PHYs.
 *
 * All modifications to the linkscan state are protected by LC_LOCK.
 *
 * Linkscan maintains the following port bitmaps
 *
 *     lc_pbm_link:
 *                 Current physical link up/down status. When a bit
 *                 in this mask is set, a link up or link down event
 *                 is recognized and signaled to any registered
 *                 handlers.
 *
 *     lc_pbm_link_change:
 *                 Mask of ports that need to recognize a link
 *                 down event. Ports are added to this mask by the
 *                 function bcm_link_change. 
 *
 *     lc_pbm_override_ports:
 *                 Bit map of ports that are currently
 *                 being explicitly set to a value. These actual value
 *                 is determined by lb_pbm_override_link. Ports are
 *                 added and removed from this mode by the routine
 *                 _bcm_link_force. 
 *
 *                 Ports that are forced to an UP state do NOT result
 *                 in a call to bcm_port_update. It is the
 *                 responsibility of the caller to configure the
 *                 correct MAC and PHY state.
 *
 *     lc_pbm_override_link:
 *                 Bit map indicating the link-up/link-down
 *                 status for those ports with override set.
 *
 *     lc_pbm_sgmii_autoneg_port:
 *                 Port that is configured in SGMII autoneg mode based
 *                 on spn_PHY_SGMII_AUTONEG. Maintaining this bitmap 
 *                 avoids the overhead of the soc_property_port_get call. 
 *
 * Calls to _bcm_link_get always returns the current status as
 * indicated by lc_pbm_link. 
 *
 * For 10G+ ports, the an additional bitmap is used:
 *
 *     lc_pbm_remote_fault:
 *                 Bit map indicating the fault (local or remote) status of
 *                 a link-up port.
 *
 * To track valid fault status, the physical link must be up and
 * the port configured so that symbols reach the MAC.  Thus, physical
 * port configuration is tracked by the lc_pbm_link state.  However,
 * if the port is receiving a fault indication, we do not want
 * to allow packet traffic.  Thus, the logical port is not yet up.
 * The logical port up is what we want to return to _bcm_link_get,
 * so we fold these two states together.  Tracking the fault
 * state requires a bit more work in the _bcm_esw_linkscan_update_port
 * logic.
 *
 * If LAG failover capability is configured through the trunk module,
 * then three more bitmaps are used:
 *
 *     lc_pbm_failover:
 *                 Bit map of ports configured for LAG failover.
 *
 *     lc_pbm_failed:
 *                 Bit map of ports configured for LAG failover which
 *                 have experienced a link down or fault and
 *                 are now in BCM_PORT_LINK_STATUS_FAILED state.  These
 *                 ports require application intervention to leave
 *                 the FAILED state.
 *
 *     lc_pbm_failed_clear:
 *                 Bit map of ports which were previously in FAILED state
 *                 but have been cleared by application intervention and
 *                 are now in DOWN state (due to link down or fault).
 *                 A port remains in this set until the next link up
 *                 without fault, at which point the hardware
 *                 sequence to remove the failover state may complete.
 *
 */

#include <sal/types.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>

#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/phyctrl.h>

#include <bcm/error.h>
#include <bcm/link.h>

/* #include <bcm_int/common/link.h> */
#include <bcm_int/esw/link.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/common/lock.h>

#include <bcm_int/esw_dispatch.h> 

typedef struct ls_handler_s {
    struct ls_handler_s         *lh_next;
    bcm_linkscan_handler_t      lh_f;
} ls_handler_t;

typedef struct ls_errstate_s {
    int			limit;		/* # errors to enter error state */
    int			delay;		/* Length of error state in seconds */
    int			count;		/* # of errors so far */
    int			wait;		/* Boolean, TRUE when in error state */
    sal_usecs_t		time;		/* Time error state was entered */
} ls_errstate_t;

typedef struct ls_cntl_s {
    char		lc_taskname[16];
    sal_mutex_t         lc_lock;        /* Synchronization */
    pbmp_t              lc_pbm_hw;      /* Hardware link scan ports */
    pbmp_t              lc_pbm_sw;      /* Software link scan ports */
    pbmp_t              lc_pbm_sgmii_autoneg_port; /* Ports with SGMII autoneg */
    int                 lc_hw_change;   /* HW Link state has changed */
    ls_handler_t        *lc_handler;    /* List of handlers */
    VOL int             lc_us;          /* Time between scans (us) */
    VOL sal_thread_t    lc_thread;      /* Link scan thread */
    sal_sem_t           lc_sema;        /* Link scan semaphore */
    ls_errstate_t	lc_error[SOC_MAX_NUM_PORTS];	/* Link error state */
    bcm_linkscan_port_handler_t lc_f[SOC_MAX_NUM_PORTS]; /* Handler for link fn */

} ls_cntl_t;

/*
 * Define:
 *	LC_LOCK/LC_UNLOCK
 * Purpose:
 *	Serialization Macros for access to lc_cntl structure.
 */

#define LC_LOCK(unit) \
        sal_mutex_take(link_control[unit]->lc_lock, sal_mutex_FOREVER)

#define LC_UNLOCK(unit) \
        sal_mutex_give(link_control[unit]->lc_lock)

#define LC_CHECK_INIT(unit) \
        if (link_control[unit] == NULL) { \
            return(BCM_E_INIT); \
        }

/* Define this if you want to know the time take to prcess link up/down event.
 * This is *not* the time between actual HW link up/down and software link 
 * up/down event.
 * The precision is limited to the VxWorks clock period (typically 10000 usec.)
 */
#define BCM_LINK_CHANGE_BENCHMARK

/*
 * Variable:
 *	link_control
 * Purpose:
 *	Hold per-unit global status for linkscan
 */

STATIC ls_cntl_t       *link_control[BCM_MAX_NUM_UNITS];

#if defined(BCM_HAWKEYE_SUPPORT)
#define PHY_AN_TX_PAUSE_CAP    (0x01)
#define PHY_AN_RX_PAUSE_CAP    (0x02)
#define HK_PAUSE_WAR_PHY_REG   (0x19)
#endif /* BCM_HAWKEYE_SUPPORT */

/*
 * Function:
 *	_bcm_link_gport_resolve
 * Purpose:
 *	Decode gport for linkscan module, only local ports should be encoded.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      gport - gport, destination encoded
 *      local_port - [OUT] local port encoded into gport
 * Returns:
 *	BCM_E_XXX 
 */

STATIC int
_bcm_link_gport_resolve(int unit, bcm_gport_t gport, bcm_port_t *local_port)
{
    bcm_module_t    modid; 
    bcm_trunk_t     trunk;
    bcm_port_t      port;
    int             id, ismymodid;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, gport, &modid, &port, &trunk, &id));

    if (-1 != id || BCM_TRUNK_INVALID != trunk) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_modid_is_local(unit, modid, &ismymodid));
    if (ismymodid != TRUE) {
        return BCM_E_PARAM;
    }

    return _bcm_gport_modport_api2hw_map(unit, modid, port, &modid, local_port);
}



/*
 * Function:
 *	_lc_pbm_init
 * Purpose:
 *	Initialize the port bitmaps in the link_control structure
 * Parameters:
 *      unit - StrataSwitch unit #.
 */

static void
_lc_pbm_init(int unit)
{
    soc_persist_t	*sop = SOC_PERSIST(unit);

    /*
     * Force initial scan by setting link change while pretending link
     * was initially up.
     */
    BCM_PBMP_ASSIGN(sop->lc_pbm_link, PBMP_ALL(unit));
    BCM_PBMP_ASSIGN(sop->lc_pbm_link_change, PBMP_PORT_ALL(unit));

    BCM_PBMP_CLEAR(sop->lc_pbm_override_ports);
    BCM_PBMP_CLEAR(sop->lc_pbm_override_link);
    BCM_PBMP_CLEAR(sop->lc_pbm_remote_fault);
    BCM_PBMP_CLEAR(sop->lc_pbm_failover);
    BCM_PBMP_CLEAR(sop->lc_pbm_failed);
    BCM_PBMP_CLEAR(sop->lc_pbm_failed_clear);
}

/*
 * Function:
 *	bcm_esw_linkscan_enable_port_get
 * Purpose:
 *	Determine whether or not linkscan is managing a given port
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port to check.
 * Returns:
 *	BCM_E_NONE - port being scanned
 *	BCM_E_DISABLED - port not being scanned
 */

int
bcm_esw_linkscan_enable_port_get(int unit, bcm_port_t port)
{
    soc_persist_t	*sop = SOC_PERSIST(unit);
    ls_cntl_t           *lc = link_control[unit];

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_link_gport_resolve(unit, port, &port));
    }

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    if (lc == NULL || lc->lc_us == 0 ||
	(!BCM_PBMP_MEMBER(lc->lc_pbm_sw, port) &&
	 !BCM_PBMP_MEMBER(lc->lc_pbm_hw, port) &&
	 !BCM_PBMP_MEMBER(sop->lc_pbm_override_ports, port))) {
	return (BCM_E_DISABLED);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_linkscan_update_asf
 * Purpose:
 *      Update Alternate Store and Forward parameters for a port
 * Parameters:
 *      unit - device
 *      port - port
 *      linkup - port link state (0=down, 1=up)
 *      speed - port speed
 *      duplex - port duplex (0=half, 1=full)
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_linkscan_update_asf(int unit, bcm_port_t port, int linkup,
                             int speed, int duplex)
{
    if (SOC_REG_IS_VALID(unit, ASFPORTSPEEDr)) {
	uint32	asf_reg, oasf_reg, asf_speed;
	int	no_10_100;

	SOC_IF_ERROR_RETURN(READ_ASFPORTSPEEDr(unit, port, &asf_reg));
	oasf_reg = asf_reg;
	if (!linkup || IS_CPU_PORT(unit, port)) {
	    speed = 0;
	}
	no_10_100 = soc_feature(unit, soc_feature_asf_no_10_100);
	switch (speed) {
	case 0:
	    asf_speed = 0;
	    break;
	case 10:
	    if (no_10_100) {
		asf_speed = 0;
	    } else {
		asf_speed = duplex ? 2 : 1;
	    }
	    break;
	case 100:
	    if (no_10_100) {
		asf_speed = 0;
	    } else {
		asf_speed = duplex ? 4 : 3;
	    }
	    break;
	case 1000:
	    asf_speed = 5;
	    break;
	case 10000:
	    asf_speed = 6;
	    break;
	case 12000:
	    asf_speed = 7;
	    break;
	default:
	    asf_speed = 0;
	    break;
	}
	soc_reg_field_set(unit, ASFPORTSPEEDr, &asf_reg,
			  PORTSPEEDf, asf_speed);
	if (asf_reg != oasf_reg) {
	    SOC_IF_ERROR_RETURN(WRITE_ASFPORTSPEEDr(unit, port, asf_reg));
	}
	return BCM_E_NONE;
    }
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
    if (SOC_REG_IS_VALID(unit, ASF_PORT_SPEEDr)) {
	uint32	asf_reg, oasf_reg, asf_speed;
	int	no_10_100;

	SOC_IF_ERROR_RETURN(READ_ASF_PORT_SPEEDr(unit, port, &asf_reg));
	oasf_reg = asf_reg;
	if (!linkup || IS_CPU_PORT(unit, port)) {
	    speed = 0;
	}
	no_10_100 = soc_feature(unit, soc_feature_asf_no_10_100);
	switch (speed) {
	case 0:
	    asf_speed = 0;
	    break;
	case 10:
	    if (no_10_100) {
		asf_speed = 0;
	    } else {
		asf_speed = duplex ? 3 : 2;
	    }
	    break;
	case 100:
	    if (no_10_100) {
		asf_speed = 0;
	    } else {
		asf_speed = duplex ? 5 : 4;
	    }
	    break;
	case 1000:
	    asf_speed = duplex ? 7 : 6;
	    break;
	case 2500:
	    asf_speed = duplex ? 9 : 8;
	    break;
	case 10000:
	    asf_speed = 21;
	    break;
	case 12000:
	case 13000:
	    asf_speed = 25;
	    break;
	case 16000:
	    asf_speed = 27;
	    break;
	case 20000:
	case 21000:
	    asf_speed = 29;
	    break;
	default:
	    asf_speed = 0;
	    break;
	}
	soc_reg_field_set(unit, ASF_PORT_SPEEDr, &asf_reg,
			  ASF_PORT_SPEEDf, asf_speed);
	if (asf_reg != oasf_reg) {
	    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_SPEEDr(unit, port, asf_reg));
	}
	return BCM_E_NONE;
    }
#endif /* BCM_BRADLEY_SUPPORT || BCM_SCORPION_SUPPORT */
    return BCM_E_NONE;
}

#if defined(BCM_LYNX_SUPPORT) || defined(BCM_HERCULES15_SUPPORT) || \
    defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)
/*
 * Function:    
 *      _bcm_esw_link_fault_get
 * Purpose:     
 *      Check for remote and local fault on 10G+ link which is already up.
 * Parameters:  
 *      unit - StrataSwitch unit #.
 *      port - Port to process.
 *      fault - (IN/OUT) Fault status
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_link_fault_get(int unit, int port, int *fault)
{
    uint64 lss;
    int rmt_fault, lcl_fault, speed;
    soc_reg_t reg;

    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, port, &speed));
        if (speed < 10000) {
            /* No fault status to retrieve */
            return BCM_E_NONE;
        }

        if (SOC_IS_LYNX(unit)) {
            /*
             * This will "work" on Lynx A0, which does not have a register
             * by this name. Rather, the resultant register address points
             * to an unused register on Lynx A0, which always returns zero.
             * On Lynx B0 and Lynx 1.5, a real MAC_RXLSSCTRL register is
             * implemented.
             */
            reg = MAC_RXLSSCTRLr;
        } else {
            reg = MAC_RXLSSSTATr;
        }

        SOC_IF_ERROR_RETURN(soc_reg_read(unit, reg,
                                soc_reg_addr(unit, reg, port, 0), &lss));
        rmt_fault = soc_reg64_field32_get(unit, reg, lss, REMOTEFAULTSTATf);
        lcl_fault = soc_reg64_field32_get(unit, reg, lss, LOCALFAULTSTATf);

        if (rmt_fault || lcl_fault) {
            *fault = TRUE;
        }
    }

    return BCM_E_NONE;
}
#endif /* LYNX, HERC15, FIREBOLT, EASYRIDER */  


#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
/*
 * Function:    
 *      _bcm_esw_link_failover_link_up
 * Purpose:     
 *      Perform the recovery sequence to restore a previously failed port
 *      when link up occurs.
 * Parameters:  
 *      unit - StrataSwitch unit #.
 *      port - Port to process.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_link_failover_link_up(int unit, int port)
{
    int failed = TRUE, retries;
    uint32 lag_fail_config, lag_fail_status;
    int failover_loopback_usec;
    soc_timeout_t to;

    failover_loopback_usec = 5000;
    retries = 5;

    BCM_IF_ERROR_RETURN
        (READ_GXPORT_LAG_FAILOVER_CONFIGr(unit, port,
                                          &lag_fail_config));

    /* Unimac may take several tries before clearing the loopback state. */
    while (retries--) {
        soc_reg_field_set(unit, GXPORT_LAG_FAILOVER_CONFIGr,
                          &lag_fail_config, REMOVE_FAILOVER_LPBKf, 0);
        BCM_IF_ERROR_RETURN
            (WRITE_GXPORT_LAG_FAILOVER_CONFIGr(unit, port,
                                               lag_fail_config));
        soc_reg_field_set(unit, GXPORT_LAG_FAILOVER_CONFIGr,
                          &lag_fail_config, REMOVE_FAILOVER_LPBKf, 1);
        BCM_IF_ERROR_RETURN
            (WRITE_GXPORT_LAG_FAILOVER_CONFIGr(unit, port,
                                               lag_fail_config));

        soc_timeout_init(&to, failover_loopback_usec, 0);
        while (!soc_timeout_check(&to)) {
            BCM_IF_ERROR_RETURN
                (READ_GXPORT_LAG_FAILOVER_STATUSr(unit, port,
                                                  &lag_fail_status));
            failed =
                soc_reg_field_get(unit, GXPORT_LAG_FAILOVER_STATUSr,
                                  lag_fail_status, LAG_FAILOVER_LOOPBACKf);
            if (!failed) {
                break;
            }
        }
        if (!failed) {
            break;
        }
    }

    if (failed) {
        return BCM_E_TIMEOUT;
    }

    /* Disable LAG failover until port is returned to trunk */
    soc_reg_field_set(unit, GXPORT_LAG_FAILOVER_CONFIGr,
                      &lag_fail_config, REMOVE_FAILOVER_LPBKf, 0);
    soc_reg_field_set(unit, GXPORT_LAG_FAILOVER_CONFIGr,
                      &lag_fail_config, LAG_FAILOVER_ENf, 0);
    BCM_IF_ERROR_RETURN
        (WRITE_GXPORT_LAG_FAILOVER_CONFIGr(unit, port,
                                           lag_fail_config));

    /* Flush any flow control status to guarantee normal traffic goes
     * to the port first. */
    BCM_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKPr(unit, port, 0));

    soc_cm_debug(DK_LINK | DK_VERBOSE,
                 "Unit %d: LAG Failed port %d status completed\n",
                 unit, port);

    return BCM_E_NONE;
}

/*
 * Function:    
 *      _bcm_esw_link_failover_link_down_force
 * Purpose:     
 *      Force a link down event on a LAG failover enabled port when
 *      a fault is detected.  This will engage the HW failover
 *      mechanism.
 * Parameters:  
 *      unit - StrataSwitch unit #.
 *      port - Port to process.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_link_failover_link_down_force(int unit, int port)
{
    int failed = FALSE;
    uint32 lag_fail_status;
    int failover_loopback_usec;
    soc_timeout_t to;

    BCM_IF_ERROR_RETURN(soc_phyctrl_enable_set(unit, port, FALSE));

    failover_loopback_usec = 5000;
    soc_timeout_init(&to, failover_loopback_usec, 0);
    while (!soc_timeout_check(&to)) {
        BCM_IF_ERROR_RETURN
            (READ_GXPORT_LAG_FAILOVER_STATUSr(unit, port,
                                              &lag_fail_status));
        failed =
            soc_reg_field_get(unit, GXPORT_LAG_FAILOVER_STATUSr,
                              lag_fail_status, LAG_FAILOVER_LOOPBACKf);
        if (failed) {
            break;
        }
    }

    if (!failed) {
        return BCM_E_TIMEOUT;
    }

    /* Turn PHY back on now that we've entered failed state */
    BCM_IF_ERROR_RETURN(soc_phyctrl_enable_set(unit, port, TRUE));

    return BCM_E_NONE;
}

/*
 * Function:    
 *      _bcm_esw_link_failover_port_disable
 * Purpose:     
 *      When a port enters failed state, disable the RX path and
 *      flush the TX FIFO.
 * Parameters:  
 *      unit - StrataSwitch unit #.
 *      port - Port to process.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_link_failover_port_disable(int unit, int port)
{
    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
        /* Can't use standard MAC enable_set because it does too much */
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MAC_CTRLr, port, RXENf, 0));

        /* Flush BigMAC TX FIFO */
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MAC_TXCTRLr, port,
                                    TXFIFO_RESETf, 1));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MAC_TXCTRLr, port,
                                    TXFIFO_RESETf, 0));
    }

    return BCM_E_NONE;
}
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

/*
 * The introduction of fault state complicates the transitions for
 * the linkscan update.  If the link is established, then its fault
 * status is valid.  If the link is down, then the fault status must be
 * false.  Since we track the "current" and "new" states of each, we
 * get this truth table:
 *
 *     Link   Fault
 *     C  N   C  N
 *     -  -   -  -
 * 1)  1  1   1  1   No change
 * 2)  1  1   1  0   Fault cleared, enable logical layer, notify link up
 * 3)  1  1   0  1   Fault detected, disable logical layer, notify link down
 * 4)  1  1   0  0   No change
 * 5)  1  0   1  0   Faulted link down, disable port, no notify
 * 6)  1  0   0  0   No fault link down, disable logical layer and port,
 *                       notify link down
 * 7)  0  1   0  1   Link up to fault state, enable port but not
 *                       logical layer, no notify
 * 8)  0  1   0  0   Link up, enable port and logical layer, notify link up
 * 9)  0  0   0  0   No change
 *
 * Now to the above we add LAG failover.  For LAG failover, when a
 * front-panel port on which trunk failover is configured goes down by
 * either of the definitions above (3 or 6), it is not actually disabled
 * but instead moved to the new FAILED state.  This state remains until
 * explicitly cleared by the application reporting that the FAILED state
 * has been properly handled (failed port removed from trunk groups), via
 * bcm_port_link_failed_clear.  When bcm_port_link_failed_clear is called,
 * link down processing completes (forced if necessary) and the link down
 * status is reported.  When the link goes back up, some final cleanup
 * of the port is performed.  After this point, the port may be put back
 * into any LAGs by the application.
 */

/*
 * Function:    
 *      _bcm_esw_linkscan_update_port
 * Purpose:     
 *      Check for and process a link event on one port
 * Parameters:  
 *      unit - StrataSwitch unit #.
 *      port - Port to process.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_linkscan_update_port(int unit, int port)
{
    soc_persist_t	*sop = SOC_PERSIST(unit);
    ls_cntl_t           *lc = link_control[unit];
    int			cur_link, change, new_link = FALSE;
    bcm_port_info_t	info;
    ls_handler_t	*lh, *lh_next = NULL;
    int			rv, cur_fault, new_fault = FALSE,
                        unforced = FALSE, notify = FALSE, logical_link;
    int                 cur_failed, new_failed = FALSE;
#if defined(BCM_LINK_CHANGE_BENCHMARK)
    sal_usecs_t         time_start = 0;
    sal_usecs_t         time_end = 0;
#endif /* BCM_LINK_CHANGE_BENCHMARK */

#if defined(BCM_HAWKEYE_SUPPORT)
    uint32 mac_data = 0, phy_data = 0;
    uint32 val = 0;
#endif /* BCM_HAWKEYE_SUPPORT */

    assert(SOC_PORT_VALID(unit, port)); 
    /*
     * Current link status is calculated in the following order:
     *   1) If link status is in an override state, the override
     *      state is used.
     *   2) If link is required to recognize a link down event, the
     *      current scan will recognize the link down (if the link
     *      was previously down, this will result in no action
     *      being taken)
     *   3) Use real detected link status.
     *        a) If using Hardware link scanning, use captured H/W
     *           value since the H/W scan will clear the latched
     *           link down event.
     *        b) If using S/W link scanning, retrieve the current
     *           link status from the PHY.
     */

    cur_link = SOC_PBMP_MEMBER(sop->lc_pbm_link, port);
    cur_fault = SOC_PBMP_MEMBER(sop->lc_pbm_remote_fault, port);
    cur_failed = SOC_PBMP_MEMBER(sop->lc_pbm_failed, port);

    change = SOC_PBMP_MEMBER(sop->lc_pbm_link_change, port);
    SOC_PBMP_PORT_REMOVE(sop->lc_pbm_link_change, port);

    if (change) {				          	  /* 2) */
	new_link = FALSE;
	rv = BCM_E_NONE;
    } else if (SOC_PBMP_MEMBER(sop->lc_pbm_override_ports,
			       port)) {				  /* 1) */
	new_link = SOC_PBMP_MEMBER(sop->lc_pbm_override_link, port);
	rv = BCM_E_NONE;
    } else if (SOC_PBMP_MEMBER(lc->lc_pbm_hw, port)) {            /* 3a) */
        if (BCM_PBMP_MEMBER(lc->lc_pbm_sgmii_autoneg_port, port)) {
            rv = _bcm_port_link_get(unit, port, 0, &new_link);
        } else {
            rv = _bcm_port_link_get(unit, port, 1, &new_link);
        }
        unforced = TRUE;
        soc_cm_debug(DK_LINK | DK_VERBOSE, "Unit %d: HW link p=%d %s\n",
                     unit, port, new_link ? "up" : "down");
    } else if (SOC_PBMP_MEMBER(lc->lc_pbm_sw, port)) {            /* 3b) */
        if (lc->lc_f[port]) {
            int state;

            rv = lc->lc_f[port](unit, port, &state);
            if (rv == BCM_E_NONE) {
                new_link = state ? TRUE : FALSE;
            } else if (rv == BCM_E_UNAVAIL) {
                rv = _bcm_port_link_get(unit, port, 0, &new_link);
            }
        } else {
            rv = _bcm_port_link_get(unit, port, 0, &new_link);
        }
        unforced = TRUE;
    } else {
	return BCM_E_NONE;	/* Port not being scanned */
    }

    if (BCM_FAILURE(rv)) {
	soc_cm_debug(DK_ERR,
		     "Port %s: Failed to recover link status: %s\n", 
		     SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
	return rv;
    }

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    /* Check for failed state before fault since it takes priority,
     * and because we may force the link down on clearing the failed state.
     */
    if (soc_feature(unit, soc_feature_port_lag_failover)) {
        if (cur_failed) {
            if (!SOC_PBMP_MEMBER(sop->lc_pbm_failed_clear, port)) {
                new_failed = cur_failed; /* Sticky until cleared */
            } else { /* Clearing failed status */
                new_failed = FALSE;
                new_link = FALSE; /* Force link down on clear */
                SOC_PBMP_PORT_REMOVE(sop->lc_pbm_failed, port);
            }
        } else if (SOC_PBMP_MEMBER(sop->lc_pbm_failover, port)) {
            uint32 lag_fail_status;
            /* Has this port failed? */
            BCM_IF_ERROR_RETURN
                (READ_GXPORT_LAG_FAILOVER_STATUSr(unit, port,
                                                  &lag_fail_status));

            new_failed =
                soc_reg_field_get(unit, GXPORT_LAG_FAILOVER_STATUSr,
                                  lag_fail_status, LAG_FAILOVER_LOOPBACKf);
            if (new_failed) {
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_link_failover_port_disable(unit, port));
                SOC_PBMP_PORT_ADD(sop->lc_pbm_failed, port);
            }
        } /* else, new_failed = FALSE from init above */
    }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

#if defined(BCM_LYNX_SUPPORT) || defined(BCM_HERCULES15_SUPPORT) || \
    defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)
    if (soc_feature(unit, soc_feature_bigmac_fault_stat) &&
        unforced && (cur_link) && (new_link) && !new_failed) {
        /*
         * Check the fault status here only if:
         * 1) The chip supports it.
         * 2) The link state is coming from the PHY (unforced).
         * 3) The link was already up and is still.
         */
        /* Check 10G fault status for change */
        rv = _bcm_esw_link_fault_get(unit, port, &new_fault);
        if (BCM_FAILURE(rv)) {
            soc_cm_debug(DK_ERR,
                   "Unit %d, Port %s: Failed to read fault status: %s\n", 
                         unit, SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
            return rv;
        }
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
        if (new_fault && soc_feature(unit, soc_feature_port_lag_failover)
            && SOC_PBMP_MEMBER(sop->lc_pbm_failover, port)) {
            /* Must force port into LAG failover mode by forcing
             * a PHY linkdown event. */
            rv = _bcm_esw_link_failover_link_down_force(unit, port);
            if (BCM_FAILURE(rv)) {
                soc_cm_debug(DK_ERR,
                             "Unit %d, Port %s: Failed to force link down for fault on LAG failover: %s\n", 
                         unit, SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
                return rv;
            } else {
                new_failed = TRUE;
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_link_failover_port_disable(unit, port));
                SOC_PBMP_PORT_ADD(sop->lc_pbm_failed, port);
            }
        }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
    }
#endif /* LYNX, HERC15, FIREBOLT, EASYRIDER */  

    if (((cur_failed && new_failed)) ||
        ((cur_link) && (new_link) && (cur_fault == new_fault)) ||
        ((!cur_link) && (!new_link))) {
	/* No change */
	return BCM_E_NONE;
    }

#if defined(BCM_LINK_CHANGE_BENCHMARK)
    time_start = sal_time_usecs();
#endif /* BCM_LINK_CHANGE_BENCHMARK */

    /* In failed state, no link processing is performed until
       the state is cleared */
    if (!new_failed) {
        /*
         * If disabling, stop ingresses from sending any more traffic to
         * this port.
         */

        if (!new_link) {
            SOC_PBMP_PORT_REMOVE(sop->lc_pbm_link, port);
            SOC_PBMP_PORT_REMOVE(sop->lc_pbm_remote_fault, port);

            rv = soc_link_fwd_set(unit, sop->lc_pbm_link);

            if (BCM_FAILURE(rv)) {
                soc_cm_debug(DK_ERR, 
                             "Port %s: soc_link_fwd_set failed: %s\n",
                             SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
                return rv;
            }
        }

        /* Program MACs
         * (only if port is not forced, and link state changed) */

        if (!SOC_PBMP_MEMBER(sop->lc_pbm_override_ports, port) &&
            (cur_link != new_link)) {
            rv = bcm_esw_port_update(unit, port, new_link);

            if (BCM_FAILURE(rv)) {
                soc_cm_debug(DK_ERR, 
                             "Port %s: bcm_port_update failed: %s\n",
                             SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
                return rv;
            }
#if defined(BCM_LYNX_SUPPORT) || defined(BCM_HERCULES15_SUPPORT) || \
    defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)
            /* Physical link completed, now we can check fault */
            if (soc_feature(unit, soc_feature_bigmac_fault_stat) &&
                new_link) {
                /*
                 * Now our rules are:
                 * 1) The chip supports it:  feature check
                 * 2) The link state is coming from the PHY: not override.
                 * 3) The physical link is now up, so can check fault.
                 */
                rv = _bcm_esw_link_fault_get(unit, port, &new_fault);
                if (BCM_FAILURE(rv)) {
                    soc_cm_debug(DK_ERR,
                                 "Port %s: Failed to read fault status: %s\n", 
                                 SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
                    return rv;
                }
            }
#endif /* LYNX, HERC15, FIREBOLT, EASYRIDER */  
        }


        /*
         * If enabling, allow traffic to go to this port.
         */

        if (new_link) {
            soc_pbmp_t pbm_link_fwd;

            SOC_PBMP_PORT_ADD(sop->lc_pbm_link, port);
            SOC_PBMP_ASSIGN(pbm_link_fwd, sop->lc_pbm_link);

            if (!new_fault) {
                SOC_PBMP_PORT_REMOVE(sop->lc_pbm_remote_fault, port);
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
                if (SOC_PBMP_MEMBER(sop->lc_pbm_failed_clear, port)) {
                    /* Previously failed port completing recovery */
                    rv = _bcm_esw_link_failover_link_up(unit, port);
                    if (BCM_FAILURE(rv)) {
                        soc_cm_debug(DK_ERR, 
                            "Port %s: failed link recovery unsuccessful: %s\n",
                            SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
                        return rv;
                    }
                    SOC_PBMP_PORT_REMOVE(sop->lc_pbm_failed_clear, port);
                }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
            } else {
                SOC_PBMP_PORT_ADD(sop->lc_pbm_remote_fault, port);
            }

            SOC_PBMP_REMOVE(pbm_link_fwd, sop->lc_pbm_remote_fault);

            rv = soc_link_fwd_set(unit, pbm_link_fwd);

            if (BCM_FAILURE(rv)) {
                soc_cm_debug(DK_ERR, 
                             "Port %s: soc_link_fwd_set failed: %s\n",
                             SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
                return rv;
            }
        }

#if defined(BCM_HAWKEYE_SUPPORT)
        if(SOC_IS_HAWKEYE(unit) && new_link) {
            if(soc_feature(unit, soc_feature_hawkeye_a0_war)) {       
                /* QSGMII Pause Work aroud for HAWKEYE A0*/
                BCM_IF_ERROR_RETURN(
                                    bcm_esw_port_phy_get(unit, port, 0, HK_PAUSE_WAR_PHY_REG, &phy_data));
                SOC_IF_ERROR_RETURN(
                                    READ_CMIC_TX_PAUSE_CAPABILITYr(unit, &mac_data));
                    
                if (phy_data & PHY_AN_TX_PAUSE_CAP) {
                    mac_data |= (1 << (port-1));
                } else {
                    mac_data &= ~(1 << (port-1));
                }
                SOC_IF_ERROR_RETURN(
                                    WRITE_CMIC_TX_PAUSE_CAPABILITYr(unit, mac_data));
                    
                SOC_IF_ERROR_RETURN(
                                    READ_CMIC_RX_PAUSE_CAPABILITYr(unit, &mac_data));
            
                if (phy_data & PHY_AN_RX_PAUSE_CAP) {
                    mac_data |= (1 << (port-1));
                } else {
                    mac_data &= ~(1 << (port-1));
                }
                SOC_IF_ERROR_RETURN(
                                    WRITE_CMIC_RX_PAUSE_CAPABILITYr(unit, mac_data));
                    
                /* Toggle ENA_EXT_CONFIG of command_config register */
                SOC_IF_ERROR_RETURN(
                                    READ_COMMAND_CONFIGr(unit, port, &mac_data));
                soc_reg_field_set(unit, COMMAND_CONFIGr, &mac_data, ENA_EXT_CONFIGf, 0);
                SOC_IF_ERROR_RETURN(
                                    WRITE_COMMAND_CONFIGr(unit, port, mac_data));
                soc_reg_field_set(unit, COMMAND_CONFIGr, &mac_data, ENA_EXT_CONFIGf, 1);
                SOC_IF_ERROR_RETURN(
                                    WRITE_COMMAND_CONFIGr(unit, port, mac_data));
            } else {
                /* QSGMII Pause Work aroud for NON HAWKEYE A0*/
                BCM_IF_ERROR_RETURN(
                                    bcm_esw_port_phy_get(unit, port, 0, HK_PAUSE_WAR_PHY_REG, &phy_data));
                SOC_IF_ERROR_RETURN(
                                    READ_CMIC_TX_PAUSE_CAPABILITYr(unit, &mac_data));
                    
                if (phy_data & PHY_AN_TX_PAUSE_CAP) {
                    mac_data |= (1 << port);
                } else {
                    mac_data &= ~(1 << port);
                }
                SOC_IF_ERROR_RETURN(
                                    WRITE_CMIC_TX_PAUSE_CAPABILITYr(unit, mac_data));
                    
                SOC_IF_ERROR_RETURN(
                                    READ_CMIC_RX_PAUSE_CAPABILITYr(unit, &mac_data));
            
                if (phy_data & PHY_AN_RX_PAUSE_CAP) {
                    mac_data |= (1 << port);
                } else {
                    mac_data &= ~(1 << port);
                }
                SOC_IF_ERROR_RETURN(
                                    WRITE_CMIC_RX_PAUSE_CAPABILITYr(unit, mac_data));

                /* Toggle ENA_EXT_CONFIG of command_config register */
                SOC_IF_ERROR_RETURN(
                                    READ_COMMAND_CONFIGr(unit, port, &mac_data));
                soc_reg_field_set(unit, COMMAND_CONFIGr, &mac_data, ENA_EXT_CONFIGf, 0);
                SOC_IF_ERROR_RETURN(
                                    WRITE_COMMAND_CONFIGr(unit, port, mac_data));
                soc_reg_field_set(unit, COMMAND_CONFIGr, &mac_data, ENA_EXT_CONFIGf, 1);
                SOC_IF_ERROR_RETURN(
                                    WRITE_COMMAND_CONFIGr(unit, port, mac_data));

                /* In HAWKEYE A0, 
                 * the back-pressure is disabled in half-duplex mode 
                 * when the bit HD_FC_ENA is cleared (binary 0).
                 * In other revision of HAWKEYE,                
                 * the back-pressure is disabled in half-duplex mode 
                 * when the bit HD_FC_ENA is set (binary 1).
                 * To avoid confusion with HAWKEYE A0, 
                 * S/W set the bit HD_FC_ENA to 1 in other revision of HAWKEYE */
                SOC_IF_ERROR_RETURN(READ_IPG_HD_BKP_CNTLr(unit, port, &val));   
                soc_reg_field_set(unit, IPG_HD_BKP_CNTLr, &val, HD_FC_ENAf, 1);   
                SOC_IF_ERROR_RETURN(WRITE_IPG_HD_BKP_CNTLr(unit, port, val));   
            }
        }
#endif /* BCM_HAWKEYE_SUPPORT */
    }

    /*
     * Call registered handlers with complete link info.
     * Display link status message, if requested.
     *
     * In case link status changed again for bcm_port_info_get,
     * overwrite the linkstatus field with logical_link.  This ensures
     * the handler is presented with a consistent alternating
     * sequence of link up/down.
     */

    notify = (cur_failed != new_failed) || /* Change of failed state */
        (cur_link && new_link) ||  /* => cur_fault != new_fault */
        (!new_link && !cur_fault) ||    /* Port down w/o prior fault */
        (new_link && !new_fault);       /* Port up w/o fault */
    logical_link = new_failed ? BCM_PORT_LINK_STATUS_FAILED :
        ((new_link && !new_fault) ? BCM_PORT_LINK_STATUS_UP :
         BCM_PORT_LINK_STATUS_DOWN);

    soc_cm_debug(DK_LINK | DK_VERBOSE,
                 "Unit %d, Port %s: Link: Current %s, New %s\n"
                 "\tFault: Current %s, New %s\n"
                 "\tFailed: Current %s, New %s\n"
                 "\tNotify %s, Logical link %s\n",
                 unit,
                 SOC_PORT_NAME(unit, port),
                 cur_link ? "up" : "down",
                 new_link ? "up" : "down",
                 cur_fault ? "yes" : "no",
                 new_fault ? "yes" : "no",
                 cur_failed ? "yes" : "no",
                 new_failed ? "yes" : "no",
                 notify ? "yes" : "no",
                 (logical_link == BCM_PORT_LINK_STATUS_FAILED) ? "failed" :
                 (logical_link ? "up" : "down"));
    

    if (notify) {
        rv = bcm_esw_port_info_get(unit, port, &info);

        if (BCM_FAILURE(rv)) {
            soc_cm_debug(DK_ERR,
                         "Port %s: bcm_port_info_get failed: %s\n", 
                         SOC_PORT_NAME(unit, port),
                         bcm_errmsg(rv));
            return rv;
        }

        if (soc_feature(unit, soc_feature_asf)) {
            rv = _bcm_esw_linkscan_update_asf(unit, port,
                                              (logical_link != 0),
                                              info.speed, info.duplex);
            if (BCM_FAILURE(rv)) {
                soc_cm_debug(DK_ERR,
                             "Port %s: linkscan ASF update failed: %s\n",
                             SOC_PORT_NAME(unit, port),
                             bcm_errmsg(rv));
                return rv;
            }
        }
    }

#if defined(BCM_LINK_CHANGE_BENCHMARK)
    time_end = sal_time_usecs();
#endif /* BCM_LINK_CHANGE_BENCHMARK */

    if (cur_failed != new_failed) {
        if (new_failed) {
            soc_cm_debug(DK_LINK,
                         "Port %s: failed\n",
                         SOC_PORT_NAME(unit, port));
        } else {
            soc_cm_debug(DK_LINK,
                         "Port %s: failed state cleared\n",
                         SOC_PORT_NAME(unit, port));
        }
    }

    if (cur_link != new_link) {
        if (new_link) {
            soc_cm_debug(DK_LINK,
                         "Port %s: link up (%dMb %s %s)\n",
                         SOC_PORT_NAME(unit, port),
                         info.speed,
                         info.duplex ? "Full Duplex" : "Half Duplex",
                         PHY_FIBER_MODE(unit, port) ?
                         "Fiber" : "Copper");
#if defined(BCM_LINK_CHANGE_BENCHMARK)
            soc_cm_debug(DK_LINK, "Link up processing took %d usecs\n",
                         SAL_USECS_SUB(time_end, time_start));
#endif /* BCM_LINK_CHANGE_BENCHMARK */
 
        } else {
            soc_cm_debug(DK_LINK,
                         "Port %s: link down\n",
                         SOC_PORT_NAME(unit, port));

#if defined(BCM_LINK_CHANGE_BENCHMARK)
            soc_cm_debug(DK_LINK, "Link down processing took %d usecs\n",
                         SAL_USECS_SUB(time_end, time_start));
#endif /* BCM_LINK_CHANGE_BENCHMARK */
        }
    }

    if (new_fault) {
        soc_cm_debug(DK_LINK, "Port %s: fault detected\n",
                     SOC_PORT_NAME(unit, port));
    } else if (cur_fault) {
        soc_cm_debug(DK_LINK, "Port %s: fault cleared\n",
                     SOC_PORT_NAME(unit, port));
    }

    if (notify) {
        soc_cm_debug(DK_LINK | DK_VERBOSE,
                     "Unit %d, Port %s: logical link notification - %s\n",
                     unit, SOC_PORT_NAME(unit, port),
                     (logical_link == BCM_PORT_LINK_STATUS_FAILED) ?
                     "failed" : (logical_link ? "up" : "down"));
        info.linkstatus = logical_link;

        for (lh = lc->lc_handler; lh; lh = lh_next) {
            /*
             * save the next linkscan handler first, in case current handler
             * unregister itself inside the handler function
             */
            lh_next = lh->lh_next;        
            lh->lh_f(unit, port, &info);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:    
 *      _bcm_esw_linkscan_update
 * Purpose:     
 *      Check for a change in link status on each link.  If a change
 *      is detected, call bcm_port_update to program the MACs for that
 *      link, and call the list of registered handlers.
 * Parameters:  
 *      unit - StrataSwitch unit #.
 *      pbm - bit map of ports to scan. 
 * Returns:
 *      Nothing.
 */

STATIC void
_bcm_esw_linkscan_update(int unit, pbmp_t pbm)
{
    soc_persist_t	*sop = SOC_PERSIST(unit);
    ls_cntl_t           *lc = link_control[unit];
    pbmp_t		save_link_change;
    int                 rv;
    bcm_port_t          port;

    LC_LOCK(unit);

    /*
     * Suspend hardware link scan here to avoid overhead of pause/resume
     * on MDIO accesses. Ideally this would not be required if the source of the
     * interrupt is the signal from the internal Serdes but we still need to do
     * this due to work-arounds in phy drivers where we need to rely on SW linkscan
     * to detect link up.
     */
    soc_linkscan_pause(unit);           /* Suspend linkscan */

    PBMP_ITER(pbm, port) {
        
	ls_errstate_t *err = &lc->lc_error[port];

	if (err->wait) {	/* Port waiting in error state */
	    if (SAL_USECS_SUB(sal_time_usecs(), err->time) >= err->delay) {
		err->wait = 0;	/* Exit error state */
		err->count = 0;

		soc_cm_debug(DK_ERR | DK_VERBOSE,
			     "Port %s: restored\n",
			     SOC_PORT_NAME(unit, port));
	    } else {
		continue;
	    }
	}

	save_link_change = sop->lc_pbm_link_change;

	rv = _bcm_esw_linkscan_update_port(unit, port);

	if (BCM_FAILURE(rv)) {
	    sop->lc_pbm_link_change = save_link_change;

	    if (++err->count >= err->limit && err->limit > 0) {
		/* Enter error state */

		soc_cm_debug(DK_ERR,
			     "Port %s: temporarily removed from linkscan\n",
			     SOC_PORT_NAME(unit, port));

		err->time = sal_time_usecs();
		err->wait = 1;
	    }
	} else if (err->count > 0) {
	    err->count--;	/* Reprieve */
	}
       
    }

    soc_linkscan_continue(unit);        /* Restart H/W link scan */

    LC_UNLOCK(unit);
}


/*
 * Function:    
 *      bcm_esw_linkscan_update
 * Purpose:     
 *      Check for a change in link status on each link.  If a change
 *      is detected, call bcm_port_update to program the MACs for that
 *      link, and call the list of registered handlers.
 * Parameters:  
 *      unit - StrataSwitch unit #.
 *      pbm - bit map of ports to scan. 
 * Returns:
 *      BCM_E_XXX.
 */
int 
bcm_esw_linkscan_update(int unit, bcm_pbmp_t pbm)
{                       
    LC_CHECK_INIT(unit);

    if (BCM_PBMP_IS_NULL(pbm)) {
        return BCM_E_NONE;
    }

    _bcm_esw_linkscan_update(unit, pbm);

    return BCM_E_NONE;
}

/*
 * Function:    
 *      _bcm_esw_linkscan_hw_interrupt
 * Purpose:     
 *      Link scan interrupt handler if using CMIC_LINK_SCAN.
 * Parameters:  
 *      StrataSwitch unit #
 * Returns:     
 *      Nothing
 */

STATIC void
_bcm_esw_linkscan_hw_interrupt(int unit)
{
    ls_cntl_t           *lc = link_control[unit];

    if ((NULL != lc) && (NULL != lc->lc_sema)) {
        lc->lc_hw_change = 1;
        sal_sem_give(lc->lc_sema);
    }
    soc_cm_debug(DK_LINK | DK_VERBOSE, "Linkscan interrupt unit %d\n", unit);
}

/*
 * Function:    
 *         bcm_link_change
 * Purpose:
 *         Force a transient link down event to be recognized,
 *         regardless of the current physical up/down state of the
 *         port.  This does not affect the physical link status. 
 * Parameters:  
 *         unit - StrataSwitch Unit #.
 *         pbm - Bitmap of ports to operate on.
 * Returns:
 *         BCM_E_XXX
 */

int
bcm_esw_link_change(int unit, pbmp_t pbm)
{
    soc_persist_t	*sop = SOC_PERSIST(unit);
    ls_cntl_t		*lc = link_control[unit];

    LC_CHECK_INIT(unit);

    SOC_PBMP_AND(pbm, PBMP_PORT_ALL(unit));

    LC_LOCK(unit);
    SOC_PBMP_OR(sop->lc_pbm_link_change, pbm);
    LC_UNLOCK(unit);

    /*
     * Wake up master thread to notice changes - required if using hardware
     * link scanning.
     */
    if (lc->lc_sema != NULL) {
        sal_sem_give(lc->lc_sema);
    }

    return(BCM_E_NONE);
}

/*
 * Function:    
 *      _bcm_esw_link_get
 * Purpose:
 *      Return linkscan's current link status for the given port.
 * Parameters:  
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch physical port number.
 *      link - (OUT) Current link status.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_DISABLED - Port not being scanned.
 * Note:
 *	This routine does not acquire the LC_LOCK, as it only reads a 
 *	snapshot of the link bitmaps.  It also cannot hold the LC_LOCK 
 *	since it is called indirectly from the linkscan thread 
 *	when requesting port info.
 */

int
_bcm_esw_link_get(int unit, bcm_port_t port, int *link)
{
    soc_persist_t	*sop = SOC_PERSIST(unit);

    if (SOC_PBMP_MEMBER(sop->lc_pbm_override_ports, port)) {
        *link =  SOC_PBMP_MEMBER(sop->lc_pbm_override_link, port);
        return (SOC_E_NONE);
    }

    BCM_IF_ERROR_RETURN
	(bcm_esw_linkscan_enable_port_get(unit, port));

    if (SOC_PBMP_MEMBER(sop->lc_pbm_failed, port)) {
        *link = BCM_PORT_LINK_STATUS_FAILED;
    } else {
        *link = (SOC_PBMP_MEMBER(sop->lc_pbm_link, port) &&
                 !SOC_PBMP_MEMBER(sop->lc_pbm_remote_fault, port)) ?
            BCM_PORT_LINK_STATUS_UP : BCM_PORT_LINK_STATUS_DOWN;
    }

    return(BCM_E_NONE);
}

/*
 * Function:    
 *      _bcm_esw_link_force
 * Purpose:
 *      Set linkscan's current link status for a port.
 * Parameters:  
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch physical port number.
 *      force - If TRUE, link status is forced to new link status.
 *              If FALSE, link status is no longer forced.
 *      link - New link status.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_INIT - Not initialized.
 * Notes:
 *	When a link is forced up or down, linkscan will stop scanning
 *	that port and _bcm_link_get will always return the forced status.
 *	This is used for purposes such as when a link is placed in MAC
 *	loopback.  If software forces a link up, it is responsible for
 *	configuring that port.
 */

int
_bcm_esw_link_force(int unit, bcm_port_t port, int force, int link)
{
    soc_persist_t	*sop = SOC_PERSIST(unit);
    ls_cntl_t		*lc = link_control[unit];
    pbmp_t		pbm;

    LC_CHECK_INIT(unit);

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
	return BCM_E_PORT;
    }

    LC_LOCK(unit);

    if (force) {
        SOC_PBMP_PORT_REMOVE(sop->lc_pbm_override_link, port);
	if (link) {
	    SOC_PBMP_PORT_ADD(sop->lc_pbm_override_link, port);
	}
        SOC_PBMP_PORT_ADD(sop->lc_pbm_override_ports, port);
    } else {
        SOC_PBMP_PORT_REMOVE(sop->lc_pbm_override_ports, port);
        SOC_PBMP_PORT_REMOVE(sop->lc_pbm_override_link, port);
        SOC_PBMP_PORT_ADD(sop->lc_pbm_link_change, port);
    }

    /*
     * Force immediate update to just this port - this allows loopback 
     * forces to take effect immediately.
     */
    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);
    _bcm_esw_linkscan_update(unit, pbm);

    LC_UNLOCK(unit);

    /*
     * Wake up master thread to notice changes - required if using hardware
     * link scanning.
     */
    if (lc->lc_sema != NULL) {
        sal_sem_give(lc->lc_sema);
    }

    return(BCM_E_NONE);
}

/*
 * Function:    
 *      _bcm_esw_link_failover_set
 * Purpose:
 *      Set whether the given port is configured for LAG failover.
 * Parameters:  
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch physical port number.
 *      enable - LAG failover mode.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_INIT - Not initialized.
 *      BCM_E_DISABLED - Port not being scanned.
 * Notes:
 *      Called by trunk failover configuration.
 */

int
_bcm_esw_link_failover_set(int unit, bcm_port_t port, int enable)
{
    soc_persist_t	*sop = SOC_PERSIST(unit);
    uint32 val;

    LC_CHECK_INIT(unit);

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
	return BCM_E_PORT;
    }

    LC_LOCK(unit);

    if (enable) {
        if (SOC_PBMP_MEMBER(sop->lc_pbm_failed, port) ||
            SOC_PBMP_MEMBER(sop->lc_pbm_failed_clear, port)) {
            /* Not yet recovered from previous failover.
             * Must wait until resolved. */
            LC_UNLOCK(unit);
            return BCM_E_PORT;
        } else {
            /* Toggle link bit to notify IPIPE on link up */
            BCM_IF_ERROR_RETURN
                (READ_GXPORT_LAG_FAILOVER_CONFIGr(unit, port, &val));
            soc_reg_field_set(unit, GXPORT_LAG_FAILOVER_CONFIGr, &val,
                              LINK_STATUS_UPf, 1);
            BCM_IF_ERROR_RETURN
                (WRITE_GXPORT_LAG_FAILOVER_CONFIGr(unit, port, val));
            soc_reg_field_set(unit, GXPORT_LAG_FAILOVER_CONFIGr, &val,
                              LINK_STATUS_UPf, 0);
            BCM_IF_ERROR_RETURN
                (WRITE_GXPORT_LAG_FAILOVER_CONFIGr(unit, port, val));
            SOC_PBMP_PORT_ADD(sop->lc_pbm_failover, port);
        }
    } else {
        SOC_PBMP_PORT_REMOVE(sop->lc_pbm_failover, port);
    }
    LC_UNLOCK(unit);

    soc_cm_debug(DK_LINK | DK_VERBOSE,
                 "Unit %d: LAG failover: Port %d - %s\n",
                 unit, port, enable ? "enabled" : "disabled");
    return BCM_E_NONE;
}

/*
 * Function:    
 *      _bcm_esw_link_failed_clear
 * Purpose:
 *      Allow a LAG failover port in failed state to complete link down
 *      processing.
 * Parameters:  
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch physical port number.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_INIT - Not initialized.
 *      BCM_E_PORT - Not a valid port or port not in failed state.
 *      BCM_E_DISABLED - Port not being scanned.
 */

int
_bcm_esw_link_failed_clear(int unit, bcm_port_t port)
{
    soc_persist_t	*sop = SOC_PERSIST(unit);

    LC_CHECK_INIT(unit);

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    if (!soc_feature(unit, soc_feature_port_lag_failover)) {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
	return BCM_E_PORT;
    }

    LC_LOCK(unit);

    if (!SOC_PBMP_MEMBER(sop->lc_pbm_failed, port)) {
        /* Not a failed port */
        LC_UNLOCK(unit);
	return BCM_E_PORT;
    } else if (SOC_PBMP_MEMBER(sop->lc_pbm_failover, port)) {
        /* Port still in trunk, can't clear yet */
        LC_UNLOCK(unit);
	return BCM_E_PORT;
    }

    /* Signal to finish link down processing in update routine */
    SOC_PBMP_PORT_ADD(sop->lc_pbm_failed_clear, port);
    LC_UNLOCK(unit);
    soc_cm_debug(DK_LINK | DK_VERBOSE,
                 "Unit %d: LAG failover failed state clear set: Port %d\n",
                 unit, port);

    return BCM_E_NONE;
}

/*
 * Function:    
 *      _bcm_esw_linkscan_thread
 * Purpose:     
 *      Scan the ports on the specified unit for link status
 *      changes and process them as they occur.
 * Parameters:  
 *      unit - StrataSwitch unit #.
 * Returns:     
 *      Nothing
 */

STATIC void
_bcm_esw_linkscan_thread(int unit)
{
    soc_persist_t    *sop = SOC_PERSIST(unit);
    ls_cntl_t        *lc = link_control[unit];
    sal_usecs_t       interval;
    int               rv;
    soc_port_t        port;

    soc_cm_debug(DK_LINK, "Linkscan starting on unit %d\n", unit);

    /* Do not clear the lc_pbm_override_ports and lc_pbm_override_link.
     * If a port in Loopback mode forces link up before enabling linkscan,
     * the port status should be still forced to up after enabling linkscan.
     */

    /*
     * Force initial scan by setting link change while pretending link
     * was initially up.
     */
    BCM_PBMP_ASSIGN(sop->lc_pbm_link, PBMP_ALL(unit));
    BCM_PBMP_ASSIGN(sop->lc_pbm_link_change, PBMP_PORT_ALL(unit));

    sal_memset(lc->lc_error, 0, sizeof (lc->lc_error));

    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        lc->lc_error[port].limit = soc_property_port_get(unit, port,
                                         spn_BCM_LINKSCAN_MAXERR, 5);
        lc->lc_error[port].delay = soc_property_port_get(unit, port,
                                         spn_BCM_LINKSCAN_ERRDELAY, 10000000);
    }

    /* Clear initial value of forwarding ports. */
    rv = soc_link_fwd_set(unit, sop->lc_pbm_link);

    if (BCM_FAILURE(rv)) {
        soc_cm_debug(DK_ERR,
             "Failed to clear forwarding ports: %s\n", 
             bcm_errmsg(rv));
        sal_thread_exit(0);
    }

    /* Register for hardware linkscan interrupt. */

    rv = soc_linkscan_register(unit, _bcm_esw_linkscan_hw_interrupt);

    if (BCM_FAILURE(rv)) {
        soc_cm_debug(DK_ERR,
                     "Failed to register handler: %s\n", 
                     bcm_errmsg(rv));
        sal_thread_exit(0);
    }

    lc->lc_thread = sal_thread_self();

    while ((interval = lc->lc_us) != 0) {
        pbmp_t            change;
        pbmp_t            hw_link;
        pbmp_t            hw_update;

        if (BCM_PBMP_IS_NULL(lc->lc_pbm_sw)) {
            interval = sal_sem_FOREVER;
        }

        /* sample changed */
        BCM_PBMP_ASSIGN(change, sop->lc_pbm_link_change); 

        /* If the interrupt indicates that link status changes,
         * process the ports indicated by HW linkscan first.
         */ 
        if (lc->lc_hw_change) {
            /* Pause linkscan to make sure that the HW link state does
             * change while updating software state.
             */
            soc_linkscan_pause(unit);
            lc->lc_hw_change = 0;

            (void)soc_linkscan_hw_link_get(unit, &hw_link);
            BCM_PBMP_AND(hw_link, lc->lc_pbm_hw);

#ifdef BCM_TRUNK_FAILOVER_SUPPORT
            /*
             * If enabled on XGS3 devices, do an early callback to the
             * trunk code that will immediately remove link down ports
             * from trunks.  This only works with hardware linkscanned
             * ports.
             */
            if (SOC_IS_XGS3_SWITCH(unit) || SOC_IS_XGS3_FABRIC(unit)) {
                extern int _bcm_xgs3_trunk_swfailover_trigger(int unit,
                                                bcm_pbmp_t ports_active,
                                                bcm_pbmp_t ports_status);
                (void)_bcm_xgs3_trunk_swfailover_trigger(unit,
                                                         lc->lc_pbm_hw,
                                                         hw_link);
            }
#endif /* BCM_TRUNK_FAILOVER_SUPPORT */
            
            /* Make sure that only valid ports are scaned */
            BCM_PBMP_ASSIGN(hw_update, hw_link);
            BCM_PBMP_XOR(hw_update, sop->lc_pbm_link);
            BCM_PBMP_AND(hw_update, lc->lc_pbm_hw);
            _bcm_esw_linkscan_update(unit, hw_update);

            /* Make sure that only valid ports are scaned */
            BCM_PBMP_AND(change, PBMP_PORT_ALL(unit));
            _bcm_esw_linkscan_update(unit, change);
            soc_linkscan_continue(unit);
        }

        /* After processing the link status changes of the ports
         * indicated by interrupt handler (mainly to trigger swfailover),
         * scan all the ports again to make sure that the link status
         * is stable. 
         * For some PHYs such as 5228, hardware linkscan may say the
         * link is up while the PHY is actually not quite done
         * autonegotiating. Rescanning make sure that the PHY link is
         * in sync with switch HW link state.
         */
        _bcm_esw_linkscan_update(unit, PBMP_PORT_ALL(unit));

        if (!BCM_PBMP_IS_NULL(change)) {   
            /* Re-scan due to hardware force */
            continue;
        }

        (void)sal_sem_take(lc->lc_sema, interval);
    }

    (void)soc_linkscan_register(unit, NULL);

    /*
     * Before exiting, re-enable all ports that were being scanned.
     *
     * For administrative reloads, application can enter reload
     * mode to avoid this disturbing of ports.
     */

    BCM_PBMP_ITER(lc->lc_pbm_sw, port) {
        (void)bcm_esw_port_enable_set(unit, port, TRUE);
    }

    BCM_PBMP_ITER(lc->lc_pbm_hw, port) {
        (void)bcm_esw_port_enable_set(unit, port, TRUE);
    }

    soc_cm_debug(DK_LINK, "Linkscan exiting\n");

    lc->lc_thread = NULL;
    sal_thread_exit(0);
}

/************************************************************************
 *********                                                      *********
 *********         Start of BCM API Exported Routines           *********
 *********                                                      *********
 ************************************************************************/

/*
 * Function:
 *      bcm_linkscan_init
 * Purpose:
 *      Initialize the linkscan software module.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_init(int unit)
{
    ls_cntl_t   	*lc;
    bcm_port_if_t       intf;
    uint32              regval;
    soc_port_t          port;

    if (link_control[unit] != NULL) {
	BCM_IF_ERROR_RETURN(bcm_esw_linkscan_detach(unit));
    }

    if ((lc = sal_alloc(sizeof (ls_cntl_t), "link_control")) == NULL) {
	return BCM_E_MEMORY;
    }

    sal_memset(lc, 0, sizeof (ls_cntl_t));


    lc->lc_lock = sal_mutex_create("bcm_link_LOCK");
    if (lc->lc_lock == NULL) {
	sal_free(lc);
        return BCM_E_MEMORY;
    }

    lc->lc_sema = sal_sem_create("bcm_link_SLEEP", 
				 sal_sem_BINARY, 0);
    if (lc->lc_sema == NULL) {
        sal_mutex_destroy(lc->lc_lock);
	sal_free(lc);
        return BCM_E_MEMORY;
    }

    link_control[unit] = lc;

    /*
     * Initialize link_control port bitmaps so bcm_port_update works
     * reasonably even if the linkscan thread is never started.
     */
    _lc_pbm_init(unit);


    if(SOC_IS_RCPU_ONLY(unit)) {
        return 0; 
    }   


    /* 1. Select between C45 and C22 for HW linkscan
     * 2. Select appropriate MDIO Bus
     * 3. Select between MDIO scan vs. link status from internal PHY
     * 4. Initialize HW linkscan PHY address map.
     */
    BCM_IF_ERROR_RETURN
        (soc_linkscan_hw_init(unit));
    
    /* For XGS 3 devices, select the source of the CMIC link status interrupt
     * to be the Internal Serdes - only for SGMII ports */ 
    BCM_PBMP_CLEAR(lc->lc_pbm_sgmii_autoneg_port);
    if (soc_feature(unit, soc_feature_sgmii_autoneg)) { 
        PBMP_ITER(PBMP_PORT_ALL(unit), port) { 
            if (soc_property_port_get(unit, port, 
                                      spn_PHY_SGMII_AUTONEG, FALSE)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_interface_get(unit, port, &intf));
                if (intf == BCM_PORT_IF_SGMII) {
                    BCM_PBMP_PORT_ADD(lc->lc_pbm_sgmii_autoneg_port, port);
                    if (port < 32) {
                        READ_CMIC_MIIM_INT_SEL_MAPr(unit, &regval);
                        regval |= (1 << port);
                        WRITE_CMIC_MIIM_INT_SEL_MAPr(unit, regval);
                    }
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
                    else if ((SOC_IS_TR_VL(unit) && !SOC_IS_ENDURO(unit)) ||
                             soc_feature(unit, soc_feature_register_hi)) {
                        READ_CMIC_MIIM_INT_SEL_MAP_HIr(unit, &regval);
                        regval |= (1 << (port - 32));
                        WRITE_CMIC_MIIM_INT_SEL_MAP_HIr(unit, regval);
                    }
#endif
                }
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:    
 *      bcm_esw_linkscan_mode_set
 * Purpose:
 *      Set the current scanning mode for the specified port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch Port #.
 *      mode - New scan mode for specified port.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_mode_set(int unit, bcm_port_t port, int mode)
{
    ls_cntl_t     *lc = link_control[unit];
    int           rv = BCM_E_NONE;
    pbmp_t	  empty_pbm;
    int		  added = 0;

    LC_CHECK_INIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_link_gport_resolve(unit, port, &port));
    }
    if ((SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port))) {
         return BCM_E_NONE;
    }
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    SOC_PBMP_CLEAR(empty_pbm);

    LC_LOCK(unit);

    /* First, remove from current configuration */
    SOC_PBMP_PORT_REMOVE(lc->lc_pbm_sw, port);
    SOC_PBMP_PORT_REMOVE(lc->lc_pbm_hw, port);

    /* Now add back to proper map */
    switch (mode) {
    case BCM_LINKSCAN_MODE_NONE:
        break;
    case BCM_LINKSCAN_MODE_SW:
        SOC_PBMP_PORT_ADD(lc->lc_pbm_sw, port);
	added = 1;
        break;
    case BCM_LINKSCAN_MODE_HW:
        SOC_PBMP_PORT_ADD(lc->lc_pbm_hw, port);

        if (BCM_PBMP_MEMBER(lc->lc_pbm_sgmii_autoneg_port, port)) {
            /* Need to run SW link scan as well on ports where the source 
             * of the link status is the internal Serdes - only SGMII ports */
            SOC_PBMP_PORT_ADD(lc->lc_pbm_sw, port);
        }
	added = 1;
        lc->lc_hw_change = 1;
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    /* Reconfigure HW linkscan in case changed */
    rv = soc_linkscan_config(unit, lc->lc_pbm_hw, empty_pbm);

    /* Prime the HW linkscan pump */
    if (SOC_PBMP_NOT_NULL(lc->lc_pbm_hw)) {
        lc->lc_hw_change = 1;
        _bcm_esw_linkscan_hw_interrupt(unit);
    }

    LC_UNLOCK(unit);

    if (lc->lc_sema != NULL) {
	sal_sem_give(lc->lc_sema);	/* register change now */
    }

    /* When no longer scanning a port, return it to the enabled state. */
    if (BCM_SUCCESS(rv) && !added) {
	rv = bcm_esw_port_enable_set(unit, port, TRUE);
    }

    return(rv);
}

/*
 * Function:    
 *      bcm_esw_linkscan_mode_set_pbm
 * Purpose:
 *      Set the current scanning mode for the specified ports.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbm  - Port bit map indicating port to set.
 *      mode - New scan mode for specified ports.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_mode_set_pbm(int unit, pbmp_t pbm, int mode)
{
    bcm_port_t  port;

    LC_CHECK_INIT(unit);

    PBMP_ITER(pbm, port) {
	BCM_IF_ERROR_RETURN
	    (bcm_esw_linkscan_mode_set(unit, port, mode));
    }

    return (BCM_E_NONE);
}

/*
 * Function:    
 *      bcm_esw_linkscan_mode_get
 * Purpose:
 *      Recover the current scanning mode for the specified port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch Port #.
 *      mode - (OUT) current scan mode for specified port.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_mode_get(int unit, bcm_port_t port, int *mode)
{
    ls_cntl_t   *lc = link_control[unit];

    LC_CHECK_INIT(unit);
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_link_gport_resolve(unit, port, &port));
    }
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }
    if (NULL == mode) {
        return BCM_E_PARAM;
    }

    if (PBMP_MEMBER(lc->lc_pbm_hw, port)) {
        *mode = BCM_LINKSCAN_MODE_HW;
    } else if (PBMP_MEMBER(lc->lc_pbm_sw, port)) {
        *mode = BCM_LINKSCAN_MODE_SW;
    } else {
        *mode = BCM_LINKSCAN_MODE_NONE;
    }

    return(BCM_E_NONE);
}

/*
 * Function:    
 *      bcm_esw_linkscan_enable_get
 * Purpose:
 *      Retrieve the current linkscan mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      us - (OUT) Pointer to microsecond scan time for software scanning, 
 *              0 if not enabled.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_enable_get(int unit, int *us)
{
    LC_CHECK_INIT(unit);

    *us = link_control[unit]->lc_us;

    return(BCM_E_NONE);
}

/*
 * Function:    
 *      bcm_esw_linkscan_enable_set
 * Purpose:
 *      Enable or disable the link scan feature.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      us - Specifies the software polling interval in micro-seconds;
 *		the value 0 disables linkscan.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_enable_set(int unit, int us)
{
    ls_cntl_t           *lc = link_control[unit];
    int                 rv = BCM_E_NONE;
    soc_timeout_t       to;
    pbmp_t              empty_pbm;
    sal_usecs_t         wait_usec;

    /* Time to wait for thread to start/end (longer for BCMSIM) */
    wait_usec = (SAL_BOOT_BCMSIM) ? 30000000 : 10000000;
 
    if (!us && lc == NULL) {	/* No error to disable if not inited */
        return(BCM_E_NONE);
    }

    LC_CHECK_INIT(unit);

    sal_snprintf(lc->lc_taskname,
                 sizeof (lc->lc_taskname),
                 "bcmLINK.%d",
                 unit);

    SOC_PBMP_CLEAR(empty_pbm);

    if (us) {
        if (us < BCM_LINKSCAN_INTERVAL_MIN) {
            us = BCM_LINKSCAN_INTERVAL_MIN;
        }

        lc->lc_us = us;

        if (lc->lc_thread != NULL) {
            /* Linkscan is already running; just update the period */
            sal_sem_give(lc->lc_sema);
            return SOC_E_NONE;
        }


        if (sal_thread_create(lc->lc_taskname,
                                     SAL_THREAD_STKSZ,
                                     soc_property_get(unit,
                                                      spn_LINKSCAN_THREAD_PRI,
                                                      50),
                                     (void (*)(void*))_bcm_esw_linkscan_thread,
                                     INT_TO_PTR(unit)) == SAL_THREAD_ERROR) {
            lc->lc_us = 0;
            rv = BCM_E_MEMORY;
        } else {
            soc_timeout_init(&to, wait_usec, 0);

            while (lc->lc_thread == NULL) {
                if (soc_timeout_check(&to)) {
                    soc_cm_debug(DK_ERR,
                                 "%s: Thread did not start\n",
                                 lc->lc_taskname);
                    lc->lc_us = 0;
                    rv = BCM_E_INTERNAL;
                    break;
                }
            }
            if (BCM_SUCCESS(rv)) {
                /* Make sure HW linkscanning is enabled on HW linkscan ports */ 
                rv = soc_linkscan_config(unit, lc->lc_pbm_hw, empty_pbm);
            }
        }
    } else if (lc->lc_thread != NULL) {
        lc->lc_us = 0;

        /* To prevent getting HW linkscan interrupt after linkscan is disabled,
         * HW linkscanning must be disabled. */ 
        rv = soc_linkscan_config(unit, empty_pbm, empty_pbm);

        sal_sem_give(lc->lc_sema);

        soc_timeout_init(&to, wait_usec, 0);

        while (lc->lc_thread != NULL) {
            if (soc_timeout_check(&to)) {
                soc_cm_debug(DK_ERR,
                             "%s: Thread did not exit\n",
			     lc->lc_taskname);
                rv = BCM_E_INTERNAL;
                break;
            }
        }
    }

    return(rv);
}

/*
 * Function:    
 *      bcm_esw_linkscan_register
 * Purpose:
 *      Register a handler to be called when a link status change is noticed.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      f - pointer to function to call when link status change is seen
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_register(int unit, bcm_linkscan_handler_t f)
{
    ls_cntl_t		*lc = link_control[unit];
    ls_handler_t	*lh;
    int			found = FALSE;

    LC_CHECK_INIT(unit);

    /* First, see if this handler already registered */
    LC_LOCK(unit);

    for (lh = lc->lc_handler; lh != NULL; lh = lh->lh_next) {
        if (lh->lh_f == f) {
            found = TRUE;
            break;
        }
    }

    if (found) {
        LC_UNLOCK(unit);
        return BCM_E_NONE;
    }

    if ((lh = sal_alloc(sizeof(*lh), "bcm_linkscan_register")) == NULL) {
        LC_UNLOCK(unit);
        return(BCM_E_MEMORY);
    }

    lh->lh_next = lc->lc_handler;
    lh->lh_f = f;
    lc->lc_handler = lh;

    LC_UNLOCK(unit);

    return(BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_linkscan_unregister
 * Purpose:
 *      Remove a previously registered handler from the callout list.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *	f    - pointer to function registered in call to 
 *             bcm_linkscan_register.
 * Returns:
 *      BCM_E_NOT_FOUND		Could not find matching handler
 *	BCM_E_NONE		Success
 */

int
bcm_esw_linkscan_unregister(int unit, bcm_linkscan_handler_t f)
{
    ls_cntl_t		*lc = link_control[unit];
    ls_handler_t	*lh, *p;

    LC_CHECK_INIT(unit);

    LC_LOCK(unit);

    for (p = NULL, lh = lc->lc_handler; lh; p = lh, lh = lh->lh_next) { 
        if (lh->lh_f == f) {
            if (p == NULL) {
                lc->lc_handler = lh->lh_next;
            } else {
                p->lh_next = lh->lh_next;
            }
            break;
        }
    }

    LC_UNLOCK(unit);

    if (lh != NULL) {
        sal_free(lh);
	return BCM_E_NONE;
    } else {
	return BCM_E_NOT_FOUND;
    }
}

/*
 * Function:
 *      bcm_esw_linkscan_port_register
 * Purpose:
 *      Register a handler to be called when a link status
 *      is to be determined by a caller provided function.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - StrataSwitch port number.
 *      f - pointer to function to call for true link status
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *	This function works with software linkscan only.  
 */

int
bcm_esw_linkscan_port_register(int unit, bcm_port_t port,
                               bcm_linkscan_port_handler_t f)
{
    ls_cntl_t   *lc = link_control[unit];
  
    LC_CHECK_INIT(unit);
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_link_gport_resolve(unit, port, &port));
    }
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    LC_LOCK(unit);
    lc->lc_f[port] = f;
    LC_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_linkscan_port_unregister
 * Purpose:
 *      Remove a previously registered handler that is used
 *      for setting link status. 
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - StrataSwitch port number.
 *	f    - pointer to function registered in call to 
 *             bcm_linkscan_port_register.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_port_unregister(int unit, bcm_port_t port,
                                 bcm_linkscan_port_handler_t f)
{
    ls_cntl_t   *lc = link_control[unit];
    int         rv;
 
    LC_CHECK_INIT(unit);
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_link_gport_resolve(unit, port, &port));
    }
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    LC_LOCK(unit);
    if (f == lc->lc_f[port]) {
    lc->lc_f[port] = NULL;
        rv = BCM_E_NONE;
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    LC_UNLOCK(unit);

    return (rv);
}

/*
 * Function:    
 *      bcm_link_wait
 * Purpose:
 *      Wait for all links in the mask to be "link up".
 * Parameters:  
 *      unit - StrataSwitch unit #.
 *      pbm - (IN/OUT) Port bit map to wait for, mask of those link up on 
 *              return.
 *      us - number of microseconds to wait.
 * Returns:
 *      BCM_E_NONE - all links are ready.
 *      BCM_E_TIMEOUT - not all links ready in specified time.
 *	BCM_E_DISABLED - linkscan not running on one or more of the ports.
 */

int
bcm_esw_link_wait(int unit, pbmp_t *pbm, int us)
{
    soc_persist_t	*sop = SOC_PERSIST(unit);
    ls_cntl_t           *lc = link_control[unit];
    soc_timeout_t       to;
    pbmp_t		sofar_pbm;
    soc_port_t		port;

    /* Input parameters check. */
    if ((NULL == pbm) || (us < 0)) {
        return (BCM_E_PARAM);
    }

    BCM_PBMP_ITER(*pbm, port) {
	BCM_IF_ERROR_RETURN
	    (bcm_esw_linkscan_enable_port_get(unit, port));
    }

    /*
     * If a port was just configured, it may have gone down but
     * lc_pbm_link may not reflect that until the next time linkscan
     * runs.  This is avoided by forcing an update of lc_pbm_link.
     */

    _bcm_esw_linkscan_update(unit, *pbm);

    soc_timeout_init(&to, (sal_usecs_t)us, 0);

    for (;;) {
	SOC_PBMP_ASSIGN(sofar_pbm, sop->lc_pbm_link);
        SOC_PBMP_REMOVE(sofar_pbm, sop->lc_pbm_remote_fault);
	SOC_PBMP_AND(sofar_pbm, *pbm);
	if (SOC_PBMP_EQ(sofar_pbm, *pbm)) {
	    break;
	}

        if (soc_timeout_check(&to)) {
            SOC_PBMP_AND(*pbm, sop->lc_pbm_link);
            SOC_PBMP_REMOVE(*pbm, sop->lc_pbm_remote_fault);
            return (BCM_E_TIMEOUT);
        } 

        sal_usleep(lc->lc_us / 4);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *	bcm_linkscan_detach
 * Purpose:
 *	Prepare linkscan module to detach specified unit.
 * Parameters:
 *	unit - StrataSwitch Unit #.
 * Returns:
 *	BCM_E_NONE - detach successful.
 *	BCM_E_XXX - detach failed.
 * Notes:
 *	This is safe to call at any time, but linkscan should only be
 *	initialized or detached from the main application thread.
 */

int
bcm_esw_linkscan_detach(int unit)
{
    ls_cntl_t           *lc = link_control[unit];
    ls_handler_t        *lh;
    pbmp_t		empty_pbm;

    if (lc == NULL) {
	return BCM_E_NONE;
    }

    SOC_PBMP_CLEAR(empty_pbm);

    SOC_IF_ERROR_RETURN(soc_linkscan_config(unit, empty_pbm, empty_pbm));

    BCM_IF_ERROR_RETURN(bcm_esw_linkscan_enable_set(unit, 0));

    /* Clean up list of handlers */

    while (lc->lc_handler != NULL) {
	lh = lc->lc_handler;
	lc->lc_handler = lh->lh_next;
        sal_free(lh);
    }

    /* Mark and not initialized and free mutex */

    if (lc->lc_sema != NULL) {
	sal_sem_destroy(lc->lc_sema);
	lc->lc_sema = NULL;
    }

    if (lc->lc_lock != NULL) {
	sal_mutex_destroy(lc->lc_lock);
	lc->lc_lock = NULL;
    }

    link_control[unit] = NULL;

    sal_free(lc);

    return(BCM_E_NONE);
}    

#if defined(BROADCOM_DEBUG)
int
bcm_esw_linkscan_dump(int unit)
{
    ls_handler_t *ent;

    for (unit = 0; unit < 3; unit++) {
        if (link_control[unit] == NULL) {
            soc_cm_print("BCM linkscan not initialized for unit %d\n", unit);
            continue;
        }

        soc_cm_print("BCM linkscan callbacks for unit %d\n", unit);
        for (ent = link_control[unit]->lc_handler; ent != NULL;
             ent = ent->lh_next) {
#if !defined(__PEDANTIC__)
            soc_cm_print("    Fn %p\n", (void *)ent->lh_f);
#else /* !defined(__PEDANTIC__) */
            soc_cm_print("    Function pointer unprintable\n");
#endif /* !defined(__PEDANTIC__) */
        }
    }

    return BCM_E_NONE;
}
#endif  /* BROADCOM_DEBUG */
