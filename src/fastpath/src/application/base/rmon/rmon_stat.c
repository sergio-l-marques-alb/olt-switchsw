/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_stat.c
*
* @purpose  this is the core file for RMON statistics group
*           
* @component SNMP-RMON
*
*
* @create  08/06/2001
*
* @author  Kumar Manish 
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include "l7_common.h"
#include "rmon_include.h"
#include "rmon_index.h"
#include "rmon_common.h"
#include "rmon_stat.h"
#include "usmdb_mib_rmon_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

/* Array of rmonEtherStats entries, external here, it is already defined in file snmp_rmon.c */
extern rmonEtherStatsEntry_t *rmonEtherStatsEntryArray;

/* The SNMP to array index mapping handle.
*/
static L7_uint32 etherStatMap;

/*********************************************************************
*
* @purpose    Initialize the RMON Ether Stat Mapping table.
*
* @returntype L7_SUCCESS    Ether Stat Mapping table is initialized.
*             L7_FAILURE    Initialization failed.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonEtherStatsTableInitialize(void)
{
    L7_uint32 i;
    rmonEtherStatsEntry_t* ese;
    etherStatMap = rmonMapCreate(RMON_ETHER_STAT_ENTRY_MAX_NUM);
    
    for(i = 0; i < RMON_ETHER_STAT_ENTRY_MAX_NUM; i++)
    {
        ese = &rmonEtherStatsEntryArray[i];
        memset (ese, 0, sizeof (rmonEtherStatsEntry_t));
    }
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose    Remove all configured entries in the table
*
* @returntype L7_SUCCESS    Ether Stat Mapping table entries are removed
*             L7_FAILURE    Unconfiguration failed.
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonEtherStatsTableUnconfigure(void)
{
    L7_uint32 index = 0;
    L7_RC_t rc = L7_SUCCESS;

    /* iterate over all entries in the table */
    while (rmonEtherStatsEntryNextGet(&index) == L7_SUCCESS)
    {
      if (rmonEtherStatsTableRemove(index) != L7_SUCCESS)
      {
        /* one of the removals failed for some reason */
        rc = L7_FAILURE;
      }

      /* set the index back to 0 to ensure we get the first entry every 
         time this ensures reindexing doesn't affect table navigation */
      index = 0;
    }
    return rc;
}


/*********************************************************************
*
* @purpose    Creates an EtherStat entry with default values
* 
* @param  intIfNum  @b{(input)}  internal interface number for this entry
* @param  ifIndex   @b{(input)}  ifIndex value of the port which has been created
* @param  Status    @b{(input)} Status to be set( here the status is set to VALID)
* @param  snmpIndex @b{(output)} snmp index selected for this interface
*
* @returntype L7_SUCCESS    if creation was successful
*             L7_FAILURE    if EtherStat Entry with that index exists, or if creation
*                            was not successful
*          
* @notes  Selects the next available snmp index for the interface.    
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonEtherStatsEntryCreate(L7_uint32 intIfNum, L7_uint32 ifIndex, L7_ushort16 Status, 
                          L7_uint32 *snmpIndex)
{   
    rmonEtherStatsEntry_t* ese;
    L7_uint32 array_index;
    L7_RC_t rc;
    
    rc = rmonMapFirstAvailableIndexGet (etherStatMap, snmpIndex);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    
    rc = rmonMapInsert (etherStatMap, *snmpIndex);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    
    /* Resolve array index from the SNMP index. */
    rc = rmonMapMatch (etherStatMap, *snmpIndex, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    ese = &rmonEtherStatsEntryArray[array_index];

    rmonEtherStatsEntryDefaultsSet(ese, *snmpIndex, intIfNum, ifIndex, Status);
       
    return L7_SUCCESS;  
}

/*********************************************************************
*
* @purpose    Insert  an EtherStat entry for a configured lag in the EtherStat Table, 
*             and also in the Index Mapping table with default values
* 
* @param      lagInterface  internal interface number for the lag
* @param      ifIndex       external  interface number for the lag
* @param      lagSnmpIndex  corresponding SNMP index in the mapping table
*
* @returntype L7_SUCCESS    if insertion was successful
*             L7_FAILURE    if insertion was not successful
*          
* @notes      
*
* @end
*
*********************************************************************/

L7_RC_t
rmonEtherStatsTableInsert(L7_uint32 lagInterface, L7_uint32 ifIndex, L7_uint32 *lagSnmpIndex)
{
    rmonEtherStatsEntry_t* ese;
    L7_uint32 array_index;
    L7_uint32 Index;
    L7_RC_t rc;
    
    rc = rmonMapFirstAvailableIndexGet (etherStatMap, &Index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    rc = rmonMapInsert (etherStatMap, Index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }
    
    /* Resolve array index from the SNMP index. */
    rc = rmonMapMatch (etherStatMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    ese = &rmonEtherStatsEntryArray[array_index];

    rmonEtherStatsEntryDefaultsSet(ese, Index, lagInterface,ifIndex,
                                      RMON_ETHER_STAT_STATUS_VALID);
      
    *lagSnmpIndex = Index;
    return L7_SUCCESS;


}

/*********************************************************************
*
* @purpose    Remove  an EtherStat entry for an unconfigured lag from the EtherStat Table, 
*             and also from the Index Mapping table with default values
* 
* @param      lagSnmpIndex  corresponding SNMP index in the mapping table
*
* @returntype L7_SUCCESS    if removal was successful
*             L7_FAILURE    if removal was not successful
*          
* @notes      
*
* @end
*
*********************************************************************/

L7_RC_t
rmonEtherStatsTableRemove(L7_uint32 lagSnmpIndex)
{
    rmonEtherStatsEntry_t* ese;
    L7_uint32 array_index;
    L7_RC_t rc;

    /* Resolve array index from the SNMP index. */
    rc = rmonMapMatch (etherStatMap, lagSnmpIndex, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    ese = &rmonEtherStatsEntryArray[array_index];

    memset (ese, 0, sizeof (rmonEtherStatsEntry_t));
    
    rc =  rmonMapRemove (etherStatMap, lagSnmpIndex);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    return L7_SUCCESS;


}

/*********************************************************************
*
* @purpose    Sets the members of EtherStat entry to default values
* 
* @param      ese       EntryStatEntry for which the default values have to be set
* @param      index     index of EtherStat Entry to create
* @param      ifIndex   ifIndex value of the port which has been created
* @param      Status    Status to be set( here the status is set to VALID)
*
* @returntype L7_SUCCESS    if setting of default values was successful
*             L7_FAILURE    if setting of default values was unsuccessful
*          
* @notes      
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonEtherStatsEntryDefaultsSet(rmonEtherStatsEntry_t* ese, L7_uint32 Index, L7_uint32 intIfNum,
                               L7_uint32 ifIndex,L7_ushort16 Status)
{
    L7_char8 *statsOwner = FD_RMON_STATS_OWNER; 
    ese->etherStatIndex      = Index;
    ese->dataSource = ifIndex;
    ese->dropEvents = 0;
    ese->octets     = 0;
    ese->pkts       = 0;
    ese->broadcastPkts  = 0;
    ese->multicastPkts  = 0;
    ese->CRCAlignErrors = 0;
    ese->undersizePkts = 0;
    ese->oversizePkts = 0;
    ese->fragments = 0;
    ese->jabbers = 0;
    ese->collisions = 0;
    ese->utilization = 0;
    ese->pkts64Octets = 0;
    ese->pkts65to127Octets = 0;
    ese->pkts128to255Octets = 0;
    ese->pkts512to1023Octets = 0;
    ese->pkts1024to1518Octets = 0;
    strncpy(ese->owner,statsOwner,strlen(statsOwner)); 
    ese->status = Status;
    ese->interface = intIfNum;
    ese->validEntry = L7_TRUE;
    return L7_SUCCESS;
                                                                          
}

/*********************************************************************
*
* @purpose    Deletes an EtherStat entry with the input ifIndex value
* 
* @param      ifIndex   ifIndex value of the etherStat entry to be deleted
*
* @returntype L7_SUCCESS    if deletion was successful
*             L7_FAILURE    if deletion was not successful
*          
* @notes      
*
* @end
*
*********************************************************************/

L7_RC_t 
rmonEtherStatsEntryDelete(L7_uint32 ifIndex)
{
   rmonEtherStatsEntry_t* ese;
   L7_RC_t rc;
   L7_BOOL interfaceFound = L7_FALSE;
   L7_uint32 next_snmp_index;
   L7_uint32 lowest_snmp_index;
   L7_uint32 highest_snmp_index;
   L7_uint32 index;

   L7_uint32 array_index;

     
   rc = rmonMapLowestGet(etherStatMap, &lowest_snmp_index);
   if (rc != L7_SUCCESS) {
       return L7_FAILURE;
   }

   index = lowest_snmp_index;

   rc = rmonMapHighestGet(etherStatMap, &highest_snmp_index);
   if (rc != L7_SUCCESS) {
       return L7_FAILURE;
   }

   while(index<= highest_snmp_index)
   {
       rc = rmonMapMatch (etherStatMap, index, &array_index);
       if (rc != L7_SUCCESS) {
           return L7_FAILURE;
       }

       ese = &rmonEtherStatsEntryArray[array_index];
       if(ese->dataSource == ifIndex)
       {
           interfaceFound = L7_TRUE;
           break;
       }
           
       rc =  rmonMapNextGet (etherStatMap, index, &next_snmp_index);
       if (rc != L7_SUCCESS) {
           return L7_FAILURE;
       }
       index = next_snmp_index;
   
   }
   if(interfaceFound == L7_TRUE)
   {
       rc =  rmonMapRemove (etherStatMap, index);
       if (rc != L7_SUCCESS) {
           return L7_FAILURE;
       }

   }
   
   return L7_SUCCESS;
  
}

/*********************************************************************
*
* @purpose    Checks index for SNMP GET requests
*
*             Index            Index of EtherStatsEntry to check
* 
* @returntype L7_SUCCESS    if valid entry
*             L7_FAILURE    if not a valid entry
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsEntryGet(L7_uint32 Index)
{ 
  rmonEtherStatsEntry_t* ese;

  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (etherStatMap, Index, &array_index);
  if (rc != L7_SUCCESS) {
      return L7_FAILURE;
  }

  ese = &rmonEtherStatsEntryArray[array_index];  
   
  if ((ese->etherStatIndex==Index) &&
      ((ese->status == RMON_ETHER_STAT_STATUS_VALID) ||
       (ese->status == RMON_ETHER_STAT_STATUS_UNDERCREATION)))
       return L7_SUCCESS;
  
  return L7_FAILURE;
}
                                                    
/*********************************************************************
*
* @purpose    Checks index for SNMP GET requests
*
* @param      Index         Index of EtherStatsEntry to check
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsEntryNextGet(L7_uint32 *Index)
{
   L7_RC_t rc;
   L7_uint32 next_snmp_index;
   rc =  rmonMapNextGet (etherStatMap, *Index, &next_snmp_index);
   if (rc != L7_SUCCESS) {
         return L7_FAILURE;
   }
   *Index = next_snmp_index;
            
        
   return L7_SUCCESS;
   
}


/*********************************************************************
*
* @purpose    Gets value of intIfNum for EtherStat entry
*
* @param      Index         index of EtherStat to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsInternalInterfaceNumGet(L7_uint32 Index, L7_uint32 *val)
{
  rmonEtherStatsEntry_t* ese;
  L7_uint32 array_index;
  L7_RC_t rc;

  rc = rmonMapMatch (etherStatMap, Index, &array_index);
  if (rc != L7_SUCCESS) {
      return L7_FAILURE;
  }

  ese = &rmonEtherStatsEntryArray[array_index];
  
  *val = ese->interface;
  return L7_SUCCESS;
  
}


/*********************************************************************
*
* @purpose    Gets value of Index of the table corresponding to the external interface number
*
* @param      Index         index of EtherStat entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes     this function is redundant nw, if not used, should be removed later
*
* @end
*
*********************************************************************/
/********
L7_RC_t
rmonEtherStatsInterfaceIndexGet(L7_uint32 ifIndex, L7_uint32 *val)
{
  rmonEtherStatsEntry_t* ese;
  L7_uint32 Index;
  for( Index =1;Index < RMON_ETHER_STAT_ENTRY_MAX_NUM; Index++)
  {
      ese = &rmonEtherStatsEntryArray[Index];
      if(ese->dataSource != ifIndex)
          continue;
      else{
          *val = Index;
          break;
      }
  
  }

  return L7_SUCCESS;
}

*******/

/*********************************************************************
*
* @purpose    Gets value of EtherStatsDataSource for EtherStat entry
*
* @param      Index         index of EtherStat to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsDataSourceGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonEtherStatsEntry_t* ese;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (etherStatMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    ese = &rmonEtherStatsEntryArray[array_index];
  
    *val = ese->dataSource;
    return L7_SUCCESS;
  
}


/*********************************************************************
*
* @purpose    Gets value of DropEvents for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsDropEventsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
    if ( usmDbEtherStatsDropEventsGet(UnitIndex, intIfIndex, val) != L7_SUCCESS )
    {
      return (L7_FAILURE);
    }

    return (L7_SUCCESS);

}

/*********************************************************************
*
* @purpose    Gets value of Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsOctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
    
    if ( usmDbEtherStatsOctetsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
 

}

/*********************************************************************
*
* @purpose    Gets value of Pkts for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
    if ( usmDbEtherStatsPktsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
 

}

/*********************************************************************
*
* @purpose    Gets value of Broadcast Pkts for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsBroadcastPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
    if ( usmDbEtherStatsBroadcastPktsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
 

}

/*********************************************************************
*
* @purpose    Gets value of Multicast Pkts for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsMulticastPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
    if ( usmDbEtherStatsMulticastPktsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
 

}

/*********************************************************************
*
* @purpose    Gets value of CRCAlignErrors for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsCRCAlignErrorsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                                L7_uint32 *val)
{
    if ( usmDbEtherStatsCRCAlignErrorsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
 

}

/*********************************************************************
*
* @purpose    Gets value of UndersizePkts for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsUndersizePktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
    if ( usmDbEtherStatsUndersizePktsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
    

}

/*********************************************************************
*
* @purpose    Gets value of OversizePkts for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsOversizePktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
    if ( usmDbEtherStatsOversizePktsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
    
}

/*********************************************************************
*
* @purpose    Gets value of Fragments for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsFragmentsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
    
    if ( usmDbEtherStatsFragmentsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
    
}

/*********************************************************************
*
* @purpose    Gets value of Jabbers for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsJabbersGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
    
    if ( usmDbEtherStatsJabbersGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
    
}

/*********************************************************************
*
* @purpose    Gets value of Collisions for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsCollisionsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
     
    if ( usmDbEtherStatsCollisionsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
    
}

/*********************************************************************
*
* @purpose    Gets value of Pkts64Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsPkts64OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
     
    if ( usmDbEtherStatsPkts64OctetsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
    
}

/*********************************************************************
*
* @purpose    Gets value of Pkts65to127Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsPkts65to127OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
     
    if ( usmDbEtherStatsPkts65to127OctetsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
    
}

/*********************************************************************
*
* @purpose    Gets value of Pkts128to255Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsPkts128to255OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
     
    if ( usmDbEtherStatsPkts128to255OctetsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
    
}

/*********************************************************************
*
* @purpose    Gets value of Pkts256to511Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsPkts256to511OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
     
    if ( usmDbEtherStatsPkts256to511OctetsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
    
}

/*********************************************************************
*
* @purpose    Gets value of Pkts512to1023Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsPkts512to1023OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
     
    if ( usmDbEtherStatsPkts512to1023OctetsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
    
}

/*********************************************************************
*
* @purpose    Gets value of Pkts1024to1518Octets for EtherStat entry
*
* @param      UnitIndex     index of current unit
*             Index         Index of the EtherStatsEntry
*             intIfIndex    internal interface number for the EtherStatEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end

***************************************************************************/
L7_RC_t
rmonEtherStatsPkts1024to1518OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val)
{
     
    if ( usmDbEtherStatsPkts1024to1518OctetsGet(UnitIndex, intIfIndex, val) == L7_SUCCESS )
        return (L7_SUCCESS);
    return (L7_FAILURE);
    
}


/*********************************************************************
*
* @purpose    Gets value of EtherStatsOwner for EtherStat entry
*
* @param      Index         index of EtherStat to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsOwnerGet(L7_uint32 Index, L7_char8 *buf)
{
  
    rmonEtherStatsEntry_t* ese;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (etherStatMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    ese = &rmonEtherStatsEntryArray[array_index];
  
    strcpy(buf, ese->owner);
    return L7_SUCCESS;
  
}

/*********************************************************************
*
* @purpose    Gets value of EtherStatsStatus for EtherStat entry
*
* @param      Index         index of EtherStat to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsStatusGet(L7_uint32 Index, L7_uint32 *val)
{
    rmonEtherStatsEntry_t* ese;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (etherStatMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    ese = &rmonEtherStatsEntryArray[array_index];
    *val = ese->status;
    return L7_SUCCESS;
  
}

/* SNMP SET Requests */
/*********************************************************************
*
* @purpose    Sets value of EtherStatsDataSource for etherStats entry
*
* @param      UnitIndex     index of current unit
*             Index         index of etherStats entry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsDataSourceSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val)
{
    rmonEtherStatsEntry_t* ese;
    L7_uint32 array_index;
    L7_uint32 intIfNum;
    L7_RC_t rc;

    rc = rmonMapMatch (etherStatMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    ese = &rmonEtherStatsEntryArray[array_index];

    /* value of datasource already matches to the value to be set, so no change */
    if( ese->dataSource == val)
        return L7_SUCCESS;

    /* else change the value to val */
    if ((ese->status == RMON_ETHER_STAT_STATUS_UNDERCREATION)  &&
         usmDbVisibleExtIfNumberCheck(UnitIndex,val) == L7_SUCCESS)
    {
      ese->dataSource = val;
      /* Also set the corresponding internal interface number */
      if (usmDbIntIfNumFromExtIfNum(val, &intIfNum) == L7_SUCCESS)
      {
        ese->interface = intIfNum;
  
        return L7_SUCCESS;
      }
    }
    return L7_FAILURE;
  
}

/*********************************************************************
*
* @purpose    Sets value of EtherStatsOwner for EtherStat entry
*
* @param      Index         index of EtherStat to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsOwnerSet(L7_uint32 Index, L7_char8 *buf)
{
    rmonEtherStatsEntry_t* ese;
    L7_uint32 array_index;
    L7_RC_t rc;

    rc = rmonMapMatch (etherStatMap, Index, &array_index);
    if (rc != L7_SUCCESS) {
        return L7_FAILURE;
    }

    ese = &rmonEtherStatsEntryArray[array_index];
  
  
    if(strlen(buf) > RMON_ETHER_STAT_OWNER_MAX_LENGTH)
        return L7_FAILURE;
    
    bzero( ese->owner, RMON_ETHER_STAT_OWNER_MAX_LENGTH+1);
    
    memcpy(ese->owner,buf,strlen(buf));
    return L7_SUCCESS;
  
}

/********************************************************************
                          Create   Under
         To:       Valid  Request  Creation  Invalid
 From:
 valid             OK       NO        OK       OK
 createRequest     N/A      N/A       N/A      N/A
 underCreation     OK       NO        OK       OK
 invalid           NO       NO        NO       OK
 nonExistent       NO       OK        NO       OK
**********************************************************************/

/*********************************************************************
*
* @purpose    Sets value of EtherStatsStatus for EtherStat entry
*
* @param      Index         index of EtherStatsEntry to set
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t
rmonEtherStatsStatusSet(L7_uint32 Index, L7_uint32 val)
{
    rmonEtherStatsEntry_t* ese;
    L7_RC_t rc = L7_FAILURE;

    L7_uint32 array_index;
    L7_RC_t rc1;

    /* If the input status is VALID, UNDERCREATION, or INVALID, the entry should be there in the */
    /* mapping table */
    if ((val == RMON_ETHER_STAT_STATUS_VALID) ||
        (val == RMON_ETHER_STAT_STATUS_UNDERCREATION) ||
       (val ==  RMON_ETHER_STAT_STATUS_INVALID)) 
    {

            rc1 = rmonMapMatch (etherStatMap, Index, &array_index);
            if (rc1 != L7_SUCCESS)
            {
              return L7_FAILURE;
            }

            ese = &rmonEtherStatsEntryArray[array_index];


            if (ese->status == val)
                return L7_SUCCESS;
    } else {

        if (val == RMON_ETHER_STAT_STATUS_CREATEREQUEST) {
            rc1 = rmonMapMatch (etherStatMap, Index, &array_index);
           if (rc1 == L7_SUCCESS) {
              return L7_FAILURE;
           }

           rc1 = rmonMapInsert (etherStatMap, Index);
           if (rc1 != L7_SUCCESS) {
              return L7_FAILURE;
           }

           rc1 = rmonMapMatch (etherStatMap, Index, &array_index);
           if (rc1 != L7_SUCCESS) {
              return L7_FAILURE;
           }
           ese = &rmonEtherStatsEntryArray[array_index];

           /* for default values,take internal interface and datasource as 1 */
           rmonEtherStatsEntryDefaultsSet(ese, Index, 1,RMON_ETHER_STAT_DEFAULT_DATASOURCE,
                                          RMON_ETHER_STAT_STATUS_UNDERCREATION);
           return L7_SUCCESS;

      } else {
          return L7_FAILURE;
      }
  }


  switch (ese->status)
     {
      case RMON_ETHER_STAT_STATUS_VALID:
          switch (val) {
          case RMON_ETHER_STAT_STATUS_UNDERCREATION:
              ese->status = val;
              ese->validEntry = L7_TRUE;
              rc = L7_SUCCESS;
            break;
          case RMON_ETHER_STAT_STATUS_INVALID:
              rc1 =  rmonMapRemove (etherStatMap, Index);
              if (rc1 != L7_SUCCESS) {
                  return L7_FAILURE;
              }
              memset (ese, 0, sizeof (rmonEtherStatsEntry_t));
              rc = L7_SUCCESS;
            break;
          default:
             rc = L7_FAILURE;
          }
      break;

       case RMON_ETHER_STAT_STATUS_UNDERCREATION:
           switch (val) 
           {
           case RMON_ETHER_STAT_STATUS_VALID:
                        if((usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT,ese->dataSource)!= L7_SUCCESS))
                            return L7_FAILURE;
                        
                        ese->status = val;
                        ese->validEntry = L7_TRUE;
                        rc = L7_SUCCESS;
                     break;
           case RMON_ETHER_STAT_STATUS_INVALID:
                        rc1 =  rmonMapRemove (etherStatMap, Index);
                        if (rc1 != L7_SUCCESS) {
                            return L7_FAILURE;
                        }
                        memset (ese, 0, sizeof (rmonEtherStatsEntry_t));
                        rc = L7_SUCCESS;
                     break;
            default:
                        rc = L7_FAILURE;
                    }
                    break;
            }
    return rc;
}

/* End Function Declarations */
