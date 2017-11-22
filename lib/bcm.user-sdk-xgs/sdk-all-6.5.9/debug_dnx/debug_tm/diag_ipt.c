
// Jericho IPT Debug CINT

int print_zero_values = 1;

int print_functions()
{	
	printf("\n");
	printf("CINT diag_ipt.c\n");
	printf("----------\n");
	printf("Functions:\n");
	printf("\t1. print_all(unit);\n");
	printf("\t2. print_max_occupancy(unit);\n");
	printf("\t3. print_flow_control_status(unit);\n");
	printf("\n");
}

int print_all(int unit)
{
	print_max_occupancy(unit);
	printf("\n");
	print_flow_control_status(unit);
	printf("\n");
}

int print_max_occupancy(int unit)
{
	print_dqcq_max_occupancy(unit);
	printf("\n");
	print_egq_max_occupancy(unit);
	printf("\n");
	print_dram_max_occupancy(unit);
	printf("\n");
	print_fdt_max_occupancy(unit);
	
}

int print_flow_control_status(int unit)
{
	printf("Active FC IPT->IPS:\n");
	print_flow_control_dqcq(unit);
	print_flow_control_dtq(unit);
	print_flow_control_egq(unit);
	print_flow_control_ocb(unit);
}

int print_flow_control_ocb(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int active_bits, queue_bit, i, bit, j;
	
	sprintf(buffer_reg, "IPT_IPT_2_IPS_FC_STATUS_VEC_2.IPT0");
	sprintf(buffer_field, "OCB_ARB_STOP_OCB_ONLY_FC");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	active_bits = val[0];
	
	if(active_bits == 1) printf("\tOCB ONLY\n", i);
	
	for(i = 0; i < 2; i++)
	{
		sprintf(buffer_reg, "IPT_IPT_2_IPS_FC_STATUS_VEC_2.IPT0");
		sprintf(buffer_field, "DTQ_2_DQCF_%d_OVTH_FC", i);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		active_bits = val[0];
		
		if(active_bits == 1) printf("\tOCB MIX Core(%d)\n", i);
	}
}

int print_flow_control_egq(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int active_bits, queue_bit, i, bit, j;
	
	for(i = 0; i < 2; i++)
	{
		sprintf(buffer_reg, "IPT_IPT_2_IPS_FC_STATUS_VEC_2.IPT0");
		sprintf(buffer_field, "TX_EGQ_%d2_IPT_SCH_FC", i);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		active_bits = val[0];
		
		if(active_bits == 1) printf("\tEGQ(%d)\n", i);
		
		sprintf(buffer_field, "TX_EGQ_%d2_EGQ_AF_FC", i);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		active_bits = val[0];
		
		if(active_bits == 1) printf("\tEGQ(%d) AF\n", i);
		
		sprintf(buffer_field, "TX_EGQ_%d2_EGQ_GEN_RCI_FC", i);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		active_bits = val[0];
		
		if(active_bits == 1) printf("\tEGQ(%d) RCI\n", i);
		
		sprintf(buffer_field, "TX_EGQ_%d2_DBLF_MIX_FC", i);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		active_bits = val[0];
		
		if(active_bits == 1) printf("\tEGQ(%d) DBLF MIX\n", i);
		
		sprintf(buffer_field, "TX_EGQ_%d2_DBLF_OCB_FC", i);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		active_bits = val[0];
		
		if(active_bits == 1) printf("\tEGQ(%d) DBLF OCB\n", i);
	}
}

int print_flow_control_dtq(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int active_bits, queue_bit, i, bit, j;
	
	sprintf(buffer_reg, "IPT_IPT_2_IPS_FC_STATUS_VEC_2.IPT0");
	sprintf(buffer_field, "DTQ_2_IPT_OVTH_FC");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	active_bits = val[0];
	
	for(i = 0; i < 12; i++)
	{
		queue_bit = (active_bits >> i) & 1;
		if(queue_bit == 1)
		{
			int core = i / 6;
			int pipe = (i % 6) / 2;
			printf("\tDTQ Core-%d Pipe-%d %s\n", core, pipe, get_fdt_type(i));
		}
		
	}
	
	for(j = 0; j < 2; j++)
	{
		sprintf(buffer_reg, "IPT_IPT_2_IPS_FC_STATUS_VEC_2.IPT0");
		sprintf(buffer_field, "DTQ_2_DQCF_%d_OVTH_FC", j);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		active_bits = val[0];
		
		for(i = 0; i < 6; i++)
		{
			queue_bit = (active_bits >> i) & 1;
			if(queue_bit == 1)
			{
				int core = (i + (j * 6)) / 6;
				int pipe = ((i + (j * 6)) % 6) / 2;
				printf("\tDTQ Core-%d Pipe-%d %s\n", core, pipe, get_fdt_type(i + (j * 6)));
			}
			
		}
	}
}

int print_flow_control_dqcq(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int active_bits, queue_bit, i, bit, j;
	
	sprintf(buffer_reg, "IPT_IPT_2_IPS_FC_STATUS_VEC.IPT0");
	sprintf(buffer_field, "IPT_2_IPS_DBLF_0_DQCQ_FC");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	active_bits = val[0];
	
	for(i = 0; i < 20; i++)
	{
		queue_bit = (active_bits >> i) & 1;
		if(queue_bit == 1)
		{
			printf("\tDQCQ %s %s\n", get_dqcq_context(i), get_dqcq_type(i));
		}
		
	}
	
	for(j == 0; i < 2; j++)
	{
		sprintf(buffer_reg, "IPT_IPT_2_IPS_FC_STATUS_VEC_2.IPT0");
		sprintf(buffer_field, "IPT_2_IPS_FMC_DBLF_%d_FC", j);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		active_bits = val[0];
		
		int active = 0;
		for(i = 0; i < 4; i++)
		{
			queue_bit = (active_bits >> i) & 1;
			if(queue_bit == 1)
			{
				printf("\tDBLF %s\n", get_dblf_type(i));
			}
			
		}
	}
}

int print_fdt_max_occupancy(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int queue, current_size, max_size, fullness;
	
	printf("FDT FIFOs:\n");
	
	sprintf(buffer_reg, "IPT_FIFOS_MAX_OCC_1.IPT0");
	sprintf(buffer_field, "FDTC_MAX_OC_QNUM");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	queue = val[0];

	sprintf(buffer_reg, "IPT_FIFOS_MAX_OCC_1.IPT0");
	sprintf(buffer_field, "FDTC_MAX_OC");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	current_size = val[0];
	
	sprintf(buffer_reg, "IPT_DTQ_MAX_SIZE.IPT0");
	sprintf(buffer_field, "DTQ_%d_MAX_SIZE_%d", queue / 6, queue % 6);
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	max_size = val[0];
	
	fullness = current_size * 100 / max_size;
	
	if(current_size != 0 || print_zero_values == 1)
	{
		int core = queue / 6;
		int pipe = (queue % 6) / 2;
		printf("\tCTRL Core-%d Pipe-%d %s:\t%d\t(%d%%)\n", core, pipe, get_fdt_type(queue), current_size, fullness);
	}
	
	sprintf(buffer_reg, "IPT_FIFOS_MAX_OCC_1.IPT0");
	sprintf(buffer_field, "FDTD_MAX_OC_QNUM");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	queue = val[0];

	sprintf(buffer_reg, "IPT_FIFOS_MAX_OCC_1.IPT0");
	sprintf(buffer_field, "FDTD_MAX_OC");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	current_size = val[0];
	
	sprintf(buffer_reg, "IPT_DTQ_MAX_SIZE.IPT0");
	sprintf(buffer_field, "DTQ_%d_MAX_SIZE_%d", queue / 6, queue % 6);
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	max_size = val[0];
	
	fullness = current_size * 100 / max_size;
	
	if(current_size != 0 || print_zero_values == 1)
	{
		int core = queue / 6;
		int pipe = (queue % 6) / 2;
		printf("\tDATA Core-%d Pipe-%d %s:\t%d\t(%d%%)\n", core, pipe, get_fdt_type(queue), current_size, fullness);
	}
}

int print_dram_max_occupancy(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int queue, current_size, max_size, fullness;
	
	printf("DRAM FIFOs:\n");
	

	sprintf(buffer_reg, "IPT_FIFOS_MAX_OCC_3.IPT0");
	sprintf(buffer_field, "MIX_MOP_DESC_FIFO_MAX_OC");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	current_size = val[0];

	sprintf(buffer_reg, "IPT_MIX_MOP_DESC_CNTXTS_SIZE.IPT0");
	sprintf(buffer_field, "MIX_MOP_DESC_CONTEXT_0_SIZE");
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	max_size = val[0];
	
	fullness = current_size * 100 / max_size;
	
	if(current_size != 0 || print_zero_values == 1)
	{
		printf("\tDRAM FIFO:\t\t%d\t(%d%%)\n", current_size, fullness);
	}
}

int print_egq_max_occupancy(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int queue, current_size, max_size, fullness;
	
	printf("EGQ FIFOs:\n");
	
	int i;
	for(i = 0; i < 2; i++)
	{
		sprintf(buffer_reg, "IPT_FIFOS_MAX_OCC_2.IPT0");
		sprintf(buffer_field, "EGQC_%d_TX_FIFO_MAX_OC", i);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		current_size = val[0];

		max_size = 128;
		fullness = current_size * 100 / max_size;
		
		if(current_size != 0 || print_zero_values == 1)
		{
			printf("\tCTRL Core-%d TX FIFO:\t%d\t(%d%%)\n", i, current_size, fullness);
		}
		
		sprintf(buffer_reg, "IPT_FIFOS_MAX_OCC_2.IPT0");
		sprintf(buffer_field, "EGQD_%d_TX_FIFO_MAX_OC", i);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		current_size = val[0];

		max_size = 128;
		fullness = current_size * 100 / max_size;
		
		if(current_size != 0 || print_zero_values == 1)
		{
			printf("\tDATA Core-%d TX FIFO:\t%d\t(%d%%)\n", i, current_size, fullness);
		}
	}
}

int print_dqcq_max_occupancy(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	int queue, current_size, max_size, fullness;
	
	printf("DQCQ / DBLF:\n");
	
	int i;
	for(i = 0; i < 2; i++)
	{
		sprintf(buffer_reg, "IPT_FIFOS_MAX_OCC_0.IPT0");
		sprintf(buffer_field, "DBLF_%d_MAX_OC_QNUM", i);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		queue = val[0];
		
		sprintf(buffer_reg, "IPT_FIFOS_MAX_OCC_0.IPT0");
		sprintf(buffer_field, "DBLF_%d_MAX_OC", i);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		current_size = val[0];
		
		sprintf(buffer_reg, "IPT_DQCF_%d_MAX_SIZE.IPT0", i);
		sprintf(buffer_field, "DQCF_%d_MAX_SIZE_%d", i, queue);
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		max_size = val[0];
		
		fullness = current_size * 100 / max_size;
		
		if(current_size != 0 || print_zero_values == 1)
		{
			printf("\t%s - %s:\t%d\t(%d%%)\n", get_dqcq_context(queue), get_dqcq_type(queue), current_size, fullness);
		}
	}
}

char* get_dblf_type(int dblf)
{
	if(dblf == 0)
	{
		return "BFMC OCB";
	}
	if(dblf == 1)
	{
		return "GFMC OCB";
	}
	if(dblf == 2)
	{
		return "BFMC Mix";
	}
	if(dblf == 3)
	{
		return "GFMC Mix";
	}
}

char* get_fdt_type(int queue)
{
	if(queue % 2 == 0)
	{
		return "OCB";
	}
	else
	{
		return "MIX";
	}
}

char* get_dqcq_type(int dqcq)
{
	int context = dqcq % 4;
	
	if(context == 0)
	{
		return "OCB High";
	}
	if(context == 1)
	{
		return "OCB Low";
	}
	if(context == 2)
	{
		return "Mix High";
	}
	if(context == 3)
	{
		return "Mix Low";
	}
	
	return "";
}

char* get_dqcq_context(int dqcq)
{
	int context = dqcq / 4;
	
	if(context == 0)
	{
		return "Local-0";
	}
	if(context == 1)
	{
		return "Local-1";
	}
	if(context == 3)
	{
		return "Fabric UC";
	}
	if(context == 4)
	{
		return "Fabric MC";
	}
	
	return "unused";
}

print_functions();
print_all(0);