//////////////TRUNK///SDK657//JUST TRUNK WORKING////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include <port.c>
#include <bridge.c>
#include <trunk.c>

global_init();
bshell (unit,"port xe1-xe12 speed=1000 rxpause=0 ls=sw txpause=0 en=1");
bshell (unit,"port xe4 speed=10000 rxpause=0 ls=sw txpause=0 en=1");
bshell (unit,"ps");
bshell (0,"gport");


/* Check if double lb key range is enabled (relevant for gracefull load balancing) */
print bcm_switch_control_set(0, bcmSwitchMcastTrunkIngressCommit, 0);
print bcm_switch_control_set(0, bcmSwitchMcastTrunkEgressCommit, 0);

/* Define VLAN-domains */
print bcm_port_class_set(0,1,bcmPortClassId,9);
print bcm_port_class_set(0,2,bcmPortClassId,9);
print bcm_port_class_set(0,3,bcmPortClassId,9);
print bcm_port_class_set(0,4,bcmPortClassId,9);

trunk_create(1,9);
trunk_member_add(1,1);
trunk_member_add(1,2);
trunk_member_add(1,3);

vp_add(0x44801000,3,0xc000001,100,0,0,0,0);
vp_add(0x44801001,3,4,100,0,0,0,0);

vswitch_create(4096);
vswitch_add(4096,0x44801000);
vswitch_add(4096,0x44801001);

multicast_create(4096,0,0);
multicast_egress_add(16781312,0xc000001,0x44801000);
multicast_egress_add(16781312,        4,0x44801001);


/* Use MACSA to calculate hash for L2 packets */
//PG116
//
print bcm_switch_control_set(unit, bcmSwitchHashL2Field0, 0);
print bcm_switch_control_set(unit, bcmSwitchHashL2Field0, BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI);

//print bcm_switch_control_set(unit, bcmSwitchHashL2Field0, 0);
//print bcm_switch_control_set(unit, bcmSwitchHashL2Field0, BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI | BCM_HASH_FIELD_MACDA_LO | BCM_HASH_FIELD_MACDA_MI | BCM_HASH_FIELD_MACDA_HI);




//           bshell (unit,"l2 clear all");
bshell (unit,"diag pp last");
bshell (unit,"l2 show");
bshell (unit,"dune \"ppd_api frwrd_mact get_block\" ");
bshell (unit,"m IPS_IPS_GENERAL_CONFIGURATIONS DIS_DEQ_CMDS=1"); //STOP
bshell (unit,"diag cosq non");
bshell (unit,"m IPS_IPS_GENERAL_CONFIGURATIONS DIS_DEQ_CMDS=0"); //START






//VOQ:

	int phy_port;
	/* De Porto Físico a SystemPort*/
    int device_id;
	int system_port_id;
	bcm_gport_t port_mod_gport, system_port_gport;
	
	bcm_gport_t gport_voq_1r8, gport_voq_2r8, gport_voq_3r8, gport_voq_conn_1r8, gport_voq_conn_2r8, gport_voq_conn_3r8;

	bcm_gport_t tc_gport;
	bcm_cosq_gport_info_t gport_info;
	bcm_cosq_gport_connection_t connection;
	bcm_gport_t dest_port;
	bcm_gport_t tc_1_gport_hr_1,tc_1_gport_hr_0;
	bcm_gport_t tc_2_gport_hr_1,tc_2_gport_hr_0;
	bcm_gport_t tc_3_gport_hr_1,tc_3_gport_hr_0;
	
	bcm_gport_t tc_4_gport_hr_1,tc_4_gport_hr_0;

    int flags;
    int num_tc_idx;
    int servico_id= 0;
    int onu_id=0;
    int rval=0;

    bcm_gport_t null_gport;
    BCM_GPORT_MODPORT_SET(null_gport, 0, 0);
	bshell (unit,"gport");
		
		
		////////////// PS
	device_id=unit;
	phy_port=1;//Porto 1!!
    system_port_id=phy_port+device_id*30;//Avoid SysPortID to overlap over a stacked system!!
    //A GPORT of MODPORT  Type that represents a LocalPort/PHYSICAL PORT in a FAP/UNIT
    BCM_GPORT_MODPORT_SET        (port_mod_gport, device_id, phy_port);
    //Create a GPORT  SYSTEMPORT that can be called using  'system_port_id', that associates a PHYSICALPORT in a FAP
    BCM_GPORT_SYSTEM_PORT_ID_SET (system_port_gport,system_port_id);
    bcm_stk_sysport_gport_set    (unit, system_port_gport, port_mod_gport);
	
	print port_mod_gport;
	print system_port_gport;
	print system_port_id;
	
	gport_info.in_gport=system_port_gport; 
	gport_info.out_gport=0;
	gport_info.cosq=0;
	print bcm_cosq_gport_handle_get (unit, /*bcmCosqGportTypeE2EPort*/ bcmCosqGportTypeE2EPortTC, &gport_info);
	tc_gport=gport_info.out_gport;
	print tc_gport;
	print gport_info;
	flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_DUAL_WFQ   |  BCM_COSQ_GPORT_REPLACE;
	print bcm_cosq_gport_add (unit,null_gport/*GPORT NULL*/,0/*HR PRIO*/,flags,tc_gport);
	print tc_gport;
	print bcm_cosq_gport_add (unit,null_gport/*GPORT NULL*/,1/*HR PRIO*/,flags,tc_gport);
	print tc_gport;
	gport_info.in_gport = tc_gport;
    gport_info.cosq = 0;  //priority 0~7
    rval= bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);
    tc_1_gport_hr_0 = gport_info.out_gport;
    gport_info.in_gport = tc_gport;
    gport_info.cosq = 1;  //priority 0~7
    rval= bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);
    tc_1_gport_hr_1 = gport_info.out_gport; 
	bshell (unit,"gport");
//--
	device_id=unit;
	phy_port=2;//Porto 2!!
    system_port_id=phy_port+device_id*30;//Avoid SysPortID to overlap over a stacked system!!
    //A GPORT of MODPORT  Type that represents a LocalPort/PHYSICAL PORT in a FAP/UNIT
    BCM_GPORT_MODPORT_SET        (port_mod_gport, device_id, phy_port);
    //Create a GPORT  SYSTEMPORT that can be called using  'system_port_id', that associates a PHYSICALPORT in a FAP
    BCM_GPORT_SYSTEM_PORT_ID_SET (system_port_gport,system_port_id);
    bcm_stk_sysport_gport_set    (unit, system_port_gport, port_mod_gport);
		
	print port_mod_gport;
	print system_port_gport;
	print system_port_id;
	
	gport_info.in_gport=system_port_gport; 
	gport_info.out_gport=0;
	gport_info.cosq=0;
	print bcm_cosq_gport_handle_get (unit, /*bcmCosqGportTypeE2EPort*/ bcmCosqGportTypeE2EPortTC, &gport_info);
	tc_gport=gport_info.out_gport;
	print tc_gport;
	print gport_info;
	flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_DUAL_WFQ   |  BCM_COSQ_GPORT_REPLACE;
	print bcm_cosq_gport_add (unit,null_gport/*GPORT NULL*/,0/*HR PRIO*/,flags,tc_gport);
	print tc_gport;
	print bcm_cosq_gport_add (unit,null_gport/*GPORT NULL*/,1/*HR PRIO*/,flags,tc_gport);
	print tc_gport;
	gport_info.in_gport = tc_gport;
    gport_info.cosq = 0;  //priority 0~7
    rval= bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);
    tc_2_gport_hr_0 = gport_info.out_gport;
    gport_info.in_gport = tc_gport;
    gport_info.cosq = 1;  //priority 0~7
    rval= bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);
    tc_2_gport_hr_1 = gport_info.out_gport; 
	bshell (unit,"gport");
	//--
	
	device_id=unit;
	phy_port=3;//Porto 3!!
    system_port_id=phy_port+device_id*30;//Avoid SysPortID to overlap over a stacked system!!
    //A GPORT of MODPORT  Type that represents a LocalPort/PHYSICAL PORT in a FAP/UNIT
    BCM_GPORT_MODPORT_SET        (port_mod_gport, device_id, phy_port);
    //Create a GPORT  SYSTEMPORT that can be called using  'system_port_id', that associates a PHYSICALPORT in a FAP
    BCM_GPORT_SYSTEM_PORT_ID_SET (system_port_gport,system_port_id);
    bcm_stk_sysport_gport_set    (unit, system_port_gport, port_mod_gport);
	gport_info.in_gport=system_port_gport; 
	gport_info.out_gport=0;
	gport_info.cosq=0;
	print bcm_cosq_gport_handle_get (unit, /*bcmCosqGportTypeE2EPort*/ bcmCosqGportTypeE2EPortTC, &gport_info);
	tc_gport=gport_info.out_gport;
	print tc_gport;
	print gport_info;
	flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_DUAL_WFQ   |  BCM_COSQ_GPORT_REPLACE;
	print bcm_cosq_gport_add (unit,null_gport/*GPORT NULL*/,0/*HR PRIO*/,flags,tc_gport);
	print tc_gport;
	print bcm_cosq_gport_add (unit,null_gport/*GPORT NULL*/,1/*HR PRIO*/,flags,tc_gport);
	print tc_gport;
	gport_info.in_gport = tc_gport;
    gport_info.cosq = 0;  //priority 0~7
    rval= bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);
    tc_3_gport_hr_0 = gport_info.out_gport;
    gport_info.in_gport = tc_gport;
    gport_info.cosq = 1;  //priority 0~7
    rval= bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);
    tc_3_gport_hr_1 = gport_info.out_gport; 
	bshell (unit,"gport");
	
	device_id=unit;
	phy_port=4;//Porto 4!!
    system_port_id=phy_port+device_id*30;//Avoid SysPortID to overlap over a stacked system!!
    //A GPORT of MODPORT  Type that represents a LocalPort/PHYSICAL PORT in a FAP/UNIT
    BCM_GPORT_MODPORT_SET        (port_mod_gport, device_id, phy_port);
    //Create a GPORT  SYSTEMPORT that can be called using  'system_port_id', that associates a PHYSICALPORT in a FAP
    BCM_GPORT_SYSTEM_PORT_ID_SET (system_port_gport,system_port_id);
    bcm_stk_sysport_gport_set    (unit, system_port_gport, port_mod_gport);
	gport_info.in_gport=system_port_gport; 
	gport_info.out_gport=0;
	gport_info.cosq=0;
	print bcm_cosq_gport_handle_get (unit, /*bcmCosqGportTypeE2EPort*/ bcmCosqGportTypeE2EPortTC, &gport_info);
	tc_gport=gport_info.out_gport;
	print tc_gport;
	print gport_info;
	flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_DUAL_WFQ   |  BCM_COSQ_GPORT_REPLACE;
	print bcm_cosq_gport_add (unit,null_gport/*GPORT NULL*/,0/*HR PRIO*/,flags,tc_gport);
	print tc_gport;
	print bcm_cosq_gport_add (unit,null_gport/*GPORT NULL*/,1/*HR PRIO*/,flags,tc_gport);
	print tc_gport;
	gport_info.in_gport = tc_gport;
    gport_info.cosq = 0;  //priority 0~7
    rval= bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);
    tc_4_gport_hr_0 = gport_info.out_gport;
    gport_info.in_gport = tc_gport;
    gport_info.cosq = 1;  //priority 0~7
    rval= bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);
    tc_4_gport_hr_1 = gport_info.out_gport; 
	bshell (unit,"gport");
	
	
	
	bshell(unit,"diag cosq print_flow_and_up is_flow=0 dest_id=1");
	bshell(unit,"diag cosq qpair egq ps=1");
	bshell(unit,"diag cosq qpair e2e ps=1");
	bshell (unit,"diag pp last");
	bshell (unit,"m IPS_IPS_GENERAL_CONFIGURATIONS DIS_DEQ_CMDS=1"); //STOP
	bshell (unit,"diag cosq non");
	bshell (unit,"m IPS_IPS_GENERAL_CONFIGURATIONS DIS_DEQ_CMDS=0"); //START

	//////
	

    //Create VoQ Bundle
    flags=BCM_COSQ_GPORT_UCAST_QUEUE_GROUP ;
    print BCM_GPORT_MODPORT_SET(dest_port, 0, 1);
    rval=bcm_cosq_gport_add(unit,  dest_port/*system port gport*/,  8/*Number of Queue*/, flags, &gport_voq_1r8/*gport to VOQ*/);
    print BCM_GPORT_MODPORT_SET(dest_port, 0, 2);
	rval=bcm_cosq_gport_add(unit,  dest_port/*system port gport*/,  8/*Number of Queue*/, flags, &gport_voq_2r8/*gport to VOQ*/);
	print BCM_GPORT_MODPORT_SET(dest_port, 0, 3);
    rval=bcm_cosq_gport_add(unit,  dest_port/*system port gport*/,  8/*Number of Queue*/, flags, &gport_voq_3r8/*gport to VOQ*/);
	bshell(unit,"gport");

    //Create VoQ_Connectors Bundle
    flags=BCM_COSQ_GPORT_VOQ_CONNECTOR ;
    rval=bcm_cosq_gport_add(unit, null_gport /*NULL_gport*/, 8 /*Number of QueueConn*/, flags, &gport_voq_conn_1r8);
    rval=bcm_cosq_gport_add(unit, null_gport /*NULL_gport*/, 8 /*Number of QueueConn*/, flags, &gport_voq_conn_2r8);
    rval=bcm_cosq_gport_add(unit, null_gport /*NULL_gport*/, 8 /*Number of QueueConn*/, flags, &gport_voq_conn_3r8);
	//
	
	/////////
	//Connects VoQ To Ingress VoQ_Connector and to Egress VoQ H

    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = 0;   //egress mid
    connection.voq = gport_voq_1r8;
    connection.voq_connector = gport_voq_conn_1r8;
    rval= bcm_cosq_gport_connection_set(unit, &connection);
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = 0;   //ingress mid
    connection.voq = gport_voq_1r8;
    connection.voq_connector = gport_voq_conn_1r8;
    rval= bcm_cosq_gport_connection_set(unit, &connection);
	//////
	/////////
	//Connects VoQ To Ingress VoQ_Connector and to Egress VoQ H

    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = 0;   //egress mid
    connection.voq = gport_voq_2r8;
    connection.voq_connector = gport_voq_conn_2r8;
    rval= bcm_cosq_gport_connection_set(unit, &connection);
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = 0;   //ingress mid
    connection.voq = gport_voq_2r8;
    connection.voq_connector = gport_voq_conn_2r8;
    rval= bcm_cosq_gport_connection_set(unit, &connection);
	//////
		//Connects VoQ To Ingress VoQ_Connector and to Egress VoQ H

    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = 0;   //egress mid
    connection.voq = gport_voq_3r8;
    connection.voq_connector = gport_voq_conn_3r8;
    rval= bcm_cosq_gport_connection_set(unit, &connection);
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = 0;   //ingress mid
    connection.voq = gport_voq_3r8;
    connection.voq_connector = gport_voq_conn_3r8;
    rval= bcm_cosq_gport_connection_set(unit, &connection);
	//////
	
	// Connects, voq_conn to HR:
	rval = bcm_cosq_gport_sched_set(unit,  gport_voq_conn_1r8,0, 1/*prio*/,0);
	rval = bcm_cosq_gport_attach   (unit,  tc_1_gport_hr_0, gport_voq_conn_1r8, 6/*prio*/);
	
	rval = bcm_cosq_gport_sched_set(unit,  gport_voq_conn_2r8,0, 1/*prio*/,0);
	rval = bcm_cosq_gport_attach   (unit,  tc_2_gport_hr_0, gport_voq_conn_2r8, 6/*prio*/);
	
	rval = bcm_cosq_gport_sched_set(unit,  gport_voq_conn_3r8,0, 1/*prio*/,0);
	rval = bcm_cosq_gport_attach   (unit,  tc_3_gport_hr_0, gport_voq_conn_3r8, 6/*prio*/);

	bshell (unit,"diag pp last ");
	bshell (unit,"l2 show");
	bshell (unit,"dune \"ppd_api frwrd_mact get_block\" ");
		
	bshell(unit,"diag cosq print_flow_and_up is_flow=0 dest_id=1");
	bshell(unit,"diag cosq qpair egq ps=1");
	bshell(unit,"diag cosq qpair e2e ps=1");
	bshell (unit,"diag pp last");
	bshell(unit,"gport");
	bshell (unit,"m IPS_IPS_GENERAL_CONFIGURATIONS DIS_DEQ_CMDS=1"); //STOP
	bshell (unit,"diag cosq non");
	bshell (unit,"m IPS_IPS_GENERAL_CONFIGURATIONS DIS_DEQ_CMDS=0"); //START
	///Connectin Voq_Conn to SE/HR

    print gport_voq_1r8;
    print gport_voq_2r8;
    print gport_voq_3r8;
	print gport_voq_conn_1r8;
    print gport_voq_conn_2r8;
    print gport_voq_conn_3r8;
	
	print tc_1_gport_hr_0;
	print tc_1_gport_hr_1;
	print tc_2_gport_hr_0;
	print tc_2_gport_hr_1;
	print tc_3_gport_hr_0;
	print tc_3_gport_hr_1;
	
	//////////////////RECOVER ///////////////////////////////
	
	trunk_member_delete(1,1);
	trunk_member_delete(1,2);
	trunk_member_delete(1,3);

	trunk_member_add(1,1);
	trunk_member_add(1,2);
	trunk_member_add(1,3);
		
	
////////////////////////////////////////



    int rval=0 ;
    int unit=   0;
    int phy_port = 4;
    int num_tc = 8;
    bcm_gport_t cl2_4sp_h, cl2_4sp_l, cl2_4sp_top_cir, cl2_4sp_top_eir, father, son;
    bcm_gport_t cl2_mode5;
    bcm_gport_t cl2_mode5_top_eir, cl2_mode5_top_cir, gport_cl_dual_cir, gport_cl_dual_pir, gport_cl_dual;
    int onu_id=0;
     flags=0;

    //////Instanciate a CL SE DUALSHAPPER
    flags  = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE4_2SP_WFQ;
	flags |= BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT | BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER;
	    rval = bcm_cosq_gport_add(unit, (auto)NULL, 1/*NA*/, flags, &gport_cl_dual);
    /* CIR, PIR scheduler handles */
    BCM_COSQ_GPORT_SCHED_CIR_CORE_SET(gport_cl_dual_cir/*CIR*/, gport_cl_dual, 0/*core*/);
    BCM_COSQ_GPORT_SCHED_PIR_CORE_SET(gport_cl_dual_pir/*EIR*/, gport_cl_dual, 0);
    /* CIR (CL) scheduler element scheduling discipline configuration */
    rval = bcm_cosq_gport_sched_set(unit, gport_cl_dual_cir,0, BCM_COSQ_SP3/*FQ in SP9 HR-Enanched*/,0);
    /* CIR (CL) scheduler element attach to CL (2nd instance) scheduler */
    rval = bcm_cosq_gport_attach  (unit, tc_4_gport_hr_0, gport_cl_dual_cir, 0);
    /* EIR (FQ) scheduler element scheduling discipline configuration */
    rval = bcm_cosq_gport_sched_set(unit, gport_cl_dual_pir, 0,BCM_COSQ_SP1/*FQ in SP11 HR-Enanched*/,0);
    /* EIR (FQ) scheduler element attach to FQ (1rt instance)scheduler */
    rval = bcm_cosq_gport_attach(unit,  tc_4_gport_hr_1, gport_cl_dual_pir, 0);

	
	
	
	
    rval = bcm_cosq_gport_sched_set(unit, gport_cl_dual_pir, 0,BCM_COSQ_SP1/*FQ in SP11 HR-Enanched*/,0);
    rval = bcm_cosq_gport_attach(unit,  tc_4_gport_hr_1, gport_cl_dual_pir, 0);	
	
	
	bcm_gport_t gport_fq_1, gport_fq_2;
    flags  = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ;
	rval = bcm_cosq_gport_add(unit, (auto)NULL, 1/*NA*/, flags, &gport_fq_1);
	rval = bcm_cosq_gport_add(unit, (auto)NULL, 1/*NA*/, flags, &gport_fq_2);	
	
    
    rval =     bcm_cosq_gport_attach(unit,  gport_cl_dual_pir/*father*/, gport_fq_1 /*son*/ , 0);	
	
	
    rval =     bcm_cosq_gport_attach(unit,  gport_cl_dual_cir/*father*/, gport_fq_2 /*son*/ , 0/*cos level*/);	
	
	
	
	rval = bcm_cosq_gport_sched_set(unit, gport_fq_1/*son*/, 0,0/*FQ in SP11 HR-Enanched*/,0);
	rval =    bcm_cosq_gport_attach(unit, gport_cl_dual_pir/*father*/,gport_fq_1/*son*/, 0);	
	
	
	
	
	
	
	
	
	
	print tc_4_gport_hr_0;
	print tc_4_gport_hr_1;
    print gport_cl_dual;
    print gport_cl_dual_pir;
    print gport_cl_dual_cir;
	print gport_fq_1;
	print gport_fq_2;









	