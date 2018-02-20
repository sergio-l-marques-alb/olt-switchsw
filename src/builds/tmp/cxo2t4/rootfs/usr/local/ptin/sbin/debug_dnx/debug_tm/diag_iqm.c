
// Jericho IQM Debug CINT

int total_ocb_buffers[2];
int total_fmc_buffers[2];
int total_mnmc_buffers[2];
int total_bdbs[2];
int total_bds[2];

int print_functions()
{	
	printf("\n");
	printf("CINT diag_iqm.c\n");
	printf("----------\n");
	printf("Functions:\n");
	printf("\t1. print_all(unit);\n");
	printf("\t2. print_totals(unit);\n");
	printf("\t3. print_free_buffers(unit);\n");
	printf("\t4. print_active_queues_status(unit);\n");
	printf("\t5. print_bd_reject_thresholds(unit);\n");
	printf("\t6. set_bd_reject_thresholds(unit);\n");
	printf("\t7. set_udpate_fifo_th(unit);\n");
	printf("\n");
}

int print_all(int unit)
{
	print_totals(unit);
	printf("\n");
	print_bd_reject_thresholds(unit);
	printf("\n");
	print_free_buffers(unit);
	printf("\n");
	print_update_fifo_th(unit);
	printf("\n");
	
	if(get_max_active_queues(unit, 0) < 30)
	{
		print_active_queues_status(unit);
	}
	else
	{
		printf("Skipped printing Active Queues\n");
		printf("You can run it manually by calling: print_active_queues_status(unit);\n");
	}
	
	printf("\n");
	print_reject_reason(unit);
	printf("\n");
}

// Get total resources values
int read_totals(int unit)
{
	reg_val val, val2;
	char buffer_reg[128];
	char buffer_field[128];
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		sprintf(buffer_reg, "ECI_GLOBAL_OCB_BUF_RANGE_%d", core);
		sprintf(buffer_field, "OCB_%d_BUFF_PTR_START", core);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		sprintf(buffer_reg, "ECI_GLOBAL_OCB_BUF_RANGE_%d", core);
		sprintf(buffer_field, "OCB_%d_BUFF_PTR_END", core);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val2);
		
		total_ocb_buffers[core] = val2[0] - val[0] + 1;
		
		sprintf(buffer_reg, "ECI_GLOBAL_FULL_MC_DB_RANGE_%d", core);
		sprintf(buffer_field, "FULL_MULTICAST_%d_DB_PTR_START", core);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		sprintf(buffer_reg, "ECI_GLOBAL_FULL_MC_DB_RANGE_%d", core);
		sprintf(buffer_field, "FULL_MULTICAST_%d_DB_PTR_END", core);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val2);
		
		total_fmc_buffers[core] = val2[0] - val[0] + 1;
		
		sprintf(buffer_reg, "ECI_GLOBAL_MINI_MC_DB_RANGE_%d", core);
		sprintf(buffer_field, "MINI_MULTICAST_%d_DB_PTR_START", core);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		sprintf(buffer_reg, "ECI_GLOBAL_MINI_MC_DB_RANGE_%d", core);
		sprintf(buffer_field, "MINI_MULTICAST_%d_DB_PTR_END", core);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val2);
		
		total_mnmc_buffers[core] = val2[0] - val[0] + 1;
	}
	
	read_bds_total(unit);
	read_bdbs_total(unit);
}

// Calculate total BDBs based on bank size and selection
int read_bds_total(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	int bank_select;
	
	total_bds[0] = 0;
	total_bds[1] = 0;
	
	sprintf(buffer_reg, "IQMT_BANK_SELECT");
	sprintf(buffer_field, "PDM_BANK_SELECT");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	bank_select = val[0];
	
	int bank;
	int core;
	for(bank = 0; bank < 6; bank++)
	{
		core = ((bank_select >> bank) & 1); // Core from bank select bits
		
		total_bds[core] += (512 * 1024); // Bank size is in 512k resolution
	}
}

// Calculate total BDBs based on bank size and selection
int read_bdbs_total(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	int bank_select;
	
	total_bdbs[0] = 0;
	total_bdbs[1] = 0;
	
	sprintf(buffer_reg, "IQMT_BANK_SELECT");
	sprintf(buffer_field, "BDB_BANK_SELECT");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	bank_select = val[0];
	
	int bank;
	int core;
	for(bank = 0; bank < 6; bank++)
	{
		core = ((bank_select >> bank) & 1); // Core from bank select bits
		
		sprintf(buffer_reg, "IQMT_BDBLL_BANK_SIZES");
		sprintf(buffer_field, "BDB_LIST_SIZE_%d", bank);

		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		total_bdbs[core] += (val[0] * 8192); // Bank size is in 8k resolution
	}
}

int print_totals(int unit)
{
	read_totals(unit);
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		printf("Total Resources for Core %d:\n", core);
		printf("\tTotal OCB  Buffers: %d\n", total_ocb_buffers[core]);
		printf("\tTotal FMC  Buffers: %d\n", total_fmc_buffers[core]);
		printf("\tTotal MNMC Buffers: %d\n", total_mnmc_buffers[core]);
		printf("\tTotal BDBs Buffers: %d\n", total_bdbs[core]);
		printf("\tTotal BDs  Buffers: %d\n", total_bds[core]);
	}
}

// Print BD Reject Thresholds (IQM_GENERAL_REJECT_CONFIGURATION_B_DS)
int print_bd_reject_thresholds(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	int real_value;
	
	int dp;
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		printf("Reject Thresholds for Core %d:\n", core);
		
		for(dp = 0; dp < 4; dp++)
		{
			sprintf(buffer_reg, "IQM_GENERAL_REJECT_CONFIGURATION_B_DS.IQM%d", core);
			sprintf(buffer_field, "OCB_RJCT_SET_OC_BD_TH_%d", dp);
			diag_reg_field_get(unit, buffer_reg, buffer_field, val);
			
			real_value = float_to_int(val[0], 8);
			printf("\tOCB Reject DP %d: \t%d (%d%%)\n", dp, real_value, real_value * 100 / total_bds[core]);
		}
		
		for(dp = 0; dp < 4; dp++)
		{
			sprintf(buffer_reg, "IQM_GENERAL_REJECT_CONFIGURATION_B_DS.IQM%d", core);
			sprintf(buffer_field, "MIXED_RJCT_SET_OC_BD_TH_%d", dp);
			diag_reg_field_get(unit, buffer_reg, buffer_field, val);
			
			real_value = float_to_int(val[0], 8);
			
			printf("\tMIXED Reject DP %d: \t%d (%d%%)\n", dp, real_value, real_value * 100 / total_bds[core]);
		}
	}
}

// Set Recommended BD Reject Thresholds (IQM_GENERAL_REJECT_CONFIGURATION_B_DS)
int set_bd_reject_thresholds(int unit)
{
	reg_val val;
	uint32 threshold;
	
	char buffer_reg[128];
	char buffer_field[128];
	int real_value;
	
	int ocb_percentage[4] = {99, 98, 97, 97};
	int mix_percentage[4] = {96, 95, 94, 94};
	
	int dp;
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{	
		for(dp = 0; dp < 4; dp++)
		{
			threshold = ocb_percentage[dp] * total_bds[core] / 100;
			val[0] = convert_value_to_float(threshold, 8, 4);

			sprintf(buffer_reg, "IQM_GENERAL_REJECT_CONFIGURATION_B_DS.IQM%d", core);
			sprintf(buffer_field, "OCB_RJCT_SET_OC_BD_TH_%d", dp);
						
			diag_reg_field_set(unit, buffer_reg, buffer_field, val);			
		}
		
		for(dp = 0; dp < 4; dp++)
		{
			threshold = mix_percentage[dp] * total_bds[core] / 100;
			val[0] = convert_value_to_float(threshold, 8, 4);
			
			sprintf(buffer_reg, "IQM_GENERAL_REJECT_CONFIGURATION_B_DS.IQM%d", core);
			sprintf(buffer_field, "MIXED_RJCT_SET_OC_BD_TH_%d", dp);
			
			diag_reg_field_set(unit, buffer_reg, buffer_field, val);	
		}
	}
}

int print_update_fifo_th(int unit)
{
	int val = read_udpate_fifo_th(unit);
	if(val == 21)
	{
		printf("Update FIFO Threshold is: %d (GOOD)\n", val);
	}
	else
	{
		printf("Update FIFO Threshold is: %d (BAD)\n", val);
	}
}

int read_udpate_fifo_th(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	sprintf(buffer_reg, "IQM_INTERNAL_FIFOS_THRESHOLDS.IQM0");
	sprintf(buffer_field, "UPDT_FF_TH");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	return val[0];
}

// Set Recommended IQM Update FIFO Thresholds (IQM_UPDT_FIFO_TH)
int set_udpate_fifo_th(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	val[0] = 21;
	sprintf(buffer_reg, "IQM_INTERNAL_FIFOS_THRESHOLDS.IQM0");
	sprintf(buffer_field, "UPDT_FF_TH");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
	
	sprintf(buffer_reg, "IQM_INTERNAL_FIFOS_THRESHOLDS.IQM1");
	sprintf(buffer_field, "UPDT_FF_TH");	
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
}

int print_free_buffers(int unit)
{
	int val;
	int percent;
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		printf("Available Buffers for Core %d:\n", core);
		
		val = get_free_bds(unit, core);
		percent = val * 100 / total_bds[core];
		//printf("\tAvailable Buffer Descriptors:\t%d\t(%d%%)\n", val, percent);
		
		val = get_free_bdbs(unit, core);
		percent = val * 100 / total_bdbs[core];
		printf("\tAvailable BDBs:\t\t\t%d\t(%d%%)\n", val, percent);
		
		val = get_free_ocb(unit, core);
		percent = val * 100 / total_ocb_buffers[core];
		printf("\tAvailable OCB Buffers:\t\t%d\t(%d%%)\n", val, percent);
		
		val = get_free_mnmc(unit, core);
		percent = val * 100 / total_mnmc_buffers[core];
		printf("\tAvailable MNMC Buffers:\t\t%d\t(%d%%)\n", val, percent);
		
		val = get_free_fmc(unit, core);
		percent = val * 100 / total_fmc_buffers[core];
		printf("\tAvailable FLMC Buffers:\t\t%d\t(%d%%)\n", val, percent);
	}
}

int get_free_bdbs(int unit, int core)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	sprintf(buffer_reg, "IQM_FREE_BDB_MINIMUM_OCCUPANCY.IQM%d", core);
	sprintf(buffer_field, "FREE_BDB_MINIMUM_OCCUPANCY");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	return val[0] + 1;
}

int get_free_bds(int unit, int core)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	sprintf(buffer_reg, "IQM_FREE_BD_COUNTER.IQM%d", core);
	sprintf(buffer_field, "FREE_BD_COUNTER");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	return val[0];
}

int get_free_mnmc(int unit, int core)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	sprintf(buffer_reg, "IQM_FREE_MNMC_DBUFFS_MINIMUM_OCCUPANCY.IQM%d", core);
	sprintf(buffer_field, "FREE_MNMC_DBUFFS_MINIMUM_OCCUPANCY");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	return val[0];
}

int get_free_ocb(int unit, int core)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	sprintf(buffer_reg, "IQM_FREE_OCB_MINIMUM_OCCUPANCY.IQM%d", core);
	sprintf(buffer_field, "FREE_OCB_MINIMUM_OCCUPANCY");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	return val[0];
}

int get_free_fmc(int unit, int core)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	sprintf(buffer_reg, "IQM_FREE_FULL_MULTICAST_DBUFFS_MINIMUM_OCCUPANCY.IQM%d", core);
	sprintf(buffer_field, "FREE_FULL_MULTICAST_DBUFFS_MINIMUM_OCCUPANCY");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	return val[0];
}

int set_indirect_command_timeout(int unit, int core, int timeout)
{
	char buffer_reg[128];
	char buffer_field[128];
	reg_val val;
	
	sprintf(buffer_reg, "IQM_INDIRECT_COMMAND.IQM%d", core);
	sprintf(buffer_field, "INDIRECT_COMMAND_TIMEOUT");	
	val[0] = timeout;
	diag_reg_field_set(unit, buffer_reg, buffer_field, val);
}

// Print queues with current Size > 0
int print_active_queues_status(int unit)
{
	int i, j, queue, size;
	reg_val val;
	char buffer_reg[128];
	int core;
	
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		printf("Non Empty Queues (Max Size) Core %d:\n", core);
		
		set_indirect_command_timeout(unit, core, 0x7FFF);
		
		
		for(queue = 0; queue < 4; queue++)
		{
			size = get_queue_max_size(unit, core, queue);
			if(size != 0)
			{
				printf("\tQueue %d:\t%dB\n", queue, size);
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
						printf("\tQueue %d:\t%dB\n", queue + j, size);
					}
				}
			}
		}
		
		set_indirect_command_timeout(unit, core, 0x10);
	}
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

int float_to_int(int f, int mnt_bits)
{
	int exp = (1 << (f >> mnt_bits));
	int mnt = f & ((1 << mnt_bits) - 1);
	
	return (exp * mnt);
} 

uint32 find_msb(uint32 x_in)
{
	uint32 n, x;
	int i;
	x = x_in;

	if (x == 0) return(0);

	n = 0;
	for (i = 16; i > 0; i >>= 1)
	{
		if (x & ((0xffffffff)<<i))
		{
			n += i;
			x >>= i;
		}
	}
	return n;
}

uint32 convert_value_to_float(uint32 x, uint32 mnt_nof_bits, uint32 exp_nof_bits)
{
	uint32 value;
	uint32 mnt = 0;
	uint32 exp = 0;
	uint32 y;
	uint32 mnt_max, exp_max, msb_bit_on;
	int msb_bit_diff;
	
	mnt_max = (0x1<<mnt_nof_bits)-1;
	exp_max = (0x1<<exp_nof_bits)-1;
	msb_bit_on = 0;
	msb_bit_diff = 0;

	y = (1 << exp_max) * mnt_max;
	
	if (x > y)
	{ 	/*
		 * x is too big for our representation
		 * Put the biggest number it can
		 */
		mnt = mnt_max;
		exp = exp_max;
		value = mnt | (exp << mnt_nof_bits);
		return value;
	}
	
	msb_bit_on = find_msb(x) ;

	msb_bit_diff = (msb_bit_on+1) - mnt_nof_bits;
	if (msb_bit_diff<=0)
	{
		msb_bit_diff = 0 ;
	}

	mnt = mnt_max << msb_bit_diff ;
	mnt &= x ;

	mnt >>= msb_bit_diff;
	exp = msb_bit_diff ;

	value = mnt | (exp << mnt_nof_bits);
	return value;
}

void print_reject_reason(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int core;
	for(core = 0; core < get_number_of_cores(unit); core++)
	{
		printf("Packet Reject Bitmap (Core %d):\n", core);
		
		sprintf(buffer_reg, "IQM_REJECT_STATUS_BMP.IQM%d", core);
		diag_reg_get(unit, buffer_reg, val);
		
		if(val[0] == 0 && val[1] == 0)
		{
			printf("\tNone\n");
		}
		else
		{
			int i, bit;
			for(i = 0; i < 39; i++)
			{
				bit = (val[i / 32] >> i) & 0x1;
				if(bit == 1)
				{
					if(i == 0 ) printf("\tBit 0\tCfgDscrdAllPktStatus\tDiscard all packets reject\n");
					if(i == 1 ) printf("\tBit 1\tBdbProtectStatus\tBDB protect reject (no BDBs left for ENQ).\n");
					if(i == 2 ) printf("\tBit 2\tQnumOvfStatus\tQueue-Number exceeded 96k\n");
					if(i == 3 ) printf("\tBit 3\tIspDuplicateStatus\tIngress-Shape duplicate Error.A packet arrived from IRR's ISP context (second path) and is targeted to an ISP Queue again.\n");
					if(i == 4 ) printf("\tBit 4\tMulticastErrorStatus\tIRR reported on MC error.\n");
					if(i == 5 ) printf("\tBit 5\tResourceErrorStatus\tResource error reported on the packet enqueue command.Packet was rejected because the IDR ran out of buffers during packet re-assembly. \n");
					if(i == 6 ) printf("\tBit 6\tQueueNotValidStatus\tPacket was sent to an invalid queue (PP reject)\n");
					if(i == 7 ) printf("\tBit 7\tIdrDramRejectStatus\tIDR reject on DRAM eligible test (queue is not eligible to DRAM when MMU blockage occurred).\n");
					if(i == 8 ) printf("\tBit 8\tBuffTypeErr\tPacket destined to OCB-Only queue was assigned with a DRAM buffer (instead OCB).\n");
					if(i == 9 ) printf("\tBit 9\tDpLevelStatus\tPacket was received with DP >= DscrdDp.\n");
					if(i == 10) printf("\tBit 10\tVoqDynDramStatus\tShared DRAM bytes occupancy level crossed drop DP threshold.\n");
					if(i == 11) printf("\tBit 11\tVoqDynBuffStatus\tShared Buffers/BDs/OCBs occupancy level crossed drop DP threshold.\n");
					if(i == 12) printf("\tBit 12\tVoqDynBdbStatus\tShared BDBs occupancy level crossed drop DP threshold.\n");
					if(i == 13) printf("\tBit 13\tOccupiedBdStatus\tOccupied BDs reject\n");
					if(i == 14) printf("\tBit 14\tFrBdbStatus\tFree BDBs reject\n");
					if(i == 15) printf("\tBit 15\tFrFmcDbStatus\tFree FMC reject\n");
					if(i == 16) printf("\tBit 16\tFrMnmcDbStatus\tFree MNMC reject\n");
					if(i == 17) printf("\tBit 17\tFrOcbDbStatus\tFree OCB reject\n");
					if(i == 18) printf("\tBit 18\tVoqMxQszStatus\tVOQ max queue size in bytes reject\n");
					if(i == 19) printf("\tBit 19\tVoqMxQszBuffStatus\tVOQ max queue size in BDs/Buffers reject\n");
					if(i == 20) printf("\tBit 20\tVoqWredStatus\tVOQ WRED test reject\n");
					if(i == 21) printf("\tBit 21\tVoqFadtStatus\tVOQ Fair-Adaptive-Dynamic-Th reject\n");
					if(i == 22) printf("\tBit 22\tSystemRedStatus\tSystem RED\n");
					if(i == 23) printf("\tBit 23\tInterlockStatus\tInterlock reject\n");
					if(i == 24) printf("\tBit 24\tGlblShrdStatus\tVSQs - global Shared resource reject\n");
					if(i == 25) printf("\tBit 25\tPortAndPgStatus\tPort and PG reject\n");
					if(i == 26) printf("\tBit 26\tVsqaMxSzStatus\tVSQA max size reject\n");
					if(i == 27) printf("\tBit 27\tVsqbMxSzStatus\tVSQB max size reject\n");
					if(i == 28) printf("\tBit 28\tVsqcMxSzStatus\tVSQC max size reject\n");
					if(i == 29) printf("\tBit 29\tVsqdMxSzStatus\tVSQD max size reject\n");
					if(i == 30) printf("\tBit 30\tVsqeMxSzStatus\tVSQE max size reject\n");
					if(i == 31) printf("\tBit 31\tVsqfMxSzStatus\tVSQF max size reject\n");
					if(i == 32) printf("\tBit 32\tVsqaWredStatus\tVSQA WRED reject\n");
					if(i == 33) printf("\tBit 33\tVsqbWredStatus\tVSQB WRED reject\n");
					if(i == 34) printf("\tBit 34\tVsqcWredStatus\tVSQC WRED reject\n");
					if(i == 35) printf("\tBit 35\tVsqdWredStatus\tVSQD WRED reject\n");
					if(i == 36) printf("\tBit 36\tVsqeWredStatus\tVSQE WRED reject\n");
					if(i == 37) printf("\tBit 37\tVsqfWredStatus\tVSQF WRED reject\n");
					if(i == 38) printf("\tBit 38\tMcFadtStatus\tMC Fair-Adaptive-Dynamic-Th reject\n");
				}
			}
		}
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