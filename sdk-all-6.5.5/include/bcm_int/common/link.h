/*
 * $Id: link.h,v 1.3 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * Common internal definitions for BCM Linkscan module
 */

#ifndef _BCM_INT_LINK_H_
#define _BCM_INT_LINK_H_

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/debug.h>
#include <bcm/link.h>
#include <soc/types.h>

#define NUM_PORTS    SOC_MAX_NUM_PORTS

/*
 * Driver specific routines
 *
 * (See description for routines below)
 */
typedef struct _bcm_ls_driver_s {
    void (*ld_hw_interrupt)(int, bcm_pbmp_t*);
    int  (*ld_port_link_get)(int, bcm_port_t, int, int*);
    int  (*ld_internal_select)(int, bcm_port_t);
    int  (*ld_update_asf)(int, bcm_port_t, int, int, int);
    int  (*ld_trunk_sw_failover_trigger)(int, bcm_pbmp_t, bcm_pbmp_t);
} _bcm_ls_driver_t;

/*
 * Handler to registered callbacks for link changes
 */
typedef struct _ls_handler_cb_s {
    struct _ls_handler_cb_s  *next;
    bcm_linkscan_handler_t   cb_f;
} _ls_handler_cb_t;

/*
 * Linkscan error state
 */
typedef struct _ls_errstate_s {
    int          limit;    /* # errors to enter error state */
    int          delay;    /* Length of error state in seconds */
    int          count;    /* # of errors so far */
    int          wait;     /* Boolean, TRUE when in error state */
    sal_usecs_t  time;     /* Time error state was entered */
} _ls_errstate_t;

/*
 * Linkscan Module control structure
 */
typedef struct _ls_control_s {
    _bcm_ls_driver_t  *driver;            /* Device specific routines */
    sal_mutex_t       lock;               /* Synchronization */
    char              taskname[16];       /* Linkscan thread name */
    VOL sal_thread_t  thread_id;          /* Linkscan thread id */
    VOL int           interval_us;        /* Time between scans (us) */
    sal_sem_t         sema;               /* Linkscan semaphore */
    pbmp_t            pbm_hw;             /* Hardware link scan ports */
    pbmp_t            pbm_sw;             /* Software link scan ports */
    pbmp_t            pbm_hw_upd;         /* Ports requiring HW link re-scan */
    pbmp_t            pbm_sgmii_autoneg;  /* Ports with SGMII autoneg */
    pbmp_t            pbm_link;           /* Ports currently up */
    pbmp_t            pbm_link_change;    /* Ports needed to recognize down */
    pbmp_t            pbm_override_ports; /* Force up/Down ports */
    pbmp_t            pbm_override_link;  /* Force up/Down status */
    pbmp_t            pbm_newly_enabled;  /* indicate linkscan is newly enabled for ports*/
    pbmp_t            pbm_remote_fault;   /* Ports receiving remote fault */
    int               hw_change;          /* HW Link state has changed */
    _ls_errstate_t    error[NUM_PORTS];   /* Link error state */
    _ls_handler_cb_t  *handler_cb;        /* Handler to list of callbacks */
    bcm_linkscan_port_handler_t
                      port_link_f[NUM_PORTS]; /* Port link fn */
} _ls_control_t;

extern  _ls_control_t           *_linkscan_control[BCM_LOCAL_UNITS_MAX];
#define LS_CONTROL(unit)        (_linkscan_control[unit])

/*
 * Driver specific routines description
 *
 * Function:
 *     ld_hw_interrupt
 * Purpose:
 *     Routine handler for hardware linkscan interrupt.
 * Parameters:
 *     unit - Device unit number
 *     pbmp - (OUT) Returns bitmap of ports that require hardware re-scan
 *
 *
 * Function:
 *     ld_port_link_get
 * Purpose:
 *     Return current PHY up/down status.
 * Parameters:
 *     unit - Device unit number
 *     port - Device port number
 *     hw   - If TRUE, assume hardware linkscan is active and use it
 *              to reduce PHY reads.
 *            If FALSE, do not use information from hardware linkscan.
 *     up   - (OUT) TRUE for link up, FALSE for link down.
 *
 *
 * Function:
 *     ld_internal_select
 * Purpose:
 *     Select the source of the CMIC link status interrupt
 *     to be the Internal Serdes on given port.
 * Parameters:
 *     unit - Device unit number
 *     port - Device port number
 *
 *
 * Function:
 *     ld_update_asf
 * Purpose:
 *     Update Alternate Store and Forward parameters for a port.
 * Parameters:
 *     unit   - Device unit number
 *     port   - Device port number
 *     linkup - port link state (0=down, 1=up)
 *     speed  - port speed
 *     duplex - port duplex (0=half, 1=full)
 *
 *
 * Function:
 *     ld_trunk_sw_failover_trigger
 * Purpose:
 *     Remove specified ports with link down from trunks.
 * Parameters:
 *     unit        - Device unit number
 *     pbmp_active - Bitmap of ports
 *     pbmp_status - Bitmap of port status
 */

extern int _bcm_linkscan_init(int unit, _bcm_ls_driver_t *driver);

extern int _bcm_link_get(int unit, bcm_port_t port, int *link);
extern int _bcm_link_force(int unit, bcm_port_t port, int force, int link);

extern int _bcm_linkscan_pause(int unit);
extern int _bcm_linkscan_continue(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
int bcm_linkscan_sync(int unit, int sync);
#endif
#endif /* _BCM_INT_LINK_H_ */
