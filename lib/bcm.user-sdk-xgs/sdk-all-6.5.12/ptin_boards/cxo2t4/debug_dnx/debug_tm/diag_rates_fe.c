// DNX - Measure Data Rates
// ========================
// Intended for FE use.

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

void rate_by_field_post_gtimer_dcm(uint32 unit,char* field,double clock_ratio, double* dcm_rate) 
{
  uint32  counter_data[2];
  uint32  counter_OVF;
  uint32  counter_result[2];
  double  rate;

  common_read_reg_long(unit,field,&counter_data,2);
  counter_result[0]  = get_field(counter_data[0], 0, 14); 
  counter_result[1]  = get_field(counter_data[0], 16, 30); 
  dcm_rate[0]            = counter_result[0] / clock_ratio / 1000000 ; // Bytes --> Bps
  dcm_rate[1]            = counter_result[1] / clock_ratio / 1000000 ; // Bytes --> Bps
  return;
}

double rate_by_field_post_gtimer_nondefult (uint32 unit,char* field,uint32 byte_cnt,
                                          double clock_ratio, int startbit, int endbit)
{

  uint32  counter_data[2];
  uint32  counter_OVF;
  uint32  counter_result[2];
  double  rate;

  common_read_reg_long(unit,field,&counter_data,2);
  counter_result[0]  = get_field(counter_data[0],startbit,endbit); 
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

void dch_rate(uint32 unit) 
{
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  double    dch_incell[3][4];
  double    dch_outcell[3][4];
  double    fifo_discard[3][4];
  double    totle_incell[3];
  double    totle_outcell[3];
  double    totle_fifodiscard[3];
  double    dch_unreachcell[4];
  double    totle_dch_unreachcell;
 
  int       pipe, mod_num;

  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 7,200,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;

  sprintf(cmd, "s DCH_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  
  bshell(unit,"s DCH_GTIMER_TRIGGER GTIMER_TRIGGER=1");

  sal_usleep(gtimer_duration * 10 * 1000); // Wait 0.1 sec

  clock_ratio = nof_clocks / (core_freq * 1000);
  
  for (pipe=0; pipe<3; pipe++) {
      totle_incell[pipe] = totle_outcell[pipe] = 0;
      for (mod_num=0; mod_num<4; mod_num++) {
          sprintf(cmd, "DCH_DCH_TOTAL_CELL_CNT_P_%d.DCH%d", pipe, mod_num);
          dch_incell[pipe][mod_num] = rate_by_field_post_gtimer(unit, cmd, 0, clock_ratio);

          sprintf(cmd, "DCH_DCH_TOTAL_OUT_CELL_CNT_P_%d.DCH%d", pipe, mod_num);
          dch_outcell[pipe][mod_num] = rate_by_field_post_gtimer(unit, cmd, 0, clock_ratio);

          sprintf(cmd, "DCH_FIFO_DISCARD_COUNTER_P(%d).DCH%d", pipe, mod_num);
          fifo_discard[pipe][mod_num] = rate_by_field_post_gtimer(unit, cmd, 0, clock_ratio);

          totle_incell[pipe]  += dch_incell[pipe][mod_num]; 
          totle_outcell[pipe] += dch_outcell[pipe][mod_num];
          totle_fifodiscard[pipe] += fifo_discard[pipe][mod_num];
      }
  }
  totle_dch_unreachcell = 0;
  for (mod_num=0; mod_num<4; mod_num++) {
          sprintf(cmd, "DCH_UN_REACH_DEST.DCH%d", mod_num);
          dch_unreachcell[mod_num] = rate_by_field_post_gtimer_nondefult(unit, cmd, 0, clock_ratio, 0, 14);
          totle_dch_unreachcell += dch_unreachcell[mod_num];
  }

  printf("\t+------------------------------------------------------------------------------------------------------------------------------------+\n");
  printf("\t|                                                              DCH Rates                                                             |\n"); 
  printf("\t+---------------------+---------------------+---------------------+---------------------+---------------------+----------------------+\n"); 
  printf("\t| Counter             |      DCH 0          |        DCH1         |        DCH2         |        DCH3         |       Total          |\n");
  printf("\t+---------------------+---------------------+---------------------+---------------------+---------------------+----------------------+\n");
  for (pipe=0; pipe<3; pipe++) {
      printf("\t|IN   CELL PIPE%d Rate |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps    |\n", 
             pipe, dch_incell[pipe][0], dch_incell[pipe][1],  dch_incell[pipe][2], dch_incell[pipe][3], totle_incell[pipe]);
      printf("\t|OUT  CELL PIPE%d Rate |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps    |\n",
             pipe, dch_outcell[pipe][0], dch_outcell[pipe][1],  dch_outcell[pipe][2], dch_outcell[pipe][3], totle_outcell[pipe]);
      printf("\t|FIFO DISC PIPE%d Rate |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps    |\n",
             pipe, fifo_discard[pipe][0], fifo_discard[pipe][1],  fifo_discard[pipe][2], fifo_discard[pipe][3], totle_fifodiscard[pipe]);
      printf("\t+---------------------+---------------------+---------------------+---------------------+---------------------+----------------------+\n"); 
 
  }
  printf("\t|UNREACH   CELL  Rate |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps    |\n",
            dch_unreachcell[0], dch_unreachcell[1],  dch_unreachcell[2], dch_unreachcell[3], totle_dch_unreachcell);
  printf("\t+---------------------+---------------------+---------------------+---------------------+---------------------+----------------------+\n"); 
 
  bshell(unit,"m DCH_GTIMER_CONFIGURATION GTIMER_ENABLE=0");
}

void dcm_rate(uint32 unit) 
{
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  double    dcm_rates[2];
  double    drop_cell[3][4];
  double    totle_dropcell[3];
  double    in_cell[3][4][4];
  double    in_cell_totle[3][4];
  int       mod_num, pipe;


  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration / 10; // 7,200,000 clocks --> 0.01 sec (divide by 10 due to DCM small counter)
  nof_clocks_int = nof_clocks;
  clock_ratio = nof_clocks / (core_freq * 1000);
 

  sprintf(cmd, "s DCM_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  
  bshell(unit,"s DCM_GTIMER_TRIGGER GTIMER_TRIGGER=1");

  sal_usleep(gtimer_duration * 10 * 1000); // Wait 0.1 sec

  for (pipe=0; pipe<3; pipe++) {
      for (mod_num=0; mod_num<4; mod_num++) {
          in_cell_totle[pipe][mod_num] = 0;

          sprintf(cmd, "DCM_TOTAL_IN_CELL_CNT_PAB(%d).DCM%d", pipe, mod_num);
          rate_by_field_post_gtimer_dcm(unit, cmd, clock_ratio, dcm_rates);
          in_cell[pipe][mod_num][0] = dcm_rates[0];
          in_cell[pipe][mod_num][1] = dcm_rates[1];

          sprintf(cmd, "DCM_TOTAL_IN_CELL_CNT_PCD(%d).DCM%d", pipe, mod_num);
          rate_by_field_post_gtimer_dcm(unit, cmd, clock_ratio, dcm_rates);
          in_cell[pipe][mod_num][2] = dcm_rates[0];
          in_cell[pipe][mod_num][3] = dcm_rates[1];
          
          in_cell_totle[pipe][mod_num] = in_cell[pipe][mod_num][0] + in_cell[pipe][mod_num][1] + in_cell[pipe][mod_num][2] + in_cell[pipe][mod_num][3];
      }
  }


  for (pipe=0; pipe<3; pipe++) {
      totle_dropcell[pipe] = 0;
      for (mod_num=0; mod_num<4; mod_num++) {
          sprintf(cmd, "DCM_DROP_TOTAL_P_CNT(%d).DCM%d", pipe, mod_num);
          drop_cell[pipe][mod_num] = rate_by_field_post_gtimer(unit, cmd, 0, clock_ratio);
          totle_dropcell[pipe]  += drop_cell[pipe][mod_num]; 
      }
  }
 
  printf("\t+------------------------------------------------------------------------------------------------------------------------------------+\n");
  printf("\t|                                                              DCM Rates                                                             |\n"); 
  printf("\t+---------------------+---------------------+---------------------+---------------------+---------------------+----------------------+\n"); 
  printf("\t| Counter             |       DCM0          |        DCM1         |        DCM2         |        DCM3         |       Total          |\n");
  printf("\t+---------------------+---------------------+---------------------+---------------------+---------------------+----------------------+\n");
  for (pipe=0; pipe<3; pipe++) {
      for (mod_num=0; mod_num<4; mod_num++) {
          printf("\t|  IN   PIPE %d   DCH%d |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps    |\n", 
                 pipe, mod_num, in_cell[pipe][0][mod_num], in_cell[pipe][1][mod_num],  in_cell[pipe][2][mod_num], in_cell[pipe][3][mod_num],
                 in_cell[pipe][0][mod_num]+in_cell[pipe][1][mod_num]+in_cell[pipe][2][mod_num]+in_cell[pipe][3][mod_num]);
      }
      printf("\t|  IN   PIPE %d  Total |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps    |\n", 
                 pipe, in_cell_totle[pipe][0], in_cell_totle[pipe][1],  in_cell_totle[pipe][2], in_cell_totle[pipe][3], 
                 in_cell_totle[pipe][0]+in_cell_totle[pipe][1]+in_cell_totle[pipe][2]+in_cell_totle[pipe][3]);
      printf("\t+---------------------+---------------------+---------------------+---------------------+---------------------+----------------------+\n"); 
  }
  for (pipe=0; pipe<3; pipe++) {
      printf("\t|DROP CELL PIPE%d Rate |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps    |\n", 
                 pipe, drop_cell[pipe][0], drop_cell[pipe][1],  drop_cell[pipe][2], drop_cell[pipe][3], totle_dropcell[pipe]);
  }
                 
  printf("\t+---------------------+---------------------+---------------------+---------------------+---------------------+----------------------+\n"); 
 


  bshell(unit,"m DCM_GTIMER_CONFIGURATION GTIMER_ENABLE=0");

}

void dcl_rate(uint32 unit) 
{
  char      cmd[800];
  double    nof_clocks;
  int       core_freq;
  int       nof_clocks_int;
  double    clock_ratio;
  int       pipe, mod_num, pri;
  core_freq      = soc_property_get(unit ,"core_clock_speed_khz",-1);
  nof_clocks     = core_freq * gtimer_duration; // 7,200,000 clocks --> 0.01 sec
  nof_clocks_int = nof_clocks;
  clock_ratio = nof_clocks / (core_freq * 1000);
  sprintf(cmd, "s DCL_GTIMER_CONFIGURATION GTIMER_ENABLE=1 GTIMER_RESET_ON_TRIGGER=1 GTIMER_CYCLE=%d",nof_clocks_int);
  bshell(unit, cmd);
  bshell(unit,"s DCL_GTIMER_TRIGGER GTIMER_TRIGGER=1");
  sal_usleep(gtimer_duration * 10 * 1000); // Wait 0.1 sec
  

  double in_cell[3][4], out_cell[3][4], totle_incell[3], totle_outcell[3]; 
  double disc_cell[3][4], totle_dropcell[3];
  for (pipe=0; pipe<3; pipe++) {
      totle_dropcell[pipe] = 0;
      totle_incell[pipe] = 0;
      totle_outcell[pipe] = 0;
      for (mod_num=0; mod_num<4; mod_num++) {
          sprintf(cmd, "DCL_TOTAL_IN_CELL_P_CNT(%d).DCL%d", pipe, mod_num);
          in_cell[pipe][mod_num] = rate_by_field_post_gtimer(unit, cmd, 0, clock_ratio);
          totle_incell[pipe]  += in_cell[pipe][mod_num]; 

          sprintf(cmd, "DCL_TOTAL_OUT_CELL_P_CNT(%d).DCL%d", pipe, mod_num);
          out_cell[pipe][mod_num] = rate_by_field_post_gtimer(unit, cmd, 0, clock_ratio);
          totle_outcell[pipe]  += out_cell[pipe][mod_num]; 
            
          disc_cell[pipe][mod_num] = 0;
          for(pri=0; pri<4; pri++) {
              sprintf(cmd, "DCL_DROPPED_IP_PR_P_%d_CNT(%d).DCL%d", pipe, pri, mod_num);
              disc_cell[pipe][mod_num] += rate_by_field_post_gtimer(unit, cmd, 0, clock_ratio);
          }
          totle_dropcell[pipe] += disc_cell[pipe][mod_num];
      }
  }

 
  printf("\t+------------------------------------------------------------------------------------------------------------------------------------+\n");
  printf("\t|                                                              DCL Rates                                                             |\n"); 
  printf("\t+---------------------+---------------------+---------------------+---------------------+---------------------+----------------------+\n"); 
  printf("\t| Counter             |       DCL0          |        DCL1         |        DCL2         |        DCL3         |       Total          |\n");
  printf("\t+---------------------+---------------------+---------------------+---------------------+---------------------+----------------------+\n");
  for (pipe=0; pipe<3; pipe++) {
      printf("\t|IN   CELL PIPE%d Rate |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps    |\n", 
                 pipe, in_cell[pipe][0], in_cell[pipe][1],  in_cell[pipe][2], in_cell[pipe][3], totle_incell[pipe]);
      printf("\t|OUT  CELL PIPE%d Rate |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps    |\n", 
                 pipe, out_cell[pipe][0], out_cell[pipe][1],  out_cell[pipe][2], out_cell[pipe][3], totle_outcell[pipe]);
      printf("\t|DISC CELL PIPE%d Rate |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps   |     %8.2f Mcps    |\n", 
                 pipe, disc_cell[pipe][0], disc_cell[pipe][1],  disc_cell[pipe][2], disc_cell[pipe][3], totle_dropcell[pipe]);
 
      printf("\t+---------------------+---------------------+---------------------+---------------------+---------------------+----------------------+\n"); 
  }
                 



  bshell(unit,"m DCL_GTIMER_CONFIGURATION GTIMER_ENABLE=0");


}

void print_all_rates(int unit) {

	printf("GTimer duration is %2.2fms\n", gtimer_duration);
	
	dch_rate(unit);
	dcm_rate(unit);
	dcl_rate(unit);
}

print_all_rates(0);