/*
 * $Id: ltc388x.c,v 1.8 Broadcom SDK $
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
 * I2C Device Driver for LTC388X an integrated power control IC.
 *
 * For new chip with Resistor configurations add new resistor
 * configuration array [ for ex sense_resistor_config_56960k
 * for Tomahawk chip] and initalise with the appropriate index
 * (chip number-G42) in IOCTL opcodes I2C_LTC_IOC_SET_CONFIG and
 * I2C_LTC_IOC_SET_RCONFIG.
*/

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>

/* L16 format conversions */
#define L16_TO_V(val) ((val)*100000/4096/100000)
#define V_TO_L16(val) ((val)*4096)
#define L16_TO_UV(val) ((val)*100000/4096*10)
#define UV_TO_L16(val) ((val)/10*4096/100000)

/* The ranges are defined in unit of one part per 100000 */
#define FAULT_LIMIT_RANGE    10000
#define WARN_LIMIT_RANGE      7500
#define MARGIN_RANGE          5000
#define POWER_GOOD_ON_RANGE   7000
#define POWER_GOOD_OFF_RANGE  8000

#define MAX_VS_CONFIG    4

#define L16_RANGE_UPPER(L16, range) ((L16) + (int)(range)*L16/100000)
#define L16_RANGE_LOWER(L16, range)  ((L16) - (int)(range)*L16/100000)


#define POWER(exponent, input_val)  exponent < 0 ? \
		            (int)((int) (input_val) << (exponent*(-1))): \
			    (int)((int) (input_val*1000000) >> \
                            exponent)/1000000

/* Calibration table in effect */
static dac_calibrate_t* dac_params;
static int dac_param_len;
static sal_mutex_t ioctl_lock = NULL;
static i2c_ltc_t* sense_resistor_config;

/* Sense Resistors Configuration Table for Tomahawk 56960k */
static i2c_ltc_t sense_resistor_config_56960k[MAX_VS_CONFIG]={
    { 0, "1.25",   "ltc3880",   I2C_LTC3880_CH0, LTC3880_1_25V_RES_CONFIG, 0 },
    { 1, "1.8",    "ltc3880",   I2C_LTC3880_CH1, LTC3880_1_8V_RES_CONFIG,  0 },
    { 2, "Analog", "ltc3882-1", I2C_BOTH_CH,     LTC3882_ANLG_RES_CONFIG,  0 },
    { 3, "3.3",    "ltc3882-0", I2C_BOTH_CH,     LTC3882_3_3V_RES_CONFIG,  0 },
};

/*
 * Convert a floating point value into a
 * LinearFloat5_11 formatted word
 */

STATIC int
#ifdef  COMPILER_HAS_DOUBLE
ltc388x_float_to_L11(double input_val, uint16* data)
#else
ltc388x_float_to_L11(int input_val, uint16* data)
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
	mantissa= POWER(exponent, input_val);
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
ltc388x_L11_to_float(uint16 input_val, void *data)
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
                                 (exponent* (-1)))/1000000:
                      (double) (mantissa << exponent);
#else
    *(int *)data= exponent < 0 ?
                   (((mantissa)*1000000) >> (exponent*(-1))):
                   ((mantissa) << exponent)*100000;
#endif
    return SOC_E_NONE;
}

STATIC int
ltc388x_wait_for_not_busy(int unit, int devno)
{
    int rv = SOC_E_NONE;
    uint8 mfr_status, saddr;
    uint32 usec, wait_usec;

    wait_usec = 0;
    usec = 10;

    saddr = soc_i2c_addr(unit, devno);

    while(wait_usec < 1000000) {
        SOC_IF_ERROR_RETURN(soc_i2c_read_byte_data(unit, saddr, PMBUS_CMD_MFR_COMMON,
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
         LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "unit %d i2c %s :ltc388x is busy !\n"),
                 unit, soc_i2c_devname(unit, devno)));
        rv = SOC_E_TIMEOUT;
    }

    return rv;
}

STATIC int
ltc388x_read(int unit, int devno, uint16 addr, uint8* data, uint32* len)
{
    int rv = SOC_E_NONE;
    uint8 saddr;

    saddr = soc_i2c_addr(unit, devno);

    if (*len == 0) {
        return SOC_E_NONE;
    }

    if (*len == 1) {
        /* reads a single byte from a device, from a designated register*/
        rv = soc_i2c_read_byte_data(unit, saddr, addr, data);
        soc_i2c_device(unit, devno)->rbyte++;
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC388X_read: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                    "rv = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *data, *len, rv));
    } else if (*len == 2) {
        /* reads a single word from a device, from a designated register*/
        rv = soc_i2c_read_word_data(unit, saddr, addr,(uint16 *)data);
        soc_i2c_device(unit, devno)->rbyte +=2;
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC388X_read: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d, "
                    "rv = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *(uint16 *)data, *len, rv));
    } else {
        /* not supported for now */
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "unit %d i2c %s: LTC388X_read fail: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    unit, soc_i2c_devname(unit,devno),
                    saddr, addr, *data, *len));
    }

    return rv;
}

STATIC int
ltc388x_write(int unit, int devno, uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 saddr;
    unsigned short val;

    saddr = soc_i2c_addr(unit, devno);

    if (len == 0) {
        /* simply writes command code to device */
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: LTC388X write: "
                    "saddr = 0x%x, addr = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno), saddr, addr, len));
        rv = soc_i2c_write_byte(unit, saddr, addr);
    } else if (len == 1) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: LTC388X write: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno), saddr, addr, *data, len));
        rv = soc_i2c_write_byte_data(unit, saddr, addr,*data);
        soc_i2c_device(unit, devno)->tbyte++;
    } else if (len == 2) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "i2c %s: LTC388X write: "
                    "saddr = 0x%x, addr = 0x%x, data = 0x%x, len = %d\n"),
                    soc_i2c_devname(unit, devno),
                    saddr, addr, *(uint16 *)data, len));
        val = *(unsigned short *)data;
        rv = soc_i2c_write_word_data(unit, saddr, addr, val);
        soc_i2c_device(unit, devno)->tbyte += 2;
    }

    if (addr == PMBUS_CMD_VOUT_COMMAND) {
        /* Make sure the set voltage command is completed internally */
        rv = ltc388x_wait_for_not_busy(unit, devno);
        if (rv == SOC_E_NONE) {
            sal_usleep(120000);
        }
    }
    return rv;
}


STATIC int
ltc388x_check_page(int unit, int devno, int ch)
{
    int rv;
    uint8 page;
    uint32 len;

    len = sizeof(char);
    rv = ltc388x_read(unit, devno, PMBUS_CMD_PAGE, &page, &len);
    if (rv != SOC_E_NONE) {
        return rv;
    }

    if (page != ch) {
        page = ch;
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "LTC388X %d set page to %d\n"),
                    soc_i2c_addr(unit, devno), page));
        rv = ltc388x_write(unit, devno, PMBUS_CMD_PAGE, &page, sizeof(char));
    }

    return rv;
}



STATIC int
ltc388x_setmin_max(int unit, int devno, int len)
{
    int rv=SOC_E_NONE;
    int data,i,ch;
    uint16 dac;

    for(i=0;i<MAX_VS_CONFIG;i++) {
         if (sal_strcmp(dac_params[len].name, sense_resistor_config[i].function) == 0) {
             break;
         }
    }
    if (i == MAX_VS_CONFIG) {
        cli_out("Error: failed to find sense resistor for VDD_%s in  LTC 3880 device.\n",
                   dac_params[len].name);
         return SOC_E_NOT_FOUND;
    }
    if ( sense_resistor_config[i].flag == 0) {
        ch=sense_resistor_config[i].ch;
        SOC_IF_ERROR_RETURN
            (ltc388x_check_page(unit, devno, ch));
        data = dac_params[len].dac_max_hwval;
        dac=(uint16) L16_RANGE_UPPER(data, 10000);
        SOC_IF_ERROR_RETURN
            (ltc388x_write(unit, devno, PMBUS_CMD_VOUT_MAX, (void *)&dac, 2));
        dac=(uint16) L16_RANGE_UPPER(data, 7000);
        SOC_IF_ERROR_RETURN
            ( ltc388x_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            (ltc388x_write(unit, devno, PMBUS_CMD_VOUT_OV_FAULT_LIMIT, (void *)&dac, 2));
        dac=(uint16) L16_RANGE_UPPER(data, 5000);
        SOC_IF_ERROR_RETURN
            ( ltc388x_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            ( ltc388x_write(unit, devno, PMBUS_CMD_VOUT_OV_WARN_LIMIT, (void *)&dac, 2));
        dac=(uint16) L16_RANGE_UPPER(data, 2000);
        SOC_IF_ERROR_RETURN
            ( ltc388x_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            ( ltc388x_write(unit, devno, PMBUS_CMD_VOUT_MARGIN_HIGH, (void *)&dac, 2));

        data=dac_params[len].dac_min_hwval;
        SOC_IF_ERROR_RETURN
            ( ltc388x_wait_for_not_busy(unit, devno));

        dac=(uint16) L16_RANGE_LOWER(data, 5000);
        SOC_IF_ERROR_RETURN
            ( ltc388x_write(unit, devno, PMBUS_CMD_VOUT_UV_FAULT_LIMIT, (void *)&dac, 2));

	if(sal_strcmp("ltc3880", sense_resistor_config[i].devname) == 0) {
	    SOC_IF_ERROR_RETURN
		( ltc388x_write(unit, devno, PMBUS_CMD_POWER_GOOD_ON, (void *)&dac, 2));

	    dac=(uint16) L16_RANGE_LOWER(data, 7000);
	    SOC_IF_ERROR_RETURN
		( ltc388x_write(unit, devno, PMBUS_CMD_POWER_GOOD_OFF, (void *)&dac, 2));
        }
        dac=(uint16) L16_RANGE_LOWER(data, 2000);
        SOC_IF_ERROR_RETURN
            ( ltc388x_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            ( ltc388x_write(unit, devno, PMBUS_CMD_VOUT_UV_WARN_LIMIT, (void *)&dac, 2));

        dac=(uint16) L16_RANGE_LOWER(data, 1000);
        SOC_IF_ERROR_RETURN
            ( ltc388x_wait_for_not_busy(unit, devno));
        SOC_IF_ERROR_RETURN
            ( ltc388x_write(unit, devno, PMBUS_CMD_VOUT_MARGIN_LOW, (void *)&dac, 2));

        rv=ltc388x_float_to_L11(sense_resistor_config[i].res_value, &dac);
        SOC_IF_ERROR_RETURN
            ( ltc388x_write(unit, devno,
                           PMBUS_CMD_IOUT_CAL_GAIN, (void *)&dac, 2));
        sal_udelay(500);

       /* Device Configured */
        sense_resistor_config[i].flag=1;

    }
    return rv;
}

STATIC int
ltc388x_get_ch_voltage(int unit, int devno, int ch, unsigned short *voltage)
{
    int rv;
    uint32 len;

    SOC_IF_ERROR_RETURN(ltc388x_check_page(unit, devno, ch));

    len = sizeof(short);
    rv = ltc388x_read(unit, devno, PMBUS_CMD_READ_VOUT, (uint8*)voltage,
                        &len);

    return rv;
}

STATIC int
ltc388x_set_ch_voltage_upper_with_supervision(int unit, int devno, int ch,
                                                        unsigned short voltage)
{
    unsigned short val;

    val = L16_RANGE_UPPER(voltage, FAULT_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_OV_FAULT_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_UPPER(voltage, WARN_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_OV_WARN_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_UPPER(voltage, MARGIN_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_MARGIN_HIGH,
                                        (uint8*)&val, sizeof(short)));


    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,
                                        (uint8*)&voltage, sizeof(short)));


    val = L16_RANGE_LOWER(voltage, MARGIN_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_MARGIN_LOW,
                                        (uint8*)&val, sizeof(short)));


    val = L16_RANGE_LOWER(voltage, WARN_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_UV_WARN_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_LOWER(voltage, FAULT_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_UV_FAULT_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_LOWER(voltage, POWER_GOOD_ON_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_POWER_GOOD_ON,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_LOWER(voltage, POWER_GOOD_OFF_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_POWER_GOOD_OFF,
                                        (uint8*)&val, sizeof(short)));

    return SOC_E_NONE;
}

STATIC int
ltc388x_set_ch_voltage_lower_with_supervision(int unit, int devno, int ch,
                                                        unsigned short voltage)
{
    unsigned short val;

    val = L16_RANGE_LOWER(voltage, FAULT_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_UV_FAULT_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_LOWER(voltage, WARN_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_UV_WARN_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_LOWER(voltage, MARGIN_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_MARGIN_LOW,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_LOWER(voltage, POWER_GOOD_ON_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_POWER_GOOD_ON,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_LOWER(voltage, POWER_GOOD_OFF_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_POWER_GOOD_OFF,
                                        (uint8*)&val, sizeof(short)));


    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,
                                        (uint8*)&voltage, sizeof(short)));


    val = L16_RANGE_UPPER(voltage, MARGIN_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_MARGIN_HIGH,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_UPPER(voltage, WARN_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_OV_WARN_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    val = L16_RANGE_UPPER(voltage, FAULT_LIMIT_RANGE);
    SOC_IF_ERROR_RETURN(ltc388x_write(unit, devno,
                                        PMBUS_CMD_VOUT_OV_FAULT_LIMIT,
                                        (uint8*)&val, sizeof(short)));

    return SOC_E_NONE;
}

STATIC int
ltc388x_set_ch_voltage(int unit, int devno, int ch, unsigned short voltage)
{
    int rv = SOC_E_NONE;
    unsigned short old_vout;

    SOC_IF_ERROR_RETURN(ltc388x_get_ch_voltage(unit, devno, ch, &old_vout));

    if (voltage >= old_vout) {
        SOC_IF_ERROR_RETURN(ltc388x_set_ch_voltage_upper_with_supervision(unit,
                                                    devno, ch, voltage));
    } else {
        SOC_IF_ERROR_RETURN(ltc388x_set_ch_voltage_lower_with_supervision(unit,
                                                    devno, ch, voltage));
    }


/*
    if (SOC_SUCCESS(rv)) {
        sal_udelay(100000);
    }
*/

    return rv;
}

STATIC int
ltc388x_dac_set_ch_voltage(int unit, int devno, int ch, unsigned short voltage)
{
    int rv;

    rv = ltc388x_set_ch_voltage(unit, devno, ch, voltage);

    if (SOC_SUCCESS(rv)) {
        /* Keep last value since DAC is write-only device */
        dac_params[ch].dac_last_val = voltage;
    }

    return rv;
}

STATIC int
ltc388x_read_current(int unit, int devno, void* data, int ch)
{
    int rv = SOC_E_NONE;
#ifdef  COMPILER_HAS_DOUBLE
    double fval,fval1,fval0;
#else
    int fval,fval1,fval0;
#endif
    uint16 dac,dac0,dac1;
    uint32 datalen;
    soc_i2c_bus_t *i2cbus;
    int i;
    i2cbus = I2CBUS(unit);
    for( i=0; i < MAX_VS_CONFIG; i++ ) {
        if (sal_strcmp(i2cbus->devs[devno]->devname,
                            sense_resistor_config[i].devname) == 0) {
             if (sense_resistor_config[i].ch == ch ) {
                 if (sense_resistor_config[i].flag == 0) {
                     if ((rv=ltc388x_check_page(unit, devno, ch)) < 0) {
                         cli_out("Error: failed to set page %d in unit:%d "
                                 " LTC388X device name: %s.\n",
                                 ch, unit, soc_i2c_devname(unit, devno));
                         return rv;
                     }
                     rv=ltc388x_float_to_L11(sense_resistor_config[i].res_value, &dac);
                     if ((rv=ltc388x_write(unit, devno,
                                     PMBUS_CMD_IOUT_CAL_GAIN, (void *)&dac, 2)) < 0) {
                         cli_out("Error: failed to set sense resistor in unit:%d"
                                 " LTC388X device name: %s.\n",
                                 unit, soc_i2c_devname(unit, devno));
                         return rv;
                     }
                     sal_udelay(500);
                 }
                 break;
             }
        }
    }
    if (i == MAX_VS_CONFIG) {
        cli_out("Error: failed to find sense resistor value for %s device .\n",
                soc_i2c_devname(unit, devno));
        return SOC_E_NOT_FOUND;
    }

    if( ch == 0xFF ) {
        /* LTC3882 in dual mode hence current at both channels
         * are added up to measure totl output current.......*/
        ch=0x00;
        datalen=2;
        if ((rv=ltc388x_check_page(unit, devno, ch)) < 0) {
            cli_out("Error: failed to set page %d in unit: %d LTC388X device name: %s.\n",
                    ch, unit, soc_i2c_devname(unit, devno));
            return rv;
        }
        if ((rv=ltc388x_read(unit, devno, PMBUS_CMD_READ_IOUT, (void *)&dac0, &datalen)) < 0) {
            cli_out("Error: failed to read current in LTC3882 device.\n");
        }
        ch=0x01;
        if ((rv=ltc388x_check_page(unit, devno, ch)) < 0) {
            cli_out("Error: failed to set page %d in unit: %d LTC388X device name: %s.\n",
                    ch, unit, soc_i2c_devname(unit, devno));
            return rv;
        }
        if ((rv=ltc388x_read(unit, devno, PMBUS_CMD_READ_IOUT, (void *)&dac1, &datalen)) < 0) {
            cli_out("Error: failed to read current in unit: %d LTC3882 device name: %s.\n",
                    unit, soc_i2c_devname(unit, devno));
            return rv;
        }
        rv=ltc388x_L11_to_float(dac1, &fval1);
        rv=ltc388x_L11_to_float(dac0, &fval0);
        fval=fval0+fval1;
    } else {
        if ((rv=ltc388x_check_page(unit, devno, ch)) < 0) {
            cli_out("Error: failed to set page %d in unit: %d LTC388X device name: %s.\n",
                    ch, unit, soc_i2c_devname(unit, devno));
            return rv;
        }
        datalen=2;
        if ((rv=ltc388x_read(unit, devno,
                        PMBUS_CMD_READ_IOUT, (void *)&dac, &datalen)) < 0) {
            cli_out("Error: failed to read current in unit: %d LTC3882 device name: %s.\n",
                    unit, soc_i2c_devname(unit, devno));
            return rv;
        }
        rv=ltc388x_L11_to_float(dac, &fval);
    }
#ifdef	COMPILER_HAS_DOUBLE
    *(double *)data=fval*1000;
#else
    *(int *)data=fval;
#endif
    return rv;
}

STATIC int
ltc388x_read_power(int unit, int devno, void* data, int ch)
{
    int rv = SOC_E_NONE;
#ifdef  COMPILER_HAS_DOUBLE
    double fval,fval1,fval0;
#else
    int fval,fval1,fval0;
#endif
    uint16 dac,dac0,dac1;
    uint32 datalen;
    if( ch == 0xFF) {
        /* LTC3882 in dual mode hence current at both channels
           are added up to measure totl output current...........*/
        ch=0x00;
        datalen=2;
        if ((rv=ltc388x_check_page(unit, devno, ch)) < 0) {
            cli_out("Error: failed to set page %d in unit: %d LTC388X device name: %s.\n",
                    ch, unit, soc_i2c_devname(unit, devno));
            return rv;
        }
        if ((rv=ltc388x_read(unit, devno, PMBUS_CMD_READ_POUT, (void *)&dac0, &datalen)) < 0) {
            cli_out("Error: failed to read current in LTC3882 device.\n");
        }
        ch=0x01;
        if ((rv=ltc388x_check_page(unit, devno, ch)) < 0) {
            cli_out("Error: failed to set page %d in unit: %d LTC388X device name: %s.\n",
                    ch, unit, soc_i2c_devname(unit, devno));
            return rv;
        }
        if ((rv=ltc388x_read(unit, devno, PMBUS_CMD_READ_POUT, (void *)&dac1, &datalen)) < 0) {
            cli_out("Error: failed to read power in unit: %d LTC3882 device name: %s.\n",
                    unit, soc_i2c_devname(unit, devno));
            return rv;
        }
        rv=ltc388x_L11_to_float(dac1,&fval1);
        rv=ltc388x_L11_to_float(dac0,&fval0);
        fval=fval0+fval1;
    } else {
        if ((rv=ltc388x_check_page(unit, devno, ch)) < 0) {
            cli_out("Error: failed to set page %d in unit: %d LTC388X device name: %s.\n",
                    ch, unit, soc_i2c_devname(unit, devno));
            return rv;
        }
        datalen=2;
        if ((rv=ltc388x_read(unit, devno,
                        PMBUS_CMD_READ_POUT, (void *)&dac, &datalen)) < 0) {
            cli_out("Error: failed to read power in unit: %d LTC3882 device name: %s.\n",
                    unit, soc_i2c_devname(unit, devno));
            return rv;
        }
        rv=ltc388x_L11_to_float(dac,&fval);
    }
#ifdef	COMPILER_HAS_DOUBLE
        *(double *)data=fval*1000;
#else
        *(int *)data=fval;
#endif
        return rv;
}


/*
 * NOTE NOTE NOTE:
 * All tables (dac_calibrate_t) passed to the ioctl() have size > 1
 * and the index is always within this range.
 */
STATIC int
ltc388x_ioctl(int unit, int devno, int opcode, void* data, int len)
{
    int rv = SOC_E_NONE;
#ifdef	COMPILER_HAS_DOUBLE
    double fval=0;
#else
    int fval=0;
#endif
    int uV,ch,i;
    uint16 dac,time_ms;
    unsigned short voltage;
    uint32 datalen;
    dac_calibrate_t* params = NULL;
    /* Using mutex lock to ensure thread-safe for ioctl operations */
    sal_mutex_take(ioctl_lock, sal_mutex_FOREVER);
    /* length field is actually used as an index into the dac_params table*/
    if( !data || ( (dac_params != NULL) && ((len > dac_param_len) && (len != I2C_BOTH_CH))))
        return SOC_E_PARAM;

    switch ( opcode ){
	/* Upload calibration table */
    case I2C_ADC_QUERY_CHANNEL :
            ch = len;
            rv = ltc388x_get_ch_voltage(unit, devno, ch, &voltage);
            fval = voltage;
#ifdef COMPILER_HAS_DOUBLE
            fval = L16_TO_V(fval);
#else
            fval = L16_TO_UV(fval);
#endif
            ((i2c_adc_t*)data)->max =
            ((i2c_adc_t*)data)->min =
            ((i2c_adc_t*)data)->val = fval;
            ((i2c_adc_t*)data)->delta = 0;
            ((i2c_adc_t*)data)->nsamples = 1;
        break;

    case I2C_DAC_IOC_SET_CALTAB:
	params = (dac_calibrate_t*)data;
	/* dac_params  initialised */
	dac_params = params;
	dac_param_len = len;
	break;

    case I2C_DAC_IOC_SETDAC_MIN:
        /* Set MIN voltage */
        if (dac_params) {
            rv = ltc388x_dac_set_ch_voltage(unit, devno, len,
                    dac_params[len].dac_min_hwval);
        }
        break;

    case I2C_DAC_IOC_SETDAC_MAX:
        /* Set MAX voltage */
        if (dac_params) {
            rv = ltc388x_dac_set_ch_voltage(unit, devno, len,
                    dac_params[len].dac_max_hwval);
        }
        break;
    
    case I2C_DAC_IOC_SETDAC_MID:
        /* Set mid-range voltage */
        if (dac_params) {
            rv = ltc388x_dac_set_ch_voltage(unit, devno, len,
                    dac_params[len].dac_min_hwval);
        }
        break;

    case I2C_DAC_IOC_CALIBRATE_MAX:
        /* Set MAX output value (from ADC) */
        if (dac_params) {
#ifdef COMPILER_HAS_DOUBLE
            fval = *((double*)data);
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "CALIBRATE_MAX setting %f\n"), fval));
#else
            fval = *((int *)data);
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "CALIBRATE_MAX setting %d\n"), fval));
#endif
            dac_params[len].max = fval;
        }
        break;

    case I2C_DAC_IOC_CALIBRATE_MIN:
        /* Set MIN output value (from ADC) */
        if (dac_params) {
#ifdef COMPILER_HAS_DOUBLE
            fval = *((double*)data);
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "CALIBRATE_MIN setting %f\n"), fval));
#else
            fval = *((int *)data);
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "CALIBRATE_MIN setting %d\n"), fval));
#endif
            dac_params[len].min = fval;
        }
        break;

    case I2C_DAC_IOC_CALIBRATE_STEP:
        /* Calibrate stepsize */
        if (dac_params) {
            dac_params[len].step =
                (dac_params[len].use_max ? -1 : 1) *
                (dac_params[len].max - dac_params[len].min) /
                (dac_params[len].dac_max_hwval - dac_params[len].dac_min_hwval);
#ifdef COMPILER_HAS_DOUBLE
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC388X calibration on function %s:"
                        "(max=%f,min=%f,step=%f)\n"),
                        unit, soc_i2c_devname(unit,devno),
                        dac_params[len].name,
                        dac_params[len].max,
                        dac_params[len].min,
                        dac_params[len].step));
#else
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC388X calibration on function %s:"
                        "(max=%d,min=%d,step=%d)\n"),
                        unit, soc_i2c_devname(unit,devno),
                        dac_params[len].name,
                        dac_params[len].max,
                        dac_params[len].min,
                        dac_params[len].step));
#endif
        }
        break;
    case I2C_DAC_IOC_SET_VOUT:
        /* Set output voltage */
        if (dac_params) {
#ifdef COMPILER_HAS_DOUBLE
            fval = *((double*)data); /* in V */
            voltage = V_TO_L16(fval);
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC388x ioctl I2C_DAC_IOC_SET_VOUT: "
                        "voltage = %d, len = %d\n"),
                        unit, soc_i2c_devname(unit,devno), voltage, len));
#else
            fval = *((int *)data); /* in uV */
            voltage = UV_TO_L16(fval);
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC388X ioctl I2C_DAC_IOC_SET_VOUT: "
                        "voltage = %d, len = %d\n"),
                        unit, soc_i2c_devname(unit,devno), voltage, len));
#endif
            if ((voltage > dac_params[len].dac_max_hwval)  ||
                    (voltage < dac_params[len].dac_min_hwval)) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                "unit %d i2c %d: LTC388X given voltage %d "
                                "beyond range for ch %d, %d, %d\n"),
                            unit, devno, voltage, len,
                            dac_params[len].dac_max_hwval,
                            dac_params[len].dac_min_hwval));
                rv=SOC_E_PARAM;
                break;
            }
            rv = ltc388x_dac_set_ch_voltage(unit, devno, len, voltage);
        }
       break;

    case I2C_REGULATOR_IOC_VOLT_GET:
        rv = ltc388x_get_ch_voltage(unit, devno, len, &voltage);
        if (rv == SOC_E_NONE) {
            *((int *)data) = L16_TO_UV(voltage);
        }
        break;

    case I2C_REGULATOR_IOC_VOLT_SET:
        uV = *(int *)data;
        voltage = UV_TO_L16(uV);
        rv = ltc388x_set_ch_voltage(unit, devno, len, voltage);
        break;

    case I2C_LTC_IOC_SET_CONFIG:
        /* Initialising sense resistor */
        i = *(int *)data;
        
        if (i == 0) {
            /* Initializing with Tomahawk chip resistor configuration */
            sense_resistor_config = sense_resistor_config_56960k;
        } else {
             cli_out("Error: Invalid chip: %d for LTC388X device %s .\n",
                    i, soc_i2c_devname(unit, devno));
             rv=SOC_E_NOT_FOUND;
             break;
        }
	if (dac_params) {
	    /* Setting max and min values for dac */
	    rv= ltc388x_setmin_max(unit, devno, len);
	}
        break;
   case I2C_LTC_IOC_SET_RCONFIG:
        i = len;
        
        if (i == 0) {
            /* Initializing with Tomahawk chip resistor configuration */
            sense_resistor_config = sense_resistor_config_56960k;
        } else {
            cli_out("Error: Invalid chip: %d for LTC388X device %s .\n",
                    i, soc_i2c_devname(unit, devno));
            rv=SOC_E_NOT_FOUND;
            break;
        }
        break;
   case I2C_LTC_IOC_NOMINAL:
	if ((rv=ltc388x_wait_for_not_busy(unit, devno)) < 0) {
            cli_out("Error: unit: %d  LTC388X device name %s Device is busy.\n",
                    unit, soc_i2c_devname(unit, devno));
            break;
        }
        ch=*((int *)data);
        if ((rv=ltc388x_check_page(unit, devno, ch)) < 0) {
            cli_out("Error: failed to set page %d in unit: %d LTC388X device name: %s.\n",
                        ch, unit, soc_i2c_devname(unit, devno));
            break;
        }
        if (dac_params) {
            dac= (uint16) dac_params[len].dac_mid_hwval;
            rv = ltc388x_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,(
                                    void *)&dac, 2);
            /* Keep last value since DAC is write-only device */
            dac_params[len].dac_last_val = dac;
        }
        break;

    case I2C_LTC_IOC_READ_VOUT:
        if ((rv=ltc388x_check_page(unit, devno, len)) < 0) {
            cli_out("Error: failed to set page %d in unit: %d LTC388X device name: %s.\n",
                        len, unit, soc_i2c_devname(unit, devno));
            break;
        }
        datalen=2;
        if ((rv=ltc388x_read(unit, devno,
                        PMBUS_CMD_READ_VOUT, (void *)&dac, &datalen)) < 0) {
            cli_out("Error: Failed to read voltage in unit:%d  LTC388x Device name:%s.\n",
                    unit, soc_i2c_devname(unit, devno));
            break;
        }
        fval=dac;

#ifdef	COMPILER_HAS_DOUBLE
        fval=L16_TO_V(fval);
        *(double *)data=fval;
#else
        fval=L16_TO_UV(fval );
        *(int *)data=(fval);
#endif
        break;
    case I2C_LTC_IOC_READ_IOUT:
        rv=ltc388x_read_current(unit, devno, &fval, len);
#ifdef	COMPILER_HAS_DOUBLE
        *(double *)data=fval;
#else
        *(int *)data=fval;
#endif
        break;

    case I2C_LTC_IOC_READ_POUT:
        rv=ltc388x_read_power(unit, devno, &fval, len);
#ifdef	COMPILER_HAS_DOUBLE
        *(double *)data=fval;
#else
        *(int *)data=fval;
#endif
        break;

    case I2C_LTC_IOC_SET_VOUT:
        /* Finding and  setting page numbers */
        if (dac_params) {
            for(i=0;i<MAX_VS_CONFIG;i++) {
                if(sal_strcmp(dac_params[len].name, 
                            sense_resistor_config[i].function) == 0 ) {
                    break;
                }
            }
            if (i == MAX_VS_CONFIG) {
                cli_out("Error: failed to find page number for VDD_%s in sense "
                        "resistor table.\n", dac_params[len].name);
                rv=SOC_E_NOT_FOUND;
                break;
            }
            ch = sense_resistor_config[i].ch;
            if ((rv=ltc388x_check_page(unit, devno, ch)) < 0) {
                cli_out("Error: failed to set page %d in "
                        "LTC388X device.\n", ch);
                break;
            }
            /* Conversion of output voltage */
#ifdef	COMPILER_HAS_DOUBLE
            fval = *((double*)data);
            voltage = (fval*4096);
#else
            fval = *((int*)data);
            /*2^12 conversion and changing from uVolt */
            voltage = (fval*4096)/1000000;
#endif
            if ((voltage < dac_params[len].dac_min_hwval)||
                    (voltage > dac_params[len].dac_max_hwval)) {
#ifdef  COMPILER_HAS_DOUBLE
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                "unit %d i2c %d: LTC388X given voltage %2.3f V "
                                "beyond range( max=%2.3f V, min=%2.3f V) for "
                                "voltage VDD_%s \n"), unit, devno, 
                            (double)voltage/4096, 
                            (double) dac_params[len].dac_max_hwval/4096,
                            (double) dac_params[len].dac_min_hwval/4096, 
                            dac_params[len].name));
#else
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                "unit %d i2c %d: LTC388X given voltage %d mV "
                                "beyond range( max=%d mV, min=%d mV ) for " 
                                "voltage  VDD_%s \n"), unit, devno, 
                            voltage*1000/4096, 
                            dac_params[len].dac_max_hwval*1000/4096,
                            dac_params[len].dac_min_hwval*1000/4096,
                            dac_params[len].name));

#endif
                rv=SOC_E_PARAM;
                break;
            }
            dac = voltage;
            /* Show what we are doing, for now ... */
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "unit %d i2c %s: LTC388X ioctl I2C_DAC_IOC_SET_VOUT "
                            ": voltage = %d, len = %d\n"),
                            unit, soc_i2c_devname(unit,devno), voltage, len));

            rv = ltc388x_write(unit, devno, PMBUS_CMD_VOUT_COMMAND,
                                (void *) &dac, 2);
            /* Keep last value since DAC is write-only device */
            dac_params[len].dac_last_val = dac;
        }
        break;

    case I2C_SET_SEQ:
        if ((rv=ltc388x_check_page(unit, devno, len)) < 0) {
            cli_out("Error: failed to set page %d in unit: %d LTC388X device name: %s.\n",
                        len, unit, soc_i2c_devname(unit, devno));
            break;
        }
#ifdef  COMPILER_HAS_DOUBLE
        fval = *((double*)data);
        rv=ltc388x_float_to_L11(fval, &time_ms);
#else
        fval = *((int*)data);
        rv=ltc388x_float_to_L11(fval/1000000, &time_ms);
#endif
	if ((rv=ltc388x_write(unit, devno, PMBUS_CMD_POWER_ON_DELAY,(void *) &time_ms, 2)) < 0) {
             cli_out("Error: failed to set TON_DELAY for %s.\n",
                     soc_i2c_devname(unit, devno));
             break;
        }
	if ((rv=ltc388x_wait_for_not_busy(unit, devno)) < 0) {
             cli_out("Error: unit: %d  LTC388X device name %s Device is busy.\n",
                     unit, soc_i2c_devname(unit, devno));
             break;
         }
        dac=0;
	if ((rv=ltc388x_write(unit, devno, PMBUS_CMD_STORE_USER_ALL, (void *)&dac, 0)) < 0) {
            cli_out("Error: Failed to store the TON_DELAY value to NVRAM in device: %s.\n",
                    soc_i2c_devname(unit, devno));
            break;
        }
        if ((rv=ltc388x_wait_for_not_busy(unit, devno)) < 0) {
             cli_out("Error: unit: %d  LTC388X device name %s Device is busy.\n",
                     unit, soc_i2c_devname(unit, devno));
             break;
         }
         break;

    case I2C_READ_SEQ:
         if ((rv=ltc388x_check_page(unit, devno, len)) < 0) {
             cli_out("Error: failed to set page %d in unit: %d LTC388X device name: %s.\n",
                     len, unit, soc_i2c_devname(unit, devno));
             break;
         }
         datalen=2;
         if ((rv=ltc388x_read(unit, devno, PMBUS_CMD_POWER_ON_DELAY,(void *) &time_ms, &datalen)) < 0) {
             cli_out("Error: failed to read TON_DELAY for %s.\n",
                     soc_i2c_devname(unit, devno));
             break;
         }
         rv=ltc388x_L11_to_float(time_ms, &fval);
#ifdef  COMPILER_HAS_DOUBLE
        *(double *)data=fval;
#else
        *(int *)data=(fval);
#endif
        break;

    default:
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "unit %d i2c %s: LTC388X_ioctl: invalid opcode (%d)\n"),
                    unit, soc_i2c_devname(unit,devno), opcode));
        break;
    }
    sal_mutex_give(ioctl_lock);
    return rv;
}

STATIC int
ltc388x_init(int unit, int devno,
        void* data, int len)
{
    int rv = SOC_E_NONE;
    uint16 dac;
    if (ioctl_lock == NULL) {
        ioctl_lock = sal_mutex_create("ltc388x_ioctl_lock");
        if (ioctl_lock == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "Fail to create ltc388x_ioctl_lock\n")));
            rv = SOC_E_MEMORY;
        }
    }
    sal_mutex_take(ioctl_lock, sal_mutex_FOREVER);
    if ((rv=ltc388x_write(unit, devno,
                    PMBUS_CMD_CLEAR_FAULTS, (void *)&dac, 0)) < 0) {
        cli_out("Error: Failed to clear fault in unit:%d  LTC388X Device name:%s.\n",
                unit, soc_i2c_devname(unit, devno));
        sal_mutex_give(ioctl_lock);
        return rv;
    }
    if ((rv=ltc388x_wait_for_not_busy(unit, devno)) < 0) {
            cli_out("Error: unit: %d  LTC388X device name %s Device is busy.\n",
                    unit, soc_i2c_devname(unit, devno));
            return rv;
    }
    dac_params = NULL;

    soc_i2c_devdesc_set(unit, devno, "Linear Tech LTC388X Step_Down DC/DC "
            "Controller");

    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "ltc388x_init: %s, devNo=0x%x\n"),
            soc_i2c_devname(unit,devno), devno));
    sal_mutex_give(ioctl_lock);
    return rv;
}


/* ltc 3880/3882 voltage control Chip Driver callout */
i2c_driver_t _soc_i2c_ltc388x_driver = {
    0x0, 0x0, /* System assigned bytes */
    LTC388X_DEVICE_TYPE,
    ltc388x_read,
    ltc388x_write,
    ltc388x_ioctl,
    ltc388x_init,
};
