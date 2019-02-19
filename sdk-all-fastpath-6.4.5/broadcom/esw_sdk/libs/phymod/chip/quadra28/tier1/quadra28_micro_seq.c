/*
*
* $Id: quadra28_micr0_seq.c, 2014/12/16 aman $
*
5 *
5 * $Copyright: Copyright 2012 Broadcom Corporation.
5 * This program is the proprietary software of Broadcom Corporation
5 * and/or its licensors, and may only be used, duplicated, modified
5 * or distributed pursuant to the terms and conditions of a separate,
5 * written license agreement executed between you and Broadcom
5 * (an "Authorized License").  Except as set forth in an Authorized
5 * License, Broadcom grants no license (express or implied), right
5 * to use, or waiver of any kind with respect to the Software, and
5 * Broadcom expressly reserves all rights in and to the Software
5 * and all intellectual property rights therein.  IF YOU HAVE
5 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
5 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
5 * ALL USE OF THE SOFTWARE.  
5 *  
5 * Except as expressly set forth in the Authorized License,
5 *  
5 * 1.     This program, including its structure, sequence and organization,
5 * constitutes the valuable trade secrets of Broadcom, and you shall use
5 * all reasonable efforts to protect the confidentiality thereof,
5 * and to use this information only in connection with your use of
5 * Broadcom integrated circuit products.
5 *  
5 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
5 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
5 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
5 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
5 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
5 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
5 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
5 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
5 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
5 * 
5 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
5 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
5 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
5 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
5 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
5 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
5 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
5 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
5 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
7  *
8  * File: quadra28.c
   * Purpose: tier1 phymod microcode download support for Broadcom 40G Quadra28 
   * note
*/

/*
 * Includes
 */
#include <phymod/phymod.h>
#include "quadra28_reg_access.h"
#include "quadra28_types.h"
#include "bcmi_quadra28_defs.h"
#include "quadra28_cfg_seq.h"

static int _quadra28_verify_fw_download(const phymod_access_t *pa);

/* If it is single PMD, we need to clear it before downloading FW*/
static int _quadra28_clear_mode_config(phymod_access_t *pa)
{
    int speed = 0;
    BCMI_QUADRA28_GENERAL_PURPOSE_3r_t gp_reg3_reg_val;
    BCMI_QUADRA28_APPS_MODE_0r_t apps_mode0_reg_val;
    BCMI_QUADRA28_PMD_CONTROLr_t  pmd_ctrl;
    PHYMOD_MEMSET(&gp_reg3_reg_val, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_3r_t));
    PHYMOD_MEMSET(&apps_mode0_reg_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_0r_t));
    PHYMOD_MEMSET(&pmd_ctrl, 0, sizeof(BCMI_QUADRA28_PMD_CONTROLr_t));

    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_PURPOSE_3r(pa, &gp_reg3_reg_val));
    speed = gp_reg3_reg_val.v[0] & 0xF ;
    if (speed == 0x4 || speed == 0x7) {
        PHYMOD_IF_ERR_RETURN(
            quadra28_channel_select (pa, Q28_ALL_LANE));
        apps_mode0_reg_val.v[0] = gp_reg3_reg_val.v[0];
        BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(apps_mode0_reg_val, 0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r(pa, apps_mode0_reg_val));
        PHYMOD_USLEEP(100);
        BCMI_QUADRA28_APPS_MODE_0r_SET(apps_mode0_reg_val, 0x8882);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r(pa, apps_mode0_reg_val));
        PHYMOD_USLEEP(100);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_PMD_CONTROLr(pa, &pmd_ctrl));
        BCMI_QUADRA28_PMD_CONTROLr_RESETf_SET(pmd_ctrl, 1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_PMD_CONTROLr(pa, pmd_ctrl));
         PHYMOD_IF_ERR_RETURN(
                 _quadra28_intf_update_wait_check(pa,apps_mode0_reg_val.v[0], 50000));
    }


    return PHYMOD_E_NONE;
}
/**   Download and Fuse firmware
 *    This function is used to download the firmware through I2C/MDIO
 *    and fuse it to SPI EEPROM if prg_eeprom flag is set
 *
 *    @param pa                 Pointer to phymod access structure
 *    @param new_fw             Pointer to firmware array
 *    @param fw_length          Length of the firmware array
 *    @param prg_eeprom         Flag used to program EEPROM
 *
 *    @return num_bytes         number of bytes successfully downloaded
 */
int quadra28_micro_download(const phymod_access_t *acc,
                                 uint8_t *new_fw,
                                 uint32_t fw_length,
                                 uint8_t prg_eeprom)
{
    uint32_t data16 = 0;
    uint32_t index = 0;
    uint16_t lane_map = 0;
 
    MDIO_BROADCAST_CONTROLr_t mdio_broadcast_ctrl_reg;
    GENERAL_CONTROL_1r_t general_ctrl_reg1;
    GEN_CTRLr_t general_ctrl_reg;
    SPI_PORT_CONTROL_STATUSr_t spi_port_ctrl_reg;
    MSG_INr_t msg_in_reg;
    CL73CONTROL8r_t cl73_ctrl8_reg;
    PMD_CONTROLr_t chip_reset_reg;
    GENERAL_CONTROL_2r_t general_ctrl_reg2;
    phymod_access_t pa;
    uint32_t phy_addr = acc->addr;
    /* Get the lane map from phymod access */
    lane_map = PHYMOD_ACC_LANE_MASK(acc);

    lane_map = (lane_map ? lane_map : 0xF); 

    PHYMOD_MEMCPY(&pa, acc, sizeof( phymod_access_t));
    PHYMOD_MEMSET(&cl73_ctrl8_reg, 0, sizeof(CL73CONTROL8r_t));
    PHYMOD_MEMSET(&mdio_broadcast_ctrl_reg, 0, sizeof(MDIO_BROADCAST_CONTROLr_t));
    PHYMOD_MEMSET(&general_ctrl_reg1, 0, sizeof(GENERAL_CONTROL_1r_t));
    PHYMOD_MEMSET(&general_ctrl_reg, 0, sizeof(GEN_CTRLr_t));
    PHYMOD_MEMSET(&spi_port_ctrl_reg, 0, sizeof(BCMI_QUADRA28_SPI_PORT_CONTROL_STATUSr_t));
    PHYMOD_MEMSET(&msg_in_reg, 0, sizeof(MSG_INr_t));
    PHYMOD_MEMSET(&chip_reset_reg, 0, sizeof(PMD_CONTROLr_t));
    PHYMOD_MEMSET(&general_ctrl_reg2, 0, sizeof(GENERAL_CONTROL_2r_t));
   

    PHYMOD_IF_ERR_RETURN(
        _quadra28_clear_mode_config(&pa));
    /* Step1: Enable broadcast mode */
    for (index = 0; index <= QUADRA28_MAX_LANE - 1; index++)
    {  
        pa.addr = (phy_addr + index);
        if (((lane_map >> index) & 1) == 0x1){
            PHYMOD_IF_ERR_RETURN
                (READ_MDIO_BROADCAST_CONTROLr(&pa,
                    &mdio_broadcast_ctrl_reg));
            MDIO_BROADCAST_CONTROLr_MDIO_BCAST_ENf_SET(mdio_broadcast_ctrl_reg, 1);     
            PHYMOD_IF_ERR_RETURN
                (WRITE_MDIO_BROADCAST_CONTROLr(&pa, mdio_broadcast_ctrl_reg));
        }
    } 
    pa.addr = phy_addr;
    
    /* Set 0xFFD1 to read from MGT-M8051 register */
    PHYMOD_IF_ERR_RETURN
        (READ_GENERAL_CONTROL_2r(&pa,&general_ctrl_reg2));
    GENERAL_CONTROL_2r_LEGACY_BOOT_SELf_SET(general_ctrl_reg2, 1);
    
    PHYMOD_IF_ERR_RETURN
        (WRITE_GENERAL_CONTROL_2r(&pa,general_ctrl_reg2));
    PHYMOD_MEMSET(&general_ctrl_reg2, 0, sizeof(GENERAL_CONTROL_2r_t)); 
  
    /* Step5: Select MDIO interface for microcode download */
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_READ_SPI_PORT_CONTROL_STATUSr(&pa, 
            &spi_port_ctrl_reg));

    BCMI_QUADRA28_SPI_PORT_CONTROL_STATUSr_SPI_PORT_USEDf_SET(spi_port_ctrl_reg, 0);                       
    BCMI_QUADRA28_SPI_PORT_CONTROL_STATUSr_SPI_BOOTf_SET(spi_port_ctrl_reg, 1);
    BCMI_QUADRA28_SPI_PORT_CONTROL_STATUSr_SPI_DWLD_DONEf_SET(spi_port_ctrl_reg, 0);
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_WRITE_SPI_PORT_CONTROL_STATUSr(&pa, 
            spi_port_ctrl_reg));    
 
    /* Step3: Reset MGT-M8051 uc at address 1.FFD0 */
    BCMI_QUADRA28_GENERAL_CONTROL_1r_SET(general_ctrl_reg1, 1);
    PHYMOD_IF_ERR_RETURN
        (WRITE_GENERAL_CONTROL_1r(&pa,
            general_ctrl_reg1));

    /* Step2: Reset EDc-M8051 uc at address 1.CA10 */
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_READ_GEN_CTRLr(&pa, &general_ctrl_reg));

    BCMI_QUADRA28_GEN_CTRLr_UCRSTf_SET(general_ctrl_reg, 1);

    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_WRITE_GEN_CTRLr(&pa, general_ctrl_reg));   
   
           
    /* Step8:  Release EDC-M8051 from reset */
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_READ_GEN_CTRLr(&pa, &general_ctrl_reg));
    BCMI_QUADRA28_GEN_CTRLr_UCRSTf_SET(general_ctrl_reg, 0);
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_WRITE_GEN_CTRLr(&pa, general_ctrl_reg));
    
    /* step9: Release MGT_M8051 uc from reset */    
    BCMI_QUADRA28_GENERAL_CONTROL_1r_SET(general_ctrl_reg1, 0);
    PHYMOD_IF_ERR_RETURN
        (WRITE_GENERAL_CONTROL_1r(&pa, general_ctrl_reg1));
   
    /* step10: wait 1 ms */
    PHYMOD_USLEEP(1000);

    /* step11: send initial RAM address */
    BCMI_QUADRA28_MSG_INr_SET (msg_in_reg, 0x8000);
    PHYMOD_IF_ERR_RETURN
       (BCMI_QUADRA28_WRITE_MSG_INr(&pa, msg_in_reg));

    /* step12: wait 15 us */
    PHYMOD_USLEEP(20);   
   
    /* step13: send microcode size to 1.ca12 */
    BCMI_QUADRA28_MSG_INr_MESSAGE_INf_SET(msg_in_reg,fw_length/2);
    PHYMOD_IF_ERR_RETURN
       (BCMI_QUADRA28_WRITE_MSG_INr(&pa, msg_in_reg));

    PHYMOD_USLEEP(20);
    /* step15 to step 16: send 16-bit microcode word */
    PHYMOD_IF_ERR_RETURN
            (BCMI_QUADRA28_READ_MSG_INr(&pa,
                &msg_in_reg)); 
    
    for (index = 0; index < (fw_length) - 1; index+=2){
        data16 = ((new_fw[index] << 8)| new_fw[index+1]);
        PHYMOD_USLEEP(15);
        BCMI_QUADRA28_MSG_INr_MESSAGE_INf_SET(msg_in_reg,data16);
        PHYMOD_IF_ERR_RETURN
            (BCMI_QUADRA28_WRITE_MSG_INr(&pa, msg_in_reg));

    }
    /* step17: wait 20 us */
    PHYMOD_USLEEP(20); 

    /* Do it from all three locations */
    /* if not read, they will be in wait and not update checksum */
    /* Q28 3 processors - flip FFD1 bits to read either from MGT-M8051 or EDC-M8051 */
    /* Verify for all processors - 3 in each */
    /* use phy address for other two EDC-M8051 lanes */
    for (index = 0; index <= (QUADRA28_MAX_LANE - 1); index += 2)
    {
         pa.addr = (phy_addr + index);
         if (((lane_map >> index) & 1) == 0x1) 
         {
            /* Verify checksum on MGT-M8051 for a phy lane */
             PHYMOD_IF_ERR_RETURN(_quadra28_verify_fw_download(&pa));
         }
    }
    pa.addr = phy_addr;
    PHYMOD_IF_ERR_RETURN
    (READ_GENERAL_CONTROL_2r(&pa,&general_ctrl_reg2));
   
    GENERAL_CONTROL_2r_LEGACY_BOOT_SELf_SET(general_ctrl_reg2, 0);

    PHYMOD_IF_ERR_RETURN
       (WRITE_GENERAL_CONTROL_2r(&pa,general_ctrl_reg2));        

    for (index = 0; index <= (QUADRA28_MAX_LANE - 1); index += 2) {
         pa.addr = (phy_addr + index);
         if (((lane_map >> index) & 1) == 0x1) 
         {
            PHYMOD_IF_ERR_RETURN(_quadra28_verify_fw_download(&pa));
         }
    }
    pa.addr = phy_addr;
    for (index = 0; index <= QUADRA28_MAX_LANE - 1; index++)
    {
        pa.addr = (phy_addr + index);
        if (((lane_map >> index) & 1) == 0x1) {
            PHYMOD_IF_ERR_RETURN
               (READ_MDIO_BROADCAST_CONTROLr(&pa, &mdio_broadcast_ctrl_reg));

            MDIO_BROADCAST_CONTROLr_MDIO_BCAST_ENf_SET(mdio_broadcast_ctrl_reg, 0);     
           
            PHYMOD_IF_ERR_RETURN 
                (WRITE_MDIO_BROADCAST_CONTROLr(&pa,
                    mdio_broadcast_ctrl_reg));
         }
     }
    return PHYMOD_E_NONE;

}

static int _quadra28_verify_fw_download(const phymod_access_t *pa)
{
    MSG_OUTr_t msg_out_reg;
    BOOT_CHECKSUMr_t checksum_reg;
    PHYMOD_MEMSET(&msg_out_reg, 0, sizeof(MSG_OUTr_t));
    PHYMOD_MEMSET(&checksum_reg, 0, sizeof(BOOT_CHECKSUMr_t));
    PHYMOD_IF_ERR_RETURN((BCMI_QUADRA28_READ_MSG_OUTr(pa,&msg_out_reg)));
    msg_out_reg.v[0] = MSG_OUTr_MESSAGE_OUTf_GET(msg_out_reg);
    
    if (0x4321 == msg_out_reg.v[0]){
       /* step19: Wait 400ms */
        PHYMOD_USLEEP(400000);
        
    } else {
       return PHYMOD_E_FAIL;
    }      
    /* Intermediate step in vbs */
    PHYMOD_IF_ERR_RETURN((BCMI_QUADRA28_READ_MSG_OUTr(pa,&msg_out_reg)));
    msg_out_reg.v[0] = MSG_OUTr_MESSAGE_OUTf_GET(msg_out_reg);   
    if (0x300 != msg_out_reg.v[0]) {
         return PHYMOD_E_FAIL;
    }
           
    /* step 20 Read checksum (expected 0x600D) */
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_READ_BOOT_CHECKSUMr(pa, &checksum_reg)); 
    checksum_reg.v[0]=BCMI_QUADRA28_BOOT_CHECKSUMr_BOOT_CHKSUMf_GET(checksum_reg); 
    if (0x600D != checksum_reg.v[0]) {
        /*Checksum is correct */
         return PHYMOD_E_FAIL;
    }
    
    return PHYMOD_E_NONE; 
}

int quadra28_firmware_info_get(const phymod_access_t *pa, phymod_core_firmware_info_t *fw_info)
{
    uint32_t fw_ver = 0;
    BOOT_CHECKSUMr_t mst_running_chksum;

    /* Read the firmware version */
    PHYMOD_IF_ERR_RETURN(
        phymod_raw_iblk_read(pa, (0x10000 | (uint32_t) 0xC161), &fw_ver));
 
    fw_info->fw_version = (fw_ver & 0xff); 
    PHYMOD_IF_ERR_RETURN( 
        BCMI_QUADRA28_READ_BOOT_CHECKSUMr(pa, 
            &mst_running_chksum));
    fw_info->fw_crc = BCMI_QUADRA28_BOOT_CHECKSUMr_BOOT_CHKSUMf_GET(mst_running_chksum);
   
    return PHYMOD_E_NONE;
}

