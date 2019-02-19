
// QAX SCH Debug CINT

int print_only_active_flows = 0; // Print only flows with rate > 0

int print_functions()
{	
	printf("\n");
	printf("CINT diag_sch.c\n");
	printf("----------\n");
	printf("Functions:\n");
	printf("\t1. print_all_flows(unit);\n");
	printf("\t2. print_port_flows(unit, port);\n");
	printf("\t3. set_sch_drm_table(unit);       // Configure DRM table with recommended values\n");
	printf("\t4. print_sch_drm_table(unit);\n");
	printf("\t5. print_rebound_rate(unit);\n");
	printf("\n");
}

void print_all(int unit)
{
	print_sch_drm_table(unit);
	printf("\n");
	print_all_flows(unit);
	printf("\n");
	print_rebound_rate(unit);
}

// Set the indirect timeout for better results under traffic
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

// Print all active flows
void print_all_flows(int unit)
{
	int i, queue;
	reg_val val;
	char buffer_reg[128];
	int port, devid;
	
	int my_modid;
	bcm_stk_modid_get(unit, my_modid);
	
	printf("Checking for active ports/flows...\n");
	
	set_indirect_command_timeout(unit, 0, 0x7FFF);
	
	// Run over all system ports
	print_only_active_flows = 1;
	for(i = 0; i < 32768; i++)
	{
		sprintf(buffer_reg, "TAR_DESTINATION_TABLE");
		diag_mem_get(unit, buffer_reg, i, val);
		if((val[0] & 0x7FFF) != 0x7FFF)
		{
			// Found a configured system port
			// Need to get the local port for this device
			sprintf(buffer_reg, "IPS_QPM_2_SYS_RED");
			diag_mem_get(unit, buffer_reg, i, val);
			devid = val[0] & 0x7FF;				// Destination Device
			port = (val[0] & 0x7F800) >> 11;	// Local Port ID
			
			if(devid == my_modid || devid == (my_modid + 1))
			{
				// Port is local
				print_port_flows(unit, port);
			}
		}
	}
	print_only_active_flows = 0;
	
	set_indirect_command_timeout(unit, 0, 0x10);
}

// Print the active flows of a given port
// Works only on local traffic flows
void print_port_flows(int unit, int port)
{
	double rate;
	int core, core2;
	int tc;
	int base_flow;
	int flow;
	
	// Check both cores
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		rate = get_port_rate(unit, core, port);
		if(rate == 0 && print_only_active_flows) continue;
		
		printf("Core %d: Port %d: \t%4.2f Gbps\n", core, port, rate);
		
		// Check flows if port is active
		if(rate != 0)
		{
			// Need to check flows from each core
			for(core2 = 0; core2 < get_number_of_cores(unit); core2++)
			{
				base_flow = get_flow_id(unit, port, core2);
				
				for(tc = 0; tc < 8; tc++)
				{
					flow = base_flow + tc;
					rate = get_flow_rate(unit, core, flow);
					if(rate != 0)
					{
						printf("\tFlow %d:\t%4.2f Gbps\n", flow, rate);
					}
				}
			}
		}
		printf("\n");
	}
}

// Get credit rate of a given port
double get_port_rate(int unit, int core, int port) 
{
	double  nof_clocks;
	int     nof_clocks_int;
	double  clock_ratio;
	double  counter_result;  
	double  sch_rate_in_gbps;
	
	int base_flow = get_flow_id(unit, port, core);
	
	int ps = get_port_base_q_pair(unit, core, port);
	
	// GTIMER Values
	double sch_device_freq_MHz = soc_property_get(unit ,"core_clock_speed_khz",-1) / 1000 ;
	nof_clocks     = sch_device_freq_MHz * 1000000 / 10; // 72,000,000 clocks --> 0.1 sec
	nof_clocks_int = nof_clocks;
	clock_ratio = nof_clocks / (sch_device_freq_MHz * 1000000);

	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	// Set the DVS counter to count specific port
	val[0] = ps;
	sprintf(buffer_reg,"SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATION.SCH%d", core);
	sprintf(buffer_field, "DVS_FILTER_PORT");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);

	val[0] = 1;
	sprintf(buffer_reg,"SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATION.SCH%d", core);
	sprintf(buffer_field, "CNT_BY_PORT");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	// Run the GTIMER
	sch_enable_gtimer(unit, core, nof_clocks_int);
	sal_usleep(200000); // Wait 0.2 sec
	
	// Get the counter result
	sprintf(buffer_reg,"SCH_DVS_CREDIT_COUNTER.SCH%d", core);
	diag_reg_get(unit, buffer_reg, val); 
	
	counter_result    = val[0] & 0x7FFFFFFF; 
	sch_rate_in_gbps  = sch_calc_rate(unit, counter_result/clock_ratio);
	
	// Reset DVS counter settings
	val[0] = 0;
	sprintf(buffer_reg,"SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATION.SCH%d", core);
	diag_reg_set(unit, buffer_reg, val);
	
	sch_disable_gtimer(unit, core);  

	return sch_rate_in_gbps;
}

// Get credit rate of a given flow
double get_flow_rate(int unit, int core, int flowId) 
{
	double  nof_clocks;
	int     nof_clocks_int;
	double  clock_ratio;
	double  counter_result;  
	double  sch_rate_in_gbps;

	// GTIMER Values
	double sch_device_freq_MHz = soc_property_get(unit ,"core_clock_speed_khz",-1) / 1000 ;
	nof_clocks     = sch_device_freq_MHz * 1000000 / 10 / 2; // 72,000,000 clocks --> 0.05 sec
	nof_clocks_int = nof_clocks;
	clock_ratio = nof_clocks / (sch_device_freq_MHz * 1000000);

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
	
	// Run the GTIMER
	sch_enable_gtimer(unit, core, nof_clocks_int);
	sal_usleep(100000); // Wait 0.1 sec
	
	// Get the counter result
	sprintf(buffer_reg,"SCH_DBG_CREDIT_COUNTER.SCH%d", core);
	diag_reg_get(unit, buffer_reg, val); 
	
	counter_result    = val[0] & 0x7FFFFFFF; 
	sch_rate_in_gbps  = sch_calc_rate(unit, counter_result/clock_ratio);
	
	// Reset DVS counter settings	
	val[0] = 0;
	sprintf(buffer_reg,"SCH_CREDIT_COUNTER_CONFIGURATION_REG_2.SCH%d", core);
	diag_reg_set(unit, buffer_reg, val);
	
	sch_disable_gtimer(unit, core);  
	
	//printf("Flow%5d Rate= %4.3fGbps\n", flowId, sch_rate_in_gbps );      
	
	return sch_rate_in_gbps;
}

// Get total rebound credit rate
double print_rebound_rate(int unit) 
{
	double  nof_clocks;
	int     nof_clocks_int;
	double  clock_ratio;
	double  counter_result;  
	double  sch_rate_in_gbps;

	double stl_rate_in_gbps;
	double scl_rate_in_gbps;
	
	// GTIMER Values
	double sch_device_freq_MHz = soc_property_get(unit ,"core_clock_speed_khz",-1) / 1000 ;
	nof_clocks     = sch_device_freq_MHz * 1000000 / 10 / 2; // 72,000,000 clocks --> 0.05 sec
	nof_clocks_int = nof_clocks;
	clock_ratio = nof_clocks / (sch_device_freq_MHz * 1000000);

	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];

	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		stl_rate_in_gbps = 0;
		scl_rate_in_gbps = 0;

		val[0] = 1;
		sprintf(buffer_reg,"SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATION.SCH%d", core);
		sprintf(buffer_field, "CNT_BY_PORT");	
		diag_reg_field_set(unit, buffer_reg, buffer_field, val);
		
		int i;
		for(i = 0; i < 256; i++)
		{
			sprintf(buffer_reg,"SCH_HR_SCHEDULER_CONFIGURATION_SHC.SCH%d", core);
			diag_mem_get(unit, buffer_reg, i, val);
			if(val[0] == 0) continue;
			
			// Set the DVS counter to count STL rebound
			val[0] = 1;
			sprintf(buffer_reg,"SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATION.SCH%d", core);
			sprintf(buffer_field, "FIELD_24_24");	
			diag_reg_field_set(unit, buffer_reg, buffer_field, val);
			
			val[0] = i;
			sprintf(buffer_reg,"SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATION.SCH%d", core);
			sprintf(buffer_field, "DVS_FILTER_PORT");	
			diag_reg_field_set(unit, buffer_reg, buffer_field, val);
			
			// Run the GTIMER
			sch_enable_gtimer(unit, core, nof_clocks_int);
			sal_usleep(100000); // Wait 0.1 sec
			
			// Get the counter result
			sprintf(buffer_reg,"SCH_DVS_CREDIT_COUNTER.SCH%d", core);
			diag_reg_get(unit, buffer_reg, val); 
			
			counter_result    = val[0] & 0x7FFFFFFF; 
			sch_rate_in_gbps  = sch_calc_rate(unit, counter_result/clock_ratio);
			
			stl_rate_in_gbps += sch_rate_in_gbps;
			
			// Reset DVS counter settings	
			val[0] = 0;
			sprintf(buffer_reg,"SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATION.SCH%d", core);
			sprintf(buffer_field, "FIELD_24_24");	
			diag_reg_field_set(unit, buffer_reg, buffer_field, val);
			
			sch_disable_gtimer(unit, core);  

			// Set the DVS counter to count SCL rebound
			val[0] = 1;
			sprintf(buffer_reg,"SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATION.SCH%d", core);
			sprintf(buffer_field, "FIELD_25_25");	
			diag_reg_field_set(unit, buffer_reg, buffer_field, val);

			// Run the GTIMER
			sch_enable_gtimer(unit, core, nof_clocks_int);
			sal_usleep(100000); // Wait 0.1 sec
			
			// Get the counter result
			sprintf(buffer_reg,"SCH_DVS_CREDIT_COUNTER.SCH%d", core);
			diag_reg_get(unit, buffer_reg, val); 
			
			counter_result    = val[0] & 0x7FFFFFFF; 
			sch_rate_in_gbps  = sch_calc_rate(unit, counter_result/clock_ratio);
			
			scl_rate_in_gbps += sch_rate_in_gbps;
			
			// Reset DVS counter settings	
			val[0] = 0;
			sprintf(buffer_reg,"SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATION.SCH%d", core);
			sprintf(buffer_field, "FIELD_25_25");	
			diag_reg_field_set(unit, buffer_reg, buffer_field, val);
			
			sch_disable_gtimer(unit, core);  
		}			
	
		printf("Core %d: STL Rebound Rate: %4.3f Gbps\n", core, stl_rate_in_gbps );
		printf("Core %d: SCL Rebound Rate: %4.3f Gbps\n", core, scl_rate_in_gbps );
		
		val[0] = 0;
		sprintf(buffer_reg,"SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATION.SCH%d", core);
		sprintf(buffer_field, "CNT_BY_PORT");	
		diag_reg_field_set(unit, buffer_reg, buffer_field, val);

	}
	
	return sch_rate_in_gbps;
}

int get_port_base_q_pair(int unit, int core, int port)
{
	reg_val val;
	char buffer_reg[128];
	
	sprintf(buffer_reg, "EGQ_DSP_PTR_MAP.EGQ%d", core);
	diag_mem_get(unit, buffer_reg, port, val);
	
	return val[0] & 0xFF;
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
  
double sch_calc_rate (int unit, double num_of_credits) {
  int credit_size;
  double sch_rate;
  credit_size = sch_get_credit_size(unit);
  sch_rate = credit_size * 8 * num_of_credits/1000000000; 
  
  return sch_rate;  
}

int sch_get_credit_size(int unit) {
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	sprintf(buffer_reg, "IPS_IPS_CREDIT_CONFIG");
	sprintf(buffer_field, "CREDIT_VALUE_0");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	return val[0];
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
	sprintf(buffer_reg,"SCH_GTIMER_CONFIGURATION.SCH%d", core);
	sprintf(buffer_field, "GTIMER_TRIGGER");
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	val[0] = 1;
	sprintf(buffer_reg,"SCH_GTIMER_CONFIGURATION.SCH%d", core);
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

void print_sch_drm_table(int unit)
{
	int links;
	int i;
	int rv;
	
	printf("Shared DRM Table:\n");
	
	printf("\t\t");
	for(i = 0; i < 8; i++)
	{
		printf("RCI %d\t", i);
	}
	printf("\n");
	
	for(links = 0; links < 1; links++) // No links on QAX...
	{
		bcm_fabric_bandwidth_profile_t profiles[8];
		
		for(i = 0; i < 8; i++)
		{
			profiles[i].rci = i;
			profiles[i].num_links = links;
		}
	
		rv = bcm_fabric_bandwidth_profile_get(unit, 8, profiles);
		if(rv != 0) break;

		if(links == 0)
		{
			printf("Local Switch:");
		}
		else
		{
			printf("Links %d:", links);
		}
		
		for(i = 0; i < 8; i++)
		{
			profiles[i].max_kbps = profiles[i].max_kbps / 1024 / 1024; // Use Gbps
			printf("\t%d", profiles[i].max_kbps);
		}
		printf("\n");
	}
}

void set_sch_drm_table(int unit)
{
	bcm_fabric_bandwidth_profile_t profiles[288];
	double rci_factor[8] = {1, 0.75, 0.60, 0.50, 0.40, 0.35, 0.25, 0.15};
	
	int link_speed;
	bcm_port_speed_get(0, 256, &link_speed);
	
	int links, rci, x;
	x = 0;
	for(links = 1; links < 37; links++)
	{
		for(rci = 0; rci < 8; rci++)
		{
			profiles[x].rci = rci;
			profiles[x].num_links = links;
			profiles[x].max_kbps = link_speed * 1.1 * rci_factor[rci] * links * 1024;
			x++;
		}
	}
	
	bcm_fabric_bandwidth_core_profile_set(unit, 0, 0, 288, profiles); 	// Core 0
	bcm_fabric_bandwidth_core_profile_set(unit, 1, 0, 288, profiles); 	// Core 1
	bcm_fabric_bandwidth_profile_set(unit, 288, profiles);				// Shared
	
	
	// Local Switch
	x = 0;
	for(rci = 0; rci < 8; rci++)
	{
		profiles[x].rci = rci;
		profiles[x].num_links = 0;
		profiles[x].max_kbps = 1.4 * 1024 * 1024 * 1024; // 1.4T
		x++;
	}
	
	bcm_fabric_bandwidth_core_profile_set(unit, 0, 0, 8, profiles); 	// Core 0
	bcm_fabric_bandwidth_core_profile_set(unit, 1, 0, 8, profiles); 	// Core 1
	bcm_fabric_bandwidth_profile_set(unit, 8, profiles);				// Shared
}

int se_to_flow_id(int se_id) {
	int flow_id = 0;
	
	if (se_id >= 0 && se_id <= 16383) // CL
	{
		flow_id = se_id * 4;
	}
	
	if (se_id >= 16384 && se_id <= 32767) // FQ and HR
	{
		flow_id = se_id - (16 * 1024);
		flow_id = flow_id * 4;
		flow_id = flow_id + 1;
	}
		
	flow_id += (64 * 1024);
	
	return flow_id;
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
