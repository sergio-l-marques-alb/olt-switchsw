/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 * *********************************************************************
 *
 * @filename             sysmib.h
 *
 * @purpose
 *
 * @component            Routing MIB Component
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
#ifndef _l7_mib_definitions_h_
#define _l7_mib_definitions_h_

#include "std.h"

#define MAX_SET_TEST_OID_LEN  15

typedef struct tag_oidSetTestVal
{
   word oid[MAX_SET_TEST_OID_LEN];
   word oidLen;
}t_oidSetTestVal;

typedef struct tag_oidTestTable
{
   word lastOid;
   t_oidSetTestVal minOid;
   t_oidSetTestVal maxOid;
}t_oidTestTable;

typedef struct tag_rangeTestTable
{
   word lastOid;
   ulng minVal;
   ulng maxVal;
}t_rangeTestTable;

/* entry is a pointer to the item to be removed from the OLL table */
#define L7MIB_EXTRACTDELETE(entry, table) \
   if( entry ) { \
      OLL_Remove( table, entry ); \
      XX_Free(entry); \
   } \
   if(table && !OLL_GetNext(table, NULL)) { \
      OLL_Delete(table); \
      table = NULL; \
   }


void L7MibInit(void);
void L7MibDeinit(void);
int oidcmp(ulng *oid, word oidLen, t_oidSetTestVal *val);
Bool CheckValueInRange(t_rangeTestTable *table, word tableSize, word lastOid, ulng Value);

/* delete MIB table implemented as OLL and deallocate all the entries */
void L7Mib_DeleteTable(t_Handle table);


#endif /* _l7_mib_definitions_h_ */
