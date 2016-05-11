/*
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
*  $Id$
*/

/*
 *         
 * 
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
 *     
 */

#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_reg.h>
#include "bcmi_dino_defs.h"
#include "dino_cfg_seq.h"
#include "dino_serdes/common/srds_api_enum.h"
#include "dino_serdes/merlin_dino_src/merlin_dino_functions.h"
#include "dino_serdes/falcon2_dino_src/falcon2_dino_functions.h"

/* uController's firmware */
extern unsigned char dino_ucode_Dino[];
extern unsigned short dino_ucode_Dino_len;

int dino_get_chipid (const phymod_access_t *pa, uint32_t *chipid, uint32_t *rev) 
{

    BCMI_DINO_CHIP_IDr_t cid;
    BCMI_DINO_CHIP_REVISIONr_t rid;

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_CHIP_IDr(pa, &cid));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_CHIP_REVISIONr(pa, &rid));

    *chipid = (BCMI_DINO_CHIP_REVISIONr_CHIP_ID_19_16f_GET(rid) << 16 ) |
               BCMI_DINO_CHIP_IDr_GET(cid);
    *rev = BCMI_DINO_CHIP_REVISIONr_CHIP_REVf_GET(rid);

    PHYMOD_DEBUG_VERBOSE(("CHIP ID: %x REV:%x\n", *chipid, *rev));

    return PHYMOD_E_NONE;
}

/**   Wait master message out 
 *    This function is to ensure whether master has sent the previous message
 *    out successfully 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param exp_message        Expected message specified by user  
 *    @param poll_time          Poll interval 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _dino_wait_mst_msgout(const phymod_access_t *pa,
                          DINO_MSGOUT_E exp_message,
                          int poll_time)
{
    int retry_count = DINO_FW_DLOAD_RETRY_CNT;
    DINO_MSGOUT_E msgout = MSGOUT_DONTCARE;
    BCMI_DINO_MST_MSGOUTr_t msgout_t; 

    do {
	    /* Read general control msg out  Register */
        PHYMOD_IF_ERR_RETURN(
	        BCMI_DINO_READ_MST_MSGOUTr(pa,&msgout_t));
	    msgout = BCMI_DINO_MST_MSGOUTr_MST_MSGOUT_VALf_GET(msgout_t); 
	    /* wait before reading again */
	    if (poll_time != 0) {
		    /* sleep for specified seconds*/
		    PHYMOD_SLEEP(poll_time);
	    }
    } while ((msgout != exp_message) &&  retry_count--);
    
    if (!retry_count) {
        if (exp_message == MSGOUT_PRGRM_DONE) { 
            PHYMOD_DEBUG_VERBOSE(("MSG OUT:%x \n",msgout));
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("Fusing Firmware failed")));
        } else {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT, (_PHYMOD_MSG("Firmware download failed")));
        }
    }

    return PHYMOD_E_NONE; 
}

int _dino_fw_enable(const phymod_access_t* pa)
{
    uint16_t retry_cnt = DINO_FW_ENABLE_RETRY_CNT;
    uint16_t data = 0;
    BCMI_DINO_FIRMWARE_ENr_t fw_en;
    PHYMOD_MEMSET(&fw_en, 0, sizeof(BCMI_DINO_FIRMWARE_ENr_t));

    do {
	    PHYMOD_IF_ERR_RETURN(
		    BCMI_DINO_READ_FIRMWARE_ENr(pa, &fw_en));
	    data = BCMI_DINO_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data != 0) && (retry_cnt--));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                               (_PHYMOD_MSG("mode config failed, micro controller is busy..")));
    }
    retry_cnt = DINO_FW_ENABLE_RETRY_CNT;
    /* Set FW_ENABLE = 1 */
    PHYMOD_IF_ERR_RETURN(
	    BCMI_DINO_READ_FIRMWARE_ENr(pa, &fw_en));
    /* coverity[operator_confusion] */
    BCMI_DINO_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en, 1);
    PHYMOD_IF_ERR_RETURN(
	    BCMI_DINO_WRITE_FIRMWARE_ENr(pa, fw_en));
   do {
	    PHYMOD_IF_ERR_RETURN(
		    BCMI_DINO_READ_FIRMWARE_ENr(pa, &fw_en));
	    data = BCMI_DINO_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data != 0) && (retry_cnt--));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("mode config failed, micro controller is busy..")));
    }

    return PHYMOD_E_NONE;
}
int _dino_core_reset_set(const phymod_access_t *pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    BCMI_DINO_GEN_CTL1r_t gen_ctrl;
    PHYMOD_MEMSET(&gen_ctrl, 0, sizeof(BCMI_DINO_GEN_CTL1r_t));
	
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_GEN_CTL1r(pa, &gen_ctrl));

    if (reset_mode == phymodResetModeHard) {
        /*Reset for chip*/
        BCMI_DINO_GEN_CTL1r_RESETBf_SET(gen_ctrl, 0);    
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_WRITE_GEN_CTL1r(pa, gen_ctrl));
        /*Above chip reset resets the cores also !*/
        PHYMOD_USLEEP(10000);
    } else if (reset_mode == phymodResetModeSoft) {
        /*Reset registers*/
	    BCMI_DINO_GEN_CTL1r_REG_RSTBf_SET(gen_ctrl, 0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_WRITE_GEN_CTL1r(pa, gen_ctrl));
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
int dino_download_prog_eeprom(const phymod_access_t *pa,
                              uint8_t *new_fw,
                              uint32_t fw_length,
                              uint8_t prg_eeprom)
{
    uint16_t num_bytes = 0;
    uint16_t j = 0;
    uint16_t data = 0;
    uint16_t data1 = 0;
    uint16_t data2 = 0;
    uint16_t check_sum = 0;
    int retry_count = DINO_FW_DLOAD_RETRY_CNT;
    FIRMWARE_VERSIONr_t  firmware_version;
    BCMI_DINO_GEN_CTL2r_t gen_ctrl2;   
    BCMI_DINO_GEN_CTL3r_t gen_ctrl3;
    BCMI_DINO_UC_BOOT_PORr_t   boot_por;
    BCMI_DINO_FIRMWARE_VERSIONr_t fw_ver;
    BCMI_DINO_BOOTr_t boot;
    BCMI_DINO_MST_MSGINr_t msg_in;
    BCMI_DINO_SPI_CODE_LOAD_ENr_t spi_code_load_en;
    BCMI_DINO_DWNLD_15r_t dwnld_15; 
    BCMI_DINO_DWNLD_16r_t dwnld_16; 
    BCMI_DINO_DWNLD_17r_t dwnld_17; 

    PHYMOD_MEMSET(&firmware_version, 0, sizeof(FIRMWARE_VERSIONr_t));
    PHYMOD_MEMSET(&boot_por, 0, sizeof(BCMI_DINO_UC_BOOT_PORr_t));
    PHYMOD_MEMSET(&msg_in, 0, sizeof(BCMI_DINO_MST_MSGINr_t));
    PHYMOD_MEMSET(&spi_code_load_en, 0, sizeof(BCMI_DINO_SPI_CODE_LOAD_ENr_t));
    PHYMOD_MEMSET(&gen_ctrl2, 0, sizeof(BCMI_DINO_GEN_CTL2r_t));
    PHYMOD_MEMSET(&gen_ctrl3, 0, sizeof(BCMI_DINO_GEN_CTL3r_t));
    PHYMOD_MEMSET(&boot, 0, sizeof(BCMI_DINO_BOOTr_t));
    PHYMOD_MEMSET(&fw_ver, 0, sizeof(BCMI_DINO_FIRMWARE_VERSIONr_t));
    PHYMOD_MEMSET(&spi_code_load_en, 0, sizeof(BCMI_DINO_SPI_CODE_LOAD_ENr_t));
    PHYMOD_MEMSET(&dwnld_15, 0, sizeof(BCMI_DINO_DWNLD_15r_t));
    PHYMOD_MEMSET(&dwnld_16, 0, sizeof(BCMI_DINO_DWNLD_16r_t));
    PHYMOD_MEMSET(&dwnld_17, 0, sizeof(BCMI_DINO_DWNLD_17r_t));

    /* Skip the firmware download 
     *  i) if serboot bin is low and mst_download, slv_dwld_done bits are set
     */
    if (!prg_eeprom) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_UC_BOOT_PORr(pa, &boot_por));
        if((BCMI_DINO_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por))&&
	        (BCMI_DINO_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por))){
            return DINO_FW_ALREADY_DOWNLOADED;
        }
    }
    /* Do chip reset */
    PHYMOD_IF_ERR_RETURN(
        _dino_core_reset_set(pa, phymodResetModeHard, phymodResetDirectionInOut));

    /* STEP 1: Put Master under Reset */
    PHYMOD_IF_ERR_RETURN(
	    BCMI_DINO_READ_GEN_CTL2r(pa, &gen_ctrl2));
    /*resetting M0 micro*/
    BCMI_DINO_GEN_CTL2r_MST_RSTBf_SET(gen_ctrl2, 0);
    /*resetting M0 micro perip*/
    BCMI_DINO_GEN_CTL2r_MST_UCP_RSTBf_SET(gen_ctrl2, 0);
    PHYMOD_IF_ERR_RETURN(
	    BCMI_DINO_WRITE_GEN_CTL2r(pa, gen_ctrl2));

    /* STEP 2: Wait for any pending SPI download
     * SPI download is not interrupted by Master Reset
     * (this is a safety feature) so we need to wait its 
     * completion before starting the MDIO Download */
    do {
        PHYMOD_IF_ERR_RETURN(
	        BCMI_DINO_READ_BOOTr(pa, &boot));
	    data1 = BCMI_DINO_BOOTr_SERBOOT_BUSYf_GET(boot); 
	    PHYMOD_USLEEP(500);
    } while((data1 != 0) && (retry_count--));
    if (retry_count == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                               (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
    }
 
    if(prg_eeprom) {
        /* BCM82332 does not have EEPROM. This feature will be added in future for BCM82793 */ 
    }

    /* STEP 3: Program master enable, slave enable, broadcast enable bits */
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_SPI_CODE_LOAD_ENr(pa, &spi_code_load_en));
    BCMI_DINO_SPI_CODE_LOAD_ENr_MST_CODE_DOWNLOAD_ENf_SET(spi_code_load_en, 1);
    BCMI_DINO_SPI_CODE_LOAD_ENr_SLV_CODE_DOWNLOAD_ENf_SET(spi_code_load_en, 0x3f);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_SPI_CODE_LOAD_ENr(pa, spi_code_load_en));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_SPI_CODE_LOAD_ENr(pa, &spi_code_load_en));
    if ((BCMI_DINO_SPI_CODE_LOAD_ENr_SLV_CODE_DOWNLOAD_ENf_GET(spi_code_load_en) != 0x3f) ||
        !(BCMI_DINO_SPI_CODE_LOAD_ENr_MST_CODE_DOWNLOAD_ENf_GET(spi_code_load_en))) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                               (_PHYMOD_MSG("ERR: BROADCAST ENABLE IS NOT SET")));
    }

    /* STEP 4: Force master download done, slave download done bit to 0 
     * set external boot (serboot) to 1 */
    PHYMOD_IF_ERR_RETURN(
	    BCMI_DINO_READ_UC_BOOT_PORr(pa, &boot_por));
    /* Force Master New Download */
    BCMI_DINO_UC_BOOT_PORr_MST_DWLD_DONEf_SET(boot_por, 0);
    /* Force Slave New Download */
    BCMI_DINO_UC_BOOT_PORr_SLV_DWLD_DONEf_SET(boot_por, 0);
    /*set to 0 for mdio default download on reset*/
    BCMI_DINO_UC_BOOT_PORr_SPI_PORT_USEDf_SET(boot_por, 0);
    BCMI_DINO_UC_BOOT_PORr_SLV_RST_ENf_SET(boot_por, 0);
    BCMI_DINO_UC_BOOT_PORr_SERBOOTf_SET(boot_por, 1);
    PHYMOD_IF_ERR_RETURN(
	    BCMI_DINO_WRITE_UC_BOOT_PORr(pa, boot_por));

    /* STEP 5: RELEASE Master under Reset */
    PHYMOD_IF_ERR_RETURN(
	    BCMI_DINO_READ_GEN_CTL2r(pa, &gen_ctrl2));
    BCMI_DINO_GEN_CTL2r_MST_UCP_RSTBf_SET(gen_ctrl2, 1); 
    PHYMOD_IF_ERR_RETURN(
	    BCMI_DINO_WRITE_GEN_CTL2r(pa, gen_ctrl2));
    PHYMOD_IF_ERR_RETURN(
	    BCMI_DINO_READ_GEN_CTL2r(pa, &gen_ctrl2));
    BCMI_DINO_GEN_CTL2r_MST_RSTBf_SET(gen_ctrl2, 1);
    PHYMOD_IF_ERR_RETURN(
	    BCMI_DINO_WRITE_GEN_CTL2r(pa, gen_ctrl2));

    /* STEP 6: MDI/I2C Download */
    /* Waiting for serboot_busy */
    retry_count = DINO_FW_DLOAD_RETRY_CNT;
    do {
        PHYMOD_IF_ERR_RETURN(
	        BCMI_DINO_READ_BOOTr(pa, &boot));
	    data1 = BCMI_DINO_BOOTr_SERBOOT_BUSYf_GET(boot); 
        PHYMOD_USLEEP(500);
    } while ((data1 != 1) && (retry_count--));
    if (retry_count == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                               (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
        return PHYMOD_E_FAIL;
    }

    PHYMOD_IF_ERR_RETURN
        (_dino_wait_mst_msgout(pa, MSGOUT_FLASH, 0));
    /* coverity[operator_confusion] */
	BCMI_DINO_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in, 0);
	PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MST_MSGINr(pa, msg_in));
    PHYMOD_IF_ERR_RETURN(
        _dino_wait_mst_msgout(pa, MSGOUT_NEXT, 0));
    /* STEP 7: Program master and slave boot address */ 
    /* coverity[operator_confusion] */
	BCMI_DINO_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in, 0);
	PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MST_MSGINr(pa, msg_in));

    num_bytes = fw_length;
    /* STEP 8: Download the 32 byte header first */
    for (j = 0; j <= DINO_FW_HEADER_SIZE; j += 2) {
	    /*sending next ucode data*/
        data = (new_fw[j + 1] << 8) | new_fw[j];
        check_sum ^= new_fw[j] ^ new_fw[j + 1];
        /* Send next word */
        BCMI_DINO_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in, data);
        PHYMOD_IF_ERR_RETURN(
		    BCMI_DINO_WRITE_MST_MSGINr(pa, msg_in));
    }
    PHYMOD_USLEEP(2100);
    /* STEP 9: Download the remaining portion of the firmware */
    for (j = DINO_FW_HEADER_SIZE + 2; j < num_bytes; j += 2) {
	    /*sending next ucode data*/
        data = (new_fw[j + 1] << 8) | new_fw[j];
        check_sum ^= new_fw[j] ^ new_fw[j + 1];
        /* Send next word */
        BCMI_DINO_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in, data);
        PHYMOD_IF_ERR_RETURN(
		    BCMI_DINO_WRITE_MST_MSGINr(pa, msg_in));
        if ((j % 32) == 0) {
            PHYMOD_USLEEP(2100);
        }
    }
    /* STEP 10: i) Verify whether the firmware download is done for master and slaves */
    retry_count = DINO_FW_DLOAD_RETRY_CNT;
    do {
	    /* check download_done flags*/
	    PHYMOD_IF_ERR_RETURN(
			    BCMI_DINO_READ_UC_BOOT_PORr(pa, &boot_por));
	    if((BCMI_DINO_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por) == 1) &&
           (BCMI_DINO_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por) == 0x3f)){
            PHYMOD_DEBUG_VERBOSE(("Firmware Download Done\n"));
            break;
        }
    } while (retry_count--);

    if (!retry_count) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT, (_PHYMOD_MSG("Firmware download failed")));
    }

    /* STEP 11: ii) Verify serboot busy and serboot done once bits */
    retry_count = DINO_FW_DLOAD_RETRY_CNT;
    do {
        PHYMOD_IF_ERR_RETURN(
	        BCMI_DINO_READ_BOOTr(pa, &boot));
	    data1 = BCMI_DINO_BOOTr_SERBOOT_BUSYf_GET(boot); 
	    data2 = BCMI_DINO_BOOTr_SERBOOT_DONE_ONCEf_GET(boot); 
        PHYMOD_USLEEP(500);
    } while (((data1 != 0) && (data2 == 1)) && (retry_count--));
    if (retry_count == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                               (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
        return PHYMOD_E_FAIL;
    }

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DWNLD_15r(pa, &dwnld_15));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DWNLD_16r(pa, &dwnld_16));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DWNLD_17r(pa, &dwnld_17));

    if ((BCMI_DINO_DWNLD_15r_DOWNLOAD_15f_GET(dwnld_15) != 0x600D) ||
        (BCMI_DINO_DWNLD_16r_DOWNLOAD_16f_GET(dwnld_16) != 0x600D) ||
        (BCMI_DINO_DWNLD_17r_DOWNLOAD_17f_GET(dwnld_17) != 0x600D)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                               (_PHYMOD_MSG("chksum Failed")));
    }
    /* Read the firmware version */
    PHYMOD_IF_ERR_RETURN(
        READ_FIRMWARE_VERSIONr(pa, &firmware_version));
    PHYMOD_DEBUG_VERBOSE(("Firmware Version: 0x%x\n", firmware_version.v[0]));

    return PHYMOD_E_NONE;
}

int _dino_core_init(const phymod_core_access_t* core,
                     const phymod_core_init_config_t* init_config)
{
    int rv = PHYMOD_E_NONE; 
    const phymod_access_t *pm_acc = &core->access;

    switch(init_config->firmware_load_method) {
        case phymodFirmwareLoadMethodInternal:
            PHYMOD_DEBUG_VERBOSE(("Starting Firmware download through MDIO, it takes few seconds...\n"));
            rv = dino_download_prog_eeprom(pm_acc,
                                           dino_ucode_Dino,
                                           dino_ucode_Dino_len,
                                           0);
            if ((rv != PHYMOD_E_NONE) &&
                (rv != DINO_FW_ALREADY_DOWNLOADED)) {
                PHYMOD_RETURN_WITH_ERR(rv,
                                       (_PHYMOD_MSG("firmware download failed")));
            } else {
                if (rv == DINO_FW_ALREADY_DOWNLOADED) {
                    rv = PHYMOD_E_NONE;
                }
                PHYMOD_DEBUG_VERBOSE(("Firmware download through MDIO success\n"));
            }
        break;
        case phymodFirmwareLoadMethodExternal:
            return PHYMOD_E_UNAVAIL;
        break;        
        case phymodFirmwareLoadMethodNone:
        break;
        case phymodFirmwareLoadMethodProgEEPROM:
            return PHYMOD_E_UNAVAIL;
        break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                                   (_PHYMOD_MSG("illegal fw load method")));
    }

    return rv;
}

int _dino_set_slice_reg (const phymod_access_t* pa, DINO_SLICE_OP_TYPE slice_op_type, uint16_t if_side, uint16_t lane)
{
    BCMI_DINO_SLICEr_t slice_reg;
    BCMI_DINO_DECD_MODE_STS0r_t mode_sts0;
    BCMI_DINO_DECD_MODE_STS1r_t mode_sts1;
    BCMI_DINO_DECD_MODE_STS2r_t mode_sts2;
    BCMI_DINO_DECD_MODE_STS3r_t mode_sts3;
    BCMI_DINO_DECD_MODE_STS4r_t mode_sts4;
    uint16_t sys_sel     = 0;
    uint16_t line_sel    = 0;
    uint16_t dev_type    = 0;
    uint16_t lane_mask   = 0;
    uint16_t slice_0     = 0;
    uint16_t slice_1     = 0;
    uint16_t slice_2     = 0;
    PHYMOD_MEMSET(&slice_reg, 0, sizeof(BCMI_DINO_SLICEr_t));
    PHYMOD_MEMSET(&mode_sts0, 0, sizeof(BCMI_DINO_DECD_MODE_STS0r_t));
    PHYMOD_MEMSET(&mode_sts1, 0, sizeof(BCMI_DINO_DECD_MODE_STS1r_t));
    PHYMOD_MEMSET(&mode_sts2, 0, sizeof(BCMI_DINO_DECD_MODE_STS2r_t));
    PHYMOD_MEMSET(&mode_sts3, 0, sizeof(BCMI_DINO_DECD_MODE_STS3r_t));
    PHYMOD_MEMSET(&mode_sts4, 0, sizeof(BCMI_DINO_DECD_MODE_STS4r_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS0r(pa, &mode_sts0));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS1r(pa, &mode_sts1));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS2r(pa, &mode_sts2));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS3r(pa, &mode_sts3));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS4r(pa, &mode_sts4));

    if (slice_op_type == DINO_SLICE_RESET) {
        slice_0 = 1;
        PHYMOD_IF_ERR_RETURN(
		    BCMI_DINO_READ_SLICEr(pa, &slice_reg));
        if ((mode_sts0.v[0] & 0xff9) ||
            (mode_sts1.v[0] & 0xf) ||
            (mode_sts2.v[0] & 0xf)) {
            slice_0 = 1;
        }
        if ((mode_sts0.v[0] & 0x2) ||
            (mode_sts1.v[0] & 0xf0) ||
            (mode_sts2.v[0] & 0xf0)) {
            slice_1 = 1;
        }
        if ((mode_sts0.v[0] & 0x4) ||
            (mode_sts1.v[0] & 0xf00) ||
            (mode_sts2.v[0] & 0xf00)) {
            slice_2 = 1;
        }
        BCMI_DINO_SLICEr_CLR(slice_reg);
        BCMI_DINO_SLICEr_SYS_SELf_SET(slice_reg, 0);
        BCMI_DINO_SLICEr_LIN_SELf_SET(slice_reg, 1);
        BCMI_DINO_SLICEr_SLICE_0_SELf_SET(slice_reg, slice_0);
        BCMI_DINO_SLICEr_SLICE_1_SELf_SET(slice_reg, slice_1);
        BCMI_DINO_SLICEr_SLICE_2_SELf_SET(slice_reg, slice_2);
        BCMI_DINO_SLICEr_LANE_SELf_SET(slice_reg, 0x1);
        PHYMOD_IF_ERR_RETURN(
		    BCMI_DINO_WRITE_SLICEr(pa, slice_reg));
        return PHYMOD_E_NONE;
    }

    if (if_side == DINO_IF_SYS) {
        sys_sel = 1;
        line_sel = 0;
    } else {
        sys_sel = 0;
        line_sel = 1;
    }

    dev_type = pa->devad;
    if(!dev_type) {
        dev_type = DINO_DEV_PMA_PMD;
    }

    if (slice_op_type == DINO_SLICE_BROADCAST) {
        if (if_side == DINO_IF_SYS) { 
            if (lane_mask & DINO_40G_PORT0_LANE_MASK) {
                slice_0 = 1;
            }
            if (lane_mask & DINO_40G_PORT1_LANE_MASK) {
                slice_1 = 1;
            }
            if (lane_mask & DINO_40G_PORT2_LANE_MASK) {
                slice_2 = 1;
            }
            if (lane_mask == DINO_40G_PORT0_LANE_MASK  ||
                lane_mask == DINO_40G_PORT1_LANE_MASK  ||
                lane_mask == DINO_40G_PORT2_LANE_MASK  ||
                lane_mask == DINO_100G_TYPE1_LANE_MASK ||
                lane_mask == DINO_100G_TYPE2_LANE_MASK ||
                lane_mask == DINO_100G_TYPE3_LANE_MASK) {
                lane_mask = DINO_40G_PORT0_LANE_MASK;
            } else {
                lane_mask = (1 << (lane % DINO_MAX_FALCON_LANE));
            }
        } else {
            if (lane < DINO_MAX_FALCON_LANE) {
                slice_0 = 1;
            } else if ((lane >= DINO_MAX_FALCON_LANE) && (lane < 8)) {
                slice_1 = 1;
            } else {
                slice_2 = 1;
            }
            if (lane_mask == DINO_40G_PORT0_LANE_MASK ||
                lane_mask == DINO_40G_PORT1_LANE_MASK ||
                lane_mask == DINO_40G_PORT2_LANE_MASK) {
                lane_mask = DINO_40G_PORT0_LANE_MASK;
            } else {
                lane_mask = (1 << (lane % DINO_MAX_FALCON_LANE));
            }
        }
    } else if (slice_op_type == DINO_SLICE_UNICAST) {
        if (lane < DINO_MAX_FALCON_LANE) {
            slice_0 = 1;
        } else if ((lane >= DINO_MAX_FALCON_LANE) && (lane < 8)) {
            slice_1 = 1;
        } else {
            slice_2 = 1;
        }
        lane_mask = (1 << (lane % DINO_MAX_FALCON_LANE));
    }

    if (dev_type == DINO_DEV_PMA_PMD) {
        PHYMOD_IF_ERR_RETURN(
		    BCMI_DINO_READ_SLICEr(pa, &slice_reg));
        BCMI_DINO_SLICEr_CLR(slice_reg);
        BCMI_DINO_SLICEr_SYS_SELf_SET(slice_reg, sys_sel);
        BCMI_DINO_SLICEr_LIN_SELf_SET(slice_reg, line_sel);
        BCMI_DINO_SLICEr_SLICE_0_SELf_SET(slice_reg, slice_0);
        BCMI_DINO_SLICEr_SLICE_1_SELf_SET(slice_reg, slice_1);
        BCMI_DINO_SLICEr_SLICE_2_SELf_SET(slice_reg, slice_2);
        BCMI_DINO_SLICEr_LANE_SELf_SET(slice_reg, lane_mask);
        PHYMOD_IF_ERR_RETURN(
		    BCMI_DINO_WRITE_SLICEr(pa, slice_reg));
	    PHYMOD_DEBUG_VERBOSE(("\n[-- slice = %x ===]\n", BCMI_DINO_SLICEr_GET(slice_reg)));
    } else {
        /* Needs to add AN slice sequence */
    }

    return PHYMOD_E_NONE;
}

int _dino_fifo_reset(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config) {
    uint16_t lane_mask = 0;
    uint16_t if_side   = 0;
    uint16_t lane  = 0;
    const phymod_access_t *pa = &phy->access;
    uint16_t data = 0;
    BCMI_DINO_FIFO_RST_FRC_CTL1r_t fifo_rst_frc_ln;
    BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_t fifo_rst_frcval_ln;
    BCMI_DINO_FIFO_RST_FRC_CTL0r_t fifo_rst_frc_sys;
    BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_t fifo_rst_frcval_sys;
    PHYMOD_MEMSET(&fifo_rst_frc_ln, 0, sizeof(BCMI_DINO_FIFO_RST_FRC_CTL1r_t));
    PHYMOD_MEMSET(&fifo_rst_frcval_ln, 0, sizeof(BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_t));
    PHYMOD_MEMSET(&fifo_rst_frc_sys, 0, sizeof(BCMI_DINO_FIFO_RST_FRC_CTL0r_t));
    PHYMOD_MEMSET(&fifo_rst_frcval_sys, 0, sizeof(BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);

    if ((config->data_rate == DINO_SPD_100G) || (config->data_rate == DINO_SPD_106G)) {
        lane_mask = 0xfff;
    }
    for (lane = 0; lane < DINO_MAX_LANE; lane++) {
        if (lane_mask & (1 << lane)) {
            if (if_side == DINO_IF_LINE) {
                /* Set force val to 0*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL1r(pa, &fifo_rst_frcval_ln));
                data = fifo_rst_frcval_ln.v[0];
                data &= ~(1 << lane);
                BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_SET(fifo_rst_frcval_ln, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL1r(pa, fifo_rst_frcval_ln));

                /* Set force to 1*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRC_CTL1r(pa, &fifo_rst_frc_ln));
                data = fifo_rst_frc_ln.v[0];
                data |= (1 << lane);
                BCMI_DINO_FIFO_RST_FRC_CTL1r_SET(fifo_rst_frc_ln, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRC_CTL1r(pa, fifo_rst_frc_ln));

                /* Set force val to default*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL1r(pa, &fifo_rst_frcval_ln));
                data = fifo_rst_frcval_ln.v[0];
                data |= (1 << lane);
                BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_SET(fifo_rst_frcval_ln, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL1r(pa, fifo_rst_frcval_ln));
                
                /* Set force to default val*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRC_CTL1r(pa, &fifo_rst_frc_ln));

                fifo_rst_frc_ln.v[0] &= ~(1 << lane);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRC_CTL1r(pa, fifo_rst_frc_ln));
            } else {
                /* Set force val to 0*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL0r(pa, &fifo_rst_frcval_sys));
                data = fifo_rst_frcval_sys.v[0];
                data &= ~(1 << lane);
                BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_SET(fifo_rst_frcval_sys, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL0r(pa, fifo_rst_frcval_sys));

                /* Set force to 1*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRC_CTL0r(pa, &fifo_rst_frc_sys));
                data = fifo_rst_frc_sys.v[0];
                data |= (1 << lane);
                BCMI_DINO_FIFO_RST_FRC_CTL0r_SET(fifo_rst_frc_sys, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRC_CTL0r(pa, fifo_rst_frc_sys));

                /* Set force val to default*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL0r(pa, &fifo_rst_frcval_sys));
                data = fifo_rst_frcval_sys.v[0];
                data |= (1 << lane);
                BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_SET(fifo_rst_frcval_sys, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL0r(pa, fifo_rst_frcval_sys));

                /* Set force to default*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRC_CTL0r(pa, &fifo_rst_frc_sys));
                fifo_rst_frc_sys.v[0] &= (1 << lane);
                BCMI_DINO_FIFO_RST_FRC_CTL0r_SET(fifo_rst_frc_sys, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRC_CTL0r(pa, fifo_rst_frc_sys));
            }
        }
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    const phymod_access_t *pa = &phy->access;

   
    PHYMOD_DEBUG_VERBOSE(("Configuring interface\n"));
    /* Configure Ref Clock*/
    PHYMOD_DEBUG_VERBOSE(("Configuring REF clock %d\n",config->ref_clock));
    PHYMOD_IF_ERR_RETURN(
        _dino_configure_ref_clock(&phy->access, config->ref_clock));

    /* Configure DUT MODE */
    PHYMOD_IF_ERR_RETURN(
        _dino_config_dut_mode_reg(phy, config));
    
    /* Configure PLL Divider*/
    PHYMOD_IF_ERR_RETURN(
        _dino_config_pll_div(phy, config));

    /* Configure Interface.*/
    PHYMOD_IF_ERR_RETURN(
        _dino_interface_set(phy, config));

    /* store phy type */
    PHYMOD_IF_ERR_RETURN(
        _dino_save_phy_type(pa, config));

    /* Do fifo reset */
    PHYMOD_IF_ERR_RETURN(
        _dino_fifo_reset(phy, config));

    /* Set firmware enable */
    PHYMOD_IF_ERR_RETURN
        (_dino_fw_enable(pa));

    return PHYMOD_E_NONE;
}

int _dino_interface_get(phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config, phymod_interface_t *intf)
{
    PHYMOD_IF_ERR_RETURN(
        _dino_restore_interface(phy, config, intf));

    return PHYMOD_E_NONE;
}

int _dino_phy_interface_config_get(phymod_phy_access_t *phy, uint32_t flags, phymod_phy_inf_config_t *config)
{
    uint16_t clock_ratio = 0;
    uint16_t lane_mask = 0;
    phymod_interface_t intf = 0;
    uint16_t lane_index = 0;
    uint32_t port_speed = 0;
    uint16_t phy_type = 0;
    const phymod_access_t *pa = &phy->access;
    BCMI_DINO_CLK_SCALER_CTLr_t clk_ctrl;
	/* Get data rate, passthru, gearbox, get refclock, get intf side*/
    BCMI_DINO_DECD_MODE_STS0r_t mode_sts0;
    BCMI_DINO_DECD_MODE_STS1r_t mode_sts1;
    BCMI_DINO_DECD_MODE_STS2r_t mode_sts2;
    BCMI_DINO_DECD_MODE_STS3r_t mode_sts3;
    BCMI_DINO_DECD_MODE_STS4r_t mode_sts4;
    PHYMOD_MEMSET(&mode_sts0, 0, sizeof(BCMI_DINO_DECD_MODE_STS0r_t));
    PHYMOD_MEMSET(&mode_sts1, 0, sizeof(BCMI_DINO_DECD_MODE_STS1r_t));
    PHYMOD_MEMSET(&mode_sts2, 0, sizeof(BCMI_DINO_DECD_MODE_STS2r_t));
    PHYMOD_MEMSET(&mode_sts3, 0, sizeof(BCMI_DINO_DECD_MODE_STS3r_t));
    PHYMOD_MEMSET(&mode_sts4, 0, sizeof(BCMI_DINO_DECD_MODE_STS4r_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    if (!lane_mask) {
        PHYMOD_DEBUG_VERBOSE(("Invalid Lanemask\n"));
        return PHYMOD_E_PARAM;
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS0r(pa, &mode_sts0));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS1r(pa, &mode_sts1));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS2r(pa, &mode_sts2));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS3r(pa, &mode_sts3));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS4r(pa, &mode_sts4));

    if ((BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_PT_2TO11f_GET(mode_sts0)) ||
       (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_PT_1TO10f_GET(mode_sts0)) ||
       (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_PT_0TO9f_GET(mode_sts0))) {
        config->data_rate = DINO_SPD_100G;
        PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_CLR(phy);
    } else if ((BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_GBOX_2TO11f_GET(mode_sts0)) ||
               (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_GBOX_1TO10f_GET(mode_sts0)) ||
               (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_GBOX_0TO9f_GET(mode_sts0))) {
        config->data_rate = DINO_SPD_100G;
        PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_SET(phy);
    } else if (lane_mask == DINO_40G_PORT0_LANE_MASK) {
        if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_40G_MP0f_GET(mode_sts0)) {
            config->data_rate = DINO_SPD_40G;
        }
    } else if (lane_mask == DINO_40G_PORT1_LANE_MASK) {
        if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_40G_MP1f_GET(mode_sts0)) {
            config->data_rate = DINO_SPD_40G;
        }
    } else if (lane_mask == DINO_40G_PORT2_LANE_MASK) {
        if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_40G_MP2f_GET(mode_sts0)) {
            config->data_rate = DINO_SPD_40G;
        }
    } else {
        for (lane_index = 0; lane_index <= DINO_MAX_LANE; lane_index ++) {
            if (lane_mask & (0x1 << lane_index)) {
                switch (lane_index) {
                    case DINO_PORT0:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP0_0f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP0_0f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT1:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP0_1f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP0_1f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT2:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP0_2f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP0_2f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT3:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP0_3f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP0_3f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT4:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP1_0f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP1_0f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT5:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP1_1f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP1_1f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT6:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP1_2f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP1_2f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT7:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP1_3f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP1_3f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT8:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP2_0f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP2_0f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT9:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP2_1f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP2_1f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT10:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP2_2f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP2_2f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT11:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP2_3f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP2_3f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                }
            }
        }
    } 

    port_speed = config->data_rate;

    /*Ethernet*/
    config->interface_modes = 0;

    PHYMOD_IF_ERR_RETURN(
        _dino_restore_phy_type(pa, config, &phy_type));

    if (phy_type == DINO_PHY_TYPE_HIGIG) {
        switch (port_speed) {
            case DINO_SPD_10G:
                config->data_rate = DINO_SPD_11G;
             break;
            case DINO_SPD_40G:
                config->data_rate = DINO_SPD_42G;
             break;
            case DINO_SPD_100G:
                config->data_rate = DINO_SPD_106G;
             break;
             default:
             break;
        }
        PHYMOD_INTF_MODES_HIGIG_SET(config);
    } else {
        config->data_rate = port_speed;
    }

    /*CLOCK_SCALEr*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_CLK_SCALER_CTLr(pa, &clk_ctrl));
    clock_ratio = BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_GET(clk_ctrl);
    if (clock_ratio == 0x320) {
        config->ref_clock = phymodRefClk156Mhz; 
    } else if (clock_ratio == 0x340) {
        config->ref_clock = phymodRefClk161Mhz; 
    }

    PHYMOD_IF_ERR_RETURN(
        _dino_interface_get(phy, config, &intf));
    config->interface_type = intf;

    return PHYMOD_E_NONE;
}

int _dino_configure_ref_clock(const phymod_access_t *pa, phymod_ref_clk_t ref_clk)
{
    uint32_t rev     = 0;
    uint32_t chip_id = 0;
    BCMI_DINO_CLK_SCALER_CTLr_t clk_ctrl;
    PHYMOD_MEMSET(&clk_ctrl, 0, sizeof(BCMI_DINO_CLK_SCALER_CTLr_t));

    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
   
    switch (ref_clk) {
        case phymodRefClk156Mhz:
        case phymodRefClk312Mhz:
        	BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x320);
        break;
        case phymodRefClk161Mhz:
        	BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x340);
        break;
        case phymodRefClk322Mhz:
            if (chip_id == DINO_CHIP_ID_82793) {
        	    BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x680);
            } else {
        	    BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x340);
            }
        break;
        case phymodRefClk644Mhz:
            BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x340);
        break;
        case phymodRefClk174Mhz:
        case phymodRefClk698Mhz:
            BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x380);
        break;
        case phymodRefClk349Mhz:
            if (chip_id == DINO_CHIP_ID_82793) {
        	    BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x700);
            } else {
        	    BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x380);
            }
        break;
        default:
	        return PHYMOD_E_PARAM;
    }
	PHYMOD_IF_ERR_RETURN(
	    BCMI_DINO_WRITE_CLK_SCALER_CTLr(pa,clk_ctrl));
    
    return PHYMOD_E_NONE;
}

/** Configure Dut Mode Register
 */
int _dino_config_dut_mode_reg(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config) 
{
    uint16_t no_of_ports      = 0;
    uint16_t port_idx         = 0;
    uint16_t lane_mask        = 0;
    uint16_t if_side          = 0;
    uint16_t dev_op_mode      = DINO_DEV_OP_MODE_INVALID;
    uint16_t sw_gpreg_data    = 0;
    BCMI_DINO_MODECTL0r_t mode_ctrl0;
    BCMI_DINO_MODECTL1r_t mode_ctrl1;
    BCMI_DINO_MODECTL2r_t mode_ctrl2;
    BCMI_DINO_MODECTL3r_t mode_ctrl3;
    phymod_phy_inf_config_t cfg;
    const phymod_access_t *pa = &phy->access;
    PHYMOD_MEMSET(&mode_ctrl0, 0, sizeof(BCMI_DINO_MODECTL0r_t));
    PHYMOD_MEMSET(&mode_ctrl1, 0, sizeof(BCMI_DINO_MODECTL1r_t));
    PHYMOD_MEMSET(&mode_ctrl2, 0, sizeof(BCMI_DINO_MODECTL2r_t));
    PHYMOD_MEMSET(&mode_ctrl3, 0, sizeof(BCMI_DINO_MODECTL3r_t));
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    DINO_GET_PORT_FROM_MODE(config, no_of_ports);

    /* Store the device operation type into SW GPREG */
    if (PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) {
        dev_op_mode = DINO_DEV_OP_MODE_GBOX;
    } else {
        dev_op_mode = DINO_DEV_OP_MODE_PT;
    }
    READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
    sw_gpreg_data &= ~(DINO_DEV_OP_MODE_MASK);  
    sw_gpreg_data |= (dev_op_mode << DINO_DEV_OP_MODE_SHIFT);  
    WRITE_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MODECTL0r(pa, &mode_ctrl0));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MODECTL1r(pa, &mode_ctrl1));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MODECTL2r(pa, &mode_ctrl2));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MODECTL3r(pa, &mode_ctrl3));

    if ((config->data_rate != DINO_SPD_100G) && (config->data_rate != DINO_SPD_106G)) {
        /* To clear all 100G mode settings */
        BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
        for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
            /* Program Modes */
            if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                 (lane_mask == DINO_40G_PORT0_LANE_MASK))) {
                BCMI_DINO_MODECTL1r_MODE_10G_MP0_0f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP0_1f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP0_2f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP0_3f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP0_0f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP0_1f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP0_2f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP0_3f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 1);
                BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
            } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                        (lane_mask == DINO_40G_PORT1_LANE_MASK))) {
                BCMI_DINO_MODECTL1r_MODE_10G_MP1_0f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP1_1f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP1_2f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP1_3f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP1_0f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP1_1f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP1_2f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP1_3f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 1);
                BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
            } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                        (lane_mask == DINO_40G_PORT2_LANE_MASK))) {
                BCMI_DINO_MODECTL1r_MODE_10G_MP2_0f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP2_1f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP2_2f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP2_3f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP2_0f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP2_1f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP2_2f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP2_3f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 1);
                BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
            } else if (config->data_rate == DINO_SPD_10G || config->data_rate == DINO_SPD_11G) {
                if ((lane_mask & (0x1 << port_idx))) {
                    switch (port_idx) {
                        case DINO_PORT0:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_0f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_0f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT1:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_1f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_1f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT2:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_2f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_2f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT3:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_3f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_3f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT4:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_0f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_0f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT5:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_1f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_1f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT6:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_2f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_2f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT7:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_3f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_3f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT8:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_0f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_0f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT9:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_1f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_1f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT10:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_2f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_2f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT11:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_3f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_3f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                    }
                }
            } else if (config->data_rate == DINO_SPD_1G) {
                /* mode configuration for 1G */
                if ((lane_mask & (0x1 << port_idx))) {
                    switch (port_idx) {
                        case DINO_PORT0:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_0f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_0f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT1:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_1f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_1f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT2:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_2f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_2f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT3:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_3f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_3f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT4:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_0f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_0f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT5:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_1f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_1f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT6:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_2f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_2f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT7:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_3f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_3f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT8:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_0f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_0f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT9:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_1f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_1f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT10:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_2f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_2f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT11:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_3f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_3f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                    }
                }
            } else {
                /* Throw error as un supported mode */
            }
        }
    } else {
        /* Dino has support for 3 different lane maps (0-9) , (1-10) and (2-11) */
        if (PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) {
            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL1r_CLR(mode_ctrl1);
            BCMI_DINO_MODECTL2r_CLR(mode_ctrl2);
            BCMI_DINO_MODECTL3r_CLR(mode_ctrl3);
            /* Gear box mode */
            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
            if (if_side == DINO_IF_SYS) {
                if (lane_mask == DINO_100G_TYPE1_LANE_MASK) {
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 1);
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
                } else if (lane_mask == DINO_100G_TYPE2_LANE_MASK) {
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 1);
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
                } else if (lane_mask == DINO_100G_TYPE3_LANE_MASK) {
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 1);
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                } else {
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 1);
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
                }
            } else {
                /* Line side GBOX if none is set setting GBOX 0 */
                if (!(BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_GET(mode_ctrl0)) &&
                    !(BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_GET(mode_ctrl0)) &&
                    !(BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_GET(mode_ctrl0))) {
                    BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 1);
                } 
            }
        } else {
            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
            BCMI_DINO_MODECTL1r_CLR(mode_ctrl1);
            BCMI_DINO_MODECTL2r_CLR(mode_ctrl2);
            BCMI_DINO_MODECTL3r_CLR(mode_ctrl3);
            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
            if (lane_mask == DINO_100G_TYPE1_LANE_MASK) {
                BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 1);
            } else if (lane_mask == DINO_100G_TYPE2_LANE_MASK) {
                BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 1);
            } else if (lane_mask == DINO_100G_TYPE3_LANE_MASK) {
                BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 1);
            } else {
                BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 1);
            }
        }
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MODECTL0r(pa, mode_ctrl0));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MODECTL1r(pa, mode_ctrl1));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MODECTL2r(pa, mode_ctrl2));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MODECTL3r(pa, mode_ctrl3));

    return PHYMOD_E_NONE;
}

int _dino_config_pll_div(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config)
{
    uint16_t if_side = 0;
    uint16_t merlin_pll_mode = 0;
    uint16_t falcon_pll_mode = 0;
    uint16_t data = 0;
    uint16_t reg_data = 0;
    uint16_t lane_mask = 0;
    uint16_t lane = 0;
    uint16_t mer_pll_mapval  = 0;
    int mer_curr_pll_mapval = 0;
    uint16_t retry_cnt = DINO_FW_DLOAD_RETRY_CNT;
    uint8_t falcon_curr_pll_mode = 0;
    phymod_phy_access_t phy_copy;
    phymod_ref_clk_t ref_clk = config->ref_clock;
    BCMI_DINO_SLICEr_t slice_reg;
    BCMI_DINO_RST_CTL_CORE_DPr_t rst_ctl_core_dp;
    BCMI_DINO_PLL_CTL3r_t pll_ctl3;
    BCMI_DINO_PLL_CTL8r_t pll_ctl8;
    BCMI_DINO_F25G_PLL_CAL_CTL_STS0r_t falcon_pll_sts;
    BCMI_DINO_PLL_CALSTS0r_t merlin_pll_sts;
    BCMI_DINO_F25G_PLL_CAL_CTL7r_t falcon_pll_ctrl;
    phymod_access_t *pa = (phymod_access_t*)&phy->access;
    PHYMOD_MEMSET(&slice_reg, 0, sizeof(BCMI_DINO_SLICEr_t));
    PHYMOD_MEMSET(&rst_ctl_core_dp, 0, sizeof(BCMI_DINO_RST_CTL_CORE_DPr_t));
    PHYMOD_MEMSET(&pll_ctl3, 0, sizeof(BCMI_DINO_PLL_CTL3r_t));
    PHYMOD_MEMSET(&pll_ctl8, 0, sizeof(BCMI_DINO_PLL_CTL8r_t));
    PHYMOD_MEMSET(&falcon_pll_ctrl, 0, sizeof(BCMI_DINO_F25G_PLL_CAL_CTL7r_t));
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    if ((config->data_rate == DINO_SPD_100G) || (config->data_rate == DINO_SPD_106G)) {
        lane_mask = DINO_100G_TYPE1_LANE_MASK;
    }

    /* Set pll_mode */
    PHYMOD_IF_ERR_RETURN(
       _dino_get_pll_modes(phy, ref_clk, config, &falcon_pll_mode, &merlin_pll_mode));
    for (if_side = DINO_IF_LINE ; if_side <= DINO_IF_SYS ; if_side++) {
        for (lane = 0; lane < DINO_MAX_LANE ; lane += 4) {
            if (lane_mask & (0xf << lane)) {
                PHYMOD_IF_ERR_RETURN (
                    _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
                if (DINO_IS_FALCON_SIDE(if_side, lane)) {
                    /* Read the current pll mode program the pll only if the mode is not configured already */
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_F25G_PLL_CAL_CTL7r(pa, &falcon_pll_ctrl));
                    falcon_curr_pll_mode = BCMI_DINO_F25G_PLL_CAL_CTL7r_PLL_MODEf_GET(falcon_pll_ctrl);
                    if (falcon_curr_pll_mode == falcon_pll_mode) {
                        continue;
                    } 
                    /* 0xd11b . enable pll . Added macro in .h file */
                    PHYMOD_IF_ERR_RETURN(
	                    falcon2_dino_wrc_ams_pll_pwrdn(0)); 
		            /* write 0 to core_dp_s_rstb while pll config .  Added macro in .h file*/
                    /*  0xd184 .wrc_falcon2_dino_core_dp_s_rstb(0); */
                    PHYMOD_IF_ERR_RETURN(
		                falcon2_dino_wrc_core_dp_s_rstb(0)); 

                    /* Set PLL div */
                    switch (falcon_pll_mode) {
                        case DINO_PLL_MODE_80:
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_80x)); 
                        break;
		                case DINO_PLL_MODE_160:
                            if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                                PHYMOD_IF_ERR_RETURN (
                                    falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_160x_refc174));
                            } else {
                                PHYMOD_IF_ERR_RETURN (
                                    falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_160x)); 
                            }
                        break;
                        case DINO_PLL_MODE_132: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_132x)); 
                        break;
                        case DINO_PLL_MODE_128: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_128x)); 
                        break;
                        case DINO_PLL_MODE_140: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_140x));
                        break;
                        case DINO_PLL_MODE_165: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_165x));
                        break;
                        case DINO_PLL_MODE_168: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_168x));
                        break;
                        case DINO_PLL_MODE_175: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_175x));
                        break;
                        case DINO_PLL_MODE_180: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_180x));
                        break;
                        case DINO_PLL_MODE_184: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_184x));
                        break;
                        case DINO_PLL_MODE_200: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_200x));
                        break;
                        case DINO_PLL_MODE_224: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_224x));
                        break;
                        case DINO_PLL_MODE_264: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_264x));
                        break;
                        default:
		                return PHYMOD_E_PARAM;
                    }
                    /* write 1 to release core_dp_s_rstb after pll config*/
                    PHYMOD_IF_ERR_RETURN (
		                falcon2_dino_wrc_core_dp_s_rstb(1));
                    retry_cnt = DINO_FW_DLOAD_RETRY_CNT;
                    /* Check for the PLL lock */
                    do {
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_F25G_PLL_CAL_CTL_STS0r(pa, &falcon_pll_sts));
                        data = BCMI_DINO_F25G_PLL_CAL_CTL_STS0r_PLL_LOCKf_GET(falcon_pll_sts);
	                    PHYMOD_USLEEP(100);
                    } while((data != 1) && (retry_cnt--));
                    if (retry_cnt == 0) {
   	                    PHYMOD_DEBUG_ERROR(("PLL LOCK failed for pll\n"));       
                        return PHYMOD_E_FAIL;
                    }
                } else {
                    /* Read current merlin PLL mode map val from scratch register*/
                    READ_DINO_PMA_PMD_REG(pa, DINO_MER_PLL_MAP_VAL_REG_ADR, reg_data);
                    mer_curr_pll_mapval = (reg_data & (0x3 << ((2*(lane/4)) + (8*if_side)))) >> ((2*(lane/4))+(8*if_side));
                    mer_pll_mapval = _dino_map_mer_pll_div(merlin_pll_mode);
                    if (mer_pll_mapval == mer_curr_pll_mapval) {
                        continue;
                    }
                    /* write 0 to core_dp_s_rstb while pll config*/
                    PHYMOD_IF_ERR_RETURN(
                        wrc_core_dp_s_rstb(0));
                    switch (mer_pll_mapval) {
                        case 0x1:
                            PHYMOD_IF_ERR_RETURN(
                                 merlin_dino_configure_pll(pa, MERLIN_DINO_pll_10p3125GHz_161p132MHz));
                        break;
                        case 0x2:
                            PHYMOD_IF_ERR_RETURN(
                                 merlin_dino_configure_pll(pa, MERLIN_DINO_pll_10p3125GHz_156p25MHz));
                        break;
                        case 0x3:
                            PHYMOD_IF_ERR_RETURN(
                                 merlin_dino_configure_pll(pa, MERLIN_DINO_pll_10p9375GHz_156p25MHz));
                        break;
                        default:
                            return PHYMOD_E_PARAM;
                    }
                    /* write 1 to core_dp_s_rstb after pll config*/
                    PHYMOD_IF_ERR_RETURN(
                        wrc_core_dp_s_rstb(1));

                    retry_cnt = DINO_FW_DLOAD_RETRY_CNT;
                    do {
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_PLL_CALSTS0r(pa, &merlin_pll_sts));
                        data = BCMI_DINO_PLL_CALSTS0r_PLL_LOCKf_GET(merlin_pll_sts);
	                    PHYMOD_USLEEP(100);
                    } while((data != 1) && (retry_cnt--));
                    if (retry_cnt == 0) {
   	                    PHYMOD_DEBUG_ERROR(("PLL LOCK failed for pll\n"));       
                        return PHYMOD_E_FAIL;
                    }
                    /* store merlin pll map value into sctratch reg */
                    reg_data &= ~(0x3 << ((2*(lane/4))+(8*if_side)));
                    reg_data |= (mer_pll_mapval << ((2*(lane/4))+(8*if_side)));
                    WRITE_DINO_PMA_PMD_REG(pa, DINO_MER_PLL_MAP_VAL_REG_ADR, reg_data);
                }
            }
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int _dino_restore_phy_type(const phymod_access_t *pa, phymod_phy_inf_config_t* config, uint16_t *phy_type) {
    uint16_t sw_gpreg_data  = 0;
    uint16_t port_idx  = 0;
    uint16_t no_of_ports = 0;
    uint16_t lane_mask = 0;
    *phy_type = DINO_PHY_TYPE_INVALID;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_PORT_FROM_MODE(config, no_of_ports);

    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if (config->data_rate == DINO_SPD_100G || config->data_rate == DINO_SPD_106G) {
            READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
            break;
        } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) && 
                    (lane_mask == DINO_40G_PORT0_LANE_MASK))) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + DINO_PORT0), sw_gpreg_data);
            break;
        } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                    (lane_mask == DINO_40G_PORT1_LANE_MASK))) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + (DINO_PORT1 * DINO_MAX_CORE_LANE)), sw_gpreg_data);
            break;
        } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                    (lane_mask == DINO_40G_PORT2_LANE_MASK))) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + (DINO_PORT2 * DINO_MAX_CORE_LANE)), sw_gpreg_data);
            break;
        } else if (config->data_rate == DINO_SPD_10G || config->data_rate == DINO_SPD_11G || config->data_rate == DINO_SPD_1G) {
            if ((lane_mask & (0x1 << port_idx))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + port_idx), sw_gpreg_data);
                break;
            }
        } else {
            /* Throw error as un supported mode */
            continue;
        }
    }

    *phy_type = ((sw_gpreg_data & DINO_PHY_TYPE_MASK) >> DINO_PHY_TYPE_SHIFT);

    return PHYMOD_E_NONE;
}
int _dino_save_phy_type(const phymod_access_t *pa, 
                         const phymod_phy_inf_config_t *config)
{
    uint16_t lane_idx = 0;
    uint16_t sw_gpreg_data = 0;
    uint16_t lane_mask = 0;
    uint16_t phy_type = DINO_PHY_TYPE_INVALID;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
        phy_type = DINO_PHY_TYPE_HIGIG;
    } else {
        phy_type = DINO_PHY_TYPE_ETH;
    }

    if (config->data_rate == DINO_SPD_100G || config->data_rate == DINO_SPD_106G) {
        lane_mask = 0x1;
    }

    for (lane_idx = 0; lane_idx < DINO_MAX_LANE; lane_idx++) {
        if (lane_mask & (1 << lane_idx)) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + lane_idx), sw_gpreg_data);
            sw_gpreg_data &= ~(DINO_PHY_TYPE_MASK);
            sw_gpreg_data |= (phy_type << DINO_PHY_TYPE_SHIFT);
            WRITE_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + lane_idx), sw_gpreg_data);
        }
    }

    return PHYMOD_E_NONE;
}
int _dino_restore_interface(phymod_phy_access_t *phy, 
                            const phymod_phy_inf_config_t *config,
                            phymod_interface_t *intf) {
    uint16_t if_side   = 0;
    uint16_t sw_gpreg_data  = 0;
    uint16_t port_idx  = 0;
    uint16_t no_of_ports = 0;
    const phymod_access_t *pa = &phy->access;
    uint16_t lane_mask = 0;
    uint16_t interface_type = DINO_INTF_TYPE_INVALID;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    DINO_GET_PORT_FROM_MODE(config, no_of_ports);

    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if (config->data_rate == DINO_SPD_100G || config->data_rate == DINO_SPD_106G) {
            READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
            break;
        } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) && 
                    (lane_mask == DINO_40G_PORT0_LANE_MASK))) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + DINO_PORT0), sw_gpreg_data);
            break;
        } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                    (lane_mask == DINO_40G_PORT1_LANE_MASK))) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + (DINO_PORT1 * DINO_MAX_CORE_LANE)), sw_gpreg_data);
            break;
        } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                    (lane_mask == DINO_40G_PORT2_LANE_MASK))) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + (DINO_PORT2 * DINO_MAX_CORE_LANE)), sw_gpreg_data);
            break;
        } else if (config->data_rate == DINO_SPD_10G || config->data_rate == DINO_SPD_11G || config->data_rate == DINO_SPD_1G) {
            if ((lane_mask & (0x1 << port_idx))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + port_idx), sw_gpreg_data);
                break;
            }
        } else {
            /* Throw error as un supported mode */
            continue;
        }
    }

    if (if_side == DINO_IF_LINE) {
        interface_type = ((sw_gpreg_data & DINO_LINE_INTF_MASK) >> DINO_LINE_INTF_SHIFT);
    } else {
        interface_type = ((sw_gpreg_data & DINO_SYS_INTF_MASK) >> DINO_SYS_INTF_SHIFT);
    }

    switch (interface_type) {
        case DINO_INTF_TYPE_SR:
            if (config->data_rate == DINO_SPD_10G ||
                config->data_rate == DINO_SPD_11G) {
                *intf = phymodInterfaceSR;
            } else if (config->data_rate == DINO_SPD_40G ||
                       config->data_rate == DINO_SPD_42G) { 
                *intf = phymodInterfaceSR4;
            } else if (config->data_rate == DINO_SPD_100G ||
                       config->data_rate == DINO_SPD_106G) {
                if ((PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) &&
                    (if_side == DINO_IF_LINE)) {
                    *intf = phymodInterfaceSR4;
                } else {
                    *intf = phymodInterfaceSR10;
                }
            } 
        break;
        case DINO_INTF_TYPE_LR:
            if (config->data_rate == DINO_SPD_10G ||
                config->data_rate == DINO_SPD_11G) {
                *intf = phymodInterfaceLR;
            } else if (config->data_rate == DINO_SPD_40G ||
                       config->data_rate == DINO_SPD_42G) { 
                *intf = phymodInterfaceLR4;
            } else if (config->data_rate == DINO_SPD_100G ||
                       config->data_rate == DINO_SPD_106G) {
                if ((PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) &&
                    (if_side == DINO_IF_LINE)) {
                    *intf = phymodInterfaceLR4;
                } else {
                    *intf = phymodInterfaceLR10;
                }
            } 
        break;
        case DINO_INTF_TYPE_ER:
            if (config->data_rate == DINO_SPD_10G ||
                config->data_rate == DINO_SPD_11G) {
                *intf = phymodInterfaceER;
            } else if (config->data_rate == DINO_SPD_40G ||
                       config->data_rate == DINO_SPD_42G) {
                *intf = phymodInterfaceER4;
            } else if (config->data_rate == DINO_SPD_100G ||
                       config->data_rate == DINO_SPD_106G) {
                if ((PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) &&
                    (if_side == DINO_IF_LINE)) {
                    *intf = phymodInterfaceER4;
                } else {
                    /* *intf = phymodInterfaceER10; */
                }
            } 
        break;
        case DINO_INTF_TYPE_CAUI4:
            *intf = phymodInterfaceCAUI4;
        break;
        case DINO_INTF_TYPE_CAUI:
            *intf = phymodInterfaceCAUI;
        break;
        case DINO_INTF_TYPE_CAUI4_C2C:
            *intf = phymodInterfaceCAUI4_C2C;
        break;
        case DINO_INTF_TYPE_CAUI4_C2M:
            *intf = phymodInterfaceCAUI4_C2M;
        break;
        case DINO_INTF_TYPE_XLAUI:
            *intf = phymodInterfaceXLAUI;
        break;
        case DINO_INTF_TYPE_XFI:
            *intf = phymodInterfaceXFI;
        break;
        case DINO_INTF_TYPE_KR:
            if (config->data_rate == DINO_SPD_1G) {
                *intf = phymodInterfaceKX;
            } else if (config->data_rate == DINO_SPD_10G ||
                       config->data_rate == DINO_SPD_11G) {
                *intf = phymodInterfaceKR;
            } else if (config->data_rate == DINO_SPD_40G ||
                       config->data_rate == DINO_SPD_42G) { 
                *intf = phymodInterfaceKR4;
            } else if (config->data_rate == DINO_SPD_100G ||
                       config->data_rate == DINO_SPD_106G) {
                if ((PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) &&
                    (if_side == DINO_IF_LINE)) {
                    *intf = phymodInterfaceKR4;
                } else {
                    *intf = phymodInterfaceKR10; 
                }
            } 
        break;
        case DINO_INTF_TYPE_SFI:
            *intf = phymodInterfaceSFI;
        break;
        case DINO_INTF_TYPE_XLPPI:
            *intf = phymodInterfaceXLPPI;
        break;
        case DINO_INTF_TYPE_CR:
            if (config->data_rate == DINO_SPD_1G) {
                *intf = phymodInterfaceCX;
            } else if (config->data_rate == DINO_SPD_10G ||
                config->data_rate == DINO_SPD_11G) {
                *intf = phymodInterfaceCR;
            } else if (config->data_rate == DINO_SPD_40G ||
                       config->data_rate == DINO_SPD_42G) { 
                *intf = phymodInterfaceCR4;
            } else if (config->data_rate == DINO_SPD_100G ||
                       config->data_rate == DINO_SPD_106G) {
                if ((PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) &&
                    (if_side == DINO_IF_LINE)) {
                    *intf = phymodInterfaceCR4;
                } else {
                    *intf = phymodInterfaceCR10; 
                }
            }
        break;
        case DINO_INTF_TYPE_VSR:
            *intf = phymodInterfaceVSR;
        break;    
        default:
        break;
    }
    return PHYMOD_E_NONE;
}
int _dino_save_interface(const phymod_phy_access_t *phy,
                         const phymod_phy_inf_config_t* config, 
                         phymod_interface_t intf)
{
    uint16_t lane_idx = 0;
    uint16_t if_side   = 0;
    uint16_t sw_gpreg_data = 0;
    const phymod_access_t *pa = &phy->access;
    uint16_t lane_mask = 0;
    uint16_t interface_type = DINO_INTF_TYPE_INVALID;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);

    switch (intf) {
        case phymodInterfaceSR:
        case phymodInterfaceSR4:
        case phymodInterfaceSR10:
            interface_type = DINO_INTF_TYPE_SR;
        break;
        case phymodInterfaceLR:
        case phymodInterfaceLR4:
        case phymodInterfaceLR10:
            interface_type = DINO_INTF_TYPE_LR;
        break;
        case phymodInterfaceER:
        case phymodInterfaceER4:
        /* case phymodInterfaceER10: */
            interface_type = DINO_INTF_TYPE_ER;
        break;
        case phymodInterfaceCAUI4:
            interface_type = DINO_INTF_TYPE_CAUI4;
        break;
        case phymodInterfaceCAUI:
            interface_type = DINO_INTF_TYPE_CAUI;
        break;
        case phymodInterfaceCAUI4_C2C:
            interface_type = DINO_INTF_TYPE_CAUI4_C2C;
        break;
        case phymodInterfaceCAUI4_C2M:
            interface_type = DINO_INTF_TYPE_CAUI4_C2M;
        break;
        case phymodInterfaceXLAUI:
            interface_type = DINO_INTF_TYPE_XLAUI;
        break;
        case phymodInterfaceXFI:
            interface_type = DINO_INTF_TYPE_XFI;
        break;
        case phymodInterfaceKX:
        case phymodInterfaceKR:
        case phymodInterfaceKR10:
        case phymodInterfaceKR4:
            interface_type = DINO_INTF_TYPE_KR;
        break;
        case phymodInterfaceSFI:
            interface_type = DINO_INTF_TYPE_SFI;
        break;
        case phymodInterfaceXLPPI:
            interface_type = DINO_INTF_TYPE_XLPPI;
        break;
        case phymodInterfaceCX:
        case phymodInterfaceCR:
        case phymodInterfaceCR4:
        case phymodInterfaceCR10:
            interface_type = DINO_INTF_TYPE_CR;
        break;
        case phymodInterfaceVSR:
            interface_type = DINO_INTF_TYPE_VSR;
        break;
        default:
        break;
    }

    if (config->data_rate == DINO_SPD_100G || config->data_rate == DINO_SPD_106G) {
        lane_mask = 0x1;
    }

    for (lane_idx = 0; lane_idx < DINO_MAX_LANE; lane_idx++) {
        if (lane_mask & (1 << lane_idx)) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + lane_idx), sw_gpreg_data);
            if (if_side == DINO_IF_LINE) {
                sw_gpreg_data &= ~(DINO_LINE_INTF_MASK);
                sw_gpreg_data |= (interface_type << DINO_LINE_INTF_SHIFT);
            } else {
                sw_gpreg_data &= ~(DINO_SYS_INTF_MASK);
                sw_gpreg_data |= (interface_type << DINO_SYS_INTF_SHIFT);
            }
            WRITE_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + lane_idx), sw_gpreg_data);
        }
    }

    return PHYMOD_E_NONE;
}

int _dino_interface_set(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config)
{
    uint16_t DFE_option  = SERDES_DFE_OPTION_NO_DFE; /*default to dfe disable*/
    uint16_t media_type  = SERDES_MEDIA_TYPE_BACK_PLANE; /* Default to Back Plane */
    uint16_t if_side   = 0;
    uint16_t lane_mask   = 0;
    uint16_t gpreg_data  = 0;
    uint16_t port_idx  = 0;
    uint16_t no_of_ports = 0;
    const phymod_access_t *pa = &phy->access;
    phymod_interface_t intf = config->interface_type; 
    DINO_GET_INTF_SIDE(phy, if_side);
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_PORT_FROM_MODE(config, no_of_ports);

    if (intf == phymodInterfaceSR4 || intf == phymodInterfaceSR10 ||
        intf == phymodInterfaceER4 || intf == phymodInterfaceLR10 ||
        intf == phymodInterfaceLR  || intf == phymodInterfaceLR4 ||
        intf == phymodInterfaceSR  || intf == phymodInterfaceER  ||
        intf == phymodInterfaceSFI  || intf == phymodInterfaceXLPPI) {
        media_type = SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS; /*OPTICAL*/
        DFE_option = SERDES_DFE_OPTION_NO_DFE;
    } else if (intf == phymodInterfaceCR4 || intf == phymodInterfaceCR ||
               intf == phymodInterfaceCR10 || intf == phymodInterfaceCX) {
        media_type = SERDES_MEDIA_TYPE_COPPER_CABLE; /*Copper*/
        DFE_option = SERDES_DFE_OPTION_DFE;
    } else if (intf == phymodInterfaceKR4 || intf == phymodInterfaceKR ||
               intf == phymodInterfaceKR10) {
        media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Back plane */
        DFE_option = SERDES_DFE_OPTION_DFE;
    } else if (intf == phymodInterfaceCAUI4 || intf == phymodInterfaceCAUI || 
               intf == phymodInterfaceCAUI4_C2M || intf == phymodInterfaceCAUI4_C2C ||
               intf == phymodInterfaceVSR) {
        media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Back plane */
        DFE_option = SERDES_DFE_OPTION_DFE;
    } else if (intf == phymodInterfaceXLAUI ||
               intf == phymodInterfaceXFI) {
        media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Back plane */
        DFE_option = SERDES_DFE_OPTION_NO_DFE;
    } else {
         PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
                (_PHYMOD_MSG("Invalid interface type..")));
    }
 
    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if (config->data_rate == DINO_SPD_100G || config->data_rate == DINO_SPD_106G) {
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            if (if_side == DINO_IF_LINE) {
                gpreg_data &= ~(DINO_LINE_MEDIA_TYPE_MASK);
                gpreg_data |= (media_type << DINO_LINE_MEDIA_TYPE_SHIFT);
                gpreg_data &= ~(DINO_LINE_DFE_OPTION_MASK);
                gpreg_data |= (DFE_option << DINO_LINE_DFE_OPTION_SHIFT);
            } else {
                gpreg_data &= ~(DINO_SYS_MEDIA_TYPE_MASK);
                gpreg_data |= (media_type << DINO_SYS_MEDIA_TYPE_SHIFT);
                gpreg_data &= ~(DINO_SYS_DFE_OPTION_MASK);
                gpreg_data |= (DFE_option << DINO_SYS_DFE_OPTION_SHIFT);
            }
            WRITE_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
        } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                    (lane_mask == DINO_40G_PORT0_LANE_MASK))) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + DINO_PORT0), gpreg_data);
            if (if_side == DINO_IF_LINE) {
                gpreg_data &= ~(DINO_LINE_MEDIA_TYPE_MASK);
                gpreg_data |= (media_type << DINO_LINE_MEDIA_TYPE_SHIFT);
                gpreg_data &= ~(DINO_LINE_DFE_OPTION_MASK);
                gpreg_data |= (DFE_option << DINO_LINE_DFE_OPTION_SHIFT);
            } else {
                gpreg_data &= ~(DINO_SYS_MEDIA_TYPE_MASK);
                gpreg_data |= (media_type << DINO_SYS_MEDIA_TYPE_SHIFT);
                gpreg_data &= ~(DINO_SYS_DFE_OPTION_MASK);
                gpreg_data |= (DFE_option << DINO_SYS_DFE_OPTION_SHIFT);
            }
            WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + DINO_PORT0), gpreg_data);
        } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                    (lane_mask == DINO_40G_PORT1_LANE_MASK))) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (DINO_PORT1 * DINO_MAX_CORE_LANE)), gpreg_data);
            if (if_side == DINO_IF_LINE) {
                gpreg_data &= ~(DINO_LINE_MEDIA_TYPE_MASK);
                gpreg_data |= (media_type << DINO_LINE_MEDIA_TYPE_SHIFT);
                gpreg_data &= ~(DINO_LINE_DFE_OPTION_MASK);
                gpreg_data |= (DFE_option << DINO_LINE_DFE_OPTION_SHIFT);
            } else {
                gpreg_data &= ~(DINO_SYS_MEDIA_TYPE_MASK);
                gpreg_data |= (media_type << DINO_SYS_MEDIA_TYPE_SHIFT);
                gpreg_data &= ~(DINO_SYS_DFE_OPTION_MASK);
                gpreg_data |= (DFE_option << DINO_SYS_DFE_OPTION_SHIFT);
            }
            WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (DINO_PORT1 * DINO_MAX_CORE_LANE)), gpreg_data);
        } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                    (lane_mask == DINO_40G_PORT2_LANE_MASK))) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (DINO_PORT2 * DINO_MAX_CORE_LANE)), gpreg_data);
            if (if_side == DINO_IF_LINE) {
                gpreg_data &= ~(DINO_LINE_MEDIA_TYPE_MASK);
                gpreg_data |= (media_type << DINO_LINE_MEDIA_TYPE_SHIFT);
                gpreg_data &= ~(DINO_LINE_DFE_OPTION_MASK);
                gpreg_data |= (DFE_option << DINO_LINE_DFE_OPTION_SHIFT);
            } else {
                gpreg_data &= ~(DINO_SYS_MEDIA_TYPE_MASK);
                gpreg_data |= (media_type << DINO_SYS_MEDIA_TYPE_SHIFT);
                gpreg_data &= ~(DINO_SYS_DFE_OPTION_MASK);
                gpreg_data |= (DFE_option << DINO_SYS_DFE_OPTION_SHIFT);
            }
            WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (DINO_PORT2 * DINO_MAX_CORE_LANE)), gpreg_data);
        } else if (config->data_rate == DINO_SPD_10G || config->data_rate == DINO_SPD_11G || config->data_rate == DINO_SPD_1G) {
            if ((lane_mask & (0x1 << port_idx))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + port_idx), gpreg_data);
                if (if_side == DINO_IF_LINE) {
                    gpreg_data &= ~(DINO_LINE_MEDIA_TYPE_MASK);
                    gpreg_data |= (media_type << DINO_LINE_MEDIA_TYPE_SHIFT);
                    gpreg_data &= ~(DINO_LINE_DFE_OPTION_MASK);
                    gpreg_data |= (DFE_option << DINO_LINE_DFE_OPTION_SHIFT);
                } else {
                    gpreg_data &= ~(DINO_SYS_MEDIA_TYPE_MASK);
                    gpreg_data |= (media_type << DINO_SYS_MEDIA_TYPE_SHIFT);
                    gpreg_data &= ~(DINO_SYS_DFE_OPTION_MASK);
                    gpreg_data |= (DFE_option << DINO_SYS_DFE_OPTION_SHIFT);
                }
                WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + port_idx), gpreg_data);
            }
        } else {
            /* Throw error as un supported mode */
        }
    }

    PHYMOD_IF_ERR_RETURN(
        _dino_save_interface(phy, config, intf));

    return PHYMOD_E_NONE;
}

int _dino_get_pll_modes(const phymod_phy_access_t *phy, phymod_ref_clk_t ref_clk, const phymod_phy_inf_config_t *config, uint16_t *fal_pll_mode, uint16_t *mer_pll_mode) 
{
    uint32_t speed = config->data_rate;

    switch (ref_clk) {
        case phymodRefClk156Mhz:
        case phymodRefClk312Mhz:
            /* No OTN support on 156 and 312 */
            if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                return PHYMOD_E_PARAM;
            }
            if (((speed == DINO_SPD_100G) || (speed == DINO_SPD_106G)) &&
                    (PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy))) {
                if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                    *fal_pll_mode = DINO_PLL_MODE_175;
                    *mer_pll_mode = DINO_PLL_MODE_70;
                } else {
                    *fal_pll_mode = DINO_PLL_MODE_165;
                    *mer_pll_mode = DINO_PLL_MODE_66;
                }
            } else {
                /*-- Falcon Line Rate is 10G/20G*/
                if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                    *fal_pll_mode = DINO_PLL_MODE_140;
                    *mer_pll_mode = DINO_PLL_MODE_70;
                } else { /*IEEE MODE*/
                    *fal_pll_mode = DINO_PLL_MODE_132;
                    *mer_pll_mode = DINO_PLL_MODE_66;
                }
            }
        break;
        case phymodRefClk161Mhz:
        case phymodRefClk322Mhz:
        case phymodRefClk644Mhz:
            /* No OTN or HIGIG Support on 161/322/644 */
            if ((PHYMOD_INTF_MODES_HIGIG_GET(config)) ||
                (PHYMOD_INTF_MODES_OTN_GET(config))) {
                return PHYMOD_E_PARAM;
            }
            if (speed == DINO_SPD_100G) {
                /*IEEE MODE*/
                *fal_pll_mode = DINO_PLL_MODE_160;
                *mer_pll_mode = DINO_PLL_MODE_64;
            } else {
                *fal_pll_mode = DINO_PLL_MODE_128;
                *mer_pll_mode = DINO_PLL_MODE_64;
	        }
        break;
        case phymodRefClk174Mhz:
        case phymodRefClk349Mhz: 
        case phymodRefClk698Mhz:
            if (speed == DINO_SPD_100G) {
                if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                    *fal_pll_mode = DINO_PLL_MODE_160;
                    *mer_pll_mode = DINO_PLL_MODE_64;
                } else {
                    return PHYMOD_E_PARAM;
                }
            } else {
                if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                    *fal_pll_mode = DINO_PLL_MODE_128;
                    *mer_pll_mode = DINO_PLL_MODE_64;
                } else {
                    return PHYMOD_E_PARAM;
                }
            }
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}
/** Map Merlin PLL Division
 * @param pll_mode 
 * @return Merlin PLL mode mapped value
 */
int _dino_map_mer_pll_div(DINO_PLL_MODE_E pll_mode) 
{
    switch (pll_mode) {
        case DINO_PLL_MODE_64:
            return 1;
        case DINO_PLL_MODE_66:
            return 2;
        case DINO_PLL_MODE_70:
            return 3;
        default:
	        PHYMOD_DEBUG_VERBOSE(("PLLMODE %d not supported\n", pll_mode));
            return PHYMOD_E_PARAM; 
    }
    return PHYMOD_E_NONE;
}
/**  PMD lock get 
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param if_side             interface side 0 - Line side 1- Sys side
 *    @param lane                lane number
 *    @param rx_pmd_locked        RX PMD locked
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _dino_rx_pmd_lock_get(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, uint32_t *rx_pmd_locked) 
{
    uint8_t pmd_lock = 1;

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN(falcon2_dino_pmd_lock_status(pa, &pmd_lock));
        *rx_pmd_locked &= pmd_lock;
    } else {
        PHYMOD_IF_ERR_RETURN(merlin_dino_pmd_lock_status(pa, &pmd_lock));
        *rx_pmd_locked &= pmd_lock;
    }

    return PHYMOD_E_NONE;
}

int _dino_get_pcs_link_status(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, uint32_t *link_sts)
{
    return _dino_rx_pmd_lock_get(pa, if_side, lane, link_sts);
}


int _dino_phy_status_dump(const phymod_access_t *pa, uint16_t if_side, uint16_t lane)
{

    USR_PRINTF(("**********************************************\n"));
    USR_PRINTF(("******* PHY status dump for PHY ID:%d ********\n", pa->addr));
    USR_PRINTF(("**********************************************\n"));
    USR_PRINTF(("**** PHY status dump for interface side:%d ****\n", if_side));
    USR_PRINTF(("***********************************************\n"));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN(falcon2_dino_display_core_config(pa));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_display_core_state(pa));
        PHYMOD_DEBUG_VERBOSE(("Falcon Status dump\n"));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_display_lane_state_hdr(pa));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_display_lane_state(pa));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_display_lane_config(pa));
    } else {
        PHYMOD_IF_ERR_RETURN(merlin_dino_display_core_config(pa));
        PHYMOD_IF_ERR_RETURN(merlin_dino_display_core_state(pa));
        PHYMOD_DEBUG_VERBOSE(("Merlin Status dump\n"));
        PHYMOD_IF_ERR_RETURN(merlin_dino_display_lane_state_hdr(pa));
        PHYMOD_IF_ERR_RETURN(merlin_dino_display_lane_state(pa));
        PHYMOD_IF_ERR_RETURN(merlin_dino_display_lane_config(pa));
    }

    return PHYMOD_E_NONE;
}

int _dino_loopback_set(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, phymod_loopback_mode_t loopback, uint32_t enable)
{
    uint16_t data = 0;
    BCMI_DINO_FIFO_RST_FRC_CTL1r_t fifo_rst_frc_ln;
    BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_t fifo_rst_frcval_ln;
    BCMI_DINO_FIFO_RST_FRC_CTL0r_t fifo_rst_frc_sys;
    BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_t fifo_rst_frcval_sys;
    PHYMOD_MEMSET(&fifo_rst_frc_ln, 0, sizeof(BCMI_DINO_FIFO_RST_FRC_CTL1r_t));
    PHYMOD_MEMSET(&fifo_rst_frcval_ln, 0, sizeof(BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_t));
    PHYMOD_MEMSET(&fifo_rst_frc_sys, 0, sizeof(BCMI_DINO_FIFO_RST_FRC_CTL0r_t));
    PHYMOD_MEMSET(&fifo_rst_frcval_sys, 0, sizeof(BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_t));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN(
            _dino_falcon_lpbk_set(pa, if_side, lane, loopback, enable));
    } else {
        PHYMOD_IF_ERR_RETURN(
            _dino_merlin_lpbk_set(pa, if_side, lane, loopback, enable));
    }
    /* Perform FIFO reset on ingress and egress path */
    /* Set force val to 0*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL1r(pa, &fifo_rst_frcval_ln));
    data = fifo_rst_frcval_ln.v[0];
    data &= ~(1 << lane);
    BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_SET(fifo_rst_frcval_ln, data);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL1r(pa, fifo_rst_frcval_ln));

    /* Set force to 1*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRC_CTL1r(pa, &fifo_rst_frc_ln));
    data = fifo_rst_frc_ln.v[0];
    data |= (1 << lane);
    BCMI_DINO_FIFO_RST_FRC_CTL1r_SET(fifo_rst_frc_ln, data);

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRC_CTL1r(pa, fifo_rst_frc_ln));

    PHYMOD_USLEEP(50);
    /* Set force val to default*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL1r(pa, &fifo_rst_frcval_ln));
    data = fifo_rst_frcval_ln.v[0];
    data |= (1 << lane);
    BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_SET(fifo_rst_frcval_ln, data);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL1r(pa, fifo_rst_frcval_ln));

    /* Set force to default*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRC_CTL1r(pa, &fifo_rst_frc_ln));
    data = fifo_rst_frc_ln.v[0];
    data &= ~(1 << lane);
    BCMI_DINO_FIFO_RST_FRC_CTL1r_SET(fifo_rst_frc_ln, data);

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRC_CTL1r(pa, fifo_rst_frc_ln));

    /* Set force val to 0*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL0r(pa, &fifo_rst_frcval_sys));
    data = fifo_rst_frcval_sys.v[0];
    data &= ~(1 << lane);
    BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_SET(fifo_rst_frcval_sys, data);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL0r(pa, fifo_rst_frcval_sys));

    /* Set force to 1*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRC_CTL0r(pa, &fifo_rst_frc_sys));
    data = fifo_rst_frc_sys.v[0];
    data |= (1 << lane);
    BCMI_DINO_FIFO_RST_FRC_CTL0r_SET(fifo_rst_frc_sys, data);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRC_CTL0r(pa, fifo_rst_frc_sys));
    PHYMOD_USLEEP(50);

    /* Set force val to default*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL0r(pa, &fifo_rst_frcval_sys));
    data = fifo_rst_frcval_sys.v[0];
    data |= (1 << lane);
    BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_SET(fifo_rst_frcval_sys, data);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL0r(pa, fifo_rst_frcval_sys));

    /* Set force to default*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRC_CTL0r(pa, &fifo_rst_frc_sys));
    data = fifo_rst_frc_sys.v[0];
    data &= ~(1 << lane);
    BCMI_DINO_FIFO_RST_FRC_CTL0r_SET(fifo_rst_frc_sys, data);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRC_CTL0r(pa, fifo_rst_frc_sys));

    return PHYMOD_E_NONE;
}

int _dino_loopback_get(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    PHYMOD_IF_ERR_RETURN(
        _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN(
            _dino_falcon_lpbk_get(pa, if_side, loopback, enable));
    } else {
        PHYMOD_IF_ERR_RETURN(
            _dino_merlin_lpbk_get(pa, if_side, loopback, enable));
    }
    PHYMOD_IF_ERR_RETURN(
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int _dino_falcon_lpbk_get(const phymod_access_t *pa, uint16_t if_side, phymod_loopback_mode_t loopback, uint32_t* enable)
{
	BCMI_DINO_F25G_TLB_RX_DIG_LPBK_CFGr_t dig_lpbk;
	BCMI_DINO_F25G_TLB_TX_RMT_LPBK_CFGr_t rmt_lpbk;
	PHYMOD_MEMSET(&dig_lpbk, 0, sizeof(BCMI_DINO_F25G_TLB_RX_DIG_LPBK_CFGr_t));
	PHYMOD_MEMSET(&rmt_lpbk, 0, sizeof(BCMI_DINO_F25G_TLB_TX_RMT_LPBK_CFGr_t));
    PHYMOD_DEBUG_VERBOSE(("Falcon loopback get\n"));

    switch (loopback) {
        case phymodLoopbackRemotePMD: /* Falcon remote loopback set */
            PHYMOD_IF_ERR_RETURN(
	            BCMI_DINO_READ_F25G_TLB_TX_RMT_LPBK_CFGr(pa,&rmt_lpbk));
	        *enable = BCMI_DINO_F25G_TLB_TX_RMT_LPBK_CFGr_RMT_LPBK_ENf_GET(rmt_lpbk);
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodLoopbackGlobal:
        case phymodLoopbackGlobalPMD: /* Falcon digital loopback set */
            /* Falcon Digital loopback get */
            PHYMOD_IF_ERR_RETURN(
	            BCMI_DINO_READ_F25G_TLB_RX_DIG_LPBK_CFGr(pa, &dig_lpbk));
	        *enable = BCMI_DINO_F25G_TLB_RX_DIG_LPBK_CFGr_DIG_LPBK_ENf_GET(dig_lpbk); 
        break;
        default :
        break;
    }

    return PHYMOD_E_NONE;
}

int _dino_merlin_lpbk_get(const phymod_access_t *pa, uint16_t if_side, phymod_loopback_mode_t loopback, uint32_t* enable)
{
	BCMI_DINO_M10G_TLB_RX_DIG_LPBK_CFGr_t dig_lpbk;
	BCMI_DINO_M10G_TLB_TX_RMT_LPBK_CFGr_t rmt_lpbk;
	PHYMOD_MEMSET(&dig_lpbk, 0, sizeof(BCMI_DINO_M10G_TLB_RX_DIG_LPBK_CFGr_t));
	PHYMOD_MEMSET(&rmt_lpbk, 0, sizeof(BCMI_DINO_M10G_TLB_TX_RMT_LPBK_CFGr_t));
    PHYMOD_DEBUG_VERBOSE(("Falcon loopback get\n"));

    switch (loopback) {
        case phymodLoopbackRemotePMD: /* Falcon remote loopback set */
            PHYMOD_IF_ERR_RETURN(
	            BCMI_DINO_READ_M10G_TLB_TX_RMT_LPBK_CFGr(pa,&rmt_lpbk));
	        *enable = BCMI_DINO_M10G_TLB_TX_RMT_LPBK_CFGr_RMT_LPBK_ENf_GET(rmt_lpbk);
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodLoopbackGlobal:
        case phymodLoopbackGlobalPMD: /* Falcon digital loopback set */
            /* Falcon Digital loopback get */
            PHYMOD_IF_ERR_RETURN(
	            BCMI_DINO_READ_M10G_TLB_RX_DIG_LPBK_CFGr(pa, &dig_lpbk));
	        *enable = BCMI_DINO_M10G_TLB_RX_DIG_LPBK_CFGr_DIG_LPBK_ENf_GET(dig_lpbk); 
        break;
        default :
        break;
    }

    return PHYMOD_E_NONE;
}


int _dino_falcon_lpbk_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, 
                          phymod_loopback_mode_t loopback, uint32_t enable)
{
    uint16_t if_other_side = 0;
    uint16_t lane_index = 0;
    uint16_t dev_op_mode      = DINO_DEV_OP_MODE_INVALID;
    uint16_t sw_gpreg_data    = 0;
    uint16_t other_side_start_lane = 0;
    uint16_t other_side_end_lane = 0;
    BCMI_DINO_DECD_MODE_STS0r_t mode_sts0;
    PHYMOD_MEMSET(&mode_sts0, 0, sizeof(BCMI_DINO_DECD_MODE_STS0r_t));

    (if_side == DINO_IF_LINE) ? (if_other_side = DINO_IF_SYS) : (if_other_side = DINO_IF_LINE); 

    PHYMOD_DEBUG_VERBOSE(("Falcon loopback set\n"));

    switch (loopback) {
        case phymodLoopbackRemotePMD: /* Falcon remote loopback set */
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(falcon2_dino_rmt_lpbk(pa, enable));
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodLoopbackGlobal:
        case phymodLoopbackGlobalPMD: /* Falcon digital loopback set */
            /* Read the GPREG to check if Gear box mode is set.
             * For GBOX mode , we need to program all system side lanes 
             */
            READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
            dev_op_mode = (sw_gpreg_data & DINO_DEV_OP_MODE_MASK) >> DINO_DEV_OP_MODE_SHIFT;  
            if (dev_op_mode == DINO_DEV_OP_MODE_GBOX) {
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_DECD_MODE_STS0r(pa, &mode_sts0));
                if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_GBOX_2TO11f_GET(mode_sts0)) {
                    other_side_start_lane = 2;
                    other_side_end_lane = 11;
                } else if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_GBOX_1TO10f_GET(mode_sts0)) {
                    other_side_start_lane = 1;
                    other_side_end_lane = 10;
                } else if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_GBOX_0TO9f_GET(mode_sts0)) {
                    other_side_start_lane = 0;
                    other_side_end_lane = 9;
                } else {
                    other_side_start_lane = 0;
                    other_side_end_lane = 9;
                }
                for (lane_index = other_side_start_lane; lane_index <= other_side_end_lane; lane_index++) {
                    PHYMOD_IF_ERR_RETURN(
                        _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_other_side, lane_index));
                    PHYMOD_IF_ERR_RETURN(merlin_dino_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
                }
            } else {
                PHYMOD_IF_ERR_RETURN(
                    _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_other_side, lane));
                PHYMOD_IF_ERR_RETURN(merlin_dino_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
            }
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(falcon2_dino_dig_lpbk_rptr(pa, enable, DIG_LPBK_SIDE));
            /* Disable the prbs_chk_en_auto_mode while seting digital loopback */
            PHYMOD_IF_ERR_RETURN(falcon2_dino_wr_prbs_chk_en_auto_mode((!enable)));
            /* TX disable while enabling digital loopback */
            PHYMOD_IF_ERR_RETURN(falcon2_dino_wr_sdk_tx_disable(enable));
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));
        break;
        default :
        break;
    }

    return PHYMOD_E_NONE;
}

int _dino_merlin_lpbk_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane,
                           phymod_loopback_mode_t loopback, uint32_t enable)
{
    uint16_t if_other_side = 0;
    uint16_t other_side_start_lane = 0;
    uint16_t other_side_end_lane = 0;
    uint16_t sw_gpreg_data = 0;
    uint16_t lane_index = 0;
    uint16_t dev_op_mode      = DINO_DEV_OP_MODE_INVALID;

    (if_side == DINO_IF_LINE) ? (if_other_side = DINO_IF_SYS) : (if_other_side = DINO_IF_LINE);

    PHYMOD_DEBUG_VERBOSE(("Merlin loopback set\n"));

    switch (loopback) {
        case phymodLoopbackRemotePMD:  
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(merlin_dino_rmt_lpbk(pa, enable));
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodLoopbackGlobal:
        case phymodLoopbackGlobalPMD: 
            /* Read the GPREG to check if Gear box mode is set.
             * For GBOX mode , we need to program only 4 line side lanes 
             */
            READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
            dev_op_mode = (sw_gpreg_data & DINO_DEV_OP_MODE_MASK) >> DINO_DEV_OP_MODE_SHIFT;  
            if ((if_side == DINO_IF_SYS) && (dev_op_mode == DINO_DEV_OP_MODE_GBOX)) {
                other_side_start_lane = 0;
                other_side_end_lane = 3;
                PHYMOD_IF_ERR_RETURN(
                    _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_other_side, lane));
                for (lane_index = other_side_start_lane; lane_index <= other_side_end_lane; lane_index++) {
                    PHYMOD_IF_ERR_RETURN(
                        _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_other_side, lane_index));
                    PHYMOD_IF_ERR_RETURN(falcon2_dino_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
                }
            } else {
                PHYMOD_IF_ERR_RETURN(
                    _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_other_side, lane));
                if ((if_side == DINO_IF_SYS) && (lane < DINO_MAX_CORE_LANE)) {
                    PHYMOD_IF_ERR_RETURN(falcon2_dino_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
                } else {
                    PHYMOD_IF_ERR_RETURN(merlin_dino_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
                }
            }
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(merlin_dino_dig_lpbk_rptr(pa, enable, DIG_LPBK_SIDE));
            /* Disable the prbs_chk_en_auto_mode while seting digital loopback */
            PHYMOD_IF_ERR_RETURN(wr_prbs_chk_en_auto_mode((!enable)));
            /* TX disable while enabling digital loopback on the line side*/
            if (if_side == DINO_IF_LINE) {
                PHYMOD_IF_ERR_RETURN(wr_sdk_tx_disable(enable));
            }
        break;
        default :
        break;
    }
    PHYMOD_IF_ERR_RETURN(
      _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int _dino_phy_polarity_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, uint16_t tx_polarity, uint16_t rx_polarity)
{
    BCMI_DINO_F25G_TLB_RX_TLB_RX_MISC_CFGr_t fal_rx_pol;
    BCMI_DINO_F25G_TLB_TX_TLB_TX_MISC_CFGr_t fal_tx_pol;
    BCMI_DINO_M10G_TLB_RX_TLB_RX_MISC_CFGr_t mer_rx_pol;
    BCMI_DINO_M10G_TLB_TX_TLB_TX_MISC_CFGr_t mer_tx_pol;
   
    PHYMOD_MEMSET(&fal_rx_pol, 0, sizeof(fal_rx_pol));
    PHYMOD_MEMSET(&fal_tx_pol, 0, sizeof(fal_tx_pol));
    PHYMOD_MEMSET(&mer_tx_pol, 0, sizeof(mer_tx_pol));
    PHYMOD_MEMSET(&mer_rx_pol, 0, sizeof(mer_rx_pol));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        if (tx_polarity != 0xFFFF) {
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_F25G_TLB_TX_TLB_TX_MISC_CFGr(pa, &fal_tx_pol));
            BCMI_DINO_F25G_TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_SET(fal_tx_pol, tx_polarity);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_F25G_TLB_TX_TLB_TX_MISC_CFGr(pa, fal_tx_pol));
        }
        if (rx_polarity != 0xFFFF) {
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_F25G_TLB_RX_TLB_RX_MISC_CFGr(pa, &fal_rx_pol));
            BCMI_DINO_F25G_TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_SET(fal_rx_pol, rx_polarity);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_F25G_TLB_RX_TLB_RX_MISC_CFGr(pa, fal_rx_pol));
        }
    } else {
        if (rx_polarity != 0xFFFF) {
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_M10G_TLB_RX_TLB_RX_MISC_CFGr(pa, &mer_rx_pol));
            BCMI_DINO_M10G_TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_SET(mer_rx_pol, rx_polarity);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_M10G_TLB_RX_TLB_RX_MISC_CFGr(pa, mer_rx_pol));
        }
        if (tx_polarity != 0xFFFF) {
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_M10G_TLB_TX_TLB_TX_MISC_CFGr(pa, &mer_tx_pol));
            BCMI_DINO_M10G_TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_SET(mer_tx_pol, tx_polarity);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_M10G_TLB_TX_TLB_TX_MISC_CFGr(pa, mer_tx_pol));
         }
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_polarity_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, uint16_t *tx_polarity, uint16_t *rx_polarity)
{
    BCMI_DINO_F25G_TLB_RX_TLB_RX_MISC_CFGr_t fal_rx_pol;
    BCMI_DINO_F25G_TLB_TX_TLB_TX_MISC_CFGr_t fal_tx_pol;
    BCMI_DINO_M10G_TLB_RX_TLB_RX_MISC_CFGr_t mer_rx_pol;
    BCMI_DINO_M10G_TLB_TX_TLB_TX_MISC_CFGr_t mer_tx_pol;
   
    PHYMOD_MEMSET(&fal_rx_pol, 0, sizeof(fal_rx_pol));
    PHYMOD_MEMSET(&fal_tx_pol, 0, sizeof(fal_tx_pol));
    PHYMOD_MEMSET(&mer_tx_pol, 0, sizeof(mer_tx_pol));
    PHYMOD_MEMSET(&mer_rx_pol, 0, sizeof(mer_rx_pol));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_F25G_TLB_TX_TLB_TX_MISC_CFGr(pa, &fal_tx_pol));
        *tx_polarity = BCMI_DINO_F25G_TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_GET(fal_tx_pol);
        
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_F25G_TLB_RX_TLB_RX_MISC_CFGr(pa, &fal_rx_pol));
        *rx_polarity = BCMI_DINO_F25G_TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_GET(fal_rx_pol);
    } else {
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_M10G_TLB_RX_TLB_RX_MISC_CFGr(pa, &mer_rx_pol));
        *rx_polarity = BCMI_DINO_M10G_TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_GET(mer_rx_pol);
            
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_M10G_TLB_TX_TLB_TX_MISC_CFGr(pa, &mer_tx_pol));
        *tx_polarity = BCMI_DINO_M10G_TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_GET(mer_tx_pol);
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_fec_set(const phymod_phy_access_t* phy, uint16_t enable)
{
    BCMI_DINO_MODECTL0r_t modectrl0;

    PHYMOD_MEMSET(&modectrl0, 0, sizeof(modectrl0));

    /* this may need to modify in AN mode*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MODECTL0r(&phy->access, &modectrl0));
    BCMI_DINO_MODECTL0r_MODE_CL91_ENf_SET(modectrl0, enable);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MODECTL0r(&phy->access, modectrl0));

    PHYMOD_IF_ERR_RETURN(
        _dino_fw_enable(&phy->access));

    return PHYMOD_E_NONE;
}

int _dino_phy_fec_get(const phymod_phy_access_t* phy, uint32_t *enable)
{
    BCMI_DINO_MODECTL0r_t modectrl0;

    PHYMOD_MEMSET(&modectrl0, 0, sizeof(modectrl0));

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MODECTL0r(&phy->access, &modectrl0));
    *enable = BCMI_DINO_MODECTL0r_MODE_CL91_ENf_GET(modectrl0);

    return PHYMOD_E_NONE;
}

int _dino_phy_power_set(const phymod_phy_access_t* phy, uint16_t if_side, uint16_t lane, const phymod_phy_power_t* power)
{
    BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_t mer_power;
    BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_t fal_power;
    PHYMOD_MEMSET(&mer_power, 0, sizeof(mer_power));
    PHYMOD_MEMSET(&fal_power, 0, sizeof(fal_power));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        if (power->tx != phymodPowerNoChange) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, &fal_power));
            if (power->tx == phymodPowerOff) {
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(fal_power, 1);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(fal_power,1);
            } else if (power->tx == phymodPowerOn) {
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(fal_power,0);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(fal_power, 0);
            } else if (power->tx == phymodPowerOffOn) {
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(fal_power, 1);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(fal_power,1);
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, fal_power));
                PHYMOD_USLEEP(10);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(fal_power, 0);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(fal_power,0);
            }
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, fal_power));
        }
        if (power->rx != phymodPowerNoChange) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, &fal_power));
            if (power->rx == phymodPowerOff) {
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(fal_power, 1);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(fal_power,1);
            } else if (power->rx == phymodPowerOn) {
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(fal_power, 0);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(fal_power,0);
            } else if (power->rx == phymodPowerOffOn) {
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(fal_power, 1);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(fal_power,1);
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, fal_power));
                PHYMOD_USLEEP(10);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(fal_power, 0);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(fal_power,0);
            }
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, fal_power));
        }
    } else {
        if (power->tx != phymodPowerNoChange) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, &mer_power));
            if (power->tx == phymodPowerOff) {
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(mer_power, 1);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(mer_power,1);
            } else if (power->tx == phymodPowerOn) {
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(mer_power,0);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(mer_power, 0);
            } else if (power->tx == phymodPowerOffOn) {
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(mer_power, 1);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(mer_power,1);
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, mer_power));
                PHYMOD_USLEEP(10);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(mer_power, 0);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(mer_power,0);
            }
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, mer_power));
        }
        if (power->rx != phymodPowerNoChange) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, &mer_power));
            if (power->rx == phymodPowerOff) {
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(mer_power, 1);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(mer_power,1);
            } else if (power->rx == phymodPowerOn) {
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(mer_power, 0);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(mer_power,0);
            } else if (power->rx == phymodPowerOffOn) {
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(mer_power, 1);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(mer_power,1);
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, mer_power));
                PHYMOD_USLEEP(10);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(mer_power, 0);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(mer_power,0);
            }
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, mer_power));
        }
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_power_get(const phymod_phy_access_t* phy, uint16_t if_side, uint16_t lane, phymod_phy_power_t* power)
{
    uint16_t force = 0, pwr = 0;
    BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_t mer_power;
    BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_t fal_power;
    PHYMOD_MEMSET(&mer_power, 0 , sizeof(mer_power));
    PHYMOD_MEMSET(&fal_power, 0 , sizeof(fal_power));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN (
            BCMI_DINO_READ_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, &fal_power));
        force = BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_GET(fal_power);
        pwr = BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_GET(fal_power);
        if (force && pwr) {
            power->tx = phymodPowerOff;
        } else {
            power->tx = phymodPowerOn;
        }
        force = BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_GET(fal_power);
        pwr = BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_GET(fal_power);
        if (force && pwr) {
            power->rx = phymodPowerOff;
        } else {
            power->rx = phymodPowerOn;
        }
    } else {
        PHYMOD_IF_ERR_RETURN (
            BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, &mer_power));
        force = BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_GET(mer_power);
        pwr = BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_GET(mer_power);
        if (force && pwr) {
            power->tx = phymodPowerOff;
        } else {
            power->tx = phymodPowerOn;
        }
        force = BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_GET(mer_power);
        pwr = BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_GET(mer_power);
        if (force && pwr) {
            power->rx = phymodPowerOff;
        } else {
            power->rx = phymodPowerOn;
        }
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_reset_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, const phymod_phy_reset_t* reset)
{
    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        if (reset->tx == phymodResetDirectionIn) {
            PHYMOD_IF_ERR_RETURN(
                    falcon2_dino_wr_ln_tx_dp_s_rstb(0));
        } else if (reset->tx == phymodResetDirectionOut) {
            PHYMOD_IF_ERR_RETURN(
                    falcon2_dino_wr_ln_tx_dp_s_rstb(1));
        } else if (reset->tx == phymodResetDirectionInOut) {
            PHYMOD_IF_ERR_RETURN(
                    falcon2_dino_wr_ln_tx_dp_s_rstb(0));
            PHYMOD_USLEEP(10);
            PHYMOD_IF_ERR_RETURN(
                    falcon2_dino_wr_ln_tx_dp_s_rstb(1));
        }
        if (reset->rx == phymodResetDirectionIn) {
            PHYMOD_IF_ERR_RETURN(
               falcon2_dino_wr_ln_rx_dp_s_rstb(0));
        } else if (reset->rx == phymodResetDirectionOut) {
            PHYMOD_IF_ERR_RETURN(
               falcon2_dino_wr_ln_rx_dp_s_rstb(1));
        } else if (reset->rx == phymodResetDirectionInOut) {
            PHYMOD_IF_ERR_RETURN(
               falcon2_dino_wr_ln_rx_dp_s_rstb(0));
            PHYMOD_USLEEP(10);
            PHYMOD_IF_ERR_RETURN(
               falcon2_dino_wr_ln_rx_dp_s_rstb(1));

        }
    } else {
        if (reset->tx == phymodResetDirectionIn) {
            PHYMOD_IF_ERR_RETURN(wr_ln_tx_dp_s_rstb(0));
        } else if (reset->tx == phymodResetDirectionOut) {
            PHYMOD_IF_ERR_RETURN(wr_ln_tx_dp_s_rstb(1));
        } else if (reset->tx == phymodResetDirectionInOut) {
            PHYMOD_IF_ERR_RETURN(wr_ln_tx_dp_s_rstb(0));
            PHYMOD_USLEEP(10);
            PHYMOD_IF_ERR_RETURN(wr_ln_tx_dp_s_rstb(1));
        }
        if (reset->rx == phymodResetDirectionIn) {
            PHYMOD_IF_ERR_RETURN(wr_ln_rx_dp_s_rstb(0));
        } else if (reset->rx == phymodResetDirectionOut) {
            PHYMOD_IF_ERR_RETURN(wr_ln_rx_dp_s_rstb(1));
        } else if (reset->rx == phymodResetDirectionInOut) {
            PHYMOD_IF_ERR_RETURN(wr_ln_rx_dp_s_rstb(0));
            PHYMOD_USLEEP(10);
            PHYMOD_IF_ERR_RETURN(wr_ln_rx_dp_s_rstb(1));
        }
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_reset_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_phy_reset_t* reset)
{
    uint16_t dp_reset = 0,__err = 0 ;

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        dp_reset = falcon2_dino_rd_ln_tx_dp_s_rstb();
        if (!dp_reset) {
            reset->tx = phymodResetDirectionIn; 
        } else {
            reset->tx = phymodResetDirectionOut; 
        }
        dp_reset = falcon2_dino_rd_ln_rx_dp_s_rstb(); 
        if (!dp_reset) {
            reset->rx = phymodResetDirectionIn; 
        } else {
            reset->rx = phymodResetDirectionOut; 
        }
    } else {
        dp_reset = rd_ln_tx_dp_s_rstb();
        if (!dp_reset) {
            reset->tx = phymodResetDirectionIn; 
        } else {
            reset->tx = phymodResetDirectionOut; 
        }
        dp_reset = rd_ln_rx_dp_s_rstb();
        if (!dp_reset) {
            reset->rx = phymodResetDirectionIn; 
        } else {
            reset->rx = phymodResetDirectionOut; 
        }
    }

    return PHYMOD_E_NONE;
}

/*Start\Stop the sequencer*/
int _dino_core_pll_sequencer_restart(const phymod_access_t* pa, uint16_t if_side, 
                                     phymod_sequencer_operation_t operation) 
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    BCMI_DINO_F25G_PLL_CAL_CTL4r_t fal_pll_ctrl;
    BCMI_DINO_PLL_CALCTL5r_t mer_pll_ctrl;

    PHYMOD_MEMSET(&fal_pll_ctrl, 0, sizeof(BCMI_DINO_F25G_PLL_CAL_CTL4r_t));
    PHYMOD_MEMSET(&mer_pll_ctrl, 0, sizeof(BCMI_DINO_PLL_CALCTL5r));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    if (lane_mask == 0) {
        lane_mask = DINO_100G_TYPE1_LANE_MASK;
    }

    for (lane = 0; lane < DINO_MAX_LANE ; lane += DINO_MAX_CORE_LANE) {
        if (lane_mask & (DINO_40G_PORT0_LANE_MASK << lane)) {
            switch (operation) {
                case phymodSeqOpStart:
                    PHYMOD_IF_ERR_RETURN (
                        _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
                    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_READ_F25G_PLL_CAL_CTL4r(pa, &fal_pll_ctrl));
                        BCMI_DINO_F25G_PLL_CAL_CTL4r_PLL_SEQ_STARTf_SET(fal_pll_ctrl,1);
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_WRITE_F25G_PLL_CAL_CTL4r(pa, fal_pll_ctrl));
                    } else {
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_READ_PLL_CALCTL5r(pa, &mer_pll_ctrl));
                        BCMI_DINO_PLL_CALCTL5r_PLL_SEQ_STARTf_SET(mer_pll_ctrl, 1);
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_WRITE_PLL_CALCTL5r(pa, mer_pll_ctrl));
                    }
                break;
                case phymodSeqOpStop:
                    PHYMOD_IF_ERR_RETURN (
                        _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
                    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_READ_F25G_PLL_CAL_CTL4r(pa, &fal_pll_ctrl));
                        BCMI_DINO_F25G_PLL_CAL_CTL4r_PLL_SEQ_STARTf_SET(fal_pll_ctrl,0);
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_WRITE_F25G_PLL_CAL_CTL4r(pa, fal_pll_ctrl));
                    } else {
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_READ_PLL_CALCTL5r(pa, &mer_pll_ctrl));
                        BCMI_DINO_PLL_CALCTL5r_PLL_SEQ_STARTf_SET(mer_pll_ctrl, 0);
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_WRITE_PLL_CALCTL5r(pa, mer_pll_ctrl));
                    }

                    break;
                case phymodSeqOpRestart: 
                    PHYMOD_IF_ERR_RETURN (
                        _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
                    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_READ_F25G_PLL_CAL_CTL4r(pa, &fal_pll_ctrl));
                        BCMI_DINO_F25G_PLL_CAL_CTL4r_PLL_SEQ_STARTf_SET(fal_pll_ctrl,0);
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_WRITE_F25G_PLL_CAL_CTL4r(pa, fal_pll_ctrl));

                        BCMI_DINO_F25G_PLL_CAL_CTL4r_PLL_SEQ_STARTf_SET(fal_pll_ctrl,1);
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_WRITE_F25G_PLL_CAL_CTL4r(pa, fal_pll_ctrl));

                    } else {
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_READ_PLL_CALCTL5r(pa, &mer_pll_ctrl));
                        BCMI_DINO_PLL_CALCTL5r_PLL_SEQ_STARTf_SET(mer_pll_ctrl, 0);
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_WRITE_PLL_CALCTL5r(pa, mer_pll_ctrl));
                        BCMI_DINO_PLL_CALCTL5r_PLL_SEQ_STARTf_SET(mer_pll_ctrl, 1);
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_WRITE_PLL_CALCTL5r(pa, mer_pll_ctrl));
                    }

                break;
                default:
	                return PHYMOD_E_PARAM;
            }
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}


/*TX transmission control*/
int _dino_phy_tx_lane_control_set(const phymod_access_t* pa, uint16_t if_side,
                                  uint16_t lane, phymod_phy_tx_lane_control_t tx_control)
{
    BCMI_DINO_MERLIN_IF_PER_LN_CTL1_PER_LANEr_t mer_tx_disable;
    BCMI_DINO_FALCON_IF_PER_LN_CTL1_PER_LANEr_t fal_tx_disable;
    PHYMOD_MEMSET(&mer_tx_disable, 0, sizeof(mer_tx_disable));
    PHYMOD_MEMSET(&fal_tx_disable, 0, sizeof(fal_tx_disable));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        if (tx_control == phymodTxSquelchOn) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_FALCON_IF_PER_LN_CTL1_PER_LANEr(pa, &fal_tx_disable));
            BCMI_DINO_FALCON_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCf_SET(fal_tx_disable, 1);
            BCMI_DINO_FALCON_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCVALf_SET(fal_tx_disable, 1);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL1_PER_LANEr(pa, fal_tx_disable));
        } else if (tx_control == phymodTxSquelchOff) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_FALCON_IF_PER_LN_CTL1_PER_LANEr(pa, &fal_tx_disable));
            BCMI_DINO_FALCON_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCf_SET(fal_tx_disable, 1);
            BCMI_DINO_FALCON_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCVALf_SET(fal_tx_disable, 0);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL1_PER_LANEr(pa, fal_tx_disable));
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    } else {
        if (tx_control == phymodTxSquelchOn) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL1_PER_LANEr(pa, &mer_tx_disable));
            BCMI_DINO_MERLIN_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCf_SET(mer_tx_disable, 1);
            BCMI_DINO_MERLIN_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCVALf_SET(mer_tx_disable, 1);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL1_PER_LANEr(pa, mer_tx_disable));
        } else if (tx_control == phymodTxSquelchOff) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL1_PER_LANEr(pa, &mer_tx_disable));
            BCMI_DINO_MERLIN_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCf_SET(mer_tx_disable, 1);
            BCMI_DINO_MERLIN_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCVALf_SET(mer_tx_disable, 0);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL1_PER_LANEr(pa, mer_tx_disable));
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_tx_lane_control_get(const phymod_access_t* pa, uint16_t if_side,
                                  uint16_t lane, phymod_phy_tx_lane_control_t* tx_control)
{
    uint16_t tx_dis_frc = 0, tx_dis_frc_val = 0;
    BCMI_DINO_MERLIN_IF_PER_LN_CTL1_PER_LANEr_t mer_tx_disable;
    BCMI_DINO_FALCON_IF_PER_LN_CTL1_PER_LANEr_t fal_tx_disable;
    PHYMOD_MEMSET(&mer_tx_disable, 0, sizeof(mer_tx_disable));
    PHYMOD_MEMSET(&fal_tx_disable, 0, sizeof(fal_tx_disable));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN (
            BCMI_DINO_READ_FALCON_IF_PER_LN_CTL1_PER_LANEr(pa, &fal_tx_disable));
        if (*tx_control == phymodTxSquelchOn || *tx_control == phymodTxSquelchOff) {
            tx_dis_frc = BCMI_DINO_FALCON_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCf_GET(fal_tx_disable);
            tx_dis_frc_val = BCMI_DINO_FALCON_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCVALf_GET(fal_tx_disable);
            if (tx_dis_frc && tx_dis_frc_val) {
                *tx_control = phymodTxSquelchOn;
            } else {
                *tx_control = phymodTxSquelchOff;
            }
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    } else {
        PHYMOD_IF_ERR_RETURN (
            BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL1_PER_LANEr(pa, &mer_tx_disable));
        if (*tx_control == phymodTxSquelchOn || *tx_control == phymodTxSquelchOff) {
            tx_dis_frc = BCMI_DINO_MERLIN_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCf_GET(mer_tx_disable);
            tx_dis_frc_val = BCMI_DINO_MERLIN_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCVALf_GET(mer_tx_disable);
            if (tx_dis_frc && tx_dis_frc_val) {
                *tx_control = phymodTxSquelchOn;
            } else {
                *tx_control = phymodTxSquelchOff;
            }
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    }
    return PHYMOD_E_NONE;
}

/*Rx control*/
int _dino_phy_rx_lane_control_set(const phymod_access_t* pa, uint16_t if_side,
                                  uint16_t lane, phymod_phy_rx_lane_control_t rx_control)
{
    BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_t mer_power;
    BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_t fal_power;
    PHYMOD_MEMSET(&mer_power, 0 , sizeof(mer_power));
    PHYMOD_MEMSET(&fal_power, 0 , sizeof(fal_power));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        if (rx_control == phymodRxSquelchOff) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_FALCON_IF_PER_LN_CTL2_PER_LANEr(pa, &fal_power));
            BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(fal_power, 0);
            BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(fal_power, 0);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL2_PER_LANEr(pa, fal_power));

        } else if (rx_control == phymodRxSquelchOn) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_FALCON_IF_PER_LN_CTL2_PER_LANEr(pa, &fal_power));
            BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(fal_power, 1);
            BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(fal_power, 1);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL2_PER_LANEr(pa, fal_power));

        } else {
            return PHYMOD_E_UNAVAIL;
        }
    } else {
        if (rx_control == phymodRxSquelchOff) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL2_PER_LANEr(pa, &mer_power));
            BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(mer_power,0);
            BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(mer_power, 0);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL2_PER_LANEr(pa, mer_power));
        } else if (rx_control == phymodRxSquelchOn) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL2_PER_LANEr(pa, &mer_power));
            BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(mer_power, 1);
            BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(mer_power,1);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL2_PER_LANEr(pa, mer_power));
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_rx_lane_control_get(const phymod_access_t* pa, uint16_t if_side,
                                  uint16_t lane, phymod_phy_rx_lane_control_t* rx_control)
{
    uint16_t force = 0, pwr = 0;
    BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_t mer_power;
    BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_t fal_power;
    PHYMOD_MEMSET(&mer_power, 0 , sizeof(mer_power));
    PHYMOD_MEMSET(&fal_power, 0 , sizeof(fal_power));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN (
            BCMI_DINO_READ_FALCON_IF_PER_LN_CTL2_PER_LANEr(pa, &fal_power));
        if (*rx_control == phymodRxSquelchOn || *rx_control == phymodRxSquelchOff) {
            force = BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_GET(fal_power);
            pwr = BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_GET(fal_power);
            if (force && pwr) {
                *rx_control = phymodRxSquelchOn;
            } else {
                *rx_control = phymodRxSquelchOff;
            }
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    } else {
        PHYMOD_IF_ERR_RETURN (
            BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL2_PER_LANEr(pa, &mer_power));
        if (*rx_control == phymodRxSquelchOn || *rx_control == phymodRxSquelchOff) {
            force = BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_GET(mer_power);
            pwr = BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_GET(mer_power);
            if (force && pwr) {
                *rx_control = phymodRxSquelchOn;
            } else {
                *rx_control = phymodRxSquelchOff;
            }
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    }

    return PHYMOD_E_NONE;
}
