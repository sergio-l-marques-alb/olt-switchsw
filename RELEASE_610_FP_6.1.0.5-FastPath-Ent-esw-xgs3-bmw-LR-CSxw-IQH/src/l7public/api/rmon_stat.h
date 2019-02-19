
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_stat.h
*
* @purpose  header file for RMON statistics group
*           
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


 
#ifndef RMON_STAT_H
#define RMON_STAT_H

#include <l7_common.h>
#include <stdio.h>

#include <string.h>

/* Header file for system defines,like max no. of interfaces, based on specific hardware        */

#include "l7_product.h"

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
rmonEtherStatsEntryGet(L7_uint32 Index);


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
rmonEtherStatsEntryNextGet(L7_uint32 *Index);


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
rmonEtherStatsInternalInterfaceNumGet (L7_uint32 Index, L7_uint32 *val);


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
L7_RC_t
rmonEtherStatsInterfaceIndexGet(L7_uint32 ifIndex, L7_uint32 *val);


/* GET calls for EtherStats Entries  */


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
rmonEtherStatsDataSourceGet (L7_uint32 Index, L7_uint32 *val); 


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
rmonEtherStatsDropEventsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                            L7_uint32 *val);
 

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
rmonEtherStatsOctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                        L7_uint32 *val);


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
rmonEtherStatsPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, L7_uint32 *val);



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
rmonEtherStatsBroadcastPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                               L7_uint32 *val);


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
rmonEtherStatsMulticastPktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                               L7_uint32 *val);


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
                                L7_uint32 *val);


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
rmonEtherStatsUndersizePktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                               L7_uint32 *val);


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
rmonEtherStatsOversizePktsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                              L7_uint32 *val);


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
rmonEtherStatsFragmentsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                           L7_uint32 *val);
 

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
rmonEtherStatsJabbersGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                         L7_uint32 *val);


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
rmonEtherStatsCollisionsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                            L7_uint32 *val);


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
rmonEtherStatsPkts64OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                              L7_uint32 *val);


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
rmonEtherStatsPkts65to127OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                                   L7_uint32 *val);


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
rmonEtherStatsPkts128to255OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                                    L7_uint32 *val);

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
rmonEtherStatsPkts256to511OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                                    L7_uint32 *val);

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
rmonEtherStatsPkts512to1023OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                                     L7_uint32 *val);

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
rmonEtherStatsPkts1024to1518OctetsGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 intIfIndex, 
                                      L7_uint32 *val);

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
rmonEtherStatsOwnerGet (L7_uint32 Index, L7_char8 *buf);


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
rmonEtherStatsStatusGet (L7_uint32 Index, L7_uint32 *val);


/* SET calls for EtherStats Entries  */


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
rmonEtherStatsDataSourceSet( L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val);


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
rmonEtherStatsOwnerSet (L7_uint32 Index, L7_char8 *buf);


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
rmonEtherStatsStatusSet (L7_uint32 Index, L7_uint32 val);


#endif  /* RMON_STAT_H */
