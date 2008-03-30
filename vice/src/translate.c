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
/* sv */ {IDGS_RESOURCE_S_BLOCKED_BY_S_SV, ""},  /* fuzzy */


/* ----------------------------------- COMMAND LINE OPTION STRINGS ------------------------------- */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options,
   c64/psid.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_USE_PAL_SYNC_FACTOR,    "Use PAL sync factor"},
/* de */ {IDCLS_USE_PAL_SYNC_FACTOR_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_USE_PAL_SYNC_FACTOR_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_PAL_SYNC_FACTOR_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_PAL_SYNC_FACTOR_NL, "Gebruik PAL synchronisatie faktor"},
/* pl */ {IDCLS_USE_PAL_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_PAL_SYNC_FACTOR_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_USE_NTSC_SYNC_FACTOR,    "Use NTSC sync factor"},
/* de */ {IDCLS_USE_NTSC_SYNC_FACTOR_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_USE_NTSC_SYNC_FACTOR_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_NTSC_SYNC_FACTOR_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_NTSC_SYNC_FACTOR_NL, "Gebruik NTSC synchronisatie faktor"},
/* pl */ {IDCLS_USE_NTSC_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_NTSC_SYNC_FACTOR_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c128/functionrom.c
   c64/c64-cmdline-options.c, c64/georam.c,
   c64/rancart.c, c64/reu.c, c64/c64cart.c,
   c64/ide64.c, cbm2/cbm2-cmdline-options.c,
   drive/iec-cmdline-options.c, drive/iec128dcr-cmdline-options.c,
   drive/ieee-cmdline-options.c, drive/tcbm-cmdline-options.c,
   fsdevice/fsdevice-cmdline-options.c, pet/pet-cmdline-options.c,
   plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_P_NAME,    "<name>"},
/* de */ {IDCLS_P_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_NAME_NL, "<naam>"},
/* pl */ {IDCLS_P_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INT_KERNEL_NAME,    "Specify name of international Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_INT_KERNEL_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_INT_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_INT_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_INT_KERNEL_NAME_NL, "Geef de naam van het internationaal Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_INT_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INT_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_DE_KERNEL_NAME,    "Specify name of German Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_DE_KERNEL_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_DE_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_DE_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_DE_KERNEL_NAME_NL, "Geef de naam van het Duits Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_DE_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_DE_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FI_KERNEL_NAME,    "Specify name of Finnish Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_FI_KERNEL_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_FI_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_FI_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FI_KERNEL_NAME_NL, "Geef de naam van het Fins Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FI_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FI_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FR_KERNEL_NAME,    "Specify name of French Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_FR_KERNEL_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_FR_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_FR_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FR_KERNEL_NAME_NL, "Geef de naam van het Frans Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FR_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FR_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_IT_KERNEL_NAME,    "Specify name of Italian Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_IT_KERNEL_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_IT_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_IT_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_IT_KERNEL_NAME_NL, "Geef de naam van het Italiaans Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_IT_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_IT_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_NO_KERNEL_NAME,    "Specify name of Norwegain Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_NO_KERNEL_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_NO_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_NO_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_NO_KERNEL_NAME_NL, "Geef de naam van het Noors Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_NO_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_NO_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SV_KERNEL_NAME,    "Specify name of Swedish Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_SV_KERNEL_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SV_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SV_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SV_KERNEL_NAME_NL, "Geef de naam van het Zweeds Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_SV_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SV_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW,    "Specify name of BASIC ROM image (lower part)"},
/* de */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_NL, "Geef de naam van het BASIC ROM bestand (laag gedeelte)"},
/* pl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH,    "Specify name of BASIC ROM image (higher part)"},
/* de */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_NL, "Geef de naam van het BASIC ROM bestand (hoog gedeelte)"},
/* pl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME,    "Specify name of international character generator ROM image"},
/* de */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_NL, "Geef de naam van het internationaal CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME,    "Specify name of German character generator ROM image"},
/* de */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_NL, "Geef de naam van het Duits CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME,    "Specify name of French character generator ROM image"},
/* de */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_NL, "Geef de naam van het Frans CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME,    "Specify name of Swedish character generator ROM image"},
/* de */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_NL, "Geef de naam van het Zweeds CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME,    "Specify name of C64 mode Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_NL, "Geef de naam van het C64 modus Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME,    "Specify name of C64 mode BASIC ROM image"},
/* de */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_NL, "Geef de naam van het C64 modus BASIC ROM bestand"},
/* pl */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_EMULATOR_ID,    "Enable emulator identification"},
/* de */ {IDCLS_ENABLE_EMULATOR_ID_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_EMULATOR_ID_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_EMULATOR_ID_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_EMULATOR_ID_NL, "Aktiveer emulator identificatie"},
/* pl */ {IDCLS_ENABLE_EMULATOR_ID_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EMULATOR_ID_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_EMULATOR_ID,    "Disable emulator identification"},
/* de */ {IDCLS_DISABLE_EMULATOR_ID_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_EMULATOR_ID_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_EMULATOR_ID_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_EMULATOR_ID_NL, "Emulator identificatie afsluiten"},
/* pl */ {IDCLS_DISABLE_EMULATOR_ID_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_EMULATOR_ID_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_ENABLE_IEE488,    "Enable the IEEE488 interface emulation"},
/* de */ {IDCLS_ENABLE_IEE488_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_IEE488_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_IEE488_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEE488_NL, "Aktiveer de IEEE488 interface emulatie"},
/* pl */ {IDCLS_ENABLE_IEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEE488_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_DISABLE_IEE488,    "Disable the IEEE488 interface emulation"},
/* de */ {IDCLS_DISABLE_IEE488_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_IEE488_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_IEE488_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEE488_NL, "De IEEE488 interface emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_IEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEE488_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_P_REVISION,    "<revision>"},
/* de */ {IDCLS_P_REVISION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_REVISION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_REVISION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_REVISION_NL, "<revisie>"},
/* pl */ {IDCLS_P_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_REVISION_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_PATCH_KERNAL_TO_REVISION,    "Patch the Kernal ROM to the specified <revision>"},
/* de */ {IDCLS_PATCH_KERNAL_TO_REVISION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_PATCH_KERNAL_TO_REVISION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_PATCH_KERNAL_TO_REVISION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_PATCH_KERNAL_TO_REVISION_NL, "Verbeter de Kernal ROM naar de opgegeven <revisie>"},
/* pl */ {IDCLS_PATCH_KERNAL_TO_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PATCH_KERNAL_TO_REVISION_SV, ""},  /* fuzzy */

#ifdef HAVE_RS232
/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU,    "Enable the $DE** ACIA RS232 interface emulation"},
/* de */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_NL, "Aktiveer de $DE** ACIA RS232 interface emulatie"},
/* pl */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU,    "Disable the $DE** ACIA RS232 interface emulation"},
/* de */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_NL, "De $DE** ACIA RS232 interface emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_SV, ""},  /* fuzzy */
#endif

#ifdef COMMON_KBD
/* c128/c128-cmdline-options.c, c64/c64-cmdline-optionc.c,
   c64/psid.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_P_NUMBER,    "<number>"},
/* de */ {IDCLS_P_NUMBER_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_NUMBER_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_NUMBER_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_NUMBER_NL, "<nummer>"},
/* pl */ {IDCLS_P_NUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_NUMBER_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX,    "Specify index of keymap file (0=symbol, 1=positional)"},
/* de */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_NL, "Geef index van het keymap bestand (0=symbool, 1=positioneel)"},
/* pl */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options,
   plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME,    "Specify name of symbolic keymap file"},
/* de */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_NL, "Geef naam van het symbolisch keymap bestand"},
/* pl */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME,    "Specify name of positional keymap file"},
/* de */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_NL, "Geef naam van het positioneel keymap bestand"},
/* pl */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_SV, ""},  /* fuzzy */
#endif

/* c128/c128mmu.c */
/* en */ {IDCLS_ACTIVATE_40_COL_MODE,    "Activate 40 column mode"},
/* de */ {IDCLS_ACTIVATE_40_COL_MODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ACTIVATE_40_COL_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ACTIVATE_40_COL_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ACTIVATE_40_COL_MODE_NL, "Aktiveer 40 kolommen modus"},
/* pl */ {IDCLS_ACTIVATE_40_COL_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ACTIVATE_40_COL_MODE_SV, ""},  /* fuzzy */

/* c128/c128mmu.c */
/* en */ {IDCLS_ACTIVATE_80_COL_MODE,    "Activate 80 column mode"},
/* de */ {IDCLS_ACTIVATE_80_COL_MODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ACTIVATE_80_COL_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ACTIVATE_80_COL_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ACTIVATE_80_COL_MODE_NL, "Aktiveer 80 kolommen modus"},
/* pl */ {IDCLS_ACTIVATE_80_COL_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ACTIVATE_80_COL_MODE_SV, ""},  /* fuzzy */

/* c128/functionrom.c */
/* en */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME,    "Specify name of internal Function ROM image"},
/* de */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_NL, "Geef de naam van het intern Functie ROM bestand"},
/* pl */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_SV, ""},  /* fuzzy */

/* c128/functionrom.c */
/* en */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME,    "Specify name of external Function ROM image"},
/* de */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_NL, "Geef de naam van het extern Functie ROM bestand"},
/* pl */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_SV, ""},  /* fuzzy */

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_INT_FUNC_ROM,    "Enable the internal Function ROM"},
/* de */ {IDCLS_ENABLE_INT_FUNC_ROM_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_INT_FUNC_ROM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_INT_FUNC_ROM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_INT_FUNC_ROM_NL, "Aktiveer de interne Functie ROM"},
/* pl */ {IDCLS_ENABLE_INT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_INT_FUNC_ROM_SV, ""},  /* fuzzy */

/* c128/functionrom.c */
/* en */ {IDCLS_DISABLE_INT_FUNC_ROM,    "Disable the internal Function ROM"},
/* de */ {IDCLS_DISABLE_INT_FUNC_ROM_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_INT_FUNC_ROM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_INT_FUNC_ROM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_INT_FUNC_ROM_NL, "De interne Functie ROM afsluiten"},
/* pl */ {IDCLS_DISABLE_INT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_INT_FUNC_ROM_SV, ""},  /* fuzzy */

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_EXT_FUNC_ROM,    "Enable the external Function ROM"},
/* de */ {IDCLS_ENABLE_EXT_FUNC_ROM_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_EXT_FUNC_ROM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_EXT_FUNC_ROM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_EXT_FUNC_ROM_NL, "Aktiveer de externe Functie ROM"},
/* pl */ {IDCLS_ENABLE_EXT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EXT_FUNC_ROM_SV, ""},  /* fuzzy */

/* c128/functionrom.c */
/* en */ {IDCLS_DISABLE_EXT_FUNC_ROM,    "Disable the external Function ROM"},
/* de */ {IDCLS_DISABLE_EXT_FUNC_ROM_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_EXT_FUNC_ROM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_EXT_FUNC_ROM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_EXT_FUNC_ROM_NL, "De externe Functie ROM afsluiten"},
/* pl */ {IDCLS_DISABLE_EXT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_EXT_FUNC_ROM_SV, ""},  /* fuzzy */

/* c64/c64-cmdline-options.c, c64/psid.c */
/* en */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR,    "Use old NTSC sync factor"},
/* de */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_NL, "Gebruik oude NTSC synchronisatie faktor"},
/* pl */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_SV, ""},  /* fuzzy */

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KERNAL_ROM_NAME,    "Specify name of Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_NL, "Geef de naam van het Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_SV, ""},  /* fuzzy */

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME,    "Specify name of BASIC ROM image"},
/* de */ {IDCLS_SPECIFY_BASIC_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_BASIC_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_NL, "Geef de naam van het BASIC ROM bestand"},
/* pl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BASIC_ROM_NAME_SV, ""},  /* fuzzy */

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME,    "Specify name of character generator ROM image"},
/* de */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_NL, "Geef de naam van het CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_SV, ""},  /* fuzzy */

#ifdef COMMON_KBD
/* c64/c64-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2,    "Specify index of keymap file (0=sym, 1=symDE, 2=pos)"},
/* de */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_NL, "Geef de index van het keymap bestand (0=sym, 1=symDE, 2=pos)"},
/* pl */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_SV, ""},  /* fuzzy */

/* c64/c64-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP,    "Specify name of symbolic German keymap file"},
/* de */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_NL, "Geef de naam van het symbolisch duits keymap bestand"},
/* pl */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_SV, ""},  /* fuzzy */
#endif

/* c64/georam.c */
/* en */ {IDCLS_ENABLE_GEORAM,    "Enable the GEORAM expansion unit"},
/* de */ {IDCLS_ENABLE_GEORAM_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_GEORAM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_GEORAM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_GEORAM_NL, "Aktiveer de GEORAM uitbreidings eenheid"},
/* pl */ {IDCLS_ENABLE_GEORAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_GEORAM_SV, ""},  /* fuzzy */

/* c64/georam.c */
/* en */ {IDCLS_DISABLE_GEORAM,    "Disable the GEORAM expansion unit"},
/* de */ {IDCLS_DISABLE_GEORAM_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_GEORAM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_GEORAM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_GEORAM_NL, "De GEORAM uitbreidings eenheid aflsuiten"},
/* pl */ {IDCLS_DISABLE_GEORAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_GEORAM_SV, ""},  /* fuzzy */

/* c64/georam.c */
/* en */ {IDCLS_SPECIFY_GEORAM_NAME,    "Specify name of GEORAM image"},
/* de */ {IDCLS_SPECIFY_GEORAM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_GEORAM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_GEORAM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_GEORAM_NAME_NL, "Geef de naam van her GEORAM bestand"},
/* pl */ {IDCLS_SPECIFY_GEORAM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_GEORAM_NAME_SV, ""},  /* fuzzy */

/* c64/georam.c, c64/ramcart.c, c64/reu.c */
/* en */ {IDCLS_P_SIZE_IN_KB,    "<size in KB>"},
/* de */ {IDCLS_P_SIZE_IN_KB_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_SIZE_IN_KB_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_SIZE_IN_KB_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_SIZE_IN_KB_NL, "<grootte in KB>"},
/* pl */ {IDCLS_P_SIZE_IN_KB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_SIZE_IN_KB_SV, ""},  /* fuzzy */

/* c64/georam.c */
/* en */ {IDCLS_GEORAM_SIZE,    "Size of the GEORAM expansion unit"},
/* de */ {IDCLS_GEORAM_SIZE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_GEORAM_SIZE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_GEORAM_SIZE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_GEORAM_SIZE_NL, "Grootte van de GEORAM uitbreidings eenheid"},
/* pl */ {IDCLS_GEORAM_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_GEORAM_SIZE_SV, ""},  /* fuzzy */

/* c64/psid.c */
/* en */ {IDCLS_SID_PLAYER_MODE,    "SID player mode"},
/* de */ {IDCLS_SID_PLAYER_MODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SID_PLAYER_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SID_PLAYER_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SID_PLAYER_MODE_NL, "SID player modus"},
/* pl */ {IDCLS_SID_PLAYER_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SID_PLAYER_MODE_SV, ""},  /* fuzzy */

/* c64/psid.c */
/* en */ {IDCLS_OVERWRITE_PSID_SETTINGS,    "Override PSID settings for Video standard and SID model"},
/* de */ {IDCLS_OVERWRITE_PSID_SETTINGS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_OVERWRITE_PSID_SETTINGS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_OVERWRITE_PSID_SETTINGS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_OVERWRITE_PSID_SETTINGS_NL, "Overschrijf PSID instellingen voor Video standaard en SID model"},
/* pl */ {IDCLS_OVERWRITE_PSID_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_OVERWRITE_PSID_SETTINGS_SV, ""},  /* fuzzy */

/* c64/psid.c */
/* en */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER,    "Specify PSID tune <number>"},
/* de */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_NL, "Geef PSID muziek <nummer>"},
/* pl */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_SV, ""},  /* fuzzy */

/* c64/ramcart.c */
/* en */ {IDCLS_ENABLE_RAMCART,    "Enable the RAMCART expansion"},
/* de */ {IDCLS_ENABLE_RAMCART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_RAMCART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_RAMCART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAMCART_NL, "Aktiveer de RAMCART uitbreiding"},
/* pl */ {IDCLS_ENABLE_RAMCART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAMCART_SV, ""},  /* fuzzy */

/* c64/ramcart.c */
/* en */ {IDCLS_DISABLE_RAMCART,    "Disable the RAMCART expansion"},
/* de */ {IDCLS_DISABLE_RAMCART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_RAMCART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_RAMCART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAMCART_NL, "De RAMCART uitbreiding aflsuiten"},
/* pl */ {IDCLS_DISABLE_RAMCART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAMCART_SV, ""},  /* fuzzy */

/* c64/ramcart.c */
/* en */ {IDCLS_SPECIFY_RAMCART_NAME,    "Specify name of RAMCART image"},
/* de */ {IDCLS_SPECIFY_RAMCART_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_RAMCART_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_RAMCART_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RAMCART_NAME_NL, "Geef de naam van het RAMCART bestand"},
/* pl */ {IDCLS_SPECIFY_RAMCART_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RAMCART_NAME_SV, ""},  /* fuzzy */

/* c64/ramcart.c */
/* en */ {IDCLS_RAMCART_SIZE,    "Size of the RAMCART expansion"},
/* de */ {IDCLS_RAMCART_SIZE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_RAMCART_SIZE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_RAMCART_SIZE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_RAMCART_SIZE_NL, "Grootte van de RAMCART uitbreiding"},
/* pl */ {IDCLS_RAMCART_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RAMCART_SIZE_SV, ""},  /* fuzzy */

/* c64/reu.c */
/* en */ {IDCLS_ENABLE_REU,    "Enable the RAM expansion unit"},
/* de */ {IDCLS_ENABLE_REU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_REU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_REU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_REU_NL, "Aktiveer de REU uitbreiding"},
/* pl */ {IDCLS_ENABLE_REU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_REU_SV, ""},  /* fuzzy */

/* c64/reu.c */
/* en */ {IDCLS_DISABLE_REU,    "Disable the RAM expansion unit"},
/* de */ {IDCLS_DISABLE_REU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_REU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_REU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_REU_NL, "De REU uitbreiding afsluiten"},
/* pl */ {IDCLS_DISABLE_REU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_REU_SV, ""},  /* fuzzy */

/* c64/reu.c */
/* en */ {IDCLS_SPECIFY_REU_NAME,    "Specify name of REU image"},
/* de */ {IDCLS_SPECIFY_REU_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_REU_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_REU_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_REU_NAME_NL, "Geef de naam van het REU bestand"},
/* pl */ {IDCLS_SPECIFY_REU_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_REU_NAME_SV, ""},  /* fuzzy */

/* c64/reu.c */
/* en */ {IDCLS_REU_SIZE,    "Size of the RAM expansion unit"},
/* de */ {IDCLS_REU_SIZE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_REU_SIZE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_REU_SIZE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_REU_SIZE_NL, "Grootte van de REU uitbreiding"},
/* pl */ {IDCLS_REU_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_REU_SIZE_SV, ""},  /* fuzzy */

#ifdef HAVE_TFE
/* c64/tfe.c */
/* en */ {IDCLS_ENABLE_TFE,    "Enable the TFE (\"The Final Ethernet\") unit"},
/* de */ {IDCLS_ENABLE_TFE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_TFE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_TFE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_TFE_NL, "Aktiveer de TFE (\"The Final Ethernet\") eenheid"},
/* pl */ {IDCLS_ENABLE_TFE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TFE_SV, ""},  /* fuzzy */

/* c64/tfe.c */
/* en */ {IDCLS_DISABLE_TFE,    "Disable the TFE (\"The Final Ethernet\") unit"},
/* de */ {IDCLS_DISABLE_TFE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_TFE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_TFE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_TFE_NL, "De TFE (\"The Final Ethernet\") eenheid afsluiten"},
/* pl */ {IDCLS_DISABLE_TFE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_TFE_SV, ""},  /* fuzzy */
#endif

/* c64/c64cart.c */
/* en */ {IDCLS_CART_ATTACH_DETACH_RESET,    "Reset machine if a cartridge is attached or detached"},
/* de */ {IDCLS_CART_ATTACH_DETACH_RESET_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_CART_ATTACH_DETACH_RESET_FR, ""},  /* fuzzy */
/* it */ {IDCLS_CART_ATTACH_DETACH_RESET_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_CART_ATTACH_DETACH_RESET_NL, "Reset machine als een cartridge wordt gekoppelt of ontkoppelt"},
/* pl */ {IDCLS_CART_ATTACH_DETACH_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_CART_ATTACH_DETACH_RESET_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_CART_ATTACH_DETACH_NO_RESET,    "Do not reset machine if a cartridge is attached or detached"},
/* de */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_FR, ""},  /* fuzzy */
/* it */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_NL, "Reset machine niet als een cartridge wordt gekoppelt of ontkoppelt"},
/* pl */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_CRT_CART,    "Attach CRT cartridge image"},
/* de */ {IDCLS_ATTACH_CRT_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_CRT_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_CRT_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_CRT_CART_NL, "Koppel CRT cartridge bestand"},
/* pl */ {IDCLS_ATTACH_CRT_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_CRT_CART_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_GENERIC_8KB_CART,    "Attach generic 8KB cartridge image"},
/* de */ {IDCLS_ATTACH_GENERIC_8KB_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_GENERIC_8KB_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_GENERIC_8KB_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_GENERIC_8KB_CART_NL, "Koppel algemeen 8KB cartridge bestand"},
/* pl */ {IDCLS_ATTACH_GENERIC_8KB_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_GENERIC_8KB_CART_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_GENERIC_16KB_CART,    "Attach generic 16KB cartridge image"},
/* de */ {IDCLS_ATTACH_GENERIC_16KB_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_GENERIC_16KB_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_GENERIC_16KB_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_GENERIC_16KB_CART_NL, "Koppel algemeen 16KB cartridge bestand"},
/* pl */ {IDCLS_ATTACH_GENERIC_16KB_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_GENERIC_16KB_CART_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART,    "Attach raw 32KB Action Replay cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_NL, "Koppel binair 32KB Action Replay cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART,    "Attach raw 64KB Retro Replay cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_NL, "Koppel binair 64KB Retro Replay cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_IDE64_CART,    "Attach raw 64KB IDE64 cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_IDE64_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_RAW_IDE64_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_RAW_IDE64_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_RAW_IDE64_CART_NL, "Koppel binair 64KB IDE64 cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_IDE64_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_IDE64_CART_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART,    "Attach raw 32KB Atomic Power cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_NL, "Koppel binair 32KB Atomic Power cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART,    "Attach raw 8KB Epyx fastload cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_NL, "Koppel binair 8KB Epyx fastload cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_SS4_CART,    "Attach raw 32KB Super Snapshot cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_SS4_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_RAW_SS4_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_RAW_SS4_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_RAW_SS4_CART_NL, "Koppel binair 32KB Super Snapshot cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_SS4_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_SS4_CART_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_SS5_CART,    "Attach raw 64KB Super Snapshot cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_SS5_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_RAW_SS5_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_RAW_SS5_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_RAW_SS5_CART_NL, "Koppel binair 64KB Super Snapshot cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_SS5_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_SS5_CART_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_CBM_IEEE488_CART,    "Attach CBM IEEE488 cartridge image"},
/* de */ {IDCLS_ATTACH_CBM_IEEE488_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_CBM_IEEE488_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_CBM_IEEE488_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_CBM_IEEE488_CART_NL, "Koppel CBM IEEE488 cartridge bestand"},
/* pl */ {IDCLS_ATTACH_CBM_IEEE488_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_CBM_IEEE488_CART_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_WESTERMANN_CART,    "Attach raw 16KB Westermann learning cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_NL, "Koppel binair 16KB Westermann learning cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_SV, ""},  /* fuzzy */

/* c64/c64cart.c */
/* en */ {IDCLS_ENABLE_EXPERT_CART,    "Enable expert cartridge"},
/* de */ {IDCLS_ENABLE_EXPERT_CART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_EXPERT_CART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_EXPERT_CART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_EXPERT_CART_NL, "Aktiveer expert cartridge"},
/* pl */ {IDCLS_ENABLE_EXPERT_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EXPERT_CART_SV, ""},  /* fuzzy */

/* c64/ide64.c */
/* en */ {IDCLS_SPECIFY_IDE64_NAME,    "Specify name of IDE64 image file"},
/* de */ {IDCLS_SPECIFY_IDE64_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_IDE64_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_IDE64_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_IDE64_NAME_NL, "Geef de naam van het IDE64 bestand"},
/* pl */ {IDCLS_SPECIFY_IDE64_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_IDE64_NAME_SV, ""},  /* fuzzy */

/* c64/ide64.c, gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDCLS_P_VALUE,    "<value>"},
/* de */ {IDCLS_P_VALUE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_VALUE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_VALUE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_VALUE_NL, "<waarde>"},
/* pl */ {IDCLS_P_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_VALUE_SV, ""},  /* fuzzy */

/* c64/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64,    "Set number of cylinders for the IDE64 emulation"},
/* de */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_NL, "Zet het aantal cylinders voor de IDE64 emulatie"},
/* pl */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_SV, ""},  /* fuzzy */

/* c64/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_HEADS_IDE64,    "Set number of heads for the IDE64 emulation"},
/* de */ {IDCLS_SET_AMOUNT_HEADS_IDE64_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_AMOUNT_HEADS_IDE64_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_AMOUNT_HEADS_IDE64_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_AMOUNT_HEADS_IDE64_NL, "Zet het aantal koppen voor de IDE64 emulatie"},
/* pl */ {IDCLS_SET_AMOUNT_HEADS_IDE64_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_AMOUNT_HEADS_IDE64_SV, ""},  /* fuzzy */

/* c64/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_SECTORS_IDE64,    "Set number of sectors for the IDE64 emulation"},
/* de */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_NL, "Zet het aantal sectors voor de IDE64 emulatie"},
/* pl */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_SV, ""},  /* fuzzy */

/* c64/ide64.c */
/* en */ {IDCLS_AUTODETECT_IDE64_GEOMETRY,    "Autodetect geometry of formatted images"},
/* de */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_FR, ""},  /* fuzzy */
/* it */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_NL, "Automatisch de grootte van de geformateerde bestanden detecteren"},
/* pl */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_SV, ""},  /* fuzzy */

/* c64/ide64.c */
/* en */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY,    "Do not autoetect geometry of formatted images"},
/* de */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_FR, ""},  /* fuzzy */
/* it */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_NL, "Niet automatisch de grootte van de geformateerde bestanden detecteren"},
/* pl */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_P_MODELNUMBER,    "<modelnumber>"},
/* de */ {IDCLS_P_MODELNUMBER_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_MODELNUMBER_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_MODELNUMBER_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_MODELNUMBER_NL, "<modelnummer>"},
/* pl */ {IDCLS_P_MODELNUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_MODELNUMBER_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CBM2_MODEL,    "Specify CBM-II model to emulate"},
/* de */ {IDCLS_SPECIFY_CBM2_MODEL_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_CBM2_MODEL_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CBM2_MODEL_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CBM2_MODEL_NL, "Geef CBM-II model om te emuleren"},
/* pl */ {IDCLS_SPECIFY_CBM2_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CBM2_MODEL_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_TO_USE_VIC_II,    "Specify to use VIC-II"},
/* de */ {IDCLS_SPECIFY_TO_USE_VIC_II_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_TO_USE_VIC_II_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_TO_USE_VIC_II_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TO_USE_VIC_II_NL, "Gebruik de VIC-II"},
/* pl */ {IDCLS_SPECIFY_TO_USE_VIC_II_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TO_USE_VIC_II_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_TO_USE_CRTC,    "Specify to use CRTC"},
/* de */ {IDCLS_SPECIFY_TO_USE_CRTC_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_TO_USE_CRTC_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_TO_USE_CRTC_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TO_USE_CRTC_NL, "Gebruik de CRTC"},
/* pl */ {IDCLS_SPECIFY_TO_USE_CRTC_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TO_USE_CRTC_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_P_LINENUMBER,    "<linenumber>"},
/* de */ {IDCLS_P_LINENUMBER_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_LINENUMBER_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_LINENUMBER_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_LINENUMBER_NL, "<lijnnummer>"},
/* pl */ {IDCLS_P_LINENUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_LINENUMBER_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE,    "Specify CBM-II model hardware (0=6x0, 1=7x0)"},
/* de */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_NL, "Geef CBM-II hardware model (0=6x0, 1=7x0)"},
/* pl */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_P_RAMSIZE,    "<ramsize>"},
/* de */ {IDCLS_P_RAMSIZE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_RAMSIZE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_RAMSIZE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_RAMSIZE_NL, "<geheugen grootte>"},
/* pl */ {IDCLS_P_RAMSIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_RAMSIZE_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIZE_OF_RAM,    "Specify size of RAM (64/128/256/512/1024 kByte)"},
/* de */ {IDCLS_SPECIFY_SIZE_OF_RAM_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SIZE_OF_RAM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SIZE_OF_RAM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SIZE_OF_RAM_NL, "Geef geheugen grootte (64/128/256/512/1024 kByte)"},
/* pl */ {IDCLS_SPECIFY_SIZE_OF_RAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SIZE_OF_RAM_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_1000_NAME,    "Specify name of cartridge ROM image for $1000"},
/* de */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_NL, "Geef de naam van het cartridge ROM bestand voor $1000"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_2000_NAME,    "Specify name of cartridge ROM image for $2000-$3fff"},
/* de */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_NL, "Geef de naam van het cartridge ROM bestand voor $2000-$3fff"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_4000_NAME,    "Specify name of cartridge ROM image for $4000-$5fff"},
/* de */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_NL, "Geef de naam van het cartridge ROM bestand voor $4000-$5fff"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_6000_NAME,    "Specify name of cartridge ROM image for $6000-$7fff"},
/* de */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_NL, "Geef de naam van het cartridge ROM bestand voor $6000-$7fff"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800,    "Enable RAM mapping in $0800-$0FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_NL, "Aktiveer RAM op adres $0800-$0FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800,    "Disable RAM mapping in $0800-$0FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_NL, "RAM op adres $0800-$0FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000,    "Enable RAM mapping in $1000-$1FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_NL, "Aktiveer RAM op adres $1000-$1FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000,    "Disable RAM mapping in $1000-$1FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_NL, "RAM op adres $1000-$1FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000,    "Enable RAM mapping in $2000-$3FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_NL, "Aktiveer RAM op adres $2000-$3FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000,    "Disable RAM mapping in $2000-$3FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_NL, "RAM op adres $2000-$3FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000,    "Enable RAM mapping in $4000-$5FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_NL, "Aktiveer RAM op adres $4000-$5FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000,    "Disable RAM mapping in $4000-$5FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_NL, "RAM op adres $4000-$5FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000,    "Enable RAM mapping in $6000-$7FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_NL, "Aktiveer RAM op adres $6000-$7FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000,    "Disable RAM mapping in $6000-$7FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_NL, "RAM op adres $6000-$7FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000,    "Enable RAM mapping in $C000-$CFFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_NL, "Aktiveer RAM op adres $C000-$CFFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000,    "Disable RAM mapping in $C000-$CFFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_NL, "RAM op adres $C000-$CFFF afsluiten"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_SV, ""},  /* fuzzy */

#ifdef COMMON_KBD
/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KEYMAP_INDEX,    "Specify index of keymap file"},
/* de */ {IDCLS_SPECIFY_KEYMAP_INDEX_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_KEYMAP_INDEX_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_KEYMAP_INDEX_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_KEYMAP_INDEX_NL, "Geef de index van het keymap bestand"},
/* pl */ {IDCLS_SPECIFY_KEYMAP_INDEX_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_KEYMAP_INDEX_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME,    "Specify name of graphics keyboard symbolic keymap file"},
/* de */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_NL, "Geef de naam van het symbolisch keymap bestand voor het grafische toetsenbord"},
/* pl */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME,    "Specify name of graphics keyboard positional keymap file"},
/* de */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_NL, "Geef de naam van het positioneel keymap bestand voor het grafische toetsenbord"},
/* pl */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME,    "Specify name of UK business keyboard symbolic keymap file"},
/* de */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_NL, "Geef de naam van het symbolisch keymap bestand voor het UK business toetsenbord"},
/* pl */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME,    "Specify name of UK business keyboard positional keymap file"},
/* de */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_NL, "Geef de naam van het positioneel keymap bestand voor het UK business toetsenbord"},
/* pl */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME,    "Specify name of German business keyboard symbolic keymap file"},
/* de */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_NL, "Geef de naam van het symbolisch keymap bestand voor het Duitse business toetsenbord"},
/* pl */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_SV, ""},  /* fuzzy */

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME,    "Specify name of German business keyboard positional keymap file"},
/* de */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_NL, "Geef de naam van het positioneel keymap bestand voor het Duitse business toetsenbord"},
/* pl */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_SV, ""},  /* fuzzy */
#endif

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_ENABLE_TRUE_DRIVE,    "Enable hardware-level emulation of disk drives"},
/* de */ {IDCLS_ENABLE_TRUE_DRIVE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_TRUE_DRIVE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_TRUE_DRIVE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_TRUE_DRIVE_NL, "Aktiveer hardware-matige emulatie van disk stations"},
/* pl */ {IDCLS_ENABLE_TRUE_DRIVE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TRUE_DRIVE_SV, ""},  /* fuzzy */

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_DISABLE_TRUE_DRIVE,    "Disable hardware-level emulation of disk drives"},
/* de */ {IDCLS_DISABLE_TRUE_DRIVE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_TRUE_DRIVE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_TRUE_DRIVE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_TRUE_DRIVE_NL, "Hardware-matige emulatie van disk stations afsluiten"},
/* pl */ {IDCLS_DISABLE_TRUE_DRIVE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_TRUE_DRIVE_SV, ""},  /* fuzzy */

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_P_TYPE,    "<type>"},
/* de */ {IDCLS_P_TYPE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_TYPE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_TYPE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_TYPE_NL, "<soort>"},
/* pl */ {IDCLS_P_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_TYPE_SV, ""},  /* fuzzy */

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_SET_DRIVE_TYPE,    "Set drive type (0: no drive)"},
/* de */ {IDCLS_SET_DRIVE_TYPE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_DRIVE_TYPE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DRIVE_TYPE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DRIVE_TYPE_NL, "Zet drive soort (0: geen drive)"},
/* pl */ {IDCLS_SET_DRIVE_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DRIVE_TYPE_SV, ""},  /* fuzzy */

/* drive/drive-cmdline-options.c,
   drive/iec-cmdline-options.c */
/* en */ {IDCLS_P_METHOD,    "<method>"},
/* de */ {IDCLS_P_METHOD_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_METHOD_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_METHOD_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_METHOD_NL, "<methode>"},
/* pl */ {IDCLS_P_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_METHOD_SV, ""},  /* fuzzy */

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_SET_DRIVE_EXTENSION_POLICY,    "Set drive 40 track extension policy (0: never, 1: ask, 2: on access)"},
/* de */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_NL, "Zet drive 40 sporen uitbreidings regels (0: nooit, 1: vraag, 2: bij toegang"},
/* pl */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME,    "Specify name of 1541 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_NL, "Geef de naam van het 1541 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME,    "Specify name of 1541-II DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_NL, "Geef de naam van het 1541-II DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME,    "Specify name of 1570 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_NL, "Geef de naam van het 1570 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME,    "Specify name of 1571 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_NL, "Geef de naam van het 1571 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME,    "Specify name of 1581 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_NL, "Geef de naam van het 1581 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_PAR_CABLE,    "Enable SpeedDOS-compatible parallel cable"},
/* de */ {IDCLS_ENABLE_PAR_CABLE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_PAR_CABLE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_PAR_CABLE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_PAR_CABLE_NL, "Aktiveer SpeedDOS-compatible parallel kabel"},
/* pl */ {IDCLS_ENABLE_PAR_CABLE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_PAR_CABLE_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_PAR_CABLE,    "Disable SpeedDOS-compatible parallel cable"},
/* de */ {IDCLS_DISABLE_PAR_CABLE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_PAR_CABLE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_PAR_CABLE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_PAR_CABLE_NL, "SpeedDOS-compatible parallel kabel afsluiten"},
/* pl */ {IDCLS_DISABLE_PAR_CABLE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_PAR_CABLE_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_SET_IDLE_METHOD,    "Set drive idling method (0: no traps, 1: skip cycles, 2: trap idle)"},
/* de */ {IDCLS_SET_IDLE_METHOD_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_IDLE_METHOD_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_IDLE_METHOD_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_IDLE_METHOD_NL, "Zet de drive idle methode (0: geen traps, 1: sla cycli over, 2: trap idle)"},
/* pl */ {IDCLS_SET_IDLE_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_IDLE_METHOD_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_2000,    "Enable 8KB RAM expansion at $2000-$3FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_2000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_2000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_DRIVE_RAM_2000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_2000_NL, "Aktiveer 8KB RAM uitbreiding op adres $2000-$3FFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_2000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_2000_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_2000,    "Disable 8KB RAM expansion at $2000-$3FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_2000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_2000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_DRIVE_RAM_2000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_2000_NL, "8KB RAM uitbreiding op adres $2000-$3FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_2000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_2000_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_4000,    "Enable 8KB RAM expansion at $4000-$5FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_4000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_4000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_DRIVE_RAM_4000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_4000_NL, "Aktiveer 8KB RAM uitbreiding op adres $4000-$5FFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_4000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_4000_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_4000,    "Disable 8KB RAM expansion at $4000-$5FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_4000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_4000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_DRIVE_RAM_4000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_4000_NL, "8KB RAM uitbreiding op adres $4000-$5FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_4000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_4000_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_6000,    "Enable 8KB RAM expansion at $6000-$7FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_6000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_6000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_DRIVE_RAM_6000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_6000_NL, "Aktiveer 8KB RAM uitbreiding op adres $6000-$7FFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_6000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_6000_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_6000,    "Disable 8KB RAM expansion at $6000-$7FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_6000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_6000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_DRIVE_RAM_6000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_6000_NL, "8KB RAM uitbreiding op adres $6000-$7FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_6000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_6000_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_8000,    "Enable 8KB RAM expansion at $8000-$9FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_8000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_8000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_DRIVE_RAM_8000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_8000_NL, "Aktiveer 8KB RAM uitbreiding op adres $8000-$9FFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_8000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_8000_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_8000,    "Disable 8KB RAM expansion at $8000-$9FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_8000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_8000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_DRIVE_RAM_8000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_8000_NL, "8KB RAM uitbreiding op adres $8000-$9FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_8000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_8000_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_A000,    "Enable 8KB RAM expansion at $A000-$BFFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_A000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_A000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_DRIVE_RAM_A000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_A000_NL, "Aktiveer 8KB RAM uitbreiding op adres $A000-$BFFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_A000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_A000_SV, ""},  /* fuzzy */

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_A000,    "Disable 8KB RAM expansion at $A000-$BFFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_A000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_A000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_DRIVE_RAM_A000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_A000_NL, "8KB RAM uitbreiding op adres $A000-$BFFF afsluiten"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_A000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_A000_SV, ""},  /* fuzzy */

/* drive/iec128dcr-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME,    "Specify name of 1571CR DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_NL, "Geef de naam van het 1571CR DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_SV, ""},  /* fuzzy */

/* drive/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME,    "Specify name of 2031 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_NL, "Geef de naam van het 2031 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_SV, ""},  /* fuzzy */

/* drive/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME,    "Specify name of 2040 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_NL, "Geef de naam van het 2040 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_SV, ""},  /* fuzzy */

/* drive/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME,    "Specify name of 3040 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_NL, "Geef de naam van het 3040 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_SV, ""},  /* fuzzy */

/* drive/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME,    "Specify name of 4040 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_NL, "Geef de naam van het 4040 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_SV, ""},  /* fuzzy */

/* drive/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME,    "Specify name of 1001/8050/8250 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_NL, "Geef de naam van het 1001/8050/8250 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_SV, ""},  /* fuzzy */

/* drive/tcbm-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME,    "Specify name of 1551 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_NL, "Geef de naam van het 1551 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_SV, ""},  /* fuzzy */

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8,    "Use <name> as directory for file system device #8"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_NL, "Gebruik <naam> als directory voor file system apparaat #8"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_SV, ""},  /* fuzzy */

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9,    "Use <name> as directory for file system device #9"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_NL, "Gebruik <naam> als directory voor file system apparaat #9"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_SV, ""},  /* fuzzy */

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10,    "Use <name> as directory for file system device #10"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_NL, "Gebruik <naam> als directory voor file system apparaat #10"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_SV, ""},  /* fuzzy */

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11,    "Use <name> as directory for file system device #11"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_NL, "Gebruik <naam> als directory voor file system apparaat #11"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_SV, ""},  /* fuzzy */

#ifdef HAVE_FFMPEG
/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDCLS_SET_AUDIO_STREAM_BITRATE,    "Set bitrate for audio stream in media file"},
/* de */ {IDCLS_SET_AUDIO_STREAM_BITRATE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_AUDIO_STREAM_BITRATE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_AUDIO_STREAM_BITRATE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_AUDIO_STREAM_BITRATE_NL, "Zet de bitrate voor het audio gedeelte van het media bestand"},
/* pl */ {IDCLS_SET_AUDIO_STREAM_BITRATE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_AUDIO_STREAM_BITRATE_SV, ""},  /* fuzzy */

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDCLS_SET_VIDEO_STREAM_BITRATE,    "Set bitrate for video stream in media file"},
/* de */ {IDCLS_SET_VIDEO_STREAM_BITRATE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_VIDEO_STREAM_BITRATE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_VIDEO_STREAM_BITRATE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_VIDEO_STREAM_BITRATE_NL, "Zet de bitrate voor het video gedeelte van het media bestand"},
/* pl */ {IDCLS_SET_VIDEO_STREAM_BITRATE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VIDEO_STREAM_BITRATE_SV, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_PET_MODEL,    "Specify PET model to emulate"},
/* de */ {IDCLS_SPECIFY_PET_MODEL_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_PET_MODEL_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PET_MODEL_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PET_MODEL_NL, "Geef PET model om te emuleren"},
/* pl */ {IDCLS_SPECIFY_PET_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PET_MODEL_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_EDITOR_ROM_NAME,    "Specify name of Editor ROM image"},
/* de */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_NL, "Geef naam van het Editor ROM bestand"},
/* pl */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME,    "Specify 4K extension ROM name at $9***"},
/* de */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_NL, "Geef naam van het 4K uitbreiding ROM bestand op adres $9***"},
/* pl */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME,    "Specify 4K extension ROM name at $A***"},
/* de */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_NL, "Geef naam van het 4K uitbreiding ROM bestand op adres $A***"},
/* pl */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME,    "Specify 4K extension ROM name at $B***"},
/* de */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_NL, "Geef naam van het 4K uitbreiding ROM bestand op adres $B***"},
/* pl */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000,    "Enable PET8296 4K RAM mapping at $9***"},
/* de */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_NL, "Activeer PET8296 4K RAM op adres $9***"},
/* pl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000,    "Disable PET8296 4K RAM mapping at $9***"},
/* de */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_NL, "PET8296 4K RAM op adres $9*** afsluiten"},
/* pl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000,    "Enable PET8296 4K RAM mapping at $A***"},
/* de */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_NL, "Aktiveer PET8296 4K RAM op adres $A***"},
/* pl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000,    "Disable PET8296 4K RAM mapping at $A***"},
/* de */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_FR, ""},  /* fuzzy */
/* it */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_NL, "PET8296 4K RAM op adres $A*** afsluiten"},
/* pl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SUPERPET_IO,    "Enable SuperPET I/O"},
/* de */ {IDCLS_ENABLE_SUPERPET_IO_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_SUPERPET_IO_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SUPERPET_IO_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SUPERPET_IO_NL, "Aktiveer SuperPET I/O"},
/* pl */ {IDCLS_ENABLE_SUPERPET_IO_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SUPERPET_IO_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SUPERPET_IO,    "Disable SuperPET I/O"},
/* de */ {IDCLS_DISABLE_SUPERPET_IO_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_SUPERPET_IO_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_SUPERPET_IO_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SUPERPET_IO_NL, "SuperPET I/O afsluiten"},
/* pl */ {IDCLS_DISABLE_SUPERPET_IO_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SUPERPET_IO_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES,    "Enable ROM 1 Kernal patches"},
/* de */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_NL, "Aktiveer ROM 1 Kernal verbeteringen"},
/* pl */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES,    "Disable ROM 1 Kernal patches"},
/* de */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_NL, "ROM 1 Kernal verbeteringen afsluiten"},
/* pl */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET,    "Switch upper/lower case charset"},
/* de */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_NL, "Schakel grote/kleine letters"},
/* pl */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET,    "Do not switch upper/lower case charset"},
/* de */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_FR, ""},  /* fuzzy */
/* it */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_NL, "Schakel niet grote/kleine letters"},
/* pl */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_EOI_BLANKS_SCREEN,    "EOI blanks screen"},
/* de */ {IDCLS_EOI_BLANKS_SCREEN_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_EOI_BLANKS_SCREEN_FR, ""},  /* fuzzy */
/* it */ {IDCLS_EOI_BLANKS_SCREEN_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_EOI_BLANKS_SCREEN_NL, "EOI maakt het scherm leeg"},
/* pl */ {IDCLS_EOI_BLANKS_SCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_EOI_BLANKS_SCREEN_SV, ""},  /* fuzzy */

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN,    "EOI does not blank screen"},
/* de */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_FR, ""},  /* fuzzy */
/* it */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_NL, "EOI maakt niet het scherm leeg"},
/* pl */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_SV, ""},  /* fuzzy */

/* pet/petpia1.c */
/* en */ {IDCLS_ENABLE_USERPORT_DIAG_PIN,    "Enable userport diagnostic pin"},
/* de */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_NL, "Aktiveer userport diagnostische pin"},
/* pl */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_SV, ""},  /* fuzzy */

/* pet/petpia1.c */
/* en */ {IDCLS_DISABLE_USERPORT_DIAG_PIN,    "Disable userport diagnostic pin"},
/* de */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_NL, "Userport diagnostische pin afsluiten"},
/* pl */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_SV, ""},  /* fuzzy */

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME,    "Specify name of Function low ROM image"},
/* de */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_NL, "Geef de naam van het Function low ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_SV, ""},  /* fuzzy */

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME,    "Specify name of Function high ROM image"},
/* de */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_NL, "Geef de naam van het Function high ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_SV, ""},  /* fuzzy */

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME,    "Specify name of Cartridge 1 low ROM image"},
/* de */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_NL, "Geef de naam van het Cartridge 1 low ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_SV, ""},  /* fuzzy */

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME,    "Specify name of Cartridge 1 high ROM image"},
/* de */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_NL, "Geef de naam van het Cartridge 1 high ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_SV, ""},  /* fuzzy */

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME,    "Specify name of Cartridge 2 low ROM image"},
/* de */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_NL, "Geef de naam van het Cartridge 2 low ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_SV, ""},  /* fuzzy */

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME,    "Specify name of Cartridge 2 high ROM image"},
/* de */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_NL, "Geef de naam van het Cartridge 2 high ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_SV, ""},  /* fuzzy */

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_RAM_INSTALLED,    "Specify size of RAM installed in kb (16/32/64)"},
/* de */ {IDCLS_SPECIFY_RAM_INSTALLED_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_RAM_INSTALLED_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_RAM_INSTALLED_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RAM_INSTALLED_NL, "Geef de hoeveelheid RAM in kb (16/32/64)"},
/* pl */ {IDCLS_SPECIFY_RAM_INSTALLED_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RAM_INSTALLED_SV, ""},  /* fuzzy */

/* plus4/ted-cmdline-options.c */
/* en */ {IDCLS_P_0_2000,    "<0-2000>"},
/* de */ {IDCLS_P_0_2000_DE, "<0-2000>"},
/* fr */ {IDCLS_P_0_2000_FR, "<0-2000>"},
/* it */ {IDCLS_P_0_2000_IT, "<0-2000>"},
/* nl */ {IDCLS_P_0_2000_NL, "<0-2000>"},
/* pl */ {IDCLS_P_0_2000_PL, "<0-2000>"},
/* sv */ {IDCLS_P_0_2000_SV, "<0-2000>"},

/* plus4/ted-cmdline-options.c */
/* en */ {IDCLS_SET_SATURATION,    "Set saturation of internal calculated palette [1000]"},
/* de */ {IDCLS_SET_SATURATION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_SATURATION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_SATURATION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_SATURATION_NL, "Zet de verzadiging van het intern berekend kleuren palette [1000]"},
/* pl */ {IDCLS_SET_SATURATION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SATURATION_SV, ""},  /* fuzzy */

/* plus4/ted-cmdline-options.c */
/* en */ {IDCLS_SET_CONTRAST,    "Set contrast of internal calculated palette [1100]"},
/* de */ {IDCLS_SET_CONTRAST_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_CONTRAST_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_CONTRAST_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_CONTRAST_NL, "Zet het contrast van het intern berekend kleuren palette [1100]"},
/* pl */ {IDCLS_SET_CONTRAST_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_CONTRAST_SV, ""},  /* fuzzy */

/* plus4/ted-cmdline-options.c */
/* en */ {IDCLS_SET_BRIGHTNESS,    "Set brightness of internal calculated palette [1100]"},
/* de */ {IDCLS_SET_BRIGHTNESS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_BRIGHTNESS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_BRIGHTNESS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_BRIGHTNESS_NL, "Zet de helderheid van het intern berekend kleuren palette [1100]"},
/* pl */ {IDCLS_SET_BRIGHTNESS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_BRIGHTNESS_SV, ""},  /* fuzzy */

/* plus4/ted-cmdline-options.c */
/* en */ {IDCLS_SET_GAMMA,    "Set gamma of internal calculated palette [900]"},
/* de */ {IDCLS_SET_GAMMA_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_GAMMA_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_GAMMA_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_GAMMA_NL, "Zet de gamma van het intern berekend kleuren palette [900]"},
/* pl */ {IDCLS_SET_GAMMA_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_GAMMA_SV, ""}   /* fuzzy */

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
/* sv */  IDGS_RESOURCE_S_BLOCKED_BY_S_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options,
   c64/psid.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_USE_PAL_SYNC_FACTOR,
/* de */  IDCLS_USE_PAL_SYNC_FACTOR_DE,
/* fr */  IDCLS_USE_PAL_SYNC_FACTOR_FR,
/* it */  IDCLS_USE_PAL_SYNC_FACTOR_IT,
/* nl */  IDCLS_USE_PAL_SYNC_FACTOR_NL,
/* pl */  IDCLS_USE_PAL_SYNC_FACTOR_PL,
/* sv */  IDCLS_USE_PAL_SYNC_FACTOR_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_USE_NTSC_SYNC_FACTOR,
/* de */  IDCLS_USE_NTSC_SYNC_FACTOR_DE,
/* fr */  IDCLS_USE_NTSC_SYNC_FACTOR_FR,
/* it */  IDCLS_USE_NTSC_SYNC_FACTOR_IT,
/* nl */  IDCLS_USE_NTSC_SYNC_FACTOR_NL,
/* pl */  IDCLS_USE_NTSC_SYNC_FACTOR_PL,
/* sv */  IDCLS_USE_NTSC_SYNC_FACTOR_SV},

/* c128/c128-cmdline-options.c, c128/functionrom.c
   c64/c64-cmdline-options.c, c64/georam.c,
   c64/rancart.c, c64/reu.c, c64/c64cart.c,
   c64/ide64.c, cbm2/cbm2-cmdline-options.c,
   drive/iec-cmdline-options.c, drive/iec128dcr-cmdline-options.c,
   drive/ieee-cmdline-options.c, drive/tcbm-cmdline-options.c,
   fsdevice/fsdevice-cmdline-options.c, pet/pet-cmdline-options.c,
   plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_P_NAME,
/* de */  IDCLS_P_NAME_DE,
/* fr */  IDCLS_P_NAME_FR,
/* it */  IDCLS_P_NAME_IT,
/* nl */  IDCLS_P_NAME_NL,
/* pl */  IDCLS_P_NAME_PL,
/* sv */  IDCLS_P_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INT_KERNEL_NAME,
/* de */  IDCLS_SPECIFY_INT_KERNEL_NAME_DE,
/* fr */  IDCLS_SPECIFY_INT_KERNEL_NAME_FR,
/* it */  IDCLS_SPECIFY_INT_KERNEL_NAME_IT,
/* nl */  IDCLS_SPECIFY_INT_KERNEL_NAME_NL,
/* pl */  IDCLS_SPECIFY_INT_KERNEL_NAME_PL,
/* sv */  IDCLS_SPECIFY_INT_KERNEL_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_DE_KERNEL_NAME,
/* de */  IDCLS_SPECIFY_DE_KERNEL_NAME_DE,
/* fr */  IDCLS_SPECIFY_DE_KERNEL_NAME_FR,
/* it */  IDCLS_SPECIFY_DE_KERNEL_NAME_IT,
/* nl */  IDCLS_SPECIFY_DE_KERNEL_NAME_NL,
/* pl */  IDCLS_SPECIFY_DE_KERNEL_NAME_PL,
/* sv */  IDCLS_SPECIFY_DE_KERNEL_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FI_KERNEL_NAME,
/* de */  IDCLS_SPECIFY_FI_KERNEL_NAME_DE,
/* fr */  IDCLS_SPECIFY_FI_KERNEL_NAME_FR,
/* it */  IDCLS_SPECIFY_FI_KERNEL_NAME_IT,
/* nl */  IDCLS_SPECIFY_FI_KERNEL_NAME_NL,
/* pl */  IDCLS_SPECIFY_FI_KERNEL_NAME_PL,
/* sv */  IDCLS_SPECIFY_FI_KERNEL_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FR_KERNEL_NAME,
/* de */  IDCLS_SPECIFY_FR_KERNEL_NAME_DE,
/* fr */  IDCLS_SPECIFY_FR_KERNEL_NAME_FR,
/* it */  IDCLS_SPECIFY_FR_KERNEL_NAME_IT,
/* nl */  IDCLS_SPECIFY_FR_KERNEL_NAME_NL,
/* pl */  IDCLS_SPECIFY_FR_KERNEL_NAME_PL,
/* sv */  IDCLS_SPECIFY_FR_KERNEL_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_IT_KERNEL_NAME,
/* de */  IDCLS_SPECIFY_IT_KERNEL_NAME_DE,
/* fr */  IDCLS_SPECIFY_IT_KERNEL_NAME_FR,
/* it */  IDCLS_SPECIFY_IT_KERNEL_NAME_IT,
/* nl */  IDCLS_SPECIFY_IT_KERNEL_NAME_NL,
/* pl */  IDCLS_SPECIFY_IT_KERNEL_NAME_PL,
/* sv */  IDCLS_SPECIFY_IT_KERNEL_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_NO_KERNEL_NAME,
/* de */  IDCLS_SPECIFY_NO_KERNEL_NAME_DE,
/* fr */  IDCLS_SPECIFY_NO_KERNEL_NAME_FR,
/* it */  IDCLS_SPECIFY_NO_KERNEL_NAME_IT,
/* nl */  IDCLS_SPECIFY_NO_KERNEL_NAME_NL,
/* pl */  IDCLS_SPECIFY_NO_KERNEL_NAME_PL,
/* sv */  IDCLS_SPECIFY_NO_KERNEL_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SV_KERNEL_NAME,
/* de */  IDCLS_SPECIFY_SV_KERNEL_NAME_DE,
/* fr */  IDCLS_SPECIFY_SV_KERNEL_NAME_FR,
/* it */  IDCLS_SPECIFY_SV_KERNEL_NAME_IT,
/* nl */  IDCLS_SPECIFY_SV_KERNEL_NAME_NL,
/* pl */  IDCLS_SPECIFY_SV_KERNEL_NAME_PL,
/* sv */  IDCLS_SPECIFY_SV_KERNEL_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW,
/* de */  IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_DE,
/* fr */  IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_FR,
/* it */  IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_IT,
/* nl */  IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_NL,
/* pl */  IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_PL,
/* sv */  IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH,
/* de */  IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_DE,
/* fr */  IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_FR,
/* it */  IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_IT,
/* nl */  IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_NL,
/* pl */  IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_PL,
/* sv */  IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME,
/* de */  IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME,
/* de */  IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME,
/* de */  IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME,
/* de */  IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME,
/* de */  IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_DE,
/* fr */  IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_FR,
/* it */  IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_IT,
/* nl */  IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_NL,
/* pl */  IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_PL,
/* sv */  IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME,
/* de */  IDCLS_SPECIFY_C64_MODE_BASIC_NAME_DE,
/* fr */  IDCLS_SPECIFY_C64_MODE_BASIC_NAME_FR,
/* it */  IDCLS_SPECIFY_C64_MODE_BASIC_NAME_IT,
/* nl */  IDCLS_SPECIFY_C64_MODE_BASIC_NAME_NL,
/* pl */  IDCLS_SPECIFY_C64_MODE_BASIC_NAME_PL,
/* sv */  IDCLS_SPECIFY_C64_MODE_BASIC_NAME_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_EMULATOR_ID,
/* de */  IDCLS_ENABLE_EMULATOR_ID_DE,
/* fr */  IDCLS_ENABLE_EMULATOR_ID_FR,
/* it */  IDCLS_ENABLE_EMULATOR_ID_IT,
/* nl */  IDCLS_ENABLE_EMULATOR_ID_NL,
/* pl */  IDCLS_ENABLE_EMULATOR_ID_PL,
/* sv */  IDCLS_ENABLE_EMULATOR_ID_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_EMULATOR_ID,
/* de */  IDCLS_DISABLE_EMULATOR_ID_DE,
/* fr */  IDCLS_DISABLE_EMULATOR_ID_FR,
/* it */  IDCLS_DISABLE_EMULATOR_ID_IT,
/* nl */  IDCLS_DISABLE_EMULATOR_ID_NL,
/* pl */  IDCLS_DISABLE_EMULATOR_ID_PL,
/* sv */  IDCLS_DISABLE_EMULATOR_ID_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_ENABLE_IEE488,
/* de */  IDCLS_ENABLE_IEE488_DE,
/* fr */  IDCLS_ENABLE_IEE488_FR,
/* it */  IDCLS_ENABLE_IEE488_IT,
/* nl */  IDCLS_ENABLE_IEE488_NL,
/* pl */  IDCLS_ENABLE_IEE488_PL,
/* sv */  IDCLS_ENABLE_IEE488_SV},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_DISABLE_IEE488,
/* de */  IDCLS_DISABLE_IEE488_DE,
/* fr */  IDCLS_DISABLE_IEE488_FR,
/* it */  IDCLS_DISABLE_IEE488_IT,
/* nl */  IDCLS_DISABLE_IEE488_NL,
/* pl */  IDCLS_DISABLE_IEE488_PL,
/* sv */  IDCLS_DISABLE_IEE488_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_P_REVISION,
/* de */  IDCLS_P_REVISION_DE,
/* fr */  IDCLS_P_REVISION_FR,
/* it */  IDCLS_P_REVISION_IT,
/* nl */  IDCLS_P_REVISION_NL,
/* pl */  IDCLS_P_REVISION_PL,
/* sv */  IDCLS_P_REVISION_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_PATCH_KERNAL_TO_REVISION,
/* de */  IDCLS_PATCH_KERNAL_TO_REVISION_DE,
/* fr */  IDCLS_PATCH_KERNAL_TO_REVISION_FR,
/* it */  IDCLS_PATCH_KERNAL_TO_REVISION_IT,
/* nl */  IDCLS_PATCH_KERNAL_TO_REVISION_NL,
/* pl */  IDCLS_PATCH_KERNAL_TO_REVISION_PL,
/* sv */  IDCLS_PATCH_KERNAL_TO_REVISION_SV},

#ifdef HAVE_RS232
/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU,
/* de */  IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_DE,
/* fr */  IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_FR,
/* it */  IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_IT,
/* nl */  IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_NL,
/* pl */  IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_PL,
/* sv */  IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU,
/* de */  IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_DE,
/* fr */  IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_FR,
/* it */  IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_IT,
/* nl */  IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_NL,
/* pl */  IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_PL,
/* sv */  IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_SV},
#endif

#ifdef COMMON_KBD
/* c128/c128-cmdline-options.c, c64/c64-cmdline-optionc.c,
   c64/psid.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_P_NUMBER,
/* de */  IDCLS_P_NUMBER_DE,
/* fr */  IDCLS_P_NUMBER_FR,
/* it */  IDCLS_P_NUMBER_IT,
/* nl */  IDCLS_P_NUMBER_NL,
/* pl */  IDCLS_P_NUMBER_PL,
/* sv */  IDCLS_P_NUMBER_SV},

/* c128/c128-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX,
/* de */  IDCLS_SPECIFY_KEYMAP_FILE_INDEX_DE,
/* fr */  IDCLS_SPECIFY_KEYMAP_FILE_INDEX_FR,
/* it */  IDCLS_SPECIFY_KEYMAP_FILE_INDEX_IT,
/* nl */  IDCLS_SPECIFY_KEYMAP_FILE_INDEX_NL,
/* pl */  IDCLS_SPECIFY_KEYMAP_FILE_INDEX_PL,
/* sv */  IDCLS_SPECIFY_KEYMAP_FILE_INDEX_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options,
   plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME,
/* de */  IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_DE,
/* fr */  IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_FR,
/* it */  IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_IT,
/* nl */  IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_NL,
/* pl */  IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_PL,
/* sv */  IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME,
/* de */  IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_DE,
/* fr */  IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_FR,
/* it */  IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_IT,
/* nl */  IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_NL,
/* pl */  IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_PL,
/* sv */  IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_SV},
#endif

/* c128/c128mmu.c */
/* en */ {IDCLS_ACTIVATE_40_COL_MODE,
/* de */  IDCLS_ACTIVATE_40_COL_MODE_DE,
/* fr */  IDCLS_ACTIVATE_40_COL_MODE_FR,
/* it */  IDCLS_ACTIVATE_40_COL_MODE_IT,
/* nl */  IDCLS_ACTIVATE_40_COL_MODE_NL,
/* pl */  IDCLS_ACTIVATE_40_COL_MODE_PL,
/* sv */  IDCLS_ACTIVATE_40_COL_MODE_SV},

/* c128/c128mmu.c */
/* en */ {IDCLS_ACTIVATE_80_COL_MODE,
/* de */  IDCLS_ACTIVATE_80_COL_MODE_DE,
/* fr */  IDCLS_ACTIVATE_80_COL_MODE_FR,
/* it */  IDCLS_ACTIVATE_80_COL_MODE_IT,
/* nl */  IDCLS_ACTIVATE_80_COL_MODE_NL,
/* pl */  IDCLS_ACTIVATE_80_COL_MODE_PL,
/* sv */  IDCLS_ACTIVATE_80_COL_MODE_SV},

/* c128/functionrom.c */
/* en */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME,
/* de */  IDCLS_SPECIFY_INT_FUNC_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_INT_FUNC_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_INT_FUNC_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_INT_FUNC_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_INT_FUNC_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_INT_FUNC_ROM_NAME_SV},

/* c128/functionrom.c */
/* en */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME,
/* de */  IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_SV},

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_INT_FUNC_ROM,
/* de */  IDCLS_ENABLE_INT_FUNC_ROM_DE,
/* fr */  IDCLS_ENABLE_INT_FUNC_ROM_FR,
/* it */  IDCLS_ENABLE_INT_FUNC_ROM_IT,
/* nl */  IDCLS_ENABLE_INT_FUNC_ROM_NL,
/* pl */  IDCLS_ENABLE_INT_FUNC_ROM_PL,
/* sv */  IDCLS_ENABLE_INT_FUNC_ROM_SV},

/* c128/functionrom.c */
/* en */ {IDCLS_DISABLE_INT_FUNC_ROM,
/* de */  IDCLS_DISABLE_INT_FUNC_ROM_DE,
/* fr */  IDCLS_DISABLE_INT_FUNC_ROM_FR,
/* it */  IDCLS_DISABLE_INT_FUNC_ROM_IT,
/* nl */  IDCLS_DISABLE_INT_FUNC_ROM_NL,
/* pl */  IDCLS_DISABLE_INT_FUNC_ROM_PL,
/* sv */  IDCLS_DISABLE_INT_FUNC_ROM_SV},

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_EXT_FUNC_ROM,
/* de */  IDCLS_ENABLE_EXT_FUNC_ROM_DE,
/* fr */  IDCLS_ENABLE_EXT_FUNC_ROM_FR,
/* it */  IDCLS_ENABLE_EXT_FUNC_ROM_IT,
/* nl */  IDCLS_ENABLE_EXT_FUNC_ROM_NL,
/* pl */  IDCLS_ENABLE_EXT_FUNC_ROM_PL,
/* sv */  IDCLS_ENABLE_EXT_FUNC_ROM_SV},

/* c128/functionrom.c */
/* en */ {IDCLS_DISABLE_EXT_FUNC_ROM,
/* de */  IDCLS_DISABLE_EXT_FUNC_ROM_DE,
/* fr */  IDCLS_DISABLE_EXT_FUNC_ROM_FR,
/* it */  IDCLS_DISABLE_EXT_FUNC_ROM_IT,
/* nl */  IDCLS_DISABLE_EXT_FUNC_ROM_NL,
/* pl */  IDCLS_DISABLE_EXT_FUNC_ROM_PL,
/* sv */  IDCLS_DISABLE_EXT_FUNC_ROM_SV},

/* c64/c64-cmdline-options.c */
/* en */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR,
/* de */  IDCLS_USE_OLD_NTSC_SYNC_FACTOR_DE,
/* fr */  IDCLS_USE_OLD_NTSC_SYNC_FACTOR_FR,
/* it */  IDCLS_USE_OLD_NTSC_SYNC_FACTOR_IT,
/* nl */  IDCLS_USE_OLD_NTSC_SYNC_FACTOR_NL,
/* pl */  IDCLS_USE_OLD_NTSC_SYNC_FACTOR_PL,
/* sv */  IDCLS_USE_OLD_NTSC_SYNC_FACTOR_SV},

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KERNAL_ROM_NAME,
/* de */  IDCLS_SPECIFY_KERNAL_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_KERNAL_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_KERNAL_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_KERNAL_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_KERNAL_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_KERNAL_ROM_NAME_SV},

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME,
/* de */  IDCLS_SPECIFY_BASIC_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_BASIC_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_BASIC_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_BASIC_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_BASIC_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_BASIC_ROM_NAME_SV},

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME,
/* de */  IDCLS_SPECIFY_CHARGEN_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_CHARGEN_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_CHARGEN_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_CHARGEN_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_CHARGEN_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_CHARGEN_ROM_NAME_SV},

#ifdef COMMON_KBD
/* c64/c64-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2,
/* de */  IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_DE,
/* fr */  IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_FR,
/* it */  IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_IT,
/* nl */  IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_NL,
/* pl */  IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_PL,
/* sv */  IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_SV},

/* c64/c64-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP,
/* de */  IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_DE,
/* fr */  IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_FR,
/* it */  IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_IT,
/* nl */  IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_NL,
/* pl */  IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_PL,
/* sv */  IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_SV},
#endif

/* c64/georam.c */
/* en */ {IDCLS_ENABLE_GEORAM,
/* de */  IDCLS_ENABLE_GEORAM_DE,
/* fr */  IDCLS_ENABLE_GEORAM_FR,
/* it */  IDCLS_ENABLE_GEORAM_IT,
/* nl */  IDCLS_ENABLE_GEORAM_NL,
/* pl */  IDCLS_ENABLE_GEORAM_PL,
/* sv */  IDCLS_ENABLE_GEORAM_SV},

/* c64/georam.c */
/* en */ {IDCLS_DISABLE_GEORAM,
/* de */  IDCLS_DISABLE_GEORAM_DE,
/* fr */  IDCLS_DISABLE_GEORAM_FR,
/* it */  IDCLS_DISABLE_GEORAM_IT,
/* nl */  IDCLS_DISABLE_GEORAM_NL,
/* pl */  IDCLS_DISABLE_GEORAM_PL,
/* sv */  IDCLS_DISABLE_GEORAM_SV},

/* c64/georam.c */
/* en */ {IDCLS_SPECIFY_GEORAM_NAME,
/* de */  IDCLS_SPECIFY_GEORAM_NAME_DE,
/* fr */  IDCLS_SPECIFY_GEORAM_NAME_FR,
/* it */  IDCLS_SPECIFY_GEORAM_NAME_IT,
/* nl */  IDCLS_SPECIFY_GEORAM_NAME_NL,
/* pl */  IDCLS_SPECIFY_GEORAM_NAME_PL,
/* sv */  IDCLS_SPECIFY_GEORAM_NAME_SV},

/* c64/georam.c */
/* en */ {IDCLS_P_SIZE_IN_KB,
/* de */  IDCLS_P_SIZE_IN_KB_DE,
/* fr */  IDCLS_P_SIZE_IN_KB_FR,
/* it */  IDCLS_P_SIZE_IN_KB_IT,
/* nl */  IDCLS_P_SIZE_IN_KB_NL,
/* pl */  IDCLS_P_SIZE_IN_KB_PL,
/* sv */  IDCLS_P_SIZE_IN_KB_SV},

/* c64/georam.c */
/* en */ {IDCLS_GEORAM_SIZE,
/* de */  IDCLS_GEORAM_SIZE_DE,
/* fr */  IDCLS_GEORAM_SIZE_FR,
/* it */  IDCLS_GEORAM_SIZE_IT,
/* nl */  IDCLS_GEORAM_SIZE_NL,
/* pl */  IDCLS_GEORAM_SIZE_PL,
/* sv */  IDCLS_GEORAM_SIZE_SV},

/* c64/psid.c */
/* en */ {IDCLS_SID_PLAYER_MODE,
/* de */  IDCLS_SID_PLAYER_MODE_DE,
/* fr */  IDCLS_SID_PLAYER_MODE_FR,
/* it */  IDCLS_SID_PLAYER_MODE_IT,
/* nl */  IDCLS_SID_PLAYER_MODE_NL,
/* pl */  IDCLS_SID_PLAYER_MODE_PL,
/* sv */  IDCLS_SID_PLAYER_MODE_SV},

/* c64/psid.c */
/* en */ {IDCLS_OVERWRITE_PSID_SETTINGS,
/* de */  IDCLS_OVERWRITE_PSID_SETTINGS_DE,
/* fr */  IDCLS_OVERWRITE_PSID_SETTINGS_FR,
/* it */  IDCLS_OVERWRITE_PSID_SETTINGS_IT,
/* nl */  IDCLS_OVERWRITE_PSID_SETTINGS_NL,
/* pl */  IDCLS_OVERWRITE_PSID_SETTINGS_PL,
/* sv */  IDCLS_OVERWRITE_PSID_SETTINGS_SV},

/* c64/psid.c */
/* en */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER,
/* de */  IDCLS_SPECIFY_PSID_TUNE_NUMBER_DE,
/* fr */  IDCLS_SPECIFY_PSID_TUNE_NUMBER_FR,
/* it */  IDCLS_SPECIFY_PSID_TUNE_NUMBER_IT,
/* nl */  IDCLS_SPECIFY_PSID_TUNE_NUMBER_NL,
/* pl */  IDCLS_SPECIFY_PSID_TUNE_NUMBER_PL,
/* sv */  IDCLS_SPECIFY_PSID_TUNE_NUMBER_SV},

/* c64/ramcart.c */
/* en */ {IDCLS_ENABLE_RAMCART,
/* de */  IDCLS_ENABLE_RAMCART_DE,
/* fr */  IDCLS_ENABLE_RAMCART_FR,
/* it */  IDCLS_ENABLE_RAMCART_IT,
/* nl */  IDCLS_ENABLE_RAMCART_NL,
/* pl */  IDCLS_ENABLE_RAMCART_PL,
/* sv */  IDCLS_ENABLE_RAMCART_SV},

/* c64/ramcart.c */
/* en */ {IDCLS_DISABLE_RAMCART,
/* de */  IDCLS_DISABLE_RAMCART_DE,
/* fr */  IDCLS_DISABLE_RAMCART_FR,
/* it */  IDCLS_DISABLE_RAMCART_IT,
/* nl */  IDCLS_DISABLE_RAMCART_NL,
/* pl */  IDCLS_DISABLE_RAMCART_PL,
/* sv */  IDCLS_DISABLE_RAMCART_SV},

/* c64/ramcart.c */
/* en */ {IDCLS_SPECIFY_RAMCART_NAME,
/* de */  IDCLS_SPECIFY_RAMCART_NAME_DE,
/* fr */  IDCLS_SPECIFY_RAMCART_NAME_FR,
/* it */  IDCLS_SPECIFY_RAMCART_NAME_IT,
/* nl */  IDCLS_SPECIFY_RAMCART_NAME_NL,
/* pl */  IDCLS_SPECIFY_RAMCART_NAME_PL,
/* sv */  IDCLS_SPECIFY_RAMCART_NAME_SV},

/* c64/ramcart.c */
/* en */ {IDCLS_RAMCART_SIZE,
/* de */  IDCLS_RAMCART_SIZE_DE,
/* fr */  IDCLS_RAMCART_SIZE_FR,
/* it */  IDCLS_RAMCART_SIZE_IT,
/* nl */  IDCLS_RAMCART_SIZE_NL,
/* pl */  IDCLS_RAMCART_SIZE_PL,
/* sv */  IDCLS_RAMCART_SIZE_SV},

/* c64/reu.c */
/* en */ {IDCLS_ENABLE_REU,
/* de */  IDCLS_ENABLE_REU_DE,
/* fr */  IDCLS_ENABLE_REU_FR,
/* it */  IDCLS_ENABLE_REU_IT,
/* nl */  IDCLS_ENABLE_REU_NL,
/* pl */  IDCLS_ENABLE_REU_PL,
/* sv */  IDCLS_ENABLE_REU_SV},

/* c64/reu.c */
/* en */ {IDCLS_DISABLE_REU,
/* de */  IDCLS_DISABLE_REU_DE,
/* fr */  IDCLS_DISABLE_REU_FR,
/* it */  IDCLS_DISABLE_REU_IT,
/* nl */  IDCLS_DISABLE_REU_NL,
/* pl */  IDCLS_DISABLE_REU_PL,
/* sv */  IDCLS_DISABLE_REU_SV},

/* c64/reu.c */
/* en */ {IDCLS_SPECIFY_REU_NAME,
/* de */  IDCLS_SPECIFY_REU_NAME_DE,
/* fr */  IDCLS_SPECIFY_REU_NAME_FR,
/* it */  IDCLS_SPECIFY_REU_NAME_IT,
/* nl */  IDCLS_SPECIFY_REU_NAME_NL,
/* pl */  IDCLS_SPECIFY_REU_NAME_PL,
/* sv */  IDCLS_SPECIFY_REU_NAME_SV},

/* c64/reu.c */
/* en */ {IDCLS_REU_SIZE,
/* de */  IDCLS_REU_SIZE_DE,
/* fr */  IDCLS_REU_SIZE_FR,
/* it */  IDCLS_REU_SIZE_IT,
/* nl */  IDCLS_REU_SIZE_NL,
/* pl */  IDCLS_REU_SIZE_PL,
/* sv */  IDCLS_REU_SIZE_SV},

#ifdef HAVE_TFE
/* c64/tfe.c */
/* en */ {IDCLS_ENABLE_TFE,
/* de */  IDCLS_ENABLE_TFE_DE,
/* fr */  IDCLS_ENABLE_TFE_FR,
/* it */  IDCLS_ENABLE_TFE_IT,
/* nl */  IDCLS_ENABLE_TFE_NL,
/* pl */  IDCLS_ENABLE_TFE_PL,
/* sv */  IDCLS_ENABLE_TFE_SV},

/* c64/tfe.c */
/* en */ {IDCLS_DISABLE_TFE,
/* de */  IDCLS_DISABLE_TFE_DE,
/* fr */  IDCLS_DISABLE_TFE_FR,
/* it */  IDCLS_DISABLE_TFE_IT,
/* nl */  IDCLS_DISABLE_TFE_NL,
/* pl */  IDCLS_DISABLE_TFE_PL,
/* sv */  IDCLS_DISABLE_TFE_SV},
#endif

/* c64/c64cart.c */
/* en */ {IDCLS_CART_ATTACH_DETACH_RESET,
/* de */  IDCLS_CART_ATTACH_DETACH_RESET_DE,
/* fr */  IDCLS_CART_ATTACH_DETACH_RESET_FR,
/* it */  IDCLS_CART_ATTACH_DETACH_RESET_IT,
/* nl */  IDCLS_CART_ATTACH_DETACH_RESET_NL,
/* pl */  IDCLS_CART_ATTACH_DETACH_RESET_PL,
/* sv */  IDCLS_CART_ATTACH_DETACH_RESET_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_CART_ATTACH_DETACH_NO_RESET,
/* de */  IDCLS_CART_ATTACH_DETACH_NO_RESET_DE,
/* fr */  IDCLS_CART_ATTACH_DETACH_NO_RESET_FR,
/* it */  IDCLS_CART_ATTACH_DETACH_NO_RESET_IT,
/* nl */  IDCLS_CART_ATTACH_DETACH_NO_RESET_NL,
/* pl */  IDCLS_CART_ATTACH_DETACH_NO_RESET_PL,
/* sv */  IDCLS_CART_ATTACH_DETACH_NO_RESET_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_CRT_CART,
/* de */  IDCLS_ATTACH_CRT_CART_DE,
/* fr */  IDCLS_ATTACH_CRT_CART_FR,
/* it */  IDCLS_ATTACH_CRT_CART_IT,
/* nl */  IDCLS_ATTACH_CRT_CART_NL,
/* pl */  IDCLS_ATTACH_CRT_CART_PL,
/* sv */  IDCLS_ATTACH_CRT_CART_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_GENERIC_8KB_CART,
/* de */  IDCLS_ATTACH_GENERIC_8KB_CART_DE,
/* fr */  IDCLS_ATTACH_GENERIC_8KB_CART_FR,
/* it */  IDCLS_ATTACH_GENERIC_8KB_CART_IT,
/* nl */  IDCLS_ATTACH_GENERIC_8KB_CART_NL,
/* pl */  IDCLS_ATTACH_GENERIC_8KB_CART_PL,
/* sv */  IDCLS_ATTACH_GENERIC_8KB_CART_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_GENERIC_16KB_CART,
/* de */  IDCLS_ATTACH_GENERIC_16KB_CART_DE,
/* fr */  IDCLS_ATTACH_GENERIC_16KB_CART_FR,
/* it */  IDCLS_ATTACH_GENERIC_16KB_CART_IT,
/* nl */  IDCLS_ATTACH_GENERIC_16KB_CART_NL,
/* pl */  IDCLS_ATTACH_GENERIC_16KB_CART_PL,
/* sv */  IDCLS_ATTACH_GENERIC_16KB_CART_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART,
/* de */  IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_DE,
/* fr */  IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_FR,
/* it */  IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_IT,
/* nl */  IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_NL,
/* pl */  IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_PL,
/* sv */  IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART,
/* de */  IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_DE,
/* fr */  IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_FR,
/* it */  IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_IT,
/* nl */  IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_NL,
/* pl */  IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_PL,
/* sv */  IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_IDE64_CART,
/* de */  IDCLS_ATTACH_RAW_IDE64_CART_DE,
/* fr */  IDCLS_ATTACH_RAW_IDE64_CART_FR,
/* it */  IDCLS_ATTACH_RAW_IDE64_CART_IT,
/* nl */  IDCLS_ATTACH_RAW_IDE64_CART_NL,
/* pl */  IDCLS_ATTACH_RAW_IDE64_CART_PL,
/* sv */  IDCLS_ATTACH_RAW_IDE64_CART_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART,
/* de */  IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_DE,
/* fr */  IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_FR,
/* it */  IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_IT,
/* nl */  IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_NL,
/* pl */  IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_PL,
/* sv */  IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART,
/* de */  IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_DE,
/* fr */  IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_FR,
/* it */  IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_IT,
/* nl */  IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_NL,
/* pl */  IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_PL,
/* sv */  IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_SS4_CART,
/* de */  IDCLS_ATTACH_RAW_SS4_CART_DE,
/* fr */  IDCLS_ATTACH_RAW_SS4_CART_FR,
/* it */  IDCLS_ATTACH_RAW_SS4_CART_IT,
/* nl */  IDCLS_ATTACH_RAW_SS4_CART_NL,
/* pl */  IDCLS_ATTACH_RAW_SS4_CART_PL,
/* sv */  IDCLS_ATTACH_RAW_SS4_CART_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_SS5_CART,
/* de */  IDCLS_ATTACH_RAW_SS5_CART_DE,
/* fr */  IDCLS_ATTACH_RAW_SS5_CART_FR,
/* it */  IDCLS_ATTACH_RAW_SS5_CART_IT,
/* nl */  IDCLS_ATTACH_RAW_SS5_CART_NL,
/* pl */  IDCLS_ATTACH_RAW_SS5_CART_PL,
/* sv */  IDCLS_ATTACH_RAW_SS5_CART_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_CBM_IEEE488_CART,
/* de */  IDCLS_ATTACH_CBM_IEEE488_CART_DE,
/* fr */  IDCLS_ATTACH_CBM_IEEE488_CART_FR,
/* it */  IDCLS_ATTACH_CBM_IEEE488_CART_IT,
/* nl */  IDCLS_ATTACH_CBM_IEEE488_CART_NL,
/* pl */  IDCLS_ATTACH_CBM_IEEE488_CART_PL,
/* sv */  IDCLS_ATTACH_CBM_IEEE488_CART_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_WESTERMANN_CART,
/* de */  IDCLS_ATTACH_RAW_WESTERMANN_CART_DE,
/* fr */  IDCLS_ATTACH_RAW_WESTERMANN_CART_FR,
/* it */  IDCLS_ATTACH_RAW_WESTERMANN_CART_IT,
/* nl */  IDCLS_ATTACH_RAW_WESTERMANN_CART_NL,
/* pl */  IDCLS_ATTACH_RAW_WESTERMANN_CART_PL,
/* sv */  IDCLS_ATTACH_RAW_WESTERMANN_CART_SV},

/* c64/c64cart.c */
/* en */ {IDCLS_ENABLE_EXPERT_CART,
/* de */  IDCLS_ENABLE_EXPERT_CART_DE,
/* fr */  IDCLS_ENABLE_EXPERT_CART_FR,
/* it */  IDCLS_ENABLE_EXPERT_CART_IT,
/* nl */  IDCLS_ENABLE_EXPERT_CART_NL,
/* pl */  IDCLS_ENABLE_EXPERT_CART_PL,
/* sv */  IDCLS_ENABLE_EXPERT_CART_SV},

/* c64/ide64.c */
/* en */ {IDCLS_SPECIFY_IDE64_NAME,
/* de */  IDCLS_SPECIFY_IDE64_NAME_DE,
/* fr */  IDCLS_SPECIFY_IDE64_NAME_FR,
/* it */  IDCLS_SPECIFY_IDE64_NAME_IT,
/* nl */  IDCLS_SPECIFY_IDE64_NAME_NL,
/* pl */  IDCLS_SPECIFY_IDE64_NAME_PL,
/* sv */  IDCLS_SPECIFY_IDE64_NAME_SV},

/* c64/ide64.c, gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDCLS_P_VALUE,
/* de */  IDCLS_P_VALUE_DE,
/* fr */  IDCLS_P_VALUE_FR,
/* it */  IDCLS_P_VALUE_IT,
/* nl */  IDCLS_P_VALUE_NL,
/* pl */  IDCLS_P_VALUE_PL,
/* sv */  IDCLS_P_VALUE_SV},

/* c64/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64,
/* de */  IDCLS_SET_AMOUNT_CYLINDERS_IDE64_DE,
/* fr */  IDCLS_SET_AMOUNT_CYLINDERS_IDE64_FR,
/* it */  IDCLS_SET_AMOUNT_CYLINDERS_IDE64_IT,
/* nl */  IDCLS_SET_AMOUNT_CYLINDERS_IDE64_NL,
/* pl */  IDCLS_SET_AMOUNT_CYLINDERS_IDE64_PL,
/* sv */  IDCLS_SET_AMOUNT_CYLINDERS_IDE64_SV},

/* c64/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_HEADS_IDE64,
/* de */  IDCLS_SET_AMOUNT_HEADS_IDE64_DE,
/* fr */  IDCLS_SET_AMOUNT_HEADS_IDE64_FR,
/* it */  IDCLS_SET_AMOUNT_HEADS_IDE64_IT,
/* nl */  IDCLS_SET_AMOUNT_HEADS_IDE64_NL,
/* pl */  IDCLS_SET_AMOUNT_HEADS_IDE64_PL,
/* sv */  IDCLS_SET_AMOUNT_HEADS_IDE64_SV},

/* c64/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_SECTORS_IDE64,
/* de */  IDCLS_SET_AMOUNT_SECTORS_IDE64_DE,
/* fr */  IDCLS_SET_AMOUNT_SECTORS_IDE64_FR,
/* it */  IDCLS_SET_AMOUNT_SECTORS_IDE64_IT,
/* nl */  IDCLS_SET_AMOUNT_SECTORS_IDE64_NL,
/* pl */  IDCLS_SET_AMOUNT_SECTORS_IDE64_PL,
/* sv */  IDCLS_SET_AMOUNT_SECTORS_IDE64_SV},

/* c64/ide64.c */
/* en */ {IDCLS_AUTODETECT_IDE64_GEOMETRY,
/* de */  IDCLS_AUTODETECT_IDE64_GEOMETRY_DE,
/* fr */  IDCLS_AUTODETECT_IDE64_GEOMETRY_FR,
/* it */  IDCLS_AUTODETECT_IDE64_GEOMETRY_IT,
/* nl */  IDCLS_AUTODETECT_IDE64_GEOMETRY_NL,
/* pl */  IDCLS_AUTODETECT_IDE64_GEOMETRY_PL,
/* sv */  IDCLS_AUTODETECT_IDE64_GEOMETRY_SV},

/* c64/ide64.c */
/* en */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY,
/* de */  IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_DE,
/* fr */  IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_FR,
/* it */  IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_IT,
/* nl */  IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_NL,
/* pl */  IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_PL,
/* sv */  IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_SV},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_P_MODELNUMBER,
/* de */  IDCLS_P_MODELNUMBER_DE,
/* fr */  IDCLS_P_MODELNUMBER_FR,
/* it */  IDCLS_P_MODELNUMBER_IT,
/* nl */  IDCLS_P_MODELNUMBER_NL,
/* pl */  IDCLS_P_MODELNUMBER_PL,
/* sv */  IDCLS_P_MODELNUMBER_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CBM2_MODEL,
/* de */  IDCLS_SPECIFY_CBM2_MODEL_DE,
/* fr */  IDCLS_SPECIFY_CBM2_MODEL_FR,
/* it */  IDCLS_SPECIFY_CBM2_MODEL_IT,
/* nl */  IDCLS_SPECIFY_CBM2_MODEL_NL,
/* pl */  IDCLS_SPECIFY_CBM2_MODEL_PL,
/* sv */  IDCLS_SPECIFY_CBM2_MODEL_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_TO_USE_VIC_II,
/* de */  IDCLS_SPECIFY_TO_USE_VIC_II_DE,
/* fr */  IDCLS_SPECIFY_TO_USE_VIC_II_FR,
/* it */  IDCLS_SPECIFY_TO_USE_VIC_II_IT,
/* nl */  IDCLS_SPECIFY_TO_USE_VIC_II_NL,
/* pl */  IDCLS_SPECIFY_TO_USE_VIC_II_PL,
/* sv */  IDCLS_SPECIFY_TO_USE_VIC_II_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_TO_USE_CRTC,
/* de */  IDCLS_SPECIFY_TO_USE_CRTC_DE,
/* fr */  IDCLS_SPECIFY_TO_USE_CRTC_FR,
/* it */  IDCLS_SPECIFY_TO_USE_CRTC_IT,
/* nl */  IDCLS_SPECIFY_TO_USE_CRTC_NL,
/* pl */  IDCLS_SPECIFY_TO_USE_CRTC_PL,
/* sv */  IDCLS_SPECIFY_TO_USE_CRTC_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_P_LINENUMBER,
/* de */  IDCLS_P_LINENUMBER_DE,
/* fr */  IDCLS_P_LINENUMBER_FR,
/* it */  IDCLS_P_LINENUMBER_IT,
/* nl */  IDCLS_P_LINENUMBER_NL,
/* pl */  IDCLS_P_LINENUMBER_PL,
/* sv */  IDCLS_P_LINENUMBER_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE,
/* de */  IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_DE,
/* fr */  IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_FR,
/* it */  IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_IT,
/* nl */  IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_NL,
/* pl */  IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_PL,
/* sv */  IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_SV},

/* cbm2/cbm2-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_P_RAMSIZE,
/* de */  IDCLS_P_RAMSIZE_DE,
/* fr */  IDCLS_P_RAMSIZE_FR,
/* it */  IDCLS_P_RAMSIZE_IT,
/* nl */  IDCLS_P_RAMSIZE_NL,
/* pl */  IDCLS_P_RAMSIZE_PL,
/* sv */  IDCLS_P_RAMSIZE_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIZE_OF_RAM,
/* de */  IDCLS_SPECIFY_SIZE_OF_RAM_DE,
/* fr */  IDCLS_SPECIFY_SIZE_OF_RAM_FR,
/* it */  IDCLS_SPECIFY_SIZE_OF_RAM_IT,
/* nl */  IDCLS_SPECIFY_SIZE_OF_RAM_NL,
/* pl */  IDCLS_SPECIFY_SIZE_OF_RAM_PL,
/* sv */  IDCLS_SPECIFY_SIZE_OF_RAM_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_1000_NAME,
/* de */  IDCLS_SPECIFY_CART_ROM_1000_NAME_DE,
/* fr */  IDCLS_SPECIFY_CART_ROM_1000_NAME_FR,
/* it */  IDCLS_SPECIFY_CART_ROM_1000_NAME_IT,
/* nl */  IDCLS_SPECIFY_CART_ROM_1000_NAME_NL,
/* pl */  IDCLS_SPECIFY_CART_ROM_1000_NAME_PL,
/* sv */  IDCLS_SPECIFY_CART_ROM_1000_NAME_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_2000_NAME,
/* de */  IDCLS_SPECIFY_CART_ROM_2000_NAME_DE,
/* fr */  IDCLS_SPECIFY_CART_ROM_2000_NAME_FR,
/* it */  IDCLS_SPECIFY_CART_ROM_2000_NAME_IT,
/* nl */  IDCLS_SPECIFY_CART_ROM_2000_NAME_NL,
/* pl */  IDCLS_SPECIFY_CART_ROM_2000_NAME_PL,
/* sv */  IDCLS_SPECIFY_CART_ROM_2000_NAME_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_4000_NAME,
/* de */  IDCLS_SPECIFY_CART_ROM_4000_NAME_DE,
/* fr */  IDCLS_SPECIFY_CART_ROM_4000_NAME_FR,
/* it */  IDCLS_SPECIFY_CART_ROM_4000_NAME_IT,
/* nl */  IDCLS_SPECIFY_CART_ROM_4000_NAME_NL,
/* pl */  IDCLS_SPECIFY_CART_ROM_4000_NAME_PL,
/* sv */  IDCLS_SPECIFY_CART_ROM_4000_NAME_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_6000_NAME,
/* de */  IDCLS_SPECIFY_CART_ROM_6000_NAME_DE,
/* fr */  IDCLS_SPECIFY_CART_ROM_6000_NAME_FR,
/* it */  IDCLS_SPECIFY_CART_ROM_6000_NAME_IT,
/* nl */  IDCLS_SPECIFY_CART_ROM_6000_NAME_NL,
/* pl */  IDCLS_SPECIFY_CART_ROM_6000_NAME_PL,
/* sv */  IDCLS_SPECIFY_CART_ROM_6000_NAME_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800,
/* de */  IDCLS_ENABLE_RAM_MAPPING_IN_0800_DE,
/* fr */  IDCLS_ENABLE_RAM_MAPPING_IN_0800_FR,
/* it */  IDCLS_ENABLE_RAM_MAPPING_IN_0800_IT,
/* nl */  IDCLS_ENABLE_RAM_MAPPING_IN_0800_NL,
/* pl */  IDCLS_ENABLE_RAM_MAPPING_IN_0800_PL,
/* sv */  IDCLS_ENABLE_RAM_MAPPING_IN_0800_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800,
/* de */  IDCLS_DISABLE_RAM_MAPPING_IN_0800_DE,
/* fr */  IDCLS_DISABLE_RAM_MAPPING_IN_0800_FR,
/* it */  IDCLS_DISABLE_RAM_MAPPING_IN_0800_IT,
/* nl */  IDCLS_DISABLE_RAM_MAPPING_IN_0800_NL,
/* pl */  IDCLS_DISABLE_RAM_MAPPING_IN_0800_PL,
/* sv */  IDCLS_DISABLE_RAM_MAPPING_IN_0800_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000,
/* de */  IDCLS_ENABLE_RAM_MAPPING_IN_1000_DE,
/* fr */  IDCLS_ENABLE_RAM_MAPPING_IN_1000_FR,
/* it */  IDCLS_ENABLE_RAM_MAPPING_IN_1000_IT,
/* nl */  IDCLS_ENABLE_RAM_MAPPING_IN_1000_NL,
/* pl */  IDCLS_ENABLE_RAM_MAPPING_IN_1000_PL,
/* sv */  IDCLS_ENABLE_RAM_MAPPING_IN_1000_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000,
/* de */  IDCLS_DISABLE_RAM_MAPPING_IN_1000_DE,
/* fr */  IDCLS_DISABLE_RAM_MAPPING_IN_1000_FR,
/* it */  IDCLS_DISABLE_RAM_MAPPING_IN_1000_IT,
/* nl */  IDCLS_DISABLE_RAM_MAPPING_IN_1000_NL,
/* pl */  IDCLS_DISABLE_RAM_MAPPING_IN_1000_PL,
/* sv */  IDCLS_DISABLE_RAM_MAPPING_IN_1000_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000,
/* de */  IDCLS_ENABLE_RAM_MAPPING_IN_2000_DE,
/* fr */  IDCLS_ENABLE_RAM_MAPPING_IN_2000_FR,
/* it */  IDCLS_ENABLE_RAM_MAPPING_IN_2000_IT,
/* nl */  IDCLS_ENABLE_RAM_MAPPING_IN_2000_NL,
/* pl */  IDCLS_ENABLE_RAM_MAPPING_IN_2000_PL,
/* sv */  IDCLS_ENABLE_RAM_MAPPING_IN_2000_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000,
/* de */  IDCLS_DISABLE_RAM_MAPPING_IN_2000_DE,
/* fr */  IDCLS_DISABLE_RAM_MAPPING_IN_2000_FR,
/* it */  IDCLS_DISABLE_RAM_MAPPING_IN_2000_IT,
/* nl */  IDCLS_DISABLE_RAM_MAPPING_IN_2000_NL,
/* pl */  IDCLS_DISABLE_RAM_MAPPING_IN_2000_PL,
/* sv */  IDCLS_DISABLE_RAM_MAPPING_IN_2000_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000,
/* de */  IDCLS_ENABLE_RAM_MAPPING_IN_4000_DE,
/* fr */  IDCLS_ENABLE_RAM_MAPPING_IN_4000_FR,
/* it */  IDCLS_ENABLE_RAM_MAPPING_IN_4000_IT,
/* nl */  IDCLS_ENABLE_RAM_MAPPING_IN_4000_NL,
/* pl */  IDCLS_ENABLE_RAM_MAPPING_IN_4000_PL,
/* sv */  IDCLS_ENABLE_RAM_MAPPING_IN_4000_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000,
/* de */  IDCLS_DISABLE_RAM_MAPPING_IN_4000_DE,
/* fr */  IDCLS_DISABLE_RAM_MAPPING_IN_4000_FR,
/* it */  IDCLS_DISABLE_RAM_MAPPING_IN_4000_IT,
/* nl */  IDCLS_DISABLE_RAM_MAPPING_IN_4000_NL,
/* pl */  IDCLS_DISABLE_RAM_MAPPING_IN_4000_PL,
/* sv */  IDCLS_DISABLE_RAM_MAPPING_IN_4000_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000,
/* de */  IDCLS_ENABLE_RAM_MAPPING_IN_6000_DE,
/* fr */  IDCLS_ENABLE_RAM_MAPPING_IN_6000_FR,
/* it */  IDCLS_ENABLE_RAM_MAPPING_IN_6000_IT,
/* nl */  IDCLS_ENABLE_RAM_MAPPING_IN_6000_NL,
/* pl */  IDCLS_ENABLE_RAM_MAPPING_IN_6000_PL,
/* sv */  IDCLS_ENABLE_RAM_MAPPING_IN_6000_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000,
/* de */  IDCLS_DISABLE_RAM_MAPPING_IN_6000_DE,
/* fr */  IDCLS_DISABLE_RAM_MAPPING_IN_6000_FR,
/* it */  IDCLS_DISABLE_RAM_MAPPING_IN_6000_IT,
/* nl */  IDCLS_DISABLE_RAM_MAPPING_IN_6000_NL,
/* pl */  IDCLS_DISABLE_RAM_MAPPING_IN_6000_PL,
/* sv */  IDCLS_DISABLE_RAM_MAPPING_IN_6000_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000,
/* de */  IDCLS_ENABLE_RAM_MAPPING_IN_C000_DE,
/* fr */  IDCLS_ENABLE_RAM_MAPPING_IN_C000_FR,
/* it */  IDCLS_ENABLE_RAM_MAPPING_IN_C000_IT,
/* nl */  IDCLS_ENABLE_RAM_MAPPING_IN_C000_NL,
/* pl */  IDCLS_ENABLE_RAM_MAPPING_IN_C000_PL,
/* sv */  IDCLS_ENABLE_RAM_MAPPING_IN_C000_SV},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000,
/* de */  IDCLS_DISABLE_RAM_MAPPING_IN_C000_DE,
/* fr */  IDCLS_DISABLE_RAM_MAPPING_IN_C000_FR,
/* it */  IDCLS_DISABLE_RAM_MAPPING_IN_C000_IT,
/* nl */  IDCLS_DISABLE_RAM_MAPPING_IN_C000_NL,
/* pl */  IDCLS_DISABLE_RAM_MAPPING_IN_C000_PL,
/* sv */  IDCLS_DISABLE_RAM_MAPPING_IN_C000_SV},

#ifdef COMMON_KBD
/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KEYMAP_INDEX,
/* de */  IDCLS_SPECIFY_KEYMAP_INDEX_DE,
/* fr */  IDCLS_SPECIFY_KEYMAP_INDEX_FR,
/* it */  IDCLS_SPECIFY_KEYMAP_INDEX_IT,
/* nl */  IDCLS_SPECIFY_KEYMAP_INDEX_NL,
/* pl */  IDCLS_SPECIFY_KEYMAP_INDEX_PL,
/* sv */  IDCLS_SPECIFY_KEYMAP_INDEX_SV},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME,
/* de */  IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_DE,
/* fr */  IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_FR,
/* it */  IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_IT,
/* nl */  IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_NL,
/* pl */  IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_PL,
/* sv */  IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_SV},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME,
/* de */  IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_DE,
/* fr */  IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_FR,
/* it */  IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_IT,
/* nl */  IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_NL,
/* pl */  IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_PL,
/* sv */  IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_SV},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME,
/* de */  IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_DE,
/* fr */  IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_FR,
/* it */  IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_IT,
/* nl */  IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_NL,
/* pl */  IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_PL,
/* sv */  IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_SV},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME,
/* de */  IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_DE,
/* fr */  IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_FR,
/* it */  IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_IT,
/* nl */  IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_NL,
/* pl */  IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_PL,
/* sv */  IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_SV},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME,
/* de */  IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_DE,
/* fr */  IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_FR,
/* it */  IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_IT,
/* nl */  IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_NL,
/* pl */  IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_PL,
/* sv */  IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_SV},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME,
/* de */  IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_DE,
/* fr */  IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_FR,
/* it */  IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_IT,
/* nl */  IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_NL,
/* pl */  IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_PL,
/* sv */  IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_SV},
#endif

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_ENABLE_TRUE_DRIVE,
/* de */  IDCLS_ENABLE_TRUE_DRIVE_DE,
/* fr */  IDCLS_ENABLE_TRUE_DRIVE_FR,
/* it */  IDCLS_ENABLE_TRUE_DRIVE_IT,
/* nl */  IDCLS_ENABLE_TRUE_DRIVE_NL,
/* pl */  IDCLS_ENABLE_TRUE_DRIVE_PL,
/* sv */  IDCLS_ENABLE_TRUE_DRIVE_SV},

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_DISABLE_TRUE_DRIVE,
/* de */  IDCLS_DISABLE_TRUE_DRIVE_DE,
/* fr */  IDCLS_DISABLE_TRUE_DRIVE_FR,
/* it */  IDCLS_DISABLE_TRUE_DRIVE_IT,
/* nl */  IDCLS_DISABLE_TRUE_DRIVE_NL,
/* pl */  IDCLS_DISABLE_TRUE_DRIVE_PL,
/* sv */  IDCLS_DISABLE_TRUE_DRIVE_SV},

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_P_TYPE,
/* de */  IDCLS_P_TYPE_DE,
/* fr */  IDCLS_P_TYPE_FR,
/* it */  IDCLS_P_TYPE_IT,
/* nl */  IDCLS_P_TYPE_NL,
/* pl */  IDCLS_P_TYPE_PL,
/* sv */  IDCLS_P_TYPE_SV},

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_SET_DRIVE_TYPE,
/* de */  IDCLS_SET_DRIVE_TYPE_DE,
/* fr */  IDCLS_SET_DRIVE_TYPE_FR,
/* it */  IDCLS_SET_DRIVE_TYPE_IT,
/* nl */  IDCLS_SET_DRIVE_TYPE_NL,
/* pl */  IDCLS_SET_DRIVE_TYPE_PL,
/* sv */  IDCLS_SET_DRIVE_TYPE_SV},

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_P_METHOD,
/* de */  IDCLS_P_METHOD_DE,
/* fr */  IDCLS_P_METHOD_FR,
/* it */  IDCLS_P_METHOD_IT,
/* nl */  IDCLS_P_METHOD_NL,
/* pl */  IDCLS_P_METHOD_PL,
/* sv */  IDCLS_P_METHOD_SV},

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_SET_DRIVE_EXTENSION_POLICY,
/* de */  IDCLS_SET_DRIVE_EXTENSION_POLICY_DE,
/* fr */  IDCLS_SET_DRIVE_EXTENSION_POLICY_FR,
/* it */  IDCLS_SET_DRIVE_EXTENSION_POLICY_IT,
/* nl */  IDCLS_SET_DRIVE_EXTENSION_POLICY_NL,
/* pl */  IDCLS_SET_DRIVE_EXTENSION_POLICY_PL,
/* sv */  IDCLS_SET_DRIVE_EXTENSION_POLICY_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1541_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1541_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1541_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1541_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1541_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1541_DOS_ROM_NAME_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1570_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1570_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1570_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1570_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1570_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1570_DOS_ROM_NAME_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1571_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1571_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1571_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1571_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1571_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1571_DOS_ROM_NAME_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1581_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1581_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1581_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1581_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1581_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1581_DOS_ROM_NAME_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_PAR_CABLE,
/* de */  IDCLS_ENABLE_PAR_CABLE_DE,
/* fr */  IDCLS_ENABLE_PAR_CABLE_FR,
/* it */  IDCLS_ENABLE_PAR_CABLE_IT,
/* nl */  IDCLS_ENABLE_PAR_CABLE_NL,
/* pl */  IDCLS_ENABLE_PAR_CABLE_PL,
/* sv */  IDCLS_ENABLE_PAR_CABLE_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_PAR_CABLE,
/* de */  IDCLS_DISABLE_PAR_CABLE_DE,
/* fr */  IDCLS_DISABLE_PAR_CABLE_FR,
/* it */  IDCLS_DISABLE_PAR_CABLE_IT,
/* nl */  IDCLS_DISABLE_PAR_CABLE_NL,
/* pl */  IDCLS_DISABLE_PAR_CABLE_PL,
/* sv */  IDCLS_DISABLE_PAR_CABLE_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_SET_IDLE_METHOD,
/* de */  IDCLS_SET_IDLE_METHOD_DE,
/* fr */  IDCLS_SET_IDLE_METHOD_FR,
/* it */  IDCLS_SET_IDLE_METHOD_IT,
/* nl */  IDCLS_SET_IDLE_METHOD_NL,
/* pl */  IDCLS_SET_IDLE_METHOD_PL,
/* sv */  IDCLS_SET_IDLE_METHOD_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_2000,
/* de */  IDCLS_ENABLE_DRIVE_RAM_2000_DE,
/* fr */  IDCLS_ENABLE_DRIVE_RAM_2000_FR,
/* it */  IDCLS_ENABLE_DRIVE_RAM_2000_IT,
/* nl */  IDCLS_ENABLE_DRIVE_RAM_2000_NL,
/* pl */  IDCLS_ENABLE_DRIVE_RAM_2000_PL,
/* sv */  IDCLS_ENABLE_DRIVE_RAM_2000_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_2000,
/* de */  IDCLS_DISABLE_DRIVE_RAM_2000_DE,
/* fr */  IDCLS_DISABLE_DRIVE_RAM_2000_FR,
/* it */  IDCLS_DISABLE_DRIVE_RAM_2000_IT,
/* nl */  IDCLS_DISABLE_DRIVE_RAM_2000_NL,
/* pl */  IDCLS_DISABLE_DRIVE_RAM_2000_PL,
/* sv */  IDCLS_DISABLE_DRIVE_RAM_2000_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_4000,
/* de */  IDCLS_ENABLE_DRIVE_RAM_4000_DE,
/* fr */  IDCLS_ENABLE_DRIVE_RAM_4000_FR,
/* it */  IDCLS_ENABLE_DRIVE_RAM_4000_IT,
/* nl */  IDCLS_ENABLE_DRIVE_RAM_4000_NL,
/* pl */  IDCLS_ENABLE_DRIVE_RAM_4000_PL,
/* sv */  IDCLS_ENABLE_DRIVE_RAM_4000_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_4000,
/* de */  IDCLS_DISABLE_DRIVE_RAM_4000_DE,
/* fr */  IDCLS_DISABLE_DRIVE_RAM_4000_FR,
/* it */  IDCLS_DISABLE_DRIVE_RAM_4000_IT,
/* nl */  IDCLS_DISABLE_DRIVE_RAM_4000_NL,
/* pl */  IDCLS_DISABLE_DRIVE_RAM_4000_PL,
/* sv */  IDCLS_DISABLE_DRIVE_RAM_4000_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_6000,
/* de */  IDCLS_ENABLE_DRIVE_RAM_6000_DE,
/* fr */  IDCLS_ENABLE_DRIVE_RAM_6000_FR,
/* it */  IDCLS_ENABLE_DRIVE_RAM_6000_IT,
/* nl */  IDCLS_ENABLE_DRIVE_RAM_6000_NL,
/* pl */  IDCLS_ENABLE_DRIVE_RAM_6000_PL,
/* sv */  IDCLS_ENABLE_DRIVE_RAM_6000_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_6000,
/* de */  IDCLS_DISABLE_DRIVE_RAM_6000_DE,
/* fr */  IDCLS_DISABLE_DRIVE_RAM_6000_FR,
/* it */  IDCLS_DISABLE_DRIVE_RAM_6000_IT,
/* nl */  IDCLS_DISABLE_DRIVE_RAM_6000_NL,
/* pl */  IDCLS_DISABLE_DRIVE_RAM_6000_PL,
/* sv */  IDCLS_DISABLE_DRIVE_RAM_6000_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_8000,
/* de */  IDCLS_ENABLE_DRIVE_RAM_8000_DE,
/* fr */  IDCLS_ENABLE_DRIVE_RAM_8000_FR,
/* it */  IDCLS_ENABLE_DRIVE_RAM_8000_IT,
/* nl */  IDCLS_ENABLE_DRIVE_RAM_8000_NL,
/* pl */  IDCLS_ENABLE_DRIVE_RAM_8000_PL,
/* sv */  IDCLS_ENABLE_DRIVE_RAM_8000_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_8000,
/* de */  IDCLS_DISABLE_DRIVE_RAM_8000_DE,
/* fr */  IDCLS_DISABLE_DRIVE_RAM_8000_FR,
/* it */  IDCLS_DISABLE_DRIVE_RAM_8000_IT,
/* nl */  IDCLS_DISABLE_DRIVE_RAM_8000_NL,
/* pl */  IDCLS_DISABLE_DRIVE_RAM_8000_PL,
/* sv */  IDCLS_DISABLE_DRIVE_RAM_8000_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_A000,
/* de */  IDCLS_ENABLE_DRIVE_RAM_A000_DE,
/* fr */  IDCLS_ENABLE_DRIVE_RAM_A000_FR,
/* it */  IDCLS_ENABLE_DRIVE_RAM_A000_IT,
/* nl */  IDCLS_ENABLE_DRIVE_RAM_A000_NL,
/* pl */  IDCLS_ENABLE_DRIVE_RAM_A000_PL,
/* sv */  IDCLS_ENABLE_DRIVE_RAM_A000_SV},

/* drive/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_A000,
/* de */  IDCLS_DISABLE_DRIVE_RAM_A000_DE,
/* fr */  IDCLS_DISABLE_DRIVE_RAM_A000_FR,
/* it */  IDCLS_DISABLE_DRIVE_RAM_A000_IT,
/* nl */  IDCLS_DISABLE_DRIVE_RAM_A000_NL,
/* pl */  IDCLS_DISABLE_DRIVE_RAM_A000_PL,
/* sv */  IDCLS_DISABLE_DRIVE_RAM_A000_SV},

/* drive/iec128dcr-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_SV},

/* drive/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_2031_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_2031_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_2031_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_2031_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_2031_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_2031_DOS_ROM_NAME_SV},

/* drive/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_2040_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_2040_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_2040_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_2040_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_2040_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_2040_DOS_ROM_NAME_SV},

/* drive/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_3040_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_3040_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_3040_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_3040_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_3040_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_3040_DOS_ROM_NAME_SV},

/* drive/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_4040_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_4040_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_4040_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_4040_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_4040_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_4040_DOS_ROM_NAME_SV},

/* drive/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1001_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1001_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1001_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1001_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1001_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1001_DOS_ROM_NAME_SV},

/* drive/tcbm-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1551_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1551_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1551_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1551_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1551_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1551_DOS_ROM_NAME_SV},

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8,
/* de */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_DE,
/* fr */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_FR,
/* it */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_IT,
/* nl */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_NL,
/* pl */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_PL,
/* sv */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_SV},

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9,
/* de */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_DE,
/* fr */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_FR,
/* it */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_IT,
/* nl */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_NL,
/* pl */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_PL,
/* sv */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_SV},

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10,
/* de */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_DE,
/* fr */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_FR,
/* it */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_IT,
/* nl */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_NL,
/* pl */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_PL,
/* sv */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_SV},

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11,
/* de */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_DE,
/* fr */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_FR,
/* it */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_IT,
/* nl */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_NL,
/* pl */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_PL,
/* sv */  IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_SV},

#ifdef HAVE_FFMPEG
/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDCLS_SET_AUDIO_STREAM_BITRATE,
/* de */  IDCLS_SET_AUDIO_STREAM_BITRATE_DE,
/* fr */  IDCLS_SET_AUDIO_STREAM_BITRATE_FR,
/* it */  IDCLS_SET_AUDIO_STREAM_BITRATE_IT,
/* nl */  IDCLS_SET_AUDIO_STREAM_BITRATE_NL,
/* pl */  IDCLS_SET_AUDIO_STREAM_BITRATE_PL,
/* sv */  IDCLS_SET_AUDIO_STREAM_BITRATE_SV},

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDCLS_SET_VIDEO_STREAM_BITRATE,
/* de */  IDCLS_SET_VIDEO_STREAM_BITRATE_DE,
/* fr */  IDCLS_SET_VIDEO_STREAM_BITRATE_FR,
/* it */  IDCLS_SET_VIDEO_STREAM_BITRATE_IT,
/* nl */  IDCLS_SET_VIDEO_STREAM_BITRATE_NL,
/* pl */  IDCLS_SET_VIDEO_STREAM_BITRATE_PL,
/* sv */  IDCLS_SET_VIDEO_STREAM_BITRATE_SV},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_PET_MODEL,
/* de */  IDCLS_SPECIFY_PET_MODEL_DE,
/* fr */  IDCLS_SPECIFY_PET_MODEL_FR,
/* it */  IDCLS_SPECIFY_PET_MODEL_IT,
/* nl */  IDCLS_SPECIFY_PET_MODEL_NL,
/* pl */  IDCLS_SPECIFY_PET_MODEL_PL,
/* sv */  IDCLS_SPECIFY_PET_MODEL_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_EDITOR_ROM_NAME,
/* de */  IDCLS_SPECIFY_EDITOR_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_EDITOR_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_EDITOR_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_EDITOR_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_EDITOR_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_EDITOR_ROM_NAME_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME,
/* de */  IDCLS_SPECIFY_4K_ROM_9XXX_NAME_DE,
/* fr */  IDCLS_SPECIFY_4K_ROM_9XXX_NAME_FR,
/* it */  IDCLS_SPECIFY_4K_ROM_9XXX_NAME_IT,
/* nl */  IDCLS_SPECIFY_4K_ROM_9XXX_NAME_NL,
/* pl */  IDCLS_SPECIFY_4K_ROM_9XXX_NAME_PL,
/* sv */  IDCLS_SPECIFY_4K_ROM_9XXX_NAME_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME,
/* de */  IDCLS_SPECIFY_4K_ROM_AXXX_NAME_DE,
/* fr */  IDCLS_SPECIFY_4K_ROM_AXXX_NAME_FR,
/* it */  IDCLS_SPECIFY_4K_ROM_AXXX_NAME_IT,
/* nl */  IDCLS_SPECIFY_4K_ROM_AXXX_NAME_NL,
/* pl */  IDCLS_SPECIFY_4K_ROM_AXXX_NAME_PL,
/* sv */  IDCLS_SPECIFY_4K_ROM_AXXX_NAME_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME,
/* de */  IDCLS_SPECIFY_4K_ROM_BXXX_NAME_DE,
/* fr */  IDCLS_SPECIFY_4K_ROM_BXXX_NAME_FR,
/* it */  IDCLS_SPECIFY_4K_ROM_BXXX_NAME_IT,
/* nl */  IDCLS_SPECIFY_4K_ROM_BXXX_NAME_NL,
/* pl */  IDCLS_SPECIFY_4K_ROM_BXXX_NAME_PL,
/* sv */  IDCLS_SPECIFY_4K_ROM_BXXX_NAME_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000,
/* de */  IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_DE,
/* fr */  IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_FR,
/* it */  IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_IT,
/* nl */  IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_NL,
/* pl */  IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_PL,
/* sv */  IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000,
/* de */  IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_DE,
/* fr */  IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_FR,
/* it */  IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_IT,
/* nl */  IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_NL,
/* pl */  IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_PL,
/* sv */  IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000,
/* de */  IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_DE,
/* fr */  IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_FR,
/* it */  IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_IT,
/* nl */  IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_NL,
/* pl */  IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_PL,
/* sv */  IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000,
/* de */  IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_DE,
/* fr */  IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_FR,
/* it */  IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_IT,
/* nl */  IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_NL,
/* pl */  IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_PL,
/* sv */  IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SUPERPET_IO,
/* de */  IDCLS_ENABLE_SUPERPET_IO_DE,
/* fr */  IDCLS_ENABLE_SUPERPET_IO_FR,
/* it */  IDCLS_ENABLE_SUPERPET_IO_IT,
/* nl */  IDCLS_ENABLE_SUPERPET_IO_NL,
/* pl */  IDCLS_ENABLE_SUPERPET_IO_PL,
/* sv */  IDCLS_ENABLE_SUPERPET_IO_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SUPERPET_IO,
/* de */  IDCLS_DISABLE_SUPERPET_IO_DE,
/* fr */  IDCLS_DISABLE_SUPERPET_IO_FR,
/* it */  IDCLS_DISABLE_SUPERPET_IO_IT,
/* nl */  IDCLS_DISABLE_SUPERPET_IO_NL,
/* pl */  IDCLS_DISABLE_SUPERPET_IO_PL,
/* sv */  IDCLS_DISABLE_SUPERPET_IO_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES,
/* de */  IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_DE,
/* fr */  IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_FR,
/* it */  IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_IT,
/* nl */  IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_NL,
/* pl */  IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_PL,
/* sv */  IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES,
/* de */  IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_DE,
/* fr */  IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_FR,
/* it */  IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_IT,
/* nl */  IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_NL,
/* pl */  IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_PL,
/* sv */  IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET,
/* de */  IDCLS_SWITCH_UPPER_LOWER_CHARSET_DE,
/* fr */  IDCLS_SWITCH_UPPER_LOWER_CHARSET_FR,
/* it */  IDCLS_SWITCH_UPPER_LOWER_CHARSET_IT,
/* nl */  IDCLS_SWITCH_UPPER_LOWER_CHARSET_NL,
/* pl */  IDCLS_SWITCH_UPPER_LOWER_CHARSET_PL,
/* sv */  IDCLS_SWITCH_UPPER_LOWER_CHARSET_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET,
/* de */  IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_DE,
/* fr */  IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_FR,
/* it */  IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_IT,
/* nl */  IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_NL,
/* pl */  IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_PL,
/* sv */  IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_EOI_BLANKS_SCREEN,
/* de */  IDCLS_EOI_BLANKS_SCREEN_DE,
/* fr */  IDCLS_EOI_BLANKS_SCREEN_FR,
/* it */  IDCLS_EOI_BLANKS_SCREEN_IT,
/* nl */  IDCLS_EOI_BLANKS_SCREEN_NL,
/* pl */  IDCLS_EOI_BLANKS_SCREEN_PL,
/* sv */  IDCLS_EOI_BLANKS_SCREEN_SV},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN,
/* de */  IDCLS_EOI_DOES_NOT_BLANK_SCREEN_DE,
/* fr */  IDCLS_EOI_DOES_NOT_BLANK_SCREEN_FR,
/* it */  IDCLS_EOI_DOES_NOT_BLANK_SCREEN_IT,
/* nl */  IDCLS_EOI_DOES_NOT_BLANK_SCREEN_NL,
/* pl */  IDCLS_EOI_DOES_NOT_BLANK_SCREEN_PL,
/* sv */  IDCLS_EOI_DOES_NOT_BLANK_SCREEN_SV},

/* pet/petpia1.c */
/* en */ {IDCLS_ENABLE_USERPORT_DIAG_PIN,
/* de */  IDCLS_ENABLE_USERPORT_DIAG_PIN_DE,
/* fr */  IDCLS_ENABLE_USERPORT_DIAG_PIN_FR,
/* it */  IDCLS_ENABLE_USERPORT_DIAG_PIN_IT,
/* nl */  IDCLS_ENABLE_USERPORT_DIAG_PIN_NL,
/* pl */  IDCLS_ENABLE_USERPORT_DIAG_PIN_PL,
/* sv */  IDCLS_ENABLE_USERPORT_DIAG_PIN_SV},

/* pet/petpia1.c */
/* en */ {IDCLS_DISABLE_USERPORT_DIAG_PIN,
/* de */  IDCLS_DISABLE_USERPORT_DIAG_PIN_DE,
/* fr */  IDCLS_DISABLE_USERPORT_DIAG_PIN_FR,
/* it */  IDCLS_DISABLE_USERPORT_DIAG_PIN_IT,
/* nl */  IDCLS_DISABLE_USERPORT_DIAG_PIN_NL,
/* pl */  IDCLS_DISABLE_USERPORT_DIAG_PIN_PL,
/* sv */  IDCLS_DISABLE_USERPORT_DIAG_PIN_SV},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME,
/* de */  IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_SV},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME,
/* de */  IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_SV},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME,
/* de */  IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_SV},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME,
/* de */  IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_SV},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME,
/* de */  IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_SV},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME,
/* de */  IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_SV},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_RAM_INSTALLED,
/* de */  IDCLS_SPECIFY_RAM_INSTALLED_DE,
/* fr */  IDCLS_SPECIFY_RAM_INSTALLED_FR,
/* it */  IDCLS_SPECIFY_RAM_INSTALLED_IT,
/* nl */  IDCLS_SPECIFY_RAM_INSTALLED_NL,
/* pl */  IDCLS_SPECIFY_RAM_INSTALLED_PL,
/* sv */  IDCLS_SPECIFY_RAM_INSTALLED_SV},

/* plus4/ted-cmdline-options.c */
/* en */ {IDCLS_P_0_2000,
/* de */  IDCLS_P_0_2000_DE,
/* fr */  IDCLS_P_0_2000_FR,
/* it */  IDCLS_P_0_2000_IT,
/* nl */  IDCLS_P_0_2000_NL,
/* pl */  IDCLS_P_0_2000_PL,
/* sv */  IDCLS_P_0_2000_SV},

/* plus4/ted-cmdline-options.c */
/* en */ {IDCLS_SET_SATURATION,
/* de */  IDCLS_SET_SATURATION_DE,
/* fr */  IDCLS_SET_SATURATION_FR,
/* it */  IDCLS_SET_SATURATION_IT,
/* nl */  IDCLS_SET_SATURATION_NL,
/* pl */  IDCLS_SET_SATURATION_PL,
/* sv */  IDCLS_SET_SATURATION_SV},

/* plus4/ted-cmdline-options.c */
/* en */ {IDCLS_SET_CONTRAST,
/* de */  IDCLS_SET_CONTRAST_DE,
/* fr */  IDCLS_SET_CONTRAST_FR,
/* it */  IDCLS_SET_CONTRAST_IT,
/* nl */  IDCLS_SET_CONTRAST_NL,
/* pl */  IDCLS_SET_CONTRAST_PL,
/* sv */  IDCLS_SET_CONTRAST_SV},

/* plus4/ted-cmdline-options.c */
/* en */ {IDCLS_SET_BRIGHTNESS,
/* de */  IDCLS_SET_BRIGHTNESS_DE,
/* fr */  IDCLS_SET_BRIGHTNESS_FR,
/* it */  IDCLS_SET_BRIGHTNESS_IT,
/* nl */  IDCLS_SET_BRIGHTNESS_NL,
/* pl */  IDCLS_SET_BRIGHTNESS_PL,
/* sv */  IDCLS_SET_BRIGHTNESS_SV},

/* plus4/ted-cmdline-options.c */
/* en */ {IDCLS_SET_GAMMA,
/* de */  IDCLS_SET_GAMMA_DE,
/* fr */  IDCLS_SET_GAMMA_FR,
/* it */  IDCLS_SET_GAMMA_IT,
/* nl */  IDCLS_SET_GAMMA_NL,
/* pl */  IDCLS_SET_GAMMA_PL,
/* sv */  IDCLS_SET_GAMMA_SV}

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

  if (en_resource==0)
    return NULL;

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
