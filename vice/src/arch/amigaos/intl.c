/*
 * intl.c - Localization routines for Amiga.
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

#include "vice.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define __USE_INLINE__

#include <proto/locale.h>

#include "archdep.h"
#include "cmdline.h"
#include "intl.h"
#include "lib.h"
/* #include "res.h" */
#include "resources.h"
#include "translate.h"
#include "util.h"
#include "ui.h"

#define countof(array) (sizeof(array) / sizeof((array)[0]))

typedef struct amiga_iso_s {
    char *amiga_locale_language;
    char *iso_language_code;
} amiga_iso_t;


/* this table holds only the currently present translation
   languages */

static amiga_iso_t amiga_to_iso[]={
  {"english.language", "en"},
  {"german.language", "de"},
  {"french.language", "fr"},
  {"italian.language", "it"},
  {"dutch.language", "nl"},
  {"polish.language", "pl"},
  {"swedish.language", "sv"},
  {0, NULL}
};

/* The language table is duplicated in
   the translate.c, make sure they match
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

/* --------------------------------------------------------------------- */

typedef struct intl_translate_s {
    int resource_id;
    char *text;
} intl_translate_t;



intl_translate_t intl_string_table[] = {

/* ----------------------- AmigaOS Menu Strings ----------------------- */

/* en */ {IDMS_FILE,    "File"},
/* de */ {IDMS_FILE_DE, "Datei"},
/* fr */ {IDMS_FILE_FR, "Fichier"},
/* it */ {IDMS_FILE_IT, "File"},
/* nl */ {IDMS_FILE_NL, "Bestand"},
/* pl */ {IDMS_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FILE_SV, "Arkiv"},

/* en */ {IDMS_AUTOSTART_IMAGE,    "Autostart disk/tape image..."},
/* de */ {IDMS_AUTOSTART_IMAGE_DE, "Autostart Disk/Band Image..."},
/* fr */ {IDMS_AUTOSTART_IMAGE_FR, "Démarrage automatique d'une image datassette..."},
/* it */ {IDMS_AUTOSTART_IMAGE_IT, "Avvia automaticamente l'immagine di un disco/cassetta..."},
/* nl */ {IDMS_AUTOSTART_IMAGE_NL, "Autostart disk/tape bestand..."},
/* pl */ {IDMS_AUTOSTART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_AUTOSTART_IMAGE_SV, "Autostarta disk-/bandavbildningsfil..."},

/* en */ {IDMS_ATTACH_DISK_IMAGE,    "Attach disk image"},
/* de */ {IDMS_ATTACH_DISK_IMAGE_DE, "Disk Image einlegen"},
/* fr */ {IDMS_ATTACH_DISK_IMAGE_FR, "Insérer une image de disque"},
/* it */ {IDMS_ATTACH_DISK_IMAGE_IT, "Seleziona l'immagine di un disco"},
/* nl */ {IDMS_ATTACH_DISK_IMAGE_NL, "Koppel disk bestand"},
/* pl */ {IDMS_ATTACH_DISK_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_DISK_IMAGE_SV, "Anslut diskettavbildningsfil"},

/* en */ {IDMS_DRIVE_8,    "Drive 8"},
/* de */ {IDMS_DRIVE_8_DE, "Laufwerk 8"},
/* fr */ {IDMS_DRIVE_8_FR, "Lecteur #8"},
/* it */ {IDMS_DRIVE_8_IT, "Drive 8"},
/* nl */ {IDMS_DRIVE_8_NL, "Drive 8"},
/* pl */ {IDMS_DRIVE_8_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_8_SV, "Enhet 8"},

/* en */ {IDMS_DRIVE_9,    "Drive 9"},
/* de */ {IDMS_DRIVE_9_DE, "Laufwerk 9"},
/* fr */ {IDMS_DRIVE_9_FR, "Lecteur #9"},
/* it */ {IDMS_DRIVE_9_IT, "Drive 9"},
/* nl */ {IDMS_DRIVE_9_NL, "Drive 9"},
/* pl */ {IDMS_DRIVE_9_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_9_SV, "Enhet 9"},

/* en */ {IDMS_DRIVE_10,    "Drive 10"},
/* de */ {IDMS_DRIVE_10_DE, "Laufwerk 10"},
/* fr */ {IDMS_DRIVE_10_FR, "Lecteur #10"},
/* it */ {IDMS_DRIVE_10_IT, "Drive 10"},
/* nl */ {IDMS_DRIVE_10_NL, "Drive 10"},
/* pl */ {IDMS_DRIVE_10_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_10_SV, "Enhet 10"},

/* en */ {IDMS_DRIVE_11,    "Drive 11"},
/* de */ {IDMS_DRIVE_11_DE, "Laufwerk 11"},
/* fr */ {IDMS_DRIVE_11_FR, "Lecteur #11"},
/* it */ {IDMS_DRIVE_11_IT, "Drive 11"},
/* nl */ {IDMS_DRIVE_11_NL, "Drive 11"},
/* pl */ {IDMS_DRIVE_11_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_11_SV, "Enhet 11"},

/* en */ {IDMS_DETACH_DISK_IMAGE,    "Detach disk image"},
/* de */ {IDMS_DETACH_DISK_IMAGE_DE, "Disk Image entfernen"},
/* fr */ {IDMS_DETACH_DISK_IMAGE_FR, "Retirer une image de disque"},
/* it */ {IDMS_DETACH_DISK_IMAGE_IT, "Rimuovi l'immagine di un disco"},
/* nl */ {IDMS_DETACH_DISK_IMAGE_NL, "Ontkoppel disk bestand"},
/* pl */ {IDMS_DETACH_DISK_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DETACH_DISK_IMAGE_SV, "Koppla från diskettavbildningsfil"},

/* en */ {IDMS_ALL,    "All"},
/* de */ {IDMS_ALL_DE, "Alle"},
/* fr */ {IDMS_ALL_FR, "Tout"},
/* it */ {IDMS_ALL_IT, "Tutti"},
/* nl */ {IDMS_ALL_NL, "Alles"},
/* pl */ {IDMS_ALL_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ALL_SV, "Alla"},

/* en */ {IDMS_FLIP_LIST,    "Flip list"},
/* de */ {IDMS_FLIP_LIST_DE, "Flipliste"},
/* fr */ {IDMS_FLIP_LIST_FR, "Groupement de disques"},
/* it */ {IDMS_FLIP_LIST_IT, "Flip list"},
/* nl */ {IDMS_FLIP_LIST_NL, "Flip lijst"},
/* pl */ {IDMS_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FLIP_LIST_SV, "Vallista"},

/* en */ {IDMS_ADD_CURRENT_IMAGE,    "Add current image"},
/* de */ {IDMS_ADD_CURRENT_IMAGE_DE, "Aktuelles Image hinzufügen"},
/* fr */ {IDMS_ADD_CURRENT_IMAGE_FR, "Ajouter l'image de disque courante"},
/* it */ {IDMS_ADD_CURRENT_IMAGE_IT, "Aggiungi l'immagine attuale"},
/* nl */ {IDMS_ADD_CURRENT_IMAGE_NL, "Voeg huidig bestand toe"},
/* pl */ {IDMS_ADD_CURRENT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ADD_CURRENT_IMAGE_SV, "Lägg till aktuell avbildning"},

/* en */ {IDMS_REMOVE_CURRENT_IMAGE,    "Remove current image"},
/* de */ {IDMS_REMOVE_CURRENT_IMAGE_DE, "Aktuelles Image entfernen"},
/* fr */ {IDMS_REMOVE_CURRENT_IMAGE_FR, "Retirer l'image de disque courante"},
/* it */ {IDMS_REMOVE_CURRENT_IMAGE_IT, "Rimuovi l'immagine attuale"},
/* nl */ {IDMS_REMOVE_CURRENT_IMAGE_NL, "Verwijder huidig bestand"},
/* pl */ {IDMS_REMOVE_CURRENT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_REMOVE_CURRENT_IMAGE_SV, "Ta bort aktuell avbildning"},

/* en */ {IDMS_ATTACH_NEXT_IMAGE,    "Attach next image"},
/* de */ {IDMS_ATTACH_NEXT_IMAGE_DE, "Nächstes Image"},
/* fr */ {IDMS_ATTACH_NEXT_IMAGE_FR, "Insérer la prochaine image"},
/* it */ {IDMS_ATTACH_NEXT_IMAGE_IT, "Seleziona la prossima immagine"},
/* nl */ {IDMS_ATTACH_NEXT_IMAGE_NL, "Koppel volgend bestand"},
/* pl */ {IDMS_ATTACH_NEXT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_NEXT_IMAGE_SV, "Anslut nästa avbildning"},

/* en */ {IDMS_ATTACH_PREVIOUS_IMAGE,    "Attach previous image"},
/* de */ {IDMS_ATTACH_PREVIOUS_IMAGE_DE, "Voriges Image"},
/* fr */ {IDMS_ATTACH_PREVIOUS_IMAGE_FR, "Insérer l'image précédente"},
/* it */ {IDMS_ATTACH_PREVIOUS_IMAGE_IT, "Seleziona l'immagine precedente"},
/* nl */ {IDMS_ATTACH_PREVIOUS_IMAGE_NL, "Koppel vorig bestand"},
/* pl */ {IDMS_ATTACH_PREVIOUS_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_PREVIOUS_IMAGE_SV, "Anslut föregående avbildning"},

/* en */ {IDMS_LOAD_FLIP_LIST,    "Load flip list"},
/* de */ {IDMS_LOAD_FLIP_LIST_DE, "Fliplist Datei Laden"},
/* fr */ {IDMS_LOAD_FLIP_LIST_FR, "Charger un groupement de disques"},
/* it */ {IDMS_LOAD_FLIP_LIST_IT, "Carica fliplist"},
/* nl */ {IDMS_LOAD_FLIP_LIST_NL, "Laad flip lijst"},
/* pl */ {IDMS_LOAD_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_FLIP_LIST_SV, "Läs in vallistefil"},

/* en */ {IDMS_SAVE_FLIP_LIST,    "Save flip list"},
/* de */ {IDMS_SAVE_FLIP_LIST_DE, "Fliplist Datei Speichern"},
/* fr */ {IDMS_SAVE_FLIP_LIST_FR, "Enregistrer le groupement de disques"},
/* it */ {IDMS_SAVE_FLIP_LIST_IT, "Salva fliplist"},
/* nl */ {IDMS_SAVE_FLIP_LIST_NL, "Opslaan flip lijst"},
/* pl */ {IDMS_SAVE_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_FLIP_LIST_SV, "Spara vallistefil"},

/* en */ {IDMS_ATTACH_TAPE_IMAGE,    "Attach tape image..."},
/* de */ {IDMS_ATTACH_TAPE_IMAGE_DE, "Band Image einlegen..."},
/* fr */ {IDMS_ATTACH_TAPE_IMAGE_FR, "Insérer une image datassette..."},
/* it */ {IDMS_ATTACH_TAPE_IMAGE_IT, "Seleziona l'immagine di una cassetta..."},
/* nl */ {IDMS_ATTACH_TAPE_IMAGE_NL, "Koppel tape bestand..."},
/* pl */ {IDMS_ATTACH_TAPE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_TAPE_IMAGE_SV, "Anslut en bandavbildningsfil..."},

/* en */ {IDMS_DETACH_TAPE_IMAGE,    "Detach tape image"},
/* de */ {IDMS_DETACH_TAPE_IMAGE_DE, "Band Image entfernen"},
/* fr */ {IDMS_DETACH_TAPE_IMAGE_FR, "Retirer une image datassette"},
/* it */ {IDMS_DETACH_TAPE_IMAGE_IT, "Rimuovi l'immagine della cassetta"},
/* nl */ {IDMS_DETACH_TAPE_IMAGE_NL, "Ontkoppel tape image"},
/* pl */ {IDMS_DETACH_TAPE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DETACH_TAPE_IMAGE_SV, "Koppla från bandavbildningsfil"},

/* en */ {IDMS_DATASSETTE_CONTROL,    "Datassette control"},
/* de */ {IDMS_DATASSETTE_CONTROL_DE, "Bandlaufwerk Kontrolle"},
/* fr */ {IDMS_DATASSETTE_CONTROL_FR, "Contrôle datassette"},
/* it */ {IDMS_DATASSETTE_CONTROL_IT, "Controllo del registratore"},
/* nl */ {IDMS_DATASSETTE_CONTROL_NL, "Datassette bediening"},
/* pl */ {IDMS_DATASSETTE_CONTROL_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DATASSETTE_CONTROL_SV, "Datasettestyrning"},

/* en */ {IDMS_STOP,    "Stop"},
/* de */ {IDMS_STOP_DE, "Stop"},
/* fr */ {IDMS_STOP_FR, "Arrêt"},
/* it */ {IDMS_STOP_IT, "Stop"},
/* nl */ {IDMS_STOP_NL, "Stop"},
/* pl */ {IDMS_STOP_PL, ""},  /* fuzzy */
/* sv */ {IDMS_STOP_SV, "Stoppa"},

/* en */ {IDMS_START,    "Start"},
/* de */ {IDMS_START_DE, "Start"},
/* fr */ {IDMS_START_FR, "Démarrer"},
/* it */ {IDMS_START_IT, "Inizia"},
/* nl */ {IDMS_START_NL, "Start"},
/* pl */ {IDMS_START_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_SV, "Starta"},

/* en */ {IDMS_FORWARD,    "Forward"},
/* de */ {IDMS_FORWARD_DE, "Forward"},
/* fr */ {IDMS_FORWARD_FR, "En avant"},
/* it */ {IDMS_FORWARD_IT, "Avanti"},
/* nl */ {IDMS_FORWARD_NL, "Vooruit"},
/* pl */ {IDMS_FORWARD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FORWARD_SV, "Spola framåt"},

/* en */ {IDMS_REWIND,    "Rewind"},
/* de */ {IDMS_REWIND_DE, "Rewind"},
/* fr */ {IDMS_REWIND_FR, "En arrière"},
/* it */ {IDMS_REWIND_IT, "Indietro"},
/* nl */ {IDMS_REWIND_NL, "Terug"},
/* pl */ {IDMS_REWIND_PL, ""},  /* fuzzy */
/* sv */ {IDMS_REWIND_SV, "Spola bakåt"},

/* en */ {IDMS_RECORD,    "Record"},
/* de */ {IDMS_RECORD_DE, "Record"},
/* fr */ {IDMS_RECORD_FR, "Enregistrer"},
/* it */ {IDMS_RECORD_IT, "Record"},
/* nl */ {IDMS_RECORD_NL, "Opname"},
/* pl */ {IDMS_RECORD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RECORD_SV, "Spela in"},

/* en */ {IDMS_RESET,    "Reset"},
/* de */ {IDMS_RESET_DE, "Reset"},
/* fr */ {IDMS_RESET_FR, "Réinitialiser"},
/* it */ {IDMS_RESET_IT, "Reset"},
/* nl */ {IDMS_RESET_NL, "Reset"},
/* pl */ {IDMS_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RESET_SV, "Nollställ"},

/* en */ {IDMS_RESET_COUNTER,    "Reset Counter"},
/* de */ {IDMS_RESET_COUNTER_DE, "Zähler zurücksetzen"},
/* fr */ {IDMS_RESET_COUNTER_FR, "Réinialiser le compteur"},
/* it */ {IDMS_RESET_COUNTER_IT, "Reset contantore"},
/* nl */ {IDMS_RESET_COUNTER_NL, "Reset teller"},
/* pl */ {IDMS_RESET_COUNTER_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RESET_COUNTER_SV, "Nollställ räknare"},

/* en */ {IDMS_ATTACH_CART_IMAGE,    "Attach cartridge image..."},
/* de */ {IDMS_ATTACH_CART_IMAGE_DE, "Erweiterungsmodule einlegen..."},
/* fr */ {IDMS_ATTACH_CART_IMAGE_FR, "Insérer une cartouche..."},
/* it */ {IDMS_ATTACH_CART_IMAGE_IT, "Seleziona l'immagine di una cartuccia..."},
/* nl */ {IDMS_ATTACH_CART_IMAGE_NL, "Koppel cartridge bestand..."},
/* pl */ {IDMS_ATTACH_CART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_CART_IMAGE_SV, "Anslut insticksmodulfil..."},

/* en */ {IDMS_4_8_16KB_AT_2000,    "4/8/16KB image at $2000..."},
/* de */ {IDMS_4_8_16KB_AT_2000_DE, "4/8/16KB Modul Image bei $2000..."},
/* fr */ {IDMS_4_8_16KB_AT_2000_FR, "Insertion d'une image 4/8/16Ko à $2000..."},
/* it */ {IDMS_4_8_16KB_AT_2000_IT, "Immagine di 4/8/16KB a $2000..."},
/* nl */ {IDMS_4_8_16KB_AT_2000_NL, "Koppel 4/8/16KB bestand in $2000..."},
/* pl */ {IDMS_4_8_16KB_AT_2000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4_8_16KB_AT_2000_SV, "4/8/16KB-fil vid $2000..."},

/* en */ {IDMS_4_8_16KB_AT_4000,    "4/8/16KB image at $4000..."},
/* de */ {IDMS_4_8_16KB_AT_4000_DE, "4/8/16KB Modul Image bei $4000..."},
/* fr */ {IDMS_4_8_16KB_AT_4000_FR, "Insertion d'une image 4/8/16Ko à $4000..."},
/* it */ {IDMS_4_8_16KB_AT_4000_IT, "Immagine di 4/8/16KB a $4000..."},
/* nl */ {IDMS_4_8_16KB_AT_4000_NL, "Koppel 4/8/16KB bestand in $4000..."},
/* pl */ {IDMS_4_8_16KB_AT_4000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4_8_16KB_AT_4000_SV, "4/8/16KB-fil vid $4000..."},

/* en */ {IDMS_4_8_16KB_AT_6000,    "4/8/16KB image at $6000..."},
/* de */ {IDMS_4_8_16KB_AT_6000_DE, "4/8/16KB Modul Image bei $6000..."},
/* fr */ {IDMS_4_8_16KB_AT_6000_FR, "Insertion d'une image 4/8/16Ko à $6000..."},
/* it */ {IDMS_4_8_16KB_AT_6000_IT, "Immagine di 4/8/16KB a $6000..."},
/* nl */ {IDMS_4_8_16KB_AT_6000_NL, "Koppel 4/8/16KB bestand in $6000..."},
/* pl */ {IDMS_4_8_16KB_AT_6000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4_8_16KB_AT_6000_SV, "4/8/16KB-fil vid $6000..."},

/* en */ {IDMS_4_8KB_AT_A000,    "4/8KB image at $A000..."},
/* de */ {IDMS_4_8KB_AT_A000_DE, "4/8KB Modul Image bei $A000..."},
/* fr */ {IDMS_4_8KB_AT_A000_FR, "Insertion d'une image 4/8Ko à $A000..."},
/* it */ {IDMS_4_8KB_AT_A000_IT, "Immagine di 4/8KB a $A000..."},
/* nl */ {IDMS_4_8KB_AT_A000_NL, "Koppel 4/8KB bestand in $A000..."},
/* pl */ {IDMS_4_8KB_AT_A000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4_8KB_AT_A000_SV, "4/8KB-fil vid $A000..."},

/* en */ {IDMS_4KB_AT_B000,    "4KB image at $B000..."},
/* de */ {IDMS_4KB_AT_B000_DE, "4KB Modul Image bei $B000..."},
/* fr */ {IDMS_4KB_AT_B000_FR, "Insertion d'une image 4Ko à $B000..."},
/* it */ {IDMS_4KB_AT_B000_IT, "Immagine di 4KB a $B000..."},
/* nl */ {IDMS_4KB_AT_B000_NL, "Koppel 4KB bestand in $B000..."},
/* pl */ {IDMS_4KB_AT_B000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4KB_AT_B000_SV, "4KB-fil vid $B000..."},

/* en */ {IDMS_DETACH_CART_IMAGE,    "Detach cartridge image"},
/* de */ {IDMS_DETACH_CART_IMAGE_DE, "Erweiterungsmodul Image entfernen"},
/* fr */ {IDMS_DETACH_CART_IMAGE_FR, "Retirer une cartouche"},
/* it */ {IDMS_DETACH_CART_IMAGE_IT, "Rimuovi l'immagine della cartuccia"},
/* nl */ {IDMS_DETACH_CART_IMAGE_NL, "Ontkoppel cartridge bestand"},
/* pl */ {IDMS_DETACH_CART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DETACH_CART_IMAGE_SV, "Koppla från insticksmodulfil"},

/* en */ {IDMS_C1_LOW_IMAGE,    "C1 low  image..."},
/* de */ {IDMS_C1_LOW_IMAGE_DE, "C1 low Image Datei..."},
/* fr */ {IDMS_C1_LOW_IMAGE_FR, "Image C1 basse..."},
/* it */ {IDMS_C1_LOW_IMAGE_IT, "Immagine C1 bassa..."},
/* nl */ {IDMS_C1_LOW_IMAGE_NL, "C1 low  bestand..."},
/* pl */ {IDMS_C1_LOW_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C1_LOW_IMAGE_SV, "C1 låg-avbildning..."},

/* en */ {IDMS_C1_HIGH_IMAGE,    "C1 high image..."},
/* de */ {IDMS_C1_HIGH_IMAGE_DE, "C1 high Image Datei..."},
/* fr */ {IDMS_C1_HIGH_IMAGE_FR, "Image C1 haute..."},
/* it */ {IDMS_C1_HIGH_IMAGE_IT, "Immagine C1 alta..."},
/* nl */ {IDMS_C1_HIGH_IMAGE_NL, "C1 high bestand..."},
/* pl */ {IDMS_C1_HIGH_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C1_HIGH_IMAGE_SV, "C1 hög-avbildning..."},

/* en */ {IDMS_C2_LOW_IMAGE,    "C2 low  image..."},
/* de */ {IDMS_C2_LOW_IMAGE_DE, "C2 low Image Datei..."},
/* fr */ {IDMS_C2_LOW_IMAGE_FR, "Image C2 basse..."},
/* it */ {IDMS_C2_LOW_IMAGE_IT, "Immagine C2 bassa..."},
/* nl */ {IDMS_C2_LOW_IMAGE_NL, "C2 low  bestand..."},
/* pl */ {IDMS_C2_LOW_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C2_LOW_IMAGE_SV, "C2 låg-avbildning..."},

/* en */ {IDMS_C2_HIGH_IMAGE,    "C2 high image..."},
/* de */ {IDMS_C2_HIGH_IMAGE_DE, "C2 high Image Datei..."},
/* fr */ {IDMS_C2_HIGH_IMAGE_FR, "Image C2 haute..."},
/* it */ {IDMS_C2_HIGH_IMAGE_IT, "Immagine C2 alta..."},
/* nl */ {IDMS_C2_HIGH_IMAGE_NL, "C2 high bestand..."},
/* pl */ {IDMS_C2_HIGH_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C2_HIGH_IMAGE_SV, "C2 hög-avbildning..."},

/* en */ {IDMS_FUNCTION_LOW_3PLUS1,    "Function low  image (3plus1)..."},
/* de */ {IDMS_FUNCTION_LOW_3PLUS1_DE, "Funktions ROM Image Datei low (3plus1)..."},
/* fr */ {IDMS_FUNCTION_LOW_3PLUS1_FR, "Image Fonction basse (3+1)..."},
/* it */ {IDMS_FUNCTION_LOW_3PLUS1_IT, "Immagine della Function low (3plus1)..."},
/* nl */ {IDMS_FUNCTION_LOW_3PLUS1_NL, "Function low  bestand (3plus1)..."},
/* pl */ {IDMS_FUNCTION_LOW_3PLUS1_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FUNCTION_LOW_3PLUS1_SV, "Funktion låg-avbildning (3plus1)..."},

/* en */ {IDMS_FUNCTION_HIGH_3PLUS1,    "Function high image (3plus1)..."},
/* de */ {IDMS_FUNCTION_HIGH_3PLUS1_DE, "Funktions ROM Image Datei high (3plus1)..."},
/* fr */ {IDMS_FUNCTION_HIGH_3PLUS1_FR, "Image Fonction haute (3+1)..."},
/* it */ {IDMS_FUNCTION_HIGH_3PLUS1_IT, "Immagine della Function high (3plus1)..."},
/* nl */ {IDMS_FUNCTION_HIGH_3PLUS1_NL, "Function high bestand (3plus1)..."},
/* pl */ {IDMS_FUNCTION_HIGH_3PLUS1_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FUNCTION_HIGH_3PLUS1_SV, "Funktion hög-avbildning (3plus1)..."},

/* en */ {IDMS_CRT_IMAGE,    "CRT image..."},
/* de */ {IDMS_CRT_IMAGE_DE, "CRT Image Datei..."},
/* fr */ {IDMS_CRT_IMAGE_FR, "Nom du fichier image CRT..."},
/* it */ {IDMS_CRT_IMAGE_IT, "Immagine CRT..."},
/* nl */ {IDMS_CRT_IMAGE_NL, "CRT bestand..."},
/* pl */ {IDMS_CRT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CRT_IMAGE_SV, "CRT-fil..."},

/* en */ {IDMS_GENERIC_8KB_IMAGE,    "Generic 8KB image..."},
/* de */ {IDMS_GENERIC_8KB_IMAGE_DE, "Generische 8Kb Image..."},
/* fr */ {IDMS_GENERIC_8KB_IMAGE_FR, "Insérer une image générique de 8Ko..."},
/* it */ {IDMS_GENERIC_8KB_IMAGE_IT, "Immagine generica di 8KB..."},
/* nl */ {IDMS_GENERIC_8KB_IMAGE_NL, "Algemeen 8KB bestand..."},
/* pl */ {IDMS_GENERIC_8KB_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_GENERIC_8KB_IMAGE_SV, "Vanlig 8KB-fil..."},

/* en */ {IDMS_GENERIC_16KB_IMAGE,    "Generic 16KB image..."},
/* de */ {IDMS_GENERIC_16KB_IMAGE_DE, "Generische 16Kb Image..."},
/* fr */ {IDMS_GENERIC_16KB_IMAGE_FR, "Insérer une image générique de 16Ko..."},
/* it */ {IDMS_GENERIC_16KB_IMAGE_IT, "Immagine generica di 16KB..."},
/* nl */ {IDMS_GENERIC_16KB_IMAGE_NL, "Algemeen 16KB bestand..."},
/* pl */ {IDMS_GENERIC_16KB_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_GENERIC_16KB_IMAGE_SV, "Vanlig 16KB-fil..."},

/* en */ {IDMS_ACTION_REPLAY_IMAGE,    "Action Replay image..."},
/* de */ {IDMS_ACTION_REPLAY_IMAGE_DE, "Action Replay Image..."},
/* fr */ {IDMS_ACTION_REPLAY_IMAGE_FR, "Insérer une cartouche Action Replay..."},
/* it */ {IDMS_ACTION_REPLAY_IMAGE_IT, "Immagine Action Replay..."},
/* nl */ {IDMS_ACTION_REPLAY_IMAGE_NL, "Action Replay bestand"},
/* pl */ {IDMS_ACTION_REPLAY_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ACTION_REPLAY_IMAGE_SV, "Action Replay-fil..."},

/* en */ {IDMS_ATOMIC_POWER_IMAGE,    "Atomic Power image..."},
/* de */ {IDMS_ATOMIC_POWER_IMAGE_DE, "Atomic Power Image..."},
/* fr */ {IDMS_ATOMIC_POWER_IMAGE_FR, "Insérer une cartouche Atomic Power..."},
/* it */ {IDMS_ATOMIC_POWER_IMAGE_IT, "Immagine Atomic Power..."},
/* nl */ {IDMS_ATOMIC_POWER_IMAGE_NL, "Atomic Power bestand..."},
/* pl */ {IDMS_ATOMIC_POWER_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATOMIC_POWER_IMAGE_SV, "Atomic Power-fil..."},

/* en */ {IDMS_EPYX_FASTLOAD_IMAGE,    "Epyx fastload image..."},
/* de */ {IDMS_EPYX_FASTLOAD_IMAGE_DE, "Epyx Fastload Image..."},
/* fr */ {IDMS_EPYX_FASTLOAD_IMAGE_FR, "Insérer une cartouche Epyx FastLoad..."},
/* it */ {IDMS_EPYX_FASTLOAD_IMAGE_IT, "Immagine Epyx fastload..."},
/* nl */ {IDMS_EPYX_FASTLOAD_IMAGE_NL, "Epyx fastload bestand..."},
/* pl */ {IDMS_EPYX_FASTLOAD_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EPYX_FASTLOAD_IMAGE_SV, "Epyx fastload-fil..."},

/* en */ {IDMS_IEEE488_INTERFACE_IMAGE,    "IEEE488 interface image..."},
/* de */ {IDMS_IEEE488_INTERFACE_IMAGE_DE, "IEEE 488 Schnittstellenmodul..."},
/* fr */ {IDMS_IEEE488_INTERFACE_IMAGE_FR, "Insérer une cartouche interface IEEE488..."},
/* it */ {IDMS_IEEE488_INTERFACE_IMAGE_IT, "Immagine dell'interfaccia IEEE488..."},
/* nl */ {IDMS_IEEE488_INTERFACE_IMAGE_NL, "IEEE488 interface bestand..."},
/* pl */ {IDMS_IEEE488_INTERFACE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_IEEE488_INTERFACE_IMAGE_SV, "IEEE488-gränssnittsfil..."},

/* en */ {IDMS_RETRO_REPLAY_IMAGE,    "Retro Replay image..."},
/* de */ {IDMS_RETRO_REPLAY_IMAGE_DE, "Retro Replay Image..."},
/* fr */ {IDMS_RETRO_REPLAY_IMAGE_FR, "Insérer une cartouche Retro Replay..."},
/* it */ {IDMS_RETRO_REPLAY_IMAGE_IT, "Immagine Retro Replay..."},
/* nl */ {IDMS_RETRO_REPLAY_IMAGE_NL, "Retro Replay bestand..."},
/* pl */ {IDMS_RETRO_REPLAY_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RETRO_REPLAY_IMAGE_SV, "Retro Replay-fil..."},

/* en */ {IDMS_IDE64_INTERFACE_IMAGE,    "IDE64 interface image..."},
/* de */ {IDMS_IDE64_INTERFACE_IMAGE_DE, "IDE64 Schnittstellenmodul..."},
/* fr */ {IDMS_IDE64_INTERFACE_IMAGE_FR, "Insérer une cartouche interface IDE64..."},
/* it */ {IDMS_IDE64_INTERFACE_IMAGE_IT, "Immagine dell'interfaccia IDE64..."},
/* nl */ {IDMS_IDE64_INTERFACE_IMAGE_NL, "IDE64 interface bestand..."},
/* pl */ {IDMS_IDE64_INTERFACE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_IDE64_INTERFACE_IMAGE_SV, "IDE64-gränssnittsfil..."},

/* en */ {IDMS_SUPER_SNAPSHOT_4_IMAGE,    "Super Snapshot 4 image..."},
/* de */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_DE, "Super Snapshot 4 Image..."},
/* fr */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_FR, "Insérer une cartouche Super Snapshot 4..."},
/* it */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_IT, "Immagine Super Snapshot 4..."},
/* nl */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_NL, "Super Snapshot 4 bestand..."},
/* pl */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_SV, "Super Snapshot 4-fil..."},

/* en */ {IDMS_SUPER_SNAPSHOT_5_IMAGE,    "Super Snapshot 5 image..."},
/* de */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_DE, "Super Snapshot 5 Image..."},
/* fr */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_FR, "Insérer une cartouche Super Snapshot 5..."},
/* it */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_IT, "Immagine Super Snapshot 5..."},
/* nl */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_NL, "Super Snapshot 5 bestand..."},
/* pl */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_SV, "Super Snapshot 5-fil..."},

/* en */ {IDMS_STRUCTURED_BASIC_IMAGE,    "Structured Basic image..."},
/* de */ {IDMS_STRUCTURED_BASIC_IMAGE_DE, "Structured Basic Image..."},
/* fr */ {IDMS_STRUCTURED_BASIC_IMAGE_FR, "Insérer une cartouche Structured Basic..."},
/* it */ {IDMS_STRUCTURED_BASIC_IMAGE_IT, "Immagine Structured Basic..."},
/* nl */ {IDMS_STRUCTURED_BASIC_IMAGE_NL, "Structured Basic bestand..."},
/* pl */ {IDMS_STRUCTURED_BASIC_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_STRUCTURED_BASIC_IMAGE_SV, "Structured Basic-fil..."},

/* en */ {IDMS_EXPERT_CART,    "Expert Cartridge"},
/* de */ {IDMS_EXPERT_CART_DE, "Expert Cartridge"},
/* fr */ {IDMS_EXPERT_CART_FR, "Activer les paramètres des cartouches experts"},
/* it */ {IDMS_EXPERT_CART_IT, "Expert Cartridge"},
/* nl */ {IDMS_EXPERT_CART_NL, "Expert Cartridge"},
/* pl */ {IDMS_EXPERT_CART_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EXPERT_CART_SV, "Expert Cartridge"},

/* en */ {IDMS_ENABLE,    "Enable"},
/* de */ {IDMS_ENABLE_DE, "Aktivieren"},
/* fr */ {IDMS_ENABLE_FR, "Activer"},
/* it */ {IDMS_ENABLE_IT, "Attivo"},
/* nl */ {IDMS_ENABLE_NL, "Aktiveer"},
/* pl */ {IDMS_ENABLE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ENABLE_SV, "Aktivera"},

/* en */ {IDMS_OFF,    "Off"},
/* de */ {IDMS_OFF_DE, "Aus"},
/* fr */ {IDMS_OFF_FR, "Off"},
/* it */ {IDMS_OFF_IT, "No"},
/* nl */ {IDMS_OFF_NL, "Uit"},
/* pl */ {IDMS_OFF_PL, ""},  /* fuzzy */
/* sv */ {IDMS_OFF_SV, "Av"},

/* en */ {IDMS_PRG,    "Prg"},
/* de */ {IDMS_PRG_DE, "Prg"},
/* fr */ {IDMS_PRG_FR, "Prg"},
/* it */ {IDMS_PRG_IT, "Prg"},
/* nl */ {IDMS_PRG_NL, "Prg"},
/* pl */ {IDMS_PRG_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PRG_SV, "Prg"},

/* en */ {IDMS_ON,    "On"},
/* de */ {IDMS_ON_DE, "Ein"},
/* fr */ {IDMS_ON_FR, "On"},
/* it */ {IDMS_ON_IT, "SÃ¬"},
/* nl */ {IDMS_ON_NL, "Aan"},
/* pl */ {IDMS_ON_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ON_SV, "På"},

/* en */ {IDMS_SET_CART_AS_DEFAULT,    "Set cartridge as default"},
/* de */ {IDMS_SET_CART_AS_DEFAULT_DE, "Aktuelles Erweiterungsmodul als standard aktivieren"},
/* fr */ {IDMS_SET_CART_AS_DEFAULT_FR, "Définir cette cartouche par défaut"},
/* it */ {IDMS_SET_CART_AS_DEFAULT_IT, "Imposta la cartuccia come predefinita"},
/* nl */ {IDMS_SET_CART_AS_DEFAULT_NL, "Zet cartridge als standaard"},
/* pl */ {IDMS_SET_CART_AS_DEFAULT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SET_CART_AS_DEFAULT_SV, "Välj insticksmodul som förval"},

/* en */ {IDMS_RESET_ON_CART_CHANGE,    "Reset on cart change"},
/* de */ {IDMS_RESET_ON_CART_CHANGE_DE, "Reset bei Wechsel von Erweiterungsmodulen"},
/* fr */ {IDMS_RESET_ON_CART_CHANGE_FR, "Réinialiser sur changement de cartouche"},
/* it */ {IDMS_RESET_ON_CART_CHANGE_IT, "Reset al cambio di cartuccia"},
/* nl */ {IDMS_RESET_ON_CART_CHANGE_NL, "Reset bij cart wisseling"},
/* pl */ {IDMS_RESET_ON_CART_CHANGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RESET_ON_CART_CHANGE_SV, "Nollställ vid modulbyte"},

/* en */ {IDMS_CART_FREEZE,    "Cartridge freeze"},
/* de */ {IDMS_CART_FREEZE_DE, "Erweiterungsmodul Freeze"},
/* fr */ {IDMS_CART_FREEZE_FR, "Geler la cartouche"},
/* it */ {IDMS_CART_FREEZE_IT, "Freeze della cartuccia"},
/* nl */ {IDMS_CART_FREEZE_NL, "Cartridge freeze"},
/* pl */ {IDMS_CART_FREEZE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CART_FREEZE_SV, "Frys"},

/* en */ {IDMS_PAUSE,    "Pause"},
/* de */ {IDMS_PAUSE_DE, "Pause"},
/* fr */ {IDMS_PAUSE_FR, "Pause"},
/* it */ {IDMS_PAUSE_IT, "Pausa"},
/* nl */ {IDMS_PAUSE_NL, "Pauze"},
/* pl */ {IDMS_PAUSE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PAUSE_SV, "Paus"},

/* en */ {IDMS_MONITOR,    "Monitor"},
/* de */ {IDMS_MONITOR_DE, "Monitor"},
/* fr */ {IDMS_MONITOR_FR, "Tracer"},
/* it */ {IDMS_MONITOR_IT, "Monitor"},
/* nl */ {IDMS_MONITOR_NL, "Monitor"},
/* pl */ {IDMS_MONITOR_PL, ""},  /* fuzzy */
/* sv */ {IDMS_MONITOR_SV, "Monitor"},

/* en */ {IDMS_HARD,    "Hard"},
/* de */ {IDMS_HARD_DE, "Hart"},
/* fr */ {IDMS_HARD_FR, "Physiquement"},
/* it */ {IDMS_HARD_IT, "Hard"},
/* nl */ {IDMS_HARD_NL, "Hard"},
/* pl */ {IDMS_HARD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_HARD_SV, "Hård"},

/* en */ {IDMS_SOFT,    "Soft"},
/* de */ {IDMS_SOFT_DE, "Soft"},
/* fr */ {IDMS_SOFT_FR, "Logiciellement"},
/* it */ {IDMS_SOFT_IT, "Soft"},
/* nl */ {IDMS_SOFT_NL, "Zacht"},
/* pl */ {IDMS_SOFT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SOFT_SV, "Mjuk"},

/* en */ {IDMS_EXIT,    "Exit"},
/* de */ {IDMS_EXIT_DE, "Exit"},
/* fr */ {IDMS_EXIT_FR, "Quitter"},
/* it */ {IDMS_EXIT_IT, "Esci"},
/* nl */ {IDMS_EXIT_NL, "Afsluiten"},
/* pl */ {IDMS_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EXIT_SV, "Avsluta"},

/* en */ {IDMS_SNAPSHOT,    "Snapshot"},
/* de */ {IDMS_SNAPSHOT_DE, "Snapshot"},
/* fr */ {IDMS_SNAPSHOT_FR, "Sauvegarde"},
/* it */ {IDMS_SNAPSHOT_IT, "Snapshot"},
/* nl */ {IDMS_SNAPSHOT_NL, "Momentopname"},
/* pl */ {IDMS_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SNAPSHOT_SV, "Ögonblicksbild"},

/* en */ {IDMS_LOAD_SNAPSHOT_IMAGE,    "Load snapshot image..."},
/* de */ {IDMS_LOAD_SNAPSHOT_IMAGE_DE, "Snapshot Image Laden..."},
/* fr */ {IDMS_LOAD_SNAPSHOT_IMAGE_FR, "Charger une sauvegarde...."},
/* it */ {IDMS_LOAD_SNAPSHOT_IMAGE_IT, "Carica immagine dello snapshot..."},
/* nl */ {IDMS_LOAD_SNAPSHOT_IMAGE_NL, "Laad momentopname bestand..."},
/* pl */ {IDMS_LOAD_SNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_SNAPSHOT_IMAGE_SV, "Läs in ögonblicksbild..."},

/* en */ {IDMS_SAVE_SNAPSHOT_IMAGE,    "Save snapshot image..."},
/* de */ {IDMS_SAVE_SNAPSHOT_IMAGE_DE, "Snapshot Image Speichern..."},
/* fr */ {IDMS_SAVE_SNAPSHOT_IMAGE_FR, "Enregistrer une sauvegarde..."},
/* it */ {IDMS_SAVE_SNAPSHOT_IMAGE_IT, "Salva immagine dello snapshot..."},
/* nl */ {IDMS_SAVE_SNAPSHOT_IMAGE_NL, "Opslaan momentopname bestand..."},
/* pl */ {IDMS_SAVE_SNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_SNAPSHOT_IMAGE_SV, "Spara ögonblicksbild..."},

/* en */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE,    "Load quicksnapshot image"},
/* de */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_DE, "Schnellladen von Snapshot Image"},
/* fr */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_FR, "Charger la sauvegarde rapide"},
/* it */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_IT, "Carica immagine quicksnapshot"},
/* nl */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_NL, "Snellaad momentopname bestand"},
/* pl */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_SV, "Läs in snabbögonblicksbild"},

/* en */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE,    "Save quicksnapshot image"},
/* de */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_DE, "Schnellspeichern von Snapshot"},
/* fr */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_FR, "Enregistrer la sauvegarde rapide"},
/* it */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_IT, "Salva immagine quicksnapshot"},
/* nl */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_NL, "Snelopslaan momentopname bestand"},
/* pl */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_SV, "Spara snabbögonblicksbildfil"},

/* en */ {IDMS_START_STOP_RECORDING,    "Start/Stop Recording History"},
/* de */ {IDMS_START_STOP_RECORDING_DE, "Start/Stop Aufnahme History"},
/* fr */ {IDMS_START_STOP_RECORDING_FR, "Démarrer/Arrêter l'historique de l'enregistrement"},
/* it */ {IDMS_START_STOP_RECORDING_IT, "Inizia/termina la registrazione della cronologia"},
/* nl */ {IDMS_START_STOP_RECORDING_NL, "Start/Stop Opname Geschiedenis"},
/* pl */ {IDMS_START_STOP_RECORDING_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_STOP_RECORDING_SV, "Börja/sluta spela in händelser"},

/* en */ {IDMS_START_STOP_PLAYBACK,    "Start/Stop Playback History"},
/* de */ {IDMS_START_STOP_PLAYBACK_DE, "Start/Stop Wiedergabe History"},
/* fr */ {IDMS_START_STOP_PLAYBACK_FR, "Démarrer/Arrêter l'historique de la lecture"},
/* it */ {IDMS_START_STOP_PLAYBACK_IT, "Inizia/termina la riproduzione della cronologia"},
/* nl */ {IDMS_START_STOP_PLAYBACK_NL, "Start/Stop Afspeel Geschiedenis"},
/* pl */ {IDMS_START_STOP_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_STOP_PLAYBACK_SV, "Börja/sluta spela upp händelser"},

/* en */ {IDMS_SET_MILESTONE,    "Set Recording Milestone"},
/* de */ {IDMS_SET_MILESTONE_DE, "Setze Aufnahme Meilenstein"},
/* fr */ {IDMS_SET_MILESTONE_FR, "Lever le Signet d'enregistrement"},
/* it */ {IDMS_SET_MILESTONE_IT, "Imposta segnalibro"},
/* nl */ {IDMS_SET_MILESTONE_NL, "Zet Opname Mijlpaal"},
/* pl */ {IDMS_SET_MILESTONE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SET_MILESTONE_SV, "Ange inspelningsbokmärke"},

/* en */ {IDMS_RETURN_TO_MILESTONE,    "Return to Milestone"},
/* de */ {IDMS_RETURN_TO_MILESTONE_DE, "Rückkehr zum Meilenstein"},
/* fr */ {IDMS_RETURN_TO_MILESTONE_FR, "Retourner au Signet d'enregistrement"},
/* it */ {IDMS_RETURN_TO_MILESTONE_IT, "Vai al segnalibro"},
/* nl */ {IDMS_RETURN_TO_MILESTONE_NL, "Ga terug naar Mijlpaal"},
/* pl */ {IDMS_RETURN_TO_MILESTONE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RETURN_TO_MILESTONE_SV, "Återgå till bokmärke"},

/* en */ {IDMS_RECORDING_START_MODE,    "Recording start mode"},
/* de */ {IDMS_RECORDING_START_MODE_DE, "Aufnahme Start Modus"},
/* fr */ {IDMS_RECORDING_START_MODE_FR, "Mode de départ d'enregistrement"},
/* it */ {IDMS_RECORDING_START_MODE_IT, "Modalità di inizio registrazione"},
/* nl */ {IDMS_RECORDING_START_MODE_NL, "Opname start modus"},
/* pl */ {IDMS_RECORDING_START_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RECORDING_START_MODE_SV, "Spelar in startläge"},

/* en */ {IDMS_SAVE_NEW_SNAPSHOT,    "Save new snapshot"},
/* de */ {IDMS_SAVE_NEW_SNAPSHOT_DE, "Speichere neuen Snapshot"},
/* fr */ {IDMS_SAVE_NEW_SNAPSHOT_FR, "Enregistrer une nouvelle sauvegarde"},
/* it */ {IDMS_SAVE_NEW_SNAPSHOT_IT, "Salva nuovo snapshot"},
/* nl */ {IDMS_SAVE_NEW_SNAPSHOT_NL, "Opslaan nieuwe momentopname"},
/* pl */ {IDMS_SAVE_NEW_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_NEW_SNAPSHOT_SV, "Spara ny ögonblicksbild"},

/* en */ {IDMS_LOAD_EXISTING_SNAPSHOT,    "Load existing snapshot"},
/* de */ {IDMS_LOAD_EXISTING_SNAPSHOT_DE, "Lade existierenden Snapshot"},
/* fr */ {IDMS_LOAD_EXISTING_SNAPSHOT_FR, "Charger une sauvegarde"},
/* it */ {IDMS_LOAD_EXISTING_SNAPSHOT_IT, "Carica snapshot esistente"},
/* nl */ {IDMS_LOAD_EXISTING_SNAPSHOT_NL, "Laad bestaande momentopname"},
/* pl */ {IDMS_LOAD_EXISTING_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_EXISTING_SNAPSHOT_SV, "Läs in existerande ögonblicksbild"},

/* en */ {IDMS_START_WITH_RESET,    "Start with reset"},
/* de */ {IDMS_START_WITH_RESET_DE, "Mit Reset starten"},
/* fr */ {IDMS_START_WITH_RESET_FR, "Réinitialiser au démarrage"},
/* it */ {IDMS_START_WITH_RESET_IT, "Inizia con reset"},
/* nl */ {IDMS_START_WITH_RESET_NL, "Start bij reset"},
/* pl */ {IDMS_START_WITH_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_WITH_RESET_SV, "Starta med nollställning"},

/* en */ {IDMS_OVERWRITE_PLAYBACK,    "Overwrite Playback"},
/* de */ {IDMS_OVERWRITE_PLAYBACK_DE, "Laufende Aufnahme überschreiben"},
/* fr */ {IDMS_OVERWRITE_PLAYBACK_FR, "Écraser la lecture actuelle"},
/* it */ {IDMS_OVERWRITE_PLAYBACK_IT, "Sovrascrivi la riproduzione"},
/* nl */ {IDMS_OVERWRITE_PLAYBACK_NL, "Overschrijf weergave"},
/* pl */ {IDMS_OVERWRITE_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDMS_OVERWRITE_PLAYBACK_SV, "Skriv över aktiv uppspelning"},

/* en */ {IDMS_SELECT_HISTORY_DIR,    "Select History files/directory"},
/* de */ {IDMS_SELECT_HISTORY_DIR_DE, "Verzeichnis für Event Aufnahme"},
/* fr */ {IDMS_SELECT_HISTORY_DIR_FR, "Sélectionnez le répertoire des captures"},
/* it */ {IDMS_SELECT_HISTORY_DIR_IT, "Seleziona i file/directory della cronologia"},
/* nl */ {IDMS_SELECT_HISTORY_DIR_NL, "Selecteer Geschiedenis directory"},
/* pl */ {IDMS_SELECT_HISTORY_DIR_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SELECT_HISTORY_DIR_SV, "Välj historikfiler/-katalog"},

/* en */ {IDMS_SAVE_STOP_MEDIA_FILE,    "Save/stop media file..."},
/* de */ {IDMS_SAVE_STOP_MEDIA_FILE_DE, "Speichern/stop media Datei..."},
/* fr */ {IDMS_SAVE_STOP_MEDIA_FILE_FR, "Enregistrer/Arrêter fichier média..."},
/* it */ {IDMS_SAVE_STOP_MEDIA_FILE_IT, "Salva/arresta file multimediale..."},
/* nl */ {IDMS_SAVE_STOP_MEDIA_FILE_NL, "Opslaan/stop media bestand..."},
/* pl */ {IDMS_SAVE_STOP_MEDIA_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_STOP_MEDIA_FILE_SV, "Starta/stoppa mediafil..."},

/* en */ {IDMS_NETPLAY,    "Netplay (experimental)..."},
/* de */ {IDMS_NETPLAY_DE, ""},  /* fuzzy */
/* fr */ {IDMS_NETPLAY_FR, ""},  /* fuzzy */
/* it */ {IDMS_NETPLAY_IT, ""},  /* fuzzy */
/* nl */ {IDMS_NETPLAY_NL, "Netplay (experimenteel)..."},
/* pl */ {IDMS_NETPLAY_PL, ""},  /* fuzzy */
/* sv */ {IDMS_NETPLAY_SV, ""},  /* fuzzy */

/* en */ {IDMS_OPTIONS,    "Options"},
/* de */ {IDMS_OPTIONS_DE, "Optionen"},
/* fr */ {IDMS_OPTIONS_FR, "Options"},
/* it */ {IDMS_OPTIONS_IT, "Opzioni"},
/* nl */ {IDMS_OPTIONS_NL, "Opties"},
/* pl */ {IDMS_OPTIONS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_OPTIONS_SV, "Alternativ"},

/* en */ {IDMS_REFRESH_RATE,    "Refresh rate"},
/* de */ {IDMS_REFRESH_RATE_DE, "Wiederholrate"},
/* fr */ {IDMS_REFRESH_RATE_FR, "Taux de rafraîchissement"},
/* it */ {IDMS_REFRESH_RATE_IT, "Velocità di aggiornamento"},
/* nl */ {IDMS_REFRESH_RATE_NL, "Vernieuwings snelheid"},
/* pl */ {IDMS_REFRESH_RATE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_REFRESH_RATE_SV, "Uppdateringshastighet"},

/* en */ {IDMS_AUTO,    "Auto"},
/* de */ {IDMS_AUTO_DE, "Automatisch"},
/* fr */ {IDMS_AUTO_FR, "Automatique"},
/* it */ {IDMS_AUTO_IT, "Auto"},
/* nl */ {IDMS_AUTO_NL, "Auto"},
/* pl */ {IDMS_AUTO_PL, ""},  /* fuzzy */
/* sv */ {IDMS_AUTO_SV, "Auto"},

/* en */ {IDMS_1_1,    "1/1"},
/* de */ {IDMS_1_1_DE, "1/1"},
/* fr */ {IDMS_1_1_FR, "1/1"},
/* it */ {IDMS_1_1_IT, "1/1"},
/* nl */ {IDMS_1_1_NL, "1/1"},
/* pl */ {IDMS_1_1_PL, "1/1"},
/* sv */ {IDMS_1_1_SV, "1/1"},

/* en */ {IDMS_1_2,    "1/2"},
/* de */ {IDMS_1_2_DE, "1/2"},
/* fr */ {IDMS_1_2_FR, "1/2"},
/* it */ {IDMS_1_2_IT, "1/2"},
/* nl */ {IDMS_1_2_NL, "1/2"},
/* pl */ {IDMS_1_2_PL, "1/2"},
/* sv */ {IDMS_1_2_SV, "1/2"},

/* en */ {IDMS_1_3,    "1/3"},
/* de */ {IDMS_1_3_DE, "1/3"},
/* fr */ {IDMS_1_3_FR, "1/3"},
/* it */ {IDMS_1_3_IT, "1/3"},
/* nl */ {IDMS_1_3_NL, "1/3"},
/* pl */ {IDMS_1_3_PL, "1/3"},
/* sv */ {IDMS_1_3_SV, "1/3"},

/* en */ {IDMS_1_4,    "1/4"},
/* de */ {IDMS_1_4_DE, "1/4"},
/* fr */ {IDMS_1_4_FR, "1/4"},
/* it */ {IDMS_1_4_IT, "1/4"},
/* nl */ {IDMS_1_4_NL, "1/4"},
/* pl */ {IDMS_1_4_PL, "1/4"},
/* sv */ {IDMS_1_4_SV, "1/4"},

/* en */ {IDMS_1_5,    "1/5"},
/* de */ {IDMS_1_5_DE, "1/5"},
/* fr */ {IDMS_1_5_FR, "1/5"},
/* it */ {IDMS_1_5_IT, "1/5"},
/* nl */ {IDMS_1_5_NL, "1/5"},
/* pl */ {IDMS_1_5_PL, "1/5"},
/* sv */ {IDMS_1_5_SV, "1/5"},

/* en */ {IDMS_1_6,    "1/6"},
/* de */ {IDMS_1_6_DE, "1/6"},
/* fr */ {IDMS_1_6_FR, "1/6"},
/* it */ {IDMS_1_6_IT, "1/6"},
/* nl */ {IDMS_1_6_NL, "1/6"},
/* pl */ {IDMS_1_6_PL, "1/6"},
/* sv */ {IDMS_1_6_SV, "1/6"},

/* en */ {IDMS_1_7,    "1/7"},
/* de */ {IDMS_1_7_DE, "1/7"},
/* fr */ {IDMS_1_7_FR, "1/7"},
/* it */ {IDMS_1_7_IT, "1/7"},
/* nl */ {IDMS_1_7_NL, "1/7"},
/* pl */ {IDMS_1_7_PL, "1/7"},
/* sv */ {IDMS_1_7_SV, "1/7"},

/* en */ {IDMS_1_8,    "1/8"},
/* de */ {IDMS_1_8_DE, "1/8"},
/* fr */ {IDMS_1_8_FR, "1/8"},
/* it */ {IDMS_1_8_IT, "1/8"},
/* nl */ {IDMS_1_8_NL, "1/8"},
/* pl */ {IDMS_1_8_PL, "1/8"},
/* sv */ {IDMS_1_8_SV, "1/8"},

/* en */ {IDMS_1_9,    "1/9"},
/* de */ {IDMS_1_9_DE, "1/9"},
/* fr */ {IDMS_1_9_FR, "1/9"},
/* it */ {IDMS_1_9_IT, "1/9"},
/* nl */ {IDMS_1_9_NL, "1/9"},
/* pl */ {IDMS_1_9_PL, "1/9"},
/* sv */ {IDMS_1_9_SV, "1/9"},

/* en */ {IDMS_1_10,    "1/10"},
/* de */ {IDMS_1_10_DE, "1/10"},
/* fr */ {IDMS_1_10_FR, "1/10"},
/* it */ {IDMS_1_10_IT, "1/10"},
/* nl */ {IDMS_1_10_NL, "1/10"},
/* pl */ {IDMS_1_10_PL, "1/10"},
/* sv */ {IDMS_1_10_SV, "1/10"},

/* en */ {IDMS_MAXIMUM_SPEED,    "Maximum Speed"},
/* de */ {IDMS_MAXIMUM_SPEED_DE, "Maximale Geschwindigkeit"},
/* fr */ {IDMS_MAXIMUM_SPEED_FR, "Vitesse Maximale"},
/* it */ {IDMS_MAXIMUM_SPEED_IT, "Velocità massima"},
/* nl */ {IDMS_MAXIMUM_SPEED_NL, "Maximale Snelheid"},
/* pl */ {IDMS_MAXIMUM_SPEED_PL, ""},  /* fuzzy */
/* sv */ {IDMS_MAXIMUM_SPEED_SV, "Maximal hastighet"},

/* en */ {IDMS_200_PERCENT,    "200%"},
/* de */ {IDMS_200_PERCENT_DE, "200%"},
/* fr */ {IDMS_200_PERCENT_FR, "200%"},
/* it */ {IDMS_200_PERCENT_IT, "200%"},
/* nl */ {IDMS_200_PERCENT_NL, "200%"},
/* pl */ {IDMS_200_PERCENT_PL, "200%"},
/* sv */ {IDMS_200_PERCENT_SV, "200%"},

/* en */ {IDMS_100_PERCENT,    "100%"},
/* de */ {IDMS_100_PERCENT_DE, "100%"},
/* fr */ {IDMS_100_PERCENT_FR, "100%"},
/* it */ {IDMS_100_PERCENT_IT, "100%"},
/* nl */ {IDMS_100_PERCENT_NL, "100%"},
/* pl */ {IDMS_100_PERCENT_PL, "100%"},
/* sv */ {IDMS_100_PERCENT_SV, "100%"},

/* en */ {IDMS_50_PERCENT,    "50%"},
/* de */ {IDMS_50_PERCENT_DE, "50%"},
/* fr */ {IDMS_50_PERCENT_FR, "50%"},
/* it */ {IDMS_50_PERCENT_IT, "50%"},
/* nl */ {IDMS_50_PERCENT_NL, "50%"},
/* pl */ {IDMS_50_PERCENT_PL, "50%"},
/* sv */ {IDMS_50_PERCENT_SV, "50%"},

/* en */ {IDMS_20_PERCENT,    "20%"},
/* de */ {IDMS_20_PERCENT_DE, "20%"},
/* fr */ {IDMS_20_PERCENT_FR, "20%"},
/* it */ {IDMS_20_PERCENT_IT, "20%"},
/* nl */ {IDMS_20_PERCENT_NL, "20%"},
/* pl */ {IDMS_20_PERCENT_PL, "20%"},
/* sv */ {IDMS_20_PERCENT_SV, "20%"},

/* en */ {IDMS_10_PERCENT,    "10%"},
/* de */ {IDMS_10_PERCENT_DE, "10%"},
/* fr */ {IDMS_10_PERCENT_FR, "10%"},
/* it */ {IDMS_10_PERCENT_IT, "10%"},
/* nl */ {IDMS_10_PERCENT_NL, "10%"},
/* pl */ {IDMS_10_PERCENT_PL, "10%"},
/* sv */ {IDMS_10_PERCENT_SV, "10%"},

/* en */ {IDMS_NO_LIMIT,    "No limit"},
/* de */ {IDMS_NO_LIMIT_DE, "Kein Limit"},
/* fr */ {IDMS_NO_LIMIT_FR, "Aucune limite"},
/* it */ {IDMS_NO_LIMIT_IT, "Nessun limite"},
/* nl */ {IDMS_NO_LIMIT_NL, "Geen limiet"},
/* pl */ {IDMS_NO_LIMIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_NO_LIMIT_SV, "Ingen gräns"},

/* en */ {IDMS_CUSTOM,    "Custom"},
/* de */ {IDMS_CUSTOM_DE, "Benutzerdefiniert"},
/* fr */ {IDMS_CUSTOM_FR, "Personnalisé"},
/* it */ {IDMS_CUSTOM_IT, "Personalizzata"},
/* nl */ {IDMS_CUSTOM_NL, "Eigen"},
/* pl */ {IDMS_CUSTOM_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CUSTOM_SV, "Egen"},

/* en */ {IDMS_WARP_MODE,    "Warp mode"},
/* de */ {IDMS_WARP_MODE_DE, "Warp modus"},
/* fr */ {IDMS_WARP_MODE_FR, "Mode turbo"},
/* it */ {IDMS_WARP_MODE_IT, "Modalità turbo"},
/* nl */ {IDMS_WARP_MODE_NL, "Warp modus"},
/* pl */ {IDMS_WARP_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_WARP_MODE_SV, "Warpläge"},

/* en */ {IDMS_FULLSCREEN,    "Fullscreen"},
/* de */ {IDMS_FULLSCREEN_DE, "Vollbild"},
/* fr */ {IDMS_FULLSCREEN_FR, "Périphérique plein écran"},
/* it */ {IDMS_FULLSCREEN_IT, "A tutto schermo"},
/* nl */ {IDMS_FULLSCREEN_NL, "Volscherm"},
/* pl */ {IDMS_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FULLSCREEN_SV, "Fullskärm"},

/* en */ {IDMS_VIDEO_CACHE,    "Video cache"},
/* de */ {IDMS_VIDEO_CACHE_DE, "Video cache"},
/* fr */ {IDMS_VIDEO_CACHE_FR, "Cache vidéo"},
/* it */ {IDMS_VIDEO_CACHE_IT, "Cache video"},
/* nl */ {IDMS_VIDEO_CACHE_NL, "Video cache"},
/* pl */ {IDMS_VIDEO_CACHE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIDEO_CACHE_SV, "Grafikcache"},

/* en */ {IDMS_DOUBLE_SIZE,    "Double size"},
/* de */ {IDMS_DOUBLE_SIZE_DE, "Doppelte Größe"},
/* fr */ {IDMS_DOUBLE_SIZE_FR, "Taille double"},
/* it */ {IDMS_DOUBLE_SIZE_IT, "Dimensione doppia"},
/* nl */ {IDMS_DOUBLE_SIZE_NL, "Dubbele grootte"},
/* pl */ {IDMS_DOUBLE_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DOUBLE_SIZE_SV, "Dubbel storlek"},

/* en */ {IDMS_DOUBLE_SCAN,    "Double scan"},
/* de */ {IDMS_DOUBLE_SCAN_DE, "Doppelt Scan"},
/* fr */ {IDMS_DOUBLE_SCAN_FR, "Double scan"},
/* it */ {IDMS_DOUBLE_SCAN_IT, "Scansione doppia"},
/* nl */ {IDMS_DOUBLE_SCAN_NL, "Dubbele scan"},
/* pl */ {IDMS_DOUBLE_SCAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DOUBLE_SCAN_SV, "Dubbelskanning"},

/* en */ {IDMS_PAL_EMULATION,    "PAL Emulation"},
/* de */ {IDMS_PAL_EMULATION_DE, "PAL Emulation"},
/* fr */ {IDMS_PAL_EMULATION_FR, "Ã‰mulation PAL"},
/* it */ {IDMS_PAL_EMULATION_IT, "Emulazione PAL"},
/* nl */ {IDMS_PAL_EMULATION_NL, "PAL Emulatie"},
/* pl */ {IDMS_PAL_EMULATION_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PAL_EMULATION_SV, "PAL-emulering"},

/* en */ {IDMS_SCALE2X,    "Scale2x"},
/* de */ {IDMS_SCALE2X_DE, "Scale2x"},
/* fr */ {IDMS_SCALE2X_FR, "Scale2x"},
/* it */ {IDMS_SCALE2X_IT, "Scale2x"},
/* nl */ {IDMS_SCALE2X_NL, "Schaal2x"},
/* pl */ {IDMS_SCALE2X_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SCALE2X_SV, "Scale2x"},

/* en */ {IDMS_VDC_SETTINGS,    "VDC settings"},
/* de */ {IDMS_VDC_SETTINGS_DE, "VDC Einstellungen"},
/* fr */ {IDMS_VDC_SETTINGS_FR, "Paramètres VDC"},
/* it */ {IDMS_VDC_SETTINGS_IT, "Impostazioni del VDC"},
/* nl */ {IDMS_VDC_SETTINGS_NL, "VDC instellingen"},
/* pl */ {IDMS_VDC_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VDC_SETTINGS_SV, "VDC-inställningar"},

/* en */ {IDMS_64KB_VIDEO_MEMORY,    "64KB video memory"},
/* de */ {IDMS_64KB_VIDEO_MEMORY_DE, "64KB Bildschirm Speicher"},
/* fr */ {IDMS_64KB_VIDEO_MEMORY_FR, "Mémoire d'affichage de 64Ko"},
/* it */ {IDMS_64KB_VIDEO_MEMORY_IT, "64KB di memoria video"},
/* nl */ {IDMS_64KB_VIDEO_MEMORY_NL, "64KB video geheugen"},
/* pl */ {IDMS_64KB_VIDEO_MEMORY_PL, ""},  /* fuzzy */
/* sv */ {IDMS_64KB_VIDEO_MEMORY_SV, "64KB grafikminne"},

/* en */ {IDMS_SWAP_JOYSTICKS,    "Swap Joysticks"},
/* de */ {IDMS_SWAP_JOYSTICKS_DE, "Joystick ports austauschen"},
/* fr */ {IDMS_SWAP_JOYSTICKS_FR, "Interchanger les joysticks"},
/* it */ {IDMS_SWAP_JOYSTICKS_IT, "Scambia joystick"},
/* nl */ {IDMS_SWAP_JOYSTICKS_NL, "Verwissel Joysticks"},
/* pl */ {IDMS_SWAP_JOYSTICKS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SWAP_JOYSTICKS_SV, "Växla spelportarna"},

/* en */ {IDMS_SOUND_PLAYBACK,    "Sound playback"},
/* de */ {IDMS_SOUND_PLAYBACK_DE, "Sound Einschalten"},
/* fr */ {IDMS_SOUND_PLAYBACK_FR, "Lecture audio"},
/* it */ {IDMS_SOUND_PLAYBACK_IT, "Riproduzione del suono"},
/* nl */ {IDMS_SOUND_PLAYBACK_NL, "Geluidsuitvoer"},
/* pl */ {IDMS_SOUND_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SOUND_PLAYBACK_SV, "Aktivera ljudåtergivning"},

/* en */ {IDMS_TRUE_DRIVE_EMU,    "True drive emulation"},
/* de */ {IDMS_TRUE_DRIVE_EMU_DE, "Präzise Floppy Emulation"},
/* fr */ {IDMS_TRUE_DRIVE_EMU_FR, "Activer l'émulation réelle des lecteurs"},
/* it */ {IDMS_TRUE_DRIVE_EMU_IT, "Attiva l'emulazione hardware dei drive"},
/* nl */ {IDMS_TRUE_DRIVE_EMU_NL, "Hardware drive emulatie"},
/* pl */ {IDMS_TRUE_DRIVE_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDMS_TRUE_DRIVE_EMU_SV, "Äkta diskettenhetsemulering"},

/* en */ {IDMS_VIRTUAL_DEVICE_TRAPS,    "Virtual device traps"},
/* de */ {IDMS_VIRTUAL_DEVICE_TRAPS_DE, "Virtuelle Geräte Traps"},
/* fr */ {IDMS_VIRTUAL_DEVICE_TRAPS_FR, "Activer les périphériques virtuels"},
/* it */ {IDMS_VIRTUAL_DEVICE_TRAPS_IT, ""},  /* fuzzy */
/* nl */ {IDMS_VIRTUAL_DEVICE_TRAPS_NL, "Virtuele apparaat traps"},
/* pl */ {IDMS_VIRTUAL_DEVICE_TRAPS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIRTUAL_DEVICE_TRAPS_SV, "Virtuella enhetstrap:ar"},

/* en */ {IDMS_DRIVE_SYNC_FACTOR,    "Drive sync factor"},
/* de */ {IDMS_DRIVE_SYNC_FACTOR_DE, "Laufwerkt Sync Faktor"},
/* fr */ {IDMS_DRIVE_SYNC_FACTOR_FR, "Facteur de synchro du lecteur"},
/* it */ {IDMS_DRIVE_SYNC_FACTOR_IT, "Fattore di sincronizzazione del drive"},
/* nl */ {IDMS_DRIVE_SYNC_FACTOR_NL, "Drive synchronisatie faktor"},
/* pl */ {IDMS_DRIVE_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_SYNC_FACTOR_SV, "Synkfaktor för diskettstation"},

/* en */ {IDMS_PAL,    "PAL"},
/* de */ {IDMS_PAL_DE, "PAL"},
/* fr */ {IDMS_PAL_FR, "PAL"},
/* it */ {IDMS_PAL_IT, "PAL"},
/* nl */ {IDMS_PAL_NL, "PAL"},
/* pl */ {IDMS_PAL_PL, "PAL"},
/* sv */ {IDMS_PAL_SV, "PAL"},

/* en */ {IDMS_NTSC,    "NTSC"},
/* de */ {IDMS_NTSC_DE, "NTSC"},
/* fr */ {IDMS_NTSC_FR, "NTSC"},
/* it */ {IDMS_NTSC_IT, "NTSC"},
/* nl */ {IDMS_NTSC_NL, "NTSC"},
/* pl */ {IDMS_NTSC_PL, "NTSC"},
/* sv */ {IDMS_NTSC_SV, "NTSC"},

/* en */ {IDMS_VIDEO_STANDARD,    "Video standard"},
/* de */ {IDMS_VIDEO_STANDARD_DE, "Video standard"},
/* fr */ {IDMS_VIDEO_STANDARD_FR, "Standard vidéo"},
/* it */ {IDMS_VIDEO_STANDARD_IT, "Standard video"},
/* nl */ {IDMS_VIDEO_STANDARD_NL, "Video standaard"},
/* pl */ {IDMS_VIDEO_STANDARD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIDEO_STANDARD_SV, "TV-standard"},

/* en */ {IDMS_PAL_G,    "PAL-G"},
/* de */ {IDMS_PAL_G_DE, "PAL-G"},
/* fr */ {IDMS_PAL_G_FR, "PAL-G"},
/* it */ {IDMS_PAL_G_IT, "PAL-G"},
/* nl */ {IDMS_PAL_G_NL, "PAL-G"},
/* pl */ {IDMS_PAL_G_PL, "PAL-G"},
/* sv */ {IDMS_PAL_G_SV, "PAL-G"},

/* en */ {IDMS_NTSC_M,    "NTSC-M"},
/* de */ {IDMS_NTSC_M_DE, "NTSC-M"},
/* fr */ {IDMS_NTSC_M_FR, "NTSC-M"},
/* it */ {IDMS_NTSC_M_IT, "NTSC-M"},
/* nl */ {IDMS_NTSC_M_NL, "NTSC-M"},
/* pl */ {IDMS_NTSC_M_PL, "NTSC-M"},
/* sv */ {IDMS_NTSC_M_SV, "NTSC-M"},

/* en */ {IDMS_OLD_NTSC_M,    "Old NTSC-M"},
/* de */ {IDMS_OLD_NTSC_M_DE, "NTSC-M alt"},
/* fr */ {IDMS_OLD_NTSC_M_FR, "Ancien NTSC-M"},
/* it */ {IDMS_OLD_NTSC_M_IT, "NTSC-M vecchio"},
/* nl */ {IDMS_OLD_NTSC_M_NL, "Oud NTSC-M"},
/* pl */ {IDMS_OLD_NTSC_M_PL, ""},  /* fuzzy */
/* sv */ {IDMS_OLD_NTSC_M_SV, "Gammal NTSC-M"},

/* en */ {IDMS_EMU_ID,    "Emulator Identification"},
/* de */ {IDMS_EMU_ID_DE, "Emulator Identifikation"},
/* fr */ {IDMS_EMU_ID_FR, "Identification de l'émulateur"},
/* it */ {IDMS_EMU_ID_IT, "Identificazione dell'emulatore"},
/* nl */ {IDMS_EMU_ID_NL, "Emulator Identificatie"},
/* pl */ {IDMS_EMU_ID_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EMU_ID_SV, "Emulatoridentifiering"},

/* en */ {IDMS_VIC_1112_IEEE_488,    "VIC-1112 IEEE 488 module"},
/* de */ {IDMS_VIC_1112_IEEE_488_DE, "VIC-1112 IEEE 488 Modul"},
/* fr */ {IDMS_VIC_1112_IEEE_488_FR, "Module VIC-1112 IEEE 488"},
/* it */ {IDMS_VIC_1112_IEEE_488_IT, "Modulo VIC-1112 IEEE 488"},
/* nl */ {IDMS_VIC_1112_IEEE_488_NL, "VIC-1112 IEEE 488 module"},
/* pl */ {IDMS_VIC_1112_IEEE_488_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIC_1112_IEEE_488_SV, "VIC-1112 IEEE 488-modul"},

/* en */ {IDMS_IEEE_488_INTERFACE,    "IEEE 488 Interface emulation"},
/* de */ {IDMS_IEEE_488_INTERFACE_DE, "IEEE488 Schnittstellen-Emulation"},
/* fr */ {IDMS_IEEE_488_INTERFACE_FR, "Interface d'émulation IEEE488"},
/* it */ {IDMS_IEEE_488_INTERFACE_IT, "Emulazione dell'interfaccia IEEE488"},
/* nl */ {IDMS_IEEE_488_INTERFACE_NL, "IEEE488 Interface emulatie"},
/* pl */ {IDMS_IEEE_488_INTERFACE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_IEEE_488_INTERFACE_SV, "Emulering av IEEE488-gränssnitt"},

/* en */ {IDMS_1351_MOUSE,    "1351 mouse emulation"},
/* de */ {IDMS_1351_MOUSE_DE, "1351 Mausemulation"},
/* fr */ {IDMS_1351_MOUSE_FR, "Émulation de la souris 1351"},
/* it */ {IDMS_1351_MOUSE_IT, "Emulazione del mouse 1351"},
/* nl */ {IDMS_1351_MOUSE_NL, "1351 muis emulatie"},
/* pl */ {IDMS_1351_MOUSE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_1351_MOUSE_SV, "1351-musemulering"},

/* en */ {IDMS_SETTINGS,    "Settings"},
/* de */ {IDMS_SETTINGS_DE, "Einstellungen"},
/* fr */ {IDMS_SETTINGS_FR, "Paramètres"},
/* it */ {IDMS_SETTINGS_IT, "Impostazioni"},
/* nl */ {IDMS_SETTINGS_NL, "Instellingen"},
/* pl */ {IDMS_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SETTINGS_SV, "Inställningar"},

/* en */ {IDMS_VIDEO_SETTINGS,    "Video settings..."},
/* de */ {IDMS_VIDEO_SETTINGS_DE, "Video Einstellungen..."},
/* fr */ {IDMS_VIDEO_SETTINGS_FR, "Paramètres vidéo..."},
/* it */ {IDMS_VIDEO_SETTINGS_IT, "Impostazioni video..."},
/* nl */ {IDMS_VIDEO_SETTINGS_NL, "Video instellingen..."},
/* pl */ {IDMS_VIDEO_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIDEO_SETTINGS_SV, "Grafikinställningar..."},

/* en */ {IDMS_VIC_SETTINGS,    "VIC settings..."},
/* de */ {IDMS_VIC_SETTINGS_DE, "VIC Einstellungen..."},
/* fr */ {IDMS_VIC_SETTINGS_FR, "Paramètres VIC..."},
/* it */ {IDMS_VIC_SETTINGS_IT, "Impostazioni del VIC..."},
/* nl */ {IDMS_VIC_SETTINGS_NL, "VIC instellingen..."},
/* pl */ {IDMS_VIC_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIC_SETTINGS_SV, "VIC-inställningar..."},

/* en */ {IDMS_CBM2_SETTINGS,    "CBM2 settings..."},
/* de */ {IDMS_CBM2_SETTINGS_DE, "CBM2 Einstellungen..."},
/* fr */ {IDMS_CBM2_SETTINGS_FR, "Paramètres CBM2..."},
/* it */ {IDMS_CBM2_SETTINGS_IT, "Impostazioni del CBM2..."},
/* nl */ {IDMS_CBM2_SETTINGS_NL, "CBM2 instellingen..."},
/* pl */ {IDMS_CBM2_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CBM2_SETTINGS_SV, "CBM2-inställningar..."},

/* en */ {IDMS_PET_SETTINGS,    "PET settings..."},
/* de */ {IDMS_PET_SETTINGS_DE, "PET Einstellungen..."},
/* fr */ {IDMS_PET_SETTINGS_FR, "Paramètres PET..."},
/* it */ {IDMS_PET_SETTINGS_IT, "Impostazioni del PET..."},
/* nl */ {IDMS_PET_SETTINGS_NL, "PET instellingen..."},
/* pl */ {IDMS_PET_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PET_SETTINGS_SV, "PET-inställningar..."},

/* en */ {IDMS_PERIPHERAL_SETTINGS,    "Peripheral settings..."},
/* de */ {IDMS_PERIPHERAL_SETTINGS_DE, "Peripherie Einstellungen..."},
/* fr */ {IDMS_PERIPHERAL_SETTINGS_FR, "Paramètres des périphériques..."},
/* it */ {IDMS_PERIPHERAL_SETTINGS_IT, "Impostazioni delle periferiche..."},
/* nl */ {IDMS_PERIPHERAL_SETTINGS_NL, "Rand Apparaten instellingen..."},
/* pl */ {IDMS_PERIPHERAL_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PERIPHERAL_SETTINGS_SV, "Inställningar för kringutrustning..."},

/* en */ {IDMS_DRIVE_SETTINGS,    "Drive settings..."},
/* de */ {IDMS_DRIVE_SETTINGS_DE, "Floppy Einstellungen..."},
/* fr */ {IDMS_DRIVE_SETTINGS_FR, "Paramètres des lecteurs..."},
/* it */ {IDMS_DRIVE_SETTINGS_IT, "Impostazioni dei drive..."},
/* nl */ {IDMS_DRIVE_SETTINGS_NL, "Drive instellingen..."},
/* pl */ {IDMS_DRIVE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_SETTINGS_SV, "Diskettenhetsinställningar..."},

/* en */ {IDMS_DATASETTE_SETTINGS,    "Datasette settings..."},
/* de */ {IDMS_DATASETTE_SETTINGS_DE, "Bandlaufwerk Einstellungen..."},
/* fr */ {IDMS_DATASETTE_SETTINGS_FR, "Paramètres du datassette..."},
/* it */ {IDMS_DATASETTE_SETTINGS_IT, "Impostazioni del registratore..."},
/* nl */ {IDMS_DATASETTE_SETTINGS_NL, "Datasette instellingen..."},
/* pl */ {IDMS_DATASETTE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DATASETTE_SETTINGS_SV, "Datasetteinställningar..."},

/* en */ {IDMS_PLUS4_SETTINGS,    "Plus4 settings..."},
/* de */ {IDMS_PLUS4_SETTINGS_DE, "Plus4 Einstellungen..."},
/* fr */ {IDMS_PLUS4_SETTINGS_FR, "Paramètres Plus4..."},
/* it */ {IDMS_PLUS4_SETTINGS_IT, "Impostazioni del Plus4..."},
/* nl */ {IDMS_PLUS4_SETTINGS_NL, "Plus4 instellingen"},
/* pl */ {IDMS_PLUS4_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PLUS4_SETTINGS_SV, "Plus4-inställningar..."},

/* en */ {IDMS_VICII_SETTINGS,    "VIC-II settings..."},
/* de */ {IDMS_VICII_SETTINGS_DE, "VIC-II Einstellungen..."},
/* fr */ {IDMS_VICII_SETTINGS_FR, "Paramètres VIC-II..."},
/* it */ {IDMS_VICII_SETTINGS_IT, "Impostazioni del VIC-II..."},
/* nl */ {IDMS_VICII_SETTINGS_NL, "VIC-II instellingen..."},
/* pl */ {IDMS_VICII_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VICII_SETTINGS_SV, "VIC-II-inställningar..."},

/* en */ {IDMS_JOYSTICK_SETTINGS,    "Joystick settings..."},
/* de */ {IDMS_JOYSTICK_SETTINGS_DE, "Joystick Einstellungen..."},
/* fr */ {IDMS_JOYSTICK_SETTINGS_FR, "Paramètres des joysticks..."},
/* it */ {IDMS_JOYSTICK_SETTINGS_IT, "Impostazioni dei joystick..."},
/* nl */ {IDMS_JOYSTICK_SETTINGS_NL, "Joystick instellingen..."},
/* pl */ {IDMS_JOYSTICK_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_JOYSTICK_SETTINGS_SV, "Inställningar för styrspak..."},

/* en */ {IDMS_JOYSTICK_DEVICE_SELECT,    "Joystick device selection"},
/* de */ {IDMS_JOYSTICK_DEVICE_SELECT_DE, ""},  /* fuzzy */
/* fr */ {IDMS_JOYSTICK_DEVICE_SELECT_FR, ""},  /* fuzzy */
/* it */ {IDMS_JOYSTICK_DEVICE_SELECT_IT, ""},  /* fuzzy */
/* nl */ {IDMS_JOYSTICK_DEVICE_SELECT_NL, "Joystick apparaat selectie"},
/* pl */ {IDMS_JOYSTICK_DEVICE_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_JOYSTICK_DEVICE_SELECT_SV, ""},  /* fuzzy */

/* en */ {IDMS_JOYSTICK_FIRE_SELECT,    "Joystick fire button selection"},
/* de */ {IDMS_JOYSTICK_FIRE_SELECT_DE, ""},  /* fuzzy */
/* fr */ {IDMS_JOYSTICK_FIRE_SELECT_FR, ""},  /* fuzzy */
/* it */ {IDMS_JOYSTICK_FIRE_SELECT_IT, ""},  /* fuzzy */
/* nl */ {IDMS_JOYSTICK_FIRE_SELECT_NL, "Joystick vuur knop selectie"},
/* pl */ {IDMS_JOYSTICK_FIRE_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_JOYSTICK_FIRE_SELECT_SV, ""},  /* fuzzy */

/* en */ {IDMS_KEYBOARD_SETTINGS,    "Keyboard settings..."},
/* de */ {IDMS_KEYBOARD_SETTINGS_DE, "Tastatur Einstellungen..."},
/* fr */ {IDMS_KEYBOARD_SETTINGS_FR, "Paramètres du clavier..."},
/* it */ {IDMS_KEYBOARD_SETTINGS_IT, "Impostazioni della tastiera..."},
/* nl */ {IDMS_KEYBOARD_SETTINGS_NL, "Toetsenbord instellingen..."},
/* pl */ {IDMS_KEYBOARD_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_KEYBOARD_SETTINGS_SV, "Tangentbordsinställningar..."},

/* en */ {IDMS_SOUND_SETTINGS,    "Sound settings..."},
/* de */ {IDMS_SOUND_SETTINGS_DE, "Sound Einstellungen..."},
/* fr */ {IDMS_SOUND_SETTINGS_FR, "Paramètres son..."},
/* it */ {IDMS_SOUND_SETTINGS_IT, "Impostazioni del suono..."},
/* nl */ {IDMS_SOUND_SETTINGS_NL, "Geluid instellingen..."},
/* pl */ {IDMS_SOUND_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SOUND_SETTINGS_SV, "Ljudinställningar..."},

/* en */ {IDMS_SID_SETTINGS,    "SID settings..."},
/* de */ {IDMS_SID_SETTINGS_DE, "SID Einstellungen..."},
/* fr */ {IDMS_SID_SETTINGS_FR, "Paramètres SID..."},
/* it */ {IDMS_SID_SETTINGS_IT, "Impostazioni del SID..."},
/* nl */ {IDMS_SID_SETTINGS_NL, "SID instellingen..."},
/* pl */ {IDMS_SID_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SID_SETTINGS_SV, "SID-inställningar..."},

/* en */ {IDMS_ROM_SETTINGS,    "ROM settings..."},
/* de */ {IDMS_ROM_SETTINGS_DE, "ROM Einstellungen..."},
/* fr */ {IDMS_ROM_SETTINGS_FR, "Paramètres ROM..."},
/* it */ {IDMS_ROM_SETTINGS_IT, "Impostazioni delle ROM..."},
/* nl */ {IDMS_ROM_SETTINGS_NL, "ROM instellingen..."},
/* pl */ {IDMS_ROM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ROM_SETTINGS_SV, "ROM-inställningar..."},

/* en */ {IDMS_RAM_SETTINGS,    "RAM settings..."},
/* de */ {IDMS_RAM_SETTINGS_DE, "RAM Einstellungen..."},
/* fr */ {IDMS_RAM_SETTINGS_FR, "Paramètres de la RAM..."},
/* it */ {IDMS_RAM_SETTINGS_IT, "Impostazioni delle RAM..."},
/* nl */ {IDMS_RAM_SETTINGS_NL, "RAM instellingen..."},
/* pl */ {IDMS_RAM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RAM_SETTINGS_SV, "RAM-inställningar..."},

/* en */ {IDMS_RS232_SETTINGS,    "RS232 settings..."},
/* de */ {IDMS_RS232_SETTINGS_DE, "RS232 Einstellungen..."},
/* fr */ {IDMS_RS232_SETTINGS_FR, "Paramètres RS232..."},
/* it */ {IDMS_RS232_SETTINGS_IT, "Impostazioni della RS232..."},
/* nl */ {IDMS_RS232_SETTINGS_NL, "RS232 instellingen..."},
/* pl */ {IDMS_RS232_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RS232_SETTINGS_SV, "RS232-inställningar..."},

/* en */ {IDMS_C128_SETTINGS,    "C128 settings..."},
/* de */ {IDMS_C128_SETTINGS_DE, "C128 Einstellungen..."},
/* fr */ {IDMS_C128_SETTINGS_FR, "Paramètres C128..."},
/* it */ {IDMS_C128_SETTINGS_IT, "Impostazioni del C128..."},
/* nl */ {IDMS_C128_SETTINGS_NL, "C128 instellingen..."},
/* pl */ {IDMS_C128_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C128_SETTINGS_SV, "C128-inställningar..."},

/* en */ {IDMS_CART_IO_SETTINGS,    "Cartridge/IO settings"},
/* de */ {IDMS_CART_IO_SETTINGS_DE, "Erweiterungsmodul Einstellungen"},
/* fr */ {IDMS_CART_IO_SETTINGS_FR, "Paramètres E/S cartouche"},
/* it */ {IDMS_CART_IO_SETTINGS_IT, "Impostazioni della cartuccia/IO"},
/* nl */ {IDMS_CART_IO_SETTINGS_NL, "Cartridge Instellingen"},
/* pl */ {IDMS_CART_IO_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CART_IO_SETTINGS_SV, "Inställningar för insticksmoduler..."},

/* en */ {IDMS_REU_SETTINGS,    "REU settings..."},
/* de */ {IDMS_REU_SETTINGS_DE, "REU Einstellungen..."},
/* fr */ {IDMS_REU_SETTINGS_FR, "Paramètres REU..."},
/* it */ {IDMS_REU_SETTINGS_IT, "Impostazioni del REU..."},
/* nl */ {IDMS_REU_SETTINGS_NL, "REU instellingen..."},
/* pl */ {IDMS_REU_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_REU_SETTINGS_SV, "REU-inställningar..."},

/* en */ {IDMS_GEORAM_SETTINGS,    "GEORAM settings..."},
/* de */ {IDMS_GEORAM_SETTINGS_DE, "GEORAM Einstellungen..."},
/* fr */ {IDMS_GEORAM_SETTINGS_FR, "Paramètres GEORAM..."},
/* it */ {IDMS_GEORAM_SETTINGS_IT, "Impostazioni della GEORAM..."},
/* nl */ {IDMS_GEORAM_SETTINGS_NL, "GEORAM instellingen..."},
/* pl */ {IDMS_GEORAM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_GEORAM_SETTINGS_SV, "GEORAM-inställningar..."},

/* en */ {IDMS_RAMCART_SETTINGS,    "RAMCART settings..."},
/* de */ {IDMS_RAMCART_SETTINGS_DE, "RAMCART Einstellungen..."},
/* fr */ {IDMS_RAMCART_SETTINGS_FR, "Paramètres RAMCART..."},
/* it */ {IDMS_RAMCART_SETTINGS_IT, "Impostazioni del RAMCART..."},
/* nl */ {IDMS_RAMCART_SETTINGS_NL, "RAMCART instellingen..."},
/* pl */ {IDMS_RAMCART_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RAMCART_SETTINGS_SV, "RAMCART-inställningar..."},

/* en */ {IDMS_PLUS60K_SETTINGS,    "PLUS60K settings..."},
/* de */ {IDMS_PLUS60K_SETTINGS_DE, "PLUS60K Einstellungen..."},
/* fr */ {IDMS_PLUS60K_SETTINGS_FR, "Paramètres PLUS60K..."},
/* it */ {IDMS_PLUS60K_SETTINGS_IT, "Impostazioni del PLUS60K..."},
/* nl */ {IDMS_PLUS60K_SETTINGS_NL, "PLUS60K instellingen..."},
/* pl */ {IDMS_PLUS60K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PLUS60K_SETTINGS_SV, "PLUS60K-inställningar..."},

/* en */ {IDMS_PLUS256K_SETTINGS,    "PLUS256K settings..."},
/* de */ {IDMS_PLUS256K_SETTINGS_DE, "PLUS256K Einstellungen..."},
/* fr */ {IDMS_PLUS256K_SETTINGS_FR, "Paramètres PLUS256K..."},
/* it */ {IDMS_PLUS256K_SETTINGS_IT, "Impostazioni del PLUS256K..."},
/* nl */ {IDMS_PLUS256K_SETTINGS_NL, "PLUS256K instellingen..."},
/* pl */ {IDMS_PLUS256K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PLUS256K_SETTINGS_SV, "PLUS256K-inställningar..."},

/* en */ {IDMS_256K_SETTINGS,    "256K settings..."},
/* de */ {IDMS_256K_SETTINGS_DE, "256K Einstellungen..."},
/* fr */ {IDMS_256K_SETTINGS_FR, "Paramètres 256K..."},
/* it */ {IDMS_256K_SETTINGS_IT, "Impostazioni del 256K..."},
/* nl */ {IDMS_256K_SETTINGS_NL, "256K instellingen..."},
/* pl */ {IDMS_256K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_256K_SETTINGS_SV, "256K-inställningar..."},

/* en */ {IDMS_IDE64_SETTINGS,    "IDE64 settings..."},
/* de */ {IDMS_IDE64_SETTINGS_DE, "IDE64 Einstellungen..."},
/* fr */ {IDMS_IDE64_SETTINGS_FR, "Paramètres IDE64..."},
/* it */ {IDMS_IDE64_SETTINGS_IT, "Impostazioni dell'IDE64..."},
/* nl */ {IDMS_IDE64_SETTINGS_NL, "IDE64 instellingen..."},
/* pl */ {IDMS_IDE64_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_IDE64_SETTINGS_SV, "IDE64-inställningar..."},

/* en */ {IDMS_ETHERNET_SETTINGS,    "Ethernet settings..."},
/* de */ {IDMS_ETHERNET_SETTINGS_DE, "Ethernet Einstellungen..."},
/* fr */ {IDMS_ETHERNET_SETTINGS_FR, "Émulation Ethernet..."},
/* it */ {IDMS_ETHERNET_SETTINGS_IT, "Impostazioni dell'Ethernet..."},
/* nl */ {IDMS_ETHERNET_SETTINGS_NL, "Ethernet instellingen..."},
/* pl */ {IDMS_ETHERNET_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ETHERNET_SETTINGS_SV, "Ethernetinställningar..."},

/* en */ {IDMS_ACIA_SETTINGS,    "ACIA settings..."},
/* de */ {IDMS_ACIA_SETTINGS_DE, "ACIA Einstellungen..."},
/* fr */ {IDMS_ACIA_SETTINGS_FR, "Paramètres ACIA..."},
/* it */ {IDMS_ACIA_SETTINGS_IT, "Impostazioni dell'ACIA..."},
/* nl */ {IDMS_ACIA_SETTINGS_NL, "ACIA instellingen..."},
/* pl */ {IDMS_ACIA_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ACIA_SETTINGS_SV, "ACIA-inställningar..."},

/* en */ {IDMS_PETREU_SETTINGS,    "PET REU settings..."},
/* de */ {IDMS_PETREU_SETTINGS_DE, "PET REU Einstellungen..."},
/* fr */ {IDMS_PETREU_SETTINGS_FR, "Paramètres PET REU..."},
/* it */ {IDMS_PETREU_SETTINGS_IT, "Impostazioni del PET REU..."},
/* nl */ {IDMS_PETREU_SETTINGS_NL, "PET REU instellingen"},
/* pl */ {IDMS_PETREU_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PETREU_SETTINGS_SV, "PET REU-inställningar..."},

/* en */ {IDMS_RS232_USERPORT_SETTINGS,    "RS232 userport settings..."},
/* de */ {IDMS_RS232_USERPORT_SETTINGS_DE, "RS232 Userport Einstellungen..."},
/* fr */ {IDMS_RS232_USERPORT_SETTINGS_FR, "Paramètres RS232 userport..."},
/* it */ {IDMS_RS232_USERPORT_SETTINGS_IT, "Impostazioni della RS232 su userport..."},
/* nl */ {IDMS_RS232_USERPORT_SETTINGS_NL, "RS232 userport instellingen..."},
/* pl */ {IDMS_RS232_USERPORT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RS232_USERPORT_SETTINGS_SV, "RS232-användarportinställningar..."},

/* en */ {IDMS_SAVE_CURRENT_SETTINGS,    "Save current settings"},
/* de */ {IDMS_SAVE_CURRENT_SETTINGS_DE, "Einstellungen speichern"},
/* fr */ {IDMS_SAVE_CURRENT_SETTINGS_FR, "Enregistrer les paramètres courants"},
/* it */ {IDMS_SAVE_CURRENT_SETTINGS_IT, "Salva le impostazioni attuali"},
/* nl */ {IDMS_SAVE_CURRENT_SETTINGS_NL, "Huidige instellingen opslaan"},
/* pl */ {IDMS_SAVE_CURRENT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_CURRENT_SETTINGS_SV, "Spara nuvarande inställningar"},

/* en */ {IDMS_LOAD_SAVED_SETTINGS,    "Load saved settings"},
/* de */ {IDMS_LOAD_SAVED_SETTINGS_DE, "Einstellungen Laden"},
/* fr */ {IDMS_LOAD_SAVED_SETTINGS_FR, "Charger les paramètres"},
/* it */ {IDMS_LOAD_SAVED_SETTINGS_IT, "Carica le impostazioni salvate"},
/* nl */ {IDMS_LOAD_SAVED_SETTINGS_NL, "Opgeslagen instelling laden"},
/* pl */ {IDMS_LOAD_SAVED_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_SAVED_SETTINGS_SV, "Läs sparade inställningar"},

/* en */ {IDMS_SET_DEFAULT_SETTINGS,    "Set default settings"},
/* de */ {IDMS_SET_DEFAULT_SETTINGS_DE, "Wiederherstellen Standard Einstellungen"},
/* fr */ {IDMS_SET_DEFAULT_SETTINGS_FR, "Rétablir les paramètres par défaut"},
/* it */ {IDMS_SET_DEFAULT_SETTINGS_IT, "Ripristina le impostazioni originarie"},
/* nl */ {IDMS_SET_DEFAULT_SETTINGS_NL, "Herstel standaard instellingen"},
/* pl */ {IDMS_SET_DEFAULT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SET_DEFAULT_SETTINGS_SV, "Återställ förvalda inställningar"},

/* en */ {IDMS_SAVE_SETTING_ON_EXIT,    "Save settings on exit"},
/* de */ {IDMS_SAVE_SETTING_ON_EXIT_DE, "Einstellungen beim Beenden speichern"},
/* fr */ {IDMS_SAVE_SETTING_ON_EXIT_FR, "Enregistrer les paramètres à la sortie"},
/* it */ {IDMS_SAVE_SETTING_ON_EXIT_IT, "Salva le impostazioni in uscita"},
/* nl */ {IDMS_SAVE_SETTING_ON_EXIT_NL, "Sla instellingen op bij afsluiten"},
/* pl */ {IDMS_SAVE_SETTING_ON_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_SETTING_ON_EXIT_SV, "Spara inställningar vid avslut"},

/* en */ {IDMS_CONFIRM_ON_EXIT,    "Confirm on exit"},
/* de */ {IDMS_CONFIRM_ON_EXIT_DE, "Bestätigung beim Beenden"},
/* fr */ {IDMS_CONFIRM_ON_EXIT_FR, "Confirmation à la sortie"},
/* it */ {IDMS_CONFIRM_ON_EXIT_IT, "Conferma all'uscita"},
/* nl */ {IDMS_CONFIRM_ON_EXIT_NL, "Bevestigen bij afsluiten"},
/* pl */ {IDMS_CONFIRM_ON_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CONFIRM_ON_EXIT_SV, "Bekräfta avslutning"},

/* en */ {IDMS_HELP,    "Help"},
/* de */ {IDMS_HELP_DE, "Hilfe"},
/* fr */ {IDMS_HELP_FR, "Aide"},
/* it */ {IDMS_HELP_IT, "Aiuto"},
/* nl */ {IDMS_HELP_NL, "Help"},
/* pl */ {IDMS_HELP_PL, ""},  /* fuzzy */
/* sv */ {IDMS_HELP_SV, "Hjälp"},

/* en */ {IDMS_ABOUT,    "About..."},
/* de */ {IDMS_ABOUT_DE, "Über VICE..."},
/* fr */ {IDMS_ABOUT_FR, "À Propos..."},
/* it */ {IDMS_ABOUT_IT, "Informazioni su VICE..."},
/* nl */ {IDMS_ABOUT_NL, "Over VICE..."},
/* pl */ {IDMS_ABOUT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ABOUT_SV, "Om VICE..."},

/* en */ {IDMS_COMMAND_LINE_OPTIONS,    "Command line options"},
/* de */ {IDMS_COMMAND_LINE_OPTIONS_DE, "Kommandozeilen Optionen"},
/* fr */ {IDMS_COMMAND_LINE_OPTIONS_FR, "Options de ligne de commande"},
/* it */ {IDMS_COMMAND_LINE_OPTIONS_IT, "Opzioni della riga di comando"},
/* nl */ {IDMS_COMMAND_LINE_OPTIONS_NL, "Commando invoer opties"},
/* pl */ {IDMS_COMMAND_LINE_OPTIONS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_COMMAND_LINE_OPTIONS_SV, "Kommandoradsflaggor"},

/* en */ {IDMS_CONTRIBUTORS,    "Contributors"},
/* de */ {IDMS_CONTRIBUTORS_DE, "VICE Hackers"},
/* fr */ {IDMS_CONTRIBUTORS_FR, "Contributeurs"},
/* it */ {IDMS_CONTRIBUTORS_IT, "Collaboratori"},
/* nl */ {IDMS_CONTRIBUTORS_NL, "Medewerkers aan het VICE project"},
/* pl */ {IDMS_CONTRIBUTORS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CONTRIBUTORS_SV, "Bidragslämnare"},

/* en */ {IDMS_LICENSE,    "License"},
/* de */ {IDMS_LICENSE_DE, "Lizenz"},
/* fr */ {IDMS_LICENSE_FR, "License"},
/* it */ {IDMS_LICENSE_IT, "Licenza"},
/* nl */ {IDMS_LICENSE_NL, "Licensie"},
/* pl */ {IDMS_LICENSE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LICENSE_SV, "Licens"},

/* en */ {IDMS_NO_WARRANTY,    "No warranty"},
/* de */ {IDMS_NO_WARRANTY_DE, "Keine Garantie!"},
/* fr */ {IDMS_NO_WARRANTY_FR, "Aucune garantie!"},
/* it */ {IDMS_NO_WARRANTY_IT, "Nessuna garanzia"},
/* nl */ {IDMS_NO_WARRANTY_NL, "Geen garantie"},
/* pl */ {IDMS_NO_WARRANTY_PL, ""},  /* fuzzy */
/* sv */ {IDMS_NO_WARRANTY_SV, "Ingen garanti"},

/* en */ {IDMS_LANGUAGE_ENGLISH,    "English"},
/* de */ {IDMS_LANGUAGE_ENGLISH_DE, "Englisch"},
/* fr */ {IDMS_LANGUAGE_ENGLISH_FR, "Anglais"},
/* it */ {IDMS_LANGUAGE_ENGLISH_IT, "Inglese"},
/* nl */ {IDMS_LANGUAGE_ENGLISH_NL, "Engels"},
/* pl */ {IDMS_LANGUAGE_ENGLISH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_ENGLISH_SV, "Engelska"},

/* en */ {IDMS_LANGUAGE_GERMAN,    "German"},
/* de */ {IDMS_LANGUAGE_GERMAN_DE, "Deutsch"},
/* fr */ {IDMS_LANGUAGE_GERMAN_FR, "Allemand"},
/* it */ {IDMS_LANGUAGE_GERMAN_IT, "Tedesco"},
/* nl */ {IDMS_LANGUAGE_GERMAN_NL, "Duits"},
/* pl */ {IDMS_LANGUAGE_GERMAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_GERMAN_SV, "Tyska"},

/* en */ {IDMS_LANGUAGE_FRENCH,    "French"},
/* de */ {IDMS_LANGUAGE_FRENCH_DE, "Französisch"},
/* fr */ {IDMS_LANGUAGE_FRENCH_FR, "Français"},
/* it */ {IDMS_LANGUAGE_FRENCH_IT, "Francese"},
/* nl */ {IDMS_LANGUAGE_FRENCH_NL, "Frans"},
/* pl */ {IDMS_LANGUAGE_FRENCH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_FRENCH_SV, "Franska"},

/* en */ {IDMS_LANGUAGE_ITALIAN,    "Italian"},
/* de */ {IDMS_LANGUAGE_ITALIAN_DE, "Italienisch"},
/* fr */ {IDMS_LANGUAGE_ITALIAN_FR, "Italien"},
/* it */ {IDMS_LANGUAGE_ITALIAN_IT, "Italiano"},
/* nl */ {IDMS_LANGUAGE_ITALIAN_NL, "Italiaans"},
/* pl */ {IDMS_LANGUAGE_ITALIAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_ITALIAN_SV, "Italiense"},

/* en */ {IDMS_LANGUAGE_DUTCH,    "Dutch"},
/* de */ {IDMS_LANGUAGE_DUTCH_DE, "Holländisch"},
/* fr */ {IDMS_LANGUAGE_DUTCH_FR, "Hollandais"},
/* it */ {IDMS_LANGUAGE_DUTCH_IT, "Olandese"},
/* nl */ {IDMS_LANGUAGE_DUTCH_NL, "Nederlands"},
/* pl */ {IDMS_LANGUAGE_DUTCH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_DUTCH_SV, "Nederländska"},

/* en */ {IDMS_LANGUAGE_POLISH,    "Polish"},
/* de */ {IDMS_LANGUAGE_POLISH_DE, "Polnisch"},
/* fr */ {IDMS_LANGUAGE_POLISH_FR, "Polonais"},
/* it */ {IDMS_LANGUAGE_POLISH_IT, "Polacco"},
/* nl */ {IDMS_LANGUAGE_POLISH_NL, "Pools"},
/* pl */ {IDMS_LANGUAGE_POLISH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_POLISH_SV, "Polska"},

/* en */ {IDMS_LANGUAGE_SWEDISH,    "Swedish"},
/* de */ {IDMS_LANGUAGE_SWEDISH_DE, "Schwedisch"},
/* fr */ {IDMS_LANGUAGE_SWEDISH_FR, "Suédois"},
/* it */ {IDMS_LANGUAGE_SWEDISH_IT, "Svedese"},
/* nl */ {IDMS_LANGUAGE_SWEDISH_NL, "Zweeds"},
/* pl */ {IDMS_LANGUAGE_SWEDISH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_SWEDISH_SV, "Svenska"},

/* en */ {IDMS_LANGUAGE,    "Language"},
/* de */ {IDMS_LANGUAGE_DE, "Sprache"},
/* fr */ {IDMS_LANGUAGE_FR, "Langage"},
/* it */ {IDMS_LANGUAGE_IT, "Lingua"},
/* nl */ {IDMS_LANGUAGE_NL, "Taal"},
/* pl */ {IDMS_LANGUAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_SV, "Språk"},

/* ----------------------- AmigaOS Message/Error Strings ----------------------- */

/* en */ {IDMES_SETTINGS_SAVED_SUCCESS,    "Settings saved successfully."},
/* de */ {IDMES_SETTINGS_SAVED_SUCCESS_DE, ""},  /* fuzzy */
/* fr */ {IDMES_SETTINGS_SAVED_SUCCESS_FR, ""},  /* fuzzy */
/* it */ {IDMES_SETTINGS_SAVED_SUCCESS_IT, ""},  /* fuzzy */
/* nl */ {IDMES_SETTINGS_SAVED_SUCCESS_NL, ""},  /* fuzzy */
/* pl */ {IDMES_SETTINGS_SAVED_SUCCESS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_SETTINGS_SAVED_SUCCESS_SV, ""},  /* fuzzy */

/* en */ {IDMES_SETTINGS_LOAD_SUCCESS,    "Settings loaded successfully."},
/* de */ {IDMES_SETTINGS_LOAD_SUCCESS_DE, ""},  /* fuzzy */
/* fr */ {IDMES_SETTINGS_LOAD_SUCCESS_FR, ""},  /* fuzzy */
/* it */ {IDMES_SETTINGS_LOAD_SUCCESS_IT, ""},  /* fuzzy */
/* nl */ {IDMES_SETTINGS_LOAD_SUCCESS_NL, ""},  /* fuzzy */
/* pl */ {IDMES_SETTINGS_LOAD_SUCCESS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_SETTINGS_LOAD_SUCCESS_SV, ""},  /* fuzzy */

/* en */ {IDMES_DFLT_SETTINGS_RESTORED,    "Default settings restored."},
/* de */ {IDMES_DFLT_SETTINGS_RESTORED_DE, ""},  /* fuzzy */
/* fr */ {IDMES_DFLT_SETTINGS_RESTORED_FR, ""},  /* fuzzy */
/* it */ {IDMES_DFLT_SETTINGS_RESTORED_IT, ""},  /* fuzzy */
/* nl */ {IDMES_DFLT_SETTINGS_RESTORED_NL, ""},  /* fuzzy */
/* pl */ {IDMES_DFLT_SETTINGS_RESTORED_PL, ""},  /* fuzzy */
/* sv */ {IDMES_DFLT_SETTINGS_RESTORED_SV, ""},  /* fuzzy */

/* en */ {IDMES_VICE_MESSAGE,    "VICE Message"},
/* de */ {IDMES_VICE_MESSAGE_DE, ""},  /* fuzzy */
/* fr */ {IDMES_VICE_MESSAGE_FR, ""},  /* fuzzy */
/* it */ {IDMES_VICE_MESSAGE_IT, ""},  /* fuzzy */
/* nl */ {IDMES_VICE_MESSAGE_NL, ""},  /* fuzzy */
/* pl */ {IDMES_VICE_MESSAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_VICE_MESSAGE_SV, ""},  /* fuzzy */

/* en */ {IDMES_OK,    "OK"},
/* de */ {IDMES_OK_DE, ""},  /* fuzzy */
/* fr */ {IDMES_OK_FR, ""},  /* fuzzy */
/* it */ {IDMES_OK_IT, ""},  /* fuzzy */
/* nl */ {IDMES_OK_NL, ""},  /* fuzzy */
/* pl */ {IDMES_OK_PL, ""},  /* fuzzy */
/* sv */ {IDMES_OK_SV, ""},  /* fuzzy */

/* en */ {IDMES_VICE_CONTRIBUTORS,    "VICE contributors"},
/* de */ {IDMES_VICE_CONTRIBUTORS_DE, "VICE Hackers"},
/* fr */ {IDMES_VICE_CONTRIBUTORS_FR, "VICE Contributeurs"},
/* it */ {IDMES_VICE_CONTRIBUTORS_IT, "VICE Collaboratori"},
/* nl */ {IDMES_VICE_CONTRIBUTORS_NL, "Medewerkers aan het VICE project"},
/* pl */ {IDMES_VICE_CONTRIBUTORS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_VICE_CONTRIBUTORS_SV, "VICE Bidragslämnare"},

/* en */ {IDMES_WHO_MADE_WHAT,    "Who made what?"},
/* de */ {IDMES_WHO_MADE_WHAT_DE, ""},  /* fuzzy */
/* fr */ {IDMES_WHO_MADE_WHAT_FR, ""},  /* fuzzy */
/* it */ {IDMES_WHO_MADE_WHAT_IT, ""},  /* fuzzy */
/* nl */ {IDMES_WHO_MADE_WHAT_NL, ""},  /* fuzzy */
/* pl */ {IDMES_WHO_MADE_WHAT_PL, ""},  /* fuzzy */
/* sv */ {IDMES_WHO_MADE_WHAT_SV, ""},  /* fuzzy */

/* en */ {IDMES_VICE_DIST_NO_WARRANTY,    "VICE is distributed WITHOUT ANY WARRANTY!"},
/* de */ {IDMES_VICE_DIST_NO_WARRANTY_DE, ""},  /* fuzzy */
/* fr */ {IDMES_VICE_DIST_NO_WARRANTY_FR, ""},  /* fuzzy */
/* it */ {IDMES_VICE_DIST_NO_WARRANTY_IT, ""},  /* fuzzy */
/* nl */ {IDMES_VICE_DIST_NO_WARRANTY_NL, ""},  /* fuzzy */
/* pl */ {IDMES_VICE_DIST_NO_WARRANTY_PL, ""},  /* fuzzy */
/* sv */ {IDMES_VICE_DIST_NO_WARRANTY_SV, ""},  /* fuzzy */

/* en */ {IDMES_WHICH_COMMANDS_AVAILABLE,    "Which command line options are available?"},
/* de */ {IDMES_WHICH_COMMANDS_AVAILABLE_DE, ""},  /* fuzzy */
/* fr */ {IDMES_WHICH_COMMANDS_AVAILABLE_FR, ""},  /* fuzzy */
/* it */ {IDMES_WHICH_COMMANDS_AVAILABLE_IT, ""},  /* fuzzy */
/* nl */ {IDMES_WHICH_COMMANDS_AVAILABLE_NL, ""},  /* fuzzy */
/* pl */ {IDMES_WHICH_COMMANDS_AVAILABLE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_WHICH_COMMANDS_AVAILABLE_SV, ""},  /* fuzzy */

/* en */ {IDMES_CANNOT_SAVE_SETTINGS,    "Cannot save settings."},
/* de */ {IDMES_CANNOT_SAVE_SETTINGS_DE, ""},  /* fuzzy */
/* fr */ {IDMES_CANNOT_SAVE_SETTINGS_FR, ""},  /* fuzzy */
/* it */ {IDMES_CANNOT_SAVE_SETTINGS_IT, ""},  /* fuzzy */
/* nl */ {IDMES_CANNOT_SAVE_SETTINGS_NL, ""},  /* fuzzy */
/* pl */ {IDMES_CANNOT_SAVE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_SAVE_SETTINGS_SV, ""},  /* fuzzy */

/* en */ {IDMES_CANNOT_LOAD_SETTINGS,    "Cannot load settings."},
/* de */ {IDMES_CANNOT_LOAD_SETTINGS_DE, ""},  /* fuzzy */
/* fr */ {IDMES_CANNOT_LOAD_SETTINGS_FR, ""},  /* fuzzy */
/* it */ {IDMES_CANNOT_LOAD_SETTINGS_IT, ""},  /* fuzzy */
/* nl */ {IDMES_CANNOT_LOAD_SETTINGS_NL, ""},  /* fuzzy */
/* pl */ {IDMES_CANNOT_LOAD_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_LOAD_SETTINGS_SV, ""},  /* fuzzy */

/* en */ {IDMES_VICE_ERROR,    "VICE Error"},
/* de */ {IDMES_VICE_ERROR_DE, ""},  /* fuzzy */
/* fr */ {IDMES_VICE_ERROR_FR, ""},  /* fuzzy */
/* it */ {IDMES_VICE_ERROR_IT, ""},  /* fuzzy */
/* nl */ {IDMES_VICE_ERROR_NL, ""},  /* fuzzy */
/* pl */ {IDMES_VICE_ERROR_PL, ""},  /* fuzzy */
/* sv */ {IDMES_VICE_ERROR_SV, ""},  /* fuzzy */

/* en */ {IDMES_NO_JOY_ON_PORT_D,    "No joystick or joypad found on port %d"},
/* de */ {IDMES_NO_JOY_ON_PORT_D_DE, ""},  /* fuzzy */
/* fr */ {IDMES_NO_JOY_ON_PORT_D_FR, ""},  /* fuzzy */
/* it */ {IDMES_NO_JOY_ON_PORT_D_IT, ""},  /* fuzzy */
/* nl */ {IDMES_NO_JOY_ON_PORT_D_NL, ""},  /* fuzzy */
/* pl */ {IDMES_NO_JOY_ON_PORT_D_PL, ""},  /* fuzzy */
/* sv */ {IDMES_NO_JOY_ON_PORT_D_SV, ""},  /* fuzzy */

/* en */ {IDMES_MOUSE_ON_PORT_D,    "A mouse was found on port %d"},
/* de */ {IDMES_MOUSE_ON_PORT_D_DE, ""},  /* fuzzy */
/* fr */ {IDMES_MOUSE_ON_PORT_D_FR, ""},  /* fuzzy */
/* it */ {IDMES_MOUSE_ON_PORT_D_IT, ""},  /* fuzzy */
/* nl */ {IDMES_MOUSE_ON_PORT_D_NL, ""},  /* fuzzy */
/* pl */ {IDMES_MOUSE_ON_PORT_D_PL, ""},  /* fuzzy */
/* sv */ {IDMES_MOUSE_ON_PORT_D_SV, ""},  /* fuzzy */

/* en */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D,    "Unknown device found on port %d"},
/* de */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_DE, ""},  /* fuzzy */
/* fr */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_FR, ""},  /* fuzzy */
/* it */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_IT, ""},  /* fuzzy */
/* nl */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_NL, ""},  /* fuzzy */
/* pl */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_PL, ""},  /* fuzzy */
/* sv */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_SV, ""},  /* fuzzy */

/* en */ {IDMES_DEVICE_NOT_GAMEPAD,    "The device on this port is not a gamepad"},
/* de */ {IDMES_DEVICE_NOT_GAMEPAD_DE, ""},  /* fuzzy */
/* fr */ {IDMES_DEVICE_NOT_GAMEPAD_FR, ""},  /* fuzzy */
/* it */ {IDMES_DEVICE_NOT_GAMEPAD_IT, ""},  /* fuzzy */
/* nl */ {IDMES_DEVICE_NOT_GAMEPAD_NL, ""},  /* fuzzy */
/* pl */ {IDMES_DEVICE_NOT_GAMEPAD_PL, ""},  /* fuzzy */
/* sv */ {IDMES_DEVICE_NOT_GAMEPAD_SV, ""},  /* fuzzy */

/* en */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT,    "The port is not mapped to an Amiga port"},
/* de */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_DE, ""},  /* fuzzy */
/* fr */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_FR, ""},  /* fuzzy */
/* it */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_IT, ""},  /* fuzzy */
/* nl */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_NL, ""},  /* fuzzy */
/* pl */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_PL, ""},  /* fuzzy */
/* sv */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_SV, ""},  /* fuzzy */

/* en */ {IDMES_CANNOT_AUTOSTART_FILE,    "Cannot autostart specified file."},
/* de */ {IDMES_CANNOT_AUTOSTART_FILE_DE, ""},  /* fuzzy */
/* fr */ {IDMES_CANNOT_AUTOSTART_FILE_FR, ""},  /* fuzzy */
/* it */ {IDMES_CANNOT_AUTOSTART_FILE_IT, ""},  /* fuzzy */
/* nl */ {IDMES_CANNOT_AUTOSTART_FILE_NL, ""},  /* fuzzy */
/* pl */ {IDMES_CANNOT_AUTOSTART_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_AUTOSTART_FILE_SV, ""},  /* fuzzy */

/* en */ {IDMES_CANNOT_ATTACH_FILE,    "Cannot attach specified file"},
/* de */ {IDMES_CANNOT_ATTACH_FILE_DE, ""},  /* fuzzy */
/* fr */ {IDMES_CANNOT_ATTACH_FILE_FR, ""},  /* fuzzy */
/* it */ {IDMES_CANNOT_ATTACH_FILE_IT, ""},  /* fuzzy */
/* nl */ {IDMES_CANNOT_ATTACH_FILE_NL, ""},  /* fuzzy */
/* pl */ {IDMES_CANNOT_ATTACH_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_ATTACH_FILE_SV, ""},  /* fuzzy */

/* en */ {IDMES_INVALID_CART,    "Invalid cartridge"},
/* de */ {IDMES_INVALID_CART_DE, ""},  /* fuzzy */
/* fr */ {IDMES_INVALID_CART_FR, ""},  /* fuzzy */
/* it */ {IDMES_INVALID_CART_IT, ""},  /* fuzzy */
/* nl */ {IDMES_INVALID_CART_NL, ""},  /* fuzzy */
/* pl */ {IDMES_INVALID_CART_PL, ""},  /* fuzzy */
/* sv */ {IDMES_INVALID_CART_SV, ""},  /* fuzzy */

/* en */ {IDMES_BAD_CART_CONFIG_IN_UI,    "Bad cartridge config in UI!"},
/* de */ {IDMES_BAD_CART_CONFIG_IN_UI_DE, ""},  /* fuzzy */
/* fr */ {IDMES_BAD_CART_CONFIG_IN_UI_FR, ""},  /* fuzzy */
/* it */ {IDMES_BAD_CART_CONFIG_IN_UI_IT, ""},  /* fuzzy */
/* nl */ {IDMES_BAD_CART_CONFIG_IN_UI_NL, ""},  /* fuzzy */
/* pl */ {IDMES_BAD_CART_CONFIG_IN_UI_PL, ""},  /* fuzzy */
/* sv */ {IDMES_BAD_CART_CONFIG_IN_UI_SV, ""},  /* fuzzy */

/* en */ {IDMES_INVALID_CART_IMAGE,    "Invalid cartridge image"},
/* de */ {IDMES_INVALID_CART_IMAGE_DE, ""},  /* fuzzy */
/* fr */ {IDMES_INVALID_CART_IMAGE_FR, ""},  /* fuzzy */
/* it */ {IDMES_INVALID_CART_IMAGE_IT, ""},  /* fuzzy */
/* nl */ {IDMES_INVALID_CART_IMAGE_NL, ""},  /* fuzzy */
/* pl */ {IDMES_INVALID_CART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_INVALID_CART_IMAGE_SV, ""},  /* fuzzy */

/* en */ {IDMES_CANNOT_CREATE_IMAGE,    "Cannot create image"},
/* de */ {IDMES_CANNOT_CREATE_IMAGE_DE, ""},  /* fuzzy */
/* fr */ {IDMES_CANNOT_CREATE_IMAGE_FR, ""},  /* fuzzy */
/* it */ {IDMES_CANNOT_CREATE_IMAGE_IT, ""},  /* fuzzy */
/* nl */ {IDMES_CANNOT_CREATE_IMAGE_NL, ""},  /* fuzzy */
/* pl */ {IDMES_CANNOT_CREATE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_CREATE_IMAGE_SV, ""},  /* fuzzy */

/* en */ {IDMES_ERROR_STARTING_SERVER,    "An error occured starting the server."},
/* de */ {IDMES_ERROR_STARTING_SERVER_DE, ""},  /* fuzzy */
/* fr */ {IDMES_ERROR_STARTING_SERVER_FR, ""},  /* fuzzy */
/* it */ {IDMES_ERROR_STARTING_SERVER_IT, ""},  /* fuzzy */
/* nl */ {IDMES_ERROR_STARTING_SERVER_NL, ""},  /* fuzzy */
/* pl */ {IDMES_ERROR_STARTING_SERVER_PL, ""},  /* fuzzy */
/* sv */ {IDMES_ERROR_STARTING_SERVER_SV, ""},  /* fuzzy */

/* en */ {IDMES_ERROR_CONNECTING_CLIENT,    "An error occured connecting the client."},
/* de */ {IDMES_ERROR_CONNECTING_CLIENT_DE, ""},  /* fuzzy */
/* fr */ {IDMES_ERROR_CONNECTING_CLIENT_FR, ""},  /* fuzzy */
/* it */ {IDMES_ERROR_CONNECTING_CLIENT_IT, ""},  /* fuzzy */
/* nl */ {IDMES_ERROR_CONNECTING_CLIENT_NL, ""},  /* fuzzy */
/* pl */ {IDMES_ERROR_CONNECTING_CLIENT_PL, ""},  /* fuzzy */
/* sv */ {IDMES_ERROR_CONNECTING_CLIENT_SV, ""},  /* fuzzy */

/* en */ {IDMES_INVALID_PORT_NUMBER,    "Invalid port number"},
/* de */ {IDMES_INVALID_PORT_NUMBER_DE, ""},  /* fuzzy */
/* fr */ {IDMES_INVALID_PORT_NUMBER_FR, ""},  /* fuzzy */
/* it */ {IDMES_INVALID_PORT_NUMBER_IT, ""},  /* fuzzy */
/* nl */ {IDMES_INVALID_PORT_NUMBER_NL, ""},  /* fuzzy */
/* pl */ {IDMES_INVALID_PORT_NUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDMES_INVALID_PORT_NUMBER_SV, ""},  /* fuzzy */

/* en */ {IDMES_THIS_MACHINE_NO_SID,    "This machine may not have a SID"},
/* de */ {IDMES_THIS_MACHINE_NO_SID_DE, ""},  /* fuzzy */
/* fr */ {IDMES_THIS_MACHINE_NO_SID_FR, ""},  /* fuzzy */
/* it */ {IDMES_THIS_MACHINE_NO_SID_IT, ""},  /* fuzzy */
/* nl */ {IDMES_THIS_MACHINE_NO_SID_NL, ""},  /* fuzzy */
/* pl */ {IDMES_THIS_MACHINE_NO_SID_PL, ""},  /* fuzzy */
/* sv */ {IDMES_THIS_MACHINE_NO_SID_SV, ""},  /* fuzzy */

/* ----------------------- AmigaOS Strings ----------------------- */

/* en */ {IDS_PRESS_KEY_BUTTON,    "Press desired key/button, move stick or press ESC for no key."},
/* de */ {IDS_PRESS_KEY_BUTTON_DE, ""},  /* fuzzy */
/* fr */ {IDS_PRESS_KEY_BUTTON_FR, ""},  /* fuzzy */
/* it */ {IDS_PRESS_KEY_BUTTON_IT, ""},  /* fuzzy */
/* nl */ {IDS_PRESS_KEY_BUTTON_NL, ""},  /* fuzzy */
/* pl */ {IDS_PRESS_KEY_BUTTON_PL, ""},  /* fuzzy */
/* sv */ {IDS_PRESS_KEY_BUTTON_SV, ""},  /* fuzzy */

};


static int intl_translate_text_table[][countof(language_table)] = {

/* ----------------------- AmigaOS Menu Strings ----------------------- */

/* en */ {IDMS_FILE,
/* de */  IDMS_FILE_DE,
/* fr */  IDMS_FILE_FR,
/* it */  IDMS_FILE_IT,
/* nl */  IDMS_FILE_NL,
/* pl */  IDMS_FILE_PL,
/* sv */  IDMS_FILE_SV},

/* en */ {IDMS_AUTOSTART_IMAGE,
/* de */  IDMS_AUTOSTART_IMAGE_DE,
/* fr */  IDMS_AUTOSTART_IMAGE_FR,
/* it */  IDMS_AUTOSTART_IMAGE_IT,
/* nl */  IDMS_AUTOSTART_IMAGE_NL,
/* pl */  IDMS_AUTOSTART_IMAGE_PL,
/* sv */  IDMS_AUTOSTART_IMAGE_SV},

/* en */ {IDMS_ATTACH_DISK_IMAGE,
/* de */  IDMS_ATTACH_DISK_IMAGE_DE,
/* fr */  IDMS_ATTACH_DISK_IMAGE_FR,
/* it */  IDMS_ATTACH_DISK_IMAGE_IT,
/* nl */  IDMS_ATTACH_DISK_IMAGE_NL,
/* pl */  IDMS_ATTACH_DISK_IMAGE_PL,
/* sv */  IDMS_ATTACH_DISK_IMAGE_SV},

/* en */ {IDMS_DRIVE_8,
/* de */  IDMS_DRIVE_8_DE,
/* fr */  IDMS_DRIVE_8_FR,
/* it */  IDMS_DRIVE_8_IT,
/* nl */  IDMS_DRIVE_8_NL,
/* pl */  IDMS_DRIVE_8_PL,
/* sv */  IDMS_DRIVE_8_SV},

/* en */ {IDMS_DRIVE_9,
/* de */  IDMS_DRIVE_9_DE,
/* fr */  IDMS_DRIVE_9_FR,
/* it */  IDMS_DRIVE_9_IT,
/* nl */  IDMS_DRIVE_9_NL,
/* pl */  IDMS_DRIVE_9_PL,
/* sv */  IDMS_DRIVE_9_SV},

/* en */ {IDMS_DRIVE_10,
/* de */  IDMS_DRIVE_10_DE,
/* fr */  IDMS_DRIVE_10_FR,
/* it */  IDMS_DRIVE_10_IT,
/* nl */  IDMS_DRIVE_10_NL,
/* pl */  IDMS_DRIVE_10_PL,
/* sv */  IDMS_DRIVE_10_SV},

/* en */ {IDMS_DRIVE_11,
/* de */  IDMS_DRIVE_11_DE,
/* fr */  IDMS_DRIVE_11_FR,
/* it */  IDMS_DRIVE_11_IT,
/* nl */  IDMS_DRIVE_11_NL,
/* pl */  IDMS_DRIVE_11_PL,
/* sv */  IDMS_DRIVE_11_SV},

/* en */ {IDMS_DETACH_DISK_IMAGE,
/* de */  IDMS_DETACH_DISK_IMAGE_DE,
/* fr */  IDMS_DETACH_DISK_IMAGE_FR,
/* it */  IDMS_DETACH_DISK_IMAGE_IT,
/* nl */  IDMS_DETACH_DISK_IMAGE_NL,
/* pl */  IDMS_DETACH_DISK_IMAGE_PL,
/* sv */  IDMS_DETACH_DISK_IMAGE_SV},

/* en */ {IDMS_ALL,
/* de */  IDMS_ALL_DE,
/* fr */  IDMS_ALL_FR,
/* it */  IDMS_ALL_IT,
/* nl */  IDMS_ALL_NL,
/* pl */  IDMS_ALL_PL,
/* sv */  IDMS_ALL_SV},

/* en */ {IDMS_FLIP_LIST,
/* de */  IDMS_FLIP_LIST_DE,
/* fr */  IDMS_FLIP_LIST_FR,
/* it */  IDMS_FLIP_LIST_IT,
/* nl */  IDMS_FLIP_LIST_NL,
/* pl */  IDMS_FLIP_LIST_PL,
/* sv */  IDMS_FLIP_LIST_SV},

/* en */ {IDMS_ADD_CURRENT_IMAGE,
/* de */  IDMS_ADD_CURRENT_IMAGE_DE,
/* fr */  IDMS_ADD_CURRENT_IMAGE_FR,
/* it */  IDMS_ADD_CURRENT_IMAGE_IT,
/* nl */  IDMS_ADD_CURRENT_IMAGE_NL,
/* pl */  IDMS_ADD_CURRENT_IMAGE_PL,
/* sv */  IDMS_ADD_CURRENT_IMAGE_SV},

/* en */ {IDMS_REMOVE_CURRENT_IMAGE,
/* de */  IDMS_REMOVE_CURRENT_IMAGE_DE,
/* fr */  IDMS_REMOVE_CURRENT_IMAGE_FR,
/* it */  IDMS_REMOVE_CURRENT_IMAGE_IT,
/* nl */  IDMS_REMOVE_CURRENT_IMAGE_NL,
/* pl */  IDMS_REMOVE_CURRENT_IMAGE_PL,
/* sv */  IDMS_REMOVE_CURRENT_IMAGE_SV},

/* en */ {IDMS_ATTACH_NEXT_IMAGE,
/* de */  IDMS_ATTACH_NEXT_IMAGE_DE,
/* fr */  IDMS_ATTACH_NEXT_IMAGE_FR,
/* it */  IDMS_ATTACH_NEXT_IMAGE_IT,
/* nl */  IDMS_ATTACH_NEXT_IMAGE_NL,
/* pl */  IDMS_ATTACH_NEXT_IMAGE_PL,
/* sv */  IDMS_ATTACH_NEXT_IMAGE_SV},

/* en */ {IDMS_ATTACH_PREVIOUS_IMAGE,
/* de */  IDMS_ATTACH_PREVIOUS_IMAGE_DE,
/* fr */  IDMS_ATTACH_PREVIOUS_IMAGE_FR,
/* it */  IDMS_ATTACH_PREVIOUS_IMAGE_IT,
/* nl */  IDMS_ATTACH_PREVIOUS_IMAGE_NL,
/* pl */  IDMS_ATTACH_PREVIOUS_IMAGE_PL,
/* sv */  IDMS_ATTACH_PREVIOUS_IMAGE_SV},

/* en */ {IDMS_LOAD_FLIP_LIST,
/* de */  IDMS_LOAD_FLIP_LIST_DE,
/* fr */  IDMS_LOAD_FLIP_LIST_FR,
/* it */  IDMS_LOAD_FLIP_LIST_IT,
/* nl */  IDMS_LOAD_FLIP_LIST_NL,
/* pl */  IDMS_LOAD_FLIP_LIST_PL,
/* sv */  IDMS_LOAD_FLIP_LIST_SV},

/* en */ {IDMS_SAVE_FLIP_LIST,
/* de */  IDMS_SAVE_FLIP_LIST_DE,
/* fr */  IDMS_SAVE_FLIP_LIST_FR,
/* it */  IDMS_SAVE_FLIP_LIST_IT,
/* nl */  IDMS_SAVE_FLIP_LIST_NL,
/* pl */  IDMS_SAVE_FLIP_LIST_PL,
/* sv */  IDMS_SAVE_FLIP_LIST_SV},

/* en */ {IDMS_ATTACH_TAPE_IMAGE,
/* de */  IDMS_ATTACH_TAPE_IMAGE_DE,
/* fr */  IDMS_ATTACH_TAPE_IMAGE_FR,
/* it */  IDMS_ATTACH_TAPE_IMAGE_IT,
/* nl */  IDMS_ATTACH_TAPE_IMAGE_NL,
/* pl */  IDMS_ATTACH_TAPE_IMAGE_PL,
/* sv */  IDMS_ATTACH_TAPE_IMAGE_SV},

/* en */ {IDMS_DETACH_TAPE_IMAGE,
/* de */  IDMS_DETACH_TAPE_IMAGE_DE,
/* fr */  IDMS_DETACH_TAPE_IMAGE_FR,
/* it */  IDMS_DETACH_TAPE_IMAGE_IT,
/* nl */  IDMS_DETACH_TAPE_IMAGE_NL,
/* pl */  IDMS_DETACH_TAPE_IMAGE_PL,
/* sv */  IDMS_DETACH_TAPE_IMAGE_SV},

/* en */ {IDMS_DATASSETTE_CONTROL,
/* de */  IDMS_DATASSETTE_CONTROL_DE,
/* fr */  IDMS_DATASSETTE_CONTROL_FR,
/* it */  IDMS_DATASSETTE_CONTROL_IT,
/* nl */  IDMS_DATASSETTE_CONTROL_NL,
/* pl */  IDMS_DATASSETTE_CONTROL_PL,
/* sv */  IDMS_DATASSETTE_CONTROL_SV},

/* en */ {IDMS_STOP,
/* de */  IDMS_STOP_DE,
/* fr */  IDMS_STOP_FR,
/* it */  IDMS_STOP_IT,
/* nl */  IDMS_STOP_NL,
/* pl */  IDMS_STOP_PL,
/* sv */  IDMS_STOP_SV},

/* en */ {IDMS_START,
/* de */  IDMS_START_DE,
/* fr */  IDMS_START_FR,
/* it */  IDMS_START_IT,
/* nl */  IDMS_START_NL,
/* pl */  IDMS_START_PL,
/* sv */  IDMS_START_SV},

/* en */ {IDMS_FORWARD,
/* de */  IDMS_FORWARD_DE,
/* fr */  IDMS_FORWARD_FR,
/* it */  IDMS_FORWARD_IT,
/* nl */  IDMS_FORWARD_NL,
/* pl */  IDMS_FORWARD_PL,
/* sv */  IDMS_FORWARD_SV},

/* en */ {IDMS_REWIND,
/* de */  IDMS_REWIND_DE,
/* fr */  IDMS_REWIND_FR,
/* it */  IDMS_REWIND_IT,
/* nl */  IDMS_REWIND_NL,
/* pl */  IDMS_REWIND_PL,
/* sv */  IDMS_REWIND_SV},

/* en */ {IDMS_RECORD,
/* de */  IDMS_RECORD_DE,
/* fr */  IDMS_RECORD_FR,
/* it */  IDMS_RECORD_IT,
/* nl */  IDMS_RECORD_NL,
/* pl */  IDMS_RECORD_PL,
/* sv */  IDMS_RECORD_SV},

/* en */ {IDMS_RESET,
/* de */  IDMS_RESET_DE,
/* fr */  IDMS_RESET_FR,
/* it */  IDMS_RESET_IT,
/* nl */  IDMS_RESET_NL,
/* pl */  IDMS_RESET_PL,
/* sv */  IDMS_RESET_SV},

/* en */ {IDMS_RESET_COUNTER,
/* de */  IDMS_RESET_COUNTER_DE,
/* fr */  IDMS_RESET_COUNTER_FR,
/* it */  IDMS_RESET_COUNTER_IT,
/* nl */  IDMS_RESET_COUNTER_NL,
/* pl */  IDMS_RESET_COUNTER_PL,
/* sv */  IDMS_RESET_COUNTER_SV},

/* en */ {IDMS_ATTACH_CART_IMAGE,
/* de */  IDMS_ATTACH_CART_IMAGE_DE,
/* fr */  IDMS_ATTACH_CART_IMAGE_FR,
/* it */  IDMS_ATTACH_CART_IMAGE_IT,
/* nl */  IDMS_ATTACH_CART_IMAGE_NL,
/* pl */  IDMS_ATTACH_CART_IMAGE_PL,
/* sv */  IDMS_ATTACH_CART_IMAGE_SV},

/* en */ {IDMS_4_8_16KB_AT_2000,
/* de */  IDMS_4_8_16KB_AT_2000_DE,
/* fr */  IDMS_4_8_16KB_AT_2000_FR,
/* it */  IDMS_4_8_16KB_AT_2000_IT,
/* nl */  IDMS_4_8_16KB_AT_2000_NL,
/* pl */  IDMS_4_8_16KB_AT_2000_PL,
/* sv */  IDMS_4_8_16KB_AT_2000_SV},

/* en */ {IDMS_4_8_16KB_AT_4000,
/* de */  IDMS_4_8_16KB_AT_4000_DE,
/* fr */  IDMS_4_8_16KB_AT_4000_FR,
/* it */  IDMS_4_8_16KB_AT_4000_IT,
/* nl */  IDMS_4_8_16KB_AT_4000_NL,
/* pl */  IDMS_4_8_16KB_AT_4000_PL,
/* sv */  IDMS_4_8_16KB_AT_4000_SV},

/* en */ {IDMS_4_8_16KB_AT_6000,
/* de */  IDMS_4_8_16KB_AT_6000_DE,
/* fr */  IDMS_4_8_16KB_AT_6000_FR,
/* it */  IDMS_4_8_16KB_AT_6000_IT,
/* nl */  IDMS_4_8_16KB_AT_6000_NL,
/* pl */  IDMS_4_8_16KB_AT_6000_PL,
/* sv */  IDMS_4_8_16KB_AT_6000_SV},

/* en */ {IDMS_4_8KB_AT_A000,
/* de */  IDMS_4_8KB_AT_A000_DE,
/* fr */  IDMS_4_8KB_AT_A000_FR,
/* it */  IDMS_4_8KB_AT_A000_IT,
/* nl */  IDMS_4_8KB_AT_A000_NL,
/* pl */  IDMS_4_8KB_AT_A000_PL,
/* sv */  IDMS_4_8KB_AT_A000_SV},

/* en */ {IDMS_4KB_AT_B000,
/* de */  IDMS_4KB_AT_B000_DE,
/* fr */  IDMS_4KB_AT_B000_FR,
/* it */  IDMS_4KB_AT_B000_IT,
/* nl */  IDMS_4KB_AT_B000_NL,
/* pl */  IDMS_4KB_AT_B000_PL,
/* sv */  IDMS_4KB_AT_B000_SV},

/* en */ {IDMS_DETACH_CART_IMAGE,
/* de */  IDMS_DETACH_CART_IMAGE_DE,
/* fr */  IDMS_DETACH_CART_IMAGE_FR,
/* it */  IDMS_DETACH_CART_IMAGE_IT,
/* nl */  IDMS_DETACH_CART_IMAGE_NL,
/* pl */  IDMS_DETACH_CART_IMAGE_PL,
/* sv */  IDMS_DETACH_CART_IMAGE_SV},

/* en */ {IDMS_C1_LOW_IMAGE,
/* de */  IDMS_C1_LOW_IMAGE_DE,
/* fr */  IDMS_C1_LOW_IMAGE_FR,
/* it */  IDMS_C1_LOW_IMAGE_IT,
/* nl */  IDMS_C1_LOW_IMAGE_NL,
/* pl */  IDMS_C1_LOW_IMAGE_PL,
/* sv */  IDMS_C1_LOW_IMAGE_SV},

/* en */ {IDMS_C1_HIGH_IMAGE,
/* de */  IDMS_C1_HIGH_IMAGE_DE,
/* fr */  IDMS_C1_HIGH_IMAGE_FR,
/* it */  IDMS_C1_HIGH_IMAGE_IT,
/* nl */  IDMS_C1_HIGH_IMAGE_NL,
/* pl */  IDMS_C1_HIGH_IMAGE_PL,
/* sv */  IDMS_C1_HIGH_IMAGE_SV},

/* en */ {IDMS_C2_LOW_IMAGE,
/* de */  IDMS_C2_LOW_IMAGE_DE,
/* fr */  IDMS_C2_LOW_IMAGE_FR,
/* it */  IDMS_C2_LOW_IMAGE_IT,
/* nl */  IDMS_C2_LOW_IMAGE_NL,
/* pl */  IDMS_C2_LOW_IMAGE_PL,
/* sv */  IDMS_C2_LOW_IMAGE_SV},

/* en */ {IDMS_C2_HIGH_IMAGE,
/* de */  IDMS_C2_HIGH_IMAGE_DE,
/* fr */  IDMS_C2_HIGH_IMAGE_FR,
/* it */  IDMS_C2_HIGH_IMAGE_IT,
/* nl */  IDMS_C2_HIGH_IMAGE_NL,
/* pl */  IDMS_C2_HIGH_IMAGE_PL,
/* sv */  IDMS_C2_HIGH_IMAGE_SV},

/* en */ {IDMS_FUNCTION_LOW_3PLUS1,
/* de */  IDMS_FUNCTION_LOW_3PLUS1_DE,
/* fr */  IDMS_FUNCTION_LOW_3PLUS1_FR,
/* it */  IDMS_FUNCTION_LOW_3PLUS1_IT,
/* nl */  IDMS_FUNCTION_LOW_3PLUS1_NL,
/* pl */  IDMS_FUNCTION_LOW_3PLUS1_PL,
/* sv */  IDMS_FUNCTION_LOW_3PLUS1_SV},

/* en */ {IDMS_FUNCTION_HIGH_3PLUS1,
/* de */  IDMS_FUNCTION_HIGH_3PLUS1_DE,
/* fr */  IDMS_FUNCTION_HIGH_3PLUS1_FR,
/* it */  IDMS_FUNCTION_HIGH_3PLUS1_IT,
/* nl */  IDMS_FUNCTION_HIGH_3PLUS1_NL,
/* pl */  IDMS_FUNCTION_HIGH_3PLUS1_PL,
/* sv */  IDMS_FUNCTION_HIGH_3PLUS1_SV},

/* en */ {IDMS_CRT_IMAGE,
/* de */  IDMS_CRT_IMAGE_DE,
/* fr */  IDMS_CRT_IMAGE_FR,
/* it */  IDMS_CRT_IMAGE_IT,
/* nl */  IDMS_CRT_IMAGE_NL,
/* pl */  IDMS_CRT_IMAGE_PL,
/* sv */  IDMS_CRT_IMAGE_SV},

/* en */ {IDMS_GENERIC_8KB_IMAGE,
/* de */  IDMS_GENERIC_8KB_IMAGE_DE,
/* fr */  IDMS_GENERIC_8KB_IMAGE_FR,
/* it */  IDMS_GENERIC_8KB_IMAGE_IT,
/* nl */  IDMS_GENERIC_8KB_IMAGE_NL,
/* pl */  IDMS_GENERIC_8KB_IMAGE_PL,
/* sv */  IDMS_GENERIC_8KB_IMAGE_SV},

/* en */ {IDMS_GENERIC_16KB_IMAGE,
/* de */  IDMS_GENERIC_16KB_IMAGE_DE,
/* fr */  IDMS_GENERIC_16KB_IMAGE_FR,
/* it */  IDMS_GENERIC_16KB_IMAGE_IT,
/* nl */  IDMS_GENERIC_16KB_IMAGE_NL,
/* pl */  IDMS_GENERIC_16KB_IMAGE_PL,
/* sv */  IDMS_GENERIC_16KB_IMAGE_SV},

/* en */ {IDMS_ACTION_REPLAY_IMAGE,
/* de */  IDMS_ACTION_REPLAY_IMAGE_DE,
/* fr */  IDMS_ACTION_REPLAY_IMAGE_FR,
/* it */  IDMS_ACTION_REPLAY_IMAGE_IT,
/* nl */  IDMS_ACTION_REPLAY_IMAGE_NL,
/* pl */  IDMS_ACTION_REPLAY_IMAGE_PL,
/* sv */  IDMS_ACTION_REPLAY_IMAGE_SV},

/* en */ {IDMS_ATOMIC_POWER_IMAGE,
/* de */  IDMS_ATOMIC_POWER_IMAGE_DE,
/* fr */  IDMS_ATOMIC_POWER_IMAGE_FR,
/* it */  IDMS_ATOMIC_POWER_IMAGE_IT,
/* nl */  IDMS_ATOMIC_POWER_IMAGE_NL,
/* pl */  IDMS_ATOMIC_POWER_IMAGE_PL,
/* sv */  IDMS_ATOMIC_POWER_IMAGE_SV},

/* en */ {IDMS_EPYX_FASTLOAD_IMAGE,
/* de */  IDMS_EPYX_FASTLOAD_IMAGE_DE,
/* fr */  IDMS_EPYX_FASTLOAD_IMAGE_FR,
/* it */  IDMS_EPYX_FASTLOAD_IMAGE_IT,
/* nl */  IDMS_EPYX_FASTLOAD_IMAGE_NL,
/* pl */  IDMS_EPYX_FASTLOAD_IMAGE_PL,
/* sv */  IDMS_EPYX_FASTLOAD_IMAGE_SV},

/* en */ {IDMS_IEEE488_INTERFACE_IMAGE,
/* de */  IDMS_IEEE488_INTERFACE_IMAGE_DE,
/* fr */  IDMS_IEEE488_INTERFACE_IMAGE_FR,
/* it */  IDMS_IEEE488_INTERFACE_IMAGE_IT,
/* nl */  IDMS_IEEE488_INTERFACE_IMAGE_NL,
/* pl */  IDMS_IEEE488_INTERFACE_IMAGE_PL,
/* sv */  IDMS_IEEE488_INTERFACE_IMAGE_SV},

/* en */ {IDMS_RETRO_REPLAY_IMAGE,
/* de */  IDMS_RETRO_REPLAY_IMAGE_DE,
/* fr */  IDMS_RETRO_REPLAY_IMAGE_FR,
/* it */  IDMS_RETRO_REPLAY_IMAGE_IT,
/* nl */  IDMS_RETRO_REPLAY_IMAGE_NL,
/* pl */  IDMS_RETRO_REPLAY_IMAGE_PL,
/* sv */  IDMS_RETRO_REPLAY_IMAGE_SV},

/* en */ {IDMS_IDE64_INTERFACE_IMAGE,
/* de */  IDMS_IDE64_INTERFACE_IMAGE_DE,
/* fr */  IDMS_IDE64_INTERFACE_IMAGE_FR,
/* it */  IDMS_IDE64_INTERFACE_IMAGE_IT,
/* nl */  IDMS_IDE64_INTERFACE_IMAGE_NL,
/* pl */  IDMS_IDE64_INTERFACE_IMAGE_PL,
/* sv */  IDMS_IDE64_INTERFACE_IMAGE_SV},

/* en */ {IDMS_SUPER_SNAPSHOT_4_IMAGE,
/* de */  IDMS_SUPER_SNAPSHOT_4_IMAGE_DE,
/* fr */  IDMS_SUPER_SNAPSHOT_4_IMAGE_FR,
/* it */  IDMS_SUPER_SNAPSHOT_4_IMAGE_IT,
/* nl */  IDMS_SUPER_SNAPSHOT_4_IMAGE_NL,
/* pl */  IDMS_SUPER_SNAPSHOT_4_IMAGE_PL,
/* sv */  IDMS_SUPER_SNAPSHOT_4_IMAGE_SV},

/* en */ {IDMS_SUPER_SNAPSHOT_5_IMAGE,
/* de */  IDMS_SUPER_SNAPSHOT_5_IMAGE_DE,
/* fr */  IDMS_SUPER_SNAPSHOT_5_IMAGE_FR,
/* it */  IDMS_SUPER_SNAPSHOT_5_IMAGE_IT,
/* nl */  IDMS_SUPER_SNAPSHOT_5_IMAGE_NL,
/* pl */  IDMS_SUPER_SNAPSHOT_5_IMAGE_PL,
/* sv */  IDMS_SUPER_SNAPSHOT_5_IMAGE_SV},

/* en */ {IDMS_STRUCTURED_BASIC_IMAGE,
/* de */  IDMS_STRUCTURED_BASIC_IMAGE_DE,
/* fr */  IDMS_STRUCTURED_BASIC_IMAGE_FR,
/* it */  IDMS_STRUCTURED_BASIC_IMAGE_IT,
/* nl */  IDMS_STRUCTURED_BASIC_IMAGE_NL,
/* pl */  IDMS_STRUCTURED_BASIC_IMAGE_PL,
/* sv */  IDMS_STRUCTURED_BASIC_IMAGE_SV},

/* en */ {IDMS_EXPERT_CART,
/* de */  IDMS_EXPERT_CART_DE,
/* fr */  IDMS_EXPERT_CART_FR,
/* it */  IDMS_EXPERT_CART_IT,
/* nl */  IDMS_EXPERT_CART_NL,
/* pl */  IDMS_EXPERT_CART_PL,
/* sv */  IDMS_EXPERT_CART_SV},

/* en */ {IDMS_ENABLE,
/* de */  IDMS_ENABLE_DE,
/* fr */  IDMS_ENABLE_FR,
/* it */  IDMS_ENABLE_IT,
/* nl */  IDMS_ENABLE_NL,
/* pl */  IDMS_ENABLE_PL,
/* sv */  IDMS_ENABLE_SV},

/* en */ {IDMS_OFF,
/* de */  IDMS_OFF_DE,
/* fr */  IDMS_OFF_FR,
/* it */  IDMS_OFF_IT,
/* nl */  IDMS_OFF_NL,
/* pl */  IDMS_OFF_PL,
/* sv */  IDMS_OFF_SV},

/* en */ {IDMS_PRG,
/* de */  IDMS_PRG_DE,
/* fr */  IDMS_PRG_FR,
/* it */  IDMS_PRG_IT,
/* nl */  IDMS_PRG_NL,
/* pl */  IDMS_PRG_PL,
/* sv */  IDMS_PRG_SV},

/* en */ {IDMS_ON,
/* de */  IDMS_ON_DE,
/* fr */  IDMS_ON_FR,
/* it */  IDMS_ON_IT,
/* nl */  IDMS_ON_NL,
/* pl */  IDMS_ON_PL,
/* sv */  IDMS_ON_SV},

/* en */ {IDMS_SET_CART_AS_DEFAULT,
/* de */  IDMS_SET_CART_AS_DEFAULT_DE,
/* fr */  IDMS_SET_CART_AS_DEFAULT_FR,
/* it */  IDMS_SET_CART_AS_DEFAULT_IT,
/* nl */  IDMS_SET_CART_AS_DEFAULT_NL,
/* pl */  IDMS_SET_CART_AS_DEFAULT_PL,
/* sv */  IDMS_SET_CART_AS_DEFAULT_SV},

/* en */ {IDMS_RESET_ON_CART_CHANGE,
/* de */  IDMS_RESET_ON_CART_CHANGE_DE,
/* fr */  IDMS_RESET_ON_CART_CHANGE_FR,
/* it */  IDMS_RESET_ON_CART_CHANGE_IT,
/* nl */  IDMS_RESET_ON_CART_CHANGE_NL,
/* pl */  IDMS_RESET_ON_CART_CHANGE_PL,
/* sv */  IDMS_RESET_ON_CART_CHANGE_SV},

/* en */ {IDMS_CART_FREEZE,
/* de */  IDMS_CART_FREEZE_DE,
/* fr */  IDMS_CART_FREEZE_FR,
/* it */  IDMS_CART_FREEZE_IT,
/* nl */  IDMS_CART_FREEZE_NL,
/* pl */  IDMS_CART_FREEZE_PL,
/* sv */  IDMS_CART_FREEZE_SV},

/* en */ {IDMS_PAUSE,
/* de */  IDMS_PAUSE_DE,
/* fr */  IDMS_PAUSE_FR,
/* it */  IDMS_PAUSE_IT,
/* nl */  IDMS_PAUSE_NL,
/* pl */  IDMS_PAUSE_PL,
/* sv */  IDMS_PAUSE_SV},

/* en */ {IDMS_MONITOR,
/* de */  IDMS_MONITOR_DE,
/* fr */  IDMS_MONITOR_FR,
/* it */  IDMS_MONITOR_IT,
/* nl */  IDMS_MONITOR_NL,
/* pl */  IDMS_MONITOR_PL,
/* sv */  IDMS_MONITOR_SV},

/* en */ {IDMS_HARD,
/* de */  IDMS_HARD_DE,
/* fr */  IDMS_HARD_FR,
/* it */  IDMS_HARD_IT,
/* nl */  IDMS_HARD_NL,
/* pl */  IDMS_HARD_PL,
/* sv */  IDMS_HARD_SV},

/* en */ {IDMS_SOFT,
/* de */  IDMS_SOFT_DE,
/* fr */  IDMS_SOFT_FR,
/* it */  IDMS_SOFT_IT,
/* nl */  IDMS_SOFT_NL,
/* pl */  IDMS_SOFT_PL,
/* sv */  IDMS_SOFT_SV},

/* en */ {IDMS_EXIT,
/* de */  IDMS_EXIT_DE,
/* fr */  IDMS_EXIT_FR,
/* it */  IDMS_EXIT_IT,
/* nl */  IDMS_EXIT_NL,
/* pl */  IDMS_EXIT_PL,
/* sv */  IDMS_EXIT_SV},

/* en */ {IDMS_SNAPSHOT,
/* de */  IDMS_SNAPSHOT_DE,
/* fr */  IDMS_SNAPSHOT_FR,
/* it */  IDMS_SNAPSHOT_IT,
/* nl */  IDMS_SNAPSHOT_NL,
/* pl */  IDMS_SNAPSHOT_PL,
/* sv */  IDMS_SNAPSHOT_SV},

/* en */ {IDMS_LOAD_SNAPSHOT_IMAGE,
/* de */  IDMS_LOAD_SNAPSHOT_IMAGE_DE,
/* fr */  IDMS_LOAD_SNAPSHOT_IMAGE_FR,
/* it */  IDMS_LOAD_SNAPSHOT_IMAGE_IT,
/* nl */  IDMS_LOAD_SNAPSHOT_IMAGE_NL,
/* pl */  IDMS_LOAD_SNAPSHOT_IMAGE_PL,
/* sv */  IDMS_LOAD_SNAPSHOT_IMAGE_SV},

/* en */ {IDMS_SAVE_SNAPSHOT_IMAGE,
/* de */  IDMS_SAVE_SNAPSHOT_IMAGE_DE,
/* fr */  IDMS_SAVE_SNAPSHOT_IMAGE_FR,
/* it */  IDMS_SAVE_SNAPSHOT_IMAGE_IT,
/* nl */  IDMS_SAVE_SNAPSHOT_IMAGE_NL,
/* pl */  IDMS_SAVE_SNAPSHOT_IMAGE_PL,
/* sv */  IDMS_SAVE_SNAPSHOT_IMAGE_SV},

/* en */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE,
/* de */  IDMS_LOAD_QUICKSNAPSHOT_IMAGE_DE,
/* fr */  IDMS_LOAD_QUICKSNAPSHOT_IMAGE_FR,
/* it */  IDMS_LOAD_QUICKSNAPSHOT_IMAGE_IT,
/* nl */  IDMS_LOAD_QUICKSNAPSHOT_IMAGE_NL,
/* pl */  IDMS_LOAD_QUICKSNAPSHOT_IMAGE_PL,
/* sv */  IDMS_LOAD_QUICKSNAPSHOT_IMAGE_SV},

/* en */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE,
/* de */  IDMS_SAVE_QUICKSNAPSHOT_IMAGE_DE,
/* fr */  IDMS_SAVE_QUICKSNAPSHOT_IMAGE_FR,
/* it */  IDMS_SAVE_QUICKSNAPSHOT_IMAGE_IT,
/* nl */  IDMS_SAVE_QUICKSNAPSHOT_IMAGE_NL,
/* pl */  IDMS_SAVE_QUICKSNAPSHOT_IMAGE_PL,
/* sv */  IDMS_SAVE_QUICKSNAPSHOT_IMAGE_SV},

/* en */ {IDMS_START_STOP_RECORDING,
/* de */  IDMS_START_STOP_RECORDING_DE,
/* fr */  IDMS_START_STOP_RECORDING_FR,
/* it */  IDMS_START_STOP_RECORDING_IT,
/* nl */  IDMS_START_STOP_RECORDING_NL,
/* pl */  IDMS_START_STOP_RECORDING_PL,
/* sv */  IDMS_START_STOP_RECORDING_SV},

/* en */ {IDMS_START_STOP_PLAYBACK,
/* de */  IDMS_START_STOP_PLAYBACK_DE,
/* fr */  IDMS_START_STOP_PLAYBACK_FR,
/* it */  IDMS_START_STOP_PLAYBACK_IT,
/* nl */  IDMS_START_STOP_PLAYBACK_NL,
/* pl */  IDMS_START_STOP_PLAYBACK_PL,
/* sv */  IDMS_START_STOP_PLAYBACK_SV},

/* en */ {IDMS_SET_MILESTONE,
/* de */  IDMS_SET_MILESTONE_DE,
/* fr */  IDMS_SET_MILESTONE_FR,
/* it */  IDMS_SET_MILESTONE_IT,
/* nl */  IDMS_SET_MILESTONE_NL,
/* pl */  IDMS_SET_MILESTONE_PL,
/* sv */  IDMS_SET_MILESTONE_SV},

/* en */ {IDMS_RETURN_TO_MILESTONE,
/* de */  IDMS_RETURN_TO_MILESTONE_DE,
/* fr */  IDMS_RETURN_TO_MILESTONE_FR,
/* it */  IDMS_RETURN_TO_MILESTONE_IT,
/* nl */  IDMS_RETURN_TO_MILESTONE_NL,
/* pl */  IDMS_RETURN_TO_MILESTONE_PL,
/* sv */  IDMS_RETURN_TO_MILESTONE_SV},

/* en */ {IDMS_RECORDING_START_MODE,
/* de */  IDMS_RECORDING_START_MODE_DE,
/* fr */  IDMS_RECORDING_START_MODE_FR,
/* it */  IDMS_RECORDING_START_MODE_IT,
/* nl */  IDMS_RECORDING_START_MODE_NL,
/* pl */  IDMS_RECORDING_START_MODE_PL,
/* sv */  IDMS_RECORDING_START_MODE_SV},

/* en */ {IDMS_SAVE_NEW_SNAPSHOT,
/* de */  IDMS_SAVE_NEW_SNAPSHOT_DE,
/* fr */  IDMS_SAVE_NEW_SNAPSHOT_FR,
/* it */  IDMS_SAVE_NEW_SNAPSHOT_IT,
/* nl */  IDMS_SAVE_NEW_SNAPSHOT_NL,
/* pl */  IDMS_SAVE_NEW_SNAPSHOT_PL,
/* sv */  IDMS_SAVE_NEW_SNAPSHOT_SV},

/* en */ {IDMS_LOAD_EXISTING_SNAPSHOT,
/* de */  IDMS_LOAD_EXISTING_SNAPSHOT_DE,
/* fr */  IDMS_LOAD_EXISTING_SNAPSHOT_FR,
/* it */  IDMS_LOAD_EXISTING_SNAPSHOT_IT,
/* nl */  IDMS_LOAD_EXISTING_SNAPSHOT_NL,
/* pl */  IDMS_LOAD_EXISTING_SNAPSHOT_PL,
/* sv */  IDMS_LOAD_EXISTING_SNAPSHOT_SV},

/* en */ {IDMS_START_WITH_RESET,
/* de */  IDMS_START_WITH_RESET_DE,
/* fr */  IDMS_START_WITH_RESET_FR,
/* it */  IDMS_START_WITH_RESET_IT,
/* nl */  IDMS_START_WITH_RESET_NL,
/* pl */  IDMS_START_WITH_RESET_PL,
/* sv */  IDMS_START_WITH_RESET_SV},

/* en */ {IDMS_OVERWRITE_PLAYBACK,
/* de */  IDMS_OVERWRITE_PLAYBACK_DE,
/* fr */  IDMS_OVERWRITE_PLAYBACK_FR,
/* it */  IDMS_OVERWRITE_PLAYBACK_IT,
/* nl */  IDMS_OVERWRITE_PLAYBACK_NL,
/* pl */  IDMS_OVERWRITE_PLAYBACK_PL,
/* sv */  IDMS_OVERWRITE_PLAYBACK_SV},

/* en */ {IDMS_SELECT_HISTORY_DIR,
/* de */  IDMS_SELECT_HISTORY_DIR_DE,
/* fr */  IDMS_SELECT_HISTORY_DIR_FR,
/* it */  IDMS_SELECT_HISTORY_DIR_IT,
/* nl */  IDMS_SELECT_HISTORY_DIR_NL,
/* pl */  IDMS_SELECT_HISTORY_DIR_PL,
/* sv */  IDMS_SELECT_HISTORY_DIR_SV},

/* en */ {IDMS_SAVE_STOP_MEDIA_FILE,
/* de */  IDMS_SAVE_STOP_MEDIA_FILE_DE,
/* fr */  IDMS_SAVE_STOP_MEDIA_FILE_FR,
/* it */  IDMS_SAVE_STOP_MEDIA_FILE_IT,
/* nl */  IDMS_SAVE_STOP_MEDIA_FILE_NL,
/* pl */  IDMS_SAVE_STOP_MEDIA_FILE_PL,
/* sv */  IDMS_SAVE_STOP_MEDIA_FILE_SV},

/* en */ {IDMS_NETPLAY,
/* de */  IDMS_NETPLAY_DE,
/* fr */  IDMS_NETPLAY_FR,
/* it */  IDMS_NETPLAY_IT,
/* nl */  IDMS_NETPLAY_NL,
/* pl */  IDMS_NETPLAY_PL,
/* sv */  IDMS_NETPLAY_SV},

/* en */ {IDMS_OPTIONS,
/* de */  IDMS_OPTIONS_DE,
/* fr */  IDMS_OPTIONS_FR,
/* it */  IDMS_OPTIONS_IT,
/* nl */  IDMS_OPTIONS_NL,
/* pl */  IDMS_OPTIONS_PL,
/* sv */  IDMS_OPTIONS_SV},

/* en */ {IDMS_REFRESH_RATE,
/* de */  IDMS_REFRESH_RATE_DE,
/* fr */  IDMS_REFRESH_RATE_FR,
/* it */  IDMS_REFRESH_RATE_IT,
/* nl */  IDMS_REFRESH_RATE_NL,
/* pl */  IDMS_REFRESH_RATE_PL,
/* sv */  IDMS_REFRESH_RATE_SV},

/* en */ {IDMS_AUTO,
/* de */  IDMS_AUTO_DE,
/* fr */  IDMS_AUTO_FR,
/* it */  IDMS_AUTO_IT,
/* nl */  IDMS_AUTO_NL,
/* pl */  IDMS_AUTO_PL,
/* sv */  IDMS_AUTO_SV},

/* en */ {IDMS_1_1,
/* de */  IDMS_1_1_DE,
/* fr */  IDMS_1_1_FR,
/* it */  IDMS_1_1_IT,
/* nl */  IDMS_1_1_NL,
/* pl */  IDMS_1_1_PL,
/* sv */  IDMS_1_1_SV},

/* en */ {IDMS_1_2,
/* de */  IDMS_1_2_DE,
/* fr */  IDMS_1_2_FR,
/* it */  IDMS_1_2_IT,
/* nl */  IDMS_1_2_NL,
/* pl */  IDMS_1_2_PL,
/* sv */  IDMS_1_2_SV},

/* en */ {IDMS_1_3,
/* de */  IDMS_1_3_DE,
/* fr */  IDMS_1_3_FR,
/* it */  IDMS_1_3_IT,
/* nl */  IDMS_1_3_NL,
/* pl */  IDMS_1_3_PL,
/* sv */  IDMS_1_3_SV},

/* en */ {IDMS_1_4,
/* de */  IDMS_1_4_DE,
/* fr */  IDMS_1_4_FR,
/* it */  IDMS_1_4_IT,
/* nl */  IDMS_1_4_NL,
/* pl */  IDMS_1_4_PL,
/* sv */  IDMS_1_4_SV},

/* en */ {IDMS_1_5,
/* de */  IDMS_1_5_DE,
/* fr */  IDMS_1_5_FR,
/* it */  IDMS_1_5_IT,
/* nl */  IDMS_1_5_NL,
/* pl */  IDMS_1_5_PL,
/* sv */  IDMS_1_5_SV},

/* en */ {IDMS_1_6,
/* de */  IDMS_1_6_DE,
/* fr */  IDMS_1_6_FR,
/* it */  IDMS_1_6_IT,
/* nl */  IDMS_1_6_NL,
/* pl */  IDMS_1_6_PL,
/* sv */  IDMS_1_6_SV},

/* en */ {IDMS_1_7,
/* de */  IDMS_1_7_DE,
/* fr */  IDMS_1_7_FR,
/* it */  IDMS_1_7_IT,
/* nl */  IDMS_1_7_NL,
/* pl */  IDMS_1_7_PL,
/* sv */  IDMS_1_7_SV},

/* en */ {IDMS_1_8,
/* de */  IDMS_1_8_DE,
/* fr */  IDMS_1_8_FR,
/* it */  IDMS_1_8_IT,
/* nl */  IDMS_1_8_NL,
/* pl */  IDMS_1_8_PL,
/* sv */  IDMS_1_8_SV},

/* en */ {IDMS_1_9,
/* de */  IDMS_1_9_DE,
/* fr */  IDMS_1_9_FR,
/* it */  IDMS_1_9_IT,
/* nl */  IDMS_1_9_NL,
/* pl */  IDMS_1_9_PL,
/* sv */  IDMS_1_9_SV},

/* en */ {IDMS_1_10,
/* de */  IDMS_1_10_DE,
/* fr */  IDMS_1_10_FR,
/* it */  IDMS_1_10_IT,
/* nl */  IDMS_1_10_NL,
/* pl */  IDMS_1_10_PL,
/* sv */  IDMS_1_10_SV},

/* en */ {IDMS_MAXIMUM_SPEED,
/* de */  IDMS_MAXIMUM_SPEED_DE,
/* fr */  IDMS_MAXIMUM_SPEED_FR,
/* it */  IDMS_MAXIMUM_SPEED_IT,
/* nl */  IDMS_MAXIMUM_SPEED_NL,
/* pl */  IDMS_MAXIMUM_SPEED_PL,
/* sv */  IDMS_MAXIMUM_SPEED_SV},

/* en */ {IDMS_200_PERCENT,
/* de */  IDMS_200_PERCENT_DE,
/* fr */  IDMS_200_PERCENT_FR,
/* it */  IDMS_200_PERCENT_IT,
/* nl */  IDMS_200_PERCENT_NL,
/* pl */  IDMS_200_PERCENT_PL,
/* sv */  IDMS_200_PERCENT_SV},

/* en */ {IDMS_100_PERCENT,
/* de */  IDMS_100_PERCENT_DE,
/* fr */  IDMS_100_PERCENT_FR,
/* it */  IDMS_100_PERCENT_IT,
/* nl */  IDMS_100_PERCENT_NL,
/* pl */  IDMS_100_PERCENT_PL,
/* sv */  IDMS_100_PERCENT_SV},

/* en */ {IDMS_50_PERCENT,
/* de */  IDMS_50_PERCENT_DE,
/* fr */  IDMS_50_PERCENT_FR,
/* it */  IDMS_50_PERCENT_IT,
/* nl */  IDMS_50_PERCENT_NL,
/* pl */  IDMS_50_PERCENT_PL,
/* sv */  IDMS_50_PERCENT_SV},

/* en */ {IDMS_20_PERCENT,
/* de */  IDMS_20_PERCENT_DE,
/* fr */  IDMS_20_PERCENT_FR,
/* it */  IDMS_20_PERCENT_IT,
/* nl */  IDMS_20_PERCENT_NL,
/* pl */  IDMS_20_PERCENT_PL,
/* sv */  IDMS_20_PERCENT_SV},

/* en */ {IDMS_10_PERCENT,
/* de */  IDMS_10_PERCENT_DE,
/* fr */  IDMS_10_PERCENT_FR,
/* it */  IDMS_10_PERCENT_IT,
/* nl */  IDMS_10_PERCENT_NL,
/* pl */  IDMS_10_PERCENT_PL,
/* sv */  IDMS_10_PERCENT_SV},

/* en */ {IDMS_NO_LIMIT,
/* de */  IDMS_NO_LIMIT_DE,
/* fr */  IDMS_NO_LIMIT_FR,
/* it */  IDMS_NO_LIMIT_IT,
/* nl */  IDMS_NO_LIMIT_NL,
/* pl */  IDMS_NO_LIMIT_PL,
/* sv */  IDMS_NO_LIMIT_SV},

/* en */ {IDMS_CUSTOM,
/* de */  IDMS_CUSTOM_DE,
/* fr */  IDMS_CUSTOM_FR,
/* it */  IDMS_CUSTOM_IT,
/* nl */  IDMS_CUSTOM_NL,
/* pl */  IDMS_CUSTOM_PL,
/* sv */  IDMS_CUSTOM_SV},

/* en */ {IDMS_WARP_MODE,
/* de */  IDMS_WARP_MODE_DE,
/* fr */  IDMS_WARP_MODE_FR,
/* it */  IDMS_WARP_MODE_IT,
/* nl */  IDMS_WARP_MODE_NL,
/* pl */  IDMS_WARP_MODE_PL,
/* sv */  IDMS_WARP_MODE_SV},

/* en */ {IDMS_FULLSCREEN,
/* de */  IDMS_FULLSCREEN_DE,
/* fr */  IDMS_FULLSCREEN_FR,
/* it */  IDMS_FULLSCREEN_IT,
/* nl */  IDMS_FULLSCREEN_NL,
/* pl */  IDMS_FULLSCREEN_PL,
/* sv */  IDMS_FULLSCREEN_SV},

/* en */ {IDMS_VIDEO_CACHE,
/* de */  IDMS_VIDEO_CACHE_DE,
/* fr */  IDMS_VIDEO_CACHE_FR,
/* it */  IDMS_VIDEO_CACHE_IT,
/* nl */  IDMS_VIDEO_CACHE_NL,
/* pl */  IDMS_VIDEO_CACHE_PL,
/* sv */  IDMS_VIDEO_CACHE_SV},

/* en */ {IDMS_DOUBLE_SIZE,
/* de */  IDMS_DOUBLE_SIZE_DE,
/* fr */  IDMS_DOUBLE_SIZE_FR,
/* it */  IDMS_DOUBLE_SIZE_IT,
/* nl */  IDMS_DOUBLE_SIZE_NL,
/* pl */  IDMS_DOUBLE_SIZE_PL,
/* sv */  IDMS_DOUBLE_SIZE_SV},

/* en */ {IDMS_DOUBLE_SCAN,
/* de */  IDMS_DOUBLE_SCAN_DE,
/* fr */  IDMS_DOUBLE_SCAN_FR,
/* it */  IDMS_DOUBLE_SCAN_IT,
/* nl */  IDMS_DOUBLE_SCAN_NL,
/* pl */  IDMS_DOUBLE_SCAN_PL,
/* sv */  IDMS_DOUBLE_SCAN_SV},

/* en */ {IDMS_PAL_EMULATION,
/* de */  IDMS_PAL_EMULATION_DE,
/* fr */  IDMS_PAL_EMULATION_FR,
/* it */  IDMS_PAL_EMULATION_IT,
/* nl */  IDMS_PAL_EMULATION_NL,
/* pl */  IDMS_PAL_EMULATION_PL,
/* sv */  IDMS_PAL_EMULATION_SV},

/* en */ {IDMS_SCALE2X,
/* de */  IDMS_SCALE2X_DE,
/* fr */  IDMS_SCALE2X_FR,
/* it */  IDMS_SCALE2X_IT,
/* nl */  IDMS_SCALE2X_NL,
/* pl */  IDMS_SCALE2X_PL,
/* sv */  IDMS_SCALE2X_SV},

/* en */ {IDMS_VDC_SETTINGS,
/* de */  IDMS_VDC_SETTINGS_DE,
/* fr */  IDMS_VDC_SETTINGS_FR,
/* it */  IDMS_VDC_SETTINGS_IT,
/* nl */  IDMS_VDC_SETTINGS_NL,
/* pl */  IDMS_VDC_SETTINGS_PL,
/* sv */  IDMS_VDC_SETTINGS_SV},

/* en */ {IDMS_64KB_VIDEO_MEMORY,
/* de */  IDMS_64KB_VIDEO_MEMORY_DE,
/* fr */  IDMS_64KB_VIDEO_MEMORY_FR,
/* it */  IDMS_64KB_VIDEO_MEMORY_IT,
/* nl */  IDMS_64KB_VIDEO_MEMORY_NL,
/* pl */  IDMS_64KB_VIDEO_MEMORY_PL,
/* sv */  IDMS_64KB_VIDEO_MEMORY_SV},

/* en */ {IDMS_SWAP_JOYSTICKS,
/* de */  IDMS_SWAP_JOYSTICKS_DE,
/* fr */  IDMS_SWAP_JOYSTICKS_FR,
/* it */  IDMS_SWAP_JOYSTICKS_IT,
/* nl */  IDMS_SWAP_JOYSTICKS_NL,
/* pl */  IDMS_SWAP_JOYSTICKS_PL,
/* sv */  IDMS_SWAP_JOYSTICKS_SV},

/* en */ {IDMS_SOUND_PLAYBACK,
/* de */  IDMS_SOUND_PLAYBACK_DE,
/* fr */  IDMS_SOUND_PLAYBACK_FR,
/* it */  IDMS_SOUND_PLAYBACK_IT,
/* nl */  IDMS_SOUND_PLAYBACK_NL,
/* pl */  IDMS_SOUND_PLAYBACK_PL,
/* sv */  IDMS_SOUND_PLAYBACK_SV},

/* en */ {IDMS_TRUE_DRIVE_EMU,
/* de */  IDMS_TRUE_DRIVE_EMU_DE,
/* fr */  IDMS_TRUE_DRIVE_EMU_FR,
/* it */  IDMS_TRUE_DRIVE_EMU_IT,
/* nl */  IDMS_TRUE_DRIVE_EMU_NL,
/* pl */  IDMS_TRUE_DRIVE_EMU_PL,
/* sv */  IDMS_TRUE_DRIVE_EMU_SV},

/* en */ {IDMS_VIRTUAL_DEVICE_TRAPS,
/* de */  IDMS_VIRTUAL_DEVICE_TRAPS_DE,
/* fr */  IDMS_VIRTUAL_DEVICE_TRAPS_FR,
/* it */  IDMS_VIRTUAL_DEVICE_TRAPS_IT,
/* nl */  IDMS_VIRTUAL_DEVICE_TRAPS_NL,
/* pl */  IDMS_VIRTUAL_DEVICE_TRAPS_PL,
/* sv */  IDMS_VIRTUAL_DEVICE_TRAPS_SV},

/* en */ {IDMS_DRIVE_SYNC_FACTOR,
/* de */  IDMS_DRIVE_SYNC_FACTOR_DE,
/* fr */  IDMS_DRIVE_SYNC_FACTOR_FR,
/* it */  IDMS_DRIVE_SYNC_FACTOR_IT,
/* nl */  IDMS_DRIVE_SYNC_FACTOR_NL,
/* pl */  IDMS_DRIVE_SYNC_FACTOR_PL,
/* sv */  IDMS_DRIVE_SYNC_FACTOR_SV},

/* en */ {IDMS_PAL,
/* de */  IDMS_PAL_DE,
/* fr */  IDMS_PAL_FR,
/* it */  IDMS_PAL_IT,
/* nl */  IDMS_PAL_NL,
/* pl */  IDMS_PAL_PL,
/* sv */  IDMS_PAL_SV},

/* en */ {IDMS_NTSC,
/* de */  IDMS_NTSC_DE,
/* fr */  IDMS_NTSC_FR,
/* it */  IDMS_NTSC_IT,
/* nl */  IDMS_NTSC_NL,
/* pl */  IDMS_NTSC_PL,
/* sv */  IDMS_NTSC_SV},

/* en */ {IDMS_VIDEO_STANDARD,
/* de */  IDMS_VIDEO_STANDARD_DE,
/* fr */  IDMS_VIDEO_STANDARD_FR,
/* it */  IDMS_VIDEO_STANDARD_IT,
/* nl */  IDMS_VIDEO_STANDARD_NL,
/* pl */  IDMS_VIDEO_STANDARD_PL,
/* sv */  IDMS_VIDEO_STANDARD_SV},

/* en */ {IDMS_PAL_G,
/* de */  IDMS_PAL_G_DE,
/* fr */  IDMS_PAL_G_FR,
/* it */  IDMS_PAL_G_IT,
/* nl */  IDMS_PAL_G_NL,
/* pl */  IDMS_PAL_G_PL,
/* sv */  IDMS_PAL_G_SV},

/* en */ {IDMS_NTSC_M,
/* de */  IDMS_NTSC_M_DE,
/* fr */  IDMS_NTSC_M_FR,
/* it */  IDMS_NTSC_M_IT,
/* nl */  IDMS_NTSC_M_NL,
/* pl */  IDMS_NTSC_M_PL,
/* sv */  IDMS_NTSC_M_SV},

/* en */ {IDMS_OLD_NTSC_M,
/* de */  IDMS_OLD_NTSC_M_DE,
/* fr */  IDMS_OLD_NTSC_M_FR,
/* it */  IDMS_OLD_NTSC_M_IT,
/* nl */  IDMS_OLD_NTSC_M_NL,
/* pl */  IDMS_OLD_NTSC_M_PL,
/* sv */  IDMS_OLD_NTSC_M_SV},

/* en */ {IDMS_EMU_ID,
/* de */  IDMS_EMU_ID_DE,
/* fr */  IDMS_EMU_ID_FR,
/* it */  IDMS_EMU_ID_IT,
/* nl */  IDMS_EMU_ID_NL,
/* pl */  IDMS_EMU_ID_PL,
/* sv */  IDMS_EMU_ID_SV},

/* en */ {IDMS_VIC_1112_IEEE_488,
/* de */  IDMS_VIC_1112_IEEE_488_DE,
/* fr */  IDMS_VIC_1112_IEEE_488_FR,
/* it */  IDMS_VIC_1112_IEEE_488_IT,
/* nl */  IDMS_VIC_1112_IEEE_488_NL,
/* pl */  IDMS_VIC_1112_IEEE_488_PL,
/* sv */  IDMS_VIC_1112_IEEE_488_SV},

/* en */ {IDMS_IEEE_488_INTERFACE,
/* de */  IDMS_IEEE_488_INTERFACE_DE,
/* fr */  IDMS_IEEE_488_INTERFACE_FR,
/* it */  IDMS_IEEE_488_INTERFACE_IT,
/* nl */  IDMS_IEEE_488_INTERFACE_NL,
/* pl */  IDMS_IEEE_488_INTERFACE_PL,
/* sv */  IDMS_IEEE_488_INTERFACE_SV},

/* en */ {IDMS_1351_MOUSE,
/* de */  IDMS_1351_MOUSE_DE,
/* fr */  IDMS_1351_MOUSE_FR,
/* it */  IDMS_1351_MOUSE_IT,
/* nl */  IDMS_1351_MOUSE_NL,
/* pl */  IDMS_1351_MOUSE_PL,
/* sv */  IDMS_1351_MOUSE_SV},

/* en */ {IDMS_SETTINGS,
/* de */  IDMS_SETTINGS_DE,
/* fr */  IDMS_SETTINGS_FR,
/* it */  IDMS_SETTINGS_IT,
/* nl */  IDMS_SETTINGS_NL,
/* pl */  IDMS_SETTINGS_PL,
/* sv */  IDMS_SETTINGS_SV},

/* en */ {IDMS_VIDEO_SETTINGS,
/* de */  IDMS_VIDEO_SETTINGS_DE,
/* fr */  IDMS_VIDEO_SETTINGS_FR,
/* it */  IDMS_VIDEO_SETTINGS_IT,
/* nl */  IDMS_VIDEO_SETTINGS_NL,
/* pl */  IDMS_VIDEO_SETTINGS_PL,
/* sv */  IDMS_VIDEO_SETTINGS_SV},

/* en */ {IDMS_VIC_SETTINGS,
/* de */  IDMS_VIC_SETTINGS_DE,
/* fr */  IDMS_VIC_SETTINGS_FR,
/* it */  IDMS_VIC_SETTINGS_IT,
/* nl */  IDMS_VIC_SETTINGS_NL,
/* pl */  IDMS_VIC_SETTINGS_PL,
/* sv */  IDMS_VIC_SETTINGS_SV},

/* en */ {IDMS_CBM2_SETTINGS,
/* de */  IDMS_CBM2_SETTINGS_DE,
/* fr */  IDMS_CBM2_SETTINGS_FR,
/* it */  IDMS_CBM2_SETTINGS_IT,
/* nl */  IDMS_CBM2_SETTINGS_NL,
/* pl */  IDMS_CBM2_SETTINGS_PL,
/* sv */  IDMS_CBM2_SETTINGS_SV},

/* en */ {IDMS_PET_SETTINGS,
/* de */  IDMS_PET_SETTINGS_DE,
/* fr */  IDMS_PET_SETTINGS_FR,
/* it */  IDMS_PET_SETTINGS_IT,
/* nl */  IDMS_PET_SETTINGS_NL,
/* pl */  IDMS_PET_SETTINGS_PL,
/* sv */  IDMS_PET_SETTINGS_SV},

/* en */ {IDMS_PERIPHERAL_SETTINGS,
/* de */  IDMS_PERIPHERAL_SETTINGS_DE,
/* fr */  IDMS_PERIPHERAL_SETTINGS_FR,
/* it */  IDMS_PERIPHERAL_SETTINGS_IT,
/* nl */  IDMS_PERIPHERAL_SETTINGS_NL,
/* pl */  IDMS_PERIPHERAL_SETTINGS_PL,
/* sv */  IDMS_PERIPHERAL_SETTINGS_SV},

/* en */ {IDMS_DRIVE_SETTINGS,
/* de */  IDMS_DRIVE_SETTINGS_DE,
/* fr */  IDMS_DRIVE_SETTINGS_FR,
/* it */  IDMS_DRIVE_SETTINGS_IT,
/* nl */  IDMS_DRIVE_SETTINGS_NL,
/* pl */  IDMS_DRIVE_SETTINGS_PL,
/* sv */  IDMS_DRIVE_SETTINGS_SV},

/* en */ {IDMS_DATASETTE_SETTINGS,
/* de */  IDMS_DATASETTE_SETTINGS_DE,
/* fr */  IDMS_DATASETTE_SETTINGS_FR,
/* it */  IDMS_DATASETTE_SETTINGS_IT,
/* nl */  IDMS_DATASETTE_SETTINGS_NL,
/* pl */  IDMS_DATASETTE_SETTINGS_PL,
/* sv */  IDMS_DATASETTE_SETTINGS_SV},

/* en */ {IDMS_PLUS4_SETTINGS,
/* de */  IDMS_PLUS4_SETTINGS_DE,
/* fr */  IDMS_PLUS4_SETTINGS_FR,
/* it */  IDMS_PLUS4_SETTINGS_IT,
/* nl */  IDMS_PLUS4_SETTINGS_NL,
/* pl */  IDMS_PLUS4_SETTINGS_PL,
/* sv */  IDMS_PLUS4_SETTINGS_SV},

/* en */ {IDMS_VICII_SETTINGS,
/* de */  IDMS_VICII_SETTINGS_DE,
/* fr */  IDMS_VICII_SETTINGS_FR,
/* it */  IDMS_VICII_SETTINGS_IT,
/* nl */  IDMS_VICII_SETTINGS_NL,
/* pl */  IDMS_VICII_SETTINGS_PL,
/* sv */  IDMS_VICII_SETTINGS_SV},

/* en */ {IDMS_JOYSTICK_SETTINGS,
/* de */  IDMS_JOYSTICK_SETTINGS_DE,
/* fr */  IDMS_JOYSTICK_SETTINGS_FR,
/* it */  IDMS_JOYSTICK_SETTINGS_IT,
/* nl */  IDMS_JOYSTICK_SETTINGS_NL,
/* pl */  IDMS_JOYSTICK_SETTINGS_PL,
/* sv */  IDMS_JOYSTICK_SETTINGS_SV},

/* en */ {IDMS_JOYSTICK_DEVICE_SELECT,
/* de */  IDMS_JOYSTICK_DEVICE_SELECT_DE,
/* fr */  IDMS_JOYSTICK_DEVICE_SELECT_FR,
/* it */  IDMS_JOYSTICK_DEVICE_SELECT_IT,
/* nl */  IDMS_JOYSTICK_DEVICE_SELECT_NL,
/* pl */  IDMS_JOYSTICK_DEVICE_SELECT_PL,
/* sv */  IDMS_JOYSTICK_DEVICE_SELECT_SV},

/* en */ {IDMS_JOYSTICK_FIRE_SELECT,
/* de */  IDMS_JOYSTICK_FIRE_SELECT_DE,
/* fr */  IDMS_JOYSTICK_FIRE_SELECT_FR,
/* it */  IDMS_JOYSTICK_FIRE_SELECT_IT,
/* nl */  IDMS_JOYSTICK_FIRE_SELECT_NL,
/* pl */  IDMS_JOYSTICK_FIRE_SELECT_PL,
/* sv */  IDMS_JOYSTICK_FIRE_SELECT_SV},

/* en */ {IDMS_KEYBOARD_SETTINGS,
/* de */  IDMS_KEYBOARD_SETTINGS_DE,
/* fr */  IDMS_KEYBOARD_SETTINGS_FR,
/* it */  IDMS_KEYBOARD_SETTINGS_IT,
/* nl */  IDMS_KEYBOARD_SETTINGS_NL,
/* pl */  IDMS_KEYBOARD_SETTINGS_PL,
/* sv */  IDMS_KEYBOARD_SETTINGS_SV},

/* en */ {IDMS_SOUND_SETTINGS,
/* de */  IDMS_SOUND_SETTINGS_DE,
/* fr */  IDMS_SOUND_SETTINGS_FR,
/* it */  IDMS_SOUND_SETTINGS_IT,
/* nl */  IDMS_SOUND_SETTINGS_NL,
/* pl */  IDMS_SOUND_SETTINGS_PL,
/* sv */  IDMS_SOUND_SETTINGS_SV},

/* en */ {IDMS_SID_SETTINGS,
/* de */  IDMS_SID_SETTINGS_DE,
/* fr */  IDMS_SID_SETTINGS_FR,
/* it */  IDMS_SID_SETTINGS_IT,
/* nl */  IDMS_SID_SETTINGS_NL,
/* pl */  IDMS_SID_SETTINGS_PL,
/* sv */  IDMS_SID_SETTINGS_SV},

/* en */ {IDMS_ROM_SETTINGS,
/* de */  IDMS_ROM_SETTINGS_DE,
/* fr */  IDMS_ROM_SETTINGS_FR,
/* it */  IDMS_ROM_SETTINGS_IT,
/* nl */  IDMS_ROM_SETTINGS_NL,
/* pl */  IDMS_ROM_SETTINGS_PL,
/* sv */  IDMS_ROM_SETTINGS_SV},

/* en */ {IDMS_RAM_SETTINGS,
/* de */  IDMS_RAM_SETTINGS_DE,
/* fr */  IDMS_RAM_SETTINGS_FR,
/* it */  IDMS_RAM_SETTINGS_IT,
/* nl */  IDMS_RAM_SETTINGS_NL,
/* pl */  IDMS_RAM_SETTINGS_PL,
/* sv */  IDMS_RAM_SETTINGS_SV},

/* en */ {IDMS_RS232_SETTINGS,
/* de */  IDMS_RS232_SETTINGS_DE,
/* fr */  IDMS_RS232_SETTINGS_FR,
/* it */  IDMS_RS232_SETTINGS_IT,
/* nl */  IDMS_RS232_SETTINGS_NL,
/* pl */  IDMS_RS232_SETTINGS_PL,
/* sv */  IDMS_RS232_SETTINGS_SV},

/* en */ {IDMS_C128_SETTINGS,
/* de */  IDMS_C128_SETTINGS_DE,
/* fr */  IDMS_C128_SETTINGS_FR,
/* it */  IDMS_C128_SETTINGS_IT,
/* nl */  IDMS_C128_SETTINGS_NL,
/* pl */  IDMS_C128_SETTINGS_PL,
/* sv */  IDMS_C128_SETTINGS_SV},

/* en */ {IDMS_CART_IO_SETTINGS,
/* de */  IDMS_CART_IO_SETTINGS_DE,
/* fr */  IDMS_CART_IO_SETTINGS_FR,
/* it */  IDMS_CART_IO_SETTINGS_IT,
/* nl */  IDMS_CART_IO_SETTINGS_NL,
/* pl */  IDMS_CART_IO_SETTINGS_PL,
/* sv */  IDMS_CART_IO_SETTINGS_SV},

/* en */ {IDMS_REU_SETTINGS,
/* de */  IDMS_REU_SETTINGS_DE,
/* fr */  IDMS_REU_SETTINGS_FR,
/* it */  IDMS_REU_SETTINGS_IT,
/* nl */  IDMS_REU_SETTINGS_NL,
/* pl */  IDMS_REU_SETTINGS_PL,
/* sv */  IDMS_REU_SETTINGS_SV},

/* en */ {IDMS_GEORAM_SETTINGS,
/* de */  IDMS_GEORAM_SETTINGS_DE,
/* fr */  IDMS_GEORAM_SETTINGS_FR,
/* it */  IDMS_GEORAM_SETTINGS_IT,
/* nl */  IDMS_GEORAM_SETTINGS_NL,
/* pl */  IDMS_GEORAM_SETTINGS_PL,
/* sv */  IDMS_GEORAM_SETTINGS_SV},

/* en */ {IDMS_RAMCART_SETTINGS,
/* de */  IDMS_RAMCART_SETTINGS_DE,
/* fr */  IDMS_RAMCART_SETTINGS_FR,
/* it */  IDMS_RAMCART_SETTINGS_IT,
/* nl */  IDMS_RAMCART_SETTINGS_NL,
/* pl */  IDMS_RAMCART_SETTINGS_PL,
/* sv */  IDMS_RAMCART_SETTINGS_SV},

/* en */ {IDMS_PLUS60K_SETTINGS,
/* de */  IDMS_PLUS60K_SETTINGS_DE,
/* fr */  IDMS_PLUS60K_SETTINGS_FR,
/* it */  IDMS_PLUS60K_SETTINGS_IT,
/* nl */  IDMS_PLUS60K_SETTINGS_NL,
/* pl */  IDMS_PLUS60K_SETTINGS_PL,
/* sv */  IDMS_PLUS60K_SETTINGS_SV},

/* en */ {IDMS_PLUS256K_SETTINGS,
/* de */  IDMS_PLUS256K_SETTINGS_DE,
/* fr */  IDMS_PLUS256K_SETTINGS_FR,
/* it */  IDMS_PLUS256K_SETTINGS_IT,
/* nl */  IDMS_PLUS256K_SETTINGS_NL,
/* pl */  IDMS_PLUS256K_SETTINGS_PL,
/* sv */  IDMS_PLUS256K_SETTINGS_SV},

/* en */ {IDMS_256K_SETTINGS,
/* de */  IDMS_256K_SETTINGS_DE,
/* fr */  IDMS_256K_SETTINGS_FR,
/* it */  IDMS_256K_SETTINGS_IT,
/* nl */  IDMS_256K_SETTINGS_NL,
/* pl */  IDMS_256K_SETTINGS_PL,
/* sv */  IDMS_256K_SETTINGS_SV},

/* en */ {IDMS_IDE64_SETTINGS,
/* de */  IDMS_IDE64_SETTINGS_DE,
/* fr */  IDMS_IDE64_SETTINGS_FR,
/* it */  IDMS_IDE64_SETTINGS_IT,
/* nl */  IDMS_IDE64_SETTINGS_NL,
/* pl */  IDMS_IDE64_SETTINGS_PL,
/* sv */  IDMS_IDE64_SETTINGS_SV},

/* en */ {IDMS_ETHERNET_SETTINGS,
/* de */  IDMS_ETHERNET_SETTINGS_DE,
/* fr */  IDMS_ETHERNET_SETTINGS_FR,
/* it */  IDMS_ETHERNET_SETTINGS_IT,
/* nl */  IDMS_ETHERNET_SETTINGS_NL,
/* pl */  IDMS_ETHERNET_SETTINGS_PL,
/* sv */  IDMS_ETHERNET_SETTINGS_SV},

/* en */ {IDMS_ACIA_SETTINGS,
/* de */  IDMS_ACIA_SETTINGS_DE,
/* fr */  IDMS_ACIA_SETTINGS_FR,
/* it */  IDMS_ACIA_SETTINGS_IT,
/* nl */  IDMS_ACIA_SETTINGS_NL,
/* pl */  IDMS_ACIA_SETTINGS_PL,
/* sv */  IDMS_ACIA_SETTINGS_SV},

/* en */ {IDMS_PETREU_SETTINGS,
/* de */  IDMS_PETREU_SETTINGS_DE,
/* fr */  IDMS_PETREU_SETTINGS_FR,
/* it */  IDMS_PETREU_SETTINGS_IT,
/* nl */  IDMS_PETREU_SETTINGS_NL,
/* pl */  IDMS_PETREU_SETTINGS_PL,
/* sv */  IDMS_PETREU_SETTINGS_SV},

/* en */ {IDMS_RS232_USERPORT_SETTINGS,
/* de */  IDMS_RS232_USERPORT_SETTINGS_DE,
/* fr */  IDMS_RS232_USERPORT_SETTINGS_FR,
/* it */  IDMS_RS232_USERPORT_SETTINGS_IT,
/* nl */  IDMS_RS232_USERPORT_SETTINGS_NL,
/* pl */  IDMS_RS232_USERPORT_SETTINGS_PL,
/* sv */  IDMS_RS232_USERPORT_SETTINGS_SV},

/* en */ {IDMS_SAVE_CURRENT_SETTINGS,
/* de */  IDMS_SAVE_CURRENT_SETTINGS_DE,
/* fr */  IDMS_SAVE_CURRENT_SETTINGS_FR,
/* it */  IDMS_SAVE_CURRENT_SETTINGS_IT,
/* nl */  IDMS_SAVE_CURRENT_SETTINGS_NL,
/* pl */  IDMS_SAVE_CURRENT_SETTINGS_PL,
/* sv */  IDMS_SAVE_CURRENT_SETTINGS_SV},

/* en */ {IDMS_LOAD_SAVED_SETTINGS,
/* de */  IDMS_LOAD_SAVED_SETTINGS_DE,
/* fr */  IDMS_LOAD_SAVED_SETTINGS_FR,
/* it */  IDMS_LOAD_SAVED_SETTINGS_IT,
/* nl */  IDMS_LOAD_SAVED_SETTINGS_NL,
/* pl */  IDMS_LOAD_SAVED_SETTINGS_PL,
/* sv */  IDMS_LOAD_SAVED_SETTINGS_SV},

/* en */ {IDMS_SET_DEFAULT_SETTINGS,
/* de */  IDMS_SET_DEFAULT_SETTINGS_DE,
/* fr */  IDMS_SET_DEFAULT_SETTINGS_FR,
/* it */  IDMS_SET_DEFAULT_SETTINGS_IT,
/* nl */  IDMS_SET_DEFAULT_SETTINGS_NL,
/* pl */  IDMS_SET_DEFAULT_SETTINGS_PL,
/* sv */  IDMS_SET_DEFAULT_SETTINGS_SV},

/* en */ {IDMS_SAVE_SETTING_ON_EXIT,
/* de */  IDMS_SAVE_SETTING_ON_EXIT_DE,
/* fr */  IDMS_SAVE_SETTING_ON_EXIT_FR,
/* it */  IDMS_SAVE_SETTING_ON_EXIT_IT,
/* nl */  IDMS_SAVE_SETTING_ON_EXIT_NL,
/* pl */  IDMS_SAVE_SETTING_ON_EXIT_PL,
/* sv */  IDMS_SAVE_SETTING_ON_EXIT_SV},

/* en */ {IDMS_CONFIRM_ON_EXIT,
/* de */  IDMS_CONFIRM_ON_EXIT_DE,
/* fr */  IDMS_CONFIRM_ON_EXIT_FR,
/* it */  IDMS_CONFIRM_ON_EXIT_IT,
/* nl */  IDMS_CONFIRM_ON_EXIT_NL,
/* pl */  IDMS_CONFIRM_ON_EXIT_PL,
/* sv */  IDMS_CONFIRM_ON_EXIT_SV},

/* en */ {IDMS_HELP,
/* de */  IDMS_HELP_DE,
/* fr */  IDMS_HELP_FR,
/* it */  IDMS_HELP_IT,
/* nl */  IDMS_HELP_NL,
/* pl */  IDMS_HELP_PL,
/* sv */  IDMS_HELP_SV},

/* en */ {IDMS_ABOUT,
/* de */  IDMS_ABOUT_DE,
/* fr */  IDMS_ABOUT_FR,
/* it */  IDMS_ABOUT_IT,
/* nl */  IDMS_ABOUT_NL,
/* pl */  IDMS_ABOUT_PL,
/* sv */  IDMS_ABOUT_SV},

/* en */ {IDMS_COMMAND_LINE_OPTIONS,
/* de */  IDMS_COMMAND_LINE_OPTIONS_DE,
/* fr */  IDMS_COMMAND_LINE_OPTIONS_FR,
/* it */  IDMS_COMMAND_LINE_OPTIONS_IT,
/* nl */  IDMS_COMMAND_LINE_OPTIONS_NL,
/* pl */  IDMS_COMMAND_LINE_OPTIONS_PL,
/* sv */  IDMS_COMMAND_LINE_OPTIONS_SV},

/* en */ {IDMS_CONTRIBUTORS,
/* de */  IDMS_CONTRIBUTORS_DE,
/* fr */  IDMS_CONTRIBUTORS_FR,
/* it */  IDMS_CONTRIBUTORS_IT,
/* nl */  IDMS_CONTRIBUTORS_NL,
/* pl */  IDMS_CONTRIBUTORS_PL,
/* sv */  IDMS_CONTRIBUTORS_SV},

/* en */ {IDMS_LICENSE,
/* de */  IDMS_LICENSE_DE,
/* fr */  IDMS_LICENSE_FR,
/* it */  IDMS_LICENSE_IT,
/* nl */  IDMS_LICENSE_NL,
/* pl */  IDMS_LICENSE_PL,
/* sv */  IDMS_LICENSE_SV},

/* en */ {IDMS_NO_WARRANTY,
/* de */  IDMS_NO_WARRANTY_DE,
/* fr */  IDMS_NO_WARRANTY_FR,
/* it */  IDMS_NO_WARRANTY_IT,
/* nl */  IDMS_NO_WARRANTY_NL,
/* pl */  IDMS_NO_WARRANTY_PL,
/* sv */  IDMS_NO_WARRANTY_SV},

/* en */ {IDMS_LANGUAGE_ENGLISH,
/* de */  IDMS_LANGUAGE_ENGLISH_DE,
/* fr */  IDMS_LANGUAGE_ENGLISH_FR,
/* it */  IDMS_LANGUAGE_ENGLISH_IT,
/* nl */  IDMS_LANGUAGE_ENGLISH_NL,
/* pl */  IDMS_LANGUAGE_ENGLISH_PL,
/* sv */  IDMS_LANGUAGE_ENGLISH_SV},

/* en */ {IDMS_LANGUAGE_GERMAN,
/* de */  IDMS_LANGUAGE_GERMAN_DE,
/* fr */  IDMS_LANGUAGE_GERMAN_FR,
/* it */  IDMS_LANGUAGE_GERMAN_IT,
/* nl */  IDMS_LANGUAGE_GERMAN_NL,
/* pl */  IDMS_LANGUAGE_GERMAN_PL,
/* sv */  IDMS_LANGUAGE_GERMAN_SV},

/* en */ {IDMS_LANGUAGE_FRENCH,
/* de */  IDMS_LANGUAGE_FRENCH_DE,
/* fr */  IDMS_LANGUAGE_FRENCH_FR,
/* it */  IDMS_LANGUAGE_FRENCH_IT,
/* nl */  IDMS_LANGUAGE_FRENCH_NL,
/* pl */  IDMS_LANGUAGE_FRENCH_PL,
/* sv */  IDMS_LANGUAGE_FRENCH_SV},

/* en */ {IDMS_LANGUAGE_ITALIAN,
/* de */  IDMS_LANGUAGE_ITALIAN_DE,
/* fr */  IDMS_LANGUAGE_ITALIAN_FR,
/* it */  IDMS_LANGUAGE_ITALIAN_IT,
/* nl */  IDMS_LANGUAGE_ITALIAN_NL,
/* pl */  IDMS_LANGUAGE_ITALIAN_PL,
/* sv */  IDMS_LANGUAGE_ITALIAN_SV},

/* en */ {IDMS_LANGUAGE_DUTCH,
/* de */  IDMS_LANGUAGE_DUTCH_DE,
/* fr */  IDMS_LANGUAGE_DUTCH_FR,
/* it */  IDMS_LANGUAGE_DUTCH_IT,
/* nl */  IDMS_LANGUAGE_DUTCH_NL,
/* pl */  IDMS_LANGUAGE_DUTCH_PL,
/* sv */  IDMS_LANGUAGE_DUTCH_SV},

/* en */ {IDMS_LANGUAGE_POLISH,
/* de */  IDMS_LANGUAGE_POLISH_DE,
/* fr */  IDMS_LANGUAGE_POLISH_FR,
/* it */  IDMS_LANGUAGE_POLISH_IT,
/* nl */  IDMS_LANGUAGE_POLISH_NL,
/* pl */  IDMS_LANGUAGE_POLISH_PL,
/* sv */  IDMS_LANGUAGE_POLISH_SV},

/* en */ {IDMS_LANGUAGE_SWEDISH,
/* de */  IDMS_LANGUAGE_SWEDISH_DE,
/* fr */  IDMS_LANGUAGE_SWEDISH_FR,
/* it */  IDMS_LANGUAGE_SWEDISH_IT,
/* nl */  IDMS_LANGUAGE_SWEDISH_NL,
/* pl */  IDMS_LANGUAGE_SWEDISH_PL,
/* sv */  IDMS_LANGUAGE_SWEDISH_SV},

/* en */ {IDMS_LANGUAGE,
/* de */  IDMS_LANGUAGE_DE,
/* fr */  IDMS_LANGUAGE_FR,
/* it */  IDMS_LANGUAGE_IT,
/* nl */  IDMS_LANGUAGE_NL,
/* pl */  IDMS_LANGUAGE_PL,
/* sv */  IDMS_LANGUAGE_SV},

/* ----------------------- AmigaOS Message/Error Strings ----------------------- */

/* en */ {IDMES_SETTINGS_SAVED_SUCCESS,
/* de */  IDMES_SETTINGS_SAVED_SUCCESS_DE,
/* fr */  IDMES_SETTINGS_SAVED_SUCCESS_FR,
/* it */  IDMES_SETTINGS_SAVED_SUCCESS_IT,
/* nl */  IDMES_SETTINGS_SAVED_SUCCESS_NL,
/* pl */  IDMES_SETTINGS_SAVED_SUCCESS_PL,
/* sv */  IDMES_SETTINGS_SAVED_SUCCESS_SV},

/* en */ {IDMES_SETTINGS_LOAD_SUCCESS,
/* de */  IDMES_SETTINGS_LOAD_SUCCESS_DE,
/* fr */  IDMES_SETTINGS_LOAD_SUCCESS_FR,
/* it */  IDMES_SETTINGS_LOAD_SUCCESS_IT,
/* nl */  IDMES_SETTINGS_LOAD_SUCCESS_NL,
/* pl */  IDMES_SETTINGS_LOAD_SUCCESS_PL,
/* sv */  IDMES_SETTINGS_LOAD_SUCCESS_SV},

/* en */ {IDMES_DFLT_SETTINGS_RESTORED,
/* de */  IDMES_DFLT_SETTINGS_RESTORED_DE,
/* fr */  IDMES_DFLT_SETTINGS_RESTORED_FR,
/* it */  IDMES_DFLT_SETTINGS_RESTORED_IT,
/* nl */  IDMES_DFLT_SETTINGS_RESTORED_NL,
/* pl */  IDMES_DFLT_SETTINGS_RESTORED_PL,
/* sv */  IDMES_DFLT_SETTINGS_RESTORED_SV},

/* en */ {IDMES_VICE_MESSAGE,
/* de */  IDMES_VICE_MESSAGE_DE,
/* fr */  IDMES_VICE_MESSAGE_FR,
/* it */  IDMES_VICE_MESSAGE_IT,
/* nl */  IDMES_VICE_MESSAGE_NL,
/* pl */  IDMES_VICE_MESSAGE_PL,
/* sv */  IDMES_VICE_MESSAGE_SV},

/* en */ {IDMES_OK,
/* de */  IDMES_OK_DE,
/* fr */  IDMES_OK_FR,
/* it */  IDMES_OK_IT,
/* nl */  IDMES_OK_NL,
/* pl */  IDMES_OK_PL,
/* sv */  IDMES_OK_SV},

/* en */ {IDMES_VICE_CONTRIBUTORS,
/* de */  IDMES_VICE_CONTRIBUTORS_DE,
/* fr */  IDMES_VICE_CONTRIBUTORS_FR,
/* it */  IDMES_VICE_CONTRIBUTORS_IT,
/* nl */  IDMES_VICE_CONTRIBUTORS_NL,
/* pl */  IDMES_VICE_CONTRIBUTORS_PL,
/* sv */  IDMES_VICE_CONTRIBUTORS_SV},

/* en */ {IDMES_WHO_MADE_WHAT,
/* de */  IDMES_WHO_MADE_WHAT_DE,
/* fr */  IDMES_WHO_MADE_WHAT_FR,
/* it */  IDMES_WHO_MADE_WHAT_IT,
/* nl */  IDMES_WHO_MADE_WHAT_NL,
/* pl */  IDMES_WHO_MADE_WHAT_PL,
/* sv */  IDMES_WHO_MADE_WHAT_SV},

/* en */ {IDMES_VICE_DIST_NO_WARRANTY,
/* de */  IDMES_VICE_DIST_NO_WARRANTY_DE,
/* fr */  IDMES_VICE_DIST_NO_WARRANTY_FR,
/* it */  IDMES_VICE_DIST_NO_WARRANTY_IT,
/* nl */  IDMES_VICE_DIST_NO_WARRANTY_NL,
/* pl */  IDMES_VICE_DIST_NO_WARRANTY_PL,
/* sv */  IDMES_VICE_DIST_NO_WARRANTY_SV},

/* en */ {IDMES_WHICH_COMMANDS_AVAILABLE,
/* de */  IDMES_WHICH_COMMANDS_AVAILABLE_DE,
/* fr */  IDMES_WHICH_COMMANDS_AVAILABLE_FR,
/* it */  IDMES_WHICH_COMMANDS_AVAILABLE_IT,
/* nl */  IDMES_WHICH_COMMANDS_AVAILABLE_NL,
/* pl */  IDMES_WHICH_COMMANDS_AVAILABLE_PL,
/* sv */  IDMES_WHICH_COMMANDS_AVAILABLE_SV},

/* en */ {IDMES_CANNOT_SAVE_SETTINGS,
/* de */  IDMES_CANNOT_SAVE_SETTINGS_DE,
/* fr */  IDMES_CANNOT_SAVE_SETTINGS_FR,
/* it */  IDMES_CANNOT_SAVE_SETTINGS_IT,
/* nl */  IDMES_CANNOT_SAVE_SETTINGS_NL,
/* pl */  IDMES_CANNOT_SAVE_SETTINGS_PL,
/* sv */  IDMES_CANNOT_SAVE_SETTINGS_SV},

/* en */ {IDMES_CANNOT_LOAD_SETTINGS,
/* de */  IDMES_CANNOT_LOAD_SETTINGS_DE,
/* fr */  IDMES_CANNOT_LOAD_SETTINGS_FR,
/* it */  IDMES_CANNOT_LOAD_SETTINGS_IT,
/* nl */  IDMES_CANNOT_LOAD_SETTINGS_NL,
/* pl */  IDMES_CANNOT_LOAD_SETTINGS_PL,
/* sv */  IDMES_CANNOT_LOAD_SETTINGS_SV},

/* en */ {IDMES_VICE_ERROR,
/* de */  IDMES_VICE_ERROR_DE,
/* fr */  IDMES_VICE_ERROR_FR,
/* it */  IDMES_VICE_ERROR_IT,
/* nl */  IDMES_VICE_ERROR_NL,
/* pl */  IDMES_VICE_ERROR_PL,
/* sv */  IDMES_VICE_ERROR_SV},

/* en */ {IDMES_NO_JOY_ON_PORT_D,
/* de */  IDMES_NO_JOY_ON_PORT_D_DE,
/* fr */  IDMES_NO_JOY_ON_PORT_D_FR,
/* it */  IDMES_NO_JOY_ON_PORT_D_IT,
/* nl */  IDMES_NO_JOY_ON_PORT_D_NL,
/* pl */  IDMES_NO_JOY_ON_PORT_D_PL,
/* sv */  IDMES_NO_JOY_ON_PORT_D_SV},

/* en */ {IDMES_MOUSE_ON_PORT_D,
/* de */  IDMES_MOUSE_ON_PORT_D_DE,
/* fr */  IDMES_MOUSE_ON_PORT_D_FR,
/* it */  IDMES_MOUSE_ON_PORT_D_IT,
/* nl */  IDMES_MOUSE_ON_PORT_D_NL,
/* pl */  IDMES_MOUSE_ON_PORT_D_PL,
/* sv */  IDMES_MOUSE_ON_PORT_D_SV},

/* en */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D,
/* de */  IDMES_UNKNOWN_DEVICE_ON_PORT_D_DE,
/* fr */  IDMES_UNKNOWN_DEVICE_ON_PORT_D_FR,
/* it */  IDMES_UNKNOWN_DEVICE_ON_PORT_D_IT,
/* nl */  IDMES_UNKNOWN_DEVICE_ON_PORT_D_NL,
/* pl */  IDMES_UNKNOWN_DEVICE_ON_PORT_D_PL,
/* sv */  IDMES_UNKNOWN_DEVICE_ON_PORT_D_SV},

/* en */ {IDMES_DEVICE_NOT_GAMEPAD,
/* de */  IDMES_DEVICE_NOT_GAMEPAD_DE,
/* fr */  IDMES_DEVICE_NOT_GAMEPAD_FR,
/* it */  IDMES_DEVICE_NOT_GAMEPAD_IT,
/* nl */  IDMES_DEVICE_NOT_GAMEPAD_NL,
/* pl */  IDMES_DEVICE_NOT_GAMEPAD_PL,
/* sv */  IDMES_DEVICE_NOT_GAMEPAD_SV},

/* en */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT,
/* de */  IDMES_NOT_MAPPED_TO_AMIGA_PORT_DE,
/* fr */  IDMES_NOT_MAPPED_TO_AMIGA_PORT_FR,
/* it */  IDMES_NOT_MAPPED_TO_AMIGA_PORT_IT,
/* nl */  IDMES_NOT_MAPPED_TO_AMIGA_PORT_NL,
/* pl */  IDMES_NOT_MAPPED_TO_AMIGA_PORT_PL,
/* sv */  IDMES_NOT_MAPPED_TO_AMIGA_PORT_SV},

/* en */ {IDMES_CANNOT_AUTOSTART_FILE,
/* de */  IDMES_CANNOT_AUTOSTART_FILE_DE,
/* fr */  IDMES_CANNOT_AUTOSTART_FILE_FR,
/* it */  IDMES_CANNOT_AUTOSTART_FILE_IT,
/* nl */  IDMES_CANNOT_AUTOSTART_FILE_NL,
/* pl */  IDMES_CANNOT_AUTOSTART_FILE_PL,
/* sv */  IDMES_CANNOT_AUTOSTART_FILE_SV},

/* en */ {IDMES_CANNOT_ATTACH_FILE,
/* de */  IDMES_CANNOT_ATTACH_FILE_DE,
/* fr */  IDMES_CANNOT_ATTACH_FILE_FR,
/* it */  IDMES_CANNOT_ATTACH_FILE_IT,
/* nl */  IDMES_CANNOT_ATTACH_FILE_NL,
/* pl */  IDMES_CANNOT_ATTACH_FILE_PL,
/* sv */  IDMES_CANNOT_ATTACH_FILE_SV},

/* en */ {IDMES_INVALID_CART,
/* de */  IDMES_INVALID_CART_DE,
/* fr */  IDMES_INVALID_CART_FR,
/* it */  IDMES_INVALID_CART_IT,
/* nl */  IDMES_INVALID_CART_NL,
/* pl */  IDMES_INVALID_CART_PL,
/* sv */  IDMES_INVALID_CART_SV},

/* en */ {IDMES_BAD_CART_CONFIG_IN_UI,
/* de */  IDMES_BAD_CART_CONFIG_IN_UI_DE,
/* fr */  IDMES_BAD_CART_CONFIG_IN_UI_FR,
/* it */  IDMES_BAD_CART_CONFIG_IN_UI_IT,
/* nl */  IDMES_BAD_CART_CONFIG_IN_UI_NL,
/* pl */  IDMES_BAD_CART_CONFIG_IN_UI_PL,
/* sv */  IDMES_BAD_CART_CONFIG_IN_UI_SV},

/* en */ {IDMES_INVALID_CART_IMAGE,
/* de */  IDMES_INVALID_CART_IMAGE_DE,
/* fr */  IDMES_INVALID_CART_IMAGE_FR,
/* it */  IDMES_INVALID_CART_IMAGE_IT,
/* nl */  IDMES_INVALID_CART_IMAGE_NL,
/* pl */  IDMES_INVALID_CART_IMAGE_PL,
/* sv */  IDMES_INVALID_CART_IMAGE_SV},

/* en */ {IDMES_CANNOT_CREATE_IMAGE,
/* de */  IDMES_CANNOT_CREATE_IMAGE_DE,
/* fr */  IDMES_CANNOT_CREATE_IMAGE_FR,
/* it */  IDMES_CANNOT_CREATE_IMAGE_IT,
/* nl */  IDMES_CANNOT_CREATE_IMAGE_NL,
/* pl */  IDMES_CANNOT_CREATE_IMAGE_PL,
/* sv */  IDMES_CANNOT_CREATE_IMAGE_SV},

/* en */ {IDMES_ERROR_STARTING_SERVER,
/* de */  IDMES_ERROR_STARTING_SERVER_DE,
/* fr */  IDMES_ERROR_STARTING_SERVER_FR,
/* it */  IDMES_ERROR_STARTING_SERVER_IT,
/* nl */  IDMES_ERROR_STARTING_SERVER_NL,
/* pl */  IDMES_ERROR_STARTING_SERVER_PL,
/* sv */  IDMES_ERROR_STARTING_SERVER_SV},

/* en */ {IDMES_ERROR_CONNECTING_CLIENT,
/* de */  IDMES_ERROR_CONNECTING_CLIENT_DE,
/* fr */  IDMES_ERROR_CONNECTING_CLIENT_FR,
/* it */  IDMES_ERROR_CONNECTING_CLIENT_IT,
/* nl */  IDMES_ERROR_CONNECTING_CLIENT_NL,
/* pl */  IDMES_ERROR_CONNECTING_CLIENT_PL,
/* sv */  IDMES_ERROR_CONNECTING_CLIENT_SV},

/* en */ {IDMES_INVALID_PORT_NUMBER,
/* de */  IDMES_INVALID_PORT_NUMBER_DE,
/* fr */  IDMES_INVALID_PORT_NUMBER_FR,
/* it */  IDMES_INVALID_PORT_NUMBER_IT,
/* nl */  IDMES_INVALID_PORT_NUMBER_NL,
/* pl */  IDMES_INVALID_PORT_NUMBER_PL,
/* sv */  IDMES_INVALID_PORT_NUMBER_SV},

/* en */ {IDMES_THIS_MACHINE_NO_SID,
/* de */  IDMES_THIS_MACHINE_NO_SID_DE,
/* fr */  IDMES_THIS_MACHINE_NO_SID_FR,
/* it */  IDMES_THIS_MACHINE_NO_SID_IT,
/* nl */  IDMES_THIS_MACHINE_NO_SID_NL,
/* pl */  IDMES_THIS_MACHINE_NO_SID_PL,
/* sv */  IDMES_THIS_MACHINE_NO_SID_SV},

/* ----------------------- AmigaOS Strings ----------------------- */

/* en */ {IDS_PRESS_KEY_BUTTON,
/* de */  IDS_PRESS_KEY_BUTTON_DE,
/* fr */  IDS_PRESS_KEY_BUTTON_FR,
/* it */  IDS_PRESS_KEY_BUTTON_IT,
/* nl */  IDS_PRESS_KEY_BUTTON_NL,
/* pl */  IDS_PRESS_KEY_BUTTON_PL,
/* sv */  IDS_PRESS_KEY_BUTTON_SV},

};

/* --------------------------------------------------------------------- */

static char *intl_text_table[countof(intl_translate_text_table)][countof(language_table)];

/* codepage conversion is not yet present */

char *intl_convert_cp(char *text, int cp)
{
  int len;
  char *buffer;

  if (text==NULL)
    return NULL;

  len=strlen(text);

  if (len==0)
    return NULL;

  buffer=strdup(text);

  return buffer;
}

static char *intl_get_string_by_id(int id)
{
  int k;

  for (k = 0; k < countof(intl_string_table); k++)
  {
    if (intl_string_table[k].resource_id==id)
      return intl_string_table[k].text;
  }
  return NULL;
}

char *intl_translate_text(int en_resource)
{
  int i;

  for (i = 0; i < countof(intl_translate_text_table); i++)
  {
    if (intl_translate_text_table[i][0]==en_resource)
    {
      if (intl_translate_text_table[i][current_language_index]!=0 &&
          intl_text_table[i][current_language_index]!=NULL &&
          strlen(intl_text_table[i][current_language_index])!=0)
        return intl_text_table[i][current_language_index];
      else
        return intl_text_table[i][0];
    }
  }
  return "";
}

/* pre-translated main window caption text so the emulation won't
   slow down because of all the translation calls */

char *intl_speed_at_text;

/* --------------------------------------------------------------------- */

void intl_init(void)
{
  int i,j;

  for (i = 0; i < countof(language_table); i++)
  {
    for (j = 0; j < countof(intl_translate_text_table); j++)
    {
      if (intl_translate_text_table[j][i]==0)
        intl_text_table[j][i]=NULL;
      else
        intl_text_table[j][i]=(char *)intl_get_string_by_id(intl_translate_text_table[j][i]);
    }
  }
}

int intl_translate_res(int resource)
{
  return resource;
}

void intl_shutdown(void)
{
}

/* --------------------------------------------------------------------- */


static void intl_update_pre_translated_text(void)
{
/*  intl_speed_at_text=intl_translate_text(IDS_S_AT_D_SPEED); */
}

char *intl_arch_language_init(void)
{
  int i;
  struct Locale *amigalang;

  amigalang=OpenLocale(NULL);
  CloseLocale(amigalang);
  for (i = 0; amiga_to_iso[i].iso_language_code != NULL; i++)
  {
    if (!strcasecmp(amiga_to_iso[i].amiga_locale_language,amigalang->loc_LanguageName))
      return amiga_to_iso[i].iso_language_code;
  }
  return "en";
}

void intl_update_ui(void)
{
  intl_update_pre_translated_text();
  ui_update_menus();
}
