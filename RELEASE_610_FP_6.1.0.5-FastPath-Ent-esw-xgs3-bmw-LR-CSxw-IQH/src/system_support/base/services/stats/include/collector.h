/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename     collector.h
*
* @purpose      The purpose of this file is to define the Counters and the
*		        Collector Class.
*
* @component    Statistics Manager
*
* @comments     none
*
* @create       10/08/2000
*
* @author       Archana Suthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_COLLECTOR_H
#define INCLUDE_COLLECTOR_H

/*
***********************************************************************
*                           CONSTANTS
***********************************************************************
*/

const L7_ushort16 MAX_MUTLING = 16;	 // This represents the maximum 
									 // number of mutlings allowable
									 // for a mutant counter


/*
***********************************************************************
*                           TYPES
***********************************************************************
*/

// A keyPair consists of a counter key and a counter id
typedef pair<L7_uint32, counter_id_t> keyPair;

/*
***********************************************************************
*                           COUNTER CLASS
***********************************************************************
*/

/*
**********************************************************************
*
* @class        Counter
*
* @purpose      This is the most elementary class in the Statistics Manager. 
*		        It is intrinsic in the sense it is only capable of doing 
*		        very basic functions. This is also the base class of the 
*		        Local, Mutant and External Counters
*          
* @attribute    L7_ulong64        value  
* @attribute    counter_size_t    size
* @attribute    L7_BOOL			  isResettable
* @attribute    L7_ulong64		  delta
*  
* @member       get Returns the value of the counter (virtual function)
* @member       set Sets the value of the counter (virtual function)
* @member	    increment Increments the value of a LocalCounter (virtual function)
* @member	    decrement Decrements the value of a LocalCounter (virtual function)
* @member	    create  Creates a counter (virtual function)
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

class Counter
{
protected:
	L7_uint32 cKey;             // counter key               
    counter_id_t cId;           // counter id
    Counter64 value;	        // This represents the current value of the 
								// counter
	counter_size_t size;		// This represents the size of the counter
	L7_BOOL isResettable;		// This checks if the counter can be reset
	Counter64 delta;	        // The difference in counter value when
								// the user asks for a reset

public:
	Counter(pStatsParm_entry_t c);   // Constructor

	virtual ~Counter();				   // Destructor

  Counter *nextPtr;

/*
**********************************************************************
*
* @function     getId()
*
* @purpose      This function returns the counter Id
*          
* @parameter	None
*
* @return		None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
    counter_id_t getId();
/*
**********************************************************************
*
* @function     getKey()
*
* @purpose      This function returns the counter key
*          
* @parameter    None
*
* @return       None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
    L7_uint32 getKey();


/*
**********************************************************************
*
* @function     getType()
*
* @purpose      This function returns the counter type
*          
* @parameters   None
*
* @returns	    None
*
* @notes        Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
    virtual counter_type_t getType() const = 0;

/*
**********************************************************************
*
* @function     clear()
*
* @purpose      This function clears the counter i.e. sets its initial value
*               to zero when created.  
*          
* @parameter    None
*
* @return	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

	virtual void clear();
/*
**********************************************************************
*
* @function     get()
*
* @purpose      This function allows the client to get the value associated
*		        with a counter  
*          
* @parameter    pCounterValue_entry_t   pInfo
*
* @return       None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

	virtual void get(pCounterValue_entry_t pInfo);

/*
**********************************************************************
*
* @function     set()
*
* @purpose      This function allows the client to reset the value associated
*		        with a counter  
*          
* @parameter    pCounterValue_entry_t   pInfo
*
* @return 	    None
* 
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
	virtual void set(pCounterValue_entry_t pInfo);
/*
**********************************************************************
*
* @function     increment()
*
* @purpose      This function allows the client to increment the value 
*               associated with a Localcounter  
*          
* @parameter    pCounterValue_entry_t pInfo
*
* @return	    None
*          
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

	virtual void increment(pCounterValue_entry_t pInfo);

/*
**********************************************************************
*
* @function     decrement()
*
* @purpose      This function allows the client to decrement the value 
*               associated with a Localcounter  
*          
* @parameter    pCounterValue_entry_t pInfo
*
* @return 	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
	virtual void decrement(pCounterValue_entry_t pInfo);

};

// The map_t consists of a keyPair (counter key + counter id)
// and a pointer to a Counter
// The Collector's flyweight is of type map_t
// In the C++ Standard library all map searches are essentially
// tree searches
typedef map<keyPair, Counter*> map_t;

// A signedCounter consists of a sign (positive or negative)
// and a pointer to a Counter. All mutlings are signedCounters
typedef struct signedCounter_s
{
  counter_sign_t sign;
  Counter        *pCtr;

} signedCounter;

/*
/------------------------------------------------------------------\
*                 END OF COUNTER DEFINITION                        *
\------------------------------------------------------------------/
*/

/*
***********************************************************************
*                          EXTERNAL COUNTER CLASS
***********************************************************************
*/

/*
**********************************************************************
*
* @class        ExternalCounter
*
* @purpose      This class inherits all attributes and methods from the Counter 
*	            class. These counters by definition obtain their values from an 
*		        external source.
*          
* @attribute    L7_VOIDFUNCPTR_t		 source
*  
* @member       get Returns the value of the counter 
* @member       set Sets the value of the counter 
*
* @notes        Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

class ExternalCounter : public Counter
{
private:
	counter_type_t cType;
    L7_VOIDFUNCPTR_t  source; // This represents a pointer to the source of the 
						  // counter
public:
	ExternalCounter(pStatsParm_entry_t);	   // Constructor


	virtual ~ExternalCounter();				   // Destructor

/*
**********************************************************************
*
* @function     getType()
*
* @purpose      This function returns the counter type
*          
* @parameter    None
*
* @return	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
    virtual counter_type_t getType() const;

/*
**********************************************************************
*
* @function     get()
*
* @purpose      This function allows the client to get the value associated
*		        with a counter  
*          
* @parameter    pCounterValue_entry_t   pInfo
*
* @return	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

	virtual void get(pCounterValue_entry_t pInfo);

/*
**********************************************************************
*
* @function     set()
*
* @purpose      This function allows the client to reset the value associated
*		        with a counter  
*          
* @parameter	pCounterValue_entry_t pInfo
*
* @return       None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

	virtual void set(pCounterValue_entry_t pInfo);


};  

/*
/------------------------------------------------------------------\
*         END OF EXTERNAL COUNTER DEFINITION                       *
\------------------------------------------------------------------/
*/



/*
***********************************************************************
*                           LOCAL COUNTER CLASS
***********************************************************************
*/

/*
**********************************************************************
*
* @class       LocalCounter
*
* @purpose     This class inherits all attributes and methods from the Counter 
* 		       class. These counters are used internally by the client to 
*		       maintain statistical data and may be incremented or decremented 
*		       when required.
*
* @attribute
*  
* @member      get Returns the value of the counter (virtual function)
* @member	   set Sets the value of the counter (virtual function)
* @member	   increment Increments the value of a LocalCounter (virtual function)
* @member	   decrement Decrements the value of a LocalCounter (virtual function)
*
* @note        Implemented in src\application\stats\private\collector.cpp
*
*
**********************************************************************
*/

class LocalCounter : public Counter
{
private:
    counter_type_t cType;

public:
	
    LocalCounter(pStatsParm_entry_t);	   // Constructor            

	virtual ~LocalCounter();			   // Destructor

/*
**********************************************************************
*
* @function     getType()
*
* @purpose      This function returns the counter type
*          
* @parameter    None
*
* @return	    None
*
* @note          Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
    virtual counter_type_t getType() const;

/*
**********************************************************************
*
* @function     increment()
*
* @purpose      This function allows the client to increment the value 
*               associated with a Localcounter  
*          
* @parameter    pCounterValue_entry_t   pInfo
*
* @return	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

	virtual void increment(pCounterValue_entry_t pInfo);
/*
**********************************************************************
*
* @function     decrement()
*
* @purpose      This function allows the client to decrement the value 
*               associated with a Localcounter  
*          
* @parameter    pCounterValue_entry_t   pInfo
*
* @return	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
	virtual void decrement(pCounterValue_entry_t pInfo);
};  

/*
/------------------------------------------------------------------\
*         END OF LOCAL COUNTER DEFINITION                          *
\------------------------------------------------------------------/
*/


/*
***********************************************************************
*                          MUTANT COUNTER CLASS
***********************************************************************
*/

/*
**********************************************************************
*
* @class        Counter
*
* @purpose      This class inherits all its attributes from the Counter class. 
*		        It is called mutant as such a counter may be derived from two
*		        or more counters.
*          
* @attribute    set_t mSet
*  
* @member       get Returns the value of the counter 
* @member	    set Sets the value of the counter 
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
**********************************************************************
*/

class MutantCounter : public Counter
{
private:
    counter_type_t cType;
	signedCounter mSet[STATS_MAX_MUTLING];

/*
**********************************************************************
*
* @function     getMutlings()
*
* @purpose      This function allows the calling function to get the 
*		        sum of the values of the mutlings of a mutant counter  
*          
* @parameter    pCounterValue_entry_t    c
* @parameter    pCounterValue_entry_t    p
*
* @return       L7_ulong64
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

	L7_ulong64 getMutlings(pCounterValue_entry_t c, pCounterValue_entry_t p);

/*
**********************************************************************
*
* @function     resetMutlings()
*
* @purpose      This function allows the calling function to reset the 
*		        mutlings of a mutant counter  
*          
* @parameter    pCounterValue_entry_t   c
* @parameter    pCounterValue_entry_t   p
* @parameter    L7_ulong64				v
*
* @return       None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
	void resetMutlings(pCounterValue_entry_t c, pCounterValue_entry_t p);


public:
	MutantCounter(pStatsParm_entry_t, signedCounter *);	// Constructor

	virtual ~MutantCounter();							// Destructor


/*
**********************************************************************
*
* @function     getType()
*
* @purpose      This function returns the counter type
*          
* @parameter    None
*
* @return       None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
    virtual counter_type_t getType() const;

/*
**********************************************************************
*
* @function     get()
*
* @purpose      This function allows the client to get the value associated
*		        with a counter  
*          
* @parameter    pCounterValue_entry_t pInfo
*
* @return       None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

	virtual void get(pCounterValue_entry_t pInfo);

/*
**********************************************************************
*
* @function     set()
*
* @purpose      This function allows the client to reset the value associated
*		        with a counter  
*          
* @parameter    pCounterValue_entry_t pInfo
*
* @return       None
* 
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
**********************************************************************
*/
	virtual void set(pCounterValue_entry_t pInfo);

/*
**********************************************************************
*
* @function     addMutlings()
*
* @purpose      This function allows the client to dynamically add 
*               mutlings to a mutantCounter
*          
* @parameter    pStatsParm_entry_t pM
*
* @return       None
*
* @end
*
**********************************************************************
*/
	
    void addMutlings(pStatsParm_entry_t pM, signedCounter *mutlings);


/*
**********************************************************************
*
* @function     delMutlings()
*
* @purpose      This function allows the client to dynamically delete
*               mutlings from a mutantCounter
*          
* @parameter    pStatsParm_entry_t pM
*
* @return       None
*
* @end
*
**********************************************************************
*/
	
    void delMutlings(pStatsParm_entry_t pM, signedCounter *mutlings);

};  

/*
/------------------------------------------------------------------\
*            END OF MUTANT COUNTER DEFINITION                      *
\------------------------------------------------------------------/
*/



/*
***********************************************************************
*                           COLLECTOR CLASS
***********************************************************************
*/

/*
**********************************************************************
*
* @class        Collector
*
* @purpose      The Collector is the heart of the Statistics Manager and is 
*		        responsible for the main part of its functionality
*
* @attribute    Counter *pCtr
* @attribute  	map_t flyweight     
*  
* @member       get Returns the value of the counter (virtual function)
* @member	    set Sets the value of the counter (virtual function)
* @member	    increment Increments the value of a LocalCounter (virtual function)
* @member	    decrement Decrements the value of a LocalCounter (virtual function)
*
* @notes        Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

class Collector
{
private:
	Counter *pCtr;			// This is a pointer to the current
							// counter
	map_t flyweight;		// This is a pointer to the 
							// Collector's counterflyweight
  
  /* 
   * Free lists for each of the counter types 
   * Instead of actually freeing the memory when a counter must be deleted
   * the pointer will be added to the appropriate free list
   */
  Counter *pMutantCtrFreeList;
  Counter *pExternalCtrFreeList;
  Counter *pLocalCtrFreeList;
/*
**********************************************************************
*
* @function     createMutlingList()
*
* @purpose      This function allows the calling function to create an 
*		        array of mutlings  
*          
* @parameter    pStatsParm_entry_t   c
* @parameter    signedCounter        *m
*
* @return 	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

	void createMutlingList (pStatsParm_entry_t c, signedCounter *m);


public:
	Collector() {pCtr = pMutantCtrFreeList = pExternalCtrFreeList = pLocalCtrFreeList = 0;}		// Constructor

	virtual ~Collector();			   // Destructor


/*
**********************************************************************
*
* @function     get()
*
* @purpose      This function allows the client to get the value associated
*		        with a counter  
*          
* @parameter    pCounterValue_entry_t pCtrValInfo
*
* @return 	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

	void get (pCounterValue_entry_t pCtrValInfo);

/*
**********************************************************************
*
* @function     reset()
*
* @purpose      This function allows the client to reset the value associated
*		        with a counter  
*          
* @parameter    pCounterValue_entry_t  pCtrStatusInfo
*
* @return 	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
	void reset(pCounterValue_entry_t pCtrStatusInfo);

/*
**********************************************************************
*
* @function     increment()
*
* @purpose      This function allows the client to increment the value 
*               associated with a Localcounter  
*          
* @parameter    pCounterValue_entry_t  pCtrIncrInfo
*
* @return 	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

	void increment(pCounterValue_entry_t pCtrIncrInfo);

/*
**********************************************************************
*
* @function     decrement()
*
* @purpose      This function allows the client to decrement the value 
*               associated with a Localcounter  
*          
* @parameter    pCounterValue_entry_t  pCtrDecrInfo
*
* @return 	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/

	void decrement(pCounterValue_entry_t  pCtrDecrInfo);

/*
**********************************************************************
*
* @function     create()
*
* @purpose      This function allows the client to create a new counter by 
*		        specifying the appropriate parameters  
*          
* @parameter    pStatsParm_entry_t pCtrCreateInfo
*
* @return 	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
	void create(pStatsParm_entry_t pCtrCreateInfo);

/*
**********************************************************************
*
* @function     addMutlings()
*
* @purpose      This function allows the client to dynamically add mutlings
*               to a mutantCounter
*
* @parameter    pStatsParm_entry_t pStatsParmEntry
*
* @return 	    None
*
*
* @end
*
**********************************************************************
*/
    void addMutlings(pStatsParm_entry_t pStatsParmEntry);


/*
**********************************************************************
*
* @function     delMutlings()
*
* @purpose      This function allows the client to dynamically delete mutlings
*               to a mutantCounter
*
* @parameter    pStatsParm_entry_t pStatsParmEntry
*
* @return 	    None
*
*
* @end
*
**********************************************************************
*/
    void delMutlings(pStatsParm_entry_t pStatsParmEntry);

/*
**********************************************************************
*
* @function     counterDelete()
*
* @purpose      This function allows the client to delete a new counter by 
*		        specifying the appropriate parameters  
*          
* @parameter    pStatsParm_entry_t pCtrCreateInfo
*
* @return 	    None
*
* @note         Implemented in src\application\stats\private\collector.cpp
*
* @end
*
**********************************************************************
*/
void counterDelete(pStatsParm_entry_t pCtrCreateInfo);

}; 


/*
/------------------------------------------------------------------\
*            END OF COLLECTOR DEFINITION                           *
\------------------------------------------------------------------/
*/


#endif	  // INCLUDE_COLLECTOR_H



