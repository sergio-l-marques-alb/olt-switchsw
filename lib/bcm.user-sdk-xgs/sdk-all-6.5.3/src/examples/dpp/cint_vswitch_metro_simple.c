 /*
 *  Diagram:                    
 *  |               -----------                        |
 *  |              /  \   /\   \                       |
 *  |        CP1  |    \  /     |   SP1        		   |
 *  |      Y C20  |     \/      |   S300,C21 Y         |
 *  |      R C30  |     /\      |   S300,C30 R         |
 *  |        CP2  |    /  \     |   SP2                |             
 *  |      G C10  |   \/   \    |   S300,C21 Y         |
 *  |              \  VSwitch  /    S300,C10 G         | 
 *  |               -----------             		   |
 *  Simple VSWITCH example, where we have 4 ports (SP1,SP2, CP1,CP2) 
 *  3 flooding groups (R,G,Y) and 7 Attachment Circuts (C20, C30,...)
 *
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../
 * cint ../../src/examples/dpp/utility/cint_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/dpp/cint_vswitch_metro_simple.c
 * cint
 * vswitch_metro_simple_run(0,200,201,202,203);
 * exit;
 *
 * tx 1 psrc=200 data=0x000000000011000074ddb68f8100012c91000015ffff
 *
 * /* Received packets on unit 0 should be: */
 * /* 0x000000000011000074ddb68f81000014ffff0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 */
 * /* 0x000000000011000074ddb68f8100012c91000015ffff0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 */
 *
 */
 
struct vswitch_metro_simple_s {
    int vswitch_start; 
    int vlan_ports[7];
    int vswitchs[3];
    int multicasts[3];
    bcm_vlan_action_set_t action;
    int multicast_vswitch;
};

/*                 vswitch_start | vlan_ports |       */
vswitch_metro_simple_s g_vswitch_metro_simple = {4096,          {0}, 
/*                 vswitchs  | multicasts */
                   {0},           {0},
/*                 action  | multicast_vswitch */
                   {0},           0
};

/* Create a C-port that recognize C-VLANs only */
int vswitch_metro_simple_create_c_port(int unit, int port) 
{
  int rv;

  /* Port TPIDs */
  rv = port_tpid_init(port,1,0);
  if (rv != BCM_E_NONE) {
      printf("Error, port_tpid_init\n");
      return rv;
  }
  rv = port_tpid_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, port_tpid_set\n");
      return rv;
  }
  /* VLAN Domains */
  rv = bcm_port_class_set(unit, port, bcmPortClassId, port);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_class_set\n");
      return rv;
  }
  /* Same interface */
  rv = bcm_port_control_set(unit,port,bcmPortControlBridge,1);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_control_set\n");
      return rv;
  }
  return rv;
}

/* Create a S-port that recognize S-VLAN, C-VLAN onlys*/
int vswitch_metro_simple_create_s_port(int unit, int port) 
{
  int rv;

  /* Port TPIDs */
  rv = port_tpid_init(port,1,1);
  if (rv != BCM_E_NONE) {
      printf("Error, port_tpid_init\n");
      return rv;
  }
  rv = port_tpid_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, port_tpid_set\n");
      return rv;
  }
  /* VLAN Domains */
  rv = bcm_port_class_set(unit, port, bcmPortClassId, port);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_class_set\n");
      return rv;
  }
  /* Same interface */
  rv = bcm_port_control_set(unit,port,bcmPortControlBridge,1);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_control_set\n");
      return rv;
  }

  return rv;
}

/* Global Configuration */
int vswitch_metro_simple_global(int unit) {
  bcm_vlan_action_set_t_init(&g_vswitch_metro_simple.action);
  g_vswitch_metro_simple.action.dt_outer = bcmVlanActionDelete;
  g_vswitch_metro_simple.action.dt_inner = bcmVlanActionDelete;
  g_vswitch_metro_simple.action.ot_outer = bcmVlanActionDelete;

  return 0;
}

/* Main function to run */
int vswitch_metro_simple_run(int unit, int s_port1, int s_port2, int c_port1, int c_port2) {

  int i;
  int rv;
  
  /* Global */
  rv = vswitch_metro_simple_global(unit);
  if (rv != BCM_E_NONE) {
    printf("Error, global\n");
    return rv;
  }

  /* Port */
  rv = vswitch_metro_simple_create_c_port(unit, c_port1);
  if (rv != BCM_E_NONE) {
    printf("Error, create_c_port\n");
    return rv;
  }
  rv = vswitch_metro_simple_create_c_port(unit, c_port2);
  if (rv != BCM_E_NONE) {
    printf("Error, create_c_port\n");
    return rv;
  }
  rv = vswitch_metro_simple_create_s_port(unit, s_port1);
  if (rv != BCM_E_NONE) {
    printf("Error, create_s_port\n");
    return rv;
  }
  rv = vswitch_metro_simple_create_s_port(unit, s_port2);
  if (rv != BCM_E_NONE) {
    printf("Error, create_s_port\n");
    return rv;
  }

  /* Vswitch */
  for (i = 0; i < 3; i++)
  {
    rv = vswitch_metro_simple_create_vswitch(unit, &(g_vswitch_metro_simple.vswitchs[i]),&(g_vswitch_metro_simple.multicasts[i]));
    if (rv != BCM_E_NONE) {
        printf("Error, create_vswitch\n");
        return rv;
    }
  }

  /* Create VLAN-Ports */
  rv = vswitch_metro_simple_create_s_c_vlan_port(unit, s_port1, 300, 21, &(g_vswitch_metro_simple.vlan_ports[0]));
  if (rv != BCM_E_NONE) {
    printf("Error, create_s_c_vlan_port\n");
    return rv;
  }
  rv = vswitch_metro_simple_create_s_c_vlan_port(unit, s_port1, 300, 30, &(g_vswitch_metro_simple.vlan_ports[1]));
  if (rv != BCM_E_NONE) {
    printf("Error, create_s_c_vlan_port\n");
    return rv;
  }
  rv = vswitch_metro_simple_create_s_c_vlan_port(unit, s_port2, 300, 21, &(g_vswitch_metro_simple.vlan_ports[2]));
  if (rv != BCM_E_NONE) {
    printf("Error, create_s_c_vlan_port\n");
    return rv;
  }
  rv = vswitch_metro_simple_create_s_c_vlan_port(unit, s_port2, 300, 10, &(g_vswitch_metro_simple.vlan_ports[3]));
  if (rv != BCM_E_NONE) {
    printf("Error, create_s_c_vlan_port\n");
    return rv;
  }
  rv = vswitch_metro_simple_create_c_vlan_port(unit, c_port1, 20, &(g_vswitch_metro_simple.vlan_ports[4]));
  if (rv != BCM_E_NONE) {
    printf("Error, create_s_c_vlan_port\n");
    return rv;
  }
  rv = vswitch_metro_simple_create_c_vlan_port(unit, c_port1, 30, &(g_vswitch_metro_simple.vlan_ports[5]));
  if (rv != BCM_E_NONE) {
    printf("Error, create_s_c_vlan_port\n");
    return rv;
  }
  rv = vswitch_metro_simple_create_c_vlan_port(unit, c_port2, 10, &(g_vswitch_metro_simple.vlan_ports[6]));
  if (rv != BCM_E_NONE) {
    printf("Error, create_s_c_vlan_port\n");
    return rv;
  }

  /* Associate VLAN-port to VSWITCH and add to flooding group */
  /* Yellow */
  rv = vswitch_metro_simple_associate_vlan_port_to_vsi(unit, g_vswitch_metro_simple.vswitchs[0], g_vswitch_metro_simple.multicasts[0], s_port1, g_vswitch_metro_simple.vlan_ports[0]);
  if (rv != BCM_E_NONE) {
    printf("Error, associate_vlan_port_to_vsi\n");
    return rv;
  }
  rv = vswitch_metro_simple_associate_vlan_port_to_vsi(unit, g_vswitch_metro_simple.vswitchs[0], g_vswitch_metro_simple.multicasts[0], s_port2, g_vswitch_metro_simple.vlan_ports[2]);
  if (rv != BCM_E_NONE) {
    printf("Error, associate_vlan_port_to_vsi\n");
    return rv;
  }
  rv = vswitch_metro_simple_associate_vlan_port_to_vsi(unit, g_vswitch_metro_simple.vswitchs[0], g_vswitch_metro_simple.multicasts[0], c_port1, g_vswitch_metro_simple.vlan_ports[4]);
  if (rv != BCM_E_NONE) {
      printf("Error, associate_vlan_port_to_vsi\n");
      return rv;
  }
  /* Red */
  rv =  vswitch_metro_simple_associate_vlan_port_to_vsi(unit, g_vswitch_metro_simple.vswitchs[1], g_vswitch_metro_simple.multicasts[1], s_port1, g_vswitch_metro_simple.vlan_ports[1]);
  if (rv != BCM_E_NONE) {
    printf("Error, associate_vlan_port_to_vsi\n");
    return rv;
  }
  rv = vswitch_metro_simple_associate_vlan_port_to_vsi(unit, g_vswitch_metro_simple.vswitchs[1], g_vswitch_metro_simple.multicasts[1], c_port1, g_vswitch_metro_simple.vlan_ports[5]);
  if (rv != BCM_E_NONE) {
    printf("Error, associate_vlan_port_to_vsi\n");
    return rv;
  }
  /* Green */
  rv = vswitch_metro_simple_associate_vlan_port_to_vsi(unit, g_vswitch_metro_simple.vswitchs[2], g_vswitch_metro_simple.multicasts[2], s_port2, g_vswitch_metro_simple.vlan_ports[3]);
  if (rv != BCM_E_NONE) {
    printf("Error, associate_vlan_port_to_vsi\n");
    return rv;
  }
  rv = vswitch_metro_simple_associate_vlan_port_to_vsi(unit, g_vswitch_metro_simple.vswitchs[2], g_vswitch_metro_simple.multicasts[2], c_port2, g_vswitch_metro_simple.vlan_ports[6]);
  if (rv != BCM_E_NONE) {
    printf("Error, associate_vlan_port_to_vsi\n");
    return rv;
  }

  return rv;
}

/* Create VSWITCH */
int vswitch_metro_simple_create_vswitch(int unit, int* vswitch, int* multicast_id) 
{
  int rv;
  g_vswitch_metro_simple.multicast_vswitch = g_vswitch_metro_simple.vswitch_start;
  /* Create MC-ID */
  rv = multicast__open_mc_group(unit,&g_vswitch_metro_simple.multicast_vswitch,0);
  if (rv != BCM_E_NONE) {
      printf("Error, multicast__open_mc_group\n");
      return rv;
  }
  /* Create VSI */
  rv = vlan__open_vlan_per_mc(unit,g_vswitch_metro_simple.vswitch_start,g_vswitch_metro_simple.multicast_vswitch);
  if (rv != BCM_E_NONE) {
      printf("Error, vlan__open_vlan_per_mc\n");
      return rv;
  }
  *vswitch = g_vswitch_metro_simple.vswitch_start;
  *multicast_id = g_vswitch_metro_simple.multicast_vswitch;
  g_vswitch_metro_simple.vswitch_start++;

  return rv;
}

/* Create VLAN port that recognize S-C VLANs , working in untagged canonical format */
int vswitch_metro_simple_create_s_c_vlan_port(int unit, int port, int s_vlan, int c_vlan, int* vlan_port_id) 
{
  int rv;
  bcm_vlan_port_t vlan_port;
  bcm_vlan_port_t_init(&vlan_port);
  /* Create S-VLANs x C-VLANs */
  vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;    
  vlan_port.port = port;
  vlan_port.match_vlan = s_vlan;
  vlan_port.match_inner_vlan = c_vlan;
  vlan_port.egress_vlan = s_vlan;
  vlan_port.egress_inner_vlan = c_vlan;
  rv = bcm_vlan_port_create(unit, &vlan_port);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_vlan_port_create\n");
      return rv;
  }
  *vlan_port_id = vlan_port.vlan_port_id;
  /* IVE */
  rv = bcm_vlan_translate_action_create(unit, *vlan_port_id, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_INVALID, &g_vswitch_metro_simple.action);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_vlan_translate_action_create\n");
      return rv;
  }

  return rv;
}

/* Create VLAN-Port C-VLAN */
int vswitch_metro_simple_create_c_vlan_port(int unit, int port, int c_vlan, int* vlan_port_id) 
{
  int rv;
  bcm_vlan_port_t vlan_port;
  bcm_vlan_port_t_init(&vlan_port);
  /* Create S-VLANs x C-VLANs */
  vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;    
  vlan_port.port = port;
  vlan_port.match_vlan = c_vlan;
  vlan_port.egress_vlan = c_vlan;
  rv = bcm_vlan_port_create(unit, &vlan_port);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_vlan_port_create\n");
      return rv;
  }
  *vlan_port_id = vlan_port.vlan_port_id;
  /* IVE */
  rv = bcm_vlan_translate_action_create(unit, *vlan_port_id, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_INVALID, &g_vswitch_metro_simple.action);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_vlan_translate_action_create\n");
      return rv;
  }

  return rv;
}

/* Associate VLAN-Port to VSWITCH */
int vswitch_metro_simple_associate_vlan_port_to_vsi(int unit, int vswitch, int multicast_id, int port, int vlan_port_id) 
{
  int rv;
  rv = bcm_vswitch_port_add(unit, vswitch,vlan_port_id);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_vswitch_port_add\n");
      return rv;
  }
  rv = multicast__vlan_encap_add(unit,multicast_id,port,vlan_port_id);
  if (rv != BCM_E_NONE) {
      printf("Error, multicast__vlan_encap_add\n");
      return rv;
  }
  return rv;
}

