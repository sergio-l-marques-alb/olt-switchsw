// DNX - Measure Data Rates
// ========================
// Intended for QAX.

double gtimer_duration = 10; // ms
int iterations = 1; // burst iterations

int print_functions()
{	
	printf("\n");
	printf("CINT diag_rates_qax.c\n");
	printf("----------\n");
	printf("Functions:\n");
	printf("\t1. print_all_rates(unit); Default Print \n");
	printf("\t2. print_max_burst(unit);\n");
	printf("\n");
}


double get_max(double x, double y)
{
	if(x > y) 
	{
		return x;
	}
	else
	{
		return y;
	}
}

uint32  common_read_reg_long(int  unit_,  char  *reg_name,  uint32  *val,  int  nof_words){    
	reg_val  r_;    
	int  i;    
	if(diag_reg_get(unit_,  reg_name,  r_)  !=  BCM_E_NONE)
	  return(0);    
	  
	for(i  =  nof_words-1;  i  >=  0;  i--)
	  val[i]  =  r_[i];    
	  
	return(1);    
};  

uint32  get_field(uint32  in,  int  first,  int  last){
	if(last  !=  31)
	  in  &=  (0x7FFFFFFF  >>  30-last);
	
	in  =  in  >>  first;
	if ((last-first+1)<32)
	  in  &=  ~(0xFFFFFFFF  <<  (last-first+1));
	
	return(in);
};

double rate_by_reg_post_gtimer(uint32 unit,char* reg,uint32 byte_cnt,double clock_ratio){
  uint32  counter_data[2];
  uint32  counter_OVF;
  uint32  counter_result[2];
  double  rate;

  common_read_reg_long(unit,reg,&counter_data,2);
  counter_result[0]  = get_field(counter_data[0],0,30); 
  rate            = counter_result[0] / clock_ratio; // Bytes --> Bps
  if (byte_cnt){
    rate            = rate * 8;                     // bps   --> bps
    rate            = rate / 1000000000;            // bps   --> gbps
  }
  else{
    rate            = rate / 1000000;            //pps --> mpps
  }
 return rate;
}

double rate_by_field_post_gtimer(uint32 unit,char* reg, char* field,uint32 byte_cnt,double clock_ratio){
  uint32  counter_data[2];
  uint32  counter_OVF;
  uint32  counter_result[2];
  double  rate;

  reg_val v;
  diag_reg_field_get(unit, reg, field, v);
  counter_result[0]  = get_field(v[0],0,30); 

  rate            = counter_result[0] / clock_ratio; // Bytes --> Bps
  if (byte_cnt){
    rate            = rate * 8;                     // bps   --> bps
    rate            = rate / 1000000000;            // bps   --> gbps
  }
  else{
    rate            = rate / 1000000;            //pps --> mpps
  }
 return rate;
}

void qax_nbi_rate (uint32 unit) {
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  uint32    counter_data[2];
  uint32    counter_OVF;
  uint32    counter_result[2];
  double    rate;
  double    rx_rate_0,rx_rate_1,tx_rate_0,tx_rate_1,rx_pkt_rate_0,rx_pkt_rate_1,tx_pkt_rate_0,tx_pkt_rate_1;

  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 6,000,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  sprintf(cmd, "m NBIH_GTIMER_CONFIGURATION GTIMER_RESET_ON_TRIGGER=1 GTIMER_ENABLE=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  
  int i;
  for(i = 0; i < iterations; i++)
  {
	  bshell(unit,"m NBIH_GTIMER_CONFIGURATION GTIMER_TRIGGER=1");
	  sal_usleep(gtimer_duration * 3 * 1000); // Wait for GTimer

	  clock_ratio = nof_clocks / (core_freq * 1000);

	  rx_rate_0     = get_max(rx_rate_0, rate_by_reg_post_gtimer(unit,"NBIH_RX_TOTAL_BYTE_COUNTER"            , 1, clock_ratio));
	  tx_rate_0     = get_max(tx_rate_0, rate_by_reg_post_gtimer(unit,"NBIH_TX_TOTAL_BYTE_COUNTER"            , 1, clock_ratio));
	  rx_pkt_rate_0 = get_max(rx_pkt_rate_0, rate_by_reg_post_gtimer(unit,"NBIH_RX_TOTAL_PKT_COUNTER"             , 0, clock_ratio));
	  tx_pkt_rate_0 = get_max(tx_pkt_rate_0, rate_by_reg_post_gtimer(unit,"NBIH_TX_TOTAL_PKT_COUNTER"             , 0, clock_ratio));
  }
  printf("\t+----------------------------------------------------------+\n");
  printf("\t|                     NBI Rates                            |\n");
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| Counter             |               Total                |\n");
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| Rx Rate             | %8.2f Gbps      %8.2f Mpps   |\n",rx_rate_0,rx_pkt_rate_0);
  printf("\t| Tx Rate             | %8.2f Gbps      %8.2f Mpps   |\n",tx_rate_0,tx_pkt_rate_0);
  printf("\t+---------------------+------------------------------------+\n");

  bshell(unit,"m NBIH_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
}

void qax_ire_rate (uint32 unit) {
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  uint32    counter_data[2];
  uint32    counter_OVF;
  uint32    counter_result[2];
  double    rate;

  double cpu_pkt_rate, nif_pkt_rate, oamp_pkt_rate, olp_pkt_rate, rcy_pkt_rate;
  
  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 6,000,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  sprintf(cmd, "m IRE_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  
  int i;
  for(i = 0; i < iterations; i++)
  {
  
	  bshell(unit,"m IRE_GTIMER_CONFIGURATION GTIMER_TRIGGER=1");

	  sal_usleep(gtimer_duration * 3 * 1000); // Wait for GTimer

	  clock_ratio = nof_clocks / (core_freq * 1000);
	  
	  cpu_pkt_rate   	= get_max(cpu_pkt_rate ,rate_by_reg_post_gtimer(unit,"IRE_CPU_PACKET_COUNTER"            , 0, clock_ratio));
	  nif_pkt_rate   	= get_max(nif_pkt_rate ,rate_by_reg_post_gtimer(unit,"IRE_NIF_PACKET_COUNTER"            , 0, clock_ratio));  
	  oamp_pkt_rate   	= get_max(oamp_pkt_rate ,rate_by_reg_post_gtimer(unit,"IRE_OAMP_PACKET_COUNTER"            , 0, clock_ratio));
	  olp_pkt_rate   	= get_max(olp_pkt_rate ,rate_by_reg_post_gtimer(unit,"IRE_OLP_PACKET_COUNTER"            , 0, clock_ratio));
	  rcy_pkt_rate   	= get_max(rcy_pkt_rate ,rate_by_reg_post_gtimer(unit,"IRE_RCY_PACKET_COUNTER"            , 0, clock_ratio));
  }
  
  printf("\t+----------------------------------------------------------+\n");
  printf("\t|                     IRE Rates                            |\n");
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| Counter             |               Total                |\n");
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| CPU       Rate      |                    %8.2f Mpps   |\n",cpu_pkt_rate);
  printf("\t| NIF       Rate      |                    %8.2f Mpps   |\n",nif_pkt_rate);
  printf("\t| OAMP      Rate      |                    %8.2f Mpps   |\n",oamp_pkt_rate);
  printf("\t| OLP       Rate      |                    %8.2f Mpps   |\n",olp_pkt_rate);
  printf("\t| RCY       Rate      |                    %8.2f Mpps   |\n",rcy_pkt_rate);
  printf("\t+---------------------+------------------------------------+\n");

  bshell(unit,"m IRE_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
}

void qax_cgm_rate (uint32 unit) {
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  double    rate;
  double    sram_enq_pkt_rate, sram_deq_pkt_rate, sram_del_pkt_rate, sram_del_rjct_pkt_rate, dram_enq_pkt_rate, dram_deq_pkt_rate, dram_del_pkt_rate;
  double    sram_enq_rate, sram_deq_rate, sram_del_rate, sram_del_rjct_rate, dram_enq_rate, dram_deq_rate, dram_del_rate;
	
  double    sram_reject_pkt;
  double    sram_to_dram_pkt, sram_to_dram_rate;
  double    sram_to_fabric_pkt, sram_to_fabric_rate;
  
  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 6,000,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  sprintf(cmd, "m CGM_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  
  sprintf(cmd, "m SPB_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  
  int i;
  for(i = 0; i < iterations; i++)
  {
	  bshell(unit,"m CGM_GTIMER_CONFIGURATION GTIMER_TRIGGER=1");
	  bshell(unit,"m SPB_GTIMER_CONFIGURATION GTIMER_TRIGGER=1");
	  sal_usleep(gtimer_duration * 3 * 1000); // Wait for GTimer
	  clock_ratio = nof_clocks / (core_freq * 1000);

	  sram_enq_pkt_rate        = get_max(sram_enq_pkt_rate      ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_SRAM_ENQ_PKT_CTR"          , 0, clock_ratio));
	  sram_deq_pkt_rate        = get_max(sram_deq_pkt_rate      ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_SRAM_DEQ_PKT_CTR"          , 0, clock_ratio));
	  sram_del_pkt_rate        = get_max(sram_del_pkt_rate      ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_SRAM_DEL_PKT_CTR"          , 0, clock_ratio));
	  sram_del_rjct_pkt_rate   = get_max(sram_del_rjct_pkt_rate ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_SRAM_ENQ_RJCT_PKT_CTR"     , 0, clock_ratio));
	  dram_enq_pkt_rate        = get_max(dram_enq_pkt_rate      ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_DRAM_ENQ_BUNDLE_CTR"       , 0, clock_ratio));
	  dram_deq_pkt_rate        = get_max(dram_deq_pkt_rate      ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_DRAM_DEQ_PKT_CTR"          , 0, clock_ratio));
	  dram_del_pkt_rate        = get_max(dram_del_pkt_rate      ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_DRAM_DEL_PKT_CTR"          , 0, clock_ratio));
	  
	  
	  sram_enq_rate        = get_max(sram_enq_rate     ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_SRAM_ENQ_BYTE_CTR"          , 1, clock_ratio));
	  sram_deq_rate        = get_max(sram_deq_rate     ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_SRAM_DEQ_BYTE_CTR"          , 1, clock_ratio));
	  sram_del_rate        = get_max(sram_del_rate     ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_SRAM_DEL_BYTE_CTR"          , 1, clock_ratio));
	  sram_del_rjct_rate   = get_max(sram_del_rjct_rate,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_SRAM_ENQ_RJCT_BYTE_CTR"     , 1, clock_ratio));
	  dram_enq_rate        = get_max(dram_enq_rate     ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_DRAM_ENQ_BUNDLE_BYTE_CTR"   , 1, clock_ratio));
	  dram_deq_rate        = get_max(dram_deq_rate     ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_DRAM_DEQ_BYTE_CTR"          , 1, clock_ratio));
	  dram_del_rate        = get_max(dram_del_rate     ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_DRAM_DEL_BYTE_CTR"          , 1, clock_ratio));
	  
	  sram_reject_pkt      = get_max(sram_reject_pkt     ,rate_by_reg_post_gtimer   (unit,"SPB_ALMOST_FULL_SRAM_REJECT_COUNTER" , 0, clock_ratio));
	  
	  sram_to_dram_pkt      = get_max(sram_to_dram_pkt     ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_SRAM_DEQ_TO_DRAM_PKT_CTR" , 0, clock_ratio));
	  sram_to_dram_rate      = get_max(sram_to_dram_rate     ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_SRAM_DEQ_TO_DRAM_BYTE_CTR" , 1, clock_ratio));	  
	  
	  sram_to_fabric_pkt      = get_max( sram_to_fabric_pkt     ,rate_by_reg_post_gtimer    (unit,"CGM_VOQ_SRAM_DEQ_TO_FABRIC_PKT_CTR" , 0, clock_ratio));
	  sram_to_fabric_rate      = get_max(sram_to_fabric_rate     ,rate_by_reg_post_gtimer   (unit,"CGM_VOQ_SRAM_DEQ_TO_FABRIC_BYTE_CTR" , 1, clock_ratio));	  
	  
  }
  printf("\t+----------------------------------------------------------+\n");
  printf("\t|                     CGM Rates                            |\n");
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| Counter             |               Total                |\n");
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| SRAM Enqueue        | %8.2f Gbps      %8.2f Mpps   |\n",sram_enq_rate,sram_enq_pkt_rate);
  printf("\t| SRAM Dequeue        | %8.2f Gbps      %8.2f Mpps   |\n",sram_deq_rate,sram_deq_pkt_rate);
  printf("\t| SRAM Delete         | %8.2f Gbps      %8.2f Mpps   |\n",sram_del_rate,sram_del_pkt_rate);
  printf("\t| SRAM Reject         | %8.2f Gbps      %8.2f Mpps   |\n",sram_del_rjct_rate,sram_del_rjct_pkt_rate);
  printf("\t| SRAM Full Reject    |                    %8.2f Mpps   |\n",sram_reject_pkt);
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| SRAM to FABRIC      | %8.2f Gbps      %8.2f Mpps   |\n",sram_to_fabric_rate,sram_to_fabric_pkt);
  printf("\t| SRAM to DRAM        | %8.2f Gbps      %8.2f Mpps   |\n",sram_to_dram_rate,sram_to_dram_pkt);
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| DRAM Enqueue        | %8.2f Gbps      %8.2f Mpps   |\n",dram_enq_rate,dram_enq_pkt_rate);
  printf("\t| DRAM Dequeue        | %8.2f Gbps      %8.2f Mpps   |\n",dram_deq_rate,dram_deq_pkt_rate);
  printf("\t| DRAM Delete         | %8.2f Gbps      %8.2f Mpps   |\n",dram_del_rate,dram_del_pkt_rate);
  printf("\t+---------------------+------------------------------------+\n");
  
  bshell(unit,"m CGM_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
  bshell(unit,"m SPB_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
}

void qax_txq_rate (uint32 unit) {
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  double    rate;
  double    egq_sram, egq_dram, fdt_sram, fdt_dram;

  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 6,000,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  sprintf(cmd, "m TXQ_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  
  int i;
  for(i = 0; i < iterations; i++)
  {
	  bshell(unit,"m TXQ_GTIMER_CONFIGURATION GTIMER_TRIGGER=1");

	  sal_usleep(gtimer_duration * 3 * 1000); // Wait for GTimer

	  clock_ratio = nof_clocks / (core_freq * 1000);

	  egq_sram        = get_max(egq_sram ,rate_by_field_post_gtimer   (unit,"TXQ_EGQ_COUNTER", "LOC_SRAM_PACKET_CNT" , 0, clock_ratio));
	  egq_dram        = get_max(egq_dram ,rate_by_field_post_gtimer   (unit,"TXQ_EGQ_COUNTER", "LOC_DRAM_PACKET_CNT" , 0, clock_ratio));
	  
	  bshell(unit, "m TXQ_FDT_PRG_COUNTER_CFG(0) PRG_N_CNT_CFG_SRAM_VAL=1 PRG_N_CNT_CFG_SRAM_MASK=0");
	  bshell(unit, "m TXQ_FDT_PRG_COUNTER_CFG(1) PRG_N_CNT_CFG_SRAM_VAL=0 PRG_N_CNT_CFG_SRAM_MASK=0");
	  
	  fdt_sram        = get_max(fdt_sram, rate_by_field_post_gtimer   (unit,"TXQ_FDT_COUNTER", "PRG_0_PACKET_CNT" , 0, clock_ratio));
	  fdt_dram        = get_max(fdt_dram, rate_by_field_post_gtimer   (unit,"TXQ_FDT_COUNTER", "PRG_1_PACKET_CNT" , 0, clock_ratio));
  }
  
  printf("\t+----------------------------------------------------------+\n");
  printf("\t|                     TXQ Rates                            |\n");
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| Counter             |               Total                |\n");
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| EGQ SRAM            |                    %8.2f Mpps   |\n",egq_sram);
  printf("\t| EGQ DRAM            |                    %8.2f Mpps   |\n",egq_dram);
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| FDT SRAM            |                    %8.2f Mpps   |\n",fdt_sram);
  printf("\t| FDT DRAM            |                    %8.2f Mpps   |\n",fdt_dram);
  printf("\t+---------------------+------------------------------------+\n");

  bshell(unit,"m TXQ_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
}

void qax_egq_rate (int unit) {
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  uint32    counter_data[2];
  uint32    counter_MSB;
  uint32    counter_LSB;
  uint32    counter_OVF;
  uint32    counter_result;
  double    rate;
  double   EHP_UC_Rate_0,EHP_DIS_Rate_0,EHP_MC_H_Rate_0,EHP_MC_L_Rate_0,PQP_UC_Rate_0,PQP_DIS_UC_Rate_0,PQP_MC_Rate_0,PQP_DIS_MC_Rate_0,FQP_Rate_0;           
  double	EHP_DIS_MCL_Rate_0, EHP_DIS_MCH_Rate_0;
  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 6,000,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  // Enable gtimer for EGQ for clk of 600MHz:

  sprintf(cmd,"s EGQ_GTIMER_CONFIG TIMER_CONFIG=%d",nof_clocks_int);
  bshell(unit, cmd);
  sprintf(cmd,"m EGQ_CHECK_BW_TO_IFC CHECK_BW_TO_IFC=0 CHECK_BW_TO_IFC_EN=0");
  bshell(unit, cmd);

  int i;
  for(i = 0; i < iterations; i++)
  {
  
	  bshell(unit,"s EGQ_GTIMER_CONFIG_CONT CLEAR_GTIMER=1 ACTIVATE_GTIMER=0");
	  bshell(unit,"s EGQ_GTIMER_CONFIG_CONT CLEAR_GTIMER=1 ACTIVATE_GTIMER=1");

	  sal_usleep(gtimer_duration * 3 * 1000); // Wait for GTimer

	  clock_ratio = nof_clocks / (core_freq * 1000);

	  EHP_MC_H_Rate_0   = get_max(EHP_MC_H_Rate_0    ,rate_by_reg_post_gtimer(unit,"EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER"            , 0, clock_ratio));
	  EHP_MC_L_Rate_0   = get_max(EHP_MC_L_Rate_0    ,rate_by_reg_post_gtimer(unit,"EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER"             , 0, clock_ratio));
	  EHP_UC_Rate_0     = get_max(EHP_UC_Rate_0      ,rate_by_reg_post_gtimer(unit,"EGQ_EHP_UNICAST_PACKET_COUNTER"                   , 0, clock_ratio));
	  EHP_DIS_Rate_0    = get_max(EHP_DIS_Rate_0     ,rate_by_reg_post_gtimer(unit,"EGQ_EHP_DISCARD_PACKET_COUNTER"                   , 0, clock_ratio));
	  EHP_DIS_MCL_Rate_0    = get_max(EHP_DIS_Rate_0     ,rate_by_reg_post_gtimer(unit,"EGQ_EHP_MULTICAST_LOW_DISCARDS_COUNTER"                   , 0, clock_ratio));
	  EHP_DIS_MCH_Rate_0    = get_max(EHP_DIS_Rate_0     ,rate_by_reg_post_gtimer(unit,"EGQ_EHP_MULTICAST_HIGH_DISCARDS_COUNTER"                   , 0, clock_ratio));
	  PQP_UC_Rate_0     = get_max(PQP_UC_Rate_0      ,rate_by_reg_post_gtimer(unit,"EGQ_PQP_UNICAST_PACKET_COUNTER"                   , 0, clock_ratio));
	  PQP_DIS_UC_Rate_0 = get_max(PQP_DIS_UC_Rate_0  ,rate_by_reg_post_gtimer(unit,"EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER"           , 0, clock_ratio));
	  PQP_MC_Rate_0     = get_max(PQP_MC_Rate_0      ,rate_by_reg_post_gtimer(unit,"EGQ_PQP_MULTICAST_PACKET_COUNTER"                 , 0, clock_ratio));
	  PQP_DIS_MC_Rate_0 = get_max(PQP_DIS_MC_Rate_0  ,rate_by_reg_post_gtimer(unit,"EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER"         , 0, clock_ratio));
	  FQP_Rate_0 		= get_max(FQP_Rate_0 		 ,rate_by_reg_post_gtimer(unit,"EGQ_FQP_PACKET_COUNTER"                           , 0, clock_ratio));

  }

  printf("\t+----------------------------------------------------------+\n");
  printf("\t|                     EGQ Rates                            |\n");
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| Counter             |               Total                |\n");
  printf("\t+---------------------+------------------------------------+\n");
  printf("\t| EHP UC         Rate |                    %8.2f Mpps   |\n",EHP_UC_Rate_0);
  printf("\t| EHP MC High    Rate |                    %8.2f Mpps   |\n",EHP_MC_H_Rate_0);
  printf("\t| EHP MC Low     Rate |                    %8.2f Mpps   |\n",EHP_MC_L_Rate_0);
  printf("\t| EHP Discard    Rate |                    %8.2f Mpps   |\n",EHP_DIS_Rate_0);
  printf("\t| EHP MCH Discrd Rate |                    %8.2f Mpps   |\n",EHP_DIS_MCH_Rate_0);
  printf("\t| EHP MCL Discrd Rate |                    %8.2f Mpps   |\n",EHP_DIS_MCL_Rate_0);
  printf("\t| PQP UC         Rate |                    %8.2f Mpps   |\n",PQP_UC_Rate_0);
  printf("\t| PQP Discard UC Rate |                    %8.2f Mpps   |\n",PQP_DIS_UC_Rate_0);
  printf("\t| PQP MC         Rate |                    %8.2f Mpps   |\n",PQP_MC_Rate_0);
  printf("\t| PQP Discard MC Rate |                    %8.2f Mpps   |\n",PQP_DIS_MC_Rate_0);
  printf("\t| FQP            Rate |                    %8.2f Mpps   |\n",FQP_Rate_0);
  printf("\t+---------------------+------------------------------------+\n");

  bshell(unit,"s EGQ_GTIMER_CONFIG_CONT CLEAR_GTIMER=0x0,ACTIVATE_GTIMER=0x0,");
}

// Print the traffic rate in each block
void print_all_rates(int unit) {

	printf("GTimer duration is %2.2fms\n", gtimer_duration);
	
	qax_nbi_rate(unit);
	qax_ire_rate(unit);
	qax_cgm_rate(unit);
	qax_txq_rate(unit);
	qax_egq_rate(unit);
}

// Print the max burst in each block
void print_max_burst(int unit, double gtimer, int iters) {

	gtimer_duration = gtimer;
	iterations = iters;
	
	printf("GTimer duration is %2.2fms\n", gtimer_duration);
	printf("Burst iterations = %d\n", iterations);
	
	qax_nbi_rate(unit);
	qax_ire_rate(unit);
	qax_cgm_rate(unit);
	qax_txq_rate(unit);
	qax_egq_rate(unit);
	
	// Restore
	gtimer_duration = 10;
	iterations = 1;
}

print_functions();
print_all_rates(0);
//print_all_bursts(0, 0.01, 50);
