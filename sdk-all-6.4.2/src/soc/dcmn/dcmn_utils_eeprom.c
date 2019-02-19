/****************************************************************** 
*                                                                   
* FILENAME:       utils_eeprom.c                      
* $Id: dcmn_utils_eeprom.c,v 1.7 Broadcom SDK $
*                                                                   
* MODULE PREFIX:  $prefix$                               
*                                                                   
* SYSTEM:           Broadcom LTD Proprietary information
*                                                                   
* CREATION DATE:  $date$                                  
*                                                                   
* LAST CHANGED:   $date$
*                                                                   
* REVISION:       $revision$                                       
*                                                                   
* FILE DESCRIPTION:                                                 
*                                                                   
* REMARKS:                                                          
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
*******************************************************************/   
#if defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__)



#include <shared/bsl.h>

#include <sal/types.h>
#include <soc/drv.h>
#include <soc/i2c.h>

#include <soc/dcmn/dcmn_utils_eeprom.h>

#include <appl/diag/dcmn/bsp_cards_consts.h> 


int negev_chassis_eeprom_write(int addr, int size, int val)
{
    int ret = 0;
    int i;
    int eeprom_addr = 0, eeprom_int_addr = 0, cur_val = 0;

    if (addr >= NEGEV_CHASSIS_EEPROM_NUM_MAX_ADRESS) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: Adress is too high. addr=%d\n"), FUNCTION_NAME(), addr));
        ret = -1;
        goto exit;
    } else if (addr < NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK) {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_1;
        eeprom_int_addr = addr;
    } else {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_2;
        eeprom_int_addr = addr - NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK;
    }

    for(i=0 ; i < size ; i++) {
        cur_val = (val >> (i * 8)) & 0xff;
        
        /*
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): i=%d, eprom_addr=0x%x, eeprom_int_addr=0x%x,CPU_I2C_ALEN_BYTE_DLEN_BYTE=0x%x, cur_val=0x%x \n"), 
FUNCTION_NAME(), i, eeprom_addr, eeprom_int_addr, CPU_I2C_ALEN_BYTE_DLEN_BYTE, cur_val));
        */

        ret = cpu_i2c_write(eeprom_addr, eeprom_int_addr, CPU_I2C_ALEN_BYTE_DLEN_BYTE, cur_val);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: cpu_i2c_write Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }

        eeprom_int_addr++;

        /* Wait AT24C4 write time cycle ~5ms */
        sal_usleep(60000);
    }

exit:
    return ret;
}

int negev_chassis_eeprom_write_str(int addr, int size, char *str)
{
    int ret = 0;
    int i;
    int eeprom_addr = 0, eeprom_int_addr = 0, cur_val = 0;

    if (addr >= NEGEV_CHASSIS_EEPROM_NUM_MAX_ADRESS) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: Adress is too high. addr=%d\n"), FUNCTION_NAME(), addr));
        ret = -1;
        goto exit;
    } else if (addr < NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK) {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_1;
        eeprom_int_addr = addr;
    } else {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_2;
        eeprom_int_addr = addr - NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK;
    }

    for(i=0 ; i < size ; i++) {
        cur_val = str[i] & 0xff;
/*        
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): i=%d, eprom_addr=0x%x, eeprom_int_addr=0x%x,CPU_I2C_ALEN_BYTE_DLEN_BYTE=0x%x, cur_val=0x%x, str=%s\n"), 
FUNCTION_NAME(), i, eeprom_addr, eeprom_int_addr, CPU_I2C_ALEN_BYTE_DLEN_BYTE, cur_val, str));
*/      

        ret = cpu_i2c_write(eeprom_addr, eeprom_int_addr, CPU_I2C_ALEN_BYTE_DLEN_BYTE, cur_val);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: cpu_i2c_write Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }

        eeprom_int_addr++;

        /* Wait AT24C4 write time cycle ~5ms */
        sal_usleep(60000);
    }

exit:
    return ret;
}

int negev_chassis_eeprom_read(int addr, int size, int *val)
{
#ifndef __KERNEL__
    int ret = 0;
    int i;
    int eeprom_addr = 0, eeprom_int_addr = 0, cur_val = 0;

    if (addr >= NEGEV_CHASSIS_EEPROM_NUM_MAX_ADRESS) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: Adress is too high. addr=%d\n"), FUNCTION_NAME(), addr));
        ret = -1;
        goto exit;
    } else if (addr < NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK) {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_1;
        eeprom_int_addr = addr;
    } else {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_2;
        eeprom_int_addr = addr - NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK;
    }

    *val = 0x0;

    for(i=0 ; i < size ; i++) {

        ret = cpu_i2c_read(eeprom_addr, eeprom_int_addr, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &cur_val);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: cpu_i2c_read Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }

        /*
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): i=%d, eprom_addr=0x%x, eeprom_int_addr=0x%x,CPU_I2C_ALEN_BYTE_DLEN_BYTE=0x%x, cur_val=0x%x, *val=0x%x \n"), 
FUNCTION_NAME(), i, eeprom_addr, eeprom_int_addr, CPU_I2C_ALEN_BYTE_DLEN_BYTE, cur_val, *val));
        */

        *val |= (cur_val & 0xff) << (i * 8);

        eeprom_int_addr++;

        /* Wait AT24C4 write time cycle ~5ms */
        sal_usleep(60000);
    }

exit:
    return ret;
#endif /* __KERNEL__ */
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("This function is unavailable in Kernel mode\n")));
    return -1;
}

int negev_chassis_eeprom_read_str(int addr, int size, char *str)
{
#ifndef __KERNEL__
    int ret = 0;
    int i;
    int eeprom_addr = 0, eeprom_int_addr = 0, cur_val = 0;

    if (addr >= NEGEV_CHASSIS_EEPROM_NUM_MAX_ADRESS) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: Adress is too high. addr=%d\n"), FUNCTION_NAME(), addr));
        ret = -1;
        goto exit;
    } else if (addr < NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK) {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_1;
        eeprom_int_addr = addr;
    } else {
        eeprom_addr = NEGEV_CHASSIS_EEPROM_ADRESS_BANK_2;
        eeprom_int_addr = addr - NEGEV_CHASSIS_EEPROM_NUM_ADRESS_PER_BANK;
    }

    for(i=0 ; i < size ; i++) {

        ret = cpu_i2c_read(eeprom_addr, eeprom_int_addr, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &cur_val);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: cpu_i2c_read Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }

/*      
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): i=%d, eprom_addr=0x%x, eeprom_int_addr=0x%x,CPU_I2C_ALEN_BYTE_DLEN_BYTE=0x%x, cur_val=0x%x, str=%s \n"), 
FUNCTION_NAME(), i, eeprom_addr, eeprom_int_addr, CPU_I2C_ALEN_BYTE_DLEN_BYTE, cur_val, str));
*/      

        str[i] = (cur_val & 0xff);

        eeprom_int_addr++;

        /* Wait AT24C4 write time cycle ~5ms */
        sal_usleep(60000);
    }

exit:
    return ret;
#endif /* __KERNEL__ */
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("This function is unavailable in Kernel mode\n")));
    return -1;
}

int eeprom_write(unsigned short card_type, int addr, int size, int val)
{

    int ret = 0;

    switch (card_type) {
    case NEGEV_CHASSIS_CARD_TYPE:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN_2:
    case FABRIC_CARD_FE1600:
    case FABRIC_CARD_FE1600_BCM88754:
    case NEGEV2_MNG_CARD:
    case LINE_CARD_ARAD:
    case LINE_CARD_ARAD_DVT:
    case LINE_CARD_ARAD_NOACP:
        ret = negev_chassis_eeprom_write(addr, size, val);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: negev_chassis_eeprom_write Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }
        break;
    default:
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: UnKnown card_type=0x%x\n"), FUNCTION_NAME(), card_type));
        ret = -1;
        goto exit;
    }

exit:
    return ret;
}

int eeprom_write_str(unsigned short card_type, int addr, int size, char *str)
{

    int ret = 0;

    switch (card_type) {
    case NEGEV_CHASSIS_CARD_TYPE:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN_2:
    case FABRIC_CARD_FE1600:
    case FABRIC_CARD_FE1600_BCM88754:
    case NEGEV2_MNG_CARD:
    case LINE_CARD_ARAD:
    case LINE_CARD_ARAD_DVT:
    case LINE_CARD_ARAD_NOACP:
        ret = negev_chassis_eeprom_write_str(addr, size, str);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: negev_chassis_eeprom_write_str Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }
        break;
    default:
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: UnKnown card_type=0x%x\n"), FUNCTION_NAME(), card_type));
        ret = -1;
        goto exit;
    }

exit:
    return ret;
}

int eeprom_read(unsigned short card_type, int addr, int size, int *val)
{

    int ret = 0;

    switch (card_type) {
    case NEGEV_CHASSIS_CARD_TYPE:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN_2:
    case FABRIC_CARD_FE1600:
    case FABRIC_CARD_FE1600_BCM88754:
    case NEGEV2_MNG_CARD:
    case LINE_CARD_ARAD:
    case LINE_CARD_ARAD_DVT:
    case LINE_CARD_ARAD_NOACP:
        ret = negev_chassis_eeprom_read(addr, size, val);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: negev_chassis_eeprom_read Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }
        break;
    default:
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: UnKnown card_type=0x%x\n"), FUNCTION_NAME(), card_type));
        ret = -1;
        goto exit;
    }

exit:
    return ret;
}

int eeprom_read_str(unsigned short card_type, int addr, int size, char *str)
{

    int ret = 0;

    switch (card_type) {
    case NEGEV_CHASSIS_CARD_TYPE:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN:
    case LINE_CARD_GFA_PETRA_B_INTERLAKEN_2:
    case FABRIC_CARD_FE1600:
    case FABRIC_CARD_FE1600_BCM88754:
    case NEGEV2_MNG_CARD:
    case LINE_CARD_ARAD:
    case LINE_CARD_ARAD_DVT:
    case LINE_CARD_ARAD_NOACP:
        ret = negev_chassis_eeprom_read_str(addr, size, str);
        if (ret != 0) {
            LOG_INFO(BSL_LS_SOC_COMMON,
                     (BSL_META("%s(): Error: negev_chassis_eeprom_read_str Failed!!! ret=%d\n"), FUNCTION_NAME(), ret));
            goto exit;
        }
        break;
    default:
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("%s(): Error: UnKnown card_type=0x%x\n"), FUNCTION_NAME(), card_type));
        ret = -1;
        goto exit;
    }

exit:
    return ret;
}
#else
/*ISO C forbids an empty source file - Leave one uncomented line*/
#include <sal/types.h>
#endif
