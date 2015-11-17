/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Example of how to add default ports using dynamic port feature ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * $Id: cint_bfd.c,v 1.15 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File: cint_dynamic_ports_init_example.c
 * Purpose: Example of using dynamic port APIs.
 *
 * Usage:
 * 
 * how to run: 
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/dpp/cint_dynamic_port_add_remove.c
 * cint ../../../../src/examples/dpp/cint_dynamic_ports_init_example.c
 * 
 * Once this cint is called, the default application ports are added using dynamic port feature.
 *
 */




int main(int unit)
{
    int                         rv;
    bcm_port_mapping_info_t     mapping_info;
    bcm_port_interface_info_t   interface_info;
    bcm_port_t                  port;
    uint32                      channel = 0;
    int                         header_type;


    /* remove port 1*/
    port = 1;
    rv = remove_port_full_example(unit, port, 0); 
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_remove, port=$port\n");
        return rv;
    }

    /* add port 1 - CGE0*/
    mapping_info.channel = 0;
    mapping_info.core = 0;
    mapping_info.tm_port = 1;
    mapping_info.num_priorities = 8;
    interface_info.phy_port = 1;
    interface_info.interface = BCM_PORT_IF_CAUI;
    interface_info.num_lanes = 10;
    port = 1;
    rv = add_port_full_example(unit, port , mapping_info, interface_info, 0, BCM_SWITCH_PORT_HEADER_TYPE_ETH ,FALSE);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_add, port=$port\n");
        return rv;
    }

    /* remove port 13 */
    port = 13;
    rv = remove_port_full_example(unit, port, 0); 
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_remove, port=$port\n");
        return rv;
    }

    /* add port 13 - 10GBase-R12 */
    mapping_info.channel = 0;
    mapping_info.core = 0;
    mapping_info.tm_port = 13;
    mapping_info.num_priorities = 8;
    interface_info.phy_port =13;
    interface_info.interface = BCM_PORT_IF_XFI;
    port = 13;
    rv = add_port_full_example(unit, port , mapping_info, interface_info, 0, BCM_SWITCH_PORT_HEADER_TYPE_ETH ,FALSE);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_add, port=$port\n");
        return rv;
    }

    /* remove port 14 */
    port = 14;
    rv = remove_port_full_example(unit, port, 0); 
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_remove, port=$port\n");
        return rv;
    }

    /* add port 14 - 10GBase-R13 */
    mapping_info.channel = 0;
    mapping_info.core = 0;
    mapping_info.tm_port = 14;
    mapping_info.num_priorities = 8;
    interface_info.phy_port =14;
    interface_info.interface = BCM_PORT_IF_XFI;
    port = 14;
    rv = add_port_full_example(unit, port , mapping_info, interface_info, 0, BCM_SWITCH_PORT_HEADER_TYPE_ETH ,FALSE);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_add, port=$port\n");
        return rv;
    }

    /* remove port 15 */
    port = 15;
    rv = remove_port_full_example(unit, port, 0); 
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_remove, port=$port\n");
        return rv;
    }

    /* add port 15 - 10GBase-R14 */
    mapping_info.channel = 0;
    mapping_info.core = 0;
    mapping_info.tm_port = 15;
    mapping_info.num_priorities = 8;
    interface_info.phy_port =15;
    interface_info.interface = BCM_PORT_IF_XFI;
    port = 15;
    rv = add_port_full_example(unit, port , mapping_info, interface_info, 0, BCM_SWITCH_PORT_HEADER_TYPE_ETH ,FALSE);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_add, port=$port\n");
        return rv;
    }

    /* remove port 16 */
    port = 16;
    rv = remove_port_full_example(unit, port, 0); 
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_remove, port=$port\n");
        return rv;
    }


    /* add port 16 - 10GBase-R15 */
    mapping_info.channel = 0;
    mapping_info.core = 0;
    mapping_info.tm_port = 16;
    mapping_info.num_priorities = 8;
    interface_info.phy_port = 16;
    interface_info.interface = BCM_PORT_IF_XFI;
    port = 16;
    rv = add_port_full_example(unit, port , mapping_info, interface_info, 0, BCM_SWITCH_PORT_HEADER_TYPE_ETH ,FALSE);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_add, port=$port\n");
        return rv;
    }

    /* remove port 16 */
    port = 17;
    rv = remove_port_full_example(unit, port, 0); 
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_remove, port=$port\n");
        return rv;
    }


    /* add port 17 - CGE1*/
    mapping_info.channel = 0;
    mapping_info.core = 0;
    mapping_info.tm_port = 17;
    mapping_info.num_priorities = 8;
    interface_info.phy_port = 17;
    interface_info.interface = BCM_PORT_IF_CAUI;
    interface_info.num_lanes = 10;
    port = 17;
    rv = add_port_full_example(unit, port , mapping_info, interface_info, 0, BCM_SWITCH_PORT_HEADER_TYPE_ETH ,FALSE);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_add, port=$port\n");
        return rv;
    }
}

print main(0);
