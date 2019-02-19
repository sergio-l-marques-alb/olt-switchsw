
/*
 * $id cint_end_tlv.c $
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
 *
 * End TLV workaround:
 * Add a value to the ACE VAR to use the End TLV workaround for OAM CCM packets with
 * port/interface status TLV fields.
 *
 * In order to use port/interface status TLV in received OAM CCM packets, call:
 * > cint utility/cint_utils_field.c
 * > cint cint_field_oam_bfd_advanced.c
 * > cint
 * > field_oam_advanced_features(unit);
 *
 * In order to use BFD flags events when the C-Flag may be set in received PDUs, call:
 * > cint utility/cint_utils_field.c
 * > cint cint_field_oam_bfd_advanced.c
 * > cint
 * > field_bfd_advanced_features(unit,<hw-trap-code>);
 * where <hw-trap-code> is dependent on the traffic type (BFDoIPv4, BFDoPWE, etc.)
 * and can be found in the traps application notes under appendix A.
 *
 */


/*
 * Sets the key to be [OAM-Trap-code, OAMP-dest-port] and sets the value for that key
 * That value is used to identify packets trapped to the OAMP and apply different workarounds if necessary.
 */
int field_oam_advanced_features(int unit)
{
    int result;
    bcm_field_group_t grp_tcam = 1;
    int group_priority = 10;
    int qual_id = 1;

    /* Create field group for OAM EndTLV WA */
    result = field__prge_action_create_field_group(unit, group_priority, grp_tcam, qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in prge_end_tlv_create_field_group\n");
        return result;
    }

    /* Set action code for the created group */
    printf("Adding entry to mark OAM packets for advances processing\n");
    result = field__prge_action_code_add_entry(unit, grp_tcam, 0xE0/*trap_code (OAM)*/, 232/*dest_port (OAMP PP port)*/,
                                               qual_id, prge_action_code_oam_end_tlv_wa);
    if (BCM_E_NONE != result) {
        printf("Error in prge_end_tlv_add_entry\n");
        return result;
    }

    return result;
}

/*
 * Sets the key to be [Trap-code, OAMP-dest-port] and sets the value for that key
 * That value is used to identify packets trapped to the OAMP and apply different workarounds if necessary.
 */
int field_bfd_advanced_features(int unit, uint8 trap_code)
{
    int result;
    bcm_field_group_t grp_tcam = 1;
    int group_priority = 10;
    int qual_id = 1;
    char *proc_name  ;

    proc_name = "field_bfd_advanced_features" ;
    printf("%s(): Enter\n",proc_name);

    /* Create field group for OAM EndTLV WA */
    result = field__prge_action_create_field_group(unit, group_priority, grp_tcam, qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in prge_end_tlv_create_field_group\n");
        return result;
    }

    /* Set action code for the created group */
    result = field__prge_action_code_add_entry(unit, grp_tcam, trap_code, 232/*dest_port (OAMP PP port)*/,
                                               qual_id, prge_action_code_bfd_c_bit_clear_wa);
    if (BCM_E_NONE != result) {
        printf("Error in prge_end_tlv_add_entry\n");
        return result;
    }

    printf("%s(): Exit OK\n",proc_name);
    return result;
}
