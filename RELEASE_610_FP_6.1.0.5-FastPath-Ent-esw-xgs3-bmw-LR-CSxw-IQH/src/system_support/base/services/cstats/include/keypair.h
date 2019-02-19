/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    keypair.h
* @purpose     API's and data structures of keypair.c
* @component   stats
* @comments    
* @create      18/06/2007
* @author      YSaritha 
* @end
*             
**********************************************************************/

#ifndef INCLUDE_KEYPAIR_H
#define INCLUDE_KEYPAIR_H

#ifdef _L7_OS_LINUX_
#define COUNTER_FLY_WEIGHT_MAX 7000
#else
#define COUNTER_FLY_WEIGHT_MAX 5000
#endif

typedef struct keyPair_s
{
  L7_uint32 key;
  L7_uint32 id;
  void *ptr;
  counter_type_t ctype;
} keyPair;

L7_RC_t keyPair_init (keyPair **);

L7_RC_t keyPair_get (keyPair *, keyPair **);

L7_RC_t keyPair_set (keyPair *, keyPair **);

#endif
