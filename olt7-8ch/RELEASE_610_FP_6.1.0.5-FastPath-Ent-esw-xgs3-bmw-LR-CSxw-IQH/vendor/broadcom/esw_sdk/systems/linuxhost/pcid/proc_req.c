/*
 * $Id: proc_req.c,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * File:        proc_req.c
 * Purpose:
 * Requires:    
 */


#include <unistd.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <soc/mem.h>
#include <soc/hash.h>

#include <soc/cmic.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <sal/appl/io.h>
#include <bde/pli/verinet.h>

#include "pcid.h"
#include "mem.h"
#include "cmicsim.h"
#include "dma.h"
#include "pli.h"

/*
 * Handles an incoming client request
 * We block until the client has something to send us and then 
 * we do something over the socket descriptor.
 * Returns 1 when a command is processed, 0 on disconnect.
 */

int pcid_process_request(pcid_info_t *pcid_info, int sockfd,
                    struct timeval *tmout)
{
    char buf[32];
    uint32 regval;
    rpc_cmd_t cmd;
    int r, unit, finished = 0;

    unit = pcid_info->unit;
    r = get_command(sockfd, tmout, &cmd);

    if (r == 0) {
	return PR_NO_REQUEST;	/* No request; return to poll later */
    }

    if (r < 0) {		/* Error; cause polling loop to exit */
	return PR_ERROR;
    }

    switch (cmd.opcode) {
	/*
	 * We only handle requests: anything else is a programming
	 * error. Once we are here, all parameters have been marshalled
	 * and we simply need to call the correct routine (see pli_cmd.c).
	 */

    case RPC_GETREG_REQ: /* PIO read of register */
	/* Get a register from our model ... */
	/* cmd.args[1] is register type (verinet.h) */

	regval = pli_getreg_service(pcid_info, unit, cmd.args[1], cmd.args[0]);
      
	strcpy(buf, "********");
	make_rpc_getreg_resp(&cmd,
			     0,
			     regval,
			     buf);
	write_command(sockfd, &cmd);
	break;

    case RPC_SETREG_REQ: /* PIO write to register */
	/* Set a register on our model... */
	pli_setreg_service(pcid_info, unit, cmd.args[2], cmd.args[0],
                           cmd.args[1]);
	make_rpc_setreg_resp(&cmd, cmd.args[1]);
	write_command(sockfd, &cmd);
	break;
      
    case RPC_IOCTL_REQ: /* Change something inside the model */
        if (pcid_info->ioctl) {
            pcid_info->ioctl(pcid_info, cmd.args);
        }
	break;
       
    case RPC_SHUTDOWN: /* Change something inside the model */
        if (pcid_info->ioctl) {
            cmd.args[0] = BCM_SIM_DEACTIVATE;
            pcid_info->ioctl(pcid_info, cmd.args);
        }
	break;
       
    case RPC_REGISTER_CLIENT:
	/* Setup port for DMA r/w requests to client and interrupts ...*/
	/* Connect to client interrupt and DMA ports and test*/
        printk("Received register client request\n");
	regval = RPC_OK;

	pcid_info->client = register_client(sockfd,
				 cmd.args[0],
				 cmd.args[1],
				 cmd.args[2]);

	if (pcid_info->client == 0) {
	    finished = 1;
	    break;
	}

	/*
	 * TEST: Write to DMA address 0 on client.
	 */
	regval = dma_writemem(pcid_info->client->dmasock,
			      0,
			      0xbabeface);
      
	/*
	 * TEST: Read from DMA address 0 on client.
	 */
	regval = dma_readmem(pcid_info->client->dmasock,0);
      
	/* TEST: Result should be the same, if not FAIL! */
	if(regval != 0xbabeface){
	    printk("ERROR: regval = 0x%x\n", regval);
	    regval = RPC_FAIL;
	}
	
	/*
	 * TEST: Send an interrupt to the client.
	 */
	send_interrupt(pcid_info->client->intsock, 0);

	regval = RPC_OK;

	/* Send Registration status back ... */
	make_rpc_register_resp(&cmd, regval);
	write_command(pcid_info->client->piosock, &cmd);

	debugk(DK_VERBOSE,
	       "Client registration: 0x%x DMA=0x%x, INT=0x%x, PIO=0x%x -OK\n",
	       pcid_info->client->inetaddr,
	       pcid_info->client->dmasock,
	       pcid_info->client->intsock,
	       pcid_info->client->piosock);

	break;

    case RPC_DISCONNECT:
	/* First disconnect the interrupt and dma sockets on client */

	unregister_client(pcid_info->client);

	finished = 1;
	break;

    default:
	/* Unknown opcode */
	break;
    }

    return (finished ? PR_ALL_DONE : PR_REQUEST_HANDLED);
}
