/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Ssh client program.  This program can be used to log into a remote machine.
 * The software supports strong authentication, encryption, and forwarding
 * of X11, TCP/IP, and authentication connections.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 * Copyright (c) 1999 Niels Provos.  All rights reserved.
 * Copyright (c) 2000, 2001, 2002, 2003 Markus Friedl.  All rights reserved.
 *
 * Modified to work with SSL by Niels Provos <provos@citi.umich.edu>
 * in Canada (German citizen).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "includes.h"
RCSID("$OpenBSD: ssh.c,v 1.257 2005/12/20 04:41:07 dtucker Exp $");

#include <openssl/evp.h>
#include <openssl/err.h>

#include "ssh.h"
#include "ssh1.h"
#include "ssh2.h"
#include "compat.h"
#include "cipher.h"
#include "xmalloc.h"
#include "packet.h"
#include "buffer.h"
#include "bufaux.h"
#include "channels.h"
#include "key.h"
#include "authfd.h"
#include "authfile.h"
#include "pathnames.h"
#include "dispatch.h"
#include "clientloop.h"
#include "log.h"
#include "readconf.h"
#include "sshconnect.h"
#include "misc.h"
#include "kex.h"
#include "mac.h"
#include "sshpty.h"
#include "match.h"
#include "msg.h"
#include "monitor_fdpass.h"
#include "uidswap.h"

#ifdef SMARTCARD
#include "scard.h"
#endif

extern int connection_out[], connection_in[], inout_v1[];    /* SSHv1 input/output socket */

extern char *__progname;
extern u_char *sshc_session_id2[];
extern u_int sshc_session_id2_len[];
extern void  *sshdGlobalSema;
static int ssh_emweb_socket_connect();
static int sshInternalCliSocketConnect();
static pid_t sshc_taskid[L7_OPENSSH_MAX_CONNECTIONS];

/* Flag indicating whether debug mode is on.  This can be set on the command line. */
#ifndef L7_SSHD
static int debug_flag = 0;
#endif /* L7_SSHD */

/* Flag indicating whether a tty should be allocated */
int tty_flag = 0;
int no_tty_flag = 0;
int force_tty_flag = 0;

/* don't exec a shell */
int no_shell_flag = 0;

/*
 * Flag indicating that nothing should be read from stdin.  This can be set
 * on the command line.
 */
int stdin_null_flag = 0;

/*
 * Flag indicating that ssh should fork after authentication.  This is useful
 * so that the passphrase can be entered manually, and then ssh goes to the
 * background.
 */
int fork_after_authentication_flag = 0;

/*
 * General data structure for command line options and options configurable
 * in configuration files.  See readconf.h.
 */
Options client_options;

/* optional user configfile */
char *config = NULL;

/*
 * Name of the host we are connecting to.  This is the name given on the
 * command line, or the HostName specified for the user-supplied name in a
 * configuration file.
 */
char *host;

/* socket address the host resolves to */
struct sockaddr_storage hostaddr;

/* Private host keys. */
Sensitive client_sensitive_data;

/* Original real UID. */
uid_t original_real_uid;
uid_t original_effective_uid;

/* command to be executed */
Buffer command;

/* Should we execute a command or invoke a subsystem? */
int subsystem_flag = 0;

/* # of replies received for global requests */
static int client_global_request_id = 0;

/* pid of proxycommand child process */
pid_t proxy_command_pid = 0;

/* fd to control socket */
int control_fd = -1;

/* Multiplexing control command */
static u_int mux_command = 0;

/* Only used in control client mode */
volatile sig_atomic_t control_client_terminate = 0;
u_int control_server_pid = 0;

/* Prints a help message to the user.  This function never returns. */

#ifndef L7_SSHD
static void
usage(void)
{
    fprintf(stderr,
"usage: ssh [-1246AaCfgkMNnqsTtVvXxY] [-b bind_address] [-c cipher_spec]\n"
"           [-D [bind_address:]port] [-e escape_char] [-F configfile]\n"
"           [-i identity_file] [-L [bind_address:]port:host:hostport]\n"
"           [-l login_name] [-m mac_spec] [-O ctl_cmd] [-o option] [-p port]\n"
"           [-R [bind_address:]port:host:hostport] [-S ctl_path]\n"
"           [-w tunnel:tunnel] [user@]hostname [command]\n"
    );
    exit(255);
}
#endif

static int ssh_session(int cn);
static int ssh_session2(int cn);
static void load_public_identity_files(void);
static void control_client(const char *path);

void ssh_var_init(void)
{
    int i;

    /*
    Set data for SSH that is not 0 default

    sshdExitSema and sshdGlobalSema should already be taken when
    entering this function
    */

    for (i = SSHD_MAX_CONNECTIONS; i < SSH_MAX_CONNECTIONS + SSHD_MAX_CONNECTIONS; i++)
    {
        connection_in[i] = -1;
        connection_out[i] = -1;
        max_packet_size[i] = 32768;

        inout_v1[i] = -1;
    }

}

int ssh_emweb_socket_connect()
{
   int ConnectSock;
   int rc = -1;
   struct sockaddr_in peer;

   ConnectSock = socket(AF_INET, SOCK_STREAM, 0);

    if (ConnectSock < 0)
    {
        debug("couldn't connect to internal socket failed, %d %s\n", errno, strerror(errno));
        return -1;
    }

    peer.sin_family = AF_INET;
    peer.sin_port = htons(L7_SSHC_EMWEB_PORT);
    peer.sin_addr.s_addr = htonl(L7_SSHD_UNSECURE_SERVER_ADDR);
    rc = connect(ConnectSock, (struct sockaddr *)&peer, sizeof(peer));

    if (rc < 0)
    {
        debug("couldn't connect to internal socket, connect failed, %d %s\n", errno, strerror(errno));
        close(ConnectSock);
        return -1;
    }

    debug("Connected to internal socket %d\n", ConnectSock);
    return ConnectSock;
}
int sshInternalCliSocketConnect()
{
  int errno;
  struct sockaddr_in    addr;
  L7_uint32  hostaddr;
  static L7_int32   ssh_client;
  int one = 1;
  struct sockaddr peer;
  int addrlen = sizeof(struct sockaddr);
  int accept_socket;
  static int i = 1;
  struct linger linger={1,1};


  /* This code should be called only once if multiple
     ssh client connections are to be invoked for now
     tested with 1 instance */
  if(i == 1)
  {
  i++;
  ssh_client = socket( AF_INET, SOCK_STREAM, 0 );

  if(ssh_client < 0)
  {
   /* Need to add exit code */
   return -1;
  }

  errno = 0;
  /* VxWorks has setsockopt(int, int, int, char *, int). */
  if ((setsockopt(ssh_client, SOL_SOCKET, SO_REUSEADDR, (char *)&one,
                  sizeof(one))) < 0)
  {
    debug( "SSH setsockopt REUSEADDR failed: %s\n" , strerror(errno));
    return -1;
  }

  /* For internal ssh, listen only on loopback */
  hostaddr = htonl(L7_SSHD_UNSECURE_SERVER_ADDR);
  memset( &addr, 0, sizeof( addr ));
  memcpy(&addr.sin_addr.s_addr, &hostaddr, sizeof(L7_uint32) );
  addr.sin_family = AF_INET;
  addr.sin_port = htons(L7_SSHC_EMWEB_PORT);

  /* bind the internal ssh listening port */
  if (  bind(ssh_client, (struct sockaddr *) &addr, sizeof(addr))
        < 0
     )
  {
    debug( "internal ssh bind failed: %s\n", strerror(errno));
    return -1;
  }

  if ( listen(ssh_client, 4 ) < 0 )
  {
    debug( "internal ssh listen failed: %s\n", strerror(errno));
       return -1;
  }
  }
  accept_socket = accept( ssh_client ,&peer ,&addrlen);

  if(accept_socket < 0)
  {
    return -1;
  }
  debug( "Internal ssh listen succeeded");

  setsockopt(accept_socket, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger));
  return accept_socket;
}

void sshc_session_cleanup(int cn)
{
  if (sshc_session_id2[cn] != NULL)
  {
#if 0 /* already freed */
    xfree(sshc_session_id2[cn]);
#endif
    sshc_session_id2[cn] = NULL;
    sshc_session_id2_len[cn] = 0;
  }
  buffer_free(&command);
  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  connection_in[cn] = connection_out[cn] = -1;
  osapiSemaGive(sshdGlobalSema);
}

/*
 * Main program for the ssh client.
 */
int
ssh_client_main(char *host_name, int address_family, char *user_name,
                char *pass_word, char *cmdstr, int cmdlen, int *cn)
{
#ifndef L7_SSHD
    int i, opt, exit_status;
    char *p, *cp, *line, buf[256];
#else
    int i;
    int client_cn = SSHD_MAX_CONNECTIONS;  /* future client support should find the next available index */
    char *p, *cp, buf[256];
#endif /* L7_SSHD */
#ifndef L7_SSHD
    struct stat st;
#endif /* L7_SSHD */
    struct passwd *pw;
#ifndef L7_SSHD
    int dummy;
    extern int optind, optreset;
    extern char *optarg;
#endif /* L7_SSHD */
    struct servent *sp;
#ifndef L7_SSHD
    Forward fwd;
#endif /* L7_SSHD */
#ifdef L7_SSHD
    int login_rc = -1;
    *cn = client_cn;
#endif /* L7_SSHD */

#ifndef L7_SSHD
    /* Ensure that fds 0, 1 and 2 are open or directed to /dev/null */
    sanitise_stdfd();

    __progname = ssh_get_progname(av[0]);
#endif /* L7_SSHD */
    init_rng();

    /*
     * Save the original real uid.  It will be needed later (uid-swapping
     * may clobber the real uid).
     */
    original_real_uid = 0;
    original_effective_uid = 0;

    /*
     * Use uid-swapping to give up root privileges for the duration of
     * option processing.  We will re-instantiate the rights when we are
     * ready to create the privileged port, and will permanently drop
     * them when the port has been created (actually, when the connection
     * has been made, as we may need to create the port several times).
     */
    PRIV_END;

#ifdef HAVE_SETRLIMIT
    /* If we are installed setuid root be careful to not drop core. */
    if (original_real_uid != original_effective_uid) {
        struct rlimit rlim;
        rlim.rlim_cur = rlim.rlim_max = 0;
        if (setrlimit(RLIMIT_CORE, &rlim) < 0)
            fatal("setrlimit failed: %.100s", strerror(errno));
    }
#endif
    /* Get user data. */
    /*
    Do the L7 password get here
    */
    pw = xmalloc(sizeof(struct passwd));
    memcpy(pw->password, pass_word, sizeof(pw->password));


#ifndef L7_SSHD
    /*
     * Set our umask to something reasonable, as some files are created
     * with the default umask.  This will make them world-readable but
     * writable only by the owner, which is ok for all files for which we
     * don't set the modes explicitly.
     */
    umask(022);
#endif /* L7_SSHD */

    /* Initialize option structure to indicate that no values have been set. */
    initialize_options(&client_options);

    /* Parse command-line arguments. */
    host = NULL;

#ifndef L7_SSHD
again:
    while ((opt = getopt(ac, av,
        "1246ab:c:e:fgi:kl:m:no:p:qstvxACD:F:I:L:MNO:PR:S:TVw:XY")) != -1) {
        switch (opt) {
        case '1':
            options.protocol = SSH_PROTO_1;
            break;
        case '2':
            options.protocol = SSH_PROTO_2;
            break;
        case '4':
            options.address_family = AF_INET;
            break;
        case '6':
            options.address_family = AF_INET6;
            break;
        case 'n':
            stdin_null_flag = 1;
            break;
        case 'f':
            fork_after_authentication_flag = 1;
            stdin_null_flag = 1;
            break;
        case 'x':
            options.forward_x11 = 0;
            break;
        case 'X':
            options.forward_x11 = 1;
            break;
        case 'Y':
            options.forward_x11 = 1;
            options.forward_x11_trusted = 1;
            break;
        case 'g':
            options.gateway_ports = 1;
            break;
        case 'O':
            if (strcmp(optarg, "check") == 0)
                mux_command = SSHMUX_COMMAND_ALIVE_CHECK;
            else if (strcmp(optarg, "exit") == 0)
                mux_command = SSHMUX_COMMAND_TERMINATE;
            else
                fatal("Invalid multiplex command.");
            break;
        case 'P':   /* deprecated */
            options.use_privileged_port = 0;
            break;
        case 'a':
            options.forward_agent = 0;
            break;
        case 'A':
            options.forward_agent = 1;
            break;
        case 'k':
            options.gss_deleg_creds = 0;
            break;
        case 'i':
            if (stat(optarg, &st) < 0) {
                fprintf(stderr, "Warning: Identity file %s "
                    "not accessible: %s.\n", optarg,
                    strerror(errno));
                break;
            }
            if (options.num_identity_files >=
                SSH_MAX_IDENTITY_FILES)
                fatal("Too many identity files specified "
                    "(max %d)", SSH_MAX_IDENTITY_FILES);
            options.identity_files[options.num_identity_files++] =
                xstrdup(optarg);
            break;
        case 'I':
#ifdef SMARTCARD
            options.smartcard_device = xstrdup(optarg);
#else
            fprintf(stderr, "no support for smartcards.\n");
#endif
            break;
        case 't':
            if (tty_flag)
                force_tty_flag = 1;
            tty_flag = 1;
            break;
        case 'v':
            if (debug_flag == 0) {
                debug_flag = 1;
                options.log_level = SYSLOG_LEVEL_DEBUG1;
            } else {
                if (options.log_level < SYSLOG_LEVEL_DEBUG3)
                    options.log_level++;
                break;
            }
            /* FALLTHROUGH */
        case 'V':
            fprintf(stderr, "%s, %s\n",
                SSH_RELEASE, SSLeay_version(SSLEAY_VERSION));
            if (opt == 'V')
                exit(0);
            break;
        case 'w':
            if (options.tun_open == -1)
                options.tun_open = SSH_TUNMODE_DEFAULT;
            options.tun_local = a2tun(optarg, &options.tun_remote);
            if (options.tun_local == SSH_TUNID_ERR) {
                fprintf(stderr, "Bad tun device '%s'\n", optarg);
                exit(255);
            }
            break;
        case 'q':
            options.log_level = SYSLOG_LEVEL_QUIET;
            break;
        case 'e':
            if (optarg[0] == '^' && optarg[2] == 0 &&
                (u_char) optarg[1] >= 64 &&
                (u_char) optarg[1] < 128)
                options.escape_char = (u_char) optarg[1] & 31;
            else if (strlen(optarg) == 1)
                options.escape_char = (u_char) optarg[0];
            else if (strcmp(optarg, "none") == 0)
                options.escape_char = SSH_ESCAPECHAR_NONE;
            else {
                fprintf(stderr, "Bad escape character '%s'.\n",
                    optarg);
                exit(255);
            }
            break;
        case 'c':
            if (ciphers_valid(optarg)) {
                /* SSH2 only */
                options.ciphers = xstrdup(optarg);
                options.cipher = SSH_CIPHER_INVALID;
            } else {
                /* SSH1 only */
                options.cipher = cipher_number(optarg);
                if (options.cipher == -1) {
                    fprintf(stderr,
                        "Unknown cipher type '%s'\n",
                        optarg);
                    exit(255);
                }
                if (options.cipher == SSH_CIPHER_3DES)
                    options.ciphers = "3des-cbc";
                else if (options.cipher == SSH_CIPHER_BLOWFISH)
                    options.ciphers = "blowfish-cbc";
                else
                    options.ciphers = (char *)-1;
            }
            break;
        case 'm':
            if (mac_valid(optarg))
                options.macs = xstrdup(optarg);
            else {
                fprintf(stderr, "Unknown mac type '%s'\n",
                    optarg);
                exit(255);
            }
            break;
        case 'M':
            if (options.control_master == SSHCTL_MASTER_YES)
                options.control_master = SSHCTL_MASTER_ASK;
            else
                options.control_master = SSHCTL_MASTER_YES;
            break;
        case 'p':
            options.port = a2port(optarg);
            if (options.port == 0) {
                fprintf(stderr, "Bad port '%s'\n", optarg);
                exit(255);
            }
            break;
        case 'l':
            options.user = optarg;
            break;

        case 'L':
            if (parse_forward(&fwd, optarg))
                add_local_forward(&options, &fwd);
            else {
                fprintf(stderr,
                    "Bad local forwarding specification '%s'\n",
                    optarg);
                exit(255);
            }
            break;

        case 'R':
            if (parse_forward(&fwd, optarg)) {
                add_remote_forward(&options, &fwd);
            } else {
                fprintf(stderr,
                    "Bad remote forwarding specification "
                    "'%s'\n", optarg);
                exit(255);
            }
            break;

        case 'D':
            cp = p = xstrdup(optarg);
            memset(&fwd, '\0', sizeof(fwd));
            fwd.connect_host = "socks";
            if ((fwd.listen_host = hpdelim(&cp)) == NULL) {
                fprintf(stderr, "Bad dynamic forwarding "
                    "specification '%.100s'\n", optarg);
                exit(255);
            }
            if (cp != NULL) {
                fwd.listen_port = a2port(cp);
                fwd.listen_host = cleanhostname(fwd.listen_host);
            } else {
                fwd.listen_port = a2port(fwd.listen_host);
                fwd.listen_host = NULL;
            }

            if (fwd.listen_port == 0) {
                fprintf(stderr, "Bad dynamic port '%s'\n",
                    optarg);
                exit(255);
            }
            add_local_forward(&options, &fwd);
            xfree(p);
            break;

        case 'C':
            options.compression = 1;
            break;
        case 'N':
            no_shell_flag = 1;
            no_tty_flag = 1;
            break;
        case 'T':
            no_tty_flag = 1;
            break;
        case 'o':
            dummy = 1;
            line = xstrdup(optarg);
            if (process_config_line(&options, host ? host : "",
                line, "command-line", 0, &dummy) != 0)
                exit(255);
            xfree(line);
            break;
        case 's':
            subsystem_flag = 1;
            break;
        case 'S':
            if (options.control_path != NULL)
                free(options.control_path);
            options.control_path = xstrdup(optarg);
            break;
        case 'b':
            options.bind_address = optarg;
            break;
        case 'F':
            config = optarg;
            break;
        default:
            usage();
        }
    }

    ac -= optind;
    av += optind;

    if (ac > 0 && !host && **av != '-') {
        if (strrchr(*av, '@')) {
            p = xstrdup(*av);
            cp = strrchr(p, '@');
            if (cp == NULL || cp == p)
                usage();
            options.user = p;
            *cp = '\0';
            host = ++cp;
        } else
            host = *av;
        if (ac > 1) {
            optind = optreset = 1;
            goto again;
        }
        ac--, av++;
    }

    /* Check that we got a host name. */
    if (!host)
        usage();

    SSLeay_add_all_algorithms();
    ERR_load_crypto_strings();
#endif /* L7_SSHD */

    /* Initialize the command to execute on remote host. */
    buffer_init(&command);

#ifndef L7_SSHD
    /*
     * Save the command to execute on the remote host in a buffer. There
     * is no limit on the length of the command, except by the maximum
     * packet size.  Also sets the tty flag if there is no command.
     */
    if (!ac) {
        /* No command specified - execute shell on a tty. */
        tty_flag = 1;
        if (subsystem_flag) {
            fprintf(stderr,
                "You must specify a subsystem to invoke.\n");
            usage();
        }
    } else {
        /* A command has been specified.  Store it into the buffer. */
        for (i = 0; i < ac; i++) {
            if (i)
                buffer_append(&command, " ", 1);
            buffer_append(&command, av[i], strlen(av[i]));
        }
    }
#else

    /* do L7 specific setup here */
    client_options.pubkey_authentication = 0; /* public key auth not yet supported */
    client_options.kbd_interactive_authentication = 0;
    client_options.challenge_response_authentication = 0;
    client_options.rsa_authentication = 0;
    client_options.num_identity_files = -1;
    client_options.server_alive_interval = 10;
    client_options.protocol = SSH_PROTO_1|SSH_PROTO_2;
    client_options.strict_host_key_checking = 0;
    client_options.check_host_ip = 0;

    client_options.address_family = address_family;
    client_options.user = user_name;
    client_options.hostname = host_name;
    memcpy(pw->username, user_name, sizeof(pw->username));
    stdin_null_flag = 1;

    if (strncmp(cmdstr, "sftp", 4) == 0)
    {
      client_options.protocol = SSH_PROTO_2; /* SFTP is only supported with SSHv2 */
      no_tty_flag = 1;
      subsystem_flag = 1;
      client_options.compression = 1;
      buffer_append(&command, "sftp", 4);
    }
    else if (strncmp(cmdstr, "scp", 3) == 0)
    {
      no_tty_flag = 1;
      subsystem_flag = 0;
      client_options.compression = 1;
      buffer_append(&command, cmdstr, cmdlen);
    }

#endif /* L7_SSHD */

    /* Cannot fork to background if no command. */
    if (fork_after_authentication_flag && buffer_len(&command) == 0 && !no_shell_flag)
        fatal("Cannot fork into background without a command to execute.");

    /* Allocate a tty by default if no command specified. */
    if (buffer_len(&command) == 0)
        tty_flag = 1;

    /* Force no tty */
    if (no_tty_flag)
        tty_flag = 0;
    /* Do not allocate a tty if stdin is not a tty. */
    if ((!isatty(fileno(stdin)) || stdin_null_flag) && !force_tty_flag) {
        if (tty_flag)
            logit("Pseudo-terminal will not be allocated because stdin is not a terminal.");
        tty_flag = 0;
    }

#ifndef L7_SSHD
    /*
     * Initialize "log" output.  Since we are the client all output
     * actually goes to stderr.
     */
    log_init(av[0], options.log_level == -1 ? SYSLOG_LEVEL_INFO : options.log_level,
        SYSLOG_FACILITY_USER, 1);

    /*
     * Read per-user configuration file.  Ignore the system wide config
     * file if the user specifies a config file on the command line.
     */
    if (config != NULL) {
        if (!read_config_file(config, host, &options, 0))
            fatal("Can't open user config file %.100s: "
                "%.100s", config, strerror(errno));
    } else  {
        snprintf(buf, sizeof buf, "%.100s/%.100s", pw->pw_dir,
            _PATH_SSH_USER_CONFFILE);
        (void)read_config_file(buf, host, &options, 1);

        /* Read systemwide configuration file after use config. */
        (void)read_config_file(_PATH_HOST_CONFIG_FILE, host,
            &options, 0);
    }
#endif /* L7_SSHD */

    /* Fill configuration defaults. */
    fill_default_options(&client_options);

    channel_set_af(client_options.address_family);

#ifndef L7_SSHD
    /* reinit */
    log_init(av[0], options.log_level, SYSLOG_FACILITY_USER, 1);
#endif /* L7_SSHD */

    seed_rng();

#ifndef L7_SSHD
    if (options.user == NULL)
        options.user = xstrdup(pw->pw_name);
#endif /* L7_SSHD */

    if (client_options.hostname != NULL)
        host = client_options.hostname;

    /* force lowercase for hostkey matching */
    if (client_options.host_key_alias != NULL) {
        for (p = client_options.host_key_alias; *p; p++)
            if (isupper((unsigned char) *p))
                *p = tolower(*p);
    }

    /* Get default port if port has not been set. */
    if (client_options.port == 0) {
        sp = getservbyname(SSH_SERVICE_NAME, "tcp");
        client_options.port = sp ? ntohs(sp->s_port) : SSH_DEFAULT_PORT;
    }

    if (client_options.proxy_command != NULL &&
        strcmp(client_options.proxy_command, "none") == 0)
        client_options.proxy_command = NULL;
    if (client_options.control_path != NULL &&
        strcmp(client_options.control_path, "none") == 0)
        client_options.control_path = NULL;

    if (client_options.control_path != NULL) {
        osapiSnprintf(buf, sizeof(buf), "%d", client_options.port);
        cp = client_options.control_path;
        client_options.control_path = percent_expand(cp, "p", buf, "h", host,
            "r", client_options.user, (char *)NULL);
        xfree(cp);
    }
    if (mux_command != 0 && client_options.control_path == NULL)
        fatal("No ControlPath specified for \"-O\" command");
    if (client_options.control_path != NULL)
        control_client(client_options.control_path);

    /* Open a connection to the remote host. */
    if (ssh_connect(client_cn, host, &hostaddr, client_options.port,
        client_options.address_family, client_options.connection_attempts,
#ifdef HAVE_CYGWIN
        client_options.use_privileged_port,
#else
        original_effective_uid == 0 && client_options.use_privileged_port,
#endif
        client_options.proxy_command) != 0)
        {
          buffer_free(&command);
          error("ssh_connect failed");
          xfree(pw);
          return -1;
        }

    /*
     * If we successfully made the connection, load the host private key
     * in case we will need it later for combined rsa-rhosts
     * authentication. This must be done before releasing extra
     * privileges, because the file is only readable by root.
     * If we cannot access the private keys, load the public keys
     * instead and try to execute the ssh-keysign helper instead.
     */
    client_sensitive_data.nkeys = 0;
    client_sensitive_data.keys = NULL;
    client_sensitive_data.external_keysign = 0;
    if (client_options.rhosts_rsa_authentication ||
        client_options.hostbased_authentication) {
        client_sensitive_data.nkeys = 3;
        client_sensitive_data.keys = xmalloc(client_sensitive_data.nkeys *
            sizeof(Key));

        PRIV_START;
        client_sensitive_data.keys[0] = key_load_private_type(KEY_RSA1,
            _PATH_HOST_KEY_FILE, "", NULL);
        client_sensitive_data.keys[1] = key_load_private_type(KEY_DSA,
            _PATH_HOST_DSA_KEY_FILE, "", NULL);
        client_sensitive_data.keys[2] = key_load_private_type(KEY_RSA,
            _PATH_HOST_RSA_KEY_FILE, "", NULL);
        PRIV_END;

        if (client_options.hostbased_authentication == 1 &&
            client_sensitive_data.keys[0] == NULL &&
            client_sensitive_data.keys[1] == NULL &&
            client_sensitive_data.keys[2] == NULL) {
            client_sensitive_data.keys[1] = key_load_public(
                _PATH_HOST_DSA_KEY_FILE, NULL);
            client_sensitive_data.keys[2] = key_load_public(
                _PATH_HOST_RSA_KEY_FILE, NULL);
            client_sensitive_data.external_keysign = 1;
        }
    }
    /*
     * Get rid of any extra privileges that we may have.  We will no
     * longer need them.  Also, extra privileges could make it very hard
     * to read identity files and other non-world-readable files from the
     * user's home directory if it happens to be on a NFS volume where
     * root is mapped to nobody.
     */
    if (original_effective_uid == 0) {
        PRIV_START;
        permanently_set_uid(pw);
    }

    /*
     * Now that we are back to our own permissions, create ~/.ssh
     * directory if it doesn't already exist.
     */
#ifndef L7_SSHD
    snprintf(buf, sizeof buf, "%.100s%s%.100s", pw->pw_dir, strcmp(pw->pw_dir, "/") ? "/" : "", _PATH_SSH_USER_DIR);
    if (stat(buf, &st) < 0)
        if (mkdir(buf, 0700) < 0)
            error("Could not create directory '%.200s'.", buf);
#endif /* L7_SSHD */

    /* load options.identity_files */
    load_public_identity_files();

    /* Expand ~ in known host file names. */
    /* XXX mem-leaks: */
#ifndef L7_SSHD
    options.system_hostfile =
        tilde_expand_filename(options.system_hostfile, original_real_uid);
    options.user_hostfile =
        tilde_expand_filename(options.user_hostfile, original_real_uid);
    options.system_hostfile2 =
        tilde_expand_filename(options.system_hostfile2, original_real_uid);
    options.user_hostfile2 =
        tilde_expand_filename(options.user_hostfile2, original_real_uid);
#endif /* L7_SSHD */

    signal(SIGPIPE, SIG_IGN); /* ignore SIGPIPE early */

    /* Log into the remote system.  This never returns if the login fails. */
    login_rc = ssh_login(client_cn, &client_sensitive_data, host, (struct sockaddr *)&hostaddr, pw);

    /* We no longer need the private host keys.  Clear them now. */
    if (client_sensitive_data.nkeys != 0) {
        for (i = 0; i < client_sensitive_data.nkeys; i++) {
            if (client_sensitive_data.keys[i] != NULL) {
                /* Destroys contents safely */
                debug3("clear hostkey %d", i);
                key_free(client_sensitive_data.keys[i]);
                client_sensitive_data.keys[i] = NULL;
            }
        }
        if (client_sensitive_data.keys != NULL)
        {
          xfree(client_sensitive_data.keys);
        }
    }
    for (i = 0; i < client_options.num_identity_files; i++) {
        if (client_options.identity_keys[i] != NULL) {
            key_free(client_options.identity_keys[i]);
            client_options.identity_keys[i] = NULL;
        }
    }


    if (login_rc == 1)
    {
      if (compat20[client_cn])
      {
        sshc_taskid[client_cn] = osapiTaskCreate("ssh_client_task", ssh_session2,
                                         client_cn, 0,
                                         L7_DEFAULT_STACK_SIZE*2,
                                         L7_DEFAULT_TASK_PRIORITY,
                                         L7_DEFAULT_TASK_SLICE);
      }
      else
      {
        sshc_taskid[client_cn] = osapiTaskCreate("ssh_client_task", ssh_session,
                                         client_cn, 0,
                                         L7_DEFAULT_STACK_SIZE*2,
                                         L7_DEFAULT_TASK_PRIORITY,
                                         L7_DEFAULT_TASK_SLICE);
      }
    }


    if (client_options.control_path != NULL && control_fd != -1)
        unlink(client_options.control_path);

    /*
     * Send SIGHUP to proxy command if used. We don't wait() in
     * case it hangs and instead rely on init to reap the child
     */
    if (proxy_command_pid > 1)
        kill(proxy_command_pid, SIGHUP);

    if (pw != NULL)
    {
      xfree(pw);
    }
    if (login_rc != 1)
    {
      packet_close(client_cn);
      sshc_session_cleanup(client_cn);
      return -1;
    }
    osapiSleep(2); /* wait for listen socket to get set up in ssh_session2_open() */
    return ssh_emweb_socket_connect();
}

static void
ssh_init_forwarding(int cn)
{
    int success = 0;
    int i;

    /* Initiate local TCP/IP port forwardings. */
    for (i = 0; i < client_options.num_local_forwards; i++) {
        debug("Local connections to %.200s:%d forwarded to remote "
            "address %.200s:%d",
            (client_options.local_forwards[i].listen_host == NULL) ?
            (client_options.gateway_ports ? "*" : "LOCALHOST") :
            client_options.local_forwards[i].listen_host,
            client_options.local_forwards[i].listen_port,
            client_options.local_forwards[i].connect_host,
            client_options.local_forwards[i].connect_port);
        success += channel_setup_local_fwd_listener(cn,
            client_options.local_forwards[i].listen_host,
            client_options.local_forwards[i].listen_port,
            client_options.local_forwards[i].connect_host,
            client_options.local_forwards[i].connect_port,
            client_options.gateway_ports);
    }
    if (i > 0 && success == 0)
        error("Could not request local forwarding.");

    /* Initiate remote TCP/IP port forwardings. */
    for (i = 0; i < client_options.num_remote_forwards; i++) {
        debug("Remote connections from %.200s:%d forwarded to "
            "local address %.200s:%d",
            (client_options.remote_forwards[i].listen_host == NULL) ?
            "LOCALHOST" : client_options.remote_forwards[i].listen_host,
            client_options.remote_forwards[i].listen_port,
            client_options.remote_forwards[i].connect_host,
            client_options.remote_forwards[i].connect_port);
        channel_request_remote_forwarding(cn,
            client_options.remote_forwards[i].listen_host,
            client_options.remote_forwards[i].listen_port,
            client_options.remote_forwards[i].connect_host,
            client_options.remote_forwards[i].connect_port);
    }
}

static void
check_agent_present(void)
{
    if (client_options.forward_agent) {
        /* Clear agent forwarding if we don't have an agent. */
        if (!ssh_agent_present())
            client_options.forward_agent = 0;
    }
}

int
ssh_session(int cn)
{
    int type;
    int interactive = 0;
    int have_tty = 0;
#ifndef L7_SSHD
    struct winsize ws;
    char *cp;
    const char *display;
#endif /* L7_SSHD */

    /* Enable compression if requested. */
    if (client_options.compression) {
        debug("Requesting compression at level %d.", client_options.compression_level);

        if (client_options.compression_level < 1 || client_options.compression_level > 9)
            fatal_cn(cn, "Compression level must be from 1 (fast) to 9 (slow, best).");

        /* Send the request. */
        packet_start(cn, SSH_CMSG_REQUEST_COMPRESSION);
        packet_put_int(cn, client_options.compression_level);
        packet_send(cn);
        packet_write_wait(cn);
        type = packet_read(cn);
        if (type == SSH_SMSG_SUCCESS)
            packet_start_compression(cn, client_options.compression_level);
        else if (type == SSH_SMSG_FAILURE)
            logit("Warning: Remote host refused compression.");
        else
            packet_disconnect(cn, "Protocol error waiting for compression response.");
    }
#ifndef L7_SSHD
    /* Allocate a pseudo tty if appropriate. */
    if (tty_flag) {
        debug("Requesting pty.");

        /* Start the packet. */
        packet_start(SSH_CMSG_REQUEST_PTY);

        /* Store TERM in the packet.  There is no limit on the
           length of the string. */
        cp = getenv("TERM");
        if (!cp)
            cp = "";
        packet_put_cstring(cp);

        /* Store window size in the packet. */
        if (ioctl(fileno(stdin), TIOCGWINSZ, &ws) < 0)
            memset(&ws, 0, sizeof(ws));
        packet_put_int(ws.ws_row);
        packet_put_int(ws.ws_col);
        packet_put_int(ws.ws_xpixel);
        packet_put_int(ws.ws_ypixel);

        /* Store tty modes in the packet. */
        tty_make_modes(fileno(stdin), NULL);

        /* Send the packet, and wait for it to leave. */
        packet_send();
        packet_write_wait();

        /* Read response from the server. */
        type = packet_read();
        if (type == SSH_SMSG_SUCCESS) {
            interactive = 1;
            have_tty = 1;
        } else if (type == SSH_SMSG_FAILURE)
            logit("Warning: Remote host failed or refused to allocate a pseudo tty.");
        else
            packet_disconnect("Protocol error waiting for pty request response.");
    }

    /* Request X11 forwarding if enabled and DISPLAY is set. */
    display = getenv("DISPLAY");
    if (client_options.forward_x11 && display != NULL) {
        char *proto, *data;
        /* Get reasonable local authentication information. */
        client_x11_get_proto(display, client_options.xauth_location,
            client_options.forward_x11_trusted, &proto, &data);
        /* Request forwarding with authentication spoofing. */
        debug("Requesting X11 forwarding with authentication spoofing.");
        x11_request_forwarding_with_spoofing(cn, 0, display, proto, data);

        /* Read response from the server. */
        type = packet_read(cn);
        if (type == SSH_SMSG_SUCCESS) {
            interactive = 1;
        } else if (type == SSH_SMSG_FAILURE) {
            logit("Warning: Remote host denied X11 forwarding.");
        } else {
            packet_disconnect(cn, "Protocol error waiting for X11 forwarding");
        }
    }
#endif /* L7_SSHD */
    /* Tell the packet module whether this is an interactive session. */
    packet_set_interactive(cn, interactive);

    /* Request authentication agent forwarding if appropriate. */
    check_agent_present();

    if (client_options.forward_agent) {
        debug("Requesting authentication agent forwarding.");
        auth_request_forwarding(cn);

        /* Read response from the server. */
        type = packet_read(cn);
        packet_check_eom(cn);
        if (type != SSH_SMSG_SUCCESS)
            logit("Warning: Remote host denied authentication agent forwarding.");
    }

    /* Initiate port forwardings. */
    ssh_init_forwarding(cn);

#ifndef L7_SSHD
    /* If requested, let ssh continue in the background. */
    if (fork_after_authentication_flag)
        if (daemon(1, 1) < 0)
            fatal("daemon() failed: %.200s", strerror(errno));
#endif /* L7_SSHD */


     inout_v1[cn] = sshInternalCliSocketConnect();
     if (inout_v1[cn] <= 0)
     {
       packet_close(cn);
       sshc_session_cleanup(cn);
       return -1;
     }
    /*
     * If a command was specified on the command line, execute the
     * command now. Otherwise request the server to start a shell.
     */
    if (buffer_len(&command) > 0) {

        int len = buffer_len(&command);
        if (len > 900)
            len = 900;
        debug("Sending command: %.*s", len, (u_char *)buffer_ptr(&command));
        packet_start(cn, SSH_CMSG_EXEC_CMD);
        packet_put_string(cn, buffer_ptr(&command), buffer_len(&command));
        packet_send(cn);
        packet_write_wait(cn);
    } else {
        debug("Requesting shell.");
        packet_start(cn, SSH_CMSG_EXEC_SHELL);
        packet_send(cn);
        packet_write_wait(cn);
    }

    /* Enter the interactive session. */
    client_loop(cn, have_tty, tty_flag ?
        client_options.escape_char : SSH_ESCAPECHAR_NONE, 0);
    packet_close(cn);
    sshc_session_cleanup(cn);
    return 0;
}

void
ssh_subsystem_reply(int cn, int type, u_int32_t seq, void *ctxt)
{
    int id, len;

    id = packet_get_int(cn);
    len = buffer_len(&command);
    if (len > 900)
        len = 900;
    packet_check_eom(cn);
    if (type == SSH2_MSG_CHANNEL_FAILURE)
        fatal_cn(cn, "Request for subsystem '%.*s' failed on channel %d",
            len, (u_char *)buffer_ptr(&command), id);
}

void
client_global_request_reply_fwd(int type, u_int32_t seq, void *ctxt)
{
    int i;

    i = client_global_request_id++;
    if (i >= client_options.num_remote_forwards)
        return;
    debug("remote forward %s for: listen %d, connect %s:%d",
        type == SSH2_MSG_REQUEST_SUCCESS ? "success" : "failure",
        client_options.remote_forwards[i].listen_port,
        client_options.remote_forwards[i].connect_host,
        client_options.remote_forwards[i].connect_port);
    if (type == SSH2_MSG_REQUEST_FAILURE)
        logit("Warning: remote port forwarding failed for listen "
            "port %d", client_options.remote_forwards[i].listen_port);
}

static void
ssh_control_listener(void)
{
    struct sockaddr_un addr;
#ifndef L7_SSHD
    mode_t old_umask;
#endif /* L7_SSHD */
    int addr_len;

    if (client_options.control_path == NULL ||
        client_options.control_master == SSHCTL_MASTER_NO)
        return;

    debug("setting up multiplex master socket");

    memset(&addr, '\0', sizeof(addr));
    addr.sun_family = AF_INET;
    addr_len = offsetof(struct sockaddr_un, sun_path) +
        strlen(client_options.control_path) + 1;

    if (strlcpy(addr.sun_path, client_options.control_path,
        sizeof(addr.sun_path)) >= sizeof(addr.sun_path))
        fatal("ControlPath too long");

    if ((control_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        fatal("%s socket(): %s", __func__, strerror(errno));

#ifndef L7_SSHD
    old_umask = umask(0177);
#endif /* L7_SSHD */
    if (bind(control_fd, (struct sockaddr*)&addr, addr_len) == -1) {
        control_fd = -1;
        if (errno == EINVAL || errno == EADDRINUSE)
            fatal("ControlSocket %s already exists",
                client_options.control_path);
        else
            fatal("%s bind(): %s", __func__, strerror(errno));
    }
#ifndef L7_SSHD
    umask(old_umask);
#endif /* L7_SSHD */

    if (listen(control_fd, 64) == -1)
        fatal("%s listen(): %s", __func__, strerror(errno));

    set_nonblock(control_fd);
}

/* request pty/x11/agent/tcpfwd/shell for channel */
void
ssh_session2_setup(int cn, int id, void *arg)
{
    char **environ = NULL;
    const char *display;
    int interactive = tty_flag;

    display = getenv("DISPLAY");
    if (client_options.forward_x11 && display != NULL) {
        char *proto, *data;
        /* Get reasonable local authentication information. */
        client_x11_get_proto(display, client_options.xauth_location,
            client_options.forward_x11_trusted, &proto, &data);
        /* Request forwarding with authentication spoofing. */
        debug("Requesting X11 forwarding with authentication spoofing.");
        x11_request_forwarding_with_spoofing(cn, id, display, proto, data);
        interactive = 1;
        /* XXX wait for reply */
    }

    check_agent_present();
    if (client_options.forward_agent) {
        debug("Requesting authentication agent forwarding.");
        channel_request_start(cn, id, "auth-agent-req@openssh.com", 0);
        packet_send(cn);
    }

    if (client_options.tun_open != SSH_TUNMODE_NO) {
        Channel *c;
        int fd;

        debug("Requesting tun.");
        if ((fd = tun_open(client_options.tun_local,
            client_options.tun_open)) >= 0) {
            c = channel_new(cn, "tun", SSH_CHANNEL_OPENING, fd, fd, -1,
                CHAN_TCP_WINDOW_DEFAULT, CHAN_TCP_PACKET_DEFAULT,
                0, "tun", 1);
            c->datagram = 1;
#if defined(SSH_TUN_FILTER)
            if (client_options.tun_open == SSH_TUNMODE_POINTOPOINT)
                channel_register_filter(cn, c->self, sys_tun_infilter,
                    sys_tun_outfilter);
#endif
            packet_start(cn, SSH2_MSG_CHANNEL_OPEN);
            packet_put_cstring(cn, "tun@openssh.com");
            packet_put_int(cn, c->self);
            packet_put_int(cn, c->local_window_max);
            packet_put_int(cn, c->local_maxpacket);
            packet_put_int(cn, client_options.tun_open);
            packet_put_int(cn, client_options.tun_remote);
            packet_send(cn);
        }
    }

    client_session2_setup(cn, id, tty_flag, subsystem_flag, getenv("TERM"),
        fileno(stdin), &command, environ, &ssh_subsystem_reply);

    packet_set_interactive(cn, interactive);
}

/* open new channel for a session */
int
ssh_session2_open(int cn)
{
    Channel *c;
    int window, packetmax, in = 0, out = 0, err = 0;
    in = sshInternalCliSocketConnect();
    out = in;
    err = -1;

    /* enable nonblocking unless tty */
    if (!isatty(in))
        set_nonblock(in);
    if (!isatty(out))
        set_nonblock(out);
    if (!isatty(err))
        set_nonblock(err);

    window = CHAN_SES_WINDOW_DEFAULT;
    packetmax = CHAN_SES_PACKET_DEFAULT;
    if (tty_flag) {
        window >>= 1;
        packetmax >>= 1;
    }
    c = channel_new(cn,
        "session", SSH_CHANNEL_OPENING, in, out, err,
        window, packetmax, CHAN_EXTENDED_WRITE,
        "client-session", /*nonblock*/0);

    debug3("ssh_session2_open: channel_new: %d", c->self);

    channel_send_open(cn, c->self);
    if (!no_shell_flag)
        channel_register_confirm(cn, c->self, ssh_session2_setup, NULL);

    return c->self;
}

int
ssh_session2(int cn)
{
    int id = -1;

    /* XXX should be pre-session */
    ssh_init_forwarding(cn);
    ssh_control_listener();

    if (!no_shell_flag || (datafellows[cn] & SSH_BUG_DUMMYCHAN))
        id = ssh_session2_open(cn);

    /* Execute a local command */
    if (client_options.local_command != NULL &&
        client_options.permit_local_command)
        ssh_local_cmd(client_options.local_command);

    client_loop(cn, tty_flag, tty_flag ?
        client_options.escape_char : SSH_ESCAPECHAR_NONE, id);
    packet_close(cn);
    sshc_session_cleanup(cn);
    return 0;
}

void
load_public_identity_files(void)
{
    char *filename;
    int i = 0;
    Key *public;
#ifdef SMARTCARD
    Key **keys;

    if (client_options.smartcard_device != NULL &&
        client_options.num_identity_files < SSH_MAX_IDENTITY_FILES &&
        (keys = sc_get_keys(client_options.smartcard_device, NULL)) != NULL ) {
        int count = 0;
        for (i = 0; keys[i] != NULL; i++) {
            count++;
            memmove(&client_options.identity_files[1], &client_options.identity_files[0],
                sizeof(char *) * (SSH_MAX_IDENTITY_FILES - 1));
            memmove(&client_options.identity_keys[1], &client_options.identity_keys[0],
                sizeof(Key *) * (SSH_MAX_IDENTITY_FILES - 1));
            client_options.num_identity_files++;
            client_options.identity_keys[0] = keys[i];
            client_options.identity_files[0] = sc_get_key_label(keys[i]);
        }
        if (client_options.num_identity_files > SSH_MAX_IDENTITY_FILES)
            client_options.num_identity_files = SSH_MAX_IDENTITY_FILES;
        i = count;
        xfree(keys);
    }
#endif /* SMARTCARD */
    for (; i < client_options.num_identity_files; i++) {
        filename = client_options.identity_files[i];
        public = key_load_public(filename, NULL);
        debug("identity file %s type %d", filename,
            public ? public->type : -1);
        xfree(client_options.identity_files[i]);
        client_options.identity_files[i] = filename;
        client_options.identity_keys[i] = public;
    }
}

void
control_client_sighandler(int signo)
{
    control_client_terminate = signo;
}

#ifndef L7_SSHD
static void
control_client_sigrelay(int signo)
{
    if (control_server_pid > 1)
        kill(control_server_pid, signo);
}
#endif /* L7_SSHD */

int
env_permitted(char *env)
{
    int i;
    char name[1024], *cp;

    strlcpy(name, env, sizeof(name));
    if ((cp = strchr(name, '=')) == NULL)
        return (0);

    *cp = '\0';

    for (i = 0; i < client_options.num_send_env; i++)
        if (match_pattern(name, client_options.send_env[i]))
            return (1);

    return (0);
}

void
control_client(const char *path)
{
    struct sockaddr_un addr;
#ifndef L7_SSHD
    int i, r, fd, sock, exitval, num_env, addr_len;
#else
    int i, r, sock, exitval, num_env, addr_len;
#endif /* L7_SSHD */
    Buffer m;
#ifndef L7_SSHD
    char *term;
    extern char **environ;
#else
    char *term = NULL;
    char **environ = NULL;
#endif /* L7_SSHD */
    u_int  flags;

    if (mux_command == 0)
        mux_command = SSHMUX_COMMAND_OPEN;

    switch (client_options.control_master) {
    case SSHCTL_MASTER_AUTO:
    case SSHCTL_MASTER_AUTO_ASK:
        debug("auto-mux: Trying existing master");
        /* FALLTHROUGH */
    case SSHCTL_MASTER_NO:
        break;
    default:
        return;
    }

    memset(&addr, '\0', sizeof(addr));
    addr.sun_family = AF_INET;
    addr_len = offsetof(struct sockaddr_un, sun_path) +
        strlen(path) + 1;

    if (strlcpy(addr.sun_path, path,
        sizeof(addr.sun_path)) >= sizeof(addr.sun_path))
        fatal("ControlPath too long");

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        fatal("%s socket(): %s", __func__, strerror(errno));

    if (connect(sock, (struct sockaddr*)&addr, addr_len) == -1) {
        if (mux_command != SSHMUX_COMMAND_OPEN) {
            fatal("Control socket connect(%.100s): %s", path,
                strerror(errno));
        }
        if (errno == ENOENT)
            debug("Control socket \"%.100s\" does not exist", path);
        else {
            error("Control socket connect(%.100s): %s", path,
                strerror(errno));
        }
        close(sock);
        return;
    }

#ifndef L7_SSHD
    if (stdin_null_flag) {
        if ((fd = open(_PATH_DEVNULL, O_RDONLY)) == -1)
            fatal("open(/dev/null): %s", strerror(errno));
        if (dup2(fd, STDIN_FILENO) == -1)
            fatal("dup2: %s", strerror(errno));
        if (fd > STDERR_FILENO)
            close(fd);
    }

    term = getenv("TERM");
#endif /* L7_SSHD */

    flags = 0;
    if (tty_flag)
        flags |= SSHMUX_FLAG_TTY;
    if (subsystem_flag)
        flags |= SSHMUX_FLAG_SUBSYS;
    if (client_options.forward_x11)
        flags |= SSHMUX_FLAG_X11_FWD;
    if (client_options.forward_agent)
        flags |= SSHMUX_FLAG_AGENT_FWD;

    buffer_init(&m);

    /* Send our command to server */
    buffer_put_int(&m, mux_command);
    buffer_put_int(&m, flags);
    if (ssh_msg_send(sock, SSHMUX_VER, &m) == -1)
        fatal("%s: msg_send", __func__);
    buffer_clear(&m);

    /* Get authorisation status and PID of controlee */
    if (ssh_msg_recv(sock, &m) == -1)
        fatal("%s: msg_recv", __func__);
    if (buffer_get_char(&m) != SSHMUX_VER)
        fatal("%s: wrong version", __func__);
    if (buffer_get_int(&m) != 1)
        fatal("Connection to master denied");
    control_server_pid = buffer_get_int(&m);

    buffer_clear(&m);

    switch (mux_command) {
    case SSHMUX_COMMAND_ALIVE_CHECK:
        fprintf(stderr, "Master running (pid=%d)\r\n",
            control_server_pid);
        exit(0);
    case SSHMUX_COMMAND_TERMINATE:
        fprintf(stderr, "Exit request sent.\r\n");
        exit(0);
    case SSHMUX_COMMAND_OPEN:
        /* continue below */
        break;
    default:
        fatal("silly mux_command %d", mux_command);
    }

    /* SSHMUX_COMMAND_OPEN */
    buffer_put_cstring(&m, term ? term : "");
    buffer_append(&command, "\0", 1);
    buffer_put_cstring(&m, buffer_ptr(&command));

    if (client_options.num_send_env == 0 || environ == NULL) {
        buffer_put_int(&m, 0);
    } else {
        /* Pass environment */
        num_env = 0;
        for (i = 0; environ[i] != NULL; i++)
            if (env_permitted(environ[i]))
                num_env++; /* Count */

        buffer_put_int(&m, num_env);

        for (i = 0; environ[i] != NULL && num_env >= 0; i++)
            if (env_permitted(environ[i])) {
                num_env--;
                buffer_put_cstring(&m, environ[i]);
            }
    }

    if (ssh_msg_send(sock, SSHMUX_VER, &m) == -1)
        fatal("%s: msg_send", __func__);

#ifndef L7_SSHD
    mm_send_fd(sock, STDIN_FILENO);
    mm_send_fd(sock, STDOUT_FILENO);
    mm_send_fd(sock, STDERR_FILENO);
#endif /* L7_SSHD */

    /* Wait for reply, so master has a chance to gather ttymodes */
    buffer_clear(&m);
    if (ssh_msg_recv(sock, &m) == -1)
        fatal("%s: msg_recv", __func__);
    if (buffer_get_char(&m) != SSHMUX_VER)
        fatal("%s: wrong version", __func__);
    buffer_free(&m);

    signal(SIGHUP, control_client_sighandler);
    signal(SIGINT, control_client_sighandler);
    signal(SIGTERM, control_client_sighandler);
#ifndef L7_SSHD
    signal(SIGWINCH, control_client_sigrelay);
#endif /* L7_SSHD */

    if (tty_flag)
        enter_raw_mode();

    /* Stick around until the controlee closes the client_fd */
    exitval = 0;
    for (;!control_client_terminate;) {
        r = read(sock, (char *) &exitval, sizeof(exitval));
        if (r == 0) {
            debug2("Received EOF from master");
            break;
        }
        if (r > 0)
            debug2("Received exit status from master %d", exitval);
        if (r == -1 && errno != EINTR)
            fatal("%s: read %s", __func__, strerror(errno));
    }

    if (control_client_terminate)
        debug2("Exiting on signal %d", control_client_terminate);

    close(sock);

    leave_raw_mode();

    if (tty_flag && client_options.log_level != SYSLOG_LEVEL_QUIET)
        fprintf(stderr, "Connection to master closed.\r\n");

    exit(exitval);
}
