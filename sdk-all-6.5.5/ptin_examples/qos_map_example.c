int pcp_in_map_id  = -1;
int pcp_out_map_id = -1;

bcm_qos_map_t qos_l2_map;
int port, idx;

/* Configure a PCP map to be applied to IVE/EVE */
print bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_L2_VLAN_PCP, &pcp_in_map_id);
print bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_L2_VLAN_PCP, &pcp_out_map_id);

bcm_qos_map_t_init(&qos_l2_map);
for (idx=0; idx<16; idx++)
{
  qos_l2_map.pkt_pri = idx >> 1; //qos_map_l2_pcp[idx];
  qos_l2_map.pkt_cfi = idx % 2;  //qos_map_l2_cfi[idx];
  qos_l2_map.int_pri = idx >> 1; //qos_map_l2_internal_pri[idx];
  qos_l2_map.color   = ((idx % 2) == 0) ? bcmColorGreen : bcmColorYellow; //qos_map_l2_internal_color[idx];
  
  print bcm_qos_map_add(unit, BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP |BCM_QOS_MAP_L2_UNTAGGED, &qos_l2_map, pcp_in_map_id);
  print bcm_qos_map_add(unit, BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP |BCM_QOS_MAP_L2_UNTAGGED, &qos_l2_map, pcp_out_map_id);
  //print bcm_qos_map_add(unit, BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_L2_VLAN_PCP, &qos_l2_map, pcp_in_map_id);
  //print bcm_qos_map_add(unit, BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_L2_VLAN_PCP, &qos_l2_map, pcp_out_map_id);
}

print bcm_qos_port_map_set(0, 0x44801000, pcp_in_map_id, pcp_out_map_id);
print bcm_qos_port_map_set(0, 0x44801001, pcp_in_map_id, pcp_out_map_id);
print bcm_qos_port_map_set(0, 0x44801000, pcp_in_map_id, pcp_out_map_id);

print bcm_switch_control_port_set(0, 9, bcmSwitchColorSelect, BCM_COLOR_OUTER_CFI);
print bcm_switch_control_port_set(0, 10, bcmSwitchColorSelect, BCM_COLOR_OUTER_CFI);
print bcm_switch_control_port_set(0, 11, bcmSwitchColorSelect, BCM_COLOR_OUTER_CFI);
print bcm_port_cfi_color_set(0,-1,0,bcmColorGreen);
print bcm_port_cfi_color_set(0,-1,1,bcmColorYellow);

for (idx=0; idx<8; idx++)
{
	print bcm_port_vlan_priority_map_set(0, 0x44801000, idx /*priority*/, 0 /*cfi*/, idx /*internal priority*/, bcmColorGreen /*color*/);
	print bcm_port_vlan_priority_map_set(0, 0x44801001, idx /*priority*/, 0 /*cfi*/, idx /*internal priority*/, bcmColorGreen /*color*/);
	print bcm_port_vlan_priority_map_set(0, 0x44801002, idx /*priority*/, 0 /*cfi*/, idx /*internal priority*/, bcmColorGreen /*color*/);
}

/* Tell Dune to Drop Red packets */
print bcm_cosq_discard_set(0, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_COLOR_BLACK);
