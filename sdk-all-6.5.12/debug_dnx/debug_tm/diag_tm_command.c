
// Jericho - Debug PP signals to IRE:
// ===============================

int print_functions()
{	
	printf("\n");
	printf("CINT diag_tm_command.c\n");
	printf("----------\n");
	printf("Functions:\n");
	printf("\t1. diag_pp(unit, core);\n");
	printf("\t2. diag_packet(unit, core);\n");
	printf("\n");
}

uint32  get_field(uint32  in,  int  first,  int  last){
	if(last  !=  31)
	  in  &=  (0x7FFFFFFF  >>  30-last);
	in  =  in  >>  first;
	if ((last-first+1)<32)
	  in  &=  ~(0xFFFFFFFF  <<  (last-first+1));
	return(in);
};


uint32  common_read_reg_long(int  unit_,  char  *reg_name,  uint32  *val,  int  nof_words){    
	reg_val  r_;    
	int  i;    
	if(diag_reg_get(unit_,  reg_name,  r_)  !=  BCM_E_NONE)
		return(0);    
	for(i  =  nof_words-1;  i  >=  0;  i--)
		val[i]  =  r_[i];      
	return(1);    
};  


uint32  insert_field(uint32  insert_to,  uint32  insert_val,  int  first,  int  last){
	uint32 insert_val_mask;
	insert_val_mask = 0xFFFFFFFF << first;
	
	if(last != 31)
	  insert_val_mask &= (0x7FFFFFFF >> 30-last);
	  
	insert_val = insert_val << first;
	insert_val &= insert_val_mask;
	insert_to &= ~insert_val_mask;
	
	return(insert_val | insert_to);
};

void  get_field_long(uint32*  in,  uint32*  out,  int  first,  int  last,  int  num_of_words){
	int  out_p  =  0    ;
	int  in_p   =  first;
	int  curr_val;  
	while(in_p  <=  last){
		int  out_possible_cp  =  32-out_p%32;
		int  in_possible_cp   =  ((32-in_p%32)  >  (last-in_p+1))?  (last-in_p+1):  32-in_p%32;
		int  nof_bits         =  (out_possible_cp  <  in_possible_cp)?  out_possible_cp:  in_possible_cp;
		curr_val              =  get_field(in[in_p/32],  in_p%32,  (in_p+nof_bits-1)%32);
		out[out_p/32]         =  insert_field(out[out_p/32],  curr_val,  out_p%32,  (out_p+nof_bits-1)%32);
		in_p                 +=  nof_bits;
		out_p                +=  nof_bits;    
	}
};  

void jericho_parse_tm_cmd (int unit, int core) {
  char    cmd[800];
  uint32  debug_data[11];
  uint32  DP;
  uint32  Destination;
  uint32  pkt_size;

  ////////// TM COMMAND //////////
  if (core==0) {
    common_read_reg_long(unit,"IRE_IHB_DEBUG_REGISTER_3(0)",&debug_data,11);
  }
  else {
    common_read_reg_long(unit,"IRE_IHB_DEBUG_REGISTER_3(1)",&debug_data,11);
  }
  //printf("DEBUG - 0x%08x%08x%08x%08x%08x%08x%08x%08x\n",debug_data[7],debug_data[6],debug_data[5],debug_data[4],debug_data[3],debug_data[2],debug_data[1],debug_data[0]);

  // Encoding TM Command:
  // IHP_TM_CMD_ETH_METER_PTR_BITS                  10:0 // 11
  // IHP_TM_CMD_LEARN_INFO_VALID_BIT                  11 // 1
  // IHP_TM_CMD_LEARN_INFO_BITS                  115:12 // 104
  // IHP_TM_CMD_LEARN_INFO_RESERVE_BITS          117:116 // 2
  // IHP_TM_CMD_DEST_VALID_BIT                       118 // 1
  // IHP_TM_CMD_SNOOP_CMD_BITS                   122:119 // 4
  // IHP_TM_CMD_MIRROR_CMD_BITS                  126:123 // 4
  // IHP_TM_CMD_DROP_PRECEDENCE_BITS             128:127 // 2
  // IHP_TM_CMD_TRAFFIC_CLASS_BITS               131:129 // 3
  // IHP_TM_CMD_DEST_BITS                        150:132 // 19
  // IHP_TM_CMD_IS_INGRESS_SHAPED_BIT                151 // 1
  // IHP_TM_CMD_INGRESS_SHAPING_DEST_BITS        168:152 // 17
  // IHP_TM_CMD_ETH_ENCAPSULATION_BITS           170:169 // 2
  // IHP_TM_CMD_STATISTICS_TAG_BITS              178:171 // 8
  // IHP_TM_CMD_ST_VSQ_PTR_BITS                  186:179 // 8
  // IHP_TM_CMD_METER_ACTION_BITS                222:187 // 36 = 2 x (17+1)
  // IHP_TM_CMD_COUNTER_ACTION_BITS              266:223 // 44 = 2 x (21+1)
  // IHP_TM_CMD_LAG_LB_KEY_BITS                  282:267 // 16
  // IHP_TM_CMD_DP_METER_COMMAND_BITS            284:283 // 2
  // IHP_TM_CMD_IGNORE_CP_BIT                        285 // 1
  // IHP_TM_CMD_LAG_MEMBER_VALID_BIT                 286 // 1
  // IHP_TM_CMD_IN_PP_PORT_BITS                  294:287 // 8
  // IHP_TM_CMD_NWK_HEADER_TRUNCATE_SIZE_BITS    302:295 // 8
  // IHP_TM_CMD_NWK_HEADER_APPEND_SIZE_PTR_BITS  310:303 // 8
  // IHP_TM_CMD_SNOOP_CODE_BITS                  318:311 // 8
  // IHP_TM_CMD_RESERVED_BITS                    322:319 // 4

  printf("Last TM Command from IHB to IRE (Core %d):\n",core);
  printf("-----------------------------------------\n");
  printf("  %-20s -  1 Bit  %8s  = 0x%x\n"  ,"Destination Valid" , "" , get_field(debug_data[3],22,22));
  printf("  %-20s -  4 Bits %8s  = 0x%01x\n","Snoop  Command"    , "" , get_field(debug_data[3],23,26));
  printf("  %-20s -  4 Bits %8s  = 0x%01x\n","Mirror Command"    , "" , get_field(debug_data[3],27,30));
  DP = insert_field(DP,get_field(debug_data[3],31,31),0,0);
  DP = insert_field(DP,get_field(debug_data[4],0,0),  1,1);
  printf("  %-20s -  2 Bits %8s  = 0x%01x\n","Drop Precedence"   , "" , DP);
  printf("  %-20s -  3 Bits %8s  = 0x%01x\n","Traffic Class"     , "" , get_field(debug_data[4],1,3));
  Destination = insert_field(Destination,get_field(debug_data[4],4,22),0,18);
  printf("  %-20s - 19 Bits %8s  = 0x%05x","Destination"       , "" , Destination);
  if (get_field(Destination,17,18)==3) { // Flow-ID
    printf(" (%s %d)","Flow ID",get_field(Destination,0,16));
  }
  else {
    if (get_field(Destination,17,18)==2) { // MC-ID
      printf(" (%s %d)","Multicast ID",get_field(Destination,0,16));
    }
    else {
      if (get_field(Destination,15,15)==1) { // LAG-ID
        printf(" (%s %d)","LAG ID",get_field(Destination,0,14));
      }
      else { // System Port
        printf(" (%s %d)","System Port",get_field(Destination,0,14));
      }
    }
  }
  printf("\n\n");
}

int jericho_ingress_get_diff_pckt_size (int unit, int core) {
  char    cmd[800];
  uint32  debug_data[1];
  uint32  bytes_to_strip;
  uint32  bytes_to_prepend;

  ////////// Bytes to Strip / Prepend //////////

  if (core==0) {
    common_read_reg_long(unit,"IRE_IHB_DEBUG_REGISTER_1(0)",&debug_data,1);
  }
  else {
    common_read_reg_long(unit,"IRE_IHB_DEBUG_REGISTER_1(1)",&debug_data,1);
  }

  bytes_to_strip    = insert_field(bytes_to_strip,  get_field(debug_data[0],0,6) ,0,6);
  bytes_to_prepend  = insert_field(bytes_to_prepend,get_field(debug_data[0],7,12),0,5);

  printf("Last Packet Size Ingress Editing (Command from IHB to IRE):\n");
  printf("-----------------------------------------------------------\n");
  printf("  %-40s = %d\n"  ,"Bytes to Remove     (-)" , bytes_to_strip);
  printf("  %-40s = %d\n"  ,"Bytes to Add        (+)" , bytes_to_prepend);
  if (bytes_to_prepend>=bytes_to_strip) {
    printf("  %-40s = %d\n","Total Bytes         (+)"          , bytes_to_prepend-bytes_to_strip);
  }
  else {
    printf("  %-40s = %d\n","Total Bytes         (-)"          , bytes_to_strip-bytes_to_prepend);
  }
  printf("\n");
  return (bytes_to_prepend-bytes_to_strip);
}

int jericho_egress_get_diff_pckt_size (int unit, int core) {
  char    cmd[800];
  uint32  debug_data[8];
  uint32  bytes_to_strip;
  uint32  bytes_to_prepend;

  ////////// Bytes to Strip / Prepend //////////

  // acc_pb_pp reg_access_arad signal_get eg addr_high 10 addr_low 0 msb 6 lsb 0 block_id 1   -> prge2post_bytes_to_strip_s [6 : 0]
  // acc_pb_pp reg_access_arad signal_get eg addr_high 10 addr_low 0 msb 13 lsb 7 block_id 1   -> prge2post_bytes_to_add_s [6 : 0]

  sprintf(cmd, "s EPNI_DBG_COMMAND DBG_ADDRESS=0xa0000,DBG_SAMPLE_ENABLE=0x1");
  bshell(unit, cmd);

  if (core==0) {
    common_read_reg_long(unit,"EPNI_DBG_DATA.EPNI0",&debug_data,8);
  }
  else {
    common_read_reg_long(unit,"EPNI_DBG_DATA.EPNI1",&debug_data,8);
  }

  //printf("DEBUG - 0x%08x%08x%08x%08x%08x%08x%08x%08x\n",debug_data[7],debug_data[6],debug_data[5],debug_data[4],debug_data[3],debug_data[2],debug_data[1],debug_data[0]);

  bytes_to_strip    = insert_field(bytes_to_strip,get_field(debug_data[0],0,6),0,6);
  bytes_to_prepend  = insert_field(bytes_to_prepend,get_field(debug_data[0],7,13),0,6);

  printf("Last Packet Size Egress Editing (EPNI Programmable Editor):\n");
  printf("-----------------------------------------------------------\n");
  printf("  %-40s = %d\n"  ,"Bytes to Remove     (-)" , bytes_to_strip);
  printf("  %-40s = %d\n"  ,"Bytes to Add        (+)" , bytes_to_prepend);
  if (bytes_to_prepend>=bytes_to_strip) {
    printf("  %-40s = %d\n","Total Bytes         (+)"          , bytes_to_prepend-bytes_to_strip);
  }
  else {
    printf("  %-40s = %d\n","Total Bytes         (-)"          , bytes_to_strip-bytes_to_prepend);
  }
  printf("\n");
  return (bytes_to_prepend-bytes_to_strip);
}

void jericho_ingress_parse_ftmh (int unit, int core) {
  char    cmd[800];
  uint32  reg_data;
  uint32  debug_data[14];
  uint32  system_headers_mode;
  uint32  FTMH[3];
  uint32  FTMH_MC_EXT;
  uint32  pkt_size;

  ////////// FTMH //////////
  if (core==0) {
    common_read_reg_long(unit,"IRE_IHB_DEBUG_REGISTER_2(0)",&debug_data,14);
  }
  else {
    common_read_reg_long(unit,"IRE_IHB_DEBUG_REGISTER_2(1)",&debug_data,14);
  }

  // Get Headers Mode
  common_read_reg_long(unit,"ECI_GLOBAL_SYS_HEADER_CFG",&reg_data,1);
  system_headers_mode = get_field(reg_data,5,6);

  // Jericho/Arad Headers Mode
  if ( (system_headers_mode==0) || (system_headers_mode==2) ) {
    FTMH[0] = insert_field(FTMH[0],get_field(debug_data[11],24,31),0 ,7);
    FTMH[0] = insert_field(FTMH[0],get_field(debug_data[12],0 ,23),8,31);
    FTMH[1] = insert_field(FTMH[1],get_field(debug_data[12],24,31),0 ,7);
    FTMH[1] = insert_field(FTMH[1],get_field(debug_data[13],0 ,23),8,31);
    FTMH[2] = insert_field(FTMH[2],get_field(debug_data[13],24,31),0 ,7);

    // Encoding Jericho/Arad FTMH:
    // ARAD_FTMH_RESERVED_BIT                       0 //  1
    // ARAD_FTMH_ECN_CAPABLE_BIT                    1 //  1
    // ARAD_FTMH_CNI_BIT                            2 //  1
    // ARAD_FTMH_DSP_EXT_PRESENT_BIT                3 //  1
    // ARAD_FTMH_MCID_OR_OUTLIF_BITS             22:4 // 19
    // ARAD_FTMH_TM_ACTION_IS_MC_BIT               23 //  1
    // ARAD_FTMH_OUT_MIRROR_DISABLE_BIT            24 //  1
    // ARAD_FTMH_PPH_TYPE_BITS                  26:25 //  2
    // ARAD_FTMH_TM_ACTION_TYPE_BITS            28:27 //  2
    // ARAD_FTMH_DP_BITS                        30:29 //  2
    // ARAD_FTMH_PP_DSP_BITS                    38:31 //  8
    // ARAD_FTMH_SRC_SYS_PORT_AGR_BITS          54:39 // 16
    // ARAD_FTMH_TRAFFIC_CLASS_BITS             57:55 //  3
    // ARAD_FTMH_PACKET_SIZE_BITS               71:58 // 14

    printf("Last Ingress FTMH from IHB to IRE (Jericho/Arad Headers Mode):\n");
    printf("--------------------------------------------------------------\n");
    printf("  %-20s           %8s  = 0x%08x%08x%08x\n","FTMH"               , ""        , FTMH[2],FTMH[1],FTMH[0]);
    printf("  %-20s - 19 Bits %8s  = 0x%05x\n"        ,"MCID / Outlif"      , "[22:4]"  , get_field(FTMH[0],4,22));
    printf("  %-20s -  1 Bit  %8s  = 0x%01x\n"        ,"Is Multicast"       , "[23]"    , get_field(FTMH[0],23,23));
    printf("  %-20s -  2 Bits %8s  = 0x%01x\n"        ,"TM Action Type"     , "[28:27]" , get_field(FTMH[0],27,28));
    printf("  %-20s -  2 Bits %8s  = 0x%01x\n"        ,"Drop Precedence"    , "[30:29]" , get_field(FTMH[0],29,30));
    printf("  %-20s - 16 Bits %8s  = 0x%04x\n"        ,"Source System Port" , "[54:39]" , get_field(FTMH[1],7,22));
    printf("  %-20s -  3 Bits %8s  = 0x%01x\n"        ,"Traffic Class"      , "[57:55]" , get_field(FTMH[1],23,25));
    pkt_size = insert_field(pkt_size,get_field(FTMH[1],26,31),0,5);
    pkt_size = insert_field(pkt_size,get_field(FTMH[2],0,7),  6,13);
    //printf("  %-20s - 14 Bits %8s  = 0x%04x\n"        ,"Packet Size"        , "[71:58]" , pkt_size);
  }
  else {
    // PetraB Headers Mode
    if (system_headers_mode==1) {
      FTMH[0] = insert_field(FTMH[0],get_field(debug_data[12],16,31) ,0 ,15);
      FTMH[0] = insert_field(FTMH[0],get_field(debug_data[13],0 ,15) ,16,31);
      FTMH[1] = insert_field(FTMH[1],get_field(debug_data[13],16 ,31),0 ,15);

      // Encoding PetraB FTMH:
      // P_FTMH_VERSION_BITS            47:46
      // P_FTMH_PACKET_SIZE_BITS        45:32
      // P_FTMH_TRAFFIC_CLASS_BITS      31:29
      // P_FTMH_SYS_SRC_PORT_BITS       28:16
      // P_FTMH_OTM_PORT_BITS            15:8
      // P_FTMH_OUT_FAP_PORT_BITS        15:8
      // P_FTMH_DP_BITS                   7:6
      // P_FTMH_ING_Q_SIG_BITS            5:4
      // P_FTMH_ACTION_TYPE_BITS          5:4
      // P_FTMH_CUD_SHORT_BITS            5:4
      // P_FTMH_PPH_PRESENT_BIT             3
      // P_FTMH_OUT_MIRROR_DISABLE_BIT      2
      // P_FTMH_EXCLUDE_SRC_BIT             1
      // P_FTMH_SYS_MC_BIT                  0

      // MC Extension
      FTMH_MC_EXT = get_field(debug_data[12],0,15);

      printf("Last Ingress FTMH from IHB to IRE (PetraB Headers Mode):\n");
      printf("--------------------------------------------------------\n");
      printf("  %-20s           %8s  = 0x%06x%08x%04x\n","FTMH"                , ""        , FTMH[1],FTMH[0],FTMH_MC_EXT);
      printf("  %-20s - 13 Bits %8s  = 0x%04x\n"        ,"MC-ID"               , "[7:0]"   , get_field(FTMH_MC_EXT,0 ,12));
      printf("  %-20s -  1 Bit  %8s  = 0x%01x\n"        ,"System MC"           , "[16]"     , get_field(FTMH[0],0 ,0));
      printf("  %-20s -  1 Bit  %8s  = 0x%01x\n"        ,"PPH Present"         , "[19]"    , get_field(FTMH[0],3 ,3));
      printf("  %-20s -  2 Bits %8s  = 0x%01x\n"        ,"TM Action Type"      , "[21:20]" , get_field(FTMH[0],4 ,5));
      printf("  %-20s -  2 Bits %8s  = 0x%01x\n"        ,"Drop Precedence"     , "[23:22]" , get_field(FTMH[0],6 ,7));
      printf("  %-20s - 13 Bits %8s  = 0x%04x\n"        ,"Source System Port"  , "[44:32]" , get_field(FTMH[0],16,28));
      printf("  %-20s -  3 Bits %8s  = 0x%01x\n"        ,"Traffic Class"       , "[57:45]" , get_field(FTMH[0],29,31));
    }
    else {
      printf("FTHM parsing not supported for System_Headers_Mode = %d !!!\n",system_headers_mode);
    }
  }
  printf("\n");
}

void jericho_egress_parse_ftmh (int unit, int core) {
  char    cmd[800];
  uint32  debug_data[8];
  uint32  sa[2];
  uint32  da[2];
  uint32  mcid;
  uint32  pkt_size;

  ////////// FTMH //////////
  // acc_pb_pp reg_access_arad signal_get eg addr_high 4 addr_low 0 msb 255 lsb 253 -> dspm2prp_system_headers_record_out_lif [2 : 0]
  // acc_pb_pp reg_access_arad signal_get eg addr_high 4 addr_low 1 msb 15 lsb 0    -> dspm2prp_system_headers_record_out_lif [18 : 3]
  // acc_pb_pp reg_access_arad signal_get eg addr_high 4 addr_low 1 msb 16 lsb 16   -> dspm2prp_system_headers_record_system_mc [0 : 0]
  // acc_pb_pp reg_access_arad signal_get eg addr_high 4 addr_low 1 msb 21 lsb 20   -> dspm2prp_system_headers_record_tm_action_type [1 : 0]
  // acc_pb_pp reg_access_arad signal_get eg addr_high 4 addr_low 1 msb 23 lsb 22   -> dspm2prp_system_headers_record_dp [1 : 0]
  // acc_pb_pp reg_access_arad signal_get eg addr_high 4 addr_low 1 msb 31 lsb 24   -> dspm2prp_system_headers_record_dsp_ptr [7 : 0]
  // acc_pb_pp reg_access_arad signal_get eg addr_high 4 addr_low 1 msb 47 lsb 32   -> dspm2prp_system_headers_record_src_system_port [15 : 0]
  // acc_pb_pp reg_access_arad signal_get eg addr_high 4 addr_low 1 msb 50 lsb 48   -> dspm2prp_system_headers_record_tc [2 : 0]
  // acc_pb_pp reg_access_arad signal_get eg addr_high 4 addr_low 1 msb 64 lsb 51   -> dspm2prp_system_headers_record_packet_size [13 : 0]

  // TODO - parse DA & SA ?!
  // acc_pb_pp reg_access_arad signal_get eg addr_high 4 addr_low 2 msb 89 lsb 42   -> dspm2prp_network_headers_record_eth_sa [47 : 0]
  // acc_pb_pp reg_access_arad signal_get eg addr_high 4 addr_low 2 msb 137 lsb 90  -> dspm2prp_network_headers_record_eth_da [47 : 0]

  sprintf(cmd, "s EGQ_DBG_COMMAND DBG_ADDRESS=0x40000,DBG_SAMPLE_ENABLE=1");
  bshell(unit, cmd);

  if (core==0) {
    common_read_reg_long(unit,"EGQ_DBG_DATA.EGQ0",&debug_data,8);
  }
  else {
    common_read_reg_long(unit,"EGQ_DBG_DATA.EGQ1",&debug_data,8);
  }

  //printf("DEBUG - 0x%08x%08x%08x%08x%08x%08x%08x%08x\n",debug_data[7],debug_data[6],debug_data[5],debug_data[4],debug_data[3],debug_data[2],debug_data[1],debug_data[0]);

  mcid    = insert_field(mcid,get_field(debug_data[7],29,31),0,2);

  sprintf(cmd, "s EGQ_DBG_COMMAND DBG_ADDRESS=0x40001,DBG_SAMPLE_ENABLE=1");
  bshell(unit, cmd);

  if (core==0) {
    common_read_reg_long(unit,"EGQ_DBG_DATA.EGQ0",&debug_data,8);
  }
  else {
    common_read_reg_long(unit,"EGQ_DBG_DATA.EGQ1",&debug_data,8);
  }
  //printf("DEBUG - 0x%08x%08x%08x%08x%08x%08x%08x%08x\n",debug_data[7],debug_data[6],debug_data[5],debug_data[4],debug_data[3],debug_data[2],debug_data[1],debug_data[0]);

  mcid    = insert_field(mcid,get_field(debug_data[0],0,15),3,18);

  printf("Last Egress FTMH in EGQ:\n");
  printf("------------------------\n");
  printf("  %-20s - 19 Bits %8s  = 0x%05x\n"        ,"MCID / Outlif"      , "" , mcid);
  printf("  %-20s -  1 Bit  %8s  = 0x%01x\n"        ,"Is Multicast"       , "" , get_field(debug_data[0],16,16));
  printf("  %-20s -  2 Bits %8s  = 0x%01x\n"        ,"TM Action Type"     , "" , get_field(debug_data[0],20,21));
  printf("  %-20s -  2 Bits %8s  = 0x%01x\n"        ,"Drop Precedence"    , "" , get_field(debug_data[0],22,23));
  printf("  %-20s -  8 Bits %8s  = 0x%02x\n"        ,"OTM Port"           , "" , get_field(debug_data[0],24 ,31));
  printf("  %-20s - 16 Bits %8s  = 0x%04x\n"        ,"Source System Port" , "" , get_field(debug_data[1],0,15));
  printf("  %-20s -  3 Bits %8s  = 0x%01x\n"        ,"Traffic Class"      , "" , get_field(debug_data[1],16,18));
  pkt_size = insert_field(pkt_size,get_field(debug_data[1],19,31),0 ,12);
  pkt_size = insert_field(pkt_size,get_field(debug_data[2],0 ,0) ,13,13);
  printf("  %-20s - 14 Bits %8s  = 0x%04x (%d Bytes)\n","Packet Size"     , "" , pkt_size,pkt_size);
  printf("\n");
}

// Parse the last buffer decision in the IDR
void jericho_diag_buffer_decision (int unit, int core) {
  char    cmd[800];
  uint32  debug_data[2];
  uint32  reg_data;
  uint32  dram_buffer = 0;
  uint32  Destination;
  uint32  snoop_cmd,mirror_cmd;
  uint32  dest_is_uc,dest_is_ing_mc,dest_is_egr_mc;
  uint32  queue_is_ocb_only,mcid_is_ocb_only,ctxt_is_ocb_only,tc_is_ocb_only;
  uint32  queue_is_ocb_eligible,queue_is_partial_dram_block,queue_is_full_dram_block,queue_is_dram_eligible,mcid_is_ocb_eligible;


  printf("Last Buffer Decision in the IDR:\n");
  printf("--------------------------------\n");
  common_read_reg_long(unit,"IDR_BUFFER_SELECTION_DECISION",&debug_data,2);

  // PacketDestination            18:0
  // PacketIsUnicast              19
  // PacketIsIngressMulticast     20
  // PacketIsEgressMulticast      21
  // QueueIsOcbOnly               22
  // McidIsOcbOnly                23
  // ReassemblyContextIsOcbOnly   24
  // TrafficClassIsOcbOnly        25
  // WantedBufferTypeIsOcbOnly    26
  // OcbOnlyBufferIsAvailable     27
  // BufferTypeIsOcbOnly          44 // was 28
  // QueueIsOcbEligible           29
  // McidIsOcbEligible            30
  // PacketSnoopCmd               34:31
  // PacketMirrorCmd              38:35
  // SnoopIsOcbEligible           39
  // MirrorIsOcbEligible          40
  // OcbShaperIsEnbaled           41
  // WantedBufferTypeIsOcbMix     42
  // OcbMixBufferIsAvailable      43
  // BufferTypeIsOcbMix           28 // was 44
  // DramBufferTypeTableKey       50:45
  // WantedBufferTypeIsFmc        51
  // WantedBufferTypeIsMmc        52
  // FmcBufferIsAvailable         53
  // MmcBufferIsAvailable         54
  // BufferTypeIsFmc              55
  // BufferTypeIsMmc              56
  // QueueIsFullDramReject        57
  // QueueIsPartialDramReject     58
  // McidIsDramReject             59
  // MmuIsBlocked                 60

  Destination = insert_field(Destination,get_field(debug_data[0],0,18),0,18);
  printf("  %-20s = 0x%05x","Destination"       , Destination);
  if (get_field(Destination,17,18)==3) { // Flow-ID
    printf(" (%s %d)","Flow ID",get_field(Destination,0,16));
  } else {
    if (get_field(Destination,17,18)==2) { // MC-ID
      printf(" (%s %d)","Multicast ID",get_field(Destination,0,16));
    } else {
      if (get_field(Destination,15,15)==1) { // LAG-ID
        printf(" (%s %d)","LAG ID",get_field(Destination,0,14));
      } else { // System Port
        printf(" (%s %d)","System Port",get_field(Destination,0,14));
      }
    }
  }
  dest_is_uc     = get_field(debug_data[0],19,19);
  dest_is_ing_mc = get_field(debug_data[0],20,20);
  dest_is_egr_mc = get_field(debug_data[0],21,21);
  if (dest_is_uc) { // UC
    printf(" - This is a Unicast Destination");
  } else {
    if (dest_is_ing_mc) { // Ing-MC
      printf(" - This is an Ingress-Multicast Destination");
    } else {
      if (dest_is_egr_mc) { // Egr-MC
        printf(" - This is an Egress-Multicast Destination");
      }
    }
  }
  printf("\n");
  snoop_cmd  = insert_field(snoop_cmd,get_field(debug_data[0],31,31),0,0);
  snoop_cmd  = insert_field(snoop_cmd,get_field(debug_data[1],0,2),1,3);
  mirror_cmd = get_field(debug_data[1],3,6);
  printf("  %-20s = 0x%01x\n","Snoop  Command"    , snoop_cmd);
  printf("  %-20s = 0x%01x\n","Mirror Command"    , mirror_cmd);

  queue_is_ocb_only = (dest_is_uc || dest_is_egr_mc) && get_field(debug_data[0],22,22);
  mcid_is_ocb_only  = dest_is_ing_mc && get_field(debug_data[0],23,23);
  ctxt_is_ocb_only  = get_field(debug_data[0],24,24);
  tc_is_ocb_only    = get_field(debug_data[0],25,25);
  if (queue_is_ocb_only) {
    printf("    The Destination Queue is OCB-only\n");
  }
  if (mcid_is_ocb_only) {
    printf("    The Destination Multicast-ID is OCB-only\n");
  }
  if (ctxt_is_ocb_only) {
    printf("    The Incoming Reassembly Context is OCB-only\n");
  }
  if (tc_is_ocb_only) {
    printf("    The Incoming Traffic-Class is OCB-only\n");
  }
  queue_is_ocb_eligible       = (dest_is_uc || dest_is_egr_mc) && get_field(debug_data[0],29,29);
  mcid_is_ocb_eligible        = dest_is_ing_mc && get_field(debug_data[0],30,30);
  queue_is_partial_dram_block = (dest_is_uc || dest_is_egr_mc) && get_field(debug_data[1],26,26);
  queue_is_full_dram_block    = (dest_is_uc || dest_is_egr_mc) && get_field(debug_data[1],25,25);
  queue_is_dram_eligible      = ~queue_is_ocb_eligible && ~queue_is_partial_dram_block && ~queue_is_full_dram_block;
  if ( (queue_is_ocb_only || mcid_is_ocb_only || ctxt_is_ocb_only || tc_is_ocb_only) && get_field(debug_data[0],26,26)) {
    printf("    ---> Wanted Buffer type is OCB-only\n");
    if (get_field(debug_data[0],27,27) && get_field(debug_data[1],12,12)) {
      printf("        ===> Buffer type is OCB-only\n");
    }
  } else {
    printf("    The Packet is Not Qualified as OCB-only\n");
    if (queue_is_ocb_eligible || mcid_is_ocb_eligible) {
      if (queue_is_ocb_eligible) {
        printf("    The Destination Queue is OCB-eligible\n");
      } else {
        if (mcid_is_ocb_eligible) {
          printf("    The Destination Multicast-ID is OCB-eligible\n");
        }
      }
      if ((snoop_cmd != 0) && ~get_field(debug_data[1],7,7)) {
        printf("    The Snoop Command is Not OCB-eligible\n");
      }
      if ((mirror_cmd != 0) && ~get_field(debug_data[1],8,8)) {
        printf("    The Mirror Command is Not OCB-eligible\n");
      }
      if (get_field(debug_data[1],9,9)) {
        printf("    The OCB Shaper is Enabled\n");
      } else {
        printf("    The OCB Shaper is Disabled\n");
        dram_buffer = 1;
      }
      if ( get_field(debug_data[1],10,10)) {
        printf("    ---> Wanted Buffer type is OCB-mix\n");
        if (get_field(debug_data[1],11,11) && get_field(debug_data[0],28,28)) {
          printf("        ===> Buffer type is OCB-mix\n");
        } else {
          common_read_reg_long(unit,"IDR_BUFFER_CHANGE_CONFIGURATION",&reg_data,1);
          if (get_field(reg_data,0,0)) {
            printf("    Enable Buffer Change from OCR to DRAM is Activated (IDR_BUFFER_CHANGE_CONFIGURATION)\n");
            dram_buffer = 1;
          }
        }
      }
    } else {
      printf("    The Packet is Not Qualified as OCB-eligible\n");
      common_read_reg_long(unit,"IDR_STATIC_CONFIGURATION",&reg_data,1);
      if (get_field(reg_data,8,8)) {
        printf("    ForceOcbForNonEligible Debug Feature is Activated (IDR_STATIC_CONFIGURATION)\n");
      } else {
        dram_buffer = 1;
      }
    }
    if (dram_buffer) {
      printf("  %-20s = 0x%01x\n","DRAM_BUFFER_TYPE Key"    , get_field(debug_data[1],13,18));
      if ( get_field(debug_data[1],19,19)) {
        printf("    ---> Wanted Buffer type is DRAM Full-Multicast\n");
        if (get_field(debug_data[1],21,21) && get_field(debug_data[1],23,23)) {
          printf("        ===> Buffer type is DRAM Full-Multicast\n");
        }
      }
      if ( get_field(debug_data[1],20,20)) {
        printf("    ---> Wanted Buffer type is DRAM Mini-Multicast\n");
        if (get_field(debug_data[1],22,22) && get_field(debug_data[1],24,24)) {
          printf("        ===> Buffer type is DRAM Mini-Multicast\n");
        }
      }
      if ( get_field(debug_data[1],27,27) && get_field(debug_data[1],28,28) ) {
        printf("    Multicast-ID is DRAM Reject, and MMU is Blocked\n");\
      }
    }
  }
  printf("\n");
}


// Parse the last packet constructor form IDR to IRR
void jericho_diag_idr_pc (int unit, int core) {
  char    cmd[800];
  uint32  debug_data[8];
  uint32  data_out[2];
  uint32  Destination;
  uint32  use_ocb,num_db,valid_bytes;
  uint32  dram_buff_size,ocb_buff_size;
  uint32  dram_buff_size_in_bytes,ocb_buff_size_in_bytes;
  uint32  packet_size;

  ////////// Packet Constructor //////////
  if (core==0) {
    common_read_reg_long(unit,"IDR_IRR_PACKET_CONSTRUCTOR(0)",&debug_data,8);
  }
  else {
    common_read_reg_long(unit,"IDR_IRR_PACKET_CONSTRUCTOR(1)",&debug_data,8);
  }

  // PcPdDestValid            0
  // PcPdDestIngMc            1
  // PcPdSnoopCmd             5:2
  // PcPdMirrorCmd            9:6
  // PcPdTrafficClass         12:10
  // PcPdDropPrecedence       14:13
  // PcPdResourceError        15
  // PcPdNumDbuffs            21:16
  // PcPdValidBytes           32:22
  // PcPdFullDramReject       33
  // PcPdSnoopCode            41:34
  // PcPdFatalError           42
  // PcPdPipe                 43
  // PcPdUseOcb               44
  // PcPdReserved             51:45
  // PcCdHeaderDelta          59:52
  // PcCdDest                 78:60
  // PcCdIsIngressShaped      79
  // PcCdIngressShapingDest   96:80
  // PcCdEthEncapsulation     98:97
  // PcCdIgnoreCp             99
  // PcCdCounterAction        143:100
  // PcCdLagLbKey             159:144
  // PcCdStVsqPtr             167:160
  // PcCdInPpPort             175:168
  // PcCdHeaderTruncate       183:176
  // PcCdHeaderAppend         191:184
  // PcCdStatisticsTag        199:192
  // PcCdDestEgressMc         200
  // PcCdUseOcbOnlyBuffer     201
  // PcCdReserved             205:202
  // PcDbuffPointer           227:206

  printf("Last Packet Constructor from IDR to IRR (Core %d):\n",core);
  printf("-------------------------------------------------\n");
  get_field_long(debug_data,data_out,0,0,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%x\n"  ,"Destination Valid"             , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,60,78,1);
  Destination = data_out[0];
  printf("  %-30s - 19 Bits %8s  = 0x%05x \t"  ,"Destination"                     , "" , Destination);
  if (get_field(Destination,17,18)==3) { // Flow-ID
    printf(" (%s %d)","Flow ID",get_field(Destination,0,16));
  } else {
    if (get_field(Destination,17,18)==1) { // Ingress MC-ID
      printf(" (%s %d)","Ingress Multicast ID",get_field(Destination,0,16));
    } else {
      if (get_field(Destination,17,18)==2) { // Egress MC-ID
        printf(" (%s %d)","Egress Multicast ID",get_field(Destination,0,16));
      } else {
        if (get_field(Destination,15,15)==1) { // LAG-ID
          printf(" (%s %d)","LAG ID",get_field(Destination,0,14));
        }  else { // System Port
          printf(" (%s %d)","System Port",get_field(Destination,0,14));
        }
      }
    }
  }
  printf("\n");
  get_field_long(debug_data,data_out,1,1,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%x\n"  ,"Destination is Ingress MC"     , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,200,200,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%x\n"  ,"Destination is Egress MC"      , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,79,79,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%x\n"  ,"Destination is Ingress Shape"  , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,80,96,1);
  printf("  %-30s - 17 Bits %8s  = 0x%05x\n","Ingress Shaping Destination"   , "" , get_field(data_out[0],0,16));

  get_field_long(debug_data,data_out,2,5,1);
  printf("  %-30s -  4 Bits %8s  = 0x%01x\n","Snoop  Command"                , "" , get_field(data_out[0],0,3));
  get_field_long(debug_data,data_out,34,41,1);
  printf("  %-30s -  8 Bits %8s  = 0x%01x\n","Snoop  Code"                   , "" , get_field(data_out[0],0,7));
  get_field_long(debug_data,data_out,6,9,1);
  printf("  %-30s -  4 Bits %8s  = 0x%01x\n","Mirror Command"                , "" , get_field(data_out[0],0,3));
  get_field_long(debug_data,data_out,13,14,1);
  printf("  %-30s -  2 Bits %8s  = 0x%01x\n","Drop Precedence"               , "" , get_field(data_out[0],0,1));
  get_field_long(debug_data,data_out,10,12,1);
  printf("  %-30s -  3 Bits %8s  = 0x%01x\n","Traffic Class"                 , "" , get_field(data_out[0],0,2));

  get_field_long(debug_data,data_out,44,44,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","Use OCB"                       , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,201,201,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","Use OCB-only"                  , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,206,227,1);
  printf("  %-30s - 22 Bits %8s  = 0x%06x\n","D-Buff Pointer"                , "" , get_field(data_out[0],0,21));

  get_field_long(debug_data,data_out,168,175,1);
  printf("  %-30s -  8 Bits %8s  = 0x%02x \t (%d)\n","In-PP Port"            , "" , get_field(data_out[0],0,7), get_field(data_out[0],0,7));
  get_field_long(debug_data,data_out,16,21,1);
  printf("  %-30s -  6 Bits %8s  = 0x%02x\n","Number of D-Buffers"           , "" , get_field(data_out[0],0,5));
  get_field_long(debug_data,data_out,22,32,1);
  printf("  %-30s - 11 Bits %8s  = 0x%03x \t (%d)\n","Valid Bytes (in Last Buffer)"  , "" , get_field(data_out[0],0,10), get_field(data_out[0],0,10));
  get_field_long(debug_data,data_out,52,59,1);
  printf("  %-30s -  8 Bits %8s  = 0x%02x\n","Header Delta"                  , "" , get_field(data_out[0],0,7));
  get_field_long(debug_data,data_out,176,183,1);
  printf("  %-30s -  8 Bits %8s  = 0x%02x\n","Header Truncate"               , "" , get_field(data_out[0],0,7));
  get_field_long(debug_data,data_out,184,191,1);
  printf("  %-30s -  8 Bits %8s  = 0x%02x\n","Header Append"                 , "" , get_field(data_out[0],0,7));
  get_field_long(debug_data,data_out,15,15,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","Resource Error"                , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,42,42,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","Fatal Error"                   , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,33,33,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","Full DRAM Reject"              , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,99,99,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","Ignore CP"                     , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,97,98,1);
  printf("  %-30s -  2 Bits %8s  = 0x%01x\n","Ethernet Encapsulation"        , "" , get_field(data_out[0],0,1));
  get_field_long(debug_data,data_out,100,143,2);
  printf("  %-30s - 44 Bits %8s  = 0x%03x%08x\n","Counter Action"            , "" , get_field(data_out[1],0,11),data_out[0]);
  get_field_long(debug_data,data_out,144,159,1);
  printf("  %-30s - 16 Bits %8s  = 0x%04x\n","LAG Load-Balancing Key"        , "" , get_field(data_out[0],0,15));
  get_field_long(debug_data,data_out,160,167,1);
  printf("  %-30s -  8 Bits %8s  = 0x%02x\n","VSQ Pointer"                   , "" , get_field(data_out[0],0,7));
  get_field_long(debug_data,data_out,192,199,1);
  printf("  %-30s -  8 Bits %8s  = 0x%02x\n","Statistics Tag"                , "" , get_field(data_out[0],0,7));

  // Calculate Packet Size
  get_field_long(debug_data,data_out,44,44,1);
  use_ocb = get_field(data_out[0],0,0);
  get_field_long(debug_data,data_out,16,21,1);
  num_db = get_field(data_out[0],0,5);
  get_field_long(debug_data,data_out,22,32,1);
  valid_bytes = get_field(data_out[0],0,10);
  common_read_reg_long(unit,"ECI_GLOBAL_GENERAL_CFG_3",&debug_data,1);
  dram_buff_size          = get_field(debug_data[0],1,2);
  dram_buff_size_in_bytes = (1<<dram_buff_size)*256;
  ocb_buff_size           = get_field(debug_data[0],3,3);
  ocb_buff_size_in_bytes  = (1<<ocb_buff_size)*256;
  if (use_ocb) {
    packet_size = ocb_buff_size_in_bytes * num_db + valid_bytes + 1;
    printf("\n  %-30s = %d Bytes (%d * %d B + %d)\n","Packet Size (OCB)"                 , packet_size, num_db, ocb_buff_size_in_bytes,  valid_bytes+1);
  } else {
    packet_size = dram_buff_size_in_bytes * num_db + valid_bytes + 1;
    printf("\n  %-30s = %d Bytes (%d * %d B + %d)\n","Packet Size (DRAM)"                , packet_size, num_db, dram_buff_size_in_bytes, valid_bytes+1);
  }
  printf("\n");
}


// Parse the last enqueue command form IRR to IQM
void jericho_diag_enqueue_cmd (int unit, int core) {
  char    cmd[800];
  uint32  debug_data[8];
  uint32  data_out[2];
  uint32  fwd_type,fwd_act,irr_q;
  char    fwd_type_parsed[20],fwd_act_parsed[20],irr_q_parsed[20];

  ////////// Enqueue Command //////////
  if (core==0) {
    common_read_reg_long(unit,"IRR_IQM_ENQUEUE_COMMAND(0)",&debug_data,8);
  }
  else {
    common_read_reg_long(unit,"IRR_IQM_ENQUEUE_COMMAND(1)",&debug_data,8);
  }

  // 2NdDest            18:0
  // CounterAction      62:19
  // Db                 84:63
  // DramReject         85
  // Dropp              87:86
  // EcnCap             88
  // FwdAct             90:89
  // FwdType            92:91
  // HdrAppendSizePtr   100:93
  // HdrDelta           108:101
  // HdrTruncateSize    116:109
  // IgnoreCp           117
  // IrrMcrEngId        119:118
  // IrrQ               121:120
  // Laghash            137:122
  // Lst                138
  // McErr              139
  // NumDb              145:140
  // OcbOnly            146
  // Outlif             165:147
  // PpPort             173:166
  // Qnum               190:174
  // QValid             191
  // ResourceErr        192
  // Rstusr             193
  // SchCmpPtr          198:194
  // Sopc               199
  // StVsqPtr           207:200
  // Tc                 210:208
  // ValidBytes         221:211

  printf("Last Enqueue Command from IRR to IQM (Core %d):\n",core);
  printf("----------------------------------------------\n");
  get_field_long(debug_data,data_out,191,191,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%x\n"  ,"Queue Valid"                   , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,174,190,1);
  printf("  %-30s - 17 Bits %8s  = 0x%05x \t (%d)\n"  ,"Queue Number"        , "" , get_field(data_out[0],0,16), get_field(data_out[0],0,16));
  get_field_long(debug_data,data_out,91,92,1);
  fwd_type = get_field(data_out[0],0,1);
  if        (fwd_type == 0) { sprintf(fwd_type_parsed, "Unicast");}
  else { if (fwd_type == 1) { sprintf(fwd_type_parsed, "Ingress MC");}
  else { if (fwd_type == 2) { sprintf(fwd_type_parsed, "Fabric MC");}
  else { if (fwd_type == 3) { sprintf(fwd_type_parsed, "Ingress Shaping");}}}}
  printf("  %-30s -  2 Bits %8s  = 0x%01x \t (%s)\n","Forward Type"          , "" , fwd_type, fwd_type_parsed);
  get_field_long(debug_data,data_out,89,90,1);
  fwd_act = get_field(data_out[0],0,1);
  if        (fwd_act == 0) { sprintf(fwd_act_parsed, "Normal");}
  else { if (fwd_act == 1) { sprintf(fwd_act_parsed, "Snoop");}
  else { if (fwd_act == 2) { sprintf(fwd_act_parsed, "Mirror");}
  else { if (fwd_act == 3) { sprintf(fwd_act_parsed, "Invalid !!!");}}}}
  printf("  %-30s -  2 Bits %8s  = 0x%01x \t (%s)\n","Forward Action"        , "" , fwd_act, fwd_act_parsed);
  get_field_long(debug_data,data_out,120,121,1);
  irr_q = get_field(data_out[0],0,1);
  if        (irr_q == 0) { sprintf(irr_q_parsed, "Multicast LP");}
  else { if (irr_q == 1) { sprintf(irr_q_parsed, "Multicast HP");}
  else { if (irr_q == 2) { sprintf(irr_q_parsed, "Ingress Shape");}
  else { if (irr_q == 3) { sprintf(irr_q_parsed, "Unicast");}}}}
  printf("  %-30s -  2 Bits %8s  = 0x%01x \t (%s)\n","IRR Queue"             , "" , irr_q, irr_q_parsed);
  get_field_long(debug_data,data_out,118,119,1);
  printf("  %-30s -  2 Bits %8s  = 0x%01x\n","Multicast Engine ID"           , "" , get_field(data_out[0],0,1));
  get_field_long(debug_data,data_out,0,18,1);
  printf("  %-30s - 19 Bits %8s  = 0x%05x\n","2nd Destination"               , "" , get_field(data_out[0],0,18));

  get_field_long(debug_data,data_out,86,87,1);
  printf("  %-30s -  2 Bits %8s  = 0x%01x\n","Drop Precedence"               , "" , get_field(data_out[0],0,1));
  get_field_long(debug_data,data_out,208,210,1);
  printf("  %-30s -  3 Bits %8s  = 0x%01x\n","Traffic Class"                 , "" , get_field(data_out[0],0,2));
  get_field_long(debug_data,data_out,146,146,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","Use OCB-only"                  , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,63,84,1);
  printf("  %-30s - 22 Bits %8s  = 0x%06x\n","D-Buff Pointer"                , "" , get_field(data_out[0],0,21));
  get_field_long(debug_data,data_out,193,193,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","Reset User Count"              , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,138,138,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","Last"                          , "" , get_field(data_out[0],0,0));

  get_field_long(debug_data,data_out,166,173,1);
  printf("  %-30s -  8 Bits %8s  = 0x%02x \t (%d)\n","In-PP Port"            , "" , get_field(data_out[0],0,7), get_field(data_out[0],0,7));
  get_field_long(debug_data,data_out,140,145,1);
  printf("  %-30s -  6 Bits %8s  = 0x%02x\n","Number of D-Buffers"           , "" , get_field(data_out[0],0,5));
  get_field_long(debug_data,data_out,211,221,1);
  printf("  %-30s - 11 Bits %8s  = 0x%03x \t (%d)\n","Valid Bytes (in Last Buffer)"  , "" , get_field(data_out[0],0,10), get_field(data_out[0],0,10));
  get_field_long(debug_data,data_out,101,108,1);
  printf("  %-30s -  8 Bits %8s  = 0x%02x\n","Header Delta"                  , "" , get_field(data_out[0],0,7));
  get_field_long(debug_data,data_out,109,116,1);
  printf("  %-30s -  8 Bits %8s  = 0x%02x\n","Header Truncate"               , "" , get_field(data_out[0],0,7));
  get_field_long(debug_data,data_out,93,100,1);
  printf("  %-30s -  8 Bits %8s  = 0x%02x\n","Header Append"                 , "" , get_field(data_out[0],0,7));
  get_field_long(debug_data,data_out,194,198,1);
  printf("  %-30s -  8 Bits %8s  = 0x%02x\n","Sch Compensation Pointer"      , "" , get_field(data_out[0],0,4));
  get_field_long(debug_data,data_out,192,192,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","Resource Error"                , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,139,139,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","Multicast Error"               , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,85,85,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","DRAM Reject"                   , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,117,117,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","Ignore CP"                     , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,88,88,1);
  printf("  %-30s -  1 Bit  %8s  = 0x%01x\n","ECN Encapsulation"             , "" , get_field(data_out[0],0,0));
  get_field_long(debug_data,data_out,19,62,2);
  printf("  %-30s - 44 Bits %8s  = 0x%03x%08x\n","Counter Action"            , "" , get_field(data_out[1],0,11),data_out[0]);
  get_field_long(debug_data,data_out,122,137,1);
  printf("  %-30s - 16 Bits %8s  = 0x%04x\n","LAG Hash"                      , "" , get_field(data_out[0],0,15));
  get_field_long(debug_data,data_out,200,207,1);
  printf("  %-30s -  8 Bits %8s  = 0x%02x\n","VSQ Pointer"                   , "" , get_field(data_out[0],0,7));
  get_field_long(debug_data,data_out,147,165,1);
  printf("  %-30s - 19 Bits %8s  = 0x%05x\n","OutLif"                        , "" , get_field(data_out[0],0,18));
  printf("\n");

}



void diag_pp(int unit, int core) {
  int i;
  int pkt_size_diff;

  jericho_parse_tm_cmd                               (unit,core);
  jericho_ingress_parse_ftmh                         (unit,core);
  jericho_egress_parse_ftmh                          (unit,core);
  pkt_size_diff = jericho_ingress_get_diff_pckt_size (unit,core);
  pkt_size_diff = jericho_egress_get_diff_pckt_size  (unit,core) + pkt_size_diff;
  
  if (pkt_size_diff!=0) {
	printf("Warning: Different Packet Size Editing in Ingress & Egress !\n");
	printf("         Outgoing Packet Size is Different from Incoming Packet Size by %d Bytes.\n\n",pkt_size_diff);
  }

}

void diag_packet(int unit, int core) {
  int i;
  int pkt_size_diff;

  jericho_parse_tm_cmd                               (unit,core);
  jericho_diag_buffer_decision                       (unit,core);
  jericho_diag_idr_pc                                (unit,core);
  jericho_diag_enqueue_cmd                           (unit,core);
}

print_functions();

diag_pp(0, 0);
diag_packet(0, 0);
