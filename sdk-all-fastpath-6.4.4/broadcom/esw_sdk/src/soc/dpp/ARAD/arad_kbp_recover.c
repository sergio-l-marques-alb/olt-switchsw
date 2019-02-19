/* $Id: arad_kbp_recover.c,v 1.50 Exp $
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
*/

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/phyreg.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/arad_kbp_rop.h>
#include <soc/dpp/ARAD/arad_kbp_xpt.h>
#include <soc/dpp/ARAD/arad_kbp_recover.h>

/*************
 * DEFINES   *
 *************/
#define ARAD_KBP_RECOVER_KBP_REG_ADDR   0X40000

/*************
 *  MACROS   *
 *************/


/*************
 * TYPE DEFS *
 *************/


/*************
 * GLOBALS   *
 *************/

extern int arad_kbp_init_rop_test(int unit);


int arad_kbp_recover_rx_enable(int unit, int mdio_id)
{
    int i = 0;
    uint32 flags = SOC_PHY_NOMAP | SOC_PHY_CLAUSE45;
    uint32 kbp_reg_addr, data;
    uint32 port = mdio_id;
    SOCDNX_INIT_FUNC_DEFS;

	kbp_reg_addr =  0X40000;

	data = 0;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    
	SHR_BITSET(&data,0);
	SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, port, flags, kbp_reg_addr, data));
    
    kbp_reg_addr = 0x40021;
	/* Check RX link status is stable */
	while (data != 0x8000 && i < 1000){
		data = 0;
		SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
        ++i;
 	}
	
exit:
    SOCDNX_FUNC_RETURN;
}

int arad_kbp_recover_rx_shut_down(int unit, int mdio_id)
{
    uint32 flags = SOC_PHY_NOMAP | SOC_PHY_CLAUSE45;
    uint32 kbp_reg_addr, data;
    uint32 port = mdio_id;
    SOCDNX_INIT_FUNC_DEFS;

    kbp_reg_addr =  0x40000;

	data = 0;
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_get(unit, port, flags, kbp_reg_addr, &data));
    
	SHR_BITCLR(&data ,0);
    SOCDNX_IF_ERR_EXIT(arad_port_phy_reg_set(unit, port, flags, kbp_reg_addr, data));
   
exit:
    SOCDNX_FUNC_RETURN;
}

/* Recover the KBP  */ /* mdio_id = 257    */
int arad_kbp_recover_run_recovery_sequence(int unit, int mdio_id, uint32 retries)
{
    int i, j, rv;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i < retries; ++i) {       
        SOCDNX_IF_ERR_EXIT(arad_kbp_recover_rx_shut_down(unit, mdio_id));
        sal_usleep(10000);
        SOCDNX_IF_ERR_EXIT(arad_kbp_recover_rx_enable(unit, mdio_id));

        for (j = 0; j < retries; ++j) {
            SOCDNX_IF_ERR_EXIT(READ_IHB_LOOKUP_REPLYr_REG32(unit, 0, &reg_val));
            if (reg_val == 0) {
                break;
            }
        }
        /* Test the KBP */
        rv = arad_kbp_init_rop_test(unit);
        if(rv == SOC_E_NONE){
            /* KBP Recovery succeeded */ 
            if (i > 0) {
                LOG_INFO( BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "KBP recovery succeeded within %d iterations.\n"), i+1)); 
            }
            break;
        } else {
            if (i < retries - 1) {
                LOG_INFO( BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "KBP recovery iteration %d/%d failed, trying again.\n"), i + 1, retries));
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL,(_BSL_SOC_MSG("KBP recovery failed.\n")));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* #if defined(BCM_88650_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030) */

