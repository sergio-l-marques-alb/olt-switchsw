file unused, remove it
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rtether.h
*
* @purpose 
*
* @component 
*
* @create 03/22/01
*
* @author alt
* @end
*
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/




struct ether_header {
    unsigned char ether_dhost[6];
    unsigned char ether_shost[6];
    unsigned short ether_type;
};





