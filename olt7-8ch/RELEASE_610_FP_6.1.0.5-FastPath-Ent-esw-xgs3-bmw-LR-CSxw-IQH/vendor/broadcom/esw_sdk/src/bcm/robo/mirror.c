/*
 * $Id: mirror.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * Mirror - Broadcom RoboSwitch Mirror API.
 */

#include <soc/drv.h>
#include <soc/debug.h>

#include <bcm/error.h>
#include <bcm/mirror.h>
#include <bcm/port.h>
#include <bcm/stack.h>

typedef struct {
    int init;           /* TRUE if Mirror module has been inited */
    int mode;       /* mirroring mode */
    int port;       /* port to set */
    int modid;          /* module id of mirror-to port */
} mirror_info_t;

static mirror_info_t    _bcm_robo_mirror_info[BCM_MAX_NUM_UNITS];

/* The mirror control for this unit */
#define MIRROR_INFO(unit)   _bcm_robo_mirror_info[unit]

/* return BCM_E_INIT if mirroring is not initialized */
#define MIRROR_INIT(unit) \
    if (!MIRROR_INFO(unit).init) {return BCM_E_INIT;}

/*
 * Function:
 *  bcm_robo_mirror_init
 * Purpose:
 *  Initialize mirror software system.
 * Parameters:
 *  unit - RoboSwitch unit #.
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_robo_mirror_init(int unit)
{
    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_init()..\n");
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    MIRROR_INFO(unit).init = TRUE;
    MIRROR_INFO(unit).mode = -1;
    MIRROR_INFO(unit).port = -1;
    return bcm_mirror_mode_set(unit, BCM_MIRROR_DISABLE);
}

/*
 * Function:
 *  bcm_robo_mirror_mode_set
 * Description:
 *  Enable or disable mirroring.  Will wait for bcm_mirror_to_set to be
 *  called to actually do the enable if needed.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *  mode - One of BCM_MIRROR_(DISABLE|L2|L2_L3)
 * Returns:
 *  BCM_E_XXX
 */

int bcm_robo_mirror_mode_set(int unit, int mode)
{
    uint32      menable, t_mode;
    int         mport, t_mport;
    pbmp_t  igr_pbmp, egr_pbmp;

    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_mode_set()..\n");
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
        
    /* Get the current Mirror setting */    
    BCM_IF_ERROR_RETURN((
            DRV_SERVICES(unit)->mirror_get)
                            (unit, &menable, (uint32 *) &mport, 
                            &igr_pbmp, &egr_pbmp));

    /* If mode is Mirror-L2, enable both Mirror-ingress and Mirror-egress, */
    /* else set Mirror-disable */
    if (mode == BCM_MIRROR_L2) {
        t_mode = TRUE;
    } else if (mode == BCM_MIRROR_L2_L3){
        soc_cm_debug(DK_ERR, "No L3 mirror mode for Robo!\n");
        t_mode = TRUE;
        return BCM_E_UNAVAIL;
    } else if (mode == BCM_MIRROR_DISABLE) {
        t_mode = FALSE;
    } else {
        return BCM_E_PARAM;
    }

    t_mport = MIRROR_INFO(unit).port;

    if ((t_mode == menable) && (t_mport == mport)) {
        MIRROR_INFO(unit).mode = mode;
        return BCM_E_NONE;
    }
    
    if (t_mode == FALSE) {
        t_mport = 0;            /* clear port when disabling */
        BCM_PBMP_CLEAR(igr_pbmp);
        BCM_PBMP_CLEAR(egr_pbmp);
    }

    if (t_mport < 0) {          /* wait for mirror_to_set() */
        MIRROR_INFO(unit).mode = mode;
        return BCM_E_NONE;
    }

    /* Set the current Mirror setting */    
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mirror_set)
                            (unit, t_mode, t_mport, 
                            igr_pbmp, egr_pbmp));
    MIRROR_INFO(unit).mode = mode;

    return BCM_E_NONE;

}   

/*
 * Function:
 *  bcm_robo_mirror_mode_get
 * Description:
 *  Get mirror mode.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *  mode - (OUT) One of BCM_MIRROR_(DISABLE|L2|L2_L3)
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_robo_mirror_mode_get(int unit, int *mode)
{
    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_mode_get()..\n");
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (MIRROR_INFO(unit).mode < 0) {
        *mode = BCM_MIRROR_DISABLE;
    } else {
        *mode = MIRROR_INFO(unit).mode;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_mirror_to_set
 * Description:
 *  Set the mirror-to port for all mirroring, enabling mirroring
 *  if a mode has previously been set.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *  port - The port to mirror all ingress/egress selections to
 * Returns:
 *  BCM_E_XXX
 */
int 
bcm_robo_mirror_to_set(int unit, bcm_port_t port)
{
    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_to_set()..\n");
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    MIRROR_INFO(unit).port = port;
    return bcm_mirror_mode_set(unit, MIRROR_INFO(unit).mode);
}

/*
 * Function:
 *  bcm_robo_mirror_to_get
 * Description:
 *  Get the mirror-to port for all mirroring
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *  port - (OUT) The port to mirror all ingress/egress selections to
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_robo_mirror_to_get(int unit, bcm_port_t *port)
{
    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_to_get()..\n");
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    *port = MIRROR_INFO(unit).port;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_robo_mirror_ingress_set
 * Description:
 *     Enable or disable mirroring per ingress
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *  port - The port to affect
 *  val - Boolean value for on/off
 * Returns:
 *     BCM_E_xxxx
 * Notes:
 *     Mirroring must also be globally enabled.
 */
int 
bcm_robo_mirror_ingress_set(int unit, bcm_port_t port, int val)
{
    uint32  menable, mport;
    pbmp_t  t_pbmp;
    soc_pbmp_t igr_pbmp, egr_pbmp;

    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_ingress_set()..\n");
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }
    MIRROR_INIT(unit);

    /* Get the current Mirror setting */    
    BCM_IF_ERROR_RETURN((
            DRV_SERVICES(unit)->mirror_get)
                            (unit, &menable, &mport, 
                            &igr_pbmp, &egr_pbmp));
    BCM_PBMP_CLEAR(t_pbmp);
    BCM_PBMP_ASSIGN(t_pbmp, igr_pbmp);

    /* BCM5345 allowed one Ingress Mirrored port only */
    if (val) {
        if (BCM_PBMP_MEMBER(t_pbmp, port)){
            return BCM_E_NONE;
        }
        BCM_PBMP_PORT_ADD(t_pbmp, port);
    } else {
        if (!BCM_PBMP_MEMBER(t_pbmp, port)){
            return BCM_E_NONE;
        }
        BCM_PBMP_PORT_REMOVE(t_pbmp, port);
    }
    
    BCM_PBMP_ASSIGN(igr_pbmp, t_pbmp);

    /* Set the current Mirror setting */    
    BCM_IF_ERROR_RETURN((
            DRV_SERVICES(unit)->mirror_set)
                            (unit, menable, mport, 
                            igr_pbmp, egr_pbmp));
    
    return BCM_E_NONE;
}   

/*
 * Function:
 *     bcm_robo_mirror_ingress_get
 * Description:
 *     Get the mirroring per ingress enabled/disabled status
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *  port - The port to check
 *  val - Place to store boolean return value for on/off
 * Returns:
 *     BCM_E_xxxx
 */
int 
bcm_robo_mirror_ingress_get(int unit, bcm_port_t port, int *val)
{
    uint32  menable, mport;
    soc_pbmp_t igr_pbmp, egr_pbmp;

    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_ingress_get()..\n");
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }
    MIRROR_INIT(unit);

     /* Get the current Mirror setting */    
    BCM_IF_ERROR_RETURN((
            DRV_SERVICES(unit)->mirror_get)
                            (unit, &menable, &mport, 
                            &igr_pbmp, &egr_pbmp));
    
    *val = (BCM_PBMP_MEMBER(igr_pbmp, port)) ? 1 : 0;
    
    return BCM_E_NONE;    
}   

/*
 * Function:
 *     bcm_robo_mirror_egress_set
 * Description:
 *     Enable or disable mirroring per egress
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *  port - The port to affect
 *  val - Boolean value for on/off
 * Returns:
 *     BCM_E_xxx
 * Notes:
 *     Mirroring must also be globally enabled.
 */
int 
bcm_robo_mirror_egress_set(int unit, bcm_port_t port, int val)
{
    uint32  menable, mport;
    pbmp_t  t_pbmp;
    soc_pbmp_t igr_pbmp, egr_pbmp;

    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_egress_set()..\n");
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }
    MIRROR_INIT(unit);

     /* Get the current Mirror setting */    
    BCM_IF_ERROR_RETURN((
            DRV_SERVICES(unit)->mirror_get)
                            (unit, &menable, &mport, 
                            &igr_pbmp, &egr_pbmp));
    BCM_PBMP_CLEAR(t_pbmp);
    BCM_PBMP_ASSIGN(t_pbmp, egr_pbmp);

    if (val) {
        if (BCM_PBMP_MEMBER(t_pbmp, port)){
            return BCM_E_NONE;
        }
        BCM_PBMP_PORT_ADD(t_pbmp, port);
    } else {
        if (!BCM_PBMP_MEMBER(t_pbmp, port)){
            return BCM_E_NONE;
        }
        BCM_PBMP_PORT_REMOVE(t_pbmp, port);
    }
    
    BCM_PBMP_ASSIGN(egr_pbmp, t_pbmp);

    /* Set the current Mirror setting */    
    BCM_IF_ERROR_RETURN((
            DRV_SERVICES(unit)->mirror_set)
                            (unit, menable, mport, 
                            igr_pbmp, egr_pbmp));
    
    return BCM_E_NONE;
}   

/*
 * Function:
 *     bcm_robo_mirror_egress_get
 * Description:
 *     Get the mirroring per egress enabled/disabled status
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *  port - The port to check
 *  val - Place to store boolean return value for on/off
 * Returns:
 *     BCM_E_xxxx
 */
int 
bcm_robo_mirror_egress_get(int unit, bcm_port_t port, int *val)
{
    uint32  menable, mport;
    soc_pbmp_t igr_pbmp, egr_pbmp;

    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_egress_get()..\n");
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }
    MIRROR_INIT(unit);
     /* Get the current Mirror setting */    
    BCM_IF_ERROR_RETURN((
            DRV_SERVICES(unit)->mirror_get)
                            (unit, &menable, &mport, 
                            &igr_pbmp, &egr_pbmp));

    *val = (BCM_PBMP_MEMBER(egr_pbmp, port)) ? 1 : 0;
    
    return BCM_E_NONE;    
}
    
/*
 * Function:
 *  bcm_robo_mirror_pfmt_set
 * Description:
 *  Set the mirroring preserve format field
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *  val - value for preserve format on/off
 * Returns:
 *  BCM_E_XXX
 */
int 
bcm_robo_mirror_pfmt_set(int unit, int val)
{
    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_pfmt_set()..unavailable\n");
    return BCM_E_UNAVAIL;
}   

/*
 * Function:
 *  bcm_robo_mirror_pfmt_get
 * Description:
 *  Set the mirroring preserve format field
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *  val - (OUT) Value for preserve format on/off
 * Returns:
 *  BCM_E_XXX
 */
int 
bcm_robo_mirror_pfmt_get(int unit, int *val)
{
    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_pfmt_get()..unavailable\n");
    return BCM_E_UNAVAIL;
}   

/*
 * Function:
 *  bcm_robo_mirror_to_pbmp_set
 * Description:
 *  Set the mirror-to port bitmap for mirroring on a given port.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *  port - The port to affect
 *      pbmp - The port bitmap of mirrored to ports for this port.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  This API interface is only supported on XGS fabric chips.
 */
int 
bcm_robo_mirror_to_pbmp_set(int unit, bcm_port_t port, bcm_pbmp_t pbmp)
{
    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_to_pbmp_set()..unavailable\n");
    return BCM_E_UNAVAIL;
}   

/*
 * Function:
 *  bcm_robo_mirror_to_pbmp_get
 * Description:
 *  Get the mirror-to port bitmap for mirroring on the
 *  specified port.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number
 *  port - The port to mirror all ingress/egress selections to
 *      pbmp - (OUT) The port bitmap of mirror-to ports for this port.
 * Returns:
 *  BCM_E_XXX
 */
int 
bcm_robo_mirror_to_pbmp_get(int unit, bcm_port_t port, bcm_pbmp_t *pbmp)
{
    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_to_pbmp_get()..unavailable\n");
    return BCM_E_UNAVAIL;
}   


/*
 * Function:
 *      bcm_mirror_port_set
 * Description:
 *      Set mirroring configuration for a port
 * Parameters:
 *      unit      - BCM device number
 *      port      - port to configure
 *      dest_mod  - module id of mirror-to port
 *                  (-1 for local port)
 *      dest_port - mirror-to port
 *      flags     - BCM_MIRROR_PORT_* flags
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Setting BCM_MIRROR_PORT_ENABLE without setting _INGRESS or
 *      _EGRESS allows the port to participate in bcm_l2_cache matches
 *      with the BCM_L2_CACHE_MIRROR bit set, and to participate in
 *      bcm_field lookups with the mirror action set.
 *
 *      If bcmSwitchDirectedMirroring is disabled for the unit and
 *      dest_mod is non-negative, then the dest_mod path is looked
 *      up using bcm_topo_port_get.
 *      If bcmSwitchDirectedMirroring is enabled for the unit and
 *      dest_mod is negative, then the local unit's modid is used
 *      as the dest_mod.
 */
int
bcm_robo_mirror_port_set(int unit, bcm_port_t port,
                    bcm_module_t dest_mod, bcm_port_t dest_port,
                    uint32 flags)
{
    int rv = BCM_E_NONE;

    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_port_set()..\n");

    /* standalone */
    /* dest_mod = -1 for local port */
    if (dest_mod == -1) {
        MIRROR_INFO(unit).modid = 0;
    } else {
        MIRROR_INFO(unit).modid = dest_mod;
    }

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    if (IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    }
    /* standalone, check if module id of mirror-to port = 0*/
    if (MIRROR_INFO(unit).modid) {
        return BCM_E_PARAM;
    }
    MIRROR_INIT(unit);
    
    /* Disable mirroring? */
    if ((flags & (BCM_MIRROR_PORT_INGRESS | 
                  BCM_MIRROR_PORT_EGRESS | 
                  BCM_MIRROR_PORT_ENABLE)) == 0) {
        BCM_IF_ERROR_RETURN
                 (bcm_mirror_ingress_set(unit, port, 0));
        BCM_IF_ERROR_RETURN
                 (bcm_mirror_egress_set(unit, port, 0));
        return BCM_E_NONE;
    }

    if (flags & BCM_MIRROR_PORT_DEST_TRUNK) {
        return BCM_E_PARAM;
    }
    if (!SOC_PORT_VALID(unit, dest_port)) {
        return BCM_E_PORT;
    }
    
    BCM_IF_ERROR_RETURN
             (bcm_mirror_to_set(unit, dest_port));

    BCM_IF_ERROR_RETURN
             (bcm_mirror_ingress_set(unit, port, 
                         flags & BCM_MIRROR_PORT_INGRESS));

    BCM_IF_ERROR_RETURN
            (bcm_mirror_egress_set(unit, port, 
                        flags & BCM_MIRROR_PORT_EGRESS));
    
    return rv;
    
}

/*
 * Function:
 *      bcm_mirror_port_get
 * Description:
 *      Get mirroring configuration for a port
 * Parameters:
 *      unit      - BCM device number
 *      port      - port to get configuration for
 *      dest_mod  - (OUT) module id of mirror-to port
 *      dest_port - (OUT) mirror-to port
 *      flags     - (OUT) BCM_MIRROR_PORT_* flags
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_mirror_port_get(int unit, bcm_port_t port,
                    bcm_module_t *dest_mod, bcm_port_t *dest_port,
                    uint32 *flags)
{
    int enable;

    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_mirror_port_get()..\n");

    if (!SOC_UNIT_VALID(unit)) {
    return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) {
    return BCM_E_PORT;
    }
    if (IS_CPU_PORT(unit, port)) {
    return BCM_E_PORT;
    }
    MIRROR_INIT(unit);

    *flags = 0;
 
    if (bcm_mirror_ingress_get(unit, port, &enable) == BCM_E_NONE &&
        enable) {
        *flags |= BCM_MIRROR_PORT_INGRESS;
    }
    if (bcm_mirror_egress_get(unit, port, &enable) == BCM_E_NONE &&
        enable) {
        *flags |= BCM_MIRROR_PORT_EGRESS;
    } 

    BCM_IF_ERROR_RETURN
          (bcm_mirror_to_get(unit, dest_port));
    /* standalone */
    *dest_mod = MIRROR_INFO(unit).modid;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mirror_vlan_set
 * Description:
 *      Set VLAN for egressing mirrored packets on a port (RSPAN)
 * Parameters:
 *      unit    - device number
 *      port    - mirror-to port to set (-1 for all ports)
 *      tpid    - tag protocol id (0 to disable)
 *      vlan    - virtual lan number (0 to disable)
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_mirror_vlan_set(int unit, bcm_port_t port,
                    uint16 tpid, uint16 vlan)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_mirror_vlan_get
 * Description:
 *      Get VLAN for egressing mirrored packets on a port (RSPAN)
 * Parameters:
 *      unit    - device number
 *      port    - mirror-to port for which to get tag info
 *      tpid    - (OUT) tag protocol id
 *      vlan    - (OUT) virtual lan number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_mirror_vlan_get(int unit, bcm_port_t port,
                    uint16 *tpid, uint16 *vlan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_mirror_egress_path_set(int unit, bcm_module_t dest_mod,
                    bcm_port_t port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_mirror_egress_path_get(int unit, bcm_module_t dest_mod,
                    bcm_port_t *port)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcm_robo_mirror_port_dest_add 
 * Purpose:
 *      Add mirroring destination to a port. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      port         -  (IN) Port mirrored port.
 *      flags        -  (IN) BCM_MIRROR_PORT_XXX flags.
 *      mirror_dest  -  (IN) Mirroring destination gport. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_robo_mirror_port_dest_add(int unit, bcm_port_t port, 
                              uint32 flags, bcm_gport_t mirror_dest) 
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *     bcm_robo_mirror_port_dest_delete
 * Purpose:
 *      Remove mirroring destination from a port. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      port         -  (IN) Port mirrored port.
 *      flags        -  (IN) BCM_MIRROR_PORT_XXX flags.
 *      mirror_dest  -  (IN) Mirroring destination gport. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_robo_mirror_port_dest_delete(int unit, bcm_port_t port, 
                                uint32 flags, bcm_gport_t mirror_dest) 
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *     bcm_robo_mirror_port_dest_delete_all
 * Purpose:
 *      Remove all mirroring destinations from a port. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      port         -  (IN) Port mirrored port.
 *      flags        -  (IN) BCM_MIRROR_PORT_XXX flags.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_robo_mirror_port_dest_delete_all(int unit, bcm_port_t port, uint32 flags) 
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *     bcm_robo_mirror_port_dest_get
 * Purpose:
 *     Get port mirroring destinations.   
 * Parameters:
 *     unit             - (IN) BCM device number. 
 *     port             - (IN) Port mirrored port.
 *     flags            - (IN) BCM_MIRROR_PORT_XXX flags.
 *     mirror_dest_size - (IN) Preallocated mirror_dest array size.
 *     mirror_dest      - (OUT)Filled array of port mirroring destinations
 *     mirror_dest_count - (OUT)Actual number of mirroring destinations filled.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_robo_mirror_port_dest_get(int unit, bcm_port_t port, uint32 flags, 
                              int mirror_dest_size, bcm_gport_t *mirror_dest,
                              int *mirror_dest_count)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *     bcm_robo_mirror_destination_create
 * Purpose:
 *     Create mirror destination description.
 * Parameters:
 *      unit         - (IN) BCM device number. 
 *      mirror_dest  - (IN) Mirror destination description.
 * Returns:
 *      BCM_X_XXX
 */
int 
bcm_robo_mirror_destination_create(int unit, bcm_mirror_destination_t *mirror_dest) 
{
    return (BCM_E_UNAVAIL);
}


/*
 * Function:
 *     bcm_robo_mirror_destination_destroy
 * Purpose:
 *     Destroy mirror destination description.
 * Parameters:
 *      unit            - (IN) BCM device number. 
 *      mirror_dest_id  - (IN) Mirror destination id.
 * Returns:
 *      BCM_X_XXX
 */
int 
bcm_robo_mirror_destination_destroy(int unit, bcm_gport_t mirror_dest_id) 
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *     bcm_robo_mirror_destination_get
 * Purpose:
 *     Get mirror destination description.
 * Parameters:
 *      unit            - (IN) BCM device number. 
 *      mirror_dest_id  - (IN) Mirror destination id.
 *      mirror_dest     - (OUT)Mirror destination description.
 * Returns:
 *      BCM_X_XXX
 */
int 
bcm_robo_mirror_destination_get(int unit, bcm_gport_t mirror_dest_id, 
                                   bcm_mirror_destination_t *mirror_dest)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *     bcm_robo_mirror_destination_traverse
 * Purpose:
 *     Traverse installed mirror destinations
 * Parameters:
 *      unit      - (IN) BCM device number. 
 *      cb        - (IN) Mirror destination traverse callback.         
 *      user_data - (IN) User cookie
 * Returns:
 *      BCM_X_XXX
 */
int 
bcm_robo_mirror_destination_traverse(int unit, bcm_mirror_destination_traverse_cb cb, 
                                    void *user_data) 
{
    return (BCM_E_UNAVAIL);
}
