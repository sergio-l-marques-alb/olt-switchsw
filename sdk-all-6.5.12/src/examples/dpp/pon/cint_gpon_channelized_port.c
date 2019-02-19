/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * Arad+ based GPON16 Application supports 4 GPON PHY ports (0-3). Each GPON PHY port
 * supports 4 channels. The channels are signaled in the GPON TID:
 * |1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|
 * |     reserved        | PCP |Chn| rsvd  |       GEM-ID          |
 * 
 * For compatibility with the existing xPON application, the PP/TM Ports will be offset from the
 * PHY port base by 8. Ports 0-127 are reserved for xPON. Ports 128+ support all other NIF types
 *
 * The following entity TM-Port/PP-Port assignements will result from the GPON configuration. Vlan-Domain must
 * be assigned in the range 0-15.
 *
 * Note: The physical port is not channelized, therefore the Ingress TM port is the TM port of channel 0.
 * In-PP-Port/Out-PP-Port/OTM-Port = GPON-Chan << 3 | PHY-Port
 * Vlan-Domain = Phy-Port << 2 | Phy-Chan
 *
 *                         PP-Port/
 * Phy-port     GPON-Chan  TM-Port    Vlan-Domain
 * -----------  ---------  ---------  -----------
 * 0            0          0          0         
 * 0            1          8          1        
 * 0            2          16         2       
 * 0            3          24         3      
 * 1            0          1          4     
 * 1            1          9          5    
 * 1            2          17         6          
 * 1            3          25         7         
 * 2            0          2          8        
 * 2            1          10         9       
 * 2            2          18         10     
 * 2            3          26         11    
 * 3            0          3          12
 * 3            1          11         13
 * 3            2          19         14
 * 3            3          27         15
 * 
 * In this example, GPON Phy Port 0 is configured with 4 channels.
 *
 * ucode_port_0.BCM88650=10GBase-R13.0
 * ucode_port_8.BCM88650=10GBase-R13.1
 * ucode_port_16.BCM88650=10GBase-R13.2
 * ucode_port_24.BCM88650=10GBase-R13.3
 * pon_application_support_enabled_0.BCM88650=TRUE
 * custom_feature_gpon_application_enable=1
 *
 * NNI ports start at 128:
 * ucode_port_128.BCM88650=10GBase-R12
 *
 * NOTE: config-sand.bcm configures port 0 as a CPU port. When configuring GPON port 0, 
 * be sure to modify the config.bcm file to assign a differnt port to CPU.0
 * For example, instead configure port 220 for CPU.0:
 *    from:
 *        ucode_port_0.BCM88650=CPU.0
 *        tm_port_header_type_in_0.BCM88650=INJECTED_2
 *        tm_port_header_type_out_0.BCM88650=TM
 *    to:
 *        ucode_port_220.BCM88650=CPU.0
 *        tm_port_header_type_in_220.BCM88650=INJECTED_2
 *        tm_port_header_type_out_220.BCM88650=TM
 *
 * To run the example:
 * 
 * cint ../../../../src/examples/dpp/cint_port_tpid.c
 * cint ../../../../src/examples/dpp/pon/cint_pon_utils.c
 * cint ../../../../src/examples/dpp/pon/cint_gpon_channelized_port.c
 * cint 
 * int  unit = 0;
 * bcm_port_t port_gpon = 0;
 * bcm_port_t port_nni = 128;
 * bcm_port_t port_gpon_pp = 16;	/* Phy-Port 0, GPON-Chan 2 */
 * uint8 num_channels = 4;	
 * uint32 tunnel = 4095;
 * uint32 qtag = 100;
 * uint32 stag = 999;
 * 
 * print port_gpon_example(unit, port_gpon, num_channels, port_nni);
 * print port_gpon_service_create(unit, port_gpon_pp, tunnel, qtag, port_nni, stag); /* Creates the specified LIFs and cross-connect */
 * exit;
 *
 */

/* Set PON tunnel profile to ARAD */
int port_gpon_config(int unit, bcm_port_t port_gpon, uint8 num_gpon_channels)
{
    int rv = 0;
    uint8 channel;
    int j;
    uint32 flags;
    bcm_gport_t port_gpon_pp;
    bcm_gport_t port_gpon_tm;
    uint32 port_gpon_vlan_domain;
    uint8 ptc_size;
    ptc_size = (is_device_or_above(unit, JERICHO) ? 4 : 3);

    if (num_gpon_channels > 4) {
        printf("\nThe number of channels must be <= 4");
	return BCM_E_PARAM;
    }

    for (channel = 0; channel < num_gpon_channels; channel++) {

	/* Associate the PP port with the PON channel */
        flags = BCM_PORT_PON_TUNNEL_WITH_ID;
        port_gpon_pp = 0;	/* Only the default profile is needed for each GPON port */
        port_gpon_tm =  channel << ptc_size | port_gpon;
        rv = bcm_port_pon_tunnel_add(unit, port_gpon_tm, flags, &port_gpon_pp);
        if (rv != BCM_E_NONE)
        {    
            printf("Error, bcm_port_pon_tunnel_add!\n");
            print rv;
            return rv;
        }
        
        /* Map the {Phy-Port, TID:GPON-Port(2)} to the PP port */
        rv = bcm_port_pon_tunnel_map_set(unit, port_gpon, channel, port_gpon_pp);
        if (rv != BCM_E_NONE)
        {    
            printf("Error, bcm_port_pon_tunnel_map_set!\n");
            print rv;
            return rv;
        } 

        rv = pon_tpid_profile_init(unit, port_gpon_pp);
	if (rv != BCM_E_NONE)
	{
            printf("Error, pon_tpid_profile_init!\n");
            print rv;
            return rv;
	}
        if (soc_property_get(unit , "bcm886xx_vlan_translate_mode",0)) {
            /* In case of port tpid_profile != 0, need to init a VLAN action
             * to modify the tpid_profile of IVE to the correct vlaue
             */
            if ((outer_tpid != 0x8100) ||( (outer_tpid == 0x8100) && (inner_tpid != 0x9100))) {
                ing_vlan_action_only_modify_tpid_init(unit);
        }
        egr_vlan_action_no_op_init(unit);
        }

        /* Set PON PP PORT VLAN Domain */
	port_gpon_vlan_domain = port_gpon_pp;
        bcm_port_class_set(unit, port_gpon_pp, bcmPortClassId, port_gpon_vlan_domain);

        bcm_vlan_control_port_set(unit, port_gpon_pp, bcmVlanPortLookupTunnelEnable, 1);

	printf("\nMapped GPON phy port %d, channel %d to TM port %d, PP port %d, vlan-domain %d",port_gpon, channel, port_gpon_tm, port_gpon_pp, port_gpon_vlan_domain);

    }
    
    return rv;
}

int port_gpon_example(int unit, bcm_port_t port_gpon, uint8 num_channels, bcm_port_t port_nni)
{
    int rv;

    pon_app_init(unit, port_gpon, port_nni, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_app_init!\n");
        print rv;
        return rv;
    }

    port_gpon_config(unit, port_gpon, num_channels);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_gpon_config!\n");
        print rv;
        return rv;
    }
}

int port_gpon_service_create(int unit, 
			     bcm_port_t port_gpon_pp, 
			     uint32 tunnel,
			     uint32 qtag,
			     bcm_port_t port_nni,
			     uint32 stag)
{

    bcm_gport_t gpon_vlan_port_id, nni_vlan_port_id;
    bcm_if_t encap_id;
    bcm_vlan_t vsi = 5000;

    int rv = BCM_E_NONE;

    /* 
     * Create GPON AC-LIF
     * Service classification: {GPON-Port(4), GEM-ID(12), QTAG(12)}
     * GPON-Port is obtained from the Vlan Domain assigned to the GPON PP Port
     */
    gpon_vlan_port_id = 0;	/* Allow the SDK to allocate the LIF-Id */
    rv =  pon_lif_create(unit, port_gpon_pp, match_otag, 0, 0, tunnel, tunnel, qtag, 0, 0, 0, &gpon_vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_lif_create!\n");
        print rv;
        return rv;
    }
    else {
        printf("Created gpon vlan-port 0x%08x pp-port %d tunnel %d qtag %d\n", 
        gpon_vlan_port_id,
	port_gpon_pp,
	tunnel,
	qtag);
    }

    /*
     * Ingress Vlan Editing (IVE)
     * Replace outer tag
     */
    rv =  pon_port_ingress_vt_set(unit, otag_to_otag2, stag, 0, gpon_vlan_port_id, 0, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_port_ingress_vt_set!\n");
        print rv;
        return rv;
    }
    else {
        printf("Configured IVE gpon vlan-port 0x%08x stag %d\n", 
        gpon_vlan_port_id,
	stag);
    }

    rv = pon_port_egress_vt_set(unit, otag_to_otag2, tunnel, qtag, 0, gpon_vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_port_egress_vt_set!\n");
        print rv;
        return rv;
    }
    else {
        printf("Configured EVE gpon vlan-port 0x%08x tunnel %d qtag %d\n", 
        gpon_vlan_port_id,
	tunnel, 
	qtag);
    }

    /*
     * Create NNI AC-LIF
     */
     nni_vlan_port_id = 0;	/* Allow the SDK to allocate the LIF-Id */
     rv = nni_lif_create(unit, port_nni, match_otag, 0, stag, 0, &nni_vlan_port_id, &encap_id);
     if (rv != BCM_E_NONE) {
         printf("Error, pon_lif_create!\n");
         print rv;
         return rv;
     }
     else {
         printf("Created nni vlan-port 0x%08x phy-port %d stag %d\n", 
	 	nni_vlan_port_id, 
		port_nni, stag);
       }

    rv = bcm_vswitch_create_with_id(0, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create_with_id!\n");
        print rv;
        return rv;
    } else {
        printf("Create vswitch %d\n", vsi);
    }


    rv = bcm_vswitch_port_add(unit, vsi, gpon_vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add!\n");
        print rv;
        return rv;
    } else {
        printf("Added gpon_vlan_port_id 0x%08x to vsi %d \n", gpon_vlan_port_id, vsi);
    }

    rv = bcm_vswitch_port_add(unit, vsi, nni_vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add!\n");
        print rv;
        return rv;
    } else {
        printf("Added nni_vlan_port_id 0x%08x to vsi %d \n", gpon_vlan_port_id, vsi);
    }

    return rv;
}

