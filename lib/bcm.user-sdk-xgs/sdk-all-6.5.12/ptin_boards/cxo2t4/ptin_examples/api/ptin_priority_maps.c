/*
	ptin_priority_maps.c
	Implemention for Pck-Prio<->Internal-Prio
	António 11/07/2017

*/

typedef struct ptin_qos_map_t
{
    char l2_int_prio[16];
    char l2_int_color[16]; //0-bcmColorGreen //1-bcmColorYellow //2-bcmColorRed
    char l2_pckt_prio[16];
    char l2_pckt_cfi[16];
};

ptin_qos_map_t qos_map;

/*! \ Map Pck-Prio to Internal-Prio
 *
 *    Map Pck-Prio to Internal-Prio
 *		
 *  @param unit         	Unit Number.
 *  @param *qos_map			pointer to map Pck-Prio to Internal-Prio
 *  @param map_create_flags Flags to "bcm_qos_map_create"
 *  @param map_add_flags    Flags to "bcm_qos_map_add"
 *  @param *map_id          Map Id. 
 *  @return 				BCM_PM_IF_SUCCESS(0) for success and corresponding error code on failure.
 */
int ptin_qos_map_create(int unit, ptin_qos_map_t *qos_map, int map_create_flags,int map_add_flags, int *map_id)
{
    int rval, idx;
    bcm_qos_map_t l2_qos_map;
    rval=bcm_qos_map_create(unit,  map_create_flags, map_id);
    bcm_qos_map_t_init(&l2_qos_map);

    for (idx=0; idx<16; idx++)
    {
        l2_qos_map.pkt_pri = qos_map->l2_pckt_prio[idx];
        l2_qos_map.pkt_cfi = qos_map->l2_pckt_cfi[idx];
        l2_qos_map.int_pri = qos_map->l2_int_prio[idx];
        switch(qos_map->l2_int_color[idx])
        {
        case 0:
            l2_qos_map.color=bcmColorGreen;
            break;
        case 1:
            l2_qos_map.color=bcmColorYellow;
            break;
        case 2:
            l2_qos_map.color=bcmColorRed;
            break;
        default:
            l2_qos_map.color=bcmColorRed;
            break;
        }
        rval=bcm_qos_map_add(unit, map_add_flags, &l2_qos_map, *map_id);
        if (BCM_FAILURE(rval))
        {
            printf("Error bcm_qos_map_add: unit %d. Error Message %s.\n", unit, bcm_errmsg(rval));
            return rval;
        }
    }
    return rval;
}




/*! \ Map Internal-Prio to Lif's Output VoQ-TC.
 *
 *    Map Internal-Prio to Lif's Output VoQ-TC.
 *		
 *  @param unit         	Unit Number.
 *  @param OutLif			OutLif Gport
 *  @param *qos_map			pointer to map Internal-Prio to TC in Lif's Output Bundle
 *  @return 				BCM_PM_IF_SUCCESS(0) for success and corresponding error code on failure.
 */
int ptin_map_LIF_IntPrio_2_VoQBundle(int unit, bcm_gport_t OutLif, ptin_qos_map_t *qos_map)
{
    int idx;
    int  num_tc=8;
    int rval=0;
    for (  idx=0; idx<num_tc; idx++)
    {
        rval=bcm_cosq_port_mapping_set(unit,OutLif, qos_map->l2_int_prio[idx],qos_map->l2_pckt_prio[idx]);
        if (BCM_FAILURE(rval))
        {
            printf("Error: unit %d, OutLif 0x%8x. Error Message %s\n", unit, OutLif, bcm_errmsg(rval));
			printf("Error: (%s) \n",bcm_errmsg(rval));
			return (rval);
		}        
    }
    return rval;
}





/*! \Configure Switch to Port to be aware of CFI bit in Packet.
 *
 *    Configure Switch to Port to be aware of CFI bit in Packet.
 *		Be aware if Qumran xX
 *    
 *  @param unit         	Unit Number.
 *  @return 				BCM_PM_IF_SUCCESS(0) for success and corresponding error code on failure.
 */
int ptin_Map_Switch_Port_Conf(int unit)
{

    int idx, rv;
	
	//Setting only 12 Frontal Ports!
	//It must be changed to other CHIP
    for (idx=1; idx<=12; idx++)
    {
        //Define OuterVlan as the field to read CFI bit!
        rv = bcm_switch_control_port_set(unit, idx/*Port*/,  bcmSwitchColorSelect, BCM_COLOR_OUTER_CFI);
		if (BCM_FAILURE(rv))
		{
			printf("Error: (%s) \n",bcm_errmsg(rv));
			return (rv);
		}
    }
    rv = bcm_port_cfi_color_set(unit,-1,0,bcmColorGreen); //if Pckt-CFI =0 -> Int-Color=Green
	if (BCM_FAILURE(rv))
	{
		printf("Error: (%s) \n",bcm_errmsg(rv));
		return (rv);
	}
    rv = bcm_port_cfi_color_set(unit,-1,1,bcmColorYellow);//if Pckt-CFI =1 -> Int-Color=Yellow
	if (BCM_FAILURE(rv))
	{
		printf("Error: (%s) \n",bcm_errmsg(rv));
		return (rv);
	}
	return (rv);
}




//#include "ptin_priority_maps.c"
//ptin_Map_Switch_Port_Conf();
/*
char l2_pckt_prio[16] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};
char l2_pckt_cfi[16] =  {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
char l2_int_prio[16] =  {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};
char l2_int_color[16] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
ptin_qos_map_t qos_map_Linear;
sal_memcpy (qos_map_Linear.l2_pckt_prio,l2_pckt_prio,16);
sal_memcpy (qos_map_Linear.l2_pckt_cfi,  l2_pckt_cfi,16);
sal_memcpy (qos_map_Linear.l2_int_prio,  l2_int_prio,16);
sal_memcpy (qos_map_Linear.l2_int_color,l2_int_color,16);
*/
//int ingress_map_id;
//print  ptin_qos_map_create(0, &qos_map_Linear, BCM_QOS_MAP_INGRESS,BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG, &ingress_map_id);
//print bcm_qos_port_map_set(0/*unit*/, InLif_1/*LIF*/, ingress_map_id /*ingress maping*/, -1 /*egress mapping*/);

/*
bshell (unit,"diag cosq non");
ptin_qos_map_t LIF_2_VoQ_map_linear;
char VoQ_map_pckt_prio[16] = {0, 1, 2, 3, 4, 5, 6, 7,     0, 0, 0, 0, 0, 0, 0, 0};
char VoQ_map_int_prio[16] =  {0, 1, 2, 3, 4, 5, 6, 7,     0, 0, 0, 0, 0, 0, 0, 0};
sal_memcpy (LIF_2_VoQ_map_linear.l2_pckt_prio,VoQ_map_pckt_prio,16);
sal_memcpy (LIF_2_VoQ_map_linear.l2_int_prio,  VoQ_map_int_prio,16);
print  ptin_map_LIF_IntPrio_2_VoQBundle(0, onu0_VoQBase,&LIF_2_VoQ_map_linear);
bshell (unit,"diag cosq non");
*/




/////////////////!!
