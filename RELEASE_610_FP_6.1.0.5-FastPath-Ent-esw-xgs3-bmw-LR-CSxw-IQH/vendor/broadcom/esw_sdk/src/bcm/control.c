/*
 * $Id: control.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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
 * BCM Dispatch Control Operations (eg: attach, detach)
 */

#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <bcm/debug.h>

#include <soc/drv.h>
#ifdef BCM_SBX_SUPPORT
#include <soc/sbx/sbx_drv.h>
#endif

#include <bcm/error.h>

#include <bcm_int/dispatch.h>
#include <bcm_int/control.h>
#include <bcm_int/api_ref.h>
#include <bcm_int/api_xlate_port.h>


#ifdef	BCM_LOOP_SUPPORT
extern bcm_dispatch_t	bcm_loop_dispatch;
#endif
#ifdef	BCM_RPC_SUPPORT
extern bcm_dispatch_t	bcm_client_dispatch;
#endif
#ifdef	BCM_ESW_SUPPORT
extern bcm_dispatch_t	bcm_esw_dispatch;
#endif
#ifdef	BCM_ROBO_SUPPORT
extern bcm_dispatch_t	bcm_robo_dispatch;
#endif
#ifdef  BCM_MODENA_SUPPORT
extern bcm_dispatch_t bcm_modena_dispatch;
#endif
#if     defined(BCM_QE2000_SUPPORT) || defined(BCM_BME3200_SUPPORT) || defined(BCM_BM9600_SUPPORT)
extern bcm_dispatch_t bcm_sbx_dispatch;
#endif
#ifdef  BCM_FE2000_SUPPORT
extern bcm_dispatch_t bcm_fe2000_dispatch;
#endif

static bcm_dispatch_t	*_bcm_disp_types[] = {
#ifdef	BCM_LOOP_SUPPORT
    &bcm_loop_dispatch,
#endif
#ifdef	BCM_RPC_SUPPORT
    &bcm_client_dispatch,
#endif
#ifdef	BCM_ESW_SUPPORT
    &bcm_esw_dispatch,
#endif
#ifdef	BCM_ROBO_SUPPORT
    &bcm_robo_dispatch,
#endif
#ifdef  BCM_MODENA_SUPPORT
    &bcm_modena_dispatch,
#endif
#if     defined(BCM_QE2000_SUPPORT) || defined(BCM_BME3200_SUPPORT) || defined(BCM_BM9600_SUPPORT)
    &bcm_sbx_dispatch,
#endif
#ifdef  BCM_FE2000_SUPPORT
    /*
     * FE2000 and FE2000XT use the same dispatch table but have different
     * chip IDs.  Both chip IDs are assigned the smae string name thus
     * the 2nd entry here is simply a placeholder and never used.
     */
    &bcm_fe2000_dispatch,
    &bcm_fe2000_dispatch,
#endif
    NULL
};

bcm_control_t		*bcm_control[BCM_CONTROL_MAX];
static sal_mutex_t	_bcm_control_lock;

#define	CONTROL_LOCK	sal_mutex_take(_bcm_control_lock, sal_mutex_FOREVER)
#define	CONTROL_UNLOCK	sal_mutex_give(_bcm_control_lock)

#define BCM_DEVICE_CONTROL_DESTROY(_unit_)                  \
        if (BCM_CONTROL(_unit_)->subtype != NULL) {       \
            sal_free(BCM_CONTROL(_unit_)->subtype);       \
        }                                                 \
        sal_free(BCM_CONTROL(_unit_));                    \
        BCM_CONTROL(_unit_) = NULL; 

int
bcm_attach(int unit, char *type, char *subtype, int remunit)
{
    bcm_dispatch_t	**dp;
    int			rv;

    BCM_DEBUG(BCM_DBG_ATTACH, ("STK %d: attach %s subtype %s as %d\n",
                               unit, (NULL !=type) ? type: "N/A", 
                               (NULL != subtype) ? subtype:"N/A",
                               remunit));

    /* Protection mutex initialization. */
    if (_bcm_control_lock == NULL) {
        _bcm_control_lock = sal_mutex_create("bcm_control");
        if (_bcm_control_lock == NULL) {
            return (BCM_E_MEMORY);
        }
    }

    CONTROL_LOCK;
    if (unit < 0) {	/* find a free unit */
        for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
            if (BCM_CONTROL(unit) == NULL) {
                break;
            }
        }
        if (unit >= BCM_CONTROL_MAX) {
            CONTROL_UNLOCK;
            return (BCM_E_FULL);
        }
    }

    /* Check if unit is already attached. */
    if (BCM_CONTROL(unit) != NULL) {
        CONTROL_UNLOCK;
        return (BCM_E_EXISTS);
    }

    /* Allocate unit control structure. */
    BCM_CONTROL(unit) = sal_alloc(sizeof(bcm_control_t), "bcm_control");
    if (BCM_CONTROL(unit) == NULL) {
        CONTROL_UNLOCK;
        return (BCM_E_MEMORY);
    }
    sal_memset(BCM_CONTROL(unit), 0, sizeof(bcm_control_t));

    if (subtype != NULL) {
        BCM_CONTROL(unit)->subtype = sal_alloc(sal_strlen(subtype)+1,
                                               "bcm_control subtype");
        if (BCM_CONTROL(unit)->subtype == NULL) {
            BCM_DEVICE_CONTROL_DESTROY(unit);
            CONTROL_UNLOCK;                                   
            return (BCM_E_MEMORY);
        }
        sal_strcpy(BCM_CONTROL(unit)->subtype, subtype);
    }

    if (type == NULL) {
        if (SOC_IS_ROBO(unit)) {
#ifdef	BCM_ROBO_SUPPORT
            type = "robo";
#endif
        }
#ifdef  BCM_SBX_SUPPORT
        else if (SOC_IS_SBX(unit)) {
	    if ((SOC_IS_SBX_QE(unit) || SOC_IS_SBX_BME(unit))) {
		type = "sbx";
	    } else {
		type = SOC_CHIP_STRING(unit);
	    }
        }
#endif
#ifdef BCM_ESW_SUPPORT
        else { 
            type = "esw";
        }
#endif
    }

    if (NULL == type) {
        BCM_DEVICE_CONTROL_DESTROY(unit);
        CONTROL_UNLOCK;                                   
        return (BCM_E_CONFIG);
    }

    /* Find dispatch type. */
    for (dp = _bcm_disp_types; *dp != NULL; dp++) {
        if (sal_strcmp(type, (*dp)->name) == 0) {
            break;
        }
    }

    if (*dp == NULL) {
        BCM_DEVICE_CONTROL_DESTROY(unit);
        CONTROL_UNLOCK;                                   
        return (BCM_E_CONFIG);
    }

    BCM_CONTROL(unit)->dispatch = *dp;
    BCM_CONTROL(unit)->unit = remunit;
    rv = BCM_DISPATCH(unit)->_attach(unit, subtype);
    if (BCM_FAILURE(rv)) {
        BCM_DEVICE_CONTROL_DESTROY(unit);
        CONTROL_UNLOCK;                                   
        return (rv);
    }
    CONTROL_UNLOCK;
    return (unit);
}

int
bcm_detach(int unit)
{
    int		rv;

    BCM_DEBUG(BCM_DBG_ATTACH, ("STK %d: unit being detached\n", unit));

    if (unit < 0 || unit >= BCM_CONTROL_MAX) {
        return BCM_E_UNIT;
    }

    if (NULL == BCM_CONTROL(unit)) {
       return (BCM_E_NONE);
    }

    CONTROL_LOCK;

    rv = BCM_DISPATCH(unit)->_detach(unit);

    /* Clean up port mappings */
    _bcm_api_xlate_port_cleanup(unit);

    BCM_DEVICE_CONTROL_DESTROY(unit);

    CONTROL_UNLOCK;

    return rv;
}

int
bcm_find(char *type, char *subtype, int remunit)
{
    int	unit;

    for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
	if (BCM_CONTROL(unit) == NULL) {
	    continue;
	}
	if (remunit != BCM_CONTROL(unit)->unit) {
	    continue;
	}
	if (type != NULL &&
	    sal_strcmp(type, BCM_CONTROL(unit)->dispatch->name) != 0) {
	    continue;
	}
	if (subtype == NULL && BCM_CONTROL(unit)->subtype != NULL) {
	    continue;
	}
	if (subtype != NULL && BCM_CONTROL(unit)->subtype == NULL) {
	    continue;
	}
	if (subtype != NULL &&
	    sal_strcmp(subtype, BCM_CONTROL(unit)->subtype) != 0) {
	    continue;
	}
	return unit;
    }
    return BCM_E_NOT_FOUND;
}

int
bcm_attach_check(int unit)
{
    if (unit < 0 || unit >= BCM_CONTROL_MAX) {
	return BCM_E_UNIT;
    }
    if (BCM_CONTROL(unit) == NULL) {
	return BCM_E_UNIT;
    }
    return BCM_E_NONE;
}

int
bcm_attach_max(int *max_units)
{
    int	unit;

    *max_units = -1;
    for (unit = 0; unit < BCM_CONTROL_MAX; unit++) {
	if (BCM_CONTROL(unit) != NULL) {
	    *max_units = unit;
	}
    }
    return BCM_E_NONE;
}

int
bcm_init(int unit)
{
    int		rv = BCM_E_NONE;
    void    *api_ptr = (void *)bcm_api_tbl;
    int     init_done = FALSE;

    /* This should never be true; done to include api table */
    if (api_ptr == NULL) {
        return 0;
    }

    if (unit < 0 || unit >= BCM_CONTROL_MAX) {
        return BCM_E_UNIT;
    }
    if (BCM_CONTROL(unit) == NULL) {

        if (!SOC_UNIT_VALID(unit)) {
            return BCM_E_UNIT;
        }
        rv = bcm_attach(unit, NULL, NULL, unit);
        if (rv < 0) {
            return rv;
        }
        if (SOC_IS_XGS(unit)) {
            init_done = TRUE;
        }
    }

    if (FALSE == init_done) {
        /* Initialize port mappings */
        _bcm_api_xlate_port_init(unit);

        rv = BCM_DISPATCH(unit)->init(unit);
    }

#ifdef BCM_CUSTOM_INIT_F
    if(BCM_SUCCESS(rv)) {
        extern int BCM_CUSTOM_INIT_F (int unit); 
        rv = BCM_CUSTOM_INIT_F (unit); 
    }
#endif /* BCM_CUSTOM_INIT_F */

    return rv; 
}


/*
 * Attach and detach dispatchable routines.
 */
int
_bcm_null_attach(int unit, char *subtype)
{
    COMPILER_REFERENCE(subtype);

    BCM_CONTROL(unit)->capability |= BCM_CAPA_LOCAL;

    return BCM_E_NONE;
}

int
_bcm_null_detach(int unit)
{
    return BCM_E_NONE;
}

#ifdef	BCM_LOOP_SUPPORT
int
_bcm_loop_attach(int unit, char *subtype)
{
    int		dunit;
    uint32	dcap;

    COMPILER_REFERENCE(subtype);

    dunit = BCM_CONTROL(unit)->unit;

    BCM_CONTROL(unit)->chip_vendor = BCM_CONTROL(dunit)->chip_vendor;
    BCM_CONTROL(unit)->chip_device = BCM_CONTROL(dunit)->chip_device;
    BCM_CONTROL(unit)->chip_revision = BCM_CONTROL(dunit)->chip_revision;
    dcap = BCM_CONTROL(dunit)->capability;
    dcap &= ~(BCM_CAPA_REMOTE|BCM_CAPA_COMPOSITE);
    BCM_CONTROL(unit)->capability |= BCM_CAPA_LOCAL | dcap;
    return BCM_E_NONE;
}

int
_bcm_loop_detach(int unit)
{
    return BCM_E_NONE;
}
#endif	/* BCM_LOOP_SUPPORT */

int
bcm_unit_valid(int unit)
{
    return BCM_UNIT_VALID(unit);
}

int
bcm_unit_local(int unit)
{
    return (BCM_UNIT_VALID(unit) && BCM_IS_LOCAL(unit));
}

int
bcm_unit_remote(int unit)
{
    return (BCM_UNIT_VALID(unit) && BCM_IS_REMOTE(unit));
}

int
bcm_unit_max(void)
{
    return BCM_CONTROL_MAX;
}

/*
 * Get the local reference of a remote device for its controlling
 * CPU.
 */

int
bcm_unit_remote_unit_get(int unit, int *remunit)
{
    if (!BCM_UNIT_VALID(unit)) {
        return BCM_E_NOT_FOUND;
    }

    if (remunit == NULL) {
        return BCM_E_PARAM;
    }

    *remunit = BCM_CONTROL(unit)->unit;

    return BCM_E_NONE;
}

/*
 * Get the subtype string that identifies the CPU controlling
 * the given unit.  subtype must point to a preallocated buffer;
 * maxlen is the maximum number of bytes that will be copied.
 *
 * returns the number of bytes copied or < 0 if an error occurs.
 *
 * NOTE:  This is currently the CPU key (mac address) as a
 * formatted string like 00:11:22:33:44:55.  It should be
 * converted to a CPU DB key with cpudb_key_parse.
 */

int
bcm_unit_subtype_get(int unit, char *subtype, int maxlen)
{
    int minlen;
    int stlen;

    if (!BCM_UNIT_VALID(unit)) {
        return BCM_E_NOT_FOUND;
    }

    if ((subtype == NULL) || (maxlen <= 0)) {
        return BCM_E_PARAM;
    }

    stlen = sal_strlen(BCM_CONTROL(unit)->subtype) + 1;
    minlen = maxlen < stlen ? maxlen : stlen;

    sal_memcpy(subtype, BCM_CONTROL(unit)->subtype, minlen);

    return minlen;
}
