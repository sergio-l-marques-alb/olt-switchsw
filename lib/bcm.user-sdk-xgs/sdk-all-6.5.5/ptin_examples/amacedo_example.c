/* QoS Maps */
int rval=0;
char color[8], remark_color[8];

int pcp_in_map_id  = -1; //PCP Ingress Map id
int pcp_eg_map_id  = -1; //PCP Eress Map id

bcm_qos_map_t qos_l2_map;
int idx;

/*Configure a Egress PCP map to be applied to  EVE */
print bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS  | BCM_QOS_MAP_L2_VLAN_PCP, &pcp_eg_map_id);

bcm_qos_map_t_init(&qos_l2_map);
printf("Egress MAP.\r\n");
for (idx=0; idx<16; idx++)
{
    qos_l2_map.pkt_pri = idx >> 1; //qos_map_l2_pcp[idx];
    qos_l2_map.pkt_cfi = idx % 2;  //qos_map_l2_cfi[idx];
    qos_l2_map.int_pri = idx >> 1; //qos_map_l2_internal_pri[idx];
    qos_l2_map.color   = ((idx % 2) == 0) ? bcmColorGreen : bcmColorYellow; //qos_map_l2_internal_color[idx];
    rval=bcm_qos_map_add(unit, BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP |BCM_QOS_MAP_L2_UNTAGGED, &qos_l2_map, pcp_eg_map_id);
    if(rval!=0)
    {
        printf("error bcm_qos_map_add Egress.\r\n");
    }
    //get_color(qos_l2_map.color,&color);
    //get_color(qos_l2_map.remark_color, &remark_color);
    printf("int_pri: %d\t int color: %s\t pkt_pri: %d pkt_cfi: %d\n",qos_l2_map.int_pri, color, qos_l2_map.pkt_pri, qos_l2_map.pkt_cfi);
}

//Associate LIF to Ingress and Egress Maps
print bcm_qos_port_map_set(0/*unit*/, 0x44801000/*LIF*/, -1 /*pcp_in_map_id*/, pcp_eg_map_id);
print bcm_qos_port_map_set(0/*unit*/, 0x44801001/*LIF*/, -1 /*pcp_in_map_id*/, pcp_eg_map_id);
print bcm_qos_port_map_set(0/*unit*/, 0x44801002/*LIF*/, -1 /*pcp_in_map_id*/, pcp_eg_map_id);

/* EVE configuration */
int action_id = 5;
bcm_vlan_action_set_t action;
bcm_vlan_translate_action_class_t action_class;
bcm_vlan_port_translation_t port_xlate;
bcm_port_tpid_class_t port_tpid_class;

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
action.priority          = pcp_eg_map_id;//Map ID
action.outer_tpid        = 0x8100;
action.inner_tpid        = 0x8100;
print bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_id, &action);

/* Associate the action to a VEP id */ //Vlan Editing Profile
bcm_vlan_translate_action_class_t_init(&action_class);
action_class.tag_format_class_id        = 2 /*Single Tag Format */;
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
port_xlate.gport = 0x44801002 /*LIF2*/;
port_xlate.flags = BCM_VLAN_ACTION_SET_EGRESS;
port_xlate.vlan_edit_class_id = 2 /*VEP id*/;
print bcm_vlan_port_translation_set(unit, &port_xlate);

printf ("policer and EVE Done! \n");



/* Create Policer with CIR=10Mbps + EIR=10Mbps / PIR=20Mbps */

bcm_policer_t policer_id;
bcm_policer_config_t pol_cfg;

policer_id=30;
bcm_policer_config_t_init(&pol_cfg);
pol_cfg.max_pkbits_sec = 45000;//-1;//20000; //No EIR Limit
pol_cfg.pkbits_sec=45000; //EIR
pol_cfg.pkbits_burst=1000;
pol_cfg.ckbits_sec=15000;//CIR
pol_cfg.ckbits_burst=1000;
pol_cfg.max_ckbits_sec = 15000;//-1;//20000; //No CIR Limit
pol_cfg.mode=bcmPolicerModeTrTcmDs;
pol_cfg.flags=BCM_POLICER_WITH_ID;
print bcm_policer_create(0, &pol_cfg, &policer_id);
print policer_id;

/* Create PMF rule to attach policer */

bcm_field_qset_t qset;
bcm_field_aset_t aset;
bcm_field_group_t group;
bcm_field_entry_t entry;
int group_priority=6;

group=1;
BCM_FIELD_QSET_INIT(qset);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyStageIngress);//Incoming pACKETS
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyInVPort);    //pASSING THROUGTH lifS
//BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyDstMac);    // DSTMAC Defined!
//BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyOutVPort);
//BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyVlanFormat); // Sensitive to VLAN FORMAT
BCM_FIELD_ASET_INIT(aset);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionPolicerLevel0);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionUsePolicerResult);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionOuterVlanPrioNew); //Acção de Alteração do PCP/DEI do Pacote
BCM_FIELD_ASET_ADD(aset,bcmFieldActionVlanActionSetNew);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionPrioIntNew); // Maped to IntPrio

print bcm_field_group_create_mode_id(0, qset, group_priority, bcmFieldGroupModeAuto, group);
print bcm_field_group_action_set(0, group, aset);
print bcm_field_entry_create(0, group, &entry);
print bcm_field_qualify_InVPort32(0, entry, 0x1000, 0xffff); //Refering to GPort_LIF gp_vp1=0x44801000;
print bcm_field_action_add(0, entry, bcmFieldActionVlanActionSetNew, action_id /*Action ID*/, 0); //

print bcm_field_entry_policer_attach(0, entry, 0, (policer_id & 0xffff));
print bcm_field_entry_install(0, entry);
print entry;

/* Without this, packets are always dropped... It looks default input color is RED */
print bcm_port_vlan_priority_map_set(0, 9/*Device or Logical PortNumber*/, 0 /* Pkt priority*/, 0 /*Pkt cfi*/, 0 /*internal priority*/, bcmColorGreen /* Internal color*/);
print bcm_port_vlan_priority_map_set(0, 9/*Device or Logical PortNumber*/, 1 /* Pkt priority*/, 0 /*Pkt cfi*/, 1 /*internal priority*/, bcmColorGreen /* Internal color*/);
print bcm_port_vlan_priority_map_set(0, 9/*Device or Logical PortNumber*/, 2 /* Pkt priority*/, 0 /*Pkt cfi*/, 2 /*internal priority*/, bcmColorGreen /* Internal color*/);
print bcm_port_vlan_priority_map_set(0, 9/*Device or Logical PortNumber*/, 3 /* Pkt priority*/, 0 /*Pkt cfi*/, 3 /*internal priority*/, bcmColorGreen /* Internal color*/);
print bcm_port_vlan_priority_map_set(0, 9/*Device or Logical PortNumber*/, 4 /* Pkt priority*/, 0 /*Pkt cfi*/, 4 /*internal priority*/, bcmColorGreen /* Internal color*/);
print bcm_port_vlan_priority_map_set(0, 9/*Device or Logical PortNumber*/, 5 /* Pkt priority*/, 0 /*Pkt cfi*/, 5 /*internal priority*/, bcmColorGreen /* Internal color*/);
print bcm_port_vlan_priority_map_set(0, 9/*Device or Logical PortNumber*/, 6 /* Pkt priority*/, 0 /*Pkt cfi*/, 6 /*internal priority*/, bcmColorGreen /* Internal color*/);
print bcm_port_vlan_priority_map_set(0, 9/*Device or Logical PortNumber*/, 7 /* Pkt priority*/, 0 /*Pkt cfi*/, 7 /*internal priority*/, bcmColorGreen /* Internal color*/);


/* Tell Dune to Drop Red packets */
print bcm_cosq_discard_set(0, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_COLOR_BLACK);

