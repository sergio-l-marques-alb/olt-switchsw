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
 * EmWeb image map definitions between server and generated code
 *
 */
#ifndef _EW_IMAP_H
#define _EW_IMAP_H

#include "ew_types.h"

typedef struct EwImageMap_s
  {
    uint32      ul_x;   /* upper-left X coordinate */
    uint32      ul_y;   /* upper-left Y coordinate */
    uint32      lr_x;   /* lower-right X coordinate */
    uint32      lr_y;   /* lower-right Y coordinate */
    const char  *url;   /* relative redirection URL */
  } EwImageMap, * EwImageMapP;

typedef struct EwImageMapTable_s
  {
    const EwImageMap *map_table; /* table of map entries */
    uintf       map_entries;     /* size of map_table */
    const char  *default_url;    /* default URL, or NULL if not specified */
  } EwImageMapTable, * EwImageMapTableP;

#endif /* _EW_IMAP_H */
