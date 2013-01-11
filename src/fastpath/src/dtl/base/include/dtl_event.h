/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dtl_event.h
*
* @purpose This header file contains definitions to support the       
*          event handler engine (callback)      
*
* @component Device Transformation Layer
*
* @comments none
*
* @author   Shekhar Kalyanam 3/14/2001
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_DTLCTRL_EVENT_H
#define INCLUDE_DTLCTRL_EVENT_H

/*
***********************************************************************
*                           CONSTANTS
***********************************************************************
*/



/*
***********************************************************************
*                           DATA TYPES
***********************************************************************
*/
typedef enum
{
  IsActive,
  IsSuspended
} EVENT_STATE_t, *pEVENT_STATE_t ;


/*
**********************************************************************
*                           GLOBAL DATA
**********************************************************************
*/
#ifdef  DTLCTRL_EVNT_GLOBALS3
  #define DTLCTRL_EVNT_EXT
#else 
  #define DTLCTRL_EVNT_EXT extern
#endif  /*DTLCTRL_EVNT_GLOBALS3*/




/*
***********************************************************************
*                       FUNCTION PROTOTYPES -- DTL INTERNAL USE ONLY
***********************************************************************
*/
/*********************************************************************
* @purpose  Change callback based on event puts it in a message queue 
* @purpose  and releases the interrupt thread
*
* @param    ddusp          @b{(input)}device driver reference to unit slot and port
* @param	  family         @b{(input)}Device Driver family type 
* @param    cmd            @b{(input)}command of type DAPI_CMD_t
* @param    event          @b{(input)}event of type DAPI_EVENT_t 
* @param    dapiEventInfo  @b{(input)}pointer to dapiEvent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlLinkChangeCallback(DAPI_USP_t *ddusp, 
                                    DAPI_FAMILY_t family, 
                                    DAPI_CMD_t cmd, 
                                    DAPI_EVENT_t event,
                                    void *dapiEventInfo) ;


/*********************************************************************
* @purpose  Change callback based on event
*
* @param    ddusp          @b{(input)}device driver reference to unit slot and port
* @param	  family         @b{(input)}Device Driver family type 
* @param    cmd            @b{(input)}command of type DAPI_CMD_t
* @param    event          @b{(input)}event of type DAPI_EVENT_t 
* @param    dapiEventInfo  @b{(input)}pointer to dapiEvent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlLinkChange(DAPI_USP_t *ddusp, 
                      DAPI_FAMILY_t family, 
                      DAPI_CMD_t cmd, 
                      DAPI_EVENT_t event,
                      void *dapiEventInfo);

/*

#ifdef __cplusplus


// The Receiver class knows how to perform the operations associated
// with carrying out a request. 
// (Design Pattern: Chain of Responsibility)
class Receiver {

public:
    // Constructor
    Receiver (L7_component_id_t cid, L7_VOIDFUNCPTR_t fp);

    // Method that delegates out a request [Action()]
    virtual void Action();

    // Identifies me (the receiver)
    virtual L7_component_id_t whoAmI { return myComponent; }

    //Destructor
    virtual ~Receiver();
private:
    L7_component_id_t myComponent;       // My (Receiver) ID
    L7_VOIDFUNCPTR_t pRcvrAction;        // carries out the request   

}; // end class Receiver


// The Event class declares an interface for executing an operation.
class Event {

public:
    // Destructor
    virtual ~Handler();

    // This method carries out the invoker request by calling the 
    // Receiver's action method.
    virtual void Execute() = 0;
protected:
    // Constructor
    Handler();

};  // end class Handler

// The Event defines a binding between a Receiver object and
// a action.  This class implements Execute() by invoking the
// corresponding operations on Receiver.
class ConcreteEvent : public Handler {
public:
    // Constructor
    ConcreteEvent(Receiver *r , DTLCTRL_EVENT_t e)
    : pReceiver(r), dtlCtrlEventID(e), eventState(IsActive) 
    {
    }

    // Carries out operation requested by the Invoker
    virtual void Execute();        // Execute()

    // My identification
    virtual DTLCTRL_EVENT_t EventIDIs() { return dtlCtrlEventID; }

    // My Receiver
    virtual Receiver * ReceiverIs() { return pReceiver; } 
private:
    Receiver *pReceiver;                 // The receiver of this event 
    DTLCTRL_EVENT_t dtlCtrlEventID;      // Event ID
    EVENT_STATE_t eventState;            // This Event State. Default
                                         // value IsActive. 

};  // end class ConcreteEvent

// The Invoker class asks the Event to carry out the request. The
// client instantiates the invoker. A client type is
// L7_component_id_t.
class Invoker : public Handler {

public:
    // Constructor
    Invoker (Event * e, L7_component_id_t cid);

    // Method that ask the Event to carry out a request
    virtual void execute();

    // Identifies me (the invoker)
    virtual L7_component_id_t whoAmI() { return myComponent;}

    // My Event is 
    virtual Event * EventIs() { return pEvent; }
    }

    //Destructor
    virtual ~Invoker();
private:
    // My (Invoker)  identity
    L7_component_id_t myComponent;
    Event *pEvent;

}; // end class Invoker 

class MacroInvoker : public Handler {

public:
    // Constructor
    MacroInvoker ();

    virtual void Add(Handler *);
    virtual void Remove(Handler *);
    virtual void Find(Handler *);

    // Method that ask the invokers to carry out a request
    virtual void execute();

    //Destructor
    virtual ~MacroInvoker();
private:

}; // end class MacroInvoker 

#endif  */ /* __cplusplus */

#endif /* INCLUDE_DTLCTRL_EVENT_H */
