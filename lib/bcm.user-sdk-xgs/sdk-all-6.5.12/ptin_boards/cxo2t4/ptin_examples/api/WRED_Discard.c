/*
	WRED_Discard.c
	Implemention for TailDrop a WRED Cofiguration
	António 11/07/2017

*/

enum device_type_t {
    DEVICE_TYPE_ARAD=0,
    DEVICE_TYPE_JERICHO=1,
    DEVICE_TYPE_QAX=2,
    DEVICE_TYPE_QUX=3
};
device_type_t device_type;



/*! \Configure tail Drop Settings. 
 *
 *    This API Configure tail Drop Settings. It is a Local Function 
 *    
 *  @param unit         	Unit Number.
 *  @param device_type 		Device Type, from device_type_t Enum
 *  @return 				BCM_PM_IF_SUCCESS(0) for success and corresponding error code on failure.
 */
int get_device_type(int unit, device_type_t *device_type)
{
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (BCM_FAILURE(rv))
	{
        printf("Error in bcm_info_get, rv=%d\n", rv);
        return rv;
    }

    if ( info.device == 0x8270 )
	{
        *device_type = DEVICE_TYPE_QUX;
		printf("DEVICE_TYPE_QUX\n");
    }
    else if ( info.device == 0x8470 )
	{
        *device_type = DEVICE_TYPE_QAX;
		printf("DEVICE_TYPE_QAX\n");
    }
	else if ( (info.device == 0x8675) || (info.device == 0x8375)|| (info.device == 0x8680) )
	{
        *device_type = DEVICE_TYPE_JERICHO;
		printf("DEVICE_TYPE_JERICHO\n");
    }
	else
	{
        *device_type = DEVICE_TYPE_ARAD;
		printf("DEVICE_TYPE_ARAD\n");
    }

    return rv;
}

/*! \Configure tail Drop Settings. 
 *
 *    This API Configure tail Drop Settings. It is a Local Function 
 *    
 *  @param unit         	Unit Number.
 *  @param gport       		VoQ Bundle Adress.
 *  @param flags			Flags to configure "bcm_cosq_gport_color_size_set".
 *  @param dp				  Drop Precedence
 *							-1 Set Clolor Blind "BCM_COSQ_GPORT_SIZE_COLOR_BLIND"
 *							 0 Green
 *							 1 Yellow
 *							 2 ??
 *						     3 ??
 *  @param cosq              Cos Queue to whitch is set Tail Drop, in VoQ Bundle.
 *  @param max_size			Queue Depth Value, where Tail Drop is effective	
 *  @return 				BCM_PM_IF_SUCCESS(0) for success and corresponding error code on failure.
 */
static int set_tail_drop_settings(int unit, bcm_gport_t gport, uint32 flags, int dp, int cosq, uint32 max_size)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_size_t gport_size;

    if (dp == -1)
	{
        flags |= BCM_COSQ_GPORT_SIZE_COLOR_BLIND;
        dp = 0;
    }

    gport_size.size_max = max_size;
    rv = bcm_cosq_gport_color_size_set(unit,gport,cosq,dp,flags,&gport_size);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_gport_color_size_set failed: (%s) \n",bcm_errmsg(rv));
		printf("unit %d, gport 0x%x, flags 0x%x, dp=%d cosq=%d max_size %d \n",unit, gport, flags, dp, cosq, max_size);
        return rv;
    }
	printf("Set Tail drop Settings unit %d, gport 0x%x, flags 0x%x, dp=%d cosq=%d max_size=%d \n",
	unit, gport, flags, dp, cosq, max_size);

    return rv;
}



/*! \Set Parameters to Configure WRED. 
 *
 *    This API Set Parameters to Configure WRED, regarding to VoQ Base address, Cos Queue and DropPrecedence Color.
 *    
 *  @param unit         	Unit Number.
 *  @param base_queue       VoQ Base of the Bundle
 *  @param cosq              Cos Queue to whitch is set Tail Drop, in VoQ Bundle.
 *  @param drop_precedence  Drop Precedence
 *							 0 Green
 *							 1 Yellow
 *							 2 Red
 *						     3 Black
							 4 All
 *  @param gain				Smoothing Parameters applied to drop Probability
 *  @param is_enable		1- to Enable WRED Discard Packets
 *  @param min_thresh		Queue Depth, to beguin drop Packets
 *  @param max_thresh  		Queue Depth, to drop All Packets.
 *							When Queue size is bigger than "max_thresh", the drop applied is 100%.
 *  @param max_prob			Drop Probability at "max_thresh".
 *							If 0, thers is not drop Packets at any Threshold
 *  @return					BCM_PM_IF_SUCCESS(0) for success and corresponding error code on failure.
 */
static int ptin_set_wred(int unit, bcm_gport_t base_queue/*32*/,int gain,int cosq,int drop_precedence,int is_enable,int min_thresh,int max_thresh,int max_prob  )
{
    bcm_error_t rv;
    uint32 flags;
    int nof_rsrc = 0;
    uint32 rsrc_flags[3] = {0};
    uint32 rsrc_max_queue_size[3] = {0};
    int dp = 0;
    int i = 0;
	bcm_cosq_gport_discard_t discard;
    int max_queue_size;

	if(max_thresh*1.2>2000000000)
	{
        printf("max_thresh will be >2000000000\n");
        return -1;
	}
	
    rv = get_device_type(unit, &device_type);
    if (BCM_FAILURE(rv))
	{
        printf("get_device_type failed: (%s) \n",bcm_errmsg(rv));
    }

    /* 1. Set vsq categories */
	// If working with VSQ
	
	
	//But even for VOQ:
	//bcmFabricQueueMin => Start Index Fabric VOQs
    rv = bcm_fabric_control_set(unit,bcmFabricQueueMin,0/*Queue ID*/);
    if (rv != BCM_E_NONE)
	{
        printf("error in bcm_fabric_control_set type: bcmFabricQueueMin\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
		//bcmFabricQueueMax => END Index Fabric VOQs
    rv = bcm_fabric_control_set(unit,bcmFabricQueueMax,(device_type == DEVICE_TYPE_QUX)?16383:32767);
    if (rv != BCM_E_NONE)
	{
        printf("error in bcm_fabric_control_set type: bcmFabricQueueMax\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    bcm_cosq_gport_discard_t_init(&discard);
    if (is_enable == 1) discard.flags |= BCM_COSQ_DISCARD_ENABLE;
	discard.flags |= BCM_COSQ_DISCARD_BYTES;
    switch (drop_precedence)
	{
		case 0:
			discard.flags |= BCM_COSQ_DISCARD_COLOR_GREEN;
		break;
		case 1:
			discard.flags |= BCM_COSQ_DISCARD_COLOR_YELLOW;
		break;
		case 2:
			discard.flags |= BCM_COSQ_DISCARD_COLOR_RED;
		break;
		case 3:
			discard.flags |= BCM_COSQ_DISCARD_COLOR_BLACK;
		break;
		default:
			discard.flags |= BCM_COSQ_DISCARD_COLOR_ALL;
		break;
    }

    discard.min_thresh = min_thresh;
    discard.max_thresh = max_thresh;
    discard.drop_probability = max_prob;
    discard.gain = gain;
	discard.refresh_time = 8;
	//print  discard;
    /* For VSQ gport , cosq parameter always 0 */
    rv = bcm_cosq_gport_discard_set(unit,base_queue,cosq,&discard);
    if (rv != BCM_E_NONE)
	{
        printf("bcm_cosq_gport_discard_set failed: (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* 4. Set enable for admission test profile A,B */
	// If working with VSQ
	//No Tail Drop
	
    /* 5. disable tail drop settings (by set them to high thresholds) so it will be affected by wred only. */
    if ((device_type == DEVICE_TYPE_QAX) || (device_type == DEVICE_TYPE_QUX))
	{
        nof_rsrc = 3;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BYTES;
        rsrc_flags[1] = BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BUFFER_DESC;
        rsrc_flags[2] = BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC;
    }
	else if (device_type == DEVICE_TYPE_JERICHO)
	{
        nof_rsrc = 1;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BUFFER_DESC;
    }
	else
	{
		//ARAD
        nof_rsrc = 2;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BYTES;
        rsrc_flags[1] = BCM_COSQ_GPORT_SIZE_BUFFER_DESC;
		rsrc_max_queue_size[0] = max_thresh*1.2;//The high Threshold set set to 1.2 times the WRED Depth value "max_thresh"
		if (rsrc_max_queue_size[0]>10000) rsrc_max_queue_size[1]=10000;
        else rsrc_max_queue_size[1] = max_thresh;
    }
    for (i = 0; i < nof_rsrc; ++i)
	{
        flags = rsrc_flags[i];
        max_queue_size = rsrc_max_queue_size[i];
        rv = set_tail_drop_settings(unit,base_queue,flags,-1/*dp*//*Color Blind*/,cosq/*cosq*/,max_queue_size);
        if (rv != BCM_E_NONE)
		{
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
	printf("Set WRED Settings unit %d, vsq 0x%x, is_enable %d, drop_precedence %d, max thresh %d, min thresh %d, max prob %d gain %d cosq %d.\n",
	unit,base_queue,is_enable,drop_precedence,max_thresh,min_thresh,max_prob,gain,cosq);
	print min_thresh;
	/* 6. Run traffic */
   return rv;
}
 

/*! \Set Parameters to Configure Tail Drop. 
 *
 *    This API Set Parameters to Configure Tail Drop, regarding to VoQ Base address, Cos Queue and DropPrecedence Color.
 *    
 *  @param unit         	Unit Number.
 *  @param base_queue       VoQ Base of the Bundle
 *  @param dp               Drop Precedence
 *							-1 Drop Blind
 *							 0 Green
 *							 1 Yellow
 *							 2 Red
 *						     3 Black
 *  @param cosq              Cos Queue to whitch is set Tail Drop, in VoQ Bundle
 *  @param max_thresh  		 Queue Depth Value, where Tail Drop is effective	
 *
 *  @return					BCM_PM_IF_SUCCESS(0) for success and corresponding error code on failure
 */
int ptin_set_tail_drop(int unit,bcm_gport_t base_queue, int dp /* DropPrecedence-1 All*/, int cosq, uint32 max_thresh)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 max_queue_size,flags;
    int index;
    bcm_cosq_vsq_info_t vsq_info;
    int nof_rsrc = 0;
    uint32 rsrc_flags[3] = {0};
    uint32 rsrc_max_queue_size[3] = {0};
    int i = 0;

	if(max_thresh*1.2>2000000000)
	{
        printf("max_thresh will be >2000000000\n");
        return -1;
	}
	
    rv = get_device_type(unit, &device_type);
    if (BCM_FAILURE(rv))
	{
        printf("get_device_type failed: (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* 1. Set category ranges: this is done by: declaring a vsq category mode and ranges */
	// If working with VSQ
	
	//But even for VOQ:
    rv = bcm_fabric_control_set(unit,bcmFabricQueueMin,0);
    if (rv != BCM_E_NONE)
	{
        printf("error in bcm_fabric_control_set type: bcmFabricQueueMin\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_fabric_control_set(unit,bcmFabricQueueMax,(device_type == DEVICE_TYPE_QUX)?16383:32767);
    if (rv != BCM_E_NONE)
	{
        printf("error in bcm_fabric_control_set type: bcmFabricQueueMax\n");
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
	
    /* 4. Set tail drop settings. */
    /* An example of tail drop settings */
    /* vsqA will have more packets dropped than vsqB (TC 2/3) */
    if ((device_type == DEVICE_TYPE_QAX) || (device_type == DEVICE_TYPE_QUX))
	{
        nof_rsrc = 3;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BYTES;
        rsrc_flags[1] = BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_BUFFER_DESC;
        rsrc_flags[2] = BCM_COSQ_GPORT_SIZE_SRAM | BCM_COSQ_GPORT_SIZE_PACKET_DESC;
        rsrc_max_queue_size[0] = 500;
        rsrc_max_queue_size[1] = 3;
        rsrc_max_queue_size[2] = 3;
    }
	else if (device_type == DEVICE_TYPE_JERICHO)
	{
        nof_rsrc = 1;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BUFFER_DESC;
        rsrc_max_queue_size[0] = 10;
    }
	else
	{
		//ARAD
        nof_rsrc = 2;
        rsrc_flags[0] = BCM_COSQ_GPORT_SIZE_BYTES;
        rsrc_flags[1] = BCM_COSQ_GPORT_SIZE_BUFFER_DESC;
		rsrc_max_queue_size[0] = max_thresh;//max_thresh*1000;
		if ((rsrc_max_queue_size[0]/50)>1500000) rsrc_max_queue_size[1]=1500000;
        else rsrc_max_queue_size[1] = max_thresh/50;
    }
    for (i = 0; i < nof_rsrc; ++i)
	{
        flags = rsrc_flags[i];
        max_queue_size = rsrc_max_queue_size[i];
        rv = set_tail_drop_settings(unit, base_queue, flags, dp,cosq,max_queue_size);
        if (BCM_FAILURE(rv))
		{
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
	
	/* 5. Set enable for admission test profile A,B */
	//If working with VSQ and For ARAD+ or Jericho
	//////////////////DISABLE WRED!!
	bcm_cosq_gport_discard_t discard;
	bcm_cosq_gport_discard_t_init(&discard);
	discard.flags |= BCM_COSQ_DISCARD_BYTES;
    discard.flags |= BCM_COSQ_DISCARD_COLOR_ALL;
    discard.min_thresh = 0;
    discard.max_thresh = 0;
    discard.drop_probability = 0;
    discard.gain = 0;
    /* For VSQ gport , cosq parameter always 0 */
    bcm_cosq_gport_discard_set(unit,base_queue,cosq,&discard);
    if (rv != BCM_E_NONE)
	{
        printf("bcm_cosq_gport_discard_set failed: (%s) \n",bcm_errmsg(rv));
        return rv;
    }
	//////////////////DISABLE WRED!!
    /* 6. Run traffic to queues */
    return rv;
}
  
/*! \Show Debug Regarding L2 Packe Flow. 
 *
 *    This API Show Debug Regarding L2 Packe Flow. 
 *    
 *  @param unit         	Unit Number.
 *  @param opt       		Not Implementd. For Future Use.
 *  @return					BCM_PM_IF_SUCCESS(0) and Print Out from Broadcom's Debug Functions
 */
 int ptin_l2_packet_flow_debug(int unit, int opt)
 {
	bshell (unit,"l2 show");
	bshell (unit,"vlan show");
	bshell (unit,"dune \"ppd_api frwrd_mact get_block\" ");
	bshell (unit,"diag pp last");
	bshell (unit,"diag pp fdt");
	bshell (unit,"diag count g");
	bshell (unit,"diag cosq non"); 
	return 0;
 }
 
 /*! \Show Debug Regarding Queue Bundle Buffers Size. 
 *
 *    This API Show Debug Regarding Queue Bundle Buffers Size.
 *    It only prints values:
 *      -> Queue Sizes != 0 AND Queue Sizes != 1048576 (Defaul QSizes) 
 *      -> drop_probability != 0 And BCM_COSQ_DISCARD_ENABLE flag Set
 *    
 *  @param unit         	Unit Number.
 *  @param opt       		Not Implementd. For Future Use.
 *  @param gportt			oQ Bundle's Gport Adsress.
 *  @return					BCM_PM_IF_SUCCESS(0) and Print Out from Broadcom's Debug Functions
 */
 int ptin_Queue_Bundle_debug(int unit, bcm_gport_t gportt, int opt)
 {
  	int tt;
	int tc;
	bcm_cosq_gport_size_t Qsize;
	bcm_cosq_gport_discard_t discard;
	printf ("=== Q-Size vs Color (Bytes) Info ===\n");
	for (tc=0; tc<8; tc++)
	{	
		for (tt=0; tt<4; tt++)
		{		
			bcm_cosq_gport_color_size_get(unit, gportt/*VoQ Bundle's Gport*/, tc /*CoS in VoQ Bundle*/, tt /*color*/,  BCM_COSQ_GPORT_SIZE_BYTES/*flags*/, &Qsize/*bcm_cosq_gport_size_t*/ );
			if ((Qsize.size_max!=0) && (Qsize.size_max!=1048576)) printf ("Gport:0x%x TC:%d Color:%d QSize MAX:%d\n", gportt, tc,tt,Qsize.size_max);
			if ((Qsize.size_min!=0) && (Qsize.size_min!=1048576)) printf ("Gport:0x%x TC:%d Color:%d QSize MIN:%d\n", gportt, tc,tt,Qsize.size_min);
		}
	}
	for (tc=0; tc<8; tc++)
	{	
		for (tt=0; tt<4; tt++)
		{		
			bcm_cosq_gport_color_size_get(unit, gportt/*0x24000020*//*P1*/ /*VoQ BundleGport*/, tc /*CoS in VoQ Bundle*/, tt /*color*/,  BCM_COSQ_GPORT_SIZE_BUFFER_DESC/*flags*/, &Qsize/*bcm_cosq_gport_size_t*/ );
			if ((Qsize.size_max!=0) && (Qsize.size_max!=1048576)) printf ("Gport:0x%x TC:%d Color:%d Buffer_Descriptor MAX Size:%d\n", gportt, tc,tt,Qsize.size_max);
			if ((Qsize.size_min!=0) && (Qsize.size_min!=1048576)) printf ("Gport:0x%x TC:%d Color:%d Buffer_Descriptor MIN Size:%d\n", gportt, tc,tt,Qsize.size_min);
		}
	}
	
 	printf ("===Discard Info ===\n");
	for (tc=0; tc<8; tc++)
	{
		for (tt=0; tt<4; tt++)
		{	
			bcm_cosq_gport_discard_t_init(&discard);
			discard.flags |= BCM_COSQ_DISCARD_ENABLE;
			discard.flags |= BCM_COSQ_DISCARD_BYTES;
			switch (tt)
			{
				case 0:
				discard.flags |= BCM_COSQ_DISCARD_COLOR_GREEN;
				break;
				case 1:
					discard.flags |= BCM_COSQ_DISCARD_COLOR_YELLOW;
				break;
				case 2:
					discard.flags |= BCM_COSQ_DISCARD_COLOR_RED;
				break;
				case 3:
					discard.flags |= BCM_COSQ_DISCARD_COLOR_BLACK;
				break;
				default:
					discard.flags |= BCM_COSQ_DISCARD_COLOR_ALL;
				break;
			}
			bcm_cosq_gport_discard_get(unit,gportt,tc,&discard);
			if ((discard.drop_probability!=0) && (discard.flags & BCM_COSQ_DISCARD_ENABLE))
			{
				printf ("GET: Gport:0x%x Color:%d tc:%d  ==It is in Wred Mode==\n", gportt, tt, tc);
				print discard;
			}
		}
	}
	bshell (unit,"diag cosq non");
 }


printf ("== Use of WRED and Tail Drop Function ==\n");
printf ("ptin_set_tail_drop(0,0x241c0020 /*VoQ Bundle Address*/, -1 /* dropPrecendence -1-ColorBlind*/, 6 /*Cosq*/, 10000/*QSize*/); \n");
printf ("ptin_set_wred     (0,0x241c0020 /*VoQ Bundle Address*/, 1/*Gain*/,6/*cosq*/,0/*DP Color*/,1/*is_enable*/, 100/*Byte Min*/,1500000/*Byte MAx*/,80 /*_drop*/); \n");
printf ("ptin_l2_packet_flow_debug(0,0);\n");
printf ("ptin_Queue_Bundle_debug(0,0x241c0020,0);\n");


 /*
 cint
  #include trunk_for_Wred_Tests_v2.c
  #include cint_Wred_TailDrop_VSQ.c
 
 
  cint
  #include trunk_for_Wred_Tests_v2.c
  #include cint_Wred_TailDrop_VSQ_v3.c
  
 cint trunk_4_Wred_tests_1.c
 cint Wred_TailDrp_Vsq_1.c
 

*/
 
  
 
 
 
 
 
