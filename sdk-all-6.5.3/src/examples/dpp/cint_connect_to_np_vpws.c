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
    COMPILER_64_SET(data_udh,0x12345678, 0x9abcdef0);
    rv = bcm_port_wide_data_set(unit, vlan_port->vlan_port_id, BCM_PORT_WIDE_DATA_INGRESS, data_udh);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }
    return rv;
}

int pw_create(int unit, bcm_mpls_port_t *mpls_port, int port)
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
    
    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }
        
    printf("pw created...\n");
    /* write auxiliary data to lif extension */        
    COMPILER_64_SET(data_udh,0x12345678, 0x9abcdef0);
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
        rv = pw_create(unit, &mpls_port, pw_port);
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

