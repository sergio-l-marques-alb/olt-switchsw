/* yacc.y - yacc ASN.1 parser */
/* This file produced automatically, do not edit! */

%{
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


#include "sr_conf.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif	/* HAVE_STDIO_H */

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif	/* HAVE_CTYPE_H */

#include "sr_proto.h"
#ifdef SR_VOS
#define SR_DEFINED_YYTEXT
#endif /* SR_VOS */
#include "mosydefs.h"

/* LVL7
#define	YYSTYPE	YYSTKTYPE
*/

#define	YYDEBUG	1

extern int snmpV2;
extern int oneflag;
extern int pepydebug;
extern int doexternals;

int	tagcontrol = TAG_UNKNOWN;
static int	tagdefault;
static int	listtype;
static OID	modid;

static int	yyporting = 0;

typedef struct ypa {
    char   *ya_text;
    int	    ya_lineno;
}		ypa, *YA;
#define	NULLYA	((YA) 0)

typedef struct	{
    char *ys_structname;
    char *ys_ptrname;
} ysa, *YS;
#define NULLYS ((YS) 0)

/* static function declarations */
static struct endpoint *new_endp
    SR_PROTOTYPE((YV yv));

static YA  new_action
    SR_PROTOTYPE((char *text,
    int lineno));

static YS new_ys
    SR_PROTOTYPE((char *str));

static struct ysub *new_ysub
    SR_PROTOTYPE((int code));

extern YT new_tag      SR_PROTOTYPE((PElementClass pele_class));

static YA  new_action (text, lineno)
char   *text;
int	lineno;
{
    register YA	    ya;

    if ((ya = (YA) calloc (1, sizeof *ya)) == NULLYA)
	yyerror ("out of memory");

    ya -> ya_text = text;
    ya -> ya_lineno = lineno;

    return ya;
}
#define	free_action(ya)	(void) free ((char *) (ya))

static YS new_ys (str)
char	*str;
{
    register YS	ys;
    register char *p, *q;
    char c;

    if((ys = (YS) malloc (sizeof *ys)) == NULLYS)
	yyerror ("out of memory");
    q = str;
    while (isspace ((u_char)*q)) q++;
    for (p = q; isalnum((u_char)*p) || *p == '_'; p++)
	continue;
    if (*p != '\0') {
	c = *p;
	*p = '\0';
	ys -> ys_structname = new_string (q);
	*p = c;
    }
    else {
	ys -> ys_structname = new_string (q);
	ys -> ys_ptrname = NULLCP;
	free (str);
	return ys;
    }
    for (q = p; *q != '$'; q++)
	if (! *q) {
	    free (str);
	    ys -> ys_ptrname = NULLCP;
	    return ys;
	}
    q++;
    while (isspace ((u_char)*q)) q++;
    for (p = q; isalnum((u_char)*p) || *p == '_'; p++)
	continue;
    *p = '\0';
    ys -> ys_ptrname = new_string (q);
    free (str);
    return ys;
}
#define	free_ys(ys) (void) free ((char *) (ys))

static int  final_actions = 0;

static struct endpoint *new_endp (yv)
YV	yv;
{
    struct endpoint *ep;

    if ((ep = (struct endpoint *) calloc (1, sizeof *ep)) == NULL)
	yyerror ("out of memory");
    ep -> ep_value = yv;

    return ep;
}

static struct ysub *new_ysub (code)
int	code;
{
    struct ysub *ys;

    if ((ys = (struct ysub *) calloc (1, sizeof *ys)) == NULL)
	yyerror ("out of memory");
    ys -> ysub_code = code;

    return ys;
}
%}

%start	ModuleDefinition


%union	{
#ifdef HAVE_STRTOL
    long    yy_number;
#else
    int     yy_number;
#endif
    char   *yy_string;
    YP	    yy_type;
    YV	    yy_value;
    YT	    yy_tag;
    YA	    yy_action;
    OID	    yy_oid;
    YS	    yy_sctrl;
    double  yy_real;
    YC	    yy_comp;
    YD	    yy_mib;
    YR	    yy_rev;
    struct ysub *yy_sub;
    struct endpoint *yy_endp;
}


%token	ABSENT ACTION ANY APPLICATION BAR BGIN BIT BITSTRING BOOLEAN
	BY CCE CHOICE COMMA COMPONENT COMPONENTS COMPONENTSOF CONTROL
	DECODER DEFAULT DEFINED DEFINITIONS DOT DOTDOT DOTDOTDOT
	ENCODER ENCRYPTED END ENUMERATED EXPORTS EXPLICIT MOSY_FALSE FROM
	ID IDENTIFIER IMPLICIT IMPORTS INCLUDES INTEGER LANGLE LBRACE
	LBRACKET LITNUMBER LITSTRING LPAREN MIN MAX NAME NIL OBJECT
	OCTET OCTETSTRING OF OPTIONAL PARAMETERTYPE PREFIXES PRESENT
	PRINTER PRIVATE RBRACE RBRACKET REAL RPAREN SECTIONS SEMICOLON
	SEQUENCE SEQUENCEOF SET SETOF SIZE STRING TAGS MOSY_TRUE UNIVERSAL
	VALA VALB VALI VALS VALP VALO VALOID VALR VALQ VALX VLENGTH
	WITH SCTRL PLUSINFINITY MINUSINFINITY
	MODULEID LASTUPDATE ORGANIZATION CONTACTINFO DESCRIPTION REVISION
	OBJECTIDENT STATUS REFERENCE
	OBJECTYPE SYNTAX BITSXX UNITS MAXACCESS ACCESS INDEX IMPLIED
	    AUGMENTS DEFVAL
	NOTIFY OBJECTS
	TRAPTYPE ENTERPRISE VARIABLES
	TEXTCONV DISPLAYHINT
	OBJECTGROUP
	NOTIFYGROUP NOTIFICATIONS
	MODCOMP MODULE MANDATORY GROUP WSYNTAX MINACCESS
	AGENTCAP PRELEASE SUPPORTS INCLUDING VARIATION CREATION

%type	<yy_number>	LITNUMBER
%type	<yy_real>	RealValue NumericRealValue SpecialRealValue
%type	<yy_string>	ID NAME LITSTRING VALA VALB VALI VALS VALP VALQ
			VALR VALO VALX VLENGTH VALOID CONTROL PARAMETERTYPE
			Control ValParameter String
			PassInt PassBool PassOid PassAny PassReal
			ModuleIdentifier Symbol SymbolList
			SCTRL
			UnitsPart DescrPart ReferPart MinAccessPart AccessPart
			MibIdentifier DisplayPart
%type	<yy_type>	Type BuiltinType DefinedType PassValue PassBit
			PassString NullPass ElementTypes OptionalTypeList
			OptionalType NamedType ComponentsOf AlternativeTypeList
			MemberTypes SubType
			Syntax WriteSyntax
%type	<yy_value>	NamedNumberList NamedNumber NumericValue
			Value BuiltinValue DefinedValue IdentifierList
			Values NamedValueList NamedValue NNlist
			ObjectID ObjectIDComponentList ObjectSubID
			IndexPart IndexTypes IndexType Index
			DefValPart ObjectPart
			VarPart VarTypeList VarTypes VarType MandatoryPart
			CreationPart Creation
%type	<yy_tag>	Tag Class
%type	<yy_action>	ACTION Action
%type   <yy_oid>	ObjIdComponentList ObjectIdentifierValue
			ObjIdComponent NumberForm NameAndNumberForm
%type	<yy_sctrl>	Sctrl
%type	<yy_comp>	CompliancePart Compliances Compliance
			VariationPart Variations Variation
%type	<yy_mib>	MibPart Mibs Mib ModulePart Modules Module
%type	<yy_rev>	RevisionPart Revisions Revision
%type	<yy_sub>	SubtypeSpec SubtypeAlternative SubtypeAlternativeList
			SubtypeValueSet SubtypeConstraint ValueRange
			SizeConstraint

%type	<yy_endp>	LowerEndPoint UpperEndpoint LowerEndValue UpperEndValue
%%

ModuleDefinition:	ModuleIdentifier DEFINITIONS TagDefault CCE
				{
    				    mymodule = $1;
				    mymoduleid = modid;
				    pass1 ();
				}
			ModuleActions ModulePrefixes
			BGIN ModuleBody END { pass2 (); final_actions++; }
			ModuleActions
	;

TagDefault:		EXPLICIT TAGS { 
			    tagdefault = 0;
			    tagcontrol = TAG_EXPLICIT;
			}
	|		IMPLICIT TAGS {
			    tagdefault = YP_IMPLICIT;
			    tagcontrol = TAG_IMPLICIT;
			}
	|		empty {
			    tagdefault = 0;
			    tagcontrol = TAG_UNKNOWN;
			}
	;

ModuleIdentifier:	ID AssignedIdentifier { $$ = $1; }
	;

AssignedIdentifier:	ObjectIdentifierValue { modid = $1; }
	|		empty
	;

ModuleActions:		ACTION {
			    if (!dflag) {
				if (Cflag > 0) {
				    if (!Pflag && *sysin)
					printf ("# line %d \"%s\"\n",
						$1 -> ya_lineno, sysin);
				    fputs ($1 -> ya_text, stdout);
				}
				else {
				    printf ("%%{\n#include \"%s-types.h\"\n",
					    mymodule);
				    printf ("%s%%}\n", $1 -> ya_text);
				    Cflag = -1;
				}
			    }
			    free_action ($1);
			}
	|		empty
	;
ModulePrefixes:
			PREFIXES String String String
			{
			    yyencdflt = $2;
			    yydecdflt = $3;
			    yyprfdflt = $4;
			}
	|		empty
	;
ModuleBody:		Exports Imports AssignmentList
	;

Exports:		EXPORTS  { listtype = TBL_EXPORT; doexternals = 0;
				   yyporting = 1; }
				 SymbolsExported SEMICOLON {
			    yyporting = 0;
#if	0
			    if (mymoduleid == NULLOID)
				warning ("EXPORTS but no ModuleIdentifier");
#endif
			}
	|		empty
	;

SymbolsExported:	SymbolList { }
	|		empty
	;

Imports:		IMPORTS  { listtype = TBL_IMPORT; yyporting = 1; }
				 SymbolsImported SEMICOLON {
			    yyporting = 0;
#if	0
			    if (mymoduleid == NULLOID)
				warning ("IMPORTS but no ModuleIdentifier");
#endif
			}

	|		empty
	;

SymbolsImported:	SymbolsFromModuleList
	|		empty
	;

SymbolsFromModuleList:	SymbolsFromModuleList SymbolsFromModule
	|		SymbolsFromModule
	;

SymbolsFromModule:	SymbolList FROM ModuleIdentifier
				{
				    addtableref ($3, modid, listtype);
				    if (!snmpV2 
					    && $3
					    && strcmp ($3, "SNMPv2-SMI") == 0)
					snmpV2 = 1;
				    if (listtype == TBL_IMPORT)
					tcltableref ($3);
				}
	;

SymbolList:		SymbolList COMMA Symbol { 
			    if (!oneflag
				    || listtype != TBL_IMPORT
				    || !check_expimp ($3))
				addtable($3, listtype);
			}
	|		Symbol {
			    if (!oneflag
				    || listtype != TBL_IMPORT
				    || !check_expimp ($1))
				addtable($1, listtype);
			}
	;

Symbol:			ID
	|		NAME
	;

AssignmentList:		AssignmentList Section Assignment
	|		empty
	;

Section:		ENCODER String
			{ yysection = YP_ENCODER; yyencpref = $2;
			  yyprint (NULLCP, 0, 1); }
	|		DECODER String
			{ yysection = YP_DECODER; yydecpref = $2;
			  yyprint (NULLCP, 0, 1); }
	|		PRINTER String
			{ yysection = YP_PRINTER; yyprfpref = $2;
			  yyprint (NULLCP, 0, 1); }
	|		SECTIONS String String String
			{
			    yysection = 0;
			    if (strcmp (yyencpref = $2, "none"))
				yysection |= YP_ENCODER;
			    if (strcmp (yydecpref = $3, "none"))
				yysection |= YP_DECODER;
			    if (strcmp (yyprfpref = $4, "none"))
				yysection |= YP_PRINTER;
			    yyprint (NULLCP, 0, 1); 
			}
	|		empty
	;

String:			ID { $$ = $1; }
	|		NAME { $$ = $1; }
	;

Assignment:		Typeassignment
	|		ModuleIDefinition
	|		ObjectIDefinition
	|		ObjectDefinition
	|		ObjectTypeDefinition
	|		NotifyDefinition
	|		TrapTypeDefinition
	|		TextualConventionDefinition
	|		ObjectGroupDefinition
	|		NotifyGroupDefinition
	|		ModComplianceDefinition
	|		AgentCapabilitiesDefinition
	|		Valueassignment
	;


ModuleIDefinition:	NAME MODULEID LASTUPDATE LITSTRING ORGANIZATION
			LITSTRING CONTACTINFO LITSTRING DESCRIPTION LITSTRING
			RevisionPart CCE ObjectID {
			    pass1_mod (mymodule, $1, $13, $4, $6, $8, $10,
				       $11);
			}
	;
RevisionPart:		Revisions { $$ = $1; }
	|		empty { $$ = NULLYR; }
	;
Revisions:		Revisions Revision { $$ = add_yr ($1, $2); }
	|		Revision { $$ = $1; }
	;
Revision:		REVISION LITSTRING DESCRIPTION LITSTRING {
			    $$ = new_yr ($2, $4);
			}
	;

ObjectIDefinition:	NAME OBJECT IDENTIFIER CCE ObjectID {
			    pass1_oid (mymodule, $1, $5, NULLCP, NULLCP,
				       NULLCP);
			}
	;
ObjectID:		NAME {
			    $$ = new_value (YV_OIDLIST);
			    $$ -> yv_idlist = new_value (YV_IDEFINED);
			    $$ -> yv_idlist -> yv_identifier = $1;
			    $$ -> yv_idlist -> yv_flags |= YV_BOUND;
			}
	|		LBRACE ObjectIDComponentList RBRACE {
			    $$ = new_value (YV_OIDLIST);
			    $$ -> yv_idlist = $2;
			}
	;
ObjectIDComponentList:	ObjectSubID { $$ = $1; }
	|		ObjectIDComponentList ObjectSubID {
			    $$ = add_value ($1, $2);
			}
	;
ObjectSubID:		LITNUMBER {
			    $$ = new_value (YV_NUMBER);
			    $$ -> yv_number = $1;
			}
	|		NAME {
			    $$ = new_value (YV_IDEFINED);
			    $$ -> yv_identifier = $1;
			    $$ -> yv_flags |= YV_BOUND;
			}
	|		NAME LPAREN LITNUMBER RPAREN {
			    $$ = new_value (YV_NUMBER);
			    $$ -> yv_number = $3;
			    $$ -> yv_named = $1;
			    $$ -> yv_flags |= YV_NAMED;
			}
	;

ObjectDefinition:	NAME OBJECTIDENT STATUS NAME DESCRIPTION LITSTRING
			ReferPart CCE ObjectID {
			    pass1_oid (mymodule, $1, $9, $4, $6, $7);
			};
ReferPart:		REFERENCE LITSTRING { $$ = $2; }
	|		empty { $$ = NULL; }
	;

ObjectTypeDefinition:	NAME OBJECTYPE SYNTAX NamedType UnitsPart Access NAME
			STATUS NAME DescrPart ReferPart IndexPart DefValPart
			CCE ObjectID {
			    pass1_obj (mymodule, $1, $15, $4, $5, $7, $9,
				       $10, $11, $12, $13);
			}
	;
UnitsPart:		UNITS LITSTRING { $$ = $2; }
	|		empty { $$ = NULL; }
	;
Access:			MAXACCESS {}
	|		ACCESS {
			    if (snmpV2)
				yyerror ("use \"MAX-ACCESS\" not \"ACCESS\"");
			}
DescrPart:		DESCRIPTION LITSTRING { $$ = $2; }
	|		empty { $$ = NULL; }
	;
IndexPart:		INDEX LBRACE IndexTypes RBRACE {
			    $$ = new_value (YV_VALIST);
			    $$ -> yv_idlist = $3;
			}
	|		AUGMENTS LBRACE NAME RBRACE {
			    $$ = new_value (YV_IDEFINED);
			    $$ -> yv_identifier = $3;
			    $$ -> yv_flags |= YV_BOUND;
			}
	|		empty { $$ = NULLYV; }
	;
IndexTypes:		IndexType { $$ = $1; }
	|		IndexTypes COMMA IndexType {
			    $$ = add_value ($1, $3);
			}
	;
IndexType:		IMPLIED Index { ($$ = $2) -> yv_flags |= YV_IMPLIED; }
	|		Index { $$ = $1; }
Index:			NAME {	/* e.g., ifIndex */
			    $$ = new_value (YV_IDEFINED);
			    $$ -> yv_identifier = $1;
			    $$ -> yv_flags |= YV_BOUND;
			}
	|		ID {	/* e.g., INTEGER */
			    $$ = new_value (YV_IDEFINED);
			    $$ -> yv_identifier = $1;
			}
	|		INTEGER { $$ = new_value (YV_NUMBER); }
	|		Octetstring { $$ = new_value (YV_STRING); }
	|		OBJECT IDENTIFIER { $$ = new_value (YV_OIDLIST); }
	;
DefValPart:		DEFVAL LBRACE Value RBRACE { $$ = $3; }
	|		empty { $$ = NULLYV; }
	;

NotifyDefinition:	NAME NOTIFY ObjectPart STATUS NAME DESCRIPTION
			LITSTRING ReferPart CCE ObjectID {
			    pass1_trap (mymodule, $1, $10, -1, $3, $5, $7, $8);
			}
	;
ObjectPart:		OBJECTS LBRACE VarTypeList RBRACE { $$ = $3; }
	|		empty { $$ = NULLYV; }
	;

TrapTypeDefinition:	NAME TRAPTYPE ENTERPRISE ObjectID VarPart DescrPart
			ReferPart CCE LITNUMBER {
			    if (snmpV2)
				yyerror ("use \"NOTIFICATION-TYPE\" not \"TRAP-TYPE\"");
			    pass1_trap (mymodule, $1, $4, $9, $5, NULLCP, $6,
				        $7);
			}
	;
VarPart:		VARIABLES LBRACE VarTypeList RBRACE { $$ = $3; }
	|		empty { $$ = NULLYV; }
	;
VarTypeList:		VarTypes {
			    ($$ = new_value (YV_VALIST)) -> yv_idlist = $1;
			}
VarTypes:		VarType { $$ = $1; }
	|		VarTypes COMMA VarType {
			    $$ = add_value ($1, $3);
			}
	;
VarType:		NAME {	/* e.g., ifIndex */
			    $$ = new_value (YV_IDEFINED);
			    $$ -> yv_identifier = $1;
			    $$ -> yv_flags |= YV_BOUND;
			}
	;

TextualConventionDefinition: ID CCE TEXTCONV DisplayPart STATUS NAME
			DESCRIPTION LITSTRING ReferPart SYNTAX NamedType {
			    pass1_conv (mymodule, $1, $11, $4, $6, $8, $9);
			}
	;
DisplayPart:		DISPLAYHINT LITSTRING { $$ = $2; }
	|		empty { $$ = NULL; }
	;

ObjectGroupDefinition:  NAME OBJECTGROUP OBJECTS LBRACE VarTypeList RBRACE
			STATUS NAME DESCRIPTION LITSTRING ReferPart CCE
			ObjectID {
			    pass1_grp (mymodule, $1, $13, $5, $8, $10, $11, 0);
			}
	;

NotifyGroupDefinition:  NAME NOTIFYGROUP NOTIFICATIONS LBRACE VarTypeList RBRACE
			STATUS NAME DESCRIPTION LITSTRING ReferPart CCE
			ObjectID {
			    pass1_grp (mymodule, $1, $13, $5, $8, $10, $11, 1);
			}
	;

ModComplianceDefinition:  NAME MODCOMP STATUS NAME DESCRIPTION LITSTRING
			  ReferPart MibPart CCE ObjectID {
			    pass1_comp (mymodule, $1, $10, $4, $6, $7, $8);
			}
	;
MibPart:		Mibs { $$ = $1; }
	|		empty { $$ = NULLYD; }
	;
Mibs:			Mibs Mib { $$ = add_yd ($1, $2); }
	|		Mib { $$ = $1; }
	;
Mib:			MODULE MibIdentifier MandatoryPart CompliancePart {
			    $$ = new_yd ();
			    $$ -> yd_modname = $2;
			    $$ -> yd_modid = modid;
			    $$ -> yd_groups = $3;
			    $$ -> yd_variations = $4;
			}
	;
MibIdentifier:		ModuleIdentifier { $$ = $1; }
	|		empty { $$ = NULL; }
	;
MandatoryPart:		MANDATORY LBRACE VarTypeList RBRACE { $$ = $3; }
	|		empty { $$ = NULLYV; }
	;
CompliancePart:		Compliances { $$ = $1; }
	|		empty { $$ = NULLYC; }
	;
Compliances:		Compliances Compliance { $$ = add_yc ($1, $2); }
	|		Compliance { $$ = $1; }
	;
Compliance:		GROUP NAME DESCRIPTION LITSTRING {
			    $$ = new_yc (YC_GROUP, $2, $4);
			}
	|		OBJECT NAME Syntax WriteSyntax MinAccessPart
			DESCRIPTION LITSTRING {
			    $$ = new_yc (YC_OBJECT, $2, $7);
			    $$ -> yc_syntax = $3;
			    $$ -> yc_wsyntax = $4;
			    $$ -> yc_access = $5;
			}
	;
Syntax:			SYNTAX NamedType { $$ = $2; }
	|		empty { $$ = NULLYP; }
	;

WriteSyntax:		WSYNTAX NamedType { $$ = $2; }
	|		empty { $$ = NULLYP; }
	;
MinAccessPart:		MINACCESS NAME { $$ = $2; }
	|		empty { $$ = NULL; }
	;

AgentCapabilitiesDefinition:  NAME AGENTCAP PRELEASE LITSTRING STATUS NAME
			DESCRIPTION LITSTRING ReferPart ModulePart CCE
			ObjectID {
			    pass1_cap (mymodule, $1, $12, $4, $6, $8, $9, $10);
			}
	;
ModulePart:		Modules { $$ = $1; }
	|		empty { $$ = NULLYD; }
	;
Modules:		Modules Module { $$ = add_yd ($1, $2); }
	|		Module { $$ = $1; }
	;
Module:			SUPPORTS ModuleIdentifier INCLUDING LBRACE VarTypeList
			RBRACE VariationPart {
			    $$ = new_yd ();
			    $$ -> yd_modname = $2;
			    $$ -> yd_modid = modid;
			    $$ -> yd_groups = $5;
			    $$ -> yd_variations = $7;
			}
	;
VariationPart:		Variations { $$ = $1; }
	|		empty { $$ = NULLYC; }
	;
Variations:		Variations Variation { $$ = add_yc ($1, $2); }
	|		Variation { $$ = $1; }
	;
Variation:		VARIATION NAME Syntax WriteSyntax AccessPart
			CreationPart DefValPart DESCRIPTION
			LITSTRING {
			    $$ = new_yc (YC_VARIATION, $2, $9);
			    $$ -> yc_syntax = $3;
			    $$ -> yc_wsyntax = $4;
			    $$ -> yc_access = $5;
			    $$ -> yc_creation = $6;
			    $$ -> yc_defval = $7;
			}
	;
AccessPart:		ACCESS NAME { $$ = $2; }
	|		empty { $$ = NULL; }
	;
CreationPart:		CREATION LBRACE Creation RBRACE { $$ = $3; }
	|		empty { $$ = NULLYV; }
Creation:		VarTypeList { $$ = $1; }
	|		empty { $$ = NULLYV; }
	;

Typeassignment:		ID CCE Type {
			    pass1_type (yyencpref, yydecpref, yyprfpref,
				    mymodule, $1, $3);
			} ;

Type:			BuiltinType Action {
			    $$ = $1;
			    $$ -> yp_direction = yysection;
			    if ($2) {
				$$ -> yp_action2 = $2 -> ya_text;
				$$ -> yp_act2_lineno= $2 -> ya_lineno;
				free_action ($2);
			    }
			}
	|		DefinedType Action {
			    $$ = $1;
			    $$ -> yp_direction = yysection;
			    if ($2) {
				$$ -> yp_action2 = $2 -> ya_text;
				$$ -> yp_act2_lineno= $2 -> ya_lineno;
				free_action ($2);
			    }
			}
	|		SubType Action {
			    $$ = $1;
			    $$ -> yp_direction = yysection;
			    if ($2) {
			    	$$ -> yp_action2 = $2 -> ya_text;
				$$ -> yp_act2_lineno = $2 -> ya_lineno;
				free_action ($2);
			    }
			}
	;

Action:			ACTION
	|		empty { $$ = NULLYA; }
	;

Control:		CONTROL
	|		empty { $$ = NULLCP; }
	;


BuiltinType:		BOOLEAN PassBool {
			    $$ = new_type (YP_BOOL);
			    $$ -> yp_intexp = $2;
			}
	|		INTEGER PassInt NNlist {
			    $$ = new_type ($3 ? YP_INTLIST : YP_INT);
			    $$ -> yp_intexp = $2;
			    if ($3)
				    $$ -> yp_value = $3;
	    		}
	|		ENUMERATED PassInt LBRACE NamedNumberList RBRACE {
			    $$ = new_type (YP_ENUMLIST);
			    $$ -> yp_intexp = $2;
			    $$ -> yp_value = $4;
			}

	|		Bitstring PassBit NNlist {
			    $$ = $2;
	    		    $$ -> yp_code = $3 ? YP_BITLIST : YP_BIT;
			    if ($3)
				$$ -> yp_value = $3;
			}
	|		BITSXX NNlist {
			    $$ = new_type (YP_BITS);
			    $$ -> yp_value = $2;
			}
	|		Octetstring PassString {
			    $$ = $2;
			    $$ -> yp_code = YP_OCT;
			}
	|		NIL { $$ = new_type (YP_NULL); }
	|		SEQUENCE Sctrl PassAny {
			    $$ = new_type (YP_SEQ);
			    $$ -> yp_strexp = $3;
			}
	|		SequenceOf Sctrl Action Control Type {
			    $$ = new_type (YP_SEQTYPE);
			    $$ -> yp_type = $5;
			    if ($2) {
				$$ -> yp_structname = $2 -> ys_structname;
				$$ -> yp_ptrname = $2 -> ys_ptrname;
				free_ys ($2);
			    }
			    if ($4) {
				$$ -> yp_control = $4;
				$$ -> yp_flags |= YP_CONTROLLED;
			    }
			    if ($3) {
				$$ -> yp_action3 = $3 -> ya_text;
				$$ -> yp_act3_lineno = $3 -> ya_lineno;
				free_action ($3);
			    }
			}
	|		SEQUENCE Sctrl Action LBRACE ElementTypes RBRACE {
			    $$ = new_type (YP_SEQLIST);
			    $$ -> yp_type = $5;
			    if ($2) {
				$$ -> yp_structname = $2 -> ys_structname;
				$$ -> yp_ptrname = $2 -> ys_ptrname;
				free_ys ($2);
			    }
			    if ($3 && $$ -> yp_action1 == NULLCP) {
				$$ -> yp_action1 = $3 -> ya_text;
                                $$ -> yp_act1_lineno = $3 -> ya_lineno;
                                free_action ($3);
                            }
                            else if ($3)
                                yyerror("too many actions associated with SEQUENCE");
	    		}
	|		SET Sctrl PassAny {
			    $$ = new_type (YP_SET);
			    $$ -> yp_strexp = $3;
			}
	|		SetOf Sctrl Action Control Type {
			    $$ = new_type (YP_SETTYPE);
			    $$ -> yp_type = $5;
			    if ($2) {
				$$ -> yp_structname = $2 -> ys_structname;
				$$ -> yp_ptrname = $2 -> ys_ptrname;
				free_ys ($2);
			    }
			    if ($4) {
				$$ -> yp_control = $4;
				$$ -> yp_flags |= YP_CONTROLLED;
			    }
			    if ($3) {
				$$ -> yp_action3 = $3 -> ya_text;
				$$ -> yp_act3_lineno = $3 -> ya_lineno;
				free_action ($3);
			    }
			}

	|		SET Sctrl Action LBRACE MemberTypes RBRACE {
			    $$ = new_type (YP_SETLIST);
			    $$ -> yp_type = $5;
			    if ($2) {
				$$ -> yp_structname = $2 -> ys_structname;
				$$ -> yp_ptrname = $2 -> ys_ptrname;
				free_ys ($2);
			    }
			    if ($3 && $$ -> yp_action1 == NULLCP) {
                                $$ -> yp_action1 = $3 -> ya_text;
                                $$ -> yp_act1_lineno = $3 -> ya_lineno;
                                free_action ($3);
                            }
                            else if ($3)
                                yyerror("too many actions associated with SET");
	    		}
	|		Tag Action IMPLICIT Type {
			    $$ = $4;
			    $$ -> yp_tag = $1;
			    if ($2 && $$ -> yp_action1 == NULLCP) {
				$$ -> yp_action1 = $2 -> ya_text;
				$$ -> yp_act1_lineno= $2 -> ya_lineno;
				free_action ($2);
			    }
			    else if ($2)
				yyerror("too many actions associated with tagged type");
			    $$ -> yp_flags |= YP_IMPLICIT | YP_TAG;
			}
	|		Tag Action EXPLICIT Type {
			    $$ = $4;
			    $$ -> yp_tag = $1;
			    if ($2 && $$ -> yp_action1 == NULLCP) {
				$$ -> yp_action1 = $2 -> ya_text;
				$$ -> yp_act1_lineno= $2 -> ya_lineno;
				free_action ($2);
			    }
			    else if ($2)
				yyerror("too many actions associated with tagged type");
			    $$ -> yp_flags |= YP_TAG;
			}
	|		Tag Action Type {
			    $$ = $3;
			    $$ -> yp_tag = $1;
			    if ($2 && $$ -> yp_action1 == NULLCP) {
				$$ -> yp_action1 = $2 -> ya_text;
				$$ -> yp_act1_lineno= $2 -> ya_lineno;
				free_action ($2);
			    }
			    else if ($2)
				yyerror("too many actions associated with tagged type");
			    $$ -> yp_flags |= (YP_TAG | tagdefault);
			}
	|		CHOICE Sctrl Action Control LBRACE 
					AlternativeTypeList RBRACE {
			    $$ = new_type (YP_CHOICE);
			    $$ -> yp_type = $6;
			    if ($2) {
				$$ -> yp_structname = $2 -> ys_structname;
				$$ -> yp_ptrname = $2 -> ys_ptrname;
				free_ys ($2);
			    }
			    if ($4) {
				$$ -> yp_control = $4;
				$$ -> yp_flags |= YP_CONTROLLED;
			    }
			    if ($$ -> yp_action1 == NULLCP && $3) {
				$$ -> yp_action1 = $3 -> ya_text;
				$$ -> yp_act1_lineno = $3 -> ya_lineno;
				free_action ($3);
			    }
			    else if ($3)
				yyerror("too many actions associated with CHOICE");
			}
	|		NAME LANGLE Type {
			    $$ = $3;
			    $$ -> yp_bound = $1;
			    $$ -> yp_flags |= YP_BOUND;
			}
	|		AnyType PassAny {
			    $$ = new_type (YP_ANY);
			    $$ -> yp_strexp = $2;
			}
	|		OBJECT IDENTIFIER PassOid {
			    $$ = new_type (YP_OID);
			    $$ -> yp_strexp = $3;
			}
	|		ENCRYPTED Type {
			    $$ = $2;
			    $$ -> yp_tag = new_tag (PE_CLASS_UNIV);
			    $$ -> yp_tag -> yt_value = new_value(YV_NUMBER);
			    $$ -> yp_tag -> yt_value -> yv_number = PE_PRIM_ENCR;
			    $$ -> yp_flags |=
			    	(YP_ENCRYPTED | YP_TAG | YP_IMPLICIT);
			}
	|		REAL PassReal {
			    $$ = new_type(YP_REAL);
			    $$ -> yp_strexp = $2;
			}
	;

NNlist:			LBRACE NamedNumberList RBRACE { $$ = $2; }
	|		empty {	$$ = NULL; }
	;
DefinedType:		ID PassValue ValParameter {
			    $$ = $2;
			    $$ -> yp_code = YP_IDEFINED;
			    $$ -> yp_identifier = $1;
			    if ($3) {
				$$ -> yp_parm = $3;
				$$ -> yp_flags |= YP_PARMVAL;
			    }
			    check_impexp ($$);
			}
	|		ID DOT ID PassValue ValParameter {
			    $$ = $4;
			    $$ -> yp_code = YP_IDEFINED;
			    $$ -> yp_identifier = $3;
			    $$ -> yp_module = $1;
			    if ($5) {
				$$ -> yp_parm = $5;
				$$ -> yp_flags |= YP_PARMVAL;
			    }
			}
	;

PassValue:		VALA {
			    $$ = new_type (YP_UNDF);
			    $$ -> yp_intexp = NULLCP;
			    $$ -> yp_strexp = $1;
			    $$ -> yp_prfexp = 'a';
			}
	|		VALB {
			    $$ = new_type (YP_UNDF);
			    $$ -> yp_intexp = $1;
			    $$ -> yp_strexp = NULLCP;
			    $$ -> yp_prfexp = 'b';
			}
	|		VALI {
			    $$ = new_type (YP_UNDF);
			    $$ -> yp_intexp = $1;
			    $$ -> yp_strexp = NULLCP;
			    $$ -> yp_prfexp = 'i';
			}
	|		VALX VLENGTH {
			    $$ = new_type (YP_UNDF);
			    $$ -> yp_intexp = $2;
			    $$ -> yp_strexp = $1;
			    $$ -> yp_prfexp = 'x';
			}
	|		VALOID {
			    $$ = new_type (YP_UNDF);
			    $$ -> yp_intexp = NULLCP;
			    $$ -> yp_strexp = $1;
			    $$ -> yp_prfexp = 'O';
			}
	|		PassString
	;

AnyType:		ANY
	|		ANY DEFINED BY NAME
	;
			
PassOid:		VALOID
	|		empty { $$ = NULLCP; }
	;

PassAny:		VALA
	|		empty { $$ = NULLCP; }
	;

PassBool:		VALB
	|		empty { $$ = NULLCP; }
	;

PassInt:		VALI
	|		empty { $$ = NULLCP; }
	;

PassBit:		VALX VLENGTH {
			    $$ = new_type (YP_UNDF);
			    $$ -> yp_intexp = $2;
			    $$ -> yp_strexp = $1;
			}
	|		NullPass
	;

PassString:		VALS {
			    $$ = new_type (YP_UNDF);
			    $$ -> yp_intexp = NULLCP;
			    $$ -> yp_strexp = $1;
			    $$ -> yp_prfexp = 's';
			}
	|		VALO VLENGTH {
			    $$ = new_type (YP_UNDF);
			    $$ -> yp_intexp = $2;
			    $$ -> yp_strexp = $1;
			    $$ -> yp_prfexp = 'o';
			}
	|		VALQ {
			    $$ = new_type (YP_UNDF);
			    $$ -> yp_intexp = NULLCP;
			    $$ -> yp_strexp = $1;
			    $$ -> yp_prfexp = 'q';
			}
	|		NullPass
	;

PassReal:		VALR 
	|		empty { $$ = NULLCP; }
	;

NullPass:		empty {
			    $$ = new_type (YP_UNDF);
			    $$ -> yp_intexp = NULLCP;
			    $$ -> yp_strexp = NULLCP;
			}
	;

ValParameter:		VALP
	|		empty { $$ = NULLCP; }
	;

NamedNumberList:	NamedNumber { $$ = $1; }
	|		NamedNumberList COMMA NamedNumber {
			    $$ = add_value ($1, $3);
			}
	;
NamedNumber:		NAME LPAREN NumericValue RPAREN Action {
			    $$ = $3;
			    $$ -> yv_named = $1;
			    $$ -> yv_flags |= YV_NAMED;
			    if ($5) {
				$$ -> yv_action = $5 -> ya_text;
				$$ -> yv_act_lineno = $5 -> ya_lineno;
				free_action ($5);
			    }
			}
	;
NumericValue:		LITNUMBER {
			    $$ = new_value (YV_NUMBER);
			    $$ -> yv_number = $1;
			}
	|		DefinedValue
	;

ElementTypes:		OptionalTypeList { $$ = $1; }
	|		empty { $$ = NULLYP; }
	;
MemberTypes:		OptionalTypeList { $$ = $1; }

	|		empty { $$ = NULLYP; }
	;
OptionalTypeList:	OptionalType { $$ = $1; }
	|		OptionalTypeList COMMA OptionalType {
			    $$ = add_type ($1, $3);
			}
	;
OptionalType:		NamedType { $$ = $1; }
	|		NamedType OPTIONAL Control {
			    $$ = $1;
			    $$ -> yp_flags |= YP_OPTIONAL;
			    if ($3) {
				$$ -> yp_optcontrol = $3;
				$$ -> yp_flags |= YP_OPTCONTROL;
			    }
			}
	|		NamedType DEFAULT Value Control {
			    $$ = $1;
			    $$ -> yp_default = $3;
			    $$ -> yp_flags |= YP_DEFAULT;
			    if ($4) {
				$$ -> yp_optcontrol = $4;
				$$ -> yp_flags |= YP_OPTCONTROL;
			    }
			}
	|		ComponentsOf { $$ = $1; }
	|		NAME ComponentsOf { $$ = $2; $$->yp_id = $1; }
	;
NamedType:		NAME Action Type {
			    $$ = $3;
			    $$ -> yp_id = $1;
			    if ($$ -> yp_action1 == NULLCP && $2) {
				$$ -> yp_action1 = $2 -> ya_text;
				$$ -> yp_act1_lineno = $2 -> ya_lineno;
				free_action ($2);
			    }
			    $$ -> yp_flags |= YP_ID;
			}
	|		Type { $$ = $1; }
	;
ComponentsOf:		Components Type {
			    $$ = $2;
			    $$ -> yp_flags |= YP_COMPONENTS;
			}
	;
AlternativeTypeList:	NamedType { $$ = $1; }
	|		AlternativeTypeList COMMA NamedType {
			    $$ = add_type ($1, $3);
			}
	;

Tag:			LBRACKET Class NumericValue RBRACKET {
			    $$ = $2;
			    $$ -> yt_value = $3;
			}
	;
Class:			UNIVERSAL { $$ = new_tag (PE_CLASS_UNIV); }
	| 		APPLICATION { $$ = new_tag (PE_CLASS_APPL); }
	|		PRIVATE { $$ = new_tag (PE_CLASS_PRIV); }
	|		empty { $$ = new_tag (PE_CLASS_CONT); }
	;


Valueassignment:	NAME Type CCE Value { }
	;

Value:			BuiltinValue
	|		DefinedValue
	;
BuiltinValue:		MOSY_TRUE {
			    $$ = new_value (YV_BOOL);
			    $$ -> yv_number = 1;
			}
	|		MOSY_FALSE {
			    $$ = new_value (YV_BOOL);
			    $$ -> yv_number = 0;
			}
	|		LITNUMBER {
			    $$ = new_value (YV_NUMBER);
			    $$ -> yv_number = $1;
			}
	|		LBRACE ID IdentifierList RBRACE {
			    $$ = new_value (YV_IDLIST);
			    $$ -> yv_idlist = new_value (YV_IDEFINED);
			    $$ -> yv_idlist -> yv_identifier = $2;
			    if ($3)
			        $$ = add_value ($$, $3);
			}
	|		LBRACE ObjectIDComponentList RBRACE {
			    $$ = new_value (YV_OIDLIST);
			    $$ -> yv_idlist = $2;
			}
	|		LITSTRING {
			    $$ = new_value (YV_STRING);
			    $$ -> yv_string = $1;
			}
	|		NIL { $$ = new_value (YV_NULL); }
	|		LBRACE Values RBRACE {
			    $$ = new_value (YV_VALIST);
			    $$ -> yv_idlist = $2;
			}
	|		NAME Value {
			    $$ = $2;
			    $$ -> yv_id = $1;
			    $$ -> yv_flags |= YV_ID;
			}
	|		Type Value {
			    $$ = $2;
			    $$ -> yv_type = $1;
			    $$ -> yv_flags |= YV_TYPE;
			}
	|		RealValue {
			    $$ = new_value (YV_REAL);
			    $$ -> yv_real = $1;
			}
	;

RealValue: 		NumericRealValue
	|		SpecialRealValue
	;

SpecialRealValue:	PLUSINFINITY { $$ = PE_REAL_INFINITY;} 
	|		MINUSINFINITY { $$ = - PE_REAL_INFINITY; }
	;

NumericRealValue:	LBRACE LITNUMBER COMMA LITNUMBER COMMA
				 LITNUMBER RBRACE {
				if ($4 != 2 && $4 != 10)
					myyerror ("Illegal base value %d", $4);
				$$ = $2 * pow ((double)$4, (double)$6);
			}
	;

DefinedValue:		ID {
			    $$ = new_value (YV_IDEFINED);
			    $$ -> yv_identifier = $1;
			}
	|		NAME {	/* XXX */
			    $$ = new_value (YV_IDEFINED);
			    $$ -> yv_identifier = $1;
			    $$ -> yv_flags |= YV_BOUND;
			}
	|		ID DOT ID {
			    $$ = new_value (YV_IDEFINED);
			    $$ -> yv_identifier = $3;
			    $$ -> yv_module = $1;
			}
	;

IdentifierList:		IdentifierList COMMA ID {
			    $$ = new_value (YV_IDEFINED);
			    $$ -> yv_identifier = $3;
			    if ($1)
			        $$ = add_value ($1, $$);
			}
	|		empty { $$ = NULLYV; }
	;

Values:			NamedValueList { $$ = $1; }
	|		empty { $$ = NULLYV; }
	;
NamedValueList:		NamedValue { $$ = $1; }
	|		NamedValueList COMMA NamedValue {
			    $$ = add_value ($1, $3);
			}
	;
NamedValue:		Value { $$ = $1; }
	;

empty:	;

Octetstring:		OCTET STRING
	|		OCTETSTRING
	;

Bitstring:		BITSTRING
	|		BIT STRING
	;

SequenceOf:		SEQUENCEOF
	|		SEQUENCE OF
	;
SetOf:			SETOF
	|		SET OF
	;
Components:		COMPONENTSOF
	|		COMPONENTS OF
	;

ObjectIdentifierValue:	LBRACE ObjIdComponentList RBRACE
			{ $$ = $2; }
	;

ObjIdComponentList:	ObjIdComponent
	|		ObjIdComponentList ObjIdComponent
			{
				$$ = addoid($1, $2);
				oid_free ($1);
				oid_free ($2);
			}
	;

ObjIdComponent:		NumberForm
	|		NameAndNumberForm
	;

NumberForm:		LITNUMBER { $$ = int2oid ($1); }
	|		DefinedValue {
				$$ = oidlookup($1->yv_identifier);
				free((char *)$1);
			}
	;

NameAndNumberForm:	NAME LPAREN NumberForm RPAREN {
				free ($1);
				$$ = $3;
			}
	;

SubType:		Type SubtypeSpec {
			    $$ -> yp_flags |= YP_SUBTYPE;
			    $$ -> yp_subtype = $2;
			}
	|		SET SizeConstraint OF Sctrl Action Control Type {
			    $$ = new_type (YP_SETTYPE);
			    $$ -> yp_type = $7;
			    if ($4) {
				$$ -> yp_structname = $4 -> ys_structname;
				$$ -> yp_ptrname = $4 -> ys_ptrname;
				free_ys ($4);
			    }
			    if ($6) {
			    	$$ -> yp_control = $6;
				$$ -> yp_flags |= YP_CONTROLLED;
			    }
			    if ($5) {
			    	$$ -> yp_action3 = $5 -> ya_text;
				$$ -> yp_act2_lineno = $5 -> ya_lineno;
				free_action ($5);
			    }                  
	    		}

	|		SEQUENCE SizeConstraint OF Sctrl Action Control Type {
			    $$ = new_type (YP_SEQTYPE);
			    $$ -> yp_type = $7;
			    if ($4) {
				$$ -> yp_structname = $4 -> ys_structname;
				$$ -> yp_ptrname = $4 -> ys_ptrname;
				free_ys ($4);
			    }
			    if ($6) {
				$$ -> yp_control = $6;
				$$ -> yp_flags |= YP_CONTROLLED;
			    }
			    if ($5) {
				$$ -> yp_action3 = $5 -> ya_text;
				$$ -> yp_act3_lineno = $5 -> ya_lineno;
				free_action ($5);
			    }
			}
	;

SubtypeSpec:		LPAREN SubtypeAlternative SubtypeAlternativeList RPAREN
			{
			    ($$ = $2) -> ysub_next = $3;
			}
	;

SubtypeAlternative:	SubtypeValueSet { $$ = $1; }
	|		SubtypeConstraint { $$ = $1; }
	;

SubtypeAlternativeList:	BAR SubtypeAlternative SubtypeAlternativeList
			{
			    ($$ = $2) -> ysub_next = $3;
			}
	|		empty { $$ = NULL; }
	;

SubtypeValueSet:	Value
			{
			    ($$ = new_ysub (YS_VALUE)) -> ysub_value = $1;
			}
	|		ContainedSubType { $$ = new_ysub (YS_CONTSUBTYPE); }
	|		ValueRange { $$ = $1; }
	|		PermittedAlphabet { $$ = new_ysub (YS_PERMITALPHA); }
	;

SubtypeConstraint:	SizeConstraint { $$ = $1; }
	|		InnerTypeConstraint { $$ = new_ysub (YS_INNER); }
	;

ContainedSubType:	INCLUDES Type	;

ValueRange:		LowerEndPoint DOTDOT UpperEndpoint
			{
			    $$ = new_ysub (YS_VALUERANGE);
			    $$ -> ysub_lower = $1;
			    $$ -> ysub_upper = $3;
			}
	;

LowerEndPoint:		LowerEndValue { $$ = $1; }
	|		LowerEndValue LANGLE { ($$ = $1) -> ep_langle = 1; }
	;

UpperEndpoint:		UpperEndValue { $$ = $1; }
	|		LANGLE UpperEndValue { ($$ = $2) -> ep_langle = 1; }
	;

LowerEndValue:		Value { $$ = new_endp ($1); }
	|		MIN   { $$ = new_endp (NULLYV); }
	;

UpperEndValue:		Value { $$ = new_endp ($1); }
	|		MAX   { $$ = new_endp (NULLYV); }
	;

SizeConstraint:		SIZE SubtypeSpec
			{
			    ($$ = new_ysub (YS_SIZE)) -> ysub_subtype = $2;
			}
	;

PermittedAlphabet:	FROM SubtypeSpec
	;

InnerTypeConstraint:	WITH COMPONENT SingleTypeConstraint
	|		WITH COMPONENTS MultipleTypeConstraints
	;

SingleTypeConstraint:	SubtypeSpec { }
	;

MultipleTypeConstraints:FullSpecification
	|		PartialSpecification
	;

FullSpecification:	LBRACE TypeConstraints RBRACE
	;

PartialSpecification:	LBRACE DOTDOTDOT COMMA TypeConstraints RBRACE
	;

TypeConstraints:	NamedConstraint
	|		NamedConstraint COMMA TypeConstraints
	;

NamedConstraint:	NAME Constraint { }
        |		Constraint
	;

Constraint:		ValueConstraint
	|		PresenceConstraint
	|		empty
	;

ValueConstraint:	SubtypeSpec { }
	;

PresenceConstraint:	PRESENT
	|		ABSENT
	;

Sctrl:			SCTRL { $$ = new_ys ($1); }
	|		empty { $$ = NULLYS; }
	;
%%

#ifdef	HPUX
#define	uchar	Uchar
#endif
#include "lex.c"
