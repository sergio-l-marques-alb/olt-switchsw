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

#include "sr_conf.h"

#include <unistd.h>	/* for unlink() and chdir() */

#include <sys/types.h>  /* for mkdir() */

#include <sys/stat.h>  /* for mkdir() */

#include "sr_proto.h"
#include "sr_type.h"

#define MIBTOOLS_MAIN

#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "mibtools.h"
#include "mib_tree.h"

static char postmosyargs[1024];
static int currentlen = 0;
static void checkargstring(void);
static int cmp_index(void);
static void get_one_prim_type(struct OID_INFO *root);
static void calculate_family_numbers(struct OID_INFO *root, int first_family);

#ifdef SR_DEBUG
#include "diag.h"

LogBook *LogBk;

LogBook *
GetLogBook()
{
    return LogBk;
}
#endif /* SR_DEBUG */

static char *subtree_to_remove = NULL;

int
main(argc, argv)
    int             argc;
    char           *argv[];
				/* end of main */
{
    FILE           *fp;
    char           *outfile;
    char           *arg;
    struct OID_INFO *ptr;

#ifdef SR_DEBUG
    if ((LogBk = OpenLog()) == NULL) {
	fprintf(stderr, "warning: OpenLog() failed\n");
	exit(-1);
    }
#endif /* SR_DEBUG */

    /* initialize values */
    outfile = DEFAULT_BASE_STR;
    root = (struct OID_INFO *) NULL;
    oid_root = (struct OID_INFO *) NULL;
    sorted_root = (struct OID_INFO *) NULL;
    tc = NULL;
    index_table = NULL;

    /* initialize values for command line flags which do not default to 0 */
    undo = 1;
    cache = 1;
    k_init = 1;
    k_term = 1;
    use_const = 1;
    ansi = 1;		/* -ansi is now the default */
    no_cvs_keywords = 1;	/* -no_cvs is now the default */
    priority = 32;
    v_elimination_data = 1;
    small_v_get = 1;
    small_v_test = 1;
    new_k_wrappers = 1;
    type_table = 1;

    postmosy_config_file = NULL;
    postmosy_config_line = NULL;
    light_config_file = NULL;
    include_file = NULL;
    fpmerge = NULL;
    override_config_file = NULL;

    /* set up the string for output of command line args to postmosy */
    (void) memset(postmosyargs, 0, sizeof(postmosyargs));
    strcat(postmosyargs, " * ");
    currentlen += 3;

    /* prepare for argument processing */
    prog_name = argv[0];
    argc--;
    argv++;

    /* While flags are present, process them */
    while ((argc > 0) && (argv[0][0] == '-')) {
	argc--;
	arg = *argv++;

	if (strcmp(arg, "-o") == 0) {
	    if (argc > 0) {
		argc--;
		outfile = *argv++;
	        strcat(postmosyargs, "-o ");
	        strcat(postmosyargs, outfile);
		strcat(postmosyargs, " ");
	        currentlen += (4 + strlen(outfile));
	        checkargstring();
	    }
	    else
		usage();
	}
	else if (strcmp(arg, "-outdir") == 0) {
            /* place the output files in the specified directory */
            if (argc > 0) {
                char *outdir = *argv++;
                argc--;
                /* mkdir in case the directory does not already exist */
                mkdir(outdir, 0777);  
                if (chdir(outdir) != 0) {
                    fprintf(stderr, 
                            "unable to change directory to %s\n", outdir);
                    perror(NULL);
                    exit(1);
                }
            }
            else {
                usage();
            }
	}
#ifndef SR_NAA_TOOLS
#ifndef SR_EP_TOOLS
	else if (strcmp(arg, "-short") == 0) {
	    strcat(postmosyargs, "-short ");
	    currentlen += 7;
	    checkargstring();
	    short_names = 1;
	}
	else if (strcmp(arg, "-include") == 0) {
	    argc--;
	    if ((include_file = *argv++) == NULL) {
                usage();
        }
	    strcat(postmosyargs, "-include ");
	    strcat(postmosyargs, include_file);
	    strcat(postmosyargs, " ");
	    currentlen += (10 + strlen(include_file));
	    checkargstring();
	}
#endif /* !SR_EP_TOOLS */
	else if (strcmp(arg, "-light") == 0) {
	    argc--;
	    light_config_file = *argv++;
	    strcat(postmosyargs, "-light ");
	    currentlen += 7;
	    checkargstring();
#ifdef SR_AGENT_TOOLS
	    snmpmibh = 1;
#endif /* SR_AGENT_TOOLS */
	}
#else /* SR_NAA_TOOLS */
	else if (strcmp(arg, "-static") == 0) {
            if (naa_dynamic) {
                fprintf(stderr, "error, cannot use -static with -dynamic\n");
                exit(-1);
            }
	    strcat(postmosyargs, "-static ");
	    currentlen += 8;
	    checkargstring();
	    naa_static = 1;
	}
	else if (strcmp(arg, "-dynamic") == 0) {
            if (naa_static) {
                fprintf(stderr, "error, cannot use -dynamic with -static\n");
                exit(-1);
            }
	    strcat(postmosyargs, "-dynamic ");
	    currentlen += 9;
	    checkargstring();
	    naa_dynamic = 1;
	}
#endif /* SR_NAA_TOOLS */
	else if (strcmp(arg, "-f") == 0) {
	    argc--;
	    if ((postmosy_config_file = *argv++) == NULL) {
                usage();
        }
	    strcat(postmosyargs, "-f ");
	    strcat(postmosyargs, postmosy_config_file);
	    strcat(postmosyargs, " ");
	    currentlen += (4 + strlen(postmosy_config_file));
	    checkargstring();
            if (postmosy_config_line) {
                fprintf(stderr, "error, cannot use -f with -c\n");
                exit(-1);
            }
	}
#ifndef SR_NAA_TOOLS
	else if (strcmp(arg, "-merge_report") == 0) {
	    argc--;
            fpmerge = fopen(*argv++, "w");
        }
#endif /* !SR_NAA_TOOLS */
	else if (strcmp(arg, "-c") == 0) {
            if (postmosy_config_line) {
                fprintf(stderr, "warning: multiple -c options not supported, only the last one will be used\n");
            }
	    argc--;
	        if ((postmosy_config_line = *argv++) == NULL) {
                 usage();
            }
            if (postmosy_config_file) {
                fprintf(stderr, "error, cannot use -f with -c\n");
                exit(-1);
            }
	    strcat(postmosyargs, "-c ");
	    strcat(postmosyargs, postmosy_config_line);
	    strcat(postmosyargs, " ");
	    currentlen += 4;
	    checkargstring();
        }
#ifndef SR_NAA_TOOLS
	else if (strncmp(arg, "-remove_subtree", 7) == 0) {
            if (subtree_to_remove) {
                fprintf(stderr, "warning: multiple -remove_subtree options not supported, only the last one will be used\n");
            }
	    argc--;
	    if ((subtree_to_remove = *argv++) == NULL) {
                usage();
        }
        remove_subtree = 1;
	    strcat(postmosyargs, "-remove_subtree ");
	    strcat(postmosyargs, subtree_to_remove);
	    strcat(postmosyargs, " ");
	    currentlen += 15;
	    currentlen += strlen(subtree_to_remove);
	    checkargstring();
	}
	else if (strcmp(arg, "-export_st") == 0) {
	    strcat(postmosyargs, "-export_st ");
	    currentlen += 11;
	    checkargstring();
	    export_st = 1;
	}
	else if (strcmp(arg, "-intel") == 0) {
	    strcat(postmosyargs, "-intel ");
	    currentlen += 7;
	    checkargstring();
	    intel = 1;
	}
#endif /* !SR_NAA_TOOLS */
	else if (strcmp(arg, "-i") == 0) {
	    strcat(postmosyargs, "-i ");
	    currentlen += 3;
	    checkargstring();
	    ignore_nonfatal_errors = 1;
	}
#ifndef SR_NAA_TOOLS
	else if (strncmp(arg, "-mergeinfo", 6) == 0) {
	    inputinfo = 1;
	    mergeinfo = 1;
	    strcat(postmosyargs, "-mergeinfo (implies -inputinfo) ");
	    currentlen += 31;
	    checkargstring();
	}
	else if (strncmp(arg, "-inputinfo", 6) == 0) {
	    strcat(postmosyargs, "-inputinfo ");
	    currentlen += 11;
	    checkargstring();
	    inputinfo = 1;
	}
	else if (strcmp(arg, "-mibtree") == 0) {
	    strcat(postmosyargs, "-mibtree ");
	    currentlen += 9;
	    checkargstring();
	    mibtree = 1;
	}
	else if (strncmp(arg, "-mgroidlist", 8 ) == 0) {
	    strcat(postmosyargs, "-mgroidlist ");
	    currentlen += 6;
	    checkargstring();
	    mgroidl = 1;
	}
	else if (strcmp(arg, "-java") == 0) {
            fprintf(stderr, "Error: -java option not supported in this version of postmosy\n");
            exit(-1);
	}
#ifdef CIAGENT_MONITOR
	else if (strncmp(arg, "-ciagentmonitor", 4) == 0) {
	    strcat(postmosyargs, "-ciagentmonitor ");
	    currentlen += 16;
	    checkargstring();
	    ciagentmonitor = 1;
	}
	else if (strcmp(arg, "-rtview") == 0) {
	    strcat(postmosyargs, "-rtview ");
	    currentlen += 8;
	    checkargstring();
	    rtview = 1;
	}
#endif /* CIAGENT_MONITOR */
#if (defined(SR_AGENT_TOOLS) || defined(SR_EP_TOOLS))
      else if (strcmp(arg, "-snmpmibh") == 0) {
          strcat(postmosyargs, "-snmpmibh ");
          currentlen += 10;
          checkargstring();
          snmpmibh = 1;
      }
#endif	/* (defined(SR_AGENT_TOOLS) || defined(SR_EP_TOOLS)) */
#ifdef SR_AGENT_TOOLS
	else if (strcmp(arg, "-capabilities") == 0) {
            /* print the capabilities of this version of postmosy and exit */
            printf("C ");
            printf("\n");
            
            exit(0);
        }
	else if (strcmp(arg, "-mgrtool") == 0) {
	    strcat(postmosyargs, "-mgrtool ");
	    currentlen += 9;
	    checkargstring();
	    mgrtool = 1;
	}
	else if (strcmp(arg, "-compute_nominator") == 0) {
	    strcat(postmosyargs, "-compute_nominator ");
	    currentlen += 19;
	    checkargstring();
	    compute_nominator = 1;
	    cache = 0;
	}
	else if (strncmp(arg, "-ind", 4) == 0) {
	    strcat(postmosyargs, "-ind ");
	    currentlen += 5;
	    checkargstring();
	    individual = 1;
	}
	else if (strncmp(arg, "-one_based", 7) == 0) {
	    strcat(postmosyargs, "-one_based ");
	    currentlen += 11;
	    checkargstring();
	    one_based = 1;
	}
	else if (strncmp(arg, "-type", 5) == 0) {
	    strcat(postmosyargs, "-type ");
	    currentlen += 6;
	    checkargstring();
	    xxx_type = 1;
	}
	else if (strcmp(arg, "-vpl") == 0) {
	    strcat(postmosyargs, "-vpl ");
	    currentlen += 5;
	    checkargstring();
	    var_param_list = 1;
	}
	else if (strncmp(arg, "-userpart", 4) == 0) {
	    strcat(postmosyargs, "-userpart ");
	    currentlen += 10;
	    checkargstring();
	    userpart = 1;
	}
	else if (strcmp(arg, "-agent") == 0) {
	    strcat(postmosyargs, "-agent ");
	    currentlen += 7;
	    checkargstring();
            agent = 1;
	}
	else if (strncmp(arg, "-traps", 5) == 0) {
	    strcat(postmosyargs, "-traps ");
	    currentlen += 7;
	    checkargstring();
            traps = 1;
	}
	else if (strcmp(arg, "-no_undo") == 0) {
	    strcat(postmosyargs, "-no_undo ");
	    currentlen += 9;
	    checkargstring();
	    undo = 0;
	}
	else if (strncmp(arg, "-stubs_only", 5) == 0) {
	    strcat(postmosyargs, "-stubs_only ");
	    currentlen += 12;
	    checkargstring();
	    stubs_only = 1;
	    agent = 1;
	}
	else if (strncmp(arg, "-v_stubs_only", 3) == 0) {
	    strcat(postmosyargs, "-v_stubs_only ");
	    currentlen += 14;
	    checkargstring();
	    stubs_only = 1;
	    v_stubs_only = 1;
	    agent = 1;
	}
	else if (strcmp(arg, "-s") == 0) {
	    strcat(postmosyargs, "-s ");
	    currentlen += 3;
	    checkargstring();
	    silent = 1;
	}
	else if (strcmp(arg, "-silent") == 0) {
	    strcat(postmosyargs, "-silent ");
	    currentlen += 8;
	    checkargstring();
	    silent = 1;
	}
	else if (strncmp(arg, "-long", 5) == 0) {
	    strcat(postmosyargs, "-long ");
	    currentlen += 6;
	    checkargstring();
	    D_long = 1;
	}
	else if (strncmp(arg, "-nocache", 4) == 0) {
	    strcat(postmosyargs, "-nocache ");
	    currentlen += 9;
	    checkargstring();
	    cache = 0;
	}
	else if (strncmp(arg, "-cache", 5) == 0) {
	    strcat(postmosyargs, "-cache ");
	    currentlen += 7;
	    checkargstring();
	    cache = 1;
	}
	else if (strcmp(arg, "-no_cvs") == 0) {
	    strcat(postmosyargs, "-no_cvs ");
	    currentlen += 8;
	    checkargstring();
            fprintf(stderr, "%s\n%s\n",
		    "NOTE: -no_cvs is now the default.",
		    "Turn CVS keywords on with the -cvs option.");
	}
	else if (strcmp(arg, "-cvs") == 0) {
	    strcat(postmosyargs, "-cvs ");
	    currentlen += 5;
	    checkargstring();
	    no_cvs_keywords = 0;
	}
	else if (strcmp(arg, "-defval_c_string") == 0) {
	    strcat(postmosyargs, "-defval_c_string ");
	    currentlen += 17;
	    checkargstring();
	    defval_c_string = 1;
	}
	else if (strcmp(arg, "-fix_rowstatus_defval") == 0) {
	    strcat(postmosyargs, "-fix_rowstatus_defval ");
	    currentlen += 22;
	    checkargstring();
	    fix_rowstatus_defval = 1;
	}
	else if (strncmp(arg, "-per_file_init", 5) == 0) {
	    strcat(postmosyargs, "-per_file_init ");
	    currentlen += 15;
	    checkargstring();
	    per_file_init = 1;
	}
	else if (strncmp(arg, "-priority", 3) == 0) {
            char priority_string[16];
	    strcat(postmosyargs, "-priority ");
	    currentlen += 10;
	    checkargstring();
	    if (argc == 0) {
		usage();
	    }
	    argc--;
	    priority = atoi(*argv++);
	    if (priority < 0) {
		priority = 0;
	    }
	    else if (priority > 255) {
		priority = 255;
	    }
            sprintf(priority_string, "%d ", priority);
	    strcat(postmosyargs, priority_string);
	    currentlen += strlen(priority_string);
	}
	else if (strcmp(arg, "-read-create") == 0) {
	    strcat(postmosyargs, "-read-create ");
	    currentlen += 13;
	    checkargstring();
	    xxx_read_create = 1;
	}
	else if (strcmp(arg, "-const") == 0) {
	    strcat(postmosyargs, "-const ");
	    currentlen += 7;
	    checkargstring();
            fprintf(stderr, "NOTE: -const is now the default.\nTurn off with -no_const\n");
	}
	else if (strcmp(arg, "-no_const") == 0) {
	    strcat(postmosyargs, "-no_const ");
	    currentlen += 10;
	    checkargstring();
	    use_const = 0;
	}
	else if (strcmp(arg, "-make") == 0) {
	    strcat(postmosyargs, "-make ");
	    currentlen += 6;
	    checkargstring();
	    make = 1;
	}
#endif /* SR_AGENT_TOOLS */
#if (defined(SR_AGENT_TOOLS) || defined(SR_EP_TOOLS))
	else if (strcmp(arg, "-ansi") == 0) {
	    fprintf(stderr, "Option -ansi is now the default.  Use -kr for K&R C.\n");
	}
	else if (strcmp(arg, "-kr") == 0) {
	    strcat(postmosyargs, "-kr ");
	    currentlen += 3;
	    checkargstring();
	    ansi = 0;
	}
#endif	/* (defined(SR_AGENT_TOOLS) || defined(SR_EP_TOOLS)) */
#ifdef SR_AGENT_TOOLS
	else if (strcmp(arg, "-nodefval") == 0) {
	    strcat(postmosyargs, "-nodefval ");
	    currentlen += 10;
	    checkargstring();
	    no_defval = 1;
	}
	else if (strcmp(arg, "-row_status") == 0) {
	    strcat(postmosyargs, "-row_status ");
	    currentlen += 12;
	    checkargstring();
	    row_status = 1;
	}
	else if (strcmp(arg, "-old_row_timer") == 0) {
	    strcat(postmosyargs, "-old_row_timer ");
	    currentlen += 15;
	    checkargstring();
	    old_row_timer = 1;
	}
	else if (strcmp(arg, "-storage_type") == 0) {
	    strcat(postmosyargs, "-storage_type ");
	    currentlen += 14;
	    checkargstring();
	    storage_type = 1;
	}
	else if (strcmp(arg, "-test_and_incr") == 0) {
	    strcat(postmosyargs, "-test_and_incr ");
	    currentlen += 15;
	    checkargstring();
	    test_and_incr = 1;
	}
	else if (strcmp(arg, "-argsused") == 0) {
	    strcat(postmosyargs, "-argsused ");
	    currentlen += 10;
	    checkargstring();
	    argsused = 1;
	}
	else if (strcmp(arg, "-check_context") == 0) {
	    strcat(postmosyargs, "-check_context ");
	    currentlen += 15;
	    checkargstring();
	    check_context = 1;
	    if (v_elimination_data || small_v_get || small_v_test ||
		new_k_stubs || new_k_wrappers || no_v_get) {
                fprintf(stderr, "\
Warning, -check_context can not be combined with any of the following:\n\
\n\
   -no_v         -small_v         -medium_v       -apo\n\
   -no_v_get     -small_v_get     -new_k_stubs    -aggregate\n\
   -no_v_test    -small_v_test    -adapt_old_k\n\
\n\
If you did not supply any of the above arguments, then use -classic_v\n\
BEFORE -check_context to turn off this warning message.\n\
\n");
                exit(-1);
            }
	}
	else if (strcmp(arg, "-parser") == 0) {
	    strcat(postmosyargs, "-parser ");
	    currentlen += 8;
	    checkargstring();
	    parser = 1;
	}
	else if (strcmp(arg, "-comments") == 0) {
	    strcat(postmosyargs, "-comments ");
	    currentlen += 10;
	    checkargstring();
	    comments = 1;
	}
	else if (strncmp(arg, "-noinit", 4) == 0) {
	    strcat(postmosyargs, "-noinit ");
	    currentlen += 8;
	    checkargstring();
	    k_init = 0;
	    k_term = 0;
	}
	else if (strncmp(arg, "-init", 4) == 0) {
	    strcat(postmosyargs, "-init ");
	    currentlen += 6;
	    checkargstring();
	    k_init = 1;
	    k_term = 1;
	}
	else if (strncmp(arg, "-instance ", 4) == 0) {
	    strcat(postmosyargs, "-instance ");
	    currentlen += 10;
	    checkargstring();
	    instance = 1;
	}
	else if (strcmp(arg, "-late") == 0) {
	    strcat(postmosyargs, "-late ");
	    currentlen += 6;
	    checkargstring();
	    late = 1;
	}
	else if ((strncmp(arg, "-table_lookup", 6) == 0) ||
		 (strncmp(arg, "-tl", 3) == 0)) {
	    strcat(postmosyargs, "-table_lookup ");
	    currentlen += 14;
	    checkargstring();
	    table_lookup = 1;
	}
	else if (strcmp(arg, "-search_table") == 0) {
	    strcat(postmosyargs, "-search_table (implies -row_status) ");
	    currentlen += 35;
	    checkargstring();
	    search_table = 1;
            row_status = 1;
	}
	else if (strcmp(arg, "-no_row_aggs") == 0) {
            fprintf(stderr, "%s not supported in this version of postmosy\n", arg);
            exit(1);
        } 
	else if (strcmp(arg, "-no_table_aggs") == 0) {
            fprintf(stderr, "%s not supported in this version of postmosy\n", arg);
            exit(1);
        }
	else if ( (strncmp(arg, "-apo", 4) == 0) ||
                  (strncmp(arg, "-aggregate", 4) == 0) ) {
            fprintf(stderr, "%s not supported in this version of postmosy\n", arg);
            exit(1);

	}
	else if (strcmp(arg, "-os") == 0) {
            fprintf(stderr, "The -os flag is no longer supported\n");
            fprintf(stderr, "It has been replaced by -small_v\n");
            exit(-1);
	}
	else if (strcmp(arg, "-c++") == 0) {
	    strcat(postmosyargs, "-c++ ");
	    currentlen += 5;
	    checkargstring();
	    cplusplus = 1;
	}
	else if (strcmp(arg, "-no_v") == 0) {
	    strcat(postmosyargs, "-no_v ");
	    currentlen += 6;
	    checkargstring();
	    v_elimination_data = 1;
	    no_v_get = 1;
	    no_v_test = 1;
	    new_k_stubs = 1;
            new_k_wrappers = 0;
            if (check_context) {
                fprintf(stderr, "error, cannot use -check_context with -no_v\n");
                exit(-1);
            }
	}
	else if (strcmp(arg, "-no_v_get") == 0) {
	    strcat(postmosyargs, "-no_v_get ");
	    currentlen += 10;
	    checkargstring();
	    v_elimination_data = 1;
	    no_v_get = 1;
	    new_k_stubs = 1;
            new_k_wrappers = 0;
            if (check_context) {
                fprintf(stderr, "error, cannot use -check_context with -no_v_get\n");
                exit(-1);
            }
	}
	else if (strncmp(arg, "-no_v_test", 5) == 0) {
	    strcat(postmosyargs, "-no_v_test ");
	    currentlen += 11;
	    checkargstring();
	    v_elimination_data = 1;
	    no_v_test = 1;
	    new_k_stubs = 1;
            new_k_wrappers = 0;
            if (check_context) {
                fprintf(stderr, "error, cannot use -check_context with -no_v_test\n");
                exit(-1);
            }
	}
	else if (strcmp(arg, "-small_v") == 0) {
	    strcat(postmosyargs, "-small_v ");
	    currentlen += 9;
	    checkargstring();
	    v_elimination_data = 1;
	    small_v_get = 1;
	    small_v_test = 1;
            if (new_k_stubs == 0) {
                new_k_wrappers = 1;
            }
            if (check_context) {
                fprintf(stderr, "error, cannot use -check_context with -small_v\n");
                exit(-1);
            }
	}
	else if (strcmp(arg, "-small_v_get") == 0) {
	    strcat(postmosyargs, "-small_v_get ");
	    currentlen += 13;
	    checkargstring();
	    v_elimination_data = 1;
	    small_v_get = 1;
            if (new_k_stubs == 0) {
                new_k_wrappers = 1;
            }
            if (check_context) {
                fprintf(stderr, "error, cannot use -check_context with -small_v_get\n");
                exit(-1);
            }
	}
	else if (strcmp(arg, "-small_v_test") == 0) {
	    strcat(postmosyargs, "-small_v_test ");
	    currentlen += 14;
	    checkargstring();
	    v_elimination_data = 1;
	    small_v_test = 1;
            if (new_k_stubs == 0) {
                new_k_wrappers = 1;
            }
            if (check_context) {
                fprintf(stderr, "error, cannot use -check_context with -small_v_test\n");
                exit(-1);
            }
	}
        else if (strcmp(arg, "-medium_v") == 0) {
            strcat(postmosyargs, "-medium_v ");
            currentlen += 10;
            checkargstring();
            v_elimination_data = 1;
            medium_v = 1;
            if (check_context) {
                fprintf(stderr, "error, cannot use -check_context with -medium_v\n");
                exit(-1);
            }
        }
	else if (strcmp(arg, "-small_v_uses_global") == 0) {
	    strcat(postmosyargs, "-small_v_uses_global ");
	    currentlen += 21;
	    checkargstring();
            small_v_uses_global = 1;
	}
	else if (strcmp(arg, "-adapt_old_k") == 0) {
	    strcat(postmosyargs, "-adapt_old_k ");
	    currentlen += 13;
	    checkargstring();
            if (new_k_stubs == 0) {
                new_k_wrappers = 1;
            }
            if (check_context) {
                fprintf(stderr, "error, cannot use -check_context with -adapt_old_k\n");
                exit(-1);
            }
	}
	else if (strncmp(arg, "-classic_v", 8) == 0) {
	    strcat(postmosyargs, "-classic_v ");
	    currentlen += 11;
	    checkargstring();
	    v_elimination_data = 0;
	    small_v_get = 0;
	    small_v_test = 0;
	    new_k_stubs = 0;
            new_k_wrappers = 0;
	    no_v_get = 0;
	}
	else if (strcmp(arg, "-override_v") == 0) {
	    argc--;
	    override_config_file = *argv++;
	    strcat(postmosyargs, "-override_v ");
	    strcat(postmosyargs, override_config_file);
	    strcat(postmosyargs, " ");
	    currentlen += (13 + strlen(override_config_file));
	    checkargstring();
	}
	else if (strncmp(arg, "-new_k_stubs", 6) == 0) {
	    strcat(postmosyargs, "-new_k_stubs ");
	    currentlen += 13;
	    checkargstring();
	    new_k_stubs = 1;
            new_k_wrappers = 0;
            if (check_context) {
                fprintf(stderr, "error, cannot use -check_context with -new_k_stubs\n");
                exit(-1);
            }
	}
	else if (strcmp(arg, "-no_type_table") == 0) {
	    strcat(postmosyargs, "-no_type_table ");
	    currentlen += 15;
	    checkargstring();
	    type_table = 0;
	}
	else if (strncmp(arg, "-offset", 4) == 0) {
	    strcat(postmosyargs, "-offset ");
	    currentlen += 8;
	    checkargstring();
	    offset = 1;
	}
	else if (strcmp(arg, "-separate_type_file") == 0) {
	    strcat(postmosyargs, "-separate_type_file ");
	    currentlen += 20;
	    checkargstring();
	    separate_type_file = 1;
	}
	else if (strcmp(arg, "-snmpoid_only") == 0) {
	    strcat(postmosyargs, "-snmpoid_only ");
	    currentlen += 14;
	    checkargstring();
	    snmpoid_only = 1;
	}
	else if (strcmp(arg, "-no_terminate") == 0) {
	    strcat(postmosyargs, "-no_terminate ");
	    currentlen += 14;
	    checkargstring();
	    k_init = 1;
	    k_term = 0;
	}
	else if (strcmp(arg, "-oidl") == 0) {
	    strcat(postmosyargs, "-oidl ");
	    currentlen += 6;
	    checkargstring();
	    oidl = 1;
	}
	else if (strcmp(arg, "-localconnect") == 0) {
	    strcat(postmosyargs, "-localconnect ");
	    currentlen += 14;
	    checkargstring();
	    localconnect = 1;
	    per_file_init = 1;
	    oidl = 1;
	}
#endif /* SR_AGENT_TOOLS */
#ifdef SR_EP_TOOLS
        else if (strcmp(arg, "-snmpinfo") == 0) {
            strcat(postmosyargs, "-snmpinfo ");
            currentlen += 10;
            checkargstring();
            snmpinfo = 1;
        }
        else if (strcmp(arg, "-build_varbinds") == 0) {
            strcat(postmosyargs, "-build_varbinds ");
            currentlen += 16;
            checkargstring();
            build_varbinds = 1;
        }
	else if (strcmp(arg, "-agent") == 0) {
	    /* generate #include lines for mibout.h as with the agent */
	    strcat(postmosyargs, "-agent ");
	    currentlen += 7;
	    checkargstring();
            agent = 1;
	}
#endif /* SR_EP_TOOLS */
#endif /* !SR_NAA_TOOLS */
	else {
            fprintf(stderr, "Error: unrecognized option: %s\n", arg);
	    usage();
	}			/* end if  */

    }				/* end of while loop */

    add_init_oi();

    if (argc == 0) {		/* input is stdin */
        if (!remove_subtree) {
	    if (inputinfo) {
	        input_snmpinfo_file(stdin);
	    } else {
	        input_file(stdin);
            }
        }
    }
    else {			/* got possibly multiple files to process */
	while (argc--) {
	    if ((fp = fopen(arg = *argv++, "r")) == NULL) {
		fprintf(stderr, "open for read failed: %s\n", arg);
		exit(-1);
	    }
            if (!remove_subtree) {
	        if (inputinfo) {
	            input_snmpinfo_file(fp);
	        } else {
	            input_file(fp);
                }
            }
	    fclose(fp);
	}			/* end of while */
    }				/* end of argc != 0 */
    if (mergeinfo || remove_subtree) {
        char *temp;

        if ((temp = (char *) getenv("SR_MGR_CONF_DIR")) == NULL) {
            temp = SR_MGR_CONF_FILE_PREFIX;
        }
	fp = open_file_read_only(temp, "/snmpinfo.dat");
	input_snmpinfo_file(fp);
        fclose(fp);
    }

    /*
     * trace back through "parent" references to flesh out OID num.num.num..
     * format
     */

    process_data();

    /*
     * Check for duplicate name with different OID (severe error).
     */
    check_names();

    /* replace - with _ in names */
    for (ptr = root; ptr != NULL; ptr = ptr->next) {
        remove_hyphens(ptr->name);
        remove_hyphens(ptr->oid_pre_str);
    }

    /*
     * Now sort by OID.  Send warning but continue if duplicate OID with
     * different name (tell which comes first).  Silently drop duplicate
     * name/OID pairs.
     */
    sort_data();

    /* find the primitive types and enumerations for textual conventions */
    get_prim_type();

    /*
     * build the mib table for use by MakeOIDFromDot
     */
    build_mib_table();

    if (!inputinfo && !remove_subtree) {
        /*
         * group nodes by family
         */
        group_nodes_by_family();

        /*
         * fill in the index and augments tables
         */
        if ((process_index_table()) < 0) {
	    if (agent) {
	        printf("WARNING: ");
	        printf("don't know name of index, ");
	        printf("method routine stubs cannot be created\n");
	        agent = 0;
	    }
        }

        process_augments_table();

    }

    /*
     * shorten names for compilers with short limit on variable name lengths
     * if -short was given on the command line
     */
    if (short_names) {
	shorten_names();
    }

    /*
     * make sure there are no conflicts in the index clauses.
     * this is primarily useful when using the -merge option.
     */
    cmp_index();

    /*
     * If there are any merge errors, do not write output files
     */
    if ((merge_errors > 0) && !ignore_nonfatal_errors) {
        fprintf(stderr, "\nMerge Failed\n");
        if (fpmerge) fprintf(fpmerge, "\nMerge Failed\n");
        if (fpmerge) fclose(fpmerge);
        return 2;
    }

    /*
     * write the output files here
     */
    output_multiple_files(outfile);

    /*
     * if there are merge warnings return 1, otherwise, return 0
     */
    if (fpmerge) fclose(fpmerge);
    if (agent_errors) {
        return 1;
    }
    if (inputinfo && merge_warnings > 0 && !ignore_nonfatal_errors) {
        return 1;
    } else {
        return 0;
    }
}

void
usage()
{
    fprintf(stderr, "usage:\n");
#ifndef SR_NAA_TOOLS
    fprintf(stderr, "%s [options] input-files...\n", prog_name);
#else /* SR_NAA_TOOLS */
    fprintf(stderr, "%s [-static | -dynamic] [other options] input-files...\n", prog_name);
#endif /* SR_NAA_TOOLS */
    fprintf(stderr, "\n");
    fprintf(stderr, "example%% %s *.def\n\n", prog_name);

    exit(-1);
}				/* end of usage */

int
isAggregateType(char *oid_type)
{
    if (oid_type == NULL) {
        return 0;
    }
    if (strcmp(oid_type, "Aggregate") == 0) {
        return 1;
    }
    return 0;
}


#define MAXBUFFSIZE 2048 
/* read input files */
int
input_file(fp)
    FILE           *fp;
{
    char            input_buffer[MAXBUFFSIZE];
    char            input_name[128];
    char            input_oid_str[128];
    char            input_type[128];
    char            input_access_type[128];
    char            input_aug[128];
    char            range_min[16], range_max[16];
    int             num_fields, found;
    struct TC      *temp_tc;
    char           *temp;
    long	    number;

    while (fgets(input_buffer, sizeof(input_buffer), fp) != NULL) {

	/*
	 * put this in so that we can deal with DOS crlf on other
	 * OSs
	 */
	if ( (strlen(input_buffer) > 1) && 
             (input_buffer[strlen(input_buffer) - 2] == '\r')) {
	  static int done = 0;

	  if (!done) {
	    fprintf(stderr, "DOS-form input file!\n");
	    done = 1;
          }
	  input_buffer[strlen(input_buffer) - 2] = '\n';
	}

	/* for backward compatibility with 7.0 mosy and v1 SMIC */
	if (input_buffer[0] == '*') {
	    old_get_enum(input_buffer, fp);
	}
	/* end backward compatibility */

	if ((input_buffer[0] != '-') &&	/* -- means comment */
	    (input_buffer[0] != '\n') &&
	    (input_buffer[0] != '*')) {

	    if ((input_buffer[0] == '%') && (input_buffer[1] == 'n')) {
		continue;
	    }
	    if ((input_buffer[0] == '%') && (input_buffer[1] == 'e')) {
		if (input_buffer[2] == 'v') {
		    get_enum(input_buffer);
		}
		else if (input_buffer[2] == 's') {
		    get_enum(input_buffer);
		}
		else if (input_buffer[2] == 'i') {
		    if (sscanf(input_buffer, "%%ei %s",
			       input_name) != 1) {
			fprintf(stderr, "%s: bad input line: %s\n", prog_name, input_buffer);
		    }
		    temp = (char *) strchr(input_buffer, '"');
                    if (temp == NULL) {
                       fprintf(stderr, "%s: bad input line: %s\n", prog_name, input_buffer);
                       exit(-1);
                     }
		    build_index_table(input_name, temp);
		}
		else if (input_buffer[2] == 'a') {
		    if (sscanf(input_buffer, "%%ea %s %s",
			       input_name, input_aug) != 2) {
			fprintf(stderr, "%s: bad input line: %s\n", prog_name, input_buffer);
			exit(-1);
		    }
		    build_augments_table(input_name, input_aug);
		}
		else if (input_buffer[2] == 'r') {
		    if (sscanf(input_buffer, "%%er %s %s %s", input_name, range_min, range_max) != 3) {
			fprintf(stderr, "%s: bad input line: %s\n", prog_name, input_buffer);
			exit(-1);
		    }
		    add_size_range(input_name, range_min, range_max);
		}
		else {
		    fprintf(stderr, "%s: bad input line: %s\n", prog_name, input_buffer);
		    exit(-1);
		}
		continue;
	    }
	    if (strncmp(input_buffer, "%defval", strlen("%defval")) == 0) {
                if (no_defval) {
		    continue;
                }
		if (sscanf(input_buffer, "%%defval %s", input_name) != 1) {
		    fprintf(stderr, "%s: bad input line: %s\n", 
			prog_name, input_buffer);
		    exit(-1);
                }
                temp = (char *) strchr(input_buffer, '\n');
                if (temp) {
		    *temp = '\0';
                }
                temp = input_buffer;
                temp += strlen("%defval");
                while (*temp == ' ') { temp++; }
                temp += strlen(input_name);
                while (*temp == ' ') { temp++; }
                add_defval(input_name, temp);
                continue;
	    }
	    if (strncmp(input_buffer, "%trap", strlen("%trap")) == 0) {
		if (sscanf(input_buffer, "%%trap %s %ld %s", 
			input_name, &number,  input_oid_str) != 3) {
		    fprintf(stderr, "%s: bad input line: %s\n", 
			prog_name, input_buffer);
		    exit(-1);
		}
		temp = (char *) strchr(input_buffer, '{');
		add_trap(input_name, input_oid_str, number, temp);
                continue;
            }
	    if ((num_fields =
		 sscanf(input_buffer, "%s %s %s %s", input_name,
			input_oid_str, input_type, input_access_type))
		!= 2 && num_fields != 4) {
		fprintf(stderr, "%s: bad input line: %s\n",
			prog_name, input_buffer);
		exit(-1);
	    }
	    else if (num_fields == 2) {
		add_oi(input_name, input_oid_str, NULL, NULL);
	    }
	    else {
		found = 0;
		if ((strcmp(input_name, "%tc") == 0) ||
		    (strcmp(input_name, "%ta") == 0)) {
		    /*
		     * see if this one is already in the list
		     */
		    temp_tc = tc;
		    while (temp_tc) {
			if (strcmp(input_oid_str, temp_tc->name) == 0) {
			    if (strcmp(temp_tc->type, input_type) == 0) {
				/*
				 * already in the table, just return
				 */
				found = 1;
			    } else if ( 
                                 ((strcmp(temp_tc->type, "Integer32") == 0) &&
			         (strcmp(input_type, "INTEGER") == 0)) ||
			         ((strcmp(input_type, "Integer32") == 0) &&
			         (strcmp(temp_tc->type, "INTEGER") == 0))
                                ) {
				/*
				 * INTEGER and Integer32 are the same
				 */
				found = 1;
			    } else if ( 
                                ((strcmp(temp_tc->type, "DisplayString") == 0)&&
			         (strcmp(input_type, "OctetString") == 0)) ||
			         ((strcmp(input_type, "DisplayString") == 0) &&
			         (strcmp(temp_tc->type, "OctetString") == 0))
                                ) {
				/*
				 * DisplayString and OctetString are the same
				 */
                                temp_tc->type = "OctetString";
				found = 1;
			    }
			    else {
				/*
				 * different value already in the table --
				 * quit
				 */
				fprintf(stderr, "error: duplicate textual conventions with ");
				fprintf(stderr, "different types:\n   %s %s\n   %s %s\n",
					temp_tc->name, temp_tc->type, temp_tc->name, input_type);
				exit(-1);
			    }
                        }
			temp_tc = temp_tc->next;
		    }		/* end while */

		    if (!found) {
			/*
			 * create a node for the linked list
			 */
			temp_tc = (struct TC *) malloc(sizeof(struct TC));
			temp_tc->name = (char *) malloc(sizeof(input_oid_str) + 1);
			temp_tc->type = (char *) malloc(sizeof(input_type) + 1);
			temp_tc->hint = (char *) malloc(sizeof(input_access_type) + 1);
			if (!temp_tc->name || !temp_tc->type || !temp_tc->hint) {
			    fprintf(stderr, "malloc error\n");
			    exit(-1);
			}
			/* fill in the node */
			strcpy(temp_tc->name, input_oid_str);
			strcpy(temp_tc->type, input_type);
			strcpy(temp_tc->hint, input_access_type);
			temp_tc->enumer = NULL;
			temp_tc->size = NULL;
			/* put the node into the list */
			temp_tc->next = tc;
			tc = temp_tc;
			temp_tc = tc;
		    }
		}
		else {
		    add_oi(input_name, input_oid_str, input_type, input_access_type);
		}
	    }
	}			/* end of not a comment */
    }				/* end of while (fgets...) */
    return 1;
}				/* end of input_file */

/* read input files in the format of snmpinfo.dat */
int
input_snmpinfo_file(fp)
    FILE           *fp;
{
    char            input_buffer[MAXBUFFSIZE];
    char            input_name[128];
    char            input_oid_str[128];
    char            input_type[128];
    char            input_access_type[128];
    int             num_fields;
    int             j;

    while (fgets(input_buffer, sizeof(input_buffer), fp) != NULL) {

        /*
         * deal with DOS crlf on other OSs
         */
        if ( (strlen(input_buffer) > 1) &&
             (input_buffer[strlen(input_buffer) - 2] == '\r')) {
          input_buffer[strlen(input_buffer) - 2] = '\n';
        }

	if ((input_buffer[0] != '#') &&	/* # means comment */
	    (input_buffer[0] != '\n') &&
	    (input_buffer[0] != '*')) {

            /* see if this line C style is a comment */
            j = 0;
            while (input_buffer[j] == ' ' || input_buffer[j] == '\t') {
                j++;
	    }
            if (input_buffer[j] == '/' && input_buffer[j + 1] == '*') {
                while (!(input_buffer[j] == '*' && input_buffer[j + 1] == '/')) {
                    if (input_buffer[j] == '\n' || input_buffer[j] == '\0') {
                        fgets(input_buffer, sizeof(input_buffer), fp);
                        j = 0;
                        /*
                         * deal with DOS crlf on other OSs
                         */
                        if ( (strlen(input_buffer) > 1) &&
                             (input_buffer[strlen(input_buffer) - 2] == '\r')) {
                          input_buffer[strlen(input_buffer) - 2] = '\n';
                        }
                    }
                    else {
                        j++;
		    }
                }
                continue;
            }

	    if (input_buffer[0] == '(') {
		get_snmpinfo_enum(fp, input_name);
		continue;
	    } 
	    if (input_buffer[0] == '{') {
		get_snmpinfo_index(fp, input_name);
		continue;
	    } 
	    if ((num_fields =
		 sscanf(input_buffer, "%s %s %s %s", input_name,
			input_oid_str, input_type, input_access_type))
		!= 3 && num_fields != 4) {
		fprintf(stderr, "%s: bad input line: %s\n",
			prog_name, input_buffer);
		exit(-1);
	    }
	    else if (num_fields == 3) {
		add_oi(input_name, input_oid_str, NULL, NULL);
	    }
	    else {
		add_oi(input_name, input_oid_str, input_type, input_access_type);
	    }
	}			/* end of not a comment */
    }				/* end of while (fgets...) */
    return 1;
}				/* end of input_snmpinfo_file */

int add_defval(name, defval)
    char *name;
    char *defval;
{
    struct OID_INFO *ptr;

    for (ptr = root; ptr != NULL; ptr = ptr->next) {
        if(strcmp(ptr->name, name) == 0) {
            ptr->defval = (char *) strdup(defval);
            return 1;
        }
    }
    return 0;
}

int
add_trap(name, enterprise, number, objects)
    char *name;
    char *enterprise;
    long number;
    char *objects;
{
    TRAP_INFO_t *trap_ptr;

    if ((trap_ptr = (TRAP_INFO_t *) malloc (sizeof(TRAP_INFO_t))) == NULL) {
        printf("out of memory\n");
        exit(1);
    }
    trap_ptr->name = (char *) malloc (strlen(name) + 1);
    trap_ptr->enterprise = (char *) malloc (strlen(enterprise) + 1);
    if (objects) {
        trap_ptr->objects = (char *) malloc (strlen(objects) + 1);
        if (!trap_ptr->objects) {
            printf("out of memory\n");
            exit(1);
        }
    }
    if ((!trap_ptr->name) || (!trap_ptr->enterprise)) {
        printf("out of memory\n");
        exit(1);
    }
    strcpy(trap_ptr->name, name);
    strcpy(trap_ptr->enterprise, enterprise);
    if (objects) {
        strcpy(trap_ptr->objects, objects);
    } else {
        trap_ptr->objects = NULL;
    }
    trap_ptr->number = number;

    trap_ptr->next = trap_info;
    trap_info = trap_ptr;
    return 1;
}

/* read the enumerations for this variable */
int
get_enum(buffer)
    char           *buffer;
{
    char            name[256], group[256];
    int             val;
    struct OID_INFO *ptr = NULL;
    struct enumerations *p = NULL;
    struct TC      *temp_tc = NULL;
    int             is_tc;
    int             duplicate_found = 0;

    if (strncmp("%ev", buffer, 3) == 0) {
	is_tc = 0;
	if ((sscanf(buffer, "%%ev %s %s %d", group, name, &val)) != 3) {
	    fprintf(stderr, "error in input file format: %s\n", buffer);
	    exit(-1);
	}
    }
    else {
	is_tc = 1;
	if ((sscanf(buffer, "%%es %s %s %d", group, name, &val)) != 3) {
	    fprintf(stderr, "error in input file format :%s\n", buffer);
	    exit(-1);
	}
    }
    if (is_tc) {
	temp_tc = tc;
	while (strcmp(temp_tc->name, group) != 0) {
	    temp_tc = temp_tc->next;
	    if (!temp_tc) {
		fprintf(stderr, "%s not found\n", group);
		exit(-1);
	    }
	}
    }
    else {
	ptr = root;
	while (strcmp(ptr->name, group) != 0) {
	    ptr = ptr->next;
	    if (!ptr) {
		fprintf(stderr, "%s not found\n", group);
		exit(-1);
	    }
	}
    }

    /* malloc space for the new node */
    temp_enum = (struct enumerations *) malloc(sizeof(struct enumerations));
    temp_enum->name = (char *) malloc(strlen(name) + 1);
    if (!temp_enum || !temp_enum->name) {
	fprintf(stderr, "malloc error\n");
	exit(-1);
    }
    /* put the correct values in the new node */
    strcpy(temp_enum->name, name);
    temp_enum->cname = strdup(name);
    if (temp_enum->cname == NULL) {
	fprintf(stderr, "malloc error\n");
	exit(-1);
    }
    remove_hyphens(temp_enum->cname);
    temp_enum->val = val;
    temp_enum->next = NULL;

    /* put this node into the list */
    if (is_tc) {
	if (!temp_tc->enumer)
	    temp_tc->enumer = temp_enum;
	else {
	    p = temp_tc->enumer;
            if (strcmp(p->name, temp_enum->name) == 0) {
                duplicate_found = 1;
            } 
            while (p->next && !duplicate_found) {
                p = p->next;
                if (strcmp(p->name, temp_enum->name) == 0) {
                    duplicate_found = 1;
                } 
            }
            if (!duplicate_found) {
                p->next = temp_enum;
            } else {
                /* Check values for inconsistencies */
                if ((temp_enum->val != p->val) && !ignore_nonfatal_errors) {
                    fprintf(stderr, "Conflicting values for %s enumeration %s (values %d and %d)\n", temp_tc->name, temp_enum->name, p->val, temp_enum->val);
                    exit(-1);
                } 
            }
        }
    }
    else {
	if (!ptr->enumer)
	    ptr->enumer = temp_enum;
	else {
	    p = ptr->enumer;
	    while (p->next)
		p = p->next;
	    p->next = temp_enum;
	}
    }
    return 1;
}

/* get enumerations from a file in the format of snmpinfo.dat */
int
get_snmpinfo_enum(fp, objectname)
    FILE           *fp;
    char           *objectname;
{
    char            name[256];
    char            buffer[256];
    int             val;
    struct OID_INFO *ptr;
    struct enumerations *p;

    name[0] = '\0';

    ptr = root;
    while (strcmp(ptr->name, objectname) != 0) {
        ptr = ptr->next;
        if (!ptr) {
	    fprintf(stderr, "%s not found\n", objectname);
	    exit(-1);
        }
    }

    buffer[0] = '\0';
    while (buffer[0] != ')') {
        if ((fgets(buffer, sizeof(buffer), fp)) == NULL) {
	    fprintf(stderr, "get_snmpinfo_enum : error in input file format\n");
	    exit(-1);
        }
        /*
         * deal with DOS crlf on other OSs
         */
        if ( (strlen(buffer) > 1) &&
             (buffer[strlen(buffer) - 2] == '\r')) {
          buffer[strlen(buffer) - 2] = '\n';
        }

	if (buffer[0] == ')') {
            continue;
	}
        if ((sscanf(buffer, "%d %s", &val, name)) != 2) {
	    fprintf(stderr, "get_snmpinfo_enum (%s)\nerror in input file format: %s\n", objectname, buffer);
	    exit(-1);
        }

       /* malloc space for the new node */
        temp_enum = (struct enumerations *) malloc(sizeof(struct enumerations));
        temp_enum->name = (char *) malloc(strlen(name) + 1);
        if (!temp_enum || !temp_enum->name) {
            fprintf(stderr, "malloc error\n");
            exit(-1);
        }
        /* put the correct values in the new node */
        strcpy(temp_enum->name, name);
        temp_enum->val = val;
        temp_enum->next = NULL;

        if (!ptr->enumer)
            ptr->enumer = temp_enum;
        else {
            p = ptr->enumer;
            while (p->next) {
                p = p->next;
	    }
            p->next = temp_enum;
        }
    } 
    return 1;
}

/* get index entries from a file in the format of snmpinfo.dat */
int
get_snmpinfo_index(fp, objectname)
    FILE           *fp;
    char           *objectname;
{
    char            buffer[256];
    char            name[256];
    index_table_t  *table;
    int             i;
    struct OID_INFO *ptr;

    ptr = root;
    while (strcmp(ptr->name, objectname) != 0) {
        ptr = ptr->next;
        if (!ptr) {
            fprintf(stderr, "%s not found\n", objectname);
            exit(-1);
        }
    }

    if ((table = (index_table_t *) malloc(sizeof(index_table_t))) == NULL) {
	fprintf(stderr, "malloc error\n");
	exit(-1);
    }
#ifdef SR_CLEAR_MALLOC
    memset(table, 0, sizeof(index_table_t));
#endif /* SR_CLEAR_MALLOC */
    if ((table->name = strdup(objectname)) == NULL) {
	fprintf(stderr, "malloc error\n");
	exit(-1);
    }

    i = 0;
    buffer[0] = '\0';
    while (buffer[0] != '}') {
        if ((fgets(buffer, sizeof(buffer), fp)) == NULL) {
	    fprintf(stderr, "get_snmpinfo_index : error in input file format\n");
	    exit(-1);
        }
        /*
         * deal with DOS crlf on other OSs
         */
        if ( (strlen(buffer) > 1) &&
             (buffer[strlen(buffer) - 2] == '\r')) {
          buffer[strlen(buffer) - 2] = '\n';
        }

	if (buffer[0] == '}') {
            continue;
	}
        if ((sscanf(buffer, " IMPLIED %s", name)) == 1) {
	    table->index_array[i].IndexType = SR_IMPLIED;
	} else if ((sscanf(buffer, "%s", name)) != 1) {
	    fprintf(stderr, "get_snmpinfo_index: error in input file format: %s\n", buffer);
	    exit(-1);
        }
	/* Should perhaps check for table overflow here? */
	if ((table->index_array[i].name = strdup(name)) == NULL) {
	    fprintf(stderr, "malloc error\n");
	    exit(-1);
	}
	table->index_array[i].length = 0;
	i++;
    }
    table->next = index_table;
    table->ptr = ptr;
    index_table = table;
    return 1;
}

/* read the enumerations for this variable */
/* old_get_enum is for backward compatibility with 7.0 mosy and v1 SMIC */
int
old_get_enum(buffer, fp)
    char           *buffer;
    FILE           *fp;
{
    char            name[256], group[256];
    int             val, number;
    struct OID_INFO *ptr = NULL;
    struct enumerations *p = NULL;
    int             is_enum;
    struct TC      *temp_tc = NULL;

    buffer++;
    if (buffer[0] == '*') {
	buffer++;
	is_enum = 1;
    }
    else {
	is_enum = 0;
    }

    if ((sscanf(buffer, "%s %d", group, &number)) != 2) {
	fprintf(stderr, "error in input file format\n");
	exit(-1);
    }
    if (is_enum) {
	temp_tc = tc;
	while (strcmp(temp_tc->name, group) != 0) {
	    temp_tc = temp_tc->next;
	    if (!temp_tc) {
		fprintf(stderr, "%s not found\n", group);
		exit(-1);
	    }
	}
    }
    else {
	ptr = root;
	while (strcmp(ptr->name, group) != 0) {
	    ptr = ptr->next;
	    if (!ptr) {
		fprintf(stderr, "%s not found\n", group);
		exit(-1);
	    }
	}
    }
    while (fscanf(fp, "%s %d", name, &val) == 2) {
	/* malloc space for the new node */
	temp_enum = (struct enumerations *) malloc(sizeof(struct enumerations));
	temp_enum->name = (char *) malloc(strlen(name) + 1);
	if (!temp_enum || !temp_enum->name) {
	    fprintf(stderr, "malloc error\n");
	    exit(-1);
	}
	/* put the correct values in the new node */
	strcpy(temp_enum->name, name);
	temp_enum->val = val;
	temp_enum->next = NULL;

	/* put this node into the list */
	if (is_enum) {
	    if (!temp_tc->enumer)
		temp_tc->enumer = temp_enum;
	    else {
		p = temp_tc->enumer;
		while (p->next)
		    p = p->next;
		p->next = temp_enum;
	    }
	}
	else {
	    if (!ptr->enumer)
		ptr->enumer = temp_enum;
	    else {
		p = ptr->enumer;
		while (p->next)
		    p = p->next;
		p->next = temp_enum;
	    }
	}

    }
    if (name[0] != ')') {
	fprintf(stderr, "error in input file format\n");
	exit(-1);
    }
    return 1;
}

int
add_size_range(name, range_min, range_max)
    char           *name;
    char           *range_min;
    char           *range_max;
{
    size_range_t   *temp_size;
    struct OID_INFO *ptr;
    struct TC      *temp_tc;

    /* loop through the list of all elements */
    for (ptr = root; ptr != NULL; ptr = ptr->next) {
	if (strcmp(name, ptr->name) == 0) {
	    /* allocate spave for this new size entry in the linked list */
	    temp_size = (size_range_t *) malloc(sizeof(size_range_t));
	    if (temp_size == NULL) {
		printf("out of memory\n");
		exit(-1);
	    }

	    /* fill in values for this new entry  */
	    temp_size->min = atol(range_min);
	    temp_size->max = atol(range_max);

      /* check that it's actually a new size */
      for (check_size = ptr->size; check_size; check_size = check_size->next)
      {
        if (check_size->min == temp_size->min && check_size->max == temp_size->max)
        {
          /* duplicate definitions */
          free(temp_size);
          return 1;
        }
      }

	    /* put this new entry into the list */
	    temp_size->next = ptr->size;
	    ptr->size = temp_size;

	    return 1;
	}
    }

    /*
     * if we didn't find a matching object, look for a matching textual
     * convention
     */
    for (temp_tc = tc; temp_tc; temp_tc = temp_tc->next) {
	if (strcmp(name, temp_tc->name) == 0) {
	    /* allocate spave for this new size entry in the linked list */
	    temp_size = (size_range_t *) malloc(sizeof(size_range_t));
	    if (temp_size == NULL) {
		printf("out of memory\n");
		exit(-1);
	    }

	    /* fill in values for this new entry  */
	    temp_size->min = atol(range_min);
	    temp_size->max = atol(range_max);

      /* check that it's actually a new size */
      for (check_size = temp_tc->size; check_size; check_size = check_size->next)
      {
        if (check_size->min == temp_size->min && check_size->max == temp_size->max)
        {
          /* duplicate definitions */
          free(temp_size);
          return 1;
        }
      }

      /* put this new entry into the list */
	    temp_size->next = temp_tc->size;
	    temp_tc->size = temp_size;

	    return 1;
	}
    }

    fprintf(stderr, "Warning: Can't find %s\n", name);
    return -1;
}

int
add_init_oi()
{
    /* add_oi("ccitt", "0", NULL, NULL); */
    add_oi("iso", "1", NULL, NULL);
    /* add_oi("joint-iso-ccitt", "2", NULL, NULL); */
    add_oi("org", "iso.3", NULL, NULL);
    add_oi("dod", "org.6", NULL, NULL);

    add_oi("mib-2", "mgmt.1", NULL, NULL);
    add_oi("snmpResearch", "enterprises.99", NULL, NULL);
    add_oi("snmpResearchMIBs", "snmpResearch.12", NULL, NULL);

    /* from the SMI */
    add_oi("internet", "dod.1", NULL, NULL);
    add_oi("directory", "internet.1", NULL, NULL);
    add_oi("mgmt", "internet.2", NULL, NULL);
    add_oi("experimental", "internet.3", NULL, NULL);
    add_oi("private", "internet.4", NULL, NULL);
    add_oi("enterprises", "private.1", NULL, NULL);
    return 1;
}

int
process_data()
{
    struct OID_INFO *ptr1;
    struct OID_INFO *ptr2 = NULL;
    char            test_buff[1024];


    /* loop through the list of all elements */
    for (ptr1 = root; ptr1 != NULL; ptr1 = ptr1->next) {

	/*
	 * check that string is "parented", i.e. not 0.something, 1.something
	 */
	if (!(((ptr1->oid_pre_str[0] == '0') || (ptr1->oid_pre_str[0] == '1') ||
	(ptr1->oid_pre_str[0] == '2')) && (ptr1->oid_pre_str[1] == '\0'))) {
            /* start at the current position and work back. If we don't find
             * a match, then try again starting at the beginning of the list */
	    for (ptr2 = ptr1; ptr2 != NULL; ptr2 = ptr2->next) {
		if (strcmp(ptr1->oid_pre_str, ptr2->name) == 0) {	/* found parent? */
		    ptr1->parent_ptr = ptr2;
		    break;	/* bail out of inner loop, we just found
				 * parent */
		}
	    }			/* end of for (ptr2 = ...) */

	    if (ptr2 == NULL) {	/* parent wasn't in list */
	        for (ptr2 = root; ptr2 != NULL; ptr2 = ptr2->next) {
		    if (strcmp(ptr1->oid_pre_str, ptr2->name) == 0) {	/* found parent? */
		        ptr1->parent_ptr = ptr2;
		        break;	/* bail out of inner loop, we just found
				 * parent */
		    }
	        }		/* end of for (ptr2 = ...) */
	    }			/* end of for (ptr2 = ...) */

	    if (ptr2 == NULL) {	/* parent wasn't in list */
		fprintf(stderr, "%s: process_data(), Couldn't find parent: %s\n",
			prog_name, ptr1->oid_pre_str);
		exit(-1);
	    }			/* end of if (ptr2 == NULL) */
	}			/* end of is parented */
    }				/* end of for (ptr1 = root;...) */

    /*
     * We've got the elements pointing to their parent element, now flesh out
     * the OID numbers.  Make sure the result is valid.
     */
    for (ptr1 = root; ptr1 != NULL; ptr1 = ptr1->next) {
	test_buff[0] = '\0';
	flesh_out(test_buff, ptr1, 0);
	if ((ptr1->oid_ptr = MakeOIDFromDot(test_buff)) == NULL) {
	    fprintf(stderr, "%s: Failure creating OID, test_buff: %s\n", prog_name, test_buff);
	    exit(-1);
	}
    }
    return 1;
}

int
flesh_out(buff, ptr, x)
    char           *buff;
    struct OID_INFO *ptr;
    int             x;
{
    /* if not, see if we've fleshed this part out yet */
    if (ptr->oid_fleshed_str) {
	strcpy(buff, ptr->oid_fleshed_str);
    }
    else {
	/*
	 * get to the root of the definition, either a parent or this element
	 */
	if (ptr->parent_ptr != NULL) {


	    /*
	     * if the variable is the same as its parent, then there is a
	     * recursive definition in the MIB
	     */
	    if (strcmp(ptr->name, ptr->parent_ptr->name) == 0) {
		fprintf(stderr, "Error: recursive definition of %s\n",
			ptr->parent_ptr->name);
		exit(-1);
	    }
            if (x > 100) {
                fprintf(stderr, "Error: recursive definition in MIB ???\n");
                exit(-1);
            }
	    flesh_out(buff, ptr->parent_ptr, ++x);
	}
	else
	    strcpy(buff, ptr->oid_pre_str);

	/*
	 * now add the post string, if any.  the "0", "1", & "2" don't
	 */
	if (ptr->oid_post_str != NULL) {
	    strcat(buff, ".");
	    strcat(buff, ptr->oid_post_str);
	}
	if ((ptr->oid_fleshed_str = (char *) malloc(strlen(buff) + 1)) == NULL) {
	    fprintf(stderr, "%s: flesh_out: Malloc error", prog_name);
	    exit(-1);
	}
	strcpy(ptr->oid_fleshed_str, buff);
    }
    return 1;
}

/*
 * output_multiple_files calls output_file() for each set of files to be
 * created. If there is no config file given on the command line then
 * output_file() will be called exactly once to generate one set of output
 * files.
 */
int
output_multiple_files(base)
    char           *base;
{
    FILE           *fp;
    char            buff[BUFF_SIZE];
    struct OID_INFO *ptr;
    char           *table[MAX_ENTRIES], *p;
    OID            *oid_table[MAX_ENTRIES];
    char            action[MAX_ENTRIES];
    int             i, j;
    OID            *tmpoid;
    static char     tmpfilename[] = "tmpmib1.cnf";
    struct OID_INFO *temp_ptr;
    int             keep_family_valid;

    /* if no config file, just need one pass through output_file() */
    if ((postmosy_config_file == NULL) && (postmosy_config_line == NULL)) {
	/* mark all entries valid */
	for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
	    ptr->valid = TRUE;
	}
        /* if the -remove_subtree option is used, mark entries in that *
         * subtree as not valid */
        if (remove_subtree) {
            if ((tmpoid = MakeOIDFromDot(subtree_to_remove)) == NULL) {
                fprintf(stderr, "Cannot remove subtree: %s\n", subtree_to_remove);
                exit(-1);
            }
	    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
                if (CmpNOID(tmpoid, ptr->oid_ptr, tmpoid->length) == 0) {
	            ptr->valid = FALSE;
                }
	    }
        }

        /*
         * For each object - store its family number in the
         * OID_INFO struct.
         */
        calculate_family_numbers(sorted_root, 0);

        /*
         * count the number of entries in each family
         */
        count_family_entries();

	output_file(base);

	return 0;
    }
    if (postmosy_config_line) {
        fp = fopen(tmpfilename, "w");
        if (fp == NULL) {
	    fprintf(stderr, "postmosy: couldn't open temp file for writing\n");
            exit(-1);
        }
        fprintf(fp, "%s\n", postmosy_config_line);
        fclose(fp);
        postmosy_config_file = tmpfilename;
    }
    /* open config file */
    fp = fopen(postmosy_config_file, "r");
    if (fp == NULL) {
	fprintf(stderr, "postmosy: couldn't open %s for reading\n",
		postmosy_config_file);
	exit(-1);
    }
    while (fgets(buff, sizeof(buff), fp) != NULL) {
        /*
         * deal with DOS crlf on other OSs
         */
        if ( (strlen(buff) > 1) &&
             (buff[strlen(buff) - 2] == '\r')) {
          buff[strlen(buff) - 2] = '\n';
        }

	/* strip off leading spaces */
	j = 0;
	while ((buff[j] == ' ') || (buff[j] == '\t')) {
	    j++;
	}

	/* '#' indicates a comment */
	if ((buff[j] != '#') && (buff[j] != '\n')) {
	    if (strlen(buff) > (unsigned) (BUFF_SIZE - 1)) {
		fprintf(stderr, "error in %s. Line too long: %s\n", postmosy_config_file, buff);
		exit(-1);
	    }
	    if (buff[j] == '[') {
		j++;
		while (buff[j] && isspace((unsigned char)buff[j])) {
		    j++;
		}
		continue;
	    }

	    table[i = 0] = &buff[j];
	    /* p = (char *) strchr(table[0], ' '); */
	    p = table[0];
	    while ((*p != ' ') && (*p != '\t') && (*p != '\n') && (*p != '\0')) {
		p++;
	    }
	    if ((*p == '\0') || (*p == '\n')) {
		fprintf(stderr, "error in %s: %s\n", postmosy_config_file, buff);
		exit(-1);
	    }
	    else
		*p = '\0';
	    p++;
	    while ((*p == ' ') || (*p == '\t'))
		p++;		/* skip blank spaces */
	    if (*p == '\n') {
		fprintf(stderr, "error in %s: %s\n", postmosy_config_file, buff);
		exit(-1);
	    }
	    /*
	     * fill the array (table[]) with entries to be included in this
	     * pass
	     */
	    do {
		i++;
		if (i > MAX_ENTRIES) {
		    fprintf(stderr, "config file max entries exceeded\n");
		    fprintf(stderr, "increase MAX_ENTRIES in mibtools.h\n");
		    exit(-1);
		}
		action[i] = p[0];
		if ((action[i] == '+') || (action[i] == '-')) {
		    table[i] = p + 1;	/* strip off the leading * '+' or '-' */
		}
		else {
		    action[i] = '+';
		    table[i] = p;
		}
		/* p = (char *) strchr(table[i], ' '); */
		p = table[i];
		while ((*p != ' ') && (*p != '\t') && (*p != '\n') && (*p != '\0')) {
		    p++;
		}
		/* if(p) { *//* } */
		if ((*p != '\0') || (*p != '\n')) {
		    *p = '\0';
		    p++;
		    /* skip blanks spaces and new lines */
		    while ((*p == ' ') || (*p == '\t') || (*p == '\n'))
			p++;
		}
		else {
		    /*
		     * replace the final newline with a NULL character
		     */
		    p = (char *) strchr(table[i], '\n');
		    if (p)
			*p = '\0';
		    p = NULL;
		}
	    } while (p && *p);
	    table[++i] = NULL;

	    /* map names from table to OIDs in oid_table */
	    i = 1;
	    while (table[i]) {
                remove_hyphens(table[i]);
		oid_table[i] = MakeOIDFromDot(table[i]);
		if (oid_table[i] == NULL) {
		    fprintf(stderr, "MakeOIDFromDot failed: %s\n", table[i]);
		    exit(-1);
		}
		i++;
	    }

	    /* mark all entries invalid */
	    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
		ptr->valid = FALSE;
	    }

	    /*
	     * look at each entry and determine if it should be included in
	     * this pass
	     */
	    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
		i = 1;
		while (table[i]) {
		    if (action[i] == '+') {
			if (CmpNOID(oid_table[i], ptr->oid_ptr, oid_table[i]->length) == 0) {
			    ptr->valid = TRUE;
			    if (ptr->parent_ptr) {
				ptr->parent_ptr->valid = TRUE;
			    }
			}
		    }
		    else if (action[i] == '-') {
			if (CmpNOID(oid_table[i], ptr->oid_ptr, oid_table[i]->length) == 0) {
			    ptr->valid = FALSE;
			}
		    }
		    else {
			fprintf(stderr, "error in input file: %s\n", postmosy_config_file);
			exit(-1);
		    }
		    i++;
		}
	    }

            /*
             * For each object - store its family number in the
             * OID_INFO struct.
             */
            calculate_family_numbers(sorted_root, 0);

            /*
             * count the number of entries in each family
             */
            count_family_entries();

            /* if none of the leaf objects in a family are valid,
             * mark the entire family as not valid */
            for (ptr = sorted_root; (ptr = next_valid_family(ptr)) != NULL;
                 ptr = ptr->next_sorted) {
                keep_family_valid = 0;
                for (temp_ptr = ptr->next_family_entry; temp_ptr;
                     temp_ptr = temp_ptr->next_family_entry) {
                    if (!temp_ptr->valid) continue;
                    keep_family_valid = 1;
                }
                if (!keep_family_valid) {
                    ptr->valid = FALSE;
                }
            }

            /* if any family is not valid, but it is augmented by
             * one or more objects that are valid, mark the family
             * as valid */
            make_families_with_valid_augments_valid();

	    /*
	     * now, write the information out to the various files
	     */
	    output_file(table[0]);
	}
    }
    fclose(fp);
    if (postmosy_config_line) {
        unlink(tmpfilename);
    }
    return 1;
}

/*
 * print SNMP Research, Inc. copyright notice inside C style comments for
 * inclusion in C files
 */
int
print_copyright(fp)
    FILE           *fp;
{
    fprintf(fp, "/*\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * Copyright (C) 1992-2006 by SNMP Research, Incorporated.\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * This software is furnished under a license and may be used and copied\n");
    fprintf(fp, " * only in accordance with the terms of such license and with the\n");
    fprintf(fp, " * inclusion of the above copyright notice. This software or any other\n");
    fprintf(fp, " * copies thereof may not be provided or otherwise made available to any\n");
    fprintf(fp, " * other person. No title to and ownership of the software is hereby\n");
    fprintf(fp, " * transferred.\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * The information in this software is subject to change without notice\n");
    fprintf(fp, " * and should not be construed as a commitment by SNMP Research, Incorporated.\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * Restricted Rights Legend:\n");
    fprintf(fp, " *  Use, duplication, or disclosure by the Government is subject to\n");
    fprintf(fp, " *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights\n");
    fprintf(fp, " *  in Technical Data and Computer Software clause at DFARS 252.227-7013;\n");
    fprintf(fp, " *  subparagraphs (c)(4) and (d) of the Commercial Computer\n");
    fprintf(fp, " *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar\n");
    fprintf(fp, " *  clauses in the NASA FAR Supplement and other corresponding\n");
    fprintf(fp, " *  governmental regulations.\n");
    fprintf(fp, " *\n");
    fprintf(fp, " */\n\n");
    fprintf(fp, "/*\n");
    fprintf(fp, " *                PROPRIETARY NOTICE\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * This software is an unpublished work subject to a confidentiality agreement\n");
    fprintf(fp, " * and is protected by copyright and trade secret law.  Unauthorized copying,\n");
    fprintf(fp, " * redistribution or other use of this work is prohibited.\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * The above notice of copyright on this source code product does not indicate\n");
    fprintf(fp, " * any actual or intended publication of such source code.\n");
    fprintf(fp, " */\n");


    fprintf(fp, "\n\n");

    fprintf(fp, "/*\n * Arguments used to create this file:\n%s\n */\n\n",
            postmosyargs);

    return 1;
}


/* print SNMP Research, Inc. copyright notice */
int
print_copyright_2(fp)
    FILE           *fp;
{
    fprintf(fp, "#\n");
    fprintf(fp, "# Copyright (C) 1992-2006 by SNMP Research, Incorporated.\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# This software is furnished under a license and may be used and copied\n");
    fprintf(fp, "# only in accordance with the terms of such license and with the\n");
    fprintf(fp, "# inclusion of the above copyright notice. This software or any other\n");
    fprintf(fp, "# copies thereof may not be provided or otherwise made available to any\n");
    fprintf(fp, "# other person. No title to and ownership of the software is hereby\n");
    fprintf(fp, "# transferred.\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# The information in this software is subject to change without notice\n");
    fprintf(fp, "# and should not be construed as a commitment by SNMP Research, Incorporated.\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# Restricted Rights Legend:\n");
    fprintf(fp, "#  Use, duplication, or disclosure by the Government is subject to restrictions\n");
    fprintf(fp, "#  as set forth in subparagraph (c)(1)(ii) of the Rights in Technical Data\n");
    fprintf(fp, "#  and Computer Software clause at DFARS 52.227-7013 and in similar clauses\n");
    fprintf(fp, "#  in the FAR and NASA FAR Supplement.\n");
    fprintf(fp, "#\n");
    fprintf(fp, "#\n");

    fprintf(fp, "\n");

    fprintf(fp, "#                PROPRIETARY NOTICE\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# This software is an unpublished work subject to a confidentiality agreement\n");
    fprintf(fp, "# and is protected by copyright and trade secret law.  Unauthorized copying,\n");
    fprintf(fp, "# redistribution or other use of this work is prohibited.\n");
    fprintf(fp, "# The above notice of copyright on this source code product does not indicate\n");
    fprintf(fp, "# any actual or intended publication of such source code.\n");
    fprintf(fp, "#\n");

    fprintf(fp, "\n\n");
    return 1;
}

#define C_CVS_KEYWORD_STRING "\n#if !defined(lint) && defined(SR_RCSID)\nstatic const char rcsid[] = \"%s\" ;\n#endif  /* !lint && SR_RCSID */\n\n/*\n* File Description:\n*/\n\n"

#define H_CVS_KEYWORD_STRING "/* %s */\n\
\n\
/*\n\
 * File Description:\n\
 */\n\
"

void
print_cvs_keywords(fp, filetype)
  FILE *fp;
  int filetype;
{
  /* static char logstring[8]; */
  static char idstring[8];
  static int done = 0;

  /*
   * this ugliness is just there so that the keywords don't get
   * expanded in _this_ file
   */
  if (!done) {
    done = 1;
    /* memset(logstring, 0, sizeof(logstring)); */
    memset(idstring, 0, sizeof(idstring));
    /* sprintf(logstring, "%s%s%s", "$", "Log", "$"); */
    sprintf(idstring, "%s%s%s", "$", "Id", "$");
  }

  if (no_cvs_keywords || fp == NULL) {
    /* this shouldn't happen */
    return;
  }

  switch (filetype) {
    case C_FILE:
      fprintf(fp, C_CVS_KEYWORD_STRING, idstring);
      break;
    case H_FILE:
      fprintf(fp, H_CVS_KEYWORD_STRING, idstring);
      break;
    default:
      /* this shouldn't happen */
      break;
  }
}
/*
 * Find the C type to be used for the given asn.1 type.
 */
int
find_type(type, name, oid_type, size)
    char           *type, *name, *oid_type;
    size_range_t   *size;
{
    char            buff[128];

    strcpy(buff, oid_type);

    if (strcmp(buff, "DisplayString") == 0) {
	strcpy(type, "OctetString     *");
	strcat(type, name);
    }
    else if (strcmp(buff, "OctetString") == 0) {
	strcpy(type, "OctetString     *");
	strcat(type, name);
    }
    else if (strcmp(buff, "PhysAddress") == 0) {
	strcpy(type, "OctetString     *");
	strcat(type, name);
    }
    else if (strcmp(buff, "NetworkAddress") == 0) {
	/* strcpy(type,"char            *"); */
	strcpy(type, "OctetString     *");
	strcat(type, name);
    }
    else if (strcmp(buff, "IpAddress") == 0) {
	strcpy(type, "SR_UINT32       ");
	strcat(type, name);
    }
    else if (strcmp(buff, "ObjectID") == 0) {
	strcpy(type, "OID             *");
	strcat(type, name);
    }
    else if (strcmp(buff, "TimeTicks") == 0) {
	strcpy(type, "SR_UINT32       ");
	strcat(type, name);
    }
    else if (strcmp(buff, "INTEGER") == 0) {
        if (size && (size->min >= 0) && (size->max > 2147483647)) {
	    strcpy(type, "SR_UINT32       ");
        } else {
	    strcpy(type, "SR_INT32        ");
        }
	strcat(type, name);
    }
    else if (strcmp(buff, "Gauge") == 0) {
	strcpy(type, "SR_UINT32       ");
	strcat(type, name);
    }
    else if (strcmp(buff, "Counter") == 0) {
	strcpy(type, "SR_UINT32       ");
	strcat(type, name);
    }

    else if (strcmp(buff, "Counter32") == 0) {
	strcpy(type, "SR_UINT32       ");
	strcat(type, name);
    }
    else if (strcmp(buff, "Gauge32") == 0) {
	strcpy(type, "SR_UINT32       ");
	strcat(type, name);
    }
    else if (strcmp(buff, "NsapAddress") == 0) {
	strcpy(type, "OctetString     *");
	strcat(type, name);
    }
    else if (strcmp(buff, "Counter64") == 0) {
	strcpy(type, "UInt64          *");
	strcat(type, name);
    }
    else if (strcmp(buff, "Unsigned32") == 0) {
	strcpy(type, "SR_UINT32       ");
	strcat(type, name);
    }
    else if (strcmp(buff, "UInteger32") == 0) {
	strcpy(type, "SR_UINT32       ");
	strcat(type, name);
    }
    else if (strcmp(buff, "Integer32") == 0) {
	strcpy(type, "SR_INT32        ");
	strcat(type, name);
    }
    else if (strcmp(buff, "Bits") == 0) {
	strcpy(type, "OctetString     *");
	strcat(type, name);
    }
    else if (strcmp(buff, "Opaque") == 0) {
	strcpy(type, "OctetString     *");
	strcat(type, name);
        /* remove the following 2 lines to add support for Qpaque */
	fprintf(stderr, "find_type(): Opaque type not supported\n");
        exit(-1);
    }

    else {
	fprintf(stderr, "find_type(): unknown type: %s\n", buff);
	exit(-1);
    }
    return 1;
}

int 
map_variable_type_for_ber(type, buf)
    char           *type, *buf;
{
    char            temp[128];

    strcpy(temp, type);

    /*
     * try to figure out what variable type to use
     */
    if (strcmp(temp, "INTEGER") == 0) {
	strcpy(buf, "INTEGER_TYPE");
    }
    else if (strcmp(temp, "DisplayString") == 0 ||
	     strcmp(temp, "PhysAddress") == 0 ||
	     strcmp(temp, "OctetString") == 0) {
	strcpy(buf, "OCTET_PRIM_TYPE");
    }
    else if (strcmp(temp, "ObjectID") == 0) {
	strcpy(buf, "OBJECT_ID_TYPE");
    }
    else if (strcmp(temp, "IpAddress") == 0 ||
	     strcmp(temp, "NetworkAddress") == 0) {
	strcpy(buf, "IP_ADDR_PRIM_TYPE");
    }
    else if (strcmp(temp, "Counter") == 0) {
	strcpy(buf, "COUNTER_TYPE");
    }
    else if (strcmp(temp, "Gauge") == 0) {
	strcpy(buf, "GAUGE_TYPE");
    }
    else if (strcmp(temp, "TimeTicks") == 0) {
	strcpy(buf, "TIME_TICKS_TYPE");
    }
    else if (strcmp(temp, "Counter32") == 0) {
	strcpy(buf, "COUNTER_32_TYPE");
    }
    else if (strcmp(temp, "Gauge32") == 0) {
	strcpy(buf, "GAUGE_32_TYPE");
    }
    else if (strcmp(temp, "NsapAddress") == 0) {
	strcpy(buf, "OCTET_PRIM_TYPE");
    }
    else if (strcmp(temp, "Counter64") == 0) {
	strcpy(buf, "COUNTER_64_TYPE");
    }
    else if (strcmp(temp, "UInteger32") == 0) {
	strcpy(buf, "U_INTEGER_32_TYPE");
    }
    else if (strcmp(temp, "Unsigned32") == 0) {
	strcpy(buf, "UNSIGNED32_TYPE");
    }
    else if (strcmp(temp, "Integer32") == 0) {
	strcpy(buf, "INTEGER_TYPE");
    }
    else if (strcmp(temp, "Bits") == 0) {
	strcpy(buf, "BITS_TYPE");
    }
    else if (strcmp(temp, "Opaque") == 0) {
	strcpy(buf, "OPAQUE_PRIM_TYPE");
    }
    else {
	/* unknown type */
	fprintf(stderr, "%s: map_variable_type_for_ber: unknown type %s\n",
		prog_name, temp);
	exit(-1);
    }
    return 1;
}


int
add_oi(input_name, input_oid_str, input_type, input_access)
    char           *input_name;
    char           *input_oid_str;
    char           *input_type;
    char           *input_access;
{
    struct OID_INFO *tmp_oi_ptr;

    if ((tmp_oi_ptr = (struct OID_INFO *) malloc(sizeof(struct OID_INFO))) == NULL) {
	fprintf(stderr, "%s: add_oi (input_name == %s).  Malloc error.\n",
		prog_name, input_name);
	exit(-1);
    }
#ifdef SR_CLEAR_MALLOC
    memset(tmp_oi_ptr, 0, sizeof(struct OID_INFO));
#endif	/* SR_CLEAR_MALLOC */

    if ((tmp_oi_ptr->name = (char *) malloc(strlen(input_name) + 1)) == NULL) {
	fprintf(stderr, "%s: add_oi (input_name == %s).  Malloc error.\n",
		prog_name, input_name);
	exit(-1);
    }
    strcpy(tmp_oi_ptr->name, input_name);

    if ((tmp_oi_ptr->oid_pre_str = (char *) malloc(strlen(input_oid_str) + 1)) == NULL) {
	fprintf(stderr, "%s: add_oi (input_name == %s).  Malloc error.\n",
		prog_name, input_name);
	exit(-1);
    }
    strcpy(tmp_oi_ptr->oid_pre_str, input_oid_str);

    /* add in the input type if it's there */
    if (input_type != NULL) {
	if ((tmp_oi_ptr->oid_type = (char *) malloc(strlen(input_type) + 1)) == NULL) {
	    fprintf(stderr, "%s: add_oi (input_name == %s).  Malloc error.\n",
		    prog_name, input_name);
	    exit(-1);
	}
	strcpy(tmp_oi_ptr->oid_type, input_type);
    }
    else {
	tmp_oi_ptr->oid_type = NULL;
    }

    /* add in the access type if it's there */
    if (input_access != NULL) {
	if ((tmp_oi_ptr->oid_access = (char *) malloc(strlen(input_access) + 1)) == NULL) {
	    fprintf(stderr, "%s: add_oi (input_name == %s).  Malloc error.\n",
		    prog_name, input_name);
	    exit(-1);
	}
	strcpy(tmp_oi_ptr->oid_access, input_access);
    }
    else {
	tmp_oi_ptr->oid_access = NULL;
    }

    if ((tmp_oi_ptr->oid_post_str = (char *) strchr(tmp_oi_ptr->oid_pre_str, '.')) != NULL) {
	*(tmp_oi_ptr->oid_post_str) = '\0';	/* break the string into pre
						 * and post parts */
	(tmp_oi_ptr->oid_post_str)++;
    }				/* end of if (strchr(...,'.') != NULL) */
    tmp_oi_ptr->parent_ptr = NULL;	/* initialize the parent_ptr */
    tmp_oi_ptr->oid_fleshed_str = NULL;	/* initialize the fleshed string */
    tmp_oi_ptr->oid_ptr = NULL;
    tmp_oi_ptr->next_sorted = NULL;
    tmp_oi_ptr->enumer = NULL;
    tmp_oi_ptr->size = NULL;
    tmp_oi_ptr->defval = NULL;
    tmp_oi_ptr->num_entries = 0;
    tmp_oi_ptr->family_nr = -1;
    tmp_oi_ptr->getinfo = -1;
    tmp_oi_ptr->testinfo = -1;

    /* now put it on the list */
    tmp_oi_ptr->next = root;
    root = tmp_oi_ptr;

    return 1;
}

/*
 * Loop through all entries looking for duplicate names with *DIFFERENT*
 * OIDs.  Ignore if they are just duplicate name/OID pairs.  sort_data() will
 * trim them out.
 */
int
check_names()
{
    struct OID_INFO *ptr1, *ptr2;

    for (ptr1 = root; ptr1 != NULL; ptr1 = ptr1->next) {
	for (ptr2 = root; ptr2 != NULL; ptr2 = ptr2->next) {
	    if (strcmp(ptr1->name, ptr2->name) == 0)
		if (cmp_oid(ptr1->oid_ptr, ptr2->oid_ptr) != 0) {
		    fprintf(stderr, "%s: check_names: Duplicate name with different OID's:%s\n", prog_name, ptr1->name);
		    fprintf(stderr, "OID1: %s, OID2: %s\n", ptr1->oid_fleshed_str, ptr2->oid_fleshed_str);
                    if (fpmerge) {
                        if (!ignore_nonfatal_errors) {
		            fprintf(fpmerge, "ERROR: "); 
                            merge_errors++;
                        } else {
		            fprintf(fpmerge, "WARNING: "); 
                            merge_warnings++;
                        }
		        fprintf(fpmerge, "%s: check_names: Duplicate name with different OID's:%s\n", prog_name, ptr1->name);
		        fprintf(fpmerge, "OID1: %s, OID2: %s\n", ptr1->oid_fleshed_str, ptr2->oid_fleshed_str);
                    }

                    if (!ignore_nonfatal_errors) {
                        fprintf(stderr, "fatal error, exiting (use -i option to ignore this error)\n\n");
		        exit(-1);
                    } else {
                        fprintf(stderr, "continuing (since -i option was used)\n\n");
                    }
		}
	}			/* end of for (ptr2...) */
    }				/* end of for (ptr1...) */
    return 1;
}				/* end of check_names */

int
sort_data()
{
    struct OID_INFO *ptr;

    for (ptr = root; ptr != NULL; ptr = ptr->next) {
	sorted_root = insert_sort_data(sorted_root, ptr);
    }
    return 1;
}

/* return 0 if the enumerations are the same, non-zero if they are different */
/* return a positive value of the merge is OK, return a -1 for merge warning */
/* and -2 for merge error */
int
cmp_enumerations(enumeration_t *e1, enumeration_t *e2, char *name)
{
    enumeration_t *tmp1, *tmp2;

    if (!e1 && !e2) {
        return 0;
    }
    if (e1 && !e2) {
        /* if only one has enumerations, fail */
        return -2;
        /* return 1 here if you want to use the enumerations instead of fail */
    }
    if (!e1 && e2) {
        /* if only one has enumerations, fail */
        return -2;
        /* return 2 here if you want to use the enumerations instead of fail */
    }
    for (tmp1 = e1, tmp2 = e2; tmp1 && tmp2; tmp1 = tmp1->next, tmp2 = tmp2->next) {
        if (tmp1->val != tmp2->val) {
            return -1;
        }
#if 0
        if (strcmp(tmp1->name, tmp2->name) != 0) {
            printf("WARNING: enumeration name conflict during merge for %s (continuing)\n", name);
            if (fpmerge) fprintf(fpmerge, "WARNING: enumeration name conflict during merge for %s (continuing)\n", name);
            merge_warnings++;
        }
#endif /* 0 */
    }
    if (tmp1) {
        /* return 1 if e1 is a superset of e2 */
        return 1;
    }
    if (tmp2) {
        /* return 2 if e2 is a superset of e1 */
        return 2;
    }
    return 0;
}

/* increment merge_errors if there are conflicts in the index clauses */
int
cmp_index() 
{
    struct index_table *table, *table2;
    struct _index_array *temp_index1, *temp_index2;
        
    for (table = index_table; table && table->next; table = table->next) {
        for (table2 = table->next; table2; table2 = table2->next) {
            if (strcmp(table->name, table2->name) == 0) {
                if (table->ptr && table2->ptr) {
                     if (strcmp(table->ptr->oid_fleshed_str,
                         table2->ptr->oid_fleshed_str) != 0) {
                         continue;
                     }
                }
                for (temp_index1 = table->index_array, 
                     temp_index2 = table2->index_array ;
                     temp_index1 && temp_index1->name && 
                     temp_index2 && temp_index2->name ; 
                     temp_index1++, temp_index2++) {

                    if (strcmp(temp_index1->name, temp_index2->name) != 0) {
                        printf("ERROR: INDEX conflicts during merge for %s\n",
                                table->name);
                        if (fpmerge) {
                            fprintf(fpmerge, 
                                "ERROR: INDEX conflicts during merge for %s\n", 
                                table->name); 
                        }
                        merge_errors++;
                    }
                }
                if (temp_index1->name || temp_index2->name) {
                        printf("ERROR: INDEX conflicts during merge for %s\n",
                                table->name);
                        if (fpmerge) {
                            fprintf(fpmerge, 
                                "ERROR: INDEX conflicts during merge for %s\n", 
                                table->name); 
                        }
                        merge_errors++;
                }
            }
        }
    }

    return 0;
}

/* return 0 if the access is the same, non-zero if it is different */
/* if different, set the access to the least restrictive access */
/* if one is read-write and one read-create, consider them both read-create */
int
cmp_access (OID_INFO_t *exist_ptr, OID_INFO_t *add_ptr) 
{
    if (strcmp(exist_ptr->oid_access, add_ptr->oid_access) == 0) {
        return 0;
    }

    if ( ((strcmp(exist_ptr->oid_access, "read-create") == 0) &&
         (strcmp(add_ptr->oid_access, "read-write") == 0)) ||
         ((strcmp(exist_ptr->oid_access, "read-write") == 0) &&
         (strcmp(add_ptr->oid_access, "read-create") == 0)) ) {
        exist_ptr->oid_access = "read-create";
        add_ptr->oid_access = "read-create";
        return 0;
    }

    if ( (strcmp(exist_ptr->oid_access, "read-create") == 0) ||
         (strcmp(add_ptr->oid_access, "read-create") == 0) ) {
        exist_ptr->oid_access = "read-create";
        add_ptr->oid_access = "read-create";
        return 1;
    }
    if ( (strcmp(exist_ptr->oid_access, "read-write") == 0) ||
         (strcmp(add_ptr->oid_access, "read-write") == 0) ) {
        exist_ptr->oid_access = "read-write";
        add_ptr->oid_access = "read-write";
        return 1;
    }
    if ( (strcmp(exist_ptr->oid_access, "read-only") == 0) ||
         (strcmp(add_ptr->oid_access, "read-only") == 0) ) {
        exist_ptr->oid_access = "read-only";
        add_ptr->oid_access = "read-only";
        return 1;
    }

    return 1;
}

/* return 0 if the syntax is the same, non-zero if it is different */
int
cmp_syntax (OID_INFO_t *exist_ptr, OID_INFO_t *add_ptr) 
{
    char *type1, *type2;
    int ret;
    char *ptr;
    char *t1, *t2;

#if 0
    get_one_prim_type(exist_ptr);
    get_one_prim_type(add_ptr);
#endif /* 0 */

    t1 = exist_ptr->oid_prim_type;
    t2 = add_ptr->oid_prim_type;

    if (!t1 || !t2) {
        return -1;
    }

    type1 = strdup(t1);
    type2 = strdup(t2);

    /* if inputinfo is true, the oid_prim_type field will include the
     * syntax and the size range. example: OctetString(0..255).
     * remove the size range here, we only care about type */
    ptr = strchr(type1, '(');
    if (ptr) *ptr = '\0';
    ptr = strchr(type2, '(');
    if (ptr) *ptr = '\0';

    /* change Counter32 to Counter, Gauge32 to Guage and Integer32 to Integer */
    if ((ptr = strstr(type1, "32"))) {
        *ptr = '\0';
    }
    if ((ptr = strstr(type2, "32"))) {
        *ptr = '\0';
    }

    /* if both objects have enumerations or neither objects have enumerations,
     * then treat INTEGER and Integer32 as the same
     */
    if ( (exist_ptr->enumer == NULL && add_ptr->enumer == NULL) ||
         (exist_ptr->enumer != NULL && add_ptr->enumer != NULL) ) {

        /* change INTEGER to Integer */
        if (strcmp(type1, "INTEGER") == 0) {
            free(type1);
            type1 = strdup("Integer");
        }
        if (strcmp(type2, "INTEGER") == 0) {
            free(type2);
            type2 = strdup("Integer");
        }
    }

    /* if one is DisplayString and one is OctetString, use OctetString */
    if (!strncmp(type1, "DisplayString", 13) && !strncmp(type2, "OctetString", 11)) {
        exist_ptr->oid_type = add_ptr->oid_type;
        free(type1);
        type1 = strdup(type2);
    }
    if (!strncmp(type2, "DisplayString", 13) && !strncmp(type1, "OctetString", 11)) {
        add_ptr->oid_type = exist_ptr->oid_type;
        free(type2);
        type2 = strdup(type1);
    }
    /* if one is PhysAddress and one is OctetString, use OctetString */
    if (!strncmp(type1, "PhysAddress", 11) && !strncmp(type2, "OctetString", 11)) {
        exist_ptr->oid_type = add_ptr->oid_type;
        free(type1);
        type1 = strdup(type2);
    }
    if (!strncmp(type2, "PhysAddress", 11) && !strncmp(type1, "OctetString", 11)) {
        add_ptr->oid_type = exist_ptr->oid_type;
        free(type2);
        type2 = strdup(type1);
    }


    ret = strcmp(type1, type2);
    free(type1); free(type2);
    return ret;
}

/* return 0 if the size is the same, non-zero if it is different */
int
cmp_info_size (OID_INFO_t *exist_ptr, OID_INFO_t *add_ptr) 
{
    char *size1, *size2;
    char *t1 = exist_ptr->oid_type;
    char *t2 = add_ptr->oid_type;
    int lo1, lo2, hi1, hi2;
    char *ptr;
    char newtype[64];

    if (!t1 || !t2) {
        return 0;
    }

    /* if inputinfo is true, the oid_type field will include the
     * syntax and the size range. example: OctetString(0..255) */
    size1 = strchr(t1, '(');
    size2 = strchr(t2, '(');

    /* if only one of the two objects has a size range, use that one */
    if (size1 && !size2) {
        add_ptr->oid_type = exist_ptr->oid_type;
        return 1;
    } else if (size2 && !size1) {
        exist_ptr->oid_type = add_ptr->oid_type;
        return 1;
    /* if they both have a size range, see if they are the same */
    } else if (size1 && size2) {
       if (strcmp(size1, size2) != 0) {
          lo1 = atoi(size1+1);
          lo2 = atoi(size2+1);

          ptr = strrchr(size1, '.');
          if (!ptr) return 1;
          hi1 = atoi(ptr+1);
          ptr = strrchr(size2, '.');
          if (!ptr) return 1;
          hi2 = atoi(ptr+1);

          *size1 = *size2 = '\0';
          sprintf(newtype, "%s(%d..%d)", t1, MIN(lo1, lo2), MAX(hi1, hi2));
          add_ptr->oid_type = strdup(newtype);
          exist_ptr->oid_type = strdup(newtype);
          return 1;
       }
    }
    /* if neither have size ranges, return OK */
    return 0;
}

/* return 0 if the size range is the same, non-zero if it is different */
int
cmp_sizes(size_range_t *s1, size_range_t *s2)
{
    size_range_t *tmp1, *tmp2;

    if (!s1 && !s2) {
        return 0;
    }
    if (!s1 || !s2) {
        if (inputinfo) {
            return 1;
        } else {
            return 0;
        }
    }
    for (tmp1 = s1, tmp2 = s2; tmp1 && tmp2; tmp1 = tmp1->next, tmp2 = tmp2->next) {
        if ( (tmp1->min != tmp2->min) || (tmp1->max != tmp2->max)) {
            return 1;
        }
    }
    if (tmp1 || tmp2) {
        return 1;
    }
    return 0;
}

/* this function will be called when 2 objects have the same name. */
int
merge_objects (OID_INFO_t *exist_ptr, OID_INFO_t *add_ptr) 
{
#if 0
    /* valid has not been set yet, so this won't work */
    /* if the objects are not being used, just return */
    if (exist_ptr->valid == FALSE && add_ptr->valid == FALSE) {
        return 0;
    }
#endif

    get_one_prim_type(exist_ptr);
    get_one_prim_type(add_ptr);

    /* check to see if the enumerations are the same */
    switch (cmp_enumerations(exist_ptr->enumer, add_ptr->enumer,
            exist_ptr->name)) {
        case 0:
        case 1:
            break;
        case 2:
            /* add_ptr->enumer is a superset, use it instead */
            exist_ptr->enumer = add_ptr->enumer;
            break;
        case -2:
            printf("ERROR: enumeration conflicts during merge for %s\n", exist_ptr->name);
            if (fpmerge) fprintf(fpmerge, "ERROR: enumeration conflicts during merge for %s\n", exist_ptr->name);
            merge_errors++;
            break;
        case -1:
        default:
            printf("WARNING: enumeration conflicts during merge for %s (continuing)\n", exist_ptr->name);
            if (fpmerge) fprintf(fpmerge, "WARNING: enumeration conflicts during merge for %s (continuing)\n", exist_ptr->name);
            merge_warnings++;
    }
    /* check to see if the type is the same */
    if ((exist_ptr->oid_type) && (add_ptr->oid_type)) {
        if (cmp_syntax(exist_ptr, add_ptr) != 0) {
            printf("ERROR: SYNTAX conflicts during merge for %s\n", exist_ptr->name);
            if (fpmerge) fprintf(fpmerge, "ERROR: SYNTAX conflicts during merge for %s\n", exist_ptr->name);
            merge_errors++;
        }
    }
    /* check to see if the access rights are the same */
    if ((exist_ptr->oid_access) && (add_ptr->oid_access)) {
        if (cmp_access(exist_ptr, add_ptr) != 0) {
            printf("WARNING: ACCESS conflicts during merge for %s (continuing)\n", exist_ptr->name); 
            if (fpmerge) fprintf(fpmerge, "WARNING: ACCESS conflicts during merge for %s (continuing)\n", exist_ptr->name); 
            merge_warnings++;
         }
    }
    /* check to see if size ranges are the same */
    if (inputinfo) {
        if (cmp_info_size(exist_ptr, add_ptr) != 0) {
            printf("WARNING: size conflicts during merge for %s (continuing)\n", exist_ptr->name); 
            if (fpmerge) fprintf(fpmerge, "WARNING: size conflicts during merge for %s (continuing)\n", exist_ptr->name); 
            merge_warnings++;
       }
    } else {
       if (cmp_sizes(exist_ptr->size, add_ptr->size) != 0) {
           printf("WARNING: size conflicts for %s (continuing)\n", exist_ptr->name);
           if (fpmerge) fprintf(fpmerge, "WARNING: size conflicts for %s (continuing)\n", exist_ptr->name);
           merge_warnings++;
       }
    }

    return 0;
}

struct OID_INFO *
insert_sort_data(exist_ptr, add_ptr)
    struct OID_INFO *exist_ptr, *add_ptr;
{
    int             cc;
    struct OID_INFO *ptr;

    if (exist_ptr == NULL)
	return (add_ptr);

    cc = cmp_oid(exist_ptr->oid_ptr, add_ptr->oid_ptr);

    if (cc == 0) {		/* Duplicate! */
        merge_objects(exist_ptr, add_ptr);
	/*
	 * if the name is the same, just lose the add_ptr
	 */
	if (strcmp(exist_ptr->name, add_ptr->name) == 0) {
             /* reassign any parent pointers that point to this node
              * before dropping this node from sorted list
              */
             for (ptr = root; ptr != NULL; ptr = ptr->next) {
                  if (ptr->parent_ptr == add_ptr) {
                      ptr->parent_ptr = exist_ptr;
                  }
             }
	    return (exist_ptr);
	} else {
#ifndef SR_SIEMENS_ICN_WN_CC_KEC
            if (agent) {
	        fprintf(stderr, "%s: ERROR: Duplicate OIDs with different names.\n", prog_name);
            } else {
	        fprintf(stderr, "%s: Warning: Duplicate OIDs with different names.\n", prog_name);
	        fprintf(stderr, "The first name will appear before the second name.\n");
            }
#else /* SR_SIEMENS_ICN_WN_CC_KEC */
	    fprintf(stderr, "%s: ERROR: Duplicate OIDs with different names.\n", prog_name);
#endif /* SR_SIEMENS_ICN_WN_CC_KEC */
	    fprintf(stderr, "OID: %s, First name: %s, Second name: %s\n",
		    exist_ptr->oid_fleshed_str, exist_ptr->name,
		    add_ptr->name);
            if (agent) {
	        fprintf(stderr, "method routine stubs cannot be generated\n");
                agent_errors++;
                agent = 0;
            }
            fprintf(stderr, "\n");

            if (fpmerge) {
#ifndef SR_SIEMENS_ICN_WN_CC_KEC
	        fprintf(fpmerge, "%s: Warning: Duplicate OIDs with different names.\n", prog_name);
	        fprintf(fpmerge, "The first name will appear before the second name.\n");
#else /* SR_SIEMENS_ICN_WN_CC_KEC */
	        fprintf(fpmerge, "%s: ERROR: Duplicate OIDs with different names.\n", prog_name);
#endif /* SR_SIEMENS_ICN_WN_CC_KEC */
	        fprintf(fpmerge, "OID: %s, First name: %s, Second name: %s\n\n",
	    	        exist_ptr->oid_fleshed_str, exist_ptr->name,
		        add_ptr->name);
            }
	    add_ptr->next_sorted = exist_ptr->next_sorted;
	    exist_ptr->next_sorted = add_ptr;
#ifndef SR_SIEMENS_ICN_WN_CC_KEC
            merge_warnings++;
#else /* SR_SIEMENS_ICN_WN_CC_KEC */
            merge_errors++;
#endif /* SR_SIEMENS_ICN_WN_CC_KEC */
	    return (exist_ptr);
	}
    }				/* end of if duplicate */
    if (cc < 0) {
	exist_ptr->next_sorted = insert_sort_data(exist_ptr->next_sorted, add_ptr);
	return (exist_ptr);
    }
    /* (cc > 0) */

    add_ptr->next_sorted = exist_ptr;
    return (add_ptr);
}

/*
 * make_families_with_valid_augments_valid
 */
void 
make_families_with_valid_augments_valid()
{
    struct OID_INFO *ptr;
    struct OID_INFO *g1, *augments_list;

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
        ptr->num_entries = 0;
        if (ptr->family_root == 1) {
            if (!ptr->augments) {
                /*
                 * if this group is not valid but is augmented, and
                 * if any of the objects in the augmenting table is valid,
                 * then mark this group as being valid
                 */
                 if (ptr->augmented) {
                     for (g1 = sorted_root; g1; g1 = g1->next_sorted) {
                         if (g1->augments) {
                             if (strcmp(g1->augments, ptr->name) == 0) {
                                 augments_list = g1->next_family_entry;
                                 while (augments_list) {
                                     if (augments_list->valid) {
                                         ptr->valid = TRUE;
                                     }
                                     augments_list = 
                                            augments_list->next_family_entry;
                                 }
                             }
                        }
                    }
                }
            }
        }
    }
}

/*
 * count_family_entries: count the number of entries in each family and
 * store the result in ptr->num_entries
 */
int 
count_family_entries()
{
    int found;
    struct OID_INFO *ptr, *temp_ptr;
    struct OID_INFO *g1, *augments_list;
    struct _index_array *index_array, *temp_index;

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
        ptr->num_entries = 0;
        if ((ptr->family_root == 1) && (ptr->valid)) {
            if (!ptr->augments) {

                /*
                 * count the columns in the table
                 */
                temp_ptr = ptr->next_family_entry;
                while (temp_ptr) {
                    if (temp_ptr->valid) {
                        ptr->num_entries = ptr->num_entries + 1;
                    }
                    temp_ptr = temp_ptr->next_family_entry;
                }

                /*
                 * if this group is augmented, count the columns in the
                 * augmenting table
                 */
                 if (ptr->augmented) {
                     for (g1 = sorted_root; g1; g1 = g1->next_sorted) {
                         if (g1->augments) {
                             if (strcmp(g1->augments, ptr->name) == 0) {
                                 augments_list = g1->next_family_entry;
                                 while (augments_list) {
                                     if (augments_list->valid) {
                                         ptr->num_entries = ptr->num_entries+1;
                                     }
                                     augments_list = 
                                            augments_list->next_family_entry;
                                 }
                             }
                        }
                    }
                }

                /*
                 * count indexes that are not defined in this table
                 */
                get_index_array(ptr->name, &index_array);
                if (index_array) {
                    /* for each index for this table */
                    for (temp_index = index_array; temp_index->name; temp_index++) {
                        /*
                         * first, see if this entry is already in the
                         * file (i.e. if is in this table).
                         */
                        found = 0;
                        temp_ptr = ptr->next_family_entry;
                        while (temp_ptr) {
                            if (strcmp(temp_ptr->name, temp_index->name) == 0) {
                                found = 1;
                            }
                            temp_ptr = temp_ptr->next_family_entry;
                        }
 
                        /*
                         * if is is not in the table, then count it now
                         */ 
                         if (!found) {
                             ptr->num_entries = ptr->num_entries + 1;
                         }
                    }       /* for(temp_index... */
                }       /* if(index_array) */
            }
        }
    }
    return 1;
}

int
group_nodes_by_family()
{
    struct OID_INFO *ptr, *temp_ptr;

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
	ptr->next_family = ptr->next_family_entry = NULL;
#ifdef APO_LEVEL_2
	ptr->next_family_entry_agg = NULL;
	ptr->first_family_entry_agg = NULL;
#endif /* APO_LEVEL_2 */
	ptr->family_root = 0;
	ptr->group_read_write = ptr->read_write = 0;
	ptr->group_read_create = 0;
    }

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {

#ifdef DAR_XXX
	/*
	 * using this statement will cause not-accessable items (in
	 * particular, entries in an INDEX clause of SNMPv2 MIBs) to be
	 * excluded from the structure in the type file (snmptype.h).
	 */
	if ((ptr->oid_access != NULL) &&
	    (strcmp(ptr->oid_access, NOT_ACCESSIBLE_STR) != 0)) {	/* } */
#else				/* DAR_XXX */
	if ((ptr->oid_access != NULL) && (ptr->oid_type != NULL) &&
            (isAggregateType(ptr->oid_type) == 0)) {
#endif				/* DAR_XXX */

            if (ptr->parent_ptr == NULL) {
                printf("ERROR: object %s appears to have no parent\n", ptr->name);
                exit(-1);
            }
	    ptr->parent_ptr->family_root = 1;

#if 0
	    if (minimal_v) {
		/* min_vfunc will be set later, to reflect whether the
		   minimized v_ functions support this family or not */
		ptr->parent_ptr->minv_supported = MINV_NOT_DETERMINED;
	    }
#endif /* 0 */

	    temp_ptr = ptr->parent_ptr;
	    while (temp_ptr->next_family_entry) {
		temp_ptr = temp_ptr->next_family_entry;
	    }
	    temp_ptr->next_family_entry = ptr;

	}
    }

#ifdef APO_LEVEL_2
    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {

	if ((ptr->oid_access != NULL) && (ptr->oid_type != NULL)) {
	    if ( (strcmp(ptr->oid_type, "Aggregate") != 0) ||
                 (strcmp(ptr->next_sorted->oid_type, "Aggregate") == 0)) {

	        temp_ptr = ptr->parent_ptr->first_family_entry_agg;
                if (temp_ptr) {
	            while (temp_ptr->next_family_entry_agg) {
	    	        temp_ptr = temp_ptr->next_family_entry_agg;
	            }
	            temp_ptr->next_family_entry_agg = ptr;
                } else {
	            ptr->parent_ptr->first_family_entry_agg = ptr;
                }
	    }
	}
    }
#endif /* APO_LEVEL_2 */

    /*
     * now determine if the item is read-only and if the family is read-only
     */
    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
	if (ptr->family_root == 1) {
	    temp_ptr = ptr->next_family_entry;
	    while (temp_ptr) {
		if ((temp_ptr->oid_access) &&
		    strcmp(temp_ptr->oid_access, READ_ONLY_STR) &&
		    strcmp(temp_ptr->oid_access, ACCESSIBLE_FOR_NOTIFY_STR) &&
		    strcmp(temp_ptr->oid_access, NOT_ACCESSIBLE_STR)) {
		    temp_ptr->read_write = temp_ptr->group_read_write = 1;
		    ptr->group_read_write = ptr->read_write = 1;
		}
		if (strcmp(temp_ptr->oid_access, READ_CREATE_STR) == 0) {
		    temp_ptr->group_read_create = 1;
		    ptr->group_read_create = 1;
		}
		temp_ptr = temp_ptr->next_family_entry;
	    }
	}
    }
    return 1;
}

int
cmp_oid(ptr1, ptr2)
    OID            *ptr1, *ptr2;
{
    int             i;
    int             min = ((ptr1->length < ptr2->length) ? ptr1->length : ptr2->length);

    for (i = 0; i < min; i++) {
	if (ptr1->oid_ptr[i] != ptr2->oid_ptr[i])
	    return (ptr1->oid_ptr[i] - ptr2->oid_ptr[i]);
    }

    /*
     * they were identical as far as they went, now check which was longer
     */
    if (ptr1->length > ptr2->length)
	return (1);
    if (ptr1->length < ptr2->length)
	return (-1);

    return (0);
}


/*
 * check the length of a string for the defs file to determine if is is too
 * long for some C compilers (31 characters). only used if -short is given on
 * the command line
 */
void
check_defs_size(word, enumeration_list, name)
    char           *word;
    struct enumerations *enumeration_list;
    char           *name;
{
    struct enumerations *temp;
    char            buf2[512];

    if (strlen(word) > (unsigned) 31) {
	printf("truncating %s to ", word);
	word[31] = '\0';
	printf("%s\n", word);
    }
    /*
     * make sure no other names will conflict with this shortened name
     */
    for (temp = enumeration_list->next; temp; temp = temp->next) {
	sprintf(buf2, "D_%s_%s", name, temp->name);
	if (strncmp(word, buf2, 31) == 0) {
	    fprintf(stderr, "\nerror: Two names map to %s\n", word);
	    fprintf(stderr, "you must change names or run postmosy ");
	    fprintf(stderr, "without -short option\n\n");
	    exit(-1);
	}
    }
}

/*
 * check the length of a string to determine if is is too long for some C
 * compilers (31 characters - 2 for I_, N_, etc). only used if -short is
 * given on the command line
 */
void
shorten_names()
{
    struct OID_INFO *ptr, *ptr2;
    int             count;

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
	if (strlen(ptr->name) > (unsigned) 29) {
	    printf("truncating %s to ", ptr->name);
	    ptr->name[29] = '\0';
	    printf("%s\n", ptr->name);

	    /*
	     * make sure no other names will conflict with this shortened
	     * name
	     */
	    count = 0;
	    for (ptr2 = sorted_root; ptr2 != NULL; ptr2 = ptr2->next_sorted) {

		if (strncmp(ptr2->name, ptr->name, 29) == 0)
		    count++;
		if (count > 1) {
		    fprintf(stderr, "\nerror: Two names map to %s\n", ptr->name);
		    fprintf(stderr, "you must change names or run postmosy ");
		    fprintf(stderr, "without -short option\n\n");
		    exit(-1);
		}
	    }
	}			/* if */
    }				/* for (ptr = ... */
}

/* replace '-' with '_' in word */
void
remove_hyphens(word)
    char           *word;
{
    int             i = 0;

    while (word[i]) {
	if (word[i] == '-')
	    word[i] = '_';
	i++;
    }
}

int
build_index_table(name, index)
    char           *name;
    char           *index;
{
    struct index_table *table;
    struct OID_INFO *ptr;

    table = (struct index_table *) malloc(sizeof(struct index_table));
    table->name = (char *) malloc(strlen(name) + 1);
    table->index = (char *) malloc(strlen(index) + 1);
    table->buff = (char *) malloc(strlen(index) + 1);
    if (!table || !table->index || !table->name || !table->buff) {
	fprintf(stderr, "build_index_table(): malloc error\n");
	exit(-1);
    }
    strcpy(table->name, name);
    strcpy(table->index, index);
    strcpy(table->buff, index);
    table->types = NULL;

    table->next = index_table;
    index_table = table;

    table->ptr = NULL;
    for (ptr = root; ptr != NULL; ptr = ptr->next_sorted) {
        if (strcmp(table->name, ptr->name) == 0) {
            table->ptr = ptr;
        }
    }
    remove_hyphens(table->name);
    if (table->ptr == NULL) {
        fprintf(stderr, "unable to find table for index %s\n", index);
        exit(-1);
    }
    return 1;
}

int
process_index_table()
{
    struct OID_INFO *ptr, *temp_ptr;
    struct index_table *table;
    char           *ch, *end;
    char           *temp_str;
    char            buff[512], buff2[512];
    int             i, x;
    int             idx;
    struct _index_array *index_array, *temp_index;

    for (table = index_table; table; table = table->next) {
	buff[0] = '\0';
	buff2[0] = '\0';
	ch = table->buff;
	while ((*ch == ' ') || (*ch == '{') || (*ch == '"'))
	    ch++;

	i = 0;
	do {
	    strcat(buff, ", ");
	    end = ch;
	    while ((*end != ' ') && (*end != ',') && (*end != '"')){
         /* change - to _ so indices with hyphens will compare correctly */
               if(*end == '-'){
                  *end = '_';
               }
		end++;
            }
	    *end = '\0';
	    if (*ch == '*') {
		table->index_array[i].IndexType = SR_IMPLIED;
		ch++;
	    }
	    /* for backward compatibility with mosy 7.0 */
	    else if (strcmp(ch, "IMPLIED") == 0) {
		table->index_array[i].IndexType = SR_IMPLIED;
		ch = end;
		ch++;
		while (*ch == ' ')
		    ch++;
		end = ch;
		while ((*end != ' ') && (*end != ',')){
         /* change - to _ so indices with hyphens will compare correctly */
                   if(*end == '-'){
                      *end = '_';
                   }
		    end++;
                }
		*end = '\0';
	    }
	    else {
		if ((table->index_array[i].IndexType = get_index_type(ch)) == 0) {
		    printf("get_index_type(%s) failed\n", ch);
		    return (-1);
		}
	    }

	    if ((temp_str = get_table_types(ch)) == NULL) {
		printf("temp_str = get_table_types(%s) failed\n", ch);
		return (-1);
	    }
	    strcat(buff, temp_str);
	    strcat(buff2, temp_str);
	    free(temp_str);
	    strcat(buff2, " ");
	    strcat(buff2, ch);
	    strcat(buff2, "\n");
	    table->index_array[i].name = ch;
	    if ((table->index_array[i].c_type = get_table_types(ch)) == NULL) {
		printf("c_type = get_table_types(%s) failed\n", ch);
		return (-1);
	    }
	    if ((table->index_array[i].asn1_type = get_table_asn1_types(ch)) == NULL) {
		printf("get_table_asn1_types(%s) failed\n", ch);
		return (-1);
	    }
	    x = get_index_length(ch);
	    table->index_array[i].length = x;
	    table->index_array[i].InThisTable = 0;
	    i++;

	    ch = ++end;
	    while (*ch == ' ')
		ch++;
	} while ((*ch != '}') && (*ch != '"') && (*ch != '\n'));

	table->index_array[i].name = NULL;
	table->index_array[i].c_type = NULL;
	table->index_array[i].asn1_type = NULL;

	if ((table->types = (char *) malloc(strlen(buff) + 1)) == NULL) {
	    fprintf(stderr, "process_index_table: malloc error\n");
	    exit(-1);
	}
	if ((table->name_type = (char *) malloc(strlen(buff2) + 1)) == NULL) {
	    fprintf(stderr, "process_index_table: malloc error\n");
	    exit(-1);
	}
	strcpy(table->types, buff);
	strcpy(table->name_type, buff2);

    }				/* end for(table... */

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
        if (ptr->family_root == 1) {
            idx = get_index_array(ptr->name, &index_array);
            temp_index = index_array;
            while (temp_index && temp_index->name) {
                temp_ptr = ptr->next_family_entry;
                while (temp_ptr) {
                    if (strcmp(temp_ptr->name, temp_index->name) == 0) {
                        temp_index->InThisTable = 1;
                    }
                    temp_ptr = temp_ptr->next_family_entry;
                }
                temp_index++;
            }
        }
    }

    return 1;
}

char           *
get_table_asn1_types(index)
    char           *index;
{
    struct OID_INFO *ptr;

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
	if (strcmp(index, ptr->name) == 0) {
	    return (ptr->oid_prim_type);
	}
    }
    return NULL;
}

int
get_index_type(index)
    char           *index;
{
    struct OID_INFO *ptr;

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
	if (strcmp(index, ptr->name) == 0) {
	    if (strcmp(ptr->oid_prim_type, "NetworkAddress") == 0) {
		return 5;
	    }
	    if (!is_oid(ptr->oid_prim_type) && !is_octetstring(ptr->oid_prim_type)) {
		return SR_DONT_CARE;
	    }
	    /*
	     * if no size range is given, then we assume that that it is
	     * variable length. This is usually the correct assumption,
	     * however, some MIB designers don't include a SIZE clause when
	     * they should
	     */
	    if (ptr->size == NULL) {
                /* see if this is a textual convention, if so, check the 
                 * size constraint of the textual convention
                 */

    		struct TC      *temp_tc;

  		temp_tc = tc;
    		for (temp_tc = tc; temp_tc; temp_tc = temp_tc->next) {
        	    if (strcmp(ptr->oid_type, temp_tc->name) == 0) {
			ptr->size = temp_tc->size;
        	    }
		}

                /* otherwise, we assume it is variable length */
		if (ptr->size == NULL) {
		    return SR_ASSUME_VARIABLE;
		}
	    }

	    /*
	     * if ptr->size->next is NULL, then there is only one line
	     * indicating the size. Otherwise we have more size information
	     * in the linked list (For example, if the SIZE clause is SIZE (0
	     * | 10..15)) 
	     */
	    if (ptr->size->next == NULL)
		if (ptr->size->min == ptr->size->max) {
		    return ptr->size->min;
		}
		else {
		    return SR_VARIABLE;
		}
	    else {
		return SR_VARIABLE;
	    }
	}
    }
    return 0;
}

char           *
get_table_types(index)
    char           *index;
{
    struct OID_INFO *ptr;
    char           *temp;
    char           *return_ptr;

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
	if (strcmp(index, ptr->name) == 0) {
	    temp = find_type2(ptr->oid_prim_type);
	    return_ptr = (char *) malloc(strlen(temp) + 1);
	    sprintf(return_ptr, "%s", temp);
	    return (return_ptr);
	}
    }
    return NULL;
}

char           *
get_index_type_from_table(name)
    char           *name;
{
    struct index_table *table;

    for (table = index_table; table; table = table->next) {
	if (strcmp(name, table->name) == 0) {
	    return (table->types);
	}
    }
    return ("\0");
}

int ctype_is_pointer(char *s)
{
    return strchr(s, '*') ? 1 : 0;
}

/* a slightly modified version of find_type() */
char           *
find_type2(oid_type)
    char           *oid_type;
{
    static char     type[128];
    char            buff[128];

    strcpy(buff, oid_type);

    /* Note that the returned C type always contains an 
       asterisk if it is a pointer type.  The function
       ctype_is_pointer() above relies on this, so if you
       ever invalidate that assumption, you have to update
       ctype_is_pointer(). */

    if (strcmp(buff, "DisplayString") == 0) {
	strcpy(type, "OctetString *");
	return (type);
    }
    else if (strcmp(buff, "OctetString") == 0) {
	strcpy(type, "OctetString *");
	return (type);
    }
    else if (strcmp(buff, "PhysAddress") == 0) {
	strcpy(type, "OctetString *");
	return (type);
    }
    else if (strcmp(buff, "NetworkAddress") == 0) {
	/* strcpy(type,"char *"); */
	strcpy(type, "OctetString *");
	return (type);
    }
    else if (strcmp(buff, "IpAddress") == 0) {
	strcpy(type, "SR_UINT32");
	return (type);
    }
    else if (strcmp(buff, "ObjectID") == 0) {
	strcpy(type, "OID *");
	return (type);
    }
    else if (strcmp(buff, "TimeTicks") == 0) {
	strcpy(type, "SR_UINT32");
	return (type);
    }
    else if (strcmp(buff, "INTEGER") == 0) {
	strcpy(type, "SR_INT32");
	return (type);
    }
    else if (strcmp(buff, "Gauge") == 0) {
	strcpy(type, "SR_UINT32");
	return (type);
    }
    else if (strcmp(buff, "Counter") == 0) {
	strcpy(type, "SR_UINT32");
	return (type);
    }

    else if (strcmp(buff, "Counter32") == 0) {
	strcpy(type, "SR_UINT32");
	return (type);
    }
    else if (strcmp(buff, "Gauge32") == 0) {
	strcpy(type, "SR_UINT32");
	return (type);
    }
    else if (strcmp(buff, "Unsigned32") == 0) {
	strcpy(type, "SR_UINT32");
	return (type);
    }
    else if (strcmp(buff, "NsapAddress") == 0) {
	strcpy(type, "OctetString *");
	return (type);
    }
    else if (strcmp(buff, "UInteger32") == 0) {
	strcpy(type, "SR_UINT32");
	return (type);
    }
    else if (strcmp(buff, "Integer32") == 0) {
	strcpy(type, "SR_INT32");
	return (type);
    }
    if (strcmp(buff, "Bits") == 0) {
	strcpy(type, "OctetString *");
	return (type);
    }
    else if (strcmp(buff, "Counter64") == 0) {
	strcpy(type, "Counter64 *");
	return (type);
    }
    else if (strcmp(buff, "Opaque") == 0) {
	strcpy(type, "OctetString *");
	return (type);
    }

    else {
	fprintf(stderr, "find_type2(): unknown type: %s\n", buff);
	exit(-1);
    }
    return ("");
}

int
get_index_length(name)
    char           *name;
{
    char            buff[128], *type;
    struct OID_INFO *ptr;


    type = NULL;
    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
	if (strcmp(name, ptr->name) == 0) {
	    type = ptr->oid_prim_type;
	}
    }
    if (!type)
	return (-2);

    strcpy(buff, type);

    if ((strcmp(buff, "INTEGER") == 0) || (strcmp(buff, "TimeTicks") == 0) ||
	(strcmp(buff, "Gauge") == 0) || (strcmp(buff, "Counter") == 0) ||
	(strcmp(buff, "Counter32") == 0) || (strcmp(buff, "Gauge32") == 0) ||
        (strcmp(buff, "Unsigned32") == 0) ||(strcmp(buff, "Integer32") == 0) ) {

	return (1);

    }
    else if ((strcmp(buff, "IpAddress") == 0)) {
	return (4);

    }
    else if ((strcmp(buff, "NetworkAddress") == 0)) {
	return (5);

    }
    else if ((strcmp(buff, "ObjectID") == 0)) {
	return (-1);
    }
    else if ((strcmp(buff, "DisplayString") == 0) ||
	     (strcmp(buff, "PhysAddress") == 0)) {
	return (-1);

    }
    else if ((strcmp(buff, "OctetString") == 0) ||
             (strcmp(buff, "Bits") == 0) ||
	     (strcmp(buff, "NsapAddress") == 0)) {
	return (-1);

    }
    else if ((strcmp(buff, "Counter64") == 0) ||
	     (strcmp(buff, "UInteger32") == 0)) {
	return (-1);

    }
    else if ((strcmp(buff, "Opaque") == 0)) {
	return (-1);
    }
    else {
	return (-1);
    }

}

int
build_augments_table(name, augments)
    char           *name;
    char           *augments;
{
    struct augments_table *table;

    table = (struct augments_table *) malloc(sizeof(struct augments_table));
    table->name = (char *) malloc(strlen(name) + 1);
    table->augments = (char *) malloc(strlen(augments) + 1);
    if (!table || !table->augments || !table->name) {
	fprintf(stderr, "build_augments_table(): malloc error\n");
	exit(-1);
    }
    strcpy(table->name, name);
    remove_hyphens(table->name);
    strcpy(table->augments, augments);

    table->next = augments_table;
    augments_table = table;
    return 1;
}

int
process_augments_table()
{
    struct augments_table *table;
    struct OID_INFO *ptr;
    char           *ch;

    for (table = augments_table; table; table = table->next) {
	/* strip out '[' and ']' */
	ch = table->augments;
	while ((*ch == ' ') || (*ch == '['))
	    ch++;
	strcpy(table->augments, ch);
	ch = table->augments;
	while ((*ch != '\0') && (*ch != ' ') && (*ch != ']'))
	    ch++;
	*ch = '\0';
    }				/* end for(table... */

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
	ptr->augments = NULL;
	ptr->augmented = 0;
	for (table = augments_table; table; table = table->next) {
	    if (strcmp(table->name, ptr->name) == 0) {
		ptr->augments = (char *) malloc(strlen(table->augments) + 1);
		if (ptr->augments == NULL) {
		    fprintf(stderr, "build_augments_table: malloc failed\n");
		    exit(-1);
		}
		strcpy(ptr->augments, table->augments);
	    }
	    if (strcmp(table->augments, ptr->name) == 0) {
		ptr->augmented = 1;
	    }
	}
    }
    return 1;
}

/* print instructions for snmpsupp.h */
int
print_support_instructions(fp_support)
    FILE           *fp_support;
{
    /* make a header for the support file */
    fprintf(fp_support, "/*\n");
    fprintf(fp_support,
	    " * If your implementation does NOT support a MIB variable, add a line in\n");
    fprintf(fp_support,
	    " * the undefs.h file containing #undef I_that-variable-name.  For example,\n");
    fprintf(fp_support,
    " * if you do not support the sysDescr variable in the system family\n");
    fprintf(fp_support,
	    " * add a line:\n");
    fprintf(fp_support, " *\n");
    fprintf(fp_support,
	    " * #undef I_sysDescr\n");
    fprintf(fp_support, " *\n");
    fprintf(fp_support, " *\n");
    fprintf(fp_support, " */\n");
    fprintf(fp_support, "\n\n");
    return 1;
}

/*
 * print #ifdefs for protection against multiple inclusion of header files
 */
int
print_start_protection(fp, base, name)
    FILE           *fp;
    char           *base;
    char           *name;
{
    static char     format[] = "SR_%s%s_H";
    char           *buff;
    int             bufsize;
    int             i = 0;

    bufsize = strlen(base) + strlen(name) + strlen(format) + 1;
    buff = (char *)malloc(bufsize);
    if (buff == NULL) {
	fprintf(stderr, "print_start_protection: malloc error\n");
	return -1;
    }
    sprintf(buff, format, base, name);
    while (buff[i]) {
	if (islower((unsigned char)buff[i])) {
	    buff[i] = toupper(buff[i]);
        }
        if (buff[i] == '-') {
            buff[i] = '_';
        }
	i++;
    }
#define CPLUSPLUS_STRING1 "\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n"

    fprintf(fp, "#ifndef %s\n", buff);
    fprintf(fp, "#define %s\n\n", buff);
    fprintf(fp, CPLUSPLUS_STRING1);
    free(buff);
    return 1;
}

/*
 * print #endif for protection against multiple inclusion of header files
 */
int
print_end_protection(fp, base, name)
    FILE           *fp;
    char           *base;
    char           *name;
{
    static char     format[] = "SR_%s%s_H";
    char           *buff;
    int             bufsize;
    int             i = 0;

#define CPLUSPLUS_STRING2 "\n\
#ifdef __cplusplus\n\
}\n\
#endif\n\n"
  
    fprintf(fp, CPLUSPLUS_STRING2);

    bufsize = strlen(base) + strlen(name) + strlen(format) + 1;
    buff = (char *)malloc(bufsize);
    if (buff == NULL) {
	fprintf(stderr, "print_end_protection: malloc error\n");
	return -1;
    }
    sprintf(buff, format, base, name);
    while (buff[i]) {
	if (islower((unsigned char)buff[i]))
	    buff[i] = toupper(buff[i]);
	i++;
    }
    fprintf(fp, "/* DO NOT PUT ANYTHING AFTER THIS #endif */\n");
    fprintf(fp, "#endif /* %s */\n", buff);
    free(buff);
    return 1;
}

extern MIB_TREE *default_mib_tree;

int 
build_mib_table()
{
    struct OID_INFO *ptr;
    OID_TREE_ELE   *otep, *oteps, *oteps_tail;
    OID             oid;

    oid.length = 0;
    oid.oid_ptr = NULL;
    default_mib_tree = CreateMIBTree("default", &oid);
    if (default_mib_tree == NULL) {
        fprintf(stderr, "build_mib_table: malloc error\n");
        return -1;
    }

    /* count the number of entries */
    oteps = oteps_tail = NULL;
    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
        otep = NewOID_TREE_ELE();
        if (otep == NULL) {
            fprintf(stderr, "build_mib_table: malloc error\n");
            return -1;
        }
        otep->oid_name = (char *)malloc(strlen(ptr->name) + 1);
        otep->oid_number_str = (char *)malloc(strlen(ptr->oid_fleshed_str) + 1);
        if ((otep->oid_name == NULL) || (otep->oid_number_str == NULL)) {
            fprintf(stderr, "build_mib_table: malloc error\n");
            return -1;
        }
	strcpy(otep->oid_name, ptr->name);
	strcpy(otep->oid_number_str, ptr->oid_fleshed_str);
	if (ptr->oid_access) {
	    otep->access = AccessStringToShort(ptr->oid_access);
	}
	else {
	    otep->access = 0;
	}
	if (ptr->oid_prim_type) {
	    otep->type = TypeStringToShort(ptr->oid_prim_type);
	}
	else {
	    otep->type = 0;
	}
        if (oteps_tail == NULL) {
            oteps_tail = oteps = otep;
        } else {
            oteps_tail->next = otep;
            oteps_tail = otep;
        }
    }

    while (oteps != NULL) {
        otep = oteps;
        oteps = oteps->next;
        otep->next = NULL;
        if (AddOTEToMibTree(default_mib_tree, otep, SR_COL_REPLACE) != otep) {
            ReleaseOID_TREE_ELE(otep);
        }
    }

    return 1;
}

FILE           *
open_file(pre, post)
    char           *pre;
    char           *post;
{
    FILE           *fp;
    char           *name;
    int             bufsize;
 
    bufsize =strlen(pre) + strlen(post) +1;
    name =(char*) malloc(bufsize);
 
    if (name == NULL) {
      fprintf(stderr, "open_file: malloc error\n");
      exit(-1);
    }

    strcpy(name, pre);
    strcat(name, post);
    if ((fp = fopen(name, "w")) == NULL) {
	fprintf(stderr, "%s: can't open file: %s\n", prog_name, name);
	perror("");
	exit(-1);
    }
    free(name);

    return (fp);
}

FILE           *
open_file_read_only(pre, post)
    char           *pre;
    char           *post;
{
    FILE           *fp;
    char           *name;
    int             bufsize;
 
    bufsize =strlen(pre) + strlen(post) +1;
    name =(char*) malloc(bufsize);
 
    if (name == NULL) {
      fprintf(stderr, "open_file_read_only: malloc error\n");
      exit(-1);
    }

    strcpy(name, pre);
    strcat(name, post);
    if ((fp = fopen(name, "r")) == NULL) {
        fprintf(stderr, "%s: can't open file: %s\n", prog_name, name);
        perror("");
        exit(-1);
    }
    free(name);

    return (fp);
}

/*
 * add the primitive type to the OID_INFO structure. For example, the
 * primitive type for DisplayString is OCTET STRING.
 * Also add size ranges and enumerations to the OID_INFO structure from
 * the TC structure. Note: if a size range is given when the object is
 * defined, this overrides the size in the TC.
 */
void
get_prim_type()
{
    static char     buff[128];
    struct TC      *temp_tc;
    struct OID_INFO *ptr;
    char           *prim_type;

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {

	if (ptr->oid_type) {
	    strcpy(buff, ptr->oid_type);
	    temp_tc = tc;
	    while (temp_tc) {
		if (strcmp(buff, temp_tc->name) == 0) {
		    strcpy(buff, temp_tc->type);
		    ptr->enumer = temp_tc->enumer;
		    if (ptr->size == NULL) {
			ptr->size = temp_tc->size;
		    }
                    temp_tc = tc;
		} else {
                    temp_tc = temp_tc->next;
		}
	    }
	    prim_type = (char *) malloc(sizeof(buff) + 1);
	    strcpy(prim_type, buff);
	    ptr->oid_prim_type = prim_type;
	}
        /* This is just to make sure that the type is valid */
        if (!inputinfo && !remove_subtree) {
            if ((ptr->oid_type) && (isAggregateType(ptr->oid_type) == 0)) {
                find_type(buff, ptr->name, ptr->oid_prim_type, NULL);
	    }
        }
    }
}

/*
 * similar to get_prim_type, but for only one object
 */
void
get_one_prim_type(struct OID_INFO *ptr)
{  
    static char     buff[128];
    struct TC      *temp_tc;
    char           *prim_type;

        if (ptr->oid_type) {
            strcpy(buff, ptr->oid_type);
            temp_tc = tc;
            while (temp_tc) {
                if (strcmp(buff, temp_tc->name) == 0) {
                    strcpy(buff, temp_tc->type);
                    ptr->enumer = temp_tc->enumer;
                    if (ptr->size == NULL) {
                        ptr->size = temp_tc->size;
                    }
                    temp_tc = tc;
                } else {  
                    temp_tc = temp_tc->next;
                }
            }
            prim_type = (char *) malloc(sizeof(buff) + 1);
            strcpy(prim_type, buff);
            ptr->oid_prim_type = prim_type;
        }
}   

/*
 * return 1 if the type is SNMPv2 only, otherwise return 0
 */
int
get_version_info(type)
    char           *type;
{
    char            buff[128];

    strcpy(buff, type);

    if ((strcmp(buff, "Counter64") == 0)) {
	return 1;
    }
    else {
	return 0;
    }
}

int
is_oid(type)
    char           *type;
{
    char            buff[128];

    strcpy(buff, type);

    if ((strcmp(buff, "ObjectID") == 0)) {
	return 1;

    }
    else {
	return 0;
    }
}

int
is_octetstring(type)
    char           *type;
{
    char            buff[128];

    strcpy(buff, type);

    if ((strcmp(buff, "NetworkAddress") == 0) ||
	(strcmp(buff, "PhysAddress") == 0)) {

	return 1;
    }
    else if ((strcmp(buff, "OctetString") == 0) ||
	     (strcmp(buff, "DisplayString") == 0) ||
	     (strcmp(buff, "Bits") == 0) ||
	     (strcmp(buff, "Opaque") == 0) ||
	     (strcmp(buff, "NsapAddress") == 0)) {
	return 1;
    }
    else if ((strcmp(buff, "Counter64") == 0)) {
	return 0;
    }
    else {
	return 0;
    }
}

int
is_counter(type)
    char           *type;
{
    char            buff[128];

    strcpy(buff, type);

    if ((strncmp(buff, "Counter", strlen("Counter")) == 0)) {
	return 1;
    } else {
	return 0;
    }
}

int
is_counter64(char *type)
{
    char            buff[128];
                                                                                
    strcpy(buff, type);
                                                                                
    if ((strncmp(buff, "Counter64", strlen("Counter64")) == 0)) {
        return 1;
    } else {
        return 0;
    }
}

int
is_bits(char *type)
{
    char            buff[128];

    strcpy(buff, type);

    if ((strncmp(buff, "Bits", strlen("Bits")) == 0)) {
	return 1;
    } else {
	return 0;
    }
}


static void
checkargstring()
{
  static char *newline = "\n * ";

  if (currentlen >= 60) {
    strcat(postmosyargs, newline);
    currentlen = 0;
  }
}

/* this routine gets the next family of mib objects */
struct OID_INFO *
next_valid_family(ptr)
    struct OID_INFO *ptr;
{
    /* look for the next valid family */
    while (ptr != NULL) {
	/* is this a family in the current file? */
	if (ptr->family_root == 1

	    && ptr->augments == 0

	    && ptr->valid != 0) {
	    /* this is a valid family, get out now */
	    break;
	}

	/* on to the next object */
	ptr = ptr->next_sorted;
    }

    /* return a pointer to the head of the family, or null */
    return (ptr);
}

static void
calculate_family_numbers(struct OID_INFO *root, int first_family)
{
    int family_nr = first_family;
    struct OID_INFO *ptr, *member;

    for (ptr = root; (ptr = next_valid_family(ptr)) != NULL;
         ptr = ptr->next_sorted) {
	ptr->family_nr = family_nr;
        for (member = ptr->next_family_entry; member; 
             member = member->next_family_entry) {
	    member->family_nr = family_nr;
        }
        family_nr++;
    }
}

