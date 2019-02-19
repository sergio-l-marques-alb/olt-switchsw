/* $Id: cint_port_extender_cb_uc.c,v 1.10 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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

/* **************************************************************************************************************************************************
 * Set following SOC properties:                                                                                                                    *
 * 1. Configure the device to be a Control Bridge: extender_control_bridge_enable. 1: Enable, 0: Disable.                                           *
 * 2. Set the number of bytes to prepend to the start of the packet: prepend_tag_bytes = 8B                                                         *
 * 3. Set the offset in bytes from the DA/SA in the packet from which to prepend the tag: prepend_tag_offset = 0                                    *
 *                                                                                                                                                  * 
 *                                                                                                                                                  * 
 *                                                                                                                                                  * 
 * The CINT demonstrates a Controlling bridge configuration as part of a Port Extender (802.BR) application.                                        *
 * The basic example handles UC traffic with no protection.                                                                                         *
 *                                                                                                                                                  *
 * Packets can be sent from/to a cascaded port or an End station.                                                                                   *
 *                                                                                                                                                  *
 *                                                                                                                                                  * 
 *                                                                                                                                                  *
 *                                                                         Network                                                                  *
 *                                                                       | Port                                                                     *
 *                                                                       |                                                                          *
 *                                                    ___________________|__________________                                                        *
 *                 __________                        |                                      |                           __________                  *
 *        LIF 1 __|          |                       |         Controlling Bridge           |                          |          |__  LIF 3        *
 *        LIF 2 __|   Port   | --------------------->|------------------------------------> |------------------------->|  Port    |__               *
 *              __|          |      In-Cascaded-Port |                    |    /\           | Out-Cascaded-Port        |          |__               *
 *              __| Extender | <---------------------|<-------------------|    |            |                          | Extender |__               *
 *                |__________|                       |                    |    |            |                          |__________|                 *
 *                                                   |                    |    |            |                                                       * 
 *                             _____________         |                    |    |            |                                                       *
 *                            |             |<------ |<-------------------     |            |                                                       *
 *                  LIF4(AC)  | End Station |        |                         |            |                                                       *
 *                            |_____________|------->|-------------------------             |                                                       *
 *                                                   |                                      |                                                       *
 *                                                   |______________________________________|                                                       *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * Configuration:                                                                                                                                   *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/dpp                                                                                                                  * 
 * cint cint_utils_global.c                                                                                                                         *
 * cint cint_utils_port.c                                                                                                                           *
 * cint cint_utils_l2.c                                                                                                                             *
 * cint cint_utils_vlan.c                                                                                                                           *
 * cint cint_port_extender_cb_uc.c                                                                                                                  *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    * 
 * port_extender_cb_uc__start_run(int unit,  port_extender_cb_uc_s *param) - param = NULL for default params                                        * 
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  * 
 * All default values could be re-written with initialization of the global structure 'g_port_extender_cb_uc', before calling the main function.   * 
 * In order to re-write only part of the values, call 'port_extender_cb_uc_struct_get(port_extender_cb_uc_s)' function and re-write the values      *
 * prior to calling the main function.                                                                                                              * 
 ************************************************************************************************************************************************** */
 
 
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */
int NOF_EXTENDER_PORTS = 3;

struct port_extender_cascaded_port_s {
    bcm_gport_t port;
    bcm_port_tag_format_class_t tag_format;     /* Tag format used for Tag classification */
};
 
struct port_extender_port_s {
    bcm_gport_t port;
    bcm_vlan_t tag_vid;                         /* Outer C-VID */
    uint16 extender_vid;                        /* Extended-Port E-CID */
    bcm_gport_t extender_gport_id;              /* The LIF ID */
};

struct port_extender_end_station_s {
    bcm_gport_t port;
    bcm_vlan_t tag_vid;
    bcm_vlan_t tag_format;                      /* Tag format used for Tag classification */
    bcm_gport_t vlan_port_id;              /* The LIF ID */
};

/*  Main Struct  */
struct port_extender_cb_uc_s {
    port_extender_cascaded_port_s cascaded_port[NUMBER_OF_PORTS];
    port_extender_port_s extender_port[NOF_EXTENDER_PORTS];
    port_extender_end_station_s end_station;
    bcm_vlan_t vsi;
    uint32 extender_tpid;
    uint32 tag_tpid;
    uint32 ext_vlan_edit_profile;               /* A VLAN-Edit profile that will be used only by Extender Ports */
};

/* Initialization of global struct*/
port_extender_cb_uc_s g_port_extender_cb_uc = { /* Cascaded Ports configuration
                                                Phy Port    Tag Format                  */
                                                {{ 13,       5   },     /* In-Cascaded-Port  */
                                                 { 14,       5   }},    /* Out-Cascaded-Port */

                                                /* Extender Ports configuration
                                                Phy Port    Tag VID     Extender VID    Extender gport ID */
                                                {{ 13,       100,       201,            0 },    /* LIF 1 */
                                                 { 13,       100,       202,            0 },    /* LIF 2 */
                                                 { 14,       100,       203,            0 }},   /* LIF 3 */

                                                /* End Station configuration
                                                Phy Port    Tag VID     Tag format      vlan_port_id*/
                                                 { 15,       100,       4,              0 },

                                                /* Additional parameters */
                                                12,         /* VSI */
                                                0x893F,     /* Extender TPID */
                                                0x8100,     /* Tag TPID */
                                                3};         /* VLAN-Edit Profile */


/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */

/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation.
 *
 * INPUT: 
 *   params: new values for g_port_extender_cb_uc
 */
int port_extender_cb_uc_init(int unit, port_extender_cb_uc_s *params) {

    int rv, cb_enable, prepend_bytes, prepend_offset, avt_mode;

    if (params != NULL) {
       sal_memcpy(&g_port_extender_cb_uc, params, sizeof(g_port_extender_cb_uc));
    }

    /* Verify Per device SOC Properties */
    cb_enable = soc_property_get(unit ,"extender_control_bridge_enable", 0);
    if (!cb_enable) {
        printf("Error in port_extender_cb_uc_init, cb_enable - %d\n", cb_enable);
        return BCM_E_PARAM;
    }

    prepend_bytes = soc_property_get(unit ,"prepend_tag_bytes", 0);
    if (prepend_bytes != 8) {
        printf("Error in port_extender_cb_uc_init, prepend_bytes - %d\n", prepend_bytes);
        return BCM_E_PARAM;
    }

    prepend_offset = soc_property_get(unit ,"prepend_tag_offset", 0);
    if (prepend_offset) {
        printf("Error in port_extender_cb_uc_init, prepend_offset - %d\n", prepend_offset);
        return BCM_E_PARAM;
    }

    /* Port Extender is only  supported in Advanced VLAN Edit (AVT) mode */
    avt_mode = soc_property_get(unit ,"bcm886xx_vlan_translate_mode", 0);
    if (!avt_mode ) {
        printf("Error in port_extender_cb_uc_init, avt_mode - %d\n", avt_mode);
        return BCM_E_PARAM;
    }

    /* Define a VSI for the switch */
    rv = bcm_vswitch_create_with_id(unit, g_port_extender_cb_uc.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create failed for vsi - %d, rv - %d\n", g_port_extender_cb_uc.vsi, rv);
        return rv;
    }

    /* Set the global Etag ethertype */
    rv = bcm_switch_control_set(unit, bcmSwitchEtagEthertype, g_port_extender_cb_uc.extender_tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set failed for bcmSwitchEtagEthertype, etag_ethertype: 0x%x\n", g_port_extender_cb_uc.etag_ethertype);
        return rv;
    }

    return BCM_E_NONE;
}


/* Configuration of a single cascaded port.
 * Various per physical port configuration that determine the prepend tag,
 * the Extender device type and filter configuration.
 * In addition, the function handles the port's TPIDs - The profile and the
 * tag format classification.
 *
 * INPUT: 
 *   cascaded_port_info: Configuration info for a single cascaded port.
 */
int port_extender_cb_uc__set_cascaded_port(int unit, port_extender_cascaded_port_s *cascaded_port_info) {

    int rv;
    uint32 vlan_domain, enable, flags;

    /* Enable prepend operation for the port */
    rv = bcm_switch_control_port_set(unit, cascaded_port_info->port, bcmSwitchPrependTagEnable, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_switch_control_set for bcmSwitchPrependTagEnable, port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    /* Configure the Port Extender Port-type to be a Cascaded Port */
    rv = bcm_port_control_set(unit, cascaded_port_info->port, bcmPortControlExtenderType, BCM_PORT_EXTENDER_TYPE_SWITCH);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set for bcmPortControlExtenderType to SWITCH, port - %d, rv - %d\n", cascaded_port_info->port, rv);
    }

    /* Attach the port to a VLAN-Domain - The same as the port in this case (but doesn't have to) */
    vlan_domain = cascaded_port_info->port;
    rv = bcm_port_class_set(unit, cascaded_port_info->port, bcmPortClassId, vlan_domain);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set for port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    /* Disable VLAN-Membership filter */
    flags = 0;        
    rv = bcm_port_vlan_member_set(unit, cascaded_port_info->port, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_vlan_member_set to disable for port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    /* Disable same-interface filter */
    enable = 0;
    rv = bcm_port_control_set(unit, cascaded_port_info->port, bcmPortControlBridge, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set for bcmPortControlBridge to Disable, port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }   

    /* Drop the untagged channel */
    enable = 1;
    rv = bcm_port_control_set(unit, cascaded_port_info->port, bcmPortControlNonEtagDrop, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set for bcmPortControlNonEtagDrop to Enable, port - %d, rv - %d\n", cascaded_port_info->port, rv);
    }

    /* Configure the TPIDs for the port and their classification */
    rv = port__tpids__set(unit, cascaded_port_info->port, g_port_extender_cb_uc.extender_tpid, g_port_extender_cb_uc.tag_tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, port__tpids__set for port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    rv = port__tag_classification__set(unit, cascaded_port_info->port, cascaded_port_info->tag_format, g_port_extender_cb_uc.extender_tpid, g_port_extender_cb_uc.tag_tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, port__tag_classification__set for port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Configuration of a Port Extender single Extender LIF
 * Create the object(LIF), associate it with a VSI and define an EVE operation. 
 *  
 * INPUT: 
 *   extender_port_info: Configuration info for a single Extender LIF.
 */
int port_extender_cb_uc__set_extender_lif(int unit, port_extender_port_s *extender_port_info) {

    int rv;
    bcm_extender_port_t extender_port;

    /* Format the supplied LIF info to the Port-Extender struct */
    bcm_extender_port_t_init(&extender_port);
    extender_port.port = extender_port_info->port;
    extender_port.match_vlan = extender_port_info->tag_vid;
    extender_port.extended_port_vid = extender_port_info->extender_vid;

    extender_port.pcp_de_select = BCM_EXTENDER_PCP_DE_SELECT_OUTER_TAG;
    extender_port.flags = BCM_EXTENDER_PORT_INGRESS_WIDE;

    /* Call the API to create the Port Extender object */
    rv = bcm_extender_port_add(unit, &extender_port);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_extender_port_add, port - %d, vlan - %d, E-TAG - %d, rv - %d\n", extender_port.port, extender_port.match_vlan, extender_port.extended_port_vid, rv);
    }
    extender_port_info->extender_gport_id = extender_port.extender_port_id;

    /* Attach the Extender-Port to a specific VSI */
    rv = bcm_vswitch_port_add(unit, g_port_extender_cb_uc.vsi, extender_port.extender_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_port_add, vsi - %d, extender_port_id - %d, rv - %d\n", g_port_extender_cb_uc.vsi, extender_port.extender_port_id, rv);
    }
    printf("Allocated extender_port_id - %x\n", extender_port.extender_port_id);

    return BCM_E_NONE;
    /* Configure an EVE action that Adds the E-TAG of the destination VM */
    rv = vlan__avt_eve_vids__set(unit, extender_port.extender_port_id, g_port_extender_cb_uc.extender_tpid, g_port_extender_cb_uc.tag_tpid, bcmVlanActionAdd, bcmVlanActionNone, extender_port_info->extender_vid, 0, g_port_extender_cb_uc.ext_vlan_edit_profile);
    if (rv != BCM_E_NONE) {
        printf("Error in vlan__avt_eve_vid_action__set, extender_port_id - %d, rv - %d\n", extender_port.extender_port_id, rv);
    }

    return BCM_E_NONE;
}


/* Configuration of a Port Extender End station
 * Handles the port's TPIDs - The profile and the tag format classification,
 * creates the AC LIF and associates it with a VSI.
 * No EVE is defined as the C-VID in this example is the same across all the
 * interfaces.
 *
 * INPUT: 
 *   end_station_info: Configuration info for a single End station
 */
int port_extender_cb_uc__set_end_station(int unit, port_extender_end_station_s *end_station_info) {

    int rv;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_translation_t vp_translation;

    /* Configure the TPIDs for the port and their classification */
    rv = port__tpids__set(unit, end_station_info->port, g_port_extender_cb_uc.tag_tpid, 0x0);
    if (rv != BCM_E_NONE) {
        printf("Error, port__tpids__set for port - %d, rv - %d\n", end_station_info->port, rv);
        return rv;
    }

    
    rv = port__tag_classification__set(unit, end_station_info->port, end_station_info->tag_format, g_port_extender_cb_uc.tag_tpid, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, port__tag_classification__set for port - %d, rv - %d\n", end_station_info->port, rv);
        return rv;
    }

    /* Create an AC LIF */
    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = end_station_info->port;
    vlan_port.match_vlan = end_station_info->tag_vid;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create for port - %d, vlan - %d, rv - %d\n", end_station_info->port, end_station_info->tag_vid, rv);
        return rv;
    }
    end_station_info->vlan_port_id = vlan_port.vlan_port_id;

    /* Attach the AC LIF to the Controller Bridge VSI */
    rv = bcm_vswitch_port_add(unit, g_port_extender_cb_uc.vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_port_add, vsi - %d, vlan_port_id - %d, rv - %d\n", g_port_extender_cb_uc.vsi, vlan_port.vlan_port_id, rv);
        return rv;
    }

    /* No IVE/EVE is configured as it is assumed that the End Station provides packets with the same C-TAG value.
       In other cases, the IVE/EVE should reflect the C-TAG manipulation. */

    /*bcm_vlan_port_tranlation_init(&vp_translation);

    vp_translation.gport = end_station_info->vlan_port_id;

    rv = bcm_vlan_port_translation_set(unit, &vp_translation);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_port_add, vsi - %d, vlan_port_id - %d, rv - %d\n", g_port_extender_cb_uc.vsi, vlan_port.vlan_port_id, rv);
        return rv;
    }*/

    bcm_vlan_action_set_t action;

    bcm_vlan_action_set_t_init(&action);

    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, 0, &action);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_translate_action_id_set, vsi - %d, vlan_port_id - %d, rv - %d\n", g_port_extender_cb_uc.vsi, vlan_port.vlan_port_id, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * Main function to run the Port Extender Control Bridge UC example 
 *  
 * Main steps of configuration: 
 *    1. Initialize test parameters
 *    2. Set Cascaded Ports
 *    3. Set Port-Extender LIFs
 *    4. Set an End Station
 *    
 * INPUT: unit  - unit
 *        param - NULL for default params, or new values.
 */
int port_extender_cb_uc__start_run(int unit,  port_extender_cb_uc_s *params) {

    int rv, idx;
    uint32 vlan_domain;

    /* Initialize the test parameters */
    rv = port_extender_cb_uc_init(unit, params);
    if (rv != BCM_E_NONE) {
        printf("Error in port_extender_cb_uc_init, rv - %d\n", rv);
        return rv;
    }

    /* Configure the cascaded ports */
    for (idx = 0; idx < NUMBER_OF_PORTS; idx++) {
        rv = port_extender_cb_uc__set_cascaded_port(unit, &g_port_extender_cb_uc.cascaded_port[idx]);
        if (rv != BCM_E_NONE) {
            printf("Error in port_extender_cb_uc__set_cascaded_port for port #%d, rv - %d\n", idx, rv);
            return rv;
        }
    }

    rv = port_extender_no_etag_filter__create(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_extender_no_etag_filter__create, rv - %d\n", rv);
        return rv;
    }


    /* Configure Port-Extender LIFs */
    for (idx = 0; idx < NOF_EXTENDER_PORTS; idx++) {
        rv = port_extender_cb_uc__set_extender_lif(unit, &g_port_extender_cb_uc.extender_port[idx]);
        if (rv != BCM_E_NONE) {
            printf("Error in port_extender_cb_uc__set_extender_lif for LIF #%d, rv - %d\n", idx, rv);
            return rv;
        }
    }


    /* Configure an End Station */
    rv = port_extender_cb_uc__set_end_station(unit, &g_port_extender_cb_uc.end_station);
    if (rv != BCM_E_NONE) {
        printf("Error in port_extender_cb_uc__set_end_station, rv - %d\n", rv);
        return rv;
    } 

    /* Configure a mac address for the endpoint. */
    l2__mact_properties_s endpoint_mact_entry = { g_port_extender_cb_uc.end_station.vlan_port_id,
                                        {0x00, 0x00, 0x12, 0x34, 0x56, 0x78},
                                        g_port_extender_cb_uc.vsi
    };

    rv = l2__mact_entry_create(unit, &endpoint_mact_entry);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create, rv - %d\n", rv);
        return rv;
    } 

    /* Configure a mac address for the out cascade port. */
    l2__mact_properties_s out_cascade_mact_entry = { g_port_extender_cb_uc.extender_port[2].extender_gport_id,
                                        {0x00, 0x00, 0x9a, 0xbc, 0xde, 0xf0},
                                        g_port_extender_cb_uc.vsi
    };

    rv = l2__mact_entry_create(unit, &out_cascade_mact_entry);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create, rv - %d\n", rv);
        return rv;
    } 


    printf("Done configuring\n");

    return BCM_E_NONE;
}

/*
 * Configuration of the extender no etag filter. 
 * Setting the port property  bcmPortControlNonEtagDrop is not enough to create no e-tag filtering - 
 *  the filtering itself is done in the field proccessor.
 * The bcmPortControlNonEtagDrop configures the port to use an e-tpid tt lookup program, and this 
 *  field program will filter any packets that fail their tt lookup. Therefore, in order to configure
 *  the filter, the port property must be set and this function should be called, per port.
 *  When the filter is removed, this field program should be removed as well.
 *    
 * INPUT: unit  - unit
 */
int port_extender_no_etag_filter__create(int unit){
    int rv;
    bcm_field_group_config_t grp_config;
    bcm_field_entry_t ent;
    int idx;

    bcm_field_group_config_t_init(&grp_config);

    /* Define the QSET */
    BCM_FIELD_QSET_INIT(grp_config.qset);
    BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyTunnelTerminated);
    BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyInPort);

    /* Define the ASET */
    BCM_FIELD_ASET_INIT(grp_config.aset);
    BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionDrop);
    
    /*  Create the Field group */
    grp_config.priority = 1;
    grp_config.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET;
    rv = bcm_field_group_config_create(unit, &grp_config);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create\n");
        return rv;
    }

    for (idx = 0; idx < NUMBER_OF_PORTS ; idx++) {
            
        rv = bcm_field_entry_create(unit, grp_config.group, &ent);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_entry_create\n");
            return rv;
        }


        rv = bcm_field_qualify_TunnelTerminated(unit, ent, 0x0, 0x1);
        if (rv != BCM_E_NONE) {
          printf("Error in bcm_field_qualify_TunnelTerminated\n");
          return rv;
        }

        rv = bcm_field_qualify_InPort(unit, ent, g_port_extender_cb_uc.cascaded_port[idx].port, ~0);
        if (rv != BCM_E_NONE) {
          printf("Error in bcm_field_qualify_InPort\n");
          return rv;
        }

        rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_action_add\n");
            return rv;
        }
    }

    /* Install all to the HW */
    rv = bcm_field_group_install(unit, grp_config.group);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_install\n");
        return rv;
    }

    printf("Done creating field filter\n");

    return rv;
}

/*
 * You can use this function to change the ports the cint is using from the defaults (13, 14 and 15).
 *    
 * INPUT: unit              - unit 
 *        in_cascaded_port  - cascaded port holding lifs 1 and 2
 *        out_cascaded_port - cascaded port holding lif 3
 *        end_station_port  - ethernet port holding lif 4
 */
int port_extender_cb_uc__start_run_dvapi(int unit, int in_cascaded_port, int out_cascaded_port, int end_station_port){
    port_extender_cb_uc_s params;

    sal_memcpy(&params, &g_port_extender_cb_uc, sizeof (g_port_extender_cb_uc));

    params.cascaded_port[0].port = in_cascaded_port;
    params.cascaded_port[1].port = out_cascaded_port;

    params.end_station.port = end_station_port;

    params.extender_port[0].port = in_cascaded_port;
    params.extender_port[1].port = in_cascaded_port;
    params.extender_port[2].port = out_cascaded_port;

    return port_extender_cb_uc__start_run(unit, &params);
}
