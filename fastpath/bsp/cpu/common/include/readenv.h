#ifndef __READENV_H__
#define __READENV_H__

//*****************************************************************************

#define BOOTENV_MAX     (1*1024)

//*****************************************************************************
//* Code
//*****************************************************************************

#ifdef __cplusplus
#define _EXTERN_ extern "C"
#else
#define _EXTERN_ extern
#endif

_EXTERN_ int open_bootenv(void);

_EXTERN_ int close_bootenv(void);

_EXTERN_ int print_bootenv(void);

_EXTERN_ int read_bootenv(char *token, char *value);

//*****************************************************************************

#endif
