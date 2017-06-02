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
 * Transparent Content Negotiation
 *
 */
#ifndef _EW_CONNEG_H
#define _EW_CONNEG_H

/*
 * EmWeb Content Negotiation
 */

/* used on compiler and server side */

/* enumerations for EwsAttrData below. This ordering is important.
 * The enumerations match the struct below and type, charset and
 * language are also used as vary_hdr_flags in ewsSelectVariant()
 */
typedef enum EwsAttr_e
{
  ewAttrName = 0,    /* name field */
  ewAttrType,        /* type field */
  ewAttrCharset,     /* charset field */
  ewAttrEncoding,    /* encoding field */
  ewAttrLanguage,    /* language field */
  ewAttrQuality      /* quality field */
} EwsAttr;

/* used on compiler side */

typedef struct EwsAttrData_s
{
  uint8 name[4];
  uint8 type[4];
  uint8 charset[4];
  uint8 encoding[4];
  uint8 language[4];
  uint8 quality[4];
} EwsAttrData;

/* holds all attributes for a document on compiler side.
 * this must match EwsAttrData_s above.
 */
typedef struct attrData_s
{
  uint32  name;
  uint32  type;
  uint32  charset;
  uint32  encoding;
  uint32  language;
  uint32  quality;
} attrData;

typedef struct VariantObj_s
{
  struct VariantObj_s  *next;
  char                 *variant;
} VariantObj;

typedef struct AttributeObj_s
{
  struct AttributeObj_s  *next;
  EwsAttr                type;
  void                   *attribute;
} AttributeObj;

/* first section of the variant data block. holds a negotiable resources variant data */
typedef struct varDataBlock_s
{
  uint32 alternates;   /* Alternates reply header */
  uint32 num_variants; /* number of variants this negotiable resource has */
} varDataBlock;

#endif /* _EW_CONNEG_H */
