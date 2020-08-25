/*
 * $Id: cint_dnx_coe_basic.c, Exp $
 $Copyright: (c) 2018 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$
 * File: cint_dnx_coe_basic.c
 * Purpose: Example of COE basic configuration.
 */

/*
 * This cint demonstrate configuration of COE.
 *
 * SOC:
 *      ucode_port_0.BCM8869X=CPU.0:core_0.0
 *      ucode_port_200.BCM8869X=CPU.8:core_1.200
 *      ucode_port_201.BCM8869X=CPU.16:core_0.201
 *      ucode_port_202.BCM8869X=CPU.24:core_0.202
 *      ucode_port_203.BCM8869X=CPU.32:core_0.203
 *
 * Usage:
 *
 * cint ../../../../src/examples/dpp/cint_port_extender_dynamic_switching.c
 * cint ../../../../src/examples/dnx/coe/cint_dnx_coe_basic.c
 * cint
 * non_coe_port=200;
 * non_coe_vlan=5;
 * print port_extender_dynamic_swithing_coe_info_init_multi_flows(0,201,201,10,0);
 * print port_extender_dynamic_swithing_coe_info_init_multi_flows(1,202,202,20,0);
 * print port_extender_dynamic_swithing_coe_info_init_multi_flows(2,203,203,30,0);
 * print port_dnx_coe_basic_service(0);
 */

int non_coe_port;
bcm_vlan_t non_coe_vlan;


 /*
  * Set coe port mapping. 
  */
 int port_dnx_coe_config(int unit, int coe_vlan, bcm_port_t coe_port, int pp_port, bcm_port_t phy_port)
 {
     int rv = 0;
     bcm_port_extender_mapping_info_t mapping_info;
     bcm_gport_t sys_gport;
 
     mapping_info.vlan = coe_vlan;
     mapping_info.pp_port = pp_port;
     mapping_info.phy_port = phy_port;
 
     /* 
      * Map port and COE VLAN ID to in pp port.
      * Add encapsulation VID.
      */
     rv = bcm_port_extender_mapping_info_set(
                unit,
                BCM_PORT_EXTENDER_MAPPING_INGRESS | BCM_PORT_EXTENDER_MAPPING_EGRESS,
                bcmPortExtenderMappingTypePortVlan,
                &mapping_info);
     if (rv != BCM_E_NONE) {
         printf("bcm_port_extender_mapping_info_set failed $rv\n");
         return rv;
     }
 
     /* 
      * Set the system port of the in pp port
      */
     BCM_GPORT_SYSTEM_PORT_ID_SET(&sys_gport, coe_port);
     rv = bcm_stk_sysport_gport_set(unit, sys_gport, coe_port);
     if (rv != BCM_E_NONE) {
         printf("bcm_stk_sysport_gport_set failed $rv\n");
         return rv;
     }
     
     return rv;
 }


 int port_dnx_create_p2p_service(int unit, int p1, bcm_vlan_t vlan1, int p2, bcm_vlan_t vlan2)
 {
     int rv;
     bcm_vlan_port_t vlan_port_1;
     bcm_vlan_port_t vlan_port_2;
     bcm_vswitch_cross_connect_t gports;
     
     bcm_vlan_port_t_init(&vlan_port_1);
     bcm_vlan_port_t_init(&vlan_port_2);
     vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
     vlan_port_2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;

     vlan_port_1.flags = BCM_VLAN_PORT_CROSS_CONNECT;
     vlan_port_2.flags = BCM_VLAN_PORT_CROSS_CONNECT;
     
     vlan_port_1.port = p1;
     vlan_port_1.match_vlan = vlan1;
     
     vlan_port_2.port = p2;
     vlan_port_2.match_vlan = vlan2;
     
     rv = bcm_vlan_port_create(0, &vlan_port_1);
     if (rv != BCM_E_NONE) {
         printf("bcm_vlan_port_create failed!\n");
         return rv;
     }

     rv = bcm_vlan_port_create(0, &vlan_port_2);
     if (rv != BCM_E_NONE) {
         printf("bcm_vlan_port_create failed!\n");
         return rv;
     }

 
     bcm_vswitch_cross_connect_t_init(&gports);
     gports.port1= vlan_port_1.vlan_port_id;
     gports.port2= vlan_port_2.vlan_port_id;
     rv = bcm_vswitch_cross_connect_add(unit, &gports);
     if (rv != BCM_E_NONE) {
         printf("P2P service create failed!\n");
         return rv;
     }

     return rv;

 }


 int port_dnx_coe_basic_service(int unit)
 {
    int rv = 0;
    int index;

    /*
     * init COE mapping info(at least 3 COE port)
     * Calling port_extender_dynamic_swithing_coe_info_init_multi_flows() for each coe port mapping.
     */

    /** enable port and set mapping*/
    for (index = 0; index < nof_coe_ports_multi_flows; index++) {
        if (coe_port_info_multi_flows[index].coe_port != 0) {
            rv = bcm_port_control_set(unit, coe_port_info_multi_flows[index].coe_port, bcmPortControlExtenderEnable, 1);
            if (rv != BCM_E_NONE) {
                printf("Enable COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
            
            rv = port_dnx_coe_config(
                        unit,
                        coe_port_info_multi_flows[index].coe_vlan,
                        coe_port_info_multi_flows[index].coe_port,
                        coe_port_info_multi_flows[index].pp_port,
                        coe_port_info_multi_flows[index].phy_port);
            if (rv != BCM_E_NONE) {
                printf("Mapping for COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
        }
    }

    /*
     * create P2P service 
     *      1: coe_port <---> coe_port
     *      2: regular port <---> coe_port
     */
    rv = port_dnx_create_p2p_service(
            unit,
            coe_port_info_multi_flows[0].coe_port,
            coe_port_info_multi_flows[0].coe_vlan+5,
            coe_port_info_multi_flows[1].coe_port,
            coe_port_info_multi_flows[1].coe_vlan+5);
    if (rv != BCM_E_NONE) {
        printf("Create P2P service for COE<-->COE failed!\n");
        return rv;
    }

    rv = port_dnx_create_p2p_service(
            unit,
            coe_port_info_multi_flows[2].coe_port,
            coe_port_info_multi_flows[2].coe_vlan+5,
            non_coe_port,
            non_coe_vlan);
    if (rv != BCM_E_NONE) {
        printf("Create P2P service for COE<-->Regular failed!\n");
        return rv;
    }
    return rv;
 }


int port_dnx_coe_basic_service_clean_up(int unit)
{
    int rv = 0;
    int index;

    for (index = 0; index < nof_coe_ports_multi_flows; index++) {
        if (coe_port_info_multi_flows[index].coe_port != 0) {
            rv = bcm_port_control_set(unit, coe_port_info_multi_flows[index].coe_port, bcmPortControlExtenderEnable, 0);
            if (rv != BCM_E_NONE) {
                printf("Disable COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
        }
    }


    return rv;
}













