/* $Id: dpp_multicast_ingress.c,v  $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST

#include <soc/dpp/multicast_imp.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_mem.h>


#include <soc/mcm/memregs.h>

#include <shared/bsl.h>

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/mbcm.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */


/* } */

/*************
 *  MACROS   *
 *************/
/* { */

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

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 * Initialization of the Arad blocks configured in this module.
 * Called as part of the initialization sequence.
 */
uint32 dpp_mc_init(
    SOC_SAND_IN  int                 unit
)
{
#ifdef _ARAD_MC_TEST_UNNEEDED_FABRIC_CODE_0
  uint32 reg_idx, fld_idx;
  SOC_TMC_MULT_FABRIC_ACTIVE_LINKS links;
#endif
  uint32 mc_tc, idx;
  SOC_SAND_U32_RANGE fmc_q_range;
  SOCDNX_INIT_FUNC_DEFS;

  /*
   *  Traffic Class to multicast traffic class mapping - set MULT-TC = TC/2
   */
  for (idx = 0; idx < ARAD_NOF_TRAFFIC_CLASSES; idx ++)
  {
    mc_tc = idx/2;
    SOCDNX_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mult_fabric_traffic_class_to_multicast_cls_map_set, (unit, idx, mc_tc)));
  }

#ifdef _ARAD_MC_TEST_UNNEEDED_FABRIC_CODE_0
  /*
   *  Enable auto-detect of active fabric links, with no mask.
   *  For this purpose - set all links as "active", and then enable auto-refresh
   */
  for (idx = 0; idx < SOC_DPP_DEFS_GET(unit, nof_fabric_links); idx++)
  {
    reg_idx = ARAD_REG_IDX_GET(idx, SOC_SAND_REG_SIZE_BITS);
    fld_idx = ARAD_FLD_IDX_GET(idx, SOC_SAND_REG_SIZE_BITS);
    SOC_SAND_SET_BIT(links.bitmap[reg_idx], 0x1, fld_idx);
  }

  SOCDNX_SAND_IF_ERR_EXIT(arad_mult_fabric_active_links_set_unsafe(unit, &links, TRUE));
#endif

#ifdef _ARAD_MC_TEST_DOUBLE_INITIALIZATION_0
  {
    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
      eg_vlan_rep;
    eg_vlan_rep.mc_id_low  = 0;
    eg_vlan_rep.mc_id_high = SOC_TMC_MULT_EG_VLAN_NOF_IDS_MAX - 1;
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_bitmap_group_range_set(unit, &eg_vlan_rep));
  }
#endif
  
    soc_sand_SAND_U32_RANGE_clear(&fmc_q_range);
    fmc_q_range.start = 0;
    fmc_q_range.end = 3;
    SOCDNX_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_enhanced_set,(unit, BCM_CORE_ALL, &fmc_q_range)));

exit:
  SOCDNX_FUNC_RETURN;
}



/*
 * This function checks if the multicast group is open (possibly empty).
 * returns TRUE if the group is open (start of a group), otherwise FALSE.
 */
int dpp_mult_does_group_exist(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  mcid,       /* MC ID of the group */
    SOC_SAND_IN  int     is_egress,  /* is the MC group an egress group */
    SOC_SAND_OUT uint8   *does_edxist
)
{
  const dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  uint32 nof_mc_ids = is_egress ? SOC_DPP_CONFIG(unit)->tm.nof_mc_ids : SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids;

  SOCDNX_INIT_FUNC_DEFS;

  if (mcid >= nof_mc_ids) {
    LOG_ERROR(BSL_LS_SOC_MULTICAST, (BSL_META_U(unit, "multicast ID %u is not under the number of multicast IDs: %u\n"), mcid, nof_mc_ids));
    *does_edxist = FALSE;
    SOC_EXIT;
  }
  if (!is_egress) { /* ingress group */
    if (DPP_MCDS_GET_TYPE(mcds, mcid) != DPP_MCDS_TYPE_VALUE_INGRESS_START) {
      *does_edxist = FALSE;
      SOC_EXIT;
    }
    DPP_MC_ASSERT(mcid == DPP_MCDS_GET_PREV_ENTRY(mcds, mcid)); /* verify that mcid is a start of a linked list */
    *does_edxist = TRUE;
    SOC_EXIT;
  } else { /* egress group */
    SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_get(unit, mcid, does_edxist));
  }

exit:
  SOCDNX_FUNC_RETURN;
}


/*
 * This function checks if the multicast group is open (possibly empty).
 * returns TRUE if the group is open (start of a group), otherwise FALSE.
 */
uint32 dpp_mult_does_group_exist_ext( 
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_MULT_ID mcid,      /* MC ID of the group */
    SOC_SAND_IN  int             is_egress, /* is the MC group an egress group */
    SOC_SAND_OUT uint8           *is_open   /* returns FALSE if not open */
)
{
  const dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  uint32 nof_mc_ids = is_egress ? SOC_DPP_CONFIG(unit)->tm.nof_mc_ids : SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids;
  SOCDNX_INIT_FUNC_DEFS;

  if (mcid >= nof_mc_ids) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("multicast ID %u is not under the number of multicast IDs: %u"), mcid, nof_mc_ids));
  }
  if (!is_egress) { /* ingress group */
    if (DPP_MCDS_GET_TYPE(mcds, mcid) != DPP_MCDS_TYPE_VALUE_INGRESS_START) {
      *is_open = FALSE;
    } else {
      DPP_MC_ASSERT(mcid == DPP_MCDS_GET_PREV_ENTRY(mcds, mcid)); /* verify that mcid is a start of a linked list */
      *is_open = TRUE;
    }
  } else { /* egress group */
    SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_get(unit, mcid, is_open));
  }

exit:
  SOCDNX_FUNC_RETURN;
}


/*
 * Relocate the given used entries, not disturbing multicast traffic to
 * the group containing the entries.
 * SWDB and hardware are updated accordingly.
 * If relocation_block_size is 0, then the function calculates
 * relocation_block_start and relocation_block_size by itself to suit mcdb_index.
 * After successful relocation, the block that was relocated is freed, except for entry mcdb_index.
 *
 * This function may overwrite the mc group replications stored in the mcds.
 */

uint32
  dpp_mcdb_relocate_entries(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                        mcdb_index,             /* table index needed for the start of a group */
    SOC_SAND_IN  uint32                        relocation_block_start, /* the start of the relocation block */
    SOC_SAND_IN  dpp_free_entries_block_size_t relocation_block_size,  /* the size of the relocation block */
    SOC_SAND_OUT SOC_TMC_ERROR                 *out_err                /* return possible errors that the caller may want to ignore: insufficient memory */
)
{
    uint32 start = relocation_block_start, found_block_start, prev_entry;
    dpp_free_entries_block_size_t size = relocation_block_size, found_block_size;
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    uint32 entry_type = DPP_MCDS_GET_TYPE(mcds, mcdb_index), cud2 = DPP_MC_NO_2ND_CUD;
    dpp_free_entries_blocks_region_t *region = 0;
    int free_alloced_block = 0;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(out_err);

    if (!size) { /* need to calculate the relocated entries */
        SOCDNX_IF_ERR_EXIT(dpp_mcds_get_relocation_block(mcds, mcdb_index, &start, &size));
        if (!size) {
            DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_FREE(DPP_MCDS_GET_TYPE(mcds, mcdb_index)));
            SOC_EXIT; /* a relocation is not needed */
        }
    }

    /* entry must be used, and if a block (size>1) is relocated, this must be non tdm egress */
    DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_USED(entry_type) && (size == 1 || entry_type == DPP_MCDS_TYPE_VALUE_EGRESS));

    SOCDNX_IF_ERR_EXIT( /* get free entries */
        dpp_mcds_get_free_entries_block(mcds, DPP_MCDS_GET_FREE_BLOCKS_PREFER_SIZE | DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL,
            size, size, &found_block_start, &found_block_size));
    if (!found_block_size) {
        *out_err = _SHR_E_FULL;
        SOC_EXIT;
    }
    DPP_MC_ASSERT(found_block_size <= size);
    prev_entry = DPP_MCDS_GET_PREV_ENTRY(mcds, start);

    free_alloced_block = 1;
    if (found_block_size == size) { /* can relocate all the block to the allocated block */
        dpp_free_entries_block_size_t s;
        uint32 after_block; /* the index if the entry after the block (pointed to by the last block entry) */

        SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds,unit, start + size - 1, entry_type, &after_block));

        for (s = 0; s < size; ++s) {
            SOCDNX_IF_ERR_EXIT( /* copy entry to new location and write to hardware */
                dpp_mcdb_copy_write(unit, start + s, found_block_start + s));
            DPP_MCDS_SET_PREV_ENTRY(mcds, found_block_start + s, s ? found_block_start + (s-1) : prev_entry);
        }
        SOCDNX_IF_ERR_EXIT(MCDS_SET_NEXT_POINTER( /* point to the new block instead of to the old one */
            mcds, unit, prev_entry, entry_type, found_block_start));
        free_alloced_block = 0;
        if (after_block != DPP_MC_EGRESS_LINK_PTR_END && after_block != MCDS_INGRESS_LINK_END(mcds)) { /* link the entry after the relocated block back to the new block */
            DPP_MCDS_SET_PREV_ENTRY(mcds, after_block, found_block_start + size - 1);
        }
    } else { /* can not relocate as one block, is non-tdm egress */
        uint32 list_next; /* the next entry in the list after the block to be relocated */
        SOCDNX_IF_ERR_EXIT(dpp_mcds_copy_replications_from_egress_block( /* set mcdb with the block's entries */
            unit, 1, start, size, &cud2, &list_next));
        free_alloced_block = 0;

        if (cud2 != DPP_MC_NO_2ND_CUD) { /* handle a change of the 2nd CUD which is always at the end of a block, as the format has a pointer */
            DPP_MC_ASSERT(SOC_IS_JERICHO(unit));
            
            
        }

        SOCDNX_IF_ERR_EXIT(mcds->set_egress_linked_list( /* replace block by a linked list */
            unit, FALSE, prev_entry, list_next, found_block_start, found_block_size, DPP_MC_CORE_BITAMAP_CORE_0, &found_block_start, out_err));
        if (*out_err) {
            SOC_EXIT;
        }
    }

    /* after reallocating, need to free the old block except for the entry which will start the linked list of the new group */
    if (mcdb_index > start) { /* free entries in relocated block before the entry to be used as a start of a MC group */
        if (!region) {
            region = dpp_mcds_get_region(mcds, mcdb_index);
        }
        SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, start, mcdb_index - 1, region, McdsFreeBuildBlocksAdd_AllMustBeUsed));
    }
    if (mcdb_index + 1 < start + size) { /* free entries in relocated block before the entry to be used as a start of a MC group */
        if (!region) {
            region = dpp_mcds_get_region(mcds, mcdb_index);
        }
        SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, mcdb_index + 1, start + (size - 1), region, McdsFreeBuildBlocksAdd_AllMustBeUsed));
    }
    *out_err = _SHR_E_NONE;

exit:
    if (free_alloced_block) {
        SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, 
            found_block_start, found_block_start + (found_block_size - 1), dpp_mcds_get_region(mcds, found_block_start), McdsFreeBuildBlocksAdd_AllMustBeUsed));
    }
    SOCDNX_FUNC_RETURN;
}


/*
 * Free a linked list that is not used any more, updating mcds and hardware.
 * Stop at the given entry, and do not free it.
 * The given linked list must not include the first entry of the group.
 * All entries in the linked list should be of the given type.
 */

uint32
  dpp_mcdb_free_linked_list_till_my_end(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    first_index,  /* table index of the first entry in the linked list to free */
    SOC_SAND_IN  uint32    entries_type, /* the type of the entries in the list */
    SOC_SAND_IN  uint32    end_index     /* the index of the end of the list to be freed */
)
{
  uint32 cur_index;
  uint32 block_start = 0, block_end = 0; /* will mark a block of consecutive entries in the linked list to free together */
  uint32 range_start = 0, range_end = 0; /* will mark the consecutive range of the region in which the block can grow */
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  dpp_free_entries_blocks_region_t *region = 0;
  SOCDNX_INIT_FUNC_DEFS;

  for (cur_index = first_index; cur_index != end_index; ) { /* loop over the linked list */
    DPP_MC_ASSERT(cur_index > 0 && cur_index < MCDS_INGRESS_LINK_END(mcds));

    if (block_end) { /* we are in a block, try enlarging it with the entry */
      if (cur_index == block_end + 1 && cur_index <= range_end) { /* Can this entry join the block at its end? */
        block_end = cur_index;
      } else if (cur_index + 1 == block_start && cur_index >= range_start) { /* Can this entry join the block at its start? */
        block_start = cur_index;
      } else { /* can't enlarge block, add block to free list and start a new one */
        SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, block_start, block_end, region, McdsFreeBuildBlocksAdd_AllMustBeUsed));
        region = dpp_mcds_get_region_and_consec_range(mcds, cur_index, &range_start, &range_end); /* get range in which we can accumulate a block */
        block_start = block_end = cur_index;
      }
    } else { /* we are at at the first entry */
      region = dpp_mcds_get_region_and_consec_range(mcds, cur_index, &range_start, &range_end); /* get range in which we can accumulate a block */
      block_start = block_end = cur_index;
    }
    SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds, unit, cur_index, entries_type, &cur_index));
  }
  if (block_end) { /* write the last discovered consecutive */
    SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, block_start, block_end, region, McdsFreeBuildBlocksAdd_AllMustBeUsed));
  }

exit:
  SOCDNX_FUNC_RETURN;
}

/*
 * Free a linked list that is not used any more, updating mcds and hardware.
 * The given linked list must not include the first entry of the group.
 * All entries in the linked list should be of the given type.
 */

uint32
  dpp_mcdb_free_linked_list(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  first_index, /* table index of the first entry in the linked list to free */
    SOC_SAND_IN uint32  entries_type /* the type of the entries in the list */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  return dpp_mcdb_free_linked_list_till_my_end(unit, first_index, entries_type, 
    (DPP_MCDS_TYPE_IS_INGRESS(entries_type) ? MCDS_INGRESS_LINK_END(mcds) : DPP_MC_EGRESS_LINK_PTR_END));
}


/*
 * Return the contents of a multicast linked list group of a given type.
 * The contents is returned in the mcds buffer. Egress destination ports are TM/PP ports.
 * if the group size is bigger than the specified max_size, only max_size entries are returned,
 * it will not be an error, and the actual size of the group is returned.
 * The group must be open.
 */

uint32 dpp_mcds_get_group(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  dpp_mc_core_bitmap_t cores_to_get, /* the linked list cores to get */
    SOC_SAND_IN  uint8                do_clear,     /* if zero, replications will be added in addition to existing ones, otherwise previous replications will be cleared */
    SOC_SAND_IN  uint8                get_bm_reps,  /* Should the function return bitmap replications in every core (if non zero) or only in core 0 */
    SOC_SAND_IN  uint32               group_id,     /* the mcid of the group */
    SOC_SAND_IN  uint32               group_type,   /* the type of the group (of the entries in the list) */
    SOC_SAND_IN  uint16               max_size,     /* the maximum number of members to return from the group */
    SOC_SAND_OUT uint16               *group_size   /* the returned actual group size */
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    uint32 entry_type = DPP_MCDS_TYPE_GET_NONE_START(group_type); /* type for the non first entry */
    const uint16 max_group_size = DPP_MCDS_TYPE_IS_INGRESS(group_type) ? DPP_MULT_MAX_INGRESS_REPLICATIONS : DPP_MULT_MAX_EGRESS_REPLICATIONS;
    uint32 end_index = DPP_MCDS_TYPE_IS_INGRESS(group_type) ? MCDS_INGRESS_LINK_END(mcds) : DPP_MC_EGRESS_LINK_PTR_END;
    uint32 cur_index = group_id, cud2;
    uint16 entries_remaining = max_size > max_group_size ? max_group_size : max_size; /* max replications to return */
    uint8 get_bm_reps_in_this_core;
    int core;
    dpp_mc_core_bitmap_t cores_bm;
    unsigned is_egress = DPP_MCDS_TYPE_IS_EGRESS(group_type);

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(group_size);
    DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_USED(group_type));
    *group_size = 0;

    if (do_clear) {
      dpp_mcds_clear_replications(mcds, group_type); /* reset the returned replications to none */
    }

    DPP_MC_FOREACH_CORE(cores_to_get, cores_bm, core) {
        cud2 = DPP_MC_NO_2ND_CUD;
        get_bm_reps_in_this_core = 1;
        if (is_egress) { /* calculate index of first group entry */
            cur_index = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, group_id, core);
            if (get_bm_reps == 0 && core != 0) {
                get_bm_reps_in_this_core = 0;
            }
        }
        /* get replications from the first entry */
        SOCDNX_IF_ERR_EXIT(mcds->get_replications_from_entry(unit, core, get_bm_reps_in_this_core, cur_index,
          DPP_MCDS_TYPE_GET_START(group_type), &cud2, &entries_remaining, group_size, &cur_index));

        /* get replications from the rest of the entries */
        while (cur_index != end_index) {
            DPP_MC_ASSERT(cur_index > 0 && cur_index < MCDS_INGRESS_LINK_END(mcds));
            SOCDNX_IF_ERR_EXIT(mcds->get_replications_from_entry(unit, core, get_bm_reps_in_this_core, cur_index, entry_type, &cud2, &entries_remaining, group_size, &cur_index));
            if (*group_size > max_group_size) { /* group is somehow bigger than allowed - internal error */
                DPP_MC_ASSERT(0);
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("group is too big")));
            }
        }
    }
    DPP_MC_ASSERT(core <= (is_egress ? SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores : 1));

exit:
  SOCDNX_FUNC_RETURN;
}


/*********************************************************************
* Initialize MC replication database
* The initialization accesses the replication table as if it was an
* Ingress replication, for all entries (including Egress MC)
**********************************************************************/
uint32 dpp_mult_rplct_tbl_entry_unoccupied_set_all(
    SOC_SAND_IN  int unit
)
{
  const dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_PCID_LITE_SKIP(unit);

#ifdef PLISIM
  if (!SAL_BOOT_PLISIM) /* do not init MCDB hardware in simulation, later instead of reading it into the mcds, use the init values */
#endif
  {
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IRR_MCDBm, MEM_BLOCK_ANY, mcds->free_value));
  }

exit:
  SOCDNX_FUNC_RETURN;
}

