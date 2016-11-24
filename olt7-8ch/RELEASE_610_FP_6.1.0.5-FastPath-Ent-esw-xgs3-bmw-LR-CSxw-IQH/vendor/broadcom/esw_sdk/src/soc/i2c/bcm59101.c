/*
 * $Id: bcm59101.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * BCM56xx I2C Device Driver for BCM59101 8-bit PoE Microcontroller Unit.
 * The MCU communicates with message types of 12-byte long packet.
 *
 */

#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>

#define BCM59101_CTN     12

STATIC int
bcm59101_read(int unit, int devno,
             uint16 addr, uint8* data, uint32* len)
{
    
#ifdef PD63000
    int i, rv = SOC_E_NONE;
    uint8 saddr = soc_i2c_addr(unit, devno);
    uint8 *ptr;
    
    if (!data) {
        return SOC_E_PARAM;
    }

    ptr = data;
    for (i = 0; i < PD63000_CTN; i++, ptr++) {
        if ((rv = soc_i2c_read_byte(unit, saddr, ptr)) < 0) {
            soc_cm_debug(DK_I2C, "i2c%d: soc_i2c_pd63000_read: "
                         "failed to read data byte %d.\n",
                         unit, i);
            break;
        }
        soc_i2c_device(unit, devno)->rbyte++;   
    }
    *len = i;
          
    return rv;
#else
    int rv = SOC_E_NONE;
    uint8 *ptr, saddr_r;
    uint32 nbytes = 0;

/*    SOC_ERROR_PRINT((0, "Gator: %s:%d unit: %d devno: %d addr: %d data: %p len: %d\n",
                     __func__, __LINE__, unit, devno, addr, data, *len)); */


    /* Valid address, memory and size must be provided */
    if ( ! len || ! data )
        return SOC_E_PARAM;

    I2C_LOCK(unit);

    saddr_r = SOC_I2C_RX_ADDR(soc_i2c_addr(unit, devno));

    soc_cm_debug(DK_I2C,
                 "bcm59101_read: devno=0x%x (data=%p) len=%d\n",
                 devno, data, (int)*len);

    /* Now, we have sent the first and second word addresses,
     * we then issue a repeated start condition, followed by
     * the device's read address (note: saddr_r)
     */
    /* Generate Start, for Write address */
    if( (rv = soc_i2c_start(unit, saddr_r)) < 0){
      soc_cm_debug(DK_I2C, "bcm59101_write(%d,%d,0x%x,%p,%d): "
          "failed to gen start\n",
          unit, devno, addr, data, *len);
              goto error;
    }

    nbytes = *len;
    ptr = data;
    if ( (rv = soc_i2c_read_bytes(unit, data, (int *)&nbytes, 0) ) < 0 ) {
        goto error;
    }
    *len = nbytes;

 error:

    soc_i2c_stop(unit);

    I2C_UNLOCK(unit);
    return rv ;

#endif
}

STATIC int
bcm59101_write(int unit, int devno,
              uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 saddr = soc_i2c_addr(unit, devno);
    uint8 *ptr;
    uint32 i;

/*    SOC_ERROR_PRINT((0, "Gator: %s:%d unit: %d devno: %d addr: %d data: %p len: %d\n",
                     __func__, __LINE__, unit, devno, addr, data, len)); */

    if (!data) {
        return SOC_E_PARAM;
    }
#ifdef PD63000
    assert((PD63000_CTN == len));
#endif

    I2C_LOCK(unit);

    if ((rv = soc_i2c_start(unit, SOC_I2C_TX_ADDR(saddr))) < 0) {
        soc_cm_debug(DK_I2C, "i2c%d: soc_i2c_pd63000_write: "
                     "failed to generate start.\n",
                     unit);
        I2C_UNLOCK(unit);
        return rv;
    }

    ptr = data;
    for (i = 0; i < len; i++, ptr++) {
        if ((rv = soc_i2c_write_one_byte(unit, *ptr) ) < 0 ) {
            soc_cm_debug(DK_I2C, "i2c%d: soc_i2c_pd63000_write: "
                         "failed to send byte %d.\n",
                         unit, i );
            break;
        }
        soc_i2c_device(unit, devno)->tbyte++;
    }

    soc_i2c_stop(unit);
    I2C_UNLOCK(unit);

    return rv;
}



STATIC int
bcm59101_ioctl(int unit, int devno,
              int opcode, void* data, int len)
{
    return SOC_E_NONE;
}

STATIC int
bcm59101_init(int unit, int devno,
             void* data, int len)
{
#ifdef PD63000
    uint8 *init_vector = (uint8*)data;
    uint8 pkt[BCM59101_CTN];
    uint32 l;
    soc_timeout_t to;
#endif
    int rv = SOC_E_NONE;

    SOC_ERROR_PRINT((0, "BCM59101 Chip Detected\n"));

    soc_i2c_devdesc_set(unit, devno, "BCM59101 PoE MCU");

#ifdef PD63000
    if ((rv = pd63000_read(unit, devno, 0, pkt, &l)) < 0) {
        soc_cm_debug(DK_I2C, "i2c%d: soc_i2c_pd63000_init: "
                     "failed to read init packet.\n", unit);
        return rv;
    }

    if ((rv = pd63000_write(unit, devno, 0, 
                            init_vector, BCM59101_CTN)) < 0) {
        soc_cm_debug(DK_I2C, "i2c%d: soc_i2c_pd63000_init: "
                     "failed to send power init packet.\n", unit);
        return rv;
    }

    soc_timeout_init(&to, 100000, 0);
    for (;;) {
         if (soc_timeout_check(&to)) {
             break;
         }
    }

    if ((rv = pd63000_read(unit, devno, 0, pkt, &l)) < 0) {
        soc_cm_debug(DK_I2C, "i2c%d: soc_i2c_pd63000_init: "
                     "failed to read power init packet.\n", unit);
        return rv;
    }
#endif

    return rv;
}


/* BCM59101 Chip Driver callout */
i2c_driver_t _soc_i2c_bcm59101_driver = {
    0x0, 0x0, /* System assigned bytes */
    BCM59101_DEVICE_TYPE,
    bcm59101_read,
    bcm59101_write,
    bcm59101_ioctl,
    bcm59101_init,
};

