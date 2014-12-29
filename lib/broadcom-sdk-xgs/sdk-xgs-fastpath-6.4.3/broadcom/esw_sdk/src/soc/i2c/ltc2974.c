/*
 * $Id: ltc2974.c,v 1.7 Broadcom SDK $
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
 * I2C Device Driver for LTC2974 an integrated power control IC.
 *
 */

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
 /* Divided by 2^13 for voltage conversion */
#define L16_TO_V(val)  ((val)*100000/8192/100000)
#define V_TO_L16(val)  ((val)*8192)
#define L16_TO_UV(val) ((val)*100000/8192*10)
#define UV_TO_L16(val) ((val)/10*8192/100000)

#define L16_RANGE_UPPER(L16, range) ((L16) + (int)(range)*L16/100000)
#define L16_RANGE_LOWER(L16, range) ((L16) - (int)(range)*L16/100000)

#define MAX_VS_CONFIG    2

#define POWER(exponent, input_val)  exponent < 0 ? \
                            (int)((int) (input_val) << (exponent*(-1))): \
                            (int)((int) (input_val*1000000) >> \
			    exponent)/1000000

static sal_mutex_t ioctl_lock = NULL;

/*
 * Convert a floating point value into a
 * LinearFloat5_11 formatted word
 */
STATIC int
#ifdef  COMPILER_HAS_DOUBLE
ltc2974_float_to_L11(double input_val, uint16* data)
#else
ltc2974_float_to_L11(int input_val, uint16* data)
#endif
{
    uint16 uExponent, uMantissa;
    /* set exponent to -16 */
    int16 exponent = -16;
    /* extract mantissa from input value */
    int mantissa = POWER(exponent, input_val); 

    /* Search for an exponent that produces
     * a valid 11-bit mantissa */
    do
    {
        if((mantissa >= -1024) &&
                (mantissa <= +1023))
        {
            break; /* stop if mantissa valid */
        }
        exponent++;
        mantissa = POWER(exponent, input_val);
    } while (exponent < +15);

    /* Format the exponent of the L11 */
    uExponent = exponent << 11;
    /* Format the mantissa of the L11 */
    uMantissa = mantissa & 0x07FF;
    /* Compute value as exponent | mantissa */
    *(data) = uExponent | uMantissa;
    return SOC_E_NONE;
}

/*
 * Convert a LinearFloat5_11 formatted word
 * into a floating point value
 */
STATIC int
ltc2974_L11_to_float(uint16 input_val, void *data)
{
    /* extract exponent as MS 5 bits */
    int8 exponent = input_val >> 11;
    /* extract mantissa as LS 11 bits */
    int16 mantissa = input_val & 0x7ff;
    /* sign extend exponent from 5 to 8 bits */
    if( exponent > 0x0F ) exponent |= 0xE0;
    /* sign extend mantissa from 11 to 16 bits */
    if( mantissa > 0x03FF ) mantissa |= 0xF800;
#ifdef  COMPILER_HAS_DOUBLE
    /* compute value as mantissa * 2^(exponent) */
    *(double *)data= exponent < 0 ?
                      (double) ((mantissa*1000000) >>
                                (exponent*(-1)))/1000000:
                      (double) (mantissa << exponent);
#else
    *(int *)data= exponent < 0 ?
                   (((mantissa)*1000000) >> (exponent*(-1))):
                   ((mantissa) << exponent)*100000;
#endif
    return SOC_E_NONE;
}

STATIC int
ltc2974_wait_for_not_busy(int unit, int devno)
{
    int rv = SOC_E_NONE;
    uint8 mfr_status, saddr;
    uint32 usec, wait_usec;

    wait_usec = 0;
    usec = 10;

    saddr = soc_i2c_addr(unit, devno);

    while(wait_usec < 1000000) {
        SOC_IF_ERROR_RETURN
            (soc_i2c_read_byte_data(unit, saddr,PMBUS_CMD_MFR_COMMON,
                                    &mfr_status));
        soc_i2c_device(unit, devno)->rbyte++;
        if ((mfr_status & 0x70) == 0x70) {
            /* Bit 6 : Chip not busy */
            /* Bit 5 : calculations not pending */
            /* Bit 4 : OUTPUT not in transition */
            break;
        } else {
            sal_udelay(usec);
            wait_usec += usec;
        }
    }

    if ((mfr_status & 0x70) != 0x70) {
        LOG_BSL_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "unit %d i2c %s :ltc2974 is busy !\n"),
                 unit, soc_i2c_devname(unit, devno)));
        rv = SOC_E_TIMEOUT;
    }
    return rv;
}


STATIC int
ltc2974_read(int unit, int devno, uint16 addr, uint8* data, uint32* len)
{
    int rv = SOC_E_NONE;
    uint8 saddr;

    saddr = soc_i2c_addr(unit, devno);

    if (*len == 0) {
        return SOC_E_NONE;
    }
    if (*len == 1) {
        /* reads a single byte from a device, from a designated register*/
        rv = soc_i2c_read_byte_data(unit, saddr, addr,data);
        soc_i2c_device(unit, devno)->rbyte++;
         LOG_BSL_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC2974_read: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                    "rv = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *data, *len, rv));
    } else if (*len == 2) {
        /* reads a single word from a device, from a designated register*/
        rv = soc_i2c_read_word_data(unit, saddr, addr,(uint16 *)data);
        soc_i2c_device(unit, devno)->rbyte +=2;
        LOG_BSL_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC2974_read: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                    "rv = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *(uint16 *)data, *len, rv));
    } else {
        /* not supported for now */
         LOG_BSL_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC2974_read fail: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *data, *len));
    }
    return rv;
}

STATIC int
ltc2974_write(int unit, int devno, uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 saddr;
    unsigned short val;

    saddr = soc_i2c_addr(unit, devno);

    if (len == 0) {
        /* simply writes command code to device */
        LOG_BSL_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: LTC2974 write: "
                    "saddr = 0x%x, addr = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno), saddr, addr, len));
        rv = soc_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        LOG_BSL_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: LTC2974 write: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno), saddr, addr, *data, len));
        rv = soc_i2c_write_byte_data(unit, saddr, addr, *data);
        soc_i2c_device(unit, devno)->tbyte++;
    } else if (len == 2) {
         LOG_BSL_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: LTC2974 write: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno),
                    saddr, addr, *(uint16 *)data, len));
        val = *(unsigned short *)data;
        rv = soc_i2c_write_word_data(unit, saddr, addr, val);
        soc_i2c_device(unit, devno)->tbyte += 2;
    }
    return rv;
}

STATIC int
ltc2974_check_page(int unit, int devno, int ch)
{
     int rv;
     uint8 page;
     uint32 len;

     len = sizeof(char);
     rv = ltc2974_read(unit, devno, PMBUS_CMD_PAGE, &page, &len);
     if (rv != SOC_E_NONE) {
         return rv;
     }

     if (page != ch) {
         page = ch;
         LOG_BSL_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "LTC2974 %d set page to %d\n"),
                     soc_i2c_addr(unit, devno), page));
         rv = ltc2974_write(unit, devno, PMBUS_CMD_PAGE, &page, sizeof(char));
     }
     return rv;
 }



/*
 * NOTE NOTE NOTE:
 * All tables (dac_calibrate_t) passed to the ioctl() have size > 1
 * and the index is always within this range.
 */
STATIC int
ltc2974_ioctl(int unit, int devno, int opcode,
              void* data, int len)
{
    int rv = SOC_E_NONE;
#ifdef COMPILER_HAS_DOUBLE
    double fval;
#else
    int fval;
#endif
    uint16 dac;
    uint32 datalen = 2;
    uint8 data8;
    /* Using mutex lock to ensure thread-safe for ioctl operations */
    sal_mutex_take(ioctl_lock, sal_mutex_FOREVER);

     /* length field is actually used as an index into the dac_params table*/
     switch (opcode) {
         case I2C_LTC_IOC_READ_VOUT:
             if ((rv=ltc2974_check_page(unit, devno, len)) < 0) {
                 cli_out("Error: failed to set page %d in LTC2974 device.\n", len);
                 break;
             }
             if ((rv=ltc2974_write(unit, devno,
                             PMBUS_CMD_CLEAR_FAULTS,(void *) &data8, 0)) < 0) {
                 cli_out("Error: Failed to clear the faults of LTC2974 device.\n");
                 break;
             }
             if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                 cli_out("Error: LTC2974 device is busy.\n");
                 break;
             }

            /* Setting Fault response to zero to prevent shut-down of device */
             data8 =0x00;
             if ((rv=ltc2974_write(unit, devno,
                             PMBUS_CMD_VOUT_OV_FAULT_RES, &data8, 1)) < 0) {
                 cli_out("Error: failed to set OV fault response of LTC2974.\n");
                 break;
             }
             if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                 cli_out("Error: LTC2974 device is busy.\n");
                 break;
             }

             /* Switching on LTC2974 device */
             data8 =0x40;
             if ((rv=ltc2974_write(unit, devno, PMBUS_CMD_OPERATION, &data8, 1)) < 0) {
                 cli_out("Error: failed to set operation register of LTC2974 device.\n");
                 break;
             }
             if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                 cli_out("Error: LTC2974 Device is busy.\n");
                 break;
             }
             data8 =0x0A;
             if ((rv=ltc2974_write(unit, devno, PMBUS_CMD_ON_OFF_CONFIG, &data8, 1)) < 0) {
                 cli_out("Error: failed to set CONFIG register of LTC2974 device.\n");
                 break;
             }
             if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                 cli_out("Error: Device LTC2974 is busy.\n");
                 break;
             }
             datalen=2;
             if ((rv=ltc2974_read(unit,devno, PMBUS_CMD_READ_VOUT, (void *)&dac, &datalen)) < 0) {
                 cli_out("Error: Failed to read VOUT of LTC2974 Device.\n");
                 break;
             }
             fval=dac;
#ifdef COMPILER_HAS_DOUBLE
             fval=L16_TO_V(fval );
             *(double *)data=fval;
#else
             fval=L16_TO_UV(fval);
             *(int *)data=(fval);
#endif
             break;

         case I2C_LTC_IOC_READ_IOUT:
             if ((rv=ltc2974_check_page(unit, devno, len)) < 0) {
                 cli_out("Error: failed to set page %d in LTC2974 device.\n", len);
                 break;
             }
             rv=ltc2974_float_to_L11(LTC2974_RES_CONFIG, &dac);
             if ((rv=ltc2974_write(unit, devno,
                             PMBUS_CMD_IOUT_CAL_GAIN, (void *)&dac, 2)) < 0) {
                 cli_out("Error: failed to set IOUT_CAL_GAIN of LTC2974 device.\n");
                 break;
             }
             sal_udelay(500);
             if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                 cli_out("Error: LTC2974 Device is busy.\n");
                 break;
             }
             if ((rv=ltc2974_read(unit, devno,PMBUS_CMD_READ_IOUT, (void *)&dac, &datalen)) < 0) {
                 cli_out("Error:Failed to read current in LTC2974 Device.\n");
                 break;
             }
             rv=ltc2974_L11_to_float(dac, &fval);
#ifdef COMPILER_HAS_DOUBLE
             *(double *)data=fval*1000;
#else
             *(int *)data=fval;
#endif
             break;

         case I2C_LTC_IOC_SET_RCONFIG:
             /* For future use in different RCONFIGs */
             break;

         case I2C_LTC_IOC_READ_POUT:
             datalen=2;
             if ((rv=ltc2974_check_page(unit, devno, len)) < 0) {
                 cli_out("Error: failed to set page %d in LTC2974 device.\n", len);
                 break;
             }
             if ((rv=ltc2974_read(unit,devno, PMBUS_CMD_READ_POUT, (void *)&dac, &datalen)) < 0) {
                 cli_out("Error: failed to read power in LTC2974 device.\n");
                 break;
             }
             /* Switching off LTC2974  */
             data8 = 0x00;
             if ((rv=ltc2974_write(unit, devno, PMBUS_CMD_OPERATION, &data8, 1)) < 0) {
                 cli_out("Error: failed to set operation register of LTC2974 device.\n");
                 break;
             }
             if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
                 cli_out("Error: LTC2974 Device is busy.\n");
                 break;
             }
             rv=ltc2974_L11_to_float(dac, &fval);
#ifdef COMPILER_HAS_DOUBLE
             *(double *)data=fval*1000;
#else
             *(int *)data=fval;
#endif
             break;

         default:
              LOG_BSL_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                 "unit %d i2c %s: ltc2974_ioctl: invalid opcode (%d)\n"),
                 unit, soc_i2c_devname(unit,devno), opcode));
             break;
     }
    sal_mutex_give(ioctl_lock);
    return rv;
}

STATIC int
ltc2974_init(int unit, int devno, void* data, int len)
{
    int rv = SOC_E_NONE;
    if (ioctl_lock == NULL) {
        ioctl_lock = sal_mutex_create("ltc2974_ioctl_lock");
        if (ioctl_lock == NULL) {
            LOG_BSL_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "Fail to create ltc2974_ioctl_lock\n")));
            rv = SOC_E_MEMORY;
        }
    }

    /* Using mutex lock to ensure thread-safe for ioctl operations */
    sal_mutex_take(ioctl_lock, sal_mutex_FOREVER);
    /* bit3 1: control the LTC2974 output through VOUT command
     *      0: control the output via VID input pins which is controlled by
     *         a PCF8574 device
     */
    if ((rv=ltc2974_write(unit, devno,
                    PMBUS_CMD_CLEAR_FAULTS,(void *) &len, 0)) < 0) {
        cli_out("Error: Failed to clear the faults of LTC2974 device.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }
    if ((rv=ltc2974_wait_for_not_busy(unit, devno)) < 0) {
        cli_out("Error: LTC2974 Device is busy.\n");
        sal_mutex_give(ioctl_lock);
        return rv;
    }
    soc_i2c_devdesc_set(unit, devno, "LTC2974 Voltage Control");
     LOG_BSL_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "ltc2974_init: %s, devNo=0x%x\n"),
             soc_i2c_devname(unit,devno), devno));

    sal_mutex_give(ioctl_lock);
    return rv;
}

/* ltc2974 voltage control Chip Driver callout */
i2c_driver_t _soc_i2c_ltc2974_driver = {
    0x0, 0x0, /* System assigned bytes */
    LTC2974_DEVICE_TYPE,
    ltc2974_read,
    ltc2974_write,
    ltc2974_ioctl,
    ltc2974_init,
    NULL,
};
