/* AAPL CORE Revision: master
 *
 * Copyright (c) 2014-2021 Avago Technologies. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** @file   sensor.c
 ** @brief  Functions for accessing Avago temperature and voltage sensors.
 ** @defgroup Sensor Sensor API for Measuring Temperature and Voltage.
 ** @{
 **/


#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"

#define AAPL_LOG_PRINT5 if(aapl->debug >= 5) aapl_log_printf

#define ALLOW_USING_SBM_TEMPERATURE_READING 1

#if AAPL_ENABLE_07NM_IP || AAPL_ENABLE_05NM_IP

static void avago_apb_start_read(Aapl_t *aapl, uint addr, int reg)
{
    if( (reg & ~0xffff) == 0 )
    {
        if( addr == 0x01 || addr == 0x0101 )
        {
            uint jtag_idcode = aapl_get_jtag_idcode(aapl, addr);
            if( jtag_idcode == 0x0a17657f ||
                jtag_idcode == 0x1a08457f ||
                jtag_idcode == 0x2a08457f   )
            {
                avago_sbus_wr(aapl, addr, 11, reg | (0<<16));
            }
        }

        avago_sbus_wr(aapl, addr, 11, reg | (1<<16));
    }
    else
    {
        avago_sbus_wr(aapl, addr, 10, reg);
        avago_sbus_wr(aapl, addr,  6, 1);
    }
}

static int avago_apb_read16(Aapl_t *aapl, uint addr, int reg)
{
    int i;
    avago_apb_start_read(aapl, addr, reg);

    for( i = 0; i < 10; i++ )
    {
        int data = avago_sbus_rd(aapl, addr, 21);
        if( (data & 7) == 0 )
            return data >> 16;
        ms_sleep(1);
    }
    return -1;
}

#if 0
static int avago_apb_read32(Aapl_t *aapl, uint addr, int reg)
{
    avago_apb_start_read(aapl, addr, reg);

    for(;;)
    {
        int busy = avago_sbus_rd(aapl, addr, 2);
        if( (busy & 7) == 0 )
            return avago_sbus_rd(aapl, addr, 20);
    }
}

static int avago_apb_write(Aapl_t *aapl, uint addr, int reg, int data)
{
    int busy;
    avago_sbus_wr(aapl, addr, 12, data);
    if( (reg & ~0xffff) == 0 )
    {
#if 1
        avago_sbus_wr(aapl, addr, 11, reg | (0<<17));
        avago_sbus_wr(aapl, addr, 11, reg | (1<<17));
#else
        avago_sbus_wr(aapl, addr, 11, reg | (1<<17));
#endif
        while( ((busy = avago_sbus_rd(aapl, addr, 21)) & 7) != 0 )
            printf("busy = 0x%x\n", busy);
    }
    else
    {
        avago_sbus_wr(aapl, addr, 10, reg);
        avago_sbus_wr(aapl, addr,  6, 2);
        while( ((busy = avago_sbus_rd(aapl, addr,  2)) & 7) != 0 )
            printf("busy = 0x%x\n", busy);
    }
    return 0;
}
#endif

#endif

/** @brief   Checks if the sensor is in reset.
 ** @details Checks if the sensor is in reset and therefore needs to be
 **          initialized by passing a non-zero frequency to one of the
 **          avago_sensor_get_* or avago_sensor_start_* functions.
 ** @details Use this function to avoid resetting an already configured sensor.
 ** @see avago_sensor_get_temperature(), avago_sensor_get_voltage(),
 ** @see avago_sensor_start_temperature(), avago_sensor_start_voltage().
 **/
BOOL avago_sensor_is_in_reset(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Sensor sbus address. */
{
#if AAPL_ENABLE_07NM_IP || AAPL_ENABLE_05NM_IP
    if( aapl_get_ip_type(aapl, addr) == AVAGO_VTMON )
    {
#if AAPL_ENABLE_07NM_IP
        BOOL is_beaver_creek_1 = aapl_get_jtag_idcode(aapl, addr) == 0x1a08457f;
        int sensor_active = is_beaver_creek_1 ? 0x10 : 0x00;
        int resets = avago_sbus_rd(aapl, addr, 0);
        return (resets & 0x1f) != sensor_active;
#else
        int resets = avago_sbus_rd(aapl, addr, 0);
        return (resets & 0x1f) != 0x00;
#endif
    }
#endif
#if AAPL_ENABLE_16NM_IP || AAPL_ENABLE_28NM_IP
    if( aapl_get_ip_type(aapl, addr) == AVAGO_THERMAL_SENSOR )
    {
        int current_reset = avago_sbus_rd(aapl, addr, 0);
        return (current_reset & 0x1) == 1;
    }
#endif
    return FALSE;
}

/** @brief Prepare sensor for measurement */
static void initialize_sensor(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Sensor sbus address. */
    uint frequency) /**< [in] Sensor clock input frequency in Hertz. */
                    /**<      If non-zero, configures the sensor, */
                    /**<      which only need be done once. */
                    /**< For the 7nm sensor, if non-zero, this should be the sbus clock input frequency before being divided. */
{
#if AAPL_ENABLE_07NM_IP || AAPL_ENABLE_05NM_IP
    if( aapl_get_ip_type(aapl, addr) == AVAGO_VTMON )
    {
        if( frequency > 0 && (avago_sbus_rd(aapl, addr, 253) & 0xf) == 0 )
        {
            int sbus_divider = avago_sbm_get_sbus_clock_divider(aapl, addr);
            uint adc_divider = (frequency / sbus_divider + 500000) / 1000000;
            uint pclk_divider = adc_divider / 20;

            if( pclk_divider == 0 ) pclk_divider = 1;
            if( adc_divider == 0 ) adc_divider = 1;

            aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "in freq = %u kHz, sbus_freq = %u kHz, adc div=%u freq=%u kHz, pclk div=%u freq=%u kHz\n",
                    frequency/1000, frequency/sbus_divider/1000, adc_divider, frequency/sbus_divider/adc_divider/1000, pclk_divider, frequency/sbus_divider/pclk_divider/1000);

            if( !avago_sensor_is_in_reset(aapl, addr) )
            {
                if( avago_sbus_rd(aapl, addr, 30) == adc_divider &&
                    avago_sbus_rd(aapl, addr, 31) == pclk_divider &&
                    avago_sbus_rd(aapl, addr, 32) == 0 )
                return;
            }

            avago_sbus_wr(aapl, addr, 30, adc_divider);
            avago_sbus_wr(aapl, addr, 31, pclk_divider);
            avago_sbus_wr(aapl, addr, 32, 0);

#if AAPL_ENABLE_07NM_IP
            if( aapl_get_jtag_idcode(aapl, addr) == 0x1a08457f )
            {
                avago_sbus_wr(aapl, addr, 0, 0x0f);
                avago_sbus_wr(aapl, addr, 0, 0x10);
            }
            else
#endif
            {
                avago_sbus_wr(aapl, addr, 0, 0x1f);
                avago_sbus_wr(aapl, addr, 0, 0x19);
                avago_sbus_wr(aapl, addr, 0, 0x00);
            }
        }
        return;
    }
#endif

#if AAPL_ENABLE_16NM_IP || AAPL_ENABLE_28NM_IP
    if( frequency > 0 && aapl_get_ip_type(aapl, addr) == AVAGO_THERMAL_SENSOR )
    {
        uint divider = (frequency + 1000000) / 2000000;

        if( !avago_sensor_is_in_reset(aapl, addr) )
        {
            int current_divider = avago_sbus_rd(aapl, addr, 1);
            if( (current_divider & 0x3ff) == (divider & 0x3ff) )
                return;
        }

        avago_sbus_wr(aapl, addr, 0x00, 0x01);
        avago_sbus_wr(aapl, addr, 0x01, divider & 0x03FF);

        if( aapl_get_process_id(aapl, addr) == AVAGO_PROCESS_B )
        {
            avago_sbus_wr(aapl, addr, 0x06, 0x50c1);
            avago_sbus_wr(aapl, addr, 0x07, 0xb529);
            avago_sbus_wr(aapl, addr, 0x08, 0x1c);
            avago_sbus_wr(aapl, addr, 0x0b, 0x07);
            avago_sbus_wr(aapl, addr, 0x0c, 0x5e70);
            avago_sbus_wr(aapl, addr, 0x0e, 0x2a);
            avago_sbus_wr(aapl, addr, 0x10, 0x5);
            /*//avago_sbus_wr(aapl, addr, 0x13, 0x0); */
            /*//avago_sbus_wr(aapl, addr, 0x14, 0x5); */
        }
    }
#endif
}

static BOOL use_sbm_for_temperature_reading(Aapl_t *aapl, uint addr)
{
#if ALLOW_USING_SBM_TEMPERATURE_READING
    uint sbm_addr = avago_make_sbus_master_addr(addr);
    if( !aapl_get_spico_running_flag(aapl, sbm_addr) )
        aapl_set_spico_running_flag(aapl,sbm_addr, avago_spico_running(aapl, sbm_addr));
    if( aapl_get_spico_running_flag(aapl, sbm_addr) )
    {
        uint fw_rev = aapl_get_firmware_rev(aapl, sbm_addr);
        return (fw_rev & 0xff) >= 0x20;
    }
    return FALSE;
#else
    (void)aapl;
    (void)addr;
    return FALSE;
#endif
}

/** @brief   Starts a temperature measurement.
 ** @details Use this function to start measurements on multiple devices.
 ** @details Note that in 16 and 28 nm, each device (sensor sbus address) can perform only
 **          one measurement (voltage or temperature) at a time.
 **
 **          In 7nm, sensor measurements need be started and initialized only once.
 **          Thereafter, up-to-date values can be read at will.
 ** @see avago_sensor_wait_temperature(), avago_sensor_get_temperature().
 **/
void avago_sensor_start_temperature(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Sensor sbus address.  Chip/ring broadcast OK. */
    int sensor,     /**< [in] Which sensor to access. A value of 0 selects */
                    /**<      the main sensor. Values of 1 through 8 */
                    /**<      select remote sensors 0 through 7. */
    uint frequency) /**< [in] Sensor clock input frequency in Hertz. */
                    /**<      If non-zero, configures the sensor, */
                    /**<      which only need be done once. */
{
    if( use_sbm_for_temperature_reading(aapl, addr) )
        return;

    initialize_sensor(aapl, addr, frequency);

#if AAPL_ENABLE_07NM_IP || AAPL_ENABLE_05NM_IP
    if( aapl_get_ip_type(aapl, addr) == AVAGO_VTMON )
        return;
#endif

    if( sensor > 8 ) sensor = 0;

    if( avago_sbus_rd(aapl, addr, 0x03) != (1U<<sensor) ||
        avago_sbus_rd(aapl, addr, 0x00) != 2 )
    {
        avago_sbus_wr(aapl, addr, 0x00, 0x01);
        avago_sbus_wr(aapl, addr, 0x03, 0x01 << sensor );
        avago_sbus_wr(aapl, addr, 0x00, 0x02);
    }
}

/** @cond INTERNAL */

#if AAPL_ENABLE_07NM_IP || AAPL_ENABLE_O5NM_IP
/** @brief Convert raw sensor data to mC value.
 ** @details Only valid for 7nm designs.
 ** @return  Returns temperature in millidegrees C.  Valid range is [-40,000..120,000] (-40C to +120C).
 **          Values outside of this range indicate an error (perhaps data from an unconnected sensor).
 **/
int avago_sensor_to_mC(Aapl_t *aapl, uint addr, uint data)
{
#if AAPL_ENABLE_05NM_IP
    if( aapl_get_process_id(aapl, addr) == AVAGO_TSMC_05 )
    {
        return ((-25325 * (0x7ff & (int)data) + (35671000 + 12500)) / 25000) * 250;
    }
#endif
#if AAPL_ENABLE_07NM_IP
    if( aapl_get_jtag_idcode(aapl, addr) == 0x1a08457f )
    {
        return ((-2395 * (0x7ff & (int)data) + (3575500 + 1250)) / 2500) * 250;
    }
    else
    {
        return ((-2454 * (0x7ff & (int)data) + (3668100 + 1250)) / 2500) * 250;
    }
#endif
}
#endif

/** @endcond */

/** @brief   Performs a blocking read on a temperature sensor.
 ** @details The sensor must already be properly initialized.
 ** @return  On success, returns the temperature in milli-degrees C.
 ** @return  On error, decrements aapl->return_code and returns -1000000.
 ** @see avago_sensor_start_temperature(), avago_sensor_get_temperature().
 **/
int avago_sensor_wait_temperature(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Sensor sbus address. */
    int sensor)     /**< [in] Which sensor to access. A value of 0 selects */
                    /**<      the main sensor. Values of 1 through 8 */
                    /**<      select remote sensors 0 through 7. */
{
    int return_code = aapl->return_code;
    int temp = -1000000;
    int data = 0;
    if( use_sbm_for_temperature_reading(aapl, addr) )
    {
        uint sbm_addr = avago_make_sbus_master_addr(addr);
        data = avago_spico_int(aapl, sbm_addr, 0x17, ((sensor & 0xf) << 12) | (addr & 0xff));
        if( data & 0x8000 )
        {
            if( data & 0x800 )
                temp = data | ~0x7ff;
            else
                temp = data & 0x7ff;
            temp *= 125;
        }
    }
    else
#if AAPL_ENABLE_07NM_IP || AAPL_ENABLE_05NM_IP
    if( aapl_get_ip_type(aapl, addr) == AVAGO_VTMON )
    {

        int sensor_reg = sensor == 0 ? 7 : sensor + 8;
        int i;
        for( i = 0; i < 10; i++ )
        {
            data = avago_apb_read16(aapl, addr, sensor_reg);
            if( data & 0x800 ) break;
            if( i > 6 )
                ms_sleep(1);
        }
        temp = avago_sensor_to_mC(aapl, addr, data);
    }
    else
#endif
    {
#if AAPL_ENABLE_16NM_IP || AAPL_ENABLE_28NM_IP
        int i;
        for( i = 0; i < 16; i++ )
        {
            if( i > 0 ) ms_sleep(5);
            data = avago_sbus_rd(aapl, addr, 65 + sensor);
            if( data & 0x8000 )
            {
                if( data & 0x800 )
                    temp = data | ~0x7ff;
                else
                    temp = data & 0x7ff;
                temp *= 125;
                break;
            }
        }
#endif
    }

    if( temp < -40000 || temp > 125000 || aapl->return_code != return_code )
    {
        aapl_fail(aapl, __func__, __LINE__, "SBus 0x%02x, Temperature conversion failed (temp = %d milliC), returning -1000000.\n",addr,temp);
        temp = -1000000;
    }
    else
        AAPL_LOG_PRINT5(aapl, AVAGO_DEBUG5, __func__, __LINE__, "Temp = %7d mC (data=0x%x)\n", temp, data);

    return temp;
}


/** @brief   Performs a temperature measurement.
 ** @details Combines the start and wait functions into one blocking operation.
 ** @return  On success, returns the temperature in milli-degrees C.
 ** @return  On error, decrements aapl->return_code and returns -1000000.
 ** @see     avago_sensor_start_temperature(), avago_sensor_wait_temperature().
 ** @see     avago_sensor_get_voltage().
 **/
int avago_sensor_get_temperature(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Sensor sbus address. */
    int sensor,     /**< [in] Which sensor to access. A value of 0 selects */
                    /**<      the main sensor. Values of 1 through 8 */
                    /**<      select remote sensors 0 through 7. */
    uint frequency) /**< [in] Sensor clock input frequency in Hertz. */
                    /**<      If non-zero, configures the sensor, */
                    /**<      which only need be done once. */
{
    int temp;
    AAPL_SENSOR_LOCK;
    avago_sensor_start_temperature(aapl, addr, sensor, frequency);
    temp = avago_sensor_wait_temperature(aapl, addr, sensor);
    AAPL_SENSOR_UNLOCK;
    return temp;
}


/** @brief   Starts a voltage measurement.
 ** @details Use this function to start measurements on multiple devices.
 ** @details Note that in 16 and 28 nm, each device (sensor sbus address) can perform only
 **          one measurement (voltage or temperature) at a time.
 **
 **          In 7nm, sensor measurements need be started and initialized only once.
 **          Thereafter, up-to-date values can be read at will.
 ** @see     avago_sensor_wait_voltage(), avago_sensor_get_voltage().
 **/
void avago_sensor_start_voltage(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Sensor sbus address.  Chip/ring broadcast OK. */
    int sensor,     /**< [in] Which sensor to access.  A value of 0 selects */
                    /**<      the VDD voltage, 1 selects CORE_VDD_VIN, and */
                    /**<      values [2..7] select SENSOR_A2D_VIN[0..5] */
                    /**< The 7nm block has remote sensors [0..7] without special designations. */
    uint frequency) /**< [in] Sensor clock input frequency in Hertz. */
                    /**<      If non-zero, configures the sensor, */
                    /**<      which only need be done once. */
{
    if( use_sbm_for_temperature_reading(aapl, addr) )
        return;

    initialize_sensor(aapl, addr, frequency);

#if AAPL_ENABLE_07NM_IP || AAPL_ENABLE_05NM_IP
    if( aapl_get_ip_type(aapl, addr) == AVAGO_VTMON )
        return;
#endif

    if( sensor > 7 ) sensor = 0;

    if( avago_sbus_rd(aapl, addr, 0x03) != (0x200U<<sensor) ||
        avago_sbus_rd(aapl, addr, 0x00) != 2 )
    {
        avago_sbus_wr(aapl, addr, 0x00, 0x01);
        avago_sbus_wr(aapl, addr, 0x03, 0x200 << sensor );
        avago_sbus_wr(aapl, addr, 0x00, 0x02);
    }
}

/** @brief   Performs a blocking read on a voltage sensor.
 ** @details The sensor must already be properly initialized.
 ** @return  On success, returns the voltage in mV.
 ** @return  On error, decrements aapl->return_code and returns -1.
 ** @see     avago_sensor_start_voltage(), avago_sensor_get_voltage().
 **/
int avago_sensor_wait_voltage(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Sensor sbus address. */
    int sensor)     /**< [in] Which sensor to access.  A value of 0 selects */
                    /**<      the VDD voltage, 1 selects CORE_VDD_VIN, and */
                    /**<      values [2..7] select SENSOR_A2D_VIN[0..5] */
                    /**< The 7nm block has remote sensors [0..7] without special designations. */
{
    int return_code = aapl->return_code;
    int data;
    int mV = -1;

    if( use_sbm_for_temperature_reading(aapl, addr) )
    {
        uint sbm_addr = avago_make_sbus_master_addr(addr);
        data = avago_spico_int(aapl, sbm_addr, 0x18, ((sensor & 0xf) << 12) | (addr & 0xff));
        if( data & 0x8000 )
        {
            if( sensor >= 6 && aapl_get_process_id(aapl, sbm_addr) == AVAGO_PROCESS_E && !aapl_check_firmware_rev(aapl,sbm_addr, __func__, __LINE__, FALSE, 1, 0x1027) )
                data = (data & 0xfff) * (sensor == 6 ? 2 : 4);

            mV = (data & 0x3fff) / 2;
        }
    }
    else
#if AAPL_ENABLE_07NM_IP || AAPL_ENABLE_05NM_IP
    if( aapl_get_ip_type(aapl, addr) == AVAGO_VTMON )
    {
        int G = 1020;
        int scale = (sensor == 6) ? 2 : (sensor == 7) ? 4 : 1;
        int sensor_reg = sensor + 0x11;
        data = avago_apb_read16(aapl, addr, sensor_reg);
#if AAPL_ENABLE_05NM_IP
        if( aapl_get_process_id(aapl, addr) == AVAGO_TSMC_05 )
            G = 1002;
#endif
        mV = scale * G * (data & 0x7ff) / 2048;
    }
    else
#endif
    {
#if AAPL_ENABLE_16NM_IP || AAPL_ENABLE_28NM_IP
        int i;
        for( i = 0; i < 8; i++ )
        {
            if( i > 0 ) ms_sleep(4);
            data = avago_sbus_rd(aapl, addr, 74 + sensor);
            if( data & 0x8000 )
            {
                mV = (data & 0xfff) / 2;
                break;
            }
        }
#endif
    }

    if( mV == -1 || aapl->return_code != return_code )
        return aapl_fail(aapl, __func__, __LINE__, "Voltage conversion failed, returning -1.\n");

    AAPL_LOG_PRINT5(aapl, AVAGO_DEBUG5, __func__, __LINE__, "Voltage = %4d mV (data=0x%x)\n",mV,data);
    return mV;
}

/** @brief   Performs a voltage measurement.
 ** @details Combines the start and wait functions into one blocking operation.
 ** @return  On success, returns the voltage in mV.
 ** @return  On error, decrements aapl->return_code and returns -1.
 ** @see     avago_sensor_start_voltage(), avago_sensor_wait_voltage().
 ** @see     avago_sensor_get_temperature().
 **/
int avago_sensor_get_voltage(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Sensor sbus address. */
    int sensor,     /**< [in] Which sensor to access.  A value of 0 selects */
                    /**<      the VDD voltage, 1 selects CORE_VDD_VIN, and */
                    /**<      values [2..7] select SENSOR_A2D_VIN[0..5] */
                    /**< The 7nm block has remote sensors [0..7] without special designations. */
    uint frequency) /**< [in] Sensor clock input frequency in Hertz. */
                    /**<      If non-zero, configures the sensor, */
                    /**<      which only need be done once. */
{
    int volt;
    AAPL_SENSOR_LOCK;
    avago_sensor_start_voltage(aapl, addr, sensor, frequency);
    volt = avago_sensor_wait_voltage(aapl, addr, sensor);
    AAPL_SENSOR_UNLOCK;
    return volt;
}


/** @} */

