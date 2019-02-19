/*
 * $Id: t3p1_cmu_sim.c,v 1.1 Broadcom SDK $
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
 * t3p1_cmu_sim.c: Guadalupe2k V1.3 CMU sim
 *
 */
 
#include <shared/bsl.h>

#include <sal/core/libc.h> 
#include <soc/types.h>
#include <soc/drv.h>

#if defined(BCM_CALADAN3_SUPPORT) && defined(BCM_CALADAN3_T3P1_SUPPORT)
#include <stdlib.h>

#include <soc/sbx/t3p1/t3p1_int.h>
#include <soc/sbx/t3p1/t3p1_tmu.h>
#include <soc/sbx/t3p1/t3p1_cmu_sim.h>
#include <soc/sbx/caladan3/simintf.h>

static char buffer[MAX_BUFFER_SIZE];

int soc_sbx_t3p1_cmu_counter_read_ext(int unit, int segment, int start, int numCounters, uint64 *counters) {
    int nextPrintPos = 0;
    int bufferSizeRemain = MAX_BUFFER_SIZE;
    int numCharPrinted;
    int i, rv, status;
    char *token;
    char *delimiters = " \t";
    uint64 cntr;
    char *tokstr=NULL;
        
    numCharPrinted = sal_snprintf(&buffer[nextPrintPos], bufferSizeRemain, "cmu read %d %d %d", segment, start, numCounters);
    nextPrintPos += numCharPrinted;
    bufferSizeRemain -= numCharPrinted;
    if (bufferSizeRemain == 0) return SOC_E_PARAM;
 
    rv = soc_sbx_caladan3_sim_sendrcv(unit, buffer, &nextPrintPos);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "sim sendrcv failed (%d)\n"), rv));
        return rv;
    }
    
    token = sal_strtok_r(buffer, delimiters, &tokstr);
    if ((token == NULL) || (sal_strcasecmp(token, "status"))){
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Recv format unrecognized: %s\n"), buffer));
        return SOC_E_PARAM;
    }
    token = sal_strtok_r(NULL, delimiters, &tokstr);
    if (token == NULL){
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Missing status value\n")));
        return SOC_E_PARAM;
    }    
    status = _shr_ctoi(token);
    if (status != 0) {
        return SOC_E_NOT_FOUND;
    }
    
    token = sal_strtok_r(NULL, delimiters, &tokstr);
    if ((token == NULL) || (sal_strcasecmp(token, "result"))){
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Missing result token\n")));
        return SOC_E_PARAM;
    }
    
    /* Simulator sends pkt count and byte counter */
    for (i=0; i<2*numCounters; i++) {
        token = sal_strtok_r(NULL, delimiters, &tokstr);
        if (token == NULL){
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Missing counter %d\n"), i));
            return SOC_E_PARAM;
        }
     
        COMPILER_64_SET(cntr,0, strtoul(token, NULL, 0));
        counters[i] = cntr;
    }
    
    return SOC_E_NONE;
}

int soc_sbx_t3p1_cmu_counter_clear_ext(int unit, int segment)  {
   int nextPrintPos = 0;
    int bufferSizeRemain = MAX_BUFFER_SIZE;
    int numCharPrinted;
    int rv, status;
    char *token;
    char *delimiters = " \t";
    char *tokstr=NULL;
      
    numCharPrinted = sal_snprintf(&buffer[nextPrintPos], bufferSizeRemain, "cmu clear %d", segment);
    nextPrintPos += numCharPrinted;
    bufferSizeRemain -= numCharPrinted;
    if (bufferSizeRemain == 0) return SOC_E_PARAM;
 
    rv = soc_sbx_caladan3_sim_sendrcv(unit, buffer, &nextPrintPos);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "sim sendrcv failed (%d)\n"), rv));
        return rv;
    }
    
    token = sal_strtok_r(buffer, delimiters, &tokstr);
    if ((token == NULL) || (sal_strcasecmp(token, "status"))){
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Recv format unrecognized: %s\n"), buffer));
        return SOC_E_PARAM;
    }
    token = sal_strtok_r(NULL, delimiters, &tokstr);
    if (token == NULL){
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Missing status value\n")));
        return SOC_E_PARAM;
    }    
    status = _shr_ctoi(token);
    if (status != 0) {
        return SOC_E_NOT_FOUND;
    }
     
    return SOC_E_NONE;
}
  
#endif
