/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 * *********************************************************************
 *
 * @filename           rfc1443.h
 *
 * @purpose
 *
 * @component          Routing MIB Component
 *
 * @comments
 *
 * @create
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#ifndef rfc1443_h
#define rfc1443_h

#include "std.h"

typedef enum tag_RowStatus
{
     active = 1
   , notInService = 2
   , notReady = 3
   , createAndGo = 4
   , createAndWait = 5
   , destroy = 6
} e_RowStatus;


/* truth value */
typedef enum tag_TruthValue
{
     _true = 1
   , _false = 2
} e_TruthValue;


typedef ulng t_IpAddr;

/* Storage type */
typedef enum tag_StorageType
{
   StorageType_other       = 1,
   StorageType_volatile    = 2,
   StorageType_nonVolatile = 3,
   StorageType_permanent   = 4,
   StorageType_readOnly    = 5
} e_StorageType;


#endif

/* --- end of file rfc1443.h --- */
