/*
 * $Id: cint_hqos.c, Exp $
 $Copyright: (c) 2019 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$
 * File: cint_hqos.c
 * Purpose: Example of H-QoS configuration.
 */

/*
 * This cint demonstrate example of H-QoS.
 * HQoS feature allow one local port mapped to more than one system ports that are mapped to different VoQ.
 * The first mapped sys_port called master system port while others called slave system port.
 *      1. Lifs created with master or slave system port will share same lookup key(same vlan domain) for VTT
 *      2. Learning information can be distinguished for master and slave
 *      3. forwarding destination can be distinguished for master and slave
 * 
 * SOC:
 *      voq_mapping_mode=DIRECT
 *      hqos_mapping_enable=1
 *
 * Usage:
 *
 * cint ../../../../src/examples/dpp/internal/cint_hqos.c
 * cint
 * hqos_example(0,13,14);
 * 
 */

/*
 * Set system port for [mod_id+port_id]
 * It can be use for master and slave system port setting 
 */
int system_port_set(int unit, int system_port_id, int mod_id, int local_port_id)
{
     int rv = BCM_E_NONE;
     bcm_gport_t sys_gport;
     bcm_gport_t dest_gport;
 
     BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, system_port_id); 
     BCM_GPORT_MODPORT_SET(dest_gport, mod_id, local_port_id);
     
     rv = bcm_stk_sysport_gport_set(unit, sys_gport, dest_gport);
     if(rv != BCM_E_NONE) {
         printf("bcm_stk_sysport_gport_set gport failed: unit=%d, sysport=0x%x, destport=0x%x\n",
                 unit, sys_gport, dest_gport);
         return rv;
     }
     printf("Set mod_id[%d]-port_id[%d] to system_port[%#x] successfully!\n", mod_id, local_port_id, system_port_id);
     return rv;
}

/*
 * assign ingress queue_id to DSP
 */
int ingress_voq_setting(int unit, bcm_gport_t sys_port, int queue_id)
{
    int rv = BCM_E_NONE;
    int core_id = 0;
    bcm_gport_t gport_queue;
    char cmd[300] = {0};

    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gport_queue, core_id, queue_id);
    rv = bcm_cosq_sysport_ingress_queue_map_set(unit, 0, sys_port, gport_queue);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_cosq_sysport_ingress_queue_map_set for system_port-[%#x], queue[%d]\n", sys_port, queue_id);
        return rv;
    }
    sprintf(cmd, "diag cosq voq id=%d detailed=1 core=%d", queue_id, core_id);
    bshell(unit, cmd);
    return rv;
}

int slave_system_port_in = 6000;
int slave_system_port_out = 6001;
int ingress_queue_port_in = 1624;    /*for CPU port*/
int ingress_queue_port_out = 1632;   /*for CPU port*/
bcm_vlan_t match_vlan = 100;

int hqos_example(int unit, int in_port, int out_port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port_1;
    bcm_vlan_port_t vlan_port_2;
    bcm_vswitch_cross_connect_t gports;
    int my_modid;
    bcm_gport_t g_slave_system_port_in;
    bcm_gport_t g_slave_system_port_out;
    bcm_gport_t g_master_system_port_out;
    
    bcm_vlan_port_t_init(&vlan_port_1);
    bcm_vlan_port_t_init(&vlan_port_2);

    BCM_GPORT_SYSTEM_PORT_ID_SET(g_master_system_port_out, out_port);
    BCM_GPORT_SYSTEM_PORT_ID_SET(g_slave_system_port_in, slave_system_port_in);
    BCM_GPORT_SYSTEM_PORT_ID_SET(g_slave_system_port_out, slave_system_port_out);

    /*set vlan domain*/
    rv = bcm_port_class_set(unit, in_port, bcmPortClassId, in_port);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_port_class_set for port[%d]!\n", in_port);
        return rv;
    }
    rv = bcm_port_class_set(unit, out_port, bcmPortClassId, out_port);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_port_class_set for port[%d]!\n", out_port);
        return rv;
    }

    /*Get my mod_id*/
    rv = bcm_stk_my_modid_get(unit, &my_modid);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_stk_my_modid_get!\n");
        return rv;
    }
    printf("Get my mod_id[%d]\n", my_modid);

    /*master system port is already set when init according to SOC*/
    /*slave system port setting*/
    rv = system_port_set(unit, slave_system_port_in, my_modid, in_port);
    if (rv != BCM_E_NONE) {
        printf("Set slave DSP failed for mod_id[%d]-port_id[%d]\n", my_modid, in_port);
        return rv;
    }
    rv = system_port_set(unit, slave_system_port_out, my_modid, out_port);
    if (rv != BCM_E_NONE) {
        printf("Set slave DSP failed for mod_id[%d]-port_id[%d]\n", my_modid, out_port);
        return rv;
    }

    /* voq mapping */    
    rv = ingress_voq_setting(unit, g_slave_system_port_in, ingress_queue_port_in);
    if (rv != BCM_E_NONE) {
        printf("Error in ingress_voq_setting!\n");
        return rv;
    }
    rv = ingress_voq_setting(unit, g_slave_system_port_out, ingress_queue_port_out);
    if (rv != BCM_E_NONE) {
        printf("Error in ingress_voq_setting!\n");
        return rv;
    }

    /*AC create with slave*/
    vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port_1.port = g_slave_system_port_in;
    vlan_port_1.match_vlan = match_vlan;
    vlan_port_1.egress_vlan = match_vlan;
    rv = bcm_vlan_port_create(unit,&vlan_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_create!\n");
        return rv;
    }
    printf("AC 1: g_lif[%#x]\n", vlan_port_1.vlan_port_id);
    /*AC create with master*/
    vlan_port_2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port_2.port = g_master_system_port_out;
    vlan_port_2.match_vlan = match_vlan;
    vlan_port_2.egress_vlan = match_vlan;
    rv = bcm_vlan_port_create(unit,&vlan_port_2);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_create!\n");
        return rv;
    }
    printf("AC 2: g_lif[%#x]\n", vlan_port_2.vlan_port_id);
    
    /*cross-connect*/
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = vlan_port_1.vlan_port_id;
    gports.port2 = vlan_port_2.vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_cross_connect_add!\n");
        return rv;
    }
    printf("Cross connect successfully!\n");
    return rv;
}


/*
 * Following is for Sem test under SOC: hqos_mapping_enable=1
 */

int hqos_sem_create_lif(bcm_vlan_port_match_t criteria, int port, bcm_vlan_t vlan, bcm_gport_t *lif_gport)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t lif;
    
    bcm_vlan_port_t_init(&lif);

    lif.criteria = criteria;
    lif.port = port;
    lif.match_vlan = vlan;

    rv = bcm_vlan_port_create(0, &lif);
    if (rv != BCM_E_NONE) {
        printf("Error in hoqs_sem_create_lif!\n");
        return rv;
    }

    *lif_gport = lif.vlan_port_id;
    return rv;
}

int hqos_sem_match_add(bcm_port_match_t criteria, int port, bcm_vlan_t vlan, bcm_gport_t lif_gport)
{
    int rv = BCM_E_NONE;
    bcm_port_match_info_t match_info;

    bcm_port_match_info_t_init(&match_info);

    match_info.port = port;
    match_info.match = criteria;
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;

    rv = bcm_port_match_add(0, lif_gport, &match_info);
    if (rv != BCM_E_NONE) {
        printf("Error in hoqs_sem_match_add!\n");
        return rv;
    }

    return rv;
}


int hqos_sem_match_delete(bcm_port_match_t criteria, int port, bcm_vlan_t vlan, bcm_gport_t lif_gport)
{
    int rv = BCM_E_NONE;
    bcm_port_match_info_t match_info;

    bcm_port_match_info_t_init(&match_info);

    match_info.port = port;
    match_info.match = criteria;
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;

    rv = bcm_port_match_delete(0, lif_gport, &match_info);
    if (rv != BCM_E_NONE) {
        printf("Error in hoqs_sem_match_delete!\n");
        return rv;
    }

    return rv;
}


int hqos_sem_test(int unit)
{
    int rv = BCM_E_NONE;
    int sysport_gport, lif_gport1, lif_gport2;


    rv = hqos_sem_create_lif(BCM_VLAN_PORT_MATCH_NONE, 0, 0, &lif_gport1);
    if (rv != BCM_E_NONE) {
        printf("Error in hqos_sem_create_lif for BCM_VLAN_PORT_MATCH_NONE!\n");
        return rv;
    }

    rv = hqos_sem_create_lif(BCM_VLAN_PORT_MATCH_PORT, 1, 0, &lif_gport1);
    if (rv != BCM_E_NONE) {
        printf("Error in hqos_sem_create_lif for BCM_VLAN_PORT_MATCH_PORT with local port!\n");
        return rv;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, 2);
    rv = hqos_sem_create_lif(BCM_VLAN_PORT_MATCH_PORT, sysport_gport, 0, &lif_gport2);
    if (rv != BCM_E_NONE) {
        printf("Error in hqos_sem_create_lif for BCM_VLAN_PORT_MATCH_PORT with sys port2!\n");
        return rv;
    }

    /*Not support for it*/
    rv = hqos_sem_match_delete(BCM_PORT_MATCH_PORT, sysport_gport, 0, lif_gport2);
    if (rv != BCM_E_UNAVAIL) {
        printf("Error in hqos_sem_match_delete for BCM_VLAN_PORT_MATCH_PORT with sys port2!\n");
        return rv;
    }

    rv = bcm_vlan_port_destroy(0, lif_gport2);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_destroy for BCM_VLAN_PORT_MATCH_PORT with sys port2!\n");
        return rv;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, 1);
    rv = hqos_sem_create_lif(BCM_VLAN_PORT_MATCH_NONE, sysport_gport, 0, &lif_gport1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_destroy for BCM_VLAN_PORT_MATCH_NONE with sys port1!\n");
        return rv;
    }

    rv = hqos_sem_match_add(BCM_PORT_MATCH_PORT, sysport_gport, 0, lif_gport1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_destroy for BCM_VLAN_PORT_MATCH_NONE with sys port1!\n");
        return rv;
    }

    rv = hqos_sem_match_delete(BCM_PORT_MATCH_PORT, sysport_gport, 0, lif_gport1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_destroy for BCM_VLAN_PORT_MATCH_NONE with sys port1!\n");
        return rv;
    }

    rv = bcm_vlan_port_destroy(0, lif_gport1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_destroy for BCM_VLAN_PORT_MATCH_NONE with sys port1!\n");
        return rv;
    }

    return rv;
}

