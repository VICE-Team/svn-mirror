/*
 * cascom.c - BayCom-compatible TCM3105 cassette-port packet-radio modem.
 *
 * Written by
 *  Felix Erckenbrecht <code@eligs.de>
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

/* Cascom - a "BayCom"-type 1200 baud AFSK packet-radio modem (TCM3105) that
   plugs into the Commodore cassette port (DIGICOM 64 / BayCom style).

   The Commodore software does all of the HDLC/AX.25 bit-banging; the TCM3105
   only modulates/demodulates.  Cassette-port wiring (as used by DIGICOM 64):

     CASS MOTOR  --> TCM3105 TXD   (digital TX data; set_motor callback)
     CASS WRITE  --> PTT trigger   (keys a real radio via a host serial line)
     TCM3105 RXD --> CASS READ     (-> CIA1 FLAG edge; clocks the software DPLL)
                 \-> CASS SENSE     (-> readable level at $01 bit 4; the bit value)

   Modes (CascomMode):
     loopback - reflect TX bits straight back onto the RX lines (self-test)
     audio    - full Bell-202 AFSK modem to/from the host soundcard
     net      - AXUDP/AXIP frame gateway to AX.25-over-IP peers

    The Dicom software can be found here:
    https://www.zimmers.net/anonftp/pub/cbm/c64/packetradio/
*/

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "alarm.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "snapshot.h"
#include "tapeport.h"
#include "util.h"
#include "vsync.h"

#include "cascom.h"
#include "cascom_dsp.h"

#ifdef USE_PORTAUDIO
#include <portaudio.h>
#endif

/* Serial PTT keys a real radio through a host serial control line (RTS/DTR);
   the host specifics live in the archdep_serial_control_* functions. */
#include "archdep_serial_control_open.h"
#include "archdep_serial_control_close.h"
#include "archdep_serial_control_set.h"
#include "archdep_serial_control_list.h"

/* The network gateway uses VICE's socket abstraction (vicesocket.h), so it is
   available on every platform with network support. */
#ifdef HAVE_NETWORK
#define CASCOM_HAVE_NET
#include "vicesocket.h"
#include "cascom_hdlc.h"
#include "cascom_nrzi.h"
#define CASCOM_NET_PLAYOUT_BITS 8192
#endif /* HAVE_NETWORK */

/* ------------------------------------------------------------------------- */
/* device line state (per port; only tape port 1 is used)                    */

static int cascom_enabled[TAPEPORT_MAX_PORTS] = { 0 };
static uint8_t cascom_write_bit[TAPEPORT_MAX_PORTS];  /* last CASS WRITE (PTT) */
static uint8_t cascom_motor[TAPEPORT_MAX_PORTS];      /* last CASS MOTOR (TX data) */
static uint8_t cascom_rx_drv[TAPEPORT_MAX_PORTS];     /* last level driven to C64 */

/* ------------------------------------------------------------------------- */
/* resources                                                                 */

static int cascom_mode = CASCOM_MODE_LOOPBACK;
static int cascom_baud = 1200;

#ifdef USE_PORTAUDIO
static int cascom_tx_rate = 48000;        /* host TX audio stream rate (Hz) */
static int cascom_rx_rate = 48000;        /* host RX audio stream rate (Hz) */
static char *cascom_tx_dev = NULL;        /* PortAudio output device name ("" = default) */
static char *cascom_rx_dev = NULL;        /* PortAudio input device name ("" = default) */
static int cascom_mark_freq = 1200;       /* Bell-202 mark */
static int cascom_space_freq = 2200;      /* Bell-202 space */
static int cascom_tx_gain = 60;           /* output amplitude, percent */
static int cascom_tx_gate = 1;            /* mute TX audio unless PTT is keyed (default on) */
#endif

#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
/* PTT (push-to-talk) - keys a real radio via a host serial control line */
static char *cascom_ptt_device = NULL;    /* host tty path ("" = PTT disabled) */
static int cascom_ptt_line = 0;           /* 0 = RTS, 1 = DTR */
static int cascom_ptt_invert = 0;         /* invert keying polarity */
#endif

#ifdef CASCOM_HAVE_NET
/* network gateway (CascomMode = net) */
static char *cascom_net_peer = NULL;      /* peer host for TX frames ("" = none) */
static int cascom_net_port = 10093;       /* peer UDP port */
static int cascom_net_listen_port = 10093;/* local UDP port for RX */
static char *cascom_net_rxfilter = NULL;  /* optional RX callsign accept-list */
#endif

static log_t cascom_log = LOG_DEFAULT;

/* ------------------------------------------------------------------------- */
/* prototypes                                                                */

static int cascom_enable(int port, int val);
static void cascom_powerup(int port);
static void cascom_shutdown(void);
static void cascom_set_motor(int port, int flag);
static void cascom_toggle_write_bit(int port, int write_bit);
static int cascom_write_snapshot(int port, struct snapshot_s *s, int write_image);
static int cascom_read_snapshot(int port, struct snapshot_s *s);
static void cascom_drive_rx(int port, int level);
static void cascom_audio_start(int port);
static void cascom_audio_stop(void);
static void cascom_audio_reconfigure(void);
static void cascom_audio_request(int port);

static tapeport_device_t cascom_device = {
    "CASCOM packet-radio modem emulation",                      /* device name shown in the tape-port UI (a BayCom-compatible TCM3105 modem) */
    TAPEPORT_DEVICE_TYPE_TAPE,                                  /* 'tape' type device */
    VICE_MACHINE_C64 | VICE_MACHINE_C64SC | VICE_MACHINE_C128,  /* C64/C64SC/C128 only */
    TAPEPORT_PORT_1_MASK,                                       /* tape port 1 only */
    cascom_enable,                                              /* enable function */
    cascom_powerup,                                             /* hard reset / powerup */
    cascom_shutdown,                                            /* shutdown */
    cascom_set_motor,                                           /* set motor line (PTT) */
    cascom_toggle_write_bit,                                    /* set write line (TXD) */
    NULL,                                                       /* NO set sense line */
    NULL,                                                       /* NO set read line */
    cascom_write_snapshot,                                      /* snapshot write */
    cascom_read_snapshot                                        /* snapshot read */
};

/* ------------------------------------------------------------------------- */
/* RX side: drive the demodulated bit onto the C64 READ (FLAG) + SENSE lines.

   READ/FLAG clocks the software DPLL (one edge per transition); SENSE carries
   the bit value at $01 bit 4 (inverted at the port, so we pass !level to give
   an intuitive non-inverted readback). NRZI makes absolute polarity moot. */
static void cascom_drive_rx(int port, int level)
{
    int bit = level ? 1 : 0;

    tapeport_set_tape_sense(bit ? 0 : 1, port);

    if (cascom_rx_drv[port] != bit) {
        tapeport_trigger_flux_change((unsigned int)bit, port);
        cascom_rx_drv[port] = (uint8_t)bit;
    }
}

/* ========================================================================= */
/* M2: AFSK modem to the host soundcard (PortAudio)                          */
/* ========================================================================= */

#ifdef USE_PORTAUDIO

/* cross-thread single-word state (CPU thread <-> PortAudio audio thread) */
static volatile int audio_tx_bit = 1;   /* CPU -> audio: current TXD level (idle = mark) */
static volatile int audio_rx_bit = 1;   /* audio -> CPU: latest demodulated bit */

static int audio_running = 0;
static int audio_port = TAPEPORT_PORT_1;
static int pa_inited = 0;
static PaStream *tx_stream = NULL;
static PaStream *rx_stream = NULL;

static cascom_mod_t tx_mod;             /* used only by the TX audio callback */
static cascom_demod_t rx_demod;         /* used only by the RX audio callback */

/* RX device-rate -> ~9600 Hz internal decimation (box-filter average) */
static int rx_decim = 1;
static int rx_dacc_n = 0;
static int32_t rx_dacc_sum = 0;

/* RX -> C64 bridge alarm, on the emulated CPU timeline (~8x bit rate) */
static alarm_t *rx_alarm = NULL;
static CLOCK rx_alarm_interval = 100;

/* one-shot bootstrap alarm: brings the audio modem online only once the CPU is
   actually executing, so the idle tone never starts during config loading and
   repeated start requests collapse into a single firing */
static alarm_t *boot_alarm = NULL;

/* The TX audio is generated on the EMULATED clock, not sampled inside the async
   PortAudio callback. Sampling a shared bit in the callback quantizes the bit
   timing to the emulator's frame-batch cadence (a burst of CPU cycles, then a
   sync-sleep), so the callback mostly sees the level the bit rests at between
   batches - collapsing the 1200-baud AFSK to (almost) the idle tone. Instead,
   cascom_tx_generate() runs on the CPU thread (from the RX alarm and on every
   MOTOR/TX-data edge) and emits the exact number of modulator samples for the
   emulated time elapsed at the current TXD level, into a ring buffer. The
   PortAudio callback only drains the ring, so every bit keeps its true duration
   regardless of thread scheduling - like a real TCM3105 clocking TXD out. */
#define CASCOM_TX_RING 16384                 /* power of two */
#define CASCOM_TX_RING_MASK (CASCOM_TX_RING - 1)
#define CASCOM_TX_PRIME 1200                 /* ~25 ms @ 48k: absorb one frame batch */
#define CASCOM_TX_GATE_RAMP_MS 4             /* PTT gate fade time (avoids clicks) */
static int16_t tx_ring[CASCOM_TX_RING];
static volatile unsigned tx_rhead = 0;       /* CPU thread writes */
static volatile unsigned tx_rtail = 0;       /* audio thread reads */
static int16_t tx_last_out = 0;
static int tx_primed = 0;
static double tx_cyc_per_sample = 20.0;      /* emulated cycles per audio sample */
static double tx_gen_acc = 0.0;              /* fractional-cycle carry */
static CLOCK tx_gen_clk = 0;                 /* emulated clk at last generation */
/* PTT audio gate: a Q15 amplitude envelope ramped toward its target (1.0 when
   keyed or the gate is off, 0.0 when idle and gated) one step per sample, so
   opening/closing the gate fades in/out instead of clicking. */
static int tx_gate_gain = 0;                 /* current Q15 gain (0..32768) */
static int tx_gate_step = 170;               /* per-sample ramp increment (Q15) */

/* RX ring: the audio thread pushes one demodulated bit per internal (~9600 Hz)
   sample; the CPU-thread RX alarm drains them in order at the same rate on the
   emulated clock. Mirror of the TX ring - without it the alarm re-samples a
   single shared bit at emulated-but-bursty instants, time-warping the demod bit
   stream (clumped at the ~50 Hz frame rate) so DIGICOM's DPLL can't track the
   data-line transitions. */
#define CASCOM_RX_RING 16384
#define CASCOM_RX_RING_MASK (CASCOM_RX_RING - 1)
#define CASCOM_RX_PRIME 256                  /* ~27 ms cushion @ 9600: absorb a frame batch */
static volatile uint8_t rx_ring[CASCOM_RX_RING];
static volatile unsigned rx_rhead = 0;       /* audio thread writes */
static volatile unsigned rx_rtail = 0;       /* CPU thread reads */
static int rx_primed = 0;
static int rx_last_bit = 1;

/* CPU thread: emit modulator samples for the emulated time elapsed since the
   previous call, at the current TXD level, into the ring buffer. */
static void cascom_tx_generate(void)
{
    double elapsed;
    int nsamp;
    int i;
    int bit;
    int target;

    if (!audio_running) {
        return;
    }
    elapsed = (double)(maincpu_clk - tx_gen_clk) + tx_gen_acc;
    nsamp = (int)(elapsed / tx_cyc_per_sample);
    if (nsamp <= 0) {
        return;
    }
    tx_gen_acc = elapsed - (double)nsamp * tx_cyc_per_sample;
    tx_gen_clk = maincpu_clk;

    bit = audio_tx_bit;
    /* optional PTT gate: like a real radio, only put a tone on the air while
       keyed. Rather than hard-switching the samples (which clicks), ramp a Q15
       amplitude envelope toward the target one step per sample; the NCO always
       advances so the tone phase stays continuous across the fade. */
    target = (!cascom_tx_gate || cascom_write_bit[TAPEPORT_PORT_1]) ? 32768 : 0;
    for (i = 0; i < nsamp; i++) {
        unsigned next = (tx_rhead + 1) & CASCOM_TX_RING_MASK;
        int16_t s;
        if (next == tx_rtail) {
            break;                           /* ring full (emulator ran ahead) */
        }
        if (tx_gate_gain < target) {
            tx_gate_gain += tx_gate_step;
            if (tx_gate_gain > target) {
                tx_gate_gain = target;
            }
        } else if (tx_gate_gain > target) {
            tx_gate_gain -= tx_gate_step;
            if (tx_gate_gain < target) {
                tx_gate_gain = target;
            }
        }
        s = cascom_mod_next(&tx_mod, bit);
        tx_ring[tx_rhead] = (int16_t)(((int32_t)s * tx_gate_gain) >> 15);
        tx_rhead = next;
    }
}

/* PortAudio TX callback (paInt16): just drain the ring the CPU thread fills. */
static int cascom_tx_cb(const void *input, void *output, unsigned long frames,
                        const PaStreamCallbackTimeInfo *timeinfo,
                        PaStreamCallbackFlags status, void *userdata)
{
    int16_t *out = (int16_t *)output;
    unsigned long i;

    for (i = 0; i < frames; i++) {
        unsigned avail = (tx_rhead - tx_rtail) & CASCOM_TX_RING_MASK;
        if (!tx_primed) {
            if (avail < CASCOM_TX_PRIME) {
                out[i] = 0;                  /* build a cushion before playing */
                continue;
            }
            tx_primed = 1;
        }
        if (avail > 0) {
            tx_last_out = tx_ring[tx_rtail];
            tx_rtail = (tx_rtail + 1) & CASCOM_TX_RING_MASK;
        }
        out[i] = tx_last_out;                /* underflow: hold last (avoid click) */
    }
    return paContinue;
}

/* RX audio callback: decimate to the internal rate and demodulate; publish the
   latest decided bit for the CPU-side bridge alarm to sample. */
static int cascom_rx_cb(const void *input, void *output, unsigned long frames,
                        const PaStreamCallbackTimeInfo *timeinfo,
                        PaStreamCallbackFlags status, void *userdata)
{
    const int16_t *in = (const int16_t *)input;
    unsigned long i;

    if (in == NULL) {
        return paContinue;
    }
    for (i = 0; i < frames; i++) {
        rx_dacc_sum += in[i];
        if (++rx_dacc_n >= rx_decim) {
            int16_t s = (int16_t)(rx_dacc_sum / rx_dacc_n);   /* box-car average */
            unsigned next;
            rx_dacc_sum = 0;
            rx_dacc_n = 0;
            audio_rx_bit = cascom_demod_next(&rx_demod, s);
            next = (rx_rhead + 1) & CASCOM_RX_RING_MASK;
            if (next != rx_rtail) {          /* push the bit; drop on overflow */
                rx_ring[rx_rhead] = (uint8_t)audio_rx_bit;
                rx_rhead = next;
            }
        }
    }
    return paContinue;
}

/* CPU-thread alarm: drain the next demodulated bit from the RX ring in order on
   the emulated timeline and drive it onto the C64 FLAG/SENSE lines. Draining the
   ring (rather than re-sampling a shared bit) keeps the data-line transitions
   evenly spaced on the emulated clock, which is what DIGICOM's DPLL tracks. */
static void cascom_rx_alarm_cb(CLOCK offset, void *data)
{
    unsigned avail = (rx_rhead - rx_rtail) & CASCOM_RX_RING_MASK;

    cascom_tx_generate();       /* keep the TX audio ring fed on the emulated clock */
    if (!rx_primed) {
        if (avail >= CASCOM_RX_PRIME) {
            rx_primed = 1;      /* build a cushion before we start draining */
        }
    }
    if (rx_primed && avail > 0) {
        rx_last_bit = rx_ring[rx_rtail];
        rx_rtail = (rx_rtail + 1) & CASCOM_RX_RING_MASK;
    }
    cascom_drive_rx(audio_port, rx_last_bit);   /* underflow: hold last bit */
    alarm_set(rx_alarm, maincpu_clk + rx_alarm_interval);
}

static PaDeviceIndex cascom_find_device(const char *name, int want_input)
{
    PaDeviceIndex count;
    PaDeviceIndex i;

    if (name == NULL || name[0] == '\0') {
        return want_input ? Pa_GetDefaultInputDevice() : Pa_GetDefaultOutputDevice();
    }
    count = Pa_GetDeviceCount();
    for (i = 0; i < count; i++) {
        const PaDeviceInfo *di = Pa_GetDeviceInfo(i);
        if (di == NULL) {
            continue;
        }
        if ((want_input ? di->maxInputChannels : di->maxOutputChannels) <= 0) {
            continue;
        }
        if (strstr(di->name, name) != NULL) {
            return i;
        }
    }
    log_warning(cascom_log, "audio %s device '%s' not found, using default.",
                want_input ? "input" : "output", name);
    return want_input ? Pa_GetDefaultInputDevice() : Pa_GetDefaultOutputDevice();
}

static void cascom_audio_start(int port)
{
    PaError err;
    PaStreamParameters op, ip;
    PaDeviceIndex od;
    PaDeviceIndex idv;
    const PaDeviceInfo *odi, *idi;
    double tx_rate = (double)cascom_tx_rate;
    double rx_rate = (double)cascom_rx_rate;
    double demod_rate;
    int win;

    if (audio_running) {
        return;
    }

    /* We are only ever reached from the one-shot bootstrap alarm (or a runtime
       reconfigure), i.e. once the CPU is actually executing - so the idle tone
       never starts during resource/config loading. The context must exist. */
    if (maincpu_alarm_context == NULL) {
        return;
    }

    if (!pa_inited) {
        err = Pa_Initialize();
        if (err != paNoError) {
            log_error(cascom_log, "Pa_Initialize failed: %s", Pa_GetErrorText(err));
            return;
        }
        pa_inited = 1;
    }

    /* internal demod at ~9600 Hz (8 samples/bit): decimate the RX device rate */
    rx_decim = (int)((rx_rate / 9600.0) + 0.5);
    if (rx_decim < 1) {
        rx_decim = 1;
    }
    demod_rate = rx_rate / (double)rx_decim;
    win = (int)((demod_rate / (double)cascom_baud) + 0.5);
    cascom_demod_init(&rx_demod, demod_rate, (double)cascom_mark_freq,
                      (double)cascom_space_freq, win);
    rx_dacc_sum = 0;
    rx_dacc_n = 0;

    cascom_mod_init(&tx_mod, tx_rate, (double)cascom_mark_freq,
                    (double)cascom_space_freq, (double)cascom_tx_gain / 100.0);
    audio_tx_bit = 1;
    audio_rx_bit = 1;

    /* TX generation clock: one audio sample every cyc_per_sample emulated cycles */
    tx_rhead = tx_rtail = 0;
    tx_primed = 0;
    tx_last_out = 0;
    tx_gen_acc = 0.0;
    tx_gen_clk = maincpu_clk;
    tx_cyc_per_sample = (double)machine_get_cycles_per_second() / tx_rate;
    /* start the PTT gate closed so the stream fades up from silence (no click);
       ramp step sized to CASCOM_TX_GATE_RAMP_MS at the current host TX rate. */
    tx_gate_gain = 0;
    tx_gate_step = (int)(32768.0 / ((double)tx_rate * (CASCOM_TX_GATE_RAMP_MS / 1000.0)) + 0.5);
    if (tx_gate_step < 1) {
        tx_gate_step = 1;
    }
    rx_rhead = rx_rtail = 0;
    rx_primed = 0;
    rx_last_bit = 1;

    od = cascom_find_device(cascom_tx_dev, 0);
    idv = cascom_find_device(cascom_rx_dev, 1);

    if (od != paNoDevice) {
        odi = Pa_GetDeviceInfo(od);
        memset(&op, 0, sizeof(op));
        op.device = od;
        op.channelCount = 1;
        op.sampleFormat = paInt16;
        op.suggestedLatency = odi ? odi->defaultLowOutputLatency : 0.05;
        op.hostApiSpecificStreamInfo = NULL;
        err = Pa_OpenStream(&tx_stream, NULL, &op, tx_rate,
                            paFramesPerBufferUnspecified, paClipOff, cascom_tx_cb, NULL);
        if (err != paNoError) {
            log_error(cascom_log, "TX stream open failed: %s", Pa_GetErrorText(err));
            tx_stream = NULL;
        } else {
            Pa_StartStream(tx_stream);
            log_message(cascom_log, "TX audio on '%s'.", odi ? odi->name : "?");
        }
    } else {
        log_warning(cascom_log, "no audio output device for TX.");
    }

    if (idv != paNoDevice) {
        idi = Pa_GetDeviceInfo(idv);
        memset(&ip, 0, sizeof(ip));
        ip.device = idv;
        ip.channelCount = 1;
        ip.sampleFormat = paInt16;
        ip.suggestedLatency = idi ? idi->defaultLowInputLatency : 0.05;
        ip.hostApiSpecificStreamInfo = NULL;
        err = Pa_OpenStream(&rx_stream, &ip, NULL, rx_rate,
                            paFramesPerBufferUnspecified, paClipOff, cascom_rx_cb, NULL);
        if (err != paNoError) {
            log_error(cascom_log, "RX stream open failed: %s", Pa_GetErrorText(err));
            rx_stream = NULL;
        } else {
            Pa_StartStream(rx_stream);
            log_message(cascom_log, "RX audio on '%s'.", idi ? idi->name : "?");
        }
    } else {
        log_warning(cascom_log, "no audio input device for RX.");
    }

    audio_port = port;
    if (rx_alarm == NULL) {
        rx_alarm = alarm_new(maincpu_alarm_context, "CascomRX", cascom_rx_alarm_cb, NULL);
    }
    rx_alarm_interval = (CLOCK)(machine_get_cycles_per_second() / (long)(cascom_baud * 8));
    if (rx_alarm_interval < 1) {
        rx_alarm_interval = 1;
    }
    alarm_set(rx_alarm, maincpu_clk + rx_alarm_interval);

    audio_running = 1;
    log_message(cascom_log,
                "audio modem started: TX %d Hz, RX %d Hz, demod ~%d Hz (decim %d, %d samp/bit), "
                "mark %d / space %d, %d baud.",
                cascom_tx_rate, cascom_rx_rate, (int)demod_rate, rx_decim, win,
                cascom_mark_freq, cascom_space_freq, cascom_baud);

    if (vsync_get_warp_mode()) {
        log_warning(cascom_log, "warp mode is enabled; the modem needs ~100%% speed to work with real audio.");
    }
}

static void cascom_audio_stop(void)
{
    if (rx_alarm != NULL) {
        alarm_unset(rx_alarm);
    }
    if (tx_stream != NULL) {
        Pa_StopStream(tx_stream);
        Pa_CloseStream(tx_stream);
        tx_stream = NULL;
    }
    if (rx_stream != NULL) {
        Pa_StopStream(rx_stream);
        Pa_CloseStream(rx_stream);
        rx_stream = NULL;
    }
    if (audio_running) {
        log_message(cascom_log, "audio modem stopped.");
    }
    audio_running = 0;
}

/* one-shot bootstrap alarm callback: runs once the CPU is executing */
static void cascom_boot_alarm_cb(CLOCK offset, void *data)
{
    if (boot_alarm != NULL) {
        alarm_unset(boot_alarm);
    }
    cascom_audio_start(audio_port);
}

/* Request that the audio modem come online. Rather than opening the streams now
   (which during config load would start the idle tone seconds before the machine
   is up, and churn once per resource setter), schedule a one-shot CPU alarm so
   the streams open only after emulation is actually running. Repeated requests
   simply re-arm the same alarm, so they collapse into a single start. */
static void cascom_audio_request(int port)
{
    if (cascom_mode != CASCOM_MODE_AUDIO || !cascom_enabled[port]) {
        return;
    }
    if (audio_running) {
        return;
    }
    if (maincpu_alarm_context == NULL) {
        return;                  /* too early; cascom_powerup() requests again */
    }
    audio_port = port;
    if (boot_alarm == NULL) {
        boot_alarm = alarm_new(maincpu_alarm_context, "CascomBoot", cascom_boot_alarm_cb, NULL);
    }
    /* fire ~0.1 s into emulation (well after config load and window creation) */
    alarm_set(boot_alarm, maincpu_clk + (CLOCK)(machine_get_cycles_per_second() / 10));
}

/* restart audio if it is running (after a config resource change) */
static void cascom_audio_reconfigure(void)
{
    if (audio_running) {
        int port = audio_port;
        cascom_audio_stop();
        cascom_audio_request(port);
    }
}

static void cascom_audio_terminate(void)
{
    /* Called from the tapeport device shutdown, which runs AFTER the CPU alarm
       context has already been destroyed (machine_maincpu_shutdown). So we must
       NOT touch rx_alarm here (it was freed with the context); just drop the
       dangling pointer and tear down the audio streams. */
    if (tx_stream != NULL) {
        Pa_StopStream(tx_stream);
        Pa_CloseStream(tx_stream);
        tx_stream = NULL;
    }
    if (rx_stream != NULL) {
        Pa_StopStream(rx_stream);
        Pa_CloseStream(rx_stream);
        rx_stream = NULL;
    }
    audio_running = 0;
    rx_alarm = NULL;
    boot_alarm = NULL;
    if (pa_inited) {
        Pa_Terminate();
        pa_inited = 0;
    }
}

#else  /* !USE_PORTAUDIO : audio mode unavailable */

/* Audio mode is compiled out entirely: its resources and command-line options
   are not registered and the CascomMode setter rejects it, so these stubs are
   only reachable from the mode-independent teardown paths. */
static void cascom_audio_stop(void) { }
static void cascom_audio_reconfigure(void) { }
static void cascom_audio_terminate(void) { }
static void cascom_audio_request(int port) { (void)port; }

#endif /* USE_PORTAUDIO */

/* ========================================================================= */
/* M3: PTT (push-to-talk) via a host serial RTS/DTR line                     */
/* ========================================================================= */
/* The keying policy (which line, inverted or not, unkey before release)
   lives here; the host serial specifics live in archdep_serial_control_*. */

#ifdef ARCHDEP_HAVE_SERIAL_CONTROL

static archdep_serial_control_t *ptt_ctl = NULL; /* open host serial port, or NULL */
static int ptt_state = 0;       /* logical PTT (1 = keyed), before invert */

/* apply the logical PTT state to the chosen RTS/DTR line (honouring invert) */
static void cascom_ptt_apply(int on)
{
    int line = cascom_ptt_line ? ARCHDEP_SERIAL_CONTROL_DTR
                               : ARCHDEP_SERIAL_CONTROL_RTS;
    int active = (on ? 1 : 0) ^ (cascom_ptt_invert ? 1 : 0);

    if (ptt_ctl != NULL) {
        archdep_serial_control_set(ptt_ctl, line, active);
    }
}

static void cascom_ptt_open(void)
{
    if (ptt_ctl != NULL) {
        return;
    }
    if (cascom_ptt_device == NULL || cascom_ptt_device[0] == '\0') {
        return;
    }
    ptt_ctl = archdep_serial_control_open(cascom_ptt_device);
    if (ptt_ctl == NULL) {
        log_error(cascom_log, "could not open PTT serial device '%s'.", cascom_ptt_device);
        return;
    }
    log_message(cascom_log, "PTT on %s of '%s'.",
                cascom_ptt_line ? "DTR" : "RTS", cascom_ptt_device);
    cascom_ptt_apply(ptt_state);
}

static void cascom_ptt_close(void)
{
    if (ptt_ctl != NULL) {
        cascom_ptt_apply(0);    /* unkey the radio before releasing the line */
        archdep_serial_control_close(ptt_ctl);
        ptt_ctl = NULL;
    }
}

static void cascom_ptt_set(int on)
{
    ptt_state = on ? 1 : 0;
    cascom_ptt_apply(ptt_state);
}

#else  /* !ARCHDEP_HAVE_SERIAL_CONTROL */

/* Serial modem-control is not available on this platform: the PTT resources,
   command-line options and UI entries are not registered, so these stubs are
   only reachable from the mode-independent enable/teardown paths. */
static void cascom_ptt_open(void) { }
static void cascom_ptt_close(void) { }
static void cascom_ptt_set(int on) { (void)on; }

#endif /* ARCHDEP_HAVE_SERIAL_CONTROL */

/* ========================================================================= */
/* M4: AXUDP network frame gateway                                           */
/* ========================================================================= */
/*
 * net mode bridges the cassette bit stream to AX.25-over-UDP (ax25ipd-style:
 * the raw AX.25 frame, no HDLC flags or FCS, is the UDP payload).
 *
 *   TX  C64 bit-bangs NRZI HDLC on CASS WRITE -> we recover bits from the edge
 *       timing -> deframe to an AX.25 frame -> sendto() the peer.
 *   RX  recvfrom() an AX.25 frame -> (optional callsign filter) -> HDLC-encode
 *       + NRZI -> clock the levels onto the C64 READ/SENSE lines at 1200 baud.
 */

#ifdef CASCOM_HAVE_NET

static int net_running = 0;
static int net_port_idx = TAPEPORT_PORT_1;
static vice_network_socket_t *net_socket = NULL;          /* AXUDP */
static vice_network_socket_address_t *net_peer_addr = NULL;
static double net_cyc_per_bit = 0.0;    /* set from the machine clock at net start */

/* TX: edges -> deframer -> UDP */
static cascom_hdlc_rx_t net_deframer;
static CLOCK net_tx_last_edge = 0;
static int net_tx_have_edge = 0;
static uint8_t net_txframe[CASCOM_HDLC_MAXFRAME];

/* RX: UDP -> HDLC encode + NRZI -> playout buffer (one level per bit) */
static cascom_nrzi_t net_rx_nrzi;
static uint8_t net_playout[CASCOM_NET_PLAYOUT_BITS];
static int net_playout_len = 0;
static int net_playout_pos = 0;
static alarm_t *net_alarm = NULL;
static alarm_t *net_boot_alarm = NULL;

/* ---- TX (C64 -> network) ---- */

static void cascom_net_send(const uint8_t *frame, int len)
{
    if (len <= 0) {
        return;
    }
    if (net_socket != NULL && net_peer_addr != NULL) {
        vice_network_send_to(net_socket, frame, (size_t)len, 0, net_peer_addr);
    }
}

static void cascom_net_feed_tx_bit(int bit)
{
    int r = cascom_hdlc_rx_bit(&net_deframer, bit, net_txframe, (int)sizeof(net_txframe));
    if (r > 0) {
        cascom_net_send(net_txframe, r);
    }
}

/* Called on each TX data-line edge (one NRZI transition). DIGICOM's cassette
   software modem clocks the transmit NRZI HDLC bitstream out on the MOTOR line
   (~857 cyc/bit = 1200 baud), *not* on CASS WRITE (which it uses as the PTT/key
   line). We recover the intervening run of 1-bits from the edge-to-edge gap. */
static void cascom_net_tx_edge(void)
{
    if (net_tx_have_edge) {
        double gap = (double)(maincpu_clk - net_tx_last_edge);
        int ones = cascom_nrzi_gap_cells(gap, net_cyc_per_bit) - 1;
        int k;
        if (ones > 7) {
            ones = 7;                   /* >6 ones = idle/abort; cap the work */
        }
        for (k = 0; k < ones; k++) {
            cascom_net_feed_tx_bit(1);
        }
        cascom_net_feed_tx_bit(0);      /* the transition itself is a data 0 */
    }
    net_tx_have_edge = 1;
    net_tx_last_edge = maincpu_clk;
}

/* ---- RX (network -> C64) ---- */

static void cascom_net_playout_put(void *ctx, int bit)
{
    if (net_playout_len < CASCOM_NET_PLAYOUT_BITS) {
        net_playout[net_playout_len++] = (uint8_t)cascom_nrzi_encode(&net_rx_nrzi, bit);
    }
}

/* optional callsign filter: accept the frame if its AX.25 dest or source call
   matches one of the comma-separated calls in CascomNetRxFilter (case-
   insensitive, SSID ignored). Empty filter accepts everything. */
static int cascom_net_addr_match(const uint8_t *frame, int len)
{
    char calls[2][8];
    const char *p;
    int i;
    int j;

    if (cascom_net_rxfilter == NULL || cascom_net_rxfilter[0] == '\0') {
        return 1;
    }
    if (len < 14) {
        return 0;
    }
    for (i = 0; i < 2; i++) {            /* dest = 0, source = 1 */
        int n = 0;
        for (j = 0; j < 6; j++) {
            char c = (char)(frame[i * 7 + j] >> 1);
            if (c != ' ') {
                calls[i][n++] = c;
            }
        }
        calls[i][n] = '\0';
    }
    for (p = cascom_net_rxfilter; *p; ) {
        char want[8];
        int n = 0;
        while (*p && *p != ',') {
            if (*p != ' ' && n < 7) {
                want[n++] = *p;
            }
            p++;
        }
        want[n] = '\0';
        if (*p == ',') {
            p++;
        }
        if (n > 0) {
            for (i = 0; i < 2; i++) {
                int eq = 1;
                int k;
                for (k = 0; want[k] || calls[i][k]; k++) {
                    int a = (unsigned char)want[k];
                    int b = (unsigned char)calls[i][k];
                    if (a >= 'a' && a <= 'z') {
                        a -= 32;
                    }
                    if (b >= 'a' && b <= 'z') {
                        b -= 32;
                    }
                    if (a != b) {
                        eq = 0;
                        break;
                    }
                }
                if (eq) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

static void cascom_net_load_frame(const uint8_t *frame, int len)
{
    net_playout_len = 0;
    net_playout_pos = 0;
    cascom_nrzi_init(&net_rx_nrzi, 1);
    /* opening flags give the C64 receiver's software DPLL time to lock */
    cascom_hdlc_encode(frame, len, 24, cascom_net_playout_put, NULL);
}

static void cascom_net_poll_recv(void)
{
    uint8_t buf[CASCOM_HDLC_MAXFRAME];
    uint8_t *frame = buf;
    ssize_t n;
    int flen;

    if (net_socket == NULL) {
        return;
    }
    if (vice_network_select_poll_one(net_socket) <= 0) {
        return;                               /* no datagram pending */
    }
    n = vice_network_receive_from(net_socket, buf, sizeof(buf), 0);
    if (n <= 0) {
        return;
    }
    flen = (int)n;
    if (cascom_net_addr_match(frame, flen)) {
        cascom_net_load_frame(frame, flen);
    }
}

/* 1200-baud playout + RX poll, on the emulated CPU timeline */
static void cascom_net_alarm_cb(CLOCK offset, void *data)
{
    if (net_playout_pos >= net_playout_len) {
        cascom_net_poll_recv();         /* idle: fetch the next frame if any */
    }
    if (net_playout_pos < net_playout_len) {
        cascom_drive_rx(net_port_idx, net_playout[net_playout_pos++]);
    }
    alarm_set(net_alarm, maincpu_clk + (CLOCK)net_cyc_per_bit);
}

/* ---- socket + lifecycle ---- */

/* AXUDP via VICE's socket abstraction: bind a UDP socket on the listen port
   and resolve the peer (IPv4, matching the ax25ipd convention). Returns 0 on
   success. */
static int cascom_net_start_axudp(void)
{
    vice_network_socket_address_t *bind_addr;

    bind_addr = vice_network_address_generate("ip4://",
                                              (unsigned short)cascom_net_listen_port);
    if (bind_addr == NULL) {
        log_error(cascom_log, "net: cannot generate bind address.");
        return -1;
    }
    net_socket = vice_network_udp_open(bind_addr);
    vice_network_address_close(bind_addr);
    if (net_socket == NULL) {
        log_error(cascom_log, "net: bind to UDP port %d failed.",
                  cascom_net_listen_port);
        return -1;
    }
    if (cascom_net_peer != NULL && cascom_net_peer[0] != '\0') {
        char *spec = util_concat("ip4://", cascom_net_peer, NULL);
        net_peer_addr = vice_network_address_generate(spec,
                                                      (unsigned short)cascom_net_port);
        lib_free(spec);
        if (net_peer_addr == NULL) {
            log_warning(cascom_log, "net: cannot resolve peer '%s'.", cascom_net_peer);
        }
    }
    return 0;
}

static void cascom_net_start(int port)
{
    if (net_running || maincpu_alarm_context == NULL) {
        return;
    }
    if (cascom_net_start_axudp() < 0) {
        return;
    }

    cascom_hdlc_rx_init(&net_deframer);
    net_tx_have_edge = 0;
    net_playout_len = 0;
    net_playout_pos = 0;
    net_port_idx = port;
    /* Derive the bit period from the emulated machine clock: DIGICOM computes its
       baud timer from the same clock, so this matches its real rate for both TX
       bit recovery and RX playout (PAL 1200 -> 821 cyc, NTSC 1200 -> 852) as long
       as the DIGICOM param's NTSCFL agrees with the emulated machine. */
    net_cyc_per_bit = (double)machine_get_cycles_per_second() / (double)cascom_baud;

    if (net_alarm == NULL) {
        net_alarm = alarm_new(maincpu_alarm_context, "CascomNet", cascom_net_alarm_cb, NULL);
    }
    alarm_set(net_alarm, maincpu_clk + (CLOCK)net_cyc_per_bit);

    net_running = 1;
    log_message(cascom_log, "net (AXUDP) started: listen :%d, peer %s:%d, %d baud.",
                cascom_net_listen_port,
                net_peer_addr != NULL ? cascom_net_peer : "(none)",
                cascom_net_port, cascom_baud);
}

static void cascom_net_stop(void)
{
    if (net_alarm != NULL) {
        alarm_unset(net_alarm);
    }
    if (net_socket != NULL) {
        vice_network_socket_close(net_socket);
        net_socket = NULL;
    }
    if (net_peer_addr != NULL) {
        vice_network_address_close(net_peer_addr);
        net_peer_addr = NULL;
    }
    if (net_running) {
        log_message(cascom_log, "net stopped.");
    }
    net_running = 0;
}

static void cascom_net_boot_alarm_cb(CLOCK offset, void *data)
{
    if (net_boot_alarm != NULL) {
        alarm_unset(net_boot_alarm);
    }
    cascom_net_start(net_port_idx);
}

/* like the audio backend, defer the actual start until the CPU is executing */
static void cascom_net_request(int port)
{
    if (cascom_mode != CASCOM_MODE_NET || !cascom_enabled[port]) {
        return;
    }
    if (net_running || maincpu_alarm_context == NULL) {
        return;
    }
    net_port_idx = port;
    if (net_boot_alarm == NULL) {
        net_boot_alarm = alarm_new(maincpu_alarm_context, "CascomNetBoot",
                                   cascom_net_boot_alarm_cb, NULL);
    }
    alarm_set(net_boot_alarm, maincpu_clk + (CLOCK)(machine_get_cycles_per_second() / 10));
}

static void cascom_net_reconfigure(void)
{
    if (net_running) {
        int port = net_port_idx;
        cascom_net_stop();
        cascom_net_request(port);
    }
}

static void cascom_net_terminate(void)
{
    /* device shutdown runs after the CPU alarm context is gone: just drop the
       dangling alarm pointers and close the socket(s). */
    if (net_socket != NULL) {
        vice_network_socket_close(net_socket);
        net_socket = NULL;
    }
    if (net_peer_addr != NULL) {
        vice_network_address_close(net_peer_addr);
        net_peer_addr = NULL;
    }
    net_running = 0;
    net_alarm = NULL;
    net_boot_alarm = NULL;
}

#else  /* !CASCOM_HAVE_NET */

/* Network mode is compiled out entirely: its resources and command-line
   options are not registered and the CascomMode setter rejects it, so these
   stubs are only reachable from the mode-independent teardown paths. */
static void cascom_net_request(int port) { (void)port; }
static void cascom_net_stop(void) { }
static void cascom_net_reconfigure(void) { }
static void cascom_net_terminate(void) { }

#endif /* CASCOM_HAVE_NET */

/* recompute the PTT/keying signal and drive the serial line. CASS WRITE is the
   DIGICOM key line (the TX data itself is on the MOTOR line). PTT keys the radio
   and, when CascomTxGate is on (default), also gates the TX audio so tones are
   only emitted while keyed (see cascom_tx_generate). */
static void cascom_update_ptt(int port)
{
    cascom_ptt_set(cascom_write_bit[port]);
}

/* ------------------------------------------------------------------------- */
/* tapeport callbacks                                                        */

static int cascom_enable(int port, int value)
{
    int val = value ? 1 : 0;

    if (cascom_enabled[port] == val) {
        return 0;
    }

    if (val) {
        cascom_write_bit[port] = 0;
        cascom_motor[port] = 0;
        cascom_rx_drv[port] = 0;
        tapeport_set_tape_sense(0, port);
        log_message(cascom_log, "enabled on tape port %d (mode %d).", port + 1, cascom_mode);
        cascom_ptt_open();
        if (cascom_mode == CASCOM_MODE_AUDIO) {
            cascom_audio_request(port);
        } else if (cascom_mode == CASCOM_MODE_NET) {
            cascom_net_request(port);
        }
    } else {
        cascom_audio_stop();
        cascom_net_stop();
        cascom_ptt_close();
        tapeport_set_tape_sense(0, port);
        log_message(cascom_log, "disabled on tape port %d.", port + 1);
    }

    cascom_enabled[port] = val;
    return 0;
}

static void cascom_powerup(int port)
{
    cascom_write_bit[port] = 0;
    cascom_motor[port] = 0;
    cascom_rx_drv[port] = 0;
    tapeport_set_tape_sense(0, port);

    /* Request the active backend; the bootstrap alarm brings it online once the
       CPU is actually executing (not during config load). */
    if (cascom_enabled[port]) {
        if (cascom_mode == CASCOM_MODE_AUDIO) {
            cascom_audio_request(port);
        } else if (cascom_mode == CASCOM_MODE_NET) {
            cascom_net_request(port);
        }
    }
}

static void cascom_shutdown(void)
{
    cascom_audio_terminate();
    cascom_net_terminate();
    cascom_ptt_close();
}

static void cascom_set_motor(int port, int flag)
{
    int m = flag ? 1 : 0;

    /* In the real DIGICOM cassette wiring the MOTOR line carries the C64->modem
       TX *data* (NRZI HDLC bitstream), not a motor/PTT control. PTT is on the
       CASS WRITE line (see cascom_toggle_write_bit / cascom_update_ptt). */
#ifdef CASCOM_HAVE_NET
    if (cascom_mode == CASCOM_MODE_NET && net_running && m != cascom_motor[port]) {
        cascom_net_tx_edge();       /* MOTOR-line edge = one NRZI transition */
    }
#endif
#ifdef USE_PORTAUDIO
    if (cascom_mode == CASCOM_MODE_AUDIO) {
        cascom_tx_generate();       /* flush samples at the old level up to this edge */
        audio_tx_bit = m;           /* then switch: MOTOR level is the TX data bit */
    }
#endif
    cascom_motor[port] = (uint8_t)m;
}

static void cascom_toggle_write_bit(int port, int write_bit)
{
    int bit = write_bit ? 1 : 0;

    if (bit == cascom_write_bit[port]) {
        return;
    }
    cascom_write_bit[port] = (uint8_t)bit;

    switch (cascom_mode) {
        case CASCOM_MODE_LOOPBACK:
            cascom_drive_rx(port, bit);
            break;
        case CASCOM_MODE_AUDIO:
            /* CASS WRITE is the PTT/key line; the TX data bit is taken from the
               MOTOR line (see cascom_set_motor). Keying is handled below. */
            break;
        case CASCOM_MODE_NET:
#ifdef CASCOM_HAVE_NET
            /* CASS WRITE is DIGICOM's PTT/key line (not the data line). Each edge
               is a key-up/key-down = a transmit-burst boundary; drop the stale
               inter-burst gap so the next burst's bit recovery starts clean. */
            if (net_running) {
                net_tx_have_edge = 0;
            }
#endif
            break;
        default:
            break;
    }

    /* CASS WRITE is the PTT/key line in the DIGICOM cassette wiring: drive the
       serial PTT + TX audio gate from it (loopback has no PTT effect). */
    cascom_update_ptt(port);
}

/* ------------------------------------------------------------------------- */
/* resources                                                                 */

static int set_cascom_mode(int val, void *param)
{
    int newmode;

    if (val < CASCOM_MODE_MIN || val > CASCOM_MODE_MAX) {
        return -1;
    }
    /* modes whose backend is not compiled in are rejected outright; their
       resources, command-line options and UI entries are not registered
       either, so the mode is completely invisible in such builds */
#ifndef USE_PORTAUDIO
    if (val == CASCOM_MODE_AUDIO) {
        return -1;
    }
#endif
#ifndef CASCOM_HAVE_NET
    if (val == CASCOM_MODE_NET) {
        return -1;
    }
#endif
    newmode = val;
    if (newmode == cascom_mode) {
        return 0;
    }

    /* if the device is active, switch the backend to match the new mode */
    if (cascom_enabled[TAPEPORT_PORT_1]) {
        cascom_audio_stop();
        cascom_net_stop();
        cascom_mode = newmode;
        if (newmode == CASCOM_MODE_AUDIO) {
            cascom_audio_request(TAPEPORT_PORT_1);
        } else if (newmode == CASCOM_MODE_NET) {
            cascom_net_request(TAPEPORT_PORT_1);
        }
    } else {
        cascom_mode = newmode;
    }
    return 0;
}

static int set_cascom_baud(int val, void *param)
{
    if (val < 50 || val > 9600) {
        return -1;
    }
    cascom_baud = val;
    cascom_audio_reconfigure();
    return 0;
}

#ifdef ARCHDEP_HAVE_SERIAL_CONTROL

static int set_cascom_ptt_line(int val, void *param)
{
    cascom_ptt_line = val ? 1 : 0;
    cascom_update_ptt(TAPEPORT_PORT_1);
    return 0;
}

static int set_cascom_ptt_invert(int val, void *param)
{
    cascom_ptt_invert = val ? 1 : 0;
    cascom_update_ptt(TAPEPORT_PORT_1);
    return 0;
}

static int set_cascom_ptt_device(const char *val, void *param)
{
    util_string_set(&cascom_ptt_device, val ? val : "");
    if (cascom_enabled[TAPEPORT_PORT_1]) {
        cascom_ptt_close();
        cascom_ptt_open();
    }
    return 0;
}

#endif /* ARCHDEP_HAVE_SERIAL_CONTROL */

#ifdef USE_PORTAUDIO

static int set_cascom_tx_rate(int val, void *param)
{
    if (val < 4000 || val > 192000) {
        return -1;
    }
    cascom_tx_rate = val;
    cascom_audio_reconfigure();
    return 0;
}

static int set_cascom_rx_rate(int val, void *param)
{
    if (val < 4000 || val > 192000) {
        return -1;
    }
    cascom_rx_rate = val;
    cascom_audio_reconfigure();
    return 0;
}

static int set_cascom_mark_freq(int val, void *param)
{
    if (val < 300 || val > 5000) {
        return -1;
    }
    cascom_mark_freq = val;
    cascom_audio_reconfigure();
    return 0;
}

static int set_cascom_space_freq(int val, void *param)
{
    if (val < 300 || val > 5000) {
        return -1;
    }
    cascom_space_freq = val;
    cascom_audio_reconfigure();
    return 0;
}

static int set_cascom_tx_gain(int val, void *param)
{
    if (val < 0 || val > 100) {
        return -1;
    }
    cascom_tx_gain = val;
    cascom_audio_reconfigure();
    return 0;
}

static int set_cascom_tx_dev(const char *val, void *param)
{
    util_string_set(&cascom_tx_dev, val ? val : "");
    cascom_audio_reconfigure();
    return 0;
}

static int set_cascom_rx_dev(const char *val, void *param)
{
    util_string_set(&cascom_rx_dev, val ? val : "");
    cascom_audio_reconfigure();
    return 0;
}

static int set_cascom_tx_gate(int val, void *param)
{
    cascom_tx_gate = val ? 1 : 0;
    return 0;
}

#endif /* USE_PORTAUDIO */

#ifdef CASCOM_HAVE_NET

static int set_cascom_net_port(int val, void *param)
{
    if (val < 1 || val > 65535) {
        return -1;
    }
    cascom_net_port = val;
    cascom_net_reconfigure();
    return 0;
}

static int set_cascom_net_listen_port(int val, void *param)
{
    if (val < 1 || val > 65535) {
        return -1;
    }
    cascom_net_listen_port = val;
    cascom_net_reconfigure();
    return 0;
}

static int set_cascom_net_peer(const char *val, void *param)
{
    util_string_set(&cascom_net_peer, val ? val : "");
    cascom_net_reconfigure();
    return 0;
}

static int set_cascom_net_rxfilter(const char *val, void *param)
{
    util_string_set(&cascom_net_rxfilter, val ? val : "");
    return 0;
}

#endif /* CASCOM_HAVE_NET */

/* Only the resources whose backend is compiled into this build are registered;
   the PTT, audio and network groups below are added on top of the base group. */

static const resource_int_t resources_int[] = {
    { "CascomMode", CASCOM_MODE_LOOPBACK, RES_EVENT_NO, (resource_value_t)CASCOM_MODE_LOOPBACK,
      &cascom_mode, set_cascom_mode, NULL },
    { "CascomBaud", 1200, RES_EVENT_NO, (resource_value_t)1200,
      &cascom_baud, set_cascom_baud, NULL },
    RESOURCE_INT_LIST_END
};

#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
static const resource_int_t resources_int_ptt[] = {
    { "CascomPttLine", 0, RES_EVENT_NO, (resource_value_t)0,
      &cascom_ptt_line, set_cascom_ptt_line, NULL },
    { "CascomPttInvert", 0, RES_EVENT_NO, (resource_value_t)0,
      &cascom_ptt_invert, set_cascom_ptt_invert, NULL },
    RESOURCE_INT_LIST_END
};

static const resource_string_t resources_string_ptt[] = {
    { "CascomPttDevice", "", RES_EVENT_NO, NULL,
      &cascom_ptt_device, set_cascom_ptt_device, NULL },
    RESOURCE_STRING_LIST_END
};
#endif /* ARCHDEP_HAVE_SERIAL_CONTROL */

#ifdef USE_PORTAUDIO
static const resource_int_t resources_int_audio[] = {
    { "CascomTxRate", 48000, RES_EVENT_NO, (resource_value_t)48000,
      &cascom_tx_rate, set_cascom_tx_rate, NULL },
    { "CascomRxRate", 48000, RES_EVENT_NO, (resource_value_t)48000,
      &cascom_rx_rate, set_cascom_rx_rate, NULL },
    { "CascomMarkFreq", 1200, RES_EVENT_NO, (resource_value_t)1200,
      &cascom_mark_freq, set_cascom_mark_freq, NULL },
    { "CascomSpaceFreq", 2200, RES_EVENT_NO, (resource_value_t)2200,
      &cascom_space_freq, set_cascom_space_freq, NULL },
    { "CascomTxGain", 60, RES_EVENT_NO, (resource_value_t)60,
      &cascom_tx_gain, set_cascom_tx_gain, NULL },
    { "CascomTxGate", 1, RES_EVENT_NO, (resource_value_t)1,
      &cascom_tx_gate, set_cascom_tx_gate, NULL },
    RESOURCE_INT_LIST_END
};

static const resource_string_t resources_string_audio[] = {
    { "CascomTxAudioDev", "", RES_EVENT_NO, NULL,
      &cascom_tx_dev, set_cascom_tx_dev, NULL },
    { "CascomRxAudioDev", "", RES_EVENT_NO, NULL,
      &cascom_rx_dev, set_cascom_rx_dev, NULL },
    RESOURCE_STRING_LIST_END
};
#endif /* USE_PORTAUDIO */

#ifdef CASCOM_HAVE_NET
static const resource_int_t resources_int_net[] = {
    { "CascomNetPort", 10093, RES_EVENT_NO, (resource_value_t)10093,
      &cascom_net_port, set_cascom_net_port, NULL },
    { "CascomNetListenPort", 10093, RES_EVENT_NO, (resource_value_t)10093,
      &cascom_net_listen_port, set_cascom_net_listen_port, NULL },
    RESOURCE_INT_LIST_END
};

static const resource_string_t resources_string_net[] = {
    { "CascomNetPeer", "", RES_EVENT_NO, NULL,
      &cascom_net_peer, set_cascom_net_peer, NULL },
    { "CascomNetRxFilter", "", RES_EVENT_NO, NULL,
      &cascom_net_rxfilter, set_cascom_net_rxfilter, NULL },
    RESOURCE_STRING_LIST_END
};
#endif /* CASCOM_HAVE_NET */

/* ------------------------------------------------------------------------- */
/* device enumeration for the settings UI                                    */

/* grow a NULL-terminated string array, appending a copy of s */
static char **cascom_list_append(char **list, int *n, int *cap, const char *s)
{
    if (*n + 2 > *cap) {
        *cap *= 2;
        list = lib_realloc(list, (size_t)*cap * sizeof(char *));
    }
    list[(*n)++] = lib_strdup(s);
    list[*n] = NULL;
    return list;
}

char **cascom_audio_device_list(int want_input)
{
    int    cap = 8;
    int    n = 0;
    char **list = lib_malloc((size_t)cap * sizeof(char *));

    list[0] = NULL;
    list = cascom_list_append(list, &n, &cap, "");   /* system default */
#ifdef USE_PORTAUDIO
    {
        int          inited_here = 0;
        PaDeviceIndex count;
        PaDeviceIndex i;

        if (!pa_inited) {
            if (Pa_Initialize() == paNoError) {
                inited_here = 1;
            }
        }
        count = Pa_GetDeviceCount();
        for (i = 0; i < count; i++) {
            const PaDeviceInfo *di = Pa_GetDeviceInfo(i);
            if (di == NULL) {
                continue;
            }
            if ((want_input ? di->maxInputChannels : di->maxOutputChannels) > 0) {
                list = cascom_list_append(list, &n, &cap, di->name);
            }
        }
        if (inited_here) {
            Pa_Terminate();
        }
    }
#else
    (void)want_input;
#endif
    return list;
}

char **cascom_serial_device_list(void)
{
    int    cap = 8;
    int    n = 0;
    char **list = lib_malloc((size_t)cap * sizeof(char *));
    char **devices;
    int    i;

    list[0] = NULL;
    list = cascom_list_append(list, &n, &cap, "");   /* none */
    devices = archdep_serial_control_list();
    for (i = 0; devices[i] != NULL; i++) {
        list = cascom_list_append(list, &n, &cap, devices[i]);
        lib_free(devices[i]);
    }
    lib_free(devices);
    return list;
}

void cascom_device_list_free(char **list)
{
    int i;
    if (list == NULL) {
        return;
    }
    for (i = 0; list[i] != NULL; i++) {
        lib_free(list[i]);
    }
    lib_free(list);
}

int cascom_resources_init(int amount)
{
    if (cascom_log == LOG_DEFAULT) {
        cascom_log = log_open("Cascom");
    }

    if (tapeport_device_register(TAPEPORT_DEVICE_CASCOM, &cascom_device) < 0) {
        return -1;
    }

    if (resources_register_int(resources_int) < 0) {
        return -1;
    }
#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
    if (resources_register_string(resources_string_ptt) < 0
        || resources_register_int(resources_int_ptt) < 0) {
        return -1;
    }
#endif
#ifdef USE_PORTAUDIO
    if (resources_register_string(resources_string_audio) < 0
        || resources_register_int(resources_int_audio) < 0) {
        return -1;
    }
#endif
#ifdef CASCOM_HAVE_NET
    if (resources_register_string(resources_string_net) < 0
        || resources_register_int(resources_int_net) < 0) {
        return -1;
    }
#endif
    return 0;
}

void cascom_resources_shutdown(void)
{
#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
    lib_free(cascom_ptt_device);
    cascom_ptt_device = NULL;
#endif
#ifdef USE_PORTAUDIO
    lib_free(cascom_tx_dev);
    lib_free(cascom_rx_dev);
    cascom_tx_dev = NULL;
    cascom_rx_dev = NULL;
#endif
#ifdef CASCOM_HAVE_NET
    lib_free(cascom_net_peer);
    lib_free(cascom_net_rxfilter);
    cascom_net_peer = NULL;
    cascom_net_rxfilter = NULL;
#endif
}

/* ------------------------------------------------------------------------- */
/* command-line options                                                      */

/* Like the resources, only the command-line options whose backend is compiled
   into this build are registered, so `-help` never shows an option that could
   only produce a "not available" situation. */

static const cmdline_option_t cmdline_options[] =
{
    { "-cascommode", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomMode", NULL,
#if defined(USE_PORTAUDIO) && defined(CASCOM_HAVE_NET)
      "<0-2>", "Set Cascom modem mode (0: Loopback self-test, 1: Audio AFSK, 2: AXUDP net)" },
#elif defined(USE_PORTAUDIO)
      "<0-1>", "Set Cascom modem mode (0: Loopback self-test, 1: Audio AFSK)" },
#elif defined(CASCOM_HAVE_NET)
      "<0|2>", "Set Cascom modem mode (0: Loopback self-test, 2: AXUDP net)" },
#else
      "<0>", "Set Cascom modem mode (0: Loopback self-test)" },
#endif
    { "-cascombaud", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomBaud", NULL,
      "<baud>", "Set Cascom baud rate (default 1200)" },
    CMDLINE_LIST_END
};

#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
static const cmdline_option_t cmdline_options_ptt[] =
{
    { "-cascompttdevice", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomPttDevice", NULL,
      "<device>", "Set Cascom PTT serial device, tty path or COM port ('' = PTT disabled)" },
    { "-cascompttline", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomPttLine", NULL,
      "<0-1>", "Set Cascom PTT line (0: RTS, 1: DTR)" },
    { "-cascompttinvert", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "CascomPttInvert", (resource_value_t)1,
      NULL, "Invert Cascom PTT keying polarity" },
    { "+cascompttinvert", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "CascomPttInvert", (resource_value_t)0,
      NULL, "Do not invert Cascom PTT keying polarity (default)" },
    CMDLINE_LIST_END
};
#endif /* ARCHDEP_HAVE_SERIAL_CONTROL */

#ifdef USE_PORTAUDIO
static const cmdline_option_t cmdline_options_audio[] =
{
    { "-cascomtxrate", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomTxRate", NULL,
      "<Hz>", "Set Cascom TX host audio sample rate (default 48000)" },
    { "-cascomrxrate", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomRxRate", NULL,
      "<Hz>", "Set Cascom RX host audio sample rate (default 48000)" },
    { "-cascomtxaudiodev", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomTxAudioDev", NULL,
      "<name>", "Set Cascom TX (output) audio device name substring ('' = default)" },
    { "-cascomrxaudiodev", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomRxAudioDev", NULL,
      "<name>", "Set Cascom RX (input) audio device name substring ('' = default)" },
    { "-cascommarkfreq", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomMarkFreq", NULL,
      "<Hz>", "Set Cascom mark frequency (default 1200)" },
    { "-cascomspacefreq", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomSpaceFreq", NULL,
      "<Hz>", "Set Cascom space frequency (default 2200)" },
    { "-cascomtxgain", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomTxGain", NULL,
      "<0-100>", "Set Cascom TX audio level percent (default 60)" },
    { "-cascomtxgate", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "CascomTxGate", (resource_value_t)1,
      NULL, "Gate (mute) Cascom TX audio unless PTT is keyed (default)" },
    { "+cascomtxgate", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "CascomTxGate", (resource_value_t)0,
      NULL, "Always emit Cascom TX audio, even when PTT is not keyed" },
    CMDLINE_LIST_END
};
#endif /* USE_PORTAUDIO */

#ifdef CASCOM_HAVE_NET
static const cmdline_option_t cmdline_options_net[] =
{
    { "-cascomnetpeer", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomNetPeer", NULL,
      "<host>", "Set Cascom network peer host for TX frames ('' = none)" },
    { "-cascomnetport", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomNetPort", NULL,
      "<port>", "Set Cascom AXUDP peer port (default 10093)" },
    { "-cascomnetlistenport", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomNetListenPort", NULL,
      "<port>", "Set Cascom AXUDP local listen port (default 10093)" },
    { "-cascomnetrxfilter", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "CascomNetRxFilter", NULL,
      "<calls>", "Set Cascom RX callsign accept-list (comma separated, '' = accept all)" },
    CMDLINE_LIST_END
};
#endif /* CASCOM_HAVE_NET */

int cascom_cmdline_options_init(void)
{
    if (cmdline_register_options(cmdline_options) < 0) {
        return -1;
    }
#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
    if (cmdline_register_options(cmdline_options_ptt) < 0) {
        return -1;
    }
#endif
#ifdef USE_PORTAUDIO
    if (cmdline_register_options(cmdline_options_audio) < 0) {
        return -1;
    }
#endif
#ifdef CASCOM_HAVE_NET
    if (cmdline_register_options(cmdline_options_net) < 0) {
        return -1;
    }
#endif
    return 0;
}

/* ------------------------------------------------------------------------- */
/* snapshot                                                                  */

/* TP_CASCOM snapshot module format:

   type  | name      | description
   --------------------------------
   BYTE  | mode      | operating mode
   BYTE  | write_bit | last CASS WRITE line state
   BYTE  | motor     | last CASS MOTOR line state
   BYTE  | rx_drv    | last driven RX line level
 */

static const char snap_module_name[] = "TP_CASCOM";
#define SNAP_MAJOR   0
#define SNAP_MINOR   1

static int cascom_write_snapshot(int port, struct snapshot_s *s, int write_image)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || SMW_B(m, (uint8_t)cascom_mode) < 0
        || SMW_B(m, cascom_write_bit[port]) < 0
        || SMW_B(m, cascom_motor[port]) < 0
        || SMW_B(m, cascom_rx_drv[port]) < 0) {
        snapshot_module_close(m);
        return -1;
    }

    return snapshot_module_close(m);
}

static int cascom_read_snapshot(int port, struct snapshot_s *s)
{
    uint8_t major_version;
    uint8_t minor_version;
    uint8_t mode;
    snapshot_module_t *m;

    m = snapshot_module_open(s, snap_module_name, &major_version, &minor_version);
    if (m == NULL) {
        return -1;
    }

    if (snapshot_version_is_bigger(major_version, minor_version, SNAP_MAJOR, SNAP_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (0
        || SMR_B(m, &mode) < 0
        || SMR_B(m, &cascom_write_bit[port]) < 0
        || SMR_B(m, &cascom_motor[port]) < 0
        || SMR_B(m, &cascom_rx_drv[port]) < 0) {
        goto fail;
    }
    /* run the restored mode through the resource setter so it is range-checked
       and the audio/net backend restarts to match; a snapshot from a build
       with more backends falls back to loopback instead of failing */
    if (resources_set_int("CascomMode", (int)mode) < 0) {
        log_warning(cascom_log,
                    "snapshot mode %d not available in this build, using loopback.",
                    (int)mode);
        if (resources_set_int("CascomMode", CASCOM_MODE_LOOPBACK) < 0) {
            goto fail;
        }
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}
