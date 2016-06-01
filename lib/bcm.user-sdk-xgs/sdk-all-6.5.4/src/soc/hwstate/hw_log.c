/*
 * $Id: hw_log.c,v 1 2014/09/22 12:12:10 Miry Heller Exp $
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
 * This file contains structure and routine declarations for the
 * Hw Log journal for Crash Recovery LOG Mechanism.
 */

#include <appl/diag/parse.h>
#include <soc/dcmn/error.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <shared/bsl.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_chip_regs.h>
#endif

#if !defined(__KERNEL__) && defined (LINUX)
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/ha.h>
#define HW_LOG_VERSION_1_0 1
#define HW_LOG_STRUCT_SIG 0
typedef enum {
    HA_HW_LOG_SUB_ID_0 = 0
} HA_sub_id_tl;
#endif
#endif

/* ERROr Module Name */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

#define HWLOG_MARGIN sizeof(HwLogEntry_t)


/* change this flag to 1 to get debug prints */
#define HW_LOG_DEBUG_ENABLE 0
#if HW_LOG_DEBUG_ENABLE
#define HW_LOG_DEBUG(op) op
#else
#define HW_LOG_DEBUG(op)
#endif


/* ------------------------------EXTERNAL FUNCTION DECLARATION-----------------------*/
int reg_test(int unit, args_t *a, void *pa);

/* ------------------------------INTERNAL FUNCTION DECLARATION-----------------------*/
STATIC int _soc_hw_log_insert_entry_to_log(int unit, LogEntry_t *data);
STATIC int _soc_hw_log_write_memory_to_log(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data);
STATIC int _soc_hw_log_read_memory_entry_from_log(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data);
STATIC int _soc_hw_log_write_register32_to_log(int unit, soc_reg_t reg, int port, int index, uint32 entry_data);
STATIC int _soc_hw_log_read_register32_entry_from_log(int unit, soc_reg_t reg, int port, int index, uint32 *entry_data);
STATIC int _soc_hw_log_write_register64_to_log(int unit, soc_reg_t reg, int port, int index, uint64 entry_data);
STATIC int _soc_hw_log_read_register64_entry_from_log(int unit, soc_reg_t reg, int port, int index, uint64 *entry_data);
STATIC int _soc_hw_log_write_reg_above64_to_log(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t entry_data);
STATIC int _soc_hw_log_read_reg_above64_entry_from_log(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t entry_data);
STATIC int _soc_hw_log_polling(int unit, sal_usecs_t time_out, int32 min_polls, soc_reg_t reg, int32 port, int32 index, soc_field_t field, uint32 expected_value);
STATIC int _soc_hw_log_direct_reg_set(int unit, int cmic_block, uint32 addr, uint32 dwc_write, uint32 *data);
STATIC int _soc_hw_log_fast_reg_set(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data);
STATIC int _soc_hw_log_fast_reg_get(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data);
STATIC int _soc_hw_log_soc_reg_set(int unit, soc_reg_t reg, int port, int index, uint64 data);
STATIC int _soc_hw_log_soc_reg_get(int unit, soc_reg_t reg, int port, int index, uint64 *data);
STATIC int _soc_hw_log_reg_set_nocache(int unit, soc_reg_t reg, int port, int index, uint64 data);

/* ------------------------------Global Variables------------------------------------*/

soc_hw_log_access_t crash_rec_access =
{
        _soc_hw_log_read_register32_entry_from_log,
        _soc_hw_log_read_register64_entry_from_log,
        _soc_hw_log_read_reg_above64_entry_from_log,
        _soc_hw_log_write_register32_to_log,
        _soc_hw_log_write_register64_to_log,
        _soc_hw_log_write_reg_above64_to_log,
        _soc_hw_log_read_memory_entry_from_log,
        _soc_hw_log_write_memory_to_log,
        _soc_hw_log_polling,
        _soc_hw_log_direct_reg_set,
        _soc_hw_log_fast_reg_set,
        _soc_hw_log_fast_reg_get,
        _soc_hw_log_soc_reg_set,
        _soc_hw_log_soc_reg_get,
        _soc_hw_log_reg_set_nocache
};

LogList_t Hw_Log_List[SOC_MAX_NUM_DEVICES];


/* ------------------------------FUNCTION IMPLEMENTATION-----------------------------*/

int
soc_hw_log_transaction_start(int unit)
{
    SOC_INIT_FUNC_DEFS;

    Hw_Log_List[unit].is_logging = 1;
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int
soc_hw_log_transaction_end(int unit)
{
    SOC_INIT_FUNC_DEFS;

    Hw_Log_List[unit].is_logging = 0;
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_soc_hw_log_log_list_is_empty(int unit)
{
    return !(Hw_Log_List[unit].header->nof_elements > 0);
}

int
soc_hw_log_is_initialized(int unit)
{
    return Hw_Log_List[unit].init;
}

/* Allow Commits only from main thread */
STATIC int
_soc_hw_log_is_my_thread(int unit)
{
    return (sal_thread_self() == Hw_Log_List[unit].tid);
}

void
soc_hw_set_immediate_hw_access(int unit, int *stored_val)
{
    if (!_soc_hw_log_is_my_thread(unit)) return;

    *stored_val = Hw_Log_List[unit].ImmediateAccess;
    Hw_Log_List[unit].ImmediateAccess = TRUE;
}

/* this function should be called with "stored_val" as the value for restored_val
   (received from soc_hw_set_immediate_hw_access)*/
void
soc_hw_restore_immediate_hw_access(int unit, int restored_val)
{
    if (_soc_hw_log_is_my_thread(unit))
        Hw_Log_List[unit].ImmediateAccess = restored_val;
}

int
soc_hw_log_do_hwlog_read_write(int unit)
{
    return SOC_IS_DONE_INIT(unit)           &&     /* don't log during init */
           soc_hw_log_is_initialized(unit)  &&     /* HW LOG is enabled and initialized */
           _soc_hw_log_is_my_thread(unit)   &&     /* Allow HW LOG manipulation only from main thread */
           Hw_Log_List[unit].is_logging     &&     /* verify there is an active transaction */
           !Hw_Log_List[unit].ImmediateAccess;     /* We are not in the middle of Commit */
}

int
soc_hw_log_init(int unit, uint32 size)
{
    uint8  *mem_ptr = NULL;

    SOC_INIT_FUNC_DEFS;

    /* allocation hw log in Shared memory
       (cold boot - create; warm boot - retrieve) */
    mem_ptr = ha_mem_alloc(unit, HA_HW_LOG_Mem_Pool, HA_HW_LOG_SUB_ID_0, 
                           HW_LOG_VERSION_1_0, HW_LOG_STRUCT_SIG, &size);
    if (!mem_ptr)
    {
        LOG_ERROR(BSL_LS_SOC_HWLOG,
                       (BSL_META_U(unit, "HW LOG Allocation failed for unit %d. LogSize %d\n"), unit, size));
        return SOC_E_MEMORY;
    }

    /* set the static data (warm and cold boot) */
    Hw_Log_List[unit].Access_cb = crash_rec_access;
    Hw_Log_List[unit].tid = sal_thread_self();
    Hw_Log_List[unit].ImmediateAccess = FALSE;
    Hw_Log_List[unit].header = (hw_log_header_t *) mem_ptr;
    Hw_Log_List[unit].journal = (HwLogEntry_t *) (mem_ptr + sizeof(hw_log_header_t));
    Hw_Log_List[unit].is_logging = 0;

    /* Reset the journal (only in Cold boot */
    if (!SOC_WARM_BOOT(unit)) {
        memset(mem_ptr, 0, size);

        Hw_Log_List[unit].header->max_elements = 
            (size - sizeof(hw_log_header_t) - HWLOG_MARGIN) / sizeof(HwLogEntry_t);
        Hw_Log_List[unit].header->nof_elements = 0;
    }

    /* mark as initialized */
    Hw_Log_List[unit].init = TRUE;

    LOG_VERBOSE(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "HW LOG Enabled for unit %d. LogSize %d Bytes\n"), unit, size));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int
soc_hw_log_deinit(int unit)
{
    Hw_Log_List[unit].ImmediateAccess = TRUE;
    Hw_Log_List[unit].init = FALSE;

    return SOC_E_NONE;
}

void
_soc_hw_log_print_list_memory_entry(int unit, LogEntry_Data_t *Data)
{
    uint32 entry_bytes = soc_mem_entry_bytes(unit, Data->MemData.Mem);
    uint32 entry_words = soc_mem_entry_words(unit, Data->MemData.Mem);

    int i;
    /* Memory entry print */
    LOG_ERROR(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "Unit %d mem %d (%s) Array %d blk %d index %d  bytesInEntry %d wordsInEntry %d data 0x%X\n"), unit, Data->MemData.Mem,
                    SOC_MEM_NAME( unit, Data->MemData.Mem), Data->MemData.ArrayIndex, Data->MemData.Blk, /*Use COPYNO_ALL for all*/
            Data->MemData.Index, entry_bytes, entry_words, Data->MemData.EntryData[0]));

    for (i = 1; i < entry_words ; i++) /* Print the data */
    {
        LOG_ERROR(BSL_LS_SOC_HWLOG,
                (BSL_META_U(unit, "[%d]%X\n"), i, Data->MemData.EntryData[i]));
    }
/*    LOG_ERROR(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "\n")));*/
}

void
_soc_hw_log_print_list_register_entry(int unit, LogEntry_Data_t *Data)
{
    /* Register entry print */
    LOG_ERROR(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "Unit %d Reg %d (%s) Port 0x%X Index %d Data 0x%X\n"), unit, Data->RegData.Reg,
                    SOC_REG_NAME( unit, Data->RegData.Reg), Data->RegData.Port,
                    Data->RegData.Index, Data->RegData.EntryData[0]));
}

void
_soc_hw_log_print_list_polling_entry(int unit, LogEntry_Data_t *Data)
{
    /* Polling entry print */
    LOG_ERROR(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "Unit %d Polling: Reg %d (%s) Timeout %d MinPolls %d Port 0x%X Index %d ExpectedValue 0x%X\n"),
                    unit,
                    Data->PollingData.Reg,
                    SOC_REG_NAME( unit, Data->PollingData.Reg),
                    Data->PollingData.TimeOut,
                    Data->PollingData.MinPolls,
                    Data->PollingData.Port,
                    Data->PollingData.Index,
                    Data->PollingData.ExpectedValue));
}

void
_soc_hw_log_print_list_direct_reg_entry(int unit, LogEntry_Data_t *Data)
{
    /* Direct Regs entry print */
    LOG_ERROR(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "Unit %d DirectRegSet: CmicBlock 0x%X Addr 0x%X DwcWrite 0x%X Data 0x%X\n"),
                    unit,
                    Data->DirectRegData.CmicBlock,
                    Data->DirectRegData.Addr,
                    Data->DirectRegData.DwcWrite,
                    Data->DirectRegData.EntryData));
}

void
_soc_hw_log_print_list_fast_reg_entry(int unit, LogEntry_Data_t *Data)
{
    /* Direct Regs entry print */
    LOG_ERROR(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "Unit %d FastRegSet: Reg %d (%s) AccType %d Addr 0x%X Block 0x%X Data 0x%X\n"),
                    unit,
                    Data->FastRegData.Reg,
                    SOC_REG_NAME( unit, Data->FastRegData.Reg),
                    Data->FastRegData.AccType,
                    Data->FastRegData.Addr,
                    Data->FastRegData.Block,
                    Data->FastRegData.EntryData[0]));
}
void
_soc_hw_log_print_list_soc_reg_entry(int unit, LogEntry_Data_t *Data)
{
    /* Regs entry print */
    LOG_ERROR(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "Unit %d SocRegData: Reg %d (%s) Port %d Index %d Data 0x%X\n"),
                    unit,
                    Data->SocRegData.Reg,
                    SOC_REG_NAME( unit, Data->SocRegData.Reg),
                    Data->SocRegData.Port,
                    Data->SocRegData.Index,
                    *((uint32 *)&Data->SocRegData.EntryData)));
}

void
soc_hw_log_print_single_entry(int unit, HwLogEntry_t *entry)
{
	if (!entry)
		return;

    if (entry->Entry.Type == LOG_ENTRY_TYPE_Memory)
    {
        _soc_hw_log_print_list_memory_entry(unit, &entry->Entry.Data);
    }
    else if ((entry->Entry.Type == LOG_ENTRY_TYPE_Register32) || (entry->Entry.Type == LOG_ENTRY_TYPE_Register64) || (entry->Entry.Type == LOG_ENTRY_TYPE_Register_Above64))
    {
        _soc_hw_log_print_list_register_entry(unit, &entry->Entry.Data);
    }
    else if (entry->Entry.Type == LOG_ENTRY_TYPE_Polling)
    {
        _soc_hw_log_print_list_polling_entry(unit, &entry->Entry.Data);
    }
    else if (entry->Entry.Type == LOG_ENTRY_TYPE_DirectRegWrite)
    {
        _soc_hw_log_print_list_direct_reg_entry(unit, &entry->Entry.Data);
    }
    else if  (entry->Entry.Type == LOG_ENTRY_TYPE_FastRegData)
    {
        _soc_hw_log_print_list_fast_reg_entry(unit, &entry->Entry.Data);
    }
    else if ((entry->Entry.Type == LOG_ENTRY_TYPE_SocRegData) || (entry->Entry.Type == LOG_ENTRY_TYPE_SocRegNoCacheData))
    {
        _soc_hw_log_print_list_soc_reg_entry(unit, &entry->Entry.Data);
    }

}

int
soc_hw_log_print_list(int unit)
{
    uint32 i;

    SOC_INIT_FUNC_DEFS;

    if (_soc_hw_log_log_list_is_empty(unit))
        return SOC_E_NONE;

    for (i=0 ; i < Hw_Log_List[unit].header->nof_elements; i++) {
        soc_hw_log_print_single_entry(unit, &(Hw_Log_List[unit].journal[i]));
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int
soc_hw_log_purge(int unit)
{
    SOC_INIT_FUNC_DEFS;

    Hw_Log_List[unit].header->nof_elements = 0;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int
soc_hw_log_commit(int unit)
{
    HwLogEntry_t   *entry;
    uint32          i;
    soc_error_t     res             = SOC_E_NONE,
                    func_res        = SOC_E_NONE;

    SOC_INIT_FUNC_DEFS;

    /* If Log is empty --> nothing to commit */
    if (_soc_hw_log_log_list_is_empty(unit)) SOC_EXIT;

    LOG_VERBOSE(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "----------   COMMIT START   ----------: \n")));

    Hw_Log_List[unit].ImmediateAccess = TRUE;

    for (i=0 ; i < Hw_Log_List[unit].header->nof_elements; i++) {

        entry = &(Hw_Log_List[unit].journal[i]);

        if (entry->Entry.Type == LOG_ENTRY_TYPE_Memory)
        {
        	res = soc_mem_array_write(unit, entry->Entry.Data.MemData.Mem, entry->Entry.Data.MemData.ArrayIndex, entry->Entry.Data.MemData.Blk, entry->Entry.Data.MemData.Index, entry->Entry.Data.MemData.EntryData);
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_Register32)
        {
        	res = soc_reg32_set(unit, entry->Entry.Data.RegData.Reg, entry->Entry.Data.RegData.Port, entry->Entry.Data.RegData.Index, entry->Entry.Data.RegData.EntryData[0]);
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_Register64)
        {
        	res = soc_reg64_set(unit, entry->Entry.Data.RegData.Reg, entry->Entry.Data.RegData.Port, entry->Entry.Data.RegData.Index, *(uint64 *)(entry->Entry.Data.RegData.EntryData));
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_Register_Above64)
        {
        	res = soc_reg_above_64_set(unit, entry->Entry.Data.RegData.Reg, entry->Entry.Data.RegData.Port, entry->Entry.Data.RegData.Index, entry->Entry.Data.RegData.EntryData);
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_Polling)
        {
/*            _soc_hw_log_print_list_polling_entry(unit, &entry->Entry.Data);*/
#ifdef BCM_ARAD_SUPPORT
            res = arad_polling(unit,  entry->Entry.Data.PollingData.TimeOut, entry->Entry.Data.PollingData.MinPolls, entry->Entry.Data.PollingData.Reg, entry->Entry.Data.PollingData.Port, entry->Entry.Data.PollingData.Index,   entry->Entry.Data.PollingData.Field, entry->Entry.Data.PollingData.ExpectedValue);
#endif
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_DirectRegWrite)
        {
            _soc_hw_log_print_list_direct_reg_entry(unit, &entry->Entry.Data);
#ifdef BCM_ARAD_SUPPORT
            res = soc_direct_reg_set(unit, entry->Entry.Data.DirectRegData.CmicBlock, entry->Entry.Data.DirectRegData.Addr, entry->Entry.Data.DirectRegData.DwcWrite, &entry->Entry.Data.DirectRegData.EntryData);
#endif
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_FastRegData)
        {
/*			_soc_hw_log_print_list_fast_reg_entry(unit, &entry->Entry.Data);*/
#ifdef BCM_ARAD_SUPPORT
            res = arad_fast_reg_set(unit, entry->Entry.Data.FastRegData.Reg, entry->Entry.Data.FastRegData.AccType, entry->Entry.Data.FastRegData.Addr, entry->Entry.Data.FastRegData.Block, entry->Entry.Data.FastRegData.EntryData);
#endif
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_SocRegData)
        {
/*            _soc_hw_log_print_list_soc_reg_entry(unit, &entry->Entry.Data);*/
            res = soc_reg_set(unit, entry->Entry.Data.SocRegData.Reg, entry->Entry.Data.SocRegData.Port, entry->Entry.Data.SocRegData.Index, entry->Entry.Data.SocRegData.EntryData);
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_SocRegNoCacheData)
        {
/*            _soc_hw_log_print_list_soc_reg_entry(unit, &entry->Entry.Data);*/
            res = soc_reg_set_nocache(unit, entry->Entry.Data.SocRegNoCacheData.Reg, entry->Entry.Data.SocRegNoCacheData.Port, entry->Entry.Data.SocRegNoCacheData.Index, entry->Entry.Data.SocRegNoCacheData.EntryData);
        }
        else
        {
            LOG_ERROR(BSL_LS_SOC_HWLOG,
                (BSL_META_U(unit, "Unit %d: Unknown Entry Type %d FAILED TO COMMIT!!!!.\n"), unit, entry->Entry.Type));
        }


        if (SOC_FAILURE(res))
        {
            LOG_ERROR(BSL_LS_SOC_HWLOG,
                            (BSL_META_U(unit, "Unit %d: FAILED TO COMMIT Entry Type %d!!!.\n"), unit, entry->Entry.Type));

            soc_hw_log_print_single_entry(unit, entry);	/* Print the problematic entry */

            /* continue execution but remember the error */
        	func_res |= res;	/* Remember error result*/
        }
    }
    
    /* NOTE: clearing the list is the caller responssibility */

    /* start logging again */
    Hw_Log_List[unit].ImmediateAccess = FALSE;

    LOG_VERBOSE(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "---------- END COMMIT ---------- : \n")));

    /* check for the error summarry for all entries */
    _SOC_IF_ERR_EXIT(func_res);

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;

}



STATIC int _soc_hw_log_insert_entry_to_log(int unit, LogEntry_t *data)
{
    HwLogEntry_t *entry = NULL;

    SOC_INIT_FUNC_DEFS;

    if (!soc_hw_log_is_initialized(unit))
    {
        /* The List is not initialized */
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                   (BSL_META_U(unit,
                      "unit:%d The Hw Log List is not initialized, failed to insert.\n"), unit));
    }

    /* Check if The list is full */
    if (Hw_Log_List[unit].header->max_elements == Hw_Log_List[unit].header->nof_elements) {
        /* The journal is full */
        _SOC_EXIT_WITH_ERR(SOC_E_FULL,
                   (BSL_META_U(unit,
                      "unit:%d The Hw Log List is FULL.\n"), unit));
    }

    /* prepare ptr to the next free entry */
    entry = &(Hw_Log_List[unit].journal[Hw_Log_List[unit].header->nof_elements]);
    Hw_Log_List[unit].header->nof_elements++;

    /* Save the new content in the log list */
    sal_memcpy(&(entry->Entry), data, sizeof(LogEntry_t)/*data->data_size*/);

	LOG_DEBUG(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "---   Entry Added to the HW LOG !!!   ---: \n")));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;

}


STATIC int _soc_hw_log_write_memory_to_log(int unit, soc_mem_t mem,
        unsigned array_index, int copyno, int index, void *entry_data)
{
    LogEntry_t entry;
    uint32 entry_words;

    sal_memset(entry.Data.MemData.EntryData, 0, sizeof(entry.Data.MemData.EntryData));

    entry_words                   = soc_mem_entry_words(unit, mem);
    /* Init the Log entry */
    entry.Data.MemData.Mem        = mem;
    entry.Data.MemData.ArrayIndex = array_index;
    entry.Data.MemData.Blk        = copyno;
    entry.Data.MemData.Index      = index;

    entry.Type                    = LOG_ENTRY_TYPE_Memory;

    sal_memcpy(entry.Data.MemData.EntryData, ((char*) entry_data),  entry_words*sizeof(int));

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}

STATIC int _soc_hw_log_read_memory_entry_from_log(int unit, soc_mem_t mem,
        unsigned array_index, int copyno, int index, void *entry_data)
{
    HwLogEntry_t    *entry;
    uint32          entry_words;
    int             found   = FALSE;
    soc_mem_t       saved_in_hw_log_mem;
    uint32          i;

    /* Get the alias memory. Note: updates 'mem' value. */
    SOC_MEM_ALIAS_TO_ORIG(unit, mem);

    if (!soc_hw_log_is_initialized(unit) || _soc_hw_log_log_list_is_empty(unit)) 
    {
        /* The List is not initialized or empty */
        goto not_found;
    }

    for (i=0 ; i < Hw_Log_List[unit].header->nof_elements; i++) {

        entry = &(Hw_Log_List[unit].journal[i]);

        if (entry->Entry.Type == LOG_ENTRY_TYPE_Memory)
        {
            saved_in_hw_log_mem = entry->Entry.Data.MemData.Mem;
            /* Get the alias memory for saved mem value. Note: updates 'saved_mem' value. */
            SOC_MEM_ALIAS_TO_ORIG(unit, saved_in_hw_log_mem);

            if (    (mem                == saved_in_hw_log_mem)
                    && (array_index     == entry->Entry.Data.MemData.ArrayIndex)
                    && ((copyno         == entry->Entry.Data.MemData.Blk) || (entry->Entry.Data.MemData.Blk == MEM_BLOCK_ANY))
                    && (index           == entry->Entry.Data.MemData.Index))
            {
                entry_words             = soc_mem_entry_words(unit, mem);

                sal_memcpy(((char *) entry_data), entry->Entry.Data.MemData.EntryData , entry_words*sizeof(int));

                found = TRUE;
                LOG_DEBUG( BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "MEM FOUND in LOG!!! .\n")) );
                HW_LOG_DEBUG(_soc_hw_log_print_list_memory_entry(unit, entry));
            }
        }
    }

not_found:
    return found;
}


STATIC int
_soc_hw_log_write_register_to_log(int unit, soc_reg_t reg, int port, int index, LogEntry_Type_t type, void * entry_data)
{
    LogEntry_t entry;
    int size = (type == LOG_ENTRY_TYPE_Register32 ? sizeof(uint32) : ((type == LOG_ENTRY_TYPE_Register64) ? sizeof(uint64) : sizeof(soc_reg_above_64_val_t)));

    sal_memset(entry.Data.RegData.EntryData, 0, sizeof(soc_reg_above_64_val_t));

    /* Init the Log entry */
    entry.Data.RegData.Reg        = reg;
    entry.Data.RegData.Port       = port;
    entry.Data.RegData.Index      = index;
    entry.Type                    = type;

    sal_memcpy(entry.Data.RegData.EntryData, (int *) entry_data, size);

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}


STATIC int
_soc_hw_log_read_register_entry_from_log(int unit, soc_reg_t reg, int port, int index, LogEntry_Type_t type, void* entry_data)
{
    HwLogEntry_t    *entry;
    uint32           i;
    int              found   = FALSE;
    soc_error_t      res     = SOC_E_NONE;

    if (!soc_hw_log_is_initialized(unit) || _soc_hw_log_log_list_is_empty(unit)) 
    {
        /* The List is not initialized or empty */
        goto not_found;
    }

    for (i=0 ; i < Hw_Log_List[unit].header->nof_elements; i++) {

        entry = &(Hw_Log_List[unit].journal[i]);

        if ((entry->Entry.Type == LOG_ENTRY_TYPE_Register32) || (entry->Entry.Type == LOG_ENTRY_TYPE_Register64) || (entry->Entry.Type == LOG_ENTRY_TYPE_Register_Above64))
        {
            if ((reg == entry->Entry.Data.RegData.Reg)
                    && ((port == entry->Entry.Data.RegData.Port) /* Exact match for lookup*/
                            || (entry->Entry.Data.RegData.Port == REG_PORT_ANY)
                            || (entry->Entry.Data.RegData.Port == SOC_CORE_ALL) /* Stored data is for all cores*/
                            || (port                           == REG_PORT_ANY)
                            || (port                           == SOC_CORE_ALL))    /*requested data is for all cores - we can return data from any port/core*/
                    && (index == entry->Entry.Data.RegData.Index))
            {
                int size = (type == LOG_ENTRY_TYPE_Register32 ? sizeof(uint32) : ((type == LOG_ENTRY_TYPE_Register64) ? sizeof(uint64) : sizeof(soc_reg_above_64_val_t)));
                sal_memcpy((int *) entry_data, entry->Entry.Data.RegData.EntryData, size);

                found = TRUE;

                LOG_DEBUG(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "REG FOUND in LOG!!! .\n")));
                HW_LOG_DEBUG(_soc_hw_log_print_list_register_entry(unit, entry->Entry.Data));
            }
        }
    }

not_found:
    if (!found)
    {
        Hw_Log_List[unit].ImmediateAccess = TRUE;
        switch (type)
        {
        case LOG_ENTRY_TYPE_Register32:
        	res = soc_reg32_get(unit, reg, port, index, entry_data);
            break;
        case LOG_ENTRY_TYPE_Register64:
        	res = soc_reg64_get(unit, reg, port, index, entry_data);
            break;
        case LOG_ENTRY_TYPE_Register_Above64:
        	res = soc_reg_above_64_get(unit, reg, port, index, entry_data);
            break;
        default:
            LOG_DEBUG(BSL_LS_SOC_HWLOG, 
                (BSL_META_U(unit, "!! Got into _soc_hw_log_read_register_entry_from_log with type %d, reg %d!! .\n"), type, reg));
        }
        Hw_Log_List[unit].ImmediateAccess = FALSE;

        if (SOC_FAILURE(res))
        {
            LOG_ERROR(BSL_LS_SOC_HWLOG,
                            (BSL_META_U(unit, "Unit %d: FAILED to READ reg %d(%s) port %d index %d !!!.\n"), 
                             unit, reg, SOC_REG_NAME( unit, reg), port, index));
        }
    }

    return res;
}

STATIC int
_soc_hw_log_write_register32_to_log(int unit, soc_reg_t reg, int port, int index, uint32 entry_data)
{
    LOG_DEBUG(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "_soc_hw_log_write_register32_to_log!.\n")));

    _soc_hw_log_write_register_to_log(unit, reg, port, index, LOG_ENTRY_TYPE_Register32, &entry_data);
    return SOC_E_NONE;
}

STATIC int
_soc_hw_log_read_register32_entry_from_log(int unit, soc_reg_t reg, int port, int index, uint32 *entry_data)
{
    LOG_DEBUG(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "_soc_hw_log_read_register32_entry_from_log!.\n")));

    _soc_hw_log_read_register_entry_from_log( unit, reg, port, index, LOG_ENTRY_TYPE_Register32, entry_data);
    return SOC_E_NONE;

}

STATIC int
_soc_hw_log_write_register64_to_log(int unit, soc_reg_t reg, int port, int index, uint64 entry_data)
{
    LOG_DEBUG(BSL_LS_SOC_HWLOG,
                (BSL_META_U(unit, "_soc_hw_log_write_register64_to_log!.\n")));

    _soc_hw_log_write_register_to_log(unit, reg, port, index, LOG_ENTRY_TYPE_Register64, &entry_data);
    return SOC_E_NONE;
}

STATIC int
_soc_hw_log_read_register64_entry_from_log(int unit, soc_reg_t reg, int port, int index,
        uint64 *entry_data)
{
    LOG_DEBUG(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "_soc_hw_log_read_register64_entry_from_log!.\n")));

    _soc_hw_log_read_register_entry_from_log( unit, reg, port, index, LOG_ENTRY_TYPE_Register64, entry_data);

    return SOC_E_NONE;
}

STATIC int
_soc_hw_log_write_reg_above64_to_log(int unit, soc_reg_t reg, int port, int index,
        soc_reg_above_64_val_t entry_data)
{
    LOG_DEBUG(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "_soc_hw_log_write_reg_above64_to_log!.\n")));

    _soc_hw_log_write_register_to_log(unit, reg, port, index, LOG_ENTRY_TYPE_Register_Above64, entry_data);
    return SOC_E_NONE;
}

STATIC int
_soc_hw_log_read_reg_above64_entry_from_log(int unit, soc_reg_t reg, int port, int index,
        soc_reg_above_64_val_t entry_data)
{
    LOG_DEBUG(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "_soc_hw_log_read_reg_above64_entry_from_log!.\n")));

    _soc_hw_log_read_register_entry_from_log( unit, reg, port, index, LOG_ENTRY_TYPE_Register_Above64, entry_data);

    return SOC_E_NONE;
}



STATIC int
_soc_hw_log_polling(int unit, sal_usecs_t time_out, int32 min_polls, soc_reg_t reg, int32 port, int32 index,
        soc_field_t field, uint32 expected_value)
{
    LogEntry_t entry;

    LOG_DEBUG(BSL_LS_SOC_HWLOG,
       (BSL_META_U(unit, "_soc_hw_log_polling!.\n")));


    /* Init the Log entry */
    entry.Data.PollingData.TimeOut      = time_out;
    entry.Data.PollingData.MinPolls     = min_polls;
    entry.Data.PollingData.Reg          = reg;
    entry.Data.PollingData.Port         = port;
    entry.Data.PollingData.Index        = index;
    entry.Data.PollingData.Field        = field;
    entry.Data.PollingData.ExpectedValue= expected_value;
    entry.Type                          = LOG_ENTRY_TYPE_Polling;

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}

STATIC int
_soc_hw_log_direct_reg_set(int unit, int cmic_block, uint32 addr, uint32 dwc_write, uint32 *data)
{
    LogEntry_t entry;

    LOG_DEBUG(BSL_LS_SOC_HWLOG,
        (BSL_META_U(unit, "_soc_hw_log_direct_reg_set!.\n")));

    entry.Data.DirectRegData.CmicBlock  = cmic_block;
    entry.Data.DirectRegData.Addr       = addr;
    entry.Data.DirectRegData.DwcWrite   = dwc_write;
    entry.Data.DirectRegData.EntryData  = *data;

    entry.Type                          = LOG_ENTRY_TYPE_DirectRegWrite;

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}
STATIC int
_soc_hw_log_fast_reg_set(int unit, soc_reg_t reg, int acc_type, int addr, int block,
        soc_reg_above_64_val_t data)
{
    LogEntry_t entry;

    LOG_DEBUG(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "_soc_hw_log_direct_reg_set!.\n")));

    entry.Data.FastRegData.Reg        = reg;
    entry.Data.FastRegData.AccType    = acc_type;
    entry.Data.FastRegData.Addr       = addr;
    entry.Data.FastRegData.Block      = block;
    sal_memcpy(entry.Data.FastRegData.EntryData, data, sizeof(soc_reg_above_64_val_t));

    entry.Type                          = LOG_ENTRY_TYPE_FastRegData;

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}

STATIC int
_soc_hw_log_fast_reg_get(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data)
{
    HwLogEntry_t    *entry          = NULL;
    int             found           = FALSE;
    soc_error_t     res             = SOC_E_NONE;
    uint32          i;

    if (!soc_hw_log_is_initialized(unit) || _soc_hw_log_log_list_is_empty(unit)) 
    {
        /* The List is not initialized or empty */
        goto not_found;
    }

    for (i=0 ; i < Hw_Log_List[unit].header->nof_elements; i++) {

        entry = &(Hw_Log_List[unit].journal[i]);

        if (entry->Entry.Type == LOG_ENTRY_TYPE_FastRegData)
        {
            if (       (reg         == entry->Entry.Data.FastRegData.Reg)
                    && (acc_type    == entry->Entry.Data.FastRegData.AccType)
                    && (addr        == entry->Entry.Data.FastRegData.Addr)
                    && (block       == entry->Entry.Data.FastRegData.Block ))
            {
                sal_memcpy(data, entry->Entry.Data.FastRegData.EntryData, sizeof(soc_reg_above_64_val_t));

                found = TRUE;
                LOG_DEBUG( BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "MEM FOUND in LOG!!! .\n")) );
                HW_LOG_DEBUG(_soc_hw_log_print_list_memory_entry(unit, entry->Entry.Data));
            }
        }
    }

not_found:
    if (!found)
    {
        Hw_Log_List[unit].ImmediateAccess = TRUE;
#ifdef BCM_ARAD_SUPPORT
        res = arad_fast_reg_get(unit, reg,  acc_type, addr, block, data);
#endif /*BCM_ARAD_SUPPORT*/

        Hw_Log_List[unit].ImmediateAccess = FALSE;

        if (SOC_FAILURE(res))
        {
            LOG_ERROR(BSL_LS_SOC_HWLOG,
                            (BSL_META_U(unit, "Unit %d: FAILED to FAST READ reg %d(%s)!\n"),
                             unit, reg, SOC_REG_NAME( unit, reg)));
        }
    }

    return res;

}
STATIC int
_soc_hw_log_soc_reg_set(int unit, soc_reg_t reg, int port, int index, uint64 data)
{
    LogEntry_t entry;

    entry.Data.SocRegData.Reg           = reg;
    entry.Data.SocRegData.Port          = port;
    entry.Data.SocRegData.Index         = index;
    entry.Type                          = LOG_ENTRY_TYPE_SocRegData;

    sal_memcpy(&entry.Data.SocRegData.EntryData, &data, sizeof(uint64));

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}

STATIC int
_soc_hw_log_soc_reg_get(int unit, soc_reg_t reg, int port, int index, uint64 *data)
{
    HwLogEntry_t   *entry  = NULL;
    uint32          i;
    int             found   = FALSE;
    soc_error_t     res     = SOC_E_NONE;

    if (!soc_hw_log_is_initialized(unit) || _soc_hw_log_log_list_is_empty(unit)) 
    {
        /* The List is not initialized or empty */
        goto not_found;
    }

    for (i=0 ; i < Hw_Log_List[unit].header->nof_elements; i++) {

        entry = &(Hw_Log_List[unit].journal[i]);

        if (entry->Entry.Type == LOG_ENTRY_TYPE_SocRegData)
        {
            if (       (reg         == entry->Entry.Data.SocRegData.Reg)
                          && ((port == entry->Entry.Data.SocRegData.Port) /* Exact match for lookup*/
                                    || (entry->Entry.Data.SocRegData.Port == REG_PORT_ANY)
                                    || (entry->Entry.Data.SocRegData.Port == SOC_CORE_ALL) /* Stored data is for all cores*/
                                    || (port == REG_PORT_ANY)
                                    || (port == SOC_CORE_ALL))    /*requested data is for all cores - we can return data from any port/core*/
                          && (index == entry->Entry.Data.SocRegData.Index))
            {
                sal_memcpy(data, &entry->Entry.Data.SocRegData.EntryData, sizeof(uint64));

                found = TRUE;
                
                LOG_DEBUG( BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "MEM FOUND in LOG!\n")) );
                HW_LOG_DEBUG(_soc_hw_log_print_list_memory_entry(unit, entry->Entry.Data));
            }
        }
    }

not_found:
    if (!found)
    {
        Hw_Log_List[unit].ImmediateAccess = TRUE;

        res = soc_reg_get(unit, reg, port, index, data);

        Hw_Log_List[unit].ImmediateAccess = FALSE;

        if (SOC_FAILURE(res))
        {
            LOG_ERROR(BSL_LS_SOC_HWLOG,
                            (BSL_META_U(unit, "Unit %d: FAILED to READ reg %d(%s) port %d index %d !\n"),
                             unit, reg, SOC_REG_NAME( unit, reg), port, index));
        }
    }

    return res;
}

STATIC int
_soc_hw_log_reg_set_nocache(int unit, soc_reg_t reg, int port, int index, uint64 data)
{
    LogEntry_t entry;
    entry.Data.SocRegNoCacheData.Reg            = reg;
    entry.Data.SocRegNoCacheData.Port           = port;
    entry.Data.SocRegNoCacheData.Index          = index;
    entry.Type                                  = LOG_ENTRY_TYPE_SocRegNoCacheData;

    sal_memcpy(&entry.Data.SocRegNoCacheData.EntryData, &data, sizeof(uint64));

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}

int
soc_hw_log_suppress(int unit)
{
    if (!SOC_UNIT_VALID(unit))
        return SOC_E_UNIT;

    /*activate immediate access*/
    Hw_Log_List[unit].ImmediateAccess = TRUE;
    LOG_DEBUG(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "--- HW LOG SUPPRESSED !!! --- : \n")));

    return SOC_E_NONE;
}

int
soc_hw_log_unsuppress(int unit)
{

    if (!SOC_UNIT_VALID(unit))
        return SOC_E_UNIT;

    /*activate immediate access*/
    Hw_Log_List[unit].ImmediateAccess = FALSE;
    LOG_DEBUG(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "--- HW LOG UNSUPPRESSED !!! --- : \n")));

    return SOC_E_NONE;
}
/********************************************************************************/
/***********************Diag & Tests*********************************************/
/********************************************************************************/
int
soc_hw_log_mem_test(int unit, soc_mem_t mem, void* data)
{
    if (!SOC_UNIT_VALID(unit))
        return SOC_E_NONE;

    soc_mem_single_test(unit, mem);

    return SOC_E_NONE;
}

int
soc_hw_log_reg_test(int unit)
{
    if (!SOC_UNIT_VALID(unit))
        return SOC_E_NONE;

    reg_test(unit, NULL, NULL); /* TR 3 */
    return SOC_E_NONE;
}

#endif /* CRASH_RECOVERY_SUPPORT */
