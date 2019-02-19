/*
 * $Id: simintf.h,v 1.6 Broadcom SDK $
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
 * SBX SIM SDK interface
 */

#ifndef __SOC_SBX_SIM_
#define __SOC_SBX_SIM_



/*
 * Function:
 *    soc_sbx_caladan3_sim_block_encode_simple
 * Purpose
 *    Routine to encode a block, table info into text for messaging
 *    returns error status
 */
int soc_sbx_caladan3_sim_block_encode_simple(char *buffer, char *block, char *table, char *op);

/*
 * Function:
 *    soc_sbx_caladan3_sim_block_encode
 * Purpose
 *    Routine to encode a block, table info into text for messaging
 *    returns error status
 */
int soc_sbx_caladan3_sim_block_encode(char *buffer, char *block,
                                    char *table, char *op, uint32 addr);

/*
 * Function:
 *    soc_sbx_caladan3_sim_block_decode
 * Purpose
 *    Routine to decode a block, table info into text for messaging
 *    returns error status
 */
int soc_sbx_caladan3_sim_block_decode(char *buffer, char *block,
                                    char *table, char *op, uint32 *addr);
/*
 * Function:
 *    soc_sbx_caladan3_sim_field_encode
 * Purpose
 *    Routine to encode a {field value} pair into text for messaging
 *    returns error status
 */
int soc_sbx_caladan3_sim_field_encode(char *buffer, char *field,
                                    uint8 *value, int width);

/*
 * Function:
 *    soc_sbx_caladan3_sim_field_decode
 * Purpose
 *    Routine to decode and extract value from {field value} pair from a text message
 * message
 *    Returns error status
 */
int soc_sbx_caladan3_sim_field_decode(char *buffer, int size, char *field,
                                     uint8 *value, int width);

/*
 * Function:
 *    soc_sbx_caladan3_sim_sendrcv
 * Purpose
 *    Send a message to the sim server and get the response back
 *    No processing of the message happens here, its up to the client
 */
int soc_sbx_caladan3_sim_sendrcv(int unit, char *buffer, int *size);

/*
 * Function:
 *    soc_sbx_caladan3_sim_status_decode
 * Purpose
 *    Routine to decode a response status, from message
 *    returns error status
 */
int soc_sbx_caladan3_sim_status_decode(char *buffer, char *verb, int *parsedlen);

/*
 * Function:
 *    soc_sbx_caladan3_sim_verb_decode
 * Purpose
 *    Routine to decode a verb,value from message
 *    returns value and the length of message consumed in parsedlen
 */
int soc_sbx_caladan3_sim_verb_decode(char *buffer, char *verb, int *parsedlen);

/*
 * Function:
 *    soc_sbx_caladan3_sim_verb_encode
 * Purpose
 *    Routine to encode verb value into message if verb given 
 *    or just encode the value if no verb given
 *    returns value and length of message generated
 */
int
soc_sbx_caladan3_sim_verb_encode(char *buffer, char *verb, int value);

/*
 * Function:
 *   soc_sbx_caladan3_sim_split
 * Purpose
 *   Break a string of data into field,value pairs
 */
int
soc_sbx_caladan3_sim_split(char *buffer, int size, char**fields, char**values);

/*
 * Function:
 *    soc_sbx_caladan3_sim_keyword_encode
 * Purpose
 *    Routine to encode keyword value into message 
 */
int
soc_sbx_caladan3_sim_keyword_encode(char *buffer, char *keyword);

/*
 * Function:
 *    soc_sbx_caladan3_sim_keyword_decode
 * Purpose
 *    Routine to check keyword in message ,
 */
int
soc_sbx_caladan3_sim_keyword_decode(char *buffer, char *keyword);

/*
 *   Function
 *     sbx_caladan3_cop_policer_create_sim
 *   Purpose
 *      COP create a policer in simulation
 */
int 
soc_sbx_caladan3_cop_policer_create_sim(int unit,
					uint32 cop,
					uint32 segment,
					uint32 policer,
					soc_sbx_caladan3_cop_policer_config_t *config,
					uint32 *handle);

#endif
