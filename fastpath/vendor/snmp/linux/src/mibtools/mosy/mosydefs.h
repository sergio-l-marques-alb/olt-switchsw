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

#ifndef SR_MOSYDEFS_H
#define SR_MOSYDEFS_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "psap.h"
#include "sr_proto.h"

struct tuple {
    int     t_type;
    char   *t_class;
    char   *t_form;
    char   *t_id;
    PElementClass t_classnum;
    int	    t_idnum;
};

typedef struct ypv {
    int     yv_code;
#define	YV_UNDF		0x00	/* ??? */
#define	YV_NUMBER	0x01	/* LITNUMBER */
#define	YV_BOOL		0x02	/* TRUE | FALSE */
#define	YV_STRING	0x03	/* LITSTRING */
#define	YV_IDEFINED	0x04	/* ID */
#define	YV_IDLIST	0x05	/* IdentifierList */
#define	YV_VALIST	0x06	/* { Values } */
#define	YV_NULL		0x07	/* NULL */
#define YV_ABSENT	0x08	/* WITH COMPONENTS .. ABSENT */
#define YV_PRESENT	0x09	/*  "	"	   .. PRESENT */
#define YV_INCLUDES	0x0a	/* INCLUDES ... */
#define YV_WITHCOMPS	0x0b	/* WITH COMPONENTS */
#define	YV_OIDLIST	0x0c	/* { object identifier } */
#define YV_REAL		0x0d	/* real value */

    union {
	int	    yv_un_number;		/* code = YV_NUMBER
						   code = YV_BOOL */

	double	    yv_un_real;			/* code = YV_REAL */

	char	   *yv_un_string;		/* code = YV_STRING */

	struct {				/* code = YV_IDEFINED */
	    char   *yv_st_module;
	    char   *yv_st_modid;
	    char   *yv_st_identifier;
	}		yv_st;

        struct ypv *yv_un_idlist;		/* code = YV_IDLIST
						   code = YV_VALIST
						   code = YV_OIDLIST */
    }                   yv_un;
#define	yv_number	yv_un.yv_un_number
#define	yv_string	yv_un.yv_un_string
#define	yv_identifier	yv_un.yv_st.yv_st_identifier
#define	yv_module	yv_un.yv_st.yv_st_module
#define yv_modid	yv_un.yv_st.yv_st_modid
#define yv_idlist	yv_un.yv_un_idlist
#define yv_real		yv_un.yv_un_real

    char   *yv_action;
    int	    yv_act_lineno;

    int	    yv_flags;
#define	YV_NOFLAGS	0x00	/* no flags */
#define	YV_ID		0x01	/* ID Value */
#define	YV_NAMED	0x02	/* NamedNumber */
#define	YV_TYPE		0x04	/* TYPE Value */
#define	YV_BOUND	0x08	/* named value */
#define	YV_IMPLIED	0x10	/* IMPLIED */
#define	YVBITS	"\020\01ID\02NAMED\03TYPE\04BOUND"

    char   *yv_id;				/* flags & YV_ID */

    char   *yv_named;				/* flags & YV_NAMED */

    struct ype *yv_type;			/* flags & YV_TYPE */

    struct ypv *yv_next;
}			ypv, *YV;
#define	NULLYV	((YV) 0)

/*  */

typedef struct ypt {
    PElementClass   yt_class;

    YV		    yt_value;
}			ypt, *YT;
#define	NULLYT	((YT) 0)

/*  */

typedef struct ype {
    int     yp_code;
#define	YP_UNDF		0x00	/* type not yet known */
#define	YP_BOOL		0x01	/* BOOLEAN */
#define	YP_INT		0x02	/* INTEGER */
#define	YP_INTLIST	0x03	/* INTEGER [ NamedNumberList ] */
#define	YP_BIT		0x04	/* BITSTRING */
#define	YP_BITLIST	0x05	/* BITSTRING [ NamedNumberList ] */
#define	YP_OCT		0x06	/* OCTETSTRING */
#define	YP_NULL		0x07	/* NULL */
#define	YP_SEQ		0x08	/* SEQUENCE */
#define	YP_SEQTYPE	0x09	/* SEQUENCE OF Type */
#define	YP_SEQLIST	0x0a	/* SEQUENCE [ ElementTypes ] */
#define	YP_SET		0x0b	/* SET */
#define	YP_SETTYPE	0x0c	/* SET OF Type */
#define	YP_SETLIST	0x0d	/* SET [ MemberTypes ] */
#define	YP_CHOICE	0x0e	/* CHOICE [ AlternativeTypeList ] */
#define	YP_ANY		0x0f	/* ANY */
#define	YP_OID		0x10	/* OBJECT IDENTIFIER */
#define	YP_IDEFINED	0x11	/* identifier */
#define YP_ENUMLIST	0x12	/* ENUMERATED */
#define YP_REAL		0x13	/* Real (floating-point) */
#define	YP_BITS		0x14	/* BITS */

    int     yp_direction;
#define YP_DECODER	0x01
#define YP_ENCODER	0x02
#define	YP_PRINTER	0x04

    union {
	struct {				/* code = YP_IDEFINED */
	    char   *yp_st_module;		    /* module name */
	    OID	    yp_st_modid;		    /* module id */
	    char   *yp_st_identifier;		    /* definition name */
	}		yp_st;

	struct ype *yp_un_type;			/* code = YP_SEQTYPE
						   code = YP_SEQLIST
						   code = YP_SETTYPE
						   code = YP_SETLIST
						   code = YP_CHOICE */

	YV	    yp_un_value;		/* code = YP_INTLIST
						   code = YP_BITLIST */
    }                   yp_un;
#define	yp_identifier	yp_un.yp_st.yp_st_identifier
#define	yp_module	yp_un.yp_st.yp_st_module
#define yp_modid	yp_un.yp_st.yp_st_modid
#define	yp_type		yp_un.yp_un_type
#define	yp_value	yp_un.yp_un_value

    char   *yp_intexp;		/* expressions to pass (use) as extra */
    char   *yp_strexp;		/* parameters (primitive values) */
    char    yp_prfexp;

    char   *yp_declexp;
    char   *yp_varexp;

    char   *yp_structname;
    char   *yp_ptrname;

    char   *yp_param_type;

    char   *yp_action0;
    int     yp_act0_lineno;

    char   *yp_action05;
    int	    yp_act05_lineno;

    char   *yp_action1;
    int	    yp_act1_lineno;

    char   *yp_action2;
    int	    yp_act2_lineno;

    char   *yp_action3;
    int	    yp_act3_lineno;

    long    yp_flags;
#define	YP_NOFLAGS	0x0000	/* no flags */
#define	YP_OPTIONAL	0x0001	/* OPTIONAL */
#define	YP_COMPONENTS	0x0002	/* COMPONENTS OF */
#define	YP_IMPLICIT	0x0004	/* IMPLICIT */
#define	YP_DEFAULT	0x0008	/* DEFAULT */
#define	YP_ID		0x0010	/* ID */
#define	YP_TAG		0x0020	/* Tag */
#define	YP_BOUND	0x0040	/* ID LANGLE */
#define	YP_PULLEDUP	0x0080	/* member is a choice */
#define YP_PARMVAL	0x0100	/* value to be passed to parm is present */
#define YP_CONTROLLED	0x0200	/* encoding item has a controller */
#define	YP_OPTCONTROL	0x0400	/*   .. */
#define	YP_ACTION1	0x0800	/* action1 acted upon */
#define	YP_PARMISOID	0x1000	/* value to be passed to parm is OID */
#define YP_ENCRYPTED	0x2000	/* encypted - which is a bit hazy */
#define YP_IMPORTED	0x4000  /* value imported from another module */
#define YP_EXPORTED	0x8000  /* value exported to another module */
#define	YP_SUBTYPE	0x10000 /* Type SubtypeSpec */
#define	YPBITS	"\020\01OPTIONAL\02COMPONENTS\03IMPLICIT\04DEFAULT\05ID\06TAG\
\07BOUND\010PULLEDUP\011PARMVAL\012CONTROLLED\013OPTCONTROL\
\014ACTION1\015PARMISOID\016ENCRYPTED\017IMPORTED\020EXPORTED"

    YV	    yp_default;				/* flags & YP_DEFAULT */

    char   *yp_id;				/* flags & YP_ID */

    YT	    yp_tag;				/* flags & YP_TAG */

    char   *yp_bound;				/* flags & YP_BOUND */

    char   *yp_parm;				/* flags & YP_PARMVAL */

    char   *yp_control;				/* flags & YP_CONTROLLED */

    char   *yp_optcontrol;			/* flags & YP_OPTCONTROL */

    struct ysub *yp_subtype;			/* flags & YP_SUBTYPE */

    char   *yp_offset;

    struct ype *yp_next;
} 			ype, *YP;
#define	NULLYP	((YP) 0)

struct endpoint {
    YV	    ep_value;
    int	    ep_langle;
};

struct ysub {
    int	    ysub_code;
#define	YS_UNDF		0x00	/* unknown */
#define	YS_VALUE	0x01	/* Value */
#define	YS_CONTSUBTYPE	0x02	/* ContainedSubType */
#define	YS_VALUERANGE	0x03	/* ValueRange */
#define	YS_PERMITALPHA	0x04	/* PermittedAlphabet */
#define	YS_SIZE		0x05	/* SizeConstraint */
#define	YS_INNER	0x06	/* InnerTypeConstraint */

    union {
	YV		ysub_un_value;

	struct {		/* code == YS_VALUERANGE */
	    struct endpoint *ysub_un_lower;
	    struct endpoint *ysub_un_upper;
	}	ysub_un_range;

				/* code == YS_SIZE */
	struct ysub     *ysub_un_subtype;
    }		ysub_un;
#define	ysub_value	ysub_un.ysub_un_value
#define	ysub_lower	ysub_un.ysub_un_range.ysub_un_lower
#define	ysub_upper	ysub_un.ysub_un_range.ysub_un_upper
#define	ysub_subtype	ysub_un.ysub_un_subtype

    struct ysub *ysub_next;
};

#define	TBL_EXPORT	0
#define TBL_IMPORT	1
#define MAX_TBLS	2

extern int tagcontrol;
#define TAG_UNKNOWN 	0
#define TAG_IMPLICIT	1
#define TAG_EXPLICIT	2

#define CH_FULLY	0
#define CH_PARTIAL	1

typedef struct yop {
    char   *yo_name;

    YP	    yo_arg;
    YP	    yo_result;
    YV	    yo_errors;
    YV	    yo_linked;

    int	    yo_opcode;
}		yop, *YO;
#define	NULLYO	((YO) 0)


typedef struct yerr {
    char   *ye_name;

    YP	    ye_param;

    int	    ye_errcode;

    int	    ye_offset;
}	    yerr, *YE;
#define	NULLYE	((YE) 0)


typedef struct ypr {
    char   *yr_utctime;
    char   *yr_descr;

    struct ypr *yr_next;
}			ypr, *YR;
#define	NULLYR	((YR) 0)

typedef struct ypc {
    int	    yc_type;
#define	YC_GROUP	0x00
#define	YC_OBJECT	0x01
#define	YC_VARIATION	0x02

    char   *yc_target;
    char   *yc_descr;

		/* if yc_type == YC_OBJECT or YC_VARIATION */
    YP	    yc_syntax;
    YP	    yc_wsyntax;
    char   *yc_access;
    char   *yc_acgran;
		/* if yc_type == YC_VARIATION */
    YV	    yc_creation;
    YV	    yc_defval;

    struct ypc *yc_next;
}			ypc, *YC;
#define	NULLYC	((YC) 0)

typedef struct ypd {
    char   *yd_modname;
    OID	    yd_modid;
    YV      yd_groups;
    YC	    yd_variations;

    struct ypd *yd_next;
}			ypd, *YD;
#define	NULLYD	((YD) 0)

/*  */

extern char *mosyversion;

extern int yysection;
extern char *yyencpref;
extern char *yydecpref;
extern char *yyprfpref;
extern char *yyencdflt;
extern char *yydecdflt;
extern char *yyprfdflt;

extern int yydebug;
extern int yylineno;

#ifdef DYNIX
#define SR_DEFINED_YYTEXT
extern unsigned char yytext[];
#endif /* DYNIX */

#ifdef SR_OSF
#define SR_DEFINED_YYTEXT
extern unsigned char yytext[];
#endif /* SR_OSF */

#ifdef SR_LYNX
#define SR_DEFINED_YYTEXT
extern char *yytext;
#endif /* SR_LYNX */

#ifdef SUNOS
#define SR_DEFINED_YYTEXT
extern char *yytext;
#endif /* SUNOS */

#ifndef SR_DEFINED_YYTEXT
extern char yytext[];
#endif /* SR_DEFINED_YYTEXT */

extern char *mymodule;

extern OID   mymoduleid;

extern char *bflag;
extern int   Cflag;
extern int   dflag;
extern int   Pflag;
extern char *sysin;

extern char *module_actions;

#if !defined(LINUX) && !defined(SOLARIS) && !defined(PC)
extern int errno;
#endif /* !LINUX && !SOLARIS && !PC */

#ifdef  PEPYMISC_C
#define EXTDECL
#else
#define EXTDECL extern
#endif
EXTDECL OID addoid
    SR_PROTOTYPE((OID o1,
                  OID o2));

EXTDECL OID int2oid
    SR_PROTOTYPE((int n));

EXTDECL OID oidlookup
    SR_PROTOTYPE((char *name));

EXTDECL char * oidname
    SR_PROTOTYPE((OID oid));

EXTDECL char * oidprint
    SR_PROTOTYPE((OID oid));

EXTDECL char *importedP
    SR_PROTOTYPE((char *name));

EXTDECL void addtable
    SR_PROTOTYPE((char *name,
                  int lt));

EXTDECL void addtableref
    SR_PROTOTYPE((char *name,
                  OID id,
                  int lt));

EXTDECL void check_impexp
    SR_PROTOTYPE((YP yp));

EXTDECL void defineoid
    SR_PROTOTYPE((char *name,
                  OID oid));

EXTDECL void initoidtbl
    SR_PROTOTYPE((void));

EXTDECL void print_expimp
    SR_PROTOTYPE((void));

#undef EXTDECL

#ifdef  MOSY_MAIN
#define EXTDECL
#else
#define EXTDECL extern
#endif

EXTDECL void yyerror(register char *s);

EXTDECL int yywrap(void);

EXTDECL void myyerror(char *fmt, ...);

EXTDECL void warning(char *fmt, ...);


EXTDECL void pass1(void);

EXTDECL void pass2(void);

EXTDECL char *sys_errname
    SR_PROTOTYPE((int i));

EXTDECL void tcltableref
    SR_PROTOTYPE((char *module));

EXTDECL int  check_expimp
    SR_PROTOTYPE((char *name));

EXTDECL void yyprint 
    SR_PROTOTYPE((char *s,
                  int f,
                  int top));

EXTDECL void pass1_mod 
    SR_PROTOTYPE((char *mod,
                  char *id,
                  YV value,
                  char *update,
                  char *org,
                  char *contact,
                  char *descr,
                  YR revs));

EXTDECL void pass1_oid 
    SR_PROTOTYPE((char *mod,
                  char *id,
                  YV value,
                  char *sname,
                  char *descr,
                  char *refer));

EXTDECL void pass1_conv
    SR_PROTOTYPE((char *mod,
                  char *id,
                  YP value,
                  char *display,
                  char *sname,
                  char *descr,
                  char *refer));

EXTDECL void pass1_obj 
    SR_PROTOTYPE((char *mod,
                  char *id,
                  YV value,
                  YP syntax,
                  char *units,
                  char *aname,
                  char *sname,
                  char *descr,
                  char *refer,
                  YV idx,
                  YV defval));

EXTDECL void pass1_trap
    SR_PROTOTYPE((char *mod,
                  char *id,
                  YV enterprise,
                  int number,
                  YV vars,
                  char *sname,
                  char *descr,
                  char *refer));

EXTDECL void pass1_grp 
    SR_PROTOTYPE((char *mod,
                  char *id,
                  YV value,
                  YV objects,
                  char *sname,
                  char *descr,
                  char *refer,
                  int notify));

EXTDECL void pass1_comp
    SR_PROTOTYPE((char *mod,
                  char *id,
                  YV value,
                  char *sname,
                  char *descr,
                  char *refer,
                  YD comp));

EXTDECL void pass1_cap 
    SR_PROTOTYPE((char *mod,
                  char *id,
                  YV value,
                  char *release,
                  char *sname,
                  char *descr,
                  char *refer,
                  YD comp));

EXTDECL void pass1_type
    SR_PROTOTYPE((char *encpref,
                  char *decpref,
                  char *prfpref,
                  char *mod,
                  char *id,
                  register YP yp));

EXTDECL YP new_type
    SR_PROTOTYPE((int code));

EXTDECL YP add_type
    SR_PROTOTYPE((register YP yp1,
                  register YP yp2));

EXTDECL YV new_value
    SR_PROTOTYPE((int code));

EXTDECL YV add_value
    SR_PROTOTYPE((register YV yp1,
                  register YV yp2));

EXTDECL YR new_yr
    SR_PROTOTYPE((char *utctime,
                  char *descr));

EXTDECL YR add_yr
    SR_PROTOTYPE((register YR yp1,
                  register YR yp2));

EXTDECL YC new_yc
    SR_PROTOTYPE((int type,
                  char *target,
                  char *descr));

EXTDECL YC add_yc
    SR_PROTOTYPE((register YC yp1,
                  register YC yp2));

EXTDECL YD new_yd
    SR_PROTOTYPE((void));

EXTDECL YD add_yd
    SR_PROTOTYPE((register YD yp1,
                  register YD yp2));

EXTDECL char *new_string
    SR_PROTOTYPE((register char  *s));

#undef EXTDECL

#ifdef  __cplusplus
}
#endif

typedef struct symtable {
    char	*sym_name;
    char	*sym_module;
    OID		sym_oid;
    struct symtable *sym_next;
} symtable, *SYM;
#define NULLSYM ((SYM)0)
extern	SYM	symtab[];

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_MOSYDEFS_H */
