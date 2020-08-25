/*
 * $Id: cint_sr_cell.c,v 1.3 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * DCMN send \ receive SR cell example:
 * 
 * The example simulate:
 *  1. define single route and send  sr cell to this route
 *  2. define route group and cell sr cell to the group
 *  3. receive sr cells
 *
 */

/*
 * Global variables
 */
uint32 MAX_SEND_WORDS = 16;
uint32 MAX_RECEIVE_WORDS = 76;
uint32 data_set[MAX_SEND_WORDS];
uint32 data_get[MAX_RECEIVE_WORDS];
uint32 received_data_actual_size = 0;


/**
 * \brief
 * Auxialiary function for getting links in route.
 * Should use reachability_get instead.
 */
int
cint_sr_cell_link_ids_get(int unit_fap_0, int unit_fe_0, int nof_hops, int* link_ids)
{
    int rv;
    int is_qax = 0, is_ramon = 0, is_jr2 = 0;

    /*
     * go from FAP0 to FE through link 15
     */
    link_ids[0] = 15;

    /*
     * go from FE to FAP1 through link 72 for Kalia-FE3200, link 2 for JerichoPlus-Ramon or 0 for other systems
     */
    rv = is_device_qax_only(unit_fap_0, &is_qax);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_qax, rv=%d, \n", rv);
        return rv;
    }
    rv = is_device_jericho2(unit_fap_0, &is_jr2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2, rv=%d, \n", rv);
        return rv;
    }
    rv = is_device_ramon(unit_fe_0, &is_ramon);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_ramon, rv=%d, \n", rv);
        return rv;
    }
    if (is_qax)
    {
        link_ids[1] = 72;
    }
    else if (is_jr2)
    {
        link_ids[1] = 144;
    }
    else
    {
        link_ids[1] = is_ramon ? 2 : 0;
    }

    return BCM_E_NONE;
}

/**
 * \brief
 *   Send cell through a specific route.
 * \param [in] nof_hops - number of hops in route.
 * \param [in] link_ids - array of links to hop through.
 */
int
cint_sr_cell_system_send_route(int unit, int nof_hops, int* link_ids)
{
    int rv, route_id;
    bcm_fabric_route_t route;

    bcm_fabric_route_t_init(&route);

    /*
     * define a single route from FAP to FAP.
     * link_ids contains the list of links to hop through.
     */
    route.number_of_hops = nof_hops;
    route.hop_ids = link_ids;

    /*
     * build data
     */
    data_set[0] = 0x00000000;
    data_set[1] = 0x11111111;
    data_set[2] = 0x22222222;
    data_set[3] = 0x33333333;
    data_set[4] = 0x44444444;
    data_set[5] = 0x55555555;
    data_set[6] = 0x66666666;
    data_set[7] = 0x77777777;
    data_set[8] = 0x88888888;
    data_set[9] = 0x99999999;
    data_set[10] = 0xaaaaaaaa;
    data_set[11] = 0xbbbbbbbb;
    data_set[12] = 0xcccccccc;
    data_set[13] = 0xdddddddd;
    data_set[14] = 0xeeeeeeee;
    data_set[15] = 0xffffffff;

    /*
     * send source-routed cell
     */
    rv = bcm_fabric_route_tx(unit, 0, route, MAX_SEND_WORDS, data_set);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_fabric_route_tx, rv=%d, \n", rv);
        return rv;
    }

    printf("cint_sr_cell_system_send_route: PASS\n");
    return BCM_E_NONE;
}

/*define single route and send  sr cell to this route*/
int
send_route(
    int unit_fap_0,
    int unit_fe_0)
{
    int rv;
    int link_ids[2] = {0};
    int nof_hops = 2;

    /*
     * Get links route
     */
    rv = cint_sr_cell_link_ids_get(unit_fap_0, unit_fe_0, nof_hops, link_ids);
    if (BCM_FAILURE(rv))
    {
        printf("Error, in cint_sr_cell_link_ids_get, rv=%d, \n", rv);
        return rv;
    }

    /*
     * Send cell in the route
     */
    rv = cint_sr_cell_system_send_route(unit_fap_0, nof_hops, link_ids);
    if (BCM_FAILURE(rv))
    {
        printf("Error, in cint_sr_cell_system_send_route, rv=%d, \n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*receive sr cells*/
int
receive_sr_cell(
    int unit,
    int max_messages,
    uint32 *data_actual_size_p)
{
    int rv, count, i;
    uint32 data_actual_size;

    /*
     * in case several sr cells received the cells are accumulated in SW 
     * For that reason it's important to read in loop (even if the relevant interrupt is down) 
     * until soc_receive_sr_cell return EMPTY error.
     */
    count = 0;
    while (count < max_messages)
    {
        /*
         * receive sr cell data
         */
        rv = bcm_fabric_route_rx(unit, 0, 76, data_get, &data_actual_size);
        /*
         * all messages was read
         */
        if (BCM_E_EMPTY == rv)
        {
            printf("No more messages to read \n");
            break;
        }
        else if (rv != BCM_E_NONE)
        {
            printf("Error, in soc_receive_sr_cell, rv=%d, \n", rv);
            return rv;
        }

        /*
         * print received data
         */
        printf("Message(%d words) received: ", data_actual_size);
        for (i = 0; i < data_actual_size; i++)
        {
            printf("0x%x ", data_get[i]);
        }
        printf("\n");

        *data_actual_size_p = data_actual_size;

        count++;
    }

    printf("%d messages received \n", count);

    printf("receive_sr_cell: PASS\n");

    return BCM_E_NONE;
}

/**
 * \brief
 *   Clear the receive buffer
 */
void
cint_sr_cell_system_clear(int unit)
{
    uint32 data_actual_size;

    receive_sr_cell(unit, 1000, &data_actual_size);

    printf("Receive buffer cleared\n");
    printf("cint_sr_cell_system_clear: PASS\n");
}

/**
 * \brief
 *   Read cell from received buffer
 */
int
cint_sr_cell_system_receive(int unit)
{
    int rv;

    rv = receive_sr_cell(unit, 1, &received_data_actual_size);
    if (rv != BCM_E_NONE)
    {
        printf("failed to receive cell\n");
        return rv;
    }

    printf("Receive buffer success\n");
    printf("cint_sr_cell_system_receive: PASS\n");

    return BCM_E_NONE;
}

/**
 * \brief
 *   Verify that the send and received packets are the same
 */
void
cint_sr_cell_system_verify()
{
    int i = 0;

    for (i = 0; i < MAX_SEND_WORDS; i++)
    {
        if (data_set[i] != data_get[i])
        {
            int k;
            printf("Failure: Received buffer is different from sent\n");
            for (k = 0; k < received_data_actual_size; k++)
            {
                printf("0x%08x 0x%08x\n", data_get[k], data_set[k]);
            }

            return BCM_E_FAIL;
        }
    }

    printf("cint_sr_cell_system_verify: PASS\n");
}

int
run_sr_cell(
    int unit)
{
    uint32 data_actual_size;
    send_route(unit);
    receive_sr_cell(unit, 1, &data_actual_size);
}

int
cint_sr_cell_system_test(
    int unit_fap_0,
    int unit_fap_1,
    int unit_fe_0)
{

    int i;
    int rv;
    uint32 data_actual_size;

    /*
     * Clear buffer
     */
    cint_sr_cell_system_clear(unit_fap_1);

    /*
     * Send source routed cell
     */
    rv = send_route(unit_fap_0, unit_fe_0);
    if (rv != BCM_E_NONE)
    {
        printf("failed to send cell\n");
        return rv;
    }

    /*
     * Receive source routed cell
     */
    rv = cint_sr_cell_system_receive(unit_fap_1);
    if (rv != BCM_E_NONE)
    {
        printf("failed to receive cell\n");
        return rv;
    }

    /*
     * Verified received source routed cell
     */
    cint_sr_cell_system_verify();

    printf("cint_sr_cell_system_test: PASS\n");

}

