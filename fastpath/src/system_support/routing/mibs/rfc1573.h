/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 * *********************************************************************
 *
 * @filename              rfc1573.h
 *
 * @purpose
 *
 * @component             Routing MIB Component
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
#ifndef rfc1573_h
#define rfc1573_h

#include "rfc1443.h"


/*
 * interface stack table
 */
typedef struct tag_ifStackTable
{
   ulng         ifStackHigherLayer;
   ulng         ifStackLowerLayer;
   e_RowStatus  ifStackStatus;
} t_ifStackTable;




#endif

/* --- end of file rfc1573.h --- */
