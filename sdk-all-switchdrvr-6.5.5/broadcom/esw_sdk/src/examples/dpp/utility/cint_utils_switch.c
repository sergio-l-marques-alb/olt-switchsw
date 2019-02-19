/* $Id: cint_utils_switch.c,$
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
 * This file provides switch basic functionality 
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 *************************************************************************************************** */



/* ************************************************************************************************** */

/*****************************************************************************
* Function:  switch__link_layer_mtu_header_trigger
* Purpose:   Function trigger mtu layer filter check on input header
* Params:
* unit - Device number
* header - header type
* enable - TRUE for enable FALSE for disable

* Return:    (int)
*******************************************************************************/
int switch__link_layer_mtu_header_trigger(int unit,int header,int enable)
{
    
    int rv = BCM_E_NONE;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    key.type = bcmSwitchLinkLayerMtuFilter;
    key.index = header;
    value.value = enable;
    
    rv = bcm_switch_control_indexed_set(unit,key,value);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    } 
    return rv;
}


/*****************************************************************************
* Function:  switch__link_layer_mtu_value_per_profile_set
* Purpose:   Link layer in ETPP has 3 MTU profiles, this function set given MTU value to given MTU profile
* Params:
* unit - Device number
* mtu_profile -  MTU profile to set the value to (1-3)
* mtu_value -  MTU value set for given profile
* Return:    (int)
*******************************************************************************/

int switch__link_layer_mtu_value_per_profile_set(int unit,int mtu_profile,int mtu_value)
{
    int rv = BCM_E_NONE;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    key.type = bcmSwitchLinkLayerMtuSize;
    key.index = mtu_profile;
    value.value = mtu_value;

    if(mtu_profile < 1 || mtu_profile > 3 ) 
    {
        printf("MTU profile should be in range 1-3 \n");
        return BCM_E_PARAM;
    }
    
    rv = bcm_switch_control_indexed_set(unit,key,value);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    } 
    return rv;
}

