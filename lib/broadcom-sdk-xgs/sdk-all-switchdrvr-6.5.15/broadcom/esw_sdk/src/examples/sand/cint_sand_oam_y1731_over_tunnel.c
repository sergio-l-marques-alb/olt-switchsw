/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~OAM test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * $Id: cint_oam_y1731.c,v 1.8 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * 
 * File: cint_sand_oam_y1371_over_tunnel.c
 * Purpose: Example of using Y.1731 OAM over MPLS-TP/PWE, for JR2 and above.
 *
 * Usage:
 * 
 For Jericho2, only the "new" MPLS calling sequence is used, using MPLS LIFs created with mpls_lsr_tunnel_example()
 cint dpp/utility/cint_utils_global.c
 cint dpp/cint_multi_device_utils.c
 cint dpp/utility/cint_utils_vlan.c
 cint dpp/utility/cint_utils_l2.c
 cint dnx/field/cint_field_utils.c
 cint sand/cint_ip_route_basic.c
 cint sand/utility/cint_sand_utils_oam.c
 cint dpp/utility/cint_utils_l3.c
 cint dpp/utility/cint_utils_mpls.c
 cint dpp/utility/cint_utils_mpls_port.c
 cint dpp/utility/cint_utils_oam.c
 cint dnx/utility/cint_dnx_utils_vpls.c
 cint sand/cint_vpls_mp_basic.c
 cint sand/cint_mpls_encapsulation_basic.c
 cint dpp/cint_qos.c
 cint dpp/cint_mpls_lsr.c
 cint dnx/oam/cint_oam_action.c
 cint dnx/crps/cint_crps_oam_config.c
 cint dnx/oam/cint_oam_basic.c
 cint sand/cint_sand_oam_y1731_over_tunnel.c
 cint
 print oam_run_with_defaults_mpls_tp(unit,13,14,15,1);
 
 For PWE, the tunnels created by vpls_mp_basic_main() are used:
 cint dpp/utility/cint_utils_global.c
 cint dpp/utility/cint_utils_l3.c
 cint dpp/utility/cint_utils_mpls.c
 cint dpp/utility/cint_utils_mpls_port.c
 cint dpp/utility/cint_utils_oam.c
 cint dnx/field/cint_field_utils.c
 cint sand/cint_ip_route_basic.c
 cint sand/cint_vpls_mp_basic.c
 cint dnx/oam/cint_oam_action.c
 cint dnx/crps/cint_crps_oam_config.c
 cint dnx/oam/cint_oam_basic.c
 cint sand/cint_sand_oam_y1731_over_tunnel.c
 cint
 print oam_run_with_defaults_pwe(unit,13,14,15);
 

 * 
 * This cint builds the following MAs:
 *         1) MA with id 1 & short name format:    1-const; 3-short format; 2-length; all the rest - MA name
 *        short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xcd, 0xab}
 * 
 * The following MEPs:
 *         1) MEP without ID: accelerated, MD-level 7,
 *         2) RMEP with ID = full entry threshold
 * 
 * 
 */
 
/* GLOBAL VAR*/

/** Jericho 1/Qumran and below only! */
int is_y1711 = 0;

int ingress_only_flag=0;
int egress_only_flag=0;
int is_pcp = 0;
int use_48_maid = 0;

/** Jericho 1/Qumran and below only! */
int sd_sf_enable=0;
int two_mep_sd_sf_flag=0;

int set_invalid_intf_id = 0;

oam__ep_info_s ep; /* store endpoint information*/
oam__ep_info_s pwe_mep; /* store endpoint information*/
oam__ep_info_s mpls_mep; /* store endpoint information*/

int lm_counter_base_id_1_mpls_ep;
int lm_counter_base_id_1_pwe_ep;

bcm_gport_t mpls_out_gport;

int port_1 = 13; /* physical port (signal generator)*/
int port_2 = 14;
int port_3 = 15;

bcm_oam_group_info_t group_info_short_ma;
bcm_oam_endpoint_info_t mep_acc_info;
bcm_oam_endpoint_info_t rmep_info;

/*
* If another PWE tunnel is needed, set this to true.
* The PWE label would be 2000
*/
int add_extra_pwe_tunnel = 0;

int timeout_events_count = 0;


/** Jericho2 and above only! */
/** value will be equalent to endpoint_memory_type in bcm_oam_endpoint_info_t structure */
int ep_memory_type = 0;

/** Indicate to which group the MEP will be assigned */
int is_short_maid = 1;

/* This is an example of using bcm_oam_event_register api.
 * A simple callback is created for CCM timeout event.
 * After a mep and rmep are created, the callback is called
 * whenever CCMTimeout event is generated.
 */
int cb(
    int unit,
    uint32 flags,
    bcm_oam_event_type_t event_type,
    bcm_oam_group_t group,
    bcm_oam_endpoint_t endpoint,
    void *user_data)
    {
        print unit;
        print flags;
        print event_type;
        print group;
        print endpoint;

        timeout_events_count++;

        return BCM_E_NONE;
    }

int register_events(int unit) {
  bcm_error_t rv;
  bcm_oam_event_types_t timeout_event, timein_event;

  BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);

  rv = bcm_oam_event_register(unit, timeout_event, cb, (void*)1);

  BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointCCMTimein);

  rv = bcm_oam_event_register(unit, timein_event, cb, (void*)2);
  return rv;
}

int read_timeout_event_count(int expected_event_count) {

    printf("timeout_events_count=%d\n",timeout_events_count);
    if (timeout_events_count==expected_event_count) {
        return BCM_E_NONE;
    }
    else {
        return BCM_E_FAIL;
    }
}

/**
 * Create accelerated y.1731 OAM endpoint of type MPLS-TP or
 * PWE.
 *
 *
 * @param unit
 * @param type May be bcmOAMEndpointTypeBHHMPLS or
 *             bcmOAMEndpointTypeBHHPwe
 *             bcmOAMEndpointTypeBHHPweGAL - Jericho, Qumran AX/UX or Jericho2 only
 *             bcmOAMEndpointTypeBhhSection - Jericho2
 * @param use_mpls_out_gport: used for TX counting 
 * 
 * @return int 
 */
int oam_example_over_tunnel(int unit,  bcm_oam_endpoint_type_t type, int use_mpls_out_gport) {
  bcm_error_t rv;
  bcm_oam_endpoint_info_t rmep_test_info;
  bcm_mpls_tunnel_switch_t tunnel_switch;
  bcm_oam_endpoint_info_t default_info;
  bcm_oam_group_info_t *group_info;
  bcm_oam_group_info_t group_info_48b_ma;
  bcm_oam_profile_t ingress_profile_id = 1, acc_ingress_profile = 1, egress_profile_id = BCM_GPORT_INVALID;
  int encap_id;
  int gport;
  int outlif , label;
  uint64  arg;
  int invalid_intf_id = 0x4007FFFF;
  int mpls_termination_label_index_enable;
  uint32 flags=0;
  bcm_gport_t out_gport = use_mpls_out_gport? 0 : BCM_GPORT_INVALID;
  uint8 group_Tx_MAID[BCM_OAM_GROUP_NAME_LENGTH] = { 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 
                                                     0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
                                                     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 
                                                     0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
                                                     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 
                                                     0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF };
  /*1-const; 3-short format; 2-length; all the rest - MA name*/
  uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};

  if (is_y1711) {
      unknown_label = 14;
      rv = mpls_add_gal_entry(unit);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n", bcm_errmsg(rv));
          return rv;
      }
  }

  if (type==bcmOAMEndpointTypeBHHMPLS) {
      if ((out_gport==BCM_GPORT_INVALID) && !is_device_or_above(unit, JERICHO2))  {
          /* In this case use the "standard" mpls_init function.*/
          printf("mpls_init\n");
           rv = mpls_init(unit, &tunnel_switch, &encap_id);
          if (rv != BCM_E_NONE) {
              printf("(%s) \n",bcm_errmsg(rv));
              return rv;
          }
          gport = tunnel_switch.tunnel_id;
          outlif = encap_id;
          label = BCM_MPLS_INDEXED_LABEL_VALUE_GET(tunnel_switch.label);/* Represents label on transmitted frames from the OAMP*/
       } else {
          /* Use the mpls_lsr example*/
          rv = mpls_lsr_tunnel_example(&unit, 1,port_1, port_2);
          if (rv != BCM_E_NONE) {
              printf("MPLS LSR example (%s) \n", bcm_errmsg(rv));
              return rv;
          }
          /* read mpls index soc property */
          if(is_device_or_above(unit, JERICHO2))
          {
              /** Not relevant for JR2 */
              mpls_termination_label_index_enable = 0;
          }
          else
          {
              mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
          }
          if (mpls_termination_label_index_enable) {
               gport = ingress_tunnel_id_indexed[0];
          }
          else {
              gport = ingress_tunnel_id;
          }         

          out_gport = mpls_lsr_info_1.ingress_intf;
   
          BCM_GPORT_TUNNEL_ID_SET(out_gport,mpls_lsr_info_1.ingress_intf );
          outlif = mpls_lsr_info_1.encap_id;
          label = 0x1234;

          mpls_mep.label = label;
          mpls_mep.mpls_out_gport =  out_gport;
          mpls_mep.out_lif = outlif;
          mpls_mep.gport = gport;

          if (device_type <= device_type_arad_plus) {
              /* In Jericho, GAL label is recognized by the special labels mechanism */
              rv = mpls_add_gal_entry(unit);
              if (rv != BCM_E_NONE) {
                  printf("(%s) \n", bcm_errmsg(rv));
                  return rv;
              }
          }
      }

  } else if (type==bcmOAMEndpointTypeBHHPwe ||
             type==bcmOAMEndpointTypeBHHPweGAL ||
             type==bcmOAMEndpointTypeBhhSection) {
      printf("pwe_init\n");
      if(is_device_or_above(unit, JERICHO2))
      {
          cint_vpls_basic_info.cw_present = 1;
          rv = vpls_mp_basic_main(unit, port_1, port_2);
          if (rv != BCM_E_NONE) {
              printf("(%s) \n",bcm_errmsg(rv));
              return rv;
          }
          if(type==bcmOAMEndpointTypeBhhSection) {
              /** Outer encapsulation port  */
              gport = section_oam_in_gport;

              /** Tunnel ID for encapsulation */
              outlif = cint_vpls_basic_info.core_arp_id;

              /** Constant GAL label for section OAM */
              label = 13;
              
              /** Same port is used for Rx and Tx */
              BCM_GPORT_TUNNEL_ID_SET(out_gport, cint_vpls_basic_info.core_arp_id);
          } else {
              /** Outer encapsulation port  */
              gport = cint_vpls_basic_info.mpls_port_id_ingress;

              /** Tunnel ID for encapsulation */
              outlif = cint_vpls_basic_info.mpls_tunnel_id;

              /** Constant label for PWE tunneling */
              label = 3000;

              /** Same port is used for Rx and Tx */
              out_gport = cint_vpls_basic_info.mpls_port_id_ingress;
          }
      }
      else
      {
          if (device_type <= device_type_arad_plus) {
            print device_type;
            if (type==bcmOAMEndpointTypeBHHPweGAL) {
                is_gal = 1;
            } else {
                pwe_cw = 1;
            }
          } else {
              pwe_cw=1;
          }
          rv = pwe_init(unit);
          if (rv != BCM_E_NONE) {
              printf("(%s) \n",bcm_errmsg(rv));
              return rv;
          }
          gport = mpls_lsr_info_1.mpls_port_id; /*in lif: mpls_port->mpls_port_id */
          outlif = mpls_lsr_info_1.encap_id; /* out lif: mpls_port->encap_id */
          label = 100; /* Represents label on transmitted frames from the OAMP*/
      }
  } else if (type==bcmOAMEndpointTypeMPLSNetwork){
  
      if(y1711_lsp_pwe_flag == 1){
        printf(" 1711 lsp_init\n");
        rv = mpls_lsr_tunnel_example(&unit, 1,port_1, port_2);
        if (rv != BCM_E_NONE) {
          printf("MPLS LSR example (%s) \n", bcm_errmsg(rv));
          return rv;
        }
        
        /* read mpls index soc property */
        mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
        if (mpls_termination_label_index_enable) {
          gport = ingress_tunnel_id_indexed[0];
        }
        else {
          gport = ingress_tunnel_id;
        }	
        out_gport = mpls_lsr_info_1.ingress_intf;
        BCM_GPORT_TUNNEL_ID_SET(out_gport,mpls_lsr_info_1.ingress_intf );
        outlif = mpls_lsr_info_1.encap_id;
        label = 0x1234;/* Represents label on transmitted frames from the OAMP*/
        
      }else {
         printf(" 1711 pwe_init\n");
         pwe_cw=0;
         rv = pwe_init(unit);
         if (rv != BCM_E_NONE) {
             printf("(%s) \n",bcm_errmsg(rv));
             return rv;
         }
         gport = mpls_lsr_info_1.mpls_port_id; /*in lif: mpls_port->mpls_port_id */
         outlif = mpls_lsr_info_1.encap_id; /* out lif: mpls_port->encap_id */
         label = 100; /* Represents label on transmitted frames from the OAMP*/
      }
    }else {
      printf("Only PWE and MPLS types supported."); 
      return 43;
  }

  if(is_device_or_above(unit, JERICHO2))
  {
      rv = set_counter_resource(unit, port_2, 0, &lm_counter_base_id_1_mpls_ep);
      BCM_IF_ERROR_RETURN(rv);
      rv = set_counter_resource(unit, port_1, 0, &lm_counter_base_id_1_pwe_ep);
      BCM_IF_ERROR_RETURN(rv);
  }
  else
  {
      rv = set_counter_source_and_engines(unit, &lm_counter_base_id_1_mpls_ep,port_1);
      BCM_IF_ERROR_RETURN(rv);
      rv = set_counter_source_and_engines(unit, &lm_counter_base_id_1_pwe_ep,port_2);
      BCM_IF_ERROR_RETURN(rv);
  }

  if (use_48_maid){ 
      /* Add a group with flexible 48 Byte MAID */ 
      bcm_oam_group_info_t_init(&group_info_48b_ma); 
      sal_memcpy(group_info_48b_ma.name, group_Tx_MAID, BCM_OAM_GROUP_NAME_LENGTH); 
      print group_info_48b_ma.name; 
      print group_Tx_MAID; 
      group_info_48b_ma.flags = BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE; 
      if (device_type >= device_type_qax) { 
          group_info_48b_ma.group_name_index = (device_type == device_type_qux) ? 0x816 : 0x2016; /* Bank 8, entry 22 */
          if(is_device_or_above(unit, JERICHO2))
          {
              /* For JR2 and above, group name index is in the OAM ID format */
              group_info_48b_ma.group_name_index = (group_info_48b_ma.group_name_index << 2);
          }
      } 
      rv = bcm_oam_group_create(unit, &group_info_48b_ma); 
      if (rv != BCM_E_NONE) { 
          printf("(%s) \n", bcm_errmsg(rv)); 
          return rv; 
      } 
      /* Set the used group for the MEPs to this group */ 
      group_info = &group_info_48b_ma; 
  } else { 
      printf("Creating group short name format\n");
      bcm_oam_group_info_t_init(&group_info_short_ma);
      sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
      rv = bcm_oam_group_create(unit, &group_info_short_ma);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n",bcm_errmsg(rv));
          return rv;
      }
      group_info = &group_info_short_ma;
  }

  if (is_pcp) 
  {
    if(is_device_or_above(unit, JERICHO2))
    {
       /*
        * Set Counter Range for PCP based counters.
        * As default all counter range defined as non-PCP.
        * To use PCP based counters, limited counter range
        * should be set. 
        * To set all counters for PCP, 
        * range_min = range_max = 0 should be used. 
        */

       /* Config Range to : Min=0, Max=0 . That allows the counters to be set */
       COMPILER_64_ZERO(arg);
       rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMin, arg);
       if (rv != BCM_E_NONE)
       {
           printf("Error no %d, in bcm_oam_control_set.\n", rv);
           return rv;
       }
       rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMax, arg);
       if (rv != BCM_E_NONE)
       {
           printf("Error no %d, in bcm_oam_control_set.\n", rv);
           return rv;
       }
    }
    else
    {
      rv = oam_qos_map_create(unit, BCM_GPORT_INVALID, BCM_GPORT_INVALID, BCM_GPORT_INVALID);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n",bcm_errmsg(rv));
          return rv;
      }
    }
  }

  if (!is_device_or_above(unit,ARAD_PLUS)) { 
    /*
    * Adding a default MEP
    */
    printf("Add default mep\n");
    bcm_oam_endpoint_info_t_init(&default_info);
    default_info.id = -1;
    default_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    rv = bcm_oam_endpoint_create(unit, &default_info);
    if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }
  }

  /*
  * Adding acc MEP
  */

  mpls_out_gport = out_gport;

  bcm_oam_endpoint_info_t_init(&mep_acc_info);

  if (ep_memory_type == 1)
  {
      mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeLmDmOffloadedEntry;
  }
  /** Profile */
  if(is_device_or_above(unit, JERICHO2))
  {

     /** Create ingress profiles */
     rv = oam_set_of_action_profiles_create(unit);
     if (rv != BCM_E_NONE)
     {
         printf("Error no %d, in oam_set_of_action_profiles_create.\n", rv);
         return rv;
     }

     if(type==bcmOAMEndpointTypeBHHMPLS || type==bcmOAMEndpointTypeBHHPweGAL)
      {
          /** Profiles for MPLS-TP */
          ingress_profile_id = oam_lif_profiles.oam_profile_mpls_single_ended;
          acc_ingress_profile = oam_acc_lif_profiles.oam_acc_profile_mpls_single_ended;
      }
      else
      {
          /** Profiles for PWE */
          ingress_profile_id = oam_lif_profiles.oam_profile_pwe_single_ended;
          acc_ingress_profile = oam_acc_lif_profiles.oam_acc_profile_pwe_single_ended;
      }

      mep_acc_info.acc_profile_id = acc_ingress_profile;
      flags = 0;
      rv = bcm_oam_lif_profile_set(unit, flags, gport, ingress_profile_id, egress_profile_id);
      if (rv != BCM_E_NONE)
      {
          printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
          return rv;
      }

      if(type==bcmOAMEndpointTypeBHHMPLS || type==bcmOAMEndpointTypeBhhSection)
      {
          /** Create egress profile - profile is used, but no actions are performed. */
          egress_profile_id = 1;
          rv = bcm_oam_profile_id_get_by_type(unit, egress_profile_id, bcmOAMProfileEgressLIF, &flags, &egress_profile_id);
          if (rv != BCM_E_NONE) {
            printf("bcm_oam_profile_id_get_by_type(egress) \n");
            return rv;
          }

          rv = bcm_oam_lif_profile_set(unit, flags, mpls_out_gport, ingress_profile_id, egress_profile_id);
          if (rv != BCM_E_NONE)
          {
              printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
              return rv;
          }

          /* Set channel type */
          bcm_oam_mpls_tp_channel_type_tx_set(0,bcmOamMplsTpChannelPweonoam,0x8902);
      }
  }

  /*RX*/
  mep_acc_info.type = type;
  mep_acc_info.group = group_info->id;
  mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
  mep_acc_info.level = (type==bcmOAMEndpointTypeMPLSNetwork) ? 0:7; /*Y1731 level*/
  mep_acc_info.gport = gport; /* in lif */
  mep_acc_info.mpls_out_gport = mpls_out_gport; /* out lif */
  mep_acc_info.lm_flags = BCM_OAM_LM_PCP * is_pcp;
  mep_acc_info.lm_counter_base_id  = lm_counter_base_id_1_mpls_ep;
  

  /*TX*/
  BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, port_2); /* port that the traffic will be transmitted on */
  mep_acc_info.name = 123;

  /*sd/sf testing use 100ms ccm tx period */
  if(sd_sf_enable){
      mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
  }
  else{
      if(is_device_or_above(unit, JERICHO2))
      {
          mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
      }
      else
      {
          mep_acc_info.ccm_period = 4;
      }
  }

  /** Relevant for Jericho 1 only */
  if(egress_only_flag ==1){
    mep_acc_info.flags2= BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY;
  }
  mep_acc_info.intf_id = set_invalid_intf_id ? invalid_intf_id : outlif;

  if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_injected_over_lsp_cnt",0)
    && (type == bcmOAMEndpointTypeBHHMPLS)
    && (y1711_cnt_1731_pkt_flag == 0)){
    /** Relevant for Jericho 1 only */
    /* Use below field to build GAL lable, ttl.exp */	
    /* MPLSTP-GAL: Label=13;Exp=0,BOS=1,TTL=64,which is from Y1731oMplstpGal*/ 
    mep_acc_info.egress_label.label = 13;
    mep_acc_info.egress_label.ttl = 0x40;
    mep_acc_info.egress_label.exp = 0;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    /* Put MEP's outlif to ITMH's oulitf*/
    mep_acc_info.intf_id = mpls_out_gport;
    bcm_oam_mpls_tp_channel_type_tx_set(0,bcmOamMplsTpChannelPweonoam,0x8902);
  } else{
    mep_acc_info.egress_label.label = label;
    mep_acc_info.egress_label.ttl = 0xa;
    mep_acc_info.egress_label.exp = 1;
    if (type == bcmOAMEndpointTypeBHHPweGAL) {
        /* Set MPLS-TP channel type as PWE GAL uses MPLS-TP MEP DB format*/
        bcm_oam_mpls_tp_channel_type_tx_set(0,bcmOamMplsTpChannelPweonoam,0x8902);
    }
  }
  if(ingress_only_flag ==1){
    mep_acc_info.flags2= BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY;
    mep_acc_info.intf_id = 0;
    mep_acc_info.egress_label.label = 0;
    mep_acc_info.egress_label.ttl = 0;
    mep_acc_info.egress_label.exp = 0;
  }

  /** Relevant for Jericho 1 only */
  if(!is_device_or_above(unit, JERICHO2))
  {
      /*The default timestamp_format is bcmOAMTimestampFormatNTP, set "oam_dm_ntp_enable" SOC to "0" to enable bcmOAMTimestampFormatIEEE1588v1*/
      mep_acc_info.timestamp_format = soc_property_get(unit,"oam_dm_ntp_enable",1) ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1;
  }

  if(type==bcmOAMEndpointTypeMPLSNetwork){
      printf("bcm_oam_endpoint_create 1711 MEP , Tunnel %d \n",tunnel_switch.tunnel_id); 
      mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S; /* LM only need to support 1s frequency */
      if(y1711_lsp_pwe_flag == 1){
           mep_acc_info.mpls_network_info.function_type = 0x01; /*indicate lsp lm*/
        }
      else{
           mep_acc_info.mpls_network_info.function_type = 0x02;/*indicate pwe lm*/
      }
  }

  printf("bcm_oam_endpoint_create mep_acc_info\n");
  rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  printf("created MEP with id %d\n", mep_acc_info.id);

  /*
  * Adding Remote MEP
  */
  bcm_oam_endpoint_info_t_init(&rmep_info);
  rmep_info.name = 0xff;
  rmep_info.local_id = mep_acc_info.id;
  rmep_info.type = type;
  rmep_info.ccm_period = sd_sf_enable ? 0 : 100;
  rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
  rmep_info.loc_clear_threshold = 1;

  printf("bcm_oam_endpoint_create RMEP\n"); 
  rv = bcm_oam_endpoint_create(unit, &rmep_info);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }
  printf("created RMEP with id %d\n", rmep_info.id);

  /* Store endpoint info in global parameter.*/
  ep.gport =  mep_acc_info.gport;
  ep.id = mep_acc_info.id;
  ep.rmep_id = rmep_info.id;


  bcm_oam_endpoint_info_t_init(&rmep_test_info);
  printf("bcm_oam_endpoint_get rmep_test_info\n"); 
  rv = bcm_oam_endpoint_get(unit, mep_acc_info.id, &rmep_test_info);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  if((type == bcmOAMEndpointTypeMPLSNetwork) && (1 == y1711_cnt_1731_pkt_flag)){
      rv = add_y1731_mep_over_tunnel(unit, group_info->id,((y1711_lsp_pwe_flag == 1) ? bcmOAMEndpointTypeBHHMPLS: bcmOAMEndpointTypeBHHPwe), gport,outlif,label,out_gport);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n",bcm_errmsg(rv));
          return rv;
      }
  }


  if((type == bcmOAMEndpointTypeMPLSNetwork) && (1 == two_1711_mep_flag) || (1 == two_mep_sd_sf_flag)){
      /* Y1711 1st MEP  to count traffic data only */
      printf(" ---1st --- 1711 MEP gport %x, outgport %x, outlif %x \n", gport,out_gport,outlif);
      /* Y1711 2nd MEP to count traffic dat & 1731 ccm/dm pkt*/
      rv = mpls_second_lsr_tunnel_create(&unit, 1,port_1, port_2);
      if (rv != BCM_E_NONE) {
          printf("MPLS LSR example (%s) \n", bcm_errmsg(rv));
          return rv;
      }
      /*After calling example_2, the global value gport,out_gport, oulit will be changed*/
      /* read mpls index soc property */
      mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
      if (mpls_termination_label_index_enable) {
          gport = ingress_tunnel_id_indexed[0];
      }
      else {
          gport = ingress_tunnel_id;
      }
      out_gport = mpls_lsr_info_1.ingress_intf;
      BCM_GPORT_TUNNEL_ID_SET(out_gport,mpls_lsr_info_1.ingress_intf );
      outlif = mpls_lsr_info_1.encap_id;
      label = 1234;/* Represents label on transmitted frames from the OAMP*/
      if(!two_mep_sd_sf_flag){
          printf(" --- 2nd ---  1711 MEP gport %x, outgport %x, outlif %x \n", gport,out_gport,outlif);
          rv = add_y1711_mep_over_tunnel(unit, group_info->id, gport,outlif,label,out_gport);
          if (rv != BCM_E_NONE) {
              printf("(%s) \n",bcm_errmsg(rv));
              return rv;
          }
      }
		
      rv = add_y1731_mep_over_tunnel(unit, group_info->id,bcmOAMEndpointTypeBHHMPLS, gport,outlif,label,out_gport);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n",bcm_errmsg(rv));
          return rv;
      }

  }

  return rv;
}

/**
 *
 *
 *
 * @param unit
 * @param port1 Port on which the MEP will exist.
 * @param port2
 * @param port3
 * @param use_mpls_out_gport - If set, MPLS outLIF on will be
 *                           associated with given value
 *
 * @return int
 */
int oam_run_with_defaults_mpls_tp (int unit, int port1, int port2, int port3, int use_mpls_out_gport) {
  bcm_error_t rv;
  port_1 = port1;
  port_2 = port2;
  port_3 = port3;

  int pwe_label = 2000;

  rv = mpls__mpls_pipe_mode_exp_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, in mpls__mpls__mpls_pipe_mode_exp_set\n");
      return rv;
  }

  rv = oam__device_type_get(unit, &device_type);
  if (rv < 0) {
      printf("Error checking whether the device is arad+.\n");
      print rv;
      return rv;
  }
 rv = oam_example_over_tunnel(unit, bcmOAMEndpointTypeBHHMPLS, use_mpls_out_gport);
 if (rv != BCM_E_NONE) {
     if(set_invalid_intf_id && (rv == BCM_E_PARAM)){
         printf(" Error checking successfully, invalid intf_id will return -4\n");
         return BCM_E_NONE;
     }
     printf("(%s) \n",bcm_errmsg(rv));
     return rv;
 }
  rv = register_events(unit);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  if (add_extra_pwe_tunnel) {
      /*
       * Create a PWE tunnel with label = 2000
       */
      mpls_port__ingress_only_info_s pwe_info;
      pwe_info.vpn = 100;
      pwe_info.flags = BCM_MPLS_PORT_CONTROL_WORD;
      pwe_info.ingress_matching_criteria = BCM_MPLS_PORT_MATCH_LABEL;
      pwe_info.ingress_pwe_label = pwe_label;
      BCM_GPORT_MPLS_PORT_ID_SET(pwe_info.mpls_port_id,((device_type == device_type_qux) ? 0x3000 : 0x4002));
      BCM_GPORT_FORWARD_PORT_SET(pwe_info.failover_port_id, 4096);
      pwe_info.port = BCM_GPORT_INVALID;
      print "Adding PWE tunnel:";
      rv = mpls_port__ingress_only_create(unit, &pwe_info);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n",bcm_errmsg(rv));
          return rv;
      }
      /*Fill the global structure*/
      pwe_mep.gport = pwe_info.mpls_port_id;
      pwe_mep.out_lif = pwe_info.encap_id;
      pwe_mep.label = pwe_label;
      if (add_extra_pwe_ep) {
          /*
           * Adding acc MEP
           */

          bcm_oam_endpoint_info_t_init(&mep_acc_info);
          /*RX*/
          mep_acc_info.type = bcmOAMEndpointTypeBHHPwe;
          mep_acc_info.group = is_short_maid ? group_info_short_ma.id : group_info_long_ma.id;
          mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
          mep_acc_info.level = 7; /*Y1731 level*/
          mep_acc_info.gport = pwe_info.mpls_port_id; /* in lif */
          mep_acc_info.mpls_out_gport = BCM_GPORT_INVALID; /* out lif */
          mep_acc_info.lm_counter_base_id  = count_pwe ? lm_counter_base_id_1_pwe_ep : 778; /* To indicate we dont want counting */
          mep_acc_info.lm_flags = BCM_OAM_LM_PCP * is_pcp;
          mep_acc_info.flags2 = BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY;

          /*TX*/
          BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, port_2); /* port that the traffic will be transmitted on */
          mep_acc_info.name = 345;

          mep_acc_info.ccm_period = 4;
          mep_acc_info.intf_id = pwe_info.encap_id;
          mep_acc_info.egress_label.label = pwe_label;
          mep_acc_info.egress_label.ttl = 0xa;
          mep_acc_info.egress_label.exp = 1;

          mep_acc_info.timestamp_format = soc_property_get(unit,"oam_dm_ntp_enable",1) ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1;

          printf("bcm_oam_endpoint_create mep_acc_info\n");
          rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
          if (rv != BCM_E_NONE) {
              printf("(%s) \n",bcm_errmsg(rv));
              return rv;
          }

          printf("created MEP with id %d\n", mep_acc_info.id);
          ep2.id = mep_acc_info.id;
     } else if (add_extra_default_pwe_ep) {

         bcm_oam_endpoint_info_t def_mep;

         /* Add default endpoint on 3rd LIF (Used for trapping OAM on more than 2 lifs hierarchy). */
         bcm_oam_endpoint_info_t_init(&def_mep);
         def_mep.level = 7; /* Y1731 level */
         def_mep.gport = pwe_info.mpls_port_id;
         def_mep.lm_flags = BCM_OAM_LM_PCP * is_pcp;
         def_mep.timestamp_format = soc_property_get(unit,"oam_dm_ntp_enable",1) ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1;
         def_mep.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS0;
         def_mep.flags |= BCM_OAM_ENDPOINT_WITH_ID;
         printf("bcm_oam_endpoint_create default_mep\n", i);

         rv = bcm_oam_endpoint_create(unit, &def_mep);
         BCM_IF_ERROR_RETURN(rv);
         printf("created MEP with id 0x%04x\n\tcounter: 0x%04x\n", def_mep.id, def_mep.lm_counter_base_id);


         printf("created MEP with id %d\n", mep_acc_info.id);
         ep2.id = mep_acc_info.id;
     }

  }


  return BCM_E_NONE;
}

int oam_run_with_defaults_pwe (int unit, int port1, int port2, int port3) {
  bcm_error_t rv;

  port_1 = port1;
  port_2 = port2;
  port_3 = port3;

  rv = mpls__mpls_pipe_mode_exp_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, in mpls__mpls__mpls_pipe_mode_exp_set\n");
      return rv;
  }


  rv = oam_example_over_tunnel(unit, bcmOAMEndpointTypeBHHPwe, 0);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  rv = register_events(unit);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  return rv;
}

int oam_run_with_defaults_pwe_gal (int unit, int port1, int port2, int port3) {
  bcm_error_t rv;

  port_1 = port1;
  port_2 = port2;
  port_3 = port3;

  rv = mpls__mpls_pipe_mode_exp_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, in mpls__mpls__mpls_pipe_mode_exp_set\n");
      return rv;
  }

  rv = oam__device_type_get(unit, &device_type);
  if (rv < 0) {
      printf("Error in getting device type\n");
      print rv;
      return rv;
  }


  rv = oam_example_over_tunnel(unit, bcmOAMEndpointTypeBHHPweGAL, 0);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  rv = register_events(unit);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  return rv;
}

/**
 * MPLS Section OAM example.
 * Creates basic mpls vpls scenario on 3 given ports and the folowing endpoint:
 * Accelerated section down MEP on port2 + RMEP
 *
 *
 * @param unit
 * @param port1
 * @param port2
 * @param port3
 *
 * @return int
 */
int oam_run_with_defaults_section (int unit, int port1, int port2, int port3) {
  bcm_error_t rv;

  port_1 = port1;
  port_2 = port2;
  port_3 = port3;

  rv = mpls__mpls_pipe_mode_exp_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, in mpls__mpls__mpls_pipe_mode_exp_set\n");
      return rv;
  }


  rv = oam_example_over_tunnel(unit, bcmOAMEndpointTypeBhhSection, 0);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  rv = register_events(unit);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  return rv;
}


