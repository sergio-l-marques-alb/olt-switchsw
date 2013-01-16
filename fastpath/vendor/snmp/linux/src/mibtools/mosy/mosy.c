/*
 *
 * Copyright (C) 1992-2006 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/* mosy.c - Managed Object Syntax-compiler (yacc-based) */

/* Originally contributed by NYSERNet Inc.  This work was partially supported
 * by the U.S. Defense Advanced Research Projects Agency and the Rome Air
 * Development Center of the U.S. Air Force Systems Command under contract
 * number F30602-88-C-0016.
 */

/*
 *			  ISODE 8.0 NOTICE
 *
 *   Acquisition, use, and distribution of this module and related
 *   materials are subject to the restrictions of a license agreement.
 *   Consult the Preface in the User's Manual for the full terms of
 *   this agreement.
 *
 *
 *			         NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions given in the file
 *    platforms/ANNOUNCEMENT.
 *
 */

#include "sr_conf.h"

#include <stdio.h>

#include <ctype.h>

#include <stdlib.h>

#include <stdarg.h>

#include <string.h>

#include <unistd.h>

#include <errno.h>


#include "sr_proto.h"
#include "mosydefs.h" /* DAR -- changed name from mosy-defs.h to mosydefs.h */


#define DEFSDIR "./"

/*    DATA */

int	Cflag = 0;		/* mosy */
int	dflag = 0;
int	Pflag = 0;		/* pepy compat... */
int	snmpV2 = 0;
int	htmlflag = 0;
int	oneflag = 0;
int	doexternals;
static int didtcl = 0;
static int linepos = 0;
static int mosydebug = 0;
static int sr_delay = 0;	/* to avoid name conflict on SINIX */
static int iflag = 0;
static int sflag = 0;
static int Sflag = 0;
static int tflag = 0;

static  char *eval = NULLCP;

char   *mymodule = "";
OID	mymoduleid;

int yysection = 0;
char *yyencpref = "none";
char *yydecpref = "none";
char *yyprfpref = "none";
char *yyencdflt = "none";
char *yydecdflt = "none";
char *yyprfdflt = "none";

static char *yymode = "";

static char autogen[BUFSIZ];

char *sysin = NULLCP;
static char sysout[BUFSIZ];
static char systcl[BUFSIZ];


FILE   *tcl = NULL;

/*  */

typedef struct ym {
    char   *ym_name;
    YV	    ym_value;

    char   *ym_update;
    char   *ym_org;
    char   *ym_contact;
    char   *ym_descr;

    YR	    ym_revs;
}		    *YM;
#define	NULLYM	((YM) 0)


typedef struct yoi {
    char   *yi_name;

    YV	    yi_value;

    char   *yi_status;
    char   *yi_descr;
    char   *yi_refer;
}		yoi, *OI;
#define	NULLOI	((OI) 0)


typedef struct tct {
    char   *tc_name;
    YP	    tc_value;

    char   *tc_display;

    char   *tc_status;
    char   *tc_descr;
    char   *tc_refer;
}		tct, *TC;
#define	NULLTC	((TC) 0)


typedef struct yot {
    char   *yo_name;
    YV	    yo_value;

    YP	    yo_syntax;
    char   *yo_units;

    char   *yo_access;

    char   *yo_status;
    char   *yo_descr;
    char   *yo_refer;

    YV	    yo_index;
    YV	    yo_defval;
}		yot, *OT;
#define	NULLOT	((OT) 0)


typedef struct ytt {
    char   *yt_name;

    YV	    yt_enterprise;
    int	    yt_number;

    YV	    yt_vars;

    char   *yt_status;
    char   *yt_descr;
    char   *yt_refer;
}		ytt, *TT;
#define	NULLTT	((TT) 0)


typedef struct yog {
    char   *yg_name;
    YV	    yg_value;

    YV	    yg_objects;

    char   *yg_status;
    char   *yg_descr;
    char   *yg_refer;

    int	    yg_notify;
}		ygt, *OG;
#define	NULLOG	((OG) 0)


typedef struct mct {
    char   *mc_name;
    YV	    mc_value;

    YD	    mc_comp;

    char   *mc_status;
    char   *mc_descr;
    char   *mc_refer;
}		mct, *MC;
#define	NULLMC	((MC) 0)


typedef struct act {
    char   *ac_name;
    YV	    ac_value;

    char   *ac_release;
    YD	    ac_comp;

    char   *ac_status;
    char   *ac_descr;
    char   *ac_refer;
}		act, *AC;
#define	NULLAC	((AC) 0)

/*  */

typedef struct symlist {
    char   *sy_encpref;
    char   *sy_decpref;
    char   *sy_prfpref;
    char   *sy_module;
    char   *sy_name;

    union {
	YM	sy_un_ym;

	OI	sy_un_yi;

	TC	sy_un_tc;

	OT	sy_un_yo;

	TT	sy_un_yt;

	OG	sy_un_yg;

	MC	sy_un_mc;

	AC	sy_un_ac;

	YP	sy_un_yp;
    }	sy_un;
#define	sy_ym	sy_un.sy_un_ym
#define	sy_yi	sy_un.sy_un_yi
#define	sy_tc	sy_un.sy_un_tc
#define	sy_yo	sy_un.sy_un_yo
#define	sy_yt	sy_un.sy_un_yt
#define	sy_yg	sy_un.sy_un_yg
#define	sy_mc	sy_un.sy_un_mc
#define	sy_ac	sy_un.sy_un_ac
#define	sy_yp	sy_un.sy_un_yp

    struct symlist *sy_next;
    struct symlist *sy_link;
}		symlist, *SY;
#define	NULLSY	((SY) 0)

static	SY	mylinks = NULLSY;
static	SY	mymodules = NULLSY;
static	SY	myconv = NULLSY;
static	SY	myidentifiers = NULLSY;
static	SY	myobjects = NULLSY;
static	SY	mytraps = NULLSY;
static	SY	mygroups = NULLSY;
static	SY	mymibs = NULLSY;
static	SY	myagents = NULLSY;
static	SY	mytypes = NULLSY;

void _asprintf(register char  *bp, char *what, char *fmt, va_list ap);

/* static function prototypes */
static void printh_line 
    SR_PROTOTYPE((char *s));

static void printh_quoted
    SR_PROTOTYPE((char *clause,
                  char *s));

static void val2prf1
    SR_PROTOTYPE ((char *mod,
                   register YV yv,
                   int level));

static void id2html
    SR_PROTOTYPE ((char *mod,
                   char *id));

static void yyprint_aux
    SR_PROTOTYPE((char *s,
                  char *mode));

static void yyerror_aux
    SR_PROTOTYPE((register char *s));

static void do_mod
    SR_PROTOTYPE((register YM ym,
                  char *id));

static void do_oid
    SR_PROTOTYPE((register OI yi,
                  char *id));

static void do_conv
    SR_PROTOTYPE((register TC tc,
                  char *id));

static void do_obj
    SR_PROTOTYPE((register OT yo,
                  char *id));

static void do_trap
    SR_PROTOTYPE((register TT yt,
                  char *id));

static void do_grp
    SR_PROTOTYPE((register OG yg,
                  char *id));

static void do_mib
    SR_PROTOTYPE((register MC mc,
                  char *id));

static void do_agent
    SR_PROTOTYPE((register AC ac,
                  char *id));

static void check_objects
    SR_PROTOTYPE((register YV yv,
                  char *clause,
                  int typesOK,
		  int notify));

static void check_object
    SR_PROTOTYPE((register YV yv,
                  char *clause,
                  int typesOK,
		  int notify));

static void print1_type	
    SR_PROTOTYPE((register YP yp,
                  int level,
                  char *id));

static int val2int	
    SR_PROTOTYPE((register YV yv));

static void val2prf	
    SR_PROTOTYPE((register YV yv,
                  int level));

static void do_tcl	
    SR_PROTOTYPE((char *id));

static void print_ym	
    SR_PROTOTYPE((register YM ym,
                  register int level));

static void print_yi	
    SR_PROTOTYPE((register OI yi,
                  register int level));

static void print_tc	
    SR_PROTOTYPE((register TC tc,
                  register int level));

static void print_yo	
    SR_PROTOTYPE((register OT yo,
                  register int level));

static void print_yt	
    SR_PROTOTYPE((register TT yt,
                  register int level));

static void print_yg	
    SR_PROTOTYPE((register OG yg,
                  register int level));

static void print_mc	
    SR_PROTOTYPE((register MC mc,
                  register int level));

static void print_ac	
    SR_PROTOTYPE((register AC ac,
                  register int level));

static void print_yr	
    SR_PROTOTYPE((register YR yr,
                  register int level));

static void print_yd	
    SR_PROTOTYPE((register YD yd,
                  register int level));

static void print_yc	
    SR_PROTOTYPE((register YC yc,
                  register int level));

static void print_type	
    SR_PROTOTYPE((register YP yp,
                  register int level));

static void print_value	
    SR_PROTOTYPE((register YV yv,
                  register int level));

static SY add_symbol 	
    SR_PROTOTYPE((register SY	s1,
                  register SY s2));

static SY new_symbol 	
    SR_PROTOTYPE((register char *encpref,
                  register char *decpref,
                  register char *prfpref,
                  register char *mod,
                  register char *id));

static char *id2str 	
    SR_PROTOTYPE((register YV yv));

static char *check_syntax
    SR_PROTOTYPE((char *name,
                  YP *ypp));

static YP lookup_type 	
    SR_PROTOTYPE((register char *mod,
                  register char *id ));

static char *val2str 	
    SR_PROTOTYPE((register YV yv));

static	OI lookup_identifier
    SR_PROTOTYPE((register char *mod,
                  register char *id));

static OG lookup_group 	
    SR_PROTOTYPE((register char *mod,
                  register char *id));

static int print_defval
    SR_PROTOTYPE((register OT yo,
                  char *id));

/*    MAIN */

/* ARGSUSED */



/* strrpbrk is like strpbrk but works on the last char instead of first:
   From the string s1, find the last occurance of any character in s2,
   and return a pointer to that last occurance; if s1 and s2 have
   no characters in common then return NULL                         */
char *
strrpbrk(char *s1, char *s2)
{
  char *p1, *p2;
  p1 = strpbrk(s1, s2);
  if(p1 == NULL)
    return NULL;

  do{
    p2 = p1;
    p1 = strpbrk(p1+1, s2);
  } while (p1 != NULL);

  return p2;
}


static OT lookup_object_aux(register char *mod,register char *id, SY start);
#define	lookup_object(mod,id)	lookup_object_aux ((mod), (id), myobjects)
#define	lookup_notify(mod,id)	lookup_object_aux ((mod), (id), mytraps)


int yyparse(void);

int
main (int argc, char  **argv, char **envp)
{
    register char  *cp,
		   *sp;

    sysout[0] = '\0';
    for (argc--, argv++; argc > 0; argc--, argv++) {
	cp = *argv;

	if (strcmp (cp, "-d") == 0) {
	    dflag++;
	    continue;
	}
	if (strcmp (cp, "-i") == 0) {
	    iflag++;
	    continue;
	}
	if (strcmp (cp, "-m") == 0) {	/* obsolete... */
	    continue;
	}
	if (strcmp (cp, "-o") == 0) {
	    if (sysout[0])
		fprintf (stderr, "too many output files\n"), exit (1);
	    if (--argc <= 0 || (*(cp = *++argv) == '-' && cp[1] != '\0'))
		goto usage;
	    (void) strcpy (sysout, cp);

	    continue;
	}
	if (strcmp (cp, "-s") == 0) {
	    sflag++;
	    continue;
	}
	if (strcmp (cp, "-S") == 0) {  /* -S to supress warnings */
	    Sflag++;
	    continue;
	}
	if (strcmp (cp, "-t") == 0) {
	    tflag++;
	    continue;
	}

	if (strcmp (cp, "-h") == 0) {
	    htmlflag++;
	    continue;
	}
	if (strcmp (cp, "-1") == 0) {
	    oneflag++;
	    continue;
	}

	if (sysin) {
usage: ;
	    fprintf (stderr,
		     "usage: mosy [-d] [-i] [-o module.def] [-s] [-t] [-1] [-h] module.my\n");
	    exit (1);
	}

	if (*cp == '-') {
	    if (*++cp != '\0')
		goto usage;
	    sysin = "";
	}
	sysin = cp;

	if (sysout[0])
	    continue;

	if ((sp = strrpbrk (cp, "/\\")) != NULL)  /* find last slash */
	    sp++;
	if (sp == NULL || *sp == '\0')
	    sp = cp;
	sp += strlen (cp = sp) - 3;
	if (sp > cp && strcmp (sp, ".my") == 0)
	    (void) sprintf (sysout, "%.*s.def", (int)(sp - cp), cp);
	else
	    (void) sprintf (sysout, "%s.def", cp);
    }
   
    if (!sflag) {  /* If not called with -s flag print version info */
       fprintf (stderr, "%s\n", mosyversion);
    } /* if called with -s */

    if (htmlflag && oneflag) {
	fprintf (stderr, "-h and -1 are mutually-exclusive");
	exit (1);
    }

    switch (mosydebug = (cp = getenv ("MOSYTEST")) && *cp ? atoi (cp) : 0) {
	case 2:
	    yydebug++;		/* fall */
	case 1:
	    sflag++;		/*   .. */
	case 0:
	    break;
    }

    if (sysin == NULLCP)
	sysin = "";

    if (*sysin && freopen (sysin, "r", stdin) == NULL) {
	fprintf (stderr, "unable to read "), perror (sysin);
	exit (1);
    }

    if (strcmp (sysout, "-") == 0)
	sysout[0] = '\0';
    if (*sysout && freopen (sysout, "w", stdout) == NULL) {
	fprintf (stderr, "unable to write "), perror (sysout);
	exit (1);
    }

    if (tflag) {
	sp = sysout + strlen (sysout) - 5;
	if (sp > sysout && strcmp (sp, ".def") == 0)
	    (void) sprintf (systcl, "%.*s.tcl", (int)(sp - sysout), sysout);
	else
	    (void) sprintf (systcl, "%s.tcl", sysout);
	if ((tcl = fopen (systcl, "w")) == NULL) {
	    fprintf (stderr, "unable to write "), perror (systcl);
	    exit (1);
	}
	fprintf (tcl,
		 "if {[info exists snmptcl_loaded(%s)]} {\n    return\n}\n",
		 systcl);
	fprintf (tcl, "set\tsnmptcl_loaded(%s)\t1\n", systcl);
    }

    if ((cp = (char *) index (mosyversion, ')')) != NULL)
	for (cp++; *cp != ' '; cp++)
	    if (*cp == '\0') {
		cp = NULL;
		break;
	    }
    if (cp == NULL)
	cp = mosyversion + strlen (mosyversion);
    (void) sprintf (autogen, "%*.*s",
	    (int)(cp - mosyversion), (int)(cp - mosyversion), mosyversion);
    if (!htmlflag)
	printf ("-- automatically generated by %s, do not edit!\n\n", autogen);

    initoidtbl ();

    exit (yyparse ());		/*NOTREACHED*/

    return 0;
}

/*    ERRORS */

void yyerror (s)
register char   *s;
{
    yyerror_aux (s);

    if (sr_delay) {
	sr_delay = -1;
	return;
    }

    if (*sysout)
	(void) unlink (sysout);
    if (tcl)
	(void) unlink (systcl);

    exit (1);
}

void
warning(char *fmt, ...)
{
    char	buffer[BUFSIZ];
    char	buffer2[BUFSIZ];
    va_list	ap;

    if(Sflag) return;

    va_start (ap, fmt);

    _asprintf (buffer, NULLCP, fmt, ap);

    va_end (ap);

    (void) sprintf (buffer2, "warning: %s", buffer);

    yyerror_aux (buffer2);
}

static void yyerror_aux (s)
register char   *s;
{
    if (linepos)
	fprintf (stderr, "\n"), linepos = 0;

    if (eval)
	fprintf (stderr, "%s %s:\n    ", yymode, eval);
    else
	fprintf (stderr, "line %d: ", yylineno);
    fprintf (stderr, "%s\n", s);
    if (!eval)
	fprintf (stderr, "last token read was \"%s\"\n", yytext);
}

/*  */

void
myyerror (char *fmt, ...)
{
    char    buffer[BUFSIZ];
    va_list ap;

    va_start (ap, fmt);

    _asprintf (buffer, NULLCP, fmt, ap);

    va_end (ap);

    yyerror (buffer);
}

/*  */

int yywrap () {
    if (linepos)
	fprintf (stderr, "\n"), linepos = 0;

    return 1;
}

/*  */

/* ARGSUSED */

void yyprint (s, f, top)
char   *s;
int	f,
	top;
{
}

static void yyprint_aux (s, mode)
char   *s,
       *mode;
{
    int	    len;
    static int nameoutput = 0;
    static int outputlinelen = 79;

    if (sflag)
	return;

    if (strcmp (yymode, mode)) {
	    if (linepos)
	        fprintf (stderr, "\n\n");

	    fprintf (stderr, "%s", mymodule);
	    nameoutput = (linepos = strlen (mymodule)) + 1;

	    fprintf (stderr, " %ss", yymode = mode);
	    linepos += strlen (yymode) + 1;
	    fprintf (stderr, ":");
	    linepos += 2;
    }

    len = strlen (s);
    if (linepos != nameoutput) {
	    if (len + linepos + 1 > outputlinelen) {
	        fprintf (stderr, "\n%*s", linepos = nameoutput, "");
	    } else {
	        fprintf (stderr, " "), linepos++;
	    }    
	}    
    fprintf (stderr, "%s", s);
    linepos += len;
}

/*    PASS1 */

void
pass1 (void)
{
    if (htmlflag) {
	printf ("<HTML>\n\n<HEAD>\n<TITLE>%s</TITLE>\n</HEAD>\n\n", mymodule);
	printf ("<BODY><PRE>\n");
	printf ("<A HREF=\"%s.txt\">%s</A> ", mymodule, mymodule);
	if (mymoduleid)
	    printf ("{ %s } ", oidprint (mymoduleid));
	printf ("<B>DEFINITIONS ::= BEGIN</B>\n");
	return;
    }
    
    if (oneflag) {
	printf ("%s ", mymodule);
	if (mymoduleid)
	    printf ("{ %s } ", oidprint (mymoduleid));
	printf ("DEFINITIONS ::= BEGIN\n");
	return;
    }

    printf ("-- object definitions compiled from %s", mymodule);
    if (mymoduleid)
	printf (" %s", oidprint (mymoduleid));
    printf ("\n\n");
}

/*  */

void pass1_mod (mod, id, value, update, org, contact, descr, revs)
char   *mod,
       *id;
YV     value;
char   *update,
       *org,
       *contact,
       *descr;
YR	revs;
{
    register SY	    sy;
    register YM	    ym;

    if (importedP (id))
	myyerror ("symbol %s is IMPORTED, so why are you defining it?", id);

    if ((ym = (YM) calloc (1, sizeof *ym)) == NULLYM)
	yyerror ("out of memory");

    ym -> ym_name = id;
    ym -> ym_value = value;
    ym -> ym_update = update;
    ym -> ym_org = org;
    ym -> ym_contact = contact;
    ym -> ym_descr = descr;
    ym -> ym_revs = revs;

    if (mosydebug) {
	if (linepos)
	    fprintf (stderr, "\n"), linepos = 0;

	fprintf (stderr, "%s.%s\n", mod ? mod : mymodule, id);
	print_ym (ym, 0);
	fprintf (stderr, "--------\n");
    }
    else
	yyprint_aux (id, "module");

    if (mymodules)
	yyerror ("only one invocation of MODULE-IDENTITY per module");
    else
	if (myobjects || mytraps || mygroups || mymibs || myagents)
	    yyerror ("MODULE-IDENTITY must be invoked at beginning of module");

    sy = new_symbol (NULLCP, NULLCP, NULLCP, mod, id);
    sy -> sy_ym = ym;
    mymodules = add_symbol (mymodules, sy);

    if (htmlflag) {
	if (htmlflag > 0)
	    print_expimp (), htmlflag = -1;

	printf ("<A NAME=\"%s\">%s</A> <B>MODULE-IDENTITY</B>\n",
		id, id);
	printf ("    <B>LAST-UPDATED</B> &#34;%s&#34;\n", update);
	printf ("    <B>ORGANIZATION</B> &#34;%s&#34;\n", org);
	printh_quoted ("    <B>CONTACT-INFO</B> ", contact);
	printh_quoted ("    <B>DESCRIPTION</B>  ", descr);
	for (; revs; revs = revs -> yr_next) {
	    printf ("    <B>REVISION</B>     &#34;%s&#34;\n",
		    revs -> yr_utctime);
	    printh_quoted ("    <B>DESCRIPTION</B>  ", revs -> yr_descr);
	}
	printf ("    <B>::=</B> ");
	val2prf (value, 2);
	printf ("\n\n");
	return;
    }

    if (oneflag) {
	char    *cp;

	if (oneflag > 0)
	    print_expimp (), oneflag = -1;

	while ((cp = (char *)index (update, '\n')) != NULL)
	    *cp++ = ' ';
	printf ("-- created from %s (%s)\n\n", id, update);
	printf ("%s OBJECT IDENTIFIER ::= ", id);
	val2prf (value, 2);
	printf ("\n\n");
	return;
    }
}

/*  */

void pass1_oid (mod, id, value, sname, descr, refer)
char   *mod,
       *id;
YV     value;
char   *sname,
       *descr,
       *refer;
{
    register SY	    sy;
    register OI	    yi;

    if (importedP (id))
	myyerror ("symbol %s is IMPORTED, so why are you defining it?", id);

    if ((yi = (OI) calloc (1, sizeof *yi)) == NULLOI)
	yyerror ("out of memory");

    yi -> yi_name = id;
    yi -> yi_value = value;
    yi -> yi_status = sname;
    yi -> yi_descr = descr;
    yi -> yi_refer = refer;

    if (mosydebug) {
	if (linepos)
	    fprintf (stderr, "\n"), linepos = 0;

	fprintf (stderr, "%s.%s\n", mod ? mod : mymodule, id);
	print_yi (yi, 0);
	fprintf (stderr, "--------\n");
    }
    else
	yyprint_aux (id, "identifier");

    sy = new_symbol (NULLCP, NULLCP, NULLCP, mod, id);
    sy -> sy_yi = yi;
    myidentifiers = add_symbol (myidentifiers, sy);

    if (htmlflag) {
	if (htmlflag > 0)
	    print_expimp (), htmlflag = -1;
	
	printf ("<A NAME=\"%s\">%s</A> ", id, id);
	if (descr) {
	    printf ("<B>OBJECT-IDENTITY</B>\n");
	    printf ("    <B>STATUS</B>       %s\n", sname);
	    printh_quoted ("    <B>DESCRIPTION</B>  ", descr);
	    if (refer)
		printh_quoted ("    <B>REFERENCE</B>    ", refer);
	    printf ("    ");
	}
	printf ("<B>::=</B> ");
	val2prf (value, 2);
	printf ("\n\n");
	return;
    }

    if (oneflag) {
	if (oneflag > 0)
	    print_expimp (), oneflag = -1;

	printf ("%s OBJECT IDENTIFIER ::= ", id);
	val2prf (value, 2);
	printf ("\n\n");
	return;
    }
}

/*  */

void pass1_conv (mod, id, value, display, sname, descr, refer)
char   *mod,
       *id;
YP     value;
char   *display,
       *sname,
       *descr,
       *refer;
{
    register SY	    sy;
    register TC	    tc;

    if (importedP (id))
	myyerror ("symbol %s is IMPORTED, so why are you defining it?", id);

    if ((tc = (TC) calloc (1, sizeof *tc)) == NULLTC)
	yyerror ("out of memory");

    tc -> tc_name = id;
    tc -> tc_value = value;
    tc -> tc_display = display;
    tc -> tc_status = sname;
    tc -> tc_descr = descr;
    tc -> tc_refer = refer;

    if (mosydebug) {
	if (linepos)
	    fprintf (stderr, "\n"), linepos = 0;

	fprintf (stderr, "%s.%s\n", mod ? mod : mymodule, id);
	print_tc (tc, 0);
	fprintf (stderr, "--------\n");
    }
    else
	yyprint_aux (id, "convention");

    sy = new_symbol (NULLCP, NULLCP, NULLCP, mod, id);
    sy -> sy_tc = tc;
    myconv = add_symbol (myconv, sy);

    if (htmlflag) {
	if (htmlflag > 0)
	    print_expimp (), htmlflag = -1;

	printf ("<A NAME=\"%s\">%s</A> <B>::= TEXTUAL-CONVENTION</B>\n",
		id, id);
	if (display)
	    printf ("    <B>DISPLAY-HINT</B> &#34;%s&#34;\n", display);
	printf ("    <B>STATUS</B>       %s\n", sname);
	printh_quoted ("    <B>DESCRIPTION</B>  ", descr);
	if (refer)
	    printh_quoted ("    <B>REFERENCE</B>    ", refer);
	printf ("    <B>SYNTAX</B>       ");
	print1_type (value, 4, NULLCP);
	printf ("\n\n");
	return;
    }

    if (oneflag) {
	if (oneflag > 0)
	    print_expimp (), oneflag = -1;

	printf ("%s ::=\n    ", id);
	print1_type (value, 0, NULLCP);
	printf ("\n\n");
	return;
    }
}

/*  */

void pass1_obj (mod, id, value, syntax, units, aname, sname, descr, refer, idx,
	   defval)
char   *mod;
char   *id;
YV     value;
YP     syntax;
char   *units;
char   *aname;
char   *sname;
char   *descr;
char   *refer;
YV     idx;
YV     defval;
{
    register SY	    sy;
    register OT	    yo;

    if (importedP (id))
	myyerror ("symbol %s is IMPORTED, so why are you defining it?", id);

    if ((yo = (OT) calloc (1, sizeof *yo)) == NULLOT)
	yyerror ("out of memory");

    yo -> yo_name = id;
    yo -> yo_value = value;
    yo -> yo_syntax = syntax;
    yo -> yo_units = units;
    yo -> yo_access = aname;
    yo -> yo_status = sname;
    yo -> yo_descr = descr;
    yo -> yo_refer = refer;
    yo -> yo_index = idx;
    yo -> yo_defval = defval;

    if (mosydebug) {
	if (linepos)
	    fprintf (stderr, "\n"), linepos = 0;

	fprintf (stderr, "%s.%s\n", mod ? mod : mymodule, id);
	print_yo (yo, 0);
	fprintf (stderr, "--------\n");
    }
    else
	yyprint_aux (id, "object");

    sy = new_symbol (NULLCP, NULLCP, NULLCP, mod, id);
    sy -> sy_yo = yo;
    myobjects = add_symbol (myobjects, sy);

    if (htmlflag) {
	if (htmlflag > 0)
	    print_expimp (), htmlflag = -1;

	printf ("<A NAME=\"%s\">%s</A> <B>OBJECT-TYPE</B>\n", id, id);
	printf ("    <B>SYNTAX</B>     ");
	print1_type (syntax, 4, NULLCP);
	printf ("\n");
	if (units)
	    printf ("    <B>UNITS</B>      %s\n", units);
	printf ("    %s %s\n",
		snmpV2 ? "<B>MAX-ACCESS</B>" : "<B>ACCESS</B>    ", aname);
	printf ("    <B>STATUS</B>     %s\n", sname);
	if (descr)
	    printh_quoted ("    <B>DESCRIPTION</B>  ", descr);
	if (refer)
	    printh_quoted ("    <B>REFERENCE</B>    ", refer);
	if (idx) {
	    if (idx -> yv_code == YV_IDEFINED)
		printf ("    <B>AUGMENTS</B>  { ");
	    else
		printf ("    <B>INDEX</B>      ");
	    val2prf (idx, 4);
	    if (idx -> yv_code == YV_IDEFINED)
		printf (" } ");
	    printf ("\n");
	}
	if (defval) {
	    printf ("    <B>DEFVAL</B>    { ");
	    htmlflag = -2;
	    val2prf (defval, 4);
	    htmlflag = -1;
	    printf (" }\n");
	}
	printf ("    <B>::=</B> ");
	val2prf (value, 2);
	printf ("\n\n");
	return;
    }

    if (oneflag) {
	if (oneflag > 0)
	    print_expimp (), oneflag = -1;

	printf ("%s OBJECT-TYPE\n", id);
	printf ("    SYNTAX  ");
	print1_type (syntax, 4, NULLCP);
	if (units)
	    printf ("\t-- UNITS %s\n", units);
	printf ("\n    ACCESS  %s\n",
		strcmp (aname, "read-create") ? aname : "read-write");
	printf ("    STATUS  %s\n",
		strcmp (sname, "current") ? sname : "mandatory");
	if (descr) {
	    printf ("    DESCRIPTION\n           \"");
	    fputs (descr, stdout);
	    printf ("\"\n");
	}
	if (refer) {
	    printf ("    REFERENCE\n           \"");
	    fputs (refer, stdout);
	    printf ("\"\n");
	}
	if (idx) {
	    if (idx -> yv_code == YV_IDEFINED)
		printf ("-- AUGMENTS { ");
	    else
		printf ("    INDEX   ");
	    val2prf (idx, 4);
	    if (idx -> yv_code == YV_IDEFINED)
		printf (" } ");
	    printf ("\n");
	}
	if (defval) {
	    printf ("    DEFVAL  { ");
	    val2prf (defval, 4);
	    printf (" }\n");
	}
	printf ("    ::= ");
	val2prf (value, 2);
	printf ("\n\n");
	return;
    }
}

/*  */

void pass1_trap (mod, id, enterprise, number, vars, sname, descr, refer)
char   *mod,
       *id;
YV     enterprise;
int    number;
YV     vars;
char   *sname,
       *descr,
       *refer;
{
    register SY	    sy;
    register TT	    yt;
    register YV	    yv,
		   *yp;

    if (importedP (id))
	myyerror ("symbol %s is IMPORTED, so why are you defining it?", id);

    if ((yt = (TT) calloc (1, sizeof *yt)) == NULLTT)
	yyerror ("out of memory");

    yt -> yt_name = id;
    yt -> yt_enterprise = enterprise;
    yt -> yt_number = number;
    yt -> yt_vars = vars;
    yt -> yt_status = sname;
    yt -> yt_descr = descr;
    yt -> yt_refer = refer;

    if (mosydebug) {
	if (linepos)
	    fprintf (stderr, "\n"), linepos = 0;

	fprintf (stderr, "%s.%s\n", mod ? mod : mymodule, id);
	print_yt (yt, 0);
	fprintf (stderr, "--------\n");
    }
    else
	yyprint_aux (id, "trap");

    sy = new_symbol (NULLCP, NULLCP, NULLCP, mod, id);
    sy -> sy_yt = yt;
    mytraps = add_symbol (mytraps, sy);

    if (htmlflag) {
	if (htmlflag > 0)
	    print_expimp (), htmlflag = -1;

	printf ("<A NAME=\"%s\">%s</A> <B>%s</B>\n", id, id,
		snmpV2 ? "NOTIFICATION-TYPE" : "TRAP-TYPE");
	yv = enterprise;
	if (snmpV2)
	    printf ("    <B>STATUS</B>     %s\n", sname);
	else {
	    printf ("    <B>ENTERPRISE</B> ");
	    if (yv -> yv_code != YV_OIDLIST)
		yyerror ("value of NOTIFICATON-TYPE isn't an object identifier");
	    for (yp = &yv -> yv_idlist;; yp = &yv -> yv_next)
		if (!(yv = *yp) -> yv_next)
		    break;
	    *yp = NULL, val2prf (enterprise, 4), *yp = yv;
	    printf ("\n");
	}
	if (vars) {
	    printf ("    %s  ",
	       snmpV2 ? "<B>OBJECTS</B>    " : "<B>VARIABLES</B>  ");
	    val2prf (vars, 4);
	    printf ("\n");
	}
	if (descr)
	    printh_quoted ("    <B>DESCRIPTION</B>  ", descr);
	if (refer)
	    printh_quoted ("    <B>REFERENCE</B>    ", refer);
	printf ("    <B>::=</B> ");
	val2prf (yv, 2);
	printf ("\n\n");
	return;
    }

    if (oneflag) {
	if (oneflag > 0)
	    print_expimp (), oneflag = -1;

	printf ("%s TRAP-TYPE\n", id);
	printf ("    ENTERPRISE  ");
	if ((yv = enterprise) -> yv_code != YV_OIDLIST)
	    yyerror ("value of NOTIFICATON-TYPE isn't an object identifier");
	for (yp = &yv -> yv_idlist;; yp = &yv -> yv_next)
	    if (!(yv = *yp) -> yv_next)
		break;
	*yp = NULL, val2prf (enterprise, 4), *yp = yv;
	printf ("\n");
	if (vars) {
	    printf ("    VARIABLES   ");
	    val2prf (vars, 4);
	    printf ("\n");
	}
	if (descr) {
	    printf ("    DESCRIPTION\n           \"");
	    fputs (descr, stdout);
	    printf ("\"\n");
	}
	if (refer) {
	    printf ("    REFERENCE\n           \"");
	    fputs (descr, stdout);
	    printf ("\"\n");
	}
	printf ("    ::= ");
	val2prf (yv, 2);
	printf ("\n\n");
    } else {   /* DAR 8-15-95 */
        printf("%%trap %s ", id);
        printf("%d ", number);
#ifdef MOSY_DESCR
        printf("%s ", sname);
#endif /* MOSY_DESCR */
        printf("%s ", id2str (enterprise));
        if (vars) {
            printf(" ");
            val2prf (vars, 0);
        }
#ifdef MOSY_DESCR
        printf("\n%%%-19s %-16s\n%s%c\n", "et", yt -> yt_name, yt -> yt_descr, '"'); 
#endif /* MOSY_DESCR */
        printf("\n");
    }
}

/*  */

void
pass1_grp (mod, id, value, objects, sname, descr, refer, notify)
char   *mod,
       *id;
YV     value,
       objects;
char   *sname,
       *descr,
       *refer;
int	notify;
{
    register SY	    sy;
    register OG	    yg;

    if (importedP (id))
	myyerror ("symbol %s is IMPORTED, so why are you defining it?", id);

    if ((yg = (OG) calloc (1, sizeof *yg)) == NULLOG)
	yyerror ("out of memory");

    yg -> yg_name = id;
    yg -> yg_value = value;
    yg -> yg_objects = objects;
    yg -> yg_status = sname;
    yg -> yg_descr = descr;
    yg -> yg_refer = refer;
    yg -> yg_notify = notify;

    if (mosydebug) {
	if (linepos)
	    fprintf (stderr, "\n"), linepos = 0;

	fprintf (stderr, "%s.%s\n", mod ? mod : mymodule, id);
	print_yg (yg, 0);
	fprintf (stderr, "--------\n");
    }
    else
	yyprint_aux (id, "group");

    sy = new_symbol (NULLCP, NULLCP, NULLCP, mod, id);
    sy -> sy_yg = yg;
    mygroups = add_symbol (mygroups, sy);

    if (htmlflag) {
	if (htmlflag > 0)
	    print_expimp (), htmlflag = -1;

	printf ("<A NAME=\"%s\">%s</A> <B>%s-GROUP</B>\n", id, id,
		notify ? "NOTIFICATION" : "OBJECT");
	printf ("    <B>OBJECTS</B>    ");
	val2prf (objects, 4);
	printf ("\n");
	printf ("    <B>STATUS</B>     %s\n", sname);
	printh_quoted ("    <B>DESCRIPTION</B>  ", descr);
	if (refer)
	    printh_quoted ("    <B>REFERENCE</B>    ", refer);
	printf ("    <B>::=</B> ");
	val2prf (value, 2);
	printf ("\n\n");
	return;
    }

    if (oneflag) {
	if (oneflag > 0)
	    print_expimp (), oneflag = -1;

	printf ("%s OBJECT IDENTIFIER ::= ", id);
	val2prf (value, 2);
	printf ("\n\n");
	return;
    }
}

/*  */

void pass1_comp (mod, id, value, sname, descr, refer, comp)
char   *mod,
       *id;
YV     value;
char   *sname,
       *descr,
       *refer;
YD	comp;
{
    register SY	    sy;
    register MC	    mc;
    register YD	    yd;
    /* register YP	    yp; */

    if (importedP (id))
	myyerror ("symbol %s is IMPORTED, so why are you defining it?", id);

    if ((mc = (MC) calloc (1, sizeof *mc)) == NULLMC)
	yyerror ("out of memory");

    mc -> mc_name = id;
    mc -> mc_value = value;
    mc -> mc_status = sname;
    mc -> mc_descr = descr;
    mc -> mc_refer = refer;
    mc -> mc_comp = comp;

    if (mosydebug) {
	if (linepos)
	    fprintf (stderr, "\n"), linepos = 0;

	fprintf (stderr, "%s.%s\n", mod ? mod : mymodule, id);
	print_mc (mc, 0);
	fprintf (stderr, "--------\n");
    }
    else
	yyprint_aux (id, "compliance");

    sy = new_symbol (NULLCP, NULLCP, NULLCP, mod, id);
    sy -> sy_mc = mc;
    mymibs = add_symbol (mymibs, sy);

    if (htmlflag) {
	if (htmlflag > 0)
	    print_expimp (), htmlflag = -1;

	printf ("<A NAME=\"%s\">%s</A> <B>MODULE-COMPLIANCE</B>\n", id, id);
	printf ("    <B>STATUS</B>     %s\n", sname);
	printh_quoted ("    <B>DESCRIPTION</B>  ", descr);
	if (refer)
	    printh_quoted ("    <B>REFERENCE</B>    ", refer);
	printf ("\n");
	for (yd = mc -> mc_comp; yd; yd = yd -> yd_next) {
	    YC	    yc;

	    printf ("    <B>MODULE</B>");
	    if (yd -> yd_modname)
		printf ("           <A HREF=\"%s.txt\">%s</A>\n",
			yd -> yd_modname, yd -> yd_modname);
	    else
		printf ("           -- this module\n");
	    printf ("    <B>MANDATORY-GROUPS</B> ");
	    val2prf1 (yd -> yd_modname, yd -> yd_groups, 7);
	    printf ("\n");
	    for (yc = yd -> yd_variations; yc; yc = yc -> yc_next) {
		printf ("        %s",
			yc -> yc_type == YC_GROUP  ? "<B>GROUP</B>        "
			                           : "<B>OBJECT</B>       ");
		id2html (yd -> yd_modname, yc -> yc_target);
		printf ("\n");
		if (yc -> yc_syntax) {
		    printf ("        <B>SYNTAX</B>       ");
		    print1_type (yc -> yc_syntax, 8, NULLCP);
		    printf ("\n");
		}
		if (yc -> yc_wsyntax) {
		    printf ("        <B>WRITE-SYNTAX</B> ");
		    print1_type (yc -> yc_wsyntax, 8, NULLCP);
		    printf ("\n");
		}
		if (yc -> yc_access)
		    printf ("        <B>MIN-ACCESS</B>   %s\n",
			    yc -> yc_access);
		printh_quoted ("        <B>DESCRIPTION</B>  ", yc -> yc_descr);
		if (yc -> yc_next || yd -> yd_next)
		    printf ("\n");
	    }
	}
	printf ("    <B>::=</B> ");
	val2prf (value, 2);	
	printf ("\n\n");
	return;
    }

    if (oneflag) {
	if (oneflag > 0)
	    print_expimp (), oneflag = -1;

	printf ("%s OBJECT IDENTIFIER ::= ", id);
	val2prf (value, 2);
	printf ("\n\n");
	return;
    }
}

/*  */

void pass1_cap (mod, id, value, release, sname, descr, refer, comp)
char   *mod,
       *id;
YV     value;
char   *release,
       *sname,
       *descr,
       *refer;
YD	comp;
{
    register SY	    sy;
    register AC	    ac;
    register YD	    yd;

    if (importedP (id))
	myyerror ("symbol %s is IMPORTED, so why are you defining it?", id);

    if ((ac = (AC) calloc (1, sizeof *ac)) == NULLAC)
	yyerror ("out of memory");

    ac -> ac_name = id;
    ac -> ac_value = value;
    ac -> ac_release = release;
    ac -> ac_status = sname;
    ac -> ac_descr = descr;
    ac -> ac_refer = refer;
    ac -> ac_comp = comp;

    if (mosydebug) {
	if (linepos)
	    fprintf (stderr, "\n"), linepos = 0;

	fprintf (stderr, "%s.%s\n", mod ? mod : mymodule, id);
	print_ac (ac, 0);
	fprintf (stderr, "--------\n");
    }
    else
	yyprint_aux (id, "capabilitie");

    sy = new_symbol (NULLCP, NULLCP, NULLCP, mod, id);
    sy -> sy_ac = ac;
    myagents = add_symbol (myagents, sy);

    if (htmlflag) {
	if (htmlflag > 0)
	    print_expimp (), htmlflag = -1;

	printf ("<A NAME=\"%s\">%s</A> <B>AGENT-CAPABILITIES</B>\n", id, id);
	printf ("    <B>PRODUCT-RELEASE</B> &#34;%s&#34;\n", release);
	printf ("    <B>STATUS</B>          %s\n", sname);
	printh_quoted ("    <B>DESCRIPTION</B>     ", descr);
	if (refer)
	    printh_quoted ("    <B>REFERENCE</B>       ", refer);
	printf ("\n");
	for (yd = ac -> ac_comp; yd; yd = yd -> yd_next) {
	    YC	    yc;

	    printf ("    <B>SUPPORTS</B>           <A HREF=\"%s.txt\">%s</A>\n",
		    yd -> yd_modname, yd -> yd_modname);
	    printf ("    <B>INCLUDES</B>           ");
	    val2prf1 (yd -> yd_modname, yd -> yd_groups, 6);
	    printf ("\n");
	    if (!yd -> yd_variations && yd -> yd_next)
		printf ("\n");
	    for (yc = yd -> yd_variations; yc; yc = yc -> yc_next) {
		printf ("        <B>VARIATION</B>      ");
		id2html (yd -> yd_modname, yc -> yc_target);
		printf ("\n");
		if (yc -> yc_syntax) {
		    printf ("        <B>SYNTAX</B>         ");
		    print1_type (yc -> yc_syntax, 8, NULLCP);
		    printf ("\n");
		}
		if (yc -> yc_wsyntax) {
		    printf ("        <B>WRITE-SYNTAX</B>   ");
		    print1_type (yc -> yc_wsyntax, 8, NULLCP);
		    printf ("\n");
		}
		if (yc -> yc_access)
		    printf ("        <B>ACCESS</B>         %s\n",
			    yc -> yc_access);
		if (yc -> yc_creation) {
		    /* printf ("        <B>CREATION-REQUIRES</B> %s\n"); */
		    printf ("        <B>CREATION-REQUIRES</B> \n");
		    val2prf1 (yd -> yd_modname, yc -> yc_creation, 4);
		    printf ("\n");
		}
		if (yc -> yc_defval) {
		    /* printf ("        <B>DEFVAL</B>         %s\n", yc -> yc_defval); */
		    printf ("        <B>DEFVAL</B>         \n");
		    htmlflag = -2;
		    val2prf1 (yd -> yd_modname, yc -> yc_defval, 4);
		    htmlflag = -1;
		    printf ("\n");
		}
		printh_quoted ("        <B>DESCRIPTION</B>    ",
			       yc -> yc_descr);
		if (yc -> yc_next || yd -> yd_next)
		    printf ("\n");
	    }
	}
	printf ("    <B>::=</B> ");
	val2prf (value, 2);	
	printf ("\n\n");
	return;
    }

    if (oneflag) {
	if (oneflag > 0)
	    print_expimp (), oneflag = -1;

	printf ("%s OBJECT IDENTIFIER ::= ", id);
	val2prf (value, 2);
	printf ("\n\n");
	return;
    }
}

/*  */

void pass1_type (encpref, decpref, prfpref, mod, id, yp)
char   *encpref,
       *decpref,
       *prfpref,
       *mod,
       *id;
register YP	yp;
{
    register SY	    sy;

    if (importedP (id))
	myyerror ("symbol %s is IMPORTED, so why are you defining it?", id);

    if (dflag && lookup_type (mod, id))	/* no duplicate entries, please... */
	return;

    if (mosydebug) {
	if (linepos)
	    fprintf (stderr, "\n"), linepos = 0;

	fprintf (stderr, "%s.%s\n", mod ? mod : mymodule, id);
	print_type (yp, 0);
	fprintf (stderr, "--------\n");
    }
    else
	if (!(yp -> yp_flags & YP_IMPORTED))
	    yyprint_aux (id, "type");

    sy = new_symbol (encpref, decpref, prfpref, mod, id);
    sy -> sy_yp = yp;
    mytypes = add_symbol (mytypes, sy);

    if (htmlflag) {
	if (htmlflag > 0)
	    print_expimp (), htmlflag = -1;

	printf ("<A NAME=\"%s\">%s</A> <B>::=</B>\n    ", id, id);
	print1_type (yp, 0, NULLCP);
	printf ("\n\n");
	return;
    }

    if (oneflag) {
	if (oneflag > 0)
	    print_expimp (), oneflag = -1;

	printf ("%s ::=\n    ", id);
	print1_type (yp, 0, NULLCP);
	printf ("\n\n");
	return;
    }
}

/*    PASS2 */

void
pass2 (void) {
    register SY	    sy;

    if (!sflag)
	(void) fflush (stderr);

    sr_delay = 1;
    didtcl = tcl ? 0 : 1;

    if (snmpV2
	    && !mymodules
	    && (myobjects || mytraps || mygroups || mymibs || myagents))
	yyerror ("MODULE-IDENTITY must be invoked at beginning of module");

    if (htmlflag) {
	if (htmlflag > 0)
	    print_expimp (), htmlflag = -1;

	printf ("<B>END</B>\n</PRE></BODY>\n\n</HTML>\n");
	goto all_done;
    }

    if (oneflag) {
	if (oneflag > 0)
	    print_expimp (), oneflag = -1;

	if (!snmpV2)
	    yyerror ("not an SNMPv2 MIB module!");

	printf ("END\n");
	goto all_done;
    }

    yymode = "module";
    for (sy = mymodules; sy; sy = sy -> sy_next) {
	if (sy -> sy_module == NULLCP)
	    yyerror ("no module name associated with symbol");

	do_mod (sy -> sy_ym, eval = sy -> sy_name);
    }
    if (mymodules)
	printf ("\n");

    yymode = "identifier";
    for (sy = myidentifiers; sy; sy = sy -> sy_next) {
	if (sy -> sy_module == NULLCP)
	    yyerror ("no module name associated with symbol");

	do_oid (sy -> sy_yi, eval = sy -> sy_name);
    }
    if (myidentifiers)
	printf ("\n");

    yymode = "convention";
    for (sy = myconv; sy; sy = sy -> sy_next) {
	if (sy -> sy_module == NULLCP)
	    yyerror ("no module name associated with symbol");

	do_conv (sy -> sy_tc, eval = sy -> sy_name);
    }

    yymode = "object";
    for (sy = myobjects; sy; sy = sy -> sy_next) {
	if (sy -> sy_module == NULLCP)
	    yyerror ("no module name associated with symbol");

	do_obj (sy -> sy_yo, eval = sy -> sy_name);
    }
    if (myobjects)
	printf ("\n");

    yymode = "trap";
    for (sy = mytraps; sy; sy = sy -> sy_next) {
	if (sy -> sy_module == NULLCP)
	    yyerror ("no module name associated with symbol");

	do_trap (sy -> sy_yt, eval = sy -> sy_name);
    }
    if (mytraps)
	printf ("\n");

    yymode = "group";
    for (sy = mygroups; sy; sy = sy -> sy_next) {
	if (sy -> sy_module == NULLCP)
	    yyerror ("no module name associated with symbol");

	do_grp (sy -> sy_yg, eval = sy -> sy_name);
    }
    if (mygroups)
	printf ("\n");

    yymode = "compliance";
    for (sy = mymibs; sy; sy = sy -> sy_next) {
	if (sy -> sy_module == NULLCP)
	    yyerror ("no module name associated with symbol");

	do_mib (sy -> sy_mc, eval = sy -> sy_name);
    }
    if (mymibs)
	printf ("\n");

    yymode = "capability";
    for (sy = myagents; sy; sy = sy -> sy_next) {
	if (sy -> sy_module == NULLCP)
	    yyerror ("no module name associated with symbol");

	do_agent (sy -> sy_ac, eval = sy -> sy_name);
    }
    if (myagents)
	printf ("\n");

all_done: ;
    (void) fflush (stdout);

    if (ferror (stdout))
	myyerror ("write error - %s", sys_errname (errno));

    if (tcl && ferror (tcl)) {
	myyerror ("write error - %s", sys_errname (errno));
	(void) fclose (tcl);
	tcl = NULL;
    }

    if (!iflag && sr_delay < 0) {
	if (*sysout)
	    (void) unlink (sysout);
	if (tcl)
	    (void) unlink (systcl);

	exit (1);
    }

    sr_delay = 0;
}

/*  */

/* ARGSUSED */

static void do_mod (ym, id)
register YM	ym;
char   *id;
{
    if (!didtcl)
	do_tcl (ym -> ym_name);
    printf ("%-20s %s\n", ym -> ym_name, id2str (ym -> ym_value));
    printf ("%%n0 %-16s module-identity\n", ym -> ym_name);
#ifdef MOSY_DESCR
    printf ("%%%-19s %s\n", "ed", ym -> ym_name);
    printf ("%s\"\n", ym -> ym_descr);
#endif /* MOSY_DESCR */
}

/*  */

/* ARGSUSED */

static void do_oid (yi, id)
register OI	yi;
char   *id;
{
    if (yi -> yi_status) {
	if (strcmp (yi -> yi_status, "current")
		&& strcmp (yi -> yi_status, "deprecated")
	        && strcmp (yi -> yi_status, "obsolete"))
	    yyerror ("value of STATUS clause isn't a valid keyword");
    }

    if (!didtcl)
	do_tcl (yi -> yi_name);
    printf ("%-20s %s\n", yi -> yi_name, id2str (yi -> yi_value));
    printf ("%%n0 %-16s object-id\n", yi -> yi_name);
}

/*  */

/* ARGSUSED */

static void do_conv (tc, id)
register TC	tc;
char   *id;
{
    YP	    yp = tc -> tc_value;
    YV	    yv;
    YV	    yv2;   /* DAR June 23, 1994 */
    struct ysub *ysub;   /* DAR June 23, 1994 */

    id = check_syntax (tc -> tc_name, &yp);

    if (strcmp (tc -> tc_status, "current")
	    && strcmp (tc -> tc_status, "deprecated")
	    && strcmp (tc -> tc_status, "obsolete"))
	yyerror ("value of STATUS clause isn't a valid keyword");

    printf ("%%%-19s %-16s %-15s \"%s\"\n", "tc", tc -> tc_name, id,
	    tc -> tc_display ? tc -> tc_display : "");

    switch (yp -> yp_code) {
        case YP_INT:           /* DAR June 23, 1994 */
try_int: ;
            if ((yp -> yp_flags & YP_SUBTYPE)
                    && !(ysub = yp -> yp_subtype) -> ysub_next
                    && ysub -> ysub_code == YS_VALUERANGE
                    && (yv = ysub -> ysub_lower -> ep_value)
                    && yv -> yv_code == YV_NUMBER
                    && (yv2 = ysub -> ysub_upper -> ep_value)
                    && yv2 -> yv_code == YV_NUMBER
                    && yv -> yv_number <= yv2 -> yv_number)
                printf ("%%%-19s %-16s %-15d %d\n", "er", tc -> tc_name,
                        yv -> yv_number, yv2 -> yv_number);
            break;

        case YP_OCT:		/* DAR Feb 12, 1996 */
try_oct: ;
            if ((yp -> yp_flags & YP_SUBTYPE)
                    && !(ysub = yp -> yp_subtype) -> ysub_next
                    && ysub -> ysub_code == YS_SIZE
                    && (ysub = ysub -> ysub_subtype))
                /*
                 * DAR, 8-22-95 -- added this do while loop to
                 * print everything in the size clause when a |
                 * is used, rather than just printing the first one
                 */
                do {
                    if (ysub -> ysub_code == YS_VALUERANGE) {
                        if ((yv = ysub -> ysub_lower -> ep_value)
                                && yv -> yv_code == YV_NUMBER
                                && (yv2 = ysub -> ysub_upper -> ep_value)
                                && yv -> yv_number <= yv2 -> yv_number)
                            printf ("%%%-19s %-16s %-15d %d\n", "er",
                                    tc -> tc_name, yv -> yv_number,
                                    yv2 -> yv_number);
                    }
                    else
                        if (ysub -> ysub_code == YS_VALUE
                                && (yv = ysub -> ysub_value)
                                && yv -> yv_code == YV_NUMBER)
                            printf ("%%%-19s %-16s %-15d %d\n", "er",
                                    tc -> tc_name, yv -> yv_number,
                                    yv -> yv_number);
                } while ((ysub = ysub->ysub_next) != NULL);
            break;

	case YP_INTLIST:
	case YP_BITS:
	    for (yv = yp -> yp_value; yv; yv = yv -> yv_next)
		if (yv -> yv_code == YV_NUMBER)
		    printf ("%%%-19s %-16s %-15s %d\n", "es",
			    tc -> tc_name, yv -> yv_named, yv -> yv_number);
		break;

        case YP_IDEFINED:      /* DAR Feb 26, 1996 */
            if (yp -> yp_flags & YP_SUBTYPE)
                switch (yp -> yp_subtype -> ysub_code) {
                    case YS_VALUERANGE:
                        goto try_int;

                    case YS_SIZE:
                        goto try_oct;

                    default:
                        break;
                }
            break;

	default:
	    break;
    }
}

/*  */

/* ARGSUSED */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* *** THIS IS WHERE THE OUTPUT FILE IS GENERATED *** */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static void do_obj (yo, id)
register OT	yo;
char   *id;
{
    register YP	    yz;
    YP	    yp;
    register YV	    yv;
    YV	    yv2;
    struct ysub *ysub;

    if (snmpV2) {
	register char *cp;

	for (cp = yo -> yo_name; *cp; cp++)
	    if (*cp == '-') {
		warning ("object type %s contains a `-' in its descriptor",
			 yo -> yo_name);
		break;
	    }
    }
    if (!didtcl)
	do_tcl (yo -> yo_name);
    printf ("%-20s %-16s ", yo -> yo_name, id2str (yo -> yo_value));

    if ((yp = yo -> yo_syntax) == NULLYP) {
	yyerror ("no syntax associated with object type");
	return;
    }
    id = check_syntax (yo -> yo_name, &yp);

    if (snmpV2) {
	if (strcmp (yo -> yo_access, "read-only")
	        && strcmp (yo -> yo_access, "accessible-for-notify")
	        && strcmp (yo -> yo_access, "read-write")
	        && strcmp (yo -> yo_access, "read-create")
	        && strcmp (yo -> yo_access, "accessible-for-notify")
	        && strcmp (yo -> yo_access, "not-accessible"))
	    yyerror ("value of MAX-ACCESS clause isn't a valid keyword");
    }
    else
	if (strcmp (yo -> yo_access, "read-only")
	        && strcmp (yo -> yo_access, "accessible-for-notify")
	        && strcmp (yo -> yo_access, "read-write")
	        && strcmp (yo -> yo_access, "write-only")
	        && strcmp (yo -> yo_access, "not-accessible"))
	    yyerror ("value of ACCESS clause isn't a valid keyword");
    switch (yp -> yp_code) {
        case YP_SEQTYPE:
	case YP_SEQLIST:
	    if (strcmp (yo -> yo_access, "not-accessible"))
		yyerror ("value of ACCESS clause isn't not-accessible");
	    /* and fall... */

	case YP_IDEFINED:
	    if (snmpV2 && (strcmp (id, "Counter32") == 0
			    || strcmp (id, "Counter64") == 0)
		    && strcmp (yo -> yo_access, "read-only")
		    && strcmp (yo -> yo_access, "accessible-for-notify")
		    && strcmp (yo -> yo_access, "not-accessible"))
		yyerror ("value of MAX-ACCESS clause should be \"read-only\" or \"not-accessible\"");
	    break;

	default:
	    break;
    }

    if (snmpV2) {
	if (strcmp (yo -> yo_status, "current")
	        && strcmp (yo -> yo_status, "deprecated")
	        && strcmp (yo -> yo_status, "obsolete"))
	    yyerror ("value of STATUS clause isn't a valid keyword");
    }
    else
	if (strcmp (yo -> yo_status, "mandatory")
	        && strcmp (yo -> yo_status, "optional")
	        && strcmp (yo -> yo_status, "deprecated")
	        && strcmp (yo -> yo_status, "obsolete"))
	    yyerror ("value of STATUS clause isn't a valid keyword");

    printf ("%-15s %-15s %s\n", id, yo -> yo_access, yo -> yo_status);
    print_defval(yo, id);            /* DAR  11/29/95   print defval */
#ifdef MOSY_DESCR
    /* FM 8/25/98 */
    printf("%%%-19s %-16s\n%s%c\n", "ed", yo -> yo_name, yo -> yo_descr, '"'); 
#endif /* MOSY_DESCR */

    switch (yp -> yp_code) {
	case YP_INT:
try_int: ;
        if (yp && yp->yp_identifier && !strcmp (yp->yp_identifier, "Unsigned32")) {

            ysub = yp->yp_subtype;
            yv   = ysub->ysub_lower->ep_value;
            yv2  = ysub->ysub_upper->ep_value;
		    
            printf ("%%%-19s %-16s %-15u %u\n", "er", yo -> yo_name,
			        yv->yv_number, yv2->yv_number);

        } else {
        
	        if ((yp -> yp_flags & YP_SUBTYPE)
		        && !(ysub = yp -> yp_subtype) -> ysub_next
		        && ysub -> ysub_code == YS_VALUERANGE
		        && (yv = ysub -> ysub_lower -> ep_value)
		        && yv -> yv_code == YV_NUMBER
		        && (yv2 = ysub -> ysub_upper -> ep_value)
		        && yv2 -> yv_code == YV_NUMBER
		        && yv -> yv_number <= yv2 -> yv_number) {
		    
		            printf ("%%%-19s %-16s %-15d %d\n", "er", yo -> yo_name,
			            yv -> yv_number, yv2 -> yv_number);
			
		    }
        
        }
	    break;

	case YP_INTLIST:
	case YP_BITS:
	    for (yv = yp -> yp_value; yv; yv = yv -> yv_next)
		if (yv -> yv_code == YV_NUMBER)
		    printf ("%%%-19s %-16s %-15s %d\n", "ev",
			    yo -> yo_name, yv -> yv_named, yv -> yv_number);
		break;

	case YP_OCT:
try_oct: ;
	    if ((yp -> yp_flags & YP_SUBTYPE)
		    && !(ysub = yp -> yp_subtype) -> ysub_next
		    && ysub -> ysub_code == YS_SIZE
		    && (ysub = ysub -> ysub_subtype))
		/*
		 * DAR, 8-22-95 -- added this do while loop to
		 * print everything in the size clause when a |
		 * is used, rather than just printing the first one
		 */
                do {
		    if (ysub -> ysub_code == YS_VALUERANGE) {
		    
		        if ((yv = ysub -> ysub_lower -> ep_value)
		                && yv -> yv_code == YV_NUMBER
		                && (yv2 = ysub -> ysub_upper -> ep_value)
		                && yv -> yv_number <= yv2 -> yv_number) {
		                
		            printf ("%%%-19s %-16s %-15d %d\n", "er",
			    	    yo -> yo_name, yv -> yv_number,
				        yv2 -> yv_number);
				}    
				    
		    }
		    else
		        if (ysub -> ysub_code == YS_VALUE
			        && (yv = ysub -> ysub_value)
			        && yv -> yv_code == YV_NUMBER)
			    printf ("%%%-19s %-16s %-15d %d\n", "er",
				    yo -> yo_name, yv -> yv_number,
				    yv -> yv_number);
                } while ((ysub = ysub->ysub_next) != NULL);
	    break;

	case YP_IDEFINED:
	    if (yp -> yp_flags & YP_SUBTYPE)
		switch (yp -> yp_subtype -> ysub_code) {
		    case YS_VALUERANGE:
		        goto try_int;

		    case YS_SIZE:
			goto try_oct;

		    default:
			break;
		}
	    break;

	default:
	    break;
    }

    if (!yo -> yo_descr) {

	    if (snmpV2) {
	        yyerror ("missing DESCRIPTION clause");
	    } else {
	        warning ("object type %s should have a DESCRIPTION clause",
		         yo -> yo_name);
		}         
    }

    if (yo -> yo_index) {
	if (yp -> yp_code != YP_SEQLIST)
	    if (yo -> yo_index -> yv_code == YV_VALIST)
		yyerror ("INDEX clause should not be present");
	    else
		yyerror ("AUGMENTS clause should not be present");
	else
	    if (yo -> yo_index -> yv_code == YV_VALIST) {
		char   *cp = "";

		check_objects (yo -> yo_index, "INDEX", snmpV2 ? 0 : 1, 0);
		for (yv = yo -> yo_index -> yv_idlist; yv; yv = yv -> yv_next)
		    if (yv -> yv_flags & YV_IMPLIED) {
			if (yv -> yv_next)
			    yyerror ("IMPLIED used on some object other than the last in INDEX clause");
		    }

		printf ("%%%-19s %-16s \"", "ei", yo -> yo_name);
		for (yv = yo -> yo_index -> yv_idlist;
		         yv;
		         yv = yv -> yv_next, cp = " ")
		    printf ("%s%s%s", cp,
			    yv -> yv_flags & YV_IMPLIED ? "*" : "",
			    yv -> yv_identifier ? yv -> yv_identifier : "UNKNOWN");
		printf ("\"\n");
	    }
	    else {
		check_object (yo -> yo_index, "AUGMENTS", 0, 0);
		printf ("%%%-19s %-16s %s\n", "ea", yo -> yo_name,
			yo -> yo_index -> yv_identifier);
	    }
    }

    if (yp -> yp_code == YP_SEQLIST) {

	if (!yo -> yo_index) {
	    if (snmpV2) {
		    yyerror ("missing INDEX clause");
	    } else {
		    warning ("object type %s should have an INDEX clause",
			    yo -> yo_name);
        }			    
    }
    
	for (yz = yp -> yp_type; yz; yz = yz -> yp_next) {
	    register YP	y = yz;

check_again: ;
	    switch (y -> yp_code) {
		case YP_INT:
		case YP_INTLIST:
		case YP_OCT:
		case YP_OID:
		case YP_BITS:
		case YP_NULL:
		    break;

	        case YP_IDEFINED:
		    if ((y = lookup_type (y -> yp_module, y -> yp_identifier)) != NULL)
			goto check_again;
		    break;

	        default:
		    yyerror ("invalid element in SEQUENCE");
		    goto done_sequence;
	    }

	    if (!(yz -> yp_flags & YP_ID)) {
		yyerror ("element in SEQUENCE missing tag");
		goto done_sequence;
	    }
	    if (lookup_object (NULLCP, yz -> yp_id) == NULL)
		myyerror ("no object type corresponding to tag in SEQUENCE: %s",
			  yz -> yp_id);
	}
done_sequence: ;
    }

    if ((yv = yo -> yo_value) -> yv_code != YV_OIDLIST) {
	yyerror ("value of object type isn't an object identifier");
	return;
    }
    for (yv = yv -> yv_idlist; yv; yv = yv -> yv_next)
	if (yv -> yv_code == YV_NUMBER && yv -> yv_number <= 0)
	    myyerror ("object identifier contains non-positive element: %d",
		      yv -> yv_number);
    if ((yv = yo -> yo_value -> yv_idlist)
	    && yv -> yv_code == YV_IDEFINED
	    && yv -> yv_next
	    && yv -> yv_next -> yv_code == YV_NUMBER
	    && !yv -> yv_next -> yv_next) {
	OT	ot;

	if ((ot = lookup_object (NULLCP, yv -> yv_identifier)) != NULL) {
	    yz = ot -> yo_syntax;
	    while (yz -> yp_code == YP_IDEFINED)
		if (!(yz = lookup_type (yz -> yp_module, yz -> yp_identifier)))
		    break;
	    if (yz && yz -> yp_code == YP_SEQLIST)
		for (yz = yz -> yp_type; yz; yz = yz -> yp_next)
		    if ((yz -> yp_flags & YP_ID)
			    && strcmp (yo -> yo_name, yz -> yp_id) == 0)
			break;
		if (!yz)
		    myyerror ("object type not contained in defining SEQUENCE: %s",
			     yv -> yv_identifier);
	}
    }
}

/*  */

/* ARGSUSED */

static void do_trap (yt, id)
register TT	yt;
char   *id;
{
    if (snmpV2) {
	if (strcmp (yt -> yt_status, "current")
	        && strcmp (yt -> yt_status, "deprecated")
	        && strcmp (yt -> yt_status, "obsolete"))
	    yyerror ("value of STATUS clause isn't a valid keyword");

	if (!didtcl)
	    do_tcl (yt -> yt_name);
	printf ("%-20s %s\n", yt -> yt_name, id2str (yt -> yt_enterprise));
	printf ("%%n0 %-16s notification\n", yt -> yt_name);
    }
    else {
	register YV	yv;

	if ((yv = yt -> yt_enterprise) == NULLYV) {
	    yyerror ("no enterprise associated with trap type");
	    goto done_enterprise;
	}
	if (yv -> yv_code != YV_OIDLIST) {
	    yyerror ("value of ENTERPRISE clause isn't an object identifier");
	    goto done_enterprise;
	}
	for (yv = yv -> yv_idlist; yv; yv = yv -> yv_next)
	    if (yv -> yv_code == YV_NUMBER && yv -> yv_number <= 0)
		myyerror ("object identifier in ENTERPRISE clause contains non-positive element: %d",
			  yv -> yv_number);
	if ((yv = yt -> yt_enterprise -> yv_idlist)
	        && yv -> yv_code == YV_IDEFINED
	        && !yv -> yv_next
	        && !lookup_object (NULLCP, yv -> yv_identifier)
	        && !lookup_identifier (NULLCP, yv -> yv_identifier)
	        && !lookup_group (NULLCP, yv -> yv_identifier)
		&& !importedP (yv -> yv_identifier))
	    myyerror ("value in ENTERPRISE clause is undefined: %s",
		      yv -> yv_identifier);

done_enterprise: ;
	if (yt -> yt_number < 0)
	    myyerror ("value of trap type isn't non-negative integer");
    }

    if (yt -> yt_vars)
	check_objects (yt -> yt_vars, snmpV2 ? "OBJECTS" : "VARIABLES", 0, 0);
}

/*  */

/* ARGSUSED */

static void do_grp (yg, id)
register OG	yg;
char   *id;
{
    register YV	    yv;

    if (strcmp (yg -> yg_status, "current")
	    && strcmp (yg -> yg_status, "deprecated")
	    && strcmp (yg -> yg_status, "obsolete"))
	yyerror ("value of STATUS clause isn't a valid keyword");

    if (!didtcl)
	do_tcl (yg -> yg_name);
    printf ("%-20s %s\n", yg -> yg_name, id2str (yg -> yg_value));
    printf ("%%n0 %-16s %s-group\n", yg -> yg_name,
	    yg -> yg_notify ? "notification" : "object");
    check_objects (yg -> yg_objects, yg -> yg_notify ? "NOTIFICATIONS"
						     : "OBJECTS", 0,
		   yg -> yg_notify);

    if (tcl) {
	fprintf (tcl, "\nsnmpmisc mibgroup %s %s", mymodule, yg -> yg_name);
	for (yv = yg -> yg_objects -> yv_idlist; yv; yv = yv -> yv_next)
	    fprintf (tcl, " \\\n    %s", yv -> yv_identifier);
	fprintf (tcl, "\n");
    }
}

/*  */

/* ARGSUSED */

static void do_mib (mc, id)
register MC	mc;
char   *id;
{
    register YD	    yd;

    if (strcmp (mc -> mc_status, "current")
	    && strcmp (mc -> mc_status, "deprecated")
	    && strcmp (mc -> mc_status, "obsolete"))
	yyerror ("value of STATUS clause isn't a valid keyword");

    if (!didtcl)
	do_tcl (mc -> mc_name);
    printf ("%-20s %s\n", mc -> mc_name, id2str (mc -> mc_value));
    printf ("%%n0 %-16s module-compliance\n", mc -> mc_name);

    for (yd = mc -> mc_comp; yd; yd = yd -> yd_next) {
	YC	yc;

/* might check mandatory groups: yd_groups */

/* might check to see that each object occurs in at least one group... */

	for (yc = yd -> yd_variations; yc; yc = yc -> yc_next)
	    switch (yc -> yc_type) {
		case YC_GROUP:
		    break;

		case YC_OBJECT:
/*  check syntax & write-syntax: yc_syntax & yc_wsyntax */

		    if (yc -> yc_access
			    && strcmp (yc -> yc_access, "read-only")
			    && strcmp (yc -> yc_access, "accessible-for-notify")
			    && strcmp (yc -> yc_access, "read-write")
			    && strcmp (yc -> yc_access, "read-create")
			    && strcmp (yc -> yc_access,
				       "accessible-for-notify")
			    && strcmp (yc -> yc_access, "not-accessible"))
			yyerror ("value of MIN-ACCESS clause isn't a valid keyword");
		    break;

		default:
		    yyerror ("unknown compliance");
		    /* NOTREACHED */
	    }
    }
}

/*  */

/* ARGSUSED */

static void do_agent (ac, id)
register AC	ac;
char   *id;
{
    register YD	    yd;
    register YV	    yv;

    if (strcmp (ac -> ac_status, "current")
	    && strcmp (ac -> ac_status, "obsolete"))
	yyerror ("value of STATUS clause isn't a valid keyword");

    if (!didtcl)
	do_tcl (ac -> ac_name);
    printf ("%-20s %s\n", ac -> ac_name, id2str (ac -> ac_value));
    printf ("%%n0 %-16s agent-capabilities\n", ac -> ac_name);

    if (tcl) {
	char    *cp = "";

	fprintf (tcl, "\nforeach f {");
	for (yd = ac -> ac_comp; yd; yd = yd -> yd_next, cp = " ")
	    fprintf (tcl, "%s%s", cp, yd -> yd_modname);
	fprintf (tcl, "} {\n\
    if {[info exists snmptcl_loaded($f.tcl)]} {\n\
	continue\n\
    }\n\
    if {[file readable $f.tcl]} {\n\
	source $f.tcl\n\
    } else {\n\
	source $snmptcl_library/mibs/$f.tcl\n\
    }\n\
}\n\n");

	fprintf (tcl, "snmpmisc agentprop %s supports ", ac -> ac_name);
    }

    for (yd = ac -> ac_comp; yd; yd = yd -> yd_next) {
	YC	yc;

	if (tcl)
	    fprintf (tcl, "\\\n    %s {", yd -> yd_modname);
	if ((yv = yd -> yd_groups) != NULL) {
	    char   *cp = "";

	    if  (yv -> yv_code != YV_VALIST)
		yyerror ("value of INCLUDES clause is not a list of object groups");
	    for (yv = yv -> yv_idlist; yv; yv = yv -> yv_next, cp = " ") {
		if (yv -> yv_code != YV_IDEFINED
		        || !(yv -> yv_flags & YV_BOUND))
		    yyerror ("element in INCLUDES clause is not an object group");
		if (tcl)
		    fprintf (tcl, "%s%s", cp, yv -> yv_identifier);
	    }
	}
	if (tcl)
	    fprintf (tcl, "} ");

	for (yc = yd -> yd_variations; yc; yc = yc -> yc_next) {
/* might check syntax & write-syntax: yc_syntax & yc_wsyntax */

	    if (yc -> yc_access
		    && strcmp (yc -> yc_access, "read-only")
		    && strcmp (yc -> yc_access, "accessible-for-notify")
		    && strcmp (yc -> yc_access, "read-write")
		    && strcmp (yc -> yc_access, "read-create")
		    && strcmp (yc -> yc_access, "not-accessible")
		    && strcmp (yc -> yc_access, "not-implemented")
		    && strcmp (yc -> yc_access, "write-only"))
		myyerror ("VARIATION %s: value of ACCESS clause isn't a valid keyword",
			  yc -> yc_target);

	    if ((yv = yc -> yc_creation) != NULL) {
		if  (yv -> yv_code != YV_VALIST)
		    yyerror ("value of CREATION-REQUIRES clause is not a list of object groups");
		for (yv = yv -> yv_idlist; yv; yv = yv -> yv_next) {
		    if (yv -> yv_code != YV_IDEFINED
		            || !(yv -> yv_flags & YV_BOUND))
			yyerror ("element in CREATION-REQUIRES clause is not an object group");
		}
	    }

/* might check defval: yc_defval */
	}
    }

    if (tcl) {
	fprintf (tcl, "\n");

	for (yd = ac -> ac_comp; yd; yd = yd -> yd_next) {
	    char   *dp,
		   *syname;
	    YC	    yc;
	    YP	    yp;
	    struct ysub *ysub;

	    for (yc = yd -> yd_variations; yc; yc = yc -> yc_next) {
		if (!yc -> yc_syntax
		        && !yc -> yc_wsyntax
		        && !yc -> yc_access
		        && !yc -> yc_creation)
		    continue;

		fprintf (tcl,
			 "snmpmisc agentprop %s variation %s %s \\\n    {",
			 ac -> ac_name, yd -> yd_modname, yc -> yc_target);
		dp = "";

		syname = "SYNTAX", yp = yc -> yc_syntax;
again: ;
		if (yp)
		    switch (yp -> yp_code) {
			case YP_INT:
			    if ((yp -> yp_flags & YP_SUBTYPE)
				    && (ysub = yp -> yp_subtype) -> ysub_code
					    == YS_VALUERANGE) {
				YV	yv2;

				if ((yv = ysub -> ysub_lower -> ep_value)
				        && yv -> yv_code == YV_NUMBER
				        && (yv2 = ysub -> ysub_upper
						       -> ep_value)
				        && yv2 -> yv_code == YV_NUMBER
				        && yv -> yv_number
				                <= yv2 -> yv_number) {
				    fprintf (tcl,
					     "%s{%s {range %d %d}}", dp,
					     syname, yv -> yv_number,
					     yv2 -> yv_number);
				    dp = " ";
				}
			    }
			    break;

			case YP_INTLIST:
			case YP_BITS:
			    fprintf (tcl, "%s{%s {enums", dp, syname);
			    dp = " ";
			    for (yv = yp -> yp_value; yv; yv = yv -> yv_next)
				fprintf (tcl, " %d", yv -> yv_number);
			    fprintf (tcl, "}}");
			    break;

			case YP_OCT:
			    if ((yp -> yp_flags & YP_SUBTYPE)
				    && (ysub = yp -> yp_subtype) -> ysub_code
					    == YS_SIZE)
				for (ysub = ysub -> ysub_subtype;
				         ysub;
				         ysub = ysub -> ysub_next) {
				    YV	    yv2;

				    if (ysub -> ysub_code == YS_VALUERANGE
					    && (yv = ysub -> ysub_lower
							  -> ep_value)
					    && yv -> yv_code == YV_NUMBER
					    && (yv2 = ysub -> ysub_upper
							   -> ep_value)
					    && yv -> yv_number
						    <= yv2 -> yv_number) {
					fprintf (tcl,
						 "%s{%s {size %d %d}}", dp,
						 syname, yv -> yv_number,
						 yv2 -> yv_number);
					dp = " ";
					break;
				    }
				}
			    break;

			default:
			    break;
		    }
		if (strcmp (syname, "SYNTAX") == 0) {
		    syname = "WRITE-SYNTAX", yp = yc -> yc_wsyntax;
		    goto again;
		}

		if (yc -> yc_access) {
		    fprintf (tcl, "%s{ACCESS %s}", dp, yc -> yc_access);
		    dp = " ";
		}

		if ((yv = yc -> yc_creation) != NULL) {
		    char   *cp = "";

		    fprintf (tcl, "%s{CREATION-REQUIRES {", dp);
		    dp = " ";
		    for (yv = yv -> yv_idlist;
			     yv;
			     yv = yv -> yv_next, cp = " ")
			fprintf (tcl, "%s%s", cp, yv -> yv_identifier);
		    fprintf (tcl, "}}");
		}

		fprintf (tcl, "}\n");
	    }
	}
    }

}

/*    IDENTIFIER HANDLING */

static	OI	lookup_identifier (mod, id)
register char *mod,
	      *id;
{
    register SY	    sy;

    for (sy = myidentifiers; sy; sy = sy -> sy_next) {
	if (mod) {
	    if (strcmp (sy -> sy_module, mod))
		continue;
	}
	else
	    if (strcmp (sy -> sy_module, mymodule)
		    && strcmp (sy -> sy_module, "UNIV"))
		continue;

	if (strcmp (sy -> sy_name, id) == 0)
	    return sy -> sy_yi;
    }

    return NULLOI;
}

/*  */

static char *id2str (yv)
register YV	yv;
{
    register char *cp,
		  *dp;
    static char buffer[BUFSIZ];

    if (yv -> yv_code != YV_OIDLIST)
	yyerror ("need an object identifer");

    cp = buffer;
    for (yv = yv -> yv_idlist, dp = ""; yv; yv = yv -> yv_next, dp = ".") {
	(void) sprintf (cp, "%s%s", dp, val2str (yv));
	cp += strlen (cp);
    }
    *cp = '\0';

    return buffer;
}

/*    GROUP HANDLING */

static	OG	lookup_group (mod, id)
register char *mod,
	      *id;
{
    register SY	    sy;

    for (sy = mygroups; sy; sy = sy -> sy_next) {
	if (mod) {
	    if (strcmp (sy -> sy_module, mod))
		continue;
	}
	else
	    if (strcmp (sy -> sy_module, mymodule)
		    && strcmp (sy -> sy_module, "UNIV"))
		continue;

	if (strcmp (sy -> sy_name, id) == 0)
	    return sy -> sy_yg;
    }

    return NULLOG;
}

/*    OBJECT HANDLING */

static	OT	lookup_object_aux (mod, id, start)
register char *mod,
	      *id;
SY	start;
{
    register SY	    sy;

    for (sy = start; sy; sy = sy -> sy_next) {
	if (mod) {
	    if (strcmp (sy -> sy_module, mod))
		continue;
	}
	else
	    if (strcmp (sy -> sy_module, mymodule)
		    && strcmp (sy -> sy_module, "UNIV"))
		continue;

	if (strcmp (sy -> sy_name, id) == 0)
	    return sy -> sy_yo;
    }

    return NULLOT;
}

/*  */

static	void 
check_objects (yv, clause, typesOK, notify)
register YV    yv;
char   *clause;
int	typesOK,
	notify;
{
    if (yv -> yv_code != YV_VALIST) {
	myyerror ("value of %s clause is not a list of object types", clause);
	return;
    }

    for (yv = yv -> yv_idlist; yv; yv = yv -> yv_next)
	check_object (yv, clause, typesOK, notify);
}

/*  */

static
void check_object (yv, clause, typesOK, notify)
register YV    yv;
char   *clause;
int	typesOK,
	notify;
{
    switch (yv -> yv_code) {
        case YV_IDEFINED:
	    if (!typesOK && !(yv -> yv_flags & YV_BOUND))
		goto not_a_type;
	    if (lookup_object_aux (yv -> yv_module, yv -> yv_identifier,
				   notify ? mytraps : myobjects) == NULL
		    && !importedP (yv -> yv_identifier))
		myyerror ("element in %s clause is undefined: %s", clause,
			  yv -> yv_identifier);
	    break;

	case YV_NUMBER:
	case YV_STRING:
	case YV_OIDLIST:
	    if (typesOK)
		break;
	    /* else fall... */
	default:
not_a_type: ;
	    myyerror ("element in %s clause is %s", clause,
		      typesOK ? "neither an object type nor a data type"
			      : "not an object type");
	    break;
    }
}

/*  */

static char *check_syntax (name, ypp)
char   *name;
YP     *ypp;
{
    char   *id;
    register YP	    yp,
		    yz;
    register YV	    yv;

    yp = *ypp;
again: ;
    switch (yp -> yp_code) {
	case YP_INT:
	    if (snmpV2 && !(yp -> yp_flags & YP_SUBTYPE))
		yyerror ("INTEGER requires either upper-lower bounds or named-number enumerations");
	    id = "INTEGER";
	    break;

	case YP_INTLIST:
	    for (yv = yp -> yp_value; yv; yv = yv -> yv_next)
		if (yv -> yv_code != YV_NUMBER)
		    yyerror ("value of enumerated INTEGER is not a number");
		else
		    if (!snmpV2 && yv -> yv_number == 0)
			yyerror ("value of enumerated INTEGER is zero");
	    for (yv = yp -> yp_value; yv; yv = yv -> yv_next) {
		register YV	v;

		for (v = yv -> yv_next; v; v = v -> yv_next)
		    if (yv -> yv_number == v -> yv_number)
			myyerror ("duplicate values in enumerated INTEGER: %d",
				  yv -> yv_number);
		if (yv -> yv_flags & YV_NAMED)
		    for (v = yv -> yv_next; v; v = v -> yv_next)
			if ((v -> yv_flags & YV_NAMED)
			        && strcmp (yv -> yv_named, v -> yv_named) == 0)
			    myyerror ("duplicate tags in enumerated INTEGER: %s",
				      yv -> yv_named);
	    }
	    id = "INTEGER";
	    break;

       case YP_OCT:
	    if (snmpV2 && !(yp -> yp_flags & YP_SUBTYPE))
	      warning ("OCTET STRING should have SIZE constraint");
	    id = "OctetString";
	    break;

       case YP_OID:
	    id = "ObjectID";
	    break;

       case YP_BITS:
	    id = "Bits";
	    break;

       case YP_NULL:
	    if (snmpV2)
		goto losing_type;
	    id = "NULL";
	    break;

	case YP_SEQTYPE:
	    if ((yz = yp -> yp_type) -> yp_code != YP_IDEFINED
		    || (yz = lookup_type (yz -> yp_module,
					  yz -> yp_identifier)) == NULL
		    || yz -> yp_code != YP_SEQLIST)
		yyerror ("value of SYNTAX clause isn't SEQUENCE OF Type, where Type::= SEQUENCE {...}");
	    /* and fall... */
	case YP_SEQLIST:
	    id = "Aggregate";
	    break;

	default:
losing_type: ;
	    id = "Invalid";
	    yyerror ("invalid value of SYNTAX clause");
	    break;

	case YP_IDEFINED:
	    if ((yz = lookup_type (yp -> yp_module, id = yp -> yp_identifier)) != NULL){
		yp = yz;
		goto again;
	    }
	    if (snmpV2) {
		if (strcmp (id, "Counter") == 0)
		    warning ("counter object type %s should have Counter32 syntax",
			     name);
		else
		    if (strcmp (id, "Gauge") == 0)
			warning ("gauge object type %s should have Unsigned32 syntax",
				 name);
	    }
	    else
		if (strcmp (id, "Counter") == 0
		        && name[strlen (name) - 1] != 's')
	    break;
    }

    *ypp = yp;

    return id;
}

/*    TYPE HANDLING */

static YP  lookup_type (mod, id)
register char *mod,
	      *id;
{
    register SY	    sy;

    for (sy = mytypes; sy; sy = sy -> sy_next) {
	if (mod) {
	    if (strcmp (sy -> sy_module, mod))
		continue;
	}
	else
	    if (strcmp (sy -> sy_module, mymodule)
		    && strcmp (sy -> sy_module, "UNIV"))
		continue;

	if (strcmp (sy -> sy_name, id) == 0)
	    return sy -> sy_yp;
    }

    return NULLYP;
}

/*    VALUE HANDLING */

static char *val2str (yv)
register YV	yv;
{
    static char buffer[BUFSIZ];

    switch (yv -> yv_code) {
	case YV_BOOL:
	    yyerror ("need a sub-identifier, not a boolean");

	case YV_NUMBER:
	    (void) sprintf (buffer, "%d", yv -> yv_number);
	    return buffer;

	case YV_STRING:
	    yyerror ("need a sub-identifier, not a string");

	case YV_IDEFINED:
	    return yv -> yv_identifier;

	case YV_IDLIST:
	    yyerror ("haven't written symbol table for values yet");

	case YV_VALIST:
	    yyerror ("need a sub-identifier, not a list of values");

	case YV_OIDLIST:
	    yyerror ("need a sub-identifier, not OBJECT IDENTIFIER");

	case YV_NULL:
	    yyerror ("need a sub-identifier, not NULL");

	case YV_REAL:
	    yyerror ("need a sub-identifier, not REAL");

	default:
	    myyerror ("unknown value: %d", yv -> yv_code);
    }
/* NOTREACHED */
    return NULL;  /* this is just for compilers that want a return statement */
}

/*    V2 -> V1 */

/* ARGSUSED */

static char *classes[] = {
    "UNIVERSAL ",
    "APPLICATION ",
    "",
    "PRIVATE "
};


static void print1_type (yp, level, id)
register YP yp;
int	level;
char   *id;
{
    int	    incr;
    register YP	    y;
    register YV	    yv;
    register YT	    yt;

    y = yp -> yp_type;

    if (yp -> yp_flags & YP_ID) {
	printf ("%s", yp -> yp_id);
	if (!(yp -> yp_flags & YP_TAG))
	   printf ("\n%*s", ++level * 4, "");
    }

    if (yp -> yp_flags & YP_TAG) {
	if (!(yt = yp -> yp_tag))
	    myyerror ("lost tag");
	printf ("[%s%d]\n", classes[yt -> yt_class], val2int (yt -> yt_value));
	level++;
	printf ("%*s", level * 4, "");
	if (yp -> yp_flags & YP_IMPLICIT)
	   printf ("IMPLICIT ");
    }
    if (yp -> yp_flags & YP_BOUND)
	printf ("%s < ", yp -> yp_bound);
    if (yp -> yp_flags & YP_COMPONENTS)
	printf ("COMPONENTS OF ");
    if (yp -> yp_flags & YP_ENCRYPTED)
	printf ("ENCRYPTED ");

    switch (yp -> yp_code) {
	case YP_BOOL:
	    printf ("BOOLEAN");
	    break;

	case YP_INT:
	    printf ("INTEGER");
	    break;

	case YP_INTLIST: 
	    printf ("INTEGER");
	    goto do_list;

	case YP_ENUMLIST:
	    printf ("ENUMERATED");
	    goto do_list;

	case YP_BITS:
	    printf ("BITS");
	    goto do_list;

	case YP_BITLIST: 
	    printf ("BITSTRING");
	    /* and fall... */
do_list:;
	    printf (" {\n");
	    level++;
	    for (yv = yp -> yp_value; yv; yv = yv -> yv_next) {
		if (!(yv -> yv_flags & YV_NAMED))
		    myyerror ("lost named number");
		printf ("%*s%s(%d)", level * 4 - 3, "", yv -> yv_named,
			val2int (yv));
		printf ("%s\n", yv -> yv_next ? "," : "");
	    }
	    level--;
	    printf ("%*s}", level * 4 - 1, "");
	    break;

	case YP_BIT:
	    printf ("BIT STRING");
	    break;

	case YP_OCT:
	    printf ("OCTET STRING");
	    break;

	case YP_NULL:
	    printf ("NULL");
	    break;

	case YP_REAL:
	    printf ("REAL");
	    break;

	case YP_SEQ:
	    printf ("SEQUENCE");
	    break;

	case YP_SEQTYPE:
	    printf ("SEQUENCE OF ");
	    if (!yp -> yp_type)
		myyerror ("lost sequence type");
	    print1_type (yp -> yp_type, level, "element");
	    break;

	case YP_SEQLIST:
	    printf ("SEQUENCE");
	    printf (yp -> yp_type ? " {\n" : " {");
	    level += (incr = (y -> yp_flags & (YP_ID | YP_TAG)) ? 1 : 2);
	    for (y = yp -> yp_type; y; y = y -> yp_next) {
		printf ("%*s", level * 4, "");
		print1_type (y, level, "element");
		printf ("%s\n", y -> yp_next ? ",\n" : "");
	    }
	    level -= incr;
	    printf (yp -> yp_type ? "%*s}" : "}", (level + 1) * 4, "");
	    break;

	case YP_SET:
	    printf ("SET");
	    break;

	case YP_SETTYPE:
	    printf ("SET OF ");
	    if (!yp -> yp_type)
		myyerror ("lost set type");
	    print1_type (yp -> yp_type, level, "member");
	    break;

	case YP_SETLIST: 
	    printf ("SET");
	    printf (yp -> yp_type ? " {\n" : " {");
	    level += (incr = (y -> yp_flags & (YP_ID | YP_TAG)) ? 1 : 2);
	    for (y = yp -> yp_type; y; y = y -> yp_next) {
		printf ("%*s", level * 4, "");
		print1_type (y, level, "member");
		printf ("%s\n", y -> yp_next ? ",\n" : "");
	    }
	    level -= incr;
	    printf (yp -> yp_type ? "%*s}" : "}", level * 4, "");
	    break;

	case YP_CHOICE:
	    printf ("CHOICE {\n");
	    level += (incr = (y -> yp_flags & (YP_ID | YP_TAG)) ? 1 : 2);
	    for (y = yp -> yp_type; y; y = y -> yp_next) {
		printf ("%*s", level * 4, "");
		print1_type (y, level, "choice");
		printf ("%s\n", y -> yp_next ? ",\n" : "");
	    }
	    level -= incr;
	    printf ("%*s}", level * 4, "");
	    break;

	case YP_ANY:
	    printf ("ANY");
	    break;

	case YP_OID:
	    printf ("OBJECT IDENTIFIER");
	    break;

	case YP_IDEFINED:
	    if (htmlflag) {
		id2html (yp -> yp_module, yp -> yp_identifier);
		break;
	    }
	    if (strcmp (yp -> yp_identifier, "Integer32") == 0)
		printf ("INTEGER (-2147483648..2147483647)");
	    else
	    if (strcmp (yp -> yp_identifier, "Counter32") == 0)
		printf ("Counter");
	    else
	    if (strcmp (yp -> yp_identifier, "Unsigned32") == 0)
		printf ("Gauge");
	    else
	    if (strcmp (yp -> yp_identifier, "Counter64") == 0)
		myyerror ("%s not allowed in SNMPv1", yp -> yp_identifier);
	    else {
/*
		if (yp -> yp_module && strcmp (yp -> yp_module, mymodule))
		    printf ("%s.", yp -> yp_module);
 */
		printf ("%s", yp -> yp_identifier);
	    }
	    break;

	default:
	    myyerror ("unknown type: %d", yp -> yp_code);
    }

    if (yp -> yp_flags & YP_OPTIONAL)
	printf ("\n%*sOPTIONAL", level * 4, "");
    else
	if (yp -> yp_flags & YP_DEFAULT) {
	    if (!yp -> yp_default)
		myyerror ("lost default");
	    printf ("\n%*sDEFAULT ", level * 4, "");
	    val2prf (yp -> yp_default, level + 2);
	}
}

/*  */

static int  val2int (yv)
register YV	yv;
{
    switch (yv -> yv_code) {
	case YV_BOOL:
	case YV_NUMBER:
	    return yv -> yv_number;

	case YV_STRING:
	    yyerror ("need an integer, not a string");

	case YV_IDEFINED:
	case YV_IDLIST:
	    yyerror ("haven't written symbol table for values yet");

	case YV_VALIST:
	    yyerror ("need an integer, not a list of values");

	case YV_OIDLIST:
	    yyerror ("need an integer, not OBJECT IDENTIFIER");

	case YV_NULL:
	    yyerror ("need an integer, not NULL");

	case YV_REAL:
	    yyerror ("need and integer, not REAL");

	default:
	    myyerror ("unknown value: %d", yv -> yv_code);
    }
/* NOTREACHED */
    return -1;  /* this is just for compilers that want a return statement */
}

/*  */

static void
val2prf1 (mod, yv, level)
char   *mod;
register YV	yv;
int	level;
{
    register YV    y;
    
    switch (yv -> yv_code) {
	case YV_IDEFINED:
	    if (!yv -> yv_module)
		yv -> yv_module = mod;
	    break;

	case YV_IDLIST:
	case YV_VALIST:
	    for (y = yv -> yv_idlist; y; y = y -> yv_next) {
		if (!y -> yv_module)
		    y -> yv_module = mod;
	    }
	    break;
    }
    
    val2prf (yv, level);
}

static void 
val2prf (yv, level)
register YV	yv;
int	level;
{
    int	    code;
    register YV    y;

    if (yv -> yv_flags & YV_ID)
	printf ("%s ", yv -> yv_id);

    if (yv -> yv_flags & YV_TYPE)	/* will this REALLY work??? */
	print1_type (yv -> yv_type, level, NULLCP);

    switch (code = yv -> yv_code) {
	case YV_BOOL:
	    printf (yv -> yv_number ? "TRUE" : "FALSE");
	    break;

	case YV_NUMBER:
	    if (yv -> yv_named)
		printf ("%s", yv -> yv_named);
	    else
		printf ("%d", yv -> yv_number);
	    break;

	case YV_STRING:
	    printf ("\"%s\"", yv -> yv_string);
	    break;

	case YV_IDEFINED:
	    if (htmlflag)
		id2html (yv -> yv_module, yv -> yv_identifier);
	    else {
		if (yv -> yv_module)
		    printf ("%s.", yv -> yv_module);
		printf ("%s", yv -> yv_identifier);
	    }
	    break;

	case YV_IDLIST:
	case YV_VALIST:
	    if (!yv -> yv_idlist
		    || !yv -> yv_idlist -> yv_next
		    || !yv -> yv_idlist -> yv_next -> yv_next)
		goto long_list;
	    printf ("{");
	    level++;
	    for (y = yv -> yv_idlist; y; y = y -> yv_next) {
		printf ("%*s", level * 4 - 3, "");
		val2prf (y, level);
		printf ("%s", y -> yv_next ? "," : "");
	    }
	    level--;
	    printf ("%*s}", level * 4 - 1, "");
	    break;

	case YV_OIDLIST:
long_list: ;
	    printf ("{");
	    for (y = yv -> yv_idlist; y; y = y -> yv_next) {
		printf (" ");
		val2prf (y, level + 1);
		printf (code != YV_OIDLIST && y -> yv_next ? ", " : " ");
	    }
	    printf ("}");
	    break;

	case YV_NULL:
	    printf ("NULL");
	    break;

	case YV_REAL:
	    yyerror ("don't grok REAL...");
	    break;

	default:
	    myyerror ("unknown value: %d", yv -> yv_code);
	/* NOTREACHED */
    }
}

/*  */

static void
id2html (mod, id)
char   *mod,
       *id;
{
    int	    definedP = 1;

    if (!mod) {
	SY	sy;

	for (sy = mylinks; sy; sy = sy -> sy_link)
	    if (strcmp (sy -> sy_name, id) == 0)
		break;
	if (!sy && !(mod = importedP (id))) {
	    if ((htmlflag < -1)
		    || !strcmp (id, "iso")
		    || !strcmp (id, "ccitt")
		    || !strcmp (id, "joint-iso-ccitt"))
	    definedP = 0;
	}
    }

    if (definedP) {
	printf ("<A HREF=\"");
	if (mod)
	    printf ("%s.html", mod);
	printf ("#%s\">", id);
    }
/*
    if (mod)
	printf ("%s.", mod);
 */
    printf ("%s", id);
    if (definedP)
	printf ("</A>");
}

/*  */

static struct pair {
    char   *p_module;
    char   *p_symbol;
    int	    p_referenced;
    int	    p_discard;
} pairs[] = {
  {  "RFC1155-SMI", "internet",		0, 0, },
  {  "RFC1155-SMI", "directory",	0, 0, },
  {  "RFC1155-SMI", "mgmt",		0, 0, },
  {  "RFC1155-SMI", "experimental",	0, 0, },
  {  "RFC1155-SMI", "private",		0, 0, },
  {  "RFC1155-SMI", "enterprises",	0, 0, },
  {  "RFC1155-SMI", "ObjectName",	0, 0, },
  {  "RFC1155-SMI", "ObjectSyntax",	0, 0, },
  {  "RFC1155-SMI", "SimpleSyntax",	0, 0, },
  {  "RFC1155-SMI", "ApplicationSyntax",0, 0, },
  {  "RFC1155-SMI", "NetworkAddress",	0, 0, },
  {  "RFC1155-SMI", "IpAddress",	0, 0, },
  {  "RFC1155-SMI", "Counter",		0, 0, },
  {  "RFC1155-SMI", "Gauge",		0, 0, },
  {  "RFC1155-SMI", "TimeTicks",	0, 0, },
  {  "RFC1155-SMI", "Opaque",		0, 0, },
  {  "RFC-1212",    "OBJECT-TYPE",	0, 0, },
  {  "RFC1213-MIB", "DisplayString",	0, 0, },
  {  "RFC1213-MIB", "PhysAddress",	0, 0, },
  {  "RFC-1215",    "TRAP-TYPE",	0, 0, },
  {  "RFC1286-MIB", "MacAddress",	0, 0, },
  {  "RFC1253-MIB", "TruthValue",	0, 0, },
  {  "RFC1316-MIB", "AutonomousType",	0, 0, },
  {  "RFC1316-MIB", "InstancePointer",	0, 0, },

  {  "SNMPv2-SMI",  "MODULE-IDENTITY",	0, 1, },
  {  "SNMPv2-SMI",  "OBJECT-IDENTITY",	0, 1, },
  {  "SNMPv2-TC",   "TEXTUAL-CONVENTION",0, 1, },
  {  "SNMPv2-CONF", "OBJECT-GROUP",	0, 1, },
  {  "SNMPv2-CONF", "MODULE-COMPLIANCE", 0, 1, },
  {  "SNMPv2-CONF", "AGENT-CAPABILITIES",0, 1, },

  {  NULLCP, NULLCP, 0, 0 },
};


int	check_expimp (name)
char   *name;
{
    register struct pair *p;

    for (p = pairs; p -> p_module; p++)
	if (strcmp (p -> p_symbol, name) == 0)
	    return (p -> p_referenced = 1);
    if (strcmp ("NOTIFICATION-TYPE", name) == 0)
	return check_expimp ("TRAP-TYPE");
    if (strcmp ("Integer32", name) == 0)
	return 1;
    if (strcmp ("Counter32", name) == 0)
	return check_expimp ("Counter");
    if (strcmp ("Unsigned32", name) == 0)
	return check_expimp ("Gauge");

    return 0;
}

/*  */

static  void printh_quoted (clause, s)
char   *clause,
       *s;
{
    int   i,
	  indent;
    char *p,
    	 *q,
	 *r;

    fputs (clause, stdout);
    if (!*s)
	printf ("\"\"");
    printf ("\n");

    if (!*s)
	return;

    indent = 0;

    for (p = s; p; p = *q ? q + 1 : NULLCP) {
	if (!(q = index (p, '\n')))
	    q = p + strlen (p);
	for (r = p; r < q; r++)
	    if (*r != ' ')
		break;
	if ((i = r - p) > 0 && ((indent == 0) || (i < indent)))
	    indent = i;
    }

    if (*s == ' ') {
	printf ("     ");
	if (indent > 0)
	    printf ("%*.*s", indent, indent, "");
    }

    for (p = s, r = "&#34;"; p; p = *q ? q + 1 : NULLCP, r = " ") {
	char buffer[BUFSIZ];

	if (!(q = index (p, '\n')))
	    q = p + strlen (p);
	else
	    if (p == q) {
		printf ("%s\n", *r != ' ' ? r : "");
		continue;
	    }
	if (*s != ' ')
	    printf ("    ");
	printf ("    %s", r);
	if (*p == ' ')
	    p += indent;

	(void) strncpy (buffer, p, i = q - p);
	buffer[i] = '\0';
	printh_line (buffer);

	printf ("%s\n", *q ? "" : "&#34;");
    }
}

/*  */

static void printh_line (s)
char   *s;
{
    char  c,
	 *p,
	 *q,
	 *r;

    for (p = s; p; p = *q ? q + 1 : NULLCP) {
	char   *x,
	       *y,
	       *z;

	if (!(q = index (p, ' ')))
	    q = p + strlen (p), y = "";
	else
	    if (p == q) {
		printf (" ");
		continue;
	    }
	    else
		y = " ";

	x = p;

	if (!strncmp (p, "RFC", 3)) {
	    p += 3;
	    if (*p == ' ')
		p++;
	    for (r = p; isdigit ((unsigned char)*r); r++)
		continue;
	    if (r != p) {
		q = r - 1;
		printf ("<A HREF=\"rfc%*.*s.txt\">%*.*s</A>",
			(int)(r - p), (int)(r - p), p, (int)(r - x), (int)(r - x), x);
		continue;
	    }

	    p = x;
	}
	if ((z = index (p, '@')) && (z != p) && (z < q)) {
	    while (x < q)
		if (((c = *x++) != '@') && (c != '.') && !isalnum ((unsigned char)c))
		    break;
	    if (x == q) {
		if ((c = *--q) != '.')
		    q++;
		printf ("<A HREF=\"mailto:%*.*s\">%*.*s</A>",
			(int)(q - p), (int)(q - p), p, (int)(q - p), (int)(q - p), p);
		if (c == '.') {
		    printf (".");
		    q++;
		}
		printf ("%s", y);
		continue;
	    }
	}
	
	while (p < q)
	    switch (c = *p++) {
		case '"': printf ("&#34;"); break;
		case '&': printf ("&#38;"); break;
		case '<': printf ("&#60;"); break;
		case '>': printf ("&#62;"); break;
		default : putchar (c);
	    }
	fputs (y, stdout);
    }
}

/*  */

static void do_tcl (id)

char   *id;
{
    didtcl = 1;

    fprintf (tcl, "\n\
if {[catch { snmpinfo mibprop %s value }]} {\n\
    if {[file readable %s.def]} {\n\
	snmpmisc mibload %s.def\n\
    } else {\n\
	snmpmisc mibload %s%s.def\n\
    }\n\
}\n", id, mymodule, mymodule, DEFSDIR, mymodule);
}


void tcltableref (module)
char   *module;
{
    if (!tcl)
	return;

    fprintf (tcl, "\n\
if {![info exists snmptcl_loaded(%s.tcl)]} {\n\
    if {[file readable %s.tcl]} {\n\
	source %s.tcl\n\
    } else {\n\
	catch { source $snmptcl_library/mibs/%s.tcl }\n\
    }\n\
}\n", module, module, module, module);
}

/*    DEBUG */

static void print_ym (ym, level)
register YM	ym;
register int	level;
{
    if (ym == NULLYM)
	return;

    fprintf (stderr, "%*sname=%s\n", level * 4, "", ym -> ym_name);
    fprintf (stderr, "%*svalue\n", level * 4, "");
    print_value (ym -> ym_value, level + 1);

    fprintf (stderr, "%*supdate=%s org=%s\n", level * 4, "",
	     ym -> ym_update, ym -> ym_org);
    fprintf (stderr, "%*scontact=%s descr=%s\n", level * 4, "",
	     ym -> ym_contact, ym -> ym_descr);

    if (ym -> ym_revs) {
	fprintf (stderr, "%*srevisions\n", level * 4, "");
	print_yr (ym -> ym_revs, level + 1);
    }
}

/*  */

static void print_yi (yi, level)
register OI	yi;
register int	level;
{
    if (yi == NULLOI)
	return;

    fprintf (stderr, "%*sname=%s\n", level * 4, "", yi -> yi_name);
    fprintf (stderr, "%*svalue\n", level * 4, "");
    print_value (yi -> yi_value, level + 1);

    if (yi -> yi_status)
	fprintf (stderr, "%*sstatus=%s descr=%s refer=%s\n", level * 4, "",
		 yi -> yi_status, yi -> yi_descr, yi -> yi_refer);
}

/*  */

static void print_tc (tc, level)
register TC	tc;
register int	level;
{
    if (tc == NULLTC)
	return;

    fprintf (stderr, "%*sname=%s\n", level * 4, "", tc -> tc_name);
    fprintf (stderr, "%*svalue\n", level * 4, "");
    print_type (tc -> tc_value, level + 1);

    fprintf (stderr, "%*sdisplay=%s\n", level * 4, "",
	     tc -> tc_display ? tc -> tc_display : "");

    fprintf (stderr, "%*sstatus=%s descr=%s refer=%s\n", level * 4, "",
	     tc -> tc_status, tc -> tc_descr,
	     tc -> tc_refer ? tc -> tc_refer : "");
}

/*  */

static void print_yo (yo, level)
register OT	yo;
register int	level;
{
    if (yo == NULLOT)
	return;

    fprintf (stderr, "%*sname=%s\n", level * 4, "", yo -> yo_name);
    fprintf (stderr, "%*svalue\n", level * 4, "");
    print_value (yo -> yo_value, level + 1);

    fprintf (stderr, "%*ssyntax\n", level * 4, "");
    print_type (yo -> yo_syntax, level + 1);
    fprintf (stderr, "%*sunits=%s access=%s\n",
	     level * 4, "", yo -> yo_units ? yo -> yo_units : "",
	     yo -> yo_access);

    fprintf (stderr, "%*sstatus=%s descr=%s refer=%s\n", level * 4, "",
	     yo -> yo_status, yo -> yo_descr ? yo -> yo_descr : "",
	     yo -> yo_refer ? yo -> yo_refer : "");

    if (yo -> yo_index) {
	fprintf (stderr, "%*sindex\n", level * 4, "");
	print_value (yo -> yo_index, level + 1);
    }

    if (yo -> yo_defval) {
	fprintf (stderr, "%*sdefval\n", level * 4, "");
	print_value (yo -> yo_defval, level + 1);
    }
}

/*  */

static void print_yt (yt, level)
register TT	yt;
register int	level;
{
    if (yt == NULLTT)
	return;

    fprintf (stderr, "%*sname=%s\n", level * 4, "", yt -> yt_name);

    fprintf (stderr, "%*sstatus=%s descr=%s refer=%s\n", level * 4, "",
	     yt -> yt_status, yt -> yt_descr ? yt -> yt_descr : "",
	     yt -> yt_refer ? yt -> yt_refer : "");

    if (yt -> yt_enterprise) {
	fprintf (stderr, "%*senterprise\n", level * 4, "");
	print_value (yt -> yt_enterprise, level + 1);
	fprintf (stderr, "%*s    number=%d\n", level * 4, "", yt -> yt_number);
    }

    if (yt -> yt_vars) {
	fprintf (stderr, "%*svariables\n", level * 4, "");
	print_value (yt -> yt_vars, level + 1);
    }
}

/*  */

static void print_yg (yg, level)
register OG	yg;
register int	level;
{
    if (yg == NULLOG)
	return;

    fprintf (stderr, "%*sname=%s (%s)\n", level * 4, "", yg -> yg_name,
	     yg -> yg_notify ? "notification" : "objects");
    fprintf (stderr, "%*svalue\n", level * 4, "");
    print_value (yg -> yg_value, level + 1);

    fprintf (stderr, "%*sstatus=%s descr=%s refer=%s\n", level * 4, "",
	     yg -> yg_status, yg -> yg_descr,
	     yg -> yg_refer ? yg -> yg_refer : "");

    if (yg -> yg_objects) {
	fprintf  (stderr, "%*sobjects\n", level * 4, "");
	print_value (yg -> yg_objects, level + 1);
    }
}

/*  */

static void print_mc (mc, level)
register MC	mc;
register int	level;
{
    if (mc == NULLMC)
	return;

    fprintf (stderr, "%*sname=%s\n", level * 4, "", mc -> mc_name);
    fprintf (stderr, "%*svalue\n", level * 4, "");
    print_value (mc -> mc_value, level + 1);

    fprintf (stderr, "%*sstatus=%s descr=%s refer=%s\n", level * 4, "",
	     mc -> mc_status, mc -> mc_descr,
	     mc -> mc_refer ? mc -> mc_refer : "");

    if (mc -> mc_comp) {
	fprintf (stderr, "%*smodules\n", level * 4, "");
	print_yd  (mc -> mc_comp, level + 1);
    }
}

/*  */

static void print_ac (ac, level)
register AC	ac;
register int	level;
{
    if (ac == NULLAC)
	return;

    fprintf (stderr, "%*sname=%s\n", level * 4, "", ac -> ac_name);
    fprintf (stderr, "%*svalue\n", level * 4, "");
    print_value (ac -> ac_value, level + 1);

    fprintf (stderr, "%*srelease=%s\n", level * 4, "", ac -> ac_release);

    fprintf (stderr, "%*sstatus=%s descr=%s refer=%s\n", level * 4, "",
	     ac -> ac_status, ac -> ac_descr,
	     ac -> ac_refer ? ac -> ac_refer : "");

    if (ac -> ac_comp) {
	fprintf (stderr, "%*smodules\n", level * 4, "");
	print_yd  (ac -> ac_comp, level + 1);
    }
}

/*  */

static void print_yr (yr, level)
register YR	yr;
register int	level;
{
    if (yr == NULLYR)
	return;

    fprintf (stderr, "%*stime=%s descr=%s\n", level * 4, "", yr -> yr_utctime,
	     yr -> yr_descr);

    if (yr -> yr_next) {
	fprintf (stderr, "%*s----\n", level * 4, "");
	print_yr (yr -> yr_next, level);
    }
}

/*  */

static void print_yd (yd, level)
register YD	yd;
register int	level;
{
    if (yd == NULLYD)
	return;

    if (yd -> yd_modname)
	fprintf (stderr, "%*sname=%s", level * 4, "", yd -> yd_modname);
    if (yd -> yd_modid)
	fprintf  (stderr, " (%s)", oidprint (yd -> yd_modid));
    fprintf (stderr, "\n");

    if (yd -> yd_groups) {
	fprintf (stderr, "%*sincludes\n", level * 4, "");
	print_value (yd -> yd_groups, level + 1);
    }
    if  (yd -> yd_variations) {
	fprintf (stderr, "%*svariations\n", level * 4, "");
	print_yc  (yd -> yd_variations, level + 1);
    }

    if (yd -> yd_next) {
	fprintf (stderr, "%*s----\n", level * 4, "");
	print_yd (yd -> yd_next, level);
    }
}

/*  */

static void print_yc (yc, level)
register YC	yc;
register int	level;
{
    if (yc == NULLYC)
	return;

    fprintf (stderr, "%*sname=%s type=%d\n", level * 4, "", yc -> yc_target,
	     yc -> yc_type);

    fprintf (stderr, "%*saccess=%s\n", level * 4, "",
	     yc -> yc_access ? yc -> yc_access : "");
    if  (yc -> yc_syntax) {
	fprintf (stderr, "%*ssyntax\n", level * 4, "");
	print_type  (yc -> yc_syntax, level + 1);
    }
    if  (yc -> yc_wsyntax) {
	fprintf (stderr, "%*swsyntax\n", level * 4, "");
	print_type  (yc -> yc_wsyntax, level + 1);
    }
    if  (yc -> yc_creation) {
	fprintf (stderr, "%*screation\n", level * 4, "");
	print_value  (yc -> yc_creation, level + 1);
    }
    if  (yc -> yc_defval) {
	fprintf (stderr, "%*sdefval\n", level * 4, "");
	print_value  (yc -> yc_defval, level + 1);
    }

    if (yc -> yc_next) {
	fprintf (stderr, "%*s----\n", level * 4, "");
	print_yc (yc -> yc_next, level);
    }
}

/*  */

static void print_type (yp, level)
register YP	yp;
register int	level;
{
    register YP	    y;
    register YV	    yv;

    if (yp == NULLYP)
	return;

    fprintf (stderr, "%*scode=0x%x flags=%s\n", level * 4, "",
	    yp -> yp_code, sprintb ((int) yp -> yp_flags, YPBITS));
    fprintf (stderr,
	    "%*sintexp=\"%s\" strexp=\"%s\" prfexp=%c declexp=\"%s\" varexp=\"%s\"\n",
	    level * 4, "", yp -> yp_intexp, yp -> yp_strexp, yp -> yp_prfexp,
	    yp -> yp_declexp, yp -> yp_varexp);
    fprintf(stderr,
	    "%*sstructname=\"%s\" ptrname=\"%s\"\n", level * 4, "",
	    yp -> yp_structname, yp -> yp_ptrname);

    if (yp -> yp_action0)
	fprintf (stderr, "%*saction0 at line %d=\"%s\"\n", level * 4, "",
		yp -> yp_act0_lineno, yp -> yp_action0);
    if (yp -> yp_action1)
	fprintf (stderr, "%*saction1 at line %d=\"%s\"\n", level * 4, "",
		yp -> yp_act1_lineno, yp -> yp_action1);
    if (yp -> yp_action2)
	fprintf (stderr, "%*saction2 at line %d=\"%s\"\n", level * 4, "",
		yp -> yp_act2_lineno, yp -> yp_action2);
    if (yp -> yp_action3)
	fprintf (stderr, "%*saction3 at line %d=\"%s\"\n", level * 4, "",
		yp -> yp_act3_lineno, yp -> yp_action3);

    if (yp -> yp_flags & YP_TAG) {
        fprintf (stderr, "%*stag class=0x%lx value=0x%lx\n", level * 4, "",
                (unsigned long)yp -> yp_tag -> yt_class,
                (unsigned long)yp -> yp_tag -> yt_value);
	print_value (yp -> yp_tag -> yt_value, level + 1);
    }

    if (yp -> yp_flags & YP_DEFAULT) {
	fprintf (stderr, "%*sdefault=0x%lx\n", level * 4, "", (unsigned long) yp -> yp_default);
	print_value (yp -> yp_default, level + 1);
    }

    if (yp -> yp_flags & YP_ID)
	fprintf (stderr, "%*sid=\"%s\"\n", level * 4, "", yp -> yp_id);

    if (yp -> yp_flags & YP_BOUND)
	fprintf (stderr, "%*sbound=\"%s\"\n", level * 4, "", yp -> yp_bound);

    if (yp -> yp_offset)
	fprintf (stderr, "%*soffset=\"%s\"\n", level * 4, "", yp -> yp_offset);

    switch (yp -> yp_code) {
	case YP_INTLIST:
	case YP_BITLIST:
	case YP_BITS:
            fprintf (stderr, "%*svalue=0x%lx\n", level * 4, "", (unsigned long)yp -> yp_value);
	    for (yv = yp -> yp_value; yv; yv = yv -> yv_next) {
		print_value (yv, level + 1);
		fprintf (stderr, "%*s----\n", (level + 1) * 4, "");
	    }
	    break;

	case YP_SEQTYPE:
	case YP_SEQLIST:
	case YP_SETTYPE:
	case YP_SETLIST:
	case YP_CHOICE:
            fprintf (stderr, "%*stype=0x%lx\n", level * 4, "", (unsigned long)yp -> yp_type);
	    for (y = yp -> yp_type; y; y = y -> yp_next) {
		print_type (y, level + 1);
		fprintf (stderr, "%*s----\n", (level + 1) * 4, "");
	    }
	    break;

	case YP_IDEFINED:
	    fprintf (stderr, "%*smodule=\"%s\" identifier=\"%s\"\n",
		    level * 4, "", yp -> yp_module ? yp -> yp_module : "",
		    yp -> yp_identifier);
	    break;

	default:
	    break;
    }
}

/*  */

static	void print_value (yv, level)
register YV	yv;
register int	level;
{
    register YV	    y;

    if (yv == NULLYV)
	return;

    fprintf (stderr, "%*scode=0x%x flags=%s\n", level * 4, "",
	    yv -> yv_code, sprintb (yv -> yv_flags, YVBITS));

    if (yv -> yv_action)
	fprintf (stderr, "%*saction at line %d=\"%s\"\n", level * 4, "",
		yv -> yv_act_lineno, yv -> yv_action);

    if (yv -> yv_flags & YV_ID)
	fprintf (stderr, "%*sid=\"%s\"\n", level * 4, "", yv -> yv_id);

    if (yv -> yv_flags & YV_NAMED)
	fprintf (stderr, "%*snamed=\"%s\"\n", level * 4, "", yv -> yv_named);

    if (yv -> yv_flags & YV_TYPE) {
        fprintf (stderr, "%*stype=0x%lx\n", level * 4, "", (unsigned long)yv -> yv_type);
	print_type (yv -> yv_type, level + 1);
    }

    switch (yv -> yv_code) {
	case YV_NUMBER:
	case YV_BOOL:
	    fprintf (stderr, "%*snumber=0x%x\n", level * 4, "",
		    yv -> yv_number);
	    break;

	case YV_STRING:
	    fprintf (stderr, "%*sstring=\"%s\"\n", level * 4, "",
		    yv -> yv_string);
	    break;

	case YV_IDEFINED:
	    if (yv -> yv_flags & YV_BOUND)
		fprintf (stderr, "%*smodule=\"%s\" identifier=\"%s\"\n",
			level * 4, "", yv -> yv_module, yv -> yv_identifier);
	    else
		fprintf (stderr, "%*sbound identifier=\"%s\"\n",
			level * 4, "", yv -> yv_identifier);
	    break;

	case YV_IDLIST:
	case YV_VALIST:
	case YV_OIDLIST:
	    for (y = yv -> yv_idlist; y; y = y -> yv_next) {
		print_value (y, level + 1);
		fprintf (stderr, "%*s----\n", (level + 1) * 4, "");
	    }
	    break;

	default:
	    break;
    }
}

/*    SYMBOLS */

static SY  new_symbol (encpref, decpref, prfpref, mod, id)
register char  *encpref,
	       *decpref,
	       *prfpref,
	       *mod,
	       *id;
{
    register SY    sy;

    if ((sy = (SY) calloc (1, sizeof *sy)) == NULLSY)
	yyerror ("out of memory");
    sy -> sy_encpref = encpref;
    sy -> sy_decpref = decpref;
    sy -> sy_prfpref = prfpref;
    sy -> sy_module = mod;
    sy -> sy_name = id;

    if (snmpV2) {
	if (strlen (id) > 64)
	    warning ("%s \"%s\" should be <= 64 characters",
		     isupper ((unsigned char)*id) ? "identifier" : "descriptor", id);
	else
	    if (index (id, '-'))
		warning ("%s \"%s\" contains a hyphen character",
			 isupper ((unsigned char)*id) ? "identifier" : "descriptor", id);
    }

    sy -> sy_link = mylinks;
    return (mylinks = sy);
}


static SY  add_symbol (s1, s2)
register SY	s1,
		s2;
{
    register SY	    sy;

    if (s1 == NULLSY)
	return s2;

    for (sy = s1; sy -> sy_next; sy = sy -> sy_next)
	continue;
    sy -> sy_next = s2;

    return s1;
}

/*    YCs */

YC	new_yc (type, target, descr)
int	type;
char   *target,
       *descr;
{
    register YC	    yc;

    if ((yc =  (YC) calloc (1, sizeof *yc)) == NULLYC)
	yyerror ("out of memory");
    yc -> yc_type = type;
    yc -> yc_target =  target;
    yc -> yc_descr = descr;

    return yc;
}


YC	add_yc (yp1, yp2)
register YC	yp1,
		yp2;
{
    register YC	    yv;

    for (yv = yp1; yv -> yc_next; yv = yv -> yc_next)
	continue;
    yv -> yc_next = yp2;

    return yp1;
}

/*    YDs */

YD	new_yd ()
{
    register YD	    yd;

    if ((yd =  (YD) calloc (1, sizeof *yd)) == NULLYD)
	yyerror ("out of memory");

    return yd;
}


YD	add_yd (yp1, yp2)
register YD	yp1,
		yp2;
{
    register YD	    yv;

    for (yv = yp1; yv -> yd_next; yv = yv -> yd_next)
	continue;
    yv -> yd_next = yp2;

    return yp1;
}

/*    YRs */

YR	new_yr (utctime, descr)
char   *utctime,
       *descr;
{
    register YR	    yr;

    if ((yr =  (YR) calloc (1, sizeof *yr)) == NULLYR)
	yyerror ("out of memory");
    yr -> yr_utctime =  utctime;
    yr -> yr_descr = descr;

    return yr;
}


YR	add_yr (yp1, yp2)
register YR	yp1,
		yp2;
{
    register YR	    yv;

    for (yv = yp1; yv -> yr_next; yv = yv -> yr_next)
	continue;
    yv -> yr_next = yp2;

    return yp1;
}

/*    TYPES */

YP	new_type (code)
int	code;
{
    register YP    yp;

    if ((yp = (YP) calloc (1, sizeof *yp)) == NULLYP)
	yyerror ("out of memory");
    yp -> yp_code = code;

    return yp;
}


YP	add_type (yp1, yp2)
register YP	yp1,
		yp2;
{
    register YP	    yp;

    for (yp = yp1; yp -> yp_next; yp = yp -> yp_next)
	continue;
    yp -> yp_next = yp2;

    return yp1;
}

/*    VALUES */

YV	new_value (code)
int	code;
{
    register YV    yv;

    if ((yv = (YV) calloc (1, sizeof *yv)) == NULLYV)
	yyerror ("out of memory");
    yv -> yv_code = code;

    return yv;
}


YV	add_value (yp1, yp2)
register YV	yp1,
		yp2;
{
    register YV	    yv;

    for (yv = yp1; yv -> yv_next; yv = yv -> yv_next)
	continue;
    yv -> yv_next = yp2;

    return yp1;
}

/*    TAGS */

YT	new_tag (PElementClass	pele_class)
{
    register YT    yt;

    if ((yt = (YT) calloc (1, sizeof *yt)) == NULLYT)
	yyerror ("out of memory");
    yt -> yt_class = pele_class;

    return yt;
}

/*    STRINGS */

char   *new_string (s)
register char  *s;
{
    register char  *p;

    if ((p = malloc ((unsigned) (strlen (s) + 1))) == NULLCP)
	yyerror ("out of memory");

    (void) strcpy (p, s);
    return p;
}

static int 
print_defval (yo, id)     /* DAR  11/29/95   print defval */
    register OT     yo;
    char *id;
{
    if (yo->yo_defval == NULL) {
        return 0;
    }

    /* fprintf(stderr, "%s defvalcode: %d\n", yo->yo_name, yo->yo_defval->yv_code); */

    switch (yo -> yo_defval -> yv_code) {
	case YV_STRING:
            printf ("%-20s %-15s ", "%defval", yo->yo_name);
            printf ("%s\n", yo->yo_defval->yv_string);
            break;

	case YV_NUMBER:
            if ((strcmp(id, "INTEGER")==0) || (strcmp(id, "Integer32")==0)) {
                printf ("%-20s %-15s ", "%defval", yo->yo_name);
                printf ("%d\n", yo->yo_defval->yv_number);
            } else if (strcmp(id, "OctetString") == 0) {
                /* This will happen if an OctetString's DEFVAL is set to
                 * something like { ''H } or { 'ff'H } since mosy treats
                 * this as a hex number. For now, just ignore this case.
                 */
            } else if ((strcmp(id, "Unsigned32") == 0) ||
                       (strcmp(id, "Gauge") == 0) ||
                       (strcmp(id, "Gauge32") == 0) ) {
                printf ("%-20s %-15s ", "%defval", yo->yo_name);
                printf ("%lu\n", (unsigned long) yo->yo_defval->yv_number);
            }

            break;

	case YV_IDEFINED:
            printf ("%-20s %-15s ", "%defval", yo->yo_name);
            printf ("%s\n", yo->yo_defval->yv_identifier);
            break;

	case YV_BOOL:
	case YV_OIDLIST:
	case YV_IDLIST:
	case YV_VALIST:
	case YV_NULL:
	case YV_REAL:
            return 0;

        default:
            return 0;
    }
    return 1;
}

