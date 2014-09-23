/*
 * Product: EmWeb
 * Release: R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 * 
 * Notice to Users of this Software Product:
 * 
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
 * source and object code format embodies valuable intellectual property 
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and 
 * copyright laws of the United States and foreign countries, as well as 
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 * 
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 * 
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 * 
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 * EmWeb database definitions
 *
 */
#ifndef _EW_COMMON_H
#define _EW_COMMON_H

#include "ew_types.h"

/*
 * Multi-byte access macros
 * The following macros retrieve data from the archive database generated
 * by the EmWeb/Compiler based on the endian of the machine.  The input to
 * these macros is the (uint8 *) array pointer found in multi-byte fields
 * defined below.
 *
 * These macros are to be used in the server target platform _only_;
 * For the development host, use the macros defined in config/ewc_config.h
 */
#if EMWEB_ENDIAN != EMWEB_ARCHIVE_ENDIAN

#define EW_BYTES_TO_UINT16(p)   (((uint16)(p)[0]<<8)|((p)[1]))
#define EW_BYTES_TO_UINT32(p)   (((uint32)(p)[0]<<24)|((uint32)(p)[1]<<16)|((uint16)(p)[2]<<8)| (p)[3])
#define EW_UINT16_TO_BYTES(p,v) {p[0]=((v)>>8)&255;p[1]=(v)&255;}
#define EW_UINT32_TO_BYTES(p,v) {p[0]=((v)>>24)&255;p[1]=((v)>>16)&255;\
                                 p[2]=((v)>>8)&255;p[3]=(v)&255;}

#else /* EMWEB_ENDIAN == EMWEB_ARCHIVE_ENDIAN */

#define EW_BYTES_TO_UINT16(p)   (*((uint16 *)(p)))
#define EW_BYTES_TO_UINT32(p)   (*((uint32 *)(p)))
#define EW_UINT16_TO_BYTES(p,v) { uint16 *pp = (uint16 *)p; *pp=v; }
#define EW_UINT32_TO_BYTES(p,v) { uint32 *pp = (uint32 *)p; *pp=v; }

#endif /* EMWEB_ENDIAN vs EMWEB_ARCHIVE_ENDIAN */


#endif /* _EW_COMMON_H */
