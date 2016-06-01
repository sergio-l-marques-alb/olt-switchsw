/* $Id: cint_utils_trap.c,$
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
 * This file provides rx basic functionality 
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 *************************************************************************************************** */


struct rx_etpp_trap_set_utils_s{
    /*
    *  mirror command
    */
    int mirror_cmd;
    /*
    *  mirror strength
    */
    int mirror_strength;
    /*
    *  mirror enable
    */
    int mirror_enable;
    /*
    *  forward strength
    */
    int fwd_strength;
    /*
    *  forward enable
    */
    int fwd_enable;
};

struct rx_lif_mtu_set_utils{
    uint32 flags;
    bcm_port_t gport; /*Gport as Global LIF*/
    uint32 mtu;
};



/* ************************************************************************************************** */


/*****************************************************************************
* Function:  rx__etpp_trap__set
* Purpose:   
* Params:
* unit        - 
* trap_type   - 
* etpp_config - 
* trap_id     - 
* Return:    (int)
*******************************************************************************/
int rx__etpp_trap__set(int unit, bcm_rx_trap_t trap_type,rx_etpp_trap_set_utils_s *etpp_config, int* trap_id)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config ={0};
    bcm_mirror_options_t mirror_cmd = {0};
    int trap_create_flags = 0; /*Trap create flags*/

  
    /*Check input*/
    
    if(etpp_config == NULL || trap_id == NULL)
    {
        printf("Error,  NULL param \n");
        return BCM_E_PARAM;
    }
    
    /*Configure etpp trap*/
    mirror_cmd.recycle_cmd = etpp_config->mirror_cmd;
    mirror_cmd.copy_strength = etpp_config->mirror_strength;
    config.trap_strength = etpp_config->fwd_strength;
    
    if(etpp_config->mirror_enable == TRUE)
    {
        
        mirror_cmd.flags |= BCM_MIRROR_OPTIONS_COPY_DROP;
    }
    if(etpp_config->fwd_enable == FALSE)
    {
       
        config.dest_port = BCM_GPORT_BLACK_HOLE;
        config.flags |= BCM_RX_TRAP_UPDATE_DEST;
    }

    config.mirror_cmd = &mirror_cmd;
    
    /*Create ETTP trap*/
    rv = bcm_rx_trap_type_create(unit,trap_create_flags,trap_type,trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    }

    /*Set the trap */
    rv = bcm_rx_trap_set(unit,(*trap_id),&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set \n");
        return rv;
    } 
    
    return rv;
}


/*****************************************************************************
* Function:  rx__ingress_trap_to_cpu
* Purpose:   Trap packet to CPU
* Params:
* unit      - Device Number
* trap_type - Can be ingress trap predefined of bcmTrapUserDefine
* trap_id   - Will return the trap Id of the created trap
* Return:    (int)
*******************************************************************************/
int rx__ingress_trap_to_cpu(int unit, bcm_rx_trap_t trap_type, int trap_strength, int* trap_id)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config ={0};

    int trap_create_flags = 0; /*Trap create flags*/

    /*Check input*/
    if(trap_id == NULL)
    {
        printf("Error,  NULL param \n");
        return BCM_E_PARAM;
    }

    config.trap_strength = trap_strength;
    config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE);
    config.dest_port = BCM_GPORT_LOCAL_CPU;

    /*Create Predefind trap*/
    rv = bcm_rx_trap_type_create(unit,trap_create_flags,trap_type,trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    }


    /*Set the trap */
    rv = bcm_rx_trap_set(unit,(*trap_id),&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set \n");
        return rv;
    } 

    return rv;
    
}

/*****************************************************************************
* Function:  rx__ingress_trap_drop
* Purpose:   Configure ingress trap to drop packets
* Params:
* unit      - Device Number
* trap_type - Can be ingress trap predefined of bcmTrapUserDefine
* trap_id   - Will return the trap Id of the created trap
* Return:    (int)
*******************************************************************************/
int rx__ingress_trap_drop(int unit, bcm_rx_trap_t trap_type, int trap_strength, int* trap_id)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config = {0};

    int trap_create_flags = 0; /*Trap create flags*/

    /*Check input*/
    if(trap_id == NULL)
    {
        printf("Error,  NULL param \n");
        return BCM_E_PARAM;
    }

    config.trap_strength = trap_strength;
    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    /*Create Predefind trap*/
    rv = bcm_rx_trap_type_create(unit, trap_create_flags, trap_type, trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    }


    /*Set the trap */
    rv = bcm_rx_trap_set(unit, (*trap_id), &config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set \n");
        return rv;
    } 

    return rv;    
}

/*****************************************************************************
* Function:  rx__trap_create_and_set
* Purpose:   Create trap and set the params with config
* Params:
* unit              - Device number
* trap_type         - Trap type to create
* trap_create_flags - Flags for api bcm_rx_trap_type_create
* trap_config       - Config of trap
* trap_id    (OUT)       - Trap id that created 
* Return:    (int)
*******************************************************************************/
int rx__trap_create_and_set(int unit,bcm_rx_trap_t trap_type,int trap_create_flags,
                                    bcm_rx_trap_config_t *trap_config,int *trap_id)
{
    int rv = BCM_E_NONE;
    
    rv = bcm_rx_trap_type_create(unit, trap_create_flags, trap_type, trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    }


    /*Set the trap */
    rv = bcm_rx_trap_set(unit, (*trap_id), trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set \n");
        return rv;
    } 

    return rv;    
}


/*****************************************************************************
* Function:  rx__lif_mtu__set
* Purpose:   Set an MTU to specific LIF
* Params:
* unit             - Device Nuber
* lif_mtu_set_info - Hold params Which MTU to set To which LIF
* Return:    (int)
*******************************************************************************/
int rx__lif_mtu__set(int unit,rx_lif_mtu_set_utils *lif_mtu_set_info)
{
    int rv = BCM_E_NONE;
    rv = bcm_rx_lif_mtu_set(unit, lif_mtu_set_info->flags,lif_mtu_set_info->gport,lif_mtu_set_info->mtu);
    
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_lif_mtu_set \n");
        return rv;
    }
    return rv;
}

/*****************************************************************************
* Function:  rx__trap_destroy
* Purpose:   Destroy the given trap id
* Params:
* unit    - Device Number
* trap_id - Trap id to Destroy
* Return:    (int)
*******************************************************************************/
int rx__trap_destroy(int unit,int trap_id)
{
     int rv = BCM_E_NONE;

    /*delete trap*/
    rv = bcm_rx_trap_type_destroy(unit, trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : bcm_rx_trap_type_destroy, returned %d\n", rv);
        return rv;
    }

    return rv;
}




