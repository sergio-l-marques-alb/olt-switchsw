cint_reset();

int diag_fabric_reachibility_mod_id=0;
int diag_cosq_is_voq_or_sysport=0;
int diag_cosq_voq_flow_id_or_sysport_num=0;

int print_delimiter()
{
	printf("\n----------------------------------------------------------------------------------------------\-----------------------------------------------\n");

}

int bcm_shell_dump(int unit,char *cmd)
{

	printf("\nBCM.%d> %s\n",unit,cmd);
	bshell(unit,cmd);
	print_delimiter();
}

int fe_diag_tm_dump(int unit)
{	
	char cmd[100];

	print_delimiter();
	bcm_shell_dump(unit,"fabric queues");
	sprintf(cmd, "fabric reachability %d",diag_fabric_reachibility_mod_id);
    bcm_shell_dump(unit,cmd);
    bcm_shell_dump(unit,"fabric connectivity");
	bcm_shell_dump(unit,"g * debug");

}

int fe_diag_tm_dump_tables(int unit)
{	
	bcm_shell_dump(unit,"d raw RTP_RMHMT");
}

int fe_diag_tm_dump_full(int unit)
{	
	char cmd[100];

	fe_diag_tm_dump(unit);
	fe_diag_tm_dump_tables(unit);

}

int fap_diag_tm_dump_full(int unit)
{
	fap_diag_tm_dump(unit);
	fap_diag_tm_tables_dump(unit);
}

int fap_diag_tm_dump(int unit)
{
	char cmd[100];

	print_delimiter();
	bcm_shell_dump(unit,"diag cosq non");
	bcm_shell_dump(unit,"diag egr max");
	sprintf(cmd, "fabric reachability %d",diag_fabric_reachibility_mod_id);
    bcm_shell_dump(unit,cmd);
	bcm_shell_dump(unit,"fabric connectivity");
	bcm_shell_dump(unit,"fabric mesh_topology");
	bcm_shell_dump(unit,"cosq conn ing");
	bcm_shell_dump(unit,"cosq conn egr");
	bcm_shell_dump(unit,"diag cosq qpair egq map");
	
	sprintf(cmd,"diag cosq print_flow_and_up is_voq=%d print_status=1 dest_id=%d", diag_cosq_is_voq_or_sysport, diag_cosq_voq_flow_id_or_sysport_num);
	bcm_shell_dump(unit,cmd);
	

}

int fap_diag_tm_tables_dump(int unit)
{
	bcm_shell_dump(unit,"d raw IRR_DESTINATION_TABLE");
	bcm_shell_dump(unit,"d raw IRR_QUEUE_IS_OCB_COMMITTED");
	bcm_shell_dump(unit,"d raw IRE_TDM_CONFIG");
	bcm_shell_dump(unit,"d raw IDR_QUEUE_IS_DRAM_ELIGIBLE");
	bcm_shell_dump(unit,"d raw IDR_QUEUE_IS_OCB_ELIGIBLE");
	bcm_shell_dump(unit,"d raw IDR_DRAM_BUFFER_TYPE");
	bcm_shell_dump(unit,"d raw IQM_CRDTDIS");
	bcm_shell_dump(unit,"d raw IQM_GRSPRM");
	bcm_shell_dump(unit,"d raw IQM_OCBPRM");
	bcm_shell_dump(unit,"d raw IQM_PACK_MODE");
	bcm_shell_dump(unit,"d raw IQM_PG_TC_BITMAP");
	bcm_shell_dump(unit,"d raw IQM_PQDMS");
	bcm_shell_dump(unit,"d raw IQM_PQRED");
	bcm_shell_dump(unit,"d raw IQM_VS_QA_QSZ");
	bcm_shell_dump(unit,"d raw IQM_VS_QB_QSZ");
	bcm_shell_dump(unit,"d raw IQM_VS_QC_QSZ");
	bcm_shell_dump(unit,"d raw IQM_VS_QD_QSZ");
	bcm_shell_dump(unit,"d raw IQM_VS_QE_QSZ");
	bcm_shell_dump(unit,"d raw IQM_VS_QF_QSZ");
	bcm_shell_dump(unit,"d raw IQM_CNG_QUE_SET");
	bcm_shell_dump(unit,"d raw IPST_CRVS");
	bcm_shell_dump(unit,"d raw IPS_QPRI");
	bcm_shell_dump(unit,"d raw IPS_CRWDTH");
	bcm_shell_dump(unit,"d raw IPS_EMPTYQCRBAL");
	bcm_shell_dump(unit,"d raw IPS_SFTH");
	bcm_shell_dump(unit,"d raw IPS_QPM_1_NO_SYS_RED");
	bcm_shell_dump(unit,"d raw IPS_QPM_1_SYS_RED");
	bcm_shell_dump(unit,"d raw IPS_QPM_2_NO_SYS_RED");
	bcm_shell_dump(unit,"d raw IPS_QPM_2_SYS_RED");
	bcm_shell_dump(unit,"d raw IPS_CRBALTH");
	bcm_shell_dump(unit,"d raw IPS_FLWID");
	bcm_shell_dump(unit,"d raw IPS_QDESC");
	bcm_shell_dump(unit,"d raw IPS_QPRI");
	bcm_shell_dump(unit,"d raw IPS_QSZTH");
	bcm_shell_dump(unit,"d raw IPS_CR_BAL_TABLE");
	bcm_shell_dump(unit,"d raw IPT_IPT_0_DEST_PIPE_MAPPING_2_FDT");
	bcm_shell_dump(unit,"d raw IPT_IPT_1_DEST_PIPE_MAPPING_2_FDT");
	bcm_shell_dump(unit,"d raw IPT_PRIORITY_BITS_MAPPING_2_FDT");
	bcm_shell_dump(unit,"d raw IPT_TDM_BIT_MAPPING_2_FDT");
	bcm_shell_dump(unit,"d raw EGQ_PCT");
	bcm_shell_dump(unit,"d raw EGQ_PPCT");
	bcm_shell_dump(unit,"d raw EGQ_DWM");
	bcm_shell_dump(unit,"d raw EGQ_DWM_8P");
	bcm_shell_dump(unit,"d raw EGQ_PDCT_TABLE");
	bcm_shell_dump(unit,"d raw EGQ_PDCMAX");
	bcm_shell_dump(unit,"d raw EGQ_PQST_TABLE");
	bcm_shell_dump(unit,"d raw EGQ_PQSMAX");
	bcm_shell_dump(unit,"d raw EGQ_QDCT_TABLE");
	bcm_shell_dump(unit,"d raw EGQ_QDCMAX");
	bcm_shell_dump(unit,"d raw EGQ_FDCMAX");
	bcm_shell_dump(unit,"d raw EGQ_QQST_TABLE");
	bcm_shell_dump(unit,"d raw EGQ_QQSMAX");
	bcm_shell_dump(unit,"d raw EGQ_FQSMAX");
	bcm_shell_dump(unit,"d raw EGQ_TC_DP_MAP");
	bcm_shell_dump(unit,"d raw EGQ_MAP_PS_TO_IFC");
	bcm_shell_dump(unit,"d raw EGQ_MC_SP_TC_MAP");
	bcm_shell_dump(unit,"d raw EGQ_EGRESS_SHAPER_CONFIGURATION");
	bcm_shell_dump(unit,"d raw EGQ_PER_IFC_CFG");
	bcm_shell_dump(unit,"d raw EGQ_EPS_PRIO_MAP");
	bcm_shell_dump(unit,"d raw EGQ_DSP_PTR_MAP");
	bcm_shell_dump(unit,"d raw SCH_FC_MAP_FCM");
	bcm_shell_dump(unit,"d raw SCH_PORT_ENABLE_PORTEN");
	bcm_shell_dump(unit,"d raw SCH_ONE_PORT_NIF_CONFIGURATION_OPNC");
	bcm_shell_dump(unit,"d raw SCH_PIR_SHAPERS_STATIC_TABEL_PSST");
	bcm_shell_dump(unit,"d raw SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCC");
	bcm_shell_dump(unit,"d raw SCH_CH_NIF_RATES_CONFIGURATION_CNRC");
	bcm_shell_dump(unit,"d raw SCH_CIR_SHAPERS_STATIC_TABEL_CSST");
	bcm_shell_dump(unit,"d raw SCH_DEVICE_RATE_MEMORY_DRM");
	bcm_shell_dump(unit,"d raw SCH_SHARED_DEVICE_RATE_SHARED_DRM");
	bcm_shell_dump(unit,"d raw SCH_SLOW_FACTOR_MEMORY_SFM");
	bcm_shell_dump(unit,"d raw SCH_TOKEN_MEMORY_CONTROLLER_TMC");
	bcm_shell_dump(unit,"d raw SCH_FLOW_INSTALLED_MEMORY_FIM");
	bcm_shell_dump(unit,"d raw SCH_FLOW_STATUS_MEMORY_FSM");
	bcm_shell_dump(unit,"d raw SCH_DSP_2_PORT_MAP_DSPP");
	bcm_shell_dump(unit,"d raw FDT_IPT_MESH_MC");

}

int fap_diag_pp_dump(int unit)
{
	print_delimiter();
	bcm_shell_dump(unit,"tm -stoponerror +progress");
	bcm_shell_dump(unit,"g * debug");
	bcm_shell_dump(unit,"diag dump");
	bcm_shell_dump(unit,"diag pp last ");
	bcm_shell_dump(unit,"diag pp rpi  ");
	bcm_shell_dump(unit,"diag pp pi   ");
	bcm_shell_dump(unit,"diag pp termi");
	bcm_shell_dump(unit,"diag pp ive  ");
	bcm_shell_dump(unit,"diag pp pkttm");
	bcm_shell_dump(unit,"diag pp dblif");
	bcm_shell_dump(unit,"diag pp fdt  ");
	bcm_shell_dump(unit,"diag pp trapsi   ");
	bcm_shell_dump(unit,"diag pp encap");
	bcm_shell_dump(unit,"diag pp fli  ");
	bcm_shell_dump(unit,"kbp kaps_show");
	bcm_shell_dump(unit,"kbp kaps_db_stats");
	bcm_shell_dump(unit,"diag pp dblem");
	bcm_shell_dump(unit,"diag pp modeg");
	bcm_shell_dump(unit,"diag prge_info   ");
	bcm_shell_dump(unit,"diag prge_last   ");
	bcm_shell_dump(unit,"diag template all");
	bcm_shell_dump(unit,"diag pp vtt  ");
	bcm_shell_dump(unit,"diag pp vtt last=1   ");
	bcm_shell_dump(unit,"diag pp eve  ");
	bcm_shell_dump(unit,"diag pp ipv4_mc  ");
	bcm_shell_dump(unit,"stg show ");
	bcm_shell_dump(unit,"diag pp flp  ");
	bcm_shell_dump(unit,"diag pp flp last=1   ");
	bcm_shell_dump(unit,"diag alloc all   ");
	bcm_shell_dump(unit,"diag dbal tif");
	bcm_shell_dump(unit,"diag dbal lp ");
	bcm_shell_dump(unit,"diag pp cc   ");
	bcm_shell_dump(unit,"diag pp occ  ");
	bcm_shell_dump(unit,"diag pp cos  ");
	bcm_shell_dump(unit,"diag pp ECMP_load_balancing  ");
	bcm_shell_dump(unit,"diag pp fec all  ");
	bcm_shell_dump(unit,"diag field res   ");
	bcm_shell_dump(unit,"diag pp klkp ");
	bcm_shell_dump(unit,"diag pp dbglem   ");
	bcm_shell_dump(unit,"diag pp rif  ");
	bcm_shell_dump(unit,"diag pp edrop");
	bcm_shell_dump(unit,"ps   ");
	bcm_shell_dump(unit,"show c   ");
}

int l2_show(int unit)
{
	bcm_shell_dump(unit,"l2 show");
}

int fap_diag_pp_dump_full(int unit)
{
	print_delimiter();
	fap_diag_pp_dump(unit);
	l2_show(unit);
	bcm_shell_dump(unit,"");

}

int fap_diag_oam(int unit)
{
	print_delimiter();
	bcm_shell_dump(unit,"diag oam ep");
	bcm_shell_dump(unit,"diag oam lu");
	bcm_shell_dump(unit,"diag oam prge");
	bcm_shell_dump(unit,"diag oam count oamp");
}