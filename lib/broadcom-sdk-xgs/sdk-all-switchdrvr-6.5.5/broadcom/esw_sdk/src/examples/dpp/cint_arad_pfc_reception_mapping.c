/* $Id: cint_arad_pfc_reception_mapping.c,v 1.3 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
* File: cint_arad_pfc_reception_mapping.c
* Purpose: Examples of how to map PFC source to port target and map PFC source to relevant priority in all ports on same interface.
*
* 1. map PFC source to port target
*
*  Example - map PFC 0 to OTM port 1 and map PFC 1 to OTM port 2
*
*  Environment:
*    - Both OTM port 1 and OTM port 2 are mapped on the same physical port 14.
*      some relevant SOC properties
*      - ucode_port_1.BCM88650=10GBase-R14.0
*      - ucode_port_2.BCM88650=10GBase-R14.0
*      - port_priorities_1.BCM88650=2
*      - port_priorities_2.BCM88650=2
*  
*  Expectation:
*    1) when PFC 0 indication received, only traffic on port 1 is stopped.
*    2) when PFC 1 indication received, only traffic on port 2 is stopped.
*
*  To Activate Above Settings Run:
*    BCM> cint examples/dpp/cint_arad_pfc_reception_mapping.c
*    BCM> cint
*    cint> 
*    cint> cint_pfc_mapping_port_over_pfc_set_example(unit, 1, 0);
*    cint> cint_pfc_mapping_port_over_pfc_set_example(unit, 2, 1);
* 
*
* 2. map PFC source to relevant priority in all ports on same interface
*
*  Example - map PFC 0 to priority 0 in port 1 and port 2 on same interface.
*
*  Environment:
*    - Both OTM port 1 and OTM port 2 are mapped on the same physical port 14.
*      some relevant SOC properties
*      - ucode_port_1.BCM88650=10GBase-R14.0
*      - ucode_port_2.BCM88650=10GBase-R14.0
*      - port_priorities_1.BCM88650=2
*      - port_priorities_2.BCM88650=2
*  
*  Expectation:
*    when PFC 0 indication received, traffic of priority 0 on port 1 and traffic of priority 0 on port 2 are stopped.
*
*  To Activate Above Settings Run:
*    BCM> cint examples/dpp/cint_arad_pfc_reception_mapping.c
*    BCM> cint
*    cint> 
*    cint> cint_pfc_mapping_intf_cosq_pfc_set_example(unit, 1, 0);
*    Or 
*    cint> cint_pfc_mapping_intf_cosq_pfc_set_example(unit, 2, 0);
*  
*/

int cint_pfc_mapping_port_over_pfc_set_example(
    int unit,
    int source_port, 
    int cosq)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlReception;

    /* set target port fc inbnd mode*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlPFCReceive, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port fc inbnd mode in cint_pfc_mapping_port_over_pfc_set_example\n");
        return rv;
    }
    
    /* set the Flow Control Path */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    BCM_GPORT_LOCAL_SET(source.port, source_port);
    source.cosq = cosq;
	target.cosq = -1;
    target.flags = BCM_COSQ_FC_PORT_OVER_PFC|BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;

    rv = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int cint_pfc_mapping_port_over_pfc_unset_example(
    int unit,
    int source_port,
    int cosq)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlReception;
    
    /* unset the Flow Control Path */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    BCM_GPORT_LOCAL_SET(source.port, source_port);
    source.cosq = cosq;
    target.cosq = -1;
    target.flags = BCM_COSQ_FC_PORT_OVER_PFC|BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;

    rv = bcm_cosq_fc_path_delete(unit, fc_direction, &source, &target);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* unset target port fc inbnd mode*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlPFCReceive, 0);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port fc inbnd mode in cint_pfc_mapping_port_over_pfc_unset_example\n");
        return rv;
    }

    return rv;
}

int cint_pfc_mapping_intf_cosq_pfc_set_example(
    int unit,
    int source_port,
    int cosq)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlReception;
        
    /*Setting target port fc inbnd mode*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlPFCReceive, 1);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port fc inbnd mode in cint_pfc_mapping_intf_cosq_pfc_set_example\n");
        return rv;
    }
    
    /* Set the Flow Control Path */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    /* source_port should be one of the ports mapped to the interface we would like to stop */
    BCM_GPORT_LOCAL_SET(source.port, source_port);
    source.cosq = cosq;
    target.flags = BCM_COSQ_FC_INTF_COSQ_PFC|BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;

    rv = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int cint_pfc_mapping_intf_cosq_pfc_unset_example(
    int unit,
    int source_port,
    int cosq)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlReception;
  
    /* unset the Flow Control Path */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    /* source_port should be one of the ports mapped to the interface we would like to stop */
    BCM_GPORT_LOCAL_SET(source.port, source_port);
    source.cosq = cosq;
    target.flags = BCM_COSQ_FC_INTF_COSQ_PFC|BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;

    rv = bcm_cosq_fc_path_delete(unit, fc_direction, &source, &target);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* unset target port fc inbnd mode*/
    rv = bcm_port_control_set(unit, source_port, bcmPortControlPFCReceive, 0);
    if (rv != BCM_E_NONE) {
        printf("Failed to set port fc inbnd mode in cint_pfc_mapping_intf_cosq_pfc_set_example\n");
        return rv;
    }

    return rv;
}

