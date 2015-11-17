

/* $Id: cint_mpls_term_flexible_utilities.c,v 1 2014/09/1 12:17:27 Rsportas Exp $
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
*/

/*
* add termination labels to one of two databases
*
*
* term_abel - termination label to enter to data base 
*
* index==1 to enter label to lsp data base in semA
* index==2 to enter MLDP label to PWE db in semB
*/

int
mpls_add_term_entry_to_db_aux(int unit, int term_label,int in_port,int index)
{
    int flags = 0;
    int tunnel_id_;
    int* tunnel_id = &tunnel_id_ ; 
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t entry; /* used for semA */

    int is_arad_plus;
    int tmpDB ;

	if (index == 1) {
		printf("\n--------------- adding termination label %d to semA to LSP DB ---------------\n", term_label); 
	}else{
		printf("\n--------------- adding termination label %d to semB to PWE DB(MLDP labels)---------------\n", term_label); 
	}


    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /* Uniform: inherit TTL and EXP, 
    * in general valid options: 
    *    both present (uniform) or none of them (Pipe)
    */
    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
		
	if (index == 1) {
		BCM_MPLS_INDEXED_LABEL_SET(entry.label, term_label,1); 
	}else{
		BCM_MPLS_INDEXED_LABEL_SET(entry.label, term_label,2);
	}	
    /* Enable when testing egress QOS, need to source cint_qos.c
    * This remarks the mpls egress packet
    */
 
    entry.qos_map_id = qos_map_id_mpls_ingress_get(unit);

    /* egress attributes */
    /* none as it just pop */
		
    rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
	printf("Error, in bcm_mpls_tunnel_switch_create\n");
	return rv;
    }

    return rv;
}



/*
set term_label at LSP database at semA ,
to this logical databases will be set TP,MLDP,TEUHP
labels
*/
int
mpls_add_term_entry_to_lsp_db(int unit, int term_label,int in_port)
{
		return mpls_add_term_entry_to_db_aux(unit,term_label,in_port,1);
}

/*
add mldp termination label to PWE database in semB
*/
int
mpls_add_term_entry_mldp_to_pwe_db(int unit, int term_label,int in_port)
{
		return mpls_add_term_entry_to_db_aux(unit,term_label,in_port,2);
}
