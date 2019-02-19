/* $Id: soc_sand_exact_match_hash.h,v 1.2 Broadcom SDK $
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
*/


#ifndef SOC_SAND_EXACT_MATCH_HASH_H_INCLUDED
/* { */
#define SOC_SAND_EXACT_MATCH_HASH_H_INCLUDED

/*************
* INCLUDES  *
*************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_exact_match.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */
/* } */


/*************
 * MACROS    *
 *************/
/* { */


/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */
/*************
* GLOBALS   *
*************/
/* { */

/* } */

/*************
* FUNCTIONS *
*************/
/* { */


void 
  soc_sand_exact_match_hash_key_to_verifier_31_19(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, 
    SOC_SAND_IN uint32 table_id, 
    SOC_SAND_OUT SOC_SAND_EXACT_MATCH_VERIFIER verifier
  );

SOC_SAND_EXACT_MATCH_HASH_VAL 
  soc_sand_exact_match_hash_key_to_hash_31_19(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, 
    SOC_SAND_IN uint32 table_id
  );

void 
  soc_sand_exact_match_hash_key_to_verifier_31_20(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, 
    SOC_SAND_IN uint32 table_id, 
    SOC_SAND_OUT SOC_SAND_EXACT_MATCH_VERIFIER verifier
  );

SOC_SAND_EXACT_MATCH_HASH_VAL 
  soc_sand_exact_match_hash_key_to_hash_31_20(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, 
    SOC_SAND_IN uint32 table_id
  );

void 
  soc_sand_exact_match_hash_key_to_verifier_25_12(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, 
    SOC_SAND_IN uint32 table_id, 
    SOC_SAND_OUT SOC_SAND_EXACT_MATCH_VERIFIER verifier
  );

SOC_SAND_EXACT_MATCH_HASH_VAL 
  soc_sand_exact_match_hash_key_to_hash_25_13(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, 
    SOC_SAND_IN uint32 table_id
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } SOC_SAND_EXACT_MATCH_HASH_H_INCLUDED*/
#endif



