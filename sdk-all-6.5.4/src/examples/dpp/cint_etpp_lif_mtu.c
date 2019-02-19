/* $Id: cint_etpp_lif_mtu Exp $
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
*/
/*
 * Test Scenario: Confgure ETPP LIF MTU trap and see packet terminated
 *
 * soc properties:
 *trap_lif_mtu_enable =1;    Jericho B0 and above
 * 
 * how to run the test: 
 * cint ../../../../src/examples/dpp/utility/cint_utils_rx.c
    cint ../../../../src/examples/dpp/utility/cint_utils_mirror.c
    cint ../../../../src/examples/dpp/utility/cint_utils_switch.c
    cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c
    cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
    cint ../../../../src/examples/dpp/cint_etpp_lif_mtu.c
 * cint
 * etpp_lif_mtu__start_run(0,NULL);
 * exit;
 * 
 * 
 * Traffic: 
 
 * Packet flow:
 
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */
int ETPP_LIF_MTU_MIRROR_CMD = 1;
int ETPP_LIF_MTU_SYS_PORT_1 = 200;
int ETPP_LIF_MTU_SYS_PORT_2 = 201;
int ETPP_LIF_MTU_NUM_OF_HEADER_CODES = bcmForwardingTypeCount;
/*Note both can have local in-LIF and out=LIF*/
int ETPP_LIF_MTU_GLOBAL_LIF_IN_PORT = 0x2000;
int ETPP_LIF_MTU_GLOBAL_LIF_OUT_PORT = 0x2001;

/*VLAN PORT type*/
int ETPP_LIF_MTU_GPORT_GLOBAL_LIF_IN_PORT = 1;
int ETPP_LIF_MTU_GPORT_GLOBAL_LIF_OUT_PORT = ETPP_LIF_MTU_GLOBAL_LIF_OUT_PORT | (17 << 26) /*Vlan port*/| (2 <<22) /*Subtype LIF*/ ; 
int ETPP_LIF_MTU_NUM_OF_USED_PORTS = 2;
int ETPP_LIF_MTU_VSI_FOR_PORTS = 10;

int etpp_lif_mtu_trap_id=0;


struct etpp_lif_mtu_vlan_create_s{
     bcm_vlan_t vsi;
     bcm_port_t port;
     int global_lif_id;
};

/*Main Struct*/
struct etpp_lif_mtu_s{
    rx_etpp_trap_set_utils_s trap_utils;
    mirror_create_with_id_s mirror_dst;
    switch_control_index_utils_s header_codes[ETPP_LIF_MTU_NUM_OF_HEADER_CODES];
    rx_lif_mtu_set_utils lif_mtu;
    etpp_lif_mtu_vlan_create_s lif_creation_info[ETPP_LIF_MTU_NUM_OF_USED_PORTS];
    l2__mact_properties_s l2_entry;
};

etpp_lif_mtu_s g_etpp_lif_mtu ={
    
/**********************************************  Trap Definition ************************************************************/
        /*mirror command                    | mirror strength | mirror enable  | forward strength | forward enable*/
     {    ETPP_LIF_MTU_MIRROR_CMD ,        1       ,     1      ,      2        ,   1        },
        
/**********************************************  Mirror Definition ************************************************************/
    {
         /*mirror command             |    System port             |  multicast id*/
        ETPP_LIF_MTU_MIRROR_CMD, ETPP_LIF_MTU_SYS_PORT_1, 0
    },
/**********************************************  Header enable for MTU Definition ************************************************************/    
    {
         /*   type of filter                |  Header Code NUM|                                        Enable Header*/
        {{bcmSwitchLinkLayerMtuFilter,   bcmForwardingTypeL2},                  {TRUE}},/* L2 switching forwarding. */
        {{bcmSwitchLinkLayerMtuFilter,   bcmForwardingTypeIp4Ucast},            {TRUE}},/* IPv4 Unicast Routing forwarding. */
        {{bcmSwitchLinkLayerMtuFilter,   bcmForwardingTypeIp4Mcast},            {TRUE}},/* IPv4 Multicast Routing forwarding. */
        {{bcmSwitchLinkLayerMtuFilter,   bcmForwardingTypeIp6Ucast},            {TRUE}},/* IPv6 Unicast Routing forwarding. */
        {{bcmSwitchLinkLayerMtuFilter,   bcmForwardingTypeIp6Mcast},            {TRUE}},/* IPv6 Multicast Routing forwarding. */
        {{bcmSwitchLinkLayerMtuFilter,   bcmForwardingTypeMpls},                {FALSE}},/* MPLS Switching forwarding. */
        {{bcmSwitchLinkLayerMtuFilter,   bcmForwardingTypeTrill},               {FALSE}},/* Trill forwarding. */
        {{bcmSwitchLinkLayerMtuFilter,   bcmForwardingTypeRxReason},            {FALSE}},/* Forwarding according to a RxReason. */
        {{bcmSwitchLinkLayerMtuFilter,   bcmForwardingTypeTrafficManagement},   {FALSE}},/* Traffic Management forwarding, when an external Packet Processor sets the forwarding decision. */
        {{bcmSwitchLinkLayerMtuFilter,   bcmForwardingTypeSnoop},               {FALSE}},/* Snooped packet. */
        {{bcmSwitchLinkLayerMtuFilter,   bcmForwardingTypeFCoE},                {FALSE}} /* Fiber Channel over Ethernet forwarding. */
    },
/**********************************************  Per LIF MTU values Definition ************************************************************/ 
    {
        /*flags lif mtu |  gport     Note will be overwritten                 |   MTU val   */
          0     ,     ETPP_LIF_MTU_GPORT_GLOBAL_LIF_OUT_PORT ,   150
    },
/**********************************************  LIF creation values Definition ************************************************************/ 
    {
         /*VSI                                     |  System Port  as GPORT      |  Global LIF*/
        { ETPP_LIF_MTU_VSI_FOR_PORTS ,ETPP_LIF_MTU_SYS_PORT_1 ,ETPP_LIF_MTU_GLOBAL_LIF_IN_PORT},
        { ETPP_LIF_MTU_VSI_FOR_PORTS ,ETPP_LIF_MTU_SYS_PORT_2 ,ETPP_LIF_MTU_GLOBAL_LIF_OUT_PORT}
    },
/**********************************************  L2 entry Definition ************************************************************/ 
    {
        /*Gport       Note will be overwritten             | mac                                                  |                             vlan                       */
        ETPP_LIF_MTU_GPORT_GLOBAL_LIF_OUT_PORT, {0x00, 0x11, 0x11, 0x11, 0x11, 0x11},ETPP_LIF_MTU_VSI_FOR_PORTS
    }
};


/********** 
  functions
 ********** */
/*****************************************************************************
* Function:  etpp_lif_mtu_init
* Purpose:   Init needed SOC properties
* Params:
* unit (IN)  - Device Number
* Return:    (int)
*******************************************************************************/
int etpp_lif_mtu_init(int unit)
{
    
    if (soc_property_get(unit, spn_TRAP_LIF_MTU_ENABLE, 0) == FALSE)
    {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
    
}

/*****************************************************************************
* Function:  etpp_lif_mtu_struct_get
* Purpose:   Cope the global pre-defined values to param
* Params:
* unit  (IN)     - Device Number
* param (OUT) - the pointer to which predefined values are copied
*******************************************************************************/
void etpp_lif_mtu_struct_get(int unit, etpp_lif_mtu_s *param)
{
     sal_memcpy(param,&g_etpp_lif_mtu, sizeof(g_etpp_lif_mtu));
     return;
}

/*****************************************************************************
* Function:  etpp_lif_mtu_trap_destroy
* Purpose:   Destroy the created trap
* Params:
* unit (IN)     - Device Number 
* Return:    (int)
*******************************************************************************/
int etpp_lif_mtu_trap_destroy(int unit) 
{
    int rv = BCM_E_NONE;

    /*delete trap*/
    rv = bcm_rx_trap_type_destroy(unit, etpp_lif_mtu_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : bcm_rx_trap_type_destroy, returned %d\n", rv);
        return rv;
    }

    return rv;
}

/*****************************************************************************
* Function:  etpp_lif_mtu__start_run_with_params
* Purpose:  Run the cint with params 
* Params:
* unit    (IN)     - Device Number 
* inPort   (IN)   - Tx to this port
* outPort   (IN)   -  Rx Port
* mtu     (IN)     - MTU val for trap, packet which MTU exceeds this values will be traped
* vsi       (IN)  - To careate vlan between ports
* Return:    (int)
*******************************************************************************/
int etpp_lif_mtu__start_run_with_params(int unit,bcm_port_t inPort,bcm_port_t outPort, uint32 mtu,bcm_vlan_t vsi)
{
    etpp_lif_mtu_s etpp_lif_mtu_info;
    etpp_lif_mtu_struct_get(unit,&etpp_lif_mtu_info);
    etpp_lif_mtu_info.lif_creation_info[0].port = inPort;
    etpp_lif_mtu_info.lif_creation_info[1].port = outPort;
    etpp_lif_mtu_info.lif_creation_info[0].vsi = vsi;
    etpp_lif_mtu_info.lif_creation_info[1].vsi = vsi;
    etpp_lif_mtu_info.lif_mtu.mtu = mtu;
    etpp_lif_mtu__start_run(unit,&etpp_lif_mtu_info);
    
}
/*Main Function*/
/*****************************************************************************
* Function:  etpp_lif_mtu__start_run
* Purpose:   Main function run default configuration for setting all needed values for ETPP LIF MTU
* Params:
* unit (IN) - Device Number
* param (IN) - overrides the default configuration
* Return:    (int)
*******************************************************************************/
int etpp_lif_mtu__start_run(int unit, etpp_lif_mtu_s *param)
{
    int rv = BCM_E_NONE;
    etpp_lif_mtu_s etpp_lif_mtu_info;
    int i = 0;
    int gport_subtype;
    bcm_gport_t gport_id;

     rv = etpp_lif_mtu_init(unit);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in etpp_lif_mtu_init\n");
        return rv;
    }

    /*Enables the call etpp_lif_mtu__start_run(0,NULL)*/
    if(param == NULL)
    {
        etpp_lif_mtu_struct_get(unit,&etpp_lif_mtu_info);
    }
    else
    {
        sal_memcpy(&etpp_lif_mtu_info, param, sizeof(g_etpp_lif_mtu));
    }
    
   
    

    /*Create mirror cmd, destination id*/
    rv = mirror__destination_with_id__create(unit,&(etpp_lif_mtu_info.mirror_dst));
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in mirror__destination__create\n");
        return rv;
    }
    else
    {
        printf("Mirror destination created id:  0x%x \n", etpp_lif_mtu_info.mirror_dst.mirror_cmd );
    }

    /*Set Header*/
    for(i = 0 ; i < ETPP_LIF_MTU_NUM_OF_HEADER_CODES; i++)
    {
        if(etpp_lif_mtu_info.header_codes[i].value.value == TRUE)
        {
            rv = switch__control_indexed__set(unit,etpp_lif_mtu_info.header_codes[i]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in switch__control_indexed__set index $ i\n");
                return rv;
            }
            else
            {
                printf("Header set, index: %d \n",i);
            }
        }
    
    }
    /*Create LIF*/
    for(i = 0 ; i < ETPP_LIF_MTU_NUM_OF_USED_PORTS ; i++)
    {
        rv = vlan__port_vsi_egress_and_igress__create(unit,etpp_lif_mtu_info.lif_creation_info[i].vsi,
                                                          etpp_lif_mtu_info.lif_creation_info[i].port,
                                                         &gport_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in vlan__port_vsi_egress_and_igress_with_id__create index $i \n");
            return rv;
        }
    }
    /*Create port return the gport of global LIF*/
    etpp_lif_mtu_info.lif_mtu.gport = gport_id;
    etpp_lif_mtu_info.l2_entry.gport_id = gport_id;
    
    /*Set MTU val for the outlif*/
    rv = rx__lif_mtu__set(unit,&(etpp_lif_mtu_info.lif_mtu));
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in rx__lif_mtu__set\n");
        return rv;
    }
    else
    {
       printf("MTU %d for GPORT 0x%x was set\n",etpp_lif_mtu_info.lif_mtu.mtu,etpp_lif_mtu_info.lif_mtu.gport); 
    }

    /*Add entry to MAC Table*/
    rv = l2__mact_entry_create(unit,etpp_lif_mtu_info.l2_entry);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in l2__mact_entry_create\n");
        return rv;
    }
    else
    {
        printf("L2 entry set \n");
    }

    
    
    /*Create trap*/
    rv = rx__etpp_trap__set(unit,bcmRxTrapEgTxMtuFilter,&(etpp_lif_mtu_info.trap_utils),&etpp_lif_mtu_trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in rx__etpp_trap__set\n");
        return rv;
    }
    else
    {
        printf("Trap bcmRxTrapEgTxMtuFilter was created and set \n");
    }

    return rv;
    
}


 
