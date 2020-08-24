/*
 * $Id: cint_ifa.c, Exp $
 $Copyright: (c) 2019 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ File: cint_ifa.c Purpose: basic examples for ifa 1.0 Datapath.
 */

/*
* Test Scenario
* 
* Add Soc Properties for Recycle port: 
*               ucode_port_245=RCY.45:core_0.$recycle_port
*               tm_port_header_type_out_245=ETH
*               tm_port_header_type_in_245=ETH
*
* ./bcm.user
* cd ../../../../regress/bcm
* cint ../../src/./examples/dpp/utility/cint_utils_global.c
* cint ../../src/./examples/sand/cint_ip_route_basic.c
* cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
* cint ../../src/./examples/dnx/instru/cint_field_IFA_datapath.c
* cint ../../src/./examples/dnx/internal/cint_route_internal.c
* cint
* 
* # Using Inport=200, Outport=201, Recycle_port=245
* ifa_route_config(0,200,201,245,52430);
* exit;
*
*  # IPT packet
* tx 1 psrc=200 data=0x000c00020000000007000100080045000057000000008006fa1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
* # Received packets on unit 0 should be:
* # Source port: 0, Destination port: 0
* # Data: 0x00000000cd1d00123456789a81000064080045000057000000007f06fb1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
* cint ../../src/./examples/dpp/utility/cint_utils_global.c
* cint ../../src/./examples/dnx/oam/cint_oam_basic.c
* cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
* cint ../../src/./examples/dnx/field/cint_field_ip_snoop.c
* cint ../../src/./examples/dnx/instru/cint_instru_ipt.c
* cint ../../src/./examples/dnx/instru/cint_field_IFA_datapath.c
* cint ../../src/./examples/dnx/instru/cint_ifa.c
* cint
* cint_instru_ifa1_initiator(0,0xa1f1a1f1,0xa1f2a1f2);
* exit;
*
* # IPT packet
* tx 1 psrc=200 data=0x000c00020000000007000100080045000057000000008006fa1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
* # Received packets on unit 0 should be:
* # Source port: 0, Destination port: 0
* # Data: 0x00000000cd1d00123456789a81000064080045000057000000007f06fb1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
* # Source port: 0, Destination port: 0
* # Data: 0x00000000cd1d00123456789a81000064080045000093000000007f06fae1c08001017fffff023a98000000000000000000000010001000000000a1f1a1f1a1f2a1f210010200003b0000ff00000010000000ffff00000000ffff20008000006680008aca1df5860021e21ddc000000f500c900000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
*
*
*/




int DEFAULT_DEVICE_ID = 0xFFFF;
int DEFAULT_HOP_LIMIT = 0xff;
int DEFAULT_MAX_IFA_LENGTH = 0x1000;
int DEFAULT_IFA_SENDER = 0xFFFF;
int DEFAULT_IFA_TEMPLATE_ID = 0x2;

int ifa_encap_id;

/*
 * Get core from port id
 */
int get_core_from_port(int unit, int port, int * core)
{
    bcm_error_t rc ;
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info ;
    bcm_gport_t gport = port;

    rc = bcm_port_get(unit, gport, &dummy_flags, &interface_info, &mapping_info);
    if (BCM_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
            (BSL_META_U(unit,"bcm_port_get failed. Error:%d (%s)\n"),
                                                    rc, bcm_errmsg(rc)));
        return -1;
    } else {
        *core = mapping_info.core ;
    }
    return 0;
}

/*
 * Example 1.0 Node configuration.
 * This example configures all node parameters through bcm_instru_ifa.
 * The IFA probe header and metadata will be built according to this configuration.
 * This configuration should be performed for all node types (intiator, transit
 * and terminator).
 *
 */
int cint_ifa1_node_parameters_config(
        int unit,
        uint32 probe_1,
        uint32 probe_2)
{
    int rv = BCM_E_NONE;
    bcm_ifa_config_info_t config_data;

    /* Use default values for all fields. Probe Marker 1,2 must be provided */
    config_data.probemarker_1 = probe_1;
    config_data.probemarker_2 = probe_2;

    config_data.device_id = DEFAULT_DEVICE_ID;
    config_data.hop_limit = DEFAULT_HOP_LIMIT;
    config_data.max_payload_length = DEFAULT_MAX_IFA_LENGTH;
    config_data.senders_handle = DEFAULT_IFA_SENDER;
    config_data.template_id = DEFAULT_IFA_TEMPLATE_ID;

    /** Set ifa1 node */
    rv = bcm_ifa_config_info_set(unit , 0, &config_data);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_ifa_config_info_set failed\n");
        printf("rv %d\n",rv);
        return rv;
    }

    return rv;
}

/*
 * Example for setting Outlif and counter for IFA
 * Counter will be used to count packets and stamping
 * the value on the IFA Header (sequence number) and on
 * IFA Metadata
 *
 * */
int cint_set_IFA_outlif_and_counter(
        int unit,int port,int *outlif,int *counter)
{
   bcm_vlan_port_t port_info;
   int counter_if;
   int rv = 0;

   /* Create Gport */

   port_info.port = port;
   port_info.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
   port_info.match_vlan = 10;
   port_info.egress_vlan = 10;
   rv = bcm_vlan_port_create(unit,&port_info);
   if (rv != BCM_E_NONE)
   {
       printf("bcm_vlan_port_create failed\n");
       printf("rv %d\n",rv);
       return rv;
   }
   *outlif = port_info.vlan_port_id;
   printf("Outlif for IFA - 0x%x\n",*outlif);

   /* Allocate Counter */
   counter_if = 0;
   rv = set_counter_resource(unit, port_info.port, counter_if, counter);
   if (rv != BCM_E_NONE)
   {
       printf("set_counter_resource failed\n");
       printf("rv %d\n",rv);
       return rv;
   }
   printf("Counter for IFA - 0x%x\n",*counter);

   return 0;
}

int cint_set_IFA_counter(
        int unit,int port, int *counter)
{
   int counter_if;
   int rv = 0;

   /* Allocate Counter */
   counter_if = 0;
   rv = set_counter_resource(unit, port, counter_if, counter);
   if (rv != BCM_E_NONE)
   {
       printf("set_counter_resource failed\n");
       printf("rv %d\n",rv);
       return rv;
   }
   printf("Counter for IFA - 0x%x\n",*counter);

   return 0;
}

/*
 * Example cint for IFA 1.0 Transit Node.
 * iPMF1 will be used to trap IFA packets. This is done by probe_marker_1 and probe_marker_2
 * fields in IFA probe header. This cint will set the PMF trap with the expected probe_marker
 * values.
 * The PMF will set IPTProfile, IPTCommand and Outlif to add current node's metadata to the packet.
 * The Outlif will be used to access counter for metadata's counter field.
 *
 */
int cint_instru_ifa1_transit(
        int unit,
        uint32 probe_1,
        uint32 probe_2,
        int out_port)
{
    bcm_field_entry_t entry_handle;
    int rv = BCM_E_NONE;
    bcm_instru_ifa_info_t ifa_info;
    int outlif;
    int counter;

    entry_handle = 0;

    /* Configure Transit Node Parameters */
    rv = cint_ifa1_node_parameters_config(unit,probe_1,probe_2);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in configuration IFA node parameters \n", rv);
       return rv;
    }

    /* Configure Field */
    rv = cint_field_IFA_datapath_intermediate_ingress_main(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding iPMF1 rule \n", rv);
       return rv;
    }

    rv = cint_field_IFA_datapath_intermediate_ingress_entry_add(unit,probe_1,probe_2,&entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding iPMF1 rule \n", rv);
       return rv;
    }

    return rv;
}

/*
 * Example cint for IFA 1.0 Egress Node.
 * ePMF1 will be added for trapping and recycling on egress side.
 * This is done by probe_marker_1 and probe_marker_2
 * fields in IFA probe header. This cint will set the PMF trap with the expected probe_marker
 * values.
 * The PMF will set "IFA-TRAP-Context"
 * This function also set out_port as "IFA-Egress-Port" according these two parameters packet will be recycled
 *
 * This function should be called after cint_instru_ifa1_transit function with same probe parameters.
 */
int cint_instru_ifa1_egress(
        int unit,
        uint32 probe_1,
        uint32 probe_2,
        int out_port,
        int recycle_port)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_t entry_handle = 0;
    bcm_instru_ifa_info_t ifa_info;

    /* Configure field */
    rv = cint_field_IFA_datapath_egress_main(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in ePMF rule creation \n", rv);
       return rv;
    }

    /* Set ePMF to trap IFA packet */
    rv = cint_field_IFA_datapath_egress_entry_add(unit,probe_1,probe_2,&entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding ePMF entry \n", rv);
       return rv;
    }

    rv = cint_field_IFA_datapath_ingress_2nd_pass_entry_add(unit,&entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding 2nd pass entry \n", rv);
       return rv;
    }

    /* Out port configuration */
    rv = bcm_instru_gport_control_set(unit, out_port,0,
                                                 bcmInstruGportControlIptTrapToRcyEnable, TRUE);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_instru_gport_control_set \n", rv);
       return rv;
    }

    return rv;
}


/*
 * cint_instru_ifa1_set_recycle_port
 * ==========================
 *
 * Chooses recycle port on the same core as the in_port and sets ifa1 trap_context_profile
 *
 * in_port [in] - packet's in port
 * recycle_port_0 [in] - recycle_port on core 0
 * recycle_port_1 [in] - recycle_port on core 1
 * *recycle_port [out] - chosen recycle_port
 */
int cint_instru_ifa1_set_recycle_port(
        int unit,
        int in_port,
        int recycle_port_0,
        int recycle_port_1,
        int *recycle_port)
{
int core=0;
int rv=0;

    rv = get_core_from_port(unit, in_port, &core);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), getting core from in port.\n", rv);
        return rv;
    }
    if(core==0)
    {
        *recycle_port=recycle_port_0;
    }
    else
    {
    	*recycle_port=recycle_port_1;
    }
    printf("Recycling on core %d\n",core);

    rv = bcm_instru_gport_control_set(unit,*recycle_port,0,bcmInstruGportControlIfaInitiatorRcyEnable,1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_instru_gport_control_set .\n", rv);
        return rv;
    }
    return rv;
}


/*
 * cint_instru_ifa1_initiator
 * ==========================
 *
 * Example cint for IFA 1.0 Ingress Node.
 *
 * Operation principle:
 *   - In the first cycle required packets (IPv4 in this example) are snooped.
 *     While original packet is forwarded on the normal path, the snooped copy
 *     is encapsulated with IFA probe-header and recycled.
 *   - In the second cycle the packet is encapsulated with IFA metadata and forwarded
 *     to the same destination as the original packet.
 *
 *
 *     First Cycle: The packet is snooped in iPMF1. Original copy is forwarded to the out_port, so it does
 *                   not hit ePMF. The snooped IFA copy, gets recycle_port so is hit in ePMF. ePMF sets
 *                   Tail_edit_profile=3. This chooses IFA_First Context in Term, Forward and ENC1-3.
 *                   IFA Header is added to the packet, including Hop_Count=0, Length=28 (Header's length)
 *                   and stamped Sequence Number. Then the packet is recycled for metadata encapsulation.
 *     Second Cycle: The packet hits iPMF1 since it has IFA Probe Header. Container is set and tail-edit-profile
 *                   is set to 3. iPMF3 is hit because of the container and then INT is set to 1. The contexts of
 *                   IFA are chosen in Term, Forward and ENC1-3. IFA Header is updated by hop++ and length+=32.
 *                   Sequence number is copied from the original IFA header.
 *
 * Pre-configuration:
 *  - Configure Route both for inport and the recycle port
 *
 * Configuration steps:
 *   - Set Node parameters including IFA session's probe-header
 *   - Configure outlif and counter to be used for sequence number stamping
 *   - Configure outlif and counter to be used for metadata counter stamping
 *   - Setup field configurations
 *   - Create IFA entity for IFA Header
 *   - Create IFA entity for IFA Metadata
 */
int cint_instru_ifa1_initiator(
        int unit,
        int in_port,
        int out_port,
        int recycle_port_0,
        int recycle_port_1,
        uint32 probe_1,
        uint32 probe_2)
{
    bcm_field_entry_t entry_handle;
    bcm_instru_ifa_info_t ifa_info;
    int header_counter;
    int recycle_port;

    entry_handle = 0;

    BCM_IF_ERROR_RETURN(cint_instru_ifa1_set_recycle_port(unit,in_port,recycle_port_0,recycle_port_1,&recycle_port));

    /* Configure Initiator Node Parameters */
    BCM_IF_ERROR_RETURN(cint_ifa1_node_parameters_config(unit,probe_1,probe_2));

    /* Set counter attached to outlif for sequence counting */
    BCM_IF_ERROR_RETURN(cint_set_IFA_counter(unit,recycle_port,&header_counter));

    /* *Create IFA entity */
    ifa_info.counter_command_id = 0;
    ifa_info.stat_cmd = header_counter;

    BCM_IF_ERROR_RETURN(bcm_instru_ifa_encap_create(unit,&ifa_info));
    ifa_encap_id = ifa_info.ifa_encap_id;

    /* Set pmf to trap all IPv4 packets and start an IFA flow */
    BCM_IF_ERROR_RETURN(cint_field_group_const_example_IFA_gen(unit, ifa_encap_id, recycle_port));

    /* Set pmf to add Metadata */
    BCM_IF_ERROR_RETURN(cint_field_IFA_datapath_intermediate_ingress_entry_add(unit,probe_1,probe_2,&entry_handle));

    return 0;
}
