/*
 * $Id: dm.c,v 1.23.14.1 Broadcom SDK $
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
 * File:    dm.c
 * Purpose: Caladan3 Direct Map table lookup libraries 
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/types.h>
#include <soc/drv.h>

#ifdef BCM_CALADAN3_SUPPORT
#include <soc/sbx/caladan3/util.h>
#include <soc/sbx/sbx_drv.h>
#include <soc/mem.h>
#include <soc/mcm/allenum.h>
#include <soc/sbx/caladan3/tmu/tmu.h>
#include <soc/sbx/caladan3/tmu/cmd.h>
#include <shared/util.h>
#include <sal/appl/sal.h>


#ifdef  TMU_DM_USE_LOCKS
#define TMU_DM_LOCK(unit)     sal_mutex_take(_tmu_dbase[unit]->dm_mutex, sal_mutex_FOREVER);
#define TMU_DM_UNLOCK(unit)   sal_mutex_give(_tmu_dbase[unit]->dm_mutex);
#else
#define TMU_DM_LOCK(unit)    
#define TMU_DM_UNLOCK(unit)    
#endif

#define NUM_OF_ENTRY 128

static int tmu_dm_debug;
/*#define TMU_DM_DEBUG*/

int _soc_sbx_caladan3_tmu_dm_table_alloc(int unit,
                                         int size, 
                                         int entry_size_bits, 
                                         uint32 flags,
                                         int *tableid)
{
    soc_sbx_caladan3_table_attr_t attr;
    int status=SOC_E_NONE;

    if (!tableid) {
        return SOC_E_PARAM;
    }

    sal_memset(&attr, 0, sizeof(attr));

    attr.num_entries = size;
    attr.entry_size_bits = entry_size_bits;

    if (attr.entry_size_bits == SOC_SBX_TMU_DM_119b_SIZE) {
        attr.lookup = SOC_SBX_TMU_LKUP_DM_119;
    } else if (attr.entry_size_bits == SOC_SBX_TMU_DM_247b_SIZE) {
        attr.lookup = SOC_SBX_TMU_LKUP_DM_247;
    } else if (attr.entry_size_bits == SOC_SBX_TMU_DM_366b_SIZE) {
        attr.lookup = SOC_SBX_TMU_LKUP_DM_366;
    } else if (attr.entry_size_bits == SOC_SBX_TMU_DM_494b_SIZE) {
        attr.lookup = SOC_SBX_TMU_LKUP_DM_494;
    } else {
        status = SOC_E_PARAM;
    }

    attr.flags = flags;
    
    if (SOC_SUCCESS(status)) {
        status = soc_sbx_caladan3_tmu_table_alloc(unit, &attr);
        if (SOC_SUCCESS(status)) {
            *tableid = attr.id;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s: unit %d Failed to allocate table %d !!!\n"), 
                       FUNCTION_NAME(), unit, status));
        }
    }

    return status;
}

int soc_sbx_caladan3_tmu_dm_table_alloc(int unit, int size, int entry_size_bits, int *tableid)
{
    return _soc_sbx_caladan3_tmu_dm_table_alloc(unit,size,entry_size_bits,0,tableid);
}

int soc_sbx_caladan3_tmu_dm_table_alloc_id(int unit, int size, int entry_size_bits, int *tableid)
{
    return _soc_sbx_caladan3_tmu_dm_table_alloc(unit,size,entry_size_bits,
                                                SOC_SBX_TMU_TABLE_FLAG_WITH_ID,
                                                tableid);
}

int soc_sbx_caladan3_tmu_dm_table_free(int unit, int tableid)
{
    return soc_sbx_caladan3_tmu_table_free(unit, tableid);
}


int soc_sbx_caladan3_tmu_dm_generic_read_write(int unit, int tableid, int entry_index, 
                                               uint32 *entry, int entry_len, uint8 read)
{
    int status = SOC_E_NONE, word;
    soc_sbx_caladan3_tmu_cmd_t *cmd = NULL;
    soc_sbx_caladan3_table_attr_t *attr;

    if (tableid < 0 || tableid >= SOC_SBX_CALADAN3_TMU_MAX_TABLE || !entry) {
        return SOC_E_PARAM;
    }

    SOC_IF_TMU_UNINIT_RETURN(unit);
    TMU_DM_LOCK(unit);



    if (SOC_SBX_C3_BMP_MEMBER(_tmu_dbase[unit]->table_cfg.alloc_bmap, tableid)) {
        attr = &_tmu_dbase[unit]->table_cfg.table_attr[tableid];
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s : unit %d table not initialized \n"), 
                   FUNCTION_NAME(), unit));
        status = SOC_E_PARAM;
    }

    if (SOC_SUCCESS(status)) {    

        status = tmu_cmd_alloc(unit, &cmd);

        if (SOC_SUCCESS(status)) {
            if (read) {
                cmd->opcode = SOC_SBX_TMU_CMD_XL_READ;
                cmd->cmd.xlread.table = tableid;
                cmd->cmd.xlread.entry_num = entry_index;
                cmd->cmd.xlread.kv_pairs = 0;
                cmd->cmd.xlread.lookup = attr->lookup;
            } else {
                cmd->opcode = SOC_SBX_TMU_CMD_XL_WRITE;
                cmd->cmd.xlwrite.table = tableid;
                cmd->cmd.xlwrite.entry_num = entry_index;
                cmd->cmd.xlwrite.offset = 0; 
                cmd->cmd.xlwrite.size = attr->entry_size_bits / SOC_SBX_TMU_CMD_WORD_SIZE;
                cmd->cmd.xlwrite.value_size = attr->entry_size_bits;
                cmd->cmd.xlwrite.lookup = attr->lookup;
		for (word=0; word < BITS2WORDS(cmd->cmd.xlwrite.value_size); word++) {
		    cmd->cmd.xlwrite.value_data[word] = entry[word];
		}		
            }

            if (SOC_SUCCESS(status)) {
	      TMU_LOCK(unit); 
                status = soc_sbx_caladan3_tmu_post_cmd(unit, SOC_SBX_CALADAN3_TMU_DEF_CMD_FIFO, 
                                                       cmd, SOC_SBX_TMU_CMD_POST_FLAG_NONE);
                
                if (SOC_SUCCESS(status)) {
                    status = soc_sbx_caladan3_tmu_get_resp(unit, SOC_SBX_CALADAN3_TMU_DEF_CMD_FIFO, cmd,
                                                           entry, entry_len);
                    if (SOC_FAILURE(status)) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "%s: unit %d Invalid response !!!\n"), 
                                   FUNCTION_NAME(), unit));
                    }
              
		}
		TMU_UNLOCK(unit);             
	    }
        }
    }

    tmu_cmd_free(unit, cmd);
    TMU_DM_UNLOCK(unit);
    return status;
}

int soc_sbx_caladan3_tmu_dm_multi_generic_read_write(int unit, int tableid, int *entry_index, 
						     uint32 *entry, int entry_len, int num_entries, uint8 read)
{
    int status = SOC_E_NONE, word, dm_entry;
    soc_sbx_caladan3_tmu_cmd_t *cmd[NUM_OF_ENTRY];
    soc_sbx_caladan3_table_attr_t *attr;
    uint32 *curr_entry;

    if (tableid < 0 || tableid >= SOC_SBX_CALADAN3_TMU_MAX_TABLE || !entry) {
        return SOC_E_PARAM;
    }

    if (num_entries > NUM_OF_ENTRY) {
	LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s: unit %d batching more than 128 dm read/write operations !!!\n"), 
                   FUNCTION_NAME(), unit));
        return SOC_E_PARAM;
    } else {
        for (dm_entry = 0; dm_entry < NUM_OF_ENTRY; dm_entry++) {
            cmd[dm_entry] = NULL;
        }
    }

    SOC_IF_TMU_UNINIT_RETURN(unit);
    TMU_DM_LOCK(unit);

    if (SOC_SBX_C3_BMP_MEMBER(_tmu_dbase[unit]->table_cfg.alloc_bmap, tableid)) {
        attr = &_tmu_dbase[unit]->table_cfg.table_attr[tableid];
	if (BITS2WORDS(attr->entry_size_bits) != entry_len) {
	    LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s : unit %d table entry length mismatch \n"), 
                       FUNCTION_NAME(), unit));
	    status = SOC_E_PARAM;
	    return status;
	}
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s : unit %d table not initialized \n"), 
                   FUNCTION_NAME(), unit));
        status = SOC_E_PARAM;
	return status;
    }

    /* generate all commands */
    curr_entry = entry;
    for (dm_entry = 0; dm_entry < num_entries; dm_entry++) {
	if (SOC_SUCCESS(status)) {    	    
	    status = tmu_cmd_alloc(unit, &cmd[dm_entry]);
	    
	    if (SOC_SUCCESS(status)) {
		if (read) {
		    cmd[dm_entry]->opcode = SOC_SBX_TMU_CMD_XL_READ;
		    cmd[dm_entry]->cmd.xlread.table = tableid;
		    cmd[dm_entry]->cmd.xlread.entry_num = entry_index[dm_entry];
		    cmd[dm_entry]->cmd.xlread.kv_pairs = 0;
		    cmd[dm_entry]->cmd.xlread.lookup = attr->lookup;
		} else {
		    cmd[dm_entry]->opcode = SOC_SBX_TMU_CMD_XL_WRITE;
		    cmd[dm_entry]->cmd.xlwrite.table = tableid;
		    cmd[dm_entry]->cmd.xlwrite.entry_num = entry_index[dm_entry];
		    cmd[dm_entry]->cmd.xlwrite.offset = 0; 
		    cmd[dm_entry]->cmd.xlwrite.size = attr->entry_size_bits / SOC_SBX_TMU_CMD_WORD_SIZE;
		    cmd[dm_entry]->cmd.xlwrite.value_size = attr->entry_size_bits;
		    cmd[dm_entry]->cmd.xlwrite.lookup = attr->lookup;
		    for (word=0; word < BITS2WORDS(cmd[dm_entry]->cmd.xlwrite.value_size); word++) {
			cmd[dm_entry]->cmd.xlwrite.value_data[word] = curr_entry[word];
		    }		
		    curr_entry += entry_len;
		}
	    }
	}
    }

    /* cache/flush all the commands */
    if (SOC_SUCCESS(status)) {
	TMU_LOCK(unit); 
	
	for (dm_entry = 0; dm_entry < num_entries; dm_entry++) {
	    status = soc_sbx_caladan3_tmu_post_cmd(unit, SOC_SBX_CALADAN3_TMU_DEF_CMD_FIFO, 
						   cmd[dm_entry], SOC_SBX_TMU_CMD_POST_CACHE);
	    
	    if (SOC_SUCCESS(status)) {
		/* do nothing */
	    } else if (status == SOC_E_FULL) {
		status = soc_sbx_caladan3_tmu_post_cmd(unit, SOC_SBX_CALADAN3_TMU_DEF_CMD_FIFO,
						       NULL, SOC_SBX_TMU_CMD_POST_FLUSH);
        if (SOC_FAILURE(status)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%s: unit %d Failed to flush cmds after fifo full %d\n"),
                 FUNCTION_NAME(), unit, status));
        }
		
		/* Send out the command that got rejected */
		status = soc_sbx_caladan3_tmu_post_cmd(unit, SOC_SBX_CALADAN3_TMU_DEF_CMD_FIFO,
						       cmd[dm_entry], SOC_SBX_TMU_CMD_POST_CACHE);
        if (SOC_FAILURE(status)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "%s: unit %d Failed to cache cmds after fifo full %d\n"),
                 FUNCTION_NAME(), unit, status));
        }
	    } else {
		LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s: unit %d Failed to DMA command command %d\n"),
                           FUNCTION_NAME(), unit, status));
	    }
	}	
	
	/* Flush out any existing commands */
	status = soc_sbx_caladan3_tmu_post_cmd(unit, SOC_SBX_CALADAN3_TMU_DEF_CMD_FIFO,
					       NULL, SOC_SBX_TMU_CMD_POST_FLUSH);
	
	if (SOC_SUCCESS(status)) {
	    /* process all command response */
	    for (dm_entry = 0; dm_entry < num_entries; dm_entry++) {
		status = soc_sbx_caladan3_tmu_get_resp(unit, SOC_SBX_CALADAN3_TMU_DEF_CMD_FIFO, cmd[dm_entry],
						       entry+entry_len*dm_entry, entry_len);
		if (SOC_FAILURE(status)) {
		    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "%s: unit %d Invalid response !!!\n"), 
                               FUNCTION_NAME(), unit));
		}
	    }
	}
	
	TMU_UNLOCK(unit);             
    }

    for (dm_entry = 0; dm_entry < num_entries; dm_entry++) {
	if (cmd[dm_entry]) {
	    tmu_cmd_free(unit, cmd[dm_entry]);
	}
    }

    TMU_DM_UNLOCK(unit);
    return status;
}

int soc_sbx_caladan3_tmu_dm_get(int unit, int tableid, int entry_index, 
                                uint32 *entry, int entry_len)
{
    int status;

    status = soc_sbx_caladan3_tmu_dm_generic_read_write(unit, tableid, entry_index,
                                                        entry, entry_len, TRUE);
    if (tmu_dm_debug) {
        do {
            int index;
            LOG_CLI((BSL_META_U(unit,
                                "unit[%d] TMU DM Read Table-ID[%d Entry-Idx[%d] \n"),
                     unit, tableid, entry_index));
            for (index=0; index < entry_len; index++) {
                LOG_CLI((BSL_META_U(unit,
                                    "0x%08X "), entry[index]));
            }
	    LOG_CLI((BSL_META_U(unit,
                                "\n")));
        } while(0);
    }
    
    return status;
}

int soc_sbx_caladan3_tmu_dm_set(int unit, int tableid, int entry_index, 
                                uint32 *entry, int entry_len)
{
    int status = SOC_E_NONE;

    status = soc_sbx_caladan3_tmu_dm_generic_read_write(unit, tableid, entry_index,
            
                                            entry, entry_len, FALSE);

    if (tmu_dm_debug) {
        do {
            int index;
            LOG_CLI((BSL_META_U(unit,
                                "unit[%d] TMU DM Write Table-ID[%d Entry-Idx[%d] \n"),
                     unit, tableid, entry_index));
            for (index=0; index < entry_len; index++) {
                LOG_CLI((BSL_META_U(unit,
                                    "0x%08X "), entry[index]));
            }
	    LOG_CLI((BSL_META_U(unit,
                                "\n")));
        } while(0);
    }

    return status;
}

int soc_sbx_caladan3_tmu_dm_multi_set(int unit, int tableid, int *entry_index, 
				      uint32 *entry, int entry_len, int num_entries)
{
    int status = SOC_E_NONE;

    status = soc_sbx_caladan3_tmu_dm_multi_generic_read_write(unit, tableid, entry_index,            
							      entry, entry_len, num_entries, FALSE);

    if (tmu_dm_debug) {
        do {
	    int index, dm_entry;

	    for (dm_entry = 0; dm_entry < num_entries; dm_entry++) {
		LOG_CLI((BSL_META_U(unit,
                                    "unit[%d] TMU DM Write Table-ID[%d Entry-Idx[%d] \n"),
                         unit, tableid, *entry_index));
		for (index=0; index < entry_len; index++) {
		    LOG_CLI((BSL_META_U(unit,
                                        "0x%08X "), entry[index]));
		}
		LOG_CLI((BSL_META_U(unit,
                                    "\n")));
		entry_index++;
		entry += entry_len;
	    }
        } while(0);
    }

    return status;
}

/* Set talbe entries as value of zero */
int soc_sbx_caladan3_tmu_dm_table_clear(int unit, int tableid, int size, 
        int entry_len)
{
    int rv = SOC_E_NONE;
    uint32 *write_buf = NULL;
    int i, j;
    int entry_index[NUM_OF_ENTRY];
    uint32 tlen;

    tlen      = entry_len * sizeof(uint32) * NUM_OF_ENTRY;
    write_buf = (uint32*)sal_alloc(tlen, "tmu-write-buf");
    if (write_buf!=NULL) {
        sal_memset(write_buf, 0, tlen);
    } else {
        return SOC_E_MEMORY;
    }

    for (i=0; i<size; i+=NUM_OF_ENTRY) {
        for (j=0; j<NUM_OF_ENTRY; j++) {
            entry_index[j] = i + j;
        }
        tlen = size-i<NUM_OF_ENTRY ? size%NUM_OF_ENTRY : NUM_OF_ENTRY;
        rv = soc_sbx_caladan3_tmu_dm_multi_set(unit, tableid, entry_index, 
               &write_buf[0], entry_len, tlen );
        if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META_U(unit,
                                "%d Failed to set DM table ID [%d] Entry[%d-%d]: %d\n"),
                     unit, tableid, i, i+NUM_OF_ENTRY, rv));
            break;
        }     
     }

    sal_free(write_buf);

    return rv;
}

#endif
