/* $Id: cint_utils_qos.c,$
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
int qos__create_egress_qos_profile_with_id_color_and_priority
                                (int unit,int id,int color,int prio)
{
    int rc = BCM_E_NONE;
    bcm_qos_map_t map;
    int flags = 0;
    int qos_map_id = id;
     /*Configure qos*/
    bcm_qos_map_t_init(&map);
    map.int_pri = prio;
    map.color = color;
    rc = qos__create_egress_qos_profile(unit,flags,&map,&qos_map_id,TRUE);
    if (BCM_E_NONE != rc)
    {
        printf("Error in qos__create_egress_qos_profile \n");
        return rc;
    }
    return rc;
}
/* creates an entry in the cos profile table which will update the TC and DP in ETPP*/
int qos__create_egress_qos_profile(/*in*/ int unit, int flags, bcm_qos_map_t *map,/*out*/ int *qos_map_id,int with_id)
{
    int rc = BCM_E_NONE;
    int create_flags = BCM_QOS_MAP_EGRESS| BCM_QOS_MAP_ENCAP_INTPRI_COLOR;

    if(with_id)
    {
        create_flags |= BCM_QOS_MAP_WITH_ID;
    }

    rc = bcm_qos_map_create(unit, create_flags, qos_map_id);
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_create\n");
        return rc;
    }

     printf("Created an entry with id: %d,  0x%x \n", *qos_map_id,*qos_map_id);


    rc = bcm_qos_map_add(unit, flags, map, (*qos_map_id));
    if (BCM_E_NONE != rc) 
    {
        printf("Error in bcm_qos_map_add %d \n",rc);
        return rc;
    }
    
   
    return rc;


}

/* Deletes and destroys the cos profile entry*/
int qos__delete_cos_profile(/*in*/ int unit, int flags, int cos_id)
{
    /* dummy map struct for the function */
    bcm_qos_map_t map;
    int rc = BCM_E_NONE;
    bcm_qos_map_t_init(&map);

    bcm_qos_map_delete( unit, flags, &map, cos_id); 
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_delete\n");
        return rc;
    }
    bcm_qos_map_destroy(unit, cos_id);
    if (BCM_E_NONE != rc)
    {
        printf("Error in bcm_qos_map_destroy\n");
        return rc;
    }

    return rc;

}

