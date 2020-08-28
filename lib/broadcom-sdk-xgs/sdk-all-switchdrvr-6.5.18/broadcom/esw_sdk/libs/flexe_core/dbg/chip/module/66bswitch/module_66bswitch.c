



/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hal.h"
#include "sal.h"
#include "reg_operate.h"
#include "b66switch.h"
#include "global_macro.h"
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int module_66bswitch_para_debug = 0;
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
*
* FUNCTION b66switch_init
*
*     
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*      chip_id:chip number used
*
* RETURNS
*
*     0: success
*     1: fail
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma        2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS b66switch_init(UINT_8 chip_id)
{
    UINT_8 chnl_id = 0;
    UINT_8 offset_id = 0;
    UINT_8 ram_id = 0;
    UINT_32 swram_value = 0xffff;
    for (chnl_id = 0; chnl_id < 3;chnl_id++)
    {
        for (offset_id = 0; offset_id < 82; offset_id++)
        {
            for (ram_id = 0; ram_id < 3; ram_id++)
            {
                b66switch_swram_set(chip_id,chnl_id,offset_id,ram_id,&swram_value);
            }
        }
    }

   /** enable protect */
    b66switch_protect_en_set(chip_id,1);

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION b66switch_dump
*
*     
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*      chip_id:chip number used
*
* RETURNS
*
*     0: success
*     1: fail
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    kejian        2018-3-31    1.0           initial
*
******************************************************************************/
RET_STATUS b66switch_dump(UINT_8 chip_id)
{    
    UINT_8 offset = 0;
    UINT_32 parameter = 0;
    
    if (chip_id > DEV_NUM - 1)
    {
        printf("[%s] invalid parameter! chip_id = %d", __FUNCTION__,chip_id);

        return RET_PARAERR;
    }

    printf("[B66SWITCH DIAG]\r\n");
    printf("+--------------------------------------------+\r\n");
    for (offset = 0; offset < CH_NUM; offset++)
    {    
        parameter = 0;
        b66switch_swram_get(chip_id, 0, offset, 0, &parameter);

        if (parameter != 0xff)
        {
            if (parameter < 80)
            {
                printf("flexe ch = %2d ---> flexe ch = %2d \r\n",offset,parameter);
            }
            else if (parameter >= 80 && parameter <= 159)
            {
                printf("flexe ch = %2d --->  mac  ch = %2d \r\n",offset,parameter - 80);
            }
            else
            {
                printf("flexe ch = %2d --->  sar  ch = %2d \r\n",offset,parameter - 160);
            }
        }
    }
    printf("+--------------------------------------------+\r\n");
    for (offset = 0; offset < CH_NUM; offset++)
    {    
        parameter = 0;
        b66switch_swram_get(chip_id, 0, offset, 1, &parameter);

        if (parameter != 0xff)
        {
            if (parameter < 80)
            {
                printf("mac ch = %2d   ---> flexe ch = %2d \r\n",offset,parameter);
            }
            else if (parameter >= 80 && parameter <= 159)
            {
                printf("mac ch = %2d   --->  mac  ch = %2d \r\n",offset,parameter - 80);
            }
            else
            {
                printf("mac ch = %2d   --->  sar  ch = %2d \r\n",offset,parameter - 160);
            }
        }
    }
    printf("+--------------------------------------------+\r\n");

    for (offset = 0; offset < CH_NUM; offset++)
    {    
        parameter = 0;
        b66switch_swram_get(chip_id, 0, offset, 2, &parameter);

        if (parameter != 0xff)
        {
            if (parameter < 80)
            {
                printf("sar ch = %2d   ---> flexe ch = %2d \r\n",offset,parameter);
            }
            else if (parameter >= 80 && parameter <= 159)
            {
                printf("sar ch = %2d   --->  mac  ch = %2d \r\n",offset,parameter - 80);
            }
            else
            {
                printf("sar ch = %2d   --->  sar  ch = %2d \r\n",offset,parameter - 160);
            }
        }
    }
    printf("+--------------------------------------------+\r\n");
    return RET_SUCCESS;
}




/******************************************************************************
*
* FUNCTION b66switch_cfg_new
*
*     
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*      chip_id:chip number used
*      src_ch:the channel of src port
*      dest_ch:the channel of dest port 
*     ch:flexe:0~79;mac:80~159;sar:160~241
* RETURNS
*
*     0: success
*     1: fail
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma        2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS b66switch_cfg(UINT_8 chip_id,UINT_8 add_del,UINT_8 ram_position,UINT_8 src_ch,UINT_8 dest_ch)
{
    UINT_32 parameter = 0;
    UINT_8 ram_id = 0;
    UINT_8 offset = 0;
	
    if (chip_id > MAX_DEV || add_del > 1 || ram_position > 2 || src_ch > 241 || dest_ch > 241)
    {
        if (module_66bswitch_para_debug)
        {
            printf("[%s]:%d,chip_id=%d,add_del=%d,ram_position=%d,src_ch=%d,dest_ch=%d\r\n",__FUNCTION__,__LINE__,
                chip_id,add_del,ram_position,src_ch,dest_ch);
        }

        return RET_PARAERR;
    }

    if (1 == add_del)
    {
        parameter = dest_ch;
    }
    else 
    {
        parameter = 0xff;
    }
    
    if (src_ch <= 79)
    {
        ram_id = 0;
        offset = src_ch;
    }
    else if (src_ch >= 80 && src_ch <= 159)
    {
        ram_id = 1;
        offset = src_ch - 80;
    }
    else
    {
        ram_id = 2;
        offset = src_ch - 160;
    }
	
    b66switch_swram_set(chip_id,ram_position,offset,ram_id,&parameter);
    
    return RET_SUCCESS;
}



/******************************************************************************
*
* FUNCTION
*
*     b66switch debug function.
*
* DESCRIPTION
*
*     RESERVED
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
*     0: success
*     1: fail
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
*    <author>        <date>            <CR_ID>         <DESCRIPTION>
*      kejian        2018-04-02         1.0            initial
*
******************************************************************************/
RET_STATUS b66switch_help(void)
{
    printf("+----------------------b66switch initialized function---------------------------------------+\r\n");
    printf("b66switch_init(UINT_8 chip_id)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("+-------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------b66switch swram dump function----------------------------------------------+\r\n");
    printf("b66switch_dump(UINT_8 chip_id)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("+-------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    return RET_SUCCESS;
}


#ifdef __cplusplus
}
#endif  
