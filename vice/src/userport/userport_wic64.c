/*
 * userport_wic64.c - Userport WiC64 wifi interface emulation.
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
 *  fixed & complemented by pottendo <pottendo@gmx.net>
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

/* #define DEBUG_WIC64 */

/* - WiC64 (C64/C128)

   C64/C128   |  I/O
   -----------------
   C (PB0)   |  I/O   databits from/to C64
   D (PB1)   |  I/O
   E (PB2)   |  I/O
   F (PB3)   |  I/O
   H (PB4)   |  I/O
   J (PB5)   |  I/O
   K (PB6)   |  I/O
   L (PB7)   |  I/O
   8 (PC2)   |  O     C64 triggers PC2 IRQ whenever data is read or write
   M (PA2)   |  O     Low=device sends data High=C64 sends data (powerup=high)
   B (FLAG2) |  I     device asserts high->low transition when databyte sent to c64 is ready (triggers irq)

   enable the device and start https://www.wic64.de/wp-content/uploads/2021/10/wic64start.zip

   for more info see https://www.wic64.de
*/

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "alarm.h"
#include "cmdline.h"
#include "maincpu.h"
#include "resources.h"
#include "joyport.h"
#include "joystick.h"
#include "snapshot.h"
#include "userport.h"
#include "userport_wic64.h"
#include "machine.h"
#include "uiapi.h"

#ifdef HAVE_LIBCURL

#include "log.h"
#ifdef DEBUG_WIC64
#define DBG(x) log_debug x
#else
#define DBG(x)
#endif

static int userport_wic64_enabled = 0;

/* Some prototypes are needed */
static uint8_t userport_wic64_read_pbx(uint8_t orig);
static void userport_wic64_store_pbx(uint8_t val, int pulse);
static void userport_wic64_store_pa2(uint8_t value);
static int userport_wic64_write_snapshot_module(snapshot_t *s);
static int userport_wic64_read_snapshot_module(snapshot_t *s);
static int userport_wic64_enable(int value);
static void userport_wic64_reset(void);

static userport_device_t userport_wic64_device = {
    "Userport WiC64",                     /* device name */
    JOYSTICK_ADAPTER_ID_NONE,             /* this is NOT a joystick adapter */
    USERPORT_DEVICE_TYPE_WIFI,            /* device is a WIFI adapter */
    userport_wic64_enable,                /* enable function */
    userport_wic64_read_pbx,              /* read pb0-pb7 function */
    userport_wic64_store_pbx,             /* store pb0-pb7 function */
    NULL,                                 /* NO read pa2 pin function */
    userport_wic64_store_pa2,             /* store pa2 pin function */
    NULL,                                 /* NO read pa3 pin function */
    NULL,                                 /* NO store pa3 pin function */
    1,                                    /* pc pin IS needed */
    NULL,                                 /* NO store sp1 pin function */
    NULL,                                 /* NO read sp1 pin function */
    NULL,                                 /* NO store sp2 pin function */
    NULL,                                 /* NO read sp2 pin function */
    userport_wic64_reset,                 /* reset function */
    NULL,                                 /* NO powerup function */
    userport_wic64_write_snapshot_module, /* snapshot write function */
    userport_wic64_read_snapshot_module   /* snapshot read function */
};


static struct alarm_s *http_get_alarm = NULL;
static char sec_token[32];
static int sec_init = 0;
static char session_id[32];
static const char *TOKEN_NAME = "sectokenname";
static void handshake_flag2(void);
static void send_binary_reply(const uint8_t *reply, int len);
static void send_reply(const char *reply);
static void userport_wic64_reset(void);

/* ------------------------------------------------------------------------- */

static int userport_wic64_enable(int value)
{
    int val = value ? 1 : 0;

    if (userport_wic64_enabled == val) {
        return 0;
    }

    userport_wic64_enabled = val;
    return 0;
}

int userport_wic64_resources_init(void)
{
    userport_wic64_reset();
    return userport_device_register(USERPORT_DEVICE_WIC64, &userport_wic64_device);
}

/* ---------------------------------------------------------------------*/

#define HTTPREPLY_MAXLEN    (0x18000)

size_t httpbufferptr = 0;
uint8_t httpbuffer[HTTPREPLY_MAXLEN];

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <curl/curl.h>

#define MAX_PARALLEL 1 /* number of simultaneous transfers */
#define NUM_URLS 10
int still_alive = 0;
CURLM *cm;

/* timezone mapping
   C64 sends just a number 0-31, bcd little endian in commandbuffer.
   offsets can then be calculated.
   TBD, Fixme for day-wraparounds incl. dates
*/
typedef struct tzones
{
    int idx;
    char *tz_name;
    int hour_offs;
    int min_offs;
} tzones_t;

static tzones_t timezones[] = {
    { 0, "Greenwich Mean Time", 0, 0 },
    { 1, "Greenwich Mean Time", 0, 0 },
    { 2, "European Central Time", 1, 0 },
    { 3, "Eastern European Time", 2, 0 },
    { 4, "Arabic Egypt Time", 2, 0 },
    { 5, "Arabic Egypt Time", 2, 0 },
    { 6, "Arabic Egypt Time", 2, 0 },
    { 7, "Near East Time", 4, 0 },
    { 8, "India Standard Time", 4, 30 },
    { 9, "Dont Know Time", 6, 0 },
    { 10, "Dont Know Time", 7, 0 },
    { 11, "Dont Know Time", 8, 0 },
    { 12, "Dont Know Time", 9, 0 },
    { 13, "Japan Standard Time", 9, 0 },
    { 14, "Australia Central Time", 9, 30 },
    { 15, "Australia Central Time", 10, 0 },
    { 16, "Dont Know Tme", 11, 0 },
    { 17, "New Zealand Standart Time", 12, 0 },
    { 18, "Midway Islands Time", -11, 0 },
    { 19, "Hawaii Standard Time", -10, 0 },
    { 20, "Alaska Standard Time", -9, 0 },
    { 21, "Pacific Standard Time", -8, 0 },
    { 22, "Phoenix Standard Time", -7, 0 },
    { 23, "Mountain Standard Time", -7, 0 },
    { 24, "Central Standard Time", -6, 0 },
    { 25, "Eastern Standard Time", -5, 0 },
    { 26, "Indiana Eastern Standard Time", -5, 0 },
    { 27, "Puerto Rico Virg. Island Time", -4, 0 },
    { 28, "Canada Newfoundland Time", -3, -30 },
    { 29, "Dont Know Time", -2, 0 },
    { 30, "Dont Know Time", -1, 0 },
    { 31, "Dont Know Time", 0, 0 },
    { 99, "Juptier Vice Time", -42, 42 },
};
static int current_tz = 2;

static size_t write_cb(char *data, size_t n, size_t l, void *userp)
{
    memcpy(&httpbuffer[httpbufferptr], data, n * l);
    httpbufferptr += (n * l);
    return n*l;
}

static void add_transfer(CURLM *cmulti, char *url)
{
    CURL *eh = curl_easy_init();
    curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(eh, CURLOPT_URL, url);
    curl_easy_setopt(eh, CURLOPT_PRIVATE, url);
    /* set USERAGENT: otherwise the server won't return data, e.g. wicradio */
    curl_easy_setopt(eh, CURLOPT_USERAGENT, "ESP32HTTPClient");
    curl_multi_add_handle(cmulti, eh);
}

static int scan_reply(const uint8_t *buffer, int len)
{
    char *t, *p;
    uint8_t *del;

    if ((t = strstr((const char*)buffer, TOKEN_NAME))) {
        /* DBG(("%s: found sectoken: %s", __FUNCTION__, t)); */
        p = t + strlen(TOKEN_NAME) + 1;
        del = (uint8_t *)strchr(p, 0x1); /* find value 01 */
        if (del) {
            *del = 0; /* terminate string */
            strncpy(sec_token, p, 31);
            DBG(("%s: token = %s", __FUNCTION__, sec_token));
        }
        send_binary_reply(++del, 1); /* move over value 01 */
        sec_init = 1;
        return 0;
    }
    if (sec_init &&
        (t = strstr((const char*)buffer, sec_token))) {
        p = t + strlen(sec_token) + 1;
        del = (uint8_t *)strchr(p, 0x1); /* find value 01 */
        if (del) {
            *del = 0; /* terminate string */
            strncpy(session_id, p, 31);
            DBG(("%s: session id = %s", __FUNCTION__, session_id));
        }
        send_binary_reply(++del, 2); /* move over value 01 */
        return 0;
    }
    return 1; /* send reply */
}

static void http_get_alarm_handler(CLOCK offset, void *data)
{
    CURLMsg *msg;
    CURLcode res;
    int msgs_left = -1;
    curl_multi_perform(cm, &still_alive);
    if (still_alive) {
        if((msg = curl_multi_info_read(cm, &msgs_left))) {
            if(msg->msg == CURLMSG_DONE) {
                char *url;
                CURL *e = msg->easy_handle;
                curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);
                DBG(("%s, R: %u - %s <%s>", __FUNCTION__,
                     msg->data.result, curl_easy_strerror(msg->data.result), url));
                res = curl_easy_perform(msg->easy_handle);
                if(res != CURLE_OK) {
                    DBG(("%s: curl_easy_perform() failed: %s\n", __FUNCTION__,
                         curl_easy_strerror(res)));
                }
                curl_multi_remove_handle(cm, e);
                curl_easy_cleanup(e);
            } else {
                DBG(("%s: CURLMsg (%u)", __FUNCTION__, msg->msg));
            }
        }
    } else {
        curl_multi_cleanup(cm);
        curl_global_cleanup();
        alarm_unset(http_get_alarm);
        if (httpbufferptr > 0) {
            DBG(("%s: got %lu bytes", __FUNCTION__, httpbufferptr));
            if (scan_reply(httpbuffer, httpbufferptr))
                send_binary_reply(httpbuffer, httpbufferptr);
        } else {
            DBG(("%s: received 0 bytes, sending '!0' back.", __FUNCTION__));
            log_message(LOG_DEFAULT, "WiC64: URL returned empty page");
            send_reply("!0");
        }
    }
}

static void do_http_get(const char *prot, char *hostname, char *path, unsigned short server_port)
{

    char thisurl[0x1000];

    strcpy(thisurl, prot);
    strcat(thisurl, hostname);
    strcat(thisurl, "/");
    strcat(thisurl, path);
    DBG(("%s: URL = '%s'", __FUNCTION__, thisurl));

    curl_global_init(CURL_GLOBAL_ALL);
    cm = curl_multi_init();

    /* Limit the amount of simultaneous connections curl should allow: */
    curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, (long)MAX_PARALLEL);

    still_alive = 1;
    httpbufferptr = 0;
    add_transfer(cm, thisurl);

    if (http_get_alarm == NULL) {
        http_get_alarm = alarm_new(maincpu_alarm_context, "HTTPGetAlarm",
                                   http_get_alarm_handler, NULL);
    }
    alarm_unset(http_get_alarm);
    alarm_set(http_get_alarm, maincpu_clk + (312 * 65));
}

/* ---------------------------------------------------------------------*/
#define WLAN_SSID   "VICE WiC64 emulation"
#define WLAN_RSSI   "123"
static unsigned char wic64_mac_address[6] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
static unsigned char wic64_internal_ip[4] = { 192, 168, 188, 5 };
static unsigned char wic64_external_ip[4] = { 204, 234, 1, 4 };
static uint8_t wic64_timezone[2] = { 0, 0};
static uint16_t wic64_udp_port = 0;
static uint16_t wic64_tcp_port = 0;

char default_server_hostname[0x100];

#define COMMANDBUFFER_MAXLEN    0x1000

#define FLAG2_ACTIVE    0
#define FLAG2_INACTIVE  1

uint8_t input_state = 0, input_command = 0;
uint8_t wic64_inputmode = 1;
uint16_t input_length = 0, commandptr = 0;
uint8_t commandbuffer[COMMANDBUFFER_MAXLEN];

char replybuffer[0x10010];
uint16_t replyptr = 0, reply_length = 0;
uint8_t reply_port_value = 0;

static struct alarm_s *flag2_alarm = NULL;

static void flag2_alarm_handler(CLOCK offset, void *data)
{
    set_userport_flag(FLAG2_INACTIVE);
    alarm_unset(flag2_alarm);
}

/* a handshake is triggered after:
   - each byte the esp received from the c64 (inputmode = true)
   - each byte put on the userport for the c64 to fetch (inputmode = false, transferdata = true)
   - a command, to signal the c64 there is a reply
*/
static void handshake_flag2(void)
{
    if (flag2_alarm == NULL) {
        flag2_alarm = alarm_new(maincpu_alarm_context, "FLAG2Alarm", flag2_alarm_handler, NULL);
    }
    alarm_unset(flag2_alarm);
    alarm_set(flag2_alarm, maincpu_clk + 3);

    set_userport_flag(FLAG2_ACTIVE);
    /* set_userport_flag(FLAG2_INACTIVE); */
}

static void reply_next_byte(void)
{
    if (replyptr < reply_length) {
        reply_port_value = replybuffer[replyptr];
        /* DBG(("reply_next_byte: %3d/%3d - %02x'%c'", replyptr, reply_length, reply_port_value, isprint(reply_port_value)?reply_port_value:'.')); */
        replyptr++;
    } else {
        reply_length = 0;
    }
}

static void hexdump(const char *buf, int len)
{
#ifdef DEBUG_WIC64
    int i;
    int idx = 0;;
    while (len > 0) {
        printf("%04x: ", (unsigned) idx);
        for (i = 0; i < 16; i++) {
            if (i < len) {
                printf("%02x ", (uint8_t) buf[idx + i]);
            } else {
                printf("   ");
            }
        }
        printf ("|");
        for (i = 0; i < 16; i++) {
            if (i < len) {
                printf("%c", isprint(buf[idx + i]) ? buf[idx + i] : '.');
            } else {
                printf(" ");
            }
        }
        printf ("|\n");
        idx += 16;
        len -= 16;
    }
#endif
}

static void send_reply(const char * reply)
{
    send_binary_reply((uint8_t *)reply, strlen(reply));
}

static void send_binary_reply(const uint8_t *reply, int len)
{
    /* highbyte first! */
    replybuffer[1] = len & 0xff;
    replybuffer[0] = (len >> 8) & 0xff;
    memcpy((char*)replybuffer + 2, reply, len);
    reply_length = len + 2;
    replyptr = 0;
    DBG(("%s: sending", __FUNCTION__));
    hexdump(replybuffer, reply_length);
    handshake_flag2();
}

/* http get */
static void do_command_01_0f(int encode)
{
    char *p, *cptr;
    int port = 80;
    char hostname[COMMANDBUFFER_MAXLEN];
    char path[COMMANDBUFFER_MAXLEN];
    char temppath[COMMANDBUFFER_MAXLEN];
    char *http_prot = NULL;

    DBG(("%s:", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */

    /* if encode is enabled, there might be binary data after <$, which is
       then encoded as a stream of hex digits */
    if (encode) {
        cptr = (char*)commandbuffer;
        p = strstr(cptr, "<$");
        if (p != NULL) {
            static char hextab[16] = "0123456789abcdef";
            int encodedlen, encodeoffset, i;
            encodeoffset = p - cptr;
            DBG(("%s: escape sequence found, offset %d", __FUNCTION__, encodeoffset));
            /* copy string before <$ */
            strncpy(temppath, cptr, encodeoffset);
            temppath[encodeoffset] = 0;
            /* copy encoded string */
            encodedlen = p[2];
            encodedlen += p[3] << 8;
            p += 4; /* skip escape sequence and len */
            for (i = 0; i < encodedlen; i++) {
                temppath[encodeoffset] = hextab[(*p >> 4) & 0xf];
                encodeoffset++;
                temppath[encodeoffset] = hextab[*p & 0xf];
                encodeoffset++;
                p++;
            }
            temppath[encodeoffset] = 0;
            /* copy string after <$ */
            strcat(temppath, p);
            /* copy back to commandbuffer buffer */
            strcpy((char*)commandbuffer, temppath);
        }
    }

    /* if url begins with !, replace by default server */
    if (commandbuffer[0] == '!') {
        DBG(("URL starts with !, default server is: %s", default_server_hostname));
        p = temppath;
        /* add the default server address */
        strcpy(p, default_server_hostname);
        p += strlen(default_server_hostname);
        /* copy command buffer */
        memcpy(p, commandbuffer + 1, COMMANDBUFFER_MAXLEN - strlen(default_server_hostname));
        /* copy back to commandbuffer buffer */
        memcpy(commandbuffer, temppath, COMMANDBUFFER_MAXLEN);
    }

    /* detect protocol and split path/hostname */
    p = (char*)commandbuffer;
    if (!strncmp(p, "http://", 7)) {
        http_prot = "http://";
    } else {
        if (!strncmp(p, "https://", 8)) {
            http_prot = "https://";
        } else {
            DBG(("malformed URL:%s", commandbuffer));
            return;
        }
    }

    p += strlen(http_prot);
    p = strtok(p, "/");
    strcpy(hostname, p);
    p = (char*)commandbuffer;
    p += (strlen(http_prot) + 1);
    p += strlen(hostname);
    memcpy(path, p, COMMANDBUFFER_MAXLEN - (p - (char*)commandbuffer));

    /* replace "%mac" by our MAC */
    p = strstr(path, "%mac");
    if (p != NULL) {
        char macstring[64]; /* MAC + session_id */
        /* copy string before %mac */
        strncpy(temppath, path, p - path);
        temppath[p - path] = 0;
        /* add the MAC address */
        sprintf(macstring, "%02x%02x%02x%02x%02x%02x%s",
                wic64_mac_address[0], wic64_mac_address[1], wic64_mac_address[2],
                wic64_mac_address[3], wic64_mac_address[4], wic64_mac_address[5],
                session_id);
        strcat(temppath, macstring);
        strcat(temppath, p + 4);
        /* copy back to path buffer */
        strcpy(path, temppath);
    }
    /* replace "%ser" by the default server */
    p = strstr(path, "%ser");
    if (p != NULL) {
        /* copy string before %ser */
        strncpy(temppath, path, p - path);
        temppath[p - path] = 0;
        /* add the default server address */
        strcat(temppath, default_server_hostname);
        /* copy string after %ser */
        strcat(temppath, p + 4);
        /* copy back to path buffer */
        strcpy(path, temppath);
        DBG(("temppath:%s", temppath));
    }

    /* now strip trailing whitspaces of path */
    p = path + strlen(path) - 1;
    while (p && isspace(*p)) {
        *p = '\0';
        p--;
    }
    do_http_get(http_prot, hostname, path, port);
}

/* set wlan ssid + password */
static void do_command_02(void)
{
    DBG(("%s: set wlan ssid + password", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    send_reply("Wlan config changed");
}

/* get wic64 ip address */
static void do_command_06(void)
{
    char buffer[0x20];
    /* FIXME: update the internal IP */
    DBG(("%s: get wic64 IP address - returning dummy address", __FUNCTION__));
    sprintf(buffer, "%d.%d.%d.%d", wic64_internal_ip[0], wic64_internal_ip[1],
            wic64_internal_ip[2], wic64_internal_ip[3]);
    send_reply(buffer);
}

/* get firmware stats */
static void do_command_07(void)
{
    DBG(("%s: get firmware stats", __FUNCTION__));
    send_reply(__DATE__ " " __TIME__);
}

/* set default server */
static void do_command_08(void)
{
    DBG(("%s: set default server", __FUNCTION__));
    strcpy(default_server_hostname, (char*)commandbuffer);
    /* this command sends no reply */
}

/* prints output to serial console */
static void do_command_09(void)
{
    DBG(("%s: output to stdout", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    log_message(LOG_DEFAULT, "WIC64: %s", commandbuffer);
    /* this command sends no reply */
}

/* get udp package */
static void do_command_0a(void)
{
    DBG(("%s: get udp package", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    /* FIXME: not implemented */
    DBG(("get UDP not implemented"));
    send_reply("!0");
}

/* send udp package */
static void do_command_0b(void)
{
    DBG(("%s:", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    /* FIXME: not implemented */
    DBG(("send UDP not implemented"));
    /* this command sends no reply */
}

/* get list of all detected wlan ssids */
static void do_command_0c(void)
{
    /* index, sep, ssid, sep, rssi, sep */
    DBG(("%s: get list of WLAN ssids", __FUNCTION__));
    send_reply("0\001vice\001255\001");
}

/* set wlan via scan id */
static void do_command_0d(void)
{
    DBG(("%s: set WLAN ssid - just return OK.", __FUNCTION__));
    send_reply("Wlan config changed");
}

/* set udp port */
static void do_command_0e(void)
{
    DBG(("%s: set udp port", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */

    wic64_udp_port = commandbuffer[0];
    wic64_udp_port += commandbuffer[1] << 8;
    DBG(("set udp port to %d", wic64_udp_port));
    /* this command sends no reply */
}

/* get connected wlan name */
static void do_command_10(void)
{
    DBG(("%s: get connected WLAN name", __FUNCTION__));
    send_reply(WLAN_SSID);
}

/* get wlan rssi signal level */
static void do_command_11(void)
{
    DBG(("%s: get wlan rssi signal level", __FUNCTION__));
    send_reply(WLAN_RSSI);
}

/* get default server */
static void do_command_12(void)
{
    DBG(("%s: get default server", __FUNCTION__));
    send_reply(default_server_hostname);
}

/* get external ip address */
static void do_command_13(void)
{
    char buffer[0x20];
    /* FIXME: update the external IP */
    DBG(("%s: get external IP address", __FUNCTION__));
    sprintf(buffer, "%d.%d.%d.%d", wic64_external_ip[0], wic64_external_ip[1],
            wic64_external_ip[2], wic64_external_ip[3]);
    send_reply(buffer);
}

/* get wic64 MAC address */
static void do_command_14(void)
{
    char buffer[0x20];
    DBG(("%s: get wic64 MAC address", __FUNCTION__));
    sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
            wic64_mac_address[0], wic64_mac_address[1], wic64_mac_address[2],
            wic64_mac_address[3], wic64_mac_address[4], wic64_mac_address[5]);
    send_reply(buffer);
}

/* get timezone+time */
static void do_command_15(void)
{
    int dst;

    /* FIXME: should also send time+date (in what format?) */
    DBG(("%s: get timezone + time", __FUNCTION__));
    static char timestr[64];
    long t = time(NULL);
    struct tm *tm = localtime(&t);
    dst = tm->tm_isdst; /* this is somehow wrong, get dst vom target tz */
    tm = gmtime(&t); /* now get the UTC */

    snprintf(timestr, 63, "%02d:%02d:%02d %02d-%02d-%04d",
             (tm->tm_hour + timezones[current_tz].hour_offs + ((dst > 0) ? 1 : 0)) % 24, /* Fixme, wrap arounds */
             (tm->tm_min + timezones[current_tz].min_offs % 60),
             tm->tm_sec, tm->tm_mday, tm->tm_mon+1, tm->tm_year + 1900);
    send_reply(timestr);
}

/* set timezone */
static void do_command_16(void)
{
    DBG(("%s: set timezone", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    wic64_timezone[0] = commandbuffer[0];
    wic64_timezone[1] = commandbuffer[1];

    int tzidx = commandbuffer[1] * 10 + commandbuffer[0];
    if (tzidx < sizeof(timezones) / sizeof (tzones_t)) {
        DBG(("setting tz to %s: %dh:%dm", timezones[tzidx].tz_name,
             timezones[tzidx].hour_offs, timezones[tzidx].min_offs));
        current_tz = tzidx;
    } else {
        DBG(("tzidx = %d - out of range", tzidx));
    }
    /* FIXME: send a reply or not? */
    send_reply("Timezone set");
}

/* get tcp */
static void do_command_1e(void)
{
    DBG(("%s: get TCP - not implemented", __FUNCTION__));
    /* FIXME: not implemented */
}

/* send tcp */
static void do_command_1f(void)
{
    DBG(("%s: send TCP - not implemented", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    /* this command sends no reply */
}

/* set tcp port */
static void do_command_20(void)
{
    DBG(("%s:", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    wic64_tcp_port = commandbuffer[0];
    wic64_tcp_port += commandbuffer[1] << 8;
    DBG(("set tcp port to %d", wic64_udp_port));
}

/* factory reset */
static void do_command_63(void)
{
    DBG(("%s:", __FUNCTION__));
    userport_wic64_reset();
    /* this command sends no reply */
}

static void do_command(void)
{
    switch (input_command) {
    case 0x01: /* http get */
        do_command_01_0f(0);
        break;
    case 0x02: /* set wlan ssid + password */
        do_command_02();
        break;
    case 0x03: /* standard firmware update */
        DBG(("command 03: standard firmware update"));
        /* this command sends no reply */
        break;
    case 0x04: /* developer firmware update */
        DBG(("command 04: developer firmware update"));
        /* this command sends no reply */
        send_reply("OK");
        break;
    case 0x05: /* developer special update */
        DBG(("command 05: developer special update"));
        /* this command sends no reply */
        break;
    case 0x06: /* get wic64 ip address */
        do_command_06();
        break;
    case 0x07: /* get firmware stats */
        do_command_07();
        break;
    case 0x08: /* set default server */
        do_command_08();
        break;
    case 0x09: /* prints output to serial console */
        do_command_09();
        break;
    case 0x0a: /* get udp package */
        do_command_0a();
        break;
    case 0x0b: /* send udp package */
        do_command_0b();
        break;
    case 0x0c: /* get list of all detected wlan ssids */
        do_command_0c();
        break;
    case 0x0d: /* set wlan via scan id */
        do_command_0d();
        break;
    case 0x0e: /* set udp port */
        do_command_0e();
        break;
    case 0x0f: /* send http with decoded url for PHP */
        do_command_01_0f(1);
        break;
    case 0x10: /* get connected wlan name */
        do_command_10();
        break;
    case 0x11: /* get wlan rssi signal level */
        do_command_11();
        break;
    case 0x12: /* get default server */
        do_command_12();
        break;
    case 0x13: /* get external ip address */
        do_command_13();
        break;
    case 0x14: /* get wic64 MAC address */
        do_command_14();
        break;
    case 0x15: /* get timezone+time */
        do_command_15();
        break;
    case 0x16: /* set timezones */
        do_command_16();
        break;
    case 0x1e: /* get tcp */
        do_command_1e();
        break;
    case 0x1f: /* send tcp */
        do_command_1f();
        break;
    case 0x20: /* set tcp port */
        do_command_20();
        break;
    case 0x63: /* factory reset */
        do_command_63();
        break;
    default:
        log_error(LOG_DEFAULT, "WiC64: unsupported command 0x%02x (len: %d)", input_command, input_length);
        input_state = 0;
        send_reply("!0");
        break;
    }
}

/* PC2 irq (pulse) triggers when C64 reads/writes to userport */
static void userport_wic64_store_pbx(uint8_t value, int pulse)
{
    /* DBG(("%s: val = %02x pulse = %d", __FUNCTION__, value, pulse)); */
    if (pulse == 1) {
        if (wic64_inputmode) {
            switch (input_state) {
            case 0:
                input_length = 0;
                commandptr = 0;
                if (value == 0x57) {    /* 'w' */
                    input_state++;
                }
                handshake_flag2();
                break;
            case 1: /* lenght low byte */
                input_length = value;
                input_state++;
                handshake_flag2();
                break;
            case 2: /* lenght high byte */
                input_length |= (value << 8);
                input_state++;
                handshake_flag2();
                break;
            case 3: /* command */
                input_command = value;
                input_state++;
                handshake_flag2();
                break;
            default:    /* additional data depending on command */
                if ((commandptr + 4) < input_length) {
                    commandbuffer[commandptr] = value;
                    commandptr++;
                    commandbuffer[commandptr] = 0;
                }
                handshake_flag2();
                break;
            }
#if 0
            DBG(("%s: input_state: %d input_length: %d input_command: %02x commandptr: %02x",
                 __FUNCTION__,
                 input_state, input_length, input_command, commandptr));
#endif
            if ((input_state == 4) && ((commandptr + 4) >= input_length)) {
                do_command();
                commandptr = input_command = input_state = input_length = 0;
            }
        } else {
            if (reply_length) {
                reply_next_byte();
            }
            handshake_flag2();
        }
    }
}

static uint8_t userport_wic64_read_pbx(uint8_t orig)
{
    uint8_t retval = reply_port_value;
    /* FIXME: what do we have to do with original value? */
    /* DBG(("%s: orig = %02x retval = %02x", __FUNCTION__, orig, retval)); */
    /* FIXME: trigger mainloop */
    return retval;
}

/* PA2 interrupt toggles input/output mode */
static void userport_wic64_store_pa2(uint8_t value)
{
    /* DBG(("userport_wic64_store_pa2 val:%02x (c64 %s - rl = %d)", value, value ? "sends" : "receives", reply_length)); */

    if ((wic64_inputmode == 1) && (value == 0) && (reply_length)) {
        DBG(("userport_wic64_store_pa2 val:%02x (c64 %s - rl = %d)", value, value ? "sends" : "receives", reply_length));
        handshake_flag2();
    }
    wic64_inputmode = value;
    /* FIXME: trigger mainloop */
}

static void userport_wic64_reset(void)
{
    /* DBG(("userport_wic64_reset")); */
    commandptr = input_command = input_state = input_length = 0;
    wic64_inputmode = 1;
    memset(session_id, 0, 32);
    memset(sec_token, 0, 32);
    sec_init = 0;
}

/* ---------------------------------------------------------------------*/

/* USERPORT_WIC64 snapshot module format:

   type  | name           | description
   -----------------------------
   BYTE  | input_state    |
   BYTE  | input_length   |
*/

static char snap_module_name[] = "UPWIC64";
#define SNAP_MAJOR   0
#define SNAP_MINOR   1

static int userport_wic64_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_B(m, input_state) < 0)
        || (SMW_B(m, input_command) < 0)) {
        snapshot_module_close(m);
        return -1;
    }
    return snapshot_module_close(m);
}

static int userport_wic64_read_snapshot_module(snapshot_t *s)
{
    uint8_t major_version, minor_version;
    snapshot_module_t *m;

    m = snapshot_module_open(s, snap_module_name, &major_version, &minor_version);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (snapshot_version_is_bigger(major_version, minor_version, SNAP_MAJOR, SNAP_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (0
        || (SMR_B(m, &input_state) < 0)
        || (SMR_B(m, &input_command) < 0)) {
        goto fail;
    }
    return snapshot_module_close(m);

  fail:
    snapshot_module_close(m);
    return -1;
}

#endif /* WIC64 */
