/* $Id: soc_sand_exact_match.h,v 1.3 Broadcom SDK $
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
#ifndef SOC_SAND_EXACT_MATCH_H_INCLUDED
/* { */
#define SOC_SAND_EXACT_MATCH_H_INCLUDED

/*************
* INCLUDES  *
*************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_SAND_EXACT_MATCH_NOF_TABLES 2

#define SOC_SAND_EXACT_MATCH_NULL  SOC_SAND_U32_MAX
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
typedef enum
{
  SOC_SAND_EXACT_MATCH_USE_AC=0,
  SOC_SAND_EXACT_MATCH_USE_OAM,
  SOC_SAND_EXACT_MATCH_NOF_USES
}SOC_SAND_EXACT_MATCH_USE;

typedef uint8* SOC_SAND_EXACT_MATCH_KEY;
typedef uint8* SOC_SAND_EXACT_MATCH_PAYLOAD;
typedef uint32 SOC_SAND_EXACT_MATCH_HASH_VAL;
typedef uint8* SOC_SAND_EXACT_MATCH_VERIFIER;
typedef uint32 SOC_SAND_EXACT_MATCH_ITER;

typedef
  uint32
    (*SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_SET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_INOUT  uint8                 *buffer,
      SOC_SAND_IN  uint32                   offset,
      SOC_SAND_IN  uint32                   len,
      SOC_SAND_IN uint8                     *data
    );

typedef
  uint32
    (*SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_GET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_IN  uint8                    *buffer,
      SOC_SAND_IN  uint32                   offset,
      SOC_SAND_IN  uint32                   len,
      SOC_SAND_OUT uint8                    *data
    );

typedef
  uint32
    (*SOC_SAND_EXACT_MATCH_HW_ENTRY_SET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   tbl_ndx,
      SOC_SAND_IN  uint32                   entry_offset,
      SOC_SAND_IN SOC_SAND_EXACT_MATCH_PAYLOAD        payload, 
      SOC_SAND_IN SOC_SAND_EXACT_MATCH_VERIFIER       verifier
    );

typedef
  uint32
    (*SOC_SAND_EXACT_MATCH_HW_ENTRY_GET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   tbl_ndx,
      SOC_SAND_IN  uint32                   entry_offset,
      SOC_SAND_OUT SOC_SAND_EXACT_MATCH_PAYLOAD        payload, 
      SOC_SAND_OUT SOC_SAND_EXACT_MATCH_VERIFIER       verifier
    );

typedef
  uint32
    (*SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_SET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_IN  uint32                   tbl_ndx,
      SOC_SAND_IN  uint32                   entry_offset,
      SOC_SAND_IN  uint8                  is_valid
    );

typedef
  uint32
    (*SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_GET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_IN  uint32                   tbl_ndx,
      SOC_SAND_IN  uint32                   entry_offset,
      SOC_SAND_OUT uint8                  *is_valid
    );

typedef
  SOC_SAND_EXACT_MATCH_HASH_VAL
    (*SOC_SAND_EXACT_MATCH_KEY_TO_HASH)(
      SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key,
      SOC_SAND_IN uint32 table_id
    );

typedef
  void
    (*SOC_SAND_EXACT_MATCH_KEY_TO_VERIFY)(
      SOC_SAND_IN   SOC_SAND_EXACT_MATCH_KEY key,
      SOC_SAND_IN   uint32 table_ndx,
      SOC_SAND_OUT  SOC_SAND_EXACT_MATCH_VERIFIER verifier
    );


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
  SOC_SAND_EXACT_MATCH_KEY  key;
  SOC_SAND_EXACT_MATCH_PAYLOAD payload; 
  SOC_SAND_EXACT_MATCH_VERIFIER verifier;
} SOC_SAND_EXACT_MATCH_ENTRY;

typedef struct {

  SOC_SAND_EXACT_MATCH_KEY  key; 
  SOC_SAND_EXACT_MATCH_HASH_VAL  hash_indx;
} SOC_SAND_EXACT_MATCH_STACK_ENTRY;

typedef struct {
  int prime_handle;
  uint32 sec_handle;
  uint32 max_insert_steps;
  uint32 key_bits;
  uint32 hash_bits;
  uint32 verifier_bits;

  SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_SET set_entry_fun;
  SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_GET get_entry_fun;
  SOC_SAND_EXACT_MATCH_HW_ENTRY_SET  hw_set_fun;
  SOC_SAND_EXACT_MATCH_HW_ENTRY_GET  hw_get_fun;
  SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_SET  is_valid_entry_set;
  SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_GET  is_valid_entry_get;

  SOC_SAND_EXACT_MATCH_HASH_VAL (*key_to_hash)(SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, SOC_SAND_IN uint32 table_id); /*get hash0 or hash1*/
  void (*key_to_verifier)(SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, SOC_SAND_IN uint32 table_ndx, SOC_SAND_OUT SOC_SAND_EXACT_MATCH_VERIFIER verifier); /*get verifier0 or verifier1*/

} SOC_SAND_EXACT_MATCH_INIT_INFO;

typedef struct
{
  SOC_SAND_EXACT_MATCH_KEY  keys[SOC_SAND_EXACT_MATCH_NOF_TABLES]; 
  uint32 *use_bitmap[SOC_SAND_EXACT_MATCH_NOF_TABLES];
  SOC_SAND_EXACT_MATCH_STACK_ENTRY *stack[SOC_SAND_EXACT_MATCH_NOF_TABLES];
  SOC_SAND_EXACT_MATCH_ENTRY *tmp_entry;
  uint32 table_size;
  uint32 key_size;
  uint32 payload_size;
  uint32  verifier_size;

} SOC_SAND_EXACT_MATCH_T;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  SOC_SAND_EXACT_MATCH_INIT_INFO
    init_info;

  SOC_SAND_EXACT_MATCH_T
    mgmt_info;
} SOC_SAND_EXACT_MATCH_INFO;

/* } */

/*************
* GLOBALS   *
*************/
/* { */

/* } */

/*************
* FUNCTIONS *
*************/
/* { */

uint32
  soc_sand_exact_match_create(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match
  );

uint32
  soc_sand_exact_match_destroy(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match
  );

uint32
  soc_sand_exact_match_entry_add(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_KEY         key,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_PAYLOAD     payload,
    SOC_SAND_OUT uint32                     *tbl_id,
    SOC_SAND_OUT uint32                     *entry_indx,
    SOC_SAND_OUT  uint32                    *nof_steps,
    SOC_SAND_OUT  uint8                   *success
  );

uint32
  soc_sand_exact_match_entry_remove(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_KEY         key
  );

uint32
  soc_sand_exact_match_entry_lookup(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_KEY         key,
    SOC_SAND_OUT  uint32                    *tbl_id,
    SOC_SAND_OUT  uint32                    *entry_indx,
    SOC_SAND_OUT  SOC_SAND_EXACT_MATCH_PAYLOAD    payload,
    SOC_SAND_OUT  uint8                   *found
  );

uint32
  soc_sand_exact_match_clear(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match
  );

uint32
  soc_sand_exact_match_get_block(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match,
    SOC_SAND_INOUT  SOC_SAND_TABLE_BLOCK_RANGE    *range,
    SOC_SAND_OUT  SOC_SAND_EXACT_MATCH_KEY        keys,
    SOC_SAND_OUT  uint32                    *nof_entries
  );

uint32
  soc_sand_exact_match_get_size_for_save(
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_INFO        *exact_match,
    SOC_SAND_OUT  uint32                    *size
  );

uint32
  soc_sand_exact_match_save(
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_INFO        *exact_match,
    SOC_SAND_OUT uint8                      *buffer,
    SOC_SAND_IN  uint32                     buffer_size_bytes,
    SOC_SAND_OUT uint32                     *actual_size_bytes
  );

uint32
  soc_sand_exact_match_load(
    SOC_SAND_IN  uint8                              **buffer,
    SOC_SAND_OUT SOC_SAND_EXACT_MATCH_INFO                *exact_match,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_KEY_TO_HASH         key_to_hash,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_KEY_TO_VERIFY       key_to_verify,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_SET     set_entry_fun,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_GET     get_entry_fun,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_HW_ENTRY_SET        hw_set_fun,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_HW_ENTRY_GET        hw_get_fun,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_SET  is_valid_entry_set,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_GET  is_valid_entry_get
  );

void
  SOC_SAND_EXACT_MATCH_INFO_clear(
    SOC_SAND_OUT SOC_SAND_EXACT_MATCH_INFO        *info
  );

#ifdef SOC_SAND_DEBUG

void
  soc_sand_SAND_EXACT_MATCH_INFO_print(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_INFO         *info
  );

#endif /* SOC_SAND_DEBUG */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } SOC_SAND_EXACT_MATCH_H_INCLUDED*/
#endif

