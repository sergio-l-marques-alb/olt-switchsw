/* 
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
 * $Id cint_fabric_egress_mc.c,v 1 Broadcom SDK$ 
 *  
 *  
 * Example walkthrough: 
 *  
 * This example shows how to configure a system of a single FE and multiple faps to replicate mc packets in the fabric and the fap's egress. 
 *  
 * The FE function configs the fabric replicate data to all FAPs which have a gport included in the gport array, gport_arr. 
 *  
 * The FAP function configs the FAP to replicate data to all the ports in gport_arr which are in the FAP. 
 *
 * cint_fabric_egress_mc_config shows an example of usage of the functions for 1 FE and 2 FAPs
 *  
 *  NOTEs :
 *      1. gports recived by FE and FAP funcitons are expected to be MODPORTs.
 *      2. The example assumes FE is configure to direct mode.
 *
 *
 *
 * Usage example : 
 * 
    cd ../../../../src/examples/dpp

    cint validate/cint_pp_util.c
    cint ../dfe/cint_fe3200_interop.c

    cd ../dcmn

    cint cint_fabric_egress_mc.c

    c

    print cint_fe3200_interop_example(1);

    int fap_0_unit = 0;
    int fap_0_id = 0;
    int fap_0_in_port = 14;

    int fap_1_unit = 2;
    int fap_1_id = 2;

    int fe_unit = 1;

    int nof_ports_fap_0 = 5;
    int ports_fap_0[nof_ports_fap_0] = {13,14,15,16,17};

    int nof_ports_fap_1 = 5;
    int ports_fap_1[nof_ports_fap_1] = {13,14,15,16,17};

    bcm_multicast_t mc_id=5007;

    bcm_gport_t gport;
    BCM_GPORT_MCAST_SET(gport, mc_id);
    print bcm_port_force_forward_set(fap_0_unit, fap_0_in_port, gport, 1);

    print cint_fabric_egress_mc_config( fap_0_unit,  fap_0_id,  fap_0_in_port,   fap_1_unit,  fap_1_id,  fe_unit, nof_ports_fap_0,  ports_fap_0,  nof_ports_fap_1,  ports_fap_1,   mc_id );
     
 *  
 */

int cint_fabric_egress_mc_config( int fap_0_unit, int fap_0_id, int fap_0_in_port,  int fap_1_unit, int fap_1_id, int fe_unit, 
                                 int nof_ports_fap_0, int *ports_fap_0, int nof_ports_fap_1, int *ports_fap_1, bcm_multicast_t mc_id )
{
    int rv, i, j;
    int nof_gports = nof_ports_fap_0 + nof_ports_fap_1;
    bcm_gport_t garr[nof_gports];

    if (nof_gports == 0) {
        printf("No destination ports defined\n");
        return rv;
    }
    
    for (i=0; i < nof_gports; i++) {
        if (i < nof_ports_fap_0) {
            BCM_GPORT_MODPORT_SET(garr[i], fap_0_id, ports_fap_0[i]); 
            printf(" ------------------------- Ports FAP 0 --- (%d) ---------------\n",ports_fap_0[i]);
        } else {
            BCM_GPORT_MODPORT_SET(garr[i], fap_1_id, ports_fap_1[i - nof_ports_fap_0]); 
            printf(" ------------------------- Ports FAP 1 --- (%d) ---------------\n",ports_fap_1[i - nof_ports_fap_0]);
        }
    }


    rv =  fabric_egress_mc_fap(fap_0_unit, mc_id, garr, nof_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, fabric_egress_mc_fap (%d)\n",fap_0_unit);
        return rv;
    } 

    rv = fabric_egress_mc_fap(fap_1_unit, mc_id, garr, nof_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, fabric_egress_mc_fap (%d)\n",fap_1_unit);
        return rv;
    } 

    rv = fabric_egress_mc_fe(fe_unit, mc_id,  garr, nof_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add (%d)\n",fap_0_unit);
        return rv;
    } 


    printf("cint_fabric_egress_mc_config: PASS\n\n");
}


int fabric_egress_mc_fe(int fe_unit, bcm_multicast_t mc_id,
                                   bcm_gport_t *gport_arr, int nof_gports)
{
    int rv;
    int i, j, tmp, new_modid, nof_modules = 0;
    bcm_module_t modid;

    bcm_gport_t;

    bcm_module_t modid_array[nof_gports];




    /*gports to modid's - 
      use gport array to create a an array of destenetion modid's */
    if (nof_gports > 0) {
        modid_array[0] = BCM_GPORT_MODPORT_MODID_GET(gport_arr[0]);
        nof_modules++;
    }

    for (i=1; i<nof_gports; i++) {
        modid = BCM_GPORT_MODPORT_MODID_GET(gport_arr[i]);
        new_modid = 1;
        for (j=0; j<nof_modules; j++) {
            if (modid == modid_array[j]) {
                new_modid = 0;
            }
        }
        if (new_modid == 1) {
            modid_array[i] = modid;
            nof_modules++;
        }
    } 

    /* 
     * Given multicast id, create and replicate multicast id over all requested devices. 
     */

    /* map local modid with modid */
    for (j = 0; j < nof_modules; j++)
    {
        tmp = j;
        BCM_FABRIC_LOCAL_MODID_SET(tmp);
        rv = bcm_fabric_modid_local_mapping_set(fe_unit, tmp, modid_array[j]);
        if(rv != 0) {
            printf("Error, in bcm_fabric_modid_local_mapping_set, rv=%d, \n", rv);
            return rv;
        }
    } 

    /* 
     * Create multicast id, and set fabric to it.
     */

    rv = bcm_multicast_create(fe_unit, BCM_MULTICAST_WITH_ID, &mc_id);
    if(rv != 0) {
        printf("Error, in bcm_multicast_create, rv=%d, \n", rv);
        return rv;
    }

    rv = bcm_fabric_multicast_set(fe_unit, mc_id, 0, nof_modules, modid_array);
    if(rv != 0) {
        printf("Error, in bcm_fabric_multicast_set, rv=%d, \n", rv);
        return rv;
    }
    
    printf("create_fe_mc_groups: PASS\n\n");
    return rv;

}


int fabric_egress_mc_fap(int fap_unit, bcm_multicast_t mc_id, bcm_gport_t *gport_arr, int nof_gports)
{
    int rv, i;
    int mymodid, modid;
    int nof_gports_in_fap = 0;
    bcm_if_t cud_array[nof_gports];

    bcm_gport_t gport_in_fap[nof_gports];

    bcm_stk_modid_get(fap_unit, &mymodid);

    /*gports to modid's - 
      use gport array to create a an array of modid's */
    for (i=0; i<nof_gports; i++) {
        modid = BCM_GPORT_MODPORT_MODID_GET(gport_arr[i]);
        if (modid == mymodid) {            
            gport_in_fap[nof_gports_in_fap] = gport_arr[i];
            cud_array[nof_gports_in_fap] = 1;
            nof_gports_in_fap++;
        }
        
    } 


    if (nof_gports_in_fap > 0) {
        

        /* mc_id egress fap_0 to gports */

        rv = bcm_multicast_create(fap_unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP, &mc_id);
        if(rv != 0) {
            printf("Error, in bcm_multicast_create, rv=%d, \n", rv);
            return rv;
        }

        rv = bcm_multicast_egress_set(fap_unit, mc_id, nof_gports_in_fap, gport_in_fap, cud_array);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_egress_set (%d)\n", fap_unit);
            return rv;
        }

    }


    printf("create_fap_mc_groups: PASS\n\n");
    return rv;
}



