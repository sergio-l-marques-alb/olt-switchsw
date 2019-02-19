
// Jericho IPS Debug CINT

int print_functions()
{	
	printf("\n");
	printf("CINT diag_ips.c\n");
	printf("----------\n");
	printf("Functions:\n");
	printf("\t1. print_all(unit);\n");
	printf("\t2. print_max_active_queues(unit);\n");
	printf("\t3. print_deleted_credit_rate(unit);\n");
	printf("\t4. print_returned_credit_rate(unit);\n");
	printf("\t5. print_max_dequeue_command(unit);\n");
	printf("\t6. print_flow_control_status(unit);\n");
	printf("\t7. print_all_queues_status(unit);\n");
	printf("\t8. print_flow_control_indications(unit);\n");
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
	print_flow_control_status(unit);
	printf("\n");
	print_flow_control_indications(unit);
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
		
		printf("Core %d: Max Active Queues = %d\n", core, val[0]);
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
		sprintf(buffer_reg,"IPS_GLOBAL_GENERAL_CFG_4.IPS%d", core);
		sprintf(buffer_field, "IPS_DEQ_CMD_RES");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		multiplier = (val[0] == 0 ? 64 : 128);
		
		int i;
		for(i = 0; i < 4; i++)
		{
			sprintf(buffer_reg,"IPS_MAX_DEQ_COMMAND_CREDIT_CONFIG.IPS%d", core);
			sprintf(buffer_field, "MAX_DQ_CMD_CRDT_%d", i);	
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

int print_flow_control_status(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	double percentage;
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		val[0] = 144; // TOTAL FC
		sprintf(buffer_reg,"IPS_FLOW_CONTROL_COUNT_SELECT.IPS%d", core);
		sprintf(buffer_field, "FC_COUNT_SEL");	
		diag_reg_field_set(unit, buffer_reg, buffer_field, val);
		
		percentage = get_flow_control_percentage(unit, core);
		if(percentage == 0)
		{
			printf("IPS(%d) is not under FC\n", core);
		}
		else
		{
			printf("IPS(%d) is under FC %4.2f%% of the time\n", core, percentage);
		}
	}
}

double get_flow_control_percentage(int unit, int core) 
{
	double  nof_clocks;
	int     nof_clocks_int;
	double  clock_ratio;
	double  counter_result;  
	double  rate_in_gbps;
	
	// GTIMER Values
	double ips_device_freq_MHz = soc_property_get(0 ,"core_clock_speed_khz",-1) / 1000 ;

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
	sprintf(buffer_reg,"IPS_FLOW_CONTROL_COUNTER.IPS%d", core);
	sprintf(buffer_field, "FC_COUNT");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);

	double percentage = val[0] * 100;
	percentage = percentage / nof_clocks;
	
	ips_disable_gtimer(unit, core);  

	return percentage;
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
	double ips_device_freq_MHz = soc_property_get(0 ,"core_clock_speed_khz",-1) / 1000 ;

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
	double ips_device_freq_MHz = soc_property_get(0 ,"core_clock_speed_khz",-1) / 1000 ;

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
	
	sprintf(buffer_reg, "IPST_CREDIT_CONFIG_1");
	sprintf(buffer_field, "CREDIT_VALUE_0");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	return val[0];
}

int get_qtype(int unit, int queue, int core)
{
	reg_val val;
	char buffer_reg[128];
	sprintf(buffer_reg, "IPS_QTYPE.IPS%d", core);
	diag_mem_get(unit, buffer_reg, queue, val);
	val[0] = val[0] & 0x1F;
	
	return val[0];
}

void print_queue_status(int unit, int core, int queue)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	// GTIMER Values
	double device_freq_MHz = soc_property_get(unit ,"core_clock_speed_khz",-1) / 1000 ;
	double nof_clocks     = device_freq_MHz * 10 * 1000; // 7,200,000 clocks --> 0.001 sec
	int nof_clocks_int = nof_clocks;
	double clock_ratio = nof_clocks / (device_freq_MHz * 1000 * 1000);
	
	// MASK
	val[0] = 0;
	sprintf(buffer_reg, "IPS_QUEUE_NUM_FILTER_MASK.IPS%d", core);
	sprintf(buffer_field, "QUEUE_NUM_FILTER_MASK");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);	
	
	// QUEUE
	val[0] = queue;
	sprintf(buffer_reg, "IPS_QUEUE_NUM_FILTER.IPS%d", core);
	sprintf(buffer_field, "QUEUE_NUM_FILTER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	ips_enable_gtimer(unit, core, nof_clocks_int);
	sal_usleep(200000); // Wait 0.2 sec
	
	// QTYPE
	printf("Queue %d (QType = %d):\n", queue, get_qtype(unit, queue, core));
	
	// CREDIT RATE
	sprintf(buffer_reg,"IPS_FCR_CREDIT_COUNTER.IPS%d", core);
	sprintf(buffer_field, "FCR_CREDIT_COUNTER");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	// VALUE IS IN CREDITS !
	double rate_in_gbps = val[0];
	rate_in_gbps  = rate_in_gbps * 8 * ips_get_credit_size(unit) / clock_ratio; // Now value is in bps
	rate_in_gbps = rate_in_gbps / 1024 / 1024 / 1024; // Now value is in Gbps
	
	printf("\tCredit Rate:\t%3.2f Gbps\n", rate_in_gbps);
	
	// CRS NORM
	sprintf(buffer_reg, "IPS_TIME_IN_NORM.IPS%d", core);
	sprintf(buffer_field, "TIME_IN_NORM");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	printf("\tCRS Normal:\t%d%%\n", val[0] * 100 / nof_clocks_int);
	
	// CRS SLOW
	sprintf(buffer_reg, "IPS_TIME_IN_SLOW.IPS%d", core);
	sprintf(buffer_field, "TIME_IN_SLOW");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	printf("\tCRS Slow:\t%d%%\n", val[0] * 100 / nof_clocks_int);
	
	printf("\tMax Queue Size:\t%dB\n", get_queue_max_size(unit, core, queue));
	
	// CR BAL
	sprintf(buffer_reg, "IPS_MAX_CR_BAL.IPS%d", core);
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
	
	sprintf(buffer_reg, "IPS_MIN_CR_BAL.IPS%d", core);
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
	
	// Get the counter result
	sprintf(buffer_reg,"IPS_RETURNED_CREDIT_COUNTER.IPS%d", core);
	sprintf(buffer_field, "RETURNED_CREDIT_COUNT");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	// Value is CREDITS !
	rate_in_gbps  = ips_calc_rate(unit, val[0]/clock_ratio);
	printf("\tReturned Credits:\t%3.2f Gbps\n", rate_in_gbps);
	
	// Get the counter result
	sprintf(buffer_reg,"IPS_DELETED_CREDIT_COUNTER.IPS%d", core);
	sprintf(buffer_field, "DELETED_CREDIT_COUNT");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	// Value is in BYTES !
	rate_in_gbps  = val[0] * 8 / clock_ratio; // Now value is in bps
	rate_in_gbps = rate_in_gbps / 1024 / 1024 / 1024; // Now value is in Mbps
	printf("\tDeleted Credits:\t%3.2f Gbps\n", rate_in_gbps);
	
	// Clean up
	ips_disable_gtimer(unit, core); 
	
	val[0] = 0x1ffff;
	sprintf(buffer_reg, "IPS_QUEUE_NUM_FILTER_MASK.IPS%d", core);
	sprintf(buffer_field, "QUEUE_NUM_FILTER_MASK");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);	
}

int get_queue_max_size(int unit, int core, int queue)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int queue_size, first_queue_size;
	
	// Clear counter mask (count specific queue)
	sprintf(buffer_reg, "IQM_PROGRAMMABLE_COUNTER_QUEUE_SELECT.IQM%d", core);
	sprintf(buffer_field, "PRG_CNT_MSK");	
	val[0] = 0;
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	// Set specific queue for counter
	sprintf(buffer_reg, "IQM_PROGRAMMABLE_COUNTER_QUEUE_SELECT.IQM%d", core);
	sprintf(buffer_field, "PRG_CNT_Q");	
	val[0] = queue;
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);

	// Clear queue max size
	sprintf(buffer_reg, "IQM_QUEUE_MAXIMUM_OCCUPANCY_QUEUE_SIZE.IQM%d", core);
	sprintf(buffer_field, "Q_MX_OC_QSZ");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	first_queue_size = val[0];
	
	// Get queue max size
	sal_usleep(10 * 1000);
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	queue_size = val[0];
	
	// TODO: Check if queue is stuck
	
	// Restore counter mask (count all queues)
	sprintf(buffer_reg, "IQM_PROGRAMMABLE_COUNTER_QUEUE_SELECT.IQM%d", core);
	sprintf(buffer_field, "PRG_CNT_MSK");	
	val[0] = 0x17fff;
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	return queue_size * 16;
}

void print_all_queues_status(int unit)
{
	int i, j, queue;
	reg_val val;
	char buffer_reg[128];
	int size;
		
	printf("Checking for active queues...\n");
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		printf("Active Queues Core %d:\n", core);
		
		set_indirect_command_timeout(unit, core, 0x7FFF);

		for(queue = 0; queue < 4; queue++)
		{
			size = get_queue_max_size(unit, core, queue);
			if(size != 0)
			{
				print_queue_status(unit, core, queue);
			}
		}
		
		sprintf(buffer_reg, "IPS_FLWID.IPS%d", core);
		for(i = 0; i < 24576; i++)
		{
			diag_mem_get(unit, buffer_reg, i, val);
			if((val[0] & 0xFFFF) != 0x3FFF)
			{
				queue = i * 4;
				for(j = 0; j < 4; j++)
				{
					size = get_queue_max_size(unit, core, queue + j);
					if(size != 0)
					{
						print_queue_status(unit, core, queue + j);
						printf("\n");
					}
				}
			}
		}
		
		set_indirect_command_timeout(unit, core, 0x10);
	}
}

void print_flow_control_indications(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int core;
	
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		printf("IPS Flow Control Indications - Core %d:\n", core);
		
		sprintf(buffer_reg, "IPS_FLOW_CONTROL_INDICATION_1.IPS%d", core);
		sprintf(buffer_field, "FLOW_CONTROL_IPT_BDQ");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		if(val[0] != 0)
		{
			printf("\tIPT BDQ --> DQCQ: 0x%05X (BDQs bitmap)\n", val[0]);
		}
		
		sprintf(buffer_reg, "IPS_FLOW_CONTROL_INDICATION_1.IPS%d", core);
		sprintf(buffer_field, "FLOW_CONTROL_IPT_EIR");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		if(val[0] != 0)
		{
			printf("\tIPT BDQ --> IPSP EIR: 0x%05X (BDQs bitmap)\n", val[0]);
		}
		
		sprintf(buffer_reg, "IPS_FLOW_CONTROL_INDICATION_1.IPS%d", core);
		sprintf(buffer_field, "FLOW_CONTROL_IPT_FMC");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		if(val[0] != 0)
		{
			printf("\tIPT --> FMC: 0x%X (bitmap - GFMC, BFMC 2/1/0)\n", val[0]);
		}
		
		sprintf(buffer_reg, "IPS_FLOW_CONTROL_INDICATION_1.IPS%d", core);
		sprintf(buffer_field, "FLOW_CONTROL_IPS_FMC");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		if(val[0] != 0)
		{
			printf("\tDQCQ FMC --> FMC: 0x%X (bitmap - Hi/Lo, OCB/Mix)\n", val[0]);
		}
		
		sprintf(buffer_reg, "IPS_FLOW_CONTROL_INDICATION_1.IPS%d", core);
		sprintf(buffer_field, "FLOW_CONTROL_IPS_IS");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		if(val[0] != 0)
		{
			printf("\tDQCQ IS --> Ingress Shaping: %d\n", val[0]);
		}
		
		sprintf(buffer_reg, "IPS_FLOW_CONTROL_INDICATION_1.IPS%d", core);
		sprintf(buffer_field, "FLOW_CONTROL_IPS_LP");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		if(val[0] != 0)
		{
			printf("\tLP DQCQ (internal FC): %d\n", val[0]);
		}
		
		sprintf(buffer_reg, "IPS_FLOW_CONTROL_INDICATION_1.IPS%d", core);
		sprintf(buffer_field, "FLOW_CONTROL_IQM_DEL");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		if(val[0] != 0)
		{
			printf("\tIQM --> Delete DQCQ: %d\n", val[0]);
		}
		
		sprintf(buffer_reg, "IPS_FLOW_CONTROL_INDICATION_1.IPS%d", core);
		sprintf(buffer_field, "FLOW_CONTROL_IRR_IS");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		if(val[0] != 0)
		{
			printf("\tIRR --> Ingress Shaping: %d\n", val[0]);
		}
	
	}
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

int set_indirect_command_timeout(int unit, int core, int timeout)
{
	char buffer_reg[128];
	char buffer_field[128];
	reg_val val;
	
	sprintf(buffer_reg, "EGQ_INDIRECT_COMMAND.EGQ%d", core);
	sprintf(buffer_field, "INDIRECT_COMMAND_TIMEOUT");	
	val[0] = timeout;
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	sprintf(buffer_reg, "IRR_INDIRECT_COMMAND");
	sprintf(buffer_field, "INDIRECT_COMMAND_TIMEOUT");	
	val[0] = timeout;
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	sprintf(buffer_reg, "IPS_INDIRECT_COMMAND.IPS%d", core);
	sprintf(buffer_field, "INDIRECT_COMMAND_TIMEOUT");	
	val[0] = timeout;
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
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
	sprintf(buffer_reg,"IPS_GTIMER_TRIGGER.IPS%d", core);
	sprintf(buffer_field, "GTIMER_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"IPS_GTIMER_TRIGGER.IPS%d", core);
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


print_functions();
print_all(0);