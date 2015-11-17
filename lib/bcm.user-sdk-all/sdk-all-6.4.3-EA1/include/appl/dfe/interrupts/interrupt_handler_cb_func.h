/*
 * $Id: interrupt_handler_cb_func.h,v 1.2 Broadcom SDK $
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
 */

#ifndef _DFE_INTERRUPT_HANDLER_CB_FUNC_H_
#define _DFE_INTERRUPT_HANDLER_CB_FUNC_H_

/* init function - add handler function to db */
void fe1600_interrupt_add_interrupt_handler_init(int unit);

/* Generic None handles - for CB without specific handling */
int fe1600_interrupt_handle_generic_none(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char *msg);
int fe1600_interrupt_recurring_action_handle_generic_none(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char *msg);

/*
 *interrupt handler functions.
 */
int fe1600_interrupt_handle_eci_serint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_eci_dch0intforce(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_eci_dch1intforce(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_eci_dch2intforce(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_eci_dch3intforce(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_dchdesccntop(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_dchunexpcellp(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_ifmafopint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dch_ifmafopint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_ifmbfopint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dch_ifmbfopint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_cpudatacellfnea0pint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_cpudatacellfnea1pint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_altopint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dch_altopint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_unreachdestevpint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_dchunexperror(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_dchdesccntos(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_dchunexpcells(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_ifmafosint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dch_ifmafosint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_ifmbfosint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dch_ifmbfosint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_cpudatacellfnea0sint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_cpudatacellfnea1sint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_altosint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dch_altosint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_unreachdestevsint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_outofsyncintp(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_outofsyncints(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_ecc_1berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dch_ecc_1berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_ecc_2berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_parityerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_errorfilterpint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dch_errorfiltersint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_ccp0srcdvcnglinkint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcl_ccp0srcdvcnglinkint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_ccp1srcdvcnglinkint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcl_ccp1srcdvcnglinkint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_ccp0chfovfint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcl_ccp0chfovfint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_ccp0clfovfint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcl_ccp0clfovfint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_ccp1chfovfint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcl_ccp1chfovfint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_ccp1clfovfint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcl_ccp1clfovfint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_dclutagparerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcl_dclutagparerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_dclmtagparerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcl_dclmtagparerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_macadatacrcerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcl_macadatacrcerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_macbdatacrcerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcl_macbdatacrcerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_ecc_1berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcl_ecc_1berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_ecc_2berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_parityerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcl_outofsyncint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcma_ecc_1berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcma_ecc_1berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcma_ecc_2berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcma_parityerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcma_outofsyncpint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcma_outofsyncsint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcmb_ecc_1berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_dcmb_ecc_1berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcmb_ecc_2berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcmb_parityerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcmb_outofsyncpint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_dcmb_outofsyncsint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_ccs_cpucaptcellfneint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_ccs_unrchdstint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_ccs_ecc_2berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_ccs_ecc_2berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_ccs_parityerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_ccs_parityerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_ecc_2berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_parityerrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_0_querydestinationacove1kint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_1_querydestinationacove1kint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_2_querydestinationacove1kint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_3_querydestinationacove1kint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_0_querydestinationacove1kint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_1_querydestinationacove1kint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_2_querydestinationacove1kint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_3_querydestinationacove1kint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_crh_0_querydestinationacove1kint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_crh_1_querydestinationacove1kint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_0_querydestinationabovemaxbaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_1_querydestinationabovemaxbaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_2_querydestinationabovemaxbaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_3_querydestinationabovemaxbaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_0_querydestinationaboveupdatebaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_1_querydestinationaboveupdatebaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_2_querydestinationaboveupdatebaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_3_querydestinationaboveupdatebaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_0_querydestinationabovemaxbaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_1_querydestinationabovemaxbaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_2_querydestinationabovemaxbaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_3_querydestinationabovemaxbaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_0_querydestinationaboveupdatebaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_1_querydestinationaboveupdatebaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_2_querydestinationaboveupdatebaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_3_querydestinationaboveupdatebaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_crh_0_querydestinationabovemaxbaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_crh_1_querydestinationabovemaxbaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_crh_0_querydestinationaboveupdatebaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_crh_1_querydestinationaboveupdatebaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_0_queryunreachablemulticastint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_1_queryunreachablemulticastint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_2_queryunreachablemulticastint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhp_3_queryunreachablemulticastint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_0_queryunreachablemulticastint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_1_queryunreachablemulticastint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_2_queryunreachablemulticastint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_drhs_3_queryunreachablemulticastint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_rtp_linkintegritychangedint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_rxcrcerrn_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_mac_rxcrcerrn_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_rxcrcerrn_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_mac_rxcrcerrn_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_rxcrcerrn_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_mac_rxcrcerrn_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_rxcrcerrn_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_mac_rxcrcerrn_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_wrongsize_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_mac_wrongsize_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_wrongsize_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_mac_wrongsize_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_wrongsize_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_mac_wrongsize_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_wrongsize_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_mac_wrongsize_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_los_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_los_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_los_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_los_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_rxlostofsync_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_rxlostofsync_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_rxlostofsync_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_rxlostofsync_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_lnklvlagen_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_lnklvlagen_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_lnklvlagen_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_lnklvlagen_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_oof_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_mac_oof_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_oof_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_mac_oof_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_oof_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_mac_oof_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_oof_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_recurring_action_handle_mac_oof_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_decerr_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_decerr_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_decerr_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_decerr_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_transmiterr_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_transmiterr_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_transmiterr_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_transmiterr_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_rxctrloverflow_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_rxctrloverflow_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_rxctrloverflow_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_rxctrloverflow_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_lnklvlagectxbn_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_lnklvlagectxbn_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_lnklvlagectxbn_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_lnklvlagectxbn_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_txfdrciffault_int_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_txfdrciffault_int_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_txfdrciffault_int_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_txfdrciffault_int_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_ecc_1berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_mac_ecc_2berrint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_0(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_1(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_2(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);
int fe1600_interrupt_handle_srd_fsrd_0_syncstatuschanged_3(int unit, int block_instance, fe1600_interrupt_type en_fe1600_inter, char *msg);

#endif /* _DFE_INTERRUPT_HANDLER_CB_FUNC_H_ */
