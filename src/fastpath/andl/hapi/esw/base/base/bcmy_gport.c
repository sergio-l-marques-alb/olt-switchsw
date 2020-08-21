/*
 * $Id: lplist.c,v 1.14 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        lplist.c
 * Purpose:     Logical port list functions
 * Requires:
 *
 * Notes:       The following is intended:
 *
 *     bcmy_gplist_t alist;
 *
 *     ... list is built up with some elements ....
 *
 *     if (!bcmy_gplist_init(alist, 0, 0))  {
 *
 *         ... list is built up with some elements, then ....
 *
 *         BCMY_GPLIST_ITER(alist, gport, count) {
 *             .... do stuff with gport ....
 *         }
 *     }
 */    

#include <sdk_config.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <ibde.h>
#include <bcm/stack.h>
#include <bcm/port.h>
#include <bcm/error.h>

#include "bcmy_gport_api.h"
#include "ptin_fpga_api.h"

/********************************************************************
 * GPORT MANIPULATION FUNCTIONS                                     *
 ********************************************************************/

/**
 * Check if gport is a valid value
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in)
 * 
 * @return BOOL : TRUE / FALSE
 */
BOOL bcmy_gport_is_valid(bcm_gport_t gport)
{
    if (0 == gport || BCM_GPORT_INVALID == gport)
    {
        return FALSE;
    }

    if (!BCM_GPORT_IS_SET(gport))
    {
        return FALSE;
    }

    return TRUE;
}

/**
 * Check if GPORT is hw mapped
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in)
 * 
 * @return BOOL : TRUE / FALSE
 */
BOOL bcmy_gport_is_hwmapped(bcm_gport_t gport)
{
    int bcm_unit, bcm_port;
    int rv;

    /* Check if gport is valid */
    if (!bcmy_gport_is_valid(gport))
    {
        return FALSE;
    }

    rv = bcmy_gport_to_unit_port(gport, &bcm_unit, &bcm_port);
    if (rv != BCMY_E_NONE)
    {
        return FALSE;
    }
    
    return bcmy_unit_port_is_hwmapped(bcm_unit, bcm_port);
}

/**
 * Check if BCM unit/port is hw mapped
 * 
 * @author mruas (04/08/20)
 * 
 * @param unit (in) BCM unit
 * @param port (in) BCM port
 * 
 * @return BOOL : TRUE / FALSE
 */
BOOL bcmy_unit_port_is_hwmapped(int unit, int port)
{
    uint32_t flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;
    int rv;
    
    rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        return FALSE;
    }
    
    return TRUE;
}


/**
 * Check if GPORT is local to this board
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in)
 * 
 * @return BOOL : TRUE / FALSE
 */
BOOL bcmy_gport_is_local(bcm_gport_t gport)
{
    int bcm_unit, modid_local, modid_base, modid_number;

    /* Check if gport is valid */
    if (!bcmy_gport_is_valid(gport))
    {
        return FALSE;
    }

    /* Local type? */
    if (BCM_GPORT_IS_LOCAL(gport))
    {
        return TRUE;
    }
    /* Check if this is a local/modport type */
    else if (BCM_GPORT_IS_MODPORT(gport))
    {
        /* Run all units */
        for (bcm_unit = 0; bcm_unit < bde->num_devices(BDE_SWITCH_DEVICES); bcm_unit++)
        {
            /* Get modid base */
            if (bcm_stk_modid_get(bcm_unit, &modid_base) != BCM_E_NONE)
            {
                return FALSE;
            }
            /* Get the number of cores */
            if (bcm_stk_modid_count(bcm_unit, &modid_number) != BCM_E_NONE)
            {
                return FALSE;
            }

            /* Run all local modid's */
            for (modid_local = modid_base; modid_local < modid_base + modid_number; modid_local++)
            {
                if (modid_local == BCM_GPORT_MODPORT_MODID_GET(gport))
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

/**
 * Return first CPU interface for this unit
 * 
 * @author mruas (04/08/20)
 * 
 * @param unit (in)
 * 
 * @return bcm_gport_t : first CPU interface 
 */
bcm_gport_t bcmy_gport_local_cpu_get_first(int unit)
{
    bcm_port_t port;
    bcm_gport_t gport_ret;
    bcm_port_config_t port_config;
    int modid_base;
    int rv;

    /* Get base modid for given unit */
    if (bcmy_unit_to_modid(unit, &modid_base, NULLPTR) != BCMY_E_NONE)
    {
        return BCMY_INVALID_VAL;
    }
    
    /* get the cpu port bitmap */
    rv = bcm_port_config_get(unit, &port_config);
    if (rv != BCM_E_NONE)
    {
        return BCMY_INVALID_VAL;
    }
    /* Check if port bitmap is not null */
    if (BCM_PBMP_IS_NULL(port_config.cpu))
    {
        return BCMY_INVALID_VAL;
    }
    /* Find first cpu port */
    for (port = 0; port < BCM_PBMP_PORT_MAX; port++)
    {
        if (BCM_PBMP_MEMBER(port_config.cpu, port))
        {
            break;
        }
    }
    /* Check if port was found */
    if (port >= BCM_PBMP_PORT_MAX)
    {
        return BCMY_INVALID_VAL;
    }
    
    /* Build gport */
    BCM_GPORT_MODPORT_SET(gport_ret, modid_base, port);

    return gport_ret;
}

/**
 * Get next CPU interface for this unit
 * 
 * @author mruas (04/08/20)
 * 
 * @param unit (in)
 * @param gport (in): CPU interface
 * 
 * @return bcm_gport_t : next CPU interface
 */
bcm_gport_t bcmy_gport_local_cpu_get_next(int unit, bcm_gport_t gport)
{
    bcm_port_t port;
    bcm_gport_t gport_ret;
    bcm_port_config_t port_config;
    int modid_base, modid_number;
    int _modid, _modport;
    int rv;

    /* Validate gport */
    if (!bcmy_gport_is_valid(gport))
    {
        return BCMY_INVALID_VAL;
    }
    /* Get modid and modport */
    _modid   = BCM_GPORT_MODPORT_MODID_GET(gport);
    _modport = BCM_GPORT_MODPORT_PORT_GET(gport);
    /* Check if any error occurred */
    if (_modid < 0 || _modport < 0 || _modport >= BCM_PBMP_PORT_MAX)
    {
        return BCMY_INVALID_VAL;
    }

    /* Get base modid for this unit */
    if (bcmy_unit_to_modid(unit, &modid_base, &modid_number) != BCMY_E_NONE)
    {
        return BCMY_INVALID_VAL;
    }
    /* Check if modid is coherent */
    if (_modid < modid_base || _modid >= modid_base+modid_number)
    {
        return BCMY_INVALID_VAL;
    }
    
    /* get the cpu port bitmap */
    rv = bcm_port_config_get(unit, &port_config);
    if (rv != BCM_E_NONE)
    {
        return BCMY_INVALID_VAL;
    }
    /* Check if port bitmap is not null */
    if (BCM_PBMP_IS_NULL(port_config.cpu))
    {
        return BCMY_INVALID_VAL;
    }
    /* Find first cpu port */
    for (port = _modport+1; port < BCM_PBMP_PORT_MAX; port++)
    {
        if (BCM_PBMP_MEMBER(port_config.cpu, port))
        {
            break;
        }
    }
    /* Check if port was found */
    if (port >= BCM_PBMP_PORT_MAX)
    {
        return BCMY_INVALID_VAL;
    }

    /* Build gport */
    BCM_GPORT_MODPORT_SET(gport_ret, modid_base, port);

    return gport_ret;
}

#if 0 /* XGS not supported */
/**
 * Return first OLP interface for this unit
 * 
 * @author mruas (04/08/20)
 * 
 * @param unit (in)
 * 
 * @return bcm_gport_t : first OLP interface
 */
bcm_gport_t bcmy_gport_local_olp_get_first(int unit)
{
    bcm_port_t port;
    bcm_gport_t gport_ret;
    bcm_port_config_t port_config;
    int modid_base;
    int rv;

    /* Get base modid for given unit */
    if (bcmy_unit_to_modid(unit, &modid_base, NULLPTR) != BCMY_E_NONE)
    {
        return BCMY_INVALID_VAL;
    }

    /* get the cpu port bitmap */
    rv = bcm_port_config_get(unit, &port_config);
    if (rv != BCM_E_NONE)
    {
        return BCMY_INVALID_VAL;
    }
    /* Check if port bitmap is not null */
    if (BCM_PBMP_IS_NULL(port_config.olp))
    {
        return BCMY_INVALID_VAL;
    }
    /* Find first cpu port */
    for (port = 0; port < BCM_PBMP_PORT_MAX; port++)
    {
        if (BCM_PBMP_MEMBER(port_config.olp, port))
        {
            break;
        }
    }
    /* Check if port was found */
    if (port >= BCM_PBMP_PORT_MAX)
    {
        return BCMY_INVALID_VAL;
    }

    /* Build gport */
    BCM_GPORT_MODPORT_SET(gport_ret, modid_base, port);

    return gport_ret;
}

/**
 * Return next OLP interface for this unit
 * 
 * @author mruas (04/08/20)
 * 
 * @param unit (in)
 * @param gport (in): OLP interface
 * 
 * @return bcm_gport_t : next OLP interface
 */
bcm_gport_t bcmy_gport_local_olp_get_next(int unit, bcm_gport_t gport)
{
    bcm_port_t port;
    bcm_gport_t gport_ret;
    bcm_port_config_t port_config;
    int modid_base, modid_number;
    int _modid, _modport;
    int rv;

    /* Validate gport */
    if (!bcmy_gport_is_valid(gport))
    {
        return BCMY_INVALID_VAL;
    }
    /* Get modid and modport */
    _modid   = BCM_GPORT_MODPORT_MODID_GET(gport);
    _modport = BCM_GPORT_MODPORT_PORT_GET(gport);
    /* Check if any error occurred */
    if (_modid < 0 || _modport < 0 || _modport >= BCM_PBMP_PORT_MAX)
    {
        return BCMY_INVALID_VAL;
    }

    /* Get base modid for this unit */
    if (bcmy_unit_to_modid(unit, &modid_base, &modid_number) != BCMY_E_NONE)
    {
        return BCMY_INVALID_VAL;
    }
    /* Modid of given port must match given modid */
    if (_modid < modid_base || _modid >= modid_base+modid_number)
    {
        return BCMY_INVALID_VAL;
    }

    /* get the cpu port bitmap */
    rv = bcm_port_config_get(unit, &port_config);
    if (rv != BCM_E_NONE)
    {
        return BCMY_INVALID_VAL;
    }
    /* Check if port bitmap is not null */
    if (BCM_PBMP_IS_NULL(port_config.olp))
    {
        return BCMY_INVALID_VAL;
    }
    /* Find first cpu port */
    for (port = _modport+1; port < BCM_PBMP_PORT_MAX; port++)
    {
        if (BCM_PBMP_MEMBER(port_config.olp, port))
        {
            break;
        }
    }
    /* Check if port was found */
    if (port >= BCM_PBMP_PORT_MAX)
    {
        return BCMY_INVALID_VAL;
    }

    /* Build gport */
    BCM_GPORT_MODPORT_SET(gport_ret, modid_base, port);

    return gport_ret;
}
#endif

/**
 * Return local/mod port associated to this gport
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in)
 * 
 * @return int : local port
 */
int bcmy_gport_bcm_port_get(bcm_gport_t gport)
{
    /* Validate gport */
    if (!bcmy_gport_is_valid(gport))
    {
        return BCMY_INVALID_VAL;
    }

    /* Check if this is a modport type */
    if (BCM_GPORT_IS_MODPORT(gport))
    {
        return BCM_GPORT_MODPORT_PORT_GET(gport);
    }
    else if (BCM_GPORT_IS_LOCAL(gport))
    {
        return BCM_GPORT_LOCAL_GET(gport);
    }
    else
    {
        return BCMY_INVALID_VAL;
    }
}

/**
 * Return local port associated to this gport
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in)
 * 
 * @return int : local port
 */
int bcmy_gport_localport_get(bcm_gport_t gport)
{
    /* Validate gport */
    if (!bcmy_gport_is_valid(gport))
    {
        return BCMY_INVALID_VAL;
    }
    /* Check if this is a modport type */
    if (!BCM_GPORT_IS_LOCAL(gport))
    {
        return BCMY_INVALID_VAL;
    }

    return BCM_GPORT_LOCAL_GET(gport);
}

/**
 * Return bcm_unit associated to this gport
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in)
 * 
 * @return int : bcm_unit
 */
int bcmy_gport_bcm_unit_get(bcm_gport_t gport)
{
    int modid, bcm_unit;
    int rv;

    /* Validate gport */
    if (!bcmy_gport_is_valid(gport))
    {
        return BCMY_INVALID_VAL;
    }

    /* Local type? */
    if (BCM_GPORT_IS_LOCAL(gport))
    {
        bcm_unit = 0;
    }
    /* modport type? */
    else if (BCM_GPORT_IS_MODPORT(gport))
    {
        /* Get modid */
        modid = BCM_GPORT_MODPORT_MODID_GET(gport);
        if (modid < 0)
        {
            return BCMY_INVALID_VAL;
        }

        /* From modid goto unit */
        rv = bcmy_modid_to_unit(modid, &bcm_unit);
        if (rv != BCMY_E_NONE)
        {
            return BCMY_INVALID_VAL;
        }
    }
    else
    {
        return BCMY_INVALID_VAL;
    }

    
    return bcm_unit;
}

/**
 * Return modid associated to this gport
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in)
 * 
 * @return int : modid
 */
int bcmy_gport_modid_get(bcm_gport_t gport)
{
    /* Validate gport */
    if (!bcmy_gport_is_valid(gport))
    {
        return BCMY_INVALID_VAL;
    }
    /* Check if this is a modport type */
    if (!BCM_GPORT_IS_MODPORT(gport))
    {
        return BCMY_INVALID_VAL;
    }
    
    return BCM_GPORT_MODPORT_MODID_GET(gport);
}

/**
 * Return modport associated to this gport
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in)
 * 
 * @return int : modport
 */
int bcmy_gport_modport_get(bcm_gport_t gport)
{
    /* Validate gport */
    if (!bcmy_gport_is_valid(gport))
    {
        return BCMY_INVALID_VAL;
    }
    /* Check if this is a modport type */
    if (!BCM_GPORT_IS_MODPORT(gport))
    {
        return BCMY_INVALID_VAL;
    }

    return BCM_GPORT_MODPORT_PORT_GET(gport);
}

/**
 * Convert bcm_unit to modid
 * 
 * @author mruas (04/08/20)
 * 
 * @param unit (in)
 * @param modid_base (out)
 * @param modid_number (out)
 * 
 * @return bcmy_error_t : 
 *       BCMY_E_NONE (success)
 *       BCMY_E_PARAM (invalid inputs)
 *       BCMY_E_FAIL (operation failed)
 */
bcmy_error_t bcmy_unit_to_modid(int unit, int *modid_base, int *modid_number)
{
    int _modid_base=-1, _modid_number=-1;
    bcmy_error_t ret = BCMY_E_NONE;
    int rv;

    /* Validate unit */
    if (unit >= bde->num_devices(BDE_SWITCH_DEVICES))
    {
        return BCMY_E_PARAM;
    }
    
#if 1
    /* Get modid base, and the number of modid's */
    rv = bcm_stk_modid_get(unit, &_modid_base);
    if (rv == BCM_E_NONE)
    {
        rv = bcm_stk_modid_count(unit, &_modid_number);
    }
    /* Any error? */
    if (rv != BCM_E_NONE)
    {
        ret = BCMY_E_FAIL;
    }
#else
#if defined (PLATFORM_CXO2T4)
    /* Valid modid's for CXO2T4 */
    if (bcm_unit >= 0 && bcm_unit <= 1)
    {
        _modid_base   = bcm_unit * 2;
        _modid_number = 2;
    }
#elif defined (PLATFORM_CXO2T2)
    /* Valid modid's for CXO2T2 */
    if (bcm_unit == 0)
    {
        /* If modid is not local to this board, return error! */
        if (ptin_fpga_board_is_working_side())
        {
            _modid_base = 0;
        }
        else
        {
            _modid_base = 2;
        }
        _modid_number = 1;
    }
#else
    /* Valid modid's for CXO2T0 */
    if (bcm_unit == 0)
    {
        _modid_base   = 0;
        _modid_number = 1;
    }
#endif
    else
    {
        ret = BCMY_E_PARAM;
    }
#endif

    /* Return values */
    if (ret == BCMY_E_NONE)
    {
        if (modid_base != NULLPTR)
        {
            *modid_base = _modid_base;
        }
        if (modid_number != NULLPTR)
        {
            *modid_number = _modid_number;
        }
    }

    return ret;
}

/**
 * Convert modid to unit
 * 
 * @author mruas (04/08/20)
 * 
 * @param modid (in)
 * @param bcm_unit (out)
 * 
 * @return bcmy_error_t : 
 *       BCMY_E_NONE (success)
 *       BCMY_E_PARAM (invalid inputs)
 */
bcmy_error_t bcmy_modid_to_unit(int modid, int *bcm_unit)
{
    int _bcm_unit = -1;

#if defined (PLATFORM_CXO2T4)
    /* Valid modid's for CXO2T4 */
    if (modid >= 0 && modid <= 3)
    {
        _bcm_unit = modid / 2;
    }
#elif defined (PLATFORM_CXO2T2)
    /* Valid modid's for CXO2T2 */
    if (modid == 0 || modid == 2)
    {
        _bcm_unit = 0;
    }
#else
    /* Valid modid's for CXO2T0 */
    if (modid == 0)
    {
        _bcm_unit = 0;
    }
#endif
    else
    {
        return BCMY_E_PARAM;
    }

    /* Return value */
    if (bcm_unit != NULLPTR)
    {
        *bcm_unit = _bcm_unit;
    }
    
    return BCMY_E_NONE;
}

/**
 * Return modid and modport associated to this gport
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in)
 * @param modid (out)
 * @param modport (out)
 * 
 * @return bcmy_error_t : 
 *       BCMY_E_NONE (success)
 *       BCMY_E_PARAM (invalid inputs)
 */
bcmy_error_t bcmy_gport_to_modid_port(bcm_gport_t gport, int *modid, bcm_port_t *modport)
{
    int _modid, _modport;

    /* Validate gport */
    if (!bcmy_gport_is_valid(gport))
    {
        return BCMY_E_PARAM;
    }
    /* Check if this is a modport type */
    if (!BCM_GPORT_IS_MODPORT(gport))
    {
        return BCMY_E_PARAM;
    }

    /* Get modid and modport */
    _modid   = BCM_GPORT_MODPORT_MODID_GET(gport);
    _modport = BCM_GPORT_MODPORT_PORT_GET(gport);
    /* Check if any error occurred */
    if (_modid < 0 || _modport < 0)
    {
        return BCMY_E_PARAM;
    }

    /* Return values */
    if (modid != NULLPTR)
    {
        *modid = _modid;
    }
    if (modport != NULLPTR)
    {
        *modport = _modport;
    }

    return BCMY_E_NONE;
}

/**
 * Return gport asscociated to a modid+modport
 * 
 * @author mruas (04/08/20)
 * 
 * @param modid (in)
 * @param port (in)
 * @param gport (out)
 * 
 * @return bcmy_error_t : 
 *       BCMY_E_NONE (success)
 *       BCMY_E_PARAM (invalid inputs)
 */
bcmy_error_t bcmy_modid_port_to_gport(int modid, bcm_port_t port, bcm_gport_t *gport)
{
    bcm_gport_t _gport;

    /* Validate params */
    if (modid < 0 || port < 0)
    {
        return BCMY_E_PARAM;
    }
    
    BCM_GPORT_MODPORT_SET(_gport, modid, port);

    /* Return value */
    if (gport != NULLPTR)
    {
        *gport = _gport;
    }
    
    return BCMY_E_NONE;
}

/**
 * Return bcm_unit/port associated to a gport
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in)
 * @param unit (out)
 * @param port (out)
 * 
 * @return bcmy_error_t : 
 *       BCMY_E_NONE (success)
 *       BCMY_E_PARAM (invalid inputs)
 */
bcmy_error_t bcmy_gport_to_unit_port(bcm_gport_t gport, int *unit, bcm_port_t *port)
{
    int bcm_unit, modid, modport;
    int rv;

    /* Validate gport */
    if (!bcmy_gport_is_valid(gport))
    {
        return BCMY_E_PARAM;
    }

    /* Local type? */
    if (BCM_GPORT_IS_LOCAL(gport))
    {
        bcm_unit = 0;
        modid    = 0;
        modport  = BCM_GPORT_LOCAL_GET(gport);
    }
    /* modport type? */
    else if (BCM_GPORT_IS_MODPORT(gport))
    {
        /* Get modid and modport */
        modid   = BCM_GPORT_MODPORT_MODID_GET(gport);
        modport = BCM_GPORT_MODPORT_PORT_GET(gport);
        /* Check if any error occurred */
        if (modid < 0 || modport < 0)
        {
            return BCMY_E_PARAM;
        }

        /* From modid goto unit */
        rv = bcmy_modid_to_unit(modid, &bcm_unit);
        if (rv != BCMY_E_NONE)
        {
            return rv;
        }
    }
    else
    {
        return BCMY_E_PARAM;
    }

    /* Return values */
    if (unit != NULLPTR)
    {
        *unit = bcm_unit;
    }
    if (port != NULLPTR)
    {
        *port = modport;
    }

    return BCMY_E_NONE;
}

/**
 * Return gport associated to a bcm_unit/port
 * 
 * @author mruas (04/08/20)
 * 
 * @param unit (in)
 * @param port (in)
 * @param gport (out)
 * 
 * @return bcmy_error_t : 
 *       BCMY_E_NONE (success)
 *       BCMY_E_PARAM (invalid inputs)
 */
bcmy_error_t bcmy_unit_port_to_gport(int unit, bcm_port_t port, bcm_gport_t *gport)
{
    bcm_gport_t _gport;
    int modid_base, modid_number;
    int rv;

    /* Validate params */
    if (unit < 0 || port < 0)
    {
        return BCMY_E_PARAM;
    }

    /* Get modid data */
    rv = bcmy_unit_to_modid(unit, &modid_base, &modid_number);
    if (rv != BCMY_E_NONE)
    {
        return rv;
    }

    /* Calculate gport */
    BCM_GPORT_MODPORT_SET(_gport, modid_base, port);
    
    /* Return gport */
    if (gport != NULLPTR)
    {
        *gport = _gport;
    }
    
    return BCMY_E_NONE;
}


/********************************************************************
 * GPORT LOOKUP TABLE TO ACESS USP AND BCM_UNIT/PORT INFORMATION    *
 ********************************************************************/

#define BCMY_BCM_UNITS_MAX  BCM_UNITS_MAX
#define BCMY_BCM_PORTS_MAX  BCM_PBMP_PORT_MAX

#define BCMY_USP_UNITS_MIN  1
#define BCMY_USP_UNITS_MAX  (L7_MAX_UNITS_PER_STACK+1)
#define BCMY_USP_SLOTS_MAX  L7_MAX_SLOTS_PER_UNIT
#define BCMY_USP_PORTS_MAX  L7_MAX_PORTS_PER_SLOT

static unsigned int _max_bcm_units = 0;
static unsigned int _max_bcm_ports = 0;
static unsigned int _max_usp_units = BCMY_USP_UNITS_MIN;
static unsigned int _max_usp_slots = 0;
static unsigned int _max_usp_ports = 0;

static BOOL _lut_initialized = FALSE;

/* This is the way to associate a gport to a unit/port (only physical), and to a USP (all ports) */
static DAPI_USP_t  table_unit_port_to_usp[BCMY_BCM_UNITS_MAX][BCMY_BCM_PORTS_MAX];
static bcm_gport_t table_unit_port_to_gport[BCMY_BCM_UNITS_MAX][BCMY_BCM_PORTS_MAX];
static bcm_gport_t table_usp_to_gport[BCMY_USP_UNITS_MAX][BCMY_USP_SLOTS_MAX][BCMY_USP_PORTS_MAX];

#define MAX_GPORTS  ((BCMY_USP_UNITS_MAX-BCMY_USP_UNITS_MIN)*BCMY_USP_SLOTS_MAX*BCMY_USP_PORTS_MAX)
static bcmy_gplist_t internal_gport_list;

/**
 * Initialize internal structures.
 * 
 * @author mruas (04/08/20) 
 *  
 * @return bcmy_error_t : 
 *      BCMY_E_NONE (Success)
 *      BCMY_E_PARAM (Invalid inputs)
 *      BCMY_E_FAIL (Init failed)
 *      BCMY_E_MEMORY (Memory allocation error)
 */
bcmy_error_t bcmy_lut_gport_init(void)
{
    int unit, slot, port;
    bcmy_error_t rv;

    /* Reset structures */
    for (unit = 0; unit < BCMY_BCM_UNITS_MAX; unit++)
    {
        for (port = 0; port < BCMY_BCM_PORTS_MAX; port++)
        {
            /* Clear table_unit_port_to_gport table */
            table_unit_port_to_gport[unit][port] = BCM_GPORT_INVALID;

            /* Clear table_unit_port_to_usp table */
            table_unit_port_to_usp[unit][port].unit = -1;
            table_unit_port_to_usp[unit][port].slot = -1;
            table_unit_port_to_usp[unit][port].port = -1;
        }
    }

    for (unit = 0; unit < BCMY_USP_UNITS_MAX; unit++)
    {
        for (slot = 0; slot < BCMY_USP_SLOTS_MAX; slot++)
        {
            for (port = 0; port < BCMY_USP_PORTS_MAX; port++)
            {
                /* Clear table_usp_to_gport table */
                table_usp_to_gport[unit][slot][port] = BCM_GPORT_INVALID;
            }
        }
    }

    bcmy_gplist_t_init(&internal_gport_list);
    rv = bcmy_gplist_init(&internal_gport_list, MAX_GPORTS, 0);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
    
    /* Tables initialized */
    _lut_initialized = TRUE;

    return BCMY_E_NONE;
}

/**
 * Obtain USP from bcm_unit/port inputs
 * 
 * @author mruas (04/08/20)
 * 
 * @param unit (in)
 * @param port (in)
 * @param usp (out)
 * 
 * @return bcmy_error_t : 
 *      BCMY_E_NONE (Success)
 *      BCMY_E_PARAM (Invalid inputs)
 *      BCMY_E_INIT (Missing initialization)
 *      BCMY_E_CONFIG (Entry not configured)
 */
bcmy_error_t bcmy_lut_unit_port_to_usp_get(int unit, int port, DAPI_USP_t *usp)
{
    /* Check if tables were initialized */
    if (!_lut_initialized)
    {
        return BCMY_E_INIT;
    }
    
    /* Validate input params */
    if (unit < 0 || unit >= _max_bcm_units ||
        port < 0 || port >= _max_bcm_ports)
    {
        return BCMY_E_PARAM;
    }

    /* Check if entry was initialized (should not be null) */
    if (table_unit_port_to_usp[unit][port].unit < 0 ||
        table_unit_port_to_usp[unit][port].slot < 0 ||
        table_unit_port_to_usp[unit][port].port < 0)
    {
        return BCMY_E_CONFIG;
    }

    /* Return gport */
    if (usp != NULLPTR)
    {
        usp->unit = table_unit_port_to_usp[unit][port].unit;
        usp->slot = table_unit_port_to_usp[unit][port].slot;
        usp->port = table_unit_port_to_usp[unit][port].port;
    }

    return BCMY_E_NONE;
}

/**
 * Obtain gport from USP input
 * 
 * @author mruas (04/08/20)
 * 
 * @param usp (in)
 * @param gport (out)
 * 
 * @return bcmy_error_t : 
 *      BCMY_E_NONE (Success)
 *      BCMY_E_PARAM (Invalid inputs)
 *      BCMY_E_INIT (Missing initialization)
 *      BCMY_E_CONFIG (Entry not configured)
 */
bcmy_error_t bcmy_lut_usp_to_gport_get(DAPI_USP_t usp, bcm_gport_t *gport)
{
    /* Check if tables were initialized */
    if (!_lut_initialized)
    {
        return BCMY_E_INIT;
    }

    /* Check if USP is valid */
    if (usp.unit < BCMY_USP_UNITS_MIN || usp.unit >=  _max_usp_units ||
        usp.slot < 0 || usp.slot >= _max_usp_slots ||
        usp.port < 0 || usp.port >= _max_usp_ports)
    {
        return BCMY_E_PARAM;
    }

    /* Entry must be cleared, before set (to avoid programming bugs) */
    if (table_usp_to_gport[usp.unit][usp.slot][usp.port] == BCM_GPORT_INVALID)
    {
        return BCMY_E_CONFIG;
    }

    /* Return gport */
    if (gport != NULLPTR)
    {
        *gport = table_usp_to_gport[usp.unit][usp.slot][usp.port];
    }

    return BCMY_E_NONE;
}

/**
 * Obtain gport from bcm_unit/port inputs
 * 
 * @author mruas (04/08/20)
 * 
 * @param unit (in)
 * @param port (in)
 * @param gport (out)
 * 
 * @return bcmy_error_t : 
 *      BCMY_E_NONE (Success)
 *      BCMY_E_PARAM (Invalid inputs)
 *      BCMY_E_INIT (Missing initialization)
 *      BCMY_E_CONFIG (Entry not configured)
 */
bcmy_error_t bcmy_lut_unit_port_to_gport_get(int unit, int port, bcm_gport_t *gport)
{
    /* Check if tables were initialized */
    if (!_lut_initialized)
    {
        return BCMY_E_INIT;
    }

    /* Validate input params */
    if (unit < 0 || unit >= _max_bcm_units ||
        port < 0 || port >= _max_bcm_ports)
    {
        return BCMY_E_PARAM;
    }

    /* Check if entry have a valid value */
    if (table_unit_port_to_gport[unit][port] == BCM_GPORT_INVALID)
    {
        return BCMY_E_CONFIG;
    }

    /* Return gport */
    if (gport != NULLPTR)
    {
        *gport = table_unit_port_to_gport[unit][port];
    }

    return BCMY_E_NONE;
}

/**
 * Obtain usp from gport
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in)
 * @param usp (out)
 * 
 * @return bcmy_error_t : 
 *      BCMY_E_NONE (Success)
 *      BCMY_E_PARAM (Invalid inputs)
 *      BCMY_E_NOT_FOUND (Entry not found)
 */
bcmy_error_t bcmy_lut_gport_to_usp_get(bcm_gport_t gport, DAPI_USP_t *usp)
{
    int usp_unit, usp_slot, usp_port;

    /* Check if tables were initialized */
    if (!_lut_initialized)
    {
        return BCMY_E_INIT;
    }

    /* Validate gport */
    if (!bcmy_gport_is_valid(gport))
    {
        return BCMY_E_PARAM;
    }

    /* Check if this gport is already present */
    for (usp_unit = BCMY_USP_UNITS_MIN; usp_unit < _max_usp_units; usp_unit++)
    {
        for (usp_slot = 0; usp_slot < _max_usp_slots; usp_slot++)
        {
            for (usp_port = 0; usp_port < _max_usp_ports; usp_port++)
            {
                if (table_usp_to_gport[usp_unit][usp_slot][usp_port] == gport)
                {
                    if (usp != NULLPTR)
                    {
                        usp->unit = usp_unit;
                        usp->slot = usp_slot;
                        usp->port = usp_port;
                    }

                    return BCMY_E_NONE;
                }
            }
        }
    }

    /* Not found */
    return BCMY_E_NOT_FOUND;
}

/**
 * Obtain bcm_unit/port from gport
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in)
 * @param unit (out)
 * @param port (out)
 * 
 * @return bcmy_error_t : 
 *      BCMY_E_NONE (Success)
 *      BCMY_E_PARAM (Invalid inputs)
 *      BCMY_E_NOT_FOUND (Entry not found)
 */
bcmy_error_t bcmy_lut_gport_to_unit_port_get(bcm_gport_t gport, int *unit, int *port)
{
    int bcm_unit, bcm_port;

    /* Check if tables were initialized */
    if (!_lut_initialized)
    {
        return BCMY_E_INIT;
    }

    /* Validate gport */
    if (!bcmy_gport_is_valid(gport))
    {
        return BCMY_E_PARAM;
    }

    /* Check if this gport is already present */
    for (bcm_unit = 0; bcm_unit < _max_bcm_units; bcm_unit++)
    {
        for (bcm_port = 0; bcm_port < _max_bcm_ports; bcm_port++)
        {
            /* If entry found, return unit and port */
            if (table_unit_port_to_gport[bcm_unit][bcm_port] == gport)
            {
                if (unit != NULLPTR)
                {
                    *unit = bcm_unit;
                }
                if (port != NULLPTR)
                {
                    *port = bcm_port;
                }

                return BCMY_E_NONE;
            }
        }
    }

    /* Not found */
    return BCMY_E_NOT_FOUND;
}

/**
 * Associate a gport to a bcm_unit/port and USP interfaces
 * 
 * @author mruas (04/08/20)
 * 
 * @param gport (in) 
 * @param unit (in) 
 * @param port (in) 
 * @param usp (in)
 *  
 * @return bcmy_error_t : 
 *      BCMY_E_NONE (Success)
 *      BCMY_E_PARAM (invalid inputs)
 *      BCMY_E_INIT (missing initialization)
 *      BCMY_E_CONFIG (entry already configured)
 *      BCMY_E_EXISTS (gport already exists in table)
 */
bcmy_error_t bcmy_lut_gport_set(bcm_gport_t gport, int unit, int port, DAPI_USP_t *usp)
{
    int bcm_unit, bcm_port;
    int usp_unit, usp_slot, usp_port;
    bcmy_error_t rv;

    /* Initialize tables if not done previously */
    if (!_lut_initialized)
    {
        rv = bcmy_lut_gport_init();
        if (rv != BCMY_E_NONE)
        {
            return rv;
        }
    }

    /* Validate input parameters */

    /* Validate gport */
    if (!bcmy_gport_is_valid(gport))
    {
        return BCMY_E_PARAM;
    }
    /* Check if unit and port are within bounds */
    if (unit >= BCMY_BCM_UNITS_MAX ||
        port >= BCMY_BCM_PORTS_MAX)
    {
        return BCMY_E_PARAM;
    }
    /* Check if USP is valid and within bounds */
    if (usp != NULLPTR)
    {
        if (usp->unit < BCMY_USP_UNITS_MIN || usp->unit >= BCMY_USP_UNITS_MAX ||
            usp->slot < 0 || usp->slot >= BCMY_USP_SLOTS_MAX ||
            usp->port < 0 || usp->port >= BCMY_USP_PORTS_MAX)
        {
            return BCMY_E_PARAM;
        }
    }

    /* Unit and port are given? If so, gport must be physical type */
    if (unit >= 0 && port >= 0)
    {
        /* Entry must be cleared, before set (to avoid programming bugs) */
        if (table_unit_port_to_gport[unit][port] != BCM_GPORT_INVALID)
        {
            return BCMY_E_CONFIG;
        }
        
        /* Check if this gport is already present in tables */
        for (bcm_unit = 0; bcm_unit < _max_bcm_units; bcm_unit++)
        {
            for (bcm_port = 0; bcm_port < _max_bcm_ports; bcm_port++)
            {
                /* Skip port to be set */
                if (bcm_unit == unit && bcm_port == port)
                {
                    continue;
                }

                /* Check table_unit_port_to_gport table to see if entry already exists */
                if (table_unit_port_to_gport[bcm_unit][bcm_port] == gport)
                {
                    return BCMY_E_EXISTS;
                }

                /* Check table_unit_port_to_usp table to see if entry already exists */
                if (usp != NULLPTR)
                {
                    if (table_unit_port_to_usp[bcm_unit][bcm_port].unit == usp->unit &&
                        table_unit_port_to_usp[bcm_unit][bcm_port].slot == usp->slot &&
                        table_unit_port_to_usp[bcm_unit][bcm_port].port == usp->port)
                    {
                        return BCMY_E_EXISTS;
                    }
                }
            }
        }
    }

    /* Check if USP is valid and within bounds */
    if (usp != NULLPTR)
    {
        /* Entry must be cleared, before set (to avoid programming bugs) */
        if (table_usp_to_gport[usp->unit][usp->slot][usp->port] != BCM_GPORT_INVALID)
        {
            return BCMY_E_CONFIG;
        }

        /* Check if this gport is already present in tables */
        for (usp_unit = BCMY_USP_UNITS_MIN; usp_unit < _max_usp_units; usp_unit++)
        {
            for (usp_slot = 0; usp_slot < _max_usp_slots; usp_slot++)
            {
                for (usp_port = 0; usp_port < _max_usp_ports; usp_port++)
                {
                    /* Skip usp_port to be set */
                    if (usp_unit == usp->unit && usp_slot == usp->slot && usp_port == usp->port)
                    {
                        continue;
                    }

                    /* Check table_usp_to_gport table to see if entry already exists */
                    if (table_usp_to_gport[usp_unit][usp_slot][usp_port] == gport)
                    {
                        return BCMY_E_EXISTS;
                    }
                }
            }
        }
    }

    /* Add gport to GPLIST */
    rv = bcmy_gplist_add(&internal_gport_list, gport);
    if (rv != BCMY_E_NONE)
    {
        return rv;
    }
    
    /* Update tables */
    if (unit >= 0 && port >= 0 && usp != NULLPTR)
    {
        table_unit_port_to_usp[unit][port].unit = usp->unit;
        table_unit_port_to_usp[unit][port].slot = usp->slot;
        table_unit_port_to_usp[unit][port].port = usp->port;
    }
    if (unit >= 0 && port >= 0)
    {
        table_unit_port_to_gport[unit][port] = gport;

        /* Update maximum indexes */
        if (unit >= _max_bcm_units)  _max_bcm_units = unit + 1;
        if (port >= _max_bcm_ports)  _max_bcm_ports = port + 1;
    }
    if (usp != NULLPTR)
    {
        table_usp_to_gport[usp->unit][usp->slot][usp->port] = gport;

        /* Update maximum indexes */
        if (usp->unit >= _max_usp_units)  _max_usp_units = usp->unit + 1;
        if (usp->slot >= _max_usp_slots)  _max_usp_slots = usp->slot + 1;
        if (usp->port >= _max_usp_ports)  _max_usp_ports = usp->port + 1;
    }

    return BCMY_E_NONE;
}

/**
 * Reset gport entry at LUT tables
 * 
 * @author mruas (04/08/20)
 *  
 * @param unit (in) 
 * @param port (in) 
 * @param usp (in)
 *  
 * @return bcmy_error_t : 
 *      BCMY_E_NONE (Success)
 *      BCMY_E_PARAM (invalid inputs)
 *      BCMY_E_CONFIG (inconsistent configurations)
 */
bcmy_error_t bcmy_lut_gport_reset(int unit, int port, DAPI_USP_t *usp)
{
    bcm_gport_t gport = BCM_GPORT_INVALID;

    /* Validate params */
    if (unit >= BCMY_BCM_UNITS_MAX ||
        port >= BCMY_BCM_PORTS_MAX)
    {
        return BCMY_E_PARAM;
    }
    if (usp != NULLPTR)
    {
        /* Check if USP is valid and within bounds */
        if (usp->unit < BCMY_USP_UNITS_MIN || usp->unit >= BCMY_USP_UNITS_MAX ||
            usp->slot < 0 || usp->slot >= BCMY_USP_SLOTS_MAX ||
            usp->port < 0 || usp->port >= BCMY_USP_PORTS_MAX)
        {
            return BCMY_E_PARAM;
        }
    }

    /* If all inputs are provided */
    if (unit >= 0 && port >= 0 && usp != NULLPTR)
    {
        /* Check data consistency */
        if (table_unit_port_to_gport[unit][port] !=
            table_usp_to_gport[usp->unit][usp->slot][usp->port])
        {
            return BCMY_E_CONFIG;
        }
    }
    /* Unit and port are valid? */
    if (unit >= 0 && port >= 0)
    {
        /* Save gport to be removed */
        gport = table_unit_port_to_gport[unit][port];

        /* Clear entry at table_unit_port_to_gport table */
        table_unit_port_to_gport[unit][port] = BCM_GPORT_INVALID;

        /* Clear entry at table_unit_port_to_usp table */
        table_unit_port_to_usp[unit][port].unit = -1;
        table_unit_port_to_usp[unit][port].slot = -1;
        table_unit_port_to_usp[unit][port].port = -1;
    }
    /* USP is given? */
    if (usp != NULLPTR)
    {
        /* Save gport to be removed */
        gport = table_usp_to_gport[usp->unit][usp->slot][usp->port];

        /* Clear entry at table_usp_to_gport table */
        table_usp_to_gport[usp->unit][usp->slot][usp->port] = BCM_GPORT_INVALID;
    }

    /* Update gplist */
    if (gport != BCM_GPORT_INVALID)
    {
        bcmy_gplist_remove(&internal_gport_list, gport);
    }
    
    return BCMY_E_NONE;
}

/**
 * Return configured gport in lookup tables
 * 
 * @author mruas (06/08/20)
 * 
 * @param index : use index to iterate over all gports
 * 
 * @return bcm_gport_t 
 */
bcm_gport_t bcmy_gport_list_get(int index)
{
    /* Check if tables were initialized */
    if (!_lut_initialized)
    {
        return BCM_GPORT_INVALID;
    }
    /* Validate index */
    if (index >= MAX_GPORTS)
    {
        return BCM_GPORT_INVALID;
    }
    
    /* Return gport */
    return bcmy_gplist_index(&internal_gport_list, index);
}

void bcmy_lut_gport_dump(void)
{
    int i;
    int bcm_unit, bcm_port;
    int usp_unit, usp_slot, usp_port;

    printf("\r\ntable_unit_port_to_gport[bcm_unit][bcm_port] = gport");
    for (bcm_unit = 0; bcm_unit < _max_bcm_units; bcm_unit++)
    {
        printf("\r\n# BCM Unit %d",  bcm_unit);
        i = 0;
        for (bcm_port = 0; bcm_port < _max_bcm_ports; bcm_port++)
        {
            if (table_unit_port_to_gport[bcm_unit][bcm_port] != BCM_GPORT_INVALID)
            {
                if (i%8 == 0)
                {
                    printf("\r\n  ");
                }
                i++;
                printf("%3d:0x%08x  ", bcm_port, table_unit_port_to_gport[bcm_unit][bcm_port]);
            }
        }
    }

    printf("\r\n\ntable_unit_port_to_usp[bcm_unit][bcm_port] = usp:{unit,slot,port}");
    for (bcm_unit = 0; bcm_unit < _max_bcm_units; bcm_unit++)
    {
        printf("\r\n# BCM Unit %d",  bcm_unit);
        i = 0;
        for (bcm_port = 0; bcm_port < _max_bcm_ports; bcm_port++)
        {
            if (table_unit_port_to_usp[bcm_unit][bcm_port].unit >= 0)
            {
                if (i%8 == 0)
                {
                    printf("\r\n  ");
                }
                i++;
                printf("%3d:{%d,%d,%3d}  ", bcm_port,
                       table_unit_port_to_usp[bcm_unit][bcm_port].unit,
                       table_unit_port_to_usp[bcm_unit][bcm_port].slot,
                       table_unit_port_to_usp[bcm_unit][bcm_port].port);
            }
        }
    }

    printf("\r\n\ntable_usp_to_gport[usp:unit][usp:slot][usp:port] = gport");
    for (usp_unit = BCMY_USP_UNITS_MIN; usp_unit < _max_usp_units; usp_unit++)
    {
        for (usp_slot = 0; usp_slot < _max_usp_slots; usp_slot++)
        {
            printf("\r\n# USP Unit %d, Slot %u",  usp_unit, usp_slot);
            i = 0;
            for (usp_port = 0; usp_port < _max_usp_ports; usp_port++)
            {
                if (table_usp_to_gport[usp_unit][usp_slot][usp_port] != BCM_GPORT_INVALID)
                {
                    if (i%8 == 0)
                    {
                        printf("\r\n  ");
                    }
                    i++;
                    printf("%3d:0x%08x  ", usp_port, table_usp_to_gport[usp_unit][usp_slot][usp_port]);
                }
            }
        }
    }

    printf("\r\n\n");
    printf("_max_bcm_units=%u  _max_bcm_ports=%u\r\n",
           _max_bcm_units, _max_bcm_ports);
    printf("_max_usp_units=%u  _max_usp_slots=%u  _max_usp_ports=%u\r\n",
           _max_usp_units, _max_usp_slots, _max_usp_ports);
}

/********************************************************************
 * GPLIST MANIPULATION FUNCTIONS                                    *
 ********************************************************************/

/* This is min size of a list when allocated. */
#ifndef BCMY_GPLIST_MIN_COUNT
#define BCMY_GPLIST_MIN_COUNT 16
#endif

/* This is the increment when reallocating */
#ifndef BCMY_GPLIST_MIN_INCREMENT
#define BCMY_GPLIST_MIN_INCREMENT 16
#endif

/* Is list empty? */
#define GP_EMPTY(list)  ((list)->gp_last < 0)

/* Number of elements currently in list */
#define GP_COUNT(list)  ((list)->gp_last + 1)

/* Size of an 'n' list elements */
#define GP_ELSIZE(n) ((n) * sizeof(bcm_gport_t))

/* Next free cell on the list.  No updates to list. */
#define GP_FIRST_FREE(list)    ((list)->gp_ports[(list)->gp_last + 1])


/*
 * Function:
 *      _bcmy_gplist_realloc
 * Purpose:
 *      Re-allocate a port list
 * Parameters:
 *      list       -  The port list to change
 *      new_count  -  The new count of ports
 * Returns:
 *      BCMY_E_XXX
 */

STATIC int
_bcmy_gplist_realloc(bcmy_gplist_t *list, int new_count)
{
    bcm_gport_t *old_ports;

    if (list == NULL || list->gp_ports == NULL) {
        return BCMY_E_INTERNAL;
    }

    old_ports = list->gp_ports;

    list->gp_ports = (bcm_gport_t *)
        sal_alloc(GP_ELSIZE(new_count), "lplist");

    if (!list->gp_ports) {
        list->gp_ports = old_ports;
        return BCMY_E_MEMORY;
    }

    sal_memcpy(list->gp_ports, old_ports, GP_ELSIZE(GP_COUNT(list)));

    list->gp_alloc = new_count;

    sal_free(old_ports);

    return BCMY_E_NONE;
}

/*
 * Function:
 *      bcmy_gplist_init
 * Purpose:
 *      Allocate and initialize a port list
 * Parameters:
 *      list        - The port list to initialize
 *      init_count  - The initial count to use
 *      flags       - List flags - deprecated and ignored
 * Returns:
 *      BCMY_E_XXX
 * Notes:
 *      If successful, then list->gp_max != 0.
 *      At least BCMY_GPLIST_MIN_COUNT ports will be allocated.
 *      The port list structure itself must be allocated already
 *      (that is, list != NULL).
 */

int
bcmy_gplist_init(bcmy_gplist_t *list, int init_count, uint32 flags)
{
    int ports = BCMY_GPLIST_MIN_COUNT;

    if (!list) {
        return BCMY_E_PARAM;
    }

    if (init_count > ports) {
        ports = init_count;
    }

    list->gp_ports = sal_alloc(GP_ELSIZE(ports), "lpports");
    if (!list->gp_ports) {
        return BCMY_E_MEMORY;
    }

    list->gp_alloc = ports;
    list->gp_last = -1;

    return BCMY_E_NONE;
}

/*
 * Function:
 *      bcmy_gplist_t_init
 * Purpose:
 *      Initialize the bcmy_gplist_t structure.
 *      Allocate and initialize a port list.
 * Parameters:
 *      list       -  Pointer to port list structure to initialize
 * Returns:
 *      None
 * Note:
 *      Although the functionality of this routine is also provided by
 *      bcmy_gplist_init, this has an API interface consistent
 *      with other structure initialization/free routines.
 */
void
bcmy_gplist_t_init(bcmy_gplist_t *list)
{
    bcmy_gplist_init(list, 0, 0);
}

/*
 * Function:
 *      bcmy_gplist_free
 * Purpose:
 *      De-allocate a port list
 * Parameters:
 *      list        - The port list to de-allocate
 * Returns:
 *      BCMY_E_XXX
 */

int
bcmy_gplist_free(bcmy_gplist_t *list)
{
    int rv = BCMY_E_PARAM;

    if (!bcmy_gplist_is_null(list)) {
        if (list->gp_ports) {
            sal_free(list->gp_ports);
        }
        list->gp_ports = 0;
        rv = BCMY_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *      bcmy_gplist_t_free
 * Purpose:
 *      De-allocate a port list
 * Parameters:
 *      list        - The port list to de-allocate
 * Returns:
 *      None
 * Note:
 *      Although the functionality of this routine is also provided by
 *      bcmy_gplist_free, this has an API interface consistent
 *      with other structure initialization/free routines.
 */
void
bcmy_gplist_t_free(bcmy_gplist_t *list)
{
    bcmy_gplist_free(list);
}

/*
 * Function:
 *      bcmy_gplist_clear
 * Purpose:
 *      Make a port list empty
 * Parameters:
 *      list       -  The port list to clear
 * Returns:
 *      BCMY_E_XXX
 * Notes:
 *      
 */

int
bcmy_gplist_clear(bcmy_gplist_t *list)
{
    if (!bcmy_gplist_is_null(list)) {
        list->gp_last = -1;
    }

    return BCMY_E_NONE;
}


/****************************************************************
 *
 * The base Logical Port List functions.
 *
 * See also the macros BCM_GPLIST_REMOVE, etc.
 *
 ****************************************************************/


/*
 * Function:
 *      bcmy_gplist_index_get
 * Purpose:
 *      Get the index of the first occurrance of a port on the list
 * Parameters:
 *      list       -- The list to search
 * Returns:
 *      Index >= 0 if found; otherwise BCMY_E_NOT_FOUND
 */

int
bcmy_gplist_index_get(bcmy_gplist_t *list, bcm_gport_t port)
{
    return bcmy_gplist_index_get_from(list, 0, port);
}


/*
 * Function:
 *      bcmy_gplist_index_get_from
 * Purpose:
 *      Get the index of the first occurrance of a port on the list
 *      starting at position
 * Parameters:
 *      list       -- The list to search
 *      position   -- The position to start the search
 * Returns:
 *      BCMY_E_PARAM if position is out of range
 *      Index >= 0 if found; otherwise BCMY_E_NOT_FOUND
 */

int
bcmy_gplist_index_get_from(bcmy_gplist_t *list, int position, bcm_gport_t port)
{
    int i;

    if (bcmy_gplist_is_empty(list)) {
        return BCMY_E_NOT_FOUND;
    }

    if (position < 0 || position > list->gp_last) {
        return BCMY_E_PARAM;
    }

    for (i = position; i <= list->gp_last; i++) {
        if (list->gp_ports[i] == port) {
            return i;
        }
    }

    return BCMY_E_NOT_FOUND;
}

/*
 * Function:
 *      bcmy_gplist_index
 * Purpose:
 *      Get the gport at position
 * Parameters:
 *      list       -- The list to index
 *      position   -- The position to retrieve gport
 * Returns:
 *      BCMX_NO_SUCH_LPORT if position is out of range
 *      otherwise gport
 */

bcm_gport_t
bcmy_gplist_index(bcmy_gplist_t *list, int position)
{
    bcm_gport_t gport = BCM_GPORT_INVALID;

    if (!bcmy_gplist_is_null(list) &&
        position >= 0 && position <= list->gp_last) {
        gport = list->gp_ports[position];
    }

    return gport;
}

/*
 * Function:
 *      bcmy_gplist_add
 * Purpose:
 *      Add a port to the end of a lplist
 * Parameters:
 *      list       -- The list to update
 *      gport      -- The port to add
 * Returns:
 *      BCMY_E_XXX
 * Notes:
 *      Will initialize the list if it is NULL.
 *      Reallocates if not enough space for list.
 */

int
bcmy_gplist_add(bcmy_gplist_t *list, bcm_gport_t gport)
{
    if (bcmy_gplist_is_null(list)) {
        BCM_IF_ERROR_RETURN(bcmy_gplist_init(list, -1, 0));
    }

    if (BCMY_GPLIST_COUNT(list) + 1 >= list->gp_alloc) {
        if (_bcmy_gplist_realloc(list, list->gp_alloc +
                                 BCMY_GPLIST_MIN_INCREMENT)
            != BCMY_E_NONE) {
            return BCMY_E_MEMORY;
        }
    }

    (list->gp_last)++;
    list->gp_ports[list->gp_last] = gport;

    return BCMY_E_NONE;
}


/*
 * Function:
 *      _bcmy_gplist_pbmp_add
 * Purpose:
 *      
 * Parameters:
 *      list       -  The port list to access
 *      unit
 *      pbm
 * Returns:
 * Notes:
 */


int
_bcmy_gplist_pbmp_add(bcmy_gplist_t *list, int unit, bcm_pbmp_t pbm)
{
    bcm_gport_t gport;
    bcm_port_t port;
    int rv = BCMY_E_NONE;

    BCM_PBMP_ITER(pbm, port) {
        if (bcmy_unit_port_to_gport(unit, port, &gport) != BCMY_E_NONE) {
            rv = BCMY_E_PARAM;
            break;
        }
    	rv = bcmy_gplist_add(list, gport);
    	if (rv < 0) {
    	    break;
    	}
    }

    return rv;
}

/*
 * Function:
 *      bcmy_gplist_port_remove
 * Purpose:
 *      Remove a port from the list.  
 * Parameters:
 *      list     - The list to act on
 *      gport    - The logical port to look for and remove
 *      all      - If true, remove all occurances; otherwise just first.
 * Returns:
 *      BCMY_E_PARAM - parameter error
 *      BCMY_E_NONE  - port removed
 * Notes:
 *
 */

int
bcmy_gplist_port_remove(bcmy_gplist_t *list, bcm_gport_t gport, int all)
{
    int rv = BCMY_E_NONE;
    int count = 0;
    int idx = 0;

    while ((idx=bcmy_gplist_index_get_from(list, idx, gport)) >= 0) {
        count++;
        bcmy_gplist_idx_remove(list, idx);
        if (!all || (idx > list->gp_last)) {
            break;
        }
    }

    if ((idx < 0 && idx != BCMY_E_NOT_FOUND) || count == 0) {
        rv = idx;
    }
   

    return rv;
}

/*
 * Function:
 *      bcmy_gplist_eq
 * Purpose:
 *      Compare two port lists
 * Parameters:
 *      list1   - List to examine
 *      list2   - List to examine
 * Returns:
 *      1 if equal, 0 if not
 * Notes:
 *     Just a simple check that every port in one list is in the other.
 *     Thus it doesn't detect differences in order or multiplicity.
 */

int
bcmy_gplist_eq(bcmy_gplist_t *list1, bcmy_gplist_t *list2)
{
    bcm_gport_t gport;
    int count;
    int list1_empty, list2_empty;

    list1_empty = bcmy_gplist_is_empty(list1);
    list2_empty = bcmy_gplist_is_empty(list2);

    if (list1_empty && list2_empty) {
        /* Both lists are empty */
        return 1;
    } else if (list1_empty != list2_empty) {
        /* One list is empty, the other not empty */
        return 0;
    } else {
        /* Both lists not empty */
        BCMY_GPLIST_IDX_ITER(list1, gport, count) {
            if (bcmy_gplist_index_get(list2, gport) < 0) {
                return 0;
            }
        }

        BCMY_GPLIST_IDX_ITER(list2, gport, count) {
            if (bcmy_gplist_index_get(list1, gport) < 0) {
                return 0;
            }
        }
    }
    return 1;
}

/*
 * Function:
 *      bcmy_gplist_append
 * Purpose:
 *      Copy list2 to end of list1
 * Parameters:
 *      list1     - List to change
 *      list2     - List copied onto the end of list1
 *
 * Returns:
 *      BCMY_E_XXX
 * Notes:
 *      Does not support sorted lists.
 *      Does not support extra flags for port types
 */

int
bcmy_gplist_append(bcmy_gplist_t *list1, bcmy_gplist_t *list2)
{
    int total;

    if (bcmy_gplist_is_null(list1)) {
        BCM_IF_ERROR_RETURN(bcmy_gplist_init(list1, -1, 0));
    }

    if (bcmy_gplist_is_empty(list2)) {
        return BCMY_E_NONE;
    }

    total = GP_COUNT(list1) + GP_COUNT(list2);
    if (list1->gp_alloc <= total) {
        BCM_IF_ERROR_RETURN(_bcmy_gplist_realloc(list1,
                total + BCMY_GPLIST_MIN_INCREMENT));
    }

    sal_memcpy(&(GP_FIRST_FREE(list1)), list2->gp_ports,
               GP_ELSIZE(GP_COUNT(list2)));

    list1->gp_last += GP_COUNT(list2);

    return BCMY_E_NONE;
}

/*
 * Function:
 *      bcmy_gplist_copy
 * Purpose:
 *      Copy src to dest, overwriting dest
 * Parameters:
 *      dest    - Destination list
 *      src     - Source list
 * Returns:
 *      BCMY_E_XXX
 * Notes:
 */

int
bcmy_gplist_copy(bcmy_gplist_t *dest, bcmy_gplist_t *src)
{
    if (bcmy_gplist_is_null(dest)) {
        BCM_IF_ERROR_RETURN(bcmy_gplist_init(dest, -1, 0));
    }

    if (bcmy_gplist_is_empty(src)) {
        return bcmy_gplist_clear(dest);
    }

    dest->gp_last = 0;
    if (dest->gp_alloc < src->gp_alloc) {
        BCM_IF_ERROR_RETURN(_bcmy_gplist_realloc(dest, src->gp_alloc));
    }

    sal_memcpy(dest->gp_ports, src->gp_ports, GP_ELSIZE(GP_COUNT(src)));
    dest->gp_last = src->gp_last;

    return BCMY_E_NONE;
}



/* Current absolute max for number of logical ports in a list */
#ifndef BCMY_GPLIST_ABSOLUTE_MAX_LEN
#define BCMY_GPLIST_ABSOLUTE_MAX_LEN 3000  
#endif

/*
 * Function:
 *      bcmy_gplist_check
 * Purpose:
 *      Debug function.  Check the consistency of a list
 * Parameters:
 *      list   - List to examine
 * Returns:
 *      0 if okay, < 0 if not consistent
 */

int
bcmy_gplist_check(bcmy_gplist_t *list)
{
    if (!list) {
        return -1;
    }

    if (!list->gp_ports) {
        return -2;
    }

    if (list->gp_alloc < 0) {
        return -3;
    }

    if (list->gp_alloc > BCMY_GPLIST_ABSOLUTE_MAX_LEN) {
        return -4;
    }

    if (list->gp_last >= list->gp_alloc) {
        return -5;
    }

    return 0;
}

/*
 * Function:
 *      bcmy_gplist_range
 * Purpose:
 *      Add a range of ports to a port list
 * Parameters:
 *      list     - list to add ports to
 *      start    - Low port to add
 *      end      - Last port to add
 * Returns:
 *      BCMY_E_XXX
 */

int
bcmy_gplist_range(bcmy_gplist_t *list, bcm_gport_t start, bcm_gport_t end)
{
    bcm_gport_t curport = start;
    int rv;

    while ((curport != end) && (curport != BCM_GPORT_INVALID)) {
        if ((rv = bcmy_gplist_add(list, curport)) < 0) {
            return rv;
        }
        curport += 1;
    }
    if (curport == end) {
        if ((rv = bcmy_gplist_add(list, curport)) < 0) {
            return rv;
        }
    }

    return BCMY_E_NONE;
}

/* Functional version of lplist macros */


/*
 * Function:
 *      bcmy_gplist_is_null
 * Purpose:
 *      
 * Parameters:
 *      list
 *
 * Returns:
 *      
 */

int
bcmy_gplist_is_null(bcmy_gplist_t *list)
{
    return BCMY_GPLIST_IS_NULL(list);
}


/*
 * Function:
 *      bcmy_gplist_is_empty
 * Purpose:
 *      
 * Parameters:
 *      list
 *
 * Returns:
 *      
 */

int
bcmy_gplist_is_empty(bcmy_gplist_t *list)
{
    return BCMY_GPLIST_IS_EMPTY(list);
}


/*
 * Function:
 *      bcmy_gplist_count
 * Purpose:
 *      
 * Parameters:
 *      list
 *
 * Returns:
 *      
 */

int
bcmy_gplist_count(bcmy_gplist_t *list)
{
    return BCMY_GPLIST_COUNT(list);
}


/*
 * Function:
 *      bcmy_gplist_remove
 * Purpose:
 *      
 * Parameters:
 *      list
 *      gport
 *
 * Returns:
 *      
 */

void
bcmy_gplist_remove(bcmy_gplist_t *list, int gport)
{
    if (!bcmy_gplist_is_empty(list)) {
        BCMY_GPLIST_REMOVE(list, gport);
    }
}


/*
 * Function:
 *      bcmy_gplist_idx_remove
 * Purpose:
 *      
 * Parameters:
 *      list
 *      idx
 *
 * Returns:
 *      
 */

void
bcmy_gplist_idx_remove(bcmy_gplist_t *list, int idx)
{
    if (!bcmy_gplist_is_empty(list)) {
        BCMY_GPLIST_IDX_REMOVE(list, idx);
    }
}


/*
 * Function:
 *      bcmy_gplist_pbmp_add
 * Purpose:
 *      
 * Parameters:
 *      list
 *      unit
 *      pbm
 *
 * Returns:
 *      
 */

int
bcmy_gplist_pbmp_add(bcmy_gplist_t *list, int unit, bcm_pbmp_t *pbm)
{
    int rv = BCMY_E_PARAM;

    if (list) {
        rv = _bcmy_gplist_pbmp_add(list, unit, *pbm);
    }

    return rv;
}


/*
 * Function:
 *      bcmy_gplist_to_pbmp
 * Purpose:
 *      
 * Parameters:
 *      list
 *      unit
 *      pbm
 *
 * Returns:
 *      void
 */

void
bcmy_gplist_to_pbmp(bcmy_gplist_t *list, int unit, bcm_pbmp_t *pbm)
{
    bcm_pbmp_t p;

    BCM_PBMP_CLEAR(p);
    if (!bcmy_gplist_is_empty(list)) {
        BCM_GPLISTPTR_TO_PBMP(list, unit, p);
    }
    *pbm = p;
}


/*
 * Function:
 *      _bcmy_gplist_lport_compare
 * Purpose:
 *      gport sort compare function
 * Parameters:
 *      a - pointer to 1st element
 *      b - pointer to 2nd element
 * Returns:
 *      -1 if *a < *b
 *       0 if *a = *b
 *      +1 if *a > *b
 */


STATIC int
_bcmy_gplist_lport_compare(void *a, void *b)
{
    bcm_gport_t        ap, bp;
    int rv;

    ap = *(bcm_gport_t *)a;
    bp = *(bcm_gport_t *)b;

    if (ap < bp) {
	rv = -1;
    } else if (ap > bp) {
	rv = 1;
    } else {
        rv = 0;
    }

    return rv;
}


/*
 * Function:
 *      bcmy_gplist_sort
 * Purpose:
 *      Sort lplist in ascending order
 * Parameters:
 *      list   - List to sort
 * Returns:
 *      BCMY_E_NONE  - list sorted
 */

int
bcmy_gplist_sort(bcmy_gplist_t *list)
{
    if (!bcmy_gplist_is_empty(list)) {
        _shr_sort(list->gp_ports, GP_COUNT(list),
                  sizeof(bcm_gport_t), _bcmy_gplist_lport_compare);
    }

    return BCMY_E_NONE;
}


/*
 * Function:
 *      bcmy_gplist_uniq
 * Purpose:
 *      Delete duplicate elements in list
 * Parameters:
 *      list   - List to uniqueify
 * Returns:
 *      BCMY_E_NONE
 */

int
bcmy_gplist_uniq(bcmy_gplist_t *list)
{
    int idx, dup;
    bcm_gport_t gport;

    if (!bcmy_gplist_is_empty(list)) {
        for ( idx = 0; idx < list->gp_last; idx++ ) {
            gport = list->gp_ports[idx];
            while ((dup = bcmy_gplist_index_get_from(list,
                                                     idx+1, gport)) >= 0) {
                bcmy_gplist_idx_remove(list, dup);
            }
        }
    }

    return BCMY_E_NONE;
}

