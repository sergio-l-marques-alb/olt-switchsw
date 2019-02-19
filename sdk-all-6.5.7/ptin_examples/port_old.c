int unit = 0;

// Do not configure translations
int xlate_config = 1;

int action_swap_id = 2;  /* Egress action 1: swap+none */
int action_push_id = 3;  /* Egress action 1: swap+push */
int action_pop_id  = 4;  /* Egress action 2: swap+pop  */

unsigned int outer_tpid = 0x8100;
unsigned int inner_tpid = 0x8100;

/*
 * Only set TPID values
 */
void tpid_set(unsigned int new_outer_tpid, unsigned int new_inner_tpid)
{
 outer_tpid = new_outer_tpid;
 inner_tpid = new_inner_tpid;
}

unsigned int advanced_vlan_editing = 0;

static int pcp_in_map_id  = -1;
static int pcp_out_map_id = -1;

/*
 * Global initialization (first steps before doing anything else)
 */
int global_init()
{
 int rv;
 bcm_port_t port;
 bcm_pbmp_t pbmp;
 bcm_gport_t gport;
 bcm_port_config_t pc;

 advanced_vlan_editing = soc_property_get(unit ,"bcm886xx_vlan_translate_mode",0);

 if (advanced_vlan_editing)
 {
  printf("Advanced translations are active\n");
 }

 printf("TPIDs to be used: 0x%x + 0x%x\n", outer_tpid, inner_tpid);

 rv=bcm_port_config_get(unit, &pc);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_config_get.\n");
  print rv;
  return rv;
 }

 /* Just put all the ports in the same vlan domain */

 BCM_PBMP_ITER(pc.xe, port)
 {
  rv = port_init(port, outer_tpid, inner_tpid);
  if (rv != BCM_E_NONE)
  {
   printf("error: port_init.\n");
   print rv;
   return rv;
  }
 }
 BCM_PBMP_ITER(pc.xl, port)
 {
  rv = port_init(port, outer_tpid, inner_tpid);
  if (rv != BCM_E_NONE)
  {
   printf("error: port_init.\n");
   print rv;
   return rv;
  }
 }
 BCM_PBMP_ITER(pc.ce, port)
 {
  rv = port_init(port, outer_tpid, inner_tpid);
  if (rv != BCM_E_NONE)
  {
   printf("error: port_init.\n");
   print rv;
   return rv;
  }
 }
 BCM_PBMP_ITER(pc.hg, port)
 {
  rv = port_init(port, outer_tpid, inner_tpid);
  if (rv != BCM_E_NONE)
  {
   printf("error: port_init.\n");
   print rv;
   return rv;
  }
 }

 /* Check if double lb key range is enabled (relevant for gracefull load balancing) */
 rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkIngressCommit, 0);
 if (rv != BCM_E_NONE)
 {
   printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
   return rv;
 }
 rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkEgressCommit, 0);
 if (rv != BCM_E_NONE)
 {
   printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
   return rv;
 }

 /* QoS initialization */
 rv = qos_init();
 if (rv != BCM_E_NONE)
 {
  printf("QoS init failed: rv=%u (%s)\n", rv, bcm_errmsg(rv));
  return rv;
 }

 /* Translations initialization */
 rv = translations_init();
 if (rv != BCM_E_NONE)
 {
  printf("Translations init failed: rv=%u (%s)\n", rv, bcm_errmsg(rv));
  return rv;
 } 

 printf("Switch initialized!\n");

 return 0;
}

/*
 * Initialize a physical port
 */
int port_init(int port, unsigned int otpid, unsigned int itpid)
{
 int rv;
 bcm_pbmp_t pbmp;
 bcm_gport_t gport;
 bcm_port_tpid_class_t port_tpid_class;

 BCM_GPORT_LOCAL_SET(gport, port);

 /* TAG mode */
 rv = bcm_port_dtag_mode_set(unit, port, BCM_PORT_DTAG_MODE_INTERNAL);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_dtag_mode_set for port %d.\n", port);
  print rv;
  return rv;
 }

 /* Filtering */
 rv = bcm_port_vlan_member_set(unit, port, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS | BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_vlan_member_set for port %d.\n", port);
  print rv;
  return rv;
 }

 /* MAC learning */
 rv = bcm_port_learn_set(unit, gport, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_tpid_set for gport 0x%x.\n", gport);
  print rv;
  return rv;
 }

 /* Default VLAN */
 rv = bcm_port_untagged_vlan_set(unit, port, 1);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_untagged_vlan_set for port %u.\n", port);
  print rv;
  return rv;
 }

 /* Remove port from VLAN 1 */
 BCM_PBMP_CLEAR(pbmp);
 BCM_PBMP_PORT_ADD(pbmp, port);
 rv = bcm_vlan_port_remove(unit, 1, pbmp);
 if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
  printf("Error, bcm_vlan_port_remove with vlan 1, port %d\n", port);
  print rv;
  return rv;
 }

 /* Egress Class set */
 rv = bcm_port_class_set(unit, port, bcmPortClassId, port);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_class_set to port %d.\n", port);
  print rv;
  return rv;
 }

 /* TPIDs */
 rv = bcm_port_tpid_delete_all(unit, port);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_tpid_delete_all to port %d.\n", port);
  print rv;
  return rv;
 }
 rv = bcm_port_tpid_set(unit, port, outer_tpid);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_tpid_set to port %d.\n", port);
  print rv;
  return rv;
 }
 rv = bcm_port_inner_tpid_set(unit, port, inner_tpid);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_inner_tpid_set for port %d.\n", port);
  print rv;
  return rv;
 }

 rv = bcm_vlan_control_port_set(unit, port, bcmVlanPortDoubleLookupEnable, 1);
 if (rv != BCM_E_NONE)
 {
  printf("Error configuring bcmVlanPortDoubleLookupEnable=1 to port %u: rv=%d\n", port, rv);
  return rv;
 }
 
 rv = bcm_switch_control_port_set(unit, port, /*type*/bcmSwitchTrunkHashPktHeaderCount,/*argumento*/ 0x2);
 //doc116, p71
 if (rv != BCM_E_NONE)
 {
  printf("Error, bcm_switch_control_port_set(), rv=%d.\n", rv);
  return rv;
 }

 /* Configure translations */
 if (xlate_config && advanced_vlan_editing) {
  // 2 port tag structures
  bcm_port_tpid_class_t_init(&port_tpid_class);
  port_tpid_class.port = port;
  port_tpid_class.tpid1 = otpid;
  port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
  port_tpid_class.tag_format_class_id = 2;  // can be any number !=0, 2 is backward for s-tag
  port_tpid_class.flags = 0;
  port_tpid_class.vlan_translation_action_id = 0;
  rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
  if (rv != BCM_E_NONE) {
   printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
   print rv;
   return rv;
  }
  //printf("port %u: Created new tag format for single-tagged traffic tag_format_class_id=%u\n", port, port_tpid_class.tag_format_class_id);
  bcm_port_tpid_class_t_init(&port_tpid_class);
  port_tpid_class.port = port;
  port_tpid_class.tpid1 = otpid;
  port_tpid_class.tpid2 = itpid;
  port_tpid_class.tag_format_class_id = 6;  // can be any number !=0, 2 is backward for s-tag
  port_tpid_class.flags = 0;
  port_tpid_class.vlan_translation_action_id = 0;
  rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
  if (rv != BCM_E_NONE) {
   printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
   print rv;
   return rv;
  }
  //printf("port %u: Created new tag format for double-tagged traffic tag_format_class_id=%u\n", port, port_tpid_class.tag_format_class_id);
 }

 printf("Port %d / gport 0x%x initialized! TPIDs=0x%x,0x%x\n", port, gport, outer_tpid, inner_tpid);
 return 0;
}

/**
 * Initialize VLAN translations
 */
int translations_init(void)
{
 int rv;
 bcm_vlan_action_set_t action;

 /* Configure translations */
 if (xlate_config && advanced_vlan_editing) {
  // Create egress translation action: swap+none
  rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_swap_id);
  if (rv != BCM_E_NONE) {
   printf("Error, bcm_vlan_translate_action_id_create\n");
   print rv;
   return rv;
  }
  bcm_vlan_action_set_t_init(&action);
  action.ut_outer = bcmVlanActionAdd;
  action.ut_inner = bcmVlanActionNone;
  action.ot_outer = bcmVlanActionReplace;
  action.ot_inner = bcmVlanActionNone;
  action.dt_outer = bcmVlanActionReplace;
  action.dt_inner = bcmVlanActionNone;
  action.outer_tpid = outer_tpid;
  action.inner_tpid = inner_tpid;
  action.ot_outer_pkt_prio = bcmVlanActionReplace;
  action.priority = pcp_out_map_id;
  rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_swap_id, &action);
  if (rv != BCM_E_NONE) {
   printf("Error, bcm_vlan_translate_action_id_set swap\n");
   print rv;
   return rv;
  }
  printf("Created new xlate action with for swap operation: action_id=%u\n", action_swap_id);

  // Create egress translation action: swap+push
  rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_push_id);
  if (rv != BCM_E_NONE) {
   printf("Error, bcm_vlan_translate_action_id_create\n");
   print rv;
   return rv;
  }
  bcm_vlan_action_set_t_init(&action);
  action.ut_outer = bcmVlanActionAdd;
  action.ut_inner = bcmVlanActionAdd;
  action.ot_outer = bcmVlanActionReplace;
  action.ot_inner = bcmVlanActionAdd;
  action.dt_outer = bcmVlanActionReplace;
  action.dt_inner = bcmVlanActionAdd;
  action.outer_tpid = outer_tpid;
  action.inner_tpid = inner_tpid;
  action.ot_outer_pkt_prio = bcmVlanActionReplace;
  action.priority = pcp_out_map_id;
  rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_push_id, &action);
  if (rv != BCM_E_NONE) {
   printf("Error, bcm_vlan_translate_action_id_set swap+push\n");
   print rv;
   return rv;
  }
  printf("Created new xlate action with for swap+push operations: action_id=%u\n", action_push_id);

  // Create egress translation action: swap+pop
  rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_pop_id);
  if (rv != BCM_E_NONE) {
   printf("Error, bcm_vlan_translate_action_id_create\n");
   print rv;
   return rv;
  }
  bcm_vlan_action_set_t_init(&action);
  action.ut_outer = bcmVlanActionAdd;
  action.ut_inner = bcmVlanActionNone;
  action.ot_outer = bcmVlanActionReplace;
  action.ot_inner = bcmVlanActionNone;
  action.dt_outer = bcmVlanActionReplace;
  action.dt_inner = bcmVlanActionDelete;
  action.outer_tpid = outer_tpid;
  action.inner_tpid = inner_tpid;
  action.ot_outer_pkt_prio = bcmVlanActionReplace;
  action.priority = pcp_out_map_id;
  rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_pop_id, &action);
  if (rv != BCM_E_NONE) {
   printf("Error, bcm_vlan_translate_action_id_set swap+pop\n");
   print rv;
   return rv;
  }
  printf("Created new xlate action with for swap+pop operations: action_id=%u\n", action_pop_id);
 }

 return BCM_E_NONE;
}

/**
 * Initialize QoS
 */
int qos_init(void)
{
 bcm_qos_map_t qos_l2_map;
 int idx;
 int rv;

 /* Configure a PCP map to be applied to IVE/EVE */
 rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_L2_VLAN_PCP, &pcp_in_map_id);
 if (rv != BCM_E_NONE)
 {
  printf("error in ingress PCP bcm_qos_map_create(): rv=%u (%s)\n", rv, bcm_errmsg(rv));
  return rv;
 }
 rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_L2_VLAN_PCP, &pcp_out_map_id);
 if (rv != BCM_E_NONE)
 {
  printf("error in egress PCP bcm_qos_map_create(): rv=%u (%s)\n", rv, bcm_errmsg(rv));
  return rv;
 }
 bcm_qos_map_t_init(&qos_l2_map);
 for (idx=0; idx<16; idx++)
 {
  qos_l2_map.pkt_pri = idx >> 1; //qos_map_l2_pcp[idx];
  qos_l2_map.pkt_cfi = idx % 2;  //qos_map_l2_cfi[idx];
  qos_l2_map.int_pri = idx >> 1; //qos_map_l2_internal_pri[idx];
  qos_l2_map.color   = ((idx % 2) == 0) ? bcmColorGreen : bcmColorYellow; //qos_map_l2_internal_color[idx];
  
  rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_L2_VLAN_PCP, &qos_l2_map, pcp_in_map_id);
  if (rv != BCM_E_NONE) {
   printf("error in PCP ingress bcm_qos_map_add(): rv=%d (%d)\n", rv, bcm_errmsg(rv));
   return rv;
  }
  rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_L2_VLAN_PCP, &qos_l2_map, pcp_out_map_id);
  if (rv != BCM_E_NONE) {
   printf("error in PCP egress bcm_qos_map_add(): rv=%d (%d)\n", rv, bcm_errmsg(rv));
   return rv;
  }
 }

 return BCM_E_NONE;
}

/*
 * Configure a VLAN Edit Profile
 */
int adv_xlate_vep(unsigned int gport, int new_ovid, int new_ivid, int edit_class_id, int is_ingress)
{
 bcm_vlan_port_translation_t port_xlate;
 int rv;

 bcm_vlan_port_translation_t_init(&port_xlate);
 port_xlate.new_outer_vlan = new_ovid;
 port_xlate.new_inner_vlan = new_ivid;
 port_xlate.gport = gport;
 port_xlate.flags = (is_ingress) ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;
 port_xlate.vlan_edit_class_id = edit_class_id;

 rv = bcm_vlan_port_translation_set(unit, &port_xlate);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vlan_port_translate_set\n");
  print rv;
  return rv;
 }

 printf("Created new VLAN Edit Profile with vlan_edit_class_id=%u\n", port_xlate.vlan_edit_class_id);

 return 0;
}

/*
 * Create an action for translations
 */
int adv_xlate_action_create(int action_id, int is_ingress,
                            int ot_outer, int dt_outer, int ot_inner, int dt_inner,
                            int ot_outer_pri, int dt_outer_pri, int ot_inner_pri, int dt_inner_pri, int map_id)
{
  int rv;
  unsigned int flags = 0;
  bcm_vlan_action_set_t action;

  if (action_id != 0)  flags |= BCM_VLAN_ACTION_SET_WITH_ID;
  if (is_ingress)      flags |= BCM_VLAN_ACTION_SET_INGRESS;
  else                 flags |= BCM_VLAN_ACTION_SET_EGRESS;

  /* Create egress translation action: swap+pop */
  rv = bcm_vlan_translate_action_id_create(unit, flags, &action_id);
  if (rv != BCM_E_NONE) {
    printf("Error, bcm_vlan_translate_action_id_create\n");
    print rv;
    return rv;
  }

  bcm_vlan_action_set_t_init(&action);
  action.ot_outer = ot_outer;
  action.ot_inner = ot_inner;
  action.dt_outer = dt_outer;
  action.dt_inner = dt_inner;
  action.ot_outer_prio     = ot_outer_pri;
  action.ot_outer_pkt_prio = ot_outer_pri;
  action.ot_inner_pkt_prio = ot_inner_pri;
  action.dt_outer_prio     = dt_outer_pri;
  action.dt_outer_pkt_prio = dt_outer_pri;
  action.dt_inner_prio     = dt_inner_pri;
  action.dt_inner_pkt_prio = dt_inner_pri;
  action.ot_outer_cfi      = ot_outer_pri;
  action.ot_inner_cfi      = ot_inner_pri;
  action.dt_outer_cfi      = dt_outer_pri;
  action.dt_inner_cfi      = dt_inner_pri;
  action.priority          = map_id;
  action.outer_tpid        = outer_tpid;
  action.inner_tpid        = inner_tpid;

  rv = bcm_vlan_translate_action_id_set(unit, (is_ingress) ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS, action_id, &action);

  if (rv != BCM_E_NONE) {
    printf("Error, bcm_vlan_translate_action_id_set: rv=%d\n", rv);
    return rv;
  }
  printf("Created new xlate action: action_id=%u\n", action_id);
}

/*
 * ???
 */
int adv_xlate_action_add(int action_id, int outer_action, int inner_action, unsigned int outer_tpid, unsigned int inner_tpid, int new_ovid, int new_ivid, int is_ingress)
{
  int rv;
  unsigned int flags = 0;

  if (is_ingress)      flags |= BCM_VLAN_ACTION_SET_INGRESS;
  else                 flags |= BCM_VLAN_ACTION_SET_EGRESS;

  rv = bcm_vlan_translate_action_add(unit, flags, outer_action, inner_action, outer_tpid, inner_tpid);
  if (rv != BCM_E_NONE) {
    printf("Error, bcm_vlan_translate_action_add: rv=%d\n", rv);
    return rv;
  }

  printf("Xlate action added\n");
}

/*
 * Configure Tag format for translations
 */
int adv_xlate_action_class_set(int tag_class_id, int edit_class_id, int action_id, int is_ingress)
{
 bcm_vlan_translate_action_class_t action_class;
 int rv;

 bcm_vlan_translate_action_class_t_init(&action_class);
 action_class.tag_format_class_id = tag_class_id;
 action_class.vlan_edit_class_id = edit_class_id;
 action_class.vlan_translation_action_id = action_id;
 action_class.flags = (is_ingress) ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;

 rv = bcm_vlan_translate_action_class_set(unit, &action_class);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vlan_translate_action_class_set\n");
  print rv;
  return rv;
 }

 printf("Created %s XLATE rule with tag_format_class_id=%u, edit_class_id=%u, action_id=%u\n", ((is_ingress) ? "INGRESS" : "EGRESS"), tag_class_id, edit_class_id, action_id);

 return 0;
}

/*
 * Configure the default VLAN for a physical port
 */
int port_defvlan_set(unsigned int port, unsigned int pvid)
{
 int rv;

 rv = bcm_port_untagged_vlan_set(unit, port, pvid);

 if (rv != BCM_E_NONE)
 {
  printf("Failed configuring the PVID: rv=%d\r\n", rv);
  return rv;
 }

 printf("PVID %u applied to port %u\r\n", pvid, port);

 return 0;                                                                                                                                                                    
}
