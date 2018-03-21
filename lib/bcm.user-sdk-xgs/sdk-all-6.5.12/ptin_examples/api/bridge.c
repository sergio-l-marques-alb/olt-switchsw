/* Dependencies: port.c */

/*
 * Create a LIF
 */
int vp_add(int unit, int lif_id, int criteria, int port, int vlan_ing, int inner_vlan_ing, int tunnel_id, int vsi, unsigned int flags)
{
 bcm_gport_t gport;
 bcm_vlan_port_t vp;
 int rv;

 /* Create virtual port */
 bcm_vlan_port_t_init(&vp);

 if (criteria <= 0 || criteria >= BCM_VLAN_PORT_MATCH_COUNT)
 {
  if (inner_vlan_ing > 0)  criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
  else if (vlan_ing > 0)   criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
  else if (vlan_ing < 0)   criteria = BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN;
  else                     criteria = BCM_VLAN_PORT_MATCH_PORT;
 }

 vp.criteria = criteria;
 vp.port = port;
 vp.match_vlan = vlan_ing;
 vp.match_inner_vlan   = inner_vlan_ing;
 vp.match_tunnel_value = tunnel_id;
 vp.egress_vlan        = vp.match_vlan;
 vp.egress_inner_vlan  = vp.match_inner_vlan;
 vp.egress_tunnel_value= vp.match_tunnel_value;
 vp.vsi = vsi; /* will be populated when the gport is added to service, using vswitch_port_add */
 vp.vlan_port_id       = lif_id;
 vp.flags              = flags;
 if (lif_id != 0)  vp.flags |= BCM_VLAN_PORT_WITH_ID;

 rv = bcm_vlan_port_create(unit, &vp);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vlan_port_create: : rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  return rv;
 }

/*
 rv = bcm_vlan_control_port_set(unit, vp.vlan_port_id, bcmVlanPortDoubleLookupEnable, (criteria != BCM_VLAN_PORT_MATCH_PORT));
 if (rv != BCM_E_NONE)
 {
  printf("Error configuring bcmVlanPortDoubleLookupEnable=%u to lif 0x%x: rv=%d\n", (criteria != BCM_VLAN_PORT_MATCH_PORT), vp.vlan_port_id, rv);
  return 0;
 }
*/

 printf("unit:%d, vlan_port_id 0x%x created for port 0x%x, outerVLAN=%d, innerVLAN=%d, tunnel=%d (VSI=%u, criteria=%d flags=0x%x)\n",unit, vp.vlan_port_id,
        vp.port, vp.match_vlan, vp.match_inner_vlan, vp.match_tunnel_value, vp.vsi, criteria, vp.flags);

 return BCM_E_NONE;
}

/*
 * Remove a LIF
 */
int vp_remove(int unit, unsigned int vlan_port_id)
{
 int rv;

 rv = bcm_vlan_port_destroy(unit, vlan_port_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, bcm_vlan_port_destroy: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  print rv;
  return rv;
 }

 printf("unit:%d, vlan_port_id 0x%x destroyed\n",unit, vlan_port_id);

 return BCM_E_NONE;
}

/*
 * ???
 */
int port_match_add(int unit,int port, int vlan, int vlan_port_id)
{
 bcm_port_match_info_t match_info;
 int rv;

 bcm_port_match_info_t_init(&match_info);

 match_info.match = BCM_PORT_MATCH_PORT;
 match_info.port = port;
 match_info.match_vlan = vlan;
 match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
 rv = bcm_port_match_add(unit, vlan_port_id, &match_info); /* associating the port to lif */
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, bcm_port_match_add: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  print rv;
  return rv;
 }
 printf("Port match added\n");
 return BCM_E_NONE;
}

/*
 * ???
 */
int port_match_remove(int unit, int port, int vlan, int vlan_port_id)
{
 bcm_port_match_info_t match_info;
 int rv;

 bcm_port_match_info_t_init(&match_info);

 match_info.match = BCM_PORT_MATCH_PORT;
 match_info.port = port;
 match_info.match_vlan = vlan;
 match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
 rv = bcm_port_match_delete(unit, vlan_port_id, &match_info); /* associating the port to lif */
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, bcm_port_match_delete: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  print rv;
  return rv;
 }
 printf("unit:%d, Port match removed: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
 return BCM_E_NONE;
}

/*
 * Create a cross-connect
 */
int cc_add(int unit, unsigned int vlan_port_1, unsigned int vlan_port_2)
{
 bcm_vswitch_cross_connect_t cross_connect;
 int rv;

 /* Add ports to VSWITCH instance */
 cross_connect.port1 = vlan_port_1;
 cross_connect.port2 = vlan_port_2;
 //cross_connect.encap1 = 0;
 //cross_connect.encap2 = 0;
 //cross_connect.flags = 0;

 rv = bcm_vswitch_cross_connect_add(unit, &cross_connect);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, bcm_vswitch_cross_connect_add: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  print rv;
  return rv;
 }
 printf("unit:%d, Cross-connect added between vlan_port 0x%x and 0x%x\n", unit, vlan_port_1, vlan_port_2);

 return BCM_E_NONE;
}

/*
 * Remove a cross-connect
 */
int cc_remove(int unit, unsigned int vlan_port_1, unsigned int vlan_port_2)
{
 bcm_vswitch_cross_connect_t cross_connect;
 int rv;

 /* Add ports to VSWITCH instance */
 cross_connect.port1 = vlan_port_1;
 cross_connect.port2 = vlan_port_2;
 //cross_connect.encap1 = 0;
 //cross_connect.encap2 = 0;
 //cross_connect.flags = 0;

 rv = bcm_vswitch_cross_connect_delete(unit, &cross_connect);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
  printf("unit:%d, Error, bcm_vswitch_cross_connect_add: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  print rv;
  return rv;
 }
 printf("unit:%d, Cross-connect between vlan_ports 0x%x and 0x%x removed\n",unit, vlan_port_1, vlan_port_2);

 return BCM_E_NONE;
}

/*
 * Create a VLAN
 */
int vlan_create(int unit, int vlan)
{
 int rv;

 /* Create VLANs */
 rv = bcm_vlan_create(unit, vlan);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
 {
  printf("unit:%d, error: bcm_vlan_create failed: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  return rv;
 }

 printf("unit:%d, Vlan %u created!\n",unit, vlan);

 return 0;
}

/*
 * Destroy a VLAN
 */
int vlan_destroy(int unit,int vlan)
{
 int rv;

 /* Create VLANs */
 rv = bcm_vlan_destroy(unit, vlan);
 if (rv != BCM_E_NONE && rv != BCM_NOT_EXIST)
 {
  printf("unit:%d, error: bcm_vlan_destroy failed: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  return rv;
 }

 printf("unit:%d, Vlan %u destroyed!\n",unit, vlan);

 return 0;
}

/*
 * Add a physical port to a VLAN
 */
int vlan_add(int unit, int vlan, int port)
{
 int rv;
 bcm_pbmp_t pbmp, ubmp;

 /* Add port member */
 BCM_PBMP_CLEAR(pbmp);
 BCM_PBMP_CLEAR(ubmp);
 BCM_PBMP_PORT_ADD(pbmp, port);

 rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
 {
  printf("unit:%d, error: bcm_vlan_port_add failed.: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  print rv;
  return rv;
 }
 if (rv == BCM_E_NONE)
 {
  printf("unit:%d, Port %u added to vlan %u\n", unit, port, vlan);
 }

 return BCM_E_NONE;
}

/*
 * Remove a port from a VLAN
 */
int vlan_remove(int unit, int vlan, int port)
{
 int rv;
 bcm_pbmp_t pbmp, ubmp;

 /* Remove port member */
 BCM_PBMP_CLEAR(pbmp);
 BCM_PBMP_CLEAR(ubmp);
 BCM_PBMP_PORT_ADD(pbmp, port);

 rv = bcm_vlan_port_remove(unit, vlan, pbmp);
 if (rv != BCM_E_NONE)
 {
  printf("unit:%d,  error: bcm_vlan_port_remove failed: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  print rv;
  return rv;
 }
 printf("unit:%d, Port %u removed from VLAN %u\n", unit, port, vlan);

/*
 // If all members were removed, destroy VLAN
 rv = bcm_vlan_port_get(unit, vlan, &pbmp, &ubmp);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_vlan_port_get failed.\n");
  print rv;
  return rv;
 }
 if (BCM_PBMP_IS_NULL(pbmp))
 {
  // Destroy VLAN
  rv = bcm_vlan_destroy(unit, vlan);
  if (rv != BCM_E_NONE)
  {
   printf("error: bcm_vlan_destroy failed.\n");
   print rv;
   return rv;
  }
  printf("VLAN %u destroyed!\n", vlan);
 }
*/

 return BCM_E_NONE;
}

/*
 * Create a VSI
 */
int vswitch_create(int unit, int vsi)
{
 int rv;

 /* Create VSWITCH instance */
 rv = bcm_vswitch_create_with_id(unit, vsi);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
 {
  printf("unit:%d, error: bcm_vswitch_create_with_id: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  print rv;
  return rv;
 }
 printf("unit:%d, VSI %u created!\n", unit, vsi);
 return 0;
}

/*
 * Destroy a VSI
 */
int vswitch_destroy(int unit, int vsi)
{
 int rv;

 /* Create VSWITCH instance */
 rv = bcm_vswitch_destroy(unit, vsi);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
 {
  printf("unit:%d, error: bcm_vswitch_destroy: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  print rv;
  return rv;
 }
 printf("unit:%d, VSI %u destroyed!\n",unit, vsi);
 return 0;
}

/*
 * Add a LIF to a VSI
 */
int vswitch_add(int unit, int vsi, int vlan_port_id)
{
 int rv;

 /* Add ports to VSWITCH instance */
 rv = bcm_vswitch_port_add(unit, vsi, vlan_port_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, bcm_vswitch_port_add: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  print rv;
  return rv;
 }

 printf("unit:%d, Vlan_port_id 0x%x added to VSI %u!\n", unit, vlan_port_id, vsi);

 return BCM_E_NONE;
}

/*
 * Remove a LIF from the VSI
 */
int vswitch_remove(int unit, int vsi, int vlan_port_id)
{
 int rv;

 /* Remove port from VSWITCH instance */
 rv = bcm_vswitch_port_delete(unit, vsi, vlan_port_id);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
  printf("unit:%d, Error, bcm_vswitch_port_delete: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  print rv;
  return rv;
 }

 printf("unit:%d, vlan_port_id 0x%x removed from VSI %u!\n", unit, vlan_port_id, vsi);

 return BCM_E_NONE;
}

/*
 * Create a MC group for replication purposes
 */
int multicast_create(int unit,int *mc_group, int is_ingress, uint32 flags)
{
 int rv;

 if (flags == 0) {
         flags = BCM_MULTICAST_TYPE_L2;
 }

 /* Create Multicast group */
 rv = bcm_multicast_create(unit, ((is_ingress) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP) | BCM_MULTICAST_WITH_ID | flags, mc_group);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, bcm_multicast_create: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  return rv;
 }
 printf("unit:%d, Multicast group 0x%x created\n",unit, *mc_group);

 return BCM_E_NONE;
}

/*
 * Destroy a MC group
 */
int multicast_destroy(int unit, int mc_group)
{
 int rv;

 /* Create Multicast group */
 rv = bcm_multicast_destroy(unit, mc_group);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, bcm_multicast_destroy: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
  return rv;
 }
 printf("unit:%d, Multicast group 0x%x destroyed\n", unit, mc_group);

 return BCM_E_NONE;
}

/*
 * Add a LIF to a MC group (for replication purposes)
 */
int multicast_ingress_add(int unit, int mc_group, int port, int vlan_port_id)
{
 int rv;
 bcm_if_t encap_id;
 
 rv = bcm_multicast_vlan_encap_get(unit, mc_group, port, vlan_port_id, &encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n",unit, mc_group, port, vlan_port_id);
  return rv;
 }

 rv = bcm_multicast_ingress_add(unit, mc_group, port, encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n",unit, mc_group, port, encap_id);
  return rv;
 }

 printf("unit:%d, Replication entry added: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));

 return BCM_E_NONE;
}

/*
 * Remove a LIF from a MC group
 */
int multicast_ingress_remove(int unit, int mc_group, int port, int vlan_port_id)
{
 int rv;
 bcm_if_t encap_id;

 rv = bcm_multicast_vlan_encap_get(unit, mc_group, port, vlan_port_id, &encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n",unit, mc_group, port, vlan_port_id);
  return rv;
 }

 rv = bcm_multicast_ingress_delete(unit, mc_group, port, encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_ingress_delete mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", unit, mc_group, port, encap_id);
  return rv;
 }

 printf("unit:%d, Replication entry removed: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));

 return BCM_E_NONE;
}

/*
 * Add a LIF to a MC group (for replication purposes)
 */
int multicast_egress_add(int unit, int mc_group, int port, int vlan_port_id)
{
 int rv;
 bcm_if_t encap_id;
 
 rv = bcm_multicast_vlan_encap_get(unit, mc_group, port, vlan_port_id, &encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n",unit, mc_group, port, vlan_port_id);
  return rv;
 }

 rv = bcm_multicast_egress_add(unit, mc_group, port, encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_egress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", unit, mc_group, port, encap_id);
  return rv;
 }

 printf("Replication entry added\n");

 return BCM_E_NONE;
}

/*
 * Remove a LIF from a MC group
 */
int multicast_egress_remove(int unit, int mc_group, int port, int vlan_port_id)
{
 int rv;
 bcm_if_t encap_id;

 rv = bcm_multicast_vlan_encap_get(unit, mc_group, port, vlan_port_id, &encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", unit, mc_group, port, vlan_port_id);
  return rv;
 }

 rv = bcm_multicast_egress_delete(unit, mc_group, port, encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_egress_delete mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n",unit, mc_group, port, encap_id);
  return rv;
 }

 printf("unit:%d, Replication entry removed: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));

 return BCM_E_NONE;
}

/*
 * Add a LIF to a MC group (for replication purposes)
 */
int multicast_l3_ingress_add(int unit, int mc_group, int port, int out_vlan)
{
 int rv;
 bcm_if_t encap_id;
 bcm_gport_t dest_gport;

 BCM_GPORT_LOCAL_SET(dest_gport, port);

 rv = bcm_multicast_l3_encap_get(unit, mc_group, dest_gport, out_vlan, &encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_l3_encap_get mc_group_id: 0x%08x  gport: 0x%08x  vlan: %u \n", unit, mc_group, dest_gport, out_vlan);
  return rv;
 }

 rv = bcm_multicast_ingress_add(unit, mc_group, dest_gport, encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  gport: 0x%08x  encap_id:  0x%08x \n", unit, mc_group, dest_gport, encap_id);
  return rv;
 }

 printf("unit:%d, Replication entry added: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));

 return BCM_E_NONE;
}

/*
 * Remove a LIF from a MC group
 */
int multicast_l3_ingress_remove(int unit, int mc_group, int port, int out_vlan)
{
 int rv;
 bcm_if_t encap_id;
 bcm_gport_t dest_gport;

 BCM_GPORT_LOCAL_SET(dest_gport, port);

 rv = bcm_multicast_l3_encap_get(unit, mc_group, port, out_vlan, &encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_l3_encap_get mc_group_id: 0x%08x  phy_port: 0x%08x  vlan: %u\n", unit, mc_group, dest_gport, out_vlan);
  return rv;
 }

 rv = bcm_multicast_ingress_delete(unit, mc_group, port, encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_ingress_delete mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", unit, mc_group, dest_gport, encap_id);
  return rv;
 }

 printf("unit:%d, Replication entry removed: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));

 return BCM_E_NONE;
}

/*
 * Add a LIF to a MC group (for replication purposes)
 */
int multicast_l3_egress_add(int unit, int mc_group, int port, int out_vlan)
{
 int rv;
 bcm_if_t encap_id;
 bcm_gport_t dest_gport;

 BCM_GPORT_LOCAL_SET(dest_gport, port);

 rv = bcm_multicast_l3_encap_get(unit, mc_group, dest_gport, out_vlan, &encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_l3_encap_get mc_group_id: 0x%08x  gport: 0x%08x  vlan: %u\n", unit, mc_group, dest_gport, out_vlan);
  return rv;
 }

 rv = bcm_multicast_egress_add(unit, mc_group, dest_gport, encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_egress_add mc_group_id:  0x%08x  gport: 0x%08x  encap_id:  0x%08x \n",unit, mc_group, dest_gport, encap_id);
  return rv;
 }

 printf("unit:%d, Replication entry added: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));

 return BCM_E_NONE;
}

/*
 * Remove a LIF from a MC group
 */
int multicast_l3_egress_remove(int unit, int mc_group, int port, int out_vlan)
{
 int rv;
 bcm_if_t encap_id;
 bcm_gport_t dest_gport;

 BCM_GPORT_LOCAL_SET(dest_gport, port);

 rv = bcm_multicast_l3_encap_get(unit, mc_group, port, out_vlan, &encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_l3_encap_get mc_group_id: 0x%08x  phy_port: 0x%08x  vlan: %u \n", unit, mc_group, port, out_vlan);
  return rv;
 }

 rv = bcm_multicast_egress_delete(unit, mc_group, port, encap_id);
 if (rv != BCM_E_NONE) {
  printf("unit:%d, Error, in bcm_multicast_egress_delete mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", unit,mc_group, port, encap_id);
  return rv;
 }

 printf("unit:%d, Replication entry removed: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));

 return BCM_E_NONE;
}


int vswitch_flood_set(int unit, unsigned int lif_id, int vlanId, int mcgroup_flood_unkn_uc, int mcgroup_flood_unkn_mc, int mcgroup_flood_bc)
{
  bcm_vlan_control_vlan_t control;
  int rv;

  printf("unit:%d, lif=0x%x, vlanId %u: mcgroup_unkn_uc=%d mcgroup_unkn_mc=%d mcgroup_unkn_bc=%d\n",unit, lif_id, vlanId,
         mcgroup_flood_unkn_uc, mcgroup_flood_unkn_mc, mcgroup_flood_bc);

  /* If LIF id is specified */
  if (lif_id != 0)
  {
    /* Unknown Unicast */
    rv = bcm_port_control_set(unit, lif_id, bcmPortControlFloodUnknownUcastGroup, mcgroup_flood_unkn_uc);
    if (rv != BCM_E_NONE)
    {
      printf("unit:%d, Error with bcm_port_control_set(FloodUnknownUcastGroup): rv=%d (%s)\n", unit,rv, bcm_errmsg(rv));
      return -1;
    }

    /* Unknown Multicast */
    rv = bcm_port_control_set(unit, lif_id, bcmPortControlFloodUnknownMcastGroup, mcgroup_flood_unkn_mc);
    if (rv != BCM_E_NONE)
    {
      printf("unit:%d, Error with bcm_port_control_set(FloodUnknownMcastGroup): rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
      return -1;
    }

    /* Unknown Multicast */
    rv = bcm_port_control_set(unit, lif_id, bcmPortControlFloodBroadcastGroup, mcgroup_flood_bc);
    if (rv != BCM_E_NONE)
    {
      printf("unit:%d, Error with bcm_port_control_set(FloodBroadcastGroup): rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
      return -1;
    }

    printf("unit:%d, ptin_hapi_bridgeVlan_flood_set for LIF 0x%x returned success\n",unit, lif_id);
  }

  /* If VLAN ID / VSI is specified */
  if (vlanId != 0)
  {
    /* Get current control definitions for this vlan */
    bcm_vlan_control_vlan_t_init(&control);

    rv = bcm_vlan_control_vlan_get(unit, vlanId, &control);
    if (rv != BCM_E_NONE)
    {
      printf("unit:%d, Error getting vlan control structure! rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
      return -1;
    }

    /* Associate a MC group */
    control.broadcast_group         = mcgroup_flood_bc;
    control.unknown_multicast_group = mcgroup_flood_unkn_mc;
    control.unknown_unicast_group   = mcgroup_flood_unkn_uc;

    /* Apply new control definitions to this vlan */
    rv = bcm_vlan_control_vlan_set(unit, vlanId, control);
    if (rv != BCM_E_NONE)
    {
      printf("unit:%d, Error with bcm_vlan_control_vlan_set: rv=%d (%s)\n",unit, rv, bcm_errmsg(rv));
      return -1;
    }
    printf("unit:%d, ptin_hapi_bridgeVlan_flood_set for VSI %u returned success\n",unit, vlanId);
  }

  return 0;
}

