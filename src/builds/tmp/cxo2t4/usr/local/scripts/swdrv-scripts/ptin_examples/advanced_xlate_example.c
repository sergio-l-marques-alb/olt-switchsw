int action_id = 5;
bcm_vlan_action_set_t action;
bcm_vlan_translate_action_class_t action_class;
bcm_vlan_port_translation_t port_xlate;

/* Create action */
print bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_WITH_ID | BCM_VLAN_ACTION_SET_EGRESS, &action_id);
bcm_vlan_action_set_t_init(&action);
//action.ut_outer = bcmVlanActionAdd;
//action.ut_inner = bcmVlanActionNone;
action.ot_outer = bcmVlanActionReplace;
action.ot_inner = bcmVlanActionNone;
action.dt_outer = bcmVlanActionReplace;
action.dt_inner = bcmVlanActionNone;
action.ot_outer_prio     = bcmVlanActionReplace;
action.ot_outer_pkt_prio = bcmVlanActionReplace;
action.ot_outer_cfi      = bcmVlanActionReplace;
action.dt_outer_prio     = bcmVlanActionReplace;
action.dt_outer_pkt_prio = bcmVlanActionReplace;
action.dt_outer_cfi      = bcmVlanActionReplace;
action.priority          = pcp_out_map_id;
action.outer_tpid        = outer_tpid;
action.inner_tpid        = inner_tpid;
print bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_id, &action);

/* Associate the action to a VEP id */
bcm_vlan_translate_action_class_t_init(&action_class);
action_class.tag_format_class_id        = 2 /*Single Tag Format*/;
action_class.vlan_edit_class_id         = 2 /*VEP id*/;
action_class.vlan_translation_action_id = action_id;
action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
print bcm_vlan_translate_action_class_set(unit, &action_class);

/* Finalize translation configurations */
bcm_vlan_port_translation_t_init(&port_xlate);
port_xlate.new_outer_vlan = 100;
port_xlate.gport = 0x44801000 /*LIF1*/;
port_xlate.flags = BCM_VLAN_ACTION_SET_EGRESS;
port_xlate.vlan_edit_class_id = 2 /*VEP id*/;
print bcm_vlan_port_translation_set(unit, &port_xlate);

bcm_vlan_port_translation_t_init(&port_xlate);
port_xlate.new_outer_vlan = 100;
port_xlate.gport = 0x44801001 /*LIF2*/;
port_xlate.flags = BCM_VLAN_ACTION_SET_EGRESS;
port_xlate.vlan_edit_class_id = 2 /*VEP id*/;
print bcm_vlan_port_translation_set(unit, &port_xlate);

bcm_vlan_port_translation_t_init(&port_xlate);
port_xlate.new_outer_vlan = 100;
port_xlate.gport = 0x44801002 /*LIF3*/;
port_xlate.flags = BCM_VLAN_ACTION_SET_EGRESS;
port_xlate.vlan_edit_class_id = 2 /*VEP id*/;
print bcm_vlan_port_translation_set(unit, &port_xlate);
