/* 
 * $Id: dcmn_patch_database.c,v 1.2 Broadcom SDK $ 
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
 * File:        dcmn_patch_database.c
 *
 */



#include <shared/bsl.h>

#include <bcm/debug.h>

#ifdef BCM_DPP_SUPPORT
#include <bcm_int/dpp/error.h>
#endif

#include <appl/diag/dcmn/dcmn_patch_database.h>
#include <appl/diag/system.h>

#if 1
STATIC appl_diag_dcmn_patch patch_database[] = {
    {1, NULL},
    /*   
    *   patch1
    *   here
    */
    {2, NULL},
    /*   
    *   patch2
    *   here
    */
    {3, NULL},
    /*   
    *   patch3
    *   here
    */
    {4, NULL},
    /*   
    *   patch4
    *   here
    */
    {5, NULL},
    /*   
    *   patch5
    *   here
    */
    {6, NULL},
    /*   
    *   patch6
    *   here
    */  
    {7, NULL},
    /*   
    *   patch7
    *   here
    */
    {8, NULL},
    /*   
    *   patch8
    *   here
    */
    {9, NULL},
    /*   
    *   patch9
    *   here
    */
    {10, NULL},
    /*   
    *   patch10
    *   here
    */
    {11, NULL},
    /*   
    *   patch11
    *   here
    */
    {12, NULL},
    /*   
    *   patch12
    *   here
    */
    {13, NULL},
    /*   
    *   patch13
    *   here
    */
    {14, NULL},
    /*   
    *   patch14
    *   here
    */
    {15, NULL},
    /*   
    *   patch15
    *   here
    */
    {16, NULL},
    /*   
    *   patch16
    *   here
    */  
    {17, NULL},
    /*   
    *   patch17
    *   here
    */
    {18, NULL},
    /*   
    *   patch18
    *   here
    */
    {19, NULL},
    /*   
    *   patch19
    *   here
    */
    {20, NULL},
    /*   
    *   patch20
    *   here
    */
    {21, NULL},
    /*   
    *   patch21
    *   here
    */
    {22, NULL},
    /*   
    *   patch22
    *   here
    */
    {23, NULL},
    /*   
    *   patch23
    *   here
    */
    {24, NULL},
    /*   
    *   patch24
    *   here
    */
    {25, NULL},
    /*   
    *   patch25
    *   here
    */
    {26, NULL},
    /*   
    *   patch26
    *   here
    */  
    {27, NULL},
    /*   
    *   patch27
    *   here
    */
    {28, NULL},
    /*   
    *   patch28
    *   here
    */
    {29, NULL},
    /*   
    *   patch29
    *   here
    */
    {30, NULL},
    /*   
    *   patch30
    *   here
    */
    {31, NULL},
    /*   
    *   patch31
    *   here
    */
    {32, NULL},
    /*   
    *   patch32
    *   here
    */
    {33, NULL},
    /*   
    *   patch33
    *   here
    */
    {34, NULL},
    /*   
    *   patch34
    *   here
    */
    {35, NULL},
    /*   
    *   patch35
    *   here
    */
    {36, NULL},
    /*   
    *   patch36
    *   here
    */  
    {37, NULL},
    /*   
    *   patch37
    *   here
    */
    {38, NULL},
    /*   
    *   patch38
    *   here
    */
    {39, NULL},
    /*   
    *   patch39
    *   here
    */
    {40, NULL},
    /*   
    *   patch40
    *   here
    */
    {41, NULL},
    /*   
    *   patch41
    *   here
    */
    {42, NULL},
    /*   
    *   patch42
    *   here
    */
    {43, NULL},
    /*   
    *   patch43
    *   here
    */
    {44, NULL},
    /*   
    *   patch44
    *   here
    */
    {45, NULL},
    /*   
    *   patch45
    *   here
    */
    {46, NULL},
    /*   
    *   patch46
    *   here
    */  
    {47, NULL},
    /*   
    *   patch47
    *   here
    */
    {48, NULL},
    /*   
    *   patch48
    *   here
    */
    {49, NULL},
    /*   
    *   patch49
    *   here
    */
    {50, NULL},
    /*   
    *   patch50
    *   here
    */
};
#endif  
  
/*
* Function:
*      soc_dcmn_patches_installed_get
* Purpose:
*      Find all installed patches
* Parameters:
*      max_arraysize        - (IN)   Max number of installed patches
*      installed_patches    - (OUT)  All installed patches
*      num_of_installed     - (OUT)  Number of installed patches
* Returns:
*      VOID
*/
int
appl_diag_dcmn_patches_installed_get (int unit, int max_arraysize, appl_diag_dcmn_patch* installed_patches, int* num_of_installed)
{
    int index, rv = 0;


    if(NULL == installed_patches) {
        rv = -1;
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "installed_patches is NULL\n")));
        return rv;            
    }

    if(NULL == num_of_installed) {
        rv = -1;
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "installed_patches is NULL\n")));
        return rv;            
    }

    *num_of_installed = 0;

    for (index = 0; index < TOTAL_PATCHES; index++){
        if (patch_database[index].str != NULL){
            if ((*num_of_installed) == max_arraysize) {
                rv = -1;
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META_U(unit,
                                      "not enough room for all installed patches\n")));
                return rv;            
            }
            else {
                installed_patches[*num_of_installed].id  = patch_database[index].id;
                installed_patches[*num_of_installed].str = patch_database[index].str;
                (*num_of_installed)++;
            }
        }
    }

    return rv;
}

/* Get Current Version Insalled */
int
appl_diag_dcmn_version_get (int unit, char **version_pointer)
{
    int rv = 0;

    if(NULL == version_pointer) {
        rv = -1;
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "version_pointer is NULL\n")));
        return rv;            
    }

    *version_pointer = _build_release;

    return rv;
}

int appl_diag_dcmn_version_patch_get (int unit, 
                                int *version_patch)
{
    int index, rv = 0;

    if(NULL == version_patch) {
        rv = -1;
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "version_patch is NULL\n")));
        return rv;            
    }
    *version_patch = 0;
    for (index = 0; index < TOTAL_PATCHES; index++){               
        if (patch_database[index].str == NULL){
            break;
        }
        *version_patch = patch_database[index].id; 
        
    }
    return rv;


}
