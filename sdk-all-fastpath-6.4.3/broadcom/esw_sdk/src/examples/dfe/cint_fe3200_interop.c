/* 
 * $Id: cint_fe3200_interop.c,v 1.6 Broadcom SDK $
 *
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * FE3200 interop example:
 *  
 * FE3200 default parameters are optimized to a pure FE3200-Jericho system.
 * FE3200 is backward compatible with DNX legacy generation (ARAD and FE1600).
 * This Cint provide as an example of what should be configured when connecting FE3200 to legacy device.
 * 
 * The following cints should be included:
 * sdk/src/examples/dfe/cint_flow_control_fe3200.c>
 *  
 */

/*
 * Function: 
 *      cint_fe3200_interop_link
 * Purpose: 
 *      Configure per link related configuration required when connecting to legacy device.  
 * Parameters: 
 *      unit                                    (IN) - unit #
 *      port                                    (IN) - link #
 *      pcs                                     (IN) - encoding bcmPortPCS64b66bFec or bcmPortPCS64b66b.
 *      cl72                                    (IN) - Enable/disable cl72
 *      remote_pipe_mapping, nof_remote_pipes   (IN) - In a case that renmote device configuration is identical - Set remote_pipe_mapping tp NULL.
 *                                                     Otherwise a manual mapping should be configured from remote pipes to local pipes.
 *                                                     For further read about bcm_fabric_link_remote_pipe_mapping_set (BCM88950 User Manual)
 * Returns: 
 *      BCM_E_XXX 
 */
int
cint_fe3200_interop_link(int unit, 
                         bcm_port_t port, 
                         int speed, 
                         bcm_port_pcs_t pcs, 
                         int cl72,
                         bcm_fabric_pipe_t *remote_pipe_mapping, int nof_remote_pipes)
{
    int rv;

    /*Disable link*/
    rv = bcm_port_enable_set(unit, port, 0);
    if (BCM_FAILURE(rv))
    {
        printf("Port disable failed unit %d, port %d.\n", unit, port);
        return rv;
    }

    /*Configure PCS*/
    rv = bcm_port_control_set(unit,  port, bcmPortControlPCS, pcs);
    if (BCM_FAILURE(rv))
    {
        printf("PCS configuration failed unit %d, port %d, $pcs.\n", unit, port);
        return rv;
    }

    /*If KR-FEC: Disable extracting congestion indication from LLFC cells*/
    if (pcs == bcmPortPCS64b66bFec) /*KR-FEC*/
    {
        rv = bcm_port_control_set(unit,  port, bcmPortControlLlfcCellsCongestionIndEnable, 0);
        if (BCM_FAILURE(rv))
        {
            printf("Disable extracting congestion indication from LLFC cells failed unit %d, port %d, $pcs.\n", unit, port);
            return rv;
        }
    }

    /*Speed*/
    rv = bcm_port_speed_set(unit,  port, speed);
    if (BCM_FAILURE(rv))
    {
        printf("Speed set failed unit %d, port %d, speed %d.\n", unit, port, speed);
        return rv;
    }

    /*CL72*/
    rv = bcm_port_phy_control_set(unit,  port, BCM_PORT_PHY_CONTROL_CL72, cl72);
    if (BCM_FAILURE(rv))
    {
        printf("CL72 configuration failed unit %d, port %d, cl72 %d.\n", unit, port, cl72);
        return rv;
    }

    /*Disable PCP - Packet Cell Packing*/
    rv = bcm_fabric_link_control_set(unit,  port, bcmFabricLinkPcpEnable, 0);
    if (BCM_FAILURE(rv))
    {
        printf("PCP disable failed unit %d, port %d.\n", unit, port);
        return rv;
    }

    /*Configure remote pipe mapping - if necessary*/
    bcm_fabric_link_remote_pipe_mapping_t pipe_config;
    bcm_fabric_link_remote_pipe_mapping_t_init((&pipe_config));
    if (remote_pipe_mapping == NULL)
    {
        /*Clear previous pipe mapping*/
        pipe_config.num_of_remote_pipes = 0;
        pipe_config.remote_pipe_mapping_max_size = 0;
        pipe_config.remote_pipe_mapping = NULL;
    } else {
        /*Set new pipe mapping*/
        pipe_config.remote_pipe_mapping = remote_pipe_mapping;
        pipe_config.remote_pipe_mapping_max_size = nof_remote_pipes;
        pipe_config.num_of_remote_pipes = nof_remote_pipes;
    }
    rv = bcm_fabric_link_remote_pipe_mapping_set(unit,port , pipe_config);
    if (BCM_FAILURE(rv))
    {
        printf("Mapping config failed unit %d port %d.\n", unit, port);
        return rv;
    }

    /*Enable link*/
    rv = bcm_port_enable_set(unit, port, 1);
    if (BCM_FAILURE(rv))
    {
        printf("Port enable failed unit %d, port %d.\n", unit, port);
        return rv;
    }

    printf("cint_fe3200_interop_link: PASS - unit %d, port %d.\n", unit, port);
    return BCM_E_NONE;
}

/*
 * Function: 
 *      cint_fe3200_interop
 * Purpose: 
 *      Configuration required when connecting FE3200 to legacy device.  
 * Parameters: 
 *      unit                                    (IN) - unit #
 *      legacy_ports, nof_legacy_ports          (IN) - array  (and size of array) of links that connected to legace device.
 *      pcs                                     (IN) - encoding bcmPortPCS64b66bFec or bcmPortPCS64b66b.
 *      cl72                                    (IN) - Enable/disable cl72
 *      remote_pipe_mapping, nof_remote_pipes   (IN) - In a case that renmote device configuration is identical - Set remote_pipe_mapping tp NULL.
 *                                                     Otherwise a manual mapping should be configured from remote pipes to local pipes.
 *                                                     For further read about bcm_fabric_link_remote_pipe_mapping_set (BCM88950 User Manual)
 *      is_fe1600_in_system                     (IN) - used to adjust FE3200 flow controls thresholds to FE1600
 * Returns: 
 *      BCM_E_XXX 
 */
int
cint_fe3200_interop(int unit, 
                    bcm_port_t *legacy_ports, int nof_legacy_ports, 
                    int speed, 
                    bcm_port_pcs_t pcs, 
                    int cl72, 
                    bcm_fabric_pipe_t *remote_pipe_mapping, int nof_remote_pipes,
                    int is_fe1600_in_system)
{
    bcm_port_t port;
    int i, rv;

    /*Configure per link related condifurations: speed, pcs, cl72, pcp, pipe_mapping*/
    for (i = 0; i <  nof_legacy_ports; i++)
    {
        port = legacy_ports[i];
        rv = cint_fe3200_interop_link(unit, port, speed, pcs, cl72, remote_pipe_mapping, nof_remote_pipes);
        if (BCM_FAILURE(rv))
        {
            printf("cint_fe3200_interop_link failed unit %d, port %d, speed %d, pcs %d, cl72 %d.\n", unit, port, speed, pcs, cl72);
            return rv;
        }
    }

    /*in case fe1600 in system - configure flow control threholds in order to match the latency between FE3200 to FE1600*/
    if (is_fe1600_in_system)
    {
        rv = cint_flow_control_fe3200_legacy_main(unit, 0);
        if (BCM_FAILURE(rv))
        {
            printf("cint_flow_control_fe3200_legacy_main failed unit %d.\n", unit);
            return rv;
        }
    }

    printf("cint_fe3200_interop: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}

/*
 * Function: 
 *      cint_fe3200_interop_example
 * Purpose: 
 *      This is an example to FE3200-ARAD system.
 *      Assuming:   link rates: 10312, pcs: KR-FEC, cl72: enabled,
 *                  ARAD pipes configuration identical to FE3200 pipe configuration.
 *                  fe1600 is not in the system.
 * Parameters: 
 *      unit                                    (IN) - unit #
 * Returns: 
 *      BCM_E_XXX 
 */
int
cint_fe3200_interop_example(int unit)
{
    int speed = 10312;
    bcm_port_pcs_t pcs = bcmPortPCS64b66bFec;
    int cl72 = 1;
    int is_fe1600_in_system = 0;
    bcm_fabric_pipe_t *remote_pipe_mapping = NULL;
    int nof_remote_pipes = 1; /*not relevant when remote_pipe_mapping set to NULL*/
    bcm_port_t legacy_ports[144];
    int nof_legacy_ports;
    bcm_port_config_t port_config;
    int rv;
    bcm_port_t port;

    /*Get all fabric link ports*/
    rv = bcm_port_config_get(unit, &port_config);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_config_get failed: unit %d.\n", unit);
        return rv;
    }

    nof_legacy_ports = 0;
    BCM_PBMP_ITER(port_config.sfi, port)
    {
        legacy_ports[nof_legacy_ports] = port;
        nof_legacy_ports++;
    }
    
    /*Configure device and all links*/
    rv = cint_fe3200_interop(unit, legacy_ports,  nof_legacy_ports, speed, pcs, cl72, remote_pipe_mapping, nof_remote_pipes, is_fe1600_in_system);
    if (BCM_FAILURE(rv))
    {
        printf("cint_fe3200_interop failed unit %d, port %d, speed %d, pcs %d, cl72 %d.\n", unit, speed, pcs, cl72);
        return rv;
    }

    printf("cint_fe3200_interop_example: PASS - unit %d.\n", unit);
    return BCM_E_NONE;
}
