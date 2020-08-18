/* $Id: cint_sand_utils_oam.c,
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * OAM utility functions
 */

/* The cint  cint_queue_tests.c  must be included: used to get core from port*/

/* Enum signifying the device type. Should be ordered oldest device to newest*/
enum device_type_t {
    device_type_arad_a0 = 0,
    device_type_arad_plus = 1,
    device_type_jericho = 2,
    device_type_jericho_b = 3,
    device_type_jericho_plus = 4,
    device_type_qax = 5,
    device_type_qux = 6,
    device_type_jericho2 = 7
};

device_type_t device_type;

int NUMBER_OF_TPID = 2;
uint16 jer2_tpid[NUMBER_OF_TPID] = {0x8100, 0x9100};

int oam__device_type_get(int unit, device_type_t *device_type) {
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get\n");
        print rv;
        return rv;
    }

    *device_type =
            (info.device == 0x8650 && info.revision == 0) ? device_type_arad_a0 :
            (info.device == 0x8660) ? device_type_arad_plus :
            (((info.device == 0x8375) || (info.device == 0x8675)) && info.revision == 0x11) ? device_type_jericho_b :
            ((info.device == 0x8375) || (info.device == 0x8675)) ? device_type_jericho :
            (info.device == 0x8470) ? device_type_qax : (info.device == 0x8680) ? device_type_jericho_plus :
            (info.device == 0x8270) ? device_type_qux : (info.device == 0x8690) ? device_type_jericho2 : -1;

    return rv;
}

/*
 * create vlan-ports
 *   vlan-port: is Logical interface identified by (port-vlan-vlan).
 */
int
oam__vswitch_metro_add_port(int unit, int port_num,  bcm_gport_t *port_id, int flags){
    int rv;
    int base = port_num * 4;
    bcm_vlan_port_t vp1;
    bcm_info_t info;
    int is_jericho2 = 0;
    bcm_vlan_port_t_init(&vp1);

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_info_get \n", proc_name);
        return rv;
    }

    if (info.device == 0x8690)
    {
        is_jericho2 = 1;
    }

    /*
     * ring protection parameters
     */
    vp1.group = vswitch_metro_mp_info.flush_group_id[port_num];
    vp1.ingress_failover_id = vswitch_metro_mp_info.ingress_failover_id[port_num];
    vp1.failover_port_id = 0;
    vp1.egress_failover_id = vswitch_metro_mp_info.egress_failover_id[port_num];
    vp1.egress_failover_port_id = 0;

    /* the match criteria is port:1, out-vlan:10,  in-vlan:20 */
    vp1.criteria = single_vlan_tag ? BCM_VLAN_PORT_MATCH_PORT_VLAN : BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    if (is_jericho2) {
        /** In OAM we usually use tpid 0x8100. Since 0x8100 is CVLAN indicate it for application */
        vp1.criteria = single_vlan_tag ? BCM_VLAN_PORT_MATCH_PORT_CVLAN : vp1.criteria;
    }
    vp1.port = vswitch_metro_mp_info.sysports[port_num];
    vp1.match_vlan = vswitch_metro_mp_info.p_vlans[base + 0];
    vp1.match_inner_vlan = single_vlan_tag ? 0 : vswitch_metro_mp_info.p_vlans[base + 1];
    /* when packet forwarded to this port then it will be set with out-vlan:100,  in-vlan:200 */
    vp1.egress_vlan = vswitch_metro_mp_info.p_vlans[base + 2];
    vp1.egress_inner_vlan = single_vlan_tag ? 0 : vswitch_metro_mp_info.p_vlans[base + 3];

    vp1.flags = flags;
    vp1.vlan_port_id = *port_id;
    /* this is relevant only when get the gport, not relevant for creation */
    /* will be pupulated when the gport is added to service, using vswitch_port_add */
    vp1.vsi = 0;
    rv = bcm_vlan_port_create(unit,&vp1);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }

    /* save the created gport */
    *port_id = vp1.vlan_port_id;

    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode || is_jericho2) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vp1);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    }
    if(verbose >= 1){
        printf("Add vlan-port-id:0x%08x in-port:0x%08x match_vlan:0x%08x match_inner_vlan:0x%08x in unit %d\n",vp1.vlan_port_id, vp1.port, vp1.match_vlan, vp1.match_inner_vlan, unit);
    }


  return rv;
}


/**
 * Part of vswitch initialization process.
 *
 *
 * @param unit
 * @param known_mac_lsb
 * @param known_vlan
 *
 * @return int
 */
int
oam__vswitch_metro_run(int unit, int known_mac_lsb, int known_vlan){
    int rv;
    /*bcm_vlan_t  vsi*/;
    bcm_mac_t kmac;
    int index;
    bcm_vlan_t kvlan;
    int flags, i;

    kmac[5] = known_mac_lsb;
    kvlan = known_vlan;
    device_type_t device_type;

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error oam__device_type_get.\n");
        print rv;
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        /* Jer2 - use "new" sand Cints for TPID*/
        rv = tpid__tpids_clear_all(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in tpid__tpids_clear_all\n");
            return rv;
        }

        for (index = 0; index < NUMBER_OF_TPID; index++)
        {
            rv = tpid__tpid_add(unit, jer2_tpid[index]);

            if (rv != BCM_E_NONE)
            {
                printf("Error, in tpid__tpid_add(tpid_value=0x%04x)\n", jer2_tpid[index]);
                return rv;
            }
        }
    } else
    {
        /* Jer1: Use existing DPP cints.
           Set ports to identify double tags packets */
        port_tpid_init(vswitch_metro_mp_info.sysports[0], 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, port_tpid_set, in unit %d\n", unit);
            print rv;
            return rv;
        }

        port_tpid_init(vswitch_metro_mp_info.sysports[1], 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, port_tpid_set\n");
            print rv;
            return rv;
        }

        port_tpid_init(vswitch_metro_mp_info.sysports[2], 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, port_tpid_set\n");
            print rv;
            return rv;
        }
    }



    /* When using new vlan translation mode, tpid and vlan actions and mapping must be configured manually */
    if (advanced_vlan_translation_mode || (device_type == device_type_jericho2)) {
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    for (index = 0; index < 12; index++) {
        if (single_vlan_tag && (((index % 2) != 0))) {
            continue;
        }
        rv = vlan__init_vlan(unit,vswitch_metro_mp_info.p_vlans[index]);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Error, vlan__init_vlan\n");
            print rv;
            return rv;
        }
    }

    /* 1. create vswitch + create Multicast for flooding */
    rv = vswitch_create(unit, &vsi, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_create\n");
        print rv;
        return rv;
    }
    printf("Created vswitch vsi=0x%x\n", vsi);

    /* 2. create first vlan-port */

    flags = 0;
    gport1 = 0;
    rv = oam__vswitch_metro_add_port(unit, 0, &gport1, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_metro_add_port_1\n");
        print rv;
        return rv;
    }
    if(verbose){
        printf("created vlan-port   0x%08x in unit %d\n",gport1, unit);
    }

    /* 3. add vlan-port to the vswitch and multicast */
    rv = vswitch_add_port(unit, vsi,vswitch_metro_mp_info.sysports[0], gport1);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port\n");
        return rv;
    }
    /* add another port to the vswitch */

    /* 4. create second vlan-port */
    flags = 0;
    gport2 = 0;
    rv = oam__vswitch_metro_add_port(unit, 1, &gport2, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_metro_add_port_2\n");
        return rv;
    }
    if(verbose){
        printf("created vlan-port   0x%08x\n\r",gport2);
    }

    /* 5. add vlan-port to the vswitch and multicast */

    /* Local unit for sysport2 is already first */
    rv = vswitch_add_port(unit, vsi,vswitch_metro_mp_info.sysports[1], gport2);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port\n");
        return rv;
    }

    /* add a third port to the vswitch */

    /* 6. create third vlan-port */
    flags = 0;
    gport3 = 0;
    rv = oam__vswitch_metro_add_port(unit, 2, &gport3, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_metro_add_port_2\n");
        return rv;
    }
    if(verbose){
        printf("created vlan=port   0x%08x in unit %d\n",gport3, unit);
    }

    /* 7. add vlan-port to the vswitch and multicast */
    rv = vswitch_add_port(unit, vsi,vswitch_metro_mp_info.sysports[2], gport3);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port\n");
        return rv;
    }

    rv = vswitch_add_l2_addr_to_gport(unit, gport3, kmac, kvlan);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_l2_addr_to_gport\n");
        return rv;
    }
    return rv;
}

/**
 * Function that demonstrate setting of my cfm mac
 *
 * @param unit
 * @param src_port
 * @param my_cfm_mac
 *
 * @return int
 */
int oam__my_cfm_mac_set(int unit, bcm_gport_t src_port, bcm_mac_t my_cfm_mac)
{
     bcm_l2_station_t station;
     int station_id;
     int rv;
     /* Intitialize an L2 station structure */
     bcm_l2_station_t_init(&station);

     station.flags = BCM_L2_STATION_OAM;
     sal_memcpy(station.dst_mac, my_cfm_mac, 6);
     sal_memset(station.dst_mac_mask, 0xff, 6);
     station.src_port = src_port;
     station.src_port_mask = -1;

     rv = bcm_l2_station_add(unit, &station_id, &station);
     if (rv != BCM_E_NONE) {
        printf("bcm_l2_station_add  %d \n", station_id);
        return rv;
    }

    return rv;
}

/**
 * Procedure used to set a port as CPU port.
 * In case additional CPU ports are required for OAM handling.
 *
 *
 * @param unit
 * @param port
 *
 * @return int
 */
int
oam__switch_contol_indexed_port_set_to_cpu(int unit, bcm_port_t port){

    int rv = BCM_E_NONE;
    bcm_switch_control_key_t port_key_info;
    bcm_switch_control_info_t port_value_info;

    port_key_info.type = bcmSwitchPortHeaderType;
    /* CPU destination is supported on outgoing direction only */
    port_key_info.index = 2; 
    /* Set the port as CPU to allow system headers */
    port_value_info.value = BCM_SWITCH_PORT_HEADER_TYPE_CPU; 

    rv = bcm_switch_control_indexed_port_set(unit,port,port_key_info,port_value_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_switch_control_indexed_port_set\n");
        return rv;
    }

    return rv;
}
