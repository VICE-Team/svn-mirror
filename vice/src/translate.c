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
  28591,	/* ISO 8859-1 */

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
/* de */ {IDGS_CANNOT_CREATE_IMAGE_S_DE, "Kann Datei `%s' nicht erzeugen"},
/* fr */ {IDGS_CANNOT_CREATE_IMAGE_S_FR, "Impossible de créer l'image `%s'"},  /* fuzzy */
/* it */ {IDGS_CANNOT_CREATE_IMAGE_S_IT, "Non è possibile creare l'immagine `%s'"},  /* fuzzy */
/* nl */ {IDGS_CANNOT_CREATE_IMAGE_S_NL, "Kan bestand `%s' niet maken"},
/* pl */ {IDGS_CANNOT_CREATE_IMAGE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_CREATE_IMAGE_S_SV, ""},  /* fuzzy */

/* event.c */
/* en */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S,    "Cannot write image file %s"},
/* de */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_DE, "Kann Datei %s nicht schreiben"},
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
/* de */ {IDGS_WRITE_TO_SOUND_DEVICE_FAILED_DE, "Schreiben auf Gerät Sound ist fehlgeschlagen."},
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
/* de */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_DE, "Aufnahmegerät %s existiert nicht!"},
/* fr */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_FR, "Le périphérique d'enregistrement %s n'existe pas!"},
/* it */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_IT, "Il device di registrazione %s non esiste!"},
/* nl */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_NL, "Opname apparaat %s bestaat niet!"},
/* pl */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RECORD_DEVICE_S_NOT_EXIST_SV, "Inspelningsenhet %s finns inte!"},

/* sound.c */
/* en */ {IDGS_RECORD_DIFFERENT_PLAYBACK,    "Recording device must be different from playback device"},
/* de */ {IDGS_RECORD_DIFFERENT_PLAYBACK_DE, "Aufnahmegerät muß unteschiedlich vom Abspielgerät sein"},
/* fr */ {IDGS_RECORD_DIFFERENT_PLAYBACK_FR, "Le périphérique d'enregistrement doit être différent du "
                                             "périphérique de lecture"},
/* it */ {IDGS_RECORD_DIFFERENT_PLAYBACK_IT, "Il device di registrazione deve essere differente da quello "
                                             "di riproduzione"},
/* nl */ {IDGS_RECORD_DIFFERENT_PLAYBACK_NL, "Opname apparaat moet anders zijn dan afspeel apparaat"},
/* pl */ {IDGS_RECORD_DIFFERENT_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RECORD_DIFFERENT_PLAYBACK_SV, "Inspelningsenhet och återspelningsenhet kan inte vara samma"},

/* sound.c */
/* en */ {IDGS_WARNING_RECORDING_REALTIME,    "Warning! Recording device %s seems to be a realtime device!"},
/* de */ {IDGS_WARNING_RECORDING_REALTIME_DE, "Warnung! Aufnahmegerät %s scheint ein Echtzeitgerät zu sein!"},
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
/* de */ {IDGS_RESOURCE_S_BLOCKED_BY_S_DE, "Resource %s wird von %s blockiert."},
/* fr */ {IDGS_RESOURCE_S_BLOCKED_BY_S_FR, ""},  /* fuzzy */
/* it */ {IDGS_RESOURCE_S_BLOCKED_BY_S_IT, ""},  /* fuzzy */
/* nl */ {IDGS_RESOURCE_S_BLOCKED_BY_S_NL, "Bron %s geblokeerd door %s."},
/* pl */ {IDGS_RESOURCE_S_BLOCKED_BY_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RESOURCE_S_BLOCKED_BY_S_SV, ""},  /* fuzzy */


/* ------------------------ COMMAND LINE OPTION STRINGS -------------------- */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options,
   c64/psid.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_USE_PAL_SYNC_FACTOR,    "Use PAL sync factor"},
/* de */ {IDCLS_USE_PAL_SYNC_FACTOR_DE, "PAL Faktor zum Synchronisieren benutzen"},
/* fr */ {IDCLS_USE_PAL_SYNC_FACTOR_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_PAL_SYNC_FACTOR_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_PAL_SYNC_FACTOR_NL, "Gebruik PAL synchronisatie faktor"},
/* pl */ {IDCLS_USE_PAL_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_PAL_SYNC_FACTOR_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_USE_NTSC_SYNC_FACTOR,    "Use NTSC sync factor"},
/* de */ {IDCLS_USE_NTSC_SYNC_FACTOR_DE, "NTSC Faktor zum Synchronisieren benutzen"},  /* fuzzy */
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
   plus4/plus4-cmdline-options.c, printerdrv/driver-select.c,
   printerdrv/output-select.c, printerdrv/output-text.c,
   rs232drv/rs232drv.c, vic20/vic20-cmdline-options.c,
   vic20/vic20cartridge.c, video/video-cmdline-options.c,
   fliplist.c, initcmdline.c, log.c, sound.c, c64/plus60k.c */
/* en */ {IDCLS_P_NAME,    "<name>"},
/* de */ {IDCLS_P_NAME_DE, "<Name>"},
/* fr */ {IDCLS_P_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_NAME_NL, "<naam>"},
/* pl */ {IDCLS_P_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INT_KERNEL_NAME,    "Specify name of international Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_INT_KERNEL_NAME_DE, "Dateiname des internationalen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_INT_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_INT_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_INT_KERNEL_NAME_NL, "Geef de naam van het internationaal Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_INT_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INT_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_DE_KERNEL_NAME,    "Specify name of German Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_DE_KERNEL_NAME_DE, "Dateiname des deutschen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_DE_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_DE_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_DE_KERNEL_NAME_NL, "Geef de naam van het Duits Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_DE_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_DE_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FI_KERNEL_NAME,    "Specify name of Finnish Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_FI_KERNEL_NAME_DE, "Dateiname des finnischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_FI_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_FI_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FI_KERNEL_NAME_NL, "Geef de naam van het Fins Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FI_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FI_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FR_KERNEL_NAME,    "Specify name of French Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_FR_KERNEL_NAME_DE, "Dateiname des französischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_FR_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_FR_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FR_KERNEL_NAME_NL, "Geef de naam van het Frans Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FR_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FR_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_IT_KERNEL_NAME,    "Specify name of Italian Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_IT_KERNEL_NAME_DE, "Dateiname des italienischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_IT_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_IT_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_IT_KERNEL_NAME_NL, "Geef de naam van het Italiaans Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_IT_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_IT_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_NO_KERNEL_NAME,    "Specify name of Norwegain Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_NO_KERNEL_NAME_DE, "Dateiname des norwegischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_NO_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_NO_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_NO_KERNEL_NAME_NL, "Geef de naam van het Noors Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_NO_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_NO_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SV_KERNEL_NAME,    "Specify name of Swedish Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_SV_KERNEL_NAME_DE, "Dateiname des schwedischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_SV_KERNEL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SV_KERNEL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SV_KERNEL_NAME_NL, "Geef de naam van het Zweeds Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_SV_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SV_KERNEL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW,    "Specify name of BASIC ROM image (lower part)"},
/* de */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_DE, "Dateiname des Basic ROMs (oberer Adressbereich"},
/* fr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_NL, "Geef de naam van het BASIC ROM bestand (laag gedeelte)"},
/* pl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH,    "Specify name of BASIC ROM image (higher part)"},
/* de */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_DE, "Dateiname des Basic ROMs (unterer Adressbereich)"},
/* fr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_NL, "Geef de naam van het BASIC ROM bestand (hoog gedeelte)"},
/* pl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME,    "Specify name of international character generator ROM image"},
/* de */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_DE, "Dateiname des internationalen Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_NL, "Geef de naam van het internationaal CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME,    "Specify name of German character generator ROM image"},
/* de */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_DE, "Dateiname des deutschen Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_NL, "Geef de naam van het Duits CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME,    "Specify name of French character generator ROM image"},
/* de */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_DE, "Dateiname des französischen Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_NL, "Geef de naam van het Frans CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME,    "Specify name of Swedish character generator ROM image"},
/* de */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_DE, "Dateiname des swedischen Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_NL, "Geef de naam van het Zweeds CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME,    "Specify name of C64 mode Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_DE, "Dateiname des Kernal ROMs im C64 Modus"},
/* fr */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_NL, "Geef de naam van het C64 modus Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME,    "Specify name of C64 mode BASIC ROM image"},
/* de */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_DE, "Dateiname des Basic ROMs im C64 Modus"}, 
/* fr */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_NL, "Geef de naam van het C64 modus BASIC ROM bestand"},
/* pl */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_ENABLE_EMULATOR_ID,    "Enable emulator identification"},
/* de */ {IDCLS_ENABLE_EMULATOR_ID_DE, "Emulatoridentifikation aktivieren"},
/* fr */ {IDCLS_ENABLE_EMULATOR_ID_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_EMULATOR_ID_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_EMULATOR_ID_NL, "Aktiveer emulator identificatie"},
/* pl */ {IDCLS_ENABLE_EMULATOR_ID_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EMULATOR_ID_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_DISABLE_EMULATOR_ID,    "Disable emulator identification"},
/* de */ {IDCLS_DISABLE_EMULATOR_ID_DE, "Emulatoridentifikation deaktivieren"},
/* fr */ {IDCLS_DISABLE_EMULATOR_ID_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_EMULATOR_ID_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_EMULATOR_ID_NL, "Emulator identificatie afsluiten"},
/* pl */ {IDCLS_DISABLE_EMULATOR_ID_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_EMULATOR_ID_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_ENABLE_IEE488,    "Enable the IEEE488 interface emulation"},
/* de */ {IDCLS_ENABLE_IEE488_DE, "IEEE488 Schnittstellenemulation aktivieren"},
/* fr */ {IDCLS_ENABLE_IEE488_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_IEE488_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEE488_NL, "Aktiveer de IEEE488 interface emulatie"},
/* pl */ {IDCLS_ENABLE_IEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEE488_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_DISABLE_IEE488,    "Disable the IEEE488 interface emulation"},
/* de */ {IDCLS_DISABLE_IEE488_DE, "IEEE488 Schnittstellenemulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEE488_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_IEE488_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEE488_NL, "De IEEE488 interface emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_IEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEE488_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_P_REVISION,    "<revision>"},
/* de */ {IDCLS_P_REVISION_DE, "<Revision>"},
/* fr */ {IDCLS_P_REVISION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_REVISION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_REVISION_NL, "<revisie>"},
/* pl */ {IDCLS_P_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_REVISION_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_PATCH_KERNAL_TO_REVISION,    "Patch the Kernal ROM to the specified <revision>"},
/* de */ {IDCLS_PATCH_KERNAL_TO_REVISION_DE, "Kernal ROM auf angegenbene Revision ändern"},
/* fr */ {IDCLS_PATCH_KERNAL_TO_REVISION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_PATCH_KERNAL_TO_REVISION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_PATCH_KERNAL_TO_REVISION_NL, "Verbeter de Kernal ROM naar de opgegeven <revisie>"},
/* pl */ {IDCLS_PATCH_KERNAL_TO_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PATCH_KERNAL_TO_REVISION_SV, ""},  /* fuzzy */

#ifdef HAVE_RS232
/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU,    "Enable the $DE** ACIA RS232 interface emulation"},
/* de */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_DE, "$DE** ACIA RS232 Schnittstellenemulation aktivieren"},
/* fr */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_NL, "Aktiveer de $DE** ACIA RS232 interface emulatie"},
/* pl */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU,    "Disable the $DE** ACIA RS232 interface emulation"},
/* de */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_DE, "$DE** ACIA RS232 Schnittstellenemulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_NL, "De $DE** ACIA RS232 interface emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_SV, ""},  /* fuzzy */
#endif

/* c128/c128-cmdline-options.c, c64/c64-cmdline-optionc.c,
   c64/psid.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vdc/vdc-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_P_NUMBER,    "<number>"},
/* de */ {IDCLS_P_NUMBER_DE, "<Nummer>"},
/* fr */ {IDCLS_P_NUMBER_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_NUMBER_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_NUMBER_NL, "<nummer>"},
/* pl */ {IDCLS_P_NUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_NUMBER_SV, ""},  /* fuzzy */

#ifdef COMMON_KBD
/* c128/c128-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX,    "Specify index of keymap file (0=symbol, 1=positional)"},
/* de */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_DE, "Aktive Tastaturbelegung (0=symbolisch) (1=positionell)"},
/* fr */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_NL, "Geef index van het keymap bestand (0=symbool, 1=positioneel)"},
/* pl */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options,
   plus4/plus4-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME,    "Specify name of symbolic keymap file"},
/* de */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_DE, "Dateiname für symbolische Tastaturbelegung"},
/* fr */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_NL, "Geef naam van het symbolisch keymap bestand"},
/* pl */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_SV, ""},  /* fuzzy */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   plus4/plus4-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME,    "Specify name of positional keymap file"},
/* de */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_DE, "Dateiname für positionelle Tastaturbelegung"},
/* fr */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_NL, "Geef naam van het positioneel keymap bestand"},
/* pl */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_SV, ""},  /* fuzzy */
#endif

/* c128/c128mmu.c */
/* en */ {IDCLS_ACTIVATE_40_COL_MODE,    "Activate 40 column mode"},
/* de */ {IDCLS_ACTIVATE_40_COL_MODE_DE, "40 Zeichen Modus"},
/* fr */ {IDCLS_ACTIVATE_40_COL_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ACTIVATE_40_COL_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ACTIVATE_40_COL_MODE_NL, "Aktiveer 40 kolommen modus"},
/* pl */ {IDCLS_ACTIVATE_40_COL_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ACTIVATE_40_COL_MODE_SV, ""},  /* fuzzy */

/* c128/c128mmu.c */
/* en */ {IDCLS_ACTIVATE_80_COL_MODE,    "Activate 80 column mode"},
/* de */ {IDCLS_ACTIVATE_80_COL_MODE_DE, "80 Zeichen Modus"},
/* fr */ {IDCLS_ACTIVATE_80_COL_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ACTIVATE_80_COL_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ACTIVATE_80_COL_MODE_NL, "Aktiveer 80 kolommen modus"},
/* pl */ {IDCLS_ACTIVATE_80_COL_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ACTIVATE_80_COL_MODE_SV, ""},  /* fuzzy */

/* c128/functionrom.c */
/* en */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME,    "Specify name of internal Function ROM image"},
/* de */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_DE, "Dateiname des internen Funktions ROMs"},
/* fr */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_NL, "Geef de naam van het intern Functie ROM bestand"},
/* pl */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_SV, ""},  /* fuzzy */

/* c128/functionrom.c */
/* en */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME,    "Specify name of external Function ROM image"},
/* de */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_DE, "Dateiname des externen Funktions ROMs"},
/* fr */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_NL, "Geef de naam van het extern Functie ROM bestand"},
/* pl */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_SV, ""},  /* fuzzy */

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_INT_FUNC_ROM,    "Enable the internal Function ROM"},
/* de */ {IDCLS_ENABLE_INT_FUNC_ROM_DE, "Internes Funktions ROM aktivieren"},
/* fr */ {IDCLS_ENABLE_INT_FUNC_ROM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_INT_FUNC_ROM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_INT_FUNC_ROM_NL, "Aktiveer de interne Functie ROM"},
/* pl */ {IDCLS_ENABLE_INT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_INT_FUNC_ROM_SV, ""},  /* fuzzy */

/* c128/functionrom.c */
/* en */ {IDCLS_DISABLE_INT_FUNC_ROM,    "Disable the internal Function ROM"},
/* de */ {IDCLS_DISABLE_INT_FUNC_ROM_DE, "Internes Funktions ROM deaktivieren"},
/* fr */ {IDCLS_DISABLE_INT_FUNC_ROM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_INT_FUNC_ROM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_INT_FUNC_ROM_NL, "De interne Functie ROM afsluiten"},
/* pl */ {IDCLS_DISABLE_INT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_INT_FUNC_ROM_SV, ""},  /* fuzzy */

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_EXT_FUNC_ROM,    "Enable the external Function ROM"},
/* de */ {IDCLS_ENABLE_EXT_FUNC_ROM_DE, "Externes Funktions ROM aktivieren"},
/* fr */ {IDCLS_ENABLE_EXT_FUNC_ROM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_EXT_FUNC_ROM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_EXT_FUNC_ROM_NL, "Aktiveer de externe Functie ROM"},
/* pl */ {IDCLS_ENABLE_EXT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EXT_FUNC_ROM_SV, ""},  /* fuzzy */

/* c128/functionrom.c */
/* en */ {IDCLS_DISABLE_EXT_FUNC_ROM,    "Disable the external Function ROM"},
/* de */ {IDCLS_DISABLE_EXT_FUNC_ROM_DE, "Externes Funktions ROM deaktivieren"},
/* fr */ {IDCLS_DISABLE_EXT_FUNC_ROM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_EXT_FUNC_ROM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_EXT_FUNC_ROM_NL, "De externe Functie ROM afsluiten"},
/* pl */ {IDCLS_DISABLE_EXT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_EXT_FUNC_ROM_SV, ""},  /* fuzzy */

/* c64/c64-cmdline-options.c, c64/psid.c */
/* en */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR,    "Use old NTSC sync factor"},
/* de */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_DE, "NTSC (alt) Faktor zum Synchronisieren benutzen"},
/* fr */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_NL, "Gebruik oude NTSC synchronisatie faktor"},
/* pl */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_SV, ""},  /* fuzzy */

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KERNAL_ROM_NAME,    "Specify name of Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_DE, "Dateiname des Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_NL, "Geef de naam van het Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_SV, ""},  /* fuzzy */

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME,    "Specify name of BASIC ROM image"},
/* de */ {IDCLS_SPECIFY_BASIC_ROM_NAME_DE, "Dateiname des Basic ROMs"},
/* fr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_BASIC_ROM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_NL, "Geef de naam van het BASIC ROM bestand"},
/* pl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BASIC_ROM_NAME_SV, ""},  /* fuzzy */

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME,    "Specify name of character generator ROM image"},
/* de */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_DE, "Dateiname des Zeichensatz ROMs"},
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
/* de */ {IDCLS_ENABLE_GEORAM_DE, "GEORAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_GEORAM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_GEORAM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_GEORAM_NL, "Aktiveer de GEORAM uitbreidings eenheid"},
/* pl */ {IDCLS_ENABLE_GEORAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_GEORAM_SV, ""},  /* fuzzy */

/* c64/georam.c */
/* en */ {IDCLS_DISABLE_GEORAM,    "Disable the GEORAM expansion unit"},
/* de */ {IDCLS_DISABLE_GEORAM_DE, "GEORAM Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_GEORAM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_GEORAM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_GEORAM_NL, "De GEORAM uitbreidings eenheid aflsuiten"},
/* pl */ {IDCLS_DISABLE_GEORAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_GEORAM_SV, ""},  /* fuzzy */

/* c64/georam.c */
/* en */ {IDCLS_SPECIFY_GEORAM_NAME,    "Specify name of GEORAM image"},
/* de */ {IDCLS_SPECIFY_GEORAM_NAME_DE, "Dateiname für GEORAM Erweiterung"},
/* fr */ {IDCLS_SPECIFY_GEORAM_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_GEORAM_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_GEORAM_NAME_NL, "Geef de naam van her GEORAM bestand"},
/* pl */ {IDCLS_SPECIFY_GEORAM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_GEORAM_NAME_SV, ""},  /* fuzzy */

/* c64/georam.c, c64/ramcart.c, c64/reu.c */
/* en */ {IDCLS_P_SIZE_IN_KB,    "<size in KB>"},
/* de */ {IDCLS_P_SIZE_IN_KB_DE, "<Grösse in KB"},
/* fr */ {IDCLS_P_SIZE_IN_KB_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_SIZE_IN_KB_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_SIZE_IN_KB_NL, "<grootte in KB>"},
/* pl */ {IDCLS_P_SIZE_IN_KB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_SIZE_IN_KB_SV, ""},  /* fuzzy */

/* c64/georam.c */
/* en */ {IDCLS_GEORAM_SIZE,    "Size of the GEORAM expansion unit"},
/* de */ {IDCLS_GEORAM_SIZE_DE, "Grösse der GEORAM Erweiterung"},
/* fr */ {IDCLS_GEORAM_SIZE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_GEORAM_SIZE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_GEORAM_SIZE_NL, "Grootte van de GEORAM uitbreidings eenheid"},
/* pl */ {IDCLS_GEORAM_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_GEORAM_SIZE_SV, ""},  /* fuzzy */

/* c64/psid.c */
/* en */ {IDCLS_SID_PLAYER_MODE,    "SID player mode"},
/* de */ {IDCLS_SID_PLAYER_MODE_DE, "SID player Modus"},
/* fr */ {IDCLS_SID_PLAYER_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SID_PLAYER_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SID_PLAYER_MODE_NL, "SID player modus"},
/* pl */ {IDCLS_SID_PLAYER_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SID_PLAYER_MODE_SV, ""},  /* fuzzy */

/* c64/psid.c */
/* en */ {IDCLS_OVERWRITE_PSID_SETTINGS,    "Override PSID settings for Video standard and SID model"},
/* de */ {IDCLS_OVERWRITE_PSID_SETTINGS_DE, "Überschreiben der PSID Einstellungen bezüglich Videostandard und SID Modell"},
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
/* de */ {IDCLS_ENABLE_RAMCART_DE, "RAMCART Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_RAMCART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_RAMCART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAMCART_NL, "Aktiveer de RAMCART uitbreiding"},
/* pl */ {IDCLS_ENABLE_RAMCART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAMCART_SV, ""},  /* fuzzy */

/* c64/ramcart.c */
/* en */ {IDCLS_DISABLE_RAMCART,    "Disable the RAMCART expansion"},
/* de */ {IDCLS_DISABLE_RAMCART_DE, "RAMCART Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAMCART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_RAMCART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAMCART_NL, "De RAMCART uitbreiding aflsuiten"},
/* pl */ {IDCLS_DISABLE_RAMCART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAMCART_SV, ""},  /* fuzzy */

/* c64/ramcart.c */
/* en */ {IDCLS_SPECIFY_RAMCART_NAME,    "Specify name of RAMCART image"},
/* de */ {IDCLS_SPECIFY_RAMCART_NAME_DE, "Dateiname für RAMCART Erweiterung"},
/* fr */ {IDCLS_SPECIFY_RAMCART_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_RAMCART_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RAMCART_NAME_NL, "Geef de naam van het RAMCART bestand"},
/* pl */ {IDCLS_SPECIFY_RAMCART_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RAMCART_NAME_SV, ""},  /* fuzzy */

/* c64/ramcart.c */
/* en */ {IDCLS_RAMCART_SIZE,    "Size of the RAMCART expansion"},
/* de */ {IDCLS_RAMCART_SIZE_DE, "Grösse der RAMCART Erweiterung"},
/* fr */ {IDCLS_RAMCART_SIZE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_RAMCART_SIZE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_RAMCART_SIZE_NL, "Grootte van de RAMCART uitbreiding"},
/* pl */ {IDCLS_RAMCART_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RAMCART_SIZE_SV, ""},  /* fuzzy */

/* c64/reu.c */
/* en */ {IDCLS_ENABLE_REU,    "Enable the RAM expansion unit"},
/* de */ {IDCLS_ENABLE_REU_DE, "REU Speichererweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_REU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_REU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_REU_NL, "Aktiveer de REU uitbreiding"},
/* pl */ {IDCLS_ENABLE_REU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_REU_SV, ""},  /* fuzzy */

/* c64/reu.c */
/* en */ {IDCLS_DISABLE_REU,    "Disable the RAM expansion unit"},
/* de */ {IDCLS_DISABLE_REU_DE, "REU Speichererweiterung deaktivieren"}, 
/* fr */ {IDCLS_DISABLE_REU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_REU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_REU_NL, "De REU uitbreiding afsluiten"},
/* pl */ {IDCLS_DISABLE_REU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_REU_SV, ""},  /* fuzzy */

/* c64/reu.c */
/* en */ {IDCLS_SPECIFY_REU_NAME,    "Specify name of REU image"},
/* de */ {IDCLS_SPECIFY_REU_NAME_DE, "Dateiname für REU Speichererweiterung"},
/* fr */ {IDCLS_SPECIFY_REU_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_REU_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_REU_NAME_NL, "Geef de naam van het REU bestand"},
/* pl */ {IDCLS_SPECIFY_REU_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_REU_NAME_SV, ""},  /* fuzzy */

/* c64/reu.c */
/* en */ {IDCLS_REU_SIZE,    "Size of the RAM expansion unit"},
/* de */ {IDCLS_REU_SIZE_DE, "Grösse der REU Speichererweiterung"},
/* fr */ {IDCLS_REU_SIZE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_REU_SIZE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_REU_SIZE_NL, "Grootte van de REU uitbreiding"},
/* pl */ {IDCLS_REU_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_REU_SIZE_SV, ""},  /* fuzzy */

#ifdef HAVE_TFE
/* c64/tfe.c */
/* en */ {IDCLS_ENABLE_TFE,    "Enable the TFE (\"The Final Ethernet\") unit"},
/* de */ {IDCLS_ENABLE_TFE_DE, "TFE Ethernetemulation aktivieren"},
/* fr */ {IDCLS_ENABLE_TFE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_TFE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_TFE_NL, "Aktiveer de TFE (\"The Final Ethernet\") eenheid"},
/* pl */ {IDCLS_ENABLE_TFE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TFE_SV, ""},  /* fuzzy */

/* c64/tfe.c */
/* en */ {IDCLS_DISABLE_TFE,    "Disable the TFE (\"The Final Ethernet\") unit"},
/* de */ {IDCLS_DISABLE_TFE_DE, "TFE Ethernetemulation deaktivieren"},
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

/* c64/ide64.c, gfxoutputdrv/ffmpegdrv.c, datasette.c,
   debug.c, mouse.c, ram.c, sound.c, vsync.c */
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

/* drive/drive-cmdline-options.c, printerdrv/interface-serial.c,
   attach.c */
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

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_P_0_2000,    "<0-2000>"},
/* de */ {IDCLS_P_0_2000_DE, "<0-2000>"},
/* fr */ {IDCLS_P_0_2000_FR, "<0-2000>"},
/* it */ {IDCLS_P_0_2000_IT, "<0-2000>"},
/* nl */ {IDCLS_P_0_2000_NL, "<0-2000>"},
/* pl */ {IDCLS_P_0_2000_PL, "<0-2000>"},
/* sv */ {IDCLS_P_0_2000_SV, "<0-2000>"},

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_SATURATION,    "Set saturation of internal calculated palette [1000]"},
/* de */ {IDCLS_SET_SATURATION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_SATURATION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_SATURATION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_SATURATION_NL, "Zet de verzadiging van het intern berekend kleuren palette [1000]"},
/* pl */ {IDCLS_SET_SATURATION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SATURATION_SV, ""},  /* fuzzy */

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_CONTRAST,    "Set contrast of internal calculated palette [1100]"},
/* de */ {IDCLS_SET_CONTRAST_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_CONTRAST_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_CONTRAST_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_CONTRAST_NL, "Zet het contrast van het intern berekend kleuren palette [1100]"},
/* pl */ {IDCLS_SET_CONTRAST_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_CONTRAST_SV, ""},  /* fuzzy */

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_BRIGHTNESS,    "Set brightness of internal calculated palette [1100]"},
/* de */ {IDCLS_SET_BRIGHTNESS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_BRIGHTNESS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_BRIGHTNESS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_BRIGHTNESS_NL, "Zet de helderheid van het intern berekend kleuren palette [1100]"},
/* pl */ {IDCLS_SET_BRIGHTNESS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_BRIGHTNESS_SV, ""},  /* fuzzy */

/* plus4/ted-cmdline-options.c`, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_GAMMA,    "Set gamma of internal calculated palette [900]"},
/* de */ {IDCLS_SET_GAMMA_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_GAMMA_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_GAMMA_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_GAMMA_NL, "Zet de gamma van het intern berekend kleuren palette [900]"},
/* pl */ {IDCLS_SET_GAMMA_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_GAMMA_SV, ""},  /* fuzzy */

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME,    "Specify name of printer driver for device #4"},
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_NL, "Geef de naam van het stuurprogramma voor apparaat #4"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_SV, ""},  /* fuzzy */

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME,    "Specify name of printer driver for device #5"},
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_NL, "Geef de naam van het stuurprogramma voor apparaat #5"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_SV, ""},  /* fuzzy */

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME,    "Specify name of printer driver for the userport printer"},
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_NL, "Geef de naam van het stuurprogramma voor de userport printer"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_SV, ""},  /* fuzzy */

/* printerdrv/interface-serial.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_4,    "Set device type for device #4 (0: NONE, 1: FS, 2: REAL)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_4_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_DEVICE_TYPE_4_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DEVICE_TYPE_4_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DEVICE_TYPE_4_NL, "Zet apparaat soort voor apparaat #4 (0: GEEN, 1: FS, 2: ECHT)"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_4_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_4_SV, ""},  /* fuzzy */

/* printerdrv/interface-serial.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_5,    "Set device type for device #5 (0: NONE, 1: FS, 2: REAL)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_5_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_DEVICE_TYPE_5_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DEVICE_TYPE_5_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DEVICE_TYPE_5_NL, "Zet apparaat soort voor apparaat #5 (0: GEEN, 1: FS, 2: ECHT)"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_5_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_5_SV, ""},  /* fuzzy */

/* printerdrv/interface-userport.c */
/* en */ {IDCLS_ENABLE_USERPORT_PRINTER,    "Enable the userport printer emulation"},
/* de */ {IDCLS_ENABLE_USERPORT_PRINTER_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_USERPORT_PRINTER_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_USERPORT_PRINTER_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_USERPORT_PRINTER_NL, "Aktiveer de userport printer emulatie"},
/* pl */ {IDCLS_ENABLE_USERPORT_PRINTER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_USERPORT_PRINTER_SV, ""},  /* fuzzy */

/* printerdrv/interface-userport.c */
/* en */ {IDCLS_DISABLE_USERPORT_PRINTER,    "Disable the userport printer emulation"},
/* de */ {IDCLS_DISABLE_USERPORT_PRINTER_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_USERPORT_PRINTER_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_USERPORT_PRINTER_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_USERPORT_PRINTER_NL, "De userport printer emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_USERPORT_PRINTER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_USERPORT_PRINTER_SV, ""},  /* fuzzy */

/* printerdrv/output-select.c */
/* en */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME,    "Specify name of output device for device #4"},
/* de */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_NL, "Geef de naam van het uitvoer apparaat voor apparaat #4"},
/* pl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_SV, ""},  /* fuzzy */

/* printerdrv/output-select.c */
/* en */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME,    "Specify name of output device for device #5"},
/* de */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_NL, "Geef de naam van het uitvoer apparaat voor apparaat #5"},
/* pl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_SV, ""},  /* fuzzy */

/* printerdrv/output-select.c */
/* en */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME,    "Specify name of output device for the userport printer"},
/* de */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_NL, "Geef de naam van het uitvoer apparaat voor de userport printer"},
/* pl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_SV, ""},  /* fuzzy */

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME,    "Specify name of printer text device or dump file"},
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_NL, "Geef de naam van het text apparaat of het dump bestand"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_SV, ""},  /* fuzzy */

/* printerdrv/output-text.c */
/* en */ {IDCLS_P_0_2,    "<0-2>"},
/* de */ {IDCLS_P_0_2_DE, "<0-2>"},
/* fr */ {IDCLS_P_0_2_FR, "<0-2>"},
/* it */ {IDCLS_P_0_2_IT, "<0-2>"},
/* nl */ {IDCLS_P_0_2_NL, "<0-2>"},
/* pl */ {IDCLS_P_0_2_PL, "<0-2>"},
/* sv */ {IDCLS_P_0_2_SV, "<0-2>"},

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_4,    "Specify printer text output device for IEC printer #4"},
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_4_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_4_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_4_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_4_NL, "Geef het text uitvoer apparaat voor IEC printer #4"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_4_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_4_SV, ""},  /* fuzzy */

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_5,    "Specify printer text output device for IEC printer #5"},
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_5_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_5_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_5_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_5_NL, "Geef het text uitvoer apparaat voor IEC printer #5"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_5_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_5_SV, ""},  /* fuzzy */

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_USERPORT,    "Specify printer text output device for userport printer"},
/* de */ {IDCLS_SPECIFY_TEXT_USERPORT_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_TEXT_USERPORT_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_TEXT_USERPORT_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TEXT_USERPORT_NL, "Geef het text uitvoer apparaat voor de userport printer"},
/* pl */ {IDCLS_SPECIFY_TEXT_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_USERPORT_SV, ""},  /* fuzzy */

/* raster/raster-cmdline-options.c */
/* en */ {IDCLS_ENABLE_VIDEO_CACHE,    "Enable the video cache"},
/* de */ {IDCLS_ENABLE_VIDEO_CACHE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_VIDEO_CACHE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_VIDEO_CACHE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_VIDEO_CACHE_NL, "Aktiveer de video cache"},
/* pl */ {IDCLS_ENABLE_VIDEO_CACHE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_VIDEO_CACHE_SV, ""},  /* fuzzy */

/* raster/raster-cmdline-options.c */
/* en */ {IDCLS_DISABLE_VIDEO_CACHE,    "Disable the video cache"},
/* de */ {IDCLS_DISABLE_VIDEO_CACHE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_VIDEO_CACHE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_VIDEO_CACHE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_VIDEO_CACHE_NL, "De video cache afsluiten"},
/* pl */ {IDCLS_DISABLE_VIDEO_CACHE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_VIDEO_CACHE_SV, ""},  /* fuzzy */

#ifdef HAVE_RS232
/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_1_NAME,    "Specify name of first RS232 device"},
/* de */ {IDCLS_SPECIFY_RS232_1_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_RS232_1_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_RS232_1_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RS232_1_NAME_NL, "Geef de naam van het eerste RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_1_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_1_NAME_SV, ""},  /* fuzzy */

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_2_NAME,    "Specify name of second RS232 device"},
/* de */ {IDCLS_SPECIFY_RS232_2_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_RS232_2_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_RS232_2_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RS232_2_NAME_NL, "Geef de naam van het tweede RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_2_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_2_NAME_SV, ""},  /* fuzzy */

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_3_NAME,    "Specify name of third RS232 device"},
/* de */ {IDCLS_SPECIFY_RS232_3_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_RS232_3_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_RS232_3_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RS232_3_NAME_NL, "Geef de naam van het derde RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_3_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_3_NAME_SV, ""},  /* fuzzy */

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_4_NAME,    "Specify name of fourth RS232 device"},
/* de */ {IDCLS_SPECIFY_RS232_4_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_RS232_4_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_RS232_4_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RS232_4_NAME_NL, "Geef de naam van het vierde RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_4_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_4_NAME_SV, ""},  /* fuzzy */
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_ENABLE_RS232_USERPORT,    "Enable RS232 userport emulation"},
/* de */ {IDCLS_ENABLE_RS232_USERPORT_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_RS232_USERPORT_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_RS232_USERPORT_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RS232_USERPORT_NL, "Aktiveer RS232 userport emulatie"},
/* pl */ {IDCLS_ENABLE_RS232_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RS232_USERPORT_SV, ""},  /* fuzzy */

/* rs232drv/rsuser.c */
/* en */ {IDCLS_DISABLE_RS232_USERPORT,    "Disable RS232 userport emulation"},
/* de */ {IDCLS_DISABLE_RS232_USERPORT_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_RS232_USERPORT_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_RS232_USERPORT_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RS232_USERPORT_NL, "RS232 userport emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_RS232_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RS232_USERPORT_SV, ""},  /* fuzzy */

/* rs232drv/rsuser.c */
/* en */ {IDCLS_P_BAUD,    "<baud>"},
/* de */ {IDCLS_P_BAUD_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_BAUD_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_BAUD_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_BAUD_NL, "<baud>"},
/* pl */ {IDCLS_P_BAUD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_BAUD_SV, ""},  /* fuzzy */

/* rs232drv/rsuser.c */
/* en */ {IDCLS_SET_BAUD_RS232_USERPORT,    "Set the baud rate of the RS232 userport emulation."},
/* de */ {IDCLS_SET_BAUD_RS232_USERPORT_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_BAUD_RS232_USERPORT_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_BAUD_RS232_USERPORT_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_BAUD_RS232_USERPORT_NL, "Zet de baud rate van de RS232 userport emulatie."},
/* pl */ {IDCLS_SET_BAUD_RS232_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_BAUD_RS232_USERPORT_SV, ""},  /* fuzzy */

/* rs232drv/rsuser.c, aciacore.c */
/* en */ {IDCLS_P_0_3,    "<0-3>"},
/* de */ {IDCLS_P_0_3_DE, "<0-3>"},
/* fr */ {IDCLS_P_0_3_FR, "<0-3>"},
/* it */ {IDCLS_P_0_3_IT, "<0-3>"},
/* nl */ {IDCLS_P_0_3_NL, "<0-3>"},
/* pl */ {IDCLS_P_0_3_PL, "<0-3>"},
/* sv */ {IDCLS_P_0_3_SV, "<0-3>"},

/* rs232drv/rsuser.c */
/* en */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT,    "Specify VICE RS232 device for userport"},
/* de */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_NL, "Geef VICE RS232 apparaat voor userport"},
/* pl */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_SV, ""},  /* fuzzy */

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_4,    "Enable IEC device emulation for device #4"},
/* de */ {IDCLS_ENABLE_IEC_4_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_IEC_4_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_IEC_4_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_4_NL, "Aktiveer IEC apparaat emulatie voor apparaat #4"},
/* pl */ {IDCLS_ENABLE_IEC_4_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_4_SV, ""},  /* fuzzy */

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_4,    "Disable IEC device emulation for device #4"},
/* de */ {IDCLS_DISABLE_IEC_4_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_IEC_4_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_IEC_4_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_4_NL, "IEC apparaat emulatie voor apparaat #4 afsluiten"},
/* pl */ {IDCLS_DISABLE_IEC_4_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_4_SV, ""},  /* fuzzy */

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_5,    "Enable IEC device emulation for device #5"},
/* de */ {IDCLS_ENABLE_IEC_5_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_IEC_5_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_IEC_5_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_5_NL, "Aktiveer IEC apparaat emulatie voor apparaat #5"},
/* pl */ {IDCLS_ENABLE_IEC_5_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_5_SV, ""},  /* fuzzy */

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_5,    "Disable IEC device emulation for device #5"},
/* de */ {IDCLS_DISABLE_IEC_5_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_IEC_5_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_IEC_5_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_5_NL, "IEC apparaat emulatie voor apparaat #5 afsluiten"},
/* pl */ {IDCLS_DISABLE_IEC_5_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_5_SV, ""},  /* fuzzy */

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_8,    "Enable IEC device emulation for device #8"},
/* de */ {IDCLS_ENABLE_IEC_8_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_IEC_8_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_IEC_8_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_8_NL, "Aktiveer IEC apparaat emulatie voor apparaat #8"},
/* pl */ {IDCLS_ENABLE_IEC_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_8_SV, ""},  /* fuzzy */

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_8,    "Disable IEC device emulation for device #8"},
/* de */ {IDCLS_DISABLE_IEC_8_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_IEC_8_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_IEC_8_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_8_NL, "IEC apparaat emulatie voor apparaat #8 afsluiten"},
/* pl */ {IDCLS_DISABLE_IEC_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_8_SV, ""},  /* fuzzy */

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_9,    "Enable IEC device emulation for device #9"},
/* de */ {IDCLS_ENABLE_IEC_9_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_IEC_9_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_IEC_9_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_9_NL, "Aktiveer IEC apparaat emulatie voor apparaat #9"},
/* pl */ {IDCLS_ENABLE_IEC_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_9_SV, ""},  /* fuzzy */

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_9,    "Disable IEC device emulation for device #9"},
/* de */ {IDCLS_DISABLE_IEC_9_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_IEC_9_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_IEC_9_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_9_NL, "IEC apparaat emulatie voor apparaat #9 afsluiten"},
/* pl */ {IDCLS_DISABLE_IEC_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_9_SV, ""},  /* fuzzy */

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_10,    "Enable IEC device emulation for device #10"},
/* de */ {IDCLS_ENABLE_IEC_10_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_IEC_10_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_IEC_10_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_10_NL, "Aktiveer IEC apparaat emulatie voor apparaat #10"},
/* pl */ {IDCLS_ENABLE_IEC_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_10_SV, ""},  /* fuzzy */

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_10,    "Disable IEC device emulation for device #10"},
/* de */ {IDCLS_DISABLE_IEC_10_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_IEC_10_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_IEC_10_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_10_NL, "IEC apparaat emulatie voor apparaat #10 afsluiten"},
/* pl */ {IDCLS_DISABLE_IEC_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_10_SV, ""},  /* fuzzy */

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_11,    "Enable IEC device emulation for device #11"},
/* de */ {IDCLS_ENABLE_IEC_11_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_IEC_11_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_IEC_11_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_11_NL, "Aktiveer IEC apparaat emulatie voor apparaat #11"},
/* pl */ {IDCLS_ENABLE_IEC_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_11_SV, ""},  /* fuzzy */

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_11,    "Disable IEC device emulation for device #11"},
/* de */ {IDCLS_DISABLE_IEC_11_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_IEC_11_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_IEC_11_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_11_NL, "IEC apparaat emulatie voor apparaat #11 afsluiten"},
/* pl */ {IDCLS_DISABLE_IEC_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_11_SV, ""},  /* fuzzy */

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_P_ENGINE,    "<engine>"},
/* de */ {IDCLS_P_ENGINE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_ENGINE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_ENGINE_NL, "<kern>"},
/* pl */ {IDCLS_P_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_ENGINE_SV, ""},  /* fuzzy */

#if !defined(HAVE_RESID) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if defined(HAVE_RESID) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 1: ReSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 1: ReSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if !defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 2: Catweasel)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 2: Catweasel)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if !defined(HAVE_RESID) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 3: HardSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 1: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: ReSID, 1: ReSID, 2: Catweasel)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if defined(HAVE_RESID) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 1: ReSID, 3: HardSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: ReSID, 1: ReSID, 3: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if !defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 2: Catweasel, 3: HardSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: ReSID, 2: Catweasel, 3: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: ReSID, 1: ReSID, 2: Catweasel, 3: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SECOND_SID,    "Enable second SID"},
/* de */ {IDCLS_ENABLE_SECOND_SID_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_SECOND_SID_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SECOND_SID_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SECOND_SID_NL, "Aktiveer stereo SID"},
/* pl */ {IDCLS_ENABLE_SECOND_SID_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SECOND_SID_SV, ""},  /* fuzzy */

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_P_BASE_ADDRESS,    "<base address>"},
/* de */ {IDCLS_P_BASE_ADDRESS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_BASE_ADDRESS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_BASE_ADDRESS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_BASE_ADDRESS_NL, "<basis adres>"},
/* pl */ {IDCLS_P_BASE_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_BASE_ADDRESS_SV, ""},  /* fuzzy */

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_2_ADDRESS,    "Specify base address for 2nd SID"},
/* de */ {IDCLS_SPECIFY_SID_2_ADDRESS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SID_2_ADDRESS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_2_ADDRESS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SID_2_ADDRESS_NL, "Geef het basis adres van de 2e SID"},
/* pl */ {IDCLS_SPECIFY_SID_2_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_2_ADDRESS_SV, ""},  /* fuzzy */

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_P_MODEL,    "<model>"},
/* de */ {IDCLS_P_MODEL_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_MODEL_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_MODEL_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_MODEL_NL, "<model>"},
/* pl */ {IDCLS_P_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_MODEL_SV, ""},  /* fuzzy */

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_MODEL,    "Specify SID model (0: 6581, 1: 8580, 2: 8580 + digi boost)"},
/* de */ {IDCLS_SPECIFY_SID_MODEL_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SID_MODEL_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_MODEL_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SID_MODEL_NL, "Geef het SID model (0: 6581, 1: 8580, 2: 8580 + digi boost)"},
/* pl */ {IDCLS_SPECIFY_SID_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_MODEL_SV, ""},  /* fuzzy */

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SID_FILTERS,    "Emulate SID filters"},
/* de */ {IDCLS_ENABLE_SID_FILTERS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_SID_FILTERS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SID_FILTERS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SID_FILTERS_NL, "SID filters emuleren"},
/* pl */ {IDCLS_ENABLE_SID_FILTERS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SID_FILTERS_SV, ""},  /* fuzzy */

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SID_FILTERS,    "Do not emulate SID filters"},
/* de */ {IDCLS_DISABLE_SID_FILTERS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_SID_FILTERS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_SID_FILTERS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SID_FILTERS_NL, "SID filters niet emuleren"},
/* pl */ {IDCLS_DISABLE_SID_FILTERS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SID_FILTERS_SV, ""},  /* fuzzy */

#ifdef HAVE_RESID
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_RESID_SAMPLING_METHOD,    "reSID sampling method (0: fast, 1: interpolating, 2: resampling, 3: fast resampling)"},
/* de */ {IDCLS_RESID_SAMPLING_METHOD_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_RESID_SAMPLING_METHOD_FR, ""},  /* fuzzy */
/* it */ {IDCLS_RESID_SAMPLING_METHOD_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_RESID_SAMPLING_METHOD_NL, "reSID sampling methode (0: snel, 1: interpoleren, 2: resampling, 3: snelle resampling)"},
/* pl */ {IDCLS_RESID_SAMPLING_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RESID_SAMPLING_METHOD_SV, ""},  /* fuzzy */

/* sid/sid-cmdline-options.c, vsync.c */
/* en */ {IDCLS_P_PERCENT,    "<percent>"},
/* de */ {IDCLS_P_PERCENT_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_PERCENT_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_PERCENT_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_PERCENT_NL, "<procent>"},
/* pl */ {IDCLS_P_PERCENT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_PERCENT_SV, ""},  /* fuzzy */

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_PASSBAND_PERCENTAGE,    "reSID resampling passband in percentage of total bandwidth (0 - 90)"},
/* de */ {IDCLS_PASSBAND_PERCENTAGE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_PASSBAND_PERCENTAGE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_PASSBAND_PERCENTAGE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_PASSBAND_PERCENTAGE_NL, "reSID resampling passband in percentage van de totale bandbreedte (0 - 90)"},
/* pl */ {IDCLS_PASSBAND_PERCENTAGE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PASSBAND_PERCENTAGE_SV, ""},  /* fuzzy */

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_RESID_GAIN_PERCENTAGE,    "reSID gain in percent (90 - 100)"},
/* de */ {IDCLS_RESID_GAIN_PERCENTAGE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_RESID_GAIN_PERCENTAGE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_RESID_GAIN_PERCENTAGE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_RESID_GAIN_PERCENTAGE_NL, "reSID versterking procent (90 - 100)"},
/* pl */ {IDCLS_RESID_GAIN_PERCENTAGE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RESID_GAIN_PERCENTAGE_SV, ""},  /* fuzzy */
#endif

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_MEMORY_16KB,    "Set the VDC memory size to 16KB"},
/* de */ {IDCLS_SET_VDC_MEMORY_16KB_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_VDC_MEMORY_16KB_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_VDC_MEMORY_16KB_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_VDC_MEMORY_16KB_NL, "Zet de VDC geheugen grootte als 16KB"},
/* pl */ {IDCLS_SET_VDC_MEMORY_16KB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VDC_MEMORY_16KB_SV, ""},  /* fuzzy */

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_MEMORY_64KB,    "Set the VDC memory size to 64KB"},
/* de */ {IDCLS_SET_VDC_MEMORY_64KB_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_VDC_MEMORY_64KB_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_VDC_MEMORY_64KB_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_VDC_MEMORY_64KB_NL, "Zet de VDC geheugen grootte als 64KB"},
/* pl */ {IDCLS_SET_VDC_MEMORY_64KB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VDC_MEMORY_64KB_SV, ""},  /* fuzzy */

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_REVISION,    "Set VDC revision (0..2)"},
/* de */ {IDCLS_SET_VDC_REVISION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_VDC_REVISION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_VDC_REVISION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_VDC_REVISION_NL, "Zet de VDC revisie (0..2)"},
/* pl */ {IDCLS_SET_VDC_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VDC_REVISION_SV, ""},  /* fuzzy */

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_P_SPEC,    "<spec>"},
/* de */ {IDCLS_P_SPEC_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_SPEC_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_SPEC_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_SPEC_NL, "<spec>"},
/* pl */ {IDCLS_P_SPEC_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_SPEC_SV, ""},  /* fuzzy */

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_MEMORY_CONFIG,    "Specify memory configuration"},
/* de */ {IDCLS_SPECIFY_MEMORY_CONFIG_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_MEMORY_CONFIG_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_MEMORY_CONFIG_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_MEMORY_CONFIG_NL, "Geef geheugen konfiguratie"},
/* pl */ {IDCLS_SPECIFY_MEMORY_CONFIG_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_MEMORY_CONFIG_SV, ""},  /* fuzzy */

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_ENABLE_VIC1112_IEEE488,    "Enable VIC-1112 IEEE488 interface"},
/* de */ {IDCLS_ENABLE_VIC1112_IEEE488_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_VIC1112_IEEE488_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_VIC1112_IEEE488_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_VIC1112_IEEE488_NL, "Aktiveer VIC-1112 IEEE488 interface"},
/* pl */ {IDCLS_ENABLE_VIC1112_IEEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_VIC1112_IEEE488_SV, ""},  /* fuzzy */

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_DISABLE_VIC1112_IEEE488,    "Disable VIC-1112 IEEE488 interface"},
/* de */ {IDCLS_DISABLE_VIC1112_IEEE488_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_VIC1112_IEEE488_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_VIC1112_IEEE488_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_VIC1112_IEEE488_NL, "VIC-1112 IEEE488 interface afsluiten"},
/* pl */ {IDCLS_DISABLE_VIC1112_IEEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_VIC1112_IEEE488_SV, ""},  /* fuzzy */

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME,    "Specify 4/8/16K extension ROM name at $2000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_NL, "Geef de naam van het bestand voor de 4/8/16K ROM op $2000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_SV, ""},  /* fuzzy */

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME,    "Specify 4/8/16K extension ROM name at $4000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_NL, "Geef de naam van het bestand voor de 4/8/16K ROM op $4000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_SV, ""},  /* fuzzy */

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME,    "Specify 4/8/16K extension ROM name at $6000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_NL, "Geef de naam van het bestand voor de 4/8/16K ROM op $6000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_SV, ""},  /* fuzzy */

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME,    "Specify 4/8K extension ROM name at $A000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_NL, "Geef de naam van het bestand voor de 4/8K ROM op $A000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_SV, ""},  /* fuzzy */

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME,    "Specify 4K extension ROM name at $B000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_NL, "Geef de naam van het bestand voor de 4K ROM op $B000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_SV, ""},  /* fuzzy */

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SPRITE_BACKGROUND,    "Enable sprite-background collision registers"},
/* de */ {IDCLS_ENABLE_SPRITE_BACKGROUND_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_SPRITE_BACKGROUND_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SPRITE_BACKGROUND_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SPRITE_BACKGROUND_NL, "Aktiveer sprite-achtergrond botsing registers"},
/* pl */ {IDCLS_ENABLE_SPRITE_BACKGROUND_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SPRITE_BACKGROUND_SV, ""},  /* fuzzy */

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SPRITE_BACKGROUND,    "Disable sprite-background collision registers"},
/* de */ {IDCLS_DISABLE_SPRITE_BACKGROUND_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_SPRITE_BACKGROUND_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_SPRITE_BACKGROUND_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SPRITE_BACKGROUND_NL, "Sprite-achtergrond botsing registers afsluiten"},
/* pl */ {IDCLS_DISABLE_SPRITE_BACKGROUND_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SPRITE_BACKGROUND_SV, ""},  /* fuzzy */

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SPRITE_SPRITE,    "Enable sprite-sprite collision registers"},
/* de */ {IDCLS_ENABLE_SPRITE_SPRITE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_SPRITE_SPRITE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SPRITE_SPRITE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SPRITE_SPRITE_NL, "Aktiveer sprite-sprite botsing registers"},
/* pl */ {IDCLS_ENABLE_SPRITE_SPRITE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SPRITE_SPRITE_SV, ""},  /* fuzzy */

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SPRITE_SPRITE,    "Disable sprite-sprite collision registers"},
/* de */ {IDCLS_DISABLE_SPRITE_SPRITE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_SPRITE_SPRITE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_SPRITE_SPRITE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SPRITE_SPRITE_NL, "Aktiveer sprite-sprite botsing registers"},
/* pl */ {IDCLS_DISABLE_SPRITE_SPRITE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SPRITE_SPRITE_SV, ""},  /* fuzzy */

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_USE_NEW_LUMINANCES,    "Use new luminances"},
/* de */ {IDCLS_USE_NEW_LUMINANCES_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_USE_NEW_LUMINANCES_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_NEW_LUMINANCES_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_NEW_LUMINANCES_NL, "Gebruik nieuwe kleuren"},
/* pl */ {IDCLS_USE_NEW_LUMINANCES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_NEW_LUMINANCES_SV, ""},  /* fuzzy */

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_USE_OLD_LUMINANCES,    "Use old luminances"},
/* de */ {IDCLS_USE_OLD_LUMINANCES_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_USE_OLD_LUMINANCES_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_OLD_LUMINANCES_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_OLD_LUMINANCES_NL, "Gebruik oude kleuren"},
/* pl */ {IDCLS_USE_OLD_LUMINANCES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_OLD_LUMINANCES_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SIZE,    "Enable double size"},
/* de */ {IDCLS_ENABLE_DOUBLE_SIZE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_DOUBLE_SIZE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_DOUBLE_SIZE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DOUBLE_SIZE_NL, "Aktiveer dubbele grootte"},
/* pl */ {IDCLS_ENABLE_DOUBLE_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DOUBLE_SIZE_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SIZE,    "Disable double size"},
/* de */ {IDCLS_DISABLE_DOUBLE_SIZE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_DOUBLE_SIZE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_DOUBLE_SIZE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DOUBLE_SIZE_NL, "Dubbele grootte afsluiten"},
/* pl */ {IDCLS_DISABLE_DOUBLE_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DOUBLE_SIZE_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SCAN,    "Enable double scan"},
/* de */ {IDCLS_ENABLE_DOUBLE_SCAN_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_DOUBLE_SCAN_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_DOUBLE_SCAN_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DOUBLE_SCAN_NL, "Aktiveer dubbele scan"},
/* pl */ {IDCLS_ENABLE_DOUBLE_SCAN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DOUBLE_SCAN_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SCAN,    "Disable double scan"},
/* de */ {IDCLS_DISABLE_DOUBLE_SCAN_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_DOUBLE_SCAN_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_DOUBLE_SCAN_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DOUBLE_SCAN_NL, "Dubbele scan afsluiten"},
/* pl */ {IDCLS_DISABLE_DOUBLE_SCAN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DOUBLE_SCAN_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_HARDWARE_SCALING,    "Enable hardware scaling"},
/* de */ {IDCLS_ENABLE_HARDWARE_SCALING_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_HARDWARE_SCALING_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_HARDWARE_SCALING_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_HARDWARE_SCALING_NL, "Aktiveer hardware schalering"},
/* pl */ {IDCLS_ENABLE_HARDWARE_SCALING_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_HARDWARE_SCALING_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_HARDWARE_SCALING,    "Disable hardware scaling"},
/* de */ {IDCLS_DISABLE_HARDWARE_SCALING_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_HARDWARE_SCALING_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_HARDWARE_SCALING_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_HARDWARE_SCALING_NL, "Hardware schalering afsluiten"},
/* pl */ {IDCLS_DISABLE_HARDWARE_SCALING_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_HARDWARE_SCALING_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SCALE2X,    "Enable Scale2x"},
/* de */ {IDCLS_ENABLE_SCALE2X_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_SCALE2X_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SCALE2X_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SCALE2X_NL, "Aktiveer Scale2x"},
/* pl */ {IDCLS_ENABLE_SCALE2X_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SCALE2X_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SCALE2X,    "Disable Scale2x"},
/* de */ {IDCLS_DISABLE_SCALE2X_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_SCALE2X_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_SCALE2X_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SCALE2X_NL, "Scale2x afsluiten"},
/* pl */ {IDCLS_DISABLE_SCALE2X_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SCALE2X_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_USE_INTERNAL_CALC_PALETTE,    "Use an internal calculated palette"},
/* de */ {IDCLS_USE_INTERNAL_CALC_PALETTE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_USE_INTERNAL_CALC_PALETTE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_INTERNAL_CALC_PALETTE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_INTERNAL_CALC_PALETTE_NL, "Gebruik een intern berekend kleuren palette"},
/* pl */ {IDCLS_USE_INTERNAL_CALC_PALETTE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_INTERNAL_CALC_PALETTE_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_USE_EXTERNAL_FILE_PALETTE,    "Use an external palette (file)"},
/* de */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_NL, "Gebruik een extern kleuren palette (bestand)"},
/* pl */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME,    "Specify name of file of external palette"},
/* de */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_NL, "Geef de naam van het extern kleuren palette bestand"},
/* pl */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FULLSCREEN_MODE,    "Enable fullscreen mode"},
/* de */ {IDCLS_ENABLE_FULLSCREEN_MODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FULLSCREEN_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FULLSCREEN_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FULLSCREEN_MODE_NL, "Aktiveer volscherm modus"},
/* pl */ {IDCLS_ENABLE_FULLSCREEN_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FULLSCREEN_MODE_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FULLSCREEN_MODE,    "Disable fullscreen mode"},
/* de */ {IDCLS_DISABLE_FULLSCREEN_MODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FULLSCREEN_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FULLSCREEN_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FULLSCREEN_MODE_NL, "Volscherm modus afsluiten"},
/* pl */ {IDCLS_DISABLE_FULLSCREEN_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FULLSCREEN_MODE_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_P_DEVICE,    "<device>"},
/* de */ {IDCLS_P_DEVICE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_DEVICE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_DEVICE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_DEVICE_NL, "<apparaat>"},
/* pl */ {IDCLS_P_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_DEVICE_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_SELECT_FULLSCREEN_DEVICE,    "Select fullscreen device"},
/* de */ {IDCLS_SELECT_FULLSCREEN_DEVICE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SELECT_FULLSCREEN_DEVICE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SELECT_FULLSCREEN_DEVICE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SELECT_FULLSCREEN_DEVICE_NL, "Selecteer volscherm apparaat"},
/* pl */ {IDCLS_SELECT_FULLSCREEN_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SELECT_FULLSCREEN_DEVICE_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN,    "Enable double size in fullscreen mode"},
/* de */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_NL, "Aktiveer dubbele grootte in volscherm modus"},
/* pl */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN,    "Disable double size in fullscreen mode"},
/* de */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_NL, "Dubbele grootte in volscherm modus afsluiten"},
/* pl */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN,    "Enable double scan in fullscreen mode"},
/* de */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_NL, "Aktiveer dubbele scan in volscherm modus"},
/* pl */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN,    "Disable double scan in fullscreen mode"},
/* de */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_NL, "Dubbele scan in volscherm modus afsluiten"},
/* pl */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_P_MODE,    "<mode>"},
/* de */ {IDCLS_P_MODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_MODE_NL, "<modus>"},
/* pl */ {IDCLS_P_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_MODE_SV, ""},  /* fuzzy */

/* video/video-cmdline-options.c */
/* en */ {IDCLS_SELECT_FULLSCREEN_MODE,    "Select fullscreen mode"},
/* de */ {IDCLS_SELECT_FULLSCREEN_MODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SELECT_FULLSCREEN_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SELECT_FULLSCREEN_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SELECT_FULLSCREEN_MODE_NL, "Selecteer volscherm modus"},
/* pl */ {IDCLS_SELECT_FULLSCREEN_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SELECT_FULLSCREEN_MODE_SV, ""},  /* fuzzy */

/* aciacore.c */
/* en */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE,    "Specify RS232 device this ACIA should work on"},
/* de */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_NL, "Geef het RS232 apparaat waarmee deze ACIA moet werken"},
/* pl */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_SV, ""},  /* fuzzy */

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_8,    "Set device type for device #8 (0: NONE, 1: FS, 2: REAL, 3: RAW)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_8_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_DEVICE_TYPE_8_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DEVICE_TYPE_8_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DEVICE_TYPE_8_NL, "Zet het apparaat soort voor apparaat #8 (0: GEEN, 1: FS, 2: ECHT, 3: BINAIR"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_8_SV, ""},  /* fuzzy */

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_9,    "Set device type for device #9 (0: NONE, 1: FS, 2: REAL, 3: RAW)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_9_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_DEVICE_TYPE_9_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DEVICE_TYPE_9_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DEVICE_TYPE_9_NL, "Zet het apparaat soort voor apparaat #9 (0: GEEN, 1: FS, 2: ECHT, 3: BINAIR"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_9_SV, ""},  /* fuzzy */

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_10,    "Set device type for device #10 (0: NONE, 1: FS, 2: REAL, 3: RAW)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_10_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_DEVICE_TYPE_10_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DEVICE_TYPE_10_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DEVICE_TYPE_10_NL, "Zet het apparaat soort voor apparaat #10 (0: GEEN, 1: FS, 2: ECHT, 3: BINAIR"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_10_SV, ""},  /* fuzzy */

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_11,    "Set device type for device #11 (0: NONE, 1: FS, 2: REAL, 3: RAW)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_11_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_DEVICE_TYPE_11_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DEVICE_TYPE_11_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DEVICE_TYPE_11_NL, "Zet het apparaat soort voor apparaat #11 (0: GEEN, 1: FS, 2: ECHT, 3: BINAIR"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_11_SV, ""},  /* fuzzy */

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_8,    "Attach disk image for drive #8 read only"},
/* de */ {IDCLS_ATTACH_READ_ONLY_8_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_READ_ONLY_8_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_READ_ONLY_8_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_READ_ONLY_8_NL, "Koppel disk bestand voor station #8 als alleen lezen"},
/* pl */ {IDCLS_ATTACH_READ_ONLY_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_ONLY_8_SV, ""},  /* fuzzy */

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_8,    "Attach disk image for drive #8 read write (if possible)"},
/* de */ {IDCLS_ATTACH_READ_WRITE_8_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_READ_WRITE_8_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_READ_WRITE_8_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_READ_WRITE_8_NL, "Koppel disk bestand voor station #8 als alleen schrijfbaar (indien mogelijk)"},
/* pl */ {IDCLS_ATTACH_READ_WRITE_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_WRITE_8_SV, ""},  /* fuzzy */

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_9,    "Attach disk image for drive #9 read only"},
/* de */ {IDCLS_ATTACH_READ_ONLY_9_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_READ_ONLY_9_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_READ_ONLY_9_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_READ_ONLY_9_NL, "Koppel disk bestand voor station #9 als alleen lezen"},
/* pl */ {IDCLS_ATTACH_READ_ONLY_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_ONLY_9_SV, ""},  /* fuzzy */

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_9,    "Attach disk image for drive #9 read write (if possible)"},
/* de */ {IDCLS_ATTACH_READ_WRITE_9_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_READ_WRITE_9_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_READ_WRITE_9_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_READ_WRITE_9_NL, "Koppel disk bestand voor station #9 als alleen schrijfbaar (indien mogelijk)"},
/* pl */ {IDCLS_ATTACH_READ_WRITE_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_WRITE_9_SV, ""},  /* fuzzy */

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_10,    "Attach disk image for drive #10 read only"},
/* de */ {IDCLS_ATTACH_READ_ONLY_10_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_READ_ONLY_10_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_READ_ONLY_10_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_READ_ONLY_10_NL, "Koppel disk bestand voor station #10 als alleen lezen"},
/* pl */ {IDCLS_ATTACH_READ_ONLY_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_ONLY_10_SV, ""},  /* fuzzy */

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_10,    "Attach disk image for drive #10 read write (if possible)"},
/* de */ {IDCLS_ATTACH_READ_WRITE_10_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_READ_WRITE_10_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_READ_WRITE_10_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_READ_WRITE_10_NL, "Koppel disk bestand voor station #10 als alleen schrijfbaar (indien mogelijk)"},
/* pl */ {IDCLS_ATTACH_READ_WRITE_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_WRITE_10_SV, ""},  /* fuzzy */

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_11,    "Attach disk image for drive #11 read only"},
/* de */ {IDCLS_ATTACH_READ_ONLY_11_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_READ_ONLY_11_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_READ_ONLY_11_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_READ_ONLY_11_NL, "Koppel disk bestand voor station #11 als alleen lezen"},
/* pl */ {IDCLS_ATTACH_READ_ONLY_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_ONLY_11_SV, ""},  /* fuzzy */

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_11,    "Attach disk image for drive #11 read write (if possible)"},
/* de */ {IDCLS_ATTACH_READ_WRITE_11_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_READ_WRITE_11_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_READ_WRITE_11_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_READ_WRITE_11_NL, "Koppel disk bestand voor station #11 als alleen schrijfbaar (indien mogelijk)"},
/* pl */ {IDCLS_ATTACH_READ_WRITE_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_WRITE_11_SV, ""},  /* fuzzy */

/* datasette.c */
/* en */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET,    "Enable automatic Datasette-Reset"},
/* de */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_NL, "Aktiveer automatische Datasette-Reset"},
/* pl */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_SV, ""},  /* fuzzy */

/* datasette.c */
/* en */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET,    "Disable automatic Datasette-Reset"},
/* de */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_NL, "Automatische Datasette-Reset afsluiten"},
/* pl */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_SV, ""},  /* fuzzy */

/* datasette.c */
/* en */ {IDCLS_SET_ZERO_TAP_DELAY,    "Set delay in cycles for a zero in the tap"},
/* de */ {IDCLS_SET_ZERO_TAP_DELAY_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_ZERO_TAP_DELAY_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_ZERO_TAP_DELAY_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_ZERO_TAP_DELAY_NL, "Zet de vertraging in cylcli voor een nul in de tap"},
/* pl */ {IDCLS_SET_ZERO_TAP_DELAY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_ZERO_TAP_DELAY_SV, ""},  /* fuzzy */

/* datasette.c */
/* en */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP,    "Set number of cycles added to each gap in the tap"},
/* de */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_NL, "Zet aantal extra cylcli voor elk gat in de tap"},
/* pl */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_SV, ""},  /* fuzzy */

#ifdef DEBUG
/* debug.c */
/* en */ {IDCLS_TRACE_MAIN_CPU,    "Trace the main CPU"},
/* de */ {IDCLS_TRACE_MAIN_CPU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_TRACE_MAIN_CPU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_TRACE_MAIN_CPU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_TRACE_MAIN_CPU_NL, "Traceer de hoofd CPU"},
/* pl */ {IDCLS_TRACE_MAIN_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_MAIN_CPU_SV, ""},  /* fuzzy */

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_MAIN_CPU,    "Do not trace the main CPU"},
/* de */ {IDCLS_DONT_TRACE_MAIN_CPU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DONT_TRACE_MAIN_CPU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DONT_TRACE_MAIN_CPU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DONT_TRACE_MAIN_CPU_NL, "Traceer de hoofd CPU niet"},
/* pl */ {IDCLS_DONT_TRACE_MAIN_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_MAIN_CPU_SV, ""},  /* fuzzy */

/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE0_CPU,    "Trace the drive0 CPU"},
/* de */ {IDCLS_TRACE_DRIVE0_CPU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_TRACE_DRIVE0_CPU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_TRACE_DRIVE0_CPU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_TRACE_DRIVE0_CPU_NL, "Traceer de CPU van drive 0/8"},
/* pl */ {IDCLS_TRACE_DRIVE0_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_DRIVE0_CPU_SV, ""},  /* fuzzy */

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE0_CPU,    "Do not trace the drive0 CPU"},
/* de */ {IDCLS_DONT_TRACE_DRIVE0_CPU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DONT_TRACE_DRIVE0_CPU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DONT_TRACE_DRIVE0_CPU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DONT_TRACE_DRIVE0_CPU_NL, "Traceer de CPU van drive 0/8 niet"},
/* pl */ {IDCLS_DONT_TRACE_DRIVE0_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_DRIVE0_CPU_SV, ""},  /* fuzzy */

/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE1_CPU,    "Trace the drive1 CPU"},
/* de */ {IDCLS_TRACE_DRIVE1_CPU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_TRACE_DRIVE1_CPU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_TRACE_DRIVE1_CPU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_TRACE_DRIVE1_CPU_NL, "Traceer de CPU van drive 1/9"},
/* pl */ {IDCLS_TRACE_DRIVE1_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_DRIVE1_CPU_SV, ""},  /* fuzzy */

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE1_CPU,    "Do not trace the drive1 CPU"},
/* de */ {IDCLS_DONT_TRACE_DRIVE1_CPU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DONT_TRACE_DRIVE1_CPU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DONT_TRACE_DRIVE1_CPU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DONT_TRACE_DRIVE1_CPU_NL, "Traceer de CPU van drive 1/9 niet"},
/* pl */ {IDCLS_DONT_TRACE_DRIVE1_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_DRIVE1_CPU_SV, ""},  /* fuzzy */

#if DRIVE_NUM > 2
/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE2_CPU,    "Trace the drive2 CPU"},
/* de */ {IDCLS_TRACE_DRIVE2_CPU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_TRACE_DRIVE2_CPU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_TRACE_DRIVE2_CPU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_TRACE_DRIVE2_CPU_NL, "Traceer de CPU van drive 2/10"},
/* pl */ {IDCLS_TRACE_DRIVE2_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_DRIVE2_CPU_SV, ""},  /* fuzzy */

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE2_CPU,    "Do not trace the drive2 CPU"},
/* de */ {IDCLS_DONT_TRACE_DRIVE2_CPU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DONT_TRACE_DRIVE2_CPU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DONT_TRACE_DRIVE2_CPU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DONT_TRACE_DRIVE2_CPU_NL, "Traceer de CPU van drive 2/10 niet"},
/* pl */ {IDCLS_DONT_TRACE_DRIVE2_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_DRIVE2_CPU_SV, ""},  /* fuzzy */
#endif

#if DRIVE_NUM > 3
/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE3_CPU,    "Trace the drive3 CPU"},
/* de */ {IDCLS_TRACE_DRIVE3_CPU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_TRACE_DRIVE3_CPU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_TRACE_DRIVE3_CPU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_TRACE_DRIVE3_CPU_NL, "Traceer de CPU van drive 3/11"},
/* pl */ {IDCLS_TRACE_DRIVE3_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_DRIVE3_CPU_SV, ""},  /* fuzzy */

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE3_CPU,    "Do not trace the drive3 CPU"},
/* de */ {IDCLS_DONT_TRACE_DRIVE3_CPU_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DONT_TRACE_DRIVE3_CPU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DONT_TRACE_DRIVE3_CPU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DONT_TRACE_DRIVE3_CPU_NL, "Traceer de CPU van drive 3/11 niet"},
/* pl */ {IDCLS_DONT_TRACE_DRIVE3_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_DRIVE3_CPU_SV, ""},  /* fuzzy */
#endif

/* debug.c */
/* en */ {IDCLS_TRACE_MODE,    "Trace mode (0=normal 1=small 2=history)"},
/* de */ {IDCLS_TRACE_MODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_TRACE_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_TRACE_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_TRACE_MODE_NL, "Traceer modus (0=normaal 1=klein 2=geschiedenis)"},
/* pl */ {IDCLS_TRACE_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_MODE_SV, ""},  /* fuzzy */
#endif

/* event.c */
/* en */ {IDCLS_PLAYBACK_RECORDED_EVENTS,    "Playback recorded events"},
/* de */ {IDCLS_PLAYBACK_RECORDED_EVENTS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_PLAYBACK_RECORDED_EVENTS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_PLAYBACK_RECORDED_EVENTS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_PLAYBACK_RECORDED_EVENTS_NL, "Afspelen opgenomen gebeurtenissen"},
/* pl */ {IDCLS_PLAYBACK_RECORDED_EVENTS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PLAYBACK_RECORDED_EVENTS_SV, ""},  /* fuzzy */

/* fliplist.c */
/* en */ {IDCLS_SPECIFY_FLIP_LIST_NAME,    "Specify name of the flip list file image"},
/* de */ {IDCLS_SPECIFY_FLIP_LIST_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_FLIP_LIST_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_FLIP_LIST_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FLIP_LIST_NAME_NL, "Geef de naam van het flip lijst bestand"},
/* pl */ {IDCLS_SPECIFY_FLIP_LIST_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FLIP_LIST_NAME_SV, ""},  /* fuzzy */

/* initcmdline.c */
/* en */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS,    "Show a list of the available options and exit normally"},
/* de */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_NL, "Toon een lijst van de beschikbare opties en exit zoals normaal"},
/* pl */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_SV, ""},  /* fuzzy */

#if (!defined  __OS2__ && !defined __BEOS__)
/* initcmdline.c */
/* en */ {IDCLS_CONSOLE_MODE,    "Console mode (for music playback)"},
/* de */ {IDCLS_CONSOLE_MODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_CONSOLE_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_CONSOLE_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_CONSOLE_MODE_NL, "Console modus (voor afspelen voor muziek)"},
/* pl */ {IDCLS_CONSOLE_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_CONSOLE_MODE_SV, ""},  /* fuzzy */

/* initcmdline.c */
/* en */ {IDCLS_ALLOW_CORE_DUMPS,    "Allow production of core dumps"},
/* de */ {IDCLS_ALLOW_CORE_DUMPS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ALLOW_CORE_DUMPS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ALLOW_CORE_DUMPS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ALLOW_CORE_DUMPS_NL, "Maken van core dumps toestaan"},
/* pl */ {IDCLS_ALLOW_CORE_DUMPS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ALLOW_CORE_DUMPS_SV, ""},  /* fuzzy */

/* initcmdline.c */
/* en */ {IDCLS_DONT_ALLOW_CORE_DUMPS,    "Do not produce core dumps"},
/* de */ {IDCLS_DONT_ALLOW_CORE_DUMPS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DONT_ALLOW_CORE_DUMPS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DONT_ALLOW_CORE_DUMPS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DONT_ALLOW_CORE_DUMPS_NL, "Maken van core dumps niet toestaan"},
/* pl */ {IDCLS_DONT_ALLOW_CORE_DUMPS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_ALLOW_CORE_DUMPS_SV, ""},  /* fuzzy */
#else
/* initcmdline.c */
/* en */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER,    "Don't call exception handler"},
/* de */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_NL, "Geen gebruik maken van de exception handler"},
/* pl */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_SV, ""},  /* fuzzy */

/* initcmdline.c */
/* en */ {IDCLS_CALL_EXCEPTION_HANDLER,    "Call exception handler (default)"},
/* de */ {IDCLS_CALL_EXCEPTION_HANDLER_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_CALL_EXCEPTION_HANDLER_FR, ""},  /* fuzzy */
/* it */ {IDCLS_CALL_EXCEPTION_HANDLER_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_CALL_EXCEPTION_HANDLER_NL, "Gebruik maken van de exception handler (standaard)"},
/* pl */ {IDCLS_CALL_EXCEPTION_HANDLER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_CALL_EXCEPTION_HANDLER_SV, ""},  /* fuzzy */
#endif

/* initcmdline.c */
/* en */ {IDCLS_RESTORE_DEFAULT_SETTINGS,    "Restore default (factory) settings"},
/* de */ {IDCLS_RESTORE_DEFAULT_SETTINGS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_RESTORE_DEFAULT_SETTINGS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_RESTORE_DEFAULT_SETTINGS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_RESTORE_DEFAULT_SETTINGS_NL, "Herstel standaard instelling"},
/* pl */ {IDCLS_RESTORE_DEFAULT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RESTORE_DEFAULT_SETTINGS_SV, ""},  /* fuzzy */

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AND_AUTOSTART,    "Attach and autostart tape/disk image <name>"},
/* de */ {IDCLS_ATTACH_AND_AUTOSTART_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_AND_AUTOSTART_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_AND_AUTOSTART_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AND_AUTOSTART_NL, "Koppel en autostart een tape/disk bestand <naam>"},
/* pl */ {IDCLS_ATTACH_AND_AUTOSTART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AND_AUTOSTART_SV, ""},  /* fuzzy */

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AND_AUTOLOAD,    "Attach and autoload tape/disk image <name>"},
/* de */ {IDCLS_ATTACH_AND_AUTOLOAD_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_AND_AUTOLOAD_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_AND_AUTOLOAD_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AND_AUTOLOAD_NL, "Koppel en autolaad een tape/disk bestand <naam>"},
/* pl */ {IDCLS_ATTACH_AND_AUTOLOAD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AND_AUTOLOAD_SV, ""},  /* fuzzy */

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_TAPE,    "Attach <name> as a tape image"},
/* de */ {IDCLS_ATTACH_AS_TAPE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_AS_TAPE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_AS_TAPE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AS_TAPE_NL, "Koppel <naam> als een tape bestand"},
/* pl */ {IDCLS_ATTACH_AS_TAPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_TAPE_SV, ""},  /* fuzzy */

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_8,    "Attach <name> as a disk image in drive #8"},
/* de */ {IDCLS_ATTACH_AS_DISK_8_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_AS_DISK_8_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_AS_DISK_8_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AS_DISK_8_NL, "Koppel <naam> als een disk bestand in station #8"},
/* pl */ {IDCLS_ATTACH_AS_DISK_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_DISK_8_SV, ""},  /* fuzzy */

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_9,    "Attach <name> as a disk image in drive #9"},
/* de */ {IDCLS_ATTACH_AS_DISK_9_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_AS_DISK_9_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_AS_DISK_9_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AS_DISK_9_NL, "Koppel <naam> als een disk bestand in station #9"},
/* pl */ {IDCLS_ATTACH_AS_DISK_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_DISK_9_SV, ""},  /* fuzzy */

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_10,    "Attach <name> as a disk image in drive #10"},
/* de */ {IDCLS_ATTACH_AS_DISK_10_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_AS_DISK_10_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_AS_DISK_10_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AS_DISK_10_NL, "Koppel <naam> als een disk bestand in station #10"},
/* pl */ {IDCLS_ATTACH_AS_DISK_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_DISK_10_SV, ""},  /* fuzzy */

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_11,    "Attach <name> as a disk image in drive #11"},
/* de */ {IDCLS_ATTACH_AS_DISK_11_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ATTACH_AS_DISK_11_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_AS_DISK_11_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AS_DISK_11_NL, "Koppel <naam> als een disk bestand in station #11"},
/* pl */ {IDCLS_ATTACH_AS_DISK_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_DISK_11_SV, ""},  /* fuzzy */

/* kbdbuf.c */
/* en */ {IDCLS_P_STRING,    "<string>"},
/* de */ {IDCLS_P_STRING_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_STRING_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_STRING_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_STRING_NL, "<string>"},
/* pl */ {IDCLS_P_STRING_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_STRING_SV, ""},  /* fuzzy */

/* kbdbuf.c */
/* en */ {IDCLS_PUT_STRING_INTO_KEYBUF,    "Put the specified string into the keyboard buffer"},
/* de */ {IDCLS_PUT_STRING_INTO_KEYBUF_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_PUT_STRING_INTO_KEYBUF_FR, ""},  /* fuzzy */
/* it */ {IDCLS_PUT_STRING_INTO_KEYBUF_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_PUT_STRING_INTO_KEYBUF_NL, "Stop de opgegeven string in de toetsenbord buffer"},
/* pl */ {IDCLS_PUT_STRING_INTO_KEYBUF_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PUT_STRING_INTO_KEYBUF_SV, ""},  /* fuzzy */

/* log.c */
/* en */ {IDCLS_SPECIFY_LOG_FILE_NAME,    "Specify log file name"},
/* de */ {IDCLS_SPECIFY_LOG_FILE_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_LOG_FILE_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_LOG_FILE_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_LOG_FILE_NAME_NL, "Geef de naam van het log bestand"},
/* pl */ {IDCLS_SPECIFY_LOG_FILE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_LOG_FILE_NAME_SV, ""},  /* fuzzy */

/* mouse.c */
/* en */ {IDCLS_ENABLE_1351_MOUSE,    "Enable emulation of the 1351 proportional mouse"},
/* de */ {IDCLS_ENABLE_1351_MOUSE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_1351_MOUSE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_1351_MOUSE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_1351_MOUSE_NL, "Aktiveer emulatie van de 1351 proportionele muis"},
/* pl */ {IDCLS_ENABLE_1351_MOUSE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_1351_MOUSE_SV, ""},  /* fuzzy */

/* mouse.c */
/* en */ {IDCLS_DISABLE_1351_MOUSE,    "Disable emulation of the 1351 proportional mouse"},
/* de */ {IDCLS_DISABLE_1351_MOUSE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_1351_MOUSE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_1351_MOUSE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_1351_MOUSE_NL, "Emulatie van de 1351 proportionele muis afsluiten"},
/* pl */ {IDCLS_DISABLE_1351_MOUSE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_1351_MOUSE_SV, ""},  /* fuzzy */

/* mouse.c */
/* en */ {IDCLS_SELECT_MOUSE_JOY_PORT,    "Select the joystick port the mouse is attached to"},
/* de */ {IDCLS_SELECT_MOUSE_JOY_PORT_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SELECT_MOUSE_JOY_PORT_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SELECT_MOUSE_JOY_PORT_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SELECT_MOUSE_JOY_PORT_NL, "Selecteer de joystick poort waar de muis aan gekoppelt is"},
/* pl */ {IDCLS_SELECT_MOUSE_JOY_PORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SELECT_MOUSE_JOY_PORT_SV, ""},  /* fuzzy */

/* ram.c */
/* en */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE,    "Set the value for the very first RAM address after powerup"},
/* de */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_NL, "Zet de waarde voor het allereerste RAM adres na koude start"},
/* pl */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_SV, ""},  /* fuzzy */

/* ram.c */
/* en */ {IDCLS_P_NUM_OF_BYTES,    "<num of bytes>"},
/* de */ {IDCLS_P_NUM_OF_BYTES_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_NUM_OF_BYTES_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_NUM_OF_BYTES_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_NUM_OF_BYTES_NL, "<aantal bytes>"},
/* pl */ {IDCLS_P_NUM_OF_BYTES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_NUM_OF_BYTES_SV, ""},  /* fuzzy */

/* ram.c */
/* en */ {IDCLS_LENGTH_BLOCK_SAME_VALUE,    "Length of memory block initialized with the same value"},
/* de */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_NL, "Geheugen blok grootte die dezelfde waarde krijgt bij initialisatie"},
/* pl */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_SV, ""},  /* fuzzy */

/* ram.c */
/* en */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN,    "Length of memory block initialized with the same pattern"},
/* de */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_FR, ""},  /* fuzzy */
/* it */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_NL, "Geheugen blok grootte met hetzelfde patroon bij initialisatie"},
/* pl */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_SV, ""},  /* fuzzy */

/* sound.c */
/* en */ {IDCLS_ENABLE_SOUND_PLAYBACK,    "Enable sound playback"},
/* de */ {IDCLS_ENABLE_SOUND_PLAYBACK_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_SOUND_PLAYBACK_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SOUND_PLAYBACK_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SOUND_PLAYBACK_NL, "Aktiveer geluid"},
/* pl */ {IDCLS_ENABLE_SOUND_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SOUND_PLAYBACK_SV, ""},  /* fuzzy */

/* sound.c */
/* en */ {IDCLS_DISABLE_SOUND_PLAYBACK,    "Disable sound playback"},
/* de */ {IDCLS_DISABLE_SOUND_PLAYBACK_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_SOUND_PLAYBACK_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_SOUND_PLAYBACK_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SOUND_PLAYBACK_NL, "Geluid afsluiten"},
/* pl */ {IDCLS_DISABLE_SOUND_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SOUND_PLAYBACK_SV, ""},  /* fuzzy */

/* sound.c */
/* en */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ,    "Set sound sample rate to <value> Hz"},
/* de */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_NL, "Zet de geluid sample rate naar <waarde> Hz"},
/* pl */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_SV, ""},  /* fuzzy */

/* sound.c */
/* en */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC,    "Set sound buffer size to <value> msec"},
/* de */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_NL, "Zet de geluid buffer grootte naar <waarde> msec"},
/* pl */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_SV, ""},  /* fuzzy */

/* sound.c */
/* en */ {IDCLS_SPECIFY_SOUND_DRIVER,    "Specify sound driver"},
/* de */ {IDCLS_SPECIFY_SOUND_DRIVER_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SOUND_DRIVER_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SOUND_DRIVER_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SOUND_DRIVER_NL, "Geef geluid stuurprogramma"},
/* pl */ {IDCLS_SPECIFY_SOUND_DRIVER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SOUND_DRIVER_SV, ""},  /* fuzzy */

/* sound.c */
/* en */ {IDCLS_P_ARGS,    "<args>"},
/* de */ {IDCLS_P_ARGS_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_ARGS_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_ARGS_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_ARGS_NL, "<parameters>"},
/* pl */ {IDCLS_P_ARGS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_ARGS_SV, ""},  /* fuzzy */

/* sound.c */
/* en */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM,    "Specify initialization parameters for sound driver"},
/* de */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_NL, "Geef initialisatie paremeters voor het geluid stuurprogramma"},
/* pl */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_SV, ""},  /* fuzzy */

/* sound.c */
/* en */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER,    "Specify recording sound driver"},
/* de */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_NL, "Geef geluid stuurprogramma voor opname"},
/* pl */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_SV, ""},  /* fuzzy */

/* sound.c */
/* en */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM,    "Specify initialization parameters for recording sound driver"},
/* de */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_NL, "Geef initialisatie paremeters voor het geluid stuurprogramma voor opname"},
/* pl */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_SV, ""},  /* fuzzy */

/* sound.c */
/* en */ {IDCLS_P_SYNC,    "<sync>"},
/* de */ {IDCLS_P_SYNC_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_SYNC_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_SYNC_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_SYNC_NL, "<sync>"},
/* pl */ {IDCLS_P_SYNC_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_SYNC_SV, ""},  /* fuzzy */

/* sound.c */
/* en */ {IDCLS_SET_SOUND_SPEED_ADJUST,    "Set sound speed adjustment (0: flexible, 1: adjusting, 2: exact)"},
/* de */ {IDCLS_SET_SOUND_SPEED_ADJUST_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_SOUND_SPEED_ADJUST_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SET_SOUND_SPEED_ADJUST_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_SOUND_SPEED_ADJUST_NL, "Zet geluid snelheid aanpassing (0: flexibel, 1: aanpassend, 2: exact)"},
/* pl */ {IDCLS_SET_SOUND_SPEED_ADJUST_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SOUND_SPEED_ADJUST_SV, ""},  /* fuzzy */

/* sysfile.c */
/* en */ {IDCLS_P_PATH,    "<path>"},
/* de */ {IDCLS_P_PATH_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_PATH_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_PATH_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_PATH_NL, "<pad>"},
/* pl */ {IDCLS_P_PATH_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_PATH_SV, ""},  /* fuzzy */

/* sysfile.c */
/* en */ {IDCLS_DEFINE_SYSTEM_FILES_PATH,    "Define search path to locate system files"},
/* de */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_NL, "Geef het zoek pad waar de systeem bestanden te vinden zijn"},
/* pl */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_SV, ""},  /* fuzzy */

/* traps.c */
/* en */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION,    "Enable general mechanisms for fast disk/tape emulation"},
/* de */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_NL, "Aktiveer algemene methoden voor snelle disk/tape emulatie"},
/* pl */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_SV, ""},  /* fuzzy */

/* traps.c */
/* en */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION,    "Disable general mechanisms for fast disk/tape emulation"},
/* de */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_NL, "Algemene methoden voor snelle disk/tape emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_SV, ""},  /* fuzzy */

/* vsync.c */
/* en */ {IDCLS_LIMIT_SPEED_TO_VALUE,    "Limit emulation speed to specified value"},
/* de */ {IDCLS_LIMIT_SPEED_TO_VALUE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_LIMIT_SPEED_TO_VALUE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_LIMIT_SPEED_TO_VALUE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_LIMIT_SPEED_TO_VALUE_NL, "Algemene methoden voor snelle disk/tape emulatie afsluiten"},
/* pl */ {IDCLS_LIMIT_SPEED_TO_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_LIMIT_SPEED_TO_VALUE_SV, ""},  /* fuzzy */

/* vsync.c */
/* en */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES,    "Update every <value> frames (`0' for automatic)"},
/* de */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_FR, ""},  /* fuzzy */
/* it */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_NL, "Scherm opbouw elke <waarde> frames (`0' voor automatisch)"},
/* pl */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_SV, ""},  /* fuzzy */

/* vsync.c */
/* en */ {IDCLS_ENABLE_WARP_MODE,    "Enable warp mode"},
/* de */ {IDCLS_ENABLE_WARP_MODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_WARP_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_WARP_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_WARP_MODE_NL, "Aktiveer warp modus"},
/* pl */ {IDCLS_ENABLE_WARP_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_WARP_MODE_SV, ""},  /* fuzzy */

/* vsync.c */
/* en */ {IDCLS_DISABLE_WARP_MODE,    "Disable warp mode"},
/* de */ {IDCLS_DISABLE_WARP_MODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_WARP_MODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_WARP_MODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_WARP_MODE_NL, "Warp modus afsluiten"},
/* pl */ {IDCLS_DISABLE_WARP_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_WARP_MODE_SV, ""},  /* fuzzy */

/* translate.c */
/* en */ {IDCLS_P_ISO_LANGUAGE_CODE,    "<iso language code>"},
/* de */ {IDCLS_P_ISO_LANGUAGE_CODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_P_ISO_LANGUAGE_CODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_P_ISO_LANGUAGE_CODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_P_ISO_LANGUAGE_CODE_NL, "<iso taal code>"},
/* pl */ {IDCLS_P_ISO_LANGUAGE_CODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_ISO_LANGUAGE_CODE_SV, ""},  /* fuzzy */

/* translate.c */
/* en */ {IDCLS_SPECIFY_ISO_LANG_CODE,    "Specify the iso code of the language"},
/* de */ {IDCLS_SPECIFY_ISO_LANG_CODE_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_ISO_LANG_CODE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_ISO_LANG_CODE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_ISO_LANG_CODE_NL, "Geef de iso code van de taal"},
/* pl */ {IDCLS_SPECIFY_ISO_LANG_CODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_ISO_LANG_CODE_SV, ""},  /* fuzzy */

/* c64/plus60k.c */
/* en */ {IDCLS_ENABLE_PLUS60K_EXPANSION,    "Enable the +60K RAM expansion"},
/* de */ {IDCLS_ENABLE_PLUS60K_EXPANSION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_PLUS60K_EXPANSION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_PLUS60K_EXPANSION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_PLUS60K_EXPANSION_NL, "Aktiveer de +60K geheugen uitbreiding"},
/* pl */ {IDCLS_ENABLE_PLUS60K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_PLUS60K_EXPANSION_SV, ""},  /* fuzzy */

/* c64/plus60k.c */
/* en */ {IDCLS_DISABLE_PLUS60K_EXPANSION,    "Disable the +60K RAM expansion"},
/* de */ {IDCLS_DISABLE_PLUS60K_EXPANSION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_PLUS60K_EXPANSION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_PLUS60K_EXPANSION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_PLUS60K_EXPANSION_NL, "De +60K geheugen uitbreiding afsluiten"},
/* pl */ {IDCLS_DISABLE_PLUS60K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_PLUS60K_EXPANSION_SV, ""},  /* fuzzy */

/* c64/plus60k.c */
/* en */ {IDCLS_SPECIFY_PLUS60K_NAME,    "Specify name of +60K image"},
/* de */ {IDCLS_SPECIFY_PLUS60K_NAME_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_PLUS60K_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PLUS60K_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PLUS60K_NAME_NL, "Geef de naam van het +60K bestand"},
/* pl */ {IDCLS_SPECIFY_PLUS60K_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PLUS60K_NAME_SV, ""}   /* fuzzy */

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
   c64/psid.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_USE_PAL_SYNC_FACTOR,
/* de */  IDCLS_USE_PAL_SYNC_FACTOR_DE,
/* fr */  IDCLS_USE_PAL_SYNC_FACTOR_FR,
/* it */  IDCLS_USE_PAL_SYNC_FACTOR_IT,
/* nl */  IDCLS_USE_PAL_SYNC_FACTOR_NL,
/* pl */  IDCLS_USE_PAL_SYNC_FACTOR_PL,
/* sv */  IDCLS_USE_PAL_SYNC_FACTOR_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
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
   plus4/plus4-cmdline-options.c, printerdrv/driver-select.c,
   printerdrv/output-select.c, printerdrv/output-text.c,
   rs232drv/rs232drv.c, vic20/vic20-cmdline-options.c,
   vic20/vic20cartridge.c, video/video-cmdline-options.c,
   fliplist.c, initcmdline.c, log.c, sound.c */
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
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_ENABLE_EMULATOR_ID,
/* de */  IDCLS_ENABLE_EMULATOR_ID_DE,
/* fr */  IDCLS_ENABLE_EMULATOR_ID_FR,
/* it */  IDCLS_ENABLE_EMULATOR_ID_IT,
/* nl */  IDCLS_ENABLE_EMULATOR_ID_NL,
/* pl */  IDCLS_ENABLE_EMULATOR_ID_PL,
/* sv */  IDCLS_ENABLE_EMULATOR_ID_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
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

/* c128/c128-cmdline-options.c, c64/c64-cmdline-optionc.c,
   c64/psid.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vdc/vdc-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_P_NUMBER,
/* de */  IDCLS_P_NUMBER_DE,
/* fr */  IDCLS_P_NUMBER_FR,
/* it */  IDCLS_P_NUMBER_IT,
/* nl */  IDCLS_P_NUMBER_NL,
/* pl */  IDCLS_P_NUMBER_PL,
/* sv */  IDCLS_P_NUMBER_SV},

#ifdef COMMON_KBD
/* c128/c128-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX,
/* de */  IDCLS_SPECIFY_KEYMAP_FILE_INDEX_DE,
/* fr */  IDCLS_SPECIFY_KEYMAP_FILE_INDEX_FR,
/* it */  IDCLS_SPECIFY_KEYMAP_FILE_INDEX_IT,
/* nl */  IDCLS_SPECIFY_KEYMAP_FILE_INDEX_NL,
/* pl */  IDCLS_SPECIFY_KEYMAP_FILE_INDEX_PL,
/* sv */  IDCLS_SPECIFY_KEYMAP_FILE_INDEX_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options,
   plus4/plus4-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME,
/* de */  IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_DE,
/* fr */  IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_FR,
/* it */  IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_IT,
/* nl */  IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_NL,
/* pl */  IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_PL,
/* sv */  IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   plus4/plus4-cmdline-options.c, vic20/vic20-cmdline-options.c */
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
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KERNAL_ROM_NAME,
/* de */  IDCLS_SPECIFY_KERNAL_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_KERNAL_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_KERNAL_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_KERNAL_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_KERNAL_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_KERNAL_ROM_NAME_SV},

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME,
/* de */  IDCLS_SPECIFY_BASIC_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_BASIC_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_BASIC_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_BASIC_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_BASIC_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_BASIC_ROM_NAME_SV},

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, vic20/vic20-cmdline-options.c */
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

/* c64/ide64.c, gfxoutputdrv/ffmpegdrv.c, datasette.c,
   debug.c, mouse.c, ram.c, sound.c, vsync.c */
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

/* drive/drive-cmdline-options.c, printerdrv/interface-serial.c,
   attach.c */
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

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_P_0_2000,
/* de */  IDCLS_P_0_2000_DE,
/* fr */  IDCLS_P_0_2000_FR,
/* it */  IDCLS_P_0_2000_IT,
/* nl */  IDCLS_P_0_2000_NL,
/* pl */  IDCLS_P_0_2000_PL,
/* sv */  IDCLS_P_0_2000_SV},

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_SATURATION,
/* de */  IDCLS_SET_SATURATION_DE,
/* fr */  IDCLS_SET_SATURATION_FR,
/* it */  IDCLS_SET_SATURATION_IT,
/* nl */  IDCLS_SET_SATURATION_NL,
/* pl */  IDCLS_SET_SATURATION_PL,
/* sv */  IDCLS_SET_SATURATION_SV},

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_CONTRAST,
/* de */  IDCLS_SET_CONTRAST_DE,
/* fr */  IDCLS_SET_CONTRAST_FR,
/* it */  IDCLS_SET_CONTRAST_IT,
/* nl */  IDCLS_SET_CONTRAST_NL,
/* pl */  IDCLS_SET_CONTRAST_PL,
/* sv */  IDCLS_SET_CONTRAST_SV},

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_BRIGHTNESS,
/* de */  IDCLS_SET_BRIGHTNESS_DE,
/* fr */  IDCLS_SET_BRIGHTNESS_FR,
/* it */  IDCLS_SET_BRIGHTNESS_IT,
/* nl */  IDCLS_SET_BRIGHTNESS_NL,
/* pl */  IDCLS_SET_BRIGHTNESS_PL,
/* sv */  IDCLS_SET_BRIGHTNESS_SV},

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_GAMMA,
/* de */  IDCLS_SET_GAMMA_DE,
/* fr */  IDCLS_SET_GAMMA_FR,
/* it */  IDCLS_SET_GAMMA_IT,
/* nl */  IDCLS_SET_GAMMA_NL,
/* pl */  IDCLS_SET_GAMMA_PL,
/* sv */  IDCLS_SET_GAMMA_SV},

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME,
/* de */  IDCLS_SPECIFY_PRT_DRIVER_4_NAME_DE,
/* fr */  IDCLS_SPECIFY_PRT_DRIVER_4_NAME_FR,
/* it */  IDCLS_SPECIFY_PRT_DRIVER_4_NAME_IT,
/* nl */  IDCLS_SPECIFY_PRT_DRIVER_4_NAME_NL,
/* pl */  IDCLS_SPECIFY_PRT_DRIVER_4_NAME_PL,
/* sv */  IDCLS_SPECIFY_PRT_DRIVER_4_NAME_SV},

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME,
/* de */  IDCLS_SPECIFY_PRT_DRIVER_5_NAME_DE,
/* fr */  IDCLS_SPECIFY_PRT_DRIVER_5_NAME_FR,
/* it */  IDCLS_SPECIFY_PRT_DRIVER_5_NAME_IT,
/* nl */  IDCLS_SPECIFY_PRT_DRIVER_5_NAME_NL,
/* pl */  IDCLS_SPECIFY_PRT_DRIVER_5_NAME_PL,
/* sv */  IDCLS_SPECIFY_PRT_DRIVER_5_NAME_SV},

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME,
/* de */  IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_DE,
/* fr */  IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_FR,
/* it */  IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_IT,
/* nl */  IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_NL,
/* pl */  IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_PL,
/* sv */  IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_SV},

/* printerdrv/interface-serial.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_4,
/* de */  IDCLS_SET_DEVICE_TYPE_4_DE,
/* fr */  IDCLS_SET_DEVICE_TYPE_4_FR,
/* it */  IDCLS_SET_DEVICE_TYPE_4_IT,
/* nl */  IDCLS_SET_DEVICE_TYPE_4_NL,
/* pl */  IDCLS_SET_DEVICE_TYPE_4_PL,
/* sv */  IDCLS_SET_DEVICE_TYPE_4_SV},

/* printerdrv/interface-serial.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_5,
/* de */  IDCLS_SET_DEVICE_TYPE_5_DE,
/* fr */  IDCLS_SET_DEVICE_TYPE_5_FR,
/* it */  IDCLS_SET_DEVICE_TYPE_5_IT,
/* nl */  IDCLS_SET_DEVICE_TYPE_5_NL,
/* pl */  IDCLS_SET_DEVICE_TYPE_5_PL,
/* sv */  IDCLS_SET_DEVICE_TYPE_5_SV},

/* printerdrv/interface-userport.c */
/* en */ {IDCLS_ENABLE_USERPORT_PRINTER,
/* de */  IDCLS_ENABLE_USERPORT_PRINTER_DE,
/* fr */  IDCLS_ENABLE_USERPORT_PRINTER_FR,
/* it */  IDCLS_ENABLE_USERPORT_PRINTER_IT,
/* nl */  IDCLS_ENABLE_USERPORT_PRINTER_NL,
/* pl */  IDCLS_ENABLE_USERPORT_PRINTER_PL,
/* sv */  IDCLS_ENABLE_USERPORT_PRINTER_SV},

/* printerdrv/interface-userport.c */
/* en */ {IDCLS_DISABLE_USERPORT_PRINTER,
/* de */  IDCLS_DISABLE_USERPORT_PRINTER_DE,
/* fr */  IDCLS_DISABLE_USERPORT_PRINTER_FR,
/* it */  IDCLS_DISABLE_USERPORT_PRINTER_IT,
/* nl */  IDCLS_DISABLE_USERPORT_PRINTER_NL,
/* pl */  IDCLS_DISABLE_USERPORT_PRINTER_PL,
/* sv */  IDCLS_DISABLE_USERPORT_PRINTER_SV},

/* printerdrv/output-select.c */
/* en */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME,
/* de */  IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_DE,
/* fr */  IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_FR,
/* it */  IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_IT,
/* nl */  IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_NL,
/* pl */  IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_PL,
/* sv */  IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_SV},

/* printerdrv/output-select.c */
/* en */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME,
/* de */  IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_DE,
/* fr */  IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_FR,
/* it */  IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_IT,
/* nl */  IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_NL,
/* pl */  IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_PL,
/* sv */  IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_SV},

/* printerdrv/output-select.c */
/* en */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME,
/* de */  IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_DE,
/* fr */  IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_FR,
/* it */  IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_IT,
/* nl */  IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_NL,
/* pl */  IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_PL,
/* sv */  IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_SV},

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME,
/* de */  IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_DE,
/* fr */  IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_FR,
/* it */  IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_IT,
/* nl */  IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_NL,
/* pl */  IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_PL,
/* sv */  IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_SV},

/* printerdrv/output-text.c */
/* en */ {IDCLS_P_0_2,
/* de */  IDCLS_P_0_2_DE,
/* fr */  IDCLS_P_0_2_FR,
/* it */  IDCLS_P_0_2_IT,
/* nl */  IDCLS_P_0_2_NL,
/* pl */  IDCLS_P_0_2_PL,
/* sv */  IDCLS_P_0_2_SV},

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_4,
/* de */  IDCLS_SPECIFY_TEXT_DEVICE_4_DE,
/* fr */  IDCLS_SPECIFY_TEXT_DEVICE_4_FR,
/* it */  IDCLS_SPECIFY_TEXT_DEVICE_4_IT,
/* nl */  IDCLS_SPECIFY_TEXT_DEVICE_4_NL,
/* pl */  IDCLS_SPECIFY_TEXT_DEVICE_4_PL,
/* sv */  IDCLS_SPECIFY_TEXT_DEVICE_4_SV},

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_5,
/* de */  IDCLS_SPECIFY_TEXT_DEVICE_5_DE,
/* fr */  IDCLS_SPECIFY_TEXT_DEVICE_5_FR,
/* it */  IDCLS_SPECIFY_TEXT_DEVICE_5_IT,
/* nl */  IDCLS_SPECIFY_TEXT_DEVICE_5_NL,
/* pl */  IDCLS_SPECIFY_TEXT_DEVICE_5_PL,
/* sv */  IDCLS_SPECIFY_TEXT_DEVICE_5_SV},

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_USERPORT,
/* de */  IDCLS_SPECIFY_TEXT_USERPORT_DE,
/* fr */  IDCLS_SPECIFY_TEXT_USERPORT_FR,
/* it */  IDCLS_SPECIFY_TEXT_USERPORT_IT,
/* nl */  IDCLS_SPECIFY_TEXT_USERPORT_NL,
/* pl */  IDCLS_SPECIFY_TEXT_USERPORT_PL,
/* sv */  IDCLS_SPECIFY_TEXT_USERPORT_SV},

/* raster/raster-cmdline-options.c */
/* en */ {IDCLS_ENABLE_VIDEO_CACHE,
/* de */  IDCLS_ENABLE_VIDEO_CACHE_DE,
/* fr */  IDCLS_ENABLE_VIDEO_CACHE_FR,
/* it */  IDCLS_ENABLE_VIDEO_CACHE_IT,
/* nl */  IDCLS_ENABLE_VIDEO_CACHE_NL,
/* pl */  IDCLS_ENABLE_VIDEO_CACHE_PL,
/* sv */  IDCLS_ENABLE_VIDEO_CACHE_SV},

/* raster/raster-cmdline-options.c */
/* en */ {IDCLS_DISABLE_VIDEO_CACHE,
/* de */  IDCLS_DISABLE_VIDEO_CACHE_DE,
/* fr */  IDCLS_DISABLE_VIDEO_CACHE_FR,
/* it */  IDCLS_DISABLE_VIDEO_CACHE_IT,
/* nl */  IDCLS_DISABLE_VIDEO_CACHE_NL,
/* pl */  IDCLS_DISABLE_VIDEO_CACHE_PL,
/* sv */  IDCLS_DISABLE_VIDEO_CACHE_SV},

#ifdef HAVE_RS232
/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_1_NAME,
/* de */  IDCLS_SPECIFY_RS232_1_NAME_DE,
/* fr */  IDCLS_SPECIFY_RS232_1_NAME_FR,
/* it */  IDCLS_SPECIFY_RS232_1_NAME_IT,
/* nl */  IDCLS_SPECIFY_RS232_1_NAME_NL,
/* pl */  IDCLS_SPECIFY_RS232_1_NAME_PL,
/* sv */  IDCLS_SPECIFY_RS232_1_NAME_SV},

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_2_NAME,
/* de */  IDCLS_SPECIFY_RS232_2_NAME_DE,
/* fr */  IDCLS_SPECIFY_RS232_2_NAME_FR,
/* it */  IDCLS_SPECIFY_RS232_2_NAME_IT,
/* nl */  IDCLS_SPECIFY_RS232_2_NAME_NL,
/* pl */  IDCLS_SPECIFY_RS232_2_NAME_PL,
/* sv */  IDCLS_SPECIFY_RS232_2_NAME_SV},

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_3_NAME,
/* de */  IDCLS_SPECIFY_RS232_3_NAME_DE,
/* fr */  IDCLS_SPECIFY_RS232_3_NAME_FR,
/* it */  IDCLS_SPECIFY_RS232_3_NAME_IT,
/* nl */  IDCLS_SPECIFY_RS232_3_NAME_NL,
/* pl */  IDCLS_SPECIFY_RS232_3_NAME_PL,
/* sv */  IDCLS_SPECIFY_RS232_3_NAME_SV},

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_4_NAME,
/* de */  IDCLS_SPECIFY_RS232_4_NAME_DE,
/* fr */  IDCLS_SPECIFY_RS232_4_NAME_FR,
/* it */  IDCLS_SPECIFY_RS232_4_NAME_IT,
/* nl */  IDCLS_SPECIFY_RS232_4_NAME_NL,
/* pl */  IDCLS_SPECIFY_RS232_4_NAME_PL,
/* sv */  IDCLS_SPECIFY_RS232_4_NAME_SV},
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_ENABLE_RS232_USERPORT,
/* de */  IDCLS_ENABLE_RS232_USERPORT_DE,
/* fr */  IDCLS_ENABLE_RS232_USERPORT_FR,
/* it */  IDCLS_ENABLE_RS232_USERPORT_IT,
/* nl */  IDCLS_ENABLE_RS232_USERPORT_NL,
/* pl */  IDCLS_ENABLE_RS232_USERPORT_PL,
/* sv */  IDCLS_ENABLE_RS232_USERPORT_SV},

/* rs232drv/rsuser.c */
/* en */ {IDCLS_DISABLE_RS232_USERPORT,
/* de */  IDCLS_DISABLE_RS232_USERPORT_DE,
/* fr */  IDCLS_DISABLE_RS232_USERPORT_FR,
/* it */  IDCLS_DISABLE_RS232_USERPORT_IT,
/* nl */  IDCLS_DISABLE_RS232_USERPORT_NL,
/* pl */  IDCLS_DISABLE_RS232_USERPORT_PL,
/* sv */  IDCLS_DISABLE_RS232_USERPORT_SV},

/* rs232drv/rsuser.c */
/* en */ {IDCLS_P_BAUD,
/* de */  IDCLS_P_BAUD_DE,
/* fr */  IDCLS_P_BAUD_FR,
/* it */  IDCLS_P_BAUD_IT,
/* nl */  IDCLS_P_BAUD_NL,
/* pl */  IDCLS_P_BAUD_PL,
/* sv */  IDCLS_P_BAUD_SV},

/* rs232drv/rsuser.c */
/* en */ {IDCLS_SET_BAUD_RS232_USERPORT,
/* de */  IDCLS_SET_BAUD_RS232_USERPORT_DE,
/* fr */  IDCLS_SET_BAUD_RS232_USERPORT_FR,
/* it */  IDCLS_SET_BAUD_RS232_USERPORT_IT,
/* nl */  IDCLS_SET_BAUD_RS232_USERPORT_NL,
/* pl */  IDCLS_SET_BAUD_RS232_USERPORT_PL,
/* sv */  IDCLS_SET_BAUD_RS232_USERPORT_SV},

/* rs232drv/rsuser.c, aciacore.c */
/* en */ {IDCLS_P_0_3,
/* de */  IDCLS_P_0_3_DE,
/* fr */  IDCLS_P_0_3_FR,
/* it */  IDCLS_P_0_3_IT,
/* nl */  IDCLS_P_0_3_NL,
/* pl */  IDCLS_P_0_3_PL,
/* sv */  IDCLS_P_0_3_SV},

/* rs232drv/rsuser.c */
/* en */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT,
/* de */  IDCLS_SPECIFY_RS232_DEVICE_USERPORT_DE,
/* fr */  IDCLS_SPECIFY_RS232_DEVICE_USERPORT_FR,
/* it */  IDCLS_SPECIFY_RS232_DEVICE_USERPORT_IT,
/* nl */  IDCLS_SPECIFY_RS232_DEVICE_USERPORT_NL,
/* pl */  IDCLS_SPECIFY_RS232_DEVICE_USERPORT_PL,
/* sv */  IDCLS_SPECIFY_RS232_DEVICE_USERPORT_SV},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_4,
/* de */  IDCLS_ENABLE_IEC_4_DE,
/* fr */  IDCLS_ENABLE_IEC_4_FR,
/* it */  IDCLS_ENABLE_IEC_4_IT,
/* nl */  IDCLS_ENABLE_IEC_4_NL,
/* pl */  IDCLS_ENABLE_IEC_4_PL,
/* sv */  IDCLS_ENABLE_IEC_4_SV},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_4,
/* de */  IDCLS_DISABLE_IEC_4_DE,
/* fr */  IDCLS_DISABLE_IEC_4_FR,
/* it */  IDCLS_DISABLE_IEC_4_IT,
/* nl */  IDCLS_DISABLE_IEC_4_NL,
/* pl */  IDCLS_DISABLE_IEC_4_PL,
/* sv */  IDCLS_DISABLE_IEC_4_SV},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_5,
/* de */  IDCLS_ENABLE_IEC_5_DE,
/* fr */  IDCLS_ENABLE_IEC_5_FR,
/* it */  IDCLS_ENABLE_IEC_5_IT,
/* nl */  IDCLS_ENABLE_IEC_5_NL,
/* pl */  IDCLS_ENABLE_IEC_5_PL,
/* sv */  IDCLS_ENABLE_IEC_5_SV},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_5,
/* de */  IDCLS_DISABLE_IEC_5_DE,
/* fr */  IDCLS_DISABLE_IEC_5_FR,
/* it */  IDCLS_DISABLE_IEC_5_IT,
/* nl */  IDCLS_DISABLE_IEC_5_NL,
/* pl */  IDCLS_DISABLE_IEC_5_PL,
/* sv */  IDCLS_DISABLE_IEC_5_SV},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_8,
/* de */  IDCLS_ENABLE_IEC_8_DE,
/* fr */  IDCLS_ENABLE_IEC_8_FR,
/* it */  IDCLS_ENABLE_IEC_8_IT,
/* nl */  IDCLS_ENABLE_IEC_8_NL,
/* pl */  IDCLS_ENABLE_IEC_8_PL,
/* sv */  IDCLS_ENABLE_IEC_8_SV},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_8,
/* de */  IDCLS_DISABLE_IEC_8_DE,
/* fr */  IDCLS_DISABLE_IEC_8_FR,
/* it */  IDCLS_DISABLE_IEC_8_IT,
/* nl */  IDCLS_DISABLE_IEC_8_NL,
/* pl */  IDCLS_DISABLE_IEC_8_PL,
/* sv */  IDCLS_DISABLE_IEC_8_SV},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_9,
/* de */  IDCLS_ENABLE_IEC_9_DE,
/* fr */  IDCLS_ENABLE_IEC_9_FR,
/* it */  IDCLS_ENABLE_IEC_9_IT,
/* nl */  IDCLS_ENABLE_IEC_9_NL,
/* pl */  IDCLS_ENABLE_IEC_9_PL,
/* sv */  IDCLS_ENABLE_IEC_9_SV},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_9,
/* de */  IDCLS_DISABLE_IEC_9_DE,
/* fr */  IDCLS_DISABLE_IEC_9_FR,
/* it */  IDCLS_DISABLE_IEC_9_IT,
/* nl */  IDCLS_DISABLE_IEC_9_NL,
/* pl */  IDCLS_DISABLE_IEC_9_PL,
/* sv */  IDCLS_DISABLE_IEC_9_SV},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_10,
/* de */  IDCLS_ENABLE_IEC_10_DE,
/* fr */  IDCLS_ENABLE_IEC_10_FR,
/* it */  IDCLS_ENABLE_IEC_10_IT,
/* nl */  IDCLS_ENABLE_IEC_10_NL,
/* pl */  IDCLS_ENABLE_IEC_10_PL,
/* sv */  IDCLS_ENABLE_IEC_10_SV},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_10,
/* de */  IDCLS_DISABLE_IEC_10_DE,
/* fr */  IDCLS_DISABLE_IEC_10_FR,
/* it */  IDCLS_DISABLE_IEC_10_IT,
/* nl */  IDCLS_DISABLE_IEC_10_NL,
/* pl */  IDCLS_DISABLE_IEC_10_PL,
/* sv */  IDCLS_DISABLE_IEC_10_SV},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_11,
/* de */  IDCLS_ENABLE_IEC_11_DE,
/* fr */  IDCLS_ENABLE_IEC_11_FR,
/* it */  IDCLS_ENABLE_IEC_11_IT,
/* nl */  IDCLS_ENABLE_IEC_11_NL,
/* pl */  IDCLS_ENABLE_IEC_11_PL,
/* sv */  IDCLS_ENABLE_IEC_11_SV},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_11,
/* de */  IDCLS_DISABLE_IEC_11_DE,
/* fr */  IDCLS_DISABLE_IEC_11_FR,
/* it */  IDCLS_DISABLE_IEC_11_IT,
/* nl */  IDCLS_DISABLE_IEC_11_NL,
/* pl */  IDCLS_DISABLE_IEC_11_PL,
/* sv */  IDCLS_DISABLE_IEC_11_SV},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_P_ENGINE,
/* de */  IDCLS_P_ENGINE_DE,
/* fr */  IDCLS_P_ENGINE_FR,
/* it */  IDCLS_P_ENGINE_IT,
/* nl */  IDCLS_P_ENGINE_NL,
/* pl */  IDCLS_P_ENGINE_PL,
/* sv */  IDCLS_P_ENGINE_SV},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,
/* de */  IDCLS_SPECIFY_SID_ENGINE_DE,
/* fr */  IDCLS_SPECIFY_SID_ENGINE_FR,
/* it */  IDCLS_SPECIFY_SID_ENGINE_IT,
/* nl */  IDCLS_SPECIFY_SID_ENGINE_NL,
/* pl */  IDCLS_SPECIFY_SID_ENGINE_PL,
/* sv */  IDCLS_SPECIFY_SID_ENGINE_SV},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SECOND_SID,
/* de */  IDCLS_ENABLE_SECOND_SID_DE,
/* fr */  IDCLS_ENABLE_SECOND_SID_FR,
/* it */  IDCLS_ENABLE_SECOND_SID_IT,
/* nl */  IDCLS_ENABLE_SECOND_SID_NL,
/* pl */  IDCLS_ENABLE_SECOND_SID_PL,
/* sv */  IDCLS_ENABLE_SECOND_SID_SV},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_P_BASE_ADDRESS,
/* de */  IDCLS_P_BASE_ADDRESS_DE,
/* fr */  IDCLS_P_BASE_ADDRESS_FR,
/* it */  IDCLS_P_BASE_ADDRESS_IT,
/* nl */  IDCLS_P_BASE_ADDRESS_NL,
/* pl */  IDCLS_P_BASE_ADDRESS_PL,
/* sv */  IDCLS_P_BASE_ADDRESS_SV},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_2_ADDRESS,
/* de */  IDCLS_SPECIFY_SID_2_ADDRESS_DE,
/* fr */  IDCLS_SPECIFY_SID_2_ADDRESS_FR,
/* it */  IDCLS_SPECIFY_SID_2_ADDRESS_IT,
/* nl */  IDCLS_SPECIFY_SID_2_ADDRESS_NL,
/* pl */  IDCLS_SPECIFY_SID_2_ADDRESS_PL,
/* sv */  IDCLS_SPECIFY_SID_2_ADDRESS_SV},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_P_MODEL,
/* de */  IDCLS_P_MODEL_DE,
/* fr */  IDCLS_P_MODEL_FR,
/* it */  IDCLS_P_MODEL_IT,
/* nl */  IDCLS_P_MODEL_NL,
/* pl */  IDCLS_P_MODEL_PL,
/* sv */  IDCLS_P_MODEL_SV},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_MODEL,
/* de */  IDCLS_SPECIFY_SID_MODEL_DE,
/* fr */  IDCLS_SPECIFY_SID_MODEL_FR,
/* it */  IDCLS_SPECIFY_SID_MODEL_IT,
/* nl */  IDCLS_SPECIFY_SID_MODEL_NL,
/* pl */  IDCLS_SPECIFY_SID_MODEL_PL,
/* sv */  IDCLS_SPECIFY_SID_MODEL_SV},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SID_FILTERS,
/* de */  IDCLS_ENABLE_SID_FILTERS_DE,
/* fr */  IDCLS_ENABLE_SID_FILTERS_FR,
/* it */  IDCLS_ENABLE_SID_FILTERS_IT,
/* nl */  IDCLS_ENABLE_SID_FILTERS_NL,
/* pl */  IDCLS_ENABLE_SID_FILTERS_PL,
/* sv */  IDCLS_ENABLE_SID_FILTERS_SV},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SID_FILTERS,
/* de */  IDCLS_DISABLE_SID_FILTERS_DE,
/* fr */  IDCLS_DISABLE_SID_FILTERS_FR,
/* it */  IDCLS_DISABLE_SID_FILTERS_IT,
/* nl */  IDCLS_DISABLE_SID_FILTERS_NL,
/* pl */  IDCLS_DISABLE_SID_FILTERS_PL,
/* sv */  IDCLS_DISABLE_SID_FILTERS_SV},

#ifdef HAVE_RESID
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_RESID_SAMPLING_METHOD,
/* de */  IDCLS_RESID_SAMPLING_METHOD_DE,
/* fr */  IDCLS_RESID_SAMPLING_METHOD_FR,
/* it */  IDCLS_RESID_SAMPLING_METHOD_IT,
/* nl */  IDCLS_RESID_SAMPLING_METHOD_NL,
/* pl */  IDCLS_RESID_SAMPLING_METHOD_PL,
/* sv */  IDCLS_RESID_SAMPLING_METHOD_SV},

/* sid/sid-cmdline-options.c, vsync.c */
/* en */ {IDCLS_P_PERCENT,
/* de */  IDCLS_P_PERCENT_DE,
/* fr */  IDCLS_P_PERCENT_FR,
/* it */  IDCLS_P_PERCENT_IT,
/* nl */  IDCLS_P_PERCENT_NL,
/* pl */  IDCLS_P_PERCENT_PL,
/* sv */  IDCLS_P_PERCENT_SV},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_PASSBAND_PERCENTAGE,
/* de */  IDCLS_PASSBAND_PERCENTAGE_DE,
/* fr */  IDCLS_PASSBAND_PERCENTAGE_FR,
/* it */  IDCLS_PASSBAND_PERCENTAGE_IT,
/* nl */  IDCLS_PASSBAND_PERCENTAGE_NL,
/* pl */  IDCLS_PASSBAND_PERCENTAGE_PL,
/* sv */  IDCLS_PASSBAND_PERCENTAGE_SV},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_RESID_GAIN_PERCENTAGE,
/* de */  IDCLS_RESID_GAIN_PERCENTAGE_DE,
/* fr */  IDCLS_RESID_GAIN_PERCENTAGE_FR,
/* it */  IDCLS_RESID_GAIN_PERCENTAGE_IT,
/* nl */  IDCLS_RESID_GAIN_PERCENTAGE_NL,
/* pl */  IDCLS_RESID_GAIN_PERCENTAGE_PL,
/* sv */  IDCLS_RESID_GAIN_PERCENTAGE_SV},
#endif

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_MEMORY_16KB,
/* de */  IDCLS_SET_VDC_MEMORY_16KB_DE,
/* fr */  IDCLS_SET_VDC_MEMORY_16KB_FR,
/* it */  IDCLS_SET_VDC_MEMORY_16KB_IT,
/* nl */  IDCLS_SET_VDC_MEMORY_16KB_NL,
/* pl */  IDCLS_SET_VDC_MEMORY_16KB_PL,
/* sv */  IDCLS_SET_VDC_MEMORY_16KB_SV},

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_MEMORY_64KB,
/* de */  IDCLS_SET_VDC_MEMORY_64KB_DE,
/* fr */  IDCLS_SET_VDC_MEMORY_64KB_FR,
/* it */  IDCLS_SET_VDC_MEMORY_64KB_IT,
/* nl */  IDCLS_SET_VDC_MEMORY_64KB_NL,
/* pl */  IDCLS_SET_VDC_MEMORY_64KB_PL,
/* sv */  IDCLS_SET_VDC_MEMORY_64KB_SV},

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_REVISION,
/* de */  IDCLS_SET_VDC_REVISION_DE,
/* fr */  IDCLS_SET_VDC_REVISION_FR,
/* it */  IDCLS_SET_VDC_REVISION_IT,
/* nl */  IDCLS_SET_VDC_REVISION_NL,
/* pl */  IDCLS_SET_VDC_REVISION_PL,
/* sv */  IDCLS_SET_VDC_REVISION_SV},

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_P_SPEC,
/* de */  IDCLS_P_SPEC_DE,
/* fr */  IDCLS_P_SPEC_FR,
/* it */  IDCLS_P_SPEC_IT,
/* nl */  IDCLS_P_SPEC_NL,
/* pl */  IDCLS_P_SPEC_PL,
/* sv */  IDCLS_P_SPEC_SV},

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_MEMORY_CONFIG,
/* de */  IDCLS_SPECIFY_MEMORY_CONFIG_DE,
/* fr */  IDCLS_SPECIFY_MEMORY_CONFIG_FR,
/* it */  IDCLS_SPECIFY_MEMORY_CONFIG_IT,
/* nl */  IDCLS_SPECIFY_MEMORY_CONFIG_NL,
/* pl */  IDCLS_SPECIFY_MEMORY_CONFIG_PL,
/* sv */  IDCLS_SPECIFY_MEMORY_CONFIG_SV},

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_ENABLE_VIC1112_IEEE488,
/* de */  IDCLS_ENABLE_VIC1112_IEEE488_DE,
/* fr */  IDCLS_ENABLE_VIC1112_IEEE488_FR,
/* it */  IDCLS_ENABLE_VIC1112_IEEE488_IT,
/* nl */  IDCLS_ENABLE_VIC1112_IEEE488_NL,
/* pl */  IDCLS_ENABLE_VIC1112_IEEE488_PL,
/* sv */  IDCLS_ENABLE_VIC1112_IEEE488_SV},

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_DISABLE_VIC1112_IEEE488,
/* de */  IDCLS_DISABLE_VIC1112_IEEE488_DE,
/* fr */  IDCLS_DISABLE_VIC1112_IEEE488_FR,
/* it */  IDCLS_DISABLE_VIC1112_IEEE488_IT,
/* nl */  IDCLS_DISABLE_VIC1112_IEEE488_NL,
/* pl */  IDCLS_DISABLE_VIC1112_IEEE488_PL,
/* sv */  IDCLS_DISABLE_VIC1112_IEEE488_SV},

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME,
/* de */  IDCLS_SPECIFY_EXT_ROM_2000_NAME_DE,
/* fr */  IDCLS_SPECIFY_EXT_ROM_2000_NAME_FR,
/* it */  IDCLS_SPECIFY_EXT_ROM_2000_NAME_IT,
/* nl */  IDCLS_SPECIFY_EXT_ROM_2000_NAME_NL,
/* pl */  IDCLS_SPECIFY_EXT_ROM_2000_NAME_PL,
/* sv */  IDCLS_SPECIFY_EXT_ROM_2000_NAME_SV},

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME,
/* de */  IDCLS_SPECIFY_EXT_ROM_4000_NAME_DE,
/* fr */  IDCLS_SPECIFY_EXT_ROM_4000_NAME_FR,
/* it */  IDCLS_SPECIFY_EXT_ROM_4000_NAME_IT,
/* nl */  IDCLS_SPECIFY_EXT_ROM_4000_NAME_NL,
/* pl */  IDCLS_SPECIFY_EXT_ROM_4000_NAME_PL,
/* sv */  IDCLS_SPECIFY_EXT_ROM_4000_NAME_SV},

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME,
/* de */  IDCLS_SPECIFY_EXT_ROM_6000_NAME_DE,
/* fr */  IDCLS_SPECIFY_EXT_ROM_6000_NAME_FR,
/* it */  IDCLS_SPECIFY_EXT_ROM_6000_NAME_IT,
/* nl */  IDCLS_SPECIFY_EXT_ROM_6000_NAME_NL,
/* pl */  IDCLS_SPECIFY_EXT_ROM_6000_NAME_PL,
/* sv */  IDCLS_SPECIFY_EXT_ROM_6000_NAME_SV},

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME,
/* de */  IDCLS_SPECIFY_EXT_ROM_A000_NAME_DE,
/* fr */  IDCLS_SPECIFY_EXT_ROM_A000_NAME_FR,
/* it */  IDCLS_SPECIFY_EXT_ROM_A000_NAME_IT,
/* nl */  IDCLS_SPECIFY_EXT_ROM_A000_NAME_NL,
/* pl */  IDCLS_SPECIFY_EXT_ROM_A000_NAME_PL,
/* sv */  IDCLS_SPECIFY_EXT_ROM_A000_NAME_SV},

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME,
/* de */  IDCLS_SPECIFY_EXT_ROM_B000_NAME_DE,
/* fr */  IDCLS_SPECIFY_EXT_ROM_B000_NAME_FR,
/* it */  IDCLS_SPECIFY_EXT_ROM_B000_NAME_IT,
/* nl */  IDCLS_SPECIFY_EXT_ROM_B000_NAME_NL,
/* pl */  IDCLS_SPECIFY_EXT_ROM_B000_NAME_PL,
/* sv */  IDCLS_SPECIFY_EXT_ROM_B000_NAME_SV},

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SPRITE_BACKGROUND,
/* de */  IDCLS_ENABLE_SPRITE_BACKGROUND_DE,
/* fr */  IDCLS_ENABLE_SPRITE_BACKGROUND_FR,
/* it */  IDCLS_ENABLE_SPRITE_BACKGROUND_IT,
/* nl */  IDCLS_ENABLE_SPRITE_BACKGROUND_NL,
/* pl */  IDCLS_ENABLE_SPRITE_BACKGROUND_PL,
/* sv */  IDCLS_ENABLE_SPRITE_BACKGROUND_SV},

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SPRITE_BACKGROUND,
/* de */  IDCLS_DISABLE_SPRITE_BACKGROUND_DE,
/* fr */  IDCLS_DISABLE_SPRITE_BACKGROUND_FR,
/* it */  IDCLS_DISABLE_SPRITE_BACKGROUND_IT,
/* nl */  IDCLS_DISABLE_SPRITE_BACKGROUND_NL,
/* pl */  IDCLS_DISABLE_SPRITE_BACKGROUND_PL,
/* sv */  IDCLS_DISABLE_SPRITE_BACKGROUND_SV},

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SPRITE_SPRITE,
/* de */  IDCLS_ENABLE_SPRITE_SPRITE_DE,
/* fr */  IDCLS_ENABLE_SPRITE_SPRITE_FR,
/* it */  IDCLS_ENABLE_SPRITE_SPRITE_IT,
/* nl */  IDCLS_ENABLE_SPRITE_SPRITE_NL,
/* pl */  IDCLS_ENABLE_SPRITE_SPRITE_PL,
/* sv */  IDCLS_ENABLE_SPRITE_SPRITE_SV},

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SPRITE_SPRITE,
/* de */  IDCLS_DISABLE_SPRITE_SPRITE_DE,
/* fr */  IDCLS_DISABLE_SPRITE_SPRITE_FR,
/* it */  IDCLS_DISABLE_SPRITE_SPRITE_IT,
/* nl */  IDCLS_DISABLE_SPRITE_SPRITE_NL,
/* pl */  IDCLS_DISABLE_SPRITE_SPRITE_PL,
/* sv */  IDCLS_DISABLE_SPRITE_SPRITE_SV},

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_USE_NEW_LUMINANCES,
/* de */  IDCLS_USE_NEW_LUMINANCES_DE,
/* fr */  IDCLS_USE_NEW_LUMINANCES_FR,
/* it */  IDCLS_USE_NEW_LUMINANCES_IT,
/* nl */  IDCLS_USE_NEW_LUMINANCES_NL,
/* pl */  IDCLS_USE_NEW_LUMINANCES_PL,
/* sv */  IDCLS_USE_NEW_LUMINANCES_SV},

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_USE_OLD_LUMINANCES,
/* de */  IDCLS_USE_OLD_LUMINANCES_DE,
/* fr */  IDCLS_USE_OLD_LUMINANCES_FR,
/* it */  IDCLS_USE_OLD_LUMINANCES_IT,
/* nl */  IDCLS_USE_OLD_LUMINANCES_NL,
/* pl */  IDCLS_USE_OLD_LUMINANCES_PL,
/* sv */  IDCLS_USE_OLD_LUMINANCES_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SIZE,
/* de */  IDCLS_ENABLE_DOUBLE_SIZE_DE,
/* fr */  IDCLS_ENABLE_DOUBLE_SIZE_FR,
/* it */  IDCLS_ENABLE_DOUBLE_SIZE_IT,
/* nl */  IDCLS_ENABLE_DOUBLE_SIZE_NL,
/* pl */  IDCLS_ENABLE_DOUBLE_SIZE_PL,
/* sv */  IDCLS_ENABLE_DOUBLE_SIZE_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SIZE,
/* de */  IDCLS_DISABLE_DOUBLE_SIZE_DE,
/* fr */  IDCLS_DISABLE_DOUBLE_SIZE_FR,
/* it */  IDCLS_DISABLE_DOUBLE_SIZE_IT,
/* nl */  IDCLS_DISABLE_DOUBLE_SIZE_NL,
/* pl */  IDCLS_DISABLE_DOUBLE_SIZE_PL,
/* sv */  IDCLS_DISABLE_DOUBLE_SIZE_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SCAN,
/* de */  IDCLS_ENABLE_DOUBLE_SCAN_DE,
/* fr */  IDCLS_ENABLE_DOUBLE_SCAN_FR,
/* it */  IDCLS_ENABLE_DOUBLE_SCAN_IT,
/* nl */  IDCLS_ENABLE_DOUBLE_SCAN_NL,
/* pl */  IDCLS_ENABLE_DOUBLE_SCAN_PL,
/* sv */  IDCLS_ENABLE_DOUBLE_SCAN_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SCAN,
/* de */  IDCLS_DISABLE_DOUBLE_SCAN_DE,
/* fr */  IDCLS_DISABLE_DOUBLE_SCAN_FR,
/* it */  IDCLS_DISABLE_DOUBLE_SCAN_IT,
/* nl */  IDCLS_DISABLE_DOUBLE_SCAN_NL,
/* pl */  IDCLS_DISABLE_DOUBLE_SCAN_PL,
/* sv */  IDCLS_DISABLE_DOUBLE_SCAN_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_HARDWARE_SCALING,
/* de */  IDCLS_ENABLE_HARDWARE_SCALING_DE,
/* fr */  IDCLS_ENABLE_HARDWARE_SCALING_FR,
/* it */  IDCLS_ENABLE_HARDWARE_SCALING_IT,
/* nl */  IDCLS_ENABLE_HARDWARE_SCALING_NL,
/* pl */  IDCLS_ENABLE_HARDWARE_SCALING_PL,
/* sv */  IDCLS_ENABLE_HARDWARE_SCALING_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_HARDWARE_SCALING,
/* de */  IDCLS_DISABLE_HARDWARE_SCALING_DE,
/* fr */  IDCLS_DISABLE_HARDWARE_SCALING_FR,
/* it */  IDCLS_DISABLE_HARDWARE_SCALING_IT,
/* nl */  IDCLS_DISABLE_HARDWARE_SCALING_NL,
/* pl */  IDCLS_DISABLE_HARDWARE_SCALING_PL,
/* sv */  IDCLS_DISABLE_HARDWARE_SCALING_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SCALE2X,
/* de */  IDCLS_ENABLE_SCALE2X_DE,
/* fr */  IDCLS_ENABLE_SCALE2X_FR,
/* it */  IDCLS_ENABLE_SCALE2X_IT,
/* nl */  IDCLS_ENABLE_SCALE2X_NL,
/* pl */  IDCLS_ENABLE_SCALE2X_PL,
/* sv */  IDCLS_ENABLE_SCALE2X_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SCALE2X,
/* de */  IDCLS_DISABLE_SCALE2X_DE,
/* fr */  IDCLS_DISABLE_SCALE2X_FR,
/* it */  IDCLS_DISABLE_SCALE2X_IT,
/* nl */  IDCLS_DISABLE_SCALE2X_NL,
/* pl */  IDCLS_DISABLE_SCALE2X_PL,
/* sv */  IDCLS_DISABLE_SCALE2X_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_USE_INTERNAL_CALC_PALETTE,
/* de */  IDCLS_USE_INTERNAL_CALC_PALETTE_DE,
/* fr */  IDCLS_USE_INTERNAL_CALC_PALETTE_FR,
/* it */  IDCLS_USE_INTERNAL_CALC_PALETTE_IT,
/* nl */  IDCLS_USE_INTERNAL_CALC_PALETTE_NL,
/* pl */  IDCLS_USE_INTERNAL_CALC_PALETTE_PL,
/* sv */  IDCLS_USE_INTERNAL_CALC_PALETTE_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_USE_EXTERNAL_FILE_PALETTE,
/* de */  IDCLS_USE_EXTERNAL_FILE_PALETTE_DE,
/* fr */  IDCLS_USE_EXTERNAL_FILE_PALETTE_FR,
/* it */  IDCLS_USE_EXTERNAL_FILE_PALETTE_IT,
/* nl */  IDCLS_USE_EXTERNAL_FILE_PALETTE_NL,
/* pl */  IDCLS_USE_EXTERNAL_FILE_PALETTE_PL,
/* sv */  IDCLS_USE_EXTERNAL_FILE_PALETTE_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME,
/* de */  IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_DE,
/* fr */  IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_FR,
/* it */  IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_IT,
/* nl */  IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_NL,
/* pl */  IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_PL,
/* sv */  IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FULLSCREEN_MODE,
/* de */  IDCLS_ENABLE_FULLSCREEN_MODE_DE,
/* fr */  IDCLS_ENABLE_FULLSCREEN_MODE_FR,
/* it */  IDCLS_ENABLE_FULLSCREEN_MODE_IT,
/* nl */  IDCLS_ENABLE_FULLSCREEN_MODE_NL,
/* pl */  IDCLS_ENABLE_FULLSCREEN_MODE_PL,
/* sv */  IDCLS_ENABLE_FULLSCREEN_MODE_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FULLSCREEN_MODE,
/* de */  IDCLS_DISABLE_FULLSCREEN_MODE_DE,
/* fr */  IDCLS_DISABLE_FULLSCREEN_MODE_FR,
/* it */  IDCLS_DISABLE_FULLSCREEN_MODE_IT,
/* nl */  IDCLS_DISABLE_FULLSCREEN_MODE_NL,
/* pl */  IDCLS_DISABLE_FULLSCREEN_MODE_PL,
/* sv */  IDCLS_DISABLE_FULLSCREEN_MODE_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_P_DEVICE,
/* de */  IDCLS_P_DEVICE_DE,
/* fr */  IDCLS_P_DEVICE_FR,
/* it */  IDCLS_P_DEVICE_IT,
/* nl */  IDCLS_P_DEVICE_NL,
/* pl */  IDCLS_P_DEVICE_PL,
/* sv */  IDCLS_P_DEVICE_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_SELECT_FULLSCREEN_DEVICE,
/* de */  IDCLS_SELECT_FULLSCREEN_DEVICE_DE,
/* fr */  IDCLS_SELECT_FULLSCREEN_DEVICE_FR,
/* it */  IDCLS_SELECT_FULLSCREEN_DEVICE_IT,
/* nl */  IDCLS_SELECT_FULLSCREEN_DEVICE_NL,
/* pl */  IDCLS_SELECT_FULLSCREEN_DEVICE_PL,
/* sv */  IDCLS_SELECT_FULLSCREEN_DEVICE_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN,
/* de */  IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_DE,
/* fr */  IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_FR,
/* it */  IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_IT,
/* nl */  IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_NL,
/* pl */  IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_PL,
/* sv */  IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN,
/* de */  IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_DE,
/* fr */  IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_FR,
/* it */  IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_IT,
/* nl */  IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_NL,
/* pl */  IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_PL,
/* sv */  IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN,
/* de */  IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_DE,
/* fr */  IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_FR,
/* it */  IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_IT,
/* nl */  IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_NL,
/* pl */  IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_PL,
/* sv */  IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN,
/* de */  IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_DE,
/* fr */  IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_FR,
/* it */  IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_IT,
/* nl */  IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_NL,
/* pl */  IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_PL,
/* sv */  IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_P_MODE,
/* de */  IDCLS_P_MODE_DE,
/* fr */  IDCLS_P_MODE_FR,
/* it */  IDCLS_P_MODE_IT,
/* nl */  IDCLS_P_MODE_NL,
/* pl */  IDCLS_P_MODE_PL,
/* sv */  IDCLS_P_MODE_SV},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_SELECT_FULLSCREEN_MODE,
/* de */  IDCLS_SELECT_FULLSCREEN_MODE_DE,
/* fr */  IDCLS_SELECT_FULLSCREEN_MODE_FR,
/* it */  IDCLS_SELECT_FULLSCREEN_MODE_IT,
/* nl */  IDCLS_SELECT_FULLSCREEN_MODE_NL,
/* pl */  IDCLS_SELECT_FULLSCREEN_MODE_PL,
/* sv */  IDCLS_SELECT_FULLSCREEN_MODE_SV},

/* aciacore.c */
/* en */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE,
/* de */  IDCLS_SPECIFY_ACIA_RS232_DEVICE_DE,
/* fr */  IDCLS_SPECIFY_ACIA_RS232_DEVICE_FR,
/* it */  IDCLS_SPECIFY_ACIA_RS232_DEVICE_IT,
/* nl */  IDCLS_SPECIFY_ACIA_RS232_DEVICE_NL,
/* pl */  IDCLS_SPECIFY_ACIA_RS232_DEVICE_PL,
/* sv */  IDCLS_SPECIFY_ACIA_RS232_DEVICE_SV},

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_8,
/* de */  IDCLS_SET_DEVICE_TYPE_8_DE,
/* fr */  IDCLS_SET_DEVICE_TYPE_8_FR,
/* it */  IDCLS_SET_DEVICE_TYPE_8_IT,
/* nl */  IDCLS_SET_DEVICE_TYPE_8_NL,
/* pl */  IDCLS_SET_DEVICE_TYPE_8_PL,
/* sv */  IDCLS_SET_DEVICE_TYPE_8_SV},

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_9,
/* de */  IDCLS_SET_DEVICE_TYPE_9_DE,
/* fr */  IDCLS_SET_DEVICE_TYPE_9_FR,
/* it */  IDCLS_SET_DEVICE_TYPE_9_IT,
/* nl */  IDCLS_SET_DEVICE_TYPE_9_NL,
/* pl */  IDCLS_SET_DEVICE_TYPE_9_PL,
/* sv */  IDCLS_SET_DEVICE_TYPE_9_SV},

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_10,
/* de */  IDCLS_SET_DEVICE_TYPE_10_DE,
/* fr */  IDCLS_SET_DEVICE_TYPE_10_FR,
/* it */  IDCLS_SET_DEVICE_TYPE_10_IT,
/* nl */  IDCLS_SET_DEVICE_TYPE_10_NL,
/* pl */  IDCLS_SET_DEVICE_TYPE_10_PL,
/* sv */  IDCLS_SET_DEVICE_TYPE_10_SV},

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_11,
/* de */  IDCLS_SET_DEVICE_TYPE_11_DE,
/* fr */  IDCLS_SET_DEVICE_TYPE_11_FR,
/* it */  IDCLS_SET_DEVICE_TYPE_11_IT,
/* nl */  IDCLS_SET_DEVICE_TYPE_11_NL,
/* pl */  IDCLS_SET_DEVICE_TYPE_11_PL,
/* sv */  IDCLS_SET_DEVICE_TYPE_11_SV},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_8,
/* de */  IDCLS_ATTACH_READ_ONLY_8_DE,
/* fr */  IDCLS_ATTACH_READ_ONLY_8_FR,
/* it */  IDCLS_ATTACH_READ_ONLY_8_IT,
/* nl */  IDCLS_ATTACH_READ_ONLY_8_NL,
/* pl */  IDCLS_ATTACH_READ_ONLY_8_PL,
/* sv */  IDCLS_ATTACH_READ_ONLY_8_SV},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_8,
/* de */  IDCLS_ATTACH_READ_WRITE_8_DE,
/* fr */  IDCLS_ATTACH_READ_WRITE_8_FR,
/* it */  IDCLS_ATTACH_READ_WRITE_8_IT,
/* nl */  IDCLS_ATTACH_READ_WRITE_8_NL,
/* pl */  IDCLS_ATTACH_READ_WRITE_8_PL,
/* sv */  IDCLS_ATTACH_READ_WRITE_8_SV},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_9,
/* de */  IDCLS_ATTACH_READ_ONLY_9_DE,
/* fr */  IDCLS_ATTACH_READ_ONLY_9_FR,
/* it */  IDCLS_ATTACH_READ_ONLY_9_IT,
/* nl */  IDCLS_ATTACH_READ_ONLY_9_NL,
/* pl */  IDCLS_ATTACH_READ_ONLY_9_PL,
/* sv */  IDCLS_ATTACH_READ_ONLY_9_SV},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_9,
/* de */  IDCLS_ATTACH_READ_WRITE_9_DE,
/* fr */  IDCLS_ATTACH_READ_WRITE_9_FR,
/* it */  IDCLS_ATTACH_READ_WRITE_9_IT,
/* nl */  IDCLS_ATTACH_READ_WRITE_9_NL,
/* pl */  IDCLS_ATTACH_READ_WRITE_9_PL,
/* sv */  IDCLS_ATTACH_READ_WRITE_9_SV},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_10,
/* de */  IDCLS_ATTACH_READ_ONLY_10_DE,
/* fr */  IDCLS_ATTACH_READ_ONLY_10_FR,
/* it */  IDCLS_ATTACH_READ_ONLY_10_IT,
/* nl */  IDCLS_ATTACH_READ_ONLY_10_NL,
/* pl */  IDCLS_ATTACH_READ_ONLY_10_PL,
/* sv */  IDCLS_ATTACH_READ_ONLY_10_SV},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_10,
/* de */  IDCLS_ATTACH_READ_WRITE_10_DE,
/* fr */  IDCLS_ATTACH_READ_WRITE_10_FR,
/* it */  IDCLS_ATTACH_READ_WRITE_10_IT,
/* nl */  IDCLS_ATTACH_READ_WRITE_10_NL,
/* pl */  IDCLS_ATTACH_READ_WRITE_10_PL,
/* sv */  IDCLS_ATTACH_READ_WRITE_10_SV},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_11,
/* de */  IDCLS_ATTACH_READ_ONLY_11_DE,
/* fr */  IDCLS_ATTACH_READ_ONLY_11_FR,
/* it */  IDCLS_ATTACH_READ_ONLY_11_IT,
/* nl */  IDCLS_ATTACH_READ_ONLY_11_NL,
/* pl */  IDCLS_ATTACH_READ_ONLY_11_PL,
/* sv */  IDCLS_ATTACH_READ_ONLY_11_SV},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_11,
/* de */  IDCLS_ATTACH_READ_WRITE_11_DE,
/* fr */  IDCLS_ATTACH_READ_WRITE_11_FR,
/* it */  IDCLS_ATTACH_READ_WRITE_11_IT,
/* nl */  IDCLS_ATTACH_READ_WRITE_11_NL,
/* pl */  IDCLS_ATTACH_READ_WRITE_11_PL,
/* sv */  IDCLS_ATTACH_READ_WRITE_11_SV},

/* datasette.c */
/* en */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET,
/* de */  IDCLS_ENABLE_AUTO_DATASETTE_RESET_DE,
/* fr */  IDCLS_ENABLE_AUTO_DATASETTE_RESET_FR,
/* it */  IDCLS_ENABLE_AUTO_DATASETTE_RESET_IT,
/* nl */  IDCLS_ENABLE_AUTO_DATASETTE_RESET_NL,
/* pl */  IDCLS_ENABLE_AUTO_DATASETTE_RESET_PL,
/* sv */  IDCLS_ENABLE_AUTO_DATASETTE_RESET_SV},

/* datasette.c */
/* en */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET,
/* de */  IDCLS_DISABLE_AUTO_DATASETTE_RESET_DE,
/* fr */  IDCLS_DISABLE_AUTO_DATASETTE_RESET_FR,
/* it */  IDCLS_DISABLE_AUTO_DATASETTE_RESET_IT,
/* nl */  IDCLS_DISABLE_AUTO_DATASETTE_RESET_NL,
/* pl */  IDCLS_DISABLE_AUTO_DATASETTE_RESET_PL,
/* sv */  IDCLS_DISABLE_AUTO_DATASETTE_RESET_SV},

/* datasette.c */
/* en */ {IDCLS_SET_ZERO_TAP_DELAY,
/* de */  IDCLS_SET_ZERO_TAP_DELAY_DE,
/* fr */  IDCLS_SET_ZERO_TAP_DELAY_FR,
/* it */  IDCLS_SET_ZERO_TAP_DELAY_IT,
/* nl */  IDCLS_SET_ZERO_TAP_DELAY_NL,
/* pl */  IDCLS_SET_ZERO_TAP_DELAY_PL,
/* sv */  IDCLS_SET_ZERO_TAP_DELAY_SV},

/* datasette.c */
/* en */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP,
/* de */  IDCLS_SET_CYCLES_ADDED_GAP_TAP_DE,
/* fr */  IDCLS_SET_CYCLES_ADDED_GAP_TAP_FR,
/* it */  IDCLS_SET_CYCLES_ADDED_GAP_TAP_IT,
/* nl */  IDCLS_SET_CYCLES_ADDED_GAP_TAP_NL,
/* pl */  IDCLS_SET_CYCLES_ADDED_GAP_TAP_PL,
/* sv */  IDCLS_SET_CYCLES_ADDED_GAP_TAP_SV},

#ifdef DEBUG
/* debug.c */
/* en */ {IDCLS_TRACE_MAIN_CPU,
/* de */  IDCLS_TRACE_MAIN_CPU_DE,
/* fr */  IDCLS_TRACE_MAIN_CPU_FR,
/* it */  IDCLS_TRACE_MAIN_CPU_IT,
/* nl */  IDCLS_TRACE_MAIN_CPU_NL,
/* pl */  IDCLS_TRACE_MAIN_CPU_PL,
/* sv */  IDCLS_TRACE_MAIN_CPU_SV},

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_MAIN_CPU,
/* de */  IDCLS_DONT_TRACE_MAIN_CPU_DE,
/* fr */  IDCLS_DONT_TRACE_MAIN_CPU_FR,
/* it */  IDCLS_DONT_TRACE_MAIN_CPU_IT,
/* nl */  IDCLS_DONT_TRACE_MAIN_CPU_NL,
/* pl */  IDCLS_DONT_TRACE_MAIN_CPU_PL,
/* sv */  IDCLS_DONT_TRACE_MAIN_CPU_SV},

/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE0_CPU,
/* de */  IDCLS_TRACE_DRIVE0_CPU_DE,
/* fr */  IDCLS_TRACE_DRIVE0_CPU_FR,
/* it */  IDCLS_TRACE_DRIVE0_CPU_IT,
/* nl */  IDCLS_TRACE_DRIVE0_CPU_NL,
/* pl */  IDCLS_TRACE_DRIVE0_CPU_PL,
/* sv */  IDCLS_TRACE_DRIVE0_CPU_SV},

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE0_CPU,
/* de */  IDCLS_DONT_TRACE_DRIVE0_CPU_DE,
/* fr */  IDCLS_DONT_TRACE_DRIVE0_CPU_FR,
/* it */  IDCLS_DONT_TRACE_DRIVE0_CPU_IT,
/* nl */  IDCLS_DONT_TRACE_DRIVE0_CPU_NL,
/* pl */  IDCLS_DONT_TRACE_DRIVE0_CPU_PL,
/* sv */  IDCLS_DONT_TRACE_DRIVE0_CPU_SV},

/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE1_CPU,
/* de */  IDCLS_TRACE_DRIVE1_CPU_DE,
/* fr */  IDCLS_TRACE_DRIVE1_CPU_FR,
/* it */  IDCLS_TRACE_DRIVE1_CPU_IT,
/* nl */  IDCLS_TRACE_DRIVE1_CPU_NL,
/* pl */  IDCLS_TRACE_DRIVE1_CPU_PL,
/* sv */  IDCLS_TRACE_DRIVE1_CPU_SV},

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE1_CPU,
/* de */  IDCLS_DONT_TRACE_DRIVE1_CPU_DE,
/* fr */  IDCLS_DONT_TRACE_DRIVE1_CPU_FR,
/* it */  IDCLS_DONT_TRACE_DRIVE1_CPU_IT,
/* nl */  IDCLS_DONT_TRACE_DRIVE1_CPU_NL,
/* pl */  IDCLS_DONT_TRACE_DRIVE1_CPU_PL,
/* sv */  IDCLS_DONT_TRACE_DRIVE1_CPU_SV},

#if DRIVE_NUM > 2
/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE2_CPU,
/* de */  IDCLS_TRACE_DRIVE2_CPU_DE,
/* fr */  IDCLS_TRACE_DRIVE2_CPU_FR,
/* it */  IDCLS_TRACE_DRIVE2_CPU_IT,
/* nl */  IDCLS_TRACE_DRIVE2_CPU_NL,
/* pl */  IDCLS_TRACE_DRIVE2_CPU_PL,
/* sv */  IDCLS_TRACE_DRIVE2_CPU_SV},

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE2_CPU,
/* de */  IDCLS_DONT_TRACE_DRIVE2_CPU_DE,
/* fr */  IDCLS_DONT_TRACE_DRIVE2_CPU_FR,
/* it */  IDCLS_DONT_TRACE_DRIVE2_CPU_IT,
/* nl */  IDCLS_DONT_TRACE_DRIVE2_CPU_NL,
/* pl */  IDCLS_DONT_TRACE_DRIVE2_CPU_PL,
/* sv */  IDCLS_DONT_TRACE_DRIVE2_CPU_SV},
#endif

#if DRIVE_NUM > 3
/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE3_CPU,
/* de */  IDCLS_TRACE_DRIVE3_CPU_DE,
/* fr */  IDCLS_TRACE_DRIVE3_CPU_FR,
/* it */  IDCLS_TRACE_DRIVE3_CPU_IT,
/* nl */  IDCLS_TRACE_DRIVE3_CPU_NL,
/* pl */  IDCLS_TRACE_DRIVE3_CPU_PL,
/* sv */  IDCLS_TRACE_DRIVE3_CPU_SV},

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE3_CPU,
/* de */  IDCLS_DONT_TRACE_DRIVE3_CPU_DE,
/* fr */  IDCLS_DONT_TRACE_DRIVE3_CPU_FR,
/* it */  IDCLS_DONT_TRACE_DRIVE3_CPU_IT,
/* nl */  IDCLS_DONT_TRACE_DRIVE3_CPU_NL,
/* pl */  IDCLS_DONT_TRACE_DRIVE3_CPU_PL,
/* sv */  IDCLS_DONT_TRACE_DRIVE3_CPU_SV},
#endif

/* debug.c */
/* en */ {IDCLS_TRACE_MODE,
/* de */  IDCLS_TRACE_MODE_DE,
/* fr */  IDCLS_TRACE_MODE_FR,
/* it */  IDCLS_TRACE_MODE_IT,
/* nl */  IDCLS_TRACE_MODE_NL,
/* pl */  IDCLS_TRACE_MODE_PL,
/* sv */  IDCLS_TRACE_MODE_SV},
#endif

/* event.c */
/* en */ {IDCLS_PLAYBACK_RECORDED_EVENTS,
/* de */  IDCLS_PLAYBACK_RECORDED_EVENTS_DE,
/* fr */  IDCLS_PLAYBACK_RECORDED_EVENTS_FR,
/* it */  IDCLS_PLAYBACK_RECORDED_EVENTS_IT,
/* nl */  IDCLS_PLAYBACK_RECORDED_EVENTS_NL,
/* pl */  IDCLS_PLAYBACK_RECORDED_EVENTS_PL,
/* sv */  IDCLS_PLAYBACK_RECORDED_EVENTS_SV},

/* fliplist.c */
/* en */ {IDCLS_SPECIFY_FLIP_LIST_NAME,
/* de */  IDCLS_SPECIFY_FLIP_LIST_NAME_DE,
/* fr */  IDCLS_SPECIFY_FLIP_LIST_NAME_FR,
/* it */  IDCLS_SPECIFY_FLIP_LIST_NAME_IT,
/* nl */  IDCLS_SPECIFY_FLIP_LIST_NAME_NL,
/* pl */  IDCLS_SPECIFY_FLIP_LIST_NAME_PL,
/* sv */  IDCLS_SPECIFY_FLIP_LIST_NAME_SV},

/* initcmdline.c */
/* en */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS,
/* de */  IDCLS_SHOW_COMMAND_LINE_OPTIONS_DE,
/* fr */  IDCLS_SHOW_COMMAND_LINE_OPTIONS_FR,
/* it */  IDCLS_SHOW_COMMAND_LINE_OPTIONS_IT,
/* nl */  IDCLS_SHOW_COMMAND_LINE_OPTIONS_NL,
/* pl */  IDCLS_SHOW_COMMAND_LINE_OPTIONS_PL,
/* sv */  IDCLS_SHOW_COMMAND_LINE_OPTIONS_SV},

#if (!defined  __OS2__ && !defined __BEOS__)
/* initcmdline.c */
/* en */ {IDCLS_CONSOLE_MODE,
/* de */  IDCLS_CONSOLE_MODE_DE,
/* fr */  IDCLS_CONSOLE_MODE_FR,
/* it */  IDCLS_CONSOLE_MODE_IT,
/* nl */  IDCLS_CONSOLE_MODE_NL,
/* pl */  IDCLS_CONSOLE_MODE_PL,
/* sv */  IDCLS_CONSOLE_MODE_SV},

/* initcmdline.c */
/* en */ {IDCLS_ALLOW_CORE_DUMPS,
/* de */  IDCLS_ALLOW_CORE_DUMPS_DE,
/* fr */  IDCLS_ALLOW_CORE_DUMPS_FR,
/* it */  IDCLS_ALLOW_CORE_DUMPS_IT,
/* nl */  IDCLS_ALLOW_CORE_DUMPS_NL,
/* pl */  IDCLS_ALLOW_CORE_DUMPS_PL,
/* sv */  IDCLS_ALLOW_CORE_DUMPS_SV},

/* initcmdline.c */
/* en */ {IDCLS_DONT_ALLOW_CORE_DUMPS,
/* de */  IDCLS_DONT_ALLOW_CORE_DUMPS_DE,
/* fr */  IDCLS_DONT_ALLOW_CORE_DUMPS_FR,
/* it */  IDCLS_DONT_ALLOW_CORE_DUMPS_IT,
/* nl */  IDCLS_DONT_ALLOW_CORE_DUMPS_NL,
/* pl */  IDCLS_DONT_ALLOW_CORE_DUMPS_PL,
/* sv */  IDCLS_DONT_ALLOW_CORE_DUMPS_SV},
#else
/* initcmdline.c */
/* en */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER,
/* de */  IDCLS_DONT_CALL_EXCEPTION_HANDLER_DE,
/* fr */  IDCLS_DONT_CALL_EXCEPTION_HANDLER_FR,
/* it */  IDCLS_DONT_CALL_EXCEPTION_HANDLER_IT,
/* nl */  IDCLS_DONT_CALL_EXCEPTION_HANDLER_NL,
/* pl */  IDCLS_DONT_CALL_EXCEPTION_HANDLER_PL,
/* sv */  IDCLS_DONT_CALL_EXCEPTION_HANDLER_SV},

/* initcmdline.c */
/* en */ {IDCLS_CALL_EXCEPTION_HANDLER,
/* de */  IDCLS_CALL_EXCEPTION_HANDLER_DE,
/* fr */  IDCLS_CALL_EXCEPTION_HANDLER_FR,
/* it */  IDCLS_CALL_EXCEPTION_HANDLER_IT,
/* nl */  IDCLS_CALL_EXCEPTION_HANDLER_NL,
/* pl */  IDCLS_CALL_EXCEPTION_HANDLER_PL,
/* sv */  IDCLS_CALL_EXCEPTION_HANDLER_SV},
#endif

/* initcmdline.c */
/* en */ {IDCLS_RESTORE_DEFAULT_SETTINGS,
/* de */  IDCLS_RESTORE_DEFAULT_SETTINGS_DE,
/* fr */  IDCLS_RESTORE_DEFAULT_SETTINGS_FR,
/* it */  IDCLS_RESTORE_DEFAULT_SETTINGS_IT,
/* nl */  IDCLS_RESTORE_DEFAULT_SETTINGS_NL,
/* pl */  IDCLS_RESTORE_DEFAULT_SETTINGS_PL,
/* sv */  IDCLS_RESTORE_DEFAULT_SETTINGS_SV},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AND_AUTOSTART,
/* de */  IDCLS_ATTACH_AND_AUTOSTART_DE,
/* fr */  IDCLS_ATTACH_AND_AUTOSTART_FR,
/* it */  IDCLS_ATTACH_AND_AUTOSTART_IT,
/* nl */  IDCLS_ATTACH_AND_AUTOSTART_NL,
/* pl */  IDCLS_ATTACH_AND_AUTOSTART_PL,
/* sv */  IDCLS_ATTACH_AND_AUTOSTART_SV},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AND_AUTOLOAD,
/* de */  IDCLS_ATTACH_AND_AUTOLOAD_DE,
/* fr */  IDCLS_ATTACH_AND_AUTOLOAD_FR,
/* it */  IDCLS_ATTACH_AND_AUTOLOAD_IT,
/* nl */  IDCLS_ATTACH_AND_AUTOLOAD_NL,
/* pl */  IDCLS_ATTACH_AND_AUTOLOAD_PL,
/* sv */  IDCLS_ATTACH_AND_AUTOLOAD_SV},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_TAPE,
/* de */  IDCLS_ATTACH_AS_TAPE_DE,
/* fr */  IDCLS_ATTACH_AS_TAPE_FR,
/* it */  IDCLS_ATTACH_AS_TAPE_IT,
/* nl */  IDCLS_ATTACH_AS_TAPE_NL,
/* pl */  IDCLS_ATTACH_AS_TAPE_PL,
/* sv */  IDCLS_ATTACH_AS_TAPE_SV},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_8,
/* de */  IDCLS_ATTACH_AS_DISK_8_DE,
/* fr */  IDCLS_ATTACH_AS_DISK_8_FR,
/* it */  IDCLS_ATTACH_AS_DISK_8_IT,
/* nl */  IDCLS_ATTACH_AS_DISK_8_NL,
/* pl */  IDCLS_ATTACH_AS_DISK_8_PL,
/* sv */  IDCLS_ATTACH_AS_DISK_8_SV},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_9,
/* de */  IDCLS_ATTACH_AS_DISK_9_DE,
/* fr */  IDCLS_ATTACH_AS_DISK_9_FR,
/* it */  IDCLS_ATTACH_AS_DISK_9_IT,
/* nl */  IDCLS_ATTACH_AS_DISK_9_NL,
/* pl */  IDCLS_ATTACH_AS_DISK_9_PL,
/* sv */  IDCLS_ATTACH_AS_DISK_9_SV},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_10,
/* de */  IDCLS_ATTACH_AS_DISK_10_DE,
/* fr */  IDCLS_ATTACH_AS_DISK_10_FR,
/* it */  IDCLS_ATTACH_AS_DISK_10_IT,
/* nl */  IDCLS_ATTACH_AS_DISK_10_NL,
/* pl */  IDCLS_ATTACH_AS_DISK_10_PL,
/* sv */  IDCLS_ATTACH_AS_DISK_10_SV},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_11,
/* de */  IDCLS_ATTACH_AS_DISK_11_DE,
/* fr */  IDCLS_ATTACH_AS_DISK_11_FR,
/* it */  IDCLS_ATTACH_AS_DISK_11_IT,
/* nl */  IDCLS_ATTACH_AS_DISK_11_NL,
/* pl */  IDCLS_ATTACH_AS_DISK_11_PL,
/* sv */  IDCLS_ATTACH_AS_DISK_11_SV},

/* kbdbuf.c */
/* en */ {IDCLS_P_STRING,
/* de */  IDCLS_P_STRING_DE,
/* fr */  IDCLS_P_STRING_FR,
/* it */  IDCLS_P_STRING_IT,
/* nl */  IDCLS_P_STRING_NL,
/* pl */  IDCLS_P_STRING_PL,
/* sv */  IDCLS_P_STRING_SV},

/* kbdbuf.c */
/* en */ {IDCLS_PUT_STRING_INTO_KEYBUF,
/* de */  IDCLS_PUT_STRING_INTO_KEYBUF_DE,
/* fr */  IDCLS_PUT_STRING_INTO_KEYBUF_FR,
/* it */  IDCLS_PUT_STRING_INTO_KEYBUF_IT,
/* nl */  IDCLS_PUT_STRING_INTO_KEYBUF_NL,
/* pl */  IDCLS_PUT_STRING_INTO_KEYBUF_PL,
/* sv */  IDCLS_PUT_STRING_INTO_KEYBUF_SV},

/* log.c */
/* en */ {IDCLS_SPECIFY_LOG_FILE_NAME,
/* de */  IDCLS_SPECIFY_LOG_FILE_NAME_DE,
/* fr */  IDCLS_SPECIFY_LOG_FILE_NAME_FR,
/* it */  IDCLS_SPECIFY_LOG_FILE_NAME_IT,
/* nl */  IDCLS_SPECIFY_LOG_FILE_NAME_NL,
/* pl */  IDCLS_SPECIFY_LOG_FILE_NAME_PL,
/* sv */  IDCLS_SPECIFY_LOG_FILE_NAME_SV},

/* mouse.c */
/* en */ {IDCLS_ENABLE_1351_MOUSE,
/* de */  IDCLS_ENABLE_1351_MOUSE_DE,
/* fr */  IDCLS_ENABLE_1351_MOUSE_FR,
/* it */  IDCLS_ENABLE_1351_MOUSE_IT,
/* nl */  IDCLS_ENABLE_1351_MOUSE_NL,
/* pl */  IDCLS_ENABLE_1351_MOUSE_PL,
/* sv */  IDCLS_ENABLE_1351_MOUSE_SV},

/* mouse.c */
/* en */ {IDCLS_DISABLE_1351_MOUSE,
/* de */  IDCLS_DISABLE_1351_MOUSE_DE,
/* fr */  IDCLS_DISABLE_1351_MOUSE_FR,
/* it */  IDCLS_DISABLE_1351_MOUSE_IT,
/* nl */  IDCLS_DISABLE_1351_MOUSE_NL,
/* pl */  IDCLS_DISABLE_1351_MOUSE_PL,
/* sv */  IDCLS_DISABLE_1351_MOUSE_SV},

/* mouse.c */
/* en */ {IDCLS_SELECT_MOUSE_JOY_PORT,
/* de */  IDCLS_SELECT_MOUSE_JOY_PORT_DE,
/* fr */  IDCLS_SELECT_MOUSE_JOY_PORT_FR,
/* it */  IDCLS_SELECT_MOUSE_JOY_PORT_IT,
/* nl */  IDCLS_SELECT_MOUSE_JOY_PORT_NL,
/* pl */  IDCLS_SELECT_MOUSE_JOY_PORT_PL,
/* sv */  IDCLS_SELECT_MOUSE_JOY_PORT_SV},

/* ram.c */
/* en */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE,
/* de */  IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_DE,
/* fr */  IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_FR,
/* it */  IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_IT,
/* nl */  IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_NL,
/* pl */  IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_PL,
/* sv */  IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_SV},

/* ram.c */
/* en */ {IDCLS_P_NUM_OF_BYTES,
/* de */  IDCLS_P_NUM_OF_BYTES_DE,
/* fr */  IDCLS_P_NUM_OF_BYTES_FR,
/* it */  IDCLS_P_NUM_OF_BYTES_IT,
/* nl */  IDCLS_P_NUM_OF_BYTES_NL,
/* pl */  IDCLS_P_NUM_OF_BYTES_PL,
/* sv */  IDCLS_P_NUM_OF_BYTES_SV},

/* ram.c */
/* en */ {IDCLS_LENGTH_BLOCK_SAME_VALUE,
/* de */  IDCLS_LENGTH_BLOCK_SAME_VALUE_DE,
/* fr */  IDCLS_LENGTH_BLOCK_SAME_VALUE_FR,
/* it */  IDCLS_LENGTH_BLOCK_SAME_VALUE_IT,
/* nl */  IDCLS_LENGTH_BLOCK_SAME_VALUE_NL,
/* pl */  IDCLS_LENGTH_BLOCK_SAME_VALUE_PL,
/* sv */  IDCLS_LENGTH_BLOCK_SAME_VALUE_SV},

/* ram.c */
/* en */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN,
/* de */  IDCLS_LENGTH_BLOCK_SAME_PATTERN_DE,
/* fr */  IDCLS_LENGTH_BLOCK_SAME_PATTERN_FR,
/* it */  IDCLS_LENGTH_BLOCK_SAME_PATTERN_IT,
/* nl */  IDCLS_LENGTH_BLOCK_SAME_PATTERN_NL,
/* pl */  IDCLS_LENGTH_BLOCK_SAME_PATTERN_PL,
/* sv */  IDCLS_LENGTH_BLOCK_SAME_PATTERN_SV},

/* sound.c */
/* en */ {IDCLS_ENABLE_SOUND_PLAYBACK,
/* de */  IDCLS_ENABLE_SOUND_PLAYBACK_DE,
/* fr */  IDCLS_ENABLE_SOUND_PLAYBACK_FR,
/* it */  IDCLS_ENABLE_SOUND_PLAYBACK_IT,
/* nl */  IDCLS_ENABLE_SOUND_PLAYBACK_NL,
/* pl */  IDCLS_ENABLE_SOUND_PLAYBACK_PL,
/* sv */  IDCLS_ENABLE_SOUND_PLAYBACK_SV},

/* sound.c */
/* en */ {IDCLS_DISABLE_SOUND_PLAYBACK,
/* de */  IDCLS_DISABLE_SOUND_PLAYBACK_DE,
/* fr */  IDCLS_DISABLE_SOUND_PLAYBACK_FR,
/* it */  IDCLS_DISABLE_SOUND_PLAYBACK_IT,
/* nl */  IDCLS_DISABLE_SOUND_PLAYBACK_NL,
/* pl */  IDCLS_DISABLE_SOUND_PLAYBACK_PL,
/* sv */  IDCLS_DISABLE_SOUND_PLAYBACK_SV},

/* sound.c */
/* en */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ,
/* de */  IDCLS_SET_SAMPLE_RATE_VALUE_HZ_DE,
/* fr */  IDCLS_SET_SAMPLE_RATE_VALUE_HZ_FR,
/* it */  IDCLS_SET_SAMPLE_RATE_VALUE_HZ_IT,
/* nl */  IDCLS_SET_SAMPLE_RATE_VALUE_HZ_NL,
/* pl */  IDCLS_SET_SAMPLE_RATE_VALUE_HZ_PL,
/* sv */  IDCLS_SET_SAMPLE_RATE_VALUE_HZ_SV},

/* sound.c */
/* en */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC,
/* de */  IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_DE,
/* fr */  IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_FR,
/* it */  IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_IT,
/* nl */  IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_NL,
/* pl */  IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_PL,
/* sv */  IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_SV},

/* sound.c */
/* en */ {IDCLS_SPECIFY_SOUND_DRIVER,
/* de */  IDCLS_SPECIFY_SOUND_DRIVER_DE,
/* fr */  IDCLS_SPECIFY_SOUND_DRIVER_FR,
/* it */  IDCLS_SPECIFY_SOUND_DRIVER_IT,
/* nl */  IDCLS_SPECIFY_SOUND_DRIVER_NL,
/* pl */  IDCLS_SPECIFY_SOUND_DRIVER_PL,
/* sv */  IDCLS_SPECIFY_SOUND_DRIVER_SV},

/* sound.c */
/* en */ {IDCLS_P_ARGS,
/* de */  IDCLS_P_ARGS_DE,
/* fr */  IDCLS_P_ARGS_FR,
/* it */  IDCLS_P_ARGS_IT,
/* nl */  IDCLS_P_ARGS_NL,
/* pl */  IDCLS_P_ARGS_PL,
/* sv */  IDCLS_P_ARGS_SV},

/* sound.c */
/* en */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM,
/* de */  IDCLS_SPECIFY_SOUND_DRIVER_PARAM_DE,
/* fr */  IDCLS_SPECIFY_SOUND_DRIVER_PARAM_FR,
/* it */  IDCLS_SPECIFY_SOUND_DRIVER_PARAM_IT,
/* nl */  IDCLS_SPECIFY_SOUND_DRIVER_PARAM_NL,
/* pl */  IDCLS_SPECIFY_SOUND_DRIVER_PARAM_PL,
/* sv */  IDCLS_SPECIFY_SOUND_DRIVER_PARAM_SV},

/* sound.c */
/* en */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER,
/* de */  IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_DE,
/* fr */  IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_FR,
/* it */  IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_IT,
/* nl */  IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_NL,
/* pl */  IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_PL,
/* sv */  IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_SV},

/* sound.c */
/* en */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM,
/* de */  IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_DE,
/* fr */  IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_FR,
/* it */  IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_IT,
/* nl */  IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_NL,
/* pl */  IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_PL,
/* sv */  IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_SV},

/* sound.c */
/* en */ {IDCLS_P_SYNC,
/* de */  IDCLS_P_SYNC_DE,
/* fr */  IDCLS_P_SYNC_FR,
/* it */  IDCLS_P_SYNC_IT,
/* nl */  IDCLS_P_SYNC_NL,
/* pl */  IDCLS_P_SYNC_PL,
/* sv */  IDCLS_P_SYNC_SV},

/* sound.c */
/* en */ {IDCLS_SET_SOUND_SPEED_ADJUST,
/* de */  IDCLS_SET_SOUND_SPEED_ADJUST_DE,
/* fr */  IDCLS_SET_SOUND_SPEED_ADJUST_FR,
/* it */  IDCLS_SET_SOUND_SPEED_ADJUST_IT,
/* nl */  IDCLS_SET_SOUND_SPEED_ADJUST_NL,
/* pl */  IDCLS_SET_SOUND_SPEED_ADJUST_PL,
/* sv */  IDCLS_SET_SOUND_SPEED_ADJUST_SV},

/* sysfile.c */
/* en */ {IDCLS_P_PATH,
/* de */  IDCLS_P_PATH_DE,
/* fr */  IDCLS_P_PATH_FR,
/* it */  IDCLS_P_PATH_IT,
/* nl */  IDCLS_P_PATH_NL,
/* pl */  IDCLS_P_PATH_PL,
/* sv */  IDCLS_P_PATH_SV},

/* sysfile.c */
/* en */ {IDCLS_DEFINE_SYSTEM_FILES_PATH,
/* de */  IDCLS_DEFINE_SYSTEM_FILES_PATH_DE,
/* fr */  IDCLS_DEFINE_SYSTEM_FILES_PATH_FR,
/* it */  IDCLS_DEFINE_SYSTEM_FILES_PATH_IT,
/* nl */  IDCLS_DEFINE_SYSTEM_FILES_PATH_NL,
/* pl */  IDCLS_DEFINE_SYSTEM_FILES_PATH_PL,
/* sv */  IDCLS_DEFINE_SYSTEM_FILES_PATH_SV},

/* traps.c */
/* en */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION,
/* de */  IDCLS_ENABLE_TRAPS_FAST_EMULATION_DE,
/* fr */  IDCLS_ENABLE_TRAPS_FAST_EMULATION_FR,
/* it */  IDCLS_ENABLE_TRAPS_FAST_EMULATION_IT,
/* nl */  IDCLS_ENABLE_TRAPS_FAST_EMULATION_NL,
/* pl */  IDCLS_ENABLE_TRAPS_FAST_EMULATION_PL,
/* sv */  IDCLS_ENABLE_TRAPS_FAST_EMULATION_SV},

/* traps.c */
/* en */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION,
/* de */  IDCLS_DISABLE_TRAPS_FAST_EMULATION_DE,
/* fr */  IDCLS_DISABLE_TRAPS_FAST_EMULATION_FR,
/* it */  IDCLS_DISABLE_TRAPS_FAST_EMULATION_IT,
/* nl */  IDCLS_DISABLE_TRAPS_FAST_EMULATION_NL,
/* pl */  IDCLS_DISABLE_TRAPS_FAST_EMULATION_PL,
/* sv */  IDCLS_DISABLE_TRAPS_FAST_EMULATION_SV},

/* vsync.c */
/* en */ {IDCLS_LIMIT_SPEED_TO_VALUE,
/* de */  IDCLS_LIMIT_SPEED_TO_VALUE_DE,
/* fr */  IDCLS_LIMIT_SPEED_TO_VALUE_FR,
/* it */  IDCLS_LIMIT_SPEED_TO_VALUE_IT,
/* nl */  IDCLS_LIMIT_SPEED_TO_VALUE_NL,
/* pl */  IDCLS_LIMIT_SPEED_TO_VALUE_PL,
/* sv */  IDCLS_LIMIT_SPEED_TO_VALUE_SV},

/* vsync.c */
/* en */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES,
/* de */  IDCLS_UPDATE_EVERY_VALUE_FRAMES_DE,
/* fr */  IDCLS_UPDATE_EVERY_VALUE_FRAMES_FR,
/* it */  IDCLS_UPDATE_EVERY_VALUE_FRAMES_IT,
/* nl */  IDCLS_UPDATE_EVERY_VALUE_FRAMES_NL,
/* pl */  IDCLS_UPDATE_EVERY_VALUE_FRAMES_PL,
/* sv */  IDCLS_UPDATE_EVERY_VALUE_FRAMES_SV},

/* vsync.c */
/* en */ {IDCLS_ENABLE_WARP_MODE,
/* de */  IDCLS_ENABLE_WARP_MODE_DE,
/* fr */  IDCLS_ENABLE_WARP_MODE_FR,
/* it */  IDCLS_ENABLE_WARP_MODE_IT,
/* nl */  IDCLS_ENABLE_WARP_MODE_NL,
/* pl */  IDCLS_ENABLE_WARP_MODE_PL,
/* sv */  IDCLS_ENABLE_WARP_MODE_SV},

/* vsync.c */
/* en */ {IDCLS_DISABLE_WARP_MODE,
/* de */  IDCLS_DISABLE_WARP_MODE_DE,
/* fr */  IDCLS_DISABLE_WARP_MODE_FR,
/* it */  IDCLS_DISABLE_WARP_MODE_IT,
/* nl */  IDCLS_DISABLE_WARP_MODE_NL,
/* pl */  IDCLS_DISABLE_WARP_MODE_PL,
/* sv */  IDCLS_DISABLE_WARP_MODE_SV},

/* translate.c */
/* en */ {IDCLS_P_ISO_LANGUAGE_CODE,
/* de */  IDCLS_P_ISO_LANGUAGE_CODE_DE,
/* fr */  IDCLS_P_ISO_LANGUAGE_CODE_FR,
/* it */  IDCLS_P_ISO_LANGUAGE_CODE_IT,
/* nl */  IDCLS_P_ISO_LANGUAGE_CODE_NL,
/* pl */  IDCLS_P_ISO_LANGUAGE_CODE_PL,
/* sv */  IDCLS_P_ISO_LANGUAGE_CODE_SV},

/* translate.c */
/* en */ {IDCLS_SPECIFY_ISO_LANG_CODE,
/* de */  IDCLS_SPECIFY_ISO_LANG_CODE_DE,
/* fr */  IDCLS_SPECIFY_ISO_LANG_CODE_FR,
/* it */  IDCLS_SPECIFY_ISO_LANG_CODE_IT,
/* nl */  IDCLS_SPECIFY_ISO_LANG_CODE_NL,
/* pl */  IDCLS_SPECIFY_ISO_LANG_CODE_PL,
/* sv */  IDCLS_SPECIFY_ISO_LANG_CODE_SV},

/* c64/plus60k.c */
/* en */ {IDCLS_ENABLE_PLUS60K_EXPANSION,
/* de */  IDCLS_ENABLE_PLUS60K_EXPANSION_DE,
/* fr */  IDCLS_ENABLE_PLUS60K_EXPANSION_FR,
/* it */  IDCLS_ENABLE_PLUS60K_EXPANSION_IT,
/* nl */  IDCLS_ENABLE_PLUS60K_EXPANSION_NL,
/* pl */  IDCLS_ENABLE_PLUS60K_EXPANSION_PL,
/* sv */  IDCLS_ENABLE_PLUS60K_EXPANSION_SV},

/* c64/plus60k.c */
/* en */ {IDCLS_DISABLE_PLUS60K_EXPANSION,
/* de */  IDCLS_DISABLE_PLUS60K_EXPANSION_DE,
/* fr */  IDCLS_DISABLE_PLUS60K_EXPANSION_FR,
/* it */  IDCLS_DISABLE_PLUS60K_EXPANSION_IT,
/* nl */  IDCLS_DISABLE_PLUS60K_EXPANSION_NL,
/* pl */  IDCLS_DISABLE_PLUS60K_EXPANSION_PL,
/* sv */  IDCLS_DISABLE_PLUS60K_EXPANSION_SV},

/* c64/plus60k.c */
/* en */ {IDCLS_SPECIFY_PLUS60K_NAME,
/* de */  IDCLS_SPECIFY_PLUS60K_NAME_DE,
/* fr */  IDCLS_SPECIFY_PLUS60K_NAME_FR,
/* it */  IDCLS_SPECIFY_PLUS60K_NAME_IT,
/* nl */  IDCLS_SPECIFY_PLUS60K_NAME_NL,
/* pl */  IDCLS_SPECIFY_PLUS60K_NAME_PL,
/* sv */  IDCLS_SPECIFY_PLUS60K_NAME_SV}

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
      IDCLS_P_ISO_LANGUAGE_CODE, IDCLS_SPECIFY_ISO_LANG_CODE },
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
