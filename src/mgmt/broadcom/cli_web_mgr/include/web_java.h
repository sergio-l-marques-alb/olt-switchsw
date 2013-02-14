/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/base/web_java.h
 *
 * @purpose Support of the EmWeb code
 *
 * @component unitmgr
 *
 * @comments tba
 *
 * @create 13-may-2003
 *
 * @author gaunce
 * @end
 *
 **********************************************************************/

#ifndef WEB_JAVA_H__
#define WEB_JAVA_H__

#define USMWEB_LED_STATUS_GREEN   1    /* These must agree with the values in MapApplet.java */
#define USMWEB_LED_STATUS_AMBER   2
#define USMWEB_LED_STATUS_YELLOW  3
#define USMWEB_LED_STATUS_RED     4
#define USMWEB_LED_STATUS_OFF     5
#define UNIT_TYPE_UNSUPPORTED 1431634177  /* These are the unit Types which after the 'and(&)'
                                             operation with 0xFFFF0000 gives the desired number
                                             in the param names to get the unsupported, config m                                             mismatch, code mismatch and 'not present' images                                                from the corresponding html files */
#define UNIT_TYPE_CFG_MISMATCH 1431568641
#define UNIT_TYPE_CODE_MISMATCH 1431503105
#define UNIT_TYPE_NOT_PRESENT 1431437569

#define APPLET_IFLAG_STACKING_MASK      0x0000FF00
#define APPLET_IFLAG_FPS_SUPPORT        0x00000100
#define APPLET_IFLAG_FPS_MODE_SUPPORT   0x00000200
#define APPLET_IFLAG_FPS_MODE           0x00000400
#define APPLET_IFLAG_FPS_STACK_MODE     0x00000400
#define APPLET_IFLAG_STACK_MODE_ONLY    0x00000800
#define APPLET_IFLAG_STACK_MODE_SUPPORT 0x00001000
#define APPLET_IFLAG_STACK_MODE_HIGIG   0x00002000
#define APPLET_IFLAG_STACK_MODE_10G     0x00004000

L7_RC_t usmWebNumSFPsGet(L7_uint32 * numSFPs);

#endif
