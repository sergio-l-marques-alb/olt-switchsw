/* psap.h - include file for presentation users (PS-USER) */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#ifndef	SR_PSAP_H
#define	SR_PSAP_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "sr_conf.h"
#include "sr_proto.h"
#include "sr_time.h"
#include <math.h>

#include "manifest.h"
#include "general.h"

/*  */

#ifndef USE_BUILTIN_OIDS
#define USE_BUILTIN_OIDS	1
#endif


typedef struct OIDentifier {
    int             oid_nelem;	/* number of sub-identifiers */

    unsigned int   *oid_elements;	/* the (ordered) list of
					 * sub-identifiers */
}               OIDentifier, *OID;

#define	NULLOID	((OID) 0)

int oid_cmp
    SR_PROTOTYPE((register OID p, register OID q));
OID oid_cpy
    SR_PROTOTYPE((register OID q));
void oid_free
    SR_PROTOTYPE((register OID oid));
char *sprintoid
    SR_PROTOTYPE((register OID oid));

int elem_cmp
    SR_PROTOTYPE((register unsigned int *ip,
                  register int i,
                  register unsigned int *jp,
                  register int j));

/*  */

typedef u_char  PElementClass;

typedef u_char  PElementForm;

#define	PE_CLASS_UNIV	0x0	/* Universal */
#define	PE_CLASS_APPL	0x1	/* Application-wide */
#define	PE_CLASS_CONT	0x2	/* Context-specific */
#define	PE_CLASS_PRIV	0x3	/* Private-use */

#define PE_PRIM_ENCR	0x00b	/* Encrypted */

#ifndef HUGE
#ifdef MAXFLOAT
#define PE_REAL_INFINITY	MAXFLOAT
#else
#define	PE_REAL_INFINITY	99.e99
#endif
#else
#define PE_REAL_INFINITY	HUGE
#endif

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_PSAP_H */

/* The rest of this file is ISODE stuff that mosy doesn't use */

