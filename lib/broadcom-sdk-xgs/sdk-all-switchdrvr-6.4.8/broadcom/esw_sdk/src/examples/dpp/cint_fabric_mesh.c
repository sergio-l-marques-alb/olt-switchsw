/*
 * $Id: cint_fabric_mesh.c,v 1.3 Broadcom SDK $
 *
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
 */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~Fabric: Fabric Mesh configurations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  
 * File:        cint_fabric_mesh.c
 * Purpose:     Example of configuration for topology settings in fabric mesh mode.
 * 
 * It is assumed diag_init is executed.
 *
 * Settings include:
 *  o  set modid groups  
 *  o  set link topology
 * 
 *  CINT usage:
 *  o    Run application: 
 *  o  run main function fabric_mesh_config_example
 */


/*  set_modid_group
 *  local_dest_id 0-2 (0-1 in MESH_MC)
 *  fap_id_count is the number of faps in fap_id_array
*/ 
int set_modid_group(int unit, int local_dest_id, int fap_id_count, bcm_module_t *fap_id_array)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_module_t group_id;

    /* Set group_id according to the given local_dest_id */
    group_id = BCM_FABRIC_GROUP_MODID_SET(local_dest_id);

    /* Set modid group with given fap-id's*/
    rv = bcm_fabric_modid_group_set(unit, group_id, fap_id_count, fap_id_array);
    if (rv != BCM_E_NONE) {
      printf("Error, in modid group set, group_id  $group_id \n");
      return rv;
    }
    return rv;
}


/*  set_mesh_topology_configurations
 *  local_dest_id 0-2 (0-1 in MESH_MC)
 *  links_count is the number of links in links_array 
*/ 
int set_link_topology(int unit, int local_dest_id, int links_count, bcm_port_t *links_array)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_module_t group_id;

    /* Set group_id according to the given local_dest_id */
    group_id = BCM_FABRIC_GROUP_MODID_SET(local_dest_id);

    /* Set fabric link topology with given links*/
    rv = bcm_fabric_link_topology_set(unit, group_id, links_count, links_array);
    if (rv != BCM_E_NONE) {
      printf("Error, in link topology set, group_id  $group_id \n");
      return rv;
    }
    return rv;
}


/* 
*  Main function
*  Jericho functionality example
*
*/
int fabric_mesh_config_example(int unit, int local_dest_id, int fap_id_count, bcm_module_t *fap_id_array, int links_count, bcm_module_t *links_array)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Set modid group */
    rv = set_modid_group(unit, local_dest_id, fap_id_count, fap_id_array);
    if (rv != BCM_E_NONE) return rv;

    /* Set link topology */
    rv = set_link_topology(unit, local_dest_id, links_count, links_array);
    if (rv != BCM_E_NONE) return rv;

    printf("fabric_mesh_config_example: PASS\n\n");

    return rv;
}


