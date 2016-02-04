/*
* $Copyright: Copyright 2015 Broadcom Corporation.
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
#include "bcmi_madura_defs.h"
#include "madura_cfg_seq.h"
#include "madura_sdk_interface.h"
#include "madura_serdes/common2/srds_api_enum.h"
#include "madura_serdes/falcon2_madura_src/falcon2_madura_functions.h"

extern unsigned char madura_falcon_ucode[];
extern unsigned char madura_falcon_slave_ucode[];
extern unsigned int madura_falcon_ucode_len;
extern unsigned int madura_falcon_ucode_slave_len;

int madura_get_chipid (const phymod_access_t *pa, uint32_t *chipid, uint32_t *rev) 
{
    BCMI_MADURA_CHIP_IDr_t cid;
    BCMI_MADURA_CHIP_REVISIONr_t rid;

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_CHIP_IDr(pa, &cid));
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_CHIP_REVISIONr(pa, &rid));

    *chipid = (BCMI_MADURA_CHIP_REVISIONr_CHIP_ID_19_16f_GET(rid) << 16 ) |
               BCMI_MADURA_CHIP_IDr_GET(cid);
    *rev = BCMI_MADURA_CHIP_REVISIONr_CHIP_REVf_GET(rid);

    PHYMOD_DEBUG_VERBOSE(("CHIP ID: %x REv:%x\n", *chipid, *rev));
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
int _madura_wait_mst_msgout(const phymod_access_t *pa,
                            MADURA_MSGOUT_E exp_message,
                            int poll_time)
{
    int retry_count = MADURA_FW_DLOAD_RETRY_CNT;
    MADURA_MSGOUT_E msgout = MSGOUT_DONTCARE;/*0*/
    BCMI_MADURA_MST_MSGOUTr_t msgout_t; 
    do {
	    /* Read general control msg out  Register */
        PHYMOD_IF_ERR_RETURN(
	        BCMI_MADURA_READ_MST_MSGOUTr(pa,&msgout_t));
	    msgout = BCMI_MADURA_MST_MSGOUTr_MST_MSGOUT_VALf_GET(msgout_t); 
	    /* wait before reading again */
	    if (poll_time != 0) {
		    /* sleep for specified seconds*/
		    PHYMOD_SLEEP(poll_time);
	    }
	    retry_count --;
    } while ((msgout != exp_message) &&  retry_count);
    
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

int _madura_fw_enable(const phymod_access_t *pa, uint16_t en_dis) 
{
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));

    BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en,en_dis);
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_FIRMWARE_ENr(pa,fw_en));

    return PHYMOD_E_NONE;
}
int madura_micro_download(const phymod_access_t *pa, unsigned char *ucode,
                         uint32_t len, uint16_t master, uint16_t *chk_sum) 
{

    BCMI_MADURA_UC_BOOT_PORr_t   boot_por;
    BCMI_MADURA_MST_MSGINr_t msg_in;
    BCMI_MADURA_MST_RUNNING_BYTE_CNTr_t mst_byte_cnt;
    BCMI_MADURA_MST_RUNNING_CHKSUMr_t run_chksum;
    BCMI_MADURA_SLV1_RUNNING_CHKSUMr_t slv1_chksum;
    BCMI_MADURA_SLV2_RUNNING_CHKSUMr_t slv2_chksum;
    BCMI_MADURA_SLV3_RUNNING_CHKSUMr_t slv3_chksum;
    BCMI_MADURA_SLV4_RUNNING_CHKSUMr_t slv4_chksum;

    uint16_t num_bytes = 0, j = 0;
    uint16_t num_words = 0;
    uint16_t check_sum = 0, cnt = MADURA_FW_DLOAD_RETRY_CNT, data = 0;

    PHYMOD_MEMSET(&boot_por, 0 , sizeof(BCMI_MADURA_UC_BOOT_PORr_t));
    PHYMOD_MEMSET(&msg_in, 0 , sizeof(BCMI_MADURA_MST_MSGINr_t));
    PHYMOD_MEMSET(&mst_byte_cnt, 0 , sizeof(BCMI_MADURA_MST_RUNNING_BYTE_CNTr_t));
    PHYMOD_MEMSET(&run_chksum, 0 , sizeof(BCMI_MADURA_MST_RUNNING_CHKSUMr_t));
    PHYMOD_MEMSET(&slv1_chksum, 0 , sizeof(BCMI_MADURA_SLV1_RUNNING_CHKSUMr_t));
    PHYMOD_MEMSET(&slv2_chksum, 0 , sizeof(BCMI_MADURA_SLV2_RUNNING_CHKSUMr_t));
    PHYMOD_MEMSET(&slv3_chksum, 0 , sizeof(BCMI_MADURA_SLV3_RUNNING_CHKSUMr_t));
    PHYMOD_MEMSET(&slv4_chksum, 0 , sizeof(BCMI_MADURA_SLV4_RUNNING_CHKSUMr_t));

	/* Send boot address if master enabled */
    if(master){
	    PHYMOD_IF_ERR_RETURN(
			    _madura_wait_mst_msgout(pa, MSGOUT_NEXT, 0));

        /* coverity[operator_confusion] */
	    BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,0);
	    PHYMOD_IF_ERR_RETURN(
			    BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));
    }
    PHYMOD_IF_ERR_RETURN(
        _madura_wait_mst_msgout(pa, MSGOUT_NEXT, 0));

    /* Calculate the number of words */
    num_words = (len) / 2;

    /* Update message in value field with word length */
    BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,num_words);
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));
    num_bytes = len;
    for (j = 0; j < num_bytes; j += 2) {
	/*sending next ucode data*/
        data = (ucode[j + 1] << 8) | ucode[j];
        check_sum ^= ucode[j] ^ ucode[j + 1];
    
        /* Send next word */
    BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,data);
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));
    }
    do {
	    /* check download_done flags*/
	    PHYMOD_IF_ERR_RETURN(
			    BCMI_MADURA_READ_UC_BOOT_PORr(pa,&boot_por));
        if (master) {
	    if(BCMI_MADURA_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por)==1){
                PHYMOD_DEBUG_VERBOSE(("Master Dload Done\n"));
                break;
            }
        } else {
	    if(BCMI_MADURA_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por)==1){
                PHYMOD_DEBUG_VERBOSE(("Slave Dload Done\n"));
                break;
            }
        }
    } while (cnt --);

    if (!cnt) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT, (_PHYMOD_MSG("Firmware download failed")));
    }

	/* check checksum */
    check_sum = (check_sum == 0) ? 0x600D : check_sum;
    if (master)  {
	    PHYMOD_IF_ERR_RETURN(
			    BCMI_MADURA_READ_MST_RUNNING_BYTE_CNTr(pa,&mst_byte_cnt));
       data=BCMI_MADURA_MST_RUNNING_BYTE_CNTr_MST_RUNNING_BYTE_CNT_VALf_GET(mst_byte_cnt);
        if (data != num_bytes) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,(_PHYMOD_MSG("Master Dload fail:")));
       }
 
        PHYMOD_DEBUG_VERBOSE(("Master Byte Cnt:%d\n", data));

		PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_RUNNING_CHKSUMr(pa,&run_chksum ));

        data=BCMI_MADURA_MST_RUNNING_CHKSUMr_MST_RUNNING_CHKSUM_VALf_GET(run_chksum);
        if (check_sum != data) {
		PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
				(_PHYMOD_MSG("Master Dload chksum Fail")));
        } 
        PHYMOD_DEBUG_VERBOSE(("Master Chk sum:0x%x\n", data));
        *chk_sum = data;
        } else {
        /*slave checksum*/
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_READ_SLV1_RUNNING_CHKSUMr(pa,&slv1_chksum));
		data=BCMI_MADURA_SLV1_RUNNING_CHKSUMr_SLV1_RUNNING_CHKSUM_VALf_GET(slv1_chksum);
		if (check_sum != data) {
			PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
					(_PHYMOD_MSG("Slave1 Dload chksum Fail")));
		} 
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_READ_SLV2_RUNNING_CHKSUMr(pa,&slv2_chksum));
		data=BCMI_MADURA_SLV2_RUNNING_CHKSUMr_SLV2_RUNNING_CHKSUM_VALf_GET(slv2_chksum);
		if (check_sum != data) {
			PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
					(_PHYMOD_MSG("Slave2 Dload chksum Fail")));
		} 
		/* skipping slave 3 since the package is supported  for 3 slaves only  
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_READ_SLV3_RUNNING_CHKSUMr(pa,&slv3_chksum));
		data=BCMI_MADURA_SLV3_RUNNING_CHKSUMr_SLV3_RUNNING_CHKSUM_VALf_GET(slv3_chksum);
		if (check_sum != data) {
			PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
					(_PHYMOD_MSG("Slave3 Dload chksum Fail")));
		} 
		*/
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_READ_SLV4_RUNNING_CHKSUMr(pa,&slv4_chksum));
		data=BCMI_MADURA_SLV4_RUNNING_CHKSUMr_SLV4_RUNNING_CHKSUM_VALf_GET(slv4_chksum);
		if (check_sum != data) {
			PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
					(_PHYMOD_MSG("Slave4 Dload chksum Fail")));
		} 

         } 
	PHYMOD_DEBUG_VERBOSE(("Slave cksum Cnt:0x%x\n", data));
        *chk_sum = data;
    return PHYMOD_E_NONE;
}

int _madura_firmware_download(const phymod_access_t *pm_acc ,
                            unsigned char *madura_master_ucode, unsigned int master_len,
                            unsigned char *madura_slave_ucode, unsigned int slave_len,
                            unsigned char prg_eeprom)
{
    uint16_t data1 = 0, retry_cnt = MADURA_FW_DLOAD_RETRY_CNT;
    uint16_t eeprom_num_of_pages = 0;
    uint16_t mst_check_sum = 0, slv_check_sum = 0;
    BCMI_MADURA_SPI_CODE_LOAD_ENr_t spi_code_load; 
    BCMI_MADURA_GEN_CTL2r_t gen_ctrl2;   
    BCMI_MADURA_GEN_CTL3r_t gen_ctrl3;
    BCMI_MADURA_UC_BOOT_PORr_t   boot_por;
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    BCMI_MADURA_FIRMWARE_VERSIONr_t fw_ver;
    
    BCMI_MADURA_BOOTr_t boot;
    BCMI_MADURA_GPREGBr_t gregB;
    BCMI_MADURA_GPREGCr_t gregC;
    BCMI_MADURA_GPREGEr_t gregE;
    BCMI_MADURA_GPREGFr_t gregF;
    BCMI_MADURA_FIRMWARE_FEATURESr_t firm_features;

    PHYMOD_MEMSET(&firm_features, 0, sizeof(BCMI_MADURA_FIRMWARE_FEATURESr_t));
    PHYMOD_MEMSET(&spi_code_load, 0 , sizeof(BCMI_MADURA_SPI_CODE_LOAD_ENr_t));
    PHYMOD_MEMSET(&gen_ctrl2, 0 , sizeof(BCMI_MADURA_GEN_CTL2r_t));
    PHYMOD_MEMSET(&gen_ctrl3, 0 , sizeof(BCMI_MADURA_GEN_CTL3r_t));
    PHYMOD_MEMSET(&boot_por, 0 , sizeof(BCMI_MADURA_UC_BOOT_PORr_t));
    PHYMOD_MEMSET(&boot, 0 , sizeof(BCMI_MADURA_BOOTr_t));
    PHYMOD_MEMSET(&gregB, 0 , sizeof(BCMI_MADURA_GPREGBr_t));
    PHYMOD_MEMSET(&gregC, 0 , sizeof(BCMI_MADURA_GPREGCr_t));
    PHYMOD_MEMSET(&gregE, 0 , sizeof(BCMI_MADURA_GPREGEr_t));
    PHYMOD_MEMSET(&gregF, 0 , sizeof(BCMI_MADURA_GPREGFr_t));
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));
    PHYMOD_MEMSET(&fw_ver, 0 , sizeof(BCMI_MADURA_FIRMWARE_VERSIONr_t));

    /* Skip the firmware download
     *  mst_download, slv_dwld_done bits are set
     */
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_READ_UC_BOOT_PORr(pm_acc,&boot_por));
   if((BCMI_MADURA_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por))&&
	(BCMI_MADURA_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por))){
     /*force DOWNLOAD*/
    }
 
    /* Do chip hard reset and Resetting the Cores */
    PHYMOD_IF_ERR_RETURN(
        _madura_core_reset_set(pm_acc, phymodResetModeHard, phymodResetDirectionInOut));
    /* Put Master under Reset */
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));
    BCMI_MADURA_GEN_CTL2r_MST_RSTBf_SET(gen_ctrl2,0);/*resetting M0 micro*/
    BCMI_MADURA_GEN_CTL2r_MST_UCP_RSTBf_SET(gen_ctrl2,0);/*resetting M0 micro perip*/
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));

    /* Wait for any pending SPI download
     * SPI download is not interrupted by Master Reset
     * (this is a safety feature) so we need to wait its 
     * completion before starting the MDIO Download */
    do {
        PHYMOD_IF_ERR_RETURN(
	        BCMI_MADURA_READ_BOOTr(pm_acc,&boot));
	    data1=BCMI_MADURA_BOOTr_SERBOOT_BUSYf_GET(boot); 
	    PHYMOD_USLEEP(1000);
    } while((data1 != 0) && (retry_cnt--));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
              (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
        return PHYMOD_E_FAIL;
    }

    /* Madura always supports serial mode if Dload
     * Master followed by slave 
     * Configure master_code_download_en, slave_code_download_en
     * and broadcast enable */
    BCMI_MADURA_SPI_CODE_LOAD_ENr_SLV_CODE_DOWNLOAD_ENf_SET(spi_code_load,0xB);/*Only 3 cores are supported for now*/
    BCMI_MADURA_SPI_CODE_LOAD_ENr_MST_CODE_DOWNLOAD_ENf_SET(spi_code_load,1);
    BCMI_MADURA_SPI_CODE_LOAD_ENr_CODE_BROADCAST_ENf_SET(spi_code_load,1);

    BCMI_MADURA_WRITE_SPI_CODE_LOAD_ENr(pm_acc,spi_code_load);

	/*Read values what is set above*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_SPI_CODE_LOAD_ENr(pm_acc,&spi_code_load));
    if (BCMI_MADURA_SPI_CODE_LOAD_ENr_CODE_BROADCAST_ENf_GET(spi_code_load) != 1 
	&& BCMI_MADURA_SPI_CODE_LOAD_ENr_MST_CODE_DOWNLOAD_ENf_GET(spi_code_load)!= 1
            && BCMI_MADURA_SPI_CODE_LOAD_ENr_SLV_CODE_DOWNLOAD_ENf_GET(spi_code_load) != 0xB) {
        return PHYMOD_E_INTERNAL;
    }
	/* Configure GPREG for programming eeprom */
    if (prg_eeprom) {
        eeprom_num_of_pages = (uint16_t)master_len/MADURA_M0_EEPROM_PAGE_SIZE;

        /* Start Page of EEPROM for master*/
	BCMI_MADURA_GPREGEr_CLR(gregE);
    PHYMOD_IF_ERR_RETURN(
	    BCMI_MADURA_WRITE_GPREGEr(pm_acc,gregE));/*set to 0*/       
    PHYMOD_IF_ERR_RETURN(
	    BCMI_MADURA_READ_GPREGFr(pm_acc,&gregF));
	BCMI_MADURA_GPREGFr_GPREGF_DATAf_SET(gregF,eeprom_num_of_pages);
	BCMI_MADURA_WRITE_GPREGFr(pm_acc,gregF);/*set to number of pages*/
        
	/* Start Page of EEPROM for slave*/
	BCMI_MADURA_GPREGBr_CLR(gregB);
    PHYMOD_IF_ERR_RETURN(
	    BCMI_MADURA_WRITE_GPREGBr(pm_acc,gregB));/*set to 0*/       
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_GPREGCr(pm_acc,&gregC));
	BCMI_MADURA_GPREGCr_GPREGC_DATAf_SET(gregC,eeprom_num_of_pages);
	BCMI_MADURA_WRITE_GPREGCr(pm_acc,gregC);/*set to number of pages*/

	/* Reset spi */
	/* Assert Reset */
	PHYMOD_IF_ERR_RETURN(
			BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));
	BCMI_MADURA_GEN_CTL2r_SPI2X_RSTBf_SET(gen_ctrl2,0);/*resetting SPI2x rstb*/
	BCMI_MADURA_GEN_CTL2r_SPI_RSTBf_SET(gen_ctrl2,0);/*resetting SPI rstb*/
	PHYMOD_IF_ERR_RETURN(
			BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));

	/* Deassert Reset */
	BCMI_MADURA_GEN_CTL2r_SPI2X_RSTBf_SET(gen_ctrl2,1);
	BCMI_MADURA_GEN_CTL2r_SPI_RSTBf_SET(gen_ctrl2,1);
	PHYMOD_IF_ERR_RETURN(
			BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));
    }
    /* Select SPI Speed*/
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_GEN_CTL3r(pm_acc,&gen_ctrl3));
    BCMI_MADURA_GEN_CTL3r_UCSPI_SLOWf_SET(gen_ctrl3,1);
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_GEN_CTL3r(pm_acc,gen_ctrl3));	

    /* Check the not downloading branch of the flow digram
     * before writing to serboot.
     * wait_mst_msgout(chip_cfg, die, MSGOUT_NOT_DWNLD, 1, 0);*/
    
    /* Read / Write boot_por
     *  write to mst_dwld_done and slv_dwld_done.
     *  write to spi_port_used
     *  write to serboot */
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_UC_BOOT_PORr(pm_acc,&boot_por));
    /* Force Master New Download */
    BCMI_MADURA_UC_BOOT_PORr_MST_DWLD_DONEf_SET(boot_por,0);
    /* Force Slave New Download */
    BCMI_MADURA_UC_BOOT_PORr_SLV_DWLD_DONEf_SET(boot_por,0);
    BCMI_MADURA_UC_BOOT_PORr_SERBOOTf_SET(boot_por,1);
    BCMI_MADURA_UC_BOOT_PORr_SPI_PORT_USEDf_SET(boot_por,prg_eeprom);/*set to 0 for mdio default download on reset*/
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_UC_BOOT_PORr(pm_acc,boot_por));

    /* RELEASE Master under Reset */
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));
    BCMI_MADURA_GEN_CTL2r_MST_UCP_RSTBf_SET(gen_ctrl2,1); 
    BCMI_MADURA_GEN_CTL2r_MST_RSTBf_SET(gen_ctrl2,1);
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));

    /* MDI/I2C Download */
    /* Waiting for serboot_busy */
    retry_cnt = MADURA_FW_DLOAD_RETRY_CNT;
    do {
        PHYMOD_IF_ERR_RETURN(
	        BCMI_MADURA_READ_BOOTr(pm_acc,&boot));
	    data1=BCMI_MADURA_BOOTr_SERBOOT_BUSYf_GET(boot); 
        PHYMOD_USLEEP(1000);
    } while ((data1 == 0) && (retry_cnt--));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
              (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
        return PHYMOD_E_FAIL;
    }
    /* Start download sequence */
    /* master boot*/
    PHYMOD_IF_ERR_RETURN(
       madura_micro_download(pm_acc, madura_master_ucode,
                             master_len, 1, &mst_check_sum));

    PHYMOD_IF_ERR_RETURN(
       madura_micro_download(pm_acc, madura_slave_ucode,
                             slave_len, 0, &slv_check_sum));
    PHYMOD_IF_ERR_RETURN (
      _madura_wait_mst_msgout(pm_acc, MSGOUT_DWNLD_DONE, 0));
    
    /* Check serboot_busy and serboot_done_once*/
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_BOOTr(pm_acc,&boot));
    data1=BCMI_MADURA_BOOTr_SERBOOT_BUSYf_GET(boot); 
    if (data1 != 0) {
        PHYMOD_DEBUG_ERROR(("WARN:SERBOOT BUSY HAS UNEXPECTED VALUE\n"));
    }
    data1=BCMI_MADURA_BOOTr_SERBOOT_DONE_ONCEf_GET(boot); 
    if (data1 != 1) {
        PHYMOD_DEBUG_ERROR(("WARN:SERBOOT DONE ONCE HAS UNEXPECTED VALUE\n"));
    }

    /* check download_done flags again */
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_UC_BOOT_PORr(pm_acc,&boot_por));
    if (BCMI_MADURA_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por)!= 1 || BCMI_MADURA_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por) != 1) {
        PHYMOD_DEBUG_ERROR(("WARN:Download Done got cleared\n"));
    }
    /* Wait for Program EEPROM*/
    if (prg_eeprom) {
        _madura_wait_mst_msgout(pm_acc, MSGOUT_PRGRM_DONE,0);
        /* Assert SPI Reset*/
    	PHYMOD_IF_ERR_RETURN(
			BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));
	BCMI_MADURA_GEN_CTL2r_SPI2X_RSTBf_SET(gen_ctrl2,0);/*resetting SPI2x rstb*/
	BCMI_MADURA_GEN_CTL2r_SPI_RSTBf_SET(gen_ctrl2,0);/*resetting SPI rstb*/
	PHYMOD_IF_ERR_RETURN(
			BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));
    }
    retry_cnt = MADURA_FW_DLOAD_RETRY_CNT;
   
     /*Apply workaround for chip A0*/
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_FIRMWARE_FEATURESr(pm_acc,&firm_features));
    BCMI_MADURA_FIRMWARE_FEATURESr_FW_FEATURESf_SET(firm_features,0x8000|BCMI_MADURA_FIRMWARE_FEATURESr_FW_FEATURESf_GET(firm_features));
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_FIRMWARE_FEATURESr(pm_acc,firm_features));

    /*setting fw enable bit*/
    /* coverity[operator_confusion] */
    BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en,1);
    BCMI_MADURA_WRITE_FIRMWARE_ENr(pm_acc,fw_en);
    do {
	    BCMI_MADURA_READ_FIRMWARE_ENr(pm_acc,&fw_en);
	    data1=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
	    PHYMOD_USLEEP(100);
    } while ((data1 != 0) && (retry_cnt --));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Fireware download failed, micro controller is busy..")));
    }
   
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_FIRMWARE_VERSIONr(pm_acc,&fw_ver));

    data1=BCMI_MADURA_FIRMWARE_VERSIONr_FIRMWARE_VERSION_VALf_GET(fw_ver); 
    PHYMOD_DEBUG_VERBOSE(("FW Version:0x%x\n", data1));

    return PHYMOD_E_NONE;
}

int _madura_core_init(const phymod_core_access_t* core,
                     const phymod_core_init_config_t* init_config)
{
    int ret_val = 0; 
    uint32_t rev = 0, chip_id = 0;
    phymod_access_t pa;
    const phymod_access_t *pm_acc = &core->access;
    BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_t pad_ctrl;
    BCMI_MADURA_GEN_CTL2r_t gen_ctrl2;
    BCMI_MADURA_FIRMWARE_FEATURESr_t firm_features;
   
    PHYMOD_MEMSET(&gen_ctrl2, 0, sizeof(BCMI_MADURA_GEN_CTL2r_t));
    PHYMOD_MEMSET(&pad_ctrl, 0, sizeof(BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_t));

    PHYMOD_MEMSET(&firm_features, 0, sizeof(BCMI_MADURA_FIRMWARE_FEATURESr_t));
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_READ_FIRMWARE_FEATURESr(pm_acc,&firm_features));
    switch(init_config->firmware_load_method)
    {
        case phymodFirmwareLoadMethodInternal:
            PHYMOD_DEBUG_VERBOSE((
               "Starting Firmware download through MDIO, it takes few seconds...\n"));

            /* In case of FW Download is only for second die i.e for odd address of the die
             * To get the micro out of reset on even die(previous die); for MDIO only
             * Applicable for only duel die chips;
             * MADURA_CHIP_ID_82790 is 82790 is single die ingnore the micro out of reset
             */
            PHYMOD_IF_ERR_RETURN(madura_get_chipid(pm_acc, &chip_id, &rev));
	    PHYMOD_MEMCPY(&pa, pm_acc, sizeof(phymod_access_t));
            
	    /*if Die B.
	      In case only DIE_B is selected:
	      - put DIE_A M0 under reset
	      - Release reset of DIE_B M0*/
	    if (((pm_acc->addr & 0x1) == 0x1) && (chip_id != MADURA_CHIP_ID_82864)) {
                /* modify the mdio address to be even */
                pa.addr &= ~(0x1);
                /* Set the 11th and 9th bit of even die to get the micro out of reset */
	
		/* Put DIE_A Master under Reset
		 - This ensures that Die_A is not downloading at the same time of Die_B*/	
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_READ_GEN_CTL2r(&pa,&gen_ctrl2));
		BCMI_MADURA_GEN_CTL2r_MST_RSTBf_SET(gen_ctrl2,0);/*resetting M0 micro*/
		BCMI_MADURA_GEN_CTL2r_MST_UCP_RSTBf_SET(gen_ctrl2,0);/*resetting M0 micro perip*/
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_WRITE_GEN_CTL2r(&pa,gen_ctrl2));
		
		/*Release reset of Die_B*/	
		PHYMOD_IF_ERR_RETURN(
			BCMI_MADURA_READ_EXT_UC_RSTB_OUT_CTLr(&pa,&pad_ctrl));
		BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_EXT_UC_RSTB_OUT_OUT_FRCVALf_SET(pad_ctrl,1);
		BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_EXT_UC_RSTB_OUT_IBOFf_SET(pad_ctrl,1);
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_WRITE_EXT_UC_RSTB_OUT_CTLr(&pa,pad_ctrl));
	    }else{ 
		    /*ALL Dies*/
		    /*Restore ext_uc_rstb in die A*/
		PHYMOD_IF_ERR_RETURN(
			BCMI_MADURA_READ_EXT_UC_RSTB_OUT_CTLr(&pa,&pad_ctrl));
		BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_EXT_UC_RSTB_OUT_OUT_FRCVALf_SET(pad_ctrl,0);
		BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_EXT_UC_RSTB_OUT_IBOFf_SET(pad_ctrl,0);
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_WRITE_EXT_UC_RSTB_OUT_CTLr(&pa,pad_ctrl));
	    }

            ret_val = _madura_firmware_download(pm_acc,
                               madura_falcon_ucode, madura_falcon_ucode_len,
                               madura_falcon_slave_ucode, madura_falcon_ucode_slave_len,
                               0);
            if ((ret_val != PHYMOD_E_NONE) && (ret_val != MADURA_FW_ALREADY_DOWNLOADED)) {
                PHYMOD_RETURN_WITH_ERR
                    (ret_val,
                    (_PHYMOD_MSG("firmware download failed")));
            } else {
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
            PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_CONFIG,
                 (_PHYMOD_MSG("illegal fw load method")));
    }
    /*check if fw runs!*/
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_READ_FIRMWARE_FEATURESr(pm_acc,&firm_features));
    return PHYMOD_E_NONE; 
}

int _madura_set_slice_reg (
    const phymod_access_t* pm_acc,
    uint16_t               ip,
    uint16_t               dev_type,
    uint16_t               lane)
{
    BCMI_MADURA_SLICEr_t slice_reg;
    BCMI_MADURA_AN_SLICEr_t slice_an_reg;
    uint16_t sys_sel=0,lin_sel=0,lane_no=0,slice_0=0,slice_1=0;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&slice_an_reg, 0 , sizeof(BCMI_MADURA_AN_SLICEr_t));
    
    /*selecting sys or line side*/	
    sys_sel = (ip==MADURA_FALCON_CORE)? 1 : 0;	
    lin_sel = (ip==MADURA_FALCON_CORE)? 0 : 1;

    /*set lane*/
    lane_no=1<<((lane%4));	
    if(lane<4)
    {
        slice_0=1;	
    }else{
	slice_1=1;
    }	
 
    if (dev_type == MADURA_DEV_PMA_PMD) {
        PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_SLICEr(pm_acc,&slice_reg));
        BCMI_MADURA_SLICEr_SYS_SELf_SET(slice_reg,sys_sel);
        BCMI_MADURA_SLICEr_LIN_SELf_SET(slice_reg,lin_sel);
        BCMI_MADURA_SLICEr_SLICE_0_SELf_SET(slice_reg,slice_0);
        BCMI_MADURA_SLICEr_SLICE_1_SELf_SET(slice_reg,slice_1);
        BCMI_MADURA_SLICEr_LANE_SELf_SET(slice_reg,lane_no);
	BCMI_MADURA_SLICEr_REG_TYPE_SELf_SET(slice_reg,0);
        PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_SLICEr(pm_acc,slice_reg));
	PHYMOD_DEBUG_VERBOSE(("\n[-- ip=%x slice = %x ===]\n",ip,BCMI_MADURA_SLICEr_GET(slice_reg)));
    } else {
        PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_AN_SLICEr(pm_acc,&slice_an_reg));
        BCMI_MADURA_AN_SLICEr_LIN_SELf_SET(slice_an_reg,sys_sel);
        BCMI_MADURA_AN_SLICEr_SYS_SELf_SET(slice_an_reg,lin_sel);
	BCMI_MADURA_AN_SLICEr_SLICE_0_SELf_SET(slice_an_reg,slice_0);
	BCMI_MADURA_AN_SLICEr_SLICE_1_SELf_SET(slice_an_reg,slice_1);
        BCMI_MADURA_AN_SLICEr_REG_TYPE_SELf_SET(slice_an_reg,0);
        BCMI_MADURA_AN_SLICEr_LANE_SELf_SET(slice_an_reg,lane_no);
        PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_AN_SLICEr(pm_acc,slice_an_reg));
    }
    
    return PHYMOD_E_NONE;
}

void _madura_lane_cast_get(const phymod_access_t *pa, MADURA_CORE_TYPE ip, 
                          uint16_t *cast_type, uint16_t *mcast_val) 
{
}

int _madura_rx_pmd_lock_get(const phymod_access_t* pa, uint32_t* rx_pmd_locked)
{
    BCMI_MADURA_TLB_RX_PMD_RX_LOCK_STSr_t rx_lck;
    BCMI_MADURA_SLICEr_t slice_reg;
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t temp = 0,intf_side=0;
    uint16_t lane_mask = 0, max_lane = 0;
 
    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&rx_lck, 0 , sizeof(BCMI_MADURA_TLB_RX_PMD_RX_LOCK_STSr_t));
    if (rx_pmd_locked) {
        *rx_pmd_locked = 0xffff;
    } else {
        return PHYMOD_E_PARAM;
    }

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    MADURA_GET_INTF_SIDE(pa, intf_side); 
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa, 
                        ip, MADURA_DEV_PMA_PMD, lane)));
	    
            /*check pmd rx lock*/ 
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                  BCMI_MADURA_READ_TLB_RX_PMD_RX_LOCK_STSr(pa,&rx_lck)));

            temp=BCMI_MADURA_TLB_RX_PMD_RX_LOCK_STSr_PMD_RX_LOCKf_GET(rx_lck);

            *rx_pmd_locked &= temp;
	    }
    }
 
    /*Reset DEV1 slice register*/	     	
    BCMI_MADURA_WRITE_SLICEr(pa,slice_reg);
    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}

int madura_is_lane_mask_set(const phymod_phy_access_t *pa, uint16_t prt, 
                           const phymod_phy_inf_config_t *config)
{
    phymod_phy_inf_config_t cfg;
    uint16_t core = 0;
    uint32_t lane_mask = 0;
    MADURA_DEVICE_AUX_MODE_T  *aux;
    const phymod_access_t *acc = &pa->access;

    lane_mask = pa->access.lane_mask;
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));
    aux = (MADURA_DEVICE_AUX_MODE_T*)config->device_aux_modes;
    MADURA_GET_IP(acc, cfg, core) ;
    PHYMOD_DEBUG_VERBOSE(("%s :: core:%d port:%d\n", __func__, core, prt));
    if (core == MADURA_2X_FALCON_CORE) {
        if ((cfg.data_rate == MADURA_SPD_40G)||(cfg.data_rate == MADURA_SPD_100G)) {
            if (prt == 0) {
                /* coverity[identical_branches] */
                if (aux->pass_thru_dual_lane) {
                    return (lane_mask  == 0x3) ? 1 : 0; 
                } else if (aux->pass_thru) {
                    if(!aux->alternate){
                        return ((lane_mask & 0xf) == 0xF) ? 1 : 0 ;
                    }else{	
                        return ((lane_mask & 0xf0) == 0xF0) ? 1 : 0 ;
                    }
                } else {
                    return ((lane_mask & 0xF) == 0xF) ? 1 : 0;
                }
            } else {
                if (aux->pass_thru_dual_lane) {
                    return (lane_mask  == 0xC) ? 1 : 0; 
                } else if (aux->pass_thru) {
                    return 0;
                } else {
                    return ((lane_mask & 0xF0) == 0xf0) ? 1 : 0; 
                }
            }
        }
#ifdef MADURA_UNTESTED_CODE
        if (cfg.data_rate == MADURA_SPD_20G) {
            /* For 20G PT and MUX we are using same lane MAP*/
            if (prt == 0) {
                return ((lane_mask & 0x3) == 0x3) ? 1 :0;
            } else {
                return ((lane_mask & 0x30) == 0x30) ? 1 :0;
            }
        }
#endif
        if (cfg.data_rate == MADURA_SPD_10G) {
            /* For 10G PT and alternate modes*/
            if (prt == 0) {
                if (!aux->alternate) {
                    return (lane_mask & 0x1) ? 1 :0;
                }else{
                    return (lane_mask & 0x10) ? 1 :0;
		}
            } else if (prt == 1) {
                if (!aux->alternate) {
	    	    return (lane_mask & 0x2) ? 1 :0;
                }else{
	    	    return (lane_mask & 0x20) ? 1 :0;
		}
            } else if (prt == 2) {
                if (!aux->alternate) {
                    return (lane_mask & 0x4) ? 1 :0;
                } else {
                    return (lane_mask & 0x40) ? 1 :0;
                }
            } else if (prt == 3) {
                if (!aux->alternate) {
                    return (lane_mask & 0x8) ? 1 :0;
                } else {
                    return (lane_mask & 0x80) ? 1 :0;
                }
            }
        }
    } else { /*System SIDE*/
        if ((cfg.data_rate == MADURA_SPD_40G)||(cfg.data_rate == MADURA_SPD_100G)) {
            if (prt == 0) {
                if (aux->pass_thru_dual_lane) {
                    return (lane_mask  == 0x3) ? 1 : 0; 
                } else if (aux->pass_thru) {
                    return ((lane_mask & 0xf) == 0xF) ? 1 : 0 ;
                } else {
                    return ((lane_mask & 0x3) == 0x3) ? 1 : 0;
                }
            } else {
                if (aux->pass_thru_dual_lane) {
                    return (lane_mask  == 0xC) ? 1 : 0; 
                } else if (aux->pass_thru) {
                    return 0;
                } else {
                    return ((lane_mask & 0xc) == 0xc) ? 1 : 0;
                }
            }
        }
#ifdef MADURA_UNTESTED_CODE
        if (cfg.data_rate == MADURA_SPD_20G) {
            if (prt == 0) {
                if (aux->pass_thru) {
                    return ((lane_mask & 0x3) == 0x3) ? 1 :0;
                } else {
                    return (lane_mask & 0x1) ? 1 :0;
                }
            } else {
                if (aux->pass_thru) {
                    return ((lane_mask & 0xc) == 0xc) ? 1 :0;
                } else {
                    return (lane_mask & 0x4) ? 1 :0;
                }
            }
        }
#endif
        if (cfg.data_rate == MADURA_SPD_10G) {
             /* For 10G PT with  1 core disabled */
            if (prt == 0) {
                return (lane_mask & 0x1) ? 1 :0; 
            } else if (prt == 1) {
                return (lane_mask & 0x2) ? 1 :0;
            } else if (prt == 2) {
                return (lane_mask & 0x4) ? 1 :0;
            } else if (prt == 3) {
                return (lane_mask & 0x8) ? 1 :0;
            }
        }
    }
    return 0;
}
/** Configure Dut Mode Register
 */
int _madura_config_dut_mode_reg(const phymod_phy_access_t *pa, const phymod_phy_inf_config_t* config) 
{

    BCMI_MADURA_P0_MODE_CTLr_t mode_ctrl0;
    BCMI_MADURA_P1_MODE_CTLr_t mode_ctrl1;
    BCMI_MADURA_P2_MODE_CTLr_t mode_ctrl2;
    BCMI_MADURA_P3_MODE_CTLr_t mode_ctrl3;
    unsigned char              mindex0 = 0xFF;
    unsigned char              mindex1 = 0xFF;
    unsigned char              mindex2 = 0xFF;
    unsigned char              mindex3 = 0xFF;


    uint16_t no_of_ports = 0, port_idx = 0;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;
    uint16_t core = 0;
    uint16_t is_lane_used = 0;
    const phymod_access_t *acc = &pa->access;
    phymod_phy_inf_config_t cfg;

    PHYMOD_MEMSET(&mode_ctrl0, 0, sizeof(BCMI_MADURA_P0_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl1, 0, sizeof(BCMI_MADURA_P1_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl2, 0, sizeof(BCMI_MADURA_P2_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl3, 0, sizeof(BCMI_MADURA_P3_MODE_CTLr_t));
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));
    MADURA_GET_IP(acc, cfg, core);

    aux_mode = (MADURA_DEVICE_AUX_MODE_T*)config->device_aux_modes;
    MADURA_GET_PORT_FROM_MODE(config, no_of_ports, aux_mode);

    if (config->data_rate != MADURA_SPD_100G) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_P0_MODE_CTLr(&pa->access, &mode_ctrl0));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_P2_MODE_CTLr(&pa->access, &mode_ctrl2));
        for (port_idx=0; port_idx < no_of_ports; port_idx++) {
            if(config->data_rate == MADURA_SPD_10G) {
                /* coverity[identical_branches] */
                if (core == MADURA_FALCON_CORE) {
                    if (pa->access.lane_mask & (1 << port_idx)) {
                       is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                } else { 
                    if (pa->access.lane_mask & (1 << port_idx)) {
                       is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                }
            } else if(config->data_rate == MADURA_SPD_40G &&
                      aux_mode->pass_thru_dual_lane) {
                /* coverity[identical_branches] */
                if (core == MADURA_2X_FALCON_CORE) {
                    if (pa->access.lane_mask & (0x3 << (port_idx * 2))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (pa->access.lane_mask & (0x3 << (port_idx * 2))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                        continue;
                    }
                }
            } else if(config->data_rate == MADURA_SPD_40G &&
                      (!aux_mode->pass_thru)) {
                if (core == MADURA_2X_FALCON_CORE) {
                    /* MUX Mode*/
                    if (pa->access.lane_mask & (0xF << (port_idx*4))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (pa->access.lane_mask & (0x3 << (port_idx * 2))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                        continue;
                    }
                }
#ifdef MADURA_UNTESTED_CODE
           } else if(config->data_rate == MADURA_SPD_20G) {
               if (aux_mode->pass_thru) {
                   if (core == MADURA_2X_FALCON_CORE) {
                       if (pa->access.lane_mask & (0x3 << (port_idx * 4))) {
                           is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                       } else {
                           continue;
                       }
                   } else {
                       if (pa->access.lane_mask & (0x3 << (port_idx * 2))) {
                           is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                       } else {
                           continue;
                       }
                   }
               } else { /* MUX mode*/
                   if (core == MADURA_2X_FALCON_CORE) {
                       if (pa->access.lane_mask & (0x3 << (port_idx * 4))) {
                           is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                       } else {
                           continue;
                       }
                   } else { /* Falcon*/
                       if (pa->access.lane_mask & (0x1 << (port_idx * 2))) {
                           is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                       } else {
                           continue;
                       }
                   }
               }
#endif
           } else {
                is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
           }
           if (!is_lane_used) {
                return PHYMOD_E_PARAM;
           }
           
            /* Program Modes */
            if (((config->data_rate == MADURA_SPD_40G /* || config->data_rate == MADURA_SPD_20G */) && (port_idx == 0)) ||
                 ((config->data_rate == MADURA_SPD_10G) && (port_idx < 2))) {
                if (config->data_rate == MADURA_SPD_40G) {
                    if (aux_mode->pass_thru_dual_lane) {
                        mindex0 = MADURA_MIDX_40GPT20_0;
                    } else if (aux_mode->pass_thru) {
                        mindex0 = MADURA_MIDX_40GPT10_0 ;
                    } else {
                        mindex0 = MADURA_MIDX_40GDM_0B ;
                    }
                    mindex1 = 0;
                }
                if (config->data_rate == MADURA_SPD_20G) {
                    mindex0 = MADURA_MIDX_40GPT20_0 ;
                }
                if (config->data_rate == MADURA_SPD_10G) {
                    if ((pa->access.lane_mask & 0x1) || (pa->access.lane_mask & 0x10)) { 
                        mindex0 = MADURA_MIDX_10GPT_0 ;
                    } 
                    if ((pa->access.lane_mask & 0x2) || (pa->access.lane_mask & 0x20)) { 
                        mindex1 = MADURA_MIDX_10GPT_1 ;
                    } 
                }
            } else {
                if (config->data_rate == MADURA_SPD_40G) {
                    if (aux_mode->pass_thru_dual_lane) {
                        mindex2 = MADURA_MIDX_40GPT20_1;
                    } else if (aux_mode->pass_thru) {
                        mindex2 = MADURA_MIDX_40GPT10_0 ;
                    } else {
                        mindex2 = MADURA_MIDX_40GDM_1B ;
                    }
                }
#ifdef MADURA_UNTESTED_CODE
                if (config->data_rate == MADURA_SPD_20G) {
                    mindex2 = MADURA_MIDX_40GPT20_1 ;
                }
#endif
                if (config->data_rate == MADURA_SPD_10G) {
                    if ((pa->access.lane_mask & 0x4) || (pa->access.lane_mask & 0x40)) { 
                        mindex2 = MADURA_MIDX_10GPT_2 ;
                    } 
                    if ((pa->access.lane_mask & 0x8) || (pa->access.lane_mask & 0x80)) { 
                        mindex3 = MADURA_MIDX_10GPT_3 ;
                    }
                }
            }
        }
    }

    if (mindex0 == 0xFF  && mindex2 == 0xFF ) {
        if (config->data_rate == MADURA_SPD_100G) {
            mindex0 = MADURA_MIDX_100GPT_0;
            mindex1 = 0;
        }
    }

    PHYMOD_DEBUG_VERBOSE(("MODE CTRL0 :0x%x\n", mindex0));
    PHYMOD_DEBUG_VERBOSE(("MODE CTRL1 :0x%x\n", mindex1));
    PHYMOD_DEBUG_VERBOSE(("MODE CTRL2 :0x%x\n", mindex2));
    PHYMOD_DEBUG_VERBOSE(("MODE CTRL3 :0x%x\n", mindex3));

    if (mindex0 != 0xFF) {
        BCMI_MADURA_P0_MODE_CTLr_P0_PVALIDf_SET(mode_ctrl0, 1);
        BCMI_MADURA_P0_MODE_CTLr_P0_MINDEXf_SET(mode_ctrl0, mindex0);
        BCMI_MADURA_WRITE_P0_MODE_CTLr(&pa->access, mode_ctrl0);
    }
    if (mindex1 != 0xFF) {
	    if (config->data_rate == MADURA_SPD_10G) {
		    BCMI_MADURA_P1_MODE_CTLr_P1_PVALIDf_SET(mode_ctrl1, 1);
	     }else{
		    BCMI_MADURA_P1_MODE_CTLr_P1_PVALIDf_SET(mode_ctrl1, 0);
	     }
        BCMI_MADURA_P1_MODE_CTLr_P1_MINDEXf_SET(mode_ctrl1, mindex1);
        BCMI_MADURA_WRITE_P1_MODE_CTLr(&pa->access, mode_ctrl1);
    }
    if (mindex2 != 0xFF) {
        BCMI_MADURA_P2_MODE_CTLr_P2_PVALIDf_SET(mode_ctrl2, 1);
        BCMI_MADURA_P2_MODE_CTLr_P2_MINDEXf_SET(mode_ctrl2, mindex2);
        BCMI_MADURA_WRITE_P2_MODE_CTLr(&pa->access, mode_ctrl2);
    }
    if (mindex3 != 0xFF) {
        BCMI_MADURA_P3_MODE_CTLr_P3_PVALIDf_SET(mode_ctrl3, 1);
        BCMI_MADURA_P3_MODE_CTLr_P3_MINDEXf_SET(mode_ctrl3, mindex3);
        BCMI_MADURA_WRITE_P3_MODE_CTLr(&pa->access, mode_ctrl3);
    }

    return PHYMOD_E_NONE;
}

int _madura_toggle_mer_fal_lane_reset(const phymod_access_t *pa) 
{
    return PHYMOD_E_NONE;
}

/** Configure Dut Mode Register
 */
int _madura_udms_config(const phymod_phy_access_t *pa, const phymod_phy_inf_config_t* config,
                        MADURA_AN_MODE mode)
{
    BCMI_MADURA_P0_MODE_CTLr_t mode_ctrl0;
    BCMI_MADURA_P1_MODE_CTLr_t mode_ctrl1;
    BCMI_MADURA_P2_MODE_CTLr_t mode_ctrl2;
    BCMI_MADURA_P3_MODE_CTLr_t mode_ctrl3;
    unsigned char              udmsen0 = 0xFF;
    unsigned char              udmsen1 = 0xFF;
    unsigned char              udmsen2 = 0xFF;
    unsigned char              udmsen3 = 0xFF;
    unsigned char              dmept0 = 0xFF;
    unsigned char              dmept2 = 0xFF;

    uint16_t no_of_ports = 0, port_idx = 0;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;
    const phymod_access_t *acc;
    phymod_phy_inf_config_t cfg;
    uint16_t core = 0;
    uint16_t is_lane_used = 0;

    PHYMOD_MEMSET(&mode_ctrl0, 0, sizeof(BCMI_MADURA_P0_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl1, 0, sizeof(BCMI_MADURA_P1_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl2, 0, sizeof(BCMI_MADURA_P2_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl3, 0, sizeof(BCMI_MADURA_P3_MODE_CTLr_t));
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));

    acc = &pa->access;
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*)config->device_aux_modes;

    MADURA_GET_PORT_FROM_MODE(config, no_of_ports, aux_mode);
    MADURA_GET_IP(acc, cfg, core);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P0_MODE_CTLr(&pa->access, &mode_ctrl0));
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P1_MODE_CTLr(&pa->access, &mode_ctrl1));
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P2_MODE_CTLr(&pa->access, &mode_ctrl2));
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P3_MODE_CTLr(&pa->access, &mode_ctrl3));

    if (config->data_rate != MADURA_SPD_100G) {
        for (port_idx=0; port_idx < no_of_ports; port_idx++) {
            if(config->data_rate == MADURA_SPD_10G) {
                /* coverity[identical_branches] */
                if (core == MADURA_FALCON_CORE) {
                    if (pa->access.lane_mask & (1 << port_idx)) {
                       is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (pa->access.lane_mask & (1 << port_idx)) {
                       is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                }
            } else if(config->data_rate == MADURA_SPD_40G &&
                      aux_mode->pass_thru_dual_lane) {
                /* coverity[identical_branches] */
                if (core == MADURA_2X_FALCON_CORE) {
                    if (pa->access.lane_mask & (0x3 << (port_idx * 2))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (pa->access.lane_mask & (0x3 << (port_idx * 2))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                        continue;
                    }
                }
            } else if(config->data_rate == MADURA_SPD_40G &&
                      (!aux_mode->pass_thru)) {
                if (core == MADURA_2X_FALCON_CORE) {
                    /* MUX Mode*/
                    if (pa->access.lane_mask & (0xF << (port_idx*4))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (pa->access.lane_mask & (0x3 << (port_idx * 2))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                        continue;
                    }
                }
#ifdef MADURA_UNTESTED_CODE
           } else if(config->data_rate == MADURA_SPD_20G) {
               if (aux_mode->pass_thru) {
                   if (core == MADURA_2X_FALCON_CORE) {
                       if (pa->access.lane_mask & (0x3 << (port_idx * 4))) {
                           is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                       } else {
                           continue;
                       }
                   } else {
                       if (pa->access.lane_mask & (0x3 << (port_idx * 2))) {
                           is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                       } else {
                           continue;
                       }
                   }
               } else { /* MUX mode*/
                   if (core == MADURA_2X_FALCON_CORE) {
                       if (pa->access.lane_mask & (0x3 << (port_idx * 4))) {
                           is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                       } else {
                           continue;
                       }
                   } else { /* Falcon*/
                       if (pa->access.lane_mask & (0x1 << (port_idx * 2))) {
                           is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                       } else {
                           continue;
                       }
                   }
               }
#endif
           } else {
                is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
           }
           if (!is_lane_used) {
                return PHYMOD_E_PARAM;
           }

           /*
            READ_MADURA_PMA_PMD_REG(acc, DP_MADURA_MODE_CTRL1_ADR, mode_ctrl1.data);
            */

            /* Program Modes */
            if (((config->data_rate == MADURA_SPD_40G /*|| config->data_rate == MADURA_SPD_20G */) && (port_idx == 0)) ||
                     ((config->data_rate == MADURA_SPD_10G) && (port_idx < 2))) {
                udmsen0 =  (mode == MADURA_AN_NONE || mode == MADURA_AN_CL73) ? 1 : 0;
                if (config->data_rate == MADURA_SPD_20G ||
                    config->data_rate == MADURA_SPD_40G) {
                    dmept0 = (mode == MADURA_AN_PROP) ? 1 : 0;
                } else if(config->data_rate == MADURA_SPD_10G) {
                    if (pa->access.lane_mask & 0x1) { 
                        dmept0 = (mode == MADURA_AN_PROP) ? 1 : 0;
                    } else if (pa->access.lane_mask & 0x2) {
                        udmsen1 =  (mode == MADURA_AN_NONE || mode == MADURA_AN_CL73) ? 1 : 0;
                        dmept0 = (mode == MADURA_AN_PROP) ? 1 : 0;
                    }
                }
            } else {
                udmsen2 = (mode == MADURA_AN_NONE || mode == MADURA_AN_CL73) ? 1 : 0;
                if (config->data_rate == MADURA_SPD_20G ||
                     config->data_rate == MADURA_SPD_40G) {
                    dmept2 = (mode == MADURA_AN_PROP) ? 1 : 0;
                } else if(config->data_rate == MADURA_SPD_10G) {
                    if (pa->access.lane_mask & 0x4) { 
                        dmept2 = (mode == MADURA_AN_PROP) ? 1 : 0;
                    } else if (pa->access.lane_mask & 0x8) {
                        udmsen3 = (mode == MADURA_AN_NONE || mode == MADURA_AN_CL73) ? 1 : 0;
                        dmept2 = (mode == MADURA_AN_PROP) ? 1 : 0;
                    }
                }
            }
        }
    } else {
        udmsen0 = (mode == MADURA_AN_NONE || mode == MADURA_AN_CL73) ? 1 : 0;
    }
    PHYMOD_DEBUG_VERBOSE(("MODE CTRL1 :0x%x\n", udmsen0));

    if (udmsen0 != 0xFF) {
        BCMI_MADURA_P0_MODE_CTLr_P0_PVALIDf_SET(mode_ctrl0, 1);
        BCMI_MADURA_P0_MODE_CTLr_P0_UDMSf_SET(mode_ctrl0, udmsen0);
        if (dmept0 != 0xFF) {
            BCMI_MADURA_P0_MODE_CTLr_P0_DPTf_SET(mode_ctrl0, dmept0);
        }
        BCMI_MADURA_WRITE_P0_MODE_CTLr(&pa->access, mode_ctrl0);
    }
    if (udmsen1 != 0xFF) {
        BCMI_MADURA_P1_MODE_CTLr_P1_PVALIDf_SET(mode_ctrl1, 1);
        BCMI_MADURA_P1_MODE_CTLr_P1_UDMSf_SET(mode_ctrl1, udmsen1);
        if (dmept0 != 0xFF) {
            BCMI_MADURA_P1_MODE_CTLr_P1_DPTf_SET(mode_ctrl1, dmept0);
        }
        BCMI_MADURA_WRITE_P1_MODE_CTLr(&pa->access, mode_ctrl1);
    }
    if (udmsen2 != 0xFF) {
        BCMI_MADURA_P2_MODE_CTLr_P2_PVALIDf_SET(mode_ctrl2, 1);
        BCMI_MADURA_P2_MODE_CTLr_P2_UDMSf_SET(mode_ctrl2, udmsen2);
        if (dmept2 != 0xFF) {
            BCMI_MADURA_P2_MODE_CTLr_P2_DPTf_SET(mode_ctrl2, dmept2);
        }
        BCMI_MADURA_WRITE_P2_MODE_CTLr(&pa->access, mode_ctrl2);
    }
    if (udmsen3 != 0xFF) {
        BCMI_MADURA_P3_MODE_CTLr_P3_PVALIDf_SET(mode_ctrl3, 1);
        BCMI_MADURA_P3_MODE_CTLr_P3_UDMSf_SET(mode_ctrl3, udmsen3);
        if (dmept2 != 0xFF) {
            BCMI_MADURA_P3_MODE_CTLr_P3_DPTf_SET(mode_ctrl3, dmept2);
        }
        BCMI_MADURA_WRITE_P3_MODE_CTLr(&pa->access, mode_ctrl3);
    }
    return PHYMOD_E_NONE;
}

int _madura_interface_set(const phymod_access_t *pa, const phymod_phy_inf_config_t* config)
{
    uint16_t DFE_option = SERDES_DFE_OPTION_NO_DFE; /*default to dfe disable*/
    uint16_t media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Default to Back Plane */
    uint16_t Tx_timing_mode = MADURA_TX_REPEATER;
    uint16_t   Tx_training_en = 0 ; 
    uint16_t intf = config->interface_type;
    uint16_t config_reg=0;
    uint16_t intf_side = 0;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;
    BCMI_MADURA_GPREG6r_t gpreg6;
    
    PHYMOD_MEMSET(&gpreg6, 0, sizeof(BCMI_MADURA_GPREG6r_t));
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_GPREG6r(pa, &gpreg6));
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));

    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));

    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));

    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));


    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        config_reg = BCMI_MADURA_MST_MISC0r_GET(mst_misc0);
    } else  if (lane_mask == 0x2) {
        config_reg = BCMI_MADURA_MST_MISC1r_GET(mst_misc1);
    } else  if (lane_mask == 0xC || lane_mask == 0x4 ) {
        config_reg = BCMI_MADURA_MST_MISC2r_GET(mst_misc2);
    } else {
        config_reg = BCMI_MADURA_MST_MISC3r_GET(mst_misc3);
    }

    /* 
     * Workaround for firmware issue :
     * Leaving  MISC registers at init value of zero does not work
     * with the current firmware
     */
    if  (BCMI_MADURA_MST_MISC0r_GET(mst_misc0) == 0) {
        BCMI_MADURA_MST_MISC0r_SET(mst_misc0,0x404);
        PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_MST_MISC0r(pa,mst_misc0));
    }

    if  (BCMI_MADURA_MST_MISC1r_GET(mst_misc1) == 0) {
        BCMI_MADURA_MST_MISC1r_SET(mst_misc1,0x404);
        PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_MST_MISC1r(pa,mst_misc1));
    }

    if  (BCMI_MADURA_MST_MISC2r_GET(mst_misc2) == 0) {
        BCMI_MADURA_MST_MISC2r_SET(mst_misc2,0x404);
        PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_MST_MISC2r(pa,mst_misc2));
    }

    if  (BCMI_MADURA_MST_MISC3r_GET(mst_misc3) == 0) {
        BCMI_MADURA_MST_MISC3r_SET(mst_misc3,0x404);
        PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_MST_MISC3r(pa,mst_misc3));
    }

    MADURA_GET_INTF_SIDE(pa, intf_side);

    PHYMOD_MEMSET(&mst_misc0, 0, sizeof(BCMI_MADURA_MST_MISC0r_t));
    PHYMOD_MEMSET(&mst_misc1, 0, sizeof(BCMI_MADURA_MST_MISC1r_t));
    PHYMOD_MEMSET(&mst_misc2, 0, sizeof(BCMI_MADURA_MST_MISC2r_t));
    PHYMOD_MEMSET(&mst_misc3, 0, sizeof(BCMI_MADURA_MST_MISC3r_t));
	
    /*configure media_type and DFE option. SR/ER/LR, CR,KR, CAUI, XFI*/
    if ( intf == phymodInterfaceER4 || intf == phymodInterfaceSR ||
         intf == phymodInterfaceLR || intf == phymodInterfaceER) {
        media_type = SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS; /*OPTICAL : SR/ER/LR */
        DFE_option = SERDES_DFE_OPTION_NO_DFE;
    } else if (intf == phymodInterfaceCR4 || intf == phymodInterfaceCR){
        media_type = SERDES_MEDIA_TYPE_COPPER_CABLE; /*Copper*/
        DFE_option = SERDES_DFE_OPTION_DFE;
    } else if (intf == phymodInterfaceKR4 || intf == phymodInterfaceKR ){
        media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Back plane */
        DFE_option = SERDES_DFE_OPTION_DFE;
    } else if (intf == phymodInterfaceCAUI4 || intf == phymodInterfaceVSR ||
               intf == phymodInterfaceCAUI) {
        media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Back plane */
        DFE_option = SERDES_DFE_OPTION_DFE;
    } else if (intf == phymodInterfaceXLAUI || intf == phymodInterfaceXFI){
        media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Back plane */
        DFE_option = SERDES_DFE_OPTION_NO_DFE;
    } else if ( ( intf == phymodInterfaceSR4 ) || ( intf == phymodInterfaceLR4)) {
        media_type = SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS; /*OPTICAL*/
        DFE_option = SERDES_DFE_OPTION_NO_DFE;
        if (config->data_rate == MADURA_SPD_100G) {
		DFE_option = SERDES_DFE_OPTION_DFE;
        }
    } else {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
               (_PHYMOD_MSG("Invalid interface type..")));
    }
   
    /*Configure Tx_timing_mode and Tx_training_en option.
     (TBD. can be added in the structure and passed from the user interface. OR what is the logic ?)
     Taking default as of now*/ 

    if (intf_side == MADURA_IF_LINE) {
        config_reg &= ~(0xff00);
        /*considering sys+line side on the same interface*/
        config_reg |= (( media_type)& 0x3) << 8 ; /*line*/
        /*considering sys+line side on the same DFE settings*/
        config_reg |= (( DFE_option ) & 0x3 ) << 10 ; /*lin*/
        /*Tx timing mode and training_en*/
        config_reg |= ((Tx_timing_mode ) & 3) << 13 ;
        config_reg |= ((Tx_training_en ) & 1) << 12;
    } else {
        config_reg &= ~(0x00ff);
        /*considering sys+line side on the same interface*/
        config_reg |= ( media_type)& 0x3 ; /*sys*/
        /*considering sys+line side on the same DFE settings*/
        config_reg |= (( DFE_option ) & 0x3 ) << 2 ; /*sys */
        /*Tx timing mode and training_en*/
        config_reg |= ((Tx_timing_mode ) & 3) << 5 ; /*mode sys*/
        config_reg |= ((Tx_training_en ) & 1) << 4; /*enable sys*/
    }



    /*same value set for 4 ports (5-8 ports will be added/supported in next package)*/
    BCMI_MADURA_MST_MISC0r_SET(mst_misc0,config_reg);
    BCMI_MADURA_MST_MISC1r_SET(mst_misc1,config_reg);
    BCMI_MADURA_MST_MISC2r_SET(mst_misc2,config_reg);
    BCMI_MADURA_MST_MISC3r_SET(mst_misc3,config_reg);

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_MST_MISC0r(pa,mst_misc0));
	if((intf==phymodInterfaceSR)||(intf==phymodInterfaceSR4)){
		BCMI_MADURA_GPREG6r_GPREG6_DATAf_SET(gpreg6,(BCMI_MADURA_GPREG6r_GET(gpreg6)& ~(0x1))|(0x1));
	}else{
		BCMI_MADURA_GPREG6r_GPREG6_DATAf_SET(gpreg6,(BCMI_MADURA_GPREG6r_GET(gpreg6)& ~(0x1)));
	}
    } else  if (lane_mask == 0x2) {
        PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_MST_MISC1r(pa,mst_misc1));
	if((intf==phymodInterfaceSR)||(intf==phymodInterfaceSR4)){
		BCMI_MADURA_GPREG6r_GPREG6_DATAf_SET(gpreg6,(BCMI_MADURA_GPREG6r_GET(gpreg6)& ~(0x2))|(0x2));
	}else{
		BCMI_MADURA_GPREG6r_GPREG6_DATAf_SET(gpreg6,(BCMI_MADURA_GPREG6r_GET(gpreg6)& ~(0x2)));
	}
    } else  if (lane_mask == 0xC || lane_mask == 0x4 ) {
        PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_MST_MISC2r(pa,mst_misc2));
	if((intf==phymodInterfaceSR)||(intf==phymodInterfaceSR4)){
		BCMI_MADURA_GPREG6r_GPREG6_DATAf_SET(gpreg6,(BCMI_MADURA_GPREG6r_GET(gpreg6)& ~(0x4))|(0x4));
	}else{
		BCMI_MADURA_GPREG6r_GPREG6_DATAf_SET(gpreg6,(BCMI_MADURA_GPREG6r_GET(gpreg6)& ~(0x4)));
	}
    } else {
        PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_MST_MISC3r(pa,mst_misc3));
	if((intf==phymodInterfaceSR)||(intf==phymodInterfaceSR4)){
		BCMI_MADURA_GPREG6r_GPREG6_DATAf_SET(gpreg6,(BCMI_MADURA_GPREG6r_GET(gpreg6)& ~(0x8))|(0x8));
	}else{
		BCMI_MADURA_GPREG6r_GPREG6_DATAf_SET(gpreg6,(BCMI_MADURA_GPREG6r_GET(gpreg6)& ~(0x8)));
	}
    }
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_GPREG6r(pa, gpreg6));/*saving SR interface status for 4 ports*/

    return PHYMOD_E_NONE;
}

int _madura_set_ieee_intf(const phymod_access_t *pa, uint16_t intf, const phymod_phy_inf_config_t *config, uint16_t ip)
{
    return PHYMOD_E_NONE;
}

int _madura_get_ieee_intf(const phymod_access_t *pa, uint16_t ip, const phymod_phy_inf_config_t *config, uint16_t *intf)
{
    return PHYMOD_E_NONE;
}

int _madura_interface_get(const phymod_access_t *pa, uint16_t ip, const phymod_phy_inf_config_t *config, uint16_t *intf)
{
    uint16_t DFE_option = SERDES_DFE_OPTION_NO_DFE; /*default to dfe disable*/
    uint16_t media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Default to Back Plane */
    uint16_t config_reg=0, is_SR=0;
    uint16_t intf_side = 0;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;
    BCMI_MADURA_GPREG6r_t gpreg6;

    PHYMOD_MEMSET(&mst_misc0, 0, sizeof(BCMI_MADURA_MST_MISC0r_t));
    PHYMOD_MEMSET(&mst_misc1, 0, sizeof(BCMI_MADURA_MST_MISC1r_t));
    PHYMOD_MEMSET(&mst_misc2, 0, sizeof(BCMI_MADURA_MST_MISC2r_t));
    PHYMOD_MEMSET(&mst_misc3, 0, sizeof(BCMI_MADURA_MST_MISC3r_t));
    PHYMOD_MEMSET(&gpreg6, 0, sizeof(BCMI_MADURA_GPREG6r_t));
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));

    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));

    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));

    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_GPREG6r(pa, &gpreg6));
    MADURA_GET_INTF_SIDE(pa, intf_side);

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        config_reg = BCMI_MADURA_MST_MISC0r_GET(mst_misc0);
	if(BCMI_MADURA_GPREG6r_GET(gpreg6)& 0x1){
    	  is_SR=1;
	}
    } else  if (lane_mask == 0x2) {
        config_reg = BCMI_MADURA_MST_MISC1r_GET(mst_misc1);
	if(BCMI_MADURA_GPREG6r_GET(gpreg6)& 0x2){
    	  is_SR=1;
	}
    } else  if (lane_mask == 0xC || lane_mask == 0x4 ) {
        config_reg = BCMI_MADURA_MST_MISC2r_GET(mst_misc2);
	if(BCMI_MADURA_GPREG6r_GET(gpreg6)& 0x4){
    	  is_SR=1;
	}
    } else {
        config_reg = BCMI_MADURA_MST_MISC3r_GET(mst_misc3);
	if(BCMI_MADURA_GPREG6r_GET(gpreg6)& 0x8){
    	  is_SR=1;
	}
    }

     if (intf_side == MADURA_IF_LINE) {
        config_reg &= (0xff00);
        /*considering sys+line side on the same interface*/
        media_type = (config_reg & 0x300)>> 8 ;
        /*considering sys+line side on the same DFE settings*/
        DFE_option = (config_reg & 0xc00)>> 10;
    } else {
        config_reg &= (0x00ff);
        media_type = (config_reg & 0x3);
        /*considering sys+line side on the same DFE settings*/
        DFE_option = (config_reg & 0xc)>> 2;
    }

     if (config->data_rate == MADURA_SPD_10G) {
	     if((media_type==SERDES_MEDIA_TYPE_BACK_PLANE)&&( DFE_option == SERDES_DFE_OPTION_DFE))
	     {
		     *intf = phymodInterfaceKR;/*CAUI*/
	     }else if(( media_type == SERDES_MEDIA_TYPE_BACK_PLANE)&&( DFE_option == SERDES_DFE_OPTION_NO_DFE))
	     {
		     *intf = phymodInterfaceXFI;
	     }else if(( media_type == SERDES_MEDIA_TYPE_COPPER_CABLE)&&( DFE_option == SERDES_DFE_OPTION_DFE))
	     {
		     *intf = phymodInterfaceCR;
	     }else if(( media_type == SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS))
	     {
		     *intf = (is_SR==1)?phymodInterfaceSR:phymodInterfaceLR;
	     }
    } else {
	    if((media_type==SERDES_MEDIA_TYPE_BACK_PLANE)&&( DFE_option == SERDES_DFE_OPTION_DFE))
	    {
		    *intf = phymodInterfaceKR4;/*CAUI4*/
	    }else if(( media_type == SERDES_MEDIA_TYPE_BACK_PLANE)&&( DFE_option == SERDES_DFE_OPTION_NO_DFE))
	    {
		    *intf = phymodInterfaceXLAUI;
	    }else if(( media_type == SERDES_MEDIA_TYPE_COPPER_CABLE)&&( DFE_option == SERDES_DFE_OPTION_DFE))
	    {
		    *intf = phymodInterfaceCR4;
	    }else if(( media_type == SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS))
	    {
		    *intf = (is_SR==1)?phymodInterfaceSR4:phymodInterfaceLR4;
	    }
    }
    return PHYMOD_E_NONE;
}

int _madura_phy_interface_config_set(const phymod_phy_access_t* pa, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    phymod_phy_inf_config_t cfg;
    phymod_phy_inf_config_t cpy_config;
    MADURA_DEVICE_AUX_MODE_T *aux_mode;
    uint8_t clear_flag = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;

    PHYMOD_MEMCPY(&cpy_config, config, sizeof(phymod_phy_inf_config_t));
    if (cpy_config.device_aux_modes == NULL) {
        cpy_config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
        clear_flag = 1;
        PHYMOD_MEMSET(cpy_config.device_aux_modes, 0, sizeof(MADURA_DEVICE_AUX_MODE_T));
    }
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) cpy_config.device_aux_modes;
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL,
        (madura_get_chipid(&pa->access, &chip_id, &rev)));

#ifdef MADURA_UNTESTED_CODE
    if ((!aux_mode->gearbox_100g_inverse_mode)  && (chip_id != MADURA_CHIP_ID_82864)) {
        aux_mode->passthru_sys_side_core = MADURA_2X_FALCON_CORE;
        if ((config->data_rate == MADURA_SPD_10G) || (config->data_rate == MADURA_SPD_40G)) {
            aux_mode->pass_thru = 1;
        }
    }
#endif

    PHYMOD_DEBUG_VERBOSE(("Configuring interface\n"));
    PHYMOD_MEMSET(&cfg, 0, sizeof(phymod_phy_inf_config_t));
    cfg.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");

    do { 
        int loc_err ; 

        loc_err = _madura_phy_interface_config_get(&pa->access, 0, &cfg);

        PHYMOD_FREE(cfg.device_aux_modes);

        if (loc_err != PHYMOD_E_NONE)  {
            if (clear_flag) {
                PHYMOD_FREE(cpy_config.device_aux_modes);
            }
            return loc_err ;
        }
    } while (0);

    /* Configure Ref Clock*/
    PHYMOD_DEBUG_VERBOSE(("Configuring REF clock %d\n",config->ref_clock));
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL, (
          _madura_configure_ref_clock(&pa->access, config->ref_clock)));

    /* Configure DUT MODE */
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL, (
        _madura_config_dut_mode_reg(pa, &cpy_config)));
    
    /* Enable UDMS for non-an MODE */
    PHYMOD_DEBUG_VERBOSE(("Configuring UDMS\n"));
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL, (
        _madura_udms_config(pa, &cpy_config, MADURA_AN_NONE)));

    /* Configure PLL Divider*/
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL, (
            _madura_config_pll_div(&pa->access, cpy_config)));

    /* Configure Interface.*/
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL, (
        _madura_interface_set(&pa->access, &cpy_config)));

    if (clear_flag) {
        PHYMOD_FREE(cpy_config.device_aux_modes);
    }

    return PHYMOD_E_NONE;
}

int _madura_phy_interface_config_get(const phymod_access_t *pa, uint32_t flags, phymod_phy_inf_config_t *config)
{
	/*get datarate,passthru, gearbox, get refclock,get intf side*/
    BCMI_MADURA_P0_MODE_CTLr_t mode_ctrl0;
    BCMI_MADURA_P1_MODE_CTLr_t mode_ctrl1;
    BCMI_MADURA_P2_MODE_CTLr_t mode_ctrl2;
    BCMI_MADURA_P3_MODE_CTLr_t mode_ctrl3;
    unsigned char              mindex0 = 0xFF;
    unsigned char              mindex1 = 0xFF;
    unsigned char              mindex2 = 0xFF;
    unsigned char              mindex3 = 0xFF;

    BCMI_MADURA_CLK_SCALER_CTLr_t clk_ctrl;
    uint16_t ip = 0, intf_side = 0;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;
    /*
    BCMI_MADURA_GPREGBr_t           gpreg11;
    */
    uint16_t clock_ratio = 0;
    uint16_t lane_mask = pa->lane_mask;

    PHYMOD_MEMSET(&mode_ctrl0, 0, sizeof(BCMI_MADURA_P0_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl2, 0, sizeof(BCMI_MADURA_P2_MODE_CTLr_t));

    PHYMOD_MEMSET(&clk_ctrl, 0, sizeof(BCMI_MADURA_CLK_SCALER_CTLr_t));
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;
    if (aux_mode == NULL) {
        PHYMOD_DEBUG_VERBOSE(("AUX MODE MEM NOT ALLOC\n"));
        return PHYMOD_E_PARAM;
    }
    PHYMOD_MEMSET(aux_mode, 0, sizeof(MADURA_DEVICE_AUX_MODE_T));

    if (!lane_mask) {
        PHYMOD_DEBUG_VERBOSE(("Invalid Lanemask\n"));
        return PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P0_MODE_CTLr(pa , &mode_ctrl0));
    mindex0 = BCMI_MADURA_P0_MODE_CTLr_P0_MINDEXf_GET(mode_ctrl0);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P1_MODE_CTLr(pa , &mode_ctrl1));
    mindex1 = BCMI_MADURA_P1_MODE_CTLr_P1_MINDEXf_GET(mode_ctrl1);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P2_MODE_CTLr(pa , &mode_ctrl2));
    mindex2 = BCMI_MADURA_P2_MODE_CTLr_P2_MINDEXf_GET(mode_ctrl2);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P3_MODE_CTLr(pa , &mode_ctrl3));
    mindex3 = BCMI_MADURA_P3_MODE_CTLr_P3_MINDEXf_GET(mode_ctrl3);

    PHYMOD_DEBUG_VERBOSE(("MODE STS:%x\n", mindex0));

    MADURA_GET_INTF_SIDE(pa, intf_side);

    if (intf_side == MADURA_IF_LINE) {
        ip = MADURA_2X_FALCON_CORE;
    } else {
        ip = MADURA_FALCON_CORE;
    }
    if (ip == MADURA_FALCON_CORE) {
        if (MADURA_MIDX_IS_4X25G_PLUS(mindex0)) {
            config->data_rate = MADURA_SPD_100G;
            aux_mode->gearbox_100g_inverse_mode =  0 ;
	    aux_mode->pass_thru = 1;
        } else if (lane_mask == 0x2) {
            switch(mindex1) {
               case MADURA_MIDX_10GPT_0:
               case MADURA_MIDX_10GPT_0A:
               case MADURA_MIDX_10HGPT_0:
               case MADURA_MIDX_10HGPT_0A:
               case MADURA_MIDX_10GPT_1:
               case MADURA_MIDX_10GPT_1A:
               case MADURA_MIDX_10HGPT_1:
               case MADURA_MIDX_10HGPT_1A:
                    config->data_rate = MADURA_SPD_10G;
                    aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask & 0x3) {
            switch(mindex0) {
                case MADURA_MIDX_40GPT10_0:
                case MADURA_MIDX_40GPT10_0A:
                case MADURA_MIDX_40GPT10_1:
                case MADURA_MIDX_40GPT10_1A:
                case MADURA_MIDX_40HGPT10_0:
                case MADURA_MIDX_40HGPT10_0A:
                case MADURA_MIDX_40HGPT10_1:
                case MADURA_MIDX_40HGPT10_1A:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 1;
                    break;

                case MADURA_MIDX_40GDM_0:
                case MADURA_MIDX_40GDM_1:
                case MADURA_MIDX_40HGDM_0:
                case MADURA_MIDX_40HGDM_1:
                case MADURA_MIDX_40GDM_0B:
                case MADURA_MIDX_40GDM_1B:
                case MADURA_MIDX_40HGDM_0B:
                case MADURA_MIDX_40HGDM_1B:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 0;
                    break;

               case MADURA_MIDX_40GPT20_0:
               case MADURA_MIDX_40GPT20_0A:
               case MADURA_MIDX_40GPT20_1:
               case MADURA_MIDX_40GPT20_1A:
               case MADURA_MIDX_40GPT20_2:
               case MADURA_MIDX_40GPT20_2A:
               case MADURA_MIDX_40GPT20_3:
               case MADURA_MIDX_40GPT20_3A:
               case MADURA_MIDX_40HGPT20_0:
               case MADURA_MIDX_40HGPT20_0A:
               case MADURA_MIDX_40GHPT20_1:
               case MADURA_MIDX_40HGPT20_1A:
               case MADURA_MIDX_40HGPT20_2:
               case MADURA_MIDX_40HGPT20_2A:
               case MADURA_MIDX_40HGPT20_3:
               case MADURA_MIDX_40HGPT20_3A:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru_dual_lane = 1;
                    break;

               case MADURA_MIDX_10GPT_0:
               case MADURA_MIDX_10GPT_0A:
               case MADURA_MIDX_10HGPT_0:
               case MADURA_MIDX_10HGPT_0A:
                    config->data_rate = MADURA_SPD_10G;
                    aux_mode->pass_thru = 1;
                    break;
            }


        } else if (lane_mask == 0x8) {
            switch(mindex3) {
               case MADURA_MIDX_10GPT_2:
               case MADURA_MIDX_10GPT_2A:
               case MADURA_MIDX_10HGPT_2:
               case MADURA_MIDX_10HGPT_2A:
               case MADURA_MIDX_10GPT_3:
               case MADURA_MIDX_10GPT_3A:
               case MADURA_MIDX_10HGPT_3:
               case MADURA_MIDX_10HGPT_3A:
                    config->data_rate = MADURA_SPD_10G;
                    aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask & 0xc) {
            switch(mindex2) {
                case MADURA_MIDX_40GPT10_0:
                case MADURA_MIDX_40GPT10_0A:
                case MADURA_MIDX_40GPT10_1:
                case MADURA_MIDX_40GPT10_1A:
                case MADURA_MIDX_40HGPT10_0:
                case MADURA_MIDX_40HGPT10_0A:
                case MADURA_MIDX_40HGPT10_1:
                case MADURA_MIDX_40HGPT10_1A:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 1;
                    break;

                case MADURA_MIDX_40GDM_0:
                case MADURA_MIDX_40GDM_1:
                case MADURA_MIDX_40HGDM_0:
                case MADURA_MIDX_40HGDM_1:
                case MADURA_MIDX_40GDM_0B:
                case MADURA_MIDX_40GDM_1B:
                case MADURA_MIDX_40HGDM_0B:
                case MADURA_MIDX_40HGDM_1B:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 0;
                    break;

               case MADURA_MIDX_40GPT20_0:
               case MADURA_MIDX_40GPT20_0A:
               case MADURA_MIDX_40GPT20_1:
               case MADURA_MIDX_40GPT20_1A:
               case MADURA_MIDX_40GPT20_2:
               case MADURA_MIDX_40GPT20_2A:
               case MADURA_MIDX_40GPT20_3:
               case MADURA_MIDX_40GPT20_3A:
               case MADURA_MIDX_40HGPT20_0:
               case MADURA_MIDX_40HGPT20_0A:
               case MADURA_MIDX_40GHPT20_1:
               case MADURA_MIDX_40HGPT20_1A:
               case MADURA_MIDX_40HGPT20_2:
               case MADURA_MIDX_40HGPT20_2A:
               case MADURA_MIDX_40HGPT20_3:
               case MADURA_MIDX_40HGPT20_3A:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru_dual_lane = 1;
                    break;

               case MADURA_MIDX_10GPT_2:
               case MADURA_MIDX_10GPT_2A:
               case MADURA_MIDX_10HGPT_2:
               case MADURA_MIDX_10HGPT_2A:
                    config->data_rate = MADURA_SPD_10G;
                    aux_mode->pass_thru = 1;
                    break;
            }

        }
        if (aux_mode->pass_thru == 1 || aux_mode->pass_thru_dual_lane == 1) {
            aux_mode->passthru_sys_side_core = MADURA_FALCON_CORE ; 
        }
    } else { /* ip = MADURA_2X_FALCON_CORE */
        if (MADURA_MIDX_IS_4X25G_PLUS(mindex0)) {
            config->data_rate = MADURA_SPD_100G;
            aux_mode->gearbox_100g_inverse_mode = 0 ;
	    aux_mode->pass_thru = 1;
        } else if (lane_mask == 0x2) {
            switch(mindex1) {
               case MADURA_MIDX_10GPT_0:
               case MADURA_MIDX_10GPT_0A:
               case MADURA_MIDX_10HGPT_0:
               case MADURA_MIDX_10HGPT_0A:
               case MADURA_MIDX_10GPT_1:
               case MADURA_MIDX_10GPT_1A:
               case MADURA_MIDX_10HGPT_1:
               case MADURA_MIDX_10HGPT_1A:
                    config->data_rate = MADURA_SPD_10G;
                    aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask & 0x3) {
            switch(mindex0) {
                case MADURA_MIDX_40GPT10_0:
                case MADURA_MIDX_40GPT10_0A:
                case MADURA_MIDX_40GPT10_1:
                case MADURA_MIDX_40GPT10_1A:
                case MADURA_MIDX_40HGPT10_0:
                case MADURA_MIDX_40HGPT10_0A:
                case MADURA_MIDX_40HGPT10_1:
                case MADURA_MIDX_40HGPT10_1A:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 1;
                    break;

                case MADURA_MIDX_40GDM_0:
                case MADURA_MIDX_40GDM_1:
                case MADURA_MIDX_40HGDM_0:
                case MADURA_MIDX_40HGDM_1:
                case MADURA_MIDX_40GDM_0B:
                case MADURA_MIDX_40GDM_1B:
                case MADURA_MIDX_40HGDM_0B:
                case MADURA_MIDX_40HGDM_1B:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 0;
                    break;

               case MADURA_MIDX_40GPT20_0:
               case MADURA_MIDX_40GPT20_0A:
               case MADURA_MIDX_40GPT20_1:
               case MADURA_MIDX_40GPT20_1A:
               case MADURA_MIDX_40GPT20_2:
               case MADURA_MIDX_40GPT20_2A:
               case MADURA_MIDX_40GPT20_3:
               case MADURA_MIDX_40GPT20_3A:
               case MADURA_MIDX_40HGPT20_0:
               case MADURA_MIDX_40HGPT20_0A:
               case MADURA_MIDX_40GHPT20_1:
               case MADURA_MIDX_40HGPT20_1A:
               case MADURA_MIDX_40HGPT20_2:
               case MADURA_MIDX_40HGPT20_2A:
               case MADURA_MIDX_40HGPT20_3:
               case MADURA_MIDX_40HGPT20_3A:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru_dual_lane = 1;
                    break;

               case MADURA_MIDX_10GPT_0:
               case MADURA_MIDX_10GPT_0A:
               case MADURA_MIDX_10HGPT_0:
               case MADURA_MIDX_10HGPT_0A:
                    config->data_rate = MADURA_SPD_10G;
                    aux_mode->pass_thru = 1;
                    break;
            }

        } else if (lane_mask == 0x8) {
            switch(mindex3) {
               case MADURA_MIDX_10GPT_2:
               case MADURA_MIDX_10GPT_2A:
               case MADURA_MIDX_10HGPT_2:
               case MADURA_MIDX_10HGPT_2A:
               case MADURA_MIDX_10GPT_3:
               case MADURA_MIDX_10GPT_3A:
               case MADURA_MIDX_10HGPT_3:
               case MADURA_MIDX_10HGPT_3A:
                    config->data_rate = MADURA_SPD_10G;
                    aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask & 0xc) {
            switch(mindex2) {
                case MADURA_MIDX_40GPT10_0:
                case MADURA_MIDX_40GPT10_0A:
                case MADURA_MIDX_40GPT10_1:
                case MADURA_MIDX_40GPT10_1A:
                case MADURA_MIDX_40HGPT10_0:
                case MADURA_MIDX_40HGPT10_0A:
                case MADURA_MIDX_40HGPT10_1:
                case MADURA_MIDX_40HGPT10_1A:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 1;
                    break;

                case MADURA_MIDX_40GDM_0:
                case MADURA_MIDX_40GDM_1:
                case MADURA_MIDX_40HGDM_0:
                case MADURA_MIDX_40HGDM_1:
                case MADURA_MIDX_40GDM_0B:
                case MADURA_MIDX_40GDM_1B:
                case MADURA_MIDX_40HGDM_0B:
                case MADURA_MIDX_40HGDM_1B:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 0;
                    break;

               case MADURA_MIDX_40GPT20_0:
               case MADURA_MIDX_40GPT20_0A:
               case MADURA_MIDX_40GPT20_1:
               case MADURA_MIDX_40GPT20_1A:
               case MADURA_MIDX_40GPT20_2:
               case MADURA_MIDX_40GPT20_2A:
               case MADURA_MIDX_40GPT20_3:
               case MADURA_MIDX_40GPT20_3A:
               case MADURA_MIDX_40HGPT20_0:
               case MADURA_MIDX_40HGPT20_0A:
               case MADURA_MIDX_40GHPT20_1:
               case MADURA_MIDX_40HGPT20_1A:
               case MADURA_MIDX_40HGPT20_2:
               case MADURA_MIDX_40HGPT20_2A:
               case MADURA_MIDX_40HGPT20_3:
               case MADURA_MIDX_40HGPT20_3A:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru_dual_lane = 1;
                    break;

               case MADURA_MIDX_10GPT_2:
               case MADURA_MIDX_10GPT_2A:
               case MADURA_MIDX_10HGPT_2:
               case MADURA_MIDX_10HGPT_2A:
                    config->data_rate = MADURA_SPD_10G;
                    aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask & 0xF0) {
            switch(mindex2) {
                case MADURA_MIDX_40GPT10_0:
                case MADURA_MIDX_40GPT10_0A:
                case MADURA_MIDX_40GPT10_1:
                case MADURA_MIDX_40GPT10_1A:
                case MADURA_MIDX_40HGPT10_0:
                case MADURA_MIDX_40HGPT10_0A:
                case MADURA_MIDX_40HGPT10_1:
                case MADURA_MIDX_40HGPT10_1A:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 1;
                    break;

                case MADURA_MIDX_40GDM_0:
                case MADURA_MIDX_40GDM_1:
                case MADURA_MIDX_40HGDM_0:
                case MADURA_MIDX_40HGDM_1:
                case MADURA_MIDX_40GDM_0B:
                case MADURA_MIDX_40GDM_1B:
                case MADURA_MIDX_40HGDM_0B:
                case MADURA_MIDX_40HGDM_1B:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 0;
                    break;

               case MADURA_MIDX_40GPT20_0:
               case MADURA_MIDX_40GPT20_0A:
               case MADURA_MIDX_40GPT20_1:
               case MADURA_MIDX_40GPT20_1A:
               case MADURA_MIDX_40GPT20_2:
               case MADURA_MIDX_40GPT20_2A:
               case MADURA_MIDX_40GPT20_3:
               case MADURA_MIDX_40GPT20_3A:
               case MADURA_MIDX_40HGPT20_0:
               case MADURA_MIDX_40HGPT20_0A:
               case MADURA_MIDX_40GHPT20_1:
               case MADURA_MIDX_40HGPT20_1A:
               case MADURA_MIDX_40HGPT20_2:
               case MADURA_MIDX_40HGPT20_2A:
               case MADURA_MIDX_40HGPT20_3:
               case MADURA_MIDX_40HGPT20_3A:
                    config->data_rate = MADURA_SPD_20G;
                    aux_mode->pass_thru = 1;
                    break;

               case MADURA_MIDX_10GPT_2:
               case MADURA_MIDX_10GPT_2A:
               case MADURA_MIDX_10HGPT_2:
               case MADURA_MIDX_10HGPT_2A:
                    config->data_rate = MADURA_SPD_10G;
                    aux_mode->pass_thru = 1;
                    break;
            }

        }
        if (aux_mode->pass_thru == 1 || aux_mode->pass_thru_dual_lane == 1) {
            aux_mode->passthru_sys_side_core = MADURA_FALCON_CORE ;
        }
    }
    config->interface_modes = 0; /*Ethernet*/

    /*CLOCK_SCALEr*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_CLK_SCALER_CTLr(pa,&clk_ctrl));
    clock_ratio = BCMI_MADURA_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_GET(clk_ctrl);
    if (clock_ratio == 0x640) {
        config->ref_clock = phymodRefClk156Mhz; 
    } else if (clock_ratio == 0x672) {
        config->ref_clock = phymodRefClk161Mhz; 
    } else if (clock_ratio == 0x800) {
        config->ref_clock = 0 ;/* set to 0 in reset value 0x800. Is it fine?*/ 
    }
    PHYMOD_IF_ERR_RETURN(
       _madura_interface_get(pa, ip, config, &intf_side));
    config->interface_type = intf_side;
    
    return PHYMOD_E_NONE;
}

int _madura_phy_pcs_link_get(const phymod_access_t *pa, uint32_t *pcs_link)
{
    if (pcs_link) {
        *pcs_link = 0xffff;
    } else {
        return PHYMOD_E_PARAM;
    }
    PHYMOD_IF_ERR_RETURN(_madura_get_pcs_link_status(pa, pcs_link));

    return PHYMOD_E_NONE;
}

int _madura_configure_ref_clock(const phymod_access_t *pa, phymod_ref_clk_t ref_clk)
{
    BCMI_MADURA_CLK_SCALER_CTLr_t clk_ctrl;
    PHYMOD_MEMSET(&clk_ctrl, 0, sizeof(BCMI_MADURA_CLK_SCALER_CTLr_t));
   
    /* uint16_t clk_scalar_code = 0x0; */
    switch (ref_clk) {
        case phymodRefClk156Mhz:
        case phymodRefClk312Mhz:
    /*  case phymodRefClk625Mhz: */
        	BCMI_MADURA_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl,0x640);
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_WRITE_CLK_SCALER_CTLr(pa,clk_ctrl));
        break;
        case phymodRefClk161Mhz:
        case phymodRefClk322Mhz:
        case phymodRefClk644Mhz:
        	BCMI_MADURA_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl,0x672);
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_WRITE_CLK_SCALER_CTLr(pa,clk_ctrl));
        break;
        default:
	    return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int _madura_get_pll_modes(phymod_ref_clk_t ref_clk,const phymod_phy_inf_config_t *config, uint16_t *fal_pll_mode, uint16_t *fal_2x_pll_mode) 
{
    uint32_t speed = config->data_rate;
    switch (ref_clk) {
        case phymodRefClk156Mhz:
        case phymodRefClk312Mhz:
            if (speed == MADURA_SPD_100G) {
                if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
	  	    *fal_pll_mode = MADURA_PLL_MODE_175;
	        } else {
		    *fal_pll_mode = MADURA_PLL_MODE_165;
	        }
            } else {
            /*-- Falcon Line Rate is 10G/20G*/
	        if (!PHYMOD_INTF_MODES_HIGIG_GET(config)) { /*IEEE MODE*/
		    *fal_pll_mode = MADURA_PLL_MODE_132;
	        } else if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
		    *fal_pll_mode = MADURA_PLL_MODE_140;
	        } else {
		    return PHYMOD_E_PARAM;
            }
        }
        break;
        case phymodRefClk161Mhz:
        case phymodRefClk322Mhz:
        case phymodRefClk644Mhz:
            if (speed == MADURA_SPD_100G) {
                if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                    return PHYMOD_E_PARAM;
                } else {
                    *fal_pll_mode = MADURA_PLL_MODE_160;
                }
            } else {
                if (!PHYMOD_INTF_MODES_HIGIG_GET(config)) {/*IEEE MODE*/
                    *fal_pll_mode = MADURA_PLL_MODE_128;
                } else {
                    return PHYMOD_E_PARAM;
                }
	    }
        break;
        case phymodRefClk174Mhz:
        case phymodRefClk349Mhz: 
        case phymodRefClk698Mhz:
            if (speed == MADURA_SPD_100G) {
                if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                    *fal_pll_mode = MADURA_PLL_MODE_160;
                } else {
                    return PHYMOD_E_PARAM;
                }
            } else {
                if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                    *fal_pll_mode = MADURA_PLL_MODE_128;
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
 * @param PLL Mode
 * @return Merlin PLL mode mapped value
 */
int _madura_map_fal_2x_pll_div(MADURA_PLL_MODE_E pll_mode) 
{
    switch (pll_mode) {
        case MADURA_PLL_MODE_64:
            return 1;
        case MADURA_PLL_MODE_66:
            return 2;
        case MADURA_PLL_MODE_70:
            return 3;
        default:
	        PHYMOD_DEBUG_VERBOSE(("PLLMODE %d not supported\n", pll_mode));
            return PHYMOD_E_PARAM; 
    }
}

int _madura_config_pll_div(const phymod_access_t *pa, const phymod_phy_inf_config_t config)
{
    uint16_t ip = 0, fal_2x_pll_mode = 0, falcon_pll_mode = 0;
    phymod_ref_clk_t ref_clk = config.ref_clock;
    uint16_t data = 0, retry_cnt = 5;
    BCMI_MADURA_SLICEr_t slice_reg;
    BCMI_MADURA_PMD_PLL_STSr_t pll_sts;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&pll_sts, 0 , sizeof(BCMI_MADURA_PMD_PLL_STSr_t));
    /*Program PLL div of all dies*/
    /* Set pll_mode */
    PHYMOD_IF_ERR_RETURN(
       _madura_get_pll_modes(ref_clk, &config, &falcon_pll_mode, &fal_2x_pll_mode));
    for (ip = 0 ; ip <=1 ;ip++) {
        PHYMOD_IF_ERR_RETURN (
          _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, 0));
		
	        wrc_falcon2_madura_ams_pll_pwrdn(0); /* 0xd11b . enable pll . Added macro in .h file */
	
		/* write 0 to core_dp_s_rstb while pll config .  Added macro in .h file*/
		wrc_falcon2_madura_core_dp_s_rstb(0); /*  0xd184 .wrc_falcon2_madura_core_dp_s_rstb(0); */

                /* Set PLL div */
                switch (falcon_pll_mode) {
                    case MADURA_PLL_MODE_80:
                        PHYMOD_IF_ERR_RETURN (
                         falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_80x)); 
                    	break;
		    case MADURA_PLL_MODE_160:
                        if (PHYMOD_INTF_MODES_OTN_GET(&config)) {
                            PHYMOD_IF_ERR_RETURN (
                             falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_160x_refc174));
                        } else {
                            PHYMOD_IF_ERR_RETURN (
                             falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_160x)); 
                        }
                    break;
                    case MADURA_PLL_MODE_132: 
                        PHYMOD_IF_ERR_RETURN (
                         falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_132x)); 
                    break;
                    case MADURA_PLL_MODE_128: 
                        PHYMOD_IF_ERR_RETURN (
                         falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_128x)); 
                    break;
                    case MADURA_PLL_MODE_140: 
                        PHYMOD_IF_ERR_RETURN (
                         falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_140x));
                    break;
                    case MADURA_PLL_MODE_165: 
                        PHYMOD_IF_ERR_RETURN (
                         falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_165x));
                    break;
                    case MADURA_PLL_MODE_168: 
                        PHYMOD_IF_ERR_RETURN (
                         falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_168x));
                    break;
                    case MADURA_PLL_MODE_175: 
                        PHYMOD_IF_ERR_RETURN (
                         falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_175x));
                    break;
                    case MADURA_PLL_MODE_180: 
                        PHYMOD_IF_ERR_RETURN (
                         falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_180x));
                    break;
                    case MADURA_PLL_MODE_184: 
                        PHYMOD_IF_ERR_RETURN (
                         falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_184x));
                    break;
                    case MADURA_PLL_MODE_200: 
                        PHYMOD_IF_ERR_RETURN (
                         falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_200x));
                    break;
                    case MADURA_PLL_MODE_224: 
                        PHYMOD_IF_ERR_RETURN (
                         falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_224x));
                    break;
                    case MADURA_PLL_MODE_264: 
                        PHYMOD_IF_ERR_RETURN (
                         falcon2_madura_configure_pll(pa, FALCON2_MADURA_pll_div_264x));
                    break;
                    default:
		        return PHYMOD_E_PARAM;
                }
                /* write 1 to release core_dp_s_rstb after pll config*/
		wrc_falcon2_madura_core_dp_s_rstb(1); 
    }
    /*check pll lock */
  do {
	  BCMI_MADURA_READ_PMD_PLL_STSr(pa,&pll_sts );
	  data=BCMI_MADURA_PMD_PLL_STSr_PMD_PLL0_LOCKf_GET(pll_sts);
	  PHYMOD_USLEEP(100);
    } while((data != 0) && (retry_cnt--));
    if (retry_cnt == 0) {
   	PHYMOD_DEBUG_ERROR(("PLL LOCK failed for pll0\n"));       
        return PHYMOD_E_FAIL;
    }

    /*Reset DEV1 slice register*/	     	
    BCMI_MADURA_WRITE_SLICEr(pa,slice_reg);
    return PHYMOD_E_NONE;
}

int _madura_core_dp_rstb(const phymod_access_t *pa, const phymod_phy_inf_config_t config)
{
    return PHYMOD_E_NONE;
}

int _madura_core_reset_set(const phymod_access_t *pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
#if 1
	uint16_t cores=3,ip=0;
	uint16_t lane=0;
	BCMI_MADURA_GEN_CTL1r_t gen_ctrl;
	BCMI_MADURA_COMMON_CTLr_t fal_cmn_ctrl;
	BCMI_MADURA_RXTX_LN_S_RSTB_CTLr_t ln_s_rstb;

	PHYMOD_MEMSET(&gen_ctrl, 0, sizeof(BCMI_MADURA_GEN_CTL1r_t));
	PHYMOD_MEMSET(&fal_cmn_ctrl, 0, sizeof(BCMI_MADURA_COMMON_CTLr_t));
	PHYMOD_MEMSET(&ln_s_rstb, 0, sizeof(BCMI_MADURA_RXTX_LN_S_RSTB_CTLr_t));
	
	PHYMOD_IF_ERR_RETURN(
			BCMI_MADURA_READ_GEN_CTL1r(pa,&gen_ctrl));	    
	if (reset_mode == phymodResetModeHard) {
	    /*Reset for chip*/
	    BCMI_MADURA_GEN_CTL1r_RESETBf_SET(gen_ctrl,0);    
	    /*Reset registers*/
	    BCMI_MADURA_GEN_CTL1r_REG_RSTBf_SET(gen_ctrl,0);
	    PHYMOD_IF_ERR_RETURN(
			    BCMI_MADURA_WRITE_GEN_CTL1r(pa,gen_ctrl));
	    /*Above chip reset resets the cores also !*/
        PHYMOD_USLEEP(10000);
    } else {
	    /*Reset registers*/
	    BCMI_MADURA_GEN_CTL1r_REG_RSTBf_SET(gen_ctrl,0);
	    PHYMOD_IF_ERR_RETURN(
			BCMI_MADURA_WRITE_GEN_CTL1r(pa,gen_ctrl));	    
	    /* Reset per falcon of 4 falcons*/    
	    while(cores>0)
	    {
		PHYMOD_IF_ERR_RETURN(
				_madura_set_slice_reg (pa,
					ip, MADURA_DEV_PMA_PMD, lane));
		/*Lane soft reset*/ 
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_READ_RXTX_LN_S_RSTB_CTLr(pa,&ln_s_rstb));
		BCMI_MADURA_RXTX_LN_S_RSTB_CTLr_LN_S_RSTBf_SET(ln_s_rstb,0);
		BCMI_MADURA_WRITE_RXTX_LN_S_RSTB_CTLr(pa,ln_s_rstb);
		PHYMOD_USLEEP(10000);
		BCMI_MADURA_RXTX_LN_S_RSTB_CTLr_LN_S_RSTBf_SET(ln_s_rstb,1);
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_WRITE_RXTX_LN_S_RSTB_CTLr(pa,ln_s_rstb));

		ip=(cores==2)?1:0;/*toggle ip for 3rd iteration*/
		lane=(ip==1)?0:4;/*toggle ip for 3rd iteration*/
   		cores--;
	    }

    }
#endif
    return PHYMOD_E_NONE;
}

int _madura_pcs_link_monitor_enable_set(const phymod_access_t *pa, uint16_t en_dis)
{
    return PHYMOD_E_NONE;
}

int _madura_pcs_link_monitor_enable_get(const phymod_access_t *pa, uint32_t *get_pcs)
{
    uint16_t lane = 0;
    uint16_t ip = 0/*, intf_side=-1*/;
    phymod_phy_inf_config_t config;
    uint16_t max_lane = 0, lane_mask=0;
    /*uint16_t reg_addr = 0; */
    BCMI_MADURA_PATT_MON_GEN_CTLr_t mon_ctrl;
    
    *get_pcs = 0x0;
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    PHYMOD_MEMSET(&mon_ctrl, 0, sizeof(BCMI_MADURA_PATT_MON_GEN_CTLr_t));

    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);
    /*MADURA_GET_INTF_SIDE(pa,intf_side); */
    
    lane_mask = (pa->lane_mask) ? pa->lane_mask : 0xff;
    max_lane = (ip == MADURA_FALCON_CORE) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;
    
    for (lane = 0; lane < max_lane; lane ++) {
        if ((lane_mask & (1 << lane))) {
        /*config slice reg */
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa,  
                        ip, MADURA_DEV_PMA_PMD, lane)));
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
			BCMI_MADURA_READ_PATT_MON_GEN_CTLr(pa,&mon_ctrl)));
                *get_pcs &= BCMI_MADURA_PATT_MON_GEN_CTLr_RX_MON_CTRLf_GET(mon_ctrl);
                PHYMOD_DEBUG_VERBOSE(("Falcon Get Link monitor for Lane:%d Data:0x%x\n",
                            lane, *get_pcs));
        }else{
		PHYMOD_DEBUG_VERBOSE(("lane %x not matching with lanemask %x",lane,lane_mask));	
        }
    }
    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}

int _madura_get_pcs_link_status(const phymod_access_t *pa, uint32_t *link_sts)
{
    uint16_t lane = 0;
    uint16_t ip = 0;
    phymod_phy_inf_config_t config;
    uint16_t max_lane = 0, lane_mask=0 /*, count_msb = 0 */;
    /* uint16_t reg_addr = 0; */
    uint32_t err_cnt = 0, get_pcs = 0;
    BCMI_MADURA_CL49_BER_STSr_t ber_status;
    BCMI_MADURA_CL82_BER_CNTr_t ber_cnt;
    BCMI_MADURA_SLICEr_t slice_reg;
    BCMI_MADURA_PCS_MON_LIVE_STSr_t pcs_mon_liv_sts;
    BCMI_MADURA_PCS_MON_STSr_t pcs_mon_sts;

    PHYMOD_MEMSET(&ber_status, 0, sizeof(BCMI_MADURA_CL49_BER_STSr_t));
    PHYMOD_MEMSET(&slice_reg, 0, sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&ber_cnt, 0, sizeof(BCMI_MADURA_CL82_BER_CNTr_t));
    PHYMOD_MEMSET(&pcs_mon_liv_sts, 0, sizeof(BCMI_MADURA_PCS_MON_LIVE_STSr_t));
    PHYMOD_MEMSET(&pcs_mon_sts, 0, sizeof(BCMI_MADURA_PCS_MON_STSr_t));

    *link_sts = 0xffff;

    PHYMOD_IF_ERR_RETURN(
         _madura_pcs_link_monitor_enable_get(pa, &get_pcs));

    if (get_pcs) {
        PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
        config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
           _madura_phy_interface_config_get(pa, 0, &config)));
    
        MADURA_GET_IP(pa, config, ip);
    
        lane_mask = (pa->lane_mask) ? pa->lane_mask : 0xf;
        max_lane = (ip == MADURA_FALCON_CORE) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;
    
        PHYMOD_DEBUG_VERBOSE((" Max_lane:%d lanemask:0x%x\n", 
                    max_lane, lane_mask));

        PHYMOD_DEBUG_VERBOSE(("Getting PCS Status\n"));
        for (lane = 0; lane < max_lane; lane ++) {
            if ((lane_mask & (1 << lane))) {
		    /* reg_addr = DP_FMON0_LIVE_STATUS_ADR + lane; */
		    /*Program Slice register dev1*/	
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
				    _madura_set_slice_reg (pa, 
					    ip, MADURA_DEV_PMA_PMD, lane)));
		    /*Check PCS live status*/ 
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
				    BCMI_MADURA_READ_PCS_MON_LIVE_STSr(pa,&pcs_mon_liv_sts)));
		    if(BCMI_MADURA_PCS_MON_LIVE_STSr_PCS_STATUSf_GET(pcs_mon_liv_sts)){
			    *link_sts &= 1;

			    /* Check PCS lock*/
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
					    BCMI_MADURA_READ_PCS_MON_STSr(pa,&pcs_mon_sts)));

			    if(BCMI_MADURA_PCS_MON_STSr_PCS_STATUS_LLf_GET(pcs_mon_sts)){
				    /* Loss of link : read error cntr*/
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
						    BCMI_MADURA_READ_CL49_BER_STSr(pa,&ber_status)));
				    err_cnt = BCMI_MADURA_CL49_BER_STSr_BER_COUNTf_GET(ber_status);
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
						    BCMI_MADURA_READ_CL82_BER_CNTr(pa,&ber_cnt)));

				    err_cnt=BCMI_MADURA_CL82_BER_CNTr_BER_COUNT_MSBf_GET(ber_cnt);
				    *link_sts &= 0;
				    PHYMOD_DEBUG_VERBOSE(("Lane:%d Loss of lock: 1 Error Cnt:0x%x\n", 
							    lane, err_cnt));
			    }
		    } else {
			    *link_sts &= 0;
		    }
	    }
	}
	PHYMOD_FREE(config.device_aux_modes);
    } else {
        PHYMOD_DEBUG_VERBOSE(("PCS MON not Enabled, Reading PMD Status\n"));
        PHYMOD_IF_ERR_RETURN(
                  _madura_rx_pmd_lock_get(pa, link_sts));
    }
    /*Reset DEV1 slice register*/	     	
     BCMI_MADURA_WRITE_SLICEr(pa,slice_reg);
    return PHYMOD_E_NONE;
}
int _madura_fal_2x_falcon_lane_map_get(
        const phymod_access_t *pa,
        const phymod_phy_inf_config_t *cfg,
        uint32_t* fal_2x_lane_map,
        uint32_t* fal_lane_map)
{

    phymod_phy_inf_config_t config;
    uint16_t core = 0;
    uint32_t lane_mask = 0;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    lane_mask = (pa->lane_mask) ? pa->lane_mask : 0xf;
    PHYMOD_MEMCPY(&config, cfg, sizeof(phymod_phy_inf_config_t));
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*)cfg->device_aux_modes;
    MADURA_GET_IP(pa, config, core);
    PHYMOD_DEBUG_VERBOSE(("%s :: core:%d\n", __func__, core));

    if (config.data_rate == MADURA_SPD_100G) {
	    if(!aux_mode->alternate){
		    *fal_2x_lane_map = 0xF;
		    *fal_lane_map = 0xF;
	    } else { /*alternate*/
		    *fal_2x_lane_map = 0xF0;
		    *fal_lane_map = 0xF;
	    }
    } else if (core == MADURA_2X_FALCON_CORE) {
        if (config.data_rate == MADURA_SPD_40G) {
            if ((aux_mode->pass_thru_dual_lane) && (!aux_mode->alternate)) { 
                *fal_2x_lane_map = lane_mask;
                *fal_lane_map = lane_mask;
            } else  if ((aux_mode->pass_thru) && (!aux_mode->alternate)) { 
                *fal_2x_lane_map = 0xf;
                *fal_lane_map = 0xF;        
            } else if ((aux_mode->pass_thru) && (aux_mode->alternate)) { 
                *fal_2x_lane_map = 0xF0;
                *fal_lane_map = 0xF;
            } 
            else { 
                *fal_2x_lane_map = lane_mask;
                if (lane_mask == 0xF)
                    *fal_lane_map = 0x3;
                else if(lane_mask == 0xF0)
                    *fal_lane_map = 0xC;
                else
                    *fal_lane_map = 0xF;
            }
#ifdef MADURA_UNTESTED_CODE
        } else if (config.data_rate == MADURA_SPD_20G) {
            if (aux_mode->pass_thru) {
                *fal_2x_lane_map = lane_mask;
                if (lane_mask == 0x3)
                    *fal_lane_map = 0x3;
                else if(lane_mask == 0x30)
                    *fal_lane_map = 0xC;
                else
                    *fal_lane_map = 0xF;
            } else {
                *fal_2x_lane_map = lane_mask; 
                if (lane_mask == 0x3)
                    *fal_lane_map = 0x1;
                else if(lane_mask == 0x30)
                    *fal_lane_map = 0x4;
                else
                    *fal_lane_map = 0x5;
            }      
#endif
        } else { /* 10G */
            if (!aux_mode->alternate) {/*passthru*/
                *fal_2x_lane_map = lane_mask;
                *fal_lane_map = lane_mask;   
            } else {
                *fal_2x_lane_map = lane_mask;
                if ((lane_mask == 0x1) || (lane_mask == 0x2))
                    *fal_lane_map = lane_mask;
                else if((lane_mask == 0x10) || (lane_mask == 0x20))
                    *fal_lane_map = (lane_mask >> 2);
                else
                    *fal_lane_map = 0xF;
            }
        }
    }
    else {
         if (config.data_rate == MADURA_SPD_40G) {
            if ((aux_mode->pass_thru_dual_lane) && (!aux_mode->alternate)) { 
                *fal_lane_map = lane_mask;
                *fal_2x_lane_map = lane_mask;
            } else if ((aux_mode->pass_thru) && (!aux_mode->alternate)) {
                *fal_2x_lane_map = 0xf;
                *fal_lane_map = 0xF;
            } else if ((aux_mode->pass_thru) && (aux_mode->alternate)) {
                *fal_2x_lane_map = 0xF0;
                *fal_lane_map = 0xF;
            }
            else {
                *fal_lane_map = lane_mask;
                if (lane_mask == 0x3)
                    *fal_2x_lane_map = 0xF;
                else if(lane_mask == 0xC)
                    *fal_2x_lane_map = 0xF0;
                else
                    *fal_2x_lane_map = 0xFF;
            }
#ifdef MADURA_UNTESTED_CODE
        } else if (config.data_rate == MADURA_SPD_20G) {
            if (aux_mode->pass_thru) {
                *fal_lane_map = lane_mask;
                if (lane_mask == 0x3)
                    *fal_2x_lane_map = 0x3;
                else if(lane_mask == 0xC)
                    *fal_2x_lane_map = 0x30;
                else
                    *fal_2x_lane_map = 0x33;
            } else {
                *fal_lane_map = lane_mask;
                if (lane_mask == 0x1)
                    *fal_2x_lane_map = 0x3;
                else if(lane_mask == 0x4)
                    *fal_2x_lane_map = 0x30;
                else
                    *fal_2x_lane_map = 0x33;
            }
#endif
        } else { /* 10G */
            if (!aux_mode->alternate) {/*PT*/
                *fal_2x_lane_map = lane_mask;
                *fal_lane_map = lane_mask;
            } else {
                *fal_lane_map = lane_mask;
                if ((lane_mask == 0x1) || (lane_mask == 0x2))
                    *fal_2x_lane_map = lane_mask;
                else if((lane_mask == 0x4) || (lane_mask == 0x8))
                    *fal_2x_lane_map = (lane_mask << 2);
                else
                    *fal_2x_lane_map = 0xff;
            }
        }
    }
    return PHYMOD_E_NONE;
}

int _madura_falcon_lpbk_get(
        const phymod_access_t *pa,
        phymod_loopback_mode_t loopback,
        uint32_t* enable,
         uint16_t ip)
{

	BCMI_MADURA_TLB_RX_DIG_LPBK_CFGr_t dig_lpbk;
	BCMI_MADURA_TLB_TX_RMT_LPBK_CFGr_t rmt_lpbk;
	PHYMOD_MEMSET(&dig_lpbk, 0, sizeof(BCMI_MADURA_TLB_RX_DIG_LPBK_CFGr_t));
	PHYMOD_MEMSET(&rmt_lpbk, 0, sizeof(BCMI_MADURA_TLB_TX_RMT_LPBK_CFGr_t));
	switch (loopback) {
        case phymodLoopbackGlobal:
            if(ip == MADURA_FALCON_CORE)
	    {	
		    return PHYMOD_E_PARAM;
	    }
        /* coverity[fallthrough] */
        case phymodLoopbackGlobalPMD:
            /* Falcon Digital loopback get */
        PHYMOD_IF_ERR_RETURN(
	        BCMI_MADURA_READ_TLB_RX_DIG_LPBK_CFGr(pa,&dig_lpbk));
	    *enable=BCMI_MADURA_TLB_RX_DIG_LPBK_CFGr_DIG_LPBK_ENf_GET(dig_lpbk); 
	    break;
        case phymodLoopbackRemotePMD: /* Falcon Remote loopback get */
        PHYMOD_IF_ERR_RETURN(
	        BCMI_MADURA_READ_TLB_TX_RMT_LPBK_CFGr(pa,&rmt_lpbk));
	    *enable=BCMI_MADURA_TLB_TX_RMT_LPBK_CFGr_RMT_LPBK_ENf_GET(rmt_lpbk);
	    break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        default:
        break;
    }

    return PHYMOD_E_NONE;
}

int _madura_loopback_get(const phymod_access_t *pa, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;

    *enable = 1; 
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa, 
                        ip, MADURA_DEV_PMA_PMD, lane)));

            /* FALCON core loopback get */
            PHYMOD_DEBUG_VERBOSE(("Falcon loopback get\n"));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_falcon_lpbk_get(pa, loopback, enable,ip)));
            break;
        }
    }
    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}

int _madura_falcon_lpbk_set(const phymod_access_t* pa, const phymod_phy_inf_config_t *config,
                           phymod_loopback_mode_t loopback, uint32_t enable)
{
    /*This function sets loop of all types for both sides*/
    uint16_t lane = 0,max_lane=0;
    uint16_t ip = 0;
    uint32_t fal_2x_lane_mask = 0;
    uint32_t fal_lane_mask = 0;
    uint32_t main_lane_mask = 0,otherside_lane_mask = 0;
#ifdef DEB
    uint32_t data1;
#endif
    
    MADURA_GET_IP(pa, (*config), ip);
    max_lane = (ip == MADURA_FALCON_CORE) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;
    
    PHYMOD_IF_ERR_RETURN(
        _madura_fal_2x_falcon_lane_map_get(pa, config, &fal_2x_lane_mask, &fal_lane_mask));
    main_lane_mask = (ip == MADURA_FALCON_CORE) ? fal_lane_mask : fal_2x_lane_mask;
    otherside_lane_mask = (ip == MADURA_FALCON_CORE) ? fal_2x_lane_mask : fal_lane_mask;

    PHYMOD_DEBUG_VERBOSE(("%s at %d : main_lane_mask=%x otherside_lane_mask=%x\n",__FUNCTION__,__LINE__,main_lane_mask,otherside_lane_mask));
    PHYMOD_DEBUG_VERBOSE(("Falcon loopback set\n"));

    switch (loopback) {
        case phymodLoopbackGlobal:
	    if( ip == MADURA_FALCON_CORE)
	    {
                return PHYMOD_E_PARAM;
	    }
        /* coverity[fallthrough] */
        case phymodLoopbackRemotePMD: /* Falcon remote loopback set */
            for(lane = 0; lane < max_lane; lane ++) {
                if (main_lane_mask & (1 << lane)) {
                    PHYMOD_IF_ERR_RETURN(
                      _madura_set_slice_reg (pa,
                                ip, MADURA_DEV_PMA_PMD,
                                 lane));
#ifdef DEB
                        PHYMOD_BUS_READ(pa,0x1d172,&data1);
                        printf("[--d172-%x--]\n",data1);
#endif
                    PHYMOD_IF_ERR_RETURN(falcon2_madura_rmt_lpbk(pa, enable));
#ifdef DEB
                        PHYMOD_BUS_READ(pa,0x1d172,&data1);
                        printf("[--d172-%x--]\n",data1);
#endif
                }
            }
            MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodLoopbackGlobalPMD: /* Falcon digital loopback set */
        {

  	    /*Flipping ip,maxlane and lanemask*/
            for(lane = 0; lane < ((max_lane%8) +4)/*if 4 then 8 and vice versa*/; lane ++) {
                if (otherside_lane_mask & (1 << lane)) {
                    PHYMOD_IF_ERR_RETURN(
                      _madura_set_slice_reg (pa,
                                ((ip==0)?1:0), MADURA_DEV_PMA_PMD,  lane));/*send toggled value of IP*/

#ifdef DEB
                        PHYMOD_BUS_READ(pa,0x1d162,&data1);
                        printf("[--d162-%x--]\n",data1);
#endif
                    PHYMOD_IF_ERR_RETURN(falcon2_madura_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
#ifdef DEB
                        PHYMOD_BUS_READ(pa,0x1d162,&data1);
                        printf("[+-d162-%x--]\n",data1);
#endif
                }
            }
            MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);

            for(lane = 0; lane < max_lane; lane ++) {
                if (main_lane_mask & (1 << lane)) {
                    PHYMOD_IF_ERR_RETURN(
                      _madura_set_slice_reg (pa, 
                                ip, MADURA_DEV_PMA_PMD,
                                 lane));
#ifdef DEB
                        PHYMOD_BUS_READ(pa,0x1d162,&data1);
                        printf("[--d162-%x--]\n",data1);
#endif
                    PHYMOD_IF_ERR_RETURN(falcon2_madura_dig_lpbk_rptr(pa, enable, DIG_LPBK_SIDE));
                    /* Disable the prbs_chk_en_auto_mode while seting digital loopback. Below is also not in validation.Keeping for now  */
                    PHYMOD_IF_ERR_RETURN(wr_falcon2_madura_prbs_chk_en_auto_mode((!enable)));
#ifdef DEB
                        PHYMOD_BUS_READ(pa,0x1d162,&data1);
                        printf("[--d162-%x--]\n",data1);
#endif
                }
            }

            MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
        }
        break;
        default :
        break;
    }

    return PHYMOD_E_NONE;
}

int _madura_if_ctrl_frc_tx_disable(const phymod_access_t *pa, const phymod_phy_inf_config_t *config,
                                  uint32_t enable)
{
#ifdef MADURA_UNTESTED_CODE
    uint16_t lane = 0;
    uint32_t fal_2x_lane_mask = 0;
    uint32_t fal_lane_mask = 0;
    FALCON2X_IF_CONTROL1_LANE0_TYPE_T fal_2x_ctrl_lane;
    FALCON_IF_CONTROL1_LANE0_TYPE_T fal_ctrl_lane;

    PHYMOD_IF_ERR_RETURN(
        _madura_fal_2x_falcon_lane_map_get(pa, config, &fal_2x_lane_mask, &fal_lane_mask));

    /* Falcon Force clear any Tx disables set by HW */
    for(lane = 0; lane < MADURA_MAX_FALCON_LANE; lane ++) {
        if (fal_lane_mask & (1 << lane)) {
            READ_MADURA_PMA_PMD_REG(pa, (FALCON_IF_CONTROL1_LANE0_ADR + lane),
                                       fal_ctrl_lane.data);
            fal_ctrl_lane.fields.pmd_tx_disable_0_frc = (enable ? 1 : 0);
            fal_ctrl_lane.fields.pmd_tx_disable_0_frcval = 0;
            WRITE_MADURA_PMA_PMD_REG(pa, (FALCON_IF_CONTROL1_LANE0_ADR + lane),
                                       fal_ctrl_lane.data);
        }
    }

#endif
    return PHYMOD_E_NONE;
}

int _madura_loopback_set(const phymod_access_t *pa, phymod_loopback_mode_t loopback, uint32_t enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lp_bck_val_misc = 0;
    uint16_t data = 0, retry_cnt = 5;
    MADURA_DEVICE_AUX_MODE_T aux_mode;
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;

    PHYMOD_MEMSET(&mst_misc0, 0, sizeof(BCMI_MADURA_MST_MISC0r_t));
    PHYMOD_MEMSET(&mst_misc1, 0, sizeof(BCMI_MADURA_MST_MISC1r_t));
    PHYMOD_MEMSET(&mst_misc2, 0, sizeof(BCMI_MADURA_MST_MISC2r_t));
    PHYMOD_MEMSET(&mst_misc3, 0, sizeof(BCMI_MADURA_MST_MISC3r_t));
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = &aux_mode;
    PHYMOD_IF_ERR_RETURN(
		    _madura_phy_interface_config_get(pa, 0, &config));
    MADURA_GET_IP(pa, config, ip);
    PHYMOD_DEBUG_VERBOSE(("%s:: IP:%s \n", __func__,
			    (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON"));

    PHYMOD_DEBUG_VERBOSE(("madura looopback set\n"));
    do {
	    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en));
	    data=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data != 0) && (retry_cnt --));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("loopback config failed, micro controller is busy..")));
    }

    /* FALCON core loopback set */
    PHYMOD_IF_ERR_RETURN(
		    _madura_falcon_lpbk_set(pa, &config, loopback, enable));

    /* Set bit 1 for loopback change */
    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
	    PHYMOD_IF_ERR_RETURN(
			    BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));
	    lp_bck_val_misc = BCMI_MADURA_MST_MISC0r_MISC0f_GET(mst_misc0); 
	    lp_bck_val_misc  = lp_bck_val_misc | 0x80 ;/*enable loopback as 1*/
        BCMI_MADURA_MST_MISC0r_MISC0f_SET(mst_misc0,lp_bck_val_misc); 
	    PHYMOD_IF_ERR_RETURN(
			    BCMI_MADURA_WRITE_MST_MISC0r(pa, mst_misc0));
    } else  if (lane_mask == 0x2) {
    	    PHYMOD_IF_ERR_RETURN(
			    BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));
	    lp_bck_val_misc = BCMI_MADURA_MST_MISC1r_MISC1f_GET(mst_misc1); 
	    lp_bck_val_misc  = lp_bck_val_misc | 0x80 ;/*enable loopback as 1*/
			    BCMI_MADURA_MST_MISC1r_MISC1f_SET(mst_misc1,lp_bck_val_misc);
	    PHYMOD_IF_ERR_RETURN(
			    BCMI_MADURA_WRITE_MST_MISC1r(pa, mst_misc1));
    } else  if (lane_mask == 0xC || lane_mask == 0x4 ) {
     	    PHYMOD_IF_ERR_RETURN(
			    BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));
	    lp_bck_val_misc = BCMI_MADURA_MST_MISC2r_MISC2f_GET(mst_misc2); 
	    lp_bck_val_misc  = lp_bck_val_misc | 0x80 ;/*enable loopback as 1*/
        BCMI_MADURA_MST_MISC2r_MISC2f_SET(mst_misc2,lp_bck_val_misc);
	    PHYMOD_IF_ERR_RETURN(
			    BCMI_MADURA_WRITE_MST_MISC2r(pa, mst_misc2));
    } else {
    	    PHYMOD_IF_ERR_RETURN(
			    BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
	    lp_bck_val_misc = BCMI_MADURA_MST_MISC3r_MISC3f_GET(mst_misc3); 
	    lp_bck_val_misc  = lp_bck_val_misc | 0x80 ;/*enable loopback as 1*/
        BCMI_MADURA_MST_MISC3r_MISC3f_SET(mst_misc3,lp_bck_val_misc);
	    PHYMOD_IF_ERR_RETURN(
			    BCMI_MADURA_WRITE_MST_MISC3r(pa, mst_misc3));
    }

    /*  Set FW_ENABLE = 1 */
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en));
    /* coverity[operator_confusion] */
    BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en,1);
    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_WRITE_FIRMWARE_ENr(pa,fw_en));
   do {
	    PHYMOD_IF_ERR_RETURN(
		    BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en));
	    data=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data != 0) && (retry_cnt --));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("loopback config failed, micro controller is busy..")));
    }

    return PHYMOD_E_NONE;
}


int _madura_falcon_phy_reset_set(const phymod_access_t *pa, const phymod_phy_reset_t* reset)
{
  #if 1  
    BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_t pwrdwn_ctrl_tx;
    BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_t pwrdwn_ctrl_rx;
    PHYMOD_MEMSET(&pwrdwn_ctrl_tx, 0, sizeof(BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_t));
    PHYMOD_MEMSET(&pwrdwn_ctrl_rx, 0, sizeof(BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_t));
    /* TX AFE Lane Reset */
    switch (reset->tx) {
        /* In Reset */
        case phymodResetDirectionIn:
		BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRC_VALf_SET(pwrdwn_ctrl_tx,1);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,1);
		BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx);
        break;
        /* Out Reset */
        case phymodResetDirectionOut:
		BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRC_VALf_SET(pwrdwn_ctrl_tx,0);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,1);
		BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx);
            /* -- Releasing forces -- */
		BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,0);
		BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx);
        break;
        /* Toggle Reset */
        case phymodResetDirectionInOut:
		BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRC_VALf_SET(pwrdwn_ctrl_tx,1);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,1);
		BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx);
		PHYMOD_USLEEP(10);
		BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRC_VALf_SET(pwrdwn_ctrl_tx,0);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,1);
		BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx);
            /* -- Releasing forces -- */
		BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,0);
		BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx);
        break;
        default:
        break;
    }
    /* RX AFE Lane Reset */
    switch (reset->rx) {
        /* In Reset */
        case phymodResetDirectionIn:
		BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx);    
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRC_VALf_SET(pwrdwn_ctrl_rx,1);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,1);
 		BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx);
        break;
        /* Out Reset */
	case phymodResetDirectionOut:
		BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx);    
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRC_VALf_SET(pwrdwn_ctrl_rx,0);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,1);
 		BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx);
            /* -- Releasing forces -- */
		BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx);    
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,0);
 		BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx);
        break;
        /* Toggle Reset */
        case phymodResetDirectionInOut:
		BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx);    
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRC_VALf_SET(pwrdwn_ctrl_rx,1);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,1);
 		BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx);
            PHYMOD_USLEEP(10);
		BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx);    
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRC_VALf_SET(pwrdwn_ctrl_rx,0);
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,1);
 		BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx);
            /* -- Releasing forces -- */
		BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx);    
		BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,0);
 		BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx);
        break;
        default:
        break;
    }

#endif
    return PHYMOD_E_NONE;
}


int _madura_fal_2x_phy_reset_set(const phymod_access_t *pa, const phymod_phy_reset_t* reset)
{
    return PHYMOD_E_NONE;
}

int _madura_phy_reset_set(const phymod_access_t *phy, const phymod_phy_reset_t* reset)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    PHYMOD_IF_ERR_RETURN(
       _madura_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(phy);
    MADURA_GET_IP(phy, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"2X_FALCON":"FALCON", max_lane, lane_mask));


    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
		PHYMOD_IF_ERR_RETURN(
                _madura_set_slice_reg (phy,
                        ip, MADURA_DEV_PMA_PMD, lane));
                /* FALCON TX/RX Reset */
                PHYMOD_DEBUG_VERBOSE(("Falcon TX/RX Reset set\n"));
		PHYMOD_IF_ERR_RETURN(
                    _madura_falcon_phy_reset_set(phy, reset));
        }
    }

    MADURA_RESET_SLICE(phy, MADURA_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}
int _madura_phy_reset_get(const phymod_access_t *pa, phymod_phy_reset_t* reset)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t rst = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr_t rx_get;
    BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr_t tx_get;
   
    PHYMOD_MEMSET(&rx_get, 0, sizeof(BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr_t));
    PHYMOD_MEMSET(&tx_get, 0, sizeof(BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    PHYMOD_IF_ERR_RETURN(
       _madura_phy_interface_config_get(pa, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));


    for (lane = 0; lane < max_lane; lane ++) {
	    if (lane_mask & (1 << lane)) {
		    PHYMOD_IF_ERR_RETURN(
				    _madura_set_slice_reg (pa,
					    ip, MADURA_DEV_PMA_PMD, lane));
		    /* FALCON TX/RX Reset */
		    PHYMOD_DEBUG_VERBOSE(("Falcon TX/RX Reset get\n"));
		    PHYMOD_IF_ERR_RETURN(
				    BCMI_MADURA_READ_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr(pa,&tx_get));
		    rst=BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr_AFE_TX_RESETf_GET(tx_get);
		    if(rst == 0) {
			    /* Out of Reset */
			    reset->tx = phymodResetDirectionOut;
		    } else {
			    /* In Reset */
			    reset->tx = phymodResetDirectionIn;
		    }
		    PHYMOD_IF_ERR_RETURN(
				    BCMI_MADURA_READ_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr(pa,&rx_get));
		    rst=BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr_AFE_RX_RESETf_GET(rx_get);
		    if(rst == 0) {
			    /* Out of Reset */
			    reset->rx = phymodResetDirectionOut;
		    } else {
			    /* In Reset */
			    reset->rx = phymodResetDirectionIn;
		    }

	    }
    }
    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}


int _madura_phy_status_dump(const phymod_access_t *pa)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", lane_mask));

    USR_PRINTF(("**********************************************\n"));
    USR_PRINTF(("******* PHY status dump for PHY ID:0x%x ********\n",pa->addr));
    USR_PRINTF(("**********************************************\n"));
    USR_PRINTF(("**** PHY status dump for interface side:%d ****\n",ip));
    USR_PRINTF(("***********************************************\n"));
    
    for(lane = 0; lane < MADURA_MAX_FALCON_LANE; lane ++) {
	    if (lane_mask & (1 << lane)) {
		    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
				    _madura_set_slice_reg (pa,
					    ip, MADURA_DEV_PMA_PMD, lane)));
		    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (falcon2_madura_display_core_config(pa)));
		    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (falcon2_madura_display_core_state(pa)));
		    break;
	    }
    }
    for(lane = 0; lane < MADURA_MAX_FALCON_LANE; lane ++) {
            if (lane_mask & (1 << lane)) {
		    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
				    _madura_set_slice_reg (pa,
					    ip, MADURA_DEV_PMA_PMD, lane)));
                PHYMOD_DEBUG_VERBOSE(("Falcon Status dump\n"));
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (falcon2_madura_display_lane_state_hdr(pa)));
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (falcon2_madura_display_lane_state(pa)));
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (falcon2_madura_display_lane_config(pa)));
            }
    }

    return PHYMOD_E_NONE;
}

int _madura_tx_enable_set(const phymod_access_t *pa, int16_t enable) 
{
    return PHYMOD_E_NONE;
}

int _madura_tx_enable_get(const phymod_access_t *pa, int16_t *enable)
{
    return PHYMOD_E_NONE;
}


int _madura_tx_dp_reset(const phymod_access_t *pa)
{
    return PHYMOD_E_NONE;
}

int _madura_rx_enable_set(const phymod_access_t *pa, int16_t enable) 
{
    return PHYMOD_E_NONE;
}

int _madura_rx_enable_get(const phymod_access_t *pa, int16_t *enable)
{
    return PHYMOD_E_NONE;
}

int _madura_rx_dp_reset(const phymod_access_t *pa)
{
    return PHYMOD_E_NONE;
}


int _madura_tx_lane_control_set(const phymod_access_t *pa,  phymod_phy_tx_lane_control_t tx_control)
{
    switch (tx_control) {
        case phymodTxTrafficDisable: /* disable tx traffic */
        case phymodTxTrafficEnable: /* enable tx traffic */
            return PHYMOD_E_UNAVAIL;
        case phymodTxReset: /* reset tx data path */
            PHYMOD_IF_ERR_RETURN(_madura_tx_dp_reset(pa));
        break;
        case phymodTxSquelchOn: /* squelch tx */
            PHYMOD_IF_ERR_RETURN(_madura_tx_enable_set(pa, MADURA_DISABLE));
        break;
        case phymodTxSquelchOff: /* squelch tx off */
            PHYMOD_IF_ERR_RETURN(_madura_tx_enable_set(pa, MADURA_ENABLE));
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}
int _madura_rx_lane_control_set(const phymod_access_t *pa,  phymod_phy_rx_lane_control_t rx_control)
{
    switch (rx_control) {
        case phymodRxReset: /* reset rx data path */
            PHYMOD_IF_ERR_RETURN(_madura_rx_dp_reset(pa));
            break;
        case phymodRxSquelchOn: /* squelch rx */
            PHYMOD_IF_ERR_RETURN(_madura_rx_enable_set(pa, MADURA_DISABLE));
        break;
        case phymodRxSquelchOff: /* squelch rx off */
            PHYMOD_IF_ERR_RETURN(_madura_rx_enable_set(pa, MADURA_ENABLE));
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}

int _madura_tx_lane_control_get(const phymod_access_t *pa,  phymod_phy_tx_lane_control_t *tx_control)
{
    int16_t tx_ctrl = 0;
    switch (*tx_control) {
        case phymodTxTrafficDisable: /* disable tx traffic */
        case phymodTxTrafficEnable: /* enable tx traffic */
        case phymodTxReset: /* reset tx data path */
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodTxSquelchOn: /* squelch tx */
            PHYMOD_IF_ERR_RETURN(_madura_tx_enable_get(pa, &tx_ctrl));
            *tx_control = tx_ctrl ? phymodTxSquelchOff: phymodTxSquelchOn;
        break;
        case phymodTxSquelchOff: /* squelch tx off */
            PHYMOD_IF_ERR_RETURN(_madura_tx_enable_get(pa, &tx_ctrl));
            *tx_control = tx_ctrl ? phymodTxSquelchOff: phymodTxSquelchOn;
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}
int _madura_rx_lane_control_get(const phymod_access_t *pa,  phymod_phy_rx_lane_control_t *rx_control)
{
    int16_t rx_ctrl = 0;
    switch (*rx_control) {
        case phymodRxReset: /* reset rx data path */
            return PHYMOD_E_UNAVAIL;
        case phymodRxSquelchOn: /* squelch rx */
            PHYMOD_IF_ERR_RETURN(_madura_rx_enable_get(pa, &rx_ctrl));
            *rx_control = rx_ctrl ? phymodRxSquelchOff: phymodRxSquelchOn;
        break;
        case phymodRxSquelchOff: /* squelch rx off */
            PHYMOD_IF_ERR_RETURN(_madura_rx_enable_get(pa, &rx_ctrl));
            *rx_control = rx_ctrl ? phymodRxSquelchOff: phymodRxSquelchOn;
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}


int _madura_config_hcd_link_sts (const phymod_access_t* pa, phymod_phy_inf_config_t config, uint16_t ip ) 
{
    return PHYMOD_E_NONE;
}


int _madura_port_from_lane_map_get(const phymod_access_t *pa, const phymod_phy_inf_config_t* cfg, uint16_t *port)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane_mask = 0;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_MEMCPY(&config, cfg, sizeof(phymod_phy_inf_config_t));
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*)cfg->device_aux_modes;
    MADURA_GET_IP(pa, config, ip);

    PHYMOD_DEBUG_VERBOSE(("%s:: IP:%s \n", __func__,
                 (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON"));

    if (config.data_rate == MADURA_SPD_100G) {
        *port = 0;
    } else if (config.data_rate == MADURA_SPD_40G) {
        if (ip == MADURA_FALCON_CORE) {
            if (aux_mode->pass_thru_dual_lane  ){
                if (lane_mask == 0x3) {        /* port 0 */
                    *port = 0;
                } else if (lane_mask == 0xC) { /* port 1 */
                    *port = 1;
                }
            } else if (aux_mode->pass_thru  ){
                if (lane_mask == 0xF) {        /*port 0 */
                    *port = 0;
                }
            } else {   /* 40G MUX */
                if (lane_mask == 0x3) {        /* port 0 */
                    *port = 0;
                } else if (lane_mask == 0xC) { /* port 1 */
                    *port = 1;
                }
            }
        } else {/*Line side*/
            if (aux_mode->pass_thru_dual_lane  ){
                if (lane_mask == 0x3) {        /* port 0 */
                    *port = 0;
                } else if (lane_mask == 0xC) { /* port 1 */
                    *port = 1;
                }
            } else if (aux_mode->pass_thru ) {
                if (lane_mask == 0xf0) {       /* port 0 . alternate*/
                    *port = 0;
                }
                else if (lane_mask == 0xF) {   /* port 0 */
                    *port = 0;
                }
            } else {   /* 40G MUX */
                if (lane_mask == 0xF) {        /* port 0 */
                    *port = 0;
                } else if (lane_mask == 0xF0) { /* port 1 */
                    *port = 1;
                }
            }
        }
#ifdef MADURA_UNTESTED_CODE
    } else if (config.data_rate == MADURA_SPD_20G) {
        if (ip == MADURA_FALCON_CORE) {
            if (aux_mode->pass_thru) {
                if (lane_mask == 0x3) {        /* port 0 */
                    *port = 0;
                } else if (lane_mask == 0xC) { /* port 1 */
                    *port = 1;
                }
            } else {   /* 20G MUX */
                if (lane_mask == 0x1) {        /* port 0 */
                    *port = 0;
                } else if (lane_mask == 0x4) { /* port 1 */
                    *port = 1;
                }
            }
        } else  { /* 20G PT & MUX */
            if (lane_mask == 0x3) {          /* port 0 */
                *port = 0;
            } else if (lane_mask == 0x30) {  /* port 1 */
                *port = 1;
            } else {
            }
        }
#endif
    } else if (config.data_rate == MADURA_SPD_10G) {
        if (ip == MADURA_FALCON_CORE) {
            if (lane_mask == 0x1) {          /* port 0 */
                *port = 0;
            } else if (lane_mask == 0x2) {   /* port 1 */
                *port = 1;
            } else if (lane_mask == 0x4) {   /* port 2 */
                *port = 2;
            } else if (lane_mask == 0x8) {   /* port 3 */
                *port = 3;
            }
        } else {
            if (lane_mask == 0x1) {          /* port 0 */
                *port = 0;
            } else if (lane_mask == 0x2) {   /* port 1 */
                *port = 1;
            } else if (lane_mask == 0x4) {  /* port 2 */
                *port = 2;
            } else if (lane_mask == 0x8) {  /* port 3 */
                *port = 3;
            } else if ((lane_mask == 0x10) && (aux_mode->alternate)){
                *port = 0;
            } else if ((lane_mask == 0x20) && (aux_mode->alternate)){
                *port = 1;
            } else if ((lane_mask == 0x40) && (aux_mode->alternate)){
                *port = 2;
            } else if ((lane_mask == 0x80) && (aux_mode->alternate)){
                *port = 3;
            }
        }
    }

    return PHYMOD_E_NONE;
}

int _madura_tx_rx_polarity_set(const phymod_access_t *pa, uint32_t tx_polarity, uint32_t rx_polarity)
{
#if 1
    BCMI_MADURA_SLICEr_t slice_reg;
    BCMI_MADURA_TLB_TX_TLB_TX_MISC_CFGr_t tx_pl_cfg;
    BCMI_MADURA_TLB_RX_TLB_RX_MISC_CFGr_t rx_pl_cfg; 
    phymod_phy_inf_config_t config;
    uint16_t ip = 0,intf_side=0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    uint32_t rx_lane_polarity = 0;
    uint32_t tx_lane_polarity = 0;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&tx_pl_cfg, 0 , sizeof(BCMI_MADURA_TLB_TX_TLB_TX_MISC_CFGr_t));
    PHYMOD_MEMSET(&rx_pl_cfg, 0 , sizeof(BCMI_MADURA_TLB_RX_TLB_RX_MISC_CFGr_t));
    if (rx_polarity == 0xFFFF && tx_polarity == 0xFFFF) {
        return PHYMOD_E_NONE;
    }

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);
    MADURA_GET_INTF_SIDE(pa, intf_side); 
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;


    /* FALCON polarity inversion */
    PHYMOD_DEBUG_VERBOSE(("Falcon polarity inversion set tx=%x rx=%xat %d\n",tx_polarity,rx_polarity,__LINE__));
	for (lane = 0; lane < max_lane; lane ++) {
	    if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD, lane)));
            if (rx_polarity != 0xFFFF) {
                rx_lane_polarity = (rx_polarity >> lane) & 0x1;
            }
            if (tx_polarity != 0xFFFF) {
                tx_lane_polarity = (tx_polarity >> lane) & 0x1;
            }
                
		/* FALCON polarity inversion */
                PHYMOD_DEBUG_VERBOSE(("Falcon polarity inversion set\n"));
               BCMI_MADURA_READ_TLB_TX_TLB_TX_MISC_CFGr(pa,&tx_pl_cfg); 
	       BCMI_MADURA_READ_TLB_RX_TLB_RX_MISC_CFGr(pa,&rx_pl_cfg);	
                /* Write to Tx misc config register */
               BCMI_MADURA_TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_SET(tx_pl_cfg,tx_lane_polarity);
               BCMI_MADURA_WRITE_TLB_TX_TLB_TX_MISC_CFGr(pa,tx_pl_cfg); 
	        /* Write to Rx misc config register */
		BCMI_MADURA_TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_SET(rx_pl_cfg,rx_lane_polarity);
		BCMI_MADURA_WRITE_TLB_RX_TLB_RX_MISC_CFGr(pa,rx_pl_cfg);
	    }
    }
    /*Reset DEV1 slice register*/	     	
    BCMI_MADURA_WRITE_SLICEr(pa,slice_reg);
    PHYMOD_FREE(config.device_aux_modes);

#endif
    return PHYMOD_E_NONE;
}

int _madura_tx_rx_polarity_get(const phymod_access_t *phy, uint32_t *tx_polarity, uint32_t *rx_polarity)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = phy;
    BCMI_MADURA_TLB_TX_TLB_TX_MISC_CFGr_t tx_pl_cfg;
    BCMI_MADURA_TLB_RX_TLB_RX_MISC_CFGr_t rx_pl_cfg; 

    *tx_polarity = 0;
    *rx_polarity = 0;

    PHYMOD_MEMSET(&tx_pl_cfg, 0 , sizeof(BCMI_MADURA_TLB_TX_TLB_TX_MISC_CFGr_t));
    PHYMOD_MEMSET(&rx_pl_cfg, 0 , sizeof(BCMI_MADURA_TLB_RX_TLB_RX_MISC_CFGr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, 
       _madura_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(phy, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"2X_FALCON":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, 
              _madura_set_slice_reg (pa,
		      ip, MADURA_DEV_PMA_PMD, lane));
	    /* FALCON polarity */
	    PHYMOD_DEBUG_VERBOSE(("Falcon polarity get\n"));
	    BCMI_MADURA_READ_TLB_TX_TLB_TX_MISC_CFGr(pa,&tx_pl_cfg); 
	    BCMI_MADURA_READ_TLB_RX_TLB_RX_MISC_CFGr(pa,&rx_pl_cfg);	
	    /* Read to Tx misc config register */
	    *tx_polarity|=(BCMI_MADURA_TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_GET(tx_pl_cfg))<<lane;
	    /* Read to Rx misc config register */
	    *rx_polarity|=(BCMI_MADURA_TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_GET(rx_pl_cfg))<<lane;
	}
    }

    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}

 int _madura_pll_sequencer_restart(const phymod_core_access_t *core, phymod_sequencer_operation_t operation)
{
    const phymod_access_t *pa = &core->access;
    BCMI_MADURA_COMMON_CTLr_t fal_cmn_ctrl;

    PHYMOD_MEMSET(&fal_cmn_ctrl, 0 , sizeof(BCMI_MADURA_COMMON_CTLr_t));
    switch(operation) {
        case phymodSeqOpStop:
        case phymodSeqOpStart:
            return PHYMOD_E_UNAVAIL;
        /* Toggle PLL Sequencer with pmd_core_dp_h_rstb_frcval */
        case phymodSeqOpRestart:
                BCMI_MADURA_READ_COMMON_CTLr(pa,&fal_cmn_ctrl);
                BCMI_MADURA_COMMON_CTLr_PMD_CORE_PLL0_DP_H_RSTB_FRCf_SET(fal_cmn_ctrl,1);
                BCMI_MADURA_COMMON_CTLr_PMD_CORE_PLL0_DP_H_RSTB_FRCVALf_SET(fal_cmn_ctrl,0);
                BCMI_MADURA_WRITE_COMMON_CTLr(pa,fal_cmn_ctrl);
                PHYMOD_USLEEP(1000);
                BCMI_MADURA_READ_COMMON_CTLr(pa,&fal_cmn_ctrl);
                BCMI_MADURA_COMMON_CTLr_PMD_CORE_PLL0_DP_H_RSTB_FRCf_SET(fal_cmn_ctrl,1);
                BCMI_MADURA_COMMON_CTLr_PMD_CORE_PLL0_DP_H_RSTB_FRCVALf_SET(fal_cmn_ctrl,1);
                BCMI_MADURA_WRITE_COMMON_CTLr(pa,fal_cmn_ctrl);
        break;
        default:
        break;
    }
    /* Clear FRC of rstb */
    BCMI_MADURA_COMMON_CTLr_PMD_CORE_PLL0_DP_H_RSTB_FRCf_SET(fal_cmn_ctrl,0);
    BCMI_MADURA_WRITE_COMMON_CTLr(pa,fal_cmn_ctrl);

    return PHYMOD_E_NONE;
}

int _madura_fec_enable_set(const phymod_access_t *phy, uint32_t enable)
{
    const phymod_access_t *pa = phy;
    BCMI_MADURA_DP_CTL0r_t fec_ctl;

    PHYMOD_MEMSET(&fec_ctl, 0, sizeof(BCMI_MADURA_DP_CTL0r_t));
    BCMI_MADURA_READ_DP_CTL0r(pa,&fec_ctl);
    BCMI_MADURA_DP_CTL0r_CL91_EN_0f_SET(fec_ctl,enable);
    BCMI_MADURA_WRITE_DP_CTL0r(pa,fec_ctl);

    return PHYMOD_E_NONE;
}


int _madura_fec_enable_get(const phymod_access_t *phy, uint32_t* enable)
{
    const phymod_access_t *pa = phy;
    BCMI_MADURA_DP_CTL0r_t fec_ctl;

    PHYMOD_MEMSET(&fec_ctl, 0, sizeof(BCMI_MADURA_DP_CTL0r_t));
    BCMI_MADURA_READ_DP_CTL0r(pa,&fec_ctl);
    *enable=BCMI_MADURA_DP_CTL0r_CL91_EN_0f_GET(fec_ctl);
    BCMI_MADURA_WRITE_DP_CTL0r(pa,fec_ctl);

    return PHYMOD_E_NONE;
}


int _madura_tx_power_set(const phymod_access_t *phy, uint32_t power_tx)
{
#if 1
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0,max_lane=0,intf_side=0;
    uint16_t lane_mask = 0;
    const phymod_access_t *pa = phy;
    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_TXr_t tx_pwr_dwn;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&tx_pwr_dwn, 0, sizeof(BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_TXr_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    PHYMOD_IF_ERR_RETURN(
       _madura_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(phy, config, ip);

    MADURA_GET_INTF_SIDE(pa, intf_side); 
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for(lane = 0; lane < max_lane; lane ++) {
	    if (lane_mask & (1 << lane)) {
		    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
					    _madura_set_slice_reg (pa, 
						    ip, MADURA_DEV_PMA_PMD, lane)));
		    /* FALCON TX Power Set */
		    PHYMOD_DEBUG_VERBOSE(("Falcon TX Power set\n"));
		    switch (power_tx) {
			    case phymodPowerOff: /* Turn off power */
				    PHYMOD_IF_ERR_RETURN(
						    BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,&tx_pwr_dwn));
				    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_TXr_LN_TX_S_PWRDNf_SET(tx_pwr_dwn,1);	 
				    PHYMOD_IF_ERR_RETURN(
						    BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,tx_pwr_dwn));
				    break;
			    case phymodPowerOn: /* Turn on power */
				    PHYMOD_IF_ERR_RETURN(
						    BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,&tx_pwr_dwn));
				    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_TXr_LN_TX_S_PWRDNf_SET(tx_pwr_dwn,0);	 
				    PHYMOD_IF_ERR_RETURN(
						    BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,tx_pwr_dwn));
				    break;
			    case phymodPowerOffOn: /* Toggle power */
				    PHYMOD_IF_ERR_RETURN(
						    BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,&tx_pwr_dwn));
				    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_TXr_LN_TX_S_PWRDNf_SET(tx_pwr_dwn,1);	 
				    PHYMOD_IF_ERR_RETURN(
						    BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,tx_pwr_dwn));
				    PHYMOD_USLEEP(10000);
				    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_TXr_LN_TX_S_PWRDNf_SET(tx_pwr_dwn,0);	 
				    PHYMOD_IF_ERR_RETURN(
						    BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,tx_pwr_dwn));
				    break;
			    case phymodPowerNoChange: /* Stay where you are */
				    break;
			    default:
				    break;
		    }
	    }
    }

    /*Reset DEV1 slice register*/	     	
    BCMI_MADURA_WRITE_SLICEr(pa,slice_reg);
    PHYMOD_FREE(config.device_aux_modes);
#endif
    return PHYMOD_E_NONE;
}


int _madura_rx_power_set(const phymod_access_t *phy, uint32_t power_rx)
{
#if 1
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0,max_lane=0,intf_side=0;
    const phymod_access_t *pa = phy;
    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_RXr_t rx_pwr_dwn;
    BCMI_MADURA_SLICEr_t slice_reg;
    
    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&rx_pwr_dwn, 0, sizeof(BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_RXr_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    PHYMOD_IF_ERR_RETURN(
       _madura_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    MADURA_GET_IP(phy, config, ip);
    MADURA_GET_INTF_SIDE(pa, intf_side); 
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

        for(lane = 0; lane < max_lane; lane ++) {
            if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa, 
                        ip, MADURA_DEV_PMA_PMD, lane)));
                /* FALCON RX Power Set */
                PHYMOD_DEBUG_VERBOSE(("Falcon RX Power set\n"));
                switch (power_rx) {
                    case phymodPowerOff: /* Turn off power */
			    PHYMOD_IF_ERR_RETURN(
					    BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,&rx_pwr_dwn));
			    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_RXr_LN_RX_S_PWRDNf_SET(rx_pwr_dwn,1);
			    PHYMOD_IF_ERR_RETURN(
					    BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,rx_pwr_dwn));
                    break;
                    case phymodPowerOn: /* Turn on power */
			    PHYMOD_IF_ERR_RETURN(
					    BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,&rx_pwr_dwn));
			    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_RXr_LN_RX_S_PWRDNf_SET(rx_pwr_dwn,0);
			    PHYMOD_IF_ERR_RETURN(
					    BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,rx_pwr_dwn));
                    break;
                    case phymodPowerOffOn: /* Toggle power */
			    PHYMOD_IF_ERR_RETURN(
					    BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,&rx_pwr_dwn));
			    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_RXr_LN_RX_S_PWRDNf_SET(rx_pwr_dwn,1);
			    PHYMOD_IF_ERR_RETURN(
					    BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,rx_pwr_dwn));
			    PHYMOD_USLEEP(10000);
			    PHYMOD_IF_ERR_RETURN(
					    BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,&rx_pwr_dwn));
			    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_RXr_LN_RX_S_PWRDNf_SET(rx_pwr_dwn,0);
			    PHYMOD_IF_ERR_RETURN(
					    BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,rx_pwr_dwn));
                    break;
                    case phymodPowerNoChange: /* Stay where you are */
                    break;
                    default:
                    break;
                }
            }
        }
    BCMI_MADURA_WRITE_SLICEr(pa,slice_reg);
#endif 
   return PHYMOD_E_NONE;
}

int _madura_tx_rx_power_get(const phymod_access_t *phy, phymod_phy_power_t* power)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0,intf_side=0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = phy;
    BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr_t rx_pwr_get;
    BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr_t tx_pwr_get;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&rx_pwr_get, 0, sizeof(BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr_t));
    PHYMOD_MEMSET(&tx_pwr_get, 0, sizeof(BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    PHYMOD_IF_ERR_RETURN(
       _madura_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(phy, config, ip);

    MADURA_GET_INTF_SIDE(pa, intf_side); 
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa, 
                        ip, MADURA_DEV_PMA_PMD, lane)));
                /* FALCON TX/RX Power Get */
                PHYMOD_DEBUG_VERBOSE(("Falcon TX/RX Power get\n"));
		PHYMOD_IF_ERR_RETURN(
			  	BCMI_MADURA_READ_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr(pa,&rx_pwr_get));
		PHYMOD_IF_ERR_RETURN(
				BCMI_MADURA_READ_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr(pa,&tx_pwr_get));
                power->tx = !BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr_AFE_RX_PWRDNf_GET(rx_pwr_get);
                power->rx = !BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr_AFE_TX_PWRDNf_GET(tx_pwr_get);
	}
    }
    PHYMOD_DEBUG_VERBOSE(("pwr tx=%d rx=%d\n",power->tx,power->rx));
    PHYMOD_FREE(config.device_aux_modes);
    BCMI_MADURA_WRITE_SLICEr(pa,slice_reg);
    return PHYMOD_E_NONE;
}
 
