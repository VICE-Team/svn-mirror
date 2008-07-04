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

#ifndef __VBCC__
#define __USE_INLINE__
#endif

#include <proto/locale.h>

#include "archdep.h"
#include "cmdline.h"
#include "intl.h"
#include "lib.h"
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
  {"deutsch.language", "de"},
  {"french.language", "fr"},
  {"français.language", "fr"},
  {"hungarian.language", "hu"},
  {"italian.language", "it"},
  {"italiano.language", "it"},
  {"dutch.language", "nl"},
  {"nederlands.language", "nl"},
  {"polish.language", "pl"},
  {"polski.language", "pl"},
  {"swedish.language", "sv"},
  {"svenska.language", "sv"},
  {NULL, NULL}
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

/* hungarian */
  "hu",

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
/* hu */ {IDMS_FILE_HU, "Fájl"},
/* it */ {IDMS_FILE_IT, "File"},
/* nl */ {IDMS_FILE_NL, "Bestand"},
/* pl */ {IDMS_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FILE_SV, "Arkiv"},

/* en */ {IDMS_AUTOSTART_IMAGE,    "Autostart disk/tape image..."},
/* de */ {IDMS_AUTOSTART_IMAGE_DE, "Autostart Disk/Band Image..."},
/* fr */ {IDMS_AUTOSTART_IMAGE_FR, "Démarrage automatique d'une image datassette..."},
/* hu */ {IDMS_AUTOSTART_IMAGE_HU, "Lemez/szalag képmás automatikus indítása..."},
/* it */ {IDMS_AUTOSTART_IMAGE_IT, "Avvia automaticamente l'immagine di un disco/cassetta..."},
/* nl */ {IDMS_AUTOSTART_IMAGE_NL, "Autostart schijf-/tapebestand..."},
/* pl */ {IDMS_AUTOSTART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_AUTOSTART_IMAGE_SV, "Autostarta disk-/bandavbildningsfil..."},

/* en */ {IDMS_ATTACH_DISK_IMAGE,    "Attach disk image"},
/* de */ {IDMS_ATTACH_DISK_IMAGE_DE, "Disk Image einlegen"},
/* fr */ {IDMS_ATTACH_DISK_IMAGE_FR, "Insérer une image de disque"},
/* hu */ {IDMS_ATTACH_DISK_IMAGE_HU, "Lemez képmás csatolása"},
/* it */ {IDMS_ATTACH_DISK_IMAGE_IT, "Seleziona l'immagine di un disco"},
/* nl */ {IDMS_ATTACH_DISK_IMAGE_NL, "Koppel schijfbestand aan"},
/* pl */ {IDMS_ATTACH_DISK_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_DISK_IMAGE_SV, "Anslut diskettavbildningsfil"},

/* en */ {IDMS_DRIVE_8,    "Drive 8"},
/* de */ {IDMS_DRIVE_8_DE, "Laufwerk 8"},
/* fr */ {IDMS_DRIVE_8_FR, "Lecteur #8"},
/* hu */ {IDMS_DRIVE_8_HU, "#8-as lemezegység"},
/* it */ {IDMS_DRIVE_8_IT, "Drive 8"},
/* nl */ {IDMS_DRIVE_8_NL, "Drive 8"},
/* pl */ {IDMS_DRIVE_8_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_8_SV, "Enhet 8"},

/* en */ {IDMS_DRIVE_9,    "Drive 9"},
/* de */ {IDMS_DRIVE_9_DE, "Laufwerk 9"},
/* fr */ {IDMS_DRIVE_9_FR, "Lecteur #9"},
/* hu */ {IDMS_DRIVE_9_HU, "#9-es lemezegység"},
/* it */ {IDMS_DRIVE_9_IT, "Drive 9"},
/* nl */ {IDMS_DRIVE_9_NL, "Drive 9"},
/* pl */ {IDMS_DRIVE_9_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_9_SV, "Enhet 9"},

/* en */ {IDMS_DRIVE_10,    "Drive 10"},
/* de */ {IDMS_DRIVE_10_DE, "Laufwerk 10"},
/* fr */ {IDMS_DRIVE_10_FR, "Lecteur #10"},
/* hu */ {IDMS_DRIVE_10_HU, "#10-es lemezegység"},
/* it */ {IDMS_DRIVE_10_IT, "Drive 10"},
/* nl */ {IDMS_DRIVE_10_NL, "Drive 10"},
/* pl */ {IDMS_DRIVE_10_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_10_SV, "Enhet 10"},

/* en */ {IDMS_DRIVE_11,    "Drive 11"},
/* de */ {IDMS_DRIVE_11_DE, "Laufwerk 11"},
/* fr */ {IDMS_DRIVE_11_FR, "Lecteur #11"},
/* hu */ {IDMS_DRIVE_11_HU, "#11-es lemezegység"},
/* it */ {IDMS_DRIVE_11_IT, "Drive 11"},
/* nl */ {IDMS_DRIVE_11_NL, "Drive 11"},
/* pl */ {IDMS_DRIVE_11_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_11_SV, "Enhet 11"},

/* en */ {IDMS_DETACH_DISK_IMAGE,    "Detach disk image"},
/* de */ {IDMS_DETACH_DISK_IMAGE_DE, "Disk Image entfernen"},
/* fr */ {IDMS_DETACH_DISK_IMAGE_FR, "Retirer une image de disque"},
/* hu */ {IDMS_DETACH_DISK_IMAGE_HU, "Lemezképmás leválasztása"},
/* it */ {IDMS_DETACH_DISK_IMAGE_IT, "Rimuovi un immagine disco"},
/* nl */ {IDMS_DETACH_DISK_IMAGE_NL, "Ontkoppel disk bestand"},
/* pl */ {IDMS_DETACH_DISK_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DETACH_DISK_IMAGE_SV, "Koppla från diskettavbildningsfil"},

/* en */ {IDMS_ALL,    "All"},
/* de */ {IDMS_ALL_DE, "Alle"},
/* fr */ {IDMS_ALL_FR, "Tout"},
/* hu */ {IDMS_ALL_HU, "Összes"},
/* it */ {IDMS_ALL_IT, "Tutti"},
/* nl */ {IDMS_ALL_NL, "Alles"},
/* pl */ {IDMS_ALL_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ALL_SV, "Alla"},

/* en */ {IDMS_FLIP_LIST,    "Flip list"},
/* de */ {IDMS_FLIP_LIST_DE, "Flipliste"},
/* fr */ {IDMS_FLIP_LIST_FR, "Groupement de disques"},
/* hu */ {IDMS_FLIP_LIST_HU, "Lemezlista"},
/* it */ {IDMS_FLIP_LIST_IT, "Flip list"},
/* nl */ {IDMS_FLIP_LIST_NL, "Flip lijst"},
/* pl */ {IDMS_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FLIP_LIST_SV, "Vallista"},

/* en */ {IDMS_ADD_CURRENT_IMAGE,    "Add current image"},
/* de */ {IDMS_ADD_CURRENT_IMAGE_DE, "Aktuelles Image hinzufügen"},
/* fr */ {IDMS_ADD_CURRENT_IMAGE_FR, "Ajouter l'image de disque courante"},
/* hu */ {IDMS_ADD_CURRENT_IMAGE_HU, "Az aktuális képmás hozzáadása"},
/* it */ {IDMS_ADD_CURRENT_IMAGE_IT, "Aggiungi l'immagine attuale"},
/* nl */ {IDMS_ADD_CURRENT_IMAGE_NL, "Voeg huidig bestand toe"},
/* pl */ {IDMS_ADD_CURRENT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ADD_CURRENT_IMAGE_SV, "Lägg till aktuell avbildning"},

/* en */ {IDMS_REMOVE_CURRENT_IMAGE,    "Remove current image"},
/* de */ {IDMS_REMOVE_CURRENT_IMAGE_DE, "Aktuelles Image entfernen"},
/* fr */ {IDMS_REMOVE_CURRENT_IMAGE_FR, "Retirer l'image de disque courante"},
/* hu */ {IDMS_REMOVE_CURRENT_IMAGE_HU, "Az aktuális képmás eltávolítása"},
/* it */ {IDMS_REMOVE_CURRENT_IMAGE_IT, "Rimuovi l'immagine attuale"},
/* nl */ {IDMS_REMOVE_CURRENT_IMAGE_NL, "Verwijder huidig bestand"},
/* pl */ {IDMS_REMOVE_CURRENT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_REMOVE_CURRENT_IMAGE_SV, "Ta bort aktuell avbildning"},

/* en */ {IDMS_ATTACH_NEXT_IMAGE,    "Attach next image"},
/* de */ {IDMS_ATTACH_NEXT_IMAGE_DE, "Nächstes Image"},
/* fr */ {IDMS_ATTACH_NEXT_IMAGE_FR, "Insérer la prochaine image"},
/* hu */ {IDMS_ATTACH_NEXT_IMAGE_HU, "Következõ képmás csatolása"},
/* it */ {IDMS_ATTACH_NEXT_IMAGE_IT, "Seleziona l'immagine successiva"},
/* nl */ {IDMS_ATTACH_NEXT_IMAGE_NL, "Koppel volgend bestand"},
/* pl */ {IDMS_ATTACH_NEXT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_NEXT_IMAGE_SV, "Anslut nästa avbildning"},

/* en */ {IDMS_ATTACH_PREVIOUS_IMAGE,    "Attach previous image"},
/* de */ {IDMS_ATTACH_PREVIOUS_IMAGE_DE, "Voriges Image"},
/* fr */ {IDMS_ATTACH_PREVIOUS_IMAGE_FR, "Insérer l'image précédente"},
/* hu */ {IDMS_ATTACH_PREVIOUS_IMAGE_HU, "Elõzõ képmás csatolása"},
/* it */ {IDMS_ATTACH_PREVIOUS_IMAGE_IT, "Seleziona l'immagine precedente"},
/* nl */ {IDMS_ATTACH_PREVIOUS_IMAGE_NL, "Koppel vorig bestand"},
/* pl */ {IDMS_ATTACH_PREVIOUS_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_PREVIOUS_IMAGE_SV, "Anslut föregående avbildning"},

/* en */ {IDMS_LOAD_FLIP_LIST,    "Load flip list"},
/* de */ {IDMS_LOAD_FLIP_LIST_DE, "Fliplist Datei Laden"},
/* fr */ {IDMS_LOAD_FLIP_LIST_FR, "Charger un groupement de disques"},
/* hu */ {IDMS_LOAD_FLIP_LIST_HU, "Lemezlista betöltése"},
/* it */ {IDMS_LOAD_FLIP_LIST_IT, "Carica flip list"},
/* nl */ {IDMS_LOAD_FLIP_LIST_NL, "Laad fliplijst"},
/* pl */ {IDMS_LOAD_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_FLIP_LIST_SV, "Läs in vallistefil"},

/* en */ {IDMS_SAVE_FLIP_LIST,    "Save flip list"},
/* de */ {IDMS_SAVE_FLIP_LIST_DE, "Fliplist Datei Speichern"},
/* fr */ {IDMS_SAVE_FLIP_LIST_FR, "Enregistrer le groupement de disques"},
/* hu */ {IDMS_SAVE_FLIP_LIST_HU, "Lemezlista mentése"},
/* it */ {IDMS_SAVE_FLIP_LIST_IT, "Salva fliplist"},
/* nl */ {IDMS_SAVE_FLIP_LIST_NL, "Opslaan fliplijst"},
/* pl */ {IDMS_SAVE_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_FLIP_LIST_SV, "Spara vallistefil"},

/* en */ {IDMS_ATTACH_TAPE_IMAGE,    "Attach tape image..."},
/* de */ {IDMS_ATTACH_TAPE_IMAGE_DE, "Band Image einlegen..."},
/* fr */ {IDMS_ATTACH_TAPE_IMAGE_FR, "Insérer une image datassette..."},
/* hu */ {IDMS_ATTACH_TAPE_IMAGE_HU, "Szalag képmás csatolása..."},
/* it */ {IDMS_ATTACH_TAPE_IMAGE_IT, "Seleziona l'immagine di una cassetta..."},
/* nl */ {IDMS_ATTACH_TAPE_IMAGE_NL, "Koppel tapebestand..."},
/* pl */ {IDMS_ATTACH_TAPE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_TAPE_IMAGE_SV, "Anslut en bandavbildningsfil..."},

/* en */ {IDMS_DETACH_TAPE_IMAGE,    "Detach tape image"},
/* de */ {IDMS_DETACH_TAPE_IMAGE_DE, "Band Image entfernen"},
/* fr */ {IDMS_DETACH_TAPE_IMAGE_FR, "Retirer une image datassette"},
/* hu */ {IDMS_DETACH_TAPE_IMAGE_HU, "Szalag képmás leválasztása"},
/* it */ {IDMS_DETACH_TAPE_IMAGE_IT, "Rimuovi immagine cassetta"},
/* nl */ {IDMS_DETACH_TAPE_IMAGE_NL, "Ontkoppel tape image"},
/* pl */ {IDMS_DETACH_TAPE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DETACH_TAPE_IMAGE_SV, "Koppla från bandavbildningsfil"},

/* en */ {IDMS_DATASSETTE_CONTROL,    "Datassette control"},
/* de */ {IDMS_DATASSETTE_CONTROL_DE, "Bandlaufwerk Kontrolle"},
/* fr */ {IDMS_DATASSETTE_CONTROL_FR, "Contrôle datassette"},
/* hu */ {IDMS_DATASSETTE_CONTROL_HU, "Magnó vezérlés"},
/* it */ {IDMS_DATASSETTE_CONTROL_IT, "Controlli del registratore"},
/* nl */ {IDMS_DATASSETTE_CONTROL_NL, "Datassette bediening"},
/* pl */ {IDMS_DATASSETTE_CONTROL_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DATASSETTE_CONTROL_SV, "Datasettestyrning"},

/* en */ {IDMS_STOP,    "Stop"},
/* de */ {IDMS_STOP_DE, "Stop"},
/* fr */ {IDMS_STOP_FR, "Arrêt"},
/* hu */ {IDMS_STOP_HU, "Leállítás"},
/* it */ {IDMS_STOP_IT, "Stop"},
/* nl */ {IDMS_STOP_NL, "Stop"},
/* pl */ {IDMS_STOP_PL, ""},  /* fuzzy */
/* sv */ {IDMS_STOP_SV, "Stoppa"},

/* en */ {IDMS_START,    "Start"},
/* de */ {IDMS_START_DE, "Start"},
/* fr */ {IDMS_START_FR, "Démarrer"},
/* hu */ {IDMS_START_HU, "Indítás"},
/* it */ {IDMS_START_IT, "Avvia"},
/* nl */ {IDMS_START_NL, "Start"},
/* pl */ {IDMS_START_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_SV, "Starta"},

/* en */ {IDMS_FORWARD,    "Forward"},
/* de */ {IDMS_FORWARD_DE, "Forward"},
/* fr */ {IDMS_FORWARD_FR, "En avant"},
/* hu */ {IDMS_FORWARD_HU, "Elõrecsévélés"},
/* it */ {IDMS_FORWARD_IT, "Avanti"},
/* nl */ {IDMS_FORWARD_NL, "Vooruit"},
/* pl */ {IDMS_FORWARD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FORWARD_SV, "Spola framåt"},

/* en */ {IDMS_REWIND,    "Rewind"},
/* de */ {IDMS_REWIND_DE, "Rewind"},
/* fr */ {IDMS_REWIND_FR, "En arrière"},
/* hu */ {IDMS_REWIND_HU, "Visszacsévélés"},
/* it */ {IDMS_REWIND_IT, "Indietro"},
/* nl */ {IDMS_REWIND_NL, "Terug"},
/* pl */ {IDMS_REWIND_PL, ""},  /* fuzzy */
/* sv */ {IDMS_REWIND_SV, "Spola bakåt"},

/* en */ {IDMS_RECORD,    "Record"},
/* de */ {IDMS_RECORD_DE, "Record"},
/* fr */ {IDMS_RECORD_FR, "Enregistrer"},
/* hu */ {IDMS_RECORD_HU, "Felvétel"},
/* it */ {IDMS_RECORD_IT, "Registra"},
/* nl */ {IDMS_RECORD_NL, "Opname"},
/* pl */ {IDMS_RECORD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RECORD_SV, "Spela in"},

/* en */ {IDMS_RESET,    "Reset"},
/* de */ {IDMS_RESET_DE, "Reset"},
/* fr */ {IDMS_RESET_FR, "Réinitialiser"},
/* hu */ {IDMS_RESET_HU, "Reset"},
/* it */ {IDMS_RESET_IT, "Reset"},
/* nl */ {IDMS_RESET_NL, "Reset"},
/* pl */ {IDMS_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RESET_SV, "Nollställ"},

/* en */ {IDMS_RESET_COUNTER,    "Reset Counter"},
/* de */ {IDMS_RESET_COUNTER_DE, "Zähler zurücksetzen"},
/* fr */ {IDMS_RESET_COUNTER_FR, "Réinialiser le compteur"},
/* hu */ {IDMS_RESET_COUNTER_HU, "Számláló nullázása"},
/* it */ {IDMS_RESET_COUNTER_IT, "Reset contantore"},
/* nl */ {IDMS_RESET_COUNTER_NL, "Reset teller"},
/* pl */ {IDMS_RESET_COUNTER_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RESET_COUNTER_SV, "Nollställ räknare"},

/* en */ {IDMS_ATTACH_CART_IMAGE,    "Attach cartridge image..."},
/* de */ {IDMS_ATTACH_CART_IMAGE_DE, "Erweiterungsmodule einlegen..."},
/* fr */ {IDMS_ATTACH_CART_IMAGE_FR, "Insérer une cartouche..."},
/* hu */ {IDMS_ATTACH_CART_IMAGE_HU, "Cartridge képmás csatolása..."},
/* it */ {IDMS_ATTACH_CART_IMAGE_IT, "Seleziona l'immagine di una cartuccia..."},
/* nl */ {IDMS_ATTACH_CART_IMAGE_NL, "Koppel cartridgebestand..."},
/* pl */ {IDMS_ATTACH_CART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_CART_IMAGE_SV, "Anslut insticksmodulfil..."},

/* en */ {IDMS_4_8_16KB_AT_2000,    "4/8/16KB image at $2000..."},
/* de */ {IDMS_4_8_16KB_AT_2000_DE, "4/8/16KB Modul Image bei $2000..."},
/* fr */ {IDMS_4_8_16KB_AT_2000_FR, "Insertion d'une image 4/8/16Ko à $2000..."},
/* hu */ {IDMS_4_8_16KB_AT_2000_HU, "4/8/16KB képmás $2000 címen..."},
/* it */ {IDMS_4_8_16KB_AT_2000_IT, "Immagine di 4/8/16KB a $2000..."},
/* nl */ {IDMS_4_8_16KB_AT_2000_NL, "4/8/16KB-bestand in $2000..."},
/* pl */ {IDMS_4_8_16KB_AT_2000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4_8_16KB_AT_2000_SV, "4/8/16KB-fil vid $2000..."},

/* en */ {IDMS_4_8_16KB_AT_4000,    "4/8/16KB image at $4000..."},
/* de */ {IDMS_4_8_16KB_AT_4000_DE, "4/8/16KB Modul Image bei $4000..."},
/* fr */ {IDMS_4_8_16KB_AT_4000_FR, "Insertion d'une image 4/8/16Ko à $4000..."},
/* hu */ {IDMS_4_8_16KB_AT_4000_HU, "4/8/16KB képmás $4000 címen..."},
/* it */ {IDMS_4_8_16KB_AT_4000_IT, "Immagine di 4/8/16KB a $4000..."},
/* nl */ {IDMS_4_8_16KB_AT_4000_NL, "4/8/16KB-bestand in $4000..."},
/* pl */ {IDMS_4_8_16KB_AT_4000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4_8_16KB_AT_4000_SV, "4/8/16KB-fil vid $4000..."},

/* en */ {IDMS_4_8_16KB_AT_6000,    "4/8/16KB image at $6000..."},
/* de */ {IDMS_4_8_16KB_AT_6000_DE, "4/8/16KB Modul Image bei $6000..."},
/* fr */ {IDMS_4_8_16KB_AT_6000_FR, "Insertion d'une image 4/8/16Ko à $6000..."},
/* hu */ {IDMS_4_8_16KB_AT_6000_HU, "4/8/16KB képmás $6000 címen..."},
/* it */ {IDMS_4_8_16KB_AT_6000_IT, "Immagine di 4/8/16KB a $6000..."},
/* nl */ {IDMS_4_8_16KB_AT_6000_NL, "4/8/16KB-bestand in $6000..."},
/* pl */ {IDMS_4_8_16KB_AT_6000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4_8_16KB_AT_6000_SV, "4/8/16KB-fil vid $6000..."},

/* en */ {IDMS_4_8KB_AT_A000,    "4/8KB image at $A000..."},
/* de */ {IDMS_4_8KB_AT_A000_DE, "4/8KB Modul Image bei $A000..."},
/* fr */ {IDMS_4_8KB_AT_A000_FR, "Insertion d'une image 4/8Ko à $A000..."},
/* hu */ {IDMS_4_8KB_AT_A000_HU, "4/8KB képmás $A000 címen..."},
/* it */ {IDMS_4_8KB_AT_A000_IT, "Immagine di 4/8KB a $A000..."},
/* nl */ {IDMS_4_8KB_AT_A000_NL, "4/8KB-bestand in $A000..."},
/* pl */ {IDMS_4_8KB_AT_A000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4_8KB_AT_A000_SV, "4/8KB-fil vid $A000..."},

/* en */ {IDMS_4KB_AT_B000,    "4KB image at $B000..."},
/* de */ {IDMS_4KB_AT_B000_DE, "4KB Modul Image bei $B000..."},
/* fr */ {IDMS_4KB_AT_B000_FR, "Insertion d'une image 4Ko à $B000..."},
/* it */ {IDMS_4KB_AT_B000_IT, "Immagine di 4KB a $B000..."},
/* hu */ {IDMS_4KB_AT_B000_HU, "4KB képmás $B000 címen..."},
/* nl */ {IDMS_4KB_AT_B000_NL, "4KB-bestand in $&B000..."},
/* pl */ {IDMS_4KB_AT_B000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4KB_AT_B000_SV, "4KB-fil vid $B000..."},

/* en */ {IDMS_DETACH_CART_IMAGE,    "Detach cartridge image"},
/* de */ {IDMS_DETACH_CART_IMAGE_DE, "Erweiterungsmodul Image entfernen"},
/* fr */ {IDMS_DETACH_CART_IMAGE_FR, "Retirer une cartouche"},
/* hu */ {IDMS_DETACH_CART_IMAGE_HU, "Cartridge képmás leválasztása"},
/* it */ {IDMS_DETACH_CART_IMAGE_IT, "Rimuovi immagine cartuccia"},
/* nl */ {IDMS_DETACH_CART_IMAGE_NL, "Ontkoppel cartridgebestand"},
/* pl */ {IDMS_DETACH_CART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DETACH_CART_IMAGE_SV, "Koppla från insticksmodulfil"},

/* en */ {IDMS_C1_LOW_IMAGE,    "C1 low  image..."},
/* de */ {IDMS_C1_LOW_IMAGE_DE, "C1 low Image Datei..."},
/* fr */ {IDMS_C1_LOW_IMAGE_FR, "Image C1 basse..."},
/* hu */ {IDMS_C1_LOW_IMAGE_HU, "C1 alsó képmás..."},
/* it */ {IDMS_C1_LOW_IMAGE_IT, "Immagine nell'area bassa di C1..."},
/* nl */ {IDMS_C1_LOW_IMAGE_NL, "C1 low-bestand..."},
/* pl */ {IDMS_C1_LOW_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C1_LOW_IMAGE_SV, "C1 låg-avbildning..."},

/* en */ {IDMS_C1_HIGH_IMAGE,    "C1 high image..."},
/* de */ {IDMS_C1_HIGH_IMAGE_DE, "C1 high Image Datei..."},
/* fr */ {IDMS_C1_HIGH_IMAGE_FR, "Image C1 haute..."},
/* hu */ {IDMS_C1_HIGH_IMAGE_HU, "C1 felsõ képmás..."},
/* it */ {IDMS_C1_HIGH_IMAGE_IT, "Immagine nell'area alta di C1..."},
/* nl */ {IDMS_C1_HIGH_IMAGE_NL, "C1 high-bestand..."},
/* pl */ {IDMS_C1_HIGH_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C1_HIGH_IMAGE_SV, "C1 hög-avbildning..."},

/* en */ {IDMS_C2_LOW_IMAGE,    "C2 low  image..."},
/* de */ {IDMS_C2_LOW_IMAGE_DE, "C2 low Image Datei..."},
/* fr */ {IDMS_C2_LOW_IMAGE_FR, "Image C2 basse..."},
/* it */ {IDMS_C2_LOW_IMAGE_IT, "Immagine nell'area bassa di C2..."},
/* hu */ {IDMS_C2_LOW_IMAGE_HU, "C2 alsó képmás..."},
/* nl */ {IDMS_C2_LOW_IMAGE_NL, "C2 low-bestand..."},
/* pl */ {IDMS_C2_LOW_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C2_LOW_IMAGE_SV, "C2 låg-avbildning..."},

/* en */ {IDMS_C2_HIGH_IMAGE,    "C2 high image..."},
/* de */ {IDMS_C2_HIGH_IMAGE_DE, "C2 high Image Datei..."},
/* fr */ {IDMS_C2_HIGH_IMAGE_FR, "Image C2 haute..."},
/* hu */ {IDMS_C2_HIGH_IMAGE_HU, "C2 felsõ képmás..."},
/* it */ {IDMS_C2_HIGH_IMAGE_IT, "Immagine nell'area alta di C2..."},
/* nl */ {IDMS_C2_HIGH_IMAGE_NL, "C2 high-bestand..."},
/* pl */ {IDMS_C2_HIGH_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C2_HIGH_IMAGE_SV, "C2 hög-avbildning..."},

/* en */ {IDMS_FUNCTION_LOW_3PLUS1,    "Function low  image (3plus1)..."},
/* de */ {IDMS_FUNCTION_LOW_3PLUS1_DE, "Funktions ROM Image Datei low (3plus1)..."},
/* fr */ {IDMS_FUNCTION_LOW_3PLUS1_FR, "Image Fonction basse (3+1)..."},
/* hu */ {IDMS_FUNCTION_LOW_3PLUS1_HU, "Function alsó képmás (3plus1)..."},
/* it */ {IDMS_FUNCTION_LOW_3PLUS1_IT, "Function (3plus1) caricata nella memoria bassa..."},
/* nl */ {IDMS_FUNCTION_LOW_3PLUS1_NL, "Function low-bestand (3plus1)..."},
/* pl */ {IDMS_FUNCTION_LOW_3PLUS1_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FUNCTION_LOW_3PLUS1_SV, "Funktion låg-avbildning (3plus1)..."},

/* en */ {IDMS_FUNCTION_HIGH_3PLUS1,    "Function high image (3plus1)..."},
/* de */ {IDMS_FUNCTION_HIGH_3PLUS1_DE, "Funktions ROM Image Datei high (3plus1)..."},
/* fr */ {IDMS_FUNCTION_HIGH_3PLUS1_FR, "Image Fonction haute (3+1)..."},
/* hu */ {IDMS_FUNCTION_HIGH_3PLUS1_HU, "Function felsõ képmás (3plus1)..."},
/* it */ {IDMS_FUNCTION_HIGH_3PLUS1_IT, "Function (3plus1) caricata nella memoria alta..."},
/* nl */ {IDMS_FUNCTION_HIGH_3PLUS1_NL, "Function high-bestand (3plus1)..."},
/* pl */ {IDMS_FUNCTION_HIGH_3PLUS1_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FUNCTION_HIGH_3PLUS1_SV, "Funktion hög-avbildning (3plus1)..."},

/* en */ {IDMS_CRT_IMAGE,    "CRT image..."},
/* de */ {IDMS_CRT_IMAGE_DE, "CRT Image Datei..."},
/* fr */ {IDMS_CRT_IMAGE_FR, "Nom du fichier image CRT..."},
/* hu */ {IDMS_CRT_IMAGE_HU, "CRT képmás..."},
/* it */ {IDMS_CRT_IMAGE_IT, "Immagine CRT..."},
/* nl */ {IDMS_CRT_IMAGE_NL, "CRT-bestand..."},
/* pl */ {IDMS_CRT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CRT_IMAGE_SV, "CRT-fil..."},

/* en */ {IDMS_GENERIC_8KB_IMAGE,    "Generic 8KB image..."},
/* de */ {IDMS_GENERIC_8KB_IMAGE_DE, "Generische 8Kb Image..."},
/* fr */ {IDMS_GENERIC_8KB_IMAGE_FR, "Insérer une image générique de 8Ko..."},
/* hu */ {IDMS_GENERIC_8KB_IMAGE_HU, "Általános 8KB képmás..."},
/* it */ {IDMS_GENERIC_8KB_IMAGE_IT, "Immagine generica di 8KB..."},
/* nl */ {IDMS_GENERIC_8KB_IMAGE_NL, "Algemeen 8KB-bestand..."},
/* pl */ {IDMS_GENERIC_8KB_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_GENERIC_8KB_IMAGE_SV, "Vanlig 8KB-fil..."},

/* en */ {IDMS_GENERIC_16KB_IMAGE,    "Generic 16KB image..."},
/* de */ {IDMS_GENERIC_16KB_IMAGE_DE, "Generische 16Kb Image..."},
/* fr */ {IDMS_GENERIC_16KB_IMAGE_FR, "Insérer une image générique de 16Ko..."},
/* hu */ {IDMS_GENERIC_16KB_IMAGE_HU, "Általános 16KB képmás..."},
/* it */ {IDMS_GENERIC_16KB_IMAGE_IT, "Immagine generica di 16KB..."},
/* nl */ {IDMS_GENERIC_16KB_IMAGE_NL, "Algemeen 16KB-bestand..."},
/* pl */ {IDMS_GENERIC_16KB_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_GENERIC_16KB_IMAGE_SV, "Vanlig 16KB-fil..."},

/* en */ {IDMS_ACTION_REPLAY_IMAGE,    "Action Replay image..."},
/* de */ {IDMS_ACTION_REPLAY_IMAGE_DE, "Action Replay Image..."},
/* fr */ {IDMS_ACTION_REPLAY_IMAGE_FR, "Insérer une cartouche Action Replay..."},
/* hu */ {IDMS_ACTION_REPLAY_IMAGE_HU, "Action Replay képmás..."},
/* it */ {IDMS_ACTION_REPLAY_IMAGE_IT, "Immagine Action Replay..."},
/* nl */ {IDMS_ACTION_REPLAY_IMAGE_NL, "Action Replay-bestand..."},
/* pl */ {IDMS_ACTION_REPLAY_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ACTION_REPLAY_IMAGE_SV, "Action Replay-fil..."},

/* en */ {IDMS_ACTION_REPLAY3_IMAGE,    "Action Replay 3 image..."},
/* de */ {IDMS_ACTION_REPLAY3_IMAGE_DE, "Action Replay 3 Image..."},
/* fr */ {IDMS_ACTION_REPLAY3_IMAGE_FR, "Insrer une cartouche Action Replay 3..."},
/* hu */ {IDMS_ACTION_REPLAY3_IMAGE_HU, "Action Replay 3 kpms..."},
/* it */ {IDMS_ACTION_REPLAY3_IMAGE_IT, "Immagine Action Replay 3..."},
/* nl */ {IDMS_ACTION_REPLAY3_IMAGE_NL, "Action Replay 3 bestand..."},
/* pl */ {IDMS_ACTION_REPLAY3_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ACTION_REPLAY3_IMAGE_SV, "Action Replay 3-fil..."},

/* en */ {IDMS_ACTION_REPLAY4_IMAGE,    "Action Replay 4 image..."},
/* de */ {IDMS_ACTION_REPLAY4_IMAGE_DE, "Action Replay 4 Image..."},
/* fr */ {IDMS_ACTION_REPLAY4_IMAGE_FR, "Insrer une cartouche Action Replay 4..."},
/* hu */ {IDMS_ACTION_REPLAY4_IMAGE_HU, "Action Replay 4 kpms..."},
/* it */ {IDMS_ACTION_REPLAY4_IMAGE_IT, "Immagine Action Replay 4..."},
/* nl */ {IDMS_ACTION_REPLAY4_IMAGE_NL, "Action Replay 4 bestand..."},
/* pl */ {IDMS_ACTION_REPLAY4_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ACTION_REPLAY4_IMAGE_SV, "Action Replay 4-fil..."},

/* en */ {IDMS_STARDOS_IMAGE,    "StarDOS image..."},
/* de */ {IDMS_STARDOS_IMAGE_DE, ""},  /* fuzzy */
/* fr */ {IDMS_STARDOS_IMAGE_FR, ""},  /* fuzzy */
/* hu */ {IDMS_STARDOS_IMAGE_HU, ""},  /* fuzzy */
/* it */ {IDMS_STARDOS_IMAGE_IT, ""},  /* fuzzy */
/* nl */ {IDMS_STARDOS_IMAGE_NL, "StarDOS bestand..."},
/* pl */ {IDMS_STARDOS_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_STARDOS_IMAGE_SV, ""},  /* fuzzy */

/* en */ {IDMS_ATOMIC_POWER_IMAGE,    "Atomic Power image..."},
/* de */ {IDMS_ATOMIC_POWER_IMAGE_DE, "Atomic Power Image..."},
/* fr */ {IDMS_ATOMIC_POWER_IMAGE_FR, "Insérer une cartouche Atomic Power..."},
/* hu */ {IDMS_ATOMIC_POWER_IMAGE_HU, "Atomic Power képmás..."},
/* it */ {IDMS_ATOMIC_POWER_IMAGE_IT, "Immagine Atomic Power..."},
/* nl */ {IDMS_ATOMIC_POWER_IMAGE_NL, "Atomic Power-bestand..."},
/* pl */ {IDMS_ATOMIC_POWER_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATOMIC_POWER_IMAGE_SV, "Atomic Power-fil..."},

/* en */ {IDMS_EPYX_FASTLOAD_IMAGE,    "Epyx fastload image..."},
/* de */ {IDMS_EPYX_FASTLOAD_IMAGE_DE, "Epyx Fastload Image..."},
/* fr */ {IDMS_EPYX_FASTLOAD_IMAGE_FR, "Insérer une cartouche Epyx FastLoad..."},
/* hu */ {IDMS_EPYX_FASTLOAD_IMAGE_HU, "Epyx gyorstöltõ képmás..."},
/* it */ {IDMS_EPYX_FASTLOAD_IMAGE_IT, "Immagine Epyx fastload..."},
/* nl */ {IDMS_EPYX_FASTLOAD_IMAGE_NL, "Epyx fastloadbestand..."},
/* pl */ {IDMS_EPYX_FASTLOAD_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EPYX_FASTLOAD_IMAGE_SV, "Epyx fastload-fil..."},

/* en */ {IDMS_IEEE488_INTERFACE_IMAGE,    "IEEE488 interface image..."},
/* de */ {IDMS_IEEE488_INTERFACE_IMAGE_DE, "IEEE 488 Schnittstellenmodul..."},
/* fr */ {IDMS_IEEE488_INTERFACE_IMAGE_FR, "Insérer une cartouche interface IEEE488..."},
/* hu */ {IDMS_IEEE488_INTERFACE_IMAGE_HU, "IEEE488 interfész képmás..."},
/* it */ {IDMS_IEEE488_INTERFACE_IMAGE_IT, "Immagine dell'interfaccia IEEE488..."},
/* nl */ {IDMS_IEEE488_INTERFACE_IMAGE_NL, "IEEE488-interfacebestand..."},
/* pl */ {IDMS_IEEE488_INTERFACE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_IEEE488_INTERFACE_IMAGE_SV, "IEEE488-gränssnittsfil..."},

/* en */ {IDMS_RETRO_REPLAY_IMAGE,    "Retro Replay image..."},
/* de */ {IDMS_RETRO_REPLAY_IMAGE_DE, "Retro Replay Image..."},
/* fr */ {IDMS_RETRO_REPLAY_IMAGE_FR, "Insérer une cartouche Retro Replay..."},
/* hu */ {IDMS_RETRO_REPLAY_IMAGE_HU, "Retro Replay képmás..."},
/* it */ {IDMS_RETRO_REPLAY_IMAGE_IT, "Immagine Retro Replay..."},
/* nl */ {IDMS_RETRO_REPLAY_IMAGE_NL, "Retro Replay-bestand..."},
/* pl */ {IDMS_RETRO_REPLAY_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RETRO_REPLAY_IMAGE_SV, "Retro Replay-fil..."},

/* en */ {IDMS_IDE64_INTERFACE_IMAGE,    "IDE64 interface image..."},
/* de */ {IDMS_IDE64_INTERFACE_IMAGE_DE, "IDE64 Schnittstellenmodul..."},
/* fr */ {IDMS_IDE64_INTERFACE_IMAGE_FR, "Insérer une cartouche interface IDE64..."},
/* hu */ {IDMS_IDE64_INTERFACE_IMAGE_HU, "IDE64 interfész képmás..."},
/* it */ {IDMS_IDE64_INTERFACE_IMAGE_IT, "Immagine dell'interfaccia IDE64..."},
/* nl */ {IDMS_IDE64_INTERFACE_IMAGE_NL, "IDE64-interfacebestand..."},
/* pl */ {IDMS_IDE64_INTERFACE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_IDE64_INTERFACE_IMAGE_SV, "IDE64-gränssnittsfil..."},

/* en */ {IDMS_SUPER_SNAPSHOT_4_IMAGE,    "Super Snapshot 4 image..."},
/* de */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_DE, "Super Snapshot 4 Image..."},
/* fr */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_FR, "Insérer une cartouche Super Snapshot 4..."},
/* hu */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_HU, "Super Snapshot 4 képmás..."},
/* it */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_IT, "Immagine Super Snapshot 4..."},
/* nl */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_NL, "Super Snapshot 4-bestand..."},
/* pl */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_SV, "Super Snapshot 4-fil..."},

/* en */ {IDMS_SUPER_SNAPSHOT_5_IMAGE,    "Super Snapshot 5 image..."},
/* de */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_DE, "Super Snapshot 5 Image..."},
/* fr */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_FR, "Insérer une cartouche Super Snapshot 5..."},
/* hu */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_HU, "Super Snapshot 5 képmás..."},
/* it */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_IT, "Immagine Super Snapshot 5..."},
/* nl */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_NL, "Super Snapshot 5-bestand..."},
/* pl */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_SV, "Super Snapshot 5-fil..."},

/* en */ {IDMS_STRUCTURED_BASIC_IMAGE,    "Structured Basic image..."},
/* de */ {IDMS_STRUCTURED_BASIC_IMAGE_DE, "Structured Basic Image..."},
/* fr */ {IDMS_STRUCTURED_BASIC_IMAGE_FR, "Insérer une cartouche Structured Basic..."},
/* hu */ {IDMS_STRUCTURED_BASIC_IMAGE_HU, "Structured Basic képmás..."},
/* it */ {IDMS_STRUCTURED_BASIC_IMAGE_IT, "Immagine Structured Basic..."},
/* nl */ {IDMS_STRUCTURED_BASIC_IMAGE_NL, "Structured Basic-bestand..."},
/* pl */ {IDMS_STRUCTURED_BASIC_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_STRUCTURED_BASIC_IMAGE_SV, "Structured Basic-fil..."},

/* en */ {IDMS_EXPERT_CART,    "Expert Cartridge"},
/* de */ {IDMS_EXPERT_CART_DE, "Expert Cartridge"},
/* fr */ {IDMS_EXPERT_CART_FR, "Activer les paramètres des cartouches experts"},
/* hu */ {IDMS_EXPERT_CART_HU, "Expert cartridge"},
/* it */ {IDMS_EXPERT_CART_IT, "Expert Cartridge"},
/* nl */ {IDMS_EXPERT_CART_NL, "Expert Cartridge"},
/* pl */ {IDMS_EXPERT_CART_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EXPERT_CART_SV, "Expert Cartridge"},

/* en */ {IDMS_ENABLE,    "Enable"},
/* de */ {IDMS_ENABLE_DE, "Aktivieren"},
/* fr */ {IDMS_ENABLE_FR, "Activer"},
/* hu */ {IDMS_ENABLE_HU, "Engedélyezés"},
/* it */ {IDMS_ENABLE_IT, "Abilita"},
/* nl */ {IDMS_ENABLE_NL, "Activeer"},
/* pl */ {IDMS_ENABLE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ENABLE_SV, "Aktivera"},

/* en */ {IDMS_OFF,    "Off"},
/* de */ {IDMS_OFF_DE, "Aus"},
/* fr */ {IDMS_OFF_FR, "Off"},
/* hu */ {IDMS_OFF_HU, "Ki"},
/* it */ {IDMS_OFF_IT, "Off"},
/* nl */ {IDMS_OFF_NL, "Uit"},
/* pl */ {IDMS_OFF_PL, ""},  /* fuzzy */
/* sv */ {IDMS_OFF_SV, "Av"},

/* en */ {IDMS_PRG,    "Prg"},
/* de */ {IDMS_PRG_DE, "Prg"},
/* fr */ {IDMS_PRG_FR, "Prg"},
/* hu */ {IDMS_PRG_HU, "Prg"},
/* it */ {IDMS_PRG_IT, "Prg"},
/* nl */ {IDMS_PRG_NL, "Prg"},
/* pl */ {IDMS_PRG_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PRG_SV, "Prg"},

/* en */ {IDMS_ON,    "On"},
/* de */ {IDMS_ON_DE, "Ein"},
/* fr */ {IDMS_ON_FR, "On"},
/* hu */ {IDMS_ON_HU, "Be"},
/* it */ {IDMS_ON_IT, "On"},
/* nl */ {IDMS_ON_NL, "Aan"},
/* pl */ {IDMS_ON_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ON_SV, "På"},

/* en */ {IDMS_SET_CART_AS_DEFAULT,    "Set cartridge as default"},
/* de */ {IDMS_SET_CART_AS_DEFAULT_DE, "Aktuelles Erweiterungsmodul als standard aktivieren"},
/* fr */ {IDMS_SET_CART_AS_DEFAULT_FR, "Définir cette cartouche par défaut"},
/* hu */ {IDMS_SET_CART_AS_DEFAULT_HU, "Cartridge alapértelmezetté tétele"},
/* it */ {IDMS_SET_CART_AS_DEFAULT_IT, "Imposta la cartuccia come predefinita"},
/* nl */ {IDMS_SET_CART_AS_DEFAULT_NL, "Zet cartridge als standaard"},
/* pl */ {IDMS_SET_CART_AS_DEFAULT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SET_CART_AS_DEFAULT_SV, "Välj insticksmodul som förval"},

/* en */ {IDMS_RESET_ON_CART_CHANGE,    "Reset on cart change"},
/* de */ {IDMS_RESET_ON_CART_CHANGE_DE, "Reset bei Wechsel von Erweiterungsmodulen"},
/* fr */ {IDMS_RESET_ON_CART_CHANGE_FR, "Réinialiser sur changement de cartouche"},
/* hu */ {IDMS_RESET_ON_CART_CHANGE_HU, "Újraindítás cartridge cserénél"},
/* it */ {IDMS_RESET_ON_CART_CHANGE_IT, "Reset al cambio di cartuccia"},
/* nl */ {IDMS_RESET_ON_CART_CHANGE_NL, "Reset bij cartwisseling"},
/* pl */ {IDMS_RESET_ON_CART_CHANGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RESET_ON_CART_CHANGE_SV, "Nollställ vid modulbyte"},

/* en */ {IDMS_CART_FREEZE,    "Cartridge freeze"},
/* de */ {IDMS_CART_FREEZE_DE, "Erweiterungsmodul Freeze"},
/* fr */ {IDMS_CART_FREEZE_FR, "Geler la cartouche"},
/* hu */ {IDMS_CART_FREEZE_HU, "Fagyasztás Cartridge-dzsel"},
/* it */ {IDMS_CART_FREEZE_IT, "Freeze della cartuccia"},
/* nl */ {IDMS_CART_FREEZE_NL, "Cartridge freeze"},
/* pl */ {IDMS_CART_FREEZE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CART_FREEZE_SV, "Frys"},

/* en */ {IDMS_PAUSE,    "Pause"},
/* de */ {IDMS_PAUSE_DE, "Pause"},
/* fr */ {IDMS_PAUSE_FR, "Pause"},
/* hu */ {IDMS_PAUSE_HU, "Szünet"},
/* it */ {IDMS_PAUSE_IT, "Pausa"},
/* nl */ {IDMS_PAUSE_NL, "Pauze"},
/* pl */ {IDMS_PAUSE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PAUSE_SV, "Paus"},

/* en */ {IDMS_MONITOR,    "Monitor"},
/* de */ {IDMS_MONITOR_DE, "Monitor"},
/* fr */ {IDMS_MONITOR_FR, "Tracer"},
/* hu */ {IDMS_MONITOR_HU, "Monitor"},
/* it */ {IDMS_MONITOR_IT, "Monitor"},
/* nl */ {IDMS_MONITOR_NL, "Monitor"},
/* pl */ {IDMS_MONITOR_PL, ""},  /* fuzzy */
/* sv */ {IDMS_MONITOR_SV, "Monitor"},

/* en */ {IDMS_HARD,    "Hard"},
/* de */ {IDMS_HARD_DE, "Hart"},
/* fr */ {IDMS_HARD_FR, "Physiquement"},
/* hu */ {IDMS_HARD_HU, "Hideg"},
/* it */ {IDMS_HARD_IT, "Hard"},
/* nl */ {IDMS_HARD_NL, "Hard"},
/* pl */ {IDMS_HARD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_HARD_SV, "Hård"},

/* en */ {IDMS_SOFT,    "Soft"},
/* de */ {IDMS_SOFT_DE, "Soft"},
/* fr */ {IDMS_SOFT_FR, "Logiciellement"},
/* hu */ {IDMS_SOFT_HU, "Meleg"},
/* it */ {IDMS_SOFT_IT, "Soft"},
/* nl */ {IDMS_SOFT_NL, "Zacht"},
/* pl */ {IDMS_SOFT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SOFT_SV, "Mjuk"},

/* en */ {IDMS_EXIT,    "Exit"},
/* de */ {IDMS_EXIT_DE, "Exit"},
/* fr */ {IDMS_EXIT_FR, "Quitter"},
/* hu */ {IDMS_EXIT_HU, "Kilépés"},
/* it */ {IDMS_EXIT_IT, "Esci"},
/* nl */ {IDMS_EXIT_NL, "Afsluiten"},
/* pl */ {IDMS_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EXIT_SV, "Avsluta"},

/* en */ {IDMS_EDIT,    "Edit"},
/* de */ {IDMS_EDIT_DE, ""},  /* fuzzy */
/* fr */ {IDMS_EDIT_FR, ""},  /* fuzzy */
/* hu */ {IDMS_EDIT_HU, ""},  /* fuzzy */
/* it */ {IDMS_EDIT_IT, ""},  /* fuzzy */
/* nl */ {IDMS_EDIT_NL, "Bewerken"},
/* pl */ {IDMS_EDIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EDIT_SV, ""},  /* fuzzy */

/* en */ {IDMS_COPY,    "Copy"},
/* de */ {IDMS_COPY_DE, ""},  /* fuzzy */
/* fr */ {IDMS_COPY_FR, ""},  /* fuzzy */
/* hu */ {IDMS_COPY_HU, ""},  /* fuzzy */
/* it */ {IDMS_COPY_IT, ""},  /* fuzzy */
/* nl */ {IDMS_COPY_NL, "Kopie"},
/* pl */ {IDMS_COPY_PL, ""},  /* fuzzy */
/* sv */ {IDMS_COPY_SV, ""},  /* fuzzy */

/* en */ {IDMS_PASTE,    "Paste"},
/* de */ {IDMS_PASTE_DE, ""},  /* fuzzy */
/* fr */ {IDMS_PASTE_FR, ""},  /* fuzzy */
/* hu */ {IDMS_PASTE_HU, ""},  /* fuzzy */
/* it */ {IDMS_PASTE_IT, ""},  /* fuzzy */
/* nl */ {IDMS_PASTE_NL, "Plakken"},
/* pl */ {IDMS_PASTE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PASTE_SV, ""},  /* fuzzy */

/* en */ {IDMS_SNAPSHOT,    "Snapshot"},
/* de */ {IDMS_SNAPSHOT_DE, "Snapshot"},
/* fr */ {IDMS_SNAPSHOT_FR, "Sauvegarde"},
/* hu */ {IDMS_SNAPSHOT_HU, "Pillanatkép"},
/* it */ {IDMS_SNAPSHOT_IT, "Snapshot"},
/* nl */ {IDMS_SNAPSHOT_NL, "Momentopname"},
/* pl */ {IDMS_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SNAPSHOT_SV, "Ögonblicksbild"},

/* en */ {IDMS_LOAD_SNAPSHOT_IMAGE,    "Load snapshot image..."},
/* de */ {IDMS_LOAD_SNAPSHOT_IMAGE_DE, "Snapshot Image Laden..."},
/* fr */ {IDMS_LOAD_SNAPSHOT_IMAGE_FR, "Charger une sauvegarde...."},
/* hu */ {IDMS_LOAD_SNAPSHOT_IMAGE_HU, "Pillanatkép betöltése..."},
/* it */ {IDMS_LOAD_SNAPSHOT_IMAGE_IT, "Carica immagine dello snapshot..."},
/* nl */ {IDMS_LOAD_SNAPSHOT_IMAGE_NL, "Laad momentopnamebestand..."},
/* pl */ {IDMS_LOAD_SNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_SNAPSHOT_IMAGE_SV, "Läs in ögonblicksbild..."},

/* en */ {IDMS_SAVE_SNAPSHOT_IMAGE,    "Save snapshot image..."},
/* de */ {IDMS_SAVE_SNAPSHOT_IMAGE_DE, "Snapshot Image Speichern..."},
/* fr */ {IDMS_SAVE_SNAPSHOT_IMAGE_FR, "Enregistrer une sauvegarde..."},
/* hu */ {IDMS_SAVE_SNAPSHOT_IMAGE_HU, "Pillanatkép mentése..."},
/* it */ {IDMS_SAVE_SNAPSHOT_IMAGE_IT, "Salva l'immagine dello snapshot..."},
/* nl */ {IDMS_SAVE_SNAPSHOT_IMAGE_NL, "Opslaan momentopnamebestand..."},
/* pl */ {IDMS_SAVE_SNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_SNAPSHOT_IMAGE_SV, "Spara ögonblicksbild..."},

/* en */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE,    "Load quicksnapshot image"},
/* de */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_DE, "Schnellladen von Snapshot Image"},
/* fr */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_FR, "Charger la sauvegarde rapide"},
/* hu */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_HU, "Gyors pillanatkép betöltése"},
/* it */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_IT, "Carica immagine dello snapshot"},
/* nl */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_NL, "Snellaad momentopnamebestand"},
/* pl */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_SV, "Läs in snabbögonblicksbild"},

/* en */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE,    "Save quicksnapshot image"},
/* de */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_DE, "Schnellspeichern von Snapshot"},
/* fr */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_FR, "Enregistrer la sauvegarde rapide"},
/* hu */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_HU, "Gyors pillanatkép fájl mentése"},
/* it */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_IT, "Salva immagine dello snapshot"},
/* nl */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_NL, "Snelopslaan momentopnamebestand"},
/* pl */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_SV, "Spara snabbögonblicksbildfil"},

/* en */ {IDMS_START_STOP_RECORDING,    "Start/Stop Recording History"},
/* de */ {IDMS_START_STOP_RECORDING_DE, "Start/Stop Aufnahme History"},
/* fr */ {IDMS_START_STOP_RECORDING_FR, "Démarrer/Arrêter l'historique de l'enregistrement"},
/* hu */ {IDMS_START_STOP_RECORDING_HU, "Esemény felvétel indítása/megállítása"},
/* it */ {IDMS_START_STOP_RECORDING_IT, "Avvia/termina la registrazione della cronologia"},
/* nl */ {IDMS_START_STOP_RECORDING_NL, "Start/Stop Opnamegeschiedenis"},
/* pl */ {IDMS_START_STOP_RECORDING_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_STOP_RECORDING_SV, "Börja/sluta spela in händelser"},

/* en */ {IDMS_START_STOP_PLAYBACK,    "Start/Stop Playback History"},
/* de */ {IDMS_START_STOP_PLAYBACK_DE, "Start/Stop Wiedergabe History"},
/* fr */ {IDMS_START_STOP_PLAYBACK_FR, "Démarrer/Arrêter l'historique de la lecture"},
/* hu */ {IDMS_START_STOP_PLAYBACK_HU, "Esemény visszajátszás indítása/megállítása"},
/* it */ {IDMS_START_STOP_PLAYBACK_IT, "Avvia/termina la riproduzione della cronologia"},
/* nl */ {IDMS_START_STOP_PLAYBACK_NL, "Start/Stop Afspeelgeschiedenis"},
/* pl */ {IDMS_START_STOP_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_STOP_PLAYBACK_SV, "Börja/sluta spela upp händelser"},

/* en */ {IDMS_SET_MILESTONE,    "Set Recording Milestone"},
/* de */ {IDMS_SET_MILESTONE_DE, "Setze Aufnahme Meilenstein"},
/* fr */ {IDMS_SET_MILESTONE_FR, "Lever le Signet d'enregistrement"},
/* hu */ {IDMS_SET_MILESTONE_HU, "Kilóméterkõ elhelyezése a felvételben"},
/* it */ {IDMS_SET_MILESTONE_IT, "Imposta segnalibro"},
/* nl */ {IDMS_SET_MILESTONE_NL, "Zet Opname Mijlpaal"},
/* pl */ {IDMS_SET_MILESTONE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SET_MILESTONE_SV, "Ange inspelningsbokmärke"},

/* en */ {IDMS_RETURN_TO_MILESTONE,    "Return to Milestone"},
/* de */ {IDMS_RETURN_TO_MILESTONE_DE, "Rückkehr zum Meilenstein"},
/* fr */ {IDMS_RETURN_TO_MILESTONE_FR, "Retourner au Signet d'enregistrement"},
/* hu */ {IDMS_RETURN_TO_MILESTONE_HU, "Visszaugrás az elõzõ kilóméterkõhöz"},
/* it */ {IDMS_RETURN_TO_MILESTONE_IT, "Vai al segnalibro"},
/* nl */ {IDMS_RETURN_TO_MILESTONE_NL, "Ga terug naar Mijlpaal"},
/* pl */ {IDMS_RETURN_TO_MILESTONE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RETURN_TO_MILESTONE_SV, "Återgå till bokmärke"},

/* en */ {IDMS_RECORDING_START_MODE,    "Recording start mode"},
/* de */ {IDMS_RECORDING_START_MODE_DE, "Aufnahme Start Modus"},
/* fr */ {IDMS_RECORDING_START_MODE_FR, "Mode de départ d'enregistrement"},
/* hu */ {IDMS_RECORDING_START_MODE_HU, "Rögzítés indításának módja"},
/* it */ {IDMS_RECORDING_START_MODE_IT, "Modalità di inizio registrazione"},
/* nl */ {IDMS_RECORDING_START_MODE_NL, "Opname start modus"},
/* pl */ {IDMS_RECORDING_START_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RECORDING_START_MODE_SV, "Spelar in startläge"},

/* en */ {IDMS_SAVE_NEW_SNAPSHOT,    "Save new snapshot"},
/* de */ {IDMS_SAVE_NEW_SNAPSHOT_DE, "Speichere neuen Snapshot"},
/* fr */ {IDMS_SAVE_NEW_SNAPSHOT_FR, "Enregistrer une nouvelle sauvegarde"},
/* hu */ {IDMS_SAVE_NEW_SNAPSHOT_HU, "Új pillanatkép mentése"},
/* it */ {IDMS_SAVE_NEW_SNAPSHOT_IT, "Salva nuovo snapshot"},
/* nl */ {IDMS_SAVE_NEW_SNAPSHOT_NL, "Opslaan nieuwe momentopname"},
/* pl */ {IDMS_SAVE_NEW_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_NEW_SNAPSHOT_SV, "Spara ny ögonblicksbild"},

/* en */ {IDMS_LOAD_EXISTING_SNAPSHOT,    "Load existing snapshot"},
/* de */ {IDMS_LOAD_EXISTING_SNAPSHOT_DE, "Lade existierenden Snapshot"},
/* fr */ {IDMS_LOAD_EXISTING_SNAPSHOT_FR, "Charger une sauvegarde"},
/* hu */ {IDMS_LOAD_EXISTING_SNAPSHOT_HU, "Létezõ pillanatkép betöltése"},
/* it */ {IDMS_LOAD_EXISTING_SNAPSHOT_IT, "Carica snapshot esistente"},
/* nl */ {IDMS_LOAD_EXISTING_SNAPSHOT_NL, "Laad bestaande momentopname"},
/* pl */ {IDMS_LOAD_EXISTING_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_EXISTING_SNAPSHOT_SV, "Läs in existerande ögonblicksbild"},

/* en */ {IDMS_START_WITH_RESET,    "Start with reset"},
/* de */ {IDMS_START_WITH_RESET_DE, "Mit Reset starten"},
/* fr */ {IDMS_START_WITH_RESET_FR, "Réinitialiser au démarrage"},
/* hu */ {IDMS_START_WITH_RESET_HU, "Indulás RESET-tel"},
/* it */ {IDMS_START_WITH_RESET_IT, "Avvia con reset"},
/* nl */ {IDMS_START_WITH_RESET_NL, "Start bij reset"},
/* pl */ {IDMS_START_WITH_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_WITH_RESET_SV, "Starta med nollställning"},

/* en */ {IDMS_OVERWRITE_PLAYBACK,    "Overwrite Playback"},
/* de */ {IDMS_OVERWRITE_PLAYBACK_DE, "Laufende Aufnahme überschreiben"},
/* fr */ {IDMS_OVERWRITE_PLAYBACK_FR, "Écraser la lecture actuelle"},
/* hu */ {IDMS_OVERWRITE_PLAYBACK_HU, "Visszajátszás felülírása"},
/* it */ {IDMS_OVERWRITE_PLAYBACK_IT, "Sovrascrivi la riproduzione"},
/* nl */ {IDMS_OVERWRITE_PLAYBACK_NL, "Overschrijf weergave"},
/* pl */ {IDMS_OVERWRITE_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDMS_OVERWRITE_PLAYBACK_SV, "Skriv över aktiv uppspelning"},

/* en */ {IDMS_SELECT_HISTORY_DIR,    "Select History files/directory"},
/* de */ {IDMS_SELECT_HISTORY_DIR_DE, "Verzeichnis für Event Aufnahme"},
/* fr */ {IDMS_SELECT_HISTORY_DIR_FR, "Sélectionnez le répertoire des captures"},
/* hu */ {IDMS_SELECT_HISTORY_DIR_HU, "Válassza ki a felvételeket/könyvtárat"},
/* it */ {IDMS_SELECT_HISTORY_DIR_IT, "Seleziona i file/directory della cronologia"},
/* nl */ {IDMS_SELECT_HISTORY_DIR_NL, "Selecteer geschiedenisdirectory"},
/* pl */ {IDMS_SELECT_HISTORY_DIR_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SELECT_HISTORY_DIR_SV, "Välj historikfiler/-katalog"},

/* en */ {IDMS_SAVE_STOP_MEDIA_FILE,    "Save/stop media file..."},
/* de */ {IDMS_SAVE_STOP_MEDIA_FILE_DE, "Speichern/stop media Datei..."},
/* fr */ {IDMS_SAVE_STOP_MEDIA_FILE_FR, "Enregistrer/Arrêter fichier média..."},
/* hu */ {IDMS_SAVE_STOP_MEDIA_FILE_HU, "Média fájl mentése/megállítása..."},
/* it */ {IDMS_SAVE_STOP_MEDIA_FILE_IT, "Salva/arresta file multimediale..."},
/* nl */ {IDMS_SAVE_STOP_MEDIA_FILE_NL, "Opslaan/stop mediabestand..."},
/* pl */ {IDMS_SAVE_STOP_MEDIA_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_STOP_MEDIA_FILE_SV, "Starta/stoppa mediafil..."},

/* en */ {IDMS_NETPLAY,    "Netplay (experimental)..."},
/* de */ {IDMS_NETPLAY_DE, "Netplay (experimentell)..."},
/* fr */ {IDMS_NETPLAY_FR, ""},  /* fuzzy */
/* hu */ {IDMS_NETPLAY_HU, "Hálózati játék (kísérleti)..."},
/* it */ {IDMS_NETPLAY_IT, "Gioco in rete (sperimentale)..."},
/* nl */ {IDMS_NETPLAY_NL, "Netplay (experimenteel)..."},
/* pl */ {IDMS_NETPLAY_PL, ""},  /* fuzzy */
/* sv */ {IDMS_NETPLAY_SV, "Nätverksspel (experimentellt)..."},

/* en */ {IDMS_OPTIONS,    "Options"},
/* de */ {IDMS_OPTIONS_DE, "Optionen"},
/* fr */ {IDMS_OPTIONS_FR, "Options"},
/* hu */ {IDMS_OPTIONS_HU, "Opciók"},
/* it */ {IDMS_OPTIONS_IT, "Opzioni"},
/* nl */ {IDMS_OPTIONS_NL, "Opties"},
/* pl */ {IDMS_OPTIONS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_OPTIONS_SV, "Alternativ"},

/* en */ {IDMS_REFRESH_RATE,    "Refresh rate"},
/* de */ {IDMS_REFRESH_RATE_DE, "Wiederholrate"},
/* fr */ {IDMS_REFRESH_RATE_FR, "Taux de rafraîchissement"},
/* hu */ {IDMS_REFRESH_RATE_HU, "Frissítési ráta"},
/* it */ {IDMS_REFRESH_RATE_IT, "Frequenza di aggiornamento"},
/* nl */ {IDMS_REFRESH_RATE_NL, "Vernieuwings snelheid"},
/* pl */ {IDMS_REFRESH_RATE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_REFRESH_RATE_SV, "Uppdateringshastighet"},

/* en */ {IDMS_AUTO,    "Auto"},
/* de */ {IDMS_AUTO_DE, "Automatisch"},
/* fr */ {IDMS_AUTO_FR, "Automatique"},
/* hu */ {IDMS_AUTO_HU, "Automatikus"},
/* it */ {IDMS_AUTO_IT, "Auto"},
/* nl */ {IDMS_AUTO_NL, "Auto"},
/* pl */ {IDMS_AUTO_PL, ""},  /* fuzzy */
/* sv */ {IDMS_AUTO_SV, "Auto"},

/* en */ {IDMS_1_1,    "1/1"},
/* de */ {IDMS_1_1_DE, "1/1"},
/* fr */ {IDMS_1_1_FR, "1/1"},
/* hu */ {IDMS_1_1_HU, "1/1"},
/* it */ {IDMS_1_1_IT, "1/1"},
/* nl */ {IDMS_1_1_NL, "1/1"},
/* pl */ {IDMS_1_1_PL, "1/1"},
/* sv */ {IDMS_1_1_SV, "1/1"},

/* en */ {IDMS_1_2,    "1/2"},
/* de */ {IDMS_1_2_DE, "1/2"},
/* fr */ {IDMS_1_2_FR, "1/2"},
/* hu */ {IDMS_1_2_HU, "1/2"},
/* it */ {IDMS_1_2_IT, "1/2"},
/* nl */ {IDMS_1_2_NL, "1/2"},
/* pl */ {IDMS_1_2_PL, "1/2"},
/* sv */ {IDMS_1_2_SV, "1/2"},

/* en */ {IDMS_1_3,    "1/3"},
/* de */ {IDMS_1_3_DE, "1/3"},
/* fr */ {IDMS_1_3_FR, "1/3"},
/* hu */ {IDMS_1_3_HU, "1/3"},
/* it */ {IDMS_1_3_IT, "1/3"},
/* nl */ {IDMS_1_3_NL, "1/3"},
/* pl */ {IDMS_1_3_PL, "1/3"},
/* sv */ {IDMS_1_3_SV, "1/3"},

/* en */ {IDMS_1_4,    "1/4"},
/* de */ {IDMS_1_4_DE, "1/4"},
/* fr */ {IDMS_1_4_FR, "1/4"},
/* hu */ {IDMS_1_4_HU, "1/4"},
/* it */ {IDMS_1_4_IT, "1/4"},
/* nl */ {IDMS_1_4_NL, "1/4"},
/* pl */ {IDMS_1_4_PL, "1/4"},
/* sv */ {IDMS_1_4_SV, "1/4"},

/* en */ {IDMS_1_5,    "1/5"},
/* de */ {IDMS_1_5_DE, "1/5"},
/* fr */ {IDMS_1_5_FR, "1/5"},
/* hu */ {IDMS_1_5_HU, "1/5"},
/* it */ {IDMS_1_5_IT, "1/5"},
/* nl */ {IDMS_1_5_NL, "1/5"},
/* pl */ {IDMS_1_5_PL, "1/5"},
/* sv */ {IDMS_1_5_SV, "1/5"},

/* en */ {IDMS_1_6,    "1/6"},
/* de */ {IDMS_1_6_DE, "1/6"},
/* fr */ {IDMS_1_6_FR, "1/6"},
/* hu */ {IDMS_1_6_HU, "1/6"},
/* it */ {IDMS_1_6_IT, "1/6"},
/* nl */ {IDMS_1_6_NL, "1/6"},
/* pl */ {IDMS_1_6_PL, "1/6"},
/* sv */ {IDMS_1_6_SV, "1/6"},

/* en */ {IDMS_1_7,    "1/7"},
/* de */ {IDMS_1_7_DE, "1/7"},
/* fr */ {IDMS_1_7_FR, "1/7"},
/* hu */ {IDMS_1_7_HU, "1/7"},
/* it */ {IDMS_1_7_IT, "1/7"},
/* nl */ {IDMS_1_7_NL, "1/7"},
/* pl */ {IDMS_1_7_PL, "1/7"},
/* sv */ {IDMS_1_7_SV, "1/7"},

/* en */ {IDMS_1_8,    "1/8"},
/* de */ {IDMS_1_8_DE, "1/8"},
/* fr */ {IDMS_1_8_FR, "1/8"},
/* hu */ {IDMS_1_8_HU, "1/8"},
/* it */ {IDMS_1_8_IT, "1/8"},
/* nl */ {IDMS_1_8_NL, "1/8"},
/* pl */ {IDMS_1_8_PL, "1/8"},
/* sv */ {IDMS_1_8_SV, "1/8"},

/* en */ {IDMS_1_9,    "1/9"},
/* de */ {IDMS_1_9_DE, "1/9"},
/* fr */ {IDMS_1_9_FR, "1/9"},
/* hu */ {IDMS_1_9_HU, "1/9"},
/* it */ {IDMS_1_9_IT, "1/9"},
/* nl */ {IDMS_1_9_NL, "1/9"},
/* pl */ {IDMS_1_9_PL, "1/9"},
/* sv */ {IDMS_1_9_SV, "1/9"},

/* en */ {IDMS_1_10,    "1/10"},
/* de */ {IDMS_1_10_DE, "1/10"},
/* fr */ {IDMS_1_10_FR, "1/10"},
/* hu */ {IDMS_1_10_HU, "1/10"},
/* it */ {IDMS_1_10_IT, "1/10"},
/* nl */ {IDMS_1_10_NL, "1/10"},
/* pl */ {IDMS_1_10_PL, "1/10"},
/* sv */ {IDMS_1_10_SV, "1/10"},

/* en */ {IDMS_MAXIMUM_SPEED,    "Maximum Speed"},
/* de */ {IDMS_MAXIMUM_SPEED_DE, "Maximale Geschwindigkeit"},
/* fr */ {IDMS_MAXIMUM_SPEED_FR, "Vitesse Maximale"},
/* hu */ {IDMS_MAXIMUM_SPEED_HU, "Maximális sebesség"},
/* it */ {IDMS_MAXIMUM_SPEED_IT, "Velocità massima"},
/* nl */ {IDMS_MAXIMUM_SPEED_NL, "Maximale Snelheid"},
/* pl */ {IDMS_MAXIMUM_SPEED_PL, ""},  /* fuzzy */
/* sv */ {IDMS_MAXIMUM_SPEED_SV, "Maximal hastighet"},

/* en */ {IDMS_200_PERCENT,    "200%"},
/* de */ {IDMS_200_PERCENT_DE, "200%"},
/* fr */ {IDMS_200_PERCENT_FR, "200%"},
/* hu */ {IDMS_200_PERCENT_HU, "200%"},
/* it */ {IDMS_200_PERCENT_IT, "200%"},
/* nl */ {IDMS_200_PERCENT_NL, "200%"},
/* pl */ {IDMS_200_PERCENT_PL, "200%"},
/* sv */ {IDMS_200_PERCENT_SV, "200%"},

/* en */ {IDMS_100_PERCENT,    "100%"},
/* de */ {IDMS_100_PERCENT_DE, "100%"},
/* fr */ {IDMS_100_PERCENT_FR, "100%"},
/* hu */ {IDMS_100_PERCENT_HU, "100%"},
/* it */ {IDMS_100_PERCENT_IT, "100%"},
/* nl */ {IDMS_100_PERCENT_NL, "100%"},
/* pl */ {IDMS_100_PERCENT_PL, "100%"},
/* sv */ {IDMS_100_PERCENT_SV, "100%"},

/* en */ {IDMS_50_PERCENT,    "50%"},
/* de */ {IDMS_50_PERCENT_DE, "50%"},
/* fr */ {IDMS_50_PERCENT_FR, "50%"},
/* hu */ {IDMS_50_PERCENT_HU, "50%"},
/* it */ {IDMS_50_PERCENT_IT, "50%"},
/* nl */ {IDMS_50_PERCENT_NL, "50%"},
/* pl */ {IDMS_50_PERCENT_PL, "50%"},
/* sv */ {IDMS_50_PERCENT_SV, "50%"},

/* en */ {IDMS_20_PERCENT,    "20%"},
/* de */ {IDMS_20_PERCENT_DE, "20%"},
/* fr */ {IDMS_20_PERCENT_FR, "20%"},
/* hu */ {IDMS_20_PERCENT_HU, "20%"},
/* it */ {IDMS_20_PERCENT_IT, "20%"},
/* nl */ {IDMS_20_PERCENT_NL, "20%"},
/* pl */ {IDMS_20_PERCENT_PL, "20%"},
/* sv */ {IDMS_20_PERCENT_SV, "20%"},

/* en */ {IDMS_10_PERCENT,    "10%"},
/* de */ {IDMS_10_PERCENT_DE, "10%"},
/* fr */ {IDMS_10_PERCENT_FR, "10%"},
/* hu */ {IDMS_10_PERCENT_HU, "10%"},
/* it */ {IDMS_10_PERCENT_IT, "10%"},
/* nl */ {IDMS_10_PERCENT_NL, "10%"},
/* pl */ {IDMS_10_PERCENT_PL, "10%"},
/* sv */ {IDMS_10_PERCENT_SV, "10%"},

/* en */ {IDMS_NO_LIMIT,    "No limit"},
/* de */ {IDMS_NO_LIMIT_DE, "Kein Limit"},
/* fr */ {IDMS_NO_LIMIT_FR, "Aucune limite"},
/* hu */ {IDMS_NO_LIMIT_HU, "Nincs határ"},
/* it */ {IDMS_NO_LIMIT_IT, "Nessun limite"},
/* nl */ {IDMS_NO_LIMIT_NL, "Geen limiet"},
/* pl */ {IDMS_NO_LIMIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_NO_LIMIT_SV, "Ingen gräns"},

/* en */ {IDMS_CUSTOM,    "Custom"},
/* de */ {IDMS_CUSTOM_DE, "Benutzerdefiniert"},
/* fr */ {IDMS_CUSTOM_FR, "Personnalisé"},
/* hu */ {IDMS_CUSTOM_HU, "Egyéni"},
/* it */ {IDMS_CUSTOM_IT, "Personalizzato"},
/* nl */ {IDMS_CUSTOM_NL, "Aangepast"},
/* pl */ {IDMS_CUSTOM_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CUSTOM_SV, "Egen"},

/* en */ {IDMS_WARP_MODE,    "Warp mode"},
/* de */ {IDMS_WARP_MODE_DE, "Warp modus"},
/* fr */ {IDMS_WARP_MODE_FR, "Mode turbo"},
/* hu */ {IDMS_WARP_MODE_HU, "Hipergyors mód"},
/* it */ {IDMS_WARP_MODE_IT, "Modalità turbo"},
/* nl */ {IDMS_WARP_MODE_NL, "Warpmodus"},
/* pl */ {IDMS_WARP_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_WARP_MODE_SV, "Warpläge"},

/* en */ {IDMS_FULLSCREEN,    "Fullscreen"},
/* de */ {IDMS_FULLSCREEN_DE, "Vollbild"},
/* fr */ {IDMS_FULLSCREEN_FR, "Périphérique plein écran"},
/* hu */ {IDMS_FULLSCREEN_HU, "Teljes képernyõ"},
/* it */ {IDMS_FULLSCREEN_IT, "Schermo intero"},
/* nl */ {IDMS_FULLSCREEN_NL, "Volscherm"},
/* pl */ {IDMS_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FULLSCREEN_SV, "Fullskärm"},

/* en */ {IDMS_VIDEO_CACHE,    "Video cache"},
/* de */ {IDMS_VIDEO_CACHE_DE, "Video cache"},
/* fr */ {IDMS_VIDEO_CACHE_FR, "Cache vidéo"},
/* hu */ {IDMS_VIDEO_CACHE_HU, "Kép gyorsítótár"},
/* it */ {IDMS_VIDEO_CACHE_IT, "Cache video"},
/* nl */ {IDMS_VIDEO_CACHE_NL, "Videocache"},
/* pl */ {IDMS_VIDEO_CACHE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIDEO_CACHE_SV, "Grafikcache"},

/* en */ {IDMS_DOUBLE_SIZE,    "Double size"},
/* de */ {IDMS_DOUBLE_SIZE_DE, "Doppelte Größe"},
/* fr */ {IDMS_DOUBLE_SIZE_FR, "Taille double"},
/* hu */ {IDMS_DOUBLE_SIZE_HU, "Dupla méret"},
/* it */ {IDMS_DOUBLE_SIZE_IT, "Dimensione doppia"},
/* nl */ {IDMS_DOUBLE_SIZE_NL, "Dubbele grootte"},
/* pl */ {IDMS_DOUBLE_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DOUBLE_SIZE_SV, "Dubbel storlek"},

/* en */ {IDMS_DOUBLE_SCAN,    "Double scan"},
/* de */ {IDMS_DOUBLE_SCAN_DE, "Doppelt Scan"},
/* fr */ {IDMS_DOUBLE_SCAN_FR, "Double scan"},
/* hu */ {IDMS_DOUBLE_SCAN_HU, "Dupla pásztázás"},
/* it */ {IDMS_DOUBLE_SCAN_IT, "Scansione doppia"},
/* nl */ {IDMS_DOUBLE_SCAN_NL, "Dubbele scan"},
/* pl */ {IDMS_DOUBLE_SCAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DOUBLE_SCAN_SV, "Dubbelskanning"},

/* en */ {IDMS_SCALE2X,    "Scale2x"},
/* de */ {IDMS_SCALE2X_DE, "Scale2x"},
/* fr */ {IDMS_SCALE2X_FR, "Scale2x"},
/* hu */ {IDMS_SCALE2X_HU, "Élsimítás"},
/* it */ {IDMS_SCALE2X_IT, "Scale2x"},
/* nl */ {IDMS_SCALE2X_NL, "Schaal2x"},
/* pl */ {IDMS_SCALE2X_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SCALE2X_SV, "Scale2x"},

/* en */ {IDMS_VDC_SETTINGS,    "VDC settings"},
/* de */ {IDMS_VDC_SETTINGS_DE, "VDC Einstellungen"},
/* fr */ {IDMS_VDC_SETTINGS_FR, "Paramètres VDC"},
/* it */ {IDMS_VDC_SETTINGS_IT, "Impostazioni VDC"},
/* hu */ {IDMS_VDC_SETTINGS_HU, "VDC beállításai"},
/* nl */ {IDMS_VDC_SETTINGS_NL, "VDC-instellingen"},
/* pl */ {IDMS_VDC_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VDC_SETTINGS_SV, "VDC-inställningar"},

/* en */ {IDMS_64KB_VIDEO_MEMORY,    "64KB video memory"},
/* de */ {IDMS_64KB_VIDEO_MEMORY_DE, "64KB Bildschirm Speicher"},
/* fr */ {IDMS_64KB_VIDEO_MEMORY_FR, "Mémoire d'affichage de 64Ko"},
/* it */ {IDMS_64KB_VIDEO_MEMORY_IT, "64KB di memoria video"},
/* hu */ {IDMS_64KB_VIDEO_MEMORY_HU, "64KB képernyõ memória"},
/* nl */ {IDMS_64KB_VIDEO_MEMORY_NL, "64KB videogeheugen"},
/* pl */ {IDMS_64KB_VIDEO_MEMORY_PL, ""},  /* fuzzy */
/* sv */ {IDMS_64KB_VIDEO_MEMORY_SV, "64KB grafikminne"},

/* en */ {IDMS_SWAP_JOYSTICKS,    "Swap Joysticks"},
/* de */ {IDMS_SWAP_JOYSTICKS_DE, "Joystick ports austauschen"},
/* fr */ {IDMS_SWAP_JOYSTICKS_FR, "Interchanger les joysticks"},
/* hu */ {IDMS_SWAP_JOYSTICKS_HU, "Botkormányok felcserélése"},
/* it */ {IDMS_SWAP_JOYSTICKS_IT, "Scambia joystick"},
/* nl */ {IDMS_SWAP_JOYSTICKS_NL, "Verwissel joysticks"},
/* pl */ {IDMS_SWAP_JOYSTICKS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SWAP_JOYSTICKS_SV, "Växla spelportarna"},

/* en */ {IDMS_SOUND_PLAYBACK,    "Sound playback"},
/* de */ {IDMS_SOUND_PLAYBACK_DE, "Sound Einschalten"},
/* fr */ {IDMS_SOUND_PLAYBACK_FR, "Lecture audio"},
/* hu */ {IDMS_SOUND_PLAYBACK_HU, "Hangok engedélyezése"},
/* it */ {IDMS_SOUND_PLAYBACK_IT, "Riproduzione del suono"},
/* nl */ {IDMS_SOUND_PLAYBACK_NL, "Geluidsuitvoer"},
/* pl */ {IDMS_SOUND_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SOUND_PLAYBACK_SV, "Aktivera ljudåtergivning"},

/* en */ {IDMS_TRUE_DRIVE_EMU,    "True drive emulation"},
/* de */ {IDMS_TRUE_DRIVE_EMU_DE, "Präzise Floppy Emulation"},
/* fr */ {IDMS_TRUE_DRIVE_EMU_FR, "Activer l'émulation réelle des lecteurs"},
/* hu */ {IDMS_TRUE_DRIVE_EMU_HU, "Valós lemezegység emuláció"},
/* it */ {IDMS_TRUE_DRIVE_EMU_IT, "Attiva l'emulazione hardware dei drive"},
/* nl */ {IDMS_TRUE_DRIVE_EMU_NL, "Hardwarematige driveemulatie"},
/* pl */ {IDMS_TRUE_DRIVE_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDMS_TRUE_DRIVE_EMU_SV, "Äkta diskettenhetsemulering"},

/* en */ {IDMS_VIRTUAL_DEVICE_TRAPS,    "Virtual device traps"},
/* de */ {IDMS_VIRTUAL_DEVICE_TRAPS_DE, "Virtuelle Geräte Traps"},
/* fr */ {IDMS_VIRTUAL_DEVICE_TRAPS_FR, "Activer les périphériques virtuels"},
/* hu */ {IDMS_VIRTUAL_DEVICE_TRAPS_HU, "Virtuális eszközök ciklus kihagyása"},
/* it */ {IDMS_VIRTUAL_DEVICE_TRAPS_IT, "Attiva le periferiche virtuali"},
/* nl */ {IDMS_VIRTUAL_DEVICE_TRAPS_NL, "Virtuele apparaattraps"},
/* pl */ {IDMS_VIRTUAL_DEVICE_TRAPS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIRTUAL_DEVICE_TRAPS_SV, "Virtuella enhetstrap:ar"},

/* en */ {IDMS_DRIVE_SYNC_FACTOR,    "Drive sync factor"},
/* de */ {IDMS_DRIVE_SYNC_FACTOR_DE, "Laufwerkt Sync Faktor"},
/* fr */ {IDMS_DRIVE_SYNC_FACTOR_FR, "Facteur de synchro du lecteur"},
/* hu */ {IDMS_DRIVE_SYNC_FACTOR_HU, "Lemezegység szinkron faktor"},
/* it */ {IDMS_DRIVE_SYNC_FACTOR_IT, "Fattore di sincronizzazione del drive"},
/* nl */ {IDMS_DRIVE_SYNC_FACTOR_NL, "Drive synchronisatiefactor"},
/* pl */ {IDMS_DRIVE_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_SYNC_FACTOR_SV, "Synkfaktor för diskettstation"},

/* en */ {IDMS_PAL,    "PAL"},
/* de */ {IDMS_PAL_DE, "PAL"},
/* fr */ {IDMS_PAL_FR, "PAL"},
/* hu */ {IDMS_PAL_HU, "PAL"},
/* it */ {IDMS_PAL_IT, "PAL"},
/* nl */ {IDMS_PAL_NL, "PAL"},
/* pl */ {IDMS_PAL_PL, "PAL"},
/* sv */ {IDMS_PAL_SV, "PAL"},

/* en */ {IDMS_NTSC,    "NTSC"},
/* de */ {IDMS_NTSC_DE, "NTSC"},
/* fr */ {IDMS_NTSC_FR, "NTSC"},
/* hu */ {IDMS_NTSC_HU, "NTSC"},
/* it */ {IDMS_NTSC_IT, "NTSC"},
/* nl */ {IDMS_NTSC_NL, "NTSC"},
/* pl */ {IDMS_NTSC_PL, "NTSC"},
/* sv */ {IDMS_NTSC_SV, "NTSC"},

/* en */ {IDMS_VIDEO_STANDARD,    "Video standard"},
/* de */ {IDMS_VIDEO_STANDARD_DE, "Video standard"},
/* fr */ {IDMS_VIDEO_STANDARD_FR, "Standard vidéo"},
/* hu */ {IDMS_VIDEO_STANDARD_HU, "Videó szabvány"},
/* it */ {IDMS_VIDEO_STANDARD_IT, "Standard video"},
/* nl */ {IDMS_VIDEO_STANDARD_NL, "Videostandaard"},
/* pl */ {IDMS_VIDEO_STANDARD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIDEO_STANDARD_SV, "TV-standard"},

/* en */ {IDMS_PAL_G,    "PAL-G"},
/* de */ {IDMS_PAL_G_DE, "PAL-G"},
/* fr */ {IDMS_PAL_G_FR, "PAL-G"},
/* hu */ {IDMS_PAL_G_HU, "PAL-G"},
/* it */ {IDMS_PAL_G_IT, "PAL-G"},
/* nl */ {IDMS_PAL_G_NL, "PAL-G"},
/* pl */ {IDMS_PAL_G_PL, "PAL-G"},
/* sv */ {IDMS_PAL_G_SV, "PAL-G"},

/* en */ {IDMS_NTSC_M,    "NTSC-M"},
/* de */ {IDMS_NTSC_M_DE, "NTSC-M"},
/* fr */ {IDMS_NTSC_M_FR, "NTSC-M"},
/* hu */ {IDMS_NTSC_M_HU, "NTSC-M"},
/* it */ {IDMS_NTSC_M_IT, "NTSC-M"},
/* nl */ {IDMS_NTSC_M_NL, "NTSC-M"},
/* pl */ {IDMS_NTSC_M_PL, "NTSC-M"},
/* sv */ {IDMS_NTSC_M_SV, "NTSC-M"},

/* en */ {IDMS_OLD_NTSC_M,    "Old NTSC-M"},
/* de */ {IDMS_OLD_NTSC_M_DE, "NTSC-M alt"},
/* fr */ {IDMS_OLD_NTSC_M_FR, "Ancien NTSC-M"},
/* hu */ {IDMS_OLD_NTSC_M_HU, "Régi NTSC-M"},
/* it */ {IDMS_OLD_NTSC_M_IT, "NTSC-M vecchio"},
/* nl */ {IDMS_OLD_NTSC_M_NL, "Oud NTSC-M"},
/* pl */ {IDMS_OLD_NTSC_M_PL, ""},  /* fuzzy */
/* sv */ {IDMS_OLD_NTSC_M_SV, "Gammal NTSC-M"},

/* en */ {IDMS_EMU_ID,    "Emulator Identification"},
/* de */ {IDMS_EMU_ID_DE, "Emulator Identifikation"},
/* fr */ {IDMS_EMU_ID_FR, "Identification de l'émulateur"},
/* hu */ {IDMS_EMU_ID_HU, "Emulátor azonosító"},
/* it */ {IDMS_EMU_ID_IT, "Identificazione dell'emulatore"},
/* nl */ {IDMS_EMU_ID_NL, "Emulatoridentificatie"},
/* pl */ {IDMS_EMU_ID_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EMU_ID_SV, "Emulatoridentifiering"},

/* en */ {IDMS_VIC_1112_IEEE_488,    "VIC-1112 IEEE 488 module"},
/* de */ {IDMS_VIC_1112_IEEE_488_DE, "VIC-1112 IEEE 488 Modul"},
/* fr */ {IDMS_VIC_1112_IEEE_488_FR, "Module VIC-1112 IEEE 488"},
/* hu */ {IDMS_VIC_1112_IEEE_488_HU, "VIC-1112 IEEE 488 modul"},
/* it */ {IDMS_VIC_1112_IEEE_488_IT, "Modulo VIC-1112 IEEE 488"},
/* nl */ {IDMS_VIC_1112_IEEE_488_NL, "VIC-1112 IEEE 488-module"},
/* pl */ {IDMS_VIC_1112_IEEE_488_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIC_1112_IEEE_488_SV, "VIC-1112 IEEE 488-modul"},

/* en */ {IDMS_IEEE_488_INTERFACE,    "IEEE 488 Interface emulation"},
/* de */ {IDMS_IEEE_488_INTERFACE_DE, "IEEE488 Schnittstellen-Emulation"},
/* fr */ {IDMS_IEEE_488_INTERFACE_FR, "Interface d'émulation IEEE488"},
/* hu */ {IDMS_IEEE_488_INTERFACE_HU, "IEEE488 interfész emuláció"},
/* it */ {IDMS_IEEE_488_INTERFACE_IT, "Emulazione dell'interfaccia IEEE 488"},
/* nl */ {IDMS_IEEE_488_INTERFACE_NL, "IEEE 488 interfaceemulatie"},
/* pl */ {IDMS_IEEE_488_INTERFACE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_IEEE_488_INTERFACE_SV, "Emulering av IEEE488-gränssnitt"},

/* en */ {IDMS_GRAB_MOUSE,    "Grab mouse events"},
/* de */ {IDMS_GRAB_MOUSE_DE, "Maus Ereignisse abfangen"},
/* fr */ {IDMS_GRAB_MOUSE_FR, ""},  /* fuzzy */
/* hu */ {IDMS_GRAB_MOUSE_HU, "Egér események elkapása"},
/* it */ {IDMS_GRAB_MOUSE_IT, ""},  /* fuzzy */
/* nl */ {IDMS_GRAB_MOUSE_NL, "Gebruik de muis"},
/* pl */ {IDMS_GRAB_MOUSE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_GRAB_MOUSE_SV, "Fånga mushändelser"},

/* en */ {IDMS_SETTINGS,    "Settings"},
/* de */ {IDMS_SETTINGS_DE, "Einstellungen"},
/* fr */ {IDMS_SETTINGS_FR, "Paramètres"},
/* hu */ {IDMS_SETTINGS_HU, "Beállítások"},
/* it */ {IDMS_SETTINGS_IT, "Impostazioni"},
/* nl */ {IDMS_SETTINGS_NL, "Instellingen"},
/* pl */ {IDMS_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SETTINGS_SV, "Inställningar"},

/* en */ {IDMS_VIDEO_SETTINGS,    "Video settings..."},
/* de */ {IDMS_VIDEO_SETTINGS_DE, "Video Einstellungen..."},
/* fr */ {IDMS_VIDEO_SETTINGS_FR, "Paramètres vidéo..."},
/* hu */ {IDMS_VIDEO_SETTINGS_HU, "Videó beállításai..."},
/* it */ {IDMS_VIDEO_SETTINGS_IT, "Impostazioni video..."},
/* nl */ {IDMS_VIDEO_SETTINGS_NL, "Video-instellingen..."},
/* pl */ {IDMS_VIDEO_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIDEO_SETTINGS_SV, "Grafikinställningar..."},

/* en */ {IDMS_VIC_SETTINGS,    "VIC settings..."},
/* de */ {IDMS_VIC_SETTINGS_DE, "VIC Einstellungen..."},
/* fr */ {IDMS_VIC_SETTINGS_FR, "Paramètres VIC..."},
/* hu */ {IDMS_VIC_SETTINGS_HU, "VIC beállításai..."},
/* it */ {IDMS_VIC_SETTINGS_IT, "Impostazioni VIC..."},
/* nl */ {IDMS_VIC_SETTINGS_NL, "VIC-instellingen..."},
/* pl */ {IDMS_VIC_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIC_SETTINGS_SV, "VIC-inställningar..."},

/* en */ {IDMS_CBM2_SETTINGS,    "CBM2 settings..."},
/* de */ {IDMS_CBM2_SETTINGS_DE, "CBM2 Einstellungen..."},
/* fr */ {IDMS_CBM2_SETTINGS_FR, "Paramètres CBM2..."},
/* hu */ {IDMS_CBM2_SETTINGS_HU, "CBM beállításai..."},
/* it */ {IDMS_CBM2_SETTINGS_IT, "Impostazioni CBM2..."},
/* nl */ {IDMS_CBM2_SETTINGS_NL, "CBM2-instellingen..."},
/* pl */ {IDMS_CBM2_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CBM2_SETTINGS_SV, "CBM2-inställningar..."},

/* en */ {IDMS_SET_PET_MODEL,    "Change PET model"},
/* de */ {IDMS_SET_PET_MODEL_DE, "PET Modell verändern"},
/* fr */ {IDMS_SET_PET_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDMS_SET_PET_MODEL_HU, "PET modell kiválasztása"},
/* it */ {IDMS_SET_PET_MODEL_IT, "Cambia modello di PET"},
/* nl */ {IDMS_SET_PET_MODEL_NL, "Verander PET-model"},
/* pl */ {IDMS_SET_PET_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SET_PET_MODEL_SV, "Byt PET-modell"},

/* en */ {IDMS_PET_SETTINGS,    "PET settings..."},
/* de */ {IDMS_PET_SETTINGS_DE, "PET Einstellungen..."},
/* fr */ {IDMS_PET_SETTINGS_FR, "Paramètres PET..."},
/* hu */ {IDMS_PET_SETTINGS_HU, "PET beállításai..."},
/* it */ {IDMS_PET_SETTINGS_IT, "Impostazioni PET..."},
/* nl */ {IDMS_PET_SETTINGS_NL, "PET-instellingen..."},
/* pl */ {IDMS_PET_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PET_SETTINGS_SV, "PET-inställningar..."},

/* en */ {IDMS_PERIPHERAL_SETTINGS,    "Peripheral settings..."},
/* de */ {IDMS_PERIPHERAL_SETTINGS_DE, "Peripherie Einstellungen..."},
/* fr */ {IDMS_PERIPHERAL_SETTINGS_FR, "Paramètres des périphériques..."},
/* hu */ {IDMS_PERIPHERAL_SETTINGS_HU, "Perifériák beállításai..."},
/* it */ {IDMS_PERIPHERAL_SETTINGS_IT, "Impostazioni periferiche..."},
/* nl */ {IDMS_PERIPHERAL_SETTINGS_NL, "Randapparateninstellingen..."},
/* pl */ {IDMS_PERIPHERAL_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PERIPHERAL_SETTINGS_SV, "Inställningar för kringutrustning..."},

/* en */ {IDMS_DRIVE_SETTINGS,    "Drive settings..."},
/* de */ {IDMS_DRIVE_SETTINGS_DE, "Floppy Einstellungen..."},
/* fr */ {IDMS_DRIVE_SETTINGS_FR, "Paramètres des lecteurs..."},
/* hu */ {IDMS_DRIVE_SETTINGS_HU, "Lemezegység beállításai..."},
/* it */ {IDMS_DRIVE_SETTINGS_IT, "Impostazioni drive..."},
/* nl */ {IDMS_DRIVE_SETTINGS_NL, "Drive-instellingen..."},
/* pl */ {IDMS_DRIVE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_SETTINGS_SV, "Diskettenhetsinställningar..."},

/* en */ {IDMS_DATASETTE_SETTINGS,    "Datasette settings..."},
/* de */ {IDMS_DATASETTE_SETTINGS_DE, "Bandlaufwerk Einstellungen..."},
/* fr */ {IDMS_DATASETTE_SETTINGS_FR, "Paramètres du datassette..."},
/* hu */ {IDMS_DATASETTE_SETTINGS_HU, "Magnó beállításai..."},
/* it */ {IDMS_DATASETTE_SETTINGS_IT, "Impostazioni registratore..."},
/* nl */ {IDMS_DATASETTE_SETTINGS_NL, "Datasette-instellingen..."},
/* pl */ {IDMS_DATASETTE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DATASETTE_SETTINGS_SV, "Datasetteinställningar..."},

/* en */ {IDMS_PLUS4_SETTINGS,    "Plus4 settings..."},
/* de */ {IDMS_PLUS4_SETTINGS_DE, "Plus4 Einstellungen..."},
/* fr */ {IDMS_PLUS4_SETTINGS_FR, "Paramètres Plus4..."},
/* hu */ {IDMS_PLUS4_SETTINGS_HU, "Plus4 beállításai..."},
/* it */ {IDMS_PLUS4_SETTINGS_IT, "Impostazioni Plus4..."},
/* nl */ {IDMS_PLUS4_SETTINGS_NL, "Plus4-instellingen..."},
/* pl */ {IDMS_PLUS4_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PLUS4_SETTINGS_SV, "Plus4-inställningar..."},

/* en */ {IDMS_VICII_SETTINGS,    "VIC-II settings..."},
/* de */ {IDMS_VICII_SETTINGS_DE, "VIC-II Einstellungen..."},
/* fr */ {IDMS_VICII_SETTINGS_FR, "Paramètres VIC-II..."},
/* hu */ {IDMS_VICII_SETTINGS_HU, "VIC-II beállításai..."},
/* it */ {IDMS_VICII_SETTINGS_IT, "Impostazioni VIC-II..."},
/* nl */ {IDMS_VICII_SETTINGS_NL, "VIC-II-instellingen..."},
/* pl */ {IDMS_VICII_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VICII_SETTINGS_SV, "VIC-II-inställningar..."},

/* en */ {IDMS_JOYSTICK_SETTINGS,    "Joystick settings..."},
/* de */ {IDMS_JOYSTICK_SETTINGS_DE, "Joystick Einstellungen..."},
/* fr */ {IDMS_JOYSTICK_SETTINGS_FR, "Paramètres des joysticks..."},
/* hu */ {IDMS_JOYSTICK_SETTINGS_HU, "Botkormány beállításai..."},
/* it */ {IDMS_JOYSTICK_SETTINGS_IT, "Impostazioni joystick..."},
/* nl */ {IDMS_JOYSTICK_SETTINGS_NL, "Joystick-instellingen..."},
/* pl */ {IDMS_JOYSTICK_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_JOYSTICK_SETTINGS_SV, "Inställningar för joystick..."},

/* en */ {IDMS_JOYSTICK_DEVICE_SELECT,    "Joystick device selection"},
/* de */ {IDMS_JOYSTICK_DEVICE_SELECT_DE, "Joystick Geräte Auswahl"},
/* fr */ {IDMS_JOYSTICK_DEVICE_SELECT_FR, ""},  /* fuzzy */
/* hu */ {IDMS_JOYSTICK_DEVICE_SELECT_HU, "Botkormány eszköz kiválasztása"},
/* it */ {IDMS_JOYSTICK_DEVICE_SELECT_IT, "Selezione dispositivo joystick"},
/* nl */ {IDMS_JOYSTICK_DEVICE_SELECT_NL, "Joystickapparaatselectie"},
/* pl */ {IDMS_JOYSTICK_DEVICE_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_JOYSTICK_DEVICE_SELECT_SV, "enhetsinställningar för joystick"},

/* en */ {IDMS_JOYSTICK_FIRE_SELECT,    "Joystick fire button selection"},
/* de */ {IDMS_JOYSTICK_FIRE_SELECT_DE, "Joystick Feuerknopf Wahl"},
/* fr */ {IDMS_JOYSTICK_FIRE_SELECT_FR, ""},  /* fuzzy */
/* hu */ {IDMS_JOYSTICK_FIRE_SELECT_HU, "Botkormány tûzgomb kiválasztás"},
/* it */ {IDMS_JOYSTICK_FIRE_SELECT_IT, "Selezione pulsante di fuoco del joystick"},
/* nl */ {IDMS_JOYSTICK_FIRE_SELECT_NL, "Joystickvuurknopselectie"},
/* pl */ {IDMS_JOYSTICK_FIRE_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_JOYSTICK_FIRE_SELECT_SV, "val för joystickens fire-knapp"},

/* en */ {IDMS_KEYBOARD_SETTINGS,    "Keyboard settings..."},
/* de */ {IDMS_KEYBOARD_SETTINGS_DE, "Tastatur Einstellungen..."},
/* fr */ {IDMS_KEYBOARD_SETTINGS_FR, "Paramètres du clavier..."},
/* hu */ {IDMS_KEYBOARD_SETTINGS_HU, "Billentyûzet beállításai..."},
/* it */ {IDMS_KEYBOARD_SETTINGS_IT, "Impostazioni tastiera..."},
/* nl */ {IDMS_KEYBOARD_SETTINGS_NL, "Toetsenbordinstellingen..."},
/* pl */ {IDMS_KEYBOARD_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_KEYBOARD_SETTINGS_SV, "Tangentbordsinställningar..."},

/* en */ {IDMS_MOUSE_SETTINGS,    "Mouse settings..."},
/* de */ {IDMS_MOUSE_SETTINGS_DE, "Maus Einstellungen..."},
/* fr */ {IDMS_MOUSE_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDMS_MOUSE_SETTINGS_HU, "Egér beállításai..."},
/* it */ {IDMS_MOUSE_SETTINGS_IT, ""},  /* fuzzy */
/* nl */ {IDMS_MOUSE_SETTINGS_NL, "Muis instellingen..."},
/* pl */ {IDMS_MOUSE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_MOUSE_SETTINGS_SV, "Musinställningar..."},

/* en */ {IDMS_SOUND_SETTINGS,    "Sound settings..."},
/* de */ {IDMS_SOUND_SETTINGS_DE, "Sound Einstellungen..."},
/* fr */ {IDMS_SOUND_SETTINGS_FR, "Paramètres son..."},
/* hu */ {IDMS_SOUND_SETTINGS_HU, "Hang beállításai..."},
/* it */ {IDMS_SOUND_SETTINGS_IT, "Impostazioni audio..."},
/* nl */ {IDMS_SOUND_SETTINGS_NL, "Geluidsinstellingen..."},
/* pl */ {IDMS_SOUND_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SOUND_SETTINGS_SV, "Ljudinställningar..."},

/* en */ {IDMS_SID_SETTINGS,    "SID settings..."},
/* de */ {IDMS_SID_SETTINGS_DE, "SID Einstellungen..."},
/* fr */ {IDMS_SID_SETTINGS_FR, "Paramètres SID..."},
/* hu */ {IDMS_SID_SETTINGS_HU, "SID beállításai..."},
/* it */ {IDMS_SID_SETTINGS_IT, "Impostazioni SID..."},
/* nl */ {IDMS_SID_SETTINGS_NL, "SID-instellingen..."},
/* pl */ {IDMS_SID_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SID_SETTINGS_SV, "SID-inställningar..."},

/* en */ {IDMS_COMPUTER_ROM_SETTINGS,    "Computer ROM settings..."},
/* de */ {IDMS_COMPUTER_ROM_SETTINGS_DE, "Computer ROM Einstellungen..."},
/* fr */ {IDMS_COMPUTER_ROM_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDMS_COMPUTER_ROM_SETTINGS_HU, "ROM-ok beállításai..."},
/* it */ {IDMS_COMPUTER_ROM_SETTINGS_IT, "Impostazioni ROM del computer..."},
/* nl */ {IDMS_COMPUTER_ROM_SETTINGS_NL, "Computer-ROM-instellingen..."},
/* pl */ {IDMS_COMPUTER_ROM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_COMPUTER_ROM_SETTINGS_SV, "ROM-inställningar för dator..."},

/* en */ {IDMS_DRIVE_ROM_SETTINGS,    "Drive ROM settings..."},
/* de */ {IDMS_DRIVE_ROM_SETTINGS_DE, "Floppy ROM Einstellungen..."},
/* fr */ {IDMS_DRIVE_ROM_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDMS_DRIVE_ROM_SETTINGS_HU, "Lemezegység ROM beállításai..."},
/* it */ {IDMS_DRIVE_ROM_SETTINGS_IT, "Impostazioni ROM del drive..."},
/* nl */ {IDMS_DRIVE_ROM_SETTINGS_NL, "Drive-ROM-instellingen..."},
/* pl */ {IDMS_DRIVE_ROM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_ROM_SETTINGS_SV, "ROM-inställningar för diskettenhet..."},

/* en */ {IDMS_RAM_SETTINGS,    "RAM settings..."},
/* de */ {IDMS_RAM_SETTINGS_DE, "RAM Einstellungen..."},
/* fr */ {IDMS_RAM_SETTINGS_FR, "Paramètres de la RAM..."},
/* hu */ {IDMS_RAM_SETTINGS_HU, "RAM beállításai..."},
/* it */ {IDMS_RAM_SETTINGS_IT, "Impostazioni RAM..."},
/* nl */ {IDMS_RAM_SETTINGS_NL, "RAM-instellingen..."},
/* pl */ {IDMS_RAM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RAM_SETTINGS_SV, "RAM-inställningar..."},

/* en */ {IDMS_RS232_SETTINGS,    "RS232 settings..."},
/* de */ {IDMS_RS232_SETTINGS_DE, "RS232 Einstellungen..."},
/* fr */ {IDMS_RS232_SETTINGS_FR, "Paramètres RS232..."},
/* hu */ {IDMS_RS232_SETTINGS_HU, "RS232 beállításai..."},
/* it */ {IDMS_RS232_SETTINGS_IT, "Impostazioni RS232..."},
/* nl */ {IDMS_RS232_SETTINGS_NL, "RS232-instellingen..."},
/* pl */ {IDMS_RS232_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RS232_SETTINGS_SV, "RS232-inställningar..."},

/* en */ {IDMS_C128_SETTINGS,    "C128 settings..."},
/* de */ {IDMS_C128_SETTINGS_DE, "C128 Einstellungen..."},
/* fr */ {IDMS_C128_SETTINGS_FR, "Paramètres C128..."},
/* hu */ {IDMS_C128_SETTINGS_HU, "C128 beállításai..."},
/* it */ {IDMS_C128_SETTINGS_IT, "Impostazioni C128..."},
/* nl */ {IDMS_C128_SETTINGS_NL, "C128-instellingen..."},
/* pl */ {IDMS_C128_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C128_SETTINGS_SV, "C128-inställningar..."},

/* en */ {IDMS_CART_IO_SETTINGS,    "Cartridge/IO settings"},
/* de */ {IDMS_CART_IO_SETTINGS_DE, "Erweiterungsmodul Einstellungen"},
/* fr */ {IDMS_CART_IO_SETTINGS_FR, "Paramètres E/S cartouche"},
/* hu */ {IDMS_CART_IO_SETTINGS_HU, "Cartridge/IO beállításai"},
/* it */ {IDMS_CART_IO_SETTINGS_IT, "Impostazioni I/O della cartuccia"},
/* nl */ {IDMS_CART_IO_SETTINGS_NL, "Cartridge/IO-instellingen"},
/* pl */ {IDMS_CART_IO_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CART_IO_SETTINGS_SV, "Inställningar för insticksmoduler..."},

/* en */ {IDMS_REU_SETTINGS,    "REU settings..."},
/* de */ {IDMS_REU_SETTINGS_DE, "REU Einstellungen..."},
/* fr */ {IDMS_REU_SETTINGS_FR, "Paramètres REU..."},
/* hu */ {IDMS_REU_SETTINGS_HU, "REU beállításai..."},
/* it */ {IDMS_REU_SETTINGS_IT, "Impostazioni REU..."},
/* nl */ {IDMS_REU_SETTINGS_NL, "REU-instellingen..."},
/* pl */ {IDMS_REU_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_REU_SETTINGS_SV, "REU-inställningar..."},

/* en */ {IDMS_GEORAM_SETTINGS,    "GEORAM settings..."},
/* de */ {IDMS_GEORAM_SETTINGS_DE, "GEORAM Einstellungen..."},
/* fr */ {IDMS_GEORAM_SETTINGS_FR, "Paramètres GEORAM..."},
/* hu */ {IDMS_GEORAM_SETTINGS_HU, "GEORAM beállításai..."},
/* it */ {IDMS_GEORAM_SETTINGS_IT, "Impostazioni GEORAM..."},
/* nl */ {IDMS_GEORAM_SETTINGS_NL, "GEORAM-instellingen..."},
/* pl */ {IDMS_GEORAM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_GEORAM_SETTINGS_SV, "GEORAM-inställningar..."},

/* en */ {IDMS_RAMCART_SETTINGS,    "RAMCART settings..."},
/* de */ {IDMS_RAMCART_SETTINGS_DE, "RAMCART Einstellungen..."},
/* fr */ {IDMS_RAMCART_SETTINGS_FR, "Paramètres RAMCART..."},
/* hu */ {IDMS_RAMCART_SETTINGS_HU, "RAMCART beállításai..."},
/* it */ {IDMS_RAMCART_SETTINGS_IT, "Impostazioni RAMCART..."},
/* nl */ {IDMS_RAMCART_SETTINGS_NL, "RAMCART-instellingen..."},
/* pl */ {IDMS_RAMCART_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RAMCART_SETTINGS_SV, "RAMCART-inställningar..."},

/* en */ {IDMS_PLUS60K_SETTINGS,    "PLUS60K settings..."},
/* de */ {IDMS_PLUS60K_SETTINGS_DE, "PLUS60K Einstellungen..."},
/* fr */ {IDMS_PLUS60K_SETTINGS_FR, "Paramètres PLUS60K..."},
/* hu */ {IDMS_PLUS60K_SETTINGS_HU, "PLUS60K beállításai..."},
/* it */ {IDMS_PLUS60K_SETTINGS_IT, "Impostazioni PLUS60K..."},
/* nl */ {IDMS_PLUS60K_SETTINGS_NL, "PLUS60K-instellingen..."},
/* pl */ {IDMS_PLUS60K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PLUS60K_SETTINGS_SV, "PLUS60K-inställningar..."},

/* en */ {IDS_PLUS256K_FILENAME,    "PLUS256K file"},
/* de */ {IDS_PLUS256K_FILENAME_DE, "PLUS256K Datei"},
/* fr */ {IDS_PLUS256K_FILENAME_FR, "Fichier PLUS256K"},
/* hu */ {IDS_PLUS256K_FILENAME_HU, "PLUS256K fájl"},
/* it */ {IDS_PLUS256K_FILENAME_IT, "File PLUS256K"},
/* nl */ {IDS_PLUS256K_FILENAME_NL, "PLUS256K-bestand"},
/* pl */ {IDS_PLUS256K_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS256K_FILENAME_SV, "PLUS256K-fil"},

/* en */ {IDS_PLUS256K_FILENAME_SELECT,    "Select file for PLUS256K"},
/* de */ {IDS_PLUS256K_FILENAME_SELECT_DE, "Datei für PLUS256K selektieren"},
/* fr */ {IDS_PLUS256K_FILENAME_SELECT_FR, "Sélectionner fichier pour PLUS256K"},
/* hu */ {IDS_PLUS256K_FILENAME_SELECT_HU, "Válasszon fájlt a PLUS256K-hoz"},
/* it */ {IDS_PLUS256K_FILENAME_SELECT_IT, "Seleziona file per PLUS256K"},
/* nl */ {IDS_PLUS256K_FILENAME_SELECT_NL, "Selecteer bestand voor PLUS256K"},
/* pl */ {IDS_PLUS256K_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS256K_FILENAME_SELECT_SV, "Ange fil för PLUS256K"},

/* en */ {IDMS_PLUS256K_SETTINGS,    "PLUS256K settings..."},
/* de */ {IDMS_PLUS256K_SETTINGS_DE, "PLUS256K Einstellungen..."},
/* fr */ {IDMS_PLUS256K_SETTINGS_FR, "Paramètres PLUS256K..."},
/* hu */ {IDMS_PLUS256K_SETTINGS_HU, "PLUS256K beállításai..."},
/* it */ {IDMS_PLUS256K_SETTINGS_IT, "Impostazioni PLUS256K..."},
/* nl */ {IDMS_PLUS256K_SETTINGS_NL, "PLUS256K-instellingen..."},
/* pl */ {IDMS_PLUS256K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PLUS256K_SETTINGS_SV, "PLUS256K-inställningar..."},

/* en */ {IDMS_256K_SETTINGS,    "256K settings..."},
/* de */ {IDMS_256K_SETTINGS_DE, "256K Einstellungen..."},
/* fr */ {IDMS_256K_SETTINGS_FR, "Paramètres 256K..."},
/* hu */ {IDMS_256K_SETTINGS_HU, "256K beállításai..."},
/* it */ {IDMS_256K_SETTINGS_IT, "Impostazioni 256K..."},
/* nl */ {IDMS_256K_SETTINGS_NL, "256K-instellingen..."},
/* pl */ {IDMS_256K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_256K_SETTINGS_SV, "256K-inställningar..."},

/* en */ {IDMS_IDE64_SETTINGS,    "IDE64 settings..."},
/* de */ {IDMS_IDE64_SETTINGS_DE, "IDE64 Einstellungen..."},
/* fr */ {IDMS_IDE64_SETTINGS_FR, "Paramètres IDE64..."},
/* hu */ {IDMS_IDE64_SETTINGS_HU, "IDE64 beállításai..."},
/* it */ {IDMS_IDE64_SETTINGS_IT, "Impostazioni IDE64..."},
/* nl */ {IDMS_IDE64_SETTINGS_NL, "IDE64-instellingen..."},
/* pl */ {IDMS_IDE64_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_IDE64_SETTINGS_SV, "IDE64-inställningar..."},

/* en */ {IDMS_MMC64_SETTINGS,    "MMC64 settings..."},
/* de */ {IDMS_MMC64_SETTINGS_DE, "MMC64 Einstellungen..."},
/* fr */ {IDMS_MMC64_SETTINGS_FR, "Paramètres MMC64..."},
/* hu */ {IDMS_MMC64_SETTINGS_HU, "MMC64 beállításai..."},
/* it */ {IDMS_MMC64_SETTINGS_IT, "Impostazioni MMC64..."},
/* nl */ {IDMS_MMC64_SETTINGS_NL, "MMC64-instellingen..."},
/* pl */ {IDMS_MMC64_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_MMC64_SETTINGS_SV, "MMC64-inställningar..."},

/* en */ {IDMS_DIGIMAX_SETTINGS,    "Digimax settings..."},
/* de */ {IDMS_DIGIMAX_SETTINGS_DE, "Digimax Einstellungen..."},
/* fr */ {IDMS_DIGIMAX_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDMS_DIGIMAX_SETTINGS_HU, "Digimax beállításai..."},
/* it */ {IDMS_DIGIMAX_SETTINGS_IT, "Impostazioni digimax..."},
/* nl */ {IDMS_DIGIMAX_SETTINGS_NL, "Digimaxinstellingen..."},
/* pl */ {IDMS_DIGIMAX_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DIGIMAX_SETTINGS_SV, "Digimaxinställningar..."},

/* en */ {IDMS_ETHERNET_SETTINGS,    "Ethernet settings..."},
/* de */ {IDMS_ETHERNET_SETTINGS_DE, "Ethernet Einstellungen..."},
/* fr */ {IDMS_ETHERNET_SETTINGS_FR, "Émulation Ethernet..."},
/* hu */ {IDMS_ETHERNET_SETTINGS_HU, "Ethernet beállításai..."},
/* it */ {IDMS_ETHERNET_SETTINGS_IT, "Impostazioni Ethernet..."},
/* nl */ {IDMS_ETHERNET_SETTINGS_NL, "Ethernetinstellingen..."},
/* pl */ {IDMS_ETHERNET_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ETHERNET_SETTINGS_SV, "Ethernetinställningar..."},

/* en */ {IDMS_ACIA_SETTINGS,    "ACIA settings..."},
/* de */ {IDMS_ACIA_SETTINGS_DE, "ACIA Einstellungen..."},
/* fr */ {IDMS_ACIA_SETTINGS_FR, "Paramètres ACIA..."},
/* hu */ {IDMS_ACIA_SETTINGS_HU, "ACIA beállításai..."},
/* it */ {IDMS_ACIA_SETTINGS_IT, "Impostazioni ACIA..."},
/* nl */ {IDMS_ACIA_SETTINGS_NL, "ACIA-instellingen..."},
/* pl */ {IDMS_ACIA_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ACIA_SETTINGS_SV, "ACIA-inställningar..."},

/* en */ {IDMS_PETREU_SETTINGS,    "PET REU settings..."},
/* de */ {IDMS_PETREU_SETTINGS_DE, "PET REU Einstellungen..."},
/* fr */ {IDMS_PETREU_SETTINGS_FR, "Paramètres PET REU..."},
/* hu */ {IDMS_PETREU_SETTINGS_HU, "PET REU beállításai..."},
/* it */ {IDMS_PETREU_SETTINGS_IT, "Impostazioni PET REU..."},
/* nl */ {IDMS_PETREU_SETTINGS_NL, "PET REU-instellingen..."},
/* pl */ {IDMS_PETREU_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PETREU_SETTINGS_SV, "PET REU-inställningar..."},

/* en */ {IDMS_SIDCART_SETTINGS,    "SID cart settings..."},
/* de */ {IDMS_SIDCART_SETTINGS_DE, "SID Modul Einstellungen..."},
/* fr */ {IDMS_SIDCART_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDMS_SIDCART_SETTINGS_HU, "SID kártya beállításai..."},
/* it */ {IDMS_SIDCART_SETTINGS_IT, "Impostazioni cartuccia SID..."},
/* nl */ {IDMS_SIDCART_SETTINGS_NL, "SID-cartinstellingen..."},
/* pl */ {IDMS_SIDCART_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SIDCART_SETTINGS_SV, "SID-modulinställningar..."},

/* en */ {IDMS_RS232_USERPORT_SETTINGS,    "RS232 userport settings..."},
/* de */ {IDMS_RS232_USERPORT_SETTINGS_DE, "RS232 Userport Einstellungen..."},
/* fr */ {IDMS_RS232_USERPORT_SETTINGS_FR, "Paramètres RS232 userport..."},
/* hu */ {IDMS_RS232_USERPORT_SETTINGS_HU, "RS232 userport beállításai..."},
/* it */ {IDMS_RS232_USERPORT_SETTINGS_IT, "Impostazioni RS232 su userport..."},
/* nl */ {IDMS_RS232_USERPORT_SETTINGS_NL, "RS232-userportinstellingen..."},
/* pl */ {IDMS_RS232_USERPORT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RS232_USERPORT_SETTINGS_SV, "RS232-användarportinställningar..."},

/* en */ {IDMS_SAVE_CURRENT_SETTINGS,    "Save current settings"},
/* de */ {IDMS_SAVE_CURRENT_SETTINGS_DE, "Einstellungen speichern"},
/* fr */ {IDMS_SAVE_CURRENT_SETTINGS_FR, "Enregistrer les paramètres courants"},
/* hu */ {IDMS_SAVE_CURRENT_SETTINGS_HU, "Jelenlegi beállítások mentése"},
/* it */ {IDMS_SAVE_CURRENT_SETTINGS_IT, "Salva le impostazioni attuali"},
/* nl */ {IDMS_SAVE_CURRENT_SETTINGS_NL, "Huidige instellingen opslaan"},
/* pl */ {IDMS_SAVE_CURRENT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_CURRENT_SETTINGS_SV, "Spara nuvarande inställningar"},

/* en */ {IDMS_LOAD_SAVED_SETTINGS,    "Load saved settings"},
/* de */ {IDMS_LOAD_SAVED_SETTINGS_DE, "Einstellungen Laden"},
/* fr */ {IDMS_LOAD_SAVED_SETTINGS_FR, "Charger les paramètres"},
/* hu */ {IDMS_LOAD_SAVED_SETTINGS_HU, "Beállítások betöltése"},
/* it */ {IDMS_LOAD_SAVED_SETTINGS_IT, "Carica le impostazioni salvate"},
/* nl */ {IDMS_LOAD_SAVED_SETTINGS_NL, "Opgeslagen instelling laden"},
/* pl */ {IDMS_LOAD_SAVED_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_SAVED_SETTINGS_SV, "Läs sparade inställningar"},

/* en */ {IDMS_SET_DEFAULT_SETTINGS,    "Set default settings"},
/* de */ {IDMS_SET_DEFAULT_SETTINGS_DE, "Wiederherstellen Standard Einstellungen"},
/* fr */ {IDMS_SET_DEFAULT_SETTINGS_FR, "Rétablir les paramètres par défaut"},
/* hu */ {IDMS_SET_DEFAULT_SETTINGS_HU, "Alapértelmezés beállítása"},
/* it */ {IDMS_SET_DEFAULT_SETTINGS_IT, "Ripristina le impostazioni originarie"},
/* nl */ {IDMS_SET_DEFAULT_SETTINGS_NL, "Herstel standaardinstellingen"},
/* pl */ {IDMS_SET_DEFAULT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SET_DEFAULT_SETTINGS_SV, "Återställ förvalda inställningar"},

/* en */ {IDMS_SAVE_SETTING_ON_EXIT,    "Save settings on exit"},
/* de */ {IDMS_SAVE_SETTING_ON_EXIT_DE, "Einstellungen beim Beenden speichern"},
/* fr */ {IDMS_SAVE_SETTING_ON_EXIT_FR, "Enregistrer les paramètres à la sortie"},
/* hu */ {IDMS_SAVE_SETTING_ON_EXIT_HU, "Beállítások mentése kilépéskor"},
/* it */ {IDMS_SAVE_SETTING_ON_EXIT_IT, "Salva le impostazioni in uscita"},
/* nl */ {IDMS_SAVE_SETTING_ON_EXIT_NL, "Sla instellingen op bij afsluiten"},
/* pl */ {IDMS_SAVE_SETTING_ON_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_SETTING_ON_EXIT_SV, "Spara inställningar vid avslut"},

/* en */ {IDMS_CONFIRM_ON_EXIT,    "Confirm on exit"},
/* de */ {IDMS_CONFIRM_ON_EXIT_DE, "Bestätigung beim Beenden"},
/* fr */ {IDMS_CONFIRM_ON_EXIT_FR, "Confirmation à la sortie"},
/* hu */ {IDMS_CONFIRM_ON_EXIT_HU, "Megerõsítés kilépéskor"},
/* it */ {IDMS_CONFIRM_ON_EXIT_IT, "Conferma all'uscita"},
/* nl */ {IDMS_CONFIRM_ON_EXIT_NL, "Bevestigen bij afsluiten"},
/* pl */ {IDMS_CONFIRM_ON_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CONFIRM_ON_EXIT_SV, "Bekräfta avslutning"},

/* en */ {IDMS_HELP,    "Help"},
/* de */ {IDMS_HELP_DE, "Hilfe"},
/* fr */ {IDMS_HELP_FR, "Aide"},
/* hu */ {IDMS_HELP_HU, "Segítség"},
/* it */ {IDMS_HELP_IT, "Aiuto"},
/* nl */ {IDMS_HELP_NL, "Help"},
/* pl */ {IDMS_HELP_PL, ""},  /* fuzzy */
/* sv */ {IDMS_HELP_SV, "Hjälp"},

/* en */ {IDMS_ABOUT,    "About..."},
/* de */ {IDMS_ABOUT_DE, "Über VICE..."},
/* fr */ {IDMS_ABOUT_FR, "À Propos..."},
/* hu */ {IDMS_ABOUT_HU, "Névjegy..."},
/* it */ {IDMS_ABOUT_IT, "Informazioni..."},
/* nl */ {IDMS_ABOUT_NL, "Over VICE..."},
/* pl */ {IDMS_ABOUT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ABOUT_SV, "Om VICE..."},

/* en */ {IDMS_COMMAND_LINE_OPTIONS,    "Command line options"},
/* de */ {IDMS_COMMAND_LINE_OPTIONS_DE, "Kommandozeilen Optionen"},
/* fr */ {IDMS_COMMAND_LINE_OPTIONS_FR, "Options de ligne de commande"},
/* hu */ {IDMS_COMMAND_LINE_OPTIONS_HU, "Parancssori opciók"},
/* it */ {IDMS_COMMAND_LINE_OPTIONS_IT, "Opzioni da riga di comando"},
/* nl */ {IDMS_COMMAND_LINE_OPTIONS_NL, "Commando invoer opties"},
/* pl */ {IDMS_COMMAND_LINE_OPTIONS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_COMMAND_LINE_OPTIONS_SV, "Kommandoradsflaggor"},

/* en */ {IDMS_CONTRIBUTORS,    "Contributors"},
/* de */ {IDMS_CONTRIBUTORS_DE, "VICE Hackers"},
/* fr */ {IDMS_CONTRIBUTORS_FR, "Contributeurs"},
/* hu */ {IDMS_CONTRIBUTORS_HU, "Közremûködõk"},
/* it */ {IDMS_CONTRIBUTORS_IT, "Collaboratori"},
/* nl */ {IDMS_CONTRIBUTORS_NL, "Medewerkers aan het VICE project"},
/* pl */ {IDMS_CONTRIBUTORS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CONTRIBUTORS_SV, "Bidragslämnare"},

/* en */ {IDMS_LICENSE,    "License"},
/* de */ {IDMS_LICENSE_DE, "Lizenz"},
/* fr */ {IDMS_LICENSE_FR, "License"},
/* hu */ {IDMS_LICENSE_HU, "Licensz"},
/* it */ {IDMS_LICENSE_IT, "Licenza"},
/* nl */ {IDMS_LICENSE_NL, "Licentie"},
/* pl */ {IDMS_LICENSE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LICENSE_SV, "Licens"},

/* en */ {IDMS_NO_WARRANTY,    "No warranty"},
/* de */ {IDMS_NO_WARRANTY_DE, "Keine Garantie!"},
/* fr */ {IDMS_NO_WARRANTY_FR, "Aucune garantie!"},
/* hu */ {IDMS_NO_WARRANTY_HU, "Nincs garancia!"},
/* it */ {IDMS_NO_WARRANTY_IT, "Nessuna garanzia"},
/* nl */ {IDMS_NO_WARRANTY_NL, "Geen garantie"},
/* pl */ {IDMS_NO_WARRANTY_PL, ""},  /* fuzzy */
/* sv */ {IDMS_NO_WARRANTY_SV, "Ingen garanti"},

/* en */ {IDMS_LANGUAGE_INTERNATIONAL,    "International"},
/* de */ {IDMS_LANGUAGE_INTERNATIONAL_DE, "International"},
/* fr */ {IDMS_LANGUAGE_INTERNATIONAL_FR, ""},  /* fuzzy */
/* hu */ {IDMS_LANGUAGE_INTERNATIONAL_HU, "Nemzetközi"},
/* it */ {IDMS_LANGUAGE_INTERNATIONAL_IT, "Internazionale"},
/* nl */ {IDMS_LANGUAGE_INTERNATIONAL_NL, "Internationaal"},
/* pl */ {IDMS_LANGUAGE_INTERNATIONAL_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_INTERNATIONAL_SV, "Internationell"},

/* en */ {IDMS_LANGUAGE_ENGLISH,    "English"},
/* de */ {IDMS_LANGUAGE_ENGLISH_DE, "Englisch"},
/* fr */ {IDMS_LANGUAGE_ENGLISH_FR, "Anglais"},
/* hu */ {IDMS_LANGUAGE_ENGLISH_HU, "Angol"},
/* it */ {IDMS_LANGUAGE_ENGLISH_IT, "Inglese"},
/* nl */ {IDMS_LANGUAGE_ENGLISH_NL, "Engels"},
/* pl */ {IDMS_LANGUAGE_ENGLISH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_ENGLISH_SV, "Engelska"},

/* en */ {IDMS_LANGUAGE_GERMAN,    "German"},
/* de */ {IDMS_LANGUAGE_GERMAN_DE, "Deutsch"},
/* fr */ {IDMS_LANGUAGE_GERMAN_FR, "Allemand"},
/* hu */ {IDMS_LANGUAGE_GERMAN_HU, "Német"},
/* it */ {IDMS_LANGUAGE_GERMAN_IT, "Tedesco"},
/* nl */ {IDMS_LANGUAGE_GERMAN_NL, "Duits"},
/* pl */ {IDMS_LANGUAGE_GERMAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_GERMAN_SV, "Tyska"},

/* en */ {IDMS_LANGUAGE_FINNISH,    "Finnish"},
/* de */ {IDMS_LANGUAGE_FINNISH_DE, "Finnisch"},
/* fr */ {IDMS_LANGUAGE_FINNISH_FR, ""},  /* fuzzy */
/* hu */ {IDMS_LANGUAGE_FINNISH_HU, "Finn"},
/* it */ {IDMS_LANGUAGE_FINNISH_IT, "Finlandese"},
/* nl */ {IDMS_LANGUAGE_FINNISH_NL, "Fins"},
/* pl */ {IDMS_LANGUAGE_FINNISH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_FINNISH_SV, "Finska"},

/* en */ {IDMS_LANGUAGE_FRENCH,    "French"},
/* de */ {IDMS_LANGUAGE_FRENCH_DE, "Französisch"},
/* fr */ {IDMS_LANGUAGE_FRENCH_FR, "Français"},
/* hu */ {IDMS_LANGUAGE_FRENCH_HU, "Francia"},
/* it */ {IDMS_LANGUAGE_FRENCH_IT, "Francese"},
/* nl */ {IDMS_LANGUAGE_FRENCH_NL, "Frans"},
/* pl */ {IDMS_LANGUAGE_FRENCH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_FRENCH_SV, "Franska"},

/* en */ {IDMS_LANGUAGE_HUNGARIAN,    "Hungarian"},
/* de */ {IDMS_LANGUAGE_HUNGARIAN_DE, "Ungarisch"},
/* fr */ {IDMS_LANGUAGE_HUNGARIAN_FR, ""},  /* fuzzy */
/* hu */ {IDMS_LANGUAGE_HUNGARIAN_HU, "Magyar"},
/* it */ {IDMS_LANGUAGE_HUNGARIAN_IT, "Ungherese"},
/* nl */ {IDMS_LANGUAGE_HUNGARIAN_NL, "Hongaars"},
/* pl */ {IDMS_LANGUAGE_HUNGARIAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_HUNGARIAN_SV, "Ungerska"},

/* en */ {IDMS_LANGUAGE_ITALIAN,    "Italian"},
/* de */ {IDMS_LANGUAGE_ITALIAN_DE, "Italienisch"},
/* fr */ {IDMS_LANGUAGE_ITALIAN_FR, "Italien"},
/* hu */ {IDMS_LANGUAGE_ITALIAN_HU, "Olasz"},
/* it */ {IDMS_LANGUAGE_ITALIAN_IT, "Italiano"},
/* nl */ {IDMS_LANGUAGE_ITALIAN_NL, "Italiaans"},
/* pl */ {IDMS_LANGUAGE_ITALIAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_ITALIAN_SV, "Italienska"},

/* en */ {IDMS_LANGUAGE_DUTCH,    "Dutch"},
/* de */ {IDMS_LANGUAGE_DUTCH_DE, "Holländisch"},
/* fr */ {IDMS_LANGUAGE_DUTCH_FR, "Hollandais"},
/* hu */ {IDMS_LANGUAGE_DUTCH_HU, "Holland"},
/* it */ {IDMS_LANGUAGE_DUTCH_IT, "Olandese"},
/* nl */ {IDMS_LANGUAGE_DUTCH_NL, "Nederlands"},
/* pl */ {IDMS_LANGUAGE_DUTCH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_DUTCH_SV, "Nederländska"},

/* en */ {IDMS_LANGUAGE_NORWEGIAN,    "Norwegian"},
/* de */ {IDMS_LANGUAGE_NORWEGIAN_DE, "Norwegisch"},
/* fr */ {IDMS_LANGUAGE_NORWEGIAN_FR, ""},  /* fuzzy */
/* hu */ {IDMS_LANGUAGE_NORWEGIAN_HU, "Norvég"},
/* it */ {IDMS_LANGUAGE_NORWEGIAN_IT, "Norvegese"},
/* nl */ {IDMS_LANGUAGE_NORWEGIAN_NL, "Noors"},
/* pl */ {IDMS_LANGUAGE_NORWEGIAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_NORWEGIAN_SV, "Norska"},

/* en */ {IDMS_LANGUAGE_POLISH,    "Polish"},
/* de */ {IDMS_LANGUAGE_POLISH_DE, "Polnisch"},
/* fr */ {IDMS_LANGUAGE_POLISH_FR, "Polonais"},
/* hu */ {IDMS_LANGUAGE_POLISH_HU, "Lengyel"},
/* it */ {IDMS_LANGUAGE_POLISH_IT, "Polacco"},
/* nl */ {IDMS_LANGUAGE_POLISH_NL, "Pools"},
/* pl */ {IDMS_LANGUAGE_POLISH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_POLISH_SV, "Polska"},

/* en */ {IDMS_LANGUAGE_SWEDISH,    "Swedish"},
/* de */ {IDMS_LANGUAGE_SWEDISH_DE, "Schwedisch"},
/* fr */ {IDMS_LANGUAGE_SWEDISH_FR, "Suédois"},
/* hu */ {IDMS_LANGUAGE_SWEDISH_HU, "Svéd"},
/* it */ {IDMS_LANGUAGE_SWEDISH_IT, "Svedese"},
/* nl */ {IDMS_LANGUAGE_SWEDISH_NL, "Zweeds"},
/* pl */ {IDMS_LANGUAGE_SWEDISH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_SWEDISH_SV, "Svenska"},

/* en */ {IDMS_LANGUAGE,    "Language"},
/* de */ {IDMS_LANGUAGE_DE, "Sprache"},
/* fr */ {IDMS_LANGUAGE_FR, "Langage"},
/* hu */ {IDMS_LANGUAGE_HU, "Nyelv"},
/* it */ {IDMS_LANGUAGE_IT, "Lingua"},
/* nl */ {IDMS_LANGUAGE_NL, "Taal"},
/* pl */ {IDMS_LANGUAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_SV, "Språk"},

/* en */ {IDMS_FULLSCREEN_STATUSBAR,    "Fullscreen Statusbar"},
/* de */ {IDMS_FULLSCREEN_STATUSBAR_DE, "Vollbild Statusleiste"},
/* fr */ {IDMS_FULLSCREEN_STATUSBAR_FR, ""},  /* fuzzy */
/* hu */ {IDMS_FULLSCREEN_STATUSBAR_HU, "Állapotsor teljes képernyõnél"},
/* it */ {IDMS_FULLSCREEN_STATUSBAR_IT, "Statusbar nella modalità a tutto schermo"},
/* nl */ {IDMS_FULLSCREEN_STATUSBAR_NL, "Volscherm Statusbalk"},
/* pl */ {IDMS_FULLSCREEN_STATUSBAR_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FULLSCREEN_STATUSBAR_SV, "Status rad vid fullskärm"},

/* en */ {IDMS_VIDEO_OVERLAY,    "Video overlay"},
/* de */ {IDMS_VIDEO_OVERLAY_DE, "Video Overlay"},
/* fr */ {IDMS_VIDEO_OVERLAY_FR, ""},  /* fuzzy */
/* hu */ {IDMS_VIDEO_OVERLAY_HU, "Videó overlay"},
/* it */ {IDMS_VIDEO_OVERLAY_IT, "Overlay video"},
/* nl */ {IDMS_VIDEO_OVERLAY_NL, "Video overlay"},
/* pl */ {IDMS_VIDEO_OVERLAY_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIDEO_OVERLAY_SV, "Video overlay"},

/* en */ {IDMS_START_SOUND_RECORD,    "Start Sound Record..."},
/* de */ {IDMS_START_SOUND_RECORD_DE, "Tonaufnahme starten..."},
/* fr */ {IDMS_START_SOUND_RECORD_FR, ""},  /* fuzzy */
/* hu */ {IDMS_START_SOUND_RECORD_HU, "Hangfelvétel indítása..."},
/* it */ {IDMS_START_SOUND_RECORD_IT, "Avvia la registrazione audio..."},
/* nl */ {IDMS_START_SOUND_RECORD_NL, "Start Geluidsopname..."},
/* pl */ {IDMS_START_SOUND_RECORD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_SOUND_RECORD_SV, "Starta ljudinspelning..."},

/* en */ {IDMS_STOP_SOUND_RECORD,    "Stop Sound Record"},
/* de */ {IDMS_STOP_SOUND_RECORD_DE, "Tonaufnahme stoppen"},
/* fr */ {IDMS_STOP_SOUND_RECORD_FR, ""},  /* fuzzy */
/* hu */ {IDMS_STOP_SOUND_RECORD_HU, "Hangfelvétel leállítása..."},
/* it */ {IDMS_STOP_SOUND_RECORD_IT, "Interrompi la registrazione audio"},
/* nl */ {IDMS_STOP_SOUND_RECORD_NL, "Stop Geluidsopname"},
/* pl */ {IDMS_STOP_SOUND_RECORD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_STOP_SOUND_RECORD_SV, "Stoppa ljudinspelning"},


/* ----------------------- AmigaOS Message/Error Strings ----------------------- */

/* en */ {IDMES_SETTINGS_SAVED_SUCCESS,    "Settings saved successfully."},
/* de */ {IDMES_SETTINGS_SAVED_SUCCESS_DE, "Einstellungen erfolgreich gespeichert."},
/* fr */ {IDMES_SETTINGS_SAVED_SUCCESS_FR, "Sauvegarde des paramètres effectuée correctement."},
/* hu */ {IDMES_SETTINGS_SAVED_SUCCESS_HU, "Beállítások sikeresen elmentve."},
/* it */ {IDMES_SETTINGS_SAVED_SUCCESS_IT, "Impostazioni salvate con successo."},
/* nl */ {IDMES_SETTINGS_SAVED_SUCCESS_NL, "Instellingen met succes opgeslagen."},
/* pl */ {IDMES_SETTINGS_SAVED_SUCCESS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_SETTINGS_SAVED_SUCCESS_SV, "Inställningarna har sparats."},

/* en */ {IDMES_SETTINGS_LOAD_SUCCESS,    "Settings loaded successfully."},
/* de */ {IDMES_SETTINGS_LOAD_SUCCESS_DE, "Einstellungen erfolgreich geladen."},
/* fr */ {IDMES_SETTINGS_LOAD_SUCCESS_FR, "Chargement des paramètres réussi."},
/* hu */ {IDMES_SETTINGS_LOAD_SUCCESS_HU, "Beállítások sikeresen elmentve."},
/* it */ {IDMES_SETTINGS_LOAD_SUCCESS_IT, "Impostazioni caricate con successo."},
/* nl */ {IDMES_SETTINGS_LOAD_SUCCESS_NL, "Instellingen met succes geladen."},
/* pl */ {IDMES_SETTINGS_LOAD_SUCCESS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_SETTINGS_LOAD_SUCCESS_SV, "Inställningarna har lästs in."},

/* en */ {IDMES_DFLT_SETTINGS_RESTORED,    "Default settings restored."},
/* de */ {IDMES_DFLT_SETTINGS_RESTORED_DE, "Standard wiederhergestellt."},
/* fr */ {IDMES_DFLT_SETTINGS_RESTORED_FR, "Paramètres par défaut restaurés."},
/* hu */ {IDMES_DFLT_SETTINGS_RESTORED_HU, "Alapértelmezett beállítások visszaállítva."},
/* it */ {IDMES_DFLT_SETTINGS_RESTORED_IT, "Impostazioni predefinite ripristinate."},
/* nl */ {IDMES_DFLT_SETTINGS_RESTORED_NL, "Standaardinstellingen hersteld."},
/* pl */ {IDMES_DFLT_SETTINGS_RESTORED_PL, ""},  /* fuzzy */
/* sv */ {IDMES_DFLT_SETTINGS_RESTORED_SV, "Förvalda inställningar återställda."},

/* en */ {IDMES_VICE_MESSAGE,    "VICE Message"},
/* de */ {IDMES_VICE_MESSAGE_DE, "VICE Nachricht"},
/* fr */ {IDMES_VICE_MESSAGE_FR, "Message VICE"},
/* hu */ {IDMES_VICE_MESSAGE_HU, "VICE üzenet"},
/* it */ {IDMES_VICE_MESSAGE_IT, "Messaggio di VICE"},
/* nl */ {IDMES_VICE_MESSAGE_NL, "VICE-bericht"},
/* pl */ {IDMES_VICE_MESSAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_VICE_MESSAGE_SV, "VICE-meddelande"},

/* en */ {IDMES_OK,    "OK"},
/* de */ {IDMES_OK_DE, "OK"},
/* fr */ {IDMES_OK_FR, "OK"},
/* hu */ {IDMES_OK_HU, "OK"},
/* it */ {IDMES_OK_IT, "OK"},
/* nl */ {IDMES_OK_NL, "OK"},
/* pl */ {IDMES_OK_PL, "OK"},
/* sv */ {IDMES_OK_SV, "OK"},

/* en */ {IDMES_VICE_CONTRIBUTORS,    "VICE contributors"},
/* de */ {IDMES_VICE_CONTRIBUTORS_DE, "VICE Hackers"},
/* fr */ {IDMES_VICE_CONTRIBUTORS_FR, "VICE Contributeurs"},
/* hu */ {IDMES_VICE_CONTRIBUTORS_HU, "VICE - közremûködõk"},
/* it */ {IDMES_VICE_CONTRIBUTORS_IT, "Collaboratori al progetto VICE"},
/* nl */ {IDMES_VICE_CONTRIBUTORS_NL, "Medewerkers aan het VICE project"},
/* pl */ {IDMES_VICE_CONTRIBUTORS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_VICE_CONTRIBUTORS_SV, "VICE Bidragslämnare"},

/* en */ {IDMES_WHO_MADE_WHAT,    "Who made what?"},
/* de */ {IDMES_WHO_MADE_WHAT_DE, "Wer hat was gemacht?"},
/* fr */ {IDMES_WHO_MADE_WHAT_FR, "Qui fait quoi?"},
/* hu */ {IDMES_WHO_MADE_WHAT_HU, "Ki mit csinált?"},
/* it */ {IDMES_WHO_MADE_WHAT_IT, "Chi ha fatto cosa?"},
/* nl */ {IDMES_WHO_MADE_WHAT_NL, "Wie heeft wat gemaakt?"},
/* pl */ {IDMES_WHO_MADE_WHAT_PL, ""},  /* fuzzy */
/* sv */ {IDMES_WHO_MADE_WHAT_SV, "Vem gjorde vad?"},

/* en */ {IDMES_VICE_DIST_NO_WARRANTY,    "VICE is distributed WITHOUT ANY WARRANTY!"},
/* de */ {IDMES_VICE_DIST_NO_WARRANTY_DE, "VICE ist ohne Garantie"},
/* fr */ {IDMES_VICE_DIST_NO_WARRANTY_FR, "VICE est distribué SANS AUCUN GARANTIE!"},
/* hu */ {IDMES_VICE_DIST_NO_WARRANTY_HU, "A VICE-t garancia nélkül terjesztjük!"},
/* it */ {IDMES_VICE_DIST_NO_WARRANTY_IT, "VICE è distribuito SENZA NESSUNA GARANZIA!"},
/* nl */ {IDMES_VICE_DIST_NO_WARRANTY_NL, "VICE heeft ABSOLUUT GEEN GARANTIE!"},
/* pl */ {IDMES_VICE_DIST_NO_WARRANTY_PL, ""},  /* fuzzy */
/* sv */ {IDMES_VICE_DIST_NO_WARRANTY_SV, "VICE distribueras UTAN NÅGON SOM HELST GARANTI!"},

/* en */ {IDMES_WHICH_COMMANDS_AVAILABLE,    "Which command line options are available?"},
/* de */ {IDMES_WHICH_COMMANDS_AVAILABLE_DE, "Welche Kommandozeilen Parameter sind verfügbar?"},
/* fr */ {IDMES_WHICH_COMMANDS_AVAILABLE_FR, "Quelles sont les lignes de commandes disponibles?"},
/* hu */ {IDMES_WHICH_COMMANDS_AVAILABLE_HU, "Milyen parancssori opciók lehetségesek?"},
/* it */ {IDMES_WHICH_COMMANDS_AVAILABLE_IT, "Quali parametri da riga di comando sono disponibili?"},
/* nl */ {IDMES_WHICH_COMMANDS_AVAILABLE_NL, "Welke commando opties zijn beschikbaar?"},
/* pl */ {IDMES_WHICH_COMMANDS_AVAILABLE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_WHICH_COMMANDS_AVAILABLE_SV, "Vilka kommandoradsflaggor är tillgängliga?"},

/* en */ {IDMES_CANNOT_SAVE_SETTINGS,    "Cannot save settings."},
/* de */ {IDMES_CANNOT_SAVE_SETTINGS_DE, "Fehler beim Speichern der Einstellungen."},
/* fr */ {IDMES_CANNOT_SAVE_SETTINGS_FR, "Impossible d'enregistrer les paramètres."},
/* hu */ {IDMES_CANNOT_SAVE_SETTINGS_HU, "A beállítások mentése nem sikerült."},
/* it */ {IDMES_CANNOT_SAVE_SETTINGS_IT, "Impossibile salvare le impostazioni."},
/* nl */ {IDMES_CANNOT_SAVE_SETTINGS_NL, "Kan de instellingen niet opslaan."},
/* pl */ {IDMES_CANNOT_SAVE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_SAVE_SETTINGS_SV, "Kan inte spara inställningar."},

/* en */ {IDMES_CANNOT_LOAD_SETTINGS,    "Cannot load settings."},
/* de */ {IDMES_CANNOT_LOAD_SETTINGS_DE, "Fehler beim Laden der Einstellungen."},
/* fr */ {IDMES_CANNOT_LOAD_SETTINGS_FR, "Impossible de charger les paramètres."},
/* hu */ {IDMES_CANNOT_LOAD_SETTINGS_HU, "A beállítások betöltése nem sikerült."},
/* it */ {IDMES_CANNOT_LOAD_SETTINGS_IT, "Non è possibile caricare le impostazioni."},
/* nl */ {IDMES_CANNOT_LOAD_SETTINGS_NL, "Kan de instellingen niet laden."},
/* pl */ {IDMES_CANNOT_LOAD_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_LOAD_SETTINGS_SV, "Kan inte läsa inställningar."},

/* en */ {IDMES_VICE_ERROR,    "VICE Error"},
/* de */ {IDMES_VICE_ERROR_DE, "VICE Fehler"},
/* fr */ {IDMES_VICE_ERROR_FR, "Erreur VICE"},
/* hu */ {IDMES_VICE_ERROR_HU, "VICE hiba"},
/* it */ {IDMES_VICE_ERROR_IT, "Errore di VICE"},
/* nl */ {IDMES_VICE_ERROR_NL, "Vice-fout"},
/* pl */ {IDMES_VICE_ERROR_PL, ""},  /* fuzzy */
/* sv */ {IDMES_VICE_ERROR_SV, "VICE-fel"},

/* en */ {IDMES_NO_JOY_ON_PORT_D,    "No joystick or joypad found on port %d"},
/* de */ {IDMES_NO_JOY_ON_PORT_D_DE, "Kein Joystick oder Joypad am Port %d gefunden"},
/* fr */ {IDMES_NO_JOY_ON_PORT_D_FR, ""},  /* fuzzy */
/* hu */ {IDMES_NO_JOY_ON_PORT_D_HU, "Nem találtam botkormányt a %d porton"},
/* it */ {IDMES_NO_JOY_ON_PORT_D_IT, "Non è stato trovato nessun joystick o joypad sulla porta %d"},
/* nl */ {IDMES_NO_JOY_ON_PORT_D_NL, "Geen joystick of joypad gevonden op poort %d"},
/* pl */ {IDMES_NO_JOY_ON_PORT_D_PL, ""},  /* fuzzy */
/* sv */ {IDMES_NO_JOY_ON_PORT_D_SV, "Ingen joystick eller joypad hittades på port %d"},

/* en */ {IDMES_MOUSE_ON_PORT_D,    "A mouse was found on port %d"},
/* de */ {IDMES_MOUSE_ON_PORT_D_DE, "Eine Maus wurde in Port %d gefunden"},
/* fr */ {IDMES_MOUSE_ON_PORT_D_FR, ""},  /* fuzzy */
/* hu */ {IDMES_MOUSE_ON_PORT_D_HU, "Nem találtam egeret a %d porton"},
/* it */ {IDMES_MOUSE_ON_PORT_D_IT, "E' stato trovato un mouse sulla porta %d"},
/* nl */ {IDMES_MOUSE_ON_PORT_D_NL, "Een muis was gevonden op poort %d"},
/* pl */ {IDMES_MOUSE_ON_PORT_D_PL, ""},  /* fuzzy */
/* sv */ {IDMES_MOUSE_ON_PORT_D_SV, "En mus hittades på port %d"},

/* en */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D,    "Unknown device found on port %d"},
/* de */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_DE, "Unbekanntes Gerät im Port %d gefunden"},
/* fr */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_FR, ""},  /* fuzzy */
/* hu */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_HU, "%d porton ismeretlen eszközt találtam"},
/* it */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_IT, "Dispositivo sconosciuto trovato sulla porta %d"},
/* nl */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_NL, "Onbekend apparaat gevonden op poort %d"},
/* pl */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_PL, ""},  /* fuzzy */
/* sv */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_SV, "En okänd enhet hittades på port %d"},

/* en */ {IDMES_DEVICE_NOT_GAMEPAD,    "The device on this port is not a gamepad"},
/* de */ {IDMES_DEVICE_NOT_GAMEPAD_DE, "Das Gerät in diesem Port ist kein Gamepad"},
/* fr */ {IDMES_DEVICE_NOT_GAMEPAD_FR, ""},  /* fuzzy */
/* hu */ {IDMES_DEVICE_NOT_GAMEPAD_HU, "Az eszköz ezen a porton nem gamepad"},
/* it */ {IDMES_DEVICE_NOT_GAMEPAD_IT, "Il dispositivo su questa porta non è un gamepad"},
/* nl */ {IDMES_DEVICE_NOT_GAMEPAD_NL, "Het apparaat op deze poort is geen gamepad"},
/* pl */ {IDMES_DEVICE_NOT_GAMEPAD_PL, ""},  /* fuzzy */
/* sv */ {IDMES_DEVICE_NOT_GAMEPAD_SV, "Enheten på denna port är ingen gamepad"},

/* en */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT,    "The port is not mapped to an Amiga port"},
/* de */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_DE, "Dieses Port ist keinem Amiga Port zugeordnet"},
/* fr */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_FR, ""},  /* fuzzy */
/* hu */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_HU, "A port nincs leképezve egy Amiga portra"},
/* it */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_IT, "Questa porta non è mappata ad una porta dell'Amiga"},
/* nl */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_NL, "De poort is niet verbonden met een Amigapoort"},
/* pl */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_PL, ""},  /* fuzzy */
/* sv */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_SV, "Porten är inte bunden till en Amiga port"},

/* en */ {IDMES_CANNOT_AUTOSTART_FILE,    "Cannot autostart specified file."},
/* de */ {IDMES_CANNOT_AUTOSTART_FILE_DE, "Autostart von angeforderter Datei fehlgeschlagen."},
/* fr */ {IDMES_CANNOT_AUTOSTART_FILE_FR, "Impossible de démarrer automatiquement le fichier spécifié"},
/* hu */ {IDMES_CANNOT_AUTOSTART_FILE_HU, "Nem lehet automatikusan elindítani a megadott fájlt."},
/* it */ {IDMES_CANNOT_AUTOSTART_FILE_IT, "Non è possibile avviare automaticamente il file specificato."},
/* nl */ {IDMES_CANNOT_AUTOSTART_FILE_NL, "Kan opgegeven bestand niet autostarten."},
/* pl */ {IDMES_CANNOT_AUTOSTART_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_AUTOSTART_FILE_SV, "Kan inte autostarta angiven fil."},

/* en */ {IDMES_CANNOT_ATTACH_FILE,    "Cannot attach specified file"},
/* de */ {IDMES_CANNOT_ATTACH_FILE_DE, "Kann Image Datei nicht einlegen"},
/* fr */ {IDMES_CANNOT_ATTACH_FILE_FR, "Impossible d'attacher le fichier spécifié"},
/* hu */ {IDMES_CANNOT_ATTACH_FILE_HU, "Nem lehet csatolni a megadott fájlt"},
/* it */ {IDMES_CANNOT_ATTACH_FILE_IT, "Non è possibile selezionare il file specificato"},
/* nl */ {IDMES_CANNOT_ATTACH_FILE_NL, "Kan het opgegeven bestand niet koppelen"},
/* pl */ {IDMES_CANNOT_ATTACH_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_ATTACH_FILE_SV, "Kan inte ansluta filen"},

/* en */ {IDMES_INVALID_CART,    "Invalid cartridge"},
/* de */ {IDMES_INVALID_CART_DE, "Ungültiges Erweiterungsmodul"},
/* fr */ {IDMES_INVALID_CART_FR, "Cartouche invalide"},
/* hu */ {IDMES_INVALID_CART_HU, "Érvénytelen cartridge"},
/* it */ {IDMES_INVALID_CART_IT, "Cartuccia non valida"},
/* nl */ {IDMES_INVALID_CART_NL, "Ongeldige cartridge"},
/* pl */ {IDMES_INVALID_CART_PL, ""},  /* fuzzy */
/* sv */ {IDMES_INVALID_CART_SV, "Ogiltig insticksmodul"},

/* en */ {IDMES_BAD_CART_CONFIG_IN_UI,    "Bad cartridge config in UI!"},
/* de */ {IDMES_BAD_CART_CONFIG_IN_UI_DE, "Fehlerhafte Modul Konfiguration im UI!"},
/* fr */ {IDMES_BAD_CART_CONFIG_IN_UI_FR, "Mauvaise configuration de la cartouche dans l'interface utilisateur!"},
/* hu */ {IDMES_BAD_CART_CONFIG_IN_UI_HU, "Rossz cartridge konfiguráció!"},
/* it */ {IDMES_BAD_CART_CONFIG_IN_UI_IT, "Configurazione della cartuccia errata nell'IU!"},
/* nl */ {IDMES_BAD_CART_CONFIG_IN_UI_NL, "Slechte cartridge configuratie in UI!"},
/* pl */ {IDMES_BAD_CART_CONFIG_IN_UI_PL, ""},  /* fuzzy */
/* sv */ {IDMES_BAD_CART_CONFIG_IN_UI_SV, "Felaktiga insticksmodulsinställningar i UI!"},

/* en */ {IDMES_INVALID_CART_IMAGE,    "Invalid cartridge image"},
/* de */ {IDMES_INVALID_CART_IMAGE_DE, "Ungültiges Erweiterungsmodul Image"},
/* fr */ {IDMES_INVALID_CART_IMAGE_FR, "Image de cartouche invalide"},
/* hu */ {IDMES_INVALID_CART_IMAGE_HU, "Érvénytelen cartridge képmás"},
/* it */ {IDMES_INVALID_CART_IMAGE_IT, "Immagine della cartuccia non valida"},
/* nl */ {IDMES_INVALID_CART_IMAGE_NL, "Ongeldig cartridgebestand"},
/* pl */ {IDMES_INVALID_CART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_INVALID_CART_IMAGE_SV, "Ogiltig insticksmodulfil"},

/* en */ {IDMES_CANNOT_CREATE_IMAGE,    "Cannot create image"},
/* de */ {IDMES_CANNOT_CREATE_IMAGE_DE, "Kann Image Datei nicht erzeugen"},
/* fr */ {IDMES_CANNOT_CREATE_IMAGE_FR, "Impossible de créer l'image"},
/* hu */ {IDMES_CANNOT_CREATE_IMAGE_HU, "Nem sikerült a képmást létrehozni"},
/* it */ {IDMES_CANNOT_CREATE_IMAGE_IT, "Non è possibile creare l'immagine"},
/* nl */ {IDMES_CANNOT_CREATE_IMAGE_NL, "Kan bestand niet maken"},
/* pl */ {IDMES_CANNOT_CREATE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_CREATE_IMAGE_SV, "Kan inte skapa avbildningsfil"},

/* en */ {IDMES_ERROR_STARTING_SERVER,    "An error occured starting the server."},
/* de */ {IDMES_ERROR_STARTING_SERVER_DE, "Fehler beim Starten des Netplay Servers."},
/* fr */ {IDMES_ERROR_STARTING_SERVER_FR, ""},  /* fuzzy */
/* hu */ {IDMES_ERROR_STARTING_SERVER_HU, "Hiba történt a játék kiszolgáló indításakor."},
/* it */ {IDMES_ERROR_STARTING_SERVER_IT, "Si è verificato un errore all'avvio del server."},
/* nl */ {IDMES_ERROR_STARTING_SERVER_NL, "Er is een fout opgetreden bij het opstarten van de server"},
/* pl */ {IDMES_ERROR_STARTING_SERVER_PL, ""},  /* fuzzy */
/* sv */ {IDMES_ERROR_STARTING_SERVER_SV, "Fel vid anslutning till server."},

/* en */ {IDMES_ERROR_CONNECTING_CLIENT,    "An error occured connecting the client."},
/* de */ {IDMES_ERROR_CONNECTING_CLIENT_DE, "Fehler beim Verbindungsaufbau zum Client"},
/* fr */ {IDMES_ERROR_CONNECTING_CLIENT_FR, ""},  /* fuzzy */
/* hu */ {IDMES_ERROR_CONNECTING_CLIENT_HU, "Hiba történt a kapcsolódáskor."},
/* it */ {IDMES_ERROR_CONNECTING_CLIENT_IT, "Si è verificato un errore durante il collegamento col client."},
/* nl */ {IDMES_ERROR_CONNECTING_CLIENT_NL, "Een fout is opgetreden bij het verbinden."},
/* pl */ {IDMES_ERROR_CONNECTING_CLIENT_PL, ""},  /* fuzzy */
/* sv */ {IDMES_ERROR_CONNECTING_CLIENT_SV, "Fel vid anslutning till klient."},

/* en */ {IDMES_INVALID_PORT_NUMBER,    "Invalid port number"},
/* de */ {IDMES_INVALID_PORT_NUMBER_DE, "Ungültige Port Nummer"},
/* fr */ {IDMES_INVALID_PORT_NUMBER_FR, ""},  /* fuzzy */
/* hu */ {IDMES_INVALID_PORT_NUMBER_HU, "Érvénytelen port szám"},
/* it */ {IDMES_INVALID_PORT_NUMBER_IT, "Numero di porta non valido"},
/* nl */ {IDMES_INVALID_PORT_NUMBER_NL, "Ongeldig poort nummer"},
/* pl */ {IDMES_INVALID_PORT_NUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDMES_INVALID_PORT_NUMBER_SV, "Ogiltigt portnummer"},

/* en */ {IDMES_THIS_MACHINE_NO_SID,    "This machine may not have a SID"},
/* de */ {IDMES_THIS_MACHINE_NO_SID_DE, "Diese Maschine hat möglicherweise keinen SID"},
/* fr */ {IDMES_THIS_MACHINE_NO_SID_FR, "Cette machine n'a peut-être pas de SID"},
/* hu */ {IDMES_THIS_MACHINE_NO_SID_HU, "Ennek a gépnek nem lehet SIDje"},
/* it */ {IDMES_THIS_MACHINE_NO_SID_IT, "Questa macchina potrebbe non avere un SID"},
/* nl */ {IDMES_THIS_MACHINE_NO_SID_NL, "Deze machine heeft mogelijk geen SID"},
/* pl */ {IDMES_THIS_MACHINE_NO_SID_PL, ""},  /* fuzzy */
/* sv */ {IDMES_THIS_MACHINE_NO_SID_SV, "Denna maskin kan inte ha en SID"},

/* en */ {IDMES_SCREENSHOT_SAVE_S_FAILED,    "Screenshot save of %s failed"},
/* de */ {IDMES_SCREENSHOT_SAVE_S_FAILED_DE, "Screenshot speicherung von %s ist fehlgeschlagen"},
/* fr */ {IDMES_SCREENSHOT_SAVE_S_FAILED_FR, ""},  /* fuzzy */
/* hu */ {IDMES_SCREENSHOT_SAVE_S_FAILED_HU, "Fénykép fájl mentése %s fájlba nem sikerült"},
/* it */ {IDMES_SCREENSHOT_SAVE_S_FAILED_IT, "Il salvataggio della schermata %s è fallito"},
/* nl */ {IDMES_SCREENSHOT_SAVE_S_FAILED_NL, "Opslaan schermafdrukbestand %s is mislukt"},
/* pl */ {IDMES_SCREENSHOT_SAVE_S_FAILED_PL, ""},  /* fuzzy */
/* sv */ {IDMES_SCREENSHOT_SAVE_S_FAILED_SV, "Fel vid skrivning av skärmdump \"%s\""},

/* en */ {IDMES_SCREENSHOT_S_SAVED,    "Screenshot %s saved."},
/* de */ {IDMES_SCREENSHOT_S_SAVED_DE, "Screenshots %s gespeicher."},
/* fr */ {IDMES_SCREENSHOT_S_SAVED_FR, ""},  /* fuzzy */
/* hu */ {IDMES_SCREENSHOT_S_SAVED_HU, "Fénykép %s fájlba elmentve."},
/* it */ {IDMES_SCREENSHOT_S_SAVED_IT, "Schermata %s salvata."},
/* nl */ {IDMES_SCREENSHOT_S_SAVED_NL, "Schermafdruk %s opgeslagen."},
/* pl */ {IDMES_SCREENSHOT_S_SAVED_PL, ""},  /* fuzzy */
/* sv */ {IDMES_SCREENSHOT_S_SAVED_SV, "Skärmdump \"%s\" sparad."},

/* ----------------------- AmigaOS Strings ----------------------- */

/* en */ {IDS_PRESS_KEY_BUTTON,    "Press desired key/button, move stick or press ESC for no key."},
/* de */ {IDS_PRESS_KEY_BUTTON_DE, "Gewünschte Taste/Knopf drücken, Knüppel bewegen oder ESC für keine Taste drücken."},
/* fr */ {IDS_PRESS_KEY_BUTTON_FR, ""},  /* fuzzy */
/* hu */ {IDS_PRESS_KEY_BUTTON_HU, "Nyomja meg a választott gombot, mozdítsa a botkormányt, vagy nyomjon ESC-et üreshez"},
/* it */ {IDS_PRESS_KEY_BUTTON_IT, "Premi il tasto/bottone desiderato, muovi la leva o premi ESC per non selezionare alcun tasto."},
/* nl */ {IDS_PRESS_KEY_BUTTON_NL, "Druk de toets/knop, beweeg de joystick of druk op ESC voor geen toets."},
/* pl */ {IDS_PRESS_KEY_BUTTON_PL, ""},  /* fuzzy */
/* sv */ {IDS_PRESS_KEY_BUTTON_SV, "Tryck önskad tangent/knapp, rör joysticken eller tryck ESC för att inte välja någon knapp."},

/* en */ {IDS_SAVE,    "Save"},
/* de */ {IDS_SAVE_DE, "Speichern"},
/* fr */ {IDS_SAVE_FR, "Enregistrer"},
/* hu */ {IDS_SAVE_HU, "Mentés"},
/* it */ {IDS_SAVE_IT, "Salva"},
/* nl */ {IDS_SAVE_NL, "Opslaan"},
/* pl */ {IDS_SAVE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_SV, "Spara"},

/* en */ {IDS_BROWSE,    "Browse"},
/* de */ {IDS_BROWSE_DE, "Stöbern"},
/* fr */ {IDS_BROWSE_FR, "Parcourir"},
/* hu */ {IDS_BROWSE_HU, "Tallóz"},
/* it */ {IDS_BROWSE_IT, "Sfoglia"},
/* nl */ {IDS_BROWSE_NL, "Bladeren"},
/* pl */ {IDS_BROWSE_PL, ""},  /* fuzzy */
/* sv */ {IDS_BROWSE_SV, "Bläddra"},

/* en */ {IDS_CURRENT_MODE,    "Current mode"},
/* de */ {IDS_CURRENT_MODE_DE, "Aktueller Modus"},
/* fr */ {IDS_CURRENT_MODE_FR, ""},  /* fuzzy */
/* hu */ {IDS_CURRENT_MODE_HU, "Jelenlegi mód"},
/* it */ {IDS_CURRENT_MODE_IT, "Modalità attuale"},
/* nl */ {IDS_CURRENT_MODE_NL, "Huidige modus"},
/* pl */ {IDS_CURRENT_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDS_CURRENT_MODE_SV, "Nuvarande läge"},

/* en */ {IDS_TCP_PORT,    "TCP-Port"},
/* de */ {IDS_TCP_PORT_DE, "TCP-Port"},
/* fr */ {IDS_TCP_PORT_FR, ""},  /* fuzzy */
/* hu */ {IDS_TCP_PORT_HU, "TCP-Port"},
/* it */ {IDS_TCP_PORT_IT, "Porta TCP"},
/* nl */ {IDS_TCP_PORT_NL, "TCP-Poort"},
/* pl */ {IDS_TCP_PORT_PL, ""},  /* fuzzy */
/* sv */ {IDS_TCP_PORT_SV, "TCP-port"},

/* en */ {IDS_START_SERVER,    "Start Server"},
/* de */ {IDS_START_SERVER_DE, "Server starten"},
/* fr */ {IDS_START_SERVER_FR, ""},  /* fuzzy */
/* hu */ {IDS_START_SERVER_HU, "Kiszolgáló elindítása"},
/* it */ {IDS_START_SERVER_IT, "Avvia server"},
/* nl */ {IDS_START_SERVER_NL, "Start server"},
/* pl */ {IDS_START_SERVER_PL, ""},  /* fuzzy */
/* sv */ {IDS_START_SERVER_SV, "Starta server"},

/* en */ {IDS_CONNECT_TO,    "Connect to"},
/* de */ {IDS_CONNECT_TO_DE, "Verbinden mit"},
/* fr */ {IDS_CONNECT_TO_FR, ""},  /* fuzzy */
/* hu */ {IDS_CONNECT_TO_HU, "Kapcsolódás ehhez:"},
/* it */ {IDS_CONNECT_TO_IT, "Connetti a"},
/* nl */ {IDS_CONNECT_TO_NL, "Maak verbinding naar"},
/* pl */ {IDS_CONNECT_TO_PL, ""},  /* fuzzy */
/* sv */ {IDS_CONNECT_TO_SV, "Anslut till"},

/* en */ {IDS_DISCONNECT,    "Disconnect"},
/* de */ {IDS_DISCONNECT_DE, "Verbindung Trennen"},
/* fr */ {IDS_DISCONNECT_FR, ""},  /* fuzzy */
/* hu */ {IDS_DISCONNECT_HU, "Szétkapcsolódás"},
/* it */ {IDS_DISCONNECT_IT, "Disconnetti"},
/* nl */ {IDS_DISCONNECT_NL, "Verbreek verbinding"},
/* pl */ {IDS_DISCONNECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_DISCONNECT_SV, "Koppla från"},

/* en */ {IDS_IDLE,    "Idle"},
/* de */ {IDS_IDLE_DE, "Idle"},
/* fr */ {IDS_IDLE_FR, ""},  /* fuzzy */
/* hu */ {IDS_IDLE_HU, "Tétlen"},
/* it */ {IDS_IDLE_IT, "Inattivo"},
/* nl */ {IDS_IDLE_NL, "Idle"},
/* pl */ {IDS_IDLE_PL, ""},  /* fuzzy */
/* sv */ {IDS_IDLE_SV, "Väntar"},

/* en */ {IDS_SERVER_LISTENING,    "Server listening"},
/* de */ {IDS_SERVER_LISTENING_DE, "Server wartet auf Verbindung"},
/* fr */ {IDS_SERVER_LISTENING_FR, ""},  /* fuzzy */
/* hu */ {IDS_SERVER_LISTENING_HU, "Várakozás kliensre"},
/* it */ {IDS_SERVER_LISTENING_IT, "Server in ascolto"},
/* nl */ {IDS_SERVER_LISTENING_NL, "Server wacht op verbinding"},
/* pl */ {IDS_SERVER_LISTENING_PL, ""},  /* fuzzy */
/* sv */ {IDS_SERVER_LISTENING_SV, "Servern lyssnar"},

/* en */ {IDS_CONNECTED_SERVER,    "Connected server"},
/* de */ {IDS_CONNECTED_SERVER_DE, "Verbunden, Server"},
/* fr */ {IDS_CONNECTED_SERVER_FR, ""},  /* fuzzy */
/* hu */ {IDS_CONNECTED_SERVER_HU, "Kiszolgáló a kapcsolatban"},
/* it */ {IDS_CONNECTED_SERVER_IT, "Server connesso"},
/* nl */ {IDS_CONNECTED_SERVER_NL, "Verbonden Server"},
/* pl */ {IDS_CONNECTED_SERVER_PL, ""},  /* fuzzy */
/* sv */ {IDS_CONNECTED_SERVER_SV, "Ansluten server"},

/* en */ {IDS_CONNECTED_CLIENT,    "Connected client"},
/* de */ {IDS_CONNECTED_CLIENT_DE, "Verbunden, Client"},
/* fr */ {IDS_CONNECTED_CLIENT_FR, ""},  /* fuzzy */
/* hu */ {IDS_CONNECTED_CLIENT_HU, "Kapcsolódó kliens"},
/* it */ {IDS_CONNECTED_CLIENT_IT, "Client connesso"},
/* nl */ {IDS_CONNECTED_CLIENT_NL, "Verbonden Client"},
/* pl */ {IDS_CONNECTED_CLIENT_PL, ""},  /* fuzzy */
/* sv */ {IDS_CONNECTED_CLIENT_SV, "Ansluten klient"},

/* en */ {IDS_NETPLAY_SETTINGS,    "Netplay Settings"},
/* de */ {IDS_NETPLAY_SETTINGS_DE, "Netplay Einstellungen"},
/* fr */ {IDS_NETPLAY_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_NETPLAY_SETTINGS_HU, "Hálózati játék beállításai"},
/* it */ {IDS_NETPLAY_SETTINGS_IT, "Impostazioni Netplay"},
/* nl */ {IDS_NETPLAY_SETTINGS_NL, "Netplayinstellingen"},
/* pl */ {IDS_NETPLAY_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_NETPLAY_SETTINGS_SV, "Inställningar för nätverksspel"},

/* en */ {IDS_SAVE_SCREENSHOT,    "Save Screenshot"},
/* de */ {IDS_SAVE_SCREENSHOT_DE, "Screenshot Speichern"},
/* fr */ {IDS_SAVE_SCREENSHOT_FR, "Enregistrer une capture d'écran"},
/* hu */ {IDS_SAVE_SCREENSHOT_HU, "Fénykép mentése"},
/* it */ {IDS_SAVE_SCREENSHOT_IT, "Salva schermata"},
/* nl */ {IDS_SAVE_SCREENSHOT_NL, "Schermafdruk opslaan"},
/* pl */ {IDS_SAVE_SCREENSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_SCREENSHOT_SV, "Spara skärmdump"},

/* en */ {IDS_CANCEL,    "Cancel"},
/* de */ {IDS_CANCEL_DE, "Abbruch"},
/* fr */ {IDS_CANCEL_FR, "Annuler"},
/* hu */ {IDS_CANCEL_HU, "Mégsem"},
/* it */ {IDS_CANCEL_IT, "Annulla"},
/* nl */ {IDS_CANCEL_NL, "Annuleren"},
/* pl */ {IDS_CANCEL_PL, ""},  /* fuzzy */
/* sv */ {IDS_CANCEL_SV, "Avbryt"},

/* en */ {IDS_CHOOSE_SCREENSHOT_FORMAT,    "Choose screenshot format"},
/* de */ {IDS_CHOOSE_SCREENSHOT_FORMAT_DE, "Wähle Screenshot Format"},
/* fr */ {IDS_CHOOSE_SCREENSHOT_FORMAT_FR, ""},  /* fuzzy */
/* hu */ {IDS_CHOOSE_SCREENSHOT_FORMAT_HU, "Válasszon fénykép formátumot"},
/* it */ {IDS_CHOOSE_SCREENSHOT_FORMAT_IT, "Seleziona il formato per il salvataggio della schermata"},
/* nl */ {IDS_CHOOSE_SCREENSHOT_FORMAT_NL, "Kies schermafdrukformaat"},
/* pl */ {IDS_CHOOSE_SCREENSHOT_FORMAT_PL, ""},  /* fuzzy */
/* sv */ {IDS_CHOOSE_SCREENSHOT_FORMAT_SV, "Välj önskat format för skärmdumpen"},

/* en */ {IDS_SAVE_S_SCREENSHOT,    "Save %s screenshot"},
/* de */ {IDS_SAVE_S_SCREENSHOT_DE, "Screenshot %s speichern"},
/* fr */ {IDS_SAVE_S_SCREENSHOT_FR, ""},  /* fuzzy */
/* hu */ {IDS_SAVE_S_SCREENSHOT_HU, "%s fénykép mentése"},
/* it */ {IDS_SAVE_S_SCREENSHOT_IT, "Salva schermata %s"},
/* nl */ {IDS_SAVE_S_SCREENSHOT_NL, "Sla schermafdruk %s op"},
/* pl */ {IDS_SAVE_S_SCREENSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_S_SCREENSHOT_SV, "Spara skärmdumpen i %s format"},

/* en */ {IDS_DETACHED_DEVICE_D,    "Detached device %d"},
/* de */ {IDS_DETACHED_DEVICE_D_DE, "Gerät %d entfernt"},
/* fr */ {IDS_DETACHED_DEVICE_D_FR, "Périphérique détaché %d"},
/* hu */ {IDS_DETACHED_DEVICE_D_HU, "%d eszköz leválasztva"},
/* it */ {IDS_DETACHED_DEVICE_D_IT, "Rimosso dispositivo %d"},
/* nl */ {IDS_DETACHED_DEVICE_D_NL, "Apparaat %d ontkoppelt"},
/* pl */ {IDS_DETACHED_DEVICE_D_PL, ""},  /* fuzzy */
/* sv */ {IDS_DETACHED_DEVICE_D_SV, "Kopplade fran enhet %d"},

/* en */ {IDS_ATTACHED_S_TO_DEVICE_D,    "Attached %s to device#%d"},
/* de */ {IDS_ATTACHED_S_TO_DEVICE_D_DE, "%s mit Gerät #%d verbunden"},
/* fr */ {IDS_ATTACHED_S_TO_DEVICE_D_FR, "%s attaché au périphérique #%s"},
/* hu */ {IDS_ATTACHED_S_TO_DEVICE_D_HU, "%s csatolva #%d eszközhöz"},
/* it */ {IDS_ATTACHED_S_TO_DEVICE_D_IT, "Selezionato %s per la periferica #%d"},
/* nl */ {IDS_ATTACHED_S_TO_DEVICE_D_NL, "%s aan apparaat#%d gekoppelt"},
/* pl */ {IDS_ATTACHED_S_TO_DEVICE_D_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACHED_S_TO_DEVICE_D_SV, "Anslöt %s till enhet %d"},

/* en */ {IDS_VICE_QUESTION,    "VICE Question"},
/* de */ {IDS_VICE_QUESTION_DE, "VICE Frage"},
/* fr */ {IDS_VICE_QUESTION_FR, "Question VICE"},
/* hu */ {IDS_VICE_QUESTION_HU, "VICE kérdés"},
/* it */ {IDS_VICE_QUESTION_IT, "Domanda di VICE"},
/* nl */ {IDS_VICE_QUESTION_NL, "VICE vraag"},
/* pl */ {IDS_VICE_QUESTION_PL, ""},  /* fuzzy */
/* sv */ {IDS_VICE_QUESTION_SV, "VICE-fraga"},

/* en */ {IDS_EXTEND_TO_40_TRACK,    "Extend image to 40-track format?"},
/* de */ {IDS_EXTEND_TO_40_TRACK_DE, "Image Dateu auf 40-Spur Format erweitern?"},
/* fr */ {IDS_EXTEND_TO_40_TRACK_FR, "Étendre l'image au format 40 pistes?"},
/* hu */ {IDS_EXTEND_TO_40_TRACK_HU, "Kiterjesszem a képmást 40 sávos formátumra?"},
/* it */ {IDS_EXTEND_TO_40_TRACK_IT, "Astendere l'immagine al formato 40 tracce?"},
/* nl */ {IDS_EXTEND_TO_40_TRACK_NL, "Bestand uitbreiden naar 40-sporen formaat?"},
/* pl */ {IDS_EXTEND_TO_40_TRACK_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXTEND_TO_40_TRACK_SV, "Utöka avbildning till 40-sparsformat?"},

/* en */ {IDS_YES_NO,    "Yes|No"},
/* de */ {IDS_YES_NO_DE, "Ja|Nein"},
/* fr */ {IDS_YES_NO_FR, ""},  /* fuzzy */
/* hu */ {IDS_YES_NO_HU, "Igen|Nem"},
/* it */ {IDS_YES_NO_IT, "Sì|No"},
/* nl */ {IDS_YES_NO_NL, "Ja|Nee"},
/* pl */ {IDS_YES_NO_PL, ""},  /* fuzzy */
/* sv */ {IDS_YES_NO_SV, "Ja|Nej"},

/* en */ {IDS_DETACHED_TAPE,    "Detached tape"},
/* de */ {IDS_DETACHED_TAPE_DE, "Band Image entfernt"},
/* fr */ {IDS_DETACHED_TAPE_FR, "Datassette détaché"},
/* hu */ {IDS_DETACHED_TAPE_HU, "Szalag leválasztva"},
/* it */ {IDS_DETACHED_TAPE_IT, "Cassetta rimossa"},
/* nl */ {IDS_DETACHED_TAPE_NL, "Tape ontkoppelt"},
/* pl */ {IDS_DETACHED_TAPE_PL, ""},  /* fuzzy */
/* sv */ {IDS_DETACHED_TAPE_SV, "Kopplade fran band"},

/* en */ {IDS_ATTACHED_TAPE_S,    "Attached tape %s"},
/* de */ {IDS_ATTACHED_TAPE_S_DE, "Band Image %s eingelegt"},
/* fr */ {IDS_ATTACHED_TAPE_S_FR, "Datassette %s attaché"},
/* hu */ {IDS_ATTACHED_TAPE_S_HU, "%s szalag csatolva"},
/* it */ {IDS_ATTACHED_TAPE_S_IT, "Cassetta %s selezionata"},
/* nl */ {IDS_ATTACHED_TAPE_S_NL, "Tape %s gekoppelt"},
/* pl */ {IDS_ATTACHED_TAPE_S_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACHED_TAPE_S_SV, "Anslöt band %s"},

/* en */ {IDS_SELECT_START_SNAPSHOT,    "Select start snapshot for event history"},
/* de */ {IDS_SELECT_START_SNAPSHOT_DE, "Startsnapshot für Ereignishistory festlegen"},
/* fr */ {IDS_SELECT_START_SNAPSHOT_FR, "Sélectionner la sauvegarde de départ pour l'historique des événements"},
/* hu */ {IDS_SELECT_START_SNAPSHOT_HU, "Válasszon kezdeti pillanatképet az esemény rögzítéshez"},
/* it */ {IDS_SELECT_START_SNAPSHOT_IT, "Seleziona lo snapshot iniziale per la cronologia degli eventi"},
/* nl */ {IDS_SELECT_START_SNAPSHOT_NL, "Selecteer start momentopname voor gebeurtenis geschiedenis"},
/* pl */ {IDS_SELECT_START_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_START_SNAPSHOT_SV, "Välj startögonblicksmodul för händelsehistorik"},

/* en */ {IDS_SELECT_END_SNAPSHOT,    "Select end snapshot for event history"},
/* de */ {IDS_SELECT_END_SNAPSHOT_DE, "Startsnapshot für Ereignishistory festlegen"},
/* fr */ {IDS_SELECT_END_SNAPSHOT_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_END_SNAPSHOT_HU, "Válasszon végsõ pillanatképet az esemény rögzítéshez"},
/* it */ {IDS_SELECT_END_SNAPSHOT_IT, "Seleziona lo snapshot finale per la cronologia degli eventi"},
/* nl */ {IDS_SELECT_END_SNAPSHOT_NL, "Selecteer eind momentopname voor gebeurtenis geschiedenis"},
/* pl */ {IDS_SELECT_END_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_END_SNAPSHOT_SV, "Välj slutögonblicksmodul för händelsehistorik"},

/* en */ {IDS_REALLY_EXIT,    "Do you really want to exit?\n\nAll the data present in the emulated RAM will be lost."},
/* de */ {IDS_REALLY_EXIT_DE, "Willst Du wirklich beenden?\n\nDaten im emulierten Speicher RAM gehen verloren."},
/* fr */ {IDS_REALLY_EXIT_FR, "Désirez-vous vraiment quitter?\n\nToutes les données présentes dans la mémoire de l'émulateur seront perdues."},
/* hu */ {IDS_REALLY_EXIT_HU, "Biztosan kilép?\n\nAz emulált memória teljes tartalma el fog veszni."},
/* it */ {IDS_REALLY_EXIT_IT, "Uscire davvero?\n\nTutti i dati presenti nella RAM emulata saranno persi."},
/* nl */ {IDS_REALLY_EXIT_NL, "Wilt U echt afsluiten?\n\nAlle data in geëmuleerd geheugen zal verloren gaan."},
/* pl */ {IDS_REALLY_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDS_REALLY_EXIT_SV, "Vill du verkligen avsluta?\n\nAlla data i emulerat RAM kommer förloras."},

/* en */ {IDS_ATTACH_CRT_IMAGE,    "Attach CRT cartridge image"},
/* de */ {IDS_ATTACH_CRT_IMAGE_DE, "CRT Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_CRT_IMAGE_FR, "Insérer une cartouche CRT"},
/* hu */ {IDS_ATTACH_CRT_IMAGE_HU, "CRT cartridge képmás csatolása"},
/* it */ {IDS_ATTACH_CRT_IMAGE_IT, "Seleziona l'immagine di una cartuccia CRT"},
/* nl */ {IDS_ATTACH_CRT_IMAGE_NL, "Koppel CRT cartridge bestand"},
/* pl */ {IDS_ATTACH_CRT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_CRT_IMAGE_SV, "Anslut CRT-insticksmodulfil"},

/* en */ {IDS_ATTACH_RAW_8KB_IMAGE,    "Attach raw 8KB cartridge image"},
/* de */ {IDS_ATTACH_RAW_8KB_IMAGE_DE, "8KB Erweiterungsmodul (raw) Image einlegen"},
/* fr */ {IDS_ATTACH_RAW_8KB_IMAGE_FR, "Insérer une cartouche 8KO"},
/* hu */ {IDS_ATTACH_RAW_8KB_IMAGE_HU, "Nyers 8KB-os cartridge képmás csatolása"},
/* it */ {IDS_ATTACH_RAW_8KB_IMAGE_IT, "Seleziona l'immagine di una cartuccia da 8KB"},
/* nl */ {IDS_ATTACH_RAW_8KB_IMAGE_NL, "Koppel binair 8KB cartridgebestand"},
/* pl */ {IDS_ATTACH_RAW_8KB_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_RAW_8KB_IMAGE_SV, "Anslut ra 8KB-insticksmodulavbilding"},

/* en */ {IDS_ATTACH_RAW_16KB_IMAGE,    "Attach raw 16KB cartridge image"},
/* de */ {IDS_ATTACH_RAW_16KB_IMAGE_DE, "16KB Erweiterungsmodul (raw) Image einlegen"},
/* fr */ {IDS_ATTACH_RAW_16KB_IMAGE_FR, "Insérer une cartouche 16KO"},
/* hu */ {IDS_ATTACH_RAW_16KB_IMAGE_HU, "Nyers 16KB-os cartridge képmás csatolása"},
/* it */ {IDS_ATTACH_RAW_16KB_IMAGE_IT, "Seleziona l'immagine di una cartuccia da 16KB"},
/* nl */ {IDS_ATTACH_RAW_16KB_IMAGE_NL, "Koppel binair 16KB cartridgebestand"},
/* pl */ {IDS_ATTACH_RAW_16KB_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_RAW_16KB_IMAGE_SV, "Anslut ra 16KB-insticksmodulavbilding"},

/* en */ {IDS_ATTACH_ACTION_REPLAY_IMAGE,    "Attach Action Replay cartridge image"},
/* de */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_DE, "Action Replay Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_FR, "Insérer une cartouche Action Replay"},
/* hu */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_HU, "Action Replay cartridge képmás csatolása"},
/* it */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_IT, "Seleziona l'immagine di una cartuccia Action Replay"},
/* nl */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_NL, "Koppel Action Replay cartridgebestand"},
/* pl */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_SV, "Anslut Action Replay-avbildningsfil"},

/* en */ {IDS_ATTACH_ACTION_REPLAY3_IMAGE,    "Attach Action Replay 3 cartridge image"},
/* de */ {IDS_ATTACH_ACTION_REPLAY3_IMAGE_DE, "Action Replay 3 Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_ACTION_REPLAY3_IMAGE_FR, "Insrer une cartouche Action Replay 3"},
/* hu */ {IDS_ATTACH_ACTION_REPLAY3_IMAGE_HU, "Action Replay 3 cartridge kpms csatolsa"},
/* it */ {IDS_ATTACH_ACTION_REPLAY3_IMAGE_IT, "Seleziona l'immagine di una cartuccia Action Replay 3"},
/* nl */ {IDS_ATTACH_ACTION_REPLAY3_IMAGE_NL, "Koppel Action Replay 3 cartridgebestand"},
/* pl */ {IDS_ATTACH_ACTION_REPLAY3_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_ACTION_REPLAY3_IMAGE_SV, "Anslut Action Replay 3-avbildningsfil"},

/* en */ {IDS_ATTACH_ACTION_REPLAY4_IMAGE,    "Attach Action Replay 4 cartridge image"},
/* de */ {IDS_ATTACH_ACTION_REPLAY4_IMAGE_DE, "Action Replay 4 Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_ACTION_REPLAY4_IMAGE_FR, "Insrer une cartouche Action Replay 4"},
/* hu */ {IDS_ATTACH_ACTION_REPLAY4_IMAGE_HU, "Action Replay 4 cartridge kpms csatolsa"},
/* it */ {IDS_ATTACH_ACTION_REPLAY4_IMAGE_IT, "Seleziona l'immagine di una cartuccia Action Replay 4"},
/* nl */ {IDS_ATTACH_ACTION_REPLAY4_IMAGE_NL, "Koppel Action Replay 4 cartridgebestand"},
/* pl */ {IDS_ATTACH_ACTION_REPLAY4_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_ACTION_REPLAY4_IMAGE_SV, "Anslut Action Replay 4-avbildningsfil"},

/* en */ {IDS_ATTACH_STARDOS_IMAGE,    "Attach StarDOS cartridge image"},
/* de */ {IDS_ATTACH_STARDOS_IMAGE_DE, ""},  /* fuzzy */
/* fr */ {IDS_ATTACH_STARDOS_IMAGE_FR, ""},  /* fuzzy */
/* hu */ {IDS_ATTACH_STARDOS_IMAGE_HU, ""},  /* fuzzy */
/* it */ {IDS_ATTACH_STARDOS_IMAGE_IT, ""},  /* fuzzy */
/* nl */ {IDS_ATTACH_STARDOS_IMAGE_NL, "Koppel StarDOS cartridgebestand"},
/* pl */ {IDS_ATTACH_STARDOS_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_STARDOS_IMAGE_SV, ""},  /* fuzzy */

/* en */ {IDS_ATTACH_ATOMIC_POWER_IMAGE,    "Attach Atomic Power cartridge image"},
/* de */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_DE, "Atomic Power Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_FR, "Insérer une cartouche Atomic Power"},
/* hu */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_HU, "Atomic Power cartridge képmás csatolása"},
/* it */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_IT, "Seleziona l'immagine di una cartuccia Atomic Power"},
/* nl */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_NL, "Koppel Atomic Power cartridgebestand"},
/* pl */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_SV, "Anslut Atomic Power-avbildningsfil"},

/* en */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE,    "Attach Epyx fastload cartridge image"},
/* de */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_DE, "Epyx Fastload Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_FR, "Insérer une cartouche Epyx FastLoad"},
/* hu */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_HU, "Epyx gyorstöltõ cartridge képmás csatolása"},
/* it */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_IT, "Seleziona l'immagine di una cartuccia Epyx fastload"},
/* nl */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_NL, "Koppel Epyx fastload cartridgebestand"},
/* pl */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_SV, "Anslut Epyx fastload-avbildningsfil"},

/* en */ {IDS_ATTACH_IEEE488_IMAGE,    "Attach IEEE interface cartridge image"},
/* de */ {IDS_ATTACH_IEEE488_IMAGE_DE, "IEEE Schnittstellen Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_IEEE488_IMAGE_FR, "Insérer une cartouche interface IEEE"},
/* hu */ {IDS_ATTACH_IEEE488_IMAGE_HU, "IEEE interfész cartridge képmás csatolása"},
/* it */ {IDS_ATTACH_IEEE488_IMAGE_IT, "Seleziona l'immagine di una cartuccia di interfaccia IEEE"},
/* nl */ {IDS_ATTACH_IEEE488_IMAGE_NL, "Koppel IEEE interface cartridgebestand"},
/* pl */ {IDS_ATTACH_IEEE488_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_IEEE488_IMAGE_SV, "Anslut IEE-gränssnittsavbildningsfil"},

/* en */ {IDS_ATTACH_RETRO_REPLAY_IMAGE,    "Attach Retro Replay cartridge image"},
/* de */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_DE, "Retro Replay Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_FR, "Insérer une cartouche Retro Replay"},
/* hu */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_HU, "Retro Replay cartridge képmás csatolása"},
/* it */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_IT, "Seleziona l'immagine di una cartuccia Retro Replay"},
/* nl */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_NL, "Koppel Retro Replay cartridgebestand"},
/* pl */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_SV, "Anslut Retro Replay-avbildningsfil"},

/* en */ {IDS_ATTACH_IDE64_IMAGE,    "Attach IDE64 interface cartridge image"},
/* de */ {IDS_ATTACH_IDE64_IMAGE_DE, "IDE64 Schnittstellen Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_IDE64_IMAGE_FR, "Insérer une cartouche interface IDE64"},
/* hu */ {IDS_ATTACH_IDE64_IMAGE_HU, "IDE64 interfész képmás csatolása"},
/* it */ {IDS_ATTACH_IDE64_IMAGE_IT, "Seleziona l'immagine di una cartuccia di interfaccia IDE64"},
/* nl */ {IDS_ATTACH_IDE64_IMAGE_NL, "Koppel IDE64 interface cartridgebestand"},
/* pl */ {IDS_ATTACH_IDE64_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_IDE64_IMAGE_SV, "Anslut IDE64-gränssnittsavbildningsfil"},

/* en */ {IDS_ATTACH_SS4_IMAGE,    "Attach Super Snapshot 4 cartridge image"},
/* de */ {IDS_ATTACH_SS4_IMAGE_DE, "Super Snapshot 4 Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_SS4_IMAGE_FR, "Insérer une cartouche Super Snapshot 4"},
/* hu */ {IDS_ATTACH_SS4_IMAGE_HU, "Super Snapshot 4 cartridge képmás csatolása"},
/* it */ {IDS_ATTACH_SS4_IMAGE_IT, "Seleziona l'immagine di una cartuccia Super Snapshot 4"},
/* nl */ {IDS_ATTACH_SS4_IMAGE_NL, "Koppel Super Snapshot 4 cartridgebestand"},
/* pl */ {IDS_ATTACH_SS4_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_SS4_IMAGE_SV, "Anslut Super Snapshot 4-avbildningsfil"},

/* en */ {IDS_ATTACH_SS5_IMAGE,    "Attach Super Snapshot 5 cartridge image"},
/* de */ {IDS_ATTACH_SS5_IMAGE_DE, "Super Snapshot 5 Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_SS5_IMAGE_FR, "Insérer une cartouche Super Snapshot 5"},
/* hu */ {IDS_ATTACH_SS5_IMAGE_HU, "Super Snapshot 5 cartridge képmás csatolása"},
/* it */ {IDS_ATTACH_SS5_IMAGE_IT, "Seleziona l'immagine di una cartuccia Super Snapshot 5"},
/* nl */ {IDS_ATTACH_SS5_IMAGE_NL, "Koppel Super Snapshot 5 cartridgebestand"},
/* pl */ {IDS_ATTACH_SS5_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_SS5_IMAGE_SV, "Anslut Super Snapshot 5-avbildningsfil"},

/* en */ {IDS_ATTACH_SB_IMAGE,    "Attach Structured Basic cartridge image"},
/* de */ {IDS_ATTACH_SB_IMAGE_DE, "Erweiterungsmodul Structured Basic einlegen"},
/* fr */ {IDS_ATTACH_SB_IMAGE_FR, ""},  /* fuzzy */
/* hu */ {IDS_ATTACH_SB_IMAGE_HU, "Structured Basic cartridge képmás csatolása"},
/* it */ {IDS_ATTACH_SB_IMAGE_IT, "Seleziona l'immagine di una cartuccia Structured Basic"},
/* nl */ {IDS_ATTACH_SB_IMAGE_NL, "Koppel Structured Basic cartridgebestand"},
/* pl */ {IDS_ATTACH_SB_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_SB_IMAGE_SV, "Anslut Structured Basic insticksavbildningsfil"},

/* en */ {IDS_AVAILABLE_CMDLINE_OPTIONS,    "\nAvailable command-line options:\n\n"},
/* de */ {IDS_AVAILABLE_CMDLINE_OPTIONS_DE, "\nZulässige Kommandozeilen Optionen:\n\n"},
/* fr */ {IDS_AVAILABLE_CMDLINE_OPTIONS_FR, "\nOptions de ligne de commande disponibles:\n\n"},
/* hu */ {IDS_AVAILABLE_CMDLINE_OPTIONS_HU, "\nLehetséges parancssori opciók:\n\n"},
/* it */ {IDS_AVAILABLE_CMDLINE_OPTIONS_IT, "\nOpzioni disponibili da riga di comando:\n\n"},
/* nl */ {IDS_AVAILABLE_CMDLINE_OPTIONS_NL, "\nBeschikbare command-line opties:\n\n"},
/* pl */ {IDS_AVAILABLE_CMDLINE_OPTIONS_PL, "\nTillgängliga kommandoradsflaggor:\n\n"},
/* sv */ {IDS_AVAILABLE_CMDLINE_OPTIONS_SV, "\nTillgängliga kommandoradsargument:\n\n"},

/* en */ {IDS_ATTACH_4_8_16KB_AT_2000,    "Attach 4/8/16KB cartridge image at $2000"},
/* de */ {IDS_ATTACH_4_8_16KB_AT_2000_DE, "4/8/16KB Modul Image bei $2000 einlegen"},
/* fr */ {IDS_ATTACH_4_8_16KB_AT_2000_FR, "Insertion d'une image 4/8/16Ko   $2000"},
/* hu */ {IDS_ATTACH_4_8_16KB_AT_2000_HU, "4/8/16KB képmás csatolása $2000 címre"},
/* it */ {IDS_ATTACH_4_8_16KB_AT_2000_IT, "Seleziona l'immagine di una cartuccia da 4/8/16KB a $2000"},
/* nl */ {IDS_ATTACH_4_8_16KB_AT_2000_NL, "Koppel 4/8/16KB-bestand op $2000"},
/* pl */ {IDS_ATTACH_4_8_16KB_AT_2000_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_4_8_16KB_AT_2000_SV, "Anslut 4/8/16KB-modulfil vid $2000"},

/* en */ {IDS_ATTACH_4_8_16KB_AT_4000,    "Attach 4/8/16KB cartridge image at $4000"},
/* de */ {IDS_ATTACH_4_8_16KB_AT_4000_DE, "4/8/16KB Modul Image bei $4000 einlegen"},
/* fr */ {IDS_ATTACH_4_8_16KB_AT_4000_FR, "Insertion d'une image 4/8/16Ko   $4000"},
/* hu */ {IDS_ATTACH_4_8_16KB_AT_4000_HU, "4/8/16KB képmás csatolása $4000 címre"},
/* it */ {IDS_ATTACH_4_8_16KB_AT_4000_IT, "Seleziona l'immagine di una cartuccia da 4/8/16KB a $4000"},
/* nl */ {IDS_ATTACH_4_8_16KB_AT_4000_NL, "Koppel 4/8/16KB-bestand op $4000"},
/* pl */ {IDS_ATTACH_4_8_16KB_AT_4000_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_4_8_16KB_AT_4000_SV, "Anslut 4/8/16KB-modulfil vid $4000"},

/* en */ {IDS_ATTACH_4_8_16KB_AT_6000,    "Attach 4/8/16KB cartridge image at $6000"},
/* de */ {IDS_ATTACH_4_8_16KB_AT_6000_DE, "4/8/16KB Modul Image bei $6000 einlegen"},
/* fr */ {IDS_ATTACH_4_8_16KB_AT_6000_FR, "Insertion d'une image 4/8/16Ko   $6000"},
/* hu */ {IDS_ATTACH_4_8_16KB_AT_6000_HU, "4/8/16KB képmás csatolása $6000 címre"},
/* it */ {IDS_ATTACH_4_8_16KB_AT_6000_IT, "Seleziona l'immagine di una cartuccia da 4/8/16KB a $6000"},
/* nl */ {IDS_ATTACH_4_8_16KB_AT_6000_NL, "Koppel 4/8/16KB-bestand op $6000"},
/* pl */ {IDS_ATTACH_4_8_16KB_AT_6000_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_4_8_16KB_AT_6000_SV, "Anslut 4/8/16KB-modulfil vid $6000"},

/* en */ {IDS_ATTACH_4_8KB_AT_A000,    "Attach 4/8KB cartridge image at $A000"},
/* de */ {IDS_ATTACH_4_8KB_AT_A000_DE, "4/8KB Modul Image bei $A000 einlegen"},
/* fr */ {IDS_ATTACH_4_8KB_AT_A000_FR, "Insertion d'une image 4/8Ko   $A000"},
/* hu */ {IDS_ATTACH_4_8KB_AT_A000_HU, "4/8KB képmás csatolása $A000 címre"},
/* it */ {IDS_ATTACH_4_8KB_AT_A000_IT, "Seleziona l'immagine di una cartuccia da 4/8KB a $A000"},
/* nl */ {IDS_ATTACH_4_8KB_AT_A000_NL, "Koppel 4/8KB-bestand op $A000"},
/* pl */ {IDS_ATTACH_4_8KB_AT_A000_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_4_8KB_AT_A000_SV, "Anslut 4/8KB-modulfil vid $A000"},

/* en */ {IDS_ATTACH_4KB_AT_B000,    "Attach 4KB cartridge image at $B000"},
/* de */ {IDS_ATTACH_4KB_AT_B000_DE, "4KB Modul Image bei $B000 einlegen"},
/* fr */ {IDS_ATTACH_4KB_AT_B000_FR, "Insertion d'une image 4Ko   $B000"},
/* hu */ {IDS_ATTACH_4KB_AT_B000_HU, "4KB képmás csatolása $B000 címre"},
/* it */ {IDS_ATTACH_4KB_AT_B000_IT, "Seleziona l'immagine di una cartuccia da 4KB a $B000"},
/* nl */ {IDS_ATTACH_4KB_AT_B000_NL, "Koppel 4KB-bestand op $B000"},
/* pl */ {IDS_ATTACH_4KB_AT_B000_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_4KB_AT_B000_SV, "Anslut 4KB-modulfil vid $B000"},

/* en */ {IDS_S_AT_D_SPEED,    "%s at %d%% speed, %d fps%s"},
/* de */ {IDS_S_AT_D_SPEED_DE, "%s bei %d%% Geschwindigkeit, %d fps%s"},
/* fr */ {IDS_S_AT_D_SPEED_FR, "%s   une vitesse de %d%%,   %d img/s%s"},
/* it */ {IDS_S_AT_D_SPEED_HU, "%s %d%% sebességen, %d fps%s"},
/* it */ {IDS_S_AT_D_SPEED_IT, "velocità %s al %d%%, %d fps%s"},
/* nl */ {IDS_S_AT_D_SPEED_NL, "%s met %d%% snelheid, %d fps%s"},
/* pl */ {IDS_S_AT_D_SPEED_PL, ""},  /* fuzzy */
/* sv */ {IDS_S_AT_D_SPEED_SV, "%s med %d%% hastighet, %d b/s%s"},

/* en */ {IDS_ATTACH,    "Attach"},
/* de */ {IDS_ATTACH_DE, "Einlegen"},
/* fr */ {IDS_ATTACH_FR, ""},  /* fuzzy */
/* hu */ {IDS_ATTACH_HU, "Csatolás"},
/* it */ {IDS_ATTACH_IT, "Seleziona"},
/* nl */ {IDS_ATTACH_NL, "Koppel"},
/* pl */ {IDS_ATTACH_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_SV, "Anslut"},

/* en */ {IDS_PARENT,    "Parent"},
/* de */ {IDS_PARENT_DE, "Vorgänger"},
/* fr */ {IDS_PARENT_FR, ""},  /* fuzzy */
/* hu */ {IDS_PARENT_HU, "Szülõ"},
/* it */ {IDS_PARENT_IT, "Livello superiore"},
/* nl */ {IDS_PARENT_NL, "Bovenliggende directory"},
/* pl */ {IDS_PARENT_PL, ""},  /* fuzzy */
/* sv */ {IDS_PARENT_SV, "Moderlåda"},

/* en */ {IDS_ATTACH_READ_ONLY,    "Attach read only"},
/* de */ {IDS_ATTACH_READ_ONLY_DE, "Schreibgeschützt einlegen"},
/* fr */ {IDS_ATTACH_READ_ONLY_FR, "Insérer en lecture seule"},
/* hu */ {IDS_ATTACH_READ_ONLY_HU, "Csatolás csak olvasható módon"},
/* it */ {IDS_ATTACH_READ_ONLY_IT, "Seleziona in sola lettura"},
/* nl */ {IDS_ATTACH_READ_ONLY_NL, "Alleen lezen"},
/* pl */ {IDS_ATTACH_READ_ONLY_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_READ_ONLY_SV, "Anslut skrivskyddat"},

/* en */ {IDS_NAME,    "Name"},
/* de */ {IDS_NAME_DE, "Name"},
/* fr */ {IDS_NAME_FR, "Nom"},
/* hu */ {IDS_NAME_HU, "Név"},
/* it */ {IDS_NAME_IT, "Nome"},
/* nl */ {IDS_NAME_NL, "Naam"},
/* pl */ {IDS_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_NAME_SV, "Namn"},

/* en */ {IDS_CREATE_IMAGE,    "Create Image"},
/* de */ {IDS_CREATE_IMAGE_DE, "Image Datei erzeugen"},
/* fr */ {IDS_CREATE_IMAGE_FR, "Création d'image"},
/* hu */ {IDS_CREATE_IMAGE_HU, "Képmás létrehozása"},
/* it */ {IDS_CREATE_IMAGE_IT, "Crea immagine"},
/* nl */ {IDS_CREATE_IMAGE_NL, "Maak Bestand"},
/* pl */ {IDS_CREATE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_CREATE_IMAGE_SV, "Skapa avbildning"},

/* en */ {IDS_NEW_IMAGE,    "New Image"},
/* de */ {IDS_NEW_IMAGE_DE, "Neue Image Datei"},
/* fr */ {IDS_NEW_IMAGE_FR, "Nouvelle image"},
/* hu */ {IDS_NEW_IMAGE_HU, "Új képmás"},
/* it */ {IDS_NEW_IMAGE_IT, "Nuova immagine"},
/* nl */ {IDS_NEW_IMAGE_NL, "Nieuw Bestand"},
/* pl */ {IDS_NEW_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEW_IMAGE_SV, "Ny avbildning"},

/* en */ {IDS_NEW_TAP_IMAGE,    "New TAP Image"},
/* de */ {IDS_NEW_TAP_IMAGE_DE, "Neue TAP Image Datei"},
/* fr */ {IDS_NEW_TAP_IMAGE_FR, "Nouvelle image TAP"},
/* hu */ {IDS_NEW_TAP_IMAGE_HU, "Új TAP képmás"},
/* it */ {IDS_NEW_TAP_IMAGE_IT, "Nuova immagine TAP"},
/* nl */ {IDS_NEW_TAP_IMAGE_NL, "Nieuw TAP-bestand"},
/* pl */ {IDS_NEW_TAP_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEW_TAP_IMAGE_SV, "Ny TAP-avbildning"},

/* en */ {IDS_OVERWRITE_EXISTING_IMAGE,    "Overwrite existing image?"},
/* de */ {IDS_OVERWRITE_EXISTING_IMAGE_DE, "Existierende Datei überschreiben?"},
/* fr */ {IDS_OVERWRITE_EXISTING_IMAGE_FR, "Écraser les images existantes?"},
/* hu */ {IDS_OVERWRITE_EXISTING_IMAGE_HU, "Felülírjam a meglévõ képmást?"},
/* it */ {IDS_OVERWRITE_EXISTING_IMAGE_IT, "Sovrascrivo immagine esistente?"},
/* nl */ {IDS_OVERWRITE_EXISTING_IMAGE_NL, "Bestaand bestand overschrijven?"},
/* pl */ {IDS_OVERWRITE_EXISTING_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_OVERWRITE_EXISTING_IMAGE_SV, "Skriva över existerande avbildning?"},

/* en */ {IDS_ABOUT,    "About"},
/* de */ {IDS_ABOUT_DE, "Über VICE"},
/* fr */ {IDS_ABOUT_FR, "À Propos"},
/* hu */ {IDS_ABOUT_HU, "Névjegy"},
/* it */ {IDS_ABOUT_IT, "Informazioni"},
/* nl */ {IDS_ABOUT_NL, "Over VICE"},
/* pl */ {IDS_ABOUT_PL, ""},  /* fuzzy */
/* sv */ {IDS_ABOUT_SV, "Om VICE"},

/* en */ {IDS_IMAGE_CONTENTS,    "Image Contents"},
/* de */ {IDS_IMAGE_CONTENTS_DE, "Image Datei Inhalt"},
/* fr */ {IDS_IMAGE_CONTENTS_FR, "Contenu de l'image"},
/* hu */ {IDS_IMAGE_CONTENTS_HU, "Tartalom"},
/* it */ {IDS_IMAGE_CONTENTS_IT, "Contenuti immagine"},
/* nl */ {IDS_IMAGE_CONTENTS_NL, "Bestandsinhoud"},
/* pl */ {IDS_IMAGE_CONTENTS_PL, ""},  /* fuzzy */
/* sv */ {IDS_IMAGE_CONTENTS_SV, "Innehall i avbildning"},

/* en */ {IDMS_DISABLED,    "Disabled"},
/* de */ {IDMS_DISABLED_DE, "Deaktiviert"},
/* fr */ {IDMS_DISABLED_FR, "Désactivé"},
/* hu */ {IDMS_DISABLED_HU, "Tiltva"},
/* it */ {IDMS_DISABLED_IT, "Disattivo"},
/* nl */ {IDMS_DISABLED_NL, "Uit"},
/* pl */ {IDMS_DISABLED_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DISABLED_SV, "Av"},

/* en */ {IDS_ENABLED,    "Enabled"},
/* de */ {IDS_ENABLED_DE, "Aktiviert"},
/* fr */ {IDS_ENABLED_FR, ""},  /* fuzzy */
/* hu */ {IDS_ENABLED_HU, "Engedélyezve"},
/* it */ {IDS_ENABLED_IT, "Attivo"},
/* nl */ {IDS_ENABLED_NL, "Aan"},
/* pl */ {IDS_ENABLED_PL, ""},  /* fuzzy */
/* sv */ {IDS_ENABLED_SV, "På"},

/* en */ {IDS_RS232_DEVICE_1,    "RS232 Device 1"},
/* de */ {IDS_RS232_DEVICE_1_DE, "RS232 Gerät 1"},
/* fr */ {IDS_RS232_DEVICE_1_FR, "1er Périphérique RS232"},
/* hu */ {IDS_RS232_DEVICE_1_HU, "RS232 1-es eszköz"},
/* it */ {IDS_RS232_DEVICE_1_IT, "Dispositivo RS232 1"},
/* nl */ {IDS_RS232_DEVICE_1_NL, "RS232 apparaat 1"},
/* pl */ {IDS_RS232_DEVICE_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_RS232_DEVICE_1_SV, "RS232-enhet 1"},

/* en */ {IDS_RS232_DEVICE_2,    "RS232 Device 2"},
/* de */ {IDS_RS232_DEVICE_2_DE, "RS232 Gerät 2"},
/* fr */ {IDS_RS232_DEVICE_2_FR, "2e Périphérique RS232"},
/* hu */ {IDS_RS232_DEVICE_2_HU, "RS232 2-es eszköz"},
/* it */ {IDS_RS232_DEVICE_2_IT, "Dispositivo RS232 2"},
/* nl */ {IDS_RS232_DEVICE_2_NL, "RS232 apparaat 2"},
/* pl */ {IDS_RS232_DEVICE_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_RS232_DEVICE_2_SV, "RS232-enhet 2"},

/* en */ {IDS_RS232_DEVICE_3,    "RS232 Device 3"},
/* de */ {IDS_RS232_DEVICE_3_DE, "RS232 Gerät 3"},
/* fr */ {IDS_RS232_DEVICE_3_FR, "3e Périphérique RS232"},
/* hu */ {IDS_RS232_DEVICE_3_HU, "RS232 3-as eszköz"},
/* it */ {IDS_RS232_DEVICE_3_IT, "Dispositivo RS232 3"},
/* nl */ {IDS_RS232_DEVICE_3_NL, "RS232 apparaat 3"},
/* pl */ {IDS_RS232_DEVICE_3_PL, ""},  /* fuzzy */
/* sv */ {IDS_RS232_DEVICE_3_SV, "RS232-enhet 3"},

/* en */ {IDS_RS232_DEVICE_4,    "RS232 Device 4"},
/* de */ {IDS_RS232_DEVICE_4_DE, "RS232 Gerät 4"},
/* fr */ {IDS_RS232_DEVICE_4_FR, "4e Périphérique RS232"},
/* hu */ {IDS_RS232_DEVICE_4_HU, "RS232 4-es eszköz"},
/* it */ {IDS_RS232_DEVICE_4_IT, "Dispositivo RS232 4"},
/* nl */ {IDS_RS232_DEVICE_4_NL, "RS232 apparaat 4"},
/* pl */ {IDS_RS232_DEVICE_4_PL, ""},  /* fuzzy */
/* sv */ {IDS_RS232_DEVICE_4_SV, "RS232-enhet 4"},

/* en */ {IDS_NONE,    "None"},
/* de */ {IDS_NONE_DE, "Kein"},
/* fr */ {IDS_NONE_FR, "Aucun"},
/* hu */ {IDS_NONE_HU, "Nincs"},
/* it */ {IDS_NONE_IT, "Nessuno"},
/* nl */ {IDS_NONE_NL, "Geen"},
/* pl */ {IDS_NONE_PL, ""},  /* fuzzy */
/* sv */ {IDS_NONE_SV, "Ingen"},

/* en */ {IDS_IRQ,    "IRQ"},
/* de */ {IDS_IRQ_DE, "IRQ"},
/* fr */ {IDS_IRQ_FR, "IRQ"},
/* hu */ {IDS_IRQ_HU, "IRQ"},
/* it */ {IDS_IRQ_IT, "IRQ"},
/* nl */ {IDS_IRQ_NL, "IRQ"},
/* pl */ {IDS_IRQ_PL, "IRQ"},
/* sv */ {IDS_IRQ_SV, "IRQ"},

/* en */ {IDS_NMI,    "NMI"},
/* de */ {IDS_NMI_DE, "NMI"},
/* fr */ {IDS_NMI_FR, "NMI"},
/* hu */ {IDS_NMI_HU, "NMI"},
/* it */ {IDS_NMI_IT, "NMI"},
/* nl */ {IDS_NMI_NL, "NMI"},
/* pl */ {IDS_NMI_PL, "NMI"},
/* sv */ {IDS_NMI_SV, "NMI"},

/* en */ {IDS_ACIA_DEVICE,    "ACIA Device"},
/* de */ {IDS_ACIA_DEVICE_DE, "ACIA Gerät"},
/* fr */ {IDS_ACIA_DEVICE_FR, "Périphérique ACIA"},
/* hu */ {IDS_ACIA_DEVICE_HU, "ACIA eszköz"},
/* it */ {IDS_ACIA_DEVICE_IT, "Dispositivo ACIA"},
/* nl */ {IDS_ACIA_DEVICE_NL, "ACIA-apparaat"},
/* pl */ {IDS_ACIA_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ACIA_DEVICE_SV, "ACIA-enhet"},

/* en */ {IDS_ACIA_INTERRUPT,    "ACIA Interrupt"},
/* de */ {IDS_ACIA_INTERRUPT_DE, "ACIA Interrupt"},
/* fr */ {IDS_ACIA_INTERRUPT_FR, "Interruption ACIA"},
/* hu */ {IDS_ACIA_INTERRUPT_HU, "ACIA megszakítás"},
/* it */ {IDS_ACIA_INTERRUPT_IT, "Interrupt dell'ACIA"},
/* nl */ {IDS_ACIA_INTERRUPT_NL, "ACIA-interrupt"},
/* pl */ {IDS_ACIA_INTERRUPT_PL, ""},  /* fuzzy */
/* sv */ {IDS_ACIA_INTERRUPT_SV, "ACIA-avbrott"},

/* en */ {IDS_ACIA_MODE,    "ACIA Mode"},
/* de */ {IDS_ACIA_MODE_DE, "ACIA Modus"},
/* fr */ {IDS_ACIA_MODE_FR, ""},  /* fuzzy */
/* hu */ {IDS_ACIA_MODE_HU, "ACIA mód"},
/* it */ {IDS_ACIA_MODE_IT, "Modalità ACIA"},
/* nl */ {IDS_ACIA_MODE_NL, "ACIA-modus"},
/* pl */ {IDS_ACIA_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ACIA_MODE_SV, "ACIA läge"},

/* en */ {IDS_ACIA_SETTINGS,    "ACIA Settings"},
/* de */ {IDS_ACIA_SETTINGS_DE, "ACIA Einstellungen"},
/* fr */ {IDS_ACIA_SETTINGS_FR, "Parametres ACIA"},
/* hu */ {IDS_ACIA_SETTINGS_HU, "ACIA beállításai"},
/* it */ {IDS_ACIA_SETTINGS_IT, "Impostazioni ACIA"},
/* nl */ {IDS_ACIA_SETTINGS_NL, "ACIA-instellingen"},
/* pl */ {IDS_ACIA_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_ACIA_SETTINGS_SV, "ACIA-inställningar"},

/* en */ {IDS_256K_ENABLED,    "256K Enabled"},
/* de */ {IDS_256K_ENABLED_DE, "256K aktiviert"},
/* fr */ {IDS_256K_ENABLED_FR, ""},  /* fuzzy */
/* hu */ {IDS_256K_ENABLED_HU, "256K engedélyezve"},
/* it */ {IDS_256K_ENABLED_IT, "256K attivi"},
/* nl */ {IDS_256K_ENABLED_NL, "256K Aan/Uit"},
/* pl */ {IDS_256K_ENABLED_PL, ""},  /* fuzzy */
/* sv */ {IDS_256K_ENABLED_SV, "256K aktiverat"},

/* en */ {IDS_256K_BASE,    "256K Base"},
/* de */ {IDS_256K_BASE_DE, "256K Basis"},
/* fr */ {IDS_256K_BASE_FR, ""},  /* fuzzy */
/* hu */ {IDS_256K_BASE_HU, "256K báziscím"},
/* it */ {IDS_256K_BASE_IT, "Indirizzo base 256K"},
/* nl */ {IDS_256K_BASE_NL, "256K basis adres"},
/* pl */ {IDS_256K_BASE_PL, ""},  /* fuzzy */
/* sv */ {IDS_256K_BASE_SV, "256K bas-adress"},

/* en */ {IDS_256K_FILENAME,    "256K file"},
/* de */ {IDS_256K_FILENAME_DE, "256K Datei"},
/* fr */ {IDS_256K_FILENAME_FR, "Fichier 256K"},
/* hu */ {IDS_256K_FILENAME_HU, "256K fájl"},
/* it */ {IDS_256K_FILENAME_IT, "File 256K"},
/* nl */ {IDS_256K_FILENAME_NL, "256K-bestand"},
/* pl */ {IDS_256K_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_256K_FILENAME_SV, "256K-fil"},

/* en */ {IDS_256K_FILENAME_SELECT,    "Select file for 256K"},
/* de */ {IDS_256K_FILENAME_SELECT_DE, "Datei für 256K selektieren"},
/* fr */ {IDS_256K_FILENAME_SELECT_FR, "Sélectionner fichier pour 256K"},
/* hu */ {IDS_256K_FILENAME_SELECT_HU, "Válasszon fájlt a 256K-hoz"},
/* it */ {IDS_256K_FILENAME_SELECT_IT, "Seleziona il file per il 256K"},
/* nl */ {IDS_256K_FILENAME_SELECT_NL, "Selecteer bestand voor 256K"},
/* pl */ {IDS_256K_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_256K_FILENAME_SELECT_SV, "Ange fil för 256K"},

/* en */ {IDS_256K_SETTINGS,    "256K Settings"},
/* de */ {IDS_256K_SETTINGS_DE, "256K Einstellungen"},
/* fr */ {IDS_256K_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_256K_SETTINGS_HU, "256K beállításai"},
/* it */ {IDS_256K_SETTINGS_IT, "Impostazioni 256K"},
/* nl */ {IDS_256K_SETTINGS_NL, "256K-instellingen"},
/* pl */ {IDS_256K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_256K_SETTINGS_SV, "256K-inställningar"},

/* en */ {IDS_MODEL_LINE,    "model line"},
/* de */ {IDS_MODEL_LINE_DE, "modell linie"},
/* fr */ {IDS_MODEL_LINE_FR, "ligne du modele"},
/* hu */ {IDS_MODEL_LINE_HU, "modell sor"},
/* it */ {IDS_MODEL_LINE_IT, "impostazioni del modello"},
/* nl */ {IDS_MODEL_LINE_NL, "modellijn"},
/* pl */ {IDS_MODEL_LINE_PL, ""},  /* fuzzy */
/* sv */ {IDS_MODEL_LINE_SV, "modelltyp"},

/* en */ {IDS_RAM_SIZE,    "RAM Size"},
/* de */ {IDS_RAM_SIZE_DE, "RAM Größe"},
/* fr */ {IDS_RAM_SIZE_FR, "Taille du RAM"},
/* hu */ {IDS_RAM_SIZE_HU, "RAM mérete"},
/* it */ {IDS_RAM_SIZE_IT, "Dimensione RAM"},
/* nl */ {IDS_RAM_SIZE_NL, "RAM-grootte"},
/* pl */ {IDS_RAM_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_SIZE_SV, "RAM-storlek"},

/* en */ {IDS_RAM_BLOCK_0400_0FFF,    "RAM Block $0400-$0FFF"},
/* de */ {IDS_RAM_BLOCK_0400_0FFF_DE, "RAM in $0400-$0FFF"},
/* fr */ {IDS_RAM_BLOCK_0400_0FFF_FR, "RAM dans $0400-$0FFF"},
/* hu */ {IDS_RAM_BLOCK_0400_0FFF_HU, "Memória blokk $0400-$0FFF"},
/* it */ {IDS_RAM_BLOCK_0400_0FFF_IT, "Blocco RAM $0400-$0FFF"},
/* nl */ {IDS_RAM_BLOCK_0400_0FFF_NL, "RAM-blok $0400-$0FFF"},
/* pl */ {IDS_RAM_BLOCK_0400_0FFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_0400_0FFF_SV, "RAM pa $0400-$0FFF"},

/* en */ {IDS_RAM_BLOCK_0800_0FFF,    "RAM Block $0800-$0FFF"},
/* de */ {IDS_RAM_BLOCK_0800_0FFF_DE, "RAM in $0800-$0FFF"},
/* fr */ {IDS_RAM_BLOCK_0800_0FFF_FR, "RAM dans $0800-$0FFF"},
/* hu */ {IDS_RAM_BLOCK_0800_0FFF_HU, "Memória blokk $0800-$0FFF"},
/* it */ {IDS_RAM_BLOCK_0800_0FFF_IT, "Blocco RAM $0800-$0FFF"},
/* nl */ {IDS_RAM_BLOCK_0800_0FFF_NL, "RAM-blok $0800-$0FFF"},
/* pl */ {IDS_RAM_BLOCK_0800_0FFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_0800_0FFF_SV, "RAM pa $0800-$0FFF"},

/* en */ {IDS_RAM_BLOCK_1000_1FFF,    "RAM Block $1000-$1FFF"},
/* de */ {IDS_RAM_BLOCK_1000_1FFF_DE, "RAM in $1000-$1FFF"},
/* fr */ {IDS_RAM_BLOCK_1000_1FFF_FR, "RAM dans $1000-$1FFF"},
/* hu */ {IDS_RAM_BLOCK_1000_1FFF_HU, "Memória blokk $1000-$1FFF"},
/* it */ {IDS_RAM_BLOCK_1000_1FFF_IT, "Blocco RAM $1000-$1FFF"},
/* nl */ {IDS_RAM_BLOCK_1000_1FFF_NL, "RAM-blok $1000-$1FFF"},
/* pl */ {IDS_RAM_BLOCK_1000_1FFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_1000_1FFF_SV, "RAM pa $1000-$1FFF"},

/* en */ {IDS_RAM_BLOCK_2000_3FFF,    "RAM Block $2000-$3FFF"},
/* de */ {IDS_RAM_BLOCK_2000_3FFF_DE, "RAM in $2000-$3FFF"},
/* fr */ {IDS_RAM_BLOCK_2000_3FFF_FR, "RAM dans $2000-$3FFF"},
/* hu */ {IDS_RAM_BLOCK_2000_3FFF_HU, "Memória blokk $2000-$3FFF"},
/* it */ {IDS_RAM_BLOCK_2000_3FFF_IT, "Blocco RAM $2000-$3FFF"},
/* nl */ {IDS_RAM_BLOCK_2000_3FFF_NL, "RAM-blok $2000-$3FFF"},
/* pl */ {IDS_RAM_BLOCK_2000_3FFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_2000_3FFF_SV, "RAM pa $2000-$3FFF"},

/* en */ {IDS_RAM_BLOCK_4000_5FFF,    "RAM Block $4000-$5FFF"},
/* de */ {IDS_RAM_BLOCK_4000_5FFF_DE, "RAM in $4000-$5FFF"},
/* fr */ {IDS_RAM_BLOCK_4000_5FFF_FR, "RAM dans $4000-$5FFF"},
/* hu */ {IDS_RAM_BLOCK_4000_5FFF_HU, "Memória blokk $4000-$5FFF"},
/* it */ {IDS_RAM_BLOCK_4000_5FFF_IT, "Blocco RAM $4000-$5FFF"},
/* nl */ {IDS_RAM_BLOCK_4000_5FFF_NL, "RAM-blok $4000-$5FFF"},
/* pl */ {IDS_RAM_BLOCK_4000_5FFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_4000_5FFF_SV, "RAM pa $4000-$5FFF"},

/* en */ {IDS_RAM_BLOCK_6000_7FFF,    "RAM Block $6000-$7FFF"},
/* de */ {IDS_RAM_BLOCK_6000_7FFF_DE, "RAM in $6000-$7FFF"},
/* fr */ {IDS_RAM_BLOCK_6000_7FFF_FR, "RAM dans $6000-$7FFF"},
/* hu */ {IDS_RAM_BLOCK_6000_7FFF_HU, "Memória blokk $6000-$7FFF"},
/* it */ {IDS_RAM_BLOCK_6000_7FFF_IT, "Blocco RAM $6000-$7FFF"},
/* nl */ {IDS_RAM_BLOCK_6000_7FFF_NL, "RAM-blok $6000-$7FFF"},
/* pl */ {IDS_RAM_BLOCK_6000_7FFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_6000_7FFF_SV, "RAM pa $6000-$7FFF"},

/* en */ {IDS_RAM_BLOCK_A000_BFFF,    "RAM Block $A000-$BFFF"},
/* de */ {IDS_RAM_BLOCK_A000_BFFF_DE, "RAM in $A000-$BFFF"},
/* fr */ {IDS_RAM_BLOCK_A000_BFFF_FR, "RAM dans $A000-$BFFF"},
/* hu */ {IDS_RAM_BLOCK_A000_BFFF_HU, "Memória blokk $A000-$BFFF"},
/* it */ {IDS_RAM_BLOCK_A000_BFFF_IT, "Blocco RAM $A000-$BFFF"},
/* nl */ {IDS_RAM_BLOCK_A000_BFFF_NL, "RAM-blok $A000-$BFFF"},
/* pl */ {IDS_RAM_BLOCK_A000_BFFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_A000_BFFF_SV, "RAM pa $A000-$BFFF"},

/* en */ {IDS_RAM_BLOCK_C000_CFFF,    "RAM Block $C000-$CFFF"},
/* de */ {IDS_RAM_BLOCK_C000_CFFF_DE, "RAM in $C000-$CFFF"},
/* fr */ {IDS_RAM_BLOCK_C000_CFFF_FR, "RAM dans $C000-$CFFF"},
/* hu */ {IDS_RAM_BLOCK_C000_CFFF_HU, "Memória blokk $C000-$CFFF"},
/* it */ {IDS_RAM_BLOCK_C000_CFFF_IT, "Blocco RAM $C000-$CFFF"},
/* nl */ {IDS_RAM_BLOCK_C000_CFFF_NL, "RAM-blok $C000-$CFFF"},
/* pl */ {IDS_RAM_BLOCK_C000_CFFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_C000_CFFF_SV, "RAM pa $C000-$CFFF"},

/* en */ {IDS_CBM2_SETTINGS,    "CBM2 Settings"},
/* de */ {IDS_CBM2_SETTINGS_DE, "CBM2 Einstellungen"},
/* fr */ {IDS_CBM2_SETTINGS_FR, "Parametres CBM2"},
/* hu */ {IDS_CBM2_SETTINGS_HU, "CBM2 Beállításai"},
/* it */ {IDS_CBM2_SETTINGS_IT, "Impostazioni CBM2"},
/* nl */ {IDS_CBM2_SETTINGS_NL, "CBM2-instellingen"},
/* pl */ {IDS_CBM2_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_CBM2_SETTINGS_SV, "CBM2-inställningar"},

/* en */ {IDS_RESET_DATASETTE_WITH_CPU,    "Reset Datasette with CPU"},
/* de */ {IDS_RESET_DATASETTE_WITH_CPU_DE, "Zurücksetzen des Bandlaufwerks bei CPU Reset"},
/* fr */ {IDS_RESET_DATASETTE_WITH_CPU_FR, "Réinitialiser la datassette avec le CPU"},
/* hu */ {IDS_RESET_DATASETTE_WITH_CPU_HU, "Alaphelyzetbe hozás CPU Reset esetén"},
/* it */ {IDS_RESET_DATASETTE_WITH_CPU_IT, "Reset del registratore al reset della CPU"},
/* nl */ {IDS_RESET_DATASETTE_WITH_CPU_NL, "Reset Datasette met CPU"},
/* pl */ {IDS_RESET_DATASETTE_WITH_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDS_RESET_DATASETTE_WITH_CPU_SV, "Nollställ Datasette med CPU"},

/* en */ {IDS_ADDITIONAL_DELAY,    "Additional Delay"},
/* de */ {IDS_ADDITIONAL_DELAY_DE, "Zusätzliche Verzögerung"},
/* fr */ {IDS_ADDITIONAL_DELAY_FR, ""},  /* fuzzy */
/* hu */ {IDS_ADDITIONAL_DELAY_HU, "Többlet szünet"},
/* it */ {IDS_ADDITIONAL_DELAY_IT, "Ritardo aggiuntivo"},
/* nl */ {IDS_ADDITIONAL_DELAY_NL, "Extra vertraging"},
/* pl */ {IDS_ADDITIONAL_DELAY_PL, ""},  /* fuzzy */
/* sv */ {IDS_ADDITIONAL_DELAY_SV, "Ytterligare fördröjning"},

/* en */ {IDS_DELAY_AT_ZERO_VALUES,    "Delay at Zero Values"},
/* de */ {IDS_DELAY_AT_ZERO_VALUES_DE, "Verzögerung bei Zero-Werte"},
/* fr */ {IDS_DELAY_AT_ZERO_VALUES_FR, "Délai aux valeurs-zéro"},
/* hu */ {IDS_DELAY_AT_ZERO_VALUES_HU, "Szünet nulla értékeknél"},
/* it */ {IDS_DELAY_AT_ZERO_VALUES_IT, "Ritardo ai valori di zero"},
/* nl */ {IDS_DELAY_AT_ZERO_VALUES_NL, "Nul-waarden vertraging"},
/* pl */ {IDS_DELAY_AT_ZERO_VALUES_PL, ""},  /* fuzzy */
/* sv */ {IDS_DELAY_AT_ZERO_VALUES_SV, "Fördröjning vid &nollvärden"},

/* en */ {IDS_DATASETTE_SETTINGS,    "Datasette Settings"},
/* de */ {IDS_DATASETTE_SETTINGS_DE, "Datasette Einstellungen"},
/* fr */ {IDS_DATASETTE_SETTINGS_FR, "Réglages datassette"},
/* hu */ {IDS_DATASETTE_SETTINGS_HU, "Magnó beállításai"},
/* it */ {IDS_DATASETTE_SETTINGS_IT, "Impostazioni registratore"},
/* nl */ {IDS_DATASETTE_SETTINGS_NL, "Datasetteinstellingen"},
/* pl */ {IDS_DATASETTE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_DATASETTE_SETTINGS_SV, "Datasetteinställningar"},

/* en */ {IDS_NEVER_EXTEND,    "Never extend"},
/* de */ {IDS_NEVER_EXTEND_DE, "Nie Erweitern"},
/* fr */ {IDS_NEVER_EXTEND_FR, "Ne jamais é&tendre"},
/* hu */ {IDS_NEVER_EXTEND_HU, "Nincs kiterjesztés"},
/* it */ {IDS_NEVER_EXTEND_IT, "Non estendere mai"},
/* nl */ {IDS_NEVER_EXTEND_NL, "Nooit uitbreiden"},
/* pl */ {IDS_NEVER_EXTEND_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEVER_EXTEND_SV, "Utöka aldrig"},

/* en */ {IDS_ASK_ON_EXTEND,    "Ask on extend"},
/* de */ {IDS_ASK_ON_EXTEND_DE, "Bei Erweiterung Rückfrage"},
/* fr */ {IDS_ASK_ON_EXTEND_FR, "Demander avant d'étendre"},
/* hu */ {IDS_ASK_ON_EXTEND_HU, "Kérdés kiterjesztés esetén"},
/* it */ {IDS_ASK_ON_EXTEND_IT, "Estendi su richiesta"},
/* nl */ {IDS_ASK_ON_EXTEND_NL, "Vragen bij uitbreiden"},
/* pl */ {IDS_ASK_ON_EXTEND_PL, ""},  /* fuzzy */
/* sv */ {IDS_ASK_ON_EXTEND_SV, "Fraga vid utökning"},

/* en */ {IDS_EXTEND_ON_ACCESS,    "Extend on access"},
/* de */ {IDS_EXTEND_ON_ACCESS_DE, "Erweitern wenn nötig"},
/* fr */ {IDS_EXTEND_ON_ACCESS_FR, "Étendre   l'acces"},
/* hu */ {IDS_EXTEND_ON_ACCESS_HU, "Kiterjesztés hozzáféréskor"},
/* it */ {IDS_EXTEND_ON_ACCESS_IT, "All'accesso"},
/* nl */ {IDS_EXTEND_ON_ACCESS_NL, "Uitbreiden bij toegang"},
/* pl */ {IDS_EXTEND_ON_ACCESS_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXTEND_ON_ACCESS_SV, "Utöka vid atkomst"},

/* en */ {IDS_NO_TRAPS,    "No traps"},
/* de */ {IDS_NO_TRAPS_DE, "Kein Traps"},
/* fr */ {IDS_NO_TRAPS_FR, "No traps"},
/* hu */ {IDS_NO_TRAPS_HU, "Folyamatos emuláció"},
/* it */ {IDS_NO_TRAPS_IT, "Non rilevare"},
/* nl */ {IDS_NO_TRAPS_NL, "Geen traps"},
/* pl */ {IDS_NO_TRAPS_PL, ""},  /* fuzzy */
/* sv */ {IDS_NO_TRAPS_SV, "Inga Traps"},

/* en */ {IDS_SKIP_CYCLES,    "Skip cycles"},
/* de */ {IDS_SKIP_CYCLES_DE, "Zyklen verwerfen"},
/* fr */ {IDS_SKIP_CYCLES_FR, "Sauter des cycles"},
/* hu */ {IDS_SKIP_CYCLES_HU, "Ciklusok kihagyása"},
/* it */ {IDS_SKIP_CYCLES_IT, "Salta cicli"},
/* nl */ {IDS_SKIP_CYCLES_NL, "Sla cycli over"},
/* pl */ {IDS_SKIP_CYCLES_PL, ""},  /* fuzzy */
/* sv */ {IDS_SKIP_CYCLES_SV, "Hoppa cykler"},

/* en */ {IDS_TRAP_IDLE,    "Trap idle"},
/* de */ {IDS_TRAP_IDLE_DE, "Trap idle"},
/* fr */ {IDS_TRAP_IDLE_FR, "Trap idle"},
/* hu */ {IDS_TRAP_IDLE_HU, "Üres ciklusok kihagyása"},
/* it */ {IDS_TRAP_IDLE_IT, "Rileva inattività"},
/* nl */ {IDS_TRAP_IDLE_NL, "Trap idle"},
/* pl */ {IDS_TRAP_IDLE_PL, ""},  /* fuzzy */
/* sv */ {IDS_TRAP_IDLE_SV, "Fånga inaktiva"},

/* en */ {IDS_DRIVE_TYPE,    "Drive type"},
/* de */ {IDS_DRIVE_TYPE_DE, "Geräte Typ"},
/* fr */ {IDS_DRIVE_TYPE_FR, "Type de lecteur"},
/* hu */ {IDS_DRIVE_TYPE_HU, "Lemezegység típus"},
/* it */ {IDS_DRIVE_TYPE_IT, "Tipo di drive"},
/* nl */ {IDS_DRIVE_TYPE_NL, "Drivesoort"},
/* pl */ {IDS_DRIVE_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDS_DRIVE_TYPE_SV, "Enhetstyp"},

/* en */ {IDS_40_TRACK_HANDLING,    "40 track handling"},
/* de */ {IDS_40_TRACK_HANDLING_DE, "40 Spur Umgang"},
/* fr */ {IDS_40_TRACK_HANDLING_FR, "Prise en charge du 40 pistes"},
/* hu */ {IDS_40_TRACK_HANDLING_HU, "40 sáv kezelése"},
/* it */ {IDS_40_TRACK_HANDLING_IT, "Gestione 40 tracce"},
/* nl */ {IDS_40_TRACK_HANDLING_NL, "40-sporenondersteuning"},
/* pl */ {IDS_40_TRACK_HANDLING_PL, ""},  /* fuzzy */
/* sv */ {IDS_40_TRACK_HANDLING_SV, "Hantering av 40 spar"},

/* en */ {IDS_DRIVE_EXPANSION,    "Drive expansion"},
/* de */ {IDS_DRIVE_EXPANSION_DE, "Laufwerk Erweiterung"},
/* fr */ {IDS_DRIVE_EXPANSION_FR, "Expansion de lecteur"},
/* hu */ {IDS_DRIVE_EXPANSION_HU, "Lemezegység bõvítés"},
/* it */ {IDS_DRIVE_EXPANSION_IT, "Espasione del drive"},
/* nl */ {IDS_DRIVE_EXPANSION_NL, "Drive uitbreiding"},
/* pl */ {IDS_DRIVE_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDS_DRIVE_EXPANSION_SV, "Enhetsutökning"},

/* en */ {IDS_IDLE_METHOD,    "Idle method"},
/* de */ {IDS_IDLE_METHOD_DE, "Idle Methode"},
/* fr */ {IDS_IDLE_METHOD_FR, "Méthode d'idle"},
/* hu */ {IDS_IDLE_METHOD_HU, "Üresjárat mód"},
/* it */ {IDS_IDLE_METHOD_IT, "Rileva inattività"},
/* nl */ {IDS_IDLE_METHOD_NL, "Idlemethode"},
/* pl */ {IDS_IDLE_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDS_IDLE_METHOD_SV, "Pausmetod"},

/* en */ {IDS_PARALLEL_CABLE,    "Parallel cable"},
/* de */ {IDS_PARALLEL_CABLE_DE, "Parallel Kabel"},
/* fr */ {IDS_PARALLEL_CABLE_FR, "Câble parallele"},
/* hu */ {IDS_PARALLEL_CABLE_HU, "Párhuzamos kábel"},
/* it */ {IDS_PARALLEL_CABLE_IT, "Cavo parallelo"},
/* nl */ {IDS_PARALLEL_CABLE_NL, "Parallelle kabel"},
/* pl */ {IDS_PARALLEL_CABLE_PL, ""},  /* fuzzy */
/* sv */ {IDS_PARALLEL_CABLE_SV, "Parallellkabel"},

/* en */ {IDS_DRIVE_SETTINGS,    "Drive Settings"},
/* de */ {IDS_DRIVE_SETTINGS_DE, "Floppy Einstellungen"},
/* fr */ {IDS_DRIVE_SETTINGS_FR, "Parametres des lecteurs"},
/* hu */ {IDS_DRIVE_SETTINGS_HU, "Lemezegység beállításai"},
/* it */ {IDS_DRIVE_SETTINGS_IT, "Impostazioni drive"},
/* nl */ {IDS_DRIVE_SETTINGS_NL, "Drive-instellingen"},
/* pl */ {IDS_DRIVE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_DRIVE_SETTINGS_SV, "Diskettenhetsinställningar"},

/* en */ {IDS_GEORAM_SIZE,    "GEORAM Size"},
/* de */ {IDS_GEORAM_SIZE_DE, "GEORAM Größe"},
/* fr */ {IDS_GEORAM_SIZE_FR, "Taille du GEORAM"},
/* hu */ {IDS_GEORAM_SIZE_HU, "GEORAM mérete"},
/* it */ {IDS_GEORAM_SIZE_IT, "Dimensione GEORAM"},
/* nl */ {IDS_GEORAM_SIZE_NL, "GEORAM-grootte"},
/* pl */ {IDS_GEORAM_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_GEORAM_SIZE_SV, "GEORAM-storlek"},

/* en */ {IDS_GEORAM_FILENAME,    "GEORAM file"},
/* de */ {IDS_GEORAM_FILENAME_DE, "GEORAM Datei"},
/* fr */ {IDS_GEORAM_FILENAME_FR, "Fichier GEORAM"},
/* hu */ {IDS_GEORAM_FILENAME_HU, "GEORAM fájl"},
/* it */ {IDS_GEORAM_FILENAME_IT, "File GEORAM"},
/* nl */ {IDS_GEORAM_FILENAME_NL, "GEORAM-bestand"},
/* pl */ {IDS_GEORAM_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_GEORAM_FILENAME_SV, "GEORAM-fil"},

/* en */ {IDS_GEORAM_FILENAME_SELECT,    "Select file for GEORAM"},
/* de */ {IDS_GEORAM_FILENAME_SELECT_DE, "Datei für GEORAM selektieren"},
/* fr */ {IDS_GEORAM_FILENAME_SELECT_FR, "Sélectionner fichier pour GEORAM"},
/* hu */ {IDS_GEORAM_FILENAME_SELECT_HU, "Válasszon fájlt a GEORAM-hoz"},
/* it */ {IDS_GEORAM_FILENAME_SELECT_IT, "Seleziona il file per il GEORAM"},
/* nl */ {IDS_GEORAM_FILENAME_SELECT_NL, "Selecteer bestand voor GEORAM"},
/* pl */ {IDS_GEORAM_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_GEORAM_FILENAME_SELECT_SV, "Välj fil för GEORAM"},

/* en */ {IDS_GEORAM_SETTINGS,    "GEORAM Settings"},
/* de */ {IDS_GEORAM_SETTINGS_DE, "GEORAM Einstellungen"},
/* fr */ {IDS_GEORAM_SETTINGS_FR, "Parametres GEORAM"},
/* hu */ {IDS_GEORAM_SETTINGS_HU, "GEORAM beállításai"},
/* it */ {IDS_GEORAM_SETTINGS_IT, "Impostazioni GEORAM"},
/* nl */ {IDS_GEORAM_SETTINGS_NL, "GEORAM-instellingen"},
/* pl */ {IDS_GEORAM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_GEORAM_SETTINGS_SV, "GEORAM-inställningar"},

/* en */ {IDS_JOYSTICK_IN_PORT_1,    "Joystick in port #1"},
/* de */ {IDS_JOYSTICK_IN_PORT_1_DE, "Joystick in Port #1"},
/* fr */ {IDS_JOYSTICK_IN_PORT_1_FR, "Joystick dans le port #1"},
/* hu */ {IDS_JOYSTICK_IN_PORT_1_HU, "Botkormány az 1-es porton"},
/* it */ {IDS_JOYSTICK_IN_PORT_1_IT, "Joystick nella porta #1"},
/* nl */ {IDS_JOYSTICK_IN_PORT_1_NL, "Joystick in poort #1"},
/* pl */ {IDS_JOYSTICK_IN_PORT_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOYSTICK_IN_PORT_1_SV, "Joystick i spelport 1"},

/* en */ {IDS_JOYSTICK_IN_PORT_2,    "Joystick in port #2"},
/* de */ {IDS_JOYSTICK_IN_PORT_2_DE, "Joystick in Port #2"},
/* fr */ {IDS_JOYSTICK_IN_PORT_2_FR, "Joystick dans le port #2"},
/* hu */ {IDS_JOYSTICK_IN_PORT_2_HU, "Botkormány a 2-es porton"},
/* it */ {IDS_JOYSTICK_IN_PORT_2_IT, "Joystick nella porta #2"},
/* nl */ {IDS_JOYSTICK_IN_PORT_2_NL, "Joystick in poort #2"},
/* pl */ {IDS_JOYSTICK_IN_PORT_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOYSTICK_IN_PORT_2_SV, "Joystick i spelport 2"},

/* en */ {IDS_JOYSTICK_SETTINGS,    "Joystick Settings"},
/* de */ {IDS_JOYSTICK_SETTINGS_DE, "Joystick Einstellungen"},
/* fr */ {IDS_JOYSTICK_SETTINGS_FR, "Parametres des joysticks"},
/* hu */ {IDS_JOYSTICK_SETTINGS_HU, "Botkormány beállításai"},
/* it */ {IDS_JOYSTICK_SETTINGS_IT, "Impostazioni joystick"},
/* nl */ {IDS_JOYSTICK_SETTINGS_NL, "Joystickinstellingen"},
/* pl */ {IDS_JOYSTICK_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOYSTICK_SETTINGS_SV, "Inställningar för joystick"},

/* en */ {IDS_KEYPAD,    "Numpad"},
/* de */ {IDS_KEYPAD_DE, "Ziffernblock"},
/* fr */ {IDS_KEYPAD_FR, ""},  /* fuzzy */
/* hu */ {IDS_KEYPAD_HU, "Numerikus billentyûk"},
/* it */ {IDS_KEYPAD_IT, "Tastierino numerico"},
/* nl */ {IDS_KEYPAD_NL, "Numpad"},
/* pl */ {IDS_KEYPAD_PL, ""},  /* fuzzy */
/* sv */ {IDS_KEYPAD_SV, "Numreriskt tangentbord"},

/* en */ {IDS_JOY_PORT_0,    "Joy Port 0"},
/* de */ {IDS_JOY_PORT_0_DE, "Joystick Port 0"},
/* fr */ {IDS_JOY_PORT_0_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_PORT_0_HU, "Botkormány port 0"},
/* it */ {IDS_JOY_PORT_0_IT, "Joy Porta 0"},
/* nl */ {IDS_JOY_PORT_0_NL, "Joy Poort 0"},
/* pl */ {IDS_JOY_PORT_0_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_PORT_0_SV, "Joy port 0"},

/* en */ {IDS_JOY_PORT_1,    "Joy Port 1"},
/* de */ {IDS_JOY_PORT_1_DE, "Joystick Port 1"},
/* fr */ {IDS_JOY_PORT_1_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_PORT_1_HU, "Botkormány port 1"},
/* it */ {IDS_JOY_PORT_1_IT, "Joy Porta 1"},
/* nl */ {IDS_JOY_PORT_1_NL, "Joy Poort 1"},
/* pl */ {IDS_JOY_PORT_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_PORT_1_SV, "Joy Port 1"},

/* en */ {IDS_JOY_PORT_2,    "Joy Port 2"},
/* de */ {IDS_JOY_PORT_2_DE, "Joystick Port 2"},
/* fr */ {IDS_JOY_PORT_2_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_PORT_2_HU, "Botkormány port 2"},
/* it */ {IDS_JOY_PORT_2_IT, "Joy Porta 2"},
/* nl */ {IDS_JOY_PORT_2_NL, "Joy Poort 2"},
/* pl */ {IDS_JOY_PORT_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_PORT_2_SV, "Joy Port 2"},

/* en */ {IDS_JOY_PORT_3,    "Joy Port 3"},
/* de */ {IDS_JOY_PORT_3_DE, "Joystick Port 3"},
/* fr */ {IDS_JOY_PORT_3_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_PORT_3_HU, "Botkormány port 3"},
/* it */ {IDS_JOY_PORT_3_IT, "Joy Porta 3"},
/* nl */ {IDS_JOY_PORT_3_NL, "Joy Poort 3"},
/* pl */ {IDS_JOY_PORT_3_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_PORT_3_SV, "Joy Port 3"},

/* en */ {IDS_STOP_BLUE,    "Stop/Blue"},
/* de */ {IDS_STOP_BLUE_DE, "Stop/Blau"},
/* fr */ {IDS_STOP_BLUE_FR, ""},  /* fuzzy */
/* hu */ {IDS_STOP_BLUE_HU, "Leállít/Kék"},
/* it */ {IDS_STOP_BLUE_IT, "Stop/Blu"},
/* nl */ {IDS_STOP_BLUE_NL, "Stop/Blauw"},
/* pl */ {IDS_STOP_BLUE_PL, ""},  /* fuzzy */
/* sv */ {IDS_STOP_BLUE_SV, "Stop/Blå"},

/* en */ {IDS_SELECT_RED,    "Select/Red"},
/* de */ {IDS_SELECT_RED_DE, "Auswahl/Rot"},
/* fr */ {IDS_SELECT_RED_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_RED_HU, "Kiválaszt/Piros"},
/* it */ {IDS_SELECT_RED_IT, "Seleziona/Rosso"},
/* nl */ {IDS_SELECT_RED_NL, "Select/Rood"},
/* pl */ {IDS_SELECT_RED_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_RED_SV, "Select/Röd"},

/* en */ {IDS_REPEAT_YELLOW,    "Repeat/Yellow"},
/* de */ {IDS_REPEAT_YELLOW_DE, "Wiederhole/Gelb"},
/* fr */ {IDS_REPEAT_YELLOW_FR, ""},  /* fuzzy */
/* hu */ {IDS_REPEAT_YELLOW_HU, "Ismétel/Sárga"},
/* it */ {IDS_REPEAT_YELLOW_IT, "Ripeti/Giallo"},
/* nl */ {IDS_REPEAT_YELLOW_NL, "Repeat/Geel"},
/* pl */ {IDS_REPEAT_YELLOW_PL, ""},  /* fuzzy */
/* sv */ {IDS_REPEAT_YELLOW_SV, "Repeat/Gul"},

/* en */ {IDS_SHUFFLE_GREEN,    "Shuffle/Green"},
/* de */ {IDS_SHUFFLE_GREEN_DE, "Mische/Grün"},
/* fr */ {IDS_SHUFFLE_GREEN_FR, ""},  /* fuzzy */
/* hu */ {IDS_SHUFFLE_GREEN_HU, "Véletlenszerû/Zöld"},
/* it */ {IDS_SHUFFLE_GREEN_IT, "Mischia/Verde"},
/* nl */ {IDS_SHUFFLE_GREEN_NL, "Shuffle/Groen"},
/* pl */ {IDS_SHUFFLE_GREEN_PL, ""},  /* fuzzy */
/* sv */ {IDS_SHUFFLE_GREEN_SV, "Shuffle/Grön"},

/* en */ {IDS_FORWARD_CHARCOAL,    "Forward/Charcoal"},
/* de */ {IDS_FORWARD_CHARCOAL_DE, "Vorwärts/Kohle"},
/* fr */ {IDS_FORWARD_CHARCOAL_FR, ""},  /* fuzzy */
/* hu */ {IDS_FORWARD_CHARCOAL_HU, "Elõre/Fekete"},
/* it */ {IDS_FORWARD_CHARCOAL_IT, "Avanti/Carboncino"},
/* nl */ {IDS_FORWARD_CHARCOAL_NL, "Forward/Charcoal"},
/* pl */ {IDS_FORWARD_CHARCOAL_PL, ""},  /* fuzzy */
/* sv */ {IDS_FORWARD_CHARCOAL_SV, "Forward/Träkol"},

/* en */ {IDS_REVERSE_CHARCOAL,    "Reverse/Charcoal"},
/* de */ {IDS_REVERSE_CHARCOAL_DE, "Rückwärts/Kohle"},
/* fr */ {IDS_REVERSE_CHARCOAL_FR, ""},  /* fuzzy */
/* hu */ {IDS_REVERSE_CHARCOAL_HU, "Vissza/Fekete"},
/* it */ {IDS_REVERSE_CHARCOAL_IT, "Indietro/Carboncino"},
/* nl */ {IDS_REVERSE_CHARCOAL_NL, "Reverse/Charcoal"},
/* pl */ {IDS_REVERSE_CHARCOAL_PL, ""},  /* fuzzy */
/* sv */ {IDS_REVERSE_CHARCOAL_SV, "Reverse/Träkol"},

/* en */ {IDS_PLAY_PAUSE_GREY,    "Play-Pause/Grey"},
/* de */ {IDS_PLAY_PAUSE_GREY_DE, "Play-Pause/Grau"},
/* fr */ {IDS_PLAY_PAUSE_GREY_FR, ""},  /* fuzzy */
/* hu */ {IDS_PLAY_PAUSE_GREY_HU, "Lejátszás-Megállítás/Szürke"},
/* it */ {IDS_PLAY_PAUSE_GREY_IT, "Avvia-Pausa/Grigio"},
/* nl */ {IDS_PLAY_PAUSE_GREY_NL, "Play-Pause/Grijs"},
/* pl */ {IDS_PLAY_PAUSE_GREY_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLAY_PAUSE_GREY_SV, "[Play/Pause]/Grå"},

/* en */ {IDS_JOY_1_DEVICE,    "Joy 1 Device"},
/* de */ {IDS_JOY_1_DEVICE_DE, "Joystick 1 Gerät"},
/* fr */ {IDS_JOY_1_DEVICE_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_1_DEVICE_HU, "1-es botkormány eszköz"},
/* it */ {IDS_JOY_1_DEVICE_IT, "Dispositivo Joy 1"},
/* nl */ {IDS_JOY_1_DEVICE_NL, "Joy 1-apparaat"},
/* pl */ {IDS_JOY_1_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_1_DEVICE_SV, "Joy 1-enhet"},

/* en */ {IDS_JOY_2_DEVICE,    "Joy 2 Device"},
/* de */ {IDS_JOY_2_DEVICE_DE, "Joystick 2 Gerät"},
/* fr */ {IDS_JOY_2_DEVICE_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_2_DEVICE_HU, "2-es botkormány eszköz"},
/* it */ {IDS_JOY_2_DEVICE_IT, "Dispositivo Joy 2"},
/* nl */ {IDS_JOY_2_DEVICE_NL, "Joy 2-apparaat"},
/* pl */ {IDS_JOY_2_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_2_DEVICE_SV, "Joy 2-enhet"},

/* en */ {IDS_JOY_1_FIRE,    "Joy 1 Fire"},
/* de */ {IDS_JOY_1_FIRE_DE, "Joystick 1 Feuer"},
/* fr */ {IDS_JOY_1_FIRE_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_1_FIRE_HU, "1-es botkormány tûz"},
/* it */ {IDS_JOY_1_FIRE_IT, "Fuoco Joy 1"},
/* nl */ {IDS_JOY_1_FIRE_NL, "Joy 1 Vuur"},
/* pl */ {IDS_JOY_1_FIRE_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_1_FIRE_SV, "Joy 1 Fire"},

/* en */ {IDS_JOY_2_FIRE,    "Joy 2 Fire"},
/* de */ {IDS_JOY_2_FIRE_DE, "Joystick 2 Feuer"},
/* fr */ {IDS_JOY_2_FIRE_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_2_FIRE_HU, "2-es botkormány tûz"},
/* it */ {IDS_JOY_2_FIRE_IT, "Fuoco Joy 2"},
/* nl */ {IDS_JOY_2_FIRE_NL, "Joy 1 Vuur"},
/* pl */ {IDS_JOY_2_FIRE_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_2_FIRE_SV, "Joy 2 Fire"},

/* en */ {IDS_SET_INPUT_JOYSTICK_1,    "Joystick #1 emulation: (0:None, 1: Numpad, 2: Keyset A/AI, 3: Keyset B/AI)"},
/* de */ {IDS_SET_INPUT_JOYSTICK_1_DE, "Joystick #1 Emulation: (0:Keine, 1: Numpad, 2: Keysatz A/AI, 3: Keysatz B/AI)"},
/* fr */ {IDS_SET_INPUT_JOYSTICK_1_FR, ""},  /* fuzzy */
/* hu */ {IDS_SET_INPUT_JOYSTICK_1_HU, "#1-es botkormány emuláció: (0: Nincs, 1: numerikus, 2: 'A' gombkészlet, 3: 'B' gombkészlet)"},
/* it */ {IDS_SET_INPUT_JOYSTICK_1_IT, "Emulazione Joystick #1: (0:Nessuna, 1: Tastierino numerico, 2: Keyset A/AI, 3: Keyset B/AI)"},
/* nl */ {IDS_SET_INPUT_JOYSTICK_1_NL, "Emulatie joystick #1: (0: Geen, 1: Numpad, 2: Keyset A/AI, 3: Keyset B/AI)"},
/* pl */ {IDS_SET_INPUT_JOYSTICK_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_SET_INPUT_JOYSTICK_1_SV, "Joystick #1 emulation: (0:Ingen, 1: Numeriskt tangentbord, 2: Teckenuppsättning A/AI, 3: Teckenuppsättning B/AI)" },

/* en */ {IDS_SET_INPUT_JOYSTICK_2,    "Joystick #2 emulation: (0: None, 1: Numpad, 2: Keyset A/AI, 3: Keyset B/AI)"},
/* de */ {IDS_SET_INPUT_JOYSTICK_2_DE, "Joystick #2 Emulation: (0:Keine, 1: Numpad, 2: Keysatz A/AI, 3: Keysatz B/AI)"},
/* fr */ {IDS_SET_INPUT_JOYSTICK_2_FR, ""},  /* fuzzy */
/* hu */ {IDS_SET_INPUT_JOYSTICK_2_HU, "#2-es botkormány emuláció: (0: Nincs, 1: numerikus, 2: 'A' gombkészlet, 3: 'B' gombkészlet)"},
/* it */ {IDS_SET_INPUT_JOYSTICK_2_IT, "Emulazione Joystick #2: (0:Nessuna, 1: Tastierino numerico, 2: Keyset A/AI, 3: Keyset B/AI)"},
/* nl */ {IDS_SET_INPUT_JOYSTICK_2_NL, "Emulatie joystick #2: (0: Geen, 1: Numpad, 2: Keyset A/AI, 3: Keyset B/AI)"},
/* pl */ {IDS_SET_INPUT_JOYSTICK_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_SET_INPUT_JOYSTICK_2_SV, "Joystick #2 emulation: (0:Ingen, 1: Numeriskt tangentbord, 2: Teckenuppsättning A/AI, 3: Teckenuppsättning B/AI)" },

/* en */ {IDS_SET_INPUT_JOYLL_1,    "Joystick #1 emulation: (0: None, 1: Numpad, 2-5: Amiga Joy 0-3"},
/* de */ {IDS_SET_INPUT_JOYLL_1_DE, "Joystick #1 Emulation: (0: Keine, 1: Numpad, 2-5: Amiga Joystick 0-3"},
/* fr */ {IDS_SET_INPUT_JOYLL_1_FR, ""},  /* fuzzy */
/* hu */ {IDS_SET_INPUT_JOYLL_1_HU, "#1-es botkormány emuláció: (0: nincs, 1: numerikus billentyûk, 2-5: amiga botkormány 0-3"},
/* it */ {IDS_SET_INPUT_JOYLL_1_IT, "Emulazione Joystick #1: (0: Nessuna, 1: Tastierino numerico, 2-5: Joy Amiga 0-3"},
/* nl */ {IDS_SET_INPUT_JOYLL_1_NL, "Emulatie joystick #1: (0: Geen, 1: Numpad, 2-5: Amiga Joy 0-3"},
/* pl */ {IDS_SET_INPUT_JOYLL_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_SET_INPUT_JOYLL_1_SV, "Joystick #1 emulation: (0:Ingen, 1: Numeriskt tangentbord, 2-5: Amiga Joy 0-3"},

/* en */ {IDS_SET_INPUT_JOYLL_2,    "Joystick #2 emulation: (0: None, 1: Numpad, 2-5: Amiga Joy 0-3"},
/* de */ {IDS_SET_INPUT_JOYLL_2_DE, "Joystick #2 Emulation: (0: Keine, 1: Numpad, 2-5: Amiga Joystick 0-3"},
/* fr */ {IDS_SET_INPUT_JOYLL_2_FR, ""},  /* fuzzy */
/* hu */ {IDS_SET_INPUT_JOYLL_2_HU, "#2-es botkormány emuláció: (0: nincs, 1: numerikus billentyûk, 2-5: amiga botkormány 0-3"},
/* it */ {IDS_SET_INPUT_JOYLL_2_IT, "Emulazione Joystick #2: (0: Nessuna, 1: Tastierino numerico, 2-5: Joy Amiga 0-3"},
/* nl */ {IDS_SET_INPUT_JOYLL_2_NL, "Emulatie Joystick #2: (0: Geen, 1: Numpad, 2-5: Amiga Joy 0-3"},
/* pl */ {IDS_SET_INPUT_JOYLL_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_SET_INPUT_JOYLL_2_SV, "Joystick #2 emulation: (0:Ingen, 1: Numeriskt tangentbord, 2-5: Amiga Joy 0-3"},

/* en */ {IDS_SAVE_SETTINGS_ON_EXIT,    "Save settings (resources) on exit"},
/* de */ {IDS_SAVE_SETTINGS_ON_EXIT_DE, "Einstellungen (Ressourcen) beim Beenden speichern"},
/* fr */ {IDS_SAVE_SETTINGS_ON_EXIT_FR, "Sauvegarder les parametres   la sortie"},
/* hu */ {IDS_SAVE_SETTINGS_ON_EXIT_HU, "Beállítások (erõforrások) mentése kilépéskor"},
/* it */ {IDS_SAVE_SETTINGS_ON_EXIT_IT, "Salva le impostazioni in uscita"},
/* nl */ {IDS_SAVE_SETTINGS_ON_EXIT_NL, "Instellingen (resources) opslaan bij afsluiten"},
/* pl */ {IDS_SAVE_SETTINGS_ON_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_SETTINGS_ON_EXIT_SV, "Spara inställningar (resurser) vid avslut"},

/* en */ {IDS_NEVER_SAVE_SETTINGS_EXIT,    "Never save settings (resources) on exit"},
/* de */ {IDS_NEVER_SAVE_SETTINGS_EXIT_DE, "Einstellungen beim Beenden nie speichern"},
/* fr */ {IDS_NEVER_SAVE_SETTINGS_EXIT_FR, "Ne jamais enregistrer les parametres   la sortie"},
/* hu */ {IDS_NEVER_SAVE_SETTINGS_EXIT_HU, "Sose mentse a beállításokat (erõforrásokat) kilépéskor"},
/* it */ {IDS_NEVER_SAVE_SETTINGS_EXIT_IT, "Non salvare mai le impostazioni in uscita"},
/* nl */ {IDS_NEVER_SAVE_SETTINGS_EXIT_NL, "Instellingen (resources) niet opslaan bij afsluiten"},
/* pl */ {IDS_NEVER_SAVE_SETTINGS_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEVER_SAVE_SETTINGS_EXIT_SV, "Spara aldrig inställningar (resurser) vid avslut"},

/* en */ {IDS_CONFIRM_QUITING_VICE,    "Confirm quiting VICE"},
/* de */ {IDS_CONFIRM_QUITING_VICE_DE, "Bestätige beenden von VICE"},
/* fr */ {IDS_CONFIRM_QUITING_VICE_FR, "Demander une confirmation avant de quitter VICE"},
/* hu */ {IDS_CONFIRM_QUITING_VICE_HU, "Megerõsítés kilépés elõtt"},
/* it */ {IDS_CONFIRM_QUITING_VICE_IT, "Conferma l'uscita da VICE"},
/* nl */ {IDS_CONFIRM_QUITING_VICE_NL, "Bevestigen van het aflsuiten van VICE"},
/* pl */ {IDS_CONFIRM_QUITING_VICE_PL, ""},  /* fuzzy */
/* sv */ {IDS_CONFIRM_QUITING_VICE_SV, "Bekräfta för att avsluta VICE"},

/* en */ {IDS_NEVER_CONFIRM_QUITING_VICE,    "Never confirm quiting VICE"},
/* de */ {IDS_NEVER_CONFIRM_QUITING_VICE_DE, "Keine Bestätigung beim Beenden von VICE"},
/* fr */ {IDS_NEVER_CONFIRM_QUITING_VICE_FR, "Ne pas demander de confirmation pour quitter VICE"},
/* hu */ {IDS_NEVER_CONFIRM_QUITING_VICE_HU, "Sosem kér megerõsítést kilépés elõtt"},
/* it */ {IDS_NEVER_CONFIRM_QUITING_VICE_IT, "Non confermare mai l'uscita da VICE"},
/* nl */ {IDS_NEVER_CONFIRM_QUITING_VICE_NL, "Niet bevestigen van het afsluiten van VICE"},
/* pl */ {IDS_NEVER_CONFIRM_QUITING_VICE_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEVER_CONFIRM_QUITING_VICE_SV, "Bekräfta inte för att avsluta VICE"},

/* en */ {IDS_PET_REU_SIZE,    "PET REU Size"},
/* de */ {IDS_PET_REU_SIZE_DE, "PET REU Größe"},
/* fr */ {IDS_PET_REU_SIZE_FR, "Taille du PET REU"},
/* hu */ {IDS_PET_REU_SIZE_HU, "PET REU mérete"},
/* it */ {IDS_PET_REU_SIZE_IT, "Dimensione PET REU"},
/* nl */ {IDS_PET_REU_SIZE_NL, "PET REU-grootte"},
/* pl */ {IDS_PET_REU_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_PET_REU_SIZE_SV, "PET REU-storlek"},

/* en */ {IDS_PET_REU_FILENAME,    "PET REU file"},
/* de */ {IDS_PET_REU_FILENAME_DE, "PET REU Datei"},
/* fr */ {IDS_PET_REU_FILENAME_FR, "Fichier PET REU"},
/* hu */ {IDS_PET_REU_FILENAME_HU, "PET REU fájl"},
/* it */ {IDS_PET_REU_FILENAME_IT, "File PET REU"},
/* nl */ {IDS_PET_REU_FILENAME_NL, "PET REU-bestand"},
/* pl */ {IDS_PET_REU_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_PET_REU_FILENAME_SV, "PET REU-fil"},

/* en */ {IDS_PET_REU_FILENAME_SELECT,    "Select file for PET REU"},
/* de */ {IDS_PET_REU_FILENAME_SELECT_DE, "Datei für PET REU selektieren"},
/* fr */ {IDS_PET_REU_FILENAME_SELECT_FR, "Sélectionner fichier pour PET REU"},
/* hu */ {IDS_PET_REU_FILENAME_SELECT_HU, "Válasszon fájlt a PET REU-hoz"},
/* it */ {IDS_PET_REU_FILENAME_SELECT_IT, "Seleziona il file per il PET REU"},
/* nl */ {IDS_PET_REU_FILENAME_SELECT_NL, "Selecteer bestand voor PET REU"},
/* pl */ {IDS_PET_REU_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_PET_REU_FILENAME_SELECT_SV, "Välj fil för PET REU"},

/* en */ {IDS_PET_REU_SETTINGS,    "PET REU Settings"},
/* de */ {IDS_PET_REU_SETTINGS_DE, "PET REU Einstellungen"},
/* fr */ {IDS_PET_REU_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_PET_REU_SETTINGS_HU, "PET REU beállításai"},
/* it */ {IDS_PET_REU_SETTINGS_IT, "Impostazioni PET REU"},
/* nl */ {IDS_PET_REU_SETTINGS_NL, "PET REU-instellingen"},
/* pl */ {IDS_PET_REU_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_PET_REU_SETTINGS_SV, "PET REU-Inställningar"},

/* en */ {IDS_PLUS256K_SETTINGS,    "PLUS256K Settings"},
/* de */ {IDS_PLUS256K_SETTINGS_DE, "PLUS256K Einstellungen"},
/* fr */ {IDS_PLUS256K_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_PLUS256K_SETTINGS_HU, "PLUS256K beállításai"},
/* it */ {IDS_PLUS256K_SETTINGS_IT, "Impostazioni PLUS256K"},
/* nl */ {IDS_PLUS256K_SETTINGS_NL, "PLUS256K-instellingen"},
/* pl */ {IDS_PLUS256K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS256K_SETTINGS_SV, "PLUS256K-inställningar"},

/* en */ {IDS_PLUS60K_BASE,    "PLUS60K Base"},
/* de */ {IDS_PLUS60K_BASE_DE, "PLUS60K Basis Adresse"},
/* fr */ {IDS_PLUS60K_BASE_FR, ""},  /* fuzzy */
/* hu */ {IDS_PLUS60K_BASE_HU, "PLUS60K báziscíme"},
/* it */ {IDS_PLUS60K_BASE_IT, "Indirizzo base PLUS60K"},
/* nl */ {IDS_PLUS60K_BASE_NL, "PLUS60K-basisadres"},
/* pl */ {IDS_PLUS60K_BASE_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS60K_BASE_SV, "PLUS60K bas-adress"},

/* en */ {IDS_PLUS60K_FILENAME,    "PLUS60K file"},
/* de */ {IDS_PLUS60K_FILENAME_DE, "PLUS60K Datei"},
/* fr */ {IDS_PLUS60K_FILENAME_FR, "Fichier PLUS60K"},
/* hu */ {IDS_PLUS60K_FILENAME_HU, "PLUS60K fájl"},
/* it */ {IDS_PLUS60K_FILENAME_IT, "File PLUS60K"},
/* nl */ {IDS_PLUS60K_FILENAME_NL, "PLUS60K-bestand"},
/* pl */ {IDS_PLUS60K_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS60K_FILENAME_SV, "PLUS60K-fil"},

/* en */ {IDS_PLUS60K_FILENAME_SELECT,    "Select file for PLUS60K"},
/* de */ {IDS_PLUS60K_FILENAME_SELECT_DE, "Datei für PLUS60K selektieren"},
/* fr */ {IDS_PLUS60K_FILENAME_SELECT_FR, "Sélectionner fichier pour PLUS60K"},
/* hu */ {IDS_PLUS60K_FILENAME_SELECT_HU, "Válasszon fájlt a PLUS60K-hoz"},
/* it */ {IDS_PLUS60K_FILENAME_SELECT_IT, "Seleziona file per PLUS60K"},
/* nl */ {IDS_PLUS60K_FILENAME_SELECT_NL, "Selecteer bestand voor PLUS60K"},
/* pl */ {IDS_PLUS60K_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS60K_FILENAME_SELECT_SV, "Ange fil för PLUS60K"},

/* en */ {IDS_PLUS60K_SETTINGS,    "PLUS60K Settings"},
/* de */ {IDS_PLUS60K_SETTINGS_DE, "PLUS60K Einstellungen"},
/* fr */ {IDS_PLUS60K_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_PLUS60K_SETTINGS_HU, "PLUS60K beállításai"},
/* it */ {IDS_PLUS60K_SETTINGS_IT, "Impostazioni PLUS60K"},
/* nl */ {IDS_PLUS60K_SETTINGS_NL, "PLUS60K-instellingen"},
/* pl */ {IDS_PLUS60K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS60K_SETTINGS_SV, "PLUS60K-inställningar"},

/* en */ {IDS_VALUE_FIRST_BYTE,    "Value of first byte"},
/* de */ {IDS_VALUE_FIRST_BYTE_DE, "Wert erstes Byte"},
/* fr */ {IDS_VALUE_FIRST_BYTE_FR, "Valeur du premier octet"},
/* hu */ {IDS_VALUE_FIRST_BYTE_HU, "Elsõ bájt értéke"},
/* it */ {IDS_VALUE_FIRST_BYTE_IT, "Valore del primo byte"},
/* nl */ {IDS_VALUE_FIRST_BYTE_NL, "Waarde van eerste byte"},
/* pl */ {IDS_VALUE_FIRST_BYTE_PL, ""},  /* fuzzy */
/* sv */ {IDS_VALUE_FIRST_BYTE_SV, "Värde pa första byte"},

/* en */ {IDS_LENGTH_CONSTANT_VALUES,    "Length of constant values"},
/* de */ {IDS_LENGTH_CONSTANT_VALUES_DE, "Länge der konstaten Werte"},
/* fr */ {IDS_LENGTH_CONSTANT_VALUES_FR, "Longeur des valeurs constantes"},
/* hu */ {IDS_LENGTH_CONSTANT_VALUES_HU, "Állandó értékek hossza"},
/* it */ {IDS_LENGTH_CONSTANT_VALUES_IT, "Numero di valori costanti"},
/* nl */ {IDS_LENGTH_CONSTANT_VALUES_NL, "Lengte van constante waarden"},
/* pl */ {IDS_LENGTH_CONSTANT_VALUES_PL, ""},  /* fuzzy */
/* sv */ {IDS_LENGTH_CONSTANT_VALUES_SV, "Längd pa konstanta värden"},

/* en */ {IDS_LENGTH_CONSTANT_PATTERN,    "Length of constant pattern"},
/* de */ {IDS_LENGTH_CONSTANT_PATTERN_DE, "Länge des konstanten Musters"},
/* fr */ {IDS_LENGTH_CONSTANT_PATTERN_FR, "Longeur des contantes pattern"},
/* hu */ {IDS_LENGTH_CONSTANT_PATTERN_HU, "Állandó minta hossza"},
/* it */ {IDS_LENGTH_CONSTANT_PATTERN_IT, "Dimensione del pattern costante"},
/* nl */ {IDS_LENGTH_CONSTANT_PATTERN_NL, "Lengte van constant patroon"},
/* pl */ {IDS_LENGTH_CONSTANT_PATTERN_PL, ""},  /* fuzzy */
/* sv */ {IDS_LENGTH_CONSTANT_PATTERN_SV, "Längd pa konstant mönster"},

/* en */ {IDS_RAM_SETTINGS,    "RAM Settings"},
/* de */ {IDS_RAM_SETTINGS_DE, "RAM Einstellungen"},
/* fr */ {IDS_RAM_SETTINGS_FR, "Parametres de la RAM"},
/* hu */ {IDS_RAM_SETTINGS_HU, "RAM beállításai"},
/* it */ {IDS_RAM_SETTINGS_IT, "Impostazioni RAM"},
/* nl */ {IDS_RAM_SETTINGS_NL, "RAM-instellingen"},
/* pl */ {IDS_RAM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_SETTINGS_SV, "RAM-inställningar"},

/* en */ {IDS_READ_WRITE,    "read/write"},
/* de */ {IDS_READ_WRITE_DE, "lese/schreibe"},
/* fr */ {IDS_READ_WRITE_FR, ""},  /* fuzzy */
/* hu */ {IDS_READ_WRITE_HU, "írható/olvasható"},
/* it */ {IDS_READ_WRITE_IT, "lettura/scrittura"},
/* nl */ {IDS_READ_WRITE_NL, "lezen/schrijven"},
/* pl */ {IDS_READ_WRITE_PL, ""},  /* fuzzy */
/* sv */ {IDS_READ_WRITE_SV, "läs/skriv"},

/* en */ {IDS_READ_ONLY,    "read-only"},
/* de */ {IDS_READ_ONLY_DE, "Schreibgschutz"},
/* fr */ {IDS_READ_ONLY_FR, "Lecture seule"},
/* hu */ {IDS_READ_ONLY_HU, "csak olvasható"},
/* it */ {IDS_READ_ONLY_IT, "a sola lettura"},
/* nl */ {IDS_READ_ONLY_NL, "alleen lezen"},
/* pl */ {IDS_READ_ONLY_PL, ""},  /* fuzzy */
/* sv */ {IDS_READ_ONLY_SV, "Endast läsatkomst"},

/* en */ {IDS_RAMCART_READ_WRITE,    "RAMCART Read/Write"},
/* de */ {IDS_RAMCART_READ_WRITE_DE, "RAMCART Lesen/Schreiben"},
/* fr */ {IDS_RAMCART_READ_WRITE_FR, ""},  /* fuzzy */
/* hu */ {IDS_RAMCART_READ_WRITE_HU, "RAMCART írható/olvasható"},
/* it */ {IDS_RAMCART_READ_WRITE_IT, "Lettura/Scrittura RAMCART"},
/* nl */ {IDS_RAMCART_READ_WRITE_NL, "RAMCART Lezen/Schrijven"},
/* pl */ {IDS_RAMCART_READ_WRITE_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAMCART_READ_WRITE_SV, "RAMCART läs/skriv"},

/* en */ {IDS_RAMCART_SIZE,    "RAMCART Size"},
/* de */ {IDS_RAMCART_SIZE_DE, "RAMCART Größe"},
/* fr */ {IDS_RAMCART_SIZE_FR, "Taille du RAMCART"},
/* hu */ {IDS_RAMCART_SIZE_HU, "RAMCART mérete"},
/* it */ {IDS_RAMCART_SIZE_IT, "Dimensione RAMCART"},
/* nl */ {IDS_RAMCART_SIZE_NL, "RAMCART-grootte"},
/* pl */ {IDS_RAMCART_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAMCART_SIZE_SV, "Storlek pa RAMCART"},

/* en */ {IDS_RAMCART_FILENAME,    "RAMCART file"},
/* de */ {IDS_RAMCART_FILENAME_DE, "RAMCART Datei"},
/* fr */ {IDS_RAMCART_FILENAME_FR, "Fichier RAMCART"},
/* hu */ {IDS_RAMCART_FILENAME_HU, "RAMCART fájl"},
/* it */ {IDS_RAMCART_FILENAME_IT, "File RAMCART"},
/* nl */ {IDS_RAMCART_FILENAME_NL, "RAMCART-bestand"},
/* pl */ {IDS_RAMCART_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAMCART_FILENAME_SV, "RAMCART-fil"},

/* en */ {IDS_RAMCART_FILENAME_SELECT,    "Select file for RAMCART"},
/* de */ {IDS_RAMCART_FILENAME_SELECT_DE, "Datei für RAMCART selektieren"},
/* fr */ {IDS_RAMCART_FILENAME_SELECT_FR, "Sélectionner fichier pour RAMCART"},
/* hu */ {IDS_RAMCART_FILENAME_SELECT_HU, "Válasszon fájlt a RAMCART-hoz"},
/* it */ {IDS_RAMCART_FILENAME_SELECT_IT, "Seleziona il file per il RAMCART"},
/* nl */ {IDS_RAMCART_FILENAME_SELECT_NL, "Selecteer bestand voor RAMCART"},
/* pl */ {IDS_RAMCART_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAMCART_FILENAME_SELECT_SV, "Välj fil för RAMCART"},

/* en */ {IDS_RAMCART_SETTINGS,    "RAMCART Settings"},
/* de */ {IDS_RAMCART_SETTINGS_DE, "RAMCART Einstellungen"},
/* fr */ {IDS_RAMCART_SETTINGS_FR, "Parametres RAMCART"},
/* hu */ {IDS_RAMCART_SETTINGS_HU, "RAMCART beállításai"},
/* it */ {IDS_RAMCART_SETTINGS_IT, "Impostazioni RAMCART"},
/* nl */ {IDS_RAMCART_SETTINGS_NL, "RAMCART-instellingen"},
/* pl */ {IDS_RAMCART_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAMCART_SETTINGS_SV, "RAMCART-inställningar"},

/* en */ {IDS_REU_SIZE,    "REU Size"},
/* de */ {IDS_REU_SIZE_DE, "REU Größe"},
/* fr */ {IDS_REU_SIZE_FR, "Taille du REU"},
/* hu */ {IDS_REU_SIZE_HU, "REU mérete"},
/* it */ {IDS_REU_SIZE_IT, "Dimensione REU"},
/* nl */ {IDS_REU_SIZE_NL, "REU-grootte"},
/* pl */ {IDS_REU_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_REU_SIZE_SV, "Storlek pa minnesexpansion"},

/* en */ {IDS_REU_FILENAME,    "REU file"},
/* de */ {IDS_REU_FILENAME_DE, "REU Datei"},
/* fr */ {IDS_REU_FILENAME_FR, "Fichier REU"},
/* hu */ {IDS_REU_FILENAME_HU, "REU fájl"},
/* it */ {IDS_REU_FILENAME_IT, "File REU"},
/* nl */ {IDS_REU_FILENAME_NL, "REU-bestand"},
/* pl */ {IDS_REU_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_REU_FILENAME_SV, "REU-fil"},

/* en */ {IDS_REU_FILENAME_SELECT,    "Select file for REU"},
/* de */ {IDS_REU_FILENAME_SELECT_DE, "Datei für REU selektieren"},
/* fr */ {IDS_REU_FILENAME_SELECT_FR, "Sélectionner fichier pour REU"},
/* hu */ {IDS_REU_FILENAME_SELECT_HU, "Válasszon fájlt a REU-hoz"},
/* it */ {IDS_REU_FILENAME_SELECT_IT, "Seleziona il file per il REU"},
/* nl */ {IDS_REU_FILENAME_SELECT_NL, "Selecteer bestand voor REU"},
/* pl */ {IDS_REU_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_REU_FILENAME_SELECT_SV, "Välj fil för REU"},

/* en */ {IDS_REU_SETTINGS,    "REU Settings"},
/* de */ {IDS_REU_SETTINGS_DE, "REU Einstellungen"},
/* fr */ {IDS_REU_SETTINGS_FR, "Parametres REU"},
/* hu */ {IDS_REU_SETTINGS_HU, "REU beállításai"},
/* it */ {IDS_REU_SETTINGS_IT, "Impostazioni REU"},
/* nl */ {IDS_REU_SETTINGS_NL, "REU-instellingen"},
/* pl */ {IDS_REU_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_REU_SETTINGS_SV, "REU-Inställningar"},

/* en */ {IDS_USERPORT_RS232,    "Userport RS232"},
/* de */ {IDS_USERPORT_RS232_DE, "Userport RS232"},
/* fr */ {IDS_USERPORT_RS232_FR, "Port utilisateur RS232"},
/* hu */ {IDS_USERPORT_RS232_HU, "Userport RS232"},
/* it */ {IDS_USERPORT_RS232_IT, "Userport RS232"},
/* nl */ {IDS_USERPORT_RS232_NL, "Userport RS232"},
/* pl */ {IDS_USERPORT_RS232_PL, ""},  /* fuzzy */
/* sv */ {IDS_USERPORT_RS232_SV, "Användarportens RS232"},

/* en */ {IDS_USERPORT_DEVICE,    "Userport RS232 Device"},
/* de */ {IDS_USERPORT_DEVICE_DE, "Userport RS232 Gerät"},
/* fr */ {IDS_USERPORT_DEVICE_FR, "Périphérique de port utilisateur RS232"},
/* hu */ {IDS_USERPORT_DEVICE_HU, "Userport RS232 eszköz"},
/* it */ {IDS_USERPORT_DEVICE_IT, "Dispositivo Userport RS232"},
/* nl */ {IDS_USERPORT_DEVICE_NL, "Userport RS232-apparaat"},
/* pl */ {IDS_USERPORT_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDS_USERPORT_DEVICE_SV, "Användarportens RS232-enhet"},

/* en */ {IDS_USERPORT_BAUD_RATE,    "Userport RS232 baud rate"},
/* de */ {IDS_USERPORT_BAUD_RATE_DE, "Userport RS232 Baud Rate"},
/* fr */ {IDS_USERPORT_BAUD_RATE_FR, "Périphérique de port utilisateur RS232"},
/* hu */ {IDS_USERPORT_BAUD_RATE_HU, "Userport RS232 átviteli ráta"},
/* it */ {IDS_USERPORT_BAUD_RATE_IT, "Velocità della RS232 su Userport"},
/* nl */ {IDS_USERPORT_BAUD_RATE_NL, "Userport RS232 baudrate"},
/* pl */ {IDS_USERPORT_BAUD_RATE_PL, ""},  /* fuzzy */
/* sv */ {IDS_USERPORT_BAUD_RATE_SV, "Användarportens RS232-baudhastighet"},

/* en */ {IDS_RS232_USERPORT_SETTINGS,    "RS232 Userport Settings"},
/* de */ {IDS_RS232_USERPORT_SETTINGS_DE, "RS232 Userport Einstellungen"},
/* fr */ {IDS_RS232_USERPORT_SETTINGS_FR, "Parametres RS232 Userport"},
/* hu */ {IDS_RS232_USERPORT_SETTINGS_HU, "RS232 Userport beállításai"},
/* it */ {IDS_RS232_USERPORT_SETTINGS_IT, "Impostazioni RS232 su userport"},
/* nl */ {IDS_RS232_USERPORT_SETTINGS_NL, "RS232 Userport-instellingen"},
/* pl */ {IDS_RS232_USERPORT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_RS232_USERPORT_SETTINGS_SV, "RS232-användarportinställningar"},

/* en */ {IDS_FAST,    "fast"},
/* de */ {IDS_FAST_DE, "Schnell"},
/* fr */ {IDS_FAST_FR, "rapide"},
/* hu */ {IDS_FAST_HU, "gyors"},
/* it */ {IDS_FAST_IT, "veloce"},
/* nl */ {IDS_FAST_NL, "snel"},
/* pl */ {IDS_FAST_PL, ""},  /* fuzzy */
/* sv */ {IDS_FAST_SV, "snabbt"},

/* en */ {IDS_INTERPOLATING,    "interpolating"},
/* de */ {IDS_INTERPOLATING_DE, "Interpolierend"},
/* fr */ {IDS_INTERPOLATING_FR, "par interpolation"},
/* hu */ {IDS_INTERPOLATING_HU, "interpoláló"},
/* it */ {IDS_INTERPOLATING_IT, "Interpolazione"},
/* nl */ {IDS_INTERPOLATING_NL, "interpolating"},
/* pl */ {IDS_INTERPOLATING_PL, ""},  /* fuzzy */
/* sv */ {IDS_INTERPOLATING_SV, "interpolerande"},

/* en */ {IDS_RESAMPLING,    "resampling"},
/* de */ {IDS_RESAMPLING_DE, "Resampling"},
/* fr */ {IDS_RESAMPLING_FR, "rééchantillonage"},
/* hu */ {IDS_RESAMPLING_HU, "újramintavételezõ"},
/* it */ {IDS_RESAMPLING_IT, "ricampionamento"},
/* nl */ {IDS_RESAMPLING_NL, "resampling"},
/* pl */ {IDS_RESAMPLING_PL, ""},  /* fuzzy */
/* sv */ {IDS_RESAMPLING_SV, "omsamplande"},

/* en */ {IDS_FAST_RESAMPLING,    "fast resampling"},
/* de */ {IDS_FAST_RESAMPLING_DE, "Schnelles Resampling"},
/* fr */ {IDS_FAST_RESAMPLING_FR, "rééchantillonage rapide"},
/* hu */ {IDS_FAST_RESAMPLING_HU, "gyors újramintavételezõ"},
/* it */ {IDS_FAST_RESAMPLING_IT, "ricampionamento veloce"},
/* nl */ {IDS_FAST_RESAMPLING_NL, "snelle resampling"},
/* pl */ {IDS_FAST_RESAMPLING_PL, ""},  /* fuzzy */
/* sv */ {IDS_FAST_RESAMPLING_SV, "snabb omsampling"},

/* en */ {IDS_6581_OLD,    "6581 (old)"},
/* de */ {IDS_6581_OLD_DE, "6581 (alt)"},
/* fr */ {IDS_6581_OLD_FR, "6581 (ancien)"},
/* hu */ {IDS_6581_OLD_HU, "6581 (régi)"},
/* it */ {IDS_6581_OLD_IT, "6581 (vecchio)"},
/* nl */ {IDS_6581_OLD_NL, "6581 (oud)"},
/* pl */ {IDS_6581_OLD_PL, ""},  /* fuzzy */
/* sv */ {IDS_6581_OLD_SV, "6581 (gammal)"},

/* en */ {IDS_8580_NEW,    "8580 (new)"},
/* de */ {IDS_8580_NEW_DE, "8580 (neu)"},
/* fr */ {IDS_8580_NEW_FR, "8580 (nouveau)"},
/* hu */ {IDS_8580_NEW_HU, "8580 (új)"},
/* it */ {IDS_8580_NEW_IT, "8580 (nuovo)"},
/* nl */ {IDS_8580_NEW_NL, "8580 (nieuw)"},
/* pl */ {IDS_8580_NEW_PL, ""},  /* fuzzy */
/* sv */ {IDS_8580_NEW_SV, "8580 (ny)"},

/* en */ {IDS_SID_MODEL,    "SID Model"},
/* de */ {IDS_SID_MODEL_DE, "SID Modell"},
/* fr */ {IDS_SID_MODEL_FR, "Modele SID"},
/* hu */ {IDS_SID_MODEL_HU, "SID típus"},
/* it */ {IDS_SID_MODEL_IT, "Modello SID"},
/* nl */ {IDS_SID_MODEL_NL, "SID-model"},
/* pl */ {IDS_SID_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_MODEL_SV, "SID-modell"},

/* en */ {IDS_SID_STEREO,    "SID Stereo"},
/* de */ {IDS_SID_STEREO_DE, "SID Stereo"},
/* fr */ {IDS_SID_STEREO_FR, "SID Stéréo"},
/* hu */ {IDS_SID_STEREO_HU, "SID sztereó"},
/* it */ {IDS_SID_STEREO_IT, "SID Stereo"},
/* nl */ {IDS_SID_STEREO_NL, "Stereo-SID"},
/* pl */ {IDS_SID_STEREO_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_STEREO_SV, "SID-stereo"},

/* en */ {IDS_STEREO_SID_AT,    "Stereo SID at"},
/* de */ {IDS_STEREO_SID_AT_DE, "Stereo SID bei"},
/* fr */ {IDS_STEREO_SID_AT_FR, "SID Stéréo  "},
/* hu */ {IDS_STEREO_SID_AT_HU, "Sztereó SID itt:"},
/* it */ {IDS_STEREO_SID_AT_IT, "SID Stereo a"},
/* nl */ {IDS_STEREO_SID_AT_NL, "Stereo-SID op"},
/* pl */ {IDS_STEREO_SID_AT_PL, ""},  /* fuzzy */
/* sv */ {IDS_STEREO_SID_AT_SV, "Stereo-SID pa"},

/* en */ {IDS_SID_FILTERS,    "SID Filters"},
/* de */ {IDS_SID_FILTERS_DE, "SID Filter"},
/* fr */ {IDS_SID_FILTERS_FR, "Filtres SID"},
/* hu */ {IDS_SID_FILTERS_HU, "SID szûrõk"},
/* it */ {IDS_SID_FILTERS_IT, "Filtri SID"},
/* nl */ {IDS_SID_FILTERS_NL, "SID-filters"},
/* pl */ {IDS_SID_FILTERS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_FILTERS_SV, "SID-filter"},

/* en */ {IDS_SAMPLE_METHOD,    "Sample method"},
/* de */ {IDS_SAMPLE_METHOD_DE, "Sample Methode"},
/* fr */ {IDS_SAMPLE_METHOD_FR, "Méthode d'échantillonnage"},
/* hu */ {IDS_SAMPLE_METHOD_HU, "Mintavételezés módja"},
/* it */ {IDS_SAMPLE_METHOD_IT, "Metodo di campionamento"},
/* nl */ {IDS_SAMPLE_METHOD_NL, "Sample methode"},
/* pl */ {IDS_SAMPLE_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAMPLE_METHOD_SV, "Samplingsmetod"},

/* en */ {IDS_PASSBAND_0_90,    "Passband (0-90%)"},
/* de */ {IDS_PASSBAND_0_90_DE, "Passband (0-90%)"},
/* fr */ {IDS_PASSBAND_0_90_FR, "Bande passante (0-90%)"},
/* hu */ {IDS_PASSBAND_0_90_HU, "Áteresztõság (0-90%)"},
/* it */ {IDS_PASSBAND_0_90_IT, "Passabanda (0-90%)"},
/* nl */ {IDS_PASSBAND_0_90_NL, "Passband (0-90%)"},
/* pl */ {IDS_PASSBAND_0_90_PL, ""},  /* fuzzy */
/* sv */ {IDS_PASSBAND_0_90_SV, "Passband (0-90%)"},

/* en */ {IDS_NOT_IMPLEMENTED_YET,    "Not implemented yet!"},
/* de */ {IDS_NOT_IMPLEMENTED_YET_DE, "Noch nicht implementiert!"},
/* fr */ {IDS_NOT_IMPLEMENTED_YET_FR, ""},  /* fuzzy */
/* hu */ {IDS_NOT_IMPLEMENTED_YET_HU, "Nincs még implementálva!"},
/* it */ {IDS_NOT_IMPLEMENTED_YET_IT, "Non ancora implementato!"},
/* nl */ {IDS_NOT_IMPLEMENTED_YET_NL, "Nog niet ondersteund!"},
/* pl */ {IDS_NOT_IMPLEMENTED_YET_PL, ""},  /* fuzzy */
/* sv */ {IDS_NOT_IMPLEMENTED_YET_SV, "Ej implementerat!"},

/* en */ {IDS_SID_SETTINGS,    "SID Settings"},
/* de */ {IDS_SID_SETTINGS_DE, "SID Einstellungen"},
/* fr */ {IDS_SID_SETTINGS_FR, "Parametres SID"},
/* hu */ {IDS_SID_SETTINGS_HU, "SID beállításai"},
/* it */ {IDS_SID_SETTINGS_IT, "Impostazioni SID"},
/* nl */ {IDS_SID_SETTINGS_NL, "SID-instellingen"},
/* pl */ {IDS_SID_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_SETTINGS_SV, "SID-inställningar"},

/* en */ {IDS_FLEXIBLE,    "Flexible"},
/* de */ {IDS_FLEXIBLE_DE, "Flexibel"},
/* fr */ {IDS_FLEXIBLE_FR, "Flexible"},
/* hu */ {IDS_FLEXIBLE_HU, "Rugalmas"},
/* it */ {IDS_FLEXIBLE_IT, "Flessibile"},
/* nl */ {IDS_FLEXIBLE_NL, "Flexibel"},
/* pl */ {IDS_FLEXIBLE_PL, ""},  /* fuzzy */
/* sv */ {IDS_FLEXIBLE_SV, "Flexibel"},

/* en */ {IDS_ADJUSTING,    "Adjusting"},
/* de */ {IDS_ADJUSTING_DE, "Anpassend"},
/* fr */ {IDS_ADJUSTING_FR, "Ajusté"},
/* hu */ {IDS_ADJUSTING_HU, "Igazodó"},
/* it */ {IDS_ADJUSTING_IT, "Adattamento in corso"},
/* nl */ {IDS_ADJUSTING_NL, "Aanpassend"},
/* pl */ {IDS_ADJUSTING_PL, ""},  /* fuzzy */
/* sv */ {IDS_ADJUSTING_SV, "Anpassande"},

/* en */ {IDS_EXACT,    "Exact"},
/* de */ {IDS_EXACT_DE, "Exakt"},
/* fr */ {IDS_EXACT_FR, "Exact"},
/* hu */ {IDS_EXACT_HU, "Pontos"},
/* it */ {IDS_EXACT_IT, "Esatta"},
/* nl */ {IDS_EXACT_NL, "Exact"},
/* pl */ {IDS_EXACT_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXACT_SV, "Exakt"},

/* en */ {IDS_SAMPLE_RATE,    "Sample Rate"},
/* de */ {IDS_SAMPLE_RATE_DE, "Sample Rate"},
/* fr */ {IDS_SAMPLE_RATE_FR, "Taux d'échantillonage"},
/* hu */ {IDS_SAMPLE_RATE_HU, "Mintavételezési ráta"},
/* it */ {IDS_SAMPLE_RATE_IT, "Frequenza di campionamento"},
/* nl */ {IDS_SAMPLE_RATE_NL, "Sample snelheid"},
/* pl */ {IDS_SAMPLE_RATE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAMPLE_RATE_SV, "Samplingshastighet"},

/* en */ {IDS_BUFFER_SIZE,    "Buffer Size"},
/* de */ {IDS_BUFFER_SIZE_DE, "Puffergröße"},
/* fr */ {IDS_BUFFER_SIZE_FR, "Taille du tampon"},
/* hu */ {IDS_BUFFER_SIZE_HU, "Puffer méret"},
/* it */ {IDS_BUFFER_SIZE_IT, "Dimensione del buffer"},
/* nl */ {IDS_BUFFER_SIZE_NL, "Buffergrootte"},
/* pl */ {IDS_BUFFER_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_BUFFER_SIZE_SV, "Buffertstorlek"},

/* en */ {IDS_OVERSAMPLE,    "Oversample"},
/* de */ {IDS_OVERSAMPLE_DE, "Oversample"},
/* fr */ {IDS_OVERSAMPLE_FR, "Suréchantillonage"},
/* hu */ {IDS_OVERSAMPLE_HU, "Túlmintavételezés"},
/* it */ {IDS_OVERSAMPLE_IT, "Sovracampionamento"},
/* nl */ {IDS_OVERSAMPLE_NL, "Oversample"},
/* pl */ {IDS_OVERSAMPLE_PL, ""},  /* fuzzy */
/* sv */ {IDS_OVERSAMPLE_SV, "-versampling"},

/* en */ {IDS_SPEED_ADJUSTMENT,    "Speed Adjustment"},
/* de */ {IDS_SPEED_ADJUSTMENT_DE, "Geschwindigkeitsanpassung"},
/* fr */ {IDS_SPEED_ADJUSTMENT_FR, ""},  /* fuzzy */
/* hu */ {IDS_SPEED_ADJUSTMENT_HU, "Sebesség módosítás"},
/* it */ {IDS_SPEED_ADJUSTMENT_IT, "Variazione velocità"},
/* nl */ {IDS_SPEED_ADJUSTMENT_NL, "Snelheidsaanpassing"},
/* pl */ {IDS_SPEED_ADJUSTMENT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SPEED_ADJUSTMENT_SV, "Hastighetsjustering"},

/* en */ {IDS_SOUND_SETTINGS,    "Sound Settings"},
/* de */ {IDS_SOUND_SETTINGS_DE, "Sound Einstellungen"},
/* fr */ {IDS_SOUND_SETTINGS_FR, "Parametres son"},
/* hu */ {IDS_SOUND_SETTINGS_HU, "Hang beállításai"},
/* it */ {IDS_SOUND_SETTINGS_IT, "Impostazioni audio"},
/* nl */ {IDS_SOUND_SETTINGS_NL, "Geluidsinstellingen"},
/* pl */ {IDS_SOUND_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SOUND_SETTINGS_SV, "Ljudinställningar"},

/* en */ {IDS_VIC_SETTINGS,    "VIC Settings"},
/* de */ {IDS_VIC_SETTINGS_DE, "VIC Einstellungen"},
/* fr */ {IDS_VIC_SETTINGS_FR, "Parametres VIC"},
/* hu */ {IDS_VIC_SETTINGS_HU, "VIC beállításai"},
/* it */ {IDS_VIC_SETTINGS_IT, "Impostazioni VIC"},
/* nl */ {IDS_VIC_SETTINGS_NL, "VIC-instellingen"},
/* pl */ {IDS_VIC_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_VIC_SETTINGS_SV, "VIC-inställningar"},

/* en */ {IDS_SPRITE_SPRITE_COL,    "Sprite Sprite Collisions"},
/* de */ {IDS_SPRITE_SPRITE_COL_DE, "Sprite-sprite Kollisionen"},
/* fr */ {IDS_SPRITE_SPRITE_COL_FR, "Collisions sprite-sprite"},
/* hu */ {IDS_SPRITE_SPRITE_COL_HU, "Sprite-sprite ütközések"},
/* it */ {IDS_SPRITE_SPRITE_COL_IT, "Collisioni sprite-sprite"},
/* nl */ {IDS_SPRITE_SPRITE_COL_NL, "Sprite-sprite botsingen"},
/* pl */ {IDS_SPRITE_SPRITE_COL_PL, ""},  /* fuzzy */
/* sv */ {IDS_SPRITE_SPRITE_COL_SV, "Sprite-sprite-kollisioner"},

/* en */ {IDS_SPRITE_BACKGROUND_COL,    "Sprite Background Collisions"},
/* de */ {IDS_SPRITE_BACKGROUND_COL_DE, "Sprite-Hintergrund Kollisionen"},
/* fr */ {IDS_SPRITE_BACKGROUND_COL_FR, "Collisions sprite-arriere-plan"},
/* hu */ {IDS_SPRITE_BACKGROUND_COL_HU, "Sprite-háttér ütközések"},
/* it */ {IDS_SPRITE_BACKGROUND_COL_IT, "Collisioni sprite-sfondo"},
/* nl */ {IDS_SPRITE_BACKGROUND_COL_NL, "Sprite-achtergrond botsingen"},
/* pl */ {IDS_SPRITE_BACKGROUND_COL_PL, ""},  /* fuzzy */
/* sv */ {IDS_SPRITE_BACKGROUND_COL_SV, "Sprite-bakgrundkollisioner"},

/* en */ {IDS_NEW_LUMINANCES,    "New Luminances"},
/* de */ {IDS_NEW_LUMINANCES_DE, "Neue Helligkeitsemulation"},
/* fr */ {IDS_NEW_LUMINANCES_FR, "Nouvelles luminescences"},
/* hu */ {IDS_NEW_LUMINANCES_HU, "Új fényerõk"},
/* it */ {IDS_NEW_LUMINANCES_IT, "Nuove luminanze"},
/* nl */ {IDS_NEW_LUMINANCES_NL, "Nieuwe kleuren"},
/* pl */ {IDS_NEW_LUMINANCES_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEW_LUMINANCES_SV, "Nya ljusstyrkor"},

/* en */ {IDS_VICII_SETTINGS,    "VIC-II Settings"},
/* de */ {IDS_VICII_SETTINGS_DE, "VIC-II Einstellungen"},
/* fr */ {IDS_VICII_SETTINGS_FR, "Parametres VIC-II"},
/* hu */ {IDS_VICII_SETTINGS_HU, "VIC-II beállításai"},
/* it */ {IDS_VICII_SETTINGS_IT, "Impostazioni VIC-II"},
/* nl */ {IDS_VICII_SETTINGS_NL, "VIC-II-instellingen"},
/* pl */ {IDS_VICII_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_VICII_SETTINGS_SV, "VIC-II-inställningar"},

/* en */ {IDS_ATTACH_TAPE_IMAGE,    "Attach tape image"},
/* de */ {IDS_ATTACH_TAPE_IMAGE_DE, "Band Image einlegen"},
/* fr */ {IDS_ATTACH_TAPE_IMAGE_FR, "Insérer une image datassette"},
/* hu */ {IDS_ATTACH_TAPE_IMAGE_HU, "Szalag képmás csatolása"},
/* it */ {IDS_ATTACH_TAPE_IMAGE_IT, "Seleziona l'immagine di una cassetta"},
/* nl */ {IDS_ATTACH_TAPE_IMAGE_NL, "Koppel tapebestand aan"},
/* pl */ {IDS_ATTACH_TAPE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_TAPE_IMAGE_SV, "Anslut en bandavbildningsfil"},

/* en */ {IDS_AUTOSTART_IMAGE,    "Autostart disk/tape image"},
/* de */ {IDS_AUTOSTART_IMAGE_DE, "Autostart Disk/Band Image"},
/* fr */ {IDS_AUTOSTART_IMAGE_FR, "Démarrage automatique d'une image datassette"},
/* hu */ {IDS_AUTOSTART_IMAGE_HU, "Lemez/szalag képmás automatikus indítása"},
/* it */ {IDS_AUTOSTART_IMAGE_IT, "Avvia automaticamente l'immagine di un disco/cassetta"},
/* nl */ {IDS_AUTOSTART_IMAGE_NL, "Autostart disk-/tapebestand"},
/* pl */ {IDS_AUTOSTART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_AUTOSTART_IMAGE_SV, "Autostarta disk-/bandavbildningsfil"},

/* en */ {IDS_IDE64_FILENAME,    "HD image file"},
/* de */ {IDS_IDE64_FILENAME_DE, "HD Image Name"},
/* fr */ {IDS_IDE64_FILENAME_FR, "Nom d'image DD"},
/* hu */ {IDS_IDE64_FILENAME_HU, "Merevlemez képmás fájl"},
/* it */ {IDS_IDE64_FILENAME_IT, "File immagine dell'HD"},
/* nl */ {IDS_IDE64_FILENAME_NL, "HD-bestand"},
/* pl */ {IDS_IDE64_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_IDE64_FILENAME_SV, "HD-avbildningsfil"},

/* en */ {IDS_IDE64_FILENAME_SELECT,    "Select HD image file"},
/* de */ {IDS_IDE64_FILENAME_SELECT_DE, "HD Image Datei selektieren"},
/* fr */ {IDS_IDE64_FILENAME_SELECT_FR, "Sélectionnez le fichier image du D.D."},
/* hu */ {IDS_IDE64_FILENAME_SELECT_HU, "Válassza ki a merevlemez képmás fájlt"},
/* it */ {IDS_IDE64_FILENAME_SELECT_IT, "Seleziona il file immagine dell'HD"},
/* nl */ {IDS_IDE64_FILENAME_SELECT_NL, "Selecteer HD-bestand"},
/* pl */ {IDS_IDE64_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_IDE64_FILENAME_SELECT_SV, "Ange HD-avbildningsfil"},

/* en */ {IDS_AUTODETECT,    "Autodetect"},
/* de */ {IDS_AUTODETECT_DE, "Automatische Erkennung"},
/* fr */ {IDS_AUTODETECT_FR, "Autodétection"},
/* hu */ {IDS_AUTODETECT_HU, "Automatikus érzékelés"},
/* it */ {IDS_AUTODETECT_IT, "Rileva automaticamente"},
/* nl */ {IDS_AUTODETECT_NL, "Autodetectie"},
/* pl */ {IDS_AUTODETECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_AUTODETECT_SV, "Autodetektera"},

/* en */ {IDS_CYLINDERS,    "Cylinders"},
/* de */ {IDS_CYLINDERS_DE, "Zylinder"},
/* fr */ {IDS_CYLINDERS_FR, "Cylindres"},
/* hu */ {IDS_CYLINDERS_HU, "Sávok"},
/* it */ {IDS_CYLINDERS_IT, "Cilindri"},
/* nl */ {IDS_CYLINDERS_NL, "Cylinders"},
/* pl */ {IDS_CYLINDERS_PL, ""},  /* fuzzy */
/* sv */ {IDS_CYLINDERS_SV, "Cylindrar"},

/* en */ {IDS_HEADS,    "Heads"},
/* de */ {IDS_HEADS_DE, "Köpfe"},
/* fr */ {IDS_HEADS_FR, "Têtes"},
/* hu */ {IDS_HEADS_HU, "Fejek"},
/* it */ {IDS_HEADS_IT, "Testine"},
/* nl */ {IDS_HEADS_NL, "Koppen"},
/* pl */ {IDS_HEADS_PL, ""},  /* fuzzy */
/* sv */ {IDS_HEADS_SV, "Huvuden"},

/* en */ {IDS_SECTORS,    "Sectors"},
/* de */ {IDS_SECTORS_DE, "Sektoren"},
/* fr */ {IDS_SECTORS_FR, "Secteurs"},
/* hu */ {IDS_SECTORS_HU, "Szektorok"},
/* it */ {IDS_SECTORS_IT, "Settori"},
/* nl */ {IDS_SECTORS_NL, "Sectors"},
/* pl */ {IDS_SECTORS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SECTORS_SV, "Sektorer"},

/* en */ {IDS_IDE64_SETTINGS,    "IDE64 settings"},
/* de */ {IDS_IDE64_SETTINGS_DE, "IDE64 Einstellungen"},
/* fr */ {IDS_IDE64_SETTINGS_FR, "Paramètres IDE64"},
/* hu */ {IDS_IDE64_SETTINGS_HU, "IDE64 beállításai"},
/* it */ {IDS_IDE64_SETTINGS_IT, "Impostazioni IDE64"},
/* nl */ {IDS_IDE64_SETTINGS_NL, "IDE64-instellingen"},
/* pl */ {IDS_IDE64_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_IDE64_SETTINGS_SV, "IDE64-inställningar"},

/* en */ {IDS_BIOS_REVISION,    "BIOS Revision"},
/* de */ {IDS_BIOS_REVISION_DE, "BIOS Revision"},
/* fr */ {IDS_BIOS_REVISION_FR, ""},  /* fuzzy */
/* hu */ {IDS_BIOS_REVISION_HU, "BIOS változat"},
/* it */ {IDS_BIOS_REVISION_IT, "Revisione BIOS"},
/* nl */ {IDS_BIOS_REVISION_NL, "BIOS-revisie"},
/* pl */ {IDS_BIOS_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDS_BIOS_REVISION_SV, "BIOS-utgåva"},

/* en */ {IDS_BIOS_FLASH_JUMPER,    "BIOS Flash Jumper"},
/* de */ {IDS_BIOS_FLASH_JUMPER_DE, "BIOS Flashjumper"},
/* fr */ {IDS_BIOS_FLASH_JUMPER_FR, ""},  /* fuzzy */
/* hu */ {IDS_BIOS_FLASH_JUMPER_HU, "BIOS Flash jumper"},
/* it */ {IDS_BIOS_FLASH_JUMPER_IT, "Flash Jumper del BIOS"},
/* nl */ {IDS_BIOS_FLASH_JUMPER_NL, "BIOS Flash Jumper"},
/* pl */ {IDS_BIOS_FLASH_JUMPER_PL, ""},  /* fuzzy */
/* sv */ {IDS_BIOS_FLASH_JUMPER_SV, "BIOS-flashbygel"},

/* en */ {IDS_SAVE_BIOS_WHEN_CHANGED,    "Save BIOS when changed"},
/* de */ {IDS_SAVE_BIOS_WHEN_CHANGED_DE, "BIOS bei Änderung speichern"},
/* fr */ {IDS_SAVE_BIOS_WHEN_CHANGED_FR, ""},  /* fuzzy */
/* hu */ {IDS_SAVE_BIOS_WHEN_CHANGED_HU, "BIOS mentése változás után"},
/* it */ {IDS_SAVE_BIOS_WHEN_CHANGED_IT, "Salva il BIOS al cambio"},
/* nl */ {IDS_SAVE_BIOS_WHEN_CHANGED_NL, "BIOS Opslaan indien gewijzigd"},
/* pl */ {IDS_SAVE_BIOS_WHEN_CHANGED_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_BIOS_WHEN_CHANGED_SV, "Spara BIOS vid ändring"},

/* en */ {IDS_BIOS_FILE,    "BIOS File"},
/* de */ {IDS_BIOS_FILE_DE, "BIOS Datei"},
/* fr */ {IDS_BIOS_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_BIOS_FILE_HU, "BIOS fájl"},
/* it */ {IDS_BIOS_FILE_IT, "File del BIOS"},
/* nl */ {IDS_BIOS_FILE_NL, "BIOS-bestand"},
/* pl */ {IDS_BIOS_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_BIOS_FILE_SV, "BIOS-fil"},

/* en */ {IDS_SELECT_BIOS_FILE,    "Select BIOS file"},
/* de */ {IDS_SELECT_BIOS_FILE_DE, "BIOS Datei wählen"},
/* fr */ {IDS_SELECT_BIOS_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_BIOS_FILE_HU, "Válasszon BIOS fájlt"},
/* it */ {IDS_SELECT_BIOS_FILE_IT, "Seleziona il file del BIOS"},
/* nl */ {IDS_SELECT_BIOS_FILE_NL, "Selecteer BIOS-bestand"},
/* pl */ {IDS_SELECT_BIOS_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_BIOS_FILE_SV, "Välj BIOS-fil"},

/* en */ {IDS_MMC64_IMAGE_READ_ONLY,    "MMC/SD Image read-only"},
/* de */ {IDS_MMC64_IMAGE_READ_ONLY_DE, "MMC/SD Image Schreibschutz"},
/* fr */ {IDS_MMC64_IMAGE_READ_ONLY_FR, ""},  /* fuzzy */
/* hu */ {IDS_MMC64_IMAGE_READ_ONLY_HU, "MMC/SD képmás csak olvasható"},
/* it */ {IDS_MMC64_IMAGE_READ_ONLY_IT, "Immagine MMC/SD in sola lettura"},
/* nl */ {IDS_MMC64_IMAGE_READ_ONLY_NL, "MMC/SD-bestand alleen-lezen"},
/* pl */ {IDS_MMC64_IMAGE_READ_ONLY_PL, ""},  /* fuzzy */
/* sv */ {IDS_MMC64_IMAGE_READ_ONLY_SV, "MMC/SD-avbildning skrivskyddad"},

/* en */ {IDS_MMC64_IMAGE_FILE,    "MMC/SD Image File"},
/* de */ {IDS_MMC64_IMAGE_FILE_DE, "MMC/SD Image Name"},
/* fr */ {IDS_MMC64_IMAGE_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_MMC64_IMAGE_FILE_HU, "MMC/SD képmás fájl"},
/* it */ {IDS_MMC64_IMAGE_FILE_IT, "File immagine MMC/SD"},
/* nl */ {IDS_MMC64_IMAGE_FILE_NL, "MMC/SD-bestand"},
/* pl */ {IDS_MMC64_IMAGE_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_MMC64_IMAGE_FILE_SV, "MMC/SD-avbildningsfil"},

/* en */ {IDS_SELECT_MMC64_IMAGE_FILE,    "Select MMC/SD image file"},
/* de */ {IDS_SELECT_MMC64_IMAGE_FILE_DE, "MMC/SD Image Date wählen"},
/* fr */ {IDS_SELECT_MMC64_IMAGE_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_MMC64_IMAGE_FILE_HU, "Válassza ki az MMC/SD képmás fájlt"},
/* it */ {IDS_SELECT_MMC64_IMAGE_FILE_IT, "Seleziona il file immagine MMC/SD"},
/* nl */ {IDS_SELECT_MMC64_IMAGE_FILE_NL, "Selecteer MMC/SD-bestand"},
/* pl */ {IDS_SELECT_MMC64_IMAGE_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_MMC64_IMAGE_FILE_SV, "Ange MMC/SD-avbildningsfil"},

/* en */ {IDS_MMC64_SETTINGS,    "MMC64 settings"},
/* de */ {IDS_MMC64_SETTINGS_DE, "MMC64 Einstellungen"},
/* fr */ {IDS_MMC64_SETTINGS_FR, "Paramètres MMC64"},
/* hu */ {IDS_MMC64_SETTINGS_HU, "MMC64 beállításai"},
/* it */ {IDS_MMC64_SETTINGS_IT, "Impostazioni MMC64"},
/* nl */ {IDS_MMC64_SETTINGS_NL, "MMC64-instellingen"},
/* pl */ {IDS_MMC64_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_MMC64_SETTINGS_SV, "MMC64-inställningar"},

/* en */ {IDS_SID_CART,    "SID Cart"},
/* de */ {IDS_SID_CART_DE, "SID Modul"},
/* fr */ {IDS_SID_CART_FR, ""},  /* fuzzy */
/* hu */ {IDS_SID_CART_HU, "SID kártya"},
/* it */ {IDS_SID_CART_IT, "Cartuccia SID"},
/* nl */ {IDS_SID_CART_NL, "SID-cart"},
/* pl */ {IDS_SID_CART_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_CART_SV, "SID-modul"},

/* en */ {IDS_SID_ENGINE,    "SID Engine"},
/* de */ {IDS_SID_ENGINE_DE, "SID Engine"},
/* fr */ {IDS_SID_ENGINE_FR, "Engin SID"},
/* hu */ {IDS_SID_ENGINE_HU, "SID Engine"},
/* it */ {IDS_SID_ENGINE_IT, "Motore SID"},
/* nl */ {IDS_SID_ENGINE_NL, "SID-kern"},
/* pl */ {IDS_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_ENGINE_SV, "SID-motor"},

/* en */ {IDS_SID_ADDRESS,    "SID Address"},
/* de */ {IDS_SID_ADDRESS_DE, "SID Adresse"},
/* fr */ {IDS_SID_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDS_SID_ADDRESS_HU, "SID cím"},
/* it */ {IDS_SID_ADDRESS_IT, "Indirizzo SID"},
/* nl */ {IDS_SID_ADDRESS_NL, "SID-adres"},
/* pl */ {IDS_SID_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_ADDRESS_SV, "SID-adress"},

/* en */ {IDS_SID_CLOCK,    "SID Clock"},
/* de */ {IDS_SID_CLOCK_DE, "SID Uhr"},
/* fr */ {IDS_SID_CLOCK_FR, ""},  /* fuzzy */
/* hu */ {IDS_SID_CLOCK_HU, "SID órajel"},
/* it */ {IDS_SID_CLOCK_IT, "SID Clock"},
/* nl */ {IDS_SID_CLOCK_NL, "SID-klok"},
/* pl */ {IDS_SID_CLOCK_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_CLOCK_SV, "SID-klocka"},

/* en */ {IDS_SIDCART_SETTINGS,    "SID cart settings"},
/* de */ {IDS_SIDCART_SETTINGS_DE, "SID Modul Einstellungen"},
/* fr */ {IDS_SIDCART_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_SIDCART_SETTINGS_HU, "SID kártya beállításai..."},
/* it */ {IDS_SIDCART_SETTINGS_IT, "Impostazioni cartuccia SID"},
/* nl */ {IDS_SIDCART_SETTINGS_NL, "SID-cartinstellingen"},
/* pl */ {IDS_SIDCART_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SIDCART_SETTINGS_SV, "SID-modulinställningarg"},

/* en */ {IDS_LOAD_FLIP_LIST_FILE,    "Load flip list file"},
/* de */ {IDS_LOAD_FLIP_LIST_FILE_DE, "Fliplist Datei Laden"},
/* fr */ {IDS_LOAD_FLIP_LIST_FILE_FR, "Charger un fichier de groupement de disques"},
/* hu */ {IDS_LOAD_FLIP_LIST_FILE_HU, "Lemezlista betöltése"},
/* it */ {IDS_LOAD_FLIP_LIST_FILE_IT, "Carica il file della fliplist"},
/* nl */ {IDS_LOAD_FLIP_LIST_FILE_NL, "Laad fliplijstbestand"},
/* pl */ {IDS_LOAD_FLIP_LIST_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_LOAD_FLIP_LIST_FILE_SV, "Läs vallistefil"},

/* en */ {IDS_CANNOT_READ_FLIP_LIST,    "Cannot read flip list file"},
/* de */ {IDS_CANNOT_READ_FLIP_LIST_DE, "Lesen von Fliplist Datei fehlgeschlagen"},
/* fr */ {IDS_CANNOT_READ_FLIP_LIST_FR, ""},  /* fuzzy */
/* hu */ {IDS_CANNOT_READ_FLIP_LIST_HU, "Nem lehet olvasni a lemezlista fájlt"},
/* it */ {IDS_CANNOT_READ_FLIP_LIST_IT, "Non è possibile caricare il file della flip list"},
/* nl */ {IDS_CANNOT_READ_FLIP_LIST_NL, "Kan het fliplijstbestand niet lezen"},
/* pl */ {IDS_CANNOT_READ_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDS_CANNOT_READ_FLIP_LIST_SV, "Kunde inte läsa vallistefil"},

/* en */ {IDS_SAVE_FLIP_LIST_FILE,    "Save flip list file"},
/* de */ {IDS_SAVE_FLIP_LIST_FILE_DE, "Fliplist Datei Speichern"},
/* fr */ {IDS_SAVE_FLIP_LIST_FILE_FR, "Enregistrer le fichier de groupement de disques actuel"},
/* hu */ {IDS_SAVE_FLIP_LIST_FILE_HU, "Lemezlista mentése fájlba"},
/* it */ {IDS_SAVE_FLIP_LIST_FILE_IT, "Salva il file della fliplist"},
/* nl */ {IDS_SAVE_FLIP_LIST_FILE_NL, "Fliplijstbestand opslaan"},
/* pl */ {IDS_SAVE_FLIP_LIST_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_FLIP_LIST_FILE_SV, "Spara vallistefil"},

/* en */ {IDS_CANNOT_WRITE_FLIP_LIST,    "Cannot write flip list file"},
/* de */ {IDS_CANNOT_WRITE_FLIP_LIST_DE, "Schreiben von Fliplist Datei fehlgeschlagen"},
/* fr */ {IDS_CANNOT_WRITE_FLIP_LIST_FR, "Impossible d'écrire le fichier de groupement de disques"},
/* hu */ {IDS_CANNOT_WRITE_FLIP_LIST_HU, "Nem sikerült a lemezlistát fájlba írni"},
/* it */ {IDS_CANNOT_WRITE_FLIP_LIST_IT, "Non è possibile scrivere il file della flip list"},
/* nl */ {IDS_CANNOT_WRITE_FLIP_LIST_NL, "Kan fliplijstbestand niet schrijven"},
/* pl */ {IDS_CANNOT_WRITE_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDS_CANNOT_WRITE_FLIP_LIST_SV, "Kan inte skriva vallistefil"},

/* en */ {IDS_ATTACH_FUNCTION_LOW_CART,    "Attach cartridge image for Function Low"},
/* de */ {IDS_ATTACH_FUNCTION_LOW_CART_DE, "Funktions ROM (low) Erweiterungsmodul einlegen"},
/* fr */ {IDS_ATTACH_FUNCTION_LOW_CART_FR, "Insérer une cartouche Fonction basse"},
/* hu */ {IDS_ATTACH_FUNCTION_LOW_CART_HU, "Cartridge képmás csatolása Function Low-hoz"},
/* it */ {IDS_ATTACH_FUNCTION_LOW_CART_IT, "Seleziona l'immagine di una cartuccia per la Function Low"},
/* nl */ {IDS_ATTACH_FUNCTION_LOW_CART_NL, "Koppel cartridgebestand voor 'Function Low'"},
/* pl */ {IDS_ATTACH_FUNCTION_LOW_CART_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_FUNCTION_LOW_CART_SV, "Anslut insticksmodulfil för funktion låg"},

/* en */ {IDS_ATTACH_FUNCTION_HIGH_CART,    "Attach cartridge image for Function High"},
/* de */ {IDS_ATTACH_FUNCTION_HIGH_CART_DE, "Funktions ROM (high) Erweiterungsmodul einlegen"},
/* fr */ {IDS_ATTACH_FUNCTION_HIGH_CART_FR, "Insérer une cartouche Fonction haute"},
/* hu */ {IDS_ATTACH_FUNCTION_HIGH_CART_HU, "Cartridge képmás csatolása Function High-hez"},
/* it */ {IDS_ATTACH_FUNCTION_HIGH_CART_IT, "Seleziona l'immagine di una cartuccia per la Function High"},
/* nl */ {IDS_ATTACH_FUNCTION_HIGH_CART_NL, "Koppel cartridgebestand voor 'Function High'"},
/* pl */ {IDS_ATTACH_FUNCTION_HIGH_CART_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_FUNCTION_HIGH_CART_SV, "Anslut insticksmodulfil för funktion hög"},

/* en */ {IDS_ATTACH_CART1_LOW,    "Attach cartridge image for Cartridge1 Low"},
/* de */ {IDS_ATTACH_CART1_LOW_DE, "Cartridge1 (low) Erweiterungsmodul einlegen"},
/* fr */ {IDS_ATTACH_CART1_LOW_FR, "Insérer une cartouche pour Cartouche1 basse"},
/* hu */ {IDS_ATTACH_CART1_LOW_HU, "Cartridge képmás csatolása alsó Cartridge1-hez"},
/* it */ {IDS_ATTACH_CART1_LOW_IT, "Seleziona l'immagine di una cartuccia per la Cartridge1 Low"},
/* nl */ {IDS_ATTACH_CART1_LOW_NL, "Koppel cartridgebestand voor 'Cartridge1 Low'"},
/* pl */ {IDS_ATTACH_CART1_LOW_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_CART1_LOW_SV, "Anslut insticksmodulfil för modul1 låg"},

/* en */ {IDS_ATTACH_CART1_HIGH,    "Attach cartridge image for Cartridge1 High"},
/* de */ {IDS_ATTACH_CART1_HIGH_DE, "Cartridge1 (high) Erweiterungsmodul einlegen"},
/* fr */ {IDS_ATTACH_CART1_HIGH_FR, "Insérer une cartouche pour Cartouche1 haute"},
/* hu */ {IDS_ATTACH_CART1_HIGH_HU, "Cartridge képmás csatolása felsõ Cartridge1-hez"},
/* it */ {IDS_ATTACH_CART1_HIGH_IT, "Seleziona l'immagine di una cartuccia per la Cartridge1 High"},
/* nl */ {IDS_ATTACH_CART1_HIGH_NL, "Koppel cartridgebestand voor 'Cartridge1 High'"},
/* pl */ {IDS_ATTACH_CART1_HIGH_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_CART1_HIGH_SV, "Anslut insticksmodulfil för modul1 hög"},

/* en */ {IDS_ATTACH_CART2_LOW,    "Attach cartridge image for Cartridge2 Low"},
/* de */ {IDS_ATTACH_CART2_LOW_DE, "Cartridge2 (low) Erweiterungsmodul einlegen"},
/* fr */ {IDS_ATTACH_CART2_LOW_FR, "Insérer une cartouche pour Cartouche2 basse"},
/* hu */ {IDS_ATTACH_CART2_LOW_HU, "Cartridge képmás csatolása alsó Cartridge2-höz"},
/* it */ {IDS_ATTACH_CART2_LOW_IT, "Seleziona l'immagine di una cartuccia per la Cartridge2 Low"},
/* nl */ {IDS_ATTACH_CART2_LOW_NL, "Koppel cartridgebestand voor 'Cartridge2 Low'"},
/* pl */ {IDS_ATTACH_CART2_LOW_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_CART2_LOW_SV, "Anslut insticksmodulfil för modul2 låg"},

/* en */ {IDS_ATTACH_CART2_HIGH,    "Attach cartridge image for Cartridge2 High"},
/* de */ {IDS_ATTACH_CART2_HIGH_DE, "Cartridge2 (high) Erweiterungsmodul einlegen"},
/* fr */ {IDS_ATTACH_CART2_HIGH_FR, "Insérer une cartouche pour Cartouche2 haute"},
/* hu */ {IDS_ATTACH_CART2_HIGH_HU, "Cartridge képmás csatolása felsõ Cartridge2-höz"},
/* it */ {IDS_ATTACH_CART2_HIGH_IT, "Seleziona l'immagine di una cartuccia per la Cartridge2 High"},
/* nl */ {IDS_ATTACH_CART2_HIGH_NL, "Koppel cartridgebestand voor 'Cartridge2 High'"},
/* pl */ {IDS_ATTACH_CART2_HIGH_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_CART2_HIGH_SV, "Anslut insticksmodulfil för modul2 hög"},

/* en */ {IDS_SAVE_SNAPSHOT_IMAGE,    "Save snapshot image"},
/* de */ {IDS_SAVE_SNAPSHOT_IMAGE_DE, "Snapshot Datei speichern"},
/* fr */ {IDS_SAVE_SNAPSHOT_IMAGE_FR, "Enregistrer le fichier de sauvegarde"},
/* hu */ {IDS_SAVE_SNAPSHOT_IMAGE_HU, "Pillanatkép fájl mentése"},
/* it */ {IDS_SAVE_SNAPSHOT_IMAGE_IT, "Salva l'immagine dello snapshot"},
/* nl */ {IDS_SAVE_SNAPSHOT_IMAGE_NL, "Momentopnamebestand opslaan"},
/* pl */ {IDS_SAVE_SNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_SNAPSHOT_IMAGE_SV, "Spara ögonblicksbildfil"},

/* en */ {IDS_LOAD_SNAPSHOT_IMAGE,    "Load snapshot image"},
/* de */ {IDS_LOAD_SNAPSHOT_IMAGE_DE, "Snapshot laden"},
/* fr */ {IDS_LOAD_SNAPSHOT_IMAGE_FR, "Charger le fichier de sauvegarde"},
/* hu */ {IDS_LOAD_SNAPSHOT_IMAGE_HU, "Pillanatkép betöltése"},
/* it */ {IDS_LOAD_SNAPSHOT_IMAGE_IT, "Carica immagine dello snapshot"},
/* nl */ {IDS_LOAD_SNAPSHOT_IMAGE_NL, "Laad momentopnamebestand"},
/* pl */ {IDS_LOAD_SNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_LOAD_SNAPSHOT_IMAGE_SV, "Läser in ögonblicksbild"},

/* en */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS,    "Save currently attached disk images"},
/* de */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_DE, "Aktuell eingelegte Diskimages speichern"},
/* fr */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_FR, "Sauvegarder les disques présentement insérés"},
/* hu */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_HU, "A jelenleg csatolt lemezek mentése"},
/* it */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_IT, "Salva le immagini dei dischi attualmente selezionati"},
/* nl */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_NL, "Opslaan huidige gekoppelde schijfbestanden"},
/* pl */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_SV, "Spara anslutna disketter diskettavbildningar"},

/* en */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS,    "Save currently loaded ROM images"},
/* de */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_DE, "Aktuelle ROMs speichern"},
/* fr */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_FR, "Sauvegarder les images ROM présentement chargés"},
/* hu */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_HU, "A jelenleg betöltött ROM-ok mentése"},
/* it */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_IT, "Salva le immagini delle ROM attualmente caricate"},
/* nl */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_NL, "Opslaan huidig gekoppelde ROMs"},
/* pl */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_SV, "Spara lästa ROM-avbildningar"},

/* en */ {IDS_CANNOT_WRITE_SNAPSHOT_S,    "Cannot write snapshot file `%s'."},
/* de */ {IDS_CANNOT_WRITE_SNAPSHOT_S_DE, "Kann Snapshot Datei `%s' nicht speichern."},
/* fr */ {IDS_CANNOT_WRITE_SNAPSHOT_S_FR, ""},  /* fuzzy */
/* hu */ {IDS_CANNOT_WRITE_SNAPSHOT_S_HU, "Nem sikerült a pillanatkép írása %s fájlba."},
/* it */ {IDS_CANNOT_WRITE_SNAPSHOT_S_IT, "Non è possibile scrivere il file di snapshot `%s'."},
/* nl */ {IDS_CANNOT_WRITE_SNAPSHOT_S_NL, "Kan momentopnamebestand `%s' niet schrijven."},
/* pl */ {IDS_CANNOT_WRITE_SNAPSHOT_S_PL, ""},  /* fuzzy */
/* sv */ {IDS_CANNOT_WRITE_SNAPSHOT_S_SV, "Kan inte skriva ögonblicksbildfil \"%s\"."},

/* en */ {IDS_CANNOT_READ_SNAPSHOT_IMG,    "Cannot read snapshot image"},
/* de */ {IDS_CANNOT_READ_SNAPSHOT_IMG_DE, "Kann Snapshot Datei nicht laden"},
/* fr */ {IDS_CANNOT_READ_SNAPSHOT_IMG_FR, ""},  /* fuzzy */
/* hu */ {IDS_CANNOT_READ_SNAPSHOT_IMG_HU, "A pillanatkép fájl nem olvasható"},
/* it */ {IDS_CANNOT_READ_SNAPSHOT_IMG_IT, "Non è possibile leggere l'immagine dello snapshot"},
/* nl */ {IDS_CANNOT_READ_SNAPSHOT_IMG_NL, "Kan momentopname bestand niet lezen"},
/* pl */ {IDS_CANNOT_READ_SNAPSHOT_IMG_PL, ""},  /* fuzzy */
/* sv */ {IDS_CANNOT_READ_SNAPSHOT_IMG_SV, "Kan inte läsa in ögonblicksbildfil"},

/* en */ {IDS_SNAPSHOT_FILE,    "snapshot file"},
/* de */ {IDS_SNAPSHOT_FILE_DE, "Snapshot Datei"},
/* fr */ {IDS_SNAPSHOT_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SNAPSHOT_FILE_HU, "pillanatkép fájl"},
/* it */ {IDS_SNAPSHOT_FILE_IT, "File di snapshot"},
/* nl */ {IDS_SNAPSHOT_FILE_NL, "momentopnamebestand"},
/* pl */ {IDS_SNAPSHOT_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SNAPSHOT_FILE_SV, "ögonblicksbildfil"},

/* en */ {IDS_SELECT_SNAPSHOT_FILE,    "Select snapshot file"},
/* de */ {IDS_SELECT_SNAPSHOT_FILE_DE, "Snapshot Datei wählen"},
/* fr */ {IDS_SELECT_SNAPSHOT_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_SNAPSHOT_FILE_HU, "Válasszon pillanatkép fájlt"},
/* it */ {IDS_SELECT_SNAPSHOT_FILE_IT, "Seleziona il file di snapshot"},
/* nl */ {IDS_SELECT_SNAPSHOT_FILE_NL, "Selecteer momentopnamebestand"},
/* pl */ {IDS_SELECT_SNAPSHOT_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_SNAPSHOT_FILE_SV, "Välj ögonblicksbildfil"},

/* en */ {IDS_FAST_PAL,    "Fast PAL"},
/* de */ {IDS_FAST_PAL_DE, "Schnelle PAL"},
/* fr */ {IDS_FAST_PAL_FR, ""},  /* fuzzy */
/* hu */ {IDS_FAST_PAL_HU, "Gyors PAL"},
/* it */ {IDS_FAST_PAL_IT, "PAL veloce"},
/* nl */ {IDS_FAST_PAL_NL, "Snelle PAL"},
/* pl */ {IDS_FAST_PAL_PL, ""},  /* fuzzy */
/* sv */ {IDS_FAST_PAL_SV, "Snabb PAL"},

/* en */ {IDS_PAL_EMULATION,    "PAL Emulation"},
/* de */ {IDS_PAL_EMULATION_DE, "PAL Emulation"},
/* fr */ {IDS_PAL_EMULATION_FR, "Émulation PAL"},
/* hu */ {IDS_PAL_EMULATION_HU, "PAL emuláció"},
/* it */ {IDS_PAL_EMULATION_IT, "Emulazione PAL"},
/* nl */ {IDS_PAL_EMULATION_NL, "PAL-emulatie"},
/* pl */ {IDS_PAL_EMULATION_PL, ""},  /* fuzzy */
/* sv */ {IDS_PAL_EMULATION_SV, "PAL-emulering"},

/* en */ {IDS_NEW_PAL_EMULATION,    "New PAL Emulation"},
/* de */ {IDS_NEW_PAL_EMULATION_DE, ""},  /* fuzzy */
/* fr */ {IDS_NEW_PAL_EMULATION_FR, ""},  /* fuzzy */
/* hu */ {IDS_NEW_PAL_EMULATION_HU, ""},  /* fuzzy */
/* it */ {IDS_NEW_PAL_EMULATION_IT, ""},  /* fuzzy */
/* nl */ {IDS_NEW_PAL_EMULATION_NL, ""},  /* fuzzy */
/* pl */ {IDS_NEW_PAL_EMULATION_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEW_PAL_EMULATION_SV, ""},  /* fuzzy */

/* en */ {IDS_EXTERNAL_PALETTE_SELECT,    "Select External Palette file"},
/* de */ {IDS_EXTERNAL_PALETTE_SELECT_DE, "Benutze externe Palette (Datei)"},
/* fr */ {IDS_EXTERNAL_PALETTE_SELECT_FR, ""},  /* fuzzy */
/* hu */ {IDS_EXTERNAL_PALETTE_SELECT_HU, "Válasszon külsõ paletta fájlt"},
/* it */ {IDS_EXTERNAL_PALETTE_SELECT_IT, "Seleziona il file per la palette esterna"},
/* nl */ {IDS_EXTERNAL_PALETTE_SELECT_NL, "Selecteer extern paletbestand"},
/* pl */ {IDS_EXTERNAL_PALETTE_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXTERNAL_PALETTE_SELECT_SV, "Använd en extern palett (fil)"},

/* en */ {IDS_GAMMA_0_2,    "Gamma (0..2)"},
/* de */ {IDS_GAMMA_0_2_DE, "Gamma (0..2)"},
/* fr */ {IDS_GAMMA_0_2_FR, "Intensité (0..2)"},
/* hu */ {IDS_GAMMA_0_2_HU, "Gamma (0..2)"},
/* it */ {IDS_GAMMA_0_2_IT, "Gamma (0..2)"},
/* nl */ {IDS_GAMMA_0_2_NL, "Gamma (0..2)"},
/* pl */ {IDS_GAMMA_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_GAMMA_0_2_SV, "Gamma (0..2)"},

/* en */ {IDS_PAL_SHADE_0_1,    "PAL Shade (0..1)"},
/* de */ {IDS_PAL_SHADE_0_1_DE, "PAL Schatten (0..1)"},
/* fr */ {IDS_PAL_SHADE_0_1_FR, "Ombrage PAL (0..1)"},
/* hu */ {IDS_PAL_SHADE_0_1_HU, "PAL pászta árnyalás (0..2)"},
/* it */ {IDS_PAL_SHADE_0_1_IT, "Ombra PAL (0..1)"},
/* nl */ {IDS_PAL_SHADE_0_1_NL, "PAL-Schaduw (0..1)"},
/* pl */ {IDS_PAL_SHADE_0_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_PAL_SHADE_0_1_SV, "PAL-skuggning (0..1)"},

/* en */ {IDS_PAL_BLUR_0_1,    "PAL Blur (0..1)"},
/* de */ {IDS_PAL_BLUR_0_1_DE, "PAL Unschärfe (0..1)"},
/* fr */ {IDS_PAL_BLUR_0_1_FR, "Flou PAL (0..1)"},
/* hu */ {IDS_PAL_BLUR_0_1_HU, "PAL elmosás (0..1)"},
/* it */ {IDS_PAL_BLUR_0_1_IT, "Sfuocatura PAL (0..1)"},
/* nl */ {IDS_PAL_BLUR_0_1_NL, "PAL-Wazigheid (0..1)"},
/* pl */ {IDS_PAL_BLUR_0_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_PAL_BLUR_0_1_SV, "PAL-suddighet (0..1)"},

/* en */ {IDS_NEW_PAL_TINT_0_2,    "Tint (0..2)"},
/* de */ {IDS_NEW_PAL_TINT_0_2_DE, ""},  /* fuzzy */
/* fr */ {IDS_NEW_PAL_TINT_0_2_FR, ""},  /* fuzzy */
/* hu */ {IDS_NEW_PAL_TINT_0_2_HU, ""},  /* fuzzy */
/* it */ {IDS_NEW_PAL_TINT_0_2_IT, ""},  /* fuzzy */
/* nl */ {IDS_NEW_PAL_TINT_0_2_NL, "Tint (0..2)"},
/* pl */ {IDS_NEW_PAL_TINT_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEW_PAL_TINT_0_2_SV, ""},  /* fuzzy */

/* en */ {IDS_NEW_PAL_PHASE_0_2,    "Odd Lines Phase (0..2)"},
/* de */ {IDS_NEW_PAL_PHASE_0_2_DE, ""},  /* fuzzy */
/* fr */ {IDS_NEW_PAL_PHASE_0_2_FR, ""},  /* fuzzy */
/* hu */ {IDS_NEW_PAL_PHASE_0_2_HU, ""},  /* fuzzy */
/* it */ {IDS_NEW_PAL_PHASE_0_2_IT, ""},  /* fuzzy */
/* nl */ {IDS_NEW_PAL_PHASE_0_2_NL, "Oneven lijnen fase (0..2)"},
/* pl */ {IDS_NEW_PAL_PHASE_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEW_PAL_PHASE_0_2_SV, ""},  /* fuzzy */

/* en */ {IDS_NEW_PAL_OFFSET_0_2,    "Odd Lines Offset (0..2)"},
/* de */ {IDS_NEW_PAL_OFFSET_0_2_DE, ""},  /* fuzzy */
/* fr */ {IDS_NEW_PAL_OFFSET_0_2_FR, ""},  /* fuzzy */
/* hu */ {IDS_NEW_PAL_OFFSET_0_2_HU, ""},  /* fuzzy */
/* it */ {IDS_NEW_PAL_OFFSET_0_2_IT, ""},  /* fuzzy */
/* nl */ {IDS_NEW_PAL_OFFSET_0_2_NL, "Oneven lijnen compensatie (0..2)"},
/* pl */ {IDS_NEW_PAL_OFFSET_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEW_PAL_OFFSET_0_2_SV, ""},  /* fuzzy */

/* en */ {IDS_PAL_MODE,    "PAL Mode"},
/* de */ {IDS_PAL_MODE_DE, "PAL Modus"},
/* fr */ {IDS_PAL_MODE_FR, "Mode PAL"},
/* hu */ {IDS_PAL_MODE_HU, "PAL mód"},
/* it */ {IDS_PAL_MODE_IT, "Modalità PAL"},
/* nl */ {IDS_PAL_MODE_NL, "PAL-modus"},
/* pl */ {IDS_PAL_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDS_PAL_MODE_SV, "PAL-läge"},

/* en */ {IDS_EXTERNAL_PALETTE,    "External Palette"},
/* de */ {IDS_EXTERNAL_PALETTE_DE, "Externe Palette"},
/* fr */ {IDS_EXTERNAL_PALETTE_FR, "Palette externe"},
/* hu */ {IDS_EXTERNAL_PALETTE_HU, "Külsõ színkészlet"},
/* it */ {IDS_EXTERNAL_PALETTE_IT, "Palette esterna"},
/* nl */ {IDS_EXTERNAL_PALETTE_NL, "Extern Palet"},
/* pl */ {IDS_EXTERNAL_PALETTE_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXTERNAL_PALETTE_SV, "Extern palett"},

/* en */ {IDS_PALETTE_FILENAME,    "Palette file"},
/* de */ {IDS_PALETTE_FILENAME_DE, "Palette Datei"},
/* fr */ {IDS_PALETTE_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_PALETTE_FILENAME_HU, "Paletta fájl"},
/* it */ {IDS_PALETTE_FILENAME_IT, "File di palette"},
/* nl */ {IDS_PALETTE_FILENAME_NL, "Paletbestand"},
/* pl */ {IDS_PALETTE_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_PALETTE_FILENAME_SV, "Palettfiler"},

/* en */ {IDS_SATURATION_0_2,    "Saturation (0..2)"},
/* de */ {IDS_SATURATION_0_2_DE, "Sättigung (0..2)"},
/* fr */ {IDS_SATURATION_0_2_FR, "Saturation (0..2)"},
/* hu */ {IDS_SATURATION_0_2_HU, "Telítettség (0..2)"},
/* it */ {IDS_SATURATION_0_2_IT, "Saturazione (0..2)"},
/* nl */ {IDS_SATURATION_0_2_NL, "Verzadiging (0..2)"},
/* pl */ {IDS_SATURATION_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_SATURATION_0_2_SV, "Mättnad (0..2)"},

/* en */ {IDS_CONTRAST_0_2,    "Contrast (0..2)"},
/* de */ {IDS_CONTRAST_0_2_DE, "Kontrast (0..2)"},
/* fr */ {IDS_CONTRAST_0_2_FR, "Contraste (0..2)"},
/* hu */ {IDS_CONTRAST_0_2_HU, "Kontraszt (0..2)"},
/* it */ {IDS_CONTRAST_0_2_IT, "Contrasto (0..2)"},
/* nl */ {IDS_CONTRAST_0_2_NL, "Contrast (0..2)"},
/* pl */ {IDS_CONTRAST_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_CONTRAST_0_2_SV, "Kontrast (0..2)"},

/* en */ {IDS_BRIGHTNESS_0_2,    "Brightness (0..2)"},
/* de */ {IDS_BRIGHTNESS_0_2_DE, "Helligkeit (0..2)"},
/* fr */ {IDS_BRIGHTNESS_0_2_FR, "Luminosité (0..2)"},
/* hu */ {IDS_BRIGHTNESS_0_2_HU, "Fényesség (0..2)"},
/* it */ {IDS_BRIGHTNESS_0_2_IT, "Luminosità (0..2)"},
/* nl */ {IDS_BRIGHTNESS_0_2_NL, "Helderheid (0..2)"},
/* pl */ {IDS_BRIGHTNESS_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_BRIGHTNESS_0_2_SV, "Ljusstyrka (0..2)"},

/* en */ {IDS_VIDEO_SETTINGS,    "Video settings"},
/* de */ {IDS_VIDEO_SETTINGS_DE, "Video Einstellungen"},
/* fr */ {IDS_VIDEO_SETTINGS_FR, "Paramètres vidéo"},
/* hu */ {IDS_VIDEO_SETTINGS_HU, "Videó beállításai"},
/* it */ {IDS_VIDEO_SETTINGS_IT, "Impostazioni video"},
/* nl */ {IDS_VIDEO_SETTINGS_NL, "Videoinstellingen"},
/* pl */ {IDS_VIDEO_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_VIDEO_SETTINGS_SV, "Grafikinställningar"},

/* en */ {IDS_VICII_PALETTE_FILENAME,    "VICII Palette file"},
/* de */ {IDS_VICII_PALETTE_FILENAME_DE, "VICII Paletten Datei"},
/* fr */ {IDS_VICII_PALETTE_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_VICII_PALETTE_FILENAME_HU, "VICII paletta fájl"},
/* it */ {IDS_VICII_PALETTE_FILENAME_IT, "File di palette VICII"},
/* nl */ {IDS_VICII_PALETTE_FILENAME_NL, "VICII paletbestand"},
/* pl */ {IDS_VICII_PALETTE_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_VICII_PALETTE_FILENAME_SV, "Läs in VICII-palettfil"},

/* en */ {IDS_VDC_PALETTE_FILENAME,    "VDC Palette file"},
/* de */ {IDS_VDC_PALETTE_FILENAME_DE, "VDC Paletten Datei"},
/* fr */ {IDS_VDC_PALETTE_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_VDC_PALETTE_FILENAME_HU, "VDC paletta fájl"},
/* it */ {IDS_VDC_PALETTE_FILENAME_IT, "File di palette VDC"},
/* nl */ {IDS_VDC_PALETTE_FILENAME_NL, "VDC-paletbestand"},
/* pl */ {IDS_VDC_PALETTE_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_VDC_PALETTE_FILENAME_SV, "Läs in VDC-palettfil"},

/* en */ {IDS_AUTO_FROM_ROM,    "Auto (from ROM)"},
/* de */ {IDS_AUTO_FROM_ROM_DE, "Automatisch (von ROM)"},
/* fr */ {IDS_AUTO_FROM_ROM_FR, ""},  /* fuzzy */
/* hu */ {IDS_AUTO_FROM_ROM_HU, "Automatikus (ROM-ból)"},
/* it */ {IDS_AUTO_FROM_ROM_IT, "Auto (dalla ROM)"},
/* nl */ {IDS_AUTO_FROM_ROM_NL, "Automatisch (van ROM)"},
/* pl */ {IDS_AUTO_FROM_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDS_AUTO_FROM_ROM_SV, "Automatisk (från ROM)"},

/* en */ {IDS_40_COLUMNS,    "40 Columns"},
/* de */ {IDS_40_COLUMNS_DE, "40 Spalten"},
/* fr */ {IDS_40_COLUMNS_FR, ""},  /* fuzzy */
/* hu */ {IDS_40_COLUMNS_HU, "40 Oszlop"},
/* it */ {IDS_40_COLUMNS_IT, "40 Colonne"},
/* nl */ {IDS_40_COLUMNS_NL, "40 Kolommen"},
/* pl */ {IDS_40_COLUMNS_PL, ""},  /* fuzzy */
/* sv */ {IDS_40_COLUMNS_SV, "40 kolumner"},

/* en */ {IDS_80_COLUMNS,    "80 Columns"},
/* de */ {IDS_80_COLUMNS_DE, "80 Spalten"},
/* fr */ {IDS_80_COLUMNS_FR, ""},  /* fuzzy */
/* hu */ {IDS_80_COLUMNS_HU, "80 Oszlop"},
/* it */ {IDS_80_COLUMNS_IT, "80 Colonne"},
/* nl */ {IDS_80_COLUMNS_NL, "80 Kolommen"},
/* pl */ {IDS_80_COLUMNS_PL, ""},  /* fuzzy */
/* sv */ {IDS_80_COLUMNS_SV, "80 kolumner"},

/* en */ {IDS_256_BYTE,    "256 Byte"},
/* de */ {IDS_256_BYTE_DE, "256 Byte"},
/* fr */ {IDS_256_BYTE_FR, ""},  /* fuzzy */
/* hu */ {IDS_256_BYTE_HU, "256 Bájt"},
/* it */ {IDS_256_BYTE_IT, "256 Byte"},
/* nl */ {IDS_256_BYTE_NL, "256 Byte"},
/* pl */ {IDS_256_BYTE_PL, ""},  /* fuzzy */
/* sv */ {IDS_256_BYTE_SV, "256 byte"},

/* en */ {IDS_2_KBYTE,    "2 kByte"},
/* de */ {IDS_2_KBYTE_DE, "2 kByte"},
/* fr */ {IDS_2_KBYTE_FR, ""},  /* fuzzy */
/* hu */ {IDS_2_KBYTE_HU, "2 kBájt"},
/* it */ {IDS_2_KBYTE_IT, "2 kByte"},
/* nl */ {IDS_2_KBYTE_NL, "2 kByte"},
/* pl */ {IDS_2_KBYTE_PL, ""},  /* fuzzy */
/* sv */ {IDS_2_KBYTE_SV, "2 kbyte"},

/* en */ {IDS_GRAPHICS,    "Graphics"},
/* de */ {IDS_GRAPHICS_DE, "Grafik"},
/* fr */ {IDS_GRAPHICS_FR, ""},  /* fuzzy */
/* hu */ {IDS_GRAPHICS_HU, "Grafikus"},
/* it */ {IDS_GRAPHICS_IT, "Grafica"},
/* nl */ {IDS_GRAPHICS_NL, "Graphics"},
/* pl */ {IDS_GRAPHICS_PL, ""},  /* fuzzy */
/* sv */ {IDS_GRAPHICS_SV, "Grafisk"},

/* en */ {IDS_BUSINESS_UK,    "Business (UK)"},
/* de */ {IDS_BUSINESS_UK_DE, "Business (UK)"},
/* fr */ {IDS_BUSINESS_UK_FR, ""},  /* fuzzy */
/* hu */ {IDS_BUSINESS_UK_HU, "Hivatalos (UK)"},
/* it */ {IDS_BUSINESS_UK_IT, "Business (UK)"},
/* nl */ {IDS_BUSINESS_UK_NL, "Business (UK)"},
/* pl */ {IDS_BUSINESS_UK_PL, ""},  /* fuzzy */
/* sv */ {IDS_BUSINESS_UK_SV, "Business (Storbritannien)"},

/* en */ {IDS_SET_PET_MODEL,    "Set PET Model"},
/* de */ {IDS_SET_PET_MODEL_DE, "Wähle PET Modell"},
/* fr */ {IDS_SET_PET_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDS_SET_PET_MODEL_HU, "PET modell kiválasztása"},
/* it */ {IDS_SET_PET_MODEL_IT, "Imposta modello di PET"},
/* nl */ {IDS_SET_PET_MODEL_NL, "Stel PET-model in"},
/* pl */ {IDS_SET_PET_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDS_SET_PET_MODEL_SV, "Välj PET-modell"},

/* en */ {IDS_MEMORY,    "Memory"},
/* de */ {IDS_MEMORY_DE, "Speicher"},
/* fr */ {IDS_MEMORY_FR, "Mémoire"},
/* hu */ {IDS_MEMORY_HU, "Memória"},
/* it */ {IDS_MEMORY_IT, "Memoria"},
/* nl */ {IDS_MEMORY_NL, "Geheugen"},
/* pl */ {IDS_MEMORY_PL, ""},  /* fuzzy */
/* sv */ {IDS_MEMORY_SV, "Minne"},

/* en */ {IDS_VIDEO_SIZE,    "Video size"},
/* de */ {IDS_VIDEO_SIZE_DE, "Videospeichergröße"},
/* fr */ {IDS_VIDEO_SIZE_FR, "Taille vidéo"},
/* hu */ {IDS_VIDEO_SIZE_HU, "Videó méret"},
/* it */ {IDS_VIDEO_SIZE_IT, "Dimensione dello schermo"},
/* nl */ {IDS_VIDEO_SIZE_NL, "Videogrootte"},
/* pl */ {IDS_VIDEO_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_VIDEO_SIZE_SV, "Skärmbredd"},

/* en */ {IDS_IO_SIZE,    "I/O size"},
/* de */ {IDS_IO_SIZE_DE, "I/O Größe"},
/* fr */ {IDS_IO_SIZE_FR, "Taille d'E/S"},
/* hu */ {IDS_IO_SIZE_HU, "I/O méret"},
/* it */ {IDS_IO_SIZE_IT, "Dimensione di I/O"},
/* nl */ {IDS_IO_SIZE_NL, "I/O-grootte"},
/* pl */ {IDS_IO_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_IO_SIZE_SV, "I/O-utrymme"},

/* en */ {IDS_KEYBOARD,    "Keyboard"},
/* de */ {IDS_KEYBOARD_DE, "Tastatur"},
/* fr */ {IDS_KEYBOARD_FR, ""},  /* fuzzy */
/* hu */ {IDS_KEYBOARD_HU, "Billentyûzet"},
/* it */ {IDS_KEYBOARD_IT, "Tastiera"},
/* nl */ {IDS_KEYBOARD_NL, "Toetsenbord"},
/* pl */ {IDS_KEYBOARD_PL, ""},  /* fuzzy */
/* sv */ {IDS_KEYBOARD_SV, "Tangentbord"},

/* en */ {IDS_CRTC_CHIP,    "CRTC chip"},
/* de */ {IDS_CRTC_CHIP_DE, "CRTC Chip"},
/* fr */ {IDS_CRTC_CHIP_FR, ""},  /* fuzzy */
/* hu */ {IDS_CRTC_CHIP_HU, "CRTC csip"},
/* it */ {IDS_CRTC_CHIP_IT, "Chip CRTC"},
/* nl */ {IDS_CRTC_CHIP_NL, "CRTC-chip"},
/* pl */ {IDS_CRTC_CHIP_PL, ""},  /* fuzzy */
/* sv */ {IDS_CRTC_CHIP_SV, "CRTC-krets"},

/* en */ {IDS_SUPERPET_IO,    "SuperPET I/O enable (disables 8x96)"},
/* de */ {IDS_SUPERPET_IO_DE, "SuperPET I/O aktivieren (deaktiviert 8x96)"},
/* fr */ {IDS_SUPERPET_IO_FR, ""},  /* fuzzy */
/* hu */ {IDS_SUPERPET_IO_HU, "SuperPET I/O engedélyezése (8x96-ot tiltja)"},
/* it */ {IDS_SUPERPET_IO_IT, "Attiva I/O del SuperPET (disattiva 8x96)"},
/* nl */ {IDS_SUPERPET_IO_NL, "SuperPET-I/O activeren (zet 8x96 uit)"},
/* pl */ {IDS_SUPERPET_IO_PL, ""},  /* fuzzy */
/* sv */ {IDS_SUPERPET_IO_SV, "Aktivera SuperPET-I/O (inaktiverar 8x96)"},

/* en */ {IDS_9xxx_AS_RAM,    "$9*** as RAM"},
/* de */ {IDS_9xxx_AS_RAM_DE, "$9*** als RAM"},
/* fr */ {IDS_9xxx_AS_RAM_FR, ""},  /* fuzzy */
/* hu */ {IDS_9xxx_AS_RAM_HU, "$9*** RAM-ként"},
/* it */ {IDS_9xxx_AS_RAM_IT, "$9*** come RAM"},
/* nl */ {IDS_9xxx_AS_RAM_NL, "$9*** als RAM"},
/* pl */ {IDS_9xxx_AS_RAM_PL, ""},  /* fuzzy */
/* sv */ {IDS_9xxx_AS_RAM_SV, "$9*** som RAM"},

/* en */ {IDS_Axxx_AS_RAM,    "$A*** as RAM"},
/* de */ {IDS_Axxx_AS_RAM_DE, "$A*** als RAM"},
/* fr */ {IDS_Axxx_AS_RAM_FR, ""},  /* fuzzy */
/* hu */ {IDS_Axxx_AS_RAM_HU, "$A*** RAM-ként"},
/* it */ {IDS_Axxx_AS_RAM_IT, "$A*** come RAM"},
/* nl */ {IDS_Axxx_AS_RAM_NL, "$A*** als RAM"},
/* pl */ {IDS_Axxx_AS_RAM_PL, ""},  /* fuzzy */
/* sv */ {IDS_Axxx_AS_RAM_SV, "$A*** som RAM"},

/* en */ {IDS_PET_SETTINGS,    "PET settings"},
/* de */ {IDS_PET_SETTINGS_DE, "PET Einstellungen"},
/* fr */ {IDS_PET_SETTINGS_FR, "Paramètres PET"},
/* hu */ {IDS_PET_SETTINGS_HU, "PET beállításai"},
/* it */ {IDS_PET_SETTINGS_IT, "Impostazioni PET"},
/* nl */ {IDS_PET_SETTINGS_NL, "PET-instellingen"},
/* pl */ {IDS_PET_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_PET_SETTINGS_SV, "PET-inställningar"},

/* en */ {IDS_CURRENT_MODEL,    "Current Model"},
/* de */ {IDS_CURRENT_MODEL_DE, "Aktuelles Modell"},
/* fr */ {IDS_CURRENT_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDS_CURRENT_MODEL_HU, "Jelenlegi modell"},
/* it */ {IDS_CURRENT_MODEL_IT, "Modello attuale"},
/* nl */ {IDS_CURRENT_MODEL_NL, "Huidig model"},
/* pl */ {IDS_CURRENT_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDS_CURRENT_MODEL_SV, "Aktuell modell"},

/* en */ {IDS_PET_MODEL,    "PET Model"},
/* de */ {IDS_PET_MODEL_DE, "PET Modell"},
/* fr */ {IDS_PET_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDS_PET_MODEL_HU, "PET modell"},
/* it */ {IDS_PET_MODEL_IT, "Modello PET"},
/* nl */ {IDS_PET_MODEL_NL, "PET-model"},
/* pl */ {IDS_PET_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDS_PET_MODEL_SV, "PET-modell"},

/* en */ {IDS_PLUS4_SETTINGS,    "PLUS4 settings"},
/* de */ {IDS_PLUS4_SETTINGS_DE, "PLUS4 Einstellungen"},
/* fr */ {IDS_PLUS4_SETTINGS_FR, "Paramètres PLUS4"},
/* hu */ {IDS_PLUS4_SETTINGS_HU, "PLUS4 beállításai"},
/* it */ {IDS_PLUS4_SETTINGS_IT, "Impostazioni PLUS4"},
/* nl */ {IDS_PLUS4_SETTINGS_NL, "PLUS4-instellingen"},
/* pl */ {IDS_PLUS4_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS4_SETTINGS_SV, "PLUS4-inställningar"},

/* en */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME,    "Select Internal Function ROM file"},
/* de */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_DE, "Internes Funktions ROM Image Datei selektieren"},
/* fr */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_HU, "Adja meg a belsõ Function ROM képmás nevét"},
/* it */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_IT, "Seleziona il file della function ROM interna"},
/* nl */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_NL, "Selecteer Intern functie-ROM-bestand"},
/* pl */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_SV, "Välj avbildning för internt funktions-ROM"},

/* en */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME,    "Select External Function ROM file"},
/* de */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_DE, "Externes Funktions ROM Image Datei selektieren"},
/* fr */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_HU, "Adja meg a külsõ Function ROM képmás nevét"},
/* it */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_IT, "Seleziona il file della function ROM esterna"},
/* nl */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_NL, "Selecteer Extern functie-ROM-bestand"},
/* pl */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_SV, "Välj avbildning för externt funktions-ROM"},

/* en */ {IDS_MACHINE_TYPE,    "Machine type"},
/* de */ {IDS_MACHINE_TYPE_DE, "Maschinen Typ"},
/* fr */ {IDS_MACHINE_TYPE_FR, "Type de machine"},
/* hu */ {IDS_MACHINE_TYPE_HU, "Gép típus"},
/* it */ {IDS_MACHINE_TYPE_IT, "Tipo di macchina"},
/* nl */ {IDS_MACHINE_TYPE_NL, "Machinesoort"},
/* pl */ {IDS_MACHINE_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDS_MACHINE_TYPE_SV, "Maskintyp"},

/* en */ {IDS_INTERNAL_FUNCTION_ROM,    "Internal Function ROM"},
/* de */ {IDS_INTERNAL_FUNCTION_ROM_DE, "Internes Funktions ROM"},
/* fr */ {IDS_INTERNAL_FUNCTION_ROM_FR, "Image ROM Fonction interne"},
/* hu */ {IDS_INTERNAL_FUNCTION_ROM_HU, "Belsõ Function ROM"},
/* it */ {IDS_INTERNAL_FUNCTION_ROM_IT, "Function ROM interna"},
/* nl */ {IDS_INTERNAL_FUNCTION_ROM_NL, "Intern functie-ROM"},
/* pl */ {IDS_INTERNAL_FUNCTION_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDS_INTERNAL_FUNCTION_ROM_SV, "Internt funktions-ROM"},

/* en */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME,    "Internal Function ROM file"},
/* de */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_DE, "Interne Funktions ROM Datei"},
/* fr */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_HU, "Belsõ Function ROM fájl"},
/* it */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_IT, "File della function ROM interna"},
/* nl */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_NL, "Intern functie-ROM-bestand"},
/* pl */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_SV, "Internt funktions-ROM"},

/* en */ {IDS_EXTERNAL_FUNCTION_ROM,    "External Function ROM"},
/* de */ {IDS_EXTERNAL_FUNCTION_ROM_DE, "Externes Funktions ROM"},
/* fr */ {IDS_EXTERNAL_FUNCTION_ROM_FR, "Image ROM Fonction externe"},
/* hu */ {IDS_EXTERNAL_FUNCTION_ROM_HU, "Külsõ Function ROM"},
/* it */ {IDS_EXTERNAL_FUNCTION_ROM_IT, "Function ROM esterna"},
/* nl */ {IDS_EXTERNAL_FUNCTION_ROM_NL, "Extern functie-ROM"},
/* pl */ {IDS_EXTERNAL_FUNCTION_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXTERNAL_FUNCTION_ROM_SV, "Externt funktions-ROM"},

/* en */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME,    "External Function ROM file"},
/* de */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_DE, "Externe Funktions ROM Datei"},
/* fr */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_HU, "Külsõ Function ROM fájl"},
/* it */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_IT, "File della function ROM esterna"},
/* nl */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_NL, "Extern functie-ROM-bestand"},
/* pl */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_SV, "Externt funktions-ROM"},

/* en */ {IDS_C128_SETTINGS,    "C128 settings"},
/* de */ {IDS_C128_SETTINGS_DE, "C128 Einstellungen"},
/* fr */ {IDS_C128_SETTINGS_FR, "Paramètres C128"},
/* hu */ {IDS_C128_SETTINGS_HU, "C128 beállításai"},
/* it */ {IDS_C128_SETTINGS_IT, "Impostazioni C128"},
/* nl */ {IDS_C128_SETTINGS_NL, "C128-instellingen"},
/* pl */ {IDS_C128_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_C128_SETTINGS_SV, "C128-inställningar"},

/* en */ {IDS_SELECT_ROM_FILE,    "Select ROM file"},
/* de */ {IDS_SELECT_ROM_FILE_DE, "ROM Datei wählen"},
/* fr */ {IDS_SELECT_ROM_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_ROM_FILE_HU, "Válasszon ROM fájlt"},
/* it */ {IDS_SELECT_ROM_FILE_IT, "Seleziona file della ROM"},
/* nl */ {IDS_SELECT_ROM_FILE_NL, "Selecteer ROM-bestand"},
/* pl */ {IDS_SELECT_ROM_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_ROM_FILE_SV, "Välj ROM-fil"},

/* en */ {IDS_COMPUTER_ROM_SETTINGS,    "Computer ROM settings"},
/* de */ {IDS_COMPUTER_ROM_SETTINGS_DE, "Computer ROM Einstellungen"},
/* fr */ {IDS_COMPUTER_ROM_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_COMPUTER_ROM_SETTINGS_HU, "Számítógép ROM beállításai"},
/* it */ {IDS_COMPUTER_ROM_SETTINGS_IT, "Impostazioni ROM del computer"},
/* nl */ {IDS_COMPUTER_ROM_SETTINGS_NL, "Computer-ROM-instellingen"},
/* pl */ {IDS_COMPUTER_ROM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_COMPUTER_ROM_SETTINGS_SV, "ROM-inställningar för dator"},

/* en */ {IDS_DRIVE_ROM_SETTINGS,    "Drive ROM settings"},
/* de */ {IDS_DRIVE_ROM_SETTINGS_DE, "Floppy ROM Einstellungen"},
/* fr */ {IDS_DRIVE_ROM_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_DRIVE_ROM_SETTINGS_HU, "Lemezegység ROM beállításai"},
/* it */ {IDS_DRIVE_ROM_SETTINGS_IT, "Impostazioni ROM del drive"},
/* nl */ {IDS_DRIVE_ROM_SETTINGS_NL, "Drive-ROM-instellingen"},
/* pl */ {IDS_DRIVE_ROM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_DRIVE_ROM_SETTINGS_SV, "ROM-inställningar för diskettenhet"},

/* en */ {IDS_DIGIMAX_ENABLED,    "Digimax Enabled"},
/* de */ {IDS_DIGIMAX_ENABLED_DE, "Digimax aktiviert"},
/* fr */ {IDS_DIGIMAX_ENABLED_FR, ""},  /* fuzzy */
/* hu */ {IDS_DIGIMAX_ENABLED_HU, "Digimax engedélyezve"},
/* it */ {IDS_DIGIMAX_ENABLED_IT, "Digimax attivato"},
/* nl */ {IDS_DIGIMAX_ENABLED_NL, "Digimax Aan/Uit"},
/* pl */ {IDS_DIGIMAX_ENABLED_PL, ""},  /* fuzzy */
/* sv */ {IDS_DIGIMAX_ENABLED_SV, "Digimax aktiverad"},

/* en */ {IDS_DIGIMAX_BASE,    "Digimax Base"},
/* de */ {IDS_DIGIMAX_BASE_DE, "Digimax Basis"},
/* fr */ {IDS_DIGIMAX_BASE_FR, ""},  /* fuzzy */
/* hu */ {IDS_DIGIMAX_BASE_HU, "Digimax báziscím"},
/* it */ {IDS_DIGIMAX_BASE_IT, "Base Digimax"},
/* nl */ {IDS_DIGIMAX_BASE_NL, "Digimax basisadres"},
/* pl */ {IDS_DIGIMAX_BASE_PL, ""},  /* fuzzy */
/* sv */ {IDS_DIGIMAX_BASE_SV, "Digimaxbas"},

/* en */ {IDS_DIGIMAX_SETTINGS,    "Digimax Settings"},
/* de */ {IDS_DIGIMAX_SETTINGS_DE, "Digimax Einstellungen"},
/* fr */ {IDS_DIGIMAX_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_DIGIMAX_SETTINGS_HU, "Digimax beállításai"},
/* it */ {IDS_DIGIMAX_SETTINGS_IT, "Impostazioni digimax"},
/* nl */ {IDS_DIGIMAX_SETTINGS_NL, "Digimaxinstellingen"},
/* pl */ {IDS_DIGIMAX_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_DIGIMAX_SETTINGS_SV, "Digimaxinställningar"},

/* en */ {IDS_SOUND_RECORDING_STOPPED,    "Sound Recording stopped..."},
/* de */ {IDS_SOUND_RECORDING_STOPPED_DE, "Tonaufnahme gestoppt..."},
/* fr */ {IDS_SOUND_RECORDING_STOPPED_FR, ""},  /* fuzzy */
/* hu */ {IDS_SOUND_RECORDING_STOPPED_HU, "Hangfelvétel leállítva..."},
/* it */ {IDS_SOUND_RECORDING_STOPPED_IT, "Registrazione audio interrotta..."},
/* nl */ {IDS_SOUND_RECORDING_STOPPED_NL, "Geluidsopname gestopt..."},
/* pl */ {IDS_SOUND_RECORDING_STOPPED_PL, ""},  /* fuzzy */
/* sv */ {IDS_SOUND_RECORDING_STOPPED_SV, "Ljudinspelning stoppad..."},

/* en */ {IDS_SOUND_RECORDING_STARTED,    "Sound Recording started..."},
/* de */ {IDS_SOUND_RECORDING_STARTED_DE, "Tonaufnahme gestartet..."},
/* fr */ {IDS_SOUND_RECORDING_STARTED_FR, ""},  /* fuzzy */
/* hu */ {IDS_SOUND_RECORDING_STARTED_HU, "Hangfelvétel elindítva..."},
/* it */ {IDS_SOUND_RECORDING_STARTED_IT, "Registrazione audio avviata..."},
/* nl */ {IDS_SOUND_RECORDING_STARTED_NL, "Geluidsopname gestart..."},
/* pl */ {IDS_SOUND_RECORDING_STARTED_PL, ""},  /* fuzzy */
/* sv */ {IDS_SOUND_RECORDING_STARTED_SV, "Ljudinspelning startad..."},

/* en */ {IDS_SELECT_RECORD_FILE,    "Select Sound Record File"},
/* de */ {IDS_SELECT_RECORD_FILE_DE, "Tonaufnahmedatei definieren"},
/* fr */ {IDS_SELECT_RECORD_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_RECORD_FILE_HU, "Válassza ki a hangfelvétel kimeneti fájlt"},
/* it */ {IDS_SELECT_RECORD_FILE_IT, "Seleziona il file per la registrazione dell'audio"},
/* nl */ {IDS_SELECT_RECORD_FILE_NL, "Selecteer geluidsopnamebestand"},
/* pl */ {IDS_SELECT_RECORD_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_RECORD_FILE_SV, "Välj ljudinspelningsfil"},

/* en */ {IDS_SOUND_RECORD_FORMAT,    "Sound Record Format"},
/* de */ {IDS_SOUND_RECORD_FORMAT_DE, "Tonaufnahmeformat"},
/* fr */ {IDS_SOUND_RECORD_FORMAT_FR, ""},  /* fuzzy */
/* hu */ {IDS_SOUND_RECORD_FORMAT_HU, "Hangfelvétel formátuma"},
/* it */ {IDS_SOUND_RECORD_FORMAT_IT, "Formato di registrazione dell'audio"},
/* nl */ {IDS_SOUND_RECORD_FORMAT_NL, "Geluid Opname Formaat"},
/* pl */ {IDS_SOUND_RECORD_FORMAT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SOUND_RECORD_FORMAT_SV, "Format för ljudinspelning"},

/* en */ {IDS_SOUND_RECORD_FILE,    "Sound Record File"},
/* de */ {IDS_SOUND_RECORD_FILE_DE, "Tonaufnahmedatei"},
/* fr */ {IDS_SOUND_RECORD_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SOUND_RECORD_FILE_HU, "Hangfelvétel fájl"},
/* it */ {IDS_SOUND_RECORD_FILE_IT, "File per la registrazione dell'audio"},
/* nl */ {IDS_SOUND_RECORD_FILE_NL, "Geluid opnamebestand"},
/* pl */ {IDS_SOUND_RECORD_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SOUND_RECORD_FILE_SV, "Ljudinspelningsfil"},

/* en */ {IDS_SOUND_RECORD_SETTINGS,    "Sound Record Settings"},
/* de */ {IDS_SOUND_RECORD_SETTINGS_DE, "Tonaufnahme Einstellungen"},
/* fr */ {IDS_SOUND_RECORD_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_SOUND_RECORD_SETTINGS_HU, "Hangfelvétel beállításai"},
/* it */ {IDS_SOUND_RECORD_SETTINGS_IT, "Impostazioni di registrazione audio"},
/* nl */ {IDS_SOUND_RECORD_SETTINGS_NL, "Geluidsopnameinstellingen"},
/* pl */ {IDS_SOUND_RECORD_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SOUND_RECORD_SETTINGS_SV, "Inställningar för ljudinspelning"},

/* en */ {IDS_BORDER_MODE,    "Border mode"},
/* de */ {IDS_BORDER_MODE_DE, "Rahmen Modus"},
/* fr */ {IDS_BORDER_MODE_FR, ""},  /* fuzzy */
/* hu */ {IDS_BORDER_MODE_HU, "Keret mód"},
/* it */ {IDS_BORDER_MODE_IT, "Modalità del bordo"},
/* nl */ {IDS_BORDER_MODE_NL, "Bordermodus"},
/* pl */ {IDS_BORDER_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDS_BORDER_MODE_SV, "Ramläge"},

/* en */ {IDMS_NORMAL,    "Normal"},
/* de */ {IDMS_NORMAL_DE, "Normal"},
/* fr */ {IDMS_NORMAL_FR, ""},  /* fuzzy */
/* hu */ {IDMS_NORMAL_HU, "Normál"},
/* it */ {IDMS_NORMAL_IT, "Normale"},
/* nl */ {IDMS_NORMAL_NL, "Normaal"},
/* pl */ {IDMS_NORMAL_PL, ""},  /* fuzzy */
/* sv */ {IDMS_NORMAL_SV, "Normal"},

/* en */ {IDS_FULL,    "Full"},
/* de */ {IDS_FULL_DE, "Full"},
/* fr */ {IDS_FULL_FR, ""},  /* fuzzy */
/* hu */ {IDS_FULL_HU, "Teljes"},
/* it */ {IDS_FULL_IT, "Intero"},
/* nl */ {IDS_FULL_NL, "Volledig"},
/* pl */ {IDS_FULL_PL, ""},  /* fuzzy */
/* sv */ {IDS_FULL_SV, "Full"},

/* en */ {IDS_DEBUG,    "Debug"},
/* de */ {IDS_DEBUG_DE, "Debug"},
/* fr */ {IDS_DEBUG_FR, ""},  /* fuzzy */
/* hu */ {IDS_DEBUG_HU, "Nyomkövetés"},
/* it */ {IDS_DEBUG_IT, "Debug"},
/* nl */ {IDS_DEBUG_NL, "Debug"},
/* pl */ {IDS_DEBUG_PL, ""},  /* fuzzy */
/* sv */ {IDS_DEBUG_SV, "Felsök"},

/* en */ {IDS_VOLUME,    "Volume (0-100%)"},
/* de */ {IDS_VOLUME_DE, "Lautstärke (0-100%)"},
/* fr */ {IDS_VOLUME_FR, ""},  /* fuzzy */
/* hu */ {IDS_VOLUME_HU, "Hangerõ (0-100%)"},
/* it */ {IDS_VOLUME_IT, "Volume (0-100%)"},
/* nl */ {IDS_VOLUME_NL, "Volume (0-100%)"},
/* pl */ {IDS_VOLUME_PL, ""},  /* fuzzy */
/* sv */ {IDS_VOLUME_SV, "Volym (0-100 %)"},

/* en */ {IDS_MOUSE_TYPE,    "Mouse type"},
/* de */ {IDS_MOUSE_TYPE_DE, "Maustyp"},
/* fr */ {IDS_MOUSE_TYPE_FR, ""},  /* fuzzy */
/* hu */ {IDS_MOUSE_TYPE_HU, "Egér típusa"},
/* it */ {IDS_MOUSE_TYPE_IT, ""},  /* fuzzy */
/* nl */ {IDS_MOUSE_TYPE_NL, "Muis soort"},
/* pl */ {IDS_MOUSE_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDS_MOUSE_TYPE_SV, "Mustyp"},

/* en */ {IDS_MOUSE_SETTINGS,    "Mouse Settings"},
/* de */ {IDS_MOUSE_SETTINGS_DE, "Maus Einstellungen"},
/* fr */ {IDS_MOUSE_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_MOUSE_SETTINGS_HU, "Egér beállítások"},
/* it */ {IDS_MOUSE_SETTINGS_IT, ""},  /* fuzzy */
/* nl */ {IDS_MOUSE_SETTINGS_NL, "Muis Instellingen"},
/* pl */ {IDS_MOUSE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_MOUSE_SETTINGS_SV, "Musinställningar"},

/* en */ {IDS_MOUSE_PORT,    "Mouse port"},
/* de */ {IDS_MOUSE_PORT_DE, "Maus Port"},
/* fr */ {IDS_MOUSE_PORT_FR, ""},  /* fuzzy */
/* hu */ {IDS_MOUSE_PORT_HU, ""},  /* fuzzy */
/* it */ {IDS_MOUSE_PORT_IT, ""},  /* fuzzy */
/* nl */ {IDS_MOUSE_PORT_NL, "Muis poort"},
/* pl */ {IDS_MOUSE_PORT_PL, ""},  /* fuzzy */
/* sv */ {IDS_MOUSE_PORT_SV, ""},  /* fuzzy */

/* en */ {IDS_MOUSE_1351,    "1351 mouse"},
/* de */ {IDS_MOUSE_1351_DE, "1351 Maus"},
/* fr */ {IDS_MOUSE_1351_FR, ""},  /* fuzzy */
/* hu */ {IDS_MOUSE_1351_HU, "1351 egér"},
/* it */ {IDS_MOUSE_1351_IT, ""},  /* fuzzy */
/* nl */ {IDS_MOUSE_1351_NL, "1351 muis"},
/* pl */ {IDS_MOUSE_1351_PL, ""},  /* fuzzy */
/* sv */ {IDS_MOUSE_1351_SV, "1351-mus"},

/* en */ {IDS_MOUSE_NEOS,    "NEOS mouse"},
/* de */ {IDS_MOUSE_NEOS_DE, "NEOS Maus"},
/* fr */ {IDS_MOUSE_NEOS_FR, ""},  /* fuzzy */
/* hu */ {IDS_MOUSE_NEOS_HU, "NEOS egér"},
/* it */ {IDS_MOUSE_NEOS_IT, ""},  /* fuzzy */
/* nl */ {IDS_MOUSE_NEOS_NL, "NEOS muis"},
/* pl */ {IDS_MOUSE_NEOS_PL, ""},  /* fuzzy */
/* sv */ {IDS_MOUSE_NEOS_SV, "NEOS-mus"},

/* en */ {IDS_MOUSE_AMIGA,    "Amiga mouse"},
/* de */ {IDS_MOUSE_AMIGA_DE, "Amiga Maus"},
/* fr */ {IDS_MOUSE_AMIGA_FR, ""},  /* fuzzy */
/* hu */ {IDS_MOUSE_AMIGA_HU, "Amiga egér"},
/* it */ {IDS_MOUSE_AMIGA_IT, ""},  /* fuzzy */
/* nl */ {IDS_MOUSE_AMIGA_NL, "Amiga muis"},
/* pl */ {IDS_MOUSE_AMIGA_PL, ""},  /* fuzzy */
/* sv */ {IDS_MOUSE_AMIGA_SV, "Amigamus"},

};

#include "intl_table.h"

/* --------------------------------------------------------------------- */

static char *intl_text_table[countof(intl_translate_text_table)][countof(language_table)];

void intl_convert_mui_table(int text_id[], char *text[])
{
  int i;

  for (i=0; text_id[i]>0; i++)
  {
    text[i]=intl_translate_text(text_id[i]);
  }
  if (text_id[i]==0)
    text[i]=NULL;
}

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

  buffer=lib_stralloc(text);

  return buffer;
}

static char *intl_get_string_by_id(int id)
{
  unsigned int k;

  for (k = 0; k < countof(intl_string_table); k++)
  {
    if (intl_string_table[k].resource_id==id)
      return intl_string_table[k].text;
  }
  return NULL;
}

char *intl_translate_text(int en_resource)
{
  unsigned int i;

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
  unsigned int i,j;

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
  intl_speed_at_text=intl_translate_text(IDS_S_AT_D_SPEED);
}

char *intl_arch_language_init(void)
{
  int i;
  struct Locale *amigalang;

  amigalang=OpenLocale(NULL);
  CloseLocale(amigalang);
  for (i = 0; amiga_to_iso[i].iso_language_code != NULL; i++)
  {
    if (!strcasecmp(amiga_to_iso[i].amiga_locale_language, amigalang->loc_LanguageName))
      return amiga_to_iso[i].iso_language_code;
  }
  return "en";
}

void intl_update_ui(void)
{
  intl_update_pre_translated_text();
  ui_update_menus();
}
