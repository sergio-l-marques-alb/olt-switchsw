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
 * File: qax_pp_trap.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_TRAP

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/mbcm_pp.h>

#include <soc/dpp/PPC/ppc_api_general.h>

#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_eg_mirror.h>

#include <soc/dpp/QAX/QAX_PP/qax_pp_trap.h>
#include <soc/dpp/dpp_wb_engine.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */



/* } */
/*************
 * MACROS    *
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

/************************************
 * Static declerations
 ************************************/

/************************************/
/*****************************************************************************
* Function:  _soc_qax_pp_trap_ser_trap_code_set
* Purpose:   Set the trap code for bcmRxTrapSer (trap code = HW id =  index in IHB_FWD_ACT_PROFILE)
* Params:
* unit     (IN)   -  Device Number
* trap_hw_id (IN) - HW id =  index in IHB_FWD_ACT_PROFILE = trap code
* Return:    (int)
*******************************************************************************/
STATIC soc_error_t _soc_qax_pp_trap_ser_trap_code_set(int unit,int trap_hw_id)
{
   uint32 regData = 0;
   int rv;
   SOCDNX_INIT_FUNC_DEFS;


    /*Get register of action profile of the SER trap*/
    rv = soc_reg32_get(unit,IHB_ACTION_PROFILE_PACKET_HAS_SERr,REG_PORT_ANY,0,&regData);
    SOCDNX_IF_ERR_EXIT(rv);
    
    /*Set the filed of trap code  = HW id =  index in IHB_FWD_ACT_PROFILE */
    soc_reg_field_set(unit,IHB_ACTION_PROFILE_PACKET_HAS_SERr,&regData,
                        ACTION_PROFILE_PACKET_HAS_SER_CPU_TRAP_CODEf,trap_hw_id);
    
    /*Write data back to register*/
    rv =soc_reg32_set(unit,IHB_ACTION_PROFILE_PACKET_HAS_SERr,REG_PORT_ANY,0,regData);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_EXIT;; 
exit:
    SOCDNX_FUNC_RETURN;
}

/*****************************************************************************
* Function:  _soc_qax_pp_trap_ser_set_unset
* Purpose:   Set the enable bit for bcmRxTrapSer
* Params:
* unit    (IN)   -  Device Number
* enable  (IN) - TRUE to enable and FALSE to disable 
* Return:    (int)
*******************************************************************************/
STATIC soc_error_t _soc_qax_pp_trap_ser_set_unset(int unit,int enable)
{
   uint32 regData = 0;
   int rv;
   SOCDNX_INIT_FUNC_DEFS;

   /*Get register which holds the bit to enable SER trap*/
    rv = soc_reg32_get(unit,IHB_FER_GENERAL_CONFIGURATIONSr,REG_PORT_ANY,0,&regData);
    SOCDNX_IF_ERR_EXIT(rv);
    
    /*Enable/Disable trap code */
    soc_reg_field_set(unit,IHB_FER_GENERAL_CONFIGURATIONSr,&regData,
                        ENABLE_TRAP_SER_PACKETf,enable);
    
    /*Write data back to register*/
    rv = soc_reg32_set(unit,IHB_FER_GENERAL_CONFIGURATIONSr,REG_PORT_ANY,0,regData);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_EXIT;
exit:
    SOCDNX_FUNC_RETURN;
}


/*****************************************************************************
* Function:  soc_qax_pp_trap_ingress_ser_set
* Purpose:   Set the trap code and enable or disable ingress ser trap
* Params:
* unit       - Device Number
* enable     - True for enable False for Disable
* trap_hw_id - HW id =  index in IHB_FWD_ACT_PROFILE = trap code
* Return:    (soc_error_t)
*******************************************************************************/
soc_error_t soc_qax_pp_trap_ingress_ser_set(int unit,int enable,int trap_hw_id)
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = _soc_qax_pp_trap_ser_set_unset(unit,enable);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = _soc_qax_pp_trap_ser_trap_code_set(unit,trap_hw_id);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

