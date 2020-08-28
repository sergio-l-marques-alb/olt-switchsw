

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "datatype.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal.h"
#include "sal.h"
#include "reg_operate.h"

/******************************************************************************
*LOCAL FUNCTION PROTOTYPE DECLARATION
******************************************************************************/
#ifdef SV_TEST
extern void soft_regp_read(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32* u32_data);
extern void soft_regp_write(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32 u32_data);
extern void soft_ram_write(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_ram_addr,UINT_32 u32_offset_addr,UINT_32 width,UINT_32* data);
extern void soft_ram_read(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_ram_addr,UINT_32 u32_offset_addr,UINT_32* data,UINT_32 cnt);
extern void soft_ram_write_single(UINT_8 chip_id,CHIP_RAM chip_ram,UINT_32 offset_addr,UINT_32* data);
extern void soft_ram_read_single(UINT_8 chip_id,CHIP_RAM chip_ram,UINT_32 offset_addr,UINT_32* data);
extern void soft_regp_write_mask(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32 u32_data,UINT_32 mask);
extern void lock_regp();
extern void unlock_regp();
#else
int flexe_acc_write(int unit, unsigned int offset, unsigned int *data, unsigned int nof_words);
int flexe_acc_read(int unit, unsigned int offset, unsigned int *data, unsigned int nof_words);


#endif


/* for debug use */
const char *bit_start = "+---";
const char *bit_middle = "----";
const char *bit_end = "---+";

#ifdef OS_LINUX
pthread_mutex_t MUTEX_REG_R_W = PTHREAD_MUTEX_INITIALIZER;
#endif

/******************************************************************************
*
* FUNCTION
*
*	  regp_read
*
* DESCRIPTION
*
*	  opuc reorder page cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base address of the register to write
*	  u32_offset_addr:the offset address of the register to write
*	  *u32_data:the pointer to the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS regp_read(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32* u32_data)
{
	int irv = 0;
	if (NULL == u32_data)
	{
		printf("invalid parameter pointer!!\r\n");

		return RET_PARAERR;
	}
#ifndef SV_TEST

	
#ifdef OS_LINUX
			pthread_mutex_lock(&MUTEX_REG_R_W);
#endif

	irv = flexe_acc_read((int)chip_id, u32_base_addr + u32_offset_addr, (unsigned int *)u32_data, 1);

#ifdef OS_LINUX
	pthread_mutex_unlock(&MUTEX_REG_R_W);
#endif

#else

#ifdef OS_LINUX
    lock_regp();
#else
	
#endif
	soft_regp_read(chip_id,u32_base_addr,u32_offset_addr,u32_data);
#ifdef OS_LINUX
    unlock_regp();
#else
    
#endif

#endif
	return irv;
}


RET_STATUS regp_read_print(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr)
{
#ifndef SV_TEST
    UINT_32 u32_data = 0;
    UINT_8 data[32] = {0}; 
    SINT_8 i = 0;    

	regp_read(chip_id,u32_base_addr, u32_offset_addr, &u32_data);
	
    table_print((char *)("begin"), 0);
    table_print((char *)("u32_read_data"), (UINT_32)u32_data);
    table_print((char *)("end"), 0);

    for(i = 0; ; i++)
    {
        data[i] = u32_data%2;
        u32_data = u32_data/2;
        if(u32_data == 0)
        {
            break;
        }

    }

    if((i + 1) < 32)
    {
        for(i = i + 1; i < 32; i++)
        {
            data[i] = 0;
        }
    }
    printf("+-----------------------------------------------+\n");
    printf("+");
    for(i=31;i>=16;i--)
    {
        printf("%2d|",i);
    }
    printf("\n");
    printf("+-----------------------------------------------+\n");
    printf("+");
    for(i=31;i>=16;i--)
    {
        printf("%2d|",data[i]);
    }
    printf("\n");
    printf("+-----------------------------------------------+\n");
    
    printf("+-----------------------------------------------+\n");
    printf("+");
    for(i=15;i>=0;i--)
    {
        printf("%2d|",i);
        if(i == 0)
        {
            break;
        }
    }
    printf("\n");
    printf("+-----------------------------------------------+\n");
    printf("+");
    for(i=15;i>=0;i--)
    {
        printf("%2d|",data[i]);
    }
    printf("\n");
    printf("+-----------------------------------------------+\n");

#endif
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  regp_read_mul
*
* DESCRIPTION
*
*	  read multi registers
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base address of the register to write
*	  u32_offset_addr:the offset address of the register to write
*	  lenth:number of register to read
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS regp_read_mul(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32 num)
{
#ifndef SV_TEST
	unsigned long int i_cnt;
	UINT_32 u32_data = 0;
	
	if(num == 0)  
	{
		num = 16;
	}
	
	if (num > 32)
	{
		num = 32;
	}

	printf("+-------------------+\n");
	for(i_cnt = 0 ; i_cnt < num ; i_cnt++)
	{	
		regp_read(chip_id,u32_base_addr,u32_offset_addr+i_cnt,&u32_data);
		printf("|");
		printf("0x%lx", u32_base_addr+u32_offset_addr+i_cnt);
		printf("\t");
		printf(" |");
		printf("0x%08x", u32_data);
		printf("|");
		printf("\n");
	}
	printf("+-------------------+\n");
		
#endif
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  regp_write
*
* DESCRIPTION
*
*	  opuc reorder page cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base address of the register to write
*	  u32_offset_addr:the offset address of the register to write
*	  u32_data:the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS regp_write(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32 u32_data)
{
	int irv = 0;
#ifndef SV_TEST

	UINT_32 uidata = u32_data;

#ifdef OS_LINUX
	pthread_mutex_lock(&MUTEX_REG_R_W);
#endif
	
	irv = flexe_acc_write((int)chip_id, u32_base_addr + u32_offset_addr, (unsigned int *)&uidata, 1);

#ifdef OS_LINUX
	pthread_mutex_unlock(&MUTEX_REG_R_W);
#endif

#else
#ifdef OS_LINUX
	lock_regp();
#else
	
#endif

	soft_regp_write(chip_id,u32_base_addr,u32_offset_addr,u32_data);
#ifdef OS_LINUX
    unlock_regp();
#else
    
#endif

#endif
	return irv;
}


/******************************************************************************
*
* FUNCTION
*
*	  regp_bit_write
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base address of the register
*	  u32_offset_addr:the offset address of the register
*	  end_bit:the end bit of the register to write
*	  start_bit:the start bit of the register to write
*	  u32_data:the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS regp_bit_write(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_8 end_bit,UINT_8 start_bit,UINT_32 u32_data)
{
#ifndef SV_TEST

    UINT_32 header = 0;
    UINT_8 crc_data = 0;        
    UINT_32 mask = 0xffffffff;
    
    if (end_bit < start_bit)
    {
        return RET_PARAERR;
    }

	if ((end_bit - start_bit) < 31)
	{
		mask = (~(mask<<(end_bit - start_bit +1)))<<start_bit;
	}
	else
	{
		mask = 0xffffffff;
	}

    header = (1 << 16) | (1 << 9) | 0x40 ;
    crc_caculate(header,&crc_data);
    header = (((UINT_32)crc_data) << 24) | (1 << 16) | (1 << 9) | 0x40 ;
    u32_data = u32_data << start_bit;
#ifdef OS_LINUX
	pthread_mutex_lock(&MUTEX_REG_R_W);
#endif
	
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,0,header);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,1,u32_base_addr+u32_offset_addr);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,2,mask);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,0x84,u32_data);

#ifdef OS_LINUX
	pthread_mutex_unlock(&MUTEX_REG_R_W);
#endif

#else
	
	UINT_32 mask = 0xffffffff;
	
	if (end_bit < start_bit)
	{
		return RET_PARAERR;
	}

	if (end_bit - start_bit < 31)
	{
		mask = (~(mask<<(end_bit - start_bit +1)))<<start_bit;
	}
	else
	{
		mask = 0xffffffff;
	}
	
	u32_data = u32_data << start_bit;

#ifdef OS_LINUX
    lock_regp();
#else
	
#endif
	soft_regp_write_mask(chip_id,u32_base_addr,u32_offset_addr,u32_data,mask);
#ifdef OS_LINUX
    unlock_regp();
#else
    
#endif
	
#endif
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  regp_field_write
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  chip_reg:the register to write
*	  u32_data:the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS regp_field_write(UINT_8 chip_id,CHIP_REG chip_reg,UINT_32 u32_data)
{
#ifndef SV_TEST

    UINT_32 mask = 0xffffffff;
    UINT_32 header = 0;
    UINT_8 crc_data = 0;

	if (chip_reg.end_bit < chip_reg.start_bit)
	{
		return RET_PARAERR;
	}

    if (chip_reg.end_bit - chip_reg.start_bit < 31)
    {
        mask = (~(mask<<(chip_reg.end_bit - chip_reg.start_bit +1)))<<chip_reg.start_bit;
    }
    else
    {
        mask = 0xffffffff;
    }
    
    header = (1 << 16) | (1 << 9) | 0x40 ;
    crc_caculate(header,&crc_data);
    header = (((UINT_32)crc_data) << 24) | (1 << 16) | (1 << 9) | 0x40 ;
    u32_data = u32_data << chip_reg.start_bit;
#ifdef OS_LINUX
	pthread_mutex_lock(&MUTEX_REG_R_W);
#endif
	
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,0,header);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,1,chip_reg.base_addr+chip_reg.offset_addr);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,2,mask);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,0x84,u32_data);
	
#ifdef OS_LINUX
	pthread_mutex_unlock(&MUTEX_REG_R_W);
#endif

#else

	UINT_32 mask = 0xffffffff;
	
	if (chip_reg.end_bit < chip_reg.start_bit)
	{
		return RET_PARAERR;
	}

	if (chip_reg.end_bit - chip_reg.start_bit < 31)
	{
		mask = (~(mask<<(chip_reg.end_bit - chip_reg.start_bit +1)))<<chip_reg.start_bit;
	}
	else
	{
		mask = 0xffffffff;
	}
	
	u32_data = u32_data << chip_reg.start_bit;

#ifdef OS_LINUX
		lock_regp();
#else
		
#endif
		soft_regp_write_mask(chip_id,chip_reg.base_addr,chip_reg.offset_addr,u32_data,mask);
#ifdef OS_LINUX
		unlock_regp();
#else
		
#endif

#endif
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  regp_bit_read
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base addr of the register
*	  u32_offset_addr:the offset addr of the register
*	  end_bit:the end bit of the register to read
*	  start_bit:the start bit of the register to read
*	  *u32_data:the pointer to the data to read
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS regp_bit_read(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_8 end_bit,UINT_8 start_bit,UINT_32* u32_data)
{
	UINT_32 read_value = 0;
	UINT_32 mask = 0xffffffff;

	if (NULL == u32_data)
	{
		printf("invalid parameter pointer!!\r\n");

		return RET_PARAERR;
	}
	
	if (end_bit < start_bit)
	{
		return RET_PARAERR;
	}

	if ((end_bit - start_bit) < 31)
	{
		mask = ~(mask<<(end_bit - start_bit +1));
	}
	else
	{
		mask = 0xffffffff;
	}
	
	regp_read(chip_id,u32_base_addr,u32_offset_addr,&read_value);
	*u32_data = (read_value >> start_bit) & mask;
	return RET_SUCCESS;
}


RET_STATUS regp_bit_read_print(UINT_8 chip_id,UINT_32 u32_base_addr, UINT_32 u32_offset_addr, UINT_8 end_bit, UINT_8 start_bit)
{
#ifndef SV_TEST
	UINT_32 read_data;
	
	regp_bit_read(chip_id,u32_base_addr, u32_offset_addr, end_bit, start_bit, &read_data);
	
    table_print((char *)("begin"), 0);
    table_print((char *)("read_data"), (UINT_32)read_data);
    table_print((char *)("end"), 0);
        
#endif
	return RET_SUCCESS;

}

/******************************************************************************
*
* FUNCTION
*
*	  regp_field_read
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  chip_reg:the register to read
*	  *u32_data:the pointer to the data to read
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS regp_field_read(UINT_8 chip_id,CHIP_REG chip_reg,UINT_32* u32_data)
{
	UINT_32 read_value = 0;
	UINT_32 mask = 0xffffffff;

	if (NULL == u32_data)
	{
		printf("invalid parameter pointer!!\r\n");

		return RET_PARAERR;
	}
	
	if (chip_reg.end_bit < chip_reg.start_bit)
	{
		return RET_PARAERR;
	}

	if (chip_reg.end_bit - chip_reg.start_bit < 31)
	{
		mask = ~(mask<<(chip_reg.end_bit - chip_reg.start_bit +1));
	}
	else
	{
		mask = 0xffffffff;
	}

	regp_read(chip_id,chip_reg.base_addr,chip_reg.offset_addr,&read_value);
	*u32_data = (read_value >> chip_reg.start_bit) & mask;
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  crc_caculate
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	value:the value to caculate crc
*	char* b:the pointer to the crc
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
void crc_caculate(unsigned int value,unsigned char* b)
{
	unsigned int i = 0;
	unsigned char d[24] = {0};
	unsigned char newcrc[8];
	
	for (i = 0; i < 24; i++)
	{
		d[i] = (value >> i) & 1;
	}
	 newcrc[0] = d[23] ^ d[21] ^ d[19] ^ d[18] ^ d[16] ^ d[14] ^ d[12] ^ d[8 ] ^ d[7 ] ^ d[6 ] ^ d[0 ];
     newcrc[1] = d[23] ^ d[22] ^ d[21] ^ d[20] ^ d[18] ^ d[17] ^ d[16] ^ d[15] ^ d[14] ^ d[13] ^ d[12] ^ d[9] ^ d[6] ^ d[1] ^ d[0];
     newcrc[2] = d[22] ^ d[17] ^ d[15] ^ d[13] ^ d[12] ^ d[10] ^ d[8 ] ^ d[6 ] ^ d[2 ] ^ d[1 ] ^ d[0 ];
     newcrc[3] = d[23] ^ d[18] ^ d[16] ^ d[14] ^ d[13] ^ d[11] ^ d[9 ] ^ d[7 ] ^ d[3 ] ^ d[2 ] ^ d[1 ];
     newcrc[4] = d[19] ^ d[17] ^ d[15] ^ d[14] ^ d[12] ^ d[10] ^ d[8 ] ^ d[4 ] ^ d[3 ] ^ d[2 ];
     newcrc[5] = d[20] ^ d[18] ^ d[16] ^ d[15] ^ d[13] ^ d[11] ^ d[9 ] ^ d[5 ] ^ d[4 ] ^ d[3 ];
     newcrc[6] = d[21] ^ d[19] ^ d[17] ^ d[16] ^ d[14] ^ d[12] ^ d[10] ^ d[6 ] ^ d[5 ] ^ d[4 ];
     newcrc[7] = d[22] ^ d[20] ^ d[18] ^ d[17] ^ d[15] ^ d[13] ^ d[11] ^ d[7 ] ^ d[6 ] ^ d[5 ];
	 for (i = 0; i < 8; i++)
	 {
		 *b = (newcrc[i] << i ) | *b;
	 }
}


/******************************************************************************
*
* FUNCTION
*
*	  ram_write
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base addr of the ram
*	  u32_ram_addr:the address of the ram
*	  u32_offset_addr:the offset of the ram
*	  width:the width of the ram
* 	  *data:the pointer to the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS ram_write(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_ram_addr,UINT_32 u32_offset_addr,UINT_32 width,UINT_32* data)
{
#ifndef SV_TEST

	UINT_32 header = 0;
	UINT_8 crc_data = 0;
	UINT_32 index = 0;
	UINT_32 payload_len = 0;

	if (NULL == data)
	{
		printf("invalid parameter pointer!!\r\n");

		return RET_PARAERR;
	}
	
	if (0 == (width % 32)) 
	{
		payload_len = width / 32;
	}
	else 
	{
		payload_len = (width  / 32) + 1;
	}
	header = (payload_len << 16) | (1 << 8) | 0x40 ;
	crc_caculate(header,&crc_data);
	header = (((UINT_32)crc_data) << 24) | (payload_len << 16) | (1 << 8) | 0x40 ;

#ifdef OS_LINUX
	pthread_mutex_lock(&MUTEX_REG_R_W);
#endif

	regp_write_for_ram(chip_id,u32_base_addr,0,header);
	regp_write_for_ram(chip_id,u32_base_addr,1,u32_ram_addr);
	regp_write_for_ram(chip_id,u32_base_addr,3,u32_offset_addr);
	
	for (index = payload_len; index >= 1; index--) 
	{
		if (1 == index) 
		{
			regp_write_for_ram(chip_id, u32_base_addr,0x84, *data);

			break;
		}
		else 
		{
			regp_write_for_ram(chip_id, u32_base_addr,0x4 + payload_len - index, *(data + (index - 1)));
		}
	}

#ifdef OS_LINUX
	pthread_mutex_unlock(&MUTEX_REG_R_W);
#endif
#else

#ifdef OS_LINUX
	lock_regp();
#else
	
#endif
	soft_ram_write(chip_id,u32_base_addr,u32_ram_addr,u32_offset_addr,width,data);
#ifdef OS_LINUX
	unlock_regp();
#else
	
#endif

#endif
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  ram_write_single
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  chip_ram: ram to write
*	  offset_addr:the offset of the ram
*	  *data:the pointer to the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS ram_write_single(UINT_8 chip_id,CHIP_RAM chip_ram,UINT_32 offset_addr,UINT_32* data)
{
#ifndef SV_TEST

	UINT_32 header = 0;
	UINT_8 crc_data = 0;
	UINT_32 index = 0;
	UINT_32 payload_len = 0;

	if (NULL == data)
	{
		printf("invalid parameter pointer!!\r\n");

		return RET_PARAERR;
	}
	
	if (0 == (chip_ram.width % 32)) 
	{
		payload_len = chip_ram.width / 32;
	}
	else 
	{
		payload_len = (chip_ram.width  / 32) + 1;
	}
	header = (payload_len << 16) | (1 << 8) | 0x40 ;
	crc_caculate(header,&crc_data);
	header = (((UINT_32)crc_data) << 24) | (payload_len << 16) | (1 << 8) | 0x40 ;
	
#ifdef OS_LINUX
	pthread_mutex_lock(&MUTEX_REG_R_W);
#endif
	
	regp_write_for_ram(chip_id,chip_ram.base_addr,0,header);
	regp_write_for_ram(chip_id,chip_ram.base_addr,1,chip_ram.ram_addr);
	regp_write_for_ram(chip_id,chip_ram.base_addr,3,offset_addr);
	
	for (index = payload_len; index >= 1; index--) 
	{
		if (1 == index) 
		{
			regp_write_for_ram(chip_id, chip_ram.base_addr,0x84, *data);

			break;
		}
		else 
		{
			regp_write_for_ram(chip_id, chip_ram.base_addr,0x4 + payload_len - index, *(data + (index - 1)));
		}
	}
		
#ifdef OS_LINUX
	pthread_mutex_unlock(&MUTEX_REG_R_W);
#endif
#else

#ifdef OS_LINUX
	lock_regp();
#else
	
#endif
	soft_ram_write_single(chip_id,chip_ram,offset_addr,data);
#ifdef OS_LINUX
	unlock_regp();
#else
	
#endif

#endif
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  ram_write_debug
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base addr of the ram
*	  u32_ram_addr:the address of the ram
*	  u32_offset_addr:the offset of the ram
*	  width:the width of the ram
* 	  *data:the pointer to the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS ram_write_debug(char * string)
{
    char *argv[CFG_MAXARGS + 1];
    UINT_8 argc = 0;
    UINT_8 index = 0;
    UINT_32 chip_id = 0,width = 0,u32_ram_addr = 0,u32_offset_addr = 0;
    UINT_32 data[128]={0};
    
    argc = parse_line(string, argv);

	sscanf(argv[0], "%x", &chip_id);
  	sscanf(argv[1], "%x", &u32_ram_addr);
	sscanf(argv[2], "%x", &u32_offset_addr);
	sscanf(argv[3], "%x", &width);
	
	for (index = 0; index < argc-4; index++)
	{
		sscanf(argv[index+4], "%x",  &data[index]);
	}
	
	ram_write((UINT_8)chip_id,TABLE_BASE_ADDR,u32_ram_addr,u32_offset_addr,width,data);
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  ram_write_test
*
* DESCRIPTION
*
*	  read multi rams
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base address of the register to write
*	  u32_offset_addr:the offset address of the register to write
*	  num:number of register to read
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS ram_write_test(UINT_8 chip_id,UINT_32 u32_addr,UINT_32 offset,UINT_32 width,UINT_32 parameter1,UINT_32 parameter2,UINT_32 parameter3)
{
#ifndef SV_TEST
	UINT_32 u32_data[128] = {0};
	u32_data[0] = parameter1;
	
	u32_data[1] = parameter2;

	u32_data[2] = parameter3;
	ram_write(chip_id,TABLE_BASE_ADDR,u32_addr,offset,width,u32_data);	
	
#endif
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  ram_read
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base address of the ram
*	  u32_ram_addr:the address of the ram
*	  u32_offset_addr:the offset of the ram
*	  *data:the pointer to the data to write
*	  cnt:ram width /32
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS ram_read(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_ram_addr,UINT_32 u32_offset_addr,UINT_32 width,UINT_32* data)
{
    UINT_8 cnt = 0;
#ifndef SV_TEST
    unsigned int index = 0;
    unsigned char crc_data = 0;
    UINT_32 header = 0;
#endif

	if (NULL == data)
	{
		printf("invalid parameter pointer!!\r\n");

		return RET_PARAERR;
	}
	
	if (0 == (width % 32)) 
	{
		cnt = width / 32;
	}
	else 
	{
		cnt = (width  / 32) + 1;
	}
	
#ifndef SV_TEST
	#if 0
	header = 0x100;
	crc_caculate(header,&crc_data);
	header = (((UINT_32)crc_data) << 24) | (1 << 8);
	#endif
	#if 1
	header = (cnt << 16) | (1 << 8) ;
	crc_caculate(header,&crc_data);
	header = (((UINT_32)crc_data) << 24) | (cnt << 16) | (1 << 8);
	#endif
		
#ifdef OS_LINUX
	pthread_mutex_lock(&MUTEX_REG_R_W);
#endif
	
	regp_write_for_ram(chip_id,u32_base_addr,0,header);
	regp_write_for_ram(chip_id,u32_base_addr,1,u32_ram_addr);
	regp_write_for_ram(chip_id,u32_base_addr,3,u32_offset_addr);

	for (index = cnt; index >= 1; index--) 
	{
		if (1 == index) 
		{
			regp_read_for_ram(chip_id, u32_base_addr,0xfd, data);
			break;
		}
		else 
		{
			regp_read_for_ram(chip_id, u32_base_addr,0xfc, (data + (index - 1)));
		}
	}
	
#ifdef OS_LINUX
	pthread_mutex_unlock(&MUTEX_REG_R_W);
#endif
	
	time_delay_ns(100);
	
#else
#ifdef OS_LINUX
	lock_regp();
#else
	
#endif
	soft_ram_read(chip_id,u32_base_addr,u32_ram_addr,u32_offset_addr,data,cnt);
#ifdef OS_LINUX
	unlock_regp();
#else
	
#endif

#endif
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  ram_read_single
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  chip_ram:the ram to write
*	  offset_addr:the offset of the ram
*	  *data:the pointer to the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx      2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS ram_read_single(UINT_8 chip_id,CHIP_RAM chip_ram,UINT_32 offset_addr,UINT_32* data)
{
#ifndef SV_TEST
    unsigned int index = 0;
    unsigned char crc_data = 0;
    unsigned int header = 0;
    UINT_32 payload_len = 0;
#endif

    if (NULL == data)
    {
        printf("invalid parameter pointer!!\r\n");

        return RET_PARAERR;
    }
    
#ifndef SV_TEST
    if (0 == (chip_ram.width % 32)) 
    {
        payload_len = chip_ram.width / 32;
    }
    else 
    {
        payload_len = (chip_ram.width / 32) + 1;
    }
    #if 0
    header = 0x100;
    crc_caculate(header,&crc_data);
    header = (((UINT_32)crc_data) << 24) | (1 << 8);
    #endif
    #if 1
    header = (payload_len << 16) | (1 << 8);
    crc_caculate(header,&crc_data);
    header = (((UINT_32)crc_data) << 24) | (payload_len << 16) | (1 << 8);
    #endif
    
#ifdef OS_LINUX
	pthread_mutex_lock(&MUTEX_REG_R_W);
#endif
	
	regp_write_for_ram(chip_id,chip_ram.base_addr,0,header);
	regp_write_for_ram(chip_id,chip_ram.base_addr,1,chip_ram.ram_addr);
	regp_write_for_ram(chip_id,chip_ram.base_addr,3,offset_addr);
	
	for (index = payload_len; index >= 1; index--) 
	{
		if (1 == index) 
		{
			regp_read_for_ram(chip_id, chip_ram.base_addr,0xfd, data);
			break;
		}
		else 
		{
			regp_read_for_ram(chip_id, chip_ram.base_addr,0xfc, (data + (index - 1)));
		}
	}
		
#ifdef OS_LINUX
	pthread_mutex_unlock(&MUTEX_REG_R_W);
#endif
	time_delay_ns(100);
#else
#ifdef OS_LINUX
	lock_regp();
#else
	
#endif
	soft_ram_read_single(chip_id,chip_ram,offset_addr,data);
#ifdef OS_LINUX
	unlock_regp();
#else
	
#endif

#endif
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  ram_read_print
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base address of the ram
*	  u32_ram_addr:the address of the ram
*	  u32_offset_addr:the offset of the ram
*	  cnt:ram width /32
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS ram_read_print(UINT_8 chip_id,UINT_32 u32_ram_addr,UINT_32 u32_offset_addr,UINT_32 width)
{
	UINT_32 data[128] = {0};
	unsigned int index = 0;
	UINT_8 cnt = 0;
	if (0 == (width % 32)) 
	{
		cnt = width / 32;
	}
	else 
	{
		cnt = (width  / 32) + 1;
	}

	ram_read(chip_id,TABLE_BASE_ADDR,u32_ram_addr,u32_offset_addr,width,data);

	for (index = 0;index < cnt;index++)
	{
		printf("base_addr[0x%x]offset_addr[0x%x]:data[%d]=0x%x\r\n",(TABLE_BASE_ADDR+u32_ram_addr),u32_offset_addr,index,data[index]);
	}
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  ram_read_mul
*
* DESCRIPTION
*
*	  read multi rams
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base address of the register to write
*	  u32_offset_addr:the offset address of the register to write
*	  num:number of register to read
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS ram_read_mul(UINT_8 chip_id,UINT_32 u32_addr,UINT_32 width,UINT_32 num)
{
#ifndef SV_TEST
    unsigned long int i_cnt;
    UINT_8 length = 0;
    UINT_32 u32_data[128] = {0};
	UINT_8 i = 0;
    
    if(num == 0)  
    {
        num = 16;
    }
    
    printf("+-------------------+\n");
    for(i_cnt = 0 ; i_cnt < num ; i_cnt++)
    {    
        ram_read(chip_id,TABLE_BASE_ADDR,u32_addr,i_cnt,width,u32_data);

        if (0 == (width % 32)) 
        {
            length = width / 32;
        }
        else 
        {
            length = (width  / 32) + 1;
        }
        
        for(i = 0; i < length;i++)
        {
            printf("|");
            printf("0x%x:%ld", u32_addr,i_cnt);
            printf("\t");
            printf(" |");
            printf("0x%08x", u32_data[i]);
            printf("|");
            printf("\n");
        }
    }
    printf("+-------------------+\n");
            
#endif
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  regp_mask_write
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  chip_reg:the register to write
*	  u32_data:the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS regp_mask_write(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_8 end_bit,UINT_8 start_bit,UINT_32 u32_data)
{
#ifndef SV_TEST        
    UINT_32 header = 0;
    UINT_8 crc_data = 0;
#endif
    UINT_32 mask = 0xffffffff;
    
    if (end_bit < start_bit)
    {
        return RET_PARAERR;
    }

	if (end_bit - start_bit < 31)
	{
		mask = (~(mask<<(end_bit - start_bit +1)))<<start_bit;
	}
	else
	{
		mask = 0xffffffff;
	}
#ifndef SV_TEST
        
    header = (1 << 16) | (1 << 9) | 0x40 ;
    crc_caculate(header,&crc_data);
    header = (((UINT_32)crc_data) << 24) | (1 << 16) | (1 << 9) | 0x40 ;
    u32_data = u32_data << start_bit;
#ifdef OS_LINUX
	pthread_mutex_lock(&MUTEX_REG_R_W);
#endif
	
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,0,header);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,1,u32_base_addr+u32_offset_addr);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,2,mask);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,0x84,u32_data);

#ifdef OS_LINUX
	pthread_mutex_unlock(&MUTEX_REG_R_W);
#endif
#else

#ifdef OS_LINUX
	lock_regp();
#else
	
#endif
	soft_regp_write_mask(chip_id,u32_base_addr,u32_offset_addr,u32_data,mask);
#ifdef OS_LINUX
	unlock_regp();
#else
	
#endif
	
#endif
	return RET_SUCCESS;
}



/******************************************************************************
*
* FUNCTION
*
*	  regp_mask_write_dword
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  chip_reg:the register to write
*	  u32_data:the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS regp_mask_write_dword(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32 mask,UINT_32 u32_data)
{
#ifndef SV_TEST
	
	UINT_32 header = 0;
	UINT_8 crc_data = 0;
		
	header = (1 << 16) | (1 << 9) | 0x40 ;
	crc_caculate(header,&crc_data);
	header = (((UINT_32)crc_data) << 24) | (1 << 16) | (1 << 9) | 0x40 ;

#ifdef OS_LINUX
	pthread_mutex_lock(&MUTEX_REG_R_W);
#endif

	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,0,header);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,1,u32_base_addr+u32_offset_addr);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,2,mask);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,0x84,u32_data);
	
#ifdef OS_LINUX
	pthread_mutex_unlock(&MUTEX_REG_R_W);
#endif

#else
	soft_regp_write_mask(chip_id,u32_base_addr,u32_offset_addr,u32_data,mask);
#endif
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  regp_field_mask_write
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  chip_reg:the register to write
*	  u32_data:the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS regp_field_mask_write(UINT_8 chip_id,CHIP_REG chip_reg,UINT_32 u32_data)
{
#ifndef SV_TEST        
    UINT_32 header = 0;
    UINT_8 crc_data = 0;
#endif
    UINT_32 mask = 0xffffffff;
    
    if (chip_reg.end_bit < chip_reg.start_bit)
    {
        return RET_PARAERR;
    }

    if (chip_reg.end_bit - chip_reg.start_bit < 31)
    {
        mask = (~(mask<<(chip_reg.end_bit - chip_reg.start_bit +1)))<<chip_reg.start_bit;
    }
    else
    {
        mask = 0xffffffff;
    }

#ifndef SV_TEST    
    header = (1 << 16) | (1 << 9) | 0x40 ;
    crc_caculate(header,&crc_data);
    header = (((UINT_32)crc_data) << 24) | (1 << 16) | (1 << 9) | 0x40 ;
    u32_data = u32_data << chip_reg.start_bit;
#ifdef OS_LINUX
	pthread_mutex_lock(&MUTEX_REG_R_W);
#endif
	
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,0,header);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,1,chip_reg.base_addr+chip_reg.offset_addr);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,2,mask);
	regp_write_for_ram(chip_id,TABLE_BASE_ADDR,0x84,u32_data);
	
#ifdef OS_LINUX
	pthread_mutex_unlock(&MUTEX_REG_R_W);
#endif
#else
#ifdef OS_LINUX
	lock_regp();
#else
	
#endif
	soft_regp_write_mask(chip_id,chip_reg.base_addr,chip_reg.offset_addr,u32_data,mask);
#ifdef OS_LINUX
	unlock_regp();
#else
	
#endif

#endif
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  address_remap
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS address_remap(UINT_8 unit,UINT_8 board_id)
{

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  regp_read_for_ram
*
* DESCRIPTION
*
*	  opuc reorder page cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base address of the register to write
*	  u32_offset_addr:the offset address of the register to write
*	  *u32_data:the pointer to the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS regp_read_for_ram(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32* u32_data)
{
	int irv = 0;
	if (NULL == u32_data)
	{
		printf("invalid parameter pointer!!\r\n");

		return RET_PARAERR;
	}
#ifndef SV_TEST

    irv = flexe_acc_read((int)chip_id, u32_base_addr + u32_offset_addr, (unsigned int *)u32_data, 1);
#else

	soft_regp_read(chip_id,u32_base_addr,u32_offset_addr,u32_data);

#endif
	return irv;
}


/******************************************************************************
*
* FUNCTION
*
*	  regp_write_for_ram
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  u32_base_addr:the base address of the register to write
*	  u32_offset_addr:the offset address of the register to write
*	  u32_data:the data to write
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 mjx       2017-10-10	 1.0		   initial
*
******************************************************************************/
RET_STATUS regp_write_for_ram(UINT_8 chip_id,UINT_32 u32_base_addr,UINT_32 u32_offset_addr,UINT_32 u32_data)
{
	int irv = 0;
#ifndef SV_TEST

	UINT_32 uidata = u32_data;



	irv = flexe_acc_write((int)chip_id, u32_base_addr + u32_offset_addr, (unsigned int *)&uidata, 1);

#else

	soft_regp_write(chip_id,u32_base_addr,u32_offset_addr,u32_data);

#endif
	return irv;
}



RET_STATUS ch_alm_report(UINT_8 chip_id, UINT_8 ch_id, CH_ALM *ch_alm_inst, UINT_8* status)
{
	UINT_32 reg_data[CH_FIELD] = {0};
	UINT_8 i_cnt = 0;

	if(chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		printf("[%s] invalid parameter!!chip_id=%d,phy_id=%d\r\n",__FUNCTION__,chip_id,ch_id);

		return RET_PARAERR;
	}
	
	if ((NULL == ch_alm_inst) || (NULL == status))
	{
		printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);

		return RET_PARAERR;
	}

	/*read reg and update mirror*/
	for (i_cnt = 0; i_cnt < CH_FIELD; i_cnt++)
	{
		regp_read(chip_id, ch_alm_inst->reg_base_addr, ch_alm_inst->reg_start_offset_addr+i_cnt, &reg_data[i_cnt]);
		ch_alm_inst->alm_mirror[i_cnt] = ch_alm_inst->alm_mirror[i_cnt] | reg_data[i_cnt];
	}

	/*report ch alm*/
	*status = ((((reg_data[ch_id / REG_WIDTH]) >> (ch_id % REG_WIDTH)) & MASK_1_BIT) 
	          | (((ch_alm_inst->alm_mirror[ch_id / REG_WIDTH]) >> (ch_id % REG_WIDTH)) & MASK_1_BIT));

	/*clear ch alm state*/
	ch_alm_inst->alm_mirror[ch_id / REG_WIDTH] &= (~((UINT_32)(MASK_1_BIT << (SHIFT_BIT_1*(ch_id % REG_WIDTH)))));

	return RET_SUCCESS;
}
