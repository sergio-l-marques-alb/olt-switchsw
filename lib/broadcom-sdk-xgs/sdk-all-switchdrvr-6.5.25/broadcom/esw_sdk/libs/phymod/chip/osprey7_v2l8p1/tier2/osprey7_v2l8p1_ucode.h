/*
 * $Id: osprey7_v2l8p1_ucode.h $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * All Rights Reserved.$
 */
#ifndef osprey7_v2l8p1_ucode_H_
#define osprey7_v2l8p1_ucode_H_ 

#include <phymod/phymod.h>


#define OSPREY7_V2L8P1_UCODE_IMAGE_VERSION "D003_00"  /* matches the version number from microcode */
#define OSPREY7_V2L8P1_UCODE_IMAGE_SIZE    145232
#define OSPREY7_V2L8P1_UCODE_STACK_SIZE    0x13F2
#define OSPREY7_V2L8P1_UCODE_IMAGE_CRC     0x224D
#define OSPREY7_V2L8P1_UCODE_DR_SIZE       0x0

extern unsigned char*  osprey7_v2l8p1_ucode_get(void);


#endif

