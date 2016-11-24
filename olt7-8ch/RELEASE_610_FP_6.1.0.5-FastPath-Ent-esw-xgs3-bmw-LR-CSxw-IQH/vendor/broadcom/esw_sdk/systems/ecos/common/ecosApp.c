#include <cyg/kernel/kapi.h>
/* to test exception */
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_arch.h>


#include <cyg/infra/cyg_type.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <cyg/io/pci.h>
#include <cyg/hal/typedefs.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>

#include <network.h>
#include <net/if.h>
#include <netinet/in.h>

cyg_thread thread_s[1];

char stack[1][65536];

cyg_handle_t main_thread;

/* and now variables for the procedure which is the thread */
cyg_thread_entry_t main_program;

/* we install our own startup routine which sets up threads */
void cyg_user_start(void)
{
    /* original 15, lower priority */
    cyg_thread_create(30, main_program, (cyg_addrword_t) 0,
		    "Main App", (void *) stack[0], 32768,
		    &main_thread, &thread_s[0]);

    cyg_thread_resume(main_thread);
}

/* this is a simple program which runs in a thread */
void main_program(cyg_addrword_t data)
{
  int message = (int) data;

  cyg_pci_init();
  
  sal_core_init();
  sal_appl_init();
  
  diag_shell(); 
}

void intf_del_ip_addr(char *if_name)
{
    struct sockaddr_in *addrp;
    struct ifreq ifr;
    int s;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        diag_printf("socket failed\n");
        return; 
    }

    addrp = (struct sockaddr_in *) &ifr.ifr_addr;
    memset(addrp, 0, sizeof(*addrp));
    addrp->sin_family = AF_INET;
    addrp->sin_len = sizeof(*addrp);
    addrp->sin_port = 0;
    addrp->sin_addr.s_addr = INADDR_ANY;

    strcpy(ifr.ifr_name, if_name);
    if (ioctl(s, SIOCDIFADDR, &ifr)) {
        perror("SIOCDIFADDR");
    }
    // Shut down interface so it can be reinitialized
    ifr.ifr_flags &= ~(IFF_UP | IFF_RUNNING);
    if (ioctl(s, SIOCSIFFLAGS, &ifr)) {
        perror("SIOCSIFFLAGS");
        return;
    }
    // All done with socket
    close(s);
}


void ifconfigt(int argc, char *argv[]) 
{
    char if_name[8];
    char addrs_ip[16];
    char addrs_netmask[16];
    char addrs_broadcast[16];
    char addrs_gateway[16];
    char addrs_server[16];
    char bootp_rec[1024];

    --argc;
    ++argv;

    if (!*argv) {
        printf("usage: ifconfig <ifname> <ipaddress> [netmask <mask>] [gw <gateway>]\n");
        return;
    }

    strcpy(if_name, *argv);

    --argc;
    ++argv;

    if (!*argv) {
        printf("usage: ifconfig <ifname> <ipaddress> [netmask <mask>] [gw <gateway>]\n");
        return;
    }

    strcpy(addrs_ip, *argv);

    --argc;
    ++argv;

    if (*argv) {
        if (strncmp(*argv, "netmask", 7)) {
            printf("usage: ifconfig <ifname> <ipaddress> [netmask <mask>] [gw <gateway>]\n");
            return;
        }
		
        --argc;
        ++argv;

        if (!*argv) {
            printf("usage: ifconfig <ifname> <ipaddress> [netmask <mask>] [gw <gateway>]\n");
            return;
        }

        strcpy(addrs_netmask, *argv);

        --argc;
        ++argv;

        if (*argv) {
            if (strncmp(*argv, "gw", 2)) {
                printf("usage: ifconfig <ifname> <ipaddress> [netmask <mask>] [gw <gateway>]\n");
                return;
            }

            --argc;
            ++argv;

            if (!*argv) {
               printf("usage: ifconfig <ifname> <ipaddress> [netmask <mask>] [gw <gateway>]\n");
               return;
            }

            strcpy(addrs_gateway, *argv);
        }
    }

    build_bootp_record(bootp_rec, if_name, addrs_ip, addrs_netmask,
                       addrs_broadcast, addrs_gateway, addrs_server);
    init_net(if_name, bootp_rec);
}
