/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gidtt.c
* @purpose     GID transition tables defintion and function
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
#include "gidtt.h"

/* For Debug*/
#include "gidapi.h"
#include "garp_debug.h"
#include "gvd.h"
/* for tracking transitions for a single Vlan */

extern L7_BOOL   bTrackVlan;
extern L7_BOOL   vlanFnd;

/******************************************************************************
 * GIDTT : GID PROTOCOL TRANSITION TABLES : IMPLEMENTATION OVERVIEW
 ******************************************************************************
 */
/* This implementation of GID uses transition tables directly. This makes the
 * the implementation appear very bulky, but the net code size impact may be
 * less than the page of code required for an algoritmic based implementation,
 * depending on the processor. A processing based implementation is planned as
 * both alternatives may be interesting.
 *
 * The Applicant and the Registrar use separate transition tables. Both use
 * a general transition table to handle most events, and separate smaller
 * tables to determine behavior when a transmit opportunity occurs (both
 * Applicant and Registrar), and when the leave timer expires (Registrar only).
 *
 * The stored states support management control directly - which leads to a
 * total of 14 applicant states and 18 registrar states (the registrar states
 * also incorporate leave timer support):
 *
 *    The Applicant may be in one of the following management states:
 *
 *		1. Normal
 *
 *		2. No protocol
 *
 *		   The protocol machine is held quiet and never sends a message, even
 *		   to prompt another GID participant on the same LAN to respond. In
 *		   this state the Applicant does messages on the media so it can
 *		   be toggled between Normal and No protocol with minimum
 *		   disruption.
 *
 *    The Registrar may be in one of the following management states:
 *
 *		1. Normal Registration.
 *
 *		2. Registration Fixed.
 *
 *		   The Registrar always reports "In" to the application and to GIP
 *		   whatever has occured on the LAN.
 *
 *		3. Registration Forbidden.
 *
 *		   The Registrar always reports "Empty" to the application and to GIP.
 *
 * A set of small tables is used to report aspects of the management state of
 * both applicant and registrar.
 *
 * The main applicant transition table (applicant_tt) is indexed by current
 * applicant state and GID event, and returns:
 *
 *		1. The new applicant state.
 *
 *		2. A start join timer instruction (when required).
 *
 * The main registrar transition table (registrar_tt) is indexed by current
 * registrar state and GID event, and returns:
 *
 *		1. The new registrar state.
 *
 *		2. A join indication or a leave indication (when required).
 *
 *		3. A start leave timer instruction (when required).
 *
 * The only user interface to both these tables is through the public
 * function gidtt_event() which accepts and returns Gid_event's (to report
 * join or leave indications), and which writes timer start requests to the
 * GID scratchpad directly.
 *
 * The Applicant transmit transition table (applicant_txtt) returns the new
 * applicant state, the message to be transmitted, and whether the join timer
 * should be restarted to transmit a further message. A modifier which
 * determines whether a Join (selected for transmission by the Applicant table)
 * should be transmitted as a JoinIn or as a JoinEmpty) is taken from a
 * Registrar state reporting table. The Registrar state is never modified by
 * transmission.
 */


/******************************************************************************
 * GIDTT : GID PROTOCOL TRANSITION TABLE : TABLE ENTRY DEFINITIONS
 ******************************************************************************
 */
enum Applicant_states
{
	Va,  /* Very anxious, active   */
	Aa,  /* Anxious,      active   */
	Qa,  /* Quiet,        active   */
	La,  /* Leaving,      active   */
	Vp,  /* Very anxious, passive  */
	Ap,  /* Anxious,      passive  */
	Qp,  /* Quiet,        passive  */
	Vo,  /* Very anxious observer  */
	Ao,  /* Anxious observer       */
	Qo,  /* Quiet observer         */
	Lo,  /* Leaving observer       */

	Von, /* Very anxious observer, non-participant      */
	Aon, /* Anxious observer,      non-participant      */
	Qon  /* Quiet_observer,        non-participant      */
};

enum {Number_of_applicant_states = Qon + 1}; /* for array sizing */
enum {Number_of_registrar_states = Mtf + 1}; /* for array sizing */

enum Timers
{
	Nt = 0,  /* No timer action    */
	Jt = 1,  /* cstart_join_timer  */
	Lt = 1   /* cstart_leave_timer */
};

enum Applicant_msg
{
	Nm = 0, /* No message to transmit */
	Jm,     /* Transmit a Join        */
	Lm,     /* Transmit a Leave       */
	Em      /* Transmit an Empty      */
};

enum Registrar_indications
{
	Ni = 0,
	Li = 1,
	Ji = 2
};

/******************************************************************************
 * GIDTT : GID PROTOCOL TRANSITION TABLES : TRANSITION TABLE STRUCTURE
 ******************************************************************************
 */

typedef struct                       /* Applicant_tt_entry */
{
	L7_uchar8 new_app_state      ;    /* {Applicant_states} */

	L7_uchar8 cstart_join_timer  ;

} Applicant_tt_entry;

typedef struct                       /* Registrar_tt_entry */
{
	L7_uchar8 new_reg_state      ;

	L7_uchar8 indications        ;

	L7_uchar8 cstart_leave_timer ;

} Registrar_tt_entry;

typedef struct                       /* Applicant_txtt_entry */
{
	L7_uchar8 new_app_state      ;

	L7_uchar8 msg_to_transmit    ; /* Applicant_msgs */

	L7_uchar8 cstart_join_timer  ;

} Applicant_txtt_entry;


/******************************************************************************
 * GIDTT : GID PROTOCOL: MAIN APPLICANT TRANSITION TABLE
 ******************************************************************************
 */

static Applicant_tt_entry
	   applicant_tt[Number_of_gid_rcv_events  + Number_of_gid_req_events +
					Number_of_gid_amgt_events + Number_of_gid_rmgt_events +
                    Number_of_gid_leaveall_events]
				   [Number_of_applicant_states]=
{  /* 
	* General applicant transition table. See description above.
	*/
	{	/* Gid_null */
		/*Va */{Va, Nt},/*Aa */{Aa, Nt},/*Qa */{Qa, Nt},/*La */{La, Nt},
		/*Vp */{Vp, Nt},/*Ap */{Ap, Nt},/*Vp */{Vp, Nt},
		/*Vo */{Vo, Nt},/*Ao */{Ao, Nt},/*Qo */{Qo, Nt},/*Lo */{Lo, Nt},
 
		/*Von*/{Von,Nt},/*Aon*/{Aon,Nt},/*Qon*/{Qon,Nt}
	},
	{	/* Gid_rcv_leaveempty */
		/*Va */{Vp, Jt},/*Aa */{Vp, Jt},/*Qa */{Vp, Jt},/*La */{Vo, Nt},
		/*Vp */{Vp, Nt},/*Ap */{Vp, Jt},/*Qp */{Vp, Jt},
		/*Vo */{Lo, Jt},/*Ao */{Lo, Jt},/*Qo */{Lo, Jt},/*Lo */{Vo, Nt},

		/*Von*/{Von,Nt},/*Aon*/{Von,Nt},/*Qon*/{Von,Nt}
	},
	{	/* Gid_rcv_leavein */
		/*Va */{Va, Nt},/*Aa */{Va, Jt},/*Qa */{Va, Jt},/*La */{La, Nt},
		/*Vp */{Vp, Nt},/*Ap */{Vp, Jt},/*Qp */{Vp, Jt},
		/*Vo */{Lo, Jt},/*Ao */{Lo, Jt},/*Qo */{Lo, Jt},/*Lo */{Vo, Nt},

		/*Von*/{Von,Nt},/*Aon*/{Von,Nt},/*Qon*/{Von,Nt}
	},
	{	/* Gid_rcv_empty */
		/*Va */{Va, Nt},/*Aa */{Va, Jt},/*Qa */{Va, Jt},/*La */{La, Nt},
		/*Vp */{Vp, Nt},/*Ap */{Vp, Jt},/*Qp */{Vp, Jt},
		/*Vo */{Vo, Nt},/*Ao */{Vo, Nt},/*Qo */{Vo, Nt},/*Lo */{Vo, Jt},

		/*Von*/{Von,Nt},/*Aon*/{Von,Nt},/*Qon*/{Von,Nt}
	},
	{	/* Gid_rcv_joinempty */
		/*Va */{Va, Nt},/*Aa */{Va, Nt},/*Qa */{Va, Jt},/*La */{Vo, Nt},
		/*Vp */{Vp, Nt},/*Ap */{Vp, Nt},/*Qp */{Vp, Jt},
		/*Vo */{Vo, Nt},/*Ao */{Vo, Nt},/*Qo */{Vo, Jt},/*Lo */{Vo, Nt},
 
		/*Von*/{Von,Nt},/*Aon*/{Von,Nt},/*Qon*/{Von,Jt}
	},
	{	/* Gid_rcv_joinin */
		/*Va */{Aa, Nt},/*Aa */{Qa, Nt},/*Qa */{Qa, Nt},/*La */{La, Nt},
		/*Vp */{Ap, Nt},/*Ap */{Qp, Nt},/*Qp */{Qp, Nt},
		/*Vo */{Ao, Nt},/*Ao */{Qo, Nt},/*Qo */{Qo, Nt},/*Lo */{Ao, Nt},
 
		/*Von*/{Aon,Nt},/*Aon*/{Qon,Nt},/*Qon*/{Qon,Nt}
	},
	{   /* Gid_join, join request. Handles repeated joins, i.e. joins for
		 * states that are already in. Does not provide feedback for joins
		 * that are forbidden by management controls, the expectation is
		 * that this table will not be directly used by new management
		 * requests.
		 */
		/*Va */{Va, Nt},/*Aa */{Aa, Nt},/*Qa */{Qa, Nt},/*La */{Va, Nt},
		/*Vp */{Vp, Nt},/*Ap */{Ap, Nt},/*Qp */{Qp, Nt},
		/*Vo */{Vp, Jt},/*Ao */{Ap, Jt},/*Qo */{Qp, Nt},/*Lo */{Vp, Jt},
 
		/*Von*/{Von,Nt},/*Aon*/{Aon,Nt},/*Qon*/{Qon,Nt}
	},
	{	/* Gid_leave, leave request. See comments for join requests above. */
		/*Va */{La, Jt},/*Aa */{La, Jt},/*Qa */{La, Jt},/*La */{La, Jt},
		/*Vp */{Vo, Nt},/*Ap */{Ao, Nt},/*Qp */{Qo, Nt},
		/*Vo */{Vo, Nt},/*Ao */{Ao, Nt},/*Qo */{Qo, Nt},/*Lo */{Lo, Nt},
 
		/*Von*/{Von,Nt},/*Aon*/{Aon,Nt},/*Qon*/{Qon,Nt}
	},
	{	/* Gid_normal_operation */
		/*Va */{Vp, Nt},/*Aa */{Vp, Nt},/*Qa */{Vp, Jt},/*La */{La, Nt},
		/*Vp */{Vp, Nt},/*Ap */{Vp, Nt},/*Qp */{Vp, Jt},
		/*Vo */{Va, Nt},/*Ao */{Va, Nt},/*Qo */{Va, Jt},/*Lo */{Lo, Nt},
 
		/*Von*/{Va, Nt},/*Aon*/{Va, Nt},/*Qon*/{Va, Jt}
	},
	{	/* Gid_no_protocol */
		/*Va */{Von,Nt},/*Aa */{Aon,Nt},/*Qa */{Qon,Nt},/*La */{Von,Nt},
		/*Vp */{Von,Nt},/*Ap */{Aon,Nt},/*Qp */{Qon,Nt},
		/*Vo */{Von,Nt},/*Ao */{Aon,Nt},/*Qo */{Qon,Nt},/*Lo */{Von,Nt},
 
		/*Von*/{Von,Nt},/*Aon*/{Aon,Nt},/*Qon*/{Qon,Nt}
	},
	{ 
		/* Gid_normal_registration, same as Gid_join, join request. */
		/*Va */{Va, Nt},/*Aa */{Aa, Nt},/*Qa */{Qa, Nt},/*La */{Va, Jt},
		/*Vp */{Vp, Nt},/*Ap */{Ap, Nt},/*Qp */{Qp, Nt},
		/*Vo */{Vp, Jt},/*Ao */{Ap, Jt},/*Qo */{Qp, Nt},/*Lo */{Vp, Jt},
 
		/*Von*/{Von,Nt},/*Aon*/{Aon,Nt},/*Qon*/{Qon,Nt}
	},
	{	/* Gid_fix_registration, same as Gid_null for the Applicant */
		/*Va */{Va, Nt},/*Aa */{Va, Nt},/*Qa */{Va, Nt},/*La */{Va, Nt},
		/*Vp */{Va, Nt},/*Ap */{Va, Nt},/*Vp */{Va, Nt},
		/*Va */{Va, Nt},/*Aa */{Va, Nt},/*Qa */{Va, Nt},/*La */{Va, Nt},
 
		/*Von*/{Von,Nt},/*Aon*/{Aon,Nt},/*Qon*/{Qon,Nt}
	},
	{	/* Gid_forbid_registration, same as Gid_null for the Applicant */
		/*Vo */{Vo, Nt},/*Ao */{Vo, Nt},/*Qo */{Vo, Nt},/*Lo */{Vo, Nt},
		/*Vp */{Vo, Nt},/*Ap */{Vo, Nt},/*Vp */{Vo, Nt},
		/*Vo */{Vo, Nt},/*Ao */{Vo, Nt},/*Qo */{Vo, Nt},/*Lo */{Vo, Nt},
 
		/*Von*/{Von,Nt},/*Aon*/{Von,Nt},/*Qon*/{Von,Nt}
	},
	{	/* Gid_rcv_leaveall */
		/*Va */{Vp, Jt},/*Aa */{Vp, Jt},/*Qa */{Vp, Jt},/*La */{Vo, Jt},
		/*Vp */{Vp, Nt},/*Ap */{Vp, Jt},/*Qp */{Vp, Jt},
		/*Vo */{Lo, Jt},/*Ao */{Lo, Jt},/*Qo */{Lo, Jt},/*Lo */{Vo, Nt},

		/*Von*/{Von,Nt},/*Aon*/{Von,Nt},/*Qon*/{Von,Nt}
	},
	{	/* Gid_rcv_leaveallrange */
		/*Va */{Vp, Nt},/*Aa */{Vp, Nt},/*Qa */{Vp, Jt},/*La */{Vo, Nt},
		/*Vp */{Vp, Nt},/*Ap */{Vp, Nt},/*Qp */{Vp, Jt},
		/*Vo */{Lo, Nt},/*Ao */{Lo, Jt},/*Qo */{Lo, Jt},/*Lo */{Vo, Nt},

		/*Von*/{Von,Nt},/*Aon*/{Von,Nt},/*Qon*/{Von,Nt}
	}
};


/******************************************************************************
 * GIDTT : GID PROTOCOL: MAIN REGISTRAR TRANSITION TABLE
 ******************************************************************************
 */

static Registrar_tt_entry
	   registrar_tt[Number_of_gid_rcv_events  + Number_of_gid_req_events +
					Number_of_gid_amgt_events + Number_of_gid_rmgt_events +
                    Number_of_gid_leaveall_events]
				   [Number_of_registrar_states] =
{

	{	/* Gid_null      */
		/*In */{Inn,Ni,Nt},
		/*Lv */{Lv, Ni,Nt},
		/*Mt */{Mt, Ni,Nt},

		/*Inr*/{Inr,Ni,Nt},
		/*Lvr*/{Lvr,Ni,Nt},
		/*Mtr*/{Mtr,Ni,Nt},

		/*Inf*/{Inf,Ni,Nt},
		/*Lvf*/{Lvf,Ni,Nt},
		/*Mtf*/{Mtf,Ni,Nt}
 	},
	{	/* Gid_rcv_leaveempty */
		/*Inn*/{Lv, Ni,Lt},
		/*Lv */{Lv, Li,Nt},
		/*Mt */{Mt, Ni,Nt},

		/*Inr*/{Inr,Ni,Nt},
		/*Lvr*/{Lvr,Ni,Nt},
		/*Mtr*/{Mtr,Ni,Nt},

		/*Inf*/{Lvf,Ni,Lt},
		/*Lvf*/{Lvf,Ni,Nt},
		/*Mtf*/{Mtf,Ni,Nt}
 	},
	{	/* Gid_rcv_leavein */
		/*Inn*/{Lv, Ni,Lt},
		/*Lv */{Lv, Li,Nt},
		/*Mt */{Mt, Ni,Nt},

		/*Inr*/{Inr,Ni,Nt},
		/*Lvr*/{Lvr,Ni,Nt},
		/*Mtr*/{Mtr,Ni,Nt},

		/*Inf*/{Lvf,Ni,Lt},
		/*Lvf*/{Lvf,Ni,Nt},
		/*Mtf*/{Mtf,Ni,Nt}
 	},
	{	/* Gid_rcv_empty */
		/*Inn*/{Inn,Ni,Nt},
		/*Lv */{Lv, Ni,Nt},
		/*Mt */{Mt, Ni,Nt},

		/*Inr*/{Inr,Ni,Nt},
		/*Lvr*/{Lvr,Ni,Nt},
		/*Mtr*/{Mtr,Ni,Nt},

		/*Inf*/{Inf,Ni,Nt},
		/*Lvf*/{Lvf,Ni,Nt},
		/*Mtf*/{Mtf,Ni,Nt}

	},
	{	/* Gid_rcv_joinempty */
		/*Inn*/{Inn,Ni,Nt},
		/*Lv */{Inn,Ji,Nt},
		/*Mt */{Inn,Ji,Nt},

		/*Inr*/{Inr,Ni,Nt},
		/*Lvr*/{Inr,Ni,Nt},
		/*Mtr*/{Inr,Ni,Nt},

		/*Inf*/{Inn,Ji,Nt},
		/*Lvf*/{Inn,Ji,Nt},
		/*Mtf*/{Inn,Ji,Nt}

	},
	{	/* Gid_rcv_joinin */
		/*Inn*/{Inn,Ni,Nt},
		/*Lv */{Inn,Ji,Nt},
		/*Mt */{Inn,Ji,Nt},

		/*Inr*/{Inr,Ni,Nt},
		/*Lvr*/{Inr,Ni,Nt},
		/*Mtr*/{Inr,Ni,Nt},

		/*Inf*/{Inn,Ji,Nt},
		/*Lvf*/{Inn,Ji,Nt},
		/*Mtf*/{Inn,Ji,Nt}
	},
	{	/* Gid_join, same as Gid_Null */
		/*In */{Inn,Ni,Nt},
		/*Lv */{Lv, Ni,Nt},
		/*Mt */{Mt, Ni,Nt},

		/*Inr*/{Inr,Ni,Nt},
		/*Lvr*/{Lvr,Ni,Nt},
		/*Mtr*/{Mtr,Ni,Nt},

		/*Inf*/{Inf,Ni,Nt},
		/*Lvf*/{Lvf,Ni,Nt},
		/*Mtf*/{Mtf,Ni,Nt}	
    },
	{	/* Gid_Leave */
		/*In */{Inn,Ni,Nt},
		/*Lv */{Lv, Ni,Nt},
		/*Mt */{Mt, Ni,Nt},

		/*Inr*/{Inr,Ni,Nt},
		/*Lvr*/{Lvr,Ni,Nt},
		/*Mtr*/{Mtr,Ni,Nt},

		/*Inf*/{Inf,Ni,Nt},
		/*Lvf*/{Lvf,Ni,Nt},
		/*Mtf*/{Mtf,Ni,Nt}
	},
	{	/* Gid_normal_operation, same as Gid_null for the Registrar */
		/*In */{Inn,Ni,Nt},
		/*Lv */{Lv, Ni,Nt},
		/*Mt */{Mt, Ni,Nt},

		/*Inr*/{Inr,Ni,Nt},
		/*Lvr*/{Lvr,Ni,Nt},
		/*Mtr*/{Mtr,Ni,Nt},

		/*Inf*/{Inf,Ni,Nt},
		/*Lvf*/{Lvf,Ni,Nt},
		/*Mtf*/{Mtf,Ni,Nt}
 	},
	{	/* Gid_no_protocol, same as Gid_null for the Registrar */
		/*In */{Inn,Ni,Nt},
		/*Lv */{Lv, Ni,Nt},
		/*Mt */{Mt, Ni,Nt},

		/*Inr*/{Inr,Ni,Nt},
		/*Lvr*/{Lvr,Ni,Nt},
		/*Mtr*/{Mtr,Ni,Nt},

		/*Inf*/{Inf,Ni,Nt},
		/*Lvf*/{Lvf,Ni,Nt},
		/*Mtf*/{Mtf,Ni,Nt}
 	},
	{	/* Gid_normal_registration */
		/*Inn*/{Inn,Ni,Nt},
		/*Lv */{Lv, Ni,Nt},
		/*Mt */{Mt, Ni,Nt},

		/*Inr*/{Inn,Ni,Nt},
		/*Lvr*/{Lv, Ni,Nt},
		/*Mtr*/{Mt, Li,Nt},

		/*Inf*/{Inn,Ni,Nt}, /** LVL7 5336 Ji->Ni, problem with machine **/
		/*Lvf*/{Lv, Ni,Nt},
		/*Mtf*/{Mt, Ni,Nt}

	},
	{	/* Gid_fix_registration */
		/*Inn*/{Inr,Ni,Nt},
		/*Lv */{Lvr,Ni,Nt},
		/*Mt */{Inr,Ji,Nt},

		/*Inr*/{Inr,Ni,Nt},
		/*Lvr*/{Lvr,Ni,Nt},
		/*Mtr*/{Inr,Ni,Nt},

		/*Inf*/{Inr,Ji,Nt},
		/*Lvf*/{Lvr,Ji,Nt},
		/*Mtf*/{Inr,Ji,Nt}

	},
	{	/* Gid_forbid_registration */
		/*Inn*/{Inf,Ni,Nt},
		/*Lv */{Lvf,Ni,Nt},
		/*Mt */{Mtf,Ni,Nt},

		/*Inr*/{Inf,Ni,Nt},
		/*Lvr*/{Lvf,Ni,Nt},
		/*Mtr*/{Mtf,Ni,Nt},

		/*Inf*/{Inf,Ni,Nt},
		/*Lvf*/{Lvf,Ni,Nt},
		/*Mtf*/{Mtf,Ni,Nt}
	},
	{	/* Gid_rcv_leaveall */
		/*Inn*/{Lv, Ni,Lt},
		/*Lv */{Lv, Li,Nt},
		/*Mt */{Mt, Ni,Nt},

		/*Inr*/{Inr,Ni,Nt},
		/*Lvr*/{Lvr,Ni,Nt},
		/*Mtr*/{Mtr,Ni,Nt},

		/*Inf*/{Lvf,Ni,Lt},
		/*Lvf*/{Lvf,Ni,Nt},
		/*Mtf*/{Mtf,Ni,Nt}
 	},
	{	/* Gid_rcv_leaveall range */
		/*Inn*/{Lv, Ni,Lt},
		/*Lv */{Lv, Ni,Nt},
		/*Mt */{Mt, Ni,Nt},

		/*Inr*/{Lvr,Ni,Lt},
		/*Lvr*/{Lvr,Ni,Nt},
		/*Mtr*/{Mtr,Ni,Nt},

		/*Inf*/{Lvf,Ni,Lt},
		/*Lvf*/{Lvf,Ni,Nt},
		/*Mtf*/{Mtf,Ni,Nt}
 	}

};


/******************************************************************************
 * GIDTT : GID PROTOCOL : APPLICANT TRANSMIT TABLE
 ******************************************************************************
 */

static Applicant_txtt_entry
	   applicant_txtt[Number_of_applicant_states] =
{
 /*Va */{Aa, Jm,Jt},/*Aa */{Qa, Jm,Nt},/*Qa */{Qa, Nm,Nt},/*La */{Vo, Lm,Nt},
 /*Vp */{Aa, Jm,Jt},/*Ap */{Qa, Jm,Nt},/*Qp */{Qp, Nm,Nt},
 /*Vo */{Vo, Nm,Nt},/*Ao */{Ao, Nm,Nt},/*Qo */{Qo, Nm,Nt},/*Lo */{Vo, Em,Nt},
 
 /*Von*/{Von,Nm,Nt},/*Aon*/{Aon,Nm,Nt},/*Qon*/{Qon,Nm,Nt}
};

/******************************************************************************
 * GIDTT : GID PROTOCOL : RECEIVE EVENTS, USER REQUESTS, & MGT PROCESSSING
 ******************************************************************************
 */

/*********************************************************************
* @purpose  an event is received.
*
* @param    Gid          port
* @param    Gid_machine  machine
* @param    Gid_event    event
*
* @returns  event
*
* @notes   
*
*       
* @end
*********************************************************************/
Gid_event gidtt_event(Gid *my_port, Gid_machine *machine, Gid_event event)
{  /*
	* Handles receive events and join or leave requests.
	*/
    Gid_event          returnVal;
	Applicant_tt_entry *atransition;
	Registrar_tt_entry *rtransition;
    
    atransition = &applicant_tt[event][machine->applicant];
	rtransition = &registrar_tt[event][machine->registrar];

    /* for debug */
    if (bTrackVlan)
    {
      if(vlanFnd)
      {
        /*if (machine->applicant != atransition->new_app_state)*/
          garpDebugTrackVlanRecord(atransition->new_app_state,event,GVRP_APP);

        /*if(machine->registrar != rtransition->new_reg_state)*/
          garpDebugTrackVlanRecord(rtransition->new_reg_state,event,GVRP_REG);
      }
    }
    

	machine->applicant = atransition->new_app_state;
	machine->registrar = rtransition->new_reg_state;

	if ((event == Gid_join) && (atransition->cstart_join_timer))
    {
        my_port->cschedule_tx_now = L7_TRUE;
    }

	my_port->cstart_join_timer  = my_port->cstart_join_timer
							    || atransition->cstart_join_timer;
	my_port->cstart_leave_timer = my_port->cstart_leave_timer
							    || rtransition->cstart_leave_timer;

	switch (rtransition->indications)
	{
        case Ji: 
           returnVal = Gid_join;
		break;

        case Li:
           returnVal = Gid_leave;
		   break;

	    case Ni:
	    default: 
           returnVal = Gid_null;
           break;
	}
    return returnVal;
}


/*********************************************************************
* @purpose  register a machine
*
* @param    Gid_machine  machine
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    Returns True if the Registrar is in, or if registration is fixed.
*
*       
* @end
*********************************************************************/
L7_BOOL gidtt_in(Gid_machine *machine)
{  
    return ((machine->registrar == Inn) || (machine->registrar == Inr));
}



/******************************************************************************
 * GIDTT : GID PROTOCOL TRANSITION TABLES : TRANSMIT MESSAGES
 ******************************************************************************
 */

/*********************************************************************
* @purpose  find if a machine need to transmit 
*
* @param    Gid          port
* @param    Gid_machine  machine
*
* @returns  event
*
* @notes   
*
*       
* @end
*********************************************************************/
Gid_event gidtt_tx(Gid         *my_port,
				   Gid_machine *machine)	
{  /*
	*
	*/

	L7_uint32 msg;
	L7_uint32 rin = 0;
	Gid_event returnVal;


    if ((msg = applicant_txtt[machine->applicant].msg_to_transmit) != Nm)
    {
        rin = machine->registrar; /*registrar_state_table[machine->registrar]*/;
    }

    my_port->cstart_join_timer  = my_port->cstart_join_timer
			|| applicant_txtt[machine->applicant].cstart_join_timer;
    switch (msg)
    {
    case Jm:
       returnVal = (((rin == Inn) || (rin == Inr)) ? Gid_tx_joinin : Gid_tx_joinempty);
       break;

    case Lm:
       /* Note: According to Table 12-3 of GARP Protocol Specification,
        * the Applicant only sends one type of 
        * Leave message -- Leave Empty. */
       /* returnVal = (rin == In ? Gid_tx_leavein : Gid_tx_leaveempty); */
       returnVal = Gid_tx_leaveempty;
       break;

    case Em: 
       returnVal = Gid_tx_empty;
       break;

    case Nm:
    default: 
       returnVal = Gid_null;
    }

    return returnVal;
}

/*********************************************************************
* @purpose  check if a machine is active
*
* @param    Gid_machine  machine
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    Returns False iff the Registrar is Normal registration, Empty, and the
*           Application is Normal membership, Very Anxious Observer.
*       
* @end
*********************************************************************/
L7_BOOL gidtt_machine_active(Gid_machine *machine)
{  
    L7_BOOL returnVal = L7_TRUE;

	if ((machine->applicant == Vo) && 
        ((machine->registrar == Mt) || (machine->registrar == Mtf)))
    {
		returnVal = L7_FALSE;
    }
	
	return returnVal;
}

/*********************************************************************
* @purpose  check if the machine is in forbidden regsitration
*
* @param    Gid_machine  machine
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_BOOL isMachineRegForbidden(Gid_machine *machine)
{
    if (machine->registrar >= Inf)
    {
        return L7_TRUE;
    }

    return L7_FALSE;
}

/*********************************************************************
* @purpose  check if the machine is in forbidden or fixed regsitration 
*
* @param    Gid_machine  machine
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_BOOL isMachineRegForbiddenOrFixed(Gid_machine *machine)
{
    if (machine->registrar >= Inr)
    {
        return L7_TRUE;
    }

    return L7_FALSE;
}

/*********************************************************************
* @purpose  init the machine to Va and Mt
*
* @param    Gid_machine  machine
*
* @returns  none
*
* @notes    none
*       
* @end
*********************************************************************/
void initMachine(Gid_machine *machine)
{
    machine->applicant     = Vo;
    machine->registrar     = Mt;
    machine->incoming_port = L7_FALSE;
    return;
}

/*********************************************************************
* @purpose  set the machine to be leaving
*
* @param    Gid_machine  machine
*
* @returns  none
*
* @notes    none
*       
* @end
*********************************************************************/
void setMachineLeave(Gid_machine *machine)
{
    machine->applicant = La;
    machine->registrar = Mt;
}

/*********************************************************************
* @purpose  set the machine to be leaving
*
* @param    Gid_machine  machine
*
* @returns  none
*
* @notes    none
*       
* @end
*********************************************************************/
void setMachineLeaveIn(Gid_machine *machine)
{
    machine->applicant = La;
    machine->registrar = Inn;
}

/*********************************************************************
* @purpose  check if this is the port received this machine
*
* @param    Gid_machine  machine
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_BOOL isIncomingPort(Gid_machine *machine)
{
    L7_BOOL  returnVal = L7_FALSE;

    /* the reciving port machine shall always stay in observer state */
    if (machine->applicant > Qp)
    {
        returnVal = L7_TRUE;
    }

    return returnVal;
}

/*********************************************************************
* @purpose  change the registrar state when a message is sent out
*
* @param    Gid_machine  machine
* @param    Gid_event    directive
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    none
*       
* @end
*********************************************************************/
void gidtt_txmsg(Gid_machine *machine,Gid_event directive)
{
    switch (directive)
    {
        case Gid_tx_joinin:
        case Gid_tx_joinempty:
           if (machine->applicant == Va)
           {
               machine->applicant = Aa;
           }
           else if (machine->applicant == Aa)
           {
               machine->applicant = Qa;
           }
           else if (machine->applicant == Vp)
           {
               machine->applicant = Aa;
           }
           else if (machine->applicant == Ap)
           {
               machine->applicant = Qa;
           }
           break;

        case Gid_tx_leaveempty:
        case Gid_tx_leavein:
           if (machine->applicant == La)
           {
               machine->applicant = Vo;
           }
           break;

        case Gid_tx_empty:
          if (machine->applicant == Lo)
          {
              machine->applicant = Vo;
          }

       default: 
           break;
    }
}


/*********************************************************************
* @purpose  checks if at least one GVRP attribute was dynamically 
*           registered through JoinIn (checks the state of 
*           Registrar machines)
*
* @param    Gid_machine  machine
* @param    L7_int32     last_gid_used
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_BOOL isAttributeRegistered(Gid_machine *machine, L7_int32  last_gid_used)
{
    L7_BOOL returnValue = L7_FALSE;
    L7_int32 i;

    for (i = last_gid_used; i >= 0; i--) 
    {
        /* an attribute was registered dynamically, if the Registrar is either
         * in Inn or Lv state, and the attribute was registered through 
         * receiving a JoinIn PDU */
        if (((Inn == machine[i].registrar) || (Lv == machine[i].registrar))
            && (L7_TRUE == machine[i].incoming_port))
        {
            returnValue = L7_TRUE;
            break;
        }            
    }

    return returnValue;
}
