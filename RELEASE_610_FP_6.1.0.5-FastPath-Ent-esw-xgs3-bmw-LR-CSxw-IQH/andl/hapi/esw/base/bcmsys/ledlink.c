/*
 * $Id: ledlink.c,v 1.1 2011/04/18 17:10:17 mruas Exp $
 * $Copyright: (c) 2002, 2003 Broadcom Corp.
 * All Rights Reserved.$
 *
 * ledlink.c
 *
 * Common LED linkscanning code for all boards. 
 */

#include <soc/drv.h>
#include "sysbrds.h"
#include <soc/phyctrl.h>

#if defined(BCM_LEDPROC_SUPPORT)
#include "ledlink.h"

/*
 * Function: 	ledproc_linkscan_cb
 * Purpose:	Call back function for LEDs on link change.
 * Parameters:	unit - unit number
 *              port - callback from this port
 *              info - pointer to structure giving status
 * Returns:	nothing
 * Notes:	Each port has one byte of data at address (0x80 + portnum).
 *		In each byte, bit 0 is used for link status.
 *		In each byte, bit 7 is used for turbo mode indication.
 */
void
hpc_default_led_linkscan_cb(int unit, soc_port_t port, bcm_port_info_t *info)
{
    uint32	portdata;
    int		byte = LS_LED_DATA_OFFSET + port;

#ifdef BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(unit)) {
        byte = LS_TUCANA_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_RAVEN_SUPPORT
    if (SOC_IS_RAVEN(unit)) {
        byte = LS_RAVEN_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH(unit)) {
        byte = LS_TR_VL_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_VALKYRIE_SUPPORT
    if (SOC_IS_VALKYRIE(unit)) {
        byte = LS_TR_VL_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        byte = LS_SC_CQ_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_RAPTOR_SUPPORT
    if (SOC_IS_RAPTOR(unit) || SOC_IS_HAWKEYE(unit)) {
        byte = LS_RAPTOR_LED_DATA_OFFSET + port;
    }
#endif

    portdata = soc_pci_read(unit, CMIC_LED_DATA_RAM(byte));

#ifdef L7_CHASSIS
    if(info->linkstatus) {
	portdata &= ~0x02;
	portdata |= 0x01;
    } else {
	portdata &= ~0x03;
    }
    /* Put off copper lead if fiber is selected and up */
    if ( (info->linkstatus) && (info->medium == SOC_PORT_MEDIUM_FIBER)) /* Fiber is Up */
    { 
       portdata &= ~0x01;
       portdata |=  0x02;
    }
#else
    if (info->linkstatus) {
	portdata |= 0x01;
    } else {
	portdata &= ~0x01;
    }
#endif

	portdata &= ~0x80;

  soc_pci_write(unit, CMIC_LED_DATA_RAM(byte), portdata);


}

#endif /* defined(BCM_LEDPROC_SUPPORT) */

