/*
 * Product:  EmWeb
 * Release:  R6_2_0
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
 * EmWeb Compression
 *
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "ew_cmp.h"

void
main( int argc, char **argv )
{
  EwCompressContext context;
  int fd;
  uint8 byte;
  uint8 *datap;
  uint32 length;

    if (argc != 2)
      {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        exit(1);
      }
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
      {
        fprintf(stderr, "%s: unable to open %s\n", argv[0], argv[1]);
        exit(1);
      }
    context = ewCompressDictionaryOpen(EMWEB_ARCHIVE_ENDIAN);
    if (context == NULL)
      {
        fprintf(stderr, "ewCompressDictionaryOpen failed\n");
        exit(1);
      }
    while (read(fd, &byte, 1) > 0)
      {
        ewCompressDictionaryWrite(context, &byte, 1);
      }
    ewCompressDictionaryCreate(context, &datap, &length);
    while ( length > 0 )
      {
        putchar(*datap++);
        length--;
      }
    if (lseek(fd, 0, SEEK_SET) < 0)
      {
       fprintf(stderr, "%s: seek failed\n", argv[0]);
       exit(1);
      }
    ewCompressBegin(context, NULL);
    while (read(fd, &byte, 1) > 0)
      {
        ewCompressWrite(context, &byte, 1);
      }
    ewCompressEnd(context);
    ewCompressDictionaryClose(context);
}

void ewCompressOutput ( void * handle, uint8 byte )
{
    putchar(byte);
}
