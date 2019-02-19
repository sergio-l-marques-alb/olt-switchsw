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
 * File:        port.c
 * Purpose:     XGS5 Port common driver.
 *
 * Notes:
 *      More than likely only new Port APIs are found in
 *      this function driver due to a huge amount of legacy code
 *      that were implemented before this model.
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trunk.h>
#include <bcm/error.h>

/* Checks for 'null' argument */
#define PARAM_NULL_CHECK(_arg)  \
    if ((_arg) == NULL) {       \
        return BCM_E_PARAM;     \
    }

/* Hardware definitions */
static bcmi_xgs5_port_hw_defs_t  *bcmi_xgs5_port_hw_defs[BCM_MAX_NUM_UNITS] = {0};

#define PORT_HW(u_)    (bcmi_xgs5_port_hw_defs[(u_)])

#define PORT_REDIRECTION_HW_DEFS_EGR_PORT(u_)  \
    (bcmi_xgs5_port_hw_defs[(u_)]->egr_port)


#define BCMI_MAX_STRENGTH_VALUE (0x3E)
/*
 * Function:
 *      bcmi_xgs5_port_addressable_local_get
 * Description:
 *      Validate that given port/gport parameter is an addressable
 *      local logical port and return local logical port in
 *      BCM port format (non-Gport).
 *      Logical port does not need to be defined (does not
 *      need to be a VALID port, only addressable).
 * Parameters:
 *      unit       - (IN) BCM device number
 *      port       - (IN) Port / Gport to validate
 *      local_port - (OUT) Port number if valid.
 * Return Value:
 *      BCM_E_NONE - Port OK
 *      BCM_E_INIT - Not initialized
 *      BCM_E_PORT - Port Invalid
 *
 * NOTE:
 *      This function only accepts those GPORT types that
 *      deal with logical BCM ports (i.e. MODPORT, LOCAL, etc.).
 *
 *      Also, unlike bcm_esw_port_local_get(), this routine only checks
 *      that the port number is within the valid port range.
 *      It does NOT check whether the port
 *      is a BCM valid port (i.e. a defined/configured port).
 *      This allows functions that requires port validation with
 *      a new logical port that has not been declared/configured
 *      in the system.
 */
int
bcmi_xgs5_port_addressable_local_get(int unit,
                                     bcm_port_t port, bcm_port_t *local_port)
{
    bcm_module_t mod;
    bcm_trunk_t trunk_id;
    int id;
    int is_local;

    /*
     * A logical port can be:
     *   - Defined and active.
     *   - Defined and inactive (SOC property portmap with ':i' option).
     *   - Not defined.
     *
     * The following port definitions/words (i.e. functions, macro names)
     * are used in the SDK as follows:
     *   - VALID       :  The logical port is defined, it may be active or
     *                    inactive.
     *                    There IS a logical-to-physical port mapping.
     *
     *   - ADDRESSABLE :  The logical port number is within the valid
     *                    range.
     *                    The port may or may have not been defined
     *                    (i.e. there is NO logical-to-physical port mapping.)
     */
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, port, &mod, local_port,
                                    &trunk_id, &id));
        if ((trunk_id != -1) || (id != -1)) {
            return BCM_E_PORT;
        }

        /* Check that port is local */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod, &is_local));

        if (!is_local) {
            return BCM_E_PORT;
        }
    } else {
        *local_port = port;
    }

    /* Check that port is within valid logical port range */
    if (!SOC_PORT_ADDRESSABLE(unit, *local_port)) {
        return BCM_E_PORT;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs5_port_fn_drv_init
 * Purpose:
 *      Initialize the Port function driver.
 * Parameters:
 *      unit - (IN) Unit number.
 *      drv  - (IN) Pointer to the Port function driver.
 *      hw_defs - (IN) Pointer to the h/w definition struct.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      This routine only intializes the function driver.
 *      The main port routine is still in the esw/port.c file.
 */
int
bcmi_xgs5_port_fn_drv_init(int unit, bcm_esw_port_drv_t *drv,
                           bcmi_xgs5_port_hw_defs_t *hw_defs)
{

    /* Assign the driver */
    BCM_ESW_PORT_DRV(unit) = drv;

    /* Assign the hardware definitions */
    if(hw_defs) {
        PORT_HW(unit) = hw_defs;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_resource_set
 * Purpose:
 *      Modify the following port resources:
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 *      - Encapsulation mode
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      resource - (IN) Port resource configuration.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 */
int
bcmi_xgs5_port_resource_set(int unit,
                            bcm_gport_t port, bcm_port_resource_t *resource)
{
    bcm_port_t  lport1, lport2;

    PARAM_NULL_CHECK(resource);

    /* Check that given port matches logical port field in structure */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_addressable_local_get(unit,
                                              port, &lport1));
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_addressable_local_get(unit,
                                              resource->port, &lport2));
    if (lport1 != lport2) {
        return BCM_E_PARAM;
    }

    return BCM_ESW_PORT_DRV(unit)->resource_multi_set(unit, 1, resource);
}


/*
 * Function:
 *      bcmi_xgs5_port_resource_get
 * Purpose:
 *      Get the port resource configuration for the specified logical port.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Logical port.
 *      resource  - (OUT) Returns port resource information.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 */
int
bcmi_xgs5_port_resource_get(int unit,
                            bcm_gport_t port, bcm_port_resource_t *resource)
{
    bcm_port_t lport;
    soc_info_t *si = &SOC_INFO(unit);
    int encap = 0;

    PARAM_NULL_CHECK(resource);

    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_addressable_local_get(unit,
                                              port, &lport));
    bcm_port_resource_t_init(resource);
    resource->flags = 0x0;
    resource->port = lport;
    resource->physical_port = si->port_l2p_mapping[lport];
    resource->lanes = si->port_num_lanes[lport];
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_speed_get(unit, lport, &resource->speed));
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_encap_get(unit, lport, &encap));
    resource->encap = encap;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs5_port_resource_multi_set
 * Purpose:
 *      Modify the following port resources:
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 *      - Encapsulation mode
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 */
int
bcmi_xgs5_port_resource_multi_set(int unit,
                                  int nport, bcm_port_resource_t *resource)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmi_xgs5_port_resource_traverse
 * Purpose:
 *      Iterates over the port resource configurations on a given
 *      unit and calls user-provided callback for every entry.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      trav_fn    - (IN) Callback function to execute.
 *      user_data  - (IN) Pointer to any user data.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_port_resource_traverse(int unit,
                                 bcm_port_resource_traverse_cb trav_fn,
                                 void *user_data)
{
    bcm_port_t port;
    bcm_port_resource_t resource;
    soc_info_t *si = &SOC_INFO(unit);

    PARAM_NULL_CHECK(trav_fn);

    for (port = 0; port < SOC_MAX_NUM_PORTS ; port++) {
        /* Skip logical ports with no assignment and management ports */
        if ((si->port_l2p_mapping[port] == -1) ||
            IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
                continue;
        }

        /* Get port information */
        if (BCM_FAILURE(bcmi_xgs5_port_resource_get(unit, port, &resource))) {
            continue;
        }

        /* Call user-provided callback routine */
        BCM_IF_ERROR_RETURN(trav_fn(unit, &resource, user_data));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_redirect_config_set
 * Purpose:
 *      Configure the redirection data
 * Parameters:
 *      unit             - (IN) device id.
 *      port             - (IN) Port number.
 *      redirect_config  - (IN) Pointer to the redirection config structure
 * Returns:
 *      None
 */
int _bcmi_xgs5_port_redirect_config_set(int unit, bcm_port_t port,
                          bcm_port_redirect_config_t *redirect_config)
{
    bcm_trunk_t trunk_id = -1;
    bcm_module_t mod_id = -1;
    bcm_port_t port_id = -1;
    int id = -1, redirect_set = 1, color = -1;
    int curr_dest_type = 0;
    int dest_type = _BCM_REDIRECT_DEST_TYPE_NONE, dest_value = -1;
    int dest_type_flags = 0;
    egr_port_entry_t egr_port_entry;
#if defined(BCM_EP_REDIRECT_VERSION_2)
    int redirect_action_value = 0, redirect_source_value = 0;
    int buffer_priority = 0;
#endif

    /* If the dest_type_field is invalid, this feature is not
       supported on this device */
    if(PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_type == INVALIDf) {
        return BCM_E_UNAVAIL;
    }

    dest_type_flags = (redirect_config->flags &
                           (BCM_PORT_REDIRECT_DESTINATION_PORT |
                            BCM_PORT_REDIRECT_DESTINATION_TRUNK |
                            BCM_PORT_REDIRECT_DESTINATION_MCAST));

    /* Make sure only one destination type is set */
    if(dest_type_flags & (dest_type_flags - 1)) {
        return BCM_E_PARAM;
    }

    /* Switch based on the destination type */
    switch(dest_type_flags) {

        case BCM_PORT_REDIRECT_DESTINATION_PORT:

            /* Make sure that the supplied destination is valid */
            if(!BCM_GPORT_IS_MODPORT(redirect_config->destination)) {
                return BCM_E_PORT;
            }

            BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                                         redirect_config->destination,
                                         &mod_id, &port_id, &trunk_id, &id));

            /* Set the destination type as GPP(Port) */
            dest_type = _BCM_REDIRECT_DEST_TYPE_PORT;

            /* Set the (Mod,Port) value as destination */
            BCMI_MOD_PORT_SET(dest_value, mod_id, port_id);

            break;

        case BCM_PORT_REDIRECT_DESTINATION_TRUNK:

            /* Make sure that the supplied destination is valid */
            if(!BCM_GPORT_IS_SET(redirect_config->destination) ||
               !BCM_GPORT_IS_TRUNK(redirect_config->destination)) {
                return BCM_E_PARAM;
            }

            /* Get the trunk id */
            trunk_id = BCM_GPORT_TRUNK_GET(redirect_config->destination);

            /* Check that the trunk id is valid */
            BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, trunk_id));

            /* Set the destination type as GPP(Port) */
            dest_type = _BCM_REDIRECT_DEST_TYPE_TRUNK;

            /* Set the trunk id as the destiantion */
            dest_value = trunk_id;

            break;

        case BCM_PORT_REDIRECT_DESTINATION_MCAST:

            /* Check the destination and make suure it is a valid l2 MC */
            if (_BCM_MULTICAST_IS_SET(redirect_config->destination)) {
                if (!_BCM_MULTICAST_IS_L2(redirect_config->destination)) {
                    return (BCM_E_PARAM);
                }
            }

            /* Set the destination type as GPP(Port) */
            dest_type = _BCM_REDIRECT_DEST_TYPE_MCAST;

            /* Set the l2 mc index as the destiantion */
            dest_value = _BCM_MULTICAST_ID_GET(redirect_config->destination);

            break;

        default:

            /* Reset the redirection flag */
            redirect_set = 0;

            /* Reset the destination type */
            dest_type = _BCM_REDIRECT_DEST_TYPE_NONE;

            /* Reset the destination */
            dest_value = 0;

            break;

    } /* End switch */

    /* Resolve the gport we are setting the redirection properties on */
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port,
                                               &mod_id, &port_id,
                                               &trunk_id, &id));

    /* Get the egr port entry */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit,
             PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->mem, MEM_BLOCK_ANY,
             port_id, &egr_port_entry));

    /* Get the current dest_type */
    curr_dest_type = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_type);

    /* Change the dest-type and dest-value only for non-mtp destinations,
       this is because this gets setup in the 'mirror' module */
    if(curr_dest_type != _BCM_REDIRECT_DEST_TYPE_MTP) {
        /* Setup the destination type and destination values */
        soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                         PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_type,
                         dest_type);

        soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                         PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_value,
                         dest_value);
    }

    /* Based on the redirection being set/cleared, set the drop_original flag,
      when rediretion is set, we will drop the original packet */
    soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
            PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->drop_original,
            (redirect_set ? 1 : 0));

    /* Set the redirected packet color */
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {
        /* If the redirection is being added, set the user provided
           color value, if it being disabled force the color to 0,
           which is a hardware default */
        if (redirect_set) {
            /* Map the external color values to internal ones */
            switch(redirect_config->color) {
                case bcmPortRedirectColorGreen:
                    color = 0;
                    break;
                case bcmPortRedirectColorYellow:
                    color = 1;
                    break;
                case bcmPortRedirectColorRed:
                    color = 2;
                    break;
                default:
                    break;
            }

            /* Set the color, if a valid value is supplied */
            if (color != -1) {
                soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                        PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_color, color);
                soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                        PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_change_color,
                        1);
            } else {
                soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                        PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_change_color,
                        0);
            }
        } else {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                             PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_color,
                             0);
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_change_color,
                    0);
        }
    } else {
        /* For the older scheme, the color from enum maps directly */
       if (redirect_set) {
            if(redirect_config->color < bcmPortRedirectColorNone) {
                    soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                             PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_color,
                             redirect_config->color);
            }
       } else {
           soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_color,
                    0);
       }
    }

    /* Set the redirected packet priority, if redirection is being disabled, set
       to hardware defaults */
    if (redirect_set) {
        if(redirect_config->priority >= 0 && redirect_config->priority <= 15) {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                             PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_priority,
                             redirect_config->priority);
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
              PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_change_priority, 1);
        } else if (redirect_config->priority == -1){
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
              PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_change_priority, 0);
        } else {
            return BCM_E_PARAM;
        }
    } else {
        soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                         PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_priority,
                         0);
        soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_change_priority, 0);
    }

#if defined(BCM_EP_REDIRECT_VERSION_2)
    /* For the newer feature, set the additionally supported fields */
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {

        /* Set the trigger strength, if disabling redirection set it to
           default */
        if (redirect_set) {
            if((redirect_config->strength >= 0) &&
               (redirect_config->strength < BCMI_MAX_STRENGTH_VALUE)) {
                soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                       PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->strength,
                       redirect_config->strength);
            } else {
                return BCM_E_NONE;
            }
        } else {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                   PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->strength,
                   0);
        }

        /* Set the buffer priority, if disabling redirection set it to
           default */
        if (redirect_set) {
            if(redirect_config->buffer_priority &
               BCM_PORT_REDIRECT_BUFFER_PRIORITY_LOW) {
                buffer_priority = _BCM_PORT_REDIRECT_BUFFER_PRIORITY_LOW;
            } else if(redirect_config->buffer_priority &
               BCM_PORT_REDIRECT_BUFFER_PRIORITY_MEDIUM) {
                buffer_priority = _BCM_PORT_REDIRECT_BUFFER_PRIORITY_MEDIUM;
            } else if(redirect_config->buffer_priority &
               BCM_PORT_REDIRECT_BUFFER_PRIORITY_HIGH) {
                buffer_priority = _BCM_PORT_REDIRECT_BUFFER_PRIORITY_HIGH;
            }

            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->buffer_priority,
                buffer_priority);
        } else {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->buffer_priority,
                0);
        }

        /* Set the redirected packet action, based on the flag. If the
           redirection is being disabled, set it to default */
        if (redirect_set) {
            if(redirect_config->flags &
               BCM_PORT_REDIRECT_DROPPED_PACKET) {
                redirect_action_value = _BCM_REDIRECT_IF_DROPPED;
            } else if(redirect_config->flags &
                      BCM_PORT_REDIRECT_NOT_DROPPED_PACKET) {
                redirect_action_value = _BCM_REDIRECT_IF_NOT_DROPPED;
            } else {
                /* Default to always redirect */
                redirect_action_value = _BCM_REDIRECT_ALWAYS;
            }

            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->action,
                    redirect_action_value);
        } else {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->action,
                    0);
        }

        /* Set the redirected packet source to use, based on the flags.
           If the redirection is being disabled, set it to default */
        if (redirect_set) {
            if(redirect_config->flags &
               BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION) {
                redirect_source_value =
                    _BCM_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION;
            } else if(redirect_config->flags &
                      _BCM_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE) {
                redirect_source_value =
                    _BCM_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE;
            } else {
                /* Default it to original source */
                redirect_source_value =
                    _BCM_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE;
            }

            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->redir_pkt_source,
                redirect_source_value);
        } else {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->redir_pkt_source,
                0);
        }

        /* Set the truncate option, if user has chosen the same. If the
           redirection is being disabled, set it to default */
        if (redirect_set) {
            if(redirect_config->flags & BCM_PORT_REDIRECT_TRUNCATE) {
                soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->redir_pkt_truncate,
                    1);
            }
        } else {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->redir_pkt_truncate,
                0);
        }
    } /* soc_feature_ep_redirect_v2 */
#endif /* BCM_EP_REDIRECT_VERSION_2 */

    /* Write the entry to hardware */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_PORTm,
                                      MEM_BLOCK_ALL, port_id,
                                      &egr_port_entry));
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs5_port_redirect_config_set
 * Purpose:
 *      Initialize the redirection config structure
 * Parameters:
 *      unit             - (IN) device id.
 *      port             - (IN) Port number.
 *      redirect_config  - (IN) Pointer to the redirection config structure
 * Returns:
 *      None
 */
int bcmi_xgs5_port_redirect_config_set(int unit, bcm_gport_t port,
                                     bcm_port_redirect_config_t *redirect_config)
{
    int local_port = -1, flags = 0;

    /* Sanity check */
    if(redirect_config == NULL) {
        return BCM_E_PARAM;
    }

    /* Check the sanity of flags */
    flags = redirect_config->flags;
    if((flags & BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE) &&
       (flags & BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION)) {
        return BCM_E_PARAM;
    }

    if((flags & BCM_PORT_REDIRECT_DROPPED_PACKET) &&
       (flags & BCM_PORT_REDIRECT_NOT_DROPPED_PACKET)) {
        return BCM_E_PARAM;
    }

    /* Check flags that are only applicable on the redirection
       2.0 */
    if(!soc_feature(unit, soc_feature_ep_redirect_v2)) {
        if(flags & (BCM_PORT_REDIRECT_TRUNCATE |
                    BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE |
                    BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION |
                    BCM_PORT_REDIRECT_DROPPED_PACKET |
                    BCM_PORT_REDIRECT_NOT_DROPPED_PACKET |
                    BCM_PORT_REDIRECT_BUFFER_PRIORITY_LOW |
                    BCM_PORT_REDIRECT_BUFFER_PRIORITY_MEDIUM |
                    BCM_PORT_REDIRECT_BUFFER_PRIORITY_HIGH)) {
                return BCM_E_PARAM;
        }
    }

    /* Validate the port and extract the local_port */
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &local_port));

    /* Set the data */
    BCM_IF_ERROR_RETURN(_bcmi_xgs5_port_redirect_config_set(unit,
                                            port, redirect_config));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_redirect_get
 * Purpose:
 *      Configure the redirection data
 * Parameters:
 *      unit             - (IN) device id.
 *      port             - (IN) Port number.
 *      redirect_config  - (OUT) Pointer to the redirection config structure
 * Returns:
 *      None
 */
int _bcmi_xgs5_port_redirect_get(int unit, bcm_port_t port,
                          bcm_port_redirect_config_t *redirect_config)
{
    int color = 0, mod_id = -1, port_id = -1;
    int dest_type, dest_value = -1;
    egr_port_entry_t  egr_port_entry;
#if defined(BCM_EP_REDIRECT_VERSION_2)
    int redirect_action_value = 0, redirect_source_value = 0;
    int buffer_priority = 0, truncate_flag = 0;
#endif

    /* If the dest_type_field is invalid, this feature is not
       supported on this device */
    if (PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_type == INVALIDf) {
        return BCM_E_UNAVAIL;
    }

    /* Read the given egress port entry */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit,
            PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->mem, MEM_BLOCK_ANY,
            port, &egr_port_entry));

    /* Zero out the outgoing struct */
    sal_memset(redirect_config, 0, sizeof(bcm_port_redirect_config_t));

    /* Reset flags */
    redirect_config->flags = 0;
    redirect_config->buffer_priority = 0;

    /* Get the dest_type and dest values */
    dest_type = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_type);
    dest_value = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_value);

    /* Switch based on the destination type */
    switch(dest_type) {

        case _BCM_REDIRECT_DEST_TYPE_PORT:

            /* Extract the (Mod,Port) value from destination */
            BCMI_MOD_PORT_MODID_GET(dest_value, mod_id);
            BCMI_MOD_PORT_PORTID_GET(dest_value, port_id);

            BCM_GPORT_MODPORT_SET(redirect_config->destination,
                                  mod_id, port_id);
            redirect_config->flags |= BCM_PORT_REDIRECT_DESTINATION_PORT;

            break;

        case _BCM_REDIRECT_DEST_TYPE_TRUNK:

            /* Put together a trunk gport and set the type */
            BCM_GPORT_TRUNK_SET(redirect_config->destination, dest_value);
            redirect_config->flags |= BCM_PORT_REDIRECT_DESTINATION_TRUNK;

            break;

        case _BCM_REDIRECT_DEST_TYPE_MCAST:

            /* Put together a trunk gport and set the type */
            _BCM_MULTICAST_GROUP_SET(redirect_config->destination,
                                     _BCM_MULTICAST_TYPE_L2,
                                     redirect_config->destination);
            redirect_config->flags |= BCM_PORT_REDIRECT_DESTINATION_MCAST;

            break;

        default:

            /* Reset the destination type */
            dest_type = _BCM_REDIRECT_DEST_TYPE_NONE;

            /* Reset the destination */
            dest_value = 0;

            break;

    } /* End switch */

    /* Get the redirected packet color */
    color = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_color);

#if defined(BCM_EP_REDIRECT_VERSION_2)
    /* Map the internal color values to external ones */
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {
        switch(color) {
            case 0:
                redirect_config->color = bcmPortRedirectColorGreen;
                break;
            case 1:
                redirect_config->color = bcmPortRedirectColorYellow;
                break;
            case 2:
                redirect_config->color = bcmPortRedirectColorRed;
                break;
            default:
                break;
        }
    } else {
#endif
    switch(color) {
        case 0:
            redirect_config->color = bcmPortRedirectColorNone;
            break;
        case 1:
            redirect_config->color = bcmPortRedirectColorGreen;
            break;
        case 2:
            redirect_config->color = bcmPortRedirectColorYellow;
            break;
        case 3:
            redirect_config->color = bcmPortRedirectColorRed;
            break;

        default:
            break;
    }
#if defined(BCM_EP_REDIRECT_VERSION_2)
    }
#endif

    /* Get the redirected packet priority */
    redirect_config->priority = soc_EGR_PORTm_field32_get(unit,
               &egr_port_entry,
               PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_priority);

#if defined(BCM_EP_REDIRECT_VERSION_2)
    /* For the newer redirection, get the additionally supported fields */
    if (soc_feature(unit, soc_feature_ep_redirect_v2)) {

        /* Get the redirected packet strength */
        redirect_config->strength = soc_EGR_PORTm_field32_get(unit,
                   &egr_port_entry,
                   PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->strength);

        /* Get the redirected packet buffer priority */
        buffer_priority = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                   PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->buffer_priority);

        if (buffer_priority ==
            _BCM_PORT_REDIRECT_BUFFER_PRIORITY_LOW) {
            redirect_config->buffer_priority |=
                BCM_PORT_REDIRECT_BUFFER_PRIORITY_LOW;
        } else if(buffer_priority ==
                  _BCM_PORT_REDIRECT_BUFFER_PRIORITY_MEDIUM) {
            redirect_config->buffer_priority |=
                BCM_PORT_REDIRECT_BUFFER_PRIORITY_MEDIUM;
        } else if(buffer_priority ==
                  _BCM_PORT_REDIRECT_BUFFER_PRIORITY_HIGH) {
            redirect_config->buffer_priority |=
                BCM_PORT_REDIRECT_BUFFER_PRIORITY_HIGH;
        }

        /* Get the redirection action value */
        redirect_action_value = soc_EGR_PORTm_field32_get(unit,
               &egr_port_entry,
               PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->action);

        /* Set the redirected packet action, based on the flags */
        if(redirect_action_value == _BCM_REDIRECT_IF_DROPPED) {
            redirect_config->flags |= BCM_PORT_REDIRECT_DROPPED_PACKET;
        } else if(redirect_action_value ==
                  _BCM_REDIRECT_IF_NOT_DROPPED) {
            redirect_config->flags |= BCM_PORT_REDIRECT_NOT_DROPPED_PACKET;
        }

        /* Get the redirection source value */
        redirect_source_value = soc_EGR_PORTm_field32_get(unit,
                  &egr_port_entry,
                  PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->redir_pkt_source);

        /* Get the redirected packet source to use and update flags */
        if(redirect_source_value ==
           _BCM_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION) {
            redirect_config->flags |=
                BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION;
        } else if(redirect_source_value ==
                  _BCM_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE) {
            redirect_config->flags |=
                BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE;
        }

        /* Set the redirected packet truncate flag, if it is set */
        truncate_flag = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                      PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->redir_pkt_truncate);
        if(truncate_flag) {
            redirect_config->flags |= BCM_PORT_REDIRECT_TRUNCATE;
        }
    }

#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_redirect_config_get
 * Purpose:
 *      Get the redirection config data
 * Parameters:
 *      unit             - (IN) device id.
 *      port             - (IN) Port number.
 *      redirect_config  - (OUT) Pointer to the redirection config structure
 * Returns:
 *      None
 */
int bcmi_xgs5_port_redirect_config_get(int unit, bcm_gport_t port,
                                 bcm_port_redirect_config_t *redirect_config)
{
    int local_port = -1;

    /* Sanity check */
    if(redirect_config == NULL) {
        return BCM_E_PARAM;
    }

    /* Validate the port and extract the local_port */
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit,
                                                     port,
                                                     &local_port));

    /* Get the data */
    BCM_IF_ERROR_RETURN(_bcmi_xgs5_port_redirect_get(unit,
                                                     local_port,
                                                     redirect_config));

    return BCM_E_NONE;
}
