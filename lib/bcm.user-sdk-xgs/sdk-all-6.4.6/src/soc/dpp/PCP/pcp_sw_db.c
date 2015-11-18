/* $Id: pcp_sw_db.c,v 1.11 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_exact_match_hash.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>

#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>
#include <soc/dpp/PCP/pcp_tbl_access.h>
#include <soc/dpp/PCP/pcp_reg_access.h>
#include <soc/dpp/PCP/pcp_sw_db.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */
#define PCP_SW_DB_FREE_LIST_MAX_SIZE (256)

#define PCP_SW_TBL_LPM_MEM_SZIE (0x80000)

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define PCP_SW_DB_ACTIVE_INS(cached, name)   \
  ((!cached)?(name):(name##_cache))

/* PAT node payload encoding - copied from ipv4_lpm_mngr:
* bits 31:30 -  pending status of entry (synced, pending add, pending remove)
* bits 29:28 - entry hw target (LPM / LEM)
* bits 27:0 - fec id
* the following macros build node payload value / extract the above fields from PAT node
* payload
*/

#define PCP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_MASK 0x3
#define PCP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_SHIFT 30

#define PCP_IPV4_LPM_MNGR_PAT_DATA_HW_TARGET_TYPE_MASK 0x3
#define PCP_IPV4_LPM_MNGR_PAT_DATA_HW_TARGET_TYPE_SHIFT (PCP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_SHIFT - 2)

#define PCP_IPV4_LPM_MNGR_PAT_DATA_FEC_MASK 0xFFFFFFF
#define PCP_IPV4_LPM_MNGR_PAT_DATA_FEC_SHIFT 0



#define PCP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(payload) \
  (((payload) >> PCP_IPV4_LPM_MNGR_PAT_DATA_FEC_SHIFT) & PCP_IPV4_LPM_MNGR_PAT_DATA_FEC_MASK)

#define PCP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(payload) \
  (((payload) >> PCP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_SHIFT) & PCP_IPV4_LPM_MNGR_PAT_DATA_PENDING_OP_TYPE_MASK)

#define PCP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(payload) \
  (((payload) >> PCP_IPV4_LPM_MNGR_PAT_DATA_HW_TARGET_TYPE_SHIFT) & PCP_IPV4_LPM_MNGR_PAT_DATA_HW_TARGET_TYPE_MASK)

#if PCP_SW_DB_DEBUG
#define PCP_SW_DB_MAGIC_NUM_VERIFY                                                  \
  SOC_SAND_MAGIC_NUM_VERIFY(Pcp_sw_db.device[unit]->rmep_info);
#endif

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

static
  PCP_SW_DB
    Pcp_sw_db;

/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC uint32
  pcp_sw_db_ipv4_arr_mem_allocator_entry_set(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            mem_id,
    SOC_SAND_IN  uint32                             entry_ndx,
    SOC_SAND_IN  PCP_ARR_MEM_ALLOCATOR_ENTRY         *entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  PCP_SW_DB_IPV4_INFO
    *ipv4_info;

  ipv4_info = Pcp_sw_db.device[unit]->ipv4_info;

  is_cached = PCP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(mem_id);
  mem_inst = PCP_ARR_MEM_ALLOCATOR_BANK_INST(mem_id);
  
  if (!is_cached)
  {
    soc_sand_os_memcpy(&(ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.array[entry_ndx]),entry,sizeof(*entry));
  }
  else
  {
    soc_sand_os_memcpy(&(ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.array_cache[entry_ndx]),entry,sizeof(*entry));
  }

  return SOC_SAND_OK;
}

STATIC uint32
  pcp_sw_db_ipv4_arr_mem_allocator_entry_get(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            mem_id,
    SOC_SAND_IN  uint32                             entry_ndx,
    SOC_SAND_OUT  PCP_ARR_MEM_ALLOCATOR_ENTRY        *entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  PCP_SW_DB_IPV4_INFO
    *ipv4_info;

  ipv4_info = Pcp_sw_db.device[unit]->ipv4_info;

  is_cached = PCP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(mem_id);
  mem_inst = PCP_ARR_MEM_ALLOCATOR_BANK_INST(mem_id);
  
  if (!is_cached)
  {
    soc_sand_os_memcpy(entry,&(ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.array[entry_ndx]), sizeof(*entry));
  }
  else
  {
    soc_sand_os_memcpy(entry,&(ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.array_cache[entry_ndx]), sizeof(*entry));
  }

  return SOC_SAND_OK;
}

STATIC uint32
  pcp_sw_db_ipv4_arr_mem_allocator_free_entry_set(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          mem_id,
    SOC_SAND_IN  PCP_ARR_MEM_ALLOCATOR_PTR         free_list
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  PCP_SW_DB_IPV4_INFO
    *ipv4_info;

  ipv4_info = Pcp_sw_db.device[unit]->ipv4_info;

  is_cached = PCP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(mem_id);
  mem_inst = PCP_ARR_MEM_ALLOCATOR_BANK_INST(mem_id);
  
  if (!is_cached)
  {
    ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.free_list = free_list;
  }
  else
  {
    ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.free_list_cache = free_list;
  }

  return SOC_SAND_OK;
}

STATIC uint32
  pcp_sw_db_ipv4_arr_mem_allocator_free_entry_get(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          mem_id,
    SOC_SAND_OUT  PCP_ARR_MEM_ALLOCATOR_PTR        *free_list
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  PCP_SW_DB_IPV4_INFO
    *ipv4_info;

  ipv4_info = Pcp_sw_db.device[unit]->ipv4_info;

  is_cached = PCP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(mem_id);
  mem_inst = PCP_ARR_MEM_ALLOCATOR_BANK_INST(mem_id);

  if (!is_cached)
  {
    *free_list = ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.free_list;
  }
  else
  {
    *free_list = ipv4_info->lpm_mngr.init_info.mem_allocators[mem_inst].arr_mem_allocator_data.free_list_cache;
  }

  return SOC_SAND_OK;

}

uint32
  pcp_ipv4_lpm_test_mem_to_mem_allocator_get(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  uint32                      mem_id,
      SOC_SAND_IN  uint32                      memory_ndx,
      SOC_SAND_OUT PCP_ARR_MEM_ALLOCATOR_INFO   **mem_allocator
  )
{
  PCP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  if (Pcp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }
  lpm_init_info = &Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info;
  
  *mem_allocator = &lpm_init_info->mem_allocators[memory_ndx];
  
  return soc_sand_ret;
}


/************************************************************************/
/* function mapping from VRF to the corresponding PAT Tree              */
/************************************************************************/
uint32
  pcp_ipv4_lpm_vrf_to_lpm_db(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      sec_handle,
    SOC_SAND_IN  uint32                      vrf_ndx,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_INFO            **pat_tree
  )
{

  PCP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  if (Pcp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }

  lpm_init_info = &Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info;

  *pat_tree = &lpm_init_info->lpms[vrf_ndx];
  
  return soc_sand_ret;
}

/*
 * Pat tree
 */



STATIC uint32
  pcp_sw_db_ipv4_arr_mem_allocator_write(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             bank_id,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  PCP_ARR_MEM_ALLOCATOR_ENTRY         *data
  )
{
  PCP_ECI_RLD1_TBL_DATA
    tbl_data;
  PCP_SW_DB_IPV4_INFO
    *ipv4_info;
  PCP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  PCP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  PCP_IPV4_LPM_MNGR_INST
    inst;
  int32
    ptr;
  uint8
    is_cached;
  uint32
    inst_id,
    cur_bank=0;
  PCP_SW_DB_LPM_INFO
    lpm_hw_info;
  uint32
    entry_in_bank=0;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Pcp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);
  lpm_init_info = &(lpm_mngr->init_info);

  is_cached = PCP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  inst_id = PCP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);

  if (!is_cached)
  {
    pcp_ipv4_lpm_entry_decode(
      lpm_mngr,
      inst_id,
      data,
      2,
      tbl_data.rld1_data
    );

    if (inst_id >= 3)/* could be in external memory, check */
    {
      res = pcp_sw_db_ipv4_lpm_hw_info_get(
              unit,
              &lpm_hw_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      cur_bank = 1 + (uint32)((inst_id-1)/lpm_hw_info.mems_per_bank);
      entry_in_bank = ((inst_id-1)%lpm_hw_info.mems_per_bank) * PCP_SW_TBL_LPM_MEM_SZIE + offset;

    }

    if (cur_bank < 3)
    {
      res = pcp_eci_rld_lpm_tbl_set_unsafe(
              unit,
              inst_id,
              offset,
              &tbl_data,
              0 /* not full, FILL rest of entry by hw */
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else
    {
      if (Pcp_sw_db.device[unit]->op_mode.ipv4_info.write_cb == NULL)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_NULL_POINTER_ERR,55,exit);
      }
      
      /* call to user function to write to 4th bank */
      res =  Pcp_sw_db.device[unit]->op_mode.ipv4_info.write_cb(
              unit,
              0, /* ignored */
              entry_in_bank, /* entry offset in the bank, assuming, each entry is 36 bits */
              tbl_data.rld1_data, /* data[3], only 36 bits are used */
              0 /* not full, FILL rest from entry by hw */
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 107, exit);
    }

  }

 /*
  * write to software shadow.
  */
  if (is_cached)
  {
    lpm_init_info->mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow_cache[2*offset] = data[0];
    lpm_init_info->mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow_cache[2*offset+1] = data[1]; 
  }
  else
  {
    lpm_init_info->mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow[2*offset] = data[0];
    lpm_init_info->mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow[2*offset+1] = data[1];
  }
 /*
  * if there is defragment, store backward pointer
  */
  if (0){ 
  /* if (lpm_init_info->flags & PCP_LPV4_LPM_SUPPORT_DEFRAG){*/
    /* for all banks except first bank, store backward pointer */
    if (inst_id < lpm_init_info->nof_banks - 1)
    {
      inst.arr[0] = data[0];
      inst.arr[1] = data[1];
      ptr = pcp_ipv4_lpm_mngr_row_to_base_addr(lpm_mngr,&inst,inst_id,0); 
      if (ptr != -1)
      {
        /* ptr at bank-id + 1 is pointed by offset at bank_id*/
        res = soc_sand_group_mem_ll_member_add(
                lpm_mngr->init_info.rev_ptrs[inst_id+1],
                ptr,
                offset
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
      }
    }
  }
  


  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_ipv4_arr_mem_allocator_write()",0,0);
}


STATIC uint32
pcp_sw_db_ipv4_arr_mem_allocator_read(
  SOC_SAND_IN  int                             unit,
  SOC_SAND_IN  uint32                             bank_id,
  SOC_SAND_IN  uint32                             offset,
  SOC_SAND_OUT  PCP_ARR_MEM_ALLOCATOR_ENTRY         *data
  )
{
  PCP_SW_DB_IPV4_INFO
    *ipv4_info;
  PCP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  /*PCP_IHB_LPM_TBL_DATA
    tbl_data,tbl_data_read;*/
  uint32
    inst_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  /*
  * read from software shadow.
  */
  ipv4_info = Pcp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  inst_id = PCP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);

  if (PCP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id))
  {
    data[0] = lpm_mngr->init_info.mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow_cache[2*offset];
    data[1] = lpm_mngr->init_info.mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow_cache[2*offset+1];
    
  }
  else
  {
    data[0] = lpm_mngr->init_info.mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow[2*offset];
    data[1] = lpm_mngr->init_info.mem_allocators[inst_id].arr_mem_allocator_data.mem_shadow[2*offset+1];
  }
  
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_ipv4_arr_mem_allocator_read()",0,0);

}

STATIC uint32
  pcp_sw_db_align_entry_ptr(
    SOC_SAND_IN   uint32            entry_place, /*offset */
    SOC_SAND_IN   uint32            key, /* device + bank-id */
    SOC_SAND_IN   uint32            new_ptr
 )
{
  uint32
    bank_id,
    unit;
  PCP_ARR_MEM_ALLOCATOR_ENTRY
    data[3];
  PCP_SW_DB_IPV4_INFO
    *ipv4_info;
  PCP_IPV4_LPM_MNGR_INFO
    *lpm_mngr;
  PCP_IPV4_LPM_MNGR_INST
    inst;
  uint32
    mem_inst;
  uint32
	  ret_val;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* get bank and device-id */
  bank_id = key & (SOC_SAND_BIT(31)|0x7);
  mem_inst = PCP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  unit = (key & SOC_SAND_RBIT(31)) >> 3;
  if (mem_inst == 0 )
  {
    return SOC_SAND_ERR;/* not expected to be bank zero*/
  }
  bank_id -= 1;
  mem_inst -= 1;

  ipv4_info = Pcp_sw_db.device[unit]->ipv4_info;
  lpm_mngr = &(ipv4_info->lpm_mngr);

  /* read old data */
  res = pcp_sw_db_ipv4_arr_mem_allocator_read(unit,bank_id,entry_place,data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  inst.arr[0] = data[0];
  inst.arr[1] = data[1];
  /* update pointer in data*/
  ret_val = pcp_ipv4_lpm_mngr_update_base_addr(lpm_mngr,&inst,mem_inst,new_ptr);

  /* write updated data */
  if (ret_val != -1)
  {
    data[0] = inst.arr[0];
    data[1] = inst.arr[1];
    res = pcp_sw_db_ipv4_arr_mem_allocator_write(unit,bank_id,entry_place,data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_align_entry_ptr()",0,0);
}

/*
 * when moving entry in mem-allocator do this alignment
 */
uint32
  pcp_sw_db_ipv4_lpm_mem_align(
    SOC_SAND_IN   int                      unit,
    SOC_SAND_IN   uint32                      bank_id,
    SOC_SAND_IN  PCP_ARR_MEM_ALLOCATOR_ENTRY   *data,
    SOC_SAND_IN   uint32                      old_place,
    SOC_SAND_IN   uint32                      new_place
 )
{
  uint32
    key;
  uint32
    mem_inst;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  mem_inst = PCP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  /* if this is bank zero, then no one is poiting to it */
  if (mem_inst == 0)
  {
    goto exit;
  }

  key = bank_id;
  key |= (unit<<3);

  res = soc_sand_group_mem_ll_func_run(
            Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_inst],
            old_place,   /* all members point to this place */
            pcp_sw_db_align_entry_ptr,/* updated pointers to point to new place*/
            key,        /* this bank and device-id */
            new_place
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_ipv4_lpm_mem_align()",0,0);
}


uint32
  pcp_sw_db_pat_tree_root_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    tree_ndx,
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE_PLACE    root_place
  )
{
  uint32
    tree_id;
  uint8
    cached;

  if (Pcp_sw_db.device[unit] == NULL)
  {
    return SOC_SAND_ERR;
  }

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  if (!cached)
  {
    Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.root = root_place;
  }
  else
  {
    Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.root_cache = root_place;
  }

  return SOC_SAND_OK;
}

uint32
  pcp_sw_db_pat_tree_root_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    tree_ndx,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_NODE_PLACE    *root_place
  )
{
  uint32
    tree_id;
  uint8
    cached;

  if (Pcp_sw_db.device[unit] == NULL)
  {
    return SOC_SAND_ERR;
  }

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  if (!cached)
  {
    *root_place = Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.root;
  }
  else
  {
    *root_place = Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.root_cache;
  }

  return SOC_SAND_OK;
}

uint32
  pcp_sw_db_pat_tree_node_set(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                          tree_ndx,
    SOC_SAND_IN uint32                          node_place,
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE                *node_info
  )
{
  uint32
    tree_id;
  uint8
    cached;

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  if (!cached)
  {
    soc_sand_os_memcpy(
      &Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory[node_place],
      node_info,
      sizeof(*node_info)
      );
  }
  else
  {
    soc_sand_os_memcpy(
      &Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory_cache[node_place],
      node_info,
      sizeof(*node_info)
      );
  }

  return SOC_SAND_OK;

}

uint32
  pcp_sw_db_pat_tree_node_get(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                           tree_ndx,
    SOC_SAND_IN uint32                            node_place,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_NODE                  *node_info
  )
{
  uint32
    tree_id;
  uint8
    cached;

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  if (!cached)
  {
    soc_sand_os_memcpy(
      node_info,
      &Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory[node_place],
      sizeof(*node_info)
    );
  }
  else
  {
    soc_sand_os_memcpy(
      node_info,
      &Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory_cache[node_place],
      sizeof(*node_info)
    );
  }

  return SOC_SAND_OK;
}

uint32
  pcp_sw_db_pat_tree_node_ref_get(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                           tree_ndx,
    SOC_SAND_IN uint32                            node_place,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_NODE                  **node_info
  )
{
  uint32
    tree_id;
  uint8
    cached;

  tree_id = SOC_SAND_PAT_TREE_TREE_INST(tree_ndx);
  cached = SOC_SAND_PAT_TREE_IS_CACHED_INST(tree_ndx);

  if (!cached)
  {
    *node_info = &Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory[node_place];
  }
  else
  {
    *node_info = &Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.lpms[tree_id].pat_tree_data.tree_memory_cache[node_place];
  }

  return SOC_SAND_OK;
}

uint32
  pcp_sw_db_ipv4_default_fec_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    default_fec
  )
{

  PCP_SW_DB_IPV4_FIELD_SET(
    unit,
    ipv4_info->default_fec,
    &default_fec
  );
}

uint32
  pcp_sw_db_ipv4_default_fec_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *default_fec
  )
{
  PCP_SW_DB_IPV4_FIELD_GET(
    unit,
    ipv4_info->default_fec,
    default_fec
  );
}

uint32
  pcp_sw_db_ipv4_nof_lpm_entries_in_lpm_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                    nof_lpm_entries_in_lpm
  )
{
  PCP_SW_DB_IPV4_FIELD_SET(
    unit,
    ipv4_info->nof_lpm_entries_in_lpm,
    &nof_lpm_entries_in_lpm
  );
}

uint32
  pcp_sw_db_ipv4_nof_lpm_entries_in_lpm_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *nof_lpm_entries_in_lpm
  )
{
  PCP_SW_DB_IPV4_FIELD_GET(
    unit,
    ipv4_info->nof_lpm_entries_in_lpm,
    nof_lpm_entries_in_lpm
  );
}


  
uint32
  pcp_sw_db_free_list_add(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            bank_id,
    SOC_SAND_IN  uint32             address
  )
{
  PCP_SW_DB_IPV4_INFO
    *ipv4_info;
  uint32
    cur_size;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Pcp_sw_db.device[unit]->ipv4_info;
  cur_size = (ipv4_info->free_list_size);
  if (cur_size >= PCP_SW_DB_FREE_LIST_MAX_SIZE)
  {
    return SOC_SAND_ERR;
  }
  ipv4_info->free_list[cur_size].bank_id = (uint8)bank_id;
  ipv4_info->free_list[cur_size].address = (uint16)address;
  ++(ipv4_info->free_list_size);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_free_list_add()",0,0);
}

uint32
  pcp_sw_db_free_list_commit(
    SOC_SAND_IN  int            unit
  )
{
  uint32
    indx,
    bank_indx,
    address;
  PCP_SW_DB_IPV4_INFO
    *ipv4_info;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Pcp_sw_db.device[unit]->ipv4_info;

    
  for (indx = 0; indx < ipv4_info->free_list_size; ++indx)
  {
    bank_indx = ipv4_info->free_list[indx].bank_id;
    address = ipv4_info->free_list[indx].address;

    res = pcp_arr_mem_allocator_free(
            &(ipv4_info->lpm_mngr.init_info.mem_allocators[bank_indx]),
            address
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  ipv4_info->free_list_size = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_free_list_commit()",0,0);
}

uint32
  pcp_sw_db_ipv4_nof_vrfs_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                   nof_vrfs
  )
{
  PCP_SW_DB_FIELD_SET(
    unit,
    op_mode.ipv4_info.nof_vrfs,
    &nof_vrfs
  );
}


uint32
  pcp_sw_db_ipv4_nof_vrfs_get(
    SOC_SAND_IN int                    unit,
    SOC_SAND_OUT uint32                    *nof_vrfs
  )
{
  PCP_SW_DB_FIELD_GET(
    unit,
    op_mode.ipv4_info.nof_vrfs,
    nof_vrfs
  );
}

uint8
  pcp_sw_db_ipv4_is_vrf_exist(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN uint32                   vrf_ndx
  )
{
  uint32 nof_vrfs;

  pcp_sw_db_ipv4_nof_vrfs_get(
    unit,
    &nof_vrfs
  );
  return (vrf_ndx < nof_vrfs) ? TRUE : FALSE;
}


uint32
  pcp_rmep_access_hw_set_callback_unsafe(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32    tbl_ndx,
    SOC_SAND_IN uint32    entry_offset,
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_PAYLOAD        payload,
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_VERIFIER       verifier
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_RMEP_HASH_0_DB_TBL_DATA
    hash_0_tbl_data;
  PCP_OAM_RMEP_HASH_1_DB_TBL_DATA
    hash_1_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(payload);
  SOC_SAND_CHECK_NULL_INPUT(verifier);

  sal_memset(&hash_0_tbl_data, 0x0, sizeof(PCP_OAM_RMEP_HASH_0_DB_TBL_DATA));
  sal_memset(&hash_1_tbl_data, 0x0, sizeof(PCP_OAM_RMEP_HASH_1_DB_TBL_DATA));

  if (tbl_ndx == 0)
  {
    res = pcp_oam_rmep_hash_0_db_tbl_get_unsafe(
            unit,
            entry_offset,
            &hash_0_tbl_data
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_sand_U8_to_U32(
            payload,
            4,
            &hash_0_tbl_data.rmep_db_ptr0
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_sand_U8_to_U32(
            verifier,
            2,
            &hash_0_tbl_data.verifier0
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = pcp_oam_rmep_hash_0_db_tbl_set_unsafe(
            unit,
            entry_offset,
            &hash_0_tbl_data
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  else /* tbl_ndx == 1*/
  {
    
    res = pcp_oam_rmep_hash_1_db_tbl_get_unsafe(
          unit,
          entry_offset,
          &hash_1_tbl_data
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    res = soc_sand_U8_to_U32(
            payload,
            4,
            &hash_1_tbl_data.rmep_db_ptr1
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = soc_sand_U8_to_U32(
            verifier,
            4,
            &hash_1_tbl_data.verifier1
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = pcp_oam_rmep_hash_1_db_tbl_set_unsafe(
            unit,
            entry_offset,
            &hash_1_tbl_data
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_ac_access_hw_set_callback_unsafe()",unit,0);
}

uint32
  pcp_rmep_access_hw_get_callback_unsafe(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32    tbl_ndx,
    SOC_SAND_IN uint32    entry_offset,
    SOC_SAND_OUT SOC_SAND_EXACT_MATCH_PAYLOAD        payload,
    SOC_SAND_OUT SOC_SAND_EXACT_MATCH_VERIFIER       verifier
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_OAM_RMEP_HASH_0_DB_TBL_DATA
    hash_0_tbl_data;
  PCP_OAM_RMEP_HASH_1_DB_TBL_DATA
    hash_1_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(payload);
  SOC_SAND_CHECK_NULL_INPUT(verifier);

  sal_memset(&hash_0_tbl_data, 0x0, sizeof(PCP_OAM_RMEP_HASH_0_DB_TBL_DATA));
  sal_memset(&hash_1_tbl_data, 0x0, sizeof(PCP_OAM_RMEP_HASH_1_DB_TBL_DATA));

  if (tbl_ndx == 0)
  {
    res = pcp_oam_rmep_hash_0_db_tbl_get_unsafe(
            unit,
            entry_offset,
            &hash_0_tbl_data
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_sand_U32_to_U8(
            &hash_0_tbl_data.rmep_db_ptr0,
            2,
            payload
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_sand_U32_to_U8(
            &hash_0_tbl_data.verifier0,
            2,
            verifier
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  }
  else /* tbl_ndx == 1*/
  {
    
    res = pcp_oam_rmep_hash_1_db_tbl_get_unsafe(
          unit,
          entry_offset,
          &hash_1_tbl_data
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_sand_U32_to_U8(
            &hash_1_tbl_data.rmep_db_ptr1,
            2,
            payload
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    res = soc_sand_U32_to_U8(
            &hash_1_tbl_data.verifier1,
            2,
            verifier
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_ac_access_hw_get_callback_unsafe()",unit,0);
}


STATIC uint32
  pcp_sw_db_buffer_get_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_IN  uint8                              *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_OUT uint8                              *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#if PCP_SW_DB_DEBUG
  PCP_SW_DB_MAGIC_NUM_VERIFY;
#endif

  res = SOC_SAND_OK; sal_memcpy(
    data,
    buffer + (offset * len),
    len
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_buffer_get_entry()",0,0);
}

uint32
  pcp_sw_db_buffer_set_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_INOUT  uint8                           *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_IN  uint8                              *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#if PCP_SW_DB_DEBUG
  PCP_SW_DB_MAGIC_NUM_VERIFY;
#endif

  res = SOC_SAND_OK; sal_memcpy(
    buffer + (offset * len),
    data,
    len
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_buffer_set_entry()",0,0);
}

/*
 * exact_match SW DB access
 */
STATIC uint32
  pcp_sw_db_exact_match_is_valid_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sec_hanlde,
    SOC_SAND_IN  uint32                 tbl_ndx,
    SOC_SAND_IN  uint32                 entry_offset,
    SOC_SAND_IN uint8                  is_valid
  )
{
  uint32
    *bit_stream;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#if PCP_SW_DB_DEBUG
  PCP_SW_DB_MAGIC_NUM_VERIFY;
#endif

  bit_stream = Pcp_sw_db.device[unit]->rmep_info->rmep_map_info.mgmt_info.use_bitmap[tbl_ndx];

  res = soc_sand_bitstream_set(
          bit_stream,
          entry_offset,
          is_valid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_exact_match_is_valid_set()",0,0);
}

STATIC uint32
  pcp_sw_db_exact_match_is_valid_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   sec_hanlde,
    SOC_SAND_IN  uint32                   tbl_ndx,
    SOC_SAND_IN  uint32                   entry_offset,
    SOC_SAND_OUT uint8                  *is_valid
  )
{
  uint32
    res = SOC_SAND_OK,
    *bit_stream,
    val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#if PCP_SW_DB_DEBUG
  PCP_SW_DB_MAGIC_NUM_VERIFY;
#endif

  bit_stream = Pcp_sw_db.device[unit]->rmep_info->rmep_map_info.mgmt_info.use_bitmap[tbl_ndx];

  val = soc_sand_bitstream_test_bit(
          bit_stream,
          entry_offset
        );
  *is_valid = SOC_SAND_NUM2BOOL(val);

  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_exact_match_is_valid_get()",0,0);
}

/*
 * }
 * { Rmep info
 */
uint32
  pcp_sw_db_rmep_exact_match_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  SOC_SAND_EXACT_MATCH_INFO       **exact_match
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#if PCP_SW_DB_DEBUG
  PCP_SW_DB_MAGIC_NUM_VERIFY;
#endif

  if (Pcp_sw_db.device[unit] == NULL)
  {
    return SOC_SAND_ERR;
  }

  *exact_match = &(Pcp_sw_db.device[unit]->rmep_info->rmep_map_info);

  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in t20e_sw_db_rmep_exact_match_get()",0,0);
}

STATIC uint32
  pcp_sw_db_device_rmep_info_initialize(
    SOC_SAND_IN  uint32                            unit
  )
{
  SOC_SAND_EXACT_MATCH_INFO
    *rmep_exact_match;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  PCP_ALLOC_ANY_SIZE(Pcp_sw_db.device[unit]->rmep_info, PCP_SW_DB_DEVICE_RMEP_INFO, 1, "Pcp_sw_db.device[unit]->rmep_info");

  rmep_exact_match = &(Pcp_sw_db.device[unit]->rmep_info->rmep_map_info);
  SOC_SAND_EXACT_MATCH_INFO_clear(rmep_exact_match);
  rmep_exact_match->init_info.prime_handle = unit;
  rmep_exact_match->init_info.sec_handle = 0;
  rmep_exact_match->init_info.get_entry_fun = pcp_sw_db_buffer_get_entry;
  rmep_exact_match->init_info.set_entry_fun = pcp_sw_db_buffer_set_entry;
  rmep_exact_match->init_info.is_valid_entry_get = pcp_sw_db_exact_match_is_valid_get;
  rmep_exact_match->init_info.is_valid_entry_set = pcp_sw_db_exact_match_is_valid_set;
  rmep_exact_match->init_info.hw_get_fun = pcp_rmep_access_hw_get_callback_unsafe;
  rmep_exact_match->init_info.hw_set_fun = pcp_rmep_access_hw_set_callback_unsafe;
  rmep_exact_match->init_info.hash_bits = 13;
  rmep_exact_match->init_info.max_insert_steps = 10;
  rmep_exact_match->init_info.key_bits = 25;
  rmep_exact_match->init_info.verifier_bits = 12;
  rmep_exact_match->init_info.key_to_hash = soc_sand_exact_match_hash_key_to_hash_25_13;
  rmep_exact_match->init_info.key_to_verifier = soc_sand_exact_match_hash_key_to_verifier_25_12;
  res = soc_sand_exact_match_create(
          rmep_exact_match
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
#if PCP_SW_DB_DEBUG
  Pcp_sw_db.device[unit]->rmep_info->soc_sand_magic_num = SOC_SAND_MAGIC_NUM_VAL;
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_device_rmep_info_initialize()",unit,0);
}

STATIC uint32
  pcp_sw_db_device_rmep_info_finalize(
    SOC_SAND_IN  int                            unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_EXACT_MATCH_INFO
    *rmep_exact_match;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#if PCP_SW_DB_DEBUG
  SOC_SAND_MAGIC_NUM_VERIFY(Pcp_sw_db.device[unit]->rmep_info);
#endif

  rmep_exact_match = &(Pcp_sw_db.device[unit]->rmep_info->rmep_map_info);
  res = soc_sand_exact_match_destroy(rmep_exact_match);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  PCP_FREE_ANY_SIZE(Pcp_sw_db.device[unit]->rmep_info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_device_rmep_info_finalize()",0,0);
}

void
  soc_petra_sw_db_op_mode_elk_enable_set(
    SOC_SAND_IN int unit,
	SOC_SAND_IN uint8  enable
  )
{
  Pcp_sw_db.device[unit]->op_mode.elk.enable = enable;
}

uint8
  soc_petra_sw_db_op_mode_elk_enable_get(
    SOC_SAND_IN int unit
  )
{
  return Pcp_sw_db.device[unit]->op_mode.elk.enable;
}

void
  soc_petra_sw_db_op_mode_elk_ilm_key_mask_bitmap_set(
    SOC_SAND_IN int unit,
	SOC_SAND_IN uint32  ilm_key_mask_bitmap
  )
{
  Pcp_sw_db.device[unit]->op_mode.elk.ilm_key_mask_bitmap = ilm_key_mask_bitmap;
}

uint32
  soc_petra_sw_db_op_mode_elk_ilm_key_mask_bitmap_get(
    SOC_SAND_IN int unit
  )
{
  return Pcp_sw_db.device[unit]->op_mode.elk.ilm_key_mask_bitmap;
}

void
  soc_petra_sw_db_op_mode_oam_enable_set(
    SOC_SAND_IN int unit,
	SOC_SAND_IN uint8  enable
  )
{
  Pcp_sw_db.device[unit]->op_mode.oam.enable = enable;
}

uint8
  soc_petra_sw_db_op_mode_oam_enable_get(
    SOC_SAND_IN int unit
  )
{
  return Pcp_sw_db.device[unit]->op_mode.oam.enable;
}

void
  soc_petra_sw_db_op_mode_sts_conf_word_set(
    SOC_SAND_IN int unit,
	SOC_SAND_IN uint32  conf_word
  )
{
  Pcp_sw_db.device[unit]->op_mode.sts.conf_word = conf_word;
}

uint32
  soc_petra_sw_db_op_mode_sts_conf_word_get(
    SOC_SAND_IN int unit
  )
{
  return Pcp_sw_db.device[unit]->op_mode.sts.conf_word;
}

uint32
  pcp_sw_db_device_op_mode_initialize(
    SOC_SAND_IN int unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  Pcp_sw_db.device[unit]->op_mode.sts.conf_word = 0xa0d9;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_device_op_mode_initialize()",0,0);
}

void
  pcp_sw_db_learning_mode_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN PCP_FRWRD_MACT_LEARNING_MODE  learning_mode
  )
{
  Pcp_sw_db.device[unit]->elk.fwd_mact.learning_mode = learning_mode;
}

PCP_FRWRD_MACT_LEARNING_MODE
  pcp_sw_db_learning_mode_get(
    SOC_SAND_IN int unit
  )
{
  return Pcp_sw_db.device[unit]->elk.fwd_mact.learning_mode;
}

void
  pcp_sw_db_is_petra_a_compatible_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8  is_petra_a_compatible
  )
{
  Pcp_sw_db.device[unit]->elk.fwd_mact.is_petra_a_compatible = is_petra_a_compatible;
}

uint8
  pcp_sw_db_is_petra_a_compatible_get(
    SOC_SAND_IN int unit
  )
{
  return Pcp_sw_db.device[unit]->elk.fwd_mact.is_petra_a_compatible;
}

uint32
  pcp_sw_db_device_elk_op_mode_initialize(
    SOC_SAND_IN int unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  Pcp_sw_db.device[unit]->elk.fwd_mact.learning_mode = 0x0;
  Pcp_sw_db.device[unit]->elk.fwd_mact.is_petra_a_compatible = 0x0;  

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_device_elk_op_mode_initialize()",0,0);
}

typedef struct
{
  uint32  nof_vrf_bits; /* number of bits to specify VRF */
  uint32  nof_banks; /* number of levels for the lookup */
  uint32  nof_mems;/* number of memories for writing the data */
  uint32  nof_rows_per_mem[25]; /* number of rows that each memory has */
  uint32 nof_bits_per_bank[4]; /* number of bits that each bank will use from the VRF/IP address for the next access*/
  uint32 bank_to_mem[4]; /* mapping from bank number, which is sequential in depth, to memory, which may or may not be sequential */
  uint32 mem_to_bank[25];
} __ATTRIBUTE_PACKED__ PCP_SW_DB_LPM_INIT_INFO;

#ifdef SOC_SAND_DEBUG

void
  PCP_SW_DB_LPM_INIT_INFO_print(
    PCP_SW_DB_LPM_INIT_INFO* info
  );
#endif

uint32
  pcp_sw_db_ipv4_calc_init_param(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  PCP_MGMT_OP_MODE    *oper_mode,
    SOC_SAND_OUT PCP_SW_DB_LPM_INIT_INFO *init_info
    )
{
  uint32
    bank_size,
    first_bank_bits,
    mems_in_bank,
    nof_inter_mems=0,
    indx,
    nof_bits,
    bits_per_depth;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* if ELK lpm not enable (and nof_vrf != 0) return error */
  if (!oper_mode->elk.enable || oper_mode->elk.mode == PCP_MGMT_ELK_LKP_MODE_EM)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR,10,exit);
  }

  if (oper_mode->elk.size_mbit == PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288)
  {
    bank_size = 0x100000;
    first_bank_bits = 21;
  }
  else
  {
    bank_size = 0x200000;
    first_bank_bits = 22;
  }
  /* if also for ELK usage then half of it for LPM */
  if (oper_mode->elk.mode == PCP_MGMT_ELK_LKP_MODE_BOTH)
  {
    bank_size /= 2;
    first_bank_bits -= 1;
  }

  /* vrf bits */
  init_info->nof_vrf_bits = soc_sand_log2_round_up(oper_mode->ipv4_info.nof_vrfs); /* number of bits to specify VRF */
  init_info->nof_banks = 4; /* as LP4 */
  if (oper_mode->ipv4_info.lpm_4th_bank_size == 0)
  {
    init_info->nof_banks = 3;
  }

  mems_in_bank = (uint32)(bank_size / 0x80000);
  nof_inter_mems = 1 + mems_in_bank * 2;
  init_info->nof_mems = 1 + mems_in_bank * 2; /* first-bank, 2nd+3rd, forth (user memory) */
  if (oper_mode->ipv4_info.lpm_4th_bank_size > 0)
  {
    init_info->nof_mems += mems_in_bank;
  }
  /* first memory includes 2 psychical banks */
  init_info->nof_rows_per_mem[0] = 2 * bank_size;

  init_info->mem_to_bank[0] = 0;
  /* mem to banks */
  for (indx = 1; indx < init_info->nof_mems; ++indx)
  {
    if (indx < nof_inter_mems)
    {
      init_info->nof_rows_per_mem[indx] = 0x80000;
    }
    else
    {
      init_info->nof_rows_per_mem[indx] = oper_mode->ipv4_info.lpm_4th_bank_size/mems_in_bank;
    }
    init_info->mem_to_bank[indx] = 1 + (indx-1)/mems_in_bank;
  }

  init_info->bank_to_mem[0] = 0;
  init_info->mem_to_bank[0] = 0;
  /* banks to mem */
  for (indx = 1; indx < init_info->nof_banks; ++indx)
  {
    init_info->bank_to_mem[indx] = 1 + mems_in_bank * (indx-1);
  }

  /* bits per bank */
  init_info->nof_bits_per_bank[0] = first_bank_bits;

  /* rest of bits, for banks > 0 */
  nof_bits = (32 + init_info->nof_vrf_bits) - (first_bank_bits);

  for (indx = 1 ; indx < (init_info->nof_banks-1); ++indx)
  {
    bits_per_depth = SOC_SAND_DIV_ROUND_UP(nof_bits, ((init_info->nof_banks-1) - indx + 1));
    init_info->nof_bits_per_bank[indx] = bits_per_depth;
    nof_bits -= bits_per_depth;
    if (bits_per_depth > 7)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR,30,exit);
    }
  }
  init_info->nof_bits_per_bank[indx] = nof_bits;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_ipv4_calc_init_param()",unit, 0);

}





uint32
  pcp_sw_db_device_initialize(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  PCP_MGMT_OP_MODE    *oper_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  PCP_ALLOC(Pcp_sw_db.device[unit], PCP_SW_DB_DEVICE, 1, "Pcp_sw_db.device[unit]");


  PCP_COPY(&Pcp_sw_db.device[unit]->op_mode,oper_mode,PCP_MGMT_OP_MODE,1);


  res = pcp_sw_db_device_rmep_info_initialize(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = pcp_sw_db_device_op_mode_initialize(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = pcp_sw_db_device_elk_op_mode_initialize(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit); 

  {
    PCP_IPV4_LPM_PXX_MODEL
      pxx_model = PCP_IPV4_LPM_PXX_MODEL_LP4; /* Which model of PXX to use (p6x, p4x, p6n, etc) */
    PCP_SW_DB_LPM_INFO
      lpm_info;
    PCP_SW_DB_LPM_INIT_INFO 
      init_info;
    uint32
      nof_vrfs;
    /* init number of VRFs for LPM lookup*/
    nof_vrfs = oper_mode->ipv4_info.nof_vrfs;


    /* allocate this sw DB only if there is routing table to support */
    if (nof_vrfs > 0)
    {
      res = pcp_sw_db_ipv4_calc_init_param(
              unit,
              oper_mode,
              &init_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#ifdef SOC_SAND_DEBUG
      PCP_SW_DB_LPM_INIT_INFO_print(&init_info);
#endif
      PCP_ALLOC_ANY_SIZE(
        Pcp_sw_db.device[unit]->ipv4_info,
        PCP_SW_DB_IPV4_INFO,
        1, "Pcp_sw_db.device[unit]->ipv4_info"
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

              
      soc_sand_os_memset(
        Pcp_sw_db.device[unit]->ipv4_info,
        0x0,
        sizeof(*Pcp_sw_db.device[unit]->ipv4_info)
      );

    lpm_info.elk_enable = oper_mode->elk.enable;
    lpm_info.lkup_mode = oper_mode->elk.mode;
    lpm_info.dram_size = oper_mode->elk.size_mbit;
    if (lpm_info.dram_size == PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288)
    {
      lpm_info.bank_size =0x100000;
    }
    else
    {
      lpm_info.bank_size = 0x200000;
    }
    /* if also for ELK usage then half of it for LPM */
    if (oper_mode->elk.mode == PCP_MGMT_ELK_LKP_MODE_BOTH)
    {
      lpm_info.bank_size /= 2;
    }
    lpm_info.mems_per_bank = (uint32)(lpm_info.bank_size / 0x80000);

    res = pcp_sw_db_ipv4_lpm_hw_info_set(
            unit,
            &lpm_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

/* calc parameters, */


      res = pcp_sw_db_ipv4_initialize(
              unit,
              oper_mode->ipv4_info.nof_vrfs,
              oper_mode->ipv4_info.max_routes_in_vrf,
              init_info.nof_vrf_bits,
              init_info.nof_banks,
              init_info.nof_bits_per_bank,
              init_info.bank_to_mem,
              init_info.mem_to_bank,
              init_info.nof_mems,
              init_info.nof_rows_per_mem,
              pxx_model,
              oper_mode->ipv4_info.flags,
              0 /* default_sys_fec */
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
    {
      Pcp_sw_db.device[unit]->ipv4_info = NULL;
    }
  }



#if PCP_SW_DB_DEBUG
  Pcp_sw_db.device[unit]->soc_sand_magic_num = SOC_SAND_MAGIC_NUM_VAL;
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in PCP_sw_db_pcp_device_initialize()",unit, 0);
}

uint32
  pcp_sw_db_device_finalize(
    SOC_SAND_IN  int                            unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#if PCP_SW_DB_DEBUG
  SOC_SAND_MAGIC_NUM_VERIFY(Pcp_sw_db.device[unit]);
#endif

  res = pcp_sw_db_ipv4_terminate(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  res = pcp_sw_db_device_rmep_info_finalize(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  PCP_FREE(Pcp_sw_db.device[unit]);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in PCP_sw_db_device_finalize()",0,0);
}

uint32
  pcp_sw_db_initialize(
  )
{
  int unit;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Pcp_sw_db.init)
  {
    goto exit;
  }

  for (unit = 0; unit < SOC_SAND_MAX_DEVICE; ++unit)
  {
    Pcp_sw_db.device[unit] = NULL;
  }

  Pcp_sw_db.init = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_initialize()",0,0);
}

uint32
  pcp_sw_db_finalize(void)
{
  uint32
    res = SOC_SAND_OK;
  uint32
    device_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (device_i = 0; device_i < SOC_SAND_MAX_DEVICE; ++device_i)
  {
    if (Pcp_sw_db.device[device_i] != NULL)
    {
      res = pcp_sw_db_device_finalize(
              device_i
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
    
    Pcp_sw_db.device[device_i] = NULL;
  }

  Pcp_sw_db.init = FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_finalize()",0,0);
}

/* for member group */
STATIC uint32
  pcp_sw_db_lpm_ll_member_entry_set(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            bank_id,
    SOC_SAND_IN  uint32                             member_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY      *member_entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  is_cached = PCP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  mem_inst = PCP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  
  PCP_COPY(
             &(PCP_SW_DB_ACTIVE_INS(is_cached,Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_inst]->group_members_data.members)[member_ndx]),
             member_entry,
             SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY,
             1
            );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_lpm_ll_member_entry_set()",0,0);
}

STATIC uint32
  pcp_sw_db_lpm_ll_member_entry_get(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                            bank_id,
    SOC_SAND_IN  uint32                             member_ndx,
    SOC_SAND_OUT  SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY      *member_entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  is_cached = PCP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  mem_inst = PCP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  
  PCP_COPY(
             member_entry,
             &(PCP_SW_DB_ACTIVE_INS(is_cached,Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_inst]->group_members_data.members)[member_ndx]),
             SOC_SAND_GROUP_MEM_LL_MEMBER_ENTRY,
             1
            );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_lpm_ll_member_entry_get()",0,0);
}


STATIC uint32
  pcp_sw_db_lpm_ll_group_entry_set(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          bank_id,
    SOC_SAND_IN  uint32                           group_ndx,
    SOC_SAND_IN  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY      *group_entry
  )
{
  uint32
    mem_inst;
  uint8
    is_cached;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  is_cached = PCP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  mem_inst = PCP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  
  PCP_COPY(
             &(PCP_SW_DB_ACTIVE_INS(is_cached,Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_inst]->group_members_data.groups)[group_ndx]),
             group_entry,
             SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY,
             1
            );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_lpm_ll_group_entry_set()",0,0);
}

STATIC uint32
  pcp_sw_db_lpm_ll_group_entry_get(
    SOC_SAND_IN   int                          unit,
    SOC_SAND_IN   uint32                          bank_id,
    SOC_SAND_IN  uint32                             group_ndx,
    SOC_SAND_OUT  SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY      *group_entry
  )
{

  uint32
    mem_inst;
  uint8
    is_cached;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  is_cached = PCP_ARR_MEM_ALLOCATOR_IS_CACHED_INST(bank_id);
  mem_inst = PCP_ARR_MEM_ALLOCATOR_BANK_INST(bank_id);
  
  PCP_COPY(
             group_entry,
             &(PCP_SW_DB_ACTIVE_INS(is_cached,Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr.init_info.rev_ptrs[mem_inst]->group_members_data.groups)[group_ndx]),
             SOC_SAND_GROUP_MEM_LL_GROUP_ENTRY,
             1
            );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_lpm_ll_group_entry_get()",0,0);
}


uint32
  pcp_sw_db_ipv4_initialize(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  nof_vrfs,
    SOC_SAND_IN uint32  max_nof_routes[PCP_NOF_VRFS],
    SOC_SAND_IN uint32  nof_vrf_bits,
    SOC_SAND_IN uint32  nof_banks,
    SOC_SAND_IN uint32  *nof_bits_per_bank,
    SOC_SAND_IN uint32  *bank_to_mem,
    SOC_SAND_IN uint32  *mem_to_bank,
    SOC_SAND_IN uint32  nof_mems,
    SOC_SAND_IN uint32  *nof_rows_per_mem, /* must be uint32 */
    SOC_SAND_IN PCP_IPV4_LPM_PXX_MODEL pxx_model,
    SOC_SAND_IN uint32  flags,
    SOC_SAND_IN SOC_SAND_PP_SYSTEM_FEC_ID  default_sys_fec
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    indx;
  PCP_SW_DB_IPV4_INFO
    *ipv4_info;
  PCP_IPV4_LPM_MNGR_INIT_INFO
    *lpm_init_info;
  uint8
    support_defrag;
  SOC_SAND_GROUP_MEM_LL_INFO
    *members_info;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Pcp_sw_db.device[unit]->ipv4_info;

  ipv4_info->nof_vrfs = nof_vrfs;
  SOC_SAND_MALLOC(ipv4_info->vrf_modified_bitmask, (sizeof(*ipv4_info->vrf_modified_bitmask)) * PCP_SW_IPV4_VRF_BITMAP_SIZE, "ipv4_info->vrf_modified_bitmask");
  soc_sand_os_memset(ipv4_info->vrf_modified_bitmask, 0, (sizeof(*ipv4_info->vrf_modified_bitmask)) * PCP_SW_IPV4_VRF_BITMAP_SIZE);

  ipv4_info->default_fec = default_sys_fec;
  ipv4_info->uc_host_table_resources = PCP_FRWRD_IPV4_HOST_TABLE_RESOURCE_LEM_THEN_LPM;
  ipv4_info->mc_host_table_resources = PCP_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE_TCAM_ONLY;
  
  /* for each vrf, there is a default route */
  ipv4_info->nof_lpm_entries_in_lpm = 0;

  pcp_PCP_IPV4_LPM_MNGR_INFO_clear(&ipv4_info->lpm_mngr);

  ipv4_info->free_list_size = 0;
  PCP_ALLOC(ipv4_info->free_list,PCP_IPV4_LPM_FREE_LIST_ITEM_INFO,PCP_SW_DB_FREE_LIST_MAX_SIZE, "ipv4_info->free_list");
  

  lpm_init_info = &(ipv4_info->lpm_mngr.init_info);

  lpm_init_info->prime_handle = unit;
  lpm_init_info->sec_handle = 0;
  lpm_init_info->nof_vrf_bits = nof_vrf_bits;
  lpm_init_info->nof_banks = nof_banks;

  SOC_SAND_MALLOC(lpm_init_info->nof_bits_per_bank, sizeof(*lpm_init_info->nof_bits_per_bank) * nof_banks, "lpm_init_info->nof_bits_per_bank");
  soc_sand_os_memcpy(lpm_init_info->nof_bits_per_bank, nof_bits_per_bank, sizeof(*lpm_init_info->nof_bits_per_bank) * nof_banks);
  
  SOC_SAND_MALLOC(lpm_init_info->bank_to_mem, sizeof(*lpm_init_info->bank_to_mem) * nof_banks,"lpm_init_info->bank_to_mem");
  soc_sand_os_memcpy(lpm_init_info->bank_to_mem, bank_to_mem, sizeof(*lpm_init_info->bank_to_mem) * nof_banks);


  SOC_SAND_MALLOC(lpm_init_info->mem_to_bank, sizeof(*lpm_init_info->mem_to_bank) * nof_mems,"lpm_init_info->mem_to_bank");
  soc_sand_os_memcpy(lpm_init_info->mem_to_bank, mem_to_bank, sizeof(*lpm_init_info->mem_to_bank) * nof_mems);

  lpm_init_info->nof_mems = nof_mems;
  
  SOC_SAND_MALLOC(lpm_init_info->nof_rows_per_mem, sizeof(*lpm_init_info->nof_rows_per_mem) * nof_mems, "lpm_init_info->nof_rows_per_mem");
  soc_sand_os_memcpy(lpm_init_info->nof_rows_per_mem, nof_rows_per_mem, sizeof(*lpm_init_info->nof_rows_per_mem) * nof_mems);
  
  lpm_init_info->mem_alloc_get_fun = pcp_ipv4_lpm_test_mem_to_mem_allocator_get;
  lpm_init_info->pat_tree_get_fun = pcp_ipv4_lpm_vrf_to_lpm_db;
  lpm_init_info->pxx_model = pxx_model;
  lpm_init_info->nof_lpms = nof_vrfs;

  lpm_init_info->nof_entries_for_hw_lpm_set_fun = pcp_sw_db_ipv4_nof_lpm_entries_in_lpm_set;
  lpm_init_info->nof_entries_for_hw_lpm_get_fun = pcp_sw_db_ipv4_nof_lpm_entries_in_lpm_get;
  lpm_init_info->max_nof_entries_for_hw_lpm = 10 * 1024;
  support_defrag = (uint8) flags & PCP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG;
 /*
  * init the LPM- PAT trees
  */
  SOC_SAND_MALLOC(lpm_init_info->lpms, sizeof(SOC_SAND_PAT_TREE_INFO) * lpm_init_info->nof_lpms, "lpm_init_info->lpms");
  soc_sand_os_memset(lpm_init_info->lpms, 0, sizeof(SOC_SAND_PAT_TREE_INFO) * lpm_init_info->nof_lpms);

  if (flags & PCP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE)
  {
    lpm_init_info->flags |= PCP_LPV4_LPM_SUPPORT_CACHE;
  }
  if (flags & PCP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG)
  {
    lpm_init_info->flags |= PCP_LPV4_LPM_SUPPORT_DEFRAG;
  }
  for (indx = 0; indx < lpm_init_info->nof_lpms; ++indx)
  {
    lpm_init_info->lpms[indx].tree_size = max_nof_routes[indx]*2; /*pat_size*/
    lpm_init_info->lpms[indx].node_set_fun = pcp_sw_db_pat_tree_node_set;
    lpm_init_info->lpms[indx].node_get_fun = pcp_sw_db_pat_tree_node_get;
    lpm_init_info->lpms[indx].node_ref_get_fun = NULL;/*pcp_sw_db_pat_tree_node_ref_get;*/
    lpm_init_info->lpms[indx].root_set_fun = pcp_sw_db_pat_tree_root_set;
    lpm_init_info->lpms[indx].root_get_fun = pcp_sw_db_pat_tree_root_get;
    lpm_init_info->lpms[indx].prime_handle = unit;
    lpm_init_info->lpms[indx].sec_handle = indx;
    lpm_init_info->lpms[indx].support_cache = (uint8) flags & PCP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE;
  }
  /*
  * init the Mem allocator
  */
  SOC_SAND_MALLOC(lpm_init_info->mem_allocators, sizeof(PCP_ARR_MEM_ALLOCATOR_INFO) * lpm_init_info->nof_mems, "lpm_init_info->mem_allocators");

  for (indx = 0; indx < lpm_init_info->nof_mems; ++indx)
  {
    PCP_ARR_MEM_ALLOCATOR_INFO_clear(&(lpm_init_info->mem_allocators[indx]));
    lpm_init_info->mem_allocators[indx].entry_get_fun = pcp_sw_db_ipv4_arr_mem_allocator_entry_get;
    lpm_init_info->mem_allocators[indx].entry_set_fun = pcp_sw_db_ipv4_arr_mem_allocator_entry_set;
    lpm_init_info->mem_allocators[indx].free_set_fun = pcp_sw_db_ipv4_arr_mem_allocator_free_entry_set;
    lpm_init_info->mem_allocators[indx].free_get_fun = pcp_sw_db_ipv4_arr_mem_allocator_free_entry_get;
    lpm_init_info->mem_allocators[indx].write_fun = pcp_sw_db_ipv4_arr_mem_allocator_write;
    lpm_init_info->mem_allocators[indx].read_fun = pcp_sw_db_ipv4_arr_mem_allocator_read;
    lpm_init_info->mem_allocators[indx].entry_move_fun = pcp_sw_db_ipv4_lpm_mem_align;
    lpm_init_info->mem_allocators[indx].instance_prim_handle = unit;
    lpm_init_info->mem_allocators[indx].instance_sec_handle = indx;
    lpm_init_info->mem_allocators[indx].entry_size = 2; /* enrty width 2 longs */
    lpm_init_info->mem_allocators[indx].nof_entries = lpm_init_info->nof_rows_per_mem[indx];
    lpm_init_info->mem_allocators[indx].support_caching = (uint8) flags & PCP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE;
    lpm_init_info->mem_allocators[indx].support_defragment = (indx==0)?FALSE:support_defrag;
    lpm_init_info->mem_allocators[indx].max_block_size = (indx==0)?1:((1 << nof_bits_per_bank[indx])*2);
  }
  /* allocate information for defragment if needed */
  if (lpm_init_info->flags & PCP_LPV4_LPM_SUPPORT_DEFRAG)
  {
    PCP_ALLOC_ANY_SIZE(lpm_init_info->rev_ptrs,SOC_SAND_GROUP_MEM_LL_INFO*,lpm_init_info->nof_mems, "lpm_init_info->rev_ptrs");
    /* allocate member groups */
    /* for first there is no pointers on it*/
    for (indx = 1; (uint32)indx < lpm_init_info->nof_mems; ++indx)
    {
      PCP_ALLOC_ANY_SIZE(lpm_init_info->rev_ptrs[indx],SOC_SAND_GROUP_MEM_LL_INFO,1, "lpm_init_info->rev_ptrs[indx]");
      members_info = lpm_init_info->rev_ptrs[indx];
      soc_sand_SAND_GROUP_MEM_LL_INFO_clear(members_info);
      members_info->auto_remove = TRUE;
      members_info->group_set_fun = pcp_sw_db_lpm_ll_group_entry_set;
      members_info->group_get_fun = pcp_sw_db_lpm_ll_group_entry_get;
      members_info->member_set_fun = pcp_sw_db_lpm_ll_member_entry_set;
      members_info->member_get_fun = pcp_sw_db_lpm_ll_member_entry_get;
      members_info->nof_elements = lpm_init_info->nof_rows_per_mem[indx-1];/* pointers to me */

      members_info->nof_groups = lpm_init_info->nof_rows_per_mem[indx];; /*size of fec table*/
      members_info->instance_prim_handle = unit;
      members_info->instance_sec_handle = indx; /*the use of the group*/
      members_info->support_caching = (uint8)lpm_init_info->flags & PCP_LPV4_LPM_SUPPORT_CACHE;
    }
  }
 /*
  * init the memory shadow
  */
  res = pcp_ipv4_lpm_mngr_create(&ipv4_info->lpm_mngr);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_ipv4_initialize()",0,0);
}


uint32
  pcp_sw_db_ipv4_terminate(
    SOC_SAND_IN  int unit
  )
{
  PCP_SW_DB_IPV4_INFO
    *ipv4_info;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ipv4_info = Pcp_sw_db.device[unit]->ipv4_info;
  if (ipv4_info == NULL)
  {
    return SOC_SAND_OK;
  }

  PCP_FREE_ANY_SIZE(ipv4_info->free_list);

  pcp_ipv4_lpm_mngr_destroy(&ipv4_info->lpm_mngr);
  soc_sand_os_free_any_size(ipv4_info->lpm_mngr.init_info.nof_bits_per_bank);
  soc_sand_os_free_any_size(ipv4_info->lpm_mngr.init_info.bank_to_mem);
  soc_sand_os_free_any_size(ipv4_info->lpm_mngr.init_info.mem_to_bank);
  soc_sand_os_free_any_size(ipv4_info->lpm_mngr.init_info.nof_rows_per_mem);
  soc_sand_os_free_any_size(ipv4_info->lpm_mngr.init_info.lpms);
  soc_sand_os_free_any_size(ipv4_info->lpm_mngr.init_info.mem_allocators);
  soc_sand_os_free_any_size(ipv4_info->vrf_modified_bitmask);

  PCP_FREE_ANY_SIZE(Pcp_sw_db.device[unit]->ipv4_info);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_ipv4_terminate()",0,0);
}


uint32
  pcp_sw_db_ipv4_lpm_hw_info_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT PCP_SW_DB_LPM_INFO                          *lpm_info
)
{
  PCP_SW_DB_IPV4_FIELD_GET(
    unit,
    ipv4_info->lpm_hw_info,
    lpm_info
  );
}

uint32
  pcp_sw_db_ipv4_lpm_hw_info_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN PCP_SW_DB_LPM_INFO                           *lpm_info
)
{
  PCP_SW_DB_IPV4_FIELD_SET(
    unit,
    ipv4_info->lpm_hw_info,
    lpm_info
  );
}


uint32
  pcp_sw_db_ipv4_cache_mode_for_ip_type_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                     type,
    SOC_SAND_OUT uint8	                          	     *pending_op
)
{
  uint32
    res = SOC_SAND_OK,
    bitmask = 0;

  res = pcp_sw_db_ipv4_cache_mode_get(
          unit,
         &bitmask
        );

  *pending_op = (type & bitmask) ? TRUE : FALSE;

  return res;
}



uint32
  pcp_sw_db_ipv4_cache_mode_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint32                                     *cache_mode_bitmask
)
{
  PCP_SW_DB_IPV4_FIELD_GET(
    unit,
    ipv4_info->cache_mode,
    cache_mode_bitmask
  );
}

uint32
  pcp_sw_db_ipv4_cache_mode_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                     cache_mode_bitmask
)
{
  PCP_SW_DB_IPV4_FIELD_SET(
    unit,
    ipv4_info->cache_mode,
    &cache_mode_bitmask
  );
}



uint32
  pcp_sw_db_ipv4_cache_modified_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint8	                          	     modified
)
{
  PCP_SW_DB_IPV4_FIELD_SET(
    unit,
    ipv4_info->cache_modified,
    &modified
  );
}

uint32
  pcp_sw_db_ipv4_cache_modified_get(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_OUT uint8	                          	     *modified
)
{
  PCP_SW_DB_IPV4_FIELD_GET(
    unit,
    ipv4_info->cache_modified,
    modified
  );
}

uint32
  pcp_sw_db_ipv4_cache_vrf_modified_get(
    SOC_SAND_IN int                                  unit,
    SOC_SAND_IN  uint32                                  vrf_ndx,
    SOC_SAND_OUT uint32                                  *vrf_modified
)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Pcp_sw_db.device[unit] == NULL || Pcp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }
  *vrf_modified = 0;

  res = soc_sand_bitstream_set_any_field(
          Pcp_sw_db.device[unit]->ipv4_info->vrf_modified_bitmask,
          vrf_ndx,
          1,
          (vrf_modified)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_ipv4_cache_vrf_modified_set()",vrf_ndx,0);
}

uint32
  pcp_sw_db_ipv4_cache_vrf_modified_set(
    SOC_SAND_IN int                                  unit,
    SOC_SAND_IN  uint32                                  vrf_ndx,
    SOC_SAND_IN uint32                                   vrf_modified
)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (Pcp_sw_db.device[unit] == NULL || Pcp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }
  res = soc_sand_bitstream_set_any_field(
          &(vrf_modified),
          vrf_ndx,
          1,
          Pcp_sw_db.device[unit]->ipv4_info->vrf_modified_bitmask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_sw_db_ipv4_cache_vrf_modified_set()",vrf_ndx,0);
}

uint32
pcp_sw_db_ipv4_cache_modified_bitmap_set(
  SOC_SAND_IN int                                    unit,
  SOC_SAND_IN uint32	                          	     *modified_bitmap
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  if (Pcp_sw_db.device[unit] == NULL || Pcp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }

  soc_sand_os_memcpy(
    Pcp_sw_db.device[unit]->ipv4_info->vrf_modified_bitmask,
    modified_bitmap,
    PCP_SW_IPV4_VRF_BITMAP_SIZE * sizeof(uint32)
    );

  return soc_sand_ret;
}

uint32
pcp_sw_db_ipv4_cache_modified_bitmap_get(
  SOC_SAND_IN int                                    unit,
  SOC_SAND_OUT uint32	                          	     *modified_bitmap
)
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  if (Pcp_sw_db.device[unit] == NULL || Pcp_sw_db.device[unit]->ipv4_info == NULL)
  {
    return SOC_SAND_ERR;
  }

  soc_sand_os_memcpy(
    modified_bitmap,
    Pcp_sw_db.device[unit]->ipv4_info->vrf_modified_bitmask,
    PCP_SW_IPV4_VRF_BITMAP_SIZE * sizeof(uint32)
  );

    return soc_sand_ret;
}


uint32
  pcp_sw_db_ipv4_lpm_mngr_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN PCP_IPV4_LPM_MNGR_INFO  *lpm_mngr
  )
{
    PCP_SW_DB_FIELD_SET(
      unit,
      ipv4_info->lpm_mngr,
      lpm_mngr
    );
}

uint32
  pcp_sw_db_ipv4_lpm_mngr_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT  PCP_IPV4_LPM_MNGR_INFO  *lpm_mngr
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  if (Pcp_sw_db.device[unit] == NULL || Pcp_sw_db.device[unit]->ipv4_info == NULL){
    return SOC_SAND_ERR;
  }

  if (Pcp_sw_db.device[unit]->op_mode.ipv4_info.nof_vrfs == 0)
  {
    return SOC_SAND_VALUE_ABOVE_MAX_ERR;/* no routing enabled */
  }

  soc_sand_ret = soc_sand_os_memcpy(
      lpm_mngr,
      &(Pcp_sw_db.device[unit]->ipv4_info->lpm_mngr),
      sizeof(*lpm_mngr)
    );
  return soc_sand_ret;
}

#ifdef SOC_SAND_DEBUG
uint32
  pcp_sw_db_ipv4_lem_add_fail_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint8    *ipv4_lem_add_fail
  )
{
  PCP_SW_DB_IPV4_FIELD_SET(
    unit,
    ipv4_info->lem_add_fail,
    ipv4_lem_add_fail
    );
}

uint32
  pcp_sw_db_ipv4_lem_add_fail_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  uint8  *ipv4_lem_add_fail
  )
{
  PCP_SW_DB_IPV4_FIELD_GET(
    unit,
    ipv4_info->lem_add_fail,
    ipv4_lem_add_fail
    );
}



void
  PCP_SW_DB_LPM_INIT_INFO_print(
    PCP_SW_DB_LPM_INIT_INFO* info
  )
{
  uint32
    indx;

  if (info == NULL)
  {
    return;
  }
  
  LOG_CLI((BSL_META("\n\r IPv4 init: \n\r")));
  LOG_CLI((BSL_META("nof_vrf_bits %u\n\r"), info->nof_vrf_bits));
  LOG_CLI((BSL_META("nof_banks %u\n\r"), info->nof_banks));
  LOG_CLI((BSL_META("nof_mems %u\n\r"), info->nof_mems));

  LOG_CLI((BSL_META("mem_to_bank: ")));
  for (indx = 0; indx < info->nof_mems; ++indx)
  {
    LOG_CLI((BSL_META("%u  "), info->mem_to_bank[indx]));
  }

  LOG_CLI((BSL_META("\n\r nof_rows_per_mem: ")));
  for (indx = 0; indx < info->nof_mems; ++indx)
  {
    LOG_CLI((BSL_META("0x%08x  "), info->nof_rows_per_mem[indx]));
  }

  LOG_CLI((BSL_META("\n\r nof_bits_per_bank: ")));
  for (indx = 0; indx < info->nof_banks; ++indx)
  {
    LOG_CLI((BSL_META("%u  "), info->nof_bits_per_bank[indx]));
  }

  LOG_CLI((BSL_META("\n\r bank_to_mem: ")));
  for (indx = 0; indx < info->nof_banks; ++indx)
  {
    LOG_CLI((BSL_META("%u  "), info->bank_to_mem[indx]));
  }
  LOG_CLI((BSL_META("\n\r")));

  return;


}

#endif

uint8
  pcp_ipv4_lpm_mngr_pat_tree_node_data_is_identical_fun(
  SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_0,
  SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_1
  )
{
  if( (PCP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(node_info_0->data) != SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM) ||
      (PCP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(node_info_1->data) != SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM))
  {
    return TRUE;
  }

  if( (PCP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(node_info_0->data) == SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE) ||
    (PCP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(node_info_1->data) == SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE))
  {
    /* It is really dead code because of PCP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(x) is (x>>30) & 0x3
     * and hence, it can't be 4 (SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE).
     * However, ignore temporary */
    /* coverity[dead_error_line] */
    return TRUE;
  }

  if( PCP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(node_info_0->data) == PCP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(node_info_1->data))
  {
    return (uint8)(node_info_1->data == node_info_0->data && node_info_0->key == node_info_1->key && node_info_0->prefix == node_info_1->prefix);
  }

  return FALSE;
}

uint8
  pcp_ipv4_lpm_mngr_pat_node_is_skip_in_lpm_identical_data_query_fun(
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info
  )
{
  /* PCP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(x) is x >> 30, and hence it can't be 4 (SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE).
   * However, temporary - ignore */
  /* coverity [result_independent_of_operands] */
  if( node_info->is_prefix &&
      ((PCP_IPV4_LPM_MGR_PAT_DATA_TO_HW_TARGET_TYPE(node_info->data) != SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM) ||
       (PCP_IPV4_LPM_MGR_PAT_DATA_TO_PENDING_OP_TYPE(node_info->data) == SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE)))
  {
    return TRUE;
  }

  return FALSE;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
