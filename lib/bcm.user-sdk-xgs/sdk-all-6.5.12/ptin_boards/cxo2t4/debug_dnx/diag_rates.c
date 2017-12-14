// DNX - Measure Data Rates
// ========================
// Intended for Jericho and QMX use.

double gtimer_duration = 10; // ms

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

double rate_by_field_post_gtimer(uint32 unit,char* field,uint32 byte_cnt,double clock_ratio){
  uint32  counter_data[2];
  uint32  counter_OVF;
  uint32  counter_result[2];
  double  rate;

  common_read_reg_long(unit,field,&counter_data,2);
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

void jericho_nbi_rate (uint32 unit) {
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
  nof_clocks     = core_freq * gtimer_duration; // 7,200,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  sprintf(cmd, "m NBIH_GTIMER_CONFIGURATION GTIMER_RESET_ON_TRIGGER=1 GTIMER_ENABLE=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  bshell(unit,"s NBIH_GTIMER_TRIGGER       GTIMER_TRIGGER=1");
  sal_usleep(gtimer_duration * 10 * 1000); // Wait 0.1 sec

  clock_ratio = nof_clocks / (core_freq * 1000);

  rx_rate_0     = rate_by_field_post_gtimer(unit,"NBIH_RX_CORE_0_BYTE_COUNTER"            , 1, clock_ratio);
  rx_rate_1     = rate_by_field_post_gtimer(unit,"NBIH_RX_CORE_1_BYTE_COUNTER"            , 1, clock_ratio);
  tx_rate_0     = rate_by_field_post_gtimer(unit,"NBIH_TX_CORE_0_BYTE_COUNTER"            , 1, clock_ratio);
  tx_rate_1     = rate_by_field_post_gtimer(unit,"NBIH_TX_CORE_1_BYTE_COUNTER"            , 1, clock_ratio);
  rx_pkt_rate_0 = rate_by_field_post_gtimer(unit,"NBIH_RX_CORE_0_PKT_COUNTER"             , 0, clock_ratio);
  rx_pkt_rate_1 = rate_by_field_post_gtimer(unit,"NBIH_RX_CORE_1_PKT_COUNTER"             , 0, clock_ratio);
  tx_pkt_rate_0 = rate_by_field_post_gtimer(unit,"NBIH_TX_CORE_0_PKT_COUNTER"             , 0, clock_ratio);
  tx_pkt_rate_1 = rate_by_field_post_gtimer(unit,"NBIH_TX_CORE_1_PKT_COUNTER"             , 0, clock_ratio);

  printf("\t+------------------------------------------------------------------------------------------------------------------------------------+\n");
  printf("\t|                                                              NBI Rates                                                             |\n");
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");
  printf("\t| Counter             |               Core 0               |               Core 1               |               Total                |\n");
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");
  printf("\t| Rx Rate             | %8.2f Gbps      %8.2f Mpps   | %8.2f Gbps      %8.2f Mpps   | %8.2f Gbps      %8.2f Mpps   |\n",rx_rate_0,rx_pkt_rate_0,rx_rate_1,rx_pkt_rate_1,rx_rate_0+rx_rate_1,rx_pkt_rate_0+rx_pkt_rate_1);
  printf("\t| Tx Rate             | %8.2f Gbps      %8.2f Mpps   | %8.2f Gbps      %8.2f Mpps   | %8.2f Gbps      %8.2f Mpps   |\n",tx_rate_0,tx_pkt_rate_0,tx_rate_1,tx_pkt_rate_1,tx_rate_0+tx_rate_1,tx_pkt_rate_0+tx_pkt_rate_1);
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");

  bshell(unit,"m NBIH_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
}

void jericho_ire_rate (uint32 unit) {
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  uint32    counter_data[2];
  uint32    counter_OVF;
  uint32    counter_result[2];
  double    rate;

  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 7,200,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  sprintf(cmd, "s IRE_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  
  bshell(unit,"s IRE_GTIMER_TRIGGER GTIMER_TRIGGER=1");

  sal_usleep(gtimer_duration * 10 * 1000); // Wait 0.1 sec

  clock_ratio = nof_clocks / (core_freq * 1000);
    
  double cpu_pkt_rate   	= rate_by_field_post_gtimer(unit,"IRE_CPU_PACKET_COUNTER"            , 0, clock_ratio);
  double nif_pkt_rate_0   	= rate_by_field_post_gtimer(unit,"IRE_NIF_PACKET_COUNTER(0)"            , 0, clock_ratio);
  double nif_pkt_rate_1		= rate_by_field_post_gtimer(unit,"IRE_NIF_PACKET_COUNTER(1)"            , 0, clock_ratio);
							  
  double oamp_pkt_rate   	= rate_by_field_post_gtimer(unit,"IRE_OAMP_PACKET_COUNTER"            , 0, clock_ratio);
  double olp_pkt_rate   	= rate_by_field_post_gtimer(unit,"IRE_OLP_PACKET_COUNTER"            , 0, clock_ratio);
  double rcy_pkt_rate_0   	= rate_by_field_post_gtimer(unit,"IRE_RCY_PACKET_COUNTER(0)"            , 0, clock_ratio);
  double rcy_pkt_rate_1		= rate_by_field_post_gtimer(unit,"IRE_RCY_PACKET_COUNTER(1)"            , 0, clock_ratio);
  double rcy_pkt_rate_10    = rate_by_field_post_gtimer(unit,"IRE_RCY_PACKET_COUNTER(2)"            , 0, clock_ratio);
  double rcy_pkt_rate_11    = rate_by_field_post_gtimer(unit,"IRE_RCY_PACKET_COUNTER(3)"            , 0, clock_ratio);

  printf("\t+------------------------------------------------------------------------------------------------------------------------------------+\n");
  printf("\t|                                                              IRE Rates                                                             |\n");
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");
  printf("\t| Counter             |               Core 0               |               Core 1               |               Total                |\n");
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");
  printf("\t| NIF       Rate      |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",nif_pkt_rate_0, nif_pkt_rate_1, nif_pkt_rate_0 + nif_pkt_rate_1);
  printf("\t| RCY #1    Rate      |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",rcy_pkt_rate_0,  rcy_pkt_rate_1, rcy_pkt_rate_0 + rcy_pkt_rate_1);
  printf("\t| RCY #2    Rate      |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",rcy_pkt_rate_10, rcy_pkt_rate_11, rcy_pkt_rate_10 + rcy_pkt_rate_11);
  printf("\t| CPU       Rate      |                                    |                                    |                    %8.2f Mpps   |\n",cpu_pkt_rate);
  printf("\t| OAMP      Rate      |                                    |                                    |                    %8.2f Mpps   |\n",oamp_pkt_rate);
  printf("\t| OLP       Rate      |                                    |                                    |                    %8.2f Mpps   |\n",olp_pkt_rate);
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");

  bshell(unit,"m IRE_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
}

void jericho_mmu_rate (uint32 unit) {
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  uint32    counter_data[2];
  uint32    counter_OVF;
  uint32    counter_result[2];
  double    rate;
  double    dram_pkt_rate, ocb_total_read_rate, ocb_total_write_rate;

  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 7,200,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  sprintf(cmd, "s OCB_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  sprintf(cmd, "s MMU_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  
  bshell(unit,"s OCB_GTIMER_TRIGGER GTIMER_TRIGGER=1");
  bshell(unit,"s MMU_GTIMER_TRIGGER GTIMER_TRIGGER=1");

  sal_usleep(gtimer_duration * 10 * 1000); // Wait 0.1 sec

  clock_ratio = nof_clocks / (core_freq * 1000);

  dram_pkt_rate   	= rate_by_field_post_gtimer   	(unit,"MMU_IDR_PACKET_COUNTER"            , 0, clock_ratio);
    
  ocb_total_write_rate   	= rate_by_field_post_gtimer   	(unit,"OCB_OCB_WR_CTR"            , 0, clock_ratio);
  ocb_total_read_rate   	= rate_by_field_post_gtimer   	(unit,"OCB_OCB_RD_CTR"            , 0, clock_ratio);

  printf("\t+-----------------------------------------------------------+\n");
  printf("\t|                         MMU Rates                         |\n");
  printf("\t+---------------------+-------------------------------------+\n");
  printf("\t| Counter             |               Total                 |\n");
  printf("\t+---------------------+-------------------------------------+\n");
  printf("\t| DRAM      Rate      |                    %8.2f Mpps    |\n",dram_pkt_rate);
  printf("\t| OCB Write Rate      |                    %8.2f M.Reads |\n",ocb_total_write_rate);
  printf("\t| OCB Read  Rate      |                    %8.2f M.Writes|\n",ocb_total_read_rate);
  printf("\t+---------------------+-------------------------------------+\n");

  bshell(unit,"m OCB_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
  bshell(unit,"m MMU_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
}

void jericho_iqm_rate (uint32 unit) {
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  uint32    counter_data[2];
  uint32    counter_OVF;
  uint32    counter_result[2];
  double    rate;
  double    enq_byte_rate_0,enq_byte_rate_1,deq_byte_rate_0,deq_byte_rate_1,enq_pkt_rate_0,enq_pkt_rate_1,deq_pkt_rate_0,deq_pkt_rate_1,enq_dis_pkt_rate_0,enq_dis_pkt_rate_1,del_pkt_rate_0,del_pkt_rate_1;

  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 7,200,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  sprintf(cmd, "s IQM_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  bshell(unit,"s IQM_GTIMER_TRIGGER GTIMER_TRIGGER=1");

  sal_usleep(gtimer_duration * 10 * 1000); // Wait 0.1 sec

  clock_ratio = nof_clocks / (core_freq * 1000);

  enq_byte_rate_0    = rate_by_field_post_gtimer   (unit,"IQM_ENQUEUE_BYTE_COUNTER.IQM0"            , 1, clock_ratio);
  enq_byte_rate_1    = rate_by_field_post_gtimer   (unit,"IQM_ENQUEUE_BYTE_COUNTER.IQM1"            , 1, clock_ratio);
  deq_byte_rate_0    = rate_by_field_post_gtimer   (unit,"IQM_DEQUEUE_BYTE_COUNTER.IQM0"            , 1, clock_ratio);
  deq_byte_rate_1    = rate_by_field_post_gtimer   (unit,"IQM_DEQUEUE_BYTE_COUNTER.IQM1"            , 1, clock_ratio);
  enq_pkt_rate_0     = rate_by_field_post_gtimer   (unit,"IQM_ENQUEUE_PACKET_COUNTER.IQM0"          , 0, clock_ratio);
  enq_pkt_rate_1     = rate_by_field_post_gtimer   (unit,"IQM_ENQUEUE_PACKET_COUNTER.IQM1"          , 0, clock_ratio);
  deq_pkt_rate_0     = rate_by_field_post_gtimer   (unit,"IQM_DEQUEUE_PACKET_COUNTER.IQM0"          , 0, clock_ratio);
  deq_pkt_rate_1     = rate_by_field_post_gtimer   (unit,"IQM_DEQUEUE_PACKET_COUNTER.IQM1"          , 0, clock_ratio);
  enq_dis_pkt_rate_0 = rate_by_field_post_gtimer   (unit,"IQM_ENQUEUE_DISCARDED_PACKET_COUNTER.IQM0", 0, clock_ratio);
  enq_dis_pkt_rate_1 = rate_by_field_post_gtimer   (unit,"IQM_ENQUEUE_DISCARDED_PACKET_COUNTER.IQM1", 0, clock_ratio);
  del_pkt_rate_0     = rate_by_field_post_gtimer   (unit,"IQM_QUEUE_DELETED_PACKET_COUNTER.IQM0"    , 0, clock_ratio);
  del_pkt_rate_1     = rate_by_field_post_gtimer   (unit,"IQM_QUEUE_DELETED_PACKET_COUNTER.IQM1"    , 0, clock_ratio);

  printf("\t+------------------------------------------------------------------------------------------------------------------------------------+\n");
  printf("\t|                                                              IQM Rates                                                             |\n");
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");
  printf("\t| Counter             |               Core 0               |               Core 1               |               Total                |\n");
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");
  printf("\t| EnQueue   Rate      | %8.2f Gbps      %8.2f Mpps   | %8.2f Gbps      %8.2f Mpps   | %8.2f Gbps      %8.2f Mpps   |\n",enq_byte_rate_0,enq_pkt_rate_0,enq_byte_rate_1,enq_pkt_rate_1,enq_byte_rate_0+enq_byte_rate_1,enq_pkt_rate_0+enq_pkt_rate_1);
  printf("\t| DeQueue   Rate      | %8.2f Gbps      %8.2f Mpps   | %8.2f Gbps      %8.2f Mpps   | %8.2f Gbps      %8.2f Mpps   |\n",deq_byte_rate_0,deq_pkt_rate_0,deq_byte_rate_1,deq_pkt_rate_1,deq_byte_rate_0+deq_byte_rate_1,deq_pkt_rate_0+deq_pkt_rate_1);
  printf("\t| Discard   Rate      |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",enq_dis_pkt_rate_0,enq_dis_pkt_rate_1,enq_dis_pkt_rate_0+enq_dis_pkt_rate_1);
  printf("\t| DeQDelete Rate      |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",del_pkt_rate_0,del_pkt_rate_1,del_pkt_rate_0+del_pkt_rate_1);
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");

  bshell(unit,"m IQM_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
}

void jericho_ipt_rate (uint32 unit) {
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  uint32    counter_data[2];
  uint32    counter_OVF;
  uint32    counter_result[2];
  double    rate;
  double    enq_pkt_rate_0, enq_pkt_rate_1, egq_pkt_rate_0, egq_pkt_rate_1, fdt_pkt_rate_0, fdt_pkt_rate_1;

  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 7,200,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  sprintf(cmd, "s IPT_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  bshell(unit,"s IPT_GTIMER_TRIGGER GTIMER_TRIGGER=1");

  sal_usleep(gtimer_duration * 10 * 1000); // Wait 0.1 sec

  clock_ratio = nof_clocks / (core_freq * 1000);

  enq_pkt_rate_0   	= rate_by_field_post_gtimer   	(unit,"IPT_ENQ_0_PKT_CNT"            , 0, clock_ratio);
  enq_pkt_rate_1   	= rate_by_field_post_gtimer   	(unit,"IPT_ENQ_1_PKT_CNT"            , 0, clock_ratio);
  
  // EGQ Counter is not accurate
  egq_pkt_rate_0   	= rate_by_field_post_gtimer   	(unit,"IPT_EGQ_0_PKT_CNT"            , 0, clock_ratio);
  egq_pkt_rate_1   	= rate_by_field_post_gtimer   	(unit,"IPT_EGQ_1_PKT_CNT"            , 0, clock_ratio);
  
  fdt_pkt_rate_0   	= rate_by_field_post_gtimer   	(unit,"IPT_FDT_0_PKT_CNT"            , 0, clock_ratio);
  fdt_pkt_rate_1   	= rate_by_field_post_gtimer   	(unit,"IPT_FDT_1_PKT_CNT"            , 0, clock_ratio);

  printf("\t+------------------------------------------------------------------------------------------------------------------------------------+\n");
  printf("\t|                                                            IPT Rates                                                               |\n");
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");
  printf("\t| Counter             |               Core 0               |               Core 1               |               Total                |\n");
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");
  printf("\t| Incoming  Rate      |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",enq_pkt_rate_0, enq_pkt_rate_1, enq_pkt_rate_0 + enq_pkt_rate_1);
  printf("\t| EGQ (inaccurate)    |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",egq_pkt_rate_0, egq_pkt_rate_1, egq_pkt_rate_0 + egq_pkt_rate_1);
  printf("\t| FDT       Rate      |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",fdt_pkt_rate_0, fdt_pkt_rate_1, fdt_pkt_rate_0 + fdt_pkt_rate_1);
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");

  bshell(unit,"m IPT_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
}

void jericho_fdt_fdr_rate (uint32 unit) {
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  uint32    counter_data[2];
  uint32    counter_OVF;
  uint32    counter_result[2];
  double    rate;
  double    fdt_ipt_desc_cell_rate, fdt_ire_desc_cell_rate, fdt_tx_data_cell_rate; 
  double    fdr_p1_cell_in_rate, fdr_p2_cell_in_rate, fdr_p3_cell_in_rate, fdr_total_cell_in_rate; 


  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 7,200,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  sprintf(cmd, "s FDT_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  bshell(unit,"s FDT_GTIMER_TRIGGER GTIMER_TRIGGER=1");
  sprintf(cmd, "s FDR_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  bshell(unit,"s FDR_GTIMER_TRIGGER GTIMER_TRIGGER=1");


  sal_usleep(gtimer_duration * 10 * 1000); // Wait 0.1 sec

  clock_ratio = nof_clocks / (core_freq * 1000);

  fdt_ipt_desc_cell_rate   	= rate_by_field_post_gtimer   	(unit, "FDT_IPT_DESC_CELL_COUNTER"            , 0, clock_ratio);
  fdt_ire_desc_cell_rate    = rate_by_field_post_gtimer     (unit, "FDT_IRE_DESC_CELL_COUNTER"            , 0, clock_ratio);
  fdt_tx_data_cell_rate     = rate_by_field_post_gtimer     (unit, "FDT_TRANSMITTED_DATA_CELLS_COUNTER"   , 0, clock_ratio);
  fdr_p1_cell_in_rate       = rate_by_field_post_gtimer     (unit, "FDR_P_1_CELL_IN_CNT", 0, clock_ratio);
  fdr_p2_cell_in_rate       = rate_by_field_post_gtimer     (unit, "FDR_P_2_CELL_IN_CNT", 0, clock_ratio);
  fdr_p3_cell_in_rate       = rate_by_field_post_gtimer     (unit, "FDR_P_3_CELL_IN_CNT", 0, clock_ratio);
  fdr_total_cell_in_rate    = rate_by_field_post_gtimer     (unit, "FDR_CELL_IN_CNT_TOTAL", 0, clock_ratio);

  printf("\t+-----------------------------------------------------------+            +-----------------------------------------------------------+                          \n");
  printf("\t|                        FDT Rates                          |            |                        FDR Rates                          |                          \n");
  printf("\t+---------------------+-------------------------------------+            +---------------------+-------------------------------------+                          \n");
  printf("\t| Counter             |               Total                 |            | Counter             |               Total                 |                          \n");
  printf("\t+---------------------+-------------------------------------+            +---------------------+-------------------------------------+                          \n");
  printf("\t| IPT Desc Cell Rate  |                     %8.2f Mcps   |            | P1  Cell In Rate    |                     %8.2f Mcps   |\n", fdt_ipt_desc_cell_rate, fdr_p1_cell_in_rate);
  printf("\t| IRE Desc Cell Rate  |                     %8.2f Mcps   |            | P2  Cell In Rate    |                     %8.2f Mcps   |\n", fdt_ire_desc_cell_rate, fdr_p2_cell_in_rate);
  printf("\t| Tx  Data Cell Rate  |                     %8.2f Mcps   |            | P3  Cell In Rate    |                     %8.2f Mcps   |\n", fdt_tx_data_cell_rate,  fdr_p3_cell_in_rate);
  printf("\t|                     |                                     |            | Total Cell In Rate  |                     %8.2f Mcps   |\n", fdr_total_cell_in_rate);         
  printf("\t+---------------------+-------------------------------------+            +---------------------+-------------------------------------+                          \n");

  bshell(unit,"m FDT_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
  bshell(unit,"m FDR_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
}

void jericho_fda_rate (uint32 unit) {
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  uint32    counter_data[2];
  uint32    counter_OVF;
  uint32    counter_result[2];
  double    rate;
  double    egq_cell_in_ipt_rate[2], egq_cell_in_meshmc_rate[2], egq_cell_in_tdm_rate[2];
  double    egq_cell_out_ipt_rate[2], egq_cell_out_meshmc_rate[2], egq_cell_out_tdm_rate[2];
  double    egq_cell_in_rate_p[3], egq_cell_out_rate_fab_pipe[2][3], egq_cell_out_rate_fab_fifo[2][3];  
  double    egq_cell_ovf_meshmc_drop_rate[2];


  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 7,200,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  sprintf(cmd, "s FDA_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  bshell(unit,"s FDA_GTIMER_TRIGGER GTIMER_TRIGGER=1");

  sal_usleep(gtimer_duration * 10 * 1000); // Wait 0.1 sec

  clock_ratio = nof_clocks / (core_freq * 1000);

  egq_cell_in_ipt_rate[0]         = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_IN_CNT_IPT(0)",      0, clock_ratio);
  egq_cell_in_ipt_rate[1]         = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_IN_CNT_IPT(1)",      0, clock_ratio);
  egq_cell_in_meshmc_rate[0]      = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_IN_CNT_MESHMC(0)",   0, clock_ratio);
  egq_cell_in_meshmc_rate[1]      = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_IN_CNT_MESHMC(1)",   0, clock_ratio);
  egq_cell_in_tdm_rate[0]         = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_IN_CNT_TDM(0)",      0, clock_ratio);
  egq_cell_in_tdm_rate[1]         = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_IN_CNT_TDM(1)",      0, clock_ratio);
  egq_cell_out_ipt_rate[0]        = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_IPT(0)",     0, clock_ratio);
  egq_cell_out_ipt_rate[1]        = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_IPT(1)",     0, clock_ratio);
  egq_cell_out_meshmc_rate[0]     = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_MESHMC(0)",  0, clock_ratio);
  egq_cell_out_meshmc_rate[1]     = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_MESHMC(1)",  0, clock_ratio);
  egq_cell_out_tdm_rate[0]        = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_TDM(0)",     0, clock_ratio);
  egq_cell_out_tdm_rate[1]        = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_TDM(1)",     0, clock_ratio);
  
  bcm_info_t info;
  bcm_info_get(unit, &info);
  if(info.device == 0x8680) // Jericho+
  {
	  egq_cell_in_rate_p[0]           = rate_by_field_post_gtimer(unit, "FDA_FAB_48_SCH_CELLS_IN_CNT_P_1",  0, clock_ratio);
	  egq_cell_in_rate_p[1]           = rate_by_field_post_gtimer(unit, "FDA_FAB_48_SCH_CELLS_IN_CNT_P_2",  0, clock_ratio);
	  egq_cell_in_rate_p[2]           = rate_by_field_post_gtimer(unit, "FDA_FAB_48_SCH_CELLS_IN_CNT_P_3",  0, clock_ratio);
  }
  else // Jericho
  {
	  egq_cell_in_rate_p[0]           = rate_by_field_post_gtimer(unit, "FDA_FAB_36_SCH_CELLS_IN_CNT_P_1",  0, clock_ratio);
	  egq_cell_in_rate_p[1]           = rate_by_field_post_gtimer(unit, "FDA_FAB_36_SCH_CELLS_IN_CNT_P_2",  0, clock_ratio);
	  egq_cell_in_rate_p[2]           = rate_by_field_post_gtimer(unit, "FDA_FAB_36_SCH_CELLS_IN_CNT_P_3",  0, clock_ratio);
  }
  
  
  egq_cell_out_rate_fab_pipe[0][0]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_FAB_PIPE_0(0)", 0, clock_ratio);
  egq_cell_out_rate_fab_pipe[0][1]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_FAB_PIPE_1(0)", 0, clock_ratio);
  egq_cell_out_rate_fab_pipe[0][2]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_FAB_PIPE_2(0)", 0, clock_ratio);
  egq_cell_out_rate_fab_pipe[1][0]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_FAB_PIPE_0(1)", 0, clock_ratio);
  egq_cell_out_rate_fab_pipe[1][1]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_FAB_PIPE_1(1)", 0, clock_ratio);
  egq_cell_out_rate_fab_pipe[1][2]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_FAB_PIPE_2(1)", 0, clock_ratio);
  egq_cell_out_rate_fab_fifo[0][0]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_FAB_FIFO_0(0)", 0, clock_ratio);
  egq_cell_out_rate_fab_fifo[0][1]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_FAB_FIFO_1(0)", 0, clock_ratio);
  egq_cell_out_rate_fab_fifo[0][2]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_FAB_FIFO_2(0)", 0, clock_ratio);  
  egq_cell_out_rate_fab_fifo[1][0]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_FAB_FIFO_0(1)", 0, clock_ratio);
  egq_cell_out_rate_fab_fifo[1][1]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_FAB_FIFO_1(1)", 0, clock_ratio);
  egq_cell_out_rate_fab_fifo[1][2]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_CELLS_OUT_CNT_FAB_FIFO_2(1)", 0, clock_ratio);
  egq_cell_ovf_meshmc_drop_rate[0]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_MESHMC_OVF_DROP_COUNT(0)", 0, clock_ratio);
  egq_cell_ovf_meshmc_drop_rate[1]   = rate_by_field_post_gtimer(unit, "FDA_EGQ_MESHMC_OVF_DROP_COUNT(1)", 0, clock_ratio);

  printf("\t+------------------------------------------------------------------------------------------------------------------------------------+\n");
  printf("\t|                                                            FDA Rates                                                               |\n");
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");
  printf("\t| Counter (Cells)     |               Core 0               |               Core 1               |               Total                |\n");
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");
  printf("\t| In  IPT Rate        |                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n",egq_cell_in_ipt_rate[0], egq_cell_in_ipt_rate[1], egq_cell_in_ipt_rate[0] + egq_cell_in_ipt_rate[1]);
  printf("\t| Out IPT Rate        |                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n",egq_cell_out_ipt_rate[0], egq_cell_out_ipt_rate[1], egq_cell_out_ipt_rate[0] + egq_cell_out_ipt_rate[1]);
  printf("\t| In  Meshmc Rate     |                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n",egq_cell_in_meshmc_rate[0], egq_cell_in_meshmc_rate[1], egq_cell_in_meshmc_rate[0] + egq_cell_in_meshmc_rate[1]); 
  printf("\t| Out Meshmc Rate     |                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n",egq_cell_out_meshmc_rate[0], egq_cell_out_meshmc_rate[1], egq_cell_out_meshmc_rate[0] + egq_cell_out_meshmc_rate[1]);
  printf("\t| Meshmc ovf drop Rate|                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n",egq_cell_ovf_meshmc_drop_rate[0], egq_cell_ovf_meshmc_drop_rate[1], egq_cell_ovf_meshmc_drop_rate[0]+egq_cell_ovf_meshmc_drop_rate[1]);
  printf("\t| In  TDM Rate        |                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n",egq_cell_in_tdm_rate[0], egq_cell_in_tdm_rate[1], egq_cell_in_tdm_rate[0]+egq_cell_in_tdm_rate[1]);
  printf("\t| Out TDM Rate        |                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n",egq_cell_out_tdm_rate[0], egq_cell_out_tdm_rate[1], egq_cell_out_tdm_rate[0] + egq_cell_out_tdm_rate[1]);
  printf("\t| Out FAB FIFO0       |                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n", egq_cell_out_rate_fab_fifo[0][0], egq_cell_out_rate_fab_fifo[1][0], egq_cell_out_rate_fab_fifo[0][0] + egq_cell_out_rate_fab_fifo[1][0]);
  printf("\t| Out FAB FIFO1       |                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n", egq_cell_out_rate_fab_fifo[0][1], egq_cell_out_rate_fab_fifo[1][1], egq_cell_out_rate_fab_fifo[0][1] + egq_cell_out_rate_fab_fifo[1][1]);
  printf("\t| Out FAB FIFO2       |                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n", egq_cell_out_rate_fab_fifo[0][2], egq_cell_out_rate_fab_fifo[1][2], egq_cell_out_rate_fab_fifo[0][2] + egq_cell_out_rate_fab_fifo[1][2]);
  printf("\t| Out FAB PIPE0       |                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n", egq_cell_out_rate_fab_pipe[0][0], egq_cell_out_rate_fab_pipe[1][0], egq_cell_out_rate_fab_pipe[0][0] + egq_cell_out_rate_fab_pipe[1][0]);
  printf("\t| Out FAB PIPE1       |                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n", egq_cell_out_rate_fab_pipe[0][1], egq_cell_out_rate_fab_pipe[1][1], egq_cell_out_rate_fab_pipe[0][1] + egq_cell_out_rate_fab_pipe[1][1]);
  printf("\t| Out FAB PIPE2       |                    %8.2f Mcps   |                    %8.2f Mcps   |                    %8.2f Mcps   |\n", egq_cell_out_rate_fab_pipe[0][2], egq_cell_out_rate_fab_pipe[1][2], egq_cell_out_rate_fab_pipe[0][2] + egq_cell_out_rate_fab_pipe[1][2]);
  printf("\t| In  FAB P1          |                                    |                                    |                    %8.2f Mcps   |\n", egq_cell_in_rate_p[0]);
  printf("\t| In  FAB P2          |                                    |                                    |                    %8.2f Mcps   |\n", egq_cell_in_rate_p[1]);
  printf("\t| In  FAB P3          |                                    |                                    |                    %8.2f Mcps   |\n", egq_cell_in_rate_p[2]);
  printf("\t+---------------------+--------------------------------------------------------------------------------------------------------------+\n");

  bshell(unit,"m FDA_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
}

void jericho_egq_rate (int unit) {
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
  double   EHP_UC_Rate_0,EHP_DIS_Rate_0,EHP_MC_H_Rate_0,EHP_MC_L_Rate_0,PQP_UC_Rate_0,PQP_DIS_UC_Rate_0,PQP_MC_Rate_0,PQP_DIS_MC_Rate_0,FQP_Rate_0, \
           EHP_UC_Rate_1,EHP_DIS_Rate_1,EHP_MC_H_Rate_1,EHP_MC_L_Rate_1,PQP_UC_Rate_1,PQP_DIS_UC_Rate_1,PQP_MC_Rate_1,PQP_DIS_MC_Rate_1,FQP_Rate_1  ;
  double    PQP_UC_Byte_Rate_0, PQP_UC_Byte_Rate_1, PQP_MC_Byte_Rate_0, PQP_MC_Byte_Rate_1;  
  double   EHP_DIS_MCL_Rate_0, EHP_DIS_MCL_Rate_1, EHP_DIS_MCH_Rate_0, EHP_DIS_MCH_Rate_1;
  
  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 7,200,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  // Enable gtimer for EGQ for clk of 720MHz:

  sprintf(cmd,"s EGQ_GTIMER_CONFIG TIMER_CONFIG=%d",nof_clocks_int);
  bshell(unit, cmd);
  sprintf(cmd,"m EGQ_CHECK_BW_TO_IFC CHECK_BW_TO_IFC=0 CHECK_BW_TO_IFC_EN=0");
  bshell(unit, cmd);

  bshell(unit,"s EGQ_GTIMER_CONFIG_CONT CLEAR_GTIMER=1 ACTIVATE_GTIMER=0");
  bshell(unit,"s EGQ_GTIMER_CONFIG_CONT CLEAR_GTIMER=1 ACTIVATE_GTIMER=1");

  sal_usleep(gtimer_duration * 10 * 1000); // Wait 0.1 sec

  clock_ratio = nof_clocks / (core_freq * 1000);

  EHP_MC_H_Rate_0   = rate_by_field_post_gtimer(unit,"EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER.EGQ0"            , 0 , clock_ratio);
  EHP_MC_H_Rate_1   = rate_by_field_post_gtimer(unit,"EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER.EGQ1"            , 0 , clock_ratio);
  EHP_MC_L_Rate_0   = rate_by_field_post_gtimer(unit,"EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER.EGQ0"             , 0 , clock_ratio);
  EHP_MC_L_Rate_1   = rate_by_field_post_gtimer(unit,"EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER.EGQ1"             , 0 , clock_ratio);
  EHP_UC_Rate_0     = rate_by_field_post_gtimer(unit,"EGQ_EHP_UNICAST_PACKET_COUNTER.EGQ0"                   , 0, clock_ratio);
  EHP_UC_Rate_1     = rate_by_field_post_gtimer(unit,"EGQ_EHP_UNICAST_PACKET_COUNTER.EGQ1"                   , 0, clock_ratio);
  EHP_DIS_Rate_0    = rate_by_field_post_gtimer(unit,"EGQ_EHP_DISCARD_PACKET_COUNTER.EGQ0"                   , 0, clock_ratio);
  EHP_DIS_Rate_1    = rate_by_field_post_gtimer(unit,"EGQ_EHP_DISCARD_PACKET_COUNTER.EGQ1"                   , 0, clock_ratio);
  
  EHP_DIS_MCL_Rate_0    = rate_by_field_post_gtimer(unit,"EGQ_EHP_MULTICAST_LOW_DISCARDS_COUNTER.EGQ0"                   , 0, clock_ratio);
  EHP_DIS_MCL_Rate_1    = rate_by_field_post_gtimer(unit,"EGQ_EHP_MULTICAST_LOW_DISCARDS_COUNTER.EGQ1"                   , 0, clock_ratio);
  EHP_DIS_MCH_Rate_0    = rate_by_field_post_gtimer(unit,"EGQ_EHP_MULTICAST_HIGH_DISCARDS_COUNTER.EGQ0"                   , 0, clock_ratio);
  EHP_DIS_MCH_Rate_1    = rate_by_field_post_gtimer(unit,"EGQ_EHP_MULTICAST_HIGH_DISCARDS_COUNTER.EGQ1"                   , 0, clock_ratio);

  PQP_UC_Rate_0     = rate_by_field_post_gtimer(unit,"EGQ_PQP_UNICAST_PACKET_COUNTER.EGQ0"                   , 0, clock_ratio);
  PQP_UC_Rate_1     = rate_by_field_post_gtimer(unit,"EGQ_PQP_UNICAST_PACKET_COUNTER.EGQ1"                   , 0, clock_ratio);
  PQP_DIS_MC_Rate_0 = rate_by_field_post_gtimer(unit,"EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER.EGQ0"           , 0, clock_ratio);
  PQP_DIS_MC_Rate_1 = rate_by_field_post_gtimer(unit,"EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER.EGQ1"           , 0, clock_ratio);
  PQP_MC_Rate_0     = rate_by_field_post_gtimer(unit,"EGQ_PQP_MULTICAST_PACKET_COUNTER.EGQ0"                 , 0, clock_ratio);
  PQP_MC_Rate_1     = rate_by_field_post_gtimer(unit,"EGQ_PQP_MULTICAST_PACKET_COUNTER.EGQ1"                 , 0, clock_ratio);
  PQP_DIS_MC_Rate_0 = rate_by_field_post_gtimer(unit,"EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER.EGQ0"         , 0, clock_ratio);
  PQP_DIS_MC_Rate_1 = rate_by_field_post_gtimer(unit,"EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER.EGQ1"         , 0, clock_ratio);
  FQP_Rate_0 = rate_by_field_post_gtimer(unit,"EGQ_FQP_PACKET_COUNTER.EGQ0"                                  , 0, clock_ratio);
  FQP_Rate_1 = rate_by_field_post_gtimer(unit,"EGQ_FQP_PACKET_COUNTER.EGQ1"                                  , 0, clock_ratio);

  PQP_UC_Byte_Rate_0     = rate_by_field_post_gtimer(unit,"EGQ_PQP_UNICAST_BYTES_COUNTER.EGQ0"               , 1, clock_ratio);
  PQP_MC_Byte_Rate_0     = rate_by_field_post_gtimer(unit,"EGQ_PQP_MULTICAST_BYTES_COUNTER.EGQ0"             , 1, clock_ratio);
  PQP_UC_Byte_Rate_1     = rate_by_field_post_gtimer(unit,"EGQ_PQP_UNICAST_BYTES_COUNTER.EGQ1"               , 1, clock_ratio);
  PQP_MC_Byte_Rate_1     = rate_by_field_post_gtimer(unit,"EGQ_PQP_MULTICAST_BYTES_COUNTER.EGQ1"             , 1, clock_ratio);

  printf("\t+------------------------------------------------------------------------------------------------------------------------------------+\n");
  printf("\t|                                                              EGQ Rates                                                             |\n");
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");
  printf("\t| Counter             |               Core 0               |               Core 1               |               Total                |\n");
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");
  printf("\t| EHP UC         Rate |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",EHP_UC_Rate_0,EHP_UC_Rate_1,EHP_UC_Rate_0+EHP_UC_Rate_1);
  printf("\t| EHP MC High    Rate |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",EHP_MC_H_Rate_0,EHP_MC_H_Rate_1,EHP_MC_H_Rate_0+EHP_MC_H_Rate_1);
  printf("\t| EHP MC Low     Rate |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",EHP_MC_L_Rate_0,EHP_MC_L_Rate_1,EHP_MC_L_Rate_0+EHP_MC_L_Rate_1);
  printf("\t| EHP Discard    Rate |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",EHP_DIS_Rate_0,EHP_DIS_Rate_1,EHP_DIS_Rate_0+EHP_DIS_Rate_1);
  printf("\t| EHP MCH Discrd Rate |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",EHP_DIS_MCH_Rate_0,EHP_DIS_MCH_Rate_1,EHP_DIS_MCH_Rate_0+EHP_DIS_MCH_Rate_1);
  printf("\t| EHP MCL Discrd Rate |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",EHP_DIS_MCL_Rate_0,EHP_DIS_MCL_Rate_1,EHP_DIS_MCL_Rate_0+EHP_DIS_MCL_Rate_1);
  printf("\t| PQP UC         Rate | %8.2f Gbps      %8.2f Mpps   | %8.2f Gbps      %8.2f Mpps   |                    %8.2f Mpps   |\n", PQP_UC_Byte_Rate_0, PQP_UC_Rate_0, PQP_UC_Byte_Rate_1, PQP_UC_Rate_1,PQP_UC_Rate_0+PQP_UC_Rate_1);
  printf("\t| PQP Discard UC Rate |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",PQP_DIS_UC_Rate_0,PQP_DIS_UC_Rate_1,PQP_DIS_UC_Rate_0+PQP_DIS_UC_Rate_1);
  printf("\t| PQP MC         Rate | %8.2f Gbps      %8.2f Mpps   | %8.2f Gbps      %8.2f Mpps   |                    %8.2f Mpps   |\n", PQP_MC_Byte_Rate_0, PQP_MC_Rate_0, PQP_MC_Byte_Rate_1, PQP_MC_Rate_1,PQP_MC_Rate_0+PQP_MC_Rate_1);
  printf("\t| PQP Discard MC Rate |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",PQP_DIS_MC_Rate_0,PQP_DIS_MC_Rate_1,PQP_DIS_MC_Rate_0+PQP_DIS_MC_Rate_1);
  printf("\t| FQP            Rate |                    %8.2f Mpps   |                    %8.2f Mpps   |                    %8.2f Mpps   |\n",FQP_Rate_0,FQP_Rate_1,FQP_Rate_0+FQP_Rate_1);
  printf("\t+---------------------+------------------------------------+------------------------------------+------------------------------------+\n");

  bshell(unit,"s EGQ_GTIMER_CONFIG_CONT CLEAR_GTIMER=0x0,ACTIVATE_GTIMER=0x0,");
}

void print_all_rates(int unit) {

	printf("GTimer duration is %2.2fms\n", gtimer_duration);
	
	jericho_nbi_rate(unit);
	jericho_ire_rate(unit);
	jericho_mmu_rate(unit);
	jericho_iqm_rate(unit);
	jericho_ipt_rate(unit);
	jericho_fdt_fdr_rate(unit);
	jericho_fda_rate(unit);
	jericho_egq_rate(unit);
}

print_all_rates(0);
