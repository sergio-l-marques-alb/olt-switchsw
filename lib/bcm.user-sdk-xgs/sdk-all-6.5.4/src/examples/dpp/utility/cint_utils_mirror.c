/* $Id: cint_utils_mirror.c,$
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

struct mirror_create_with_id_s{
   int mirror_cmd;
   int sys_port;
   int mc_id;
};



/* ************************************************************************************************** */


/*****************************************************************************
* Function:  mirror__full_destination__create
* Purpose:   Create mirro destination by entring full config
* Params:
* unit        - D
* mirror_dest - Full mirror dest configuration
* Return:    (int)
*******************************************************************************/
int mirror__full_destination__create(int unit, bcm_mirror_destination_t *mirror_dest)
{
    int rv = BCM_E_NONE;
 
    int flags = 0;

    if(mirror_dest == NULL)
    {
        printf("Error, mirror_dest is NULL \n");
        return BCM_E_PARAM;
    }
    
    rv = bcm_mirror_destination_create(unit,mirror_dest);
    
    return rv;
}


/*****************************************************************************
* Function:  mirror__destination_with_id__create
* Purpose:   Create destination WITH_ID to sys_port/mc
* Params:
* unit       - Device Number
* info (IN) - include:
        mirror_cmd - destination id
        sys_port   - system port
        mc_id      - Mulicast id otherwise 0
* Return:    (int)
*******************************************************************************/
int mirror__destination_with_id__create(int unit, mirror_create_with_id_s *info){
    bcm_mirror_destination_t  dest = {0};
    bcm_error_t               rv = BCM_E_NONE;

    /* Define the mirror command (system port or MC) */
    if(info->mc_id == FALSE)
    {
        BCM_GPORT_SYSTEM_PORT_ID_SET(dest.gport, info->sys_port);
    } 
    else 
    {
        BCM_GPORT_MCAST_SET(dest.gport, info->mc_id);
    }
    
    BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, info->mirror_cmd);
    
    dest.flags = BCM_MIRROR_DEST_WITH_ID;
    rv = bcm_mirror_destination_create(unit, &dest);
    
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_mirror_destination_creates failed $rv \n");
        return rv;
    }
    return rv;
}



