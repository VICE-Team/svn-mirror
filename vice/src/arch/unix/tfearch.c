/*
 * tfearch.c - TFE ("The final ethernet") emulation, 
 *                 architecture-dependant stuff
 *
 * Written by
 *  Spiro Trikaliotis <trik-news@gmx.de>
 * 
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#ifdef HAVE_TFE 

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "log.h"
#include "tfe.h"
#include "tfearch.h"
/* #include "uitfe.h" */

#include <sys/socket.h>
#include <features.h>    /* for the glibc version number */
#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>     /* the L2 protocols */
#else
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>   /* The L2 protocols */
#endif

#include <netinet/in.h> /* defines htons() */
#include <unistd.h>     /* defined close() */
#include <fcntl.h>

#include <errno.h>     // errno
#include <sys/ioctl.h> // ioctl
#include <net/if.h>    // struct ifreq
#include <linux/if_ether.h>


/** #define TFE_DEBUG_ARCH 1 **/
/** #define TFE_DEBUG_PKTDUMP 1 **/

/* #define TFE_DEBUG_FRAMES - might be defined in TFE.H! */

#define TFE_DEBUG_WARN 1 /* this should not be deactivated */


/* ------------------------------------------------------------------------- */
/*    variables needed                                                       */


static log_t tfe_arch_log = LOG_ERR;

static int packet_socket = 0;  /* file descriptor of our network interface */
static int packet_socket_if = -1; /* number of the network interface */


#ifdef TFE_DEBUG_PKTDUMP

static
void debug_output( const char *text, BYTE *what, int count )
{
    char buffer[256];
    char *p = buffer;
    char *pbuffer1 = what;
    int len1 = count;
    int i;

    sprintf(buffer, "\n%s: length = %u\n", text, len1);
    log_message( tfe_arch_log, buffer );
    do {
        p = buffer;
        for (i=0; (i<8) && len1>0; len1--, i++) {
            sprintf( p, "%02x ", (unsigned int)(unsigned char)*pbuffer1++);
            p += 3;
        }
        *(p-1) = '\n'; *p = 0;
        log_message( tfe_arch_log, buffer );
    } while (len1>0);
}
#endif // #ifdef TFE_DEBUG_PKTDUMP

/*
 These functions let the UI enumerate the available interfaces.

 First, tfe_arch_enumadapter_open() is used to start enumeration.

 tfe_arch_enumadapter is then used to gather information for each adapter present
 on the system, where:

   ppname points to a pointer which will hold the name of the interface
   ppdescription points to a pointer which will hold the description of the interface

   For each of these parameters, new memory is allocated, so it has to be
   freed with lib_free().

 tfe_arch_enumadapter_close() must be used to stop processing.

 Each function returns 1 on success, and 0 on failure.
 tfe_arch_enumadapter() only fails if there is no more adpater; in this case, 
   *ppname and *ppdescription are not altered.
*/

static struct ifconf configdata;
static int socketdesc = -1;
static struct ifreq *pifreq = NULL;

int tfe_arch_enumadapter_open(void)
{
    static char buffer[8192];

    assert(socketdesc == -1);

    if ((socketdesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log_message(tfe_arch_log, "Cannot open socket for enumerating ethernet adapters");
        return 0;
    }

    memset(buffer, 0, sizeof(buffer));
    configdata.ifc_len = sizeof(buffer);
    configdata.ifc_buf = buffer;

    if (ioctl(socketdesc, SIOCGIFCONF, &configdata) < 0) {
        log_message(tfe_arch_log, "Error performing ioctl() for enumerating ethernet adapters");
        close(socketdesc);
        return 0;
    }

    if (configdata.ifc_len >= sizeof(buffer) ) {
        log_message(tfe_arch_log, "configdata to big while enumerating ethernet adapters");
        return 0;
    }

    /* initialize the pointer for later */
    pifreq = (struct ifreq*) buffer;

    /* calculate (and remember) the count of entries in configdata */
    configdata.ifc_len /= sizeof(struct ifreq*);

    return 1;
}

int tfe_arch_enumadapter(char **ppname, char **ppdescription)
{
    assert(pifreq != 0);
    assert(socketdesc >= 0);

    if (configdata.ifc_len == 0)
        return 0;

    for (; configdata.ifc_len!=0; configdata.ifc_len--, pifreq++) {

       /* get the flags for this interface; skip if it's not up */
       if (ioctl(socketdesc, SIOCGIFFLAGS, pifreq) < 0) {
           continue;
       }

       if ((pifreq->ifr_flags & IFF_UP) != 0) {

           /* we found an interface that is up, return that */
           *ppname = lib_stralloc(pifreq->ifr_name);
           *ppdescription = lib_stralloc("");

           /* prepare for the next iteration */
           configdata.ifc_len--;
           pifreq++;
           return 1;
       }
    }

    return 0;
}

int tfe_arch_enumadapter_close(void)
{
    assert(pifreq != 0);
    assert(socketdesc >= 0);

    close(socketdesc);

    socketdesc = -1;
    pifreq = NULL;

    return 1;
}

/* ------------------------------------------------------------------------- */
/*    the architecture-dependend functions                                   */

int tfe_arch_init(void)
{
    tfe_arch_log = log_open("TFEARCH");

/* only for testing! */
#if 1
    if (tfe_arch_enumadapter_open()) {
        int i;
        char *pname;
        char *pdesc;

        do {
            if ((i = tfe_arch_enumadapter(&pname, &pdesc)) != 0) {
                log_message(tfe_arch_log, "Found adapter: '%s', desc: '%s'", pname, pdesc);
                lib_free(pname);
                lib_free(pdesc);
            }
        } while (i != 0);

        tfe_arch_enumadapter_close();
    }
#endif

    return 1;
}

void tfe_arch_pre_reset( void )
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_pre_reset()." );
#endif
}

void tfe_arch_post_reset( void )
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_post_reset()." );
#endif
}


int tfe_arch_activate(const char * const interface_name)
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_activate()." );
#endif

    struct sockaddr_ll bindto;
    int oldfcntl;
    struct ifreq ifreq;
    struct packet_mreq packet_mreq;

    if (packet_socket)
        return 1;

    packet_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (!packet_socket) {
        log_message( tfe_arch_log, "socket() FAILED!" );
        return 0;
    }
    log_message( tfe_arch_log, "socket() succeeded!" );

    /* now, we need the network interface number */
    memset(&ifreq, 0, sizeof(ifreq));
    strncpy(ifreq.ifr_name, interface_name, sizeof(ifreq.ifr_name));

    if (ioctl(packet_socket, SIOCGIFINDEX, &ifreq) == -1) {
        log_message(tfe_arch_log, "could not determine interface number!" );
        close(packet_socket);
        packet_socket = 0;
        return 0;
    }

    packet_socket_if = ifreq.ifr_ifindex;
    log_message(tfe_arch_log, "interface number for %s found to be %u!",
        interface_name, packet_socket_if );

    /* bind to the specific interface */
    memset(&bindto, 0, sizeof(bindto));
    bindto.sll_family = AF_PACKET;
    bindto.sll_ifindex = packet_socket_if;
    bindto.sll_protocol = htons(ETH_P_ALL);

    if (bind(packet_socket, (struct sockaddr *) &bindto, sizeof(bindto)) == -1) {
        log_message(tfe_arch_log, "bind() FAILED!");
        close(packet_socket);
        packet_socket = 0;
        return 0;
    }
    log_message( tfe_arch_log, "bind() succeeded!" );

    oldfcntl = fcntl(packet_socket, F_GETFL, 0);
    if (oldfcntl != -1)
        oldfcntl = fcntl(packet_socket, F_SETFL, oldfcntl|O_NONBLOCK);

    if (oldfcntl == -1) {
        log_message(tfe_arch_log, "fcntl() FAILED!");
        close(packet_socket);
        packet_socket = 0;
        return 0;
    }
    log_message( tfe_arch_log, "fcntl() succeeded!" );

    /* set promiscuous mode */
    memset(&packet_mreq, 0, sizeof(packet_mreq));

    packet_mreq.mr_ifindex = packet_socket_if;
    packet_mreq.mr_type = PACKET_MR_PROMISC;
    if (setsockopt(packet_socket, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &packet_mreq, sizeof(packet_mreq)) < 0) {
        log_message(tfe_arch_log, "enter promiscuous mode: FAILED!");
        close(packet_socket);
        packet_socket = 0;
        return 0;
    }
    log_message( tfe_arch_log, "enter promiscuous mode succeeded!" ); 

    return 1;
}

void tfe_arch_deactivate( void )
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_deactivate()." );
#endif

    if (packet_socket) {
        close(packet_socket);
        packet_socket = 0;
    }
}

void tfe_arch_set_mac( const BYTE mac[6] )
{
#if defined(TFE_DEBUG_ARCH) || defined(TFE_DEBUG_FRAMES)
    log_message( tfe_arch_log, "New MAC address set: %02X:%02X:%02X:%02X:%02X:%02X.",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
#endif
}

void tfe_arch_set_hashfilter(const DWORD hash_mask[2])
{
#if defined(TFE_DEBUG_ARCH) || defined(TFE_DEBUG_FRAMES)
    log_message( tfe_arch_log, "New hash filter set: %08X:%08X.",
        hash_mask[1], hash_mask[0]);
#endif
}

void tfe_arch_recv_ctl( int bBroadcast,   /* broadcast */
                        int bIA,          /* individual address (IA) */
                        int bMulticast,   /* multicast if address passes the hash filter */
                        int bCorrect,     /* accept correct frames */
                        int bPromiscuous, /* promiscuous mode */
                        int bIAHash       /* accept if IA passes the hash filter */
                      )
{
#if defined(TFE_DEBUG_ARCH) || defined(TFE_DEBUG_FRAMES)
    log_message( tfe_arch_log, "tfe_arch_recv_ctl() called with the following parameters:" );
    log_message( tfe_arch_log, "\tbBroadcast   = %s", bBroadcast   ? "TRUE" : "FALSE" );
    log_message( tfe_arch_log, "\tbIA          = %s", bIA          ? "TRUE" : "FALSE" );
    log_message( tfe_arch_log, "\tbMulticast   = %s", bMulticast   ? "TRUE" : "FALSE" );
    log_message( tfe_arch_log, "\tbCorrect     = %s", bCorrect     ? "TRUE" : "FALSE" );
    log_message( tfe_arch_log, "\tbPromiscuous = %s", bPromiscuous ? "TRUE" : "FALSE" );
    log_message( tfe_arch_log, "\tbIAHash      = %s", bIAHash      ? "TRUE" : "FALSE" );
#endif
}

void tfe_arch_line_ctl(int bEnableTransmitter, int bEnableReceiver )
{
#if defined(TFE_DEBUG_ARCH) || defined(TFE_DEBUG_FRAMES)
    log_message( tfe_arch_log, "tfe_arch_line_ctl() called with the following parameters:" );
    log_message( tfe_arch_log, "\tbEnableTransmitter = %s", bEnableTransmitter ? "TRUE" : "FALSE" );
    log_message( tfe_arch_log, "\tbEnableReceiver    = %s", bEnableReceiver    ? "TRUE" : "FALSE" );
#endif
}


void tfe_arch_transmit(int force,       /* FORCE: Delete waiting frames in transmit buffer */
                       int onecoll,     /* ONECOLL: Terminate after just one collision */
                       int inhibit_crc, /* INHIBITCRC: Do not append CRC to the transmission */
                       int tx_pad_dis,  /* TXPADDIS: Disable padding to 60 Bytes */
                       int txlength,    /* Frame length */
                       BYTE *txframe    /* Pointer to the frame to be transmitted */
                      )
{
    int len;

    struct sockaddr_ll addr_to;

#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_transmit() called, with: "
        "force = %s, onecoll = %s, inhibit_crc=%s, tx_pad_dis=%s, txlength=%u",
        force ?       "TRUE" : "FALSE", 
        onecoll ?     "TRUE" : "FALSE", 
        inhibit_crc ? "TRUE" : "FALSE", 
        tx_pad_dis ?  "TRUE" : "FALSE", 
        txlength
        );
#endif

    assert(packet_socket != 0);
    assert(packet_socket_if >= 0);

#ifdef TFE_DEBUG_PKTDUMP
    debug_output( "Transmit frame: ", txframe, txlength);
#endif // #ifdef TFE_DEBUG_PKTDUMP

    /* bind to the specific interface */
    memset(&addr_to, 0, sizeof(addr_to));

    addr_to.sll_family = AF_PACKET;
    addr_to.sll_ifindex = packet_socket_if;
    memcpy(addr_to.sll_addr, txframe, 6);
/* @@@
    addr_to.sll_addr[0] =  txframe[0];
    addr_to.sll_addr[1] =  txframe[1];
    addr_to.sll_addr[2] =  txframe[2];
    addr_to.sll_addr[3] =  txframe[3];
    addr_to.sll_addr[4] =  txframe[4];
    addr_to.sll_addr[5] =  txframe[5];
*/
    addr_to.sll_halen = 6;

    len = sendto(packet_socket, txframe, txlength, 0, (struct sockaddr*) &addr_to, sizeof(struct sockaddr_ll));

    if (len<=0) {
        log_message(tfe_arch_log, "WARNING! Could not send packet, errno=%u (%s)!", errno, strerror(errno));
    }
}

/*
  tfe_arch_receive()

  This function checks if there was a frame received.
  If so, it returns 1, else 0.

  If there was no frame, none of the parameters is changed!

  If there was a frame, the following actions are done:

  - at maximum *plen byte are transferred into the buffer given by pbuffer
  - *plen gets the length of the received frame, EVEN if this is more
    than has been copied to pbuffer!
  - if the dest. address was accepted by the hash filter, *phashed is set, else
    cleared.
  - if the dest. address was accepted by the hash filter, *phash_index is
    set to the number of the rule leading to the acceptance
  - if the receive was ok (good CRC and valid length), *prx_ok is set, 
    else cleared.
  - if the dest. address was accepted because it's exactly our MAC address
    (set by tfe_arch_set_mac()), *pcorrect_mac is set, else cleared.
  - if the dest. address was accepted since it was a broadcast address,
    *pbroadcast is set, else cleared.
  - if the received frame had a crc error, *pcrc_error is set, else cleared
*/
int tfe_arch_receive(BYTE *pbuffer  ,    /* where to store a frame */
                     int  *plen,         /* IN: maximum length of frame to copy; 
                                            OUT: length of received frame 
                                            OUT can be bigger than IN if received frame was
                                                longer than supplied buffer */
                     int  *phashed,      /* set if the dest. address is accepted by the hash filter */
                     int  *phash_index,  /* hash table index if hashed == TRUE */   
                     int  *prx_ok,       /* set if good CRC and valid length */
                     int  *pcorrect_mac, /* set if dest. address is exactly our IA */
                     int  *pbroadcast,   /* set if dest. address is a broadcast address */
                     int  *pcrc_error    /* set if received frame had a CRC error */
                    )
{
    int len;


#ifdef TFE_DEBUG_ARCH
//    log_message( tfe_arch_log, "tfe_arch_receive() called, with *plen=%u.", *plen );
#endif

    assert(packet_socket != 0);
    assert((*plen&1)==0);

    len = recv(packet_socket, pbuffer, *plen, 0);

    if (len<=0) {
        /* nothing has been received */
        return 0;
    }

#ifdef TFE_DEBUG_PKTDUMP
    debug_output( "Received frame: ", pbuffer, len );
#endif // #ifdef TFE_DEBUG_PKTDUMP

    if (len&1)
        ++len;

    *plen = len;

    /* we don't decide if this frame fits the needs;
     * by setting all zero, we let tfe.c do the work
     * for us
     */
    *phashed =
    *phash_index =
    *pbroadcast = 
    *pcorrect_mac =
    *pcrc_error = 0;

    /* this frame has been received correctly */
    *prx_ok = 1;

    return 1;
}

#endif /* #ifdef HAVE_TFE */
