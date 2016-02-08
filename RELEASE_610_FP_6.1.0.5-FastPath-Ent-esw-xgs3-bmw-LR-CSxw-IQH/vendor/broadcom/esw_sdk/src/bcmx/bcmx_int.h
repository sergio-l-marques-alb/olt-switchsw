/*
 * $Id: bcmx_int.h,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * File:        bcmx_int.h
 * Purpose:     Internal header file for BCMX
 */

#ifndef   _BCMX_BCMX_INT_H_
#define   _BCMX_BCMX_INT_H_


#include <sal/core/sync.h>
#include <bcm/types.h>

/*
 * Check that given lport has correct format.
 * Note that it does NOT check if lport is a valid BCMX logical port.
 */
#define BCMX_IS_LPORT(_lport)    (BCM_GPORT_IS_SET(_lport))

/* Same as above, but performs a return in calling routine on failure */
#define BCMX_LPORT_CHECK(_lport)   \
    if (!BCMX_IS_LPORT(_lport)) { return BCM_E_PORT; }

/* Check that given lport is a valid BCMX logical port (returns on failure) */
#define BCMX_LPORT_VALID_CHECK(_lport)   \
    if (!bcmx_lport_valid(_lport)) { return BCM_E_PORT; }

/* Cast lport <-> gport */
#define BCMX_LPORT_TO_GPORT_CAST(_lport)    ((bcm_gport_t)(_lport))
#define BCMX_GPORT_TO_LPORT_CAST(_gport)    ((bcmx_lport_t)(_gport))

/* Check that given lport is a physical port */
#define BCMX_LPORT_IS_PHYSICAL(_lport)                              \
    (BCM_GPORT_IS_MODPORT(_lport) || BCM_GPORT_IS_DEVPORT(_lport))

/*
 * Check that given lport is a virtual port (does not belong
 * to a local unit)
 */
#define BCMX_LPORT_IS_VIRTUAL(_lport)                               \
    (!BCMX_LPORT_IS_PHYSICAL(_lport) &&                             \
     !BCM_GPORT_IS_LOCAL(_lport) && !BCM_GPORT_IS_LOCAL_CPU(_lport))


/* Check for 'null' argument (returns on failure) */
#define BCMX_PARAM_NULL_CHECK(_arg)  if ((_arg) == NULL) {return BCM_E_PARAM;}


/*
 * Currently, these result in the last error being returned;
 * Check rv < 0 as well to return first error.
 */
#define BCMX_RV_REPLACE(tmp_rv, rv) if ((tmp_rv) < 0) (rv) = (tmp_rv)

#define BCMX_RV_REPLACE_OK(tmp_rv, rv, ok_rv) \
    if ((tmp_rv) < 0 && (tmp_rv) != (ok_rv)) (rv) = (tmp_rv)

#include <bcm_int/control.h>
#define BCMX_UNITS_MAX BCM_CONTROL_MAX

extern int bcmx_unit_list[BCMX_UNITS_MAX];
extern int bcmx_unit_count;

extern sal_mutex_t bcmx_config_lock;

extern int _bcmx_rx_running;
extern volatile int _bcmx_l2n_running;


#define BCMX_CONFIG_LOCK sal_mutex_take(bcmx_config_lock, sal_sem_FOREVER)
#define BCMX_CONFIG_UNLOCK sal_mutex_give(bcmx_config_lock)

#define BCMX_UNIT_ITER(unit, i)                                           \
    for (i = 0, unit = bcmx_unit_list[0];                                 \
         i < bcmx_unit_count;                                             \
         unit = bcmx_unit_list[++i])

#define BCMX_LOCAL_UNIT_ITER(unit, i)                                     \
    for (i = 0, unit = bcmx_unit_list[0];                                 \
         i < bcmx_unit_count;                                             \
         unit = bcmx_unit_list[++i]) if (BCM_IS_LOCAL(unit))


/* Get local CPU lport if set */
extern bcmx_lport_t bcmx_lport_local_cpu[BCM_CONTROL_MAX];
#define BCMX_LPORT_LOCAL_CPU_GET(bcm_unit) (bcmx_lport_local_cpu[bcm_unit])


/*
 * Conversion BCMX to/from BCM destination information
 */
typedef struct _bcmx_dest_bcmx_s {
    bcmx_lport_t  port;    /* BCMX logical port (gport format) */
    bcm_trunk_t   trunk;   /* Trunk id */
    int           mcast;   /* Mcast */
} _bcmx_dest_bcmx_t;

typedef struct _bcmx_dest_bcm_s {
    int           unit;         /* Device unit number */
    bcm_port_t    port;         /* Device port number (old format) or gport */
    bcm_module_t  module_id;    /* Module id */
    bcm_port_t    module_port;  /* Module relative port (old format) or gport */
    bcm_trunk_t   trunk;        /* Trunk id */
    int           mcast;        /* Mcast */
} _bcmx_dest_bcm_t;


/*
 * Conversion flags BCMX to/from BCM destination information
 *
 * NOTES:
 *   GPORT_AWARE  - Convert to new gport format, used only when
 *                  translating to BCM
 *   TRUNK        - Data is in old format, data is in 'trunk' member
 *   MCAST        - Data is in old format, data is in 'mcast' member
 */
#define BCMX_DEST_GPORT_AWARE  0x00000001  /* BCM can handle GPORT */
#define BCMX_DEST_TRUNK        0x00000002  /* Trunk */
#define BCMX_DEST_MCAST        0x00000004  /* Mcast */
#define BCMX_DEST_DISCARD      0x00000008  /* Black hole */
#define BCMX_DEST_LOCAL_CPU    0x00000010  /* Local CPU */

/*
 * Initializer routines
 */
extern void
_bcmx_dest_bcmx_t_init(_bcmx_dest_bcmx_t *dest_bcmx);

extern void
_bcmx_dest_bcm_t_init(_bcmx_dest_bcm_t *dest_bcm);

/*
 * Conversion routines for destination structures
 */
extern int
_bcmx_dest_to_bcm(_bcmx_dest_bcmx_t *from_bcmx, _bcmx_dest_bcm_t *to_bcm,
                  uint32 *flags);

extern int
_bcmx_dest_from_bcm(_bcmx_dest_bcmx_t *to_bcmx, _bcmx_dest_bcm_t *from_bcm,
                    uint32 *flags);

/*
 * Conversion default flag for BCMX to/from BCM.
 * 
 * Current default flag value indicates to convert to old BCM format
 * (non-gport format), for _bcmx_dest_to_modid_port() or
 * _bcmx_dest_unit_port() routines.
 */
#define BCMX_DEST_CONVERT_NON_GPORT  0x0
#define BCMX_DEST_CONVERT_DEFAULT    BCMX_DEST_CONVERT_NON_GPORT

/*
 * Conversion routines for specific destination information
 * (module/port, unit/port, ...)
 */
extern int
_bcmx_dest_to_modid_port(bcmx_lport_t port,
                         bcm_module_t *bcm_module, bcm_port_t *bcm_port,
                         uint32 flags);
extern int
_bcmx_dest_from_modid_port(bcmx_lport_t *port,
                           bcm_module_t bcm_module, bcm_port_t bcm_port,
                           uint32 flags);
extern int
_bcmx_dest_to_unit_port(bcmx_lport_t port,
                        int *bcm_unit, bcm_port_t *bcm_port,
                        uint32 flags);

extern int
_bcmx_dest_from_unit_port(bcmx_lport_t *port,
                          int bcm_unit, bcm_port_t bcm_port,
                          uint32 flags);

extern int
_bcmx_port_changed(bcmx_lport_t lport);


#endif /* _BCMX_BCMX_INT_H_ */
