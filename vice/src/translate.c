/*
 * translate.c - Global internationalization routines.
 *
 * Written by
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

#include "config.h"

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmdline.h"

#ifdef HAS_TRANSLATION
#include "intl.h"
#endif

#include "lib.h"
#include "log.h"
#include "resources.h"
#include "translate.h"
#include "util.h"

#define countof(array) (sizeof(array) / sizeof((array)[0]))

typedef struct translate_s {
    int resource_id;
    char *text;
} translate_t;

#ifdef HAS_TRANSLATION
char *current_language = NULL;
int current_language_index = 0;

/* The language table is usually duplicated in
   the arch intl.c, make sure they match
   when adding a new language */

static char *language_table[] = {

/* english */
  "en",

/* danish */
  "da",

/* german */
  "de",

/* french */
  "fr",

/* hungarian */
  "hu",

/* italian */
  "it",

/* dutch */
  "nl",

/* polish */
  "pl",

/* swedish */
  "sv",

/* turkish */
  "tr"
};


/* This is the codepage table, which holds the codepage
   used per language to encode the original text */


static int language_cp_table[] = {

/* english */
  28591,	/* ISO 8859-1 */

/* danish */
  28591,	/* ISO 8859-1 */

/* german */
  28591,	/* ISO 8859-1 */

/* french */
  28591,	/* ISO 8859-1 */

/* hungarian */
  28592,	/* ISO 8859-2 */

/* italian */
  28591,	/* ISO 8859-1 */

/* dutch */
  28591,	/* ISO 8859-1 */

/* polish */
  28592,	/* ISO 8859-2 */

/* swedish */
  28591,	/* ISO 8859-1 */

/* turkish */
  28599	/* ISO 8859-9 */
};
#endif

/* GLOBAL STRING ID TEXT TABLE */

translate_t string_table[] = {

/* autostart.c */
/* en */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE,    N_("Cannot load snapshot file.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_DA, "Kunne ikke indlæse snapshot-fil."},
/* de */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_DE, "Kann Snapshot Datei nicht laden."},
/* fr */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_FR, "Impossible de charger l'instantané."},
/* hu */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_HU, "Nem sikerült betölteni a pillanatkép fájlt."},
/* it */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_IT, "Non è possibile caricare il file di snapshot."},
/* nl */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_NL, "Kan momentopnamebestand niet laden."},
/* pl */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_SV, "Kan inte ladda ögonblicksbildfil."},
/* tr */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_TR, "Anlýk görüntü dosyasý yüklenemedi."},
#endif

/* debug.c */
/* en */ {IDGS_PLAYBACK_ERROR_DIFFERENT,    N_("Playback error: %s different from line %d of file debug%06d")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_PLAYBACK_ERROR_DIFFERENT_DA, "Afspilningsfejl: %s forskellig fra linje %d i filen debug%06d"},
/* de */ {IDGS_PLAYBACK_ERROR_DIFFERENT_DE, "Wiedergabe Fehler: %s ist unterschiedlich von Zeile %d in Datei debug%06d"},
/* fr */ {IDGS_PLAYBACK_ERROR_DIFFERENT_FR, "Erreur de lecture: %s différent de la ligne %d du fichier debug%06d"},
/* hu */ {IDGS_PLAYBACK_ERROR_DIFFERENT_HU, "Visszajátszási hiba: %s különbözik a %d. sorban a debug%06d fájl esetén"},
/* it */ {IDGS_PLAYBACK_ERROR_DIFFERENT_IT, "Errore di riproduzione: %s è differente dalla linea %d del file di debug "
                                            "%06d"},
/* nl */ {IDGS_PLAYBACK_ERROR_DIFFERENT_NL, "Afspeelfout: %s is anders dan regel %d van bestand debug%06d"},
/* pl */ {IDGS_PLAYBACK_ERROR_DIFFERENT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_PLAYBACK_ERROR_DIFFERENT_SV, "Återspelningsfel: %s inte lika som rad %d i filen debug%06d"},
/* tr */ {IDGS_PLAYBACK_ERROR_DIFFERENT_TR, "Kayýttan yürütme hatasý: %s dosyanýn %d satýrýndan farklý (hata "
                                            "ayýklama%06d)"},
#endif

/* event.c */
/* en */ {IDGS_CANNOT_CREATE_IMAGE,    N_("Cannot create image file!")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_CREATE_IMAGE_DA, "Kunne ikke oprette image-fil!"},
/* de */ {IDGS_CANNOT_CREATE_IMAGE_DE, "Kann Imagedatei nicht erzeugen!"},
/* fr */ {IDGS_CANNOT_CREATE_IMAGE_FR, "Impossible de créer le fichier image!"},
/* hu */ {IDGS_CANNOT_CREATE_IMAGE_HU, "Nem sikerült a képmás fájlt létrehozni!"},
/* it */ {IDGS_CANNOT_CREATE_IMAGE_IT, "Non è possibile creare il file immagine!"},
/* nl */ {IDGS_CANNOT_CREATE_IMAGE_NL, "Kan bestand niet maken!"},
/* pl */ {IDGS_CANNOT_CREATE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_CREATE_IMAGE_SV, "Kan inte skapa avbildningsfil!"},
/* tr */ {IDGS_CANNOT_CREATE_IMAGE_TR, "Imaj dosyasý yaratýlamadý!"},
#endif

/* event.c */
/* en */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S,    N_("Cannot write image file %s")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_DA, "Kunne ikks skrive image-fil %s"},
/* de */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_DE, "Kann Datei %s nicht schreiben"},
/* fr */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_FR, "Impossible d'écrire le fichier image %s"},
/* hu */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_HU, "Nem sikerült a %s képmás fájlba írni"},
/* it */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_IT, "Non è possibile scrivere l'immagine %s"},
/* nl */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_NL, "Kan niet schrijven naar bestand %s"},
/* pl */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_SV, "Kan inte skriva avbildningsfil %s"},
/* tr */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_TR, "%s imaj dosyasýna yazýlamadý"},
#endif

/* event.c */
/* en */ {IDGS_CANNOT_FIND_MAPPED_NAME_S,    N_("Cannot find mapped name for %s")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_DA, "Kunne ikke finde mappet navn for %s"},
/* de */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_DE, "Kann zugeordneten Namen für `%s' nicht finden."},
/* fr */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_FR, "Impossible de trouver le nom correspondant à %s"},
/* hu */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_HU, "A VICE %s leképzett nevét nem találja"},
/* it */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_IT, "Non è possibile trovare il nome mappato per %s"},
/* nl */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_NL, "Kan vertaalde naam voor %s niet vinden"},
/* pl */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_SV, "Kan inte kopplat namn för %s"},
/* tr */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_TR, "%s için eþleþme ismi bulunamadý"},
#endif

/* event.c */
/* en */ {IDGS_CANT_CREATE_START_SNAP_S,    N_("Could not create start snapshot file %s.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANT_CREATE_START_SNAP_S_DA, "Kunne ikke oprette start-snapshot-filen %s."},
/* de */ {IDGS_CANT_CREATE_START_SNAP_S_DE, "Kann Start Snapshot Datei nicht erzeugen: %s"},
/* fr */ {IDGS_CANT_CREATE_START_SNAP_S_FR, "Impossible de créer l'instantané de départ %s."},
/* hu */ {IDGS_CANT_CREATE_START_SNAP_S_HU, "Nem sikerült a %s kezdeti pillanatkép fájlt létrehozni."},
/* it */ {IDGS_CANT_CREATE_START_SNAP_S_IT, "Non è possibile creare il file di inizio snapshot %s."},
/* nl */ {IDGS_CANT_CREATE_START_SNAP_S_NL, "Kon het start momentopnamebestand %s niet maken."},
/* pl */ {IDGS_CANT_CREATE_START_SNAP_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANT_CREATE_START_SNAP_S_SV, "Kunde inte skapa startögonblicksbildfilen %s."},
/* tr */ {IDGS_CANT_CREATE_START_SNAP_S_TR, "Anlýk görüntü dosyasý %s yaratýlamadý."},
#endif

/* event.c */
/* en */ {IDGS_ERROR_READING_END_SNAP_S,    N_("Error reading end snapshot file %s.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_ERROR_READING_END_SNAP_S_DA, "Fejl under læsning af slut-snapshot %s."},
/* de */ {IDGS_ERROR_READING_END_SNAP_S_DE, "Kann Ende Snapshot Datei nicht lesen: %s"},
/* fr */ {IDGS_ERROR_READING_END_SNAP_S_FR, "Erreur de lecture de l'instantané de fin %s."},
/* hu */ {IDGS_ERROR_READING_END_SNAP_S_HU, "Hiba az utolsó %s pillanatkép fájl írásakor."},
/* it */ {IDGS_ERROR_READING_END_SNAP_S_IT, "Errore durante la lettura del file di fine snapshot %s."},
/* nl */ {IDGS_ERROR_READING_END_SNAP_S_NL, "Fout bij het lezen van het eind van het momentopnamebestand %s."},
/* pl */ {IDGS_ERROR_READING_END_SNAP_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ERROR_READING_END_SNAP_S_SV, "Fel vid läsning av slutögonblicksbildfilen %s."},
/* tr */ {IDGS_ERROR_READING_END_SNAP_S_TR, "Anlýk görüntü dosyasý %s okuma hatasý."},
#endif

/* event.c */
/* en */ {IDGS_CANT_CREATE_END_SNAP_S,    N_("Could not create end snapshot file %s.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANT_CREATE_END_SNAP_S_DA, "Kunne ikke oprette slut-snapshot-filen %s."},
/* de */ {IDGS_CANT_CREATE_END_SNAP_S_DE, "Kann Ende Snapshot Datei nicht erzeugen: %s"},
/* fr */ {IDGS_CANT_CREATE_END_SNAP_S_FR, "Impossible de créer l'instantané de fin %s."},
/* hu */ {IDGS_CANT_CREATE_END_SNAP_S_HU, "Nem sikerült a %s befejezõ pillanatkép fájlt létrehozni."},
/* it */ {IDGS_CANT_CREATE_END_SNAP_S_IT, "Non è possibile creare il file di fine snapshot %s."},
/* nl */ {IDGS_CANT_CREATE_END_SNAP_S_NL, "Kon het eind momentopnamebestand %s niet maken."},
/* pl */ {IDGS_CANT_CREATE_END_SNAP_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANT_CREATE_END_SNAP_S_SV, "Kunde inte skapa slutögonblicksbildfilen %s."},
/* tr */ {IDGS_CANT_CREATE_END_SNAP_S_TR, "Son anlýk görüntü dosyasý %s yaratýlamadý."},
#endif

/* event.c */
/* en */ {IDGS_CANT_OPEN_END_SNAP_S,    N_("Could not open end snapshot file %s.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANT_OPEN_END_SNAP_S_DA, "Kunne ikke åbne slut-snapshot-filen %s."},
/* de */ {IDGS_CANT_OPEN_END_SNAP_S_DE, "Kann Ende Snapshot Datei nicht öffnen: %s"},
/* fr */ {IDGS_CANT_OPEN_END_SNAP_S_FR, "Impossible d'ouvrir l'instantané de fin %s."},
/* hu */ {IDGS_CANT_OPEN_END_SNAP_S_HU, "Nem sikerült az utolsó %s pillanatkép fájlt megnyitni."},
/* it */ {IDGS_CANT_OPEN_END_SNAP_S_IT, "Non è possibile aprire il file di fine snapshot %s."},
/* nl */ {IDGS_CANT_OPEN_END_SNAP_S_NL, "Kon het eind momentopnamebestand %s niet openen."},
/* pl */ {IDGS_CANT_OPEN_END_SNAP_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANT_OPEN_END_SNAP_S_SV, "Kunde inte öppna slutögonblicksbildfilen %s."},
/* tr */ {IDGS_CANT_OPEN_END_SNAP_S_TR, "Son anlýk görüntü dosyasý %s açýlamadý."},
#endif

/* event.c */
/* en */ {IDGS_CANT_FIND_SECTION_END_SNAP,    N_("Could not find event section in end snapshot file.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANT_FIND_SECTION_END_SNAP_DA, "Kunne ikke finde hændelses-sektionen i slut-snapshot-filen."},
/* de */ {IDGS_CANT_FIND_SECTION_END_SNAP_DE, "Kann Sektion in Ende Snapshotdatei nicht finden."},
/* fr */ {IDGS_CANT_FIND_SECTION_END_SNAP_FR, "Impossible de trouver la section event dans l'instantané de fin."},
/* hu */ {IDGS_CANT_FIND_SECTION_END_SNAP_HU, "Nem találtam esemény részt az utolsó pillanatkép fájlban."},
/* it */ {IDGS_CANT_FIND_SECTION_END_SNAP_IT, "Non è possibile trovare la sezione eventi nel file di fine snapshot."},
/* nl */ {IDGS_CANT_FIND_SECTION_END_SNAP_NL, "Kon de gebeurtenis sectie in eind momentopnamebestand niet vinden."},
/* pl */ {IDGS_CANT_FIND_SECTION_END_SNAP_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANT_FIND_SECTION_END_SNAP_SV, "Kunde inte hinna händelsedelen i slutögonblicksbilden."},
/* tr */ {IDGS_CANT_FIND_SECTION_END_SNAP_TR, "Son anlýk görüntü dosyasýnda olay bölümü bulunamadý."},
#endif

/* event.c */
/* en */ {IDGS_ERROR_READING_START_SNAP_TRIED,    N_("Error reading start snapshot file. Tried %s and %s")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_ERROR_READING_START_SNAP_TRIED_DA, "Fejl under læsning af start-snapshot. Prøvede %s og %s"},
/* de */ {IDGS_ERROR_READING_START_SNAP_TRIED_DE, "Fehler beim Lesen der Start Snapshot Datei."
                                                  " Versuch gescheitert bei %s und %s."},
/* fr */ {IDGS_ERROR_READING_START_SNAP_TRIED_FR, "Erreur de lecture de l'instantané de départ. Essayé %s et %s"},
/* hu */ {IDGS_ERROR_READING_START_SNAP_TRIED_HU, "Nem sikerült az elsõ pillanatkép fájl olvasása. Kipróbálva: %s és %s"},
/* it */ {IDGS_ERROR_READING_START_SNAP_TRIED_IT, "Errore durante la lettura del file di inizio snapshot. Ho provato %s e "
                                                  "%s"},
/* nl */ {IDGS_ERROR_READING_START_SNAP_TRIED_NL, "Fout bij het lezen van het start momentopnamebestand. Heb %s en %s "
                                                  "geprobeerd"},
/* pl */ {IDGS_ERROR_READING_START_SNAP_TRIED_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ERROR_READING_START_SNAP_TRIED_SV, "Fel vid läsning av startögonblicksbildfil. Försökte med %s och %s"},
/* tr */ {IDGS_ERROR_READING_START_SNAP_TRIED_TR, "Baþlangýç anlýk görüntü dosyasý okuma hatasý. %s ve %s denendi"},
#endif

/* event.c */
/* en */ {IDGS_ERROR_READING_START_SNAP,    N_("Error reading start snapshot file.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_ERROR_READING_START_SNAP_DA, "Fejl under læsning af start-snapshot."},
/* de */ {IDGS_ERROR_READING_START_SNAP_DE, "Fehler beim Lesen der Start Snapshot Datei."},
/* fr */ {IDGS_ERROR_READING_START_SNAP_FR, "Erreur de lecture de l'instantané de départ."},
/* hu */ {IDGS_ERROR_READING_START_SNAP_HU, "Nem sikerült az indító pillanatkép fájl olvasása."},
/* it */ {IDGS_ERROR_READING_START_SNAP_IT, "Errore durante la lettura del file di inizio snapshot."},
/* nl */ {IDGS_ERROR_READING_START_SNAP_NL, "Fout bij het lezen van het start momentopnamebestand."},
/* pl */ {IDGS_ERROR_READING_START_SNAP_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ERROR_READING_START_SNAP_SV, "Fel vid läsning av startögonblicksbildfil."},
/* tr */ {IDGS_ERROR_READING_START_SNAP_TR, "Baþlangýç anlýk görüntü dosyasý okuma hatasý."},
#endif

/* screenshot.c */
/* en */ {IDGS_SORRY_NO_MULTI_RECORDING,    N_("Sorry. Multiple recording is not supported.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_SORRY_NO_MULTI_RECORDING_DA, "Kun en optagelse kan foretages ad gangen."},
/* de */ {IDGS_SORRY_NO_MULTI_RECORDING_DE, "Eine Aufnahme ist zur Zeit aktiv. Mehrfache Aufnahme ist nicht möglich."},
/* fr */ {IDGS_SORRY_NO_MULTI_RECORDING_FR, "Désolé. De multiples enregistrements ne sont pas supporté."},
/* hu */ {IDGS_SORRY_NO_MULTI_RECORDING_HU, "Sajnálom: többszörös felvétel nem támogatott."},
/* it */ {IDGS_SORRY_NO_MULTI_RECORDING_IT, "Le registrazioni multiple non sono supportate."},
/* nl */ {IDGS_SORRY_NO_MULTI_RECORDING_NL, "Sorry. Meerdere opnamen wordt niet ondersteund."},
/* pl */ {IDGS_SORRY_NO_MULTI_RECORDING_PL, ""},  /* fuzzy */
/* sv */ {IDGS_SORRY_NO_MULTI_RECORDING_SV, "Endast en inspelning kan göras åt gången."},
/* tr */ {IDGS_SORRY_NO_MULTI_RECORDING_TR, "Üzgünüz. Çoklu kayýt desteklenmiyor."},
#endif

/* sound.c */
/* en */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED,    N_("write to sound device failed.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_DA, "Kunne ikke skrive til lydenhed."},
/* de */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_DE, "Schreiben auf Gerät Sound ist fehlgeschlagen."},
/* fr */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_FR, "Impossible d'écriture sur le périphérique de son."},
/* hu */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_HU, "Nem sikerült a hangeszközre írni"},
/* it */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_IT, "scrittura sulla scheda audio fallita."},
/* nl */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_NL, "schrijfactie naar geluidsapparaat is mislukt."},
/* pl */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_PL, ""},  /* fuzzy */
/* sv */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_SV, "misslyckades att skriva till ljudenhet."},
/* tr */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_TR, "ses aygýtýna yazma baþarýsýz oldu."},
#endif

/* sound.c */
/* en */ {IDGS_CANNOT_OPEN_SID_ENGINE,    N_("Cannot open SID engine")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_OPEN_SID_ENGINE_DA, "Kunne ikke åbne SID-motor"},
/* de */ {IDGS_CANNOT_OPEN_SID_ENGINE_DE, "Kann SID Engine nicht öffnen"},
/* fr */ {IDGS_CANNOT_OPEN_SID_ENGINE_FR, "Erreur de chargement de l'engin de son SID"},
/* hu */ {IDGS_CANNOT_OPEN_SID_ENGINE_HU, "Nem sikerült a SID motort megnyitni"},
/* it */ {IDGS_CANNOT_OPEN_SID_ENGINE_IT, "Non è possibile aprire il motore SID"},
/* nl */ {IDGS_CANNOT_OPEN_SID_ENGINE_NL, "Kan de SID-kern niet openen"},
/* pl */ {IDGS_CANNOT_OPEN_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_OPEN_SID_ENGINE_SV, "Kan inte öppna SID-motor"},
/* tr */ {IDGS_CANNOT_OPEN_SID_ENGINE_TR, "SID motoru açýlamýyor"},
#endif

/* sound.c */
/* en */ {IDGS_CANNOT_INIT_SID_ENGINE,    N_("Cannot initialize SID engine")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_INIT_SID_ENGINE_DA, "Kunne ikke initialisere SID-motor"},
/* de */ {IDGS_CANNOT_INIT_SID_ENGINE_DE, "Kann SID Engine nicht initialisieren"},
/* fr */ {IDGS_CANNOT_INIT_SID_ENGINE_FR, "Erreur d'initialisation de l'engin de son SID"},
/* hu */ {IDGS_CANNOT_INIT_SID_ENGINE_HU, "Nem sikerült a SID motort elindítani"},
/* it */ {IDGS_CANNOT_INIT_SID_ENGINE_IT, "Non è possibile inizializzare il motore SID"},
/* nl */ {IDGS_CANNOT_INIT_SID_ENGINE_NL, "Kan de SID-kern niet initialiseren"},
/* pl */ {IDGS_CANNOT_INIT_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_INIT_SID_ENGINE_SV, "Kan inte initiera SID-motor"},
/* tr */ {IDGS_CANNOT_INIT_SID_ENGINE_TR, "SID motorunun baþlangýç ayarlarý yapýlamýyor"},
#endif

/* sound.c */
/* en */ {IDGS_INIT_FAILED_FOR_DEVICE_S,    N_("initialization failed for device `%s'.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_INIT_FAILED_FOR_DEVICE_S_DA, "Initialisering fejlede for enhed \"%s\"."},
/* de */ {IDGS_INIT_FAILED_FOR_DEVICE_S_DE, "Initialisierung von Gerät `%s' fehlgeschlagen."},
/* fr */ {IDGS_INIT_FAILED_FOR_DEVICE_S_FR, "erreur d'initialisation du périphérique `%s'."},
/* hu */ {IDGS_INIT_FAILED_FOR_DEVICE_S_HU, "A `%s' eszközt nem sikerült elindítani."},
/* it */ {IDGS_INIT_FAILED_FOR_DEVICE_S_IT, "inizializzazione fallita per il device `%s'."},
/* nl */ {IDGS_INIT_FAILED_FOR_DEVICE_S_NL, "initialisatie voor apparaat `%s' is mislukt."},
/* pl */ {IDGS_INIT_FAILED_FOR_DEVICE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_INIT_FAILED_FOR_DEVICE_S_SV, "initiering misslyckades för enhet \"%s\"."},
/* tr */ {IDGS_INIT_FAILED_FOR_DEVICE_S_TR, "`%s' aygýtý için baþlangýç ayarlarý baþarýsýz oldu."},
#endif

/* sound.c */
/* en */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT,    N_("device '%s' not found or not supported.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_DA, "enhed \"%s\" ikke fundet eller understøttet."},
/* de */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_DE, "Gerät '%s' konnte nicht gefunden werden oder ist nicht unterstützt."},
/* fr */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_FR, "périphérique '%s' non trouvé ou non supporté."},
/* hu */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_HU, "A '%s' eszköz nem létezik, vagy nem támogatott."},
/* it */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_IT, "il device '%s' non è stato trovato oppure non è supportato."},
/* nl */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_NL, "apparaat '%s' niet gevonden of wordt niet ondersteund"},
/* pl */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_SV, "enheten \"%s\" hittades eller stöds ej."},
/* tr */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_TR, "'%s' aygýtý yok ya da desteklenmiyor."},
#endif

/* sound.c */
/* en */ {IDGS_RECORD_DEVICE_S_NOT_EXIST,    N_("Recording device %s doesn't exist!")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_DA, "Optageenheden %s findes ikke!"},
/* de */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_DE, "Aufnahmegerät %s existiert nicht!"},
/* fr */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_FR, "Le périphérique d'enregistrement %s n'existe pas!"},
/* hu */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_HU, "A %s felvevõ eszköz nem létezik!"},
/* it */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_IT, "Il device di registrazione %s non esiste!"},
/* nl */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_NL, "Opnameapparaat %s bestaat niet!"},
/* pl */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_SV, "Inspelningsenhet %s finns inte!"},
/* tr */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_TR, "Kayýt aygýtý %s mevcut deðil!"},
#endif

/* sound.c */
/* en */ {IDGS_RECORD_DIFFERENT_PLAYBACK,    N_("Recording device must be different from playback device")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RECORD_DIFFERENT_PLAYBACK_DA, "Indspilningsenhed og optagelsesenhed skal være forskellige"},
/* de */ {IDGS_RECORD_DIFFERENT_PLAYBACK_DE, "Aufnahmegerät muß unteschiedlich vom Abspielgerät sein"},
/* fr */ {IDGS_RECORD_DIFFERENT_PLAYBACK_FR, "Le périphérique de son d'enregistrement doivent être différents"},
/* hu */ {IDGS_RECORD_DIFFERENT_PLAYBACK_HU, "A felvevõ és lejátszó eszközöknek különböznie kell"},
/* it */ {IDGS_RECORD_DIFFERENT_PLAYBACK_IT, "Il device di registrazione deve essere differente da quello di "
                                             "riproduzione"},
/* nl */ {IDGS_RECORD_DIFFERENT_PLAYBACK_NL, "Opnameapparaat moet anders zijn dan afspeelapparaat"},
/* pl */ {IDGS_RECORD_DIFFERENT_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RECORD_DIFFERENT_PLAYBACK_SV, "Inspelningsenhet och återspelningsenhet kan inte vara samma"},
/* tr */ {IDGS_RECORD_DIFFERENT_PLAYBACK_TR, "Kayýt aygýtý kayýttan yürütme aygýtý ile farklý olmak zorundadýr"},
#endif

/* sound.c */
/* en */ {IDGS_WARNING_RECORDING_REALTIME,    N_("Warning! Recording device %s seems to be a realtime device!")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_WARNING_RECORDING_REALTIME_DA, "Advarsel! Optageenheden %s synes at være en realtidsenhed!"},
/* de */ {IDGS_WARNING_RECORDING_REALTIME_DE, "Warnung! Aufnahmegerät %s scheint ein Echtzeitgerät zu sein!"},
/* fr */ {IDGS_WARNING_RECORDING_REALTIME_FR, "Attention! Le périphérique d'enregistrement %s semble être un "
                                              "périphérique en temps réel"},
/* hu */ {IDGS_WARNING_RECORDING_REALTIME_HU, "Figyelem! A %s felvevõ eszköz úgy tûnik, valós idejû eszköz!"},
/* it */ {IDGS_WARNING_RECORDING_REALTIME_IT, "Attenzione! Il device di registrazione %s sembra essere un dispositivo "
                                              "realtime!"},
/* nl */ {IDGS_WARNING_RECORDING_REALTIME_NL, "Waarschuwing! Opnameapparaat %s lijkt een realtime apparaat te zijn!"},
/* pl */ {IDGS_WARNING_RECORDING_REALTIME_PL, ""},  /* fuzzy */
/* sv */ {IDGS_WARNING_RECORDING_REALTIME_SV, "Varning! Inspelningsenheten %s verkar vara en realtidsenhet!"},
/* tr */ {IDGS_WARNING_RECORDING_REALTIME_TR, "Uyarý! Kayýt aygýtý %s gerçek zamanlý bir aygýt gibi görünüyor!"},
#endif

/* sound.c */
/* en */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR,    N_("The recording device doesn't support current sound parameters")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_DA, "Optageenheden understøtter ikke de valgte lydparametre"},
/* de */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_DE, "Aufnahmegerät unterstütz derzeitige Soundeinstellungen nicht"},
/* fr */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_FR, "Le périphérique d'enregistrement ne supporte pas les paramètres actuels"},
/* hu */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_HU, "A felvevõ eszköz nem támogatja az aktuális hangbeállításokat"},
/* it */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_IT, "Il device di registrazione non supporta i parametri attuali"},
/* nl */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_NL, "Het opname apparaat ondersteunt de huidige geluid opties niet"},
/* pl */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_SV, "Inspelningsenheten stöder inte aktuella ljudinställningar"},
/* tr */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_TR, "Kayýt aygýtý geçerli ses parametrelerini desteklemiyor"},
#endif

/* sound.c */
/* en */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE,    N_("Sound buffer overflow (cycle based)")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_DA, "Lydbuffer overløb (cykelbaseret)"},
/* de */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_DE, "Sound Puffer Überlauf (Zyklus basiert)"},
/* fr */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_FR, "Erreur de dépassement de tampon son (cycle based)"},
/* hu */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_HU, "Hangpuffer túlcsordulás (ciklus alapú)"},
/* it */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_IT, "Overflow del buffer sonoro (cycle based)"},
/* nl */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_NL, "Geluidsbuffer overloop (cyclus gebaseerd)"},
/* pl */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_SV, "För mycket data i ljudbufferten (cykelbaserad)"},
/* tr */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_TR, "Ses arabellek taþmasý (cycle bazlý)"},
#endif

/* sound.c */
/* en */ {IDGS_SOUND_BUFFER_OVERFLOW,    N_("Sound buffer overflow.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_SOUND_BUFFER_OVERFLOW_DA, "Lydbuffer overløb."},
/* de */ {IDGS_SOUND_BUFFER_OVERFLOW_DE, "Sound Puffer Überlauf."},
/* fr */ {IDGS_SOUND_BUFFER_OVERFLOW_FR, "Erreur de dépassement de tampon son."},
/* hu */ {IDGS_SOUND_BUFFER_OVERFLOW_HU, "Hangpuffer túlcsordulás."},
/* it */ {IDGS_SOUND_BUFFER_OVERFLOW_IT, "Overflow del buffer sonoro."},
/* nl */ {IDGS_SOUND_BUFFER_OVERFLOW_NL, "Geluidsbuffer overloop."},
/* pl */ {IDGS_SOUND_BUFFER_OVERFLOW_PL, ""},  /* fuzzy */
/* sv */ {IDGS_SOUND_BUFFER_OVERFLOW_SV, "För mycket data i ljudbufferten."},
/* tr */ {IDGS_SOUND_BUFFER_OVERFLOW_TR, "Ses arabellek taþmasý."},
#endif

/* sound.c */
/* en */ {IDGS_CANNOT_FLUSH,    N_("cannot flush.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_FLUSH_DA, "Kan ikke tømme."},
/* de */ {IDGS_CANNOT_FLUSH_DE, "Entleerung nicht möglich."},
/* fr */ {IDGS_CANNOT_FLUSH_FR, "impossible de vider."},
/* hu */ {IDGS_CANNOT_FLUSH_HU, "nem sikerült üríteni a puffert."},
/* it */ {IDGS_CANNOT_FLUSH_IT, "non è possibile svuotare."},
/* nl */ {IDGS_CANNOT_FLUSH_NL, "kan niet spoelen."},
/* pl */ {IDGS_CANNOT_FLUSH_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_FLUSH_SV, "kan inte tömma."},
/* tr */ {IDGS_CANNOT_FLUSH_TR, "temizlenemedi."},
#endif

/* sound.c */
/* en */ {IDGS_FRAGMENT_PROBLEMS,    N_("fragment problems.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_FRAGMENT_PROBLEMS_DA, "Fragmentproblem."},
/* de */ {IDGS_FRAGMENT_PROBLEMS_DE, "Fragmentierungsproblem."},
/* fr */ {IDGS_FRAGMENT_PROBLEMS_FR, "problèmes de fragments."},
/* hu */ {IDGS_FRAGMENT_PROBLEMS_HU, "töredékrész problémák."},
/* it */ {IDGS_FRAGMENT_PROBLEMS_IT, "problemi di frammentazione."},
/* nl */ {IDGS_FRAGMENT_PROBLEMS_NL, "fragment problemen."},
/* pl */ {IDGS_FRAGMENT_PROBLEMS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_FRAGMENT_PROBLEMS_SV, "fragmentprogram."},
/* tr */ {IDGS_FRAGMENT_PROBLEMS_TR, "parçalara ayýrma problemi."},
#endif

/* sound.c */
/* en */ {IDGS_RUNNING_TOO_SLOW,    N_("running too slow.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RUNNING_TOO_SLOW_DA, "Kører for langsomt."},
/* de */ {IDGS_RUNNING_TOO_SLOW_DE, "Ablauf zu langsam."},
/* fr */ {IDGS_RUNNING_TOO_SLOW_FR, "l'exécution est trop lente."},
/* hu */ {IDGS_RUNNING_TOO_SLOW_HU, "túl lassan megy."},
/* it */ {IDGS_RUNNING_TOO_SLOW_IT, "esecuzione troppo lenta."},
/* nl */ {IDGS_RUNNING_TOO_SLOW_NL, "draait te langzaam."},
/* pl */ {IDGS_RUNNING_TOO_SLOW_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RUNNING_TOO_SLOW_SV, "går för långsamt."},
/* tr */ {IDGS_RUNNING_TOO_SLOW_TR, "çok yavaþ çalýþýyor."},
#endif

/* sound.c */
/* en */ {IDGS_STORE_SOUNDDEVICE_FAILED,    N_("store to sounddevice failed.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_STORE_SOUNDDEVICE_FAILED_DA, "skrivning til lydenhed fejlede."},
/* de */ {IDGS_STORE_SOUNDDEVICE_FAILED_DE, "Speichern auf Sound Gerät ist fehlgeschlagen."},
/* fr */ {IDGS_STORE_SOUNDDEVICE_FAILED_FR, "erreur d'enregistrement sur le périphérique de son."},
/* hu */ {IDGS_STORE_SOUNDDEVICE_FAILED_HU, "a hangeszközön történõ tárolás nem sikerült."},
/* it */ {IDGS_STORE_SOUNDDEVICE_FAILED_IT, "memorizzazione sulla scheda audio fallita."},
/* nl */ {IDGS_STORE_SOUNDDEVICE_FAILED_NL, "opslag naar geluidsapparaat is mislukt."},
/* pl */ {IDGS_STORE_SOUNDDEVICE_FAILED_PL, ""},  /* fuzzy */
/* sv */ {IDGS_STORE_SOUNDDEVICE_FAILED_SV, "misslyckades spara i ljudenheten."},
/* tr */ {IDGS_STORE_SOUNDDEVICE_FAILED_TR, "ses aygýtýna saklama baþarýsýz oldu."},
#endif

/* c64/c64export.c */
/* en */ {IDGS_RESOURCE_S_BLOCKED_BY_S,    N_("Resource %s blocked by %s.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RESOURCE_S_BLOCKED_BY_S_DA, "Resoursen %s er blokeret af %s."},
/* de */ {IDGS_RESOURCE_S_BLOCKED_BY_S_DE, "Resource %s wird von %s blockiert."},
/* fr */ {IDGS_RESOURCE_S_BLOCKED_BY_S_FR, "La ressource %s est bloquée par %s."},
/* hu */ {IDGS_RESOURCE_S_BLOCKED_BY_S_HU, "A %s erõforrást %s fogja."},
/* it */ {IDGS_RESOURCE_S_BLOCKED_BY_S_IT, "Risorsa %s bloccata da %s."},
/* nl */ {IDGS_RESOURCE_S_BLOCKED_BY_S_NL, "Bron %s geblokeerd door %s."},
/* pl */ {IDGS_RESOURCE_S_BLOCKED_BY_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RESOURCE_S_BLOCKED_BY_S_SV, "Resursen %s blockeras av %s."},
/* tr */ {IDGS_RESOURCE_S_BLOCKED_BY_S_TR, "Kaynak %s, %s tarafýndan bloklandý."},
#endif

#ifdef HAVE_NETWORK
/* network.c */
/* en */ {IDGS_TESTING_BEST_FRAME_DELAY,    N_("Testing best frame delay...")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_TESTING_BEST_FRAME_DELAY_DA, "Tester bedste billed-forsinkelse..."},
/* de */ {IDGS_TESTING_BEST_FRAME_DELAY_DE, "Teste optimale Framerate..."},
/* fr */ {IDGS_TESTING_BEST_FRAME_DELAY_FR, "Recherche du meilleur délai d'image..."},
/* hu */ {IDGS_TESTING_BEST_FRAME_DELAY_HU, "Legjobb képkocka késleltetés tesztelése..."},
/* it */ {IDGS_TESTING_BEST_FRAME_DELAY_IT, "Rileva il miglior ritardo tra frame..."},
/* nl */ {IDGS_TESTING_BEST_FRAME_DELAY_NL, "Beste framevertraging aan het testen..."},
/* pl */ {IDGS_TESTING_BEST_FRAME_DELAY_PL, ""},  /* fuzzy */
/* sv */ {IDGS_TESTING_BEST_FRAME_DELAY_SV, "Testar bästa ramfördröjning..."},
/* tr */ {IDGS_TESTING_BEST_FRAME_DELAY_TR, "En iyi frame bekleme süresi test ediliyor..."},
#endif

/* network.c */
/* en */ {IDGS_USING_D_FRAMES_DELAY,    N_("Using %d frames delay.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_USING_D_FRAMES_DELAY_DA, "Bruger %d billeders forsinkelse."},
/* de */ {IDGS_USING_D_FRAMES_DELAY_DE, "Benutze %d Frameverzögerung."},
/* fr */ {IDGS_USING_D_FRAMES_DELAY_FR, "Utilisation d'un délai de %d images."},
/* hu */ {IDGS_USING_D_FRAMES_DELAY_HU, "%d képkocka késleltetés használata."},
/* it */ {IDGS_USING_D_FRAMES_DELAY_IT, "Utilizzo %d frame di ritardo."},
/* nl */ {IDGS_USING_D_FRAMES_DELAY_NL, "%d frames vertraging in gebruik."},
/* pl */ {IDGS_USING_D_FRAMES_DELAY_PL, ""},  /* fuzzy */
/* sv */ {IDGS_USING_D_FRAMES_DELAY_SV, "Använder %d ramars fördröjning."},
/* tr */ {IDGS_USING_D_FRAMES_DELAY_TR, "%d frame bekleme süresi kullanýlýyor."},
#endif

/* network.c */
/* en */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER,    N_("Cannot load snapshot file for transfer")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_DA, "Kunne ikke indlæse snapshot til overførsel."},
/* de */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_DE, "Kann Snapshot Datei für Transfer nicht laden"},
/* fr */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_FR, "Impossible de charger l'instantané pour le transfer"},
/* hu */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_HU, "Nem sikerült az átküldendõ pillanatkép fájlt betölteni"},
/* it */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_IT, "Non à possibile caricare il file di snapshot per il trasferimento"},
/* nl */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_NL, "Kan het momentopnamebestand voor versturen niet laden"},
/* pl */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_SV, "Kan inte läsa in ögonblicksbildfil för överföring."},
/* tr */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_TR, "Transfer için anlýk görüntü dosyasý yüklenemedi"},
#endif

/* network.c */
/* en */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT,    N_("Sending snapshot to client...")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_DA, "Sender snapshot til klient..."},
/* de */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_DE, "Sende Snapshot zu Client..."},
/* fr */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_FR, "Envoi de l'instantané au client..."},
/* hu */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_HU, "Pillanatkép küldése a kliensnek..."},
/* it */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_IT, "Invio dell'immagine dello snapshot al client in corso..."},
/* nl */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_NL, "Momentopname wordt naar de andere computer verstuurd..."},
/* pl */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_SV, "Sänder ögonblicksbild till klient..."},
/* tr */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_TR, "Anlýk görüntü dosyasý istemciye gönderiliyor..."},
#endif

/* network.c */
/* en */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT,    N_("Cannot send snapshot to client")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_DA, "Kunne ikke sende snapshot til klient"},
/* de */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_DE, "Kann Snapshot Datei nicht senden"},
/* fr */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_FR, "Impossible d'envoyer l'instantané au client"},
/* hu */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_HU, "Nem sikerült pillanatképet elküldeni a klienshez"},
/* it */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_IT, "Non à possibile inviare lo snapshot al client"},
/* nl */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_NL, "Kan de momentopname niet naar de andere computer versturen"},
/* pl */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_SV, "Kan inte sända ögonblicksbild till klient"},
/* tr */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_TR, "Anlýk görüntü dosyasý istemciye gönderilemedi"},
#endif

/* network.c */
/* en */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S,    N_("Cannot create snapshot file %s")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_DA, "Kunne ikke oprette snapshot-fil %s"},
/* de */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_DE, "Kann Snapshot Datei `%s' nicht erzeugen"},
/* fr */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_FR, "Impossible de créer l'instantané %s"},
/* hu */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_HU, "Nem sikerült a pillanatképfájlt létrehozni (%s)"},
/* it */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_IT, "Non à possibile creare il file di snapshot %s"},
/* nl */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_NL, "Kan het momentopnamebestand %s niet maken"},
/* pl */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_SV, "Kan inte skapa ögonblicksbildfil %s"},
/* tr */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_TR, "Anlýk görüntü dosyasý %s yaratýlamadý"},
#endif

/* network.c */
/* en */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S,    N_("Cannot open snapshot file %s")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_DA, "Kunne ikke åbne snapshot-fil %s"},
/* de */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_DE, "Kann Snapshot Datei `%s' nicht öffnen"},
/* fr */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_FR, "Impossible d'ouvrir le fichier %s"},
/* hu */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_HU, "Nem sikerült a pillanatképfájlt megnyitni (%s)"},
/* it */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_IT, "Non à possibile aprire il file di snapshot %s"},
/* nl */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_NL, "Kan het momentopnamebestand %s niet openen"},
/* pl */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_SV, "Kan inte öppna ögonblicksbildfil %s"},
/* tr */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_TR, "Anlýk görüntü dosyasý %s açýlamadý"},
#endif

/* network.c */
/* en */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT,    N_("Server is waiting for a client...")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_DA, "Server venter på en klient..."},
/* de */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_DE, "Server wartet auf Client..."},
/* fr */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_FR, "Le serveur attend la connexion d'un client..."},
/* hu */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_HU, "A kiszolgáló kliensre vár..."},
/* it */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_IT, "Il server à in attesa di un client..."},
/* nl */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_NL, "De server wacht op de andere computer..."},
/* pl */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_SV, "Servern väntar på en klient..."},
/* tr */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_TR, "Sunucu bir istemcinin baðlanmasý için bekliyor..."},
#endif

/* network.c */
/* en */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT,    N_("Cannot create snapshot file. Select different history directory!")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_DA, "Kunne ikke oprette snapshot-fil. Vælg et andet historik-katalog!"},
/* de */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_DE, "Kann Snapshot Datei nicht erzeugen. Verwende ein anderes "
                                                   "Verzeichnis!"},
/* fr */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_FR, "Impossible de créer le fichier d'instantané. Sélectionnez un "
                                                   "répertoire d'historique différent"},
/* hu */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_HU, "Nem sikerült pillanatkép fájlt létrehozni. Kérem válasszon más "
                                                   "könyvtárat!"},
/* it */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_IT, "Non è possibile creare il file di snapshot. Seleziona una directory "
                                                   "diversa per la cronologia."},
/* nl */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_NL, "Kan het momentopnamebestand niet maken. Selecteer een andere "
                                                   "geschiedenisdirectory!"},
/* pl */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_SV, "Kan inte skapa ögonblicksbildfil. Välj en annan historikkatalog!"},
/* tr */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_TR, "Anlýk görüntü dosyasý yaratýlamadý. Farklý bir tarihçe dizini seçin!"},
#endif

/* network.c */
/* en */ {IDGS_CANNOT_RESOLVE_S,    N_("Cannot resolve %s")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_RESOLVE_S_DA, "Kan ikke opslå %s"},
/* de */ {IDGS_CANNOT_RESOLVE_S_DE, "Kann %s nicht auflösen"},
/* fr */ {IDGS_CANNOT_RESOLVE_S_FR, "Impossible de résoudre %s"},
/* hu */ {IDGS_CANNOT_RESOLVE_S_HU, "Nem sikerült %s-t feloldani"},
/* it */ {IDGS_CANNOT_RESOLVE_S_IT, "Non à possibile risolvere %s"},
/* nl */ {IDGS_CANNOT_RESOLVE_S_NL, "Onbekende host %s"},
/* pl */ {IDGS_CANNOT_RESOLVE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_RESOLVE_S_SV, "Kan inte slå upp %s"},
/* tr */ {IDGS_CANNOT_RESOLVE_S_TR, "%s çözülemedi"},
#endif

/* network.c */
/* en */ {IDGS_CANNOT_CONNECT_TO_S,    N_("Cannot connect to %s (no server running on port %d).")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_CONNECT_TO_S_DA, "Kan ikke forbinde til %s (ingen server på port %d)."},
/* de */ {IDGS_CANNOT_CONNECT_TO_S_DE, "Kann zu %s nicht verbinden (Kein Server aktiv auf Port %d)."},
/* fr */ {IDGS_CANNOT_CONNECT_TO_S_FR, "Impossible de se connecter à %s (aucun serveur sur le port %d)."},
/* hu */ {IDGS_CANNOT_CONNECT_TO_S_HU, "Nem sikerült %s-hez kapcsolódni (a %d porton nem fut kiszolgáló)"},
/* it */ {IDGS_CANNOT_CONNECT_TO_S_IT, "Non è possibile connettersi a %s (nessun server è attivo sulla porta %d)."},
/* nl */ {IDGS_CANNOT_CONNECT_TO_S_NL, "Kan geen verbinding maken met %s (er is geen server aanwezig op poort %d)."},
/* pl */ {IDGS_CANNOT_CONNECT_TO_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_CONNECT_TO_S_SV, "Kan inte ansluta till %s (ingen server igång på port %d)."},
/* tr */ {IDGS_CANNOT_CONNECT_TO_S_TR, "%s baðlantýsý kurulamadý (port %d üzerinde çalýþan sunucu yok)."},
#endif

/* network.c */
/* en */ {IDGS_RECEIVING_SNAPSHOT_SERVER,    N_("Receiving snapshot from server...")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RECEIVING_SNAPSHOT_SERVER_DA, "Modtager snapshot fra server..."},
/* de */ {IDGS_RECEIVING_SNAPSHOT_SERVER_DE, "Empfange Snapshot von Server..."},
/* fr */ {IDGS_RECEIVING_SNAPSHOT_SERVER_FR, "Réception de l'instantané du serveur..."},
/* hu */ {IDGS_RECEIVING_SNAPSHOT_SERVER_HU, "Pillanatkép fogadása a szerverrõl..."},
/* it */ {IDGS_RECEIVING_SNAPSHOT_SERVER_IT, "Ricezione dello snapshot dal server in corso..."},
/* nl */ {IDGS_RECEIVING_SNAPSHOT_SERVER_NL, "Momentopname van de server wordt ontvangen..."},
/* pl */ {IDGS_RECEIVING_SNAPSHOT_SERVER_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RECEIVING_SNAPSHOT_SERVER_SV, "Tar emot ögoblicksbild från server..."},
/* tr */ {IDGS_RECEIVING_SNAPSHOT_SERVER_TR, "Sunucudan anlýk görüntü alýnýyor..."},
#endif

/* network.c */
/* en */ {IDGS_NETWORK_OUT_OF_SYNC,    N_("Network out of sync - disconnecting.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_NETWORK_OUT_OF_SYNC_DA, "Netværk ude af synk - afbryder forbindelsen."},
/* de */ {IDGS_NETWORK_OUT_OF_SYNC_DE, "Netzwerksynchronisationsproblem - Trenne Verbindung."},
/* fr */ {IDGS_NETWORK_OUT_OF_SYNC_FR, "Le réseau n'est pas synchronisé (out of sync error). Déconnexion."},
/* hu */ {IDGS_NETWORK_OUT_OF_SYNC_HU, "A hálózat kiesett a szinkronból - szétkapcsolódás"},
/* it */ {IDGS_NETWORK_OUT_OF_SYNC_IT, "Rete non sincronizzata - disconnesione in corso."},
/* nl */ {IDGS_NETWORK_OUT_OF_SYNC_NL, "Netwerk niet synchroon - verbinding wordt verbroken."},
/* pl */ {IDGS_NETWORK_OUT_OF_SYNC_PL, ""},  /* fuzzy */
/* sv */ {IDGS_NETWORK_OUT_OF_SYNC_SV, "Nätverket ur synk - kopplar från."},
/* tr */ {IDGS_NETWORK_OUT_OF_SYNC_TR, "Að senkronizasyonu bozuldu - baðlantý kesiliyor."},
#endif

/* network.c */
/* en */ {IDGS_REMOTE_HOST_DISCONNECTED,    N_("Remote host disconnected.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_REMOTE_HOST_DISCONNECTED_DA, "Fjernmaskine afbrudt."},
/* de */ {IDGS_REMOTE_HOST_DISCONNECTED_DE, "Entfernter Rechner getrennt."},
/* fr */ {IDGS_REMOTE_HOST_DISCONNECTED_FR, "L'hôte distant s'est déconnecté."},
/* hu */ {IDGS_REMOTE_HOST_DISCONNECTED_HU, "A távoli számítogép lekapcsolódott."},
/* it */ {IDGS_REMOTE_HOST_DISCONNECTED_IT, "Disconnesso dall'host remoto."},
/* nl */ {IDGS_REMOTE_HOST_DISCONNECTED_NL, "Andere computer heeft de verbinding verbroken."},
/* pl */ {IDGS_REMOTE_HOST_DISCONNECTED_PL, ""},  /* fuzzy */
/* sv */ {IDGS_REMOTE_HOST_DISCONNECTED_SV, "Frånkopplad från fjärrvärd."},
/* tr */ {IDGS_REMOTE_HOST_DISCONNECTED_TR, "Uzak sunucu baðlantýsý kesildi."},
#endif

/* network.c */
/* en */ {IDGS_REMOTE_HOST_SUSPENDING,    N_("Remote host suspending...")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_REMOTE_HOST_SUSPENDING_DA, "Fjernmaskine pauser..."},
/* de */ {IDGS_REMOTE_HOST_SUSPENDING_DE, "Entfernter Rechner angehaltedn..."},
/* fr */ {IDGS_REMOTE_HOST_SUSPENDING_FR, "L'hôte distant à suspendu..."},
/* hu */ {IDGS_REMOTE_HOST_SUSPENDING_HU, "A távoli számítógép felfüggesztést kért..."},
/* it */ {IDGS_REMOTE_HOST_SUSPENDING_IT, "Sospesione dell'host remoto in corso..."},
/* nl */ {IDGS_REMOTE_HOST_SUSPENDING_NL, "Andere computer halt de netplay..."},
/* pl */ {IDGS_REMOTE_HOST_SUSPENDING_PL, ""},  /* fuzzy */
/* sv */ {IDGS_REMOTE_HOST_SUSPENDING_SV, "FJärrvärden pausar..."},
/* tr */ {IDGS_REMOTE_HOST_SUSPENDING_TR, "Uzak sunucu askýda..."},
#endif

#ifdef HAVE_IPV6
/* network.c */
/* en */ {IDGS_CANNOT_SWITCH_IPV4_IPV6,    N_("Cannot switch IPV4/IPV6 while netplay is active.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_DA, "Kan ikke skifte mellem IPV4/IPV6 mens netværksspil er igang."},
/* de */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_DE, "Kann zwischen IPV4/IPV6 nicht wechseln, solange netplay aktiv ist."},
/* fr */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_FR, "Ne peut alterner IPV4/IPV6 pendant que netplay est actif."},
/* hu */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_HU, "Nem válthat IPV4/IPV6 között, amíg a hálózati játék aktív"},
/* it */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_IT, "Non à possibile commutare tra IPV4 e IPV6 mentre netplay à attivo."},
/* nl */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_NL, "Kan niet schakelen tussen IPV4/IPV6 omdat netplay actief is."},
/* pl */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_SV, "Kan inte växla mellan IPV4/IPV6 medan nätverksspel är aktivt."},
/* tr */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_TR, "Netplay aktifken IPV4/IPV6 arasý geçiþ yapýlamaz."},
#endif
#endif
#endif

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM,    N_("ffmpegdrv: Cannot open video stream")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_DA, "ffmpegdrv: Kan ikke åbne video-strøm"},
/* de */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_DE, "ffmpegdrv: Kann video stream nicht öffnen"},
/* fr */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_FR, "ffmpegdrv: Impossible d'ouvrir le flux vidéo"},
/* hu */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_HU, "ffmpegdrv: Nem sikerült megnyitni a videó adatfolyamot"},
/* it */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_IT, "ffmpegdrv: Non à possibile aprire il flusso video"},
/* nl */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_NL, "ffmpegdrv: Kan de videostroom niet openen"},
/* pl */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_PL, ""},  /* fuzzy */
/* sv */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_SV, "ffmpegdrv: Kan inte öppna videoström"},
/* tr */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_TR, "ffmpegdrv: Görüntü akýþý açýlamadý"},
#endif

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM,    N_("ffmpegdrv: Cannot open audio stream")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_DA, "ffmpegdrv: Kan ikke åbne lyd-strøm"},
/* de */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_DE, "ffmpegdrv: Kann audio stream nicht öffnen"},
/* fr */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_FR, "ffmpegdrv: Impossible d'ouvrir le flux audio"},
/* hu */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_HU, "ffmpegdrv: Nem sikerült megnyitni a audió adatfolyamot"},
/* it */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_IT, "ffmpegdrv: Non à possibile aprire il flusso audio"},
/* nl */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_NL, "ffmpegdrv: Kan de audiostroom niet openen"},
/* pl */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_PL, ""},  /* fuzzy */
/* sv */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_SV, "ffmpegdrv: Kan inte öppna ljudström"},
/* tr */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_TR, "ffmpegdrv: Ses akýþý açýlamadý"},
#endif

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDGS_FFMPEG_CANNOT_OPEN_S,    N_("ffmpegdrv: Cannot open %s")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_FFMPEG_CANNOT_OPEN_S_DA, "ffmpegdrv: Kan ikke åbne %s"},
/* de */ {IDGS_FFMPEG_CANNOT_OPEN_S_DE, "ffmpegdrv: Öffnen von %s fehlgeschlagen"},
/* fr */ {IDGS_FFMPEG_CANNOT_OPEN_S_FR, "ffmpegdrv: Impossible d'ouvrir %s"},
/* hu */ {IDGS_FFMPEG_CANNOT_OPEN_S_HU, "ffmpegdrv: Nem sikerült megnyitni: %s"},
/* it */ {IDGS_FFMPEG_CANNOT_OPEN_S_IT, "ffmpegdrv: Non à possibile aprire %s"},
/* nl */ {IDGS_FFMPEG_CANNOT_OPEN_S_NL, "ffmpegdrv: Kan %s niet openen"},
/* pl */ {IDGS_FFMPEG_CANNOT_OPEN_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_FFMPEG_CANNOT_OPEN_S_SV, "ffmpegdrv: Kan inte öppna %s"},
/* tr */ {IDGS_FFMPEG_CANNOT_OPEN_S_TR, "ffmpegdrv: %s açýlamadý"},
#endif

/* c64/c64io.c */
/* en */ {IDGS_IO_READ_COLL_AT_X_FROM,    N_("I/O read collision at %X from ")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_IO_READ_COLL_AT_X_FROM_DA, "I/O-læsekollision ved %X fra "},
/* de */ {IDGS_IO_READ_COLL_AT_X_FROM_DE, "I/O Lese Kollision bei %X von "},
/* fr */ {IDGS_IO_READ_COLL_AT_X_FROM_FR, "Collision lors de la lecture E/S at %X à partir de "},
/* hu */ {IDGS_IO_READ_COLL_AT_X_FROM_HU, "I/O olvasás ütközés %X-nél, hely: "},
/* it */ {IDGS_IO_READ_COLL_AT_X_FROM_IT, "Collisione di I/O in lettura a %X da "},
/* nl */ {IDGS_IO_READ_COLL_AT_X_FROM_NL, "I/O lees botsing op %X van "},
/* pl */ {IDGS_IO_READ_COLL_AT_X_FROM_PL, ""},  /* fuzzy */
/* sv */ {IDGS_IO_READ_COLL_AT_X_FROM_SV, "I/O-läskollision vid %X från "},
/* tr */ {IDGS_IO_READ_COLL_AT_X_FROM_TR, "G/Ç okuma çakýþmasý %X'de "},
#endif

/* c64/c64io.c */
/* en */ {IDGS_AND,    N_(" and ")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_AND_DA, " og "},
/* de */ {IDGS_AND_DE, " und "},
/* fr */ {IDGS_AND_FR, " et "},
/* hu */ {IDGS_AND_HU, " és "},
/* it */ {IDGS_AND_IT, " e "},
/* nl */ {IDGS_AND_NL, " en "},
/* pl */ {IDGS_AND_PL, ""},  /* fuzzy */
/* sv */ {IDGS_AND_SV, " och "},
/* tr */ {IDGS_AND_TR, " ve "},
#endif

/* c64/c64io.c */
/* en */ {IDGS_ALL_DEVICES_DETACHED,    N_(".\nAll the named devices will be detached.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_ALL_DEVICES_DETACHED_DA, ".\nAlle de navngivne enheder vil blive frakoblet."},
/* de */ {IDGS_ALL_DEVICES_DETACHED_DE, ".\nAlle genannten Geräte werden entfernt."},
/* fr */ {IDGS_ALL_DEVICES_DETACHED_FR, ".\nTous les périphériques nommés vont être détachés."},
/* hu */ {IDGS_ALL_DEVICES_DETACHED_HU, ".\nMinden megnevezett eszköz le lesz választva."},
/* it */ {IDGS_ALL_DEVICES_DETACHED_IT, ".\nTutti i device specificati sarranno rimossi."},
/* nl */ {IDGS_ALL_DEVICES_DETACHED_NL, ".\nAlle genoemde apparaten zullen worden ontkoppelt."},
/* pl */ {IDGS_ALL_DEVICES_DETACHED_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ALL_DEVICES_DETACHED_SV, ".\nAlla namngivna enheter kommer kopplas från."},
/* tr */ {IDGS_ALL_DEVICES_DETACHED_TR, ".\nTüm isimlendirilmiþ aygýtlar çýkartýlacaktýr."},
#endif

/* c64/digimax.c */
/* en */ {IDGS_DIGIMAX_NOT_WITH_RESID,    N_("Digimax cannot be used with ReSID\nPlease switch SID Engine to FastSID")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_DIGIMAX_NOT_WITH_RESID_DA, "Digimax kan ikke bruges med ReSID\nSkift SID-motor til FastSID"},
/* de */ {IDGS_DIGIMAX_NOT_WITH_RESID_DE, "Digimax kann mit ReSID nicht benutzt werden\nBitte SID Engine auf FastSID "
                                          "setzen"},
/* fr */ {IDGS_DIGIMAX_NOT_WITH_RESID_FR, "Digimax ne peut être utilisé avec ReSID\nVeuillez utiliser l’engin SID "
                                          "FastSID"},
/* hu */ {IDGS_DIGIMAX_NOT_WITH_RESID_HU, "A Digimax nem használható ReSID-del\nKérem váltsa a SID motort FastSID-re"},
/* it */ {IDGS_DIGIMAX_NOT_WITH_RESID_IT, "Digimax non può essere usato con il ReSID\nCambia il motore SID a FastSID"},
/* nl */ {IDGS_DIGIMAX_NOT_WITH_RESID_NL, "Digimax kan niet gebruikt worden met ReSID\nschakel de SID-kern over naar "
                                          "FastSID"},
/* pl */ {IDGS_DIGIMAX_NOT_WITH_RESID_PL, ""},  /* fuzzy */
/* sv */ {IDGS_DIGIMAX_NOT_WITH_RESID_SV, "Digimax kan inte användas med ReSID\nByt SID-motor till FastSID"},
/* tr */ {IDGS_DIGIMAX_NOT_WITH_RESID_TR, "Digimax ReSID ile birlikte kullanýlamaz\nLütfen SID Motorunu FastSID'e çevirin"},
#endif

/* c64dtv/c64dtvflash.c */
/* en */ {IDGS_NO_FILENAME,    N_("No filename!")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_NO_FILENAME_DA, "Intet filnavn!"},
/* de */ {IDGS_NO_FILENAME_DE, "Dateiname fehlt!"},
/* fr */ {IDGS_NO_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDGS_NO_FILENAME_HU, "Nincs fájlnév!"},
/* it */ {IDGS_NO_FILENAME_IT, "Nessun nome file!"},
/* nl */ {IDGS_NO_FILENAME_NL, "Geen bestandsnaam!"},
/* pl */ {IDGS_NO_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDGS_NO_FILENAME_SV, "Filnamn saknas!"},
/* tr */ {IDGS_NO_FILENAME_TR, "Dosya ismi yok!"},
#endif

/* c64dtv/c64dtvflash.c */
/* en */ {IDGS_ILLEGAL_FILENAME,    N_("Illegal filename!")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_ILLEGAL_FILENAME_DA, "Ugyldigt filnavn!"},
/* de */ {IDGS_ILLEGAL_FILENAME_DE, "Ungültiger Dateiname!"},
/* fr */ {IDGS_ILLEGAL_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDGS_ILLEGAL_FILENAME_HU, "Érvénytelen fájlnév!"},
/* it */ {IDGS_ILLEGAL_FILENAME_IT, "Nome file non valido!"},
/* nl */ {IDGS_ILLEGAL_FILENAME_NL, "Bestandsnaam ongeldig!"},
/* pl */ {IDGS_ILLEGAL_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ILLEGAL_FILENAME_SV, "Ogiltigt filnamn!"},
/* tr */ {IDGS_ILLEGAL_FILENAME_TR, "Kural dýþý dosya ismi!"},
#endif

/* c64dtv/c64dtvflash.c */
/* en */ {IDGS_ERROR_CREATING_FILE_S,    N_("Error creating file %s!")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_ERROR_CREATING_FILE_S_DA, "Fejl under oprettelse af filen %s!"},
/* de */ {IDGS_ERROR_CREATING_FILE_S_DE, "Fehler beim Schreiben: %s."},
/* fr */ {IDGS_ERROR_CREATING_FILE_S_FR, ""},  /* fuzzy */
/* hu */ {IDGS_ERROR_CREATING_FILE_S_HU, "Hiba %s fájl létrehozása közben!"},
/* it */ {IDGS_ERROR_CREATING_FILE_S_IT, "Errore durante la creazione del file %s!"},
/* nl */ {IDGS_ERROR_CREATING_FILE_S_NL, "Fout bij het maken van bestand %s!"},
/* pl */ {IDGS_ERROR_CREATING_FILE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ERROR_CREATING_FILE_S_SV, "Fel när filen \"%s\" skulle skapas!"},
/* tr */ {IDGS_ERROR_CREATING_FILE_S_TR, "%s dosyasý yaratýlýrken hata oluþtu!"},
#endif

/* c64dtv/c64dtvflash.c */
/* en */ {IDGS_ERROR_WRITING_TO_FILE_S,    N_("Error writing to file %s!")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_ERROR_WRITING_TO_FILE_S_DA, "Fejl ved skrivning til filen %s!"},
/* de */ {IDGS_ERROR_WRITING_TO_FILE_S_DE, "Fehler bei Schreibzugriff: %s!"},
/* fr */ {IDGS_ERROR_WRITING_TO_FILE_S_FR, ""},  /* fuzzy */
/* hu */ {IDGS_ERROR_WRITING_TO_FILE_S_HU, "Hiba %s fájlba írás közben!"},
/* it */ {IDGS_ERROR_WRITING_TO_FILE_S_IT, "Errore durante la scrittura sul file %s!"},
/* nl */ {IDGS_ERROR_WRITING_TO_FILE_S_NL, "Fout bij het schrijven naar bestand %s!"},
/* pl */ {IDGS_ERROR_WRITING_TO_FILE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ERROR_WRITING_TO_FILE_S_SV, "Fel vid skrivning till filen %s!"},
/* tr */ {IDGS_ERROR_WRITING_TO_FILE_S_TR, "%s dosyasýna yazýlýrken hata oluþtu!"},
#endif

/* c64dtv/c64dtvflash.c */
/* en */ {IDGS_DTV_ROM_CREATED,    N_("DTV ROM image created successfully")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_DTV_ROM_CREATED_DA, "DTV-ROM-image oprettet med success"},
/* de */ {IDGS_DTV_ROM_CREATED_DE, "DTV ROM Image erfolgreich erzeugt"},
/* fr */ {IDGS_DTV_ROM_CREATED_FR, ""},  /* fuzzy */
/* hu */ {IDGS_DTV_ROM_CREATED_HU, "DTV ROM képmás sikeresen létrehozva"},
/* it */ {IDGS_DTV_ROM_CREATED_IT, "Immagine ROM DTV creata con successo"},
/* nl */ {IDGS_DTV_ROM_CREATED_NL, "DTV ROM bestand gemaakt!"},
/* pl */ {IDGS_DTV_ROM_CREATED_PL, ""},  /* fuzzy */
/* sv */ {IDGS_DTV_ROM_CREATED_SV, "DTV-ROM-avbildning skapades framgångsrikt"},
/* tr */ {IDGS_DTV_ROM_CREATED_TR, "DTV ROM imajý baþarýyla oluþturuldu"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_COMMAND_S_UNKNOWN,    N_("Command `%s' unknown.\n")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_COMMAND_S_UNKNOWN_DA, "Ukendt kommando \"%s\".\n"},
/* de */ {IDGS_COMMAND_S_UNKNOWN_DE, "Kommand `%s' ist unbekannt.\n"},
/* fr */ {IDGS_COMMAND_S_UNKNOWN_FR, ""},  /* fuzzy */
/* hu */ {IDGS_COMMAND_S_UNKNOWN_HU, "`%s' parancs ismeretlen.\n"},
/* it */ {IDGS_COMMAND_S_UNKNOWN_IT, "Comando `%s' sconosciuto.\n"},
/* nl */ {IDGS_COMMAND_S_UNKNOWN_NL, "Commando %s' is onbekent.\n"},
/* pl */ {IDGS_COMMAND_S_UNKNOWN_PL, ""},  /* fuzzy */
/* sv */ {IDGS_COMMAND_S_UNKNOWN_SV, "Kommandot \"%s\" är okänt.\n"},
/* tr */ {IDGS_COMMAND_S_UNKNOWN_TR, "`%s' komutu bilinmiyor.\n"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_NO_HELP_AVAILABLE_FOR_S,    N_("No help available for `%s'\n")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_NO_HELP_AVAILABLE_FOR_S_DA, "Ingen hjælp tilgængelig for \"%s\"\n"},
/* de */ {IDGS_NO_HELP_AVAILABLE_FOR_S_DE, "Es ist keine Hilfe für `%s' verfügbar\n"},
/* fr */ {IDGS_NO_HELP_AVAILABLE_FOR_S_FR, ""},  /* fuzzy */
/* hu */ {IDGS_NO_HELP_AVAILABLE_FOR_S_HU, "Nincs súgó a `%s' parancshoz\n"},
/* it */ {IDGS_NO_HELP_AVAILABLE_FOR_S_IT, "Nessun help disponibile per `%s'\n"},
/* nl */ {IDGS_NO_HELP_AVAILABLE_FOR_S_NL, "Geen help beschikbaar voor `%s'\n"},
/* pl */ {IDGS_NO_HELP_AVAILABLE_FOR_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_NO_HELP_AVAILABLE_FOR_S_SV, "Hjälp saknas för \"%s\"\n"},
/* tr */ {IDGS_NO_HELP_AVAILABLE_FOR_S_TR, "`%s' için mevcut bir yardým metni yok\n"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_SYNTAX_S_S,    N_("\nSyntax: %s %s\n")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_SYNTAX_S_S_DA, "\nSyntax: %s %s\n"},
/* de */ {IDGS_SYNTAX_S_S_DE, "\nSyntax: %s %s\n"},
/* fr */ {IDGS_SYNTAX_S_S_FR, ""},  /* fuzzy */
/* hu */ {IDGS_SYNTAX_S_S_HU, "\nSzintaxis: %s %s\n"},
/* it */ {IDGS_SYNTAX_S_S_IT, "\nSintassi: %s %s\n"},
/* nl */ {IDGS_SYNTAX_S_S_NL, "\nSyntax: %s %s\n"},
/* pl */ {IDGS_SYNTAX_S_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_SYNTAX_S_S_SV, "\nSyntax: %s %s\n"},
/* tr */ {IDGS_SYNTAX_S_S_TR, "\nSözdizimi: %s %s\n"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_ABBREVIATION_S,    N_("Abbreviation: %s\n")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_ABBREVIATION_S_DA, "Forkortelse: %s\n"},
/* de */ {IDGS_ABBREVIATION_S_DE, "Abkürzung: %s\n"},
/* fr */ {IDGS_ABBREVIATION_S_FR, ""},  /* fuzzy */
/* hu */ {IDGS_ABBREVIATION_S_HU, "Rövidítés: %s\n"},
/* it */ {IDGS_ABBREVIATION_S_IT, "Abbreviazione: %s\n"},
/* nl */ {IDGS_ABBREVIATION_S_NL, "Afkorting: %s\n"},
/* pl */ {IDGS_ABBREVIATION_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ABBREVIATION_S_SV, "Förkortning: %s\n"},
/* tr */ {IDGS_ABBREVIATION_S_TR, "Kýsaltma: %s\n"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_AVAILABLE_COMMANDS_ARE,    N_("Available commands are:")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_AVAILABLE_COMMANDS_ARE_DA, "Tilgængelige kommandoer:"},
/* de */ {IDGS_AVAILABLE_COMMANDS_ARE_DE, "Zulässige Kommandos sind:"},
/* fr */ {IDGS_AVAILABLE_COMMANDS_ARE_FR, ""},  /* fuzzy */
/* hu */ {IDGS_AVAILABLE_COMMANDS_ARE_HU, "Lehetséges parancsok:"},
/* it */ {IDGS_AVAILABLE_COMMANDS_ARE_IT, "I comandi disponibili sono:"},
/* nl */ {IDGS_AVAILABLE_COMMANDS_ARE_NL, "Beschikbare commandos zijn:"},
/* pl */ {IDGS_AVAILABLE_COMMANDS_ARE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_AVAILABLE_COMMANDS_ARE_SV, "Tillgängliga kommandon:"},
/* tr */ {IDGS_AVAILABLE_COMMANDS_ARE_TR, "Kullanýlabilir komutlar:"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MACHINE_STATE_COMMANDS,    N_("Machine state commands:")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MACHINE_STATE_COMMANDS_DA, "Kommandoer for maskinstatus:"},
/* de */ {IDGS_MACHINE_STATE_COMMANDS_DE, "Maschinen Zustandskommandos:"},
/* fr */ {IDGS_MACHINE_STATE_COMMANDS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MACHINE_STATE_COMMANDS_HU, "Gép állapot parancsok:"},
/* it */ {IDGS_MACHINE_STATE_COMMANDS_IT, "Comandi di stato del computer:"},
/* nl */ {IDGS_MACHINE_STATE_COMMANDS_NL, "Machine staat commandos:"},
/* pl */ {IDGS_MACHINE_STATE_COMMANDS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MACHINE_STATE_COMMANDS_SV, "Kommandon för maskinstatus:"},
/* tr */ {IDGS_MACHINE_STATE_COMMANDS_TR, "Makine durum komutlarý:"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_MEMSPACE_R_BANKNAME,    N_("[<memspace>] [bankname]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_MEMSPACE_R_BANKNAME_DA, "[<hukommelsesområde>] [banknavn]"},
/* de */ {IDGS_RP_MEMSPACE_R_BANKNAME_DE, "[<Speicherbereich>] [Speicherbank]"},
/* fr */ {IDGS_RP_MEMSPACE_R_BANKNAME_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_MEMSPACE_R_BANKNAME_HU, "[<memória tér>] [lap név]"},
/* it */ {IDGS_RP_MEMSPACE_R_BANKNAME_IT, "[<spazio di memoria>] [nome banco]"},
/* nl */ {IDGS_RP_MEMSPACE_R_BANKNAME_NL, "[<geheugenruimte>] [banknaam]"},
/* pl */ {IDGS_RP_MEMSPACE_R_BANKNAME_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_MEMSPACE_R_BANKNAME_SV, "[<minnesområde>] [banknamn]"},
/* tr */ {IDGS_RP_MEMSPACE_R_BANKNAME_TR, "[<bellekalaný>] [bankismi]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_BANK_DESCRIPTION,    N_("If bankname is not given, print the possible banks for the memspace.\n"
                                           "If bankname is given set the current bank in the memspace to the given\n"
                                           "bank.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_BANK_DESCRIPTION_DA, "Hvis banknavn ikke angives vises mulige banker for hukommelsesområdet.\n"
                                        "Hvis banknavn angives sættes aktuel bank i hukommelsesområdet til den\n"
                                        "angivne bank."},
/* de */ {IDGS_MON_BANK_DESCRIPTION_DE, "Falls `bankname' nicht angegeben wird, Ausgabe der möglichen Bänke in `memspace'.\n"
                                        "Falls `bankname' angegeben wird, setze aktuelle Bank in `memspace' mit angegebener Bank."},
/* fr */ {IDGS_MON_BANK_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_BANK_DESCRIPTION_HU, "Ha a lap száma nincs megadva, kiírja a lehetséges lapokat a memória térbõl.\n"
                                        "Ha meg van adva, az aktuális lap számot a memória térben a megadott lapra állítja."},
/* it */ {IDGS_MON_BANK_DESCRIPTION_IT, "Se non viene specificato il nome del banco, stampa tutti i banchi\n"
                                        "possibili per lo spazio di memoria.\n"
                                        "Se viene specificato il nome del banco, imposta il banco attuale nello\n"
                                        "spazio di memoria specificato."},
/* nl */ {IDGS_MON_BANK_DESCRIPTION_NL, "Wanneer de banknaam niet is opgegeven, toon de mogelijke banken voor de geheugen-\n"
                                        "ruimte. Wanneer de banknaam is opgegeven, zet de huidige bank in de geheugen-\n"
                                        "ruimte naar de opgegeven bank."},
/* pl */ {IDGS_MON_BANK_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_BANK_DESCRIPTION_SV, "Om banknamn inte anges, skriv ut möjliga bankar för minnesområdet.\n"
                                        "Om banknamn anges, sätt aktuell bank i minnesområdet till den givna\n"
                                        "banken."},
/* tr */ {IDGS_MON_BANK_DESCRIPTION_TR, "Eðer bank ismi verilmemiþse, bellek alaný için uygun banklar yazdýrýlýr.\n"
                                        "Eðer bank ismi verilmiþse, geçerli bank bellek alaný içinde verilen\n"
                                        "banka eþitlenir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_BACKTRACE_DESCRIPTION,    N_("Print JSR call chain (most recent call first). Stack offset\n"
                                                "relative to SP+1 is printed in parentheses. This is a best guess\n"
                                                "only.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_BACKTRACE_DESCRIPTION_DA, "Vis JSR-kaldekæde (seneste kald øverst). Stakafstand\n"
                                             "relativ til SP+1 vises i parantes. Dette er kun et\n"
                                             "gæt."},
/* de */ {IDGS_MON_BACKTRACE_DESCRIPTION_DE, "Zeige JSR Aufrufkette (letzter Aufruf zuerst). Stack offset\n"
                                             "wird relative zu SP+1 in Klammern gezeigt. Dies ist jedoch nur\n"
                                             "eine Schätzung."},
/* fr */ {IDGS_MON_BACKTRACE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_BACKTRACE_DESCRIPTION_HU, "JSR hívás lista kiírása (legutóbbi elõször). A verem SP+1\n"
                                             "helyen lévõ tartalma zárójelben. Ez csak\n"
                                             "egy legjobb becslés."},
/* it */ {IDGS_MON_BACKTRACE_DESCRIPTION_IT, "Stampa la catena di chiamate JSR (la chiamata più recente per prima).\n"
                                             "L'offset dello stack relativo a SP+1 è stampato tra parentesi. Questo è\n"
                                             "solo l'ipotesi migliore."},
/* nl */ {IDGS_MON_BACKTRACE_DESCRIPTION_NL, "Toon JSR aanvraag ketting (meest recente aanvraag eerst). De stack offset\n"
                                             "relatief ten opzichte van SP+1 zal binnen aanhalingstekens worden getoond.\n"
                                             "Dit is alleen de beste schatting"},
/* pl */ {IDGS_MON_BACKTRACE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_BACKTRACE_DESCRIPTION_SV, "Skriv JSR-anropskedja (senaste anrop först). Stackavstånd\n"
                                             "relativt till SP+1 visas inom parantes. Detta är bara en\n"
                                             "gissning."},
/* tr */ {IDGS_MON_BACKTRACE_DESCRIPTION_TR, "JSR çaðrý zincirini yazdýr (en son çaðrýlar öncelikli). Yýðýt ofseti\n"
                                             "SP+1'e baðlý olarak parantez içersinde yazdýrýlýr. Bu yalnýzca en iyi\n"
                                             "tahmindir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_CPU_DESCRIPTION,    N_("Specify the type of CPU currently used (6502/z80).")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_CPU_DESCRIPTION_DA, "Angiv typen af CPU der anvendes for øjeblikket (6502/z80)."},
/* de */ {IDGS_MON_CPU_DESCRIPTION_DE, "CPU Typ setzen (6502/z80)"},
/* fr */ {IDGS_MON_CPU_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_CPU_DESCRIPTION_HU, "A processzor típusának megadása (6502/z80)."},
/* it */ {IDGS_MON_CPU_DESCRIPTION_IT, "Specifica il tipo di CPU utilizzata al momento (6502/z80)."},
/* nl */ {IDGS_MON_CPU_DESCRIPTION_NL, "Geef het soort van de huidige CPU (6502/z80)."},
/* pl */ {IDGS_MON_CPU_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_CPU_DESCRIPTION_SV, "Ange typ av CPU som används för närvarande (6502/z80)."},
/* tr */ {IDGS_MON_CPU_DESCRIPTION_TR, "Þu anda kullanýlmakta olan CPU tipini belirleyin (6502/z80)."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_COUNT,    N_("[<count>]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_COUNT_DA, "[<antal>]"},
/* de */ {IDGS_RP_COUNT_DE, "[<Zähler>]"},
/* fr */ {IDGS_RP_COUNT_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_COUNT_HU, "[<darab>]"},
/* it */ {IDGS_RP_COUNT_IT, "[<contatore>]"},
/* nl */ {IDGS_RP_COUNT_NL, "[<aantal>]"},
/* pl */ {IDGS_RP_COUNT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_COUNT_SV, "[<antal>]"},
/* tr */ {IDGS_RP_COUNT_TR, "[<adet>]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_CPUHISTORY_DESCRIPTION,    N_("Show <count> last executed commands.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_CPUHISTORY_DESCRIPTION_DA, "Vis <antal> senest eksekverede instruktioner."},
/* de */ {IDGS_MON_CPUHISTORY_DESCRIPTION_DE, "Zeige [<Zähler>] zuletzt ausgeführte Kommandos"},
/* fr */ {IDGS_MON_CPUHISTORY_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_CPUHISTORY_DESCRIPTION_HU, "<darab> utoljára végrehajtott utasítás mutatása."},
/* it */ {IDGS_MON_CPUHISTORY_DESCRIPTION_IT, "Mostra gli ultimi <contatore> comandi eseguiti."},
/* nl */ {IDGS_MON_CPUHISTORY_DESCRIPTION_NL, "Toon laatst <aantal> uitgevoerde commandos."},
/* pl */ {IDGS_MON_CPUHISTORY_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_CPUHISTORY_DESCRIPTION_SV, "Visa <antal> senast exekverade kommandon."},
/* tr */ {IDGS_MON_CPUHISTORY_DESCRIPTION_TR, "<adet> son çalýþtýrýlan komutlarý göster."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_QP_FILENAME,    N_("\"<filename>\"")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_QP_FILENAME_DA, "\"<filnavn>\""},
/* de */ {IDGS_QP_FILENAME_DE, "\"<Dateiname>\""},
/* fr */ {IDGS_QP_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDGS_QP_FILENAME_HU, "\"<fájlnév>\""},
/* it */ {IDGS_QP_FILENAME_IT, "\"<nome del file>\""},
/* nl */ {IDGS_QP_FILENAME_NL, "\"<bestandsnaam>\""},
/* pl */ {IDGS_QP_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDGS_QP_FILENAME_SV, "\"<filnamn>\""},
/* tr */ {IDGS_QP_FILENAME_TR, "\"<dosyaismi>\""},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_DUMP_DESCRIPTION,    N_("Write a snapshot of the machine into the file specified.\n"
                                           "This snapshot is compatible with a snapshot written out by the UI.\n"
                                           "Note: No ROM images are included into the dump.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_DUMP_DESCRIPTION_DA, "Skriv et snapshot til den angivne fil.\n"
                                        "Dette snapshot er kompatibelt med dem, der oprettes fra menuen.\n"
                                        "OBS: ROM-images inkluderes ikke i snapshot."},
/* de */ {IDGS_MON_DUMP_DESCRIPTION_DE, "Schreibe eine Snapsho Datei der Maschine auf die angebene Datei.\n"
                                        "Dieser Snapshot ist kompatible zu einem Snapshot initiiert aus dem Menü.\n"
                                        "Hinweis: Die ROM Images werden im Snapshot nicht inkludiert."},
/* fr */ {IDGS_MON_DUMP_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_DUMP_DESCRIPTION_HU, "Pillanatkép írása a géprõl a megadott fájlba. A pillanatkép\n"
                                        "kompatibilis azzal, amelyet a grafikus felület hoz létre.\n"
                                        "Megjegyzés: a ROM képmások nem kerülnek bele."},
/* it */ {IDGS_MON_DUMP_DESCRIPTION_IT, "Scrive uno snapshot del computer sul file specificato.\n"
                                        "Questo snapshot è compatibile con uno snapshot scritto dall'IU.\n"
                                        "Nota: Le immagini ROM non sono incluse nel dump."},
/* nl */ {IDGS_MON_DUMP_DESCRIPTION_NL, "Schrijf een momentopname van de machine naar de opgegeven bestand.\n"
                                        "Deze momentopname is compatibel met een momentopname zoals geschreven van\n"
                                        "uit het menu.\n"
                                        "NB: ROM bestanden zijn niet opgenomen in de momentopname."},
/* pl */ {IDGS_MON_DUMP_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_DUMP_DESCRIPTION_SV, "Skriv en ögonblicksbild av maskinen till den angivna filen.\n"
                                        "Ögonblicksbilden är kompatibel med de som skapas från menyn.\n"
                                        "Observera: ROM-avbildningar inkluderas inte i bilden."},
/* tr */ {IDGS_MON_DUMP_DESCRIPTION_TR, "Belirtilen dosya içersine makinenin anlýk görüntüsünü yaz.\n"
                                        "Bu anlýk görüntü UI dýþýndan yazdýrýlan anlýk görüntü ile uyumludur.\n"
                                        "Not: Hiçbir ROM imajý döküm içersinde yer almaz."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_ADDRESS,    N_("<address>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_ADDRESS_DA, "<adresse>"},
/* de */ {IDGS_P_ADDRESS_DE, "<Adresse>"},
/* fr */ {IDGS_P_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_ADDRESS_HU, "<cím>"},
/* it */ {IDGS_P_ADDRESS_IT, "<indirizzo>"},
/* nl */ {IDGS_P_ADDRESS_NL, "<Adres"},
/* pl */ {IDGS_P_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_ADDRESS_SV, "<adress>"},
/* tr */ {IDGS_P_ADDRESS_TR, "<adres>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_GOTO_DESCRIPTION,    N_("Change the PC to ADDRESS and continue execution")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_GOTO_DESCRIPTION_DA, "Ændr programtælleren (PC) til ADRESSE og fortsæt eksekvering"},
/* de */ {IDGS_MON_GOTO_DESCRIPTION_DE, "Setze PC auf ADRESSE und setze Ausführung fort"},
/* fr */ {IDGS_MON_GOTO_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_GOTO_DESCRIPTION_HU, "A PC állítása ADDRESS címre és folytatás"},
/* it */ {IDGS_MON_GOTO_DESCRIPTION_IT, "Cambia il PC in INDIRIZZO e continua l'esecuzione"},
/* nl */ {IDGS_MON_GOTO_DESCRIPTION_NL, "Zet de PC naar ADRES en ga door met de uitvoering van instructies."},
/* pl */ {IDGS_MON_GOTO_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_GOTO_DESCRIPTION_SV, "Ändra PC till ADRESS och fortsätt exekveringen"},
/* tr */ {IDGS_MON_GOTO_DESCRIPTION_TR, "PC'yi ADDRESS'e çevirip çalýþtýrmaya devam edin"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_IO_DESCRIPTION,    N_("Print out the I/O area of the emulated machine.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_IO_DESCRIPTION_DA, "Udskriv I/O-området for den emulerede maskine."},
/* de */ {IDGS_MON_IO_DESCRIPTION_DE, "Drucke den I/O Bereich der emuliertern Maschine."},
/* fr */ {IDGS_MON_IO_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_IO_DESCRIPTION_HU, "Az emulált gép I/O tartományának mutatása."},
/* it */ {IDGS_MON_IO_DESCRIPTION_IT, "Stampa l'area di I/O del computer emulato."},
/* nl */ {IDGS_MON_IO_DESCRIPTION_NL, "Toon de I/O geheugenruimte van de huidige machine."},
/* pl */ {IDGS_MON_IO_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_IO_DESCRIPTION_SV, "Skriv I/O-området för den emulerade maskinen."},
/* tr */ {IDGS_MON_IO_DESCRIPTION_TR, "Emüle edilen makinenin G/Ç alanýný yazdýr."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_NEXT_DESCRIPTION,    N_("Advance to the next instruction.  Subroutines are treated as\n"
                                           "a single instruction.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_NEXT_DESCRIPTION_DA, "Gå frem til næste instruktion. Subrutiner bliver behandlet som\n"
                                        "en enkelt instruktion."},
/* de */ {IDGS_MON_NEXT_DESCRIPTION_DE, "Fahre bis zur nächsten Instruktion fort.\n"
                                        "Unterroutinen werden wie eine einzelne Instruktion behandelt."},
/* fr */ {IDGS_MON_NEXT_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_NEXT_DESCRIPTION_HU, "A következõ utasításra lépés.  A szubrutinokat egy\n"
                                        "utasításnak tekinti."},
/* it */ {IDGS_MON_NEXT_DESCRIPTION_IT, "Passa alla prossima istruzione. Le subrutine sono trattate come\n"
                                        "una singola istruzione."},
/* nl */ {IDGS_MON_NEXT_DESCRIPTION_NL, "Ga naar de volgende instructie. Subroutines worden behandled als een\n"
                                        "enkele instructie."},
/* pl */ {IDGS_MON_NEXT_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_NEXT_DESCRIPTION_SV, "Gå till nästa instruktion. Underrutiner hanteras som en\n"
                                        "enda instruktion."},
/* tr */ {IDGS_MON_NEXT_DESCRIPTION_TR, "Sonraki komuta ilerle.  Altrutinler tek bir komutmuþ\n"
                                        "gibi iþlendi."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_REG_NAME_P_NUMBER,    N_("[<reg_name> = <number> [, <reg_name> = <number>]*]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_REG_NAME_P_NUMBER_DA, "[<regnavn> = <tal> [, <regnavn> = <tal>]*]"},
/* de */ {IDGS_RP_REG_NAME_P_NUMBER_DE, "[<Register Name> = <Zahl> [, <Register Name> = <Zahl>]*]"},
/* fr */ {IDGS_RP_REG_NAME_P_NUMBER_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_REG_NAME_P_NUMBER_HU, "[<reg_név> = <szám> [, <reg_név> = <szám>]*]"},
/* it */ {IDGS_RP_REG_NAME_P_NUMBER_IT, "[<nome registro> = <numero> [, <nome registro> = <numero>]*]"},
/* nl */ {IDGS_RP_REG_NAME_P_NUMBER_NL, "[<Register_naam> = <nummer> [, <register_naam> = <nummer>]*]"},
/* pl */ {IDGS_RP_REG_NAME_P_NUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_REG_NAME_P_NUMBER_SV, "[<regnamn> = <tal> [, <regnamn> = <tal>]*]"},
/* tr */ {IDGS_RP_REG_NAME_P_NUMBER_TR, "[<reg_ismi> = <sayý> [, <reg_ismi> = <sayý>]*]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_REGISTERS_DESCRIPTION,    N_("Assign respective registers.  With no parameters, display register\n"
                                                "values.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_REGISTERS_DESCRIPTION_DA, "Tildel værdi til registre. Uden parametre vises nuværende værdier\n"
                                             "i registrene."},
/* de */ {IDGS_MON_REGISTERS_DESCRIPTION_DE, "Zuweisung auf entsprechende Register. Ohne Parameter, zeige Register Werte."},
/* fr */ {IDGS_MON_REGISTERS_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_REGISTERS_DESCRIPTION_HU, "Megadott regiszterek beállítása. Paraméterek nélkül, a regiszterek\n"
                                             "tartalmának kiírása."},
/* it */ {IDGS_MON_REGISTERS_DESCRIPTION_IT, "Assegna i rispettivi registri. Senza parametri, mostra i valori dei\n"
                                             "registri."},
/* nl */ {IDGS_MON_REGISTERS_DESCRIPTION_NL, "Vul opgegeven registers met de toebehorende waarde. Zonder parameter, toon\n"
                                             "de huidige waarden in de registers."},
/* pl */ {IDGS_MON_REGISTERS_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_REGISTERS_DESCRIPTION_SV, "Tilldela respektive register. Utan parametrar visas innehållet\n"
                                             "i registren."},
/* tr */ {IDGS_MON_REGISTERS_DESCRIPTION_TR, "Sýrayla registerlara ata.  parametreler olmadan, register deðerlerini\n"
                                             "görüntüle."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_TYPE,    N_("[<type>]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_TYPE_DA, "[<type>]"},
/* de */ {IDGS_RP_TYPE_DE, "[<Typ>]"},
/* fr */ {IDGS_RP_TYPE_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_TYPE_HU, "[<típus>]"},
/* it */ {IDGS_RP_TYPE_IT, "[<typo>]"},
/* nl */ {IDGS_RP_TYPE_NL, "[<soort>]"},
/* pl */ {IDGS_RP_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_TYPE_SV, "[<typ>]"},
/* tr */ {IDGS_RP_TYPE_TR, "[<tip>]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_RESET_DESCRIPTION,    N_("Reset the machine or drive. Type: 0 = soft, 1 = hard, 8-11 = drive.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_RESET_DESCRIPTION_DA, "Reset maskine eller diskettedrev. Type: 0 = blød, 1 = hård,\n"
                                         "8-11 = diskettedrev."},
/* de */ {IDGS_MON_RESET_DESCRIPTION_DE, "Reset der Maschine oder des Laufwerks. Typ: 0 = soft, 1 = hart, 8-11 = Laufwerk."},
/* fr */ {IDGS_MON_RESET_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_RESET_DESCRIPTION_HU, "A számítógép vagy meghajtó újraindítása. Típus: 0 = meleg, 1 = hideg, 8-11 = meghajtó."},
/* it */ {IDGS_MON_RESET_DESCRIPTION_IT, "Resetta il computer o il drive. Tipo: 0 = soft, 1 = hard, 8-11 = drive."},
/* nl */ {IDGS_MON_RESET_DESCRIPTION_NL, "Reset de machine of drive. Soort: 0 = zacht, 1 = hard, 8-11 = drive."},
/* pl */ {IDGS_MON_RESET_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_RESET_DESCRIPTION_SV, "Återställ maskinen eller diskettstationen. Typ: 0 = mjuk, 1 = hård,\n"
                                         "8-11 = diskettstation."},
/* tr */ {IDGS_MON_RESET_DESCRIPTION_TR, "Makine ya da sürücüyü resetle. Tip: 0 = soft, 1 = hard, 8-11 = drive."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_RETURN_DESCRIPTION,    N_("Continues execution and returns to the monitor just before the next\n"
                                             "RTS or RTI is executed.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_RETURN_DESCRIPTION_DA, "Fortsæt eksekveringen og vend tilbage til monitoren lige inden næste\n"
                                          "RTS eller RTI udføres."},
/* de */ {IDGS_MON_RETURN_DESCRIPTION_DE, "Setzt Ausführung fort und retourniert bevor der nächsten RTS oder RTI instruction."},
/* fr */ {IDGS_MON_RETURN_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_RETURN_DESCRIPTION_HU, "Folytatja a végrehajtást és visszatér a monitorba a következõ\n"
                                          "RTS vagy RTI utasítás elõtt."},
/* it */ {IDGS_MON_RETURN_DESCRIPTION_IT, "Continua l'esecuzione e ritorna al monitor subito prima della prossima\n"
                                          "esecuzione di RTS o RTI."},
/* nl */ {IDGS_MON_RETURN_DESCRIPTION_NL, "Ga door met uitvoeren van instructies en kom terug naar de monitor net voor de\n"
                                          "volgende RTS of RTI wordt uitgevoerd."},
/* pl */ {IDGS_MON_RETURN_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_RETURN_DESCRIPTION_SV, "Fortsätt exekveringen och återgå till monitorn precis innan nästa\n"
                                          "RTS eller RTI exekveras."},
/* tr */ {IDGS_MON_RETURN_DESCRIPTION_TR, "Çalýþmaya devam eder ve sonraki RTS ya da RTI çalýþtýrýlmadan hemen önce\n"
                                          "monitöre geri döner."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_SCREEN_DESCRIPTION,    N_("Displays the contents of the screen.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_SCREEN_DESCRIPTION_DA, "Viser skærmens indhold."},
/* de */ {IDGS_MON_SCREEN_DESCRIPTION_DE, "Zeigt Bildschirminhalt."},
/* fr */ {IDGS_MON_SCREEN_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_SCREEN_DESCRIPTION_HU, "A képernyõ tartalmának mutatása."},
/* it */ {IDGS_MON_SCREEN_DESCRIPTION_IT, "Mostra il contenuto dello schermo."},
/* nl */ {IDGS_MON_SCREEN_DESCRIPTION_NL, "Toont de inhoud van het scherm."},
/* pl */ {IDGS_MON_SCREEN_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_SCREEN_DESCRIPTION_SV, "Visar skärmens innehåll."},
/* tr */ {IDGS_MON_SCREEN_DESCRIPTION_TR, "Ekran içeriðini görüntüler."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_STEP_DESCRIPTION,    N_("Single-step through instructions.  COUNT allows stepping\n"
                                           "more than a single instruction at a time.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_STEP_DESCRIPTION_DA, "Gå skridtvis gennem instruktioner. Hvis ANTAL angives kan\n"
                                        "flere instruktioner udføres i hvert skridt."},
/* de */ {IDGS_MON_STEP_DESCRIPTION_DE, "Einzelschritt Ausführung durch Instruktionen. COUNT erlaubt die Ausführung\n"
                                        "mehrerer Instruktionen auf einmal."},
/* fr */ {IDGS_MON_STEP_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_STEP_DESCRIPTION_HU, "Lépkedés egyesével az utasításokon.  A DARAB-bal beállítható,\n"
                                        "ha több utasítást kell ugrani egyszerre."},
/* it */ {IDGS_MON_STEP_DESCRIPTION_IT, "Modalità single step attraverso le istruzioni. Il CONTATORE permette\n"
                                        "di attraversare più di una singola istruzione per volta."},
/* nl */ {IDGS_MON_STEP_DESCRIPTION_NL, "Enkel-stap door de instructies. AANTAL zorgt ervoor dat er meerdere\n"
                                        "instructies worden uitgevoerd in een keer."},
/* pl */ {IDGS_MON_STEP_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_STEP_DESCRIPTION_SV, "Stega genom instruktioner. ANTAL gör det möjligt att stega\n"
                                        "genom mer än en enkel instruktion i varje steg."},
/* tr */ {IDGS_MON_STEP_DESCRIPTION_TR, "Komutlara doðru tek-adým.  COUNT bir defada birden fazla\n"
                                        "komutu adýmlamaya izin verir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_UNDUMP_DESCRIPTION,    N_("Read a snapshot of the machine from the file specified.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_UNDUMP_DESCRIPTION_DA, "Indlæs snapshot af maskinen fra den angivne fil."},
/* de */ {IDGS_MON_UNDUMP_DESCRIPTION_DE, "Lese einen Snapshot der Maschine aus der angegebenen Datei."},
/* fr */ {IDGS_MON_UNDUMP_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_UNDUMP_DESCRIPTION_HU, "A pillanatkép betöltése a megadott fájlból."},
/* it */ {IDGS_MON_UNDUMP_DESCRIPTION_IT, "Legge lo snapshot del computer dal file specificato."},
/* nl */ {IDGS_MON_UNDUMP_DESCRIPTION_NL, "Lees een momentopname voor de machine van het opgegeven bestand."},
/* pl */ {IDGS_MON_UNDUMP_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_UNDUMP_DESCRIPTION_SV, "Läs en ögonblicksbild för maskinen från den angivna filen."},
/* tr */ {IDGS_MON_UNDUMP_DESCRIPTION_TR, "Belirtilen dosyadan makinenin anlýk görüntüsünü oku."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_SYMBOL_TABLE_COMMANDS,    N_("Symbol table commands:")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_SYMBOL_TABLE_COMMANDS_DA, "Symboltabel-kommandoer:"},
/* de */ {IDGS_SYMBOL_TABLE_COMMANDS_DE, "Symbol Tabellen Kommandos:"},
/* fr */ {IDGS_SYMBOL_TABLE_COMMANDS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_SYMBOL_TABLE_COMMANDS_HU, "Szimbólum táblázat parancsok:"},
/* it */ {IDGS_SYMBOL_TABLE_COMMANDS_IT, "Comandi tabella dei simboli:"},
/* nl */ {IDGS_SYMBOL_TABLE_COMMANDS_NL, "Symbool tabel commandos:"},
/* pl */ {IDGS_SYMBOL_TABLE_COMMANDS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_SYMBOL_TABLE_COMMANDS_SV, "Kommandon för symboltabell:"},
/* tr */ {IDGS_SYMBOL_TABLE_COMMANDS_TR, "Sembol tablosu komutlarý:"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_MEMSPACE_P_ADDRESS_P_LABEL,    N_("[<memspace>] <address> <label>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_MEMSPACE_P_ADDRESS_P_LABEL_DA, "[<huk.område>] <adresse> <etiket>"},
/* de */ {IDGS_RP_MEMSPACE_P_ADDRESS_P_LABEL_DE, "[<Speicherbereich>] <Adresse> <Bezeichner>"},
/* fr */ {IDGS_RP_MEMSPACE_P_ADDRESS_P_LABEL_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_MEMSPACE_P_ADDRESS_P_LABEL_HU, "[<memória tér>] <cím> <címke>"},
/* it */ {IDGS_RP_MEMSPACE_P_ADDRESS_P_LABEL_IT, "[<spazio di memoria>] <indirizzo> <etichetta>"},
/* nl */ {IDGS_RP_MEMSPACE_P_ADDRESS_P_LABEL_NL, "[<geheugenruimte>] <adres> <label>"},
/* pl */ {IDGS_RP_MEMSPACE_P_ADDRESS_P_LABEL_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_MEMSPACE_P_ADDRESS_P_LABEL_SV, "[<minnesområde>] <adress> <etikett>"},
/* tr */ {IDGS_RP_MEMSPACE_P_ADDRESS_P_LABEL_TR, "[<bellekalaný>] <adres> <etiket>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_ADD_LABEL_DESCRIPTION,    N_("<memspace> is one of: C: 8: 9: 10: 11:\n"
                                                "<address>  is the address which should get the label.\n"
                                                "<label>    is the name of the label; it must start with a dot (\".\").\n\n"
                                                "Map a given address to a label.  This label can be used when entering\n"
                                                "assembly code and is shown during disassembly.  Additionally, it can\n"
                                                "be used whenever an address must be specified.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_ADD_LABEL_DESCRIPTION_DA, "<huk.område>   er en af C: 8: 9: 10: eller 11:\n"
                                             "<adresse>      Er adressen som skal have etiketten.\n"
                                             "<etiket>       er navnet på etiketten; skal starte med punktum (\".\").\n"
                                             "\n"
                                             "Sæt en etiket for en adresse. Etiketten kan angives når der indtastes\n"
                                             "assemblerkode, og vises ved disassemblering. Desuden kan den bruges\n"
                                             "hvor en adresse skal angives."},
/* de */ {IDGS_MON_ADD_LABEL_DESCRIPTION_DE, "<Speicherbereich> ist aus: C: 8: 9: 10: 11:\n"
                                             "<Adresse>         bezeichnet die Adresse welche mit dem Bezeichner assoziert werden soll.\n"
                                             "<Bezeichner>      Name des Bezeichners; Muß mit einem Punkt (\".\") starten.\n"
                                             "\n"
                                             "Assoziiert einen Bezeichner mit einer Adresse. Dieser Bezeichner kann \n"
                                             "bei der Eingabe von Asssmblercode benutzt werden und wird auch beim\n"
                                             "Disassemblieren gezeigt. Darüber hinaus kann der Bezeichner überall\n"
                                             "dort genutzt werden, wo eine Adresseingabe nötig ist."},
/* fr */ {IDGS_MON_ADD_LABEL_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_ADD_LABEL_DESCRIPTION_HU, "A <memória tér> ezek közül: C: 8: 9: 10: 11:\n"
                                             "<cím>  amelyik címhez a címkét rendelje.\n"
                                             "<címke>    a címke neve; ponttal kell kezdõdjön (\".\").\n"
                                             "\n"
                                             "Az adott cím rendelése a címkéhez.  Ez a címke használható assembly\n"
                                             "program beírásakor, és visszafejtéskor megjelenik.  Mindig használható,\n"
                                             "amikor valamilyen címet kell megadni."},
/* it */ {IDGS_MON_ADD_LABEL_DESCRIPTION_IT, "<spazio di memoria> è uno tra: C: 8: 9: 10: 11:\n"
                                             "<indirizzo> è l'indirizzo a cui deve essere associata l'etichettal.\n"
                                             "<etichetta> è il nome dell'etichetta; deve iniziare con un punto (\".\").\n"
                                             "\n"
                                             "Associa l'indirizzo specificato all'etichetta. Questa etichetta può\n"
                                             "essere usata quando si inserisce codice assembly ed è mostrata\n"
                                             "nella disassemblazione. In aggiunta, può essere usata ogni volta che\n"
                                             "deve essere specificato un indirizzo."},
/* nl */ {IDGS_MON_ADD_LABEL_DESCRIPTION_NL, "<geheugenruimte> is een van: C: 8: 9: 10: 11:\n"
                                             "<adres>          is het adres waaraan de label gekoppelt moet worden.\n"
                                             "<label>          is de naam van de label; de naam moet beginnen met een punt\n"
                                             "                 (\".\").\n\n"
                                             "Koppel een opgegeven adres aan een label. Dit label kan worden gebruikt bij\n"
                                             "het invoeren van assembleer code en wordt getoont bij een disassemblering.\n"
                                             "Tevens kan de label gebruikt worden in plaats van het adres."},
/* pl */ {IDGS_MON_ADD_LABEL_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_ADD_LABEL_DESCRIPTION_SV, "<minnesområde> är en av: C: 8: 9: 10: 11:\n"
                                             "<adress>       är adressen som ska få etiketten.\n"
                                             "<etikett>      är namnet på etiketten; måste starta med punkt (\".\").\n"
                                             "\n"
                                             "Koppla en adress till en etikett. Etiketten kan anges när assemblerkod\n"
                                             "matas in och visas vid disassemblering. I tillägg kan den användas\n"
                                             "närhelst en adress måste anges."},
/* tr */ {IDGS_MON_ADD_LABEL_DESCRIPTION_TR, "<bellekalaný> bunlardan biridir: C: 8: 9: 10: 11:\n"
                                             "<adres>       etiketin duracaðý adresi belirler.\n"
                                             "<etiket>      etiketin ismidir; nokta karakteriyle baþlamalýdýr (\".\").\n"
                                             "\n"
                                             "Etiketi verilen adrese eþleþtir.  Bu etiket assembly kod girilirken kullanýlýr\n"
                                             "ve disassembly sýrasýnda gösterilir.  Ek olarak, ne zaman bir adres\n"
                                             "belirtilmesi gerekse kullanýlabilir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_MEMSPACE_P_LABEL,    N_("[<memspace>] <label>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_MEMSPACE_P_LABEL_DA, "[<huk.område>] <etiket>"},
/* de */ {IDGS_RP_MEMSPACE_P_LABEL_DE, "[<Speicherbereich>] <Bezeichner>"},
/* fr */ {IDGS_RP_MEMSPACE_P_LABEL_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_MEMSPACE_P_LABEL_HU, "[<memória tartomány>] <címke>"},
/* it */ {IDGS_RP_MEMSPACE_P_LABEL_IT, "[<spazio di memoria>] <etichetta>"},
/* nl */ {IDGS_RP_MEMSPACE_P_LABEL_NL, "[<geheugenruimte>] <label>"},
/* pl */ {IDGS_RP_MEMSPACE_P_LABEL_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_MEMSPACE_P_LABEL_SV, "[<minnesområde>] <etikett>"},
/* tr */ {IDGS_RP_MEMSPACE_P_LABEL_TR, "[<bellekalaný>] <etiket>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_DELETE_LABEL_DESCRIPTION,    N_("<memspace> is one of: C: 8: 9: 10: 11:\n"
                                                   "<label>    is the name of the label; it must start with a dot (\".\").\n\n"
                                                   "Delete a previously defined label.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_DELETE_LABEL_DESCRIPTION_DA, "<huk.område>   er en af C: 8: 9: 10: eller 11:\n"
                                                "<etiket>       er navnet på etiketten; skal starte med punktum (\".\").\n"
                                                "\n"
                                                "Fjern en tidligere oprettet etiket."},
/* de */ {IDGS_MON_DELETE_LABEL_DESCRIPTION_DE, "<Speicherbereich> ist aus: C: 8: 9: 10: 11:\n"
                                                "<Bezeichner>      Name des Bezeichners; Muß mit einem Punkt (\\\".\\\") starten.\n"
                                                "\n"
                                                "Löscht einen zuvor definierten Bezeichner."},
/* fr */ {IDGS_MON_DELETE_LABEL_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_DELETE_LABEL_DESCRIPTION_HU, "<memória tér> egy ezek közül: C: 8: 9: 10: 11:\n"
                                                "<címke>    a címke neve; ponttal kell kezdõdjön (\".\").\n"
                                                "\n"
                                                "Elõzõleg megadott címke törlése."},
/* it */ {IDGS_MON_DELETE_LABEL_DESCRIPTION_IT, "<spazio di memoria> è uno tra: C: 8: 9: 10: 11:\n"
                                                "<etichetta> è il nome dell'etichetta; deve iniziare con un punto (\".\").\n"
                                                "\n"
                                                "Cancella una etichetta definita precedentemente."},
/* nl */ {IDGS_MON_DELETE_LABEL_DESCRIPTION_NL, "<geheugenruimte> is een van: C: 8: 9: 10: 11:\n"
                                                "<label>          is de naam van de label; de naam moet beginnen met een\n"
                                                "punt (\".\").\n\n"
                                                "Verwijder een gekoppelde label."},
/* pl */ {IDGS_MON_DELETE_LABEL_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_DELETE_LABEL_DESCRIPTION_SV, "<minnesområde> är en av: C: 8: 9: 10: 11:\n"
                                                "<etikett>      är namnet på etiketten; måste starta med punkt (\".\").\n"
                                                "\n"
                                                "Ta bort en tidigare angiven etikett."},
/* tr */ {IDGS_MON_DELETE_LABEL_DESCRIPTION_TR, "<bellekalaný> bunlardan biridir: C: 8: 9: 10: 11:\n"
                                                "<etiket>      etiketin ismidir; nokta karakteriyle baþlamalýdýr (\".\").\n"
                                                "\n"
                                                "Önceden tanýmlanmýþ bir etiketi sil."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_MEMSPACE_QP_FILENAME,    N_("[<memspace>] \"<filename>\"")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_MEMSPACE_QP_FILENAME_DA, "[<huk.område>] \"<filnavn>\""},
/* de */ {IDGS_RP_MEMSPACE_QP_FILENAME_DE, "[<Speicherbereich>] \"<Dateiname>\""},
/* fr */ {IDGS_RP_MEMSPACE_QP_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_MEMSPACE_QP_FILENAME_HU, "[<memória tartomány>] \"<fájlnév>\""},
/* it */ {IDGS_RP_MEMSPACE_QP_FILENAME_IT, "[<spazio di memoria>] \"<nome file>\""},
/* nl */ {IDGS_RP_MEMSPACE_QP_FILENAME_NL, "[<geheugenruimte>] \"<bestandsnaam>\""},
/* pl */ {IDGS_RP_MEMSPACE_QP_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_MEMSPACE_QP_FILENAME_SV, "[<minnesområde>] \"<filnamn>\""},
/* tr */ {IDGS_RP_MEMSPACE_QP_FILENAME_TR, "[<bellekalaný>] <dosyaismi>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_LOAD_LABELS_DESCRIPTION,    N_("Load a file containing a mapping of labels to addresses.  If no memory\n"
                                                  "space is specified, the default readspace is used.\n\n" 
                                                  "The format of the file is the one written out by the `save_labels' command;\n" 
                                                  "it consists of some `add_label' commands, written one after the other.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_LOAD_LABELS_DESCRIPTION_DA, "Indlæs en fil som indeholder mapning fra labels til adresser.\n"
                                               "Hvis der ikke angives hukommelsesområde, anvendes det normale læseområde.\n"
                                               "\n"
                                               "Formatet på filen er det, som skrives af \"save_labels\"-kommandoen;\n"
                                               "det består af \"add_label\"-kommandoer efter hinanden."},
/* de */ {IDGS_MON_LOAD_LABELS_DESCRIPTION_DE, "Laded eine Datei mit Zuordnung von Adressen zu Bezeichnern. Wenn kein \n"
                                               "Speicherbereich angegeben wurde, wird der Standard Speicherplatz\n"
                                               "verwendet.  Das Format der Datei wird durch das Kommande `save_lables'\n"
                                               "definiert.  Es besteht aus eine Reihe von `add_label' Kommandos,\n"
                                               "hintereinander angeordnet."},
/* fr */ {IDGS_MON_LOAD_LABELS_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_LOAD_LABELS_DESCRIPTION_HU, "Címkékrõl címekre leképezéseket tartalmazó fájl betöltése.  Ha nincs memória\n"
                                               "tér megadva, az alap tér lesz használva.\n"
                                               "\n"
                                               "A fájl formátuma megegyezik a `save_labels' parancs által kiírtéval;\n"
                                               "néhány egymás után írt `add_label' parancsból áll."},
/* it */ {IDGS_MON_LOAD_LABELS_DESCRIPTION_IT, "Carica un file che contiente l'associazione tra le etichette e gli indirizzi.\n"
                                               "Se non è specificato alcuno spazio di memoria, viene usato lo spazio\n"
                                               "di lettura predefinito.\n"
                                               "\n"
                                               "Il formato del file è quello dato in output dal comando `save_labels';\n"
                                               "è composto da alcuni comandi `add_label', scritti uno di seguito all'altro."},
/* nl */ {IDGS_MON_LOAD_LABELS_DESCRIPTION_NL, "Laad een bestand met de koppelingen tussen labels en adressen. Als er geen\n"
                                               "geheugenruimte is opgegeven zal de standaard geheugenruimte worden gebruikt.\n"
                                               "Het formaat van het bestand is die door het commando 'save_labels' wordt\n"
                                               "geschreven; het bestaat uit een aantal 'add_label' commandos na elkaar."},
/* pl */ {IDGS_MON_LOAD_LABELS_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_LOAD_LABELS_DESCRIPTION_SV, "Läs in en fil som innehåller koppling från etiketter till adresser.\n"
                                               "Om inget minnesområde anges användas standard-läsadressen.\n"
                                               "\n"
                                               "Formatet på filen är samma som skrivs av \"save_labels\"-kommandot;\n"
                                               "det består av några \"add_label\"-kommandon efter varandra."},
/* tr */ {IDGS_MON_LOAD_LABELS_DESCRIPTION_TR, "Etiketleri adreslerle eþleþtirilmiþ bir dosya yükleyin.  Eðer bellek\n"
                                               "alaný belirtilmemiþse, varsayýlan okuma alaný kullanýlýr.\n"
                                               "\n"
                                               "Dosyanýn biçimi `save_labels' komutu ile yazýlmýþ olandýr;\n"
                                               "bu peþ peþe yazýlmýþ `add_label' komutlarýndan ibarettir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_SAVE_LABELS_DESCRIPTION,    N_("Save labels to a file.  If no memory space is specified, all of the\n"
                                                  "labels are saved.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_SAVE_LABELS_DESCRIPTION_DA, "Gem etiketter til fil. Hvis der ikke angives et hukommelsesområde\n"
                                               "gemmes alle etiketter."},
/* de */ {IDGS_MON_SAVE_LABELS_DESCRIPTION_DE, "Speichere Bezeichner in eine Datei. Wenn kein Speicherbereich\n"
                                               "angegeben wurde, werden alle Bezeichner gespeichert."},
/* fr */ {IDGS_MON_SAVE_LABELS_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_SAVE_LABELS_DESCRIPTION_HU, "Címkék mentése fájlba.  Ha nincs memória tartomány megadva, az\n"
                                               "összes címkét menti."},
/* it */ {IDGS_MON_SAVE_LABELS_DESCRIPTION_IT, "Salva le etichette su file. Se non è specificato alcuno spazio di memoria,\n"
                                               "vengono salvate tutte le etichette."},
/* nl */ {IDGS_MON_SAVE_LABELS_DESCRIPTION_NL, "Sla de labels op in een bestand. Als er geen geheugenruimte is opgegeven\n"
                                               "worden alle labels opgeslagen."},
/* pl */ {IDGS_MON_SAVE_LABELS_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_SAVE_LABELS_DESCRIPTION_SV, "Spara etiketter till en fil. Om inget minnesområde anges sparas alla\n"
                                               "etiketterna."},
/* tr */ {IDGS_MON_SAVE_LABELS_DESCRIPTION_TR, "Etiketleri bir dosyaya kaydet.  Eðer bellek alaný belirtilmemiþse, tüm\n"
                                               "etiketler kaydedilir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_MEMSPACE,    N_("[<memspace>]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_MEMSPACE_DA, "[<huk.område>]"},
/* de */ {IDGS_RP_MEMSPACE_DE, "[<Speicherbereich>]"},
/* fr */ {IDGS_RP_MEMSPACE_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_MEMSPACE_HU, "[<memória tartomány>]"},
/* it */ {IDGS_RP_MEMSPACE_IT, "[<spazio di memoria>]"},
/* nl */ {IDGS_RP_MEMSPACE_NL, "[<geheugenruimte>]"},
/* pl */ {IDGS_RP_MEMSPACE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_MEMSPACE_SV, "[<minnesområde>]"},
/* tr */ {IDGS_RP_MEMSPACE_TR, "[<bellekalaný>]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_SHOW_LABELS_DESCRIPTION,    N_("Display current label mappings.  If no memory space is specified, show\n"
                                                  "all labels.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_SHOW_LABELS_DESCRIPTION_DA, "Vis aktuelle etiketter. Hvis der ikke angives et hukommelsesområde vises\n"
                                               "alle etiketter."},
/* de */ {IDGS_MON_SHOW_LABELS_DESCRIPTION_DE, "Zeige aktuelle Bezeichner Zuordnungen. Wenn kein Speicherbereich angegeben\n"
                                               "wurde, werden alle Bezeichner gezeigt."},
/* fr */ {IDGS_MON_SHOW_LABELS_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_SHOW_LABELS_DESCRIPTION_HU, "Az aktuális címke leképezések mutatása.  Ha nincs a memória tér megadva, az\n"
                                               "összeset mutatja."},
/* it */ {IDGS_MON_SHOW_LABELS_DESCRIPTION_IT, "Mostra l'associazione delle etichette correnti. Se non è specificato alcun\n"
                                               "spazio di memoria, verranno mostrate tutte le etichette."},
/* nl */ {IDGS_MON_SHOW_LABELS_DESCRIPTION_NL, "Toon huidige label koppelingen. Als er geen geheugenruimte is opgegeven\n"
                                               "worden all labels getoont."},
/* pl */ {IDGS_MON_SHOW_LABELS_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_SHOW_LABELS_DESCRIPTION_SV, "Visar aktuella etiketter. Om inget minnesområde anges visas alla\n"
                                               "etiketter."},
/* tr */ {IDGS_MON_SHOW_LABELS_DESCRIPTION_TR, "Geçerli etiket eþleþmelerini görüntüle.  Eðer bellek alaný belirtilmemiþse,\n"
                                               "tüm etiketleri göster."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_ASSEMBLER_AND_MEMORY_COMMANDS,    N_("Assembler and memory commands:")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_ASSEMBLER_AND_MEMORY_COMMANDS_DA, "Assembler- og hukommelseskommandoer:"},
/* de */ {IDGS_ASSEMBLER_AND_MEMORY_COMMANDS_DE, "Assembler und Speicher Kommandos:"},
/* fr */ {IDGS_ASSEMBLER_AND_MEMORY_COMMANDS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_ASSEMBLER_AND_MEMORY_COMMANDS_HU, "Assembler és memória parancsok:"},
/* it */ {IDGS_ASSEMBLER_AND_MEMORY_COMMANDS_IT, "Comandi assembler e memoria:"},
/* nl */ {IDGS_ASSEMBLER_AND_MEMORY_COMMANDS_NL, "Assembler en geheugen commandos:"},
/* pl */ {IDGS_ASSEMBLER_AND_MEMORY_COMMANDS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ASSEMBLER_AND_MEMORY_COMMANDS_SV, "Assembler- och minneskommandon:"},
/* tr */ {IDGS_ASSEMBLER_AND_MEMORY_COMMANDS_TR, "Assembler ve bellek komutlarý:"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_ADDRESS_P_DATA_LIST,    N_("[<address>] <data_list>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_ADDRESS_P_DATA_LIST_DA, "[<adresse>] <dataliste>"},
/* de */ {IDGS_RP_ADDRESS_P_DATA_LIST_DE, "[<Adresse>] <Datenliste>"},
/* fr */ {IDGS_RP_ADDRESS_P_DATA_LIST_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_ADDRESS_P_DATA_LIST_HU, "[<cím>] <adat_lista>"},
/* it */ {IDGS_RP_ADDRESS_P_DATA_LIST_IT, "[<indirizzo>] <lista di dati>"},
/* nl */ {IDGS_RP_ADDRESS_P_DATA_LIST_NL, "[<adres>] <data_lijst>"},
/* pl */ {IDGS_RP_ADDRESS_P_DATA_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_ADDRESS_P_DATA_LIST_SV, "[<adress>] <datalista>"},
/* tr */ {IDGS_RP_ADDRESS_P_DATA_LIST_TR, "[<adres>] <veri_listesi>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_WRITE_DESCRIPTION,    N_("Write the specified data at `address'.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_WRITE_DESCRIPTION_DA, "Gem angivne data på \"adresse\"."},
/* de */ {IDGS_MON_WRITE_DESCRIPTION_DE, "Schreibe das angegebene Datum auf `Adresse'."},
/* fr */ {IDGS_MON_WRITE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_WRITE_DESCRIPTION_HU, "A megadott adat `cím' helyre írása."},
/* it */ {IDGS_MON_WRITE_DESCRIPTION_IT, "Scrive il dato specificato all'indirizzo."},
/* nl */ {IDGS_MON_WRITE_DESCRIPTION_NL, "Schrijf de opgegevem data op `adres'."},
/* pl */ {IDGS_MON_WRITE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_WRITE_DESCRIPTION_SV, "Skriv angiven data på \"adress\"."},
/* tr */ {IDGS_MON_WRITE_DESCRIPTION_TR, "`adres' bölümünde belirlenmiþ bir veri yaz."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_ADDRESS_RP_INSTRUCTION,    N_("<address> [ <instruction> [: <instruction>]* ]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_ADDRESS_RP_INSTRUCTION_DA, "<adresse> [ <instruktion> [: <instruktion>]* ]"},
/* de */ {IDGS_P_ADDRESS_RP_INSTRUCTION_DE, "<Adresse> [ <Instruktion> [: <Instruktion>]* ]"},
/* fr */ {IDGS_P_ADDRESS_RP_INSTRUCTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_ADDRESS_RP_INSTRUCTION_HU, "<cím> [ <utasítás> [: <utasítás>]* ]"},
/* it */ {IDGS_P_ADDRESS_RP_INSTRUCTION_IT, "<indirizzo> [ <istruzione> [: <istruzione>]* ]"},
/* nl */ {IDGS_P_ADDRESS_RP_INSTRUCTION_NL, "<adres> [ <instructie> [: <instructie>]* ]"},
/* pl */ {IDGS_P_ADDRESS_RP_INSTRUCTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_ADDRESS_RP_INSTRUCTION_SV, "<adress> [ <instruktion> [: <instruktion>]* ]"},
/* tr */ {IDGS_P_ADDRESS_RP_INSTRUCTION_TR, "<adres> [ <komut> [: <komut>]* ]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_ASSEMBLE_DESCRIPTION,    N_("Assemble instructions to the specified address.  If only one\n"
                                               "instruction is specified, enter assembly mode (enter an empty line to\n"
                                               "exit assembly mode).")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_ASSEMBLE_DESCRIPTION_DA, "Skriv assemblerinstruktioner til adresse. Hvis kun en instruktion\n"
                                            "angives, skiftes til assembler-tilstand (indtast en tom linje for\n"
                                            "at afslutte assembler-tilstanden)."},
/* de */ {IDGS_MON_ASSEMBLE_DESCRIPTION_DE, "Assmbler Instruktionen zur angebenen Adresse. Wenn nur eine Instruktion\n"
                                            "angegeben wurde, wird der assemblier Modus aktiviert (Eingabe einer leeren\n"
                                            "Zeile beendet den assemblier Modus)."},
/* fr */ {IDGS_MON_ASSEMBLE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_ASSEMBLE_DESCRIPTION_HU, "Utasítások fordítása a megadott címre. Ha csak egy utasítás\n"
                                            "van, fordító módba lép (üres sort kell beírni a kilépéshez)."},
/* it */ {IDGS_MON_ASSEMBLE_DESCRIPTION_IT, "Assembla le istruzioni a partire dall'indirizzo specificato. Se solo una\n"
                                            "istruzione è specificata, entra in modalità assembly (inserire una\n"
                                            "linea vuota per uscire da questa modalità)."},
/* nl */ {IDGS_MON_ASSEMBLE_DESCRIPTION_NL, "Assembleer instructies naar het opgegeven adres. Als er alleen een instructie\n"
                                            "is opgegeven dan wordt de assembleer modus ingeschakelt (geef een lege regel\n"
                                            "in om de assembleer modus te verlaten)."},
/* pl */ {IDGS_MON_ASSEMBLE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_ASSEMBLE_DESCRIPTION_SV, "Assemblera instruktioner till angiven adress. Om endast en\n"
                                            "instruktion angavs, aktivera assemblerläge (mata in en tom rad\n"
                                            "för att avsluta assemblerläget)."},
/* tr */ {IDGS_MON_ASSEMBLE_DESCRIPTION_TR, "Komutlarý belirlenmiþ adrese çevir.  Eðer yalnýzca bir\n"
                                            "komut belirlenmiþse, assembly moduna gir (assembly modundan çýkmak için\n"
                                            "boþ bir satýr girin)."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_ADDRESS_RANGE_P_ADDRESS,    N_("<address_range> <address>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_ADDRESS_RANGE_P_ADDRESS_DA, "<adresseområde> <adresse>"},
/* de */ {IDGS_P_ADDRESS_RANGE_P_ADDRESS_DE, "<Adressbereich> <Adresse>"},
/* fr */ {IDGS_P_ADDRESS_RANGE_P_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_ADDRESS_RANGE_P_ADDRESS_HU, "<címtartomány> <cím>"},
/* it */ {IDGS_P_ADDRESS_RANGE_P_ADDRESS_IT, "<intervallo indirizzi> <indirizzo>"},
/* nl */ {IDGS_P_ADDRESS_RANGE_P_ADDRESS_NL, "<adres_reeks> <adres>"},
/* pl */ {IDGS_P_ADDRESS_RANGE_P_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_ADDRESS_RANGE_P_ADDRESS_SV, "<adressområde> <adress>"},
/* tr */ {IDGS_P_ADDRESS_RANGE_P_ADDRESS_TR, "<adres_aralýðý> <adres>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_COMPARE_DESCRIPTION,    N_("Compare memory from the source specified by the address range to the\n"
                                              "destination specified by the address.  The regions may overlap.  Any\n"
                                              "values that miscompare are displayed using the default displaytype.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_COMPARE_DESCRIPTION_DA, "Sammenlign hukommelse fra kilden, angivet ved adresseområdet med\n"
                                           "målet, angivet ved adresse. Områderne kan overlappe. Alle værdier\n"
                                           "som er forskellige vil blive vist med standard visning."},
/* de */ {IDGS_MON_COMPARE_DESCRIPTION_DE, "Vergleiche angegebenen Adressbereich mit der Zieladresse.\n"
                                           "Die Bereiche können überlappen. Ungleiche Werte werden mit dem\n"
                                           "Standardanzeigetyp gezeigt."},
/* fr */ {IDGS_MON_COMPARE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_COMPARE_DESCRIPTION_HU, "A forrás memóriatartomány összehasonlítása a céllal megadott\n"
                                           "memóriatartománnyal. A tartományok átfedése megengedett.  Minden\n"
                                           "meg nem egyezõ értéket kiír az alap adat típussal."},
/* it */ {IDGS_MON_COMPARE_DESCRIPTION_IT, "Confronta la memoria della sorgente specificata dall'intervallo di\n"
                                           "indirizzi con la destinazione specificata dall'indirizzo. Le regioni possono\n"
                                           "sovrapporsi. Qualsiasi valore che non corrisponde è visualizzato\n"
                                           "utilizzando il tipo di dato predefinito."},
/* nl */ {IDGS_MON_COMPARE_DESCRIPTION_NL, "Vergelijk geheugen van de bron opgegeven door de adres reeks met het doel\n"
                                           "opgegeven door het adres. De reeksen mogen overlappen. De data die niet\n"
                                           "gelijk is zal worden getoont volgens de huidige toon soort."},
/* pl */ {IDGS_MON_COMPARE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_COMPARE_DESCRIPTION_SV, "Jämför minne från källan som anges av adressområdet mot destinationen\n"
                                           "som anges av adressen. Områdena kan vara överlappande. Alla värden\n"
                                           "som skiljer sig vid jämförelsen visas med standardvisningssättet."},
/* tr */ {IDGS_MON_COMPARE_DESCRIPTION_TR, "Belleði adresle belirtilen kaynaktan adresle belirtilen hedefin bulunduðu\n"
                                           "aralýkta karþýlaþtýr.  Bölgeler belki üst üste binebilir.  Uyuþmayan\n"
                                           "herhangi bir deðer varsayýlan görüntüleme tipine baðlý olarak görüntülenir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_CHECKNUM,    N_("<checknum>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_CHECKNUM_DA, "<checknum>"},
/* de */ {IDGS_P_CHECKNUM_DE, "<Checknummer>"},
/* fr */ {IDGS_P_CHECKNUM_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_CHECKNUM_HU, "<ell_pont_szám>"},
/* it */ {IDGS_P_CHECKNUM_IT, "<checknum>"},
/* nl */ {IDGS_P_CHECKNUM_NL, "<checknum>"},
/* pl */ {IDGS_P_CHECKNUM_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_CHECKNUM_SV, "<kontrollnr>"},
/* tr */ {IDGS_P_CHECKNUM_TR, "<kontrolnumarasý>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_DELETE_DESCRIPTION,    N_("Delete checkpoint `checknum'.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_DELETE_DESCRIPTION_DA, "Slet checkpoint \"checknum\"."},
/* de */ {IDGS_MON_DELETE_DESCRIPTION_DE, "Lösche Checkpoint `Checknummer'."},
/* fr */ {IDGS_MON_DELETE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_DELETE_DESCRIPTION_HU, "`ell_pont_szám' ellenõrzési pont törlése."},
/* it */ {IDGS_MON_DELETE_DESCRIPTION_IT, "Cancella il punto di controllo `checknum'."},
/* nl */ {IDGS_MON_DELETE_DESCRIPTION_NL, "Verwijder checkpunt `checknum'."},
/* pl */ {IDGS_MON_DELETE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_DELETE_DESCRIPTION_SV, "Ta bort kontrollpunkt \"kontrollnr\"."},
/* tr */ {IDGS_MON_DELETE_DESCRIPTION_TR, "`kontrolnumarasý' kontrol noktasýný sil."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_ADDRESS_RP_ADDRESS,    N_("[<address> [<address>]]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_ADDRESS_RP_ADDRESS_DA, "[<adresse> [<adresse>]]"},
/* de */ {IDGS_RP_ADDRESS_RP_ADDRESS_DE, "[<Adresse> [<Adresse>]]"},
/* fr */ {IDGS_RP_ADDRESS_RP_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_ADDRESS_RP_ADDRESS_HU, "[<cím> [<cím>]]"},
/* it */ {IDGS_RP_ADDRESS_RP_ADDRESS_IT, "[<indirizzo> [<indirizzo>]]"},
/* nl */ {IDGS_RP_ADDRESS_RP_ADDRESS_NL, "[<adres> [<adres>]]"},
/* pl */ {IDGS_RP_ADDRESS_RP_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_ADDRESS_RP_ADDRESS_SV, "[<adress> [<adress>]]"},
/* tr */ {IDGS_RP_ADDRESS_RP_ADDRESS_TR, "[<adres> [<adres>]]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_DISASS_DESCRIPTION,    N_("Disassemble instructions.  If two addresses are specified, they are\n"
                                             "used as a start and end address.  If only one is specified, it is\n"
                                             "treated as the start address and a default number of instructions are\n"
                                             "disassembled.  If no addresses are specified, a default number of\n"
                                             "instructions are disassembled from the dot address.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_DISASS_DESCRIPTION_DA, "Disassemblér instruktioner. Hvis der angives to adresser anvendes disse\n"
                                          "som start- og slutadresse. Angives kun en adresse bruges denne som startadresse\n"
                                          "og et standard antal instruktioner disassembleres. Hvis ingen adresse\n"
                                          "angives disassembleres et standard antal instruktioner fra den aktuelle\nadresse."},
/* de */ {IDGS_MON_DISASS_DESCRIPTION_DE, "Disassembliere Instruktionen. Wenn zwei Adressen angegeben werden,\n"
                                          "werden diese als Start- und Zieladresse interpretiert. Wenn nur eine\n"
                                          "Adress angegeben wird, wird diese asl Startadresse interpretiert und\n"
                                          "die Standardanzahl an Instruktionen werden disassembliert. Wenn keine\n"
                                          "Adresse angegeben wird, wird von der aktuellen Adresse disassembliert."},
/* fr */ {IDGS_MON_DISASS_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_DISASS_DESCRIPTION_HU, "Utasítások visszafejtése. Ha két cím van megadva, elsõ és utolsó\n"
                                          "címként értelmezi õket. Ha csak egy, akkor elsõ címként, és az alap\n"
                                          "számú utasítás kerül visszafejtésre. Ha nincs cím megadva, az aktuális\n"
                                          "címtõl kezdi a visszafejtést, és a szokásos számú utasítást ír ki."},
/* it */ {IDGS_MON_DISASS_DESCRIPTION_IT, "Disassembla le istruzioni. Se sono specificati due indirizzi, questi\n"
                                          "sono utilizzati come indirizzo di inizio e fine. Se ne viene specificato\n"
                                          "solo uno, è usato come indirizzo di partenza e viene disassemblato\n"
                                          "il numero di istruzioni predefinito. Se non viene specificato nessun\n"
                                          "indirizzo, viene disassemblato il numero di istruzioni predefinito\n"
                                          "dall'indirizzo dot."},
/* nl */ {IDGS_MON_DISASS_DESCRIPTION_NL, "Disassembleer instructies. Als er twee adressen zijn opgegeven dan worden\n"
                                          "deze gebruikt als een start en eind adres. Als er maar een adres is\n"
                                          "opgegeven dan wordt deze gebruikt als het start adres en een standaard\n"
                                          "aantal instructies zal worden gedisassembleerd. Als er geen adressen zijn\n"
                                          "opgegeven dan wordt een standaard aantal instructies gedisassembleerd vanaf\n"
                                          "het huidige adres."},
/* pl */ {IDGS_MON_DISASS_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_DISASS_DESCRIPTION_SV, "Disassemblera instruktioner. Om två adresser anges används de som\n"
                                          "start- och slutadress. Om endast en anges används den som startadress\n"
                                          "och ett standardantal instruktioner disassembleras. Om ingen adress\n"
                                          "anges disassembleras ett standardantal instruktioner från punktadressen."},
/* tr */ {IDGS_MON_DISASS_DESCRIPTION_TR, "Komutlarý disassemble et.  Eðer iki adres girildiyse, bunlar baþlangýç\n"
                                          "ve bitiþ adresleri olarak kullanýlýr.  Eðer tek adres girildiyse, bu\n"
                                          "adres baþlangýç adresi olarak kabul edilir ve varsayýlan sayýda komutlar\n"
                                          "disassemble edilir.  Eðer hiç adres girilmediyse, varsayýlan sayýda\n"
                                          "komutlar nokta baþlangýç adresinden itibaren disassemble edilir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_ADDRESS_RANGE_P_DATA_LIST,    N_("<address_range> <data_list>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_ADDRESS_RANGE_P_DATA_LIST_DA, "<adresseområde> <dataliste>"},
/* de */ {IDGS_P_ADDRESS_RANGE_P_DATA_LIST_DE, "<Adressbereich> <Datenliste>"},
/* fr */ {IDGS_P_ADDRESS_RANGE_P_DATA_LIST_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_ADDRESS_RANGE_P_DATA_LIST_HU, "<cím_tartomány> <adat_lista>"},
/* it */ {IDGS_P_ADDRESS_RANGE_P_DATA_LIST_IT, "<intervallo di indirizzi> <lista di dati>"},
/* nl */ {IDGS_P_ADDRESS_RANGE_P_DATA_LIST_NL, "<adres_reeks> <data_lijst>"},
/* pl */ {IDGS_P_ADDRESS_RANGE_P_DATA_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_ADDRESS_RANGE_P_DATA_LIST_SV, "<adressområde> <datalista>"},
/* tr */ {IDGS_P_ADDRESS_RANGE_P_DATA_LIST_TR, "<adres_aralýðý> <veri_listesi>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_FILL_DESCRIPTION,    N_("Fill memory in the specified address range with the data in\n"
                                           "<data_list>.  If the size of the address range is greater than the size\n"
                                           "of the data_list, the data_list is repeated.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_FILL_DESCRIPTION_DA, "Fyld hukommelsen i det angivne adresseområde med data fra <dataliste>.\n"
                                        "Hvis størrelsen på adresseområdet er større end størrelsen af dataliste\n"
                                        "gentages indholdet i dataliste."},
/* de */ {IDGS_MON_FILL_DESCRIPTION_DE, "Fülle den angegebenen Adressbereich mit den Daten aus <Datenliste>.\n"
                                        "Wenn die Größe des Adressbereichs größer als <Datenliste> ist, werden\n"
                                        "die Daten wiederholt."},
/* fr */ {IDGS_MON_FILL_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_FILL_DESCRIPTION_HU, "A memória kitöltése az <adat_lista> paraméterben megadott adatokkal.\n"
                                        "Ha a címtartomány nagyobb, mint a lista, ismételni fogja azt."},
/* it */ {IDGS_MON_FILL_DESCRIPTION_IT, "Riempe la memoria nell'intervallo di indirizzi specificato con i dati\n"
                                        "presenti in <lista di dati>. Se la dimensione dell'intervallo è maggiore\n"
                                        "della dimensione della lista di dati, la lista di dati è ripetuta."},
/* nl */ {IDGS_MON_FILL_DESCRIPTION_NL, "Vul het geheugen van de opgegeven adres reeks op met de data in <data_lijst>.\n"
                                        "Als de grootte van de adres reeks groter is dan de grootte van de data_lijst\n"
                                        "dan zal de data_lijst worder herhaald."},
/* pl */ {IDGS_MON_FILL_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_FILL_DESCRIPTION_SV, "Fyll minnet i det angivna området med data i <datalista>.\n"
                                        "Om storleken på adressområdet är större än storleken på datalistan\n"
                                        "repeteras innehållet i datalistan."},
/* tr */ {IDGS_MON_FILL_DESCRIPTION_TR, "<veri_listesi>'nde bulunan verilerle belirtilen bellek aralýðýný doldur.\n"
                                        "Eðer verilen adres aralýðý veri_listesi'nden büyükse, veri_listesi'nde\n"
                                        "yer alan veriler tekrar ettirilir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_HUNT_DESCRIPTION,    N_("Hunt memory in the specified address range for the data in\n"
                                           "<data_list>.  If the data is found, the starting address of the match\n"
                                           "is displayed.  The entire range is searched for all possible matches.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_HUNT_DESCRIPTION_DA, "Søg i hukommelsen i det angivne dataområde efter <dataliste>.\n"
                                        "Om data findes vises startadressen for matchet. Hele\n"
                                        "området gennemsøges for at finde alle mulige match."},
/* de */ {IDGS_MON_HUNT_DESCRIPTION_DE, "Suche in angebenen Adressbereich nach <Datenliste>. Wenn die Daten gefunden\n"
                                        "wurden, wird die Startadresse gezeigt. Der komplette Bereich wird alle\n"
                                        "Vorkommnisse von <Datenliste> untersucht."},
/* fr */ {IDGS_MON_HUNT_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_HUNT_DESCRIPTION_HU, "Az <adat_lista> adat keresése a memóriában. Ha megtalálja a sorozatot,\n"
                                        "a kezdõcímét kiírja. Az egész tartományt végignézi az összes lehetséges\n"
                                        "találat felfedezéséhez."},
/* it */ {IDGS_MON_HUNT_DESCRIPTION_IT, "Cerca nell'intervallo di indirizzi specificato i dati nella <lista di dati>.\n"
                                        "Se il dato è trovato, viene visualizzato l'indirizzo iniziale della\n"
                                        "corrispondenza. Tutto l'intervallo viene ricercato per tutte le possibili\n"
                                        "corrispondenze."},
/* nl */ {IDGS_MON_HUNT_DESCRIPTION_NL, "Zoek het adres bereik voor de data in <data_lijst>. Als de data is gevonden\n"
                                        "dan wordt het start adres getoont. Er zal door de hele reeks worden gezocht."},
/* pl */ {IDGS_MON_HUNT_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_HUNT_DESCRIPTION_SV, "Sök i minnet i det angivna området med data efter <datalista>.\n"
                                        "Om data påträffas visas startadressen för träffen. Hela\n"
                                        "området genomsöks för att hitta alla möjliga träffar."},
/* tr */ {IDGS_MON_HUNT_DESCRIPTION_TR, "<veri_listesi>'nde bulunan veriler için belirtilen bellek aralýðýnda\n"
                                        "arama yap.  Eðer veri bulunursa, eþleþen adres aralýðýnýn baþlangýç\n"
                                        "adresi görüntülenir.  Tüm alan mümkün olan eþleþmeler için aranacaktýr."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_ADDRESS_OPT_RANGE,    N_("<address_opt_range>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_ADDRESS_OPT_RANGE_DA, "<adresse_evt_område>"},
/* de */ {IDGS_P_ADDRESS_OPT_RANGE_DE, "<Optionaler Adressbereich>"},
/* fr */ {IDGS_P_ADDRESS_OPT_RANGE_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_ADDRESS_OPT_RANGE_HU, "<címtartomány>"},
/* it */ {IDGS_P_ADDRESS_OPT_RANGE_IT, "<intervallo di indirizzi opzionale>"},
/* nl */ {IDGS_P_ADDRESS_OPT_RANGE_NL, "<optionele_adres_reeks>"},
/* pl */ {IDGS_P_ADDRESS_OPT_RANGE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_ADDRESS_OPT_RANGE_SV, "<adress_ev_område>"},
/* tr */ {IDGS_P_ADDRESS_OPT_RANGE_TR, "<opsiyonel_adres_aralýðý>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_I_DESCRIPTION,    N_("Display memory contents as PETSCII text.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_I_DESCRIPTION_DA, "Vis hukommelsesindhold som PETSCII-text."},
/* de */ {IDGS_MON_I_DESCRIPTION_DE, "Zeige Speicherinhalt als PETSCII Text."},
/* fr */ {IDGS_MON_I_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_I_DESCRIPTION_HU, "Memória tartalom mutatása PETSCII szövegként."},
/* it */ {IDGS_MON_I_DESCRIPTION_IT, "Mostra il contenuto della memoria come testo PETSCII."},
/* nl */ {IDGS_MON_I_DESCRIPTION_NL, "Toon geheugen inhoud als PETSCII tekst."},
/* pl */ {IDGS_MON_I_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_I_DESCRIPTION_SV, "Visa minnesinnehåll som PETSCII-text."},
/* tr */ {IDGS_MON_I_DESCRIPTION_TR, "Bellek içeriðini PETSCII yazý biçiminde görüntüle."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_II_DESCRIPTION,    N_("Display memory contents as screen code text.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_II_DESCRIPTION_DA, "Vis hukommelses-indhold som skærm-koder."},
/* de */ {IDGS_MON_II_DESCRIPTION_DE, "Zeige Speicherinhalt als Bildschirmcode Text."},
/* fr */ {IDGS_MON_II_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_II_DESCRIPTION_HU, "Memória tartalom megjelenítése képernyõ kód szövegként."},
/* it */ {IDGS_MON_II_DESCRIPTION_IT, "Mostra il contenuto della memoria come testo screen code."},
/* nl */ {IDGS_MON_II_DESCRIPTION_NL, "Toon geheugen inhoud als scherm code tekst."},
/* pl */ {IDGS_MON_II_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_II_DESCRIPTION_SV, "Visa minnesinnehåll som skärmkodstext."},
/* tr */ {IDGS_MON_II_DESCRIPTION_TR, "Bellek içeriðini ekran kodu yazý biçiminde görüntüle."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE,    N_("[<data_type>] [<address_opt_range>]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE_DA, "[<datatype>] [<adresse_evt_område>]"},
/* de */ {IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE_DE, "[<Datentyp>] [<Optionaler Adressbereich>]"},
/* fr */ {IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE_HU, "[<adat_típus>] [<címtartomány>]"},
/* it */ {IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE_IT, "[<dati>] [<intervallo di indirizzi opzionale>]"},
/* nl */ {IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE_NL, "[<data_soort>] [<optionele_adres_reeks>]"},
/* pl */ {IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE_SV, "[<datatyp>] [<adress_ev_område>]"},
/* tr */ {IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE_TR, "[<veri_tipi>] [<opsiyonel_adres_aralýðý>]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_MEM_DESCRIPTION,    N_("Display the contents of memory.  If no datatype is given, the default\n"
                                          "is used.  If only one address is specified, the length of data\n"
                                          "displayed is based on the datatype.  If no addresses are given, the\n"
                                          "'dot' address is used.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_MEM_DESCRIPTION_DA, "Vis hukommelsens indhold. Hvis datatype ikke angives, bruges standard.\n"
                                       "Hvis kun en adresse angives, justeres længden på visningen baseret\n"
                                       "på datatypen. Hvis der ikke angives nogen adresse anvendes \"punktum\"-adressen."},
/* de */ {IDGS_MON_MEM_DESCRIPTION_DE, "Zeige Speicherinhalt. Wenn kein Datentyp angegeben wurde, wird der\n"
                                       "Standarddatentyp verwendet. Wenn nur eine Adresse angegeben wird, hängt\n"
                                       "die Länge der Daten vom Datentyp ab. Wenn keine Adresse angeben wurde, \n"
                                       "wird die aktuelle Adresse verwendet."},
/* fr */ {IDGS_MON_MEM_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_MEM_DESCRIPTION_HU, "A memória tartalmának megjelenítése. Ha nincs adat típus megadva,\n"
                                       "az alapbeállításthasználja. Ha egy cím van megadva, a megjelenített adat\n"
                                       "mérete az adat típustól függ. Ha nincs cím megadva, a 'pont' címet használja."},
/* it */ {IDGS_MON_MEM_DESCRIPTION_IT, "Mostra il contenuto della memoria. Se non è specificato alcun tipo di\n"
                                       "dato, verrà utilizzato quello predefinito. Se è specificato solo un indirizzo,\n"
                                       "la lunghezza del dato visualizato è basato sul tipo. Se non è specificato\n"
                                       "nessun indirizzo, varrà utilizzato l'indirizzo 'dot'."},
/* nl */ {IDGS_MON_MEM_DESCRIPTION_NL, "Toon de geheugen inhoud. Als er geen datasoort is opgegeven dan zal de standaard\n"
                                       "worden gebruikt. Als er maar een adres is opgegeven dan zal de lengte van de\n"
                                       "getoonde data gebaseerd worden op de datasoort. Als er geen adres is opgegeven\n"
                                       "dan zal het huidige adres worden gebruikt."},
/* pl */ {IDGS_MON_MEM_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_MEM_DESCRIPTION_SV, "Visa minnets innehåll. Om ingen datatyp anges, används standard.\n"
                                       "Om endast en adress anges, justeras längden på visad data baserat\n"
                                       "på datatypen. Om ingen adress anges används \"punkt\"-adressen."},
/* tr */ {IDGS_MON_MEM_DESCRIPTION_TR, "Bellek içeriðini göster.  Eðer veri tipi girilmemiþse, varsayýlan\n"
                                       "kullanýlýr.  Yalnýzca bir adres belirtilmiþse, gösterilen verinin\n"
                                       "uzunluðu veri tipine baðlýdýr.  Eðer hiç adres girilmemiþse, 'nokta'\n"
                                       "adresi kullanýlýr."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_MEMCHAR_DESCRIPTION,    N_("Display the contents of memory as character data.  If only one address\n"
                                              "is specified, only one character is displayed.  If no addresses are\n"
                                              "given, the ``dot'' address is used.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_MEMCHAR_DESCRIPTION_DA, "Vis hukommelsens indhold som tegndata. Hvis kun en adresse\n"
                                           "angives, vises kun et tegn. Hvis ingen adresse angives benyttes\n"
                                           "\"punktum\"-adressen."},
/* de */ {IDGS_MON_MEMCHAR_DESCRIPTION_DE, "Zeige den Inhalt von Speicher als Character Daten. Wenn nur eine Adresse\n"
                                           "angegeben wurde, wird nur ein Zeichen gezeigt. Wenn keine Adresse angegeben\n"
                                           "wurde, wird die aktuelle Adresse verwendet."},
/* fr */ {IDGS_MON_MEMCHAR_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_MEMCHAR_DESCRIPTION_HU, "A memória tartalmának megjelenítése karakter grafikaként.  Ha csak egy cím\n"
                                           "adott, egyetlen karakter rajzolódik ki.  Ha nincs cím megadva,\n"
                                           "a ``pont'' címet használja."},
/* it */ {IDGS_MON_MEMCHAR_DESCRIPTION_IT, "Mosta il contenuto della memoria come dati caratteri. Se è specificato\n"
                                           "solo un indirizzo, verrà visualizzato solo un carattere. Se non è\n"
                                           "specificato alcun indirizzo, varrà utilizzato l'indirizzo ``dot'' ."},
/* nl */ {IDGS_MON_MEMCHAR_DESCRIPTION_NL, "Toon de geheugen inhoud als karakter data. Als er maar een adres is\n"
                                           "opgegeven dan zal er maar een karakter worden weergegeven. Als er geen\n"
                                           "adres is opgegeven dan zal het huidige adres gebruikt worden."},
/* pl */ {IDGS_MON_MEMCHAR_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_MEMCHAR_DESCRIPTION_SV, "Visa minnets innehåll som teckendata. Om endast en adress\n"
                                           "anges, visas endast ett tecken. Om ingen adress anges används\n"
                                           "\"punkt\"-adressen."},
/* tr */ {IDGS_MON_MEMCHAR_DESCRIPTION_TR, "Bellek içeriðini karakter verisi olarak göster.  Yalnýzca bir adres\n"
                                           "belirtilmiþse, yalnýzca tek karakter görüntülenir.  Eðer hiç adres\n"
                                           "girilmemiþse, 'nokta' adresi kullanýlýr."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_QP_FILENAME_P_FORMAT,    N_("\"<filename>\" <format>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_QP_FILENAME_P_FORMAT_DA, "\"<filnavn>\" <format>"},
/* de */ {IDGS_QP_FILENAME_P_FORMAT_DE, "\"<Dateiname>\" <Format>"},
/* fr */ {IDGS_QP_FILENAME_P_FORMAT_FR, ""},  /* fuzzy */
/* hu */ {IDGS_QP_FILENAME_P_FORMAT_HU, "\"<fájlnév>\" <formátum>"},
/* it */ {IDGS_QP_FILENAME_P_FORMAT_IT, "\"<nome del file>\" <formato>"},
/* nl */ {IDGS_QP_FILENAME_P_FORMAT_NL, "\"<betandsnaam>\" <formaat>"},
/* pl */ {IDGS_QP_FILENAME_P_FORMAT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_QP_FILENAME_P_FORMAT_SV, "\"<filnamn>\" <format>"},
/* tr */ {IDGS_QP_FILENAME_P_FORMAT_TR, "\"<dosyaismi>\" <biçim>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_MEMMAPSAVE_DESCRIPTION,    N_("Save the memmap as a picture. Format is:\n"
                                                 "0 = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_MEMMAPSAVE_DESCRIPTION_DA, "Gem memmap som billede. Format:\n"
                                              "0 = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
/* de */ {IDGS_MON_MEMMAPSAVE_DESCRIPTION_DE, "Speicher die memmap als Bilddaten. Formate:\n"
                                              "0 = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
/* fr */ {IDGS_MON_MEMMAPSAVE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_MEMMAPSAVE_DESCRIPTION_HU, "Memória térkép mentése képként. A formátum:\n"
                                              "0 = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
/* it */ {IDGS_MON_MEMMAPSAVE_DESCRIPTION_IT, "Salva la mappa di memoria come immagine. I Formati sono:\n"
                                              "0 = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
/* nl */ {IDGS_MON_MEMMAPSAVE_DESCRIPTION_NL, "Sla de memmap op als een afbeelding. Formaat is:\n"
                                                 "0 = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
/* pl */ {IDGS_MON_MEMMAPSAVE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_MEMMAPSAVE_DESCRIPTION_SV, "Sparar minneskartan som en bild. Format:\n"
                                              "0 = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
/* tr */ {IDGS_MON_MEMMAPSAVE_DESCRIPTION_TR, "Bellek haritasýný resim olarak kaydet. Biçim:\n"
                                              "0 = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_MASK_RP_ADDRESS_OPT_RANGE,    N_("[<mask>] [<address_opt_range>]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_MASK_RP_ADDRESS_OPT_RANGE_DA, "[<maske>] [<adresse_evt_område>]"},
/* de */ {IDGS_RP_MASK_RP_ADDRESS_OPT_RANGE_DE, "[<Maske>] [<Optionaler Adressbereich>]"},
/* fr */ {IDGS_RP_MASK_RP_ADDRESS_OPT_RANGE_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_MASK_RP_ADDRESS_OPT_RANGE_HU, "[<maszk>] [<címtartomány>]"},
/* it */ {IDGS_RP_MASK_RP_ADDRESS_OPT_RANGE_IT, "[<maschera>] [<intervallo di indirizzi opzionale>]"},
/* nl */ {IDGS_RP_MASK_RP_ADDRESS_OPT_RANGE_NL, "[<masker>] [<optionele_adres_reeks>]"},
/* pl */ {IDGS_RP_MASK_RP_ADDRESS_OPT_RANGE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_MASK_RP_ADDRESS_OPT_RANGE_SV, "[<mask>] [<adress_ev_område>]"},
/* tr */ {IDGS_RP_MASK_RP_ADDRESS_OPT_RANGE_TR, "[<maske>] [<opsiyonel_adres_aralýðý>]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_MEMMAPSHOW_DESCRIPTION,    N_("Show the memmap. The mask can be specified to show only those\n"
                                                 "locations with accesses of certain type(s). The mask is a number\n"
                                                 "with the bits \"ioRWXrwx\", where RWX are for ROM and rwx for RAM.\n"
                                                 "Optionally, an address range can be specified.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_MEMMAPSHOW_DESCRIPTION_DA, "Vis memmap. Masken kan angives for kun at vise de celler, som\n"
                                              "er tilgået med valgte typer tilgang. Masken er et tal med\n"
                                              "bitsne \"ioRWXrwx\", hvor RWX er ROM og rwx er RAM.\n"
                                              "Et adresseområde kan eventuelt angives."},
/* de */ {IDGS_MON_MEMMAPSHOW_DESCRIPTION_DE, "Zeige die memmap. Die Maske kann so definiert werden, dass nur jene Orte\n"
                                              "mit Zugriff auf spezifische Typ(en) gezeigt werden. Die Maske ist eine Nummer\n"
                                              "mit Bits \"ioRWXrwx\", wobei RWX für ROM und rwx für RAM steht.\n"
                                              "Optional kann ein Adressbereich definiert werden."},
/* fr */ {IDGS_MON_MEMMAPSHOW_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_MEMMAPSHOW_DESCRIPTION_HU, "A memória térkép mutatása. A maszk úgy állítható, hogy csak azokat a\n"
                                              "helyeket mutassa, amelyek a megadott eléréssel rendelkeznek. A maszk\n"
                                              "egy szám az \"ioRWXrwx\" bitekkel, ahol az RWX a ROM-ra, az rwx a RAM-ra\n"
                                              "vonatkozik. Tetszés szerint egy címtartomány is megadható."},
/* it */ {IDGS_MON_MEMMAPSHOW_DESCRIPTION_IT, "Mostra la mappa di memoria. La maschera può essere specificata per\n"
                                              "mostrare solo quelle locazioni con accessi un un certo tipo. La maschera\n"
                                              "è un numero con i bit \"ioRWXrwx\", dove RWX sono per la ROM e\n"
                                              "rwx per la RAM.\n"
                                              "Opzionalmente, può essere specificato anche un intervallo di indirizzi."},
/* nl */ {IDGS_MON_MEMMAPSHOW_DESCRIPTION_NL, "Toon de memmap. Het masker kan worden opgegeven zodat er alleen de\n"
                                              "adressen met toegrijpingen van een bepaalde soort(en) zal worden getoond.\n"
                                              "Het masker is een nummer met de bits \"ioRWXrwx\", waar RWX is voor ROM,\n"
                                              "en rwx voor RAM. Er kan optioneel een adres bereik worden opgegeven."},
/* pl */ {IDGS_MON_MEMMAPSHOW_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_MEMMAPSHOW_DESCRIPTION_SV, "Visa minneskartan. Masken kan anges för att endast visa de\n"
                                              "celler som gäller specifik(a) typ(er). Masken är ett tal\n"
                                              "med bitarna \"ioRWXrwx\", där RWX är för ROM och rwx är för\n"
                                              "RAM. Ett minnesområde kan eventuellt anges."},
/* tr */ {IDGS_MON_MEMMAPSHOW_DESCRIPTION_TR, "Bellek haritasýný göster. Maske, yalnýzca belirli tiplerce eriþilebilen\n"
                                              "konumlarda belirtilebilir. Maske \"ioRWXrwx\" bitlerinden oluþan bir\n"
                                              "sayýdýr. RWX ROM'u, rwx RAM'i ifade eder. Opsiyonel olarak bir adres\n"
                                              "aralýðý belirtilebilir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_MEMMAPZAP_DESCRIPTION,    N_("Clear the memmap.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_MEMMAPZAP_DESCRIPTION_DA, "Nulstil memmap."},
/* de */ {IDGS_MON_MEMMAPZAP_DESCRIPTION_DE, "Lösche die memmap."},
/* fr */ {IDGS_MON_MEMMAPZAP_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_MEMMAPZAP_DESCRIPTION_HU, "A memória térkép törlése."},
/* it */ {IDGS_MON_MEMMAPZAP_DESCRIPTION_IT, "Cancella la mappa di meoria."},
/* nl */ {IDGS_MON_MEMMAPZAP_DESCRIPTION_NL, "Veeg de memmap schoon."},
/* pl */ {IDGS_MON_MEMMAPZAP_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_MEMMAPZAP_DESCRIPTION_SV, "Töm minneskartan."},
/* tr */ {IDGS_MON_MEMMAPZAP_DESCRIPTION_TR, "Bellek haritasýný temizle."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_MEMSPRITE_DESCRIPTION,    N_("Display the contents of memory as sprite data.  If only one address is\n"
                                                "specified, only one sprite is displayed.  If no addresses are given,\n"
                                                "the ``dot'' address is used.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_MEMSPRITE_DESCRIPTION_DA, "Vis hukommelsens indhold som spritedata. Hvis kun en adresse\n"
                                             "angives, vises kun en sprite. Hvis ingen adresse angives bruges\n"
                                             "\"punktum\"-adressen."},
/* de */ {IDGS_MON_MEMSPRITE_DESCRIPTION_DE, "Zeige den Speicher als Sprite Daten. Wenn nur eine Adresse angegeben wurde,\n"
                                             "wird nur ein Sprite gezeitg. Wenn keine Adresse angegeben wurde, wird\n"
                                             "die aktuelle Adresse verwendet."},
/* fr */ {IDGS_MON_MEMSPRITE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_MEMSPRITE_DESCRIPTION_HU, "A memória tartalmának megjelenítése sprite adatkéne. Ha csak egy cím\n"
                                             "van megadva, egyetlen sprite-ot mutat.  Ha egyáltalán nincs cím megadva,\n"
                                             "az aktuális címet használja."},
/* it */ {IDGS_MON_MEMSPRITE_DESCRIPTION_IT, "Mostra il contenuto della memoria come dati sprite. Se è specificato\n"
                                             "solo un indirizzo, verrà mostrato solo uno sprite. Se non è specificato\n"
                                             "alcun indirizzo, varrà utilizzato l'indirizzo ``dot''."},
/* nl */ {IDGS_MON_MEMSPRITE_DESCRIPTION_NL, "Toon de geheugen inhoud als sprite data. Als er maar een adres is opgegeven\n"
                                             "dan zal er maar een sprite worden getoond. Als er geen adres is opgegeven\n"
                                             "dan zal het huidige adres worden gebruikt."},
/* pl */ {IDGS_MON_MEMSPRITE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_MEMSPRITE_DESCRIPTION_SV, "Visa minnets innehåll som spritedata. Om endast en adress\n"
                                             "anges, visas endast en sprite. Om ingen adress anges används\n"
                                             "\"punkt\"-adressen."},
/* tr */ {IDGS_MON_MEMSPRITE_DESCRIPTION_TR, "Bellek içeriðini yaratýk verisi olarak göster.  Yalnýzca bir adres\n"
                                             "belirtilmiþse, yalnýzca tek yaratýk görüntülenir.  Eðer hiç adres\n"
                                             "girilmemiþse, 'nokta' adresi kullanýlýr."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_MOVE_DESCRIPTION,    N_("Move memory from the source specified by the address range to\n"
                                           "the destination specified by the address.  The regions may overlap.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_MOVE_DESCRIPTION_DA, "Kopiér data fra kilden angivet af hukommelsesområdet til destinationen\n"
                                        "som angives af adressen. Regionerne kan overlappe."},
/* de */ {IDGS_MON_MOVE_DESCRIPTION_DE, "Verschiebe Speicher von Startadresse definiert durch den Adressbereich zum\n"
                                        "Zielbereich definiert durch die Adresse. Die Bereiche können überlappend sein."},
/* fr */ {IDGS_MON_MOVE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_MOVE_DESCRIPTION_HU, "Memória másolása a forrás területrõl a cél cím által meghatározott\n"
                                        "területre. A tartományok átfedése megengedett."},
/* it */ {IDGS_MON_MOVE_DESCRIPTION_IT, "Sposta la memoria dalla sorgente specificata dall'intervallo di indirizzi\n"
                                        "alla destinazione specificata dall'indirizzo. Le regioni possono\n"
                                        "sovrapporsi."},
/* nl */ {IDGS_MON_MOVE_DESCRIPTION_NL, "Verplaats data van de opgegeven bron adres reeks naar het doel adres. De\n"
                                        "reeksen kunnen overlappen."},
/* pl */ {IDGS_MON_MOVE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_MOVE_DESCRIPTION_SV, "Flytta minne från källadressen som anges av minnesområdet\n"
                                        "till destinationen som anges av adressen. Regionerna kan vara\n"
                                        "överlappande."},
/* tr */ {IDGS_MON_MOVE_DESCRIPTION_TR, "Belleði belirtilmiþ kaynak adres aralýðýndan belirtilmiþ hedef\n"
                                        "adresine taþý.  Alanlar üst üste binebilir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_CHECKPOINT_COMMANDS,    N_("Checkpoint commands:")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_CHECKPOINT_COMMANDS_DA, "Checkpoint-kommandoer:"},
/* de */ {IDGS_CHECKPOINT_COMMANDS_DE, "Checkpoint Kommandos:"},
/* fr */ {IDGS_CHECKPOINT_COMMANDS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_CHECKPOINT_COMMANDS_HU, "Ellenõrzési pont parancsok:"},
/* it */ {IDGS_CHECKPOINT_COMMANDS_IT, "Comandi punti di controllo:"},
/* nl */ {IDGS_CHECKPOINT_COMMANDS_NL, "Checkpunt commandos:"},
/* pl */ {IDGS_CHECKPOINT_COMMANDS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CHECKPOINT_COMMANDS_SV, "Kommandon för brytpunkter:"},
/* tr */ {IDGS_CHECKPOINT_COMMANDS_TR, "Kontrol Noktasý komutlarý:"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_ADDRESS_RP_COND_EXPR,    N_("[<address> [if <cond_expr>] ]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_ADDRESS_RP_COND_EXPR_DA, "[<adresse> [if <betingelse>] ]"},
/* de */ {IDGS_RP_ADDRESS_RP_COND_EXPR_DE, "[<Adresse> [if <bedingter Ausdruck>] ]"},
/* fr */ {IDGS_RP_ADDRESS_RP_COND_EXPR_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_ADDRESS_RP_COND_EXPR_HU, "[<cím> [if <felt_kif>] ]"},
/* it */ {IDGS_RP_ADDRESS_RP_COND_EXPR_IT, "[<indirizzo> [if <espressione condizione>] ]"},
/* nl */ {IDGS_RP_ADDRESS_RP_COND_EXPR_NL, "[<adres> [ald <cond_expr>] ]"},
/* pl */ {IDGS_RP_ADDRESS_RP_COND_EXPR_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_ADDRESS_RP_COND_EXPR_SV, "[<adress> [om <villkorsuttryck>] ]"},
/* tr */ {IDGS_RP_ADDRESS_RP_COND_EXPR_TR, "[<adres> [if <koþul_idafesi>] ]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_BREAK_DESCRIPTION,    N_("If no address is given, the currently valid watchpoints are printed.\n"
                                            "If an address is given, a breakpoint is set for that address and the\n"
                                            "breakpoint number is printed.  A conditional expression can also be\n"
                                            "specified for the breakpoint.  For more information on conditions, see\n"
                                            "the CONDITION command.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_BREAK_DESCRIPTION_DA, "Hvis ingen adresse angives vises nuværende watchpoints. Hvis en\n"
                                         "adresse angives sættes et watchpoint på denne och watchpointets\n"
                                         "nummer vises. En betingelse kan også angives for et watchpoint.\n"
                                         "For mere information om betingelser, se kommandoen CONDITION."},
/* de */ {IDGS_MON_BREAK_DESCRIPTION_DE, "Wenn keine Adresse angegeben wurde, werden die aktuellen watchpoints gezeigt.\n"
                                         "Wenn eine Adresse angegebene wurde, wird ein breakpoint auf diese Adresse\n"
                                         "gesetzt und die breakpoint Nummer wird ausgegeben. Ein bedingter Ausdruck\n"
                                         "can zu dem breakpoint zugeordnet werden. Für mehr Informations über \n"
                                         "bedingte Ausdrücke, bitte Kommando CONDITION ansehen."},
/* fr */ {IDGS_MON_BREAK_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_BREAK_DESCRIPTION_HU, "Ha nincs cím megadva, az aktuálisan érvényes figyelési pontokat\n"
                                         "listázza. Ha van cím a paraméterben, töréspontot helyez el az adott\n"
                                         "címre, és a számát kiírja. Egy feltétel kifejezés is megadható a törésponthoz.\n"
                                         "A feltételekrõl több információhoz lásd a CONDITION parancs súgóját!"},
/* it */ {IDGS_MON_BREAK_DESCRIPTION_IT, "Se non viene specificato alcun indirizzo, i watchpoint attuali vengono\n"
                                         "stampati.Se viene specificato un indirizzo, viene impostato un breakpoint\n"
                                         "per tale indirizzo e viene stampato il suo numero. Può anche essere\n"
                                         "specificata una condizione per il breakpoint. Per ulteriori informazioni,\n"
                                         "vedere il comando CONDITION."},
/* nl */ {IDGS_MON_BREAK_DESCRIPTION_NL, "Als er geen adres is opgegeven dan zullen de huidige geldige kijkpunten\n"
                                         "geprint. Als er een adres is opgegeven dan zal er een breekpunt gezet worden\n"
                                         "voor dat adres en de nummer van het breekpunt zal worden getoond. Een\n"
                                         "conditie kan worden opgegeven voor het breekpunt. Voor meer informatie over\n"
                                         "condities zie het CONDITION commando."},
/* pl */ {IDGS_MON_BREAK_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_BREAK_DESCRIPTION_SV, "Om ingen adress anges visas de övervakningspunkter som är giltiga.\n"
                                         "Om en adress anges sätts en brytpunkt på den adressen och dess\n"
                                         "nummer visas. Ett villkorsuttryck kan också anges för brytpunkten.\n"
                                         "För mer information om villkor, se kommandot CONDITION."},
/* tr */ {IDGS_MON_BREAK_DESCRIPTION_TR, "Eðer adres girilmemiþse, geçerli mevcut izleme noktalarý yazdýrýlýr.\n"
                                         "Eðer bir adres girilmiþse, o adres için bir kesme noktasý iþaretlenir\n"
                                         "ve kesme noktasý sayýsý yazdýrýlýr.  Ayný zamanda kesme noktasý için bir\n"
                                         "koþul ifadesi de belirtilebilir.  Koþullar ile ilgili daha çok bilgi almak\n"
                                         "için, CONDITION komutuna bakabilirsiniz."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_CHECKNUM_QP_COMMAND,    N_("<checknum> \"<command>\"")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_CHECKNUM_QP_COMMAND_DA, "<checknum> \"<kommando>\""},
/* de */ {IDGS_P_CHECKNUM_QP_COMMAND_DE, "<Checknum> \"<Kommando>\""},
/* fr */ {IDGS_P_CHECKNUM_QP_COMMAND_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_CHECKNUM_QP_COMMAND_HU, "<ell_pont_szám> \"<parancs>\""},
/* it */ {IDGS_P_CHECKNUM_QP_COMMAND_IT, "<checknum> \"<comando>\""},
/* nl */ {IDGS_P_CHECKNUM_QP_COMMAND_NL, "<checknum> \"<commando>\""},
/* pl */ {IDGS_P_CHECKNUM_QP_COMMAND_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_CHECKNUM_QP_COMMAND_SV, "<kontrollnr> \"<kommando>\""},
/* tr */ {IDGS_P_CHECKNUM_QP_COMMAND_TR, "<kontrolnumarasý> \"<komut>\""},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_COMMAND_DESCRIPTION,    N_("Specify `command' as the command to execute when checkpoint `checknum'\n"
                                              "is hit.  Note that the `x' command is not yet supported as a\n"
                                              "command argument.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_COMMAND_DESCRIPTION_DA, "Angiv \"kommando\" som kommando der skal eksekveres når ckeckpoint\n"
                                           "\"checknum\" nås. Vær opmærksom på at kommandoen \"x\" endnu ikke\n"
                                           "er understøttet som parameter."},
/* de */ {IDGS_MON_COMMAND_DESCRIPTION_DE, "Definiere `Kommando', welches bei Checkpoint `Checknum' ausgeführt wird.\n"
                                           "Achtung Kommando `x' wird zurzeit noch nicht als Kommando Argument\n"
                                           "unterstützt."},
/* fr */ {IDGS_MON_COMMAND_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_COMMAND_DESCRIPTION_HU, "`parancs' megadása egy `ell_pont_szám' sorszámával adott ellenõrzési pont\n"
                                           "elérésekor történõ végrehajtásra.  Az `x' parancs egyelõre nem használható\n"
                                           "itt paraméterként."},
/* it */ {IDGS_MON_COMMAND_DESCRIPTION_IT, "Specifica `comando' come il comando da eseguire quando\n"
                                           "viene incontrato il punto di controllo `checknum'. Nota che \n"
                                           "il comando `x' non è ancora supportato come parametro."},
/* nl */ {IDGS_MON_COMMAND_DESCRIPTION_NL, "Geef `commando' als het command om uit te voeren wanneer checkpunt `checknum'\n"
                                           "wordt uitgevoerd. NB: het `x' commando is op dit moment geen geldige\n"
                                           "parameter."},
/* pl */ {IDGS_MON_COMMAND_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_COMMAND_DESCRIPTION_SV, "Ange \"kommando\" som kommando att exekvera när vid träff på\n"
                                           "kontrollpunkten·\"kontrollnr\". Observera att kommandot \"x\" ännu\n"
                                           "inte stöds som kommandoparameter."},
/* tr */ {IDGS_MON_COMMAND_DESCRIPTION_TR, "Kontrol noktasý `kontrolnumarasý'na ulaþýldýðýnda `komut' olarak\n"
                                           "çalýþtýrýlacak komutu belirt.  Henüz `x' komutunun bir komut argümaný\n"
                                           "olarak desteklenmediðini not edin."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_CHECKNUM_IF_P_COND_EXPR,    N_("<checknum> if <cond_expr>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_CHECKNUM_IF_P_COND_EXPR_DA, "<checknum> if <betingelse>"},
/* de */ {IDGS_P_CHECKNUM_IF_P_COND_EXPR_DE, "<Checknum> if <bedingter Ausdruck>"},
/* fr */ {IDGS_P_CHECKNUM_IF_P_COND_EXPR_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_CHECKNUM_IF_P_COND_EXPR_HU, "<ell_pont_szám> if <kifejezés>"},
/* it */ {IDGS_P_CHECKNUM_IF_P_COND_EXPR_IT, "<checknum> if <espressione condizione>"},
/* nl */ {IDGS_P_CHECKNUM_IF_P_COND_EXPR_NL, "<checknum> als <cond_expr>"},
/* pl */ {IDGS_P_CHECKNUM_IF_P_COND_EXPR_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_CHECKNUM_IF_P_COND_EXPR_SV, "<kontrollnum> if <villkorsuttryck>"},
/* tr */ {IDGS_P_CHECKNUM_IF_P_COND_EXPR_TR, "<kontrolnumarasý> if <koþul_idafesi>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_CONDITION_DESCRIPTION,    N_("Each time the specified checkpoint is examined, the condition is\n"
                                                "evaluated.  If it evalutes to true, the checkpoint is activated.\n"
                                                "Otherwise, it is ignored.  If registers are specified in the expression,\n"
                                                "the values used are those at the time the checkpoint is examined, not\n"
                                                "when the condition is set.\n"
                                                "The condition can make use of registers (.A, .X, .Y, .PC, .SP) and\n"
                                                "compare them (==, !=, <, >, <=, >=) again other registers or constants.\n"
                                                "Registers can be the registers of other devices; this is denoted by\n"
                                                "a memspace prefix (i.e., c:, 8:, 9:, 10:, 11:\n"
                                                "Examples: .A == 0, .X == .Y, 8:.X == .X\n")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_CONDITION_DESCRIPTION_DA, "Betingelsen evalueres hver gang det angivne checkpoint undersøges.\n"
                                             "Hvis evalueres til sand aktiveres checkpointet, allers ignoreres\n"
                                             "det. Hvis betingelsen indeholder registre, anvendes de værdier,\n"
                                             "registrene har når betingelsen evalueres, ikke når betingelsen\n"
                                             "oprettes.\n"
                                             "Betingelsen kan anvende registrene (.A, .X, .Y, .PC, .SP) og\n"
                                             "sammenligne dem (==, !=, <, >, <=, >=) med andre registre eller\n"
                                             "konstanter. Registre kan være i andre enheder, og dette angives\n"
                                             "som et hukommelsesområdeprefix (dvs., c:, 8:, 9:, 10:, 11:)\n"
                                             "Eksempel: .A == 0, .X == .Y, 8:.X == .X\n"},
/* de */ {IDGS_MON_CONDITION_DESCRIPTION_DE, "Bei jeder Überprüfung des Checkpoints, wird die Bedingung evaluiert.\n"
                                             "Ist das Ergebnis `Wahr', wird der Checkpoint aktiviert, sonst ingnoriert.\n"
                                             "Wenn Register benutzt werden, werden deren Werte zum Zeitpunkt der \n"
                                             "Überprüfung herangezogen.\n"
                                             "Die Bedingung kann Register benutzen (.A, .X, .Y, .PC, .SP) und gegen \n"
                                             "Register oder Konstanten vergleichen (==, !=, <, >, <=, >=).\n"
                                             "Register können auch Geräteregister sein; dazu wird die memspace Prefix-syntax \n"
                                             "verwendet: c:, 8:, 9:, 10:, 11:\n"
                                             "Beispiel: .A == 0, .X == .Y, 8:.X == .X\n"},
/* fr */ {IDGS_MON_CONDITION_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_CONDITION_DESCRIPTION_HU, "Minden alkalommal, amikor egy adott ellenõrzési pontot megvizsgál, a\n"
                                             "feltételt kiértékeli. Ha az eredmény igaz, a töréspont aktiválódik.\n"
                                             "Egyébként, figyelmen kívül hagyja. Ha regiszterek szerepelnek a kifejezésben,\n"
                                             "az ellenõrzés pillanatában vett értékük számít, nem pedig a beállításkori.\n"
                                             "A feltétel használhatja a regisztereket ( .A, .X, .Y, .PC, .SP) és össze is\n"
                                             "hasonlíthatja õket (==, !=, <, >, <=, >=) más regiszterekkel.\n"
                                             "A regiszterek lehetnek más eszközök regiszterei is; azokat egy memória tér\n"
                                             "elõtaggal kell ellátni (vagyis c:, 8:, 9:, 10:, 11:).\n"
                                             "Példák: .A == 0, .X == .Y, 8:.X == .X\n"},
/* it */ {IDGS_MON_CONDITION_DESCRIPTION_IT, "Ogni volta che si esamina il punto di controllo specificato, viene valutata\n"
                                             "la condizione. Se è vera, il punto di controllo è attivato.\n"
                                             "Altrimenti viene ignorato. Se nell'espressione sono specificati i registri,\n"
                                             "questi valori sono quelli usati quando viene esaminato il punto di controllo,\n"
                                             "non quando viene impostata la condizione.\n"
                                             "La condizione può fare uso dei registri (.A, .X, .Y, .PC, .SP) e confontarli\n"
                                             "(==, !=, <, >, <=, >=) con altri registri o costanti.\n"
                                             "I registri possono essere registri di altre periferiche; questo è denotato da\n"
                                             "un prefisso che specifica lo spazio di memoria (cioé, c:, 8:, 9:, 10:, 11:)\n"
                                             "Alcuni esempi: .A == 0, .X == .Y, 8:.X == .X\n"},
/* nl */ {IDGS_MON_CONDITION_DESCRIPTION_NL, "Elke keer de opgegeven checkpunt wordt onderzocht zal de conditie worden\n"
                                             "geevalueerd. Als de evaluatie positief is dan zal de checkpunt geactiveerd\n"
                                             "worden, bij negatief zal er niets worden gedaan. Als er registers zijn\n"
                                             "opgegeven in de expressie dan zullen de waarden in die registers gebruikt\n"
                                             "worden op het moment dat de checkpunt wordt onderzocht, niet op het moment\n"
                                             "dat de conditie is opgegeven.\n"
                                             "De conditie kan gebruik maken van registers (.A, .X, .Y, .PC, .SP) en\n"
                                             "kan ze vergelijken (==, !=, <, >, <=, >=) met andere registers of constanten\n"
                                             "Registers kunnen de registers van andere apparaten zijn; de manier van opgeven\n"
                                             "is door een geheugenruimte prefix (bijvoorbeeld: c:, 8:, 9:, 10:, 11:\n"
                                             "Voorbeelden: .A == 0, .X == .Y, 8:.X == .X\n"},
/* pl */ {IDGS_MON_CONDITION_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_CONDITION_DESCRIPTION_SV, "Villkoret utvärderas varje gång den givna kontrollpunkten\n"
                                             "undersöks. Om det utvärderas till sant aktiveras kontrollpunkten.\n"
                                             "I annat fall ignoreras den. Om uttrycket innehåller register\n"
                                             "används de värden de har då kontrollpunkten utvärderas, inte\n"
                                             "när villkoret sattes.\n"
                                             "Villkoret kan använda register (.A, .X, .Y, .PC, .SP) och\n"
                                             "jämföra dem (==, !=, <, >, <=, >=) mot andra register eller\n"
                                             "konstanter. Register kan vara register i andra enheter, detta\n"
                                             "anges som ett minnesområdesprefix (dvs., c:, 8:, 9:, 10:, 11:)\n"
                                             "Exempel: .A == 0, .X == .Y, 8:.X == .X\n"},
/* tr */ {IDGS_MON_CONDITION_DESCRIPTION_TR, "Her defasýnda belirtilmiþ bir kontrol noktasý incelendiðinde, koþul\n"
                                             "deðerlendirilir.  Koþul doðru sonuç verirse, kontrol noktasý aktive edilir.\n"
                                             "Aksi taktirde göz ardý edilir.  Eðer ifadede registerlar tanýmlanmýþsa,\n"
                                             "bu deðerler ifade tanýmlandýðýnda deðil kontrol noktasý incelendiðinde\n"
                                             "kullanýlýrlar.\n"
                                             "Ýfadelerde (.A, .X, .Y, .PC, .SP) registerlarý kullanýlabilir ve\n"
                                             "(==, !=, <, >, <=, >=) opertatörleriyle diðer register ve sabit deðerlerle.\n"
                                             "karþýlaþtýrýlabilirler. Registerlar baþka aygýtlarýn registerlarý da olabilir;\n"
                                             "bu bir bellek alaný ön ekiyle ifade edilir (yani, c:, 8:, 9:, 10:, 11:\n"
                                             "Örnekler: .A == 0, .X == .Y, 8:.X == .X\n"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_DISABLE_DESCRIPTION,    N_("Disable checkpoint `checknum'.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_DISABLE_DESCRIPTION_DA, "Deaktivér checkpoint \"checknum\"."},
/* de */ {IDGS_MON_DISABLE_DESCRIPTION_DE, "Checkpoint `Checknum' deaktivieren."},
/* fr */ {IDGS_MON_DISABLE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_DISABLE_DESCRIPTION_HU, "`ell_pont_szám' jelû ellenõrzési pont tiltása."},
/* it */ {IDGS_MON_DISABLE_DESCRIPTION_IT, "Disattiva il punto di controllo `checknum'."},
/* nl */ {IDGS_MON_DISABLE_DESCRIPTION_NL, "Checkpunt `checknum' uitschakelen."},
/* pl */ {IDGS_MON_DISABLE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_DISABLE_DESCRIPTION_SV, "Inaktivera kontrollpunkt \"kontrollnr\"."},
/* tr */ {IDGS_MON_DISABLE_DESCRIPTION_TR, "Kontrol Noktasý `kontrolnumarasý'ný pasifleþtir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_ENABLE_DESCRIPTION,    N_("Enable checkpoint `checknum'.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_ENABLE_DESCRIPTION_DA, "Aktivér checkpoint \"checknum\"."},
/* de */ {IDGS_MON_ENABLE_DESCRIPTION_DE, "Checkpoint `Checknum' aktivieren."},
/* fr */ {IDGS_MON_ENABLE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_ENABLE_DESCRIPTION_HU, "`ell_pont_szám' jelû ellenõrzési pont engedélyezése."},
/* it */ {IDGS_MON_ENABLE_DESCRIPTION_IT, "Attiva il punto di controllo `checknum'."},
/* nl */ {IDGS_MON_ENABLE_DESCRIPTION_NL, "Activeer checkpunt `checknum'."},
/* pl */ {IDGS_MON_ENABLE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_ENABLE_DESCRIPTION_SV, "Aktivera kontrollpunkt \"kontrollnr\"."},
/* tr */ {IDGS_MON_ENABLE_DESCRIPTION_TR, "Kontrol Noktasý `kontrolnumarasý'ný aktif et."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_CHECKNUM_RP_COUNT,    N_("<checknum> [<count>]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_CHECKNUM_RP_COUNT_DA, "<checknum> [<antal>]"},
/* de */ {IDGS_P_CHECKNUM_RP_COUNT_DE, "<Checknum> [<Zähler>]"},
/* fr */ {IDGS_P_CHECKNUM_RP_COUNT_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_CHECKNUM_RP_COUNT_HU, "<ell_pont_szám> [<darab>]"},
/* it */ {IDGS_P_CHECKNUM_RP_COUNT_IT, "<checknum> [<contatore>]"},
/* nl */ {IDGS_P_CHECKNUM_RP_COUNT_NL, "<checknum> [<aantal>]"},
/* pl */ {IDGS_P_CHECKNUM_RP_COUNT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_CHECKNUM_RP_COUNT_SV, "<kontrollnum> [<antal>]"},
/* tr */ {IDGS_P_CHECKNUM_RP_COUNT_TR, "<kontrolnumarasý> [<adet>]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_IGNORE_DESCRIPTION,    N_("Ignore a checkpoint a given number of crossings.  If no count is given,\n"
                                             "the default value is 1.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_IGNORE_DESCRIPTION_DA, "Ignorér et checkpoint det angivne antal gange. Hvis antal ikke angives\n"
                                          "bruges standardværdien 1."},
/* de */ {IDGS_MON_IGNORE_DESCRIPTION_DE, "Ignoriere den Checkpoint für die angegebene Anzahl an Aktivierungen.\n"
                                          "Wenn kein `Zähler' angegeben wurde, wird der Standardwert 1 verwendet."},
/* fr */ {IDGS_MON_IGNORE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_IGNORE_DESCRIPTION_HU, "Az ellenõrzési pont figyelmen kívül hagyása megadott számú áthaladáskor.  Ha nincs\n"
                                          "szám megadva, az alapértelmezett érték 1."},
/* it */ {IDGS_MON_IGNORE_DESCRIPTION_IT, "Ignora un punto di controllo un numero specificato di occorrenze. Se\n"
                                          "non è specificato alcun valore, quello predefinito è 1."},
/* nl */ {IDGS_MON_IGNORE_DESCRIPTION_NL, "Negeer een checkpunt een opgegeven aantal keer. Als er geen aantal is\n"
                                          "opgegeven dan wordt de standaard waarde van 1 worden gebruikt."},
/* pl */ {IDGS_MON_IGNORE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_IGNORE_DESCRIPTION_SV, "Ignorera en kontrollpunkt det givna antalet gånger. Om inget antal ges\n"
                                          "är standardvärdet 1."},
/* tr */ {IDGS_MON_IGNORE_DESCRIPTION_TR, "Bir kontrol noktasýný verilen adette geçiþ için yoksay.  Eðer adet verilmemiþse,\n"
                                          "varsayýlan deðer 1'dir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_ADDRESS,    N_("[<address>]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_ADDRESS_DA, "[<adresse>]"},
/* de */ {IDGS_RP_ADDRESS_DE, "[<Adresse>]"},
/* fr */ {IDGS_RP_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_ADDRESS_HU, "[<cím>]"},
/* it */ {IDGS_RP_ADDRESS_IT, "[<indirizzo>]"},
/* nl */ {IDGS_RP_ADDRESS_NL, "[<adres>]"},
/* pl */ {IDGS_RP_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_ADDRESS_SV, "[<adress>]"},
/* tr */ {IDGS_RP_ADDRESS_TR, "[<adres>]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_UNTIL_DESCRIPTION,    N_("If no address is given, the currently valid breakpoints are printed.\n"
                                            "If an address is given, a temporary breakpoint is set for that address\n"
                                            "and the breakpoint number is printed.  Control is returned to the\n"
                                            "emulator by this command.  The breakpoint is deleted once it is hit.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_UNTIL_DESCRIPTION_DA, "Hvis ingen adresse angives vises de aktuelle breakpoints. Hvis en\n"
                                         "adresse angives sættes et midlertidigt breakpoint, og nummeret på\n"
                                         "breakpointet vises. Kontrollen returneres til emulatoren af denne\n"
                                         "kommando. Breakpointet fjernes når det nås."},
/* de */ {IDGS_MON_UNTIL_DESCRIPTION_DE, "Wenn keine Adresse angegeben wurde, werden die aktuell gültigen \n"
                                         "Breakpoints angezeigt. Wenn eine Adresse angegeben wurde, wird ein \n"
                                         "temporärer Breakpoint auf dieser Adresse gesetzt und die Breakpoint Nummer wird\n"
                                         "angezeigt. Die Kontrolle wirdd bei diesem Kommando an die Emulation gegeben.\n"
                                         "Der Breakpoint wird gelöscht, wenn er einmal getroffen wurde."},
/* fr */ {IDGS_MON_UNTIL_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_UNTIL_DESCRIPTION_HU, "Ha nincs cím megadva, a jelenleg érvényes töréspontokat írja ki.\n"
                                         "Ha van adott cím, egy ideiglenes töréspontot helyez el az adott címre,\n"
                                         "és a töréspont száma kiíródik.  A vezérlés visszakerül az emulátorhoz\n"
                                         "e parancs által. A töréspont elérésekor törlõdik."},
/* it */ {IDGS_MON_UNTIL_DESCRIPTION_IT, "Se non viene specificato alcun indirizzo, vengono stampati i\n"
                                         "breakpoint attualmente validi. Se viene specificato un indirizzo,\n"
                                         "viene impostato un breakpoint temporaneo per tale indirizzo e\n"
                                         "viene stampato il suo numero. Il controllo ritorna all'emulatore\n"
                                         "per mezzo di questo comando. Il breakpoint è cancellato quando\n"
                                         "viene incontrato."},
/* nl */ {IDGS_MON_UNTIL_DESCRIPTION_NL, "Als er geen adres is opgegeven dan zullen de huidig geldige breekpunten\n"
                                         "worden geprint. Als er een adres is opgegeven dan wordt er een tijdelijk\n"
                                         "breekpunt worden gezet voor dat adres en het breekpunt nummer zal worden\n"
                                         "getoond. Na het geven van dit commando zal er terug worden gegaan naar de\n"
                                         "emulator. Het breekpunt zal worden verwijdert na het uitvoeren."},
/* pl */ {IDGS_MON_UNTIL_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_UNTIL_DESCRIPTION_SV, "Om du inte anger någon adress visas de för närvarande giltiga\n"
                                         "brytpunkterna. Om en adress anges sätts en temporär brytpunkt för\n"
                                         "den adressen och numret på den nya brytpunkten visas. Styrningen\n"
                                         "återlämnas till emulatorn för det här kommandot. Brytpunkten\n"
                                         "tas bort när den har nåtts."},
/* tr */ {IDGS_MON_UNTIL_DESCRIPTION_TR, "Eðer adres girilmemiþse, geçerli mevcut kesme noktalarý yazdýrýlýr.\n"
                                         "Eðer bir adres girilmiþse, o adres için geçici bir kesme noktasý\n"
                                         "iþaretlenir ve kesme noktasý sayýsý yazdýrýlýr.  Kontrol bu komutla\n"
                                         "emülatöre döndürülür.  Bir kere eriþildiði zaman kesme noktasý silinir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_R_LOADSTORE_R_ADDRESS_R_ADDRESS,    N_("[loadstore] [address [address]]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_R_LOADSTORE_R_ADDRESS_R_ADDRESS_DA, "[loadstore] [adresse [adresse]]"},
/* de */ {IDGS_R_LOADSTORE_R_ADDRESS_R_ADDRESS_DE, "[lade/speichere] [Adresse [Adresse]]"},
/* fr */ {IDGS_R_LOADSTORE_R_ADDRESS_R_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_R_LOADSTORE_R_ADDRESS_R_ADDRESS_HU, "[loadstore] [cím [cím]]"},
/* it */ {IDGS_R_LOADSTORE_R_ADDRESS_R_ADDRESS_IT, "[loadstore] [indirizzo [indirizzo]]"},
/* nl */ {IDGS_R_LOADSTORE_R_ADDRESS_R_ADDRESS_NL, "[laad/opslaan] [adres [adres]]"},
/* pl */ {IDGS_R_LOADSTORE_R_ADDRESS_R_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_R_LOADSTORE_R_ADDRESS_R_ADDRESS_SV, "[loadstore] [adress [adress]]"},
/* tr */ {IDGS_R_LOADSTORE_R_ADDRESS_R_ADDRESS_TR, "[yükle/depola] [adres [adres]]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_WATCH_DESCRIPTION,    N_("Set a watchpoint.  If a single address is specified, set a watchpoint\n"
                                            "for that address.  If two addresses are specified, set a watchpoint\n"
                                            "for the memory locations between the two addresses.\n"
                                            "`loadstore' is either `load' or `store' to specify on which operation\n"
                                            "the monitor breaks. If not specified, the monitor breaks on both\n"
                                            "operations.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_WATCH_DESCRIPTION_DA, "Sæt et watchpoint. Hvis der angives en enkelt adresse sættes et\n"
                                         "watchpoint for adressen. Hvis der angives to adresser sættes et\n"
                                         "watchpoint for hukommelsesområdet mellem adresserne.\n"
                                         "\"loadstore\" er enten \"load\" eller \"store\" og angiver hvilken\n"
                                         "operation emulatoren skal stoppe ved. Hvis loadstore ikke angives\n"
                                         "stopper emulatoren på begge operationer."},
/* de */ {IDGS_MON_WATCH_DESCRIPTION_DE, "Setze einen Watchpoint. Wenn eine einzelne Adresse angegeben wurde, setze \n"
                                         "den Watchpoint für diese Adresse. Wenn zwei Adressen angegeben wurden, \n"
                                         "setze den Watchpoint auf die Adressen zwischen den beiden Adressen.\n"
                                         "`loadstore' ist entweder `load' oder `store' und bezeichnet die Operation, \n"
                                         "bei welcher der Monitor anhalten soll. Wenn keine Angabe gemacht wurde, \n"
                                         "hält der Monitor bei beiden Operationen an."},
/* fr */ {IDGS_MON_WATCH_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_WATCH_DESCRIPTION_HU, "Figyelési pont megadása.  Ha egy cím van a paraméterben, az adott\n"
                                         "címre állítbe figyelési pontot. Ha két cím adott, akkor a kettõ közötti\n"
                                         "összes memória helyre beállítja.\n"
                                         "A `loadstore' vagy `load' vagy `store' lehet, azt adja meg, hogy olvasás\n"
                                         "vagy írás hatására áll meg a monitor program. Ha nincs megadva,\n"
                                         "mindkét mûvelet hatására megáll."},
/* it */ {IDGS_MON_WATCH_DESCRIPTION_IT, "Imposta un watchpoint. Se viene specificato solo un indirizzo, imposta\n"
                                         "un watchpoint per tale indirizzo. Se vengono specificati due indirizzi,\n"
                                         "imposta un watchpoint per le locazioni di memoria comprese tra i due\n"
                                         "indirizzi.\n"
                                         "`loadstore' può essere `load' o `store' per specificare su quale\n"
                                         "operazione il monitor si interrompe. Se non viene specificato, il monitor\n"
                                         "si interrompe su entrambe le operazioni."},
/* nl */ {IDGS_MON_WATCH_DESCRIPTION_NL, "Zet een kijkpunt. Als er een enkel adres is opgegeven dan zal er een\n"
                                         "kijkpunt worden gezet voor dat adres. Als er twee adressen zijn opgegeven\n"
                                         "dan zal er een kijkpunt worden gezet voor de adressen tussen de twee\n"
                                         "adressen.\n"
                                         "laad/opslaan is 'load' of 'store' om aan te geven bij welke operatie de\n"
                                         "monitor zou moeten starten. Als deze niet opgegeven is dan zal de monitor\n"
                                         "starten bij beide operaties."},
/* pl */ {IDGS_MON_WATCH_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_WATCH_DESCRIPTION_SV, "Sätt en övervakningspunkt. Om du anger en ensam adress sätts en\n"
                                         "övervakningspunkt för adressen. Om två adresser anges sätts en\n"
                                         "övervakningspunkt för minnesområdet mellan adresserna.\n"
                                         "\"loadstore\" är antingen \"load\" eller \"store\" och anger vilken\n"
                                         "operation emulatorn skall stoppa för. Om den inte anges stoppar\n"
                                         "emulatorn på bägge operationerna."},
/* tr */ {IDGS_MON_WATCH_DESCRIPTION_TR, "Bir izleme noktasý koy.  Eðer tek bir adres belirtilmiþse, izleme\n"
                                         "noktasýný o adrese koy.  Eðer iki adres belirtilmiþse, izleme noktasýný\n"
                                         "o iki adres arasýnda kalan adreslere koy.\n"
                                         "`yükle/depola' monitörün kesildiði operatöre baðlý olarak `yükle' ya da\n"
                                         "`depola' iþlemlerinde çalýþabilir. Eðer belirtilmemiþse, her iki iþlemde de\n"
                                         "monitör kesilecektir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_R_ADDRESS_R_ADDRESS,    N_("[address [address]]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_R_ADDRESS_R_ADDRESS_DA, "[adresse [adresse]]"},
/* de */ {IDGS_R_ADDRESS_R_ADDRESS_DE, "[Adresse [Adresse]]"},
/* fr */ {IDGS_R_ADDRESS_R_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_R_ADDRESS_R_ADDRESS_HU, "[cím [cím]]"},
/* it */ {IDGS_R_ADDRESS_R_ADDRESS_IT, "[indirizzo [indirizzo]]"},
/* nl */ {IDGS_R_ADDRESS_R_ADDRESS_NL, "[adres [adres]]"},
/* pl */ {IDGS_R_ADDRESS_R_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_R_ADDRESS_R_ADDRESS_SV, "[adress [adress]]"},
/* tr */ {IDGS_R_ADDRESS_R_ADDRESS_TR, "[adres [adres]]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_TRACE_DESCRIPTION,    N_("Set a tracepoint.  If a single address is specified, set a tracepoint\n"
                                            "for that address.  If two addresses are specified, set a tracepoint\n"
                                            "for the memory locations between the two addresses.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_TRACE_DESCRIPTION_DA, "Sæt et tracepoint. Hvis der angives en enkelt adresse sættes et\n"
                                         "tracepoint for adressen. Hvis der angives to adresser sættes et\n"
                                         "tracepoint for hukommelsesområdet mellem adresserne."},
/* de */ {IDGS_MON_TRACE_DESCRIPTION_DE, "Setze einen Tracepoint. Wenn eine einzelne Adresse angegeben wurde, setze eine\n"
                                         "Tracepoint für diese Adresse. Wenn zwei Adressen angegeben wurden, \n"
                                         "setze Tracepoints für die Speicheradressen zwischen diesen beiden Adressen."},
/* fr */ {IDGS_MON_TRACE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_TRACE_DESCRIPTION_HU, "Követési pont beállítása.  Ha egyetlen cím van megadva, a követési pontot\n"
                                         "ahhoz állítja be.  Ha két cím van megadva, a követési pontot\n"
                                         "az adott tartományra állítja be."},
/* it */ {IDGS_MON_TRACE_DESCRIPTION_IT, "Imposta un tracepoint. Se viene specificato solo un indirizzo, imposta\n"
                                         "un tracepoint per tale indirizzo. Se vengono specificati due indirizzi,\n"
                                         "imposta un tracepoint per le locazioni di memoria comprese tra i\n"
                                         "due indirizzi."},
/* nl */ {IDGS_MON_TRACE_DESCRIPTION_NL, "Zet een traceerpunt. Als er een enkel adres is opgegeven dan zal er een\n"
                                         "traceerpunt worden gezet voor het adres. Als er twee adressen zijn\n"
                                         "opgegeven dan zal er een traceerpunt worden gezet voor de adressen tussen\n"
                                         "de twee adressen."},
/* pl */ {IDGS_MON_TRACE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_TRACE_DESCRIPTION_SV, "Sätt en spårningspunkt. Om du anger en ensam adress sätts en\n"
                                         "spårningspunkt för adressen. Om två adresser anges sätts en\n"
                                         "spårningspunkt för minnesområdet mellan adresserna."},
/* tr */ {IDGS_MON_TRACE_DESCRIPTION_TR, "Takip noktasý koy.  Eðer tek bir adres belirtilmiþse, takip nokasýný\n"
                                         "o adrese koy.  Eðer iki adres belirtilmiþse, izleme noktasýný\n"
                                         "o iki adres arasýnda kalan adreslere koy."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MONITOR_STATE_COMMANDS,    N_("Monitor state commands:")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MONITOR_STATE_COMMANDS_DA, "Monitorstatuskommandoer:"},
/* de */ {IDGS_MONITOR_STATE_COMMANDS_DE, "Monitor Zustand Kommandos:"},
/* fr */ {IDGS_MONITOR_STATE_COMMANDS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MONITOR_STATE_COMMANDS_HU, "Monitor állapot parancsok:"},
/* it */ {IDGS_MONITOR_STATE_COMMANDS_IT, "Comandi stato monitor:"},
/* nl */ {IDGS_MONITOR_STATE_COMMANDS_NL, "Monitor status commandos:"},
/* pl */ {IDGS_MONITOR_STATE_COMMANDS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MONITOR_STATE_COMMANDS_SV, "Kommandon för monitorstatus:"},
/* tr */ {IDGS_MONITOR_STATE_COMMANDS_TR, "Monitör durum komutlarý:"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_DEVICE_DESCRIPTION,    N_("Set the default memory device to either the computer `c:' or the\n"
                                             "specified disk drive (`8:', `9:').")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_DEVICE_DESCRIPTION_DA, "Sæt det valgte hukommelses område til enten computeren \"c:\" eller\n"
                                          "det angivne diskettedrev (\"8:\", \"9:\")."},
/* de */ {IDGS_MON_DEVICE_DESCRIPTION_DE, "Setze das aktuelle Speichergerät entweder auf `c:' \n"
                                          "oder das angegebene Laufwerk (`8:', `9:')."},
/* fr */ {IDGS_MON_DEVICE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_DEVICE_DESCRIPTION_HU, "Az alapértelmezett memória eszköz beállítása a számítógépre `c:' vagy\n"
                                          "a megadott lemezmeghajtóra (`8:', `9:')."},
/* it */ {IDGS_MON_DEVICE_DESCRIPTION_IT, "Imposta la periferica di memoria predefinita o sul computer `c:'\n"
                                          "oppure sul disk drive specificato (`8:', `9:')."},
/* nl */ {IDGS_MON_DEVICE_DESCRIPTION_NL, "Zet de standaard geheugen apparaat naar of de computer `c:' of de\n"
                                          "opgegeven disk drive (`8:', `9:')."},
/* pl */ {IDGS_MON_DEVICE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_DEVICE_DESCRIPTION_SV, "Sätt förvald minnesenhet antingen till datorn \"c:\" eller\n"
                                          "den angivna diskettenheten (\"8:\", \"9:\")."},
/* tr */ {IDGS_MON_DEVICE_DESCRIPTION_TR, "Varsayýlan bellek aygýtýný bilgisayar `c:' ya da (`8:', `9:') ile\n"
                                          "belirtilmiþ disket dürücü olarak belirle."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_EXIT_DESCRIPTION,    N_("Leave the monitor and return to execution.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_EXIT_DESCRIPTION_DA, "Forlad monitoren og gå tilbage til eksekvering."},
/* de */ {IDGS_MON_EXIT_DESCRIPTION_DE, "Verlasse Monitor und setze Ausführung fort."},
/* fr */ {IDGS_MON_EXIT_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_EXIT_DESCRIPTION_HU, "A monitor elhagyása és a végrehajtás folytatása."},
/* it */ {IDGS_MON_EXIT_DESCRIPTION_IT, "Esce dal monitor e ritorna all'esecuzione."},
/* nl */ {IDGS_MON_EXIT_DESCRIPTION_NL, "Verlaat de monitor en ga terug naar de uitvoering."},
/* pl */ {IDGS_MON_EXIT_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_EXIT_DESCRIPTION_SV, "Lämna monitorn och återgå till programkörningen."},
/* tr */ {IDGS_MON_EXIT_DESCRIPTION_TR, "Monitörü terk et ve çalýþtýrmaya geri dön."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_QUIT_DESCRIPTION,    N_("Exit the emulator immediately.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_QUIT_DESCRIPTION_DA, "Afslut emulatoren øjblikkeligt."},
/* de */ {IDGS_MON_QUIT_DESCRIPTION_DE, "Emulator sofort beenden."},
/* fr */ {IDGS_MON_QUIT_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_QUIT_DESCRIPTION_HU, "Azonnali kilépés az emulátorból."},
/* it */ {IDGS_MON_QUIT_DESCRIPTION_IT, "Esci immediatamente dall'emulatore."},
/* nl */ {IDGS_MON_QUIT_DESCRIPTION_NL, "Onmiddelijk de emulator sluiten."},
/* pl */ {IDGS_MON_QUIT_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_QUIT_DESCRIPTION_SV, "Avsluta emulatorn omedelbart."},
/* tr */ {IDGS_MON_QUIT_DESCRIPTION_TR, "Emülasyondan derhal çýk."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_RADIX_DESCRIPTION,    N_("Set the default radix to hex, decimal, octal, or binary.  With no\n"
                                            "argument, the current radix is printed.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_RADIX_DESCRIPTION_DA, "Sæt standardradix til hex, decimal, oktal eller binær. Hvis der\n"
                                         "ikke angives et argument vises det aktuelle radix."},
/* de */ {IDGS_MON_RADIX_DESCRIPTION_DE, "Setze die Standardzahlenbasis auf Hexadezimal, Dezimal, Oktal oder Binär.\n"
                                         "Bei keinem Argument wird die akteulle Zahlenbasis ausgegeben."},
/* fr */ {IDGS_MON_RADIX_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_RADIX_DESCRIPTION_HU, "Az alap számrendszer hexadecimális, decimális, oktális vagy bináris.\n"
                                         "Paraméter nélkül az aktuálisat mutatja."},
/* it */ {IDGS_MON_RADIX_DESCRIPTION_IT, "Imposta la radice predefinita a esadecimale, decimale, ottale o\n"
                                         "binaria. Senza parametro viene stampata la radice attuale."},
/* nl */ {IDGS_MON_RADIX_DESCRIPTION_NL, "Zet de standaard radix naar hex, decimaal, octaal of binair. Als er\n"
                                         "geen parameter is opgegeven dan zal de huidige radix worden getoond."},
/* pl */ {IDGS_MON_RADIX_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_RADIX_DESCRIPTION_SV, "Sätt standardradix till hex, decimal, oktal eller binär. Om du\n"
                                         "inte ger något argument visas nuvarande radix."},
/* tr */ {IDGS_MON_RADIX_DESCRIPTION_TR, "Varsayýlan sayý tabanýný 16'lý, 10'lu, 8'li ya da 2'li yap.\n"
                                         "Argümansýz olarak geçerli sayý tabaný yazdýrýlýr."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_R_ON_R_OFF_R_TOGGLE,    N_("[on|off|toggle]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_R_ON_R_OFF_R_TOGGLE_DA, "[on|off|toggle]"},
/* de */ {IDGS_R_ON_R_OFF_R_TOGGLE_DE, "[on|off|toggle]"},
/* fr */ {IDGS_R_ON_R_OFF_R_TOGGLE_FR, ""},  /* fuzzy */
/* hu */ {IDGS_R_ON_R_OFF_R_TOGGLE_HU, ""},  /* fuzzy */
/* it */ {IDGS_R_ON_R_OFF_R_TOGGLE_IT, "[on|off|toggle]"},
/* nl */ {IDGS_R_ON_R_OFF_R_TOGGLE_NL, "[on|off|toggle]"},
/* pl */ {IDGS_R_ON_R_OFF_R_TOGGLE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_R_ON_R_OFF_R_TOGGLE_SV, "[on|off|toggle]"},
/* tr */ {IDGS_R_ON_R_OFF_R_TOGGLE_TR, "[on|off|toggle]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_SIDEFX_DESCRIPTION,    N_("Control how monitor generated reads affect memory locations that have\n"
                                             "read side-effects.  If the argument is 'on' then reads may cause\n"
                                             "side-effects.  If the argument is 'off' then reads don't cause\n"
                                             "side-effects.  If the argument is 'toggle' then the current mode is\n"
                                             "switched.  No argument displays the current state.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_SIDEFX_DESCRIPTION_DA, "Bestemmer hvordan læsninger i monitoren påvirker hukommelsesområder,\n"
                                          "der har sedeeffekter ved læsning. Hvis argumentet er \"on\" kan læsninger\n"
                                          "foresage sideeffekter. Hvis argumentet er \"off\" vil læsninger ikke\n"
                                          "foresage sideeffekter. Hvis argumentet er \"toggle\" skiftes\n"
                                          "den nuværende tilstand. Hvis der ikke angives noget argument vises\n"
                                          "den nuværende tilstand."},
/* de */ {IDGS_MON_SIDEFX_DESCRIPTION_DE, "Steuerung, wie die vom Monitor erzeugten Lesezugriffe die Speicherzellen\n"
                                          "mit Lese-Seiteneffekten betreffen.\n"
                                          "Bei Argument \"on\" werden Lese-Seiteneffekte erzeugt.\n"
                                          "Bei Argument \"off\" werden keine Lese-Seiteneffekte erzeugt.\n"
                                          "Bei Argumenrt \"toggle\" wird das Verhalten vertauscht.\n"
                                          "Bei keinem Argument wird das aktuelle Verhalten ausgegebene."},
/* fr */ {IDGS_MON_SIDEFX_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_SIDEFX_DESCRIPTION_HU, "Beállítja, hogyan hatnak a monitorprogram memória olvasásai azokra a\n"
                                          "memóriahelyekre, amelyeknek olvasás-mellékhatásai vannak. Ha a paraméter\n"
                                          "'on', az olvasások okozhatnak mellékhatásokat. Ha a paraméter 'off', akkor pedig\n"
                                          "nem okoznak. Ha a paraméter 'toggle', az aktuális módot átkapcsolja.\n"
                                          "Paraméter nélküli hívás kiírja az aktuális módot."},
/* it */ {IDGS_MON_SIDEFX_DESCRIPTION_IT, "Controlla come le letture generate dal monitor affliggono le locazioni\n"
                                          "di memoria che hanno degli effetti collaterali sulla lettura.\n"
                                          "Se il paremetro è 'on', le letture possono causare effetti collaterali.\n"
                                          "Se il parametro è 'off', le letture non causano effetti collaterali.\n"
                                          "Se il parametro è 'scambia', la modalità attuale viene invertita.\n"
                                          "Nessun parametro mostra lo stato attuale."},
/* nl */ {IDGS_MON_SIDEFX_DESCRIPTION_NL, "Geef aan hoe de lees acties van de monitor de geheugen locaties met lees\n"
                                          "bij-effecten beinvloed. Als de parameter 'off' is dan zullen lees acties\n"
                                          "geen bij-effecten vertonen. Als de parameter 'on' is dan zullen lees\n"
                                          "acties bij-effecten vertonen. Als de parameter 'toggle' is dan zal de\n"
                                          "huidige modus worden geschakeld. Als er geen parameter is dan zal de\n"
                                          "huidige modus worden getoond."},
/* pl */ {IDGS_MON_SIDEFX_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_SIDEFX_DESCRIPTION_SV, "Styr hur läsningar i monitorn påverkar minnesområden som har\n"
                                          "sidoeffekter vid läsning. Om argumentet är \"on\" tillåts läsningar\n"
                                          "att orsaka sidoeffekter. Om argumentet är \"off\" tillåts inte\n"
                                          "läsningar att orsaka sidoeffekter. Om argumentet är \"toggle\" byts\n"
                                          "det nuvarande läget. Om du inte anger några argument visas\n"
                                          "nuvarande läge."},
/* tr */ {IDGS_MON_SIDEFX_DESCRIPTION_TR, "Okuma yan etkileri bulunan bellek konumlarýný monitör tarafýndan oluþturulan\n"
                                          "okumalarýn nasýl etkilediðini kontrol et.  Argüman 'on' ise, okumalar yan\n"
                                          "etkiye neden olabilir.  Argüman 'off' ise okumalarýn bir yan etkisi\n"
                                          "olmayacaktýr.  Eðer argüman 'toggle' ise geçerli mod deðiþtirilecektir.\n"
                                          "Argüman verilmemesi o anki durumu görüntüler."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_DISK_COMMANDS,    N_("Disk commands:")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_DISK_COMMANDS_DA, "Diskettekommandoer:"},
/* de */ {IDGS_DISK_COMMANDS_DE, "Disk Kommandos:"},
/* fr */ {IDGS_DISK_COMMANDS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_DISK_COMMANDS_HU, "Lemez parancsok:"},
/* it */ {IDGS_DISK_COMMANDS_IT, "Comandi del disco:"},
/* nl */ {IDGS_DISK_COMMANDS_NL, "Disk commandos:"},
/* pl */ {IDGS_DISK_COMMANDS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_DISK_COMMANDS_SV, "Diskettkommandon:"},
/* tr */ {IDGS_DISK_COMMANDS_TR, "Disk komutlarý:"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_DISK_COMMAND,    N_("<disk command>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_DISK_COMMAND_DA, "<diskkommando>"},
/* de */ {IDGS_P_DISK_COMMAND_DE, "<Disk Kommando>"},
/* fr */ {IDGS_P_DISK_COMMAND_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_DISK_COMMAND_HU, "<lemez parancs>"},
/* it */ {IDGS_P_DISK_COMMAND_IT, "<comando disco>"},
/* nl */ {IDGS_P_DISK_COMMAND_NL, "<disk commando>"},
/* pl */ {IDGS_P_DISK_COMMAND_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_DISK_COMMAND_SV, "<diskkommando>"},
/* tr */ {IDGS_P_DISK_COMMAND_TR, "<disk komutu>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_AT_DESCRIPTION,    N_("Perform a disk command on the currently attached disk image on drive 8.\n"
                                         "The specified disk command is sent to the drive's channel #15.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_AT_DESCRIPTION_DA, "Udfører en diskkommando på det diskette-image, der er tilsluttet\n"
                                      "drev 8. Diskkommandoen sendes til drevets kanal #15."},
/* de */ {IDGS_MON_AT_DESCRIPTION_DE, "Führe ein Diskkommando auf dem aktuell eingelegtem Diskimage auf Laufwerk 8\n"
                                      "aus.\n"
                                      "Das angegebene Kommando wird auf dem Laufwerks Kommandokanal #15 geschickt."},
/* fr */ {IDGS_MON_AT_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_AT_DESCRIPTION_HU, "Lemez parancs végrehajtása az aktuálisan csatolt lemezen, a 8-as egységen.\n"
                                      "A megadott lemez parancsot a meghajtó #15-ös csatornájára küldi."},
/* it */ {IDGS_MON_AT_DESCRIPTION_IT, "Esegue un comando disco sull'immagine disco attualmente presente nel\n"
                                      "drive 8. Il comando specificato è inviato al canale #15 del drive."},
/* nl */ {IDGS_MON_AT_DESCRIPTION_NL, "Doe een disk commando operatie op de huidig gekoppelde disk bestand in drive\n"
                                      "8, Het opgegeven disk commando zal worden gestuurt naar kanaal #15 van de\n"
                                      "drive."},
/* pl */ {IDGS_MON_AT_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_AT_DESCRIPTION_SV, "Utför ett diskkommando på den diskettavbildning som är ansluten till\n"
                                      "enhet 8. Diskkommandot sänds på diskettenhetens kanal nummer 15."},
/* tr */ {IDGS_MON_AT_DESCRIPTION_TR, "8 nolu sürücüde takýlý disk imajý üzerinde bir disk komutu çalýþtýr.\n"
                                      "Belirtilen disk komutu sürücünün 15 numaralý kanalýna gönderilir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_FILENAME_P_DEVICE,    N_("<filename> <device>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_FILENAME_P_DEVICE_DA, "<filnavn> <enhed>"},
/* de */ {IDGS_P_FILENAME_P_DEVICE_DE, "<Dateiname> <Gerät>"},
/* fr */ {IDGS_P_FILENAME_P_DEVICE_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_FILENAME_P_DEVICE_HU, "<fájlnév> <eszköz>"},
/* it */ {IDGS_P_FILENAME_P_DEVICE_IT, "<nome del file> <perifericd>"},
/* nl */ {IDGS_P_FILENAME_P_DEVICE_NL, "<bestandsnaam> <apparaat>"},
/* pl */ {IDGS_P_FILENAME_P_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_FILENAME_P_DEVICE_SV, "<filnamn> <enhet>"},
/* tr */ {IDGS_P_FILENAME_P_DEVICE_TR, "<dosyaismi> <aygýt>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_ATTACH_DESCRIPTION,    N_("Attach file to device. (device 32 = cart)")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_ATTACH_DESCRIPTION_DA, "Tilslut en fil til enhed. (enhed 32 = cartridge)"},
/* de */ {IDGS_MON_ATTACH_DESCRIPTION_DE, "Benutze Datei für das Gerät. (Gerät 32 = Modul)"},
/* fr */ {IDGS_MON_ATTACH_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_ATTACH_DESCRIPTION_HU, "Fájl csatolása az eszközhöz. (32-es eszköz = cart)"},
/* it */ {IDGS_MON_ATTACH_DESCRIPTION_IT, "Seleziona il file per la perifica. (periferica 32 = cartuccia)"},
/* nl */ {IDGS_MON_ATTACH_DESCRIPTION_NL, "Koppel bestand aan apparaat. (apparaat 32 = cart"},
/* pl */ {IDGS_MON_ATTACH_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_ATTACH_DESCRIPTION_SV, "Anslut en fil till en enhet. (enhet 32 = insticksmodul)"},
/* tr */ {IDGS_MON_ATTACH_DESCRIPTION_TR, "Aygýta dosya yerleþtir. (aygýt 32 = kartuþ)"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS,    N_("\"<filename>\" <device> <address>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS_DA, "\"<filnamn>\" <enhed> <adresse>"},
/* de */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS_DE, "\"<Dateiname>\" <Gerät> <Adresse>"},
/* fr */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS_HU, "\"<fájlnév>\" <eszköz> <cím>"},
/* it */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS_IT, "\"<nome del file>\" <periferica> <indirizzo>"},
/* nl */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS_NL, "\"<bestandsnaam>\" <aparraat> <adres>"},
/* pl */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS_SV, "\"<filnamn>\" <enhet> <adress>"},
/* tr */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS_TR, "\"<dosyaismi>\" <aygýt> <adres>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_BLOAD_DESCRIPTION,    N_("Load the specified file into memory at the specified address.\n"
                                            "If device is 0, the file is read from the file system.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_BLOAD_DESCRIPTION_DA, "Indlæs den angivne fil til hukommelsen på den angivne adresse.\n"
                                         "Hvis enhed er 0 læses fra filsystemet."},
/* de */ {IDGS_MON_BLOAD_DESCRIPTION_DE, "Lade angegebene Datei in den Speicher an die angegebene Adresse.\n"
                                         "Bei Gerät 0, wird die Datei vom Dateisystem gelesen."},
/* fr */ {IDGS_MON_BLOAD_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_BLOAD_DESCRIPTION_HU, "A megadott fájl betöltése a memóriába a megadott címre.\n"
                                         "Ha az eszköz 0, a fájlt a fájlrendszerbõl olvassa."},
/* it */ {IDGS_MON_BLOAD_DESCRIPTION_IT, "Carica il file specificato in memoria all'indirizzo specificato.\n"
                                         "Se la periferica è 0, il file è letto dal file system."},
/* nl */ {IDGS_MON_BLOAD_DESCRIPTION_NL, "Laad het opgegeven bestand naar het geheugen met het opgegeven adres.\n"
                                         "Als het apparaat 0 is dan zal het bestand van het bestandssysteem\n"
                                         "worden geladen."},
/* pl */ {IDGS_MON_BLOAD_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_BLOAD_DESCRIPTION_SV, "Läs den angivna filen till minnet på den angivna adressen.\n"
                                         "Om enheten är 0 läses filen från filsystemet."},
/* tr */ {IDGS_MON_BLOAD_DESCRIPTION_TR, "Belirtilmiþ dosyayý belirtilen adresten itibaren belleðe yükle.\n"
                                         "Aygýt 0 ise, dosya dosya sistemi üzerinden okunur."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_TRACK_P_SECTOR_RP_ADDRESS,    N_("<track> <sector> [<address>]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_TRACK_P_SECTOR_RP_ADDRESS_DA, "<spor> <sektor> [<adresse>]"},
/* de */ {IDGS_P_TRACK_P_SECTOR_RP_ADDRESS_DE, "<Spur> <Sektor> [<Adresse>]"},
/* fr */ {IDGS_P_TRACK_P_SECTOR_RP_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_TRACK_P_SECTOR_RP_ADDRESS_HU, "<sáv> <szektor> [<cím>]"},
/* it */ {IDGS_P_TRACK_P_SECTOR_RP_ADDRESS_IT, "<traccia> <settore> [<indirizzo>]"},
/* nl */ {IDGS_P_TRACK_P_SECTOR_RP_ADDRESS_NL, "<spoor> <sector> [<adres>]"},
/* pl */ {IDGS_P_TRACK_P_SECTOR_RP_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_TRACK_P_SECTOR_RP_ADDRESS_SV, "<spår> <sektor> [<adress>]"},
/* tr */ {IDGS_P_TRACK_P_SECTOR_RP_ADDRESS_TR, "<track> <sektör> [<adres>]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_BLOCK_READ_DESCRIPTION,    N_("Read the block at the specified track and sector.  If an address is\n"
                                                 "specified, the data is loaded into memory.  If no address is given, the\n"
                                                 "data is displayed using the default datatype.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_BLOCK_READ_DESCRIPTION_DA, "Læser en blok fra det angivne spor og sektor. Hvis en adresse angives\n"
                                              "indlæses data til hukommelsen. Hvis der ikke angives nogen adresse vises\n"
                                              "indholdet med standarddatatypen."},
/* de */ {IDGS_MON_BLOCK_READ_DESCRIPTION_DE, "Lese den Block von angegebener Spur und Sektor. Wenn eine Adresse\n"
                                              "angegeben wurde, lade in auf die entsprechende Adresse. Wenn keine Adresse\n"
                                              "angegeben ist, zeige die Daten im aktuellen Anzeigemodus für Daten."},
/* fr */ {IDGS_MON_BLOCK_READ_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_BLOCK_READ_DESCRIPTION_HU, "Blokk beolvasása a megadott sávból és szektorból.,  Ha egy cím is meg\n"
                                              "van adva, az adatot a memóriába tölti.  Ha az nincs, az\n"
                                              "adatot az alapbeállítás szerinti formában mutatja."},
/* it */ {IDGS_MON_BLOCK_READ_DESCRIPTION_IT, "Legge il blocco dalla traccia e settore specificati. Se è specificato\n"
                                              "un indirizzo, il dato è caricato in memoria. Se non è specificato alcun\n"
                                              "indirizzo, il dato è mostrato usando il tipo di dato predefinito."},
/* nl */ {IDGS_MON_BLOCK_READ_DESCRIPTION_NL, "Lees het blok van de opgegeven spoor en sector. Als er een adres is\n"
                                              "opgegeven dan zal de data naar dat adres in geheugen worden geladen.\n"
                                              "Als er geen adres is opgegeven dan zal de data worder getoond met de\n"
                                              "huidige datasoort."},
/* pl */ {IDGS_MON_BLOCK_READ_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_BLOCK_READ_DESCRIPTION_SV, "Läser blocket på det angivna spåret och sektorn. Om du anger en\n"
                                              "adress läses data till minnet. Om du inte anger någon adress visas\n"
                                              "innehållet enligt standarddatatypen."},
/* tr */ {IDGS_MON_BLOCK_READ_DESCRIPTION_TR, "Belirtilmiþ track ve sektördeki bloðu oku.  Eðer bir adres belirtilmiþse,\n"
                                              "veri belleðe yüklenir.  Eðer adres belirtilmemiþse, veri varsayýlan veri\n"
                                              "tipi kullanýlarak görüntülenir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS1_P_ADDRESS2,    N_("\"<filename>\" <device> <address1> <address2>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS1_P_ADDRESS2_DA, "\"<filnavn>\" <enhed> <adresse1> <adresse2>"},
/* de */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS1_P_ADDRESS2_DE, "\"<Dateiname>\" <Gerät> <Adresse1> <Adresse2>"},
/* fr */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS1_P_ADDRESS2_FR, ""},  /* fuzzy */
/* hu */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS1_P_ADDRESS2_HU, "\"<fájlnév>\" <eszköz> <cím1> <cím2>"},
/* it */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS1_P_ADDRESS2_IT, "\"<nome del file>\" <periferica> <indirizzo1> <indirizzo2>"},
/* nl */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS1_P_ADDRESS2_NL, "\"<bestandsnaam>\" <aparraat> <adres1> <adres2>"},
/* pl */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS1_P_ADDRESS2_PL, ""},  /* fuzzy */
/* sv */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS1_P_ADDRESS2_SV, "\"<filnamn>\" <enhet> <adress1> <adress2>"},
/* tr */ {IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS1_P_ADDRESS2_TR, "\"<dosyaismi>\" <aygýt> <adres1> <adres2>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_BSAVE_DESCRIPTION,    N_("Save the memory from address1 to address2 to the specified file.\n"
                                            "If device is 0, the file is written to the file system.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_BSAVE_DESCRIPTION_DA, "Gem hukommelsen fra adresse1 til adresse2 til den angivne fil.\n"
                                         "Hvis enhed er 0 skrives til filsystemet."},
/* de */ {IDGS_MON_BSAVE_DESCRIPTION_DE, "Speichere den Speicherinhalt von Adresse1 zu Adresse2 in die angegebene Datei.\n"
                                         "Wenn das Gerät 0 angegeben wurde, speichere im Dateisystem."},
/* fr */ {IDGS_MON_BSAVE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_BSAVE_DESCRIPTION_HU, "A memória cím1 és cím2 közötti részének fájlba mentése.\n"
                                         "Ha az eszköz 0, a fájl a fájlrendszerbe kerül."},
/* it */ {IDGS_MON_BSAVE_DESCRIPTION_IT, "Salva la memoria compresa tra indirizzo1 e indirizzo2 sul file specificato.\n"
                                         "Se la periferica è 0, il file è scritto sul file system."},
/* nl */ {IDGS_MON_BSAVE_DESCRIPTION_NL, "Sla het geheugen tussen adres1 en adres2 op naar het opgegeven bestand.\n"
                                         "Als het apparaat 0 is dan zal het bestand naar het bestandssysteem worden\n"
                                         "geschreven."},
/* pl */ {IDGS_MON_BSAVE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_BSAVE_DESCRIPTION_SV, "Spara minnet från adress1 till adress2 till den angivna filen.\n"
                                         "Om enheten är 0 skrivs filen till filsystemet."},
/* tr */ {IDGS_MON_BSAVE_DESCRIPTION_TR, "Adres1'den adres2'ye kadar olan belleði belirtilen dosyaya kaydet.\n"
                                         "Eðer aygýt 0 ise, dosya dosya sistemine yazýlýr."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_TRACK_P_SECTOR_P_ADDRESS,    N_("<track> <sector> <address>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_TRACK_P_SECTOR_P_ADDRESS_DA, "<spor> <sektor> <adresse>"},
/* de */ {IDGS_P_TRACK_P_SECTOR_P_ADDRESS_DE, "<Spur> <Sektor> <Adress>"},
/* fr */ {IDGS_P_TRACK_P_SECTOR_P_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_TRACK_P_SECTOR_P_ADDRESS_HU, "<sáv> <szektor> <cím>"},
/* it */ {IDGS_P_TRACK_P_SECTOR_P_ADDRESS_IT, "<traccia> <settore> <indirizzo>"},
/* nl */ {IDGS_P_TRACK_P_SECTOR_P_ADDRESS_NL, "<spoor> <sector> <adres>"},
/* pl */ {IDGS_P_TRACK_P_SECTOR_P_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_TRACK_P_SECTOR_P_ADDRESS_SV, "<spår> <sektor> <adress>"},
/* tr */ {IDGS_P_TRACK_P_SECTOR_P_ADDRESS_TR, "<track> <sektör> <adres>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_BLOCK_WRITE_DESCRIPTION,    N_("Write a block of data at `address' on the specified track and sector\n"
                                                  "of disk in drive 8.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_BLOCK_WRITE_DESCRIPTION_DA, "Skriv datablok på \"adresse\" til det angivne spor og sektor\n"
                                               "på disken i drev 8."},
/* de */ {IDGS_MON_BLOCK_WRITE_DESCRIPTION_DE, "Schreibe einen Datenblock von `Adresse' auf die angegebene Spure und Sektor\n"
                                               "in die Imagedatei von Laufwerk 8."},
/* fr */ {IDGS_MON_BLOCK_WRITE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_BLOCK_WRITE_DESCRIPTION_HU, "Egy adat blokk írása `cím' helyrõl a megadott sávba és szektorba\n"
                                               "a 8-as meghajtón."},
/* it */ {IDGS_MON_BLOCK_WRITE_DESCRIPTION_IT, "Scrive il blocco di dati all'indirizzo sulla traccia e settore specificati\n"
                                               "del disco nel drive 8."},
/* nl */ {IDGS_MON_BLOCK_WRITE_DESCRIPTION_NL, "Schrijf een data blok van `adres' naar de opgegeven spoor en sector van\n"
                                               "de disk in drive 8."},
/* pl */ {IDGS_MON_BLOCK_WRITE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_BLOCK_WRITE_DESCRIPTION_SV, "Skriv ett datablock på \"adress\" till det angivna spåret och sektorn\n"
                                               "på disken i enhet 8."},
/* tr */ {IDGS_MON_BLOCK_WRITE_DESCRIPTION_TR, "Belirtilmiþ track ve sektörlerde `adres'te yer alan veri bloðunu 8\n"
                                               "numaralý sürücüdeki diske yaz."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_DIRECTORY,    N_("<directory>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_DIRECTORY_DA, "<katalog>"},
/* de */ {IDGS_P_DIRECTORY_DE, "<Verzeichnis>"},
/* fr */ {IDGS_P_DIRECTORY_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_DIRECTORY_HU, "<könyvtár>"},
/* it */ {IDGS_P_DIRECTORY_IT, "<directory>"},
/* nl */ {IDGS_P_DIRECTORY_NL, "<directory>"},
/* pl */ {IDGS_P_DIRECTORY_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_DIRECTORY_SV, "<katalog>"},
/* tr */ {IDGS_P_DIRECTORY_TR, "<dizin>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_CD_DESCRIPTION,    N_("Change the working directory.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_CD_DESCRIPTION_DA, "Skift arbejdskatalog."},
/* de */ {IDGS_MON_CD_DESCRIPTION_DE, "Arbeitsverzeichnis ändern."},
/* fr */ {IDGS_MON_CD_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_CD_DESCRIPTION_HU, "Munkakönyvtár megváltoztatása."},
/* it */ {IDGS_MON_CD_DESCRIPTION_IT, "Cambia la directory di lavoro."},
/* nl */ {IDGS_MON_CD_DESCRIPTION_NL, "Verander de werkdirectory."},
/* pl */ {IDGS_MON_CD_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_CD_DESCRIPTION_SV, "Byt arbetskatalog."},
/* tr */ {IDGS_MON_CD_DESCRIPTION_TR, "Çalýþma dizinini deðiþtir."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_DETACH_DESCRIPTION,    N_("Detach file from device. (device 32 = cart)")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_DETACH_DESCRIPTION_DA, "Frakobl filen fra enheden. (enhed 32 = cartridge)"},
/* de */ {IDGS_MON_DETACH_DESCRIPTION_DE, "Entferne Datei von Gerät. (Gerät 32 = Modul)"},
/* fr */ {IDGS_MON_DETACH_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_DETACH_DESCRIPTION_HU, "Fájl leválasztása az eszközrõl. (32-es eszköz = cart)"},
/* it */ {IDGS_MON_DETACH_DESCRIPTION_IT, "Rimuovi il file dalla periferica. (periferica 32 = cart)"},
/* nl */ {IDGS_MON_DETACH_DESCRIPTION_NL, "Ontkoppel bestand van apparaat. (apparaat 32 = cart)"},
/* pl */ {IDGS_MON_DETACH_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_DETACH_DESCRIPTION_SV, "Koppla från filen från enheten. (enhet 32 = insticksmodul)"},
/* tr */ {IDGS_MON_DETACH_DESCRIPTION_TR, "Aygýttan dosya çýkart. (aygýt 32 = kartuþ)"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_DIRECTORY,    N_("[<directory>]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_DIRECTORY_DA, "[<katalog>]"},
/* de */ {IDGS_RP_DIRECTORY_DE, "[<Verzeichnis>]"},
/* fr */ {IDGS_RP_DIRECTORY_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_DIRECTORY_HU, "[<könyvtár>]"},
/* it */ {IDGS_RP_DIRECTORY_IT, "[<directory>]"},
/* nl */ {IDGS_RP_DIRECTORY_NL, "[<directory>]"},
/* pl */ {IDGS_RP_DIRECTORY_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_DIRECTORY_SV, "[<katalog>]"},
/* tr */ {IDGS_RP_DIRECTORY_TR, "[<dizin>]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_DIR_DESCRIPTION,    N_("Display the directory contents.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_DIR_DESCRIPTION_DA, "Vis katalogets indhold."},
/* de */ {IDGS_MON_DIR_DESCRIPTION_DE, "Zeige den Verzeichnisinhalt."},
/* fr */ {IDGS_MON_DIR_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_DIR_DESCRIPTION_HU, "A könyvtár tartalmának mutatása."},
/* it */ {IDGS_MON_DIR_DESCRIPTION_IT, "Mostra il contenuto della directory."},
/* nl */ {IDGS_MON_DIR_DESCRIPTION_NL, "Toon de directory inhoud."},
/* pl */ {IDGS_MON_DIR_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_DIR_DESCRIPTION_SV, "Visa katalogens innehåll."},
/* tr */ {IDGS_MON_DIR_DESCRIPTION_TR, "Dizin içeriðini görüntüle."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_QP_FILENAME_P_DEVICE_RP_ADDRESS,    N_("\"<filename>\" <device> [<address>]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_QP_FILENAME_P_DEVICE_RP_ADDRESS_DA, "\"<filnavn>\" <enhed> [<adresse>]"},
/* de */ {IDGS_QP_FILENAME_P_DEVICE_RP_ADDRESS_DE, "\"<Dateiname>\" <Gerät> [<Adresse>]"},
/* fr */ {IDGS_QP_FILENAME_P_DEVICE_RP_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_QP_FILENAME_P_DEVICE_RP_ADDRESS_HU, "\"<fájlnév>\" <eszköz> [<cím>]"},
/* it */ {IDGS_QP_FILENAME_P_DEVICE_RP_ADDRESS_IT, "\"<nome del file>\" <periferica> [<indirizzo>]"},
/* nl */ {IDGS_QP_FILENAME_P_DEVICE_RP_ADDRESS_NL, "\"<bestandsnaam>\" <apparaat> [<adres>]"},
/* pl */ {IDGS_QP_FILENAME_P_DEVICE_RP_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_QP_FILENAME_P_DEVICE_RP_ADDRESS_SV, "\"<filnamn>\" <enhet> [<adress>]"},
/* tr */ {IDGS_QP_FILENAME_P_DEVICE_RP_ADDRESS_TR, "\"<dosyaismi>\" <aygýt> [<adres>]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_LOAD_DESCRIPTION,    N_("Load the specified file into memory at the specified address. Set BASIC\n"
                                           "pointers appropriately if loaded into computer memory (not all emulators).\n"
                                           "Use (otherwise ignored) two-byte load address from file if no address\n"
                                           "specified.\n"
                                           "If device is 0, the file is read from the file system.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_LOAD_DESCRIPTION_DA, "Indlæs filen i hukommelsen på den angivne adresse. Opdaterer\n"
                                        "BASIC-pointers ved læsning til computerens hukommelse (ikke alle\n"
                                        "emulatorer). Hvis ingen adresse angives bruges load-adressen fra (som ellers ignoreres).\n"
                                        "Hvis enhed er 0 læses fra filsystemet."},
/* de */ {IDGS_MON_LOAD_DESCRIPTION_DE, "Lade die angegebene Datei in den Speicher auf die angegebene Adresse.\n"
                                        "Setze den BASIC Zeiger entsprechend, wenn in den Rechnerspeicher geladen \n"
                                        "wurde (nicht bei allen Emulatoren!).\n"
                                        "Verwende (sonst ignoriert) die zwei-Byte Lade-Adresse aus der Datei, wenn\n"
                                        "keine Adresse angegeben wurde.\n"
                                        "Bei Gerät 0, wird die Datei vom Dateisystem gelade."},
/* fr */ {IDGS_MON_LOAD_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_LOAD_DESCRIPTION_HU, "A megadott fájl betöltése a megadott címre. A BASIC mutatókat\n"
                                        "a megfelelõ módon beállítja, ha a számítógép memóriába tölti az adatot\n"
                                        "(nem minden emulátornál). Ha nincs cím megadva, az amúgy figyelmen hagyott\n"
                                        "két bájtos betöltési címet használja a fájlból.\n"
                                        "Ha az eszköz 0, a fájlt a fájlrendszerrõl olvassa."},
/* it */ {IDGS_MON_LOAD_DESCRIPTION_IT, "Carica il file specificato in memroia all'indirizzo specificato. Imposta\n"
                                        "i puntatori del BASIC in modo appropriato se caricato nella memoria\n"
                                        "del computer (non su tutti gli emulatori).\n"
                                        "Usa (altrimenti ignora) l'indirizzo di caricamento su due byte del file\n"
                                        "se non viene specificato alcun indirizzo.\n"
                                        "Se la periferica è 0, il file viene letto dal file system."},
/* nl */ {IDGS_MON_LOAD_DESCRIPTION_NL, "Laad het opgegeven bestand naar geheugen op het opgegeven adres. Zet de BASIC\n"
                                        "start en eind indicators correct als het bestand naar het computer geheugen\n"
                                        "geladen wordt (niet alle emulatoren). De eerste twee bytes van het bestand\n"
                                        "zullen als laad adres worden gebruikt als er geen adres is opgegeven. Als\n"
                                        "het apparaat 0 is dan zal het bestand van het bestandssysteem worden geladen."},
/* pl */ {IDGS_MON_LOAD_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_LOAD_DESCRIPTION_SV, "Läs in den angivna filen i minnet på den angivna adressen. Sätter\n"
                                        "motsvarande BASIC-pekare vid läsning till datorminnet (inte alla\n"
                                        "emulatorer). Om ingen adress anges används tvåbytesadressen i filen (ignoreras annars).\n"
                                        "Om enheten är 0 läses filen från filsystemet."},
/* tr */ {IDGS_MON_LOAD_DESCRIPTION_TR, "Belirtilmiþ dosyayý belirtilen adresten itibaren belleðe yükle. Eðer\n"
                                        "bilgisayar belleðine yüklenmiþse BASIC imleçlerini uygun bir biçimde\n"
                                        "ayarla (tüm emülatörlerde deðil). Eðer adres belirtilmemiþse iki bytelýk\n"
                                        "dosya yükleme adresini kullan (diðer ihtimalde yoksay).\n"
                                        "Eðer aygýt 0 ise, dosya dosya sisteminden okunur."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_PWD_DESCRIPTION,    N_("Show current working directory.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_PWD_DESCRIPTION_DA, "Vis aktuelt arbejdskatalog."},
/* de */ {IDGS_MON_PWD_DESCRIPTION_DE, "Zeige aktuelles Arbeitsverzeichnis."},
/* fr */ {IDGS_MON_PWD_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_PWD_DESCRIPTION_HU, "Munkakönyvtár mutatása."},
/* it */ {IDGS_MON_PWD_DESCRIPTION_IT, "Mostra la directory di lavoro attuale."},
/* nl */ {IDGS_MON_PWD_DESCRIPTION_NL, "Toon de huidige werk directory."},
/* pl */ {IDGS_MON_PWD_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_PWD_DESCRIPTION_SV, "Visa aktuell arbetskatalog."},
/* tr */ {IDGS_MON_PWD_DESCRIPTION_TR, "Geçerli çalýþma dizinini göster."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_SAVE_DESCRIPTION,    N_("Save the memory from address1 to address2 to the specified file.\n"
                                           "Write two-byte load address.\n"
                                           "If device is 0, the file is written to the file system.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_SAVE_DESCRIPTION_DA, "Gem hukommelsen fra  adresse1 til adresse2 til den angivne fil.\n"
                                        "Skriv 2-byte loadadresse i starten af filen.\n"
                                        "Hvis enhed er 0 skrives til filsystemet."},
/* de */ {IDGS_MON_SAVE_DESCRIPTION_DE, "Speichere den Speicher von Adresse1 bis Adresse2 in die angegebene Datei.\n"
                                        "Schreibe eine zwei-Byte Lade Adresse.\n"
                                        "Bei Gerät 0, wird dei Datei in das Dateisystem geschrieben."},
/* fr */ {IDGS_MON_SAVE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_SAVE_DESCRIPTION_HU, "A memória cím1 és cím2 közötti tartományának fájlba mentése.\n"
                                        "A kétbájtos betöltési címet is kiírja.\n"
                                        "Ha az eszköz 0, a fájlt a fájlrendszeren hozza létre."},
/* it */ {IDGS_MON_SAVE_DESCRIPTION_IT, "Salva la memoria compresa tra indirizzo1 e indirizzo2 sul file\n"
                                        "specificato.\n"
                                        "Scrive l'indirizzo di caricamento su due byte.\n"
                                        "Se la periferica è 0, il file è scritto sul file system."},
/* nl */ {IDGS_MON_SAVE_DESCRIPTION_NL, "Sla het geheugen tussen adres1 en adres2 op naar het opgegeven bestand.\n"
                                        "Schrijf een load adres van twee bytes.\n"
                                        "Als het apparaat 0 is dan zal het bestand geschreven worden naar het\n"
                                        "bestandssysteem."},
/* pl */ {IDGS_MON_SAVE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_SAVE_DESCRIPTION_SV, "Spara minnet från adress1 till adress2 till den angivna filen.\n"
                                        "Skriv tvåbytes inläsningsadress.\n"
                                        "Om enheten är 0 skrivs filen till filsystemet."},
/* tr */ {IDGS_MON_SAVE_DESCRIPTION_TR, "Adres1'den adres2'ye kadar olan belleði belirtilen dosyaya kaydet.\n"
                                        "Ýki bytelýk yükeme adresini yaz.\n"
                                        "Eðer aygýt 0 ise, dosya dosya sistemine yazýlýr."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_OTHER_COMMANDS,    N_("Other commands:")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_OTHER_COMMANDS_DA, "Andre kommandoer:"},
/* de */ {IDGS_OTHER_COMMANDS_DE, "Andere Kommandos:"},
/* fr */ {IDGS_OTHER_COMMANDS_FR, ""},  /* fuzzy */
/* hu */ {IDGS_OTHER_COMMANDS_HU, "Egyéb parancsok:"},
/* it */ {IDGS_OTHER_COMMANDS_IT, "Altri comandi:"},
/* nl */ {IDGS_OTHER_COMMANDS_NL, "Andere commandos:"},
/* pl */ {IDGS_OTHER_COMMANDS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_OTHER_COMMANDS_SV, "Andra kommandon:"},
/* tr */ {IDGS_OTHER_COMMANDS_TR, "Diðer komutlar:"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_DISPLAY_NUMBER_DESCRIPTION,    N_("Display the specified number in decimal, hex, octal and binary.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_DISPLAY_NUMBER_DESCRIPTION_DA, "Vis det angivne tal som decimal, hexadecimal, oktal og binært."},
/* de */ {IDGS_MON_DISPLAY_NUMBER_DESCRIPTION_DE, "Zeige die angegebene Zahl in dezimal, hexadezimal, oktal oder binär."},
/* fr */ {IDGS_MON_DISPLAY_NUMBER_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_DISPLAY_NUMBER_DESCRIPTION_HU, "A megadott szám kiírása decimálisan, hexadecimálisan, oktálisan és binárisan."},
/* it */ {IDGS_MON_DISPLAY_NUMBER_DESCRIPTION_IT, "Mostra il numero specificato in decimale, esadecimale, ottale e binario."},
/* nl */ {IDGS_MON_DISPLAY_NUMBER_DESCRIPTION_NL, "Toon het opgegeven nummer in decimaal, hex, octaal en binair."},
/* pl */ {IDGS_MON_DISPLAY_NUMBER_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_DISPLAY_NUMBER_DESCRIPTION_SV, "Visa det angivna talet decimalt, hexadecimalt, oktalt och binärt."},
/* tr */ {IDGS_MON_DISPLAY_NUMBER_DESCRIPTION_TR, "Belirtilen sayýyý 10'lu, 16'lý, 8'li ya da 2'li olarak görüntüle."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_CARTFREEZE_DESCRIPTION,    N_("Use cartridge freeze.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_CARTFREEZE_DESCRIPTION_DA, "Brug cartridge frys-funktion."},
/* de */ {IDGS_MON_CARTFREEZE_DESCRIPTION_DE, "Erweiterungsmodul Freeze benutzen."},
/* fr */ {IDGS_MON_CARTFREEZE_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_CARTFREEZE_DESCRIPTION_HU, "Cartridge fagyasztás használata."},
/* it */ {IDGS_MON_CARTFREEZE_DESCRIPTION_IT, "Usa freeze della cartuccia."},
/* nl */ {IDGS_MON_CARTFREEZE_DESCRIPTION_NL, "Gebruik cartridge freeze."},
/* pl */ {IDGS_MON_CARTFREEZE_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_CARTFREEZE_DESCRIPTION_SV, "Använd insticksmodulens frysfunktion."},
/* tr */ {IDGS_MON_CARTFREEZE_DESCRIPTION_TR, "Kartuþ freeze kullan"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_RP_COMMAND,    N_("[<command>]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_RP_COMMAND_DA, "[<kommando>]"},
/* de */ {IDGS_RP_COMMAND_DE, "[<Kommando>]"},
/* fr */ {IDGS_RP_COMMAND_FR, ""},  /* fuzzy */
/* hu */ {IDGS_RP_COMMAND_HU, "[<parancs>]"},
/* it */ {IDGS_RP_COMMAND_IT, "[<commando>]"},
/* nl */ {IDGS_RP_COMMAND_NL, "[<commando>]"},
/* pl */ {IDGS_RP_COMMAND_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RP_COMMAND_SV, "[<kommando>]"},
/* tr */ {IDGS_RP_COMMAND_TR, "[<komut>]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_HELP_DESCRIPTION,    N_("If no argument is given, prints out a list of all available commands\n" 
                                           "If an argument is given, prints out specific help for that command.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_HELP_DESCRIPTION_DA, "Hvis der ikke angives et argument vises en liste over mulige kommandoer.\n"
                                        "Angives et argument vises hjælp for denne kommando."},
/* de */ {IDGS_MON_HELP_DESCRIPTION_DE, "Wenn kein Argement angegeben wurde, werden alle Kommandos ausgegeben.\n"
                                        "Mit Argument, zeige die Hilfe für das angegebene Kommand."},
/* fr */ {IDGS_MON_HELP_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_HELP_DESCRIPTION_HU, "Ha nincs paraméter, az összes létezõ parancs listáját mutatja.\n"
                                        "Ha van, az adott parancshoz mutatja a súgót."},
/* it */ {IDGS_MON_HELP_DESCRIPTION_IT, "Se non viene specificato alcun argomeno, stamapa la lista di tutti i\n"
                                        "comandi disponibili.\n"
                                        "Se viene specificato un argomento, stampa l'aiuto relativo a quel\n"
                                        "comando."},
/* nl */ {IDGS_MON_HELP_DESCRIPTION_NL, "Als er geen parameter is opgegeven dan zal een lijst van alle commandos\n"
                                        "worden weergegeven. Als er een parameter is opgegeven dan zal de help voor\n"
                                        "het opgegeven command worden weergegeven."},
/* pl */ {IDGS_MON_HELP_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_HELP_DESCRIPTION_SV, "Om du inte anger några argument visas alla tillgängliga kommandon.\n"
                                        "Om du anger ett argument visas hjälp för det specifika kommandot."},
/* tr */ {IDGS_MON_HELP_DESCRIPTION_TR, "Eðer hiç argüman verilmemiþse, tüm mevcut komutlarýn listesi yazdýrýlýr\n"
                                        "Eðer bir argüman verilmiþse, o komuta özel yardým bilgisi yazdýrýlýr."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_QP_STRING,    N_("\"<string>\"")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_QP_STRING_DA, "\"<streng>\""},
/* de */ {IDGS_QP_STRING_DE, "\"<Zeichenkette>\""},
/* fr */ {IDGS_QP_STRING_FR, ""},  /* fuzzy */
/* hu */ {IDGS_QP_STRING_HU, "\"<sztring>\""},
/* it */ {IDGS_QP_STRING_IT, "\"<stringa>\""},
/* nl */ {IDGS_QP_STRING_NL, "\"<string>\""},
/* pl */ {IDGS_QP_STRING_PL, ""},  /* fuzzy */
/* sv */ {IDGS_QP_STRING_SV, "\"<sträng>\""},
/* tr */ {IDGS_QP_STRING_TR, "\"<yazý>\""},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_KEYBUF_DESCRIPTION,    N_("Put the specified string into the keyboard buffer.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_KEYBUF_DESCRIPTION_DA, "Indsæt den angivne streng i tastatur-bufferen."},
/* de */ {IDGS_MON_KEYBUF_DESCRIPTION_DE, "Definierte Eingabe in Tastaturpuffer bereitstellen."},
/* fr */ {IDGS_MON_KEYBUF_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_KEYBUF_DESCRIPTION_HU, "A megadott sztring elhelyezése a billentyûzet pufferben."},
/* it */ {IDGS_MON_KEYBUF_DESCRIPTION_IT, "Metti la stringa specificata nel buffer di tastiera."},
/* nl */ {IDGS_MON_KEYBUF_DESCRIPTION_NL, "Plaats de opgegeven string in de toetsenbordbuffer."},
/* pl */ {IDGS_MON_KEYBUF_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_KEYBUF_DESCRIPTION_SV, "Lägg den angivna strängen i tangentbordsbufferten."},
/* tr */ {IDGS_MON_KEYBUF_DESCRIPTION_TR, "Klavye arabelleðine belirtilen yazýyý koy."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_PLAYBACK_DESCRIPTION,    N_("Monitor commands from the specified file are read and executed.  This\n"
                                               "command stops at the end of file or when a STOP command is read.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_PLAYBACK_DESCRIPTION_DA, "Monitorkommandoer indlæses fra den angivne fil og udføres. Kommandoen\n"
                                            "stopper når slutningen af filen nås, eller når der opdages en STOP-kommando."},
/* de */ {IDGS_MON_PLAYBACK_DESCRIPTION_DE, "Monitor Kommandos aus der angegebenen Datei werden gelesen und ausgeführt.\n"
                                            "Dieses Kommando stoppt, wenn das Ende der Datei erreicht wurde, oder das\n"
                                            "Kommando STOP gelesen wurde."},
/* fr */ {IDGS_MON_PLAYBACK_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_PLAYBACK_DESCRIPTION_HU, "A monitor parancsokat beolvassa és végrehajtja a megadott fájlból.\n"
                                            "A fájl végéig halad, vagy egy beolvasott STOP parancsig."},
/* it */ {IDGS_MON_PLAYBACK_DESCRIPTION_IT, "I comandi del monitor sono letti ed eseguiti dal file specificato. Questo\n"
                                            "comando si arresta alla fine del file o quando viene letto il comando\n"
                                            "STOP."},
/* nl */ {IDGS_MON_PLAYBACK_DESCRIPTION_NL, "Monitor commandos van het opgegeven bestand zullen worden gelezen en\n"
                                            "uitgevoerd. Dit commando stopt aan het einde van het bestand of wanneer\n"
                                            "een STOP commando wordt gelezen."},
/* pl */ {IDGS_MON_PLAYBACK_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_PLAYBACK_DESCRIPTION_SV, "Monitorkommandon läses in och körs från den angivna filen. Kommandot\n"
                                            "stoppar vid filslut eller när ett STOP-kommando läses."},
/* tr */ {IDGS_MON_PLAYBACK_DESCRIPTION_TR, "Belirtilen dosyadaki monitör komutlarý okundu ve çalýþtýrýldý.  Bu\n"
                                            "komut dosya sonunda ya da STOP komutu okunduðunda sonlanýr."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_EXPRESSION,    N_("<expression>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_EXPRESSION_DA, "<udtryk>"},
/* de */ {IDGS_P_EXPRESSION_DE, "<Bedingung>"},
/* fr */ {IDGS_P_EXPRESSION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_EXPRESSION_HU, "<kifejezés>"},
/* it */ {IDGS_P_EXPRESSION_IT, "<espressione>"},
/* nl */ {IDGS_P_EXPRESSION_NL, "<expressie>"},
/* pl */ {IDGS_P_EXPRESSION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_EXPRESSION_SV, "<uttryck>"},
/* tr */ {IDGS_P_EXPRESSION_TR, "<ifade>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_PRINT_DESCRIPTION,    N_("Evaluate the specified expression and output the result.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_PRINT_DESCRIPTION_DA, "Evaluér det angivne udtryk og vis resultatet."},
/* de */ {IDGS_MON_PRINT_DESCRIPTION_DE, "Evaluiere den angegebenen Ausdruck und zeige das Ergebnis."},
/* fr */ {IDGS_MON_PRINT_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_PRINT_DESCRIPTION_HU, "Kiértékeli a megadott kifejezést és kiírja az eredményt."},
/* it */ {IDGS_MON_PRINT_DESCRIPTION_IT, "Calcola l'espressione specificata e mostra il risultato."},
/* nl */ {IDGS_MON_PRINT_DESCRIPTION_NL, "Evalueer de opgegeven expressie en toon het resultaat."},
/* pl */ {IDGS_MON_PRINT_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_PRINT_DESCRIPTION_SV, "Utvärdera det angivna uttrycket och visa resultatet."},
/* tr */ {IDGS_MON_PRINT_DESCRIPTION_TR, "Belirtilen ifadeyi deðerlendir ve sonucun çýktýsýný ver."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_RECORD_DESCRIPTION,    N_("After this command, all commands entered are written to the specified\n"
                                             "file until the STOP command is entered.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_RECORD_DESCRIPTION_DA, "Efter denne kommando skrives alle kommandoer til den angivne fil,\n"
                                          "indtil der angives en STOP-kommando."},
/* de */ {IDGS_MON_RECORD_DESCRIPTION_DE, "Nach diesem Kommando werden alle eingebenen Kommands in die angegebene\n"
                                          "Datei gespeicher solang bis das Kommando STOP eingegeben wurde."},
/* fr */ {IDGS_MON_RECORD_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_RECORD_DESCRIPTION_HU, "Ezután a parancs után az összes parancsot a megadott fájlba\n"
                                          "írja, amíg STOP parancs nem érkezik."},
/* it */ {IDGS_MON_RECORD_DESCRIPTION_IT, "Dopo questo comando, tutti i comandi inseriti saranno scritti sul file\n"
                                          "specificato fino a quando verrà inserito il comando STOP."},
/* nl */ {IDGS_MON_RECORD_DESCRIPTION_NL, "Na dit commando zullen alle opgegeven commandos worden geschreven naar\n"
                                          "het opgegeven bestand totdat het STOP command wordt opgegeven."},
/* pl */ {IDGS_MON_RECORD_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_RECORD_DESCRIPTION_SV, "Efter detta kommando kommer alla kommandon som anges att skrivas till\n"
                                          "den angivna filen tills ett STOP-kommando anges."},
/* tr */ {IDGS_MON_RECORD_DESCRIPTION_TR, "Bu komuttan sonra, tüm girilen komutlar ve STOP komutunu görene dek\n"
                                          "belirtilen dosyaya yazýlacaktýr."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_QP_RESOURCE,    N_("\"<resource>\"")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_QP_RESOURCE_DA, "\"<resource>\""},
/* de */ {IDGS_QP_RESOURCE_DE, "\"<Ressource>\""},
/* fr */ {IDGS_QP_RESOURCE_FR, ""},  /* fuzzy */
/* hu */ {IDGS_QP_RESOURCE_HU, "\"<erõforrás>\""},
/* it */ {IDGS_QP_RESOURCE_IT, "\"<risorsa>\""},
/* nl */ {IDGS_QP_RESOURCE_NL, "\"<resource>\""},
/* pl */ {IDGS_QP_RESOURCE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_QP_RESOURCE_SV, "\"<resurs>\""},
/* tr */ {IDGS_QP_RESOURCE_TR, "\"<kaynak>\""},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_RESOURCEGET_DESCRIPTION,    N_("Displays the value of the resource.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_RESOURCEGET_DESCRIPTION_DA, "Viser værdien for ressourcen."},
/* de */ {IDGS_MON_RESOURCEGET_DESCRIPTION_DE, "Zeige den Werde der Ressource."},
/* fr */ {IDGS_MON_RESOURCEGET_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_RESOURCEGET_DESCRIPTION_HU, "Az erõforrás értékének kiírása."},
/* it */ {IDGS_MON_RESOURCEGET_DESCRIPTION_IT, "Mostra il valore della risorsa."},
/* nl */ {IDGS_MON_RESOURCEGET_DESCRIPTION_NL, "Toont de waarde van de resource."},
/* pl */ {IDGS_MON_RESOURCEGET_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_RESOURCEGET_DESCRIPTION_SV, "Visar värdet för resursen."},
/* tr */ {IDGS_MON_RESOURCEGET_DESCRIPTION_TR, "Kaynaðýn deðerini görüntüler."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_QP_RESOURCE_QP_VALUE,    N_("\"<resource>\" \"<value>\"")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_QP_RESOURCE_QP_VALUE_DA, "\"<resource>\" \"<værdi>\""},
/* de */ {IDGS_QP_RESOURCE_QP_VALUE_DE, "\"<Ressource>\" \"<Wert>\""},
/* fr */ {IDGS_QP_RESOURCE_QP_VALUE_FR, ""},  /* fuzzy */
/* hu */ {IDGS_QP_RESOURCE_QP_VALUE_HU, "\"<erõforrás>\" \"<érték>\""},
/* it */ {IDGS_QP_RESOURCE_QP_VALUE_IT, "\"<risorsa>\" \"<valore>\""},
/* nl */ {IDGS_QP_RESOURCE_QP_VALUE_NL, "\"<resource>\" \"<waarde>\""},
/* pl */ {IDGS_QP_RESOURCE_QP_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_QP_RESOURCE_QP_VALUE_SV, "\"<resurs>\" \"<värde>\""},
/* tr */ {IDGS_QP_RESOURCE_QP_VALUE_TR, "\"<kaynak>\" \"<deðer>\""},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_RESOURCESET_DESCRIPTION,    N_("Sets the value of the resource.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_RESOURCESET_DESCRIPTION_DA, "Sætter værdien for ressourcen."},
/* de */ {IDGS_MON_RESOURCESET_DESCRIPTION_DE, "Setze den Werde der Ressource."},
/* fr */ {IDGS_MON_RESOURCESET_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_RESOURCESET_DESCRIPTION_HU, "Az erõforrás értékének beállítása."},
/* it */ {IDGS_MON_RESOURCESET_DESCRIPTION_IT, "Imposta il valore della risorsa."},
/* nl */ {IDGS_MON_RESOURCESET_DESCRIPTION_NL, "Plaats de waarde in de resource."},
/* pl */ {IDGS_MON_RESOURCESET_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_RESOURCESET_DESCRIPTION_SV, "Ställer in ett värde för resursen."},
/* tr */ {IDGS_MON_RESOURCESET_DESCRIPTION_TR, "Kaynaðýn deðerini belirler."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_STOP_DESCRIPTION,    N_("Stop recording commands.  See `record'.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_STOP_DESCRIPTION_DA, "Afslut optagelse. Se \"record\"."},
/* de */ {IDGS_MON_STOP_DESCRIPTION_DE, "Stoppe die Aufnahme von Kommandos. Siehe auch `Record'."},
/* fr */ {IDGS_MON_STOP_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_STOP_DESCRIPTION_HU, "A parancsok rögzítésének befejezése.  Lásd `record'"},
/* it */ {IDGS_MON_STOP_DESCRIPTION_IT, "Ferma il comando di registrazione. Guarda `registra'."},
/* nl */ {IDGS_MON_STOP_DESCRIPTION_NL, "Stop de opname van commandos. Zie 'record'."},
/* pl */ {IDGS_MON_STOP_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_STOP_DESCRIPTION_SV, "Sluta spela in kommandon. Se \"record\"."},
/* tr */ {IDGS_MON_STOP_DESCRIPTION_TR, "Kayýt komutlarýný durdur.  `record'a bak."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_QP_FILENAME_RP_FORMAT,    N_("\"<filename>\" [<format>]")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_QP_FILENAME_RP_FORMAT_DA, "\"<filnavn>\" [<format>]"},
/* de */ {IDGS_QP_FILENAME_RP_FORMAT_DE, "\"<Dateiname>\" [<Format>]"},
/* fr */ {IDGS_QP_FILENAME_RP_FORMAT_FR, ""},  /* fuzzy */
/* hu */ {IDGS_QP_FILENAME_RP_FORMAT_HU, "\"<fájlnév>\" [<formátum>]"},
/* it */ {IDGS_QP_FILENAME_RP_FORMAT_IT, "\"<nome del file>\" [<formato>]"},
/* nl */ {IDGS_QP_FILENAME_RP_FORMAT_NL, "\"<bestandsnaam>\" [<formaat>]"},
/* pl */ {IDGS_QP_FILENAME_RP_FORMAT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_QP_FILENAME_RP_FORMAT_SV, "\"<filnamn>\" [<format>]"},
/* tr */ {IDGS_QP_FILENAME_RP_FORMAT_TR, "\"<dosyaismi>\" [<biçim>]"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_SCREENSHOT_DESCRIPTION,    N_("Take a screenshot. Format is:\n"
                                                 "default = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_SCREENSHOT_DESCRIPTION_DA, "Gem et screenshot. Format er:\n"
                                              "standard = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
/* de */ {IDGS_MON_SCREENSHOT_DESCRIPTION_DE, "Erzeuge Screenshot. Format ist:\n"
                                              "Standard = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
/* fr */ {IDGS_MON_SCREENSHOT_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_SCREENSHOT_DESCRIPTION_HU, "Képernyõkép készítése. A formátum lehet:\n"
                                              "alap = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
/* it */ {IDGS_MON_SCREENSHOT_DESCRIPTION_IT, "Cattura schermo. Il formato è:\n"
                                              "predefinito = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
/* nl */ {IDGS_MON_SCREENSHOT_DESCRIPTION_NL, "Sla een schermafdruk op. Formaat is:\n"
                                              "standaard = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
/* pl */ {IDGS_MON_SCREENSHOT_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_SCREENSHOT_DESCRIPTION_SV, "Ta en skärmbild. Format är:\n"
                                              "standard = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
/* tr */ {IDGS_MON_SCREENSHOT_DESCRIPTION_TR, "Ekran görüntüsü al. Biçim:\n"
                                              "varsayýlan = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF."},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_P_COMMAND,    N_("<command>")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_P_COMMAND_DA, "<kommando>"},
/* de */ {IDGS_P_COMMAND_DE, "<Kommando>"},
/* fr */ {IDGS_P_COMMAND_FR, ""},  /* fuzzy */
/* hu */ {IDGS_P_COMMAND_HU, "<parancs>"},
/* it */ {IDGS_P_COMMAND_IT, "<commando>"},
/* nl */ {IDGS_P_COMMAND_NL, "<commando>"},
/* pl */ {IDGS_P_COMMAND_PL, ""},  /* fuzzy */
/* sv */ {IDGS_P_COMMAND_SV, "<kommando>"},
/* tr */ {IDGS_P_COMMAND_TR, "<komut>"},
#endif

/* monitor/mon_command.c */
/* en */ {IDGS_MON_TAPECTRL_DESCRIPTION,    N_("Control the datasette. Valid commands:\n"
                                               "0 = stop, 1 = start, 2 = forward, 3 = rewind, 4 = record,\n"
                                               "5 = reset, 6 = reset counter.")},
#ifdef HAS_TRANSLATION
/* da */ {IDGS_MON_TAPECTRL_DESCRIPTION_DA, "Styr datasette. Lovlige kommandoer:\n"
                                            "0 = stop, 1 = start, 2 = spol frem, 3 = spol tilbage,\n"
                                            "4 = optag, 5 = reset, 6 = nulstil tæller."},
/* de */ {IDGS_MON_TAPECTRL_DESCRIPTION_DE, "Steuere die Datasette. Gültige Kommandos sind:\n"
                                            "0 = Stop, 1 = Start, 2 = Forwärts, 3 = Rückwärts, 4 = Aufnahme,\n"
                                            "5 = Reset, 6 = Reset Zähler."},
/* fr */ {IDGS_MON_TAPECTRL_DESCRIPTION_FR, ""},  /* fuzzy */
/* hu */ {IDGS_MON_TAPECTRL_DESCRIPTION_HU, "Szalagos egység vezérlése. Érvényes parancsok:\n"
                                            "0 = start, 1 = indít, 2 = elõre, 3 = vissza, 4 = felvétel,\n"
                                            "5 = újraindít, 6 = számlálót nulláz"},
/* it */ {IDGS_MON_TAPECTRL_DESCRIPTION_IT, "Controlla il registratore. Comandi validi:\n"
                                            "0 = ferma, 1 = avvia, 2 = avanti, 3 = indietro, 4 = registra,\n"
                                            "5 = reset, 6 = reset contatore."},
/* nl */ {IDGS_MON_TAPECTRL_DESCRIPTION_NL, "Geef datasetta commandos. Geldige commandos:\n"
                                            "0 = stop, 1 = start, 2 = vooruit, 3 = terug, 4 = opnemen,\n"
                                            "5 = reset, 6 = reset teller."},
/* pl */ {IDGS_MON_TAPECTRL_DESCRIPTION_PL, ""},  /* fuzzy */
/* sv */ {IDGS_MON_TAPECTRL_DESCRIPTION_SV, "Styr bandspelaren. Giltiga kommandon:\n"
                                            "0 = stoppa, 1 = starta, 2 = spola framåt, 3 = spola bakåt,\n"
                                            "4 = spela in, 5 = återställ, 6 = nollställ räknare."},
/* tr */ {IDGS_MON_TAPECTRL_DESCRIPTION_TR, "Teybi kontrol et. Geçerli komutlar:\n"
                                            "0 = durdur, 1 = baþlat, 2 = ileri, 3 = geri, 4 = kayýt,\n"
                                            "5 = reset, 6 = reset sayacý."},
#endif

/* ------------------------ COMMAND LINE OPTION STRINGS -------------------- */

/* autostart.c */
/* en */ {IDCLS_ENABLE_AUTOSTARTWITHCOLON,    N_("On autostart, use the 'RUN' command with a colon, i.e., 'RUN:'")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_AUTOSTARTWITHCOLON_DA, "Brug \"RUN\"-kommando med kolon ved autostart, dvs. \"RUN:\""},
/* de */ {IDCLS_ENABLE_AUTOSTARTWITHCOLON_DE, "Bei Autostart soll 'RUN' mit Doppelpunkt ausgeführt werden ('RUN:')"},
/* fr */ {IDCLS_ENABLE_AUTOSTARTWITHCOLON_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_AUTOSTARTWITHCOLON_HU, "Automatikus indításnál a 'RUN' parancs kettõsponttal, vagyis 'RUN:'"},
/* it */ {IDCLS_ENABLE_AUTOSTARTWITHCOLON_IT, "All'avvio automatico, usa il comando 'RUN' con i due punti; cioè 'RUN:'"},
/* nl */ {IDCLS_ENABLE_AUTOSTARTWITHCOLON_NL, "Bij het autostarten, gebruik het 'RUN' commando met een dubbele punt, dus "
                                              "'RUN:'"},
/* pl */ {IDCLS_ENABLE_AUTOSTARTWITHCOLON_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_AUTOSTARTWITHCOLON_SV, "Vid autostart, använd \"RUN\"-kommando med kolon, dvs. \"RUN:\""},
/* tr */ {IDCLS_ENABLE_AUTOSTARTWITHCOLON_TR, "Otomatik baþlatýldýðýnda, 'RUN' komutunu ':' ile kullan, yani, 'RUN:'"},
#endif

/* autostart.c */
/* en */ {IDCLS_DISABLE_AUTOSTARTWITHCOLON,    N_("On autostart, do not use the 'RUN' command with a colon; i.e., 'RUN'")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_AUTOSTARTWITHCOLON_DA, "Brug ikke \"RUN\"-kommando med kolon ved autostart, dvs. \"RUN\""},
/* de */ {IDCLS_DISABLE_AUTOSTARTWITHCOLON_DE, "Bei Autostart soll 'RUN' ohne Doppelpunkt ausgeführt werden ('RUN')"},
/* fr */ {IDCLS_DISABLE_AUTOSTARTWITHCOLON_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_AUTOSTARTWITHCOLON_HU, "Automatikus indításnak a 'RUN' parancs kettõspont nélkül, vagyis 'RUN'"},
/* it */ {IDCLS_DISABLE_AUTOSTARTWITHCOLON_IT, "All'avvio automatico, non usare il comando 'RUN' con i due punti; cioè 'RUN'"},
/* nl */ {IDCLS_DISABLE_AUTOSTARTWITHCOLON_NL, "Bij het autostarten, gebruik het 'RUN' commando niet met een dubbele "
                                               "punt, dus 'RUN'"},
/* pl */ {IDCLS_DISABLE_AUTOSTARTWITHCOLON_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_AUTOSTARTWITHCOLON_SV, "Vid autostart, använd inte \"RUN\"-kommando med kolon, dvs. \"RUN\""},
/* tr */ {IDCLS_DISABLE_AUTOSTARTWITHCOLON_TR, "Otomatik baþlatýldýðýnda, 'RUN' komutunu ':' ile kullanma, yani, 'RUN'"},
#endif

/* autostart.c */
/* en */ {IDCLS_ENABLE_AUTOSTART_HANDLE_TDE,    N_("Handle True Drive Emulation on autostart")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_AUTOSTART_HANDLE_TDE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_AUTOSTART_HANDLE_TDE_DE, "Präzise Floppy Emulation bei Autostart beachten"},
/* fr */ {IDCLS_ENABLE_AUTOSTART_HANDLE_TDE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_AUTOSTART_HANDLE_TDE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_AUTOSTART_HANDLE_TDE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_AUTOSTART_HANDLE_TDE_NL, "Manipuleer hardwarematige drive emulatie bij autostarten"},
/* pl */ {IDCLS_ENABLE_AUTOSTART_HANDLE_TDE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_AUTOSTART_HANDLE_TDE_SV, "Hantera äkta diskettenhetsemulering vid autostart"},
/* tr */ {IDCLS_ENABLE_AUTOSTART_HANDLE_TDE_TR, ""},  /* fuzzy */
#endif

/* autostart.c */
/* en */ {IDCLS_DISABLE_AUTOSTART_HANDLE_TDE,    N_("Do not handle True Drive Emulation on autostart")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_AUTOSTART_HANDLE_TDE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_AUTOSTART_HANDLE_TDE_DE, "Präzise Floppy Emulation bei Autostart nicht beachten"},
/* fr */ {IDCLS_DISABLE_AUTOSTART_HANDLE_TDE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_AUTOSTART_HANDLE_TDE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_AUTOSTART_HANDLE_TDE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_AUTOSTART_HANDLE_TDE_NL, "Geen manipulatie van de hardwarematige drive emulatie bij autostarten"},
/* pl */ {IDCLS_DISABLE_AUTOSTART_HANDLE_TDE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_AUTOSTART_HANDLE_TDE_SV, "Hantera inte äkta diskettenhetsemulering vid autostart"},
/* tr */ {IDCLS_DISABLE_AUTOSTART_HANDLE_TDE_TR, ""},  /* fuzzy */
#endif

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_USE_PAL_SYNC_FACTOR,    N_("Use PAL sync factor")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_PAL_SYNC_FACTOR_DA, "Anvend PAL-synkfaktor"},
/* de */ {IDCLS_USE_PAL_SYNC_FACTOR_DE, "PAL Faktor zum Synchronisieren benutzen"},
/* fr */ {IDCLS_USE_PAL_SYNC_FACTOR_FR, "Utiliser PAL"},
/* hu */ {IDCLS_USE_PAL_SYNC_FACTOR_HU, "PAL szinkron faktor használata"},
/* it */ {IDCLS_USE_PAL_SYNC_FACTOR_IT, "Usa il fattore di sincronizzazione PAL"},
/* nl */ {IDCLS_USE_PAL_SYNC_FACTOR_NL, "Gebruik PAL-synchronisatiefaktor"},
/* pl */ {IDCLS_USE_PAL_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_PAL_SYNC_FACTOR_SV, "Använd PAL-synkfaktor"},
/* tr */ {IDCLS_USE_PAL_SYNC_FACTOR_TR, "PAL senkron faktörü kullan"},
#endif

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_USE_NTSC_SYNC_FACTOR,    N_("Use NTSC sync factor")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_NTSC_SYNC_FACTOR_DA, "Anvend NTSC-synkfaktor"},
/* de */ {IDCLS_USE_NTSC_SYNC_FACTOR_DE, "NTSC Sync Faktor benutzen"},
/* fr */ {IDCLS_USE_NTSC_SYNC_FACTOR_FR, "Utiliser NTSC"},
/* hu */ {IDCLS_USE_NTSC_SYNC_FACTOR_HU, "NTSC szinkron faktor használata"},
/* it */ {IDCLS_USE_NTSC_SYNC_FACTOR_IT, "Usa il fattore di sincronizzazione NTSC"},
/* nl */ {IDCLS_USE_NTSC_SYNC_FACTOR_NL, "Gebruik NTSC-synchronisatiefaktor"},
/* pl */ {IDCLS_USE_NTSC_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_NTSC_SYNC_FACTOR_SV, "Använd NTSC-synkfaktor"},
/* tr */ {IDCLS_USE_NTSC_SYNC_FACTOR_TR, "NTSC senkron faktörü kullan"},
#endif

/* c128/c128-cmdline-options.c, c128/functionrom.c,
   c64/c64-cmdline-options.c, c64/georam.c,
   c64/ramcart.c, c64/reu.c, c64/cart/c64cart.c,
   c64/cart/ide64.c, cbm2/cbm2-cmdline-options.c,
   drive/iec/iec-cmdline-options.c, drive/iec128dcr/iec128dcr-cmdline-options.c,
   drive/ieee/ieee-cmdline-options.c, drive/tcbm/tcbm-cmdline-options.c,
   fsdevice/fsdevice-cmdline-options.c, pet/pet-cmdline-options.c,
   plus4/plus4-cmdline-options.c, printerdrv/driver-select.c,
   printerdrv/output-select.c, printerdrv/output-text.c,
   rs232drv/rs232drv.c, vic20/vic20-cmdline-options.c,
   vic20/vic20cartridge.c, video/video-cmdline-options.c,
   fliplist.c, initcmdline.c, log.c, sound.c, c64/plus60k.c,
   c64/c64_256k.c, pet/petreu.c, c64/plus256k.c */
/* en */ {IDCLS_P_NAME,    N_("<name>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_NAME_DA, "<navn>"},
/* de */ {IDCLS_P_NAME_DE, "<Name>"},
/* fr */ {IDCLS_P_NAME_FR, "<nom>"},
/* hu */ {IDCLS_P_NAME_HU, "<név>"},
/* it */ {IDCLS_P_NAME_IT, "<nome>"},
/* nl */ {IDCLS_P_NAME_NL, "<naam>"},
/* pl */ {IDCLS_P_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_NAME_SV, "<namn>"},
/* tr */ {IDCLS_P_NAME_TR, "<isim>"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INT_KERNEL_NAME,    N_("Specify name of international Kernal ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_INT_KERNEL_NAME_DA, "Angiv navn på internationalt kernal-ROM-image"},
/* de */ {IDCLS_SPECIFY_INT_KERNEL_NAME_DE, "Dateiname des internationalen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_INT_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal internationale"},
/* hu */ {IDCLS_SPECIFY_INT_KERNEL_NAME_HU, "Adja meg a nemzetközi Kernal ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_INT_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal internazionale"},
/* nl */ {IDCLS_SPECIFY_INT_KERNEL_NAME_NL, "Geef de naam van het internationaal Kernal-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_INT_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INT_KERNEL_NAME_SV, "Ange namn på internationell kernal-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_INT_KERNEL_NAME_TR, "Uluslararasý Kernal ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_DE_KERNEL_NAME,    N_("Specify name of German Kernal ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_DE_KERNEL_NAME_DA, "Angiv navn på tysk kernal-ROM-image"},
/* de */ {IDCLS_SPECIFY_DE_KERNEL_NAME_DE, "Dateiname des deutschen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_DE_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal allemande"},
/* hu */ {IDCLS_SPECIFY_DE_KERNEL_NAME_HU, "Adja meg a német Kernal ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_DE_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal tedesca"},
/* nl */ {IDCLS_SPECIFY_DE_KERNEL_NAME_NL, "Geef de naam van het Duitse Kernal-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_DE_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_DE_KERNEL_NAME_SV, "Ange namn på tysk kernal-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_DE_KERNEL_NAME_TR, "Alman Kernal ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FI_KERNEL_NAME,    N_("Specify name of Finnish Kernal ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_FI_KERNEL_NAME_DA, "Angiv navn på finsk kernal-ROM-image"},
/* de */ {IDCLS_SPECIFY_FI_KERNEL_NAME_DE, "Dateiname des finnischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_FI_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal finlandaise"},
/* hu */ {IDCLS_SPECIFY_FI_KERNEL_NAME_HU, "Adja meg a finn Kernal ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_FI_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal finlandese"},
/* nl */ {IDCLS_SPECIFY_FI_KERNEL_NAME_NL, "Geef de naam van het Fins Kernal-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_FI_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FI_KERNEL_NAME_SV, "Ange namn på finsk kernal-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_FI_KERNEL_NAME_TR, "Finlandiya Kernal ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FR_KERNEL_NAME,    N_("Specify name of French Kernal ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_FR_KERNEL_NAME_DA, "Angiv navn på fransk kernal-ROM-image"},
/* de */ {IDCLS_SPECIFY_FR_KERNEL_NAME_DE, "Dateiname des französischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_FR_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal française"},
/* hu */ {IDCLS_SPECIFY_FR_KERNEL_NAME_HU, "Adja meg a francia Kernal ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_FR_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal francese"},
/* nl */ {IDCLS_SPECIFY_FR_KERNEL_NAME_NL, "Geef de naam van het Frans Kernal-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_FR_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FR_KERNEL_NAME_SV, "Ange namn på fransk kernal-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_FR_KERNEL_NAME_TR, "Fransýz Kernal ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_IT_KERNEL_NAME,    N_("Specify name of Italian Kernal ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_IT_KERNEL_NAME_DA, "Angiv navn på italiensk kernal-ROM-image"},
/* de */ {IDCLS_SPECIFY_IT_KERNEL_NAME_DE, "Dateiname des italienischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_IT_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal italienne"},
/* hu */ {IDCLS_SPECIFY_IT_KERNEL_NAME_HU, "Adja meg az olasz Kernal ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_IT_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal italiana"},
/* nl */ {IDCLS_SPECIFY_IT_KERNEL_NAME_NL, "Geef de naam van het Italiaans Kernal-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_IT_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_IT_KERNEL_NAME_SV, "Ange namn på italiensk kernal-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_IT_KERNEL_NAME_TR, "Ýtalyan Kernal ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_NO_KERNEL_NAME,    N_("Specify name of Norwegian Kernal ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_NO_KERNEL_NAME_DA, "Angiv navn på norsk kernal-ROM-image"},
/* de */ {IDCLS_SPECIFY_NO_KERNEL_NAME_DE, "Dateiname des norwegischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_NO_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal norvégienne"},
/* hu */ {IDCLS_SPECIFY_NO_KERNEL_NAME_HU, "Adja meg a norvég Kernal ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_NO_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal norvegese"},
/* nl */ {IDCLS_SPECIFY_NO_KERNEL_NAME_NL, "Geef de naam van het Noors Kernal-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_NO_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_NO_KERNEL_NAME_SV, "Ange namn på norsk kernal-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_NO_KERNEL_NAME_TR, "Norveç Kernal ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SV_KERNEL_NAME,    N_("Specify name of Swedish Kernal ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SV_KERNEL_NAME_DA, "Angiv navn på svensk kernal-ROM-image"},
/* de */ {IDCLS_SPECIFY_SV_KERNEL_NAME_DE, "Dateiname des schwedischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_SV_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal suédoise"},
/* hu */ {IDCLS_SPECIFY_SV_KERNEL_NAME_HU, "Adja meg s svéd Kernal ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_SV_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal svedese"},
/* nl */ {IDCLS_SPECIFY_SV_KERNEL_NAME_NL, "Geef de naam van het Zweeds Kernal-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_SV_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SV_KERNEL_NAME_SV, "Ange namn på svensk kernal-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_SV_KERNEL_NAME_TR, "Ýsveç Kernal ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW,    N_("Specify name of BASIC ROM image (lower part)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_DA, "Angiv navn på BASIC ROM-image (lav del)"},
/* de */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_DE, "Dateiname des Basic ROMs (oberer Adressbereich"},
/* fr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_FR, "Spécifier le nom de l'image BASIC ROM (partie basse)"},
/* hu */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_HU, "Adja meg a BASIC ROM képmás nevét (alsó rész)"},
/* it */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_IT, "Specifica il nome dell'immagine della ROM del BASIC (parte inferiore)"},
/* nl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_NL, "Geef de naam van het BASIC-ROM-bestand (laag gedeelte)"},
/* pl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_SV, "Ange namn på BASIC ROM-avbildning (nedre delen)"},
/* tr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_TR, "BASIC ROM imajýnýn (alt kýsým) ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH,    N_("Specify name of BASIC ROM image (higher part)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_DA, "Angiv navn på BASIC ROM-image (høj del)"},
/* de */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_DE, "Dateiname des Basic ROMs (unterer Adressbereich)"},
/* fr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_FR, "Spécifier le nom de l'image BASIC ROM (partie haute)"},
/* hu */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_HU, "Adja meg a BASIC ROM képmás nevét (felsõ rész)"},
/* it */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_IT, "Specifica il nome dell'immagine della ROM del BASIC (parte superiore)"},
/* nl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_NL, "Geef de naam van het BASIC-ROM-bestand (hoog gedeelte)"},
/* pl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_SV, "Ange namn på BASIC ROM-avbildning (övre delen)"},
/* tr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_TR, "BASIC ROM imajýnýn (üst kýsým) ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME,    N_("Specify name of international character generator ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_DA, "Angiv navn på internationalt tegngenerator-ROM-image"},
/* de */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_DE, "Dateiname des internationalen Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_FR, "Spécifier le nom de l'image du générateur de caractères internationaux"},
/* hu */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_HU, "Adja meg a nemzetközi karaktergenerátor ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del generatore di caratteri "
                                                 "internazionale"},
/* nl */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_NL, "Geef de naam van het internationaal CHARGEN-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_SV, "Ange namn på internationell teckengenerator-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_TR, "Uluslararasý Karakter Oluþturucu ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME,    N_("Specify name of German character generator ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_DA, "Angiv navn på tysk tegngenerator-ROM-image"},
/* de */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_DE, "Dateiname des deutschen Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_FR, "Spécifier le nom de l'image du générateur de caractères allemands"},
/* hu */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_HU, "Adja meg a német karaktergenerátor ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del generatore di caratteri "
                                                "tedesco"},
/* nl */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_NL, "Geef de naam van het Duitse CHARGEN-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_SV, "Ange namn på tysk teckengenerator-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_TR, "Alman Karakter Oluþturucu ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME,    N_("Specify name of French character generator ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_DA, "Angiv navn på fransk tegngenerator-ROM-image"},
/* de */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_DE, "Dateiname des französischen Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_FR, "Spécifier le nom de l'image du générateur de caractères français"},
/* hu */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_HU, "Adja meg a francia karaktergenerátor ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del generatore di caratteri "
                                                "francese"},
/* nl */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_NL, "Geef de naam van het Franse CHARGEN-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_SV, "Ange namn på fransk teckengenerator-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_TR, "Finlandiya Karakter Oluþturucu ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME,    N_("Specify name of Swedish character generator ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_DA, "Angiv navn på svensk tegngenerator-ROM-image"},
/* de */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_DE, "Dateiname des swedischen Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_FR, "Spécifier le nom de l'image du générateur de caractères suédois"},
/* hu */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_HU, "Adja meg a svéd karaktergenerátor ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del generatore di caratteri "
                                                "svedese"},
/* nl */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_NL, "Geef de naam van het Zweedse CHARGEN-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_SV, "Ange namn på svensk teckengenerator-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_TR, "Ýsveç Karakter Oluþturucu ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME,    N_("Specify name of C64 mode Kernal ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_DA, "Angiv navn på C64-kernal-ROM-image"},
/* de */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_DE, "Dateiname des Kernal ROMs im C64 Modus"},
/* fr */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_FR, "Spécifier le nom de l'image Kernal du mode C64"},
/* hu */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_HU, "Adja meg a C64 módú Kernal ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal in modalità C64"},
/* nl */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_NL, "Geef de naam van het Kernal-ROM-bestand voor de C64-modus"},
/* pl */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_SV, "Ange namn på C64-kernal-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_TR, "C64 modu Kernal ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME,    N_("Specify name of C64 mode BASIC ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_DA, "Angiv navn på C64-BASIC-ROM-image"},
/* de */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_DE, "Dateiname des Basic ROMs im C64 Modus"}, 
/* fr */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_FR, "Spécifier le nom de l'image BASIC ROM du mode C64"},
/* hu */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_HU, "Adja meg a C64 módú BASIC ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_IT, "Specifica il nome dell'immagine della ROM in modalità C64"},
/* nl */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_NL, "Geef de naam van het BASIC-ROM-bestand voor de C64-modus"},
/* pl */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_SV, "Ange namn på C64-BASIC-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_TR, "C64 modu BASIC ROM imajýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_ENABLE_EMULATOR_ID,    N_("Enable emulator identification")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_EMULATOR_ID_DA, "Aktivér emulatoridentifikation"},
/* de */ {IDCLS_ENABLE_EMULATOR_ID_DE, "Emulatoridentifikation aktivieren"},
/* fr */ {IDCLS_ENABLE_EMULATOR_ID_FR, "Activer l'identification de l'émulateur"},
/* hu */ {IDCLS_ENABLE_EMULATOR_ID_HU, "Emulátor azonosító engedélyezése"},
/* it */ {IDCLS_ENABLE_EMULATOR_ID_IT, "Attiva l'identificazione dell'emulatore"},
/* nl */ {IDCLS_ENABLE_EMULATOR_ID_NL, "Activeer emulatoridentificatie"},
/* pl */ {IDCLS_ENABLE_EMULATOR_ID_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EMULATOR_ID_SV, "Aktivera emulatoridentifiering"},
/* tr */ {IDCLS_ENABLE_EMULATOR_ID_TR, "Emülatör tespitini aktif et"},
#endif

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_DISABLE_EMULATOR_ID,    N_("Disable emulator identification")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_EMULATOR_ID_DA, "Deaktivér emulatoridentifikation"},
/* de */ {IDCLS_DISABLE_EMULATOR_ID_DE, "Emulatoridentifikation deaktivieren"},
/* fr */ {IDCLS_DISABLE_EMULATOR_ID_FR, "Désactiver l'identification de l'émulateur"},
/* hu */ {IDCLS_DISABLE_EMULATOR_ID_HU, "Emulátor azonosító tiltása"},
/* it */ {IDCLS_DISABLE_EMULATOR_ID_IT, "Disattiva l'identificazione dell'emulatore"},
/* nl */ {IDCLS_DISABLE_EMULATOR_ID_NL, "Emulatoridentificatie uitschakelen"},
/* pl */ {IDCLS_DISABLE_EMULATOR_ID_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_EMULATOR_ID_SV, "Inaktivera emulatoridentifiering"},
/* tr */ {IDCLS_DISABLE_EMULATOR_ID_TR, "Emülatör tespitini pasifleþtir"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_ENABLE_IEE488,    N_("Enable the IEEE488 interface emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEE488_DA, "Aktivér emulering af IEEE488-interface"},
/* de */ {IDCLS_ENABLE_IEE488_DE, "IEEE488 Schnittstellenemulation aktivieren"},
/* fr */ {IDCLS_ENABLE_IEE488_FR, "Activer l'interface d'émulation IEEE488"},
/* hu */ {IDCLS_ENABLE_IEE488_HU, "IEEE488 interfész emuláció engedélyezése"},
/* it */ {IDCLS_ENABLE_IEE488_IT, "Attiva l'emulazione dell'interfaccia IEEE488"},
/* nl */ {IDCLS_ENABLE_IEE488_NL, "Activeer de IEEE488-interface-emulatie"},
/* pl */ {IDCLS_ENABLE_IEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEE488_SV, "Aktivera emulering av IEEE488-gränssnitt"},
/* tr */ {IDCLS_ENABLE_IEE488_TR, "IEEE488 arabirim emülasyonunu aktif et"},
#endif

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_DISABLE_IEE488,    N_("Disable the IEEE488 interface emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEE488_DA, "Deaktiver emulering af IEEE488-interface"},
/* de */ {IDCLS_DISABLE_IEE488_DE, "IEEE488 Schnittstellenemulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEE488_FR, "Désactiver l'interface d'émulation IEEE488"},
/* hu */ {IDCLS_DISABLE_IEE488_HU, "IEEE488 interfész emuláció tiltása"},
/* it */ {IDCLS_DISABLE_IEE488_IT, "Disattiva l'emulazione dell'interfaccia IEEE488"},
/* nl */ {IDCLS_DISABLE_IEE488_NL, "De IEEE488-interface-emulatie uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEE488_SV, "Inaktivera emulering av IEEE488-gränssnitt"},
/* tr */ {IDCLS_DISABLE_IEE488_TR, "IEEE488 arabirim emülasyonunu pasifleþtir"},
#endif

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_P_REVISION,    N_("<revision>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_REVISION_DA, "<revision>"},
/* de */ {IDCLS_P_REVISION_DE, "<Revision>"},
/* fr */ {IDCLS_P_REVISION_FR, "<revision>"},
/* hu */ {IDCLS_P_REVISION_HU, "<változat>"},
/* it */ {IDCLS_P_REVISION_IT, "<revisione>"},
/* nl */ {IDCLS_P_REVISION_NL, "<revisie>"},
/* pl */ {IDCLS_P_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_REVISION_SV, "<utgåva>"},
/* tr */ {IDCLS_P_REVISION_TR, "<revizyon>"},
#endif

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_PATCH_KERNAL_TO_REVISION,    N_("Patch the Kernal ROM to the specified <revision>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PATCH_KERNAL_TO_REVISION_DA, "Patch kerne-ROM til den angivne <revision>"},
/* de */ {IDCLS_PATCH_KERNAL_TO_REVISION_DE, "Kernal ROM auf angegenbene Revision ändern"},
/* fr */ {IDCLS_PATCH_KERNAL_TO_REVISION_FR, "Mettre à jour le ROM Kernal à la <revision> spécifiée"},
/* hu */ {IDCLS_PATCH_KERNAL_TO_REVISION_HU, "A Kernal ROM átírása adott <változatra>"},
/* it */ {IDCLS_PATCH_KERNAL_TO_REVISION_IT, "Fai il patch della ROM del Kernal alla <revisione> specificata"},
/* nl */ {IDCLS_PATCH_KERNAL_TO_REVISION_NL, "Verbeter de Kernal-ROM naar de opgegeven <revisie>"},
/* pl */ {IDCLS_PATCH_KERNAL_TO_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PATCH_KERNAL_TO_REVISION_SV, "Patcha kernal-ROM till angiven <utgåva>"},
/* tr */ {IDCLS_PATCH_KERNAL_TO_REVISION_TR, "Kernal ROM'u belirtilmiþ <revizyon>'a yama"},
#endif

#ifdef HAVE_RS232
/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU,    N_("Enable the $DE** ACIA RS232 interface emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_DA, "Aktivér $DE** ACIA RS232-interface emulering"},
/* de */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_DE, "$DE** ACIA RS232 Schnittstellenemulation aktivieren"},
/* fr */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_FR, "Activer l'émulation de l'interface $DE** ACIA RS232"},
/* hu */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_HU, "$DE** ACIA RS232 interfész emuláció engedélyezése"},
/* it */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_IT, "Attiva l'emulazione dell'interfaccia RS232 su ACIA a $DE**"},
/* nl */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_NL, "Activeer de $DE** ACIA RS232-interface-emulatie"},
/* pl */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_SV, "Aktivera $DE** ACIA RS232-gränssnittsemulering"},
/* tr */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_TR, "$DE** ACIA RS232 arabirim emülasyonunu aktif et"},
#endif

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU,    N_("Disable the $DE** ACIA RS232 interface emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_DA, "Deaktiver $DE** ACIA-RS232-interface emulering"},
/* de */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_DE, "$DE** ACIA RS232 Schnittstellenemulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_FR, "Désactiver l'émulation de l'interface $DE** ACIA RS232"},
/* hu */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_HU, "$DE** ACIA RS232 interfész emuláció tiltása"},
/* it */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_IT, "Disattiva l'emulazione dell'interfaccia RS232 su ACIA a $DE**"},
/* nl */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_NL, "De $DE** ACIA RS232 interface emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_SV, "Inaktivera $DE** ACIA-RS232-gränssnittsemulering"},
/* tr */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_TR, "$DE** ACIA RS232 arabirim emülasyonunu pasifleþtir"},
#endif
#endif

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vdc/vdc-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_P_NUMBER,    N_("<number>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_NUMBER_DA, "<nummer>"},
/* de */ {IDCLS_P_NUMBER_DE, "<Nummer>"},
/* fr */ {IDCLS_P_NUMBER_FR, "<numero>"},
/* hu */ {IDCLS_P_NUMBER_HU, "<szám>"},
/* it */ {IDCLS_P_NUMBER_IT, "<numero>"},
/* nl */ {IDCLS_P_NUMBER_NL, "<nummer>"},
/* pl */ {IDCLS_P_NUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_NUMBER_SV, "<nummer>"},
/* tr */ {IDCLS_P_NUMBER_TR, "<sayý>"},
#endif

#ifdef COMMON_KBD
/* c128/c128-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX,    N_("Specify index of keymap file (0=symbol, 1=positional)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_DA, "Angiv indeks for tastaturindstillingsfil (0=symbolsk, 1=positionsbestemt)"},
/* de */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_DE, "Aktive Tastaturbelegung (0=symbolisch) (1=positionell)"},
/* fr */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_FR, "Spécifier l'index du fichier keymap (0=symboles, 1=position)"},
/* hu */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_HU, "Adja meg a billentyûzet leképzési módot (0=szimbolikus, 1=pozíció "
                                              "szerinti)"},
/* it */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_IT, "Specifica l'indice del file della mappa della tastiera (0=simbolico, "
                                              "1=posizionale)"},
/* nl */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_NL, "Geef index van het keymapbestand (0=symbool, 1=positioneel)"},
/* pl */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_SV, "Ange index för för tangentbordsinställningsfil (0=symbolisk, "
                                              "1=positionsriktig)"},
/* tr */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_TR, "Tuþ haritasý dosyasýnýn indeksini belirt (0=sembol, 1=konumsal)"},
#endif

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   plus4/plus4-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME,    N_("Specify name of symbolic keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_DA, "Angiv filnavn for symbolsk tastatur"},
/* de */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_DE, "Dateiname für symbolische Tastaturbelegung"},
/* fr */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_FR, "Spécifier le nom du fichier de mappage clavier symbolique"},
/* hu */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_HU, "Adja meg a szimbolikus leképzésfájl nevét"},
/* it */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_IT, "Specifica il nome del file della mappa simbolica della tastiera"},
/* nl */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_NL, "Geef naam van het symbolisch keymapbestand"},
/* pl */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_SV, "Ange fil för symbolisk tangentbordsemulering"},
/* tr */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_TR, "Sembolik tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   plus4/plus4-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME,    N_("Specify name of positional keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_DA, "Angiv fil for positionsbestemt tastatur"},
/* de */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_DE, "Dateiname für positionelle Tastaturbelegung"},
/* fr */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_FR, "Spécifier le nom du fichier de mappage clavier positionnel"},
/* hu */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_HU, "Adja meg a pozíció szerinti leképzésfájl nevét"},
/* it */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_IT, "Specifica il nome del file della mappa posizionale della tastiera"},
/* nl */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_NL, "Geef naam van het positioneel keymapbestand"},
/* pl */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_SV, "Ange fil för positionsriktig tangentbordsemulering"},
/* tr */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_TR, "Konumsal tuþ haritasý dosyasýnýn ismini belirt"},
#endif
#endif

/* c128/c128mmu.c */
/* en */ {IDCLS_ACTIVATE_40_COL_MODE,    N_("Activate 40 column mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ACTIVATE_40_COL_MODE_DA, "Aktivér 40-kolonners tilstand"},
/* de */ {IDCLS_ACTIVATE_40_COL_MODE_DE, "40 Zeichen Modus"},
/* fr */ {IDCLS_ACTIVATE_40_COL_MODE_FR, "Activer le mode 40 colonnes"},
/* hu */ {IDCLS_ACTIVATE_40_COL_MODE_HU, "40 oszlopos mód aktiválása"},
/* it */ {IDCLS_ACTIVATE_40_COL_MODE_IT, "Attiva la modalità a 40 colonne"},
/* nl */ {IDCLS_ACTIVATE_40_COL_MODE_NL, "Activeer 40 kolomsmodus"},
/* pl */ {IDCLS_ACTIVATE_40_COL_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ACTIVATE_40_COL_MODE_SV, "Aktivera 40-kolumnersläge"},
/* tr */ {IDCLS_ACTIVATE_40_COL_MODE_TR, "40 sütun modunu aktif et"},
#endif

/* c128/c128mmu.c */
/* en */ {IDCLS_ACTIVATE_80_COL_MODE,    N_("Activate 80 column mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ACTIVATE_80_COL_MODE_DA, "Aktivér 80-kolonners tilstand"},
/* de */ {IDCLS_ACTIVATE_80_COL_MODE_DE, "80 Zeichen Modus"},
/* fr */ {IDCLS_ACTIVATE_80_COL_MODE_FR, "Activer le mode 80 colonnes"},
/* hu */ {IDCLS_ACTIVATE_80_COL_MODE_HU, "80 oszlopos mód aktiválása"},
/* it */ {IDCLS_ACTIVATE_80_COL_MODE_IT, "Attiva la modalità a 80 colonne"},
/* nl */ {IDCLS_ACTIVATE_80_COL_MODE_NL, "Activeer 80 kolomsmodus"},
/* pl */ {IDCLS_ACTIVATE_80_COL_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ACTIVATE_80_COL_MODE_SV, "Aktivera 80-kolumnersläge"},
/* tr */ {IDCLS_ACTIVATE_80_COL_MODE_TR, "80 sütun modunu aktif et"},
#endif

/* c128/c128mmu.c */
/* en */ {IDCLS_GO64_MODE,    N_("Always switch to C64 mode on reset")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_GO64_MODE_DA, "Skift altid til C64-tilstand ved reset"},
/* de */ {IDCLS_GO64_MODE_DE, "Immer bei Reset in C64 Modus schalten"},
/* fr */ {IDCLS_GO64_MODE_FR, "Toujours revenir au mode C64 au redémarrage"},
/* hu */ {IDCLS_GO64_MODE_HU, "Mindig C64 módban újraindítás után"},
/* it */ {IDCLS_GO64_MODE_IT, "Cambia sempre in modalita C64 al reset"},
/* nl */ {IDCLS_GO64_MODE_NL, "Start altijd op in C64-modus na reset"},
/* pl */ {IDCLS_GO64_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_GO64_MODE_SV, "Växla alltid till C64-läge vid återställning"},
/* tr */ {IDCLS_GO64_MODE_TR, "Resetlendiðinde her zaman C64 moduna geç"},
#endif

/* c128/c128mmu.c */
/* en */ {IDCLS_GO128_MODE,    N_("Always switch to C128 mode on reset")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_GO128_MODE_DA, "Skift altid til C128-tilstand ved reset"},
/* de */ {IDCLS_GO128_MODE_DE, "Immer be Reset in C128 Modus schalten"},
/* fr */ {IDCLS_GO128_MODE_FR, "Toujours revenir au mode C128 au redémarrage"},
/* hu */ {IDCLS_GO128_MODE_HU, "Mindig C128 mód újraindítás után"},
/* it */ {IDCLS_GO128_MODE_IT, "Cambia sempre in modalita C128 al reset"},
/* nl */ {IDCLS_GO128_MODE_NL, "Start altijd op in C128-modus na reset"},
/* pl */ {IDCLS_GO128_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_GO128_MODE_SV, "Växla alltid till C128-läge vid återställning"},
/* tr */ {IDCLS_GO128_MODE_TR, "Resetlendiðinde her zaman C128 moduna geç"},
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME,    N_("Specify name of internal Function ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_DA, "Angiv navn på internt funktions-ROM-image"},
/* de */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_DE, "Dateiname des internen Funktions ROMs"},
/* fr */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_FR, "Spécifier le nom de l'image ROM des fonctions internes"},
/* hu */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_HU, "Adja meg a belsõ Function ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_IT, "Specifica il nome dell'immagine della Function ROM interna"},
/* nl */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_NL, "Geef de naam van het intern functie-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_SV, "Ange namn på intern funktions-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_TR, "Dahili Function ROM imajýnýn ismini belirt"},
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME,    N_("Specify name of external Function ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_DA, "Angiv navn på externt funktions-ROM-image"},
/* de */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_DE, "Dateiname des externen Funktions ROMs"},
/* fr */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_FR, "Spécifier le nom de l'image ROM des fonctions externes"},
/* hu */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_HU, "Adja meg a külsõ Function ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_IT, "Specifica il nome dell'immagine della Function ROM esterna"},
/* nl */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_NL, "Geef de naam van het extern functie-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_SV, "Ange namn på extern funktions-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_TR, "Harici Function ROM imajýnýn ismini belirt"},
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_INT_FUNC_ROM,    N_("Enable the internal Function ROM")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_INT_FUNC_ROM_DA, "Aktivér intern funktions-ROM"},
/* de */ {IDCLS_ENABLE_INT_FUNC_ROM_DE, "Internes Funktions ROM aktivieren"},
/* fr */ {IDCLS_ENABLE_INT_FUNC_ROM_FR, "Activer l'image ROM des fonctions internes"},
/* hu */ {IDCLS_ENABLE_INT_FUNC_ROM_HU, "Belsõ Function ROM engedélyezése"},
/* it */ {IDCLS_ENABLE_INT_FUNC_ROM_IT, "Attiva la Function ROM interna"},
/* nl */ {IDCLS_ENABLE_INT_FUNC_ROM_NL, "Activeer de interne functie-ROM"},
/* pl */ {IDCLS_ENABLE_INT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_INT_FUNC_ROM_SV, "Aktivera internt funktions-ROM"},
/* tr */ {IDCLS_ENABLE_INT_FUNC_ROM_TR, "Dahili Function ROM'u aktif et"},
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_DISABLE_INT_FUNC_ROM,    N_("Disable the internal Function ROM")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_INT_FUNC_ROM_DA, "Deaktiver intern funktions-ROM"},
/* de */ {IDCLS_DISABLE_INT_FUNC_ROM_DE, "Internes Funktions ROM deaktivieren"},
/* fr */ {IDCLS_DISABLE_INT_FUNC_ROM_FR, "Désactiver l'image ROM des fonctions internes"},
/* hu */ {IDCLS_DISABLE_INT_FUNC_ROM_HU, "Belsõ Function ROM tiltása"},
/* it */ {IDCLS_DISABLE_INT_FUNC_ROM_IT, "Disattiva la Function ROM interna"},
/* nl */ {IDCLS_DISABLE_INT_FUNC_ROM_NL, "De interne functie-ROM uitschakelen"},
/* pl */ {IDCLS_DISABLE_INT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_INT_FUNC_ROM_SV, "Inaktivera internt funktions-ROM"},
/* tr */ {IDCLS_DISABLE_INT_FUNC_ROM_TR, "Dahili Function ROM'u pasifleþtir"},
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_EXT_FUNC_ROM,    N_("Enable the external Function ROM")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_EXT_FUNC_ROM_DA, "Aktivér extern funktions-ROM"},
/* de */ {IDCLS_ENABLE_EXT_FUNC_ROM_DE, "Externes Funktions ROM aktivieren"},
/* fr */ {IDCLS_ENABLE_EXT_FUNC_ROM_FR, "Activer l'image ROM des fonctions externes"},
/* hu */ {IDCLS_ENABLE_EXT_FUNC_ROM_HU, "Külsõ Function ROM engedélyezése"},
/* it */ {IDCLS_ENABLE_EXT_FUNC_ROM_IT, "Attiva la Function ROM esterna"},
/* nl */ {IDCLS_ENABLE_EXT_FUNC_ROM_NL, "Activeer de externe functie-ROM"},
/* pl */ {IDCLS_ENABLE_EXT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EXT_FUNC_ROM_SV, "Aktivera externt funktions-ROM"},
/* tr */ {IDCLS_ENABLE_EXT_FUNC_ROM_TR, "Harici Function ROM'u aktif et"},
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_DISABLE_EXT_FUNC_ROM,    N_("Disable the external Function ROM")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_EXT_FUNC_ROM_DA, "Deaktiver externt funktions-ROM"},
/* de */ {IDCLS_DISABLE_EXT_FUNC_ROM_DE, "Externes Funktions ROM deaktivieren"},
/* fr */ {IDCLS_DISABLE_EXT_FUNC_ROM_FR, "Désactiver l'image ROM des fonctions externes"},
/* hu */ {IDCLS_DISABLE_EXT_FUNC_ROM_HU, "Külsõ Function ROM tiltása"},
/* it */ {IDCLS_DISABLE_EXT_FUNC_ROM_IT, "Disattiva la Function ROM esterna"},
/* nl */ {IDCLS_DISABLE_EXT_FUNC_ROM_NL, "De externe functie-ROM uitschakelen"},
/* pl */ {IDCLS_DISABLE_EXT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_EXT_FUNC_ROM_SV, "Inaktivera externt funktions-ROM"},
/* tr */ {IDCLS_DISABLE_EXT_FUNC_ROM_TR, "Harici Function ROM'u pasifleþtir"},
#endif

/* c64/c64-cmdline-options.c, c64/psid.c */
/* en */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR,    N_("Use old NTSC sync factor")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_DA, "Anvend gammel NTSC-synkfaktor"},
/* de */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_DE, "NTSC (alt) Faktor zum Synchronisieren benutzen"},
/* fr */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_FR, "Utiliser l'ancien mode NTSC"},
/* hu */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_HU, "Régi NTSC szinkron faktor használata"},
/* it */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_IT, "Usa il vecchio fattore di sincronizzazione NTSC"},
/* nl */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_NL, "Gebruik oude NTSC-synchronisatiefactor"},
/* pl */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_SV, "Använd gammal NTSC-synkfaktor"},
/* tr */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_TR, "Eski NTSC senkron faktörü kullan"},
#endif

/* c64dtv/c64dtv-cmdline-options.c */
/* en */ {IDCLS_SELECT_HUMMER_USERPORT_DEVICE,    N_("Select Hummer Userport device (0: None, 1: ADC, 2: Joystick)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SELECT_HUMMER_USERPORT_DEVICE_DA, "Vælg enhed for Hummer brugerport (0: ingen, 1: ADC, 2: joystick)"},
/* de */ {IDCLS_SELECT_HUMMER_USERPORT_DEVICE_DE, "Hummer Userport Gerät auswählen (0: Kein, 1: ADC, 2: Joystick)"},
/* fr */ {IDCLS_SELECT_HUMMER_USERPORT_DEVICE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SELECT_HUMMER_USERPORT_DEVICE_HU, "Hummer Userport eszköz megadása (0: nincs, 1: ADC, 2: Joystick)"},
/* it */ {IDCLS_SELECT_HUMMER_USERPORT_DEVICE_IT, "Seleziona il dispositivo sulla userport dell'Hummer (0: Nessuno, 1:"
                                                  " ADC, 2: Joystick)"},
/* nl */ {IDCLS_SELECT_HUMMER_USERPORT_DEVICE_NL, "Selecteer Hummer Userport apparaat (0: Geen, 1: ADC, 2: Joystick"},
/* pl */ {IDCLS_SELECT_HUMMER_USERPORT_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SELECT_HUMMER_USERPORT_DEVICE_SV, "Välj enhet för Hummeranvändarport (0: ingen, 1: ADC, 2: styrspak)"},
/* tr */ {IDCLS_SELECT_HUMMER_USERPORT_DEVICE_TR, "Hummer Userport aygýtýný seç (0: Yok, 1: ADC, 2: Joystick)"},
#endif

/* c64dtv/c64dtv-cmdline-options.c */
/* en */ {IDCLS_P_PORT,    N_("<port>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_PORT_DA, "<port>"},
/* de */ {IDCLS_P_PORT_DE, "<Port>"},
/* fr */ {IDCLS_P_PORT_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_P_PORT_HU, "<port>"},
/* it */ {IDCLS_P_PORT_IT, "<porta>"},
/* nl */ {IDCLS_P_PORT_NL, "<poort>"},
/* pl */ {IDCLS_P_PORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_PORT_SV, "<port>"},
/* tr */ {IDCLS_P_PORT_TR, "<port>"},
#endif

/* c64dtv/c64dtv-cmdline-options.c */
/* en */ {IDCLS_HUMMER_JOY_PORT,    N_("Select which joystick port should be mapped to the userport")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_HUMMER_JOY_PORT_DA, "Vælg hvilken joystick port der skal kobles til brugerporten"},
/* de */ {IDCLS_HUMMER_JOY_PORT_DE, "Wähle welches Joystickport dem Userport zugeordnet werden soll"},
/* fr */ {IDCLS_HUMMER_JOY_PORT_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_HUMMER_JOY_PORT_HU, "Válassza ki a joystick portot, amely a userport-ra képzõdik le"},
/* it */ {IDCLS_HUMMER_JOY_PORT_IT, "Seleziona quali porte joystick devono essere mappate sulla userport"},
/* nl */ {IDCLS_HUMMER_JOY_PORT_NL, "Selecteer welke joystick poort moet worden omgeleidt naar de userport"},
/* pl */ {IDCLS_HUMMER_JOY_PORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_HUMMER_JOY_PORT_SV, "Ange vilken spelport som skall kopplas till användarporten"},
/* tr */ {IDCLS_HUMMER_JOY_PORT_TR, "Bu userporta hangi joystick portunun eþleþtirilmesi gerektiðini seçin"},
#endif

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KERNAL_ROM_NAME,    N_("Specify name of Kernal ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_DA, "Angiv navn på kernal-ROM-image"},
/* de */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_DE, "Dateiname des Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_FR, "Spécifier le nom de l'image ROM Kernal"},
/* hu */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_HU, "Adja meg a Kernal ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal"},
/* nl */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_NL, "Geef de naam van het Kernal-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_SV, "Ange namn på kernal-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_TR, "Kernal ROM imajýnýn ismini belirt"},
#endif

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME,    N_("Specify name of BASIC ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_BASIC_ROM_NAME_DA, "Angiv navn på BASIC-ROM-image"},
/* de */ {IDCLS_SPECIFY_BASIC_ROM_NAME_DE, "Dateiname des Basic ROMs"},
/* fr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_FR, "Spécifier le nom de l'image BASIC ROM"},
/* hu */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HU, "Adja meg a BASIC ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_BASIC_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del BASIC"},
/* nl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_NL, "Geef de naam van het BASIC-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BASIC_ROM_NAME_SV, "Ange namn på BASIC-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_TR, "BASIC ROM imajýnýn ismini belirt"},
#endif

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME,    N_("Specify name of character generator ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_DA, "Angiv navn på BASIC-ROM-image"},
/* de */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_DE, "Dateiname des Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_FR, "Spécifier le nom de l'image du générateur de caractères"},
/* hu */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_HU, "Adja meg a karaktergenerátor ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_IT, "Specifica il nome della ROM del generatore di caratteri"},
/* nl */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_NL, "Geef de naam van het CHARGEN-ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_SV, "Ange namn på teckengenerator-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_TR, "Karakter Oluþturucu ROM imajýnýn ismini belirt"},
#endif

#ifdef COMMON_KBD
/* c64/c64-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2,    N_("Specify index of keymap file (0=sym, 1=symDE, 2=pos)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_DA, "Angiv indeks for tastaturindstillingsfil (0=symbolsk, 1=symbolsk "
                                                  "tysk, 2=positionsbestemt)"},
/* de */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_DE, "Index für Keymap Datei festlegen (0=symbol, 1=symDE, 2=positional)"},
/* fr */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_FR, "Spécifier l'index du fichier keymap (0=sym, 1=symDE, 2=pos)"},
/* hu */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_HU, "Adja meg a billentyûzet leképzési fájl típusát (0=szimbolikus, "
                                                  "1=német szimbolikus, 2=pozíció szerinti)"},
/* it */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_IT, "Specifica l'indice del file della mappa della tastiera (0=sim, "
                                                  "1=simGER, 2=pos)"},
/* nl */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_NL, "Geef de index van het keymapbestand (0=sym, 1=symDE, 2=pos)"},
/* pl */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_SV, "Ange index för för tangentbordsinställningsfil (0=symbolisk, "
                                                  "1=symbolisk tysk, 2=positionsriktig)"},
/* tr */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_TR, "Tuþ haritasý dosyasýnýn indeksini belirt (0=sembol, 1=sembol Almanca, "
                                                  "2=konumsal)"},
#endif

/* c64/c64-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP,    N_("Specify name of symbolic German keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_DA, "Angiv fil for tysk symbolsk tastaturindstilling"},
/* de */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_DE, "Name von symbolischer Keymap Datei definieren"},
/* fr */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_FR, "Spécifier le nom du fichier symbolique de mappage clavier"},
/* hu */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_HU, "Adja meg a nevét a német billentyûzet leképzés fájlnak"},
/* it */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_IT, "Specifica il nome del file della mappa simbolica della tastiera tedesca"},
/* nl */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_NL, "Geef de naam van het symbolische Duitse keymapbestand"},
/* pl */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_SV, "Ange fil för tysk symbolisk tangentbordsemulering"},
/* tr */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_TR, "Sembolik Almanca tuþ haritasý dosyasýnýn ismini belirt"},
#endif
#endif

/* c64/georam.c */
/* en */ {IDCLS_ENABLE_GEORAM,    N_("Enable the GEORAM expansion unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_GEORAM_DA, "Aktivér GEORAM-udviddelsesenhed"},
/* de */ {IDCLS_ENABLE_GEORAM_DE, "GEORAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_GEORAM_FR, "Activer l'unité d'expansion GEORAM"},
/* hu */ {IDCLS_ENABLE_GEORAM_HU, "GEORAM Expansion Unit engedélyezése"},
/* it */ {IDCLS_ENABLE_GEORAM_IT, "Attiva l'espansione GEORAM"},
/* nl */ {IDCLS_ENABLE_GEORAM_NL, "Activeer de GEORAM-uitbreidingseenheid"},
/* pl */ {IDCLS_ENABLE_GEORAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_GEORAM_SV, "Aktivera GEORAM-expansionsenhet"},
/* tr */ {IDCLS_ENABLE_GEORAM_TR, "GEORAM geniþletme birimini aktif et"},
#endif

/* c64/georam.c */
/* en */ {IDCLS_DISABLE_GEORAM,    N_("Disable the GEORAM expansion unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_GEORAM_DA, "Deaktiver GEORAM-udviddelsesenhed"},
/* de */ {IDCLS_DISABLE_GEORAM_DE, "GEORAM Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_GEORAM_FR, "Désactiver l'unité d'expansion GEORAM"},
/* hu */ {IDCLS_DISABLE_GEORAM_HU, "GEORAM Expansion Unit tiltása"},
/* it */ {IDCLS_DISABLE_GEORAM_IT, "Disattiva l'espansione GEORAM"},
/* nl */ {IDCLS_DISABLE_GEORAM_NL, "De GEORAM-uitbreidingseenheid uitschakelen"},
/* pl */ {IDCLS_DISABLE_GEORAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_GEORAM_SV, "Inaktivera GEORAM-expansionsenhet"},
/* tr */ {IDCLS_DISABLE_GEORAM_TR, "GEORAM geniþletme birimini pasifleþtir"},
#endif

/* c64/georam.c */
/* en */ {IDCLS_SPECIFY_GEORAM_NAME,    N_("Specify name of GEORAM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_GEORAM_NAME_DA, "Angiv navn på GEORAM-image"},
/* de */ {IDCLS_SPECIFY_GEORAM_NAME_DE, "Dateiname für GEORAM Erweiterung"},
/* fr */ {IDCLS_SPECIFY_GEORAM_NAME_FR, "Spécifier le nom de l'image GEORAM"},
/* hu */ {IDCLS_SPECIFY_GEORAM_NAME_HU, "Adja meg a GEORAM képmás nevét"},
/* it */ {IDCLS_SPECIFY_GEORAM_NAME_IT, "Specifica il nome dell'immagine GEORAM"},
/* nl */ {IDCLS_SPECIFY_GEORAM_NAME_NL, "Geef de naam van het GEORAM-bestand"},
/* pl */ {IDCLS_SPECIFY_GEORAM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_GEORAM_NAME_SV, "Ange namn på GEORAM-avbildning"},
/* tr */ {IDCLS_SPECIFY_GEORAM_NAME_TR, "GEORAM imajýnýn ismini belirt"},
#endif

/* c64/georam.c, c64/ramcart.c, c64/reu.c, pet/petreu.c */
/* en */ {IDCLS_P_SIZE_IN_KB,    N_("<size in KB>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_SIZE_IN_KB_DA, "<størrelse i KB>"},
/* de */ {IDCLS_P_SIZE_IN_KB_DE, "<Grösse in KB>"},
/* fr */ {IDCLS_P_SIZE_IN_KB_FR, "<taille en KO>"},
/* hu */ {IDCLS_P_SIZE_IN_KB_HU, "<méret KB-ban>"},
/* it */ {IDCLS_P_SIZE_IN_KB_IT, "<dimensione in KB>"},
/* nl */ {IDCLS_P_SIZE_IN_KB_NL, "<grootte in KB>"},
/* pl */ {IDCLS_P_SIZE_IN_KB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_SIZE_IN_KB_SV, "<storlek i KB>"},
/* tr */ {IDCLS_P_SIZE_IN_KB_TR, "<KB cinsinden boyut>"},
#endif

/* c64/georam.c */
/* en */ {IDCLS_GEORAM_SIZE,    N_("Size of the GEORAM expansion unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_GEORAM_SIZE_DA, "Størrelse af GEORAM-udviddelsesenhed"},
/* de */ {IDCLS_GEORAM_SIZE_DE, "Grösse der GEORAM Erweiterung"},
/* fr */ {IDCLS_GEORAM_SIZE_FR, "Taille de l'unité d'expansion GEORAM"},
/* hu */ {IDCLS_GEORAM_SIZE_HU, "GEORAM Expansion Unit mérete"},
/* it */ {IDCLS_GEORAM_SIZE_IT, "Dimensione dell'espansione GEORAM"},
/* nl */ {IDCLS_GEORAM_SIZE_NL, "Grootte van de GEORAM-uitbreidingseenheid"},
/* pl */ {IDCLS_GEORAM_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_GEORAM_SIZE_SV, "Storlek på GEORAM-expansionsenhet"},
/* tr */ {IDCLS_GEORAM_SIZE_TR, "GEORAM geniþletme biriminin boyutu"},
#endif

/* pet/petreu.c */
/* en */ {IDCLS_ENABLE_PETREU,    N_("Enable the PET Ram and Expansion Unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_PETREU_DA, "Aktivér PET REU"},
/* de */ {IDCLS_ENABLE_PETREU_DE, "PET RAM Speicher und Erweiterungsmodul aktivieren"},
/* fr */ {IDCLS_ENABLE_PETREU_FR, "Activer l'expansion d'unité et de RAM du PET"},
/* hu */ {IDCLS_ENABLE_PETREU_HU, "PET RAM and Expansion Unit engedélyezése"},
/* it */ {IDCLS_ENABLE_PETREU_IT, "Attiva l'unità di espansione e della RAM del PET"},
/* nl */ {IDCLS_ENABLE_PETREU_NL, "Activeer de PET-Ram en uitbreidingseenheid"},
/* pl */ {IDCLS_ENABLE_PETREU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_PETREU_SV, "Aktivera PET Ram- och -expansionsenhet"},
/* tr */ {IDCLS_ENABLE_PETREU_TR, "PET Ram ve Geniþletme Birimi'ni aktif et"},
#endif

/* pet/petreu.c */
/* en */ {IDCLS_DISABLE_PETREU,    N_("Disable the PET Ram and Expansion Unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_PETREU_DA, "Deaktiver PET REU"},
/* de */ {IDCLS_DISABLE_PETREU_DE, "PET RAM Speicher und Erweiterungsmodul dektivieren"},
/* fr */ {IDCLS_DISABLE_PETREU_FR, "Désactiver l'expansion d'unité et de RAM du PET"},
/* hu */ {IDCLS_DISABLE_PETREU_HU, "PET RAM and Expansion Unit tiltása"},
/* it */ {IDCLS_DISABLE_PETREU_IT, "Disattiva l'unità di espansione e della RAM del PET"},
/* nl */ {IDCLS_DISABLE_PETREU_NL, "De PET-RAM en uitbreidingseenheid uitschakelen"},
/* pl */ {IDCLS_DISABLE_PETREU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_PETREU_SV, "Inaktivera PET Ram- och -expansionsenhet"},
/* tr */ {IDCLS_DISABLE_PETREU_TR, "PET Ram ve Geniþletme Birimi'ni pasifleþtir"},
#endif

/* pet/petreu.c */
/* en */ {IDCLS_SPECIFY_PETREU_NAME,    N_("Specify name of PET Ram and Expansion Unit image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PETREU_NAME_DA, "Angiv navn på PET REU-image"},
/* de */ {IDCLS_SPECIFY_PETREU_NAME_DE, "Namen für PET RAM Speicher und Erweiterungsmodul definieren"},
/* fr */ {IDCLS_SPECIFY_PETREU_NAME_FR, "Spécifier le nom de l'image d’unité d'expansion RAM"},
/* hu */ {IDCLS_SPECIFY_PETREU_NAME_HU, "Adja meg a PET RAM and Expansion Unit képmás nevét"},
/* it */ {IDCLS_SPECIFY_PETREU_NAME_IT, "Specifica il nome dell'immagine dell'unità di espansione e della RAM del PET"},
/* nl */ {IDCLS_SPECIFY_PETREU_NAME_NL, "Geef de naam van het PET-RAM en uitbreidingseenheid bestand"},
/* pl */ {IDCLS_SPECIFY_PETREU_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PETREU_NAME_SV, "Ange namn på PET Ram- och -expansionsenhet"},
/* tr */ {IDCLS_SPECIFY_PETREU_NAME_TR, "PET Ram ve Geniþletme Birimi'nin ismini belirt"},
#endif

/* pet/petreu.c */
/* en */ {IDCLS_PETREU_SIZE,    N_("Size of the PET Ram and Expansion Unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PETREU_SIZE_DA, "Størrelse på PET REU"},
/* de */ {IDCLS_PETREU_SIZE_DE, "Größe des PET RAM Speicher und Erweiterungsmodul"},
/* fr */ {IDCLS_PETREU_SIZE_FR, "Taille de l'unité d'expansion RAM"},
/* hu */ {IDCLS_PETREU_SIZE_HU, "PET RAM and Expansion Unit mérete"},
/* it */ {IDCLS_PETREU_SIZE_IT, "Dimensione dell'unità di espansione e della RAM del PET"},
/* nl */ {IDCLS_PETREU_SIZE_NL, "Grootte van de PET-RAM en uitbreidingseenheid"},
/* pl */ {IDCLS_PETREU_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PETREU_SIZE_SV, "Storlek på PET Ram- och -expansionsenhet"},
/* tr */ {IDCLS_PETREU_SIZE_TR, "PET Ram ve Geniþletme Birimi'nin boyutu"},
#endif

/* c64/psid.c */
/* en */ {IDCLS_SID_PLAYER_MODE,    N_("SID player mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SID_PLAYER_MODE_DA, "SID-afspiller tilstand"},
/* de */ {IDCLS_SID_PLAYER_MODE_DE, "SID player Modus"},
/* fr */ {IDCLS_SID_PLAYER_MODE_FR, "Mode jukebox SID"},
/* hu */ {IDCLS_SID_PLAYER_MODE_HU, "SID lejátszó mód"},
/* it */ {IDCLS_SID_PLAYER_MODE_IT, "Modalità del SID player"},
/* nl */ {IDCLS_SID_PLAYER_MODE_NL, "SID-playermodus"},
/* pl */ {IDCLS_SID_PLAYER_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SID_PLAYER_MODE_SV, "SID-spelarläge"},
/* tr */ {IDCLS_SID_PLAYER_MODE_TR, "SID çalar modu"},
#endif

/* c64/psid.c */
/* en */ {IDCLS_OVERWRITE_PSID_SETTINGS,    N_("Override PSID settings for Video standard and SID model")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_OVERWRITE_PSID_SETTINGS_DA, "Overstyr PSID-indstillinger for Video-standard og SID-model"},
/* de */ {IDCLS_OVERWRITE_PSID_SETTINGS_DE, "Überschreiben der PSID Einstellungen bezüglich Videostandard und SID Modell"},
/* fr */ {IDCLS_OVERWRITE_PSID_SETTINGS_FR, "Écraser les paramètres PSID pour le standard vidéo et le modèle SID"},
/* hu */ {IDCLS_OVERWRITE_PSID_SETTINGS_HU, "A videó szabvány és a SID modell PSID beállításainak felülbírálása"},
/* it */ {IDCLS_OVERWRITE_PSID_SETTINGS_IT, "Non tener conto delle impostazioni del PSID per lo standard video e il "
                                            "modello di SID"},
/* nl */ {IDCLS_OVERWRITE_PSID_SETTINGS_NL, "Overschrijf PSID-instellingen voor videostandaard en SID-model"},
/* pl */ {IDCLS_OVERWRITE_PSID_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_OVERWRITE_PSID_SETTINGS_SV, "Ersätt PSID-inställningar för videostandard och SID-modell"},
/* tr */ {IDCLS_OVERWRITE_PSID_SETTINGS_TR, "Video standardý ve SID modeli için PSID ayarlarýný geçersiz kýl"},
#endif

/* c64/psid.c */
/* en */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER,    N_("Specify PSID tune <number>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_DA, "Vælg PSID-nummer <nummer>"},
/* de */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_DE, "PSID Stück <Nummer> definieren"},
/* fr */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_FR, "Spécifier le <numero> de piste PSID"},
/* hu */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_HU, "Adja meg a PSID hangzatok <szám>"},
/* it */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_IT, "Specifica la melodia <numero> del PSID"},
/* nl */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_NL, "Geef PSID-deuntje <nummer>"},
/* pl */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_SV, "Ange PSID-låt <nummer>"},
/* tr */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_TR, "PSID parça <numara>'sýný belirt"},
#endif

/* c64/ramcart.c */
/* en */ {IDCLS_ENABLE_RAMCART,    N_("Enable the RAMCART expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAMCART_DA, "Aktivér RAMCART-udviddelse"},
/* de */ {IDCLS_ENABLE_RAMCART_DE, "RAMCART Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_RAMCART_FR, "Activer l'expansion RAMCART"},
/* hu */ {IDCLS_ENABLE_RAMCART_HU, "RAMCART bõvítés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAMCART_IT, "Attiva l'espansione RAMCART"},
/* nl */ {IDCLS_ENABLE_RAMCART_NL, "Activeer de RAMCART-uitbreiding"},
/* pl */ {IDCLS_ENABLE_RAMCART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAMCART_SV, "Aktivera RAMCART-expansion"},
/* tr */ {IDCLS_ENABLE_RAMCART_TR, "RAMCART geniþlemesini aktif et"},
#endif

/* c64/ramcart.c */
/* en */ {IDCLS_DISABLE_RAMCART,    N_("Disable the RAMCART expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAMCART_DA, "Deaktiver RAMCART-udviddelse"},
/* de */ {IDCLS_DISABLE_RAMCART_DE, "RAMCART Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAMCART_FR, "Désactiver l'expansion RAMCART"},
/* hu */ {IDCLS_DISABLE_RAMCART_HU, "RAMCART bõvítés tiltása"},
/* it */ {IDCLS_DISABLE_RAMCART_IT, "Disattiva l'espansione RAMCART"},
/* nl */ {IDCLS_DISABLE_RAMCART_NL, "De RAMCART-uitbreiding uitschakelen"},
/* pl */ {IDCLS_DISABLE_RAMCART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAMCART_SV, "Inaktivera RAMCART-expansion"},
/* tr */ {IDCLS_DISABLE_RAMCART_TR, "RAMCART geniþlemesini pasifleþtir"},
#endif

/* c64/ramcart.c */
/* en */ {IDCLS_SPECIFY_RAMCART_NAME,    N_("Specify name of RAMCART image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RAMCART_NAME_DA, "Angiv navn på RAMCART-image"},
/* de */ {IDCLS_SPECIFY_RAMCART_NAME_DE, "Dateiname für RAMCART Erweiterung"},
/* fr */ {IDCLS_SPECIFY_RAMCART_NAME_FR, "Spécifier le nom de l'image RAMCART"},
/* hu */ {IDCLS_SPECIFY_RAMCART_NAME_HU, "Adja meg a RAMCART képmás nevét"},
/* it */ {IDCLS_SPECIFY_RAMCART_NAME_IT, "Specifica il nome dell'immagine RAMCART"},
/* nl */ {IDCLS_SPECIFY_RAMCART_NAME_NL, "Geef de naam van het RAMCART-bestand"},
/* pl */ {IDCLS_SPECIFY_RAMCART_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RAMCART_NAME_SV, "Ange namn på RAMCART-avbildning"},
/* tr */ {IDCLS_SPECIFY_RAMCART_NAME_TR, "RAMCART imajýnýn ismini belirt"},
#endif

/* c64/ramcart.c */
/* en */ {IDCLS_RAMCART_SIZE,    N_("Size of the RAMCART expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_RAMCART_SIZE_DA, "Størrelse på RAMCART-udviddelse"},
/* de */ {IDCLS_RAMCART_SIZE_DE, "Grösse der RAMCART Erweiterung"},
/* fr */ {IDCLS_RAMCART_SIZE_FR, "Taille de l'expansion RAMCART"},
/* hu */ {IDCLS_RAMCART_SIZE_HU, "RAMCART bõvítés mérete"},
/* it */ {IDCLS_RAMCART_SIZE_IT, "Dimensione dell'espansione RAMCART"},
/* nl */ {IDCLS_RAMCART_SIZE_NL, "Grootte van de RAMCART-uitbreiding"},
/* pl */ {IDCLS_RAMCART_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RAMCART_SIZE_SV, "Storlek på RAMCART-expansion"},
/* tr */ {IDCLS_RAMCART_SIZE_TR, "RAMCART geniþlemesinin boyutu"},
#endif

/* c64/dqbb.c */
/* en */ {IDCLS_ENABLE_DQBB,    N_("Enable Double Quick Brown Box")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DQBB_DA, "Aktivér Double Quick Brown Box"},
/* de */ {IDCLS_ENABLE_DQBB_DE, "Double Quick Brown Box aktivieren"},
/* fr */ {IDCLS_ENABLE_DQBB_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_DQBB_HU, "Double Quick Brown Box engedélyezése"},
/* it */ {IDCLS_ENABLE_DQBB_IT, "Attiva Double Quick Brown Box"},
/* nl */ {IDCLS_ENABLE_DQBB_NL, "Activeer de Double Quick Brown Box cartridge"},
/* pl */ {IDCLS_ENABLE_DQBB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DQBB_SV, "Aktivera Double Quick Brown Box"},
/* tr */ {IDCLS_ENABLE_DQBB_TR, "Double Quick Brown Box'ý aktif et"},
#endif

/* c64/dqbb.c */
/* en */ {IDCLS_DISABLE_DQBB,    N_("Disable Double Quick Brown Box")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DQBB_DA, "Deaktiver Double Quick Brown Box"},
/* de */ {IDCLS_DISABLE_DQBB_DE, "Double Quick Brown Box Modul deaktivieren"},
/* fr */ {IDCLS_DISABLE_DQBB_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_DQBB_HU, "Double Quick Brown Box tiltása"},
/* it */ {IDCLS_DISABLE_DQBB_IT, "Disattiva Double Quick Brown Box"},
/* nl */ {IDCLS_DISABLE_DQBB_NL, "De Double Quick Brown Box cartridge uitschakelen"},
/* pl */ {IDCLS_DISABLE_DQBB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DQBB_SV, "Inaktivera Double Quick Brown Box"},
/* tr */ {IDCLS_DISABLE_DQBB_TR, "Double Quick Brown Box'ý pasifleþtir"},
#endif

/* c64/dqbb.c */
/* en */ {IDCLS_SPECIFY_DQBB_NAME,    N_("Specify Double Quick Brown Box filename")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_DQBB_NAME_DA, "Angiv filnavn for Double Quick Brown Box"},
/* de */ {IDCLS_SPECIFY_DQBB_NAME_DE, "Double Quick Brown Box Dateiname spezifizieren"},
/* fr */ {IDCLS_SPECIFY_DQBB_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_DQBB_NAME_HU, "Adja meg a Double Quick Brown Box fájlnevet"},
/* it */ {IDCLS_SPECIFY_DQBB_NAME_IT, "Specifica il nome del file Double Quick Brown Box"},
/* nl */ {IDCLS_SPECIFY_DQBB_NAME_NL, "Geef de naam van het Double Quick Brown Box cartridge-bestand"},
/* pl */ {IDCLS_SPECIFY_DQBB_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_DQBB_NAME_SV, "Ange filnamn för Double Quick Brown Box"},
/* tr */ {IDCLS_SPECIFY_DQBB_NAME_TR, "Double Quick Brown Box dosya ismi belirt"},
#endif

/* c64/isepic.c */
/* en */ {IDCLS_ENABLE_ISEPIC,    N_("Enable the isepic cart")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_ISEPIC_DA, "Aktivér isepic-cartridge"},
/* de */ {IDCLS_ENABLE_ISEPIC_DE, "Isepic Modul aktivieren"},
/* fr */ {IDCLS_ENABLE_ISEPIC_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_ISEPIC_HU, "Isepic cartridge engedélyezése"},
/* it */ {IDCLS_ENABLE_ISEPIC_IT, "Attiva la cartuccia isepic"},
/* nl */ {IDCLS_ENABLE_ISEPIC_NL, "Activeer de isepic cartridge"},
/* pl */ {IDCLS_ENABLE_ISEPIC_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_ISEPIC_SV, "Aktivera isepic-insticksmodul"},
/* tr */ {IDCLS_ENABLE_ISEPIC_TR, "Isepic kartuþunu aktif et"},
#endif

/* c64/isepic.c */
/* en */ {IDCLS_DISABLE_ISEPIC,    N_("Disable the isepic cart")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_ISEPIC_DA, "Deaktiver isepic-cartridge"},
/* de */ {IDCLS_DISABLE_ISEPIC_DE, "Isepic Modul deaktivieren"},
/* fr */ {IDCLS_DISABLE_ISEPIC_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_ISEPIC_HU, "Isepic cartridge tiltása"},
/* it */ {IDCLS_DISABLE_ISEPIC_IT, "Disattiva la cartuccia isepic"},
/* nl */ {IDCLS_DISABLE_ISEPIC_NL, "De isepic cartridge uitschakelen"},
/* pl */ {IDCLS_DISABLE_ISEPIC_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_ISEPIC_SV, "Inaktivera isepic-insticksmodul."},
/* tr */ {IDCLS_DISABLE_ISEPIC_TR, "Isepic kartuþunu pasifleþtir"},
#endif

/* c64/mmc64.c */
/* en */ {IDCLS_ENABLE_MMC64,    N_("Enable the MMC64 expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_MMC64_DA, "Aktivér MMC64-udviddelse"},
/* de */ {IDCLS_ENABLE_MMC64_DE, "MMC64 Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_MMC64_FR, "Activer l'expansion MMC64"},
/* hu */ {IDCLS_ENABLE_MMC64_HU, "MMC64 bõvítés engedélyezése"},
/* it */ {IDCLS_ENABLE_MMC64_IT, "Attiva l'espansione MMC64"},
/* nl */ {IDCLS_ENABLE_MMC64_NL, "Activeer de MMC64-uitbreiding"},
/* pl */ {IDCLS_ENABLE_MMC64_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_MMC64_SV, "Aktivera MMC64-expansion"},
/* tr */ {IDCLS_ENABLE_MMC64_TR, "MMC64 geniþlemesini aktif et"},
#endif

/* c64/mmc64.c */
/* en */ {IDCLS_DISABLE_MMC64,    N_("Disable the MMC64 expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_MMC64_DA, "Deaktiver MMC64-udviddelse"},
/* de */ {IDCLS_DISABLE_MMC64_DE, "MMC64 Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_MMC64_FR, "Désactiver l'expansion MMC64"},
/* hu */ {IDCLS_DISABLE_MMC64_HU, "MMC64 bõvítés tiltása"},
/* it */ {IDCLS_DISABLE_MMC64_IT, "Disattiva l'espansione MMC64"},
/* nl */ {IDCLS_DISABLE_MMC64_NL, "De MMC64-uitbreiding uitschakelen"},
/* pl */ {IDCLS_DISABLE_MMC64_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_MMC64_SV, "Inaktivera MMC64-expansion"},
/* tr */ {IDCLS_DISABLE_MMC64_TR, "MMC64 geniþlemesini pasifleþtir"},
#endif

/* c64/mmc64.c */
/* en */ {IDCLS_SPECIFY_MMC64_BIOS_NAME,    N_("Specify name of MMC64 BIOS image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_MMC64_BIOS_NAME_DA, "Angiv navn på MMC64-BIOS-image"},
/* de */ {IDCLS_SPECIFY_MMC64_BIOS_NAME_DE, "Name für MMC64 BIOS Datei spezifizieren"},
/* fr */ {IDCLS_SPECIFY_MMC64_BIOS_NAME_FR, "Spécifier le nom de l'image MMC64 BIOS"},
/* hu */ {IDCLS_SPECIFY_MMC64_BIOS_NAME_HU, "Adja meg az MMC64 BIOS képmás nevét"},
/* it */ {IDCLS_SPECIFY_MMC64_BIOS_NAME_IT, "Specifica il nome dell'immagine del BIOS MMC64"},
/* nl */ {IDCLS_SPECIFY_MMC64_BIOS_NAME_NL, "Geef de naam van het MMC64-BIOS-bestand"},
/* pl */ {IDCLS_SPECIFY_MMC64_BIOS_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_MMC64_BIOS_NAME_SV, "Ange namn på MMC64-BIOS-avbildning"},
/* tr */ {IDCLS_SPECIFY_MMC64_BIOS_NAME_TR, "MMC64 BIOS imajýnýn ismini belirt"},
#endif

/* c64/mmc64.c */
/* en */ {IDCLS_SPECIFY_MMC64_IMAGE_NAME,    N_("Specify name of MMC64 image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_MMC64_IMAGE_NAME_DA, "Angiv navn på MMC64-image"},
/* de */ {IDCLS_SPECIFY_MMC64_IMAGE_NAME_DE, "Name der MMC64 Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_MMC64_IMAGE_NAME_FR, "Spécifier le nom de l'image MMC64"},
/* hu */ {IDCLS_SPECIFY_MMC64_IMAGE_NAME_HU, "Adja meg az MMC64 képmás nevét"},
/* it */ {IDCLS_SPECIFY_MMC64_IMAGE_NAME_IT, "Specifica il nome dell'immagine MMC64"},
/* nl */ {IDCLS_SPECIFY_MMC64_IMAGE_NAME_NL, "Geef de naam van het MMC64-bestand"},
/* pl */ {IDCLS_SPECIFY_MMC64_IMAGE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_MMC64_IMAGE_NAME_SV, "Ange namn på MMC64-avbildning"},
/* tr */ {IDCLS_SPECIFY_MMC64_IMAGE_NAME_TR, "MMC64 imajýnýn ismini belirt"},
#endif

/* c64/mmc64.c */
/* en */ {IDCLS_MMC64_READONLY,    N_("Set the MMC64 card to read-only")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_MMC64_READONLY_DA, "Skrivebeskyt MMC64-kort"},
/* de */ {IDCLS_MMC64_READONLY_DE, "MMC64 Karte Schreibschutz"},
/* fr */ {IDCLS_MMC64_READONLY_FR, "Définir la carte MMC64 en lecture seule"},
/* hu */ {IDCLS_MMC64_READONLY_HU, "MMC64 kártya csak olvasható"},
/* it */ {IDCLS_MMC64_READONLY_IT, "Imposta la cartuccia MMC64 in sola lettura"},
/* nl */ {IDCLS_MMC64_READONLY_NL, "Zet de MMC64 card als alleen lezen"},
/* pl */ {IDCLS_MMC64_READONLY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_MMC64_READONLY_SV, "Skrivskydda MMC64-kort"},
/* tr */ {IDCLS_MMC64_READONLY_TR, "MMC64 kartýný salt okunura çevir"},
#endif

/* c64/mmc64.c */
/* en */ {IDCLS_MMC64_READWRITE,    N_("Set the MMC64 card to read/write")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_MMC64_READWRITE_DA, "Tillad skrivning på MMC64-kort"},
/* de */ {IDCLS_MMC64_READWRITE_DE, "MMC64 Karte Schreibzugriff"},
/* fr */ {IDCLS_MMC64_READWRITE_FR, "Définir la carte MMC64 en lecture/écriture"},
/* hu */ {IDCLS_MMC64_READWRITE_HU, "MMC64 kártya írható/olvasható"},
/* it */ {IDCLS_MMC64_READWRITE_IT, "Imposta la cartuccia MMC64 in lettura/scrittura"},
/* nl */ {IDCLS_MMC64_READWRITE_NL, "Zet de MMC64 card als lezen/schrijven"},
/* pl */ {IDCLS_MMC64_READWRITE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_MMC64_READWRITE_SV, "Tillåt skrivning på MMC64-kort"},
/* tr */ {IDCLS_MMC64_READWRITE_TR, "MMC64 kartýný okunur/yazýlýra çevir"},
#endif

/* c64/mmc64.c */
/* en */ {IDCLS_MMC64_BIOS_WRITE,    N_("Save the MMC64 bios when changed")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_MMC64_BIOS_WRITE_DA, "Gem MMC64-BIOS ved ændring"},
/* de */ {IDCLS_MMC64_BIOS_WRITE_DE, "MMC64 BIOS speichern bei Änderung"},
/* fr */ {IDCLS_MMC64_BIOS_WRITE_FR, "Sauvegarder le BIOS MMC64 lorsque modifié"},
/* hu */ {IDCLS_MMC64_BIOS_WRITE_HU, "MMC64 BIOS mentése változás után"},
/* it */ {IDCLS_MMC64_BIOS_WRITE_IT, "Salva il bios MMC64 al cambio"},
/* nl */ {IDCLS_MMC64_BIOS_WRITE_NL, "Sla de MMC64-bios op als er veranderingen zijn gemaakt"},
/* pl */ {IDCLS_MMC64_BIOS_WRITE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_MMC64_BIOS_WRITE_SV, "Spara MMC64-BIOS vid ändring"},
/* tr */ {IDCLS_MMC64_BIOS_WRITE_TR, "MMC64 BIOS'unu deðiþtiðinde kaydet"},
#endif

/* c64/reu.c */
/* en */ {IDCLS_ENABLE_REU,    N_("Enable the RAM expansion unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_REU_DA, "Aktivér RAM-udviddelsesenhed"},
/* de */ {IDCLS_ENABLE_REU_DE, "REU Speichererweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_REU_FR, "Activer l'unité d'expansion RAM"},
/* hu */ {IDCLS_ENABLE_REU_HU, "RAM Expansion Unit engedélyezése"},
/* it */ {IDCLS_ENABLE_REU_IT, "Attiva l'espansione di RAM"},
/* nl */ {IDCLS_ENABLE_REU_NL, "Activeer de RAM-uitbreidingseenheid (REU)"},
/* pl */ {IDCLS_ENABLE_REU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_REU_SV, "Aktivera RAM-expansionsenhet"},
/* tr */ {IDCLS_ENABLE_REU_TR, "RAM geniþletme birimini aktif et"},
#endif

/* c64/reu.c */
/* en */ {IDCLS_DISABLE_REU,    N_("Disable the RAM expansion unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_REU_DA, "Deaktiver RAM-udviddelsesenhed"},
/* de */ {IDCLS_DISABLE_REU_DE, "REU Speichererweiterung deaktivieren"}, 
/* fr */ {IDCLS_DISABLE_REU_FR, "Désactiver l'unité d'expansion RAM"},
/* hu */ {IDCLS_DISABLE_REU_HU, "RAM Expansion Unit tiltása"},
/* it */ {IDCLS_DISABLE_REU_IT, "Disattiva l'espansione di RAM"},
/* nl */ {IDCLS_DISABLE_REU_NL, "De RAM-uitbreidingseenheid (REU) uitschakelen"},
/* pl */ {IDCLS_DISABLE_REU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_REU_SV, "Inaktivera RAM-expansionsenhet"},
/* tr */ {IDCLS_DISABLE_REU_TR, "RAM geniþletme birimini pasifleþtir"},
#endif

/* c64/reu.c */
/* en */ {IDCLS_SPECIFY_REU_NAME,    N_("Specify name of REU image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_REU_NAME_DA, "Angiv navn på REU-image"},
/* de */ {IDCLS_SPECIFY_REU_NAME_DE, "Dateiname für REU Speichererweiterung"},
/* fr */ {IDCLS_SPECIFY_REU_NAME_FR, "Spécifier le nom de l'image REU"},
/* hu */ {IDCLS_SPECIFY_REU_NAME_HU, "Adja meg a REU képmás nevét"},
/* it */ {IDCLS_SPECIFY_REU_NAME_IT, "Specifica il nome dell'immagine REU"},
/* nl */ {IDCLS_SPECIFY_REU_NAME_NL, "Geef de naam van het REU-bestand"},
/* pl */ {IDCLS_SPECIFY_REU_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_REU_NAME_SV, "Ange namn på REU-avbildning"},
/* tr */ {IDCLS_SPECIFY_REU_NAME_TR, "REU imajýnýn ismini belirt"},
#endif

/* c64/reu.c */
/* en */ {IDCLS_REU_SIZE,    N_("Size of the RAM expansion unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_REU_SIZE_DA, "Størrelse på RAM-udviddelsesenhed"},
/* de */ {IDCLS_REU_SIZE_DE, "Grösse der REU Speichererweiterung"},
/* fr */ {IDCLS_REU_SIZE_FR, "Taille de l'unité d'expansion RAM"},
/* hu */ {IDCLS_REU_SIZE_HU, "RAM Expansion Unit mérete"},
/* it */ {IDCLS_REU_SIZE_IT, "Dimensione dell'espansione di RAM"},
/* nl */ {IDCLS_REU_SIZE_NL, "Grootte van de RAM-uitbreidingseenheid (REU)"},
/* pl */ {IDCLS_REU_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_REU_SIZE_SV, "Storlek på RAM-expansionsenhet"},
/* tr */ {IDCLS_REU_SIZE_TR, "RAM geniþletme biriminin boyutu"},
#endif

#ifdef HAVE_TFE
/* c64/tfe.c */
/* en */ {IDCLS_ENABLE_TFE,    N_("Enable the TFE (\"The Final Ethernet\") unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_TFE_DA, "Aktivér TFE-enhed (\"The Final Ethernet\")"},
/* de */ {IDCLS_ENABLE_TFE_DE, "TFE Ethernetemulation aktivieren"},
/* fr */ {IDCLS_ENABLE_TFE_FR, "Activer l'unité TFE (\"The Final Ethernet\")"},
/* hu */ {IDCLS_ENABLE_TFE_HU, "TFE (\"the final ethernet\") egység engedélyezése"},
/* it */ {IDCLS_ENABLE_TFE_IT, "Attiva l'unità TFE (\"The Final Ethernet\")"},
/* nl */ {IDCLS_ENABLE_TFE_NL, "Activeer de TFE (\"The Final Ethernet\")-eenheid"},
/* pl */ {IDCLS_ENABLE_TFE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TFE_SV, "Aktivera TFE-enhet (\"The Final Ethernet\")"},
/* tr */ {IDCLS_ENABLE_TFE_TR, "TFE (\"The Final Ethernet\") birimini aktif et"},
#endif

/* c64/tfe.c */
/* en */ {IDCLS_DISABLE_TFE,    N_("Disable the TFE (\"The Final Ethernet\") unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_TFE_DA, "Deaktiver TFE-enhed (\"The Final Ethernet\")"},
/* de */ {IDCLS_DISABLE_TFE_DE, "TFE Ethernetemulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_TFE_FR, "Désactiver l'unité TFE"},
/* hu */ {IDCLS_DISABLE_TFE_HU, "TFE (\"The Final Ethernet\") egység tiltása"},
/* it */ {IDCLS_DISABLE_TFE_IT, "Disattiva l'unità TFE (\"The Final Ethernet\")"},
/* nl */ {IDCLS_DISABLE_TFE_NL, "De TFE (\"The Final Ethernet\")-eenheid uitschakelen"},
/* pl */ {IDCLS_DISABLE_TFE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_TFE_SV, "Inaktivera TFE-enhet (\"The Final Ethernet\")"},
/* tr */ {IDCLS_DISABLE_TFE_TR, "TFE (\"The Final Ethernet\") birimini pasifleþtir"},
#endif

/* c64/tfe.c */
/* en */ {IDCLS_TFE_INTERFACE,    N_("Set the system ethernet interface for TFE emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TFE_INTERFACE_DA, "Indstil systemets netkort til TFE-emulering"},
/* de */ {IDCLS_TFE_INTERFACE_DE, "Setze die System Ethernet Schnittstelle für die TFE Emulation"},
/* fr */ {IDCLS_TFE_INTERFACE_FR, "Définir l’interface ethernet pour l’émulation TFE"},
/* hu */ {IDCLS_TFE_INTERFACE_HU, "Rendszer ethernet eszköz beállítása TFE emulációhoz"},
/* it */ {IDCLS_TFE_INTERFACE_IT, "Imposta l'interfaccia ethernet per l'emulazione TFE"},
/* nl */ {IDCLS_TFE_INTERFACE_NL, "Zet de systeem ethernetinterface voor de TFE-emulatie"},
/* pl */ {IDCLS_TFE_INTERFACE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TFE_INTERFACE_SV, "Ställ in systemets ethernetgränssnitt för TFE-emulering"},
/* tr */ {IDCLS_TFE_INTERFACE_TR, "Sistem ethernet arabirimini TFE emülasyonu için ayarla"},
#endif

/* c64/tfe.c */
/* en */ {IDCLS_ENABLE_TFE_AS_RRNET,    N_("Enable RRNet mode of TFE emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_TFE_AS_RRNET_DA, "Aktivér RRNet-tilstand af TFE-emulering"},
/* de */ {IDCLS_ENABLE_TFE_AS_RRNET_DE, "RRNet Modus der TFE Emulation aktivieren"},
/* fr */ {IDCLS_ENABLE_TFE_AS_RRNET_FR, "Activer le mode RRNet de l’émulation TFE"},
/* hu */ {IDCLS_ENABLE_TFE_AS_RRNET_HU, "RRNet mód engedélyezése TFE emulációhoz"},
/* it */ {IDCLS_ENABLE_TFE_AS_RRNET_IT, "Attiva la modalità RRNet dell'emulazione TFE"},
/* nl */ {IDCLS_ENABLE_TFE_AS_RRNET_NL, "Activeer RRNet-modus van de TFE-emulatie"},
/* pl */ {IDCLS_ENABLE_TFE_AS_RRNET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TFE_AS_RRNET_SV, "Aktivera RRNet-läge på TFE-emulering"},
/* tr */ {IDCLS_ENABLE_TFE_AS_RRNET_TR, "TFE emülasyonunun RRNet modunu aktif et"},
#endif

/* c64/tfe.c */
/* en */ {IDCLS_DISABLE_TFE_AS_RRNET,    N_("Disable RRNet mode of TFE emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_TFE_AS_RRNET_DA, "Deaktiver RRNet-läge af TFE-emulering"},
/* de */ {IDCLS_DISABLE_TFE_AS_RRNET_DE, "RRNet Modus der TFE Emulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_TFE_AS_RRNET_FR, "Désactiver le mode RRNet de l’émulation TFE"},
/* hu */ {IDCLS_DISABLE_TFE_AS_RRNET_HU, "RRNet mód tiltása TFE emulációhoz"},
/* it */ {IDCLS_DISABLE_TFE_AS_RRNET_IT, "Disattiva la modalità RRNet dell'emulazione TFE"},
/* nl */ {IDCLS_DISABLE_TFE_AS_RRNET_NL, "RRNet-modus van de TFE-emulatie uitschakelen"},
/* pl */ {IDCLS_DISABLE_TFE_AS_RRNET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_TFE_AS_RRNET_SV, "Inaktivera RRNet-läge på TFE-emulering"},
/* tr */ {IDCLS_DISABLE_TFE_AS_RRNET_TR, "TFE emülasyonunun RRNet modunu pasifleþtir"},
#endif
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_CART_ATTACH_DETACH_RESET,    N_("Reset machine if a cartridge is attached or detached")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_CART_ATTACH_DETACH_RESET_DA, "Reset maskinen hvis et cartridge tilsluttes eller frakobles"},
/* de */ {IDCLS_CART_ATTACH_DETACH_RESET_DE, "Reset der Maschine wenn ein Erweiterungsmodul eingelegt oder entfernt wird"},
/* fr */ {IDCLS_CART_ATTACH_DETACH_RESET_FR, "Réinitialiser si une cartouche est insérée ou retirée"},
/* hu */ {IDCLS_CART_ATTACH_DETACH_RESET_HU, "Reset a cartridge csatolásakorgy leválasztásakor"},
/* it */ {IDCLS_CART_ATTACH_DETACH_RESET_IT, "Fare il reset della macchina se una cartuccia è collegata o rimossa"},
/* nl */ {IDCLS_CART_ATTACH_DETACH_RESET_NL, "Reset machine als een cartridge wordt aangekoppeld of ontkoppeld"},
/* pl */ {IDCLS_CART_ATTACH_DETACH_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_CART_ATTACH_DETACH_RESET_SV, "Nollställ maskinen om en insticksmodul kopplas till eller från"},
/* tr */ {IDCLS_CART_ATTACH_DETACH_RESET_TR, "Bir kartuþ takýlýr ya da çýkartýlýrsa makineyi resetle"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_CART_ATTACH_DETACH_NO_RESET,    N_("Do not reset machine if a cartridge is attached or detached")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_DA, "Reset ikke maskinen hvis et cartridge tilsluttes eller frakobles"},
/* de */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_DE, "Kein Reset der Maschine wenn ein Erweiterungsmodul eingelegt oder "
                                                "entfernt wird"},
/* fr */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_FR, "Ne pas réinitialiser si une cartouche est insérée ou retirée"},
/* hu */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_HU, "Nincs reset a cartridge csatolásakorgy leválasztásakor"},
/* it */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_IT, "Non fare il reset della macchina se una cartuccia è collegata o rimossa"},
/* nl */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_NL, "Reset machine niet als een cartridge wordt aangekoppeld of ontkoppeld"},
/* pl */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_SV, "Nollställ inte maskinen om en insticksmodul kopplas till eller från"},
/* tr */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_TR, "Bir kartuþ takýlýr ya da çýkartýlýrsa makineyi resetleme"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_CRT_CART,    N_("Attach CRT cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_CRT_CART_DA, "Tilslut CRT-cartridge image"},
/* de */ {IDCLS_ATTACH_CRT_CART_DE, "CRT Erweiterungsmodul Image einlegen"},
/* fr */ {IDCLS_ATTACH_CRT_CART_FR, "Insérer une cartouche CRT"},
/* hu */ {IDCLS_ATTACH_CRT_CART_HU, "CRT cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_CRT_CART_IT, "Seleziona l'immagine di una cartuccia CRT"},
/* nl */ {IDCLS_ATTACH_CRT_CART_NL, "Koppel CRT-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_CRT_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_CRT_CART_SV, "Anslut CRT-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_CRT_CART_TR, "CRT kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_GENERIC_8KB_CART,    N_("Attach generic 8KB cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_GENERIC_8KB_CART_DA, "Tilslut standard 8KB-cartridge image"},
/* de */ {IDCLS_ATTACH_GENERIC_8KB_CART_DE, "Generisches 8K Erweiterungsmodul Image einlegen"},
/* fr */ {IDCLS_ATTACH_GENERIC_8KB_CART_FR, "Insérer une cartouche générique 8KO"},
/* hu */ {IDCLS_ATTACH_GENERIC_8KB_CART_HU, "Általános 8KB cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_GENERIC_8KB_CART_IT, "Seleziona l'immagine di una cartuccia generica di 8KB"},
/* nl */ {IDCLS_ATTACH_GENERIC_8KB_CART_NL, "Koppel generiek 8KB-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_GENERIC_8KB_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_GENERIC_8KB_CART_SV, "Anslut vanlig 8KB-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_GENERIC_8KB_CART_TR, "8KB jenerik kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_GENERIC_16KB_CART,    N_("Attach generic 16KB cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_GENERIC_16KB_CART_DA, "Tilslut standard 16KB-cartridge image"},
/* de */ {IDCLS_ATTACH_GENERIC_16KB_CART_DE, "Generisches 16K Erweiterungsmodul Image einlegen"},
/* fr */ {IDCLS_ATTACH_GENERIC_16KB_CART_FR, "Insérer une cartouche générique 16KO"},
/* hu */ {IDCLS_ATTACH_GENERIC_16KB_CART_HU, "Általános 16KB cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_GENERIC_16KB_CART_IT, "Seleziona l'immagine di una cartuccia generica di 16KB"},
/* nl */ {IDCLS_ATTACH_GENERIC_16KB_CART_NL, "Koppel generiek 16KB-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_GENERIC_16KB_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_GENERIC_16KB_CART_SV, "Anslut vanlig 16KB-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_GENERIC_16KB_CART_TR, "16KB jenerik kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_STARDOS_CART,    N_("Attach raw 16KB StarDOS cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_RAW_STARDOS_CART_DA, "Tilslut råt 16KB StarDOS-cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_STARDOS_CART_DE, "16KB StarDOS Erweiterungsmodul Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_STARDOS_CART_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ATTACH_RAW_STARDOS_CART_HU, "Nyers 16KB-os StarDOS cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_RAW_STARDOS_CART_IT, "Seleziona l'immagine di una cartuccia StarDOS di 16KB"},
/* nl */ {IDCLS_ATTACH_RAW_STARDOS_CART_NL, "Koppel binair 16KB StarDOS-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_RAW_STARDOS_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_STARDOS_CART_SV, "Anslut rå 16KB StarDOS-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_RAW_STARDOS_CART_TR, "Düz 16KB StarDOS kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART,    N_("Attach raw 32KB Action Replay IV cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART_DA, "Tilslut råt 32KB-Action Replay IV-cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART_DE, "32K Action Replay Image IV einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART_HU, "32KB-os Action Replay IV cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART_IT, "Seleziona l'immagine di una cartuccia Action Replay IV di 32KB"},
/* nl */ {IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART_NL, "Koppel binair 32KB Action-Replay-IV-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART_SV, "Anslut rå 32KB-Action Replay IV-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART_TR, "Düz 32KB Action Replay IV kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART,    N_("Attach raw 16KB Action Replay III cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART_DA, "Tilslut råt 16KB-Action Replay III-cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART_DE, "16KB Action Replay Image III (raw) einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART_HU, "16KB-os Action Replay III cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART_IT, "Seleziona l'immagine di una cartuccia Action Replay III di 16KB"},
/* nl */ {IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART_NL, "Koppel binair 16KB Action-Replay-III-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART_SV, "Anslut rå 16KB-Action Replay III-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART_TR, "Düz 16KB Action Replay III kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART,    N_("Attach raw 32KB Action Replay cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_DA, "Tilslut råt 32KB-Action Replay-cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_DE, "32K Action Replay Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_FR, "Insérer une cartouche Action Replay 32KO"},
/* hu */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_HU, "32KB-os Action Replay cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_IT, "Seleziona l'immagine di una cartuccia Action Replay di 32KB"},
/* nl */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_NL, "Koppel binair 32KB Action-Replay-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_SV, "Anslut rå 32KB-Action Replay-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_TR, "Düz 32KB Action Replay kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART,    N_("Attach raw 64KB Retro Replay cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_DA, "Tilslut råt 64KB-Retro Replay-cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_DE, "64K Retro Replay Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_FR, "Insérer une cartouche Retro Replay 64KO"},
/* hu */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_HU, "64KB-os Retro Replay cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_IT, "Seleziona l'immagine di una cartuccia Retro Replay di 64KB"},
/* nl */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_NL, "Koppel binair 64KB Retro-Replay-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_SV, "Anslut rå 64KB-Retro Replay-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_TR, "Düz 64KB Retro Replay kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_IDE64_CART,    N_("Attach raw 64KB IDE64 cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_RAW_IDE64_CART_DA, "Tilslut råt 64KB IDE64-cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_IDE64_CART_DE, "64KB IDE64 Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_IDE64_CART_FR, "Insérer une cartouche IDE64 64KO"},
/* hu */ {IDCLS_ATTACH_RAW_IDE64_CART_HU, "64KB-os IDE64 cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_RAW_IDE64_CART_IT, "Seleziona l'immagine di una cartuccia IDE64 di 64KB"},
/* nl */ {IDCLS_ATTACH_RAW_IDE64_CART_NL, "Koppel binair 64KB IDE64-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_RAW_IDE64_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_IDE64_CART_SV, "Anslut rå 64KB IDE64-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_RAW_IDE64_CART_TR, "Düz 64KB IDE64 kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART,    N_("Attach raw 32KB Atomic Power cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_DA, "Tilslut råt 32KB Atomic Power-cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_DE, "32KB Atomic Power Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_FR, "Insérer une cartouche Atomic Power 32KO"},
/* hu */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_HU, "32KB-os Atomic Power cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_IT, "Seleziona l'immagine di una cartuccia Atomic Power di 32KB"},
/* nl */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_NL, "Koppel binair 32KB Atomic-Power-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_SV, "Anslut rå 32KB Atomic Power-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_TR, "Düz 32KB Atomic Power kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART,    N_("Attach raw 8KB Epyx fastload cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_DA, "Tilslut råt 8KB Epyx fastload-cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_DE, "8KB Epyx Fastload Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_FR, "Insérer une cartouche Epyx FastLoad 8KO"},
/* hu */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_HU, "8KB-os Epyx gyorstöltõ cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_IT, "Seleziona l'immagine di una cartuccia Epyx fastload di 8KB"},
/* nl */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_NL, "Koppel binair 8KB Epyx-fastload-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_SV, "Anslut rå 8KB Epyx fastload-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_TR, "Düz 8KB Epyx fastload kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_SS4_CART,    N_("Attach raw 32KB Super Snapshot cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_RAW_SS4_CART_DA, "Tilslut råt 32KB Super Snapshot-cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_SS4_CART_DE, "32KB Super Snapshot (raw) Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_SS4_CART_FR, "Insérer une cartouche Super Snapshot 32KO"},
/* hu */ {IDCLS_ATTACH_RAW_SS4_CART_HU, "32KB-os Super Snapshot cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_RAW_SS4_CART_IT, "Seleziona l'immagine di una cartuccia Super Snapshot di 32KB"},
/* nl */ {IDCLS_ATTACH_RAW_SS4_CART_NL, "Koppel binair 32KB Super-Snapshot-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_RAW_SS4_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_SS4_CART_SV, "Anslut rå 32KB Super Snapshot-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_RAW_SS4_CART_TR, "Düz 32KB Super Snapshot kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_SS5_CART,    N_("Attach raw 64KB Super Snapshot cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_RAW_SS5_CART_DA, "Tilslut råt 64KB-Super Snapshot-cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_SS5_CART_DE, "64KB Super Snapshot (raw) Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_SS5_CART_FR, "Insérer une cartouche Super Snapshot 64KO"},
/* hu */ {IDCLS_ATTACH_RAW_SS5_CART_HU, "64KB-os Super Snapshot cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_RAW_SS5_CART_IT, "Seleziona l'immagine di una cartuccia Super Snapshot di 64KB"},
/* nl */ {IDCLS_ATTACH_RAW_SS5_CART_NL, "Koppel binair 64KB Super-Snapshot-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_RAW_SS5_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_SS5_CART_SV, "Anslut rå 64KB-Super Snapshot-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_RAW_SS5_CART_TR, "Düz 64KB Super Snapshot kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_CBM_IEEE488_CART,    N_("Attach CBM IEEE488 cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_CBM_IEEE488_CART_DA, "Tilslut CBM IEEE488-cartridge image"},
/* de */ {IDCLS_ATTACH_CBM_IEEE488_CART_DE, "CBM IEEE488 Erweiterungsmodul einlegen"},
/* fr */ {IDCLS_ATTACH_CBM_IEEE488_CART_FR, "Insérer une cartouche CBM IEEE488"},
/* hu */ {IDCLS_ATTACH_CBM_IEEE488_CART_HU, "CBM IEEE488 cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_CBM_IEEE488_CART_IT, "Seleziona l'immagine di una cartuccia CBM IEE488"},
/* nl */ {IDCLS_ATTACH_CBM_IEEE488_CART_NL, "Koppel CBM IEEE488-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_CBM_IEEE488_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_CBM_IEEE488_CART_SV, "Anslut CBM IEEE488-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_CBM_IEEE488_CART_TR, "CBM IEEE488 kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_WESTERMANN_CART,    N_("Attach raw 16KB Westermann learning cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_DA, "Tilslut råt 16KB-Westermann learning-cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_DE, "16KB Westermann learning (raw) Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_FR, "Insérer une cartouche Westermann Learning 16KO"},
/* hu */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_HU, "16KB-os Westermann learning cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_IT, "Seleziona l'immagine di una cartuccia Westermann learning di 16KB"},
/* nl */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_NL, "Koppel binair 16KB Westermann-learning-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_SV, "Anslut rå 16KB-Westermann learning-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_TR, "Düz 16KB Westermann learning kartuþ imajý yerleþtir"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ENABLE_EXPERT_CART,    N_("Enable expert cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_EXPERT_CART_DA, "Aktivér Expert Cartridge"},
/* de */ {IDCLS_ENABLE_EXPERT_CART_DE, "Expert Cartridge aktivieren"},
/* fr */ {IDCLS_ENABLE_EXPERT_CART_FR, "Activer les paramètres experts de cartouches"},
/* hu */ {IDCLS_ENABLE_EXPERT_CART_HU, "Expert cartridge engedélyezése"},
/* it */ {IDCLS_ENABLE_EXPERT_CART_IT, "Attiva l'Expert Cartridge"},
/* nl */ {IDCLS_ENABLE_EXPERT_CART_NL, "Activeer expertcartridge"},
/* pl */ {IDCLS_ENABLE_EXPERT_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EXPERT_CART_SV, "Aktivera Expert Cartridge"},
/* tr */ {IDCLS_ENABLE_EXPERT_CART_TR, "Expert kartuþunu aktif et"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_DISABLE_CART,    N_("Disable default cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_CART_DA, "Deaktiver standard cartridge"},
/* de */ {IDCLS_DISABLE_CART_DE, "Standard Modul deaktivieren"},
/* fr */ {IDCLS_DISABLE_CART_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_CART_HU, "Alap cartridge tiltása"},
/* it */ {IDCLS_DISABLE_CART_IT, "Disattiva la cartuccia predefinita"},
/* nl */ {IDCLS_DISABLE_CART_NL, "De standaard cartridge uitschakelen"},
/* pl */ {IDCLS_DISABLE_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_CART_SV, "Inaktivera standardinsticksmodul"},
/* tr */ {IDCLS_DISABLE_CART_TR, "Varsayýlan kartuþu pasifleþtir"},
#endif

/* c64/cart/ide64.c */
/* en */ {IDCLS_SPECIFY_IDE64_NAME,    N_("Specify name of IDE64 image file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_IDE64_NAME_DA, "Angiv navn på IDE64-cartridge image"},
/* de */ {IDCLS_SPECIFY_IDE64_NAME_DE, "Namen für IDE64 Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_IDE64_NAME_FR, "Spécifier le nom de l'image IDE64"},
/* hu */ {IDCLS_SPECIFY_IDE64_NAME_HU, "Adja meg az IDE64 képmás fájlnevét"},
/* it */ {IDCLS_SPECIFY_IDE64_NAME_IT, "Specifica il nome del file immagine dell'IDE64"},
/* nl */ {IDCLS_SPECIFY_IDE64_NAME_NL, "Geef de naam van het IDE64-bestand"},
/* pl */ {IDCLS_SPECIFY_IDE64_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_IDE64_NAME_SV, "Ange namn på IDE64-insticksmodulfil"},
/* tr */ {IDCLS_SPECIFY_IDE64_NAME_TR, "IDE64 imaj dosyasýnýn ismini belirt"},
#endif

/* c64/cart/ide64.c, gfxoutputdrv/ffmpegdrv.c, datasette.c,
   debug.c, mouse.c, ram.c, sound.c, vsync.c */
/* en */ {IDCLS_P_VALUE,    N_("<value>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_VALUE_DA, "<værdi>"},
/* de */ {IDCLS_P_VALUE_DE, "<Wert>"},
/* fr */ {IDCLS_P_VALUE_FR, "<valeur>"},
/* hu */ {IDCLS_P_VALUE_HU, "<érték>"},
/* it */ {IDCLS_P_VALUE_IT, "<valore>"},
/* nl */ {IDCLS_P_VALUE_NL, "<waarde>"},
/* pl */ {IDCLS_P_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_VALUE_SV, "<värde>"},
/* tr */ {IDCLS_P_VALUE_TR, "<deðer>"},
#endif

/* c64/cart/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64,    N_("Set number of cylinders for the IDE64 emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_DA, "Vælg antal cylindrer for IDE64-emuleringen"},
/* de */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_DE, "Anzahl der Zylinder für IDE64 Emulation"},
/* fr */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_FR, "Régler le nombre de cylindres pour l'émulation IDE64"},
/* hu */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_HU, "Adja meg a sávok számát IDE64 emulációhoz"},
/* it */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_IT, "Imposta il numero di cilindri dell'emulazione IDE64"},
/* nl */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_NL, "Stel het aantal cylinders voor de IDE64-emulatie in"},
/* pl */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_SV, "Välj antal cylindrar för IDE64-emuleringen"},
/* tr */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_TR, "IDE64 emülasyonu için silindir adedini girin"},
#endif

/* c64/cart/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_HEADS_IDE64,    N_("Set number of heads for the IDE64 emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_AMOUNT_HEADS_IDE64_DA, "Vælg antal hoveder for IDE64-emuleringen"},
/* de */ {IDCLS_SET_AMOUNT_HEADS_IDE64_DE, "Anzahl der Laufwerksköpfe für IDE64 Emulation"},
/* fr */ {IDCLS_SET_AMOUNT_HEADS_IDE64_FR, "Régler le nombre de têtes pour l'émulation IDE64"},
/* hu */ {IDCLS_SET_AMOUNT_HEADS_IDE64_HU, "Adja meg a fejek számát IDE64 emulációhoz"},
/* it */ {IDCLS_SET_AMOUNT_HEADS_IDE64_IT, "Imposta il numero di testine dell'emulazione IDE64"},
/* nl */ {IDCLS_SET_AMOUNT_HEADS_IDE64_NL, "Stel het aantal koppen voor de IDE64-emulatie in"},
/* pl */ {IDCLS_SET_AMOUNT_HEADS_IDE64_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_AMOUNT_HEADS_IDE64_SV, "Ange antal huvuden för IDE64-emuleringen"},
/* tr */ {IDCLS_SET_AMOUNT_HEADS_IDE64_TR, "IDE64 emülasyonu için kafa adedini girin"},
#endif

/* c64/cart/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_SECTORS_IDE64,    N_("Set number of sectors for the IDE64 emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_DA, "Vælg antal sektorer for IDE64-emuleringen"},
/* de */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_DE, "Anzahl der Sektoren für IDE64 Emulation"},
/* fr */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_FR, "Régler le nombre de secteurs pour l'émulation IDE64"},
/* hu */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_HU, "Adja meg a szektorok számát IDE64 emulációhoz"},
/* it */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_IT, "Imposta il numero di settori dell'emulazione IDE64"},
/* nl */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_NL, "Stel het aantal sectors voor de IDE64-emulatie in"},
/* pl */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_SV, "Ange antal sektorer för IDE64-emuleringen"},
/* tr */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_TR, "IDE64 emülasyonu için sektör adedini girin"},
#endif

/* c64/cart/ide64.c */
/* en */ {IDCLS_AUTODETECT_IDE64_GEOMETRY,    N_("Autodetect geometry of formatted images")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_DA, "Autodetektér geometri fra formatterede images"},
/* de */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_DE, "Geometrie von formatierten Image Dateien automatisch erkennen"},
/* fr */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_FR, "Détecter automatiquement la géométrie des images formattées"},
/* hu */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_HU, "Formázott képmások geometriájának automatikus felismerése"},
/* it */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_IT, "Rileva automaticamente la geometria delle immagini formattate"},
/* nl */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_NL, "Automatisch de grootte van de geformateerde bestanden detecteren"},
/* pl */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_SV, "Autodetektera geometri från formaterad avbildningsfil"},
/* tr */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_TR, "Formatlanmýþ imajlarýn yapýlarýný otomatik algýla"},
#endif

/* c64/cart/ide64.c */
/* en */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY,    N_("Do not autodetect geometry of formatted images")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_DA, "Autodetektér ikke geometri fra formatterede images"},
/* de */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_DE, "Geometrie von formatierten Image Dateien nicht automatisch erkennen"},
/* fr */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_FR, "Ne pas détecter automatiquement la géométrie des images formattées"},
/* hu */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_HU, "Formázott képmások geometriáját ne ismerje fel automatikusan"},
/* it */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_IT, "Non rilevare automaticamente la geometrica delle immagini formattate"},
/* nl */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_NL, "Niet automatisch de grootte van de geformateerde bestanden detecteren"},
/* pl */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_SV, "Autodetektera inte geometri från formaterad avbildningsfil"},
/* tr */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_TR, "Formatlanmýþ imajlarýn yapýlarýný otomatik algýlama"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_P_MODELNUMBER,    N_("<modelnumber>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_MODELNUMBER_DA, "<modelnummer>"},
/* de */ {IDCLS_P_MODELNUMBER_DE, "<Modell Nummer>"},
/* fr */ {IDCLS_P_MODELNUMBER_FR, "<numerodemodele>"},
/* hu */ {IDCLS_P_MODELNUMBER_HU, "<modellszám>"},
/* it */ {IDCLS_P_MODELNUMBER_IT, "<numero di modello>"},
/* nl */ {IDCLS_P_MODELNUMBER_NL, "<modelnummer>"},
/* pl */ {IDCLS_P_MODELNUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_MODELNUMBER_SV, "<modellnummer>"},
/* tr */ {IDCLS_P_MODELNUMBER_TR, "<modelnumarasý>"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CBM2_MODEL,    N_("Specify CBM-II model to emulate")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CBM2_MODEL_DA, "Angiv emuleret CBM-II-model"},
/* de */ {IDCLS_SPECIFY_CBM2_MODEL_DE, "CBM-II Modell für Emulation definieren"},
/* fr */ {IDCLS_SPECIFY_CBM2_MODEL_FR, "Spécifier le modèle CBM-II à émuler"},
/* hu */ {IDCLS_SPECIFY_CBM2_MODEL_HU, "Adja meg az emulálandó CBM-II modell számát"},
/* it */ {IDCLS_SPECIFY_CBM2_MODEL_IT, "Specifica il modello di CBM II da emulare"},
/* nl */ {IDCLS_SPECIFY_CBM2_MODEL_NL, "Geef CBM-II-model om te emuleren"},
/* pl */ {IDCLS_SPECIFY_CBM2_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CBM2_MODEL_SV, "Ange CBM-II-modell att emulera"},
/* tr */ {IDCLS_SPECIFY_CBM2_MODEL_TR, "Emüle edilecek CBM-II modelini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_TO_USE_VIC_II,    N_("Specify to use VIC-II")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TO_USE_VIC_II_DA, "Brug VIC-II"},
/* de */ {IDCLS_SPECIFY_TO_USE_VIC_II_DE, "VIC-II Unterstützung aktivieren"},
/* fr */ {IDCLS_SPECIFY_TO_USE_VIC_II_FR, "Spécifier l'utilisation de VIC-II"},
/* hu */ {IDCLS_SPECIFY_TO_USE_VIC_II_HU, "VIC-II használata"},
/* it */ {IDCLS_SPECIFY_TO_USE_VIC_II_IT, "Specifica di utilizzare il VIC-II"},
/* nl */ {IDCLS_SPECIFY_TO_USE_VIC_II_NL, "Gebruik de VIC-II"},
/* pl */ {IDCLS_SPECIFY_TO_USE_VIC_II_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TO_USE_VIC_II_SV, "Ange för att använda VIC-II"},
/* tr */ {IDCLS_SPECIFY_TO_USE_VIC_II_TR, "VIC-II kullanýmý için seçin"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_TO_USE_CRTC,    N_("Specify to use CRTC")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TO_USE_CRTC_DA, "Brug CRTC"},
/* de */ {IDCLS_SPECIFY_TO_USE_CRTC_DE, "CRTC Unterstützung aktivieren"},
/* fr */ {IDCLS_SPECIFY_TO_USE_CRTC_FR, "Spécifier l'utilisation de CRTC"},
/* hu */ {IDCLS_SPECIFY_TO_USE_CRTC_HU, "CRTC használata"},
/* it */ {IDCLS_SPECIFY_TO_USE_CRTC_IT, "Specifica di utilizzare il CRTC"},
/* nl */ {IDCLS_SPECIFY_TO_USE_CRTC_NL, "Gebruik de CRTC"},
/* pl */ {IDCLS_SPECIFY_TO_USE_CRTC_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TO_USE_CRTC_SV, "Ange för att använda CRTC"},
/* tr */ {IDCLS_SPECIFY_TO_USE_CRTC_TR, "CRTC kullanýmý için seçin"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_P_LINENUMBER,    N_("<linenumber>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_LINENUMBER_DA, "<linjenummer>"},
/* de */ {IDCLS_P_LINENUMBER_DE, "<Zeilennummer>"},
/* fr */ {IDCLS_P_LINENUMBER_FR, "<numérodeligne>"},
/* hu */ {IDCLS_P_LINENUMBER_HU, "<sorszám>"},
/* it */ {IDCLS_P_LINENUMBER_IT, "<numero di linea>"},
/* nl */ {IDCLS_P_LINENUMBER_NL, "<lijnnummer>"},
/* pl */ {IDCLS_P_LINENUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_LINENUMBER_SV, "<linjenummer>"},
/* tr */ {IDCLS_P_LINENUMBER_TR, "<satýrnumarasý>"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE,    N_("Specify CBM-II model hardware (0=6x0, 1=7x0)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_DA, "Angiv hardware for CBM-II-model (0=6x0, 1=7x0)"},
/* de */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_DE, "CBM-II Hardware Modell definieren (0=6x0, 1=7x0)"},
/* fr */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_FR, "Spécifier le modèle hardware CBM-II (0=6x0, 1=7x0)"},
/* hu */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_HU, "Adja meg a CBM-II hardver modellt (0=6x0, 1=7x0)"},
/* it */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_IT, "Specifica il modello hardware del CBM-II (0=6x0, 1=7x0)"},
/* nl */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_NL, "Geef CBM-II-hardwaremodel (0=6x0, 1=7x0)"},
/* pl */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_SV, "Ange maskinvara för CBM-II-modell (0=6x0, 1=7x0)"},
/* tr */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_TR, "CBM-II modeli donanýmýný belirt (0=6x0, 1=7x0)"},
#endif

/* cbm2/cbm2-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_P_RAMSIZE,    N_("<ramsize>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_RAMSIZE_DA, "<ramstørrelse>"},
/* de */ {IDCLS_P_RAMSIZE_DE, "<RAM Größe>"},
/* fr */ {IDCLS_P_RAMSIZE_FR, "<tailleram>"},
/* hu */ {IDCLS_P_RAMSIZE_HU, "<ramméret>"},
/* it */ {IDCLS_P_RAMSIZE_IT, "<dimensione della ram>"},
/* nl */ {IDCLS_P_RAMSIZE_NL, "<geheugengrootte>"},
/* pl */ {IDCLS_P_RAMSIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_RAMSIZE_SV, "<ramstorlek>"},
/* tr */ {IDCLS_P_RAMSIZE_TR, "<ramboyutu>"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIZE_OF_RAM,    N_("Specify size of RAM (64/128/256/512/1024 kByte)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SIZE_OF_RAM_DA, "Angiv størrelse på RAM (64/128/256/512/1024 kByte)"},
/* de */ {IDCLS_SPECIFY_SIZE_OF_RAM_DE, "RAM Größe definieren (64/128/256/512/1024 kByte)"},
/* fr */ {IDCLS_SPECIFY_SIZE_OF_RAM_FR, "Spécifier la taille de la RAM (64/128/256/512/1024 KO)"},
/* hu */ {IDCLS_SPECIFY_SIZE_OF_RAM_HU, "Adja meg a RAM méretét (64/128/256/512/1024 kBájt)"},
/* it */ {IDCLS_SPECIFY_SIZE_OF_RAM_IT, "Specifica la dimensione della RAM (4/128/256/512/1024 kByte)"},
/* nl */ {IDCLS_SPECIFY_SIZE_OF_RAM_NL, "Geef geheugengrootte (64/128/256/512/1024 kByte)"},
/* pl */ {IDCLS_SPECIFY_SIZE_OF_RAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SIZE_OF_RAM_SV, "Ange storlek på RAM (64/128/256/512/1024 kByte)"},
/* tr */ {IDCLS_SPECIFY_SIZE_OF_RAM_TR, "RAM boyutunu belirt (64/128/256/512/1024 kByte)"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_1000_NAME,    N_("Specify name of cartridge ROM image for $1000")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_DA, "Angiv navn på cartridge-ROM-image for $1000"},
/* de */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_DE, "Name für ROM Module Image Datei ($1000)"},
/* fr */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_FR, "Spécifier le nom de l’image de cartouche ROM pour $1000"},
/* hu */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_HU, "Adja meg a $1000 címû cartridge ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_IT, "Specifica il nome dell'immagine della cartuccia ROM a $1000"},
/* nl */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_NL, "Geef de naam van het cartridge-ROM-bestand voor $1000"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_SV, "Ange namn på insticksmodul-ROM-avbildning för $1000"},
/* tr */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_TR, "$1000 adresi için kartuþ ROM imaj ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_2000_NAME,    N_("Specify name of cartridge ROM image for $2000-$3fff")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_DA, "Angiv navn på cartridge-ROM-image for $2000-3fff"},
/* de */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_DE, "Name für ROM Module Image Datei ($2000-3fff)"},
/* fr */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_FR, "Spécifier le nom de l’image de cartouche ROM pour $2000-$3fff"},
/* hu */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_HU, "Adja meg a $2000-$3fff címû cartridge ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_IT, "Specifica il nome dell'immagine della ROM della cartuccia per "
                                               "$2000-$3fff"},
/* nl */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_NL, "Geef de naam van het cartridge-ROM-bestand voor $2000-$3fff"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_SV, "Ange namn på insticksmodul-ROM-avbildning för $2000-$3fff"},
/* tr */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_TR, "$2000-$3fff adres aralýðý için kartuþ ROM imaj ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_4000_NAME,    N_("Specify name of cartridge ROM image for $4000-$5fff")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_DA, "Angiv navn på cartridge-ROM-image for $4000-$5fff"},
/* de */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_DE, "Name für ROM Module Image Datei ($4000-5fff)"},
/* fr */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_FR, "Spécifier le nom de l’image de cartouche ROM pour $4000-$5fff"},
/* hu */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_HU, "Adja meg a $4000-$5fff címû cartridge ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_IT, "Specifica il nome dell'immagine della ROM della cartuccia per "
                                               "$4000-$5fff"},
/* nl */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_NL, "Geef de naam van het cartridge-ROM-bestand voor $4000-$5fff"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_SV, "Ange namn på insticksmodul-ROM-avbildning för $4000-$5fff"},
/* tr */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_TR, "$4000-$5fff adres aralýðý için kartuþ ROM imaj ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_6000_NAME,    N_("Specify name of cartridge ROM image for $6000-$7fff")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_DA, "Angiv navn på cartridge-ROM-image for $6000-$7fff"},
/* de */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_DE, "Name für ROM Module Image Datei ($6000-7fff)"},
/* fr */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_FR, "Spécifier le nom de l’image de cartouche ROM pour $6000-$7fff"},
/* hu */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_HU, "Adja meg a $6000-$7fff címû cartridge ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_IT, "Specifica il nome dell'immagine della ROM della cartuccia per "
                                               "$6000-$7fff"},
/* nl */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_NL, "Geef de naam van het cartridge-ROM-bestand voor $6000-$7fff"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_SV, "Ange namn på insticksmodul-ROM-avbildning för $6000-$7fff"},
/* tr */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_TR, "$6000-$7fff adres aralýðý için kartuþ ROM imaj ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800,    N_("Enable RAM mapping in $0800-$0FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_DA, "Aktivér RAM-mappning på $0800-$0FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_DE, "RAM Zuordnung in $0800-$0FFF aktivieren"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_FR, "Activer le mappage RAM dans $0800-$0FFF"},
/* hu */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_HU, "$0800-$0FFF RAM leképzés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_IT, "Attiva il mappaggio della RAM a $0800-$0FFF"},
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_NL, "Activeer RAM op adres $0800-$0FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_SV, "Aktivera RAM-mappning på $0800-$0FFF"},
/* tr */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_TR, "$0800-$0FFF aralýðýnda RAM eþleþtirmeyi aktif et"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800,    N_("Disable RAM mapping in $0800-$0FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_DA, "Deaktiver RAM-mappning på $0800-$0FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_DE, "RAM Zuordnung in $0800-$0FFF deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_FR, "Désactiver le mappage RAM dans $0800-$0FFF"},
/* hu */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_HU, "$0800-$0FFF RAM leképzés tiltása"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_IT, "Disattiva il mappaggio della RAM a $0800-$0FFF"},
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_NL, "RAM op adres $0800-$0FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_SV, "Inaktivera RAM-mappning på $0800-$0FFF"},
/* tr */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_TR, "$0800-$0FFF aralýðýnda RAM eþleþtirmeyi pasifleþtir"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000,    N_("Enable RAM mapping in $1000-$1FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_DA, "Aktivér RAM-mappning på $1000-$1FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_DE, "RAM Zuordnung in $1000-$1FFF aktivieren"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_FR, "Activer le mappage RAM dans $1000-$1FFF"},
/* hu */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_HU, "$1000-$1FFF RAM leképzés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_IT, "Attiva il mappaggio della RAM a $1000-$1FFF"},
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_NL, "Activeer RAM op adres $1000-$1FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_SV, "Aktivera RAM-mappning på $1000-$1FFF"},
/* tr */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_TR, "$1000-$1FFF aralýðýnda RAM eþleþtirmeyi aktif et"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000,    N_("Disable RAM mapping in $1000-$1FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_DA, "Deaktiver RAM-mappning på $1000-$1FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_DE, "RAM Zuordnung in $1000-$1FFF deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_FR, "Désactiver le mappage RAM dans $1000-$1FFF"},
/* hu */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_HU, "$1000-$1FFF RAM leképzés tiltása"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_IT, "Disattiva il mappaggio della RAM a $1000-$1FFF"},
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_NL, "RAM op adres $1000-$1FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_SV, "Inaktivera RAM-mappning på $1000-$1FFF"},
/* tr */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_TR, "$1000-$1FFF aralýðýnda RAM eþleþtirmeyi pasifleþtir"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000,    N_("Enable RAM mapping in $2000-$3FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_DA, "Aktivér RAM-mappning på $2000-$3FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_DE, "RAM Zuordnung in $2000-$3FFF aktivieren"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_FR, "Activer le mappage RAM dans $2000-$3FFF"},
/* hu */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_HU, "$2000-$3FFF RAM leképzés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_IT, "Attiva il mappaggio della RAM a $2000-$3FFF"},
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_NL, "Activeer RAM op adres $2000-$3FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_SV, "Aktivera RAM-mappning på $2000-$3FFF"},
/* tr */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_TR, "$2000-$3FFF aralýðýnda RAM eþleþtirmeyi aktif et"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000,    N_("Disable RAM mapping in $2000-$3FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_DA, "Deaktiver RAM-mappning på $2000-$3FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_DE, "RAM Zuordnung in $2000-$3FFF deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_FR, "Désactiver le mappage RAM dans $2000-$3FFF"},
/* hu */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_HU, "$2000-$3FFF RAM leképzés tiltása"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_IT, "Disattiva il mappaggio della RAM a $2000-$3FFF"},
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_NL, "RAM op adres $2000-$3FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_SV, "Inaktivera RAM-mappning på $2000-$3FFF"},
/* tr */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_TR, "$2000-$3FFF aralýðýnda RAM eþleþtirmeyi pasifleþtir"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000,    N_("Enable RAM mapping in $4000-$5FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_DA, "Aktivér RAM-mappning på $4000-$5FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_DE, "RAM Zuordnung in $4000-$5FFF aktivieren"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_FR, "Activer le mappage RAM dans $4000-$5FFF"},
/* hu */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_HU, "$4000-$5FFF RAM leképzés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_IT, "Attiva il mappaggio della RAM a $4000-$5FFF"},
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_NL, "Activeer RAM op adres $4000-$5FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_SV, "Aktivera RAM-mappning på $4000-$5FFF"},
/* tr */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_TR, "$4000-$5FFF aralýðýnda RAM eþleþtirmeyi aktif et"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000,    N_("Disable RAM mapping in $4000-$5FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_DA, "Deaktiver RAM-mappning på $4000-$5FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_DE, "RAM Zuordnung in $4000-$5FFF deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_FR, "Désactiver le mappage RAM dans $4000-$5FFF"},
/* hu */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_HU, "$4000-$5FFF RAM leképzés tiltása"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_IT, "Disattiva il mappaggio della RAM a $4000-$5FFF"},
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_NL, "RAM op adres $4000-$5FFF uitchakelen"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_SV, "Inaktivera RAM-mappning på $4000-$5FFF"},
/* tr */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_TR, "$4000-$5FFF aralýðýnda RAM eþleþtirmeyi pasifleþtir"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000,    N_("Enable RAM mapping in $6000-$7FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_DA, "Aktivér RAM-mappning på $6000-$7FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_DE, "RAM Zuordnung in $6000-$7FFF aktivieren"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_FR, "Activer le mappage RAM dans $6000-$7FFF"},
/* hu */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_HU, "$6000-$7FFF RAM leképzés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_IT, "Attiva il mappaggio della RAM a $6000-$7FFF"},
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_NL, "Activeer RAM op adres $6000-$7FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_SV, "Aktivera RAM-mappning på $6000-$7FFF"},
/* tr */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_TR, "$6000-$7FFF aralýðýnda RAM eþleþtirmeyi aktif et"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000,    N_("Disable RAM mapping in $6000-$7FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_DA, "Deaktiver RAM-mappning på $6000-$7FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_DE, "RAM Zuordnung in $6000-$7FFF deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_FR, "Désactiver le mappage RAM dans $6000-$7FFF"},
/* hu */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_HU, "$6000-$7FFF RAM leképzés tiltása"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_IT, "Disattiva il mappaggio della RAM a $6000-$7FFF"},
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_NL, "RAM op adres $6000-$7FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_SV, "Inaktivera RAM-mappning på $6000-$7FFF"},
/* tr */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_TR, "$6000-$7FFF aralýðýnda RAM eþleþtirmeyi pasifleþtir"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000,    N_("Enable RAM mapping in $C000-$CFFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_DA, "Aktivér RAM-mappning på $C000-$CFFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_DE, "RAM Zuordnung in $C000-$CFFF aktivieren"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_FR, "Activer le mappage RAM dans $C000-$CFFF"},
/* hu */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_HU, "$C000-$CFFF RAM leképzés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_IT, "Attiva il mappaggio della RAM a $C000-$CFFF"},
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_NL, "Activeer RAM op adres $C000-$CFFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_SV, "Aktivera RAM-mappning på $C000-$CFFF"},
/* tr */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_TR, "$C000-$CFFF aralýðýnda RAM eþleþtirmeyi aktif et"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000,    N_("Disable RAM mapping in $C000-$CFFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_DA, "Deaktiver RAM-mappning på $C000-$CFFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_DE, "RAM Zuordnung in $C000-$CFFF deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_FR, "Désactiver le mappage RAM dans $C000-$CFFF"},
/* hu */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_HU, "$C000-$CFFF RAM leképzés tiltása"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_IT, "Disattiva il mappaggio della RAM a $C000-$CFFF"},
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_NL, "RAM op adres $C000-$CFFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_SV, "Inaktivera RAM-mappning på $C000-$CFFF"},
/* tr */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_TR, "$C000-$CFFF aralýðýnda RAM eþleþtirmeyi pasifleþtir"},
#endif

#ifdef COMMON_KBD
/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KEYMAP_INDEX,    N_("Specify index of keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_KEYMAP_INDEX_DA, "Angiv indeks for tastaturindstillingsfil"},
/* de */ {IDCLS_SPECIFY_KEYMAP_INDEX_DE, "Index der Keymap Datei definieren"},
/* fr */ {IDCLS_SPECIFY_KEYMAP_INDEX_FR, "Spécifier l'index du fichier de mappage clavier"},
/* hu */ {IDCLS_SPECIFY_KEYMAP_INDEX_HU, "Adja meg a billentyûzet leképzés fájl típusát"},
/* it */ {IDCLS_SPECIFY_KEYMAP_INDEX_IT, "Specifica l'indice del file della mappa della tastiera"},
/* nl */ {IDCLS_SPECIFY_KEYMAP_INDEX_NL, "Geef de index van het keymapbestand"},
/* pl */ {IDCLS_SPECIFY_KEYMAP_INDEX_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_KEYMAP_INDEX_SV, "Ange index o tangentbordsfil"},
/* tr */ {IDCLS_SPECIFY_KEYMAP_INDEX_TR, "Tuþ haritasý dosyasýnýn indeksini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME,    N_("Specify name of graphics keyboard symbolic keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_DA, "Angiv fil for symbolsk tastaturemulering for \"graphics\"-tastatur"},
/* de */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_DE, "Keymap Datei für graphics keyboard symbolic definieren"},
/* fr */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier symbolique"},
/* hu */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_HU, "Adja meg a grafikus és szimbolikus billentyûzet leképzési fájl nevét."},
/* it */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_IT, "Specifica il nome del file della mappa simbolica della tastiera grafica"},
/* nl */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_NL, "Geef de naam van het symbolisch keymapbestand voor het grafische "
                                                "toetsenbord"},
/* pl */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_SV, "Ange fil för symbolisk tangentbordsemulering för "
                                                "\"graphics\"-tangentbord"},
/* tr */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_TR, "Grafik klavyesi sembolik tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME,    N_("Specify name of graphics keyboard positional keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_DA, "Angiv fil for positionsbestemt tastaturemulering for \"graphics\"-"
                                                "tastatur"},
/* de */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_DE, "Keymap Datei für graphics keyboard positional definieren"},
/* fr */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier positionnel"},
/* hu */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_HU, "Adja meg a grafikus és pozíció szerinti billentyûzet leképzési fájl "
                                                "nevét."},
/* it */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_IT, "Specifica il nome del file della mappa posizionale della tastiera "
                                                "grafica"},
/* nl */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_NL, "Geef de naam van het positioneel keymapbestand voor het grafische "
                                                "toetsenbord"},
/* pl */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_SV, "Ange fil för positionsriktig tangentbordsemulering för "
                                                "\"graphics\"-tangentbord"},
/* tr */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_TR, "Grafik klavyesi konumsal tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME,    N_("Specify name of UK business keyboard symbolic keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_DA, "Angiv fil for symbolsk tastaturemulering for Britisk \"business\"-"
                                                "tastatur"},
/* de */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_DE, "Keymap Datei für UK business keyboard symbolic definieren"},
/* fr */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier symbolique UK"},
/* hu */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_HU, "Adja meg az angol, hivatalos, szimbolikus billentyûzet leképzési fájl "
                                                "nevét."},
/* it */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_IT, "Specifica il nome del file della mappa simbolica della tastiera UK "
                                                "business"},
/* nl */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_NL, "Geef de naam van het symbolisch keymapbestand voor het "
                                                "UK-businesstoetsenbord"},
/* pl */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_SV, "Ange fil för symbolisk tangentbordsemulering för brittiskt "
                                                "\"business\"-tangentbord"},
/* tr */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_TR, "UK business klavyesi sembolik tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME,    N_("Specify name of UK business keyboard positional keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_DA, "Angiv fil for positionsbestemt tastaturemulering for Britisk "
                                                "\"business\"-tastatur"},
/* de */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_DE, "Keymap Datei für UK business keyboard positional definieren"},
/* fr */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier positionnel UK"},
/* hu */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_HU, "Adja meg a UK hivatalos pozíció szerinti billentyûzet leképzési fájl "
                                                "nevét."},
/* it */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_IT, "Adja meg az angol, hivatalos, pozíció szerinti billentyûzet leképzési "
                                                "fájl nevét."},
/* nl */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_NL, "Geef de naam van het positioneel keymapbestand voor het "
                                                "UK-businesstoetsenbord"},
/* pl */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_SV, "Ange fil för positionsriktig tangentbordsemulering för brittiskt "
                                                "\"business\"-tangentbord"},
/* tr */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_TR, "UK business klavyesi konumsal tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME,    N_("Specify name of German business keyboard symbolic keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_DA, "Angiv fil for symbolsk tastaturemulering for Tysk \"business\"-tastatur"},
/* de */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_DE, "Keymap Datei für German business keyboard symbolic definieren"},
/* fr */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier symbolique allemand"},
/* hu */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_HU, "Adja meg a német, hivatalos, szimbolikus billentyûzet leképzési fájl "
                                                "nevét."},
/* it */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_IT, "Specifica il nome del file della mappa simbolica della tastiera "
                                                "business tedesca"},
/* nl */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_NL, "Geef de naam van het symbolisch keymapbestand voor het Duitse "
                                                "businesstoetsenbord"},
/* pl */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_SV, "Ange fil för symbolisk tangentbordsemulering för tyskt "
                                                "\"business\"-tangentbord"},
/* tr */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_TR, "Alman business klavyesi sembolik tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME,    N_("Specify name of German business keyboard positional keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_DA, "Angiv fil for positionsbestemt tastaturemulering for Tysk "
                                                "\"business\"-tastatur"},
/* de */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_DE, "Keymap Datei für German business keyboard positional definieren"},
/* fr */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier positionnel allemand"},
/* hu */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_HU, "Adja meg a német, hivatalos, pozíció szerinti billentyûzet leképzési "
                                                "fájl nevét."},
/* it */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_IT, "Specifica il nome del file della mappa posizionale della tastiera "
                                                "business tedesca"},
/* nl */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_NL, "Geef de naam van het positioneel keymapbestand voor het Duitse "
                                                "businesstoetsenbord"},
/* pl */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_SV, "Ange fil för positionsriktig tangentbordsemulering för tyskt "
                                                "\"business\"-tangentbord"},
/* tr */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_TR, "Alman business klavyesi konumsal tuþ haritasý dosyasýnýn ismini belirt"},
#endif
#endif

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_ENABLE_TRUE_DRIVE,    N_("Enable hardware-level emulation of disk drives")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_TRUE_DRIVE_DA, "Aktivér emulering af diskettedrev på hardware-niveau"},
/* de */ {IDCLS_ENABLE_TRUE_DRIVE_DE, "Hardware Emulation für Disklaufwerke aktivieren"},
/* fr */ {IDCLS_ENABLE_TRUE_DRIVE_FR, "Activer l'émulation de disques au niveau physique"},
/* hu */ {IDCLS_ENABLE_TRUE_DRIVE_HU, "Lemezegységek hardver szintû emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_TRUE_DRIVE_IT, "Attiva l'emulazione hardware dei disk drive"},
/* nl */ {IDCLS_ENABLE_TRUE_DRIVE_NL, "Activeer hardwarematige emulatie van diskdrives"},
/* pl */ {IDCLS_ENABLE_TRUE_DRIVE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TRUE_DRIVE_SV, "Aktivera emulering av diskettstationer på maskinvarunivå"},
/* tr */ {IDCLS_ENABLE_TRUE_DRIVE_TR, "Disk sürücülerinin donaným seviyesinde emülasyonunu aktif et"},
#endif

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_DISABLE_TRUE_DRIVE,    N_("Disable hardware-level emulation of disk drives")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_TRUE_DRIVE_DA, "Deaktivér emulering af diskettedrev på hardware-niveau"},
/* de */ {IDCLS_DISABLE_TRUE_DRIVE_DE, "Hardware Emulation für Disklaufwerke deaktivieren"},
/* fr */ {IDCLS_DISABLE_TRUE_DRIVE_FR, "Désactiver l'émulation de disques au niveau physique"},
/* hu */ {IDCLS_DISABLE_TRUE_DRIVE_HU, "Lemezegységek hardver szintû emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_TRUE_DRIVE_IT, "Disattiva l'emulazione hardware dei disk drive"},
/* nl */ {IDCLS_DISABLE_TRUE_DRIVE_NL, "Hardwarematige emulatie van diskdrives uitschakelen"},
/* pl */ {IDCLS_DISABLE_TRUE_DRIVE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_TRUE_DRIVE_SV, "Inaktivera emulering av diskettstationer på maskinvarunivå"},
/* tr */ {IDCLS_DISABLE_TRUE_DRIVE_TR, "Disk sürücülerinin donaným seviyesinde emülasyonunu pasifleþtir"},
#endif

/* drive/drive-cmdline-options.c, printerdrv/interface-serial.c,
   attach.c, drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_P_TYPE,    N_("<type>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_TYPE_DA, "<type>"},
/* de */ {IDCLS_P_TYPE_DE, "<Typ>"},
/* fr */ {IDCLS_P_TYPE_FR, "<type>"},
/* hu */ {IDCLS_P_TYPE_HU, "<típus>"},
/* it */ {IDCLS_P_TYPE_IT, "<tipo>"},
/* nl */ {IDCLS_P_TYPE_NL, "<soort>"},
/* pl */ {IDCLS_P_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_TYPE_SV, "<typ>"},
/* tr */ {IDCLS_P_TYPE_TR, "<tip>"},
#endif

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_SET_DRIVE_TYPE,    N_("Set drive type (0: no drive)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DRIVE_TYPE_DA, "Angiv enhedstype (0: ingen enhed)"},
/* de */ {IDCLS_SET_DRIVE_TYPE_DE, "Setze Laufwerkstyp (0: kein Laufwerk)"},
/* fr */ {IDCLS_SET_DRIVE_TYPE_FR, "Spécifier le type de lecteur de disque (0: pas de lecteur)"},
/* hu */ {IDCLS_SET_DRIVE_TYPE_HU, "Adja meg a lemezegység típusát (0: nincs)"},
/* it */ {IDCLS_SET_DRIVE_TYPE_IT, "Imposta il tipo di drive (0: nessun drive)"},
/* nl */ {IDCLS_SET_DRIVE_TYPE_NL, "Zet drive soort (0: geen drive)"},
/* pl */ {IDCLS_SET_DRIVE_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DRIVE_TYPE_SV, "Ange enhetstyp (0: ingen enhet)"},
/* tr */ {IDCLS_SET_DRIVE_TYPE_TR, "Sürücü tipini ayarlayýn (0: sürücü yok)"},
#endif

/* drive/drive-cmdline-options.c,
   drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_P_METHOD,    N_("<method>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_METHOD_DA, "<metode>"},
/* de */ {IDCLS_P_METHOD_DE, "<Methode>"},
/* fr */ {IDCLS_P_METHOD_FR, "<methode>"},
/* hu */ {IDCLS_P_METHOD_HU, "<mód>"},
/* it */ {IDCLS_P_METHOD_IT, "<metodo>"},
/* nl */ {IDCLS_P_METHOD_NL, "<methode>"},
/* pl */ {IDCLS_P_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_METHOD_SV, "<metod>"},
/* tr */ {IDCLS_P_METHOD_TR, "<metod>"},
#endif

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_SET_DRIVE_EXTENSION_POLICY,    N_("Set drive 40 track extension policy (0: never, 1: ask, 2: on access)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_DA, "Angiv drevets regel for 40-spors udviddelse (0: aldrig, 1: spørg, 2: ved "
                                               "tilgang)"},
/* de */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_DE, "40 Spur Erweiterungsmethode (0: nie, 1: Rückfrage, 2: bei Bedarf)"},
/* fr */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_FR, "Spécifier la règle d'extention 40 pistes (0: jamais, 1: demander, 2: à "
                                               "l'accès)"},
/* hu */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_HU, "Adja meg a 40 sávra kiterjesztés terjesztés módját (0: soha, 1: kérdés, "
                                               "2: hozzáféréskor)"},
/* it */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_IT, "Imposta la politica di estensione a 40 tracce del drive (0: mai, 1: a "
                                               "richiesta, 2: all'accesso)"},
/* nl */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_NL, "Stel beleid voor 40-sporenuitbreiding drive in (0: nooit, 1: vraag, 2: "
                                               "bij toegang)"},
/* pl */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_SV, "Ange regel för 40-spårsutökning (0: aldrig, 1: fråga, 2: vid åtkomst)"},
/* tr */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_TR, "Sürücü 40 track büyüme kuralýný ayarla (0: asla, 1: sor, 2: eriþildiðinde)"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME,    N_("Specify name of 1541 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_DA, "Angiv navn på 1541-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_DE, "Name der 1541 DOS ROM Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1541 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_HU, "Adja meg az 1541 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1541"},
/* nl */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_NL, "Geef de naam van het 1541 DOS ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_SV, "Ange namn på 1541-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_TR, "1541 DOS ROM imajýnýn ismini belirt"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME,    N_("Specify name of 1541-II DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_DA, "Angivn navn på 1541-II-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_DE, "Name der 1541-II DOS ROM Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1541-II DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_HU, "Adja meg az 1541-II DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1541-II"},
/* nl */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_NL, "Geef de naam van het 1541-II DOS ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_SV, "Ange namn på 1541-II-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_TR, "1541-II DOS ROM imajýnýn ismini belirt"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME,    N_("Specify name of 1570 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_DA, "Angivn navn på 1570-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_DE, "Name der 1570 DOS ROM Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1570 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_HU, "Adja meg az 1570 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1570"},
/* nl */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_NL, "Geef de naam van het 1570 DOS ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_SV, "Ange namn på 1570-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_TR, "1570 DOS ROM imajýnýn ismini belirt"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME,    N_("Specify name of 1571 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_DA, "Angivn navn på 1571-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_DE, "Name der 1571 DOS ROM Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1571 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_HU, "Adja meg az 1571 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1571"},
/* nl */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_NL, "Geef de naam van het 1571 DOS ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_SV, "Ange namn på 1571-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_TR, "1571 DOS ROM imajýnýn ismini belirt"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME,    N_("Specify name of 1581 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_DA, "Angivn navn på 1581-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_DE, "Name der 1581 DOS ROM Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1581 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_HU, "Adja meg az 1581 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1581"},
/* nl */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_NL, "Geef de naam van het 1581 DOS ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_SV, "Ange namn på 1581-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_TR, "1581 DOS ROM imajýnýn ismini belirt"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SET_IDLE_METHOD,    N_("Set drive idling method (0: no traps, 1: skip cycles, 2: trap idle)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_IDLE_METHOD_DA, "Vælg pausemetode for diskettedrev (0: ingen traps, 1: spring over cykler, 2: "
                                    "trap ledig)"},
/* de */ {IDCLS_SET_IDLE_METHOD_DE, "Laufwerks idling Methode (0: kein Traps, 1: Zyklen verwerfen, 2: trap idle)"},
/* fr */ {IDCLS_SET_IDLE_METHOD_FR, "Spécifier la méthode d'idle (0: pas de traps, 1: sauter des cycles, 2: trap idle)"},
/* hu */ {IDCLS_SET_IDLE_METHOD_HU, "Lemezegység üresjárási módja (0: folytonos emuláció, 1: ciklusok kihagyása, 2: üres "
                                    "DOS ciklusok kihagyása)"},
/* it */ {IDCLS_SET_IDLE_METHOD_IT, "Imposta il metodo per rilevare l'inattività del drive (0: non rilevare, 1: "
                                    "salta cicli, 2: rileva inattività)"},
/* nl */ {IDCLS_SET_IDLE_METHOD_NL, "Stel de drive-idlemethode in (0: geen traps, 1: sla cycli over, 2: trap idle)"},
/* pl */ {IDCLS_SET_IDLE_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_IDLE_METHOD_SV, "Ställ in väntemetod för diskettstation (0: fånta inge, 1: hoppa cykler, 2: fånga "
                                    "väntan)"},
/* tr */ {IDCLS_SET_IDLE_METHOD_TR, "Sürücü boþa vakit geçirme metodunu ayarlayýn (0: trap yok, 1: cyclelarý atla, 2: "
                                    "trap boþa vakit geçirme)"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_2000,    N_("Enable 8KB RAM expansion at $2000-$3FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DRIVE_RAM_2000_DA, "Aktivér 8KB RAM-udviddelse på $2000-$3FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_2000_DE, "8KB RAM Erweiterung bei $2000-3fff aktivieren"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_2000_FR, "Enable 8KB RAM expansion at $2000-$3FFF"},
/* hu */ {IDCLS_ENABLE_DRIVE_RAM_2000_HU, "8KB RAM kiterjeszés engedélyezése a $2000-$3FFF címen"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_2000_IT, "Attiva l'espansione di RAM di 8KB a $2000-$3FFF"},
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_2000_NL, "Activeer 8KB RAM-uitbreiding op adres $2000-$3FFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_2000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_2000_SV, "Aktivera 8KB RAM-expansion på $2000-$3FFF"},
/* tr */ {IDCLS_ENABLE_DRIVE_RAM_2000_TR, "$2000-$3FFF arasýnda 8KB RAM geniþletmeyi aktif et"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_2000,    N_("Disable 8KB RAM expansion at $2000-$3FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DRIVE_RAM_2000_DA, "Deaktiver 8KB RAM-udviddelse på $2000-$3FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_2000_DE, "8KB RAM Erweiterung bei $2000-3fff deaktivieren"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_2000_FR, "Désactiver l'exansion de 8KB RAM à $2000-$3FFF"},
/* hu */ {IDCLS_DISABLE_DRIVE_RAM_2000_HU, "8KB RAM kiterjeszés tiltása a $2000-$3FFF címen"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_2000_IT, "Disattiva l'espansione di RAM di 8KB a $2000-$3FFF"},
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_2000_NL, "8KB RAM-uitbreiding op adres $2000-$3FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_2000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_2000_SV, "Inaktivera 8KB RAM-expansion på $2000-$3FFF"},
/* tr */ {IDCLS_DISABLE_DRIVE_RAM_2000_TR, "$2000-$3FFF arasýnda 8KB RAM geniþletmeyi pasifleþtir"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_4000,    N_("Enable 8KB RAM expansion at $4000-$5FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DRIVE_RAM_4000_DA, "Aktivér 8KB RAM-udviddelse på $4000-$5FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_4000_DE, "8KB RAM Erweiterung bei $4000-5fff aktivieren"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_4000_FR, "Enable 8KB RAM expansion at $4000-$5FFF"},
/* hu */ {IDCLS_ENABLE_DRIVE_RAM_4000_HU, "8KB RAM kiterjeszés engedélyezése a $4000-$5FFF címen"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_4000_IT, "Attiva l'espansione di RAM di 8KB a $4000-$5FFF"},
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_4000_NL, "Activeer 8KB RAM-uitbreiding op adres $4000-$5FFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_4000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_4000_SV, "Aktivera 8KB RAM-expansion på $4000-$5FFF"},
/* tr */ {IDCLS_ENABLE_DRIVE_RAM_4000_TR, "$4000-$5FFF arasýnda 8KB RAM geniþletmeyi aktif et"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_4000,    N_("Disable 8KB RAM expansion at $4000-$5FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DRIVE_RAM_4000_DA, "Deaktiver 8KB RAM-udviddelse på $4000-$5FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_4000_DE, "8KB RAM Erweiterung bei $4000-5fff deaktivieren"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_4000_FR, "Désactiver l'exansion de 8KB RAM à $4000-$5FFF"},
/* hu */ {IDCLS_DISABLE_DRIVE_RAM_4000_HU, "8KB RAM kiterjeszés tiltása a $4000-$5FFF címen"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_4000_IT, "Disattiva l'espansione di RAM di 8KB a $4000-$5FFF"},
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_4000_NL, "8KB RAM-uitbreiding op adres $4000-$5FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_4000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_4000_SV, "Inaktivera 8KB RAM-expansion på $4000-$5FFF"},
/* tr */ {IDCLS_DISABLE_DRIVE_RAM_4000_TR, "$4000-$5FFF arasýnda 8KB RAM geniþletmeyi pasifleþtir"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_6000,    N_("Enable 8KB RAM expansion at $6000-$7FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DRIVE_RAM_6000_DA, "Aktivér 8KB RAM-udviddelse på $6000-$7FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_6000_DE, "8KB RAM Erweiterung bei $6000-7fff aktivieren"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_6000_FR, "Enable 8KB RAM expansion at $6000-$7FFF"},
/* hu */ {IDCLS_ENABLE_DRIVE_RAM_6000_HU, "8KB RAM kiterjeszés engedélyezése a $6000-$7FFF címen"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_6000_IT, "Attiva l'espansione di RAM di 8KB a $6000-$7FFF"},
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_6000_NL, "Activeer 8KB RAM-uitbreiding op adres $6000-$7FFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_6000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_6000_SV, "Aktivera 8KB RAM-expansion på $6000-$7FFF"},
/* tr */ {IDCLS_ENABLE_DRIVE_RAM_6000_TR, "$6000-$7FFF arasýnda 8KB RAM geniþletmeyi aktif et"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_6000,    N_("Disable 8KB RAM expansion at $6000-$7FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DRIVE_RAM_6000_DA, "Deaktiver 8KB RAM-udviddelse på $6000-$7FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_6000_DE, "8KB RAM Erweiterung bei $6000-7fff deaktivieren"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_6000_FR, "Désactiver l'exansion de 8KB RAM à $6000-$7FFF"},
/* hu */ {IDCLS_DISABLE_DRIVE_RAM_6000_HU, "8KB RAM kiterjeszés tiltása a $6000-$7FFF címen"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_6000_IT, "Disattiva l'espansione di RAM di 8KB a $6000-$7FFF"},
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_6000_NL, "8KB RAM-uitbreiding op adres $6000-$7FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_6000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_6000_SV, "Inaktivera 8KB RAM-expansion på $6000-$7FFF"},
/* tr */ {IDCLS_DISABLE_DRIVE_RAM_6000_TR, "$6000-$7FFF arasýnda 8KB RAM geniþletmeyi pasifleþtir"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_8000,    N_("Enable 8KB RAM expansion at $8000-$9FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DRIVE_RAM_8000_DA, "Aktivér 8KB RAM-udviddelse på $8000-$9FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_8000_DE, "8KB RAM Erweiterung bei $8000-9fff aktivieren"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_8000_FR, "Enable 8KB RAM expansion at $8000-$9FFF"},
/* hu */ {IDCLS_ENABLE_DRIVE_RAM_8000_HU, "8KB RAM kiterjeszés engedélyezése a $8000-$9FFF címen"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_8000_IT, "Attiva l'espansione di RAM di 8KB a $8000-$9FFF"},
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_8000_NL, "Activeer 8KB RAM-uitbreiding op adres $8000-$9FFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_8000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_8000_SV, "Aktivera 8KB RAM-expansion på $8000-$9FFF"},
/* tr */ {IDCLS_ENABLE_DRIVE_RAM_8000_TR, "$8000-$9FFF arasýnda 8KB RAM geniþletmeyi aktif et"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_8000,    N_("Disable 8KB RAM expansion at $8000-$9FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DRIVE_RAM_8000_DA, "Deaktiver 8KB RAM-udviddelse på $8000-$9FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_8000_DE, "8KB RAM Erweiterung bei $8000-9fff deaktivieren"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_8000_FR, "Désactiver l'exansion de 8KB RAM à $8000-$9FFF"},
/* hu */ {IDCLS_DISABLE_DRIVE_RAM_8000_HU, "8KB RAM kiterjeszés tiltása a $8000-$9FFF címen"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_8000_IT, "Disattiva l'espansione di RAM di 8KB a $8000-$9FFF"},
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_8000_NL, "8KB RAM-uitbreiding op adres $8000-$9FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_8000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_8000_SV, "Inaktivera 8KB RAM-expansion på $8000-$9FFF"},
/* tr */ {IDCLS_DISABLE_DRIVE_RAM_8000_TR, "$8000-$9FFF arasýnda 8KB RAM geniþletmeyi pasifleþtir"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_A000,    N_("Enable 8KB RAM expansion at $A000-$BFFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DRIVE_RAM_A000_DA, "Aktivér 8KB RAM-udviddelse på $A000-$BFFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_A000_DE, "8KB RAM Erweiterung bei $a000-bfff aktivieren"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_A000_FR, "Enable 8KB RAM expansion at $A000-$BFFF"},
/* hu */ {IDCLS_ENABLE_DRIVE_RAM_A000_HU, "8KB RAM kiterjeszés engedélyezése a $A000-$BFFF címen"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_A000_IT, "Attiva l'espansione di RAM di 8KB a $A000-$BFFF"},
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_A000_NL, "Activeer 8KB RAM-uitbreiding op adres $A000-$BFFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_A000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_A000_SV, "Aktivera 8KB RAM-expansion på $A000-$BFFF"},
/* tr */ {IDCLS_ENABLE_DRIVE_RAM_A000_TR, "$A000-$BFFF arasýnda 8KB RAM geniþletmeyi aktif et"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_A000,    N_("Disable 8KB RAM expansion at $A000-$BFFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DRIVE_RAM_A000_DA, "Deaktiver 8KB RAM-udviddelse på $A000-$BFFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_A000_DE, "8KB RAM Erweiterung bei $a000-bfff deaktivieren"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_A000_FR, "Désactiver l'exansion de 8KB RAM à $A000-$BFFF"},
/* hu */ {IDCLS_DISABLE_DRIVE_RAM_A000_HU, "8KB RAM kiterjeszés tiltása a $A000-$BFFF címen"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_A000_IT, "Disattiva l'espansione di RAM di 8KB a $A000-$BFFF"},
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_A000_NL, "8KB RAM-uitbreiding op adres $A000-$BFFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_A000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_A000_SV, "Inaktivera 8KB RAM-expansion på $A000-$BFFF"},
/* tr */ {IDCLS_DISABLE_DRIVE_RAM_A000_TR, "$A000-$BFFF arasýnda 8KB RAM geniþletmeyi pasifleþtir"},
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_PAR_CABLE_C64EXP_TYPE,    N_("Set parallel cable type (0: none, 1: standard, 2: Dolphin DOS 3)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PAR_CABLE_C64EXP_TYPE_DA, "Vælg typen af parallelkabel (0: ingen, 1: standard, 2: Dolphin DOS 3)"},
/* de */ {IDCLS_PAR_CABLE_C64EXP_TYPE_DE, "Parallelkabel Typ (0: kein, 1: Standard, 2: Dolphin DOS 3)"},
/* fr */ {IDCLS_PAR_CABLE_C64EXP_TYPE_FR, "Définir le type de câble parallèle (0: aucun, 1: standard, 2: Dolphin DOS 3)"},
/* hu */ {IDCLS_PAR_CABLE_C64EXP_TYPE_HU, "Adja meg a párhuzamos kábel típusát (0: nincs, 1: standard, 2: Dolphin DOS 3)"},
/* it */ {IDCLS_PAR_CABLE_C64EXP_TYPE_IT, "Imposta il tipo di cavo parallelo (0: nessuno, 1: standard, 2: Dolphin DOS 3)"},
/* nl */ {IDCLS_PAR_CABLE_C64EXP_TYPE_NL, "Zet parallele-kabelsoort (0: geen, 1: standaard, 2: Dolphin DOS 3)"},
/* pl */ {IDCLS_PAR_CABLE_C64EXP_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PAR_CABLE_C64EXP_TYPE_SV, "Välj typ av parallellkabel (0: ingen, 1: standard, 2: Dolphin DOS 3)"},
/* tr */ {IDCLS_PAR_CABLE_C64EXP_TYPE_TR, "Paralel kablo tipini ayarla (0: yok, 1: standart, 2: Dolphin DOS 3)"},
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_ENABLE_PROFDOS,    N_("Enable Professional DOS")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_PROFDOS_DA, "Aktivér Professional DOS"},
/* de */ {IDCLS_ENABLE_PROFDOS_DE, "Professional DOS aktivieren"},
/* fr */ {IDCLS_ENABLE_PROFDOS_FR, "Activer le DOS Professionnel"},
/* hu */ {IDCLS_ENABLE_PROFDOS_HU, "Professional DOS engedélyezése"},
/* it */ {IDCLS_ENABLE_PROFDOS_IT, "Attiva Professional DOS"},
/* nl */ {IDCLS_ENABLE_PROFDOS_NL, "Activeer Professional DOS"},
/* pl */ {IDCLS_ENABLE_PROFDOS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_PROFDOS_SV, "Aktivera Professional DOS"},
/* tr */ {IDCLS_ENABLE_PROFDOS_TR, "Professional DOS'u aktif et"},
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_DISABLE_PROFDOS,    N_("Disable Professional DOS")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_PROFDOS_DA, "Deaktiver Professional DOS"},
/* de */ {IDCLS_DISABLE_PROFDOS_DE, "Professional DOS deaktivieren"},
/* fr */ {IDCLS_DISABLE_PROFDOS_FR, "Désactiver le DOS Professionnel"},
/* hu */ {IDCLS_DISABLE_PROFDOS_HU, "Professional DOS tiltása"},
/* it */ {IDCLS_DISABLE_PROFDOS_IT, "Disattiva Professional DOS"},
/* nl */ {IDCLS_DISABLE_PROFDOS_NL, "Professional DOS uitschakelen"},
/* pl */ {IDCLS_DISABLE_PROFDOS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_PROFDOS_SV, "Inaktivera Professional DOS"},
/* tr */ {IDCLS_DISABLE_PROFDOS_TR, "Professional DOS'u pasifleþtir"},
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME,    N_("Specify name of Professional DOS 1571 ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_DA, "Angiv navn på Professional DOS 1571-ROM-image"},
/* de */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_DE, "Name von Professional DOS 1571 Datei definieren"},
/* fr */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_HU, "Adja meg a Professional DOS 1571 ROM képmást"},
/* it */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del Professional DOS 1571"},
/* nl */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_NL, "Geef de naam van het Professional DOS 1571 ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_SV, "Ange namn på Professional DOS 1571-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_TR, "Professional DOS 1571 ROM imaj ismini belirt"},
#endif

/* drive/iec/plus4exp/plus4exp-cmdline-options.c */
/* en */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE,    N_("Set parallel cable type (0: none, 1: standard)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_DA, "Vælg typen af parallelkabel parallellkabel (0: ingen, 1: standard)"},
/* de */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_DE, "Parallelkabel Typ (0: kein, 1: Standard)"},
/* fr */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_FR, "Définir le type de câble parallèle (0: aucun, 1: standard)"},
/* hu */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_HU, "Adja meg a párhuzamos kábel típusát (0: nincs, 1: standard)"},
/* it */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_IT, "Imposta il tipo di cavo parallelo (0: nessuno, 1: standard)"},
/* nl */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_NL, "Zet parallelle-kabelsoort (0: geen, 1: standaard)"},
/* pl */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_SV, "Välj typ av parallellkabel (0: ingen, 1: standard)"},
/* tr */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_TR, "Paralel kablo tipini ayarlayýn (0: yok, 1: standart)"},
#endif

/* drive/iec128dcr/iec128dcr-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME,    N_("Specify name of 1571CR DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_DA, "Angiv navn på 1571CR-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_DE, "Name der 1571CR DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1571CR DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_HU, "Adja meg az 1571CR DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1571CR"},
/* nl */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_NL, "Geef de naam van het 1571CR DOS ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_SV, "Ange namn på 1571CR-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_TR, "1571CR DOS ROM imaj ismini belirt"},
#endif

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME,    N_("Specify name of 2031 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_DA, "Angiv navn på 2031-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_DE, "Name der 2031 DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 2031 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_HU, "Adja meg a 2031 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 2031"},
/* nl */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_NL, "Geef de naam van het 2031 DOS ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_SV, "Ange namn på 2031-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_TR, "2031 DOS ROM imaj ismini belirt"},
#endif

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME,    N_("Specify name of 2040 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_DA, "Angiv navn på 2040-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_DE, "Name der 2040 DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 2040 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_HU, "Adja meg a 2040 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 2040"},
/* nl */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_NL, "Geef de naam van het 2040 DOS ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_SV, "Ange namn på 2040-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_TR, "2040 DOS ROM imaj ismini belirt"},
#endif

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME,    N_("Specify name of 3040 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_DA, "Angiv navn på 3040-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_DE, "Name der 3040 DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 3040 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_HU, "Adja meg a 3040 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 3040"},
/* nl */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_NL, "Geef de naam van het 3040 DOS ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_SV, "Ange namn på 3040-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_TR, "3040 DOS ROM imaj ismini belirt"},
#endif

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME,    N_("Specify name of 4040 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_DA, "Angiv navn på 4040-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_DE, "Name der 4040 DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 4040 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_HU, "Adja meg a 4040 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 4040"},
/* nl */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_NL, "Geef de naam van het 4040 DOS ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_SV, "Ange namn på 4040-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_TR, "4040 DOS ROM imaj ismini belirt"},
#endif

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME,    N_("Specify name of 1001/8050/8250 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_DA, "Angiv navn på 1001/8050/8250-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_DE, "Name der 1001/8040/8205 DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1001/8050/8250 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_HU, "Adja meg az 1001/8050/8250 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1001/8050/8250"},
/* nl */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_NL, "Geef de naam van het 1001/8050/8250 DOS ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_SV, "Ange namn på 1001/8050/8250-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_TR, "1001/8050/8250 DOS ROM imaj ismini belirt"},
#endif

/* drive/tcbm/tcbm-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME,    N_("Specify name of 1551 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_DA, "Angiv navn på 1551-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_DE, "Name der 4040 DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1551 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_HU, "Adja meg az 1551 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1551"},
/* nl */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_NL, "Geef de naam van het 1551 DOS ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_SV, "Ange namn på 1551-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_TR, "1551 DOS ROM imaj ismini belirt"},
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8,    N_("Use <name> as directory for file system device #8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_DA, "Brug <navn> som katalog for filsystembaseret drev #8"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_DE, "Benutze <Name> für Verzeichnis beim Verzeichniszugriff für Gerät #8"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_FR, "Utiliser le répertoire <nom> comme système de fichiers pour le lecteur "
                                                "#8"},
/* hu */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_HU, "<név> könyvtár használata az #8-as egység fájlrendszerének"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_IT, "Una <nome> come direcory per il file system della periferica #8"},
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_NL, "Gebruik <naam> als directory voor bestandssysteemapparaat #8"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_SV, "Ange <namn> som katalog för filsystemsbaserad enhet 8"},
/* tr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_PL, "<isim>'i aygýt #8'in dosya sistemi dizini olarak kullan"},
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9,    N_("Use <name> as directory for file system device #9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_DA, "Brug <navn> som katalog for filsystembaseret drev #9"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_DE, "Benutze <Name> für Verzeichnis beim Verzeichniszugriff für Gerät #9"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_FR, "Utiliser le répertoire <nom> comme système de fichiers pour le lecteur "
                                                "#9"},
/* hu */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_HU, "<név> könyvtár használata az #9-es egység fájlrendszerének"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_IT, "Una <nome> come direcory per il file system della periferica #9"},
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_NL, "Gebruik <naam> als directory voor bestandssysteemapparaat #9"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_SV, "Ange <namn> som katalog för filsystemsbaserad enhet 9"},
/* tr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_TR, "<isim>'i aygýt #9'un dosya sistemi dizini olarak kullan"},
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10,    N_("Use <name> as directory for file system device #10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_DA, "Brug <navn> som katalog for filsystembaseret drev #10"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_DE, "Benutze <Name> für Verzeichnis beim Verzeichniszugriff für Gerät #10"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_FR, "Utiliser le répertoire <nom> comme système de fichiers pour le lecteur "
                                                 "#10"},
/* hu */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_HU, "<név> könyvtár használata az #10-es egység fájlrendszerének"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_IT, "Una <nome> come direcory per il file system della periferica #10"},
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_NL, "Gebruik <naam> als directory voor bestandssysteemapparaat #10"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_SV, "Ange <namn> som katalog för filsystemsbaserad enhet 10"},
/* tr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_TR, "<isim>'i aygýt #10'un dosya sistemi dizini olarak kullan"},
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11,    N_("Use <name> as directory for file system device #11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_DA, "Brug <navn> som katalog for filsystembaseret drev #11"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_DE, "Benutze <Name> für Verzeichnis beim Verzeichniszugriff für Gerät #11"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_FR, "Utiliser le répertoire <nom> comme système de fichiers pour le lecteur "
                                                 "#11"},
/* hu */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_HU, "<név> könyvtár használata az #11-es egység fájlrendszerének"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_IT, "Una <nome> come direcory per il file system della periferica #11"},
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_NL, "Gebruik <naam> als directory voor bestandssysteemapparaat #11"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_SV, "Ange <namn> som katalog för filsystemsbaserad enhet 11"},
/* tr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_TR, "<isim>'i aygýt #11'in dosya sistemi dizini olarak kullan"},
#endif

#ifdef HAVE_FFMPEG
/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDCLS_SET_AUDIO_STREAM_BITRATE,    N_("Set bitrate for audio stream in media file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_AUDIO_STREAM_BITRATE_DA, "Angiv bitrate for lydstrøm i mediefil"},
/* de */ {IDCLS_SET_AUDIO_STREAM_BITRATE_DE, "Bitrate für Audio Stream für Media Datei setzen"},
/* fr */ {IDCLS_SET_AUDIO_STREAM_BITRATE_FR, "Régler le bitrate audio du fichier média"},
/* hu */ {IDCLS_SET_AUDIO_STREAM_BITRATE_HU, "Adja meg az audió bitrátát a média fájlhoz"},
/* it */ {IDCLS_SET_AUDIO_STREAM_BITRATE_IT, "Imposta il bitrate del flusso audio nel file multimediale"},
/* nl */ {IDCLS_SET_AUDIO_STREAM_BITRATE_NL, "Zet de bitrate voor het audiogedeelte van het mediabestand"},
/* pl */ {IDCLS_SET_AUDIO_STREAM_BITRATE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_AUDIO_STREAM_BITRATE_SV, "Ange bithastighet för ljudström i mediafil"},
/* tr */ {IDCLS_SET_AUDIO_STREAM_BITRATE_TR, "Ses akýþý için ortam dosyasýndaki bithýzý'ný ayarla"},
#endif

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDCLS_SET_VIDEO_STREAM_BITRATE,    N_("Set bitrate for video stream in media file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_VIDEO_STREAM_BITRATE_DA, "Angiv bitrate for videostrøm i mediefil"},
/* de */ {IDCLS_SET_VIDEO_STREAM_BITRATE_DE, "Bitrate für Video Stream für Media Datei setzen"},
/* fr */ {IDCLS_SET_VIDEO_STREAM_BITRATE_FR, "Régler le bitrate vidéo du fichier média"},
/* it */ {IDCLS_SET_VIDEO_STREAM_BITRATE_IT, "Imposta il bitrate per il flusso video nel file multimediale"},
/* hu */ {IDCLS_SET_VIDEO_STREAM_BITRATE_HU, "Adja meg az videó bitrátát a média fájlhoz"},
/* nl */ {IDCLS_SET_VIDEO_STREAM_BITRATE_NL, "Zet de bitrate voor het videogedeelte van het mediabestand"},
/* pl */ {IDCLS_SET_VIDEO_STREAM_BITRATE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VIDEO_STREAM_BITRATE_SV, "Ange bithastighet för videoström i mediafil"},
/* tr */ {IDCLS_SET_VIDEO_STREAM_BITRATE_TR, "Görüntü akýþý için ortam dosyasýndaki bithýzý'ný ayarla"},
#endif
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_PET_MODEL,    N_("Specify PET model to emulate")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PET_MODEL_DA, "Angiv emuleret PET-model"},
/* de */ {IDCLS_SPECIFY_PET_MODEL_DE, "PET Modell spezifizieren"},
/* fr */ {IDCLS_SPECIFY_PET_MODEL_FR, "Spécifier le modèle PET à émuler"},
/* hu */ {IDCLS_SPECIFY_PET_MODEL_HU, "Adja meg az emulált PET modellt"},
/* it */ {IDCLS_SPECIFY_PET_MODEL_IT, "Specifica il modello di PET da emulare"},
/* nl */ {IDCLS_SPECIFY_PET_MODEL_NL, "Geef PET-model om te emuleren"},
/* pl */ {IDCLS_SPECIFY_PET_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PET_MODEL_SV, "Ange PET-modell att emulera"},
/* tr */ {IDCLS_SPECIFY_PET_MODEL_TR, "Emüle edilecek PET modelini belirt"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_EDITOR_ROM_NAME,    N_("Specify name of Editor ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_DA, "Angiv navn på Editor-ROM-image"},
/* de */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_DE, "Name für Editor ROM Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_FR, "Spécifier le nom de l’image ROM Editor"},
/* hu */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_HU, "Adja meg a szerkesztõ ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_IT, "Specifica il nome dell'immagine della Editor ROM"},
/* nl */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_NL, "Geef naam van het Editor ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_SV, "Ange namn på Editor-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_TR, "Editor ROM imajýnýn ismini belirt"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME,    N_("Specify 4K extension ROM name at $9***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_DA, "Angiv navn på 4K-udviddelses-ROM på $9***"},
/* de */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_DE, "4K Erweiterungs ROM Datei Name bei $9*** definieren"},
/* fr */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_FR, "Spécifier le nom de l'extension ROM 4K à $9***"},
/* hu */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_HU, "Adja meg a $9*** címû 4K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_IT, "Specifica il nome della ROM di estensione di 4k a $9***"},
/* nl */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_NL, "Geef naam van het 4K uitbreiding ROM-bestand op adres $9***"},
/* pl */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_SV, "Ange namn på 4K-utöknings-ROM på $9***"},
/* tr */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_TR, "$9*** adresindeki 4K uzantýsý ROM ismini belirt"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME,    N_("Specify 4K extension ROM name at $A***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_DA, "Angiv navn på 4K-udviddelses-ROM på $A***"},
/* de */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_DE, "4K Erweiterungs ROM Datei Name bei $A*** definieren"},
/* fr */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_FR, "Spécifier le nom de l'extension ROM 4K à $A***"},
/* hu */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_HU, "Adja meg a $A*** címû 4K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_IT, "Specifica il nome della ROM di estensione di 4k a $A***"},
/* nl */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_NL, "Geef naam van het 4K uitbreiding ROM-bestand op adres $A***"},
/* pl */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_SV, "Ange namn på 4K-utöknings-ROM på $A***"},
/* tr */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_TR, "$A*** adresindeki 4K uzantýsý ROM ismini belirt"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME,    N_("Specify 4K extension ROM name at $B***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_DA, "Angiv navn på 4K-udviddelses-ROM på $B***"},
/* de */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_DE, "4K Erweiterungs ROM Datei Name bei $B*** definieren"},
/* fr */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_FR, "Spécifier le nom de l'extension ROM 4K à $B***"},
/* hu */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_HU, "Adja meg a $B*** címû 4K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_IT, "Specifica il nome della ROM di estensione di 4k a $B***"},
/* nl */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_NL, "Geef naam van het 4K uitbreiding ROM-bestand op adres $B***"},
/* pl */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_SV, "Ange namn på 4K-utöknings-ROM på $B***"},
/* tr */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_TR, "$B*** adresindeki 4K uzantýsý ROM ismini belirt"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000,    N_("Enable PET8296 4K RAM mapping at $9***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_DA, "Aktivér PET8296 4K-RAM-mapping på $9***"},
/* de */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_DE, "PET8296 4K RAM Zuordnung bei $9*** aktivieren"},
/* fr */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_FR, "Activer l'extension PET8296 4K à $9***"},
/* hu */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_HU, "PET8296 4K RAM engedélyezése a $9*** címen"},
/* it */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_IT, "Attiva il mapping della RAM di 4K del PET8296 a $9***"},
/* nl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_NL, "Activeer PET8296 4K RAM op adres $9***"},
/* pl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_SV, "Aktivera PET8296 4K-RAM-mappning på $9***"},
/* tr */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_TR, "$9*** adresindeki PET8296 4K RAM eþleþtirmesini aktif et"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000,    N_("Disable PET8296 4K RAM mapping at $9***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_DA, "Deaktiver PET8296 4K-RAM-mapping på $9***"},
/* de */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_DE, "PET8296 4K RAM Zuordnung bei $9*** deaktivieren"},
/* fr */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_FR, "Désactiver l'extension PET8296 4K à $9***"},
/* hu */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_HU, "PET8296 4K RAM tiltása a $9*** címen"},
/* it */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_IT, "Disattiva il mapping della RAM di 4K del PET8296 a $9***"},
/* nl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_NL, "PET8296 4K RAM op adres $9*** uitschakelen"},
/* pl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_SV, "Inaktivera PET8296 4K-RAM-mappning på $9***"},
/* tr */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_TR, "$9*** adresindeki PET8296 4K RAM eþleþtirmesini pasifleþtir"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000,    N_("Enable PET8296 4K RAM mapping at $A***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_DA, "Aktivér PET8296 4K-RAM-mapping på $A***"},
/* de */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_DE, "PET8296 4K RAM Zuordnung bei $A*** aktivieren"},
/* fr */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_FR, "Activer l'extension PET8296 4K à $A***"},
/* hu */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_HU, "PET8296 4K RAM engedélyezése a $A*** címen"},
/* it */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_IT, "Attiva il mapping della RAM di 4K del PET8296 a $A***"},
/* nl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_NL, "Activeer PET8296 4K RAM op adres $A***"},
/* pl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_SV, "Aktivera PET8296 4K-RAM-mappning på $A***"},
/* tr */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_TR, "$A*** adresindeki PET8296 4K RAM eþleþtirmesini aktif et"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000,    N_("Disable PET8296 4K RAM mapping at $A***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_DA, "Deaktiver PET8296 4K-RAM-mapping på $A***"},
/* de */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_DE, "PET8296 4K RAM Zuordnung bei $A*** deaktivieren"},
/* fr */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_FR, "Désactiver l'extension PET8296 4K à $A***"},
/* hu */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_HU, "PET8296 4K RAM tiltása a $A*** címen"},
/* it */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_IT, "Disattiva il mapping della RAM di 4K del PET8296 a $9***"},
/* nl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_NL, "PET8296 4K RAM op adres $A*** uitschakelen"},
/* pl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_SV, "Inaktivera PET8296 4K-RAM-mappning på $A***"},
/* tr */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_TR, "$A*** adresindeki PET8296 4K RAM eþleþtirmesini pasifleþtir"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SUPERPET_IO,    N_("Enable SuperPET I/O")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SUPERPET_IO_DA, "Aktivér SuperPET-I/O"},
/* de */ {IDCLS_ENABLE_SUPERPET_IO_DE, "SuperPET I/O aktivieren"},
/* fr */ {IDCLS_ENABLE_SUPERPET_IO_FR, "Activer les E/S SuperPET"},
/* hu */ {IDCLS_ENABLE_SUPERPET_IO_HU, "SuperPET I/O engedélyezése"},
/* it */ {IDCLS_ENABLE_SUPERPET_IO_IT, "Attiva l'I/O del SuperPET"},
/* nl */ {IDCLS_ENABLE_SUPERPET_IO_NL, "Activeer SuperPET-I/O"},
/* pl */ {IDCLS_ENABLE_SUPERPET_IO_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SUPERPET_IO_SV, "Aktivera SuperPET-I/O"},
/* tr */ {IDCLS_ENABLE_SUPERPET_IO_TR, "SuperPET G/Ç'ý aktif et"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SUPERPET_IO,    N_("Disable SuperPET I/O")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SUPERPET_IO_DA, "Deaktiver SuperPET-I/O"},
/* de */ {IDCLS_DISABLE_SUPERPET_IO_DE, "SuperPET I/O deaktivieren"},
/* fr */ {IDCLS_DISABLE_SUPERPET_IO_FR, "Désactiver les E/S SuperPET"},
/* hu */ {IDCLS_DISABLE_SUPERPET_IO_HU, "SuperPET I/O tiltása"},
/* it */ {IDCLS_DISABLE_SUPERPET_IO_IT, "Disattiva l'I/O del SuperPET"},
/* nl */ {IDCLS_DISABLE_SUPERPET_IO_NL, "SuperPET-I/O uitschakelen"},
/* pl */ {IDCLS_DISABLE_SUPERPET_IO_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SUPERPET_IO_SV, "Inaktivera SuperPET-I/O"},
/* tr */ {IDCLS_DISABLE_SUPERPET_IO_TR, "SuperPET G/Ç'ý pasifleþtir"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES,    N_("Enable ROM 1 Kernal patches")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_DA, "Aktivér ROM 1 kerneændringer"},
/* de */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_DE, "ROM 1 Kernal patches aktivieren"},
/* fr */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_FR, "Activer les patches ROM 1 Kernal"},
/* hu */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_HU, "ROM 1 Kernal foltozás engedélyezése"},
/* it */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_IT, "Attiva le patch del Kernal della ROM 1"},
/* nl */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_NL, "Activeer ROM 1 Kernalverbeteringen"},
/* pl */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_SV, "Aktivera ROM 1-Kernalpatchar"},
/* tr */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_TR, "ROM 1 Kernal yamalarýný aktif et"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES,    N_("Disable ROM 1 Kernal patches")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_DA, "Deaktiver ROM 1 kerneændringer"},
/* de */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_DE, "ROM 1 Kernal patches deaktivieren"},
/* fr */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_FR, "Désactiver les patches ROM 1 Kernal"},
/* hu */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_HU, "ROM 1 Kernal foltozás tiltása"},
/* it */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_IT, "Disattiva le patch del Kernal della ROM 1"},
/* nl */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_NL, "ROM 1 Kernalverbeteringen uitschakelen"},
/* pl */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_SV, "Inaktivera ROM 1-Kernalpatchar"},
/* tr */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_TR, "ROM 1 Kernal yamalarýný pasifleþtir"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET,    N_("Switch upper/lower case charset")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_DA, "Skift mellem store og små bogstaver"},
/* de */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_DE, "Wechsel Groß-/Kleinschreibung Zeichensatz"},
/* fr */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_FR, "Inverser le charset majuscule et minuscule"},
/* hu */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_HU, "Váltás nagy-/kisbetûs karakterkészletre"},
/* it */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_IT, "Scambia il set di caratteri maiuscolo/minuscolo"},
/* nl */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_NL, "Schakel tussen grote/kleine letters"},
/* pl */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_SV, "Växla små/stora bokstäver i teckengeneratorn"},
/* tr */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_TR, "Büyük/küçük harf karakter seti deðiþtir"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET,    N_("Do not switch upper/lower case charset")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_DA, "Skift ikke mellem store og små bogstaver"},
/* de */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_DE, "Kein Wechsel Groß-/Kleinschreibung Zeichensatz"},
/* fr */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_FR, "Ne pas inverser le charset majuscule et minuscule"},
/* hu */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_HU, "Ne váltson a nagy-/kisbetûs karakterkészletre"},
/* it */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_IT, "Non scambiare il set di caratteri maiuscolo/minuscolo"},
/* nl */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_NL, "Schakel niet tussen grote/kleine letters"},
/* pl */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_SV, "Växla inte små/stora bokstäver i teckengeneratorn"},
/* tr */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_TR, "Büyük/küçük harf karakter seti deðiþtirme"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_EOI_BLANKS_SCREEN,    N_("EOI blanks screen")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_EOI_BLANKS_SCREEN_DA, "EOI sletter skærmen"},
/* de */ {IDCLS_EOI_BLANKS_SCREEN_DE, "EOI löscht Bildschirm"},
/* fr */ {IDCLS_EOI_BLANKS_SCREEN_FR, "EOI vide l'écran"},
/* hu */ {IDCLS_EOI_BLANKS_SCREEN_HU, "EOI törli a képernyõt"},
/* it */ {IDCLS_EOI_BLANKS_SCREEN_IT, "EOI oscura lo schermo"},
/* nl */ {IDCLS_EOI_BLANKS_SCREEN_NL, "EOI maakt het scherm leeg"},
/* pl */ {IDCLS_EOI_BLANKS_SCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_EOI_BLANKS_SCREEN_SV, "EOI tömmer skärmen"},
/* tr */ {IDCLS_EOI_BLANKS_SCREEN_TR, "EOI ekraný boþaltýr"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN,    N_("EOI does not blank screen")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_DA, "EOI sletter ikke skærmen"},
/* de */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_DE, "EIO löscht Bildschirm nicht"},
/* fr */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_FR, "EOI ne vide pas l'écran"},
/* hu */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_HU, "EOI nem törli a képernyõt"},
/* it */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_IT, "EOI non oscura lo schermo"},
/* nl */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_NL, "EOI maakt niet het scherm leeg"},
/* pl */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_SV, "EOI tömmer inte skärmen"},
/* tr */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_TR, "EOI ekraný boþaltmaz"},
#endif

/* pet/petpia1.c */
/* en */ {IDCLS_ENABLE_USERPORT_DIAG_PIN,    N_("Enable userport diagnostic pin")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_DA, "Aktivér diagnostik på brugerporten"},
/* de */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_DE, "Userport Diagnose Pin aktivieren"},
/* fr */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_FR, "Activer la pin de diagnostique userport"},
/* hu */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_HU, "Userport diagnosztikai láb engedélyezése"},
/* it */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_IT, "Attiva il pin diagnostico della userport"},
/* nl */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_NL, "Activeer userport diagnostische pin"},
/* pl */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_SV, "Aktivera diagnostik på användarporten"},
/* tr */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_TR, "Userport diyagnostik pinini aktif et"},
#endif

/* pet/petpia1.c */
/* en */ {IDCLS_DISABLE_USERPORT_DIAG_PIN,    N_("Disable userport diagnostic pin")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_DA, "Deaktiver diagnostik på brugerporten"},
/* de */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_DE, "Userport Diagnose Pin deaktivieren"},
/* fr */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_FR, "Désactiver la pin de diagnostique userport"},
/* hu */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_HU, "Userport diagnosztikai láb tiltása"},
/* it */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_IT, "Disattiva il pin diagnostico della userport"},
/* nl */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_NL, "Userport diagnostische pin uitschakelen"},
/* pl */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_SV, "Inaktivera diagnostik på användarporten"},
/* tr */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_TR, "Userport diyagnostik pinini pasifleþtir"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME,    N_("Specify name of Function low ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_DA, "Angiv navn på image for lav funktions-ROM"},
/* de */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_DE, "Namen für Funktions ROM Datei (unterer Bereich) definieren"},
/* fr */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_FR, "Spécifier le nom de l'image basse ROM Fonction"},
/* hu */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_HU, "Adja meg az alsó Function ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_IT, "Specifica il nome dell'immagine della Function ROM bassa"},
/* nl */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_NL, "Geef de naam van het Function low ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_SV, "Ange namn på ROM-avbildning för lågt funktions-ROM"},
/* tr */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_TR, "Function low ROM imajýnýn ismini belirt"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME,    N_("Specify name of Function high ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_DA, "Angiv navn på image for høj funktions-ROM"},
/* de */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_DE, "Namen für Funktions ROM Datei (oberer Bereich) definieren"},
/* fr */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_FR, "Spécifier le nom de l'image haute ROM Fonction"},
/* hu */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_HU, "Adja meg a felsõ Function ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_IT, "Specifica il nome dell'immagine della Function ROM alta"},
/* nl */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_NL, "Geef de naam van het Function high ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_SV, "Ange namn på ROM-avbildning för högt funktions-ROM"},
/* tr */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_TR, "Function high ROM imajýnýn ismini belirt"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME,    N_("Specify name of Cartridge 1 low ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_DA, "Angiv navn på image for lav cartridge 1 ROM"},
/* de */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_DE, "Name für Erweiterungsmodul 1 ROM Datei (unterer Bereich) definieren"},
/* fr */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_FR, "Spécifier le nom de l'image basse ROM Cartouche 1"},
/* hu */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_HU, "Adja meg az alsó Cartridge 1 ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM Cartridge 1 bassa"},
/* nl */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_NL, "Geef de naam van het Cartridge 1 low ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_SV, "Ange namn på ROM-avbildning för lågt insticksmodul 1-ROM"},
/* tr */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_TR, "Cartridge 1 low ROM imajýnýn ismini belirt"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME,    N_("Specify name of Cartridge 1 high ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_DA, "Angiv navn på image for høj cartridge 1 ROM"},
/* de */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_DE, "Name für Erweiterungsmodul 1 ROM Datei (oberer Bereich) definieren"},
/* fr */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_FR, "Spécifier le nom de l'image haute ROM Cartouche 1"},
/* hu */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_HU, "Adja meg a felsõ Cartridge 1 ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_IT, "Specifica il nomer dell'immagine della ROM Cartridge 1 alta"},
/* nl */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_NL, "Geef de naam van het Cartridge 1 high ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_SV, "Ange namn på ROM-avbildning för högt insticksmodul 1-ROM"},
/* tr */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_TR, "Cartridge 1 high ROM imajýnýn ismini belirt"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME,    N_("Specify name of Cartridge 2 low ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_DA, "Angiv navn på image for lav cartridge 2 ROM"},
/* de */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_DE, "Name für Erweiterungsmodul 2 ROM Datei (unterer Bereich) definieren"},
/* fr */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_FR, "Spécifier le nom de l'image basse ROM Cartouche 2"},
/* hu */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_HU, "Adja meg az alsó Cartridge 2 ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM Cartridge 2 bassa"},
/* nl */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_NL, "Geef de naam van het Cartridge 2 low ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_SV, "Ange namn på ROM-avbildning för lågt insticksmodul 2-ROM"},
/* tr */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_TR, "Cartridge 2 low ROM imajýnýn ismini belirt"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME,    N_("Specify name of Cartridge 2 high ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_DA, "Angiv navn på image for høj cartridge 1 ROM"},
/* de */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_DE, "Name für Erweiterungsmodul 2 ROM Datei (oberer Bereich) definieren"},
/* fr */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_FR, "Spécifier le nom de l'image haute ROM Cartouche 2"},
/* hu */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_HU, "Adja meg a felsõ Cartridge 2 ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_IT, "Specifica il nomer dell'immagine della ROM Cartridge 2 alta"},
/* nl */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_NL, "Geef de naam van het Cartridge 2 high ROM-bestand"},
/* pl */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_SV, "Ange namn på ROM-avbildning för högt insticksmodul 2-ROM"},
/* tr */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_TR, "Cartridge 2 high ROM imajýnýn ismini belirt"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_RAM_INSTALLED,    N_("Specify size of RAM installed in kb (16/32/64)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RAM_INSTALLED_DA, "Angiv størrelse på installeret RAM i kB (16/32/64)"},
/* de */ {IDCLS_SPECIFY_RAM_INSTALLED_DE, "Größe des emulierten RAM Speichers in kb (16/32/64) spezifizieren"},
/* fr */ {IDCLS_SPECIFY_RAM_INSTALLED_FR, "Spécifier la taille du RAM installé (16/32/64)"},
/* hu */ {IDCLS_SPECIFY_RAM_INSTALLED_HU, "Adja meg a telepített RAM méretét kb-ban (16/32/64)"},
/* it */ {IDCLS_SPECIFY_RAM_INSTALLED_IT, "Specifica la dimensione della RAM installata in kb (16/32/64)"},
/* nl */ {IDCLS_SPECIFY_RAM_INSTALLED_NL, "Geef de hoeveelheid RAM in kb (16/32/64)"},
/* pl */ {IDCLS_SPECIFY_RAM_INSTALLED_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RAM_INSTALLED_SV, "Ange storlek på installerat RAM i kb (16/32/64)"},
/* tr */ {IDCLS_SPECIFY_RAM_INSTALLED_TR, "Kurulmuþ olan RAM boyutunu kb cinsinden belirt (16/32/64)"},
#endif

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_SATURATION,    N_("Set saturation of internal calculated palette [1000]")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_SATURATION_DA, "Indstil mætning af internt beregnet palette [1000]"},
/* de */ {IDCLS_SET_SATURATION_DE, "Sättigung für intern berechneten Palette setzen [1000]"},
/* fr */ {IDCLS_SET_SATURATION_FR, "Régler la saturation de la palette interne calculée [1000]"},
/* hu */ {IDCLS_SET_SATURATION_HU, "A számolt paletta telítettsége [1000]"},
/* it */ {IDCLS_SET_SATURATION_IT, "Imposta la saturazione della palette calcolata internamente [1000]"},
/* nl */ {IDCLS_SET_SATURATION_NL, "Zet de verzadiging van het intern berekend kleurenpalet [1000]"},
/* pl */ {IDCLS_SET_SATURATION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SATURATION_SV, "Ställ in mättnad på internt beräknad palett [1000]"},
/* tr */ {IDCLS_SET_SATURATION_TR, "Dahili hesaplanmýþ paletin doygunluðunu ayarlayýn [1000]"},
#endif

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_CONTRAST,    N_("Set contrast of internal calculated palette [1100]")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_CONTRAST_DA, "Indstil kontrast for internt beregnet palette [1100]"},
/* de */ {IDCLS_SET_CONTRAST_DE, "Kontrast für intern berechneten Palette setzen [1100]"},
/* fr */ {IDCLS_SET_CONTRAST_FR, "Régler le contraste de la palette interne calculée [1100]"},
/* hu */ {IDCLS_SET_CONTRAST_HU, "A számolt paletta kontraszt értéke [1100]"},
/* it */ {IDCLS_SET_CONTRAST_IT, "Imposta il constrasto della palette calcolata internamente [1100]"},
/* nl */ {IDCLS_SET_CONTRAST_NL, "Zet het contrast van het intern berekend kleurenpalet [1100]"},
/* pl */ {IDCLS_SET_CONTRAST_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_CONTRAST_SV, "Ställ in kontrast på internt beräknad palett [1100]"},
/* tr */ {IDCLS_SET_CONTRAST_TR, "Dahili hesaplanmýþ paletin karþýtlýðýný ayarlayýn [1100]"},
#endif

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_BRIGHTNESS,    N_("Set brightness of internal calculated palette [1100]")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_BRIGHTNESS_DA, "Indstil lysstyrke for internt beregnet palette [1100]"},
/* de */ {IDCLS_SET_BRIGHTNESS_DE, "Helliogkeit für intern berechneten Palette setzen [1100]"},
/* fr */ {IDCLS_SET_BRIGHTNESS_FR, "Régler la luminosité de la palette interne calculée [1100]"},
/* hu */ {IDCLS_SET_BRIGHTNESS_HU, "A számolt paletta fényerõssége [1100]"},
/* it */ {IDCLS_SET_BRIGHTNESS_IT, "Imposta la luminanza della palette calcolata internamente [1100]"},
/* nl */ {IDCLS_SET_BRIGHTNESS_NL, "Zet de helderheid van het intern berekend kleurenpalet [1100]"},
/* pl */ {IDCLS_SET_BRIGHTNESS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_BRIGHTNESS_SV, "Ställ in ljusstyrka på internt beräknad palett [1100]"},
/* tr */ {IDCLS_SET_BRIGHTNESS_TR, "Dahili hesaplanmýþ paletin parlaklýðýný ayarlayýn [1100]"},
#endif

/* plus4/ted-cmdline-options.c`, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_GAMMA,    N_("Set gamma of internal calculated palette [900]")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_GAMMA_DA, "Indstil gamma for internt beregnet palette [900]"},
/* de */ {IDCLS_SET_GAMMA_DE, "Gamma für intern berechneten Palette setzen [900]"},
/* fr */ {IDCLS_SET_GAMMA_FR, "Régler le gamma de la palette interne calculée [900]"},
/* hu */ {IDCLS_SET_GAMMA_HU, "A számolt paletta gammája [900]"},
/* it */ {IDCLS_SET_GAMMA_IT, "Imposta la gamma della palette calcolata internamente [900]"},
/* nl */ {IDCLS_SET_GAMMA_NL, "Zet de gamma van het intern berekend kleurenpalet [900]"},
/* pl */ {IDCLS_SET_GAMMA_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_GAMMA_SV, "Ställ in gamma på internt beräknad palett [900]"},
/* tr */ {IDCLS_SET_GAMMA_TR, "Dahili hesaplanmýþ paletin gamasýný ayarlayýn [900]"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_TINT,    N_("Set tint of internal calculated palette [1000]")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_TINT_DA, "Indstil farvetone for internt beregnet palette [1000]"},
/* de */ {IDCLS_SET_TINT_DE, "Farbton für intern berechneten Palette setzen [1000]"},
/* fr */ {IDCLS_SET_TINT_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_TINT_HU, "A számolt, belsõ paletta telítettsége [1000]"},
/* it */ {IDCLS_SET_TINT_IT, "Imposta la sfumatura della palette calcolata internamente [1000]"},
/* nl */ {IDCLS_SET_TINT_NL, "Zet de tint van het intern berekend kleuren palette [1000]"},
/* pl */ {IDCLS_SET_TINT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_TINT_SV, "Ställ in ton på internt beräknad palett [1000]"},
/* tr */ {IDCLS_SET_TINT_TR, "Dahili hesaplanmýþ paletin tonunu ayarlayýn [1000]"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_ODDLINES_PHASE,    N_("Set phase for color carrier in odd lines [1250]")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_ODDLINES_PHASE_DA, "Vælg fase for farve-bærebølge på ulige scanlinjer [1250]"},
/* de */ {IDCLS_SET_ODDLINES_PHASE_DE, "Phase für Farbträger in ungeraden Zeilen setzen [1250]"},
/* fr */ {IDCLS_SET_ODDLINES_PHASE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_ODDLINES_PHASE_HU, "Szín vivõ fázisa a páratlan sorokban [1250]"},
/* it */ {IDCLS_SET_ODDLINES_PHASE_IT, "Imposta la fase per la portante colore nelle linee dispari [1250]"},
/* nl */ {IDCLS_SET_ODDLINES_PHASE_NL, "Zet de fase van de kleuren drager in oneven lijnen [1250]"},
/* pl */ {IDCLS_SET_ODDLINES_PHASE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_ODDLINES_PHASE_SV, "Välj fas för färgbärarvåg på udda rader [1250]"},
/* tr */ {IDCLS_SET_ODDLINES_PHASE_TR, "Tek satýrlardaki renk taþýyýcýlarý için fazý ayarlayýn [1250]"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_ODDLINES_OFFSET,    N_("Set phase offset for color carrier in odd lines [750]")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_ODDLINES_OFFSET_DA, "Vælg fase-offset for farve-bærebølge på ulige scanlinjer [750]"},
/* de */ {IDCLS_SET_ODDLINES_OFFSET_DE, "Phasenoffset für Farbträger in ungeraden Zeilen setzen [750]"},
/* fr */ {IDCLS_SET_ODDLINES_OFFSET_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_ODDLINES_OFFSET_HU, "Szín vivõ fázis offszet a páratlan sorokban [750]"},
/* it */ {IDCLS_SET_ODDLINES_OFFSET_IT, "Imposta l'offset della fase per la portate colore nelle linee dispari [750]"},
/* nl */ {IDCLS_SET_ODDLINES_OFFSET_NL, "Zet de fase compensatie van de kleuren drager in oneven lijnen [750]"},
/* pl */ {IDCLS_SET_ODDLINES_OFFSET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_ODDLINES_OFFSET_SV, "Välj fasutjämning för färgbärarvåg på udda rader [1250]"},
/* tr */ {IDCLS_SET_ODDLINES_OFFSET_TR, "Tek satýrlardaki renk taþýyýcýlarý için faz ofsetini ayarlayýn [750]"},
#endif

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME,    N_("Specify name of printer driver for device #4")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_DA, "Angiv navn på printer-driver for enhed #4"},
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_DE, "Name für Druckertreiber Gerät #4 definieren"},
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_FR, "Spécifier le nom du pilote imprimante pour le périphérique #4"},
/* hu */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_HU, "Adja meg a #4-es nyomtatómeghajtó nevét"},
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_IT, "Specifica il nome del driver di stampa per la periferica #4"},
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_NL, "Geef de naam van het stuurprogramma voor apparaat #4"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_SV, "Ange namn på skrivardrivrutin för enhet 4"},
/* tr */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_TR, "Aygýt #4 için yazýcý sürücüsü ismini belirt"},
#endif

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME,    N_("Specify name of printer driver for device #5")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_DA, "Angiv navn på printer-driver for enhed #5"},
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_DE, "Name für Druckertreiber Gerät #5 definieren"},
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_FR, "Spécifier le nom du pilote imprimante pour le périphérique #5"},
/* hu */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_HU, "Adja meg a #5-ös nyomtatómeghajtó nevét"},
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_IT, "Specifica il nome del driver di stampa per la periferica #5"},
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_NL, "Geef de naam van het stuurprogramma voor apparaat #5"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_SV, "Ange namn på skrivardrivrutin för enhet 5"},
/* tr */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_DA, "Aygýt #5 için yazýcý sürücüsü ismini belirt"},
#endif

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME,    N_("Specify name of printer driver for the userport printer")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_DA, "Angiv navn på printerdriver for brugerport-printer"},
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_DE, "Name für Userport Druckertreiber definieren"},
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_FR, "Spécifier le nom du pilote imprimante pour l'imprimante userport"},
/* hu */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_HU, "Adja meg a userport nyomtatómeghajtó nevét"},
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_IT, "Specifica il nome del driver di stampa per la stampante su userport"},
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_NL, "Geef de naam van het stuurprogramma voor de userport printer"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_SV, "Ange namn på skrivardrivrutin för användarporten"},
/* tr */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_TR, "Userport yazýcýsý için yazýcý sürücüsü ismini belirt"},
#endif

/* printerdrv/interface-serial.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_4,    N_("Set device type for device #4 (0: NONE, 1: FS, 2: REAL)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DEVICE_TYPE_4_DA, "Sæt enhedstype for enhed #4 (0: INGEN, 1: FS, 2: ÆGTE)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_4_DE, "Geräte Typ für Gerät #4 (0: NONE, 1: FS, 2: REAL) setzen"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_4_FR, "Spécifier le type de périphérique pour #4 (0: AUCUN, 1: SF, 2: RÉEL)"},
/* hu */ {IDCLS_SET_DEVICE_TYPE_4_HU, "Adja meg a #4-es eszköz típusát (0:Nincs, 1: Fájlrendszer, 2: Valódi)"},
/* it */ {IDCLS_SET_DEVICE_TYPE_4_IT, "Specifica il tipo di periferica #4 (0 NESSUNA, 1 FS, 2 REALE)"},
/* nl */ {IDCLS_SET_DEVICE_TYPE_4_NL, "Zet apparaat soort voor apparaat #4 (0: GEEN, 1: FS, 2: ECHT)"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_4_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_4_SV, "Ange enhetstyp för enhet 4 (0: INGEN, 1: FS, 2: ÄKTA)"},
/* tr */ {IDCLS_SET_DEVICE_TYPE_4_TR, "Aygýt #4 için aygýt tipini ayarlayýn (0: YOK, 1: FS, 2: GERÇEK)"},
#endif

/* printerdrv/interface-serial.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_5,    N_("Set device type for device #5 (0: NONE, 1: FS, 2: REAL)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DEVICE_TYPE_5_DA, "Sæt enhedstype for enhed #5 (0: INGEN, 1: FS, 2: ÆGTE)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_5_DE, "Geräte Typ für Gerät #5 (0: NONE, 1: FS, 2: REAL) setzen"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_5_FR, "Spécifier le type de périphérique pour #5 (0: AUCUN, 1: SF, 2: RÉEL)"},
/* hu */ {IDCLS_SET_DEVICE_TYPE_5_HU, "Adja meg a #5-ös eszköz típusát (0:Nincs, 1: Fájlrendszer, 2: Valódi)"},
/* it */ {IDCLS_SET_DEVICE_TYPE_5_IT, "Specifica il tipo di periferica #4 (0 NESSUNA, 1 FS, 2 REALE)"},
/* nl */ {IDCLS_SET_DEVICE_TYPE_5_NL, "Zet apparaat soort voor apparaat #5 (0: GEEN, 1: FS, 2: ECHT)"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_5_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_5_SV, "Ange enhetstyp för enhet 5 (0: INGEN, 1: FS, 2: ÄKTA)"},
/* tr */ {IDCLS_SET_DEVICE_TYPE_5_TR, "Aygýt #5 için aygýt tipini ayarlayýn (0: YOK, 1: FS, 2: GERÇEK)"},
#endif

/* printerdrv/interface-userport.c */
/* en */ {IDCLS_ENABLE_USERPORT_PRINTER,    N_("Enable the userport printer emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_USERPORT_PRINTER_DA, "Aktivér emulering af printer på brugerporten"},
/* de */ {IDCLS_ENABLE_USERPORT_PRINTER_DE, "Userport Drucker Emulation aktivieren"},
/* fr */ {IDCLS_ENABLE_USERPORT_PRINTER_FR, "Acviver l'émulation de l'imprimante userport"},
/* hu */ {IDCLS_ENABLE_USERPORT_PRINTER_HU, "Userport nyomtató emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_USERPORT_PRINTER_IT, "Attiva l'emulazione della stampante su userport"},
/* nl */ {IDCLS_ENABLE_USERPORT_PRINTER_NL, "Activeer de userport printeremulatie"},
/* pl */ {IDCLS_ENABLE_USERPORT_PRINTER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_USERPORT_PRINTER_SV, "Aktivera emulering av skrivare på användarporten"},
/* tr */ {IDCLS_ENABLE_USERPORT_PRINTER_TR, "Userport yazýcý emülasyonunu aktif et"},
#endif

/* printerdrv/interface-userport.c */
/* en */ {IDCLS_DISABLE_USERPORT_PRINTER,    N_("Disable the userport printer emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_USERPORT_PRINTER_DA, "Deaktivér emulering af printer på brugerporten"},
/* de */ {IDCLS_DISABLE_USERPORT_PRINTER_DE, "Userport Drucker Emulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_USERPORT_PRINTER_FR, "Désactiver l'émulation de l'imprimante userport"},
/* hu */ {IDCLS_DISABLE_USERPORT_PRINTER_HU, "Userport nyomtató emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_USERPORT_PRINTER_IT, "Disattiva l'emulazione della stampante su userport"},
/* nl */ {IDCLS_DISABLE_USERPORT_PRINTER_NL, "De userport printeremulatie uitschakelen"},
/* pl */ {IDCLS_DISABLE_USERPORT_PRINTER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_USERPORT_PRINTER_SV, "Inaktivera emulering av skrivare på användarporten"},
/* tr */ {IDCLS_DISABLE_USERPORT_PRINTER_TR, "Userport yazýcý emülasyonunu pasifleþtir"},
#endif

/* printerdrv/output-select.c */
/* en */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME,    N_("Specify name of output device for device #4")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_DA, "Angiv navn på uddataenhed for enhed #4"},
/* de */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_DE, "Name für Druckerausgabe Datei Gerät #4 definieren"},
/* fr */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_FR, "Spécifiez le nom du périphérique de sortie pour le périphérique #4"},
/* hu */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_HU, "Adja meg a kimeneti eszköz nevét a #4-es egységhez"},
/* it */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_IT, "Specifica il nome del dispositivo di output per la periferica #4"},
/* nl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_NL, "Geef de naam van het uitvoer apparaat voor apparaat #4"},
/* pl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_SV, "Ange namn på utskriftsenhet för enhet 4"},
/* tr */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_TR, "Aygýt #4 için çýkýþ aygýtý ismini belirt"},
#endif

/* printerdrv/output-select.c */
/* en */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME,    N_("Specify name of output device for device #5")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_DA, "Angiv navn på uddataenhed for enhed #5"},
/* de */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_DE, "Name für Druckerausgabe Datei Gerät #5 definieren"},
/* fr */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_FR, "Spécifiez le nom du périphérique de sortie pour le périphérique #5"},
/* hu */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_HU, "Adja meg a kimeneti eszköz nevét a #5-ös egységhez"},
/* it */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_IT, "Specifica il nome del dispositivo di output per la periferica #5"},
/* nl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_NL, "Geef de naam van het uitvoer apparaat voor apparaat #5"},
/* pl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_SV, "Ange namn på utskriftsenhet för enhet 5"},
/* tr */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_TR, "Aygýt #5 için çýkýþ aygýtý ismini belirt"},
#endif

/* printerdrv/output-select.c */
/* en */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME,    N_("Specify name of output device for the userport printer")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_DA, "Angiv navn på uddataenhed for brugerports-printer"},
/* de */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_DE, "Name für Druckerausgabe Datei Userport definieren"},
/* fr */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_FR, "Spécifiez le nom du périphérique de sortie pour l'imprimante "
                                                   "userport"},
/* hu */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_HU, "Adja meg a kimeneti eszköz nevét a userport nyomtatóhoz"},
/* it */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_IT, "Specifica il nome del dispositivo di output per la stampante su "
                                                   "userport"},
/* nl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_NL, "Geef de naam van het uitvoer apparaat voor de userport printer"},
/* pl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_SV, "Ange namn på utskriftsenhet för användarporten"},
/* tr */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_TR, "Userport yazýcýsý için çýkýþ aygýtý ismini belirt"},
#endif

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME,    N_("Specify name of printer text device or dump file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_DA, "Angiv navn på printer-tekst-enhed eller dump-fil"},
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_DE, "Name für Druckerausgabe Textgerät oder Dump Datei definieren"},
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_FR, "Spécifier le nom de l'imprimante texte ou \"dump file\""},
/* hu */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_HU, "Adja meg a szövegnyomtató eszköz nevét, vagy a kimeneti fájlt"},
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_IT, "Specifica il nome del dispositivo testuale di stampa o salva su file"},
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_NL, "Geef de naam van het text apparaat of het dumpbestand"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_SV, "Ange namn på textenhet för utskrift eller dumpfil"},
/* tr */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_TR, "Metin aygýtý yazýcýsý ya da döküm dosyasýnýn ismini belirt"},
#endif

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_4,    N_("Specify printer text output device for IEC printer #4")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TEXT_DEVICE_4_DA, "Angiv printer tekst-uddata enhed for IEC-printer #4"},
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_4_DE, "IEC Drucker #4 Gerät für Textausgabe definieren"},
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_4_FR, "Spécifier le périphérique de sortie text pour l'imprimante IEC #4"},
/* hu */ {IDCLS_SPECIFY_TEXT_DEVICE_4_HU, "Adja meg a szövegnyomtató eszközt a #4-es IEC nyomtatóhoz"},
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_4_IT, "Specifica il nome del dispositivo di output testuale di stampa per la "
                                          "stampante IEC #4"},
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_4_NL, "Geef het text uitvoer apparaat voor IEC printer #4"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_4_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_4_SV, "Ange skrivartextutenhet för IEC-skrivare 4"},
/* tr */ {IDCLS_SPECIFY_TEXT_DEVICE_4_TR, "IEC yazýcýsý #4 için metin çýktý aygýtý yazýcýsý belirt"},
#endif

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_5,    N_("Specify printer text output device for IEC printer #5")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TEXT_DEVICE_5_DA, "Angiv printer tekst-uddata enhed for IEC-printer #5"},
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_5_DE, "IEC Drucker #5 Gerät für Textausgabe definieren"},
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_5_FR, "Spécifier le périphérique de sortie text pour l'imprimante IEC #5"},
/* hu */ {IDCLS_SPECIFY_TEXT_DEVICE_5_HU, "Adja meg a szövegnyomtató eszközt a #5-es IEC nyomtatóhoz"},
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_5_IT, "Specifica il nome del dispositivo di output testuale di stampa per la "
                                          "stampante IEC #5"},
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_5_NL, "Geef het text uitvoer apparaat voor IEC printer #5"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_5_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_5_SV, "Ange skrivartextutenhet för IEC-skrivare 5"},
/* tr */ {IDCLS_SPECIFY_TEXT_DEVICE_5_TR, "IEC yazýcýsý #5 için metin çýktý aygýtý yazýcýsý belirt"},
#endif

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_USERPORT,    N_("Specify printer text output device for userport printer")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TEXT_USERPORT_DA, "Angiv printer tekst-uddata enhed for brugerports-printer"},
/* de */ {IDCLS_SPECIFY_TEXT_USERPORT_DE, "IEC Userport Drucker Gerät für Textausgabe definieren"},
/* fr */ {IDCLS_SPECIFY_TEXT_USERPORT_FR, "Spécifier le périphérique de sortie text pour l'imprimante userport"},
/* hu */ {IDCLS_SPECIFY_TEXT_USERPORT_HU, "Adja meg a szövegnyomtató eszközt a userport nyomtatóhoz"},
/* it */ {IDCLS_SPECIFY_TEXT_USERPORT_IT, "Specifica il nome del dispositivo di output testuale di stampa per la "
                                          "stampante su userport"},
/* nl */ {IDCLS_SPECIFY_TEXT_USERPORT_NL, "Geef het text uitvoer apparaat voor de userport printer"},
/* pl */ {IDCLS_SPECIFY_TEXT_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_USERPORT_SV, "Ange skrivartextutenhet för användarportsskrivare"},
/* tr */ {IDCLS_SPECIFY_TEXT_USERPORT_TR, "Userport yazýcýsý için metin çýktý aygýtý yazýcýsý belirt"},
#endif

/* raster/raster-cmdline-options.c */
/* en */ {IDCLS_ENABLE_VIDEO_CACHE,    N_("Enable the video cache")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_VIDEO_CACHE_DA, "Aktivér videocache"},
/* de */ {IDCLS_ENABLE_VIDEO_CACHE_DE, "Video Cache aktivieren"},
/* fr */ {IDCLS_ENABLE_VIDEO_CACHE_FR, "Activer le cache vidéo"},
/* hu */ {IDCLS_ENABLE_VIDEO_CACHE_HU, "Videó gyorsítótár engedélyezése"},
/* it */ {IDCLS_ENABLE_VIDEO_CACHE_IT, "Attiva la cache video"},
/* nl */ {IDCLS_ENABLE_VIDEO_CACHE_NL, "Activeer de videocache"},
/* pl */ {IDCLS_ENABLE_VIDEO_CACHE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_VIDEO_CACHE_SV, "Aktivera videocache"},
/* tr */ {IDCLS_ENABLE_VIDEO_CACHE_TR, "Görüntü önbelleðini aktif et"},
#endif

/* raster/raster-cmdline-options.c */
/* en */ {IDCLS_DISABLE_VIDEO_CACHE,    N_("Disable the video cache")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_VIDEO_CACHE_DA, "Deaktiver videocache"},
/* de */ {IDCLS_DISABLE_VIDEO_CACHE_DE, "Video Cache deaktivieren"},
/* fr */ {IDCLS_DISABLE_VIDEO_CACHE_FR, "Désactiver le cache vidéo"},
/* hu */ {IDCLS_DISABLE_VIDEO_CACHE_HU, "Videó gyorsítótár tiltása"},
/* it */ {IDCLS_DISABLE_VIDEO_CACHE_IT, "Disattiva la cache video"},
/* nl */ {IDCLS_DISABLE_VIDEO_CACHE_NL, "De videocache uitschakelen"},
/* pl */ {IDCLS_DISABLE_VIDEO_CACHE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_VIDEO_CACHE_SV, "Inaktivera videocache"},
/* tr */ {IDCLS_DISABLE_VIDEO_CACHE_TR, "Görüntü önbelleðini pasifleþtir"},
#endif

#ifdef HAVE_RS232
/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_1_NAME,    N_("Specify name of first RS232 device")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RS232_1_NAME_DA, "Angiv navn på første RS232-enhed"},
/* de */ {IDCLS_SPECIFY_RS232_1_NAME_DE, "Name für erstes RS232 Gerät definieren"},
/* fr */ {IDCLS_SPECIFY_RS232_1_NAME_FR, "Spécifier le nom du 1er périphérique RS232"},
/* hu */ {IDCLS_SPECIFY_RS232_1_NAME_HU, "Adja meg az elsõ RS232-es eszköz nevét"},
/* it */ {IDCLS_SPECIFY_RS232_1_NAME_IT, "Specifica il nome della prima RS232"},
/* nl */ {IDCLS_SPECIFY_RS232_1_NAME_NL, "Geef de naam van het eerste RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_1_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_1_NAME_SV, "Ange namn på första RS232-enheten"},
/* tr */ {IDCLS_SPECIFY_RS232_1_NAME_TR, "Ýlk RS232 aygýtýnýn ismini belirt"},
#endif

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_2_NAME,    N_("Specify name of second RS232 device")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RS232_2_NAME_DA, "Angiv navn på anden RS232-enhed"},
/* de */ {IDCLS_SPECIFY_RS232_2_NAME_DE, "Name für zweites RS232 Gerät definieren"},
/* fr */ {IDCLS_SPECIFY_RS232_2_NAME_FR, "Spécifier le nom du 2e périphérique RS232"},
/* hu */ {IDCLS_SPECIFY_RS232_2_NAME_HU, "Adja meg a második RS232-es eszköz nevét"},
/* it */ {IDCLS_SPECIFY_RS232_2_NAME_IT, "Specifica il nome della seconda RS232"},
/* nl */ {IDCLS_SPECIFY_RS232_2_NAME_NL, "Geef de naam van het tweede RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_2_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_2_NAME_SV, "Ange namn på andra RS232-enheten"},
/* tr */ {IDCLS_SPECIFY_RS232_2_NAME_TR, "Ýkinci RS232 aygýtýnýn ismini belirt"},
#endif

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_3_NAME,    N_("Specify name of third RS232 device")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RS232_3_NAME_DA, "Angiv navn på tredje RS232-enhed"},
/* de */ {IDCLS_SPECIFY_RS232_3_NAME_DE, "Name für drittes RS232 Gerät definieren"},
/* fr */ {IDCLS_SPECIFY_RS232_3_NAME_FR, "Spécifier le nom du 3e périphérique RS232"},
/* hu */ {IDCLS_SPECIFY_RS232_3_NAME_HU, "Adja meg a harmadik RS232-es eszköz nevét"},
/* it */ {IDCLS_SPECIFY_RS232_3_NAME_IT, "Specifica il nome della terza RS232"},
/* nl */ {IDCLS_SPECIFY_RS232_3_NAME_NL, "Geef de naam van het derde RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_3_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_3_NAME_SV, "Ange namn på tredje RS232-enheten"},
/* tr */ {IDCLS_SPECIFY_RS232_3_NAME_TR, "Üçüncü RS232 aygýtýnýn ismini belirt"},
#endif

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_4_NAME,    N_("Specify name of fourth RS232 device")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RS232_4_NAME_DA, "Angiv navn på fjerde RS232-enhed"},
/* de */ {IDCLS_SPECIFY_RS232_4_NAME_DE, "Name für viertes RS232 Gerät definieren"},
/* fr */ {IDCLS_SPECIFY_RS232_4_NAME_FR, "Spécifier le nom du 4e périphérique RS232"},
/* hu */ {IDCLS_SPECIFY_RS232_4_NAME_HU, "Adja meg a negyedik RS232-es eszköz nevét"},
/* it */ {IDCLS_SPECIFY_RS232_4_NAME_IT, "Specifica il nome della quarta RS232"},
/* nl */ {IDCLS_SPECIFY_RS232_4_NAME_NL, "Geef de naam van het vierde RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_4_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_4_NAME_SV, "Ange namn på fjärde RS232-enheten"},
/* tr */ {IDCLS_SPECIFY_RS232_4_NAME_TR, "Dördüncü RS232 aygýtýnýn ismini belirt"},
#endif
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_ENABLE_RS232_USERPORT,    N_("Enable RS232 userport emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RS232_USERPORT_DA, "Aktivér RS232-emulering på brugerporten"},
/* de */ {IDCLS_ENABLE_RS232_USERPORT_DE, "RS232 Userport Emulation aktivieren"},
/* fr */ {IDCLS_ENABLE_RS232_USERPORT_FR, "Activer l'émulation de périphérique RS232 userport"},
/* hu */ {IDCLS_ENABLE_RS232_USERPORT_HU, "RS232 userport emuláció engedélyezése"},
/* it */ {IDCLS_ENABLE_RS232_USERPORT_IT, "Attiva l'emulazione della RS232 su userport"},
/* nl */ {IDCLS_ENABLE_RS232_USERPORT_NL, "Activeer RS232-userportemulatie"},
/* pl */ {IDCLS_ENABLE_RS232_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RS232_USERPORT_SV, "Aktivera RS232-emulering på användarporten"},
/* tr */ {IDCLS_ENABLE_RS232_USERPORT_TR, "RS232 userport emülasyonunu aktif et"},
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_DISABLE_RS232_USERPORT,    N_("Disable RS232 userport emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RS232_USERPORT_DA, "Deaktiver RS232-emulering på brugerporten"},
/* de */ {IDCLS_DISABLE_RS232_USERPORT_DE, "RS232 Userport Emulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_RS232_USERPORT_FR, "Désactiver l'émulation de périphérique RS232 userport"},
/* hu */ {IDCLS_DISABLE_RS232_USERPORT_HU, "RS232 userport emuláció tiltása"},
/* it */ {IDCLS_DISABLE_RS232_USERPORT_IT, "Disattiva l'emulazione della RS232 su userport"},
/* nl */ {IDCLS_DISABLE_RS232_USERPORT_NL, "RS232-userportemulatie uitschakelen"},
/* pl */ {IDCLS_DISABLE_RS232_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RS232_USERPORT_SV, "Inaktivera RS232-emulering på användarporten"},
/* tr */ {IDCLS_DISABLE_RS232_USERPORT_TR, "RS232 userport emülasyonunu pasifleþtir"},
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_P_BAUD,    N_("<baud>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_BAUD_PL, "<baud>"},
/* de */ {IDCLS_P_BAUD_DE, "<Baud>"},
/* fr */ {IDCLS_P_BAUD_FR, "<baud>"},
/* hu */ {IDCLS_P_BAUD_HU, "<bitráta>"},
/* it */ {IDCLS_P_BAUD_IT, "<baud>"},
/* nl */ {IDCLS_P_BAUD_NL, "<baud>"},
/* pl */ {IDCLS_P_BAUD_PL, "<baud>"},
/* sv */ {IDCLS_P_BAUD_SV, "<baud>"},
/* tr */ {IDCLS_P_BAUD_TR, "<baud>"},
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_SET_BAUD_RS232_USERPORT,    N_("Set the baud rate of the RS232 userport emulation.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_BAUD_RS232_USERPORT_DA, "Angiv baudhastighet for RS232-emuleringen på brugerporten."},
/* de */ {IDCLS_SET_BAUD_RS232_USERPORT_DE, "Baudrate für RS232 Userport Emulation setzen"},
/* fr */ {IDCLS_SET_BAUD_RS232_USERPORT_FR, "Spécifiez la vitesse en baud du périphérique RS232 userport"},
/* hu */ {IDCLS_SET_BAUD_RS232_USERPORT_HU, "Bitráta megadása az RS232 userport emulációhoz."},
/* it */ {IDCLS_SET_BAUD_RS232_USERPORT_IT, "Imposta la velocità della RS232 su userport (in baud)"},
/* nl */ {IDCLS_SET_BAUD_RS232_USERPORT_NL, "Zet de baud rate van de RS232 userport emulatie."},
/* pl */ {IDCLS_SET_BAUD_RS232_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_BAUD_RS232_USERPORT_SV, "Ange baudhastighet för RS232-emuleringen på användarporten."},
/* tr */ {IDCLS_SET_BAUD_RS232_USERPORT_TR, "RS232 userport emülasyonunun baud rate'ini ayarla."},
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT,    N_("Specify VICE RS232 device for userport")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_DA, "Angiv VICE RS232-enhed for brugerporten"},
/* de */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_DE, "VICE RS232 Gerät für Userport definieren"},
/* fr */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_FR, "Spécifier un périphérique VICE RS232 pour le userport"},
/* hu */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_HU, "VICE RS232-es userport eszköz megadása"},
/* it */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_IT, "Specifica il dispositivo RS232 di VICE su userport"},
/* nl */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_NL, "Geef VICE RS232-apparaat voor userport"},
/* pl */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_SV, "Ange VICE RS232-enhet för användarporten"},
/* tr */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_TR, "Userport için VICE RS232 aygýtýný belirt"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_4,    N_("Enable IEC device emulation for device #4")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_4_DA, "Aktivér IEC-enhedsemulering for enhed #4"},
/* de */ {IDCLS_ENABLE_IEC_4_DE, "IEC Geräte Emulation für Gerät #4 aktivieren"},
/* fr */ {IDCLS_ENABLE_IEC_4_FR, "Activer l'émulation IEC pour le périphérique #4"},
/* hu */ {IDCLS_ENABLE_IEC_4_HU, "IEC eszköz #4 emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_IEC_4_IT, "Attiva l'emulazione IEC per la periferica #4"},
/* nl */ {IDCLS_ENABLE_IEC_4_NL, "Activeer IEC-apparaat emulatie voor apparaat #4"},
/* pl */ {IDCLS_ENABLE_IEC_4_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_4_SV, "Aktivera IEC-enhetsemulering för enhet 4"},
/* tr */ {IDCLS_ENABLE_IEC_4_TR, "Aygýt #4 için IEC aygýt emülasyonunu aktif et"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_4,    N_("Disable IEC device emulation for device #4")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_4_DA, "Deaktiver IEC-enhedsemulering for enhed #4"},
/* de */ {IDCLS_DISABLE_IEC_4_DE, "IEC Geräte Emulation für Gerät #4 deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEC_4_FR, "Désactiver l'émulation IEC pour le périphérique #4"},
/* hu */ {IDCLS_DISABLE_IEC_4_HU, "IEC eszköz #4 emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_IEC_4_IT, "Disattiva l'emulazione IEC per la periferica #4"},
/* nl */ {IDCLS_DISABLE_IEC_4_NL, "IEC-apparaat emulatie voor apparaat #4 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_4_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_4_SV, "Inaktivera IEC-enhetsemulering för enhet 4"},
/* tr */ {IDCLS_DISABLE_IEC_4_TR, "Aygýt #4 için IEC aygýt emülasyonunu pasifleþtir"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_5,    N_("Enable IEC device emulation for device #5")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_5_DA, "Aktivér IEC-enhedsemulering for enhed #5"},
/* de */ {IDCLS_ENABLE_IEC_5_DE, "IEC Geräte Emulation für Gerät #5 aktivieren"},
/* fr */ {IDCLS_ENABLE_IEC_5_FR, "Activer l'émulation IEC pour le périphérique #5"},
/* hu */ {IDCLS_ENABLE_IEC_5_HU, "IEC eszköz #5 emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_IEC_5_IT, "Attiva l'emulazione IEC per la periferica #5"},
/* nl */ {IDCLS_ENABLE_IEC_5_NL, "Activeer IEC-apparaat emulatie voor apparaat #5"},
/* pl */ {IDCLS_ENABLE_IEC_5_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_5_SV, "Aktivera IEC-enhetsemulering för enhet 5"},
/* tr */ {IDCLS_ENABLE_IEC_5_TR, "Aygýt #5 için IEC aygýt emülasyonunu aktif et"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_5,    N_("Disable IEC device emulation for device #5")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_5_DA, "Deaktiver IEC-enhedsemulering for enhed #5"},
/* de */ {IDCLS_DISABLE_IEC_5_DE, "IEC Geräte Emulation für Gerät #5 deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEC_5_FR, "Désactiver l'émulation IEC pour le périphérique #5"},
/* hu */ {IDCLS_DISABLE_IEC_5_HU, "IEC eszköz #5 emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_IEC_5_IT, "Disattiva l'emulazione IEC per la periferica #5"},
/* nl */ {IDCLS_DISABLE_IEC_5_NL, "IEC-apparaat emulatie voor apparaat #5 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_5_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_5_SV, "Inaktivera IEC-enhetsemulering för enhet 5"},
/* tr */ {IDCLS_DISABLE_IEC_5_TR, "Aygýt #5 için IEC aygýt emülasyonunu pasifleþtir"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_8,    N_("Enable IEC device emulation for device #8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_8_DA, "Aktivér IEC-enhedsemulering for enhed #8"},
/* de */ {IDCLS_ENABLE_IEC_8_DE, "IEC Geräte Emulation für Gerät #8 aktivieren"},
/* fr */ {IDCLS_ENABLE_IEC_8_FR, "Activer l'émulation IEC pour le périphérique #8"},
/* hu */ {IDCLS_ENABLE_IEC_8_HU, "IEC eszköz #8 emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_IEC_8_IT, "Attiva l'emulazione IEC per la periferica #8"},
/* nl */ {IDCLS_ENABLE_IEC_8_NL, "Activeer IEC-apparaat emulatie voor apparaat #8"},
/* pl */ {IDCLS_ENABLE_IEC_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_8_SV, "Aktivera IEC-enhetsemulering för enhet 8"},
/* tr */ {IDCLS_ENABLE_IEC_8_TR, "Aygýt #8 için IEC aygýt emülasyonunu aktif et"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_8,    N_("Disable IEC device emulation for device #8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_8_DA, "Deaktiver IEC-enhedsemulering for enhed #8"},
/* de */ {IDCLS_DISABLE_IEC_8_DE, "IEC Geräte Emulation für Gerät #8 deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEC_8_FR, "Désactiver l'émulation IEC pour le périphérique #8"},
/* hu */ {IDCLS_DISABLE_IEC_8_HU, "IEC eszköz #8 emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_IEC_8_IT, "Disattiva l'emulazione IEC per la periferica #8"},
/* nl */ {IDCLS_DISABLE_IEC_8_NL, "IEC-apparaat emulatie voor apparaat #8 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_8_SV, "Inaktivera IEC-enhetsemulering för enhet 8"},
/* tr */ {IDCLS_DISABLE_IEC_8_TR, "Aygýt #8 için IEC aygýt emülasyonunu pasifleþtir"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_9,    N_("Enable IEC device emulation for device #9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_9_DA, "Aktivér IEC-enhedsemulering for enhed #9"},
/* de */ {IDCLS_ENABLE_IEC_9_DE, "IEC Geräte Emulation für Gerät #9 aktivieren"},
/* fr */ {IDCLS_ENABLE_IEC_9_FR, "Activer l'émulation IEC pour le périphérique #9"},
/* hu */ {IDCLS_ENABLE_IEC_9_HU, "IEC eszköz #9 emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_IEC_9_IT, "Attiva l'emulazione IEC per la periferica #9"},
/* nl */ {IDCLS_ENABLE_IEC_9_NL, "Activeer IEC-apparaat emulatie voor apparaat #9"},
/* pl */ {IDCLS_ENABLE_IEC_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_9_SV, "Aktivera IEC-enhetsemulering för enhet 9"},
/* tr */ {IDCLS_ENABLE_IEC_9_TR, "Aygýt #9 için IEC aygýt emülasyonunu aktif et"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_9,    N_("Disable IEC device emulation for device #9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_9_DA, "Deaktiver IEC-enhedsemulering for enhed #9"},
/* de */ {IDCLS_DISABLE_IEC_9_DE, "IEC Geräte Emulation für Gerät #9 deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEC_9_FR, "Désactiver l'émulation IEC pour le périphérique #9"},
/* hu */ {IDCLS_DISABLE_IEC_9_HU, "IEC eszköz #9 emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_IEC_9_IT, "Disattiva l'emulazione IEC per la periferica #9"},
/* nl */ {IDCLS_DISABLE_IEC_9_NL, "IEC-apparaat emulatie voor apparaat #9 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_9_SV, "Inaktivera IEC-enhetsemulering för enhet 9"},
/* tr */ {IDCLS_DISABLE_IEC_9_TR, "Aygýt #9 için IEC aygýt emülasyonunu pasifleþtir"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_10,    N_("Enable IEC device emulation for device #10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_10_DA, "Aktivér IEC-enhedsemulering for enhed #10"},
/* de */ {IDCLS_ENABLE_IEC_10_DE, "IEC Geräte Emulation für Gerät #10 aktivieren"},
/* fr */ {IDCLS_ENABLE_IEC_10_FR, "Activer l'émulation IEC pour le périphérique #10"},
/* hu */ {IDCLS_ENABLE_IEC_10_HU, "IEC eszköz #10 emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_IEC_10_IT, "Attiva l'emulazione IEC per la periferica #10"},
/* nl */ {IDCLS_ENABLE_IEC_10_NL, "Activeer IEC-apparaat emulatie voor apparaat #10"},
/* pl */ {IDCLS_ENABLE_IEC_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_10_SV, "Aktivera IEC-enhetsemulering för enhet 10"},
/* tr */ {IDCLS_ENABLE_IEC_10_TR, "Aygýt #10 için IEC aygýt emülasyonunu aktif et"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_10,    N_("Disable IEC device emulation for device #10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_10_DA, "Deaktiver IEC-enhedsemulering for enhed #10"},
/* de */ {IDCLS_DISABLE_IEC_10_DE, "IEC Geräte Emulation für Gerät #10 deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEC_10_FR, "Désactiver l'émulation IEC pour le périphérique #10"},
/* hu */ {IDCLS_DISABLE_IEC_10_HU, "IEC eszköz #10 emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_IEC_10_IT, "Disattiva l'emulazione IEC per la periferica #10"},
/* nl */ {IDCLS_DISABLE_IEC_10_NL, "IEC-apparaat emulatie voor apparaat #10 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_10_SV, "Inaktivera IEC-enhetsemulering för enhet 10"},
/* tr */ {IDCLS_DISABLE_IEC_10_TR, "Aygýt #10 için IEC aygýt emülasyonunu pasifleþtir"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_11,    N_("Enable IEC device emulation for device #11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_11_DA, "Aktivér IEC-enhedsemulering for enhed #11"},
/* de */ {IDCLS_ENABLE_IEC_11_DE, "IEC Geräte Emulation für Gerät #11 aktivieren"},
/* fr */ {IDCLS_ENABLE_IEC_11_FR, "Activer l'émulation IEC pour le périphérique #11"},
/* hu */ {IDCLS_ENABLE_IEC_11_HU, "IEC eszköz #11 emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_IEC_11_IT, "Attiva l'emulazione IEC per la periferica #11"},
/* nl */ {IDCLS_ENABLE_IEC_11_NL, "Activeer IEC-apparaat emulatie voor apparaat #11"},
/* pl */ {IDCLS_ENABLE_IEC_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_11_SV, "Aktivera IEC-enhetsemulering för enhet 11"},
/* tr */ {IDCLS_ENABLE_IEC_11_TR, "Aygýt #11 için IEC aygýt emülasyonunu aktif et"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_11,    N_("Disable IEC device emulation for device #11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_11_DA, "Deaktiver IEC-enhedsemulering for enhed #11"},
/* de */ {IDCLS_DISABLE_IEC_11_DE, "IEC Geräte Emulation für Gerät #11 deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEC_11_FR, "Désactiver l'émulation IEC pour le périphérique #11"},
/* hu */ {IDCLS_DISABLE_IEC_11_HU, "IEC eszköz #11 emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_IEC_11_IT, "Disattiva l'emulazione IEC per la periferica #11"},
/* nl */ {IDCLS_DISABLE_IEC_11_NL, "IEC-apparaat emulatie voor apparaat #11 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_11_SV, "Inaktivera IEC-enhetsemulering för enhet 11"},
/* tr */ {IDCLS_DISABLE_IEC_11_TR, "Aygýt #11 için IEC aygýt emülasyonunu pasifleþtir"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_P_ENGINE,    N_("<engine>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_ENGINE_DA, "<motor>"},
/* de */ {IDCLS_P_ENGINE_DE, "<Engine>"},
/* fr */ {IDCLS_P_ENGINE_FR, "<engin>"},
/* hu */ {IDCLS_P_ENGINE_HU, "<motor>"},
/* it */ {IDCLS_P_ENGINE_IT, "<motore>"},
/* nl */ {IDCLS_P_ENGINE_NL, "<kern>"},
/* pl */ {IDCLS_P_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_ENGINE_SV, "<motor>"},
/* tr */ {IDCLS_P_ENGINE_TR, "<motor>"},
#endif

#if !defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID)"},
#endif
#endif

#if defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID)"},
#endif
#endif

#if !defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 2: Catweasel)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 2: Catweasel)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: Catweasel)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 2: Catweasel)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 2: Catweasel)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 2: Catweasel)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 2: Catweasel)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 2: Catweasel)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 2: Catweasel)"},
#endif
#endif

#if !defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 3: HardSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 3: HardSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 3: HardSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 3: HardSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 3: HardSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 3: HardSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 3: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 3: HardSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 3: HardSID)"},
#endif
#endif

#if !defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 4: ParSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 4: ParSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 4: ParSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 4: ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 4: ParSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 4: ParSID)"},
#endif
#endif

#if !defined(HAVE_RESID) && defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 7: ReSID-FP)"},
#endif
#endif

#if defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 2: Catweasel)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 2: Catweasel)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 2: Catweasel)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 2: Catweasel)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 2: Catweasel)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 2: Catweasel)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 2: Catweasel)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 2: Catweasel)"},
#endif
#endif

#if defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 3: HardSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 3: HardSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 3: HardSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 3: HardSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 3: HardSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 3: HardSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 3: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 3: HardSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 3: HardSID)"},
#endif
#endif

#if defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 4: ParSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 4: ParSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 3: HardSID, 4: ParSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 4: ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 4: ParSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 4: ParSID)"},
#endif
#endif

#if defined(HAVE_RESID) && defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 7: ParSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 3: HardSID, 7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 7: ReSID-FP)"},
#endif
#endif

#if !defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 2: Catweasel, 3: HardSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 2: Catweasel, 3: HardSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: Catweasel, 3: HardSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 2: Catweasel, 3: HardSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 2: Catweasel, 3: HardSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 2: Catweasel, 3: HardSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 2: Catweasel, "
                                       "3: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 2: Catweasel, 3: HardSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 2: Catweasel, 3: HardSID)"},
#endif
#endif

#if !defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 2: Catweasel, 4: ParSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 2: Catweasel, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: Catweasel, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 2: Catweasel, 4:ParSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 2: Catweasel, 4: ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 2: Catweasel, "
                                       "4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 2: Catweasel, 4: ParSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 2: Catweasel, 4: ParSID)"},
#endif
#endif

#if !defined(HAVE_RESID) && defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 2: Catweasel, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 2: Catweasel, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: Catweasel, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 2: Catweasel, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 2: Catweasel, 7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 2: Catweasel, "
                                       "7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 2: Catweasel, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 2: Catweasel, 7: ReSID-FP)"},
#endif
#endif

#if !defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 3: HardSID, 4: ParSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 3: HardSID, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 3: HardSID, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 3: HardSID, 4: ParSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 3: HardSID, 4: ParSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 3: HardSID, 4: ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 3: HardSID, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 3: HardSID, 4: ParSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 3: HardSID, 4: ParSID)"},
#endif
#endif

#if !defined(HAVE_RESID) && defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 3: HardSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 3: HardSID, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 3: HardSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 3: HardSID, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 3: HardSID, 7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 3: HardSID, 7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 3: HardSID, "
                                       "7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 3: HardSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 3: HardSID, 7: ReSID-FP)"},
#endif
#endif

#if !defined(HAVE_RESID) && defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 4: ParSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 4: ParSID, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 4: ParSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 4: ParSID, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 4: ParSID, 7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 4: ParSID, 7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 4: ParSID, "
                                       "7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 4: ParSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 4: ParSID, 7: ReSID-FP)"},
#endif
#endif

#if defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 2: Catweasel, 3: HardSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 2: Catweasel, "
                                       "3: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID)"},
#endif
#endif

#if defined(HAVE_RESID) && !defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 2: Catweasel, 4: ParSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 2: Catweasel, "
                                       "4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID)"},
#endif
#endif

#if defined(HAVE_RESID) && defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 2: Catweasel, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 2: Catweasel, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 2: Catweasel, 7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 2: Catweasel, 7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 2: Catweasel, "
                                       "7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 2: Catweasel, 7: ReSID-FP)"},
#endif
#endif

#if defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 3: HardSID, 4: ParSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 3: HardSID, "
                                       "4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID)"},
#endif
#endif

#if defined(HAVE_RESID) && defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 3: HardSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 3: HardSID, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 3: HardSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 3: HardSID, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 3: HardSID, 7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 3: HardSID, 7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 3: HardSID, "
                                       "7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 3: HardSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 3: HardSID, 7: ReSID-FP)"},
#endif
#endif

#if defined(HAVE_RESID) && defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 4: ParSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 4: ParSID, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 4: ParSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 4: ParSID, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 4: ParSID, 7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 4: ParSID, 7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 4: ParSID, "
                                       "7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 4: ParSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 4: ParSID, 7: ReSID-FP)"},
#endif
#endif

#if !defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 2: Catweasel, 3: "
                                       "HardSID, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
#endif
#endif

#if !defined(HAVE_RESID) && defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 2: Catweasel, 3: "
                                       "HardSID, 7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
#endif
#endif

#if !defined(HAVE_RESID) && defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 2: Catweasel, 4: "
                                       "ParSID, 7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)"},
#endif
#endif

#if !defined(HAVE_RESID) && defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 3: HardSID, 4: "
                                       "ParSID, 7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
#endif
#endif

#if defined(HAVE_RESID) && !defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: "
                                       "ParSID)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: "
                                       "ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 2: Catweasel, "
                                       "3: HardSID, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
#endif
#endif

#if defined(HAVE_RESID) && defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 2: Catweasel, 3: HardSID, "
                                       "7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, "
                                       "7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 2: Catweasel, "
                                       "3: HardSID, 7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 7: ReSID-FP)"},
#endif
#endif

#if defined(HAVE_RESID) && defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 2: Catweasel, 4: ParSID, "
                                       "7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID, "
                                       "7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 2: Catweasel, "
                                       "4: ParSID, 7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID, 7: ReSID-FP)"},
#endif
#endif

#if defined(HAVE_RESID) && defined(HAVE_RESID_FP) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 3: HardSID, 4: ParSID, "
                                       "7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID, "
                                       "7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 3: HardSID, "
                                       "4: ParSID, 7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
#endif
#endif

#if !defined(HAVE_RESID) && defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID, 7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID, "
                                       "7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 2: Catweasel, 3: HardSID, 4: ParSID, "
                                       "7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID, "
                                       "7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 2: Catweasel, 3: "
                                       "HardSID, 4: ParSID, 7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID, 7: ReSID-FP)"},
#endif
#endif

#if defined(HAVE_RESID) && defined(HAVE_RESID_FP) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    N_("Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID, "
                                          "7: ReSID-FP)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID, "
"7: ReSID-FP)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID, "
                                       "7: ReSID-FP)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID, "
                                       "7: ReSID-FP)"},
/* hu */ {IDCLS_SPECIFY_SID_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: "
                                       "ParSID, 7: ReSID-FP)"},
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: "
                                       "ParSID, 7: ReSID-FP)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: ReSID, 2: Catweasel, "
                                       "3: HardSID, 4: ParSID, 7: ReSID-FP)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID, "
                                       "7: ReSID-FP)"},
/* tr */ {IDCLS_SPECIFY_SID_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID, "
                                       "7: ReSID-FP)"},
#endif
#endif

#if !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIDCART_ENGINE,    N_("Specify SID engine (0: FastSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SIDCART_ENGINE_DA, "Vælg SID-motor (0: FastSID)"},
/* de */ {IDCLS_SPECIFY_SIDCART_ENGINE_DE, "SID Engine wählen (0: FastSID)"},
/* fr */ {IDCLS_SPECIFY_SIDCART_ENGINE_FR, "Spécifier l’engin SID (0: FastSID)"},
/* hu */ {IDCLS_SPECIFY_SIDCART_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID)"},
/* it */ {IDCLS_SPECIFY_SIDCART_ENGINE_IT, "Specifica il motore SID (0: FastSID)"},
/* nl */ {IDCLS_SPECIFY_SIDCART_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID)"},
/* pl */ {IDCLS_SPECIFY_SIDCART_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SIDCART_ENGINE_SV, "Ange SID-motor (0: FastSID)"},
/* tr */ {IDCLS_SPECIFY_SIDCART_ENGINE_TR, "SID motorunu belirt (0: FastSID)"},
#endif
#endif

#if defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIDCART_ENGINE,    N_("Specify SID engine (0: FastSID, 1: Catweasel)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SIDCART_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: Catweasel)"},
/* de */ {IDCLS_SPECIFY_SIDCART_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: Catweasel)"},
/* fr */ {IDCLS_SPECIFY_SIDCART_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: Catweasel)"},
/* hu */ {IDCLS_SPECIFY_SIDCART_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: Catweasel)"},
/* it */ {IDCLS_SPECIFY_SIDCART_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: Catweasel)"},
/* nl */ {IDCLS_SPECIFY_SIDCART_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: Catweasel)"},
/* pl */ {IDCLS_SPECIFY_SIDCART_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SIDCART_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: Catweasel)"},
/* tr */ {IDCLS_SPECIFY_SIDCART_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: Catweasel)"},
#endif
#endif

#if !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIDCART_ENGINE,    N_("Specify SID engine (0: FastSID, 2: HardSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SIDCART_ENGINE_DA, "Vælg SID-motor (0: FastSID, 2: HardSID)"},
/* de */ {IDCLS_SPECIFY_SIDCART_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: HardSID)"},
/* fr */ {IDCLS_SPECIFY_SIDCART_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 2: HardSID)"},
/* hu */ {IDCLS_SPECIFY_SIDCART_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 2: HardSID)"},
/* it */ {IDCLS_SPECIFY_SIDCART_ENGINE_IT, "Specifica il motore SID (0: FastSID, 2: HardSID)"},
/* nl */ {IDCLS_SPECIFY_SIDCART_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 2: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SIDCART_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SIDCART_ENGINE_SV, "Ange SID-motor (0: FastSID, 2: HardSID)"},
/* tr */ {IDCLS_SPECIFY_SIDCART_ENGINE_TR, "SID motorunu belirt (0: FastSID, 2: HardSID)"},
#endif
#endif

#if !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIDCART_ENGINE,    N_("Specify SID engine (0: FastSID, 3: ParSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SIDCART_ENGINE_DA, "Vælg SID-motor (0: FastSID, 3: ParSID)"},
/* de */ {IDCLS_SPECIFY_SIDCART_ENGINE_DE, "SID Engine wählen (0: FastSID, 3: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SIDCART_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 3: ParSID)"},
/* hu */ {IDCLS_SPECIFY_SIDCART_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 3: ParSID)"},
/* it */ {IDCLS_SPECIFY_SIDCART_ENGINE_IT, "Specifica il motore SID (0: FastSID, 3: ParSID)"},
/* nl */ {IDCLS_SPECIFY_SIDCART_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 3: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SIDCART_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SIDCART_ENGINE_SV, "Ange SID-motor (0: FastSID, 3: ParSID)"},
/* tr */ {IDCLS_SPECIFY_SIDCART_ENGINE_TR, "SID motorunu belirt (0: FastSID, 3: ParSID)"},
#endif
#endif

#if defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIDCART_ENGINE,    N_("Specify SID engine (0: FastSID, 1: Catweasel, 2: HardSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SIDCART_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: Catweasel, 2: HardSID)"},
/* de */ {IDCLS_SPECIFY_SIDCART_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: Catweasel, 2: HardSID)"},
/* fr */ {IDCLS_SPECIFY_SIDCART_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: Catweasel, 2: HardSID)"},
/* hu */ {IDCLS_SPECIFY_SIDCART_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: Catweasel, 2: HardSID)"},
/* it */ {IDCLS_SPECIFY_SIDCART_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: Catweasel, 2: HardSID)"},
/* nl */ {IDCLS_SPECIFY_SIDCART_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: Catweasel, 2: "
                                           "HardSID)"},
/* pl */ {IDCLS_SPECIFY_SIDCART_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SIDCART_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: Catweasel, 2: HardSID)"},
/* tr */ {IDCLS_SPECIFY_SIDCART_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: Catweasel, 2: HardSID)"},
#endif
#endif

#if defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIDCART_ENGINE,    N_("Specify SID engine (0: FastSID, 1: Catweasel, 3: ParSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SIDCART_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: Catweasel, 3: ParSID)"},
/* de */ {IDCLS_SPECIFY_SIDCART_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: Catweasel, 3: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SIDCART_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: Catweasel, 3: ParSID)"},
/* hu */ {IDCLS_SPECIFY_SIDCART_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: Catweasel, 3: ParSID)"},
/* it */ {IDCLS_SPECIFY_SIDCART_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: Catweasel, 3: ParSID)"},
/* nl */ {IDCLS_SPECIFY_SIDCART_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: Catweasel, 3: "
                                           "ParSID)"},
/* pl */ {IDCLS_SPECIFY_SIDCART_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SIDCART_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: Catweasel, 3: ParSID)"},
/* tr */ {IDCLS_SPECIFY_SIDCART_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: Catweasel, 3: ParSID)"},
#endif
#endif

#if !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIDCART_ENGINE,    N_("Specify SID engine (0: FastSID, 2: HardSID, 3: ParSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SIDCART_ENGINE_DA, "Vælg SID-motor (0: FastSID, 2: HardSID, 3: ParSID)"},
/* de */ {IDCLS_SPECIFY_SIDCART_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: HardSID, 3: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SIDCART_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 2: HardSID, 3: ParSID)"},
/* hu */ {IDCLS_SPECIFY_SIDCART_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 2: HardSID, 3: ParSID)"},
/* it */ {IDCLS_SPECIFY_SIDCART_ENGINE_IT, "Specifica il motore SID (0: FastSID, 2: HardSID, 3: ParSID)"},
/* nl */ {IDCLS_SPECIFY_SIDCART_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 2: HardSID, 3: "
                                           "ParSID)"},
/* pl */ {IDCLS_SPECIFY_SIDCART_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SIDCART_ENGINE_SV, "Ange SID-motor (0: FastSID, 2: HardSID, 3: ParSID)"},
/* tr */ {IDCLS_SPECIFY_SIDCART_ENGINE_TR, "SID motorunu belirt (0: FastSID, 2: HardSID, 3: ParSID)"},
#endif
#endif

#if defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIDCART_ENGINE,    N_("Specify SID engine (0: FastSID, 1: Catweasel, 2: HardSID, 3: ParSID)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SIDCART_ENGINE_DA, "Vælg SID-motor (0: FastSID, 1: Catweasel, 2: HardSID, 3: ParSID)"},
/* de */ {IDCLS_SPECIFY_SIDCART_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: Catweasel, 2: HardSID, 3: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SIDCART_ENGINE_FR, "Spécifier l’engin SID (0: FastSID, 1: Catweasel, 2: HardSID, 3: ParSID)"},
/* hu */ {IDCLS_SPECIFY_SIDCART_ENGINE_HU, "Adja meg a SID motort (0: Gyors SID, 1: Catweasel, 2: HardSID, 3: ParSID)"},
/* it */ {IDCLS_SPECIFY_SIDCART_ENGINE_IT, "Specifica il motore SID (0: FastSID, 1: Catweasel, 2: HardSID, 3: ParSID)"},
/* nl */ {IDCLS_SPECIFY_SIDCART_ENGINE_NL, "Geef op welke SID-kern gebruikt moet worden (0: FastSID, 1: Catweasel, 2: "
                                           "HardSID, 3: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SIDCART_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SIDCART_ENGINE_SV, "Ange SID-motor (0: FastSID, 1: Catweasel, 2: HardSID, 3: ParSID)"},
/* tr */ {IDCLS_SPECIFY_SIDCART_ENGINE_TR, "SID motorunu belirt (0: FastSID, 1: Catweasel, 2: HardSID, 3: ParSID)"},
#endif
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SECOND_SID,    N_("Enable second SID")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SECOND_SID_DA, "Aktivér sekundær SID"},
/* de */ {IDCLS_ENABLE_SECOND_SID_DE, "Zweiten SID aktivieren"},
/* fr */ {IDCLS_ENABLE_SECOND_SID_FR, "Activer le second SID"},
/* hu */ {IDCLS_ENABLE_SECOND_SID_HU, "Második SID engedélyezése"},
/* it */ {IDCLS_ENABLE_SECOND_SID_IT, "Attiva Secondo SID"},
/* nl */ {IDCLS_ENABLE_SECOND_SID_NL, "Activeer stereo-SID"},
/* pl */ {IDCLS_ENABLE_SECOND_SID_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SECOND_SID_SV, "Aktivera andra SID"},
/* tr */ {IDCLS_ENABLE_SECOND_SID_TR, "Ýkinci SID'i aktif et"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SIDCART,    N_("Enable SID Cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SIDCART_DA, "Aktivér SID-cartridge"},
/* de */ {IDCLS_ENABLE_SIDCART_DE, "SID Modul aktivieren"},
/* fr */ {IDCLS_ENABLE_SIDCART_FR, "Activer la cartouche SID"},
/* hu */ {IDCLS_ENABLE_SIDCART_HU, "SID cartridge engedélyezése"},
/* it */ {IDCLS_ENABLE_SIDCART_IT, "Attiva cartuccia SID"},
/* nl */ {IDCLS_ENABLE_SIDCART_NL, "Activeer SID-Cartridge"},
/* pl */ {IDCLS_ENABLE_SIDCART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SIDCART_SV, "Aktivera SID-instickskort"},
/* tr */ {IDCLS_ENABLE_SIDCART_TR, "SID kartuþunu aktif et"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SIDCART,    N_("Disable SID Cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SIDCART_DA, "Deaktiver SID-cartridge"},
/* de */ {IDCLS_DISABLE_SIDCART_DE, "SID Modul deaktivieren"},
/* fr */ {IDCLS_DISABLE_SIDCART_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_SIDCART_HU, "SID cartridge tiltása"},
/* it */ {IDCLS_DISABLE_SIDCART_IT, "Disattiva cartuccia SID"},
/* nl */ {IDCLS_DISABLE_SIDCART_NL, "SID-Cartridge uitschakelen"},
/* pl */ {IDCLS_DISABLE_SIDCART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SIDCART_SV, "Inaktivera SID-instickskort"},
/* tr */ {IDCLS_DISABLE_SIDCART_TR, "SID kartuþunu pasifleþtir"},
#endif

/* sid/sid-cmdline-options.c, c64/plus60k, c64/c64_256k.c */
/* en */ {IDCLS_P_BASE_ADDRESS,    N_("<base address>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_BASE_ADDRESS_DA, "<baseadresse>"},
/* de */ {IDCLS_P_BASE_ADDRESS_DE, "<Basis Adresse>"},
/* fr */ {IDCLS_P_BASE_ADDRESS_FR, "<adresse de base>"},
/* hu */ {IDCLS_P_BASE_ADDRESS_HU, "<báziscím>"},
/* it */ {IDCLS_P_BASE_ADDRESS_IT, "<indirizzo base>"},
/* nl */ {IDCLS_P_BASE_ADDRESS_NL, "<basisadres>"},
/* pl */ {IDCLS_P_BASE_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_BASE_ADDRESS_SV, "<basadress>"},
/* tr */ {IDCLS_P_BASE_ADDRESS_TR, "<taban adresi>"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_2_ADDRESS,    N_("Specify base address for 2nd SID")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_2_ADDRESS_DA, "Angiv baseadresse for sekundær SID"},
/* de */ {IDCLS_SPECIFY_SID_2_ADDRESS_DE, "Basis Adresse für zweiten SID definieren"},
/* fr */ {IDCLS_SPECIFY_SID_2_ADDRESS_FR, "Spécifier l'adresse de base pour le second SID"},
/* hu */ {IDCLS_SPECIFY_SID_2_ADDRESS_HU, "Adja meg a báziscímét a második SID-nek"},
/* it */ {IDCLS_SPECIFY_SID_2_ADDRESS_IT, "Specifica l'indirizzo di base per il secondo SID"},
/* nl */ {IDCLS_SPECIFY_SID_2_ADDRESS_NL, "Geef het basisadres van de 2e SID"},
/* pl */ {IDCLS_SPECIFY_SID_2_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_2_ADDRESS_SV, "Ange basadress för andra SID"},
/* tr */ {IDCLS_SPECIFY_SID_2_ADDRESS_TR, "2nci SID için taban adresini belirt"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_P_MODEL,    N_("<model>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_MODEL_DA, "<model>"},
/* de */ {IDCLS_P_MODEL_DE, "<Modell>"},
/* fr */ {IDCLS_P_MODEL_FR, "<modèle>"},
/* hu */ {IDCLS_P_MODEL_HU, "<modell>"},
/* it */ {IDCLS_P_MODEL_IT, "<modello>"},
/* nl */ {IDCLS_P_MODEL_NL, "<model>"},
/* pl */ {IDCLS_P_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_MODEL_SV, "<modell>"},
/* tr */ {IDCLS_P_MODEL_TR, "<model>"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_MODEL,    N_("Specify SID model (0: 6581, 1: 8580, 2: 8580 + digi boost, 4: DTV)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SID_MODEL_DA, "Angiv SID-model (0: 6581, 1: 8580, 2: 8580 + digitalforstærkning, 4: DTV)"},
/* de */ {IDCLS_SPECIFY_SID_MODEL_DE, "SID Modell definieren (0: 6581, 1: 8580, 2: 8580 + digi boost, 4: DTV)"},
/* fr */ {IDCLS_SPECIFY_SID_MODEL_FR, "Spécifier le modèle SID (0: 6581, 1: 8580, 2: 8580 + Boost digital, 4: DTV)"},
/* hu */ {IDCLS_SPECIFY_SID_MODEL_HU, "Adja meg a SID típusát (0: 6581, 1: 8580, 2: 8580 + digi boost, 4: DTV)"},
/* it */ {IDCLS_SPECIFY_SID_MODEL_IT, "Specifica il modello di SID (0: 6581, 1: 8580, 2: 8580 + digi boost, 4: DTV)"},
/* nl */ {IDCLS_SPECIFY_SID_MODEL_NL, "Geef het SID model (0: 6581, 1: 8580, 2: 8580 + digi boost, 4: DTV)"},
/* pl */ {IDCLS_SPECIFY_SID_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_MODEL_SV, "Ange SID-modell (0: 6581, 1: 8580, 2: 8580 + digiförstärkning, 4: DTV)"},
/* tr */ {IDCLS_SPECIFY_SID_MODEL_TR, "SID modelini belirt (0: 6581, 1: 8580, 2: 8580 + digi boost, 4: DTV)"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SID_FILTERS,    N_("Emulate SID filters")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SID_FILTERS_DA, "Emulér SID-filtre"},
/* de */ {IDCLS_ENABLE_SID_FILTERS_DE, "SID Filter Emulation aktivieren"},
/* fr */ {IDCLS_ENABLE_SID_FILTERS_FR, "Émuler les filtres SID"},
/* hu */ {IDCLS_ENABLE_SID_FILTERS_HU, "SID szûrõk emulációja"},
/* it */ {IDCLS_ENABLE_SID_FILTERS_IT, "Emula i filtri del SID"},
/* nl */ {IDCLS_ENABLE_SID_FILTERS_NL, "Emuleer SID-filters"},
/* pl */ {IDCLS_ENABLE_SID_FILTERS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SID_FILTERS_SV, "Emulera SID-filter"},
/* tr */ {IDCLS_ENABLE_SID_FILTERS_TR, "SID filtrelerini emüle et"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SID_FILTERS,    N_("Do not emulate SID filters")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SID_FILTERS_DA, "Emulér ikke SID-filtre"},
/* de */ {IDCLS_DISABLE_SID_FILTERS_DE, "SID Filter Emulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_SID_FILTERS_FR, "Ne pas émuler les filtres SID"},
/* hu */ {IDCLS_DISABLE_SID_FILTERS_HU, "Nem emulálja a SID szûrõket"},
/* it */ {IDCLS_DISABLE_SID_FILTERS_IT, "Non emulare i filtri del SID"},
/* nl */ {IDCLS_DISABLE_SID_FILTERS_NL, "SID-filters niet emuleren"},
/* pl */ {IDCLS_DISABLE_SID_FILTERS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SID_FILTERS_SV, "Emulera inte SID-filter"},
/* tr */ {IDCLS_DISABLE_SID_FILTERS_TR, "SID filtrelerini emüle etme"},
#endif

#ifdef HAVE_RESID
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_RESID_SAMPLING_METHOD,    N_("reSID sampling method (0: fast, 1: interpolating, 2: resampling, 3: fast "
                                             "resampling)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_RESID_SAMPLING_METHOD_DA, "reSID-samplingmetode (0: hurtig, 1: interpolerende, 2: omsampling, 3: hurtig "
                                          "omsampling)"},
/* de */ {IDCLS_RESID_SAMPLING_METHOD_DE, "reSID Sample Methode (0: schnell, 1: interpolierend, 2: resampling, 3: "
                                          "schnelles resampling)"},
/* fr */ {IDCLS_RESID_SAMPLING_METHOD_FR, "Méthode reSID (0: rapide, 1: interpolation, 2: rééchantillonnage, 3: "
                                          "rééchantillonnage rapide)"},
/* hu */ {IDCLS_RESID_SAMPLING_METHOD_HU, "reSID mintavételezési mód (0: gyors, 1: interpoláló, 2: újramintavételezõ, 3: "
                                          "gyors újramintavételezõ)"},
/* it */ {IDCLS_RESID_SAMPLING_METHOD_IT, "Metodo di campionamento del reSID (0: veloce, 1: interpolato, 2: ricampionato "
                                          "3: ricampionamento veloce)"},
/* nl */ {IDCLS_RESID_SAMPLING_METHOD_NL, "reSID-sampling methode (0: snel, 1: interpoleren, 2: resampling, 3: snelle "
                                          "resampling)"},
/* pl */ {IDCLS_RESID_SAMPLING_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RESID_SAMPLING_METHOD_SV, "reSID-samplingsmetod (0: snabb, 1: interpolerande, 2: omsamplning, 3: snabb "
                                          "omsampling)"},
/* tr */ {IDCLS_RESID_SAMPLING_METHOD_TR, "reSID örnekleme metodu (0: hýzlý, 1: ara deðer bulma, 2: yeniden örnekleme, "
                                          "3: hýzlý yeniden örnekleme)"},
#endif

/* sid/sid-cmdline-options.c, vsync.c */
/* en */ {IDCLS_P_PERCENT,    N_("<percent>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_PERCENT_DA, "<procent>"},
/* de */ {IDCLS_P_PERCENT_DE, "<prozent>"},
/* fr */ {IDCLS_P_PERCENT_FR, "<pourcent>"},
/* hu */ {IDCLS_P_PERCENT_HU, "<százalék>"},
/* it */ {IDCLS_P_PERCENT_IT, "<percento>"},
/* nl */ {IDCLS_P_PERCENT_NL, "<procent>"},
/* pl */ {IDCLS_P_PERCENT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_PERCENT_SV, "<procent>"},
/* tr */ {IDCLS_P_PERCENT_TR, "<yüzde>"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_PASSBAND_PERCENTAGE,    N_("reSID resampling passband in percentage of total bandwidth (0 - 90)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PASSBAND_PERCENTAGE_DA, "Pasbånd for reSID-resampling i procent af total båndbredde (0 - 90)"},
/* de */ {IDCLS_PASSBAND_PERCENTAGE_DE, "reSID Resampling Passband Prozentwert der gesamte Bandbreite (0 - 90)\n(0 - 90, "
                                        "niedrig ist schneller, höher ist besser)"},
/* fr */ {IDCLS_PASSBAND_PERCENTAGE_FR, "Bande passante pour le resampling reSID en pourcentage de la bande totale "
                                        "(0 - 90)"},
/* hu */ {IDCLS_PASSBAND_PERCENTAGE_HU, "reSID újramintavételezési sávnak a teljes sávszélességre vonatkoztatott aránya "
                                        "(0 - 90)"},
/* it */ {IDCLS_PASSBAND_PERCENTAGE_IT, "Banda passante di ricampionamento del reSID in percentuale di quella totale "
                                        "(0 - 90)"},
/* nl */ {IDCLS_PASSBAND_PERCENTAGE_NL, "reSID-resampling passband in percentage van de totale bandbreedte (0 - 90)"},
/* pl */ {IDCLS_PASSBAND_PERCENTAGE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PASSBAND_PERCENTAGE_SV, "Passband för reSID-resampling i procent av total bandbredd (0 - 90)"},
/* tr */ {IDCLS_PASSBAND_PERCENTAGE_TR, "reSID yeniden örnekleme passband'inin toplam bant geniþliðine yüzdesel oraný "
                                        "(0 - 90)"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_RESID_GAIN_PERCENTAGE,    N_("reSID gain in percent (90 - 100)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_RESID_GAIN_PERCENTAGE_DA, "Gain for reSID i procent (90 - 100)"},
/* de */ {IDCLS_RESID_GAIN_PERCENTAGE_DE, "reSID Gain in Prozent (90 - 100)"},
/* fr */ {IDCLS_RESID_GAIN_PERCENTAGE_FR, "Gain reSID en pourcent (90 - 100)"},
/* hu */ {IDCLS_RESID_GAIN_PERCENTAGE_HU, "reSID százalékos erõsítés (90 - 100)"},
/* it */ {IDCLS_RESID_GAIN_PERCENTAGE_IT, "Guadagno del reSID in percentuale (90 - 100)"},
/* nl */ {IDCLS_RESID_GAIN_PERCENTAGE_NL, "reSID-versterking procent (90 - 100)"},
/* pl */ {IDCLS_RESID_GAIN_PERCENTAGE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RESID_GAIN_PERCENTAGE_SV, "Gain för reSID i procent (90 - 100)"},
/* tr */ {IDCLS_RESID_GAIN_PERCENTAGE_TR, "reSID gain yüzdesi (90 - 100)"},
#endif
#endif

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_MEMORY_16KB,    N_("Set the VDC memory size to 16KB")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_VDC_MEMORY_16KB_DA, "Sæt VDC-hukommelsesstørrelse til 16KB"},
/* de */ {IDCLS_SET_VDC_MEMORY_16KB_DE, "VDC Speichgröße auf 16KB setzen"},
/* fr */ {IDCLS_SET_VDC_MEMORY_16KB_FR, "Régler la taille de la mémoire VDC à 16KO"},
/* hu */ {IDCLS_SET_VDC_MEMORY_16KB_HU, "VDC memória méret beállítása 16KB-ra"},
/* it */ {IDCLS_SET_VDC_MEMORY_16KB_IT, "Imposta la dimensione della memoria del VDC a 16KB"},
/* nl */ {IDCLS_SET_VDC_MEMORY_16KB_NL, "Zet de VDC-geheugengrootte als 16KB"},
/* pl */ {IDCLS_SET_VDC_MEMORY_16KB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VDC_MEMORY_16KB_SV, "Sätt VDC-minnesstorlek till 16KB"},
/* tr */ {IDCLS_SET_VDC_MEMORY_16KB_TR, "VDC bellek boyutunu 16KB'a ayarla"},
#endif

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_MEMORY_64KB,    N_("Set the VDC memory size to 64KB")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_VDC_MEMORY_64KB_DA, "Sæt VDC-hukommelsesstørrelse til 64KB"},
/* de */ {IDCLS_SET_VDC_MEMORY_64KB_DE, "VDC Speichgröße auf 64KB setzen"},
/* fr */ {IDCLS_SET_VDC_MEMORY_64KB_FR, "Régler la taille de la mémoire VDC à 64KO"},
/* hu */ {IDCLS_SET_VDC_MEMORY_64KB_HU, "VDC memória méret beállítása 64KB-ra"},
/* it */ {IDCLS_SET_VDC_MEMORY_64KB_IT, "Imposta la dimensione della memoria del VDC a 64KB"},
/* nl */ {IDCLS_SET_VDC_MEMORY_64KB_NL, "Zet de VDC-geheugengrootte als 64KB"},
/* pl */ {IDCLS_SET_VDC_MEMORY_64KB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VDC_MEMORY_64KB_SV, "Sätt VDC-minnesstorlek till 64KB"},
/* tr */ {IDCLS_SET_VDC_MEMORY_64KB_TR, "VDC bellek boyutunu 64KB'a ayarla"},
#endif

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_REVISION,    N_("Set VDC revision (0..2)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_VDC_REVISION_DA, "Angiv VDC-revision (0..2)"},
/* de */ {IDCLS_SET_VDC_REVISION_DE, "VDC Revision (0..2) setzen"},
/* fr */ {IDCLS_SET_VDC_REVISION_FR, "Régler la révision VDC (0..2)"},
/* hu */ {IDCLS_SET_VDC_REVISION_HU, "VDC változatszám beállítása (0-2)"},
/* it */ {IDCLS_SET_VDC_REVISION_IT, "Imposta la revisione del VDC (0..2)"},
/* nl */ {IDCLS_SET_VDC_REVISION_NL, "Zet de VDC-revisie (0..2)"},
/* pl */ {IDCLS_SET_VDC_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VDC_REVISION_SV, "Ange VDC-revision (0..2)"},
/* tr */ {IDCLS_SET_VDC_REVISION_TR, "VDC revizyonunu ayarla (0..2)"},
#endif

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_P_SPEC,    N_("<spec>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_SPEC_DA, "<spec>"},
/* de */ {IDCLS_P_SPEC_DE, "<Spec>"},
/* fr */ {IDCLS_P_SPEC_FR, "<spec>"},
/* hu */ {IDCLS_P_SPEC_HU, "<spec>"},
/* it */ {IDCLS_P_SPEC_IT, "<spec>"},
/* nl */ {IDCLS_P_SPEC_NL, "<spec>"},
/* pl */ {IDCLS_P_SPEC_PL, "<spec>"},
/* sv */ {IDCLS_P_SPEC_SV, "<spec>"},
/* tr */ {IDCLS_P_SPEC_TR, "<þart>"},
#endif

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_MEMORY_CONFIG,    N_("Specify memory configuration")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_MEMORY_CONFIG_DA, "Angiv hukommelseskonfiguration"},
/* de */ {IDCLS_SPECIFY_MEMORY_CONFIG_DE, "Speicher Konfiguration definieren"},
/* fr */ {IDCLS_SPECIFY_MEMORY_CONFIG_FR, "Spécifier la configuration de la mémoire"},
/* hu */ {IDCLS_SPECIFY_MEMORY_CONFIG_HU, "Adja meg a memória konfigurációt"},
/* it */ {IDCLS_SPECIFY_MEMORY_CONFIG_IT, "Specifica la configurazione della memoria"},
/* nl */ {IDCLS_SPECIFY_MEMORY_CONFIG_NL, "Geef geheugenconfiguratie"},
/* pl */ {IDCLS_SPECIFY_MEMORY_CONFIG_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_MEMORY_CONFIG_SV, "Ange minneskonfiguration"},
/* tr */ {IDCLS_SPECIFY_MEMORY_CONFIG_TR, "Bellek konfigürasyonunu ayarlayýn"},
#endif

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_ENABLE_VIC1112_IEEE488,    N_("Enable VIC-1112 IEEE488 interface")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_VIC1112_IEEE488_DA, "Aktivér VIC-1112-IEEE488-interface"},
/* de */ {IDCLS_ENABLE_VIC1112_IEEE488_DE, "VIC-1112 IEEE488 Schnittstelle aktivieren"},
/* fr */ {IDCLS_ENABLE_VIC1112_IEEE488_FR, "Activer l'interface VIC-1112 IEEE488"},
/* hu */ {IDCLS_ENABLE_VIC1112_IEEE488_HU, "VIC-1112 IEEE488 interfész engedélyezése"},
/* it */ {IDCLS_ENABLE_VIC1112_IEEE488_IT, "Attiva l'interfaccia IEEE488 del VIC-1112"},
/* nl */ {IDCLS_ENABLE_VIC1112_IEEE488_NL, "Activeer VIC-1112 IEEE488-interface"},
/* pl */ {IDCLS_ENABLE_VIC1112_IEEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_VIC1112_IEEE488_SV, "Aktivera VIC-1112-IEEE488-gränssnitt"},
/* tr */ {IDCLS_ENABLE_VIC1112_IEEE488_TR, "VIC-1112 IEEE488 arabirimini aktif et"},
#endif

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_DISABLE_VIC1112_IEEE488,    N_("Disable VIC-1112 IEEE488 interface")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_VIC1112_IEEE488_DA, "Deaktiver VIC-1112-IEEE488-interface"},
/* de */ {IDCLS_DISABLE_VIC1112_IEEE488_DE, "VIC-1112 IEEE488 Schnittstelle deaktivieren"},
/* fr */ {IDCLS_DISABLE_VIC1112_IEEE488_FR, "Désactiver l'interface VIC-1112 IEEE488"},
/* hu */ {IDCLS_DISABLE_VIC1112_IEEE488_HU, "VIC-1112 IEEE488 interfész tiltása"},
/* it */ {IDCLS_DISABLE_VIC1112_IEEE488_IT, "Disattiva l'interfaccia IEEE488 del VIC-1112"},
/* nl */ {IDCLS_DISABLE_VIC1112_IEEE488_NL, "VIC-1112 IEEE488-interface uitschakelen"},
/* pl */ {IDCLS_DISABLE_VIC1112_IEEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_VIC1112_IEEE488_SV, "Inaktivera VIC-1112-IEEE488-gränssnitt"},
/* tr */ {IDCLS_DISABLE_VIC1112_IEEE488_TR, "VIC-1112 IEEE488 arabirimini pasifleþtir"},
#endif

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME,    N_("Specify 4/8/16K extension ROM name at $2000")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_DA, "Angiv navn for 4/8/16K-udviddelses-ROM på $2000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_DE, "4/8/16K Erweiterungs ROM Datei Name für $2000 definieren"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_FR, "Spécifier le nom de l'extension ROM 4/8/16K à $2000"},
/* hu */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_HU, "Adja meg a $2000 címû 4/8/16K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_IT, "Specifica il nome della ROM di estensione di 4/8/16K a $2000"},
/* nl */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_NL, "Geef de naam van het bestand voor de 4/8/16K-ROM op $2000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_SV, "Ange namn för 4/8/16K-utöknings-ROM på $2000"},
/* tr */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_TR, "$2000 adresindeki 4/8/16K uzantýsý ROM ismini belirt"},
#endif

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME,    N_("Specify 4/8/16K extension ROM name at $4000")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_DA, "Angiv navn for 4/8/16K-udviddelses-ROM på $4000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_DE, "4/8/16K Erweiterungs ROM Datei Name für $4000 definieren"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_FR, "Spécifier le nom de l'extension ROM 4/8/16K à $4000"},
/* hu */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_HU, "Adja meg a $4000 címû 4/8/16K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_IT, "Specifica il nome della ROM di estensione di 4/8/16K a $4000"},
/* nl */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_NL, "Geef de naam van het bestand voor de 4/8/16K-ROM op $4000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_SV, "Ange namn för 4/8/16K-utöknings-ROM på $4000"},
/* tr */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_TR, "$4000 adresindeki 4/8/16K uzantýsý ROM ismini belirt"},
#endif

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME,    N_("Specify 4/8/16K extension ROM name at $6000")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_DA, "Angiv navn for 4/8/16K-udviddelses-ROM på $6000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_DE, "4/8/16K Erweiterungs ROM Datei Name für $6000 definieren"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_FR, "Spécifier le nom de l'extension ROM 4/8/16K à $6000"},
/* hu */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_HU, "Adja meg a $6000 címû 4/8/16K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_IT, "Specifica il nome della ROM di estensione di 4/8/16K a $6000"},
/* nl */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_NL, "Geef de naam van het bestand voor de 4/8/16K-ROM op $6000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_SV, "Ange namn för 4/8/16K-utöknings-ROM på $6000"},
/* tr */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_TR, "$6000 adresindeki 4/8/16K uzantýsý ROM ismini belirt"},
#endif

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME,    N_("Specify 4/8K extension ROM name at $A000")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_DA, "Angiv navn for 4/8K-udviddelses-ROM på $A000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_DE, "4/8K Erweiterungs ROM Datei Name für $A000 definieren"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_FR, "Spécifier le nom de l'extension ROM 4/8K à $A000"},
/* hu */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_HU, "Adja meg a $A000 címû 4/8K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_IT, "Specifica il nome della ROM di estensione di 4/8K a $A000"},
/* nl */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_NL, "Geef de naam van het bestand voor de 4/8K-ROM op $A000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_SV, "Ange namn för 4/8K-utöknings-ROM på $A000"},
/* tr */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_TR, "$A000 adresindeki 4/8K uzantýsý ROM ismini belirt"},
#endif

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME,    N_("Specify 4K extension ROM name at $B000")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_DA, "Angiv navn for 4K-udviddelses-ROM på $B000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_DE, "4K Erweiterungs ROM Datei Name für $B000 definieren"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_FR, "Spécifier le nom de l'extension ROM 4K à $B000"},
/* hu */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_HU, "Adja meg a $B000 címû 4K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_IT, "Specifica il nome della ROM di estensione di 4K a $B000"},
/* nl */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_NL, "Geef de naam van het bestand voor de 4K-ROM op $B000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_SV, "Ange namn för 4K-utöknings-ROM på $B000"},
/* tr */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_TR, "$B000 adresindeki 4K uzantýsý ROM ismini belirt"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_BORDER_MODE,    N_("Set VIC-II border display mode (0: normal, 1: full, 2: debug)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_BORDER_MODE_DA, "Vælg VIC-II-rammevisningstilstand (0: normal, 1: fuld, 2: fejlsøgning)"},
/* de */ {IDCLS_SET_BORDER_MODE_DE, "VIC-II Rahmen Darstellung Modus (0: normal, 1: full, 2: debug)"},
/* fr */ {IDCLS_SET_BORDER_MODE_FR, "Régler le mode de bordure VIC-II (0: normal, 1: complet, 2: debug)"},
/* hu */ {IDCLS_SET_BORDER_MODE_HU, "VIC-II keret megjelenítési mód (0: normál, 1: teljes, 2: hibakeresés)"},
/* it */ {IDCLS_SET_BORDER_MODE_IT, "Imposta la modalità di visualizzazione del bordo del VIC-II (0: normale, 1: "
                                    "intero, 2: debug)"},
/* nl */ {IDCLS_SET_BORDER_MODE_NL, "Zet VIC-II border weergavemodus (0: normaal, 1: volledig, 2: debug)"},
/* pl */ {IDCLS_SET_BORDER_MODE_PL, ""}, /* fuzzy */
/* sv */ {IDCLS_SET_BORDER_MODE_SV, "Välj VIC II-ramvisningsläge (0: normal, 1: full, 2: felsökning)"},
/* tr */ {IDCLS_SET_BORDER_MODE_TR, "VIC-II çerçeve gösterim modunu ayarlayýn (0: normal, 1: tam, 2: hata ayýklama)"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SPRITE_BACKGROUND,    N_("Enable sprite-background collision registers")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SPRITE_BACKGROUND_DA, "Aktivér sprite-til-baggrunds-kollisionsregistre"},
/* de */ {IDCLS_ENABLE_SPRITE_BACKGROUND_DE, "Sprite-Hintergrund Kollisionen aktivieren"},
/* fr */ {IDCLS_ENABLE_SPRITE_BACKGROUND_FR, "Activer les registres de collisions de sprite avec arrière-plan"},
/* hu */ {IDCLS_ENABLE_SPRITE_BACKGROUND_HU, "Sprite-háttér ütközési regiszterek engedélyezése"},
/* it */ {IDCLS_ENABLE_SPRITE_BACKGROUND_IT, "Attiva i registri di collisione sprite-sfondo"},
/* nl */ {IDCLS_ENABLE_SPRITE_BACKGROUND_NL, "Activeer sprite-achtergrond botsing registers"},
/* pl */ {IDCLS_ENABLE_SPRITE_BACKGROUND_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SPRITE_BACKGROUND_SV, "Aktivera sprite-till-bakgrund-kollisionsregister"},
/* tr */ {IDCLS_ENABLE_SPRITE_BACKGROUND_TR, "Yaratýk-arka plan çarpýþma registerlarýný aktif et"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SPRITE_BACKGROUND,    N_("Disable sprite-background collision registers")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SPRITE_BACKGROUND_DA, "Deaktiver sprite-til-baggrunds-kollisionsregistre"},
/* de */ {IDCLS_DISABLE_SPRITE_BACKGROUND_DE, "Sprite-Hintergrund Kollisionen deaktivieren"},
/* fr */ {IDCLS_DISABLE_SPRITE_BACKGROUND_FR, "Désactiver les registres de collisions de sprite avec arrière-plan"},
/* hu */ {IDCLS_DISABLE_SPRITE_BACKGROUND_HU, "Sprite-háttér ütközési regiszterek tiltása"},
/* it */ {IDCLS_DISABLE_SPRITE_BACKGROUND_IT, "Disattiva i registri di collisione sprite-sfondo"},
/* nl */ {IDCLS_DISABLE_SPRITE_BACKGROUND_NL, "Sprite-achtergrond botsing registers uitschakelen"},
/* pl */ {IDCLS_DISABLE_SPRITE_BACKGROUND_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SPRITE_BACKGROUND_SV, "Inaktivera sprite-till-bakgrund-kollisionsregister"},
/* tr */ {IDCLS_DISABLE_SPRITE_BACKGROUND_TR, "Yaratýk-arka plan çarpýþma registerlarýný pasifleþtir"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SPRITE_SPRITE,    N_("Enable sprite-sprite collision registers")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SPRITE_SPRITE_DA, "Aktivér sprite-til-sprite-kollisionsregistre"},
/* de */ {IDCLS_ENABLE_SPRITE_SPRITE_DE, "Sprite-Sprite Kollisionen aktivieren"},
/* fr */ {IDCLS_ENABLE_SPRITE_SPRITE_FR, "Activer les registres de collisions de sprite avec sprite"},
/* hu */ {IDCLS_ENABLE_SPRITE_SPRITE_HU, "Sprite-sprite ütközési regiszterek engedélyezése"},
/* it */ {IDCLS_ENABLE_SPRITE_SPRITE_IT, "Attiva i registri di collisione sprite-sprite"},
/* nl */ {IDCLS_ENABLE_SPRITE_SPRITE_NL, "Activeer sprite-sprite botsing registers"},
/* pl */ {IDCLS_ENABLE_SPRITE_SPRITE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SPRITE_SPRITE_SV, "Aktivera sprite-till-sprite-kollisionsregister"},
/* tr */ {IDCLS_ENABLE_SPRITE_SPRITE_TR, "Yaratýk-yaratýk çarpýþma registerlarýný aktif et"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SPRITE_SPRITE,    N_("Disable sprite-sprite collision registers")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SPRITE_SPRITE_DA, "Deaktiver sprite-til-sprite-kollisionsregistre"},
/* de */ {IDCLS_DISABLE_SPRITE_SPRITE_DE, "Sprite-Sprite Kollisionen deaktivieren"},
/* fr */ {IDCLS_DISABLE_SPRITE_SPRITE_FR, "Désactiver les registres de collisions de sprite avec sprite"},
/* hu */ {IDCLS_DISABLE_SPRITE_SPRITE_HU, "Sprite-sprite ütközési regiszterek tiltása"},
/* it */ {IDCLS_DISABLE_SPRITE_SPRITE_IT, "Disattiva i registri di collisione sprite-sprite"},
/* nl */ {IDCLS_DISABLE_SPRITE_SPRITE_NL, "Sprite-sprite botsing registers uitschakelen"},
/* pl */ {IDCLS_DISABLE_SPRITE_SPRITE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SPRITE_SPRITE_SV, "Inaktivera sprite-till-sprite-kollisionsregister"},
/* tr */ {IDCLS_DISABLE_SPRITE_SPRITE_TR, "Yaratýk-yaratýk çarpýþma registerlarýný pasifleþtir"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_USE_NEW_LUMINANCES,    N_("Use new luminances")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_NEW_LUMINANCES_DA, "Brug nye lysstyrker"},
/* de */ {IDCLS_USE_NEW_LUMINANCES_DE, "Neue Helligkeitsemulation"},
/* fr */ {IDCLS_USE_NEW_LUMINANCES_FR, "Utiliser les nouvelles luminescences"},
/* hu */ {IDCLS_USE_NEW_LUMINANCES_HU, "Az új fényerõket használja"},
/* it */ {IDCLS_USE_NEW_LUMINANCES_IT, "Usa nuove luminanze"},
/* nl */ {IDCLS_USE_NEW_LUMINANCES_NL, "Gebruik nieuwe kleuren"},
/* pl */ {IDCLS_USE_NEW_LUMINANCES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_NEW_LUMINANCES_SV, "Använd nya ljusstyrkor"},
/* tr */ {IDCLS_USE_NEW_LUMINANCES_TR, "Yeni parlaklýklarý kullan"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_USE_OLD_LUMINANCES,    N_("Use old luminances")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_OLD_LUMINANCES_DA, "Brug gamle lysstyrker"},
/* de */ {IDCLS_USE_OLD_LUMINANCES_DE, "Alte Helligkeitsemulation"},
/* fr */ {IDCLS_USE_OLD_LUMINANCES_FR, "Utiliser les anciennes luminescences"},
/* hu */ {IDCLS_USE_OLD_LUMINANCES_HU, "A régi fényerõket használja"},
/* it */ {IDCLS_USE_OLD_LUMINANCES_IT, "Usa vecchie luminanze"},
/* nl */ {IDCLS_USE_OLD_LUMINANCES_NL, "Gebruik oude kleuren"},
/* pl */ {IDCLS_USE_OLD_LUMINANCES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_OLD_LUMINANCES_SV, "Använd gamla ljusstyrkor"},
/* tr */ {IDCLS_USE_OLD_LUMINANCES_TR, "Eski parlaklýklarý kullan"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SIZE,    N_("Enable double size")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DOUBLE_SIZE_DA, "Aktivér dobbelt størrelse"},
/* de */ {IDCLS_ENABLE_DOUBLE_SIZE_DE, "Doppelte Größe aktivieren"},
/* fr */ {IDCLS_ENABLE_DOUBLE_SIZE_FR, "Taille double"},
/* hu */ {IDCLS_ENABLE_DOUBLE_SIZE_HU, "Dupla méret engedélyezése"},
/* it */ {IDCLS_ENABLE_DOUBLE_SIZE_IT, "Attiva la dimensione doppia"},
/* nl */ {IDCLS_ENABLE_DOUBLE_SIZE_NL, "Activeer dubbele grootte"},
/* pl */ {IDCLS_ENABLE_DOUBLE_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DOUBLE_SIZE_SV, "Aktivera dubbel storlek"},
/* tr */ {IDCLS_ENABLE_DOUBLE_SIZE_TR, "Çift boyutu aktif et"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SIZE,    N_("Disable double size")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DOUBLE_SIZE_DA, "Deaktiver dobbelt størrelse"},
/* de */ {IDCLS_DISABLE_DOUBLE_SIZE_DE, "Doppelte Größe deaktivieren"},
/* fr */ {IDCLS_DISABLE_DOUBLE_SIZE_FR, "Taille normale"},
/* hu */ {IDCLS_DISABLE_DOUBLE_SIZE_HU, "Dupla méret tiltása"},
/* it */ {IDCLS_DISABLE_DOUBLE_SIZE_IT, "Disattiva la dimensione doppia"},
/* nl */ {IDCLS_DISABLE_DOUBLE_SIZE_NL, "Dubbele grootte uitschakelen"},
/* pl */ {IDCLS_DISABLE_DOUBLE_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DOUBLE_SIZE_SV, "Inaktivera dubbel storlek"},
/* tr */ {IDCLS_DISABLE_DOUBLE_SIZE_TR, "Çift boyutu pasifleþtir"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SCAN,    N_("Enable double scan")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DOUBLE_SCAN_DA, "Aktivér dobbelt-skan"},
/* de */ {IDCLS_ENABLE_DOUBLE_SCAN_DE, "Doppelt Scan aktivieren"},
/* fr */ {IDCLS_ENABLE_DOUBLE_SCAN_FR, "Activer le mode double scan"},
/* hu */ {IDCLS_ENABLE_DOUBLE_SCAN_HU, "Dupla pásztázás engedélyezése"},
/* it */ {IDCLS_ENABLE_DOUBLE_SCAN_IT, "Attiva la scansione doppia"},
/* nl */ {IDCLS_ENABLE_DOUBLE_SCAN_NL, "Activeer dubbele scan"},
/* pl */ {IDCLS_ENABLE_DOUBLE_SCAN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DOUBLE_SCAN_SV, "Aktivera dubbelskanning"},
/* tr */ {IDCLS_ENABLE_DOUBLE_SCAN_TR, "Çift taramayý aktif et"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SCAN,    N_("Disable double scan")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DOUBLE_SCAN_DA, "Deaktiver dobbelt-skan"},
/* de */ {IDCLS_DISABLE_DOUBLE_SCAN_DE, "Doppelt Scan deaktivieren"},
/* fr */ {IDCLS_DISABLE_DOUBLE_SCAN_FR, "Désactiver le mode double scan"},
/* hu */ {IDCLS_DISABLE_DOUBLE_SCAN_HU, "Dupla pásztázás tiltása"},
/* it */ {IDCLS_DISABLE_DOUBLE_SCAN_IT, "Disattiva la scansione doppia"},
/* nl */ {IDCLS_DISABLE_DOUBLE_SCAN_NL, "Dubbele scan uitschakelen"},
/* pl */ {IDCLS_DISABLE_DOUBLE_SCAN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DOUBLE_SCAN_SV, "Inaktivera dubbelskanning"},
/* tr */ {IDCLS_DISABLE_DOUBLE_SCAN_TR, "Çift taramayý pasifleþtir"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_HARDWARE_SCALING,    N_("Enable hardware scaling")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_HARDWARE_SCALING_DA, "Aktivér hardware-skalering"},
/* de */ {IDCLS_ENABLE_HARDWARE_SCALING_DE, "Hardwareunterstützung für Skalierung aktivieren"},
/* fr */ {IDCLS_ENABLE_HARDWARE_SCALING_FR, "Activer le \"scaling\" matériel"},
/* hu */ {IDCLS_ENABLE_HARDWARE_SCALING_HU, "Hardver átméretezés engedélyezése"},
/* it */ {IDCLS_ENABLE_HARDWARE_SCALING_IT, "Attiva l'hardware scaling"},
/* nl */ {IDCLS_ENABLE_HARDWARE_SCALING_NL, "Activeer hardwareschaling"},
/* pl */ {IDCLS_ENABLE_HARDWARE_SCALING_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_HARDWARE_SCALING_SV, "Aktivera maskinvaruskalning"},
/* tr */ {IDCLS_ENABLE_HARDWARE_SCALING_TR, "Donaným destekli ölçeklendirmeyi aktif et"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_HARDWARE_SCALING,    N_("Disable hardware scaling")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_HARDWARE_SCALING_DA, "Deaktiver hardware-skalering"},
/* de */ {IDCLS_DISABLE_HARDWARE_SCALING_DE, "Hardwareunterstützung für Skalierung deaktivieren"},
/* fr */ {IDCLS_DISABLE_HARDWARE_SCALING_FR, "Désactiver le \"scaling\" matériel"},
/* hu */ {IDCLS_DISABLE_HARDWARE_SCALING_HU, "Hardver átméretezés tiltása"},
/* it */ {IDCLS_DISABLE_HARDWARE_SCALING_IT, "Disattiva l'hardware scaling"},
/* nl */ {IDCLS_DISABLE_HARDWARE_SCALING_NL, "Hardwareschaling uitschakelen"},
/* pl */ {IDCLS_DISABLE_HARDWARE_SCALING_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_HARDWARE_SCALING_SV, "Inaktivera maskinvaruskalning"},
/* tr */ {IDCLS_DISABLE_HARDWARE_SCALING_TR, "Donaným destekli ölçeklendirmeyi pasifleþtir"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SCALE2X,    N_("Enable Scale2x")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SCALE2X_DA, "Aktivér Scale2x"},
/* de */ {IDCLS_ENABLE_SCALE2X_DE, "Scale2x aktivieren"},
/* fr */ {IDCLS_ENABLE_SCALE2X_FR, "Activer Scale2x"},
/* hu */ {IDCLS_ENABLE_SCALE2X_HU, "Élsimítás engedélyezése"},
/* it */ {IDCLS_ENABLE_SCALE2X_IT, "Attiva Scale2x"},
/* nl */ {IDCLS_ENABLE_SCALE2X_NL, "Activeer Schaal2x"},
/* pl */ {IDCLS_ENABLE_SCALE2X_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SCALE2X_SV, "Aktivera Scale2x"},
/* tr */ {IDCLS_ENABLE_SCALE2X_TR, "2x Geniþlet'i aktif et"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SCALE2X,    N_("Disable Scale2x")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SCALE2X_DA, "Deaktiver Scale2x"},
/* de */ {IDCLS_DISABLE_SCALE2X_DE, "Scale2x deaktivieren"},
/* fr */ {IDCLS_DISABLE_SCALE2X_FR, "Désactiver Scale2x"},
/* hu */ {IDCLS_DISABLE_SCALE2X_HU, "Élsimítás tiltása"},
/* it */ {IDCLS_DISABLE_SCALE2X_IT, "Disattiva Scale2x"},
/* nl */ {IDCLS_DISABLE_SCALE2X_NL, "Schaal2x uitschakelen"},
/* pl */ {IDCLS_DISABLE_SCALE2X_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SCALE2X_SV, "Inaktivera Scale2x"},
/* tr */ {IDCLS_DISABLE_SCALE2X_TR, "2x Geniþlet'i pasifleþtir"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_USE_INTERNAL_CALC_PALETTE,    N_("Use an internal calculated palette")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_INTERNAL_CALC_PALETTE_DA, "Brug en internt beregnet palette"},
/* de */ {IDCLS_USE_INTERNAL_CALC_PALETTE_DE, "Benutzse intern berechnete Palette"},
/* fr */ {IDCLS_USE_INTERNAL_CALC_PALETTE_FR, "Utiliser une palette interne calculée"},
/* hu */ {IDCLS_USE_INTERNAL_CALC_PALETTE_HU, "Belsõleg számított paletta használata"},
/* it */ {IDCLS_USE_INTERNAL_CALC_PALETTE_IT, "Usa una palette interna calcolata"},
/* nl */ {IDCLS_USE_INTERNAL_CALC_PALETTE_NL, "Gebruik een intern berekend kleurenpalet"},
/* pl */ {IDCLS_USE_INTERNAL_CALC_PALETTE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_INTERNAL_CALC_PALETTE_SV, "Använd en internt beräknad palett"},
/* tr */ {IDCLS_USE_INTERNAL_CALC_PALETTE_TR, "Dahili hesaplanmýþ bir palet kullan"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_USE_EXTERNAL_FILE_PALETTE,    N_("Use an external palette (file)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_DA, "Brug en ekstern palette (fil)"},
/* de */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_DE, "Benutze externe Palette (Datei)"},
/* fr */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_FR, "Utiliser une palette externe (fichier)"},
/* hu */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_HU, "Külsõ paletta használata (fájl)"},
/* it */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_IT, "Usa una palette esterna (file)"},
/* nl */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_NL, "Gebruik een extern kleurenpalet (bestand)"},
/* pl */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_SV, "Använd en extern palett (fil)"},
/* tr */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_TR, "Harici bir palet (dosyadan) kullan"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME,    N_("Specify name of file of external palette")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_DA, "Angiv filnavn for ekstern palette"},
/* de */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_DE, "Dateiname für externe Palette definieren"},
/* fr */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_FR, "Spécifier le nom du fichier de la palette externe"},
/* hu */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_HU, "Adja meg a külsõ paletta nevét"},
/* it */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_IT, "Specifica il nome del file della palette esterna"},
/* nl */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_NL, "Geef de naam van het extern kleurenpaletbestand"},
/* pl */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_SV, "Ange namn på fil för extern palett"},
/* tr */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_TR, "Harici palet dosyasýnýn ismini belirt"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FULLSCREEN_MODE,    N_("Enable fullscreen mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FULLSCREEN_MODE_DA, "Aktivér fuldskærmstilstand"},
/* de */ {IDCLS_ENABLE_FULLSCREEN_MODE_DE, "Vollbild Modus aktivieren"},
/* fr */ {IDCLS_ENABLE_FULLSCREEN_MODE_FR, "Activer le mode plein écran"},
/* hu */ {IDCLS_ENABLE_FULLSCREEN_MODE_HU, "Teljesképernyõs mód engedélyezése"},
/* it */ {IDCLS_ENABLE_FULLSCREEN_MODE_IT, "Visualizza a tutto schermo"},
/* nl */ {IDCLS_ENABLE_FULLSCREEN_MODE_NL, "Activeer volschermmodus"},
/* pl */ {IDCLS_ENABLE_FULLSCREEN_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FULLSCREEN_MODE_SV, "Aktivera fullskärmsläge"},
/* tr */ {IDCLS_ENABLE_FULLSCREEN_MODE_TR, "Tam ekran modunu aktif et"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FULLSCREEN_MODE,    N_("Disable fullscreen mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FULLSCREEN_MODE_DA, "Deaktiver fuldskærmstilstand"},
/* de */ {IDCLS_DISABLE_FULLSCREEN_MODE_DE, "Vollbild Modus deaktivieren"},
/* fr */ {IDCLS_DISABLE_FULLSCREEN_MODE_FR, "Désactiver le mode plein écran"},
/* hu */ {IDCLS_DISABLE_FULLSCREEN_MODE_HU, "Teljesképernyõs mód tiltása"},
/* it */ {IDCLS_DISABLE_FULLSCREEN_MODE_IT, "Non visualizzare a tutto schermo"},
/* nl */ {IDCLS_DISABLE_FULLSCREEN_MODE_NL, "Volschermmodus uitschakelen"},
/* pl */ {IDCLS_DISABLE_FULLSCREEN_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FULLSCREEN_MODE_SV, "Inaktivera fullskärmsläge"},
/* tr */ {IDCLS_DISABLE_FULLSCREEN_MODE_TR, "Tam ekran modunu pasifleþtir"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_P_DEVICE,    N_("<device>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_DEVICE_DA, "<enhed>"},
/* de */ {IDCLS_P_DEVICE_DE, "<Gerät>"},
/* fr */ {IDCLS_P_DEVICE_FR, "<périphérique>"},
/* hu */ {IDCLS_P_DEVICE_HU, "<eszköz>"},
/* it */ {IDCLS_P_DEVICE_IT, "<dispositivo>"},
/* nl */ {IDCLS_P_DEVICE_NL, "<apparaat>"},
/* pl */ {IDCLS_P_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_DEVICE_SV, "<enhet>"},
/* tr */ {IDCLS_P_DEVICE_TR, "<aygýt>"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_SELECT_FULLSCREEN_DEVICE,    N_("Select fullscreen device")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SELECT_FULLSCREEN_DEVICE_DA, "Angiv fuldskærmsenhed"},
/* de */ {IDCLS_SELECT_FULLSCREEN_DEVICE_DE, "Vollbild Gerät selektieren"},
/* fr */ {IDCLS_SELECT_FULLSCREEN_DEVICE_FR, "Sélectionner le périphérique plein écran"},
/* hu */ {IDCLS_SELECT_FULLSCREEN_DEVICE_HU, "Teljesképernyõs eszköz kiválasztása"},
/* it */ {IDCLS_SELECT_FULLSCREEN_DEVICE_IT, "Seleziona il dispositivo per la visualizzazione a tutto schermo"},
/* nl */ {IDCLS_SELECT_FULLSCREEN_DEVICE_NL, "Selecteer volschermapparaat"},
/* pl */ {IDCLS_SELECT_FULLSCREEN_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SELECT_FULLSCREEN_DEVICE_SV, "Ange fullskärmsenhet"},
/* tr */ {IDCLS_SELECT_FULLSCREEN_DEVICE_TR, "Tam ekran aygýtý seç"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN,    N_("Enable double size in fullscreen mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_DA, "Aktivér dobbelt størrelse i fuldskærms-tilstand"},
/* de */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_DE, "Doppelte Größe im Vollbild Modus aktivieren"},
/* fr */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_FR, "Activer \"Taille double\" en plein écran"},
/* hu */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_HU, "Dupla méret engedélyezése teljesképernyõs módban"},
/* it */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_IT, "Attiva la dimensione doppia nella visualizzazione a tutto schermo"},
/* nl */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_NL, "Activeer dubbele grootte in volschermmodus"},
/* pl */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_SV, "Aktivera dubbel storlek i fullskärmsläge"},
/* tr */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_TR, "Tam ekran modunda çift boyutu aktif et"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN,    N_("Disable double size in fullscreen mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_DA, "Deaktiver dobbelt størrelse i fuldskærms-tilstand"},
/* de */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_DE, "Doppelte Große im Vollbild Modus deaktivieren"},
/* fr */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_FR, "Désactiver \"Taille double\" en plein écran"},
/* hu */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_HU, "Dupla méret tiltása teljesképernyõs módban"},
/* it */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_IT, "Disattiva la dimensione doppia nella visualizzazione a tutto schermo"},
/* nl */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_NL, "Dubbele grootte in volschermmodus uitschakelen"},
/* pl */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_SV, "Inaktivera dubbel storlek i fullskärmsläge"},
/* tr */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_TR, "Tam ekran modunda çift boyutu pasifleþtir"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN,    N_("Enable double scan in fullscreen mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_DA, "Aktivér dobbelt-skan i fuldskærms-tilstand"},
/* de */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_DE, "Doppelt Scan im Vollbild Modus aktivieren"},
/* fr */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_FR, "Activer \"Double scan\" en plein écran"},
/* hu */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_HU, "Dupla pásztázás engedélyezése teljesképernyõs módban"},
/* it */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_IT, "Attiva la scansione doppia nella visualizzazione a tutto schermo"},
/* nl */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_NL, "Activeer dubbele scan in volschermmodus"},
/* pl */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_SV, "Aktivera dubbelskanning i fullskärmsläge"},
/* tr */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_TR, "Tam ekran modunda çift taramayý aktif et"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN,    N_("Disable double scan in fullscreen mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_DA, "Deaktiver dobbelt-skan i fuldskærms-tilstand"},
/* de */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_DE, "Doppelt Scan im Vollbild Modus deaktivieren"},
/* fr */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_FR, "Désactiver \"Double scan\" en plein écran"},
/* hu */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_HU, "Dupla pásztázás tiltása teljesképernyõs módban"},
/* it */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_IT, "Disattiva la scansione doppia nella visualizzazione a tutto schermo"},
/* nl */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_NL, "Dubbele scan in volschermmodus uitschakelen"},
/* pl */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_SV, "Inaktivera dubbelskanning i fullskärmsläge"},
/* tr */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_TR, "Tam ekran modunda çift taramayý pasifleþtir"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_P_MODE,    N_("<mode>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_MODE_DA, "<tilstand>"},
/* de */ {IDCLS_P_MODE_DE, "<Modus>"},
/* fr */ {IDCLS_P_MODE_FR, "<mode>"},
/* hu */ {IDCLS_P_MODE_HU, "<mód>"},
/* it */ {IDCLS_P_MODE_IT, "<modalità>"},
/* nl */ {IDCLS_P_MODE_NL, "<modus>"},
/* pl */ {IDCLS_P_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_MODE_SV, "<läge>"},
/* tr */ {IDCLS_P_MODE_TR, "<mod>"},
#endif

/* video/video-cmdline-options.c */
/* en */ {IDCLS_SELECT_FULLSCREEN_MODE,    N_("Select fullscreen mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SELECT_FULLSCREEN_MODE_DA, "Aktivér fuldskærms-tilstand"},
/* de */ {IDCLS_SELECT_FULLSCREEN_MODE_DE, "Vollbild Modus wählen"},
/* fr */ {IDCLS_SELECT_FULLSCREEN_MODE_FR, "Sélectionner le mode plein écran"},
/* hu */ {IDCLS_SELECT_FULLSCREEN_MODE_HU, "Teljes képernyõs mód kiválasztása"},
/* it */ {IDCLS_SELECT_FULLSCREEN_MODE_IT, "Seleziona la modalità di visualizzazione a tutto schermo"},
/* nl */ {IDCLS_SELECT_FULLSCREEN_MODE_NL, "Selecteer volschermmodus"},
/* pl */ {IDCLS_SELECT_FULLSCREEN_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SELECT_FULLSCREEN_MODE_SV, "Aktivera fullskärmsläge"},
/* tr */ {IDCLS_SELECT_FULLSCREEN_MODE_TR, "Tam ekran modu seç"},
#endif

/* aciacore.c */
/* en */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE,    N_("Specify RS232 device this ACIA should work on")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_DA, "Angiv RS232-enhed denne ACIA skal bruge"},
/* de */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_DE, "RS232 Gerät für welches ACIA funktionieren soll ist zu spezifizieren"},
/* fr */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_FR, "Spécifier le périphérique RS232 sur lequel cet ACIA doit fonctionner"},
/* hu */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_HU, "Adja meg az RS232-es egységet, amivel az ACIA mûködni fog"},
/* it */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_IT, "Specifica il dispositivo RS232 con il quale questa ACIA dovrebbe "
                                              "funzionare"},
/* nl */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_NL, "Geef het RS232-apparaat waarmee deze ACIA moet werken"},
/* pl */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_SV, "Ange RS232-enhet denna ACIA skall arbeta på"},
/* tr */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_TR, "Bu ACIA'in çalýþmasý gereken RS232 aygýtýný belirt"},
#endif

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_8,    N_("Set device type for device #8 (0: NONE, 1: FILESYSTEM, 2: OPENCBM, 3: "
                                         "BLOCK DEVICE)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DEVICE_TYPE_8_DA, "Vælg type for enhed #8 (0: INGEN, 1: FILSYSTEM, 2: OPENCBM, 3: BLOKENHED)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_8_DE, "Geräte Typ für Gerät #8 (0: Kein, 1: Dateisystem, 2: OpenCBM, 3: Block Gerät)"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_8_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_DEVICE_TYPE_8_HU, "Adja meg a #8-es egység típusát (0: Nincs, 1: fájlrendszer, 2: OPENCBM, 3 "
                                      "blokk eszköz)"},
/* it */ {IDCLS_SET_DEVICE_TYPE_8_IT, "Imposta il tipo di periferica #8 (0 NESSUNA, 1: FILESYSTEM, 2: OPENCBM, 3: "
                                      "DISPOSITIVO A BLOCCHI)"},
/* nl */ {IDCLS_SET_DEVICE_TYPE_8_NL, "Zet het apparaatsoort voor apparaat #8 (0: GEEN, 1: FILESYSTEM, 2: OPENCBM, 3: "
                                      "BLOCK APPARAAT)"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_8_SV, "Ställ in enhetstyp för enhet 8 (0: INGEN, 1: FILSYSTEM, 2: OPENCBM, 3: BLOCKENHET)"},
/* tr */ {IDCLS_SET_DEVICE_TYPE_8_TR, "Aygýt #8 için aygýt tipini belirt (0: YOK, 1: DOSYASÝSTEMÝ, 2: OPENCBM, 3: "
                                      "BLOK AYIGTI)"},
#endif

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_9,    N_("Set device type for device #9 (0: NONE, 1: FILESYSTEM, 2: OPENCBM, 3: "
                                         "BLOCK DEVICE)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DEVICE_TYPE_9_DA, "Vælg type for enhed #9 (0: INGEN, 1: FILSYSTEM, 2: OPENCBM, 3: BLOKENHED)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_9_DE, "Geräte Typ für Gerät #9 (0: Kein, 1: Dateisystem, 2: OpenCBM, 3: Block Gerät)"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_9_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_DEVICE_TYPE_9_HU, "Adja meg a #9-es egység típusát (0: Nincs, 1: fájlrendszer, 2: OPENCBM, 3 "
                                      "blokk eszköz)"},
/* it */ {IDCLS_SET_DEVICE_TYPE_9_IT, "Imposta il tipo di periferica #9 (0 NESSUNA, 1: FILESYSTEM, 2: OPENCBM, 3: "
                                      "DISPOSITIVO A BLOCCHI)"},
/* nl */ {IDCLS_SET_DEVICE_TYPE_9_NL, "Zet het apparaatsoort voor apparaat #9 (0: GEEN, 1: FILESYSTEM, 2: OPENCBM, 3: "
                                      "BLOCK APPARAAT)"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_9_SV, "Ställ in enhetstyp för enhet 9 (0: INGEN, 1: FILSYSTEM, 2: OPENCBM, 3: BLOCKENHET)"},
/* tr */ {IDCLS_SET_DEVICE_TYPE_9_TR, "Aygýt #9 için aygýt tipini belirt (0: YOK, 1: DOSYASÝSTEMÝ, 2: OPENCBM, 3: "
                                      "BLOK AYIGTI)"},
#endif

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_10,    N_("Set device type for device #10 (0: NONE, 1: FILESYSTEM, 2: OPENCBM, 3: BLOCK "
                                          "DEVICE)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DEVICE_TYPE_10_DA, "Vælg type for enhed #10 (0: INGEN, 1: FILSYSTEM, 2: OPENCBM, 3: BLOKENHED)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_10_DE, "Geräte Typ für Gerät #10 (0: Kein, 1: Dateisystem, 2: OpenCBM, 3: Block Gerät)"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_10_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_DEVICE_TYPE_10_HU, "Adja meg a #10-es egység típusát (0: Nincs, 1: fájlrendszer, 2: OPENCBM, 3 "
                                       "blokk eszköz)"},
/* it */ {IDCLS_SET_DEVICE_TYPE_10_IT, "Imposta il tipo di periferica #10 (0 NESSUNA, 1: FILESYSTEM, 2: OPENCBM, 3: "
                                      "DISPOSITIVO A BLOCCHI)"},
/* nl */ {IDCLS_SET_DEVICE_TYPE_10_NL, "Zet het apparaatsoort voor apparaat #10 (0: GEEN, 1: FILESYSTEM, 2: OPENCBM, 3: "
                                       "BLOCK APPARAAT)"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_10_SV, "Ställ in enhetstyp för enhet 10 (0: INGEN, 1: FILSYSTEM, 2: OPENCBM, 3: BLOCKENHET)"},
/* tr */ {IDCLS_SET_DEVICE_TYPE_10_TR, "Aygýt #10 için aygýt tipini belirt (0: YOK, 1: DOSYASÝSTEMÝ, 2: OPENCBM, 3: "
                                       "BLOK AYIGTI)"},
#endif

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_11,    N_("Set device type for device #11 (0: NONE, 1: FILESYSTEM, 2: OPENCBM, 3: BLOCK "
                                          "DEVICE)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DEVICE_TYPE_11_DA, "Vælg type for enhed #11 (0: INGEN, 1: FILSYSTEM, 2: OPENCBM, 3: BLOKENHED)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_11_DE, "Geräte Typ für Gerät #11 (0: Kein, 1: Dateisystem, 2: OpenCBM, 3: Block Gerät)"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_11_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_DEVICE_TYPE_11_HU, "Adja meg a #11-es egység típusát (0: Nincs, 1: fájlrendszer, 2: OPENCBM, 3 "
                                       "blokk eszköz)"},
/* it */ {IDCLS_SET_DEVICE_TYPE_11_IT, "Imposta il tipo di periferica #11 (0 NESSUNA, 1: FILESYSTEM, 2: OPENCBM, 3: "
                                      "DISPOSITIVO A BLOCCHI)"},
/* nl */ {IDCLS_SET_DEVICE_TYPE_11_NL, "Zet het apparaatsoort voor apparaat #11 (0: GEEN, 1: FILESYSTEM, 2: OPENCBM, 3: "
                                       "BLOCK APPARAAT)"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_11_SV, "Ställ in enhetstyp för enhet 11 (0: INGEN, 1: FILSYSTEM, 2: OPENCBM, 3: BLOCKENHET)"},
/* tr */ {IDCLS_SET_DEVICE_TYPE_11_TR, "Aygýt #11 için aygýt tipini belirt (0: YOK, 1: DOSYASÝSTEMÝ, 2: OPENCBM, 3: "
                                       "BLOK AYIGTI)"},
#endif

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_8,    N_("Attach disk image for drive #8 read only")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_READ_ONLY_8_DA, "Tilslut disk-image til drev #8 skrivebeskyttet"},
/* de */ {IDCLS_ATTACH_READ_ONLY_8_DE, "Disk Image als Laufwerk #8 benutzen (schreibgeschützt)"},
/* fr */ {IDCLS_ATTACH_READ_ONLY_8_FR, "Charger une image de disque pour le lecteur #8 en lecture seule"},
/* hu */ {IDCLS_ATTACH_READ_ONLY_8_HU, "Csak olvasható képmás csatolása a #8-as lemezegységhez"},
/* it */ {IDCLS_ATTACH_READ_ONLY_8_IT, "Seleziona l'immagine del disco per il drive #8 in sola lettura"},
/* nl */ {IDCLS_ATTACH_READ_ONLY_8_NL, "Koppel schijfbestand voor drive #8 als alleen-lezen"},
/* pl */ {IDCLS_ATTACH_READ_ONLY_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_ONLY_8_SV, "Anslut diskettavbildningsfil för enhet 8 med skrivskydd"},
/* tr */ {IDCLS_ATTACH_READ_ONLY_8_TR, "Sürücü #8 için disk imajýný salt okunur olarak yerleþtir"},
#endif

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_8,    N_("Attach disk image for drive #8 read write (if possible)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_READ_WRITE_8_DA, "Tilslut disk-image til drev #8 skrivbar (hvis muligt)"},
/* de */ {IDCLS_ATTACH_READ_WRITE_8_DE, "Disk Image als Laufwerk #8 benutzen (Schreibzugriff, wenn möglich)"},
/* fr */ {IDCLS_ATTACH_READ_WRITE_8_FR, "Charger une image de disque pour le lecteur #8 en lecture-écriture (si "
                                        "possible)"},
/* hu */ {IDCLS_ATTACH_READ_WRITE_8_HU, "Írható-olvasható képmás csatolása a #8-as lemezegységhez"},
/* it */ {IDCLS_ATTACH_READ_WRITE_8_IT, "Seleziona l'immagine del disco per il drive #8 in lettura/scrittura (se "
                                        "possibile)"},
/* nl */ {IDCLS_ATTACH_READ_WRITE_8_NL, "Koppel schijfbestand voor drive #8 als schrijfbaar (indien mogelijk)"},
/* pl */ {IDCLS_ATTACH_READ_WRITE_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_WRITE_8_SV, "Anslut diskettavbildningsfil för enhet 8 skrivbar (om möjligt)"},
/* tr */ {IDCLS_ATTACH_READ_WRITE_8_TR, "Sürücü #8 için disk imajýný okunur/yazýlýr olarak yerleþtir (mümkünse)"},
#endif

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_9,    N_("Attach disk image for drive #9 read only")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_READ_ONLY_9_DA, "Tilslut disk-image til drev #9 skrivebeskyttet"},
/* de */ {IDCLS_ATTACH_READ_ONLY_9_DE, "Disk Image als Laufwerk #9 benutzen (schreibgeschützt)"},
/* fr */ {IDCLS_ATTACH_READ_ONLY_9_FR, "Charger une image de disque pour le lecteur #9 en lecture seule"},
/* hu */ {IDCLS_ATTACH_READ_ONLY_9_HU, "Csak olvasható képmás csatolása a #9-es lemezegységhez"},
/* it */ {IDCLS_ATTACH_READ_ONLY_9_IT, "Seleziona l'immagine del disco per il drive #9 in sola lettura"},
/* nl */ {IDCLS_ATTACH_READ_ONLY_9_NL, "Koppel schijfbestand voor drive #9 als alleen-lezen"},
/* pl */ {IDCLS_ATTACH_READ_ONLY_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_ONLY_9_SV, "Anslut diskettavbildningsfil för enhet 9 med skrivskydd"},
/* tr */ {IDCLS_ATTACH_READ_ONLY_9_TR, "Sürücü #9 için disk imajýný salt okunur olarak yerleþtir"},
#endif

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_9,    N_("Attach disk image for drive #9 read write (if possible)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_READ_WRITE_9_DA, "Tilslut disk-image til drev #9 skrivbar (hvis muligt)"},
/* de */ {IDCLS_ATTACH_READ_WRITE_9_DE, "Disk Image als Laufwerk #9 benutzen (Schreibzugriff, wenn möglich)"},
/* fr */ {IDCLS_ATTACH_READ_WRITE_9_FR, "Charger une image de disque pour le lecteur #9 en lecture-écriture (si "
                                        "possible)"},
/* hu */ {IDCLS_ATTACH_READ_WRITE_9_HU, "Írható-olvasható képmás csatolása a #9-es lemezegységhez"},
/* it */ {IDCLS_ATTACH_READ_WRITE_9_IT, "Seleziona l'immagine del disco per il drive #9 in lettura/scrittura (se "
                                        "possibile)"},
/* nl */ {IDCLS_ATTACH_READ_WRITE_9_NL, "Koppel schijfbestand voor drive #9 als schrijfbaar (indien mogelijk)"},
/* pl */ {IDCLS_ATTACH_READ_WRITE_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_WRITE_9_SV, "Anslut diskettavbildningsfil för enhet 9 skrivbar (om möjligt)"},
/* tr */ {IDCLS_ATTACH_READ_WRITE_9_TR, "Sürücü #9 için disk imajýný okunur/yazýlýr olarak yerleþtir (mümkünse)"},
#endif

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_10,    N_("Attach disk image for drive #10 read only")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_READ_ONLY_10_DA, "Tilslut disk-image til drev #10 skrivebeskyttet"},
/* de */ {IDCLS_ATTACH_READ_ONLY_10_DE, "Disk Image als Laufwerk #10 benutzen (schreibgeschützt)"},
/* fr */ {IDCLS_ATTACH_READ_ONLY_10_FR, "Charger une image de disque pour le lecteur #10 en lecture seule"},
/* hu */ {IDCLS_ATTACH_READ_ONLY_10_HU, "Csak olvasható képmás csatolása a #10-es lemezegységhez"},
/* it */ {IDCLS_ATTACH_READ_ONLY_10_IT, "Seleziona l'immagine del disco per il drive #10 in sola lettura"},
/* nl */ {IDCLS_ATTACH_READ_ONLY_10_NL, "Koppel schijfbestand voor drive #10 als alleen-lezen"},
/* pl */ {IDCLS_ATTACH_READ_ONLY_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_ONLY_10_SV, "Anslut diskettavbildningsfil för enhet 10 med skrivskydd"},
/* tr */ {IDCLS_ATTACH_READ_ONLY_10_TR, "Sürücü #10 için disk imajýný salt okunur olarak yerleþtir"},
#endif

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_10,    N_("Attach disk image for drive #10 read write (if possible)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_READ_WRITE_10_DA, "Tilslut disk-image til drev #10 skrivbar (hvis muligt)"},
/* de */ {IDCLS_ATTACH_READ_WRITE_10_DE, "Disk Image als Laufwerk #10 benutzen (Schreibzugriff, wenn möglich)"},
/* fr */ {IDCLS_ATTACH_READ_WRITE_10_FR, "Charger une image de disque pour le lecteur #10 en lecture-écriture (si "
                                         "possible)"},
/* hu */ {IDCLS_ATTACH_READ_WRITE_10_HU, "Írható-olvasható képmás csatolása a #10-es lemezegységhez"},
/* it */ {IDCLS_ATTACH_READ_WRITE_10_IT, "Seleziona l'immagine del disco per il drive #10 in lettura/scrittura (se "
                                         "possibile)"},
/* nl */ {IDCLS_ATTACH_READ_WRITE_10_NL, "Koppel schijfbestand voor drive #10 als schrijfbaar (indien mogelijk)"},
/* pl */ {IDCLS_ATTACH_READ_WRITE_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_WRITE_10_SV, "Anslut diskettavbildningsfil för enhet 10 skrivbar (om möjligt)"},
/* tr */ {IDCLS_ATTACH_READ_WRITE_10_TR, "Sürücü #10 için disk imajýný okunur/yazýlýr olarak yerleþtir (mümkünse)"},
#endif

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_11,    N_("Attach disk image for drive #11 read only")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_READ_ONLY_11_DA, "Tilslut disk-image til drev #11 skrivebeskyttet"},
/* de */ {IDCLS_ATTACH_READ_ONLY_11_DE, "Disk Image als Laufwerk #11 benutzen (schreibgeschützt)"},
/* fr */ {IDCLS_ATTACH_READ_ONLY_11_FR, "Charger une image de disque pour le lecteur #11 en lecture seule"},
/* hu */ {IDCLS_ATTACH_READ_ONLY_11_HU, "Csak olvasható képmás csatolása a #11-es lemezegységhez"},
/* it */ {IDCLS_ATTACH_READ_ONLY_11_IT, "Seleziona l'immagine del disco per il drive #11 in sola lettura"},
/* nl */ {IDCLS_ATTACH_READ_ONLY_11_NL, "Koppel schijfbestand voor drive #11 als alleen-lezen"},
/* pl */ {IDCLS_ATTACH_READ_ONLY_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_ONLY_11_SV, "Anslut diskettavbildningsfil för enhet 11 med skrivskydd"},
/* tr */ {IDCLS_ATTACH_READ_ONLY_11_TR, "Sürücü #11 için disk imajýný salt okunur olarak yerleþtir"},
#endif

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_11,    N_("Attach disk image for drive #11 read write (if possible)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_READ_WRITE_11_DA, "Tilslut disk-image til drev #11 skrivbar (hvis muligt)"},
/* de */ {IDCLS_ATTACH_READ_WRITE_11_DE, "Disk Image als Laufwerk #11 benutzen (Schreibzugriff, wenn möglich)"},
/* fr */ {IDCLS_ATTACH_READ_WRITE_11_FR, "Charger une image de disque pour le lecteur #11 en lecture-écriture (si "
                                         "possible)"},
/* hu */ {IDCLS_ATTACH_READ_WRITE_11_HU, "Írható-olvasható képmás csatolása a #11-es lemezegységhez"},
/* it */ {IDCLS_ATTACH_READ_WRITE_11_IT, "Seleziona l'immagine del disco per il drive #11 in lettura/scrittura (se "
                                         "possibile)"},
/* nl */ {IDCLS_ATTACH_READ_WRITE_11_NL, "Koppel schijfbestand voor drive #11 als schrijfbaar (indien mogelijk)"},
/* pl */ {IDCLS_ATTACH_READ_WRITE_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_WRITE_11_SV, "Anslut diskettavbildningsfil för enhet 11 skrivbar (om möjligt)"},
/* tr */ {IDCLS_ATTACH_READ_WRITE_11_TR, "Sürücü #11 için disk imajýný okunur/yazýlýr olarak yerleþtir (mümkünse)"},
#endif

/* datasette.c */
/* en */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET,    N_("Enable automatic Datasette-Reset")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_DA, "Aktivér automatisk Datasette-reset"},
/* de */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_DE, "Automatisches Datasette-Reset aktivieren"},
/* fr */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_FR, "Activer le redémarrage automatique du Datasette"},
/* hu */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_HU, "Automatikus magnó Reset engedélyezése"},
/* it */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_IT, "Attiva il reset automatico del registratore"},
/* nl */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_NL, "Activeer automatische Datasette-Reset"},
/* pl */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_SV, "Aktivera automatisk Datasetteåterställning"},
/* tr */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_TR, "Otomatik Teyp-Reset'i aktif et"},
#endif

/* datasette.c */
/* en */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET,    N_("Disable automatic Datasette-Reset")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_DA, "Deaktiver automatisk Datasette-reset"},
/* de */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_DE, "Automatisches Datasette-Reset deaktivieren"},
/* fr */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_FR, "Désactiver le redémarrage automatique du Datasette"},
/* hu */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_HU, "Automatikus magnó Reset tiltása"},
/* it */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_IT, "Disattiva il reset automatico del registratore"},
/* nl */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_NL, "Automatische Datasette-Reset uitschakelen"},
/* pl */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_SV, "Inaktivera automatisk Datasetteåterställning"},
/* tr */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_TR, "Otomatik Teyp-Reset'i pasifleþtir"},
#endif

/* datasette.c */
/* en */ {IDCLS_SET_ZERO_TAP_DELAY,    N_("Set delay in cycles for a zero in the tap")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_ZERO_TAP_DELAY_DA, "Angiv forsinkelse i cykler for et nul i tap-filen"},
/* de */ {IDCLS_SET_ZERO_TAP_DELAY_DE, "Verzögerung für Zero in Zyklen im Tap"},
/* fr */ {IDCLS_SET_ZERO_TAP_DELAY_FR, "Spécifier le délai en cycles pour un zéro sur le ruban"},
/* hu */ {IDCLS_SET_ZERO_TAP_DELAY_HU, "Adja meg ciklusokban a legelsõ szalagrés hosszát"},
/* it */ {IDCLS_SET_ZERO_TAP_DELAY_IT, "Imposta il ritardo in cicli per uno zero nel tap"},
/* nl */ {IDCLS_SET_ZERO_TAP_DELAY_NL, "Zet de vertraging in cycli voor een nul in de tap"},
/* pl */ {IDCLS_SET_ZERO_TAP_DELAY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_ZERO_TAP_DELAY_SV, "Ange väntecykler för nolla i tap-filen"},
/* tr */ {IDCLS_SET_ZERO_TAP_DELAY_TR, "Baðlantý anýnda cycle tipinden bekleme süresini ayarla"},
#endif

/* datasette.c */
/* en */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP,    N_("Set number of cycles added to each gap in the tap")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_DA, "Angiv antal cykler der lægges til hvert hul i tap-filen"},
/* de */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_DE, "Setze Anzahl der Zyklen für jedes Loch im Tap"},
/* fr */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_FR, "Spécifier le nombre de cycles ajouté a chaque raccord du ruban"},
/* hu */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_HU, "Adja meg ciklusokban a szalagfájlok közti üres rés hosszát"},
/* it */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_IT, "Imposta il numero di cicli aggiunti ad ogni gap nel tap"},
/* nl */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_NL, "Zet aantal extra cycli voor elk gat in de tap"},
/* pl */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_SV, "Ange cykler som läggs till varje gap i tap-filen"},
/* tr */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_TR, "Baðlantý anýnda her boþluk için eklenecek cycle sayýsýný ayarla"},
#endif

#ifdef DEBUG
/* debug.c */
/* en */ {IDCLS_TRACE_MAIN_CPU,    N_("Trace the main CPU")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TRACE_MAIN_CPU_DA, "Trace hovedprocessoren"},
/* de */ {IDCLS_TRACE_MAIN_CPU_DE, "Haupt CPU verfolgen"},
/* fr */ {IDCLS_TRACE_MAIN_CPU_FR, "Tracer l'UCT principal"},
/* hu */ {IDCLS_TRACE_MAIN_CPU_HU, "Fõ CPU nyomkövetése"},
/* it */ {IDCLS_TRACE_MAIN_CPU_IT, "Traccia la CPU principale"},
/* nl */ {IDCLS_TRACE_MAIN_CPU_NL, "Traceer de hoofd-CPU"},
/* pl */ {IDCLS_TRACE_MAIN_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_MAIN_CPU_SV, "Spåra huvudprocessorn"},
/* tr */ {IDCLS_TRACE_MAIN_CPU_TR, "Ana CPU'yu izle"},
#endif

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_MAIN_CPU,    N_("Do not trace the main CPU")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DONT_TRACE_MAIN_CPU_DA, "Trace ikke hovedprocessoren"},
/* de */ {IDCLS_DONT_TRACE_MAIN_CPU_DE, "Haupt CPU nicht verfolgen"},
/* fr */ {IDCLS_DONT_TRACE_MAIN_CPU_FR, "Ne pas tracer l'UCT principal"},
/* hu */ {IDCLS_DONT_TRACE_MAIN_CPU_HU, "Fõ CPU nyomkövetésének tiltása"},
/* it */ {IDCLS_DONT_TRACE_MAIN_CPU_IT, "Non tracciare la CPU principale"},
/* nl */ {IDCLS_DONT_TRACE_MAIN_CPU_NL, "Traceer de hoofd-CPU niet"},
/* pl */ {IDCLS_DONT_TRACE_MAIN_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_MAIN_CPU_SV, "Spåra inte huvudprocessorn"},
/* tr */ {IDCLS_DONT_TRACE_MAIN_CPU_TR, "Ana CPU'yu izleme"},
#endif

/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE0_CPU,    N_("Trace the drive0 CPU")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TRACE_DRIVE0_CPU_DA, "Trace processor i drev 0"},
/* de */ {IDCLS_TRACE_DRIVE0_CPU_DE, "Laufwerk0 CPU verfolgen"},
/* fr */ {IDCLS_TRACE_DRIVE0_CPU_FR, "Tracer l'UCT du drive0"},
/* hu */ {IDCLS_TRACE_DRIVE0_CPU_HU, "A 0-s lemezegység CPU-jának nyomkövetése"},
/* it */ {IDCLS_TRACE_DRIVE0_CPU_IT, "Traccia la CPU del drive0"},
/* nl */ {IDCLS_TRACE_DRIVE0_CPU_NL, "Traceer de CPU van drive 0/8"},
/* pl */ {IDCLS_TRACE_DRIVE0_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_DRIVE0_CPU_SV, "Spåra processor i diskettstation 0"},
/* tr */ {IDCLS_TRACE_DRIVE0_CPU_TR, "Sürücü0 CPU'sunu izle"},
#endif

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE0_CPU,    N_("Do not trace the drive0 CPU")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DONT_TRACE_DRIVE0_CPU_DA, "Trace ikke processor i drev 0"},
/* de */ {IDCLS_DONT_TRACE_DRIVE0_CPU_DE, "Laufwerk0 CPU nicht verfolgen"},
/* fr */ {IDCLS_DONT_TRACE_DRIVE0_CPU_FR, "Ne pas tracer l'UCT du drive0"},
/* hu */ {IDCLS_DONT_TRACE_DRIVE0_CPU_HU, "A 0-s lemezegység CPU-ja nyomkövetésének tiltása"},
/* it */ {IDCLS_DONT_TRACE_DRIVE0_CPU_IT, "Non tracciare la CPU del drive0"},
/* nl */ {IDCLS_DONT_TRACE_DRIVE0_CPU_NL, "Traceer de CPU van drive 0/8 niet"},
/* pl */ {IDCLS_DONT_TRACE_DRIVE0_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_DRIVE0_CPU_SV, "Spåra inte processor i diskettstation 0"},
/* tr */ {IDCLS_DONT_TRACE_DRIVE0_CPU_TR, "Sürücü0 CPU'sunu izleme"},
#endif

/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE1_CPU,    N_("Trace the drive1 CPU")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TRACE_DRIVE1_CPU_DA, "Trace processor i drev 1"},
/* de */ {IDCLS_TRACE_DRIVE1_CPU_DE, "Laufwerk1 CPU verfolgen"},
/* fr */ {IDCLS_TRACE_DRIVE1_CPU_FR, "Tracer l'UCT du drive1"},
/* hu */ {IDCLS_TRACE_DRIVE1_CPU_HU, "Az 1-es lemezegység CPU-jának nyomkövetése"},
/* it */ {IDCLS_TRACE_DRIVE1_CPU_IT, "Traccia la CPU del drive1"},
/* nl */ {IDCLS_TRACE_DRIVE1_CPU_NL, "Traceer de CPU van drive 1/9"},
/* pl */ {IDCLS_TRACE_DRIVE1_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_DRIVE1_CPU_SV, "Spåra processor i diskettstation 1"},
/* tr */ {IDCLS_TRACE_DRIVE1_CPU_TR, "Sürücü1 CPU'sunu izle"},
#endif

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE1_CPU,    N_("Do not trace the drive1 CPU")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DONT_TRACE_DRIVE1_CPU_DA, "Trace ikke processor i drev 1"},
/* de */ {IDCLS_DONT_TRACE_DRIVE1_CPU_DE, "Laufwerk1 CPU nicht verfolgen"},
/* fr */ {IDCLS_DONT_TRACE_DRIVE1_CPU_FR, "Ne pas tracer l'UCT du drive1"},
/* hu */ {IDCLS_DONT_TRACE_DRIVE1_CPU_HU, "Az 1-es lemezegység CPU-ja nyomkövetésének tiltása"},
/* it */ {IDCLS_DONT_TRACE_DRIVE1_CPU_IT, "Non tracciare la CPU del drive1"},
/* nl */ {IDCLS_DONT_TRACE_DRIVE1_CPU_NL, "Traceer de CPU van drive 1/9 niet"},
/* pl */ {IDCLS_DONT_TRACE_DRIVE1_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_DRIVE1_CPU_SV, "Spåra inte processor i diskettstation 1"},
/* tr */ {IDCLS_DONT_TRACE_DRIVE1_CPU_TR, "Sürücü1 CPU'sunu izleme"},
#endif

#if DRIVE_NUM > 2
/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE2_CPU,    N_("Trace the drive2 CPU")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TRACE_DRIVE2_CPU_DA, "Trace processor i drev 2"},
/* de */ {IDCLS_TRACE_DRIVE2_CPU_DE, "Laufwerk2 CPU verfolgen"},
/* fr */ {IDCLS_TRACE_DRIVE2_CPU_FR, "Tracer l'UCT du drive2"},
/* hu */ {IDCLS_TRACE_DRIVE2_CPU_HU, "A 2-es lemezegység CPU-jának nyomkövetése"},
/* it */ {IDCLS_TRACE_DRIVE2_CPU_IT, "Traccia la CPU del drive2"},
/* nl */ {IDCLS_TRACE_DRIVE2_CPU_NL, "Traceer de CPU van drive 2/10"},
/* pl */ {IDCLS_TRACE_DRIVE2_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_DRIVE2_CPU_SV, "Spåra processor i diskettstation 2"},
/* tr */ {IDCLS_TRACE_DRIVE2_CPU_TR, "Sürücü2 CPU'sunu izle"},
#endif

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE2_CPU,    N_("Do not trace the drive2 CPU")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DONT_TRACE_DRIVE2_CPU_DA, "Trace ikke processor i drev 2"},
/* de */ {IDCLS_DONT_TRACE_DRIVE2_CPU_DE, "Laufwerk2 CPU nicht verfolgen"},
/* fr */ {IDCLS_DONT_TRACE_DRIVE2_CPU_FR, "Ne pas tracer l'UCT du drive2"},
/* hu */ {IDCLS_DONT_TRACE_DRIVE2_CPU_HU, "A 2-es lemezegység CPU-ja nyomkövetésének tiltása"},
/* it */ {IDCLS_DONT_TRACE_DRIVE2_CPU_IT, "Non tracciare la CPU del drive2"},
/* nl */ {IDCLS_DONT_TRACE_DRIVE2_CPU_NL, "Traceer de CPU van drive 2/10 niet"},
/* pl */ {IDCLS_DONT_TRACE_DRIVE2_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_DRIVE2_CPU_SV, "Spåra inte processor i diskettstation 2"},
/* tr */ {IDCLS_DONT_TRACE_DRIVE2_CPU_TR, "Sürücü2 CPU'sunu izleme"},
#endif
#endif

#if DRIVE_NUM > 3
/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE3_CPU,    N_("Trace the drive3 CPU")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TRACE_DRIVE3_CPU_DA, "Trace processor i drev 3"},
/* de */ {IDCLS_TRACE_DRIVE3_CPU_DE, "Laufwerk3 CPU verfolgen"},
/* fr */ {IDCLS_TRACE_DRIVE3_CPU_FR, "Tracer l'UCT du drive3"},
/* hu */ {IDCLS_TRACE_DRIVE3_CPU_HU, "A 3-as lemezegység CPU-jának nyomkövetése"},
/* it */ {IDCLS_TRACE_DRIVE3_CPU_IT, "Traccia la CPU del drive3"},
/* nl */ {IDCLS_TRACE_DRIVE3_CPU_NL, "Traceer de CPU van drive 3/11"},
/* pl */ {IDCLS_TRACE_DRIVE3_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_DRIVE3_CPU_SV, "Spåra processor i diskettstation 3"},
/* tr */ {IDCLS_TRACE_DRIVE3_CPU_TR, "Sürücü3 CPU'sunu izle"},
#endif

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE3_CPU,    N_("Do not trace the drive3 CPU")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DONT_TRACE_DRIVE3_CPU_DA, "Trace ikke processor i drev 3"},
/* de */ {IDCLS_DONT_TRACE_DRIVE3_CPU_DE, "Laufwerk3 CPU nicht verfolgen"},
/* fr */ {IDCLS_DONT_TRACE_DRIVE3_CPU_FR, "Ne pas tracer l'UCT du drive3"},
/* hu */ {IDCLS_DONT_TRACE_DRIVE3_CPU_HU, "A 3-as lemezegység CPU-ja nyomkövetésének tiltása"},
/* it */ {IDCLS_DONT_TRACE_DRIVE3_CPU_IT, "Non tracciare la CPU del drive3"},
/* nl */ {IDCLS_DONT_TRACE_DRIVE3_CPU_NL, "Traceer de CPU van drive 3/11 niet"},
/* pl */ {IDCLS_DONT_TRACE_DRIVE3_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_DRIVE3_CPU_SV, "Spåra inte processor i diskettstation 0"},
/* tr */ {IDCLS_DONT_TRACE_DRIVE3_CPU_TR, "Sürücü3 CPU'sunu izleme"},
#endif
#endif

/* debug.c */
/* en */ {IDCLS_TRACE_MODE,    N_("Trace mode (0=normal 1=small 2=history)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TRACE_MODE_DA, "Trace-tilstand (0=normal 1=lille 2=historik)"},
/* de */ {IDCLS_TRACE_MODE_DE, "Verfolgung Modus (0=normal 1=klein 2=Geschichte)"},
/* fr */ {IDCLS_TRACE_MODE_FR, "Mode de trace (0=normal 1=petit 2=historique)"},
/* hu */ {IDCLS_TRACE_MODE_HU, "Nyomkövetési mód (0=normál 1=kicsi 2=elõzmények)"},
/* it */ {IDCLS_TRACE_MODE_IT, "Modalità di tracciamento (0=normale 1=piccola 2=storica)"},
/* nl */ {IDCLS_TRACE_MODE_NL, "Traceermodus (0=normaal 1=klein 2=geschiedenis)"},
/* pl */ {IDCLS_TRACE_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_MODE_SV, "Spårningsläge (0=normal 1=liten 2=historik)"},
/* tr */ {IDCLS_TRACE_MODE_TR, "Ýzleme modu (0=normal 1=küçük 2=tarihçe)"},
#endif
#endif

/* event.c */
/* en */ {IDCLS_PLAYBACK_RECORDED_EVENTS,    N_("Playback recorded events")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PLAYBACK_RECORDED_EVENTS_DA, "Afspil hændelsesoptagelse"},
/* de */ {IDCLS_PLAYBACK_RECORDED_EVENTS_DE, "Wiedergabe von aufgenommener Ereignisse"},
/* fr */ {IDCLS_PLAYBACK_RECORDED_EVENTS_FR, "Lire les événements enregistrés"},
/* hu */ {IDCLS_PLAYBACK_RECORDED_EVENTS_HU, "A felvett események visszajátszása"},
/* it */ {IDCLS_PLAYBACK_RECORDED_EVENTS_IT, "Riproduzione degli eventi registrati"},
/* nl */ {IDCLS_PLAYBACK_RECORDED_EVENTS_NL, "Afspelen opgenomen gebeurtenissen"},
/* pl */ {IDCLS_PLAYBACK_RECORDED_EVENTS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PLAYBACK_RECORDED_EVENTS_SV, "Spela upp inspelade händelser"},
/* tr */ {IDCLS_PLAYBACK_RECORDED_EVENTS_TR, "Kayýttan yürütme kaydedilmiþ olaylarý"},
#endif

/* monitor.c */
/* en */ {IDCLS_EXECUTE_MONITOR_FROM_FILE,    N_("Execute monitor commands from file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_DA, "Udfør monitorkommandoer fra fil"},
/* de */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_DE, "Monitor Kommandos von Datei ausführen"},
/* fr */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_FR, "Exécuter des commandes moniteur depuis un fichier"},
/* hu */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_HU, "Monitorprogram parancsok futtatása fájlból"},
/* it */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_IT, "Esegui i comandi del monitor da un file"},
/* nl */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_NL, "Uitvoeren van commandos uit bestand"},
/* pl */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_PL, ""}, /* fuzzy */
/* sv */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_SV, "Exekvera monitorkommandon från fil"},
/* tr */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_TR, "Dosyadan monitör komutlarý çalýþtýr"},
#endif

/* monitor.c */
/* en */ {IDCLS_SET_INITIAL_BREAKPOINT,    N_("Set an initial breakpoint for the monitor")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_INITIAL_BREAKPOINT_DA, "Sæt et initielt breakpoint for monitoren"},
/* de */ {IDCLS_SET_INITIAL_BREAKPOINT_DE, "Setze initialen Breakpoint für Monitor"},
/* fr */ {IDCLS_SET_INITIAL_BREAKPOINT_FR, "Régler un point d’arrêt initial pour le moniteur"},
/* hu */ {IDCLS_SET_INITIAL_BREAKPOINT_HU, "Kezdeti töréspont megadása a monitorprogramnak"},
/* it */ {IDCLS_SET_INITIAL_BREAKPOINT_IT, "Imposta un breakpoint iniziale per il monitor"},
/* nl */ {IDCLS_SET_INITIAL_BREAKPOINT_NL, "Zet een beginnende breekpunt voor de monitor"},
/* pl */ {IDCLS_SET_INITIAL_BREAKPOINT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_INITIAL_BREAKPOINT_SV, "Sätt en ursprunglig brytpunkt för monitorn"},
/* tr */ {IDCLS_SET_INITIAL_BREAKPOINT_TR, "Monitör için dahili bir kesme noktasý ayarla"},
#endif

/* fliplist.c */
/* en */ {IDCLS_SPECIFY_FLIP_LIST_NAME,    N_("Specify name of the flip list file image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_FLIP_LIST_NAME_DA, "Angiv navn på bladreliste"},
/* de */ {IDCLS_SPECIFY_FLIP_LIST_NAME_DE, "Namen für Fliplist Datei definieren"},
/* fr */ {IDCLS_SPECIFY_FLIP_LIST_NAME_FR, "Spécifier le nom de l'image du fichier de groupement de disques"},
/* hu */ {IDCLS_SPECIFY_FLIP_LIST_NAME_HU, "Adja meg a lemezlista fájl nevét"},
/* it */ {IDCLS_SPECIFY_FLIP_LIST_NAME_IT, "Specifica il nome dell'immagine del file della flip list"},
/* nl */ {IDCLS_SPECIFY_FLIP_LIST_NAME_NL, "Geef de naam van het fliplijstbestand"},
/* pl */ {IDCLS_SPECIFY_FLIP_LIST_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FLIP_LIST_NAME_SV, "Ange namn på vallistefilavbildningen"},
/* tr */ {IDCLS_SPECIFY_FLIP_LIST_NAME_TR, "Sýralama listesi dosya imajý ismi belirt"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS,    N_("Show a list of the available options and exit normally")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_DA, "Vis en liste over tilgængelige parametre og afslut"},
/* de */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_DE, "Liste von verfügbaren Optionen zeigen und beenden"},
/* fr */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_FR, "Montrer une liste des options disponibles et terminer normalement"},
/* hu */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_HU, "Kilistázza a lehetséges opciókat és kilép"},
/* it */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_IT, "Mostra la lista delle opzioni disponibili ed esce"},
/* nl */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_NL, "Toon een lijst van de beschikbare opties en exit zoals normaal"},
/* pl */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_SV, "Visa lista över tillgängliga flaggor och avsluta normalt"},
/* tr */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_TR, "Mevcut seçeneklerin listesini göster ve normal çýk"},
#endif

#if (!defined  __OS2__ && !defined __BEOS__)
/* initcmdline.c */
/* en */ {IDCLS_CONSOLE_MODE,    N_("Console mode (for music playback)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_CONSOLE_MODE_DA, "Konsoltilstand (for musikafspilning)"},
/* de */ {IDCLS_CONSOLE_MODE_DE, "Konsolenmodus (für Musikwiedergabe)"},
/* fr */ {IDCLS_CONSOLE_MODE_FR, "Mode console (pour l'écoute de musique)"},
/* hu */ {IDCLS_CONSOLE_MODE_HU, "Konzol mód (zene lejátszáshoz)"},
/* it */ {IDCLS_CONSOLE_MODE_IT, "Modalità console (per la riproduzione musicale)"},
/* nl */ {IDCLS_CONSOLE_MODE_NL, "Consolemodus (voor afspelen voor muziek)"},
/* pl */ {IDCLS_CONSOLE_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_CONSOLE_MODE_SV, "Konsolläge (för musikspelning)"},
/* tr */ {IDCLS_CONSOLE_MODE_TR, "Konsol modu (müzik çalmak için)"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ALLOW_CORE_DUMPS,    N_("Allow production of core dumps")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ALLOW_CORE_DUMPS_DA, "Tillad generering af core-dumps"},
/* de */ {IDCLS_ALLOW_CORE_DUMPS_DE, "Core Dumps ermöglichen"},
/* fr */ {IDCLS_ALLOW_CORE_DUMPS_FR, "Permettre la production de \"core dumps\""},
/* hu */ {IDCLS_ALLOW_CORE_DUMPS_HU, "Core dump készítés engedélyezése"},
/* it */ {IDCLS_ALLOW_CORE_DUMPS_IT, "Consente la produzione di core dump"},
/* nl */ {IDCLS_ALLOW_CORE_DUMPS_NL, "Maken van core dumps toestaan"},
/* pl */ {IDCLS_ALLOW_CORE_DUMPS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ALLOW_CORE_DUMPS_SV, "Tillåt skapa minnesutskrift"},
/* tr */ {IDCLS_ALLOW_CORE_DUMPS_TR, "Bellek durumunun kayýtlarýnýn üretilmesine izin ver"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_DONT_ALLOW_CORE_DUMPS,    N_("Do not produce core dumps")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DONT_ALLOW_CORE_DUMPS_DA, "Lav ikke core-dumps"},
/* de */ {IDCLS_DONT_ALLOW_CORE_DUMPS_DE, "Core Dumps verhindern"},
/* fr */ {IDCLS_DONT_ALLOW_CORE_DUMPS_FR, "Ne pas produire de \"core dumps\""},
/* hu */ {IDCLS_DONT_ALLOW_CORE_DUMPS_HU, "Core dump készítés tiltása"},
/* it */ {IDCLS_DONT_ALLOW_CORE_DUMPS_IT, "Non produce core dump"},
/* nl */ {IDCLS_DONT_ALLOW_CORE_DUMPS_NL, "Maken van core dumps niet toestaan"},
/* pl */ {IDCLS_DONT_ALLOW_CORE_DUMPS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_ALLOW_CORE_DUMPS_SV, "Skapa inte minnesutskrift"},
/* tr */ {IDCLS_DONT_ALLOW_CORE_DUMPS_TR, "Bellek durumunun kayýtlarýný üretme"},
#endif
#else
/* initcmdline.c */
/* en */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER,    N_("Don't call exception handler")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_DA, "Kald ikke exception-handler"},
/* de */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_DE, "Ausnahmebehandlung vermeiden"},
/* fr */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_FR, "Ne pas utiliser l'assistant d'exception"},
/* hu */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_HU, "Ne hívja a kivétel kezelõt"},
/* it */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_IT, "Richiama il gestore delle eccezioni"},
/* nl */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_NL, "Geen gebruik maken van de exception handler"},
/* pl */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_SV, "Anropa inte undantagshanterare"},
/* tr */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_TR, "Kural dýþý iþleyiciyi çaðýrma"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_CALL_EXCEPTION_HANDLER,    N_("Call exception handler (default)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_CALL_EXCEPTION_HANDLER_DA, "Kald exception-handler (standard)"},
/* de */ {IDCLS_CALL_EXCEPTION_HANDLER_DE, "Ausnahmebehandlung aktivieren (Default)"},
/* fr */ {IDCLS_CALL_EXCEPTION_HANDLER_FR, "Utiliser l'assistant d'exception (par défaut)"},
/* hu */ {IDCLS_CALL_EXCEPTION_HANDLER_HU, "Kivétel kezelõ hívása (alapértelmezés)"},
/* it */ {IDCLS_CALL_EXCEPTION_HANDLER_IT, "Richiama il gestore delle eccezioni (predefinito)"},
/* nl */ {IDCLS_CALL_EXCEPTION_HANDLER_NL, "Gebruik maken van de exception handler (standaard)"},
/* pl */ {IDCLS_CALL_EXCEPTION_HANDLER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_CALL_EXCEPTION_HANDLER_SV, "Anropa undantagshanterare (standard)"},
/* tr */ {IDCLS_CALL_EXCEPTION_HANDLER_TR, "Kural dýþý iþleyiciyi çaðýr (varsayýlan)"},
#endif
#endif

/* initcmdline.c */
/* en */ {IDCLS_RESTORE_DEFAULT_SETTINGS,    N_("Restore default (factory) settings")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_RESTORE_DEFAULT_SETTINGS_DA, "Gendan standardindstillinger"},
/* de */ {IDCLS_RESTORE_DEFAULT_SETTINGS_DE, "Wiederherstellen Standard Einstellungen"},
/* fr */ {IDCLS_RESTORE_DEFAULT_SETTINGS_FR, "Rétablir les paramètres par défaut (usine)"},
/* hu */ {IDCLS_RESTORE_DEFAULT_SETTINGS_HU, "Alap (gyári) beállítások visszaállítása"},
/* it */ {IDCLS_RESTORE_DEFAULT_SETTINGS_IT, "Ripristina le impostazioni originarie"},
/* nl */ {IDCLS_RESTORE_DEFAULT_SETTINGS_NL, "Herstel standaardinstelling"},
/* pl */ {IDCLS_RESTORE_DEFAULT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RESTORE_DEFAULT_SETTINGS_SV, "Återställ förvalda inställningar"},
/* tr */ {IDCLS_RESTORE_DEFAULT_SETTINGS_TR, "Varsayýlan ayarlara (fabrika ayarlarý) geri dön"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_P_FILE,    N_("<filename>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_FILE_DA, "<filnavn>"},
/* de */ {IDCLS_P_FILE_DE, "<Dateiname>"},
/* fr */ {IDCLS_P_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_P_FILE_HU, "<fájlnév>"},
/* it */ {IDCLS_P_FILE_IT, "<nome del file>"},
/* nl */ {IDCLS_P_FILE_NL, "<bestandsnaam>"},
/* pl */ {IDCLS_P_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_FILE_SV, "<filnamn>"},
/* tr */ {IDCLS_P_FILE_TR, "<dosyaismi>"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_SPECIFY_CONFIG_FILE,    N_("Specify config file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CONFIG_FILE_DA, "Angiv konfigurationsfil"},
/* de */ {IDCLS_SPECIFY_CONFIG_FILE_DE, "Konfigurationsdateiname definieren"},
/* fr */ {IDCLS_SPECIFY_CONFIG_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_CONFIG_FILE_HU, "Adja meg a konfigurációs fájl nevét"},
/* it */ {IDCLS_SPECIFY_CONFIG_FILE_IT, "Specifica il nome del file di configurazione"},
/* nl */ {IDCLS_SPECIFY_CONFIG_FILE_NL, "Geef configuratie bestandsnaam"},
/* pl */ {IDCLS_SPECIFY_CONFIG_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CONFIG_FILE_SV, "Ange inställningsfil"},
/* tr */ {IDCLS_SPECIFY_CONFIG_FILE_TR, "Konfigürasyon dosyasýný belirt"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AND_AUTOSTART,    N_("Attach and autostart tape/disk image <name>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AND_AUTOSTART_DA, "Tilslut og autostart bånd-/disk-image <navn>"},
/* de */ {IDCLS_ATTACH_AND_AUTOSTART_DE, "Einlegen und Autostart Disk/Band Image Datei <Name>"},
/* fr */ {IDCLS_ATTACH_AND_AUTOSTART_FR, "Insérer et démarrer l'image de disque/datassette <nom>"},
/* hu */ {IDCLS_ATTACH_AND_AUTOSTART_HU, "<név> lemez/szalag képmás csatolása és automatikus elindítása"},
/* it */ {IDCLS_ATTACH_AND_AUTOSTART_IT, "Seleziona ed avvia l'immagine di una cassetta/disco <nome>"},
/* nl */ {IDCLS_ATTACH_AND_AUTOSTART_NL, "Koppel en autostart een tape/diskbestand <naam>"},
/* pl */ {IDCLS_ATTACH_AND_AUTOSTART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AND_AUTOSTART_SV, "Anslut och starta band-/diskettavbildning <namn>"},
/* tr */ {IDCLS_ATTACH_AND_AUTOSTART_TR, "Teyp/Disk imajý <isim> yerleþtir ve otomatik baþlat"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AND_AUTOLOAD,    N_("Attach and autoload tape/disk image <name>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AND_AUTOLOAD_DA, "Tilslut og læs bånd-/disk-image <navn>"},
/* de */ {IDCLS_ATTACH_AND_AUTOLOAD_DE, "Einlegen und Autoload Disk/Band Image Datei <Name>"},
/* fr */ {IDCLS_ATTACH_AND_AUTOLOAD_FR, "Insérer et charger l'image de disque/datassette <nom>"},
/* hu */ {IDCLS_ATTACH_AND_AUTOLOAD_HU, "<név> lemez/szalag képmás csatolása és automatikus betöltése"},
/* it */ {IDCLS_ATTACH_AND_AUTOLOAD_IT, "Seleziona e carica l'immagine di una cassetta/disco <nome>"},
/* nl */ {IDCLS_ATTACH_AND_AUTOLOAD_NL, "Koppel en autolaad een tape-/diskbestand <naam>"},
/* pl */ {IDCLS_ATTACH_AND_AUTOLOAD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AND_AUTOLOAD_SV, "Anslut och läs in band-/diskettavbildning <namn>"},
/* tr */ {IDCLS_ATTACH_AND_AUTOLOAD_TR, "Teyp/Disk imajý <isim> yerleþtir ve otomatik yükle"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_TAPE,    N_("Attach <name> as a tape image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AS_TAPE_DA, "Tilslut <navn> som bånd-image"},
/* de */ {IDCLS_ATTACH_AS_TAPE_DE, "Image Datei <Name> einlegen"},
/* fr */ {IDCLS_ATTACH_AS_TAPE_FR, "Insérer <nom> comme image de datassette"},
/* hu */ {IDCLS_ATTACH_AS_TAPE_HU, "<név> szalag képmás csatolása"},
/* it */ {IDCLS_ATTACH_AS_TAPE_IT, "Seleziona <nome> come un'immagine di una cassetta"},
/* nl */ {IDCLS_ATTACH_AS_TAPE_NL, "Koppel <naam> als een tapebestand"},
/* pl */ {IDCLS_ATTACH_AS_TAPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_TAPE_SV, "Anslut <namn> som bandavbildning"},
/* tr */ {IDCLS_ATTACH_AS_TAPE_TR, "Teyp imajý olarak <isim> yerleþtir"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_8,    N_("Attach <name> as a disk image in drive #8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AS_DISK_8_DA, "Tilslut <navn> som disk-image i enhed #8"},
/* de */ {IDCLS_ATTACH_AS_DISK_8_DE, "Image Datei <Name> im Laufwerk #8 einlegen"},
/* fr */ {IDCLS_ATTACH_AS_DISK_8_FR, "Insérer <nom> comme image de disque dans le lecteur #8"},
/* hu */ {IDCLS_ATTACH_AS_DISK_8_HU, "<név> lemezképmás csatolása #8-as egységként"},
/* it */ {IDCLS_ATTACH_AS_DISK_8_IT, "Seleziona <nome> come un'immagine di un disco nel drive #8"},
/* nl */ {IDCLS_ATTACH_AS_DISK_8_NL, "Koppel <naam> aan als een schijfbestand in drive #8"},
/* pl */ {IDCLS_ATTACH_AS_DISK_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_DISK_8_SV, "Anslut <namn> som diskettavbildning i enhet 8"},
/* tr */ {IDCLS_ATTACH_AS_DISK_8_TR, "Sürücü #8 disk imajý olarak <isim> yerleþtir"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_9,    N_("Attach <name> as a disk image in drive #9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AS_DISK_9_DA, "Tilslut <navn> som disk-image i enhed #9"},
/* de */ {IDCLS_ATTACH_AS_DISK_9_DE, "Image Datei <Name> im Laufwerk #9 einlegen"},
/* fr */ {IDCLS_ATTACH_AS_DISK_9_FR, "Insérer <nom> comme image de disque dans le lecteur #9"},
/* hu */ {IDCLS_ATTACH_AS_DISK_9_HU, "<név> lemezképmás csatolása #9-es egységként"},
/* it */ {IDCLS_ATTACH_AS_DISK_9_IT, "Seleziona <nome> come un'immagine di un disco nel drive #9"},
/* nl */ {IDCLS_ATTACH_AS_DISK_9_NL, "Koppel <naam> aan als een schijfbestand in drive #9"},
/* pl */ {IDCLS_ATTACH_AS_DISK_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_DISK_9_SV, "Anslut <namn> som diskettavbildning i enhet 9"},
/* tr */ {IDCLS_ATTACH_AS_DISK_9_TR, "Sürücü #9 disk imajý olarak <isim> yerleþtir"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_10,    N_("Attach <name> as a disk image in drive #10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AS_DISK_10_DA, "Tilslut <navn> som disk-image i enhed #10"},
/* de */ {IDCLS_ATTACH_AS_DISK_10_DE, "Image Datei <Name> im Laufwerk #10 einlegen"},
/* fr */ {IDCLS_ATTACH_AS_DISK_10_FR, "Insérer <nom> comme image de disque dans le lecteur #10"},
/* hu */ {IDCLS_ATTACH_AS_DISK_10_HU, "<név> lemezképmás csatolása #10-es egységként"},
/* it */ {IDCLS_ATTACH_AS_DISK_10_IT, "Seleziona <nome> come un'immagine di un disco nel drive #10"},
/* nl */ {IDCLS_ATTACH_AS_DISK_10_NL, "Koppel <naam> aan als een schijfbestand in drive #10"},
/* pl */ {IDCLS_ATTACH_AS_DISK_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_DISK_10_SV, "Anslut <namn> som diskettavbildning i enhet 10"},
/* tr */ {IDCLS_ATTACH_AS_DISK_10_TR, "Sürücü #10 disk imajý olarak <isim> yerleþtir"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_11,    N_("Attach <name> as a disk image in drive #11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AS_DISK_11_DA, "Tilslut <navn> som disk-image i enhed #11"},
/* de */ {IDCLS_ATTACH_AS_DISK_11_DE, "Image Datei <Name> im Laufwerk #11 einlegen"},
/* fr */ {IDCLS_ATTACH_AS_DISK_11_FR, "Insérer <nom> comme image de disque dans le lecteur #11"},
/* hu */ {IDCLS_ATTACH_AS_DISK_11_HU, "<név> lemezképmás csatolása #11-es egységként"},
/* it */ {IDCLS_ATTACH_AS_DISK_11_IT, "Seleziona <nome> come un'immagine di un disco nel drive #11"},
/* nl */ {IDCLS_ATTACH_AS_DISK_11_NL, "Koppel <naam> aan als een schijfbestand in drive #11"},
/* pl */ {IDCLS_ATTACH_AS_DISK_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_DISK_11_SV, "Anslut <namn> som diskettavbildning i enhet 11"},
/* tr */ {IDCLS_ATTACH_AS_DISK_11_TR, "Sürücü #11 disk imajý olarak <isim> yerleþtir"},
#endif

/* kbdbuf.c */
/* en */ {IDCLS_P_STRING,    N_("<string>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_STRING_DA, "<streng>"},
/* de */ {IDCLS_P_STRING_DE, "<String>"},
/* fr */ {IDCLS_P_STRING_FR, "<chaine>"},
/* hu */ {IDCLS_P_STRING_HU, "<sztring>"},
/* it */ {IDCLS_P_STRING_IT, "<stringa>"},
/* nl */ {IDCLS_P_STRING_NL, "<string>"},
/* pl */ {IDCLS_P_STRING_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_STRING_SV, "<sträng>"},
/* tr */ {IDCLS_P_STRING_TR, "<yazý>"},
#endif

/* kbdbuf.c */
/* en */ {IDCLS_PUT_STRING_INTO_KEYBUF,    N_("Put the specified string into the keyboard buffer.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PUT_STRING_INTO_KEYBUF_DA, "Indsæt den angivne streng i tastatur-bufferen."},
/* de */ {IDCLS_PUT_STRING_INTO_KEYBUF_DE, "Definierte Eingabe in Tastaturpuffer bereitstellen."},
/* fr */ {IDCLS_PUT_STRING_INTO_KEYBUF_FR, "Placer la chaîne spécifiée dans le tampon clavier."},
/* hu */ {IDCLS_PUT_STRING_INTO_KEYBUF_HU, "A megadott string bemásolása a billentyûzet pufferbe."},
/* it */ {IDCLS_PUT_STRING_INTO_KEYBUF_IT, "Metti la stringa specificata nel buffer di tastiera."},
/* nl */ {IDCLS_PUT_STRING_INTO_KEYBUF_NL, "Plaats de opgegeven string in de toetsenbordbuffer."},
/* pl */ {IDCLS_PUT_STRING_INTO_KEYBUF_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PUT_STRING_INTO_KEYBUF_SV, "Lägg den angivna strängen i tangentbordsbufferten."},
/* tr */ {IDCLS_PUT_STRING_INTO_KEYBUF_TR, "Klavye arabelleðine belirtilen yazýyý koy."},
#endif

/* log.c */
/* en */ {IDCLS_SPECIFY_LOG_FILE_NAME,    N_("Specify log file name")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_LOG_FILE_NAME_DA, "Angiv navn på logfil"},
/* de */ {IDCLS_SPECIFY_LOG_FILE_NAME_DE, "Logdateiname definieren"},
/* fr */ {IDCLS_SPECIFY_LOG_FILE_NAME_FR, "Spécifier le nom du fichier log"},
/* hu */ {IDCLS_SPECIFY_LOG_FILE_NAME_HU, "Adja meg a naplófájl nevét"},
/* it */ {IDCLS_SPECIFY_LOG_FILE_NAME_IT, "Specifica il nome del file di log"},
/* nl */ {IDCLS_SPECIFY_LOG_FILE_NAME_NL, "Geef de naam van het logbestand"},
/* pl */ {IDCLS_SPECIFY_LOG_FILE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_LOG_FILE_NAME_SV, "Ange namn på loggfil"},
/* tr */ {IDCLS_SPECIFY_LOG_FILE_NAME_TR, "Log dosyasý ismini belirt"},
#endif

/* mouse.c */
/* en */ {IDCLS_ENABLE_MOUSE_GRAB,    N_("Enable mouse grab")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_MOUSE_GRAB_DA, "Aktivér overtagelse af mus"},
/* de */ {IDCLS_ENABLE_MOUSE_GRAB_DE, "Maus aktivieren"},
/* fr */ {IDCLS_ENABLE_MOUSE_GRAB_FR, "Saisir la souris"},
/* hu */ {IDCLS_ENABLE_MOUSE_GRAB_HU, "Egér elkapás engedélyezése"},
/* it */ {IDCLS_ENABLE_MOUSE_GRAB_IT, "Attiva cattura mouse"},
/* nl */ {IDCLS_ENABLE_MOUSE_GRAB_NL, "Gebruiken van de muis inschakelen"},
/* pl */ {IDCLS_ENABLE_MOUSE_GRAB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_MOUSE_GRAB_SV, "Aktivera fångande av mus"},
/* tr */ {IDCLS_ENABLE_MOUSE_GRAB_TR, "Mouse yakalamayý aktif et"},
#endif

/* mouse.c */
/* en */ {IDCLS_DISABLE_MOUSE_GRAB,    N_("Disable mouse grab")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_MOUSE_GRAB_DA, "Deaktiver overtagelse af mus"},
/* de */ {IDCLS_DISABLE_MOUSE_GRAB_DE, "Maus deaktivieren"},
/* fr */ {IDCLS_DISABLE_MOUSE_GRAB_FR, "Ne pas saisir la souris"},
/* hu */ {IDCLS_DISABLE_MOUSE_GRAB_HU, "Egér elkapás tiltása"},
/* it */ {IDCLS_DISABLE_MOUSE_GRAB_IT, "Disattiva cattura mouse"},
/* nl */ {IDCLS_DISABLE_MOUSE_GRAB_NL, "Gebruiken van de muis uitschakelen"},
/* pl */ {IDCLS_DISABLE_MOUSE_GRAB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_MOUSE_GRAB_SV, "Inaktivera fångande av mus"},
/* tr */ {IDCLS_DISABLE_MOUSE_GRAB_TR, "Mouse yakalamayý pasifleþtir"},
#endif

/* mouse.c */
/* en */ {IDCLS_SELECT_MOUSE_JOY_PORT,    N_("Select the joystick port the mouse is attached to")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SELECT_MOUSE_JOY_PORT_DA, "Vælg hvilken joystickport musen er tilsluttet"},
/* de */ {IDCLS_SELECT_MOUSE_JOY_PORT_DE, "Joystickport für Maus auswählen"},
/* fr */ {IDCLS_SELECT_MOUSE_JOY_PORT_FR, "Sélectionner le port sur lequel attacher la souris"},
/* hu */ {IDCLS_SELECT_MOUSE_JOY_PORT_HU, "Válassza ki a joystick portot, ahová az egér csatolva van"},
/* it */ {IDCLS_SELECT_MOUSE_JOY_PORT_IT, "Seleziona la porta joystick a cui è collegato il mouse"},
/* nl */ {IDCLS_SELECT_MOUSE_JOY_PORT_NL, "Selecteer de joystickpoort waar de muis aan gekoppelt is"},
/* pl */ {IDCLS_SELECT_MOUSE_JOY_PORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SELECT_MOUSE_JOY_PORT_SV, "Ange vilken spelport musen är ansluten till"},
/* tr */ {IDCLS_SELECT_MOUSE_JOY_PORT_TR, "Mouse'un baðlý olduðu joystick portunu seçin"},
#endif

/* mouse.c */
/* en */ {IDCLS_SELECT_MOUSE_TYPE,    N_("Select the mouse type (0 = 1351, 1 = NEOS, 2 = Amiga, 3 = Paddles)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SELECT_MOUSE_TYPE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SELECT_MOUSE_TYPE_DE, "Maus typ wählen (0 = 1351, 1 = NEOS, 2 = Amiga, 3 = Paddles)"},
/* fr */ {IDCLS_SELECT_MOUSE_TYPE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SELECT_MOUSE_TYPE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SELECT_MOUSE_TYPE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SELECT_MOUSE_TYPE_NL, "Selecteer de muis soort (0 = 1351, 1 = NEOS, 2 = Amiga, 3 = Paddles)"},
/* pl */ {IDCLS_SELECT_MOUSE_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SELECT_MOUSE_TYPE_SV, "Välj mustyp (0 = 1351, 1 = NEOS, 2 = Amiga, 3 = paddlar)"},
/* tr */ {IDCLS_SELECT_MOUSE_TYPE_TR, ""},  /* fuzzy */
#endif

/* ps2mouse.c */
/* en */ {IDCLS_ENABLE_PS2MOUSE,    N_("Enable PS/2 mouse on userport")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_PS2MOUSE_DA, "Aktivér PS/2-mus på brugerporten"},
/* de */ {IDCLS_ENABLE_PS2MOUSE_DE, "PS/2 Userport Emulation aktivieren"},
/* fr */ {IDCLS_ENABLE_PS2MOUSE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_PS2MOUSE_HU, "PS/2 egér engedélyezése a userporton"},
/* it */ {IDCLS_ENABLE_PS2MOUSE_IT, "Attiva mouse PS/2 su userport"},
/* nl */ {IDCLS_ENABLE_PS2MOUSE_NL, "Aktiveer emulatie van een PS/2 muis op de userport"},
/* pl */ {IDCLS_ENABLE_PS2MOUSE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_PS2MOUSE_SV, "Aktivera PS/2-mus på användarport"},
/* tr */ {IDCLS_ENABLE_PS2MOUSE_TR, "Userport üzerinde PS/2 mouse'u aktif et"},
#endif

/* ps2mouse.c */
/* en */ {IDCLS_DISABLE_PS2MOUSE,    N_("Disable PS/2 mouse on userport")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_PS2MOUSE_DA, "Deaktiver PS/2-mus på brugerporten"},
/* de */ {IDCLS_DISABLE_PS2MOUSE_DE, "PS/2 Userport Emulation deaktivieren\""},
/* fr */ {IDCLS_DISABLE_PS2MOUSE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_PS2MOUSE_HU, "PS/2 egér tiltása a userporton"},
/* it */ {IDCLS_DISABLE_PS2MOUSE_IT, "Disattiva mouse PS/2 su userport"},
/* nl */ {IDCLS_DISABLE_PS2MOUSE_NL, "Emulatie van een PS/2 muis op de userport afsluiten"},
/* pl */ {IDCLS_DISABLE_PS2MOUSE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_PS2MOUSE_SV, "Inaktivera PS/2-mus på användarport"},
/* tr */ {IDCLS_DISABLE_PS2MOUSE_TR, "Userport üzerinde PS/2 mouse'u pasifleþtir"},
#endif

/* ram.c */
/* en */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE,    N_("Set the value for the very first RAM address after powerup")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_DA, "Angiv værdien for den allerførste RAM adresse efter power-up"},
/* de */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_DE, "Wert für erstes Byte im RAM nach Kaltstart setzen"},
/* fr */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_FR, "Spécifier la valeur de la première adresse RAM après la mise sous "
                                                "tension"},
/* hu */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_HU, "Adja meg a legelsõ RAM cím értékét bekapcsolás után"},
/* it */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_IT, "Imposta il valore del primissimo indirizzo della RAM dopo l'accensione"},
/* nl */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_NL, "Zet de waarde voor het allereerste RAM adres na koude start"},
/* pl */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_SV, "Ange värdet på den allra första RAM-adressen efter strömpåslag"},
/* tr */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_TR, "Güç verildiðinde baþlangýçtaki RAM adreslerine atanacak deðeri belirle"},
#endif

/* ram.c */
/* en */ {IDCLS_P_NUM_OF_BYTES,    N_("<num of bytes>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_NUM_OF_BYTES_DA, "<antal bytes>"},
/* de */ {IDCLS_P_NUM_OF_BYTES_DE, "<Anzahl an Bytes>"},
/* fr */ {IDCLS_P_NUM_OF_BYTES_FR, "<nombre d'octets>"},
/* hu */ {IDCLS_P_NUM_OF_BYTES_HU, "<bájtok száma>"},
/* it */ {IDCLS_P_NUM_OF_BYTES_IT, "<numero di byte>"},
/* nl */ {IDCLS_P_NUM_OF_BYTES_NL, "<aantal bytes>"},
/* pl */ {IDCLS_P_NUM_OF_BYTES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_NUM_OF_BYTES_SV, "<antal byte>"},
/* tr */ {IDCLS_P_NUM_OF_BYTES_TR, "<byte sayýsý>"},
#endif

/* ram.c */
/* en */ {IDCLS_LENGTH_BLOCK_SAME_VALUE,    N_("Length of memory block initialized with the same value")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_DA, "Længde på hukommelsesblok som initialiseres med samme værdi"},
/* de */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_DE, "Länge des Speicherblocks der mit dem gleichen Wert initialisiert ist"},
/* fr */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_FR, "Longeur du premier bloc mémoire initialisé avec la même valeur"},
/* hu */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_HU, "Azonos értékkel feltöltött memória blokkoknak a hossza"},
/* it */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_IT, "Lunghezza del blocco di memoria inizializzato con lo stesso valore"},
/* nl */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_NL, "Geheugenblokgrootte die dezelfde waarde krijgt bij initialisatie"},
/* pl */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_SV, "Längd på minnesblock som initierats med samma värde"},
/* tr */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_TR, "Ayný deðerle balangýç ayarlarý yapýlan bellek bloðu uzunluðu"},
#endif

/* ram.c */
/* en */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN,    N_("Length of memory block initialized with the same pattern")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_DA, "Længde på hukommelsesblok som initialiseres med samme mønster"},
/* de */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_DE, "Länge des Speicherblocks der mit dem gleichen Muster initialisiert ist"},
/* fr */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_FR, "Longeur du premier bloc mémoire initialisé avec le même pattern"},
/* hu */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_HU, "Azonos mintával feltöltött memória blokkoknak a hossza"},
/* it */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_IT, "Lunghezza del blocco di memoria inizializzato con lo stesso pattern"},
/* nl */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_NL, "Geheugenblokgrootte met hetzelfde patroon bij initialisatie"},
/* pl */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_SV, "Längd på minnesblock som initierats med samma mönster"},
/* tr */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_TR, "Ayný desenle balangýç ayarlarý yapýlan bellek bloðu uzunluðu"},
#endif

/* sound.c */
/* en */ {IDCLS_ENABLE_SOUND_PLAYBACK,    N_("Enable sound playback")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_ENABLE_SOUND_PLAYBACK_DA, "Slå lydemulering til"},
/* de */ {IDCLS_ENABLE_SOUND_PLAYBACK_DE, "Sound Wiedergaben einschalten"},
/* fr */ {IDCLS_ENABLE_SOUND_PLAYBACK_FR, "Activer le son"},
/* hu */ {IDCLS_ENABLE_SOUND_PLAYBACK_HU, "Hangok engedélyezése"},
/* it */ {IDCLS_ENABLE_SOUND_PLAYBACK_IT, "Attiva la riproduzione del suono"},
/* nl */ {IDCLS_ENABLE_SOUND_PLAYBACK_NL, "Activeer geluidsweergave"},
/* pl */ {IDCLS_ENABLE_SOUND_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SOUND_PLAYBACK_SV, "Aktivera ljudåtergivning"},
/* tr */ {IDCLS_ENABLE_SOUND_PLAYBACK_PL, "Sesi aktif et"},
#endif

/* sound.c */
/* en */ {IDCLS_DISABLE_SOUND_PLAYBACK,    N_("Disable sound playback")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_DISABLE_SOUND_PLAYBACK_DA, "Slå lydemulering fra"},
/* de */ {IDCLS_DISABLE_SOUND_PLAYBACK_DE, "Sound Wiedergaben ausschalten"},
/* fr */ {IDCLS_DISABLE_SOUND_PLAYBACK_FR, "Désactiver le son"},
/* hu */ {IDCLS_DISABLE_SOUND_PLAYBACK_HU, "Hangok tiltása"},
/* it */ {IDCLS_DISABLE_SOUND_PLAYBACK_IT, "Disattiva la riproduzione del suono"},
/* nl */ {IDCLS_DISABLE_SOUND_PLAYBACK_NL, "Geluid uitschakelen"},
/* pl */ {IDCLS_DISABLE_SOUND_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SOUND_PLAYBACK_SV, "Inaktivera ljudåtergivning"},
/* tr */ {IDCLS_DISABLE_SOUND_PLAYBACK_TR, "Sesi pasifleþtir"},
#endif

/* sound.c */
/* en */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ,    N_("Set sound sample rate to <value> Hz")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_DA, "Sæt lydsamplefrekvens til <værdi> Hz"},
/* de */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_DE, "Setze Sound Sample Rate zu <Wert> Hz"},
/* fr */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_FR, "Régler le taux d'échantillonage à <valeur> Hz"},
/* hu */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_HU, "Hang mintavételezési ráta beállítása <érték> Hz-re"},
/* it */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_IT, "Imposta la velocità di campionamento del suono a <valore> Hz"},
/* nl */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_NL, "Zet de geluid sample rate naar <waarde> Hz"},
/* pl */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_SV, "Sätt ljudsamplingshastighet till <värde> Hz"},
/* tr */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_TR, "Ses örnekleme hýzýný <deðer> Hz olarak ayarla"},
#endif

/* sound.c */
/* en */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC,    N_("Set sound buffer size to <value> msec")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_DA, "Sæt lydbufferstørrelse til <værdi> ms"},
/* de */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_DE, "Setze Source Buffer Größe zu <Wert> msek"},
/* fr */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_FR, "Régler la taille du tampon son à <valeur> ms"},
/* hu */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_HU, "A hangpuffer méretét <érték> mp-re állítja"},
/* it */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_IT, "Imposta la dimensione del buffer del suono a <valore> msec"},
/* nl */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_NL, "Zet de geluidsbuffergrootte naar <waarde> msec"},
/* pl */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_SV, "Sätt ljudbuffertstorlek till <värde> ms"},
/* tr */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_TR, "Ses arabellek boyutunu <deðer> milisaniye olarak ayarla"},
#endif

/* sound.c */
/* en */ {IDCLS_SPECIFY_SOUND_DRIVER,    N_("Specify sound driver")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_SPECIFY_SOUND_DRIVER_DA, "Angiv lyddriver"},
/* de */ {IDCLS_SPECIFY_SOUND_DRIVER_DE, "Sound Treiber spezifieren"},
/* fr */ {IDCLS_SPECIFY_SOUND_DRIVER_FR, "Spécifier le pilote de son"},
/* hu */ {IDCLS_SPECIFY_SOUND_DRIVER_HU, "Adja meg a hangmodul nevét"},
/* it */ {IDCLS_SPECIFY_SOUND_DRIVER_IT, "Specifica il driver audio"},
/* nl */ {IDCLS_SPECIFY_SOUND_DRIVER_NL, "Geef geluidsstuurprogramma"},
/* pl */ {IDCLS_SPECIFY_SOUND_DRIVER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SOUND_DRIVER_SV, "Ange ljuddrivrutin"},
/* tr */ {IDCLS_SPECIFY_SOUND_DRIVER_TR, "Ses sürücüsü belirt"},
#endif

/* sound.c */
/* en */ {IDCLS_P_ARGS,    N_("<args>")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_P_ARGS_DA, "<argumenter>"},
/* de */ {IDCLS_P_ARGS_DE, "<Argumente>"},
/* fr */ {IDCLS_P_ARGS_FR, "<args>"},
/* hu */ {IDCLS_P_ARGS_HU, "<argumentumok>"},
/* it */ {IDCLS_P_ARGS_IT, "<argomenti>"},
/* nl */ {IDCLS_P_ARGS_NL, "<parameters>"},
/* pl */ {IDCLS_P_ARGS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_ARGS_SV, "<flaggor>"},
/* tr */ {IDCLS_P_ARGS_TR, "<argümanlar>"},
#endif

/* sound.c */
/* en */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM,    N_("Specify initialization parameters for sound driver")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_DA, "Angiv initialiseringsparametre for lyddriver"},
/* de */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_DE, "Initialisierungsparameter des Sound Treibers spezifizieren"},
/* fr */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_FR, "Spécifier les paramètres d'initialisation pour le pilote son"},
/* hu */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_HU, "Adja meg a hangmodul indulási paramétereit"},
/* it */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_IT, "Specifica i parametri di inizializzazione del driver audio"},
/* nl */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_NL, "Geef de initialisatieparameters voor het geluidsstuurprogramma"},
/* pl */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_SV, "Ange initieringsflaggor för ljuddrivrutin"},
/* tr */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_TR, "Ses sürücüsü için baþlangýç parametrelerini belirt"},
#endif

/* sound.c */
/* en */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER,    N_("Specify recording sound driver")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_DA, "Angiv driver til lydoptagelse"},
/* de */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_DE, "Sound Treiber für Aufnahme spezifizieren"},
/* fr */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_FR, "Spécifier le pilote d'enregistrement"},
/* hu */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_HU, "Adja meg a felvételhez használt hangmodult"},
/* it */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_IT, "Specifica il driver di registrazione del suono"},
/* nl */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_NL, "Geef geluidsstuurprogramma voor opname"},
/* pl */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_SV, "Ange ljuddrivrutin för inspelning"},
/* tr */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_TR, "Ses kayýt sürücüsünü belirt"},
#endif

/* sound.c */
/* en */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM,    N_("Specify initialization parameters for recording sound driver")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_DA, "Angiv initialiseringsparametre for driver til lydoptagelse"},
/* de */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_DE, "Initialisierungsparameter für Aufnahme Sound Treiber spezifieren"},
/* fr */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_FR, "Spécifier les paramètres d'initialisation pour le pilote "
                                                   "d'enregistrement"},
/* hu */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_HU, "Adja meg a felvételhez használt hangmodul indulási paramétereit"},
/* it */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_IT, "Specifica i parametri di inizializzazione per il driver audio di "
                                                   "registrazione"},
/* nl */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_NL, "Geef initialisatieparameters voor het geluidsstuurprogramma voor "
                                                   "opname"},
/* pl */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_SV, "Ange initieringsflaggor för ljuddrivrutin för inspelning"},
/* tr */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_TR, "Ses kayýt sürücüsü için baþlangýç parametrelerini belirt"},
#endif

/* sound.c */
/* en */ {IDCLS_P_SYNC,    N_("<sync>")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_P_SYNC_DA, "<sync>"},
/* de */ {IDCLS_P_SYNC_DE, "<Sync>"},
/* fr */ {IDCLS_P_SYNC_FR, "<sync>"},
/* hu */ {IDCLS_P_SYNC_HU, "<sync>"},
/* it */ {IDCLS_P_SYNC_IT, "<sync>"},
/* nl */ {IDCLS_P_SYNC_NL, "<sync>"},
/* pl */ {IDCLS_P_SYNC_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_SYNC_SV, "<synk>"},
/* tr */ {IDCLS_P_SYNC_TR, "<senkron>"},
#endif

/* sound.c */
/* en */ {IDCLS_SET_SOUND_SPEED_ADJUST,    N_("Set sound speed adjustment (0: flexible, 1: adjusting, 2: exact)")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_SET_SOUND_SPEED_ADJUST_DA, "Indstil lydjusteringshastighed (0: fleksibel, 1: justerende, 2: nøjagtig)"},
/* de */ {IDCLS_SET_SOUND_SPEED_ADJUST_DE, "Setze Sound Geschwindigkeit Anpassung (0: flexibel, 1: anpassend, 2: exakt)"},
/* fr */ {IDCLS_SET_SOUND_SPEED_ADJUST_FR, "Choisir la méthode d'ajustement du son (0: flexible, 1: ajusté 2: exact)"},
/* hu */ {IDCLS_SET_SOUND_SPEED_ADJUST_HU, "Adja meg a zene sebesség igazítását (0: rugalmas. 1: igazodó, 2: pontos)"},
/* it */ {IDCLS_SET_SOUND_SPEED_ADJUST_IT, "Imposta il tipo di adattamento della velocità dell'audio (0: flessibile, "
                                           "1: adattabile, 2: esatta)"},
/* nl */ {IDCLS_SET_SOUND_SPEED_ADJUST_NL, "Zet geluidssnelheid aanpassing (0: flexibel, 1: aanpassend, 2: exact)"},
/* pl */ {IDCLS_SET_SOUND_SPEED_ADJUST_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SOUND_SPEED_ADJUST_SV, "Ställ in ljudhastighetsjustering (0: flexibel, 1: justerande, 2: exakt)"},
/* tr */ {IDCLS_SET_SOUND_SPEED_ADJUST_TR, "Ses hýzý ayarlamasýný yapýn (0: esnek, 1: düzeltme, 2: aynen)"},
#endif

/* sysfile.c */
/* en */ {IDCLS_P_PATH,    N_("<path>")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_P_PATH_DA, "<sti>"},
/* de */ {IDCLS_P_PATH_DE, "<Pfad>"},
/* fr */ {IDCLS_P_PATH_FR, "<chemin>"},
/* hu */ {IDCLS_P_PATH_HU, "<elérési út>"},
/* it */ {IDCLS_P_PATH_IT, "<percorso>"},
/* nl */ {IDCLS_P_PATH_NL, "<pad>"},
/* pl */ {IDCLS_P_PATH_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_PATH_SV, "<sökväg>"},
/* tr */ {IDCLS_P_PATH_TR, "<yol>"},
#endif

/* sysfile.c */
/* en */ {IDCLS_DEFINE_SYSTEM_FILES_PATH,    N_("Define search path to locate system files")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_DA, "Angiv sti til systemfiler"},
/* de */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_DE, "Suchpfad für Systemdateien definieren"},
/* fr */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_FR, "Définir le chemin de recherche pour trouver les fichiers systèmes"},
/* hu */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_HU, "Keresési útvonal megadása a rendszerfájlok megtalálására"},
/* it */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_IT, "Definisci il path di ricerca per cercare i file di sistema"},
/* nl */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_NL, "Geef het zoek pad waar de systeem bestanden te vinden zijn"},
/* pl */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_SV, "Ange sökväg för att hitta systemfiler"},
/* tr */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_TR, "Sistem dosyalarýnýn konumunu belirlemek için arama yolu tanýmlayýn"},
#endif

/* traps.c */
/* en */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION,    N_("Enable general mechanisms for fast disk/tape emulation")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_DA, "Aktivér generelle mekanismer for hurtig disk-/båndemulering"},
/* de */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_DE, "Allgemeine Mechanismen für schnelle Disk/Band Emulation aktivieren"},
/* fr */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_FR, "Activer les méchanismes généraux pour l'émulation disque/datassette "
                                                "rapide"},
/* hu */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_HU, "A lemez/szalag emulációt gyorsító mechanizmusok engedélyezése"},
/* it */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_IT, "Attiva meccanismo generale per l'emulazione veloce del disco/cassetta"},
/* nl */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_NL, "Activeer algemene methoden voor snelle disk/tape emulatie"},
/* pl */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_SV, "Aktivera generella mekanismer för snabb disk-/bandemulering"},
/* tr */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_TR, "Hýzlý disk/teyp emülasyonu için genel mekanizmayý aktif et"},
#endif

/* traps.c */
/* en */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION,    N_("Disable general mechanisms for fast disk/tape emulation")},
#ifdef HAS_TRANSLATE
/* da */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_DA, "Deaktiver generelle mekanismer for hurtig disk-/båndemulering"},
/* de */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_DE, "Allgemeine Mechanismen für schnelle Disk/Band Emulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_FR, "Désactiver les méchanismes généraux pour l'émulation disque/datassette "
                                                 "rapide"},
/* hu */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_HU, "A lemez/szalag emulációt gyorsító mechanizmusok tiltása"},
/* it */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_IT, "Disattiva meccanismo generale per l'emulazione veloce del "
                                                 "disco/cassetta"},
/* nl */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_NL, "Algemene methoden voor snelle disk/tape emulatie uitschakelen"},
/* pl */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_SV, "Inaktivera generella mekanismer för snabb disk-/bandemulering"},
/* tr */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_TR, "Hýzlý disk/teyp emülasyonu için genel mekanizmayý pasifleþtir"},
#endif

/* vsync.c */
/* en */ {IDCLS_LIMIT_SPEED_TO_VALUE,    N_("Limit emulation speed to specified value")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_LIMIT_SPEED_TO_VALUE_DA, "Begræns emuleringshastighed til angiven værdi"},
/* de */ {IDCLS_LIMIT_SPEED_TO_VALUE_DE, "Emulationsgeschwindigkeit auf Wert beschränken."},
/* fr */ {IDCLS_LIMIT_SPEED_TO_VALUE_FR, "Limiter la vitesse d'émulation à une valeur specifiée"},
/* hu */ {IDCLS_LIMIT_SPEED_TO_VALUE_HU, "Emulációs sebesség lehatárolása adott értékre"},
/* it */ {IDCLS_LIMIT_SPEED_TO_VALUE_IT, "Limita la velocità di emulazione al valore specificato"},
/* nl */ {IDCLS_LIMIT_SPEED_TO_VALUE_NL, "Beperk emulatiesnelheid tot de opgegeven waarde"},
/* pl */ {IDCLS_LIMIT_SPEED_TO_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_LIMIT_SPEED_TO_VALUE_SV, "Begränsa emuleringshastighet till angivet värde"},
/* tr */ {IDCLS_LIMIT_SPEED_TO_VALUE_TR, "Emülasyon hýzýný belirtilmiþ deðer ile limitle"},
#endif

/* vsync.c */
/* en */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES,    N_("Update every <value> frames (`0' for automatic)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_DA, "Opdatér for hver <værdi> billeder (\"0\" for automatisk)"},
/* de */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_DE, "Jedes <Wert> Bild aktualisieren (`0' für Automatik)"},
/* fr */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_FR, "Mise à jour toutes les <valeur> images (`0' pour auto.)"},
/* hu */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_HU, "Frissítsen minden <érték> képkocka elteltével (0 automatikust jelent)"},
/* it */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_IT, "Aggiorna ogni <valore> frame (`0' per automatico)"},
/* nl */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_NL, "Scherm opbouw elke <waarde> frames (`0' voor automatisch)"},
/* pl */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_SV, "Uppdatera varje <värde> ramar (\"0\" för automatiskt)"},
/* tr */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_TR, "Her <deðer> framede bir güncelle (otomatik için `0')"},
#endif

/* vsync.c */
/* en */ {IDCLS_ENABLE_WARP_MODE,    N_("Enable warp mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_WARP_MODE_DA, "Aktivér warp-tilstand"},
/* de */ {IDCLS_ENABLE_WARP_MODE_DE, "Warp Mode Aktivieren"},
/* fr */ {IDCLS_ENABLE_WARP_MODE_FR, "Activer Turbo"},
/* hu */ {IDCLS_ENABLE_WARP_MODE_HU, "Hipergyors mód engedélyezése"},
/* it */ {IDCLS_ENABLE_WARP_MODE_IT, "Attiva la modalità turbo"},
/* nl */ {IDCLS_ENABLE_WARP_MODE_NL, "Activeer warpmodus"},
/* pl */ {IDCLS_ENABLE_WARP_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_WARP_MODE_SV, "Aktivera warpläge"},
/* tr */ {IDCLS_ENABLE_WARP_MODE_TR, "Warp modu aktif et"},
#endif

/* vsync.c */
/* en */ {IDCLS_DISABLE_WARP_MODE,    N_("Disable warp mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_WARP_MODE_DA, "Deaktiver warp-tilstand"},
/* de */ {IDCLS_DISABLE_WARP_MODE_DE, "*Warp Mode Deaktivieren"},
/* fr */ {IDCLS_DISABLE_WARP_MODE_FR, "Désactiver Turbo"},
/* hu */ {IDCLS_DISABLE_WARP_MODE_HU, "Hipergyors mód tiltása"},
/* it */ {IDCLS_DISABLE_WARP_MODE_IT, "Disattiva la modalità turbo"},
/* nl */ {IDCLS_DISABLE_WARP_MODE_NL, "Warpmodus uitschakelen"},
/* pl */ {IDCLS_DISABLE_WARP_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_WARP_MODE_SV, "Inaktivera warpläge"},
/* tr */ {IDCLS_DISABLE_WARP_MODE_TR, "Warp modu pasifleþtir"},
#endif

/* translate.c */
/* en */ {IDCLS_P_ISO_LANGUAGE_CODE,    N_("<iso language code>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_ISO_LANGUAGE_CODE_DA, "<iso-sprogkode>"},
/* de */ {IDCLS_P_ISO_LANGUAGE_CODE_DE, "<iso Sprachcode>"},
/* fr */ {IDCLS_P_ISO_LANGUAGE_CODE_FR, "<iso language code>"},
/* hu */ {IDCLS_P_ISO_LANGUAGE_CODE_HU, "<iso nyelv kód>"},
/* it */ {IDCLS_P_ISO_LANGUAGE_CODE_IT, "<codice iso lingua>"},
/* nl */ {IDCLS_P_ISO_LANGUAGE_CODE_NL, "<iso taalcode>"},
/* pl */ {IDCLS_P_ISO_LANGUAGE_CODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_ISO_LANGUAGE_CODE_SV, "<iso-språkkod>"},
/* tr */ {IDCLS_P_ISO_LANGUAGE_CODE_TR, "<iso dil kodu>"},
#endif

/* translate.c */
/* en */ {IDCLS_SPECIFY_ISO_LANG_CODE,    N_("Specify the iso code of the language")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_ISO_LANG_CODE_DA, "Angiv ISO-koden for sproget"},
/* de */ {IDCLS_SPECIFY_ISO_LANG_CODE_DE, "Iso Sprachcode spezifizieren"},
/* fr */ {IDCLS_SPECIFY_ISO_LANG_CODE_FR, "Spécifier le code iso du langage"},
/* hu */ {IDCLS_SPECIFY_ISO_LANG_CODE_HU, "Adja meg a nyelv iso kódját"},
/* it */ {IDCLS_SPECIFY_ISO_LANG_CODE_IT, "Specifica il codice ISO della lingua"},
/* nl */ {IDCLS_SPECIFY_ISO_LANG_CODE_NL, "Geef de isocode van de taal"},
/* pl */ {IDCLS_SPECIFY_ISO_LANG_CODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_ISO_LANG_CODE_SV, "Ange ISO-koden för språket"},
/* tr */ {IDCLS_SPECIFY_ISO_LANG_CODE_TR, "Dil için iso kodu belirt"},
#endif

/* c64/plus256k.c */
/* en */ {IDCLS_ENABLE_PLUS256K_EXPANSION,    N_("Enable the PLUS256K RAM expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_PLUS256K_EXPANSION_DA, "Aktivér PLUS256K RAM-udviddelse"},
/* de */ {IDCLS_ENABLE_PLUS256K_EXPANSION_DE, "PLUS256K RAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_PLUS256K_EXPANSION_FR, "Activer l'expansion PLUS256K RAM"},
/* hu */ {IDCLS_ENABLE_PLUS256K_EXPANSION_HU, "PLUS256K RAM kiterjesztés engedélyezése"},
/* it */ {IDCLS_ENABLE_PLUS256K_EXPANSION_IT, "Attiva l'espansione PLUS256K RAM"},
/* nl */ {IDCLS_ENABLE_PLUS256K_EXPANSION_NL, "Activeer de PLUS256K-geheugenuitbreiding"},
/* pl */ {IDCLS_ENABLE_PLUS256K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_PLUS256K_EXPANSION_SV, "Aktivera PLUS256K RAM-expansion"},
/* tr */ {IDCLS_ENABLE_PLUS256K_EXPANSION_TR, "PLUS256K RAM geniþletmesini aktif et"},
#endif

/* c64/plus256k.c */
/* en */ {IDCLS_DISABLE_PLUS256K_EXPANSION,    N_("Disable the PLUS256K RAM expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_PLUS256K_EXPANSION_DA, "Deaktiver PLUS256K RAM-udviddelse"},
/* de */ {IDCLS_DISABLE_PLUS256K_EXPANSION_DE, "PLUS256K RAM Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_PLUS256K_EXPANSION_FR, "Désactiver l'expansion PLUS256K RAM"},
/* hu */ {IDCLS_DISABLE_PLUS256K_EXPANSION_HU, "PLUS256K RAM kiterjesztés tiltása"},
/* it */ {IDCLS_DISABLE_PLUS256K_EXPANSION_IT, "Disattiva l'espansione PLUS256K RAM"},
/* nl */ {IDCLS_DISABLE_PLUS256K_EXPANSION_NL, "De PLUS256K-geheugenuitbreiding uitschakelen"},
/* pl */ {IDCLS_DISABLE_PLUS256K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_PLUS256K_EXPANSION_SV, "Inaktivera PLUS256K RAM-expansion"},
/* tr */ {IDCLS_DISABLE_PLUS256K_EXPANSION_TR, "PLUS256K RAM geniþletmesini pasifleþtir"},
#endif

/* c64/plus256k.c */
/* en */ {IDCLS_SPECIFY_PLUS256K_NAME,    N_("Specify name of PLUS256K image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PLUS256K_NAME_DA, "Angiv navn på PLUS256K-image"},
/* de */ {IDCLS_SPECIFY_PLUS256K_NAME_DE, "Name der PLUS256K Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_PLUS256K_NAME_FR, "Spécifier le nom de l'image PLUS256K RAM"},
/* hu */ {IDCLS_SPECIFY_PLUS256K_NAME_HU, "Adja meg a PLUS256K-s képmás nevét"},
/* it */ {IDCLS_SPECIFY_PLUS256K_NAME_IT, "Specifica il nome dell'immagine PLUS256K"},
/* nl */ {IDCLS_SPECIFY_PLUS256K_NAME_NL, "Geef de naam van het PLUS256K-bestand"},
/* pl */ {IDCLS_SPECIFY_PLUS256K_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PLUS256K_NAME_SV, "Ange namn på PLUS256K-avbildning"},
/* tr */ {IDCLS_SPECIFY_PLUS256K_NAME_TR, "PLUS256K imajýnýn ismini belirt"},
#endif

/* c64/plus60k.c */
/* en */ {IDCLS_ENABLE_PLUS60K_EXPANSION,    N_("Enable the PLUS60K RAM expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_PLUS60K_EXPANSION_DA, "Aktivér PLUS60K RAM-udviddelse"},
/* de */ {IDCLS_ENABLE_PLUS60K_EXPANSION_DE, "PLUS60K RAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_PLUS60K_EXPANSION_FR, "Activer l'expansion PLUS60K RAM"},
/* hu */ {IDCLS_ENABLE_PLUS60K_EXPANSION_HU, "PLUS60K RAM bõvítés engedélyezése"},
/* it */ {IDCLS_ENABLE_PLUS60K_EXPANSION_IT, "Attiva l'espansione PLUS60K RAM"},
/* nl */ {IDCLS_ENABLE_PLUS60K_EXPANSION_NL, "Activeer de PLUS60K-geheugenuitbreiding"},
/* pl */ {IDCLS_ENABLE_PLUS60K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_PLUS60K_EXPANSION_SV, "Aktivera PLUS60K RAM-expansion"},
/* tr */ {IDCLS_ENABLE_PLUS60K_EXPANSION_TR, "PLUS60K RAM geniþletmesini aktif et"},
#endif

/* c64/plus60k.c */
/* en */ {IDCLS_DISABLE_PLUS60K_EXPANSION,    N_("Disable the PLUS60K RAM expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_PLUS60K_EXPANSION_DA, "Deaktiver PLUS60K RAM-udviddelse"},
/* de */ {IDCLS_DISABLE_PLUS60K_EXPANSION_DE, "PLUS60K RAM Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_PLUS60K_EXPANSION_FR, "Désactiver l'expansion PLUS60K RAM"},
/* hu */ {IDCLS_DISABLE_PLUS60K_EXPANSION_HU, "PLUS60K RAM bõvítés tiltása"},
/* it */ {IDCLS_DISABLE_PLUS60K_EXPANSION_IT, "Disattiva l'espansione PLUS60K RAM"},
/* nl */ {IDCLS_DISABLE_PLUS60K_EXPANSION_NL, "De PLUS60K-geheugenuitbreiding uitschakelen"},
/* pl */ {IDCLS_DISABLE_PLUS60K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_PLUS60K_EXPANSION_SV, "Inaktivera PLUS60K RAM-expansion"},
/* tr */ {IDCLS_DISABLE_PLUS60K_EXPANSION_TR, "PLUS60K RAM geniþletmesini pasifleþtir"},
#endif

/* c64/plus60k.c */
/* en */ {IDCLS_SPECIFY_PLUS60K_NAME,    N_("Specify name of PLUS60K image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PLUS60K_NAME_DA, "Angiv navn på PLUS60K-image"},
/* de */ {IDCLS_SPECIFY_PLUS60K_NAME_DE, "Name der PLUS60K Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_PLUS60K_NAME_FR, "Spécifier le nom de l'image PLUS60K RAM"},
/* hu */ {IDCLS_SPECIFY_PLUS60K_NAME_HU, "Adja meg a nevét a PLUS60K képmásnak"},
/* it */ {IDCLS_SPECIFY_PLUS60K_NAME_IT, "Specifica il nome dell'immagine PLUS60K"},
/* nl */ {IDCLS_SPECIFY_PLUS60K_NAME_NL, "Geef de naam van het PLUS60K-bestand"},
/* pl */ {IDCLS_SPECIFY_PLUS60K_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PLUS60K_NAME_SV, "Ange namn på PLUS60K-avbildning"},
/* tr */ {IDCLS_SPECIFY_PLUS60K_NAME_TR, "PLUS60K imajýnýn ismini belirt"},
#endif

/* c64/plus60k.c */
/* en */ {IDCLS_PLUS60K_BASE,    N_("Base address of the PLUS60K expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PLUS60K_BASE_DA, "Baseadresse for PLUS60K-udviddelse"},
/* de */ {IDCLS_PLUS60K_BASE_DE, "Basis Adresse für PLUS60K Erweiterung"},
/* fr */ {IDCLS_PLUS60K_BASE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_PLUS60K_BASE_HU, "A PLUS60K bõvítés báziscíme"},
/* it */ {IDCLS_PLUS60K_BASE_IT, "Indirizzo base dell'espansione PLUS60K"},
/* nl */ {IDCLS_PLUS60K_BASE_NL, "Basisadres van de PLUS60K-geheugenuitbreiding"},
/* pl */ {IDCLS_PLUS60K_BASE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PLUS60K_BASE_SV, "Basadress för PLUS60K-expansion"},
/* tr */ {IDCLS_PLUS60K_BASE_TR, "PLUS60K geniþletmesinin taban adresi"},
#endif

/* c64/c64_256k.c */
/* en */ {IDCLS_ENABLE_C64_256K_EXPANSION,    N_("Enable the 256K RAM expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_C64_256K_EXPANSION_DA, "Aktivér 256K RAM-udviddelse"},
/* de */ {IDCLS_ENABLE_C64_256K_EXPANSION_DE, "256K RAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_C64_256K_EXPANSION_FR, "Activer l'expansion RAM 256K"},
/* hu */ {IDCLS_ENABLE_C64_256K_EXPANSION_HU, "256K RAM kiterjesztés engedélyezése"},
/* it */ {IDCLS_ENABLE_C64_256K_EXPANSION_IT, "Attiva l'espansione 256K RAM"},
/* nl */ {IDCLS_ENABLE_C64_256K_EXPANSION_NL, "Activeer de 256K-geheugenuitbreiding"},
/* pl */ {IDCLS_ENABLE_C64_256K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_C64_256K_EXPANSION_SV, "Aktivera 256K RAM-expansion"},
/* tr */ {IDCLS_ENABLE_C64_256K_EXPANSION_TR, "256K RAM geniþletmesini aktif et"},
#endif

/* c64/c64_256k.c */
/* en */ {IDCLS_DISABLE_C64_256K_EXPANSION,    N_("Disable the 256K RAM expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_C64_256K_EXPANSION_DA, "Deaktiver 256K RAM-udviddelse"},
/* de */ {IDCLS_DISABLE_C64_256K_EXPANSION_DE, "256K RAM Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_C64_256K_EXPANSION_FR, "Désactiver l'expansion RAM 256K"},
/* hu */ {IDCLS_DISABLE_C64_256K_EXPANSION_HU, "256K RAM kiterjesztés tiltása"},
/* it */ {IDCLS_DISABLE_C64_256K_EXPANSION_IT, "Disattiva l'espansione 256K RAM"},
/* nl */ {IDCLS_DISABLE_C64_256K_EXPANSION_NL, "De 256K-geheugenuitbreiding uitschakelen"},
/* pl */ {IDCLS_DISABLE_C64_256K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_C64_256K_EXPANSION_SV, "Inaktivera 256K RAM-expansion"},
/* tr */ {IDCLS_DISABLE_C64_256K_EXPANSION_TR, "256K RAM geniþletmesini pasifleþtir"},
#endif

/* c64/c64_256k.c */
/* en */ {IDCLS_SPECIFY_C64_256K_NAME,    N_("Specify name of 256K image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_C64_256K_NAME_DA, "Angiv navn på 256K-image"},
/* de */ {IDCLS_SPECIFY_C64_256K_NAME_DE, "Name der 256K Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_C64_256K_NAME_FR, "Spécifier le nom de l'image 256K"},
/* hu */ {IDCLS_SPECIFY_C64_256K_NAME_HU, "Adja meg a 256K-s képmás nevét"},
/* it */ {IDCLS_SPECIFY_C64_256K_NAME_IT, "Specifica il nome dell'immagine 256K"},
/* nl */ {IDCLS_SPECIFY_C64_256K_NAME_NL, "Geef de naam van het 256K-bestand"},
/* pl */ {IDCLS_SPECIFY_C64_256K_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_C64_256K_NAME_SV, "Ange namn på 256K-avbildning"},
/* tr */ {IDCLS_SPECIFY_C64_256K_NAME_TR, "256K imajýnýn ismini belirt"},
#endif

/* c64/c64_256k.c */
/* en */ {IDCLS_C64_256K_BASE,    N_("Base address of the 256K expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_C64_256K_BASE_DA, "Baseadresse for 256K-udviddelse"},
/* de */ {IDCLS_C64_256K_BASE_DE, "Basis Adresse für 256K RAM Erweiterung"},
/* fr */ {IDCLS_C64_256K_BASE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_C64_256K_BASE_HU, "A 256K-s RAM kiterjesztés báziscíme"},
/* it */ {IDCLS_C64_256K_BASE_IT, "Indirizzo base dell'espansione 256K"},
/* nl */ {IDCLS_C64_256K_BASE_NL, "Basisadres van de 256K-geheugenuitbreiding"},
/* pl */ {IDCLS_C64_256K_BASE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_C64_256K_BASE_SV, "Basadress för 256K-expansionen"},
/* tr */ {IDCLS_C64_256K_BASE_TR, "256K geniþletmesinin taban adresi"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_STB_CART,    "Attach raw Structured Basic cartridge image"},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_RAW_STB_CART_DA, "Tilslut rå Strucured BASIC-cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_STB_CART_DE, "Structured Basic (raw) Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_STB_CART_FR, "Insérer une cartouche Raw Structured Basic"},
/* hu */ {IDCLS_ATTACH_RAW_STB_CART_HU, "Structured Basic cartridge képmás csatolása"},
/* it */ {IDCLS_ATTACH_RAW_STB_CART_IT, "Seleziona l'immagine di una cartuccia Structured Basic"},
/* nl */ {IDCLS_ATTACH_RAW_STB_CART_NL, "Koppel binair Structured-Basic-cartridgebestand aan"},
/* pl */ {IDCLS_ATTACH_RAW_STB_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_STB_CART_SV, "Anslut rå Strucured Basic-insticksmodulfil"},
/* tr */ {IDCLS_ATTACH_RAW_STB_CART_TR, "Düz yapýlandýrýlmýþ Basic kartuþ imajý yerleþtir"},
#endif

/* plus4/plus4memcsory256k.c */
/* en */ {IDCLS_ENABLE_CS256K_EXPANSION,    N_("Enable the CSORY 256K RAM expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_CS256K_EXPANSION_DA, "Aktivér CSORY 256K RAM-udviddelse"},
/* de */ {IDCLS_ENABLE_CS256K_EXPANSION_DE, "CSORY 256K RAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_CS256K_EXPANSION_FR, "Activer l'expansion RAM 256K CSORY"},
/* hu */ {IDCLS_ENABLE_CS256K_EXPANSION_HU, "CSORY 256K RAM kiterjesztés engedélyezése"},
/* it */ {IDCLS_ENABLE_CS256K_EXPANSION_IT, "Attiva l'espansione RAM CSORY 256K"},
/* nl */ {IDCLS_ENABLE_CS256K_EXPANSION_NL, "Activeer de CSORY 256K-geheugenuitbreiding"},
/* pl */ {IDCLS_ENABLE_CS256K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_CS256K_EXPANSION_SV, "Aktivera CSORY 256K RAM-expansion"},
/* tr */ {IDCLS_ENABLE_CS256K_EXPANSION_TR, "CSORY 256K RAM geniþletmesini aktif et"},
#endif

/* plus4/plus4memhannes256k.c */
/* en */ {IDCLS_ENABLE_H256K_EXPANSION,    N_("Enable the HANNES 256K RAM expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_H256K_EXPANSION_DA, "Aktivér HANNES 256K RAM-udviddelse"},
/* de */ {IDCLS_ENABLE_H256K_EXPANSION_DE, "HANNES 256K RAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_H256K_EXPANSION_FR, "Activer l'expansion RAM 256K HANNES"},
/* hu */ {IDCLS_ENABLE_H256K_EXPANSION_HU, "HANNES 256K RAM kiterjesztés engedélyezése"},
/* it */ {IDCLS_ENABLE_H256K_EXPANSION_IT, "Attiva l'espansione RAM HANNES 256K"},
/* nl */ {IDCLS_ENABLE_H256K_EXPANSION_NL, "Activeer de HANNES 256K-geheugenuitbreiding"},
/* pl */ {IDCLS_ENABLE_H256K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_H256K_EXPANSION_SV, "Aktivera HANNES 256K RAM-expansion"},
/* tr */ {IDCLS_ENABLE_H256K_EXPANSION_TR, "HANNES 256K RAM geniþletmesini aktif et"},
#endif

/* plus4/plus4memhannes256k.c */
/* en */ {IDCLS_ENABLE_H1024K_EXPANSION,    N_("Enable the HANNES 1024K RAM expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_H1024K_EXPANSION_DA, "Aktivér HANNES 1024K RAM-udviddelse"},
/* de */ {IDCLS_ENABLE_H1024K_EXPANSION_DE, "HANNES 1024K RAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_H1024K_EXPANSION_FR, "Activer l'expansion RAM 1024K HANNES"},
/* hu */ {IDCLS_ENABLE_H1024K_EXPANSION_HU, "HANNES 1024K RAM kiterjesztés engedélyezése"},
/* it */ {IDCLS_ENABLE_H1024K_EXPANSION_IT, "Attiva l'espansione RAM HANNES 1024K"},
/* nl */ {IDCLS_ENABLE_H1024K_EXPANSION_NL, "Activeer de HANNES 1024K-geheugenuitbreiding"},
/* pl */ {IDCLS_ENABLE_H1024K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_H1024K_EXPANSION_SV, "Aktivera HANNES 1024K RAM-expansion"},
/* tr */ {IDCLS_ENABLE_H1024K_EXPANSION_TR, "HANNES 1024K RAM geniþletmesini aktif et"},
#endif

/* plus4/plus4memhannes256k.c */
/* en */ {IDCLS_ENABLE_H4096K_EXPANSION,    N_("Enable the HANNES 4096K RAM expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_H4096K_EXPANSION_DA, "Aktivér HANNES 4096K RAM-udviddelse"},
/* de */ {IDCLS_ENABLE_H4096K_EXPANSION_DE, "HANNES 4096K RAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_H4096K_EXPANSION_FR, "Activer l'expansion RAM 4096K HANNES"},
/* hu */ {IDCLS_ENABLE_H4096K_EXPANSION_HU, "HANNES 4096K RAM kiterjesztés engedélyezése"},
/* it */ {IDCLS_ENABLE_H4096K_EXPANSION_IT, "Attiva l'espansione RAM HANNES 4096K"},
/* nl */ {IDCLS_ENABLE_H4096K_EXPANSION_NL, "Activeer de HANNES 4096K-geheugenuitbreiding"},
/* pl */ {IDCLS_ENABLE_H4096K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_H4096K_EXPANSION_SV, "Aktivera HANNES 4096K RAM-expansion"},
/* tr */ {IDCLS_ENABLE_H4096K_EXPANSION_TR, "HANNES 4096K RAM geniþletmesini aktif et"},
#endif

/* c64dtv/c64dtvblitter.c */
/* en */ {IDCLS_SPECIFY_DTV_REVISION,    N_("Specify DTV Revision (2: DTV2, 3: DTV3)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_DTV_REVISION_DA, "Angiv DTV-revision (2: DTV2, 3: DTV3)"},
/* de */ {IDCLS_SPECIFY_DTV_REVISION_DE, "DTV Revision definieren (2: DTV2, 3: DTV3)"},
/* fr */ {IDCLS_SPECIFY_DTV_REVISION_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_DTV_REVISION_HU, "Adja meg a DTV verzióját (2: DTV2, 3: DTV3)"},
/* it */ {IDCLS_SPECIFY_DTV_REVISION_IT, "Specifica la revisione DTV (2: DTV2, 3: DTV3)"},
/* nl */ {IDCLS_SPECIFY_DTV_REVISION_NL, "Geef de DTV revisie (2: DTV2, 3: DTV3)"},
/* pl */ {IDCLS_SPECIFY_DTV_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_DTV_REVISION_SV, "Ange DTV-revision (2: DTV2, 3: DTV3)"},
/* tr */ {IDCLS_SPECIFY_DTV_REVISION_TR, "DTV Revizyonunu belirt (2: DTV2, 3: DTV3)"},
#endif

/* c64dtv/c64dtvblitter.c */
/* en */ {IDCLS_ENABLE_DTV_BLITTER_LOG,    N_("Enable DTV blitter logs.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DTV_BLITTER_LOG_DA, "Aktivér DTV-blitterlog."},
/* de */ {IDCLS_ENABLE_DTV_BLITTER_LOG_DE, "DTV Blitter Log aktivieren."},
/* fr */ {IDCLS_ENABLE_DTV_BLITTER_LOG_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_DTV_BLITTER_LOG_HU, "DTV blit naplók engedélyezése."},
/* it */ {IDCLS_ENABLE_DTV_BLITTER_LOG_IT, "Attiva log blitter DTV."},
/* nl */ {IDCLS_ENABLE_DTV_BLITTER_LOG_NL, "Aktiveer DTV blitter logs."},
/* pl */ {IDCLS_ENABLE_DTV_BLITTER_LOG_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DTV_BLITTER_LOG_SV, "Aktivera DTV-blitterloggar."},
/* tr */ {IDCLS_ENABLE_DTV_BLITTER_LOG_TR, "DTV blitter loglarýný aktif et"},
#endif

/* c64dtv/c64dtvblitter.c */
/* en */ {IDCLS_DISABLE_DTV_BLITTER_LOG,    N_("Disable DTV blitter logs.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DTV_BLITTER_LOG_DA, "Deaktiver DTV-blitterlog."},
/* de */ {IDCLS_DISABLE_DTV_BLITTER_LOG_DE, "DTV Blitter Log deaktivieren."},
/* fr */ {IDCLS_DISABLE_DTV_BLITTER_LOG_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_DTV_BLITTER_LOG_HU, "DTV blit naplók tiltása."},
/* it */ {IDCLS_DISABLE_DTV_BLITTER_LOG_IT, "Disattiva log blitter DTV."},
/* nl */ {IDCLS_DISABLE_DTV_BLITTER_LOG_NL, "DTV Blitter logs afsluiten."},
/* pl */ {IDCLS_DISABLE_DTV_BLITTER_LOG_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DTV_BLITTER_LOG_SV, "Inaktivera DTV-blitterloggar."},
/* tr */ {IDCLS_DISABLE_DTV_BLITTER_LOG_TR, "DTV blitter loglarýný pasifleþtir"},
#endif

/* c64dtv/c64dtvdma.c */
/* en */ {IDCLS_ENABLE_DTV_DMA_LOG,    N_("Enable DTV DMA logs.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DTV_DMA_LOG_DA, "Aktivér DTV DMA-log."},
/* de */ {IDCLS_ENABLE_DTV_DMA_LOG_DE, "DTV DMA Logs aktivieren."},
/* fr */ {IDCLS_ENABLE_DTV_DMA_LOG_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_DTV_DMA_LOG_HU, "DTV DMA naplók engedélyezése."},
/* it */ {IDCLS_ENABLE_DTV_DMA_LOG_IT, "Attiva log DMA DTV."},
/* nl */ {IDCLS_ENABLE_DTV_DMA_LOG_NL, "Aktiveer DTV DMA logs."},
/* pl */ {IDCLS_ENABLE_DTV_DMA_LOG_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DTV_DMA_LOG_SV, "Aktivera DTV-dma-loggar."},
/* tr */ {IDCLS_ENABLE_DTV_DMA_LOG_TR, "DTV DMA loglarýný aktif et"},
#endif

/* c64dtv/c64dtvdma.c */
/* en */ {IDCLS_DISABLE_DTV_DMA_LOG,    N_("Disable DTV DMA logs.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DTV_DMA_LOG_DA, "Deaktiver DTV DMA-log."},
/* de */ {IDCLS_DISABLE_DTV_DMA_LOG_DE, "DTV DMA Logs deaktivieren."},
/* fr */ {IDCLS_DISABLE_DTV_DMA_LOG_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_DTV_DMA_LOG_HU, "DTV dma naplók tiltása."},
/* it */ {IDCLS_DISABLE_DTV_DMA_LOG_IT, "Disattiva log DMA DTV."},
/* nl */ {IDCLS_DISABLE_DTV_DMA_LOG_NL, "DTV DMA logs afsluiten."},
/* pl */ {IDCLS_DISABLE_DTV_DMA_LOG_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DTV_DMA_LOG_SV, "Inaktivera DTV-dma-loggar."},
/* tr */ {IDCLS_DISABLE_DTV_DMA_LOG_TR, "DTV DMA loglarýný pasifleþtir"},
#endif

/* c64dtv/c64dtvflash.c */
/* en */ {IDCLS_SPECIFY_C64DTVROM_NAME,    N_("Specify name of C64DTV ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_C64DTVROM_NAME_DA, "Angiv navn på C64DTV-ROM-image"},
/* de */ {IDCLS_SPECIFY_C64DTVROM_NAME_DE, "Name von C64DTV ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_C64DTVROM_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_C64DTVROM_NAME_HU, "Adja meg a C64DTV ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_C64DTVROM_NAME_IT, "Specifica il nome dell'immagine ROM DTV"},
/* nl */ {IDCLS_SPECIFY_C64DTVROM_NAME_NL, "Geef de naam van her C64DTV ROM bestand"},
/* pl */ {IDCLS_SPECIFY_C64DTVROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_C64DTVROM_NAME_SV, "Ange namn på C64DTV-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_C64DTVROM_NAME_TR, "C64DTV ROM imajýnýn ismini belirt"},
#endif

/* c64dtv/c64dtvflash.c */
/* en */ {IDCLS_ENABLE_C64DTVROM_RW,    N_("Enable writing to C64DTV ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_C64DTVROM_RW_DA, "Tillad skrivning til C64DTV-ROM-image."},
/* de */ {IDCLS_ENABLE_C64DTVROM_RW_DE, "Schreibzugriff auf C64DTV ROM Image erlauben"},
/* fr */ {IDCLS_ENABLE_C64DTVROM_RW_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_C64DTVROM_RW_HU, "C64DTV ROM képmásba írás engedélyezése"},
/* it */ {IDCLS_ENABLE_C64DTVROM_RW_IT, "Attiva la scrittura sull'immagine ROM C64DTV"},
/* nl */ {IDCLS_ENABLE_C64DTVROM_RW_NL, "Aktiveer schrijven naar C64DTV ROM bestand"},
/* pl */ {IDCLS_ENABLE_C64DTVROM_RW_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_C64DTVROM_RW_SV, "Aktivera skrivning till C64DTV-ROM-avbildning."},
/* tr */ {IDCLS_ENABLE_C64DTVROM_RW_TR, "C64DTV ROM imajýna yazmayý aktif et"},
#endif

/* c64dtv/c64dtvflash.c */
/* en */ {IDCLS_DISABLE_C64DTVROM_RW,    N_("Disable writing to C64DTV ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_C64DTVROM_RW_DA, "Forbyd skrivning til C64DTV-ROM-image."},
/* de */ {IDCLS_DISABLE_C64DTVROM_RW_DE, "Schreibzugriff auf C64DTV ROM Image verhindern"},
/* fr */ {IDCLS_DISABLE_C64DTVROM_RW_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_C64DTVROM_RW_HU, "C64DTV ROM képmásba írás tiltása"},
/* it */ {IDCLS_DISABLE_C64DTVROM_RW_IT, "Disattiva la scrittura sull'immagine ROM C64DTV"},
/* nl */ {IDCLS_DISABLE_C64DTVROM_RW_NL, "Schrijven naar C64DTV ROM bestand afsluiten"},
/* pl */ {IDCLS_DISABLE_C64DTVROM_RW_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_C64DTVROM_RW_SV, "Inaktivera skrivning till C64DTV-ROM-avbildning."},
/* tr */ {IDCLS_DISABLE_C64DTVROM_RW_TR, "C64DTV ROM imajýna yazmayý pasifleþtir"},
#endif

/* c64dtv/c64dtvflash.c */
/* en */ {IDCLS_ENABLE_DTV_FLASH_LOG,    N_("Enable DTV flash chip logs.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DTV_FLASH_LOG_DA, "Aktivér logs for DTV-flashkreds."},
/* de */ {IDCLS_ENABLE_DTV_FLASH_LOG_DE, "DTV Flashchip Log aktivieren."},
/* fr */ {IDCLS_ENABLE_DTV_FLASH_LOG_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_DTV_FLASH_LOG_HU, "DTV flash chip naplók engedélyezése."},
/* it */ {IDCLS_ENABLE_DTV_FLASH_LOG_IT, "Attiva i log del chip flash DTV."},
/* nl */ {IDCLS_ENABLE_DTV_FLASH_LOG_NL, "Aktiveer DTV flash chip logs."},
/* pl */ {IDCLS_ENABLE_DTV_FLASH_LOG_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DTV_FLASH_LOG_SV, "Aktivera loggar för DTV-flashkrets."},
/* tr */ {IDCLS_ENABLE_DTV_FLASH_LOG_TR, "DTV flash çip loglarýný aktif et."},
#endif

/* c64dtv/c64dtvflash.c */
/* en */ {IDCLS_DISABLE_DTV_FLASH_LOG,    N_("Disable DTV flash chip logs.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DTV_FLASH_LOG_DA, "Deaktiver logs for DTV-flashkreds."},
/* de */ {IDCLS_DISABLE_DTV_FLASH_LOG_DE, "DTV Flashchip Log deaktivieren."},
/* fr */ {IDCLS_DISABLE_DTV_FLASH_LOG_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_DTV_FLASH_LOG_HU, "DTV flash chip naplók tiltása."},
/* it */ {IDCLS_DISABLE_DTV_FLASH_LOG_IT, "Disattiva i log del chip flash DTV."},
/* nl */ {IDCLS_DISABLE_DTV_FLASH_LOG_NL, "DTV flash chip logs afsluiten."},
/* pl */ {IDCLS_DISABLE_DTV_FLASH_LOG_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DTV_FLASH_LOG_SV, "Inaktivera loggar för DTV-flashkrets."},
/* tr */ {IDCLS_DISABLE_DTV_FLASH_LOG_TR, "DTV flash çip loglarýný pasifleþtir."},
#endif

/* c64dtv/flash-trap.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS,    N_("Use <name> as directory for flash file system device")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_DA, "Brug <navn> som katalog for filsystembaseret flashenhed"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_DE, "Benutze <Name> für Verzeichnis Gerät Flash Dateisystem"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_HU, "<név> könyvtár használata a flash fájlrendszer eszközhöz"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_IT, "Una <nome> come directory per il file system flash"},
/* nl */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_NL, "Gebruik <naam> als directory voor het flash bestandssysteem apparaat"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_SV, "Ange <namn> som katalog för filsystemsbaserad flashenhet"},
/* tr */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_TR, "Flash dosya sistem aygýtý için <isim>'i dizin olarak kullan"},
#endif

/* c64dtv/flash-trap.c */
/* en */ {IDCLS_ENABLE_TRUE_FLASH_FS,    N_("Enable true hardware flash file system")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_TRUE_FLASH_FS_DA, "Aktivér ægte hardwareemulering af flashfilsystem"},
/* de */ {IDCLS_ENABLE_TRUE_FLASH_FS_DE, "Präzises Hardware Flashdateisystem aktivieren"},
/* fr */ {IDCLS_ENABLE_TRUE_FLASH_FS_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_TRUE_FLASH_FS_HU, "Igazi hardver flash fájlrendszer engedélyezése"},
/* it */ {IDCLS_ENABLE_TRUE_FLASH_FS_IT, "Attiva l'emulazione hardware del file system flash"},
/* nl */ {IDCLS_ENABLE_TRUE_FLASH_FS_NL, "Activeer hardwarmatige flash bestandssysteem"},
/* pl */ {IDCLS_ENABLE_TRUE_FLASH_FS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TRUE_FLASH_FS_SV, "Aktivera maskinvarubaserat flash-filsystem"},
/* tr */ {IDCLS_ENABLE_TRUE_FLASH_FS_TR, "Gerçek donaným flash dosya sistemini aktif et"},
#endif

/* c64dtv/flash-trap.c */
/* en */ {IDCLS_DISABLE_TRUE_FLASH_FS,    N_("Disable true hardware flash file system")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_TRUE_FLASH_FS_DA, "Deaktiver ægte hardwareemulering af flashfilsystem"},
/* de */ {IDCLS_DISABLE_TRUE_FLASH_FS_DE, "Präzises Hardware Flashdateisystem deaktivieren"},
/* fr */ {IDCLS_DISABLE_TRUE_FLASH_FS_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_TRUE_FLASH_FS_HU, "Igazi hardver flash fájlrendszer tiltása"},
/* it */ {IDCLS_DISABLE_TRUE_FLASH_FS_IT, "Disattiva l'emulazione hardware del file system flash"},
/* nl */ {IDCLS_DISABLE_TRUE_FLASH_FS_NL, "Hardwatematig flash bestandssysteem uitschakelen"},
/* pl */ {IDCLS_DISABLE_TRUE_FLASH_FS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_TRUE_FLASH_FS_SV, "Inaktivera maskinvaruemelring för flashfilsystem"},
/* tr */ {IDCLS_DISABLE_TRUE_FLASH_FS_TR, "Gerçek donaným flash dosya sistemini pasifleþtir"},
#endif

#ifdef HAVE_MIDI
/* midi.c */
/* en */ {IDCLS_ENABLE_MIDI_EMU,    N_("Enable MIDI emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_MIDI_EMU_DA, "Aktivér MIDI-emulering"},
/* de */ {IDCLS_ENABLE_MIDI_EMU_DE, "MIDI Emulation aktivieren"},
/* fr */ {IDCLS_ENABLE_MIDI_EMU_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_MIDI_EMU_HU, "MIDI emuláció engedélyezése"},
/* it */ {IDCLS_ENABLE_MIDI_EMU_IT, "Attiva l'emulazione MIDI"},
/* nl */ {IDCLS_ENABLE_MIDI_EMU_NL, "Activeer MIDI emulatie"},
/* pl */ {IDCLS_ENABLE_MIDI_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_MIDI_EMU_SV, "Aktivera MIDI-emulering"},
/* tr */ {IDCLS_ENABLE_MIDI_EMU_TR, "MIDI emülasyonunu aktif et"},
#endif

/* midi.c */
/* en */ {IDCLS_DISABLE_MIDI_EMU,    N_("Disable MIDI emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_MIDI_EMU_DA, "Deaktiver MIDI-emulering"},
/* de */ {IDCLS_DISABLE_MIDI_EMU_DE, "MIDI Emulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_MIDI_EMU_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_MIDI_EMU_HU, "MIDI emuláció tiltása"},
/* it */ {IDCLS_DISABLE_MIDI_EMU_IT, "Disattiva l'emulazione MIDI"},
/* nl */ {IDCLS_DISABLE_MIDI_EMU_NL, "MIDI emulatie uitschakelen"},
/* pl */ {IDCLS_DISABLE_MIDI_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_MIDI_EMU_SV, "Inaktivera MIDI-emulering"},
/* tr */ {IDCLS_DISABLE_MIDI_EMU_TR, "MIDI emülasyonunu pasifleþtir"},
#endif

/* c64/c64-midi.c */
/* en */ {IDCLS_SPECIFY_C64_MIDI_TYPE,    N_("MIDI interface type (0: Sequential, 1: Passport, 2: DATEL, 3: Namesoft, 4: "
                                             "Maplin)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_C64_MIDI_TYPE_DA, "MIDI-interfacetype (0: Sekventiel, 1: Passport, 2: DATEL, 3: Namesoft, 4: "
                                          "Maplin)"},
/* de */ {IDCLS_SPECIFY_C64_MIDI_TYPE_DE, "MIDI Interface Typ (0: Sequential, 1: Passport, 2: DATEL, 3: Namesoft, 4: "
                                          "Maplin)"},
/* fr */ {IDCLS_SPECIFY_C64_MIDI_TYPE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_C64_MIDI_TYPE_HU, "MIDI interfész típus (0: soros, 1: Passport, 2: DATEL, 3: Namesoft, 4: Maplin)"},
/* it */ {IDCLS_SPECIFY_C64_MIDI_TYPE_IT, "Tipo interfaccia MIDI (0: Sequential, 1: Passport, 2: DATEL, 3: Namesoft, 4: "
                                          "Maplin)"},
/* nl */ {IDCLS_SPECIFY_C64_MIDI_TYPE_NL, "MIDI interface soort (0: Sequential, 1: Passport, 2: DATEL, 3: Namesoft, 4: "
                                          "Maplin)"},
/* pl */ {IDCLS_SPECIFY_C64_MIDI_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_C64_MIDI_TYPE_SV, "Typ av MIDI-gränssnitt (0: sekventiell, 1: Passport, 2: DATEL, 3: Namesoft, "
                                          "4: Maplin)"},
/* tr */ {IDCLS_SPECIFY_C64_MIDI_TYPE_TR, "MIDI arabirim tipi (0: Sequential, 1: Passport, 2: DATEL, 3: Namesoft, 4: "
                                          "Maplin)"},
#endif
#endif

/* c64/digimax.c */
/* en */ {IDCLS_ENABLE_DIGIMAX,    N_("Enable the digimax cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DIGIMAX_DA, "Aktivér Digimax-cartridge"},
/* de */ {IDCLS_ENABLE_DIGIMAX_DE, "Digimax Cartridge aktivieren"},
/* fr */ {IDCLS_ENABLE_DIGIMAX_FR, "Activer la cartouche Digimax"},
/* hu */ {IDCLS_ENABLE_DIGIMAX_HU, "Digimax cartridge engedélyezése"},
/* it */ {IDCLS_ENABLE_DIGIMAX_IT, "Attiva la cartuccia digimax"},
/* nl */ {IDCLS_ENABLE_DIGIMAX_NL, "Activeer de digimaxcartridge"},
/* pl */ {IDCLS_ENABLE_DIGIMAX_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DIGIMAX_SV, "Aktivera Digimax-cartridge"},
/* tr */ {IDCLS_ENABLE_DIGIMAX_TR, "Digimax kartuþunu aktif et"},
#endif

/* c64/digimax.c */
/* en */ {IDCLS_DISABLE_DIGIMAX,    N_("Disable the digimax cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DIGIMAX_DA, "Deaktiver Digimax-cartridge"},
/* de */ {IDCLS_DISABLE_DIGIMAX_DE, "Digimax Cartridge deaktivieren"},
/* fr */ {IDCLS_DISABLE_DIGIMAX_FR, "Désactiver la cartouche Digimax"},
/* hu */ {IDCLS_DISABLE_DIGIMAX_HU, "Digimax cartridge tiltása"},
/* it */ {IDCLS_DISABLE_DIGIMAX_IT, "Disattiva la cartuccia digimax"},
/* nl */ {IDCLS_DISABLE_DIGIMAX_NL, "De digimaxcartridge uitschakelen"},
/* pl */ {IDCLS_DISABLE_DIGIMAX_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DIGIMAX_SV, "Inaktivera Digimax Cartridge"},
/* tr */ {IDCLS_DISABLE_DIGIMAX_TR, "Digimax kartuþunu pasifleþtir"},
#endif

/* c64/digimax.c */
/* en */ {IDCLS_DIGIMAX_BASE,    N_("Base address of the digimax cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DIGIMAX_BASE_DA, "Baseadresse for Digimax-cartridge"},
/* de */ {IDCLS_DIGIMAX_BASE_DE, "Basis Adresse für Digimax Erweiterung"},
/* fr */ {IDCLS_DIGIMAX_BASE_FR, "Adresse de base de la cartouche Digimax"},
/* hu */ {IDCLS_DIGIMAX_BASE_HU, "A digimax cartridge báziscíme"},
/* it */ {IDCLS_DIGIMAX_BASE_IT, "Indirizzo base della cartuccia digimax"},
/* nl */ {IDCLS_DIGIMAX_BASE_NL, "Basisadres van de digimaxcartridge"},
/* pl */ {IDCLS_DIGIMAX_BASE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DIGIMAX_BASE_SV, "Basadress för Digimax-cartridge"},
/* tr */ {IDCLS_DIGIMAX_BASE_PL, "Digimax kartuþunun taban adresi"}
#endif

};

/* --------------------------------------------------------------------- */

static char *get_string_by_id(int id)
{
  unsigned int k;

  for (k = 0; k < countof(string_table); k++)
  {
    if (string_table[k].resource_id==id)
      return string_table[k].text;
  }
  return NULL;
}

#ifdef HAS_TRANSLATION
#include "translate_table.h"

static char *text_table[countof(translate_text_table)][countof(language_table)];

static void translate_text_init(void)
{
  unsigned int i,j;
  char *temp;

  for (i = 0; i < countof(language_table); i++)
  {
    for (j = 0; j < countof(translate_text_table); j++)
    {
      if (translate_text_table[j][i]==0)
        text_table[j][i]=NULL;
      else
      {
        temp=get_string_by_id(translate_text_table[j][i]);
        text_table[j][i]=intl_convert_cp(temp, language_cp_table[i]);
      }
    }
  }
}

char translate_id_error_text[30];

char *translate_text(int en_resource)
{
  unsigned int i;
  char *retval = NULL;

  if (en_resource == IDCLS_UNUSED)
      return NULL;

  if (en_resource == 0)
  {
    log_error(LOG_DEFAULT, "TRANSLATE ERROR: ID 0 was requested.");
    return "ID 0 translate error";
  }

  if (en_resource < 0x10000)
  {
    retval = intl_translate_text(en_resource);
  }
  else
  {
    for (i = 0; i < countof(translate_text_table); i++)
    {
      if (translate_text_table[i][0] == en_resource)
      {
        if (translate_text_table[i][current_language_index]!=0 &&
            text_table[i][current_language_index]!=NULL &&
            strlen(text_table[i][current_language_index])!=0)
          retval = text_table[i][current_language_index];
        else
          retval = text_table[i][0];
      }
    }
  }

  if (retval == NULL)
  {
    log_error(LOG_DEFAULT, "TRANSLATE ERROR: ID %d was requested, and would be returning NULL.",en_resource);
    sprintf(translate_id_error_text,"ID %d translate error",en_resource);
    retval = translate_id_error_text;
  }

  return retval;
}

int translate_res(int en_resource)
{
  return intl_translate_res(en_resource);
}

/* --------------------------------------------------------------------- */

static int set_current_language(const char *lang, void *param)
{
    unsigned int i;

    util_string_set(&current_language, "en");
    current_language_index = 0;

    if (strlen(lang) != 2)
        return 0;

    for (i = 0; i < countof(language_table); i++) {
        if (!strcasecmp(lang,language_table[i])) {
            current_language_index=i;
            util_string_set(&current_language, language_table[i]);
            intl_update_ui();
            return 0;
        }
    }

    return 0;
}

static const resource_string_t resources_string[] = {
    { "Language", "en", RES_EVENT_NO, NULL,
      &current_language, set_current_language, NULL },
    { NULL }
};

int translate_resources_init(void)
{
  intl_init();
  translate_text_init();

  return resources_register_string(resources_string);
}

void translate_resources_shutdown(void)
{
  unsigned int i,j;

  for (i = 0; i < countof(language_table); i++)
  {
    for (j = 0; j < countof(translate_text_table); j++)
    {
      if (text_table[j][i]!=NULL)
        lib_free(text_table[j][i]);
    }
  }
  intl_shutdown();
  lib_free(current_language);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-lang", SET_RESOURCE, 1,
      NULL, NULL, "Language", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_ISO_LANGUAGE_CODE, IDCLS_SPECIFY_ISO_LANG_CODE,
      NULL, NULL },
    { NULL }
};

int translate_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

void translate_arch_language_init(void)
{
  char *lang;

  lang=intl_arch_language_init();
  set_current_language(lang, "");
}
#else

char *translate_text(int en_resource)
{
  if (en_resource == IDCLS_UNUSED)
      return NULL;

  if (en_resource == 0)
  {
    log_error(LOG_DEFAULT, "TRANSLATE ERROR: ID 0 was requested.");
    return "ID 0 translate error";
  }

  return _(get_string_by_id(en_resource));
}
#endif
