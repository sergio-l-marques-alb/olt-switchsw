/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    garp_leaveall_timer.c
* @purpose     Implementaion of the GARP Leave All timer task
* @component   GARP
* @comments    none
* @create      02/21/2005
* @author      vrynkov
* @author      
* @end
*             
**********************************************************************/
                    
#include "garp_leaveall_timer.h"
#include "gidapi.h"


/* prototype for system and library functions */
extern int rand ();

extern avlTree_t         gvrpTreeData;

osapiTimerDescr_t   *pGarpLeaveAllTimerHolder;

static garpTimerStruct_t *garpTimer = L7_NULLPTR;


/* Number of interfaces where GVRP or GMRP, or both 
 * GVRP and GMRP, is/are enabled */
static L7_uint32 interfaces_used = 0;


/* Number of interfaces with GVRP enabled */
static L7_uint32 gvrp_used       = 0;


/* Number of interfaces with GMRP enabled */
static L7_uint32 gmrp_used       = 0;


extern void gid_leaveall_timer_expired(L7_uint32);
extern void gid_gmr_leaveall_timer_expired(L7_uint32, Gid *, Gmr *);

extern gmrpInstance_t *gmrpInstance;
extern GARPCBptr GARPCB;
extern GarpInfo_t garpInfo;

extern Gvr *gvrp_app;
extern void gid_leaveall(Gid *);
extern L7_RC_t isAttributeRegistered(Gid_machine *machine, 
                                     L7_int32  last_gid_used);

/*********************************************************************
* @purpose  Generate a random value divisible by GARP_TIMER_GRANULARITY_MS.
*
* @param    L7_uint32       leaveall_timeout
*
* @returns  L7_uint32       the random number divisible by 
*                           GARP_TIMER_GRANULARITY_MS
*
* @notes    generate a random number in the range 
*           leaveall_timeout .. (leaveall_timeout * GARP_LEAVE_ALL_TIMER_RANGE)
*           divisible by GARP_TIMER_GRANULARITY_MS.
*
* @end
*********************************************************************/
L7_uint32 GenerateLeaveAllTimeoutValue(L7_uint32 leaveall_timeout)
{
    L7_uint32   random_time;
    L7_ushort16 remainder;
    L7_ushort16 whole;
    L7_uint32   ceiling;


    /* get the high value for range */
    ceiling = leaveall_timeout * GARP_LEAVE_ALL_TIMER_RANGE;


    /* get the random value in the range (leaveall_timeout..ceiling) */
    random_time = 
        leaveall_timeout + (rand() % (ceiling - leaveall_timeout + 1));


    /* random_time should be divisible by GARP_TIMER_GRANULARITY_MS */
    remainder = random_time % GARP_TIMER_GRANULARITY_MS;


    /* the Leave All timers running on ports decrement in
     * GARP_TIMER_GRANULARITY_MS (which is a "time slice" of the 
     * Garp Leave All timer).  So we round the generated random_time to the 
     * nearest value divisible by GARP_TIMER_GRANULARITY_MS. */

    if (remainder > 0) 
    {
        if (remainder <= (GARP_TIMER_GRANULARITY_MS / 2)) 
        {   
            /* round by decreasing random_time.  Let's say the randomly-
             * generated random_time is 2105, and 
             * GARP_TIMER_GRANULARITY_MS is 250.  Then remainder 
             * is 105.  We deduct it from random_time, and get 
             * random_time = 2000.  random_time is now divisible by
             * the GARP Leave All Timer time slice 
             * (GARP_TIMER_GRANULARITY_MS) */
            random_time = random_time - remainder;
        }
        else
        {   
            /* round by increasing random_time.  Let's say the randomly-
             * generated random_time is 2170, and 
             * GARP_TIMER_GRANULARITY_MS is 250.  Then we round
             * random_timer to 2250, which is divisible by  
             * (ARP_LEAVEALL_TIMER_GRANULARITY_MS. */

            whole       = random_time / GARP_TIMER_GRANULARITY_MS;
            random_time = whole * GARP_TIMER_GRANULARITY_MS +
                          GARP_TIMER_GRANULARITY_MS;
        }
    }

    return random_time;
}


/*********************************************************************
* @purpose  Send a message to GARP Leave ALL Task.
*
* @param    garpTimerCommand_t command 
* @param    L7_uint32          port_no
* @param    GARPApplication    app 
* @param    L7_ushort16        vlan_id
* @param    L7_uint32          time
* @param    L7_uint32          leaveall_timeout
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    used by other GARP subsystems to send messages/commands 
*           to the GARP Leave All task (for example, create GVRP 
*           Leave All timer on port, restart GMRP Leave All timer,
*           print the value of GVRP and GMRP timers running on
*           port, etc.)
*           
* @end
*********************************************************************/
void garpUpdateLeaveAllTimer(garpTimerCommand_t command, 
                                L7_uint32       port_no,
                                GARPApplication app, 
                                L7_ushort16     vlan_id, 
                                L7_uint32       time, 
                                L7_uint32       leaveall_timeout)
{
    L7_uint32 garpLeaveAllPropagationTime;
    L7_uint32 leaveall_timeout_new;
    L7_BOOL   timer_deleted;

    /* process the command */
    switch (command)
    {
    case ADD_LEAVEALL_TIMER:
        /* we only increase the number of interfaces in use if neither
         * GVRP nor GMRP Leave All Timer is running on the port */
        if ((L7_FALSE == garpTimer[port_no].gvrp_enabled) &&
            (L7_FALSE == garpTimer[port_no].gmrp_enabled))
        {
            interfaces_used++;
        }

        if ((GARP_GVRP_APP == app) &&  
            (L7_FALSE == garpTimer[port_no].gvrp_enabled))
        {
            /* enabling GVRP */
            garpTimer[port_no].gvrp_enabled       = L7_TRUE;
            garpTimer[port_no].gvrp_leaveall_left = 
                               GenerateLeaveAllTimeoutValue(leaveall_timeout);
            gvrp_used++;
        } 
        else
        if (GARP_GMRP_APP == app) 
        {
            /* Leave All Timer value (gmrp_leaveall_left) for this instance of 
             * GMRP is alredy set in gid_create_gid(); here we just have to update
             * the internal structure 
             */
            if ((L7_FALSE == garpTimer[port_no].gmrp_enabled) ||
                (0 == garpTimer[port_no].gmrp_instances))
            {
                /* mark this port as one that enables GMRP */
                garpTimer[port_no].gmrp_enabled = L7_TRUE;

                /* increment the counter of ports with GMRP enabled */
                gmrp_used++;
            }

            /* increment the counter of GMRP instances on the port */
            ++(garpTimer[port_no].gmrp_instances);
        }

        garpTimer[port_no].leaveall_timeout = leaveall_timeout;
        garpTimer[port_no].port_no          = port_no;

        break;


    case UPDATE_LEAVEALL_TIMER:
        /* timer is changed if GVRP is enabled  */
        if (L7_TRUE == garpTimer[port_no].gvrp_enabled)  
        {
            garpTimer[port_no].leaveall_timeout = leaveall_timeout;

            /* re-set GVRP Leave All timer to a new value */
            garpTimer[port_no].gvrp_leaveall_left = 
                               GenerateLeaveAllTimeoutValue(leaveall_timeout);
        }

        break;

    case DELETE_TIMER:
        timer_deleted = L7_FALSE;

        if ((GARP_GVRP_APP == app) && 
            (L7_TRUE == garpTimer[port_no].gvrp_enabled))
        {
            /* "turn off" GVRP Leave All timer running on current port */
            garpTimer[port_no].gvrp_enabled       = L7_FALSE;
            garpTimer[port_no].gvrp_leaveall_left = 0;

            /* decrement the counter of GVRP Leave All timers */
            if (gvrp_used > 0)
            {
                gvrp_used--;
            }

            timer_deleted = L7_TRUE;
        }
        else 
        if ((GARP_GMRP_APP == app) && 
            (L7_TRUE == garpTimer[port_no].gmrp_enabled)) 
        {
            if (garpTimer[port_no].gmrp_instances > 0)
            {
               /* decerement the counter of GMRP Leave All timers
                * running on GMRP instances on the current port */
               --(garpTimer[port_no].gmrp_instances);

               if (0 == garpTimer[port_no].gmrp_instances) 
               {              
                   garpTimer[port_no].gmrp_enabled = L7_FALSE;

                  /* decrement the global system counter of GMRP Leave All
                   * timers (all timers running on the system) */
                  if (gmrp_used > 0)
                  {
                      gmrp_used--;
                  }
               }

               timer_deleted = L7_TRUE;
            }            
        }

        /* check if bot applications are disabled */
        if ((L7_FALSE == garpTimer[port_no].gvrp_enabled) && 
            (L7_FALSE == garpTimer[port_no].gmrp_enabled) &&
            (L7_TRUE  == timer_deleted))
        {
            /* the number of interfaces running either GVRP, or 
             * GMRP timer(s),  or both, decreases */
            if (interfaces_used > 0)
            {
                interfaces_used--;
            }
        }

        break;


    case RESTART_LEAVEALL_TIMER:
        /* calculate how long it took the Leave All command to propagate 
         * to this function */
        garpLeaveAllPropagationTime = osapiTimeMillisecondsGetOffset(time);

        /* get the new random value for Leave All timer */
        leaveall_timeout_new = 
            GenerateLeaveAllTimeoutValue(garpTimer[port_no].leaveall_timeout);

        /* check if the time to propagate RESTART_TIMER command to this
         * function is greater than the Leave All timeout value */
        if (garpLeaveAllPropagationTime < leaveall_timeout_new) 
        {
            /* no; adjust the new value for Leave All timeout 
             * by the propagation delay.  It is OK that leaveall_timeout_new
             * is not divisible by GARP_TIMER_GRANULARITY_MS
             * (see comments in GeneratetLeaveAllTimeoutValue()) -- 
             * garpLeaveAllUpdateGVRPTimers() or 
             * garpLeaveAllUpdateGMRPTimers() will handle this value
             * correctly. */
            leaveall_timeout_new = 
                leaveall_timeout_new - garpLeaveAllPropagationTime;
        }
        else
        {
            /* yes; Leave All Timer expired while RESTART_LEAVEALL_TIMER
             * was propagating.  In this unlikely event, Leave All
             * timer is set to 0. */
            leaveall_timeout_new = 0;
        }

        if ((GARP_GVRP_APP == app) && 
            (L7_TRUE == garpTimer[port_no].gvrp_enabled)) 
        {    
            /* set the new value for GVRP Leave All timer */
            garpTimer[port_no].gvrp_leaveall_left = leaveall_timeout_new;                                                             
        }

        break;

    default:
        break;

    } /* end of switch (command) */

    return;
}


/*********************************************************************
* @purpose  Prints information on timer running on a particular port.
*
* @param    L7_uint32 port_no
*
* @returns  void
*
* @notes   	Prints out the fields of garpTimer struct for
*           a particular port.  It also shows how many interfaces/ports 
*           are enabled (used), and how many GVRP and GMRP Leave All 
*           timers are currently running on the device, and how soon 
*           they will expire.  
*           For information/debugging purposes. 
*
* @end
*********************************************************************/
void garpPrintTimerInfo(L7_uint32 port_no)
{
    sysapiPrintf("\nFields of garpTimer[%u]:\n", port_no);

    sysapiPrintf("           port_no = %u \n", garpTimer[port_no].port_no);

    sysapiPrintf("  leaveall_timeout = %5u msec \n", 
            garpTimer[port_no].leaveall_timeout);

    sysapiPrintf("gvrp_leaveall_left = %5u msec\n", 
            garpTimer[port_no].gvrp_leaveall_left);

    sysapiPrintf("      gvrp_enabled = %1u \n", 
            garpTimer[port_no].gvrp_enabled);

    sysapiPrintf("      gmrp_enabled = %1u \n", 
             garpTimer[port_no].gmrp_enabled);

    sysapiPrintf("    gmrp_instances = %u\n", 
              garpTimer[port_no].gmrp_instances);                                                      
    
    sysapiPrintf("\nInterfaces/Applications:\n");
    sysapiPrintf("   interfaces_used = %u\n", interfaces_used);
    sysapiPrintf("         gvrp_used = %u\n", gvrp_used);
    sysapiPrintf("         gmrp_used = %u\n", gmrp_used);

    return;
}


/*********************************************************************
* @purpose  Prints information on GMRP Leave All timers running on
*           all instances of GMRP on the current port.
*
* @param    L7_uint32 port_no
* @param    L7_uint32 VLAN Id
*
* @returns  void
*
* @notes   	Similar to garpPrintTimerInfo(). 
*           For information/debugging purposes. 
*
* @end
*********************************************************************/
void gmrpPrintTimerInfo(L7_uint32 port_no, L7_uint32 vlan_Id)
{
    Gmr *my_gmr;
    Gid *first_port, *my_port;
    L7_uint32 vlanId, nextvlanId, index;

    vlanId     = 0;
    nextvlanId = 0;
    index      = 0;

    sysapiPrintf("\nFields of garpTimer[%u]:\n", port_no);

    sysapiPrintf("           port_no = %u \n", garpTimer[port_no].port_no);

    sysapiPrintf("      gmrp_enabled = %1u \n", 
             garpTimer[port_no].gmrp_enabled);

    sysapiPrintf("    gmrp_instances = %u\n", 
             garpTimer[port_no].gmrp_instances);

    sysapiPrintf("\nInterfaces/Applications:\n");
    sysapiPrintf("   interfaces_used = %u\n", interfaces_used);
    sysapiPrintf("         gvrp_used = %u\n", gvrp_used);
    sysapiPrintf("         gmrp_used = %u\n\n", gmrp_used);


    while (L7_SUCCESS == dot1qNextVlanGet(vlanId, &nextvlanId))
    {
        if (L7_SUCCESS == gmrpInstanceIndexFind(nextvlanId, &index))
        {
            my_gmr     = gmrpInstance[index].gmr;
            first_port = my_gmr->g.gid;           

            if ((Gid *)L7_NULL != first_port) 
            {  
                my_port = (Gid *)L7_NULL;

                if (L7_TRUE == gid_find_port((Gid*)first_port, 
                                             port_no, (void*)&my_port))
                {
                    sysapiPrintf(
                        "gmrp_leaveall_left = %5u msec VID = %u\n",  
                        my_port->gmrp_leaveall_left, nextvlanId);
                }
            }
        }

        vlanId = nextvlanId;
    }

    return;
}


/*********************************************************************
* @purpose  Create garpTimer structure.
*
* @param    void
*
* @returns  L7_NULLPTR
* @returns  garpTimerStruct_t *garpTimer
*
* @notes   	Creates and initializes garpTimer, where every
*           element points to a data structure describing GARP 
*           timer(s) running on a specific port.
*
* @end
*********************************************************************/
L7_RC_t garpLeaveAllTimerStructInit()
{

    garpTimer = 
        (garpTimerStruct_t *)osapiMalloc(L7_GARP_COMPONENT_ID, 
            sizeof(garpTimerStruct_t) * GARP_INTF_MAX_COUNT);

    if ((garpTimerStruct_t *)L7_NULLPTR == garpTimer)
    {
        LOG_MSG("garpTimerStructInit(): unable to allocate garpTimer, exiting.\n");
        return L7_FAILURE;
    }

    memset((void *)garpTimer, 0x00, 
           (sizeof(garpTimerStruct_t) * GARP_INTF_MAX_COUNT));

    return L7_SUCCESS;                
}

/*********************************************************************
* @purpose  Decrements GVRP Leave All timers on all enabled interfaces.
*
* @returns  void
*
* @notes   	Decrements GVRP Leave All timers on all enabled ports
*           by GARP_TIMER_GRANULARITY_MS.
*           Sends GVRP Leave All message if the timer expired.
*
* @end
*********************************************************************/
void static garpLeaveAllUpdateGVRPTimers()
{
    L7_uint32 i;

    for (i = 0; i < GARP_INTF_MAX_COUNT; i++) 
    {
        if (L7_TRUE == garpTimer[i].gvrp_enabled) 
        {
            /* check if the Leave All Timer is about to expire */
            if (garpTimer[i].gvrp_leaveall_left > GARP_TIMER_GRANULARITY_MS) 
            {  
                /* not yet -- decrement it by GARP_TIMER_GRANULARITY_MS */
                garpTimer[i].gvrp_leaveall_left -= GARP_TIMER_GRANULARITY_MS;
            }
            else
            {
                /* yes, Leave All timer expired.  Process the expiration event */
                gid_leaveall_timer_expired(garpTimer[i].port_no);

                /* and generate a new random value for the timer */
                garpTimer[i].gvrp_leaveall_left = 
                    GenerateLeaveAllTimeoutValue(
                        garpTimer[i].leaveall_timeout);
            }
        }
    }

    return;        
}

/*********************************************************************
* @purpose  Decrements GMRP Leave All timers on all enabled interfaces.
*
* @returns  void
*
* @notes   	Decrements GMRP Leave All timers on all enabled ports
*           by GARP_TIMER_GRANULARITY_MS.
*           Sends GMRP Leave All message if the timer expired.
*
* @end
*********************************************************************/
void static garpLeaveAllUpdateGMRPTimers()
{
    Gmr *my_gmr;
    Gid *my_port;
    Gid *this_port;
    L7_uint32 vlanId, nextvlanId, index;

    vlanId     = 0;
    nextvlanId = 0;
    index      = 0;


    while (L7_SUCCESS == dot1qNextVlanGet(vlanId, &nextvlanId))
    {
        if (L7_SUCCESS == gmrpInstanceIndexFind(nextvlanId, &index))
        {
            my_gmr    = gmrpInstance[index].gmr;
            my_port   = my_gmr->g.gid;
            this_port = my_port;

            if (L7_NULL != (Gid *)my_port) 
            {           
                do
                {
                    if ((L7_TRUE == my_port->is_enabled) && 
                        (L7_TRUE == my_port->is_connected)) 
                    {                   
                        if (my_port->gmrp_leaveall_left > GARP_TIMER_GRANULARITY_MS) 
                        {   
                            /* not yet -- decrement it by GARP_TIMER_GRANULARITY_MS */
                            my_port->gmrp_leaveall_left -= GARP_TIMER_GRANULARITY_MS;
                        }   
                        else
                        {
                            /*sysapiPrintf("Sending GMRP LeaveAll for VID=%u Port=%u\n", nextvlanId, my_port->port_no);*/

                            /* yes, Leave All timer expired.  Process the expiration event */
                            my_port->transmit_leaveall = L7_TRUE;
                            gid_gmr_leaveall_timer_expired(nextvlanId, my_port, my_gmr);
                                                    
                            /* and generate a new random value for the timer */
                            my_port->gmrp_leaveall_left = 
                                GenerateLeaveAllTimeoutValue(my_port->leaveall_timeout); 
                        }
                    }

                    my_port = my_port->next_in_port_ring;

                } while (this_port != my_port);
            }
        }

        vlanId = nextvlanId;
    }

    return;        
}

/*********************************************************************
* @purpose  Propagates the Leave All event to all attributes registered
*           on the port (causing the attributes to transition to Leaving 
*           state), and then calls gid_leave_timer_expired() to 
*           remove all dynamic attributes that are in Leaving state. 
*
* @param    L7_uint32 port_no 
*
* @returns  void
*
* @notes    When the "shutdown" command is issued for some port, 
*           all dynamic VLANs registered on that port have to be removed.
*           The easy and clean way to do it is to act as if GVRP Leave All
*           timer running on that port expired ("expire" the timer 
*           "prematurely").  
*           gid_leaveall() function causes Registrars of all 
*           attributes registered on the port to transition to 
*           Leaving state (Lv).
*           Normally, after that the Leave Timer is started;  here, however,
*           we don't need to wait to deregister attributes, so we just
*           call gid_leave_timer_expired() without starting Leave timer.
*           gid_leave_timer_expired() also sends LeaveEmpty messages
*           from all connected ports.
*
* @end
*********************************************************************/
void garpExpireGVRPLeaveAllTimer(L7_uint32 port_no)
{  
    Garp *application;
    Gid  *my_port;
    L7_uint32 gvrpMode;

    /* if gvrp mode is disabled, the leaveall timer expiry should not take effect */
    if ((garpGVRPEnableGet(&gvrpMode) == L7_SUCCESS) && (gvrpMode==L7_FALSE))
      return;

    application = &gvrp_app->g;

    if (L7_TRUE == garpIsValidIntf(port_no)) 
    {
        garpTimer[port_no].gvrp_leaveall_left = 
             GenerateLeaveAllTimeoutValue(garpTimer[port_no].leaveall_timeout);

        if (gid_find_port(application->gid, port_no, (void*)&my_port))
        {
            /* send the leave all to all machines */
            gid_leaveall(my_port);
        
            my_port->transmit_leaveall = L7_FALSE;
            my_port->cstart_join_timer = L7_FALSE;

            if (my_port->application->last_gid_used > 0)
            {
                if (L7_TRUE == isAttributeRegistered(my_port->machines, 
                                       my_port->application->last_gid_used-1)) 
                {
                    gid_leave_timer_expired(port_no);
                }
            }
        }
    }
       
    return;    
}


/*********************************************************************
* @purpose  Propagates the Leave All event to all attributes of all
*           GMRP instances registered on the port (causing the attributes
*           to transition to Leaving state), and then calls 
*           gid_gmr_leave_timer_expired() to remove all dynamic attributes 
*           that are in Leaving state. 
*
* @param    L7_uint32 port_no 
*
* @returns  void
*
* @notes    When the "shutdown" command is issued for some port, 
*           all GMRP instances on that port have to be removed.
*           The easy and clean way to do it is to act as if all GMRP Leave All
*           timers running on all GMRP instances on that port expired 
*           ("expire" the timers "prematurely").  
*           gid_leaveall() function causes Registrars of all 
*           attributes registered on a certain GMRP instance on the port 
*           to transition to Leaving state (Lv).
*           Normally, after that the Leave Timer is started;  here, however,
*           we don't need to wait to deregister attributes, so we just
*           call gid_leave_timer_expired() without starting Leave timer.
*           gid_leave_timer_expired() also sends LeaveEmpty messages
*           from all connected ports.
*
* @end
*********************************************************************/
void garpExpireGMRPLeaveAllTimers(L7_uint32 port_no)
{  
    Gmr *my_gmr;
    Gid *my_port;
    Gid *this_port;
    L7_uint32 vlanId, nextvlanId, index;
    L7_uint32 gmrpMode;

    /* if gmrp mode is disabled, the leaveall timer expiry should not take effect */
    if ((garpGMRPEnableGet(&gmrpMode) == L7_SUCCESS) && (gmrpMode==L7_FALSE))
      return;

    vlanId     = 0;
    nextvlanId = 0;
    index      = 0;

    if (L7_TRUE != garpIsValidIntf(port_no)) 
    {
        return;
    }

    while (L7_SUCCESS == dot1qNextVlanGet(vlanId, &nextvlanId))
    {
        if (L7_SUCCESS == gmrpInstanceIndexFind(nextvlanId, &index))
        {
            my_gmr    = gmrpInstance[index].gmr;
            my_port   = my_gmr->g.gid;
            this_port = my_port;

            if (L7_NULL != (Gid *)my_port) 
            {           
                do
                {
                    if (my_port->port_no == port_no) 
                    {               
                        my_port->transmit_leaveall = L7_FALSE;
                        my_port->cstart_join_timer = L7_FALSE;

                        gid_leaveall(my_port);

                        /* and generate a new random value for the timer */
                        my_port->gmrp_leaveall_left = 
                            GenerateLeaveAllTimeoutValue(my_port->leaveall_timeout);

                        if (my_port->application->last_gid_used > 0)
                        {
                            if (L7_TRUE == 
                                isAttributeRegistered(my_port->machines, 
                                       my_port->application->last_gid_used-1)) 
                            {
                                gid_gmr_leave_timer_expired(nextvlanId, port_no);
                            }
                        }

                        break;
                    }

                    my_port = my_port->next_in_port_ring;

                } while (this_port != my_port);
            }
        }

        vlanId = nextvlanId;
    }

    return;    
}

/*********************************************************************
* @purpose  Main Garp Leave All Timer Process Routine (driver).
*
* @param    void 
*
* @returns  L7_ERROR
*
* @notes   	Processes any leave all timers that have expired
*
* @end
*********************************************************************/
void garpLeaveAllTimerProcess()
{  
    L7_BOOL   gvrp_enabled_and_used, gmrp_enabled_and_used;

    /* check if GVRP is enabled and used. */
    /* gvrp_used shows how many interfaces enable GVRP, and
     * GARP_GVRP_IS_ENABLED shows if GVRP is enabled administratevily.
     * gvrp_enabled_and_used will be true if GVRP is enabled administratevily
     * and at least one interface has GVRP enabled. */
    gvrp_enabled_and_used = (gvrp_used > 0) && 
                            (L7_TRUE == GARP_GVRP_IS_ENABLED);


    /* check if GMRP is enabled and used. */
    /* gmrp_used shows how many interfaces enable GMRP, and
     * GARP_GMRP_IS_ENABLED shows if GMRP is enabled administratevily.
     * gmrp_enabled_and_used will be true if GMRP is enabled administratevily
     * and at least one interface has GMRP enabled. */
    gmrp_enabled_and_used = (gmrp_used > 0) && 
                            (L7_TRUE == GARP_GMRP_IS_ENABLED);


    /* if GVRP or GMRP is enabled (or both) and used */
    if ((interfaces_used > 0) && ((L7_TRUE == gvrp_enabled_and_used) || 
                                  (L7_TRUE == gmrp_enabled_and_used)))
    {
        if (L7_TRUE == gvrp_enabled_and_used)
        {   
            /* GVRP is enabled and used -- update GVRP Leave All timers
             * (and process expired GVRP Leave All timers -- if any) */
            garpLeaveAllUpdateGVRPTimers();
        }

        if (L7_TRUE == gmrp_enabled_and_used)
        {
            /* GMRP is enabled and used -- update GMRP Leave All timers                  
             * (and process expired GMRP Leave All timers -- if any) */ 
            garpLeaveAllUpdateGMRPTimers();
        }

    }
}



/*********************************************************************
*
* @purpose  osapi timer callback function for GARP
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*
*********************************************************************/
void garpLeaveAllTimerCallback(void)
{
  GarpPacket    msg;
  L7_BOOL       restart_timer;

  restart_timer  = L7_FALSE;

  /* If either GARP application is enabled, restart the timer.
     Else, if all GARP applications are disabled, wait until all learned GARP entries 
     are deleted before the timer expires.

   */  


   if ((GARP_GVRP_IS_ENABLED == L7_TRUE) || (GARP_GMRP_IS_ENABLED == L7_TRUE))
   {          
     restart_timer  = L7_TRUE;
   }
   else 
   {
       /* Determine if there are any outstanding GMRP or GVRP entries which
          have not been purged.  If so, keep firing the leaveall timer until 
          they have been purged */

       if (avlTreeCount(&gvrpTreeData) > 0)
       {
           /* Note: Alternate check: getNumberOfGVDEntries() */
           restart_timer  = L7_TRUE;
       }
       else if (garpInfo.infoCounters.gmrpRegistationCount > 0) 
       {
           restart_timer  = L7_TRUE;
       }
       else
       {
           restart_timer  = L7_FALSE;
		   pGarpLeaveAllTimerHolder = L7_NULLPTR;
           GARP_TRACE_TIMER_EVENTS("%s:  Stopping leaveall timers\n", __FUNCTION__);
       }

       /* Debug:  Count the number of times the leaveall timer expires while garp is disabled */
       if ((GARP_GVRP_IS_ENABLED == L7_FALSE) && (GARP_GMRP_IS_ENABLED == L7_FALSE))
       {          
           garpInfo.infoCounters.leaveAllTimerExpiriesAfterGARPDisable++;
       }

   }

   /* Always send a timer expiry message */
   memset((void *)&msg, 0, sizeof(msg));
   msg.msgId = G_LEAVEALL_TIMER;

   garpTraceQueueMsgSend(&msg);

   /* Callbacks from the timer are sent as L7_NO_WAIT to avoid backing up the osapiTimerTask
      if the application has an issue.  However, restart the timer if there is a problem
      to ensure that the application services any pending timeouts when it can */
   if (osapiMessageSend(garpQueue, (void*)&msg, sizeof(msg),L7_NO_WAIT, L7_MSG_PRIORITY_NORM) 
       != L7_SUCCESS)
   {
       L7_LOG(L7_LOG_SEVERITY_WARNING, L7_GARP_COMPONENT_ID,
           "garpTimerCallback(): unable to send message to GARP queue."
           " The garpQueue is full, logs specifics of the message"
           " content like internal interface number, type of message etc");
       restart_timer  = L7_TRUE;
   }
   else
   {
       osapiSemaGive(garpInfo.msgQSema);
   }

   if (restart_timer == L7_TRUE) 
   {
       osapiTimerAdd ( (void*)garpLeaveAllTimerCallback,L7_NULL,L7_NULL,
                       GARP_TIMER_GRANULARITY_MS,&pGarpLeaveAllTimerHolder );
   }
   else
   {
       /* No timer is running */
       garpInfo.leaveAllTimersRunning = L7_FALSE;
   }


}

