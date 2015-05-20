/* $Id: dpp_multicast_egress.c,v  $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST

#include <soc/dpp/multicast_imp.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_sw_db.h>


#include <soc/mcm/memregs.h>


/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/TMC/tmc_api_multicast_egress.h>
#include <soc/dpp/ARAD/arad_wb_db.h>
#include <soc/dpp/port_sw_db.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_MAX          (8*1024)


#define DPP_EGQ_VLAN_TABLE_TBL_ENTRY_SIZE 9
#define DPP_FDA_MC_ENTRY_SIZE 3

#define DPP_FDA_BITS_PER_GROUP 2
#define DPP_FDA_GROUPS_PER_ENTRY 36
#define  DPP_FDA_GROUP_MASK ((1 << DPP_FDA_BITS_PER_GROUP) - 1)
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


/* Mark the given egress group as open in the warm boot data */
uint32 dpp_egress_group_open_set(
    SOC_SAND_IN  int     unit, /* device */
    SOC_SAND_IN  uint32  group_id,  /* multicast ID */
    SOC_SAND_IN  uint8   is_open    /* non zero value will mark the group as open */
)
{
    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(group_id < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
    if (arad_sw_db_egress_group_open_get(unit, group_id) == is_open) {
        SOC_EXIT;
    }
    SOCDNX_IF_ERR_EXIT(arad_sw_db_egress_group_open_set(unit, group_id, is_open));
    if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_HW_ENABLE_MODE) {
        /* If the hardware uses a table to control which (per core) egress group is active, then configure it */
        uint32 data[DPP_FDA_MC_ENTRY_SIZE];
        int index = group_id / DPP_FDA_GROUPS_PER_ENTRY;
        int offset = DPP_FDA_BITS_PER_GROUP * (group_id % DPP_FDA_GROUPS_PER_ENTRY);
        int offset_words = offset / SOC_SAND_NOF_BITS_IN_UINT32;
        uint32 group_bits = (offset % SOC_SAND_NOF_BITS_IN_UINT32) << (uint32)DPP_FDA_GROUP_MASK;

        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_MCm(unit, MEM_BLOCK_ANY, index, data));
        if (is_open) {
            data[offset_words] |= group_bits;
        } else {
            data[offset_words] &= ~group_bits;
        }
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_MCm(unit, MEM_BLOCK_ANY, index, data));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

/* Check if the given egress group is created=open, will return 1 if the group is marked as open, or 0 */
uint8 dpp_egress_group_open_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 group_id /* multicast ID */
)
{
  DPP_MC_ASSERT(group_id < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
  return arad_sw_db_egress_group_open_get(unit, group_id);
}


/*
 * Set the outlif (cud) to (local) port mapping from the given cud to the given port.
 */
uint32 dpp_mult_cud_to_port_map_set(
    SOC_SAND_IN int                 unit, /* input device */
    SOC_SAND_IN uint32              cud,  /* input cud/outlif */
    SOC_SAND_IN SOC_TMC_FAP_PORT_ID port  /* input (local egress) port */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  uint32 entry = 0;

  SOCDNX_INIT_FUNC_DEFS;
  if (cud > mcds->max_egr_cud_field || port >= DPP_MULT_EGRESS_PORT_INVALID) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("input too big")));
  }

  soc_mem_field32_set(unit, EGQ_MAP_OUTLIF_TO_DSPm, &entry, DSPf, port);
  SOCDNX_IF_ERR_EXIT(WRITE_EGQ_MAP_OUTLIF_TO_DSPm(unit, MEM_BLOCK_ANY, cud, &entry));

exit:
  SOCDNX_FUNC_RETURN;
}

/*
 * Get the outlif (cud) to (local) port mapping from the given cud.
 */
uint32 dpp_mult_cud_to_port_map_get(
    SOC_SAND_IN  int                 unit, /* input device */
    SOC_SAND_IN  uint32              cud,  /* input cud/outlif */
    SOC_SAND_OUT SOC_TMC_FAP_PORT_ID *port /* output (local egress) port */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  uint32 entry;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(port);
  if (cud > mcds->max_egr_cud_field || *port >= DPP_MULT_EGRESS_PORT_INVALID) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("input too big")));
  }

  SOCDNX_IF_ERR_EXIT(READ_EGQ_MAP_OUTLIF_TO_DSPm(unit, MEM_BLOCK_ANY, cud, &entry));
  *port = soc_mem_field32_get(unit, EGQ_MAP_OUTLIF_TO_DSPm, &entry, DSPf);

exit:
  SOCDNX_FUNC_RETURN;
}



/*
 * Gets the egress multicast group with the specified multicast id.
 * will return up to mc_group_size replications, and the exact
 * Works with both TDM and non TDM groups.
 * The group's replication number is returned in exact_mc_group_size.
 * The number of replications returned in the output arrays is
 * min{mc_group_size, exact_mc_group_size}.
 * It is not an error if the group is not open.
 */
uint32 dpp_mult_eg_get_group(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t group_mcid,           /* group id */
    SOC_SAND_IN  uint32      mc_group_size,        /* maximum replications to return */
    SOC_SAND_OUT soc_gport_t *ports,               /* output ports (array of size mc_group_size) */
    SOC_SAND_OUT soc_if_t    *cuds,                /* output CUDs (array of size mc_group_size) */
    SOC_SAND_OUT uint32      *exact_mc_group_size, /* the number of replications in the group will be returned here */
    SOC_SAND_OUT uint8       *is_open              /* will return if the group is open (false or true) */
)
{
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(exact_mc_group_size);
  SOCDNX_NULL_CHECK(is_open);
  if (mc_group_size && (!ports || !cuds)) { /* we check that the output data pointers are not null if we need to return data */
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("NULL pointer")));
  }



  *is_open = dpp_mult_does_group_exist(unit, group_mcid, TRUE) ;
  if (*is_open) {
    uint16 group_size;
    SOCDNX_IF_ERR_EXIT(dpp_mcds_get_group(
      unit, DPP_MC_CORE_BITAMAP_ALL_ACTIVE_CORES(unit), TRUE, group_mcid, DPP_MCDS_TYPE_VALUE_EGRESS, mc_group_size, &group_size));
    *exact_mc_group_size = group_size;
    SOCDNX_IF_ERR_EXIT(dpp_mcds_copy_replications_to_arrays(unit, 1, mc_group_size,  ports, cuds));
  } else { /* group is not open */
    *exact_mc_group_size = 0;
  }

exit:
  SOCDNX_FUNC_RETURN;
}


/*
 * Read a bitmap from the egress MC bitmap table.
 */
uint32 dpp_egq_vlan_table_tbl_get(
    SOC_SAND_IN   int    unit,
    SOC_SAND_IN   uint32 entry_offset,
    SOC_SAND_OUT  uint32 *vlan_table_tbl_data
)
{
    int core, nof_active_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    uint32 *bitmap_ptr = vlan_table_tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    for (core = 0; core < nof_active_cores; ++core) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_VLAN_TABLEm(unit, EGQ_BLOCK(unit, core), entry_offset, bitmap_ptr));
        bitmap_ptr += SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32;
    }
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Write a bitmap to the egress MC bitmap table.
 */
uint32 dpp_egq_vlan_table_tbl_set(
    SOC_SAND_IN   int                  unit,
    SOC_SAND_IN   uint32               entry_offset,
    SOC_SAND_IN   uint32* vlan_table_tbl_data
)
{
    int core, nof_active_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    const uint32 *bitmap_ptr = vlan_table_tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    for (core = 0; core < nof_active_cores; ++core) {
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_VLAN_TABLEm(unit, EGQ_BLOCK(unit, core), entry_offset, (uint32*)bitmap_ptr));
        bitmap_ptr += SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32;
    }
exit:
    SOCDNX_FUNC_RETURN;
}



/*
 * Gets the egress replications (ports) of the given bitmap.
 * If the bitmap is a vlan egress group, it does not have to be open/created.
 */
uint32 dpp_mult_eg_bitmap_group_get(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  dpp_mc_id_t                           bitmap_id, /* ID of the bitmap */
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group     /* output port bitmap */
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(group);
    SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_get(unit, bitmap_id, &group->bitmap[0]));
exit:
    SOCDNX_FUNC_RETURN;
}


/* entry format writing functions */

/*
 * This function writes egress format 0 (port+CUD replications with a link pointer) to a mcds mcdb entry and then to hardware.
 * The replications to write are specified by structure pointers, NULL pointers mean disabled replications.
 */
uint32 dpp_mult_eg_write_entry_port_cud(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  dpp_mc_id_t    multicast_id_ndx, /* mcdb to write to */
    SOC_SAND_IN  dpp_rep_data_t *rep1,            /* replication 1 */
    SOC_SAND_IN  dpp_rep_data_t *rep2,            /* replication 2 (7 bit port for Arad) */
    SOC_SAND_IN  uint32         next_entry,       /* the next entry */
    SOC_SAND_IN  uint32         prev_entry        /* the previous entry written only to mcds */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, multicast_id_ndx);
  SOCDNX_INIT_FUNC_DEFS;

  mcds->egr_mc_write_entry_port_cud(unit, mcdb_entry, rep1, rep2, next_entry); /* set the hardware fields */
  SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, multicast_id_ndx)); /* write to hardware */

  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, prev_entry); /* set software link to previous entry */
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, prev_entry == multicast_id_ndx ? DPP_MCDS_TYPE_VALUE_EGRESS_START : DPP_MCDS_TYPE_VALUE_EGRESS);

exit:
  SOCDNX_FUNC_RETURN;
}

/*
 * This function writes egress format 4/5 (port_CUD replications with no link pointer) to a mcds mcdb entry and then to hardware.
 * The replications to write are specified by structure pointers, NULL pointers mean disabled replications.
 */
uint32 dpp_mult_eg_write_entry_port_cud_noptr(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN dpp_mc_id_t    multicast_id_ndx, /* mcdb to write to */
    SOC_SAND_IN dpp_rep_data_t *rep1,            /* replication 1 */
    SOC_SAND_IN dpp_rep_data_t *rep2,            /* replication 2 */
    SOC_SAND_IN uint8          use_next,         /* If zero, select format indicating end of linked list, otherwise */
                                                 /* select format indicating that the following entry is next. */
    SOC_SAND_IN uint32         prev_entry        /* the previous entry written only to mcds */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *mcdb_entry = dpp_mcds_get_mcdb_entry(unit, multicast_id_ndx);
  SOCDNX_INIT_FUNC_DEFS;

  mcds->egr_mc_write_entry_port_cud_noptr(unit, mcdb_entry, rep1, rep2, use_next); /* set the hardware fields */
  SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, multicast_id_ndx)); /* write to hardware */

  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, prev_entry); /* set software link to previous entry */
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, prev_entry == multicast_id_ndx ? DPP_MCDS_TYPE_VALUE_EGRESS_START : DPP_MCDS_TYPE_VALUE_EGRESS);

exit:
  SOCDNX_FUNC_RETURN;
}

/*
 * This function writes egress format 2 (CUD only with link pointer) to a mcds mcdb entry and then to hardware.
 * The replications to write are specified by structure pointers, NULL pointers mean disabled replications.
 */
uint32 dpp_mult_eg_write_entry_cud(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN dpp_mc_id_t    multicast_id_ndx, /* mcdb index to write to */
    SOC_SAND_IN dpp_rep_data_t *rep1,            /* replication 1 */
    SOC_SAND_IN dpp_rep_data_t *rep2,            /* replication 2  */
    SOC_SAND_IN uint32         next_entry,       /* the next entry */
    SOC_SAND_IN uint32         prev_entry        /* the previous entry written only to mcds */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *mcdb_entry = dpp_mcds_get_mcdb_entry(unit, multicast_id_ndx);
  SOCDNX_INIT_FUNC_DEFS;

  mcds->egr_mc_write_entry_cud(unit, mcdb_entry, rep1, rep2, next_entry); /* set the hardware fields */
  SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, multicast_id_ndx)); /* write to hardware */

  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, prev_entry); /* set software link to previous entry */
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, prev_entry == multicast_id_ndx ? DPP_MCDS_TYPE_VALUE_EGRESS_START : DPP_MCDS_TYPE_VALUE_EGRESS);

exit:
  SOCDNX_FUNC_RETURN;
}

/*
 * This function writes egress format 6/7 (CUD only with no link pointer) to a mcds mcdb entry and then to hardware.
 * The replications to write are specified by structure pointers, NULL pointers mean disabled replications.
 */
uint32 dpp_mult_eg_write_entry_cud_noptr(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN dpp_mc_id_t    multicast_id_ndx, /* mcdb to write to */
    SOC_SAND_IN dpp_rep_data_t *rep1,            /* replication 1 */
    SOC_SAND_IN dpp_rep_data_t *rep2,            /* replication 2 */
    SOC_SAND_IN dpp_rep_data_t *rep3,            /* replication 3 */
    SOC_SAND_IN uint8          use_next,         /* If zero, select format indicating end of linked list, otherwise */
                                                 /* select format indicating that the following entry is next. */
    SOC_SAND_IN uint32         prev_entry        /* the previous entry written only to mcds */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *mcdb_entry = dpp_mcds_get_mcdb_entry(unit, multicast_id_ndx);
  SOCDNX_INIT_FUNC_DEFS;

  mcds->egr_mc_write_entry_cud_noptr(unit, mcdb_entry, rep1, rep2, rep3, use_next); /* set the hardware fields */
  SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, multicast_id_ndx)); /* write to hardware */

  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, prev_entry); /* set software link to previous entry */
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, prev_entry == multicast_id_ndx ? DPP_MCDS_TYPE_VALUE_EGRESS_START : DPP_MCDS_TYPE_VALUE_EGRESS);

exit:
  SOCDNX_FUNC_RETURN;
}

/*
 * This function writes egress format 1 (bitmap+CUD) to a mcds mcdb entry and then to hardware.
 * The replication to write is specified by structure pointers, NULL pointers mean disabled replication.
 */
uint32 dpp_mult_eg_write_entry_bm_cud(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN dpp_mc_id_t    multicast_id_ndx, /* mcdb to write to */
    SOC_SAND_IN dpp_rep_data_t *rep,             /* the replication */
    SOC_SAND_IN uint32         next_entry,       /* the next entry */
    SOC_SAND_IN uint32         prev_entry        /* the previous entry written only to mcds */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *mcdb_entry = dpp_mcds_get_mcdb_entry(unit, multicast_id_ndx);
  SOCDNX_INIT_FUNC_DEFS;

  mcds->egr_mc_write_entry_bm_cud(unit, mcdb_entry, rep, next_entry); /* set the hardware fields */
  SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, multicast_id_ndx)); /* write to hardware */

  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, prev_entry); /* set software link to previous entry */
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, prev_entry == multicast_id_ndx ? DPP_MCDS_TYPE_VALUE_EGRESS_START : DPP_MCDS_TYPE_VALUE_EGRESS);

exit:
  SOCDNX_FUNC_RETURN;
}


/*
 * Creates a block of egress entries without pointers.
 * The next entry is always the next block entry implicitly by the format,
 * except for the last entry if last_entry_pointer==0.
 * Used to set the whole block if it is the last in the group,
 * or otherwise the whole block except for the last entry with the pointer.
 */
uint32 dpp_mult_eg_add_group_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  int                           core,               /* core of the linked list */
    SOC_SAND_IN  uint32                        block_start,        /* mcdb index of block start */
    SOC_SAND_IN  uint16                        port_offset,        /* The offset of the port_entries in port+outlif replications of the mcds */
    SOC_SAND_IN  uint16                        outlif_offset,      /* The offset of the outlif_entries in port+outlif replications of the mcds */
    SOC_SAND_IN  dpp_free_entries_block_size_t block_size,         /* number of entries to write in the block (excluding a possible last entry with an end of list format) */
    SOC_SAND_IN  dpp_free_entries_block_size_t last_entry_pointer, /* must be either: 0-last block entry ends group, 1-last block entry with pointer not handled by this function */
    SOC_SAND_IN  dpp_free_entries_block_size_t port_entries,       /* number of entries to create from remaining port+outlif replications */
    SOC_SAND_IN  dpp_free_entries_block_size_t outlif_entries,     /* number of entries to create from remaining outlif replications */
    SOC_SAND_IN  dpp_free_entries_block_size_t port_replications,  /* number of port+outlif replications (0-1) to add in one partially filled entry */
    SOC_SAND_IN  dpp_free_entries_block_size_t outlif_replications /* number of port+outlif replications (0-2) to add in one partially filled entry */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  uint32 last_entry = block_start + block_size + last_entry_pointer - 1; /* last entry of the block */
  uint32 index = block_start, prev = 0; /* current entry, previous entry in the block */
  dpp_free_entries_block_size_t left;
  dpp_rep_data_t *rep;
  SOCDNX_INIT_FUNC_DEFS;

  DPP_MC_ASSERT(
    mcds->nof_egr_port_outlif_reps[core] >= port_entries * 2 &&
    mcds->nof_egr_outlif_reps[core] >= outlif_entries * 3 &&
    block_size > 0 && port_replications <= 1 &&
    outlif_replications <= 2 && last_entry_pointer <= 1 &&
    port_entries + outlif_entries + port_replications + (outlif_replications ? 1 : 0) == block_size);
  DPP_MC_ASSERT(port_replications == 0 && outlif_replications == 0); /* not yet supported */
  DPP_MC_ASSERT(port_offset + port_entries * 2 <= mcds->nof_egr_port_outlif_reps[core] && 
    outlif_offset + outlif_entries  * 3 <= mcds->nof_egr_outlif_reps[core]);
  rep = mcds->egr_port_outlif_reps + port_offset;
  for (left = port_entries; left; --left, ++index) { /* add port+outlif entries */
    dpp_rep_data_t* rep1 = rep++;
    dpp_rep_data_t* rep2 = rep++;
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_port_cud_noptr(
      unit, index, rep1, rep2, last_entry - index, prev)); /* write to hardware */
    prev = index;
  }
  rep = mcds->egr_outlif_reps + outlif_offset;
  for (left = outlif_entries; left; --left, ++index) { /* add outlif entries */
    dpp_rep_data_t *rep1 = rep++;
    dpp_rep_data_t *rep2 = rep++;
    dpp_rep_data_t *rep3 = rep++;
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_cud_noptr( unit, index, rep1, rep2, rep3, last_entry - index, prev)); /* write to hardware */
    prev = index;
  }
  DPP_MC_ASSERT(index == block_start + block_size); /* verify we filled the given size */

exit:
  SOCDNX_FUNC_RETURN;
}

/*
 * This helper function writes a mcdb entry with a pointer,
 * according to preferred types and availability.
 */
uint32 dpp_mult_eg_write_pointer_entry(
    SOC_SAND_IN    int unit,
    SOC_SAND_IN    uint32 index,                    /* the mcdb index to write to */
    SOC_SAND_IN    uint32 prev_entry,               /* the mcdb index of the previous entry (to link to only in the mcds) */
    SOC_SAND_IN    uint32 next_entry,               /* the mcdb index of the next entry which will be the pointer in the entry */
    SOC_SAND_IN    uint16 remaining_ports_offset,   /* The offset of the nof_remaining_ports ports in the port+outlif replications of the mcds */
    SOC_SAND_INOUT uint16 *nof_port_couples,        /* The number of replication couples with the same outlif and with the first port < 127 moved to */
    SOC_SAND_INOUT uint16 *nof_remaining_ports,     /* The number of remaining port+outlif replications now at the end of the port+outlif replications */
    SOC_SAND_INOUT uint16 *nof_outlif_replications, /* The number of outlif only replications in the mcds */
    SOC_SAND_INOUT uint16 *nof_bitmap_replications  /* The number of bitmap+outlif only replications in the mcds */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  SOCDNX_INIT_FUNC_DEFS;

  /* select entry type by checking availability according to entry type precedence */
  if (*nof_port_couples) { /* use a port couple if available */
    --*nof_port_couples;
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_port_cud(unit, index,
      mcds->egr_port_outlif_reps + (2 * (*nof_port_couples) + 1),
      mcds->egr_port_outlif_reps + (2 * *nof_port_couples),
      next_entry, prev_entry));

  } else if (*nof_bitmap_replications) { /* use a bitmap replication if available */

    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_bm_cud(
      unit, index, mcds->egr_bitmap_reps + --*nof_bitmap_replications,
      next_entry, prev_entry));

  } else if (*nof_outlif_replications) { /* use outlif only replications if available */

    uint16 reps = *nof_outlif_replications >= 2 ? 2 : 1;
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_cud( unit, index,
      mcds->egr_outlif_reps + ((*nof_outlif_replications )- 1),
      reps == 2 ? mcds->egr_outlif_reps + ((*nof_outlif_replications) - 2) : 0,
      next_entry, prev_entry));
    (*nof_outlif_replications) -= reps;

  } else if (*nof_remaining_ports) { /* use a port+outlif replication if available */

    dpp_rep_data_t *port_outlif_ptr = mcds->egr_port_outlif_reps + (remaining_ports_offset + --*nof_remaining_ports);
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && DPP_MCDS_REP_DATA_GET_EGR_PORT(port_outlif_ptr) < ARAD_MULT_EGRESS_SMALL_PORT_INVALID) {
      /* store the replication in the 2nd port of the format */
      SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_port_cud(unit, index, 0, port_outlif_ptr, next_entry, prev_entry));
    } else { /* store the replication in the 1st port of the format */
      SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_port_cud(unit, index, port_outlif_ptr, 0, next_entry, prev_entry));
    }

  } else { /* It is a bug if code reached here, since an entry must be left both for this usage and for the start of the group */

    DPP_MC_ASSERT(0);
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("wrong function input")));

  }

exit:
  SOCDNX_FUNC_RETURN;
}


/*
 * This helper function writes a mcdb entry with a pointer to a given arad_mcdb_entry_t structure,
 * according to preferred types and availability.
 * This function is used to construct the first entry of an egress multicast group.
 * Since it does not write directly to the mcds, it does not (and can not) write the previous entry mcds link.
 */
uint32 dpp_mult_eg_write_pointer_entry_struct(
    SOC_SAND_IN    int unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *entry,        /* entry to write to */
    SOC_SAND_IN    uint32 next_entry,               /* the mcdb index of the next entry which will be the pointer in the entry */
    SOC_SAND_IN    uint16 remaining_ports_offset,   /* The offset of the nof_remaining_ports ports in the port+outlif replications of the mcds */
    SOC_SAND_INOUT uint16 *nof_port_couples,        /* The number of replication couples with the same outlif and with the first port < 127 moved to */
    SOC_SAND_INOUT uint16 *nof_remaining_ports,     /* The number of remaining port+outlif replications now at the end of the port+outlif replications */
    SOC_SAND_INOUT uint16 *nof_outlif_replications, /* The number of outlif only replications in the mcds */
    SOC_SAND_INOUT uint16 *nof_bitmap_replications  /* The number of bitmap+outlif only replications in the mcds */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  SOCDNX_INIT_FUNC_DEFS;

  /* select entry type by checking availability according to entry type precedence */
  if (*nof_port_couples) { /* use a port couple if available */

    --*nof_port_couples;
    mcds->egr_mc_write_entry_port_cud(unit, entry,
      mcds->egr_port_outlif_reps + (2 * (*nof_port_couples) + 1),
      mcds->egr_port_outlif_reps + (2 * *nof_port_couples),
      next_entry);

  } else if (*nof_bitmap_replications) { /* use a bitmap replication if available */

    mcds->egr_mc_write_entry_bm_cud(unit, entry,
      mcds->egr_bitmap_reps + --*nof_bitmap_replications, next_entry);

  } else if (*nof_remaining_ports == 2 && *nof_outlif_replications == 0)  {

    /* If all the replications are two port+outlif, they fit in one entry without a pointer */
    DPP_MC_ASSERT(!remaining_ports_offset);
    mcds->egr_mc_write_entry_port_cud_noptr(unit, entry,
      mcds->egr_port_outlif_reps,
      mcds->egr_port_outlif_reps + 1, 0);
    *nof_remaining_ports = 0;

  } else if (*nof_outlif_replications == 3 && *nof_remaining_ports == 0)  {

    /* If all the replications are three outlif only, they fit in one entry without a pointer */
    mcds->egr_mc_write_entry_cud_noptr(unit, entry,
      mcds->egr_outlif_reps,
      mcds->egr_outlif_reps + 1,
      mcds->egr_outlif_reps + 2, 0);
    (*nof_outlif_replications) = 0;

  } else if (*nof_outlif_replications) { /* use outlif only replications if available */

    uint16 reps = *nof_outlif_replications >= 2 ? 2 : 1;
    mcds->egr_mc_write_entry_cud(unit, entry,
      mcds->egr_outlif_reps + ((*nof_outlif_replications )- 1),
      reps == 2 ? mcds->egr_outlif_reps + ((*nof_outlif_replications) - 2) : 0,
      next_entry);
    (*nof_outlif_replications) -= reps;

  } else if (*nof_remaining_ports) { /* use a port+outlif replication if available */

    dpp_rep_data_t *port_outlif_ptr = mcds->egr_port_outlif_reps + (remaining_ports_offset + --*nof_remaining_ports);
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && DPP_MCDS_REP_DATA_GET_EGR_PORT(port_outlif_ptr) < ARAD_MULT_EGRESS_SMALL_PORT_INVALID) {
      /* store the replication in the 2nd port of the format */
      mcds->egr_mc_write_entry_port_cud(unit, entry, 0, port_outlif_ptr, next_entry);
    } else { /* store the replication in the 1st port of the format */
      mcds->egr_mc_write_entry_port_cud(unit, entry, port_outlif_ptr, 0, next_entry);
    }

  } else { /* It is a bug if code reached here, since an entry must be left both for this usage and for the start of the group */

    DPP_MC_ASSERT(0);
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("wrong function input")));

  }

exit:
  SOCDNX_FUNC_RETURN;
}


/*
 * Set a linked list of the input egress entries, possibly using a provided free block as the first allocation.
 * The replications are taken form the mcds.
 * If is_group_start is non zero, then list_prev is the (free and reserved) group start entry and it is set with replications.
 * Otherwise we do not handle the start of the egress group so there is no need for special handling of the first entry.
 * If the function fails, it will free the given allocated block.
 * In the start_block_index entry, link to the previous entry according to the previous entry of input_block_index.
 * On failure allocated entries are freed, including alloced_block_start.
 * Linked lists replaced by new linked lists are not freed by this function.
 */

uint32 dpp_mcds_set_egress_linked_list(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint8                         is_group_start,      /* specifies if list_prev is a group start to be filled or not */
    SOC_SAND_IN  uint32                        group_id,            /* If is_group_start this is the group ID, otherwise this is the entry preceding the single linked list to be created */
    SOC_SAND_IN  uint32                        list_end,            /* The entry that end of the created linked list will point to */
    SOC_SAND_IN  uint32                        alloced_block_start, /* start index of an allocated block to use for the free list */
    SOC_SAND_IN  dpp_free_entries_block_size_t alloced_block_size,  /* size of the allocated block to use, should be 0 if none */
    SOC_SAND_IN  dpp_mc_core_bitmap_t          cores_to_set,        /* cores of linked lists to set */
    SOC_SAND_OUT uint32                        *list_start,         /* The first entry of the created linked list */
    SOC_SAND_OUT SOC_TMC_ERROR                 *out_err             /* return possible errors that the caller may want to ignore: insufficient memory or duplicate replications */
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    int failed = 1;
    uint32 linked_lists[SOC_DPP_DEFS_MAX(NOF_CORES)] = {0}, linked_list_ends[SOC_DPP_DEFS_MAX(NOF_CORES)] = {0}; /* the linked lists created for each core */
    uint32 block_start = alloced_block_start;
    uint32 list_prev; /* The entry preceding the linked list to be created */
    dpp_free_entries_block_size_t block_size = alloced_block_size;
    uint16 pointer_entry = list_end == DPP_MC_EGRESS_LINK_PTR_END ? 0 : 1; /* number of pointer entries needed at the end of the block, must be 1 or 0 */
    arad_mcdb_entry_t start_entries[SOC_DPP_DEFS_MAX(NOF_CORES)];
    dpp_mc_core_bitmap_t core_bm;

    uint16 nof_port_couples;        /* The number of replication couples with the same outlif and with the first port < 127 moved to */
    uint16 nof_remaining_ports;     /* The number of remaining port+outlif replications now at the end of the port+outlif replications */
    uint16 nof_outlif_replications; /* The number of outlif only replications in the mcds */
    uint16 nof_bitmap_replications; /* The number of bitmap+outlif only replications in the mcds */
    uint16 remaining_ports_offset;
    uint32 core;

    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) { /* init for cleanup on error */
        linked_lists[core] = list_end;
    }
    SOCDNX_NULL_CHECK(out_err);
    DPP_MC_ASSERT(core <= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores);

    /* create the new linked lists, one per core */
    DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) {
        /* init the linked list created so far to empty */
        linked_list_ends[core] = list_prev = is_group_start ? DPP_MCDS_GET_EGRESS_GROUP_START(mcds, group_id, core) : group_id;

        SOCDNX_IF_ERR_EXIT(dpp_mult_process_egress_replications( /* process the input replications */
          unit, core, &nof_port_couples, &nof_remaining_ports, &nof_outlif_replications, &nof_bitmap_replications, out_err));
        if (*out_err) {
            SOC_EXIT; /* if duplicate replications were found, exit */
        }
        remaining_ports_offset = nof_port_couples * 2;

        if (is_group_start) { /* build first group entry for writing after building all the linked list */
            if (nof_port_couples + nof_remaining_ports + nof_outlif_replications + nof_bitmap_replications == 0) { /* handle empty group */
                start_entries[core].word0 = mcds->free_value[0];
                start_entries[core].word1 = mcds->free_value[1];
            } else { /* set first group entry for a non empty group */
                start_entries[core].word1 = start_entries[core].word0 = 0;
                SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_pointer_entry_struct(
                  unit, start_entries + core, DPP_MC_EGRESS_LINK_PTR_END, remaining_ports_offset,
                  &nof_port_couples, &nof_remaining_ports, &nof_outlif_replications, &nof_bitmap_replications));
            }
        }
        
        /* estimate the minimum number of entries needed for the group, will likely be lower than the number needed, but not higher */
        if (nof_port_couples + (nof_remaining_ports + 1) / 2 + (nof_outlif_replications + 2) / 3 + nof_bitmap_replications >
            dpp_mcds_unoccupied_get(mcds) + block_size) { /* check if we have enough free entries to attempt the operation (not exact) */
            *out_err = _SHR_E_FULL;
            SOC_EXIT;
        }

        /* build the linked list, loop while we have remaining replications not written */
        while (nof_port_couples + nof_remaining_ports + nof_outlif_replications + nof_bitmap_replications) {
            uint16 port_outlif_full_entries = nof_remaining_ports / 2; /* number of port+outlif entries that can be created with no pointers */
            uint16 outlif_full_entries = nof_outlif_replications / 3; /* number of outlif entries that can be created with no pointers */
            uint16 net_block_size = port_outlif_full_entries + outlif_full_entries ; /* the size we want for our block without a pointer entry */
            uint16 full_block_size; /* the block size including the terminating pointer entry */
            uint32 prev_entry = list_prev, index = -1;

            if (!net_block_size) { /* if we have no entry suited for a block, we always need a pointer entry */
                pointer_entry = 1;
            } else if (net_block_size && pointer_entry && /* we have replications suitable for a block terminated by a pointer entry (not at the group end) */
                nof_port_couples + nof_bitmap_replications == 0 &&                /* If we have no available pointer entries to create, */
                (nof_remaining_ports % 2) + (nof_outlif_replications % 3) == 0) { /* and no reminder replications from full non pointer entries */
                /* Try to "convert" entries that can be non pointer to pointer */
                --net_block_size; 
                if (outlif_full_entries) {
                    --outlif_full_entries;
                } else {
                    DPP_MC_ASSERT(port_outlif_full_entries);
                    --port_outlif_full_entries;
                }
            }
            full_block_size = net_block_size + pointer_entry;
            if (full_block_size >= DPP_MCDS_MAX_FREE_BLOCK_SIZE) {
                full_block_size = DPP_MCDS_MAX_FREE_BLOCK_SIZE;
                net_block_size = DPP_MCDS_MAX_FREE_BLOCK_SIZE - pointer_entry;
            }
            DPP_MC_ASSERT(full_block_size && full_block_size <= DPP_MCDS_MAX_FREE_BLOCK_SIZE &&
                             (net_block_size || full_block_size == 1));

            if (!block_size) { /* no available entries, we need to allocate */
                SOCDNX_IF_ERR_EXIT(dpp_mcds_get_free_entries_block( /* allocate free entries */
                  mcds, DPP_MCDS_GET_FREE_BLOCKS_PREFER_INGRESS | DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL,
                  full_block_size, full_block_size, &block_start, &block_size));
                if (!block_size) { /* could not get free entries */
                    *out_err = _SHR_E_FULL;
                    SOC_EXIT; /* will free the linked list allocated so far */
                }
                DPP_MC_ASSERT(block_size <= full_block_size);
            }

            net_block_size = block_size - pointer_entry;
            if (net_block_size) { /* create a block */
                uint16 block_port_outlif_entries, block_outlif_entries; /* number of entries actually used in the block */
                full_block_size = block_size;

                if (net_block_size < port_outlif_full_entries) { /* calculate how many entries of each type are included in the block */
                    block_port_outlif_entries = net_block_size;
                    block_outlif_entries = 0;
                } else {
                    block_port_outlif_entries = port_outlif_full_entries;
                    block_outlif_entries = net_block_size - port_outlif_full_entries;
                  
                }
                DPP_MC_ASSERT(block_port_outlif_entries +  block_outlif_entries + pointer_entry == block_size &&
                              port_outlif_full_entries >= block_port_outlif_entries &&
                              outlif_full_entries >= block_outlif_entries &&
                              nof_remaining_ports >= block_port_outlif_entries * 2 &&
                              nof_outlif_replications >= block_outlif_entries * 3);
                nof_remaining_ports -= block_port_outlif_entries * 2; /* update remaining replication counts */
                nof_outlif_replications -= block_outlif_entries * 3;
            
                SOCDNX_IF_ERR_EXIT(dpp_mult_eg_add_group_block( /* write the block, excluding the last entry if it is a pointer */
                  unit, core, block_start, remaining_ports_offset + nof_remaining_ports, nof_outlif_replications, net_block_size,
                  pointer_entry, block_port_outlif_entries, block_outlif_entries, 0, 0));

                DPP_MC_ASSERT(port_outlif_full_entries >= block_port_outlif_entries && outlif_full_entries >= block_outlif_entries);
                port_outlif_full_entries -= block_port_outlif_entries;
                outlif_full_entries -= block_outlif_entries;

                index = block_start + net_block_size;
                prev_entry = index - 1;
                /* finished creating the block non pointer entries */
            } else {
                full_block_size = 1;
                index = block_start;
                pointer_entry = 1;
            }

            if (pointer_entry) { /* This is not a block or a block ending in a pointer entry which we need to write */
                SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_pointer_entry( /* write entry index with a pointer to linked_list */
                  unit, index, prev_entry, linked_lists[core], remaining_ports_offset,
                  &nof_port_couples, &nof_remaining_ports, &nof_outlif_replications, &nof_bitmap_replications));
            } else {
                pointer_entry = 1;
                DPP_MC_ASSERT(linked_lists[core] == list_end);
            }

            DPP_MC_ASSERT(block_size >= full_block_size);
            if (linked_lists[core] != list_end) { /* set software back link from the previously generated linked list (in previous iterations) to the last entry in this block */
                DPP_MCDS_SET_PREV_ENTRY(mcds, linked_lists[core], block_start + full_block_size - 1);
            } else {
                linked_list_ends[core] = block_start + full_block_size - 1;
            }

            linked_lists[core] = block_start; /* Mark the current block/entry as the start of the linked list */
            if (block_size -= full_block_size) { /* update the part of the current block that we used */
                block_start += full_block_size;
            }

        } /* finished creating the linked list */
        DPP_MC_ASSERT(nof_port_couples + nof_remaining_ports + nof_outlif_replications + nof_bitmap_replications == 0 && !block_size);
    }

    /* If we are successful so far, and allocated everything, finalize the group by writing one entry per linked list/core */
    list_prev = group_id;
    DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) {
        /* now connect the written linked list to its preceding and succeeding (if any) entries */
        if (is_group_start) { /* build first group entry for writing after building all the linked list */
            arad_mcdb_entry_t *mcdb_entry;
            list_prev = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, group_id, core);
            mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, list_prev);
            mcdb_entry->word0 = start_entries[core].word0; /* copy previously filled hardware fields (next pinter not set) */
            mcdb_entry->word1 &= DPP_MCDS_WORD1_KEEP_BITS_MASK;
            mcdb_entry->word1 |= start_entries[core].word1;

            DPP_MCDS_SET_PREV_ENTRY(mcds, list_prev, list_prev); /* set software link to previous entry */
            DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_EGRESS_START); /* set type */
        }
        if (linked_lists[core] != list_end) { /* set software back link from the start of the created linked list to the previous entry */
            DPP_MCDS_SET_PREV_ENTRY(mcds, linked_lists[core], list_prev);
        }
        SOCDNX_IF_ERR_EXIT(MCDS_SET_NEXT_POINTER( /* set next pointer and write to hardware the entry preceding the linked list */
          mcds, unit, list_prev, DPP_MCDS_TYPE_VALUE_EGRESS_START, linked_lists[core]));
        linked_lists[core] = list_end; /* prevent the freeing of the linked list on error */

        if (list_end != DPP_MC_EGRESS_LINK_PTR_END) { /* if group continues, link the continuation to the created linked list */
            DPP_MCDS_SET_PREV_ENTRY(mcds, list_end, linked_list_ends[core]);
        }
    }

    failed = 0;
exit:
    if (failed) { /* handle cleanup of allocated entries that were not used due to function failure */
        DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) {
            if (linked_lists[core] != list_end) { /* free the linked list allocated so far on error or when running out of mcdb entries */
                  dpp_mcdb_free_linked_list_till_my_end(unit, linked_lists[core], DPP_MCDS_TYPE_VALUE_EGRESS, list_end);
            }
        }
        if (block_size) { /* free the current block on error */
            DPP_MC_ASSERT(failed || (block_start >= alloced_block_start &&  block_start < alloced_block_start + alloced_block_size));
            dpp_mcds_build_free_blocks(unit, mcds,
              block_start, block_start + block_size - 1, dpp_mcds_get_region(mcds, block_start),
              block_start == alloced_block_start ? McdsFreeBuildBlocksAddAll : McdsFreeBuildBlocksAdd_AllMustBeUsed);
        }
    }
    SOCDNX_FUNC_RETURN;
}

/*
 * This API sets the egress group to the given replications,
 * configuring its linked list.
 * If the group does not exist, it will be created or an error will be returned based on allow_create.
 * Creation may involve relocating the mcdb entry which will be the start
 * of the group, and possibly other consecutive entries.
 *
 * We always want to create entries with pointers from port+outlif couples and from bitmaps.
 * We need to leave one entry with a pointer for the start of the group.
 * every block of entries with no pointers ends with an entry pointer, except for the end of the group.
 */
uint32 dpp_mult_eg_group_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  dpp_mc_id_t   mcid,         /* the group mcid */
    SOC_SAND_IN  uint8         allow_create, /* if non zero, will create the group if it does not exist */
    SOC_SAND_IN  uint32        group_size,   /* size of ports and cuds to read group replication data from */
    SOC_SAND_IN  uint32        *ports,       /* array containing logical ports (destinations) */
    SOC_SAND_IN  soc_if_t      *cuds,        /* array containing encapsulations/CUDs/outlifs */
    SOC_SAND_OUT SOC_TMC_ERROR *out_err      /* return possible errors that the caller may want to ignore */
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    int group_exists = 0, failed = 1, core;
    uint32 entry_type, created_list, group_entry_id;
    uint32 old_group_entries[SOC_DPP_DEFS_MAX(NOF_CORES)] = {DPP_MC_EGRESS_LINK_PTR_END}; /* the linked lists (per core) of the previous group content */
    const dpp_mc_core_bitmap_t cores_to_set = DPP_MC_CORE_BITAMAP_ALL_ACTIVE_CORES(unit);
    dpp_mc_core_bitmap_t core_bm, group_start_alloced_bm = DPP_MC_CORE_BITAMAP_NO_CORES;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(out_err);


    DPP_MC_ASSERT(mcid < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
    group_exists = dpp_egress_group_open_get(unit, mcid);
    if (!group_exists && !allow_create) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
    }
    DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) { /* For each core handle its existing linked list or its first linked list entries */
        group_entry_id = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, mcid, core);
        entry_type = DPP_MCDS_GET_TYPE(mcds, group_entry_id);
        DPP_MC_ASSERT(group_exists == DPP_MCDS_TYPE_IS_EGRESS_START(entry_type));

        /* If this is a new group, we need to reserve its first entry, and possibly relocate the entry there */
        if (!group_exists && group_entry_id != MCDS_INGRESS_LINK_END(mcds)) {
            if (DPP_MCDS_TYPE_IS_USED(entry_type)) { /* relocate conflicting entries if needed */
                SOCDNX_IF_ERR_EXIT(dpp_mcdb_relocate_entries(unit, group_entry_id, 0, 0, out_err));
                if (*out_err) { /* If failed due to lack of memories, do the same */
                    SOC_EXIT;
                }
            } else { /* just allocate the start entry of the group */
                SOCDNX_IF_ERR_EXIT(dpp_mcds_reserve_group_start(mcds, group_entry_id));
            }
            group_start_alloced_bm |= DPP_MC_CORE_BITAMAP_CORE_0 << core;
        }

        if (group_exists) { /* for an existing group store its linked list (to be freed) at old_group_entries */
            SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds, unit, group_entry_id, DPP_MCDS_TYPE_VALUE_EGRESS, old_group_entries + core));
        }
    }

    SOCDNX_IF_ERR_EXIT(dpp_mcds_copy_replications_from_arrays(
      unit, 1, 1, group_size, ports, cuds)); /* copy group replications to mcds */

    SOCDNX_IF_ERR_EXIT(dpp_mcds_set_egress_linked_list( /* build the group, including the first entry */
      unit, TRUE, mcid, DPP_MC_EGRESS_LINK_PTR_END, 0, 0, cores_to_set, &created_list, out_err));
    if (*out_err) { /* If failed due to lack of memories, do the same */
        SOC_EXIT;
    }

    if (group_exists) { /* for an existing group free its previous linked lists */
        DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) {
            if (old_group_entries[core] != DPP_MC_EGRESS_LINK_PTR_END) { /* free previous linked list of the group not used any more */
                SOCDNX_IF_ERR_EXIT(dpp_mcdb_free_linked_list(unit, old_group_entries[core], DPP_MCDS_TYPE_VALUE_EGRESS));
            }
        }
    } else {
        SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_set(unit, mcid, 1));
    }

    failed = 0;
exit:
    if (group_start_alloced_bm && failed) { /* free linked list start entries if needed */
        DPP_MC_ASSERT(!group_exists);
        DPP_MC_FOREACH_CORE(group_start_alloced_bm, core_bm, core) {
            group_entry_id = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, mcid, core);
            dpp_mcds_build_free_blocks(unit, mcds, group_entry_id, group_entry_id,
              dpp_mcds_get_region(mcds, group_entry_id), McdsFreeBuildBlocksAdd_AllMustBeUsed);
        }
    }
    SOCDNX_FUNC_RETURN;
}


/*********************************************************************
*     This procedure configures the range of values of the
*     multicast ids entry points that their multicast groups
*     are to be found according to a bitmap (as opposed to a
*     Link List). Only the max bitmap ID is configurable.
*********************************************************************/
uint32 dpp_mult_eg_bitmap_group_range_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
)
{
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);
  if (info->mc_id_high >= DPP_MC_EGR_NOF_BITMAPS || info->mc_id_low)
  {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid bitmap range")));
  }

  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, SOC_CORE_ALL, 0, EGRESS_REP_BITMAP_GROUP_VALUE_TOPf,  info->mc_id_high));

exit:
  SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     This procedure configures the range of values of the
*     multicast ids entry points that their multicast groups
*     are to be found according to a bitmap (as opposed to a
*     Link List).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  dpp_mult_eg_bitmap_group_range_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
)
{
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(info);

  /* Always zero (like Petra-B), no register to set */
  info->mc_id_low = 0;

  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, SOC_CORE_ALL, 0, EGRESS_REP_BITMAP_GROUP_VALUE_TOPf, &(info->mc_id_high)));

exit:
  SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     This API closes egress-multicast-replication group for
*     the specific multicast-id. The user only specifies the
*     multicast-id. All inner link-list/bitmap nodes are freed
*     and handled by the driver
*********************************************************************/
uint32 dpp_mult_eg_group_close(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t mcid
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    uint32 group_entry_id, core;
    const dpp_mc_core_bitmap_t cores_to_set = DPP_MC_CORE_BITAMAP_ALL_ACTIVE_CORES(unit);
    dpp_mc_core_bitmap_t core_bm;
    SOC_TMC_ERROR internal_err;
    SOCDNX_INIT_FUNC_DEFS;

    if (dpp_mult_does_group_exist(unit, mcid, TRUE)) { /* do nothing if not open */
        if (mcid <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high) { /* this is a vlan bitmap group */

            SOCDNX_IF_ERR_EXIT(dpp_mult_eg_bitmap_group_close(unit, mcid));

        } else { /* not a bitmap group */

            SOCDNX_IF_ERR_EXIT(dpp_mult_eg_group_set( /* empty the group */
              unit, mcid, FALSE, 0, 0, 0, &internal_err));
            if (internal_err) { /* should never require more entries, so if this happens it is an internal error */
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("too many entries")));
            }

            DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) { /* For each core handle its existing linked list or its first linked list entries */
                group_entry_id = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, mcid, core);
                if (group_entry_id != MCDS_INGRESS_LINK_END(mcds)) { /* free the group's starting entry, this also marks the group as closed */
                    SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, group_entry_id, group_entry_id,
                      dpp_mcds_get_region(mcds, group_entry_id), McdsFreeBuildBlocksAdd_AllMustBeUsed));
                } else { /* treat the non allocable last entry specifically */
                    arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, MCDS_INGRESS_LINK_END(mcds));
                    mcdb_entry->word0 = mcds->free_value[0];
                    mcdb_entry->word1 &= DPP_MCDS_WORD1_KEEP_BITS_MASK;
                    mcdb_entry->word1 |= mcds->free_value[1];
                    SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, MCDS_INGRESS_LINK_END(mcds))); /* write to hardware */
                }
            }

            SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_set(unit, mcid, 0));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Adds the given replication to the non bitmap egress multicast group.
 * It is an error if the group is not open.
 */
uint32 dpp_mult_eg_port_add(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  dpp_mc_id_t           group_mcid,          /* group mcid */
    SOC_SAND_IN  SOC_TMC_MULT_EG_ENTRY *replication,        /* replication to add */
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err             /* return possible errors that the caller may want to ignore */
)
{
    uint16 group_size;
    uint32 linked_list_start, created_list;
    uint32 old_group_entries[SOC_DPP_DEFS_MAX(NOF_CORES)] = {DPP_MC_EGRESS_LINK_PTR_END}; /* the linked lists (per core) of the previous group content */
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    int core;
    dpp_mc_core_bitmap_t changed_cores = DPP_MC_CORE_BITAMAP_NO_CORES, core_bm;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(replication);
    SOCDNX_NULL_CHECK(out_err);
    if (replication->type != SOC_TMC_MULT_EG_ENTRY_TYPE_OFP) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unsupported type")));
    }

    if (!dpp_egress_group_open_get(unit, group_mcid)) { /* group is not open */
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
    }

    { /* clear the replications in mcds and add the new replication */
        soc_if_t cud = replication->cud;
        uint32 port = replication->port;
        SOCDNX_IF_ERR_EXIT(dpp_mcds_copy_replications_from_arrays( unit, TRUE, TRUE, 1,  &port, &cud));
    }
    /* set changed_cores to contain cores for which a replication is added, and store current linked lists of these cores */
    for (core = 0; core < SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; ++core) {
        if (mcds->nof_reps[core] > 0) {
            changed_cores |= DPP_MC_CORE_BITAMAP_CORE_0 << core;
        }

        linked_list_start = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, group_mcid, core);
        DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, linked_list_start) == DPP_MCDS_TYPE_VALUE_EGRESS_START);
        SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER( /* store previous linked lists to be freed */
          mcds, unit, linked_list_start, DPP_MCDS_TYPE_VALUE_EGRESS, old_group_entries + core));
    }

    /* get the replications of the current group into the mcds */
    SOCDNX_IF_ERR_EXIT(dpp_mcds_get_group( unit, changed_cores, FALSE, group_mcid,
      DPP_MCDS_TYPE_VALUE_EGRESS_START, DPP_MULT_MAX_EGRESS_REPLICATIONS - 1, &group_size));
    if (group_size >= DPP_MULT_MAX_EGRESS_REPLICATIONS - 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many replications")));
    }

    /* recreate the group with the extra replication by building the linked lists, including the first entry */
    SOCDNX_IF_ERR_EXIT(dpp_mcds_set_egress_linked_list(unit, TRUE, group_mcid,
      DPP_MC_EGRESS_LINK_PTR_END, 0, 0, changed_cores, &created_list, out_err));
    if (*out_err) { /* If failed, do some error code fixing */
        if (*out_err == _SHR_E_PARAM) {
            *out_err = _SHR_E_EXISTS;
        }
    } else { /* If succeeded, free previous linked lists that are not used any more */
        DPP_MC_FOREACH_CORE(changed_cores, core_bm, core) {
            if (old_group_entries[core] != DPP_MC_EGRESS_LINK_PTR_END) {
                SOCDNX_IF_ERR_EXIT(dpp_mcdb_free_linked_list( unit, old_group_entries[core], DPP_MCDS_TYPE_VALUE_EGRESS));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Removes the given replication from the non bitmap egress multicast group.
 * It is an error if the group is not open or does not contain the replication.
 */
uint32 dpp_mult_eg_port_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  dpp_mc_id_t           group_mcid,   /* group mcid */
    SOC_SAND_IN  SOC_TMC_MULT_EG_ENTRY *replication, /* replication to remove */
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err      /* return possible errors that the caller may want to ignore */
)
{
  uint16 group_size;
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  soc_if_t cud;
  uint32 port;
  int core;
  dpp_mc_core_bitmap_t changed_cores = DPP_MC_CORE_BITAMAP_NO_CORES;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(replication);
  if (replication->type != SOC_TMC_MULT_EG_ENTRY_TYPE_OFP) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unsupported")));
  }


  if (!dpp_egress_group_open_get(unit, group_mcid)) { /* group is not open */
    SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
  }
  cud = replication->cud;
  port = replication->port;

  /* Set changed_cores to contain the cores of the replication to be removed */
  SOCDNX_IF_ERR_EXIT(dpp_mcds_copy_replications_from_arrays(unit, TRUE, TRUE, 1, &port, &cud)); /* Clear the replications in mcds and add the replication */
  for (core = 0; core < SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; ++core) {
    if (mcds->nof_reps[core] > 0) {
      changed_cores |= DPP_MC_CORE_BITAMAP_CORE_0 << core;
    }
  }

  /* get the replications of the relevant cores of the group into the mcds */
  SOCDNX_IF_ERR_EXIT(dpp_mcds_get_group(unit, changed_cores, TRUE, group_mcid,
    DPP_MCDS_TYPE_VALUE_EGRESS_START, DPP_MULT_MAX_EGRESS_REPLICATIONS - 1, &group_size));
  if (group_size >= DPP_MULT_MAX_EGRESS_REPLICATIONS - 1) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many replications")));
  }

  /* remove the given replication from the mcds */
  SOCDNX_IF_ERR_EXIT(dpp_mult_remove_replication(unit, DPP_MCDS_TYPE_VALUE_EGRESS_START, port, cud, out_err, &changed_cores));
  if (*out_err) { /* If the replication was not found, exit */
    SOC_EXIT;
  }

  /* recreate the group with the extra replication */
  {
    uint32 old_group_entries[SOC_DPP_DEFS_MAX(NOF_CORES)], created_list;
    dpp_mc_core_bitmap_t core_bm;
    DPP_MC_FOREACH_CORE(changed_cores, core_bm, core) {
      SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER( /* store previous linked list to be freed */
        mcds, unit, DPP_MCDS_GET_EGRESS_GROUP_START(mcds, group_mcid, core), DPP_MCDS_TYPE_VALUE_EGRESS, old_group_entries + core));
    }

    SOCDNX_IF_ERR_EXIT(dpp_mcds_set_egress_linked_list( /* build the group, including the first entry */
      unit, TRUE, group_mcid, DPP_MC_EGRESS_LINK_PTR_END, 0, 0, changed_cores, &created_list, out_err));
    if (*out_err) { /* If failed due to lack of free entries */
#ifndef ARAD_EGRESS_MC_DELETE_FAILS_ON_FULL_MCDB
      soc_error_t ret1, ret2 = SOC_E_NONE;
      /* If we can not reconstruct the group due to MCDB being full, just remove the replication/entry */
      DPP_MC_FOREACH_CORE(changed_cores, core_bm, core) {
        ret1 = dpp_mcds_remove_replications_from_egress_group(unit, DPP_MCDS_GET_EGRESS_GROUP_START(mcds, group_mcid, core), port, cud);
        if (ret2 == SOC_E_NONE) {
          ret2 = ret1;
        }
      }
      if (ret2 != SOC_E_NONE) {
        SOCDNX_EXIT_WITH_ERR(ret2, (_BSL_SOCDNX_MSG("failed to delete egress replication")));
      }
      *out_err = _SHR_E_NONE;
#endif /* ARAD_EGRESS_MC_DELETE_FAILS_ON_FULL_MCDB */
    } else { /* free previous linked list of the group not used any more */
      DPP_MC_FOREACH_CORE(changed_cores, core_bm, core) {
        if (old_group_entries[core] != DPP_MC_EGRESS_LINK_PTR_END) {
          SOCDNX_IF_ERR_EXIT(dpp_mcdb_free_linked_list(unit, old_group_entries[core], DPP_MCDS_TYPE_VALUE_EGRESS));
        }
      }
    }
  }

exit:
  SOCDNX_FUNC_RETURN;
}


/*
 * Returns the size of the multicast group with the specified multicast id.
 * Not needed for bcm APIs, so not tested.
 * returns 0 for non open groups.
 */
uint32 dpp_mult_eg_group_size_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  dpp_mc_id_t  multicast_id_ndx,
    SOC_SAND_OUT uint32       *mc_group_size
)
{
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(mc_group_size);


  if (multicast_id_ndx <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high) { /* this is a vlan bitmap group */

    *mc_group_size = 0;
    if (dpp_mult_does_group_exist(unit, multicast_id_ndx, TRUE)) { /* group open */
      dpp_mc_egr_bitmap_t bitmap;
      uint32 bits, *bits_ptr = bitmap;
      unsigned bits_left, words_left = (SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32) * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;

      SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_get(unit, multicast_id_ndx, bitmap));
      for (; words_left; --words_left) {
        bits = *(bits_ptr++);
        for (bits_left = SOC_SAND_NOF_BITS_IN_UINT32; bits_left; --bits_left) {
          *mc_group_size += (bits & 1);
          bits >>= 1;
        }
      }
    }

  } else { /* not a bitmap */

    uint8 is_open;
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_get_group(
      unit, multicast_id_ndx, 0, 0, 0, mc_group_size, &is_open));

  }

exit:
  SOCDNX_FUNC_RETURN;
}


/*
 * This function creates or destroys a bitmap group, Setting the bitmap to not replicate (0).
 */
uint32 dpp_mult_eg_bitmap_group_zero(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t multicast_id_ndx, /* group mcid */
    SOC_SAND_IN  uint8       create            /* should be 1 for create, and 0 for destroy */
)
{
    uint32 data[DPP_EGQ_VLAN_TABLE_TBL_ENTRY_SIZE] = {0};

    SOCDNX_INIT_FUNC_DEFS;
    if (multicast_id_ndx > SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("MCID is not a bitmap group")));
    }

    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_VLAN_TABLEm(unit, EGQ_BLOCK(unit, SOC_CORE_ALL), multicast_id_ndx, data)); /* mark bitmap as containing no ports */
    SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_set(unit, multicast_id_ndx, create)); /* mark the group as open */

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * This function opens a bitmap group, and sets it to replicate to the given ports.
 */
uint32 dpp_mult_eg_bitmap_group_create(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t multicast_id_ndx /* group mcid */
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_bitmap_group_zero(unit, multicast_id_ndx, 1));
exit:
    SOCDNX_FUNC_RETURN;
}


/*********************************************************************
*     Set the bitmap of the given egress bitmap group to the given bitmap.
*     The bitmap is of TM ports (and not of local ports).
*********************************************************************/
uint32 dpp_mult_eg_bitmap_group_update(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  dpp_mc_id_t                           multicast_id_ndx,
    SOC_SAND_IN  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group /* TM port bitmap to set */
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(group);
    if (multicast_id_ndx >= DPP_MC_EGR_NOF_BITMAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ID is too high for a multicast bitmap")));
    }

    SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_set(unit, multicast_id_ndx, &group->bitmap[0]));
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * This function closed a bitmap group, clearing its hardware replications.
 */
uint32 dpp_mult_eg_bitmap_group_close(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  dpp_mc_id_t  multicast_id_ndx
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_bitmap_group_zero(unit, multicast_id_ndx, 0));
exit:
    SOCDNX_FUNC_RETURN;
}


/*********************************************************************
*   Get the index inside a bitmap from a local port.
*   The bitmap index is the index in the bitmap depending on the core ID and TM port.
*********************************************************************/
uint32 dpp_mult_eg_bitmap_local_port_2_bitmap_bit_index(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID port,          /* local port */
    SOC_SAND_OUT uint32              *out_bit_index /* returned bit index */
)
{
    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_IS_JERICHO(unit)) {
        uint32 tm_port;
        int core;
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
        *out_bit_index = tm_port + SOC_TMC_NOF_FAP_PORTS_PER_CORE * core;
    } else {
        *out_bit_index = port;
    }
exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Add port members of the Egress-Multicast and/or modify
*     the number of logical copies required at port.
*********************************************************************/
uint32 dpp_mult_eg_bitmap_group_port_add(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  dpp_mc_id_t         multicast_id_ndx,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID port,         /* local port to add */
    SOC_SAND_OUT SOC_TMC_ERROR       *out_err      /* return possible errors that the caller may want to ignore */
)
{
    uint32 bit_index, word_index, bit_mask;
    dpp_mc_egr_bitmap_t bitmap = {0};

    SOCDNX_INIT_FUNC_DEFS;

    if (multicast_id_ndx >= DPP_MC_EGR_NOF_BITMAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ID is too high for a multicast bitmap")));
    }
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_bitmap_local_port_2_bitmap_bit_index(unit, port, &bit_index));
    word_index = bit_index / SOC_SAND_NOF_BITS_IN_UINT32; 
    bit_mask = (uint32)1 << (bit_index & (SOC_SAND_NOF_BITS_IN_UINT32 - 1));

    SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_get(unit, multicast_id_ndx, bitmap));

    if (bitmap[word_index] & bit_mask) {
        *out_err = _SHR_E_EXISTS; /* port is already replicated to */
    } else {
        bitmap[word_index] |= bit_mask;
        SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_set(unit, multicast_id_ndx, bitmap));
        *out_err = _SHR_E_NONE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/*********************************************************************
*     Removes a port member of the egress multicast.
*********************************************************************/
uint32 dpp_mult_eg_bitmap_group_port_remove(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  dpp_mc_id_t         multicast_id_ndx,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID port,         /* local port to remove */
    SOC_SAND_OUT SOC_TMC_ERROR       *out_err      /* return possible errors that the caller may want to ignore */
)
{
    uint32 bit_index, word_index, bit_mask;
    dpp_mc_egr_bitmap_t bitmap = {0};

    SOCDNX_INIT_FUNC_DEFS;

    if (multicast_id_ndx >= DPP_MC_EGR_NOF_BITMAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ID is too high for a multicast bitmap")));
    }
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_bitmap_local_port_2_bitmap_bit_index(unit, port, &bit_index));
    word_index = bit_index / SOC_SAND_NOF_BITS_IN_UINT32; 
    bit_mask = (uint32)1 << (bit_index & (SOC_SAND_NOF_BITS_IN_UINT32 - 1));

    SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_get(unit, multicast_id_ndx, bitmap));

    if (bitmap[word_index] & bit_mask) {
        bitmap[word_index] &= ~bit_mask;
        SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_set(unit, multicast_id_ndx, bitmap));
        *out_err = _SHR_E_NONE;
    } else {
        *out_err = _SHR_E_NOT_FOUND; /* port is not replicated to */
    }

exit:
    SOCDNX_FUNC_RETURN;
}

