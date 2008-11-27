/*! \file aciacore.c \n
 *  \author André Fachat, Spiro Trikaliotis\n
 *  \brief  Template file for ACIA 6551 emulation.
 *
 * Written by
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Spiro Trikaliotis <spiro.trikaliotis@gmx.de>
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

#include <assert.h>
#include <stdio.h>

#include "acia.h"
#include "alarm.h"
#include "clkguard.h"
#include "cmdline.h"
#include "interrupt.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "rs232drv.h"
#include "snapshot.h"
#include "translate.h"
#include "types.h"


#undef  DEBUG   /*!< define if you want "normal" debugging output */
#undef  DEBUG_VERBOSE /*!< define if you want very verbose debugging output. */
/* #define DEBUG */
/* #define DEBUG_VERBOSE */

/*! \brief Helper macro for outputting debugging messages */
#ifdef DEBUG
# define DEBUG_LOG_MESSAGE(_x) log_message _x
#else
# define DEBUG_LOG_MESSAGE(_x)
#endif

/*! \brief Helper macro for outputting verbose debugging messages */
#ifdef DEBUG_VERBOSE
# define DEBUG_VERBOSE_LOG_MESSAGE(_x) log_message _x
#else
# define DEBUG_VERBOSE_LOG_MESSAGE(_x)
#endif

/*! \brief specify the transmit state the ACIA is currently in

 \remark
   The numerical values must remain as they are!
   int_acia_tx() relies on them when testing and decrementing
   the in_tx variable!
*/
enum acia_tx_state {
    ACIA_TX_STATE_NO_TRANSMIT = 0, /*!< currently, there is no transmit processed */
    ACIA_TX_STATE_TX_STARTED = 1,  /*!< the transmit has already begun */
    ACIA_TX_STATE_DR_WRITTEN = 2   /*!< the data register has been written, but the byte written is not yet in transmit */
};

static alarm_t *acia_alarm_tx = NULL; /*!< handling of the transmit (TX) alarm */
static alarm_t *acia_alarm_rx = NULL; /*!< handling of the receive (RX) alarm */
static unsigned int acia_int_num;     /*!< the (internal) number for the ACIA interrupt as returned by interrupt_cpu_status_int_new(). */

static int acia_ticks = 21111;  /*!< number of clock ticks per char */
static int acia_ticks_rx = 21111;  /*!< number of clock ticks per char for reception (workaround for NovaTerm, cf. remark for set_acia_ticks() */
static int fd = -1;             /*!< file descriptor used to access the RS232 physical device on the host machine */
static enum acia_tx_state in_tx = ACIA_TX_STATE_NO_TRANSMIT;   /*!< indicates that a transmit is currently ongoing */
static int irq = 0;
static BYTE cmd;        /*!< value of the 6551 command register */
static BYTE ctrl;       /*!< value of the 6551 control register */
static BYTE rxdata;     /*!< data that has been received last */
static BYTE txdata;     /*!< data prepared to be send */
static BYTE status;     /*!< value of the 6551 status register */
static BYTE ectrl;      /*!< value of the extended control register of the turbo232 card */
static int alarm_active_tx = 0;    /*!< 1 if TX alarm is set; else 0 */
static int alarm_active_rx = 0;    /*!< 1 if RX alarm is set; else 0 */

static log_t acia_log = LOG_ERR; /*!< the log where to write debugging messages */

static void int_acia_tx(CLOCK offset, void *data);
static void int_acia_rx(CLOCK offset, void *data);

static BYTE acia_last_read = 0;  /*!< the byte read the last time (for RMW) */

/******************************************************************/

/*! \brief the clock value the TX alarm has last been set to fire at

 \note
  If alarm_active_tx is set to 1, to alarm is
  actually set. If alarm_active_tx is 0, then
  the alarm either has already fired, or it
  already has been cancelled.
*/
static CLOCK acia_alarm_clk_tx = 0;

/*! \brief the clock value the RX alarm has last been set to fire at

 \note
  If alarm_active_rx is set to 1, to alarm is
  actually set. If alarm_active_rx is 0, then
  the alarm either has already fired, or it
  already has been cancelled.
*/
static CLOCK acia_alarm_clk_rx = 0;

/*! \brief the arch-dependant RS232 device to use for this acia implementation */
static int acia_device;

/*! \brief the type of interrupt implemented by the ACIA
 
 The ACIA either implements an IRQ (IK_IRQ), an NMI (IK_NMI),
 or no interrupt at all (IK_NONE).

 \note
   As some cartridges can be switched between these modes,
   it is necessary to remember this value.
*/
static enum cpu_int acia_irq = IK_NONE;

/*! \brief the type of interrupt implemented by the ACIA,
  as defined in the resources

  Essentially, this is the same info as acia_irq. As the
  resource stored in the VICE system is different from
  the actual value in acia_irq, the resources value is
  stored here.
*/
static int acia_irq_res;

/*! \brief the acia variant implemented.
  Specifies if this acia implements a "raw" 6551 device
  (ACIA_MODE_NORMAL), a swiftlink device (ACIA_MODE_SWIFTLINK)
  or a turbo232 device (ACIA_MODE_TURBO232).
*/
static int acia_mode = ACIA_MODE_NORMAL; 

/*! \brief The handshake lines as currently seen by the ACIA */
static enum rs232handshake_out acia_rs232_status_lines = 0;

/******************************************************************/

/*! \brief the bps rates available in the order of the control register

  This array is used to set the bps rate of the 6551.
  For this, the values are set in the same order as
  they are defined in the CONTROL register.

 \remark
   the first value is bogus. It should be 16*external clock.

 \remark
   swiftlink and turbo232 modes use the same table
   except they double the values.
*/
static const double acia_bps_table[16] = {
    10, 50, 75, 109.92, 134.58, 150, 300, 600, 1200, 1800,
    2400, 3600, 4800, 7200, 9600, 19200
};

/*! \brief the extra bps rates of the turbo232 card

   This lists the extra bps rates available in the
   turbo232 card. In the turbo232 card, if the CTRL register
   is set to the bps rate of 10 bps, the extended ctrl
   register determines the bps rates. The extended ctrl
   register is used as an index in this table to get the
   bps rate.

 \remark
   the last value is a bogus value and in the real module
   that value is reserved for future use.
*/
static const double t232_bps_table[4] = {
    230400, 115200, 57600, 28800
};

/******************************************************************/

/*! \internal \brief Change the device resource for this ACIA

 \param val
   The device no. to use for this ACIA

 \param param
   Unused

 \return
   0 on success, -1 on error.

 \remark
   This function is called whenever the resource 
   MYACIA "Dev" is changed.
*/
static int acia_set_device(int val, void *param)
{

    if (fd >= 0) {
        log_error(acia_log,
                  "acia_set_device(): "
                  "Device open, change effective only after close!");
    }

    acia_device = val;
    return 0;
}

/*! \internal \brief Generate an ACIA interrupt

 This function is used to generate an ACIA interrupt.
 Depending upon the type of interrupt to generate
 (IK_NONE, IK_IRQ or IK_NMI), the appropriate function
 is called (or no function at all if the interrupt type
 is set to IK_NONE).

 \param aciairq
   The interrupt type to use.
   Must be one of IK_NONE, IK_IRQ or IK_NMI.

 \param int_num
   the (internal) number for the ACIA interrupt as returned
   by interrupt_cpu_status_int_new()

 \param value
   The state to set this interrupt to.\n

 \remark
   In case of aciairq = IK_NONE, value must be IK_NONE, too.\n
   In case of aciairq = IK_IRQ, value must be one of IK_IRQ or IK_NONE.\n
   In case of aciairq = IK_NMI, value must be one of IK_NMI or IK_NONE.\n
*/
static void acia_set_int(int aciairq, unsigned int int_num, int value)
{
    assert( (value == aciairq) || (value == IK_NONE));

    if (aciairq == IK_IRQ)
        mycpu_set_irq(int_num, value);
    if (aciairq == IK_NMI)
        mycpu_set_nmi(int_num, value);
}

/*! \internal \brief Change the Interrupt resource for this ACIA

 \param val
   The interrupt type to use:
      0 = none,
      1 = IRQ,
      2 = NMI.

 \param param
   Unused

 \return
   0 on success, -1 on error.

 \remark
   This function is called whenever the resource
   MYACIA "Irq" is changed.
*/
static int acia_set_irq(int new_irq_res, void *param)
{
    enum cpu_int new_irq;
    static const enum cpu_int irq_tab[] = { IK_NONE, IK_IRQ, IK_NMI };

    /*
     * if an invalid interrupt type has been given, return
     * with an error.
     */
    if (new_irq_res < 0 || new_irq_res > 2)
        return -1;

    new_irq = irq_tab[new_irq_res];

    if (acia_irq != new_irq) {
        acia_set_int(acia_irq, acia_int_num, IK_NONE);
        if (irq) {
            acia_set_int(new_irq, acia_int_num, new_irq);
        }
    }
    acia_irq = new_irq;
    acia_irq_res = new_irq_res;

    return 0;
}

/*! \internal \brief get the bps rate ("baud rate") of the ACIA

 \return
   the bps rate the acia is currently programmed to.
*/
static double get_acia_bps(void)
{
    switch(acia_mode) {
      case ACIA_MODE_NORMAL:
        return acia_bps_table[ctrl & ACIA_CTRL_BITS_BPS_MASK];

      case ACIA_MODE_SWIFTLINK:
        return acia_bps_table[ctrl & ACIA_CTRL_BITS_BPS_MASK] * 2;

      case ACIA_MODE_TURBO232:
        if ((ctrl & ACIA_CTRL_BITS_BPS_MASK) == ACIA_CTRL_BITS_BPS_16X_EXT_CLK)
            return t232_bps_table[ectrl & T232_ECTRL_BITS_EXT_BPS_MASK];
        else
            return acia_bps_table[ctrl & ACIA_CTRL_BITS_BPS_MASK] * 2;

      default:
        log_message(acia_log, "Invalid acia_mode = %u in get_acia_bps()", acia_mode);
        return acia_bps_table[0]; /* return dummy value */
    }
}

/*! \internal \brief set the ticks between characters of the ACIA according to the bps rate

 Set the ticks that will pass for one character to be transferred
 according to the current ACIA settings.
*/
static void set_acia_ticks(void)
{
    unsigned int bits;

    DEBUG_LOG_MESSAGE((acia_log, "Setting ACIA to %u bps", 
        (unsigned int) get_acia_bps() ));

    switch (ctrl & ACIA_CTRL_BITS_WORD_LENGTH_MASK) {
    case ACIA_CTRL_BITS_WORD_LENGTH_8:
        bits = 8;
        break;
    case ACIA_CTRL_BITS_WORD_LENGTH_7:
        bits = 7;
        break;
    case ACIA_CTRL_BITS_WORD_LENGTH_6:
        bits = 6;
        break;
    default:
        /*
         * this case is only for gcc to calm down, as it wants to warn that
         * bits is used uninitialised - which it is not.
         */
        /* FALL THROUGH */
    case ACIA_CTRL_BITS_WORD_LENGTH_5:
        bits = 5;
        break;
    }

    /* 
     * we neglect the fact that we might have 1.5 stop bits instead of 2
     */
    bits += 1 /* the start bit */
            + ((cmd & ACIA_CMD_BITS_PARITY_ENABLED) ? 1 : 0) /* parity or not */
            + ((ctrl & ACIA_CTRL_BITS_2_STOP) ? 2 : 1);   /* 1 or 2 stop bits */

    /*
     * calculate time in ticks for the data bits
     * including start, stop and parity bits.
     */
    acia_ticks = (int) (machine_get_cycles_per_second() / get_acia_bps() * bits);

    /*
     * Note: With 10 bit, the timing is very hard to NovaTerm 9.6c with 57600 bps
     * on reception. It cannot cope and behaves erroneously, at least when configured
     * for NMI interrupts. This is because the interrupt routine needs too much
     * time to execute, leaving not more than 20 cycles for the non-NMI routine.
     * Thus, it was decided to add 25% "safety margin" to allow NovaTerm to react
     * appropriately. This gives the main program 50 extra cycles.
     * This way, NovaTerm can cope with the transmission at 57600 bps.
     */
    acia_ticks_rx = acia_ticks * 5 / 4;

    /* adjust the alarm rate for reception */
    if (alarm_active_rx) {
        acia_alarm_clk_rx = myclk + acia_ticks_rx;
        alarm_set(acia_alarm_rx, acia_alarm_clk_rx);
        alarm_active_rx = 1;
    }

    /*
     * set the baud rate of the physical device
     */
    rs232drv_set_bps(fd, (unsigned int)get_acia_bps());

}

/*! \internal \brief Change the emulation mode for this ACIA

 \param val
   The exact device type to emulate.
      ACIA_MODE_NORMAL for a "normal" 6551 device,
      ACIA_MODE_SWIFTLINK for a SwiftLink device, or
      ACIA_MODE_TURBO232 for a Turbo232 device.

 \param param
   Unused

 \return
   0 on success, -1 on error.

 \remark
   This function is called whenever the resource
   MYACIA "Mode" is changed.
*/
static int acia_set_mode(int new_mode, void *param)
{
    if (new_mode < ACIA_MODE_LOWEST || new_mode > ACIA_MODE_HIGHEST)
        return -1;

    acia_mode = new_mode;
    set_acia_ticks();

    return 0;
}

/*! \brief integer resources used by the ACIA module */
static const resource_int_t resources_int[] = {
    { MYACIA "Dev", MyDevice, RES_EVENT_NO, NULL,
      &acia_device, acia_set_device, NULL },
    { MYACIA "Irq", MyIrq, RES_EVENT_NO, NULL,
      &acia_irq_res, acia_set_irq, NULL },
    { MYACIA "Mode", ACIA_MODE_NORMAL, RES_EVENT_NO, NULL,
      &acia_mode, acia_set_mode, NULL },
    { NULL }
};

/*! \brief initialize the ACIA resources

 \return
   0 on success, else -1.

 \remark
   Registers the integer resources
*/
int myacia_init_resources(void)
{
    return resources_register_int(resources_int);
}

/*! \brief the command-line options available for the ACIA */
static const cmdline_option_t cmdline_options[] = {
    { "-myaciadev", SET_RESOURCE, 1,
      NULL, NULL, MYACIA "Dev", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_SPECIFY_ACIA_RS232_DEVICE,
      "<0-3>", NULL },
    { NULL }
};

/*! \brief initialize the command-line options

 \return
   0 on success, else -1.

 \remark
   Registers the command-line options
*/
int myacia_init_cmdline_options(void) {
    return cmdline_register_options(cmdline_options);
}

/******************************************************************/
/* auxiliary functions */

/*! \internal \brief Prevent clock overflow by adjusting clock value

 \param sub
   The number of clock ticks to adjust the clock by subtracting
   from the current value

 \param var
   The data as has been given to clk_guard_add_callback() as
   3rd parameter. For this implementation, always NULL.

 \remark
   In order to prevent a clock overflow, the system is able
   to subtract a given amount from the clock values. When this
   happens, this function is called in order for the module to
   adjust its own values.
*/
static void clk_overflow_callback(CLOCK sub, void *var)
{
    assert(var == NULL);

    acia_alarm_clk_tx -= sub;
    acia_alarm_clk_rx -= sub;
}

/*! \internal \brief Get the modem status and set the status register accordingly

 This function reads the physical modem status lines (DSR, DCD)
 and sets the emulated ACIA status register accordingly.

 \return
   The new value of the status register
*/
static int acia_get_status(void) {
    enum rs232handshake_in modem_status = rs232drv_get_status(fd);

    status &= ~ (ACIA_SR_BITS_DCD | ACIA_SR_BITS_DSR);

    if ( modem_status & RS232_HSI_CTS ) {
        status |= ACIA_SR_BITS_DCD; /* we treat CTS like DCD */
    }

    if ( modem_status & RS232_HSI_DSR ) {
        status |= ACIA_SR_BITS_DSR;
    }

    return status;
}

/*! \internal \brief Set the handshake (output) lines to the status of the cmd register

 This function sets the physical handshake lines accordingly
 to the state of the emulated ACIA cmd register.
*/
static void acia_set_handshake_lines(void)
{
    switch (cmd & ACIA_CMD_BITS_TRANSMITTER_MASK) {
        case ACIA_CMD_BITS_TRANSMITTER_NO_RTS:
            /* unset RTS, we are NOT ready to receive */
            acia_rs232_status_lines &= ~ RS232_HSO_RTS;
            if (alarm_active_rx) {
                /* diable RX alarm */
                alarm_active_rx = 0;
                alarm_unset(acia_alarm_rx);
            }
            break;

        case ACIA_CMD_BITS_TRANSMITTER_BREAK:
            /* FALL THROUGH */
        case ACIA_CMD_BITS_TRANSMITTER_TX_WITH_IRQ:
            /* FALL THROUGH */
        case ACIA_CMD_BITS_TRANSMITTER_TX_WO_IRQ:
            /* set RTS, we are ready to receive */
            acia_rs232_status_lines |= RS232_HSO_RTS;

            if ( ! alarm_active_rx ) {
                /* enable RX alarm */
                alarm_active_rx = 1;
                set_acia_ticks();
            }
            break;
    }

    if (cmd & ACIA_CMD_BITS_DTR_ENABLE_RECV_AND_IRQ) {
        /* set DTR, we are ready to transmit and receive */
        acia_rs232_status_lines |= RS232_HSO_DTR;
    }
    else {
        /* unset DTR, we are NOT ready to receive or to transmit */
        acia_rs232_status_lines &= ~ RS232_HSO_DTR;
    }
    /* set the RTS and the DTR status */
    rs232drv_set_status(fd, acia_rs232_status_lines);
}

/*! \brief initialize the ACIA */
void myacia_init(void)
{
    acia_int_num = interrupt_cpu_status_int_new(maincpu_int_status, MYACIA);

    acia_alarm_tx = alarm_new(mycpu_alarm_context, MYACIA, int_acia_tx, NULL);
    acia_alarm_rx = alarm_new(mycpu_alarm_context, MYACIA, int_acia_rx, NULL);

    clk_guard_add_callback(mycpu_clk_guard, clk_overflow_callback, NULL);

    if (acia_log == LOG_ERR)
        acia_log = log_open(MYACIA);
}

/*! \brief reset the ACIA */
void myacia_reset(void)
{
    DEBUG_LOG_MESSAGE((acia_log, "reset_myacia"));

    acia_rs232_status_lines = 0;
    rs232drv_set_status(fd, acia_rs232_status_lines);

    cmd = ACIA_CMD_DEFAULT_AFTER_HW_RESET;
    ctrl = ACIA_CTRL_DEFAULT_AFTER_HW_RESET;
    ectrl = T232_ECTRL_DEFAULT_AFTER_HW_RESET;

    set_acia_ticks();

    status = ACIA_SR_DEFAULT_AFTER_HW_RESET;
    in_tx = ACIA_TX_STATE_NO_TRANSMIT;

    if (fd >= 0)
        rs232drv_close(fd);
    fd = -1;

    alarm_unset(acia_alarm_tx);
    alarm_unset(acia_alarm_rx);
    alarm_active_tx = 0;
    alarm_active_rx = 0;

    acia_set_int(acia_irq, acia_int_num, IK_NONE);
    irq = 0;
}

/******************************************************************/
/* dump definitions and functions */

/* -------------------------------------------------------------------------- */
/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0/0
 */

#define ACIA_DUMP_VER_MAJOR      1 /*!< the major version number of the dump data */
#define ACIA_DUMP_VER_MINOR      0 /*!< the minor version number of the dump data */

/*
 * Layout of the dump data:
 *
 * UBYTE        TDR     Transmit Data Register
 * UBYTE        RDR     Receiver Data Register
 * UBYTE        SR      Status Register (includes state of IRQ line)
 * UBYTE        CMD     Command Register
 * UBYTE        CTRL    Control Register
 *
 * UBYTE        IN_TX   0 = no data to tx; 2 = TDR valid; 1 = in transmit (cf. enum acia_tx_state)
 *
 * DWORD        TICKSTX ticks till the next TDR empty interrupt
 *
 * DWORD        TICKSRX ticks till the next RDF empty interrupt
 *                      TICKSRX has been added with 2.0.9; if it does not
 *                      exist on read, it is assumed that it has the same
 *                      value as TICKSTX to emulate the old behaviour.
 */

/*! the name of this module */
static const char module_name[] = MYACIA;

/*! \brief Write the snapshot module for the ACIA

 \param p
   Pointer to the snapshot data

 \return
   0 on success, -1 on error

 \remark
   Is it sensible to put the ACIA into a snapshot? It is unlikely
   the "other side" of the connection will be able to handle this
   case if a transfer was under way, anyway.

 \todo FIXME!!!  Error check.

 \todo FIXME!!!  If no connection, emulate carrier lost or so.
*/
int myacia_snapshot_write_module(snapshot_t *p)
{
    snapshot_module_t *m;

    m = snapshot_module_create(p, module_name, (BYTE)ACIA_DUMP_VER_MAJOR,
                               (BYTE)ACIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    SMW_B(m, txdata);
    SMW_B(m, rxdata);
    SMW_B(m, (BYTE)(acia_get_status() | (irq ? ACIA_SR_BITS_IRQ : 0)));
    SMW_B(m, cmd);
    SMW_B(m, ctrl);
    SMW_B(m, (BYTE)(in_tx));

    if (alarm_active_tx) {
        SMW_DW(m, (acia_alarm_clk_tx - myclk));
    } else {
        SMW_DW(m, 0);
    }

    /* new with VICE 2.0.9 */

    if (alarm_active_rx) {
        SMW_DW(m, (acia_alarm_clk_rx - myclk));
    } else {
        SMW_DW(m, 0);
    }

    snapshot_module_close(m);

    return 0;
}

/*! \brief Read the snapshot module for the ACIA

 \param p
   Pointer to the snapshot data

 \return
   0 on success, -1 on error

 \remark
   The format has been extended in VICE 2.0.9.
   However, the version number remained the same.
   This function tries to read the new values.
   If they are not available, it mimics the old
   behaviour and reports success, anyway.

 \remark
   It is unclear if it is sensible to mimic the
   old behaviour, as the old implementation was
   severely broken.
*/
int myacia_snapshot_read_module(snapshot_t *p)
{
    BYTE vmajor, vminor;
    BYTE byte;
    DWORD dword;
    snapshot_module_t *m;

    alarm_unset(acia_alarm_tx);   /* just in case we don't find module */
    alarm_unset(acia_alarm_rx);   /* just in case we don't find module */
    alarm_active_tx = 0;
    alarm_active_rx = 0;

    mycpu_set_int_noclk(acia_int_num, 0);

    m = snapshot_module_open(p, module_name, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != ACIA_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    SMR_B(m, &txdata);
    SMR_B(m, &rxdata);

    irq = 0;
    SMR_B(m, &status);
    if (status & ACIA_SR_BITS_IRQ) {
        status &= ~ACIA_SR_BITS_IRQ;
        irq = 1;
        mycpu_set_int_noclk(acia_int_num, acia_irq);
    } else {
        mycpu_set_int_noclk(acia_int_num, 0);
    }

    SMR_B(m, &cmd);
    if ((cmd & ACIA_CMD_BITS_DTR_ENABLE_RECV_AND_IRQ) && (fd < 0)) {
        fd = rs232drv_open(acia_device);
        acia_set_handshake_lines();
    } else
        if ((fd >= 0) && !(cmd & ACIA_CMD_BITS_DTR_ENABLE_RECV_AND_IRQ)) {
        rs232drv_close(fd);
        fd = -1;
    }

    SMR_B(m, &ctrl);
    set_acia_ticks();

    SMR_B(m, &byte);
    in_tx = byte;

    SMR_DW(m, &dword);
    if (dword) {
        acia_alarm_clk_tx = myclk + dword;
        alarm_set(acia_alarm_tx, acia_alarm_clk_tx);
        alarm_active_tx = 1;

        /*
         * for compatibility reasons of old snapshots with new ones,
         * set the RX alarm to the same value.
         * if we have a new snapshot (2.0.9 and up), this will be
         * overwritten directly afterwards.
         */
        acia_alarm_clk_rx = myclk + dword;
        alarm_set(acia_alarm_rx, acia_alarm_clk_rx);
        alarm_active_rx = 1;
    }

    /*
     * this is new with VICE 2.0.9; thus, only use the settings
     * if it does exist.
     */
    if ( SMR_DW(m, &dword) >= 0 ) {
        if (dword) {
            acia_alarm_clk_rx = myclk + dword;
            alarm_set(acia_alarm_rx, acia_alarm_clk_rx);
            alarm_active_rx = 1;
        } else {
            alarm_unset(acia_alarm_rx);
            alarm_active_rx = 0;
        }
    }

    if (snapshot_module_close(m) < 0)
        return -1;

    return 0;
}


/*! \brief write the ACIA register values
  This function is used to write the ACIA values from the computer.

  \param addr
    The address of the ACIA register to write

  \param byte
    The value to set the register to
*/
void REGPARM2 myacia_store(WORD addr, BYTE byte)
{
    int acia_register_size;

    DEBUG_LOG_MESSAGE((acia_log, "store_myacia(%04x,%02x)", addr, byte));

    if (mycpu_rmw_flag) {
        myclk --;
        mycpu_rmw_flag = 0;
        myacia_store(addr, acia_last_read);
        myclk ++;
    }

    if (acia_mode==ACIA_MODE_TURBO232)
      acia_register_size=7;
    else
      acia_register_size=3;

    switch(addr & acia_register_size) {
      case ACIA_DR:
        txdata = byte;
        if (cmd & ACIA_CMD_BITS_DTR_ENABLE_RECV_AND_IRQ) {
            if (in_tx == ACIA_TX_STATE_NO_TRANSMIT) {
                in_tx = ACIA_TX_STATE_DR_WRITTEN;
            } else {
                if (in_tx == ACIA_TX_STATE_DR_WRITTEN) {
                    log_message(acia_log, "ACIA: data register written "
                        "although data has not been sent yet.");
                }
                in_tx = ACIA_TX_STATE_DR_WRITTEN;
            }
            if (alarm_active_tx == 0) {
                acia_alarm_clk_tx = myclk + 1;
                alarm_set(acia_alarm_tx, acia_alarm_clk_tx);
                alarm_active_tx = 1;
            }
            status &= ~ ACIA_SR_BITS_TRANSMIT_DR_EMPTY; /* clr TDRE */
        }
        break;
      case ACIA_SR:
        if (fd >= 0)
            rs232drv_close(fd);
        fd = -1;
        status &= ~ ACIA_SR_BITS_OVERRUN_ERROR;
        cmd &= ACIA_CMD_BITS_PARITY_TYPE_MASK | ACIA_CMD_BITS_PARITY_ENABLED;
        in_tx = ACIA_TX_STATE_NO_TRANSMIT;
        acia_set_int(acia_irq, acia_int_num, IK_NONE);
        irq = 0;
        alarm_unset(acia_alarm_tx);
        alarm_active_tx = 0;
        break;
      case ACIA_CTRL:
        ctrl = byte;
        set_acia_ticks();
        break;
      case ACIA_CMD:
        cmd = byte;
        acia_set_handshake_lines();
        if ((cmd & ACIA_CMD_BITS_DTR_ENABLE_RECV_AND_IRQ) && (fd < 0)) {
            fd = rs232drv_open(acia_device);
            /* enable RX alarm */
            alarm_active_rx = 1;
            set_acia_ticks();
        } else
            if ((fd >= 0) && !(cmd & ACIA_CMD_BITS_DTR_ENABLE_RECV_AND_IRQ)) {
                rs232drv_close(fd);
                alarm_unset(acia_alarm_tx);
                alarm_active_tx = 0;
                fd = -1;
        }
        break;
      case T232_ECTRL:
        if ((ctrl & ACIA_CTRL_BITS_BPS_MASK) == ACIA_CTRL_BITS_BPS_16X_EXT_CLK) {
          ectrl=byte;
          set_acia_ticks();
        }
    }
}

/*! \brief read the ACIA register values

  This function is used to read the ACIA values from the computer.
  All side-effects are executed.

  \param addr
    The address of the ACIA register to read

  \return
    The value the register has
*/
BYTE REGPARM1 myacia_read(WORD addr)
{
#if 0 /* def DEBUG */
    static BYTE myacia_read_(WORD);
    BYTE byte = myacia_read_(addr);
    static WORD last_addr = 0;
    static BYTE last_byte = 0;

    if ((addr != last_addr) || (byte != last_byte)) {
        DEBUG_LOG_MESSAGE((acia_log, "read_myacia(%04x) -> %02x", addr, byte));
    }
    last_addr = addr; last_byte = byte;
    return byte;
}
static BYTE myacia_read_(WORD addr)
{
#endif
    int acia_register_size;

    if (acia_mode==ACIA_MODE_TURBO232)
      acia_register_size=7;
    else
      acia_register_size=3;

    switch(addr & acia_register_size) {
      case ACIA_DR:
        status &= ~ ACIA_SR_BITS_RECEIVE_DR_FULL;
        acia_last_read = rxdata;
        return rxdata;
      case ACIA_SR:
        {
            BYTE c = acia_get_status() | (irq ? ACIA_SR_BITS_IRQ : 0);
            acia_set_int(acia_irq, acia_int_num, IK_NONE);
            irq = 0;
            acia_last_read = c;
            return c;
        }
      case ACIA_CTRL:
        acia_last_read = ctrl;
        return ctrl;
      case ACIA_CMD:
        acia_last_read = cmd;
        return cmd;
      case T232_NDEF1:
      case T232_NDEF2:
      case T232_NDEF3:
        return 0xff;
      case T232_ECTRL:
        return ectrl 
                     + ((ctrl & ACIA_CTRL_BITS_BPS_MASK) == ACIA_CTRL_BITS_BPS_16X_EXT_CLK) 
                       ? T232_ECTRL_BITS_EXT_ACTIVE 
                       : 0;
    }
    /* should never happen */
    return 0;
}

/*! \brief read the ACIA register values without side effects
  This function reads the ACIA values, so they can be accessed like
  an array of bytes. No side-effects that would be performed if a real
  read access would occur are executed.

  \param addr
    The address of the ACIA register to read

  \return
    The value the register has

  \todo
    Currently unused
*/
BYTE myacia_peek(WORD addr)
{
    switch(addr & 3) {
      case ACIA_DR:
        return rxdata;
      case ACIA_SR:
        {
            BYTE c = status | (irq ? ACIA_SR_BITS_IRQ : 0);
            return c;
        }
      case ACIA_CTRL:
        return ctrl;
      case ACIA_CMD:
        return cmd;
    }
    return 0;
}

/******************************************************************/
/* alarm functions */

/*! \internal \brief Transmit (TX) alarm function 

 This function is called when the transmit alarm fires.
 It checks if there is any data to send. If there is some,
 this data is sent to the physical RS232 device.

 \param offset
   The clock offset this alarm is executed at.

   The current implementation of the emulation core does
   not allow to guarantee that the alarm will fire exactly
   at the time it was scheduled at. The offset tells the
   alarm function how many cycles have passed since the
   time the alarm was scheduled to fire. Thus, (myclk - offset)
   yiels the clock count which the alarm was scheduled to.

 \param data
   Additional data defined in the call to alarm_new().
   For the acia implementation, this is always NULL.

 \remark
   If we just transmitted a value, the alarm is re-scheduled for
   the time when the transmission has completed. This way, we
   ensure that we do not send out faster than a real ACIA could
   do.

 \todo
   If no transmit is in progress (in_tx == ACIA_TX_STATE_NO_TRANSMIT),
   it is not necessary to schedule a new alarm.
*/
static void int_acia_tx(CLOCK offset, void *data)
{
    DEBUG_VERBOSE_LOG_MESSAGE((acia_log, "int_acia_tx(offset=%ld, myclk=%d", offset, myclk));

    assert(data == NULL);

    if ((in_tx == ACIA_TX_STATE_DR_WRITTEN) && (fd >= 0)) {
        rs232drv_putc(fd,txdata);

        /* tell the status register that the transmit register is empty */
        status |= ACIA_SR_BITS_TRANSMIT_DR_EMPTY;

        /* generate an interrupt if the ACIA was programmed to generate one */
        if ( (cmd & ACIA_CMD_BITS_TRANSMITTER_MASK) == ACIA_CMD_BITS_TRANSMITTER_TX_WITH_IRQ ) {
            acia_set_int(acia_irq, acia_int_num, acia_irq);
            irq = 1;
        }
    }

    if (in_tx != ACIA_TX_STATE_NO_TRANSMIT) {
        /*
         * ACIA_TX_STATE_DR_WRITTEN is decremented to ACIA_TX_STATE_TX_STARTED
         * ACIA_TX_STATE_TX_STARTED is decremented to ACIA_TX_STATE_NO_TRANSMIT
         */
        in_tx--;
    }

    if (in_tx != ACIA_TX_STATE_NO_TRANSMIT) {
        /* re-schedule alarm */
        acia_alarm_clk_tx = myclk + acia_ticks;
        alarm_set(acia_alarm_tx, acia_alarm_clk_tx);
        alarm_active_tx = 1;

    }
    else {
        alarm_unset(acia_alarm_tx);
        alarm_active_tx = 0;
    }
}

/*! \internal \brief Receive (RX) alarm function 

 This function is called when the receive alarm fires.
 It checks if there is any data received. If there is some,
 this data is made available in the ACIA data register (DR)

 \param offset
   The clock offset this alarm is executed at.

   The current implementation of the emulation core does
   not allow to guarantee that the alarm will fire exactly
   at the time it was scheduled at. The offset tells the
   alarm function how many cycles have passed since the
   time the alarm was scheduled to fire. Thus, (myclk - offset)
   yiels the clock count which the alarm was scheduled to.

 \param data
   Additional data defined in the call to alarm_new().
   For the acia implementation, this is always NULL.

 \remark
   The alarm is re-scheduled for the time when the reception
   has completed.
*/
static void int_acia_rx(CLOCK offset, void *data)
{
    DEBUG_VERBOSE_LOG_MESSAGE((acia_log, "int_acia_rx(offset=%ld, myclk=%d", offset, myclk));

    assert(data == NULL);

    do {
        BYTE received_byte;

        if (fd < 0) {
            break;
        }

        if ( ! rs232drv_getc(fd, &received_byte) ) {
            break;
        }

        DEBUG_LOG_MESSAGE((acia_log, "received byte: %u = '%c'.", 
            (unsigned) received_byte, received_byte));

        /*! \todo: What happens on the real 6551? Is the old value overwritten in
         * case of an overrun, or is it not?
         */
        rxdata = received_byte;

        /* generate an interrupt if the ACIA was configured to generate one */
        if ( ! (cmd & ACIA_CMD_BITS_IRQ_DISABLED) ) {
            acia_set_int(acia_irq, acia_int_num, acia_irq);
            irq = 1;
        }

        if ( status & ACIA_SR_BITS_RECEIVE_DR_FULL) {
            status |= ACIA_SR_BITS_OVERRUN_ERROR;
            break;
        }

        status |= ACIA_SR_BITS_RECEIVE_DR_FULL;

    } while (0);


    acia_alarm_clk_rx = myclk + acia_ticks_rx;
    alarm_set(acia_alarm_rx, acia_alarm_clk_rx);
    alarm_active_rx = 1;
}
