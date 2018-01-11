/*
 * $Id:$
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
 * File:       bcm_utils.h
 */
 
#ifndef _BCM_UTILS_H_
#define _BCM_UTILS_H_

#include <stdio.h>
#include "types.h"


#define SAL_USECS_SUB(_t2, _t1)        (((int) ((_t2) - (_t1))))
#define SAL_USECS_ADD(_t, _us)        ((_t) + (_us))

typedef uint32 sal_usecs_t;
typedef int     soc_module_t;

typedef struct soc_timeout_s {
    sal_usecs_t		expire;
    sal_usecs_t		usec;
    int			    min_polls;
    int			    polls;
    sal_usecs_t		exp_delay;
} soc_timeout_t;

#define NUM_MODULES     4
/* We use 4 phy device ids.                                           */
/* PHY0_DEV_ID specifies the first phy id in a sequential series of 4 */
/* eg: 0, 1, 2, 3                                                     */
#define PHY0_DEV_ID      0
#define PHY_DEVAD        1

/* Data Structures */
typedef struct phy_ctrl_s {
    int                 unit;               /* BCM unit number */
    soc_module_t        module;             /* BCM module number */
    uint16              phy_id;             /* phy dev ID */
    uint8               phy_devad;          /* phy device addr */
    int                 flags;              /* PHY flags for internal use */
    soc_timeout_t       to;                 /* timer for multi pass initialization */
    uint8               *firmware;          /* firmware pointer */
    int                 firmware_len;       /* firmware length in bytes */
    int                 dl_dividend;        /* divident for MDC during firmware download */
    int                 dl_divisor;         /* divisor for MDC during firmware download */
    int                 tsc_sys_port;       /* used to identify tsc system port */
    int                 lane_mask;          /* specify port lane */
} phy_ctrl_t; 

extern phy_ctrl_t phyctrl[];
#define EXT_PHY_SW_STATE(unit, module)  (&phyctrl[module])

extern int bcm_mdio_read(int unit, phy_ctrl_t *pc, uint16 phy_reg_addr, uint16 *val);
extern int bcm_mdio_write(int unit, phy_ctrl_t *pc, uint16 phy_reg_addr, uint16 val);
extern int bcm_mdio_modify(int unit, phy_ctrl_t *pc, uint16 phy_reg_addr, uint16 val, uint16 mask);

#define READ_PHY_REG(_unit, _pc,  _addr, _value) \
            (bcm_mdio_read((_unit), (_pc), (_addr), (_value)))
#define WRITE_PHY_REG(_unit, _pc, _addr, _value) \
            (bcm_mdio_write((_unit), (_pc), (_addr), (_value)))
#define MODIFY_PHY_REG(_unit, _pc, _addr, _value, _mask) \
            (bcm_mdio_modify((_unit), (_pc), (_addr), (_value), (_mask)))

extern void
bcm_timeout_init(soc_timeout_t *to, uint32 usec, int min_polls);
extern int
bcm_timeout_check(soc_timeout_t *to);
void
bcm_udelay(int usec);

extern void *
bcm_memcpy(void *dst_void, const void *src_void, uint32 len);
extern void *
bcm_memset(void *dst_void, int val, size_t len);

#define do_nothing(fmt, args...) {}

/* Any layer log macros */
#define BCM_LOG_ERR printf
#define BCM_LOG_WARN printf
#define BCM_LOG_CLI printf
#define BCM_LOG_DEBUG do_nothing

#define sal_mutex_FOREVER	(-1)
#define sal_mutex_NOWAIT	0
typedef struct sal_sem_s{
    char sal_opaque_type;
} *sal_sem_t;

typedef struct sal_mutex_s{
    char mutex_opaque_type;
} *sal_mutex_t;

/*
 * bcm_mutex_t
 */
typedef struct bcm_mutex_s {
    sal_sem_t       sem;
    char            *desc;
} bcm_mutex_t;

extern sal_mutex_t sbus_lock;
extern sal_mutex_t bcm_mutex_create(char *desc);
extern void bcm_mutex_destroy(sal_mutex_t m);
extern int bcm_mutex_take(sal_mutex_t m, int usec);
extern int bcm_mutex_give(sal_mutex_t m);
#endif /* _BCM_UTILS_H_ */
