
// QAX IPS Debug CINT

int print_functions()
{	
	printf("\n");
	printf("CINT diag_ingress_qax.c\n");
	printf("----------\n");
	printf("Functions:\n");
	printf("\t1. print_all(unit);\n");
	printf("\t2. print_pts_gci_status(unit);\n");
	printf("\t3. print_flow_control_status(unit);\n");
	printf("\n");
}

int print_all(int unit)
{
	print_flow_control_status(unit);
	printf("\n");
	print_pts_gci_status(unit);
	printf("\n");
}

int print_flow_control_status(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	double percentage;
	
	printf("PTS to IPT FC:\n", core);
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		sprintf(buffer_reg,"PTS_PTS_TO_IPT_FC_STATUS_VEC_STICKY.PTS%d", core);
		
		sprintf(buffer_field, "PDQ_FC_STICKY");	
		diag_reg_field_set(unit, buffer_reg, buffer_field, val);
		printf("\tPDQ\t=%d\n", val[0]);
		
		sprintf(buffer_field, "GFMC_FC_STICKY");	
		diag_reg_field_set(unit, buffer_reg, buffer_field, val);
		printf("\tGFMC\t=%d\n", val[0]);
		
		sprintf(buffer_field, "BFMC_FC_STICKY");	
		diag_reg_field_set(unit, buffer_reg, buffer_field, val);
		printf("\tBFMC\t=%d\n", val[0]);
		
		sprintf(buffer_field, "EIR_CRDT_FC_STICKY");	
		diag_reg_field_set(unit, buffer_reg, buffer_field, val);
		printf("\tEIR_CR\t=%d\n", val[0]);
	}
}

void pts_enable_gtimer(int unit, int core, int nof_clocks_int){
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	val[0] = nof_clocks_int;
	sprintf(buffer_reg,"PTS_GTIMER_CONFIGURATION.PTS%d", core);
	sprintf(buffer_field, "GTIMER_CYCLE");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"PTS_GTIMER_CONFIGURATION.PTS%d", core);
	sprintf(buffer_field, "GTIMER_RESET_ON_TRIGGER");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"PTS_GTIMER_CONFIGURATION.PTS%d", core);
	sprintf(buffer_field, "GTIMER_ENABLE");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 0;
	sprintf(buffer_reg,"PTS_GTIMER_CONFIGURATION.PTS%d", core);
	sprintf(buffer_field, "GTIMER_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"PTS_GTIMER_CONFIGURATION.PTS%d", core);
	sprintf(buffer_field, "GTIMER_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
}

void pts_disable_gtimer(int unit, int core) {
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	val[0] = 1;
	sprintf(buffer_reg,"PTS_GTIMER_CONFIGURATION.PTS%d", core);
	sprintf(buffer_field, "GTIMER_RESET_ON_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 0;
	sprintf(buffer_reg,"PTS_GTIMER_CONFIGURATION.PTS%d", core);
	sprintf(buffer_field, "GTIMER_ENABLE");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
}

void print_pts_gci_status(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	printf("PTS GCI Status:\n");
	
	int found = 0;
	
	// GTIMER Values
	double sch_device_freq_MHz = soc_property_get(0 ,"core_clock_speed_khz",-1) / 1000 ;
	double nof_clocks     = sch_device_freq_MHz * 10 * 1000; // 7,200,000 clocks --> 0.01 sec
	int nof_clocks_int = nof_clocks;
	double clock_ratio = nof_clocks / (sch_device_freq_MHz * 1000 * 1000);
	
	int gci;
	for(gci = 0; gci < 4; gci++)
	{
		val[0] = gci;
		sprintf(buffer_reg, "PTS_PTS_DEBUG_CONTROLS");
		sprintf(buffer_field, "GCI_CNT_SEL");
		diag_reg_field_set(unit, buffer_reg, buffer_field, val);
		
		pts_enable_gtimer(unit, 0, nof_clocks_int);
		sal_usleep(100000); // Wait 0.1 sec
		
		sprintf(buffer_reg, "PTS_GCI_CNT");
		sprintf(buffer_field, "GCI_CNT");
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		val[0] = val[0] & 0x7FFFFFFF; //Mask MSB
		printf("\tGCI %d: %d%%\n", gci, val[0] * 100 / nof_clocks_int);
	}
	
	pts_disable_gtimer(unit, 0);  
}

int get_number_of_cores(int unit)
{
	char *s;
	s = soc_property_get_str(unit, "device_core_mode");
	
	if(s == NULL) return 1; // Arad, QAX
	
	if (sal_strcmp(s, "SYMMETRIC") == 0) {
		return 2;
	} 
	if (sal_strcmp(s, "SINGLE_CORE") == 0) {
		return 1;
	}
	
	return 2; // Assume Jericho as default
}

int print_cgm_occupancy(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	printf("CGM Occupancy:\n");
	
	sprintf(buffer_reg, "CGM_SRAM_BUFFERS_FREE_MIN_STATUS");
	sprintf(buffer_field, "SRAM_BUFFERS_FREE_MIN_STATUS");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	printf("\tFree SRAM Buffers:\t%d\t(%d%%)\n", val[0], val * 100 / 16384);
	
	sprintf(buffer_reg, "CGM_SRAM_PDBS_FREE_MIN_STATUS");
	sprintf(buffer_field, "SRAM_PDBS_FREE_MIN_STATUS");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	printf("\tFree SRAM PDBs:\t%d\t(%d%%)", val[0], val * 100 / 16380); // Without prefetch
	printf("\t(Including Prefetch: %d%%)\n", val * 100 / 16384);
}

print_functions();
print_all(0);