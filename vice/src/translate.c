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

#ifdef HAS_TRANSLATION

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmdline.h"
#include "intl.h"
#include "lib.h"
#include "resources.h"
#include "translate.h"
#include "util.h"

#define countof(array) (sizeof(array) / sizeof((array)[0]))

typedef struct translate_s {
    int resource_id;
    char *text;
} translate_t;

char *current_language = NULL;
int current_language_index = 0;


/* The language table is usually duplicated in
   the arch intl.c, make sure they match
   when adding a new language */

static char *language_table[] = {

/* english */
  "en",

/* german */
  "de",

/* french */
  "fr",

/* italian */
  "it",

/* dutch */
  "nl",

/* polish */
  "pl",

/* swedish */
  "sv"
};


/* This is the codepage table, which holds the codepage
   used per language to encode the original text */


static int language_cp_table[] = {

/* english */
  28591,	/* ISO 8859-1 */

/* german */
  28591,	/* ISO 8859-1 */

/* french */
  28591,	/* ISO 8859-1 */

/* italian */
  28605,	/* ISO 8859-15 */

/* dutch */
  28591,	/* ISO 8859-1 */

/* polish */
  28592,	/* ISO 8859-2 */

/* swedish */
  28591	/* ISO 8859-1 */
};

/* GLOBAL STRING ID TEXT TABLE */

translate_t string_table[] = {

/* autostart.c */
/* en */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE,    "Cannot load snapshot file."},
/* de */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_DE, "Kann Snapshot Datei nicht laden."},
/* fr */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_FR, "Impossible de charger le fichier de sauvegarde."},
/* it */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_IT, "Non è possibile caricare il file di snapshot."},
/* nl */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_NL, "Kan momentopname bestand niet laden."},
/* pl */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE_SV, "Kan inte ladda ögonblicksbildfil."},

/* debug.c */
/* en */ {IDGS_PLAYBACK_ERROR_DIFFERENT,    "Playback error: %s different from line %d of file debug%06d"},
/* de */ {IDGS_PLAYBACK_ERROR_DIFFERENT_DE, ""},  /* fuzzy */
/* fr */ {IDGS_PLAYBACK_ERROR_DIFFERENT_FR, ""},  /* fuzzy */
/* it */ {IDGS_PLAYBACK_ERROR_DIFFERENT_IT, ""},  /* fuzzy */
/* nl */ {IDGS_PLAYBACK_ERROR_DIFFERENT_NL, "Afspeel fout: %s is anders dan lijn %d van bestand debug%06d"},
/* pl */ {IDGS_PLAYBACK_ERROR_DIFFERENT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_PLAYBACK_ERROR_DIFFERENT_SV, ""},  /* fuzzy */

/* event.c */
/* en */ {IDGS_CANNOT_CREATE_IMAGE_S,    "Cannot create image file %s"},
/* de */ {IDGS_CANNOT_CREATE_IMAGE_S_DE, "Kann Image `%s' nicht erzeugen."},  /* fuzzy */
/* fr */ {IDGS_CANNOT_CREATE_IMAGE_S_FR, "Impossible de créer l'image `%s'."},  /* fuzzy */
/* it */ {IDGS_CANNOT_CREATE_IMAGE_S_IT, "Non è possibile creare l'immagine `%s'."},  /* fuzzy */
/* nl */ {IDGS_CANNOT_CREATE_IMAGE_S_NL, "Kan bestand `%s' niet maken."},
/* pl */ {IDGS_CANNOT_CREATE_IMAGE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_CREATE_IMAGE_S_SV, ""},  /* fuzzy */

/* event.c */
/* en */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S,    "Cannot write image file %s"},
/* de */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_DE, ""},  /* fuzzy */
/* fr */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_FR, ""},  /* fuzzy */
/* it */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_IT, ""},  /* fuzzy */
/* nl */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_NL, "Kan niet schrijven naar bestand %s"},
/* pl */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_SV, ""},  /* fuzzy */

/* event.c */
/* en */ {IDGS_CANNOT_FIND_MAPPED_NAME_S,    "Cannot find mapped name for %s"},
/* de */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_DE, "Laden der Textschrift `%s' fehlgeschlagen."},  /* fuzzy */
/* fr */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_FR, "Impossible de charger la police de caractères %s."},  /* fuzzy */
/* it */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_IT, ""},  /* fuzzy */
/* nl */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_NL, "Kan vertaalde naam voor %s niet vinden"},
/* pl */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_SV, ""},  /* fuzzy */

/* event.c */
/* en */ {IDGS_CANT_CREATE_START_SNAP_S,    "Could not create start snapshot file %s."},
/* de */ {IDGS_CANT_CREATE_START_SNAP_S_DE, "Kann Start Snapshot Datei nicht erzeugen: %s"},
/* fr */ {IDGS_CANT_CREATE_START_SNAP_S_FR, "Impossible de créer le fichier de sauvegarde de départ %s."},
/* it */ {IDGS_CANT_CREATE_START_SNAP_S_IT, "Non è possibile creare il file di inizio snapshot %s."},
/* nl */ {IDGS_CANT_CREATE_START_SNAP_S_NL, "Kon het start momentopname bestand %s niet maken."},
/* pl */ {IDGS_CANT_CREATE_START_SNAP_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANT_CREATE_START_SNAP_S_SV, "Kunde inte skapa startögonblicksbildfilen %s."},

/* event.c */
/* en */ {IDGS_ERROR_READING_END_SNAP_S,    "Error reading end snapshot file %s."},
/* de */ {IDGS_ERROR_READING_END_SNAP_S_DE, "Kann Ende Snapshot Datei nicht lesen: %s"},
/* fr */ {IDGS_ERROR_READING_END_SNAP_S_FR, "Erreur de lecture dans le fichier de sauvegarde de fin %s."},
/* it */ {IDGS_ERROR_READING_END_SNAP_S_IT, "Errore durante la lettura del file di fine snapshot %s."},
/* nl */ {IDGS_ERROR_READING_END_SNAP_S_NL, "Fout bij het lezen van het eind van het momentopname bestand %s."},
/* pl */ {IDGS_ERROR_READING_END_SNAP_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ERROR_READING_END_SNAP_S_SV, "Fel vid läsning av slutögonblicksbildfilen %s."},

/* event.c */
/* en */ {IDGS_CANT_CREATE_END_SNAP_S,    "Could not create end snapshot file %s."},
/* de */ {IDGS_CANT_CREATE_END_SNAP_S_DE, "Kann Ende Snapshot Datei nicht erzeugen: %s"},
/* fr */ {IDGS_CANT_CREATE_END_SNAP_S_FR, "Impossible de créer le fichier de sauvegarde de fin %s."},
/* it */ {IDGS_CANT_CREATE_END_SNAP_S_IT, "Non è possibile creare il file di fine snapshot %s."},  /* fuzzy */
/* nl */ {IDGS_CANT_CREATE_END_SNAP_S_NL, "Kon het eind momentopname bestand %s niet maken."},
/* pl */ {IDGS_CANT_CREATE_END_SNAP_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANT_CREATE_END_SNAP_S_SV, "Kunde inte skapa slutögonblicksbildfilen %s."},

/* event.c */
/* en */ {IDGS_CANT_OPEN_END_SNAP_S,    "Could not open end snapshot file %s."},
/* de */ {IDGS_CANT_OPEN_END_SNAP_S_DE, "Kann Ende Snapshot Datei nicht öffnen: %s"},
/* fr */ {IDGS_CANT_OPEN_END_SNAP_S_FR, "Impossible d'ouvrir le fichier de sauvegarde de fin %s."},
/* it */ {IDGS_CANT_OPEN_END_SNAP_S_IT, "Non è possibile aprire il file di fine snapshot %s."},  /* fuzzy */
/* nl */ {IDGS_CANT_OPEN_END_SNAP_S_NL, "Kon het eind momentopname bestand %s niet openen."},
/* pl */ {IDGS_CANT_OPEN_END_SNAP_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANT_OPEN_END_SNAP_S_SV, "Kunde inte öppna slutögonblicksbildfilen %s."},

/* event.c */
/* en */ {IDGS_CANT_FIND_SECTION_END_SNAP,    "Could not find event section in end snapshot file."},
/* de */ {IDGS_CANT_FIND_SECTION_END_SNAP_DE, "Kann Sektion in Ende Snapshotdatei nicht finden."},
/* fr */ {IDGS_CANT_FIND_SECTION_END_SNAP_FR, "Impossible de trouver la section des événements dans le fichier de "
                                              "sauvegarde de fin."},
/* it */ {IDGS_CANT_FIND_SECTION_END_SNAP_IT, "Non è possibile trovare la sezione eventi nel file di fine snapshot."},
/* nl */ {IDGS_CANT_FIND_SECTION_END_SNAP_NL, "Kon de gebeurtenis sectie in eind momentopname bestand niet vinden."},
/* pl */ {IDGS_CANT_FIND_SECTION_END_SNAP_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANT_FIND_SECTION_END_SNAP_SV, "Kunde inte hinna händelsedelen i slutögonblicksbilden."},

/* event.c */
/* en */ {IDGS_ERROR_READING_START_SNAP_TRIED,    "Error reading start snapshot file. Tried %s and %s"},
/* de */ {IDGS_ERROR_READING_START_SNAP_TRIED_DE, "Fehler beim Lesen der Start Snapshot Datei."
                                                  " Versuch gescheitert bei %s und %s."},
/* fr */ {IDGS_ERROR_READING_START_SNAP_TRIED_FR, "Erreur de lecture  du fichier de sauvegarde de départ. "
                                                  "%s et %s ont été testés"},
/* it */ {IDGS_ERROR_READING_START_SNAP_TRIED_IT, "Errore durante la lettura del file di inizio snapshot. "
                                                  "Ho provato %s e %s"},
/* nl */ {IDGS_ERROR_READING_START_SNAP_TRIED_NL, "Fout bij het lezen van het start momentopname bestand. Heb %s en %s "
                                                  "geprobeerd"},
/* pl */ {IDGS_ERROR_READING_START_SNAP_TRIED_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ERROR_READING_START_SNAP_TRIED_SV, "Fel vid läsning av startögonblicksbildfil. Försökte med %s och %s"},

/* event.c */
/* en */ {IDGS_ERROR_READING_START_SNAP,    "Error reading start snapshot file."},
/* de */ {IDGS_ERROR_READING_START_SNAP_DE, "Fehler beim Lesen der Start Snapshot Datei."},
/* fr */ {IDGS_ERROR_READING_START_SNAP_FR, "Erreur de lecture du fichier de sauvegarde de départ."},
/* it */ {IDGS_ERROR_READING_START_SNAP_IT, "Errore durante la lettura del file di inizio snapshot."},
/* nl */ {IDGS_ERROR_READING_START_SNAP_NL, "Fout bij het lezen van het start momentopname bestand."},
/* pl */ {IDGS_ERROR_READING_START_SNAP_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ERROR_READING_START_SNAP_SV, "Fel vid läsning av startögonblicksbildfil."},

/* screenshot.c */
/* en */ {IDGS_SORRY_NO_MULTI_RECORDING,    "Sorry. Multiple recording is not supported."},
/* de */ {IDGS_SORRY_NO_MULTI_RECORDING_DE, "Eine Aufnahme ist zur Zeit aktiv. Mehrfache Aufnahme ist nicht möglich."},
/* fr */ {IDGS_SORRY_NO_MULTI_RECORDING_FR, "Désolé. Vous ne pouvez enregistrer plus d'une chose à la fois."},
/* it */ {IDGS_SORRY_NO_MULTI_RECORDING_IT, "Le registrazioni multiple non sono supportate."},
/* nl */ {IDGS_SORRY_NO_MULTI_RECORDING_NL, "Sorry. Meerdere opnames wordt niet ondersteund."},
/* pl */ {IDGS_SORRY_NO_MULTI_RECORDING_PL, ""},  /* fuzzy */
/* sv */ {IDGS_SORRY_NO_MULTI_RECORDING_SV, "Endast en inspelning kan göras åt gången."},

/* sound.c */
/* en */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED,    "write to sound device failed."},
/* de */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_DE, "Schreiben auf Sound Gerät ist fehlgeschlagen."},
/* fr */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_FR, "Impossible d'écriture sur le périphérique de son."},
/* it */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_IT, "scrittura sulla scheda audio fallita."},
/* nl */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_NL, "schrijf actie naar geluidsapparaat faalt."},
/* pl */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_PL, ""},  /* fuzzy */
/* sv */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_SV, "misslyckades att skriva till ljudenhet."},

/* sound.c */
/* en */ {IDGS_CANNOT_OPEN_SID_ENGINE,    "Cannot open SID engine"},
/* de */ {IDGS_CANNOT_OPEN_SID_ENGINE_DE, "Kann SID Engine nicht öffnen"},
/* fr */ {IDGS_CANNOT_OPEN_SID_ENGINE_FR, "Erreur de chargement de l'engin de son SID"},
/* it */ {IDGS_CANNOT_OPEN_SID_ENGINE_IT, "Non è possibile aprire il motore SID"},
/* nl */ {IDGS_CANNOT_OPEN_SID_ENGINE_NL, "Kan de SID kern niet openen"},
/* pl */ {IDGS_CANNOT_OPEN_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_OPEN_SID_ENGINE_SV, "Kan inte öppna SID-motor"},

/* sound.c */
/* en */ {IDGS_CANNOT_INIT_SID_ENGINE,    "Cannot initialize SID engine"},
/* de */ {IDGS_CANNOT_INIT_SID_ENGINE_DE, "Kann SID Engine nicht initialisieren"},
/* fr */ {IDGS_CANNOT_INIT_SID_ENGINE_FR, "Erreur d'initialisation de l'engin de son SID"},
/* it */ {IDGS_CANNOT_INIT_SID_ENGINE_IT, "Non è possibile inizializzare il motore SID"},
/* nl */ {IDGS_CANNOT_INIT_SID_ENGINE_NL, "Kan de SID kern niet initialiseren"},
/* pl */ {IDGS_CANNOT_INIT_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_INIT_SID_ENGINE_SV, "Kan inte initiera SID-motor"},

/* sound.c */
/* en */ {IDGS_INIT_FAILED_FOR_DEVICE_S,    "Initialization failed for device `%s'."},
/* de */ {IDGS_INIT_FAILED_FOR_DEVICE_S_DE, "Initialisierung von Gerät `%s' fehlgeschlagen."},
/* fr */ {IDGS_INIT_FAILED_FOR_DEVICE_S_FR, "erreur d'initialisation du périphérique `%s'."},
/* it */ {IDGS_INIT_FAILED_FOR_DEVICE_S_IT, "inizializzazione fallita per il device `%s'."},
/* nl */ {IDGS_INIT_FAILED_FOR_DEVICE_S_NL, "initialisatie voor apparaat `%s' faalt."},
/* pl */ {IDGS_INIT_FAILED_FOR_DEVICE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_INIT_FAILED_FOR_DEVICE_S_SV, "initiering misslyckades för enhet \"%s\"."},

/* sound.c */
/* en */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT,    "device '%s' not found or not supported."},
/* de */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_DE, "Gerät '%s' konnte nicht gefunden werden oder ist nicht unterstützt."},
/* fr */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_FR, "périphérique '%s' non trouvé ou non supporté."},
/* it */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_IT, "il device '%s' non è stato trovato oppure non è supportato."},
/* nl */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_NL, "apparaat '%s' niet gevonden of wordt niet ondersteund"},
/* pl */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT_SV, "enheten \"%s\" hittades eller stöds ej."},

/* sound.c */
/* en */ {IDGS_RECORD_DEVICE_S_NOT_EXIST,    "Recording device %s doesn't exist!"},
/* de */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_DE, "Aufnahme Gerät %s existiert nicht!"},
/* fr */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_FR, "Le périphérique d'enregistrement %s n'existe pas!"},
/* it */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_IT, "Il device di registrazione %s non esiste!"},
/* nl */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_NL, "Opname apparaat %s bestaat niet!"},
/* pl */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_SV, "Inspelningsenhet %s finns inte!"},

/* sound.c */
/* en */ {IDGS_RECORD_DIFFERENT_PLAYBACK,    "Recording device must be different from playback device"},
/* de */ {IDGS_RECORD_DIFFERENT_PLAYBACK_DE, "Aufnahme Gerät muß unteschiedlich vom Abspielgerät sein"},
/* fr */ {IDGS_RECORD_DIFFERENT_PLAYBACK_FR, "Le périphérique d'enregistrement doit être différent du "
                                             "périphérique de lecture"},
/* it */ {IDGS_RECORD_DIFFERENT_PLAYBACK_IT, "Il device di registrazione deve essere differente da quello "
                                             "di riproduzione"},
/* nl */ {IDGS_RECORD_DIFFERENT_PLAYBACK_NL, "Opname apparaat moet anders zijn dan afspeel apparaat"},
/* pl */ {IDGS_RECORD_DIFFERENT_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RECORD_DIFFERENT_PLAYBACK_SV, "Inspelningsenhet och återspelningsenhet kan inte vara samma"},

/* sound.c */
/* en */ {IDGS_WARNING_RECORDING_REALTIME,    "Warning! Recording device %s seems to be a realtime device!"},
/* de */ {IDGS_WARNING_RECORDING_REALTIME_DE, "Warnung! Aufnahme Gerät %s scheint ein Echtzeitgerät zu sein!"},
/* fr */ {IDGS_WARNING_RECORDING_REALTIME_FR, "Attention! Le périphérique d'enregistrement %s semble être un "
                                              "périphérique en temps réel"},
/* it */ {IDGS_WARNING_RECORDING_REALTIME_IT, "Attenzione! Il device di registrazione %s sembra essere un "
                                              "dispositivo realtime!"},
/* nl */ {IDGS_WARNING_RECORDING_REALTIME_NL, "Waarschuwing! Opname apparaat %s lijkt een realtime apparaat te zijn!"},
/* pl */ {IDGS_WARNING_RECORDING_REALTIME_PL, ""},  /* fuzzy */
/* sv */ {IDGS_WARNING_RECORDING_REALTIME_SV, "Varning! Inspelningsenheten %s verkar vara en realtidsenhet!"},

/* sound.c */
/* en */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR,    "The recording device doesn't support current sound parameters"},
/* de */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_DE, "Aufnahmegerät unterstütz derzeitige Soundeinstellungen nicht"},
/* fr */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_FR, "Le périphérique d'enregistrement ne supporte pas les paramètres "
                                                "de son actuellement configurés"},
/* it */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_IT, "Il device di registrazione non supporta i parametri attuali"},
/* nl */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_NL, "Het opname apparaat ondersteunt de huidige geluid opties niet"},
/* pl */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_SV, "Inspelningsenheten stöder inte aktuella ljudinställningar"},

/* sound.c */
/* en */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE,    "Sound buffer overflow (cycle based)"},
/* de */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_DE, "Sound Puffer Überlauf (Zyklus basiert)"},
/* fr */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_FR, "Erreur de dépassement de limite du tampon son (basé sur les cycles)"},
/* it */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_IT, "Overflow del buffer sonoro (cycle based)"},
/* nl */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_NL, "Geluidsbuffer overstroming (cyclus gebaseerd)"},
/* pl */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_PL, ""},  /* fuzzy */
/* sv */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_SV, "För mycket data i ljudbufferten (cykelbaserad)"},

/* sound.c */
/* en */ {IDGS_SOUND_BUFFER_OVERFLOW,    "Sound buffer overflow."},
/* de */ {IDGS_SOUND_BUFFER_OVERFLOW_DE, "Sound Puffer Überlauf."},
/* fr */ {IDGS_SOUND_BUFFER_OVERFLOW_FR, "Erreur de dépassement de limite du tampon son."},
/* it */ {IDGS_SOUND_BUFFER_OVERFLOW_IT, "Overflow del buffer sonoro."},
/* nl */ {IDGS_SOUND_BUFFER_OVERFLOW_NL, "Geluidsbuffer overstroming."},
/* pl */ {IDGS_SOUND_BUFFER_OVERFLOW_PL, ""},  /* fuzzy */
/* sv */ {IDGS_SOUND_BUFFER_OVERFLOW_SV, "För mycket data i ljudbufferten."},

/* sound.c */
/* en */ {IDGS_CANNOT_FLUSH,    "cannot flush."},
/* de */ {IDGS_CANNOT_FLUSH_DE, "Entleerung nicht möglich."},
/* fr */ {IDGS_CANNOT_FLUSH_FR, "impossible de vider."},
/* it */ {IDGS_CANNOT_FLUSH_IT, "non è possibile svuotare."},
/* nl */ {IDGS_CANNOT_FLUSH_NL, "kan niet spoelen."},
/* pl */ {IDGS_CANNOT_FLUSH_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_FLUSH_SV, "kan inte tömma."},

/* sound.c */
/* en */ {IDGS_FRAGMENT_PROBLEMS,    "fragment problems."},
/* de */ {IDGS_FRAGMENT_PROBLEMS_DE, "Fragmentierungsproblem."},
/* fr */ {IDGS_FRAGMENT_PROBLEMS_FR, "problèmes de fragments."},
/* it */ {IDGS_FRAGMENT_PROBLEMS_IT, "problemi di frammentazione."},
/* nl */ {IDGS_FRAGMENT_PROBLEMS_NL, "fragment problemen."},
/* pl */ {IDGS_FRAGMENT_PROBLEMS_PL, ""},  /* fuzzy */
/* sv */ {IDGS_FRAGMENT_PROBLEMS_SV, "fragmentprogram."},

/* sound.c */
/* en */ {IDGS_RUNNING_TOO_SLOW,    "running too slow."},
/* de */ {IDGS_RUNNING_TOO_SLOW_DE, "Ablauf zu langsam."},
/* fr */ {IDGS_RUNNING_TOO_SLOW_FR, "l'exécution est trop lente."},
/* it */ {IDGS_RUNNING_TOO_SLOW_IT, "esecuzione troppo lenta."},
/* nl */ {IDGS_RUNNING_TOO_SLOW_NL, "draait te langzaam."},
/* pl */ {IDGS_RUNNING_TOO_SLOW_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RUNNING_TOO_SLOW_SV, "går för långsamt."},

/* sound.c */
/* en */ {IDGS_STORE_SOUNDDEVICE_FAILED,    "store to sounddevice failed."},
/* de */ {IDGS_STORE_SOUNDDEVICE_FAILED_DE, "Speichern auf Sound Gerät ist fehlgeschlagen."},
/* fr */ {IDGS_STORE_SOUNDDEVICE_FAILED_FR, "erreur d'enregistrement sur le périphérique de son."},
/* it */ {IDGS_STORE_SOUNDDEVICE_FAILED_IT, "memorizzazione sulla scheda audio fallita."},
/* nl */ {IDGS_STORE_SOUNDDEVICE_FAILED_NL, "opslag naar geluidsapparaat faalt."},
/* pl */ {IDGS_STORE_SOUNDDEVICE_FAILED_PL, ""},  /* fuzzy */
/* sv */ {IDGS_STORE_SOUNDDEVICE_FAILED_SV, "misslyckades spara i ljudenheten."},

/* c64/c64export.c */
/* en */ {IDGS_RESOURCE_S_BLOCKED_BY_S,    "Resource %s blocked by %s."},
/* de */ {IDGS_RESOURCE_S_BLOCKED_BY_S_DE, ""},  /* fuzzy */
/* fr */ {IDGS_RESOURCE_S_BLOCKED_BY_S_FR, ""},  /* fuzzy */
/* it */ {IDGS_RESOURCE_S_BLOCKED_BY_S_IT, ""},  /* fuzzy */
/* nl */ {IDGS_RESOURCE_S_BLOCKED_BY_S_NL, "Bron %s geblokeerd door %s."},
/* pl */ {IDGS_RESOURCE_S_BLOCKED_BY_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RESOURCE_S_BLOCKED_BY_S_SV, ""}   /* fuzzy */

};

/* GLOBAL STRING ID TRANSLATION TABLE */

static int translate_text_table[][countof(language_table)] = {

/* autostart.c */
/* en */ {IDGS_CANNOT_LOAD_SNAPSHOT_FILE,
/* de */  IDGS_CANNOT_LOAD_SNAPSHOT_FILE_DE,
/* fr */  IDGS_CANNOT_LOAD_SNAPSHOT_FILE_FR,
/* it */  IDGS_CANNOT_LOAD_SNAPSHOT_FILE_IT,
/* nl */  IDGS_CANNOT_LOAD_SNAPSHOT_FILE_NL,
/* pl */  IDGS_CANNOT_LOAD_SNAPSHOT_FILE_PL,
/* sv */  IDGS_CANNOT_LOAD_SNAPSHOT_FILE_SV},

/* debug.c */
/* en */ {IDGS_PLAYBACK_ERROR_DIFFERENT,
/* de */  IDGS_PLAYBACK_ERROR_DIFFERENT_DE,
/* fr */  IDGS_PLAYBACK_ERROR_DIFFERENT_FR,
/* it */  IDGS_PLAYBACK_ERROR_DIFFERENT_IT,
/* nl */  IDGS_PLAYBACK_ERROR_DIFFERENT_NL,
/* pl */  IDGS_PLAYBACK_ERROR_DIFFERENT_PL,
/* sv */  IDGS_PLAYBACK_ERROR_DIFFERENT_SV},

/* event.c */
/* en */ {IDGS_CANNOT_CREATE_IMAGE_S,
/* de */  IDGS_CANNOT_CREATE_IMAGE_S_DE,
/* fr */  IDGS_CANNOT_CREATE_IMAGE_S_FR,
/* it */  IDGS_CANNOT_CREATE_IMAGE_S_IT,
/* nl */  IDGS_CANNOT_CREATE_IMAGE_S_NL,
/* pl */  IDGS_CANNOT_CREATE_IMAGE_S_PL,
/* sv */  IDGS_CANNOT_CREATE_IMAGE_S_SV},

/* event.c */
/* en */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S,
/* de */  IDGS_CANNOT_WRITE_IMAGE_FILE_S_DE,
/* fr */  IDGS_CANNOT_WRITE_IMAGE_FILE_S_FR,
/* it */  IDGS_CANNOT_WRITE_IMAGE_FILE_S_IT,
/* nl */  IDGS_CANNOT_WRITE_IMAGE_FILE_S_NL,
/* pl */  IDGS_CANNOT_WRITE_IMAGE_FILE_S_PL,
/* sv */  IDGS_CANNOT_WRITE_IMAGE_FILE_S_SV},

/* event.c */
/* en */ {IDGS_CANNOT_FIND_MAPPED_NAME_S,
/* de */  IDGS_CANNOT_FIND_MAPPED_NAME_S_DE,
/* fr */  IDGS_CANNOT_FIND_MAPPED_NAME_S_FR,
/* it */  IDGS_CANNOT_FIND_MAPPED_NAME_S_IT,
/* nl */  IDGS_CANNOT_FIND_MAPPED_NAME_S_NL,
/* pl */  IDGS_CANNOT_FIND_MAPPED_NAME_S_PL,
/* sv */  IDGS_CANNOT_FIND_MAPPED_NAME_S_SV},

/* event.c */
/* en */ {IDGS_CANT_CREATE_START_SNAP_S,
/* de */  IDGS_CANT_CREATE_START_SNAP_S_DE,
/* fr */  IDGS_CANT_CREATE_START_SNAP_S_FR,
/* it */  IDGS_CANT_CREATE_START_SNAP_S_IT,
/* nl */  IDGS_CANT_CREATE_START_SNAP_S_NL,
/* pl */  IDGS_CANT_CREATE_START_SNAP_S_PL,
/* sv */  IDGS_CANT_CREATE_START_SNAP_S_SV},

/* event.c */
/* en */ {IDGS_ERROR_READING_END_SNAP_S,
/* de */  IDGS_ERROR_READING_END_SNAP_S_DE,
/* fr */  IDGS_ERROR_READING_END_SNAP_S_FR,
/* it */  IDGS_ERROR_READING_END_SNAP_S_IT,
/* nl */  IDGS_ERROR_READING_END_SNAP_S_NL,
/* pl */  IDGS_ERROR_READING_END_SNAP_S_PL,
/* sv */  IDGS_ERROR_READING_END_SNAP_S_SV},

/* event.c */
/* en */ {IDGS_CANT_CREATE_END_SNAP_S,
/* de */  IDGS_CANT_CREATE_END_SNAP_S_DE,
/* fr */  IDGS_CANT_CREATE_END_SNAP_S_FR,
/* it */  IDGS_CANT_CREATE_END_SNAP_S_IT,
/* nl */  IDGS_CANT_CREATE_END_SNAP_S_NL,
/* pl */  IDGS_CANT_CREATE_END_SNAP_S_PL,
/* sv */  IDGS_CANT_CREATE_END_SNAP_S_SV},

/* event.c */
/* en */ {IDGS_CANT_OPEN_END_SNAP_S,
/* de */  IDGS_CANT_OPEN_END_SNAP_S_DE,
/* fr */  IDGS_CANT_OPEN_END_SNAP_S_FR,
/* it */  IDGS_CANT_OPEN_END_SNAP_S_IT,
/* nl */  IDGS_CANT_OPEN_END_SNAP_S_NL,
/* pl */  IDGS_CANT_OPEN_END_SNAP_S_PL,
/* sv */  IDGS_CANT_OPEN_END_SNAP_S_SV},

/* event.c */
/* en */ {IDGS_CANT_FIND_SECTION_END_SNAP,
/* de */  IDGS_CANT_FIND_SECTION_END_SNAP_DE,
/* fr */  IDGS_CANT_FIND_SECTION_END_SNAP_FR,
/* it */  IDGS_CANT_FIND_SECTION_END_SNAP_IT,
/* nl */  IDGS_CANT_FIND_SECTION_END_SNAP_NL,
/* pl */  IDGS_CANT_FIND_SECTION_END_SNAP_PL,
/* sv */  IDGS_CANT_FIND_SECTION_END_SNAP_SV},

/* event.c */
/* en */ {IDGS_ERROR_READING_START_SNAP_TRIED,
/* de */  IDGS_ERROR_READING_START_SNAP_TRIED_DE,
/* fr */  IDGS_ERROR_READING_START_SNAP_TRIED_FR,
/* it */  IDGS_ERROR_READING_START_SNAP_TRIED_IT,
/* nl */  IDGS_ERROR_READING_START_SNAP_TRIED_NL,
/* pl */  IDGS_ERROR_READING_START_SNAP_TRIED_PL,
/* sv */  IDGS_ERROR_READING_START_SNAP_TRIED_SV},

/* event.c */
/* en */ {IDGS_ERROR_READING_START_SNAP,
/* de */  IDGS_ERROR_READING_START_SNAP_DE,
/* fr */  IDGS_ERROR_READING_START_SNAP_FR,
/* it */  IDGS_ERROR_READING_START_SNAP_IT,
/* nl */  IDGS_ERROR_READING_START_SNAP_NL,
/* pl */  IDGS_ERROR_READING_START_SNAP_PL,
/* sv */  IDGS_ERROR_READING_START_SNAP_SV},

/* screenshot.c */
/* en */ {IDGS_SORRY_NO_MULTI_RECORDING,
/* de */  IDGS_SORRY_NO_MULTI_RECORDING_DE,
/* fr */  IDGS_SORRY_NO_MULTI_RECORDING_FR,
/* it */  IDGS_SORRY_NO_MULTI_RECORDING_IT,
/* nl */  IDGS_SORRY_NO_MULTI_RECORDING_NL,
/* pl */  IDGS_SORRY_NO_MULTI_RECORDING_PL,
/* sv */  IDGS_SORRY_NO_MULTI_RECORDING_SV},

/* sound.c */
/* en */ {IDGS_RECORD_DEVICE_S_NOT_EXIST,
/* de */  IDGS_RECORD_DEVICE_S_NOT_EXIST_DE,
/* fr */  IDGS_RECORD_DEVICE_S_NOT_EXIST_FR,
/* it */  IDGS_RECORD_DEVICE_S_NOT_EXIST_IT,
/* nl */  IDGS_RECORD_DEVICE_S_NOT_EXIST_NL,
/* pl */  IDGS_RECORD_DEVICE_S_NOT_EXIST_PL,
/* sv */  IDGS_RECORD_DEVICE_S_NOT_EXIST_SV},

/* sound.c */
/* en */ {IDGS_RECORD_DIFFERENT_PLAYBACK,
/* de */  IDGS_RECORD_DIFFERENT_PLAYBACK_DE,
/* fr */  IDGS_RECORD_DIFFERENT_PLAYBACK_FR,
/* it */  IDGS_RECORD_DIFFERENT_PLAYBACK_IT,
/* nl */  IDGS_RECORD_DIFFERENT_PLAYBACK_NL,
/* pl */  IDGS_RECORD_DIFFERENT_PLAYBACK_PL,
/* sv */  IDGS_RECORD_DIFFERENT_PLAYBACK_SV},

/* sound.c */
/* en */ {IDGS_WARNING_RECORDING_REALTIME,
/* de */  IDGS_WARNING_RECORDING_REALTIME_DE,
/* fr */  IDGS_WARNING_RECORDING_REALTIME_FR,
/* it */  IDGS_WARNING_RECORDING_REALTIME_IT,
/* nl */  IDGS_WARNING_RECORDING_REALTIME_NL,
/* pl */  IDGS_WARNING_RECORDING_REALTIME_PL,
/* sv */  IDGS_WARNING_RECORDING_REALTIME_SV},

/* sound.c */
/* en */ {IDGS_INIT_FAILED_FOR_DEVICE_S,
/* de */  IDGS_INIT_FAILED_FOR_DEVICE_S_DE,
/* fr */  IDGS_INIT_FAILED_FOR_DEVICE_S_FR,
/* it */  IDGS_INIT_FAILED_FOR_DEVICE_S_IT,
/* nl */  IDGS_INIT_FAILED_FOR_DEVICE_S_NL,
/* pl */  IDGS_INIT_FAILED_FOR_DEVICE_S_PL,
/* sv */  IDGS_INIT_FAILED_FOR_DEVICE_S_SV},

/* sound.c */
/* en */ {IDGS_RECORD_NOT_SUPPORT_SOUND_PAR,
/* de */  IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_DE,
/* fr */  IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_FR,
/* it */  IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_IT,
/* nl */  IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_NL,
/* pl */  IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_PL,
/* sv */  IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_SV},

/* sound.c */
/* en */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED,
/* de */  IDGS_WRITE_TO_SOUND_DEVICE_FAILED_DE,
/* fr */  IDGS_WRITE_TO_SOUND_DEVICE_FAILED_FR,
/* it */  IDGS_WRITE_TO_SOUND_DEVICE_FAILED_IT,
/* nl */  IDGS_WRITE_TO_SOUND_DEVICE_FAILED_NL,
/* pl */  IDGS_WRITE_TO_SOUND_DEVICE_FAILED_PL,
/* sv */  IDGS_WRITE_TO_SOUND_DEVICE_FAILED_SV},

/* sound.c */
/* en */ {IDGS_CANNOT_OPEN_SID_ENGINE,
/* de */  IDGS_CANNOT_OPEN_SID_ENGINE_DE,
/* fr */  IDGS_CANNOT_OPEN_SID_ENGINE_FR,
/* it */  IDGS_CANNOT_OPEN_SID_ENGINE_IT,
/* nl */  IDGS_CANNOT_OPEN_SID_ENGINE_NL,
/* pl */  IDGS_CANNOT_OPEN_SID_ENGINE_PL,
/* sv */  IDGS_CANNOT_OPEN_SID_ENGINE_SV},

/* sound.c */
/* en */ {IDGS_CANNOT_INIT_SID_ENGINE,
/* de */  IDGS_CANNOT_INIT_SID_ENGINE_DE,
/* fr */  IDGS_CANNOT_INIT_SID_ENGINE_FR,
/* it */  IDGS_CANNOT_INIT_SID_ENGINE_IT,
/* nl */  IDGS_CANNOT_INIT_SID_ENGINE_NL,
/* pl */  IDGS_CANNOT_INIT_SID_ENGINE_PL,
/* sv */  IDGS_CANNOT_INIT_SID_ENGINE_SV},

/* sound.c */
/* en */ {IDGS_DEVICE_S_NOT_FOUND_SUPPORT,
/* de */  IDGS_DEVICE_S_NOT_FOUND_SUPPORT_DE,
/* fr */  IDGS_DEVICE_S_NOT_FOUND_SUPPORT_FR,
/* it */  IDGS_DEVICE_S_NOT_FOUND_SUPPORT_IT,
/* nl */  IDGS_DEVICE_S_NOT_FOUND_SUPPORT_NL,
/* pl */  IDGS_DEVICE_S_NOT_FOUND_SUPPORT_PL,
/* sv */  IDGS_DEVICE_S_NOT_FOUND_SUPPORT_SV},

/* sound.c */
/* en */ {IDGS_SOUND_BUFFER_OVERFLOW_CYCLE,
/* de */  IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_DE,
/* fr */  IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_FR,
/* it */  IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_IT,
/* nl */  IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_NL,
/* pl */  IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_PL,
/* sv */  IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_SV},

/* sound.c */
/* en */ {IDGS_SOUND_BUFFER_OVERFLOW,
/* de */  IDGS_SOUND_BUFFER_OVERFLOW_DE,
/* fr */  IDGS_SOUND_BUFFER_OVERFLOW_FR,
/* it */  IDGS_SOUND_BUFFER_OVERFLOW_IT,
/* nl */  IDGS_SOUND_BUFFER_OVERFLOW_NL,
/* pl */  IDGS_SOUND_BUFFER_OVERFLOW_PL,
/* sv */  IDGS_SOUND_BUFFER_OVERFLOW_SV},

/* sound.c */
/* en */ {IDGS_CANNOT_FLUSH,
/* de */  IDGS_CANNOT_FLUSH_DE,
/* fr */  IDGS_CANNOT_FLUSH_FR,
/* it */  IDGS_CANNOT_FLUSH_IT,
/* nl */  IDGS_CANNOT_FLUSH_NL,
/* pl */  IDGS_CANNOT_FLUSH_PL,
/* sv */  IDGS_CANNOT_FLUSH_SV},

/* sound.c */
/* en */ {IDGS_FRAGMENT_PROBLEMS,
/* de */  IDGS_FRAGMENT_PROBLEMS_DE,
/* fr */  IDGS_FRAGMENT_PROBLEMS_FR,
/* it */  IDGS_FRAGMENT_PROBLEMS_IT,
/* nl */  IDGS_FRAGMENT_PROBLEMS_NL,
/* pl */  IDGS_FRAGMENT_PROBLEMS_PL,
/* sv */  IDGS_FRAGMENT_PROBLEMS_SV},

/* sound.c */
/* en */ {IDGS_RUNNING_TOO_SLOW,
/* de */  IDGS_RUNNING_TOO_SLOW_DE,
/* fr */  IDGS_RUNNING_TOO_SLOW_FR,
/* it */  IDGS_RUNNING_TOO_SLOW_IT,
/* nl */  IDGS_RUNNING_TOO_SLOW_NL,
/* pl */  IDGS_RUNNING_TOO_SLOW_PL,
/* sv */  IDGS_RUNNING_TOO_SLOW_SV},

/* sound.c */
/* en */ {IDGS_STORE_SOUNDDEVICE_FAILED,
/* de */  IDGS_STORE_SOUNDDEVICE_FAILED_DE,
/* fr */  IDGS_STORE_SOUNDDEVICE_FAILED_FR,
/* it */  IDGS_STORE_SOUNDDEVICE_FAILED_IT,
/* nl */  IDGS_STORE_SOUNDDEVICE_FAILED_NL,
/* pl */  IDGS_STORE_SOUNDDEVICE_FAILED_PL,
/* sv */  IDGS_STORE_SOUNDDEVICE_FAILED_SV},

/* c64/c64export.c */
/* en */ {IDGS_RESOURCE_S_BLOCKED_BY_S,
/* de */  IDGS_RESOURCE_S_BLOCKED_BY_S_DE,
/* fr */  IDGS_RESOURCE_S_BLOCKED_BY_S_FR,
/* it */  IDGS_RESOURCE_S_BLOCKED_BY_S_IT,
/* nl */  IDGS_RESOURCE_S_BLOCKED_BY_S_NL,
/* pl */  IDGS_RESOURCE_S_BLOCKED_BY_S_PL,
/* sv */  IDGS_RESOURCE_S_BLOCKED_BY_S_SV}

};

/* --------------------------------------------------------------------- */

static char *text_table[countof(translate_text_table)][countof(language_table)];

static char *get_string_by_id(int id)
{
  int k;

  for (k = 0; k < countof(string_table); k++)
  {
    if (string_table[k].resource_id==id)
      return string_table[k].text;
  }
  return NULL;
}

/* FIXME: the translate_text_init function currently doesn't
   convert chars to the currently used codepage, this should
   probably be handled with a intl_convert_cp(text, src_cp, dest_cp)
   call in the future.
 */

static void translate_text_init(void)
{
  int i,j;
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

char *translate_text(int en_resource)
{
  int i;

  if (en_resource<0x10000)
    return intl_translate_text(en_resource);

  for (i = 0; i < countof(translate_text_table); i++)
  {
    if (translate_text_table[i][0]==en_resource)
    {
      if (translate_text_table[i][current_language_index]!=0 &&
          text_table[i][current_language_index]!=NULL &&
          strlen(text_table[i][current_language_index])!=0)
        return text_table[i][current_language_index];
      else
        return text_table[i][0];
    }
  }
  return "";
}

int translate_res(int en_resource)
{
  return intl_translate_res(en_resource);
}

/* --------------------------------------------------------------------- */

static int set_current_language(resource_value_t v, void *param)
{
  int i;

  const char *lang = (const char *)v;

  util_string_set(&current_language, "en");
  current_language_index=0;
  if (strlen(lang)!=2)
    return 0;

  for (i = 0; i < countof(language_table); i++)
  {
    if (!strcasecmp(lang,language_table[i]))
    {
      current_language_index=i;
      util_string_set(&current_language, language_table[i]);
      intl_update_ui();
      return 0;
    }
  }
  return 0;
}

static const resource_t resources[] = {
  { "Language", RES_STRING, (resource_value_t)"en",
    (void *)&current_language, set_current_language, NULL },
  { NULL }
};

int translate_resources_init(void)
{
  intl_init();
  translate_text_init();
  return resources_register(resources);
}

void translate_resources_shutdown(void)
{
  int i,j;

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
    { "-lang", SET_RESOURCE, 1, NULL, NULL, "Language", NULL,
      "<iso language code>", "Specify the iso code of the language" },
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

#endif
