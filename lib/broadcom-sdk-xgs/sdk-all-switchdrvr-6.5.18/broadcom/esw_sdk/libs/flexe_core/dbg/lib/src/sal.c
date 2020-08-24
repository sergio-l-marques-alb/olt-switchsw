
/******************************************************************************
*INCLUDE FILES
******************************************************************************/
/*#include "global_variable.h"*/
#include "global_macro.h"
/*#include "debug_macro.h"*/
#include "sal.h"
#include "hal.h"
#include "datatype.h"
#include "reg_operate.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef OS_LINUX
#include <unistd.h>
#include <sys/time.h>
#endif

/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/



/******************************************************************************
*VARIABLE DEFINE
******************************************************************************/
UINT_32 flag = 1;



/******************************************************************************
*LOCAL FUNCTION PROTOTYPE DECLARATION
******************************************************************************/
void self_check(void);

#ifdef SV_TEST
extern soft_delay(int);
#endif


/******************************************************************************
*GLOBAL FUNCTION DEFINITION
******************************************************************************/


void time_delay_ns(unsigned long int delay_conut)
{

#ifndef SV_TEST
#ifdef OS_LINUX
    unsigned long int i;
   
    delay_conut = delay_conut*2;

    
    while(delay_conut > 0)
    {
        for(i = 0; i < delay_conut;)
        {
            i++;
        }
        delay_conut--;
    }
#else
    while( delay_conut > 0 )
    {
        delay_conut-- ;
    }
#endif
#else
    soft_delay(delay_conut);
#endif


    
}

void mod_print(const char *data, ...) 
{
   printf(data);
}

int sys_init(void)
{
   return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*        RESERVED
*
* DESCRIPTION
*
*       RESERVED
*
* NOTE
*
*        RESERVED
*
* PARAMETERS
*
*       RESERVED
*
* RETURNS
*
*       RESERVED
*
* CALLS
*
*       RESERVED
*
* CALLED BY
*
*        RESERVED
*
* VERSION
*
*    <author>        <data>            <CR_ID>        <DESCRIPTION>
*      xuzhisheng      2012-02-20                      initial
*
******************************************************************************/
int parse_line(char *line, char *argv[])
{
    int nargs = 0;
    static char str[STR_BUF_SIZE];    /*STR_BUF_SIZE=256*/
    char *stringBuf = str;

    strncpy(str, line, sizeof(str)-1);

    while (nargs < CFG_MAXARGS)    /*CFG_MAXARGS=16*/
    {
        /* skip any white space */
        while ((*stringBuf == ' ') || (*stringBuf == '\t'))
        {
            ++stringBuf;
        }

        if (*stringBuf == '\0')
        {
            /* end of line, no more args    */
            argv[nargs] = NULL;
            return (nargs);
        }

        argv[nargs++] = stringBuf;    /* begin of argument string    */

        /* find end of string */
        while (*stringBuf && (*stringBuf != ' ') && (*stringBuf != '\t'))
        {
            ++stringBuf;
        }

        if (*stringBuf == '\0')
        {
            /* end of line, no more args    */
            argv[nargs] = NULL;
            return (nargs);
        }

        *stringBuf++ = '\0';        /* terminate current arg     */
    } /*while (nargs < CFG_MAXARGS)*/

    return (nargs);
}



/******************************************************************************
*
* FUNCTION
*
*        RESERVED
*
* DESCRIPTION
*
*       RESERVED
*
* NOTE
*
*        RESERVED
*
* PARAMETERS
*
*       RESERVED
*
* RETURNS
*
*       RESERVED
*
* CALLS
*
*       RESERVED
*
* CALLED BY
*
*        RESERVED
*
* VERSION
*
*    <author>        <data>            <CR_ID>        <DESCRIPTION>
*      xuzhisheng      2012-02-20                      initial
*
******************************************************************************/
void table_print(char *string, unsigned long int print_val)
{
    UINT_8 i_cnt;
    const char *p_string[4] = {"begin", "BEGIN", "end", "END"};
    
    if ((0 == strcmp(string,p_string[0])) || (0 == strcmp(string,p_string[1]))
        || (0 == strcmp(string,p_string[2])) || (0 == strcmp(string,p_string[3])))
    {
        printf("+");
        for (i_cnt = 1; i_cnt < 59; i_cnt++)
        {
            printf("-");
        }
        printf("+");
        printf("\n");
    }
    else
    {
        if (flag)
        {
            printf("|%-45s", string);
            printf("|0x%-10lx", print_val);
            printf("|");
            printf("\n");
        }
        else 
        {
            printf("|%-45s", string);
            printf("|%-12ld", print_val);
            printf("|");
            printf("\n");
        }
    }
}



/******************************************************************************
*
* FUNCTION
*
*        RESERVED
*
* DESCRIPTION
*
*       RESERVED
*
* NOTE
*
*        RESERVED
*
* PARAMETERS
*
*       RESERVED
*
* RETURNS
*
*       RESERVED
*
* CALLS
*
*       RESERVED
*
* CALLED BY
*
*        RESERVED
*
* VERSION
*
*    <author>        <data>            <CR_ID>        <DESCRIPTION>
*      xuzhisheng      2012-02-20                      initial
*
******************************************************************************/
void time_delay(unsigned long int delay_conut)
{
#ifndef SV_TEST
    while( delay_conut > 0 )
    {
        delay_conut-- ;
    }

#else
    soft_delay(delay_conut);
#endif
}

/******************************************************************************
*
* FUNCTION
*
*     diag_print_title
*
* DESCRIPTION
*
*     Print diag diagram title.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     RESERVED
*
* RETURNS
*
*     RESERVED
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <date>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-29    1.0           initial
*
******************************************************************************/
void diag_print_title(const char *title, ...)
{
    int i = 0;
    int len = 0;
    char str_buff[81];
    va_list args;

    va_start(args, title);
    len = vsnprintf(str_buff, 81, title, args);
    len = (len > 80) ? 80 : len;
    va_end(args);

    printf("|");
    for (i = 0; i < 53 - len/2; i++) printf(" ");
    printf("%s", str_buff);
    for (i = 0; i < 54 - (len - len/2); i++) printf(" ");
    printf("|\n");
}

/******************************************************************************
*
* FUNCTION
*
*     diag_print_2_title
*
* DESCRIPTION
*
*     Print diag diagram title.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     RESERVED
*
* RETURNS
*
*     RESERVED
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <date>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-29    1.0           initial
*
******************************************************************************/
void diag_print_2_title(const char *title1, const char *title2)
{
    int i = 0;
    int len = 0;

    len = strlen(title1);
    printf("|");
    for (i = 0; i < 26 - len/2; i++) printf(" ");
    printf("%s", title1);
    for (i = 0; i < 27 - (len - len/2); i++) printf(" ");

    len = strlen(title2);
    printf("|");
    for (i = 0; i < 26 - len/2; i++) printf(" ");
    printf("%s", title2);
    for (i = 0; i < 27 - (len - len/2); i++) printf(" ");

    printf("|\n");
}

/******************************************************************************
*
* FUNCTION
*
*     diag_cnt_val_to_str
*
* DESCRIPTION
*
*     Diag count value to string.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     RESERVED
*
* RETURNS
*
*     RESERVED
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <date>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-29    1.0           initial
*
******************************************************************************/
void diag_cnt_val_to_str(unsigned   int *cnt_val, int val_dep, int is_hex, char *str_buff, int buff_len)
{
    UINT_64 val64 = 0;

     /* support 1~64bit */
    switch (val_dep)
    {
        case 1 : val64 = *cnt_val; break;
        case 2 : val64 = *(cnt_val + 1); val64 <<= 32; val64 += *cnt_val; break;
        default: break;
    }
   
     /* 0-dec, 1-hex */
    switch (is_hex)
    {
        case 0 : snprintf(str_buff, buff_len, "%llu", val64); break;
        case 1 : snprintf(str_buff, buff_len, "0x%llx", val64); break;
        default: break;
    }
}

/******************************************************************************
*
* FUNCTION
*
*     diag_print_cnt
*
* DESCRIPTION
*
*     Print diag count.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     RESERVED
*
* RETURNS
*
*     RESERVED
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <date>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-29    1.0           initial
*
******************************************************************************/
void diag_print_cnt(const char *cnt1, char *val1, const char *cnt2, char *val2)
{
    if (NULL != cnt1 && NULL != cnt2)
        printf("| %28s = %-20s | %28s = %-20s |\n", cnt1, val1, cnt2, val2);
    else if (NULL == cnt1 && NULL != cnt2)
        printf("| %28s   %-20s | %28s = %-20s |\n", " ", " ", cnt2, val2);
    else if (NULL != cnt1 && NULL == cnt2)
        printf("| %28s = %-20s | %28s   %-20s |\n", cnt1, val1, " ", " ");
    else
        printf("| %28s   %-20s | %28s   %-20s |\n", " ", " ", " ", " ");
}

/******************************************************************************
*
* FUNCTION
*
*     diag_print_line
*
* DESCRIPTION
*
*     Print diag line.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     RESERVED
*
* RETURNS
*
*     RESERVED
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <date>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-29    1.0           initial
*
******************************************************************************/
void diag_print_line(void)
{
    printf("+--------------------------+--------------------------+--------------------------+--------------------------+\n");
}

