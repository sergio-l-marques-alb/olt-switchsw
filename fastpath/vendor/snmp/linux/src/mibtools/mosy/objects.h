/* objects.h - MIB objects */

/*
 *			  ISODE 8.0 NOTICE
 *
 *   Acquisition, use, and distribution of this module and related
 *   materials are subject to the restrictions of a license agreement.
 *   Consult the Preface in the User's Manual for the full terms of
 *   this agreement.
 *
 *
 *			4BSD/ISODE SNMPv2 NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions given in the file
 *    SNMPv2-READ-ME.
 *
 */

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef SR_OBJECTS_H
#define SR_OBJECTS_H

#include "psap.h"

/*  */

typedef struct object_syntax {
    char           *os_name;	/* syntax name */

    IFP             os_encode;	/* data -> PE */
    IFP             os_decode;	/* PE -> data */
    IFP             os_free;	/* free data */

    IFP             os_parse;	/* str -> data */
    IFP             os_print;	/* data -> tty */

    char          **os_data1;	/* squirrel nests */
    int             os_data2;	/* .. */

    char           *os_textual;	/* if a textual convention */
    char           *os_display;	/* .. */
}               object_syntax, *OS;

#define	NULLOS	((OS) 0)

int             readsyntax(), add_syntax();
OS              text2syn();
int             _prim2ulong();
u_long          prim2ulong();
PE              _ulong2prim(), ulong2prim();

typedef struct counter64 {
    u_long          hiword;
    u_long          loword;
}               counter64;

/*  */

typedef struct object_type {
    char           *ot_text;	/* OBJECT DESCRIPTOR */
    char           *ot_id;	/* OBJECT IDENTIFIER */
    OID             ot_name;	/* .. */

    OS              ot_syntax;	/* SYNTAX */

    int             ot_access;	/* ACCESS */
#define	OT_NONE		0x00
#define	OT_RDONLY	0x01
#define	OT_WRONLY	0x02
#define	OT_RDWRITE	(OT_RDONLY | OT_WRONLY)
#define	OT_RDCREAT	OT_RDWRITE	/* doesn't need to be unique... */

    u_long          ot_views;	/* for views */

    int             ot_status;	/* STATUS */
#define	OT_OBSOLETE	0x00
#define	OT_MANDATORY	0x01
#define	OT_OPTIONAL	0x02
#define	OT_DEPRECATED	0x03

    caddr_t         ot_info;	/* object information */
    IFP             ot_getfnx;	/* get/get-next method */
    IFP             ot_setfnx;	/* set method */
#define	type_SNMP_PDUs_commit	(-1)
#define	type_SNMP_PDUs_rollback	(-2)

    caddr_t         ot_save;	/* for set method */

    struct object_type *ot_chain;	/* hash-bucket for text2obj */

    struct object_type *ot_sibling;	/* linked-list for name2obj */
    struct object_type *ot_children;	/* .. */

    struct object_type *ot_next;/* linked-list for get-next */
}               object_type, *OT;

#define	NULLOT	((OT) 0)


extern int      _dont_check_descr;
extern int      once_only_Tbuckets;

extern OT       anchor;
extern OT       chain;


int             readobjects();
int             add_objects();
OT              name2obj(), text2obj();
OID             text2oid();
char           *oid2ode_aux();


/*  */

typedef struct object_instance {
    OID             oi_name;	/* instance OID */

    OT              oi_type;	/* prototype */
}               object_instance, *OI;

#define	NULLOI	((OI) 0)

OI              name2inst(), next2inst(), text2inst();

/*  */

extern IFP      o_advise;


struct objreg {
    char           *ob_name;
    caddr_t         ob_info;
};

void            register_objects();


int             o_generic(), s_generic();

int             o_number();
int             o_longword();
#define	o_integer(oi,v,value)	o_longword ((oi), (v), (integer) (value))

int             o_string();
int             o_qbstring();

int             o_specific();
#define	o_ipaddr(oi,v,value)	o_specific ((oi), (v), (caddr_t) (value))

#ifdef	BSD44
#define	o_clnpaddr(oi,v,value)	o_specific ((oi), (v), (caddr_t) (value))
#endif


int             mediaddr2oid();
#define	ipaddr2oid(ip,addr) \
	mediaddr2oid ((ip), (u_char*) (addr), sizeof (struct in_addr), 0)

#ifdef	BSD44
#define	clnpaddr2oid(ip,addr) \
	mediaddr2oid ((ip), \
		      (u_char *) (addr) -> isoa_genaddr, \
		      (int) (addr) -> isoa_len, 1)
#endif

OID             oid_extend(), oid_normalize();

/*  */

extern int      debug;
extern char     PY_pepy[BUFSIZ];


char           *strdup();

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_OBJECTS_H */
