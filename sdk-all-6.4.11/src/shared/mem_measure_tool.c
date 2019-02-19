/*
 * $Id: mem_measure_tool.c,v 1.0 Broadcom SDK $
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
 * This file contains the memory measurement tool implementation.
 */

#include <shared/mem_measure_tool.h>

#include <shared/error.h>

int memory_measurement_tool_initialized = 0;
memory_measurement_tool_t memory_measurement_tool;

int memory_consumption_start_measurement(char *str)
{
    int fnd = 0;
    int idx;
    for(idx = 0;idx < memory_measurement_tool.count;idx++) { 
        if(sal_strcmp(memory_measurement_tool.elements[idx].id, str) == 0) {
            if(memory_measurement_tool.elements[idx].is_active) {
                return _SHR_E_EXISTS;
            }
            memory_measurement_tool.elements[idx].is_active = 1;
            fnd = 1; 
            break; 
        } 
    } 
    if(!fnd) {
        if(memory_measurement_tool.count + 1 > MEMORY_MEASUREMENT_ELEMENTS_MAX_NUM) {
            return _SHR_E_MEMORY;
        }
        memory_measurement_tool.elements[memory_measurement_tool.count].is_active = 1; 
        memory_measurement_tool.elements[memory_measurement_tool.count].sal_size = 0; 
        memory_measurement_tool.elements[memory_measurement_tool.count].sw_state_size = 0;
        sal_strncpy(memory_measurement_tool.elements[memory_measurement_tool.count].id, str, MEMORY_MEASUREMENT_ID_MAX_LENGTH - 1); 
        memory_measurement_tool.elements[memory_measurement_tool.count].thread_id = sal_thread_self(); 
        memory_measurement_tool.count++; 
    }
    return _SHR_E_NONE;
}

int memory_consumption_end_measurement(char *str)
{
    int idx;
    int fnd = 0;
    for(idx = 0;idx < memory_measurement_tool.count;idx++) {
        if((sal_strcmp(memory_measurement_tool.elements[idx].id, str) == 0) && (memory_measurement_tool.elements[idx].thread_id == sal_thread_self())) {
            fnd = 1;
            memory_measurement_tool.elements[idx].is_active = 0;
            if(idx != (memory_measurement_tool.count - 1)) {
                memory_measurement_tool.elements[idx] = memory_measurement_tool.elements[memory_measurement_tool.count - 1];
                memory_measurement_tool.count--;
            }
            break; 
        } 
    } 
    if (fnd != 1) {
        return _SHR_E_NOT_FOUND;
    }
    return _SHR_E_NONE;
}

void memory_measurement_tool_init(void)
{
    memory_measurement_tool.count = 0;
}

int memory_measurement_print(char *str)
{
#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS_PRINT
    int fnd = 0;
    int idx;
    for(idx = 0;idx < memory_measurement_tool.count;idx++) {
        if(sal_strcmp(memory_measurement_tool.elements[idx].id, str) == 0) {
            fnd = 1;
            cli_out(" Str = %s\n is_active = %d\n sal_size = %u sw_state_size = %u\n\n", 
                    memory_measurement_tool.elements[idx].id, memory_measurement_tool.elements[idx].is_active, 
                    memory_measurement_tool.elements[idx].sal_size, memory_measurement_tool.elements[idx].sw_state_size);           
        }
    }
    if (fnd != 1) {
        return _SHR_E_NOT_FOUND;
    }
    return _SHR_E_NONE;
#else
    return _SHR_E_DISABLED;
#endif
}
