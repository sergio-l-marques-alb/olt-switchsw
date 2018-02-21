
// QAX IPS Debug CINT

int print_functions()
{	
	printf("\n");
	printf("CINT diag_ips_qax.c\n");
	printf("----------\n");
	printf("Functions:\n");
	printf("\t1. print_all(unit);\n");
	printf("\t2. print_max_active_queues(unit);\n");
	printf("\t3. print_deleted_credit_rate(unit);\n");
	printf("\t4. print_returned_credit_rate(unit);\n");
	printf("\t5. print_max_dequeue_command(unit);\n");
	printf("\t6. print_queue_status(int unit, int queue);\n");
	printf("\t7. print_all_queues_status(int unit);\n");
	printf("\n");
}

int print_all(int unit)
{
	print_max_active_queues(unit);
	printf("\n");
	print_deleted_credit_rate(unit);
	printf("\n");
	print_returned_credit_rate(unit);
	printf("\n");
	print_max_dequeue_command(unit);
	printf("\n");
	print_ips_credit_rate(unit);
	printf("\n");
	if(get_max_active_queues(unit, 0) < 10)
	{
		print_all_queues_status(unit);
	}
	else
	{
		printf("Skipped printing Active Queues\n");
		printf("You can run it manually by calling: print_all_queues_status(unit);\n");
	}
	
	printf("\n");
}

// Print Max Active Queues Count
int print_max_active_queues(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		val[0] = get_max_active_queues(unit, core);
		printf("Core %d: Max Total Active Queues = %d\n", core, val[0]);
		
		sprintf(buffer_reg,"IPS_MAX_ACTIVE_QUEUE_COUNT.IPS%d", core);
		sprintf(buffer_field, "MAX_ACTIVE_QUEUE_COUNT");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		printf("Core %d: Max Total Active Queues = %d\n", core, val[0]);
		
		sprintf(buffer_reg,"IPS_MAX_S_DQCQ_QUEUE_COUNT.IPS%d", core);
		sprintf(buffer_field, "MAX_S_DQCQ_QUEUE_COUNT");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		printf("\tMax SRAM Active Queues = %d\n", val[0]);
		
		sprintf(buffer_reg,"IPS_MAX_D_DQCQ_QUEUE_COUNT.IPS%d", core);
		sprintf(buffer_field, "MAX_D_DQCQ_QUEUE_COUNT");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		printf("\tMax DRAM Active Queues = %d\n", val[0]);
		
		sprintf(buffer_reg,"IPS_MAX_S_DQCQ_DRAM_QUEUE_COUNT.IPS%d", core);
		sprintf(buffer_field, "MAX_S_DQCQ_DRAM_QUEUE_COUNT");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		printf("\tMax SRAM-to-DRAM Active Queues = %d\n", val[0]);
	}
}

int get_max_active_queues(int unit, int core)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	sprintf(buffer_reg,"IPS_MAX_ACTIVE_QUEUE_COUNT.IPS%d", core);
	sprintf(buffer_field, "MAX_ACTIVE_QUEUE_COUNT");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	return val[0];
}

int print_max_dequeue_command(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int values[4];
	
	int multiplier;
		
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		sprintf(buffer_reg,"IPS_IPS_GENERAL_CONFIGURATIONS.IPS%d", core);
		sprintf(buffer_field, "DEQ_CMD_RES");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		multiplier = (val[0] == 0 ? 64 : 128);
		
		int i;
		for(i = 0; i < 4; i++)
		{
			sprintf(buffer_reg,"IPS_MAX_S_DEQ_COMMAND_CREDIT_CONFIG.IPS%d", core);
			sprintf(buffer_field, "MAX_SDQ_CMD_CRDT_%d", i);	
			diag_reg_field_get(unit, buffer_reg, buffer_field, val);
			
			values[i] = val[0] * multiplier;
		}
		
		printf("Core %d: Max Dequeue Command (Bytes)\n", core);
		printf("\t1   Active Queue :\t%d\n", values[0]);
		printf("\t2-3 Active Queues:\t%d\n", values[1]);
		printf("\t4-7 Active Queues:\t%d\n", values[2]);
		printf("\t8+  Active Queues:\t%d\n", values[3]);
	}
}

int print_deleted_credit_rate(int unit)
{
	double rate;
	int core;
	
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		rate = get_deleted_credit_rate(unit, core);
		printf("Core %d: Deleted Credit Rate = %4.2f Gbps\n", core, rate);
	}
}

int print_returned_credit_rate(int unit)
{
	double rate;
	int core;
	
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		rate = get_returned_credit_rate(unit, core);
		printf("Core %d: Returned Credit Rate = %4.2f Gbps\n", core, rate);
	}
}

double get_returned_credit_rate(int unit, int core) 
{
	double  nof_clocks;
	int     nof_clocks_int;
	double  clock_ratio;
	double  counter_result;  
	double  rate_in_gbps;
	
	// GTIMER Values
	double ips_device_freq_MHz = soc_property_get(unit ,"core_clock_speed_khz",-1) / 1000 ;

	nof_clocks     = ips_device_freq_MHz * 1000000 / 10; // 72,000,000 clocks --> 0.1 sec
	nof_clocks_int = nof_clocks;
	clock_ratio = nof_clocks / (ips_device_freq_MHz * 1000000);

	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	// Run the GTIMER
	ips_enable_gtimer(unit, core, nof_clocks_int);
	sal_usleep(200000); // Wait 0.2 sec
	
	// Get the counter result
	sprintf(buffer_reg,"IPS_RETURNED_CREDIT_COUNTER.IPS%d", core);
	sprintf(buffer_field, "RETURNED_CREDIT_COUNT");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	// Value is CREDITS !
	//printf("%d\n", val[0]);
	rate_in_gbps  = ips_calc_rate(unit, val[0]/clock_ratio);
	
	ips_disable_gtimer(unit, core);  

	return rate_in_gbps;
}

double get_deleted_credit_rate(int unit, int core) 
{
	double  nof_clocks;
	int     nof_clocks_int;
	double  clock_ratio;
	double  counter_result;  
	double  rate_in_gbps;
	
	// GTIMER Values
	double ips_device_freq_MHz = soc_property_get(unit ,"core_clock_speed_khz",-1) / 1000 ;

	nof_clocks     = ips_device_freq_MHz * 1000000 / 10; // 72,000,000 clocks --> 0.1 sec
	nof_clocks_int = nof_clocks;
	clock_ratio = nof_clocks / (ips_device_freq_MHz * 1000000);

	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	// Run the GTIMER
	ips_enable_gtimer(unit, core, nof_clocks_int);
	sal_usleep(200000); // Wait 0.2 sec
	
	// Get the counter result
	sprintf(buffer_reg,"IPS_DELETED_CREDIT_COUNTER.IPS%d", core);
	sprintf(buffer_field, "DELETED_CREDIT_COUNT");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	// Value is in BYTES !
	//printf("%d\n", val[0]);
	rate_in_gbps  = val[0] * 8 / clock_ratio; // Now value is in bps
	rate_in_gbps = rate_in_gbps / 1024 / 1024 / 1024; // Now value is in Mbps
	
	ips_disable_gtimer(unit, core);  

	return rate_in_gbps;
}

double ips_calc_rate (int unit, double num_of_credits) {
  int credit_size;
  double sch_rate;
  credit_size = ips_get_credit_size(unit);

  sch_rate = credit_size * 8 * num_of_credits / 1000000000; 
  
  return sch_rate;  
}

int ips_get_credit_size(int unit) {
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	sprintf(buffer_reg, "IPS_IPS_CREDIT_CONFIG");
	sprintf(buffer_field, "CREDIT_VALUE_0");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	return val[0];
}

void ips_enable_gtimer(int unit, int core, int nof_clocks_int){
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	val[0] = nof_clocks_int;
	sprintf(buffer_reg,"IPS_GTIMER_CONFIGURATION.IPS%d", core);
	sprintf(buffer_field, "GTIMER_CYCLE");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"IPS_GTIMER_CONFIGURATION.IPS%d", core);
	sprintf(buffer_field, "GTIMER_RESET_ON_TRIGGER");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"IPS_GTIMER_CONFIGURATION.IPS%d", core);
	sprintf(buffer_field, "GTIMER_ENABLE");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 0;
	sprintf(buffer_reg,"IPS_GTIMER_CONFIGURATION.IPS%d", core);
	sprintf(buffer_field, "GTIMER_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"IPS_GTIMER_CONFIGURATION.IPS%d", core);
	sprintf(buffer_field, "GTIMER_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
}

void ips_disable_gtimer(int unit, int core) {
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	val[0] = 1;
	sprintf(buffer_reg,"IPS_GTIMER_CONFIGURATION.IPS%d", core);
	sprintf(buffer_field, "GTIMER_RESET_ON_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 0;
	sprintf(buffer_reg,"IPS_GTIMER_CONFIGURATION.IPS%d", core);
	sprintf(buffer_field, "GTIMER_ENABLE");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
}

int get_qtype(int unit, int queue)
{
	reg_val val;
	char buffer_reg[128];
	sprintf(buffer_reg, "IPS_QTYPE");
	diag_mem_get(unit, buffer_reg, queue, val);
	val[0] = val[0] & 0x1F;
	
	return val[0];
}

void print_queue_status(int unit, int queue)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	// GTIMER Values
	double device_freq_MHz = soc_property_get(unit ,"core_clock_speed_khz",-1) / 1000 ;
	double nof_clocks     = device_freq_MHz * 10 * 1000; // 7,200,000 clocks --> 0.01 sec
	int nof_clocks_int = nof_clocks;
	double clock_ratio = nof_clocks / (device_freq_MHz * 1000 * 1000);
	
	// MASK
	val[0] = 0;
	sprintf(buffer_reg, "IPS_CPTRD_QUEUE_TRIGGER");
	sprintf(buffer_field, "QUEUE_NUM_FILTER_MASK");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);	
	
	// QUEUE
	val[0] = queue;
	sprintf(buffer_reg, "IPS_CPTRD_QUEUE_TRIGGER");
	sprintf(buffer_field, "QUEUE_NUM_FILTER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	ips_enable_gtimer(unit, 0, nof_clocks_int);
	sal_usleep(200000); // Wait 0.2 sec
	
	// QTYPE
	printf("Queue %d (QType = %d):\n", queue, get_qtype(unit, queue));
	
	// CREDIT RATE
	sprintf(buffer_reg,"IPS_FCR_CREDIT_COUNTER");
	sprintf(buffer_field, "FCR_CREDIT_COUNTER");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	// VALUE IS IN CREDITS !
	double rate_in_gbps;
	rate_in_gbps  = val[0] * 8 * ips_get_credit_size(unit) / clock_ratio; // Now value is in bps
	rate_in_gbps = rate_in_gbps / 1024 / 1024 / 1024; // Now value is in Gbps
	
	printf("\tCredit Rate:\t%3.2f Gbps\n", rate_in_gbps);
	
	// CRS NORM
	sprintf(buffer_reg, "IPS_TIME_IN_NORM");
	sprintf(buffer_field, "TIME_IN_NORM");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	printf("\tCRS Normal:\t%d%%\n", val[0] * 100 / nof_clocks_int);
	
	// CRS SLOW
	sprintf(buffer_reg, "IPS_TIME_IN_SLOW");
	sprintf(buffer_field, "TIME_IN_SLOW");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	printf("\tCRS Slow:\t%d%%\n", val[0] * 100 / nof_clocks_int);
	
	// CR BAL
	sprintf(buffer_reg, "IPS_MAX_CR_BAL");
	sprintf(buffer_field, "MAX_CR_BAL");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	sal_usleep(10000); // Wait 0.01 sec
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	val[0] = val[0] & 0x3FFFF;
	
	if(val[0] >> 17 & 1 == 1)
	{
		val[0] = (~val[0] & 0x3FFFF) + 1;
		printf("\tMax Credit Balance:\t-%dB\n", val[0]);
	}
	else
	{
		printf("\tMax Credit Balance:\t%dB\n", val[0]);
	}
	
	sprintf(buffer_reg, "IPS_MIN_CR_BAL");
	sprintf(buffer_field, "MIN_CR_BAL");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	sal_usleep(10000); // Wait 0.01 sec
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	val[0] = val[0] & 0x3FFFF;
	
	if(val[0] >> 17 & 1 == 1)
	{
		val[0] = (~val[0] & 0x3FFFF) + 1;
		printf("\tMin Credit Balance:\t-%dB\n", val[0]);
	}
	else
	{
		printf("\tMin Credit Balance:\t%dB\n", val[0]);
	}

	// Clean up
	ips_disable_gtimer(unit, 0); 
	
	val[0] = 0x3fff;
	sprintf(buffer_reg, "IPS_CPTRD_QUEUE_TRIGGER");
	sprintf(buffer_field, "QUEUE_NUM_FILTER_MASK");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);	
}

void print_all_queues_status(int unit)
{
	int i, queue;
	reg_val val;
	char buffer_reg[128];
	int port, devid;
	
	int my_modid;
	bcm_stk_modid_get(unit, my_modid);
	
	printf("Checking for active queues...\n");
	
	set_indirect_command_timeout(unit, 0, 0x7FFF);
	
	// Run over all EGQ Q Pairs to find active ports
	// Assume local switch (QAX)
	for(i = 0; i < 512; i++)
	{
		sprintf(buffer_reg, "EGQ_PQSMAX");
		diag_mem_get(unit, buffer_reg, i, val);
		if(val[0] != 0)
		{
			port = find_port_from_qpair(unit, i);
			if(port == -1) continue;
			
			print_port_queues(unit, port);
		}
	}
	
	set_indirect_command_timeout(unit, 0, 0x10);
}

int find_port_from_qpair(int unit, int qpair)
{
	reg_val val;
	char buffer_reg[128];
	int i;
	
	for(i = 0; i < 256; i++)
	{
		sprintf(buffer_reg, "EGQ_DSP_PTR_MAP");
		diag_mem_get(unit, buffer_reg, i, val);
		val[0] = val[0] & 0xFF;
		if(val[0] == qpair)
		{
			return i;
		}
	}
	
	return -1;
}

// Print the active queues of a given port
// Works only on local traffic flows
void print_port_queues(int unit, int port)
{
	int active_flow;
	int core;
	int tc;
	int base_flow;
	int flow;
	int queue;
	int first = 1;
	
	// Check both cores
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		base_flow = get_flow_id(unit, port, core);
		
		for(tc = 0; tc < 8; tc++)
		{
			flow = base_flow + tc;
			queue = get_base_queue(unit, port) + tc;
			
			if(get_qtype(unit, queue) == 15)
			{
				active_flow = 1; // Push queue
			}
			else
			{
				active_flow = check_if_flow_is_active(unit, core, flow);
			}
						
			if(active_flow != 0) 
			{
				if(first) printf("Port %d:\n", port);
				
				first = 0;
				print_queue_status(unit, queue);
			}
		}
		
		printf("\n");
	}
}

// Return positive if flow is active
// Return 0 if flow is NOT active
int check_if_flow_is_active(int unit, int core, int flowId) 
{
	int  counter_result;  

	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	// Set the DVS counter to count specific flow
	val[0] = flowId + (64 * 1024); // QAX Flow ID offset is +64k
	sprintf(buffer_reg,"SCH_CREDIT_COUNTER_CONFIGURATION_REG_1.SCH%d", core);
	sprintf(buffer_field, "FILTER_FLOW");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);

	val[0] = 1;
	sprintf(buffer_reg,"SCH_CREDIT_COUNTER_CONFIGURATION_REG_2.SCH%d", core);
	sprintf(buffer_field, "FILTER_BY_FLOW");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"SCH_CREDIT_COUNTER_CONFIGURATION_REG_2.SCH%d", core);
	sprintf(buffer_field, "FILTER_BY_SUB_FLOW");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 0;
	sprintf(buffer_reg,"SCH_CREDIT_COUNTER_CONFIGURATION_REG_2.SCH%d", core);
	sprintf(buffer_field, "FILTER_BY_DEST_FAP");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	// Get the counter result
	sprintf(buffer_reg,"SCH_DBG_CREDIT_COUNTER.SCH%d", core);
	diag_reg_get(unit, buffer_reg, val); 
	diag_reg_get(unit, buffer_reg, val); 
	
	counter_result    = val[0] & 0x7FFFFFFF; 
	
	// Reset DVS counter settings	
	val[0] = 0;
	sprintf(buffer_reg,"SCH_CREDIT_COUNTER_CONFIGURATION_REG_2.SCH%d", core);
	diag_reg_set(unit, buffer_reg, val);
	
	return counter_result;
}

// Print core total credit rate
void print_ips_credit_rate(int unit)
{
	int queue = 0;
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	// GTIMER Values
	double device_freq_MHz = soc_property_get(unit ,"core_clock_speed_khz",-1) / 1000 ;
	double nof_clocks     = device_freq_MHz * 10 * 1000; // 7,200,000 clocks --> 0.01 sec
	int nof_clocks_int = nof_clocks;
	double clock_ratio = nof_clocks / (device_freq_MHz * 1000 * 1000);

	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		ips_enable_gtimer(unit, 0, nof_clocks_int);
		sal_usleep(100 * 1000); 
			
		// CREDIT RATE
		sprintf(buffer_reg,"IPS_FCR_CREDIT_COUNTER.IPS%d", core);
		sprintf(buffer_field, "FCR_CREDIT_COUNTER");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		// VALUE IS IN CREDITS !
		double rate_in_gbps;
		rate_in_gbps  = val[0] * 8 * ips_get_credit_size(unit) / clock_ratio; // Now value is in bps
		rate_in_gbps = rate_in_gbps / 1024 / 1024;// Now value is in Gbps

		printf("\tCore %d: Total Credit Rate:\t%3.2f Mbps\n", core, rate_in_gbps);
	}
	// Clean up
	ips_disable_gtimer(unit, 0); 
}

int get_base_queue(int unit, int port)
{
	uint32 val[32];
	diag_mem_get(unit, "TAR_DESTINATION_TABLE", port, val); 
	return (val[0] & 0x7FFF);
}

int get_flow_id(int unit, int port, int core)
{
	char buffer_mem[128];
	sprintf(buffer_mem,"IPS_FLWID.IPS%d", core);
	
	int index = get_base_queue(unit, port) / 4;
	uint32 val[32];
	diag_mem_get(unit, buffer_mem, index, val); 
	return (val[0] & 0xFFFF) * 4;
}

int set_indirect_command_timeout(int unit, int core, int timeout)
{
	char buffer_reg[128];
	char buffer_field[128];
	reg_val val;
	
	sprintf(buffer_reg, "EGQ_INDIRECT_COMMAND.EGQ%d", core);
	sprintf(buffer_field, "INDIRECT_COMMAND_TIMEOUT");	
	val[0] = timeout;
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	sprintf(buffer_reg, "TAR_INDIRECT_COMMAND.TAR%d", core);
	sprintf(buffer_field, "INDIRECT_COMMAND_TIMEOUT");	
	val[0] = timeout;
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	sprintf(buffer_reg, "IPS_INDIRECT_COMMAND.IPS%d", core);
	sprintf(buffer_field, "INDIRECT_COMMAND_TIMEOUT");	
	val[0] = timeout;
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
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

print_functions();
print_all(0);