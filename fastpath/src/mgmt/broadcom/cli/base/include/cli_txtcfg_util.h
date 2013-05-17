#ifndef CLI_TXT_CFG_H
#define CLI_TXT_CFG_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "commdefs.h"
#include "datatypes.h"
#include "comm_structs.h"

#ifndef CONFIG_SCRIPT_MAX_COMMAND_SIZE
#define CONFIG_SCRIPT_MAX_COMMAND_SIZE 1024
#endif

#define CLI_TXTCFG_UNCOMP_BUFSIZE (CONFIG_SCRIPT_MAX_COMMAND_SIZE + CONFIG_SCRIPT_MAX_COMMAND_SIZE/1000 + 12)
#undef CLI_TXTCFG_UNCOMP_BUFSIZE
#define CLI_TXTCFG_UNCOMP_BUFSIZE (CONFIG_SCRIPT_MAX_COMMAND_SIZE * 2)

typedef struct
{
  L7_int32 filedesc;
  char filename[32];
  L7_fileHdr_t cfgHdr;
  L7_char8 cmdBuf[CLI_TXTCFG_UNCOMP_BUFSIZE];
  L7_int32 length;
  L7_int32 offset;
} cliTxtCfgCmd_t;

L7_RC_t cliTxtCfgCmdInit (L7_char8 * filename, cliTxtCfgCmd_t * cmdp);
void cliTxtCfgCmdEnd (cliTxtCfgCmd_t * cmdp);
L7_char8 *cliTxtCfgCmdGet (cliTxtCfgCmd_t * cmdp, L7_BOOL skipComment);
L7_RC_t cliTxtCfgWriteBlock (L7_int32 fd, L7_char8 * block, L7_uint32 * lenp, L7_BOOL useComp);
L7_RC_t cliTxtCfgCopyConfig (L7_char8 * readFile, L7_char8 * writeFile, L7_BOOL useComp);
void cliTxtCfgSetConfigFlag(L7_BOOL flag);
L7_BOOL cliTxtCfgGetConfigFlag(void);

#endif
