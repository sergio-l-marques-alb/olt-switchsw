/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Password authentication.  This file contains the functions to check whether
 * the password is valid for the user.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 * Copyright (c) 1999 Dug Song.  All rights reserved.
 * Copyright (c) 2000 Markus Friedl.  All rights reserved.
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
RCSID("$OpenBSD: auth-passwd.c,v 1.34 2005/07/19 15:32:26 otto Exp $");

#include "packet.h"
#include "buffer.h"
#include "log.h"
#include "servconf.h"
#include "auth.h"
#include "auth-options.h"

#ifdef L7_SSHD
#include "canohost.h"
#include "commdefs.h"
#include "usmdb_user_mgmt_api.h"
#include "usmdb_trapmgr_api.h"
#endif /* L7_SSHD */

#ifndef L7_SSHD
extern Buffer loginmsg;
#else
extern Buffer loginmsg[];
#endif /* L7_SSHD */
extern ServerOptions options;

#ifdef HAVE_LOGIN_CAP
extern login_cap_t *lc;
#endif


#define DAY     (24L * 60 * 60) /* 1 day in seconds */
#define TWO_WEEKS   (2L * 7 * DAY)  /* 2 weeks in seconds */

void
disable_forwarding(void)
{
    no_port_forwarding_flag = 1;
    no_agent_forwarding_flag = 1;
    no_x11_forwarding_flag = 1;
}

/*
 * Tries to authenticate the user using password.  Returns true if
 * authentication succeeds.
 */
int
auth_password(Authctxt *authctxt, const char *password)
{
#ifndef L7_SSHD
    struct passwd * pw = authctxt->pw;
#endif /* L7_SSHD */
    int result, ok = authctxt->valid;
#ifndef L7_SSHD
#if defined(USE_SHADOW) && defined(HAS_SHADOW_EXPIRE)
    static int expire_checked = 0;
#endif

#ifndef HAVE_CYGWIN
    if (pw->pw_uid == 0 && options.permit_root_login != PERMIT_YES)
        ok = 0;
#endif
    if (*password == '\0' && options.permit_empty_passwd == 0)
        return 0;

#ifdef KRB5
    if (options.kerberos_authentication == 1) {
        int ret = auth_krb5_password(authctxt, password);
        if (ret == 1 || ret == 0)
            return ret && ok;
        /* Fall back to ordinary passwd authentication. */
    }
#endif
#ifdef HAVE_CYGWIN
    if (is_winnt) {
        HANDLE hToken = cygwin_logon_user(pw, password);

        if (hToken == INVALID_HANDLE_VALUE)
            return 0;
        cygwin_set_impersonation_token(hToken);
        return ok;
    }
#endif
#ifdef USE_PAM
    if (options.use_pam)
        return (sshpam_auth_passwd(authctxt, password) && ok);
#endif
#if defined(USE_SHADOW) && defined(HAS_SHADOW_EXPIRE)
    if (!expire_checked) {
        expire_checked = 1;
        if (auth_shadow_pwexpired(authctxt))
            authctxt->force_pwchange = 1;
    }
#endif
#endif /* L7_SSHD */
    result = sys_auth_passwd(authctxt, password);
    if (authctxt->force_pwchange)
        disable_forwarding();
    return (result && ok);
}

#ifdef BSD_AUTH
static void
warn_expiry(Authctxt *authctxt, auth_session_t *as)
{
    char buf[256];
    quad_t pwtimeleft, actimeleft, daysleft, pwwarntime, acwarntime;

    pwwarntime = acwarntime = TWO_WEEKS;

    pwtimeleft = auth_check_change(as);
    actimeleft = auth_check_expire(as);
#ifdef HAVE_LOGIN_CAP
    if (authctxt->valid) {
        pwwarntime = login_getcaptime(lc, "password-warn", TWO_WEEKS,
            TWO_WEEKS);
        acwarntime = login_getcaptime(lc, "expire-warn", TWO_WEEKS,
            TWO_WEEKS);
    }
#endif
    if (pwtimeleft != 0 && pwtimeleft < pwwarntime) {
        daysleft = pwtimeleft / DAY + 1;
        snprintf(buf, sizeof(buf),
            "Your password will expire in %lld day%s.\n",
            daysleft, daysleft == 1 ? "" : "s");
#ifndef L7_SSHD
        buffer_append(&loginmsg, buf, strlen(buf));
#else
        buffer_append(&loginmsg[authctxt->cn], buf, strlen(buf));
#endif /* L7_SSHD */
    }
    if (actimeleft != 0 && actimeleft < acwarntime) {
        daysleft = actimeleft / DAY + 1;
        snprintf(buf, sizeof(buf),
            "Your account will expire in %lld day%s.\n",
            daysleft, daysleft == 1 ? "" : "s");
#ifndef L7_SSHD
        buffer_append(&loginmsg, buf, strlen(buf));
#else
        buffer_append(&loginmsg[authctxt->cn], buf, strlen(buf));
#endif /* L7_SSHD */
    }
}

int
sys_auth_passwd(Authctxt *authctxt, const char *password)
{
    struct passwd *pw = authctxt->pw;
    auth_session_t *as;
    static int expire_checked = 0;

    as = auth_usercheck(pw->pw_name, authctxt->style, "auth-ssh",
        (char *)password);
    if (as == NULL)
        return (0);
    if (auth_getstate(as) & AUTH_PWEXPIRED) {
        auth_close(as);
        disable_forwarding();
        authctxt->force_pwchange = 1;
        return (1);
    } else {
        if (!expire_checked) {
            expire_checked = 1;
            warn_expiry(authctxt, as);
        }
        return (auth_close(as));
    }
}
#elif !defined(CUSTOM_SYS_AUTH_PASSWD)
int
sys_auth_passwd(Authctxt *authctxt, const char *password)
{
  struct passwd *pw = authctxt->pw;
#ifndef L7_SSHD
  char *encrypted_password;

  /* Just use the supplied fake password if authctxt is invalid */
  char *pw_password = authctxt->valid ? shadow_pw(pw) : pw->pw_passwd;

  /* Check for users with no password. */
  if (strcmp(pw_password, "") == 0 && strcmp(password, "") == 0)
    return (1);

  /* Encrypt the candidate password using the proper salt. */
  encrypted_password = xcrypt(password,
                              (pw_password[0] && pw_password[1]) ? pw_password : "xx");

  /*
   * Authentication is accepted if the encrypted passwords
   * are identical.
   */
  return (strcmp(encrypted_password, pw_password) == 0);
#else
  L7_uint32           rc, accessLevel, UnitIndex = 1;
  L7_BOOL             challengeFlag = L7_FALSE;
  static L7_uchar8    challengeState[L7_USER_MGR_STATE_FIELD_SIZE + 1];
    L7_uint32           challengeFlags = 0;
  int                 ret;
  char               *ipAddr;
    L7_uint32           loginType;

  memset(challengeState, 0, sizeof(challengeState));
    memset(pw->challengePhrase, 0, (L7_USER_MGR_MAX_CHALLENGE_PHRASE_LENGTH + 1));

    accessLevel = L7_LOGIN_ACCESS_READ_ONLY;
    debug("sys_auth_passwd: authenticating user \"%s\", password \"%s\"", pw->username, "xxxx");

  /* Here, we try to login with the password given by the client.  If the client has a zero
   * length password, don't let it count against the lockout count, since it may be a function of
   * the client, not the user.
   */
  if (strlen(pw->password) == 0)
  {
    loginType = L7_LOGIN_TYPE_SERIAL;
  }
  else
  {
    loginType = L7_LOGIN_TYPE_SSH;
  }

  ipAddr = (L7_uchar8 *)get_remote_ipaddr(authctxt->cn);

  rc = usmDbAuthenticateUserAllowChallenge(UnitIndex, pw->username, (L7_char8 *)password,
                                           L7_USER_MGR_COMPONENT_ID, &accessLevel, ACCESS_LINE_SSH,
                                           ACCESS_LEVEL_LOGIN,
                                           L7_NULLPTR, ipAddr,
                                           challengeState, &challengeFlag, pw->challengePhrase,
                                           &challengeFlags);
  if (rc == L7_SUCCESS || (rc == L7_FAILURE && challengeFlag == L7_TRUE))
    /*
      if (usmDbLoginAuthenticateUser(1, uname, pw, &accessLevel) == L7_SUCCESS)
    */
  {
    osapiStrncpySafe(pw->password, password, ((strlen(password) >= L7_PASSWORD_SIZE) ? L7_PASSWORD_SIZE : strlen(password)+1));
        pw->accessLvl = accessLevel;

    if (rc == L7_SUCCESS)
    {
            debug("sys_auth_passwd: user \"%s\", password \"%s\" authenticated", pw->username, "xxxxx");
    }
    else
    {
            debug("sys_auth_passwd: user \"%s\", password \"%s\" authenticated, but challenged", pw->username, "xxxxx");
    }
    ret = 1;
  }
  else
  {
        debug("sys_auth_passwd: user \"%s\", password \"%s\" NOT authenticated", pw->username, "xxxx");
    ret = 0;
#if 0 /* There is already a trap generated in user manager. We don't need duplicate traps */
    usmDbTrapMgrInvalidUserLoginAttemptedTrapSend("CLI", ipAddr);
#endif
  }

  return (ret);
#endif /* L7_SSHD */
}
#endif
