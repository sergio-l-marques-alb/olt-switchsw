/*
*  C Implementation: switch_flow
*
* Description: 
*
*
* Author: Milton Ruas,,, <mruas@mruas-laptop>, (C) 2010
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include <bcm/port.h>
#include <bcm/link.h>
#include <bcm/stg.h>
#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/stat.h>
#include <bcm/policer.h>
#include <bcm/field.h>
#include <bcm/trunk.h>
#include <bcm/qos.h>
#include <bcm/switch.h>
#include <bcm/stack.h>
#include <bcm/l2.h>

#include <dapi_struct.h>
#include <broad_l2_lag.h>
#include <broad_common.h>
#include <broad_debug.h>
#include <broad_group_xgs3.h>
#include "ptin_hapi.h"

/*
 * Board specific initialization code
 * used by the board definitions below.
 */
#include "l7_common.h"
#include "hpc_db.h"
#include "broad_hpc_db.h"
#include "sal/appl/config.h"
#include "broad_policy.h"

#include <stdio.h>
#include <string.h>

// Tags
#define CTAG_TPID 0x8100
#define STAG_TPID 0x88A8

//#define FLOWS_MAX          128

#define FPENTRIES_MAX_IN_A_GROUP  128
#define FPGROUP_START             8
#define FPGROUPS_MAX              1
#define FPENTRY_START             ((FPGROUP_START-1)*FPENTRIES_MAX_IN_A_GROUP+1)
#define FPENTRIES_MAX             ((uint16) FPGROUPS_MAX*FPENTRIES_MAX_IN_A_GROUP)

#define LOW_PRIORITY    0
#define NORMAL_PRIORITY 1
#define HIGH_PRIORITY   2

uint16 rules_available = FPGROUPS_MAX*FPENTRIES_MAX_IN_A_GROUP;

#define XLATE_MAX_ENTRIES 768
uint16 xlate_ingress_stag_available_entries = XLATE_MAX_ENTRIES;
uint16 xlate_ingress_dtag_available_entries = XLATE_MAX_ENTRIES;
uint16 xlate_egress_stag_available_entries = XLATE_MAX_ENTRIES;


st_ptin_vlan_defs ptin_vlan_defs = { 1, 1};

int ptin_hapi_unit=0;
DAPI_USP_t  usp_map[SYSTEM_N_PORTS];

typedef struct {
  int     id;
  uint16  cvid;
} st_fpentry;

//typedef struct {
//  uint8       inUse;
//  st_HwEthernetMef10CeVidMap port[SYSTEM_N_PORTS];
//  uint16      vid_xlate[SYSTEM_N_PORTS];
//  uint16      vid_root;
//  uint8       number_of_roots, number_of_leafs;
//  int         fpentry_id[CLIENTS_MAX];
//} st_flow_struct;
//
//st_flow_struct flow_database[FLOWS_MAX];
//
//uint8 vlan_id_usage[4096];


struct_fpentry_data fpentry_data[FPENTRIES_MAX];

// List of flow counters
st_ptin_fcounters ptin_fcounters[FLOW_COUNTERS_MAX];

// Semaphore for fcounters access
void *sem_fcounters_access=L7_NULL;

/* FPGA map */
volatile st_fpga_map *fpga_map=MAP_FAILED;

static L7_RC_t ptin_systemPolicies(void);

int hapi_ptin_get_available_rules(void)
{
  return rules_available;
}

int hapi_ptin_get_available_vlanXlate_entries(enum_ptin_vlan_xlate_entrytype entryType)
{
  int ret;

  switch ((uint8) entryType) {
    case ptin_vlan_xlate_ingress_stag:
      ret = xlate_ingress_stag_available_entries;
      break;

    case ptin_vlan_xlate_ingress_dtag:
      ret = xlate_ingress_dtag_available_entries;
      break;

    case ptin_vlan_xlate_egress_stag:
      ret = xlate_egress_stag_available_entries;
      break;

    default:
      ret = 0;
  }

  return ret;
}

L7_RC_t hapi_ptin_get_hw_resources(st_ptin_hw_resources *resources)
{
  uint8 group_id;

  for (group_id=0; group_id<FPGROUP_START-1; group_id++)
  {
    resources->ffp_available_rules[group_id] = policy_group_available_rules(0, group_id);
    //printf("%s(%d) ffp_available_rules[%u]=%u\r\n",__FILE__,__LINE__,group_id,resources->ffp_available_rules[group_id]);
  }
  resources->ffp_available_rules[FPGROUP_START-1] = rules_available;

  resources->vlanXlate_available_entries.ing_stag = hapi_ptin_get_available_vlanXlate_entries(ptin_vlan_xlate_ingress_stag);
  resources->vlanXlate_available_entries.ing_dtag = hapi_ptin_get_available_vlanXlate_entries(ptin_vlan_xlate_ingress_dtag);
  resources->vlanXlate_available_entries.egr_stag = hapi_ptin_get_available_vlanXlate_entries(ptin_vlan_xlate_egress_stag);

  resources->vlans_available.igmp = hapiBroadPtinIgmp_numberOfAvailableVlans();
  resources->vlans_available.dhcp = hapiBroadPtinDhcp_numberOfAvailableVlans();
  resources->vlans_available.bcastLim = hapiBroadPtinBCastLim_numberOfAvailableVlans();

  resources->flowCounters_available_entries = hapiBroadPtinFlowCounters_numberOfAvailableEntries();

//printf("%s(%d) vlanXlate_ing_stag_available_entries=%u\r\n",__FILE__,__LINE__,resources->vlanXlate_ing_stag_available_entries);
//printf("%s(%d) vlanXlate_ing_dtag_available_entries=%u\r\n",__FILE__,__LINE__,resources->vlanXlate_ing_dtag_available_entries);
//printf("%s(%d) vlanXlate_egr_stag_available_entries=%u\r\n",__FILE__,__LINE__,resources->vlanXlate_egr_stag_available_entries);

  return L7_SUCCESS;
}

//#define FLOW_COMPARE_DIFFERENT          0
//#define FLOW_COMPARE_PORT_STAG_MISMATCH 1
//#define FLOW_COMPARE_EQUAL              2
//
//static int flow_compare(uint16 *flow_id, st_HwEthernetMef10CeVidMap *port);
//static int flow_add_translation_entries(uint8 port, uint16 old_vid, uint16 new_vid);
//static int flow_remove_translation_entries(uint16 flow_id, st_HwEthernetMef10CeVidMap *port);
//static int flow_add_translation_entries(uint16 flow_id, st_HwEthernetMef10CeVidMap *port);

//void l2_addr_callback(int unit, bcm_l2_addr_t *l2addr, int operation, void *userdata);

L7_RC_t hapi_ptin_vlan_defs(int create_vlan_1, int include_cpu_intf)
{
  ptin_vlan_defs.create_vlan_1 = create_vlan_1 & 1;
  ptin_vlan_defs.include_cpu_intf = include_cpu_intf & 1;

  return L7_SUCCESS;
}


L7_RC_t hapi_ptin_vlan_translate(int operation, int port, uint16 vlan_old, uint16 *vlan_new)
{
  int unit;
  int prio;
  bcm_error_t status=BCM_E_NONE;

  // Validate arguments
  if (port<0 || port>=SYSTEM_N_PORTS || vlan_old>4095 || vlan_new==NULL)
    return L7_FAILURE;

  // Translate port
  unit = usp_map[port].unit;
  port = (int) usp_map[port].port;

  switch (operation) {

    case VLAN_XLATE_OPER_GET_INGRESS_ST:
      status=bcm_vlan_translate_get(unit,port,vlan_old,vlan_new,&prio);
      break;

    case VLAN_XLATE_OPER_GET_INGRESS_DT:
      status=bcm_vlan_dtag_get(unit,port,vlan_old,vlan_new,&prio);
      break;

    case VLAN_XLATE_OPER_GET_EGRESS_ST:
      status=bcm_vlan_translate_egress_get(unit,port,vlan_old,vlan_new,&prio);
      break;

    case VLAN_XLATE_OPER_ADD_INGRESS_ST:
      if ((status=bcm_vlan_translate_add(unit,port,vlan_old,*vlan_new,-1))==BCM_E_NONE)
      {
        if (xlate_ingress_stag_available_entries>0)  xlate_ingress_stag_available_entries--;
      }
      break;

    case VLAN_XLATE_OPER_ADD_INGRESS_DT:
      if ((status=bcm_vlan_dtag_add(unit,port,vlan_old,*vlan_new,-1))==BCM_E_NONE)
      {
        if (xlate_ingress_dtag_available_entries>0)  xlate_ingress_dtag_available_entries--;
      }
      break;
      
    case VLAN_XLATE_OPER_ADD_EGRESS_ST:
      if ((status=bcm_vlan_translate_egress_add(unit,port,vlan_old,*vlan_new,-1))==BCM_E_NONE)
      {
        if (xlate_egress_stag_available_entries>0)  xlate_egress_stag_available_entries--;
      }
      break;

    case VLAN_XLATE_OPER_DEL_INGRESS_ST:
      if ((status=bcm_vlan_translate_delete(unit,port,vlan_old))==BCM_E_NONE)
      {
        xlate_ingress_stag_available_entries++;
      }
      break;
  
    case VLAN_XLATE_OPER_DEL_INGRESS_DT:
      if ((status=bcm_vlan_dtag_delete(unit,port,vlan_old))==BCM_E_NONE)
      {
        xlate_ingress_dtag_available_entries++;
      }
      break;
  
    case VLAN_XLATE_OPER_DEL_EGRESS_ST:
      if ((status=bcm_vlan_translate_egress_delete(unit,port,vlan_old))==BCM_E_NONE)
      {
        xlate_egress_stag_available_entries++;
      }
      break;

    case VLAN_XLATE_OPER_DELALL_INGRESS_ST:
      if ((status=bcm_vlan_translate_delete_all(unit))==BCM_E_NONE)
      {
        xlate_ingress_stag_available_entries=XLATE_MAX_ENTRIES;
      }
      break;
  
    case VLAN_XLATE_OPER_DELALL_INGRESS_DT:
      if ((status=bcm_vlan_dtag_delete_all(unit))==BCM_E_NONE)
      {
        xlate_ingress_dtag_available_entries=XLATE_MAX_ENTRIES;
      }
      break;
  
    case VLAN_XLATE_OPER_DELALL_EGRESS_ST:
      if ((status=bcm_vlan_translate_egress_delete_all(unit))==BCM_E_NONE)
      {
        xlate_egress_stag_available_entries=XLATE_MAX_ENTRIES;
      }
      break;

    case VLAN_XLATE_OPER_DELALL_INGRESS:
      if ((status=bcm_vlan_translate_delete_all(unit))==BCM_E_NONE)
      {
        xlate_ingress_stag_available_entries=XLATE_MAX_ENTRIES;
        if ((status=bcm_vlan_dtag_delete_all(unit))==BCM_E_NONE)
        {
          xlate_ingress_dtag_available_entries=XLATE_MAX_ENTRIES;
        }
      }
      break;

    case VLAN_XLATE_OPER_DELALL_EGRESS:
      if ((status=bcm_vlan_translate_egress_delete_all(unit))==BCM_E_NONE)
      {
        xlate_egress_stag_available_entries=XLATE_MAX_ENTRIES;
      }
      break;

    case VLAN_XLATE_OPER_DELETE_ALL:
      if ((status=bcm_vlan_translate_delete_all(unit))==BCM_E_NONE)
      {
        xlate_ingress_stag_available_entries=XLATE_MAX_ENTRIES;
        if ((status=bcm_vlan_dtag_delete_all(unit))==BCM_E_NONE)
        {
          xlate_ingress_dtag_available_entries=XLATE_MAX_ENTRIES;
          if ((status=bcm_vlan_translate_egress_delete_all(unit))==BCM_E_NONE)
          {
            xlate_egress_stag_available_entries=XLATE_MAX_ENTRIES;
          }
        }
      }
      break;

    default:
      status = L7_FAILURE;
  }

  // Resources error
  if (status==BCM_E_RESOURCE)
    return L7_TABLE_IS_FULL;

  // Other errors
  if (status!=BCM_E_NONE)
    return L7_FAILURE;

  // Success
  return L7_SUCCESS;
}

L7_RC_t hapi_ptin_flow_init(void)
{
  bcm_field_qset_t qset;
  bcm_field_group_t group_id;
  int i, status=0;
  int    portIndex, portNo, maxPorts;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;

  // Initialize Field Processor module
  printf("hapi_ptin_flow_init: hapi_ptin_flow_init: %d\n",status);

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));

  dapiCardPtr = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr = dapiCardPtr->slotMap;

  // Maximum number of ports
  if ((maxPorts=sysapiHpcCardInfoPtr->numOfNiPorts)>SYSTEM_N_PORTS)  maxPorts=SYSTEM_N_PORTS;

  printf("%s(%d) Max number of ports=%u\r\n", __FUNCTION__, __LINE__,maxPorts);

  for (portIndex = 0; portIndex < maxPorts; portIndex++)
  {
    portNo   = hapiSlotMapPtr[portIndex].portNum;

    usp_map[portNo].slot = hapiSlotMapPtr[portIndex].slotNum;
    usp_map[portNo].unit = hapiSlotMapPtr[portIndex].bcm_cpuunit;
    usp_map[portNo].port = hapiSlotMapPtr[portIndex].bcm_port;
    printf("%s(%d) PortIndex=%u PortNo=%u => remapped to %u\r\n", __FUNCTION__, __LINE__,portIndex,portNo,hapiSlotMapPtr[portIndex].bcm_port);
  }
  for (portIndex = maxPorts; portIndex < SYSTEM_N_PORTS; portIndex++)
  {
    portNo   = hapiSlotMapPtr[portIndex].portNum;

    usp_map[portNo].slot = usp_map[0].slot;
    usp_map[portNo].unit = usp_map[0].unit;
    usp_map[portNo].port = portIndex;
  }

  ptin_hapi_unit = usp_map[0].unit;

  // Clear database
//memset(flow_database,0,sizeof(st_flow_struct)*FLOWS_MAX);
  memset(fpentry_data,0,sizeof(struct_fpentry_data)*FPENTRIES_MAX);
  for (i=0; i<FPENTRIES_MAX; i++) {
    fpentry_data[i].entry_index = i;
  }
//memset(vlan_id_usage,0,sizeof(uint8)*4096);
//vlan_id_usage[0]=1;
//vlan_id_usage[1]=1;
//vlan_id_usage[4095]=1;

  // Initialize qualifiers to be used
  memset(&qset, 0, sizeof(bcm_field_qset_t));

  BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyStage);
  BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyDrop);
  BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyInPorts);
  //BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyDstMac);
  //BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyOutPorts);
  //BCM_FIELD_QSET_ADD(qset,bcmFieldQualifySrcPort);
  BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyOuterVlan);
  BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyOuterVlanId);
  BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyInnerVlan);
  BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyInnerVlanId);
  //BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyInnerVlanPri);
  BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyEtherType);
  BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyIpType);
  //BCM_FIELD_QSET_ADD(qset,bcmFieldQualifySrcIp6);
  BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyDstIp6);

  printf("%s(%d) Contents:\r\n", __FUNCTION__, __LINE__);
  int k;
  for (k=0; k<_SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX); k++)
  {
      printf("%08X ",qset.w[k]);
  }
  printf("\r\n");
  for (k=0; k<_SHR_BITDCLSIZE(BCM_FIELD_USER_NUM_UDFS); k++)
  {
      printf("%08X ",qset.udf_map[k]);
  }
  printf("\r\n");

  // Create groups
  for (group_id=FPGROUP_START; group_id<FPGROUP_START+FPGROUPS_MAX; group_id++)  {

    if ((status=bcm_field_group_create_mode_id(ptin_hapi_unit, qset, BCM_FIELD_GROUP_PRIO_ANY, bcmFieldGroupModeDouble, group_id))!=0)
    //if ((status=bcm_field_group_create_id(ptin_hapi_unit,qset,BCM_FIELD_GROUP_PRIO_ANY,group_id))!=0)
    {
      printf("hapi_ptin_flow_init: error creating field group %u \"%s\"\n",group_id,bcm_errmsg(status));
      //return L7_FAILURE;
    }
    else  {
      printf("hapi_ptin_flow_init: bcm_field_group_create_id(0,field_qset,BCM_FIELD_GROUP_PRIO_ANY,%d)\n",group_id);
    }
  }

  // Initialize list of flow counters
  memset(ptin_fcounters,0x00,sizeof(st_ptin_fcounters)*FLOW_COUNTERS_MAX);

/*
  // Entry to copy pbits to internal priority: not necessary using fastpath! 
  // Create fp entry only if fpentry is not created
  if (bcm_field_entry_create_id(ptin_hapi_unit,FPGROUP_START,FPENTRY_START))  {
    printf("hapi_ptin_flow_init: cannot create (fpentry=%d)\n",FPENTRY_START);
    return L7_FAILURE;
  }
  // Define entry priority
  if (bcm_field_entry_prio_set(ptin_hapi_unit,FPENTRY_START,LOW_PRIORITY))  {
    printf("hapi_ptin_flow_init: error with bcm_field_entry_prio_set (fpentry=%d)\n",FPENTRY_START);
    return L7_FAILURE;
  }
  // Define qualifiers
  if (bcm_field_qualify_Stage(ptin_hapi_unit,FPENTRY_START,bcmFieldStageDefault))  {
    printf("hapi_ptin_flow_init: error with bcm_field_qualify_Stage (fpentry=%d)\n",FPENTRY_START);
    status=bcm_field_entry_destroy(ptin_hapi_unit,FPENTRY_START);
    if (status==BCM_E_NONE) rules_available++;
    return L7_FAILURE;
  }
  if (bcm_field_qualify_Drop(ptin_hapi_unit,FPENTRY_START,0,1))  {
    printf("hapi_ptin_flow_init: error with bcm_field_qualify_drop (fpentry=%d)\n",FPENTRY_START);
    status=bcm_field_entry_destroy(ptin_hapi_unit,FPENTRY_START);
    if (status==BCM_E_NONE) rules_available++;
    return L7_FAILURE;
  }

//if (bcm_field_qualify_InnerVlanPri(ptin_hapi_unit,FPENTRY_START,3,0x7))  {
//  printf("hapi_ptin_flow_init: error with bcm_field_qualify_drop (fpentry=%d)\n",FPENTRY_START);
//  status=bcm_field_entry_destroy(ptin_hapi_unit,FPENTRY_START); 
//  if (status==BCM_E_NONE) rules_available++; 
//  return L7_FAILURE;
//}

  // Define actions
  if (bcm_field_action_add(ptin_hapi_unit,FPENTRY_START,bcmFieldActionPrioPktAndIntCopy,0,0))  {
    printf("hapi_ptin_flow_init: error with bcm_field_action_add (fpentry=%d)\n",FPENTRY_START);
    status=bcm_field_entry_destroy(ptin_hapi_unit,FPENTRY_START);
    if (status==BCM_E_NONE) rules_available++;
    return L7_FAILURE;
  }

  // (Re)Install entry
  if (bcm_field_entry_install(ptin_hapi_unit,FPENTRY_START)) {
    printf("hapi_ptin_flow_init: error installing (fpentry=%d):\n",FPENTRY_START);
    status=bcm_field_entry_destroy(ptin_hapi_unit,FPENTRY_START);
    if (status==BCM_E_NONE) rules_available++;
    return L7_FAILURE;
  }

  // Store data
  fpentry_data[0].stag.vid_value = 0;
  fpentry_data[0].stag.vid_mask  = 0;
  fpentry_data[0].ctag.vid_value = 0;
  fpentry_data[0].ctag.vid_mask  = 0;
  fpentry_data[0].policer_id = 0;
  fpentry_data[0].inUse = 1;
*/
  //printf("hapi_ptin_flow_init: registering callback \"%s\"\n",bcm_errmsg(bcm_l2_addr_register(0,l2_addr_callback,NULL)));
  //bcm_l2_addr_register(ptin_hapi_unit,l2_addr_callback,NULL);

  if (ptin_systemPolicies()!=L7_SUCCESS)
  {
    printf("%s(%d) Error with ptin_systemPolicies\r\n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  printf("%s(%d) Success with ptin_systemPolicies\r\n",__FUNCTION__,__LINE__);

  return L7_SUCCESS;
}

int hapi_ptin_get_realPhysicalPort(int port)
{
  if (port>SYSTEM_N_PORTS) {
    return port;
  }

  return usp_map[port].port;
}


L7_RC_t hapi_ptin_phy_config_init(void)
{
  int error=0;
  uint8  port;
  TAddrMap fpga_AddrMap;

  // Load FPGA
  if ((fpga_map = (volatile st_fpga_map *) AddrAlloc((void *) &fpga_AddrMap, (int) 0x10000000, (long) sizeof(uint8)*0xFFFF))!=MAP_FAILED)
  {
    /* If FPGA id is not valid, free FPGA map */
    if (fpga_map->registo[FPGA_ID1_REG]==0x00 || fpga_map->registo[FPGA_ID0_REG]==0x00 ||
        fpga_map->registo[FPGA_ID1_REG]==0xff || fpga_map->registo[FPGA_ID0_REG]==0xff)  {
      AddrFree(&fpga_AddrMap);
      fpga_map = MAP_FAILED;
    }
    /* Otherwise, make some initializations */
    else
    {
      /* Release External PHYs RESET */
      fpga_map->registo[FPGA_EXTPHY_RESET]=0xFF;
      /* Disable all TX */
      fpga_map->registo[FPGA_TXDISABLE_REG]=0xFF;
    }
  }

//bcm_pbmp_t pbm={{0xffff}};
//bcm_vlan_port_remove(ptin_hapi_unit,1,pbm);
//bcm_l2_age_timer_set(ptin_hapi_unit,60);

  // Set MCAST_FLOOD_NONE flooding mode for Vlan 1
  error = bcm_vlan_mcast_flood_set(ptin_hapi_unit, 1, BCM_VLAN_MCAST_FLOOD_NONE);
  if (error) {
    printf("%s: bcm_vlan_mcast_flood_set failed: error \"%s\"\n", __FUNCTION__,bcm_errmsg(error));
  }

  for (port=0;port<SYSTEM_N_PORTS; port++) {

//  if ((error=bcm_port_encap_set(0,port,BCM_PORT_ENCAP_IEEE)))  {
//    printf("%s: bcm_port_encap_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
//  if ((error=bcm_port_interface_set(0,port,BCM_PORT_IF_GMII)))  {
//    printf("%s: bcm_port_interface_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
//  if ((error=bcm_linkscan_mode_set(0,port,BCM_LINKSCAN_MODE_SW)))  {
//    printf("%s: bcm_linkscan_mode_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
//  if ((error=bcm_port_speed_set(0,port,1000)))  {
//    printf("%s: bcm_port_speed_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
//  if ((error=bcm_port_autoneg_set(0,port,FALSE)))  {
//    printf("%s: bcm_port_autoneg_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
//  if ((error=bcm_port_duplex_set(0,port,TRUE)))  {
//    printf("%s: bcm_port_duplex_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
//  if ((error=bcm_port_pause_set(0,port,FALSE,FALSE)))  {
//    printf("%s: bcm_port_pause_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
//  if (bcm_port_frame_max_set(0,port,2048))  {
//    printf("bcm_port_frame_max_set failed in port %u\n",port);
//    return L7_FAILURE;
//  }
//  if ((error=bcm_port_stp_set(0,port,BCM_STG_STP_FORWARD)))  {
//    printf("%s: bcm_port_stp_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
//
//  // All ports are UNI's
//  if ((error=bcm_port_dtag_mode_set(usp_map[port].unit,port,BCM_PORT_DTAG_MODE_INTERNAL)))  {
//    printf("%s: bcm_port_dtag_mode_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
//  // Outer tag is an S-tag
//  if ((error=bcm_port_tpid_set(usp_map[port].unit,port,STAG_TPID)))  {
//    printf("%s: bcm_port_tpid_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
    // Inner tag is a C-tag
    if ((error=bcm_port_inner_tpid_set(usp_map[port].unit,port,CTAG_TPID)))  {
      printf("%s: bcm_port_inner_tpid_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
      return L7_FAILURE;
    }
//
//  // No packets are discarded (classification based on tag)
//  if ((error=bcm_port_discard_set(usp_map[port].unit,port,BCM_PORT_DISCARD_NONE)))  {
//    printf("%s: bcm_port_discard_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
//  // Packets ingressing with no expected vlan are discarded
//  if ((error=bcm_port_vlan_member_set(usp_map[port].unit,port,BCM_PORT_VLAN_MEMBER_INGRESS/*| BCM_PORT_VLAN_MEMBER_EGRESS*/)))  {
//    printf("%s: bcm_port_vlan_member_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
//
//  // Default vlan for untagged packets
//  if ((error=bcm_port_untagged_vlan_set(usp_map[port].unit,port,1)))  {
//    printf("%s: bcm_port_vlan_member_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }
//
//  // Activar o MAC learning
//  if ((error=bcm_port_learn_set(usp_map[port].unit,port,BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD)))  {
//    printf("%s: bcm_port_learn_set failed in port %u: \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    //return L7_FAILURE;
//  }

    // Algumas definicoesµes default para as traducoes
    if ((error=bcm_vlan_control_port_set(usp_map[port].unit,port,bcmVlanTranslateIngressEnable,1)) ||
        (error=bcm_vlan_control_port_set(usp_map[port].unit,port,bcmVlanTranslateIngressMissDrop,1)) ||
        (error=bcm_vlan_control_port_set(usp_map[port].unit,port,bcmVlanTranslateEgressEnable,1)) ||
        (error=bcm_vlan_control_port_set(usp_map[port].unit,port,bcmVlanTranslateEgressMissDrop,1)))  {
      printf("%s: bcm_vlan_control_port_set failed in port %u: \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
      return L7_FAILURE;
    }

    // Maximum packet size for counting effects
    WRITE_GPORT_CNTMAXSIZEr(usp_map[port].unit,port,1518);

    if ((error=hapi_ptin_clearCounters(port)))  {
      printf("%s: hapi_ptin_clearCounters failed in port %u\n", __FUNCTION__,port);
      return L7_FAILURE;
    }

//  if ((error=bcm_port_enable_set(unit,port,FALSE))) {
//    printf("%s: bcm_port_enable_set failed in port %u: error \"%s\"\n", __FUNCTION__,port,bcm_errmsg(error));
//    return L7_FAILURE;
//  }

  }//for (i=unit;i<N_ETHs; i++)

  return(0);
}

// Control transmit disable in external phys
L7_RC_t hapi_ptin_phy_set(uint8 port, uint8 tx_disable)
{
  uint16 addr, value;

  // Validate port
  if (port>=SYSTEM_N_PORTS) {
    return L7_FAILURE;
  }
  // For non XE interfaces, do nothing
  if (port<16) {
    return L7_SUCCESS;
  }

  printf("%s(%d) unit=%u, port=%u, port2=%u\r\n",__FUNCTION__,__LINE__,usp_map[port].unit,port,usp_map[port].port);

  // Get port addr
  if ( soc_phy_cfg_addr_get( usp_map[port].unit, usp_map[port].port, 0, &addr) != SOC_E_NONE )
  {
    return L7_FAILURE;
  }

  printf("%s(%d) Addr=0x%X\r\n",__FUNCTION__,__LINE__,addr);

  // Get tx disable value
  if ( soc_miimc45_read( usp_map[port].unit, addr, 1, 0x09, &value) != SOC_E_NONE ) {
    return L7_FAILURE;
  }

  printf("%s(%d) Read value=0x%X tx_disable=%u\r\n",__FUNCTION__,__LINE__,value,tx_disable);

  // Only change, if is ti be changed
  if ((value & 1)!=(tx_disable & 1)) {
    printf("%s(%d) i am here\r\n",__FUNCTION__,__LINE__);
    value &= ~((uint16) 0x01);
    value |= (uint16) (tx_disable & 1);
    printf("%s(%d) Write value=0x%X\r\n",__FUNCTION__,__LINE__,value);
    if ( soc_miimc45_write( usp_map[port].unit, addr, 1, 0x09, value) != SOC_E_NONE ) {
      return L7_FAILURE;
    }
  }

  printf("%s(%d) i am here\r\n",__FUNCTION__,__LINE__);

  // Success
  return L7_SUCCESS;
}

L7_RC_t hapi_ptin_fpentry_read(st_bw_profile *profile, struct_fpentry_data *fpentry_ptr)
{
  int fpentry_id, policer_id;
  bcm_policer_config_t policer;

  // Check if the given fpentry_ptr is valid
  if (fpentry_ptr==NULL || fpentry_ptr->entry_index>=FPENTRIES_MAX || fpentry_ptr->fpentry_id==0)  {
    printf( "%s(%d): invalid fpentry (%d)\n", __FUNCTION__, __LINE__,fpentry_ptr->fpentry_id);
    return L7_FAILURE;
  }

  fpentry_id = fpentry_ptr->fpentry_id;

  if (!fpentry_ptr->inUse) {
    printf( "%s: fpentry not installed (fpentry=%d)\n",__FUNCTION__,fpentry_id);
    return L7_FAILURE;
  }

  policer_id = fpentry_ptr->policer_id;
  if (policer_id<=0) {
    printf( "%s: invalid policer id (%d) (fpentry=%d)\n",__FUNCTION__,policer_id,fpentry_id);
    return L7_FAILURE;
  }

  // Create bw policer
  bcm_policer_config_t_init(&policer);

  if (bcm_policer_get(ptin_hapi_unit,policer_id,&policer))  {
    printf( "%s: configuration reading failed (fpentry=%d)\n",__FUNCTION__,fpentry_id);
    return L7_FAILURE;
  }

  if (profile!=NULL) {
    profile->cir = (uint32) policer.ckbits_sec;
    profile->eir = (uint32) (policer.pkbits_sec-policer.ckbits_sec);
    profile->cbs = (uint32) (policer.ckbits_burst*1000/8);
    profile->ebs = (uint32) ((policer.pkbits_burst-policer.ckbits_burst)*1000/8);

    profile->port_bmp    = fpentry_ptr->port_bmp;
    profile->port_output = fpentry_ptr->port_output;
    profile->stag        = fpentry_ptr->stag;
    profile->ctag        = fpentry_ptr->ctag;
  }

  return L7_SUCCESS;
}

L7_RC_t hapi_ptin_fpentry_counters(st_HWEth_VlanStatistics_Block *counters, struct_fpentry_data *fpentry_ptr)
{
  int fpentry_id;
  uint64 val;

  // Check if the given fpentry_ptr is valid
  if (fpentry_ptr==NULL || fpentry_ptr->entry_index>=FPENTRIES_MAX || fpentry_ptr->fpentry_id==0)  {
    printf( "%s(%d): invalid fpentry (%d)\n", __FUNCTION__, __LINE__,fpentry_ptr->fpentry_id);
    return L7_FAILURE;
  }

  fpentry_id = fpentry_ptr->fpentry_id;

  if (!fpentry_ptr->inUse) {
    printf( "%s: fpentry not installed (fpentry=%d)\n",__FUNCTION__,fpentry_id);
    return L7_FAILURE;
  }

  if (bcm_field_counter_get(ptin_hapi_unit,fpentry_ptr->fpentry_id,0,&val)!=BCM_E_NONE) {
    printf("%s(%d) Error with bcm_field_meter_get (1)\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  else  {
    counters->etherNotRedPackets = val;
  }
  if (bcm_field_counter_get(ptin_hapi_unit,fpentry_ptr->fpentry_id,1,&val)!=BCM_E_NONE) {
    printf("%s(%d) Error with bcm_field_meter_get (2)\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  else  {
    counters->etherRedPackets = val;
  }

  return L7_SUCCESS;
}

#define HAPI_PTIN_FPENTRY_STAG_SENSITIVE  0

L7_RC_t hapi_ptin_fpentry_create(st_bw_profile *profile, struct_fpentry_data **fpentry_ptr)
{
  int priority=NORMAL_PRIORITY, entry_index, index_free, fpentry_id, group_id, policer_id;
  bcm_policer_config_t policer;
  bcm_ip6_t payload_d = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  bcm_ip6_t payload_m = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  bcm_error_t status;
  struct_fpentry_data *fpentry;

  // Fpentry pointer
  fpentry = (fpentry_ptr!=NULL) ? *fpentry_ptr : NULL;

  if (fpentry!=NULL) {
    printf("%s(%d) in_use=%u\r\n", __FUNCTION__, __LINE__,fpentry->inUse);
    printf("%s(%d) entry_index=%u\r\n", __FUNCTION__, __LINE__,fpentry->entry_index);
    printf("%s(%d) fpentry_id =%u\r\n", __FUNCTION__, __LINE__,fpentry->fpentry_id);
    printf("%s(%d) port_bmp   =0x%X\r\n", __FUNCTION__, __LINE__,fpentry->port_bmp);
    printf("%s(%d) port_output=%u\r\n", __FUNCTION__, __LINE__,fpentry->port_output);
    printf("%s(%d) svlan      =%u/%u\r\n", __FUNCTION__, __LINE__,fpentry->stag.vid_value,fpentry->stag.vid_mask);
    printf("%s(%d) cvlan      =%u/%u\r\n", __FUNCTION__, __LINE__,fpentry->ctag.vid_value,fpentry->ctag.vid_mask);
    printf("%s(%d) policy_id  =%d\r\n", __FUNCTION__, __LINE__,fpentry->policer_id);
  }

  // If there is no profile or no actions to be done,
  //  - Destroy fpentry
  //  - And leave function
  if (profile==NULL || ( profile->port_output==(uint8)-1 && profile->cir==(uint32)-1 ) )
  {
    printf( "%s: Removing all (fpentry=%d)\n",__FUNCTION__,fpentry_id);
    if (fpentry!=NULL) {
      hapi_ptin_fpentry_destroy(fpentry);
      *fpentry_ptr=NULL;
    }
    return L7_SUCCESS;
  }

  // Priority definition: is hiher when there is a profile associated
  if (profile->cir!=(uint32)-1)
  {
    priority = NORMAL_PRIORITY;
  }
  else
  {
    priority = LOW_PRIORITY;
  }

  // If FPentry is not in use, find another which shares the same inputs
  if (fpentry==NULL ||
      fpentry->entry_index>=FPENTRIES_MAX || fpentry->fpentry_id==0 ||
      !fpentry->inUse)
  {
    // Search for an entry with the same input parameters
    // If not found, use the first empty
    index_free = -1;
    for (entry_index=0; entry_index<FPENTRIES_MAX; entry_index++)
    {
      fpentry = &fpentry_data[entry_index];
      // Save first empty entry
      if (!fpentry->inUse)
      {
        if (index_free==-1)  index_free = entry_index;
      }
      else
      {
        // If inputs are the same (SVid and CVid), break cycle
        if ( profile->stag.vid_value == fpentry->stag.vid_value &&
             profile->stag.vid_mask  == fpentry->stag.vid_mask  &&
             profile->ctag.vid_value == fpentry->ctag.vid_value &&
             profile->ctag.vid_mask  == fpentry->ctag.vid_mask )
          break;
      }
    }

    // If not found, use the first empty entry
    if (entry_index>=FPENTRIES_MAX)
    {
      // No free entries situation
      if (index_free == -1)
      {
        printf("%s(%d) There is no room for another rule\r\n",__FUNCTION__,__LINE__);
        return L7_TABLE_IS_FULL;
      }
      entry_index = index_free;
      fpentry     = &fpentry_data[entry_index];
    }
  }
  else
  {
    entry_index = fpentry->entry_index;
  }
  
  // Some useful variables
  fpentry_id  = 0;
  policer_id  = 0;
  if (fpentry->inUse)
  {
    fpentry_id = fpentry->fpentry_id;
    policer_id = fpentry->policer_id;
  }    
  // Group id
  group_id=entry_index/FPENTRIES_MAX_IN_A_GROUP+FPGROUP_START;
 
  // At this point we have a valid fpentry pointer and a valid entry_index

  // Check entry data, if it is necessary to remove it first, or other things
  if (fpentry->inUse)
  {
    printf( "%s: This entry already exists (fpentry=%d)\n",__FUNCTION__,fpentry_id);

    printf("%s(%d) inUse   = %u\r\n",__FUNCTION__,__LINE__,fpentry->inUse);
    printf("%s(%d) index   = %u\r\n",__FUNCTION__,__LINE__,fpentry->entry_index);
    printf("%s(%d) fpentry = %u\r\n",__FUNCTION__,__LINE__,fpentry->fpentry_id);
    printf("%s(%d) stag    = %u/%u\r\n",__FUNCTION__,__LINE__,fpentry->stag.vid_value,fpentry->stag.vid_mask);
    printf("%s(%d) ctag    = %u/%u\r\n",__FUNCTION__,__LINE__,fpentry->ctag.vid_value,fpentry->ctag.vid_mask);
    printf("%s(%d) portbmp = 0x%X\r\n",__FUNCTION__,__LINE__,fpentry->port_bmp);
    printf("%s(%d) portout = %u\r\n",__FUNCTION__,__LINE__,fpentry->port_output);
    printf("%s(%d) policyId= %u\r\n",__FUNCTION__,__LINE__,fpentry->policer_id);
    printf("%s(%d) counter = %u\r\n",__FUNCTION__,__LINE__,fpentry->counter);

    // Compare input args
    // If some of them is different, destroy entry
    if ( (fpentry->port_bmp       != profile->port_bmp      ) ||
         (fpentry->port_output    != profile->port_output   ) ||
         (fpentry->stag.vid_value != profile->stag.vid_value) ||
         (fpentry->stag.vid_mask  != profile->stag.vid_mask ) ||
         (fpentry->ctag.vid_value != profile->ctag.vid_value) ||
         (fpentry->ctag.vid_mask  != profile->ctag.vid_mask ) ||
         (fpentry->policer_id==0 && profile->cir!=(uint32)-1) ||
         (fpentry->policer_id!=0 && profile->cir==(uint32)-1) )
    {
      printf( "%s: Inputs are different... destroying fpentry (fpentry=%d)\n",__FUNCTION__,fpentry_id);
      // Remove entry
      if (hapi_ptin_fpentry_destroy(fpentry)==0)  {
        printf( "%s: fpentry %d removed\n",__FUNCTION__,fpentry_id);
        *fpentry_ptr=NULL;
      }
      else  {
        printf( "hapi_ptin_fpentry_create: error removing fpentry %d\n",fpentry_id);
        return L7_FAILURE;
      }
    }
    // Input arguments are equal
    else
    {
      printf( "%s: Inputs are the same... Going to compare BWP (fpentry=%d)\n",__FUNCTION__,fpentry_id);

      // If there is no BWP defined...
      if (policer_id==0)
      {
        if ( profile->cir==(uint32)-1 )  {
          printf( "%s: Configurations are the same... do nothing (fpentry=%d)\n",__FUNCTION__,fpentry_id);
          (fpentry->counter)++;     // Increment number of creations
          if (fpentry_ptr!=NULL)  *fpentry_ptr=fpentry;
          return L7_SUCCESS;
        }
      }
      // If there is a BWP defined...
      else  {
        if ( profile->cir!=(uint32)-1 )  {
          // Get current profile settings
          if (bcm_policer_get(ptin_hapi_unit,policer_id,&policer))  {
            printf( "%s: configuration reading failed (fpentry=%d)\n",__FUNCTION__,fpentry_id);
            return L7_FAILURE;
          }
          // Compare configurations
          if ( ( policer.ckbits_sec   == profile->cir ) &&
               ( policer.ckbits_burst == (profile->cbs*8/1000) ) &&
               ( policer.pkbits_sec   == policer.ckbits_sec + profile->eir ) &&
               ( policer.pkbits_burst == policer.ckbits_burst + (profile->ebs*8/1000) ) )  {
            printf( "%s: Configurations are the same... do nothing (fpentry=%d)\n",__FUNCTION__,fpentry->fpentry_id);
            /* Para permitir a correct remoçao dos perfis */
            //(fpentry->counter)++;   // Increment number of creations
            if (fpentry_ptr!=NULL)  *fpentry_ptr=fpentry;
            return L7_SUCCESS;
          }
        }
      }
    }
  }

  // Create bw policer
  bcm_policer_config_t_init(&policer);
  if ( profile->cir!=(L7_uint32)-1 ) {
    policer.flags         = 0;
    policer.mode          = bcmPolicerModeTrTcm;
    policer.ckbits_sec    = (uint32) profile->cir;
    policer.ckbits_burst  = (uint32) profile->cbs*8/1000;
    policer.pkbits_sec    = policer.ckbits_sec + (uint32) profile->eir;
    policer.pkbits_burst  = policer.ckbits_burst + (uint32) profile->ebs*8/1000;
  }

  // Create fp entry only if fpentry is not created, or if was destroyed previously (different inputs)
  if (!fpentry->inUse)  {
    printf( "%s: Recreating rule (fpentry=%d)\n",__FUNCTION__,fpentry_id);

    if ((status=bcm_field_entry_create(ptin_hapi_unit,group_id,&fpentry_id)))  {
      printf( "hapi_ptin_fpentry_create: cannot create (fpentry=%d)\n",fpentry_id);
      if (status==BCM_E_RESOURCE)  return L7_TABLE_IS_FULL;
      return L7_FAILURE;
    }
    // Update number of available 
    if (rules_available>0)  rules_available--;

    // Only attach stat_id, if entry is new
    if (bcm_field_counter_create(ptin_hapi_unit,fpentry_id)!=BCM_E_NONE) {
      printf("%s(%d) Error with bcm_field_counter_create\n", __FUNCTION__, __LINE__);
      //return L7_FAILURE;
    }
    else
      printf("%s(%d) Success with bcm_field_counter_create\n", __FUNCTION__, __LINE__);

    // Define entry priority
    if (bcm_field_entry_prio_set(ptin_hapi_unit,fpentry_id,priority))  {
      printf( "hapi_ptin_flow_init: error with bcm_field_entry_prio_set (fpentry=%d)\n",fpentry_id);
      return L7_FAILURE;
    }
    // Define qualifiers
    if (bcm_field_qualify_Stage(ptin_hapi_unit,fpentry_id,bcmFieldStageLast))  {
      printf( "hapi_ptin_fpentry_create: error with bcm_field_qualify_Stage (fpentry=%d)\n",fpentry_id);
      status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
      if (status==BCM_E_NONE) rules_available++;
      return L7_FAILURE;
    }
    if (bcm_field_qualify_Drop(ptin_hapi_unit,fpentry_id,0,1))  {
      printf( "hapi_ptin_fpentry_create: error with bcm_field_qualify_drop (fpentry=%d)\n",fpentry_id);
      status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
      if (status==BCM_E_NONE) rules_available++;
      return L7_FAILURE;
    }

    // Include source ports
    {
      // Calculate real port bitmap
      bcm_pbmp_t value, mask;
      uint32 value_tmp=0;
      int p;

      for (p=0; p<SYSTEM_N_PORTS; p++) {
        if (!((profile->port_bmp>>p) & 1)) continue;
        value_tmp |= (L7_uint32) 1<<usp_map[p].port;
      }
      value.pbits[0]=value_tmp;
      mask.pbits[0]=0x1fffff;
      if ((status=bcm_field_qualify_InPorts(ptin_hapi_unit,fpentry_id,value,mask)))  {
        printf("%s(%d) hapi_ptin_fpentry_create: error with bcm_field_qualify_InPorts (fpentry=%d) \"%s\"\n", __FUNCTION__, __LINE__,fpentry_id,bcm_errmsg(status));
        status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
        if (status==BCM_E_NONE) rules_available++;
        return L7_FAILURE;
      }
    }

    // SVlan rule
    if (profile->stag.vid_value<4096 && profile->stag.vid_mask!=0)
    {
      if (bcm_field_qualify_OuterVlan(ptin_hapi_unit,fpentry_id,profile->stag.vid_value,profile->stag.vid_mask))  {
        printf( "hapi_ptin_fpentry_create: error with bcm_field_qualify_drop (fpentry=%d)\n",fpentry_id);
        status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
        if (status==BCM_E_NONE) rules_available++;
        return L7_FAILURE;
      }
    }

    // CVlan Rule
    if (profile->ctag.vid_value<4096 && profile->ctag.vid_mask!=0)
    {
      if (bcm_field_qualify_EtherType(ptin_hapi_unit,fpentry_id,0x8100,0xFFFF))  {
        printf( "hapi_ptin_fpentry_create: error with bcm_field_qualify_EtherType\n");
        status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
        if (status==BCM_E_NONE) rules_available++;
        return L7_FAILURE;
      }
      if (bcm_field_qualify_IpType(ptin_hapi_unit,fpentry_id,bcmFieldIpTypeNonIp))  {
        printf( "hapi_ptin_fpentry_create: error with bcm_field_qualify_IpType\n");
        status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
        if (status==BCM_E_NONE) rules_available++;
        return L7_FAILURE;
      }
      payload_d[0] = (uint8) ((profile->ctag.vid_value>>8) & 0x0F);
      payload_d[1] = (uint8) (profile->ctag.vid_value & 0xFF);
      payload_m[0] = (uint8) ((profile->ctag.vid_mask>>8) & 0x0F);
      payload_m[1] = (uint8) (profile->ctag.vid_mask & 0xFF);
      if (bcm_field_qualify_DstIp6(ptin_hapi_unit,fpentry_id,payload_d,payload_m))  {
        printf( "hapi_ptin_fpentry_create: error with bcm_field_qualify_DstIp6\n");
        status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
        if (status==BCM_E_NONE) rules_available++;
        return L7_FAILURE;
      }
    }

    //Define actions
    // This is only for a valid BWP
    if ( profile->cir!=(L7_uint32)-1 )
    {
      if (bcm_field_action_add(ptin_hapi_unit,fpentry_id,bcmFieldActionRpDrop,0,0))  {
        printf( "hapi_ptin_fpentry_create: error with bcm_field_action_add (fpentry=%d)\n",fpentry_id);
        status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
        if (status==BCM_E_NONE) rules_available++;
        return L7_FAILURE;
      }
      // This action, must not be executed, to not override the CoS bcmFieldActionPrioPktAndIntNew actions
      /*if (bcm_field_action_add(ptin_hapi_unit,fpentry_id,bcmFieldActionPrioPktAndIntCopy,0,0))  {
        printf( "hapi_ptin_fpentry_create: error with bcm_field_action_add (fpentry=%d)\n",fpentry_id);
        status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
        if (status==BCM_E_NONE) rules_available++;
        return L7_FAILURE;
      }*/
    }

    if ((status=bcm_field_action_add(ptin_hapi_unit,fpentry_id,bcmFieldActionUpdateCounter,BCM_FIELD_COUNTER_MODE_RED_NOTRED,0)))  {
      printf( "%s(%d) hapi_ptin_fpentry_create: error with bcm_field_action_add (fpentry=%d) status=\"%s\"\n", __FUNCTION__, __LINE__,fpentry_id,bcm_errmsg(status));
      status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
      if (status==BCM_E_NONE) rules_available++;
      return L7_FAILURE;
    }

    // Only execute port redirection, if port is valid!
    if (profile->port_output<SYSTEM_N_PORTS) {
      printf( "%s: Adding Redirect action (fpentry=%d)\n",__FUNCTION__,fpentry_id);
      if ((status=bcm_field_action_add(ptin_hapi_unit,fpentry_id,bcmFieldActionRedirectPort,0,usp_map[profile->port_output].port)))  {
        printf( "%s(%d) hapi_ptin_fpentry_create: error with bcm_field_action_add (fpentry=%d) status=\"%s\"\n", __FUNCTION__, __LINE__,fpentry_id,bcm_errmsg(status));
        status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
        if (status==BCM_E_NONE) rules_available++;
        return L7_FAILURE;
      }
      if ((status=bcm_field_action_add(ptin_hapi_unit,fpentry_id,bcmFieldActionPrioPktAndIntCopy,0,0)))  {
        printf( "%s(%d) hapi_ptin_fpentry_create: error with bcm_field_action_add (fpentry=%d) status=\"%s\"\n", __FUNCTION__, __LINE__,fpentry_id,bcm_errmsg(status));
        status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
        if (status==BCM_E_NONE) rules_available++;
        return L7_FAILURE;
      }
    }

    // Policer is only configured for a valid BWP
    policer_id = 0;
    if ( profile->cir!=(L7_uint32)-1 )
    {
      printf( "%s: Creating BWP (fpentry=%d)\n",__FUNCTION__,fpentry_id);

      if (bcm_policer_create(ptin_hapi_unit,&policer,&policer_id) || policer_id==0)  {
        printf( "hapi_ptin_fpentry_create: error creating policy (fpentry=%d)\n",fpentry_id);
        status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
        if (status==BCM_E_NONE) rules_available++;
        return L7_FAILURE;
      }
  
      // Attach policer to fpentry
      if (bcm_field_entry_policer_attach(ptin_hapi_unit,fpentry_id,0,policer_id))  {
        printf( "hapi_ptin_fpentry_create: error attaching policer (fpentry=%d)\n",fpentry_id);
        bcm_policer_destroy(ptin_hapi_unit,policer_id);
        status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
        if (status==BCM_E_NONE) rules_available++;
        return L7_FAILURE;
      }
    }
    // Store data
    fpentry->fpentry_id  = fpentry_id;
    fpentry->port_bmp    = profile->port_bmp;
    fpentry->port_output = profile->port_output;
    fpentry->stag        = profile->stag;
    fpentry->ctag        = profile->ctag;
    fpentry->policer_id  = policer_id;
    fpentry->counter     = 0;
    fpentry->inUse       = 1;
  }
  else  {
    printf( "%s: Redefining BWP (fpentry=%d)\n",__FUNCTION__,fpentry_id);

    policer_id = fpentry->policer_id;

    /* Only if CIR is not 'NO PROFILE' */
    if (policer_id!=0 && profile->cir!=(L7_uint32)-1)
    {
      if (bcm_policer_set(ptin_hapi_unit,policer_id,&policer))  {
        printf( "hapi_ptin_fpentry_create: reconfiguration failed (fpentry=%d)\n",fpentry_id);
        return L7_FAILURE;
      }
    }

    // Remove entry
    if (bcm_field_entry_remove(ptin_hapi_unit,fpentry_id)) {
      printf( "hapi_ptin_fpentry_create: error removing (fpentry=%d)\n",fpentry_id);
      return L7_FAILURE;
    }
  }

  // (Re)Install entry
  if (bcm_field_entry_install(ptin_hapi_unit,fpentry_id)) {
    printf( "hapi_ptin_fpentry_create: error installing (fpentry=%d)\n",fpentry_id);
    bcm_field_entry_policer_detach(ptin_hapi_unit,fpentry_id,0);
    bcm_policer_destroy(ptin_hapi_unit,policer_id);
    status=bcm_field_entry_destroy(ptin_hapi_unit,fpentry_id);
    if (status==BCM_E_NONE) rules_available++;
    // Clear inUse parameter
    fpentry->inUse = 0;
    return L7_FAILURE;
  }

  printf("%s(%d) inUse   = %u\r\n",__FUNCTION__,__LINE__,fpentry->inUse);
  printf("%s(%d) index   = %u\r\n",__FUNCTION__,__LINE__,fpentry->entry_index);
  printf("%s(%d) fpentry = %u\r\n",__FUNCTION__,__LINE__,fpentry->fpentry_id);
  printf("%s(%d) stag    = %u/%u\r\n",__FUNCTION__,__LINE__,fpentry->stag.vid_value,fpentry->stag.vid_mask);
  printf("%s(%d) ctag    = %u/%u\r\n",__FUNCTION__,__LINE__,fpentry->ctag.vid_value,fpentry->ctag.vid_mask);
  printf("%s(%d) portbmp = 0x%X\r\n",__FUNCTION__,__LINE__,fpentry->port_bmp);
  printf("%s(%d) portout = %u\r\n",__FUNCTION__,__LINE__,fpentry->port_output);
  printf("%s(%d) policyId= %u\r\n",__FUNCTION__,__LINE__,fpentry->policer_id);
  printf("%s(%d) counter = %u\r\n",__FUNCTION__,__LINE__,fpentry->counter);

  printf("%s(%d) New rule with fpentry_id=%d\n", __FUNCTION__, __LINE__,fpentry_id);

  // Return created fpentry
  if (fpentry_ptr!=NULL)  *fpentry_ptr=fpentry;

  return L7_SUCCESS;
}

L7_RC_t hapi_ptin_fpentry_destroy(struct_fpentry_data *fpentry_ptr)
{
  int policer_id;

  if (fpentry_ptr!=NULL) {
    printf("%s(%d) in_use=%u\r\n", __FUNCTION__, __LINE__,fpentry_ptr->inUse);
    printf("%s(%d) entry_index=%u\r\n", __FUNCTION__, __LINE__,fpentry_ptr->entry_index);
    printf("%s(%d) fpentry_id=%u\r\n", __FUNCTION__, __LINE__,fpentry_ptr->fpentry_id);
  }

  if (fpentry_ptr==NULL || fpentry_ptr->entry_index>=FPENTRIES_MAX || fpentry_ptr->fpentry_id==0)  {
    printf( "%s(%d): invalid fpentry (%d)\n", __FUNCTION__, __LINE__,fpentry_ptr->fpentry_id);
    return L7_FAILURE;
  }

  // Only destroy entry if entry_id is valid, and is in use
  if (fpentry_ptr->fpentry_id>0 && fpentry_ptr->inUse)
  {
    printf("%s(%d) inUse   = %u\r\n",__FUNCTION__,__LINE__,   fpentry_ptr->inUse);
    printf("%s(%d) index   = %u\r\n",__FUNCTION__,__LINE__,   fpentry_ptr->entry_index);
    printf("%s(%d) fpentry = %u\r\n",__FUNCTION__,__LINE__,   fpentry_ptr->fpentry_id);
    printf("%s(%d) stag    = %u/%u\r\n",__FUNCTION__,__LINE__,fpentry_ptr->stag.vid_value,fpentry_ptr->stag.vid_mask);
    printf("%s(%d) ctag    = %u/%u\r\n",__FUNCTION__,__LINE__,fpentry_ptr->ctag.vid_value,fpentry_ptr->ctag.vid_mask);
    printf("%s(%d) portbmp = 0x%X\r\n",__FUNCTION__,__LINE__, fpentry_ptr->port_bmp);
    printf("%s(%d) portout = %u\r\n",__FUNCTION__,__LINE__,   fpentry_ptr->port_output);
    printf("%s(%d) policyId= %u\r\n",__FUNCTION__,__LINE__,   fpentry_ptr->policer_id);
    printf("%s(%d) counter = %u\r\n",__FUNCTION__,__LINE__,   fpentry_ptr->counter);

    #if 1
    // If this entry was created several times, rewind these all creations
    if (fpentry_ptr->counter>0)
    {
      printf( "hapi_ptin_fpentry_destroy: There are more %u counters.\n",fpentry_ptr->counter);
      (fpentry_ptr->counter)--;
      return L7_SUCCESS;
    }
    #endif

    // Uninstall entry
    if (bcm_field_entry_remove(ptin_hapi_unit,fpentry_ptr->fpentry_id))  {
      printf( "hapi_ptin_fpentry_destroy: error with bcm_field_entry_remove\n");
      return L7_FAILURE;
    }
    
    policer_id = fpentry_ptr->policer_id;
    if (policer_id>0)  {
      // Detach policer to fpentry
      if (bcm_field_entry_policer_detach(ptin_hapi_unit,fpentry_ptr->fpentry_id,0))  {
        printf( "hapi_ptin_fpentry_destroy: error detaching policer (fpentry=%d)\n",fpentry_ptr->fpentry_id);
        return L7_FAILURE;
      }
      // Destroy policer
      if (bcm_policer_destroy(ptin_hapi_unit,policer_id))  {
        printf( "hapi_ptin_fpentry_destroy: error destroying policer (fpentry=%d)\n",fpentry_ptr->fpentry_id);
        return L7_FAILURE;
      }
    }
    fpentry_ptr->policer_id = 0;
  
    // Only attach stat_id, if entry is new
    if (bcm_field_counter_destroy(ptin_hapi_unit,fpentry_ptr->fpentry_id)!=BCM_E_NONE) {
      printf("%s(%d) Error with bcm_field_counter_destroy\n", __FUNCTION__, __LINE__);
      //return L7_FAILURE;
    }
    else
      printf("%s(%d) Success with bcm_field_counter_destroy\n", __FUNCTION__, __LINE__);

    // Destroy fpentry
    if (bcm_field_entry_destroy(ptin_hapi_unit,fpentry_ptr->fpentry_id))  {
      printf( "hapi_ptin_fpentry_destroy: error destroying fpentry (fpentry=%d)\n",fpentry_ptr->fpentry_id);
      return L7_FAILURE;
    }
    rules_available++;
  }
  
  printf("%s(%d) Success removing fpentry %u\n", __FUNCTION__, __LINE__,fpentry_ptr->fpentry_id);

  // !!! Never use setmem instruction, or modify entry_id field !!!
  fpentry_ptr->port_bmp      = 0;
  fpentry_ptr->port_output   = 0;
  fpentry_ptr->stag.vid_value= 0;
  fpentry_ptr->stag.vid_mask = 0;
  fpentry_ptr->ctag.vid_value= 0;
  fpentry_ptr->ctag.vid_mask = 0;
  fpentry_ptr->counter       = 0;

  fpentry_ptr->fpentry_id = 0;
  fpentry_ptr->inUse = 0;

  return L7_SUCCESS;
}


L7_RC_t hapi_ptin_readCounters(uint8 port, st_HWEthRFC2819_Statistics_Block *Rx, st_HWEthRFC2819_Statistics_Block *Tx)
{
  L7_uint64 tmp=0, tmp1=0, tmp2=0, tmp3=0;
  L7_uint64 mtuePkts=0;
  L7_uint64 pkts1519to2047, pkts2048to4095, pkts4096to9216, pkts9217to16383;

  if (port>=SYSTEM_N_PORTS)  return L7_FAILURE;

  osapiSemaTake(sem_shell_access,L7_WAIT_FOREVER);
  if (port<16) {
  
    // Rx counters
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GRMTUEr, 0, &mtuePkts);                             /* Packets > MTU bytes (good and bad) */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GRDROPr          , 0, &tmp1);
    soc_counter_get(usp_map[port].unit, usp_map[port].port, DROP_PKT_CNT_INGr, 0, &tmp2);
    Rx->etherStatsDropEvents = tmp1 + tmp2 + mtuePkts;                                                          /* Drop Events */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GRBYTr , 0, &tmp1);
    soc_counter_get(usp_map[port].unit, usp_map[port].port, RRBYTr , 0, &tmp2);
    Rx->etherStatsOctets = tmp1 + tmp2;                                                                         /* Octets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GRPKTr , 0, &Rx->etherStatsPkts);                   /* Packets (>=64 bytes) */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GRBCAr , 0, &Rx->etherStatsBroadcastPkts);          /* Broadcasts */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GRMCAr , 0, &Rx->etherStatsMulticastPkts);          /* Muilticast */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GRFCSr , 0, &Rx->etherStatsCRCAlignErrors);         /* FCS Errors (64-1518 bytes)*/
    Rx->etherStatsCollisions = 0;                                                                               /* Collisions */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GRUNDr , 0, &Rx->etherStatsUndersizePkts);          /* Undersize */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GROVRr,  0, &tmp1);
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GRMGVr,  0, &tmp2);
    Rx->etherStatsOversizePkts = tmp1 + tmp2;                                                                   /* Oversize: 1519-MTU bytes */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GRFRGr , 0, &Rx->etherStatsFragments);              /* Fragments */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GRJBRr , 0, &Rx->etherStatsJabbers);                /* Jabbers */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GR64r  , 0, &Rx->etherStatsPkts64Octets);           /* 64B packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GR127r , 0, &Rx->etherStatsPkts65to127Octets);      /* 65-127B packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GR255r , 0, &Rx->etherStatsPkts128to255Octets);     /* 128-255B packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GR511r , 0, &Rx->etherStatsPkts256to511Octets);     /* 256-511B packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GR1023r, 0, &Rx->etherStatsPkts512to1023Octets);    /* 512-1023B packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GR1518r, 0, &Rx->etherStatsPkts1024to1518Octets);   /* 1024-1518B packets */

    soc_counter_get(usp_map[port].unit, usp_map[port].port, GR2047r, 0, &pkts1519to2047);   /* 1519-2047 Bytes packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GR4095r, 0, &pkts2048to4095);   /* 2048-4095 Bytes packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GR9216r, 0, &pkts4096to9216);   /* 4096-9216 Bytes packets */
    pkts9217to16383 = 0;
    Rx->etherStatsPkts1519toMaxOctets = pkts1519to2047 + pkts2048to4095 + pkts4096to9216 + pkts9217to16383;

    soc_counter_get_rate(usp_map[port].unit, usp_map[port].port, GRBYTr , 0, &Rx->Throughput);                  /* Throughput */

    // Tx counters
    //soc_counter_get(usp_map[port].unit, usp_map[port].port, DROP_PKT_CNTr   , 0, &tmp1);
    //soc_counter_get(usp_map[port].unit, usp_map[port].port, HOLDROP_PKT_CNTr, 0, &tmp2);
    soc_counter_get(usp_map[port].unit, usp_map[port].port, EGRDROPPKTCOUNTr, 0, &tmp3);
    Tx->etherStatsDropEvents = /*tmp1 + tmp2 +*/ tmp3;                                                              /* Drop Events */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GTBYTr , 0, &Tx->etherStatsOctets);                 /* Octets */                   
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GTPKTr , 0, &Tx->etherStatsPkts);                   /* Packets */                  
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GTBCAr , 0, &Tx->etherStatsBroadcastPkts);          /* Broadcasts */               
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GTMCAr , 0, &Tx->etherStatsMulticastPkts);          /* Muilticast */               
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GTFCSr , 0, &Tx->etherStatsCRCAlignErrors);         /* FCS Errors (64-1518 bytes)*/
    Tx->etherStatsCollisions = 0;                                                                               /* Collisions */               
    Tx->etherStatsUndersizePkts = 0;                                                                            /* Undersize */                
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GTOVRr,  0, &tmp1);
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GTMGVr,  0, &tmp2);
    Tx->etherStatsOversizePkts = tmp1 + tmp2;                                                                   /* Oversize: 1519-MTU bytes */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GTFRGr , 0, &Tx->etherStatsFragments);              /* Fragments */               
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GTJBRr , 0, &Tx->etherStatsJabbers);                /* Jabbers */                 
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GT64r  , 0, &Tx->etherStatsPkts64Octets);           /* 64B packets */             
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GT127r , 0, &Tx->etherStatsPkts65to127Octets);      /* 65-127B packets */         
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GT255r , 0, &Tx->etherStatsPkts128to255Octets);     /* 128-255B packets */        
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GT511r , 0, &Tx->etherStatsPkts256to511Octets);     /* 256-511B packets */        
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GT1023r, 0, &Tx->etherStatsPkts512to1023Octets);    /* 512-1023B packets */       
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GT1518r, 0, &Tx->etherStatsPkts1024to1518Octets);   /* 1024-1518B packets */

    soc_counter_get(usp_map[port].unit, usp_map[port].port, GT2047r, 0, &pkts1519to2047);   /* 1519-2047 Bytes packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GT4095r, 0, &pkts2048to4095);   /* 2048-4095 Bytes packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, GT9216r, 0, &pkts4096to9216);   /* 4096-9216 Bytes packets */
    pkts9217to16383 = 0;
    Tx->etherStatsPkts1519toMaxOctets = pkts1519to2047 + pkts2048to4095 + pkts4096to9216 + pkts9217to16383;

    soc_counter_get_rate(usp_map[port].unit, usp_map[port].port, GTBYTr , 0, &Tx->Throughput);                  /* Throughput */              
  }
  else {
    // Rx counters
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IRMEGr , 0, &tmp1);
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IRMEBr , 0, &tmp2);
    mtuePkts = tmp1 + tmp2;                                                                                     /* Packets > MTU bytes (good and bad) */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IRDROPr          , 0, &tmp1);
    soc_counter_get(usp_map[port].unit, usp_map[port].port, DROP_PKT_CNT_INGr, 0, &tmp2);
    tmp = tmp1 + tmp2;
    ( tmp >= mtuePkts ) ? ( tmp -= mtuePkts ) : ( tmp = 0 );
    Rx->etherStatsDropEvents = tmp + mtuePkts;                                                                  /* Drop Events */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IRBYTr , 0, &Rx->etherStatsOctets);                 /* Octets */                   
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IRPKTr , 0, &Rx->etherStatsPkts);                   /* Packets (>=64 bytes) */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IRBCAr , 0, &Rx->etherStatsBroadcastPkts);          /* Broadcasts */               
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IRMCAr , 0, &Rx->etherStatsMulticastPkts);          /* Muilticast */               
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IRFCSr , 0, &Rx->etherStatsCRCAlignErrors);         /* FCS Errors (64-1518 bytes)*/
    Rx->etherStatsCollisions = 0;                                                                               /* Collisions */               
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IRUNDr , 0, &Rx->etherStatsUndersizePkts);          /* Undersize */                
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IROVRr , 0, &Rx->etherStatsOversizePkts);           /* Oversize: 1523-MTU bytes */ 
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IRFRGr , 0, &Rx->etherStatsFragments);              /* Fragments */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IRJBRr , 0, &Rx->etherStatsJabbers);                /* Jabbers */                  
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IR64r  , 0, &Rx->etherStatsPkts64Octets);           /* 64B packets */              
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IR127r , 0, &Rx->etherStatsPkts65to127Octets);      /* 65-127B packets */          
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IR255r , 0, &Rx->etherStatsPkts128to255Octets);     /* 128-255B packets */         
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IR511r , 0, &Rx->etherStatsPkts256to511Octets);     /* 256-511B packets */         
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IR1023r, 0, &Rx->etherStatsPkts512to1023Octets);    /* 512-1023B packets */        
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IR1518r, 0, &Rx->etherStatsPkts1024to1518Octets);   /* 1024-1518B packets */       

    soc_counter_get(usp_map[port].unit, usp_map[port].port, IR2047r, 0, &pkts1519to2047);   /* 1519-2047 Bytes packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IR4095r, 0, &pkts2048to4095);   /* 2048-4095 Bytes packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IR9216r, 0, &pkts4096to9216);   /* 4096-9216 Bytes packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IR16383r,0, &pkts9217to16383);  /* 9217-16383 Bytes packets */
    Rx->etherStatsPkts1519toMaxOctets = pkts1519to2047 + pkts2048to4095 + pkts4096to9216 + pkts9217to16383;

    soc_counter_get_rate(usp_map[port].unit, usp_map[port].port, IRBYTr , 0, &Rx->Throughput);                  /* Throughput */               

    // Tx counters
    //soc_counter_get(usp_map[port].unit, usp_map[port].port, DROP_PKT_CNTr   , 0, &tmp1);
    //soc_counter_get(usp_map[port].unit, usp_map[port].port, HOLDROP_PKT_CNTr, 0, &tmp2);
    soc_counter_get(usp_map[port].unit, usp_map[port].port, EGRDROPPKTCOUNTr, 0, &tmp3);
    Tx->etherStatsDropEvents = /*tmp1 + tmp2 +*/ tmp3;                                                          /* Drop Events */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, ITBYTr , 0, &Tx->etherStatsOctets);                 /* Octets */                   
    soc_counter_get(usp_map[port].unit, usp_map[port].port, ITPKTr , 0, &Tx->etherStatsPkts);                   /* Packets */                  
    soc_counter_get(usp_map[port].unit, usp_map[port].port, ITBCAr , 0, &Tx->etherStatsBroadcastPkts);          /* Broadcasts */               
    soc_counter_get(usp_map[port].unit, usp_map[port].port, ITMCAr , 0, &Tx->etherStatsMulticastPkts);          /* Muilticast */               
    soc_counter_get(usp_map[port].unit, usp_map[port].port, ITFCSr , 0, &Tx->etherStatsCRCAlignErrors);         /* FCS Errors (64-1518 bytes)*/
    Tx->etherStatsCollisions = 0;                                                                               /* Collisions */               
    Tx->etherStatsUndersizePkts = 0;                                                                            /* Undersize */                
    soc_counter_get(usp_map[port].unit, usp_map[port].port, ITOVRr,  0, &Tx->etherStatsOversizePkts);           /* Oversize: 1523-MTU bytes */ 
    soc_counter_get(usp_map[port].unit, usp_map[port].port, ITFRGr , 0, &Tx->etherStatsFragments);              /* Fragments */                
    Tx->etherStatsJabbers       = 0;                                                                            /* Jabbers */                  
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IT64r  , 0, &Tx->etherStatsPkts64Octets);           /* 64B packets */              
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IT127r , 0, &Tx->etherStatsPkts65to127Octets);      /* 65-127B packets */          
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IT255r , 0, &Tx->etherStatsPkts128to255Octets);     /* 128-255B packets */         
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IT511r , 0, &Tx->etherStatsPkts256to511Octets);     /* 256-511B packets */         
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IT1023r, 0, &Tx->etherStatsPkts512to1023Octets);    /* 512-1023B packets */        
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IT1518r, 0, &Tx->etherStatsPkts1024to1518Octets);   /* 1024-1518B packets */       

    soc_counter_get(usp_map[port].unit, usp_map[port].port, IT2047r, 0, &pkts1519to2047);   /* 1519-2047 Bytes packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IT4095r, 0, &pkts2048to4095);   /* 2048-4095 Bytes packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IT9216r, 0, &pkts4096to9216);   /* 4096-9216 Bytes packets */
    soc_counter_get(usp_map[port].unit, usp_map[port].port, IT16383r,0, &pkts9217to16383);  /* 9217-16383 Bytes packets */
    Tx->etherStatsPkts1519toMaxOctets = pkts1519to2047 + pkts2048to4095 + pkts4096to9216 + pkts9217to16383;

    soc_counter_get_rate(usp_map[port].unit, usp_map[port].port, ITBYTr , 0, &Tx->Throughput);                  /* Throughput */               
  }
  osapiSemaGive(sem_shell_access);

  /* Only missing undersize and fragment packets in packet counting */
  Rx->etherStatsPkts += Rx->etherStatsUndersizePkts + Rx->etherStatsFragments;

  /* Throughput in bps */
  Rx->Throughput *= 8;
  Tx->Throughput *= 8;

  return L7_SUCCESS;
}

L7_RC_t hapi_ptin_clearCounters(uint8 port)
{ 
  if (port>=SYSTEM_N_PORTS)  return L7_FAILURE;

  osapiSemaTake(sem_shell_access,L7_WAIT_FOREVER);
  if (port<16) {
    // Rx counters
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GRDROPr          , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, DROP_PKT_CNT_INGr, 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GRMTUEr, 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GRPKTr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GRBYTr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, RRPKTr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, RRBYTr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GRBCAr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GRMCAr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GRFCSr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GRUNDr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GROVRr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GRMGVr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GRFRGr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GRJBRr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GR64r  , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GR127r , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GR255r , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GR511r , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GR1023r, 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GR1518r, 0, 0);

    /* Tx counters */
    soc_counter_set(usp_map[port].unit, usp_map[port].port, DROP_PKT_CNTr   , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, HOLDROP_PKT_CNTr, 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, EGRDROPPKTCOUNTr, 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GTPKTr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GTBYTr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GTBCAr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GTMCAr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GTFCSr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GTOVRr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GTMGVr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GTFRGr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GTJBRr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GT64r  , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GT127r , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GT255r , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GT511r , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GT1023r, 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, GT1518r, 0, 0);
  }
  else
  {
    // Rx counters
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IRDROPr          , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, DROP_PKT_CNT_INGr, 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IRMEGr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IRMEBr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IRPKTr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IRBYTr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IRBCAr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IRMCAr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IRFCSr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IRUNDr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IROVRr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IRFRGr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IRJBRr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IR64r  , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IR127r , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IR255r , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IR511r , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IR1023r, 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IR1518r, 0, 0);

    /* Tx counters */
    soc_counter_set(usp_map[port].unit, usp_map[port].port, DROP_PKT_CNTr   , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, HOLDROP_PKT_CNTr, 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, EGRDROPPKTCOUNTr, 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, ITPKTr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, ITBYTr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, ITBCAr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, ITMCAr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, ITFCSr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, ITOVRr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, ITFRGr , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IT64r  , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IT127r , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IT255r , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IT511r , 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IT1023r, 0, 0);
    soc_counter_set(usp_map[port].unit, usp_map[port].port, IT1518r, 0, 0);
  }
  osapiSemaGive(sem_shell_access);

  return L7_SUCCESS;
}


// Count the number of available flow counters
//
uint16 hapiBroadPtinFlowCounters_numberOfAvailableEntries(void)
{
  uint16 i, number;

  number = 0;

  for (i=0; i<FLOW_COUNTERS_MAX; i++) {
    if (ptin_fcounters[i].flow_id>0 && ptin_fcounters[i].flow_id<(uint16)-1)  number++;
  }

  return (FLOW_COUNTERS_MAX-number);
}

// Find a particular flow counter
//
static st_ptin_fcounters *ptin_find_flow_counters(uint16 flow_id, uint32 client_channel)
{
  uint8 i;

  // Find this flow id
  for (i=0; i<FLOW_COUNTERS_MAX; i++) {
    if (ptin_fcounters[i].flow_id==0 || ptin_fcounters[i].flow_id==(uint16)-1)
      continue;
    if (ptin_fcounters[i].flow_id==flow_id && ptin_fcounters[i].client_channel==client_channel)
      break;
  }

  // Not found situation
  if (i>=FLOW_COUNTERS_MAX)
    return NULL;

  // Return found element
  return &ptin_fcounters[i];
}

// Add a flow counter
//
L7_RC_t hapi_ptin_flow_counters_add(uint16 flow_id, enum_ptin_flow_type flow_type,
                                uint32 pon_port_bmp, uint32 eth_port_bmp, uint16 pon_vlan, uint16 eth_vlan,
                                uint32 client_channel)
{
  uint8  i, port;
  bcm_pbmp_t eth_ports, pon_ports, msk_ports;
  st_ptin_fcounters *flow_counters;
  uint8               flow, dir;
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId;
  L7_RC_t             rc = L7_SUCCESS;
  bcm_mac_t           dmac;
  uint16              eth_type;
  L7_uint8            to_be_dropped, ip_type;
  L7_uint32           channel_ip;
  L7_uint8           *inports_ptr, *vlan_ptr;
  uint8 value[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  uint8 mask[16]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  // Validate arguments
  if (flow_id==0 || flow_id==(uint16)-1 || 
      pon_port_bmp==0 || eth_port_bmp==0 ||
      pon_vlan<=1 || pon_vlan>=4094 || eth_vlan<=1 || eth_vlan>=4094 /*||
      client_channel==0*/)
  {
    printf("%s(%d) Invalid parameters\r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  /* Check for conflicting counters */
  for (i=0; i<FLOW_COUNTERS_MAX; i++)
  {
    /* Skip not used entries */
    if (ptin_fcounters[i].flow_id==0)  continue;

    /* Skip counters of other services */
    if (flow_id!=ptin_fcounters[i].flow_id)  continue;

    /* First conflict situation:
       There is a client counter, but is request a global service counter */
    if (client_channel==0 && ptin_fcounters[i].client_channel!=0)  break;

    /* Second conflict situation:
       There is a global service counter, but is requested a client counter */
    if (client_channel!=0 && ptin_fcounters[i].client_channel==0)  break;
  }

  /* Check if a conflict siatuation was found */
  if (i<FLOW_COUNTERS_MAX)
  {
    printf("%s(%d) Can't create policy: conflict with counter flow_id=%u, client=%u\r\n", __FUNCTION__, __LINE__,
           flow_id,ptin_fcounters[i].client_channel);
    return L7_REQUEST_DENIED;
  }

  // If this flow id exists, remove it
  if ((flow_counters=ptin_find_flow_counters(flow_id,client_channel))!=NULL) {
    if (flow_counters->policy_id!=0)
    {
      if (hapiBroadPolicyDelete(flow_counters->policy_id)!=L7_SUCCESS)
      {
        printf("%s(%d) Can't delete policy (%u)\r\n", __FUNCTION__, __LINE__,flow_counters->policy_id);
        return L7_FAILURE;
      }
    }
    memset(flow_counters,0x00,sizeof(st_ptin_fcounters));
  }

  // Find the first free element to be stored
  for (i=0; i<FLOW_COUNTERS_MAX && ptin_fcounters[i].flow_id!=0; i++);
  // if not found, return error
  if (i>=FLOW_COUNTERS_MAX)
  {
    printf("%s(%d) No more counters can be added\r\n", __FUNCTION__, __LINE__);
    return L7_TABLE_IS_FULL;
  }

  // Pointer to the element to be used
  flow_counters = &ptin_fcounters[i];

  // Clear all policy ids
  memset(flow_counters,0x00,sizeof(st_ptin_fcounters));

  // Convert port list, to the physical port list
  BCM_PBMP_CLEAR(pon_ports);
  BCM_PBMP_CLEAR(eth_ports);
  BCM_PBMP_CLEAR(msk_ports);
  for (port=0; port<SYSTEM_N_PORTS; port++,pon_port_bmp>>=1,eth_port_bmp>>=1) {
    if (pon_port_bmp & 1)  {
      //printf("%s(%d) Port %u added to pon ports\r\n", __FUNCTION__, __LINE__,port);
      BCM_PBMP_PORT_ADD(pon_ports,usp_map[port].port);
    }
    if (eth_port_bmp & 1) {
      //printf("%s(%d) Port %u added to eth ports\r\n", __FUNCTION__, __LINE__,port);
      BCM_PBMP_PORT_ADD(eth_ports,usp_map[port].port);
    }
    BCM_PBMP_PORT_ADD(msk_ports,usp_map[port].port);
  }

  // Create Policy Id
  if ((rc=hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYS_EXT /*BROAD_POLICY_TYPE_SYSTEM*/,&policyId))!=L7_SUCCESS)
  {
    printf("%s(%d) Can't create policy: rc=%d\r\n", __FUNCTION__, __LINE__,rc);
    return L7_TABLE_IS_FULL;
  }

  // Run all paket types and ports
  for (dir=PTIN_PORTS_PON_RX; dir<PTIN_PORTS_MAX; dir++)
  {
    for (flow=PTIN_FLOW_TYPE_UNICAST; flow<PTIN_FLOW_TYPE_MAX; flow++)
    {
      // Create Rule
      if ((rc=hapiBroadPolicyRuleAdd(policyId,&ruleId))!=L7_SUCCESS) {
        break;
      }

      // Port and vlan rules
      memset(mask,0xff,sizeof(uint32));
      switch (dir) {
        case PTIN_PORTS_PON_RX:
          inports_ptr = (uint8 *) &pon_ports;
          vlan_ptr = (uint8 *) &pon_vlan;
          break;

        case PTIN_PORTS_ETH_RX:
        default:
          inports_ptr = (uint8 *) &eth_ports;
          vlan_ptr = (uint8 *) &eth_vlan;
          break;
      }
      if (hapiBroadPolicyRuleQualifierAdd(policyId,ruleId,BROAD_FIELD_INPORTS,inports_ptr,(uint8 *) &msk_ports)!=L7_SUCCESS) {
        hapiBroadPolicyRuleDelete(policyId,ruleId);
        break;
      }
      if (hapiBroadPolicyRuleQualifierAdd(policyId,ruleId,BROAD_FIELD_IVID,vlan_ptr,mask)!=L7_SUCCESS) {
        hapiBroadPolicyRuleDelete(policyId,ruleId);
        break;
      }

      // By default, packets to measure are not to be dropped
      to_be_dropped = 0;
      // Packet type definition
      switch (flow) {
        case PTIN_FLOW_TYPE_UNICAST:
          // Unicast DMAC
          memset(dmac,0x00,sizeof(bcm_mac_t));
          memset(mask,0x00,sizeof(uint8)*6);
          dmac[0] = 0x00;
          mask[0] = 0x01;
          rc = hapiBroadPolicyRuleQualifierAdd(policyId,ruleId,BROAD_FIELD_MACDA,(uint8 *) dmac,(uint8 *) mask);
          break;

        case PTIN_FLOW_TYPE_BROADCAST:
          memset(dmac,0xff,sizeof(bcm_mac_t));
          memset(mask,0xff,sizeof(uint8)*6);
          rc = hapiBroadPolicyRuleQualifierAdd(policyId,ruleId,BROAD_FIELD_MACDA,(uint8 *) dmac,(uint8 *) mask);
          break;

        case PTIN_FLOW_TYPE_MULTICAST:
          memset(dmac,0x00,sizeof(bcm_mac_t));
          memset(mask,0x00,sizeof(uint8)*6);
          dmac[0] = 0x01;
          mask[0] = 0x01;
          rc = hapiBroadPolicyRuleQualifierAdd(policyId,ruleId,BROAD_FIELD_MACDA,(uint8 *) dmac,(uint8 *) mask);
          break;

        case PTIN_FLOW_TYPE_DROPPED:
          to_be_dropped = 1;
          break;
      }
      // Something went wrong
      if (rc != L7_SUCCESS) {
        hapiBroadPolicyRuleDelete(policyId,ruleId);
        break;
      }

      // In case client/channel is not null, add more specific rules
      if (client_channel!=0) {
        switch ((uint8) flow_type) {
          case PTIN_FLOW_TYPE_UNICAST:
            // VLAN Tag
            eth_type = 0x8100;
            memset(mask,0xff,sizeof(uint16));
            rc = hapiBroadPolicyRuleQualifierAdd(policyId,ruleId,BROAD_FIELD_ETHTYPE,(uint8 *) &eth_type,(uint8 *) mask);
            if (rc == L7_SUCCESS) {
              // Non IP packet
              ip_type = BROAD_IP_TYPE_NONIP;
              mask[0] = 0xFF;
              rc = hapiBroadPolicyRuleQualifierAdd(policyId,ruleId,BROAD_FIELD_IP_TYPE,(uint8 *) &ip_type,(uint8 *) mask);
              if (rc == L7_SUCCESS) {
                // C-VLAN
                memset(value,0x00,sizeof(uint8)*16);
                memset(mask ,0x00,sizeof(uint8)*16);
                value[0] = (uint8) ((client_channel>>8) & 0x0F);
                value[1] = (uint8) (client_channel & 0xFF);
                mask[0]  = 0x0F;
                mask[1]  = 0xFF;
                rc = hapiBroadPolicyRuleQualifierAdd(policyId,ruleId,BROAD_FIELD_IP6_DST,(uint8 *) value,(uint8 *) mask);
              }
            }
            break;
          case PTIN_FLOW_TYPE_MULTICAST:
            // IP Packet
            eth_type = 0x0800;
            memset(mask,0xff,sizeof(uint16));
            rc = hapiBroadPolicyRuleQualifierAdd(policyId,ruleId,BROAD_FIELD_ETHTYPE,(uint8 *) &eth_type,(uint8 *) mask);
            if (rc == L7_SUCCESS) {
              // IPv4 packet
              ip_type = BROAD_IP_TYPE_IPV4;
              mask[0] = 0xFF;
              rc = hapiBroadPolicyRuleQualifierAdd(policyId,ruleId,BROAD_FIELD_IP_TYPE,(uint8 *) &ip_type,(uint8 *) mask);
              if (rc == L7_SUCCESS) {
                // Channel IP
                channel_ip = client_channel;
                memset(mask ,0xff,sizeof(uint32));
                rc = hapiBroadPolicyRuleQualifierAdd(policyId,ruleId,BROAD_FIELD_DIP,(uint8 *) &channel_ip,(uint8 *) mask);
              }
            }
            break;
        }
        // Something went wrong
        if (rc != L7_SUCCESS) {
          hapiBroadPolicyRuleDelete(policyId,ruleId);
          break;
        }
      }

      // Drop rule
      mask[0] = 0x01;
      if (hapiBroadPolicyRuleQualifierAdd(policyId,ruleId,BROAD_FIELD_DROP,(uint8 *) &to_be_dropped,(uint8 *) mask)) {
        hapiBroadPolicyRuleDelete(policyId,ruleId);
        break;
      }

      // Add counters
      if (hapiBroadPolicyRuleCounterAdd(policyId,ruleId,BROAD_COUNT_PACKETS,0)!=L7_SUCCESS) {
        hapiBroadPolicyRuleDelete(policyId,ruleId);
        break;
      }
    }
    // If something went wrong, remove created policies
    if (flow<PTIN_FLOW_TYPE_MAX) {
      break;
    }
  }

  // Someting went wrong... break cycle!
  if (dir<PTIN_PORTS_MAX)
  {
    hapiBroadPolicyDelete(policyId);
    printf("%s(%d) Something went wrong\r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Commit new policy
  if ((rc=hapiBroadPolicyCommit(policyId))!=L7_SUCCESS)
  {
    printf("%s(%d) Can't commit policy: rc=%d\r\n", __FUNCTION__, __LINE__,rc);
    hapiBroadPolicyDelete(policyId);
    return L7_TABLE_IS_FULL;
  }

  // Save policy id
  flow_counters->policy_id = policyId;

  // Save source data
  flow_counters->flow_id = flow_id;
  flow_counters->flow_type = flow_type;
  flow_counters->client_channel = client_channel;

  return L7_SUCCESS;
}

// Remove a Flow counter
//
L7_RC_t hapi_ptin_flow_counters_remove(uint16 flow_id, uint32 client_channel)
{
  uint8 i;
  st_ptin_fcounters *flow_counters;
  L7_RC_t rc, rc_global = L7_SUCCESS;

  // Validate arguments
  if (flow_id==0 || flow_id==(uint16)-1)  {
    printf("%s(%d) Invalid arguments\r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Run all fcounters elements
  for (i=0; i<FLOW_COUNTERS_MAX; i++) {
    rc= L7_SUCCESS;
    flow_counters = &ptin_fcounters[i];
    // array element must be in use
    if (flow_counters->flow_id==0 || flow_counters->flow_id==(uint16)-1)
      continue;
    // Check the elements to be cleared
    if (flow_counters->flow_id==flow_id &&
        (/*client_channel==0 ||*/ client_channel==(uint32)-1 || flow_counters->client_channel==client_channel))  {
      // Delete policy
      if (flow_counters->policy_id!=0) {
        if ((rc=hapiBroadPolicyDelete(flow_counters->policy_id))!=L7_SUCCESS) {
          printf("%s(%d) Error removing flowId=%u/client=%u counters\r\n", __FUNCTION__, __LINE__,flow_counters->flow_id,flow_counters->client_channel);
          rc_global = L7_FAILURE;
        }
        else {
          printf("%s(%d) Removed flowId=%u/client=%u counters\r\n", __FUNCTION__, __LINE__,flow_counters->flow_id,flow_counters->client_channel);
        }
      }
      // Clear array element
      if (rc==L7_SUCCESS)  memset(flow_counters,0x00,sizeof(st_ptin_fcounters));
    }
  }

  if (rc_global!=L7_SUCCESS)  {
    printf("%s(%d) Some error ocurred\r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

// Update procedure of flow counters (internal and periodic call)

static uint32 clock_t0[FLOW_COUNTERS_MAX];

L7_RC_t hapi_ptin_flow_counters_update(void)
{
  static int start=1; //, i;

  uint8 i;
  st_ptin_fcounters *flow_counters;
  uint32 numCounters;
  BROAD_POLICY_STATS_t counters[24];
  st_ptin_flow_stats   stats;
  uint32 clock_tnow, clock_tdelta;
  L7_clocktime ct;
  L7_RC_t rc;

  // first time initializations
  if (start) {
    sem_fcounters_access = osapiSemaBCreate(OSAPI_SEM_Q_FIFO,OSAPI_SEM_FULL);
    memset(clock_t0,0x00,sizeof(uint32)*FLOW_COUNTERS_MAX);
    start=0;
  }

  // Validate semaphore
  if (sem_fcounters_access==L7_NULL) {
    //printf("%s(%d) Semaphore not initializes\r\n", __FUNCTION__, __LINE__);
    return L7_SUCCESS;
  }

  for (i=0; i<FLOW_COUNTERS_MAX; i++) {
    flow_counters = &ptin_fcounters[i];

    // Validate array element
    if (flow_counters->flow_id==0 || flow_counters->flow_id==(uint16)-1 || flow_counters->policy_id==0)
      continue;

    // Read counters
    numCounters = PTIN_FLOW_TYPE_MAX * PTIN_PORTS_MAX;
    if ((rc=hapiBroadPolicyStatsGet(flow_counters->policy_id,&numCounters,counters))!=L7_SUCCESS || numCounters!=8) {
      memset(&flow_counters->stats_abs,0x00,sizeof(st_ptin_flow_stats));
      memset(&flow_counters->stats_diff,0x00,sizeof(st_ptin_flow_stats));
      continue;
    }

    // Extract time instant
    osapiClockTimeRaw ( &ct );
    // Clock now in milliseconds
    clock_tnow = ct.seconds*1000 + ct.nanoseconds/1000000;

    // Extract counters absolute values
    stats.pon_rx.pktUnicast   = counters[0].statMode.counter.count;
    stats.pon_rx.pktBroadcast = counters[1].statMode.counter.count;
    stats.pon_rx.pktMulticast = counters[2].statMode.counter.count;
    stats.pon_rx.pktDropped   = counters[3].statMode.counter.count;
    stats.pon_rx.pktTotal     = counters[0].statMode.counter.count + counters[1].statMode.counter.count + counters[2].statMode.counter.count;

    stats.eth_rx.pktUnicast   = counters[4].statMode.counter.count;
    stats.eth_rx.pktBroadcast = counters[5].statMode.counter.count;
    stats.eth_rx.pktMulticast = counters[6].statMode.counter.count;
    stats.eth_rx.pktDropped   = counters[7].statMode.counter.count;
    stats.eth_rx.pktTotal     = counters[4].statMode.counter.count + counters[5].statMode.counter.count + counters[6].statMode.counter.count;

    osapiSemaTake(sem_fcounters_access,L7_WAIT_FOREVER);

    // Calculate difference of packets, relatively to the last measure
    stats.pon_rx.pktUnicast   -= flow_counters->stats_abs.pon_rx.pktUnicast;
    stats.pon_rx.pktBroadcast -= flow_counters->stats_abs.pon_rx.pktBroadcast;
    stats.pon_rx.pktMulticast -= flow_counters->stats_abs.pon_rx.pktMulticast;
    stats.pon_rx.pktDropped   -= flow_counters->stats_abs.pon_rx.pktDropped;
    stats.pon_rx.pktTotal     -= flow_counters->stats_abs.pon_rx.pktTotal;

    stats.eth_rx.pktUnicast   -= flow_counters->stats_abs.eth_rx.pktUnicast;  
    stats.eth_rx.pktBroadcast -= flow_counters->stats_abs.eth_rx.pktBroadcast;
    stats.eth_rx.pktMulticast -= flow_counters->stats_abs.eth_rx.pktMulticast;
    stats.eth_rx.pktDropped   -= flow_counters->stats_abs.eth_rx.pktDropped;  
    stats.eth_rx.pktTotal     -= flow_counters->stats_abs.eth_rx.pktTotal;    

    // Save the absolute values
    flow_counters->stats_abs.pon_rx.pktUnicast   = counters[0].statMode.counter.count;
    flow_counters->stats_abs.pon_rx.pktBroadcast = counters[1].statMode.counter.count;
    flow_counters->stats_abs.pon_rx.pktMulticast = counters[2].statMode.counter.count;
    flow_counters->stats_abs.pon_rx.pktDropped   = counters[3].statMode.counter.count;
    flow_counters->stats_abs.pon_rx.pktTotal     = counters[0].statMode.counter.count + counters[1].statMode.counter.count + counters[2].statMode.counter.count;

    flow_counters->stats_abs.eth_rx.pktUnicast   = counters[4].statMode.counter.count;
    flow_counters->stats_abs.eth_rx.pktBroadcast = counters[5].statMode.counter.count;
    flow_counters->stats_abs.eth_rx.pktMulticast = counters[6].statMode.counter.count;
    flow_counters->stats_abs.eth_rx.pktDropped   = counters[7].statMode.counter.count;
    flow_counters->stats_abs.eth_rx.pktTotal     = counters[4].statMode.counter.count + counters[5].statMode.counter.count + counters[6].statMode.counter.count;

    // Calculate throughput
    // RX: if not the first instance...
    if (clock_t0[i]>0)  {
      // Difference of time
      clock_tdelta = clock_tnow-clock_t0[i];  // Difference of time in thousands of a second

      // Calculate throughput in pps
      if (clock_tdelta>0) {
        flow_counters->stats_diff.pon_rx.pktUnicast   = stats.pon_rx.pktUnicast  *1000/clock_tdelta;
        flow_counters->stats_diff.pon_rx.pktBroadcast = stats.pon_rx.pktBroadcast*1000/clock_tdelta;
        flow_counters->stats_diff.pon_rx.pktMulticast = stats.pon_rx.pktMulticast*1000/clock_tdelta;
        flow_counters->stats_diff.pon_rx.pktDropped   = stats.pon_rx.pktDropped  *1000/clock_tdelta;
        flow_counters->stats_diff.pon_rx.pktTotal     = stats.pon_rx.pktTotal    *1000/clock_tdelta;

        flow_counters->stats_diff.eth_rx.pktUnicast   = stats.eth_rx.pktUnicast  *1000/clock_tdelta;
        flow_counters->stats_diff.eth_rx.pktBroadcast = stats.eth_rx.pktBroadcast*1000/clock_tdelta;
        flow_counters->stats_diff.eth_rx.pktMulticast = stats.eth_rx.pktMulticast*1000/clock_tdelta;
        flow_counters->stats_diff.eth_rx.pktDropped   = stats.eth_rx.pktDropped  *1000/clock_tdelta;
        flow_counters->stats_diff.eth_rx.pktTotal     = stats.eth_rx.pktTotal    *1000/clock_tdelta;
      }
      // Update static variables
      if (clock_tdelta>0) {
        clock_t0[i] = clock_tnow;
      }
    }
    // If is the first instance...
    else {
      clock_t0[i] = clock_tnow;
    }

    osapiSemaGive(sem_fcounters_access);
  }

  return L7_SUCCESS;
}

// Get absolute flow counters values
//
L7_RC_t hapi_ptin_flow_absCounters_read(uint16 flow_id, uint32 client_channel, st_ptin_flow_stats *stats)
{
  st_ptin_fcounters *flow_counters;

  // Validate arguments
  if (flow_id==0 || flow_id==(uint16)-1)  {
    printf("%s(%d) Invalid arguments\r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Verify if this flow_id exists. If so, return error
  if ((flow_counters=ptin_find_flow_counters(flow_id,client_channel))==NULL || flow_counters->policy_id==0) {
    printf("%s(%d) Not found client\r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  /* Update measuring data */
  hapi_ptin_flow_counters_update();

  // Copy saved stats
  osapiSemaTake(sem_fcounters_access,L7_WAIT_FOREVER);
  memcpy(stats,&flow_counters->stats_abs,sizeof(st_ptin_flow_stats));
  osapiSemaGive(sem_fcounters_access);

  return L7_SUCCESS;
}

// Get differential (packets per second) flow counters values
//
L7_RC_t hapi_ptin_flow_diffCounters_read(uint16 flow_id, uint32 client_channel, st_ptin_flow_stats *stats)
{
  st_ptin_fcounters *flow_counters;

  // Validate arguments
  if (flow_id==0 || flow_id==(uint16)-1) {
    printf("%s(%d) Invalid arguments\r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  // Verify if this flow_id exists. If so, return error
  if ((flow_counters=ptin_find_flow_counters(flow_id,client_channel))==NULL || flow_counters->policy_id==0) {
    printf("%s(%d) Client not found\r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  /* Update measuring data */
  hapi_ptin_flow_counters_update();

  // Copy saved stats
  osapiSemaTake(sem_fcounters_access,L7_WAIT_FOREVER);
  memcpy(stats,&flow_counters->stats_diff,sizeof(st_ptin_flow_stats));
  osapiSemaGive(sem_fcounters_access);

  return L7_SUCCESS;
}


// Add broadcast rate limiter to a UNICAST flow
// Inputs:
//  flow_id       >> EVC id
//  pon_port_bmp  >> pon ports bitmap
//  eth_port_bmp  >> eth ports bitmap
//  pon_vlan      >> pon vlan
//  eth_vlan      >> eth vlan
// Returns:
//  0 = Success
//  other = failure
//
#define MAX_VLANS  60
//static L7_ushort16 bitstreamId_list[MAX_VLANS];
static L7_BOOL        policy_first_time = L7_TRUE;
static L7_ushort16    policy_vlan_list[MAX_VLANS];
static L7_uint32      policy_srcPort_list[MAX_VLANS];
static DAPI_USP_t     policy_dstPort_list[MAX_VLANS];
static BROAD_POLICY_t policyId_list[MAX_VLANS];
static L7_BOOL        policy_redirection[MAX_VLANS];
static L7_uint32      policy_trunk_id[MAX_VLANS];

L7_RC_t hapi_ptin_flow_bcastpack_limit(DAPI_USP_t *usp, uint8 enable, uint16 vlanId, DAPI_t *dapi_g)
{
  L7_uint8               index_free, index;
  L7_RC_t                result=L7_SUCCESS;
  BROAD_PORT_t          *hapiPortPtr, *hapiLagMemberPortPtr;
  DAPI_PORT_t           *dapiPortPtr;
  bcmx_lport_t           lport;
  L7_uint32              i;

  bcm_pbmp_t          inPorts;
  BROAD_SYSTEM_t     *hapiSystem;
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId  = BROAD_POLICY_RULE_INVALID;
  BROAD_METER_ENTRY_t meterInfo;
  BROAD_POLICY_TYPE_t policyType = BROAD_POLICY_TYPE_SYSTEM;
  uint8               to_be_dropped = 0;
  bcm_mac_t           dmac         = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  uint8               mask[16]     = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  uint8               mask_vlan[2] = { 0x0f, 0xf0 };

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;
  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);
  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);

  memset(&inPorts,0x00,sizeof(bcm_pbmp_t));

  // Initializations
  if (policy_first_time)
  {
    //printf("%s(%d) First time\r\n", __FUNCTION__, __LINE__);
    memset(policy_vlan_list,0x00,sizeof(L7_ushort16)*MAX_VLANS);
    memset(policy_srcPort_list,0x00,sizeof(L7_uint32)*MAX_VLANS);
    memset(policy_dstPort_list,0xFF,sizeof(DAPI_USP_t)*MAX_VLANS);
    memset(policyId_list,0xFF,sizeof(BROAD_POLICY_t)*MAX_VLANS);
    memset(policy_redirection,0x00,sizeof(policy_redirection));
    memset(policy_trunk_id,0xFF,sizeof(policy_trunk_id));
    policy_first_time = L7_FALSE;
  }

  // Rate limiter
  meterInfo.cir       = 64;
  meterInfo.cbs       = 64;
  meterInfo.pir       = 64;
  meterInfo.pbs       = 64;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
  meterInfo.ffpmtr_id = 0; /* Not used for XGS3 */
  meterInfo.port      = 0; /* Not used for XGS3 */

  /* Ignore last 4 bits (multiple of 16) */
  vlanId &= 0xfff0;

  // Validate VlanId
  if (vlanId<2 || vlanId>4093)
    return L7_FAILURE;

  // Find this vlan
  index_free = (L7_uint8)-1;
  for (index=0; index<MAX_VLANS; index++)
  {
    // Save the first free index
    if (index_free>=MAX_VLANS &&
        (policy_vlan_list[index]<2 || policy_vlan_list[index]>4093))
      index_free = index;
    // If there is a matching vlan, leave search cycle
    if (vlanId==policy_vlan_list[index])  break;
  }
  //printf("%s(%d) index=%u, index_free=%u\r\n", __FUNCTION__, __LINE__,index,index_free);
  // If Vlan was not found, use the first free index
  if (index>=MAX_VLANS)
  {
    //printf("%s(%d) Not found vlan %u\r\n", __FUNCTION__, __LINE__,vlanId);
    if (enable)
    {
      //printf("%s(%d) New vlan (%u) in index %u\r\n", __FUNCTION__, __LINE__,vlanId,index_free);
      // IF there is no free index, return error
      if (index_free>=MAX_VLANS)  return L7_TABLE_IS_FULL;
      index = index_free;
    }
    else
    {
      //printf("%s(%d) Vlan %u don't exist. Nothing to do\r\n", __FUNCTION__, __LINE__,vlanId);
      // Vlan don't exist... nothing to report, because the intention was to deactivate it
      return L7_SUCCESS;
    }
  }
  // Vlan was found
  else
  {
    //printf("%s(%d) Vlan %u found in index %u!\r\n", __FUNCTION__, __LINE__,vlanId,index);
  }

  // Remove interfaces
  // Only remove, if enable is FALSE and vlan+interface are valid
  if (!enable && policyId_list[index]!=BROAD_POLICY_INVALID)
  {
    //printf("%s(%d) Removing interfaces!\r\n", __FUNCTION__, __LINE__);

    if (IS_PORT_TYPE_CPU(dapiPortPtr))
    {
      //printf("%s(%d) Removing all interfaces!\r\n", __FUNCTION__, __LINE__);
      if (hapiBroadPolicyRemoveFromAll(policyId_list[index])!=L7_SUCCESS)
        result = L7_FAILURE;
    }
    else if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
    {
      //printf("%s(%d) I am here\r\n",__FUNCTION__,__LINE__);

      hapiBroadLagCritSecEnter();
      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
        {
          hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);
          lport = hapiLagMemberPortPtr->bcmx_lport;

          //printf("%s(%d) Dealing with interface 0x%X\r\n",__FUNCTION__,__LINE__,lport);

          if (hapiBroadPolicyRemoveFromIface(policyId_list[index],lport)!=L7_SUCCESS)
          {
            hapiBroadLagCritSecExit();
            //printf("%s(%d) Error removing interface 0x%X\r\n",__FUNCTION__,__LINE__,lport);
            return L7_FAILURE;
          }
          else
          {
            //printf("%s(%d) Removing interface 0x%X\r\n",__FUNCTION__,__LINE__,lport);
          }
        }
      }
      hapiBroadLagCritSecExit();
    }
    else
    {
      lport = hapiPortPtr->bcmx_lport;
      if (hapiBroadPolicyRemoveFromIface(policyId_list[index],lport)!=L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      else
      {
        //printf("%s(%d) Removing interface 0x%X\r\n",__FUNCTION__,__LINE__,lport);
      }
    }

    // Check if there is no interfaces
    for (i = 0; i < L7_MAX_PHYSICAL_PORTS_PER_UNIT; i++)
    {
      lport = bcmx_unit_port_to_lport(0, i);

      if (hapiBroadPolicyIfaceCheck(policyId_list[index],lport)==L7_ALREADY_CONFIGURED)
        break;
    }
    // If still there is some interfaces, there is nothing to do
    if (i<L7_MAX_PHYSICAL_PORTS_PER_UNIT)
    {
      //printf("%s(%d) There is still some interfaces\r\n",__FUNCTION__,__LINE__);
      return L7_SUCCESS;
    }
    //printf("%s(%d) No interfaces: Vlan will be removed\r\n",__FUNCTION__,__LINE__);
  }

  // Only manage the policies, if enable is FALSE, or if vlans differ, or if some policyId is null
  if (!enable ||
      policy_vlan_list[index]!=vlanId ||
      policyId_list[index]==BROAD_POLICY_INVALID)
  {
    // If some policyId is not null, delete both
    if (policyId_list[index]!=BROAD_POLICY_INVALID)
    {
      hapiBroadPolicyDelete(policyId_list[index]);
      policyId_list[index] = BROAD_POLICY_INVALID;
      policy_vlan_list[index] = 0;
      //printf("%s(%d) Removed policies\r\n", __FUNCTION__, __LINE__);
    }
    
    // Only create policies, if Enable is TRUE
    if ( enable )
    {
      //printf("%s(%d) enable is TRUE... [vlan=%u]\r\n", __FUNCTION__, __LINE__,vlanId);
      policyId = policyId_list[index];
      if (result == L7_SUCCESS && policyId_list[index] == BROAD_POLICY_INVALID)
      {
        policyId = BROAD_POLICY_INVALID;
        if ((result=hapiBroadPolicyCreate(policyType, &policyId))!=L7_SUCCESS)
        {
          //printf("%s(%d) Error creating policy\r\n",__FUNCTION__,__LINE__);
          return L7_TABLE_IS_FULL;
        }
  
        hapiBroadPolicyPriorityRuleAdd(policyId, &ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
        hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_INPORTS, (uint8 *)&inPorts,         (uint8 *) mask);
        hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_OVID   , (uint8 *)&vlanId,          (uint8 *) mask_vlan);
        hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_MACDA  , (uint8 *) dmac           , (uint8 *) mask);
        hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_DROP   , (uint8 *) &to_be_dropped , (uint8 *) mask);
        hapiBroadPolicyRuleNonConfActionAdd(policyId, ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
        hapiBroadPolicyRuleMeterAdd(policyId, ruleId, &meterInfo);
    
        if ((result=hapiBroadPolicyCommit(policyId))!=L7_SUCCESS)
        {
          //printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
          result = L7_TABLE_IS_FULL;    // Error to be returned
        }
      }
    
      //printf("%s(%d) activation done. Proceeding... PolicyId=%d\r\n", __FUNCTION__, __LINE__,policyId);
  
      // Check that no interfaces are associeate to the new policies
      if (result==L7_SUCCESS)
      {
        //printf("%s(%d) I am here\r\n",__FUNCTION__,__LINE__);
  
        // Remove all interfaces from the recently created policies
        if (hapiBroadPolicyRemoveFromAll(policyId)!=L7_SUCCESS)
        {
          //printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
          result = L7_FAILURE;
        }
      }
      else
      {
        //printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
      }
  
      // If everything went well, save the new policies Ids
      if (result==L7_SUCCESS)
      {
        policyId_list[index] = policyId;
        policy_vlan_list[index] = vlanId;
      }
      else
      {
        //printf("%s(%d) An error ocurred with (re)activation. Destroying what was created...\r\n", __FUNCTION__, __LINE__);
        /* attempt to delete the policy in case it was created */
        (void)hapiBroadPolicyDelete(policyId);
        policyId_list[index] = BROAD_POLICY_INVALID;
        policy_vlan_list[index] = 0;
      }
    }
    // If input parameters does not change anything, leave returning success
  }

  // Add interfaces
  // Only add, if enable is TRUE, and there is no error
  if (result == L7_SUCCESS && enable &&
      (policyId_list[index]!=BROAD_POLICY_INVALID) &&
      !IS_PORT_TYPE_CPU(dapiPortPtr) )
  {
    //printf("%s(%d) adding interfaces!\r\n", __FUNCTION__, __LINE__);

    // Add interfaces
    if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
    {
      //printf("%s(%d) I am here\r\n",__FUNCTION__,__LINE__);

      hapiBroadLagCritSecEnter();
      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        //printf("%s(%d) dapiPortPtr->modeparm.lag.memberSet[%d].inUse = %d\r\n",__FUNCTION__,__LINE__,i,dapiPortPtr->modeparm.lag.memberSet[i].inUse);
        if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
        {
          hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);
          lport = hapiLagMemberPortPtr->bcmx_lport;

          //printf("%s(%d) Dealing with interface 0x%X\r\n",__FUNCTION__,__LINE__,lport);

          if (hapiBroadPolicyApplyToIface(policyId_list[index],lport)!=L7_SUCCESS)
          {  
            hapiBroadLagCritSecExit();
            //printf("%s(%d) Error adding interface 0x%X\r\n",__FUNCTION__,__LINE__,lport);
            return L7_FAILURE;
          }
          else
          {
            //printf("%s(%d) Adding interface 0x%X\r\n",__FUNCTION__,__LINE__,lport);
          }
        }
      }
      hapiBroadLagCritSecExit();
    }
    else
    {
      lport = hapiPortPtr->bcmx_lport;
      if (hapiBroadPolicyApplyToIface(policyId_list[index],lport)!=L7_SUCCESS)
      {
        //printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
        return L7_FAILURE;
      }
      else
      {
        //printf("%s(%d) Adding interface 0x%X\r\n",__FUNCTION__,__LINE__,lport);
      }
    }

    //printf("%s(%d) success\r\n",__FUNCTION__,__LINE__);
    return L7_SUCCESS;
  }

  //printf("%s(%d) The End\r\n", __FUNCTION__, __LINE__);

  return result;
}

L7_RC_t hapi_ptin_flow_bitstream_upFwd(DAPI_USP_t *usp, uint8 admin, uint16 vlanId, DAPI_t *dapi_g)
{
  L7_uint8                index_free, index;

  //L7_uint16           vlanId;
  DAPI_USP_t          dst_port;
  L7_uint8            port;
  L7_uint32           port_bmp, port_bmp_tmp;
  bcm_pbmp_t          inPorts;
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId  = BROAD_POLICY_RULE_INVALID;
  BROAD_POLICY_TYPE_t policyType = BROAD_POLICY_TYPE_SYSTEM;
  uint8               to_be_dropped = 0;
  uint8               mask[16]     = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  uint8               mask_vlan[2] = { 0x0f, 0xf0 };

  BROAD_PORT_t    *hapiPortPtr;

  memset(&inPorts,0x00,sizeof(bcm_pbmp_t));

  // Initializations
  if (policy_first_time)
  {
    //printf("%s(%d) First time\r\n", __FUNCTION__, __LINE__);
    memset(policy_vlan_list,0x00,sizeof(L7_ushort16)*MAX_VLANS);
    memset(policy_srcPort_list,0x00,sizeof(L7_uint32)*MAX_VLANS);
    memset(policy_dstPort_list,0xFF,sizeof(DAPI_USP_t)*MAX_VLANS);
    memset(policyId_list,0xFF,sizeof(BROAD_POLICY_t)*MAX_VLANS);
    memset(policy_redirection,0x00,sizeof(policy_redirection));
    memset(policy_trunk_id,0xFF,sizeof(policy_trunk_id));
    policy_first_time = L7_FALSE;
  }

  /* Ignore last 4 bits (multiple of 16) */
  vlanId &= 0xfff0;

  // Validate VlanId
  if (usp==L7_NULLPTR || dapi_g==L7_NULLPTR || vlanId==0)
    return L7_FAILURE;

  // Find this vlan
  index_free = (L7_uint8)-1;
  for (index=0; index<MAX_VLANS; index++)
  {
    // Save the first free index
    if (index_free>=MAX_VLANS && (policy_vlan_list[index]==0))
      index_free = index;
    // If there is a matching vlan, leave search cycle
    if (vlanId==policy_vlan_list[index])  break;
  }
  printf("%s(%d) index=%u, index_free=%u\r\n", __FUNCTION__, __LINE__,index,index_free);
  // If Vlan was not found, use the first free index
  if (index>=MAX_VLANS)
  {
    printf("%s(%d) Not found vlan %u\r\n", __FUNCTION__, __LINE__,vlanId);
    if (admin)
    {
      printf("%s(%d) New vlan (%u) in index %u\r\n", __FUNCTION__, __LINE__,vlanId,index_free);
      // IF there is no free index, return error
      if (index_free>=MAX_VLANS)  return L7_TABLE_IS_FULL;
      index = index_free;
    }
    else
    {
      printf("%s(%d) Vlan %u don't exist. Nothing to do\r\n", __FUNCTION__, __LINE__,vlanId);
      // Vlan don't exist... nothing to report, because the intention was to deactivate it
      return L7_SUCCESS;
    }
  }
  // Vlan was found
  else
  {
    printf("%s(%d) Vlan %u found in index %u!\r\n", __FUNCTION__, __LINE__,vlanId,index);
  }

  printf("%s(%d) vlanID=%u, port={%d,%d,%d}\r\n", __FUNCTION__, __LINE__,vlanId,usp->unit,usp->slot,usp->port);

  // Initialize inputs
  port_bmp = 0;
  dst_port.unit = -1;
  dst_port.slot = -1;
  dst_port.port = -1;
  // Only get inputs from static table, if its entry is valid
  if (vlanId==policy_vlan_list[index])
  {
    port_bmp = policy_srcPort_list[index];
    dst_port = policy_dstPort_list[index];
  }

  printf("%s(%d) vlanID=%u, port_bmp=%u, dst_port={%d,%d,%d}\r\n", __FUNCTION__, __LINE__,vlanId,port_bmp,dst_port.unit,dst_port.slot,dst_port.port);

  // If is a GPON port, update port bitmap
  if (usp->slot==0 && usp->port<SYSTEM_N_GPON_PORTS)
  {
    port = usp->port;
    if (admin)
      port_bmp |= (L7_uint32) 1<<port;
    else
      port_bmp &= ~((L7_uint32) 1<<port);
  }
  // Otherwise, update destination port
  else
  {
    if (admin)
    {
      dst_port = *usp;
    }
    else
    {
      dst_port.unit = -1;
      dst_port.slot = -1;
      dst_port.port = -1;
    }
  }

  printf("%s(%d) vlanID=%u, port_bmp=%u, dst_port={%d,%d,%d}\r\n", __FUNCTION__, __LINE__,vlanId,port_bmp,dst_port.unit,dst_port.slot,dst_port.port);

  // If nothing changes, return success (Only for active services)
  if (vlanId==policy_vlan_list[index] &&
      port_bmp==policy_srcPort_list[index] &&
      dst_port.unit==policy_dstPort_list[index].unit &&
      dst_port.slot==policy_dstPort_list[index].slot &&
      dst_port.port==policy_dstPort_list[index].port &&
      policyId_list[index]!=BROAD_POLICY_INVALID)
  {
    printf("%s(%d) Nothing to do\r\n", __FUNCTION__, __LINE__);
    return L7_SUCCESS;
  }
  
  // Remove policy if it exists
  if (policyId_list[index]!=BROAD_POLICY_INVALID)
  {
    printf("%s(%d) Removing policy\r\n", __FUNCTION__, __LINE__);
    hapiBroadPolicyDelete(policyId_list[index]);
    policyId_list[index] = BROAD_POLICY_INVALID;
  }

  // Situation where the VLAN entry should be removed
  if (port_bmp==0 && (dst_port.unit==-1 && dst_port.slot==-1 && dst_port.port==-1))
  {
    printf("%s(%d) Removing entry %u\r\n", __FUNCTION__, __LINE__,index);
    policy_vlan_list[index] = 0;
    policy_srcPort_list[index] = 0;
    policy_dstPort_list[index].unit = -1;
    policy_dstPort_list[index].slot = -1;
    policy_dstPort_list[index].port = -1;
    policy_redirection [index] = L7_FALSE;
    return L7_SUCCESS;
  }

  // Save these parameters
  policy_vlan_list[index] = vlanId;
  policy_srcPort_list[index] = port_bmp;
  policy_dstPort_list[index] = dst_port;
  policy_redirection [index] = L7_TRUE;

  // Wait for the next instance to have the complete data
  if (vlanId==0 ||
      port_bmp==0 || 
      (dst_port.unit==-1 && dst_port.slot==-1 && dst_port.port==-1))
  {
    printf("%s(%d) Not enough data... waiting\r\n", __FUNCTION__, __LINE__);
    return L7_SUCCESS;
  }

  printf("%s(%d) i=%u, policy_trunk_id=%d\r\n", __FUNCTION__, __LINE__,index,policy_trunk_id[index]);

  // If destination port is a lag, and does not exist in hardware, abandon function
  hapiPortPtr = HAPI_PORT_GET(&dst_port,dapi_g);
  if (hapiPortPtr->port_is_lag && hapiPortPtr->hapiModeparm.lag.tgid==(L7_uint32)-1)
  {
    printf("%s(%d) Lag non-existent in hw\r\n", __FUNCTION__, __LINE__);
    return L7_SUCCESS;
  }

  printf("%s(%d) Preparing creation\r\n", __FUNCTION__, __LINE__);

  // Translate to hardware ports
  BCM_PBMP_CLEAR(inPorts);
  port_bmp_tmp = port_bmp;
  for (port=0; port<SYSTEM_N_PORTS; port++,port_bmp_tmp>>=1) {
    if (port_bmp_tmp & 1)  {
      //printf("%s(%d) Port %u added to pon ports\r\n", __FUNCTION__, __LINE__,port);
      BCM_PBMP_PORT_ADD(inPorts,usp_map[port].port);
    }
  }

  // Only manage the policies, if admin is FALSE, or if vlans differ, or if some policyId is null
    
  printf("%s(%d) I'm here\r\n", __FUNCTION__, __LINE__);

  policyId = policyId_list[index];
  if (policyId_list[index] == BROAD_POLICY_INVALID)
  {
    policyId = BROAD_POLICY_INVALID;
    if (hapiBroadPolicyCreate(policyType, &policyId)!=L7_SUCCESS)
    {
      printf("%s(%d) Error creating policy\r\n",__FUNCTION__,__LINE__);
//    policy_vlan_list[index] = 0;
//    policy_srcPort_list[index]=0;
//    policy_dstPort_list[index].unit=-1;
//    policy_dstPort_list[index].slot=-1;
//    policy_dstPort_list[index].port=-1;
      return L7_TABLE_IS_FULL;
    }

    hapiBroadPolicyPriorityRuleAdd(policyId, &ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
    hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_INPORTS, (uint8 *)&inPorts,       (uint8 *) mask);
    hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_OVID   , (uint8 *)&vlanId,        (uint8 *) mask_vlan);
    hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_DROP   , (uint8 *)&to_be_dropped, (uint8 *) mask);
    hapiBroadPolicyRuleActionAdd(policyId, ruleId, BROAD_ACTION_REDIRECT, dst_port.unit, dst_port.slot, dst_port.port);
    hapiBroadPolicyRuleActionAdd(policyId, ruleId, BROAD_ACTION_SET_COSQ_AS_PKTPRIO, 0, 0, 0);

    if (hapiBroadPolicyCommit(policyId)!=L7_SUCCESS)
    {
      printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
      (void)hapiBroadPolicyDelete(policyId);
//    policy_vlan_list[index] = 0;
//    policy_srcPort_list[index]=0;
//    policy_dstPort_list[index].unit=-1;
//    policy_dstPort_list[index].slot=-1;
//    policy_dstPort_list[index].port=-1;

      return L7_TABLE_IS_FULL;    // Error to be returned
    }

    policyId_list[index] = policyId;
    printf("%s(%d) Creation executed... PolicyId=%d\r\n", __FUNCTION__, __LINE__,policyId);
  }

  printf("%s(%d) activation done. Proceeding... PolicyId=%d\r\n", __FUNCTION__, __LINE__,policyId);

  return L7_SUCCESS;
}

L7_RC_t hapi_ptin_flow_bitstream_lagRecheck(DAPI_USP_t *usp, uint8 admin, DAPI_t *dapi_g)
{
  BROAD_PORT_t    *hapiPortPtr;
  L7_uint16        i;

  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);

  if (usp==L7_NULLPTR || dapi_g==L7_NULLPTR || hapiPortPtr==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  // Only apply this to LAGs
  if (usp->slot==0 || !hapiPortPtr->port_is_lag)
  {
    return L7_SUCCESS;
  }

  printf("%s(%d) Analysing interface {%d,%d,%d}\r\n",__FUNCTION__,__LINE__,usp->unit,usp->slot,usp->port);

  // Run all services which are using this interface
  for (i=0; i<MAX_VLANS; i++)
  {
    /* Only check for rules used for upstream flows */
    if (!policy_redirection[i])  continue;

    // Only analyse fully configured services
    if (policy_vlan_list[i]==0 ||
        policy_srcPort_list[i]==0 ||
        policy_dstPort_list[i].unit==-1 ||
        policy_dstPort_list[i].slot==-1 ||
        policy_dstPort_list[i].port==-1)
      continue;

    // For the services, which the configured LAG matches, update policy
    if (usp->unit==policy_dstPort_list[i].unit &&
        usp->slot==policy_dstPort_list[i].slot &&
        usp->port==policy_dstPort_list[i].port)
    {
      printf("%s(%d) Match found: index=%u (saved_policyId=%d, saved_trunk_id=%d, new_trunk_id=%d)\r\n",__FUNCTION__,__LINE__,
             i, 
             policyId_list[i],
             policy_trunk_id[i],
             hapiPortPtr->hapiModeparm.lag.tgid);
      // Deactivate
      if (policyId_list[i]!=BROAD_POLICY_INVALID &&
          hapiPortPtr->hapiModeparm.lag.tgid!=policy_trunk_id[i] /*(L7_uint32)-1*/)
      {
        printf("%s(%d) Deactivating rule...\r\n",__FUNCTION__,__LINE__);
        hapi_ptin_flow_bitstream_upFwd(usp, L7_FALSE, policy_vlan_list[i], dapi_g);
        // Restore destination interface (necessary to lift up this service)
        policy_dstPort_list[i].unit = usp->unit;
        policy_dstPort_list[i].slot = usp->slot;
        policy_dstPort_list[i].port = usp->port;
        /* Clear stored trunk id */
        policy_trunk_id[i] = (L7_uint32) -1;
      }

      // Activate
      if (policyId_list[i]==BROAD_POLICY_INVALID &&
          hapiPortPtr->hapiModeparm.lag.tgid!=(L7_uint32)-1 &&
          hapiPortPtr->hapiModeparm.lag.tgid!=policy_trunk_id[i])
      {
        printf("%s(%d) Reactivating rule...\r\n",__FUNCTION__,__LINE__);
        hapi_ptin_flow_bitstream_upFwd(usp, L7_TRUE, policy_vlan_list[i], dapi_g);
        /* Save new trunk_id */
        policy_trunk_id[i] = hapiPortPtr->hapiModeparm.lag.tgid;
      }
    }
  }

  return L7_SUCCESS;
}


uint16 hapiBroadPtinBCastLim_numberOfAvailableVlans(void)
{
  uint16 i, number;

  number = 0;
  for (i=0; i<MAX_VLANS; i++)
  {
    if (policy_vlan_list[i]>0 && policy_vlan_list[i]<(uint16)-1)  number++;
  }

  return (MAX_VLANS-number);
}


L7_RC_t ptin_systemPolicies(void)
{
  bcm_pbmp_t          inPorts;
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId  = BROAD_POLICY_RULE_INVALID;
  BROAD_METER_ENTRY_t meterInfo;
  BROAD_POLICY_TYPE_t policyType = BROAD_POLICY_TYPE_SYSTEM;
  uint8               L2StationMove = 1, L2SrcHit = 1;
  uint16              vlanId     = 1;
  uint16              etherType  = 0x8809;
  bcm_mac_t           dmac      = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x02 };
  uint8               mask[16]  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  L7_RC_t             result;

  // Only ethernet and 10G ports
  inPorts.pbits[0] = 0xfff00;

  // Forward LACP packets to CPU

  // Rate limiter
  meterInfo.cir       = 64;
  meterInfo.cbs       = 64;
  meterInfo.pir       = 64;
  meterInfo.pbs       = 64;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
  meterInfo.ffpmtr_id = 0; /* Not used for XGS3 */
  meterInfo.port      = 0; /* Not used for XGS3 */

  if ((result=hapiBroadPolicyCreate(policyType, &policyId))!=L7_SUCCESS)
    return result;

  hapiBroadPolicyPriorityRuleAdd(policyId, &ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_INPORTS, (uint8 *)&inPorts,         (uint8 *) mask);
  hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_MACDA  , (uint8 *) dmac           , (uint8 *) mask);
  hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_OVID   , (uint8 *)&vlanId,          (uint8 *) mask);
  hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_ETHTYPE, (uint8 *)&etherType      , (uint8 *) mask);
  hapiBroadPolicyRuleActionAdd(policyId, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
  hapiBroadPolicyRuleActionAdd(policyId, ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  hapiBroadPolicyRuleNonConfActionAdd(policyId, ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  hapiBroadPolicyRuleMeterAdd(policyId, ruleId, &meterInfo);

  result=hapiBroadPolicyCommit(policyId);

  // L2 Station move
  // Rate limiter
  meterInfo.cir       = 64;
  meterInfo.cbs       = 64;
  meterInfo.pir       = 64;
  meterInfo.pbs       = 64;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
  meterInfo.ffpmtr_id = 0; /* Not used for XGS3 */
  meterInfo.port      = 0; /* Not used for XGS3 */

  if ((result=hapiBroadPolicyCreate(policyType, &policyId))!=L7_SUCCESS)
    return result;

  hapiBroadPolicyPriorityRuleAdd(policyId, &ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_L2_STATION_MOVE, (uint8 *)&L2StationMove, (uint8 *) mask);
  hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_L2_SRC_HIT, (uint8 *)&L2SrcHit, (uint8 *) mask);
  hapiBroadPolicyRuleActionAdd(policyId, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_LOW_PRIORITY_COS, 0, 0);
  hapiBroadPolicyRuleActionAdd(policyId, ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  hapiBroadPolicyRuleNonConfActionAdd(policyId, ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  hapiBroadPolicyRuleMeterAdd(policyId, ruleId, &meterInfo);

  result=hapiBroadPolicyCommit(policyId);

  printf("%s(%d) Rate limiter for L2StationMove\r\n",__FUNCTION__,__LINE__);

  // Rate limiter for inband traffic

  meterInfo.cir       = 4096;
  meterInfo.cbs       = 128;
  meterInfo.pir       = 4096;
  meterInfo.pbs       = 128;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
  meterInfo.ffpmtr_id = 0; /* Not used for XGS3 */
  meterInfo.port      = 0; /* Not used for XGS3 */

  vlanId = 4093;

  if ((result=hapiBroadPolicyCreate(policyType, &policyId))!=L7_SUCCESS)
    return result;

  hapiBroadPolicyPriorityRuleAdd(policyId, &ruleId, BROAD_POLICY_RULE_PRIORITY_HIGHEST);
  hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_OVID, (uint8 *)&vlanId,          (uint8 *) mask);
  hapiBroadPolicyRuleNonConfActionAdd(policyId, ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  hapiBroadPolicyRuleMeterAdd(policyId, ruleId, &meterInfo);

  result=hapiBroadPolicyCommit(policyId);

  printf("%s(%d) Rate limit applied to inband management!\r\n",__FUNCTION__,__LINE__);

  return result;
}

/**
 * Dump all active counters
 * 
 * @param flow_id : EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE 
 */
L7_RC_t ptin_flow_counters_dump( L7_uint16 flow_id )
{
  L7_uint16 i;
  st_ptin_fcounters *flow_counters;

  if (flow_id==0 || flow_id==(uint16)-1)
  {
    printf("Invalid EVC id (%u)\r\n",flow_id);
    return L7_FAILURE;
  }

  printf("Active flow counters for EVC=%u:\r\n",flow_id);

  for (i=0; i<FLOW_COUNTERS_MAX; i++) {

    flow_counters = &ptin_fcounters[i];

    // array element must be in use
    if (flow_counters->flow_id==0 || flow_counters->flow_id==(uint16)-1)
      continue;

    // Check the elements to be cleared
    if (flow_counters->flow_id!=flow_id)
      continue;

    printf("  Client/Channel=0x%08x (policyId=%d)\r\n", flow_counters->client_channel,flow_counters->policy_id);
  }

  printf("Done!\r\n");

  return L7_SUCCESS;
}

/**
 * Dump all active counters
 * 
 * @param flow_id : EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE 
 */
L7_RC_t ptin_flow_counters_clear( L7_uint16 flow_id )
{
  L7_uint16 i;
  st_ptin_fcounters *flow_counters;

  if (flow_id==0 || flow_id==(uint16)-1)
  {
    printf("Invalid EVC id (%u)\r\n",flow_id);
    return L7_FAILURE;
  }

  printf("Active flow counters for EVC=%u:\r\n",flow_id);

  for (i=0; i<FLOW_COUNTERS_MAX; i++) {

    flow_counters = &ptin_fcounters[i];

    // array element must be in use
    if (flow_counters->flow_id==0 || flow_counters->flow_id==(uint16)-1)
      continue;

    // Check the elements to be cleared
    if (flow_counters->flow_id!=flow_id)
      continue;

    printf("  Removing client/Channel=0x%08x (policyId=%d) counter... ", flow_counters->client_channel, flow_counters->policy_id);
    if (hapi_ptin_flow_counters_remove(flow_id, flow_counters->client_channel)==L7_SUCCESS)
      printf("ok\r\n");
    else
      printf("ERROR\r\n");
  }

  printf("Done!\r\n");

  return L7_SUCCESS;
}

