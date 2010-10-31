/*
 * rawnetarch.c - raw ethernet interface,
 *                 architecture-dependant stuff
 *
 * Written by
 *  Thomas Bretz
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

#include <os2.h>
#include <string.h>

#include "ipspy.h"
#include "log.h"
#include "rawnetarch.h"
#include "types.h"

/* ------------------------------------------------------------------------- */
/*    variables needed                                                       */
static log_t rawnet_arch_log = LOG_ERR;

static UCHAR auchInterface[16 + 1] = "";
static USHORT usOldMode = 0;
static ULONG ulHandle = 0;

static const char *IpSpyError(APIRET i)
{
    switch (i) {
        case RC_IPSPY_NOERROR:
            return "No Error.";
        case RC_IPSPY_TCPIP_NOT_FOUND:
            return "TCP/IP not found.";
        case RC_IPSPY_SOCKET_ERROR:
            return "Socket error.";
        case RC_IPSPY_QUEUE_ERROR:
            return "Queue error.";
        case RC_IPSPY_NO_MEMORY:
            return "No memory.";
        case RC_IPSPY_CANNOT_START_THREADS:
            return "Cannot start threads.";
        case RC_IPSPY_NOT_INITIALIZED:
            return "IpSpy not initialized.";
        case RC_IPSPY_BUFFER_TOO_SMALL:
            return "Buffer too small";
        case RC_IPSPY_ALREADY_INITIALIZED:
            return "Already initialized.";
        case RC_IPSPY_INVALID_PARAM:
            return "Invalid param.";
        case RC_IPSPY_MUTEX_ERROR:
            return "Mutex error.";
        case RC_IPSPY_TOO_MANY_HANDLES:
            return "Too many handles.";
        case RC_IPSPY_CANNOT_OPEN_DRIVER:
            return "Cannot open driver 'ipspy.os2'";
        case RC_IPSPY_DRIVER_ERROR:
            return "Driver error.";
        case RC_IPSPY_MODE_NOT_SUPPORTED:
            return "Mode not supported.";
        case RC_IPSPY_NOT_YET_SUPPORTED:
            return "Not yet supported.";
    }
    return "n/a";
}

static void IpSpyInstallDriver(void)
{
    log_debug("-------------------------------------------------------------");
    log_debug(" Before you can use TFE support, you must modify your");
    log_debug(" \\IBMCOM\\PROTOCOL.INI. You must add the following lines at");
    log_debug(" the end of the file:");
    log_debug("    [IPSPY_NIF]");
    log_debug("    drivername = IPSPY$");
    log_debug("    bindings = MYMAC_nif");
    log_debug(" where MYMAC is the section name of the MAC adapter driver");
    log_debug(" you want to use (only one adapter is supported)");
    log_debug("");
    log_debug(" Also add following line at the end of your CONFIG.SYS:");
    log_debug("    DEVICE=[PATH]\\IPSPY.OS2");
    log_debug("");
    log_debug(" After you have made these changes, you must reboot your PC.");
    log_debug("-------------------------------------------------------------");
}

/* ------------------------------------------------------------------------- */
/*    the architecture-dependend functions                                   */
int rawnet_arch_init(void)
{
    APIRET rc;
    UCHAR **pIFs;
    int i;
    UCHAR *pVersion;

    rawnet_arch_log = log_open("ArchTFE");

    // view the version
    rc = IpSpy_Version(&pVersion);
    if (rc != RC_IPSPY_NOERROR) {
        log_debug("IpSpyVersion Error [%d]: %s", rc, IpSpyError(rc));
        return 0;
    }

    log_debug("Found IpSpy Version: %s", pVersion);

    // query all available interfaces
    rc = IpSpy_QueryInterfaces(&pIFs);
    if (rc != RC_IPSPY_NOERROR) {
        log_debug("IpSpyQueryIF Error [%d]: %s", rc, IpSpyError(rc));
        return 0;
    }

    // show all available interfaces
    if (!pIFs) {
        return 0;
    }

    for (i = 0; pIFs[i]; i++) {
        log_debug(" %s", pIFs[i]);
    }

    return 1;
}

void rawnet_arch_pre_reset(void)
{
    log_message(rawnet_arch_log, "rawnet_arch_pre_reset()" );
}

void rawnet_arch_post_reset(void)
{
    log_message(rawnet_arch_log, "rawnet_arch_post_reset()" );
}

int rawnet_arch_activate(const char *interface_name)
{
    APIRET rc;
    UCHAR *pSocketError;
    ULONG ulSocketError;
    const USHORT usMode = DIRECTED_MODE | BROADCAST_MODE | PROMISCUOUS_MODE;

    log_message(rawnet_arch_log, "rawnet_arch_activate()");

    strcpy(auchInterface, "lan0");

    // save receive mode
    rc = IpSpy_QueryReceiveMode(&usOldMode, NULL);
    if (rc != RC_IPSPY_NOERROR) {
        log_debug("IpSpy_QueryReceiveMode Error [%d]: %s", rc, IpSpyError(rc));
        if (rc == RC_IPSPY_CANNOT_OPEN_DRIVER) {
            IpSpyInstallDriver();
        }
    }

    log_debug("IpSpy - Receive Mode %d", usOldMode);

    // we want all packets
    rc = IpSpy_SetReceiveMode(usMode, auchInterface, NULL);
    if (rc != RC_IPSPY_NOERROR) {
        if (rc == RC_IPSPY_MODE_NOT_SUPPORTED) {
            log_debug("Promiscuous mode not supported");
        } else {
            log_debug("IpSpy_SetReceiveMode Error: %d", rc);
            if (rc == RC_IPSPY_CANNOT_OPEN_DRIVER) {
                IpSpyInstallDriver();
            }
        }
        return 0;
    }

    // init monitor
    rc = IpSpy_Init(&ulHandle, auchInterface);
    if (rc != RC_IPSPY_NOERROR) {
        if (rc == RC_IPSPY_SOCKET_ERROR) {
            IpSpy_GetLastSocketError(&ulSocketError, &pSocketError);
            log_debug("IpSpyInit SocketError: [%d] %s", ulSocketError, pSocketError);
        } else {
            log_debug("IpSpyInit Error: %d\n", rc);
        }

        return 0;
    }

    log_message(rawnet_arch_log, "rawnet_arch_activated.");

    return 1;
}

void rawnet_arch_deactivate( void )
{
    APIRET rc;

    log_message(rawnet_arch_log, "rawnet_arch_deactivate().");

    // end monitor
    rc = IpSpy_Exit(ulHandle);
    if (rc != RC_IPSPY_NOERROR) {
        log_debug("IpSpyExit Error: %d", rc);
    }

    // ip stack relexation
    rc = IpSpy_SetReceiveMode(usOldMode, auchInterface, NULL);
    if (rc != RC_IPSPY_NOERROR) {
        log_debug("IpSpy_SetReceiveMode Error: %d", rc);
    }

    log_debug("rawnet_arch_deactivated");
}

void rawnet_arch_set_mac( const BYTE mac[6] )
{
    log_message(rawnet_arch_log, "New MAC address set: %02X:%02X:%02X:%02X:%02X:%02X.", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void rawnet_arch_set_hashfilter(const DWORD hash_mask[2])
{
    log_message(rawnet_arch_log, "New hash filter set: %08X:%08X.", hash_mask[1], hash_mask[0]);
}

/* int bBroadcast   - broadcast */
/* int bIA          - individual address (IA) */
/* int bMulticast   - multicast if address passes the hash filter */
/* int bCorrect     - accept correct frames */
/* int bPromiscuous - promiscuous mode */
/* int bIAHash      - accept if IA passes the hash filter */

void rawnet_arch_recv_ctl(int bBroadcast, int bIA, int bMulticast, int bCorrect, int bPromiscuous, int bIAHash)
{
    log_message(rawnet_arch_log, "rawnet_arch_recv_ctl() called with the following parameters:");
    log_message(rawnet_arch_log, " bBroadcast   = %s", bBroadcast ? "TRUE" : "FALSE");
    log_message(rawnet_arch_log, " bIA          = %s", bIA ? "TRUE" : "FALSE");
    log_message(rawnet_arch_log, " bMulticast   = %s", bMulticast ? "TRUE" : "FALSE");
    log_message(rawnet_arch_log, " bCorrect     = %s", bCorrect ? "TRUE" : "FALSE");
    log_message(rawnet_arch_log, " bPromiscuous = %s", bPromiscuous ? "TRUE" : "FALSE");
    log_message(rawnet_arch_log, " bIAHash      = %s", bIAHash ? "TRUE" : "FALSE");
}

void rawnet_arch_line_ctl(int bEnableTransmitter, int bEnableReceiver )
{
    log_message(rawnet_arch_log, "rawnet_arch_line_ctl() called with the following parameters:");
    log_message(rawnet_arch_log, " bEnableTransmitter = %s", bEnableTransmitter ? "TRUE" : "FALSE");
    log_message(rawnet_arch_log, " bEnableReceiver    = %s", bEnableReceiver ? "TRUE" : "FALSE");
}

/* int force       - FORCE: Delete waiting frames in transmit buffer */
/* int onecoll     - ONECOLL: Terminate after just one collision */
/* int inhibit_crc - INHIBITCRC: Do not append CRC to the transmission */
/* int tx_pad_dis  - TXPADDIS: Disable padding to 60 Bytes */
/* int txlength    - Frame length */
/* BYTE *txframe   - Pointer to the frame to be transmitted */

void rawnet_arch_transmit(int force, int onecoll, int inhibit_crc, int tx_pad_dis, int txlength, BYTE *txframe)
{
    APIRET rc;
    USHORT usType;
    ULONG ulTimeStamp;
    USHORT usUnknown;

    log_message(rawnet_arch_log, "rawnet_arch_transmit() called, with: force = %s, onecoll = %s, inhibit_crc=%s, tx_pad_dis=%s, txlength=%u",
                force ? "TRUE" : "FALSE", onecoll ? "TRUE" : "FALSE", inhibit_crc ? "TRUE" : "FALSE", tx_pad_dis ? "TRUE" : "FALSE", txlength);

    rc = IpSpy_WriteRaw(ulHandle, txframe, txlength, usType, ulTimeStamp, usUnknown);
    log_debug("IpSpy_WriteRaw Error [%d]: %s", rc, IpSpyError(rc));
}

/*
  rawnet_arch_receive()

  This function checks if there was a frame received.
  If so, it returns 1, else 0.

  If there was no frame, none of the parameters is changed!

  If there was a frame, the following actions are done:

  - at maximum *plen byte are transferred into the buffer given by pbuffer
    *plen gets the length of the received frame, EVEN if this is more
    than has been copied to pbuffer!
  - if the dest. address was accepted by the hash filter, *phashed is set, else
    cleared.
  - if the dest. address was accepted by the hash filter, *phash_index is
    set to the number of the rule leading to the acceptance
  - if the receive was ok (good CRC and valid length), *prx_ok is set,
    else cleared.
  - if the dest. address was accepted because it's exactly our MAC address
    (set by rawnet_arch_set_mac()), *pcorrect_mac is set, else cleared.
  - if the dest. address was accepted since it was a broadcast address,
    *pbroadcast is set, else cleared.
  - if the received frame had a crc error, *pcrc_error is set, else cleared
*/

/* BYTE *pbuffer     - where to store a frame */
/* int *plen         - IN: maximum length of frame to copy;
                       OUT: length of received frame
                       OUT can be bigger than IN if received frame was
                           longer than supplied buffer */
/* int *phashed      - set if the dest. address is accepted by the hash filter */
/* int *phash_index  - hash table index if hashed == TRUE */
/* int *prx_ok       - set if good CRC and valid length */
/* int *pcorrect_mac - set if dest. address is exactly our IA */
/* int *pbroadcast   - set if dest. address is a broadcast address */
/* int *pcrc_error   - set if received frame had a CRC error */

int rawnet_arch_receive(BYTE *pbuffer, int *plen, int *phashed, int *phash_index, int *prx_ok, int *pcorrect_mac, int *pbroadcast, int *pcrc_error)
{
    APIRET rc;
    USHORT usType;
    ULONG ulTimeStamp;
    USHORT usUnknown;

    rc = RC_IPSPY_NOERROR;
    *plen = 6;

    if (rc != RC_IPSPY_NOERROR) {
        log_debug("IpSpy_ReadRaw Error [%d]: %s", rc, IpSpyError(rc));
        return 0;
    }

    // length has to be even (see tfe.c)
    if (*plen & 1 == 0) {
        (*plen)++;
    }

    /*
     * we don't decide if this frame fits the needs;
     * by setting all zero, we let tfe.c do the work
     * for us
     */
    *phashed = *phash_index = *pbroadcast = *pcorrect_mac = *pcrc_error = 0;

    /* this frame has been received correctly */
    *prx_ok = 1;

    return 1;
}

#endif /* #ifdef HAVE_TFE */
