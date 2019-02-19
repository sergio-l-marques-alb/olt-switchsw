/* 
 * Cint file to configure ingress chip to support connecting with NP
 */

/*
 * 1. SOC config:
 *        custom_feature_conn_to_np_enable=1
 *        custom_feature_conn_to_np_debug=0
 *        bcm886xx_vlan_translate_mode=1
 *        ftmh_dsp_extension_add.BCM88675=1
 *        system_ftmh_load_balancing_ext_mode.BCM88675=ENABLED
 *        field_class_id_size_0.BCM88675=8
 *        field_class_id_size_2.BCM88675=24
 *
 * 2. Calling sequence:
 *        2.1 Create LIF
 *
 *        2.2 Add Data-format entry for ILKN port
 *
 *        2.3 PMF configuration to extract LIF-extension to user header
 *
 *
 *
 *
 *
 *
 * To Activate Above Settings Run:
 *      cint ../../../../src/examples/dpp/utility/cint_utils_global.c 
 *      cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
 *      cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c 
 *      cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c 
 *      cint ../../../../src/examples/dpp/utility/cint_utils_mpls.c  
 *      cint ../../../../src/examples/dpp/utility/cint_utils_mpls_port.c 
 *      cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c  
 *      cint ../../../../src/examples/dpp/cint_port_tpid.c 
 *      cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c 
 *      cint ../../../../src/examples/dpp/cint_vswitch_cross_connect_p2p.c  
 *      cint ../../../../src/examples/dpp/cint_connect_to_np_vpws.c
 *
 *      service_port_init(13,13,14);
 *      print config_port_ingress(0, ac_port);
 *
 *      print config_ivec_value_mapping(0,0);
 *      print config_ivec_value_mapping(0,1);
 *      print config_ivec_value_mapping(0,2);
 *      print config_ivec_value_mapping(0,3);
 *
 *      print config_ingress_vpws(0,0);
 *      print config_vlan_status_on_lif(0,vlan_port.vlan_port_id,3);
 *      print config_ingress_vpws(0,1);
 *      print config_extract_lif_auxiliary_data(0);
 *
 *
 *
 *
 */

int ac_port=13;
int pw_port=13;
int ilk_port=14;



/*
 * Setting for IVEC to carry vlan tag number
 *
 */
int outer_tpid = 0x8100;
int inner_tpid = 0x9100;
uint16 no_tag = 0;
uint16 c_tag = 1;
uint16 s_tag = 2;
uint16 prio_tag = 3;
uint16 i_tag = 4;
uint16 c_c_tag = 5;
uint16 s_c_tag = 6;
uint16 prio_c_tag = 7;
uint16 c_s_tag = 9;
uint16 s_s_tag = 10;
uint16 prio_s_tag = 11;

int tpid_profile_tag_format_init(int unit, bcm_port_t port)
{
    int rv = 0;
    int index;
    bcm_port_tpid_class_t port_tpid_class;
        
    rv = bcm_port_tpid_delete_all(unit,port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_delete_all $rv\n");
        return rv;
    }

    /* identify TPID on port */
    rv = bcm_port_tpid_add(unit, port, outer_tpid,0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_add $rv\n");
        return rv;
    }
    
    /* identify TPID on port */
    rv = bcm_port_inner_tpid_set(unit, port, inner_tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_inner_tpid_set $rv\n");
        return rv;
    }
    
    /* no tag */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = no_tag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
        return rv;
    }
    /*s-tag*/
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = outer_tpid;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = s_tag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
        return rv;
    }
    
    /*c-tag*/
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = inner_tpid;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = c_tag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
        return rv;
    }
    
    /*s_c-tag*/
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = outer_tpid;
    port_tpid_class.tpid2 = inner_tpid;
    port_tpid_class.tag_format_class_id = s_c_tag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
        return rv;
    }
    
    /*c_c-tag*/
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = inner_tpid;
    port_tpid_class.tpid2 = inner_tpid;
    port_tpid_class.tag_format_class_id = c_c_tag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set s-c-tag $rv\n");
        return rv;
    }
    return rv;
}


int config_ivec_value_mapping(int unit, uint32 vlan_edit_profile)
{
    int i;
    bcm_vlan_translate_action_class_t action_class;
    uint8 ivec_value;
    int rv = 0;

    bcm_vlan_translate_action_class_t_init(&action_class);
    
    for (i = 0; i <= 10; i++) {
        action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
        action_class.vlan_edit_class_id = vlan_edit_profile;
        action_class.tag_format_class_id = i;        /* tag_format */
        if ((i==2) || (i==1)) {
            ivec_value=vlan_edit_profile & 0x3 | 1<<2;
        } else if (i==0) {
            ivec_value=vlan_edit_profile & 0x3;
        } else {
            ivec_value=vlan_edit_profile & 0x3 | 3<<2;
        }
        ivec_value = ivec_value | 0x30;
        printf("IVEC value is %#x\n", ivec_value);
        action_class.vlan_translation_action_id = ivec_value;

        rv = bcm_vlan_translate_action_class_set(unit, &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set: $rv\n");
            return rv;
        }

    }

    return rv;
}

/*
 * config VLAN_STATUS on a LIF
 */
int config_vlan_status_on_lif(int unit, bcm_gport_t g_port, uint32 vlan_status)
{
    int rv = 0;
    /* set vlan-edit-profile for created lif */
    bcm_vlan_port_translation_t translation;
    
    translation.gport = g_port;
    translation.flags=BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_port_translation_get(unit, &translation);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_get: $rv\n");
        return rv;
    }

    translation.vlan_edit_class_id=vlan_status;
    rv = bcm_vlan_port_translation_set(unit, &translation);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set: $rv\n");
        return rv;
    }
    printf("Config vlan_status on LIF[%#x] successfully\n", g_port);
    return rv;
}

int config_port_ingress(int unit, bcm_port_t port)
{
    int rv = 0;

    rv = bcm_port_class_set(unit, port, bcmPortClassId, port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set: $rv\n");
        return rv;
    }

    rv = tpid_profile_tag_format_init(unit, port);
    if (rv != BCM_E_NONE) {
        printf("Error, tpid_profile_tag_format_init: $rv\n");
        return rv;
    }
    return rv;
}


/*
 * 
 * 1. config vlan status on one lif which create on this port
 *      g_port: lif gport
 */
int config_ivec_ingress(int unit, bcm_gport_t g_port, uint32 vlan_status)
{
    int rv = 0;
    
    rv = config_vlan_status_on_lif(unit, g_port, vlan_status);
    if (rv != BCM_E_NONE) {
        printf("Error, config_vlan_status_on_lif: $rv\n");
        return rv;
    }


    /* if vlan_status vlaues are same on different lifs, configure once with config_ivec_value_mapping() */
    rv = config_ivec_value_mapping(unit, vlan_status);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ivec_value_mapping: $rv\n");
        return rv;
    }

    printf("config_ivec_ingress sucessfully!\n");
    return rv;
}


int ac_create(int unit, bcm_vlan_port_t *vlan_port, int port, uint16 vlan)
{
    int rv = 0;
    uint64 data_udh;

    bcm_vlan_port_t_init(vlan_port);
    vlan_port->flags |= BCM_VLAN_PORT_INGRESS_WIDE | BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE;
    vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port->port = port;
    vlan_port->match_vlan = vlan;
    vlan_port->egress_vlan = 3;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }

    /* write auxiliary data to lif extension */
    COMPILER_64_SET(data_udh,0x12345678, 0x9abcdef1);
    rv = bcm_port_wide_data_set(unit, vlan_port->vlan_port_id, BCM_PORT_WIDE_DATA_INGRESS, data_udh);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }
    return rv;
}

int pw_create(int unit, bcm_mpls_port_t *mpls_port, int port, int vsi)
{
    int rv = 0;
    uint64 data_udh;
    
    bcm_mpls_port_t_init(mpls_port);
    cross_connect_info_init(ac_port, 0, port, 0);

    /*mpls_port->flags2 |= BCM_MPLS_PORT2_INGRESS_WIDE;
        
    rv = mpls_port_create(unit,pw_port,mpls_port,0,0,0);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port_create: $rv\n");
        return rv;
    }
    */

    int egress_intf;
    int in_vc_label = cross_connect_info.in_vc_label_base++;
    int eg_vc_label = cross_connect_info.eg_vc_label_base++;
    
    rv = mpls_tunnels_config(unit, port, &egress_intf, in_vc_label, eg_vc_label);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_tunnels_config\n");
        return rv;
    }
    
    /* add port, according to VC label */
    bcm_mpls_port_t_init(mpls_port);
  
    /* set port attribures */
    mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port->match_label = in_vc_label;
    mpls_port->flags2= BCM_MPLS_PORT2_INGRESS_WIDE;
    mpls_port->egress_tunnel_if = egress_intf;    
    mpls_port->flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port->port = port;
    mpls_port->egress_label.label = eg_vc_label;    
    mpls_port->failover_id = 0;
    mpls_port->failover_port_id = 0;
    mpls_port->failover_mc_group = 0;
    
    rv = bcm_mpls_port_add(unit, vsi, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }
        
    printf("pw created...\n");
    /* write auxiliary data to lif extension */        
    COMPILER_64_SET(data_udh,0x12345678, 0x9abcdef1);
    rv = bcm_port_wide_data_set(unit, mpls_port->mpls_port_id, BCM_PORT_WIDE_DATA_INGRESS, data_udh);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }
    printf("pw write auxiliary data...\n");
    return rv;
}

uint32 g_port_out=13, g_flow_id=0x123, g_slot_out=0, g_out_fp=0x321, g_ps=0, g_main_type=1, g_sub_type=0;

void data_entry_init(uint32 port_out, uint32 flow_id, uint32 slot_out, uint32 out_fp, uint32 ps, uint32 main_type, uint32 sub_type)
{
    g_port_out=port_out;
    g_flow_id=flow_id;
    g_slot_out=slot_out;
    g_out_fp=out_fp;
    g_ps=ps;
    g_main_type=main_type;
    g_sub_type=sub_type;
}


void fill_data_format_entry(uint32 *data, uint32 port_out, uint32 flow_id, uint32 slot_out, uint32 out_fp, uint32 ps, uint32 main_type, uint32 sub_type)
{
    data[0] = (sub_type&0x3) | ((main_type&0x7) << 2) | ((ps&0x7) << 5) | ((out_fp&0x1ffff) << 8) | ((slot_out&0x7f) << 25);
    data[1] = (flow_id&0xffff) | ((port_out&0xff) << 16);
}

int data_format_entry_create(int unit, bcm_gport_t *global_lif_id)
{
    uint32 data[2];
    int rv = 0;
    
    bcm_tunnel_initiator_t virtual_tunnel;
    bcm_tunnel_initiator_t_init(&virtual_tunnel);

    virtual_tunnel.type = bcmTunnelTypeCount;

    /* fill information into data-entry */
    fill_data_format_entry(data, g_port_out, g_flow_id, g_slot_out, g_out_fp, g_ps, g_main_type, g_sub_type);

    virtual_tunnel.aux_data = data[0];
    virtual_tunnel.flow_label = data[1];

    rv = bcm_tunnel_initiator_create(unit, NULL, &virtual_tunnel);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create: $rv\n");
        return rv;
    }

    *global_lif_id = virtual_tunnel.tunnel_id;
    
    return rv;

        
}


int config_extract_lif_auxiliary_data(int unit)
{
    int rv = 0;
    
    /* config for user header */
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_extraction_field_t ext;
    bcm_field_extraction_action_t extact;

    bcm_field_data_qualifier_t lif_extension;
    bcm_field_data_qualifier_t_init(&lif_extension);
    lif_extension.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    lif_extension.offset = 0;
    lif_extension.length = 16;
    lif_extension.qualifier = bcmFieldQualifyInVPortWide;
    rv = bcm_field_data_qualifier_create(unit, &lif_extension);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_data_qualifier_create: $rv\n");
        return rv;
    }


    bcm_field_group_config_t_init(&grp);
    bcm_field_extraction_action_t_init(&extact);
    bcm_field_extraction_field_t_init(&ext);

    grp.group = -1;

    BCM_FIELD_QSET_INIT(grp.qset);
    rv = bcm_field_qset_data_qualifier_add(unit, grp.qset, lif_extension.qual_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_qset_data_qualifier_add: $rv\n");
        return rv;
    }


    grp.priority = 15;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeDirectExtraction;

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassDestSet);

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_config_create: $rv\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_action_set: $rv\n");
        return rv;
    }


    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_entry_create: $rv\n");
        return rv;
    }

    extact.action = bcmFieldActionClassDestSet;
    ext.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext.bits = 16;
    ext.lsb = 0;
    ext.qualifier = lif_extension.qual_id;
    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 1, &ext);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_direct_extraction_action_add: $rv\n");
        return rv;
    }

    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_install: $rv\n");
        return rv;
    }

    return rv;
}


int cross_connect_lif_to_ilk_direction(int unit, bcm_gport_t lif_gport, int ilkn_port, bcm_gport_t virtual_tunnel)
{
    int rv;
    bcm_vswitch_cross_connect_t gports;
    bcm_gport_t gport_ilk;
    
    BCM_GPORT_LOCAL_SET(gport_ilk, ilkn_port);
    gports.port1 = lif_gport;
    gports.port2 = gport_ilk;
    gports.encap1= BCM_FORWARD_ENCAP_ID_INVALID;
    gports.encap2= virtual_tunnel;
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add: $rv\n");
        return rv;
    }
    return rv;
}

void service_port_init(int t_ac_port, int t_pw_port, int t_ilk_port)
{
    ac_port = t_ac_port;
    pw_port = t_pw_port;
    ilk_port = t_ilk_port;
}

bcm_vlan_port_t vlan_port;
bcm_mpls_port_t mpls_port;

/* create lif cross-connect to ILKN port */
int config_ingress_vpws(int unit, int lif_type)
{
    int rv = 0;

    bcm_gport_t g_virtual_tunnel_lif_id;

    if (lif_type == 0) {        /* lif is AC */
        rv = ac_create(unit, &vlan_port, ac_port, 20);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_create: $rv\n");
            return rv;
        }
        printf("AC create sucessfully!\n");
        rv = data_format_entry_create(unit, &g_virtual_tunnel_lif_id);
        if (rv != BCM_E_NONE) {
            printf("Error, data_format_entry_create: $rv\n");
            return rv;
        }
        printf("data-entry create successfully\n");
        rv = cross_connect_lif_to_ilk_direction(unit, vlan_port.vlan_port_id, ilk_port, g_virtual_tunnel_lif_id);
        if (rv != BCM_E_NONE) {
            printf("Error, cross_connect_lif_to_ilk_bidirection: $rv\n");
            return rv;
        }
        printf("AC configure sucessfully!\n");
    } else if (lif_type == 1) {        /* lif is PW */
        rv = pw_create(unit, &mpls_port, pw_port, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_create: $rv\n");
            return rv;
        }

        rv = data_format_entry_create(unit, &g_virtual_tunnel_lif_id);
        if (rv != BCM_E_NONE) {
            printf("Error, data_format_entry_create: $rv\n");
            return rv;
        }

        rv = cross_connect_lif_to_ilk_direction(unit, mpls_port.mpls_port_id, ilk_port, g_virtual_tunnel_lif_id);
        if (rv != BCM_E_NONE) {
            printf("Error, cross_connect_lif_to_ilk_bidirection: $rv\n");
            return rv;
        }
        printf("pw create sucessfully!\n");

                
    } else {        /* wrong lif type */
        printf("Wrong input of lif type[%d]\n", lif_type);
    }
    return rv;
}


int config_ingress_vpws_test(int unit)
{
    int pwe_ps=7;
    int pwe_output_fp=0x1000;
    int ac_ps=4;
    int ac_output_fp=0x2000;

    int rv;
    int i;
    
    printf("ac_port[%d], pw_port[%d], ilk_port[%d]\n", ac_port, pw_port, ilk_port);

    rv = config_port_ingress(unit, ac_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_port_ingress: $rv\n");
        return rv;
    }

    for (i = 0; i < 4; i++) {
        rv = config_ivec_value_mapping(unit, i);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ivec_value_mapping[%d]\n",i);
            return rv;
        }
    }

    
    data_entry_init(13, 0x1001, 0, pwe_output_fp, pwe_ps, 1, 0);
    printf("Create AC for VPWS\n");
    rv = config_ingress_vpws(0,0);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ingress_vpws-ac: $rv\n");
        return rv;
    }
    printf("Config Vlan_status[2] on AC\n");
    rv = config_vlan_status_on_lif(unit,vlan_port.vlan_port_id,2);
    if (rv != BCM_E_NONE) {
        printf("Error, config_vlan_status_on_lif: $rv\n");
        return rv;
    }

    
    data_entry_init(13, 0x1001, 0, ac_output_fp, ac_ps, 0, 0);
    printf("Create AC for VPWS\n");
    rv = config_ingress_vpws(unit,1);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ingress_vpws-pw: $rv\n");
        return rv;
    }

    rv = config_extract_lif_auxiliary_data(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, config_extract_lif_auxiliary_data: $rv\n");
        return rv;
    }

    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port: $rv\n");
        return rv;
    }

    return rv;
}


/*
 *====================================================================================================
 *------------------------------------VPLS cint-------------------------------------------------------
 *
 *
 *
 *====================================================================================================
 */

bcm_mac_t dest_mac = {0x00,0x00,0x00,0x00,0x00,0x12};
bcm_mac_t dest_mac1 = {0x04,0x05,0x06,0x07,0x08,0x09};

int l2_addr_add_point_to_data_entry(int unit, bcm_mac_t mac, uint16 vid, bcm_gport_t port, int encap_id) {
    int rv;
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.port = port;
    l2addr.vid = vid;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.encap_id = encap_id;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }
    return BCM_E_NONE;
}

int config_ilk_port(int unit, bcm_gport_t ilk_port)
{
    int rv;
    bcm_port_extender_mapping_info_t mapping_info;
    
    mapping_info.user_define_value=0;
    mapping_info.pp_port=ilk_port;
    mapping_info.phy_port=ilk_port;

    rv = bcm_port_extender_mapping_info_set(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_extender_mapping_info_set\n");
        return rv;
    }

    return rv;
}


/* create AC and PW and added to VSI */
int config_ingress_vpls_uc(int unit)
{
    bcm_gport_t global_lif_id_ac, global_lif_id_pw;
    int pwe_ps=7;
    int pwe_output_fp=0x1000;
    int ac_ps=3;
    int ac_output_fp=0x2000;
    int vsi=30;
    int rv;
    int i;

    printf("ac_port[%d], pw_port[%d], ilk_port[%d]\n", ac_port, pw_port, ilk_port);
    rv = config_port_ingress(unit, ac_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_port_ingress\n");
        return rv;
    }

    for (i = 0; i < 4; i++) {
        rv = config_ivec_value_mapping(unit,i);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ivec_value_mapping[%d]\n",i);
            return rv;
        }
    }

    printf("Create VPN...\n");
    rv = bcm_vswitch_create_with_id(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create_with_id\n");
        return rv;
    }

    printf("Create LIF...\n");
    rv = ac_create(unit, &vlan_port, ac_port, 20);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_create\n");
        return rv;
    }

    rv = pw_create(unit, &mpls_port, pw_port, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, pw_create\n");
        return rv;
    }

    /* for pw */
    data_entry_init(13, 0x1001, 0, ac_output_fp, ac_ps, 1, 1);
    rv = data_format_entry_create(unit, &global_lif_id_pw);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for pw failed\n");
        return rv;
    }
    rv = l2_addr_add_point_to_data_entry(unit,dest_mac1,vsi,ilk_port,global_lif_id_pw);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add_point_to_data_entry for ac failed\n");
        return rv;
    }
    /* for ac */
    data_entry_init(13, 0x1000, 0, pwe_output_fp, pwe_ps, 1, 1);
    rv = data_format_entry_create(unit, &global_lif_id_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for ac failed\n");
        return rv;
    }
    rv = l2_addr_add_point_to_data_entry(unit,dest_mac,vsi,ilk_port,global_lif_id_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add_point_to_data_entry for ac failed\n");
        return rv;
    }
    
    printf("Add AC LIF to VSI...\n");
    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    
    printf("Config VlanStatus on AC...\n");
    rv = config_vlan_status_on_lif(unit,vlan_port.vlan_port_id,1);
    if (rv != BCM_E_NONE) {
        printf("Error, config_vlan_status_on_lif\n");
        return rv;
    }

    rv = config_extract_lif_auxiliary_data(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, config_extract_lif_auxiliary_data\n");
        return rv;
    }
    
    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }

    printf("Config ingress of VPLS UC successfully\n");
    return rv;

}


bcm_mac_t mc_mac = {0x01,0x00,0x5e,0x00,0x00,0x01};

int l2_addr_add_multicast(int unit, bcm_mac_t mac, uint16 vid, bcm_multicast_t mc_id, int encap_id) {
    int rv;
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, mac, vid);
    
    l2addr.vid = vid;
    l2addr.encap_id = encap_id;
    l2addr.l2mc_group = mc_id;
    l2addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;


    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }
    return BCM_E_NONE;
}


/**
 *  config VPLS MC with two cases:
 *      1. flood in the VSI
 *      2. Create Multicast group in the VSI
 */
int config_ingress_vpls_mc(int unit)
{
    bcm_gport_t global_lif_id_ac, global_lif_id_pw;
    int pwe_ps=7;
    int pwe_output_fp=0x1000;
    int ac_ps=3;
    int ac_output_fp=0x2000;
    int vsi=30;
    int rv;
    int i;

    int flag;
    int cud;
    bcm_gport_t dest_gport; 
    bcm_gport_t global_lif_id_mc;
    bcm_vlan_control_vlan_t vsi_control;

    bcm_multicast_t mc_id_reserve=6001;
    bcm_mac_t mc_mac = {0x01,0x00,0x5e,0x00,0x00,0x01};
    
    /* mc_id should < 4096 for flood application */
    bcm_multicast_t mc_id=vsi;

    printf("ac_port[%d], pw_port[%d], ilk_port[%d]\n", ac_port, pw_port, ilk_port);
    rv = config_port_ingress(unit, ac_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_port_ingress\n");
        return rv;
    }

    for (i = 0; i < 4; i++) {
        rv = config_ivec_value_mapping(unit,i);
        if (rv != BCM_E_NONE) {
            printf("Error, config_ivec_value_mapping[%d]\n",i);
            return rv;
        }
    }

    printf("Create VPN...\n");
    rv = bcm_vswitch_create_with_id(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create_with_id\n");
        return rv;
    }

    printf("Create LIF...\n");
    rv = ac_create(unit, &vlan_port, ac_port, 20);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_create\n");
        return rv;
    }

    rv = pw_create(unit, &mpls_port, pw_port, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, pw_create\n");
        return rv;
    }

    printf("Add AC LIF to VSI...\n");
    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    /* for pw */
    data_entry_init(13, 0x1001, 0, ac_output_fp, ac_ps, 1, 1);
    rv = data_format_entry_create(unit, &global_lif_id_pw);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for pw failed\n");
        return rv;
    }
    rv = l2_addr_add_point_to_data_entry(unit,dest_mac1,vsi,ilk_port,global_lif_id_pw);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add_point_to_data_entry for ac failed\n");
        return rv;
    }
    /* for ac */
    data_entry_init(13, 0x1000, 0, pwe_output_fp, pwe_ps, 1, 1);
    rv = data_format_entry_create(unit, &global_lif_id_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for ac failed\n");
        return rv;
    }
    rv = l2_addr_add_point_to_data_entry(unit,dest_mac,vsi,ilk_port,global_lif_id_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add_point_to_data_entry for ac failed\n");
        return rv;
    }

    printf("Config VlanStatus on AC...\n");
    rv = config_vlan_status_on_lif(unit,vlan_port.vlan_port_id,1);
    if (rv != BCM_E_NONE) {
        printf("Error, config_vlan_status_on_lif\n");
        return rv;
    }
    
    rv = config_extract_lif_auxiliary_data(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, config_extract_lif_auxiliary_data\n");
        return rv;
    }
    
    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }


    /*
     * config for VPN flood
     */
    printf("Create flood group...\n");
    flag=BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP;
    rv = bcm_multicast_create(unit,flag,&mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create\n");
        return rv;
    }

    bcm_vlan_control_vlan_t_init(&vsi_control);
    vsi_control.forwarding_vlan = vsi;
    vsi_control.unknown_unicast_group   = mc_id;
    vsi_control.unknown_multicast_group = mc_id;
    vsi_control.broadcast_group         = vsi;
    rv = bcm_vlan_control_vlan_set(unit,vsi,vsi_control);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_control_vlan_set\n");
        return rv;
    }

    data_entry_init(13, 0x1000, 0, mc_id, 7, 2, 1);
    rv = data_format_entry_create(unit, &global_lif_id_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create for flood\n");
        return rv;
    }

    print global_lif_id_mc;

    cud=global_lif_id_mc;
    BCM_GPORT_LOCAL_SET(dest_gport,ilk_port);
    rv = bcm_multicast_ingress_add(unit,mc_id,dest_gport,cud);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    /*
     * config for VPLS multicast group in VPN
     */

    /* 1. create one mc group to add ilk port as destination, containing only one copy */
    mc_id=6000;    
    flag=BCM_MULTICAST_TYPE_L2|BCM_MULTICAST_WITH_ID|BCM_MULTICAST_INGRESS_GROUP;
    print bcm_multicast_create(unit,flag,&mc_id_reserve);

    data_entry_init(13, 0x1000, 0, mc_id, 7, 2, 1);
    print data_format_entry_create(unit, &global_lif_id_mc);
    print global_lif_id_mc;

    print l2_addr_add_multicast(unit,mc_mac,vsi,mc_id_reserve,global_lif_id_mc);

    cud=global_lif_id_mc;
    BCM_GPORT_LOCAL_SET(dest_gport,ilk_port);
    print bcm_multicast_ingress_add(unit,mc_id_reserve,dest_gport,cud);

    /* 2. create real mc group */

    printf("Config ingress of VPLS MC successfully\n");
    return rv;


}

/*
 *====================================================================================================
 *------------------------------------MPLS cint-------------------------------------------------------
 * SOC config on ingress side
 *
 * custom_feature_conn_to_np_enable=1
 * custom_feature_conn_to_np_debug=0
 * bcm886xx_vlan_translate_mode=1
 * ftmh_dsp_extension_add.BCM88675=1
 * system_ftmh_load_balancing_ext_mode.BCM88675=ENABLED
 * field_class_id_size_0.BCM88675=8
 * field_class_id_size_2.BCM88675=24
 * custom_feature_injection_with_user_header_enable=1
 * custom_feature_vendor_custom_pp_port_14=1
 * custom_feature_vendor_customer65=1
 * custom_feature_disable_xtmh=0
 *------------------------------------------------------------------------
 * Add soc config for CPU inject packet: custom_feature_injection_with_user_header_enable=1
 *
 *
 * MPLS SWAP
 * Files should cint:
 *  cd ../../../../src/examples/dpp
 *  cint utility/cint_utils_global.c
 *  cint utility/cint_utils_mpls.c
 *  cint cint_qos.c
 *  cint cint_multi_device_utils.c
 *  cint utility/cint_utils_l2.c
 *  cint utility/cint_utils_l3.c
 *  cint cint_mpls_lsr.c
 *  
 *  cint cint_connect_to_np_vpws.c
 *  cint cint_nph_egress.c
 *  cint
 *
 *  print config_ingress_mpls_push(0);
 *   
 *====================================================================================================
 */

int out_port_mpls = 13;
int in_port_mpls = 13;

unsigned char cpu_port_to_sent_oam=203;

/* ptch + ITMH.base + PPH.base + user header */
/*
 * cpu_pkt[9]=0xA5: indicate this MPLS packet is injected from CPU port.
 */
unsigned char cpu_pkt[] = 
   {0x70, 0xc8, \   /*ptch*/
    0x62, 0x00, 0x00, 0x02, \ /*ITMH.base*/
    0x05, 0x00, 0x00, 0xA5, 0x00, 0x00, 0x00, \ /*PPH.base*/
    0x12, 0x34, 0x56, 0x78, \   /* user header */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x0A, 0x00, 0x02, 0x00, 0x81, 0x00, 0x00, 0x1E, \
    0x88, 0x47, 0x00, 0x19, 0x02, 0x0A, 0x00, 0x3E, 0x85, 0x0B, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, \
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, \
    0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, \
    0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, \
    0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45};

/* ptch + user header */
unsigned char cpu_pkt_1[] = 
   {0x80, 0xc8, \
    0x12, 0x34, 0x56, 0x78, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x0A, 0x00, 0x02, 0x00, 0x81, 0x00, 0x00, 0x1E, \
    0x88, 0x47, 0x00, 0x19, 0x02, 0x0A, 0x00, 0x3E, 0x85, 0x0B, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, \
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, \
    0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, \
    0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, \
    0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45};


int hw_cpu_tx_packet(int unit, int pkt_len)
{
    int rc;
    int pkt_flags;

    bcm_pkt_t* tx_pkt;
    pkt_flags = BCM_TX_CRC_ALLOC;
    rc = bcm_pkt_alloc(unit, pkt_len, pkt_flags, &tx_pkt);
    if (rc != BCM_E_NONE) {
        printf("error, bcm_pkt_alloc\n");
        return rc;
    }
    sal_memset(tx_pkt->_pkt_data.data, 0, pkt_len);
    tx_pkt->pkt_data = &tx_pkt->_pkt_data;

    sal_memcpy(tx_pkt->_pkt_data.data, cpu_pkt, pkt_len);

    rc = bcm_tx(unit, tx_pkt, NULL);
    if (rc != BCM_E_NONE) { 
        printf("error, bcm_tx\n");
        return rc;
    }

    rc = bcm_pkt_free(0, tx_pkt);
    if (rc != BCM_E_NONE) {
        printf("error, bcm_pkt_free\n");
        return rc;
    } 
    return rc;
}


void service_port_init(int t_in_port, int t_out_port, int t_ilk_port)
{
    in_port_mpls = t_in_port;
    out_port_mpls = t_out_port;
    ilk_port = t_ilk_port;
}


int config_ingress_mpls_swap(int unit)
{
    int rv;
    int out_sysport, in_sysport;

    int my_mac_lsb = 0x11;  /* set MAC to 00:00:00:00:00:11 */
    int next_hop_lsb = 0x22; /* set MAC to 00:00:00:00:00:22 */
    int in_label = 1000;
    int out_label  = 2000;
    int in_vid = 10;
    int eg_vid = 10;
    int out_to_tunnel = 1;
    int term_label = 400;
    uint32 next_header_flags = 0; /* indicate next protocol is MPLS */
    int units_ids[1];    
    
    bcm_gport_t global_lif_id;
    create_l3_egress_s l3_egress_fec;

    units_ids[0] = unit;

    rv = port_to_system_port(unit, ilk_port, &out_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for out_port\n");
        return rv;
    }

    rv = port_to_system_port(unit, in_port_mpls, &in_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for in_port\n");
        return rv;
    }

    /* init */
    mpls_lsr_init(in_sysport, out_sysport, my_mac_lsb, next_hop_lsb, in_label, out_label, in_vid, eg_vid, out_to_tunnel);

    /* 1. mpls lsr config */
    rv =  mpls_lsr_config_init(units_ids, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_lsr_config_init\n");
        return rv;
    }

    /* 2.  add mpls switch entry */
    /* 2.1 create date entry */
    data_entry_init(out_port_mpls,g_flow_id,1,g_out_fp,7,1,3);
    rv = data_format_entry_create(unit, &global_lif_id);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Global lif of data_entry: %#x\n",global_lif_id);
    
    /* 2.2 create FEC */
    l3_egress_fec.out_gport = mpls_lsr_info_1.eg_port;
    l3_egress_fec.vlan = mpls_lsr_info_1.eg_vid;
    l3_egress_fec.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;
    l3_egress_fec.arp_encap_id = global_lif_id;
    rv = l3__egress_only_fec__create(unit, &l3_egress_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }
    printf("FEC[%#x] to resolve ilk_port[%d]-outlif[%#x]\n",l3_egress_fec.fec_id, ilk_port, global_lif_id);
    
    /* 2.3 add switch entry */
    rv = mpls_add_switch_entry(unit,mpls_lsr_info_1.in_label, mpls_lsr_info_1.eg_label, l3_egress_fec.fec_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_switch_entry\n");
        return rv;
    }

    /* 3. add mpls termination entry */
    rv = mpls_add_term_entry_multi_device(units_ids,1,term_label,next_header_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_term_entry_multi_device\n");
        return rv;
    }


    /* 4. optional, config when running test on Ingress chip.*/
    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }

    return 0;
}


int config_ingress_mpls_pop(int unit)
{
    int rv;
    int out_sysport, in_sysport;

    int my_mac_lsb = 0x11;  /* set MAC to 00:00:00:00:00:11 */
    int next_hop_lsb = 0x22; /* set MAC to 00:00:00:00:00:22 */
    int in_label = 1000;
    int out_label  = 2000;
    int in_vid = 20;
    int eg_vid = 20;
    int out_to_tunnel = 1;
    int term_label = 400;
    uint32 next_header_flags = 0; /* indicate next protocol is MPLS */
    int units_ids[1];
    int tunnel_id;
    
    bcm_gport_t global_lif_id;
    create_l3_egress_s l3_egress_fec;

    units_ids[0] = unit;

    rv = port_to_system_port(unit, ilk_port, &out_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for out_port\n");
        return rv;
    }

    rv = port_to_system_port(unit, in_port_mpls, &in_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for in_port\n");
        return rv;
    }

    /* init */
    mpls_lsr_init(in_sysport, out_sysport, my_mac_lsb, next_hop_lsb, in_label, out_label, in_vid, eg_vid, out_to_tunnel);

    /* 1. mpls lsr config */
    rv =  mpls_lsr_config_init(units_ids, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_lsr_config_init\n");
        return rv;
    }

    /* 2.  add mpls pop entry */
    /* 2.1 create date entry */
    data_entry_init(out_port_mpls,g_flow_id,1,g_out_fp,7,1,3);
    rv = data_format_entry_create(unit, &global_lif_id);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Global lif of data_entry: %#x\n",global_lif_id);
    
    /* 2.2 create FEC */
    l3_egress_fec.out_gport = mpls_lsr_info_1.eg_port;
    l3_egress_fec.vlan = mpls_lsr_info_1.eg_vid;
    l3_egress_fec.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;
    l3_egress_fec.arp_encap_id = global_lif_id;
    rv = l3__egress_only_fec__create(unit, &l3_egress_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }
    printf("FEC[%#x] to resolve ilk_port[%d]-outlif[%#x]\n",l3_egress_fec.fec_id, ilk_port, global_lif_id);
    
    /* 2.3 add pop entry */
    rv = mpls_add_php_entry(unit, mpls_lsr_info_1.in_label, 0, 0, l3_egress_fec.fec_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_term_entry_info\n");
        return rv;
    }

    /* 3. add mpls termination entry */
    rv = mpls_add_term_entry_multi_device(units_ids,1,term_label,next_header_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_term_entry_multi_device\n");
        return rv;
    }


    /* 4. optional, config when running test on Ingress chip.*/
    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }

    return 0;
}


int config_ingress_mpls_push(int unit)
{
    int rv;
    int out_sysport, in_sysport;

    int my_mac_lsb = 0x11;  /* set MAC to 00:00:00:00:00:11 */
    int next_hop_lsb = 0x22; /* set MAC to 00:00:00:00:00:22 */
    int in_label = 1000;
    int out_label  = 2000;
    int in_vid = 30;
    int eg_vid = 30;
    int out_to_tunnel = 1;
    int term_label = 400;
    uint32 next_header_flags = 0; /* indicate next protocol is MPLS */
    int units_ids[1];    
    
    bcm_gport_t global_lif_id;
    create_l3_egress_s l3_egress_fec;

    units_ids[0] = unit;

    rv = port_to_system_port(unit, ilk_port, &out_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for out_port\n");
        return rv;
    }

    rv = port_to_system_port(unit, in_port_mpls, &in_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for in_port\n");
        return rv;
    }

    /* init */
    mpls_lsr_init(in_sysport, out_sysport, my_mac_lsb, next_hop_lsb, in_label, out_label, in_vid, eg_vid, out_to_tunnel);

    /* 1. mpls lsr config */
    rv =  mpls_lsr_config_init(units_ids, 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_lsr_config_init\n");
        return rv;
    }


    /* 2.  add mpls push entry */
    /* 2.1 create date entry */
    data_entry_init(out_port_mpls,g_flow_id,1,g_out_fp,7,1,3);
    rv = data_format_entry_create(unit, &global_lif_id);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Global lif of data_entry: %#x\n",global_lif_id);

    /* 2.2 create FEC */
    l3_egress_fec.out_gport = mpls_lsr_info_1.eg_port;
    l3_egress_fec.vlan = mpls_lsr_info_1.eg_vid;
    l3_egress_fec.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;
    l3_egress_fec.arp_encap_id = global_lif_id;
    rv = l3__egress_only_fec__create(unit, &l3_egress_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }
    printf("FEC[%#x] to resolve ilk_port[%d]-outlif[%#x]\n",l3_egress_fec.fec_id, ilk_port, global_lif_id);

    /* 2.3 add push entry */
    rv = mpls_add_switch_push_entry(unit, mpls_lsr_info_1.in_label, mpls_lsr_info_1.eg_label, l3_egress_fec.fec_id, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_term_entry_info\n");
        return rv;
    }

    /* 3. add mpls termination entry */
    rv = mpls_add_term_entry_multi_device(units_ids,1,term_label,next_header_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_term_entry_multi_device\n");
        return rv;
    }

    /* 4. optional, config when running test on Ingress chip.*/
    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }

    return 0;

}


/*
 *====================================================================================================
 * Add soc config for CPU inject packet: custom_feature_injection_with_user_header_enable=1
 *
 * Files should cint:
 *  cd ../../../../src/examples/dpp
 *  cint utility/cint_utils_global.c
 *  cint utility/cint_utils_mpls.c
 *  cint cint_qos.c
 *  cint cint_multi_device_utils.c
 *  cint utility/cint_utils_l2.c
 *  cint utility/cint_utils_l3.c
 *  cint cint_mpls_lsr.c
 *  
 *  cint cint_connect_to_np_vpws.c
 *  cint cint_nph_egress.c
 *  cint
 *  print config_ingress_mpls_cpu(0, cpu_port_to_sent_oam);
 *  
 *====================================================================================================
 */

int config_ingress_mpls_cpu(int unit, int cpu_port)
{
    int rv;
    bcm_gport_t global_lif_id;
    create_l3_egress_s l3_egress_fec;
    int out_sysport;

    rv = port_to_system_port(unit, ilk_port, &out_sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for out_port\n");
        return rv;
    }

    data_entry_init(out_port_mpls,g_flow_id,1,g_out_fp,7,1,3);
    rv = data_format_entry_create(unit, &global_lif_id);
    if (rv != BCM_E_NONE) {
        printf("Error, data_format_entry_create\n");
        return rv;
    }
    printf("Global lif of data_entry: %#x\n",global_lif_id);
    /* create FEC */
    l3_egress_fec.out_gport = out_sysport;
    l3_egress_fec.arp_encap_id = global_lif_id;
    
    rv = l3__egress_only_fec__create(unit, &l3_egress_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }
    printf("FEC[%#x] to resolve ilk_port[%d]-outlif[%#x]\n",l3_egress_fec.fec_id, ilk_port, global_lif_id); 


    printf("Config ingress of ilk_port...\n");
    rv = config_ilk_port(unit, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ilk_port\n");
        return rv;
    }


    /* construct cpu packet: OAM over MPLS
     * Carried with PTCH_2 and user define header
     *
     */
    cpu_pkt[1] = cpu_port;

    cpu_pkt[4] = l3_egress_fec.fec_id & 0xff;
    cpu_pkt[3] = (l3_egress_fec.fec_id & 0xff00)>>8;
    cpu_pkt[2] = (cpu_pkt[2]&0xfe) | ((l3_egress_fec.fec_id & 0x10000)>>16);
        

    rv = hw_cpu_tx_packet(unit, 100);
    if (rv != BCM_E_NONE) {
        printf("Error, hw_cpu_tx_packet\n");
        return rv;
    }



    return BCM_E_NONE;
}




/*
 *====================================================================================================
 *------------------------------------Parser header-------------------------------------------------------
 *
 *
 *
 *====================================================================================================
 */

void parse_xtmh_header(uint8 *xtmh)
{
    uint8 tmp;
    uint16 tmp16;
    printf("=========================XTMH=======================\n");
    printf("xtmh.type(2):       %#x\n", (xtmh[0]&0xc0)>>6);
    printf("xtmh.P_T(2):        %#x\n", (xtmh[0]&0x30)>>4);
    tmp = (xtmh[0]&0xf)<<2 | (xtmh[1]&0xc0)>>6; 
    printf("xtmh.PktHeadLen(6): %#x\n", tmp);
    printf("xtmh.DP(3):         %#x\n", (xtmh[1]&0x38)>>3);
    printf("xtmh.RSV(3):        %#x\n", (xtmh[1]&0x07));
    tmp16 = xtmh[2];
    tmp16 = tmp16<<8 | xtmh[3];
    printf("xtmh.Flow_ID(16):   %#x\n", tmp16);
    printf("====================================================\n");
}

void parse_nph_header(uint8 *nph, int type)
{
    uint16 tmp16;
    uint32 tmp32;
    
    printf("==========================NPH=======================\n");
    printf("NPH.M/U(1):         %#x\n", (nph[0]&0x80)>>7);
    printf("NPH.Main_type(3):   %#x\n", (nph[0]&0x70)>>4);
    printf("NPH.Sub_type(4):    %#x\n", (nph[0]&0x0f));

    printf("NPH.MulPriority(2): %#x\n", (nph[1]&0xc0)>>6);
    printf("NPH.PTP(1):         %#x\n", (nph[1]&0x20)>>5);
    printf("NPH.CoS(3):         %#x\n", (nph[1]&0x1c)>>2);
    printf("NPH.Color(2):       %#x\n", (nph[1]&0x03));

    tmp16 = nph[2];
    tmp16 = tmp16<<8 | nph[3];
    printf("NPH.Slot_in(16):    %#x\n", tmp16);

    printf("NPH.VlanStatus(2):  %#x\n", (nph[4]&0xc0)>>6);
    printf("NPH.Del_Len(6):     %#x\n", (nph[4]&0x3f));

    printf("NPH.DFT_PRI(4):     %#x\n", (nph[5]&0xf0)>>4);
    printf("NPH.PS(3):          %#x\n", (nph[5]&0x0e)>>1);

    if (type == 3) {
        tmp32 = (nph[5]&0x01)<<16 | (nph[6]<<8) | nph[7];
        printf("NPH.MC_FP(17):      %#x\n", tmp32);
    } else {
        tmp32 = (nph[5]&0x01)<<16 | (nph[6]<<8) | nph[7];
        printf("NPH.Output_FP(17):  %#x\n", tmp32);
    }
    
    printf("NPH.R/L(1):         %#x\n", (nph[8]&0x80)>>7);
    printf("NPH.H/S(1):         %#x\n", (nph[8]&0x40)>>6);
    printf("NPH.Tag_num(2):     %#x\n", (nph[8]&0x30)>>4);

    if (type == 3) {
        tmp16 = (nph[8]&0x0f)<<4 | (nph[9]&0xf0)>>4;
        printf("NPH.RSV(8):         %#x\n", tmp16);        
        printf("NPH.Hash_value(4):  %#x\n", (nph[9]&0x0f));
    } else {
        printf("NPH.B_F(1):         %#x\n", (nph[8]&0x08)>>3);
        tmp16 = (nph[8]&0x07)<<8 | nph[9];
        printf("NPH.Port_Out(11):   %#x\n", tmp16);

    }
    printf("NPH.Learn_on(1):    %#x\n", (nph[10]&0x80)>>7);
    printf("NPH.Learn_stat(1):  %#x\n", (nph[10]&0x40)>>6);
    
    if (type == 3) {
        printf("RSV(1):     %#x\n", (nph[10]&0x20)>>5);
    } else {
        printf("NPH.MSP_OPO(1):     %#x\n", (nph[10]&0x20)>>5);
    }
    /*vpws*/
    if (type == 0) {        
        tmp16 = (nph[10]&0x1f)<<8 | nph[11];
        printf("NPH.Ulei_Port(13):  %#x\n", tmp16);
        
        printf("NPH.RSV(1):         %#x\n", (nph[12]&0x80)>>7);
        printf("NPH.Slot_Out(7):    %#x\n", (nph[12]&0x7f));
        printf("NPH.RSV(1):         %#x\n", (nph[13]&0x80)>>7);
        printf("NPH.DS_Mode(2):     %#x\n", (nph[13]&0x60)>>5);
        printf("NPH.RSV(5):         %#x\n", (nph[13]&0x1f));
        tmp16 = nph[14]<<8 | nph[15];
        printf("NPH.NH_inde(16):    %#x\n", tmp16);        
        
    } else if (type == 1) {/*vpls*/
        tmp16 = (nph[10]&0x1f)<<8 | nph[11];
        printf("NPH.VPN_ID(13):     %#x\n", tmp16);

        printf("NPH.RSV(1):         %#x\n", (nph[12]&0x80)>>7);
        printf("NPH.Slot_Out(7):    %#x\n", (nph[12]&0x7f));
        printf("NPH.RSV(1):         %#x\n", (nph[13]&0x80)>>7);
        printf("NPH.DS_Mode(2):     %#x\n", (nph[13]&0x60)>>5);
        printf("NPH.RSV(4):         %#x\n", (nph[13]&0x1e)>>1);

        tmp32 = (nph[13]&0x01)<<16 | nph[14]<<8 | nph[15];
        printf("NPH.Src_FP(17):     %#x\n", tmp32);
    } else if (type == 3) {/*vpls MC*/
        tmp16 = (nph[10]&0x1f)<<8 | nph[11];
        printf("NPH.VPN_ID(13):     %#x\n", tmp16);
        
        tmp16 = nph[12]<<1 | (nph[13]&0x80)>>7;
        printf("NPH.RSV(9):         %#x\n", tmp16);
        printf("NPH.DS(2):          %#x\n", (nph[13]&0x60)>>5);
        printf("NPH.MAC_pri(2):     %#x\n", (nph[13]&0x18)>>3);
        printf("NPH.IGMP(1):        %#x\n", (nph[13]&0x04)>>2);
        printf("NPH.R/M(1):         %#x\n", (nph[13]&0x02)>>1);
        tmp32 = (nph[13]&0x01)<<16 | nph[14]<<8 | nph[15];
        printf("NPH.Src_FP(17):     %#x\n", tmp32);
    }
}


