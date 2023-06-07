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

#define DEBUG_WIC64

/* - WiC64 (C64/C128)

 PET    VIC20  C64/C128   |  I/O
--------------------------------
                C (PB0)   |  I/O   databits from/to C64
                D (PB1)   |  I/O
                E (PB2)   |  I/O
                F (PB3)   |  I/O
                H (PB4)   |  I/O
                J (PB5)   |  I/O
                K (PB6)   |  I/O
                L (PB7)   |  I/O
READ(1) PA6(2)  8 (PC2)   |  O     C64 triggers PC2 IRQ whenever data is read or write
CB2     CB2     M (PA2)   |  O     Low=device sends data High=C64 sends data (powerup=high)
CA1     CB1     B (FLAG2) |  I     device asserts high->low transition when databyte sent to c64 is ready (triggers irq)

(1) tape #1 read
(2) also connected to tape "sense" (PLAY, F.FWD or REW pressed)

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
#include "lib.h"
#include "util.h"

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

static int wic64_set_default_server(const char *val, void *p);

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
    /* HACK: We put a 0 into the struct here, although pin 8 of the userport
       (which is PC2 on the C64) is actually used. This is needed so the device
       can be registered in xvic (where the pin is driven by PA6). */
    0,                                    /* pc pin IS needed */
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
static struct alarm_s *tcp_get_alarm = NULL;
static struct alarm_s *tcp_send_alarm = NULL;
static char sec_token[32];
static int sec_init = 0;
static char session_id[32];
static const char *TOKEN_NAME = "sectokenname";
static void handshake_flag2(void);
static void send_binary_reply(const uint8_t *reply, size_t len);
static void send_reply(const char *reply);
static void userport_wic64_reset(void);
static char *default_server_hostname = NULL;

static const resource_string_t wic64_resources[] =
{
    { "WIC64DefaultServer", "http://x.wic64.net/", (resource_event_relevant_t)0, NULL,
      &default_server_hostname, wic64_set_default_server, NULL },
    RESOURCE_STRING_LIST_END,
};

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

static int wic64_set_default_server(const char *val, void *v)
{
    util_string_set(&default_server_hostname, val);
    return 0;
}

int userport_wic64_resources_init(void)
{
    userport_wic64_reset();
    if (resources_register_string(wic64_resources) < 0) {
        return -1;
    }
    return userport_device_register(USERPORT_DEVICE_WIC64, &userport_wic64_device);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-wic64server", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "WIC64DefaultServer", NULL,
      "<URL>", "Specify default server URL" },
    CMDLINE_LIST_END
};

int userport_wic64_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ---------------------------------------------------------------------*/

#define HTTPREPLY_MAXLEN    (0x18000)
#define COMMANDBUFFER_MAXLEN    0x1000

static size_t httpbufferptr = 0;
static uint8_t httpbuffer[HTTPREPLY_MAXLEN];
static char encoded_helper[COMMANDBUFFER_MAXLEN];

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifndef WINDOWS_COMPILE
#include <unistd.h>
#endif
#include <curl/curl.h>

#define MAX_PARALLEL 1 /* number of simultaneous transfers */
#define NUM_URLS 10
int still_alive = 0;
CURLM *cm;                      /* used for http(s) */
CURL *curl;                     /* used for telnet */
uint8_t curl_buf[240];          /* this slows down by smaller chunks sent to C64, improves BBSs  */
uint8_t curl_send_buf[COMMANDBUFFER_MAXLEN];
uint16_t curl_send_len;

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
    int dst;                    /* add DST or not, still not perfekt */
} tzones_t;

static tzones_t timezones[] = {
    { 0, "Greenwich Mean Time", 0, 0, 1},
    { 1, "Greenwich Mean Time", 0, 0, 1},
    { 2, "European Central Time", 1, 0, 1 },
    { 3, "Eastern European Time", 2, 0, 1 },
    { 4, "Arabic Egypt Time", 2, 0, 1 },
    { 5, "Arabic Egypt Time", 2, 0, 1 },
    { 6, "Arabic Egypt Time", 2, 0, 1 },
    { 7, "Near East Time", 4, 0, 1 },
    { 8, "India Standard Time", 5, 30, 0 },
    { 9, "Dont Know Time", 6, 0, 0 },
    { 10, "Dont Know Time", 7, 0, 0 },
    { 11, "China Standard Time", 8, 0, 0 },
    { 12, "Korean Standard Time", 9, 0, 0 },
    { 13, "Japan Standard Time", 9, 0, 0},
    { 14, "Australia Central Time", 9, 30, 0 },
    { 15, "Australia Eastern Time", 10, 0, 0 },
    { 16, "Dont Know Tme", 11, 0,0 },
    { 17, "New Zealand Standart Time", 12, 0, 0 },
    { 18, "Midway Islands Time", -11, 0, 0 },
    { 19, "Hawaii Standard Time", -10, 0, 0 },
    { 20, "Alaska Standard Time", -8, 0, 0 },
    { 21, "Pacific Standard Time", -7, 0, 0 },
    { 22, "Phoenix Standard Time", -7, 0, 0 },
    { 23, "Mountain Standard Time", -6, 0, 0 },
    { 24, "Central Standard Time", -5, 0, 0 },
    { 25, "Eastern Standard Time", -4, 0, 0 },
    { 26, "Indiana Eastern Standard Time", -5, 0, 0 },
    { 27, "Puerto Rico Virg. Island Time", -4, 0, 0 },
    { 28, "Canada Newfoundland Time", -2, -30, 0 },
    { 29, "Dont Know Time", -2, 0, 0 },
    { 30, "Dont Know Time", -1, 0, 0 },
    { 31, "Dont Know Time", 0, 0, 0 },
    { 99, "Juptier Vice Time", -42, 42, 1 },
};
static int current_tz = 2;

static void hexdump(const char *buf, int len)
{
#ifdef DEBUG_WIC64
    int i;
    int idx = 0;
    int lines = 0;
    while (len > 0) {
        printf("%04x: ", (unsigned) idx);
        if (lines++ > 7) {      /* just print 8 lines */
            printf("...\n");
            break;
        }
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

static size_t write_cb(char *data, size_t n, size_t l, void *userp)
{
    memcpy(&httpbuffer[httpbufferptr], data, n * l);
    httpbufferptr += (n * l);
    return n*l;
}

static void add_transfer(CURLM *cmulti, char *url)
{
    CURL *eh = curl_easy_init();
#ifdef DEBUG_WIC64
    curl_easy_setopt(eh, CURLOPT_VERBOSE, 1L);
#endif
    curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(eh, CURLOPT_URL, url);
    curl_easy_setopt(eh, CURLOPT_PRIVATE, url);
#if 0
    curl_easy_setopt(eh, CURLOPT_SSL_VERIFYHOST, 0L); /* makes windows happy with https:// URLs */
    curl_easy_setopt(eh, CURLOPT_SSL_VERIFYPEER, 0L); /* but decreases security */
#else
    curl_easy_setopt(eh, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
#endif

/* set USERAGENT: otherwise the server won't return data, e.g. wicradio */
    curl_easy_setopt(eh, CURLOPT_USERAGENT, "ESP32HTTPClient");
    curl_multi_add_handle(cmulti, eh);
}

static void update_prefs(uint8_t *buffer, size_t len)
{
    /* manage preferences in memory only for now */
    DBG(("%s: requested", __FUNCTION__));
    hexdump((char *)buffer, (int)len);
    char *t;
    char *p;
    char *pref = NULL;
    char *val = "";
    char *ret = "";

    if (len > 0) {
        p = (char *)buffer + 1; /* skip \001 */
        t = strchr(p, '\001');
        if ((t != NULL) && ((t - p) < 31)) {
            *t = '\0';
            pref = p;
        }
        p = t + 1; /* skip \0 */
        t = strchr(p, '\001');
        if ((t != NULL) && ((t - p) < 31)) {
            *t = '\0';
            val = p;
        }
        ret = t + 1; /* hope string is terminated */
        log_message(LOG_DEFAULT, "WiC64: user-pref '%s' = '%s', ret = '%s'",
                    pref, val, ret);
    } else {
        return;
    }

    if (sec_init &&
        (strcmp(pref, sec_token) == 0)) {
        strncpy(session_id, val, 31);
        DBG(("%s: session id = %s", __FUNCTION__, session_id));
    }
    if (strcmp(pref, TOKEN_NAME) == 0) {
        strncpy(sec_token, val, 31);
        DBG(("%s: token = %s", __FUNCTION__, sec_token));
        sec_init = 1;
    }
    send_reply(ret);
}

static void http_get_alarm_handler(CLOCK offset, void *data)
{
    CURLMsg *msg;
    CURLMcode r;
    int msgs_left = -1;
    long response = -1;
    char *url = "<unknown>";

    r = curl_multi_perform(cm, &still_alive);
    if (r != CURLM_OK) {
        DBG(("%s: curl_multi_perform failed: %s", __FUNCTION__, curl_multi_strerror(r)));
        msg = curl_multi_info_read(cm, &msgs_left);
        if (msg) {
            DBG(("%s: msg: %u, %s", __FUNCTION__, msg->data.result, curl_easy_strerror(msg->data.result)));
            curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);
            DBG(("%s, R: %u - %s <%s>", __FUNCTION__,
                 msg->data.result, curl_easy_strerror(msg->data.result), url));
        }
        send_reply("!0");       /* maybe wrong here */
        goto out;
    }
    if (still_alive) {
        /* http request not yet finished */
        return;
    }
    msg = curl_multi_info_read(cm, &msgs_left);
    if (msg) {
        CURLcode res;
        res = curl_easy_getinfo(msg->easy_handle,
                                CURLINFO_RESPONSE_CODE,
                                &response);
        if (res != CURLE_OK) {
            DBG(("%s: curl_easy_getinfo(...&response failed: %s", __FUNCTION__,
                 curl_easy_strerror(res)));
            send_reply("!0");   /* maybe wrong here */
            goto out;
        }
        res = curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);
        if (res != CURLE_OK) {
            /* ignore problem, URL is only for debugging */
            DBG(("%s: curl_easy_getinfo(...&URL failed: %s", __FUNCTION__,
                 curl_easy_strerror(res)));
        }
        if (response == 404) {
            log_message(LOG_DEFAULT, "WiC64: URL '%s' not found (http code: %ld)", url, response);
            goto out;
        }
        /* Fixme: check if curl_easy... needs cleanup, or if code below is sufficient */
    }
    if (response == 201) {
        /* prefs update requested, handles replies */
        update_prefs(httpbuffer, httpbufferptr);
        goto out;
    }

    if (response == 200) {
        DBG(("%s: got %lu bytes, URL: '%s', http code = %ld",
             __FUNCTION__, httpbufferptr, url, response));
        send_binary_reply(httpbuffer, httpbufferptr);
    } else {
        DBG(("%s: http code = %ld, sending '!0' back.", __FUNCTION__, response));
        log_message(LOG_DEFAULT, "WiC64: URL '%s' returned %lu bytes (http code: %ld)",
                    url, httpbufferptr, response);
        send_reply("!0");
    }

  out:
    curl_multi_cleanup(cm);
    curl_global_cleanup();
    alarm_unset(http_get_alarm);
    memset(httpbuffer, 0, httpbufferptr);
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
static unsigned char wic64_mac_address[6] = { 0x00, 0xd8, 0x61, 0xdd, 0xee, 0xff }; /* 4 latter bytes will be overwritten by randoms */
static unsigned char wic64_internal_ip[4] = { 192, 168, 42, 42 }; /* just something meaningful */
static unsigned char wic64_external_ip[4] = { 0, 0, 0, 0 }; /* just a dummy, report not implemented to user cmd 0x13 */
static uint8_t wic64_timezone[2] = { 0, 0};
static uint16_t wic64_udp_port = 0;
static uint16_t wic64_tcp_port = 0;

#define FLAG2_ACTIVE    0
#define FLAG2_INACTIVE  1
#define FLAG2_TOGGLE_DELAY 3    /* delay in cycles to toggle flag2 */

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
    alarm_set(flag2_alarm, maincpu_clk + FLAG2_TOGGLE_DELAY);

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

static void send_reply(const char * reply)
{
    send_binary_reply((uint8_t *)reply, strlen(reply));
}

static void send_binary_reply(const uint8_t *reply, size_t len)
{
    /* highbyte first! */
    replybuffer[1] = len & 0xff;
    replybuffer[0] = (len >> 8) & 0xff;
    memcpy((char*)replybuffer + 2, reply, len);
    reply_length = len + 2;
    replyptr = 0;
    if (len > 0) {
        DBG(("%s: sending", __FUNCTION__));
        hexdump(replybuffer, reply_length);
    }
    handshake_flag2();
}

static int _encode(char **p, int len)
{
    int enc_it = 0;
    int i;
    static char hextab[16] = "0123456789abcdef";

    for (i = 0; i < len; i++) {
        encoded_helper[enc_it++] = hextab[((**p) >> 4) & 0xf];
        encoded_helper[enc_it++] = hextab[(**p) & 0xf];
        (*p)++;
    }
    encoded_helper[enc_it] = '\0';
    return enc_it;
}

/* encode binary after escape '$<' */
static void do_command_0f(void)
{
    char *p;
    char *endmarker;
    char *cptr;
    char *tptr;
    char temppath[COMMANDBUFFER_MAXLEN];
    int len;
    int l;

    DBG(("%s: encode URL", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */

    /* if encode is enabled, there might be binary data after <$, which is
       then encoded as a stream of hex digits */
    cptr = (char*)commandbuffer;
    tptr = temppath;
    endmarker = cptr + commandptr;
    len = 0;
    while ((len < COMMANDBUFFER_MAXLEN) &&
           ((p = strstr(cptr, "<$")) != NULL) &&
           (cptr < endmarker)) {
        l = (int)(p - cptr);
        len += l;
        DBG(("%s: escape sequence found, offset %d", __FUNCTION__, len));

        /* copy string before <$ */
        memcpy(tptr, cptr, (size_t)l);
        tptr += l;
        l = p[2];
        l += p[3] << 8;
        p += 4; /* skip escape sequence and len */
        l = _encode(&p, l);
        memcpy(tptr, encoded_helper, l);
        len += l;
        cptr = p;
        tptr += l;
    }
    l = 0;
    if (cptr < endmarker) {
        /* copy remaining commandbuffer */
        l = (int)(endmarker - cptr);
        memcpy(temppath + len, cptr, (size_t)l);
    }
    commandptr = len + l;
    memcpy(commandbuffer, temppath, commandptr);
    commandbuffer[commandptr] = '\0'; /* URL must be a valid string */
}

/* http get */
static void do_command_01(void)
{
    char *p;
    int port = 80;
    char hostname[COMMANDBUFFER_MAXLEN];
    char path[COMMANDBUFFER_MAXLEN];
    char temppath[COMMANDBUFFER_MAXLEN];
    char *http_prot = NULL;
    int i;

    DBG(("%s:", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */

    /* sanity check if URL is OK in principle */
    for (i = 0; i < commandptr; i++) {
        if (!isprint(commandbuffer[i])) {
            log_message(LOG_DEFAULT,
                        "WIC64: bad char '0x%02x' detected in URL at offet %d, %s",
                        commandbuffer[i], i, commandbuffer);
        }
    }

    /* if url begins with !, replace by default server */
    if (commandbuffer[0] == '!') {
        const char *sv;
        resources_get_string("WIC64DefaultServer", &sv);
        DBG(("URL starts with !, default server is: %s", sv));
        p = temppath;
        /* add the default server address */
        strcpy(p, sv);
        p += strlen(sv);
        /* copy command buffer */
        memcpy(p, commandbuffer + 1, COMMANDBUFFER_MAXLEN - strlen(sv));
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
        const char *sv;
        resources_get_string("WIC64DefaultServer", &sv);
        /* copy string before %ser */
        strncpy(temppath, path, p - path);
        temppath[p - path] = 0;
        /* add the default server address */
        strcat(temppath, sv);
        /* copy string after %ser */
        strcat(temppath, p + 4);
        /* copy back to path buffer */
        strcpy(path, temppath);
        DBG(("temppath:%s", temppath));
    }
    /* see below, noprintables need to be overruled, otherwise libcure complains */
    p = strstr(path, "&pid=");
    if (p != NULL) {
        DBG(("%s: patching &pid=XY", __FUNCTION__));
        *(p + 5) = 'X';
        *(p + 6) = 'Y';
    }

    /* now strip trailing whitspaces of path */
    p = path + strlen(path) - 1;
    while (isspace(*p)) {
        *p = '\0';
        p--;
    }
    /* remove trailing nonprintables - otherwise libcurl rejects the URL
       probably a bug on the app side, fixes at least artillery duel */
    while (!isprint(*p)) {
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
    resources_set_string("WIC64DefaultServer", (char *)commandbuffer);
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
    log_message(LOG_DEFAULT, "WIC64: get UDP not implemented, returning empty packet");
    send_reply("");
}

/* send udp package */
static void do_command_0b(void)
{
    DBG(("%s:", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    /* FIXME: not implemented */
    log_message(LOG_DEFAULT, "WIC64: send UDP not implemented");
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
    send_reply("vice: use your host OS, wlan unchanged");
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
    const char *sv;
    resources_get_string("WIC64DefaultServer", &sv);
    DBG(("%s: get default server", __FUNCTION__));
    send_reply(sv);
}

/* get external ip address */
static void do_command_13(void)
{
    char buffer[0x20];
    /* FIXME: update the external IP */
    DBG(("%s: get external IP address", __FUNCTION__));
    log_message(LOG_DEFAULT, "WiC64: command 13, external IP address not implemented, returning 0.0.0.0");
    sprintf(buffer, "%d.%d.%d.%d",
            wic64_external_ip[0], wic64_external_ip[1],
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

    DBG(("%s: get timezone + time", __FUNCTION__));
    static char timestr[64];
    long t = time(NULL);
    struct tm *tm = localtime(&t);
    dst = tm->tm_isdst; /* this is somehow wrong, get dst vom target tz */
    t = t + timezones[current_tz].hour_offs * 3600 +
        ((dst > 0) ? 3600 : 0) * timezones[current_tz].dst + /* some TZs have DST others not */
        timezones[current_tz].min_offs * 60;
    tm = gmtime(&t); /* now get the UTC */
    snprintf(timestr, 63, "%02d:%02d:%02d %02d-%02d-%04d",
             tm->tm_hour, tm->tm_min, tm->tm_sec, tm->tm_mday, tm->tm_mon+1, tm->tm_year + 1900);
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
    /* no reply */
}

/* get timezone */
static void do_command_17(void)
{
    char buf[16];
    DBG(("%s: get timezone", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    snprintf(buf, 16, "%d",
            timezones[current_tz].hour_offs * 3600 +
            timezones[current_tz].min_offs * 60);
    send_reply(buf);
}

/* check update */
static void do_command_18(void)
{
    DBG(("%s: check update", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    send_reply("0");
}

/* read prefs */
static void do_command_19(void)
{
    char buffer[256];
    DBG(("%s: read prefs not implemented", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    snprintf(buffer, 255, "%s", "vice");
    send_reply(buffer);
}

/* wrtie prefs */
static void do_command_1a(void)
{
    DBG(("%s: write prefs not implemented", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    send_reply("");
}

/* get tcp */
static void do_command_1e(void)
{
    DBG(("%s: get TCP - not implemented", __FUNCTION__));
    log_message(LOG_DEFAULT, "WIC64: get TCP not implemented, returning empty packet");
    send_reply("");
    /* FIXME: not implemented */
}

/* send tcp */
static void do_command_1f(void)
{
    DBG(("%s: send TCP - not implemented", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    log_message(LOG_DEFAULT, "WIC64: send TCP not implemented");
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

/* open a curl connection */
static void do_connect(void)
{
    CURLcode res;

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, commandbuffer);
    /* Do not do the transfer - only connect to host */
    curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);
    res = curl_easy_perform(curl);
    DBG(("%s: curl_easy_perform: %s",__FUNCTION__, curl_easy_strerror(res)));
    if (res != CURLE_OK) {
        send_reply("!E");
    } else {
        send_reply("0");
    }
}

static void do_command_21(void)
{
    DBG(("%s: connect TCP", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */

    char tmp[COMMANDBUFFER_MAXLEN];

    strcpy(tmp, "telnet://");
    memcpy(tmp + 9, commandbuffer, commandptr + 1); /* copy '\0' */
    commandptr += 9;
    memcpy(commandbuffer, tmp, commandptr + 1);
    do_connect();
}

static void tcp_get_alarm_handler(CLOCK offset, void *data)
{
    CURLcode res;
    size_t nread;
    static size_t total_read;

    res = curl_easy_recv(curl,
                         curl_buf + total_read,
                         sizeof(curl_buf) - total_read,
                         &nread);
    alarm_set(tcp_get_alarm, maincpu_clk + (312 * 65 * 30));
    total_read += nread;
    if ((res == CURLE_OK) && (nread == 0)) {
        /* connection closed */
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        alarm_unset(tcp_get_alarm);
        curl = NULL;
        DBG(("%s: connection closed", __FUNCTION__));
    }

    if ((res == CURLE_OK) || (res == CURLE_AGAIN)) {
        if (nread) {
            DBG(("%s: nread = %lu, total_read = %lu", __FUNCTION__, nread, total_read));
        }
        send_binary_reply(curl_buf, total_read);
    } else {
        DBG(("%s: curl_easy_recv: %s", __FUNCTION__, curl_easy_strerror(res)));
        send_reply("!E");
    }
    total_read = 0;
}

static void do_command_22(void)
{
    if (commandptr > 0) {
        DBG(("%s: get TCP1", __FUNCTION__));
        hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    }

    if (!curl) {
        DBG(("%s: connection lost", __FUNCTION__));
        send_reply("!E");
        return;
    }

    if (tcp_get_alarm == NULL) {
        tcp_get_alarm = alarm_new(maincpu_alarm_context, "TCPGetAlarm",
                                  tcp_get_alarm_handler, NULL);
    }
    alarm_unset(tcp_get_alarm);
    alarm_set(tcp_get_alarm, maincpu_clk + (312 * 65));
    /* no reply here, but from alarm handler */
}

static void tcp_send_alarm_handler(CLOCK offset, void *data)
{
    CURLcode res;
    size_t nsent;
    static size_t nsent_total;

    alarm_set(tcp_send_alarm, maincpu_clk + (312 * 65));

    nsent = 0;
    res = curl_easy_send(curl, curl_send_buf + nsent_total,
                         curl_send_len - nsent_total, &nsent);
    nsent_total += nsent;

    if (nsent_total < curl_send_len) {
        return;
    }
    nsent_total = 0; /* reset ptr for sending */

    if (res == CURLE_OK) {
        alarm_unset(tcp_send_alarm);
        DBG(("%s: tcp sent successfully", __FUNCTION__));
        send_reply("0");
    } else {
        DBG(("%s: curl_easy_send: %s", __FUNCTION__, curl_easy_strerror(res)));
        send_reply("!E");
    }
}

static void do_command_23(void)
{
    DBG(("%s: send TCP1", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */

    memcpy(curl_send_buf, commandbuffer, commandptr);
    curl_send_len = commandptr;

    if (tcp_send_alarm == NULL) {
        tcp_send_alarm = alarm_new(maincpu_alarm_context, "TCPSendAlarm",
                                  tcp_send_alarm_handler, NULL);
    }
    alarm_unset(tcp_send_alarm);
    alarm_set(tcp_send_alarm, maincpu_clk + (312 * 65));
    /* no reply here, but from alarm handler */
}

static void do_command_24(void)
{
    DBG(("%s: httppost - not implemented", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */

    /* this command sends no reply */
    send_reply("!E");
}

static void do_command_25(void)
{
    DBG(("%s: big loader - not implemented", __FUNCTION__));
    hexdump((const char *)commandbuffer, commandptr); /* commands may contain '0' */
    /* this command sends no reply */
    send_reply("!E");
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
        do_command_01();
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
        do_command_0f();
        do_command_01();
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
    case 0x16: /* set timezone */
        do_command_16();
        break;
    case 0x17: /* get timezone */
        do_command_17();
        break;
    case 0x18: /* check update */
        do_command_18();
        break;
    case 0x19: /* read prefs */
        do_command_19();
        break;
    case 0x1a: /* save prefs */
        do_command_1a();
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
    case 0x21: /* connect TCP */
        do_command_21();
        break;
    case 0x22: /* get tcp1 */
        do_command_22();
        break;
    case 0x23: /* send tcp1  */
        do_command_23();
        break;
    case 0x24: /* httppost  */
        do_command_24();
        break;
    case 0x25: /* loading bigttp  */
        do_command_25();
        break;
    case 0x26: /* getVersion  */
        do_command_07(); /* same as cmd07 here */
        break;
    case 0x63: /* factory reset */
        do_command_63();
        break;
    default:
        log_error(LOG_DEFAULT, "WiC64: unsupported command 0x%02x (len: %d)", input_command, input_length);
        input_state = 0;
        send_reply("!E");
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
                memset(commandbuffer, 0, COMMANDBUFFER_MAXLEN);
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

    if ((wic64_inputmode == 1)
        && (value == 0)
        && (reply_length)) {
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
    wic64_mac_address[2] = lib_unsigned_rand(0, 255);
    wic64_mac_address[3] = lib_unsigned_rand(0, 255);
    wic64_mac_address[4] = lib_unsigned_rand(0, 255);
    wic64_mac_address[5] = lib_unsigned_rand(0, 255);
    wic64_internal_ip[2] = lib_unsigned_rand(1, 254);
    wic64_internal_ip[3] = lib_unsigned_rand(1, 254);
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
