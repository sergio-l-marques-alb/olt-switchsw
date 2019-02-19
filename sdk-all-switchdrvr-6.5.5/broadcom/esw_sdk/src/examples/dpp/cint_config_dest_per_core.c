/* $Id: cint_ser Exp $
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
 * Test Scenario: Configure different destination per core
 * 
 * how to run the test: 
  cint ../../../../src/examples/dpp/utility/cint_utils_rx.c
  cint ../../../../src/examples/dpp/cint_config_dest_per_core.c
  cint
  dest_per_core__start_run_TrapSaEqualsDa(0);
  exit;
 * 
 * 
 
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */


int _trap_id=0;



/*****************************************************************************
* Function:  _trap_destroy
* Purpose:   Destroy the created trap
* Params:
* unit (IN)     - Device Number 
* Return:    (int)
*******************************************************************************/
int _trap_destroy(int unit) 
{
    int rv = BCM_E_NONE;

    /*delete trap*/
    rv = rx__trap_destroy(unit, _trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : ser_ingress_trap_destroy, returned %d\n", rv);
        return rv;
    }

    return rv;
}

/*****************************************************************************
* Function:  dest_per_core__start_run_TrapSaEqualsDa
* Purpose:   Packet that arrive on core 0 will be dropped and packet on core 1 will be forwarded to CPU
* Params:
* unit - Device Number
* Return:    (int)
*******************************************************************************/
int dest_per_core__start_run_TrapSaEqualsDa(int unit)
{
    int rv = BCM_E_NONE;
    /*Packet that arrive on core 0 will be dropped and packet on core 1 will be forwarded to CPU*/
    rv = dest_per_core__start_run(unit,bcmRxTrapSaEqualsDa,4,BCM_GPORT_BLACK_HOLE,BCM_GPORT_LOCAL_CPU);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in dest_per_core__start_run\n");
        return rv;
    }
}
    

/*Main Function*/

/*****************************************************************************
* Function:  dest_per_core__start_run
* Purpose:   
* Params:
* unit      - Device Number
* trap_type - Type to create
* trap_strength - strength of trap
* dest0  - Destination for core0
* dest1  - Destination for core1
* Return:    (int)
*******************************************************************************/
int dest_per_core__start_run(int unit,bcm_rx_trap_t trap_type,int trap_strength,bcm_gport_t dest0,bcm_gport_t dest1)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_core_config_t core_config_arr[2];
    int trap_create_flags = 0;

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST);
    trap_config.trap_strength = trap_strength;
    trap_config.core_config_arr_len = 2;
    core_config_arr[0].dest_port = dest0;
    core_config_arr[1].dest_port = dest1;
    trap_config.core_config_arr = &core_config_arr;
    


    rv = rx__trap_create_and_set(unit, trap_type, trap_create_flags, &trap_config, &_trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in rx__trap_create_and_set\n");
        return rv;
    }

    return rv;
    
}

 
