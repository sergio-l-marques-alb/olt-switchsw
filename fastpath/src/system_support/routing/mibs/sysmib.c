/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 * *********************************************************************
 *
 * @filename                    sysmib.c
 *
 * @purpose                     MIBs subsystem initialization and auxillary
 *                              functions.
 *
 * @component                   Routing MIB Component
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
#include <string.h>
#include "std.h"
#include "sysmib.h"
#include "timer.ext"
#include "oll.ext"
#include "xx.ext"

ulng sys_timeSystemUp;

void L7MibInit()
{
   sys_timeSystemUp = TIMER_SysTime();
   /*!!! Put your SNMP agent initilization here */
}

void L7MibDeinit()
{
   /*!!! Put your SNMP agent deinitilization here */
}

int oidcmp(ulng *oid, word oidLen, t_oidSetTestVal *val)
{
   word i;

   for(i = 0; i < val->oidLen; i++)
   {
      if(i == oidLen)
         return -1;
      if(oid[i] > val->oid[i])
         return 1;
      if(oid[i] < val->oid[i])
         return -1;
   }
   return 0;
}

Bool CheckValueInRange(t_rangeTestTable *table, word tableSize, word lastOid, ulng Value)
{
   Bool valueChecked = FALSE;
   word i;

   for(i = 0; i < tableSize; i++)
      if(table[i].lastOid == lastOid)
      {
         valueChecked = TRUE;
         if(Value >= table[i].minVal && 
            Value <= table[i].maxVal)
            return TRUE;
      }

   if(valueChecked)
      return FALSE;
   return TRUE;
}


/* delete MIB table implemented as OLL */
void L7Mib_DeleteTable (t_Handle table)
{
   t_Handle p_Entry = OLL_GetNext(table, NULLP);
   t_Handle p_Next  = NULLP;

   while(p_Entry)
   {
      p_Next = OLL_GetNext(table, p_Entry);
      XX_Free(p_Entry);
      p_Entry = p_Next;
   }

    OLL_Delete(table);   
}
