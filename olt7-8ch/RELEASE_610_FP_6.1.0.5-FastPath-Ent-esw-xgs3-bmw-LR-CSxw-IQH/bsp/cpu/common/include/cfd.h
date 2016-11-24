/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  cfd.h
*
* @purpose   Configuration Data Defines
*
* @component sysapi
*
* @create    09/06/2000
*
* @author    paulq
*
* @end
*
*********************************************************************/
#ifndef __cfdh
#define __cfdh

#ifdef __cplusplus
extern "C" {
#endif

L7_RC_t getCFDField( L7_uchar8 field,
                     L7_char8 *pCFD,
                     void *pbuffer);
L7_RC_t RuntimeVPDFileGenerate(L7_char8 *pCFD);

#define SERIAL_EEPROM_SIZE  256

/* CFD vectors....what's inside */
#define CFD_FILLER                    0x00
#define CFD_CARD_ID                   0x01
#define CFD_REVISION_ID               0x02
#define CFD_CRC                       0x03
#define CFD_CARD_BASE_MAC             0xA0
#define CFD_NUMBER_MAC_ADDRESSES      0xA1
#define CFD_CPU_ID                    0xA2
#define CFD_CPU_TYPE                  0xA3
#define CFD_INT_LEVEL                 0xB0
#define CFD_SLOTS_SUPPORTED           0xC0
#define CFD_SN                        0xC1
#define CFD_TYPE                      0xC2
#define CFD_MODEL                     0xC3
#define CFD_MFGR                      0xC4
#define CFD_PN                        0xC5
#define CFD_FRU                       0xC6
#define CFD_DESC                      0xC7
#define CFD_MAINT_LVL                 0xC8
#define CFD_OID                       0xC9
#define CFD_PASSWORD                  0xD0
#define CFD_SERVICE_TAG               0xD1
#define CFD_HW_REV                    0xD2
#define CFD_BOARD_ID                  0xD3
#define CFD_EXTENDED_VECTOR_CODE      0xFF

/* Pack the EEPROM */

  typedef struct cfd_mapping_t
  {
    L7_uchar8 length __attribute__ ((packed));
    L7_uchar8 vector __attribute__ ((packed));
    L7_uchar8 data[4] __attribute__ ((packed));
  } cfd_mapping;

/*
* Card id's
* PAY ATTENTION!!
* ALL CARD ID'S AND REVISION ID'S ARE NOW KEPT UNDER THE CONTROL
* OF THE DRIVER DATABASE FILE hpc_db.h
*/

/* CFD CPU TYPES */
#define MPC860      0x01
#define MPC855T     0x02
#define MPC860T     0x03
#define MPC850      0x04
#define MPC860P     0x05
#define MPC603      0x06
#define TYPE_BCM1125 0x07
#define TYPE_BCM4704 0x08


#ifdef __cplusplus
}
#endif

#endif /* __cfdh */
