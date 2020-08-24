/*
 * $Id: cint_load_balancing_basic.c
 */
/*
 * This cint provides a basic example of how to use the load balancing simulator.
 */


int MAX_SIGNAL_NAME_LENGTH = 64;
int MAX_STAGE_NAME_LENGTH = 16;
int MAX_BLOCK_NAME_LENGTH = 8;

/*
 * Global structure that holds the packet headers information (see structure description for more information) .
 */
load_blancing_sim_headers_info_t headers_info;
int is_adapter = 0;

/**
 * \brief
 * This is a basic example on how to use the load balancing simulator.
 *
 * \retval Zero if no error was detected
 * \retval Negative if error was detected.
 */
int
load_balancing_sim_example()
{
    int rv = BCM_E_NONE;
    uint32 packet_size = 68;
   /*
    *   The input packet from which the load balancing values will be calculated
    *    Ethernet:
    *        DA=00:0c:00:02:00:00, SA=00:00:07:00:01:00
    *        Protocol=0x8100, UserPriority=2, VLAN=1, Type=0x800
    *    IPv4:
    *        SIP= 192.128.1.1
    *        DIP= 127.255.255.02
    */

    uint8 packet_header[68] = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x01, 0x00, 0x08, 0x00, 0x45, 0x00,
              0x00, 0x35, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0xfa, 0x45, 0xc0, 0x80, 0x01, 0x01, 0x7f, 0xff,
              0xff, 0x02, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
              0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x00 };

    /*
     * Header offsets
     * Indicate where each header in the packet begins and the header type
     */
    load_blancing_sim_headers_info_t pkt_headers_info;
    pkt_headers_info.nof_header_offsets = 2;
    pkt_headers_info.forwarding_layer = 1;
    pkt_headers_info.header_offsets[0].offset = 0;
    pkt_headers_info.header_offsets[0].header_type = LB_SIM_HEADER_ETH;
    pkt_headers_info.header_offsets[1].offset = 112;
    pkt_headers_info.header_offsets[1].header_type = LB_SIM_HEADER_IPV4;

    /*
     * HW configuration
     * The relevant load balancing HW values which were extracted from the device
     */
    load_blancing_sim_hw_cfg_t load_blancing_sim_hw_cfg;

    load_blancing_sim_hw_cfg.device = DEVICE_TYPE_JERICHO2_B0;

    load_blancing_sim_hw_cfg.parser_seed = 0x00000000;
    load_blancing_sim_hw_cfg.clients_polynomial_seeds[0] = 0;
    load_blancing_sim_hw_cfg.clients_polynomial_seeds[1] = 0;
    load_blancing_sim_hw_cfg.clients_polynomial_seeds[2] = 0;
    load_blancing_sim_hw_cfg.clients_polynomial_seeds[3] = 0;
    load_blancing_sim_hw_cfg.clients_polynomial_seeds[4] = 0;

    load_blancing_sim_hw_cfg.clients_polynomial[0]= 0x015d;
    load_blancing_sim_hw_cfg.clients_polynomial[1]= 0x00d7;
    load_blancing_sim_hw_cfg.clients_polynomial[2]= 0x0039;
    load_blancing_sim_hw_cfg.clients_polynomial[3]= 0x0ac5;
    load_blancing_sim_hw_cfg.clients_polynomial[4]= 0x09e7;

    load_blancing_sim_hw_cfg.clients_16_crc_selection[0] = 0x00;
    load_blancing_sim_hw_cfg.clients_16_crc_selection[1] = 0x00;
    load_blancing_sim_hw_cfg.clients_16_crc_selection[2] = 0x00;
    load_blancing_sim_hw_cfg.clients_16_crc_selection[3] = 0x00;
    load_blancing_sim_hw_cfg.clients_16_crc_selection[4] = 0x00;



    uint32 lb_keys[5];

    rv = load_balancing_sim_main(packet_header,packet_size, &pkt_headers_info, &load_blancing_sim_hw_cfg, lb_keys);
    if(rv != 0)
    {
        printf("load_balancing_sim_main failed \n");
    }

    int key;
    for(key =0; key < 5; key++)
    {
        printf("key %d is 0x%x\n",key,lb_keys[key]);
    }

    return rv;
}


/**
 * \brief
 *  Validate that a sent packet load balancing keys values are matching the load balancing simulator values
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   The core that the packet entered.
 * \retval Zero if no error was detected
 * \retval Negative if error was detected.
 */
int
load_balancing_sim_validate(int unit, int core)
{
    int rv= BCM_E_NONE;
    int iter;
    int byte_count = 0;
    int packet_header_size_in_uint32 = 36;
    uint32 flipped_packet_header[36];
    uint8 packet_header[256];
    uint32 ecmp_keys[2];
    uint32 lag_key;
    uint32 nwk_key;


    char* block_name = "IRPP";
    char* key_from_name = "FWD1";
    char* key_to_name = "FWD2";
    char* fwd_index_from_name = "VTT5";
    char* fwd_index_to_name = "FWD1";
    char* packet_signal_name = "header";
    char* ecmp_key_signal_name = "ECMP_LB_Keys";
    char* lag_key_signal_name = "LAG_LB_Key";
    char* nwk_key_signal_name = "NWK_LB_Key";
    char* fwd_index_signal_name = "Fwd_Layer_Index";

    /*
     * Read the HW configuration from the device
     */
    load_blancing_sim_hw_cfg_t load_blancing_sim_hw_cfg;

    load_balancing_sim_hw_cfg_get(unit,&load_blancing_sim_hw_cfg);

    /* In J2C/Q2A devices the signal "header" is called "pkt_header" */
    if ((load_blancing_sim_hw_cfg.device == DEVICE_TYPE_QUMRAN2_A0 || load_blancing_sim_hw_cfg.device == DEVICE_TYPE_JERICHO2C) && !is_adapter)
    {
        packet_signal_name = "pkt_header";
    }
    bcm_instru_vis_signal_key_t signal_key[5];
    bcm_instru_vis_signal_result_t signal_result[5];

    /*
     * Collect signals to compare against the simulator output (LB keys) and some of the simulator inputs (packet data and forward index)
     */

    /* packet signal */
    sal_memset(&signal_key[0], 0x0, sizeof(signal_key[0]));
    sal_strncpy(signal_key[0].block, block_name, MAX_BLOCK_NAME_LENGTH-1);
    sal_strncpy(signal_key[0].signal, packet_signal_name, MAX_SIGNAL_NAME_LENGTH-1);

    /* ECMP key signal */
    sal_memset(&signal_key[1], 0x0, sizeof(signal_key[1]));
    sal_strncpy(signal_key[1].block, block_name, MAX_BLOCK_NAME_LENGTH-1);
    sal_strncpy(signal_key[1].from, key_from_name, MAX_STAGE_NAME_LENGTH-1);
    sal_strncpy(signal_key[1].to, key_to_name, MAX_STAGE_NAME_LENGTH-1);
    sal_strncpy(signal_key[1].signal, ecmp_key_signal_name, MAX_SIGNAL_NAME_LENGTH-1);

    /* Lag key signal */
    sal_memset(&signal_key[2], 0x0, sizeof(signal_key[2]));
    sal_strncpy(signal_key[2].block, block_name, MAX_BLOCK_NAME_LENGTH-1);
    sal_strncpy(signal_key[2].from, key_from_name, MAX_STAGE_NAME_LENGTH-1);
    sal_strncpy(signal_key[2].to, key_to_name, MAX_STAGE_NAME_LENGTH-1);
    sal_strncpy(signal_key[2].signal, lag_key_signal_name, MAX_SIGNAL_NAME_LENGTH-1);

    /* NWK key signal */
    sal_memset(&signal_key[3], 0x0, sizeof(signal_key[3]));
    sal_strncpy(signal_key[3].block, block_name, MAX_BLOCK_NAME_LENGTH-1);
    sal_strncpy(signal_key[3].from, key_from_name, MAX_STAGE_NAME_LENGTH-1);
    sal_strncpy(signal_key[3].to, key_to_name, MAX_STAGE_NAME_LENGTH-1);
    sal_strncpy(signal_key[3].signal, nwk_key_signal_name, MAX_SIGNAL_NAME_LENGTH-1);

    /* forwarding header */
    sal_memset(&signal_key[4], 0x0, sizeof(signal_key[4]));
    sal_strncpy(signal_key[4].block, block_name, MAX_BLOCK_NAME_LENGTH-1);
    sal_strncpy(signal_key[4].from, fwd_index_from_name, MAX_STAGE_NAME_LENGTH-1);
    sal_strncpy(signal_key[4].to, fwd_index_to_name, MAX_STAGE_NAME_LENGTH-1);
    sal_strncpy(signal_key[4].signal, fwd_index_signal_name, MAX_SIGNAL_NAME_LENGTH-1);

    rv = bcm_instru_vis_signal_get(unit, core, BCM_INSTRU_VIS_SIGNAL_FLAG_RETURN_ON_1ST_ERROR, 5, signal_key, signal_result);
    if(rv != BCM_E_NONE)
    {
        printf("bcm_dnx_instru_vis_signal_get failed \n");
        return rv;
    }
    /*
     * Make sure that all the signals are valid
     */
    for(iter = 0;iter < 4;iter++)
    {
        if(signal_result[iter].status != BCM_E_NONE)
        {
            printf("bcm_dnx_instru_vis_signal_get failed on signal number %d (core %d) err %d \n",iter,core,rv);
            return rv;
        }
    }

    /*
     * Change the packet signal into the format of the simulator.
     */
    int packet_size_32 = ((signal_result[0].size + 31) / 32);

    for(iter = 0;iter < packet_size_32;iter++)
    {
        int byte_iter;
        for(byte_iter = 0;byte_iter < 4;byte_iter++)
        {
            packet_header[byte_count++] = (signal_result[0].value[packet_size_32 - 1 -iter] >> ((3-byte_iter) << 3)) & 0xFF;
        }
    }

    /*
     * Get the forwarding index
     */
    headers_info.forwarding_layer = signal_result[4].value[0] & 0x7;

    uint32 lb_keys[5];
    /*
     * Run the simulator
     */
    rv = load_balancing_sim_main(packet_header, byte_count, &headers_info, &load_blancing_sim_hw_cfg, lb_keys);
    if(rv != BCM_E_NONE)
    {
        printf("load_balancing_sim_main failed \n");
    }

    uint32 ecmp_hier_1_key = signal_result[1].value[0] & 0xFFFF;
    uint32 ecmp_hier_2_key = (signal_result[1].value[0] >> 16) & 0xFFFF;
    uint32 ecmp_hier_3_key = signal_result[1].value[1] & 0xFFFF;

    printf("NWK key expected 0x%x received 0x%x \n",signal_result[3].value[0], lb_keys[0]);
    printf("LAG key expected 0x%x received 0x%x \n",signal_result[2].value[0], lb_keys[1]);
    printf("ECMP key 1 expected 0x%x received 0x%x \n",ecmp_hier_1_key, lb_keys[2]);
    printf("ECMP key 2 expected 0x%x received 0x%x \n",ecmp_hier_2_key, lb_keys[3]);
    printf("ECMP key 3 expected 0x%x received 0x%x \n",ecmp_hier_3_key, lb_keys[4]);

    /*
     * Verify that the signals of the LB keys are matching the simulation keys.
     */
    if(signal_result[3].value[0] != lb_keys[0])
    {
        printf("NWK key was expected to be 0x%x but received 0x%x \n",signal_result[3].value[0], lb_keys[0]);
        return BCM_E_FAIL;
    }

    if(signal_result[2].value[0] != lb_keys[1])
    {
        printf("LAG key was expected to be 0x%x but received 0x%x \n",signal_result[2].value[0], lb_keys[1]);
        return BCM_E_FAIL;
    }

    if(ecmp_hier_1_key != lb_keys[2])
    {
        printf("NWK key was expected to be 0x%x but received 0x%x \n",ecmp_hier_1_key, lb_keys[2]);
        return BCM_E_FAIL;
    }

    if(ecmp_hier_2_key != lb_keys[3])
    {
        printf("NWK key was expected to be 0x%x but received 0x%x \n",ecmp_hier_2_key, lb_keys[3]);
        return BCM_E_FAIL;
    }

    if(ecmp_hier_3_key != lb_keys[4])
    {
        printf("NWK key was expected to be 0x%x but received 0x%x \n",ecmp_hier_3_key, lb_keys[4]);
        return BCM_E_FAIL;
    }

    return rv;
}

