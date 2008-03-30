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
/* de */ {IDGS_PLAYBACK_ERROR_DIFFERENT_DE, "Wiedergabe Fehler: %s ist unterschiedlich von Zeile %d in Datei debug%06d"},
/* fr */ {IDGS_PLAYBACK_ERROR_DIFFERENT_FR, "Erreur de lecture: %s est différente de la ligne %d du fichier débug%06d"},
/* it */ {IDGS_PLAYBACK_ERROR_DIFFERENT_IT, "Errore di riproduzione: %s è differente dalla linea %d del file di debug %06d"},
/* nl */ {IDGS_PLAYBACK_ERROR_DIFFERENT_NL, "Afspeel fout: %s is anders dan lijn %d van bestand debug%06d"},
/* pl */ {IDGS_PLAYBACK_ERROR_DIFFERENT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_PLAYBACK_ERROR_DIFFERENT_SV, "Återspelningsfel: %s inte lika som rad %d i filen debug%06d"},

/* event.c */
/* en */ {IDGS_CANNOT_CREATE_IMAGE_S,    "Cannot create image file %s"},
/* de */ {IDGS_CANNOT_CREATE_IMAGE_S_DE, "Kann Datei `%s' nicht erzeugen"},
/* fr */ {IDGS_CANNOT_CREATE_IMAGE_S_FR, "Impossible de créer le fichier image %s"},
/* it */ {IDGS_CANNOT_CREATE_IMAGE_S_IT, "Non è possibile creare l'immagine %s"},
/* nl */ {IDGS_CANNOT_CREATE_IMAGE_S_NL, "Kan bestand `%s' niet maken"},
/* pl */ {IDGS_CANNOT_CREATE_IMAGE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_CREATE_IMAGE_S_SV, "Kan inte skapa avbildningsfil %s"},

/* event.c */
/* en */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S,    "Cannot write image file %s"},
/* de */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_DE, "Kann Datei %s nicht schreiben"},
/* fr */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_FR, "Impossible d'écrire le fichier image %s"},
/* it */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_IT, "Non è possibile scrivere l'immagine %s"},
/* nl */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_NL, "Kan niet schrijven naar bestand %s"},
/* pl */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_WRITE_IMAGE_FILE_S_SV, "Kan inte skriva avbildningsfil %s"},

/* event.c */
/* en */ {IDGS_CANNOT_FIND_MAPPED_NAME_S,    "Cannot find mapped name for %s"},
/* de */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_DE, "Kann zugeordneten Namen für `%s' nicht finden."},
/* fr */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_FR, "Impossible de trouver le nom correspondant à %s"},
/* it */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_IT, ""},  /* fuzzy */
/* nl */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_NL, "Kan vertaalde naam voor %s niet vinden"},
/* pl */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_FIND_MAPPED_NAME_S_SV, "Kan inte kopplat namn för %s"},

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
/* it */ {IDGS_CANT_CREATE_END_SNAP_S_IT, "Non è possibile creare il file di fine snapshot %s."},
/* nl */ {IDGS_CANT_CREATE_END_SNAP_S_NL, "Kon het eind momentopname bestand %s niet maken."},
/* pl */ {IDGS_CANT_CREATE_END_SNAP_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANT_CREATE_END_SNAP_S_SV, "Kunde inte skapa slutögonblicksbildfilen %s."},

/* event.c */
/* en */ {IDGS_CANT_OPEN_END_SNAP_S,    "Could not open end snapshot file %s."},
/* de */ {IDGS_CANT_OPEN_END_SNAP_S_DE, "Kann Ende Snapshot Datei nicht öffnen: %s"},
/* fr */ {IDGS_CANT_OPEN_END_SNAP_S_FR, "Impossible d'ouvrir le fichier de sauvegarde de fin %s."},
/* it */ {IDGS_CANT_OPEN_END_SNAP_S_IT, "Non è possibile aprire il file di fine snapshot %s."},
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
/* fr */ {IDGS_RESOURCE_S_BLOCKED_BY_S_FR, "La ressource %s est bloquée par %s."},
/* it */ {IDGS_RESOURCE_S_BLOCKED_BY_S_IT, "Risorsa %s bloccata da %s."},
/* nl */ {IDGS_RESOURCE_S_BLOCKED_BY_S_NL, "Bron %s geblokeerd door %s."},
/* pl */ {IDGS_RESOURCE_S_BLOCKED_BY_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RESOURCE_S_BLOCKED_BY_S_SV, "Resursen %s blockeras av %s."},

#ifdef HAVE_NETWORK
/* network.c */
/* en */ {IDGS_TESTING_BEST_FRAME_DELAY,    "Testing best frame delay..."},
/* de */ {IDGS_TESTING_BEST_FRAME_DELAY_DE, "Teste optimale Framerate..."},
/* fr */ {IDGS_TESTING_BEST_FRAME_DELAY_FR, ""},  /* fuzzy */
/* it */ {IDGS_TESTING_BEST_FRAME_DELAY_IT, "Rileva il miglior ritardo tra frame..."},
/* nl */ {IDGS_TESTING_BEST_FRAME_DELAY_NL, "Beste frame vertraging aan het testen..."},
/* pl */ {IDGS_TESTING_BEST_FRAME_DELAY_PL, ""},  /* fuzzy */
/* sv */ {IDGS_TESTING_BEST_FRAME_DELAY_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_USING_D_FRAMES_DELAY,    "Using %d frames delay."},
/* de */ {IDGS_USING_D_FRAMES_DELAY_DE, "Benutze %d Frameverzögerung."},
/* fr */ {IDGS_USING_D_FRAMES_DELAY_FR, ""},  /* fuzzy */
/* it */ {IDGS_USING_D_FRAMES_DELAY_IT, "Utilizzo %d frame di ritardo."},
/* nl */ {IDGS_USING_D_FRAMES_DELAY_NL, "%d frames vertraging in gebruik."},
/* pl */ {IDGS_USING_D_FRAMES_DELAY_PL, ""},  /* fuzzy */
/* sv */ {IDGS_USING_D_FRAMES_DELAY_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER,    "Cannot load snapshot file for transfer"},
/* de */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_DE, "Kann Snapshot Datei für Transfer nicht laden"},
/* fr */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_FR, ""},  /* fuzzy */
/* it */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_IT, "Non à possibile caricare il file di snapshot per il trasferimento"},
/* nl */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_NL, "Kan het momentopname bestand voor versturen niet laden"},
/* pl */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT,    "Sending snapshot to client..."},
/* de */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_DE, "Sende Snapshot zu Client..."},
/* fr */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_FR, ""},  /* fuzzy */
/* it */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_IT, "Invio dell'immagine dello snapshot al client in corso..."},
/* nl */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_NL, "Momentopname wordt naar de andere computer verstuurd..."},
/* pl */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT,    "Cannot send snapshot to client"},
/* de */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_DE, "Kann Snapshot Datei nicht senden"},
/* fr */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_FR, ""},  /* fuzzy */
/* it */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_IT, "Non à possibile inviare lo snapshot al client"},
/* nl */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_NL, "Kan de momentopname niet naar de andere computer versturen"},
/* pl */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S,    "Cannot create snapshot file %s"},
/* de */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_DE, "Kann Snapshot Datei `%s' nicht erzeugen"},
/* fr */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_FR, ""},  /* fuzzy */
/* it */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_IT, "Non à possibile creare il file di snapshot %s"},
/* nl */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_NL, "Kan het momentopname bestand %s niet maken"},
/* pl */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S,    "Cannot open snapshot file %s"},
/* de */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_DE, "Kann Snapshot Datei `%s' nicht öffnen"},
/* fr */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_FR, ""},  /* fuzzy */
/* it */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_IT, "Non à possibile aprire il file di snapshot %s"},
/* nl */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_NL, "Kan het momentopname bestand %s niet openen"},
/* pl */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT,    "Server is waiting for a client..."},
/* de */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_DE, "Server wartet auf Client..."},
/* fr */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_FR, ""},  /* fuzzy */
/* it */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_IT, "Il server à in attesa di un client..."},
/* nl */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_NL, "De server wacht op de andere computer..."},
/* pl */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT,    "Cannot create snapshot file %s. Select different history directory!"},
/* de */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_DE, "Kann Snapshot Datei `%s' nicht erzeugen. Verwende ein anderes Verzeichnis!"},
/* fr */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_FR, ""},  /* fuzzy */
/* it */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_IT, "Non à possibile creare il file di snapshot %s. Seleziona una directory diversa per la cronologia."},
/* nl */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_NL, "Kan het momentopname bestand %s niet maken. Selecteer een andere geschiedenis folder!"},
/* pl */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_CANNOT_RESOLVE_S,    "Cannot resolve %s"},
/* de */ {IDGS_CANNOT_RESOLVE_S_DE, "Kann %s nicht auflösen"},
/* fr */ {IDGS_CANNOT_RESOLVE_S_FR, ""},  /* fuzzy */
/* it */ {IDGS_CANNOT_RESOLVE_S_IT, "Non à possibile risolvere %s"},
/* nl */ {IDGS_CANNOT_RESOLVE_S_NL, "Onbekende host %s"},
/* pl */ {IDGS_CANNOT_RESOLVE_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_RESOLVE_S_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_CANNOT_CONNECT_TO_S,    "Cannot connect to %s (no server running on port %d)."},
/* de */ {IDGS_CANNOT_CONNECT_TO_S_DE, "Kann zu %s nicht verbinden (Kein Server aktiv auf Port %d)."},
/* fr */ {IDGS_CANNOT_CONNECT_TO_S_FR, ""},  /* fuzzy */
/* it */ {IDGS_CANNOT_CONNECT_TO_S_IT, "Non à possibile connettersi a %s (nessun server è attivo sulla porta %d)."},
/* nl */ {IDGS_CANNOT_CONNECT_TO_S_NL, "Kan geen connectie maken met %s (er is geen server aanwezig op poort %d)."},
/* pl */ {IDGS_CANNOT_CONNECT_TO_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_CONNECT_TO_S_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_RECEIVING_SNAPSHOT_SERVER,    "Receiving snapshot from server..."},
/* de */ {IDGS_RECEIVING_SNAPSHOT_SERVER_DE, "Empfange Snapshot von Server..."},
/* fr */ {IDGS_RECEIVING_SNAPSHOT_SERVER_FR, ""},  /* fuzzy */
/* it */ {IDGS_RECEIVING_SNAPSHOT_SERVER_IT, "Ricezione dello snapshot dal server in corso..."},
/* nl */ {IDGS_RECEIVING_SNAPSHOT_SERVER_NL, "Momentopname van de server wordt ontvangen..."},
/* pl */ {IDGS_RECEIVING_SNAPSHOT_SERVER_PL, ""},  /* fuzzy */
/* sv */ {IDGS_RECEIVING_SNAPSHOT_SERVER_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_NETWORK_OUT_OF_SYNC,    "Network out of sync - disconnecting."},
/* de */ {IDGS_NETWORK_OUT_OF_SYNC_DE, "Netzwerksynchronisationsproblem - Trenne Verbindung."},
/* fr */ {IDGS_NETWORK_OUT_OF_SYNC_FR, ""},  /* fuzzy */
/* it */ {IDGS_NETWORK_OUT_OF_SYNC_IT, "Rete non sincronizzata - disconnesione in corso."},
/* nl */ {IDGS_NETWORK_OUT_OF_SYNC_NL, "Netwerk niet synchroon - connectie wordt verbroken."},
/* pl */ {IDGS_NETWORK_OUT_OF_SYNC_PL, ""},  /* fuzzy */
/* sv */ {IDGS_NETWORK_OUT_OF_SYNC_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_REMOTE_HOST_DISCONNECTED,    "Remote host disconnected."},
/* de */ {IDGS_REMOTE_HOST_DISCONNECTED_DE, "Entfernter Rechner getrennt."},
/* fr */ {IDGS_REMOTE_HOST_DISCONNECTED_FR, ""},  /* fuzzy */
/* it */ {IDGS_REMOTE_HOST_DISCONNECTED_IT, "Disconnesso dall'host remoto."},
/* nl */ {IDGS_REMOTE_HOST_DISCONNECTED_NL, "Andere computer heeft de verbinding verbroken."},
/* pl */ {IDGS_REMOTE_HOST_DISCONNECTED_PL, ""},  /* fuzzy */
/* sv */ {IDGS_REMOTE_HOST_DISCONNECTED_SV, ""},  /* fuzzy */

/* network.c */
/* en */ {IDGS_REMOTE_HOST_SUSPENDING,    "Remote host suspending..."},
/* de */ {IDGS_REMOTE_HOST_SUSPENDING_DE, "Entfernter Rechner angehaltedn..."},
/* fr */ {IDGS_REMOTE_HOST_SUSPENDING_FR, ""},  /* fuzzy */
/* it */ {IDGS_REMOTE_HOST_SUSPENDING_IT, "Sospesione dell'host remoto in corso..."},
/* nl */ {IDGS_REMOTE_HOST_SUSPENDING_NL, "Andere computer halt de netplay..."},
/* pl */ {IDGS_REMOTE_HOST_SUSPENDING_PL, ""},  /* fuzzy */
/* sv */ {IDGS_REMOTE_HOST_SUSPENDING_SV, ""},  /* fuzzy */

#ifdef HAVE_IPV6
/* network.c */
/* en */ {IDGS_CANNOT_SWITCH_IPV4_IPV6,    "Cannot switch IPV4/IPV6 while netplay is active."},
/* de */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_DE, "Kann zwischen IPV4/IPV6 nicht wechseln, solange netplay aktiv ist."},
/* fr */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_FR, ""},  /* fuzzy */
/* it */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_IT, "Non à possibile commutare tra IPV4 e IPV6 mentre netplay à attivo."},
/* nl */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_NL, "Kan niet schakelen tussen IPV4/IPV6 omdat netplay actief is."},
/* pl */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_PL, ""},  /* fuzzy */
/* sv */ {IDGS_CANNOT_SWITCH_IPV4_IPV6_SV, ""},  /* fuzzy */
#endif
#endif

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM,    "ffmpegdrv: Cannot open video stream"},
/* de */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_DE, "ffmpegdrv: Kann video stream nicht öffnen"},
/* fr */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_FR, ""},  /* fuzzy */
/* it */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_IT, "ffmpegdrv: Non à possibile aprire il flusso video"},
/* nl */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_NL, "ffmpegdrv: Kan de video stroom niet openen"},
/* pl */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_PL, ""},  /* fuzzy */
/* sv */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_SV, ""},  /* fuzzy */

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM,    "ffmpegdrv: Cannot open audio stream"},
/* de */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_DE, "ffmpegdrv: Kann audio stream nicht öffnen"},
/* fr */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_FR, ""},  /* fuzzy */
/* it */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_IT, "ffmpegdrv: Non à possibile aprire il flusso audio"},
/* nl */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_NL, "ffmpegdrv: Kan de audio stroom niet openen"},
/* pl */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_PL, ""},  /* fuzzy */
/* sv */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_SV, ""},  /* fuzzy */

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDGS_FFMPEG_CANNOT_OPEN_S,    "ffmpegdrv: Cannot open %s"},
/* de */ {IDGS_FFMPEG_CANNOT_OPEN_S_DE, "ffmpegdrv: Öffnen von %s fehlgeschlagen"},
/* fr */ {IDGS_FFMPEG_CANNOT_OPEN_S_FR, ""},  /* fuzzy */
/* it */ {IDGS_FFMPEG_CANNOT_OPEN_S_IT, "ffmpegdrv: Non à possibile aprire %s"},
/* nl */ {IDGS_FFMPEG_CANNOT_OPEN_S_NL, "ffmpegdrv: Kan %s niet openen"},
/* pl */ {IDGS_FFMPEG_CANNOT_OPEN_S_PL, ""},  /* fuzzy */
/* sv */ {IDGS_FFMPEG_CANNOT_OPEN_S_SV, ""},  /* fuzzy */

/* c64/c64io.c */
/* en */ {IDGS_IO_READ_COLL_AT_X_FROM,    "I/O read collision at %X from "},
/* de */ {IDGS_IO_READ_COLL_AT_X_FROM_DE, ""},  /* fuzzy */
/* fr */ {IDGS_IO_READ_COLL_AT_X_FROM_FR, ""},  /* fuzzy */
/* it */ {IDGS_IO_READ_COLL_AT_X_FROM_IT, ""},  /* fuzzy */
/* nl */ {IDGS_IO_READ_COLL_AT_X_FROM_NL, "I/O lees botsing op %X van "},
/* pl */ {IDGS_IO_READ_COLL_AT_X_FROM_PL, ""},  /* fuzzy */
/* sv */ {IDGS_IO_READ_COLL_AT_X_FROM_SV, ""},  /* fuzzy */

/* c64/c64io.c */
/* en */ {IDGS_AND,    " and "},
/* de */ {IDGS_AND_DE, ""},  /* fuzzy */
/* fr */ {IDGS_AND_FR, ""},  /* fuzzy */
/* it */ {IDGS_AND_IT, ""},  /* fuzzy */
/* nl */ {IDGS_AND_NL, " en "},
/* pl */ {IDGS_AND_PL, ""},  /* fuzzy */
/* sv */ {IDGS_AND_SV, ""},  /* fuzzy */

/* c64/c64io.c */
/* en */ {IDGS_ALL_DEVICES_DETACHED,    ".\nAll the named devices will be detached."},
/* de */ {IDGS_ALL_DEVICES_DETACHED_DE, ""},  /* fuzzy */
/* fr */ {IDGS_ALL_DEVICES_DETACHED_FR, ""},  /* fuzzy */
/* it */ {IDGS_ALL_DEVICES_DETACHED_IT, ""},  /* fuzzy */
/* nl */ {IDGS_ALL_DEVICES_DETACHED_NL, ".\nAlle genoemde apparaten zullen worden ontkoppelt."},
/* pl */ {IDGS_ALL_DEVICES_DETACHED_PL, ""},  /* fuzzy */
/* sv */ {IDGS_ALL_DEVICES_DETACHED_SV, ""},  /* fuzzy */

/* ------------------------ COMMAND LINE OPTION STRINGS -------------------- */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_USE_PAL_SYNC_FACTOR,    "Use PAL sync factor"},
/* de */ {IDCLS_USE_PAL_SYNC_FACTOR_DE, "PAL Faktor zum Synchronisieren benutzen"},
/* fr */ {IDCLS_USE_PAL_SYNC_FACTOR_FR, "Utiliser PAL"},
/* it */ {IDCLS_USE_PAL_SYNC_FACTOR_IT, "Usa il fattore di sincronizzazione PAL"},
/* nl */ {IDCLS_USE_PAL_SYNC_FACTOR_NL, "Gebruik PAL synchronisatie faktor"},
/* pl */ {IDCLS_USE_PAL_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_PAL_SYNC_FACTOR_SV, "Använd PAL-synkfaktor"},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_USE_NTSC_SYNC_FACTOR,    "Use NTSC sync factor"},
/* de */ {IDCLS_USE_NTSC_SYNC_FACTOR_DE, "NTSC Sync Faktor benutzen"},
/* fr */ {IDCLS_USE_NTSC_SYNC_FACTOR_FR, "Utiliser NTSC"},
/* it */ {IDCLS_USE_NTSC_SYNC_FACTOR_IT, "Usa il fattore di sincronizzazione NTSC"},
/* nl */ {IDCLS_USE_NTSC_SYNC_FACTOR_NL, "Gebruik NTSC synchronisatie faktor"},
/* pl */ {IDCLS_USE_NTSC_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_NTSC_SYNC_FACTOR_SV, "Använd NTSC-synkfaktor"},

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
/* en */ {IDCLS_P_NAME,    "<name>"},
/* de */ {IDCLS_P_NAME_DE, "<Name>"},
/* fr */ {IDCLS_P_NAME_FR, "<nom>"},
/* it */ {IDCLS_P_NAME_IT, "<nome>"},
/* nl */ {IDCLS_P_NAME_NL, "<naam>"},
/* pl */ {IDCLS_P_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_NAME_SV, "<namn>"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INT_KERNEL_NAME,    "Specify name of international Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_INT_KERNEL_NAME_DE, "Dateiname des internationalen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_INT_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal internationale"},
/* it */ {IDCLS_SPECIFY_INT_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal internazionale"},
/* nl */ {IDCLS_SPECIFY_INT_KERNEL_NAME_NL, "Geef de naam van het internationaal Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_INT_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INT_KERNEL_NAME_SV, "Ange namn på internationell kernal-ROM-avbildning"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_DE_KERNEL_NAME,    "Specify name of German Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_DE_KERNEL_NAME_DE, "Dateiname des deutschen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_DE_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal allemande"},
/* it */ {IDCLS_SPECIFY_DE_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal tedesca"},
/* nl */ {IDCLS_SPECIFY_DE_KERNEL_NAME_NL, "Geef de naam van het Duits Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_DE_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_DE_KERNEL_NAME_SV, "Ange namn på tysk kernal-ROM-avbildning"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FI_KERNEL_NAME,    "Specify name of Finnish Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_FI_KERNEL_NAME_DE, "Dateiname des finnischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_FI_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal finlandaise"},
/* it */ {IDCLS_SPECIFY_FI_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal finlandese"},
/* nl */ {IDCLS_SPECIFY_FI_KERNEL_NAME_NL, "Geef de naam van het Fins Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FI_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FI_KERNEL_NAME_SV, "Ange namn på finsk kernal-ROM-avbildning"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FR_KERNEL_NAME,    "Specify name of French Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_FR_KERNEL_NAME_DE, "Dateiname des französischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_FR_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal française"},
/* it */ {IDCLS_SPECIFY_FR_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal francese"},
/* nl */ {IDCLS_SPECIFY_FR_KERNEL_NAME_NL, "Geef de naam van het Frans Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FR_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FR_KERNEL_NAME_SV, "Ange namn på fransk kernal-ROM-avbildning"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_IT_KERNEL_NAME,    "Specify name of Italian Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_IT_KERNEL_NAME_DE, "Dateiname des italienischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_IT_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal italienne"},
/* it */ {IDCLS_SPECIFY_IT_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal italiana"},
/* nl */ {IDCLS_SPECIFY_IT_KERNEL_NAME_NL, "Geef de naam van het Italiaans Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_IT_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_IT_KERNEL_NAME_SV, "Ange namn på italiensk kernal-ROM-avbildning"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_NO_KERNEL_NAME,    "Specify name of Norwegain Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_NO_KERNEL_NAME_DE, "Dateiname des norwegischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_NO_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal norvégienne"},
/* it */ {IDCLS_SPECIFY_NO_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal norvegese"},
/* nl */ {IDCLS_SPECIFY_NO_KERNEL_NAME_NL, "Geef de naam van het Noors Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_NO_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_NO_KERNEL_NAME_SV, "Ange namn på norsk kernal-ROM-avbildning"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SV_KERNEL_NAME,    "Specify name of Swedish Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_SV_KERNEL_NAME_DE, "Dateiname des schwedischen Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_SV_KERNEL_NAME_FR, "Spécifier le nom de l'image Kernal suédoise"},
/* it */ {IDCLS_SPECIFY_SV_KERNEL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal svedese"},
/* nl */ {IDCLS_SPECIFY_SV_KERNEL_NAME_NL, "Geef de naam van het Zweeds Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_SV_KERNEL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SV_KERNEL_NAME_SV, "Ange namn på svensk kernal-ROM-avbildning"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW,    "Specify name of BASIC ROM image (lower part)"},
/* de */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_DE, "Dateiname des Basic ROMs (oberer Adressbereich"},
/* fr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_FR, "Spécifier le nom de l'image BASIC ROM (partie basse)"},
/* it */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_IT, "Specifica il nome dell'immagine della ROM del BASIC (parte inferiore)"},
/* nl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_NL, "Geef de naam van het BASIC ROM bestand (laag gedeelte)"},
/* pl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_SV, "Ange namn på BASIC ROM-avbildning (nedre delen)"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH,    "Specify name of BASIC ROM image (higher part)"},
/* de */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_DE, "Dateiname des Basic ROMs (unterer Adressbereich)"},
/* fr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_FR, "Spécifier le nom de l'image BASIC ROM (partie haute)"},
/* it */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_IT, "Specifica il nome dell'immagine della ROM del BASIC (parte superiore)"},
/* nl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_NL, "Geef de naam van het BASIC ROM bestand (hoog gedeelte)"},
/* pl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_SV, "Ange namn på BASIC ROM-avbildning (övre delen)"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME,    "Specify name of international character generator ROM image"},
/* de */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_DE, "Dateiname des internationalen Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_FR, "Spécifier le nom de l'image du générateur de caractères internationaux"},
/* it */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del generatore di caratteri internazionale"},
/* nl */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_NL, "Geef de naam van het internationaal CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_SV, "Ange namn på internationell teckengenerator-ROM-avbildning"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME,    "Specify name of German character generator ROM image"},
/* de */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_DE, "Dateiname des deutschen Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_FR, "Spécifier le nom de l'image du générateur de caractères allemands"},
/* it */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del generatore di caratteri tedesco"},
/* nl */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_NL, "Geef de naam van het Duits CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_SV, "Ange namn på tysk teckengenerator-ROM-avbildning"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME,    "Specify name of French character generator ROM image"},
/* de */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_DE, "Dateiname des französischen Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_FR, "Spécifier le nom de l'image du générateur de caractères français"},
/* it */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del generatore di caratteri francese"},
/* nl */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_NL, "Geef de naam van het Frans CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_SV, "Ange namn på fransk teckengenerator-ROM-avbildning"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME,    "Specify name of Swedish character generator ROM image"},
/* de */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_DE, "Dateiname des swedischen Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_FR, "Spécifier le nom de l'image du générateur de caractères suédois"},
/* it */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del generatore di caratteri svedese"},
/* nl */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_NL, "Geef de naam van het Zweeds CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_SV, "Ange namn på svensk teckengenerator-ROM-avbildning"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME,    "Specify name of C64 mode Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_DE, "Dateiname des Kernal ROMs im C64 Modus"},
/* fr */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_FR, "Spécifier le nom de l'image Kernal du mode C64"},
/* it */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal in modalità C64"},
/* nl */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_NL, "Geef de naam van het C64 modus Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_SV, "Ange namn på C64-kernal-ROM-avbildning"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME,    "Specify name of C64 mode BASIC ROM image"},
/* de */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_DE, "Dateiname des Basic ROMs im C64 Modus"}, 
/* fr */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_FR, "Spécifier le nom de l'image BASIC ROM du mode C64"},
/* it */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_IT, "Specifica il nome dell'immagine della ROM in modalità C64"},
/* nl */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_NL, "Geef de naam van het C64 modus BASIC ROM bestand"},
/* pl */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_C64_MODE_BASIC_NAME_SV, "Ange namn på C64-BASIC-ROM-avbildning"},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_ENABLE_EMULATOR_ID,    "Enable emulator identification"},
/* de */ {IDCLS_ENABLE_EMULATOR_ID_DE, "Emulatoridentifikation aktivieren"},
/* fr */ {IDCLS_ENABLE_EMULATOR_ID_FR, "Activer l'identification de l'émulateur"},
/* it */ {IDCLS_ENABLE_EMULATOR_ID_IT, "Attiva l'identificazione dell'emulatore"},
/* nl */ {IDCLS_ENABLE_EMULATOR_ID_NL, "Aktiveer emulator identificatie"},
/* pl */ {IDCLS_ENABLE_EMULATOR_ID_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EMULATOR_ID_SV, "Aktivera emulatoridentifiering"},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_DISABLE_EMULATOR_ID,    "Disable emulator identification"},
/* de */ {IDCLS_DISABLE_EMULATOR_ID_DE, "Emulatoridentifikation deaktivieren"},
/* fr */ {IDCLS_DISABLE_EMULATOR_ID_FR, "Désactiver l'identification de l'émulateur"},
/* it */ {IDCLS_DISABLE_EMULATOR_ID_IT, "Disattiva l'identificazione dell'emulatore"},
/* nl */ {IDCLS_DISABLE_EMULATOR_ID_NL, "Emulator identificatie afsluiten"},
/* pl */ {IDCLS_DISABLE_EMULATOR_ID_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_EMULATOR_ID_SV, "Inaktivera emulatoridentifiering"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_ENABLE_IEE488,    "Enable the IEEE488 interface emulation"},
/* de */ {IDCLS_ENABLE_IEE488_DE, "IEEE488 Schnittstellenemulation aktivieren"},
/* fr */ {IDCLS_ENABLE_IEE488_FR, "Activer l'interface d'émulation IEEE488"},
/* it */ {IDCLS_ENABLE_IEE488_IT, "Attiva l'emulazione dell'interfaccia IEEE488"},
/* nl */ {IDCLS_ENABLE_IEE488_NL, "Aktiveer de IEEE488 interface emulatie"},
/* pl */ {IDCLS_ENABLE_IEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEE488_SV, "Aktivera emulering av IEEE488-gränssnitt"},

/* c128/c128-cmdline-options.c */
/* en */ {IDCLS_DISABLE_IEE488,    "Disable the IEEE488 interface emulation"},
/* de */ {IDCLS_DISABLE_IEE488_DE, "IEEE488 Schnittstellenemulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEE488_FR, "Désactiver l'interface d'émulation IEEE488"},
/* it */ {IDCLS_DISABLE_IEE488_IT, "Disattiva l'emulazione dell'interfaccia IEEE488"},
/* nl */ {IDCLS_DISABLE_IEE488_NL, "De IEEE488 interface emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_IEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEE488_SV, "Inaktivera emulering av IEEE488-gränssnitt"},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_P_REVISION,    "<revision>"},
/* de */ {IDCLS_P_REVISION_DE, "<Revision>"},
/* fr */ {IDCLS_P_REVISION_FR, "<révision>"},
/* it */ {IDCLS_P_REVISION_IT, "<revisione>"},
/* nl */ {IDCLS_P_REVISION_NL, "<revisie>"},
/* pl */ {IDCLS_P_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_REVISION_SV, "<utgåva>"},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_PATCH_KERNAL_TO_REVISION,    "Patch the Kernal ROM to the specified <revision>"},
/* de */ {IDCLS_PATCH_KERNAL_TO_REVISION_DE, "Kernal ROM auf angegenbene Revision ändern"},
/* fr */ {IDCLS_PATCH_KERNAL_TO_REVISION_FR, "Mettre à jour le ROM Kernal à la <révision> spécifiée"},
/* it */ {IDCLS_PATCH_KERNAL_TO_REVISION_IT, "Fai il patch della ROM del Kernal alla <revisione> specificata"},
/* nl */ {IDCLS_PATCH_KERNAL_TO_REVISION_NL, "Verbeter de Kernal ROM naar de opgegeven <revisie>"},
/* pl */ {IDCLS_PATCH_KERNAL_TO_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PATCH_KERNAL_TO_REVISION_SV, "Patcha kernal-ROM till angiven <utgåva>"},

#ifdef HAVE_RS232
/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU,    "Enable the $DE** ACIA RS232 interface emulation"},
/* de */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_DE, "$DE** ACIA RS232 Schnittstellenemulation aktivieren"},
/* fr */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_FR, "Activer l'émulation de l'interface ACIA $DE** RS232"},
/* it */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_IT, "Attiva l'emulazione dell'interfaccia RS232 su ACIA a $DE**"},
/* nl */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_NL, "Aktiveer de $DE** ACIA RS232 interface emulatie"},
/* pl */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_SV, "Aktivera $DE** ACIA RS232-gränssnittsemulering"},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU,    "Disable the $DE** ACIA RS232 interface emulation"},
/* de */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_DE, "$DE** ACIA RS232 Schnittstellenemulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_FR, "Désactiver l'émulation de l'interface $DE** ACIA RS232"},
/* it */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_IT, "Disattiva l'emulazione dell'interfaccia RS232 su ACIA a $DE**"},
/* nl */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_NL, "De $DE** ACIA RS232 interface emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_SV, "Inaktivera $DE** ACIA-RS232-gränssnittsemulering"},
#endif

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vdc/vdc-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_P_NUMBER,    "<number>"},
/* de */ {IDCLS_P_NUMBER_DE, "<Nummer>"},
/* fr */ {IDCLS_P_NUMBER_FR, "<numéro>"},
/* it */ {IDCLS_P_NUMBER_IT, "<numero>"},
/* nl */ {IDCLS_P_NUMBER_NL, "<nummer>"},
/* pl */ {IDCLS_P_NUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_NUMBER_SV, "<nummer>"},

#ifdef COMMON_KBD
/* c128/c128-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX,    "Specify index of keymap file (0=symbol, 1=positional)"},
/* de */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_DE, "Aktive Tastaturbelegung (0=symbolisch) (1=positionell)"},
/* fr */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_FR, "Spécifier l'index du fichier keymap (0=symboles, 1=position)"},
/* it */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_IT, "Specifica l'indice del file della mappa della tastiera (0=simbolico, 1=posizionale)"},
/* nl */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_NL, "Geef index van het keymap bestand (0=symbool, 1=positioneel)"},
/* pl */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_KEYMAP_FILE_INDEX_SV, "Ange index för för tangentbordsinställningsfil (0=symbolisk, 1=positionsriktig)"},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   plus4/plus4-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME,    "Specify name of symbolic keymap file"},
/* de */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_DE, "Dateiname für symbolische Tastaturbelegung"},
/* fr */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_FR, "Spécifier le nom du fichier de mappage clavier symbolique"},
/* it */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_IT, "Specifica il nome del file della mappa simbolica della tastiera"},
/* nl */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_NL, "Geef naam van het symbolisch keymap bestand"},
/* pl */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_SV, "Ange fil för symbolisk tangentbordsemulering"},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   plus4/plus4-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME,    "Specify name of positional keymap file"},
/* de */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_DE, "Dateiname für positionelle Tastaturbelegung"},
/* fr */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_FR, "Spécifier le nom du fichier de mappage clavier positionnel"},
/* it */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_IT, "Specifica il nome del file della mappa posizionale della tastiera"},
/* nl */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_NL, "Geef naam van het positioneel keymap bestand"},
/* pl */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_SV, "Ange fil för positionsriktig tangentbordsemulering"},
#endif

/* c128/c128mmu.c */
/* en */ {IDCLS_ACTIVATE_40_COL_MODE,    "Activate 40 column mode"},
/* de */ {IDCLS_ACTIVATE_40_COL_MODE_DE, "40 Zeichen Modus"},
/* fr */ {IDCLS_ACTIVATE_40_COL_MODE_FR, "Activer le mode 40 colonnes"},
/* it */ {IDCLS_ACTIVATE_40_COL_MODE_IT, "Attiva la modalità a 40 colonne"},
/* nl */ {IDCLS_ACTIVATE_40_COL_MODE_NL, "Aktiveer 40 kolommen modus"},
/* pl */ {IDCLS_ACTIVATE_40_COL_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ACTIVATE_40_COL_MODE_SV, "Aktivera 40-kolumnersläge"},

/* c128/c128mmu.c */
/* en */ {IDCLS_ACTIVATE_80_COL_MODE,    "Activate 80 column mode"},
/* de */ {IDCLS_ACTIVATE_80_COL_MODE_DE, "80 Zeichen Modus"},
/* fr */ {IDCLS_ACTIVATE_80_COL_MODE_FR, "Activer le mode 80 colonnes"},
/* it */ {IDCLS_ACTIVATE_80_COL_MODE_IT, "Attiva la modalità a 80 colonne"},
/* nl */ {IDCLS_ACTIVATE_80_COL_MODE_NL, "Aktiveer 80 kolommen modus"},
/* pl */ {IDCLS_ACTIVATE_80_COL_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ACTIVATE_80_COL_MODE_SV, "Aktivera 80-kolumnersläge"},

/* c128/functionrom.c */
/* en */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME,    "Specify name of internal Function ROM image"},
/* de */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_DE, "Dateiname des internen Funktions ROMs"},
/* fr */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_FR, "Spécifier le nom de l'image ROM des fonctions internes"},
/* it */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_IT, "Specifica il nome dell'immagine della Function ROM interna"},
/* nl */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_NL, "Geef de naam van het intern Functie ROM bestand"},
/* pl */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_SV, "Ange namn på intern funktions-ROM-avbildning"},

/* c128/functionrom.c */
/* en */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME,    "Specify name of external Function ROM image"},
/* de */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_DE, "Dateiname des externen Funktions ROMs"},
/* fr */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_FR, "Spécifier le nom de l'image ROM des fonctions externes"},
/* it */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_IT, "Specifica il nome dell'immagine della Function ROM esterna"},
/* nl */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_NL, "Geef de naam van het extern Functie ROM bestand"},
/* pl */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_SV, "Ange namn på extern funktions-ROM-avbildning"},

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_INT_FUNC_ROM,    "Enable the internal Function ROM"},
/* de */ {IDCLS_ENABLE_INT_FUNC_ROM_DE, "Internes Funktions ROM aktivieren"},
/* fr */ {IDCLS_ENABLE_INT_FUNC_ROM_FR, "Activer l'image ROM des fonctions internes"},
/* it */ {IDCLS_ENABLE_INT_FUNC_ROM_IT, "Attiva la Function ROM interna"},
/* nl */ {IDCLS_ENABLE_INT_FUNC_ROM_NL, "Aktiveer de interne Functie ROM"},
/* pl */ {IDCLS_ENABLE_INT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_INT_FUNC_ROM_SV, "Aktivera internt funktions-ROM"},

/* c128/functionrom.c */
/* en */ {IDCLS_DISABLE_INT_FUNC_ROM,    "Disable the internal Function ROM"},
/* de */ {IDCLS_DISABLE_INT_FUNC_ROM_DE, "Internes Funktions ROM deaktivieren"},
/* fr */ {IDCLS_DISABLE_INT_FUNC_ROM_FR, "Désactiver l'image ROM des fonctions internes"},
/* it */ {IDCLS_DISABLE_INT_FUNC_ROM_IT, "Disattiva la Function ROM interna"},
/* nl */ {IDCLS_DISABLE_INT_FUNC_ROM_NL, "De interne Functie ROM afsluiten"},
/* pl */ {IDCLS_DISABLE_INT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_INT_FUNC_ROM_SV, "Inaktivera internt funktions-ROM"},

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_EXT_FUNC_ROM,    "Enable the external Function ROM"},
/* de */ {IDCLS_ENABLE_EXT_FUNC_ROM_DE, "Externes Funktions ROM aktivieren"},
/* fr */ {IDCLS_ENABLE_EXT_FUNC_ROM_FR, "Activer l'image ROM des fonctions externes"},
/* it */ {IDCLS_ENABLE_EXT_FUNC_ROM_IT, "Attiva la Function ROM esterna"},
/* nl */ {IDCLS_ENABLE_EXT_FUNC_ROM_NL, "Aktiveer de externe Functie ROM"},
/* pl */ {IDCLS_ENABLE_EXT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EXT_FUNC_ROM_SV, "Aktivera externt funktions-ROM"},

/* c128/functionrom.c */
/* en */ {IDCLS_DISABLE_EXT_FUNC_ROM,    "Disable the external Function ROM"},
/* de */ {IDCLS_DISABLE_EXT_FUNC_ROM_DE, "Externes Funktions ROM deaktivieren"},
/* fr */ {IDCLS_DISABLE_EXT_FUNC_ROM_FR, "Désactiver l'image ROM des fonctions externes"},
/* it */ {IDCLS_DISABLE_EXT_FUNC_ROM_IT, "Disattiva la Function ROM esterna"},
/* nl */ {IDCLS_DISABLE_EXT_FUNC_ROM_NL, "De externe Functie ROM afsluiten"},
/* pl */ {IDCLS_DISABLE_EXT_FUNC_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_EXT_FUNC_ROM_SV, "Inaktivera externt funktions-ROM"},

/* c64/c64-cmdline-options.c, c64/psid.c */
/* en */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR,    "Use old NTSC sync factor"},
/* de */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_DE, "NTSC (alt) Faktor zum Synchronisieren benutzen"},
/* fr */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_FR, "Utiliser l'ancien mode NTSC"},
/* it */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_IT, "Usa il vecchio fattore di sincronizzazione NTSC"},
/* nl */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_NL, "Gebruik oude NTSC synchronisatie faktor"},
/* pl */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_SV, "Använd gammal NTSC-synkfaktor"},

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KERNAL_ROM_NAME,    "Specify name of Kernal ROM image"},
/* de */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_DE, "Dateiname des Kernal ROMs"},
/* fr */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_FR, "Spécifier le nom de l'image ROM Kernal"},
/* it */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del Kernal"},
/* nl */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_NL, "Geef de naam van het Kernal ROM bestand"},
/* pl */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_KERNAL_ROM_NAME_SV, "Ange namn på kernal-ROM-avbildning"},

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BASIC_ROM_NAME,    "Specify name of BASIC ROM image"},
/* de */ {IDCLS_SPECIFY_BASIC_ROM_NAME_DE, "Dateiname des Basic ROMs"},
/* fr */ {IDCLS_SPECIFY_BASIC_ROM_NAME_FR, "Spécifier le nom de l'image BASIC ROM"},
/* it */ {IDCLS_SPECIFY_BASIC_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del BASIC"},
/* nl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_NL, "Geef de naam van het BASIC ROM bestand"},
/* pl */ {IDCLS_SPECIFY_BASIC_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BASIC_ROM_NAME_SV, "Ange namn på BASIC-ROM-avbildning"},

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME,    "Specify name of character generator ROM image"},
/* de */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_DE, "Dateiname des Zeichensatz ROMs"},
/* fr */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_FR, "Spécifier le nom de l'image du générateur de caractères"},
/* it */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_IT, "Specifica il nome della ROM del generatore di caratteri"},
/* nl */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_NL, "Geef de naam van het CHARGEN ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CHARGEN_ROM_NAME_SV, "Ange namn på teckengenerator-ROM-avbildning"},

#ifdef COMMON_KBD
/* c64/c64-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2,    "Specify index of keymap file (0=sym, 1=symDE, 2=pos)"},
/* de */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_DE, "Index für Keymap Datei festlegen (0=symbol, 1=symDE, 2=positional)"},
/* fr */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_FR, "Spécifier l'index du fichier keymap (0=sym, 1=symDE, 2=pos)"},
/* it */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_IT, "Specifica l'indice del file della mappa della tastiera (0=sim, 1=simGER, 2=pos)"},
/* nl */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_NL, "Geef de index van het keymap bestand (0=sym, 1=symDE, 2=pos)"},
/* pl */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_SV, "Ange index för för tangentbordsinställningsfil (0=symbolisk, 1=symbolisk tysk, 2=positionsriktig)"},

/* c64/c64-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP,    "Specify name of symbolic German keymap file"},
/* de */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_DE, "Name von symbolischer Keymap Datei definieren"},
/* fr */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_IT, "Specifica il nome del file della mappa simbolica della tastiera tedesca"},
/* nl */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_NL, "Geef de naam van het symbolisch duits keymap bestand"},
/* pl */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_SV, "Ange fil för tysk symbolisk tangentbordsemulering"},
#endif

/* c64/georam.c */
/* en */ {IDCLS_ENABLE_GEORAM,    "Enable the GEORAM expansion unit"},
/* de */ {IDCLS_ENABLE_GEORAM_DE, "GEORAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_GEORAM_FR, "Activer l'unité d'expansion GEORAM"},
/* it */ {IDCLS_ENABLE_GEORAM_IT, "Attiva l'espansione GEORAM"},
/* nl */ {IDCLS_ENABLE_GEORAM_NL, "Aktiveer de GEORAM uitbreidings eenheid"},
/* pl */ {IDCLS_ENABLE_GEORAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_GEORAM_SV, "Aktivera GEORAM-expansionsenhet"},

/* c64/georam.c */
/* en */ {IDCLS_DISABLE_GEORAM,    "Disable the GEORAM expansion unit"},
/* de */ {IDCLS_DISABLE_GEORAM_DE, "GEORAM Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_GEORAM_FR, "Désactiver l'unité d'expansion GEORAM"},
/* it */ {IDCLS_DISABLE_GEORAM_IT, "Disattiva l'espansione GEORAM"},
/* nl */ {IDCLS_DISABLE_GEORAM_NL, "De GEORAM uitbreidings eenheid aflsuiten"},
/* pl */ {IDCLS_DISABLE_GEORAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_GEORAM_SV, "Inaktivera GEORAM-expansionsenhet"},

/* c64/georam.c */
/* en */ {IDCLS_SPECIFY_GEORAM_NAME,    "Specify name of GEORAM image"},
/* de */ {IDCLS_SPECIFY_GEORAM_NAME_DE, "Dateiname für GEORAM Erweiterung"},
/* fr */ {IDCLS_SPECIFY_GEORAM_NAME_FR, "Spécifier le nom de l'image GEORAM"},
/* it */ {IDCLS_SPECIFY_GEORAM_NAME_IT, "Specifica il nome dell'immagine GEORAM"},
/* nl */ {IDCLS_SPECIFY_GEORAM_NAME_NL, "Geef de naam van het GEORAM bestand"},
/* pl */ {IDCLS_SPECIFY_GEORAM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_GEORAM_NAME_SV, "Ange namn på GEORAM-avbildning"},

/* c64/georam.c, c64/ramcart.c, c64/reu.c, pet/petreu.c */
/* en */ {IDCLS_P_SIZE_IN_KB,    "<size in KB>"},
/* de */ {IDCLS_P_SIZE_IN_KB_DE, "<Grösse in KB>"},
/* fr */ {IDCLS_P_SIZE_IN_KB_FR, "<taille en KO>"},
/* it */ {IDCLS_P_SIZE_IN_KB_IT, "<dimensione in KB>"},
/* nl */ {IDCLS_P_SIZE_IN_KB_NL, "<grootte in KB>"},
/* pl */ {IDCLS_P_SIZE_IN_KB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_SIZE_IN_KB_SV, "<storlek i KB>"},

/* c64/georam.c */
/* en */ {IDCLS_GEORAM_SIZE,    "Size of the GEORAM expansion unit"},
/* de */ {IDCLS_GEORAM_SIZE_DE, "Grösse der GEORAM Erweiterung"},
/* fr */ {IDCLS_GEORAM_SIZE_FR, "Taille de l'unité d'expansion GEORAM"},
/* it */ {IDCLS_GEORAM_SIZE_IT, "Dimensione dell'espansione GEORAM"},
/* nl */ {IDCLS_GEORAM_SIZE_NL, "Grootte van de GEORAM uitbreidings eenheid"},
/* pl */ {IDCLS_GEORAM_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_GEORAM_SIZE_SV, "Storlek på GEORAM-expansionsenhet"},

/* pet/petreu.c */
/* en */ {IDCLS_ENABLE_PETREU,    "Enable the PET Ram and Expansion Unit"},
/* de */ {IDCLS_ENABLE_PETREU_DE, "PET RAM Speicher und Erweiterungsmodul aktivieren"},
/* fr */ {IDCLS_ENABLE_PETREU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_PETREU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_PETREU_NL, "Aktiveer de PET RAM en uitbreidings eenheid"},
/* pl */ {IDCLS_ENABLE_PETREU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_PETREU_SV, ""},  /* fuzzy */

/* pet/petreu.c */
/* en */ {IDCLS_DISABLE_PETREU,    "Disable the PET Ram and Expansion Unit"},
/* de */ {IDCLS_DISABLE_PETREU_DE, "PET RAM Speicher und Erweiterungsmodul dektivieren"},
/* fr */ {IDCLS_DISABLE_PETREU_FR, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_PETREU_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_PETREU_NL, "De PET RAM en uitbreidings eenheid afsluiten"},
/* pl */ {IDCLS_DISABLE_PETREU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_PETREU_SV, ""},  /* fuzzy */

/* pet/petreu.c */
/* en */ {IDCLS_SPECIFY_PETREU_NAME,    "Specify name of PET Ram and Expansion Unit image"},
/* de */ {IDCLS_SPECIFY_PETREU_NAME_DE, "Namen für PET RAM Speicher und Erweiterungsmodul definieren"},
/* fr */ {IDCLS_SPECIFY_PETREU_NAME_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PETREU_NAME_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PETREU_NAME_NL, "Geef de naam van het PET RAM en uitbreidings eenheid bestand"},
/* pl */ {IDCLS_SPECIFY_PETREU_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PETREU_NAME_SV, ""},  /* fuzzy */

/* pet/petreu.c */
/* en */ {IDCLS_PETREU_SIZE,    "Size of the PET Ram and Expansion Unit"},
/* de */ {IDCLS_PETREU_SIZE_DE, "Größe des PET RAM Speicher und Erweiterungsmodul"},
/* fr */ {IDCLS_PETREU_SIZE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_PETREU_SIZE_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_PETREU_SIZE_NL, "Grootte van de PET RAM en uitbreidings eenheid"},
/* pl */ {IDCLS_PETREU_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PETREU_SIZE_SV, ""},  /* fuzzy */

/* c64/psid.c */
/* en */ {IDCLS_SID_PLAYER_MODE,    "SID player mode"},
/* de */ {IDCLS_SID_PLAYER_MODE_DE, "SID player Modus"},
/* fr */ {IDCLS_SID_PLAYER_MODE_FR, "Mode jukebox SID"},
/* it */ {IDCLS_SID_PLAYER_MODE_IT, "Modalità del SID player"},
/* nl */ {IDCLS_SID_PLAYER_MODE_NL, "SID player modus"},
/* pl */ {IDCLS_SID_PLAYER_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SID_PLAYER_MODE_SV, "SID-spelarläge"},

/* c64/psid.c */
/* en */ {IDCLS_OVERWRITE_PSID_SETTINGS,    "Override PSID settings for Video standard and SID model"},
/* de */ {IDCLS_OVERWRITE_PSID_SETTINGS_DE, "Überschreiben der PSID Einstellungen bezüglich Videostandard und SID Modell"},
/* fr */ {IDCLS_OVERWRITE_PSID_SETTINGS_FR, "Écraser les paramètres PSID pour le standard vidéo et le modèle SID"},
/* it */ {IDCLS_OVERWRITE_PSID_SETTINGS_IT, "Non tener conto delle impostazioni del PSID per lo standard video e il modello di SID"},
/* nl */ {IDCLS_OVERWRITE_PSID_SETTINGS_NL, "Overschrijf PSID instellingen voor Video standaard en SID model"},
/* pl */ {IDCLS_OVERWRITE_PSID_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_OVERWRITE_PSID_SETTINGS_SV, "Ersätt PSID-inställningar för videostandard och SID-modell"},

/* c64/psid.c */
/* en */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER,    "Specify PSID tune <number>"},
/* de */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_DE, "PSID Stück <Nummer> definieren"},
/* fr */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_FR, "Spécifier <numéro> piste PSID"},
/* it */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_IT, "Specifica la melodia <numero> del PSID"},
/* nl */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_NL, "Geef PSID muziek <nummer>"},
/* pl */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_SV, "Ange PSID-låt <nummer>"},

/* c64/ramcart.c */
/* en */ {IDCLS_ENABLE_RAMCART,    "Enable the RAMCART expansion"},
/* de */ {IDCLS_ENABLE_RAMCART_DE, "RAMCART Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_RAMCART_FR, "Activer l'expansion RAMCART"},
/* it */ {IDCLS_ENABLE_RAMCART_IT, "Attiva l'espansione RAMCART"},
/* nl */ {IDCLS_ENABLE_RAMCART_NL, "Aktiveer de RAMCART uitbreiding"},
/* pl */ {IDCLS_ENABLE_RAMCART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAMCART_SV, "Aktivera RAMCART-expansion"},

/* c64/ramcart.c */
/* en */ {IDCLS_DISABLE_RAMCART,    "Disable the RAMCART expansion"},
/* de */ {IDCLS_DISABLE_RAMCART_DE, "RAMCART Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAMCART_FR, "Désactiver l'expansion RAMCART"},
/* it */ {IDCLS_DISABLE_RAMCART_IT, "Disattiva l'espansione RAMCART"},
/* nl */ {IDCLS_DISABLE_RAMCART_NL, "De RAMCART uitbreiding afsluiten"},
/* pl */ {IDCLS_DISABLE_RAMCART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAMCART_SV, "Inaktivera RAMCART-expansion"},

/* c64/ramcart.c */
/* en */ {IDCLS_SPECIFY_RAMCART_NAME,    "Specify name of RAMCART image"},
/* de */ {IDCLS_SPECIFY_RAMCART_NAME_DE, "Dateiname für RAMCART Erweiterung"},
/* fr */ {IDCLS_SPECIFY_RAMCART_NAME_FR, "Spécifier le nom de l'image RAMCART"},
/* it */ {IDCLS_SPECIFY_RAMCART_NAME_IT, "Specifica il nome dell'immagine RAMCART"},
/* nl */ {IDCLS_SPECIFY_RAMCART_NAME_NL, "Geef de naam van het RAMCART bestand"},
/* pl */ {IDCLS_SPECIFY_RAMCART_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RAMCART_NAME_SV, "Ange namn på RAMCART-avbildning"},

/* c64/ramcart.c */
/* en */ {IDCLS_RAMCART_SIZE,    "Size of the RAMCART expansion"},
/* de */ {IDCLS_RAMCART_SIZE_DE, "Grösse der RAMCART Erweiterung"},
/* fr */ {IDCLS_RAMCART_SIZE_FR, "Taille de l'expansion RAMCART"},
/* it */ {IDCLS_RAMCART_SIZE_IT, "Dimensione dell'espansione RAMCART"},
/* nl */ {IDCLS_RAMCART_SIZE_NL, "Grootte van de RAMCART uitbreiding"},
/* pl */ {IDCLS_RAMCART_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RAMCART_SIZE_SV, "Storlek på RAMCART-expansion"},

/* c64/reu.c */
/* en */ {IDCLS_ENABLE_REU,    "Enable the RAM expansion unit"},
/* de */ {IDCLS_ENABLE_REU_DE, "REU Speichererweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_REU_FR, "Activer l'unité d'expansion RAM"},
/* it */ {IDCLS_ENABLE_REU_IT, "Attiva l'espansione di RAM"},
/* nl */ {IDCLS_ENABLE_REU_NL, "Aktiveer de REU uitbreiding"},
/* pl */ {IDCLS_ENABLE_REU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_REU_SV, "Aktivera RAM-expansionsenhet"},

/* c64/reu.c */
/* en */ {IDCLS_DISABLE_REU,    "Disable the RAM expansion unit"},
/* de */ {IDCLS_DISABLE_REU_DE, "REU Speichererweiterung deaktivieren"}, 
/* fr */ {IDCLS_DISABLE_REU_FR, "Désactiver l'unité d'expansion RAM"},
/* it */ {IDCLS_DISABLE_REU_IT, "Disattiva l'espansione di RAM"},
/* nl */ {IDCLS_DISABLE_REU_NL, "De REU uitbreiding afsluiten"},
/* pl */ {IDCLS_DISABLE_REU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_REU_SV, "Inaktivera RAM-expansionsenhet"},

/* c64/reu.c */
/* en */ {IDCLS_SPECIFY_REU_NAME,    "Specify name of REU image"},
/* de */ {IDCLS_SPECIFY_REU_NAME_DE, "Dateiname für REU Speichererweiterung"},
/* fr */ {IDCLS_SPECIFY_REU_NAME_FR, "Spécifier le nom de l'image REU"},
/* it */ {IDCLS_SPECIFY_REU_NAME_IT, "Specifica il nome dell'immagine REU"},
/* nl */ {IDCLS_SPECIFY_REU_NAME_NL, "Geef de naam van het REU bestand"},
/* pl */ {IDCLS_SPECIFY_REU_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_REU_NAME_SV, "Ange namn på REU-avbildning"},

/* c64/reu.c */
/* en */ {IDCLS_REU_SIZE,    "Size of the RAM expansion unit"},
/* de */ {IDCLS_REU_SIZE_DE, "Grösse der REU Speichererweiterung"},
/* fr */ {IDCLS_REU_SIZE_FR, "Taille de l'unité d'expansion RAM"},
/* it */ {IDCLS_REU_SIZE_IT, "Dimensione dell'espansione di RAM"},
/* nl */ {IDCLS_REU_SIZE_NL, "Grootte van de REU uitbreiding"},
/* pl */ {IDCLS_REU_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_REU_SIZE_SV, "Storlek på RAM-expansionsenhet"},

#ifdef HAVE_TFE
/* c64/tfe.c */
/* en */ {IDCLS_ENABLE_TFE,    "Enable the TFE (\"The Final Ethernet\") unit"},
/* de */ {IDCLS_ENABLE_TFE_DE, "TFE Ethernetemulation aktivieren"},
/* fr */ {IDCLS_ENABLE_TFE_FR, "Activer l'unité TFE (\"The Final Ethernet\")"},
/* it */ {IDCLS_ENABLE_TFE_IT, "Attiva l'unità TFEÂ·(\"theÂ·finalÂ·ethernet\")"},
/* nl */ {IDCLS_ENABLE_TFE_NL, "Aktiveer de TFE (\"The Final Ethernet\") eenheid"},
/* pl */ {IDCLS_ENABLE_TFE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TFE_SV, "Aktivera TFE-enhet (\"the final ethernet\")"},

/* c64/tfe.c */
/* en */ {IDCLS_DISABLE_TFE,    "Disable the TFE (\"The Final Ethernet\") unit"},
/* de */ {IDCLS_DISABLE_TFE_DE, "TFE Ethernetemulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_TFE_FR, "Désactiver l'unité TFE"},
/* it */ {IDCLS_DISABLE_TFE_IT, "Disattiva l'unità TFEÂ·(\"theÂ·finalÂ·ethernet\")"},
/* nl */ {IDCLS_DISABLE_TFE_NL, "De TFE (\"The Final Ethernet\") eenheid afsluiten"},
/* pl */ {IDCLS_DISABLE_TFE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_TFE_SV, "Inaktivera TFE-enhet (\"the final ethernet\")"},
#endif

/* c64/cart/c64cart.c */
/* en */ {IDCLS_CART_ATTACH_DETACH_RESET,    "Reset machine if a cartridge is attached or detached"},
/* de */ {IDCLS_CART_ATTACH_DETACH_RESET_DE, "Reset der Maschine wenn ein Erweiterungsmodul eingelegt oder entfernt wird"},
/* fr */ {IDCLS_CART_ATTACH_DETACH_RESET_FR, "Réinitialiser si une cartouche est insérée ou retirée"},
/* it */ {IDCLS_CART_ATTACH_DETACH_RESET_IT, "Fare il reset della macchina se una cartuccia è collegata o rimossa"},
/* nl */ {IDCLS_CART_ATTACH_DETACH_RESET_NL, "Reset machine als een cartridge wordt gekoppelt of ontkoppelt"},
/* pl */ {IDCLS_CART_ATTACH_DETACH_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_CART_ATTACH_DETACH_RESET_SV, "Nollställ maskinen om en insticksmodul kopplas till eller från"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_CART_ATTACH_DETACH_NO_RESET,    "Do not reset machine if a cartridge is attached or detached"},
/* de */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_DE, "Kein Reset der Maschine wenn ein Erweiterungsmodul eingelegt oder entfernt wird"},
/* fr */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_FR, "Ne pas réinitialiser si une cartouche est insérée ou retirée"},
/* it */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_IT, "Non fare il reset della macchina se una cartuccia è collegata o rimossa"},
/* nl */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_NL, "Reset machine niet als een cartridge wordt gekoppelt of ontkoppelt"},
/* pl */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_CART_ATTACH_DETACH_NO_RESET_SV, "Nollställ inte maskinen om en insticksmodul kopplas till eller från"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_CRT_CART,    "Attach CRT cartridge image"},
/* de */ {IDCLS_ATTACH_CRT_CART_DE, "CRT Erweiterungsmodul Image einlegen"},
/* fr */ {IDCLS_ATTACH_CRT_CART_FR, "Insérer une cartouche CRT"},
/* it */ {IDCLS_ATTACH_CRT_CART_IT, "Seleziona l'immagine di una cartuccia CRT"},
/* nl */ {IDCLS_ATTACH_CRT_CART_NL, "Koppel CRT cartridge bestand"},
/* pl */ {IDCLS_ATTACH_CRT_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_CRT_CART_SV, "Anslut CRT-insticksmodulfil"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_GENERIC_8KB_CART,    "Attach generic 8KB cartridge image"},
/* de */ {IDCLS_ATTACH_GENERIC_8KB_CART_DE, "Generisches 8K Erweiterungsmodul Image einlegen"},
/* fr */ {IDCLS_ATTACH_GENERIC_8KB_CART_FR, "Insérer une cartouche générique 8KO"},
/* it */ {IDCLS_ATTACH_GENERIC_8KB_CART_IT, "Seleziona l'immagine di una cartuccia generica di 8KB"},
/* nl */ {IDCLS_ATTACH_GENERIC_8KB_CART_NL, "Koppel algemeen 8KB cartridge bestand"},
/* pl */ {IDCLS_ATTACH_GENERIC_8KB_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_GENERIC_8KB_CART_SV, "Anslut vanlig 8KB-insticksmodulfil"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_GENERIC_16KB_CART,    "Attach generic 16KB cartridge image"},
/* de */ {IDCLS_ATTACH_GENERIC_16KB_CART_DE, "Generisches 16K Erweiterungsmodul Image einlegen"},
/* fr */ {IDCLS_ATTACH_GENERIC_16KB_CART_FR, "Insérer une cartouche générique 16KO"},
/* it */ {IDCLS_ATTACH_GENERIC_16KB_CART_IT, "Seleziona l'immagine di una cartuccia generica di 16KB"},
/* nl */ {IDCLS_ATTACH_GENERIC_16KB_CART_NL, "Koppel algemeen 16KB cartridge bestand"},
/* pl */ {IDCLS_ATTACH_GENERIC_16KB_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_GENERIC_16KB_CART_SV, "Anslut vanlig 16KB-insticksmodulfil"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART,    "Attach raw 32KB Action Replay cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_DE, "32K Action Replay Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_FR, "Insérer une cartouche Action Replay 32KO"},
/* it */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_IT, "Seleziona l'immagine di una cartuccia Action Replay di 32KB"},
/* nl */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_NL, "Koppel binair 32KB Action Replay cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_SV, "Anslut rå 32KB-Action Replay-insticksmodulfil"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART,    "Attach raw 64KB Retro Replay cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_DE, "64K Retro Replay Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_FR, "Insérer une cartouche Retro Replay 64KO"},
/* it */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_IT, "Seleziona l'immagine di una cartuccia Retro Replay di 64KB"},
/* nl */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_NL, "Koppel binair 64KB Retro Replay cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_SV, "Anslut rå 64KB-Retro Replay-insticksmodulfil"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_IDE64_CART,    "Attach raw 64KB IDE64 cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_IDE64_CART_DE, "64KB IDE64 Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_IDE64_CART_FR, "Insérer une cartouche IDE64 64KO"},
/* it */ {IDCLS_ATTACH_RAW_IDE64_CART_IT, "Seleziona l'immagine di una cartuccia IDE64 di 64KB"},
/* nl */ {IDCLS_ATTACH_RAW_IDE64_CART_NL, "Koppel binair 64KB IDE64 cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_IDE64_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_IDE64_CART_SV, "Anslut rå 64KB IDE64-insticksmodulfil"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART,    "Attach raw 32KB Atomic Power cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_DE, "32KB Atomic Power Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_FR, "Insérer une cartouche Atomic Power 32KO"},
/* it */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_IT, "Seleziona l'immagine di una cartuccia Atomic Power di 32KB"},
/* nl */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_NL, "Koppel binair 32KB Atomic Power cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_SV, "Anslut rå 32KB Atomic Power-insticksmodulfil"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART,    "Attach raw 8KB Epyx fastload cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_DE, "8KB Epyx Fastload Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_FR, "Insérer une cartouche Epyx FastLoad 8KO"},
/* it */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_IT, "Seleziona l'immagine di una cartuccia Epyx fastload di 8KB"},
/* nl */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_NL, "Koppel binair 8KB Epyx fastload cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_SV, "Anslut rå 8KB Epyx fastload-insticksmodulfil"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_SS4_CART,    "Attach raw 32KB Super Snapshot cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_SS4_CART_DE, "32KB Super Snapshot (raw) Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_SS4_CART_FR, "Insérer une cartouche Super Snapshot 32KO"},
/* it */ {IDCLS_ATTACH_RAW_SS4_CART_IT, "Seleziona l'immagine di una cartuccia Super Snapshot di 32KB"},
/* nl */ {IDCLS_ATTACH_RAW_SS4_CART_NL, "Koppel binair 32KB Super Snapshot cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_SS4_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_SS4_CART_SV, "Anslut rå 32KB Super Snapshot-insticksmodulfil"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_SS5_CART,    "Attach raw 64KB Super Snapshot cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_SS5_CART_DE, "64KB Super Snapshot (raw) Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_SS5_CART_FR, "Insérer une cartouche Super Snapshot 64KO"},
/* it */ {IDCLS_ATTACH_RAW_SS5_CART_IT, "Seleziona l'immagine di una cartuccia Super Snapshot di 64KB"},
/* nl */ {IDCLS_ATTACH_RAW_SS5_CART_NL, "Koppel binair 64KB Super Snapshot cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_SS5_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_SS5_CART_SV, "Anslut rå 64KB-Super Snapshot-insticksmodulfil"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_CBM_IEEE488_CART,    "Attach CBM IEEE488 cartridge image"},
/* de */ {IDCLS_ATTACH_CBM_IEEE488_CART_DE, "CBM IEEE488 Erweiterungsmodul einlegen"},
/* fr */ {IDCLS_ATTACH_CBM_IEEE488_CART_FR, "Insérer une cartouche CBM IEEE488"},
/* it */ {IDCLS_ATTACH_CBM_IEEE488_CART_IT, "Seleziona l'immagine di una cartuccia CBM IEE488"},
/* nl */ {IDCLS_ATTACH_CBM_IEEE488_CART_NL, "Koppel CBM IEEE488 cartridge bestand"},
/* pl */ {IDCLS_ATTACH_CBM_IEEE488_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_CBM_IEEE488_CART_SV, "Anslut CBM IEEE488-insticksmodulfil"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_WESTERMANN_CART,    "Attach raw 16KB Westermann learning cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_DE, "16KB Westermann learning (raw) Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_FR, "Insérer une cartouche Westermann Learning 16KO"},
/* it */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_IT, "Seleziona l'immagine di una cartuccia Westermann learning di 16KB"},
/* nl */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_NL, "Koppel binair 16KB Westermann learning cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_WESTERMANN_CART_SV, "Anslut rå 16KB-Westermann learning-insticksmodulfil"},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ENABLE_EXPERT_CART,    "Enable expert cartridge"},
/* de */ {IDCLS_ENABLE_EXPERT_CART_DE, "Expert Cartridge aktivieren"},
/* fr */ {IDCLS_ENABLE_EXPERT_CART_FR, "Activer les paramètres experts de cartouches"},
/* it */ {IDCLS_ENABLE_EXPERT_CART_IT, "Attiva l'Expert Cartridge"},
/* nl */ {IDCLS_ENABLE_EXPERT_CART_NL, "Aktiveer expert cartridge"},
/* pl */ {IDCLS_ENABLE_EXPERT_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EXPERT_CART_SV, "Aktivera Expert Cartridge"},

/* c64/cart/ide64.c */
/* en */ {IDCLS_SPECIFY_IDE64_NAME,    "Specify name of IDE64 image file"},
/* de */ {IDCLS_SPECIFY_IDE64_NAME_DE, "Namen für IDE64 Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_IDE64_NAME_FR, "Spécifier le nom de l'image IDE64"},
/* it */ {IDCLS_SPECIFY_IDE64_NAME_IT, "Specifica il nome del file immagine dell'IDE64"},
/* nl */ {IDCLS_SPECIFY_IDE64_NAME_NL, "Geef de naam van het IDE64 bestand"},
/* pl */ {IDCLS_SPECIFY_IDE64_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_IDE64_NAME_SV, "Ange namn på IDE64-insticksmodulfil"},

/* c64/cart/ide64.c, gfxoutputdrv/ffmpegdrv.c, datasette.c,
   debug.c, mouse.c, ram.c, sound.c, vsync.c */
/* en */ {IDCLS_P_VALUE,    "<value>"},
/* de */ {IDCLS_P_VALUE_DE, "<Wert>"},
/* fr */ {IDCLS_P_VALUE_FR, "<valeur>"},
/* it */ {IDCLS_P_VALUE_IT, "<valore>"},
/* nl */ {IDCLS_P_VALUE_NL, "<waarde>"},
/* pl */ {IDCLS_P_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_VALUE_SV, "<värde>"},

/* c64/cart/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64,    "Set number of cylinders for the IDE64 emulation"},
/* de */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_DE, "Anzahl der Zylinder für IDE64 Emulation"},
/* fr */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_FR, "Régler le nombre de cylindres pour l'émulation IDE64"},
/* it */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_IT, "Imposta il numero di cilindri dell'emulazione IDE64"},
/* nl */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_NL, "Zet het aantal cylinders voor de IDE64 emulatie"},
/* pl */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64_SV, "Välj antal cylindrar för IDE64-emuleringen"},

/* c64/cart/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_HEADS_IDE64,    "Set number of heads for the IDE64 emulation"},
/* de */ {IDCLS_SET_AMOUNT_HEADS_IDE64_DE, "Anzahl der Laufwerksköpfe für IDE64 Emulation"},
/* fr */ {IDCLS_SET_AMOUNT_HEADS_IDE64_FR, "Régler le nombre de têtes pour l'émulation IDE64"},
/* it */ {IDCLS_SET_AMOUNT_HEADS_IDE64_IT, "Imposta il numero di testine dell'emulazione IDE64"},
/* nl */ {IDCLS_SET_AMOUNT_HEADS_IDE64_NL, "Zet het aantal koppen voor de IDE64 emulatie"},
/* pl */ {IDCLS_SET_AMOUNT_HEADS_IDE64_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_AMOUNT_HEADS_IDE64_SV, "Ange antal huvuden för IDE64-emuleringen"},

/* c64/cart/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_SECTORS_IDE64,    "Set number of sectors for the IDE64 emulation"},
/* de */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_DE, "Anzahl der Sektoren für IDE64 Emulation"},
/* fr */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_FR, "Régler le nombre de secteurs pour l'émulation IDE64"},
/* it */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_IT, "Imposta il numero di settori dell'emulazione IDE64"},
/* nl */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_NL, "Zet het aantal sectors voor de IDE64 emulatie"},
/* pl */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_AMOUNT_SECTORS_IDE64_SV, "Ange antal sektorer för IDE64-emuleringen"},

/* c64/cart/ide64.c */
/* en */ {IDCLS_AUTODETECT_IDE64_GEOMETRY,    "Autodetect geometry of formatted images"},
/* de */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_DE, "Geometrie von formatierten Image Dateien automatisch erkennen"},
/* fr */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_FR, "Détecter automatiquement la géométrie des images formattées"},
/* it */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_IT, "Rileva automaticamente la geometria delle immagini formattate"},
/* nl */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_NL, "Automatisch de grootte van de geformateerde bestanden detecteren"},
/* pl */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_AUTODETECT_IDE64_GEOMETRY_SV, "Autodetektera geometri från formaterad avbildningsfil"},

/* c64/cart/ide64.c */
/* en */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY,    "Do not autodetect geometry of formatted images"},
/* de */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_DE, "Geometrie von formatierten Image Dateien nicht automatisch erkennen"},
/* fr */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_FR, "Ne pas détecter automatiquement la géométrie des images formattées"},
/* it */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_IT, "Non rilevare automaticamente la geometrica delle immagini formattate"},
/* nl */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_NL, "Niet automatisch de grootte van de geformateerde bestanden detecteren"},
/* pl */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_SV, "Autodetektera inte geometri från formaterad avbildningsfil"},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_P_MODELNUMBER,    "<modelnumber>"},
/* de */ {IDCLS_P_MODELNUMBER_DE, "<Modell Nummer>"},
/* fr */ {IDCLS_P_MODELNUMBER_FR, "<numérodemodèle>"},
/* it */ {IDCLS_P_MODELNUMBER_IT, "<numero di modello>"},
/* nl */ {IDCLS_P_MODELNUMBER_NL, "<modelnummer>"},
/* pl */ {IDCLS_P_MODELNUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_MODELNUMBER_SV, "<modellnummer>"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CBM2_MODEL,    "Specify CBM-II model to emulate"},
/* de */ {IDCLS_SPECIFY_CBM2_MODEL_DE, "CBM-II Modell für Emulation definieren"},
/* fr */ {IDCLS_SPECIFY_CBM2_MODEL_FR, "Spécifier le modèle CBM-II à émuler"},
/* it */ {IDCLS_SPECIFY_CBM2_MODEL_IT, "Specifica il modello di CBM II da emulare"},
/* nl */ {IDCLS_SPECIFY_CBM2_MODEL_NL, "Geef CBM-II model om te emuleren"},
/* pl */ {IDCLS_SPECIFY_CBM2_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CBM2_MODEL_SV, "Ange CBM-II-modell att emulera"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_TO_USE_VIC_II,    "Specify to use VIC-II"},
/* de */ {IDCLS_SPECIFY_TO_USE_VIC_II_DE, "VIC-II Unterstützung aktivieren"},
/* fr */ {IDCLS_SPECIFY_TO_USE_VIC_II_FR, "Spécifier l'utilisation de VIC-II"},
/* it */ {IDCLS_SPECIFY_TO_USE_VIC_II_IT, "Specifica di utilizzare il VIC-II"},
/* nl */ {IDCLS_SPECIFY_TO_USE_VIC_II_NL, "Gebruik de VIC-II"},
/* pl */ {IDCLS_SPECIFY_TO_USE_VIC_II_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TO_USE_VIC_II_SV, "Ange för att använda VIC-II"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_TO_USE_CRTC,    "Specify to use CRTC"},
/* de */ {IDCLS_SPECIFY_TO_USE_CRTC_DE, "CRTC Unterstützung aktivieren"},
/* fr */ {IDCLS_SPECIFY_TO_USE_CRTC_FR, "Spécifier l'utilisation de CRTC"},
/* it */ {IDCLS_SPECIFY_TO_USE_CRTC_IT, "Specifica di utilizzare il CRTC"},
/* nl */ {IDCLS_SPECIFY_TO_USE_CRTC_NL, "Gebruik de CRTC"},
/* pl */ {IDCLS_SPECIFY_TO_USE_CRTC_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TO_USE_CRTC_SV, "Ange för att använda CRTC"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_P_LINENUMBER,    "<linenumber>"},
/* de */ {IDCLS_P_LINENUMBER_DE, "<Zeilennummer>"},
/* fr */ {IDCLS_P_LINENUMBER_FR, "<numérodeligne>"},
/* it */ {IDCLS_P_LINENUMBER_IT, "<numero di linea>"},
/* nl */ {IDCLS_P_LINENUMBER_NL, "<lijnnummer>"},
/* pl */ {IDCLS_P_LINENUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_LINENUMBER_SV, "<linjenummer>"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE,    "Specify CBM-II model hardware (0=6x0, 1=7x0)"},
/* de */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_DE, "CBM-II Hardware Modell definieren (0=6x0, 1=7x0)"},
/* fr */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_FR, "Spécifier le modèle CBM-II (0=6x0, 1=7x0)"},
/* it */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_IT, "Specifica il modello hardware del CBM-II (0=6x0,Â·1=7x0)"},
/* nl */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_NL, "Geef CBM-II hardware model (0=6x0, 1=7x0)"},
/* pl */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_SV, "Ange maskinvara för CBM-II-modell (0=6x0, 1=7x0)"},

/* cbm2/cbm2-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_P_RAMSIZE,    "<ramsize>"},
/* de */ {IDCLS_P_RAMSIZE_DE, "<RAM Größe>"},
/* fr */ {IDCLS_P_RAMSIZE_FR, "<tailleram>"},
/* it */ {IDCLS_P_RAMSIZE_IT, "<dimensione della ram>"},
/* nl */ {IDCLS_P_RAMSIZE_NL, "<geheugen grootte>"},
/* pl */ {IDCLS_P_RAMSIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_RAMSIZE_SV, "<ramstorlek>"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIZE_OF_RAM,    "Specify size of RAM (64/128/256/512/1024 kByte)"},
/* de */ {IDCLS_SPECIFY_SIZE_OF_RAM_DE, "RAM Größe definieren (64/128/256/512/1024 kByte)"},
/* fr */ {IDCLS_SPECIFY_SIZE_OF_RAM_FR, "Spécifier la taille de la RAM (64/128/256/512/1024 kilo-octets)"},
/* it */ {IDCLS_SPECIFY_SIZE_OF_RAM_IT, "Specifica la dimensione della RAM (4/128/256/512/1024 kByte)"},
/* nl */ {IDCLS_SPECIFY_SIZE_OF_RAM_NL, "Geef geheugen grootte (64/128/256/512/1024 kByte)"},
/* pl */ {IDCLS_SPECIFY_SIZE_OF_RAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SIZE_OF_RAM_SV, "Ange storlek på RAM (64/128/256/512/1024 kByte)"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_1000_NAME,    "Specify name of cartridge ROM image for $1000"},
/* de */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_DE, "Name für ROM Module Image Datei ($1000)"},
/* fr */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_FR, "Spécifier le nom de la l'image de cartouche ROM pour $1000"},
/* it */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_IT, "Specifica il nome dell'immagine della cartuccia ROM a $1000"},
/* nl */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_NL, "Geef de naam van het cartridge ROM bestand voor $1000"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_SV, "Ange namn på insticksmodul-ROM-avbildning för $1000"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_2000_NAME,    "Specify name of cartridge ROM image for $2000-$3fff"},
/* de */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_DE, "Name für ROM Module Image Datei ($2000-3fff)"},
/* fr */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_FR, "Spécifier le nom de la l'image de cartouche ROM pour $2000-$3fff"},
/* it */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_IT, "Specifica il nome dell'immagine della ROM della cartuccia per $2000-$3fff"},
/* nl */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_NL, "Geef de naam van het cartridge ROM bestand voor $2000-$3fff"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_SV, "Ange namn på insticksmodul-ROM-avbildning för $2000-$3fff"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_4000_NAME,    "Specify name of cartridge ROM image for $4000-$5fff"},
/* de */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_DE, "Name für ROM Module Image Datei ($4000-5fff)"},
/* fr */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_FR, "Spécifier le nom de la l'image de cartouche ROM pour $4000-$5fff"},
/* it */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_IT, "Specifica il nome dell'immagine della ROM della cartuccia per $4000-$5fff"},
/* nl */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_NL, "Geef de naam van het cartridge ROM bestand voor $4000-$5fff"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_SV, "Ange namn på insticksmodul-ROM-avbildning för $4000-$5fff"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_6000_NAME,    "Specify name of cartridge ROM image for $6000-$7fff"},
/* de */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_DE, "Name für ROM Module Image Datei ($6000-7fff)"},
/* fr */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_FR, "Spécifier le nom de la l'image de cartouche ROM pour $6000-$7fff"},
/* it */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_IT, "Specifica il nome dell'immagine della ROM della cartuccia per $6000-$7fff"},
/* nl */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_NL, "Geef de naam van het cartridge ROM bestand voor $6000-$7fff"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_SV, "Ange namn på insticksmodul-ROM-avbildning för $6000-$7fff"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800,    "Enable RAM mapping in $0800-$0FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_DE, "RAM Zuordnung in $0800-$0FFF aktivieren"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_FR, "Activer le mappage RAM dans $0800-$0FFF"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_IT, "Attiva il mappaggio della RAM a $0800-$0FFF"},
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_NL, "Aktiveer RAM op adres $0800-$0FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_SV, "Aktivera RAM-mappning på $0800-$0FFF"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800,    "Disable RAM mapping in $0800-$0FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_DE, "RAM Zuordnung in $0800-$0FFF deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_FR, "Désactiver le mappage RAM dans $0800-$0FFF"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_IT, "Disattiva il mappaggio della RAM a $0800-$0FFF"},
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_NL, "RAM op adres $0800-$0FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_SV, "Inaktivera RAM-mappning på $0800-$0FFF"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000,    "Enable RAM mapping in $1000-$1FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_DE, "RAM Zuordnung in $1000-$1FFF aktivieren"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_FR, "Activer le mappage RAM dans $1000-$1FFF"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_IT, "Attiva il mappaggio della RAM a $1000-$1FFF"},
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_NL, "Aktiveer RAM op adres $1000-$1FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_SV, "Aktivera RAM-mappning på $1000-$1FFF"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000,    "Disable RAM mapping in $1000-$1FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_DE, "RAM Zuordnung in $1000-$1FFF deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_FR, "Désactiver le mappage RAM dans $1000-$1FFF"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_IT, "Disattiva il mappaggio della RAM a $1000-$1FFF"},
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_NL, "RAM op adres $1000-$1FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_SV, "Inaktivera RAM-mappning på $1000-$1FFF"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000,    "Enable RAM mapping in $2000-$3FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_DE, "RAM Zuordnung in $2000-$3FFF aktivieren"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_FR, "Activer le mappage RAM dans $2000-$3FFF"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_IT, "Attiva il mappaggio della RAM a $2000-$3FFF"},
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_NL, "Aktiveer RAM op adres $2000-$3FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_SV, "Aktivera RAM-mappning på $2000-$3FFF"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000,    "Disable RAM mapping in $2000-$3FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_DE, "RAM Zuordnung in $2000-$3FFF deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_FR, "Désactiver le mappage RAM dans $2000-$3FFF"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_IT, "Disattiva il mappaggio della RAM a $2000-$3FFF"},
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_NL, "RAM op adres $2000-$3FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_SV, "Inaktivera RAM-mappning på $2000-$3FFF"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000,    "Enable RAM mapping in $4000-$5FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_DE, "RAM Zuordnung in $4000-$5FFF aktivieren"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_FR, "Activer le mappage RAM dans $4000-$5FFF"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_IT, "Attiva il mappaggio della RAM a $4000-$5FFF"},
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_NL, "Aktiveer RAM op adres $4000-$5FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_SV, "Aktivera RAM-mappning på $4000-$5FFF"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000,    "Disable RAM mapping in $4000-$5FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_DE, "RAM Zuordnung in $4000-$5FFF deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_FR, "Désactiver le mappage RAM dans $4000-$5FFF"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_IT, "Disattiva il mappaggio della RAM a $4000-$5FFF"},
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_NL, "RAM op adres $4000-$5FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_SV, "Inaktivera RAM-mappning på $4000-$5FFF"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000,    "Enable RAM mapping in $6000-$7FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_DE, "RAM Zuordnung in $6000-$7FFF aktivieren"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_FR, "Activer le mappage RAM dans $6000-$7FFF"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_IT, "Attiva il mappaggio della RAM a $6000-$7FFF"},
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_NL, "Aktiveer RAM op adres $6000-$7FFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_SV, "Aktivera RAM-mappning på $6000-$7FFF"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000,    "Disable RAM mapping in $6000-$7FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_DE, "RAM Zuordnung in $6000-$7FFF deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_FR, "Désactiver le mappage RAM dans $6000-$7FFF"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_IT, "Disattiva il mappaggio della RAM a $6000-$7FFF"},
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_NL, "RAM op adres $6000-$7FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_SV, "Inaktivera RAM-mappning på $6000-$7FFF"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000,    "Enable RAM mapping in $C000-$CFFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_DE, "RAM Zuordnung in $C000-$CFFF aktivieren"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_FR, "Activer le mappage RAM dans $C000-$CFFF"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_IT, "Attiva il mappaggio della RAM a $C000-$CFFF"},
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_NL, "Aktiveer RAM op adres $C000-$CFFF"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_SV, "Aktivera RAM-mappning på $C000-$CFFF"},

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000,    "Disable RAM mapping in $C000-$CFFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_DE, "RAM Zuordnung in $C000-$CFFF deaktivieren"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_FR, "Désactiver le mappage RAM dans $C000-$CFFF"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_IT, "Disattiva il mappaggio della RAM a $C000-$CFFF"},
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_NL, "RAM op adres $C000-$CFFF afsluiten"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_SV, "Inaktivera RAM-mappning på $C000-$CFFF"},

#ifdef COMMON_KBD
/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KEYMAP_INDEX,    "Specify index of keymap file"},
/* de */ {IDCLS_SPECIFY_KEYMAP_INDEX_DE, "Index der Keymap Datei definieren"},
/* fr */ {IDCLS_SPECIFY_KEYMAP_INDEX_FR, "Spécifier l'index du fichier de mappage clavier"},
/* it */ {IDCLS_SPECIFY_KEYMAP_INDEX_IT, "Specifica l'indice del file della mappa della tastiera"},
/* nl */ {IDCLS_SPECIFY_KEYMAP_INDEX_NL, "Geef de index van het keymap bestand"},
/* pl */ {IDCLS_SPECIFY_KEYMAP_INDEX_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_KEYMAP_INDEX_SV, "Ange index o tangentbordsfil"},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME,    "Specify name of graphics keyboard symbolic keymap file"},
/* de */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_DE, "Keymap Datei für graphics keyboard symbolic definieren"},
/* fr */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier symbolique"},
/* it */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_IT, "Specifica il nome del file della mappa simbolica della tastiera grafica"},
/* nl */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_NL, "Geef de naam van het symbolisch keymap bestand voor het grafische toetsenbord"},
/* pl */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_SV, "Ange fil för symbolisk tangentbordsemulering för \"graphics\"-tangentbord"},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME,    "Specify name of graphics keyboard positional keymap file"},
/* de */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_DE, "Keymap Datei für graphics keyboard positional definieren"},
/* fr */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier positionnel"},
/* it */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_IT, "Specifica il nome del file della mappa posizionale della tastiera grafica"},
/* nl */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_NL, "Geef de naam van het positioneel keymap bestand voor het grafische toetsenbord"},
/* pl */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_SV, "Ange fil för positionsriktig tangentbordsemulering för \"graphics\"-tangentbord"},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME,    "Specify name of UK business keyboard symbolic keymap file"},
/* de */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_DE, "Keymap Datei für UK business keyboard symbolic definieren"},
/* fr */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier symbolique UK"},
/* it */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_IT, "Specifica il nome del file della mappa simbolica della tastiera UK business"},
/* nl */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_NL, "Geef de naam van het symbolisch keymap bestand voor het UK business toetsenbord"},
/* pl */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_SV, "Ange fil för symbolisk tangentbordsemulering för brittiskt \"business\"-tangentbord"},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME,    "Specify name of UK business keyboard positional keymap file"},
/* de */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_DE, "Keymap Datei für UK business keyboard positional definieren"},
/* fr */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier positionnel UK"},
/* it */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_IT, "Specifica il nome del file della mappa posizionale della tastiera UK business"},
/* nl */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_NL, "Geef de naam van het positioneel keymap bestand voor het UK business toetsenbord"},
/* pl */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_SV, "Ange fil för positionsriktig tangentbordsemulering för brittiskt \"business\"-tangentbord"},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME,    "Specify name of German business keyboard symbolic keymap file"},
/* de */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_DE, "Keymap Datei für German business keyboard symbolic definieren"},
/* fr */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier symbolique allemand"},
/* it */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_IT, "Specifica il nome del file della mappa simbolica della tastiera business tedesca"},
/* nl */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_NL, "Geef de naam van het symbolisch keymap bestand voor het Duitse business toetsenbord"},
/* pl */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_SV, "Ange fil för symbolisk tangentbordsemulering för tyskt \"business\"-tangentbord"},

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME,    "Specify name of German business keyboard positional keymap file"},
/* de */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_DE, "Keymap Datei für German business keyboard positional definieren"},
/* fr */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier positionnel allemand"},
/* it */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_IT, "Specifica il nome del file della mappa posizionale della tastiera business tedesca"},
/* nl */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_NL, "Geef de naam van het positioneel keymap bestand voor het Duitse business toetsenbord"},
/* pl */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_SV, "Ange fil för positionsriktig tangentbordsemulering för tyskt \"business\"-tangentbord"},
#endif

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_ENABLE_TRUE_DRIVE,    "Enable hardware-level emulation of disk drives"},
/* de */ {IDCLS_ENABLE_TRUE_DRIVE_DE, "Hardware Emulation für Disklaufwerke aktivieren"},
/* fr */ {IDCLS_ENABLE_TRUE_DRIVE_FR, "Activer l'émulation des disque au niveau physique"},
/* it */ {IDCLS_ENABLE_TRUE_DRIVE_IT, "Attiva l'emulazione hardware dei disk drive"},
/* nl */ {IDCLS_ENABLE_TRUE_DRIVE_NL, "Aktiveer hardware-matige emulatie van disk drives"},
/* pl */ {IDCLS_ENABLE_TRUE_DRIVE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TRUE_DRIVE_SV, "Aktivera emulering av diskettstationer på maskinvarunivå"},

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_DISABLE_TRUE_DRIVE,    "Disable hardware-level emulation of disk drives"},
/* de */ {IDCLS_DISABLE_TRUE_DRIVE_DE, "Hardware Emulation für Disklaufwerke deaktivieren"},
/* fr */ {IDCLS_DISABLE_TRUE_DRIVE_FR, "Désactiver l'émulation des disque au niveau physique"},
/* it */ {IDCLS_DISABLE_TRUE_DRIVE_IT, "Disattiva l'emulazione hardware dei disk drive"},
/* nl */ {IDCLS_DISABLE_TRUE_DRIVE_NL, "Hardware-matige emulatie van disk drives afsluiten"},
/* pl */ {IDCLS_DISABLE_TRUE_DRIVE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_TRUE_DRIVE_SV, "Inaktivera emulering av diskettstationer på maskinvarunivå"},

/* drive/drive-cmdline-options.c, printerdrv/interface-serial.c,
   attach.c */
/* en */ {IDCLS_P_TYPE,    "<type>"},
/* de */ {IDCLS_P_TYPE_DE, "<Typ>"},
/* fr */ {IDCLS_P_TYPE_FR, "<type>"},
/* it */ {IDCLS_P_TYPE_IT, "<tipo>"},
/* nl */ {IDCLS_P_TYPE_NL, "<soort>"},
/* pl */ {IDCLS_P_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_TYPE_SV, "<typ>"},

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_SET_DRIVE_TYPE,    "Set drive type (0: no drive)"},
/* de */ {IDCLS_SET_DRIVE_TYPE_DE, "Setze Laufwerkstyp (0: kein Laufwerk)"},
/* fr */ {IDCLS_SET_DRIVE_TYPE_FR, "Spécifier le type de lecteur de disque (0: pas de lecteur)"},
/* it */ {IDCLS_SET_DRIVE_TYPE_IT, "Imposta il tipo di drive (0: nessun drive)"},
/* nl */ {IDCLS_SET_DRIVE_TYPE_NL, "Zet drive soort (0: geen drive)"},
/* pl */ {IDCLS_SET_DRIVE_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DRIVE_TYPE_SV, "Ange enhetstyp (0: ingen enhet)"},

/* drive/drive-cmdline-options.c,
   drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_P_METHOD,    "<method>"},
/* de */ {IDCLS_P_METHOD_DE, "<Methode>"},
/* fr */ {IDCLS_P_METHOD_FR, "<méthode>"},
/* it */ {IDCLS_P_METHOD_IT, "<metodo>"},
/* nl */ {IDCLS_P_METHOD_NL, "<methode>"},
/* pl */ {IDCLS_P_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_METHOD_SV, "<metod>"},

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_SET_DRIVE_EXTENSION_POLICY,    "Set drive 40 track extension policy (0: never, 1: ask, 2: on access)"},
/* de */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_DE, "40 Spur Erweiterungsmethode (0: nie, 1: Rückfrage, 2: bei Bedarf)"},
/* fr */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_FR, "Spécifier la règle d'extention 40 pistes (0: jamais, 1: demander, 2: à l'accès)"},
/* it */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_IT, "Imposta la politica di estensione a 40 tracce del drive (0: mai, 1: a richiesta, 2: all'accesso)"},
/* nl */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_NL, "Zet drive 40 sporen uitbreidings regels (0: nooit, 1: vraag, 2: bij toegang)"},
/* pl */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_SV, "Ange regel för 40-spårsutökning (0: aldrig, 1: fråga, 2: vid åtkomst)"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME,    "Specify name of 1541 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_DE, "Name der 1541 DOS ROM Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1541 DOS ROM"},
/* it */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1541"},
/* nl */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_NL, "Geef de naam van het 1541 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_SV, "Ange namn på 1541-DOS-ROM-avbildning"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME,    "Specify name of 1541-II DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_DE, "Name der 1541-II DOS ROM Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1541-II DOS ROM"},
/* it */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1541-II"},
/* nl */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_NL, "Geef de naam van het 1541-II DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_SV, "Ange namn på 1541-II-DOS-ROM-avbildning"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME,    "Specify name of 1570 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_DE, "Name der 1570 DOS ROM Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1570 DOS ROM"},
/* it */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1570"},
/* nl */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_NL, "Geef de naam van het 1570 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_SV, "Ange namn på 1570-DOS-ROM-avbildning"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME,    "Specify name of 1571 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_DE, "Name der 1571 DOS ROM Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1571 DOS ROM"},
/* it */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1571"},
/* nl */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_NL, "Geef de naam van het 1571 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_SV, "Ange namn på 1571-DOS-ROM-avbildning"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME,    "Specify name of 1581 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_DE, "Name der 1581 DOS ROM Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1581 DOS ROM"},
/* it */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1581"},
/* nl */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_NL, "Geef de naam van het 1581 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_SV, "Ange namn på 1581-DOS-ROM-avbildning"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_PAR_CABLE,    "Enable SpeedDOS-compatible parallel cable"},
/* de */ {IDCLS_ENABLE_PAR_CABLE_DE, "SpeedDOS kompatibles Parallelkabel aktivieren"},
/* fr */ {IDCLS_ENABLE_PAR_CABLE_FR, "Activer l'utilisation du câble parallèle compatible SpeedDOS"},
/* it */ {IDCLS_ENABLE_PAR_CABLE_IT, "Attiva il cavo parallelo compatibile con SpeedDOS"},
/* nl */ {IDCLS_ENABLE_PAR_CABLE_NL, "Aktiveer SpeedDOS-compatible parallel kabel"},
/* pl */ {IDCLS_ENABLE_PAR_CABLE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_PAR_CABLE_SV, "Aktivera SpeedDOS-kompatibel parallellkabel"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_PAR_CABLE,    "Disable SpeedDOS-compatible parallel cable"},
/* de */ {IDCLS_DISABLE_PAR_CABLE_DE, "SpeedDOS kompatibles Parallelkabel deaktivieren"},
/* fr */ {IDCLS_DISABLE_PAR_CABLE_FR, "Désactiver l'utilisation du câble parallèle  compatible SpeedDOS"},
/* it */ {IDCLS_DISABLE_PAR_CABLE_IT, "Disttiva il cavo parallelo compatibile con SpeedDOS"},
/* nl */ {IDCLS_DISABLE_PAR_CABLE_NL, "SpeedDOS-compatible parallel kabel afsluiten"},
/* pl */ {IDCLS_DISABLE_PAR_CABLE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_PAR_CABLE_SV, "Inaktivera SpeedDOS-kompatibel parallellkabel"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SET_IDLE_METHOD,    "Set drive idling method (0: no traps, 1: skip cycles, 2: trap idle)"},
/* de */ {IDCLS_SET_IDLE_METHOD_DE, "Laufwerks idling Methode (0: kein Traps, 1: Zyklen verwerfen, 2: trap idle)"},
/* fr */ {IDCLS_SET_IDLE_METHOD_FR, "Spécifier la méthode d'idle (0: no traps, 1: sauter des cycles, 2: trap idle)"},
/* it */ {IDCLS_SET_IDLE_METHOD_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_IDLE_METHOD_NL, "Zet de drive idle methode (0: geen traps, 1: sla cycli over, 2: trap idle)"},
/* pl */ {IDCLS_SET_IDLE_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_IDLE_METHOD_SV, "Ställ in väntemetod för diskettstation (0: fånta inge, 1: hoppa cykler, 2: fånga väntan)"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_2000,    "Enable 8KB RAM expansion at $2000-$3FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_2000_DE, "8KB RAM Erweiterung bei $2000-3fff aktivieren"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_2000_FR, "Enable 8KB RAM expansion at $2000-$3FFF"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_2000_IT, "Attiva l'espansione di RAM di 8KB a $2000-$3FFF"},
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_2000_NL, "Aktiveer 8KB RAM uitbreiding op adres $2000-$3FFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_2000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_2000_SV, "Aktivera 8KB RAM-expansion på $2000-$3FFF"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_2000,    "Disable 8KB RAM expansion at $2000-$3FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_2000_DE, "8KB RAM Erweiterung bei $2000-3fff deaktivieren"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_2000_FR, "Désactiver l'exansion de 8KB RAM à $2000-$3FFF"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_2000_IT, "Disattiva l'espansione di RAM di 8KB a $2000-$3FFF"},
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_2000_NL, "8KB RAM uitbreiding op adres $2000-$3FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_2000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_2000_SV, "Inaktivera 8KB RAM-expansion på $2000-$3FFF"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_4000,    "Enable 8KB RAM expansion at $4000-$5FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_4000_DE, "8KB RAM Erweiterung bei $4000-5fff aktivieren"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_4000_FR, "Enable 8KB RAM expansion at $4000-$5FFF"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_4000_IT, "Attiva l'espansione di RAM di 8KB a $4000-$5FFF"},
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_4000_NL, "Aktiveer 8KB RAM uitbreiding op adres $4000-$5FFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_4000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_4000_SV, "Aktivera 8KB RAM-expansion på $4000-$5FFF"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_4000,    "Disable 8KB RAM expansion at $4000-$5FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_4000_DE, "8KB RAM Erweiterung bei $4000-5fff deaktivieren"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_4000_FR, "Désactiver l'exansion de 8KB RAM à $4000-$5FFF"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_4000_IT, "Disattiva l'espansione di RAM di 8KB a $4000-$5FFF"},
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_4000_NL, "8KB RAM uitbreiding op adres $4000-$5FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_4000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_4000_SV, "Inaktivera 8KB RAM-expansion på $4000-$5FFF"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_6000,    "Enable 8KB RAM expansion at $6000-$7FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_6000_DE, "8KB RAM Erweiterung bei $6000-7fff aktivieren"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_6000_FR, "Enable 8KB RAM expansion at $6000-$7FFF"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_6000_IT, "Attiva l'espansione di RAM di 8KB a $6000-$7FFF"},
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_6000_NL, "Aktiveer 8KB RAM uitbreiding op adres $6000-$7FFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_6000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_6000_SV, "Aktivera 8KB RAM-expansion på $6000-$7FFF"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_6000,    "Disable 8KB RAM expansion at $6000-$7FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_6000_DE, "8KB RAM Erweiterung bei $6000-7fff deaktivieren"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_6000_FR, "Désactiver l'exansion de 8KB RAM à $6000-$7FFF"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_6000_IT, "Disattiva l'espansione di RAM di 8KB a $6000-$7FFF"},
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_6000_NL, "8KB RAM uitbreiding op adres $6000-$7FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_6000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_6000_SV, "Inaktivera 8KB RAM-expansion på $6000-$7FFF"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_8000,    "Enable 8KB RAM expansion at $8000-$9FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_8000_DE, "8KB RAM Erweiterung bei $8000-9fff aktivieren"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_8000_FR, "Enable 8KB RAM expansion at $8000-$9FFF"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_8000_IT, "Attiva l'espansione di RAM di 8KB a $8000-$9FFF"},
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_8000_NL, "Aktiveer 8KB RAM uitbreiding op adres $8000-$9FFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_8000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_8000_SV, "Aktivera 8KB RAM-expansion på $8000-$9FFF"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_8000,    "Disable 8KB RAM expansion at $8000-$9FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_8000_DE, "8KB RAM Erweiterung bei $8000-9fff deaktivieren"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_8000_FR, "Désactiver l'exansion de 8KB RAM à $8000-$9FFF"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_8000_IT, "Disattiva l'espansione di RAM di 8KB a $8000-$9FFF"},
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_8000_NL, "8KB RAM uitbreiding op adres $8000-$9FFF afsluiten"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_8000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_8000_SV, "Inaktivera 8KB RAM-expansion på $8000-$9FFF"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_A000,    "Enable 8KB RAM expansion at $A000-$BFFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_A000_DE, "8KB RAM Erweiterung bei $a000-bfff aktivieren"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_A000_FR, "Enable 8KB RAM expansion at $A000-$BFFF"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_A000_IT, "Attiva l'espansione di RAM di 8KB a $A000-$BFFF"},
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_A000_NL, "Aktiveer 8KB RAM uitbreiding op adres $A000-$BFFF"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_A000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_A000_SV, "Aktivera 8KB RAM-expansion på $A000-$BFFF"},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_A000,    "Disable 8KB RAM expansion at $A000-$BFFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_A000_DE, "8KB RAM Erweiterung bei $a000-bfff deaktivieren"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_A000_FR, "Désactiver l'exansion de 8KB RAM à $A000-$BFFF"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_A000_IT, "Disattiva l'espansione di RAM di 8KB a $A000-$BFFF"},
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_A000_NL, "8KB RAM uitbreiding op adres $A000-$BFFF afsluiten"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_A000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_A000_SV, "Inaktivera 8KB RAM-expansion på $A000-$BFFF"},

/* drive/iec128dcr/iec128dcr-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME,    "Specify name of 1571CR DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_DE, "Name der 1571CR DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1571CR DOS ROM"},
/* it */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1571CR"},
/* nl */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_NL, "Geef de naam van het 1571CR DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_SV, "Ange namn på 1571CR-DOS-ROM-avbildning"},

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME,    "Specify name of 2031 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_DE, "Name der 2031 DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 2031 DOS ROM"},
/* it */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 2031"},
/* nl */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_NL, "Geef de naam van het 2031 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_SV, "Ange namn på 2031-DOS-ROM-avbildning"},

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME,    "Specify name of 2040 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_DE, "Name der 2040 DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 2040 DOS ROM"},
/* it */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 2040"},
/* nl */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_NL, "Geef de naam van het 2040 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_SV, "Ange namn på 2040-DOS-ROM-avbildning"},

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME,    "Specify name of 3040 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_DE, "Name der 3040 DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 3040 DOS ROM"},
/* it */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 3040"},
/* nl */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_NL, "Geef de naam van het 3040 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_SV, "Ange namn på 3040-DOS-ROM-avbildning"},

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME,    "Specify name of 4040 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_DE, "Name der 4040 DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 4040 DOS ROM"},
/* it */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 4040"},
/* nl */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_NL, "Geef de naam van het 4040 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_SV, "Ange namn på 4040-DOS-ROM-avbildning"},

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME,    "Specify name of 1001/8050/8250 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_DE, "Name der 1001/8040/8205 DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1001/8050/8250 DOS ROM"},
/* it */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1001/8050/8250"},
/* nl */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_NL, "Geef de naam van het 1001/8050/8250 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_SV, "Ange namn på 1001/8050/8250-DOS-ROM-avbildning"},

/* drive/tcbm/tcbm-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME,    "Specify name of 1551 DOS ROM image"},
/* de */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_DE, "Name der 4040 DOS ROM Datei definieren"},
/* fr */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1551 DOS ROM"},
/* it */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM del DOS del 1551"},
/* nl */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_NL, "Geef de naam van het 1551 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_SV, "Ange namn på 1551-DOS-ROM-avbildning"},

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8,    "Use <name> as directory for file system device #8"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_DE, "Benutze <Name> für Verzeichnis beim Verzeichniszugriff für Gerät #8"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_FR, "Utiliser le répertoire <nom> comme système de fichiers pour le lecteur #8"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_IT, "Una <nome> come direcory per il file system della periferica #8"},
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_NL, "Gebruik <naam> als directory voor file system apparaat #8"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_SV, "Ange <namn> som katalog för filsystemsbaserad enhet 8"},

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9,    "Use <name> as directory for file system device #9"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_DE, "Benutze <Name> für Verzeichnis beim Verzeichniszugriff für Gerät #9"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_FR, "Utiliser le répertoire <nom> comme système de fichiers pour le lecteur #9"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_IT, "Una <nome> come direcory per il file system della periferica #9"},
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_NL, "Gebruik <naam> als directory voor file system apparaat #9"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_SV, "Ange <namn> som katalog för filsystemsbaserad enhet 9"},

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10,    "Use <name> as directory for file system device #10"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_DE, "Benutze <Name> für Verzeichnis beim Verzeichniszugriff für Gerät #10"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_FR, "Utiliser le répertoire <nom> comme système de fichiers pour le lecteur #10"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_IT, "Una <nome> come direcory per il file system della periferica #10"},
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_NL, "Gebruik <naam> als directory voor file system apparaat #10"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_SV, "Ange <namn> som katalog för filsystemsbaserad enhet 10"},

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11,    "Use <name> as directory for file system device #11"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_DE, "Benutze <Name> für Verzeichnis beim Verzeichniszugriff für Gerät #11"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_FR, "Utiliser le répertoire <nom> comme système de fichiers pour le lecteur #11"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_IT, "Una <nome> come direcory per il file system della periferica #11"},
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_NL, "Gebruik <naam> als directory voor file system apparaat #11"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_SV, "Ange <namn> som katalog för filsystemsbaserad enhet 11"},

#ifdef HAVE_FFMPEG
/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDCLS_SET_AUDIO_STREAM_BITRATE,    "Set bitrate for audio stream in media file"},
/* de */ {IDCLS_SET_AUDIO_STREAM_BITRATE_DE, "Bitrate für Audio Stream für Media Datei setzen"},
/* fr */ {IDCLS_SET_AUDIO_STREAM_BITRATE_FR, "Régler le bitrate audio du fichier média"},
/* it */ {IDCLS_SET_AUDIO_STREAM_BITRATE_IT, "Imposta il bitrate del flusso audio nel file multimediale"},
/* nl */ {IDCLS_SET_AUDIO_STREAM_BITRATE_NL, "Zet de bitrate voor het audio gedeelte van het media bestand"},
/* pl */ {IDCLS_SET_AUDIO_STREAM_BITRATE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_AUDIO_STREAM_BITRATE_SV, "Ange bithastighet för ljudström i mediafil"},

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDCLS_SET_VIDEO_STREAM_BITRATE,    "Set bitrate for video stream in media file"},
/* de */ {IDCLS_SET_VIDEO_STREAM_BITRATE_DE, "Bitrate für Video Stream für Media Datei setzen"},
/* fr */ {IDCLS_SET_VIDEO_STREAM_BITRATE_FR, "Régler le bitrate vidéo du fichier média"},
/* it */ {IDCLS_SET_VIDEO_STREAM_BITRATE_IT, "Imposta il bitrate per il flusso video nel file multimediale"},
/* nl */ {IDCLS_SET_VIDEO_STREAM_BITRATE_NL, "Zet de bitrate voor het video gedeelte van het media bestand"},
/* pl */ {IDCLS_SET_VIDEO_STREAM_BITRATE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VIDEO_STREAM_BITRATE_SV, "Ange bithastighet för videoström i mediafil"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_PET_MODEL,    "Specify PET model to emulate"},
/* de */ {IDCLS_SPECIFY_PET_MODEL_DE, "PET Modell spezifizieren"},
/* fr */ {IDCLS_SPECIFY_PET_MODEL_FR, "Spécifier le modèle PET à émuler"},
/* it */ {IDCLS_SPECIFY_PET_MODEL_IT, "Specifica il modello di PET da emulare"},
/* nl */ {IDCLS_SPECIFY_PET_MODEL_NL, "Geef PET model om te emuleren"},
/* pl */ {IDCLS_SPECIFY_PET_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PET_MODEL_SV, "Ange PET-modell att emulera"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_EDITOR_ROM_NAME,    "Specify name of Editor ROM image"},
/* de */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_DE, "Name für Editor ROM Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_FR, "Spécifier la vitesse de l'éditeur d'image ROM"},
/* it */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_IT, "Specifica il nome dell'immagine della Editor ROM"},
/* nl */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_NL, "Geef naam van het Editor ROM bestand"},
/* pl */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_SV, "Ange namn på Editor-ROM-avbildning"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME,    "Specify 4K extension ROM name at $9***"},
/* de */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_DE, "4K Erweiterungs ROM Datei Name bei $9*** definieren"},
/* fr */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_FR, "Spécifier le nom de l'extension ROM 4K à $9***"},
/* it */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_IT, "Specifica il nome della ROM di estensione di 4k a $9***"},
/* nl */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_NL, "Geef naam van het 4K uitbreiding ROM bestand op adres $9***"},
/* pl */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_SV, "Ange namn på 4K-utöknings-ROM på $9***"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME,    "Specify 4K extension ROM name at $A***"},
/* de */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_DE, "4K Erweiterungs ROM Datei Name bei $A*** definieren"},
/* fr */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_FR, "Spécifier le nom de l'extension ROM 4K à $A***"},
/* it */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_IT, "Specifica il nome della ROM di estensione di 4k a $A***"},
/* nl */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_NL, "Geef naam van het 4K uitbreiding ROM bestand op adres $A***"},
/* pl */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_SV, "Ange namn på 4K-utöknings-ROM på $A***"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME,    "Specify 4K extension ROM name at $B***"},
/* de */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_DE, "4K Erweiterungs ROM Datei Name bei $B*** definieren"},
/* fr */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_FR, "Spécifier le nom de l'extension ROM 4K à $B***"},
/* it */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_IT, "Specifica il nome della ROM di estensione di 4k a $B***"},
/* nl */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_NL, "Geef naam van het 4K uitbreiding ROM bestand op adres $B***"},
/* pl */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_SV, "Ange namn på 4K-utöknings-ROM på $B***"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000,    "Enable PET8296 4K RAM mapping at $9***"},
/* de */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_DE, "PET8296 4K RAM Zuordnung bei $9*** aktivieren"},
/* fr */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_FR, "Activer l'extension PET8296 4K à $9***"},
/* it */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_IT, "Attiva il mapping della RAM di 4K del PET8296 a $9***"},
/* nl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_NL, "Aktiveer PET8296 4K RAM op adres $9***"},
/* pl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_SV, "Aktivera PET8296 4K-RAM-mappning på $9***"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000,    "Disable PET8296 4K RAM mapping at $9***"},
/* de */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_DE, "PET8296 4K RAM Zuordnung bei $9*** deaktivieren"},
/* fr */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_FR, "Désactiver l'extension PET8296 4K à $9***"},
/* it */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_IT, "Disattiva il mapping della RAM di 4K del PET8296 a $9***"},
/* nl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_NL, "PET8296 4K RAM op adres $9*** afsluiten"},
/* pl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_SV, "Inaktivera PET8296 4K-RAM-mappning på $9***"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000,    "Enable PET8296 4K RAM mapping at $A***"},
/* de */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_DE, "PET8296 4K RAM Zuordnung bei $A*** aktivieren"},
/* fr */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_FR, "Activer l'extension PET8296 4K à $A***"},
/* it */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_IT, "Attiva il mapping della RAM di 4K del PET8296 a $A***"},
/* nl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_NL, "Aktiveer PET8296 4K RAM op adres $A***"},
/* pl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_SV, "Aktivera PET8296 4K-RAM-mappning på $A***"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000,    "Disable PET8296 4K RAM mapping at $A***"},
/* de */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_DE, "PET8296 4K RAM Zuordnung bei $A*** deaktivieren"},
/* fr */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_FR, "Désactiver l'extension PET8296 4K à $A***"},
/* it */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_IT, "Disattiva il mapping della RAM di 4K del PET8296 a $9***"},
/* nl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_NL, "PET8296 4K RAM op adres $A*** afsluiten"},
/* pl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_SV, "Inaktivera PET8296 4K-RAM-mappning på $A***"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SUPERPET_IO,    "Enable SuperPET I/O"},
/* de */ {IDCLS_ENABLE_SUPERPET_IO_DE, "SuperPET I/O aktivieren"},
/* fr */ {IDCLS_ENABLE_SUPERPET_IO_FR, "Activer les E/S SuperPET"},
/* it */ {IDCLS_ENABLE_SUPERPET_IO_IT, "Attiva l'I/O del SuperPET"},
/* nl */ {IDCLS_ENABLE_SUPERPET_IO_NL, "Aktiveer SuperPET I/O"},
/* pl */ {IDCLS_ENABLE_SUPERPET_IO_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SUPERPET_IO_SV, "Aktivera SuperPET-I/O"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SUPERPET_IO,    "Disable SuperPET I/O"},
/* de */ {IDCLS_DISABLE_SUPERPET_IO_DE, "SuperPET I/O deaktivieren"},
/* fr */ {IDCLS_DISABLE_SUPERPET_IO_FR, "Désactiver les E/S SuperPET"},
/* it */ {IDCLS_DISABLE_SUPERPET_IO_IT, "Disattiva l'I/O del SuperPET"},
/* nl */ {IDCLS_DISABLE_SUPERPET_IO_NL, "SuperPET I/O afsluiten"},
/* pl */ {IDCLS_DISABLE_SUPERPET_IO_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SUPERPET_IO_SV, "Inaktivera SuperPET-I/O"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES,    "Enable ROM 1 Kernal patches"},
/* de */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_DE, "ROM 1 Kernal patches aktivieren"},
/* fr */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_FR, "Activer les patches ROM 1 Kernal"},
/* it */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_IT, "Attiva le patch del Kernal della ROM 1"},
/* nl */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_NL, "Aktiveer ROM 1 Kernal verbeteringen"},
/* pl */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_SV, "Aktivera ROM 1-Kernalpatchar"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES,    "Disable ROM 1 Kernal patches"},
/* de */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_DE, "ROM 1 Kernal patches deaktivieren"},
/* fr */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_FR, "Désactiver les patches ROM 1 Kernal"},
/* it */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_IT, "Disattiva le patch del Kernal della ROM 1"},
/* nl */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_NL, "ROM 1 Kernal verbeteringen afsluiten"},
/* pl */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_SV, "Inaktivera ROM 1-Kernalpatchar"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET,    "Switch upper/lower case charset"},
/* de */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_DE, "Wechsel Groß-/Kleinschreibung Zeichensatz"},
/* fr */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_FR, "Inverser les modes majuscules et minuscules"},
/* it */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_IT, "Scambia il set di caratteri maiuscolo/minuscolo"},
/* nl */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_NL, "Schakel grote/kleine letters"},
/* pl */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_SV, "Växla små/stora bokstäver i teckengeneratorn"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET,    "Do not switch upper/lower case charset"},
/* de */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_DE, "Kein Wechsel Groß-/Kleinschreibung Zeichensatz"},
/* fr */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_FR, "Ne pas inverser les modes majuscules et minuscules"},
/* it */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_IT, "Non scambiare il set di caratteri maiuscolo/minuscolo"},
/* nl */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_NL, "Schakel niet grote/kleine letters"},
/* pl */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_SV, "Växla inte små/stora bokstäver i teckengeneratorn"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_EOI_BLANKS_SCREEN,    "EOI blanks screen"},
/* de */ {IDCLS_EOI_BLANKS_SCREEN_DE, "EOI löscht Bildschirm"},
/* fr */ {IDCLS_EOI_BLANKS_SCREEN_FR, "EOI vide l'écran"},
/* it */ {IDCLS_EOI_BLANKS_SCREEN_IT, "EOI oscura lo schermo"},
/* nl */ {IDCLS_EOI_BLANKS_SCREEN_NL, "EOI maakt het scherm leeg"},
/* pl */ {IDCLS_EOI_BLANKS_SCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_EOI_BLANKS_SCREEN_SV, "EOI tömmer skärmen"},

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN,    "EOI does not blank screen"},
/* de */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_DE, "EIO löscht Bildschirm nicht"},
/* fr */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_FR, "EOI ne vide pas l'écran"},
/* it */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_IT, "EOI non oscura lo schermo"},
/* nl */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_NL, "EOI maakt niet het scherm leeg"},
/* pl */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_SV, "EOI tömmer inte skärmen"},

/* pet/petpia1.c */
/* en */ {IDCLS_ENABLE_USERPORT_DIAG_PIN,    "Enable userport diagnostic pin"},
/* de */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_DE, "Userport Diagnose Pin aktivieren"},
/* fr */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_FR, "Activer la pin de diagnostique userport"},
/* it */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_IT, "Attiva il pin diagnostico della userport"},
/* nl */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_NL, "Aktiveer userport diagnostische pin"},
/* pl */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_SV, "Aktivera diagnostik på användarporten"},

/* pet/petpia1.c */
/* en */ {IDCLS_DISABLE_USERPORT_DIAG_PIN,    "Disable userport diagnostic pin"},
/* de */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_DE, "Userport Diagnose Pin deaktivieren"},
/* fr */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_FR, "Désactiver la pin de diagnostique userport"},
/* it */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_IT, "Disattiva il pin diagnostico della userport"},
/* nl */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_NL, "Userport diagnostische pin afsluiten"},
/* pl */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_SV, "Inaktivera diagnostik på användarporten"},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME,    "Specify name of Function low ROM image"},
/* de */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_DE, "Namen für Funktions ROM Datei (unterer Bereich) definieren"},
/* fr */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_FR, "Spécifier le nom de l'image basse ROM Fonction"},
/* it */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_IT, "Specifica il nome dell'immagine della Function ROM bassa"},
/* nl */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_NL, "Geef de naam van het Function low ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_SV, "Ange namn på ROM-avbildning för lågt funktions-ROM"},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME,    "Specify name of Function high ROM image"},
/* de */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_DE, "Namen für Funktions ROM Datei (oberer Bereich) definieren"},
/* fr */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_FR, "Spécifier le nom de l'image haute ROM Fonction"},
/* it */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_IT, "Specifica il nome dell'immagine della Function ROM alta"},
/* nl */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_NL, "Geef de naam van het Function high ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_SV, "Ange namn på ROM-avbildning för högt funktions-ROM"},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME,    "Specify name of Cartridge 1 low ROM image"},
/* de */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_DE, "Name für Erweiterungsmodul 1 ROM Datei (unterer Bereich) definieren"},
/* fr */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_FR, "Spécifier le nom de l'image basse ROM Cartouche 1"},
/* it */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM Cartridge 1 bassa"},
/* nl */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_NL, "Geef de naam van het Cartridge 1 low ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_SV, "Ange namn på ROM-avbildning för lågt insticksmodul 1-ROM"},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME,    "Specify name of Cartridge 1 high ROM image"},
/* de */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_DE, "Name für Erweiterungsmodul 1 ROM Datei (oberer Bereich) definieren"},
/* fr */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_FR, "Spécifier le nom de l'image haute ROM Cartouche 1"},
/* it */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_IT, "Specifica il nomer dell'immagine della ROM Cartridge 1 alta"},
/* nl */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_NL, "Geef de naam van het Cartridge 1 high ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_SV, "Ange namn på ROM-avbildning för högt insticksmodul 1-ROM"},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME,    "Specify name of Cartridge 2 low ROM image"},
/* de */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_DE, "Name für Erweiterungsmodul 2 ROM Datei (unterer Bereich) definieren"},
/* fr */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_FR, "Spécifier le nom de l'image basse ROM Cartouche 2"},
/* it */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_IT, "Specifica il nome dell'immagine della ROM Cartridge 2 bassa"},
/* nl */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_NL, "Geef de naam van het Cartridge 2 low ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_SV, "Ange namn på ROM-avbildning för lågt insticksmodul 2-ROM"},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME,    "Specify name of Cartridge 2 high ROM image"},
/* de */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_DE, "Name für Erweiterungsmodul 2 ROM Datei (oberer Bereich) definieren"},
/* fr */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_FR, "Spécifier le nom de l'image haute ROM Cartouche 2"},
/* it */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_IT, "Specifica il nomer dell'immagine della ROM Cartridge 2 alta"},
/* nl */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_NL, "Geef de naam van het Cartridge 2 high ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_SV, "Ange namn på ROM-avbildning för högt insticksmodul 2-ROM"},

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_RAM_INSTALLED,    "Specify size of RAM installed in kb (16/32/64)"},
/* de */ {IDCLS_SPECIFY_RAM_INSTALLED_DE, "Größe des emulierten RAM Speichers in kb (16/32/64) spezifizieren"},
/* fr */ {IDCLS_SPECIFY_RAM_INSTALLED_FR, "Spécifier la taille du RAM installé (16/32/64)"},
/* it */ {IDCLS_SPECIFY_RAM_INSTALLED_IT, "Specifica la dimensione della RAM installata in kb (16/32/64)"},
/* nl */ {IDCLS_SPECIFY_RAM_INSTALLED_NL, "Geef de hoeveelheid RAM in kb (16/32/64)"},
/* pl */ {IDCLS_SPECIFY_RAM_INSTALLED_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RAM_INSTALLED_SV, "Ange storlek på installerat RAM i kb (16/32/64)"},

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
/* de */ {IDCLS_SET_SATURATION_DE, "Sättigung für intern berechneten Palette setzen [1000]"},
/* fr */ {IDCLS_SET_SATURATION_FR, "Régler la saturation de la palette interne calculée [1000]"},
/* it */ {IDCLS_SET_SATURATION_IT, "Imposta la saturazione della palette calcolata internamente [1000]"},
/* nl */ {IDCLS_SET_SATURATION_NL, "Zet de verzadiging van het intern berekend kleuren palette [1000]"},
/* pl */ {IDCLS_SET_SATURATION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SATURATION_SV, "Ställ in mättnad på internt beräknad palett [1000]"},

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_CONTRAST,    "Set contrast of internal calculated palette [1100]"},
/* de */ {IDCLS_SET_CONTRAST_DE, "Kontrast für intern berechneten Palette setzen [1100]"},
/* fr */ {IDCLS_SET_CONTRAST_FR, "Régler le contraste de la palette interne calculée [1100]"},
/* it */ {IDCLS_SET_CONTRAST_IT, "Imposta il constrasto della palette calcolata internamente [1100]"},
/* nl */ {IDCLS_SET_CONTRAST_NL, "Zet het contrast van het intern berekend kleuren palette [1100]"},
/* pl */ {IDCLS_SET_CONTRAST_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_CONTRAST_SV, "Ställ in kontrast på internt beräknad palett [1100]"},

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_BRIGHTNESS,    "Set brightness of internal calculated palette [1100]"},
/* de */ {IDCLS_SET_BRIGHTNESS_DE, "Helliogkeit für intern berechneten Palette setzen [1100]"},
/* fr */ {IDCLS_SET_BRIGHTNESS_FR, "Régler la luminosité de la palette interne calculée [1100]"},
/* it */ {IDCLS_SET_BRIGHTNESS_IT, "Imposta la luminanza della palette calcolata internamente [1100]"},
/* nl */ {IDCLS_SET_BRIGHTNESS_NL, "Zet de helderheid van het intern berekend kleuren palette [1100]"},
/* pl */ {IDCLS_SET_BRIGHTNESS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_BRIGHTNESS_SV, "Ställ in ljusstyrka på internt beräknad palett [1100]"},

/* plus4/ted-cmdline-options.c`, vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_GAMMA,    "Set gamma of internal calculated palette [900]"},
/* de */ {IDCLS_SET_GAMMA_DE, "Gamma für intern berechneten Palette setzen [900]"},
/* fr */ {IDCLS_SET_GAMMA_FR, "Régler le gamma de la palette interne calculée [900]"},
/* it */ {IDCLS_SET_GAMMA_IT, "Imposta la gamma della palette calcolata internamente [900]"},
/* nl */ {IDCLS_SET_GAMMA_NL, "Zet de gamma van het intern berekend kleuren palette [900]"},
/* pl */ {IDCLS_SET_GAMMA_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_GAMMA_SV, "Ställ in gamma på internt beräknad palett [900]"},

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME,    "Specify name of printer driver for device #4"},
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_DE, "Name für Druckertreiber Gerät #4 definieren"},
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_FR, "Spécifier le nom du pilote imprimante pour le périphérique #4"},
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_IT, "Specifica il nome del driver di stampa per la periferica #4"},
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_NL, "Geef de naam van het stuurprogramma voor apparaat #4"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_SV, "Ange namn på skrivardrivrutin för enhet 4"},

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME,    "Specify name of printer driver for device #5"},
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_DE, "Name für Druckertreiber Gerät #5 definieren"},
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_FR, "Spécifier le nom du pilote imprimante pour le périphérique #5"},
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_IT, "Specifica il nome del driver di stampa per la periferica #5"},
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_NL, "Geef de naam van het stuurprogramma voor apparaat #5"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_SV, "Ange namn på skrivardrivrutin för enhet 5"},

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME,    "Specify name of printer driver for the userport printer"},
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_DE, "Name für Userport Druckertreiber definieren"},
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_FR, "Spécifier le nom du pilote imprimante pour l'imprimante userport"},
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_IT, "Specifica il nome del driver di stampa per la stampante su userport"},
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_NL, "Geef de naam van het stuurprogramma voor de userport printer"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_SV, "Ange namn på skrivardrivrutin för användarporten"},

/* printerdrv/interface-serial.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_4,    "Set device type for device #4 (0: NONE, 1: FS, 2: REAL)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_4_DE, "Geräte Typ für Gerät #4 (0: NONE, 1: FS, 2: REAL) setzen"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_4_FR, "Spécifier le type de périphérique pour #4 (0: AUCUN, 1: SF, 2: RÉEL)"},
/* it */ {IDCLS_SET_DEVICE_TYPE_4_IT, "Specifica il tipo di periferica #4 (0 NESSUNA, 1 FS, 2 REALE)"},
/* nl */ {IDCLS_SET_DEVICE_TYPE_4_NL, "Zet apparaat soort voor apparaat #4 (0: GEEN, 1: FS, 2: ECHT)"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_4_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_4_SV, "Ange enhetstyp för enhet 4 (0: INGEN, 1: FS, 2: ÄKTA)"},

/* printerdrv/interface-serial.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_5,    "Set device type for device #5 (0: NONE, 1: FS, 2: REAL)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_5_DE, "Geräte Typ für Gerät #5 (0: NONE, 1: FS, 2: REAL) setzen"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_5_FR, "Spécifier le type de périphérique pour #5 (0: AUCUN, 1: SF, 2: RÉEL)"},
/* it */ {IDCLS_SET_DEVICE_TYPE_5_IT, "Specifica il tipo di periferica #4 (0 NESSUNA, 1 FS, 2 REALE)"},
/* nl */ {IDCLS_SET_DEVICE_TYPE_5_NL, "Zet apparaat soort voor apparaat #5 (0: GEEN, 1: FS, 2: ECHT)"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_5_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_5_SV, "Ange enhetstyp för enhet 5 (0: INGEN, 1: FS, 2: ÄKTA)"},

/* printerdrv/interface-userport.c */
/* en */ {IDCLS_ENABLE_USERPORT_PRINTER,    "Enable the userport printer emulation"},
/* de */ {IDCLS_ENABLE_USERPORT_PRINTER_DE, "Userport Drucker Emulation aktivieren"},
/* fr */ {IDCLS_ENABLE_USERPORT_PRINTER_FR, "Acviver l'émulation de l'imprimante userport"},
/* it */ {IDCLS_ENABLE_USERPORT_PRINTER_IT, "Attiva l'emulazione della stampante su userport"},
/* nl */ {IDCLS_ENABLE_USERPORT_PRINTER_NL, "Aktiveer de userport printer emulatie"},
/* pl */ {IDCLS_ENABLE_USERPORT_PRINTER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_USERPORT_PRINTER_SV, "Aktivera emulering av skrivare på användarporten"},

/* printerdrv/interface-userport.c */
/* en */ {IDCLS_DISABLE_USERPORT_PRINTER,    "Disable the userport printer emulation"},
/* de */ {IDCLS_DISABLE_USERPORT_PRINTER_DE, "Userport Drucker Emulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_USERPORT_PRINTER_FR, "Désactiver l'émulation de l'imprimante userport"},
/* it */ {IDCLS_DISABLE_USERPORT_PRINTER_IT, "Disttiva l'emulazione della stampante su userport"},
/* nl */ {IDCLS_DISABLE_USERPORT_PRINTER_NL, "De userport printer emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_USERPORT_PRINTER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_USERPORT_PRINTER_SV, "Inaktivera emulering av skrivare på användarporten"},

/* printerdrv/output-select.c */
/* en */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME,    "Specify name of output device for device #4"},
/* de */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_DE, "Name für Druckerausgabe Datei Gerät #4 definieren"},
/* fr */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_FR, "Spécifiez le nom du périphérique de sortie pour le périphérique #4"},
/* it */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_IT, "Specifica il nome del dispositivo di output per la periferica #4"},
/* nl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_NL, "Geef de naam van het uitvoer apparaat voor apparaat #4"},
/* pl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_SV, "Ange namn på utskriftsenhet för enhet 4"},

/* printerdrv/output-select.c */
/* en */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME,    "Specify name of output device for device #5"},
/* de */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_DE, "Name für Druckerausgabe Datei Gerät #5 definieren"},
/* fr */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_FR, "Spécifiez le nom du périphérique de sortie pour le périphérique #5"},
/* it */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_IT, "Specifica il nome del dispositivo di output per la periferica #4"},
/* nl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_NL, "Geef de naam van het uitvoer apparaat voor apparaat #5"},
/* pl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_SV, "Ange namn på utskriftsenhet för enhet 5"},

/* printerdrv/output-select.c */
/* en */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME,    "Specify name of output device for the userport printer"},
/* de */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_DE, "Name für Druckerausgabe Datei Userport definieren"},
/* fr */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_FR, "Spécifiez le nom du périphérique de sortie pour l'imprimante userport"},
/* it */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_IT, "Specifica il nome del dispositivo di output per la stampante su userport"},
/* nl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_NL, "Geef de naam van het uitvoer apparaat voor de userport printer"},
/* pl */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_SV, "Ange namn på utskriftsenhet för användarporten"},

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME,    "Specify name of printer text device or dump file"},
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_DE, "Name für Druckerausgabe Textgerät oder Dump Datei definieren"},
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_FR, "Spécifier le nom de l'imprimante text ou \"dump file\""},
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_IT, "Specifica il nome del dispositivo testuale di stampa o salva su file"},
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_NL, "Geef de naam van het text apparaat of het dump bestand"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_SV, "Ange namn på textenhet för utskrift eller dumpfil"},

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
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_4_DE, "IEC Drucker #4 Gerät für Textausgabe definieren"},
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_4_FR, "Spécifier le périphérique de sortie text pour l'imprimante IEC #4"},
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_4_IT, "Specifica il nome del dispositivo di output testuale di stampa per la stampante IEC #4"},
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_4_NL, "Geef het text uitvoer apparaat voor IEC printer #4"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_4_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_4_SV, "Ange skrivartextutenhet för IEC-skrivare 4"},

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_5,    "Specify printer text output device for IEC printer #5"},
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_5_DE, "IEC Drucker #5 Gerät für Textausgabe definieren"},
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_5_FR, "Spécifier le périphérique de sortie text pour l'imprimante IEC #5"},
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_5_IT, "Specifica il nome del dispositivo di output testuale di stampa per la stampante IEC #5"},
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_5_NL, "Geef het text uitvoer apparaat voor IEC printer #5"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_5_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_5_SV, "Ange skrivartextutenhet för IEC-skrivare 5"},

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_USERPORT,    "Specify printer text output device for userport printer"},
/* de */ {IDCLS_SPECIFY_TEXT_USERPORT_DE, "IEC Userport Drucker Gerät für Textausgabe definieren"},
/* fr */ {IDCLS_SPECIFY_TEXT_USERPORT_FR, "Spécifier le périphérique de sortie text pour l'imprimante userport"},
/* it */ {IDCLS_SPECIFY_TEXT_USERPORT_IT, "Specifica il nome del dispositivo di output testuale di stampa per la stampante su userport"},
/* nl */ {IDCLS_SPECIFY_TEXT_USERPORT_NL, "Geef het text uitvoer apparaat voor de userport printer"},
/* pl */ {IDCLS_SPECIFY_TEXT_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_USERPORT_SV, "Ange skrivartextutenhet för användarportsskrivare"},

/* raster/raster-cmdline-options.c */
/* en */ {IDCLS_ENABLE_VIDEO_CACHE,    "Enable the video cache"},
/* de */ {IDCLS_ENABLE_VIDEO_CACHE_DE, "Video Cache aktivieren"},
/* fr */ {IDCLS_ENABLE_VIDEO_CACHE_FR, "Activer le cache vidéo"},
/* it */ {IDCLS_ENABLE_VIDEO_CACHE_IT, "Attiva la cache video"},
/* nl */ {IDCLS_ENABLE_VIDEO_CACHE_NL, "Aktiveer de video cache"},
/* pl */ {IDCLS_ENABLE_VIDEO_CACHE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_VIDEO_CACHE_SV, "Aktivera videocache"},

/* raster/raster-cmdline-options.c */
/* en */ {IDCLS_DISABLE_VIDEO_CACHE,    "Disable the video cache"},
/* de */ {IDCLS_DISABLE_VIDEO_CACHE_DE, "Video Cache deaktivieren"},
/* fr */ {IDCLS_DISABLE_VIDEO_CACHE_FR, "Désactiver le cache vidéo"},
/* it */ {IDCLS_DISABLE_VIDEO_CACHE_IT, "Disattiva la cache video"},
/* nl */ {IDCLS_DISABLE_VIDEO_CACHE_NL, "De video cache afsluiten"},
/* pl */ {IDCLS_DISABLE_VIDEO_CACHE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_VIDEO_CACHE_SV, "Inaktivera videocache"},

#ifdef HAVE_RS232
/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_1_NAME,    "Specify name of first RS232 device"},
/* de */ {IDCLS_SPECIFY_RS232_1_NAME_DE, "Name für erstes RS232 Gerät definieren"},
/* fr */ {IDCLS_SPECIFY_RS232_1_NAME_FR, "Spécifier le nom du 1er périphérique RS232"},
/* it */ {IDCLS_SPECIFY_RS232_1_NAME_IT, "Specifica il nome della prima RS232"},
/* nl */ {IDCLS_SPECIFY_RS232_1_NAME_NL, "Geef de naam van het eerste RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_1_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_1_NAME_SV, "Ange namn på första RS232-enheten"},

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_2_NAME,    "Specify name of second RS232 device"},
/* de */ {IDCLS_SPECIFY_RS232_2_NAME_DE, "Name für zweites RS232 Gerät definieren"},
/* fr */ {IDCLS_SPECIFY_RS232_2_NAME_FR, "Spécifier le nom du 2e périphérique RS232"},
/* it */ {IDCLS_SPECIFY_RS232_2_NAME_IT, "Specifica il nome della seconda RS232"},
/* nl */ {IDCLS_SPECIFY_RS232_2_NAME_NL, "Geef de naam van het tweede RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_2_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_2_NAME_SV, "Ange namn på andra RS232-enheten"},

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_3_NAME,    "Specify name of third RS232 device"},
/* de */ {IDCLS_SPECIFY_RS232_3_NAME_DE, "Name für drittes RS232 Gerät definieren"},
/* fr */ {IDCLS_SPECIFY_RS232_3_NAME_FR, "Spécifier le nom du 3e périphérique RS232"},
/* it */ {IDCLS_SPECIFY_RS232_3_NAME_IT, "Specifica il nome della terza RS232"},
/* nl */ {IDCLS_SPECIFY_RS232_3_NAME_NL, "Geef de naam van het derde RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_3_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_3_NAME_SV, "Ange namn på tredje RS232-enheten"},

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_4_NAME,    "Specify name of fourth RS232 device"},
/* de */ {IDCLS_SPECIFY_RS232_4_NAME_DE, "Name für viertes RS232 Gerät definieren"},
/* fr */ {IDCLS_SPECIFY_RS232_4_NAME_FR, "Spécifier le nom du 4e périphérique RS232"},
/* it */ {IDCLS_SPECIFY_RS232_4_NAME_IT, "Specifica il nome della quarta RS232"},
/* nl */ {IDCLS_SPECIFY_RS232_4_NAME_NL, "Geef de naam van het vierde RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_4_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_4_NAME_SV, "Ange namn på fjärde RS232-enheten"},
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_ENABLE_RS232_USERPORT,    "Enable RS232 userport emulation"},
/* de */ {IDCLS_ENABLE_RS232_USERPORT_DE, "RS232 Userport Emulation aktivieren"},
/* fr */ {IDCLS_ENABLE_RS232_USERPORT_FR, "Activer l'émulation de périphérique RS232 userport"},
/* it */ {IDCLS_ENABLE_RS232_USERPORT_IT, "Attiva l'emulazione della RS232 su userport"},
/* nl */ {IDCLS_ENABLE_RS232_USERPORT_NL, "Aktiveer RS232 userport emulatie"},
/* pl */ {IDCLS_ENABLE_RS232_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_RS232_USERPORT_SV, "Aktivera RS232-emulering på användarporten"},

/* rs232drv/rsuser.c */
/* en */ {IDCLS_DISABLE_RS232_USERPORT,    "Disable RS232 userport emulation"},
/* de */ {IDCLS_DISABLE_RS232_USERPORT_DE, "RS232 Userport Emulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_RS232_USERPORT_FR, "Désactiver l'émulation de périphérique RS232 userport"},
/* it */ {IDCLS_DISABLE_RS232_USERPORT_IT, "Disattiva l'emulazione della RS232 su userport"},
/* nl */ {IDCLS_DISABLE_RS232_USERPORT_NL, "RS232 userport emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_RS232_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_RS232_USERPORT_SV, "Inaktivera RS232-emulering på användarporten"},

/* rs232drv/rsuser.c */
/* en */ {IDCLS_P_BAUD,    "<baud>"},
/* de */ {IDCLS_P_BAUD_DE, "<Baud>"},
/* fr */ {IDCLS_P_BAUD_FR, "<baud>"},
/* it */ {IDCLS_P_BAUD_IT, "<baud>"},
/* nl */ {IDCLS_P_BAUD_NL, "<baud>"},
/* pl */ {IDCLS_P_BAUD_PL, "<baud>"},
/* sv */ {IDCLS_P_BAUD_SV, "<baud>"},

/* rs232drv/rsuser.c */
/* en */ {IDCLS_SET_BAUD_RS232_USERPORT,    "Set the baud rate of the RS232 userport emulation."},
/* de */ {IDCLS_SET_BAUD_RS232_USERPORT_DE, "Baudrate für RS232 Userport Emulation setzen"},
/* fr */ {IDCLS_SET_BAUD_RS232_USERPORT_FR, "Spécifiez la vitesse en baud du périphérique RS232 userport"},
/* it */ {IDCLS_SET_BAUD_RS232_USERPORT_IT, "Imposta la velocità della RS232 su userport (in baud)"},
/* nl */ {IDCLS_SET_BAUD_RS232_USERPORT_NL, "Zet de baud rate van de RS232 userport emulatie."},
/* pl */ {IDCLS_SET_BAUD_RS232_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_BAUD_RS232_USERPORT_SV, "Ange baudhastighet för RS232-emuleringen på användarporten."},

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
/* de */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_DE, "VICE RS232 Gerät für Userport definieren"},
/* fr */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_FR, "Spécifier un périphérique VICE RS232 pour le userport"},
/* it */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_IT, "Specifica il dispositivo RS232 di VICE su userport"},
/* nl */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_NL, "Geef VICE RS232 apparaat voor userport"},
/* pl */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_SV, "Ange VICE RS232-enhet för användarporten"},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_4,    "Enable IEC device emulation for device #4"},
/* de */ {IDCLS_ENABLE_IEC_4_DE, "IEC Geräte Emulation für Gerät #4 aktivieren"},
/* fr */ {IDCLS_ENABLE_IEC_4_FR, "Activer l'émulation IEC pour le périphérique #4"},
/* it */ {IDCLS_ENABLE_IEC_4_IT, "Attiva l'emulazione IEC per la periferica #4"},
/* nl */ {IDCLS_ENABLE_IEC_4_NL, "Aktiveer IEC apparaat emulatie voor apparaat #4"},
/* pl */ {IDCLS_ENABLE_IEC_4_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_4_SV, "Aktivera IEC-enhetsemulering för enhet 4"},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_4,    "Disable IEC device emulation for device #4"},
/* de */ {IDCLS_DISABLE_IEC_4_DE, "IEC Geräte Emulation für Gerät #4 deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEC_4_FR, "Désactiver l'émulation IEC pour le périphérique #4"},
/* it */ {IDCLS_DISABLE_IEC_4_IT, "Disattiva l'emulazione IEC per la periferica #4"},
/* nl */ {IDCLS_DISABLE_IEC_4_NL, "IEC apparaat emulatie voor apparaat #4 afsluiten"},
/* pl */ {IDCLS_DISABLE_IEC_4_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_4_SV, "Inaktivera IEC-enhetsemulering för enhet 4"},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_5,    "Enable IEC device emulation for device #5"},
/* de */ {IDCLS_ENABLE_IEC_5_DE, "IEC Geräte Emulation für Gerät #5 aktivieren"},
/* fr */ {IDCLS_ENABLE_IEC_5_FR, "Activer l'émulation IEC pour le périphérique #5"},
/* it */ {IDCLS_ENABLE_IEC_5_IT, "Attiva l'emulazione IEC per la periferica #5"},
/* nl */ {IDCLS_ENABLE_IEC_5_NL, "Aktiveer IEC apparaat emulatie voor apparaat #5"},
/* pl */ {IDCLS_ENABLE_IEC_5_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_5_SV, "Aktivera IEC-enhetsemulering för enhet 5"},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_5,    "Disable IEC device emulation for device #5"},
/* de */ {IDCLS_DISABLE_IEC_5_DE, "IEC Geräte Emulation für Gerät #5 deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEC_5_FR, "Désactiver l'émulation IEC pour le périphérique #5"},
/* it */ {IDCLS_DISABLE_IEC_5_IT, "Disattiva l'emulazione IEC per la periferica #5"},
/* nl */ {IDCLS_DISABLE_IEC_5_NL, "IEC apparaat emulatie voor apparaat #5 afsluiten"},
/* pl */ {IDCLS_DISABLE_IEC_5_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_5_SV, "Inaktivera IEC-enhetsemulering för enhet 5"},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_8,    "Enable IEC device emulation for device #8"},
/* de */ {IDCLS_ENABLE_IEC_8_DE, "IEC Geräte Emulation für Gerät #8 aktivieren"},
/* fr */ {IDCLS_ENABLE_IEC_8_FR, "Activer l'émulation IEC pour le périphérique #8"},
/* it */ {IDCLS_ENABLE_IEC_8_IT, "Attiva l'emulazione IEC per la periferica #8"},
/* nl */ {IDCLS_ENABLE_IEC_8_NL, "Aktiveer IEC apparaat emulatie voor apparaat #8"},
/* pl */ {IDCLS_ENABLE_IEC_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_8_SV, "Aktivera IEC-enhetsemulering för enhet 8"},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_8,    "Disable IEC device emulation for device #8"},
/* de */ {IDCLS_DISABLE_IEC_8_DE, "IEC Geräte Emulation für Gerät #8 deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEC_8_FR, "Désactiver l'émulation IEC pour le périphérique #8"},
/* it */ {IDCLS_DISABLE_IEC_8_IT, "Disattiva l'emulazione IEC per la periferica #8"},
/* nl */ {IDCLS_DISABLE_IEC_8_NL, "IEC apparaat emulatie voor apparaat #8 afsluiten"},
/* pl */ {IDCLS_DISABLE_IEC_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_8_SV, "Inaktivera IEC-enhetsemulering för enhet 8"},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_9,    "Enable IEC device emulation for device #9"},
/* de */ {IDCLS_ENABLE_IEC_9_DE, "IEC Geräte Emulation für Gerät #9 aktivieren"},
/* fr */ {IDCLS_ENABLE_IEC_9_FR, "Activer l'émulation IEC pour le périphérique #9"},
/* it */ {IDCLS_ENABLE_IEC_9_IT, "Attiva l'emulazione IEC per la periferica #9"},
/* nl */ {IDCLS_ENABLE_IEC_9_NL, "Aktiveer IEC apparaat emulatie voor apparaat #9"},
/* pl */ {IDCLS_ENABLE_IEC_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_9_SV, "Aktivera IEC-enhetsemulering för enhet 9"},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_9,    "Disable IEC device emulation for device #9"},
/* de */ {IDCLS_DISABLE_IEC_9_DE, "IEC Geräte Emulation für Gerät #9 deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEC_9_FR, "Désactiver l'émulation IEC pour le périphérique #9"},
/* it */ {IDCLS_DISABLE_IEC_9_IT, "Disattiva l'emulazione IEC per la periferica #9"},
/* nl */ {IDCLS_DISABLE_IEC_9_NL, "IEC apparaat emulatie voor apparaat #9 afsluiten"},
/* pl */ {IDCLS_DISABLE_IEC_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_9_SV, "Inaktivera IEC-enhetsemulering för enhet 9"},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_10,    "Enable IEC device emulation for device #10"},
/* de */ {IDCLS_ENABLE_IEC_10_DE, "IEC Geräte Emulation für Gerät #10 aktivieren"},
/* fr */ {IDCLS_ENABLE_IEC_10_FR, "Activer l'émulation IEC pour le périphérique #10"},
/* it */ {IDCLS_ENABLE_IEC_10_IT, "Attiva l'emulazione IEC per la periferica #10"},
/* nl */ {IDCLS_ENABLE_IEC_10_NL, "Aktiveer IEC apparaat emulatie voor apparaat #10"},
/* pl */ {IDCLS_ENABLE_IEC_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_10_SV, "Aktivera IEC-enhetsemulering för enhet 10"},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_10,    "Disable IEC device emulation for device #10"},
/* de */ {IDCLS_DISABLE_IEC_10_DE, "IEC Geräte Emulation für Gerät #10 deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEC_10_FR, "Désactiver l'émulation IEC pour le périphérique #10"},
/* it */ {IDCLS_DISABLE_IEC_10_IT, "Disattiva l'emulazione IEC per la periferica #10"},
/* nl */ {IDCLS_DISABLE_IEC_10_NL, "IEC apparaat emulatie voor apparaat #10 afsluiten"},
/* pl */ {IDCLS_DISABLE_IEC_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_10_SV, "Inaktivera IEC-enhetsemulering för enhet 10"},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_11,    "Enable IEC device emulation for device #11"},
/* de */ {IDCLS_ENABLE_IEC_11_DE, "IEC Geräte Emulation für Gerät #11 aktivieren"},
/* fr */ {IDCLS_ENABLE_IEC_11_FR, "Activer l'émulation IEC pour le périphérique #11"},
/* it */ {IDCLS_ENABLE_IEC_11_IT, "Attiva l'emulazione IEC per la periferica #11"},
/* nl */ {IDCLS_ENABLE_IEC_11_NL, "Aktiveer IEC apparaat emulatie voor apparaat #11"},
/* pl */ {IDCLS_ENABLE_IEC_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_11_SV, "Aktivera IEC-enhetsemulering för enhet 11"},

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_11,    "Disable IEC device emulation for device #11"},
/* de */ {IDCLS_DISABLE_IEC_11_DE, "IEC Geräte Emulation für Gerät #11 deaktivieren"},
/* fr */ {IDCLS_DISABLE_IEC_11_FR, "Désactiver l'émulation IEC pour le périphérique #11"},
/* it */ {IDCLS_DISABLE_IEC_11_IT, "Disattiva l'emulazione IEC per la periferica #11"},
/* nl */ {IDCLS_DISABLE_IEC_11_NL, "IEC apparaat emulatie voor apparaat #11 afsluiten"},
/* pl */ {IDCLS_DISABLE_IEC_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_11_SV, "Inaktivera IEC-enhetsemulering för enhet 11"},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_P_ENGINE,    "<engine>"},
/* de */ {IDCLS_P_ENGINE_DE, "<Engine>"},
/* fr */ {IDCLS_P_ENGINE_FR, "<engin>"},
/* it */ {IDCLS_P_ENGINE_IT, "<motore>"},
/* nl */ {IDCLS_P_ENGINE_NL, "<kern>"},
/* pl */ {IDCLS_P_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_ENGINE_SV, "<motor>"},

#if !defined(HAVE_RESID) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if defined(HAVE_RESID) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 1: ReSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·1:Â·ReSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 1: ReSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if !defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 2: Catweasel)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: Catweasel)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·2:Â·Catweasel)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 2: Catweasel)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if !defined(HAVE_RESID) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 3: HardSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 3: HardSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·3:Â·HardSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 3: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if !defined(HAVE_RESID) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·4:Â·ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 2: Catweasel)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·1:Â·ReSID,Â·2:Â·Catweasel)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 1: ReSID, 2: Catweasel)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if defined(HAVE_RESID) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 1: ReSID, 3: HardSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 3: HardSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·1:Â·ReSID,Â·3:Â·HardSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 1: ReSID, 3: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if defined(HAVE_RESID) && !defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 1: ReSID, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·1:Â·ReSID,Â·4:Â·ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 1: ReSID, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if !defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 2: Catweasel, 3: HardSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: Catweasel, 3: HardSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·2:Â·Catweasel,Â·3:Â·HardSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 2: Catweasel, 3: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if !defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 2: Catweasel, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: Catweasel, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·2:Â·Catweasel,Â·4:Â·ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 2: Catweasel, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if !defined(HAVE_RESID) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 3: HardSID, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 3: HardSID, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·3:Â·HardSID,Â·4:Â·ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 3: HardSID, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && !defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·1:Â·ReSID,Â·2:Â·Catweasel,Â·3:Â·HardSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && !defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·1:Â·ReSID,Â·2:Â·Catweasel,Â·4:Â·ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if defined(HAVE_RESID) && !defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·1:Â·ReSID,Â·3:Â·HardSID,Â·4:Â·ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if !defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·2:Â·Catweasel,Â·3:Â·HardSID,Â·4:Â·ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

#if defined(HAVE_RESID) && defined(HAVE_CATWEASELMKIII) && defined(HAVE_HARDSID) && defined(HAVE_PARSID)
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_ENGINE,    "Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* de */ {IDCLS_SPECIFY_SID_ENGINE_DE, "SID Engine wählen (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* fr */ {IDCLS_SPECIFY_SID_ENGINE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SID_ENGINE_IT, "Specifica il motore SID (0:Â·FastSID,Â·1:Â·ReSID,Â·2:Â·Catweasel,Â·3:Â·HardSID,Â·4:Â·ParSID)"},
/* nl */ {IDCLS_SPECIFY_SID_ENGINE_NL, "Geef op welke SID kern gebruikt moet worden (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID)"},
/* pl */ {IDCLS_SPECIFY_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_ENGINE_SV, ""},  /* fuzzy */
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SECOND_SID,    "Enable second SID"},
/* de */ {IDCLS_ENABLE_SECOND_SID_DE, "Zweiten SID aktivieren"},
/* fr */ {IDCLS_ENABLE_SECOND_SID_FR, "Activer le second SID"},
/* it */ {IDCLS_ENABLE_SECOND_SID_IT, "Attiva Secondo SID"},
/* nl */ {IDCLS_ENABLE_SECOND_SID_NL, "Aktiveer stereo SID"},
/* pl */ {IDCLS_ENABLE_SECOND_SID_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SECOND_SID_SV, "Aktivera andra SID"},

/* sid/sid-cmdline-options.c, c64/plus60k, c64/c64_256k.c */
/* en */ {IDCLS_P_BASE_ADDRESS,    "<base address>"},
/* de */ {IDCLS_P_BASE_ADDRESS_DE, "<Basis Adresse>"},
/* fr */ {IDCLS_P_BASE_ADDRESS_FR, "<adresse de base>"},
/* it */ {IDCLS_P_BASE_ADDRESS_IT, "<indirizzo base>"},
/* nl */ {IDCLS_P_BASE_ADDRESS_NL, "<basis adres>"},
/* pl */ {IDCLS_P_BASE_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_BASE_ADDRESS_SV, "<basadress>"},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_2_ADDRESS,    "Specify base address for 2nd SID"},
/* de */ {IDCLS_SPECIFY_SID_2_ADDRESS_DE, "Basis Adresse für zweiten SID definieren"},
/* fr */ {IDCLS_SPECIFY_SID_2_ADDRESS_FR, "Spécifier l'adresse de base pour le second SID"},
/* it */ {IDCLS_SPECIFY_SID_2_ADDRESS_IT, "Specifica l'indirizzo di base per il secondo SID"},
/* nl */ {IDCLS_SPECIFY_SID_2_ADDRESS_NL, "Geef het basis adres van de 2e SID"},
/* pl */ {IDCLS_SPECIFY_SID_2_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_2_ADDRESS_SV, "Ange basadress för andra SID"},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_P_MODEL,    "<model>"},
/* de */ {IDCLS_P_MODEL_DE, "<Modell>"},
/* fr */ {IDCLS_P_MODEL_FR, "<modèle>"},
/* it */ {IDCLS_P_MODEL_IT, "<modello>"},
/* nl */ {IDCLS_P_MODEL_NL, "<model>"},
/* pl */ {IDCLS_P_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_MODEL_SV, "<modell>"},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SID_MODEL,    "Specify SID model (0: 6581, 1: 8580, 2: 8580 + digi boost)"},
/* de */ {IDCLS_SPECIFY_SID_MODEL_DE, "SID Modell definieren (0: 6581, 1: 8580, 2: 8580 + digi boost)"},
/* fr */ {IDCLS_SPECIFY_SID_MODEL_FR, "Spécifier le modèle SID (0: 6581, 1: 8580, 2: 8580 + Boost digital)"},
/* it */ {IDCLS_SPECIFY_SID_MODEL_IT, "Specifica il modello di SID (0: 6581, 1: 8580, 2: 8580 + digi boost)"},
/* nl */ {IDCLS_SPECIFY_SID_MODEL_NL, "Geef het SID model (0: 6581, 1: 8580, 2: 8580 + digi boost)"},
/* pl */ {IDCLS_SPECIFY_SID_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SID_MODEL_SV, "Ange SID-modell (0: 6581, 1: 8580, 2: 8580 + digiförstärkning)"},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SID_FILTERS,    "Emulate SID filters"},
/* de */ {IDCLS_ENABLE_SID_FILTERS_DE, "SID Filter Emulation aktivieren"},
/* fr */ {IDCLS_ENABLE_SID_FILTERS_FR, "Émuler les filtres SID"},
/* it */ {IDCLS_ENABLE_SID_FILTERS_IT, "Emula i filtri del SID"},
/* nl */ {IDCLS_ENABLE_SID_FILTERS_NL, "SID filters emuleren"},
/* pl */ {IDCLS_ENABLE_SID_FILTERS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SID_FILTERS_SV, "Emulera SID-filter"},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SID_FILTERS,    "Do not emulate SID filters"},
/* de */ {IDCLS_DISABLE_SID_FILTERS_DE, "SID Filter Emulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_SID_FILTERS_FR, "Ne pas émuler les filtres SID"},
/* it */ {IDCLS_DISABLE_SID_FILTERS_IT, "Non emulare i filtri del SID"},
/* nl */ {IDCLS_DISABLE_SID_FILTERS_NL, "SID filters niet emuleren"},
/* pl */ {IDCLS_DISABLE_SID_FILTERS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SID_FILTERS_SV, "Emulera inte SID-filter"},

#ifdef HAVE_RESID
/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_RESID_SAMPLING_METHOD,    "reSID sampling method (0: fast, 1: interpolating, 2: resampling, 3: fast resampling)"},
/* de */ {IDCLS_RESID_SAMPLING_METHOD_DE, "reSID Sample Methode (0: schnell, 1: interpolierend, 2: resampling, 3: schnelles resampling)"},
/* fr */ {IDCLS_RESID_SAMPLING_METHOD_FR, "Méthode reSID (0: rapide, 1: interpolation, 2: rééchantillonnage, 3: rééchantillonnage rapide)"},
/* it */ {IDCLS_RESID_SAMPLING_METHOD_IT, "Metodo di campionamento del reSID (0: veloce, 1: interpolato,2: ricampionato 3: ricampionamento veloce)"},
/* nl */ {IDCLS_RESID_SAMPLING_METHOD_NL, "reSID sampling methode (0: snel, 1: interpoleren, 2: resampling, 3: snelle resampling)"},
/* pl */ {IDCLS_RESID_SAMPLING_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RESID_SAMPLING_METHOD_SV, "reSID-samplingsmetod (0: snabb, 1: interpolerande, 2: omsamplning, 3: snabb omsampling)"},

/* sid/sid-cmdline-options.c, vsync.c */
/* en */ {IDCLS_P_PERCENT,    "<percent>"},
/* de */ {IDCLS_P_PERCENT_DE, "<prozent>"},
/* fr */ {IDCLS_P_PERCENT_FR, "<pourcent>"},
/* it */ {IDCLS_P_PERCENT_IT, "<percento>"},
/* nl */ {IDCLS_P_PERCENT_NL, "<procent>"},
/* pl */ {IDCLS_P_PERCENT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_PERCENT_SV, "<procent>"},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_PASSBAND_PERCENTAGE,    "reSID resampling passband in percentage of total bandwidth (0 - 90)"},
/* de */ {IDCLS_PASSBAND_PERCENTAGE_DE, "reSID Resampling Passband Prozentwert der gesamte Bandbreite (0 - 90)\n(0 - 90, niedrig ist schneller, höher ist besser)"},
/* fr */ {IDCLS_PASSBAND_PERCENTAGE_FR, "Bande passante pour le resampling reSID en pourcentage de la bande totale (0 - 90)"},
/* it */ {IDCLS_PASSBAND_PERCENTAGE_IT, "Banda passante di ricampionamento del reSID in percentuale di quella totale (0 - 90)"},
/* nl */ {IDCLS_PASSBAND_PERCENTAGE_NL, "reSID resampling passband in percentage van de totale bandbreedte (0 - 90)"},
/* pl */ {IDCLS_PASSBAND_PERCENTAGE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PASSBAND_PERCENTAGE_SV, "Passband för reSID-resampling i procent av total bandbredd (0 - 90)"},

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_RESID_GAIN_PERCENTAGE,    "reSID gain in percent (90 - 100)"},
/* de */ {IDCLS_RESID_GAIN_PERCENTAGE_DE, "reSID Gain in Prozent (90 - 100)"},
/* fr */ {IDCLS_RESID_GAIN_PERCENTAGE_FR, "Gain reSID en pourcent (90 - 100)"},
/* it */ {IDCLS_RESID_GAIN_PERCENTAGE_IT, "Guadagno del reSID in percentuale (90 - 100)"},
/* nl */ {IDCLS_RESID_GAIN_PERCENTAGE_NL, "reSID versterking procent (90 - 100)"},
/* pl */ {IDCLS_RESID_GAIN_PERCENTAGE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RESID_GAIN_PERCENTAGE_SV, "Gain för reSID i procent (90 - 100)"},
#endif

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_MEMORY_16KB,    "Set the VDC memory size to 16KB"},
/* de */ {IDCLS_SET_VDC_MEMORY_16KB_DE, "VDC Speichgröße auf 16KB setzen"},
/* fr */ {IDCLS_SET_VDC_MEMORY_16KB_FR, "Régler la taille de la mémoire VDC à 16KO"},
/* it */ {IDCLS_SET_VDC_MEMORY_16KB_IT, "Imposta la dimensione della memoria del VDC a 16KB"},
/* nl */ {IDCLS_SET_VDC_MEMORY_16KB_NL, "Zet de VDC geheugen grootte als 16KB"},
/* pl */ {IDCLS_SET_VDC_MEMORY_16KB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VDC_MEMORY_16KB_SV, "Sätt VDC-minnesstorlek till 16KB"},

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_MEMORY_64KB,    "Set the VDC memory size to 64KB"},
/* de */ {IDCLS_SET_VDC_MEMORY_64KB_DE, "VDC Speichgröße auf 64KB setzen"},
/* fr */ {IDCLS_SET_VDC_MEMORY_64KB_FR, "Régler la taille de la mémoire VDC à 64KO"},
/* it */ {IDCLS_SET_VDC_MEMORY_64KB_IT, "Imposta la dimensione della memoria del VDC a 64KB"},
/* nl */ {IDCLS_SET_VDC_MEMORY_64KB_NL, "Zet de VDC geheugen grootte als 64KB"},
/* pl */ {IDCLS_SET_VDC_MEMORY_64KB_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VDC_MEMORY_64KB_SV, "Sätt VDC-minnesstorlek till 64KB"},

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_REVISION,    "Set VDC revision (0..2)"},
/* de */ {IDCLS_SET_VDC_REVISION_DE, "VDC Revision (0..2) setzen"},
/* fr */ {IDCLS_SET_VDC_REVISION_FR, "Régler la révision VDC (0..2)"},
/* it */ {IDCLS_SET_VDC_REVISION_IT, "Imposta la revisione del VDC (0..2)"},
/* nl */ {IDCLS_SET_VDC_REVISION_NL, "Zet de VDC revisie (0..2)"},
/* pl */ {IDCLS_SET_VDC_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VDC_REVISION_SV, "Ange VDC-revision (0..2)"},

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_P_SPEC,    "<spec>"},
/* de */ {IDCLS_P_SPEC_DE, "<Spec>"},
/* fr */ {IDCLS_P_SPEC_FR, "<spec>"},
/* it */ {IDCLS_P_SPEC_IT, "<spec>"},
/* nl */ {IDCLS_P_SPEC_NL, "<spec>"},
/* pl */ {IDCLS_P_SPEC_PL, "<spec>"},
/* sv */ {IDCLS_P_SPEC_SV, "<spec>"},

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_MEMORY_CONFIG,    "Specify memory configuration"},
/* de */ {IDCLS_SPECIFY_MEMORY_CONFIG_DE, "Speicher Konfiguration definieren"},
/* fr */ {IDCLS_SPECIFY_MEMORY_CONFIG_FR, "Spécifier la configuration de la mémoire"},
/* it */ {IDCLS_SPECIFY_MEMORY_CONFIG_IT, "Specifica la configurazione della memoria"},
/* nl */ {IDCLS_SPECIFY_MEMORY_CONFIG_NL, "Geef geheugen konfiguratie"},
/* pl */ {IDCLS_SPECIFY_MEMORY_CONFIG_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_MEMORY_CONFIG_SV, "Ange minneskonfiguration"},

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_ENABLE_VIC1112_IEEE488,    "Enable VIC-1112 IEEE488 interface"},
/* de */ {IDCLS_ENABLE_VIC1112_IEEE488_DE, "VIC-1112 IEEE488 Schnittstelle aktivieren"},
/* fr */ {IDCLS_ENABLE_VIC1112_IEEE488_FR, "Activer l'interface VIC-1112 IEEE488"},
/* it */ {IDCLS_ENABLE_VIC1112_IEEE488_IT, "Attiva l'interfaccia IEEE488 del VIC-1112"},
/* nl */ {IDCLS_ENABLE_VIC1112_IEEE488_NL, "Aktiveer VIC-1112 IEEE488 interface"},
/* pl */ {IDCLS_ENABLE_VIC1112_IEEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_VIC1112_IEEE488_SV, "Aktivera VIC-1112-IEEE488-gränssnitt"},

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_DISABLE_VIC1112_IEEE488,    "Disable VIC-1112 IEEE488 interface"},
/* de */ {IDCLS_DISABLE_VIC1112_IEEE488_DE, "VIC-1112 IEEE488 Schnittstelle deaktivieren"},
/* fr */ {IDCLS_DISABLE_VIC1112_IEEE488_FR, "Désactiver l'interface VIC-1112 IEEE488"},
/* it */ {IDCLS_DISABLE_VIC1112_IEEE488_IT, "Disattiva l'interfaccia IEEE488 del VIC-1112"},
/* nl */ {IDCLS_DISABLE_VIC1112_IEEE488_NL, "VIC-1112 IEEE488 interface afsluiten"},
/* pl */ {IDCLS_DISABLE_VIC1112_IEEE488_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_VIC1112_IEEE488_SV, "Inaktivera VIC-1112-IEEE488-gränssnitt"},

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME,    "Specify 4/8/16K extension ROM name at $2000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_DE, "4/8/16K Erweiterungs ROM Datei Name für $2000 definieren"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_FR, "Spécifier le nom de l'extension ROM 4/8/16K à $2000"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_IT, "Specifica il nome della ROM di estensione di 4/8/16K a $2000"},
/* nl */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_NL, "Geef de naam van het bestand voor de 4/8/16K ROM op $2000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_SV, "Ange namn för 4/8/16K-utöknings-ROM på $2000"},

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME,    "Specify 4/8/16K extension ROM name at $4000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_DE, "4/8/16K Erweiterungs ROM Datei Name für $4000 definieren"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_FR, "Spécifier le nom de l'extension ROM 4/8/16K à $4000"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_IT, "Specifica il nome della ROM di estensione di 4/8/16K a $4000"},
/* nl */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_NL, "Geef de naam van het bestand voor de 4/8/16K ROM op $4000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_SV, "Ange namn för 4/8/16K-utöknings-ROM på $4000"},

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME,    "Specify 4/8/16K extension ROM name at $6000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_DE, "4/8/16K Erweiterungs ROM Datei Name für $6000 definieren"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_FR, "Spécifier le nom de l'extension ROM 4/8/16K à $6000"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_IT, "Specifica il nome della ROM di estensione di 4/8/16K a $6000"},
/* nl */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_NL, "Geef de naam van het bestand voor de 4/8/16K ROM op $6000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_SV, "Ange namn för 4/8/16K-utöknings-ROM på $6000"},

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME,    "Specify 4/8K extension ROM name at $A000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_DE, "4/8K Erweiterungs ROM Datei Name für $A000 definieren"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_FR, "Spécifier le nom de l'extension ROM 4/8K à $A000"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_IT, "Specifica il nome della ROM di estensione di 4/8K a $A000"},
/* nl */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_NL, "Geef de naam van het bestand voor de 4/8K ROM op $A000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_SV, "Ange namn för 4/8K-utöknings-ROM på $A000"},

/* vic20/vic20cartridge.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME,    "Specify 4K extension ROM name at $B000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_DE, "4K Erweiterungs ROM Datei Name für $B000 definieren"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_FR, "Spécifier le nom de l'extension ROM 4K à $B000"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_IT, "Specifica il nome della ROM di estensione di 4K a $B000"},
/* nl */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_NL, "Geef de naam van het bestand voor de 4K ROM op $B000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_SV, "Ange namn för 4K-utöknings-ROM på $B000"},

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SPRITE_BACKGROUND,    "Enable sprite-background collision registers"},
/* de */ {IDCLS_ENABLE_SPRITE_BACKGROUND_DE, "Sprite-Hintergrund Kollisionen aktivieren"},
/* fr */ {IDCLS_ENABLE_SPRITE_BACKGROUND_FR, "Activer les registres de collisions Sprite-Arrière-plan"},
/* it */ {IDCLS_ENABLE_SPRITE_BACKGROUND_IT, "Attiva i registri di collisione sprite-sfondo"},
/* nl */ {IDCLS_ENABLE_SPRITE_BACKGROUND_NL, "Aktiveer sprite-achtergrond botsing registers"},
/* pl */ {IDCLS_ENABLE_SPRITE_BACKGROUND_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SPRITE_BACKGROUND_SV, "Aktivera sprite-till-bakgrund-kollisionsregister"},

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SPRITE_BACKGROUND,    "Disable sprite-background collision registers"},
/* de */ {IDCLS_DISABLE_SPRITE_BACKGROUND_DE, "Sprite-Hintergrund Kollisionen deaktivieren"},
/* fr */ {IDCLS_DISABLE_SPRITE_BACKGROUND_FR, "Désactiver les registres de collisions Sprite-Arrière-plan"},
/* it */ {IDCLS_DISABLE_SPRITE_BACKGROUND_IT, "Disattiva i registri di collisione sprite-sfondo"},
/* nl */ {IDCLS_DISABLE_SPRITE_BACKGROUND_NL, "Sprite-achtergrond botsing registers afsluiten"},
/* pl */ {IDCLS_DISABLE_SPRITE_BACKGROUND_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SPRITE_BACKGROUND_SV, "Inaktivera sprite-till-bakgrund-kollisionsregister"},

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SPRITE_SPRITE,    "Enable sprite-sprite collision registers"},
/* de */ {IDCLS_ENABLE_SPRITE_SPRITE_DE, "Sprite-Sprite Kollisionen aktivieren"},
/* fr */ {IDCLS_ENABLE_SPRITE_SPRITE_FR, "Activer les registres de collisions Sprites-Sprites"},
/* it */ {IDCLS_ENABLE_SPRITE_SPRITE_IT, "Attiva i registri di collisione sprite-sprite"},
/* nl */ {IDCLS_ENABLE_SPRITE_SPRITE_NL, "Aktiveer sprite-sprite botsing registers"},
/* pl */ {IDCLS_ENABLE_SPRITE_SPRITE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SPRITE_SPRITE_SV, "Aktivera sprite-till-sprite-kollisionsregister"},

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SPRITE_SPRITE,    "Disable sprite-sprite collision registers"},
/* de */ {IDCLS_DISABLE_SPRITE_SPRITE_DE, "Sprite-Sprite Kollisionen deaktivieren"},
/* fr */ {IDCLS_DISABLE_SPRITE_SPRITE_FR, "Désactiver les registres de collisions Sprites-Sprites"},
/* it */ {IDCLS_DISABLE_SPRITE_SPRITE_IT, "Disattiva i registri di collisione sprite-sprite"},
/* nl */ {IDCLS_DISABLE_SPRITE_SPRITE_NL, "Sprite-sprite botsing registers afsluiten"},
/* pl */ {IDCLS_DISABLE_SPRITE_SPRITE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SPRITE_SPRITE_SV, "Inaktivera sprite-till-sprite-kollisionsregister"},

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_USE_NEW_LUMINANCES,    "Use new luminances"},
/* de */ {IDCLS_USE_NEW_LUMINANCES_DE, "Neue Helligkeitsemulation"},
/* fr */ {IDCLS_USE_NEW_LUMINANCES_FR, "Utiliser les nouvelles luminescences"},
/* it */ {IDCLS_USE_NEW_LUMINANCES_IT, "Usa nuove luminanze"},
/* nl */ {IDCLS_USE_NEW_LUMINANCES_NL, "Gebruik nieuwe kleuren"},
/* pl */ {IDCLS_USE_NEW_LUMINANCES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_NEW_LUMINANCES_SV, "Använd nya ljusstyrkor"},

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_USE_OLD_LUMINANCES,    "Use old luminances"},
/* de */ {IDCLS_USE_OLD_LUMINANCES_DE, "Alte Helligkeitsemulation"},
/* fr */ {IDCLS_USE_OLD_LUMINANCES_FR, "Utiliser les anciennes luminescences"},
/* it */ {IDCLS_USE_OLD_LUMINANCES_IT, "Usa vecchie luminanze"},
/* nl */ {IDCLS_USE_OLD_LUMINANCES_NL, "Gebruik oude kleuren"},
/* pl */ {IDCLS_USE_OLD_LUMINANCES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_OLD_LUMINANCES_SV, "Använd gamla ljusstyrkor"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SIZE,    "Enable double size"},
/* de */ {IDCLS_ENABLE_DOUBLE_SIZE_DE, "Doppelte Größe aktivieren"},
/* fr */ {IDCLS_ENABLE_DOUBLE_SIZE_FR, "Taille double"},
/* it */ {IDCLS_ENABLE_DOUBLE_SIZE_IT, "Attiva la dimensione doppia"},
/* nl */ {IDCLS_ENABLE_DOUBLE_SIZE_NL, "Aktiveer dubbele grootte"},
/* pl */ {IDCLS_ENABLE_DOUBLE_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DOUBLE_SIZE_SV, "Aktivera dubbel storlek"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SIZE,    "Disable double size"},
/* de */ {IDCLS_DISABLE_DOUBLE_SIZE_DE, "Doppelte Größe deaktivieren"},
/* fr */ {IDCLS_DISABLE_DOUBLE_SIZE_FR, "Taille normale"},
/* it */ {IDCLS_DISABLE_DOUBLE_SIZE_IT, "Disattiva la dimensione doppia"},
/* nl */ {IDCLS_DISABLE_DOUBLE_SIZE_NL, "Dubbele grootte afsluiten"},
/* pl */ {IDCLS_DISABLE_DOUBLE_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DOUBLE_SIZE_SV, "Inaktivera dubbel storlek"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SCAN,    "Enable double scan"},
/* de */ {IDCLS_ENABLE_DOUBLE_SCAN_DE, "Doppelt Scan aktivieren"},
/* fr */ {IDCLS_ENABLE_DOUBLE_SCAN_FR, "Activer le mode double scan"},
/* it */ {IDCLS_ENABLE_DOUBLE_SCAN_IT, "Attiva la scansione doppia"},
/* nl */ {IDCLS_ENABLE_DOUBLE_SCAN_NL, "Aktiveer dubbele scan"},
/* pl */ {IDCLS_ENABLE_DOUBLE_SCAN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DOUBLE_SCAN_SV, "Aktivera dubbelskanning"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SCAN,    "Disable double scan"},
/* de */ {IDCLS_DISABLE_DOUBLE_SCAN_DE, "Doppelt Scan deaktivieren"},
/* fr */ {IDCLS_DISABLE_DOUBLE_SCAN_FR, "Désactiver le mode double scan"},
/* it */ {IDCLS_DISABLE_DOUBLE_SCAN_IT, "Disattiva la scansione doppia"},
/* nl */ {IDCLS_DISABLE_DOUBLE_SCAN_NL, "Dubbele scan afsluiten"},
/* pl */ {IDCLS_DISABLE_DOUBLE_SCAN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DOUBLE_SCAN_SV, "Inaktivera dubbelskanning"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_HARDWARE_SCALING,    "Enable hardware scaling"},
/* de */ {IDCLS_ENABLE_HARDWARE_SCALING_DE, "Hardwareunterstützung für Skalierung aktivieren"},
/* fr */ {IDCLS_ENABLE_HARDWARE_SCALING_FR, "Activer le \"scaling\" matériel"},
/* it */ {IDCLS_ENABLE_HARDWARE_SCALING_IT, "Attiva l'hardware scaling"},
/* nl */ {IDCLS_ENABLE_HARDWARE_SCALING_NL, "Aktiveer hardware schalering"},
/* pl */ {IDCLS_ENABLE_HARDWARE_SCALING_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_HARDWARE_SCALING_SV, "Aktivera maskinvaruskalning"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_HARDWARE_SCALING,    "Disable hardware scaling"},
/* de */ {IDCLS_DISABLE_HARDWARE_SCALING_DE, "Hardwareunterstützung für Skalierung deaktivieren"},
/* fr */ {IDCLS_DISABLE_HARDWARE_SCALING_FR, "Désactiver le \"scaling\" matériel"},
/* it */ {IDCLS_DISABLE_HARDWARE_SCALING_IT, "Disattiva l'hardware scaling"},
/* nl */ {IDCLS_DISABLE_HARDWARE_SCALING_NL, "Hardware schalering afsluiten"},
/* pl */ {IDCLS_DISABLE_HARDWARE_SCALING_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_HARDWARE_SCALING_SV, "Inaktivera maskinvaruskalning"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SCALE2X,    "Enable Scale2x"},
/* de */ {IDCLS_ENABLE_SCALE2X_DE, "Scale2x aktivieren"},
/* fr */ {IDCLS_ENABLE_SCALE2X_FR, "Activer Scale2x"},
/* it */ {IDCLS_ENABLE_SCALE2X_IT, "Attiva Scale2x"},
/* nl */ {IDCLS_ENABLE_SCALE2X_NL, "Aktiveer Scale2x"},
/* pl */ {IDCLS_ENABLE_SCALE2X_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SCALE2X_SV, "Aktivera Scale2x"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SCALE2X,    "Disable Scale2x"},
/* de */ {IDCLS_DISABLE_SCALE2X_DE, "Scale2x deaktivieren"},
/* fr */ {IDCLS_DISABLE_SCALE2X_FR, "Désactiver Scale2x"},
/* it */ {IDCLS_DISABLE_SCALE2X_IT, "Disattiva Scale2x"},
/* nl */ {IDCLS_DISABLE_SCALE2X_NL, "Scale2x afsluiten"},
/* pl */ {IDCLS_DISABLE_SCALE2X_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SCALE2X_SV, "Inaktivera Scale2x"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_USE_INTERNAL_CALC_PALETTE,    "Use an internal calculated palette"},
/* de */ {IDCLS_USE_INTERNAL_CALC_PALETTE_DE, "Benutzse intern berechnete Palette"},
/* fr */ {IDCLS_USE_INTERNAL_CALC_PALETTE_FR, "Utiliser une palette interne calculée"},
/* it */ {IDCLS_USE_INTERNAL_CALC_PALETTE_IT, "Usa una palette interna calcolata"},
/* nl */ {IDCLS_USE_INTERNAL_CALC_PALETTE_NL, "Gebruik een intern berekend kleuren palette"},
/* pl */ {IDCLS_USE_INTERNAL_CALC_PALETTE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_INTERNAL_CALC_PALETTE_SV, "Använd en internt beräknad palett"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_USE_EXTERNAL_FILE_PALETTE,    "Use an external palette (file)"},
/* de */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_DE, "Benutze externe Palette (Datei)"},
/* fr */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_FR, "Utiliser une palette externe (fichier)"},
/* it */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_IT, "Usa una palette esterna (file)"},
/* nl */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_NL, "Gebruik een extern kleuren palette (bestand)"},
/* pl */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_USE_EXTERNAL_FILE_PALETTE_SV, "Använd en extern palett (fil)"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME,    "Specify name of file of external palette"},
/* de */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_DE, "Dateiname für externe Palette definieren"},
/* fr */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_FR, "Spécifier le nom du fichier de la palette externe"},
/* it */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_IT, "Specifica il nome del file della palette esterna"},
/* nl */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_NL, "Geef de naam van het extern kleuren palette bestand"},
/* pl */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_SV, "Ange namn på fil för extern palett"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FULLSCREEN_MODE,    "Enable fullscreen mode"},
/* de */ {IDCLS_ENABLE_FULLSCREEN_MODE_DE, "Vollbild Modus aktivieren"},
/* fr */ {IDCLS_ENABLE_FULLSCREEN_MODE_FR, "Activer le mode plein écran"},
/* it */ {IDCLS_ENABLE_FULLSCREEN_MODE_IT, "Visualizza a tutto schermo"},
/* nl */ {IDCLS_ENABLE_FULLSCREEN_MODE_NL, "Aktiveer volscherm modus"},
/* pl */ {IDCLS_ENABLE_FULLSCREEN_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FULLSCREEN_MODE_SV, "Aktivera fullskärmsläge"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FULLSCREEN_MODE,    "Disable fullscreen mode"},
/* de */ {IDCLS_DISABLE_FULLSCREEN_MODE_DE, "Vollbild Modus deaktivieren"},
/* fr */ {IDCLS_DISABLE_FULLSCREEN_MODE_FR, "Désactiver le mode plein écran"},
/* it */ {IDCLS_DISABLE_FULLSCREEN_MODE_IT, "Non visualizzare a tutto schermo"},
/* nl */ {IDCLS_DISABLE_FULLSCREEN_MODE_NL, "Volscherm modus afsluiten"},
/* pl */ {IDCLS_DISABLE_FULLSCREEN_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FULLSCREEN_MODE_SV, "Inaktivera fullskärmsläge"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_P_DEVICE,    "<device>"},
/* de */ {IDCLS_P_DEVICE_DE, "<Gerät>"},
/* fr */ {IDCLS_P_DEVICE_FR, "<périphérique>"},
/* it */ {IDCLS_P_DEVICE_IT, "<dispositivo>"},
/* nl */ {IDCLS_P_DEVICE_NL, "<apparaat>"},
/* pl */ {IDCLS_P_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_DEVICE_SV, "<enhet>"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_SELECT_FULLSCREEN_DEVICE,    "Select fullscreen device"},
/* de */ {IDCLS_SELECT_FULLSCREEN_DEVICE_DE, "Vollbild Gerät selektieren"},
/* fr */ {IDCLS_SELECT_FULLSCREEN_DEVICE_FR, "Sélectionner le périphérique plein écran"},
/* it */ {IDCLS_SELECT_FULLSCREEN_DEVICE_IT, "Seleziona il dispositivo per la visualizzazione a tutto schermo"},
/* nl */ {IDCLS_SELECT_FULLSCREEN_DEVICE_NL, "Selecteer volscherm apparaat"},
/* pl */ {IDCLS_SELECT_FULLSCREEN_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SELECT_FULLSCREEN_DEVICE_SV, "Ange fullskärmsenhet"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN,    "Enable double size in fullscreen mode"},
/* de */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_DE, "Doppelte Größe im Vollbild Modus aktivieren"},
/* fr */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_FR, "Activer \"Taille double\" en plein écran"},
/* it */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_IT, "Attiva la dimensione doppia nella visualizzazione a tutto schermo"},
/* nl */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_NL, "Aktiveer dubbele grootte in volscherm modus"},
/* pl */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_SV, "Aktivera dubbel storlek i fullskärmsläge"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN,    "Disable double size in fullscreen mode"},
/* de */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_DE, "Doppelte Große im Vollbild Modus deaktivieren"},
/* fr */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_FR, "Désactiver \"Taille double\" en plein écran"},
/* it */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_IT, "Disattiva la dimensione doppia nella visualizzazione a tutto schermo"},
/* nl */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_NL, "Dubbele grootte in volscherm modus afsluiten"},
/* pl */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_SV, "Inaktivera dubbel storlek i fullskärmsläge"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN,    "Enable double scan in fullscreen mode"},
/* de */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_DE, "Doppelt Scan im Vollbild Modus aktivieren"},
/* fr */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_FR, "Activer \"Double scan\" en plein écran"},
/* it */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_IT, "Attiva la scansione doppia nella visualizzazione a tutto schermo"},
/* nl */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_NL, "Aktiveer dubbele scan in volscherm modus"},
/* pl */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_SV, "Aktivera dubbelskanning i fullskärmsläge"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN,    "Disable double scan in fullscreen mode"},
/* de */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_DE, "Doppelt Scan im Vollbild Modus deaktivieren"},
/* fr */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_FR, "Désactiver \"Double scan\" en plein écran"},
/* it */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_IT, "Disattiva la scansione doppia nella visualizzazione a tutto schermo"},
/* nl */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_NL, "Dubbele scan in volscherm modus afsluiten"},
/* pl */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_SV, "Inaktivera dubbelskanning i fullskärmsläge"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_P_MODE,    "<mode>"},
/* de */ {IDCLS_P_MODE_DE, "<Modus>"},
/* fr */ {IDCLS_P_MODE_FR, "<mode>"},
/* it */ {IDCLS_P_MODE_IT, "<modalità>"},
/* nl */ {IDCLS_P_MODE_NL, "<modus>"},
/* pl */ {IDCLS_P_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_MODE_SV, "<läge>"},

/* video/video-cmdline-options.c */
/* en */ {IDCLS_SELECT_FULLSCREEN_MODE,    "Select fullscreen mode"},
/* de */ {IDCLS_SELECT_FULLSCREEN_MODE_DE, "Vollbild Modus wählen"},
/* fr */ {IDCLS_SELECT_FULLSCREEN_MODE_FR, "Sélectionner le mode plein écran"},
/* it */ {IDCLS_SELECT_FULLSCREEN_MODE_IT, "Seleziona la modalità di visualizzazione a tutto schermo"},
/* nl */ {IDCLS_SELECT_FULLSCREEN_MODE_NL, "Selecteer volscherm modus"},
/* pl */ {IDCLS_SELECT_FULLSCREEN_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SELECT_FULLSCREEN_MODE_SV, "Aktivera fullskärmsläge"},

/* aciacore.c */
/* en */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE,    "Specify RS232 device this ACIA should work on"},
/* de */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_DE, "RS232 Gerät für welches ACIA funktionieren soll ist zu spezifizieren"},
/* fr */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_FR, "Spécifier le périphérique RS232 sur lequel doit fonctionner l'ACIA"},
/* it */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_IT, "Specifica il dispositivo RS232 con il quale questa ACIA dovrebbe funzionare"},
/* nl */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_NL, "Geef het RS232 apparaat waarmee deze ACIA moet werken"},
/* pl */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_ACIA_RS232_DEVICE_SV, "Ange RS232-enhet denna ACIA skall arbeta på"},

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_8,    "Set device type for device #8 (0: NONE, 1: FS, 2: REAL, 3: RAW)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_8_DE, "Geräte Typ für Gerät #8 (0: NONE, 1: FS, 2: REAL, 3: RAW)"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_8_FR, "Régler le type de périphérique pour #8 (0: AUCUN, 1: FS, 2: RÉEL, 3: RAW)"},
/* it */ {IDCLS_SET_DEVICE_TYPE_8_IT, "Imposta il tipo di periferica #8 (0 NESSUNA, 1 FS, 2: REALE, 3 RAW)"},
/* nl */ {IDCLS_SET_DEVICE_TYPE_8_NL, "Zet het apparaat soort voor apparaat #8 (0: GEEN, 1: FS, 2: ECHT, 3: BINAIR"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_8_SV, "Ställ in enhetstyp för enhet 8 (0: INGEN, 1: FS, 2: ÄKTA, 3: RÅ)"},

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_9,    "Set device type for device #9 (0: NONE, 1: FS, 2: REAL, 3: RAW)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_9_DE, "Geräte Typ für Gerät #9 (0: NONE, 1: FS, 2: REAL, 3: RAW)"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_9_FR, "Régler le type de périphérique pour #9 (0: AUCUN, 1: FS, 2: RÉEL, 3: RAW)"},
/* it */ {IDCLS_SET_DEVICE_TYPE_9_IT, "Imposta il tipo di periferica #9 (0 NESSUNA, 1 FS, 2: REALE, 3 RAW)"},
/* nl */ {IDCLS_SET_DEVICE_TYPE_9_NL, "Zet het apparaat soort voor apparaat #9 (0: GEEN, 1: FS, 2: ECHT, 3: BINAIR"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_9_SV, "Ställ in enhetstyp för enhet 9 (0: INGEN, 1: FS, 2: ÄKTA, 3: RÅ)"},

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_10,    "Set device type for device #10 (0: NONE, 1: FS, 2: REAL, 3: RAW)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_10_DE, "Geräte Typ für Gerät #10 (0: NONE, 1: FS, 2: REAL, 3: RAW)"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_10_FR, "Régler le type de périphérique pour #10 (0: AUCUN, 1: FS, 2: RÉEL, 3: RAW)"},
/* it */ {IDCLS_SET_DEVICE_TYPE_10_IT, "Imposta il tipo di periferica #10 (0 NESSUNA, 1 FS, 2: REALE, 3 RAW)"},
/* nl */ {IDCLS_SET_DEVICE_TYPE_10_NL, "Zet het apparaat soort voor apparaat #10 (0: GEEN, 1: FS, 2: ECHT, 3: BINAIR"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_10_SV, "Ställ in enhetstyp för enhet 10 (0: INGEN, 1: FS, 2: ÄKTA, 3: RÅ)"},

/* attach.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_11,    "Set device type for device #11 (0: NONE, 1: FS, 2: REAL, 3: RAW)"},
/* de */ {IDCLS_SET_DEVICE_TYPE_11_DE, "Geräte Typ für Gerät #11 (0: NONE, 1: FS, 2: REAL, 3: RAW)"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_11_FR, "Régler le type de périphérique pour #11 (0: AUCUN, 1: FS, 2: RÉEL, 3: RAW)"},
/* it */ {IDCLS_SET_DEVICE_TYPE_11_IT, "Imposta il tipo di periferica #11 (0 NESSUNA, 1 FS, 2: REALE, 3 RAW)"},
/* nl */ {IDCLS_SET_DEVICE_TYPE_11_NL, "Zet het apparaat soort voor apparaat #11 (0: GEEN, 1: FS, 2: ECHT, 3: BINAIR"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_11_SV, "Ställ in enhetstyp för enhet 11 (0: INGEN, 1: FS, 2: ÄKTA, 3: RÅ)"},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_8,    "Attach disk image for drive #8 read only"},
/* de */ {IDCLS_ATTACH_READ_ONLY_8_DE, "Disk Image als Laufwerk #8 benutzen (schreibgeschützt)"},
/* fr */ {IDCLS_ATTACH_READ_ONLY_8_FR, "Insérer une image de disque dans le lecteur #8 en lecture seule"},
/* it */ {IDCLS_ATTACH_READ_ONLY_8_IT, "Seleziona l'immagine del disco per il drive #8 in sola lettura"},
/* nl */ {IDCLS_ATTACH_READ_ONLY_8_NL, "Koppel disk bestand voor drive #8 als alleen lezen"},
/* pl */ {IDCLS_ATTACH_READ_ONLY_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_ONLY_8_SV, "Anslut diskettavbildningsfil för enhet 8 med skrivskydd"},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_8,    "Attach disk image for drive #8 read write (if possible)"},
/* de */ {IDCLS_ATTACH_READ_WRITE_8_DE, "Disk Image als Laufwerk #8 benutzen (Schreibzugriff, wenn möglich)"},
/* fr */ {IDCLS_ATTACH_READ_WRITE_8_FR, "Insérer une image de disque dans le lecteur #8 en lecture/écriture (si possible)"},
/* it */ {IDCLS_ATTACH_READ_WRITE_8_IT, "Seleziona l'immagine del disco per il drive #8 in lettura/scrittura (se possibile)"},
/* nl */ {IDCLS_ATTACH_READ_WRITE_8_NL, "Koppel disk bestand voor drive #8 als schrijfbaar (indien mogelijk)"},
/* pl */ {IDCLS_ATTACH_READ_WRITE_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_WRITE_8_SV, "Anslut diskettavbildningsfil för enhet 8 skrivbar (om möjligt)"},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_9,    "Attach disk image for drive #9 read only"},
/* de */ {IDCLS_ATTACH_READ_ONLY_9_DE, "Disk Image als Laufwerk #9 benutzen (schreibgeschützt)"},
/* fr */ {IDCLS_ATTACH_READ_ONLY_9_FR, "Insérer une image de disque dans le lecteur #9 en lecture seule"},
/* it */ {IDCLS_ATTACH_READ_ONLY_9_IT, "Seleziona l'immagine del disco per il drive #9 in sola lettura"},
/* nl */ {IDCLS_ATTACH_READ_ONLY_9_NL, "Koppel disk bestand voor drive #9 als alleen lezen"},
/* pl */ {IDCLS_ATTACH_READ_ONLY_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_ONLY_9_SV, "Anslut diskettavbildningsfil för enhet 9 med skrivskydd"},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_9,    "Attach disk image for drive #9 read write (if possible)"},
/* de */ {IDCLS_ATTACH_READ_WRITE_9_DE, "Disk Image als Laufwerk #9 benutzen (Schreibzugriff, wenn möglich)"},
/* fr */ {IDCLS_ATTACH_READ_WRITE_9_FR, "Insérer une image de disque dans le lecteur #9 en lecture/écriture (si possible)"},
/* it */ {IDCLS_ATTACH_READ_WRITE_9_IT, "Seleziona l'immagine del disco per il drive #9 in lettura/scrittura (se possibile)"},
/* nl */ {IDCLS_ATTACH_READ_WRITE_9_NL, "Koppel disk bestand voor drive #9 als schrijfbaar (indien mogelijk)"},
/* pl */ {IDCLS_ATTACH_READ_WRITE_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_WRITE_9_SV, "Anslut diskettavbildningsfil för enhet 9 skrivbar (om möjligt)"},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_10,    "Attach disk image for drive #10 read only"},
/* de */ {IDCLS_ATTACH_READ_ONLY_10_DE, "Disk Image als Laufwerk #10 benutzen (schreibgeschützt)"},
/* fr */ {IDCLS_ATTACH_READ_ONLY_10_FR, "Insérer une image de disque dans le lecteur #10 en lecture seule"},
/* it */ {IDCLS_ATTACH_READ_ONLY_10_IT, "Seleziona l'immagine del disco per il drive #10 in sola lettura"},
/* nl */ {IDCLS_ATTACH_READ_ONLY_10_NL, "Koppel disk bestand voor drive #10 als alleen lezen"},
/* pl */ {IDCLS_ATTACH_READ_ONLY_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_ONLY_10_SV, "Anslut diskettavbildningsfil för enhet 10 med skrivskydd"},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_10,    "Attach disk image for drive #10 read write (if possible)"},
/* de */ {IDCLS_ATTACH_READ_WRITE_10_DE, "Disk Image als Laufwerk #10 benutzen (Schreibzugriff, wenn möglich)"},
/* fr */ {IDCLS_ATTACH_READ_WRITE_10_FR, "Insérer une image de disque dans le lecteur #10 en lecture/écriture (si possible)"},
/* it */ {IDCLS_ATTACH_READ_WRITE_10_IT, "Seleziona l'immagine del disco per il drive #10 in lettura/scrittura (se possibile)"},
/* nl */ {IDCLS_ATTACH_READ_WRITE_10_NL, "Koppel disk bestand voor drive #10 als schrijfbaar (indien mogelijk)"},
/* pl */ {IDCLS_ATTACH_READ_WRITE_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_WRITE_10_SV, "Anslut diskettavbildningsfil för enhet 10 skrivbar (om möjligt)"},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_ONLY_11,    "Attach disk image for drive #11 read only"},
/* de */ {IDCLS_ATTACH_READ_ONLY_11_DE, "Disk Image als Laufwerk #11 benutzen (schreibgeschützt)"},
/* fr */ {IDCLS_ATTACH_READ_ONLY_11_FR, "Insérer une image de disque dans le lecteur #11 en lecture seule"},
/* it */ {IDCLS_ATTACH_READ_ONLY_11_IT, "Seleziona l'immagine del disco per il drive #11 in sola lettura"},
/* nl */ {IDCLS_ATTACH_READ_ONLY_11_NL, "Koppel disk bestand voor drive #11 als alleen lezen"},
/* pl */ {IDCLS_ATTACH_READ_ONLY_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_ONLY_11_SV, "Anslut diskettavbildningsfil för enhet 11 med skrivskydd"},

/* attach.c */
/* en */ {IDCLS_ATTACH_READ_WRITE_11,    "Attach disk image for drive #11 read write (if possible)"},
/* de */ {IDCLS_ATTACH_READ_WRITE_11_DE, "Disk Image als Laufwerk #11 benutzen (Schreibzugriff, wenn möglich)"},
/* fr */ {IDCLS_ATTACH_READ_WRITE_11_FR, "Insérer une image de disque dans le lecteur #11 en lecture/écriture (si possible)"},
/* it */ {IDCLS_ATTACH_READ_WRITE_11_IT, "Seleziona l'immagine del disco per il drive #11 in lettura/scrittura (se possibile)"},
/* nl */ {IDCLS_ATTACH_READ_WRITE_11_NL, "Koppel disk bestand voor drive #11 als schrijfbaar (indien mogelijk)"},
/* pl */ {IDCLS_ATTACH_READ_WRITE_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_READ_WRITE_11_SV, "Anslut diskettavbildningsfil för enhet 11 skrivbar (om möjligt)"},

/* datasette.c */
/* en */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET,    "Enable automatic Datasette-Reset"},
/* de */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_DE, "Automatisches Datasette-Reset aktivieren"},
/* fr */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_FR, "Activer le redémarrage automatique du Datasette"},
/* it */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_IT, "Attiva il reset del registratore automatico"},
/* nl */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_NL, "Aktiveer automatische Datasette-Reset"},
/* pl */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_AUTO_DATASETTE_RESET_SV, "Aktivera automatisk Datasetteåterställning"},

/* datasette.c */
/* en */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET,    "Disable automatic Datasette-Reset"},
/* de */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_DE, "Automatisches Datasette-Reset deaktivieren"},
/* fr */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_FR, "Désactiver le redémarrage automatique du Datasette"},
/* it */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_IT, "Disattiva il reset del registratore automatico"},
/* nl */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_NL, "Automatische Datasette-Reset afsluiten"},
/* pl */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_AUTO_DATASETTE_RESET_SV, "Inaktivera automatisk Datasetteåterställning"},

/* datasette.c */
/* en */ {IDCLS_SET_ZERO_TAP_DELAY,    "Set delay in cycles for a zero in the tap"},
/* de */ {IDCLS_SET_ZERO_TAP_DELAY_DE, "Verzögerung für Zero in Zyklen im Tap"},
/* fr */ {IDCLS_SET_ZERO_TAP_DELAY_FR, "Spécifier le délai en cycles pour un zéro sur le ruban"},
/* it */ {IDCLS_SET_ZERO_TAP_DELAY_IT, "Imposta il ritardo in cicli per uno zero nel tap"},
/* nl */ {IDCLS_SET_ZERO_TAP_DELAY_NL, "Zet de vertraging in cylcli voor een nul in de tap"},
/* pl */ {IDCLS_SET_ZERO_TAP_DELAY_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_ZERO_TAP_DELAY_SV, "Ange väntecykler för nolla i tap-filen"},

/* datasette.c */
/* en */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP,    "Set number of cycles added to each gap in the tap"},
/* de */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_DE, "Setze Anzahl der Zyklen für jedes Loch im Tap"},
/* fr */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_FR, "Spécifier le nombre de cycles ajouté a chaque raccord du ruban"},
/* it */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_IT, "Imposta il numero di cicli aggiunti ad ogni gap nel tap"},
/* nl */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_NL, "Zet aantal extra cylcli voor elk gat in de tap"},
/* pl */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_CYCLES_ADDED_GAP_TAP_SV, "Ange cykler som läggs till varje gap i tap-filen"},

#ifdef DEBUG
/* debug.c */
/* en */ {IDCLS_TRACE_MAIN_CPU,    "Trace the main CPU"},
/* de */ {IDCLS_TRACE_MAIN_CPU_DE, "Haupt CPU verfolgen"},
/* fr */ {IDCLS_TRACE_MAIN_CPU_FR, "Tracer le CPU principal"},
/* it */ {IDCLS_TRACE_MAIN_CPU_IT, "Traccia la CPU principale"},
/* nl */ {IDCLS_TRACE_MAIN_CPU_NL, "Traceer de hoofd CPU"},
/* pl */ {IDCLS_TRACE_MAIN_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_MAIN_CPU_SV, "Spåra huvudprocessorn"},

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_MAIN_CPU,    "Do not trace the main CPU"},
/* de */ {IDCLS_DONT_TRACE_MAIN_CPU_DE, "Haupt CPU nicht verfolgen"},
/* fr */ {IDCLS_DONT_TRACE_MAIN_CPU_FR, "Ne pas tracer le CPU principal"},
/* it */ {IDCLS_DONT_TRACE_MAIN_CPU_IT, "Non tracciare la CPU principale"},
/* nl */ {IDCLS_DONT_TRACE_MAIN_CPU_NL, "Traceer de hoofd CPU niet"},
/* pl */ {IDCLS_DONT_TRACE_MAIN_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_MAIN_CPU_SV, "Spåra inte huvudprocessorn"},

/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE0_CPU,    "Trace the drive0 CPU"},
/* de */ {IDCLS_TRACE_DRIVE0_CPU_DE, "Laufwerk0 CPU verfolgen"},
/* fr */ {IDCLS_TRACE_DRIVE0_CPU_FR, "Tracer le CPU de drive0"},
/* it */ {IDCLS_TRACE_DRIVE0_CPU_IT, "Traccia la CPU del drive0"},
/* nl */ {IDCLS_TRACE_DRIVE0_CPU_NL, "Traceer de CPU van drive 0/8"},
/* pl */ {IDCLS_TRACE_DRIVE0_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_DRIVE0_CPU_SV, "Spåra processor i diskettstation 0"},

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE0_CPU,    "Do not trace the drive0 CPU"},
/* de */ {IDCLS_DONT_TRACE_DRIVE0_CPU_DE, "Laufwerk0 CPU nicht verfolgen"},
/* fr */ {IDCLS_DONT_TRACE_DRIVE0_CPU_FR, "Ne pas tracer le CPU de drive0"},
/* it */ {IDCLS_DONT_TRACE_DRIVE0_CPU_IT, "Non tracciare la CPU del drive0"},
/* nl */ {IDCLS_DONT_TRACE_DRIVE0_CPU_NL, "Traceer de CPU van drive 0/8 niet"},
/* pl */ {IDCLS_DONT_TRACE_DRIVE0_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_DRIVE0_CPU_SV, "Spåra inte processor i diskettstation 0"},

/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE1_CPU,    "Trace the drive1 CPU"},
/* de */ {IDCLS_TRACE_DRIVE1_CPU_DE, "Laufwerk1 CPU verfolgen"},
/* fr */ {IDCLS_TRACE_DRIVE1_CPU_FR, "Tracer le CPU de drive1"},
/* it */ {IDCLS_TRACE_DRIVE1_CPU_IT, "Traccia la CPU del drive1"},
/* nl */ {IDCLS_TRACE_DRIVE1_CPU_NL, "Traceer de CPU van drive 1/9"},
/* pl */ {IDCLS_TRACE_DRIVE1_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_DRIVE1_CPU_SV, "Spåra processor i diskettstation 1"},

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE1_CPU,    "Do not trace the drive1 CPU"},
/* de */ {IDCLS_DONT_TRACE_DRIVE1_CPU_DE, "Laufwerk1 CPU nicht verfolgen"},
/* fr */ {IDCLS_DONT_TRACE_DRIVE1_CPU_FR, "Ne pas tracer le CPU de drive1"},
/* it */ {IDCLS_DONT_TRACE_DRIVE1_CPU_IT, "Non tracciare la CPU del drive1"},
/* nl */ {IDCLS_DONT_TRACE_DRIVE1_CPU_NL, "Traceer de CPU van drive 1/9 niet"},
/* pl */ {IDCLS_DONT_TRACE_DRIVE1_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_DRIVE1_CPU_SV, "Spåra inte processor i diskettstation 1"},

#if DRIVE_NUM > 2
/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE2_CPU,    "Trace the drive2 CPU"},
/* de */ {IDCLS_TRACE_DRIVE2_CPU_DE, "Laufwerk2 CPU verfolgen"},
/* fr */ {IDCLS_TRACE_DRIVE2_CPU_FR, "Tracer le CPU de drive2"},
/* it */ {IDCLS_TRACE_DRIVE2_CPU_IT, "Traccia la CPU del drive2"},
/* nl */ {IDCLS_TRACE_DRIVE2_CPU_NL, "Traceer de CPU van drive 2/10"},
/* pl */ {IDCLS_TRACE_DRIVE2_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_DRIVE2_CPU_SV, "Spåra processor i diskettstation 2"},

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE2_CPU,    "Do not trace the drive2 CPU"},
/* de */ {IDCLS_DONT_TRACE_DRIVE2_CPU_DE, "Laufwerk2 CPU nicht verfolgen"},
/* fr */ {IDCLS_DONT_TRACE_DRIVE2_CPU_FR, "Ne pas tracer le CPU de drive2"},
/* it */ {IDCLS_DONT_TRACE_DRIVE2_CPU_IT, "Non tracciare la CPU del drive2"},
/* nl */ {IDCLS_DONT_TRACE_DRIVE2_CPU_NL, "Traceer de CPU van drive 2/10 niet"},
/* pl */ {IDCLS_DONT_TRACE_DRIVE2_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_DRIVE2_CPU_SV, "Spåra inte processor i diskettstation 2"},
#endif

#if DRIVE_NUM > 3
/* debug.c */
/* en */ {IDCLS_TRACE_DRIVE3_CPU,    "Trace the drive3 CPU"},
/* de */ {IDCLS_TRACE_DRIVE3_CPU_DE, "Laufwerk3 CPU verfolgen"},
/* fr */ {IDCLS_TRACE_DRIVE3_CPU_FR, "Tracer le CPU de drive3"},
/* it */ {IDCLS_TRACE_DRIVE3_CPU_IT, "Traccia la CPU del drive3"},
/* nl */ {IDCLS_TRACE_DRIVE3_CPU_NL, "Traceer de CPU van drive 3/11"},
/* pl */ {IDCLS_TRACE_DRIVE3_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_DRIVE3_CPU_SV, "Spåra processor i diskettstation 3"},

/* debug.c */
/* en */ {IDCLS_DONT_TRACE_DRIVE3_CPU,    "Do not trace the drive3 CPU"},
/* de */ {IDCLS_DONT_TRACE_DRIVE3_CPU_DE, "Laufwerk3 CPU nicht verfolgen"},
/* fr */ {IDCLS_DONT_TRACE_DRIVE3_CPU_FR, "Ne pas tracer le CPU de drive3"},
/* it */ {IDCLS_DONT_TRACE_DRIVE3_CPU_IT, "Non tracciare la CPU del drive3"},
/* nl */ {IDCLS_DONT_TRACE_DRIVE3_CPU_NL, "Traceer de CPU van drive 3/11 niet"},
/* pl */ {IDCLS_DONT_TRACE_DRIVE3_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_TRACE_DRIVE3_CPU_SV, "Spåra inte processor i diskettstation 0"},
#endif

/* debug.c */
/* en */ {IDCLS_TRACE_MODE,    "Trace mode (0=normal 1=small 2=history)"},
/* de */ {IDCLS_TRACE_MODE_DE, "Verfolgung Modus (0=normal 1=klein 2=Geschichte)"},
/* fr */ {IDCLS_TRACE_MODE_FR, "Mode de trace (0=normal 1=petit 2=historique)"},
/* it */ {IDCLS_TRACE_MODE_IT, "Modalità di tracciamento (0=normale 1=piccola 2=storica)"},
/* nl */ {IDCLS_TRACE_MODE_NL, "Traceer modus (0=normaal 1=klein 2=geschiedenis)"},
/* pl */ {IDCLS_TRACE_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_TRACE_MODE_SV, "Spårningsläge (0=normal 1=liten 2=historik)"},
#endif

/* event.c */
/* en */ {IDCLS_PLAYBACK_RECORDED_EVENTS,    "Playback recorded events"},
/* de */ {IDCLS_PLAYBACK_RECORDED_EVENTS_DE, "Wiedergabe von aufgenommener Ereignisse"},
/* fr */ {IDCLS_PLAYBACK_RECORDED_EVENTS_FR, "Jouer les événements enregistrés"},
/* it */ {IDCLS_PLAYBACK_RECORDED_EVENTS_IT, "Riproduzione degli eventi registrati"},
/* nl */ {IDCLS_PLAYBACK_RECORDED_EVENTS_NL, "Afspelen opgenomen gebeurtenissen"},
/* pl */ {IDCLS_PLAYBACK_RECORDED_EVENTS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PLAYBACK_RECORDED_EVENTS_SV, "Spela upp inspelade händelser"},

/* fliplist.c */
/* en */ {IDCLS_SPECIFY_FLIP_LIST_NAME,    "Specify name of the flip list file image"},
/* de */ {IDCLS_SPECIFY_FLIP_LIST_NAME_DE, "Namen für Fliplist Datei definieren"},
/* fr */ {IDCLS_SPECIFY_FLIP_LIST_NAME_FR, "Spécifier le nom de l'image du fichier de groupement de disques"},
/* it */ {IDCLS_SPECIFY_FLIP_LIST_NAME_IT, "Specifica il nome dell'immagine del file della flip list"},
/* nl */ {IDCLS_SPECIFY_FLIP_LIST_NAME_NL, "Geef de naam van het flip lijst bestand"},
/* pl */ {IDCLS_SPECIFY_FLIP_LIST_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_FLIP_LIST_NAME_SV, "Ange namn på vallistefilavbildningen"},

/* initcmdline.c */
/* en */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS,    "Show a list of the available options and exit normally"},
/* de */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_DE, "Liste von verfügbaren Optionen zeigen und beenden"},
/* fr */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_FR, "Montrer une liste des options disponibles et terminer normalement"},
/* it */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_IT, "Mostra la lista delle opzioni disponibili ed esce"},
/* nl */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_NL, "Toon een lijst van de beschikbare opties en exit zoals normaal"},
/* pl */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_SV, "Visa lista över tillgängliga flaggor och avsluta normalt"},

#if (!defined  __OS2__ && !defined __BEOS__)
/* initcmdline.c */
/* en */ {IDCLS_CONSOLE_MODE,    "Console mode (for music playback)"},
/* de */ {IDCLS_CONSOLE_MODE_DE, "Konsolenmodus (für Musikwiedergabe)"},
/* fr */ {IDCLS_CONSOLE_MODE_FR, "Mode console (pour l'écoute de musique)"},
/* it */ {IDCLS_CONSOLE_MODE_IT, "Modalità console (per la riproduzione musicale)"},
/* nl */ {IDCLS_CONSOLE_MODE_NL, "Console modus (voor afspelen voor muziek)"},
/* pl */ {IDCLS_CONSOLE_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_CONSOLE_MODE_SV, "Konsolläge (för musikspelning)"},

/* initcmdline.c */
/* en */ {IDCLS_ALLOW_CORE_DUMPS,    "Allow production of core dumps"},
/* de */ {IDCLS_ALLOW_CORE_DUMPS_DE, "Core Dumps ermöglichen"},
/* fr */ {IDCLS_ALLOW_CORE_DUMPS_FR, "Permettre la production de \"core dumps\""},
/* it */ {IDCLS_ALLOW_CORE_DUMPS_IT, "Consente la produzione di core dump"},
/* nl */ {IDCLS_ALLOW_CORE_DUMPS_NL, "Maken van core dumps toestaan"},
/* pl */ {IDCLS_ALLOW_CORE_DUMPS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ALLOW_CORE_DUMPS_SV, "Tillåt skapa minnesutskrift"},

/* initcmdline.c */
/* en */ {IDCLS_DONT_ALLOW_CORE_DUMPS,    "Do not produce core dumps"},
/* de */ {IDCLS_DONT_ALLOW_CORE_DUMPS_DE, "Core Dumps verhindern"},
/* fr */ {IDCLS_DONT_ALLOW_CORE_DUMPS_FR, "Ne pas produire de \"core dumps\""},
/* it */ {IDCLS_DONT_ALLOW_CORE_DUMPS_IT, "Non produce core dump"},
/* nl */ {IDCLS_DONT_ALLOW_CORE_DUMPS_NL, "Maken van core dumps niet toestaan"},
/* pl */ {IDCLS_DONT_ALLOW_CORE_DUMPS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_ALLOW_CORE_DUMPS_SV, "Skapa inte minnesutskrift"},
#else
/* initcmdline.c */
/* en */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER,    "Don't call exception handler"},
/* de */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_DE, "Ausnahmebehandlung vermeiden"},
/* fr */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_FR, "Ne pas utiliser l'assistant d'exception"},
/* it */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_IT, "Richiama il gestore delle eccezioni"},
/* nl */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_NL, "Geen gebruik maken van de exception handler"},
/* pl */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_SV, "Anropa inte undantagshanterare"},

/* initcmdline.c */
/* en */ {IDCLS_CALL_EXCEPTION_HANDLER,    "Call exception handler (default)"},
/* de */ {IDCLS_CALL_EXCEPTION_HANDLER_DE, "Ausnahmebehandlung aktivieren (Default)"},
/* fr */ {IDCLS_CALL_EXCEPTION_HANDLER_FR, "Utiliser l'assistant d'exception par défaut"},
/* it */ {IDCLS_CALL_EXCEPTION_HANDLER_IT, "Richiama il gestore delle eccezioni (predefinito)"},
/* nl */ {IDCLS_CALL_EXCEPTION_HANDLER_NL, "Gebruik maken van de exception handler (standaard)"},
/* pl */ {IDCLS_CALL_EXCEPTION_HANDLER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_CALL_EXCEPTION_HANDLER_SV, "Anropa undantagshanterare (standard)"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_RESTORE_DEFAULT_SETTINGS,    "Restore default (factory) settings"},
/* de */ {IDCLS_RESTORE_DEFAULT_SETTINGS_DE, "Wiederherstellen Standard Einstellungen"},
/* fr */ {IDCLS_RESTORE_DEFAULT_SETTINGS_FR, "Rétablir les paramètres par défaut"},
/* it */ {IDCLS_RESTORE_DEFAULT_SETTINGS_IT, "Ripristina le impostazioni originarie"},
/* nl */ {IDCLS_RESTORE_DEFAULT_SETTINGS_NL, "Herstel standaard instelling"},
/* pl */ {IDCLS_RESTORE_DEFAULT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_RESTORE_DEFAULT_SETTINGS_SV, "Återställ förvalda inställningar"},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AND_AUTOSTART,    "Attach and autostart tape/disk image <name>"},
/* de */ {IDCLS_ATTACH_AND_AUTOSTART_DE, "Einlegen und Autostart Disk/Band Image Datei <Name>"},
/* fr */ {IDCLS_ATTACH_AND_AUTOSTART_FR, "Insérer et démarrer l'image de disque/datassette <nom>"},
/* it */ {IDCLS_ATTACH_AND_AUTOSTART_IT, "Seleziona ed avvia l'immagine di una cassetta/disco <nome>"},
/* nl */ {IDCLS_ATTACH_AND_AUTOSTART_NL, "Koppel en autostart een tape/disk bestand <naam>"},
/* pl */ {IDCLS_ATTACH_AND_AUTOSTART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AND_AUTOSTART_SV, "Anslut och starta band-/diskettavbildning <namn>"},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AND_AUTOLOAD,    "Attach and autoload tape/disk image <name>"},
/* de */ {IDCLS_ATTACH_AND_AUTOLOAD_DE, "Einlegen und Autoload Disk/Band Image Datei <Name>"},
/* fr */ {IDCLS_ATTACH_AND_AUTOLOAD_FR, "Insérer et charger l'image de disque/datassette <nom>"},
/* it */ {IDCLS_ATTACH_AND_AUTOLOAD_IT, "Seleziona e carica l'immagine di una cassetta/disco <nome>"},
/* nl */ {IDCLS_ATTACH_AND_AUTOLOAD_NL, "Koppel en autolaad een tape/disk bestand <naam>"},
/* pl */ {IDCLS_ATTACH_AND_AUTOLOAD_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AND_AUTOLOAD_SV, "Anslut och läs in band-/diskettavbildning <namn>"},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_TAPE,    "Attach <name> as a tape image"},
/* de */ {IDCLS_ATTACH_AS_TAPE_DE, "Image Datei <Name> einlegen"},
/* fr */ {IDCLS_ATTACH_AS_TAPE_FR, "Insérer <nom> comme image de datassette"},
/* it */ {IDCLS_ATTACH_AS_TAPE_IT, "Seleziona <nome> come un'immagine di una cassetta"},
/* nl */ {IDCLS_ATTACH_AS_TAPE_NL, "Koppel <naam> als een tape bestand"},
/* pl */ {IDCLS_ATTACH_AS_TAPE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_TAPE_SV, "Anslut <namn> som bandavbildning"},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_8,    "Attach <name> as a disk image in drive #8"},
/* de */ {IDCLS_ATTACH_AS_DISK_8_DE, "Image Datei <Name> im Laufwerk #8 einlegen"},
/* fr */ {IDCLS_ATTACH_AS_DISK_8_FR, "Insérer <nom> comme image de disque dans le lecteur #8"},
/* it */ {IDCLS_ATTACH_AS_DISK_8_IT, "Seleziona <nome> come un'immagine di un disco nel drive #8"},
/* nl */ {IDCLS_ATTACH_AS_DISK_8_NL, "Koppel <naam> als een disk bestand in drive #8"},
/* pl */ {IDCLS_ATTACH_AS_DISK_8_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_DISK_8_SV, "Anslut <namn> som diskettavbildning i enhet 8"},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_9,    "Attach <name> as a disk image in drive #9"},
/* de */ {IDCLS_ATTACH_AS_DISK_9_DE, "Image Datei <Name> im Laufwerk #9 einlegen"},
/* fr */ {IDCLS_ATTACH_AS_DISK_9_FR, "Insérer <nom> comme image de disque dans le lecteur #9"},
/* it */ {IDCLS_ATTACH_AS_DISK_9_IT, "Seleziona <nome> come un'immagine di un disco nel drive #9"},
/* nl */ {IDCLS_ATTACH_AS_DISK_9_NL, "Koppel <naam> als een disk bestand in drive #9"},
/* pl */ {IDCLS_ATTACH_AS_DISK_9_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_DISK_9_SV, "Anslut <namn> som diskettavbildning i enhet 9"},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_10,    "Attach <name> as a disk image in drive #10"},
/* de */ {IDCLS_ATTACH_AS_DISK_10_DE, "Image Datei <Name> im Laufwerk #10 einlegen"},
/* fr */ {IDCLS_ATTACH_AS_DISK_10_FR, "Insérer <nom> comme image de disque dans le lecteur #10"},
/* it */ {IDCLS_ATTACH_AS_DISK_10_IT, "Seleziona <nome> come un'immagine di un disco nel drive #10"},
/* nl */ {IDCLS_ATTACH_AS_DISK_10_NL, "Koppel <naam> als een disk bestand in drive #10"},
/* pl */ {IDCLS_ATTACH_AS_DISK_10_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_DISK_10_SV, "Anslut <namn> som diskettavbildning i enhet 10"},

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_11,    "Attach <name> as a disk image in drive #11"},
/* de */ {IDCLS_ATTACH_AS_DISK_11_DE, "Image Datei <Name> im Laufwerk #11 einlegen"},
/* fr */ {IDCLS_ATTACH_AS_DISK_11_FR, "Insérer <nom> comme image de disque dans le lecteur #11"},
/* it */ {IDCLS_ATTACH_AS_DISK_11_IT, "Seleziona <nome> come un'immagine di un disco nel drive #11"},
/* nl */ {IDCLS_ATTACH_AS_DISK_11_NL, "Koppel <naam> als een disk bestand in drive #11"},
/* pl */ {IDCLS_ATTACH_AS_DISK_11_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_AS_DISK_11_SV, "Anslut <namn> som diskettavbildning i enhet 11"},

/* kbdbuf.c */
/* en */ {IDCLS_P_STRING,    "<string>"},
/* de */ {IDCLS_P_STRING_DE, "<String>"},
/* fr */ {IDCLS_P_STRING_FR, "<chaine>"},
/* it */ {IDCLS_P_STRING_IT, "<stringa>"},
/* nl */ {IDCLS_P_STRING_NL, "<string>"},
/* pl */ {IDCLS_P_STRING_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_STRING_SV, "<sträng>"},

/* kbdbuf.c */
/* en */ {IDCLS_PUT_STRING_INTO_KEYBUF,    "Put the specified string into the keyboard buffer"},
/* de */ {IDCLS_PUT_STRING_INTO_KEYBUF_DE, "Definierte Eingabe in Tastaturpuffer bereitstellen"},
/* fr */ {IDCLS_PUT_STRING_INTO_KEYBUF_FR, "Placer la chaîne de caractères spécifiée dans le tampon clavier"},
/* it */ {IDCLS_PUT_STRING_INTO_KEYBUF_IT, "Metti la stringa specificata nel buffer di tastiera"},
/* nl */ {IDCLS_PUT_STRING_INTO_KEYBUF_NL, "Stop de opgegeven string in de toetsenbord buffer"},
/* pl */ {IDCLS_PUT_STRING_INTO_KEYBUF_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PUT_STRING_INTO_KEYBUF_SV, "Lägg den angivna strängen i tangentbordsbufferten"},

/* log.c */
/* en */ {IDCLS_SPECIFY_LOG_FILE_NAME,    "Specify log file name"},
/* de */ {IDCLS_SPECIFY_LOG_FILE_NAME_DE, "Logdateiname definieren"},
/* fr */ {IDCLS_SPECIFY_LOG_FILE_NAME_FR, "Spécifier le nom du fichier log"},
/* it */ {IDCLS_SPECIFY_LOG_FILE_NAME_IT, "Specifica il nome del file di log"},
/* nl */ {IDCLS_SPECIFY_LOG_FILE_NAME_NL, "Geef de naam van het log bestand"},
/* pl */ {IDCLS_SPECIFY_LOG_FILE_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_LOG_FILE_NAME_SV, "Ange namn på loggfil"},

/* mouse.c */
/* en */ {IDCLS_ENABLE_1351_MOUSE,    "Enable emulation of the 1351 proportional mouse"},
/* de */ {IDCLS_ENABLE_1351_MOUSE_DE, "Emulation der 1351 Proportional Mouse aktivieren"},
/* fr */ {IDCLS_ENABLE_1351_MOUSE_FR, "Activer l'émulation de la souris 1351"},
/* it */ {IDCLS_ENABLE_1351_MOUSE_IT, "Attiva l'emulazione del mouse 1351"},
/* nl */ {IDCLS_ENABLE_1351_MOUSE_NL, "Aktiveer emulatie van de 1351 proportionele muis"},
/* pl */ {IDCLS_ENABLE_1351_MOUSE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_1351_MOUSE_SV, "Aktivera emulering av proportionell mus 1351"},

/* mouse.c */
/* en */ {IDCLS_DISABLE_1351_MOUSE,    "Disable emulation of the 1351 proportional mouse"},
/* de */ {IDCLS_DISABLE_1351_MOUSE_DE, "Emulation der 1351 Proportional Mouse deaktivieren"},
/* fr */ {IDCLS_DISABLE_1351_MOUSE_FR, "Désactiver l'émulation de la souris 1351"},
/* it */ {IDCLS_DISABLE_1351_MOUSE_IT, "Disattiva l'emulazione del mouse 1351"},
/* nl */ {IDCLS_DISABLE_1351_MOUSE_NL, "Emulatie van de 1351 proportionele muis afsluiten"},
/* pl */ {IDCLS_DISABLE_1351_MOUSE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_1351_MOUSE_SV, "Inaktivera emulering av proportionell mus 1351"},

/* mouse.c */
/* en */ {IDCLS_SELECT_MOUSE_JOY_PORT,    "Select the joystick port the mouse is attached to"},
/* de */ {IDCLS_SELECT_MOUSE_JOY_PORT_DE, "Joystickport für Maus auswählen"},
/* fr */ {IDCLS_SELECT_MOUSE_JOY_PORT_FR, "Sélectionner le port joystick sur lequel la souris est attachée"},
/* it */ {IDCLS_SELECT_MOUSE_JOY_PORT_IT, "Seleziona la porta joystick a cui è collegato il mouse"},
/* nl */ {IDCLS_SELECT_MOUSE_JOY_PORT_NL, "Selecteer de joystick poort waar de muis aan gekoppelt is"},
/* pl */ {IDCLS_SELECT_MOUSE_JOY_PORT_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SELECT_MOUSE_JOY_PORT_SV, "Ange vilken spelport musen är ansluten till"},

/* ram.c */
/* en */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE,    "Set the value for the very first RAM address after powerup"},
/* de */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_DE, "Wert für erstes Byte im RAM nach Kaltstart setzen"},
/* fr */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_FR, "Spécifier la valeur de la première adresse RAM après la mise sous tension"},
/* it */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_IT, "Imposta il valore del primissimo indirizzo della RAM dopo l'accensione"},
/* nl */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_NL, "Zet de waarde voor het allereerste RAM adres na koude start"},
/* pl */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_SV, "Ange värdet på den allra första RAM-adressen efter strömpåslag"},

/* ram.c */
/* en */ {IDCLS_P_NUM_OF_BYTES,    "<num of bytes>"},
/* de */ {IDCLS_P_NUM_OF_BYTES_DE, "<Anzahl an Bytes>"},
/* fr */ {IDCLS_P_NUM_OF_BYTES_FR, "<nombre d'octets>"},
/* it */ {IDCLS_P_NUM_OF_BYTES_IT, "<numero di byte>"},
/* nl */ {IDCLS_P_NUM_OF_BYTES_NL, "<aantal bytes>"},
/* pl */ {IDCLS_P_NUM_OF_BYTES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_NUM_OF_BYTES_SV, "<antal byte>"},

/* ram.c */
/* en */ {IDCLS_LENGTH_BLOCK_SAME_VALUE,    "Length of memory block initialized with the same value"},
/* de */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_DE, "Länge des Speicherblocks der mit dem gleichen Wert initialisiert ist"},
/* fr */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_FR, "Longeur du premier bloc mémoire initialisé avec la même valeur"},
/* it */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_IT, "Lunghezza del blocco di memoria inizializzato con lo stesso valore"},
/* nl */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_NL, "Geheugen blok grootte die dezelfde waarde krijgt bij initialisatie"},
/* pl */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_SV, "Längd på minnesblock som initierats med samma värde"},

/* ram.c */
/* en */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN,    "Length of memory block initialized with the same pattern"},
/* de */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_DE, "Länge des Speicherblocks der mit dem gleichen Muster initialisiert ist"},
/* fr */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_FR, "Longeur du premier bloc mémoire initialisé avec le même pattern"},
/* it */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_IT, "Lunghezza del blocco di memoria inizializzato con lo stesso pattern"},
/* nl */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_NL, "Geheugen blok grootte met hetzelfde patroon bij initialisatie"},
/* pl */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_SV, "Längd på minnesblock som initierats med samma mönster"},

/* sound.c */
/* en */ {IDCLS_ENABLE_SOUND_PLAYBACK,    "Enable sound playback"},
/* de */ {IDCLS_ENABLE_SOUND_PLAYBACK_DE, "Sound Wiedergaben einschalten"},
/* fr */ {IDCLS_ENABLE_SOUND_PLAYBACK_FR, "Activer le son"},
/* it */ {IDCLS_ENABLE_SOUND_PLAYBACK_IT, "Attiva la riproduzione del suono"},
/* nl */ {IDCLS_ENABLE_SOUND_PLAYBACK_NL, "Aktiveer geluid"},
/* pl */ {IDCLS_ENABLE_SOUND_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SOUND_PLAYBACK_SV, "Aktivera ljudåtergivning"},

/* sound.c */
/* en */ {IDCLS_DISABLE_SOUND_PLAYBACK,    "Disable sound playback"},
/* de */ {IDCLS_DISABLE_SOUND_PLAYBACK_DE, "Sound Wiedergaben ausschalten"},
/* fr */ {IDCLS_DISABLE_SOUND_PLAYBACK_FR, "Désactiver le son"},
/* it */ {IDCLS_DISABLE_SOUND_PLAYBACK_IT, "Disattiva la riproduzione del suono"},
/* nl */ {IDCLS_DISABLE_SOUND_PLAYBACK_NL, "Geluid afsluiten"},
/* pl */ {IDCLS_DISABLE_SOUND_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SOUND_PLAYBACK_SV, "Inaktivera ljudåtergivning"},

/* sound.c */
/* en */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ,    "Set sound sample rate to <value> Hz"},
/* de */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_DE, "Setze Sound Sample Rate zu <Wert> Hz"},
/* fr */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_FR, "Régler le taux d'échantillonage à <valeur> Hz"},
/* it */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_IT, "Imposta la velocità di campionamento del suono a <valore> Hz"},
/* nl */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_NL, "Zet de geluid sample rate naar <waarde> Hz"},
/* pl */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SAMPLE_RATE_VALUE_HZ_SV, "Sätt ljudsamplingshastighet till <värde> Hz"},

/* sound.c */
/* en */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC,    "Set sound buffer size to <value> msec"},
/* de */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_DE, "Setze Source Buffer Größe zu <Wert> msek"},
/* fr */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_FR, "Régler la taille du tampon à <valeur> ms"},
/* it */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_IT, "Imposta la dimensione del buffer del suono a <valore> msec"},
/* nl */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_NL, "Zet de geluid buffer grootte naar <waarde> msec"},
/* pl */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_SV, "Sätt ljudbuffertstorlek till <värde> ms"},

/* sound.c */
/* en */ {IDCLS_SPECIFY_SOUND_DRIVER,    "Specify sound driver"},
/* de */ {IDCLS_SPECIFY_SOUND_DRIVER_DE, "Sound Treiber spezifieren"},
/* fr */ {IDCLS_SPECIFY_SOUND_DRIVER_FR, "Spécifier le pilote son"},
/* it */ {IDCLS_SPECIFY_SOUND_DRIVER_IT, "Specifica il driver audio"},
/* nl */ {IDCLS_SPECIFY_SOUND_DRIVER_NL, "Geef geluid stuurprogramma"},
/* pl */ {IDCLS_SPECIFY_SOUND_DRIVER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SOUND_DRIVER_SV, "Ange ljuddrivrutin"},

/* sound.c */
/* en */ {IDCLS_P_ARGS,    "<args>"},
/* de */ {IDCLS_P_ARGS_DE, "<Argumente>"},
/* fr */ {IDCLS_P_ARGS_FR, "<args>"},
/* it */ {IDCLS_P_ARGS_IT, "<argomenti>"},
/* nl */ {IDCLS_P_ARGS_NL, "<parameters>"},
/* pl */ {IDCLS_P_ARGS_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_ARGS_SV, "<flaggor>"},

/* sound.c */
/* en */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM,    "Specify initialization parameters for sound driver"},
/* de */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_DE, "Initialisierungsparameter des Sound Treibers spezifizieren"},
/* fr */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_FR, "Spécifier les paramètres d'initialisation pour le pilote son"},
/* it */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_IT, "Specifica i parametri di inizializzazione del driver audio"},
/* nl */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_NL, "Geef initialisatie parameters voor het geluid stuurprogramma"},
/* pl */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SOUND_DRIVER_PARAM_SV, "Ange initieringsflaggor för ljuddrivrutin"},

/* sound.c */
/* en */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER,    "Specify recording sound driver"},
/* de */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_DE, "Sound Treiber für Aufnahme spezifizieren"},
/* fr */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_FR, "Spécifier le pilote d'enregistrement son"},
/* it */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_IT, "Specifica il driver di registrazione del suono"},
/* nl */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_NL, "Geef geluid stuurprogramma voor opname"},
/* pl */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_SV, "Ange ljuddrivrutin för inspelning"},

/* sound.c */
/* en */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM,    "Specify initialization parameters for recording sound driver"},
/* de */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_DE, "Initialisierungsparameter für Aufnahme Sound Treiber spezifieren"},
/* fr */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_FR, "Spécifier les paramètres d'initialisation pour le pilote d'enregistrement son"},
/* it */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_IT, "Specifica i parametri di inizializzazione per il driver di registrazione dell'audio"},
/* nl */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_NL, "Geef initialisatie parameters voor het geluid stuurprogramma voor opname"},
/* pl */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_SV, "Ange initieringsflaggor för ljuddrivrutin för inspelning"},

/* sound.c */
/* en */ {IDCLS_P_SYNC,    "<sync>"},
/* de */ {IDCLS_P_SYNC_DE, "<Sync>"},
/* fr */ {IDCLS_P_SYNC_FR, "<sync>"},
/* it */ {IDCLS_P_SYNC_IT, "<sync>"},
/* nl */ {IDCLS_P_SYNC_NL, "<sync>"},
/* pl */ {IDCLS_P_SYNC_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_SYNC_SV, "<synk>"},

/* sound.c */
/* en */ {IDCLS_SET_SOUND_SPEED_ADJUST,    "Set sound speed adjustment (0: flexible, 1: adjusting, 2: exact)"},
/* de */ {IDCLS_SET_SOUND_SPEED_ADJUST_DE, "Setze Sound Geschwindigkeit Anpassung (0: flexibel, 1: anpassend, 2: exakt)"},
/* fr */ {IDCLS_SET_SOUND_SPEED_ADJUST_FR, "Régler l'ajustement son (0: flexible, 1: ajusté 2: exact"},
/* it */ {IDCLS_SET_SOUND_SPEED_ADJUST_IT, "Imposta il tipo di adattamento della velocità del suono (0: flessibile, 1:adattabile, 2: esatta)"},
/* nl */ {IDCLS_SET_SOUND_SPEED_ADJUST_NL, "Zet geluid snelheid aanpassing (0: flexibel, 1: aanpassend, 2: exact)"},
/* pl */ {IDCLS_SET_SOUND_SPEED_ADJUST_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SOUND_SPEED_ADJUST_SV, "Ställ in ljudhastighetsjustering (0: flexibel, 1: justerande, 2: exakt)"},

/* sysfile.c */
/* en */ {IDCLS_P_PATH,    "<path>"},
/* de */ {IDCLS_P_PATH_DE, "<Pfad>"},
/* fr */ {IDCLS_P_PATH_FR, "<chemin>"},
/* it */ {IDCLS_P_PATH_IT, "<percorso>"},
/* nl */ {IDCLS_P_PATH_NL, "<pad>"},
/* pl */ {IDCLS_P_PATH_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_P_PATH_SV, "<sökväg>"},

/* sysfile.c */
/* en */ {IDCLS_DEFINE_SYSTEM_FILES_PATH,    "Define search path to locate system files"},
/* de */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_DE, "Suchpfad für Systemdateien definieren"},
/* fr */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_FR, "Définir le chemin de recherche pour trouver les fichiers systèmes"},
/* it */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_NL, "Geef het zoek pad waar de systeem bestanden te vinden zijn"},
/* pl */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_SV, "Ange sökväg för att hitta systemfiler"},

/* traps.c */
/* en */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION,    "Enable general mechanisms for fast disk/tape emulation"},
/* de */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_DE, "Allgemeine Mechanismen für schnelle Disk/Band Emulation aktivieren"},
/* fr */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_FR, "Activer les méchanismes généraux pour l'émulation disque/datassette rapide"},
/* it */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_IT, "Attiva meccanismo generale per l'emulazione veloce del disco/cassetta"},
/* nl */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_NL, "Aktiveer algemene methoden voor snelle disk/tape emulatie"},
/* pl */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TRAPS_FAST_EMULATION_SV, "Aktivera generella mekanismer för snabb disk-/bandemulering"},

/* traps.c */
/* en */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION,    "Disable general mechanisms for fast disk/tape emulation"},
/* de */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_DE, "Allgemeine Mechanismen für schnelle Disk/Band Emulation deaktivieren"},
/* fr */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_FR, "Désactiver les méchanismes généraux pour l'émulation disque/datassette rapide"},
/* it */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_IT, "Disattiva meccanismo generale per l'emulazione veloce del disco/cassetta"},
/* nl */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_NL, "Algemene methoden voor snelle disk/tape emulatie afsluiten"},
/* pl */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_TRAPS_FAST_EMULATION_SV, "Inaktivera generella mekanismer för snabb disk-/bandemulering"},

/* vsync.c */
/* en */ {IDCLS_LIMIT_SPEED_TO_VALUE,    "Limit emulation speed to specified value"},
/* de */ {IDCLS_LIMIT_SPEED_TO_VALUE_DE, "Emulationsgeschwindigkeit auf Wert beschränken."},
/* fr */ {IDCLS_LIMIT_SPEED_TO_VALUE_FR, "Limiter la vitesse d'émulation à une valeur specifiée"},
/* it */ {IDCLS_LIMIT_SPEED_TO_VALUE_IT, "Limita la velocità di emulazione al valore specificato"},
/* nl */ {IDCLS_LIMIT_SPEED_TO_VALUE_NL, "Limiteer de emulatie snelheid tot de opgegeven waarde"},
/* pl */ {IDCLS_LIMIT_SPEED_TO_VALUE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_LIMIT_SPEED_TO_VALUE_SV, "Begränsa emuleringshastighet till angivet värde"},

/* vsync.c */
/* en */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES,    "Update every <value> frames (`0' for automatic)"},
/* de */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_DE, "Jedes <Wert> Bild aktualisieren (`0' für Automatik)"},
/* fr */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_FR, "Mise à jour toutes les <valeur> images (`0' pour auto.)"},
/* it */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_IT, "Aggiorna ogni <valore> frame (`0' per automatico)"},
/* nl */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_NL, "Scherm opbouw elke <waarde> frames (`0' voor automatisch)"},
/* pl */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_UPDATE_EVERY_VALUE_FRAMES_SV, "Uppdatera varje <värde> ramar (\"0\" för automatiskt)"},

/* vsync.c */
/* en */ {IDCLS_ENABLE_WARP_MODE,    "Enable warp mode"},
/* de */ {IDCLS_ENABLE_WARP_MODE_DE, "Warp Mode Aktivieren"},
/* fr */ {IDCLS_ENABLE_WARP_MODE_FR, "Activer mode turbo"},
/* it */ {IDCLS_ENABLE_WARP_MODE_IT, "Attiva la modalità turbo"},
/* nl */ {IDCLS_ENABLE_WARP_MODE_NL, "Aktiveer warp modus"},
/* pl */ {IDCLS_ENABLE_WARP_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_WARP_MODE_SV, "Aktivera warpläge"},

/* vsync.c */
/* en */ {IDCLS_DISABLE_WARP_MODE,    "Disable warp mode"},
/* de */ {IDCLS_DISABLE_WARP_MODE_DE, "*Warp Mode Deaktivieren"},
/* fr */ {IDCLS_DISABLE_WARP_MODE_FR, "Désactiver mode turbo"},
/* it */ {IDCLS_DISABLE_WARP_MODE_IT, "Disattiva la modalità turbo"},
/* nl */ {IDCLS_DISABLE_WARP_MODE_NL, "Warp modus afsluiten"},
/* pl */ {IDCLS_DISABLE_WARP_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_WARP_MODE_SV, "Inaktivera warpläge"},

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

/* c64/plus256k.c */
/* en */ {IDCLS_ENABLE_PLUS256K_EXPANSION,    "Enable the PLUS2566K RAM expansion"},
/* de */ {IDCLS_ENABLE_PLUS256K_EXPANSION_DE, "PLUS256K RAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_PLUS256K_EXPANSION_FR, "Activer l'expansion RAM PLUS256K"},
/* it */ {IDCLS_ENABLE_PLUS256K_EXPANSION_IT, "Attiva l'espansione PLUS256K RAM"},
/* nl */ {IDCLS_ENABLE_PLUS256K_EXPANSION_NL, "Aktiveer de PLUS256K geheugen uitbreiding"},
/* pl */ {IDCLS_ENABLE_PLUS256K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_PLUS256K_EXPANSION_SV, "Aktivera PLUS256K RAM-expansion"},

/* c64/plus256k.c */
/* en */ {IDCLS_DISABLE_PLUS256K_EXPANSION,    "Disable the PLUS256K RAM expansion"},
/* de */ {IDCLS_DISABLE_PLUS256K_EXPANSION_DE, "PLUS256K RAM Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_PLUS256K_EXPANSION_FR, "Désactiver l'expansion RAM PLUS256K"},
/* it */ {IDCLS_DISABLE_PLUS256K_EXPANSION_IT, "Disattiva l'espansione PLUS256K RAM"},
/* nl */ {IDCLS_DISABLE_PLUS256K_EXPANSION_NL, "De PLUS256K geheugen uitbreiding afsluiten"},
/* pl */ {IDCLS_DISABLE_PLUS256K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_PLUS256K_EXPANSION_SV, "Inaktivera PLUS256K RAM-expansion"},

/* c64/plus256k.c */
/* en */ {IDCLS_SPECIFY_PLUS256K_NAME,    "Specify name of PLUS256K image"},
/* de */ {IDCLS_SPECIFY_PLUS256K_NAME_DE, "Name der PLUS256K Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_PLUS256K_NAME_FR, "Spécifier le nom de l'image PLUS256K"},
/* it */ {IDCLS_SPECIFY_PLUS256K_NAME_IT, "Specifica il nome dell'immagine PLUS256K"},
/* nl */ {IDCLS_SPECIFY_PLUS256K_NAME_NL, "Geef de naam van het PLUS256K bestand"},
/* pl */ {IDCLS_SPECIFY_PLUS256K_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PLUS256K_NAME_SV, "Ange namn på PLUS256K-avbildning"},

/* c64/plus60k.c */
/* en */ {IDCLS_ENABLE_PLUS60K_EXPANSION,    "Enable the PLUS60K RAM expansion"},
/* de */ {IDCLS_ENABLE_PLUS60K_EXPANSION_DE, "PLUS60K RAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_PLUS60K_EXPANSION_FR, "Activer l'expansion RAM PLUS60K"},
/* it */ {IDCLS_ENABLE_PLUS60K_EXPANSION_IT, "Attiva l'espansione PLUS60K RAM"},
/* nl */ {IDCLS_ENABLE_PLUS60K_EXPANSION_NL, "Aktiveer de PLUS60K geheugen uitbreiding"},
/* pl */ {IDCLS_ENABLE_PLUS60K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_PLUS60K_EXPANSION_SV, "Aktivera PLUS60K RAM-expansion"},

/* c64/plus60k.c */
/* en */ {IDCLS_DISABLE_PLUS60K_EXPANSION,    "Disable the PLUS60K RAM expansion"},
/* de */ {IDCLS_DISABLE_PLUS60K_EXPANSION_DE, "PLUS60K RAM Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_PLUS60K_EXPANSION_FR, "Désactiver l'expansion RAM PLUS60K"},
/* it */ {IDCLS_DISABLE_PLUS60K_EXPANSION_IT, "Disattiva l'espansione PLUS60K RAM"},
/* nl */ {IDCLS_DISABLE_PLUS60K_EXPANSION_NL, "De PLUS60K geheugen uitbreiding afsluiten"},
/* pl */ {IDCLS_DISABLE_PLUS60K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_PLUS60K_EXPANSION_SV, "Inaktivera PLUS60K RAM-expansion"},

/* c64/plus60k.c */
/* en */ {IDCLS_SPECIFY_PLUS60K_NAME,    "Specify name of PLUS60K image"},
/* de */ {IDCLS_SPECIFY_PLUS60K_NAME_DE, "Name der PLUS60K Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_PLUS60K_NAME_FR, "Spécifier le nom de l'image PLUS60K"},
/* it */ {IDCLS_SPECIFY_PLUS60K_NAME_IT, "Specifica il nome dell'immagine PLUS60K"},
/* nl */ {IDCLS_SPECIFY_PLUS60K_NAME_NL, "Geef de naam van het PLUS60K bestand"},
/* pl */ {IDCLS_SPECIFY_PLUS60K_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PLUS60K_NAME_SV, "Ange namn på PLUS60K-avbildning"},

/* c64/plus60k.c */
/* en */ {IDCLS_PLUS60K_BASE,    "Base address of the PLUS60K expansion"},
/* de */ {IDCLS_PLUS60K_BASE_DE, "Basis Adresse für PLUS60K Erweiterung"},
/* fr */ {IDCLS_PLUS60K_BASE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_PLUS60K_BASE_IT, "Indirizzo base dell'espansione PLUS60K"},
/* nl */ {IDCLS_PLUS60K_BASE_NL, "Basis adres van de PLUS60K geheugen uitbreiding"},
/* pl */ {IDCLS_PLUS60K_BASE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_PLUS60K_BASE_SV, ""},  /* fuzzy */

/* c64/c64_256k.c */
/* en */ {IDCLS_ENABLE_C64_256K_EXPANSION,    "Enable the 256K RAM expansion"},
/* de */ {IDCLS_ENABLE_C64_256K_EXPANSION_DE, "256K RAM Erweiterung aktivieren"},
/* fr */ {IDCLS_ENABLE_C64_256K_EXPANSION_FR, "Activer l'expansion RAM 256K"},
/* it */ {IDCLS_ENABLE_C64_256K_EXPANSION_IT, "Attiva l'espansione 256K RAM"},
/* nl */ {IDCLS_ENABLE_C64_256K_EXPANSION_NL, "Aktiveer de 256K geheugen uitbreiding"},
/* pl */ {IDCLS_ENABLE_C64_256K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_C64_256K_EXPANSION_SV, "Aktivera 256K RAM-expansion"},

/* c64/c64_256k.c */
/* en */ {IDCLS_DISABLE_C64_256K_EXPANSION,    "Disable the 256K RAM expansion"},
/* de */ {IDCLS_DISABLE_C64_256K_EXPANSION_DE, "256K RAM Erweiterung deaktivieren"},
/* fr */ {IDCLS_DISABLE_C64_256K_EXPANSION_FR, "Désactiver l'expansion RAM 256K"},
/* it */ {IDCLS_DISABLE_C64_256K_EXPANSION_IT, "Disattiva l'espansione 256K RAM"},
/* nl */ {IDCLS_DISABLE_C64_256K_EXPANSION_NL, "De 256K geheugen uitbreiding afsluiten"},
/* pl */ {IDCLS_DISABLE_C64_256K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_C64_256K_EXPANSION_SV, "Inaktivera 256K RAM-expansion"},

/* c64/c64_256k.c */
/* en */ {IDCLS_SPECIFY_C64_256K_NAME,    "Specify name of 256K image"},
/* de */ {IDCLS_SPECIFY_C64_256K_NAME_DE, "Name der 256K Image Datei definieren"},
/* fr */ {IDCLS_SPECIFY_C64_256K_NAME_FR, "Spécifier le nom de l'image 256K"},
/* it */ {IDCLS_SPECIFY_C64_256K_NAME_IT, "Specifica il nome dell'immagine 256K"},
/* nl */ {IDCLS_SPECIFY_C64_256K_NAME_NL, "Geef de naam van het 256K bestand"},
/* pl */ {IDCLS_SPECIFY_C64_256K_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_C64_256K_NAME_SV, "Ange namn på 256K-avbildning"},

/* c64/c64_256k.c */
/* en */ {IDCLS_C64_256K_BASE,    "Base address of the 256K expansion"},
/* de */ {IDCLS_C64_256K_BASE_DE, "Basis Adresse für 256K RAM Erweiterung"},
/* fr */ {IDCLS_C64_256K_BASE_FR, ""},  /* fuzzy */
/* it */ {IDCLS_C64_256K_BASE_IT, "Indirizzo base dell'espansione 256K"},
/* nl */ {IDCLS_C64_256K_BASE_NL, "Basis adres van de 256K geheugen uitbreiding"},
/* pl */ {IDCLS_C64_256K_BASE_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_C64_256K_BASE_SV, ""},  /* fuzzy */

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_STB_CART,    "Attach raw Structured Basic cartridge image"},
/* de */ {IDCLS_ATTACH_RAW_STB_CART_DE, "Structured Basic (raw) Image einlegen"},
/* fr */ {IDCLS_ATTACH_RAW_STB_CART_FR, "Insérer une cartouche Structured Basic"},
/* it */ {IDCLS_ATTACH_RAW_STB_CART_IT, "Seleziona l'immagine di una cartuccia Structured Basic"},
/* nl */ {IDCLS_ATTACH_RAW_STB_CART_NL, "Koppel binair Structured Basic cartridge bestand"},
/* pl */ {IDCLS_ATTACH_RAW_STB_CART_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ATTACH_RAW_STB_CART_SV, "Anslut rå Structured Basic-insticksmodulfil"},

/* plus4/plus4memcsory256k.c */
/* en */ {IDCLS_ENABLE_CS256K_EXPANSION,    "Enable the CSORY 256K RAM expansion"},
/* de */ {IDCLS_ENABLE_CS256K_EXPANSION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_CS256K_EXPANSION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_CS256K_EXPANSION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_CS256K_EXPANSION_NL, "Aktiveer de CSORY 256K geheugen uitbreiding"},
/* pl */ {IDCLS_ENABLE_CS256K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_CS256K_EXPANSION_SV, ""},  /* fuzzy */

/* plus4/plus4memhannes256k.c */
/* en */ {IDCLS_ENABLE_H256K_EXPANSION,    "Enable the HANNES 256K RAM expansion"},
/* de */ {IDCLS_ENABLE_H256K_EXPANSION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_H256K_EXPANSION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_H256K_EXPANSION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_H256K_EXPANSION_NL, "Aktiveer de HANNES 256K geheugen uitbreiding"},
/* pl */ {IDCLS_ENABLE_H256K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_H256K_EXPANSION_SV, ""},  /* fuzzy */

/* plus4/plus4memhannes256k.c */
/* en */ {IDCLS_ENABLE_H1024K_EXPANSION,    "Enable the HANNES 1024K RAM expansion"},
/* de */ {IDCLS_ENABLE_H1024K_EXPANSION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_H1024K_EXPANSION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_H1024K_EXPANSION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_H1024K_EXPANSION_NL, "Aktiveer de HANNES 1024K geheugen uitbreiding"},
/* pl */ {IDCLS_ENABLE_H1024K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_H1024K_EXPANSION_SV, ""},  /* fuzzy */

/* plus4/plus4memhannes256k.c */
/* en */ {IDCLS_ENABLE_H4096K_EXPANSION,    "Enable the HANNES 4096K RAM expansion"},
/* de */ {IDCLS_ENABLE_H4096K_EXPANSION_DE, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_H4096K_EXPANSION_FR, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_H4096K_EXPANSION_IT, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_H4096K_EXPANSION_NL, "Aktiveer de HANNES 4096K geheugen uitbreiding"},
/* pl */ {IDCLS_ENABLE_H4096K_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_H4096K_EXPANSION_SV, ""}   /* fuzzy */

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

#ifdef HAVE_NETWORK
/* network.c */
/* en */ {IDGS_TESTING_BEST_FRAME_DELAY,
/* de */  IDGS_TESTING_BEST_FRAME_DELAY_DE,
/* fr */  IDGS_TESTING_BEST_FRAME_DELAY_FR,
/* it */  IDGS_TESTING_BEST_FRAME_DELAY_IT,
/* nl */  IDGS_TESTING_BEST_FRAME_DELAY_NL,
/* pl */  IDGS_TESTING_BEST_FRAME_DELAY_PL,
/* sv */  IDGS_TESTING_BEST_FRAME_DELAY_SV},

/* network.c */
/* en */ {IDGS_USING_D_FRAMES_DELAY,
/* de */  IDGS_USING_D_FRAMES_DELAY_DE,
/* fr */  IDGS_USING_D_FRAMES_DELAY_FR,
/* it */  IDGS_USING_D_FRAMES_DELAY_IT,
/* nl */  IDGS_USING_D_FRAMES_DELAY_NL,
/* pl */  IDGS_USING_D_FRAMES_DELAY_PL,
/* sv */  IDGS_USING_D_FRAMES_DELAY_SV},

/* network.c */
/* en */ {IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER,
/* de */  IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_DE,
/* fr */  IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_FR,
/* it */  IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_IT,
/* nl */  IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_NL,
/* pl */  IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_PL,
/* sv */  IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_SV},

/* network.c */
/* en */ {IDGS_SENDING_SNAPSHOT_TO_CLIENT,
/* de */  IDGS_SENDING_SNAPSHOT_TO_CLIENT_DE,
/* fr */  IDGS_SENDING_SNAPSHOT_TO_CLIENT_FR,
/* it */  IDGS_SENDING_SNAPSHOT_TO_CLIENT_IT,
/* nl */  IDGS_SENDING_SNAPSHOT_TO_CLIENT_NL,
/* pl */  IDGS_SENDING_SNAPSHOT_TO_CLIENT_PL,
/* sv */  IDGS_SENDING_SNAPSHOT_TO_CLIENT_SV},

/* network.c */
/* en */ {IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT,
/* de */  IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_DE,
/* fr */  IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_FR,
/* it */  IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_IT,
/* nl */  IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_NL,
/* pl */  IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_PL,
/* sv */  IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_SV},

/* network.c */
/* en */ {IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S,
/* de */  IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_DE,
/* fr */  IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_FR,
/* it */  IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_IT,
/* nl */  IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_NL,
/* pl */  IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_PL,
/* sv */  IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_SV},

/* network.c */
/* en */ {IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S,
/* de */  IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_DE,
/* fr */  IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_FR,
/* it */  IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_IT,
/* nl */  IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_NL,
/* pl */  IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_PL,
/* sv */  IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_SV},

/* network.c */
/* en */ {IDGS_SERVER_IS_WAITING_FOR_CLIENT,
/* de */  IDGS_SERVER_IS_WAITING_FOR_CLIENT_DE,
/* fr */  IDGS_SERVER_IS_WAITING_FOR_CLIENT_FR,
/* it */  IDGS_SERVER_IS_WAITING_FOR_CLIENT_IT,
/* nl */  IDGS_SERVER_IS_WAITING_FOR_CLIENT_NL,
/* pl */  IDGS_SERVER_IS_WAITING_FOR_CLIENT_PL,
/* sv */  IDGS_SERVER_IS_WAITING_FOR_CLIENT_SV},

/* network.c */
/* en */ {IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT,
/* de */  IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_DE,
/* fr */  IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_FR,
/* it */  IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_IT,
/* nl */  IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_NL,
/* pl */  IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_PL,
/* sv */  IDGS_CANNOT_CREATE_SNAPSHOT_S_SELECT_SV},

/* network.c */
/* en */ {IDGS_CANNOT_RESOLVE_S,
/* de */  IDGS_CANNOT_RESOLVE_S_DE,
/* fr */  IDGS_CANNOT_RESOLVE_S_FR,
/* it */  IDGS_CANNOT_RESOLVE_S_IT,
/* nl */  IDGS_CANNOT_RESOLVE_S_NL,
/* pl */  IDGS_CANNOT_RESOLVE_S_PL,
/* sv */  IDGS_CANNOT_RESOLVE_S_SV},

/* network.c */
/* en */ {IDGS_CANNOT_CONNECT_TO_S,
/* de */  IDGS_CANNOT_CONNECT_TO_S_DE,
/* fr */  IDGS_CANNOT_CONNECT_TO_S_FR,
/* it */  IDGS_CANNOT_CONNECT_TO_S_IT,
/* nl */  IDGS_CANNOT_CONNECT_TO_S_NL,
/* pl */  IDGS_CANNOT_CONNECT_TO_S_PL,
/* sv */  IDGS_CANNOT_CONNECT_TO_S_SV},

/* network.c */
/* en */ {IDGS_RECEIVING_SNAPSHOT_SERVER,
/* de */  IDGS_RECEIVING_SNAPSHOT_SERVER_DE,
/* fr */  IDGS_RECEIVING_SNAPSHOT_SERVER_FR,
/* it */  IDGS_RECEIVING_SNAPSHOT_SERVER_IT,
/* nl */  IDGS_RECEIVING_SNAPSHOT_SERVER_NL,
/* pl */  IDGS_RECEIVING_SNAPSHOT_SERVER_PL,
/* sv */  IDGS_RECEIVING_SNAPSHOT_SERVER_SV},

/* network.c */
/* en */ {IDGS_NETWORK_OUT_OF_SYNC,
/* de */  IDGS_NETWORK_OUT_OF_SYNC_DE,
/* fr */  IDGS_NETWORK_OUT_OF_SYNC_FR,
/* it */  IDGS_NETWORK_OUT_OF_SYNC_IT,
/* nl */  IDGS_NETWORK_OUT_OF_SYNC_NL,
/* pl */  IDGS_NETWORK_OUT_OF_SYNC_PL,
/* sv */  IDGS_NETWORK_OUT_OF_SYNC_SV},

/* network.c */
/* en */ {IDGS_REMOTE_HOST_DISCONNECTED,
/* de */  IDGS_REMOTE_HOST_DISCONNECTED_DE,
/* fr */  IDGS_REMOTE_HOST_DISCONNECTED_FR,
/* it */  IDGS_REMOTE_HOST_DISCONNECTED_IT,
/* nl */  IDGS_REMOTE_HOST_DISCONNECTED_NL,
/* pl */  IDGS_REMOTE_HOST_DISCONNECTED_PL,
/* sv */  IDGS_REMOTE_HOST_DISCONNECTED_SV},

/* network.c */
/* en */ {IDGS_REMOTE_HOST_SUSPENDING,
/* de */  IDGS_REMOTE_HOST_SUSPENDING_DE,
/* fr */  IDGS_REMOTE_HOST_SUSPENDING_FR,
/* it */  IDGS_REMOTE_HOST_SUSPENDING_IT,
/* nl */  IDGS_REMOTE_HOST_SUSPENDING_NL,
/* pl */  IDGS_REMOTE_HOST_SUSPENDING_PL,
/* sv */  IDGS_REMOTE_HOST_SUSPENDING_SV},

#ifdef HAVE_IPV6
/* network.c */
/* en */ {IDGS_CANNOT_SWITCH_IPV4_IPV6,
/* de */  IDGS_CANNOT_SWITCH_IPV4_IPV6_DE,
/* fr */  IDGS_CANNOT_SWITCH_IPV4_IPV6_FR,
/* it */  IDGS_CANNOT_SWITCH_IPV4_IPV6_IT,
/* nl */  IDGS_CANNOT_SWITCH_IPV4_IPV6_NL,
/* pl */  IDGS_CANNOT_SWITCH_IPV4_IPV6_PL,
/* sv */  IDGS_CANNOT_SWITCH_IPV4_IPV6_SV},
#endif
#endif

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDGS_FFMPEG_CANNOT_OPEN_VSTREAM,
/* de */  IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_DE,
/* fr */  IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_FR,
/* it */  IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_IT,
/* nl */  IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_NL,
/* pl */  IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_PL,
/* sv */  IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_SV},

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDGS_FFMPEG_CANNOT_OPEN_ASTREAM,
/* de */  IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_DE,
/* fr */  IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_FR,
/* it */  IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_IT,
/* nl */  IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_NL,
/* pl */  IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_PL,
/* sv */  IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_SV},

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDGS_FFMPEG_CANNOT_OPEN_S,
/* de */  IDGS_FFMPEG_CANNOT_OPEN_S_DE,
/* fr */  IDGS_FFMPEG_CANNOT_OPEN_S_FR,
/* it */  IDGS_FFMPEG_CANNOT_OPEN_S_IT,
/* nl */  IDGS_FFMPEG_CANNOT_OPEN_S_NL,
/* pl */  IDGS_FFMPEG_CANNOT_OPEN_S_PL,
/* sv */  IDGS_FFMPEG_CANNOT_OPEN_S_SV},

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_USE_PAL_SYNC_FACTOR,
/* de */  IDCLS_USE_PAL_SYNC_FACTOR_DE,
/* fr */  IDCLS_USE_PAL_SYNC_FACTOR_FR,
/* it */  IDCLS_USE_PAL_SYNC_FACTOR_IT,
/* nl */  IDCLS_USE_PAL_SYNC_FACTOR_NL,
/* pl */  IDCLS_USE_PAL_SYNC_FACTOR_PL,
/* sv */  IDCLS_USE_PAL_SYNC_FACTOR_SV},

/* c64/c64io.c */
/* en */ {IDGS_IO_READ_COLL_AT_X_FROM,
/* de */  IDGS_IO_READ_COLL_AT_X_FROM_DE,
/* fr */  IDGS_IO_READ_COLL_AT_X_FROM_FR,
/* it */  IDGS_IO_READ_COLL_AT_X_FROM_IT,
/* nl */  IDGS_IO_READ_COLL_AT_X_FROM_NL,
/* pl */  IDGS_IO_READ_COLL_AT_X_FROM_PL,
/* sv */  IDGS_IO_READ_COLL_AT_X_FROM_SV},

/* c64/c64io.c */
/* en */ {IDGS_AND,
/* de */  IDGS_AND_DE,
/* fr */  IDGS_AND_FR,
/* it */  IDGS_AND_IT,
/* nl */  IDGS_AND_NL,
/* pl */  IDGS_AND_PL,
/* sv */  IDGS_AND_SV},

/* c64/c64io.c */
/* en */ {IDGS_ALL_DEVICES_DETACHED,
/* de */  IDGS_ALL_DEVICES_DETACHED_DE,
/* fr */  IDGS_ALL_DEVICES_DETACHED_FR,
/* it */  IDGS_ALL_DEVICES_DETACHED_IT,
/* nl */  IDGS_ALL_DEVICES_DETACHED_NL,
/* pl */  IDGS_ALL_DEVICES_DETACHED_PL,
/* sv */  IDGS_ALL_DEVICES_DETACHED_SV},

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

/* c128/c128-cmdline-options.c, c128/functionrom.c,
   c64/c64-cmdline-options.c, c64/georam.c,
   c64/ramcart.c, c64/reu.c, c64/c64cart.c,
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

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
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

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
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

/* c64/georam.c, c64/ramcart.c, c64/reu.c */
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

/* pet/petreu.c */
/* en */ {IDCLS_ENABLE_PETREU,
/* de */  IDCLS_ENABLE_PETREU_DE,
/* fr */  IDCLS_ENABLE_PETREU_FR,
/* it */  IDCLS_ENABLE_PETREU_IT,
/* nl */  IDCLS_ENABLE_PETREU_NL,
/* pl */  IDCLS_ENABLE_PETREU_PL,
/* sv */  IDCLS_ENABLE_PETREU_SV},

/* pet/petreu.c */
/* en */ {IDCLS_DISABLE_PETREU,
/* de */  IDCLS_DISABLE_PETREU_DE,
/* fr */  IDCLS_DISABLE_PETREU_FR,
/* it */  IDCLS_DISABLE_PETREU_IT,
/* nl */  IDCLS_DISABLE_PETREU_NL,
/* pl */  IDCLS_DISABLE_PETREU_PL,
/* sv */  IDCLS_DISABLE_PETREU_SV},

/* pet/petreu.c */
/* en */ {IDCLS_SPECIFY_PETREU_NAME,
/* de */  IDCLS_SPECIFY_PETREU_NAME_DE,
/* fr */  IDCLS_SPECIFY_PETREU_NAME_FR,
/* it */  IDCLS_SPECIFY_PETREU_NAME_IT,
/* nl */  IDCLS_SPECIFY_PETREU_NAME_NL,
/* pl */  IDCLS_SPECIFY_PETREU_NAME_PL,
/* sv */  IDCLS_SPECIFY_PETREU_NAME_SV},

/* pet/petreu.c */
/* en */ {IDCLS_PETREU_SIZE,
/* de */  IDCLS_PETREU_SIZE_DE,
/* fr */  IDCLS_PETREU_SIZE_FR,
/* it */  IDCLS_PETREU_SIZE_IT,
/* nl */  IDCLS_PETREU_SIZE_NL,
/* pl */  IDCLS_PETREU_SIZE_PL,
/* sv */  IDCLS_PETREU_SIZE_SV},

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

/* c64/cart/ide64.c */
/* en */ {IDCLS_SPECIFY_IDE64_NAME,
/* de */  IDCLS_SPECIFY_IDE64_NAME_DE,
/* fr */  IDCLS_SPECIFY_IDE64_NAME_FR,
/* it */  IDCLS_SPECIFY_IDE64_NAME_IT,
/* nl */  IDCLS_SPECIFY_IDE64_NAME_NL,
/* pl */  IDCLS_SPECIFY_IDE64_NAME_PL,
/* sv */  IDCLS_SPECIFY_IDE64_NAME_SV},

/* c64/cart/ide64.c, gfxoutputdrv/ffmpegdrv.c, datasette.c,
   debug.c, mouse.c, ram.c, sound.c, vsync.c */
/* en */ {IDCLS_P_VALUE,
/* de */  IDCLS_P_VALUE_DE,
/* fr */  IDCLS_P_VALUE_FR,
/* it */  IDCLS_P_VALUE_IT,
/* nl */  IDCLS_P_VALUE_NL,
/* pl */  IDCLS_P_VALUE_PL,
/* sv */  IDCLS_P_VALUE_SV},

/* c64/cart/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_CYLINDERS_IDE64,
/* de */  IDCLS_SET_AMOUNT_CYLINDERS_IDE64_DE,
/* fr */  IDCLS_SET_AMOUNT_CYLINDERS_IDE64_FR,
/* it */  IDCLS_SET_AMOUNT_CYLINDERS_IDE64_IT,
/* nl */  IDCLS_SET_AMOUNT_CYLINDERS_IDE64_NL,
/* pl */  IDCLS_SET_AMOUNT_CYLINDERS_IDE64_PL,
/* sv */  IDCLS_SET_AMOUNT_CYLINDERS_IDE64_SV},

/* c64/cart/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_HEADS_IDE64,
/* de */  IDCLS_SET_AMOUNT_HEADS_IDE64_DE,
/* fr */  IDCLS_SET_AMOUNT_HEADS_IDE64_FR,
/* it */  IDCLS_SET_AMOUNT_HEADS_IDE64_IT,
/* nl */  IDCLS_SET_AMOUNT_HEADS_IDE64_NL,
/* pl */  IDCLS_SET_AMOUNT_HEADS_IDE64_PL,
/* sv */  IDCLS_SET_AMOUNT_HEADS_IDE64_SV},

/* c64/cart/ide64.c */
/* en */ {IDCLS_SET_AMOUNT_SECTORS_IDE64,
/* de */  IDCLS_SET_AMOUNT_SECTORS_IDE64_DE,
/* fr */  IDCLS_SET_AMOUNT_SECTORS_IDE64_FR,
/* it */  IDCLS_SET_AMOUNT_SECTORS_IDE64_IT,
/* nl */  IDCLS_SET_AMOUNT_SECTORS_IDE64_NL,
/* pl */  IDCLS_SET_AMOUNT_SECTORS_IDE64_PL,
/* sv */  IDCLS_SET_AMOUNT_SECTORS_IDE64_SV},

/* c64/cart/ide64.c */
/* en */ {IDCLS_AUTODETECT_IDE64_GEOMETRY,
/* de */  IDCLS_AUTODETECT_IDE64_GEOMETRY_DE,
/* fr */  IDCLS_AUTODETECT_IDE64_GEOMETRY_FR,
/* it */  IDCLS_AUTODETECT_IDE64_GEOMETRY_IT,
/* nl */  IDCLS_AUTODETECT_IDE64_GEOMETRY_NL,
/* pl */  IDCLS_AUTODETECT_IDE64_GEOMETRY_PL,
/* sv */  IDCLS_AUTODETECT_IDE64_GEOMETRY_SV},

/* c64/cart/ide64.c */
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

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1541_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1541_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1541_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1541_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1541_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1541_DOS_ROM_NAME_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1570_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1570_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1570_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1570_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1570_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1570_DOS_ROM_NAME_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1571_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1571_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1571_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1571_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1571_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1571_DOS_ROM_NAME_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1581_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1581_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1581_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1581_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1581_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1581_DOS_ROM_NAME_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_PAR_CABLE,
/* de */  IDCLS_ENABLE_PAR_CABLE_DE,
/* fr */  IDCLS_ENABLE_PAR_CABLE_FR,
/* it */  IDCLS_ENABLE_PAR_CABLE_IT,
/* nl */  IDCLS_ENABLE_PAR_CABLE_NL,
/* pl */  IDCLS_ENABLE_PAR_CABLE_PL,
/* sv */  IDCLS_ENABLE_PAR_CABLE_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_PAR_CABLE,
/* de */  IDCLS_DISABLE_PAR_CABLE_DE,
/* fr */  IDCLS_DISABLE_PAR_CABLE_FR,
/* it */  IDCLS_DISABLE_PAR_CABLE_IT,
/* nl */  IDCLS_DISABLE_PAR_CABLE_NL,
/* pl */  IDCLS_DISABLE_PAR_CABLE_PL,
/* sv */  IDCLS_DISABLE_PAR_CABLE_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SET_IDLE_METHOD,
/* de */  IDCLS_SET_IDLE_METHOD_DE,
/* fr */  IDCLS_SET_IDLE_METHOD_FR,
/* it */  IDCLS_SET_IDLE_METHOD_IT,
/* nl */  IDCLS_SET_IDLE_METHOD_NL,
/* pl */  IDCLS_SET_IDLE_METHOD_PL,
/* sv */  IDCLS_SET_IDLE_METHOD_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_2000,
/* de */  IDCLS_ENABLE_DRIVE_RAM_2000_DE,
/* fr */  IDCLS_ENABLE_DRIVE_RAM_2000_FR,
/* it */  IDCLS_ENABLE_DRIVE_RAM_2000_IT,
/* nl */  IDCLS_ENABLE_DRIVE_RAM_2000_NL,
/* pl */  IDCLS_ENABLE_DRIVE_RAM_2000_PL,
/* sv */  IDCLS_ENABLE_DRIVE_RAM_2000_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_2000,
/* de */  IDCLS_DISABLE_DRIVE_RAM_2000_DE,
/* fr */  IDCLS_DISABLE_DRIVE_RAM_2000_FR,
/* it */  IDCLS_DISABLE_DRIVE_RAM_2000_IT,
/* nl */  IDCLS_DISABLE_DRIVE_RAM_2000_NL,
/* pl */  IDCLS_DISABLE_DRIVE_RAM_2000_PL,
/* sv */  IDCLS_DISABLE_DRIVE_RAM_2000_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_4000,
/* de */  IDCLS_ENABLE_DRIVE_RAM_4000_DE,
/* fr */  IDCLS_ENABLE_DRIVE_RAM_4000_FR,
/* it */  IDCLS_ENABLE_DRIVE_RAM_4000_IT,
/* nl */  IDCLS_ENABLE_DRIVE_RAM_4000_NL,
/* pl */  IDCLS_ENABLE_DRIVE_RAM_4000_PL,
/* sv */  IDCLS_ENABLE_DRIVE_RAM_4000_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_4000,
/* de */  IDCLS_DISABLE_DRIVE_RAM_4000_DE,
/* fr */  IDCLS_DISABLE_DRIVE_RAM_4000_FR,
/* it */  IDCLS_DISABLE_DRIVE_RAM_4000_IT,
/* nl */  IDCLS_DISABLE_DRIVE_RAM_4000_NL,
/* pl */  IDCLS_DISABLE_DRIVE_RAM_4000_PL,
/* sv */  IDCLS_DISABLE_DRIVE_RAM_4000_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_6000,
/* de */  IDCLS_ENABLE_DRIVE_RAM_6000_DE,
/* fr */  IDCLS_ENABLE_DRIVE_RAM_6000_FR,
/* it */  IDCLS_ENABLE_DRIVE_RAM_6000_IT,
/* nl */  IDCLS_ENABLE_DRIVE_RAM_6000_NL,
/* pl */  IDCLS_ENABLE_DRIVE_RAM_6000_PL,
/* sv */  IDCLS_ENABLE_DRIVE_RAM_6000_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_6000,
/* de */  IDCLS_DISABLE_DRIVE_RAM_6000_DE,
/* fr */  IDCLS_DISABLE_DRIVE_RAM_6000_FR,
/* it */  IDCLS_DISABLE_DRIVE_RAM_6000_IT,
/* nl */  IDCLS_DISABLE_DRIVE_RAM_6000_NL,
/* pl */  IDCLS_DISABLE_DRIVE_RAM_6000_PL,
/* sv */  IDCLS_DISABLE_DRIVE_RAM_6000_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_8000,
/* de */  IDCLS_ENABLE_DRIVE_RAM_8000_DE,
/* fr */  IDCLS_ENABLE_DRIVE_RAM_8000_FR,
/* it */  IDCLS_ENABLE_DRIVE_RAM_8000_IT,
/* nl */  IDCLS_ENABLE_DRIVE_RAM_8000_NL,
/* pl */  IDCLS_ENABLE_DRIVE_RAM_8000_PL,
/* sv */  IDCLS_ENABLE_DRIVE_RAM_8000_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_8000,
/* de */  IDCLS_DISABLE_DRIVE_RAM_8000_DE,
/* fr */  IDCLS_DISABLE_DRIVE_RAM_8000_FR,
/* it */  IDCLS_DISABLE_DRIVE_RAM_8000_IT,
/* nl */  IDCLS_DISABLE_DRIVE_RAM_8000_NL,
/* pl */  IDCLS_DISABLE_DRIVE_RAM_8000_PL,
/* sv */  IDCLS_DISABLE_DRIVE_RAM_8000_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_A000,
/* de */  IDCLS_ENABLE_DRIVE_RAM_A000_DE,
/* fr */  IDCLS_ENABLE_DRIVE_RAM_A000_FR,
/* it */  IDCLS_ENABLE_DRIVE_RAM_A000_IT,
/* nl */  IDCLS_ENABLE_DRIVE_RAM_A000_NL,
/* pl */  IDCLS_ENABLE_DRIVE_RAM_A000_PL,
/* sv */  IDCLS_ENABLE_DRIVE_RAM_A000_SV},

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_A000,
/* de */  IDCLS_DISABLE_DRIVE_RAM_A000_DE,
/* fr */  IDCLS_DISABLE_DRIVE_RAM_A000_FR,
/* it */  IDCLS_DISABLE_DRIVE_RAM_A000_IT,
/* nl */  IDCLS_DISABLE_DRIVE_RAM_A000_NL,
/* pl */  IDCLS_DISABLE_DRIVE_RAM_A000_PL,
/* sv */  IDCLS_DISABLE_DRIVE_RAM_A000_SV},

/* drive/iec128dcr/iec128dcr-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_SV},

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_2031_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_2031_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_2031_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_2031_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_2031_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_2031_DOS_ROM_NAME_SV},

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_2040_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_2040_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_2040_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_2040_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_2040_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_2040_DOS_ROM_NAME_SV},

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_3040_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_3040_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_3040_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_3040_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_3040_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_3040_DOS_ROM_NAME_SV},

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_4040_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_4040_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_4040_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_4040_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_4040_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_4040_DOS_ROM_NAME_SV},

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME,
/* de */  IDCLS_SPECIFY_1001_DOS_ROM_NAME_DE,
/* fr */  IDCLS_SPECIFY_1001_DOS_ROM_NAME_FR,
/* it */  IDCLS_SPECIFY_1001_DOS_ROM_NAME_IT,
/* nl */  IDCLS_SPECIFY_1001_DOS_ROM_NAME_NL,
/* pl */  IDCLS_SPECIFY_1001_DOS_ROM_NAME_PL,
/* sv */  IDCLS_SPECIFY_1001_DOS_ROM_NAME_SV},

/* drive/tcbm/tcbm-cmdline-options.c */
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

/* c64/plus256k.c */
/* en */ {IDCLS_ENABLE_PLUS256K_EXPANSION,
/* de */  IDCLS_ENABLE_PLUS256K_EXPANSION_DE,
/* fr */  IDCLS_ENABLE_PLUS256K_EXPANSION_FR,
/* it */  IDCLS_ENABLE_PLUS256K_EXPANSION_IT,
/* nl */  IDCLS_ENABLE_PLUS256K_EXPANSION_NL,
/* pl */  IDCLS_ENABLE_PLUS256K_EXPANSION_PL,
/* sv */  IDCLS_ENABLE_PLUS256K_EXPANSION_SV},

/* c64/plus256k.c */
/* en */ {IDCLS_DISABLE_PLUS256K_EXPANSION,
/* de */  IDCLS_DISABLE_PLUS256K_EXPANSION_DE,
/* fr */  IDCLS_DISABLE_PLUS256K_EXPANSION_FR,
/* it */  IDCLS_DISABLE_PLUS256K_EXPANSION_IT,
/* nl */  IDCLS_DISABLE_PLUS256K_EXPANSION_NL,
/* pl */  IDCLS_DISABLE_PLUS256K_EXPANSION_PL,
/* sv */  IDCLS_DISABLE_PLUS256K_EXPANSION_SV},

/* c64/plus256k.c */
/* en */ {IDCLS_SPECIFY_PLUS256K_NAME,
/* de */  IDCLS_SPECIFY_PLUS256K_NAME_DE,
/* fr */  IDCLS_SPECIFY_PLUS256K_NAME_FR,
/* it */  IDCLS_SPECIFY_PLUS256K_NAME_IT,
/* nl */  IDCLS_SPECIFY_PLUS256K_NAME_NL,
/* pl */  IDCLS_SPECIFY_PLUS256K_NAME_PL,
/* sv */  IDCLS_SPECIFY_PLUS256K_NAME_SV},

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
/* sv */  IDCLS_SPECIFY_PLUS60K_NAME_SV},

/* c64/plus60k.c */
/* en */ {IDCLS_PLUS60K_BASE,
/* de */  IDCLS_PLUS60K_BASE_DE,
/* fr */  IDCLS_PLUS60K_BASE_FR,
/* it */  IDCLS_PLUS60K_BASE_IT,
/* nl */  IDCLS_PLUS60K_BASE_NL,
/* pl */  IDCLS_PLUS60K_BASE_PL,
/* sv */  IDCLS_PLUS60K_BASE_SV},

/* c64/c64_256k.c */
/* en */ {IDCLS_ENABLE_C64_256K_EXPANSION,
/* de */  IDCLS_ENABLE_C64_256K_EXPANSION_DE,
/* fr */  IDCLS_ENABLE_C64_256K_EXPANSION_FR,
/* it */  IDCLS_ENABLE_C64_256K_EXPANSION_IT,
/* nl */  IDCLS_ENABLE_C64_256K_EXPANSION_NL,
/* pl */  IDCLS_ENABLE_C64_256K_EXPANSION_PL,
/* sv */  IDCLS_ENABLE_C64_256K_EXPANSION_SV},

/* c64/c64_256k.c */
/* en */ {IDCLS_DISABLE_C64_256K_EXPANSION,
/* de */  IDCLS_DISABLE_C64_256K_EXPANSION_DE,
/* fr */  IDCLS_DISABLE_C64_256K_EXPANSION_FR,
/* it */  IDCLS_DISABLE_C64_256K_EXPANSION_IT,
/* nl */  IDCLS_DISABLE_C64_256K_EXPANSION_NL,
/* pl */  IDCLS_DISABLE_C64_256K_EXPANSION_PL,
/* sv */  IDCLS_DISABLE_C64_256K_EXPANSION_SV},

/* c64/c64_256k.c */
/* en */ {IDCLS_SPECIFY_C64_256K_NAME,
/* de */  IDCLS_SPECIFY_C64_256K_NAME_DE,
/* fr */  IDCLS_SPECIFY_C64_256K_NAME_FR,
/* it */  IDCLS_SPECIFY_C64_256K_NAME_IT,
/* nl */  IDCLS_SPECIFY_C64_256K_NAME_NL,
/* pl */  IDCLS_SPECIFY_C64_256K_NAME_PL,
/* sv */  IDCLS_SPECIFY_C64_256K_NAME_SV},

/* c64/c64_256k.c */
/* en */ {IDCLS_C64_256K_BASE,
/* de */  IDCLS_C64_256K_BASE_DE,
/* fr */  IDCLS_C64_256K_BASE_FR,
/* it */  IDCLS_C64_256K_BASE_IT,
/* nl */  IDCLS_C64_256K_BASE_NL,
/* pl */  IDCLS_C64_256K_BASE_PL,
/* sv */  IDCLS_C64_256K_BASE_SV},

/* c64/cart/c64cart.c */
/* en */ {IDCLS_ATTACH_RAW_STB_CART,
/* de */  IDCLS_ATTACH_RAW_STB_CART_DE,
/* fr */  IDCLS_ATTACH_RAW_STB_CART_FR,
/* it */  IDCLS_ATTACH_RAW_STB_CART_IT,
/* nl */  IDCLS_ATTACH_RAW_STB_CART_NL,
/* pl */  IDCLS_ATTACH_RAW_STB_CART_PL,
/* sv */  IDCLS_ATTACH_RAW_STB_CART_SV},

/* plus4/plus4memcsory256k.c */
/* en */ {IDCLS_ENABLE_CS256K_EXPANSION,
/* de */  IDCLS_ENABLE_CS256K_EXPANSION_DE,
/* fr */  IDCLS_ENABLE_CS256K_EXPANSION_FR,
/* it */  IDCLS_ENABLE_CS256K_EXPANSION_IT,
/* nl */  IDCLS_ENABLE_CS256K_EXPANSION_NL,
/* pl */  IDCLS_ENABLE_CS256K_EXPANSION_PL,
/* sv */  IDCLS_ENABLE_CS256K_EXPANSION_SV},

/* plus4/plus4memhannes256k.c */
/* en */ {IDCLS_ENABLE_H256K_EXPANSION,
/* de */  IDCLS_ENABLE_H256K_EXPANSION_DE,
/* fr */  IDCLS_ENABLE_H256K_EXPANSION_FR,
/* it */  IDCLS_ENABLE_H256K_EXPANSION_IT,
/* nl */  IDCLS_ENABLE_H256K_EXPANSION_NL,
/* pl */  IDCLS_ENABLE_H256K_EXPANSION_PL,
/* sv */  IDCLS_ENABLE_H256K_EXPANSION_SV},

/* plus4/plus4memhannes256k.c */
/* en */ {IDCLS_ENABLE_H1024K_EXPANSION,
/* de */  IDCLS_ENABLE_H1024K_EXPANSION_DE,
/* fr */  IDCLS_ENABLE_H1024K_EXPANSION_FR,
/* it */  IDCLS_ENABLE_H1024K_EXPANSION_IT,
/* nl */  IDCLS_ENABLE_H1024K_EXPANSION_NL,
/* pl */  IDCLS_ENABLE_H1024K_EXPANSION_PL,
/* sv */  IDCLS_ENABLE_H1024K_EXPANSION_SV},

/* plus4/plus4memhannes256k.c */
/* en */ {IDCLS_ENABLE_H4096K_EXPANSION,
/* de */  IDCLS_ENABLE_H4096K_EXPANSION_DE,
/* fr */  IDCLS_ENABLE_H4096K_EXPANSION_FR,
/* it */  IDCLS_ENABLE_H4096K_EXPANSION_IT,
/* nl */  IDCLS_ENABLE_H4096K_EXPANSION_NL,
/* pl */  IDCLS_ENABLE_H4096K_EXPANSION_PL,
/* sv */  IDCLS_ENABLE_H4096K_EXPANSION_SV}

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

static int set_current_language(const char *lang, void *param)
{
    int i;

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

