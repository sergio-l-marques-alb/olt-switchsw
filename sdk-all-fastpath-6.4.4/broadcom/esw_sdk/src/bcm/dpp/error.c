/*
 * $Id: error.c,v 1.8 Broadcom SDK $
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
 * Module: Error translation
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COMMON

#include <shared/bsl.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/TMC/tmc_api_framework.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/debug.h>

int
translate_sand_success_failure(const SOC_SAND_SUCCESS_FAILURE result)
{
    switch(result) {
        case SOC_SAND_SUCCESS:
            return BCM_E_NONE;
            
        case SOC_SAND_FAILURE_OUT_OF_RESOURCES:
        case SOC_SAND_FAILURE_OUT_OF_RESOURCES_2:
        case SOC_SAND_FAILURE_OUT_OF_RESOURCES_3:
            return BCM_E_FULL;
            
        case SOC_SAND_FAILURE_REMOVE_ENTRY_FIRST:
            return BCM_E_EXISTS;
            
        case SOC_SAND_FAILURE_INTERNAL_ERR:
            return BCM_E_INTERNAL;

        case SOC_SAND_FAILURE_UNKNOWN_ERR:
        default:
             break;
    }
    return BCM_E_PARAM; 
}

int
handle_sand_result(uint32 soc_sand_result)
{
#if BROADCOM_DEBUG
    uint32 proc_id, err_id;
    char *err_name, *err_text, *soc_sand_proc_name, *soc_sand_module_name;
#endif /* BROADCOM_DEBUG */
    uint16 error_code;

    error_code = soc_sand_get_error_code_from_error_word(soc_sand_result);
  
    if (error_code != SOC_SAND_OK) {
#if BROADCOM_DEBUG
        err_id = soc_sand_error_code_to_string(error_code, &err_name,&err_text) ;

        if (soc_sand_get_error_code_from_error_word(err_id) != SOC_SAND_OK) {
            err_text = "No error code description (or procedure id) found" ;
        }

        proc_id = soc_sand_get_proc_id_from_error_word(soc_sand_result) ;
        soc_sand_proc_id_to_string((unsigned long)proc_id,&soc_sand_module_name,&soc_sand_proc_name)  ;

        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META("SOC_SAND Error Code 0x%X (fail):\r\n"
                            "Text : %s\n\r" /*Error name*/
                            "%s\r\n"        /*Error description*/
                            "Procedure id: 0x%04X (Mod: %s, Proc: %s)\n\r"),
                   err_id,
                   err_name,
                   err_text,
                   proc_id,
                   soc_sand_module_name,
                   soc_sand_proc_name));
#endif /* BROADCOM_DEBUG */        
        
        /* map soc error to BCM error */        
        switch (error_code) {
        case (uint16) SOC_PPD_ERR_OUT_OF_RESOURCES:
            return BCM_E_RESOURCE;
        case (uint16) SOC_PPD_ERR_NOT_EXIST:
            return BCM_E_NOT_FOUND;
        case (uint16) SOC_TMC_INPUT_OUT_OF_RANGE:
            return BCM_E_PARAM;
        case (uint16) SOC_TMC_CONFIG_ERR:
            return BCM_E_CONFIG;
        case (uint16) ARAD_CELL_NO_RECEIVED_CELL_ERR:
            return BCM_E_EMPTY;
        default:
            return BCM_E_INTERNAL;        
        }
    }
    
    return BCM_E_NONE;
}
