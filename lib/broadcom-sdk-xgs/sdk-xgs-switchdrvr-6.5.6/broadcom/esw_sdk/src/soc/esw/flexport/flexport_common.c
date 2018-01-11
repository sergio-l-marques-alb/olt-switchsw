/*
* $Id: $
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
* $File:  flexport_common.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#include <soc/flexport/flexport_common.h>

/*! @file flexport_common.c
 *   @brief FlexPort common (across chips) functions and structures.
 *   Details are shown below.
 */


/*! @fn int soc_detach_flex_phase(int unit, phase_callback_type *flex_phases,
 *                phase_callback_type phase, int *phase_pos)
 *   @param unit Chip unit number.
 *   @param flex_phases Pointer to the array holding the callbacks implementing
 *          FlexPort phases.
 *   @param phase The callback implementing the phase to be detached.
 *   @param phase_pos Position of the detached callback in the array is
 *          returned
 *          in this variable.
 *   @brief Detaches a phase from the FlexPort operations implemented with a
 *          callback function. Detaches the first instance of the callback.
 */
int
soc_detach_flex_phase (
    int                  unit,
    phase_callback_type *flex_phases,
    phase_callback_type  phase,
    int                 *phase_pos
    )
{
    int i;

    *phase_pos = -1;
    for (i = 0; i < MAX_FLEX_PHASES; i++) {
        if (flex_phases[i] == phase) {
            flex_phases[i] = NULL;
            *phase_pos = i;
            break;
        }
    }

    if (*phase_pos < 0) {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Phase not found, noting detached!\n")));
        return SOC_E_NONE;
    }

    return SOC_E_NONE;
}


/*! @fn int soc_attach_flex_phase(int unit, phase_callback_type *flex_phases,
 *               phase_callback_type phase, int phase_pos)
 *   @param unit Chip unit number.
 *   @param flex_phases Pointer to the array holding the callbacks implementing
 *          FlexPort phases.
 *   @param phase The callback implementing the phase to be attached.
 *   @param phase_pos Position where the callback be attached in the array.
 *   @brief Attaches a phase in the FlexPort operations implemented with a
 *          callback function.
 */
int
soc_attach_flex_phase (
    int                  unit,
    phase_callback_type *flex_phases,
    phase_callback_type  phase,
    int                  phase_pos
    )
{
    if (phase_pos < 0 || phase_pos >= MAX_FLEX_PHASES) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                       "Illegal phase position; "
                       "must be in the 0 .. %0d range!\n"),
                   MAX_FLEX_PHASES-1));
        return SOC_E_FAIL;
    }

    flex_phases[phase_pos] = phase;

    return SOC_E_NONE;
}


/*! @fn int soc_check_flex_phase(int unit, phase_callback_type *flex_phases,
 *               phase_callback_type phase, int *present)
 *   @param unit Chip unit number.
 *   @param flex_phases Pointer to the array holding the callbacks implementing
 *          FlexPort phases.
 *   @param phase The presence of callback phase to be checked.
 *   @param present Set to 1 if the callback exists, 0 otherwise.
 *   @brief Checks whether a phase callback function exists in the phase array.
 */
int
soc_check_flex_phase (
    int                  unit,
    phase_callback_type *flex_phases,
    phase_callback_type  phase,
    int                 *present
    )
{
    int i;

    *present = -1;
    for (i = 0; i < MAX_FLEX_PHASES; i++) {
        if (flex_phases[i] == phase) {
            *present = 1;
            break;
        }
    }

    if (*present < 0) {
        *present = 0;
    }

    return SOC_E_NONE;
}
