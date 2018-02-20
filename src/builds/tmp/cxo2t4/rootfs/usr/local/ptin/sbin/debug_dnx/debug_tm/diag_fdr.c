
// Jericho FDR Debug CINT

int print_functions()
{	
	printf("\n");
	printf("CINT diag_fdr.c\n");
	printf("----------\n");
	printf("Functions:\n");
	printf("\t1. print_all(unit);\n");
	printf("\t2. print_aluwp(unit);\n");
	printf("\t3. print_fdr_fifo_status(unit);\n");
	printf("\t4. print_fda_fifo_status(unit);\n");
	printf("\n");
}

int print_all(int unit)
{
	print_aluwp(unit);
	printf("\n");
	print_fdr_fifo_status(unit);
	printf("\n");
	print_fda_fifo_status(unit);
	printf("\n");
}

// Print ALUWP Value
int print_aluwp(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	sprintf(buffer_reg,"FDR_FDR_ENABLERS_REGISTER_1.FDR0");
	sprintf(buffer_field, "FIELD_23_30");	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	printf("ALUWP = %d", val[0]);
	if(val[0] != 253)
	{
		// Known issue, SDK-94704
		printf("  -- Should be 253 !\n");
	}
	else
	{
		printf("\n");
	}
}

// Print FDR FIFOs Status
int print_fdr_fifo_status(int unit)
{
	int fdr, ifm, pipe;
	int val;
		
	printf("FDR FIFO Status:\n");
	
	// FDR A-B
	for(fdr = 0; fdr < 2; fdr++)
	{
		// IFM A-B
		for(ifm = 0; ifm < 2; ifm++)
		{
			// Pipes 1-3
			for(pipe = 1; pipe < 4; pipe++)
			{
				val = get_fdr_fifo_status(unit, fdr, ifm, pipe);
				printf("\tFDR%s IFM%s:\t%d\n", fdr ? "A" : "B", ifm ? "A" : "B", val);
			}
		}
	}
}

// Get a specific FDR FIFO watermark
int get_fdr_fifo_status(int unit, int fdr_a_b /*0=A, 1=B*/, int ifm_a_b /*0=A, 1=B*/, int pipe /* 1-3 */)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];
	
	if(fdr_a_b == 0)
	{
		// FDR A
		sprintf(buffer_reg,"FDR_P_%d_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDRA.FDR0", pipe);
		if(ifm_a_b == 0)
		{
			// IFM A
			sprintf(buffer_field, "P_%d_MAX_IFMAFFDRA", pipe);	
		}
		else
		{
			// IFM B
			sprintf(buffer_field, "P_%d_MAX_IFMBFFDRA", pipe);	
		}
	}
	else
	{
		// FDR B
		sprintf(buffer_reg,"FDR_P_%d_FDR_OVERFLOWS_AND_FIFOS_STATUSES_FDR_B.FDR0", pipe);
		if(ifm_a_b == 0)
		{
			// IFM A
			sprintf(buffer_field, "P_%d_MAX_IFMAFFDRB", pipe);	
		}
		else
		{
			// IFM B
			sprintf(buffer_field, "P_%d_MAX_IFMBFFDRB", pipe);	
		}
	}
	
	diag_reg_field_get(unit, buffer_reg, buffer_field, val);
	
	return val[0];
}

int print_fda_fifo_status(int unit)
{
	reg_val val;
	char buffer_reg[128];
	char buffer_field[128];

	int core, pipe;
	
	printf("FDA->EGQ FIFO Status:\n");
	
	// CORE 0, 1
	for(core = 0; core < 2; core++)
	{
		sprintf(buffer_reg,"FDA_EGQ_FAB_FIFOS_WM(%d)", core);
		
		// Pipes 0-2
		for(pipe = 0; pipe < 3; pipe++)
		{
			sprintf(buffer_field, "EGQ_N_FAB_%d_FIFO_WM", pipe);	
			diag_reg_field_get(unit, buffer_reg, buffer_field, val);
			
			printf("\tFDA->EGQ(%d) Pipe(%d):\t%d\n", core, pipe, val[0]);
		}
	}
	for(core = 0; core < 2; core++)
	{
		sprintf(buffer_reg,"FDA_EGQ_TDM_FIFO_WM(%d)", core);
		
		sprintf(buffer_field, "EGQ_N_TDM_FIFO_WM");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		printf("\tFDA->EGQ(%d) TDM:\t%d\n", core, val[0]);
	}
	for(core = 0; core < 2; core++)
	{
		sprintf(buffer_reg,"FDA_EGQ_MESHMC_FIFO_WM(%d)", core);
		
		sprintf(buffer_field, "EGQ_N_MESHMC_FIFO_WM");	
		diag_reg_field_get(unit, buffer_reg, buffer_field, val);
		
		printf("\tFDA->EGQ(%d) MESH MC:\t%d\n", core, val[0]);
	}
}


print_functions();
print_all(0);