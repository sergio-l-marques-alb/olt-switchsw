/*
 * $Id: hw_log.h,v 1 2014/09/22 12:12:10 Miry Heller Exp $
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
 * Crash Recovery Mechanism.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */

#ifndef _HW_LOG_H
#define _HW_LOG_H

#ifdef CRASH_RECOVERY_SUPPORT
#include <sal/core/thread.h>
#include <sal/core/time.h>
#include <soc/register.h>

extern int
soc_hw_log_commit_after_each_api(int unit);

extern int
soc_hw_log_get_working_mode(int unit);

extern int
soc_hw_log_do_hwlog_read_write(int unit);
#endif /* CRASH_RECOVERY_SUPPORT */

extern int
soc_hw_log_suppress(int unit);

extern int
soc_hw_log_unsuppress(int unit);

#if defined(BCM_ARAD_SUPPORT) && defined(CRASH_RECOVERY_SUPPORT)
#define         BCM_UNIT_HW_COMMIT(unit)                        soc_hw_log_commit_after_each_api(unit);
#define         BCM_UNIT_DO_HW_READ_WRITE(unit)                 soc_hw_log_do_hwlog_read_write(unit)
#else
#define         BCM_UNIT_HW_COMMIT(unit)
#define         BCM_UNIT_DO_HW_READ_WRITE(unit)                 (0)
#endif

#ifdef CRASH_RECOVERY_SUPPORT
typedef enum HwLogMode
{
    HW_LOG_MODE_Disabled = 0,
    HW_LOG_MODE_Commit_After_Each_Api,
    HW_LOG_MODE_Commit_Per_User_Demand
} HwLogMode_t;

typedef enum LogEntry_Type
{
    LOG_ENTRY_TYPE_UnKnown = 0,
    LOG_ENTRY_TYPE_Memory,
    LOG_ENTRY_TYPE_Register32,
    LOG_ENTRY_TYPE_Register64,
    LOG_ENTRY_TYPE_Register_Above64,
    LOG_ENTRY_TYPE_Polling,
    LOG_ENTRY_TYPE_FastRegData,
    LOG_ENTRY_TYPE_DirectRegWrite,
    LOG_ENTRY_TYPE_SocRegData,
    LOG_ENTRY_TYPE_SocRegNoCacheData
} LogEntry_Type_t;


typedef struct MemData
{
    soc_mem_t               Mem;
    uint32                  ArrayIndex;
    int                     Blk /*(copyno)*/;
    int                     Index;
    char                    EntryData[SOC_REG_ABOVE_64_MAX_SIZE_U32 * sizeof(int)];
} MemData_t;

typedef struct RegData
{
    soc_reg_t               Reg;
    int                     Port;
    int                     Index;
    soc_reg_above_64_val_t  EntryData;
} RegData_t;

typedef struct PollingData
{
    sal_usecs_t             TimeOut;
    int32                   MinPolls;
    soc_reg_t               Reg;
    int32                   Port;
    int32                   Index;
    soc_field_t             Field;
    int32                   ExpectedValue;
}
PollingData_t;

typedef struct DirectReg_Data
{
    int                     CmicBlock;
    uint32                  Addr;
    uint32                  DwcWrite;
    uint32                  EntryData;
}
DirectRegData_t;

typedef struct FastReg_Data
{
    soc_reg_t               Reg;
    int                     AccType;
    int                     Addr;
    int                     Block;
    soc_reg_above_64_val_t  EntryData;
}
FastReg_Data_t;

typedef struct SocReg_Data
{
    soc_reg_t               Reg;
    int                     Port;
    int                     Index;
    uint64                  EntryData;
}
SocReg_Data_t;

typedef union LogEntry_Data
{
    MemData_t               MemData;
    RegData_t               RegData;
    PollingData_t           PollingData;
    DirectRegData_t         DirectRegData;
    FastReg_Data_t          FastRegData;
    SocReg_Data_t           SocRegData;
    SocReg_Data_t           SocRegNoCacheData;
} LogEntry_Data_t;

typedef struct LogEntry
{
    LogEntry_Type_t         Type;
    LogEntry_Data_t         Data;
} LogEntry_t;

typedef struct HwLogEntry
{
    LogEntry_t              Entry;
    struct HwLogEntry       *Next;
} HwLogEntry_t;

typedef struct LogList
{
    int                 Unit;
    sal_thread_t        TID;
    HwLogEntry_t        *Head;
    HwLogEntry_t        *Current;
    HwLogEntry_t        *End;
    uint32              IsEmpty;
    uint32              EntriesCounter;
    uint32              NumOfElements;
    HwLogMode_t         WorkingMode;
    uint32              HwLogSize;
    int                 ImmediateAccess;    /* Bypass Hw Log */
    soc_hw_log_access_t Access_cb;          /* Access callbacks*/
} LogList_t;

extern int
soc_hw_log_init(int unit);

extern int
soc_hw_log_deinit(int unit);

extern int
soc_hw_log_commit(int unit);

extern int
soc_hw_log_purge(int unit);

extern int
soc_hw_log_diagshow(int unit);

extern int
soc_hw_log_register_callbacks(int unit);

extern int
soc_hw_log_mem_test(int unit, soc_mem_t mem, void* data);

extern int
soc_hw_log_print_list(int unit);

extern int
soc_hw_log_reg_test(int unit);

extern int
soc_mem_single_test(int unit, soc_mem_t mem);

extern int
soc_hw_log_is_enabled(int unit);

extern void
soc_hw_set_immediate_hw_access(int unit, int *access);

extern void
soc_hw_restore_immediate_hw_access(int unit, int access);

#endif 
#endif /*_HW_LOG_H*/
