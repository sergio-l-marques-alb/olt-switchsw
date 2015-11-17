/*
 * $Id: fe3200_stack.c,v 1.3.164.1 Broadcom SDK $
 *
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
 * SOC FE3200 STACK
 */
 
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <soc/dfe/cmn/dfe_stack.h>
#include <soc/dfe/fe3200/fe3200_stack.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_STAT


#if defined(BCM_88950_A0)


/*
 * Function:
 *      soc_fe3200_stk_modid_set
 * Purpose:
 *      Set the module ID of the local device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      fe_id - (IN)     FE id  
 * Returns:
 *      SOC_E_XXX      Error occurred  
 */
soc_error_t 
soc_fe3200_stk_modid_set(int unit, uint32 fe_id)
{
    uint32 reg_val32;
    SOCDNX_INIT_FUNC_DEFS;
        
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg_val32));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg_val32, DEV_ID_0f, fe_id);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg_val32));
   
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe3200_stk_modid_get
 * Purpose:
 *      Get the module ID of the local device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      fe_id - (OUT)    FE id  
 * Returns:
 *      SOC_E_XXX      Error occurred  
 */
soc_error_t 
soc_fe3200_stk_modid_get(int unit, uint32* fe_id)
{
    uint32 reg_val32;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg_val32));
    *fe_id = soc_reg_field_get(unit, ECI_GLOBAL_GENERAL_CFG_2r, reg_val32, DEV_ID_0f);
    
exit:
    SOCDNX_FUNC_RETURN;
  
}

soc_error_t
soc_fe3200_stk_valid_module_id_verify(int unit, bcm_module_t module, int verify_divisble_by_32)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (module > SOC_FE3200_STK_MAX_MODULE)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid module id , above max\n")));
    }

    if (module < 0)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid module id , below zero \n")));
    }

    if (verify_divisble_by_32 && (module % SOC_FE3200_STK_FAP_GROUP_SIZE != 0))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid module id , must be divisible by 32 \n")));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_max_all_reachable_verify(int unit, bcm_module_t module)
{
    uint32 field_val;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;

    /* Verify validity of module id */

    SOCDNX_IF_ERR_EXIT(soc_fe3200_stk_valid_module_id_verify(unit, module, 1));

    /* Verify that AlrcMaxBaseIndex <= UpdateBaseIndex */

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, &reg_val64));
    field_val = soc_reg64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val64, UPDATE_BASE_INDEXf);
    if ((module/SOC_FE3200_STK_FAP_GROUP_SIZE) > field_val) /* AlrcMaxBaseIndex to be configured is > UpdateBaseIndex */
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Max Fap Id for all reachable vector can't be higher than Max number of Faps in the system configured \n")));
    }
       
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_max_all_reachable_set(int unit, bcm_module_t module)
{
    uint32 fap_group_num = 0, reg_val32;
    SOCDNX_INIT_FUNC_DEFS;

    fap_group_num = module / SOC_FE3200_STK_FAP_GROUP_SIZE;

    SOCDNX_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_CFGr(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_ALL_REACHABLE_CFGr, &reg_val32, ALRC_MAX_BASE_INDEXf, fap_group_num);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_CFGr(unit, reg_val32));
    
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_max_all_reachable_get(int unit, bcm_module_t *module)
{
    uint32 reg_val32, field_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_CFGr(unit, &reg_val32));
    field_val = soc_reg_field_get(unit, RTP_ALL_REACHABLE_CFGr, reg_val32, ALRC_MAX_BASE_INDEXf);

    *module = SOC_FE3200_STK_FAP_GROUP_SIZE * field_val;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_max_fap_verify(int unit, bcm_module_t module)
{
    uint32 reg_val32, field_val;

    SOCDNX_INIT_FUNC_DEFS;

    /* Verify validity of module id */

    SOCDNX_IF_ERR_EXIT(soc_fe3200_stk_valid_module_id_verify(unit, module, 1));

    /* Verify that AlrcMaxBaseIndex <= UpdateBaseIndex */

    SOCDNX_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_CFGr(unit, &reg_val32));
    field_val = soc_reg_field_get(unit, RTP_ALL_REACHABLE_CFGr, reg_val32, ALRC_MAX_BASE_INDEXf);
    if (field_val > (module/SOC_FE3200_STK_FAP_GROUP_SIZE)) /* UpdateBaseIndex to be configured is  < AlrcMaxBaseIndex */
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Max Fap Id for all reachable vector can't be higher than Max number of Faps in the system configured \n")));
    }
          
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_max_fap_set(int unit, bcm_module_t module)
{
    uint64 reg_val64;
    uint32 fap_group_num = 0;
    SOCDNX_INIT_FUNC_DEFS;

    fap_group_num = module / SOC_FE3200_STK_FAP_GROUP_SIZE;

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, &reg_val64));
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, UPDATE_BASE_INDEXf, fap_group_num);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_val64));
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_max_fap_get(int unit, bcm_module_t *module)
{
    uint64 reg_val64;
    uint32 field_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, &reg_val64));
    field_val = soc_reg64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val64, UPDATE_BASE_INDEXf);

    *module = SOC_FE3200_STK_FAP_GROUP_SIZE * field_val;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stk_module_all_reachable_ignore_id_set(int unit, bcm_module_t module, int arg)
{
    int i, first_empty_cell = -1; 
    uint32 reg_val, valid_field, id_field;
    SOCDNX_INIT_FUNC_DEFS;

    if (module > SOC_FE3200_STK_MAX_MODULE)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid module id , above max\n")));
    }

    for (i=0 ; i < SOC_FE3200_STK_MAX_IGNORED_FAP_IDS ; i++) /* Loop over all ignoring register */
    {
        SOCDNX_IF_ERR_EXIT(READ_RTP_ALRC_EXCLUDEr(unit, i, &reg_val));
        id_field = soc_reg_field_get(unit, RTP_ALRC_EXCLUDEr, reg_val, ALRC_EXCLUDE_N_DEVICEf);
        valid_field = soc_reg_field_get(unit, RTP_ALRC_EXCLUDEr, reg_val, ALRC_EXCLUDE_N_VALIDf);

        if (valid_field == 1) /* The register is holding a fap id which is ignored */
        {
            if (id_field == module) 
            {
                if (arg == 0) /* need to clear this register */ 
                {
                    soc_reg_field_set(unit, RTP_ALRC_EXCLUDEr, &reg_val, ALRC_EXCLUDE_N_DEVICEf, 0);
                    soc_reg_field_set(unit, RTP_ALRC_EXCLUDEr, &reg_val, ALRC_EXCLUDE_N_VALIDf, 0);
                    SOCDNX_IF_ERR_EXIT(WRITE_RTP_ALRC_EXCLUDEr(unit, i, reg_val));
                    SOC_EXIT;  /* the way we implemented, each fap id will be ignored only once, so we can exit safely without worrying that an additional register will ignore this id*/
                }
                else /* fap id is already ignored, no need to reignore it*/
                {
                    SOC_EXIT; 
                }
            }
        }
        else if (first_empty_cell == -1 && arg == 1) /* Haven't found a place in the array to configure the module id*/
        {
            first_empty_cell = i;
        }
    }

    if (arg == 0) /* there was nothing to clear */
    {
        SOC_EXIT;
    }
    
    if (first_empty_cell == -1) /* No empty place to ignore the new fap id*/
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Can't ignore this FAP ID - 16 FAP IDs are already ignored \n")));
    }
    else /* Ignore FAP ID*/
    {
        SOCDNX_IF_ERR_EXIT(READ_RTP_ALRC_EXCLUDEr(unit, first_empty_cell, &reg_val));
        soc_reg_field_set(unit, RTP_ALRC_EXCLUDEr, &reg_val, ALRC_EXCLUDE_N_DEVICEf, module);
        soc_reg_field_set(unit, RTP_ALRC_EXCLUDEr, &reg_val, ALRC_EXCLUDE_N_VALIDf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_ALRC_EXCLUDEr(unit, first_empty_cell, reg_val));
    }
exit:
    SOCDNX_FUNC_RETURN;

}

soc_error_t
soc_fe3200_stk_module_all_reachable_ignore_id_get(int unit, bcm_module_t module, int *arg)
{
    int i, found = 0;
    uint32 reg_val, id_field, valid_field;
    SOCDNX_INIT_FUNC_DEFS;

    for (i=0 ; i < SOC_FE3200_STK_MAX_IGNORED_FAP_IDS ; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_RTP_ALRC_EXCLUDEr(unit, i, &reg_val));
        id_field = soc_reg_field_get(unit, RTP_ALRC_EXCLUDEr, reg_val, ALRC_EXCLUDE_N_DEVICEf);
        valid_field = soc_reg_field_get(unit, RTP_ALRC_EXCLUDEr, reg_val, ALRC_EXCLUDE_N_VALIDf);

        if (valid_field == 1 && id_field == module)
        {
            found = 1;
            break;
        }
    }

    *arg = found;

exit:
    SOCDNX_FUNC_RETURN;
}



#endif

#undef _ERR_MSG_MODULE_NAME

