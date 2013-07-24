#ifndef L7_DEFINES_H
#define L7_DEFINES_H

#include "cli_web_exports.h"
#include "sshd_exports.h"
#include "user_manager_exports.h"

struct passwd
{
    unsigned char username[L7_SSHC_USERNAME_SIZE_MAX];
    unsigned char password[L7_SSHC_PASSWORD_SIZE_MAX];
    unsigned char challengePhrase[L7_USER_MGR_MAX_CHALLENGE_PHRASE_LENGTH + 1];
    unsigned int  accessLvl;
};

#endif /* L7_DEFINES_H */
