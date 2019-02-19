
// Jericho Flow Control Debug CINT

int qpair_to_port[2][256];
int nif_to_qpair[2][128];

int print_functions()
{	
	printf("\n");
	printf("CINT diag_fc.c\n");
	printf("----------\n");
	printf("Functions:\n");
	printf("\t1. print_all(unit);\n");
	printf("\t2. print_nif_egq_fc(unit);\n");
	printf("\t3. print_egq_sch_fc(unit);\n");
	printf("\t4. print_cfc_egq_fc(unit);\n");
	printf("\t5. print_sch_rci_status(unit);\n");
	printf("\t6. print_rci_watermark(unit);\n");
	printf("\t7. print_ipt_gci_status(unit);\n");
	printf("\t8. print_ipt_gci_backoff_level(unit);\n");
	printf("\n");
}

int print_all(int unit)
{
	print_nif_egq_fc(unit);
	printf("\n");
	print_egq_sch_fc(unit);
	printf("\n");
	print_cfc_egq_fc(unit);
	printf("\n");
	print_sch_rci_status(unit);
	printf("\n");
	print_rci_watermark(unit);
	printf("\n");
	print_ipt_gci_status(unit);
	printf("\n");
	print_ipt_gci_backoff_level(unit);
	printf("\n");
}

void map_qpair_to_port(int unit)
{
	char buffer_reg[128];
	reg_val val;
	
	int port, qpair;
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		int i;
		for(i = 0; i < 128; i++)
		{
			sprintf(buffer_reg, "EGQ_DSP_PTR_MAP.EGQ%d", core);
			diag_mem_get(unit, buffer_reg, i, val);
			qpair = val[0] & 0xFF;
			port = (val[0] >> 16) & 0xFFFF;
						
			if(qpair == 0xFF)
			{
				qpair_to_port[core][val[0]] = -1;
			}
			else
			{
				int priorities = get_port_number_of_priorities(unit, core, qpair);
				int j;
				for(j = 0; j < priorities; j++)
				{
					qpair_to_port[core][qpair + j] = port;
				}
			}
		}
		
	}
}

void map_nif_to_qpair(int unit)
{
	char buffer_reg[128];
	reg_val val;
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		int i;
		for(i = 0; i < 128; i++)
		{
			sprintf(buffer_reg, "EGQ_PER_IFC_CFG.EGQ%d", core);
			diag_mem_get(unit, buffer_reg, i, val);
			val[0] = (val[0] >> 23) & 0xFF;
			
			nif_to_qpair[core][i] = val[0];
		}
		
	}
}

void print_nif_egq_fc(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	printf("NIF --> EGQ:\n");
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		int found = 0;

		printf("\tCore %d:", core);
		
		sprintf(buffer_reg, "EGQ_NIF_FLOW_CONTROL.EGQ%d", core);
		sprintf(buffer_field, "NIF_FLOW_CONTROL");
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		if(val[0] != 0)
		{
			if(found == 0) printf("\n");
			found = 1;
			int j, bit;
			for(j = 0; j < 128; j++)
			{
				bit = (val[j / 32] >> (j % 32)) & 1;
				int qpair = nif_to_qpair[core][j];
				if(bit == 1 && qpair != 0xFF)
				{
					printf("\t\tNIF %d Port %d\n", j, qpair_to_port[core][qpair]);
				}
			}
		}
		
		if(found == 0) printf(" None\n");
	}
}

void print_egq_sch_fc(int unit)
{
	char buffer_reg[128];
	char buffer_field[128];
	reg_val val;
	
	printf("EGQ --> SCH:\n");
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		int found = 0;
		
		val[0] = core;
		sprintf(buffer_reg, "CFC_EGQ_SEL");
		sprintf(buffer_field, "EGQ_SEL");
		diag_reg_field_set(unit, buffer_reg, buffer_field, val);
		
		printf("\tCore %d:", core);
		
		// DEVICE LEVEL
		sprintf(buffer_reg, "CFC_EGQ_FC_STATUS");
		sprintf(buffer_field, "EGQ_DEV_FC");
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		if(val[0] != 0)
		{
			if(found == 0) printf("\n");
			found = 1;
			printf("\t\tDevice level FC\n");
		}
		
		// ERP
		sprintf(buffer_reg, "CFC_EGQ_FC_STATUS");
		sprintf(buffer_field, "EGQ_ERP_FC");
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		if(val[0] != 0)
		{
			if(found == 0) printf("\n");
			found = 1;
			printf("\t\tERP FC\n");
		}
		
		// ERP TC
		sprintf(buffer_reg, "CFC_EGQ_FC_STATUS");
		sprintf(buffer_field, "EGQ_ERP_TC_FC");
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		if(val[0] != 0)
		{
			if(found == 0) printf("\n");
			found = 1;
			int j, bit;
			for(j = 0; j < 8; j++)
			{
				bit = (val[0] >> j) & 1;
				if(bit == 1)
				{
					printf("\t\tERP TC %d FC\n", j);
				}
			}
		}
		
		// Interface FC
		sprintf(buffer_reg, "CFC_EGQ_IF_FC_STATUS");
		sprintf(buffer_field, "EGQ_IF_FC_STATUS");
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		if(val[0] != 0)
		{
			if(found == 0) printf("\n");
			found = 1;
			int j, bit;
			for(j = 0; j < 128; j++)
			{
				bit = (val[j / 32] >> (j % 32)) & 1;
				if(bit == 1)
				{
					printf("\t\tNIF %d\n", j);
				}
			}
		}
		
		// QPAIR PFC
		int i;
		for(i = 0; i < 4; i++)
		{
			val[0] = i;
			val[1] = 0;
			
			sprintf(buffer_reg, "CFC_EGQ_STATUS_SEL");
			sprintf(buffer_field, "EGQ_PFC_STATUS_SEL");
			diag_reg_field_set(unit, buffer_reg, buffer_field, val);
			
			sprintf(buffer_reg, "CFC_EGQ_PFC_STATUS");
			sprintf(buffer_field, "EGQ_PFC_STATUS");
			diag_reg_field_get(unit, buffer_reg, buffer_field, val);
			diag_reg_field_get(unit, buffer_reg, buffer_field, val);
			
			if(val[0] != 0)
			{
				if(found == 0) printf("\n");
				found = 1;
				print_active_pfcs(core, i * 64, val[0]);
			}
			if(val[1] != 0)
			{
				if(found == 0) printf("\n");
				found = 1;
				print_active_pfcs(core, i * 64 + 32, val[1]);
			}
		}
		
		if(found == 0) printf(" None\n");
	}
}

void print_cfc_egq_fc(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	printf("CFC --> EGQ:\n");
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		int found = 0;

		printf("\tCore %d:", core);
		
		sprintf(buffer_reg, "EGQ_CFC_FLOW_CONTROL.EGQ%d", core);
		sprintf(buffer_field, "CFC_FLOW_CONTROL");
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		if(val[0] != 0)
		{
			if(found == 0) printf("\n");
			found = 1;
			int j, bit;
			for(j = 0; j < 256; j++)
			{
				bit = (val[j / 32] >> (j % 32)) & 1;
				if(bit == 1)
				{
					printf("\t\QPAIR %d Port %d\n", j, qpair_to_port[core][j]);
				}
			}
		}
		
		if(found == 0) printf(" None\n");
	}
}

void print_active_pfcs(int core, int offset, uint32 val)
{
	int j, bit;
	for(j = 0; j < 32; j++)
	{
		bit = (val >> j) & 1;
		if(bit == 1)
		{
			printf("\t\tPort %d QPair %d\n", qpair_to_port[core][offset + j], offset + j);
		}
	}
}

void sch_enable_gtimer(int unit, int core, int nof_clocks_int){
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	val[0] = nof_clocks_int;
	sprintf(buffer_reg,"SCH_GTIMER_CONFIGURATION.SCH%d", core);
	sprintf(buffer_field, "GTIMER_CYCLE");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"SCH_GTIMER_CONFIGURATION.SCH%d", core);
	sprintf(buffer_field, "GTIMER_RESET_ON_TRIGGER");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"SCH_GTIMER_CONFIGURATION.SCH%d", core);
	sprintf(buffer_field, "GTIMER_ENABLE");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 0;
	sprintf(buffer_reg,"SCH_GTIMER_TRIGGER.SCH%d", core);
	sprintf(buffer_field, "GTIMER_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"SCH_GTIMER_TRIGGER.SCH%d", core);
	sprintf(buffer_field, "GTIMER_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
}

void sch_disable_gtimer(int unit, int core) {
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	val[0] = 1;
	sprintf(buffer_reg,"SCH_GTIMER_CONFIGURATION.SCH%d", core);
	sprintf(buffer_field, "GTIMER_RESET_ON_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 0;
	sprintf(buffer_reg,"SCH_GTIMER_CONFIGURATION.SCH%d", core);
	sprintf(buffer_field, "GTIMER_ENABLE");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
}

void print_sch_rci_status(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	printf("SCH RCI Status:\n");
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		int found = 0;

		printf("\tCore %d:\n", core);
		
		// GTIMER Values
		double sch_device_freq_MHz = soc_property_get(0 ,"core_clock_speed_khz",-1) / 1000 ;
		double nof_clocks     = sch_device_freq_MHz * 10 * 1000; // 7,200,000 clocks --> 0.01 sec
		int nof_clocks_int = nof_clocks;
		double clock_ratio = nof_clocks / (sch_device_freq_MHz * 1000 * 1000);
		
		int rci;
		for(rci = 0; rci < 8; rci++)
		{
			val[0] = rci;
			sprintf(buffer_reg, "SCH_DBG_DVS_RCI_COUNTERS_CONFIGURATION.SCH%d", core);
			sprintf(buffer_field, "RCI_LEVEL");
			diag_reg_field_set(unit, buffer_reg, buffer_field, val);
			
			sch_enable_gtimer(unit, core, nof_clocks_int);
			sal_usleep(100000); // Wait 0.1 sec
			
			sprintf(buffer_reg, "SCH_DBG_DVS_RCI_COUNTER.SCH%d", core);
			sprintf(buffer_field, "DVS_RCI_CNT");
			diag_reg_field_get(unit, buffer_reg, buffer_field, val);
			
			printf("\t\tRCI %d: %d%%\n", rci, val[0] * 100 / nof_clocks_int);
		}
		
		sch_disable_gtimer(unit, core);  
	}
}

int get_port_number_of_priorities(int unit, int core, int qpair)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	sprintf(buffer_reg, "EGQ_PS_MODE.EGQ%d", core);
	sprintf(buffer_field, "PS_MODE");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	int ps = qpair / 8;
	int index = (ps * 2) / 32;
	int bit = (ps * 2) % 32;
	
	int mode = (val[index] >> bit) & 0x3;
	
	if(mode == 0) return 1;
	if(mode == 1) return 2;
	if(mode == 2) return 8;
	
	return 0;
}

void print_rci_watermark(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	printf("RCI Score Watermark:\n");
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		printf("\tCore %d:\n", core);
		
		sprintf(buffer_reg, "FDR_FDR_SCH_RCI_SCORE_WM[%d]", core);
		
		sprintf(buffer_field, "SCH_N_RCI_SCORE_MEM_CONG_WM");
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		printf("\t\tFDR Links Score\t\t=%d, RCI =%d\n", core, val[0], rci_score_to_level(unit, val[0]));
		
		sprintf(buffer_field, "SCH_N_RCI_SCORE_IPT_WM");
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		printf("\t\tIPT Score\t\t=%d, RCI =%d\n", val[0], rci_score_to_level(unit, val[0]));
		
		sprintf(buffer_field, "SCH_N_RCI_SCORE_HIGH_SEV_WM");
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		printf("\t\tFDR High Severity Score\t=%d, RCI =%d\n", val[0], rci_score_to_level(unit, val[0]));
		
		sprintf(buffer_field, "SCH_N_RCI_SCORE_FDA_WM");
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		printf("\t\tFDA FIFO RCI Score\t=%d, RCI =%d\n", val[0], rci_score_to_level(unit, val[0]));
	}
	
}

int rci_score_to_level(int unit, int score)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	sprintf(buffer_reg, "FDR_FDR_ENABLERS_REGISTER_3");
	
	int rci;
	for(rci = 1; rci < 8; rci++)
	{
		sprintf(buffer_field, "RCI_SCORE_LEVEL_TH_%d", rci);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		if(score < val[0]) return (rci - 1);
	}
	
	return 7;
}

int get_number_of_cores(int unit)
{
	char *s;
	s = soc_property_get_str(unit, "device_core_mode");
	
	if(s == NULL) return 1; // Arad
	
	if (sal_strcmp(s, "SYMMETRIC") == 0) {
		return 2;
	} 
	if (sal_strcmp(s, "SINGLE_CORE") == 0) {
		return 1;
	}
	
	return 2; // Assume Jericho as default
}

int print_ipt_gci_status(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	printf("IPT GCI Status:\n");
	
	// GTIMER Values
	double sch_device_freq_MHz = soc_property_get(0 ,"core_clock_speed_khz",-1) / 1000 ;
	double nof_clocks     = sch_device_freq_MHz * 10 * 1000; // 7,200,000 clocks --> 0.01 sec
	int nof_clocks_int = nof_clocks;
	double clock_ratio = nof_clocks / (sch_device_freq_MHz * 1000 * 1000);
	
	int gci;
	for(gci = 0; gci < 4; gci++)
	{
		val[0] = gci;
		sprintf(buffer_reg, "IPT_DEBUG_CONTROLS");
		sprintf(buffer_field, "GCI_CNT_SEL");
		diag_reg_field_set(unit, buffer_reg, buffer_field, val);
		
		ipt_enable_gtimer(unit, nof_clocks_int);
		sal_usleep(100000); // Wait 0.1 sec
		
		sprintf(buffer_reg, "IPT_GCI_CNT");
		sprintf(buffer_field, "GCI_CNT");
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		printf("\tGCI %d: %d%%\n", gci, val[0] * 100 / nof_clocks_int);
	}
		
	ipt_disable_gtimer(unit);
}

int print_ipt_gci_backoff_level(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	printf("IPT GCI Backoff Level:\n");
	
	
	int gci_thresholds[3];
	int i;
	for(i = 0; i < 3; i++)
	{
		sprintf(buffer_reg, "IPT_GCI_BACKOFF_RANGE_THRESHOLDS");
		sprintf(buffer_field, "CNGST_LVL_THRESH_%d", i);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		gci_thresholds[i] = val[0];
	}
	
	sprintf(buffer_reg, "PTS_GCI_BKFF_LEVEL");
	sprintf(buffer_field, "GCI_BKFF_MAX_LEVEL%d", i);
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	for(i = 0; i < 3; i++)
	{
		if(val[0] <= gci_thresholds[i])
		{
			printf("\tGCI Level is %d (%d)\n", i, val[0]);
			break;
		}
		else if(i == 2)
		{
			printf("\tGCI Level is 3 (%d)\n", val[0]);
		}
	}
}

void ipt_enable_gtimer(int unit, int nof_clocks_int){
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	val[0] = nof_clocks_int;
	sprintf(buffer_reg,"IPT_GTIMER_CONFIGURATION");
	sprintf(buffer_field, "GTIMER_CYCLE");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"IPT_GTIMER_CONFIGURATION");
	sprintf(buffer_field, "GTIMER_RESET_ON_TRIGGER");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"IPT_GTIMER_CONFIGURATION");
	sprintf(buffer_field, "GTIMER_ENABLE");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 0;
	sprintf(buffer_reg,"IPT_GTIMER_TRIGGER");
	sprintf(buffer_field, "GTIMER_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"IPT_GTIMER_TRIGGER");
	sprintf(buffer_field, "GTIMER_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
}

void ipt_disable_gtimer(int unit) {
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	val[0] = 1;
	sprintf(buffer_reg,"IPT_GTIMER_CONFIGURATION");
	sprintf(buffer_field, "GTIMER_RESET_ON_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 0;
	sprintf(buffer_reg,"IPT_GTIMER_CONFIGURATION");
	sprintf(buffer_field, "GTIMER_ENABLE");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
}


map_qpair_to_port(0);
map_nif_to_qpair(0);

print_functions();
print_all(0);


