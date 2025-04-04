/*
 * vsidstubs.c - dummies for unneeded/unused functions
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include <stdbool.h>

#include "c64.h"
#include "c64-midi.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "c64fastiec.h"
#include "c64iec.h"
#include "c64mem.h"
#include "c64-cmdline-options.h"
#include "c64_256k.h"
#include "cartridge.h"
#include "cbmdos.h"
#include "cia.h"
#include "clockport.h"
#include "diskcontents.h"
#include "diskcontents-block.h"
#include "diskimage.h"
#include "drive.h"
#include "drive-check.h"
#include "driveimage.h"
#include "driver-select.h"
#include "drivetypes.h"
#include "ds1602.h"
#include "fileio.h"
#include "fsdevice.h"
#include "fsdevice-filename.h"
#include "gfxoutput.h"
#include "iecbus.h"
#include "iecdrive.h"
#include "imagecontents.h"
#include "midi.h"
#include "machine.h"
#include "machine-bus.h"
#include "machine-drive.h"
#include "machine-printer.h"
#include "pet/petpia.h"
#include "printer.h"
#include "sampler.h"
#include "snapshot.h"
#include "tap.h"
#include "tape.h"
#include "tapecart.h"
#include "tapeport.h"
#include "tapecontents.h"
#include "tape-snapshot.h"
#include "userport.h"
#include "userport_io_sim.h"
#ifdef HAVE_LIBCURL
#include "userport_wic64.h"
#endif
#include "vdrive.h"
#include "vdrive-bam.h"
#include "vdrive-command.h"
#include "vdrive-iec.h"
#include "vdrive-internal.h"
#include "vicii-phi1.h"
#include "ds1202_1302.h"

/*******************************************************************************
    Memory related
*******************************************************************************/

int c64_256k_enabled = 0; /* always 0, needed from c64gluelogic.c */

/* needed from c64gluelogic.c */
void c64_256k_cia_set_vbank(int ciabank)
{
}

/*******************************************************************************
    Drive related
*******************************************************************************/

int machine_drive_image_attach(struct disk_image_s *image, unsigned int unit, unsigned int drive)
{
    return -1;
}

int machine_drive_image_detach(struct disk_image_s *image, unsigned int unit, unsigned int drive)
{
    return -1;
}

void machine_drive_stub(void)
{
}


static drive_type_info_t drive_dummy_list[] = {
    { NULL, -1 }
};


/** \brief  Dummy function
 *
 * Added here to make gtk3/widgets/drivetypewidget.c compile, due to using
 * static libraries. This function will not be used in the VSID Gtk3 UI
 */
drive_type_info_t *machine_drive_get_type_info_list(void)
{
    return drive_dummy_list;
}

int drive_check_expansion2000(int type)
{
    return 0;
}

int drive_check_expansion4000(int type)
{
    return 0;
}

int drive_check_expansion6000(int type)
{
    return 0;
}

int drive_check_expansion8000(int type)
{
    return 0;
}

int drive_check_expansionA000(int type)
{
    return 0;
}

int drive_check_profdos(int type)
{
    return 0;
}

int drive_check_stardos(int type)
{
    return 0;
}

int drive_check_supercard(int type)
{
    return 0;
}

int drive_check_rtc(int type)
{
    return 0;
}

int drive_check_iec(int type)
{
    return 0;
}

int drive_check_image_format(unsigned int format, unsigned int dnr)
{
    return -1;
}

/*******************************************************************************
    Cartridge system
*******************************************************************************/

/* Expansion port signals.  */
export_t export = { 0, 0, 0, 0 }; /* c64 export */

/* the following two are used by the old non cycle exact vic-ii emulation */
static uint8_t mem_phi[0x1000];

int cartridge_attach_image(int type, const char *filename)
{
    return -1;
}

int cartridge_attach_add_image(int type, const char *filename)
{
    return -1;
}

void cartridge_detach_image(int type)
{
}

uint8_t *ultimax_romh_phi1_ptr(uint16_t addr)
{
    return mem_phi;
}

uint8_t *ultimax_romh_phi2_ptr(uint16_t addr)
{
    return mem_phi;
}

int cartridge_save_image(int type, const char *filename)
{
    return -1;
}

int cartridge_save_secondary_image(int type, const char *filename)
{
    return -1;
}

int cartridge_flush_image(int type)
{
    return -1;
}

int cartridge_flush_secondary_image(int type)
{
    return -1;
}

int cartridge_can_save_image(int crtid)
{
    return 0;
}

int cartridge_can_flush_image(int crtid)
{
    return 0;
}

int cartridge_can_save_secondary_image(int crtid)
{
    return 0;
}

int cartridge_can_flush_secondary_image(int crtid)
{
    return 0;
}

int cartridge_enable(int crtid)
{
    return -1;
}

int cartridge_disable(int crtid)
{
    return -1;
}

int cartridge_type_enabled(int crtid)
{
    return 0;
}

void cartridge_set_default(void)
{
}

void cartridge_unset_default(void)
{
}

cartridge_info_t *cartridge_get_info_list(void)
{
    return NULL;
}

/* return cartridge type of main slot
   returns 0 (CARTRIDGE_CRT) if crt file */
int cartridge_get_id(int slot)
{
    return CARTRIDGE_NONE;
}

char *cartridge_get_filename_by_slot(int slot)
{
    return NULL;
}

void cartridge_trigger_freeze(void)
{
}

midi_interface_t midi_interface[] = {
    MIDI_INFERFACE_LIST_END
};


/*******************************************************************************
    clockport
*******************************************************************************/

clockport_supported_devices_t clockport_supported_devices[] = { { 0, NULL } };


/*******************************************************************************
    gfxoutput drivers
*******************************************************************************/

int gfxoutput_early_init(int help)
{
    return 0;
}

void gfxoutput_shutdown(void)
{
}

int gfxoutput_num_drivers(void)
{
    return 0;
}

/* FIXME: this stub can be removed once all GUI's have been adapted to
          not use this call for vsid */
gfxoutputdrv_t *gfxoutput_get_driver(const char *drvname)
{
    return NULL;
}

/* FIXME: this stub can be removed once all GUI's have been adapted to
          not use this call for vsid */
gfxoutputdrv_t *gfxoutput_drivers_iter_next(void)
{
    return NULL;
}

/* FIXME: this stub can be removed once all GUI's have been adapted to
          not use this call for vsid */
gfxoutputdrv_t *gfxoutput_drivers_iter_init(void)
{
    return NULL;
}

/* FIXME: this table can be removed once all GUI's have been adapted to
          not use this table for vsid */
gfxoutputdrv_format_t ffmpegdrv_formatlist[] =
{
    { NULL, NULL, NULL, 0 }
};


int gfxoutput_resources_init(void)
{
    return 0;
}

int gfxoutput_cmdline_options_init(void)
{
    return 0;
}

/*******************************************************************************
    printers
*******************************************************************************/

void printer_shutdown(void)
{
}

int printer_serial_late_init(void)
{
    return 0;
}

/* FIXME: this stub can be removed once all GUI's have been adapted to
          not use this call for vsid */
void printer_formfeed(unsigned int prnr)
{
}


/*******************************************************************************
    rtc
*******************************************************************************/

/* FIXME: this stub can be removed once the drive code has been stubbed */
rtc_ds1216e_t *ds1216e_init(char *device)
{
    return NULL;
}

/* FIXME: this stub can be removed once the drive code has been stubbed */
void ds1216e_destroy(rtc_ds1216e_t *context, int save)
{
}

uint8_t ds1216e_read(rtc_ds1216e_t *context, uint16_t address, uint8_t origbyte)
{
    return 0;
}

rtc_ds1202_1302_t *ds1202_1302_init(char *device, int rtc_type)
{
    return NULL;
}

void ds1202_1302_set_lines(rtc_ds1202_1302_t *context, unsigned int ce_line, unsigned int sclk_line, unsigned int input_bit)
{
}

uint8_t ds1202_1302_read_data_line(rtc_ds1202_1302_t *context)
{
    return 1;
}

void ds1202_1302_destroy(rtc_ds1202_1302_t *context, int save)
{
}

int ds1202_1302_write_snapshot(rtc_ds1202_1302_t *context, snapshot_t *s)
{
    return -1;
}

int ds1202_1302_read_snapshot(rtc_ds1202_1302_t *context, snapshot_t *s)
{
    return -1;
}

int ds1602_write_snapshot(rtc_ds1602_t *context, snapshot_t *s)
{
    return -1;
}

void ds1602_destroy(rtc_ds1602_t *context, int save)
{
}

rtc_ds1602_t *ds1602_init(char *device, time_t offset0)
{
    return NULL;
}

uint8_t ds1602_read_data_line(rtc_ds1602_t *context)
{
    return 0;
}

void ds1602_set_data_line(rtc_ds1602_t *context, uint8_t data)
{
}

void ds1602_set_clk_line(rtc_ds1602_t *context, uint8_t data)
{
}

void ds1602_set_reset_line(rtc_ds1602_t *context, uint8_t data)
{
}

int ds1602_read_snapshot(rtc_ds1602_t *context, snapshot_t *s)
{
    return -1;
}

/*******************************************************************************
    tape
*******************************************************************************/

tape_image_t *tape_image_dev[TAPEPORT_MAX_PORTS] = { NULL };

int tape_image_attach(unsigned int unit, const char *name)
{
    return 0;
}

void tape_shutdown(void)
{
}

int tape_tap_attached(int port)
{
    return 0;
}

int tape_seek_start(tape_image_t *tape_image)
{
    return 0;
}

int tape_seek_to_file(tape_image_t *tape_image, unsigned int file_number)
{
    return 0;
}

void tape_image_event_playback(unsigned int unit, const char *filename)
{
}

int tape_image_detach(unsigned int unit)
{
    return 0;
}

void tape_image_detach_all(void)
{
}

int tap_seek_start(tap_t *tap)
{
    return 0;
}

int tape_image_create(const char *name, unsigned int type)
{
    return 0;
}

int tape_snapshot_write_module(int port, snapshot_t *s, int save_image)
{
    return 0;
}

int tape_snapshot_read_module(int port, snapshot_t *s)
{
    return 0;
}

int tape_read(tape_image_t *tape_image, uint8_t *buf, size_t size)
{
    return 0;
}

tape_file_record_t *tape_get_current_file_record(tape_image_t *tape_image)
{
    return NULL;
}

int tape_image_open(tape_image_t *tape_image)
{
    return -1;
}

int tape_image_close(tape_image_t *tape_image)
{
    return 0;
}

int tape_internal_close_tape_image(tape_image_t *tape_image)
{
    return 0;
}

tape_image_t *tape_internal_open_tape_image(const char *name, unsigned int read_only)
{
    return NULL;
}

int tape_seek_to_next_file(tape_image_t *tape_image, unsigned int allow_rewind)
{
    return -1;
}

unsigned long tape_get_offset(tape_image_t *tape_image)
{
    return -1;
}

void tape_get_header(tape_image_t *tape_image, uint8_t *name)
{
}

const char *tape_get_file_name(int port)
{
    return NULL;
}

int tapeport_valid_port(int port)
{
    return 0;
}

/*****************************************************************************
 *  tapecart                                                                 *
 ****************************************************************************/

int tapecart_is_valid(const char *filename)
{
    return 0;   /* FALSE */
}

int tapecart_attach_tcrt(const char *filename, void *unused)
{
    return -1;
}

int tapecart_flush_tcrt(void)
{
    return -1;
}

/*******************************************************************************
    imagecontents
*******************************************************************************/

image_contents_screencode_t *image_contents_to_screencode(image_contents_t *contents)
{
    return NULL;
}

char *image_contents_filename_by_number(image_contents_t *contents, unsigned int file_index)
{
    return NULL;
}

image_contents_t *diskcontents_filesystem_read(const char *file_name)
{
    return NULL;
}

void image_contents_destroy(image_contents_t *contents)
{
}

char *image_contents_file_to_string(image_contents_file_list_t * p, char convert_to_ascii)
{
    return NULL;
}

char *image_contents_to_string(image_contents_t * contents, char convert_to_ascii)
{
    return NULL;
}

image_contents_t *tapecontents_read(const char *file_name)
{
    return NULL;
}

image_contents_t *diskcontents_block_read(struct vdrive_s *vdrive, int part)
{
    return NULL;
}

/*******************************************************************************
    fileio
*******************************************************************************/

void fileio_close(fileio_info_t *info)
{
}

fileio_info_t *fileio_open(const char *file_name, const char *path,
                                unsigned int format, unsigned int command,
                                unsigned int type, int *reclenp)
{
    return NULL;
}

unsigned int fileio_read(fileio_info_t *info, uint8_t *buf, unsigned int len)
{
    return 0;
}

unsigned int fileio_get_bytes_left(fileio_info_t *info)
{
    return 0;
}

unsigned int fileio_ferror(fileio_info_t *info)
{
    return 0;
}

unsigned int fileio_write(fileio_info_t *info, uint8_t *buf, unsigned int len)
{
    return 0;
}

unsigned int fileio_rename(const char *src_name, const char *dest_name, const char *path, unsigned int format)
{
    return 0;
}

unsigned int fileio_scratch(const char *file_name, const char *path, unsigned int format)
{
    return 0;
}


/*******************************************************************************
    fileio
*******************************************************************************/

void fsdevice_init(void)
{
}

void fsdevice_shutdown(void)
{
}

int fsdevice_attach(unsigned int device, unsigned int drive, const char *name)
{
    return 0;
}

void fsdevice_set_directory(char *filename, unsigned int unit)
{
}

int fsdevice_limit_namelength(vdrive_t *vdrive, uint8_t *name)
{
    return 0;
}

/*******************************************************************************
    diskimage
*******************************************************************************/

void disk_image_init(void)
{
}

const char *disk_image_fsimage_name_get(const disk_image_t *image)
{
    return NULL;
}

void disk_image_media_destroy(disk_image_t *image)
{
}

int disk_image_close(disk_image_t *image)
{
    return 0;
}

void disk_image_destroy(disk_image_t *image)
{
}

disk_image_t *disk_image_create(void)
{
    return NULL;
}

int disk_image_open(disk_image_t *image)
{
    return 0;
}

void disk_image_rawimage_driver_name_set(disk_image_t *image)
{
}

void disk_image_fsimage_name_set(disk_image_t *image, const char *name)
{
}

void disk_image_media_create(disk_image_t *image)
{
}

int disk_image_fsimage_create(const char *name, unsigned int type)
{
    return 0;
}

int disk_image_fsimage_create_dxm(const char *name, const char *dname, unsigned int type)
{
    return 0;
}

int disk_image_fsimage_create_dhd(const char *name, const char *dname, unsigned int type)
{
    return 0;
}

int disk_image_write_sector(disk_image_t *image, const uint8_t *buf, const disk_addr_t *dadr)
{
    return 0;
}

int disk_image_read_sector(const disk_image_t *image, uint8_t *buf, const disk_addr_t *dadr)
{
    return 0;
}

const char *disk_image_name_get(const disk_image_t *image)
{
    return NULL;
}

unsigned int disk_image_sector_per_track(unsigned int format, unsigned int track)
{
    return 0;
}

int disk_image_write_p64_image(const disk_image_t *image)
{
    return 0;
}

void disk_image_attach_log(const disk_image_t *image, signed int lognum, unsigned int unit, unsigned int drive)
{
}

void disk_image_detach_log(const disk_image_t *image, signed int lognum, unsigned int unit, unsigned int drive)
{
}

int disk_image_check_sector(const disk_image_t *image, unsigned int track, unsigned int sector)
{
    return 0;
}

void disk_image_name_set(disk_image_t *image, const char *name)
{
}

void *disk_image_fsimage_fd_get(const disk_image_t *image)
{
    return NULL;
}

void P64ImageDestroy(PP64Image Instance)
{
}

/*******************************************************************************
    c64bus
*******************************************************************************/

int machine_bus_lib_directory(unsigned int unit, const char *pattern, uint8_t **buf)
{
    return 0;
}

int machine_bus_lib_read_sector(unsigned int unit, unsigned int track, unsigned int sector, uint8_t *buf)
{
    return 0;
}

int machine_bus_lib_write_sector(unsigned int unit, unsigned int track, unsigned int sector, uint8_t *buf)
{
    return 0;
}

unsigned int machine_bus_device_type_get(unsigned int unit)
{
    return 0;
}

void machine_bus_status_truedrive_set(unsigned int unit, unsigned int enable)
{
}

void machine_bus_status_drivetype_set(unsigned int unit, unsigned int enable)
{
}

void machine_bus_status_virtualdevices_set(unsigned int unit, unsigned int enable)
{
}

void machine_bus_eof_callback_set(void (*func)(void))
{
}

void machine_bus_attention_callback_set(void (*func)(void))
{
}

void machine_bus_init_machine(void)
{
}

/*******************************************************************************
    iecbus
*******************************************************************************/

iecbus_t iecbus;
void (*iecbus_update_ports)(void) = NULL;

void iecbus_status_set(unsigned int type, unsigned int unit, unsigned int enable)
{
}

int iecbus_device_write(unsigned int unit, uint8_t data)
{
    return 0;
}

uint8_t iecbus_device_read(void)
{
    return 0;
}

/*******************************************************************************
    drive
*******************************************************************************/

diskunit_context_t *diskunit_context[NUM_DISK_UNITS];

void drive_setup_context(void)
{
}

void drive_cpu_early_init_all(void)
{
}

void drive_cpu_trigger_reset(unsigned int dnr)
{
}

void drive_shutdown(void)
{
}

int drive_image_detach(disk_image_t *image, unsigned int unit, unsigned int drive)
{
    return 0;
}

int drive_image_attach(disk_image_t *image, unsigned int unit, unsigned int drive)
{
    return 0;
}

void drive_set_last_read(unsigned int track, unsigned int sector, uint8_t *buffer, struct diskunit_context_s *drv)
{
}

void drive_set_disk_memory(uint8_t *id, unsigned int track, unsigned int sector, struct diskunit_context_s *drv)
{
}

void drive_cpu_execute_one(diskunit_context_t *drv, CLOCK clk_value)
{
}

void drive_cpu_execute_all(CLOCK clk_value)
{
}

int drive_num_leds(unsigned int dnr)
{
    return 1;
}

int drive_check_type(unsigned int drive_type, unsigned int dnr)
{
    return 0;
}

int drive_check_extend_policy(int drive_type)
{
    return 0;
}

int drive_check_idle_method(int drive_type)
{
    return 0;
}

int drive_check_parallel_cable(int drive_type)
{
    return 0;
}

int drive_get_disk_drive_type(int dnr)
{
    return 0;
}

int drive_is_dualdrive_by_devnr(int devnr)
{
    return 0;
}

int drive_get_type_by_devnr(int devnr)
{
    return 0;
}

/*******************************************************************************
    vdrive
*******************************************************************************/

void vdrive_init(void)
{
}

int vdrive_device_setup(vdrive_t *vdrive, unsigned int unit)
{
    return 0;
}

void vdrive_device_shutdown(vdrive_t *vdrive)
{
}

int vdrive_iec_attach(unsigned int unit, const char *name)
{
    return 0;
}

int vdrive_bam_get_disk_id(unsigned int unit, unsigned int drive, uint8_t *id)
{
    return 0;
}

int vdrive_bam_set_disk_id(unsigned int unit, unsigned int drive, uint8_t *id)
{
    return 0;
}

void vdrive_detach_image(disk_image_t *image, unsigned int unit, unsigned int drive, vdrive_t *vdrive)
{
}

int vdrive_attach_image(disk_image_t *image, unsigned int unit, unsigned int drive, vdrive_t *vdrive)
{
    return 0;
}

void vdrive_get_last_read(unsigned int *track, unsigned int *sector, uint8_t **buffer)
{
}

int vdrive_internal_create_format_disk_image(const char *filename, const char *diskname, unsigned int type)
{
    return 0;
}

int vdrive_iec_close(vdrive_t *vdrive, unsigned int secondary)
{
    return 0;
}

int vdrive_iec_write(vdrive_t *vdrive, uint8_t data, unsigned int secondary)
{
    return 0;
}

int vdrive_iec_open(vdrive_t *vdrive, const uint8_t *name, unsigned int length, unsigned int secondary, cbmdos_cmd_parse_t *cmd_parse_ext)
{
    return 0;
}

int vdrive_iec_read(vdrive_t *vdrive, uint8_t *data, unsigned int secondary)
{
    return 0;
}

int vdrive_command_execute(vdrive_t *vdrive, const uint8_t *buf, unsigned int length)
{
    return 0;
}

int vdrive_ext_write_sector(vdrive_t *vdrive, int drive, const uint8_t *buf, unsigned int track, unsigned int sector)
{
    return 0;
}

int vdrive_ext_read_sector(vdrive_t *vdrive, int drive, uint8_t *buf, unsigned int track, unsigned int sector)
{
    return 0;
}

struct disk_image_s *vdrive_get_image(vdrive_t *vdrive, unsigned int drive)
{
    return NULL;
}

/*******************************************************************************
    c64 stuff
*******************************************************************************/

int tapeport_device_register(int id, tapeport_device_t *device)
{
    return 0;
}

void tapeport_trigger_flux_change(unsigned int on, int port)
{
}

void tapeport_set_tape_sense(int sense, int port)
{
}

int tape_seek_to_offset(tape_image_t *tape_image, unsigned long offset)
{
    return 0;
}

int tap_seek_to_offset(tap_t *tap, unsigned long offset)
{
    return 0;
}

int iec_available_busses(void)
{
    return 0;
}

int machine_get_num_keyboard_types(void)
{
    return 0;
}

kbdtype_info_t *machine_get_keyboard_info_list(void)
{
    return NULL;
}

void drive_cpu_trigger_reset_button(unsigned int dnr, unsigned int button)
{
}

int drive_has_buttons(unsigned int dnr)
{
    return 0;
}

int drive_image_type_to_drive_type(unsigned int type)
{
    return 0;
}

unsigned int drive_check_dual(unsigned int type)
{
    return 0;
}

unsigned int drive_get_num_heads(unsigned int type)
{
    return 0;
}

unsigned int drive_get_half_tracks(unsigned int type)
{
    return 0;
}

bool drive_is_jammed(int mynumber)
{
    return false;
}

char *drive_jam_reason(int mynumber)
{
    return NULL;
}

userport_desc_t *userport_get_valid_devices(int sort)
{
    return NULL;
}

const char *userport_get_device_type_desc(int type)
{
    return NULL;
}

#if !defined(USE_SDLUI) && !defined(USE_SDL2UI)
tapeport_desc_t *tapeport_get_valid_devices(int port, int sort)
{
    return NULL;
}

const char *tapeport_get_device_type_desc(int type)
{
    return NULL;
}
#endif

void userport_io_sim_set_pbx_out_lines(uint8_t val)
{
}

int userport_device_register(int id, userport_device_t *device)
{
    return -1;
}

bool pia1_get_diagnostic_pin(void)
{
    return false;
}

#ifdef HAVE_LIBCURL
const tzones_t *userport_wic64_get_timezones(size_t *num_zones)
{
    return NULL;
}

void userport_wic64_factory_reset(void)
{
}
#endif

/*******************************************************************************
    Sampler
*******************************************************************************/

sampler_device_t *sampler_get_devices(void)
{
    return NULL;
}

void sampler_stop(void)
{
}

void sampler_start(int channels, char *devname)
{
}

uint8_t sampler_get_sample(int channel)
{
    return 0;
}

/*******************************************************************************
 *  Printer                                                                    *
 ******************************************************************************/

const driver_select_list_t *driver_select_get_drivers(void)
{
    return NULL;
}

bool driver_select_is_printer(const char *drv_name)
{
    return false;
}

bool driver_select_is_plotter(const char *drv_name)
{
    return false;
}

bool driver_select_has_iec_bus(const char *drv_name)
{
    return false;
}

bool driver_select_has_ieee488_bus(const char *drv_name)
{
    return false;
}

bool driver_select_has_userport(const char *drv_name)
{
    return false;
}

bool driver_select_has_text_output(const char *drv_name)
{
    return false;
}

bool driver_select_has_graphics_output(const char *drv_name)
{
    return false;
}


/*******************************************************************************
    UI
*******************************************************************************/

int userport_get_device(void)
{
    return 0;
}
