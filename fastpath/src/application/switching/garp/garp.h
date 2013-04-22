/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    garpapi.h
* @purpose     GARP internal api functions to the applications
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author      
* @end
*             
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
#ifndef INCLUDE_GARP_API_H_
#define INCLUDE_GARP_API_H_

#include "l7_common.h"
#include "garpcfg.h"
#include "garpapi.h"
#include "component_mask.h"
   
/******************************************************************************
 * GARP : GENERIC ATTRIBUTE REGISTRATION PROTOCOL : COMMON APPLICATION ELEMENTS
 ******************************************************************************
 */


typedef struct /* Garp */
{  /*
	* Each GARP, i.e. each instance of an application which uses the GARP
	* protocol, is represented as a struct or control block with common initial
	* fields.  These comprise pointers to application specific functions which
	* are by the GID and GIP components to signal protocol events to the
	* application and other controls common to all applications, together with
	* a pointer to the instances of GID (one per port) for the application, and
	* to GIP (one per application). The signalling functions include the
	* addition and removal of ports, which the application should use to
	* initialise port attributes with any management state required.
	*/

	L7_int32  process_id;

	void       *gid;

	/*L7_uchar8 gip[L7_MAX_VLANS];*/
    L7_uint32  *gip;  /*this gip is common to all GARP Applications- GVRP and GMRP*/

	L7_int32  max_gid_index;

	L7_int32  last_gid_used;

	void	(*join_indication_fn)( void *, void *my_port, L7_uint32 joining_gid_index);
	void	(*leave_indication_fn)(void *, void *gid,
										   L7_uint32 leaving_gid_index);
	void	(*join_propagated_fn)( void *, void *gid,
										   L7_uint32 joining_gid_index);
	void	(*leave_propagated_fn)(void *, void *gid,
										   L7_uint32 leaving_gid_index);

	void	(*transmit_fn)(        void *, void *gid);
	void    (*receive_fn)(         void *, void *gid, GarpPdu *pdu);

	void    (*added_port_fn)(      void *, L7_uint32 port_no);
	void    (*removed_port_fn)(    void *, L7_uint32 port_no);

	GARPApplication app;
	L7_ushort16 vlan_id;/*used only by gmrp*/
} Garp;



typedef struct GarpInfoCounters_s /* Garp */
{  	
    L7_int32   gmrpRegistationCount;    /*  Count of multicast addresses kept by GMRP */ 
    L7_int32   gmrpRxPdusNotProcessed;  /*  Count of received PDUs not processed for various reasons */ 
    L7_uint32  gmrpInstanceForcedUpdates;      /* for debug only */    


    L7_int32   timerExpiriesAfterGARPDisable;  /*  Debug: Determine how long it takes to purge tables
                                                          after disabling both protocols */ 

    L7_int32   leaveAllTimerExpiriesAfterGARPDisable;  /*  Debug: Determine how long it takes to purge tables
                                                          after disabling both protocols */ 

    L7_uint32   garpPDUsReceived;           /* for debug only */    
    L7_uint32   gvrpPDUsReceived;           /* for debug only */    
    L7_uint32   gmrpPDUsReceived;           /* for debug only */    


} GarpInfoCounters_t;

typedef struct GarpInfo_s /* Garp */
{  	
    L7_int32  msgsRecvdOnQueue;
    L7_int32  queueUsageOverFiftyPercent;
    L7_int32  queueUsageOverEightyPercent;
    L7_int32  queueUsageOverNinetyPercent;

    /* Counting semaphore. Semaphore is given every time a message is added to the
      * garpQueue or garpPduQueue. The processing task waits on this 
      * semaphore before reading from the queues. */
    void           *msgQSema;

    AcquiredMask  acquiredList[L7_MAX_INTERFACE_COUNT+1]; /* bit mask of components that have
                                                           acquired the interface */
    GarpInfoCounters_t   infoCounters;    /*  Count of multicast addresses kept by GMRP */ 

    L7_BOOL     timersRunning;                  /* garp osapi timers are running */
    L7_BOOL     leaveAllTimersRunning;          /* garp osapi timers are running */



} GarpInfo_t;



#endif
