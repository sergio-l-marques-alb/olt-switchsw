#ifndef __BOXS_DEBUG__HEADER__
#define __BOXS_DEBUG__HEADER__


typedef enum
{

  BOXS_DBG_FLAG_cpConfigGroup = 1,      /* 1 */
  BOXS_DBG_FLAG_cpGeneralGroup, /* 2 */
  BOXS_DBG_FLAG_LAST_ENTRY /* 16 */
} BoxsDebugTraceFlags_t;

#define boxsDebugTrace(__flag__, __args__... )      \
{                                                   \
    if (boxsDebugTraceCfg[__flag__] == L7_TRUE)     \
    {                                               \
         sysapiPrintf( __args__);                \
    }                                               \
}

void boxsDebugTraceDump(void);

void boxsDebugTraceSet(L7_uint32 flag);

void boxsDebugTraceClear(L7_uint32 flag);

void boxsDebugTraceLinkerAssist(void);

extern L7_uint32 boxsDebugTraceCfg[BOXS_DBG_FLAG_LAST_ENTRY];

#endif /* __BOXS_DEBUG__HEADER__ */
