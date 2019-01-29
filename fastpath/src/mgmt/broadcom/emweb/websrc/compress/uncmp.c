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
#include <stdlib.h>
#include <unistd.h>
#include "ew_cmp_i.h"
#include "osapi.h"

void
main( int argc, char **argv )
{
  EwDecompressContext context;
  EwCompressDictionaryHeader header;
  uint8 *dictionary;
  uint32 dict_size;
  int fd;
  uint8 byte;

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
    if (read(fd, &header, sizeof(header)) != sizeof(header))
      {
        fprintf(stderr, "%s: unable to read header\n", argv[0]);
        exit(1);
      }
    dict_size = EW_BYTES_TO_UINT16(header.dict_size);
    dict_size += 3 * EW_BYTES_TO_UINT16(header.index_size);
    dictionary = (uint8 *) malloc(dict_size + sizeof(header));
    if (dictionary == NULL)
      {
        fprintf(stderr, "%s: unable to allocate dictionary\n", argv[0]);
        exit(1);
      }
    memcpy(dictionary, &header, sizeof(header));
    if (read(fd, dictionary + sizeof(header), dict_size) != dict_size)
      {
        fprintf(stderr, "%s: unable to read dictionary\n", argv[0]);
        exit(1);
      }
    context = ewDecompressBegin(dictionary, (void *) fd);
    if (context == NULL)
      {
        fprintf(stderr, "ewDecompressBegin failed\n");
        exit(1);
      }
    while (ewDecompressRead(context, &byte, 1) == 1)
      {
        putchar(byte);
      }
    free(context);
    free(dictionary);
}

sintf ewDecompressInput ( void * handle )
{
  int fd = (int) handle;
  uint8 byte;

    if (read(fd, &byte, 1) != 1)
      {
        return -1;
      }
    else
      {
        return byte;
      }
}

void * ewaAlloc ( uint32 len )
{
  return osapiMalloc ( L7_CLI_WEB_COMPONENT_ID, len );  /* LVL7_P0006 */
}  
void ewaFree ( void * ptr )
{
  osapiFree ( L7_CLI_WEB_COMPONENT_ID, ptr ) ;  /* LVL7_P0006 */
}  




