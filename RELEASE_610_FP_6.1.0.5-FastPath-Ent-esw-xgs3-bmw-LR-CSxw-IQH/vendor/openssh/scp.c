/*
 * scp - secure remote copy.  This is basically patched BSD rcp which
 * uses ssh to do the data transfer (instead of using rcmd).
 *
 * NOTE: This version should NOT be suid root.  (This uses ssh to
 * do the transfer and ssh has the necessary privileges.)
 *
 * 1995 Timo Rinne <tri@iki.fi>, Tatu Ylonen <ylo@cs.hut.fi>
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */
/*
 * Copyright (c) 1999 Theo de Raadt.  All rights reserved.
 * Copyright (c) 1999 Aaron Campbell.  All rights reserved.
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

/*
 * Parts from:
 *
 * Copyright (c) 1983, 1990, 1992, 1993, 1995
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "includes.h"
RCSID("$OpenBSD: scp.c,v 1.130 2006/01/31 10:35:43 djm Exp $");

#include "xmalloc.h"
#include "atomicio.h"
#include "pathnames.h"
#include "log.h"
#include "misc.h"
#include "progressmeter.h"

#ifdef L7_SSHD
#include "simapi.h"
#endif /* L7_SSHD */

extern char *__progname;
#ifdef L7_SSHD
extern int ssh_client_main(char *, int, char *, char *, char *, int, int *);
extern void sshc_quit_pending(int cn, int sock);    /* Quit the client loop. */
#endif /* L7_SSHD */

void bwlimit(int);

/* Struct for addargs */
arglist args;

/* Bandwidth limit */
off_t limit_rate = 0;

/* Name of current file being transferred. */
char *curfile;

/* This is set to non-zero to enable verbose mode. */
int verbose_mode = 0;

/* This is set to zero if the progressmeter is not desired. */
#ifndef L7_SSHD
int showprogress = 1;
#else
static int showprogress = 0;
#endif /* L7_SSHD */

/* This is the program to execute for the secured connection. ("ssh" or -S) */
char *ssh_program = _PATH_SSH_PROGRAM;

/* This is used to store the pid of ssh_program */
pid_t do_cmd_pid = -1;

#ifndef L7_SSHD
static void
killchild(int signo)
{
    if (do_cmd_pid > 1) {
        kill(do_cmd_pid, signo ? signo : SIGTERM);
        waitpid(do_cmd_pid, NULL, 0);
    }

    if (signo)
        _exit(1);
    exit(1);
}

static int
do_local_cmd(arglist *a)
{
    u_int i;
    int status;
    pid_t pid;

    if (a->num == 0)
        fatal("do_local_cmd: no arguments");

    if (verbose_mode) {
        fprintf(stderr, "Executing:");
        for (i = 0; i < a->num; i++)
            fprintf(stderr, " %s", a->list[i]);
        fprintf(stderr, "\n");
    }
    if ((pid = fork()) == -1)
        fatal("do_local_cmd: fork: %s", strerror(errno));

    if (pid == 0) {
        execvp(a->list[0], a->list);
        perror(a->list[0]);
        exit(1);
    }

    do_cmd_pid = pid;
    signal(SIGTERM, killchild);
    signal(SIGINT, killchild);
    signal(SIGHUP, killchild);

    while (waitpid(pid, &status, 0) == -1)
        if (errno != EINTR)
            fatal("do_local_cmd: waitpid: %s", strerror(errno));

    do_cmd_pid = -1;

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        return (-1);

    return (0);
}
#endif /* L7_SSHD */

/*
 * This function executes the given command as the specified user on the
 * given host.  This returns < 0 if execution fails, and >= 0 otherwise. This
 * assigns the input and output file descriptors on success.
 */

int
do_cmd(char *host, char *remuser, char *cmd, int *fdin, int *fdout, int argc)
{
#ifndef L7_SSHD
    int pin[2], pout[2], reserved[2];

    if (verbose_mode)
        fprintf(stderr,
            "Executing: program %s host %s, user %s, command %s\n",
            ssh_program, host,
            remuser ? remuser : "(unspecified)", cmd);

    /*
     * Reserve two descriptors so that the real pipes won't get
     * descriptors 0 and 1 because that will screw up dup2 below.
     */
    pipe(reserved);

    /* Create a socket pair for communicating with ssh. */
    if (pipe(pin) < 0)
        fatal("pipe: %s", strerror(errno));
    if (pipe(pout) < 0)
        fatal("pipe: %s", strerror(errno));

    /* Free the reserved descriptors. */
    close(reserved[0]);
    close(reserved[1]);

    /* Fork a child to execute the command on the remote host using ssh. */
    do_cmd_pid = fork();
    if (do_cmd_pid == 0) {
        /* Child. */
        close(pin[1]);
        close(pout[0]);
        dup2(pin[0], 0);
        dup2(pout[1], 1);
        close(pin[0]);
        close(pout[1]);

        replacearg(&args, 0, "%s", ssh_program);
        if (remuser != NULL)
            addargs(&args, "-l%s", remuser);
        addargs(&args, "%s", host);
        addargs(&args, "%s", cmd);

        execvp(ssh_program, args.list);
        perror(ssh_program);
        exit(1);
    } else if (do_cmd_pid == -1) {
        fatal("fork: %s", strerror(errno));
    }
    /* Parent.  Close the other side, and return the local side. */
    close(pin[0]);
    *fdout = pin[1];
    close(pout[1]);
    *fdin = pout[0];
    signal(SIGTERM, killchild);
    signal(SIGINT, killchild);
    signal(SIGHUP, killchild);
#endif /* L7_SSHD */
    return 0;
}

typedef struct {
    size_t cnt;
    char *buf;
} BUF;

BUF *allocbuf(BUF *, int, int);
void lostconn(int);
void nospace(void);
int okname(char *);
void run_err(const char *,...);
void verifydir(char *);

#ifndef L7_SSHD
struct passwd *pwd;
#endif /* L7_SSHD */
uid_t userid;
int errs, remin, remout;
int pflag, iamremote, iamrecursive, targetshouldbedirectory;

#define CMDNEEDS    64
char cmd[CMDNEEDS];     /* must hold "rcp -r -p -d\0" */

int response(void);
void rsource(char *, struct stat *);
#ifndef L7_SSHD
void sink(int, char *[]);
void source(int, char *[]);
#else
int sink(int, char *);
int source(char *);
#endif /* L7_SSHD */
void tolocal(int, char *[]);
void toremote(char *, int, char *[]);
#ifndef L7_SSHD
void usage(void);
#endif /* L7_SSHD */

#ifndef L7_SSHD
int
main(int argc, char **argv)
{
    int ch, fflag, tflag, status;
    double speed;
    char *targ, *endp;
    extern char *optarg;
    extern int optind;

    /* Ensure that fds 0, 1 and 2 are open or directed to /dev/null */
    sanitise_stdfd();

    __progname = ssh_get_progname(argv[0]);

    memset(&args, '\0', sizeof(args));
    args.list = NULL;
    addargs(&args, "%s", ssh_program);
    addargs(&args, "-x");
    addargs(&args, "-oForwardAgent no");
    addargs(&args, "-oPermitLocalCommand no");
    addargs(&args, "-oClearAllForwardings yes");

    fflag = tflag = 0;
    while ((ch = getopt(argc, argv, "dfl:prtvBCc:i:P:q1246S:o:F:")) != -1)
        switch (ch) {
        /* User-visible flags. */
        case '1':
        case '2':
        case '4':
        case '6':
        case 'C':
            addargs(&args, "-%c", ch);
            break;
        case 'o':
        case 'c':
        case 'i':
        case 'F':
            addargs(&args, "-%c%s", ch, optarg);
            break;
        case 'P':
            addargs(&args, "-p%s", optarg);
            break;
        case 'B':
            addargs(&args, "-oBatchmode yes");
            break;
        case 'l':
            speed = strtod(optarg, &endp);
            if (speed <= 0 || *endp != '\0')
                usage();
            limit_rate = speed * 1024;
            break;
        case 'p':
            pflag = 1;
            break;
        case 'r':
            iamrecursive = 1;
            break;
        case 'S':
            ssh_program = xstrdup(optarg);
            break;
        case 'v':
            addargs(&args, "-v");
            verbose_mode = 1;
            break;
        case 'q':
            addargs(&args, "-q");
            showprogress = 0;
            break;

        /* Server options. */
        case 'd':
            targetshouldbedirectory = 1;
            break;
        case 'f':   /* "from" */
            iamremote = 1;
            fflag = 1;
            break;
        case 't':   /* "to" */
            iamremote = 1;
            tflag = 1;
#ifdef HAVE_CYGWIN
            setmode(0, O_BINARY);
#endif
            break;
        default:
            usage();
        }
    argc -= optind;
    argv += optind;

    if ((pwd = getpwuid(userid = getuid())) == NULL)
        fatal("unknown user %u", (u_int) userid);

    if (!isatty(STDERR_FILENO))
        showprogress = 0;

    remin = STDIN_FILENO;
    remout = STDOUT_FILENO;

    if (fflag) {
        /* Follow "protocol", send data. */
        (void) response();
        source(argc, argv);
        exit(errs != 0);
    }
    if (tflag) {
        /* Receive data. */
        sink(argc, argv);
        exit(errs != 0);
    }
    if (argc < 2)
        usage();
    if (argc > 2)
        targetshouldbedirectory = 1;

    remin = remout = -1;
    do_cmd_pid = -1;
    /* Command to be executed on remote system using "ssh". */
    (void) snprintf(cmd, sizeof cmd, "scp%s%s%s%s",
        verbose_mode ? " -v" : "",
        iamrecursive ? " -r" : "", pflag ? " -p" : "",
        targetshouldbedirectory ? " -d" : "");

    (void) signal(SIGPIPE, lostconn);

    if ((targ = colon(argv[argc - 1]))) /* Dest is remote host. */
        toremote(targ, argc, argv);
    else {
        if (targetshouldbedirectory)
            verifydir(argv[argc - 1]);
        tolocal(argc, argv);    /* Dest is local host. */
    }
    /*
     * Finally check the exit status of the ssh process, if one was forked
     * and no error has occured yet
     */
    if (do_cmd_pid != -1 && errs == 0) {
        if (remin != -1)
            (void) close(remin);
        if (remout != -1)
            (void) close(remout);
        if (waitpid(do_cmd_pid, &status, 0) == -1)
            errs = 1;
        else {
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
                errs = 1;
        }
    }
    exit(errs != 0);
}
#else
    /*
    Get source/dest and other params from L7
    */
int
scp_main(char *hostname, int address_family, char *username, char *password,
           unsigned int clisocket, char *remote_file,
           char *local_file, unsigned int updownflag)
{
  int inout = -1;
  int rc = 0;
  int cn = -1;
  targetshouldbedirectory = iamrecursive = iamremote = pflag = 0;

  switch(address_family)
  {
    case L7_AF_INET:
      address_family = AF_INET;
      break;
    case L7_AF_INET6:
      address_family = AF_INET6;
      break;
    default:
      break;
  }
  /*
  client command             runs this on server
  ----------------------------------------------
  scp foo server:bar         scp -t bar
  scp server:bar foo         scp -f bar
  */

  /* local to remote */
  if (updownflag == L7_TRANSFER_DIRECTION_UP)
  {
    (void) osapiSnprintf(cmd, sizeof(cmd), "scp -t %s", remote_file);
    inout = ssh_client_main(hostname, address_family, username, password, cmd, strlen(cmd), &cn);
    if (inout < 0)
    {
      return -1;
    }
    remin = inout;
    remout = inout;
    if (response() < 0)
    {
      sshc_quit_pending(cn, inout);
      shutdown(inout, SHUT_RDWR);
      (void) close(inout);
      inout = -1;
      remin = remout = -1;
      return (-1);
    }

    rc = source(local_file);
  }

  /* remote to local */
  else if (updownflag == L7_TRANSFER_DIRECTION_DOWN)
  {
    (void) osapiSnprintf(cmd, sizeof(cmd), "scp -f %s", remote_file);
    inout = ssh_client_main(hostname, address_family, username, password, cmd, strlen(cmd), &cn);
    if (inout < 0)
    {
      return -1;
    }
    remin = inout;
    remout = inout;
    rc = sink(1, local_file);
  }
  else
  {
    return -1;
  }
  sshc_quit_pending(cn, inout);
  shutdown(inout, SHUT_RDWR);
  (void) close(inout);
  inout = -1;
  remin = remout = -1;

  return rc;
}
#endif /* L7_SSHD */

void
toremote(char *targ, int argc, char **argv)
{
#ifndef L7_SSHD
    int i, len;
    char *bp, *host, *src, *suser, *thost, *tuser, *arg;
    arglist alist;

    memset(&alist, '\0', sizeof(alist));
    alist.list = NULL;

    *targ++ = 0;
    if (*targ == 0)
        targ = ".";

    arg = xstrdup(argv[argc - 1]);
    if ((thost = strrchr(arg, '@'))) {
        /* user@host */
        *thost++ = 0;
        tuser = arg;
        if (*tuser == '\0')
            tuser = NULL;
    } else {
        thost = arg;
        tuser = NULL;
    }

    if (tuser != NULL && !okname(tuser)) {
        xfree(arg);
        return;
    }

    for (i = 0; i < argc - 1; i++) {
        src = colon(argv[i]);
        if (src) {  /* remote to remote */
            freeargs(&alist);
            addargs(&alist, "%s", ssh_program);
            if (verbose_mode)
                addargs(&alist, "-v");
            addargs(&alist, "-x");
            addargs(&alist, "-oClearAllForwardings yes");
            addargs(&alist, "-n");

            *src++ = 0;
            if (*src == 0)
                src = ".";
            host = strrchr(argv[i], '@');

            if (host) {
                *host++ = 0;
                host = cleanhostname(host);
                suser = argv[i];
                if (*suser == '\0')
                    suser = pwd->pw_name;
                else if (!okname(suser))
                    continue;
                addargs(&alist, "-l");
                addargs(&alist, "%s", suser);
            } else {
                host = cleanhostname(argv[i]);
            }
            addargs(&alist, "%s", host);
            addargs(&alist, "%s", cmd);
            addargs(&alist, "%s", src);
            addargs(&alist, "%s%s%s:%s",
                tuser ? tuser : "", tuser ? "@" : "",
                thost, targ);
            if (do_local_cmd(&alist) != 0)
                errs = 1;
        } else {    /* local to remote */
            if (remin == -1) {
                len = strlen(targ) + CMDNEEDS + 20;
                bp = xmalloc(len);
                (void) snprintf(bp, len, "%s -t %s", cmd, targ);
                host = cleanhostname(thost);
                if (do_cmd(host, tuser, bp, &remin,
                    &remout, argc) < 0)
                    exit(1);
                if (response() < 0)
                    exit(1);
                (void) xfree(bp);
            }
            source(1, argv + i);
        }
    }
#endif /* L7_SSHD */
}

void
tolocal(int argc, char **argv)
{
#ifndef L7_SSHD
    int i, len;
    char *bp, *host, *src, *suser;
    arglist alist;

    memset(&alist, '\0', sizeof(alist));
    alist.list = NULL;

    for (i = 0; i < argc - 1; i++) {
        if (!(src = colon(argv[i]))) {  /* Local to local. */
            freeargs(&alist);
            addargs(&alist, "%s", _PATH_CP);
            if (iamrecursive)
                addargs(&alist, "-r");
            if (pflag)
                addargs(&alist, "-p");
            addargs(&alist, "%s", argv[i]);
            addargs(&alist, "%s", argv[argc-1]);
            if (do_local_cmd(&alist))
                ++errs;
            continue;
        }
        *src++ = 0;
        if (*src == 0)
            src = ".";
        if ((host = strrchr(argv[i], '@')) == NULL) {
            host = argv[i];
            suser = NULL;
        } else {
            *host++ = 0;
            suser = argv[i];
            if (*suser == '\0')
                suser = pwd->pw_name;
        }
        host = cleanhostname(host);
        len = strlen(src) + CMDNEEDS + 20;
        bp = xmalloc(len);
        (void) snprintf(bp, len, "%s -f %s", cmd, src);
        if (do_cmd(host, suser, bp, &remin, &remout, argc) < 0) {
            (void) xfree(bp);
            ++errs;
            continue;
        }
        xfree(bp);
        sink(1, argv + argc - 1);
        (void) close(remin);
        remin = remout = -1;
    }
#endif /* L7_SSHD */
}

#ifndef L7_SSHD
void
source(int argc, char **argv)
#else
int
source(char* name)
#endif /* L7_SSHD */
{
    struct stat stb;
    static BUF buffer;
    BUF *bp;
#ifndef L7_SSHD
    off_t i, amt, statbytes;
    size_t result;
    int fd = -1, haderr, indx;
#else
    unsigned long i, amt, statbytes;
    size_t result;
    int fd = -1, haderr;
#endif /* L7_SSHD */
    char *last, buf[2048];
    int len;

#ifdef L7_SSHD
    buffer.buf = NULL;
#endif /* L7_SSHD */

#ifndef L7_SSHD
    for (indx = 0; indx < argc; ++indx) {
        name = argv[indx];
#endif /* L7_SSHD */
        statbytes = 0;
        len = strlen(name);
        while (len > 1 && name[len-1] == '/')
            name[--len] = '\0';
        if (strchr(name, '\n') != NULL) {
            run_err("%s: skipping, filename contains a newline",
                name);
            goto next;
        }
#ifndef L7_SSHD
        if ((fd = open(name, O_RDONLY, 0)) < 0)
#else
        if ((fd = osapiFsOpen(name)) == L7_ERROR)
#endif /* L7_SSHD */
          goto syserr;
        if (fstat(fd, &stb) < 0) {
syserr:         run_err("%s: %s", name, strerror(errno));
            goto next;
        }
        switch (stb.st_mode & S_IFMT) {
        case S_IFREG:
            break;
#ifndef L7_SSHD
        case S_IFDIR:
            if (iamrecursive) {
                rsource(name, &stb);
                goto next;
            }
            /* FALLTHROUGH */
#endif /* L7_SSHD */
        default:
            run_err("%s: not a regular file", name);
            goto next;
        }
        if ((last = strrchr(name, '/')) == NULL)
            last = name;
        else
            ++last;
        curfile = last;
        if (pflag) {
            /*
             * Make it compatible with possible future
             * versions expecting microseconds.
             */
#ifndef L7_SSHD
            (void) snprintf(buf, sizeof buf, "T%lu 0 %lu 0\n",
#else
            (void) osapiSnprintf(buf, sizeof buf, "T%lu 0 %lu 0\n",
#endif /* L7_SSHD */
                (u_long) stb.st_mtime,
                (u_long) stb.st_atime);
            (void) atomicio(vwrite, remout, buf, strlen(buf));
            if (response() < 0)
                goto next;
        }
#define FILEMODEMASK    (S_ISUID|S_ISGID|S_IRWXU|S_IRWXG|S_IRWXO)
#ifndef L7_SSHD
        snprintf(buf, sizeof buf, "C%04o %lld %s\n",
#else
        osapiSnprintf(buf, sizeof buf, "C%04o %lld %s\n",
#endif /* L7_SSHD */
            (u_int) (stb.st_mode & FILEMODEMASK),
            (long long)stb.st_size, last);
        if (verbose_mode) {
            fprintf(stderr, "Sending file modes: %s", buf);
        }
        (void) atomicio(vwrite, remout, buf, strlen(buf));
        if (response() < 0)
            goto next;
        if ((bp = allocbuf(&buffer, fd, 2048)) == NULL) {
next:           if (fd != -1) {
#ifndef L7_SSHD
                  (void) close(fd);
#else
                  (void) osapiFsClose(fd);
#endif /* L7_SSHD */
                fd = -1;
            }
#ifndef L7_SSHD
            continue;
#else
        if (buffer.buf != NULL)
        {
          xfree(buffer.buf);
          buffer.buf = NULL;
          buffer.cnt = 0;
        }
        logit("source: failed");
        return -1;
#endif /* L7_SSHD */
        }
        if (showprogress)
            start_progress_meter(curfile, stb.st_size, (off_t *) &statbytes);
#ifdef L7_SSHD
        simTransferBytesCompletedSet(0);
#endif /* L7_SSHD */
        /* Keep writing after an error so that we stay sync'd up. */
        for (haderr = i = 0; i < stb.st_size; i += bp->cnt) {
            amt = bp->cnt;
            if (i + amt > stb.st_size)
                amt = stb.st_size - i;
            if (!haderr) {
#ifndef L7_SSHD
              result = atomicio(read, fd, bp->buf, amt);
#else
              result = atomicio(vread, fd, bp->buf, amt);
#endif /* L7_SSHD */
                if (result != amt)
#ifndef L7_SSHD
                    haderr = errno;
#else
                {
                  if (buffer.buf != NULL)
                  {
                    xfree(buffer.buf);
                    buffer.buf = NULL;
                    buffer.cnt = 0;
                  }
                  (void) osapiFsClose(fd);
                  return -1;
                }
#endif /* L7_SSHD */
            }
            if (haderr)
                (void) atomicio(vwrite, remout, bp->buf, amt);
            else {
                result = atomicio(vwrite, remout, bp->buf, amt);
                if (result != amt)
#ifndef L7_SSHD
                    haderr = errno;
#else
                {
                  if (buffer.buf != NULL)
                  {
                    xfree(buffer.buf);
                    buffer.buf = NULL;
                    buffer.cnt = 0;
                  }
                  (void) osapiFsClose(fd);
                  return -1;
                }
#endif /* L7_SSHD */
                statbytes += result;
#ifdef L7_SSHD
                simTransferBytesCompletedSet(statbytes);
#endif /* L7_SSHD */
            }
            if (limit_rate)
                bwlimit(amt);
        }
        if (showprogress)
            stop_progress_meter();

        if (fd != -1) {
#ifndef L7_SSHD
            if (close(fd) < 0 && !haderr)
#else
            if (osapiFsClose(fd) == L7_ERROR && !haderr)
#endif /* L7_SSHD */
                haderr = errno;
            fd = -1;
        }
        if (!haderr)
            (void) atomicio(vwrite, remout, "", 1);
        else
            run_err("%s: %s", name, strerror(haderr));
        (void) response();
#ifndef L7_SSHD
    }
#endif /* L7_SSHD */


#ifdef L7_SSHD
    if (buffer.buf != NULL)
    {
      xfree(buffer.buf);
      buffer.buf = NULL;
      buffer.cnt = 0;
    }
    if (!haderr)
    return 0;
    else
      return -1;
#endif /* L7_SSHD */
}
#ifndef L7_SSHD
void
rsource(char *name, struct stat *statp)
{
    DIR *dirp;
    struct dirent *dp;
    char *last, *vect[1], path[1100];

    if (!(dirp = opendir(name))) {
        run_err("%s: %s", name, strerror(errno));
        return;
    }
    last = strrchr(name, '/');
    if (last == 0)
        last = name;
    else
        last++;
    if (pflag) {
        (void) snprintf(path, sizeof(path), "T%lu 0 %lu 0\n",
            (u_long) statp->st_mtime,
            (u_long) statp->st_atime);
        (void) atomicio(vwrite, remout, path, strlen(path));
        if (response() < 0) {
            closedir(dirp);
            return;
        }
    }
    (void) snprintf(path, sizeof path, "D%04o %d %.1024s\n",
        (u_int) (statp->st_mode & FILEMODEMASK), 0, last);
    if (verbose_mode)
        fprintf(stderr, "Entering directory: %s", path);
    (void) atomicio(vwrite, remout, path, strlen(path));
    if (response() < 0) {
        closedir(dirp);
        return;
    }
    while ((dp = readdir(dirp)) != NULL) {
        if (dp->d_ino == 0)
            continue;
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
            continue;
        if (strlen(name) + 1 + strlen(dp->d_name) >= sizeof(path) - 1) {
            run_err("%s/%s: name too long", name, dp->d_name);
            continue;
        }
        (void) snprintf(path, sizeof path, "%s/%s", name, dp->d_name);
        vect[0] = path;
        source(1, vect);
    }
    (void) closedir(dirp);
    (void) atomicio(vwrite, remout, "E\n", 2);
    (void) response();
}
#endif /* L7_SSHD */

void
bwlimit(int amount)
{
    static struct timeval bwstart, bwend;
    static int lamt, thresh = 16384;
    u_int64_t waitlen;
    struct timespec ts, rm;


#ifndef L7_SSHD
    if (!timerisset(&bwstart)) {
        gettimeofday(&bwstart, NULL);
        return;
    }
#else
    if (!((bwstart).tv_sec || (bwstart).tv_usec)) {
#ifdef _L7_OS_LINUX_
        gettimeofday(&bwstart, NULL);
#endif /* _L7_OS_LINUX_ */
#ifdef _L7_OS_VXWORKS_
       clock_gettime(CLOCK_REALTIME, (struct timespec *)&bwstart);
#endif /* _L7_OS_VXWORKS_ */
        return;
    }
#endif /* L7_SSHD */

    lamt += amount;
    if (lamt < thresh)
        return;

#ifndef L7_SSHD
    gettimeofday(&bwend, NULL);
    timersub(&bwend, &bwstart, &bwend);
    if (!timerisset(&bwend))
        return;
#else
#ifdef _L7_OS_LINUX_
    gettimeofday(&bwend, NULL);
#endif /* _L7_OS_LINUX_ */
#ifdef _L7_OS_VXWORKS_
    clock_gettime(CLOCK_REALTIME, (struct timespec *)&bwend);
#endif /* _L7_OS_VXWORKS_ */
    timersub(&bwend, &bwstart, &bwend);
    if (!((bwend).tv_sec || (bwend).tv_usec))
        return;
#endif /* L7_SSHD */

    lamt *= 8;
    waitlen = (double)1000000L * lamt / limit_rate;

    bwstart.tv_sec = waitlen / 1000000L;
    bwstart.tv_usec = waitlen % 1000000L;
#ifndef L7_SSHD
    if (timercmp(&bwstart, &bwend, >)) {
#else
    if  (((bwstart).tv_sec == (bwend).tv_sec && (bwstart).tv_usec > (bwend).tv_usec) ||
         (bwstart).tv_sec > (bwend).tv_sec) {
#endif /* L7_SSHD */
        timersub(&bwstart, &bwend, &bwend);

        /* Adjust the wait time */
        if (bwend.tv_sec) {
            thresh /= 2;
            if (thresh < 2048)
                thresh = 2048;
        } else if (bwend.tv_usec < 100) {
            thresh *= 2;
            if (thresh > 32768)
                thresh = 32768;
        }

        TIMEVAL_TO_TIMESPEC(&bwend, &ts);
        while (nanosleep(&ts, &rm) == -1) {
            if (errno != EINTR)
                break;
            ts = rm;
        }
    }

    lamt = 0;
#ifndef L7_SSHD
    gettimeofday(&bwstart, NULL);
#else
#ifdef _L7_OS_LINUX_
    gettimeofday(&bwstart, NULL);
#endif /* _L7_OS_LINUX_ */
#ifdef _L7_OS_VXWORKS_
    clock_gettime(CLOCK_REALTIME, (struct timespec *)&bwstart);
#endif /* _L7_OS_VXWORKS_ */
#endif /* L7_SSHD */
}

#ifndef L7_SSHD
void
sink(int argc, char **argv)
#else
int
sink(int argc, char *argv)
#endif /* L7_SSHD */
{
    static BUF buffer;
    struct stat stb;
    enum {
        YES, NO, DISPLAYED
    } wrerr;
    BUF *bp;
    off_t i;
#ifndef L7_SSHD
    size_t j, count;
    int amt, exists, first, mask, mode, ofd, omode;
    off_t size, statbytes;
#else
    size_t count;
    int amt, exists, first, mode, ofd, j;
    off_t size = 0, statbytes = 0;
#endif /* L7_SSHD */
    int setimes, targisdir, wrerrno = 0;
#ifndef L7_SSHD
    char ch, *cp, *np, *targ, *why, *vect[1], buf[2048];
#else
    char ch, *cp, *np, *targ, *why, buf[2048];
#endif /* L7_SSHD */
    struct timeval tv[2];

#ifdef L7_SSHD
    buffer.buf = NULL;
#endif /* L7_SSHD */

#define atime   tv[0]
#define mtime   tv[1]
#define SCREWUP(str)    { why = str; goto screwup; }

    setimes = targisdir = 0;
#ifndef L7_SSHD
    mask = umask(0);
    if (!pflag)
        (void) umask(mask);
    if (argc != 1) {
        run_err("ambiguous target");
        exit(1);
    }
#endif /* L7_SSHD */
#ifndef L7_SSHD
    targ = *argv;
#else
    targ = argv;
#endif /* L7_SSHD */
    if (targetshouldbedirectory)
        verifydir(targ);

    (void) atomicio(vwrite, remout, "", 1);
    if (stat(targ, &stb) == 0 && S_ISDIR(stb.st_mode))
        targisdir = 1;
    for (first = 1;; first = 0) {
        cp = buf;
#ifndef L7_SSHD
        if (atomicio(read, remin, cp, 1) != 1)
          return;
#else
        if (atomicio(vread, remin, cp, 1) != 1)
        {
          if (buffer.buf != NULL)
          {
            xfree(buffer.buf);
            buffer.buf = NULL;
            buffer.cnt = 0;
          }
          /* determine whether the file was completely downloaded */
          if (size == statbytes && statbytes != 0)
          {
            debug("sink: statbytes=%ld size=%ld", (unsigned long) statbytes, (unsigned long) size);
            return 0;
          }
          else
          {
            error("sink: statbytes=%ld size=%ld", (unsigned long) statbytes, (unsigned long) size);
            return -1;
          }
        }
#endif /* L7_SSHD */
        if (*cp++ == '\n')
            SCREWUP("unexpected <newline>");
        do {
#ifndef L7_SSHD
          if (atomicio(read, remin, &ch, sizeof(ch)) != sizeof(ch))
#else
          if (atomicio(vread, remin, &ch, sizeof(ch)) != sizeof(ch))
#endif /* L7_SSHD */
                SCREWUP("lost connection");
            *cp++ = ch;
        } while (cp < &buf[sizeof(buf) - 1] && ch != '\n');
        *cp = 0;
        if (verbose_mode)
            fprintf(stderr, "Sink: %s", buf);

        if (buf[0] == '\01' || buf[0] == '\02') {
#ifndef L7_SSHD
            if (iamremote == 0)
                (void) atomicio(vwrite, STDERR_FILENO,
                    buf + 1, strlen(buf + 1));
            if (buf[0] == '\02')
                exit(1);
            ++errs;
            continue;
#else
            {
              error(buf+1);
              if (buffer.buf != NULL)
              {
                xfree(buffer.buf);
                buffer.buf = NULL;
                buffer.cnt = 0;
              }
              return -1;
            }
#endif /* L7_SSHD */
        }
        if (buf[0] == 'E') {
            (void) atomicio(vwrite, remout, "", 1);
#ifndef L7_SSHD
            return;
#else
            return 0;
#endif /* L7_SSHD */
        }
        if (ch == '\n')
            *--cp = 0;

        cp = buf;
        if (*cp == 'T') {

            setimes++;
            cp++;
            mtime.tv_sec = strtol(cp, &cp, 10);
            if (!cp || *cp++ != ' ')
                SCREWUP("mtime.sec not delimited");
            mtime.tv_usec = strtol(cp, &cp, 10);
            if (!cp || *cp++ != ' ')
                SCREWUP("mtime.usec not delimited");
            atime.tv_sec = strtol(cp, &cp, 10);
            if (!cp || *cp++ != ' ')
                SCREWUP("atime.sec not delimited");
            atime.tv_usec = strtol(cp, &cp, 10);
            if (!cp || *cp++ != '\0')
                SCREWUP("atime.usec not delimited");
            (void) atomicio(vwrite, remout, "", 1);
#ifndef L7_SSHD
            continue;
#else
            return -1;
#endif /* L7_SSHD */
        }
        if (*cp != 'C' && *cp != 'D') {

            /*
             * Check for the case "rcp remote:foo\* local:bar".
             * In this case, the line "No match." can be returned
             * by the shell before the rcp command on the remote is
             * executed so the ^Aerror_message convention isn't
             * followed.
             */
#ifndef L7_SSHD
            if (first) {
                run_err("%s", cp);
                exit(1);
            }
#endif /* L7_SSHD */
            SCREWUP("expected control record");
        }
        mode = 0;
        for (++cp; cp < buf + 5; cp++) {
            if (*cp < '0' || *cp > '7')
                SCREWUP("bad mode");
            mode = (mode << 3) | (*cp - '0');
        }
        if (*cp++ != ' ')
            SCREWUP("mode not delimited");

#ifndef L7_SSHD
        for (size = 0; isdigit(*cp);)
#else
        for (size = 0; isdigit((int)*cp);)
#endif /* L7_SSHD */
            size = size * 10 + (*cp++ - '0');
        if (*cp++ != ' ')
            SCREWUP("size not delimited");
        if ((strchr(cp, '/') != NULL) || (strcmp(cp, "..") == 0)) {

#ifndef L7_SSHD
            run_err("error: unexpected filename: %s", cp);
            exit(1);
#else
            SCREWUP("error: unexpected filename");
#endif /* L7_SSHD */
        }
        if (targisdir) {
            static char *namebuf;
            static size_t cursize;
            size_t need;

            need = strlen(targ) + strlen(cp) + 250;
            if (need > cursize) {
                if (namebuf)
                    xfree(namebuf);
                namebuf = xmalloc(need);
                cursize = need;
            }
#ifndef L7_SSHD
            (void) snprintf(namebuf, need, "%s%s%s", targ,
#else
            (void) osapiSnprintf(namebuf, need, "%s%s%s", targ,
#endif /* L7_SSHD */
                strcmp(targ, "/") ? "/" : "", cp);
            np = namebuf;
        } else
            np = targ;
        curfile = cp;
        exists = stat(np, &stb) == 0;
#ifndef L7_SSHD
        if (buf[0] == 'D') {
            int mod_flag = pflag;
            if (!iamrecursive)
                SCREWUP("received directory without -r");
            if (exists) {
                if (!S_ISDIR(stb.st_mode)) {
                    errno = ENOTDIR;
                    goto bad;
                }
                if (pflag)
                    (void) chmod(np, mode);
            } else {
                /* Handle copying from a read-only
                   directory */
                mod_flag = 1;
                if (mkdir(np, mode | S_IRWXU) < 0)
                    goto bad;
            }
            vect[0] = xstrdup(np);
            sink(1, vect);
            if (setimes) {
                setimes = 0;
                if (utimes(vect[0], tv) < 0)
                    run_err("%s: set times: %s",
                        vect[0], strerror(errno));
            }
            if (mod_flag)
                (void) chmod(vect[0], mode);
            if (vect[0])
                xfree(vect[0]);
            continue;
        }
        omode = mode;
        mode |= S_IWRITE;
        if ((ofd = open(np, O_WRONLY|O_CREAT, mode)) < 0) {
bad:            run_err("%s: %s", np, strerror(errno));
            continue;
        }
#else
        if ((ofd = osapiFsOpen(np)) == L7_ERROR)
        {
          if (osapiFsCreateFile(np) != L7_SUCCESS)
          {
            ofd = -1;
          }
          else
          {
            ofd = osapiFsOpen(np);
            if (ofd == L7_ERROR)
            {
              ofd = -1;
            }
          }
        }
        if (ofd < 0)
        {
          return -1;
        }
#endif /* L7_SSHD */
        (void) atomicio(vwrite, remout, "", 1);
        if ((bp = allocbuf(&buffer, ofd, 4096)) == NULL) {
#ifndef L7_SSHD
            (void) close(ofd);
            continue;
#else
            (void) osapiFsClose(ofd);
            return -1;
#endif /* L7_SSHD */
        }
        cp = bp->buf;
        wrerr = NO;

        statbytes = 0;
        if (showprogress)
            start_progress_meter(curfile, size, &statbytes);
#ifdef L7_SSHD
        simTransferBytesCompletedSet(0);
#endif /* L7_SSHD */
        for (count = i = 0; i < size; i += 4096) {
            amt = 4096;
            if (i + amt > size)
                amt = size - i;
            count += amt;
            do {
#ifndef L7_SSHD
              j = atomicio(read, remin, cp, amt);
#else
              debug("sink: reading %d bytes from connection %d", amt, remin);
              j = atomicio(vread, remin, cp, amt);
#endif /* L7_SSHD */
#ifndef L7_SSHD
                if (j == 0) {
                    run_err("%s", j ? strerror(errno) :
                        "dropped connection");
                    exit(1);
                }
#else
              if (j != amt) {
                    if (buffer.buf != NULL)
                    {
                      xfree(buffer.buf);
                      buffer.buf = NULL;
                      buffer.cnt = 0;
                    }
                    (void) osapiFsClose(ofd);
                    error("sink: dropped connection");
                    return -1;
              }
              debug("sink: read %d bytes from connection %d", j, remin);
#endif /* L7_SSHD */
                amt -= j;
                cp += j;
                statbytes += j;
#ifdef L7_SSHD
                simTransferBytesCompletedSet(statbytes);
#endif /* L7_SSHD */
            } while (amt > 0);

            if (limit_rate)
                bwlimit(4096);

            if (count == bp->cnt) {
                /* Keep reading so we stay sync'd up. */
                if (wrerr == NO) {
                    if (atomicio(vwrite, ofd, bp->buf,
                        count) != count) {
#ifndef L7_SSHD
                        wrerr = YES;
                        wrerrno = errno;
#else
                    if (buffer.buf != NULL)
                    {
                      xfree(buffer.buf);
                      buffer.buf = NULL;
                      buffer.cnt = 0;
                    }
                    (void) osapiFsClose(ofd);
                    error("sink: dropped connection");
                    return -1;
#endif /* L7_SSHD */
                    }
                }
                count = 0;
                cp = bp->buf;
            }
        }
#ifdef L7_SSHD
        debug("sink: finished reading from connection %d", remin);
#endif /* L7_SSHD */
        if (showprogress)
            stop_progress_meter();
        if (count != 0 && wrerr == NO &&
            atomicio(vwrite, ofd, bp->buf, count) != count) {
#ifndef L7_SSHD
            wrerr = YES;
            wrerrno = errno;
#else
            if (buffer.buf != NULL)
            {
              xfree(buffer.buf);
              buffer.buf = NULL;
              buffer.cnt = 0;
            }
            (void) osapiFsClose(ofd);
            error("sink: dropped connection");
            return -1;
#endif /* L7_SSHD */
        }
        if (wrerr == NO && ftruncate(ofd, size) != 0) {
            run_err("%s: truncate: %s", np, strerror(errno));
            wrerr = DISPLAYED;
        }
#ifndef L7_SSHD
        if (pflag) {
            if (exists || omode != mode)
#ifdef HAVE_FCHMOD
                if (fchmod(ofd, omode)) {
#else /* HAVE_FCHMOD */
                if (chmod(np, omode)) {
#endif /* HAVE_FCHMOD */
                    run_err("%s: set mode: %s",
                        np, strerror(errno));
                    wrerr = DISPLAYED;
                }
        } else {
            if (!exists && omode != mode)
#ifdef HAVE_FCHMOD
                if (fchmod(ofd, omode & ~mask)) {
#else /* HAVE_FCHMOD */
                if (chmod(np, omode & ~mask)) {
#endif /* HAVE_FCHMOD */
                    run_err("%s: set mode: %s",
                        np, strerror(errno));
                    wrerr = DISPLAYED;
                }
        }
#endif /* L7_SSHD */
#ifndef L7_SSHD
        if (close(ofd) == -1) {
#else
        if (osapiFsClose(ofd) == L7_ERROR) {
#endif /* L7_SSHD */
            wrerr = YES;
            wrerrno = errno;
        }
        (void) response();
#ifndef L7_SSHD
        if (setimes && wrerr == NO) {
            setimes = 0;
            if (utimes(np, tv) < 0) {
                run_err("%s: set times: %s",
                    np, strerror(errno));
                wrerr = DISPLAYED;
            }
        }
#endif /* L7_SSHD */
        switch (wrerr) {
        case YES:
            run_err("%s: %s", np, strerror(wrerrno));
            break;
        case NO:
            (void) atomicio(vwrite, remout, "", 1);
#ifdef L7_SSHD
            if (buffer.buf != NULL)
            {
              xfree(buffer.buf);
              buffer.buf = NULL;
              buffer.cnt = 0;
            }
            debug("sink: statbytes=%ld size=%ld", (unsigned long) statbytes, (unsigned long) size);
            return 0;
#endif /* L7_SSHD */
            /*break;  NO BREAK DUE TO RETURN ABOVE */
        case DISPLAYED:
            break;
        }
#ifdef L7_SSHD
        SCREWUP("unknown error");
#endif /* L7_SSHD */
    }
screwup:
    run_err("protocol error: %s", why);
#ifndef L7_SSHD
    exit(1);
#else
    if (buffer.buf != NULL)
    {
      xfree(buffer.buf);
      buffer.buf = NULL;
      buffer.cnt = 0;
    }
    error("sink: screwup error");
    return -1;
#endif /* L7_SSHD */
}

int
response(void)
{
    char ch, *cp, resp, rbuf[2048];

#ifndef L7_SSHD
    if (atomicio(read, remin, &resp, sizeof(resp)) != sizeof(resp))
      lostconn(0);
#else
    if (atomicio(vread, remin, &resp, sizeof(resp)) != sizeof(resp))
      return (-1);
#endif /* L7_SSHD */

    cp = rbuf;
    switch (resp) {
    case 0:     /* ok */
        return (0);
    default:
        *cp++ = resp;
        /* FALLTHROUGH */
    case 1:     /* error, followed by error msg */
    case 2:     /* fatal error, "" */
#ifdef L7_SSHD
        debug("Error on response()");
#endif /* L7_SSHD */
        do {
#ifndef L7_SSHD
            if (atomicio(read, remin, &ch, sizeof(ch)) != sizeof(ch))
              lostconn(0);
#else
            if (atomicio(vread, remin, &ch, sizeof(ch)) != sizeof(ch))
              return (-1);
#endif /* L7_SSHD */
            *cp++ = ch;
        } while (cp < &rbuf[sizeof(rbuf) - 1] && ch != '\n');
#ifndef L7_SSHD
        if (!iamremote)
            (void) atomicio(vwrite, STDERR_FILENO, rbuf, cp - rbuf);
#endif
        ++errs;
        if (resp == 1)
            return (-1);
#ifndef L7_SSHD
        exit(1);
#else
        return (-1);
#endif /* L7_SSHD */
    }
    /* NOTREACHED */
#ifdef L7_SSHD
  return -1;
#endif /* L7_SSHD */
}
#ifndef L7_SSHD
void
usage(void)
{
    (void) fprintf(stderr,
        "usage: scp [-1246BCpqrv] [-c cipher] [-F ssh_config] [-i identity_file]\n"
        "           [-l limit] [-o ssh_option] [-P port] [-S program]\n"
        "           [[user@]host1:]file1 [...] [[user@]host2:]file2\n");
    exit(1);
}
#endif /* L7_SSHD */

void
run_err(const char *fmt,...)
{
#ifndef L7_SSHD
    static FILE *fp;
#endif /* L7_SSHD */
    va_list ap;

    ++errs;
#ifndef L7_SSHD
    if (fp == NULL && !(fp = fdopen(remout, "w")))
        return;
    (void) fprintf(fp, "%c", 0x01);
    (void) fprintf(fp, "scp: ");
    va_start(ap, fmt);
    (void) vfprintf(fp, fmt, ap);
    va_end(ap);
    (void) fprintf(fp, "\n");
    (void) fflush(fp);

    if (!iamremote) {
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        fprintf(stderr, "\n");
    }
#else
    va_start(ap, fmt);
    error(fmt, ap);
    va_end(ap);
#endif /* L7_SSHD */
}

void
verifydir(char *cp)
{
    struct stat stb;

    if (!stat(cp, &stb)) {
        if (S_ISDIR(stb.st_mode))
            return;
        errno = ENOTDIR;
    }
    run_err("%s: %s", cp, strerror(errno));
#ifndef L7_SSHD
    killchild(0);
#endif /* L7_SSHD */
}

int
okname(char *cp0)
{
    int c;
    char *cp;

    cp = cp0;
    do {
        c = (int)*cp;
#ifndef L7_SSHD
        if (c & 0200)
            goto bad;
#endif /* L7_SSHD */
        if (!isalpha(c) && !isdigit(c)) {
            switch (c) {
            case '\'':
            case '"':
            case '`':
            case ' ':
            case '#':
                goto bad;
            default:
                break;
            }
        }
    } while (*++cp);
    return (1);

bad:    fprintf(stderr, "%s: invalid user name\n", cp0);
    return (0);
}

BUF *
allocbuf(BUF *bp, int fd, int blksize)
{
    size_t size;
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
    struct stat stb;

    if (fstat(fd, &stb) < 0) {
        run_err("fstat: %s", strerror(errno));
        return (0);
    }
    size = roundup(stb.st_blksize, blksize);
    if (size == 0)
        size = blksize;
#else /* HAVE_STRUCT_STAT_ST_BLKSIZE */
    size = blksize;
#endif /* HAVE_STRUCT_STAT_ST_BLKSIZE */
    if (bp->cnt >= size)
        return (bp);
    if (bp->buf == NULL)
        bp->buf = xmalloc(size);
    else
        bp->buf = xrealloc(bp->buf, size);
    memset(bp->buf, 0, size);
    bp->cnt = size;
    return (bp);
}

void
lostconn(int signo)
{
#ifndef L7_SSHD
    if (!iamremote)
        write(STDERR_FILENO, "lost connection\n", 16);
    if (signo)
        _exit(1);
    else
        exit(1);
#endif /* L7_SSHD */
}
