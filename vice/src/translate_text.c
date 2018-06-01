/*
 * translate_text.c - Translation texts to be included in translate.c
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

translate_t string_table[] = {

/* c64/c64-cmdline-options.c, scpu64/scpu64-cmdline-options.c */
/* en */ {IDCLS_SET_BURST_MOD,    N_("Burst modification (0 = None, 1 = CIA1, 2 = CIA2)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_BURST_MOD_DA, "Burst modification (0 = ingen, 1 = CIA1, 2 = CIA2)"},
/* de */ {IDCLS_SET_BURST_MOD_DE, "Burst Modifikation (0 = Kein, 1 = CIA1, 2 = CIA2)"},
/* es */ {IDCLS_SET_BURST_MOD_ES, "Modificación arranque (0=No, 1=CIA1, 2=CIA2)"},
/* fr */ {IDCLS_SET_BURST_MOD_FR, "Modification Burst (0 = Aucune, 1 = CIA1, 2 = CIA2)"},
/* hu */ {IDCLS_SET_BURST_MOD_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_BURST_MOD_IT, "Burst modification (0 = Nessuna, 1 = CIA1, 2 = CIA2)"},
/* ko */ {IDCLS_SET_BURST_MOD_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_BURST_MOD_NL, "Burst modificatie (0 = Geen, 1 = CIA1, 2 = CIA2)"},
/* pl */ {IDCLS_SET_BURST_MOD_PL, "Modyfikacja trybu synchronicznego (0 = Brak, 1 = CIA1, 2 = CIA2)"},
/* ru */ {IDCLS_SET_BURST_MOD_RU, "Burst modification (0 = None, 1 = CIA1, 2 = CIA2)"},
/* sv */ {IDCLS_SET_BURST_MOD_SV, "Burstmodifiering (0 = ingen, 1 = CIA1, 2 = CIA2)"},
/* tr */ {IDCLS_SET_BURST_MOD_TR, "Burst deðiþikliði (0 = Yok, 1 = CIA1, 2 = CIA2)"},
#endif

/* c64/c64-cmdline-options.c, scpu64/scpu64-cmdline-options.c */
/* en */ {IDCLS_SET_IEC_RESET,    N_("Computer reset goes to IEC bus (0 = No, 1 = Yes)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_IEC_RESET_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_IEC_RESET_DE, "Computer Reset auf IEC Bus (0 = Nein, 1 = Ja)"},
/* es */ {IDCLS_SET_IEC_RESET_ES, "Reinicio del ordenador va a IEC bus (0 = No, 1 = Sí)"},
/* fr */ {IDCLS_SET_IEC_RESET_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_IEC_RESET_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_IEC_RESET_IT, "Reset del computer va sul bus IEC (0 = No, 1 = Sì)"},
/* ko */ {IDCLS_SET_IEC_RESET_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_IEC_RESET_NL, "Computer reset gaat naar de IEC bus (0 = Nee, 1 = Ja)"},
/* pl */ {IDCLS_SET_IEC_RESET_PL, "Reset komputera u¿ywa szyny IEC (0 = nie, 1 = tak)"},
/* ru */ {IDCLS_SET_IEC_RESET_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_IEC_RESET_SV, "Datornollställning går till IEC-bussen (0 = nej, 1 = ja)"},
/* tr */ {IDCLS_SET_IEC_RESET_TR, ""},  /* fuzzy */
#endif


/* plus4/plus4acia.c */
/* en */ {IDCLS_ENABLE_ACIA_EMU,    N_("Enable the ACIA emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_ACIA_EMU_DA, "Aktivér ACIA-emulering"},
/* de */ {IDCLS_ENABLE_ACIA_EMU_DE, "ACIA Emulation aktivieren"},
/* es */ {IDCLS_ENABLE_ACIA_EMU_ES, "Habilitar emulación ACIA"},
/* fr */ {IDCLS_ENABLE_ACIA_EMU_FR, "Activer l'émulation ACIA"},
/* hu */ {IDCLS_ENABLE_ACIA_EMU_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_ACIA_EMU_IT, "Attiva emulazione ACIA"},
/* ko */ {IDCLS_ENABLE_ACIA_EMU_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_ACIA_EMU_NL, "ACIA emulatie inschakelen"},
/* pl */ {IDCLS_ENABLE_ACIA_EMU_PL, "W³±cz emulacjê ACIA"},
/* ru */ {IDCLS_ENABLE_ACIA_EMU_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_ACIA_EMU_SV, "Aktivera ACIA-emuleringen"},
/* tr */ {IDCLS_ENABLE_ACIA_EMU_TR, "ACIA emülasyonunu aktif et"},
#endif

/* plus4/plus4acia.c */
/* en */ {IDCLS_DISABLE_ACIA_EMU,    N_("Disable the ACIA emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_ACIA_EMU_DA, "Deaktivér ACIA-emulering"},
/* de */ {IDCLS_DISABLE_ACIA_EMU_DE, "ACIA Emulation deaktivieren"},
/* es */ {IDCLS_DISABLE_ACIA_EMU_ES, "Deshabilitar emulación ACIA"},
/* fr */ {IDCLS_DISABLE_ACIA_EMU_FR, "Désactiver l'émulation ACIA"},
/* hu */ {IDCLS_DISABLE_ACIA_EMU_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_ACIA_EMU_IT, "Disattiva emulazione ACIA"},
/* ko */ {IDCLS_DISABLE_ACIA_EMU_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_ACIA_EMU_NL, "ACIA emulatie uitschakelen"},
/* pl */ {IDCLS_DISABLE_ACIA_EMU_PL, "Wy³±cz emulacjê ACIA"},
/* ru */ {IDCLS_DISABLE_ACIA_EMU_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_ACIA_EMU_SV, "Inaktivera ACIA-emuleringen"},
/* tr */ {IDCLS_DISABLE_ACIA_EMU_TR, "ACIA emülasyonunu pasifleþtir"},
#endif

/* c128/c128mmu.c */
/* en */ {IDCLS_ACTIVATE_40_COL_MODE,    N_("Activate 40 column mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ACTIVATE_40_COL_MODE_DA, "Aktivér 40-kolonners tilstand"},
/* de */ {IDCLS_ACTIVATE_40_COL_MODE_DE, "40 Spalten Modus aktivieren"},
/* es */ {IDCLS_ACTIVATE_40_COL_MODE_ES, "Activar modo 40 columnas"},
/* fr */ {IDCLS_ACTIVATE_40_COL_MODE_FR, "Activer le mode 40 colonnes"},
/* hu */ {IDCLS_ACTIVATE_40_COL_MODE_HU, "40 oszlopos mód aktiválása"},
/* it */ {IDCLS_ACTIVATE_40_COL_MODE_IT, "Attiva modalità a 40 colonne"},
/* ko */ {IDCLS_ACTIVATE_40_COL_MODE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ACTIVATE_40_COL_MODE_NL, "Activeer 40 kolomsmodus"},
/* pl */ {IDCLS_ACTIVATE_40_COL_MODE_PL, "W³±cz tryb 40 kolumn"},
/* ru */ {IDCLS_ACTIVATE_40_COL_MODE_RU, "°ÚâØÒØàÞÒÐâì àÕÖØÜ 40 ÚÞÛÞÝÞÚ"},
/* sv */ {IDCLS_ACTIVATE_40_COL_MODE_SV, "Aktivera 40-kolumnersläge"},
/* tr */ {IDCLS_ACTIVATE_40_COL_MODE_TR, "40 sütun modunu aktif et"},
#endif

/* c128/c128mmu.c */
/* en */ {IDCLS_ACTIVATE_80_COL_MODE,    N_("Activate 80 column mode")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ACTIVATE_80_COL_MODE_DA, "Aktivér 80-kolonners tilstand"},
/* de */ {IDCLS_ACTIVATE_80_COL_MODE_DE, "80 Spalten Modus aktivieren"},
/* es */ {IDCLS_ACTIVATE_80_COL_MODE_ES, "Activar modo 80 columnas"},
/* fr */ {IDCLS_ACTIVATE_80_COL_MODE_FR, "Activer le mode 80 colonnes"},
/* hu */ {IDCLS_ACTIVATE_80_COL_MODE_HU, "80 oszlopos mód aktiválása"},
/* it */ {IDCLS_ACTIVATE_80_COL_MODE_IT, "Attiva modalità a 80 colonne"},
/* ko */ {IDCLS_ACTIVATE_80_COL_MODE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ACTIVATE_80_COL_MODE_NL, "Activeer 80 kolomsmodus"},
/* pl */ {IDCLS_ACTIVATE_80_COL_MODE_PL, "W³±cz tryb 80 kolumn"},
/* ru */ {IDCLS_ACTIVATE_80_COL_MODE_RU, "°ÚâØÒØàÞÒÐâì àÕÖØÜ 80 ÚÞÛÞÝÞÚ"},
/* sv */ {IDCLS_ACTIVATE_80_COL_MODE_SV, "Aktivera 80-kolumnersläge"},
/* tr */ {IDCLS_ACTIVATE_80_COL_MODE_TR, "80 sütun modunu aktif et"},
#endif

/* c128/c128mmu.c */
/* en */ {IDCLS_GO64_MODE,    N_("Always switch to C64 mode on reset")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_GO64_MODE_DA, "Skift altid til C64-tilstand ved reset"},
/* de */ {IDCLS_GO64_MODE_DE, "Immer bei Reset in C64 Modus schalten"},
/* es */ {IDCLS_GO64_MODE_ES, "Al reiniciar, pasar siempre al modo C64"},
/* fr */ {IDCLS_GO64_MODE_FR, "Toujours revenir au mode C64 au redémarrage"},
/* hu */ {IDCLS_GO64_MODE_HU, "Mindig C64 módban újraindítás után"},
/* it */ {IDCLS_GO64_MODE_IT, "Vai sempre in modalità C64 al reset"},
/* ko */ {IDCLS_GO64_MODE_KO, "´Ù½Ã ½ÃÀÛÇÏ¸é Ç×»ó C64 ¸ðµå ·Î ¹Ù²ò´Ï´Ù"},
/* nl */ {IDCLS_GO64_MODE_NL, "Start altijd op in C64 modus na reset"},
/* pl */ {IDCLS_GO64_MODE_PL, "Przy restarcie zawsze prze³±czaj w tryb C64"},
/* ru */ {IDCLS_GO64_MODE_RU, "Always switch to C64 mode on reset"},
/* sv */ {IDCLS_GO64_MODE_SV, "Växla alltid till C64-läge vid återställning"},
/* tr */ {IDCLS_GO64_MODE_TR, "Resetlendiðinde her zaman C64 moduna geç"},
#endif

/* c128/c128mmu.c */
/* en */ {IDCLS_GO128_MODE,    N_("Always switch to C128 mode on reset")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_GO128_MODE_DA, "Skift altid til C128-tilstand ved reset"},
/* de */ {IDCLS_GO128_MODE_DE, "Immer be Reset in C128 Modus schalten"},
/* es */ {IDCLS_GO128_MODE_ES, "Al reiniciar, pasar siempre al modo C128"},
/* fr */ {IDCLS_GO128_MODE_FR, "Toujours revenir au mode C128 au redémarrage"},
/* hu */ {IDCLS_GO128_MODE_HU, "Mindig C128 mód újraindítás után"},
/* it */ {IDCLS_GO128_MODE_IT, "Vai sempre in modalità C128 al reset"},
/* ko */ {IDCLS_GO128_MODE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_GO128_MODE_NL, "Start altijd op in C128 modus na reset"},
/* pl */ {IDCLS_GO128_MODE_PL, "Przy restarcie zawsze prze³±czaj w tryb C128"},
/* ru */ {IDCLS_GO128_MODE_RU, "²áÕÓÔÐ ßÕàÕÚÛîçÐâì Ò àÕÖØÜ C128 ßÞáÛÕ ßÕàÕ×ÐßãáÚÐ"},
/* sv */ {IDCLS_GO128_MODE_SV, "Växla alltid till C128-läge vid återställning"},
/* tr */ {IDCLS_GO128_MODE_TR, "Resetlendiðinde her zaman C128 moduna geç"},
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME,    N_("Specify name of internal Function ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_DA, "Angiv navn på internt funktions-ROM-image"},
/* de */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_DE, "Name der internen Funktions ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_ES, "Especificar nombre imagen ROM función interna"},
/* fr */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_FR, "Spécifier le nom de l'image ROM des fonctions internes"},
/* hu */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_HU, "Adja meg a belsõ Function ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_IT, "Specifica il nome immagine della Function ROM interna"},
/* ko */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_NL, "Geef de naam van het intern functie ROM bestand"},
/* pl */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_PL, "Okre¶l nazwê obrazu wewnêtrznego Function ROM-u"},
/* ru */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_RU, "Specify name of internal Function ROM image"},
/* sv */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_SV, "Ange namn på intern funktions-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_INT_FUNC_ROM_NAME_TR, "Dahili Function ROM imajýnýn ismini belirt"},
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME,    N_("Specify name of external Function ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_DA, "Angiv navn på externt funktions-ROM-image"},
/* de */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_DE, "Name der externen Funktions ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_ES, "Especificar nombre imagen ROM función externa"},
/* fr */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_FR, "Spécifier le nom de l'image ROM des fonctions externes"},
/* hu */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_HU, "Adja meg a külsõ Function ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_IT, "Specifica il nome immagine della Function ROM esterna"},
/* ko */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_NL, "Geef de naam van het extern functie ROM bestand"},
/* pl */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_PL, "Okre¶l nazwê obrazu zewnêtrznego Function ROM-u"},
/* ru */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_RU, "Specify name of external Function ROM image"},
/* sv */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_SV, "Ange namn på extern funktions-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_TR, "Harici Function ROM imajýnýn ismini belirt"},
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_INT_FUNC_ROM,    N_("Type of internal Function ROM: (0: None, 1: ROM, 2: RAM, 3: RTC)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_INT_FUNC_ROM_DA, "Typen af intern funktions-ROM: (0: Ingen, 1: ROM, 2: RAM, 3:RTC)"},
/* de */ {IDCLS_ENABLE_INT_FUNC_ROM_DE, "Typ des internen Funktions ROM: (0: Kein, 1: ROM, 2: RAM, 3: RTC)"},
/* es */ {IDCLS_ENABLE_INT_FUNC_ROM_ES, "Tipo de Function ROM interna: (0: None, 1: ROM, 2: RAM, 3: RTC)"},
/* fr */ {IDCLS_ENABLE_INT_FUNC_ROM_FR, "Type de ROM Fonction interne: (0: Aucun, 1: ROM, 2: RAM, 3: RTC)"},
/* hu */ {IDCLS_ENABLE_INT_FUNC_ROM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_INT_FUNC_ROM_IT, "Tipo di Function ROM interna: (0: None, 1: ROM, 2: RAM, 3: RTC)"},
/* ko */ {IDCLS_ENABLE_INT_FUNC_ROM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_INT_FUNC_ROM_NL, "Intern functie ROM soort: (0: Geen, 1: ROM, 2: RAM, 3: RTC)"},
/* pl */ {IDCLS_ENABLE_INT_FUNC_ROM_PL, "Typ wewnêtrznego Function ROM-u: (0: ¯aden, 1: ROM, 2: RAM, 3: RTC)"},
/* ru */ {IDCLS_ENABLE_INT_FUNC_ROM_RU, "Type of internal Function ROM: (0: None, 1: ROM, 2: RAM, 3: RTC)"},
/* sv */ {IDCLS_ENABLE_INT_FUNC_ROM_SV, "Typ för intern funktions-ROM (0: ingen, 1: ROM, 2: RAM, 3: RTC)"},
/* tr */ {IDCLS_ENABLE_INT_FUNC_ROM_TR, "Dahili Function ROM Tipi: (0: Yok, 1: ROM, 2: RAM, 3: RTC)"},
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_EXT_FUNC_ROM,    N_("Type of external Function ROM: (0: None, 1: ROM, 2: RAM, 3: RTC)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_EXT_FUNC_ROM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_EXT_FUNC_ROM_DE, "Typ des esternen Funktions ROM: (0: Kein, 1: ROM, 2: RAM, 3: RTC)"},
/* es */ {IDCLS_ENABLE_EXT_FUNC_ROM_ES, "Tipo de Function ROM externa: (0: None, 1: ROM, 2: RAM, 3: RTC)"},
/* fr */ {IDCLS_ENABLE_EXT_FUNC_ROM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_EXT_FUNC_ROM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_EXT_FUNC_ROM_IT, "Tipo di Function ROM esterna: (0: None, 1: ROM, 2: RAM, 3: RTC)"},
/* ko */ {IDCLS_ENABLE_EXT_FUNC_ROM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_EXT_FUNC_ROM_NL, "Extern functie ROM soort: (0: Geen, 1: ROM, 2: RAM, 3: RTC)"},
/* pl */ {IDCLS_ENABLE_EXT_FUNC_ROM_PL, "Typ zewnêtrznego Function ROM-u: (0: ¯aden, 1: ROM, 2: RAM, 3: RTC)"},
/* ru */ {IDCLS_ENABLE_EXT_FUNC_ROM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EXT_FUNC_ROM_SV, "Typ för extern funktions-ROM (0: ingen, 1: ROM, 2: RAM, 3: RTC)"},
/* tr */ {IDCLS_ENABLE_EXT_FUNC_ROM_TR, ""},  /* fuzzy */
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE,    N_("Enable saving of the internal function RTC data when changed.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE_DE, "Interne Funktions RTC Daten bei Änderung speichern."},
/* es */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE_ES, "Permitir grabar datos de Internal Function RTC cuando cambien"},
/* fr */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE_IT, "Attiva il salvataggio dei dati RTC della internal function alla modifica."},
/* ko */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE_NL, "Opslaan van de interne functie RTC gegevens bij wijziging inschakelen."},
/* pl */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE_PL, "Zapisuj dane zegara czasu rzeczywistego wewnêtrznego Function ROM-u przy zmianie"},
/* ru */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_INT_FUNC_RTC_SAVE_TR, ""},  /* fuzzy */
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE,    N_("Disable saving of the internal function RTC data when changed.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE_DE, "Interne Funktions RTC Daten bei Änderung nicht speichern."},
/* es */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE_ES, "Deshabilitar grabar datos de Internal Function RTC cuando cambien"},
/* fr */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE_IT, "Disattiva il salvataggio dei dati RTC della internal function alla modifica."},
/* ko */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE_NL, "Opslaan van de interne functie RTC gegevens bij wijziging uitschakelen."},
/* pl */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE_PL, "Nie zapisuj danych zegara czasu rzeczywistego wewnêtrznego Function ROM-u przy zmianie"},
/* ru */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_INT_FUNC_RTC_SAVE_TR, ""},  /* fuzzy */
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE,    N_("Enable saving of the external function RTC data when changed.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE_DE, "Externe Funktions RTC Daten bei Änderung speichern."},
/* es */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE_ES, "Permitir grabar datos de External Function RTC cuando cambien"},
/* fr */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE_IT, "Attiva il salvataggio dei dati RTC della external function alla modifica."},
/* ko */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE_NL, "Opslaan van de externe functie RTC gegevens bij wijziging inschakelen."},
/* pl */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE_PL, "Zapisuj dane zegara czasu rzeczywistego zewnêtrznego Function ROM-u przy zmianie"},
/* ru */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_EXT_FUNC_RTC_SAVE_TR, ""},  /* fuzzy */
#endif

/* c128/functionrom.c */
/* en */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE,    N_("Disable saving of the external function RTC data when changed.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE_DE, "Externe Funktions RTC Daten bei Änderung nicht speichern."},
/* es */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE_ES, "Deshabilitar grabar datos de External Function RTC cuando cambien"},
/* fr */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE_IT, "Disattiva il salvataggio dei dati RTC della external function alla modifica."},
/* ko */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE_NL, "Opslaan van de externe functie RTC gegevens bij wijziging uitschakelen."},
/* pl */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE_PL, "Nie zapisuj danych zegara czasu rzeczywistego zewnêtrznego Function ROM-u przy zmianie"},
/* ru */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_EXT_FUNC_RTC_SAVE_TR, ""},  /* fuzzy */
#endif

/* c64/c64-cmdline-options.c, c64/psid.c, scpu64/scpu64-cmdline-options.c */
/* en */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR,    N_("Use old NTSC sync factor")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_DA, "Anvend gammel NTSC-synkfaktor"},
/* de */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_DE, "Alten NTSC Sync Faktor verwenden"},
/* es */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_ES, "Usar antiguo factor de sincronización NTSC"},
/* fr */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_FR, "Utiliser l'ancien mode NTSC"},
/* hu */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_HU, "Régi NTSC szinkron faktor használata"},
/* it */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_IT, "Usa il vecchio fattore di sincronizzazione NTSC"},
/* ko */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_NL, "Gebruik oude NTSC synchronisatiefactor"},
/* pl */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_PL, "U¿ywaj starej synchronizacji NTSC"},
/* ru */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_RU, "Use old NTSC sync factor"},
/* sv */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_SV, "Använd gammal NTSC-synkfaktor"},
/* tr */ {IDCLS_USE_OLD_NTSC_SYNC_FACTOR_TR, "Eski NTSC senkron faktörü kullan"},
#endif

/* c64/c64-cmdline-options.c */
/* en */ {IDCLS_SET_C64_MODEL,    N_("Set C64 model (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs, pet64, ultimax)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_C64_MODEL_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_C64_MODEL_DE, "Wähle C64 Modell (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs, pet64, ultimax)"},
/* es */ {IDCLS_SET_C64_MODEL_ES, "Seleccionar modelo C64 (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs, pet64, ultimax)"},
/* fr */ {IDCLS_SET_C64_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_C64_MODEL_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_C64_MODEL_IT, "Imposta modello C64 (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs, pet64, ultimax)"},
/* ko */ {IDCLS_SET_C64_MODEL_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_C64_MODEL_NL, "Stel C64 model in (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs, pet64, ultimax)"},
/* pl */ {IDCLS_SET_C64_MODEL_PL, "Wybierz model C64 (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs, pet64, ultimax)"},
/* ru */ {IDCLS_SET_C64_MODEL_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_C64_MODEL_SV, "Välj C64-modell (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs, pet64, ultimax)"},
/* tr */ {IDCLS_SET_C64_MODEL_TR, ""},  /* fuzzy */
#endif

/* scpu64/scpu64-cmdline-options.c */
/* en */ {IDCLS_SET_SCPU64_MODEL,    N_("Set C64 model (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_SCPU64_MODEL_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_SCPU64_MODEL_DE, "Wähle C64 Modell (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs)"},
/* es */ {IDCLS_SET_SCPU64_MODEL_ES, "Seleccionar modelo C64 (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs)"},
/* fr */ {IDCLS_SET_SCPU64_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_SCPU64_MODEL_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_SCPU64_MODEL_IT, "Imposta modello C64 (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs)"},
/* ko */ {IDCLS_SET_SCPU64_MODEL_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_SCPU64_MODEL_NL, "Stel C64 model in (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs)"},
/* pl */ {IDCLS_SET_SCPU64_MODEL_PL, "Wybierz model C64 (c64/c64c/c64old, ntsc/newntsc/oldntsc, drean, jap, c64gs)"},
/* ru */ {IDCLS_SET_SCPU64_MODEL_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SCPU64_MODEL_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SET_SCPU64_MODEL_TR, ""},  /* fuzzy */
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SET_PLUS4_MODEL,    N_("Set Plus4 model (c16/c16pal/c16ntsc, plus4/plus4pal/plus4ntsc, v364/cv364, c232)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_PLUS4_MODEL_DA, "Angiv Plus4-modem (c16/c16pal/c16ntsc, plus4/plus4pal/plus4ntsc, v364/cv364, c232)"},
/* de */ {IDCLS_SET_PLUS4_MODEL_DE, "Setze Plus4 Modell (c16/c16pal/c16ntsc, plus4/plus4pal/plus4ntsc, v364/cv364, c232)"},
/* es */ {IDCLS_SET_PLUS4_MODEL_ES, "Seleccionar Plus4 model (c16/c16pal/c16ntsc, plus4/plus4pal/plus4ntsc, v364/cv364, c232)"},
/* fr */ {IDCLS_SET_PLUS4_MODEL_FR, "Définir le modèle Plus4 (c16/c16pal/c16ntsc, plus4/plus4pal/plus4ntsc, v364/cv364, c232)"},
/* hu */ {IDCLS_SET_PLUS4_MODEL_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_PLUS4_MODEL_IT, "Imposta modello Plus4 (c16/c16pal/c16ntsc, plus4/plus4pal/plus4ntsc, v364/cv364, c232)"},
/* ko */ {IDCLS_SET_PLUS4_MODEL_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_PLUS4_MODEL_NL, "Stel Plus4 model in (c16/c16pal/c16ntsc, plus4/plus4pal/plus4ntsc, v364/cv364, c232)"},
/* pl */ {IDCLS_SET_PLUS4_MODEL_PL, "Ustaw model Plus4 (c16/c16pal/c16ntsc, plus4/plus4pal/plus4ntsc, v364/cv364, c232)"},
/* ru */ {IDCLS_SET_PLUS4_MODEL_RU, "Set Plus4 model (c16/c16pal/c16ntsc, plus4/plus4pal/plus4ntsc, v364/cv364, c232)"},
/* sv */ {IDCLS_SET_PLUS4_MODEL_SV, "Välj Plus4-modell (c16/c16pal/c16ntsc, plus4/plus4pal/plus4ntsc, v364/cv364, c232)"},
/* tr */ {IDCLS_SET_PLUS4_MODEL_TR, "Plus4 modelini ayarla (c16/c16pal/c16ntsc, plus4/plus4pal/plus4ntsc, v364/cv364, c232)"},
#endif

/* c64dtv/c64dtv-cmdline-options.c */
/* en */ {IDCLS_SET_DTV_MODEL,    N_("Set DTV model (v2/v2pal/v2ntsc, v3/v3pal/v3ntsc, hummer)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DTV_MODEL_DA, "Angiv DTV-model (v2/v2pal/v2ntsc, v3/v3pal/v3ntsc, hummer)"},
/* de */ {IDCLS_SET_DTV_MODEL_DE, "Setze DTV Modell (v2/v2pal/v2ntsc, v3/v3pal/v3ntsc, hummer)"},
/* es */ {IDCLS_SET_DTV_MODEL_ES, "Seleccionar DTV model (v2/v2pal/v2ntsc, v3/v3pal/v3ntsc, hummer)"},
/* fr */ {IDCLS_SET_DTV_MODEL_FR, "Définir le modèle DTV (v2/v2pal/v2ntsc, v3/v3pal/v3ntsc, hummer)"},
/* hu */ {IDCLS_SET_DTV_MODEL_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DTV_MODEL_IT, "Imposta modello DTV (v2/v2pal/v2ntsc, v3/v3pal/v3ntsc, hummer)"},
/* ko */ {IDCLS_SET_DTV_MODEL_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DTV_MODEL_NL, "Stel DTV model in (v2/v2pal/v2ntsc, v3/v3pal/v3ntsc, hummer)"},
/* pl */ {IDCLS_SET_DTV_MODEL_PL, "Ustaw model DTV (v2/v2pal/v2ntsc, v3/v3pal/v3ntsc, hummer)"},
/* ru */ {IDCLS_SET_DTV_MODEL_RU, "Set DTV model (v2/v2pal/v2ntsc, v3/v3pal/v3ntsc, hummer)"},
/* sv */ {IDCLS_SET_DTV_MODEL_SV, "Välj DTV-modell (v2/v2pal/v2ntsc, v3/v3pal/v3ntsc, hummer)"},
/* tr */ {IDCLS_SET_DTV_MODEL_TR, "DTV modelini ayarla (v2/v2pal/v2ntsc, v3/v3pal/v3ntsc, hummer)"},
#endif

/* scpu64/scpu64-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SCPU64_ROM_NAME,    N_("Specify name of SCPU64 ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SCPU64_ROM_NAME_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_SCPU64_ROM_NAME_DE, "Name von SCPU64 ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_SCPU64_ROM_NAME_ES, "Especificar nombre imagen ROM SCPU64"},
/* fr */ {IDCLS_SPECIFY_SCPU64_ROM_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_SCPU64_ROM_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SCPU64_ROM_NAME_IT, "Specifica il nome immagine della ROM della SCPU64"},
/* ko */ {IDCLS_SPECIFY_SCPU64_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SCPU64_ROM_NAME_NL, "Geef de naam van het SCPU64 ROM bestand"},
/* pl */ {IDCLS_SPECIFY_SCPU64_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM SCPU64"},
/* ru */ {IDCLS_SPECIFY_SCPU64_ROM_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SCPU64_ROM_NAME_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_SCPU64_ROM_NAME_TR, ""},  /* fuzzy */
#endif

/* c64/c64-cmdline-options.c, scpu64/scpu64-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2,    N_("Specify index of keymap file (0=sym, 1=symDE, 2=pos)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_DA, "Angiv indeks for tastaturindstillingsfil (0=symbolsk, 1=symbolsk tysk, 2=positionsbestemt)"},
/* de */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_DE, "Index für Keymap Datei festlegen (0=symbol, 1=symDE, 2=positional)"},
/* es */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_ES, "Especificar índice fichero mapa teclado (0=sim, 1=simAL, 2=pos)"},
/* fr */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_FR, "Spécifier l'index du fichier keymap (0=sym, 1=symDE, 2=pos)"},
/* hu */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_HU, "Adja meg a billentyûzet leképzési fájl típusát (0=szimbolikus, 1=német szimbolikus, 2=pozíció szerinti)"},
/* it */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_IT, "Specifica l'indice del file della mappa della tastiera (0=sim, 1=simGER, 2=pos)"},
/* ko */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_NL, "Geef de index van het toetstoewijzingsbestand (0=sym, 1=symDE, 2=pos)"},
/* pl */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_PL, "Podaj indeks dla uk³adu klawiatury (0=symbol, 1=symbolDE, 2=pozycja)"},
/* ru */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_RU, "Specify index of keymap file (0=sym, 1=symDE, 2=pos)"},
/* sv */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_SV, "Ange index för för tangentbordsinställningsfil (0=symbolisk, 1=symbolisk tysk, 2=positionsriktig)"},
/* tr */ {IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_TR, "Tuþ haritasý dosyasýnýn indeksini belirt (0=sembol, 1=sembol Almanca, 2=konumsal)"},
#endif

/* c64/c64-cmdline-options.c, scpu64/scpu64-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP,    N_("Specify name of symbolic German keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_DA, "Angiv fil for tysk symbolsk tastaturindstilling"},
/* de */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_DE, "Name von symbolischer Keymap Datei wählen"},
/* es */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_ES, "Especificar nombre del fichero teclado simbólico alemán"},
/* fr */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_FR, "Spécifier le nom du fichier symbolique de mappage clavier"},
/* hu */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_HU, "Adja meg a nevét a német billentyûzet leképzés fájlnak"},
/* it */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_IT, "Specifica il nome del file della mappa simbolica della tastiera tedesca"},
/* ko */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_NL, "Geef de naam van het symbolische Duitse toetstoewijzingsbestand"},
/* pl */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_PL, "Okre¶l nazwê pliku symbolicznego niemieckiego uk³adu klawiatury"},
/* ru */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_RU, "Specify name of symbolic German keymap file"},
/* sv */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_SV, "Ange fil för tysk symbolisk tangentbordsemulering"},
/* tr */ {IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_TR, "Sembolik Almanca tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* c64/c64gluelogic.c */
/* en */ {IDCLS_SET_GLUE_LOGIC_TYPE,    N_("Set glue logic type (0 = discrete, 1 = 252535-01)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_GLUE_LOGIC_TYPE_DA, "Vælg glue logic type (0=diskret, 1=252535-01)"},
/* de */ {IDCLS_SET_GLUE_LOGIC_TYPE_DE, "Setze Glue-Lokig Typ (0 = diskret, 1 = 252535-01)"},
/* es */ {IDCLS_SET_GLUE_LOGIC_TYPE_ES, "Seleccionar tipo de 'glue' lógico (0 = discrete, 1 = 252535-01)"},
/* fr */ {IDCLS_SET_GLUE_LOGIC_TYPE_FR, "Définir le type de logique glue (0 = discrète, 1 = 252535-01)"},
/* hu */ {IDCLS_SET_GLUE_LOGIC_TYPE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_GLUE_LOGIC_TYPE_IT, "Imposta il tipo di glue logic (0 = discreta, 1 = 252535-01)"},
/* ko */ {IDCLS_SET_GLUE_LOGIC_TYPE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_GLUE_LOGIC_TYPE_NL, "Stel glue logic type in (0 = discreet, 1 = 252535-01)"},
/* pl */ {IDCLS_SET_GLUE_LOGIC_TYPE_PL, "Ustaw typ glue logic (0 = dyskretna, 1 = 252535-01)"},
/* ru */ {IDCLS_SET_GLUE_LOGIC_TYPE_RU, "Set glue logic type (0 = discrete, 1 = 252535-01)"},
/* sv */ {IDCLS_SET_GLUE_LOGIC_TYPE_SV, "Välj typ av klisterlogik (0 = diskret, 1 = 252535-01)"},
/* tr */ {IDCLS_SET_GLUE_LOGIC_TYPE_TR, "Glue logic tipini ayarla (0 = baðlantýyý kes, 1 = 252535-01)"},
#endif

/* c64dtv/c64dtv-cmdline-options.c */
/* en */ {IDCLS_ENABLE_HUMMER_ADC,    N_("Enable Hummer ADC")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_HUMMER_ADC_DA, "Aktivér Hummer ADC"},
/* de */ {IDCLS_ENABLE_HUMMER_ADC_DE, "Hummer ADC Aktivieren"},
/* es */ {IDCLS_ENABLE_HUMMER_ADC_ES, "Permitir Hummer ADC"},
/* fr */ {IDCLS_ENABLE_HUMMER_ADC_FR, "Activer Hummer ADC"},
/* hu */ {IDCLS_ENABLE_HUMMER_ADC_HU, "Hummer ADC engedélyezése"},
/* it */ {IDCLS_ENABLE_HUMMER_ADC_IT, "Attiva Hummer ADC"},
/* ko */ {IDCLS_ENABLE_HUMMER_ADC_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_HUMMER_ADC_NL, "Hummer ADC inschakelen"},
/* pl */ {IDCLS_ENABLE_HUMMER_ADC_PL, "W³±cz Hummer ADC"},
/* ru */ {IDCLS_ENABLE_HUMMER_ADC_RU, "Enable Hummer ADC"},
/* sv */ {IDCLS_ENABLE_HUMMER_ADC_SV, "Aktivera Hummer-A/D-omvandlare"},
/* tr */ {IDCLS_ENABLE_HUMMER_ADC_TR, "Hummer ADC'yi aktif et"},
#endif

/* c64dtv/c64dtv-cmdline-options.c */
/* en */ {IDCLS_DISABLE_HUMMER_ADC,    N_("Disable Hummer ADC")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_HUMMER_ADC_DA, "Deaktivér Hummer ADC"},
/* de */ {IDCLS_DISABLE_HUMMER_ADC_DE, "Hummer ADC deaktivieren"},
/* es */ {IDCLS_DISABLE_HUMMER_ADC_ES, "Deshabilitar Hummer ADC"},
/* fr */ {IDCLS_DISABLE_HUMMER_ADC_FR, "Désactiver Hummer ADC"},
/* hu */ {IDCLS_DISABLE_HUMMER_ADC_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_HUMMER_ADC_IT, "Disattiva Hummer ADC"},
/* ko */ {IDCLS_DISABLE_HUMMER_ADC_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_HUMMER_ADC_NL, "Hummer ADC uitschakelen"},
/* pl */ {IDCLS_DISABLE_HUMMER_ADC_PL, "Wy³±cz Hummer ADC"},
/* ru */ {IDCLS_DISABLE_HUMMER_ADC_RU, "Disable Hummer ADC"},
/* sv */ {IDCLS_DISABLE_HUMMER_ADC_SV, "Inaktivera Hummer-A/D-omvandlare"},
/* tr */ {IDCLS_DISABLE_HUMMER_ADC_TR, "Hummer ADC'yi pasifleþtir"},
#endif

/* c64/c64io.c, vic20/vic20io.c */
/* en */ {IDCLS_SELECT_CONFLICT_HANDLING,    N_("Select the way the I/O collisions should be handled, (0: error message and detach all involved carts, 1: error message and detach last attached involved carts, 2: warning in log and 'AND' the valid return values")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SELECT_CONFLICT_HANDLING_DA, "Vælg hvordan I/O-kollisioner skal håndteres (0: fejlbesked og afbryd alle involverede cartridges, 1: fejlbesked og afbryd senest tilsluttede cartridge, 2: Advarsel i log og returner 'logisk OG' af de gyldige returværdier)"},
/* de */ {IDCLS_SELECT_CONFLICT_HANDLING_DE, "Wähle, wie I/O Kollisionen behandelt werden sollen, (0: Fehler Meldung\nund Entfernen sämtlicher betroffener Module, 1: Fehler Meldung und\nEntfernen des letzten Moduls, 2: Warnung im Log und liefere logische\nUND-Verknüpfung gültiger Rückgabewerte"},
/* es */ {IDCLS_SELECT_CONFLICT_HANDLING_ES, "Seleccionar la forma en que las colisiones deben ser manejadas, (0: mensaje de error y desconectar todas los cartuchos involucrados, 1: mensaje de error y desconectar el último cartucho involucrado, 2: advertencia en  log y 'AND' los valores válidos de retorno"},
/* fr */ {IDCLS_SELECT_CONFLICT_HANDLING_FR, "Sélectionner la façon que les collisions E/S devraient être traitées, (0: message d'erreur et détachement de toutes les cartouches impliquées, 1: message d'erreur et détachement de la dernière cartouches impliquées à avoir été attachée, 2: message d'avertissement avec journalisation et 'ET' les valeurs de retour valides"},
/* hu */ {IDCLS_SELECT_CONFLICT_HANDLING_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SELECT_CONFLICT_HANDLING_IT, "Seleziona come gestire le collisioni di I/O, (0: messaggio di errore e rimozione di tutte le cartucce coinvolte, 1: messaggio di errore e rimozione delle ultima cartucce coinvolte, 2: warning nei log e 'AND' il valore di ritorno valido"},
/* ko */ {IDCLS_SELECT_CONFLICT_HANDLING_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SELECT_CONFLICT_HANDLING_NL, "Selecteer de manier waarop I/O conflicten worden afgehandeld, (0: foutmelding en ontkoppel alle betrokken cartridges, 1: foutmelding en ontkoppel alle cartridges behalve de eerste cartridge, 2: waarschuwing in het logbestand en doe een logische 'AND' met de teruggegeven waarden"},
/* pl */ {IDCLS_SELECT_CONFLICT_HANDLING_PL, "Wybierz metodê obs³ugi konfliktów we/wy, (0: komunikat b³êdu i od³±czenie wszystkich konfliktuj±cych kartrid¿ów, 1: komunikat b³êdu i od³±czenie ostatnio pod³±czonych konfliktuj±cych kartrid¿ów, 2: ostrze¿enie w logu i 'AND' w³a¶ciwe warto¶ci)"},
/* ru */ {IDCLS_SELECT_CONFLICT_HANDLING_RU, "Select the way the I/O collisions should be handled, (0: error message and detach all involved carts, 1: error message and detach last attached involved carts, 2: warning in log and 'AND' the valid return values"},
/* sv */ {IDCLS_SELECT_CONFLICT_HANDLING_SV, "Väljer hur I/O-kollisioner skall hanteras, (0: felmeddelande och koppla från alla inblandade moduler, 1: felmeddelande och koppla från sist anslutna inblandade modul, 2: varning i logg och logisk OCH på giltiga returvärden)"},
/* tr */ {IDCLS_SELECT_CONFLICT_HANDLING_TR, "G/Ç çakýþmalarýnýn deðerlendirilme yöntemini seç, (0: hata mesajý ve tüm ilgili cihazlarýn çýkarýlmasý, 1: hata mesajý ve en son takýlmýþ tüm ilgili cihazlarýn çýkarýlmasý, 2: log'a uyarý yazýlmasý ve geçerli dönüþ deðerlerinin 'VE'lenmesi"},
#endif

/* samplerdrv/file_drv.c */
/* en */ {IDCLS_SPECIFY_SAMPLE_NAME,    N_("Specify name of sample file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SAMPLE_NAME_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_SAMPLE_NAME_DE, "Namen für Sample Datei wählen"},
/* es */ {IDCLS_SPECIFY_SAMPLE_NAME_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_SAMPLE_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_SAMPLE_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SAMPLE_NAME_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SPECIFY_SAMPLE_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SAMPLE_NAME_NL, "Geef naam van het samplebestand"},
/* pl */ {IDCLS_SPECIFY_SAMPLE_NAME_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_SPECIFY_SAMPLE_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SAMPLE_NAME_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_SAMPLE_NAME_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_P_SIZE,    N_("<size>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_SIZE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_P_SIZE_DE, "<Größe>"},
/* es */ {IDCLS_P_SIZE_ES, "<Tamaño>"},
/* fr */ {IDCLS_P_SIZE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_P_SIZE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_SIZE_IT, "<dimensione>"},
/* ko */ {IDCLS_P_SIZE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_SIZE_NL, "<grootte>"},
/* pl */ {IDCLS_P_SIZE_PL, "<wielko¶æ>"},
/* ru */ {IDCLS_P_SIZE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_P_SIZE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_P_SIZE_TR, ""},  /* fuzzy */
#endif

/* pet/pet-sidcart.c */
/* en */ {IDCLS_P_ADDRESS,    N_("<address>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_ADDRESS_DA, "<adresse>"},
/* de */ {IDCLS_P_ADDRESS_DE, "<Adresse>"},
/* es */ {IDCLS_P_ADDRESS_ES, "<dirección>"},
/* fr */ {IDCLS_P_ADDRESS_FR, "<adresse>"},
/* hu */ {IDCLS_P_ADDRESS_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_ADDRESS_IT, "<indirizzo>"},
/* ko */ {IDCLS_P_ADDRESS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_ADDRESS_NL, "<adres>"},
/* pl */ {IDCLS_P_ADDRESS_PL, "<address>"},
/* ru */ {IDCLS_P_ADDRESS_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_P_ADDRESS_SV, "<adress>"},
/* tr */ {IDCLS_P_ADDRESS_TR, "<adres>"},
#endif

/* pet/pet-sidcart.c */
/* en */ {IDCLS_P_CLOCK,    N_("<clock>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_CLOCK_DA, ""},  /* fuzzy */
/* de */ {IDCLS_P_CLOCK_DE, "<Takt>"},
/* es */ {IDCLS_P_CLOCK_ES, "<Reloj>"},
/* fr */ {IDCLS_P_CLOCK_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_P_CLOCK_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_CLOCK_IT, "<clock>"},
/* ko */ {IDCLS_P_CLOCK_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_CLOCK_NL, "<klok>"},
/* pl */ {IDCLS_P_CLOCK_PL, "<zegar>"},
/* ru */ {IDCLS_P_CLOCK_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_P_CLOCK_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_P_CLOCK_TR, ""},  /* fuzzy */
#endif

/* pet/petreu.c */
/* en */ {IDCLS_ENABLE_PETREU,    N_("Enable the PET Ram and Expansion Unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_PETREU_DA, "Aktivér PET REU"},
/* de */ {IDCLS_ENABLE_PETREU_DE, "PET RAM Speicher und Erweiterungsmodul aktivieren"},
/* es */ {IDCLS_ENABLE_PETREU_ES, "Permitir PET ram y Unidad Expansión"},
/* fr */ {IDCLS_ENABLE_PETREU_FR, "Activer l'expansion d'unité et de RAM du PET"},
/* hu */ {IDCLS_ENABLE_PETREU_HU, "PET RAM and Expansion Unit engedélyezése"},
/* it */ {IDCLS_ENABLE_PETREU_IT, "Attiva la RAM e l'unità di espansione del PET"},
/* ko */ {IDCLS_ENABLE_PETREU_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_PETREU_NL, "PET RAM en uitbreidingseenheid inschakelen"},
/* pl */ {IDCLS_ENABLE_PETREU_PL, "W³±cz PET Ram i Expansion Unit"},
/* ru */ {IDCLS_ENABLE_PETREU_RU, "Enable the PET Ram and Expansion Unit"},
/* sv */ {IDCLS_ENABLE_PETREU_SV, "Aktivera PET Ram- och -expansionsenhet"},
/* tr */ {IDCLS_ENABLE_PETREU_TR, "PET Ram ve Geniþletme Birimi'ni aktif et"},
#endif

/* pet/petreu.c */
/* en */ {IDCLS_DISABLE_PETREU,    N_("Disable the PET Ram and Expansion Unit")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_PETREU_DA, "Deaktivér PET REU"},
/* de */ {IDCLS_DISABLE_PETREU_DE, "PET RAM Speicher und Erweiterungsmodul dektivieren"},
/* es */ {IDCLS_DISABLE_PETREU_ES, "Deshabilitar PET RAM y Unidad Expansión"},
/* fr */ {IDCLS_DISABLE_PETREU_FR, "Désactiver l'expansion d'unité et de RAM du PET"},
/* hu */ {IDCLS_DISABLE_PETREU_HU, "PET RAM and Expansion Unit tiltása"},
/* it */ {IDCLS_DISABLE_PETREU_IT, "Disattiva la RAM e l'unità di espansione del PET"},
/* ko */ {IDCLS_DISABLE_PETREU_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_PETREU_NL, "PET RAM en uitbreidingseenheid uitschakelen"},
/* pl */ {IDCLS_DISABLE_PETREU_PL, "Wy³±cz PET Ram i Expansion Unit"},
/* ru */ {IDCLS_DISABLE_PETREU_RU, "Disable the PET Ram and Expansion Unit"},
/* sv */ {IDCLS_DISABLE_PETREU_SV, "Inaktivera PET Ram- och -expansionsenhet"},
/* tr */ {IDCLS_DISABLE_PETREU_TR, "PET Ram ve Geniþletme Birimi'ni pasifleþtir"},
#endif

/* pet/petreu.c */
/* en */ {IDCLS_SPECIFY_PETREU_NAME,    N_("Specify name of PET Ram and Expansion Unit image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PETREU_NAME_DA, "Angiv navn på PET REU-image"},
/* de */ {IDCLS_SPECIFY_PETREU_NAME_DE, "Namen für PET RAM Speicher und Erweiterungsmodul wählen"},
/* es */ {IDCLS_SPECIFY_PETREU_NAME_ES, "Especificar nombre imagen PET Ram y Unidad Expansión"},
/* fr */ {IDCLS_SPECIFY_PETREU_NAME_FR, "Spécifier le nom de l'image d’unité d'expansion RAM"},
/* hu */ {IDCLS_SPECIFY_PETREU_NAME_HU, "Adja meg a PET RAM and Expansion Unit képmás nevét"},
/* it */ {IDCLS_SPECIFY_PETREU_NAME_IT, "Specifica il nome immagine della RAM e dell'unità di espansione del PET"},
/* ko */ {IDCLS_SPECIFY_PETREU_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PETREU_NAME_NL, "Geef de naam van het PET RAM en uitbreidingseenheid bestand"},
/* pl */ {IDCLS_SPECIFY_PETREU_NAME_PL, "Okre¶l nazwê obrazu PET Ram i Expansion Unit"},
/* ru */ {IDCLS_SPECIFY_PETREU_NAME_RU, "Specify name of PET Ram and Expansion Unit image"},
/* sv */ {IDCLS_SPECIFY_PETREU_NAME_SV, "Ange namn på PET Ram- och -expansionsenhet"},
/* tr */ {IDCLS_SPECIFY_PETREU_NAME_TR, "PET Ram ve Geniþletme Birimi'nin ismini belirt"},
#endif

/* pet/petreu.c */
/* en */ {IDCLS_PETREU_SIZE,    N_("Size of the PET Ram and Expansion Unit. (128/512/1024/2048)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PETREU_SIZE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_PETREU_SIZE_DE, "Größe des PET RAM Speicher und Erweiterungsmodul. (128/512/1024/2048)"},
/* es */ {IDCLS_PETREU_SIZE_ES, "Tamaño de PET Ram y Unidad Expansión. (128/512/1024/2048)"},
/* fr */ {IDCLS_PETREU_SIZE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_PETREU_SIZE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_PETREU_SIZE_IT, "Dimensione della RAM e dell'unità di espansione del PET. (128/512/1024/2048)"},
/* ko */ {IDCLS_PETREU_SIZE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PETREU_SIZE_NL, "Grootte van de PET RAM en uitbreidingseenheid. (128/512/1024/2048)"},
/* pl */ {IDCLS_PETREU_SIZE_PL, "Rozmiar PET Ram i Expansion Unit. (128/512/1024/2048)"},
/* ru */ {IDCLS_PETREU_SIZE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_PETREU_SIZE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_PETREU_SIZE_TR, ""},  /* fuzzy */
#endif

/* pet/petdww.c */
/* en */ {IDCLS_SPECIFY_PETDWW_NAME,    N_("Specify name of PET DWW image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PETDWW_NAME_DA, "Angiv navn på PET DWW-image"},
/* de */ {IDCLS_SPECIFY_PETDWW_NAME_DE, "Namen für PET DWW Imagedatei wählen"},
/* es */ {IDCLS_SPECIFY_PETDWW_NAME_ES, "Especificar nombre imagen PET DWW"},
/* fr */ {IDCLS_SPECIFY_PETDWW_NAME_FR, "Spécifier le nom de l'image PET DWW"},
/* hu */ {IDCLS_SPECIFY_PETDWW_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PETDWW_NAME_IT, "Specifica il nome immagine PET DWW"},
/* ko */ {IDCLS_SPECIFY_PETDWW_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PETDWW_NAME_NL, "Geef de naam van het PET DWW bestand"},
/* pl */ {IDCLS_SPECIFY_PETDWW_NAME_PL, "Okre¶l nazwê obrazu PET DWW"},
/* ru */ {IDCLS_SPECIFY_PETDWW_NAME_RU, "Specify name of PET DWW image"},
/* sv */ {IDCLS_SPECIFY_PETDWW_NAME_SV, "Ange namn på PET DWW-avbildning"},
/* tr */ {IDCLS_SPECIFY_PETDWW_NAME_TR, "PET DWW imajýnýn ismini belirt"},
#endif

/* pet/petdww.c */
/* en */ {IDCLS_ENABLE_PETDWW,    N_("Enable the PET DWW hi-res board")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_PETDWW_DA, "Aktivér PET DWW højopløsningskort"},
/* de */ {IDCLS_ENABLE_PETDWW_DE, "PET DWW hi-res Karte aktivieren"},
/* es */ {IDCLS_ENABLE_PETDWW_ES, "Permitir tablero de alta resolución PET DWW"},
/* fr */ {IDCLS_ENABLE_PETDWW_FR, "Activer la carte haute-rés PET DWW"},
/* hu */ {IDCLS_ENABLE_PETDWW_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_PETDWW_IT, "Attiva la scheda hi-res PET DWW"},
/* ko */ {IDCLS_ENABLE_PETDWW_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_PETDWW_NL, "PET DWW hi-res bord inschakelen"},
/* pl */ {IDCLS_ENABLE_PETDWW_PL, "W³±cz uk³ad PET DWW hi-res"},
/* ru */ {IDCLS_ENABLE_PETDWW_RU, "Enable the PET DWW hi-res board"},
/* sv */ {IDCLS_ENABLE_PETDWW_SV, "Aktivera PET DWW-högupplösningskort"},
/* tr */ {IDCLS_ENABLE_PETDWW_TR, "PET DWW hi-res boardunu aktif et"},
#endif

/* pet/petdww.c */
/* en */ {IDCLS_DISABLE_PETDWW,    N_("Disable the PET DWW hi-res board")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_PETDWW_DA, "Deaktivér PET DWW højopløsningskort"},
/* de */ {IDCLS_DISABLE_PETDWW_DE, "PET DWW Hi-Res Karte deaktivieren"},
/* es */ {IDCLS_DISABLE_PETDWW_ES, "Deshabilitar tablero de alta resolución PET DWW"},
/* fr */ {IDCLS_DISABLE_PETDWW_FR, "Désactiver la carte haute-rés PET DWW"},
/* hu */ {IDCLS_DISABLE_PETDWW_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_PETDWW_IT, "Disattiva la scheda hi-res PET DWW"},
/* ko */ {IDCLS_DISABLE_PETDWW_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_PETDWW_NL, "PET DWW hi-res bord uitschakelen"},
/* pl */ {IDCLS_DISABLE_PETDWW_PL, "Wy³±cz p³ytê PET DWW hi-res"},
/* ru */ {IDCLS_DISABLE_PETDWW_RU, "Disable the PET DWW hi-res board"},
/* sv */ {IDCLS_DISABLE_PETDWW_SV, "Inaktivera PET DWW-högupplösningskort"},
/* tr */ {IDCLS_DISABLE_PETDWW_TR, "PET DWW hi-res boardu pasifleþtir"},
#endif

/* c64/psid.c */
/* en */ {IDCLS_OVERWRITE_PSID_SETTINGS,    N_("Override PSID settings for Video standard and SID model")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_OVERWRITE_PSID_SETTINGS_DA, "Overstyr PSID-indstillinger for Video-standard og SID-model"},
/* de */ {IDCLS_OVERWRITE_PSID_SETTINGS_DE, "PSID Einstellung für Video Standard und SID Modell verändern"},
/* es */ {IDCLS_OVERWRITE_PSID_SETTINGS_ES, "Sobrescribir ajustes para vídeo standard y modelo SID"},
/* fr */ {IDCLS_OVERWRITE_PSID_SETTINGS_FR, "Écraser les paramètres PSID pour le standard vidéo et le modèle SID"},
/* hu */ {IDCLS_OVERWRITE_PSID_SETTINGS_HU, "A videó szabvány és a SID modell PSID beállításainak felülbírálása"},
/* it */ {IDCLS_OVERWRITE_PSID_SETTINGS_IT, "Non tener conto delle impostazioni del PSID per lo standard video e il modello di SID"},
/* ko */ {IDCLS_OVERWRITE_PSID_SETTINGS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_OVERWRITE_PSID_SETTINGS_NL, "Overschrijf PSID instellingen voor videostandaard en SID model"},
/* pl */ {IDCLS_OVERWRITE_PSID_SETTINGS_PL, "Zast±p ustawienia PSID standardu wideo i modelu SID"},
/* ru */ {IDCLS_OVERWRITE_PSID_SETTINGS_RU, "Override PSID settings for Video standard and SID model"},
/* sv */ {IDCLS_OVERWRITE_PSID_SETTINGS_SV, "Ersätt PSID-inställningar för videostandard och SID-modell"},
/* tr */ {IDCLS_OVERWRITE_PSID_SETTINGS_TR, "Video standardý ve SID modeli için PSID ayarlarýný geçersiz kýl"},
#endif

/* c64/psid.c */
/* en */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER,    N_("Specify PSID tune <number>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_DA, "Vælg PSID-nummer <nummer>"},
/* de */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_DE, "PSID Stück <Nummer> wählen"},
/* es */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_ES, "Especificar PSID tono <número>"},
/* fr */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_FR, "Spécifier le <numéro> de piste PSID"},
/* hu */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_HU, "Adja meg a PSID hangzatok <szám>"},
/* it */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_IT, "Specifica la melodia <numero> del PSID"},
/* ko */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_NL, "Geef PSID melodie <nummer>"},
/* pl */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_PL, "Okre¶l ton PSID <numer>"},
/* ru */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_RU, "Specify PSID tune <number>"},
/* sv */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_SV, "Ange PSID-låt <nummer>"},
/* tr */ {IDCLS_SPECIFY_PSID_TUNE_NUMBER_TR, "PSID parça <numara>'sýný belirt"},
#endif

/* c64/cart/supersnapshot.c */
/* en */ {IDCLS_ENABLE_SS_RAM_EXPANSION,    N_("Enable SS 32k RAM expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SS_RAM_EXPANSION_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_SS_RAM_EXPANSION_DE, "SS 32k RAM Erweiterung aktivieren"},
/* es */ {IDCLS_ENABLE_SS_RAM_EXPANSION_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_SS_RAM_EXPANSION_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_SS_RAM_EXPANSION_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SS_RAM_EXPANSION_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_SS_RAM_EXPANSION_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SS_RAM_EXPANSION_NL, "SS 32k RAM uitbreiding inschakelen"},
/* pl */ {IDCLS_ENABLE_SS_RAM_EXPANSION_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_ENABLE_SS_RAM_EXPANSION_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SS_RAM_EXPANSION_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_SS_RAM_EXPANSION_TR, ""},  /* fuzzy */
#endif

/* c64/cart/supersnapshot.c */
/* en */ {IDCLS_DISABLE_SS_RAM_EXPANSION,    N_("Disable SS 32k RAM expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SS_RAM_EXPANSION_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_SS_RAM_EXPANSION_DE, "SS 32k RAM Erweiterung deaktivieren"},
/* es */ {IDCLS_DISABLE_SS_RAM_EXPANSION_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_SS_RAM_EXPANSION_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_SS_RAM_EXPANSION_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_SS_RAM_EXPANSION_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_SS_RAM_EXPANSION_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SS_RAM_EXPANSION_NL, "SS 32k RAM uitbreiding uitschakelen"},
/* pl */ {IDCLS_DISABLE_SS_RAM_EXPANSION_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_DISABLE_SS_RAM_EXPANSION_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SS_RAM_EXPANSION_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_SS_RAM_EXPANSION_TR, ""},  /* fuzzy */
#endif

/* plus4/plus4cart.c */
/* en */ {IDCLS_SMART_ATTACH_CART,    N_("Smart-attach cartridge image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SMART_ATTACH_CART_DA, "Smart-tilslut cartridge-image"},
/* de */ {IDCLS_SMART_ATTACH_CART_DE, "Smart-Einlegen Modul Image"},
/* es */ {IDCLS_SMART_ATTACH_CART_ES, "Insertar imagen cartucho inteligentemente"},
/* fr */ {IDCLS_SMART_ATTACH_CART_FR, "Insertion intelligente d'une image de cartouche"},
/* hu */ {IDCLS_SMART_ATTACH_CART_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SMART_ATTACH_CART_IT, "Seleziona una cartuccia"},
/* ko */ {IDCLS_SMART_ATTACH_CART_KO, "Ä«Æ®¸®Áö ÀÌ¹ÌÁö¸¦ ½º¸¶Æ®-ºÙÀÌ±â ÇÏ±â"},
/* nl */ {IDCLS_SMART_ATTACH_CART_NL, "Slim-koppel cartridge bestand"},
/* pl */ {IDCLS_SMART_ATTACH_CART_PL, "Inteligentnie zamontuj obraz kartrid¿a"},
/* ru */ {IDCLS_SMART_ATTACH_CART_RU, "Smart-attach cartridge image"},
/* sv */ {IDCLS_SMART_ATTACH_CART_SV, "Smartanslut insticksmodulfil"},
/* tr */ {IDCLS_SMART_ATTACH_CART_TR, "Akýllý kartuþ imajý yerleþtir"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_P_MODELNUMBER,    N_("<modelnumber>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_MODELNUMBER_DA, "<modelnummer>"},
/* de */ {IDCLS_P_MODELNUMBER_DE, "<Modell Nummer>"},
/* es */ {IDCLS_P_MODELNUMBER_ES, "<número modelo>"},
/* fr */ {IDCLS_P_MODELNUMBER_FR, "<numerodemodele>"},
/* hu */ {IDCLS_P_MODELNUMBER_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_MODELNUMBER_IT, "<numero di modello>"},
/* ko */ {IDCLS_P_MODELNUMBER_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_MODELNUMBER_NL, "<modelnummer>"},
/* pl */ {IDCLS_P_MODELNUMBER_PL, "<modelnumber>"},
/* ru */ {IDCLS_P_MODELNUMBER_RU, "<modelnumber>"},
/* sv */ {IDCLS_P_MODELNUMBER_SV, "<modellnummer>"},
/* tr */ {IDCLS_P_MODELNUMBER_TR, "<modelnumarasý>"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CBM2_MODEL,    N_("Specify CBM-II model to emulate. (610, 620, 620+, 710, 720, 720+)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CBM2_MODEL_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_CBM2_MODEL_DE, "CBM-II Modell für Emulation wählen. (610, 620, 620+, 710, 720, 720+)"},
/* es */ {IDCLS_SPECIFY_CBM2_MODEL_ES, "Especificar modelo CBM-II para emular. (610, 620, 620+, 710, 720, 720+)"},
/* fr */ {IDCLS_SPECIFY_CBM2_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_CBM2_MODEL_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CBM2_MODEL_IT, "Specifica il modello di CBM II da emulare. (610, 620, 620+, 710, 720, 720+)"},
/* ko */ {IDCLS_SPECIFY_CBM2_MODEL_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CBM2_MODEL_NL, "Geef CBM-II model om te emuleren. (610, 620, 620+, 710, 720, 720+)"},
/* pl */ {IDCLS_SPECIFY_CBM2_MODEL_PL, "Okre¶l emulowany model CBM-II. (610, 620, 620+, 710, 720, 720+)"},
/* ru */ {IDCLS_SPECIFY_CBM2_MODEL_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CBM2_MODEL_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_CBM2_MODEL_TR, ""},  /* fuzzy */
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CBM5X0_MODEL,    N_("Specify CBM-II model to emulate. (510)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CBM5X0_MODEL_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_CBM5X0_MODEL_DE, "CBM-II Modell für Emulation wählen. (510)"},
/* es */ {IDCLS_SPECIFY_CBM5X0_MODEL_ES, "Especificar modelo CBM-II para emular. (510)"},
/* fr */ {IDCLS_SPECIFY_CBM5X0_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_CBM5X0_MODEL_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_CBM5X0_MODEL_IT, "Specifica il modello di CBM II da emulare. (510)"},
/* ko */ {IDCLS_SPECIFY_CBM5X0_MODEL_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CBM5X0_MODEL_NL, "Geef CBM-II model om te emuleren. (510)"},
/* pl */ {IDCLS_SPECIFY_CBM5X0_MODEL_PL, "Okre¶l emulowany model CBM-II. (510)"},
/* ru */ {IDCLS_SPECIFY_CBM5X0_MODEL_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_CBM5X0_MODEL_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_CBM5X0_MODEL_TR, ""},  /* fuzzy */
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_TO_USE_VIC_II,    N_("Specify to use VIC-II")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TO_USE_VIC_II_DA, "Brug VIC-II"},
/* de */ {IDCLS_SPECIFY_TO_USE_VIC_II_DE, "VIC-II Unterstützung aktivieren"},
/* es */ {IDCLS_SPECIFY_TO_USE_VIC_II_ES, "Especificar usar CBM-II"},
/* fr */ {IDCLS_SPECIFY_TO_USE_VIC_II_FR, "Spécifier l'utilisation de VIC-II"},
/* hu */ {IDCLS_SPECIFY_TO_USE_VIC_II_HU, "VIC-II használata"},
/* it */ {IDCLS_SPECIFY_TO_USE_VIC_II_IT, "Specifica di utilizzare il VIC-II"},
/* ko */ {IDCLS_SPECIFY_TO_USE_VIC_II_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TO_USE_VIC_II_NL, "Gebruik de VIC-II"},
/* pl */ {IDCLS_SPECIFY_TO_USE_VIC_II_PL, "Okre¶l u¿ycie VIC-II"},
/* ru */ {IDCLS_SPECIFY_TO_USE_VIC_II_RU, "Specify to use VIC-II"},
/* sv */ {IDCLS_SPECIFY_TO_USE_VIC_II_SV, "Ange för att använda VIC-II"},
/* tr */ {IDCLS_SPECIFY_TO_USE_VIC_II_TR, "VIC-II kullanýmý için seçin"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_TO_USE_CRTC,    N_("Specify to use CRTC")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TO_USE_CRTC_DA, "Brug CRTC"},
/* de */ {IDCLS_SPECIFY_TO_USE_CRTC_DE, "CRTC Unterstützung aktivieren"},
/* es */ {IDCLS_SPECIFY_TO_USE_CRTC_ES, "Especificar usar CRT"},
/* fr */ {IDCLS_SPECIFY_TO_USE_CRTC_FR, "Spécifier l'utilisation de CRTC"},
/* hu */ {IDCLS_SPECIFY_TO_USE_CRTC_HU, "CRTC használata"},
/* it */ {IDCLS_SPECIFY_TO_USE_CRTC_IT, "Specifica di utilizzare il CRTC"},
/* ko */ {IDCLS_SPECIFY_TO_USE_CRTC_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TO_USE_CRTC_NL, "Gebruik de CRTC"},
/* pl */ {IDCLS_SPECIFY_TO_USE_CRTC_PL, "Okre¶l u¿ycie CRTC"},
/* ru */ {IDCLS_SPECIFY_TO_USE_CRTC_RU, "Specify to use CRTC"},
/* sv */ {IDCLS_SPECIFY_TO_USE_CRTC_SV, "Ange för att använda CRTC"},
/* tr */ {IDCLS_SPECIFY_TO_USE_CRTC_TR, "CRTC kullanýmý için seçin"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_P_LINENUMBER,    N_("<linenumber>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_LINENUMBER_DA, "<linjenummer>"},
/* de */ {IDCLS_P_LINENUMBER_DE, "<Zeilennummer>"},
/* es */ {IDCLS_P_LINENUMBER_ES, "<número linea>"},
/* fr */ {IDCLS_P_LINENUMBER_FR, "<numérodeligne>"},
/* hu */ {IDCLS_P_LINENUMBER_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_LINENUMBER_IT, "<numero di linea>"},
/* ko */ {IDCLS_P_LINENUMBER_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_LINENUMBER_NL, "<lijnnummer>"},
/* pl */ {IDCLS_P_LINENUMBER_PL, "<linenumber>"},
/* ru */ {IDCLS_P_LINENUMBER_RU, "<linenumber>"},
/* sv */ {IDCLS_P_LINENUMBER_SV, "<linjenummer>"},
/* tr */ {IDCLS_P_LINENUMBER_TR, "<satýrnumarasý>"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE,    N_("Specify CBM-II model hardware (0=6x0, 1=7x0)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_DA, "Angiv hardware for CBM-II-model (0=6x0, 1=7x0)"},
/* de */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_DE, "CBM-II Hardware Modell wählen (0=6x0, 1=7x0)"},
/* es */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_ES, "Especificar modelo hardware CBM-II (0=6x0, 1=7x0)"},
/* fr */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_FR, "Spécifier le modèle hardware CBM-II (0=6x0, 1=7x0)"},
/* hu */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_HU, "Adja meg a CBM-II hardver modellt (0=6x0, 1=7x0)"},
/* it */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_IT, "Specifica il modello hardware del CBM-II (0=6x0, 1=7x0)"},
/* ko */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_NL, "Geef CBM-II hardwaremodel (0=6x0, 1=7x0)"},
/* pl */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_PL, "Okre¶l model sprzêtu CBM-II (0=6x0, 1=7x0)"},
/* ru */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_RU, "Specify CBM-II model hardware (0=6x0, 1=7x0)"},
/* sv */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_SV, "Ange maskinvara för CBM-II-modell (0=6x0, 1=7x0)"},
/* tr */ {IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_TR, "CBM-II modeli donanýmýný belirt (0=6x0, 1=7x0)"},
#endif

/* cbm2/cbm2-cmdline-options.c, plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_P_RAMSIZE,    N_("<RAM size>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_RAMSIZE_DA, "<RAM-størrelse>"},
/* de */ {IDCLS_P_RAMSIZE_DE, "<RAM Größe>"},
/* es */ {IDCLS_P_RAMSIZE_ES, "<Tamaño RAM>"},
/* fr */ {IDCLS_P_RAMSIZE_FR, "<Taille de la RAM>"},
/* hu */ {IDCLS_P_RAMSIZE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_RAMSIZE_IT, "<Dimensione RAM>"},
/* ko */ {IDCLS_P_RAMSIZE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_RAMSIZE_NL, "<RAM grootte>"},
/* pl */ {IDCLS_P_RAMSIZE_PL, "<Rozmiar RAM>"},
/* ru */ {IDCLS_P_RAMSIZE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_P_RAMSIZE_SV, "<RAM-storlek>"},
/* tr */ {IDCLS_P_RAMSIZE_TR, "<RAM boyutu>"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIZE_OF_RAM,    N_("Specify size of RAM (64/128/256/512/1024 kByte)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SIZE_OF_RAM_DA, "Angiv størrelse på RAM (64/128/256/512/1024 kByte)"},
/* de */ {IDCLS_SPECIFY_SIZE_OF_RAM_DE, "RAM Größe wählen (64/128/256/512/1024 kByte)"},
/* es */ {IDCLS_SPECIFY_SIZE_OF_RAM_ES, "Especificar tamaño RAM  (64/128/256/512/1024 KB)"},
/* fr */ {IDCLS_SPECIFY_SIZE_OF_RAM_FR, "Spécifier la taille de la RAM (64/128/256/512/1024 KO)"},
/* hu */ {IDCLS_SPECIFY_SIZE_OF_RAM_HU, "Adja meg a RAM méretét (64/128/256/512/1024 kBájt)"},
/* it */ {IDCLS_SPECIFY_SIZE_OF_RAM_IT, "Specifica la dimensione della RAM (4/128/256/512/1024 kByte)"},
/* ko */ {IDCLS_SPECIFY_SIZE_OF_RAM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SIZE_OF_RAM_NL, "Geef geheugengrootte (64/128/256/512/1024 kByte)"},
/* pl */ {IDCLS_SPECIFY_SIZE_OF_RAM_PL, "Okre¶l rozmiar RAM-u (64/128/256/512/1024 kBajty)"},
/* ru */ {IDCLS_SPECIFY_SIZE_OF_RAM_RU, "Specify size of RAM (64/128/256/512/1024 kByte)"},
/* sv */ {IDCLS_SPECIFY_SIZE_OF_RAM_SV, "Ange storlek på RAM (64/128/256/512/1024 kByte)"},
/* tr */ {IDCLS_SPECIFY_SIZE_OF_RAM_TR, "RAM boyutunu belirt (64/128/256/512/1024 kByte)"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2,    N_("Specify size of RAM (128/256/512/1024 kByte)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2_DE, "RAM Größe wählen (128/256/512/1024 kByte)"},
/* es */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2_ES, "Especificar tamaño RAM  (128/256/512/1024 kByte)"},
/* fr */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2_IT, "Specifica la dimensione della RAM (128/256/512/1024 kByte)"},
/* ko */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2_NL, "Geef geheugengrootte (128/256/512/1024 kByte)"},
/* pl */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2_PL, "Okre¶l rozmiar RAM-u (128/256/512/1024 kB)"},
/* ru */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_SIZE_OF_RAM_CBM2_TR, ""},  /* fuzzy */
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_1000_NAME,    N_("Specify name of cartridge ROM image for $1000")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_DA, "Angiv navn på cartridge-ROM-image for $1000"},
/* de */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_DE, "Name für ROM Module Imagedatei ($1000)"},
/* es */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_ES, "Especificar nombre imagen cartucho ROM para $1000"},
/* fr */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_FR, "Spécifier le nom de l’image de cartouche ROM pour $1000"},
/* hu */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_HU, "Adja meg a $1000 címû cartridge ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_IT, "Specifica il nome immagine della cartuccia ROM a $1000"},
/* ko */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_NL, "Geef de naam van het cartridge ROM bestand voor $1000"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_PL, "Okre¶l nazwê obrazu ROM kartrid¿a w $1000"},
/* ru */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_RU, "Specify name of cartridge ROM image for $1000"},
/* sv */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_SV, "Ange namn på insticksmodul-ROM-avbildning för $1000"},
/* tr */ {IDCLS_SPECIFY_CART_ROM_1000_NAME_TR, "$1000 adresi için kartuþ ROM imaj ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_2000_NAME,    N_("Specify name of cartridge ROM image for $2000-$3fff")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_DA, "Angiv navn på cartridge-ROM-image for $2000-3fff"},
/* de */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_DE, "Name für ROM Module Imagedatei ($2000-3fff)"},
/* es */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_ES, "Especificar nombre imagen cartucho ROM para $2000-$3FFF"},
/* fr */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_FR, "Spécifier le nom de l’image de cartouche ROM pour $2000-$3fff"},
/* hu */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_HU, "Adja meg a $2000-$3fff címû cartridge ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_IT, "Specifica il nome immagine della ROM della cartuccia per $2000-$3fff"},
/* ko */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_NL, "Geef de naam van het cartridge ROM bestand voor $2000-$3fff"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_PL, "Okre¶l nazwê obrazu ROM kartrid¿a w $2000-$3fff"},
/* ru */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_RU, "Specify name of cartridge ROM image for $2000-$3fff"},
/* sv */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_SV, "Ange namn på insticksmodul-ROM-avbildning för $2000-3fff"},
/* tr */ {IDCLS_SPECIFY_CART_ROM_2000_NAME_TR, "$2000-$3fff adres aralýðý için kartuþ ROM imaj ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_4000_NAME,    N_("Specify name of cartridge ROM image for $4000-$5fff")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_DA, "Angiv navn på cartridge-ROM-image for $4000-$5fff"},
/* de */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_DE, "Name für ROM Module Imagedatei ($4000-5fff)"},
/* es */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_ES, "Especificar nombre imagen cartucho ROM para $4000-$5FFF"},
/* fr */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_FR, "Spécifier le nom de l’image de cartouche ROM pour $4000-$5fff"},
/* hu */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_HU, "Adja meg a $4000-$5fff címû cartridge ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_IT, "Specifica il nome immagine della ROM della cartuccia per $4000-$5fff"},
/* ko */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_NL, "Geef de naam van het cartridge ROM bestand voor $4000-$5fff"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_PL, "Okre¶l nazwê obrazu ROM kartrid¿a w $4000-$5fff"},
/* ru */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_RU, "Specify name of cartridge ROM image for $4000-$5fff"},
/* sv */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_SV, "Ange namn på insticksmodul-ROM-avbildning för $4000-$5fff"},
/* tr */ {IDCLS_SPECIFY_CART_ROM_4000_NAME_TR, "$4000-$5fff adres aralýðý için kartuþ ROM imaj ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_ROM_6000_NAME,    N_("Specify name of cartridge ROM image for $6000-$7fff")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_DA, "Angiv navn på cartridge-ROM-image for $6000-$7fff"},
/* de */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_DE, "Name für ROM Module Imagedatei ($6000-7fff)"},
/* es */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_ES, "Especificar nombre imagen cartucho ROM para $6000-$7FFF"},
/* fr */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_FR, "Spécifier le nom de l’image de cartouche ROM pour $6000-$7fff"},
/* hu */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_HU, "Adja meg a $6000-$7fff címû cartridge ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_IT, "Specifica il nome immagine della ROM della cartuccia per $6000-$7fff"},
/* ko */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_NL, "Geef de naam van het cartridge ROM bestand voor $6000-$7fff"},
/* pl */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_PL, "Okre¶l nazwê obrazu ROM kartrid¿a w $6000-$7fff"},
/* ru */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_RU, "Specify name of cartridge ROM image for $6000-$7fff"},
/* sv */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_SV, "Ange namn på insticksmodul-ROM-avbildning för $6000-$7fff"},
/* tr */ {IDCLS_SPECIFY_CART_ROM_6000_NAME_TR, "$6000-$7fff adres aralýðý için kartuþ ROM imaj ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800,    N_("Enable RAM mapping in $0800-$0FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_DA, "Aktivér RAM-mappning på $0800-$0FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_DE, "RAM Zuordnung in $0800-$0FFF aktivieren"},
/* es */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_ES, "Permitir mapeado RAM en $0800-$0FFF"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_FR, "Activer le mappage RAM dans $0800-$0FFF"},
/* hu */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_HU, "$0800-$0FFF RAM leképzés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_IT, "Attiva mappatura della RAM a $0800-$0FFF"},
/* ko */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_NL, "RAM op adres $0800-$0FFF inschakelen"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_PL, "W³±cz mapowanie RAM w $0800-$0FFF"},
/* ru */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_RU, "Enable RAM mapping in $0800-$0FFF"},
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_SV, "Aktivera RAM-mappning på $0800-$0FFF"},
/* tr */ {IDCLS_ENABLE_RAM_MAPPING_IN_0800_TR, "$0800-$0FFF aralýðýnda RAM eþleþtirmeyi aktif et"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800,    N_("Disable RAM mapping in $0800-$0FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_DA, "Deaktivér RAM-mappning på $0800-$0FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_DE, "RAM Zuordnung in $0800-$0FFF deaktivieren"},
/* es */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_ES, "Deshabilitar mapeado RAM en $0800-$0FFF"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_FR, "Désactiver le mappage RAM dans $0800-$0FFF"},
/* hu */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_HU, "$0800-$0FFF RAM leképzés tiltása"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_IT, "Disattiva mappatura della RAM a $0800-$0FFF"},
/* ko */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_NL, "RAM op adres $0800-$0FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_PL, "Wy³±cz mapowanie RAM w $0800-$0FFF"},
/* ru */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_RU, "Disable RAM mapping in $0800-$0FFF"},
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_SV, "Inaktivera RAM-mappning på $0800-$0FFF"},
/* tr */ {IDCLS_DISABLE_RAM_MAPPING_IN_0800_TR, "$0800-$0FFF aralýðýnda RAM eþleþtirmeyi pasifleþtir"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000,    N_("Enable RAM mapping in $1000-$1FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_DA, "Aktivér RAM-mappning på $1000-$1FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_DE, "RAM Zuordnung in $1000-$1FFF aktivieren"},
/* es */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_ES, "Permitir mapeado RAM en $1000-$1FFF"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_FR, "Activer le mappage RAM dans $1000-$1FFF"},
/* hu */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_HU, "$1000-$1FFF RAM leképzés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_IT, "Attiva mappatura della RAM a $1000-$1FFF"},
/* ko */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_NL, "RAM op adres $1000-$1FFF inschakelen"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_PL, "W³±cz mapowanie RAM w $1000-$1FFF"},
/* ru */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_RU, "Enable RAM mapping in $1000-$1FFF"},
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_SV, "Aktivera RAM-mappning på $1000-$1FFF"},
/* tr */ {IDCLS_ENABLE_RAM_MAPPING_IN_1000_TR, "$1000-$1FFF aralýðýnda RAM eþleþtirmeyi aktif et"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000,    N_("Disable RAM mapping in $1000-$1FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_DA, "Deaktivér RAM-mappning på $1000-$1FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_DE, "RAM Zuordnung in $1000-$1FFF deaktivieren"},
/* es */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_ES, "Deshabilitar mapeado RAM en $1000-$1FFF"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_FR, "Désactiver le mappage RAM dans $1000-$1FFF"},
/* hu */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_HU, "$1000-$1FFF RAM leképzés tiltása"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_IT, "Disattiva mappatura della RAM a $1000-$1FFF"},
/* ko */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_NL, "RAM op adres $1000-$1FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_PL, "Wy³±cz mapowanie RAM w $1000-$1FFF"},
/* ru */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_RU, "Disable RAM mapping in $1000-$1FFF"},
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_SV, "Inaktivera RAM-mappning på $1000-$1FFF"},
/* tr */ {IDCLS_DISABLE_RAM_MAPPING_IN_1000_TR, "$1000-$1FFF aralýðýnda RAM eþleþtirmeyi pasifleþtir"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000,    N_("Enable RAM mapping in $2000-$3FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_DA, "Aktivér RAM-mappning på $2000-$3FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_DE, "RAM Zuordnung in $2000-3FFF aktivieren"},
/* es */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_ES, "Permitir mapeado RAM en $2000-$3FFF"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_FR, "Activer le mappage RAM dans $2000-$3FFF"},
/* hu */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_HU, "$2000-$3FFF RAM leképzés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_IT, "Attiva mappatura della RAM a $2000-$3FFF"},
/* ko */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_NL, "RAM op adres $2000-$3FFF inschakelen"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_PL, "W³±cz mapowanie RAM w $2000-$3FFF"},
/* ru */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_RU, "Enable RAM mapping in $2000-$3FFF"},
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_SV, "Aktivera RAM-mappning på $2000-$3FFF"},
/* tr */ {IDCLS_ENABLE_RAM_MAPPING_IN_2000_TR, "$2000-$3FFF aralýðýnda RAM eþleþtirmeyi aktif et"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000,    N_("Disable RAM mapping in $2000-$3FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_DA, "Deaktivér RAM-mappning på $2000-$3FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_DE, "RAM Zuordnung in $2000-$3FFF deaktivieren"},
/* es */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_ES, "Deshabilitar mapeado RAM en $2000-$3FFF"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_FR, "Désactiver le mappage RAM dans $2000-$3FFF"},
/* hu */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_HU, "$2000-$3FFF RAM leképzés tiltása"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_IT, "Disattiva mappatura della RAM a $2000-$3FFF"},
/* ko */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_NL, "RAM op adres $2000-$3FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_PL, "Wy³±cz mapowanie RAM w $2000-$3FFF"},
/* ru */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_RU, "Disable RAM mapping in $2000-$3FFF"},
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_SV, "Inaktivera RAM-mappning på $2000-$3FFF"},
/* tr */ {IDCLS_DISABLE_RAM_MAPPING_IN_2000_TR, "$2000-$3FFF aralýðýnda RAM eþleþtirmeyi pasifleþtir"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000,    N_("Enable RAM mapping in $4000-$5FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_DA, "Aktivér RAM-mappning på $4000-$5FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_DE, "RAM Zuordnung in $4000-$5FFF aktivieren"},
/* es */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_ES, "Permitir mapeado RAM en $4000-$5FFF"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_FR, "Activer le mappage RAM dans $4000-$5FFF"},
/* hu */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_HU, "$4000-$5FFF RAM leképzés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_IT, "Attiva mappatura della RAM a $4000-$5FFF"},
/* ko */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_NL, "RAM op adres $4000-$5FFF inschakelen"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_PL, "W³±cz mapowanie RAM w $4000-$5FFF"},
/* ru */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_RU, "Enable RAM mapping in $4000-$5FFF"},
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_SV, "Aktivera RAM-mappning på $4000-$5FFF"},
/* tr */ {IDCLS_ENABLE_RAM_MAPPING_IN_4000_TR, "$4000-$5FFF aralýðýnda RAM eþleþtirmeyi aktif et"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000,    N_("Disable RAM mapping in $4000-$5FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_DA, "Deaktivér RAM-mappning på $4000-$5FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_DE, "RAM Zuordnung in $4000-$5FFF deaktivieren"},
/* es */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_ES, "Deshabilitar mapeado RAM en $4000-$5FFF"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_FR, "Désactiver le mappage RAM dans $4000-$5FFF"},
/* hu */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_HU, "$4000-$5FFF RAM leképzés tiltása"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_IT, "Disattiva mappatura della RAM a $4000-$5FFF"},
/* ko */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_NL, "RAM op adres $4000-$5FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_PL, "Wy³±cz mapowanie RAM w $4000-$5FFF"},
/* ru */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_RU, "Disable RAM mapping in $4000-$5FFF"},
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_SV, "Inaktivera RAM-mappning på $4000-$5FFF"},
/* tr */ {IDCLS_DISABLE_RAM_MAPPING_IN_4000_TR, "$4000-$5FFF aralýðýnda RAM eþleþtirmeyi pasifleþtir"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000,    N_("Enable RAM mapping in $6000-$7FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_DA, "Aktivér RAM-mappning på $6000-$7FFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_DE, "RAM Zuordnung in $6000-$7FFF aktivieren"},
/* es */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_ES, "Permitir mapeado RAM en $6000-$7FFF"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_FR, "Activer le mappage RAM dans $6000-$7FFF"},
/* hu */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_HU, "$6000-$7FFF RAM leképzés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_IT, "Attiva mappatura della RAM a $6000-$7FFF"},
/* ko */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_NL, "RAM op adres $6000-$7FFF inschakelen"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_PL, "W³±cz mapowanie RAM w $6000-$7FFF"},
/* ru */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_RU, "Enable RAM mapping in $6000-$7FFF"},
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_SV, "Aktivera RAM-mappning på $6000-$7FFF"},
/* tr */ {IDCLS_ENABLE_RAM_MAPPING_IN_6000_TR, "$6000-$7FFF aralýðýnda RAM eþleþtirmeyi aktif et"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000,    N_("Disable RAM mapping in $6000-$7FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_DA, "Deaktivér RAM-mappning på $6000-$7FFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_DE, "RAM Zuordnung in $6000-$7FFF deaktivieren"},
/* es */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_ES, "Deshabilitar mapeado RAM en $6000-$7FFF"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_FR, "Désactiver le mappage RAM dans $6000-$7FFF"},
/* hu */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_HU, "$6000-$7FFF RAM leképzés tiltása"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_IT, "Disattiva mappatura della RAM a $6000-$7FFF"},
/* ko */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_NL, "RAM op adres $6000-$7FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_PL, "Wy³±cz mapowanie RAM w $6000-$7FFF"},
/* ru */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_RU, "Disable RAM mapping in $6000-$7FFF"},
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_SV, "Inaktivera RAM-mappning på $6000-$7FFF"},
/* tr */ {IDCLS_DISABLE_RAM_MAPPING_IN_6000_TR, "$6000-$7FFF aralýðýnda RAM eþleþtirmeyi pasifleþtir"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000,    N_("Enable RAM mapping in $C000-$CFFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_DA, "Aktivér RAM-mappning på $C000-$CFFF"},
/* de */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_DE, "RAM Zuordnung in $C000-$CFFF aktivieren"},
/* es */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_ES, "Permitir mapeado RAM en $C000-$CFFF"},
/* fr */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_FR, "Activer le mappage RAM dans $C000-$CFFF"},
/* hu */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_HU, "$C000-$CFFF RAM leképzés engedélyezése"},
/* it */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_IT, "Attiva mappatura della RAM a $C000-$CFFF"},
/* ko */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_NL, "RAM op adres $C000-$CFFF inschakelen"},
/* pl */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_PL, "W³±cz mapowanie RAM w $C000-$CFFF"},
/* ru */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_RU, "Enable RAM mapping in $C000-$CFFF"},
/* sv */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_SV, "Aktivera RAM-mappning på $C000-$CFFF"},
/* tr */ {IDCLS_ENABLE_RAM_MAPPING_IN_C000_TR, "$C000-$CFFF aralýðýnda RAM eþleþtirmeyi aktif et"},
#endif

/* cbm2/cbm2-cmdline-options.c */
/* en */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000,    N_("Disable RAM mapping in $C000-$CFFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_DA, "Deaktivér RAM-mappning på $C000-$CFFF"},
/* de */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_DE, "RAM Zuordnung in $C000-$CFFF deaktivieren"},
/* es */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_ES, "Deshabilitar mapeado RAM en $C000-$CFFF"},
/* fr */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_FR, "Désactiver le mappage RAM dans $C000-$CFFF"},
/* hu */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_HU, "$C000-$CFFF RAM leképzés tiltása"},
/* it */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_IT, "Disattiva mappatura della RAM a $C000-$CFFF"},
/* ko */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_NL, "RAM op adres $C000-$CFFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_PL, "Wy³±cz mapowanie RAM w $C000-$CFFF"},
/* ru */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_RU, "Disable RAM mapping in $C000-$CFFF"},
/* sv */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_SV, "Inaktivera RAM-mappning på $C000-$CFFF"},
/* tr */ {IDCLS_DISABLE_RAM_MAPPING_IN_C000_TR, "$C000-$CFFF aralýðýnda RAM eþleþtirmeyi pasifleþtir"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_KEYMAP_INDEX,    N_("Specify index of keymap file (0: UK business symbolic, 1: UK business positional, 2: Graphics symbolic, 3: Graphics positional, 4: German business symbolic, 5: German business positional)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_KEYMAP_INDEX_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_KEYMAP_INDEX_DE, "Index der Keymap Datei wählen (0: UK business symbolic, 1: UK business positional, 2: Graphics symbolic, 3: Graphics positional, 4: German business symbolic, 5: German business positional)"},
/* es */ {IDCLS_SPECIFY_KEYMAP_INDEX_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_KEYMAP_INDEX_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_KEYMAP_INDEX_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_KEYMAP_INDEX_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SPECIFY_KEYMAP_INDEX_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_KEYMAP_INDEX_NL, "Geef index van het toetstoewijzingsbestand (0: UK business symbolisch, 1: UK business positioneel, 2: grafisch symbolisch, 3: grafisch positioneel, 4: Duits business symbolisch, 5: Duits business positioneel)"},
/* pl */ {IDCLS_SPECIFY_KEYMAP_INDEX_PL, "Podaj indeks uk³adu klawiatury (0: UK biznesowa - symbol, 1: UK biznesowa - pozycja, 2: Graficzna - symbol, 3: Graficzna - pozycja, 4: Niemiecka biznesowa - symbol, 5: Niemiecka biznesowa - pozycja)"},
/* ru */ {IDCLS_SPECIFY_KEYMAP_INDEX_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_KEYMAP_INDEX_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_KEYMAP_INDEX_TR, ""},  /* fuzzy */
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME,    N_("Specify name of graphics keyboard symbolic keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_DA, "Angiv fil for symbolsk tastaturemulering for \"graphics\"-tastatur"},
/* de */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_DE, "Keymap Datei für graphics keyboard symbolic wählen"},
/* es */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_ES, "Especificar nombre fichero gráficos teclado simbólico"},
/* fr */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier symbolique"},
/* hu */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_HU, "Adja meg a grafikus és szimbolikus billentyûzet leképzési fájl nevét."},
/* it */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_IT, "Specifica il nome del file della mappa simbolica della tastiera grafica"},
/* ko */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_NL, "Geef de naam van het symbolisch toetstoewijzingsbestand voor het grafische toetsenbord"},
/* pl */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_PL, "Okre¶l nazwê pliku symbolicznego uk³adu klawiatury graficznej"},
/* ru */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_RU, "Specify name of graphics keyboard symbolic keymap file"},
/* sv */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_SV, "Ange fil för symbolisk tangentbordsemulering för \"graphics\"-tangentbord"},
/* tr */ {IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_TR, "Grafik klavyesi sembolik tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME,    N_("Specify name of graphics keyboard positional keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_DA, "Angiv fil for positionsbestemt tastaturemulering for \"graphics\"-tastatur"},
/* de */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_DE, "Keymap Datei für graphics keyboard positional wählen"},
/* es */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_ES, "Especificar nombre fichero gráficos teclado posicional"},
/* fr */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier positionnel"},
/* hu */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_HU, "Adja meg a grafikus és pozíció szerinti billentyûzet leképzési fájl nevét."},
/* it */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_IT, "Specifica il nome del file della mappa posizionale della tastiera grafica"},
/* ko */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_NL, "Geef de naam van het positioneel toetstoewijzingsbestand voor het grafische toetsenbord"},
/* pl */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_PL, "Okre¶l nazwê pliku pozycyjnego uk³adu klawiatury graficznej"},
/* ru */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_RU, "Specify name of graphics keyboard positional keymap file"},
/* sv */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_SV, "Ange fil för positionsriktig tangentbordsemulering för \"graphics\"-tangentbord"},
/* tr */ {IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_TR, "Grafik klavyesi konumsal tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME,    N_("Specify name of UK business keyboard symbolic keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_DA, "Angiv fil for symbolsk tastaturemulering for Britisk \"business\"-tastatur"},
/* de */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_DE, "Keymap Datei für UK business keyboard symbolic wählen"},
/* es */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_ES, "Especificar nombre fichero teclado UK business simbólico"},
/* fr */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier symbolique UK"},
/* hu */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_HU, "Adja meg az angol, hivatalos, szimbolikus billentyûzet leképzési fájl nevét."},
/* it */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_IT, "Specifica il nome del file della mappa simbolica della tastiera UK business"},
/* ko */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_NL, "Geef de naam van het symbolisch toetstoewijzingsbestand voor het UK zakelijk toetsenbord"},
/* pl */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_PL, "Okre¶l nazwê pliku symbolicznego uk³adu angielskiej klawiatury biznesowej"},
/* ru */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_RU, "Specify name of UK business keyboard symbolic keymap file"},
/* sv */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_SV, "Ange fil för symbolisk tangentbordsemulering för brittiskt \"business\"-tangentbord"},
/* tr */ {IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_TR, "UK business klavyesi sembolik tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME,    N_("Specify name of UK business keyboard positional keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_DA, "Angiv fil for positionsbestemt tastaturemulering for Britisk \"business\"-tastatur"},
/* de */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_DE, "Keymap Datei für UK business keyboard positional wählen"},
/* es */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_ES, "Especificar nombre fichero teclado UK business posicional"},
/* fr */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier positionnel UK"},
/* hu */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_HU, "Adja meg az angol, hivatalos, pozíció szerinti billentyûzet leképzési fájl nevét."},
/* it */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_IT, "Specifica il nome del file della mappa posizionale della tastiera UK business"},
/* ko */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_NL, "Geef de naam van het positioneel toetstoewijzingsbestand voor het UK zakelijk toetsenbord"},
/* pl */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_PL, "Okre¶l nazwê pliku pozycyjnego uk³adu angielskiej klawiatury biznesowej"},
/* ru */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_RU, "Specify name of UK business keyboard positional keymap file"},
/* sv */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_SV, "Ange fil för positionsriktig tangentbordsemulering för brittiskt \"business\"-tangentbord"},
/* tr */ {IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_TR, "UK business klavyesi konumsal tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME,    N_("Specify name of German business keyboard symbolic keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_DA, "Angiv fil for symbolsk tastaturemulering for Tysk \"business\"-tastatur"},
/* de */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_DE, "Keymap Datei für German business keyboard symbolic wählen"},
/* es */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_ES, "Especificar nombre fichero teclado Alemán business simbólico"},
/* fr */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier symbolique allemand"},
/* hu */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_HU, "Adja meg a német, hivatalos, szimbolikus billentyûzet leképzési fájl nevét."},
/* it */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_IT, "Specifica il nome del file della mappa simbolica della tastiera business tedesca"},
/* ko */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_NL, "Geef de naam van het symbolisch toetstoewijzingsbestand voor het Duitse zakelijk toetsenbord"},
/* pl */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_PL, "Okre¶l nazwê pliku symbolicznego uk³adu niemieckiej klawiatury biznesowej"},
/* ru */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_RU, "Specify name of German business keyboard symbolic keymap file"},
/* sv */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_SV, "Ange fil för symbolisk tangentbordsemulering för tyskt \"business\"-tangentbord"},
/* tr */ {IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_TR, "Alman business klavyesi sembolik tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME,    N_("Specify name of German business keyboard positional keymap file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_DA, "Angiv fil for positionsbestemt tastaturemulering for Tysk \"business\"-tastatur"},
/* de */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_DE, "Keymap Datei für German business keyboard positional wählen"},
/* es */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_ES, "Especificar nombre fichero teclado Alemán business posicional"},
/* fr */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_FR, "Spécifier le nom du fichier de mappage clavier positionnel allemand"},
/* hu */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_HU, "Adja meg a német, hivatalos, pozíció szerinti billentyûzet leképzési fájl nevét."},
/* it */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_IT, "Specifica il nome del file della mappa posizionale della tastiera business tedesca"},
/* ko */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_NL, "Geef de naam van het positioneel toetstoewijzingsbestand voor het Duitse zakelijk toetsenbord"},
/* pl */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_PL, "Okre¶l nazwê pliku pozycyjnego uk³adu niemieckiej klawiatury biznesowej"},
/* ru */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_RU, "Specify name of German business keyboard positional keymap file"},
/* sv */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_SV, "Ange fil för positionsriktig tangentbordsemulering för tyskt \"business\"-tangentbord"},
/* tr */ {IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_TR, "Alman business klavyesi konumsal tuþ haritasý dosyasýnýn ismini belirt"},
#endif

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_P_RPM,    N_("<RPM>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_RPM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_P_RPM_DE, "<RPM>"},
/* es */ {IDCLS_P_RPM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_P_RPM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_P_RPM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_RPM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_P_RPM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_RPM_NL, "<RPM>"},
/* pl */ {IDCLS_P_RPM_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_P_RPM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_P_RPM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_P_RPM_TR, ""},  /* fuzzy */
#endif

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_P_WOBBLE,    N_("<Wobble>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_WOBBLE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_P_WOBBLE_DE, "<Schwankung>"},
/* es */ {IDCLS_P_WOBBLE_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_P_WOBBLE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_P_WOBBLE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_WOBBLE_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_P_WOBBLE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_WOBBLE_NL, "<Wiebel>"},
/* pl */ {IDCLS_P_WOBBLE_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_P_WOBBLE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_P_WOBBLE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_P_WOBBLE_TR, ""},  /* fuzzy */
#endif

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_SET_DRIVE_RPM,    N_("Set drive rpm (30000 = 300rpm)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DRIVE_RPM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_DRIVE_RPM_DE, "Setze Laufwerk RPM (30000 = 300rpm)"},
/* es */ {IDCLS_SET_DRIVE_RPM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_DRIVE_RPM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_DRIVE_RPM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DRIVE_RPM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SET_DRIVE_RPM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DRIVE_RPM_NL, "Stel drive rpm in (30000 = 300rpm)"},
/* pl */ {IDCLS_SET_DRIVE_RPM_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_SET_DRIVE_RPM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DRIVE_RPM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SET_DRIVE_RPM_TR, ""},  /* fuzzy */
#endif

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_SET_DRIVE_WOBBLE,    N_("Set drive wobble (100 = +/-0.5rpm)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DRIVE_WOBBLE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_DRIVE_WOBBLE_DE, "Setze Laufwaerkschwankung (100 = +/-0.5rpm)"},
/* es */ {IDCLS_SET_DRIVE_WOBBLE_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_DRIVE_WOBBLE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_DRIVE_WOBBLE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DRIVE_WOBBLE_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SET_DRIVE_WOBBLE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DRIVE_WOBBLE_NL, "Stel drive wiebel in (100 = +/-0.5rpm)"},
/* pl */ {IDCLS_SET_DRIVE_WOBBLE_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_SET_DRIVE_WOBBLE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DRIVE_WOBBLE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SET_DRIVE_WOBBLE_TR, ""},  /* fuzzy */
#endif

/* gfxoutputdrv/doodledrv.c */
/* en */ {IDCLS_P_COLOR,    N_("<color>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_COLOR_DA, "<farve>"},
/* de */ {IDCLS_P_COLOR_DE, "<Farbe>"},
/* es */ {IDCLS_P_COLOR_ES, "<color>"},
/* fr */ {IDCLS_P_COLOR_FR, "<couleur>"},
/* hu */ {IDCLS_P_COLOR_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_COLOR_IT, "<colore>"},
/* ko */ {IDCLS_P_COLOR_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_COLOR_NL, "<kleur>"},
/* pl */ {IDCLS_P_COLOR_PL, "<kolor>"},
/* ru */ {IDCLS_P_COLOR_RU, "<æÒÕâ>"},
/* sv */ {IDCLS_P_COLOR_SV, "<färg>"},
/* tr */ {IDCLS_P_COLOR_TR, "<renk>"},
#endif

/* gfxoutputdrv/doodledrv.c, gfxoutputdrv/koaladrv.c */
/* en */ {IDCLS_OVERSIZED_HANDLING,    N_("Select the way the oversized input should be handled, (0: scale down, 1: crop left top, 2: crop center top,  3: crop right top, 4: crop left center, 5: crop center, 6: crop right center, 7: crop left bottom, 8: crop center bottom, 9:  crop right bottom)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_OVERSIZED_HANDLING_DA, "Angiv håndtering af for stort input (0: skalér ned, 1: klip venstre top, 2: klip midt top,  3: klip højre top, 4: klip venstre midt, 5: klip centreret, 6: klip højre midt, 7: klip venstre bund, 8: klip midt bund, 9: klip højre bund)"},
/* de */ {IDCLS_OVERSIZED_HANDLING_DE, "Wähle, wie übergroßer Input behandelt werden soll, (0: skaliere, 1: schneide links oben, 2: schneide mitte oben,  3: schneide rechts oben, 4: schneide links mitte, 5: schneide mitte, 6: schneide rechts mitte, 7: schneide links unten, 8: schneide mitte unten, 9:  schneide rechts unten)"},
/* es */ {IDCLS_OVERSIZED_HANDLING_ES, "Seleccionar cómo se manejarán las entradas sobredimensionadas, (0: escalar, 1:recorte sup izq, 2:recorte sup centro, 3:recorte sup dcha, 4:recorte centro izq, 5:recorte centro, 6:recorte centro dcha, 7:recorte abajo izq, 8:recorte centro abajo, 9:recorte abajo dcha)"},
/* fr */ {IDCLS_OVERSIZED_HANDLING_FR, "Sélectionner la façon de traiter les entrées trop longues (0: réduire, 1-9: Rogner à partir du coin: 1: haut-gauche, 2: haut-centre, 3: haut-droite, 4: centre-gauche, 5: centre, 6: centre-droite, 7: bas-gauche, 8: bas-centre, 9: bas-droite)"},
/* hu */ {IDCLS_OVERSIZED_HANDLING_HU, ""},  /* fuzzy */
/* it */ {IDCLS_OVERSIZED_HANDLING_IT, "Seleziona come gestire il sovradimensionamento dell'input, (0: scale down, 1: ritaglia sinistra alto, 2: ritaglia centro alto,  3: ritaglia destra alto, 4: ritaglia sinistra centro, 5: ritaglia centro, 6: ritaglia destra centro, 7: ritaglia sinistra basso, 8: ritaglia centro basso, 9:  ritaglia destra basso)"},
/* ko */ {IDCLS_OVERSIZED_HANDLING_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_OVERSIZED_HANDLING_NL, "Selecteer de manier waarop de te grote invoer moet worden afgehandeld, (0: maak kleiner, 1: knip linker bovenkant, 2: knip midden bovenkant, 3: knip rechter bovenkant, 4: knip linker midden, 5: knip midden, 6: knip rechter midden, 7: knip linker onderkant, 8: knip midden onderkant, 9: knip rechter onderkant)"},
/* pl */ {IDCLS_OVERSIZED_HANDLING_PL, "Wybierz sposób, w jaki ma byæ obs³ugiwany za du¿y obraz, (0: zmniejszenie, 1: ¶ciêcie z góry z lewej, 2: ¶ciêcie z góry, 3: ¶ciêcie z prawej z góry, 4: ¶ciêcie z lewej, 5: ¶ciêcie od ¶rodka, 6: ¶ciêcie z prawej, 7: ¶ciêcie z lewej z do³u, 8: ¶ciêcie z do³u, 9: ¶ciêcie z prawej z do³u)"},
/* ru */ {IDCLS_OVERSIZED_HANDLING_RU, "Select the way the oversized input should be handled, (0: scale down, 1: crop left top, 2: crop center top,  3: crop right top, 4: crop left center, 5: crop center, 6: crop right center, 7: crop left bottom, 8: crop center bottom, 9:  crop right bottom)"},
/* sv */ {IDCLS_OVERSIZED_HANDLING_SV, "Välj hur för stor indata skall hanteras, (0: skala ned, 1: visa övre vänstra, 2: visa övre mitten, 3: visa övre högra, 4: visa vänstra mitten, 5: visa mitten, 6: visa högra mitten, 7: visa nedre vänstra, 8: visa nedre mitten, 9: visa nedre högre)"},
/* tr */ {IDCLS_OVERSIZED_HANDLING_TR, "Büyük girdilerin ele alýnma yöntemini seçin, (0: küçült, 1: sol üstten kýrp, 2: ortalý üstten kýrp,  3: sað üstten kýrp, 4: soldan ortalý kýrp, 5: ortadan kýrp, 6: saðdan ortalý kýrp, 7: sol alttan kýrp, 8: ortalý alttan kýrp, 9:  sað alttan kýrp)"},
#endif

/* gfxoutputdrv/doodledrv.c, gfxoutputdrv/koaladrv.c */
/* en */ {IDCLS_UNDERSIZED_HANDLING,    N_("Select the way the undersized input should be handled, (0: scale up, 1: borderize)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_UNDERSIZED_HANDLING_DA, ""},  /* fuzzy */
/* de */ {IDCLS_UNDERSIZED_HANDLING_DE, "Wie soll ungenügende Eingabe behandelt werden (0: skaliere, 1: borderize)"},
/* es */ {IDCLS_UNDERSIZED_HANDLING_ES, "Seleccionar cómo se manejará la entrada fuera de rango,  (0: scale up, 1: borderize)"},
/* fr */ {IDCLS_UNDERSIZED_HANDLING_FR, "Sélectionner la façon de traiter les entrée trop courtes, (0: agrandir, 1: ajouter bordure)"},
/* hu */ {IDCLS_UNDERSIZED_HANDLING_HU, ""},  /* fuzzy */
/* it */ {IDCLS_UNDERSIZED_HANDLING_IT, "Seleziona come gestire il sottodimensionamento dell'input, (0: scale up, 1: borderize)"},
/* ko */ {IDCLS_UNDERSIZED_HANDLING_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_UNDERSIZED_HANDLING_NL, "Selecteer de manier waarop de te kleine invoer moet worden afgehandeld, (0: maak groter, 1: maak border)"},
/* pl */ {IDCLS_UNDERSIZED_HANDLING_PL, "Wybierz sposób obs³ugi ma³ego obrazu (0: skaluj, 1: dodaj ramkê)"},
/* ru */ {IDCLS_UNDERSIZED_HANDLING_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_UNDERSIZED_HANDLING_SV, "Välj hur för liten indata skall hanteras (0: skala upp, 1: inramning)"},
/* tr */ {IDCLS_UNDERSIZED_HANDLING_TR, ""},  /* fuzzy */
#endif

/* gfxoutputdrv/doodledrv.c */
/* en */ {IDCLS_MULTICOLOR_HANDLING,    N_("Select the way the multicolor to hires should be handled, (0: b&w, 1: 2 colors, 2: 4 colors, 3: gray scale,  4: best cell colors)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_MULTICOLOR_HANDLING_DA, "Angiv måden multicolor til højopløsning skal håndteres på, (0: sort/hvid, 1: 2 farver, 2: 4 farver, 3: gråtoner,  4: bedste cellefarver)"},
/* de */ {IDCLS_MULTICOLOR_HANDLING_DE, "Wähle, wie Multicolor zu Hochauflösung behandelte werden soll, (0: s&w, 1: 2 Farben, 2: 4 Farben, 4 Graustufen, 4: Beste Zellenfarben)"},
/* es */ {IDCLS_MULTICOLOR_HANDLING_ES, "Seleccionar cómo se manejarán el cambio multicolor-alta resolución (0:ByN, 1:2 colores, 2:4 colores, 3:escala grises, 4:mejores colores)"},
/* fr */ {IDCLS_MULTICOLOR_HANDLING_FR, "Sélectionner la façon de traiter multicolore à haute résolution, (0: noir et blanc, 1: 2 couleurs, 2: 4 couleurs, 3: échelle de gris, 4: meilleures couleurs)"},
/* hu */ {IDCLS_MULTICOLOR_HANDLING_HU, ""},  /* fuzzy */
/* it */ {IDCLS_MULTICOLOR_HANDLING_IT, "Seleziona come gestire il multicolor in hires, (0: b&w, 1: 2 colori, 2: 4 colori, 3: scala di grigi,  4: migliori colori cella)"},
/* ko */ {IDCLS_MULTICOLOR_HANDLING_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_MULTICOLOR_HANDLING_NL, "Selecteer de manier waarop 'multicolor' naar 'hires' conversie moet worden afgehandeld, (0: zwart/wit, 1: 2 kleuren, 2: 4 kleuren, 3: grijs, 4: best mogelijke kleuren in de cel)"},
/* pl */ {IDCLS_MULTICOLOR_HANDLING_PL, "Wybiera sposób obs³ugi wielu kolorów w wysokiej rozdzielczo¶ci (0: czarno-bia³y, 1: 2 kolory, 2: 4 kolory, 3: skala szaro¶ci, 4: najlepsze kolory komórek)"},
/* ru */ {IDCLS_MULTICOLOR_HANDLING_RU, "Select the way the multicolor to hires should be handled, (0: b&w, 1: 2 colors, 2: 4 colors, 3: gray scale,  4: best cell colors)"},
/* sv */ {IDCLS_MULTICOLOR_HANDLING_SV, "Välj hur flerfärgs-till-högupplöst skall hanteras, (0: svartvit, 1: 2 färger, 2: fyra färger, 3: gråskala, 4: bästa cellfärger)"},
/* tr */ {IDCLS_MULTICOLOR_HANDLING_TR, "Çok renkliden yüksek çözünürlüðe geçiþin ele alýnma yöntemini seçin, (0: siyah&beyaz, 1: 2 renk, 2: 4 renk, 3: gri tonlu,  4: en iyi hücre renkleri)"},
#endif

/* gfxoutputdrv/doodledrv.c */
/* en */ {IDCLS_TED_LUM_HANDLING,    N_("Select the way the TED luminosity should be handled, (0: ignore, 1: dither)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TED_LUM_HANDLING_DA, "Angiv måden TED-lysstyrker skal håndteres på (0: ignorér, 1: blanding)"},
/* de */ {IDCLS_TED_LUM_HANDLING_DE, "Wähle, wie TED Leuchtstärke behandelt werden soll, (0: ignorieren, 1: dither)"},
/* es */ {IDCLS_TED_LUM_HANDLING_ES, "Seleccionar cómo se manejarán la luminosidad TED (0:ignorar, 1:dither)"},
/* fr */ {IDCLS_TED_LUM_HANDLING_FR, "Sélectionner la façon de traiter la luminosité TED, (0: ignorer, 1: tramer)"},
/* hu */ {IDCLS_TED_LUM_HANDLING_HU, ""},  /* fuzzy */
/* it */ {IDCLS_TED_LUM_HANDLING_IT, "Seleziona come gestire la luminosità TED, (0: ignora, 1: dither)"},
/* ko */ {IDCLS_TED_LUM_HANDLING_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_TED_LUM_HANDLING_NL, "Selecteer de manier waarop de TED kleuren moeten worden afgehandeld, (0: negeren, 1: dither)"},
/* pl */ {IDCLS_TED_LUM_HANDLING_PL, "Wybór sposobu obs³ugi jaskrawo¶ci TED (0: ignoruj, 1: ditheruj)"},
/* ru */ {IDCLS_TED_LUM_HANDLING_RU, "Select the way the TED luminosity should be handled, (0: ignore, 1: dither)"},
/* sv */ {IDCLS_TED_LUM_HANDLING_SV, "Välj hur TED-luminositet skall hanteras, (0: ignorera, 1: färgutjämna)"},
/* tr */ {IDCLS_TED_LUM_HANDLING_TR, "TED lüminozitesinin ele alýnma yöntemini seçin, (0: yok say, 1: dither)"},
#endif

/* gfxoutputdrv/doodledrv.c */
/* en */ {IDCLS_CRTC_TEXT_COLOR,    N_("Select the CRTC text color (0: white, 1: amber, 2: green)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_CRTC_TEXT_COLOR_DA, "Vælg CRTC tekstfarve (0: hvid, 1: ravgul, 2: grøn)"},
/* de */ {IDCLS_CRTC_TEXT_COLOR_DE, "Wähle CRTC Textfarbe (0: weiß, 1: Bernstein, 2: grün)"},
/* es */ {IDCLS_CRTC_TEXT_COLOR_ES, "Seleccionar color texto CRTC (0:blanco, 1:ámbar, 2:verde)"},
/* fr */ {IDCLS_CRTC_TEXT_COLOR_FR, "Sélectionner la couleur du texte CRTC (0: blanc, 1: ambre, 2: vert)"},
/* hu */ {IDCLS_CRTC_TEXT_COLOR_HU, ""},  /* fuzzy */
/* it */ {IDCLS_CRTC_TEXT_COLOR_IT, "Seleziona il colore del testo CRTC (0: bianco, 1: ambra, 2: verde)"},
/* ko */ {IDCLS_CRTC_TEXT_COLOR_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_CRTC_TEXT_COLOR_NL, "Selecteer de CRTC tekst kleur (0: wit, 1: amber, 2: groen)"},
/* pl */ {IDCLS_CRTC_TEXT_COLOR_PL, "Wybierz kolor tekstu CRTC (0: bia³y, 1: bursztynowy, 2: zielony)"},
/* ru */ {IDCLS_CRTC_TEXT_COLOR_RU, "Select the CRTC text color (0: white, 1: amber, 2: green)"},
/* sv */ {IDCLS_CRTC_TEXT_COLOR_SV, "Välj CRTC-textfärg (0: vit, 1: gul, 2: grön)"},
/* tr */ {IDCLS_CRTC_TEXT_COLOR_TR, "CRTC yazý rengini seçin (0: beyaz, 1: kehribar, 2: yeþil)"},
#endif

/* drive/drive-cmdline-options.c */
/* en */ {IDCLS_SET_DRIVE_EXTENSION_POLICY,    N_("Set drive 40 track extension policy (0: never, 1: ask, 2: on access)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_DA, "Angiv drevets regel for 40-spors udviddelse (0: aldrig, 1: spørg, 2: ved tilgang)"},
/* de */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_DE, "40 Spur Erweiterungsmethode (0: nie, 1: Rückfrage, 2: bei Bedarf)"},
/* es */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_ES, "Seleccionar política unidad disco extensión 40 pistas (0:nunca, 1:preguntar, 2:al acceder)"},
/* fr */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_FR, "Spécifier la règle d'extention 40 pistes (0: jamais, 1: demander, 2: à l'accès)"},
/* hu */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_HU, "Adja meg a 40 sávra kiterjesztés terjesztés módját (0: soha, 1: kérdés, 2: hozzáféréskor)"},
/* it */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_IT, "Imposta la politica di estensione a 40 tracce del drive (0: mai, 1: a richiesta, 2: all'accesso)"},
/* ko */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_NL, "Stel beleid voor 40-sporenuitbreiding drive in (0: nooit, 1: vraag, 2: bij toegang)"},
/* pl */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_PL, "Okre¶l zasadê rozszerzania do 40 ¶cie¿ek (0: nigdy, 1: pytaj, 2: przy dostêpie)"},
/* ru */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_RU, "Set drive 40 track extension policy (0: never, 1: ask, 2: on access)"},
/* sv */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_SV, "Ange regel för 40-spårsutökning (0: aldrig, 1: fråga, 2: vid åtkomst)"},
/* tr */ {IDCLS_SET_DRIVE_EXTENSION_POLICY_TR, "Sürücü 40 track büyüme kuralýný ayarla (0: asla, 1: sor, 2: eriþildiðinde)"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME,    N_("Specify name of 1540 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME_DE, "Name der 1540 DOS ROM Imagedatei wählen"},
/* es */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME_ES, "Especificar nombre imagen ROM de 1540 DOS "},
/* fr */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 1540"},
/* ko */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME_NL, "Geef de naam van het 1540 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 1540"},
/* ru */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_1540_DOS_ROM_NAME_TR, ""},  /* fuzzy */
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME,    N_("Specify name of 1541 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_DA, "Angiv navn på 1541-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_DE, "Name der 1541 DOS ROM Imagedatei wählen"},
/* es */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_ES, "Especificar nombre imagen ROM de 1541 DOS "},
/* fr */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1541 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_HU, "Adja meg az 1541 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 1541"},
/* ko */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_NL, "Geef de naam van het 1541 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 1541"},
/* ru */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_RU, "Specify name of 1541 DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_SV, "Ange namn på 1541-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1541_DOS_ROM_NAME_TR, "1541 DOS ROM imajýnýn ismini belirt"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME,    N_("Specify name of 1541-II DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_DA, "Angivn navn på 1541-II-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_DE, "Name der 1541-II DOS ROM Imagedatei wählen"},
/* es */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_ES, "Especificar nombre imagen ROM de 1541-II DOS"},
/* fr */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1541-II DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_HU, "Adja meg az 1541-II DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 1541-II"},
/* ko */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_NL, "Geef de naam van het 1541-II DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 1541-II"},
/* ru */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_RU, "Specify name of 1541-II DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_SV, "Ange namn på 1541-II-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_TR, "1541-II DOS ROM imajýnýn ismini belirt"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME,    N_("Specify name of 1570 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_DA, "Angivn navn på 1570-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_DE, "Name der 1570 DOS ROM Imagedatei wählen"},
/* es */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_ES, "Especificar nombre imagen ROM de 1570 DOS"},
/* fr */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1570 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_HU, "Adja meg az 1570 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 1570"},
/* ko */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_NL, "Geef de naam van het 1570 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 1570"},
/* ru */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_RU, "Specify name of 1570 DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_SV, "Ange namn på 1570-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1570_DOS_ROM_NAME_TR, "1570 DOS ROM imajýnýn ismini belirt"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME,    N_("Specify name of 1571 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_DA, "Angivn navn på 1571-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_DE, "Name der 1571 DOS ROM Imagedatei wählen"},
/* es */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_ES, "Especificar nombre imagen ROM de 1571 DOS "},
/* fr */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1571 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_HU, "Adja meg az 1571 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 1571"},
/* ko */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_NL, "Geef de naam van het 1571 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 1571"},
/* ru */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_RU, "Specify name of 1571 DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_SV, "Ange namn på 1571-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1571_DOS_ROM_NAME_TR, "1571 DOS ROM imajýnýn ismini belirt"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME,    N_("Specify name of 1581 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_DA, "Angivn navn på 1581-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_DE, "Name der 1581 DOS ROM Imagedatei wählen"},
/* es */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_ES, "Especificar nombre imagen ROM de 1581 DOS "},
/* fr */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1581 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_HU, "Adja meg az 1581 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 1581"},
/* ko */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_NL, "Geef de naam van het 1581 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 1581"},
/* ru */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_RU, "Specify name of 1581 DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_SV, "Ange namn på 1581-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1581_DOS_ROM_NAME_TR, "1581 DOS ROM imajýnýn ismini belirt"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME,    N_("Specify name of 2000 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME_DA, "Angiv navn på 2000-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME_DE, "Name der 2000 DOS ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME_ES, "Especificar nombre imagen 2000 DOS ROM"},
/* fr */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 2000 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del 2000 DOS"},
/* ko */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME_NL, "Geef de naam van het 2000 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 2000"},
/* ru */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME_RU, "Specify name of 2000 DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME_SV, "Ange namn på 2000-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_2000_DOS_ROM_NAME_TR, "2000 DOS ROM imaj ismini belirt"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME,    N_("Specify name of 4000 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME_DA, "Angiv navn på 4000-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME_DE, "Name der 4000 DOS ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME_ES, "Especificar nombre imagen 4000 DOS ROM"},
/* fr */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 4000 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del 4000 DOS"},
/* ko */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME_NL, "Geef de naam van het 4000 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 4000"},
/* ru */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME_RU, "Specify name of 4000 DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME_SV, "Ange namn på 4000-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_4000_DOS_ROM_NAME_TR, "4000 DOS ROM imaj ismini belirt"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_SET_IDLE_METHOD,    N_("Set drive idling method (0: no traps, 1: skip cycles, 2: trap idle)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_IDLE_METHOD_DA, "Vælg tomgangsmetode for diskettedrev (0: ingen traps, 1: spring over cykler, 2: trap ledig)"},
/* de */ {IDCLS_SET_IDLE_METHOD_DE, "Laufwerks idling Methode (0: kein Traps, 1: Zyklen verwerfen, 2: trap idle)"},
/* es */ {IDCLS_SET_IDLE_METHOD_ES, "Seleccionar método tiempo inactivo (0: sin intercepción, 1:saltar ciclos, 2:interceptar tiempos inactivos)"},
/* fr */ {IDCLS_SET_IDLE_METHOD_FR, "Spécifier la méthode d'idle (0: pas de traps, 1: sauter des cycles, 2: trap idle)"},
/* hu */ {IDCLS_SET_IDLE_METHOD_HU, "Lemezegység üresjárási módja (0: folytonos emuláció, 1: ciklusok kihagyása, 2: üres DOS ciklusok kihagyása)"},
/* it */ {IDCLS_SET_IDLE_METHOD_IT, "Imposta il metodo per rilevare l'inattività del drive (0: non rilevare, 1: salta cicli, 2: rileva inattività)"},
/* ko */ {IDCLS_SET_IDLE_METHOD_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_IDLE_METHOD_NL, "Stel de drive idlemethode in (0: geen traps, 1: sla cycli over, 2: trap idle)"},
/* pl */ {IDCLS_SET_IDLE_METHOD_PL, "Okre¶l metodê bezczynno¶ci napêdu (0: no traps, 1: opuszczaj cykle, 2: trap idle)"},
/* ru */ {IDCLS_SET_IDLE_METHOD_RU, "Set drive idling method (0: no traps, 1: skip cycles, 2: trap idle)"},
/* sv */ {IDCLS_SET_IDLE_METHOD_SV, "Ställ in väntemetod för diskettstation (0: fånta inge, 1: hoppa cykler, 2: fånga väntan)"},
/* tr */ {IDCLS_SET_IDLE_METHOD_TR, "Sürücü boþa vakit geçirme metodunu ayarlayýn (0: trap yok, 1: cyclelarý atla, 2: trap boþa vakit geçirme)"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RTC_SAVE,    N_("Enable saving of FD2000/4000 RTC data when changed.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DRIVE_RTC_SAVE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_DRIVE_RTC_SAVE_DE, "FD2000/4000 RTC Daten bei Änderung speichern."},
/* es */ {IDCLS_ENABLE_DRIVE_RTC_SAVE_ES, "Permitir grabación de datos cuando cambien de  FD2000/4000 RTC."},
/* fr */ {IDCLS_ENABLE_DRIVE_RTC_SAVE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_DRIVE_RTC_SAVE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_DRIVE_RTC_SAVE_IT, "Attiva il salvataggio dei dati RTC del FD2000/4000 alla modifica."},
/* ko */ {IDCLS_ENABLE_DRIVE_RTC_SAVE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DRIVE_RTC_SAVE_NL, "Opslaan van de FD2000/4000 RTC gegevens bij wijziging inschakelen."},
/* pl */ {IDCLS_ENABLE_DRIVE_RTC_SAVE_PL, "Zapisuj dane zegara czasu rzeczywistego FD2000/4000 przy zmianie"},
/* ru */ {IDCLS_ENABLE_DRIVE_RTC_SAVE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_DRIVE_RTC_SAVE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_DRIVE_RTC_SAVE_TR, ""},  /* fuzzy */
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RTC_SAVE,    N_("Disable saving of FD2000/4000 RTC data when changed.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DRIVE_RTC_SAVE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_DRIVE_RTC_SAVE_DE, "FD2000/4000 RTC Daten bei Änderung nicht speichern."},
/* es */ {IDCLS_DISABLE_DRIVE_RTC_SAVE_ES, "Deshabilitar grabación de datos cuando cambien de  FD2000/4000 RTC."},
/* fr */ {IDCLS_DISABLE_DRIVE_RTC_SAVE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_DRIVE_RTC_SAVE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_DRIVE_RTC_SAVE_IT, "Disattiva il salvataggio dei dati RTC del FD2000/4000 alla modifica."},
/* ko */ {IDCLS_DISABLE_DRIVE_RTC_SAVE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DRIVE_RTC_SAVE_NL, "Opslaan van de FD2000/4000 RTC gegevens bij wijziging uitschakelen."},
/* pl */ {IDCLS_DISABLE_DRIVE_RTC_SAVE_PL, "Nie zapisuj danych zegara czasu rzeczywistego FD2000/4000 przy zmianie"},
/* ru */ {IDCLS_DISABLE_DRIVE_RTC_SAVE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_DRIVE_RTC_SAVE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_DRIVE_RTC_SAVE_TR, ""},  /* fuzzy */
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_2000,    N_("Enable 8KB RAM expansion at $2000-$3FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DRIVE_RAM_2000_DA, "Aktivér 8kB RAM-udviddelse på $2000-$3FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_2000_DE, "8KB RAM Erweiterung bei $2000-3fff aktivieren"},
/* es */ {IDCLS_ENABLE_DRIVE_RAM_2000_ES, "Permitir expansión RAM de 8KB en $2000-$3FFF"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_2000_FR, "Activer l'exansion de 8KB RAM à $2000-$3FFF"},
/* hu */ {IDCLS_ENABLE_DRIVE_RAM_2000_HU, "8KB RAM kiterjeszés engedélyezése a $2000-$3FFF címen"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_2000_IT, "Attiva l'espansione di RAM di 8KB a $2000-$3FFF"},
/* ko */ {IDCLS_ENABLE_DRIVE_RAM_2000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_2000_NL, "8KB RAM uitbreiding op adres $2000-$3FFF inschakelen"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_2000_PL, "W³±cz rozszerzenie 8KB RAM w $2000-$3FFF"},
/* ru */ {IDCLS_ENABLE_DRIVE_RAM_2000_RU, "Enable 8KB RAM expansion at $2000-$3FFF"},
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_2000_SV, "Aktivera 8KB RAM-expansion på $2000-$3FFF"},
/* tr */ {IDCLS_ENABLE_DRIVE_RAM_2000_TR, "$2000-$3FFF arasýnda 8KB RAM geniþletmeyi aktif et"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_2000,    N_("Disable 8KB RAM expansion at $2000-$3FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DRIVE_RAM_2000_DA, "Deaktivér 8kB RAM-udviddelse på $2000-$3FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_2000_DE, "8KB RAM Erweiterung bei $2000-3fff deaktivieren"},
/* es */ {IDCLS_DISABLE_DRIVE_RAM_2000_ES, "Deshabilitar expansión RAM de 8KB en $2000-$3FFF"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_2000_FR, "Désactiver l'exansion de 8KB RAM à $2000-$3FFF"},
/* hu */ {IDCLS_DISABLE_DRIVE_RAM_2000_HU, "8KB RAM kiterjeszés tiltása a $2000-$3FFF címen"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_2000_IT, "Disattiva l'espansione di RAM di 8KB a $2000-$3FFF"},
/* ko */ {IDCLS_DISABLE_DRIVE_RAM_2000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_2000_NL, "8KB RAM uitbreiding op adres $2000-$3FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_2000_PL, "Wy³±cz rozszerzenie 8KB RAM w $2000-$3FFF"},
/* ru */ {IDCLS_DISABLE_DRIVE_RAM_2000_RU, "Disable 8KB RAM expansion at $2000-$3FFF"},
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_2000_SV, "Inaktivera 8KB RAM-expansion på $2000-$3FFF"},
/* tr */ {IDCLS_DISABLE_DRIVE_RAM_2000_TR, "$2000-$3FFF arasýnda 8KB RAM geniþletmeyi pasifleþtir"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_4000,    N_("Enable 8KB RAM expansion at $4000-$5FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DRIVE_RAM_4000_DA, "Aktivér 8kB RAM-udviddelse på $4000-$5FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_4000_DE, "8KB RAM Erweiterung bei $4000-5fff aktivieren"},
/* es */ {IDCLS_ENABLE_DRIVE_RAM_4000_ES, "Permitir expansión RAM de 8KB en $4000-$5FFF"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_4000_FR, "Activer l'exansion de 8KB RAM à $4000-$5FFF"},
/* hu */ {IDCLS_ENABLE_DRIVE_RAM_4000_HU, "8KB RAM kiterjeszés engedélyezése a $4000-$5FFF címen"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_4000_IT, "Attiva l'espansione di RAM di 8KB a $4000-$5FFF"},
/* ko */ {IDCLS_ENABLE_DRIVE_RAM_4000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_4000_NL, "8KB RAM uitbreiding op adres $4000-$5FFF inschakelen"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_4000_PL, "W³±cz rozszerzenie 8KB RAM w $4000-$5FFF"},
/* ru */ {IDCLS_ENABLE_DRIVE_RAM_4000_RU, "Enable 8KB RAM expansion at $4000-$5FFF"},
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_4000_SV, "Aktivera 8KB RAM-expansion på $4000-$5FFF"},
/* tr */ {IDCLS_ENABLE_DRIVE_RAM_4000_TR, "$4000-$5FFF arasýnda 8KB RAM geniþletmeyi aktif et"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_4000,    N_("Disable 8KB RAM expansion at $4000-$5FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DRIVE_RAM_4000_DA, "Deaktivér 8kB RAM-udviddelse på $4000-$5FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_4000_DE, "8KB RAM Erweiterung bei $4000-5fff deaktivieren"},
/* es */ {IDCLS_DISABLE_DRIVE_RAM_4000_ES, "Deshabilitar expansión RAM de 8KB en $4000-$5FFF"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_4000_FR, "Désactiver l'exansion de 8KB RAM à $4000-$5FFF"},
/* hu */ {IDCLS_DISABLE_DRIVE_RAM_4000_HU, "8KB RAM kiterjeszés tiltása a $4000-$5FFF címen"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_4000_IT, "Disattiva l'espansione di RAM di 8KB a $4000-$5FFF"},
/* ko */ {IDCLS_DISABLE_DRIVE_RAM_4000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_4000_NL, "8KB RAM uitbreiding op adres $4000-$5FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_4000_PL, "Wy³±cz rozszerzenie 8KB RAM w $4000-$5FFF"},
/* ru */ {IDCLS_DISABLE_DRIVE_RAM_4000_RU, "Disable 8KB RAM expansion at $4000-$5FFF"},
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_4000_SV, "Inaktivera 8KB RAM-expansion på $4000-$5FFF"},
/* tr */ {IDCLS_DISABLE_DRIVE_RAM_4000_TR, "$4000-$5FFF arasýnda 8KB RAM geniþletmeyi pasifleþtir"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_6000,    N_("Enable 8KB RAM expansion at $6000-$7FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DRIVE_RAM_6000_DA, "Aktivér 8kB RAM-udviddelse på $6000-$7FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_6000_DE, "8KB RAM Erweiterung bei $6000-7fff aktivieren"},
/* es */ {IDCLS_ENABLE_DRIVE_RAM_6000_ES, "Permitir expansión RAM de 8KB en $6000-$7FFF"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_6000_FR, "Activer l'exansion de 8KB RAM à $6000-$7FFF"},
/* hu */ {IDCLS_ENABLE_DRIVE_RAM_6000_HU, "8KB RAM kiterjeszés engedélyezése a $6000-$7FFF címen"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_6000_IT, "Attiva l'espansione di RAM di 8KB a $6000-$7FFF"},
/* ko */ {IDCLS_ENABLE_DRIVE_RAM_6000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_6000_NL, "8KB RAM uitbreiding op adres $6000-$7FFF inschakelen"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_6000_PL, "W³±cz rozszerzenie 8KB RAM w $6000-$7FFF"},
/* ru */ {IDCLS_ENABLE_DRIVE_RAM_6000_RU, "Enable 8KB RAM expansion at $6000-$7FFF"},
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_6000_SV, "Aktivera 8KB RAM-expansion på $6000-$7FFF"},
/* tr */ {IDCLS_ENABLE_DRIVE_RAM_6000_TR, "$6000-$7FFF arasýnda 8KB RAM geniþletmeyi aktif et"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_6000,    N_("Disable 8KB RAM expansion at $6000-$7FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DRIVE_RAM_6000_DA, "Deaktivér 8kB RAM-udviddelse på $6000-$7FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_6000_DE, "8KB RAM Erweiterung bei $6000-7fff deaktivieren"},
/* es */ {IDCLS_DISABLE_DRIVE_RAM_6000_ES, "Deshabilitar expansión RAM de 8KB en $6000-$7FFF"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_6000_FR, "Désactiver l'exansion de 8KB RAM à $6000-$7FFF"},
/* hu */ {IDCLS_DISABLE_DRIVE_RAM_6000_HU, "8KB RAM kiterjeszés tiltása a $6000-$7FFF címen"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_6000_IT, "Disattiva l'espansione di RAM di 8KB a $6000-$7FFF"},
/* ko */ {IDCLS_DISABLE_DRIVE_RAM_6000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_6000_NL, "8KB RAM uitbreiding op adres $6000-$7FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_6000_PL, "Wy³±cz rozszerzenie 8KB RAM w $6000-$7FFF"},
/* ru */ {IDCLS_DISABLE_DRIVE_RAM_6000_RU, "Disable 8KB RAM expansion at $6000-$7FFF"},
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_6000_SV, "Inaktivera 8KB RAM-expansion på $6000-$7FFF"},
/* tr */ {IDCLS_DISABLE_DRIVE_RAM_6000_TR, "$6000-$7FFF arasýnda 8KB RAM geniþletmeyi pasifleþtir"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_8000,    N_("Enable 8KB RAM expansion at $8000-$9FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DRIVE_RAM_8000_DA, "Aktivér 8kB RAM-udviddelse på $8000-$9FFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_8000_DE, "8KB RAM Erweiterung bei $8000-9fff aktivieren"},
/* es */ {IDCLS_ENABLE_DRIVE_RAM_8000_ES, "Permitir expansión RAM de 8KB en $8000-$9FFF"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_8000_FR, "Activer l'exansion de 8KB RAM à $8000-$9FFF"},
/* hu */ {IDCLS_ENABLE_DRIVE_RAM_8000_HU, "8KB RAM kiterjeszés engedélyezése a $8000-$9FFF címen"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_8000_IT, "Attiva l'espansione di RAM di 8KB a $8000-$9FFF"},
/* ko */ {IDCLS_ENABLE_DRIVE_RAM_8000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_8000_NL, "8KB RAM uitbreiding op adres $8000-$9FFF inschakelen"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_8000_PL, "W³±cz rozszerzenie 8KB RAM w $8000-$9FFF"},
/* ru */ {IDCLS_ENABLE_DRIVE_RAM_8000_RU, "Enable 8KB RAM expansion at $8000-$9FFF"},
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_8000_SV, "Aktivera 8KB RAM-expansion på $8000-$9FFF"},
/* tr */ {IDCLS_ENABLE_DRIVE_RAM_8000_TR, "$8000-$9FFF arasýnda 8KB RAM geniþletmeyi aktif et"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_8000,    N_("Disable 8KB RAM expansion at $8000-$9FFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DRIVE_RAM_8000_DA, "Deaktivér 8kB RAM-udviddelse på $8000-$9FFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_8000_DE, "8KB RAM Erweiterung bei $8000-9fff deaktivieren"},
/* es */ {IDCLS_DISABLE_DRIVE_RAM_8000_ES, "Deshabilitar expansión RAM de 8KB en $8000-$9FFF"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_8000_FR, "Désactiver l'exansion de 8KB RAM à $8000-$9FFF"},
/* hu */ {IDCLS_DISABLE_DRIVE_RAM_8000_HU, "8KB RAM kiterjeszés tiltása a $8000-$9FFF címen"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_8000_IT, "Disattiva l'espansione di RAM di 8KB a $8000-$9FFF"},
/* ko */ {IDCLS_DISABLE_DRIVE_RAM_8000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_8000_NL, "8KB RAM uitbreiding op adres $8000-$9FFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_8000_PL, "Wy³±cz rozszerzenie 8KB RAM w $8000-$9FFF"},
/* ru */ {IDCLS_DISABLE_DRIVE_RAM_8000_RU, "Disable 8KB RAM expansion at $8000-$9FFF"},
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_8000_SV, "Inaktivera 8KB RAM-expansion på $8000-$9FFF"},
/* tr */ {IDCLS_DISABLE_DRIVE_RAM_8000_TR, "$8000-$9FFF arasýnda 8KB RAM geniþletmeyi pasifleþtir"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_ENABLE_DRIVE_RAM_A000,    N_("Enable 8KB RAM expansion at $A000-$BFFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DRIVE_RAM_A000_DA, "Aktivér 8kB RAM-udviddelse på $A000-$BFFF"},
/* de */ {IDCLS_ENABLE_DRIVE_RAM_A000_DE, "8KB RAM Erweiterung bei $A000-Bfff aktivieren"},
/* es */ {IDCLS_ENABLE_DRIVE_RAM_A000_ES, "Permitir expansión RAM de 8KB en $A000-$BFFF"},
/* fr */ {IDCLS_ENABLE_DRIVE_RAM_A000_FR, "Activer l'exansion de 8KB RAM à $A000-$BFFF"},
/* hu */ {IDCLS_ENABLE_DRIVE_RAM_A000_HU, "8KB RAM kiterjeszés engedélyezése a $A000-$BFFF címen"},
/* it */ {IDCLS_ENABLE_DRIVE_RAM_A000_IT, "Attiva l'espansione di RAM di 8KB a $A000-$BFFF"},
/* ko */ {IDCLS_ENABLE_DRIVE_RAM_A000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DRIVE_RAM_A000_NL, "8KB RAM uitbreiding op adres $A000-$BFFF inschakelen"},
/* pl */ {IDCLS_ENABLE_DRIVE_RAM_A000_PL, "W³±cz rozszerzenie 8KB RAM w $A000-$BFFF"},
/* ru */ {IDCLS_ENABLE_DRIVE_RAM_A000_RU, "Enable 8KB RAM expansion at $A000-$BFFF"},
/* sv */ {IDCLS_ENABLE_DRIVE_RAM_A000_SV, "Aktivera 8KB RAM-expansion på $A000-$BFFF"},
/* tr */ {IDCLS_ENABLE_DRIVE_RAM_A000_TR, "$A000-$BFFF arasýnda 8KB RAM geniþletmeyi aktif et"},
#endif

/* drive/iec/iec-cmdline-options.c */
/* en */ {IDCLS_DISABLE_DRIVE_RAM_A000,    N_("Disable 8KB RAM expansion at $A000-$BFFF")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DRIVE_RAM_A000_DA, "Deaktivér 8kB RAM-udviddelse på $A000-$BFFF"},
/* de */ {IDCLS_DISABLE_DRIVE_RAM_A000_DE, "8KB RAM Erweiterung bei $A000-Bfff deaktivieren"},
/* es */ {IDCLS_DISABLE_DRIVE_RAM_A000_ES, "Deshabilitar expansión RAM de 8KB en $A000-$BFFF"},
/* fr */ {IDCLS_DISABLE_DRIVE_RAM_A000_FR, "Désactiver l'exansion de 8KB RAM à $A000-$BFFF"},
/* hu */ {IDCLS_DISABLE_DRIVE_RAM_A000_HU, "8KB RAM kiterjeszés tiltása a $A000-$BFFF címen"},
/* it */ {IDCLS_DISABLE_DRIVE_RAM_A000_IT, "Disattiva l'espansione di RAM di 8KB a $A000-$BFFF"},
/* ko */ {IDCLS_DISABLE_DRIVE_RAM_A000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DRIVE_RAM_A000_NL, "8KB RAM uitbreiding op adres $A000-$BFFF uitschakelen"},
/* pl */ {IDCLS_DISABLE_DRIVE_RAM_A000_PL, "Wy³±cz rozszerzenie 8KB RAM w $A000-$BFFF"},
/* ru */ {IDCLS_DISABLE_DRIVE_RAM_A000_RU, "Disable 8KB RAM expansion at $A000-$BFFF"},
/* sv */ {IDCLS_DISABLE_DRIVE_RAM_A000_SV, "Inaktivera 8KB RAM-expansion på $A000-$BFFF"},
/* tr */ {IDCLS_DISABLE_DRIVE_RAM_A000_TR, "$A000-$BFFF arasýnda 8KB RAM geniþletmeyi pasifleþtir"},
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_PAR_CABLE_C64EXP_TYPE,    N_("Set parallel cable type (0: none, 1: standard, 2: Dolphin DOS 3, 3: Formel 64)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PAR_CABLE_C64EXP_TYPE_DA, "Vælg typen af parallelkabel (0: ingen, 1: standard, 2: Dolphin DOS 3 3: Formel 64)"},
/* de */ {IDCLS_PAR_CABLE_C64EXP_TYPE_DE, "Parallelkabeltyp (0: Kein, 1: Standard, 2: Dolphin DOS 3, 3: Formel 64)"},
/* es */ {IDCLS_PAR_CABLE_C64EXP_TYPE_ES, "Seleccionar tipo de cable paralelo (0:no, 1:estándar, 2:Dolphin DOS 3, 3:Formel 64)"},
/* fr */ {IDCLS_PAR_CABLE_C64EXP_TYPE_FR, "Définir le type de câble parallèle (0: aucun, 1: standard, 2: Dolphin DOS 3, 3: Formel 64)"},
/* hu */ {IDCLS_PAR_CABLE_C64EXP_TYPE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_PAR_CABLE_C64EXP_TYPE_IT, "Imposta il tipo di cavo parallelo (0: none, 1: standard, 2: Dolphin DOS 3, 3: Formel 64)"},
/* ko */ {IDCLS_PAR_CABLE_C64EXP_TYPE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PAR_CABLE_C64EXP_TYPE_NL, "Stel parallelle kabelsoort in (0: geen, 1: standaard, 2: Dolphin DOS 3, 3: Formel 64)"},
/* pl */ {IDCLS_PAR_CABLE_C64EXP_TYPE_PL, "Okre¶l typ ³±cza równoleg³ego (0: brak, 1: zwyk³e, 2: Dolphin DOS 3, 3: Formel 64)"},
/* ru */ {IDCLS_PAR_CABLE_C64EXP_TYPE_RU, "Set parallel cable type (0: none, 1: standard, 2: Dolphin DOS 3, 3: Formel 64)"},
/* sv */ {IDCLS_PAR_CABLE_C64EXP_TYPE_SV, "Välj typ av parallellkabel (0: ingen, 1: standard, 2: Dolphin DOS 3, 3: Formel 64)"},
/* tr */ {IDCLS_PAR_CABLE_C64EXP_TYPE_TR, "Paralel kablo tipini belirle (0: yok, 1: standart, 2: Dolphin DOS 3, 3: Formel 64)"},
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_ENABLE_PROFDOS,    N_("Enable Professional DOS")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_PROFDOS_DA, "Aktivér Professional DOS"},
/* de */ {IDCLS_ENABLE_PROFDOS_DE, "Professional DOS aktivieren"},
/* es */ {IDCLS_ENABLE_PROFDOS_ES, "Permitir DOS profesional"},
/* fr */ {IDCLS_ENABLE_PROFDOS_FR, "Activer le DOS Professionnel"},
/* hu */ {IDCLS_ENABLE_PROFDOS_HU, "Professional DOS engedélyezése"},
/* it */ {IDCLS_ENABLE_PROFDOS_IT, "Attiva Professional DOS"},
/* ko */ {IDCLS_ENABLE_PROFDOS_KO, "Professional DOS »ç¿ë°¡´ÉÇÏ°Ô ÇÏ±â"},
/* nl */ {IDCLS_ENABLE_PROFDOS_NL, "Activeer Professional DOS"},
/* pl */ {IDCLS_ENABLE_PROFDOS_PL, "W³±cz Professional DOS"},
/* ru */ {IDCLS_ENABLE_PROFDOS_RU, "Enable Professional DOS"},
/* sv */ {IDCLS_ENABLE_PROFDOS_SV, "Aktivera Professional DOS"},
/* tr */ {IDCLS_ENABLE_PROFDOS_TR, "Professional DOS'u aktif et"},
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_DISABLE_PROFDOS,    N_("Disable Professional DOS")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_PROFDOS_DA, "Deaktivér Professional DOS"},
/* de */ {IDCLS_DISABLE_PROFDOS_DE, "Professional DOS deaktivieren"},
/* es */ {IDCLS_DISABLE_PROFDOS_ES, "Deshabilitar Professional DOS"},
/* fr */ {IDCLS_DISABLE_PROFDOS_FR, "Désactiver le DOS Professionnel"},
/* hu */ {IDCLS_DISABLE_PROFDOS_HU, "Professional DOS tiltása"},
/* it */ {IDCLS_DISABLE_PROFDOS_IT, "Disattiva Professional DOS"},
/* ko */ {IDCLS_DISABLE_PROFDOS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_PROFDOS_NL, "Professional DOS uitschakelen"},
/* pl */ {IDCLS_DISABLE_PROFDOS_PL, "Wy³±cz Professional DOS"},
/* ru */ {IDCLS_DISABLE_PROFDOS_RU, "Disable Professional DOS"},
/* sv */ {IDCLS_DISABLE_PROFDOS_SV, "Inaktivera Professional DOS"},
/* tr */ {IDCLS_DISABLE_PROFDOS_TR, "Professional DOS'u pasifleþtir"},
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME,    N_("Specify name of Professional DOS 1571 ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_DA, "Angiv navn på Professional DOS 1571-ROM-image"},
/* de */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_DE, "Name von Professional DOS 1571 Datei wählen"},
/* es */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_ES, "Especificar nombre imagen ROM Professional DOS 1571"},
/* fr */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_FR, "Spécifier le nom du nouveau ROM Professional DOS 1571"},
/* hu */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_HU, "Adja meg a Professional DOS 1571 ROM képmást"},
/* it */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_IT, "Specifica il nome immagine della ROM del Professional DOS 1571"},
/* ko */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_NL, "Geef de naam van het Professional DOS 1571 ROM bestand"},
/* pl */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM Professional DOS 1571"},
/* ru */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_RU, "Specify name of Professional DOS 1571 ROM image"},
/* sv */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_SV, "Ange namn på Professional DOS 1571-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_PROFDOS_1571_ROM_NAME_TR, "Professional DOS 1571 ROM imaj ismini belirt"},
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SUPERCARD,    N_("Enable SuperCard+")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SUPERCARD_DA, "Aktivér SuperCard+"},
/* de */ {IDCLS_ENABLE_SUPERCARD_DE, "SuperCard+ aktivieren"},
/* es */ {IDCLS_ENABLE_SUPERCARD_ES, "Permitir SuperCard+"},
/* fr */ {IDCLS_ENABLE_SUPERCARD_FR, "Activer SuperCard+"},
/* hu */ {IDCLS_ENABLE_SUPERCARD_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SUPERCARD_IT, "Attiva SuperCard+"},
/* ko */ {IDCLS_ENABLE_SUPERCARD_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SUPERCARD_NL, "Activeer SuperCard+"},
/* pl */ {IDCLS_ENABLE_SUPERCARD_PL, "W³±cz SuperCard+"},
/* ru */ {IDCLS_ENABLE_SUPERCARD_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SUPERCARD_SV, "Aktivera SuperCard+"},
/* tr */ {IDCLS_ENABLE_SUPERCARD_TR, "SuperCard+'ý aktif et"},
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SUPERCARD,    N_("Disable SuperCard+")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SUPERCARD_DA, "Deaktivér SuperCard+"},
/* de */ {IDCLS_DISABLE_SUPERCARD_DE, "SuperCard+ deaktivieren"},
/* es */ {IDCLS_DISABLE_SUPERCARD_ES, "Deshabilitar SuperCard+"},
/* fr */ {IDCLS_DISABLE_SUPERCARD_FR, "Désactiver SuperCard+"},
/* hu */ {IDCLS_DISABLE_SUPERCARD_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_SUPERCARD_IT, "Disattiva SuperCard+"},
/* ko */ {IDCLS_DISABLE_SUPERCARD_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SUPERCARD_NL, "SuperCard+ uitschakelen"},
/* pl */ {IDCLS_DISABLE_SUPERCARD_PL, "Wy³±cz SuperCard+"},
/* ru */ {IDCLS_DISABLE_SUPERCARD_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SUPERCARD_SV, "Inaktivera SuperCard+"},
/* tr */ {IDCLS_DISABLE_SUPERCARD_TR, "SuperCard+'ý pasifleþtir"},
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME,    N_("Specify name of SuperCard+ ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME_DA, "Angiv navn på SuperCard+ ROM-image"},
/* de */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME_DE, "Name von SuperCard+ ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME_ES, "Especificar nombre de Imagen ROM SuperCard+"},
/* fr */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME_FR, "Spécifier le nom de l'image ROM SuperCard+"},
/* hu */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME_IT, "Specifica il nome immagine della ROM del SuperCard+"},
/* ko */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME_NL, "Geef de naam van het SuperCard+ ROM bestand"},
/* pl */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM SuperCard+"},
/* ru */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME_SV, "Ange namn på SuperCard+-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_SUPERCARD_ROM_NAME_TR, "SuperCard+ ROM imajýnýn ismini belirt"},
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_ENABLE_STARDOS,    N_("Enable StarDOS")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_STARDOS_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_STARDOS_DE, "StarDOS aktivieren"},
/* es */ {IDCLS_ENABLE_STARDOS_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_STARDOS_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_STARDOS_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_STARDOS_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_STARDOS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_STARDOS_NL, "Activeer StarDOS"},
/* pl */ {IDCLS_ENABLE_STARDOS_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_ENABLE_STARDOS_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_STARDOS_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_STARDOS_TR, ""},  /* fuzzy */
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_DISABLE_STARDOS,    N_("Disable StarDOS")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_STARDOS_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_STARDOS_DE, "StarDOS deaktivieren"},
/* es */ {IDCLS_DISABLE_STARDOS_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_STARDOS_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_STARDOS_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_STARDOS_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_STARDOS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_STARDOS_NL, "StarDOS uitschakelen"},
/* pl */ {IDCLS_DISABLE_STARDOS_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_DISABLE_STARDOS_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_STARDOS_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_STARDOS_TR, ""},  /* fuzzy */
#endif

/* drive/iec/c64exp/c64exp-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_STARDOS_ROM_NAME,    N_("Specify name of StarDOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_STARDOS_ROM_NAME_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_STARDOS_ROM_NAME_DE, "Name der StarDOS DOS ROM Imagedatei wählen"},
/* es */ {IDCLS_SPECIFY_STARDOS_ROM_NAME_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_STARDOS_ROM_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_STARDOS_ROM_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_STARDOS_ROM_NAME_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SPECIFY_STARDOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_STARDOS_ROM_NAME_NL, "Geef de naam van het StarDOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_STARDOS_ROM_NAME_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_SPECIFY_STARDOS_ROM_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_STARDOS_ROM_NAME_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_STARDOS_ROM_NAME_TR, ""},  /* fuzzy */
#endif

/* drive/iec/plus4exp/plus4exp-cmdline-options.c */
/* en */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE,    N_("Set parallel cable type (0: none, 1: standard)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_DA, "Vælg typen af parallelkabel parallellkabel (0: ingen, 1: standard)"},
/* de */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_DE, "Parallelkabeltyp (0: kein, 1: Standard)"},
/* es */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_ES, "Seleccionar tipo de cable paralelo (0:no, 1:standard)"},
/* fr */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_FR, "Définir le type de câble parallèle (0: aucun, 1: standard)"},
/* hu */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_HU, "Adja meg a párhuzamos kábel típusát (0: nincs, 1: standard)"},
/* it */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_IT, "Imposta il tipo di cavo parallelo (0: nessuno, 1: standard)"},
/* ko */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_NL, "Stel parallelle kabelsoort in (0: geen, 1: standaard)"},
/* pl */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_PL, "Ustaw typ kabla portu równoleg³ego (0: brak, 1: typowy)"},
/* ru */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_RU, "Set parallel cable type (0: none, 1: standard)"},
/* sv */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_SV, "Välj typ av parallellkabel (0: ingen, 1: standard)"},
/* tr */ {IDCLS_PAR_CABLE_PLUS4EXP_TYPE_TR, "Paralel kablo tipini ayarlayýn (0: yok, 1: standart)"},
#endif

/* drive/iec128dcr/iec128dcr-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME,    N_("Specify name of 1571CR DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_DA, "Angiv navn på 1571CR-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_DE, "Name der 1571CR DOS ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_ES, "Especificar nombre imagen DOS ROM 1571CR"},
/* fr */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1571CR DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_HU, "Adja meg az 1571CR DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 1571CR"},
/* ko */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_NL, "Geef de naam van het 1571CR DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 1571CR"},
/* ru */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_RU, "Specify name of 1571CR DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_SV, "Ange namn på 1571CR-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_TR, "1571CR DOS ROM imaj ismini belirt"},
#endif

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME,    N_("Specify name of 2031 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_DA, "Angiv navn på 2031-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_DE, "Name der 2031 DOS ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_ES, "Especificar nombre imagen DOS ROM 2031"},
/* fr */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 2031 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_HU, "Adja meg a 2031 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 2031"},
/* ko */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_NL, "Geef de naam van het 2031 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 2031"},
/* ru */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_RU, "Specify name of 2031 DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_SV, "Ange namn på 2031-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_2031_DOS_ROM_NAME_TR, "2031 DOS ROM imaj ismini belirt"},
#endif

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME,    N_("Specify name of 2040 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_DA, "Angiv navn på 2040-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_DE, "Name der 2040 DOS ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_ES, "Especificar nombre imagen DOS ROM 2040"},
/* fr */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 2040 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_HU, "Adja meg a 2040 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 2040"},
/* ko */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_NL, "Geef de naam van het 2040 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 2040"},
/* ru */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_RU, "Specify name of 2040 DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_SV, "Ange namn på 2040-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_2040_DOS_ROM_NAME_TR, "2040 DOS ROM imaj ismini belirt"},
#endif

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME,    N_("Specify name of 3040 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_DA, "Angiv navn på 3040-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_DE, "Name der 3040 DOS ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_ES, "Especificar nombre imagen DOS ROM 3040"},
/* fr */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 3040 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_HU, "Adja meg a 3040 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 3040"},
/* ko */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_NL, "Geef de naam van het 3040 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 3040"},
/* ru */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_RU, "Specify name of 3040 DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_SV, "Ange namn på 3040-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_3040_DOS_ROM_NAME_TR, "3040 DOS ROM imaj ismini belirt"},
#endif

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME,    N_("Specify name of 4040 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_DA, "Angiv navn på 4040-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_DE, "Name der 4040 DOS ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_ES, "Especificar nombre imagen DOS ROM 4040"},
/* fr */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 4040 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_HU, "Adja meg a 4040 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 4040"},
/* ko */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_NL, "Geef de naam van het 4040 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 4040"},
/* ru */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_RU, "Specify name of 4040 DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_SV, "Ange namn på 4040-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_4040_DOS_ROM_NAME_TR, "4040 DOS ROM imaj ismini belirt"},
#endif

/* drive/ieee/ieee-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME,    N_("Specify name of 1001/8050/8250 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_DA, "Angiv navn på 1001/8050/8250-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_DE, "Name der 1001/8040/8205 DOS ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_ES, "Especificar nombre imagen DOS ROM 1001/8050/8250"},
/* fr */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1001/8050/8250 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_HU, "Adja meg az 1001/8050/8250 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 1001/8050/8250"},
/* ko */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_NL, "Geef de naam van het 1001/8050/8250 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 1001/8050/8250"},
/* ru */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_RU, "Specify name of 1001/8050/8250 DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_SV, "Ange namn på 1001/8050/8250-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1001_DOS_ROM_NAME_TR, "1001/8050/8250 DOS ROM imaj ismini belirt"},
#endif

/* drive/tcbm/tcbm-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME,    N_("Specify name of 1551 DOS ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_DA, "Angiv navn på 1551-DOS-ROM-image"},
/* de */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_DE, "Name der 1551 DOS ROM Datei wählen"},
/* es */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_ES, "Especificar nombre imagen DOS ROM 1551"},
/* fr */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_FR, "Spécifier le nom de l'image 1551 DOS ROM"},
/* hu */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_HU, "Adja meg az 1551 DOS ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_IT, "Specifica il nome immagine della ROM del DOS del 1551"},
/* ko */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_NL, "Geef de naam van het 1551 DOS ROM bestand"},
/* pl */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM DOS-a 1551"},
/* ru */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_RU, "Specify name of 1551 DOS ROM image"},
/* sv */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_SV, "Ange namn på 1551-DOS-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_1551_DOS_ROM_NAME_TR, "1551 DOS ROM imaj ismini belirt"},
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8,    N_("Use <name> as directory for file system device #8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_DA, "Brug <navn> som katalog for filsystembaseret drev #8"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_DE, "Benutze <Name> für Verzeichnis beim Verzeichniszugriff für Gerät #8"},
/* es */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_ES, "Usar <nombre> como directorio fichero sistema periférico #8"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_FR, "Utiliser le répertoire <nom> comme système de fichiers pour le lecteur #8"},
/* hu */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_HU, "<név> könyvtár használata az #8-as egység fájlrendszerének"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_IT, "Usa <nome> come directory per il file system della periferica #8"},
/* ko */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_NL, "Gebruik <naam> als directory voor bestandssysteemapparaat #8"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_PL, "Uzyj <nazwa> jako katalog urz±dzenia systemu plików #8"},
/* ru */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_RU, "Use <name> as directory for file system device #8"},
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_SV, "Ange <namn> som katalog för filsystemsbaserad enhet 8"},
/* tr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_TR, "<isim>'i aygýt #8'in dosya sistemi dizini olarak kullan"},
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9,    N_("Use <name> as directory for file system device #9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_DA, "Brug <navn> som katalog for filsystembaseret drev #9"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_DE, "Benutze <Name> für Verzeichnis beim Verzeichniszugriff für Gerät #9"},
/* es */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_ES, "Usar <nombre> como directorio fichero sistema periférico #9"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_FR, "Utiliser le répertoire <nom> comme système de fichiers pour le lecteur #9"},
/* hu */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_HU, "<név> könyvtár használata az #9-es egység fájlrendszerének"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_IT, "Usa <nome> come directory per il file system della periferica #9"},
/* ko */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_NL, "Gebruik <naam> als directory voor bestandssysteemapparaat #9"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_PL, "Uzyj <nazwa> jako katalog urz±dzenia systemu plików #9"},
/* ru */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_RU, "Use <name> as directory for file system device #9"},
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_SV, "Ange <namn> som katalog för filsystemsbaserad enhet 9"},
/* tr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_TR, "<isim>'i aygýt #9'un dosya sistemi dizini olarak kullan"},
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10,    N_("Use <name> as directory for file system device #10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_DA, "Brug <navn> som katalog for filsystembaseret drev #10"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_DE, "Benutze <Name> für Verzeichnis beim Verzeichniszugriff für Gerät #10"},
/* es */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_ES, "Usar <nombre> como directorio fichero sistema periférico #10"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_FR, "Utiliser le répertoire <nom> comme système de fichiers pour le lecteur #10"},
/* hu */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_HU, "<név> könyvtár használata az #10-es egység fájlrendszerének"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_IT, "Usa <nome> come directory per il file system della periferica #10"},
/* ko */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_NL, "Gebruik <naam> als directory voor bestandssysteemapparaat #10"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_PL, "Uzyj <nazwa> jako katalog urz±dzenia systemu plików #10"},
/* ru */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_RU, "Use <name> as directory for file system device #10"},
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_SV, "Ange <namn> som katalog för filsystemsbaserad enhet 10"},
/* tr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_TR, "<isim>'i aygýt #10'un dosya sistemi dizini olarak kullan"},
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11,    N_("Use <name> as directory for file system device #11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_DA, "Brug <navn> som katalog for filsystembaseret drev #11"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_DE, "Benutze <Name> für Verzeichnis beim Verzeichniszugriff für Gerät #11"},
/* es */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_ES, "Usar <nombre> como directorio fichero sistema periférico #10"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_FR, "Utiliser le répertoire <nom> comme système de fichiers pour le lecteur #11"},
/* hu */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_HU, "<név> könyvtár használata az #11-es egység fájlrendszerének"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_IT, "Usa <nome> come directory per il file system della periferica #11"},
/* ko */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_NL, "Gebruik <naam> als directory voor bestandssysteemapparaat #11"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_PL, "Uzyj <nazwa> jako katalog urz±dzenia systemu plików #11"},
/* ru */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_RU, "Use <name> as directory for file system device #11"},
/* sv */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_SV, "Ange <namn> som katalog för filsystemsbaserad enhet 11"},
/* tr */ {IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_TR, "<isim>'i aygýt #11'in dosya sistemi dizini olarak kullan"},
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00,    N_("Enable on-read support for P00 files on drive 8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00_DE, "On-read Unterstützung für P00 Dateien auf Laufwerk 8 aktivieren"},
/* es */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00_NL, "Ondersteuning voor het lezen van P00 bestanden voor drive 8 inschakelen"},
/* pl */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00_PL, "W³±cz obs³ugê plików P00 przy odczycie z napêdu 8"},
/* ru */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_FSDEVICE_8_CONVERT_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00,    N_("Disable on-read support for P00 files on drive 8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00_DE, "On-read Unterstützung für P00 Dateien auf Laufwerk 8 deaktivieren"},
/* es */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00_NL, "Ondersteuning voor het lezen van P00 bestanden voor drive 8 uitschakelen"},
/* pl */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00_PL, "Wy³±cz obs³ugê plików P00 przy odczycie z napêdu 8"},
/* ru */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_FSDEVICE_8_CONVERT_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00,    N_("Enable on-read support for P00 files on drive 9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00_DE, "On-read Unterstützung für P00 Dateien auf Laufwerk 9 aktivieren"},
/* es */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00_NL, "Ondersteuning voor het lezen van P00 bestanden voor drive 9 inschakelen"},
/* pl */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00_PL, "W³±cz obs³ugê plików P00 przy odczycie z napêdu 9"},
/* ru */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_FSDEVICE_9_CONVERT_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00,    N_("Disable on-read support for P00 files on drive 9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00_DE, "On-read Unterstützung für P00 Dateien auf Laufwerk 9 deaktivieren"},
/* es */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00_NL, "Ondersteuning voor het lezen van P00 bestanden voor drive 9 uitschakelen"},
/* pl */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00_PL, "Wy³±cz obs³ugê plików P00 przy odczycie z napêdu 9"},
/* ru */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_FSDEVICE_9_CONVERT_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00,    N_("Enable on-read support for P00 files on drive 10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00_DE, "On-read Unterstützung für P00 Dateien auf Laufwerk 10 aktivieren"},
/* es */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00_NL, "Ondersteuning voor het lezen van P00 bestanden voor drive 10 inschakelen"},
/* pl */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00_PL, "W³±cz obs³ugê plików P00 przy odczycie z napêdu 10"},
/* ru */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_FSDEVICE_10_CONVERT_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00,    N_("Disable on-read support for P00 files on drive 10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00_DE, "On-read Unterstützung für P00 Dateien auf Laufwerk 10 deaktivieren"},
/* es */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00_NL, "Ondersteuning voor het lezen van P00 bestanden voor drive 10 uitschakelen"},
/* pl */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00_PL, "Wy³±cz obs³ugê plików P00 przy odczycie z napêdu 10"},
/* ru */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_FSDEVICE_10_CONVERT_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00,    N_("Enable on-read support for P00 files on drive 11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00_DE, "On-read Unterstützung für P00 Dateien auf Laufwerk 11 aktivieren"},
/* es */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00_NL, "Ondersteuning voor het lezen van P00 bestanden voor drive 11 inschakelen"},
/* pl */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00_PL, "W³±cz obs³ugê plików P00 przy odczycie z napêdu 11"},
/* ru */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_FSDEVICE_11_CONVERT_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00,    N_("Disable on-read support for P00 files on drive 11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00_DE, "On-read Unterstützung für P00 Dateien auf Laufwerk 11 deaktivieren"},
/* es */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00_NL, "Ondersteuning voor het lezen van P00 bestanden voor drive 11 uitschakelen"},
/* pl */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00_PL, "Wy³±cz obs³ugê plików P00 przy odczycie z napêdu 11"},
/* ru */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_FSDEVICE_11_CONVERT_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00,    N_("Enable saving of P00 files on drive 8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00_DE, "Speichern von P00 Dateien auf Laufwerk 8 aktivieren"},
/* es */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00_NL, "Opslaan van P00 bestanden voor drive 8 inschakelen"},
/* pl */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00_PL, "W³±cz zapis plików P00 w napêdzie 8"},
/* ru */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_FSDEVICE_8_SAVE_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00,    N_("Disable saving of P00 files on drive 8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00_DE, "Speichern von P00 Dateien auf Laufwerk 8 deaktivieren"},
/* es */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00_NL, "Opslaan van P00 bestanden voor drive 8 uitschakelen"},
/* pl */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00_PL, "Wy³±cz zapis plików P00 w napêdzie 8"},
/* ru */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_FSDEVICE_8_SAVE_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00,    N_("Enable saving of P00 files on drive 9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00_DE, "Speichern von P00 Dateien auf Laufwerk 9 aktivieren"},
/* es */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00_NL, "Opslaan van P00 bestanden voor drive 9 inschakelen"},
/* pl */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00_PL, "W³±cz zapis plików P00 w napêdzie 9"},
/* ru */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_FSDEVICE_9_SAVE_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00,    N_("Disable saving of P00 files on drive 9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00_DE, "Speichern von P00 Dateien auf Laufwerk 9 deaktivieren"},
/* es */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00_NL, "Opslaan van P00 bestanden voor drive 9 uitschakelen"},
/* pl */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00_PL, "Wy³±cz zapis plików P00 w napêdzie 9"},
/* ru */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_FSDEVICE_9_SAVE_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00,    N_("Enable saving of P00 files on drive 10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00_DE, "Speichern von P00 Dateien auf Laufwerk 10 aktivieren"},
/* es */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00_NL, "Opslaan van P00 bestanden voor drive 10 inschakelen"},
/* pl */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00_PL, "W³±cz zapis plików P00 w napêdzie 10"},
/* ru */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_FSDEVICE_10_SAVE_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00,    N_("Disable saving of P00 files on drive 10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00_DE, "Speichern von P00 Dateien auf Laufwerk 10 deaktivieren"},
/* es */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00_NL, "Opslaan van P00 bestanden voor drive 10 uitschakelen"},
/* pl */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00_PL, "Wy³±cz zapis plików P00 w napêdzie 10"},
/* ru */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_FSDEVICE_10_SAVE_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00,    N_("Enable saving of P00 files on drive 11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00_DE, "Speichern von P00 Dateien auf Laufwerk 11 aktivieren"},
/* es */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00_NL, "Opslaan van P00 bestanden voor drive 11 inschakelen"},
/* pl */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00_PL, "W³±cz zapis plików P00 w napêdzie 11"},
/* ru */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_FSDEVICE_11_SAVE_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00,    N_("Disable saving of P00 files on drive 11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00_DE, "Speichern von P00 Dateien auf Laufwerk 11 deaktivieren"},
/* es */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00_NL, "Opslaan van P00 bestanden voor drive 11 uitschakelen"},
/* pl */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00_PL, "Wy³±cz zapis plików P00 w napêdzie 11"},
/* ru */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_FSDEVICE_11_SAVE_P00_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM,    N_("Hide CBM files (only show P00 files) for drive 8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM_DE, "Laufwerk 8 CBM Dateien ausblenden (nur P00 Dateien zeigen)"},
/* es */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM_NL, "Verberg CBM bestanden (toon alleen P00 bestanden) voor drive 8"},
/* pl */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM_PL, "Ukryj pliki CBM (pokazuj tylko pliki P00) dla napêdu 8"},
/* ru */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_FSDEVICE_8_HIDE_CBM_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM,    N_("Do not hide CBM files (show all files) for drive 8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM_DE, "Laufwerk 8 alle Dateien zeigen (CBM Dateien nicht ausblenden)"},
/* es */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM_NL, "Verberg CBM bestanden niet (toon alle bestanden) voor drive 8"},
/* pl */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM_PL, "Nie ukrywaj plików CBM (pokazuj wszystkie pliki) dla napêdu 8"},
/* ru */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_FSDEVICE_8_HIDE_CBM_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM,    N_("Hide CBM files (only show P00 files) for drive 9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM_DE, "Laufwerk 9 CBM Dateien ausblenden (nur P00 Dateien zeigen)"},
/* es */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM_NL, "Verberg CBM bestanden (toon alleen P00 bestanden) voor drive 9"},
/* pl */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM_PL, "Ukryj pliki CBM (pokazuj tylko pliki P00) dla napêdu 9"},
/* ru */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_FSDEVICE_9_HIDE_CBM_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM,    N_("Do not hide CBM files (show all files) for drive 9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM_DE, "Laufwerk 9 alle Dateien zeigen (CBM Dateien nicht ausblenden)"},
/* es */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM_NL, "Verberg CBM bestanden niet (toon alle bestanden) voor drive 9"},
/* pl */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM_PL, "Nie ukrywaj plików CBM (pokazuj wszystkie pliki) dla napêdu 9"},
/* ru */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_FSDEVICE_9_HIDE_CBM_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM,    N_("Hide CBM files (only show P00 files) for drive 10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM_DE, "Laufwerk 10 CBM Dateien ausblenden (nur P00 Dateien zeigen)"},
/* es */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM_NL, "Verberg CBM bestanden (toon alleen P00 bestanden) voor drive 10"},
/* pl */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM_PL, "Ukryj pliki CBM (pokazuj tylko pliki P00) dla napêdu 10"},
/* ru */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_FSDEVICE_10_HIDE_CBM_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM,    N_("Do not hide CBM files (show all files) for drive 10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM_DE, "Laufwerk 10 alle Dateien zeigen (CBM Dateien nicht ausblenden)"},
/* es */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM_NL, "Verberg CBM bestanden niet (toon alle bestanden) voor drive 10"},
/* pl */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM_PL, "Nie ukrywaj plików CBM (pokazuj wszystkie pliki) dla napêdu 10"},
/* ru */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_FSDEVICE_10_HIDE_CBM_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM,    N_("Hide CBM files (only show P00 files) for drive 11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM_DE, "Laufwerk 11 CBM Dateien ausblenden (nur P00 Dateien zeigen)"},
/* es */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM_NL, "Verberg CBM bestanden (toon alleen P00 bestanden) voor drive 11"},
/* pl */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM_PL, "Ukryj pliki CBM (pokazuj tylko pliki P00) dla napêdu 11"},
/* ru */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_FSDEVICE_11_HIDE_CBM_TR, ""},  /* fuzzy */
#endif

/* fsdevice/fsdevice-cmdline-options.c */
/* en */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM,    N_("Do not hide CBM files (show all files) for drive 11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM_DE, "Laufwerk 11 alle Dateien zeigen (CBM Dateien nicht ausblenden)"},
/* es */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM_NL, "Verberg CBM bestanden niet (toon alle bestanden) voor drive 11"},
/* pl */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM_PL, "Nie ukrywaj plików CBM (pokazuj wszystkie pliki) dla napêdu 11"},
/* ru */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_FSDEVICE_11_HIDE_CBM_TR, ""},  /* fuzzy */
#endif

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDCLS_SET_AUDIO_STREAM_BITRATE,    N_("Set bitrate for audio stream in media file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_AUDIO_STREAM_BITRATE_DA, "Angiv bitrate for lydstrøm i mediefil"},
/* de */ {IDCLS_SET_AUDIO_STREAM_BITRATE_DE, "Bitrate für Audio Stream für Mediendatei setzen"},
/* es */ {IDCLS_SET_AUDIO_STREAM_BITRATE_ES, "Seleccionar velocidad muestreo para flujo audio en fichero media"},
/* fr */ {IDCLS_SET_AUDIO_STREAM_BITRATE_FR, "Régler le bitrate audio du fichier média"},
/* hu */ {IDCLS_SET_AUDIO_STREAM_BITRATE_HU, "Adja meg az audió bitrátát a média fájlhoz"},
/* it */ {IDCLS_SET_AUDIO_STREAM_BITRATE_IT, "Imposta il bitrate del flusso audio nel file multimediale"},
/* ko */ {IDCLS_SET_AUDIO_STREAM_BITRATE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_AUDIO_STREAM_BITRATE_NL, "Zet de bitrate voor het audiogedeelte van het mediabestand"},
/* pl */ {IDCLS_SET_AUDIO_STREAM_BITRATE_PL, "Okre¶l szybko¶æ transmisji strumienia audio w pliku multimediów"},
/* ru */ {IDCLS_SET_AUDIO_STREAM_BITRATE_RU, "Set bitrate for audio stream in media file"},
/* sv */ {IDCLS_SET_AUDIO_STREAM_BITRATE_SV, "Ange bithastighet för ljudström i mediafil"},
/* tr */ {IDCLS_SET_AUDIO_STREAM_BITRATE_TR, "Ses akýþý için ortam dosyasýndaki bithýzý'ný ayarla"},
#endif

/* gfxoutputdrv/ffmpegdrv.c */
/* en */ {IDCLS_SET_VIDEO_STREAM_BITRATE,    N_("Set bitrate for video stream in media file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_VIDEO_STREAM_BITRATE_DA, "Angiv bitrate for videostrøm i mediefil"},
/* de */ {IDCLS_SET_VIDEO_STREAM_BITRATE_DE, "Bitrate für Video Stream für Mediendatei setzen"},
/* es */ {IDCLS_SET_VIDEO_STREAM_BITRATE_ES, "Seleccionar velocidad muestreo para flujo vídeo en fichero media"},
/* fr */ {IDCLS_SET_VIDEO_STREAM_BITRATE_FR, "Régler le bitrate vidéo du fichier média"},
/* hu */ {IDCLS_SET_VIDEO_STREAM_BITRATE_HU, "Adja meg az videó bitrátát a média fájlhoz"},
/* it */ {IDCLS_SET_VIDEO_STREAM_BITRATE_IT, "Imposta il bitrate per il flusso video nel file multimediale"},
/* ko */ {IDCLS_SET_VIDEO_STREAM_BITRATE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_VIDEO_STREAM_BITRATE_NL, "Zet de bitrate voor het videogedeelte van het mediabestand"},
/* pl */ {IDCLS_SET_VIDEO_STREAM_BITRATE_PL, "Okre¶l szybko¶æ transmisji strumienia wideo w pliku multimediów"},
/* ru */ {IDCLS_SET_VIDEO_STREAM_BITRATE_RU, "Set bitrate for video stream in media file"},
/* sv */ {IDCLS_SET_VIDEO_STREAM_BITRATE_SV, "Ange bithastighet för videoström i mediafil"},
/* tr */ {IDCLS_SET_VIDEO_STREAM_BITRATE_TR, "Görüntü akýþý için ortam dosyasýndaki bithýzý'ný ayarla"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_PET_MODEL,    N_("Specify PET model to emulate. (2001/3008/3016/3032/3032B/4016/4032/4032B/8032/8096/8296/SuperPET)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PET_MODEL_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_PET_MODEL_DE, "PET model für Emulation wählen. (2001/3008/3016/3032/3032B/4016/4032/4032B/8032/8096/8296/SuperPET)"},
/* es */ {IDCLS_SPECIFY_PET_MODEL_ES, "Especificar modelo PET a emular. (2001/3008/3016/3032/3032B/4016/4032/4032B/8032/8096/8296/SuperPET)"},
/* fr */ {IDCLS_SPECIFY_PET_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_PET_MODEL_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PET_MODEL_IT, "Specifica il modello PET da emulare. (2001/3008/3016/3032/3032B/4016/4032/4032B/8032/8096/8296/SuperPET)"},
/* ko */ {IDCLS_SPECIFY_PET_MODEL_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PET_MODEL_NL, "Geef PET model om te emuleren. (2001/3008/3016/3032/3032B/4016/4032/4032B/8032/8096/8296/SuperPET)"},
/* pl */ {IDCLS_SPECIFY_PET_MODEL_PL, "Wybierz emulowany model PET. (2001/3008/3016/3032/3032B/4016/4032/4032B/8032/8096/8296/SuperPET)"},
/* ru */ {IDCLS_SPECIFY_PET_MODEL_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PET_MODEL_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_PET_MODEL_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_EDITOR_ROM_NAME,    N_("Specify name of Editor ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_DA, "Angiv navn på Editor-ROM-image"},
/* de */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_DE, "Name für Editor ROM Imagedatei wählen"},
/* es */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_ES, "Especificar nombre imagen editor ROM"},
/* fr */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_FR, "Spécifier le nom de l’image ROM Editor"},
/* hu */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_HU, "Adja meg a szerkesztõ ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_IT, "Specifica il nome immagine della Editor ROM"},
/* ko */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_NL, "Geef naam van het Editor ROM bestand"},
/* pl */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM Editor"},
/* ru */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_RU, "Specify name of Editor ROM image"},
/* sv */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_SV, "Ange namn på Editor-ROM-avbildning"},
/* tr */ {IDCLS_SPECIFY_EDITOR_ROM_NAME_TR, "Editor ROM imajýnýn ismini belirt"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME,    N_("Specify 4K extension ROM name at $9***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_DA, "Angiv navn på 4K-udviddelses-ROM på $9***"},
/* de */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_DE, "4K Erweiterungs ROM Dateiname bei $9*** wählen"},
/* es */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_ES, "Especificar nombre extensión ROM 4KB en $9***"},
/* fr */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_FR, "Spécifier le nom de l'extension ROM 4K à $9***"},
/* hu */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_HU, "Adja meg a $9*** címû 4K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_IT, "Specifica il nome della ROM di estensione di 4k a $9***"},
/* ko */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_NL, "Geef naam van het 4K uitbreiding ROM bestand op adres $9***"},
/* pl */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_PL, "Okre¶l nazwê rozszerzenia 4K ROM w $9***"},
/* ru */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_RU, "Specify 4K extension ROM name at $9***"},
/* sv */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_SV, "Ange namn på 4K-utöknings-ROM på $9***"},
/* tr */ {IDCLS_SPECIFY_4K_ROM_9XXX_NAME_TR, "$9*** adresindeki 4K uzantýsý ROM ismini belirt"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME,    N_("Specify 4K extension ROM name at $A***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_DA, "Angiv navn på 4K-udviddelses-ROM på $A***"},
/* de */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_DE, "4K Erweiterungs ROM Dateiname bei $A*** wählen"},
/* es */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_ES, "Especificar nombre extensión ROM 4KB en $A***"},
/* fr */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_FR, "Spécifier le nom de l'extension ROM 4K à $A***"},
/* hu */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_HU, "Adja meg a $A*** címû 4K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_IT, "Specifica il nome della ROM di estensione di 4k a $A***"},
/* ko */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_NL, "Geef naam van het 4K uitbreiding ROM bestand op adres $A***"},
/* pl */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_PL, "Okre¶l nazwê rozszerzenia 4K ROM w $A***"},
/* ru */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_RU, "Specify 4K extension ROM name at $A***"},
/* sv */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_SV, "Ange namn på 4K-utöknings-ROM på $A***"},
/* tr */ {IDCLS_SPECIFY_4K_ROM_AXXX_NAME_TR, "$A*** adresindeki 4K uzantýsý ROM ismini belirt"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME,    N_("Specify 4K extension ROM name at $B***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_DA, "Angiv navn på 4K-udviddelses-ROM på $B***"},
/* de */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_DE, "4K Erweiterungs ROM Dateiname bei $B*** wählen"},
/* es */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_ES, "Especificar nombre extensión ROM 4KB en $B***"},
/* fr */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_FR, "Spécifier le nom de l'extension ROM 4K à $B***"},
/* hu */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_HU, "Adja meg a $B*** címû 4K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_IT, "Specifica il nome della ROM di estensione di 4k a $B***"},
/* ko */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_NL, "Geef naam van het 4K uitbreiding ROM bestand op adres $B***"},
/* pl */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_PL, "Okre¶l nazwê rozszerzenia 4K ROM w $B***"},
/* ru */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_RU, "Specify 4K extension ROM name at $B***"},
/* sv */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_SV, "Ange namn på 4K-utöknings-ROM på $B***"},
/* tr */ {IDCLS_SPECIFY_4K_ROM_BXXX_NAME_TR, "$B*** adresindeki 4K uzantýsý ROM ismini belirt"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000,    N_("Enable PET8296 4K RAM mapping at $9***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_DA, "Aktivér PET8296 4K-RAM-mapping på $9***"},
/* de */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_DE, "PET8296 4K RAM Zuordnung bei $9*** aktivieren"},
/* es */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_ES, "Permitir mapeado 4KB de RAM PET8296 en $9***"},
/* fr */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_FR, "Activer l'extension PET8296 4K à $9***"},
/* hu */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_HU, "PET8296 4K RAM engedélyezése a $9*** címen"},
/* it */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_IT, "Attiva la mappatura della RAM di 4K del PET8296 a $9***"},
/* ko */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_NL, "PET8296 4K RAM op adres $9*** inschakelen"},
/* pl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_PL, "W³±cz mapowanie 4K RAM PET8296 w $9***"},
/* ru */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_RU, "Enable PET8296 4K RAM mapping at $9***"},
/* sv */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_SV, "Aktivera PET8296 4K-RAM-mappning på $9***"},
/* tr */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_TR, "$9*** adresindeki PET8296 4K RAM eþleþtirmesini aktif et"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000,    N_("Disable PET8296 4K RAM mapping at $9***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_DA, "Deaktivér PET8296 4K-RAM-mapping på $9***"},
/* de */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_DE, "PET8296 4K RAM Zuordnung bei $9*** deaktivieren"},
/* es */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_ES, "Deshabilitar mapeado 4KB de RAM PET8296 en $9***"},
/* fr */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_FR, "Désactiver l'extension PET8296 4K à $9***"},
/* hu */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_HU, "PET8296 4K RAM tiltása a $9*** címen"},
/* it */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_IT, "Disattiva la mappatura della RAM di 4K del PET8296 a $9***"},
/* ko */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_NL, "PET8296 4K RAM op adres $9*** uitschakelen"},
/* pl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_PL, "Wy³±cz mapowanie 4K RAM PET8296 w $9***"},
/* ru */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_RU, "Disable PET8296 4K RAM mapping at $9***"},
/* sv */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_SV, "Inaktivera PET8296 4K-RAM-mappning på $9***"},
/* tr */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_TR, "$9*** adresindeki PET8296 4K RAM eþleþtirmesini pasifleþtir"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000,    N_("Enable PET8296 4K RAM mapping at $A***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_DA, "Aktivér PET8296 4K-RAM-mapping på $A***"},
/* de */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_DE, "PET8296 4K RAM Zuordnung bei $A*** aktivieren"},
/* es */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_ES, "Permitir mapeado 4KB de RAM PET8296 en $A***"},
/* fr */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_FR, "Activer l'extension PET8296 4K à $A***"},
/* hu */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_HU, "PET8296 4K RAM engedélyezése a $A*** címen"},
/* it */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_IT, "Attiva la mappatura della RAM di 4K del PET8296 a $A***"},
/* ko */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_NL, "PET8296 4K RAM op adres $A*** inschakelen"},
/* pl */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_PL, "W³±cz mapowanie 4K RAM PET8296 w $A***"},
/* ru */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_RU, "Enable PET8296 4K RAM mapping at $A***"},
/* sv */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_SV, "Aktivera PET8296 4K-RAM-mappning på $A***"},
/* tr */ {IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_TR, "$A*** adresindeki PET8296 4K RAM eþleþtirmesini aktif et"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000,    N_("Disable PET8296 4K RAM mapping at $A***")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_DA, "Deaktivér PET8296 4K-RAM-mapping på $A***"},
/* de */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_DE, "PET8296 4K RAM Zuordnung bei $A*** deaktivieren"},
/* es */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_ES, "Deshabilitar mapeado 4KB de RAM PET8296 en $A***"},
/* fr */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_FR, "Désactiver l'extension PET8296 4K à $A***"},
/* hu */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_HU, "PET8296 4K RAM tiltása a $A*** címen"},
/* it */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_IT, "Disattiva la mappatura della RAM di 4K del PET8296 a $A***"},
/* ko */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_NL, "PET8296 4K RAM op adres $A*** uitschakelen"},
/* pl */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_PL, "Wy³±cz mapowanie 4K RAM PET8296 w $A***"},
/* ru */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_RU, "Disable PET8296 4K RAM mapping at $A***"},
/* sv */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_SV, "Inaktivera PET8296 4K-RAM-mappning på $A***"},
/* tr */ {IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_TR, "$A*** adresindeki PET8296 4K RAM eþleþtirmesini pasifleþtir"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SUPERPET_IO,    N_("Enable SuperPET I/O")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SUPERPET_IO_DA, "Aktivér SuperPET-I/O"},
/* de */ {IDCLS_ENABLE_SUPERPET_IO_DE, "SuperPET I/O aktivieren"},
/* es */ {IDCLS_ENABLE_SUPERPET_IO_ES, "Permitir entrada/salida SuperPET"},
/* fr */ {IDCLS_ENABLE_SUPERPET_IO_FR, "Activer les E/S SuperPET"},
/* hu */ {IDCLS_ENABLE_SUPERPET_IO_HU, "SuperPET I/O engedélyezése"},
/* it */ {IDCLS_ENABLE_SUPERPET_IO_IT, "Attiva l'I/O del SuperPET"},
/* ko */ {IDCLS_ENABLE_SUPERPET_IO_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SUPERPET_IO_NL, "SuperPET I/O inschakelen"},
/* pl */ {IDCLS_ENABLE_SUPERPET_IO_PL, "W³±cz we/wy SuperPET"},
/* ru */ {IDCLS_ENABLE_SUPERPET_IO_RU, "Enable SuperPET I/O"},
/* sv */ {IDCLS_ENABLE_SUPERPET_IO_SV, "Aktivera SuperPET-I/O"},
/* tr */ {IDCLS_ENABLE_SUPERPET_IO_TR, "SuperPET G/Ç'ý aktif et"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SUPERPET_IO,    N_("Disable SuperPET I/O")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SUPERPET_IO_DA, "Deaktivér SuperPET-I/O"},
/* de */ {IDCLS_DISABLE_SUPERPET_IO_DE, "SuperPET I/O deaktivieren"},
/* es */ {IDCLS_DISABLE_SUPERPET_IO_ES, "Deshabilitar entrada/salida SuperPET"},
/* fr */ {IDCLS_DISABLE_SUPERPET_IO_FR, "Désactiver les E/S SuperPET"},
/* hu */ {IDCLS_DISABLE_SUPERPET_IO_HU, "SuperPET I/O tiltása"},
/* it */ {IDCLS_DISABLE_SUPERPET_IO_IT, "Disattiva l'I/O del SuperPET"},
/* ko */ {IDCLS_DISABLE_SUPERPET_IO_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SUPERPET_IO_NL, "SuperPET I/O uitschakelen"},
/* pl */ {IDCLS_DISABLE_SUPERPET_IO_PL, "Wy³±cz we/wy SuperPET"},
/* ru */ {IDCLS_DISABLE_SUPERPET_IO_RU, "Disable SuperPET I/O"},
/* sv */ {IDCLS_DISABLE_SUPERPET_IO_SV, "Inaktivera SuperPET-I/O"},
/* tr */ {IDCLS_DISABLE_SUPERPET_IO_TR, "SuperPET G/Ç'ý pasifleþtir"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES,    N_("Enable ROM 1 Kernal patches")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_DA, "Aktivér ROM 1 kerneændringer"},
/* de */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_DE, "ROM 1 Kernal patches aktivieren"},
/* es */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_ES, "Permitir parches ROM 1 Kernal"},
/* fr */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_FR, "Activer les patches ROM 1 Kernal"},
/* hu */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_HU, "ROM 1 Kernal foltozás engedélyezése"},
/* it */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_IT, "Attiva le patch del Kernal della ROM 1"},
/* ko */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_NL, "ROM 1 Kernalverbeteringen inschakelen"},
/* pl */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_PL, "W³±cz ³atki Kernala ROM 1"},
/* ru */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_RU, "Enable ROM 1 Kernal patches"},
/* sv */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_SV, "Aktivera ROM 1-Kernalpatchar"},
/* tr */ {IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_TR, "ROM 1 Kernal yamalarýný aktif et"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES,    N_("Disable ROM 1 Kernal patches")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_DA, "Deaktivér ROM 1 kerneændringer"},
/* de */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_DE, "ROM 1 Kernal patches deaktivieren"},
/* es */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_ES, "Deshabilitar parches ROM 1 Kernal"},
/* fr */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_FR, "Désactiver les patches ROM 1 Kernal"},
/* hu */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_HU, "ROM 1 Kernal foltozás tiltása"},
/* it */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_IT, "Disattiva le patch del Kernal della ROM 1"},
/* ko */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_NL, "ROM 1 Kernalverbeteringen uitschakelen"},
/* pl */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_PL, "Wy³±cz ³atki Kernala ROM 1"},
/* ru */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_RU, "Disable ROM 1 Kernal patches"},
/* sv */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_SV, "Inaktivera ROM 1-Kernalpatchar"},
/* tr */ {IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_TR, "ROM 1 Kernal yamalarýný pasifleþtir"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET,    N_("Switch upper/lower case charset")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_DA, "Skift mellem store og små bogstaver"},
/* de */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_DE, "Wechsel Groß-/Kleinschreibung Zeichensatz"},
/* es */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_ES, "Intercambiar mayúsculas/minúscula"},
/* fr */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_FR, "Inverser le charset majuscule et minuscule"},
/* hu */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_HU, "Váltás nagy-/kisbetûs karakterkészletre"},
/* it */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_IT, "Scambia il set di caratteri maiuscolo/minuscolo"},
/* ko */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_NL, "Schakel tussen grote/kleine letters"},
/* pl */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_PL, "Prze³±cz zestaw wielkich/ma³ych znaków"},
/* ru */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_RU, "²ÚÛîçØâì ÒÕàåÝØÙ/ÝØÖÝØÙ àÕÓØáâà"},
/* sv */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_SV, "Växla små/stora bokstäver i teckengeneratorn"},
/* tr */ {IDCLS_SWITCH_UPPER_LOWER_CHARSET_TR, "Büyük/küçük harf karakter seti deðiþtir"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET,    N_("Do not switch upper/lower case charset")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_DA, "Skift ikke mellem store og små bogstaver"},
/* de */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_DE, "Kein Wechsel Groß-/Kleinschreibung Zeichensatz"},
/* es */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_ES, "No intercambiar mayúsculas/minúsculas"},
/* fr */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_FR, "Ne pas inverser le charset majuscule et minuscule"},
/* hu */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_HU, "Ne váltson a nagy-/kisbetûs karakterkészletre"},
/* it */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_IT, "Non scambiare il set di caratteri maiuscolo/minuscolo"},
/* ko */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_NL, "Schakel niet tussen grote/kleine letters"},
/* pl */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_PL, "Nie prze³±czaj zestawu wielkich/ma³ych znaków"},
/* ru */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_RU, "½Õ ÒÚÛîçÐâì ÒÕàåÝØÙ/ÝØÖÝØÙ àÕÓØáâà"},
/* sv */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_SV, "Växla inte små/stora bokstäver i teckengeneratorn"},
/* tr */ {IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_TR, "Büyük/küçük harf karakter seti deðiþtirme"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_EOI_BLANKS_SCREEN,    N_("EOI blanks screen")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_EOI_BLANKS_SCREEN_DA, "EOI sletter skærmen"},
/* de */ {IDCLS_EOI_BLANKS_SCREEN_DE, "EOI löscht Bildschirm"},
/* es */ {IDCLS_EOI_BLANKS_SCREEN_ES, "EOI pantalla en blanco"},
/* fr */ {IDCLS_EOI_BLANKS_SCREEN_FR, "EOI vide l'écran"},
/* hu */ {IDCLS_EOI_BLANKS_SCREEN_HU, "EOI törli a képernyõt"},
/* it */ {IDCLS_EOI_BLANKS_SCREEN_IT, "EOI oscura lo schermo"},
/* ko */ {IDCLS_EOI_BLANKS_SCREEN_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_EOI_BLANKS_SCREEN_NL, "EOI maakt het scherm leeg"},
/* pl */ {IDCLS_EOI_BLANKS_SCREEN_PL, "Koniec przerwania czy¶ci ekran"},
/* ru */ {IDCLS_EOI_BLANKS_SCREEN_RU, "EOI blanks screen"},
/* sv */ {IDCLS_EOI_BLANKS_SCREEN_SV, "EOI tömmer skärmen"},
/* tr */ {IDCLS_EOI_BLANKS_SCREEN_TR, "EOI ekraný boþaltýr"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN,    N_("EOI does not blank screen")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_DA, "EOI sletter ikke skærmen"},
/* de */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_DE, "EIO löscht Bildschirm nicht"},
/* es */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_ES, "EOI pantalla no en blanco"},
/* fr */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_FR, "EOI ne vide pas l'écran"},
/* hu */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_HU, "EOI nem törli a képernyõt"},
/* it */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_IT, "EOI non oscura lo schermo"},
/* ko */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_NL, "EOI maakt het scherm niet leeg"},
/* pl */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_PL, "Koniec przerwania nie czy¶ci ekranu"},
/* ru */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_RU, "EOI does not blank screen"},
/* sv */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_SV, "EOI tömmer inte skärmen"},
/* tr */ {IDCLS_EOI_DOES_NOT_BLANK_SCREEN_TR, "EOI ekraný boþaltmaz"},
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SET_CPU_SWITCH_TO_6502,    N_("Set SuperPET CPU switch to '6502'")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_CPU_SWITCH_TO_6502_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_CPU_SWITCH_TO_6502_DE, "Setze SuperPET CPU Schalter auf '6502'"},
/* es */ {IDCLS_SET_CPU_SWITCH_TO_6502_ES, "Ajustar interruptor SuperPET CPU"},
/* fr */ {IDCLS_SET_CPU_SWITCH_TO_6502_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_CPU_SWITCH_TO_6502_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_CPU_SWITCH_TO_6502_IT, "Imposta pulsante CPU SuperPET a '6502'"},
/* ko */ {IDCLS_SET_CPU_SWITCH_TO_6502_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_CPU_SWITCH_TO_6502_NL, "Zet SuperPET CPU schakelaar naar '6502'"},
/* pl */ {IDCLS_SET_CPU_SWITCH_TO_6502_PL, "Ustaw prze³±cznik CPU SuperPET na '6502\""},
/* ru */ {IDCLS_SET_CPU_SWITCH_TO_6502_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_CPU_SWITCH_TO_6502_SV, "Sätt SuperPET-processoromkopplaren till \"6502\""},
/* tr */ {IDCLS_SET_CPU_SWITCH_TO_6502_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SET_CPU_SWITCH_TO_6809,    N_("Set SuperPET CPU switch to '6809'")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_CPU_SWITCH_TO_6809_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_CPU_SWITCH_TO_6809_DE, "Setze SuperPET CPU Schalter auf '6809'"},
/* es */ {IDCLS_SET_CPU_SWITCH_TO_6809_ES, "Ajustar interruptor SuperPET CPU para '6809'"},
/* fr */ {IDCLS_SET_CPU_SWITCH_TO_6809_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_CPU_SWITCH_TO_6809_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_CPU_SWITCH_TO_6809_IT, "Imposta pulsante CPU SuperPET a '6809'"},
/* ko */ {IDCLS_SET_CPU_SWITCH_TO_6809_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_CPU_SWITCH_TO_6809_NL, "Zet SuperPET CPU schakelaar naar '6809'"},
/* pl */ {IDCLS_SET_CPU_SWITCH_TO_6809_PL, "Ustaw prze³±cznik CPU SuperPET na '6809\""},
/* ru */ {IDCLS_SET_CPU_SWITCH_TO_6809_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_CPU_SWITCH_TO_6809_SV, "Sätt SuperPET-processoromkopplaren till \"6809\""},
/* tr */ {IDCLS_SET_CPU_SWITCH_TO_6809_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SET_CPU_SWITCH_TO_PROG,    N_("Set SuperPET CPU switch to 'Prog'")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_CPU_SWITCH_TO_PROG_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_CPU_SWITCH_TO_PROG_DE, "Setze SuperPET CPU Schalter auf 'Prog'"},
/* es */ {IDCLS_SET_CPU_SWITCH_TO_PROG_ES, "Ajustar interruptor SuperPET CPU para 'Prog'"},
/* fr */ {IDCLS_SET_CPU_SWITCH_TO_PROG_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_CPU_SWITCH_TO_PROG_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_CPU_SWITCH_TO_PROG_IT, "Imposta pulsante CPU SuperPET a 'Prog'"},
/* ko */ {IDCLS_SET_CPU_SWITCH_TO_PROG_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_CPU_SWITCH_TO_PROG_NL, "Zet SuperPET CPU schakelaar naar 'Prog'"},
/* pl */ {IDCLS_SET_CPU_SWITCH_TO_PROG_PL, "Ustaw prze³±cznik CPU SuperPET na 'Prog\""},
/* ru */ {IDCLS_SET_CPU_SWITCH_TO_PROG_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_CPU_SWITCH_TO_PROG_SV, "Sätt SuperPET-processoromkopplaren till \"Prog\""},
/* tr */ {IDCLS_SET_CPU_SWITCH_TO_PROG_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_6809_ROM_AT_A000,    N_("Specify 4K to 24K ROM file name at $A000 for 6809")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_6809_ROM_AT_A000_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_6809_ROM_AT_A000_DE, "4K-24K Erweiterungs ROM Dateiname für $A000 für 6809 wählen"},
/* es */ {IDCLS_SPECIFY_6809_ROM_AT_A000_ES, "Especificar nombre ROM 4K a 24K  en $A000 para 6809"},
/* fr */ {IDCLS_SPECIFY_6809_ROM_AT_A000_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_6809_ROM_AT_A000_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_6809_ROM_AT_A000_IT, "Specifica il nome della ROM da 4K a 24K a $A000 per il 6809"},
/* ko */ {IDCLS_SPECIFY_6809_ROM_AT_A000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_6809_ROM_AT_A000_NL, "Geef de naam van het bestand voor de 4K tot 24K ROM op $A000 voor de 6809"},
/* pl */ {IDCLS_SPECIFY_6809_ROM_AT_A000_PL, "Okre¶l nazwê pliku ROM-u 4K do 24K w $A000 dla 6809"},
/* ru */ {IDCLS_SPECIFY_6809_ROM_AT_A000_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_6809_ROM_AT_A000_SV, "Ange namn för 4-24K-utöknings-ROM på $A000 för 6809"},
/* tr */ {IDCLS_SPECIFY_6809_ROM_AT_A000_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_6809_ROM_AT_B000,    N_("Specify 4K to 20K ROM file name at $B000 for 6809")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_6809_ROM_AT_B000_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_6809_ROM_AT_B000_DE, "4K-20K Erweiterungs ROM Dateiname für $B000 für 6809 wählen"},
/* es */ {IDCLS_SPECIFY_6809_ROM_AT_B000_ES, "Especificar nombre ROM 4K a 20K  en $B000 para 6809"},
/* fr */ {IDCLS_SPECIFY_6809_ROM_AT_B000_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_6809_ROM_AT_B000_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_6809_ROM_AT_B000_IT, "Specifica il nome della ROM da 4K a 20K a $B000 per il 6809"},
/* ko */ {IDCLS_SPECIFY_6809_ROM_AT_B000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_6809_ROM_AT_B000_NL, "Geef de naam van het bestand voor de 4K tot 20K ROM op $B000 voor de 6809"},
/* pl */ {IDCLS_SPECIFY_6809_ROM_AT_B000_PL, "Okre¶l nazwê pliku ROM-u 4K do 20K w $B000 dla 6809"},
/* ru */ {IDCLS_SPECIFY_6809_ROM_AT_B000_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_6809_ROM_AT_B000_SV, "Ange namn för 4-20K-utöknings-ROM på $B000 för 6809"},
/* tr */ {IDCLS_SPECIFY_6809_ROM_AT_B000_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_6809_ROM_AT_C000,    N_("Specify 4K to 16K ROM file name at $C000 for 6809")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_6809_ROM_AT_C000_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_6809_ROM_AT_C000_DE, "4K-16K Erweiterungs ROM Dateiname für $C000 für 6809 wählen"},
/* es */ {IDCLS_SPECIFY_6809_ROM_AT_C000_ES, "Especificar nombre ROM 4K a 16K  en $C000 para 6809"},
/* fr */ {IDCLS_SPECIFY_6809_ROM_AT_C000_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_6809_ROM_AT_C000_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_6809_ROM_AT_C000_IT, "Specifica il nome della ROM da 4K a 16K a $C000 per il 6809"},
/* ko */ {IDCLS_SPECIFY_6809_ROM_AT_C000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_6809_ROM_AT_C000_NL, "Geef de naam van het bestand voor de 4K tot 16K ROM op $C000 voor de 6809"},
/* pl */ {IDCLS_SPECIFY_6809_ROM_AT_C000_PL, "Okre¶l nazwê pliku ROM-u 4K do 16K w $C000 dla 6809"},
/* ru */ {IDCLS_SPECIFY_6809_ROM_AT_C000_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_6809_ROM_AT_C000_SV, "Ange namn för 4-16K-utöknings-ROM på $C000 för 6809"},
/* tr */ {IDCLS_SPECIFY_6809_ROM_AT_C000_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_6809_ROM_AT_D000,    N_("Specify 4K to 12K ROM file name at $D000 for 6809")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_6809_ROM_AT_D000_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_6809_ROM_AT_D000_DE, "4K-12K Erweiterungs ROM Dateiname für $D000 für 6809 wählen"},
/* es */ {IDCLS_SPECIFY_6809_ROM_AT_D000_ES, "Especificar nombre ROM 4K a 12K  en $D000 para 6809"},
/* fr */ {IDCLS_SPECIFY_6809_ROM_AT_D000_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_6809_ROM_AT_D000_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_6809_ROM_AT_D000_IT, "Specifica il nome della ROM da 4K a 12K a $D000 per il 6809"},
/* ko */ {IDCLS_SPECIFY_6809_ROM_AT_D000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_6809_ROM_AT_D000_NL, "Geef de naam van het bestand voor de 4K tot 12K ROM op $D000 voor de 6809"},
/* pl */ {IDCLS_SPECIFY_6809_ROM_AT_D000_PL, "Okre¶l nazwê pliku ROM-u 4K do 12K w $D000 dla 6809"},
/* ru */ {IDCLS_SPECIFY_6809_ROM_AT_D000_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_6809_ROM_AT_D000_SV, "Ange namn för 4-12K-utöknings-ROM på $D000 för 6809"},
/* tr */ {IDCLS_SPECIFY_6809_ROM_AT_D000_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_6809_ROM_AT_E000,    N_("Specify 2K or 8K ROM file name at $E000 for 6809")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_6809_ROM_AT_E000_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_6809_ROM_AT_E000_DE, "4K-8K Erweiterungs ROM Dateiname für $E000 für 6809 wählen"},
/* es */ {IDCLS_SPECIFY_6809_ROM_AT_E000_ES, "Especificar nombre ROM 4K a 8K  en $E000 para 6809"},
/* fr */ {IDCLS_SPECIFY_6809_ROM_AT_E000_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_6809_ROM_AT_E000_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_6809_ROM_AT_E000_IT, "Specifica il nome della ROM da 2K a 8K a $E000 per il 6809"},
/* ko */ {IDCLS_SPECIFY_6809_ROM_AT_E000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_6809_ROM_AT_E000_NL, "Geef de naam van het bestand voor de 2K of 8K ROM op $E000 voor de 6809"},
/* pl */ {IDCLS_SPECIFY_6809_ROM_AT_E000_PL, "Okre¶l nazwê pliku ROM-u 2K lub 8K w $E000 dla 6809"},
/* ru */ {IDCLS_SPECIFY_6809_ROM_AT_E000_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_6809_ROM_AT_E000_SV, "Ange namn för 2/8K-utöknings-ROM på $E000 för 6809"},
/* tr */ {IDCLS_SPECIFY_6809_ROM_AT_E000_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_6809_ROM_AT_F000,    N_("Specify 4K ROM file name at $F000 for 6809")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_6809_ROM_AT_F000_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_6809_ROM_AT_F000_DE, "4K Erweiterungs ROM Dateiname für $F000 für 6809 wählen"},
/* es */ {IDCLS_SPECIFY_6809_ROM_AT_F000_ES, "Especificar nombre ROM 4K en $F000 para 6809"},
/* fr */ {IDCLS_SPECIFY_6809_ROM_AT_F000_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_6809_ROM_AT_F000_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_6809_ROM_AT_F000_IT, "Specifica il nome della ROM da 4K a $F000 per il 6809"},
/* ko */ {IDCLS_SPECIFY_6809_ROM_AT_F000_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_6809_ROM_AT_F000_NL, "Geef de naam van het bestand voor de 4K ROM op $F000 voor de 6809"},
/* pl */ {IDCLS_SPECIFY_6809_ROM_AT_F000_PL, "Okre¶l nazwê pliku ROM-u 4K w $F000 dla 6809"},
/* ru */ {IDCLS_SPECIFY_6809_ROM_AT_F000_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_6809_ROM_AT_F000_SV, "Ange namn för 4K-utöknings-ROM på $F000 för 6809"},
/* tr */ {IDCLS_SPECIFY_6809_ROM_AT_F000_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET4032_COLOUR_RGBI,    N_("RGBI colour extension to PET 4032")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET4032_COLOUR_RGBI_DA, ""},  /* fuzzy */
/* de */ {IDCLS_PET4032_COLOUR_RGBI_DE, "RGBI Farberweiterung für PET 4032"},
/* es */ {IDCLS_PET4032_COLOUR_RGBI_ES, "Extension color RGBI para PET 4032"},
/* fr */ {IDCLS_PET4032_COLOUR_RGBI_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_PET4032_COLOUR_RGBI_HU, ""},  /* fuzzy */
/* it */ {IDCLS_PET4032_COLOUR_RGBI_IT, "Estensione colore RGBI per PET 4032"},
/* ko */ {IDCLS_PET4032_COLOUR_RGBI_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PET4032_COLOUR_RGBI_NL, "RGBI kleur uitbreiding voor de PET 4032"},
/* pl */ {IDCLS_PET4032_COLOUR_RGBI_PL, "Rozszerzenie barw RGBI do PET 4032"},
/* ru */ {IDCLS_PET4032_COLOUR_RGBI_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_PET4032_COLOUR_RGBI_SV, "RGBI-färgutökning för PET 4032"},
/* tr */ {IDCLS_PET4032_COLOUR_RGBI_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET4032_COLOUR_ANALOG,    N_("Analog colour extension to PET 4032")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET4032_COLOUR_ANALOG_DA, ""},  /* fuzzy */
/* de */ {IDCLS_PET4032_COLOUR_ANALOG_DE, "Analog Farberweiterung für PET 4032"},
/* es */ {IDCLS_PET4032_COLOUR_ANALOG_ES, "Extension analógica de color para PET 4032"},
/* fr */ {IDCLS_PET4032_COLOUR_ANALOG_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_PET4032_COLOUR_ANALOG_HU, ""},  /* fuzzy */
/* it */ {IDCLS_PET4032_COLOUR_ANALOG_IT, "Estensione colore analogica per PET 4032"},
/* ko */ {IDCLS_PET4032_COLOUR_ANALOG_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PET4032_COLOUR_ANALOG_NL, "Analoge kleur uitbreiding voor de PET 4032"},
/* pl */ {IDCLS_PET4032_COLOUR_ANALOG_PL, "Rozszerzenie analogowych barw do PET 4032"},
/* ru */ {IDCLS_PET4032_COLOUR_ANALOG_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_PET4032_COLOUR_ANALOG_SV, "Analog färgutökning för PET 4032"},
/* tr */ {IDCLS_PET4032_COLOUR_ANALOG_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_P_COLOUR_0_255,    N_("<Colour 0-255>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_COLOUR_0_255_DA, ""},  /* fuzzy */
/* de */ {IDCLS_P_COLOUR_0_255_DE, "<Farbe 0-255>"},
/* es */ {IDCLS_P_COLOUR_0_255_ES, "<Color 0-255>"},
/* fr */ {IDCLS_P_COLOUR_0_255_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_P_COLOUR_0_255_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_COLOUR_0_255_IT, "<Colore 0-255>"},
/* ko */ {IDCLS_P_COLOUR_0_255_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_COLOUR_0_255_NL, "<Kleur 0-255>"},
/* pl */ {IDCLS_P_COLOUR_0_255_PL, "<Kolor 0-255>"},
/* ru */ {IDCLS_P_COLOUR_0_255_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_P_COLOUR_0_255_SV, "<Färg 0-255>"},
/* tr */ {IDCLS_P_COLOUR_0_255_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET4032_COLOUR_BACKGROUND,    N_("Analog colour background on PET 4032")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET4032_COLOUR_BACKGROUND_DA, ""},  /* fuzzy */
/* de */ {IDCLS_PET4032_COLOUR_BACKGROUND_DE, "Analog Hintergrundfarbe für PET 4032"},
/* es */ {IDCLS_PET4032_COLOUR_BACKGROUND_ES, "Color del fondo analógico para PET 4032"},
/* fr */ {IDCLS_PET4032_COLOUR_BACKGROUND_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_PET4032_COLOUR_BACKGROUND_HU, ""},  /* fuzzy */
/* it */ {IDCLS_PET4032_COLOUR_BACKGROUND_IT, "Coloro sfondo analogico su PET 4032"},
/* ko */ {IDCLS_PET4032_COLOUR_BACKGROUND_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PET4032_COLOUR_BACKGROUND_NL, "Analoge kleuren achtergrond voor de PET 4032"},
/* pl */ {IDCLS_PET4032_COLOUR_BACKGROUND_PL, "Analogowy kolor t³a w PET 4032"},
/* ru */ {IDCLS_PET4032_COLOUR_BACKGROUND_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_PET4032_COLOUR_BACKGROUND_SV, "Analog färgbakgrund på PET 4032"},
/* tr */ {IDCLS_PET4032_COLOUR_BACKGROUND_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET_RAM_SIZE,    N_("PET RAM size (4/8/16/32/96/128)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET_RAM_SIZE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_PET_RAM_SIZE_DE, "PET RAM Größe (4/8/16/32/96/128)"},
/* es */ {IDCLS_PET_RAM_SIZE_ES, "Tamaño de la RAM en PET  (4/8/16/32/96/128)"},
/* fr */ {IDCLS_PET_RAM_SIZE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_PET_RAM_SIZE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_PET_RAM_SIZE_IT, "Dimensione RAM PET (4/8/16/32/96/128)"},
/* ko */ {IDCLS_PET_RAM_SIZE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PET_RAM_SIZE_NL, "PET RAM grootte (4/8/16/32/96/128)"},
/* pl */ {IDCLS_PET_RAM_SIZE_PL, "Wielko¶æ RAM-u PET (4/8/16/32/96/128)"},
/* ru */ {IDCLS_PET_RAM_SIZE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_PET_RAM_SIZE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_PET_RAM_SIZE_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_PET_IO_SIZE,    N_("PET I/O size (256/2048)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET_IO_SIZE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_PET_IO_SIZE_DE, "PET I/O Größe (256/2048)"},
/* es */ {IDCLS_PET_IO_SIZE_ES, "Tamaño PET I/O (256/2048)"},
/* fr */ {IDCLS_PET_IO_SIZE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_PET_IO_SIZE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_PET_IO_SIZE_IT, "Dimensione I/O PET (256/2048)"},
/* ko */ {IDCLS_PET_IO_SIZE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PET_IO_SIZE_NL, "PET I/O grootte (256/2048)"},
/* pl */ {IDCLS_PET_IO_SIZE_PL, "Rozmiar we/wy PET (256/2048)"},
/* ru */ {IDCLS_PET_IO_SIZE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_PET_IO_SIZE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_PET_IO_SIZE_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_ENABLE_CRTC,    N_("Enable CRTC")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_CRTC_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_CRTC_DE, "CRTC aktivieren"},
/* es */ {IDCLS_ENABLE_CRTC_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_CRTC_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_CRTC_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_CRTC_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_CRTC_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_CRTC_NL, "CRTC inschakelen"},
/* pl */ {IDCLS_ENABLE_CRTC_PL, "W³±cz CRTC"},
/* ru */ {IDCLS_ENABLE_CRTC_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_CRTC_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_CRTC_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_DISABLE_CRTC,    N_("Disable CRTC")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_CRTC_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_CRTC_DE, "CRTC deaktivieren"},
/* es */ {IDCLS_DISABLE_CRTC_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_CRTC_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_CRTC_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_CRTC_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_CRTC_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_CRTC_NL, "CRTC uitschakelen"},
/* pl */ {IDCLS_DISABLE_CRTC_PL, "Wy³±cz CRTC"},
/* ru */ {IDCLS_DISABLE_CRTC_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_CRTC_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_CRTC_TR, ""},  /* fuzzy */
#endif

/* pet/pet-cmdline-options.c */
/* en */ {IDCLS_SET_VIDEO_SIZE,    N_("Set video size (0: Automatic, 40: 40 Columns, 80: 80 Columns)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_VIDEO_SIZE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_VIDEO_SIZE_DE, "Video Größe (0: Automatisch, 40: 40 Spalten, 80: 80 Spalten)"},
/* es */ {IDCLS_SET_VIDEO_SIZE_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_VIDEO_SIZE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_VIDEO_SIZE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_VIDEO_SIZE_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SET_VIDEO_SIZE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_VIDEO_SIZE_NL, "Stel video grootte in (0: Automatisch, 40: 40 Kolommen, 80: 80 Kolommen)"},
/* pl */ {IDCLS_SET_VIDEO_SIZE_PL, "Okre¶l wielko¶æ obrazu (0: Automatyczna, 40: 40 kolumn, 80: 80 kolumn)"},
/* ru */ {IDCLS_SET_VIDEO_SIZE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VIDEO_SIZE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SET_VIDEO_SIZE_TR, ""},  /* fuzzy */
#endif

/* pet/petpia1.c */
/* en */ {IDCLS_ENABLE_USERPORT_DIAG_PIN,    N_("Enable userport diagnostic pin")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_DA, "Aktivér diagnostik på brugerporten"},
/* de */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_DE, "Userport Diagnose Pin aktivieren"},
/* es */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_ES, "Permitir pin puerto diagnostico"},
/* fr */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_FR, "Activer la pin de diagnostique userport"},
/* hu */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_HU, "Userport diagnosztikai láb engedélyezése"},
/* it */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_IT, "Attiva pin diagnostico su userport"},
/* ko */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_NL, "Userport diagnostic pin inschakelen"},
/* pl */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_PL, "W³±cz pin diagnostyczny userportu"},
/* ru */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_RU, "Enable userport diagnostic pin"},
/* sv */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_SV, "Aktivera diagnostik på användarporten"},
/* tr */ {IDCLS_ENABLE_USERPORT_DIAG_PIN_TR, "Userport diyagnostik pinini aktif et"},
#endif

/* pet/petpia1.c */
/* en */ {IDCLS_DISABLE_USERPORT_DIAG_PIN,    N_("Disable userport diagnostic pin")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_DA, "Deaktivér diagnostik på brugerporten"},
/* de */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_DE, "Userport Diagnose Pin deaktivieren"},
/* es */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_ES, "Deshabilitar pin puerto diagnostico"},
/* fr */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_FR, "Désactiver la pin de diagnostique userport"},
/* hu */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_HU, "Userport diagnosztikai láb tiltása"},
/* it */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_IT, "Disattiva pin diagnostico su userport"},
/* ko */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_NL, "Userport diagnostic pin uitschakelen"},
/* pl */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_PL, "Wy³±cz pin diagnostyczny userportu"},
/* ru */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_RU, "Disable userport diagnostic pin"},
/* sv */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_SV, "Inaktivera diagnostik på användarporten"},
/* tr */ {IDCLS_DISABLE_USERPORT_DIAG_PIN_TR, "Userport diyagnostik pinini pasifleþtir"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME,    N_("Specify name of Function low ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_DA, "Angiv navn på image for lav funktions-ROM"},
/* de */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_DE, "Namen für Funktions ROM Datei (unterer Bereich) wählen"},
/* es */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_ES, "Especificar nombre imagen ROM función baja"},
/* fr */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_FR, "Spécifier le nom de l'image basse ROM Fonction"},
/* hu */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_HU, "Adja meg az alsó Function ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_IT, "Specifica il nome immagine della Function ROM bassa"},
/* ko */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_NL, "Geef de naam van het Function low ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM Function low"},
/* ru */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_RU, "Specify name of Function low ROM image"},
/* sv */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_SV, "Ange namn på ROM-avbildning för lågt funktions-ROM"},
/* tr */ {IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_TR, "Function low ROM imajýnýn ismini belirt"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME,    N_("Specify name of Function high ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_DA, "Angiv navn på image for høj funktions-ROM"},
/* de */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_DE, "Namen für Funktions ROM Datei (oberer Bereich) wählen"},
/* es */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_ES, "Especificar nombre imagen ROM función alta"},
/* fr */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_FR, "Spécifier le nom de l'image haute ROM Fonction"},
/* hu */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_HU, "Adja meg a felsõ Function ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_IT, "Specifica il nome immagine della Function ROM alta"},
/* ko */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_NL, "Geef de naam van het Function high ROM bestand"},
/* pl */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM Function high"},
/* ru */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_RU, "Specify name of Function high ROM image"},
/* sv */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_SV, "Ange namn på ROM-avbildning för högt funktions-ROM"},
/* tr */ {IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_TR, "Function high ROM imajýnýn ismini belirt"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME,    N_("Specify name of Cartridge 1 low ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_DA, "Angiv navn på image for lav cartridge 1 ROM"},
/* de */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_DE, "Name für Erweiterungsmodul 1 ROM Datei (unterer Bereich) wählen"},
/* es */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_ES, "Especificar nombre imagen ROM cartucho 1 bajo"},
/* fr */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_FR, "Spécifier le nom de l'image basse ROM Cartouche 1"},
/* hu */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_HU, "Adja meg az alsó Cartridge 1 ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_IT, "Specifica il nome immagine della ROM Cartridge 1 bassa"},
/* ko */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_NL, "Geef de naam van het Cartridge 1 low ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM Cartridge 1 low"},
/* ru */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_RU, "Specify name of Cartridge 1 low ROM image"},
/* sv */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_SV, "Ange namn på ROM-avbildning för lågt insticksmodul 1-ROM"},
/* tr */ {IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_TR, "Cartridge 1 low ROM imajýnýn ismini belirt"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME,    N_("Specify name of Cartridge 1 high ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_DA, "Angiv navn på image for høj cartridge 1 ROM"},
/* de */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_DE, "Name für Erweiterungsmodul 1 ROM Datei (oberer Bereich) wählen"},
/* es */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_ES, "Especificar nombre imagen ROM cartucho 1 alto"},
/* fr */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_FR, "Spécifier le nom de l'image haute ROM Cartouche 1"},
/* hu */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_HU, "Adja meg a felsõ Cartridge 1 ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_IT, "Specifica il nome immagine della ROM Cartridge 1 alta"},
/* ko */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_NL, "Geef de naam van het Cartridge 1 high ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM Cartridge 1 high"},
/* ru */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_RU, "Specify name of Cartridge 1 high ROM image"},
/* sv */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_SV, "Ange namn på ROM-avbildning för högt insticksmodul 1-ROM"},
/* tr */ {IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_TR, "Cartridge 1 high ROM imajýnýn ismini belirt"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME,    N_("Specify name of Cartridge 2 low ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_DA, "Angiv navn på image for lav cartridge 2 ROM"},
/* de */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_DE, "Name für Erweiterungsmodul 2 ROM Datei (unterer Bereich) wählen"},
/* es */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_ES, "Especificar nombre imagen ROM cartucho 2 bajo"},
/* fr */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_FR, "Spécifier le nom de l'image basse ROM Cartouche 2"},
/* hu */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_HU, "Adja meg az alsó Cartridge 2 ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_IT, "Specifica il nome immagine della ROM Cartridge 2 bassa"},
/* ko */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_NL, "Geef de naam van het Cartridge 2 low ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM Cartridge 2 low"},
/* ru */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_RU, "Specify name of Cartridge 2 low ROM image"},
/* sv */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_SV, "Ange namn på ROM-avbildning för lågt insticksmodul 2-ROM"},
/* tr */ {IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_TR, "Cartridge 2 low ROM imajýnýn ismini belirt"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME,    N_("Specify name of Cartridge 2 high ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_DA, "Angiv navn på image for høj cartridge 1 ROM"},
/* de */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_DE, "Name für Erweiterungsmodul 2 ROM Datei (oberer Bereich) wählen"},
/* es */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_ES, "Especificar nombre imagen ROM cartucho 2 alto"},
/* fr */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_FR, "Spécifier le nom de l'image haute ROM Cartouche 2"},
/* hu */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_HU, "Adja meg a felsõ Cartridge 2 ROM képmás nevét"},
/* it */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_IT, "Specifica il nome immagine della ROM Cartridge 2 alta"},
/* ko */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_NL, "Geef de naam van het Cartridge 2 high ROM bestand"},
/* pl */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_PL, "Okre¶l nazwê obrazu ROM Cartridge 2 high"},
/* ru */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_RU, "Specify name of Cartridge 2 high ROM image"},
/* sv */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_SV, "Ange namn på ROM-avbildning för högt insticksmodul 2-ROM"},
/* tr */ {IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_TR, "Cartridge 2 high ROM imajýnýn ismini belirt"},
#endif

/* plus4/plus4-cmdline-options.c */
/* en */ {IDCLS_SPECIFY_RAM_INSTALLED,    N_("Specify size of RAM installed in kb (16/32/64)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RAM_INSTALLED_DA, "Angiv størrelse på installeret RAM i kB (16/32/64)"},
/* de */ {IDCLS_SPECIFY_RAM_INSTALLED_DE, "Größe des emulierten RAM Speichers in kb (16/32/64) spezifizieren"},
/* es */ {IDCLS_SPECIFY_RAM_INSTALLED_ES, "Especificar tamaño RAM instalada en KB (16/32/64)"},
/* fr */ {IDCLS_SPECIFY_RAM_INSTALLED_FR, "Spécifier la taille du RAM installé (16/32/64)"},
/* hu */ {IDCLS_SPECIFY_RAM_INSTALLED_HU, "Adja meg a telepített RAM méretét kb-ban (16/32/64)"},
/* it */ {IDCLS_SPECIFY_RAM_INSTALLED_IT, "Specifica la dimensione della RAM installata in kb (16/32/64)"},
/* ko */ {IDCLS_SPECIFY_RAM_INSTALLED_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RAM_INSTALLED_NL, "Geef de hoeveelheid RAM in kb (16/32/64)"},
/* pl */ {IDCLS_SPECIFY_RAM_INSTALLED_PL, "Okre¶l ilo¶æ zainstalowanego RAM-u w kb (16/32/64)"},
/* ru */ {IDCLS_SPECIFY_RAM_INSTALLED_RU, "Specify size of RAM installed in kb (16/32/64)"},
/* sv */ {IDCLS_SPECIFY_RAM_INSTALLED_SV, "Ange storlek på installerat RAM i kb (16/32/64)"},
/* tr */ {IDCLS_SPECIFY_RAM_INSTALLED_TR, "Kurulmuþ olan RAM boyutunu kb cinsinden belirt (16/32/64)"},
#endif

/* viciisc/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_VICII_MODEL,    N_("Set VIC-II model (6569/6569r1/8565/6567/8562/6567r56a/6572)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_VICII_MODEL_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_VICII_MODEL_DE, "Wähle VIC-II Modell (6569/6569r1/8565/6567/8562/6567r56a/6572)"},
/* es */ {IDCLS_SET_VICII_MODEL_ES, "Seleccionar modelo VIC-II (6569/6569r1/8565/6567/8562/6567r56a/6572)"},
/* fr */ {IDCLS_SET_VICII_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_VICII_MODEL_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_VICII_MODEL_IT, "Imposta modello VIC-II (6569/6569r1/8565/6567/8562/6567r56a/6572)"},
/* ko */ {IDCLS_SET_VICII_MODEL_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_VICII_MODEL_NL, "Zet VIC-II model (6569/6569r1/8565/6567/8562/6567r56a/6572)"},
/* pl */ {IDCLS_SET_VICII_MODEL_PL, "Ustaw model VIC-II (6569/6569r1/8565/6567/8562/6567r56a/6572)"},
/* ru */ {IDCLS_SET_VICII_MODEL_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_VICII_MODEL_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SET_VICII_MODEL_TR, ""},  /* fuzzy */
#endif

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME,    N_("Specify name of printer driver for device #4. (ascii/mps803/nl10/raw)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_DE, "Name für Druckertreiber für Gerät #4 wählen. (ascii/mps803/nl10/raw)"},
/* es */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_ES, "Especificar nombre controlador impresora periférico #4. (ascii/mps803/nl10/raw)"},
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_IT, "Specifica il nome del driver di stampa per la periferica #4. (ascii/mps803/nl10/raw)"},
/* ko */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_NL, "Geef de naam van het stuurprogramma voor apparaat #4. (ascii/mps803/nl10/raw)"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_PL, "Okre¶l nazwê sterownika drukarki urz±dzenia #4. (ascii/mps803/nl10/raw)"},
/* ru */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_PRT_DRIVER_4_NAME_TR, ""},  /* fuzzy */
#endif

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME,    N_("Specify name of printer driver for device #5. (ascii/mps803/nl10/raw)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_DE, "Name für Druckertreiber für Gerät #5 wählen. (ascii/mps803/nl10/raw)"},
/* es */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_ES, "Especificar nombre controlador impresora periférico #5. (ascii/mps803/nl10/raw)"},
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_IT, "Specifica il nome del driver di stampa per la periferica #5. (ascii/mps803/nl10/raw)"},
/* ko */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_NL, "Geef de naam van het stuurprogramma voor apparaat #5. (ascii/mps803/nl10/raw)"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_PL, "Okre¶l nazwê sterownika drukarki urz±dzenia #5. (ascii/mps803/nl10/raw)"},
/* ru */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_PRT_DRIVER_5_NAME_TR, ""},  /* fuzzy */
#endif

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME,    N_("Specify name of printer driver for device #6. (1520/raw)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME_DE, "Name für Druckertreiber für Gerät #6 wählen. (1520/raw)"},
/* es */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME_ES, "Especificar nombre controlador impresora periférico #6. (ascii/mps803/nl10/raw)"},
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME_IT, "Specifica il nome del driver di stampa per la periferica #4. (1520/raw)"},
/* ko */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME_NL, "Geef de naam van het stuurprogramma voor apparaat #6. (1520/raw)"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME_PL, "Okre¶l nazwê sterownika drukarki urz±dzenia #6. (1520/raw)"},
/* ru */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_PRT_DRIVER_6_NAME_TR, ""},  /* fuzzy */
#endif

/* printerdrv/driver-select.c */
/* en */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME,    N_("Specify name of printer driver for the userport printer. (ascii/nl10/raw)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_DE, "Name für Userport Druckertreiber wählen. (ascii/nl10/raw)"},
/* es */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_ES, "Especificar nombre controlador impresión para puerto impresora. (ascii/nl10/raw)"},
/* fr */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_IT, "Specifica il nome del driver di stampa per la stampante su userport. (ascii/nl10/raw)"},
/* ko */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_NL, "Geef de naam van het stuurprogramma voor de userport printer. (ascii/nl10/raw)"},
/* pl */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_PL, "Okre¶l nazwê sterownika drukarki dla drukarki w userporcie. (ascii/nl10/raw)"},
/* ru */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_TR, ""},  /* fuzzy */
#endif

/* printerdrv/interface-serial.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_6,    N_("Set device type for device #6 (0: None, 1: Filesystem, 2: Real)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DEVICE_TYPE_6_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_DEVICE_TYPE_6_DE, "Gerätetyp für Gerät #6 (0: Kein, 1: Dateisystem, 2: Real) setzen"},
/* es */ {IDCLS_SET_DEVICE_TYPE_6_ES, "Seleccionar tipo para el periférico #6 (0:Ninguno 1: Filesystem, 2:Real)"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_6_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_DEVICE_TYPE_6_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DEVICE_TYPE_6_IT, "Specifica il tipo di periferica #6 (0: Nessuna, 1: Filesystem, 2: Reale)"},
/* ko */ {IDCLS_SET_DEVICE_TYPE_6_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DEVICE_TYPE_6_NL, "Stel apparaatsoort in voor apparaat #6 (0: Geen, 1: Bestandssysteem, 2: Echt)"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_6_PL, "Okre¶l typ urz±dzenia #6 (0: Brak, 1: System plików, 2: Rzeczywiste)"},
/* ru */ {IDCLS_SET_DEVICE_TYPE_6_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_6_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SET_DEVICE_TYPE_6_TR, ""},  /* fuzzy */
#endif

/* printerdrv/interface-serial.c */
/* en */ {IDCLS_SET_DEVICE_TYPE_7,    N_("Set device type for device #7 (0: None, 2: Real)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_DEVICE_TYPE_7_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_DEVICE_TYPE_7_DE, "Gerätetyp für Gerät #7 (0: Kein, 2: Real) setzen"},
/* es */ {IDCLS_SET_DEVICE_TYPE_7_ES, "Seleccionar tipo para el periférico #7 (0:Ninguno 1: Filesystem, 2:Real)"},
/* fr */ {IDCLS_SET_DEVICE_TYPE_7_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_DEVICE_TYPE_7_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_DEVICE_TYPE_7_IT, "Specifica il tipo di periferica #7 (0: Nessuna, 1: Filesystem, 2: Reale)"},
/* ko */ {IDCLS_SET_DEVICE_TYPE_7_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_DEVICE_TYPE_7_NL, "Stel apparaatsoort in voor apparaat #7 (0: Geen, 2: Echt)"},
/* pl */ {IDCLS_SET_DEVICE_TYPE_7_PL, "Okre¶l typ urz±dzenia #7 (0: Brak, 2: Rzeczywiste)"},
/* ru */ {IDCLS_SET_DEVICE_TYPE_7_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_DEVICE_TYPE_7_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SET_DEVICE_TYPE_7_TR, ""},  /* fuzzy */
#endif

/* printerdrv/interface-userport.c */
/* en */ {IDCLS_ENABLE_USERPORT_PRINTER,    N_("Enable the userport printer emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_USERPORT_PRINTER_DA, "Aktivér emulering af printer på brugerporten"},
/* de */ {IDCLS_ENABLE_USERPORT_PRINTER_DE, "Userport Drucker Emulation aktivieren"},
/* es */ {IDCLS_ENABLE_USERPORT_PRINTER_ES, "Permitir emulación puerto impresora"},
/* fr */ {IDCLS_ENABLE_USERPORT_PRINTER_FR, "Acviver l'émulation de l'imprimante userport"},
/* hu */ {IDCLS_ENABLE_USERPORT_PRINTER_HU, "Userport nyomtató emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_USERPORT_PRINTER_IT, "Attiva l'emulazione della stampante su userport"},
/* ko */ {IDCLS_ENABLE_USERPORT_PRINTER_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_USERPORT_PRINTER_NL, "Userport printeremulatie inschakelen"},
/* pl */ {IDCLS_ENABLE_USERPORT_PRINTER_PL, "W³±cz emulacjê drukarki w userporcie"},
/* ru */ {IDCLS_ENABLE_USERPORT_PRINTER_RU, "Enable the userport printer emulation"},
/* sv */ {IDCLS_ENABLE_USERPORT_PRINTER_SV, "Aktivera emulering av skrivare på användarporten"},
/* tr */ {IDCLS_ENABLE_USERPORT_PRINTER_TR, "Userport yazýcý emülasyonunu aktif et"},
#endif

/* printerdrv/interface-userport.c */
/* en */ {IDCLS_DISABLE_USERPORT_PRINTER,    N_("Disable the userport printer emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_USERPORT_PRINTER_DA, "Deaktivér emulering af printer på brugerporten"},
/* de */ {IDCLS_DISABLE_USERPORT_PRINTER_DE, "Userport Drucker Emulation deaktivieren"},
/* es */ {IDCLS_DISABLE_USERPORT_PRINTER_ES, "Deshabilitar emulación puerto impresora"},
/* fr */ {IDCLS_DISABLE_USERPORT_PRINTER_FR, "Désactiver l'émulation de l'imprimante userport"},
/* hu */ {IDCLS_DISABLE_USERPORT_PRINTER_HU, "Userport nyomtató emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_USERPORT_PRINTER_IT, "Disattiva l'emulazione della stampante su userport"},
/* ko */ {IDCLS_DISABLE_USERPORT_PRINTER_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_USERPORT_PRINTER_NL, "Userport printeremulatie uitschakelen"},
/* pl */ {IDCLS_DISABLE_USERPORT_PRINTER_PL, "Wy³±cz emulacjê drukarki w userporcie"},
/* ru */ {IDCLS_DISABLE_USERPORT_PRINTER_RU, "Disable the userport printer emulation"},
/* sv */ {IDCLS_DISABLE_USERPORT_PRINTER_SV, "Inaktivera emulering av skrivare på användarporten"},
/* tr */ {IDCLS_DISABLE_USERPORT_PRINTER_TR, "Userport yazýcý emülasyonunu pasifleþtir"},
#endif

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME,    N_("Specify name of printer text device or dump file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_DA, "Angiv navn på printer-tekst-enhed eller dump-fil"},
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_DE, "Name für Druckerausgabe Textgerät oder Dump Datei wählen"},
/* es */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_ES, "Especificar nombre del periférico para imprimir texto o fichero de volcado"},
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_FR, "Spécifier le nom de l'imprimante texte ou \"dump file\""},
/* hu */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_HU, "Adja meg a szövegnyomtató eszköz nevét, vagy a kimeneti fájlt"},
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_IT, "Specifica il nome del dispositivo testuale di stampa o salva su file"},
/* ko */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_NL, "Geef de naam van het printer tekst apparaat of het dumpbestand"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_PL, "Okre¶l nazwê urz±dzenia tekstowego drukarki lub pliku zrzutu"},
/* ru */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_RU, "Specify name of printer text device or dump file"},
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_SV, "Ange namn på textenhet för utskrift eller dumpfil"},
/* tr */ {IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_TR, "Metin aygýtý yazýcýsý ya da döküm dosyasýnýn ismini belirt"},
#endif

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_4,    N_("Specify printer text output device for printer #4")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TEXT_DEVICE_4_DA, "Angiv printer tekst-uddata enhed for printer #4"},
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_4_DE, "Textausgabe Gerät für Drucker #4 wählen"},
/* es */ {IDCLS_SPECIFY_TEXT_DEVICE_4_ES, "Especificar periférico salida impresión texto impresora #4"},
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_4_FR, "Spécifier le périphérique de sortie texte pour l'imprimante 4"},
/* hu */ {IDCLS_SPECIFY_TEXT_DEVICE_4_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_4_IT, "Specifica il dispositivo di uscita testuale per la stampante #4"},
/* ko */ {IDCLS_SPECIFY_TEXT_DEVICE_4_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_4_NL, "Geef het printer tekst uitvoer apparaat voor printer #4"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_4_PL, "Okre¶l urz±dzenie wyj¶cia tekstowego drukarki #4"},
/* ru */ {IDCLS_SPECIFY_TEXT_DEVICE_4_RU, "Specify printer text output device for printer #4"},
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_4_SV, "Ange skrivartextutenhet för skrivare 4"},
/* tr */ {IDCLS_SPECIFY_TEXT_DEVICE_4_TR, "Yazýcý #4 için metin çýktý aygýtý yazýcýsý belirt"},
#endif

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_5,    N_("Specify printer text output device for printer #5")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TEXT_DEVICE_5_DA, "Angiv printer tekst-uddata enhed for printer #5"},
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_5_DE, "Textausgabe Gerät für Drucker #5 wählen"},
/* es */ {IDCLS_SPECIFY_TEXT_DEVICE_5_ES, "Especificar periférico salida impresión texto impresora #5"},
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_5_FR, "Spécifier le périphérique de sortie texte pour l'imprimante 5"},
/* hu */ {IDCLS_SPECIFY_TEXT_DEVICE_5_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_5_IT, "Specifica il dispositivo di uscita testuale per la stampante #5"},
/* ko */ {IDCLS_SPECIFY_TEXT_DEVICE_5_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_5_NL, "Geef het printer tekst uitvoer apparaat voor printer #5"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_5_PL, "Okre¶l urz±dzenie wyj¶cia tekstowego drukarki #5"},
/* ru */ {IDCLS_SPECIFY_TEXT_DEVICE_5_RU, "Specify printer text output device for printer #5"},
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_5_SV, "Ange skrivartextutenhet för skrivare 5"},
/* tr */ {IDCLS_SPECIFY_TEXT_DEVICE_5_TR, "Yazýcý #5 için metin çýktý aygýtý yazýcýsý belirt"},
#endif

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_DEVICE_6,    N_("Specify printer text output device for printer #6")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TEXT_DEVICE_6_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_TEXT_DEVICE_6_DE, "Textausgabe Gerät für Drucker #6 wählen"},
/* es */ {IDCLS_SPECIFY_TEXT_DEVICE_6_ES, "Especificar periférico salida impresión texto impresora #6"},
/* fr */ {IDCLS_SPECIFY_TEXT_DEVICE_6_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_TEXT_DEVICE_6_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_TEXT_DEVICE_6_IT, "Specifica il dispositivo di uscita testuale per la stampante #6"},
/* ko */ {IDCLS_SPECIFY_TEXT_DEVICE_6_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TEXT_DEVICE_6_NL, "Geef het printer tekst uitvoer apparaat voor printer #6"},
/* pl */ {IDCLS_SPECIFY_TEXT_DEVICE_6_PL, "Okre¶l urz±dzenie wyj¶cia tekstowego drukarki #6"},
/* ru */ {IDCLS_SPECIFY_TEXT_DEVICE_6_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TEXT_DEVICE_6_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_TEXT_DEVICE_6_TR, ""},  /* fuzzy */
#endif

/* printerdrv/output-text.c */
/* en */ {IDCLS_SPECIFY_TEXT_USERPORT,    N_("Specify printer text output device for userport printer")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TEXT_USERPORT_DA, "Angiv printer tekst-uddata enhed for brugerports-printer"},
/* de */ {IDCLS_SPECIFY_TEXT_USERPORT_DE, "Textausgabe Gerät für Userport Drucker wählen"},
/* es */ {IDCLS_SPECIFY_TEXT_USERPORT_ES, "Especificar periférico de salida para imprimir texto impresora puerto usuario"},
/* fr */ {IDCLS_SPECIFY_TEXT_USERPORT_FR, "Spécifier le périphérique de sortie text pour l'imprimante userport"},
/* hu */ {IDCLS_SPECIFY_TEXT_USERPORT_HU, "Adja meg a szövegnyomtató eszközt a userport nyomtatóhoz"},
/* it */ {IDCLS_SPECIFY_TEXT_USERPORT_IT, "Specifica il nome del dispositivo di output testuale di stampa per la stampante su userport"},
/* ko */ {IDCLS_SPECIFY_TEXT_USERPORT_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TEXT_USERPORT_NL, "Geef het printer tekst uitvoer apparaat voor de userport printer"},
/* pl */ {IDCLS_SPECIFY_TEXT_USERPORT_PL, "Okre¶l urz±dzenie wyj¶cia tekstu dla drukarki w userporcie"},
/* ru */ {IDCLS_SPECIFY_TEXT_USERPORT_RU, "Specify printer text output device for userport printer"},
/* sv */ {IDCLS_SPECIFY_TEXT_USERPORT_SV, "Ange skrivartextutenhet för användarportsskrivare"},
/* tr */ {IDCLS_SPECIFY_TEXT_USERPORT_TR, "Userport yazýcýsý için metin çýktý aygýtý yazýcýsý belirt"},
#endif

/* raster/raster-cmdline-options.c */
/* en */ {IDCLS_ENABLE_VIDEO_CACHE,    N_("Enable the video cache")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_VIDEO_CACHE_DA, "Aktivér videocache"},
/* de */ {IDCLS_ENABLE_VIDEO_CACHE_DE, "Video Cache aktivieren"},
/* es */ {IDCLS_ENABLE_VIDEO_CACHE_ES, "Permitir caché de video"},
/* fr */ {IDCLS_ENABLE_VIDEO_CACHE_FR, "Activer le cache vidéo"},
/* hu */ {IDCLS_ENABLE_VIDEO_CACHE_HU, "Videó gyorsítótár engedélyezése"},
/* it */ {IDCLS_ENABLE_VIDEO_CACHE_IT, "Attiva la cache video"},
/* ko */ {IDCLS_ENABLE_VIDEO_CACHE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_VIDEO_CACHE_NL, "Videocache inschakelen"},
/* pl */ {IDCLS_ENABLE_VIDEO_CACHE_PL, "W³±cz cache wideo"},
/* ru */ {IDCLS_ENABLE_VIDEO_CACHE_RU, "Enable the video cache"},
/* sv */ {IDCLS_ENABLE_VIDEO_CACHE_SV, "Aktivera videocache"},
/* tr */ {IDCLS_ENABLE_VIDEO_CACHE_TR, "Görüntü önbelleðini aktif et"},
#endif

/* raster/raster-cmdline-options.c */
/* en */ {IDCLS_DISABLE_VIDEO_CACHE,    N_("Disable the video cache")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_VIDEO_CACHE_DA, "Deaktivér videocache"},
/* de */ {IDCLS_DISABLE_VIDEO_CACHE_DE, "Video Cache deaktivieren"},
/* es */ {IDCLS_DISABLE_VIDEO_CACHE_ES, "Deshabilitar caché de vídeo"},
/* fr */ {IDCLS_DISABLE_VIDEO_CACHE_FR, "Désactiver le cache vidéo"},
/* hu */ {IDCLS_DISABLE_VIDEO_CACHE_HU, "Videó gyorsítótár tiltása"},
/* it */ {IDCLS_DISABLE_VIDEO_CACHE_IT, "Disattiva la cache video"},
/* ko */ {IDCLS_DISABLE_VIDEO_CACHE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_VIDEO_CACHE_NL, "Videocache uitschakelen"},
/* pl */ {IDCLS_DISABLE_VIDEO_CACHE_PL, "Wy³±cz cache wideo"},
/* ru */ {IDCLS_DISABLE_VIDEO_CACHE_RU, "Disable the video cache"},
/* sv */ {IDCLS_DISABLE_VIDEO_CACHE_SV, "Inaktivera videocache"},
/* tr */ {IDCLS_DISABLE_VIDEO_CACHE_TR, "Görüntü önbelleðini pasifleþtir"},
#endif

#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_1_NAME,    N_("Specify name of first RS232 device")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RS232_1_NAME_DA, "Angiv navn på første RS232-enhed"},
/* de */ {IDCLS_SPECIFY_RS232_1_NAME_DE, "Name für erstes RS232 Gerät wählen"},
/* es */ {IDCLS_SPECIFY_RS232_1_NAME_ES, "Especificar el nombre del primer periférico RS232"},
/* fr */ {IDCLS_SPECIFY_RS232_1_NAME_FR, "Spécifier le nom du 1er périphérique RS232"},
/* hu */ {IDCLS_SPECIFY_RS232_1_NAME_HU, "Adja meg az elsõ RS232-es eszköz nevét"},
/* it */ {IDCLS_SPECIFY_RS232_1_NAME_IT, "Specifica il nome della prima RS232"},
/* ko */ {IDCLS_SPECIFY_RS232_1_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RS232_1_NAME_NL, "Geef de naam van het eerste RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_1_NAME_PL, "Okre¶l nazwê pierwszego urz±dzenia RS232"},
/* ru */ {IDCLS_SPECIFY_RS232_1_NAME_RU, "Specify name of first RS232 device"},
/* sv */ {IDCLS_SPECIFY_RS232_1_NAME_SV, "Ange namn på första RS232-enheten"},
/* tr */ {IDCLS_SPECIFY_RS232_1_NAME_TR, "Ýlk RS232 aygýtýnýn ismini belirt"},
#endif

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_2_NAME,    N_("Specify name of second RS232 device")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RS232_2_NAME_DA, "Angiv navn på anden RS232-enhed"},
/* de */ {IDCLS_SPECIFY_RS232_2_NAME_DE, "Name für zweites RS232 Gerät wählen"},
/* es */ {IDCLS_SPECIFY_RS232_2_NAME_ES, "Especificar el nombre del segundo periférico RS232"},
/* fr */ {IDCLS_SPECIFY_RS232_2_NAME_FR, "Spécifier le nom du 2e périphérique RS232"},
/* hu */ {IDCLS_SPECIFY_RS232_2_NAME_HU, "Adja meg a második RS232-es eszköz nevét"},
/* it */ {IDCLS_SPECIFY_RS232_2_NAME_IT, "Specifica il nome della seconda RS232"},
/* ko */ {IDCLS_SPECIFY_RS232_2_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RS232_2_NAME_NL, "Geef de naam van het tweede RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_2_NAME_PL, "Okre¶l nazwê drugiego urz±dzenia RS232"},
/* ru */ {IDCLS_SPECIFY_RS232_2_NAME_RU, "Specify name of second RS232 device"},
/* sv */ {IDCLS_SPECIFY_RS232_2_NAME_SV, "Ange namn på andra RS232-enheten"},
/* tr */ {IDCLS_SPECIFY_RS232_2_NAME_TR, "Ýkinci RS232 aygýtýnýn ismini belirt"},
#endif

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_3_NAME,    N_("Specify name of third RS232 device")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RS232_3_NAME_DA, "Angiv navn på tredje RS232-enhed"},
/* de */ {IDCLS_SPECIFY_RS232_3_NAME_DE, "Name für drittes RS232 Gerät wählen"},
/* es */ {IDCLS_SPECIFY_RS232_3_NAME_ES, "Especificar el nombre del tercer periférico RS232"},
/* fr */ {IDCLS_SPECIFY_RS232_3_NAME_FR, "Spécifier le nom du 3e périphérique RS232"},
/* hu */ {IDCLS_SPECIFY_RS232_3_NAME_HU, "Adja meg a harmadik RS232-es eszköz nevét"},
/* it */ {IDCLS_SPECIFY_RS232_3_NAME_IT, "Specifica il nome della terza RS232"},
/* ko */ {IDCLS_SPECIFY_RS232_3_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RS232_3_NAME_NL, "Geef de naam van het derde RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_3_NAME_PL, "Okre¶l nazwê trzeciego urz±dzenia RS232"},
/* ru */ {IDCLS_SPECIFY_RS232_3_NAME_RU, "Specify name of third RS232 device"},
/* sv */ {IDCLS_SPECIFY_RS232_3_NAME_SV, "Ange namn på tredje RS232-enheten"},
/* tr */ {IDCLS_SPECIFY_RS232_3_NAME_TR, "Üçüncü RS232 aygýtýnýn ismini belirt"},
#endif

/* rs232drv/rs232drv.c */
/* en */ {IDCLS_SPECIFY_RS232_4_NAME,    N_("Specify name of fourth RS232 device")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RS232_4_NAME_DA, "Angiv navn på fjerde RS232-enhed"},
/* de */ {IDCLS_SPECIFY_RS232_4_NAME_DE, "Name für viertes RS232 Gerät wählen"},
/* es */ {IDCLS_SPECIFY_RS232_4_NAME_ES, "Especificar el nombre del cuarto periférico RS232"},
/* fr */ {IDCLS_SPECIFY_RS232_4_NAME_FR, "Spécifier le nom du 4e périphérique RS232"},
/* hu */ {IDCLS_SPECIFY_RS232_4_NAME_HU, "Adja meg a negyedik RS232-es eszköz nevét"},
/* it */ {IDCLS_SPECIFY_RS232_4_NAME_IT, "Specifica il nome della quarta RS232"},
/* ko */ {IDCLS_SPECIFY_RS232_4_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RS232_4_NAME_NL, "Geef de naam van het vierde RS232 apparaat"},
/* pl */ {IDCLS_SPECIFY_RS232_4_NAME_PL, "Okre¶l nazwê czwartego urz±dzenia RS232"},
/* ru */ {IDCLS_SPECIFY_RS232_4_NAME_RU, "Specify name of fourth RS232 device"},
/* sv */ {IDCLS_SPECIFY_RS232_4_NAME_SV, "Ange namn på fjärde RS232-enheten"},
/* tr */ {IDCLS_SPECIFY_RS232_4_NAME_TR, "Dördüncü RS232 aygýtýnýn ismini belirt"},
#endif
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_ENABLE_RS232_USERPORT,    N_("Enable RS232 userport emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_RS232_USERPORT_DA, "Aktivér RS232-emulering på brugerporten"},
/* de */ {IDCLS_ENABLE_RS232_USERPORT_DE, "RS232 Userport Emulation aktivieren"},
/* es */ {IDCLS_ENABLE_RS232_USERPORT_ES, "Permitir Emulación puerto RS232"},
/* fr */ {IDCLS_ENABLE_RS232_USERPORT_FR, "Activer l'émulation de périphérique RS232 userport"},
/* hu */ {IDCLS_ENABLE_RS232_USERPORT_HU, "RS232 userport emuláció engedélyezése"},
/* it */ {IDCLS_ENABLE_RS232_USERPORT_IT, "Attiva l'emulazione della RS232 su userport"},
/* ko */ {IDCLS_ENABLE_RS232_USERPORT_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_RS232_USERPORT_NL, "RS232 userportemulatie inschakelen"},
/* pl */ {IDCLS_ENABLE_RS232_USERPORT_PL, "W³±cz emulacjê userportu RS232"},
/* ru */ {IDCLS_ENABLE_RS232_USERPORT_RU, "Enable RS232 userport emulation"},
/* sv */ {IDCLS_ENABLE_RS232_USERPORT_SV, "Aktivera RS232-emulering på användarporten"},
/* tr */ {IDCLS_ENABLE_RS232_USERPORT_TR, "RS232 userport emülasyonunu aktif et"},
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_DISABLE_RS232_USERPORT,    N_("Disable RS232 userport emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_RS232_USERPORT_DA, "Deaktivér RS232-emulering på brugerporten"},
/* de */ {IDCLS_DISABLE_RS232_USERPORT_DE, "RS232 Userport Emulation deaktivieren"},
/* es */ {IDCLS_DISABLE_RS232_USERPORT_ES, "Deshabilitar Emulación puerto RS232"},
/* fr */ {IDCLS_DISABLE_RS232_USERPORT_FR, "Désactiver l'émulation de périphérique RS232 userport"},
/* hu */ {IDCLS_DISABLE_RS232_USERPORT_HU, "RS232 userport emuláció tiltása"},
/* it */ {IDCLS_DISABLE_RS232_USERPORT_IT, "Disattiva l'emulazione della RS232 su userport"},
/* ko */ {IDCLS_DISABLE_RS232_USERPORT_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_RS232_USERPORT_NL, "RS232 userportemulatie uitschakelen"},
/* pl */ {IDCLS_DISABLE_RS232_USERPORT_PL, "Wy³±cz emulacjê userportu RS232"},
/* ru */ {IDCLS_DISABLE_RS232_USERPORT_RU, "Disable RS232 userport emulation"},
/* sv */ {IDCLS_DISABLE_RS232_USERPORT_SV, "Inaktivera RS232-emulering på användarporten"},
/* tr */ {IDCLS_DISABLE_RS232_USERPORT_TR, "RS232 userport emülasyonunu pasifleþtir"},
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_P_BAUD,    N_("<baud>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_BAUD_DA, "<baud>"},
/* de */ {IDCLS_P_BAUD_DE, "<Baud>"},
/* es */ {IDCLS_P_BAUD_ES, "<baudios>"},
/* fr */ {IDCLS_P_BAUD_FR, "<baud>"},
/* hu */ {IDCLS_P_BAUD_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_BAUD_IT, "<baud>"},
/* ko */ {IDCLS_P_BAUD_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_BAUD_NL, "<baud>"},
/* pl */ {IDCLS_P_BAUD_PL, "<bod>"},
/* ru */ {IDCLS_P_BAUD_RU, "<baud>"},
/* sv */ {IDCLS_P_BAUD_SV, "<baud>"},
/* tr */ {IDCLS_P_BAUD_TR, "<baud>"},
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_SET_BAUD_RS232_USERPORT,    N_("Set the baud rate of the RS232 userport emulation.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_BAUD_RS232_USERPORT_DA, "Angiv baudhastighet for RS232-emuleringen på brugerporten."},
/* de */ {IDCLS_SET_BAUD_RS232_USERPORT_DE, "Baudrate für RS232 Userport Emulation setzen"},
/* es */ {IDCLS_SET_BAUD_RS232_USERPORT_ES, "Seleccionar velocidad (baudios) para Emulación puerto RS232"},
/* fr */ {IDCLS_SET_BAUD_RS232_USERPORT_FR, "Spécifiez la vitesse en baud du périphérique RS232 userport"},
/* hu */ {IDCLS_SET_BAUD_RS232_USERPORT_HU, "Bitráta megadása az RS232 userport emulációhoz."},
/* it */ {IDCLS_SET_BAUD_RS232_USERPORT_IT, "Imposta la velocità della RS232 su userport (in baud)"},
/* ko */ {IDCLS_SET_BAUD_RS232_USERPORT_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_BAUD_RS232_USERPORT_NL, "Stel de baud rate in van de RS232 userport emulatie."},
/* pl */ {IDCLS_SET_BAUD_RS232_USERPORT_PL, "Okre¶l prêdko¶æ transmisji emulacji userportu RS232."},
/* ru */ {IDCLS_SET_BAUD_RS232_USERPORT_RU, "Set the baud rate of the RS232 userport emulation."},
/* sv */ {IDCLS_SET_BAUD_RS232_USERPORT_SV, "Ange baudhastighet för RS232-emuleringen på användarporten."},
/* tr */ {IDCLS_SET_BAUD_RS232_USERPORT_TR, "RS232 userport emülasyonunun baud rate'ini ayarla."},
#endif

/* rs232drv/rsuser.c */
/* en */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT,    N_("Specify VICE RS232 device for userport")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_DA, "Angiv VICE RS232-enhed for brugerporten"},
/* de */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_DE, "VICE RS232 Gerät für Userport wählen"},
/* es */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_ES, "Especificar periférico VICE RS232 para puerto usuario"},
/* fr */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_FR, "Spécifier un périphérique VICE RS232 pour le userport"},
/* hu */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_HU, "VICE RS232-es userport eszköz megadása"},
/* it */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_IT, "Specifica il dispositivo RS232 di VICE per userport"},
/* ko */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_NL, "Geef VICE RS232 apparaat voor userport"},
/* pl */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_PL, "Okre¶l urz±dzenie VICE RS232 w userporcie"},
/* ru */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_RU, "Specify VICE RS232 device for userport"},
/* sv */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_SV, "Ange VICE RS232-enhet för användarporten"},
/* tr */ {IDCLS_SPECIFY_RS232_DEVICE_USERPORT_TR, "Userport için VICE RS232 aygýtýný belirt"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_4,    N_("Enable IEC device emulation for device #4")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_4_DA, "Aktivér IEC-enhedsemulering for enhed #4"},
/* de */ {IDCLS_ENABLE_IEC_4_DE, "IEC Geräte Emulation für Gerät #4 aktivieren"},
/* es */ {IDCLS_ENABLE_IEC_4_ES, "Permitir emulación IEC para periférico #4 "},
/* fr */ {IDCLS_ENABLE_IEC_4_FR, "Activer l'émulation IEC pour le périphérique #4"},
/* hu */ {IDCLS_ENABLE_IEC_4_HU, "IEC eszköz #4 emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_IEC_4_IT, "Attiva emulazione IEC per la periferica #4"},
/* ko */ {IDCLS_ENABLE_IEC_4_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_4_NL, "IEC apparaat emulatie voor apparaat #4 inschakelen"},
/* pl */ {IDCLS_ENABLE_IEC_4_PL, "W³±cz emulacjê IEC urz±dzenia #4"},
/* ru */ {IDCLS_ENABLE_IEC_4_RU, "Enable IEC device emulation for device #4"},
/* sv */ {IDCLS_ENABLE_IEC_4_SV, "Aktivera IEC-enhetsemulering för enhet 4"},
/* tr */ {IDCLS_ENABLE_IEC_4_TR, "Aygýt #4 için IEC aygýt emülasyonunu aktif et"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_4,    N_("Disable IEC device emulation for device #4")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_4_DA, "Deaktivér IEC-enhedsemulering for enhed #4"},
/* de */ {IDCLS_DISABLE_IEC_4_DE, "IEC Geräte Emulation für Gerät #4 deaktivieren"},
/* es */ {IDCLS_DISABLE_IEC_4_ES, "Deshabilitar emulación IEC para periférico #4 "},
/* fr */ {IDCLS_DISABLE_IEC_4_FR, "Désactiver l'émulation IEC pour le périphérique #4"},
/* hu */ {IDCLS_DISABLE_IEC_4_HU, "IEC eszköz #4 emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_IEC_4_IT, "Disattiva emulazione IEC per la periferica #4"},
/* ko */ {IDCLS_DISABLE_IEC_4_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_4_NL, "IEC apparaat emulatie voor apparaat #4 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_4_PL, "Wy³±cz emulacjê urz±dzenia IEC dla urz±dzenia #4"},
/* ru */ {IDCLS_DISABLE_IEC_4_RU, "Disable IEC device emulation for device #4"},
/* sv */ {IDCLS_DISABLE_IEC_4_SV, "Inaktivera IEC-enhetsemulering för enhet 4"},
/* tr */ {IDCLS_DISABLE_IEC_4_TR, "Aygýt #4 için IEC aygýt emülasyonunu pasifleþtir"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_5,    N_("Enable IEC device emulation for device #5")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_5_DA, "Aktivér IEC-enhedsemulering for enhed #5"},
/* de */ {IDCLS_ENABLE_IEC_5_DE, "IEC Geräte Emulation für Gerät #5 aktivieren"},
/* es */ {IDCLS_ENABLE_IEC_5_ES, "Permitir emulación IEC para periférico #5"},
/* fr */ {IDCLS_ENABLE_IEC_5_FR, "Activer l'émulation IEC pour le périphérique #5"},
/* hu */ {IDCLS_ENABLE_IEC_5_HU, "IEC eszköz #5 emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_IEC_5_IT, "Attiva emulazione IEC per la periferica #5"},
/* ko */ {IDCLS_ENABLE_IEC_5_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_5_NL, "IEC apparaat emulatie voor apparaat #5 inschakelen"},
/* pl */ {IDCLS_ENABLE_IEC_5_PL, "W³±cz emulacjê IEC urz±dzenia #5"},
/* ru */ {IDCLS_ENABLE_IEC_5_RU, "Enable IEC device emulation for device #5"},
/* sv */ {IDCLS_ENABLE_IEC_5_SV, "Aktivera IEC-enhetsemulering för enhet 5"},
/* tr */ {IDCLS_ENABLE_IEC_5_TR, "Aygýt #5 için IEC aygýt emülasyonunu aktif et"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_5,    N_("Disable IEC device emulation for device #5")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_5_DA, "Deaktivér IEC-enhedsemulering for enhed #5"},
/* de */ {IDCLS_DISABLE_IEC_5_DE, "IEC Geräte Emulation für Gerät #5 deaktivieren"},
/* es */ {IDCLS_DISABLE_IEC_5_ES, "Deshabilitar emulación IEC para periférico #5"},
/* fr */ {IDCLS_DISABLE_IEC_5_FR, "Désactiver l'émulation IEC pour le périphérique #5"},
/* hu */ {IDCLS_DISABLE_IEC_5_HU, "IEC eszköz #5 emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_IEC_5_IT, "Disattiva emulazione IEC per la periferica #5"},
/* ko */ {IDCLS_DISABLE_IEC_5_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_5_NL, "IEC apparaat emulatie voor apparaat #5 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_5_PL, "Wy³±cz emulacjê urz±dzenia IEC dla urz±dzenia #5"},
/* ru */ {IDCLS_DISABLE_IEC_5_RU, "Disable IEC device emulation for device #5"},
/* sv */ {IDCLS_DISABLE_IEC_5_SV, "Inaktivera IEC-enhetsemulering för enhet 5"},
/* tr */ {IDCLS_DISABLE_IEC_5_TR, "Aygýt #5 için IEC aygýt emülasyonunu pasifleþtir"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_6,    N_("Enable IEC device emulation for device #6")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_6_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_IEC_6_DE, "IEC Geräte Emulation für Gerät #6 aktivieren"},
/* es */ {IDCLS_ENABLE_IEC_6_ES, "Permitir emulación IEC para periférico #6"},
/* fr */ {IDCLS_ENABLE_IEC_6_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_IEC_6_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_IEC_6_IT, "Attiva emulazione IEC per la periferica #6"},
/* ko */ {IDCLS_ENABLE_IEC_6_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_6_NL, "IEC apparaat emulatie voor apparaat #6 inschakelen"},
/* pl */ {IDCLS_ENABLE_IEC_6_PL, "W³±cz emulacjê IEC urz±dzenia #6"},
/* ru */ {IDCLS_ENABLE_IEC_6_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_6_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_IEC_6_TR, ""},  /* fuzzy */
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_6,    N_("Disable IEC device emulation for device #6")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_6_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_IEC_6_DE, "IEC Geräte Emulation für Gerät #6 deaktivieren"},
/* es */ {IDCLS_DISABLE_IEC_6_ES, "Deshabilitar emulación IEC para periférico #6"},
/* fr */ {IDCLS_DISABLE_IEC_6_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_IEC_6_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_IEC_6_IT, "Disattiva emulazione IEC per la periferica #6"},
/* ko */ {IDCLS_DISABLE_IEC_6_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_6_NL, "IEC apparaat emulatie voor apparaat #6 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_6_PL, "Wy³±cz emulacjê urz±dzenia IEC dla urz±dzenia #6"},
/* ru */ {IDCLS_DISABLE_IEC_6_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_6_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_IEC_6_TR, ""},  /* fuzzy */
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_7,    N_("Enable IEC device emulation for device #7")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_7_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_IEC_7_DE, "IEC Geräte Emulation für Gerät #7 aktivieren"},
/* es */ {IDCLS_ENABLE_IEC_7_ES, "Permitir emulación IEC para periférico #7"},
/* fr */ {IDCLS_ENABLE_IEC_7_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_IEC_7_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_IEC_7_IT, "Attiva emulazione IEC per la periferica #7"},
/* ko */ {IDCLS_ENABLE_IEC_7_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_7_NL, "IEC apparaat emulatie voor apparaat #7 inschakelen"},
/* pl */ {IDCLS_ENABLE_IEC_7_PL, "W³±cz emulacjê IEC urz±dzenia #7"},
/* ru */ {IDCLS_ENABLE_IEC_7_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IEC_7_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_IEC_7_TR, ""},  /* fuzzy */
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_7,    N_("Disable IEC device emulation for device #7")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_7_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_IEC_7_DE, "IEC Geräte Emulation für Gerät #7 deaktivieren"},
/* es */ {IDCLS_DISABLE_IEC_7_ES, "Deshabilitar emulación IEC para periférico #7"},
/* fr */ {IDCLS_DISABLE_IEC_7_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_IEC_7_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_IEC_7_IT, "Disattiva emulazione IEC per la periferica #7"},
/* ko */ {IDCLS_DISABLE_IEC_7_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_7_NL, "IEC apparaat emulatie voor apparaat #7 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_7_PL, "Wy³±cz emulacjê urz±dzenia IEC dla urz±dzenia #7"},
/* ru */ {IDCLS_DISABLE_IEC_7_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IEC_7_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_IEC_7_TR, ""},  /* fuzzy */
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_8,    N_("Enable IEC device emulation for device #8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_8_DA, "Aktivér IEC-enhedsemulering for enhed #8"},
/* de */ {IDCLS_ENABLE_IEC_8_DE, "IEC Geräte Emulation für Gerät #8 aktivieren"},
/* es */ {IDCLS_ENABLE_IEC_8_ES, "Permitir emulación IEC para periférico #8"},
/* fr */ {IDCLS_ENABLE_IEC_8_FR, "Activer l'émulation IEC pour le lecteur #8"},
/* hu */ {IDCLS_ENABLE_IEC_8_HU, "IEC eszköz #8 emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_IEC_8_IT, "Attiva emulazione IEC per la periferica #8"},
/* ko */ {IDCLS_ENABLE_IEC_8_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_8_NL, "IEC apparaat emulatie voor apparaat #8 inschakelen"},
/* pl */ {IDCLS_ENABLE_IEC_8_PL, "W³±cz emulacjê IEC urz±dzenia #8"},
/* ru */ {IDCLS_ENABLE_IEC_8_RU, "Enable IEC device emulation for device #8"},
/* sv */ {IDCLS_ENABLE_IEC_8_SV, "Aktivera IEC-enhetsemulering för enhet 8"},
/* tr */ {IDCLS_ENABLE_IEC_8_TR, "Aygýt #8 için IEC aygýt emülasyonunu aktif et"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_8,    N_("Disable IEC device emulation for device #8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_8_DA, "Deaktivér IEC-enhedsemulering for enhed #8"},
/* de */ {IDCLS_DISABLE_IEC_8_DE, "IEC Geräte Emulation für Gerät #8 deaktivieren"},
/* es */ {IDCLS_DISABLE_IEC_8_ES, "Deshabilitar emulación IEC para periférico #8"},
/* fr */ {IDCLS_DISABLE_IEC_8_FR, "Désactiver l'émulation IEC pour le lecteur #8"},
/* hu */ {IDCLS_DISABLE_IEC_8_HU, "IEC eszköz #8 emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_IEC_8_IT, "Disattiva emulazione IEC per la periferica #8"},
/* ko */ {IDCLS_DISABLE_IEC_8_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_8_NL, "IEC apparaat emulatie voor apparaat #8 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_8_PL, "Wy³±cz emulacjê urz±dzenia IEC dla urz±dzenia #8"},
/* ru */ {IDCLS_DISABLE_IEC_8_RU, "Disable IEC device emulation for device #8"},
/* sv */ {IDCLS_DISABLE_IEC_8_SV, "Inaktivera IEC-enhetsemulering för enhet 8"},
/* tr */ {IDCLS_DISABLE_IEC_8_TR, "Aygýt #8 için IEC aygýt emülasyonunu pasifleþtir"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_9,    N_("Enable IEC device emulation for device #9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_9_DA, "Aktivér IEC-enhedsemulering for enhed #9"},
/* de */ {IDCLS_ENABLE_IEC_9_DE, "IEC Geräte Emulation für Gerät #9 aktivieren"},
/* es */ {IDCLS_ENABLE_IEC_9_ES, "Permitir emulación IEC para periférico #9"},
/* fr */ {IDCLS_ENABLE_IEC_9_FR, "Activer l'émulation IEC pour le lecteur #9"},
/* hu */ {IDCLS_ENABLE_IEC_9_HU, "IEC eszköz #9 emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_IEC_9_IT, "Attiva emulazione IEC per la periferica #9"},
/* ko */ {IDCLS_ENABLE_IEC_9_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_9_NL, "IEC apparaat emulatie voor apparaat #9 inschakelen"},
/* pl */ {IDCLS_ENABLE_IEC_9_PL, "W³±cz emulacjê IEC urz±dzenia #9"},
/* ru */ {IDCLS_ENABLE_IEC_9_RU, "Enable IEC device emulation for device #9"},
/* sv */ {IDCLS_ENABLE_IEC_9_SV, "Aktivera IEC-enhetsemulering för enhet 9"},
/* tr */ {IDCLS_ENABLE_IEC_9_TR, "Aygýt #9 için IEC aygýt emülasyonunu aktif et"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_9,    N_("Disable IEC device emulation for device #9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_9_DA, "Deaktivér IEC-enhedsemulering for enhed #9"},
/* de */ {IDCLS_DISABLE_IEC_9_DE, "IEC Geräte Emulation für Gerät #9 deaktivieren"},
/* es */ {IDCLS_DISABLE_IEC_9_ES, "Deshabilitar emulación IEC para periférico #9"},
/* fr */ {IDCLS_DISABLE_IEC_9_FR, "Désactiver l'émulation IEC pour le lecteur #9"},
/* hu */ {IDCLS_DISABLE_IEC_9_HU, "IEC eszköz #9 emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_IEC_9_IT, "Disattiva emulazione IEC per la periferica #9"},
/* ko */ {IDCLS_DISABLE_IEC_9_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_9_NL, "IEC apparaat emulatie voor apparaat #9 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_9_PL, "Wy³±cz emulacjê urz±dzenia IEC dla urz±dzenia #9"},
/* ru */ {IDCLS_DISABLE_IEC_9_RU, "Disable IEC device emulation for device #9"},
/* sv */ {IDCLS_DISABLE_IEC_9_SV, "Inaktivera IEC-enhetsemulering för enhet 9"},
/* tr */ {IDCLS_DISABLE_IEC_9_TR, "Aygýt #9 için IEC aygýt emülasyonunu pasifleþtir"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_10,    N_("Enable IEC device emulation for device #10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_10_DA, "Aktivér IEC-enhedsemulering for enhed #10"},
/* de */ {IDCLS_ENABLE_IEC_10_DE, "IEC Geräte Emulation für Gerät #10 aktivieren"},
/* es */ {IDCLS_ENABLE_IEC_10_ES, "Permitir emulación IEC para periférico #10"},
/* fr */ {IDCLS_ENABLE_IEC_10_FR, "Activer l'émulation IEC pour le lecteur #10"},
/* hu */ {IDCLS_ENABLE_IEC_10_HU, "IEC eszköz #10 emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_IEC_10_IT, "Attiva emulazione IEC per la periferica #10"},
/* ko */ {IDCLS_ENABLE_IEC_10_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_10_NL, "IEC apparaat emulatie voor apparaat #10 inschakelen"},
/* pl */ {IDCLS_ENABLE_IEC_10_PL, "W³±cz emulacjê IEC urz±dzenia #10"},
/* ru */ {IDCLS_ENABLE_IEC_10_RU, "Enable IEC device emulation for device #10"},
/* sv */ {IDCLS_ENABLE_IEC_10_SV, "Aktivera IEC-enhetsemulering för enhet 10"},
/* tr */ {IDCLS_ENABLE_IEC_10_TR, "Aygýt #10 için IEC aygýt emülasyonunu aktif et"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_10,    N_("Disable IEC device emulation for device #10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_10_DA, "Deaktivér IEC-enhedsemulering for enhed #10"},
/* de */ {IDCLS_DISABLE_IEC_10_DE, "IEC Geräte Emulation für Gerät #10 deaktivieren"},
/* es */ {IDCLS_DISABLE_IEC_10_ES, "Deshabilitar emulación IEC para periférico #10"},
/* fr */ {IDCLS_DISABLE_IEC_10_FR, "Désactiver l'émulation IEC pour le lecteur #10"},
/* hu */ {IDCLS_DISABLE_IEC_10_HU, "IEC eszköz #10 emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_IEC_10_IT, "Disattiva emulazione IEC per la periferica #10"},
/* ko */ {IDCLS_DISABLE_IEC_10_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_10_NL, "IEC apparaat emulatie voor apparaat #10 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_10_PL, "Wy³±cz emulacjê urz±dzenia IEC dla urz±dzenia #10"},
/* ru */ {IDCLS_DISABLE_IEC_10_RU, "Disable IEC device emulation for device #10"},
/* sv */ {IDCLS_DISABLE_IEC_10_SV, "Inaktivera IEC-enhetsemulering för enhet 10"},
/* tr */ {IDCLS_DISABLE_IEC_10_TR, "Aygýt #10 için IEC aygýt emülasyonunu pasifleþtir"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_ENABLE_IEC_11,    N_("Enable IEC device emulation for device #11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IEC_11_DA, "Aktivér IEC-enhedsemulering for enhed #11"},
/* de */ {IDCLS_ENABLE_IEC_11_DE, "IEC Geräte Emulation für Gerät #11 aktivieren"},
/* es */ {IDCLS_ENABLE_IEC_11_ES, "Permitir emulación IEC para periférico #11"},
/* fr */ {IDCLS_ENABLE_IEC_11_FR, "Activer l'émulation IEC pour le lecteur #11"},
/* hu */ {IDCLS_ENABLE_IEC_11_HU, "IEC eszköz #11 emulációjának engedélyezése"},
/* it */ {IDCLS_ENABLE_IEC_11_IT, "Attiva emulazione IEC per la periferica #11"},
/* ko */ {IDCLS_ENABLE_IEC_11_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IEC_11_NL, "IEC apparaat emulatie voor apparaat #11 inschakelen"},
/* pl */ {IDCLS_ENABLE_IEC_11_PL, "W³±cz emulacjê IEC urz±dzenia #11"},
/* ru */ {IDCLS_ENABLE_IEC_11_RU, "Enable IEC device emulation for device #11"},
/* sv */ {IDCLS_ENABLE_IEC_11_SV, "Aktivera IEC-enhetsemulering för enhet 11"},
/* tr */ {IDCLS_ENABLE_IEC_11_TR, "Aygýt #11 için IEC aygýt emülasyonunu aktif et"},
#endif

/* serial/serial-iec-device.c */
/* en */ {IDCLS_DISABLE_IEC_11,    N_("Disable IEC device emulation for device #11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IEC_11_DA, "Deaktivér IEC-enhedsemulering for enhed #11"},
/* de */ {IDCLS_DISABLE_IEC_11_DE, "IEC Geräte Emulation für Gerät #11 deaktivieren"},
/* es */ {IDCLS_DISABLE_IEC_11_ES, "Deshabilitar emulación IEC para periférico #11"},
/* fr */ {IDCLS_DISABLE_IEC_11_FR, "Désactiver l'émulation IEC pour le lecteur #11"},
/* hu */ {IDCLS_DISABLE_IEC_11_HU, "IEC eszköz #11 emulációjának tiltása"},
/* it */ {IDCLS_DISABLE_IEC_11_IT, "Disattiva emulazione IEC per la periferica #11"},
/* ko */ {IDCLS_DISABLE_IEC_11_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IEC_11_NL, "IEC apparaat emulatie voor apparaat #11 uitschakelen"},
/* pl */ {IDCLS_DISABLE_IEC_11_PL, "Wy³±cz emulacjê urz±dzenia IEC dla urz±dzenia #11"},
/* ru */ {IDCLS_DISABLE_IEC_11_RU, "Disable IEC device emulation for device #11"},
/* sv */ {IDCLS_DISABLE_IEC_11_SV, "Inaktivera IEC-enhetsemulering för enhet 11"},
/* tr */ {IDCLS_DISABLE_IEC_11_TR, "Aygýt #11 için IEC aygýt emülasyonunu pasifleþtir"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SIDCART,    N_("Enable the SID cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SIDCART_DA, "Aktivér SID-cartridget"},
/* de */ {IDCLS_ENABLE_SIDCART_DE, "SID Modul aktivieren"},
/* es */ {IDCLS_ENABLE_SIDCART_ES, "Permitir cartucho SID"},
/* fr */ {IDCLS_ENABLE_SIDCART_FR, "Activer la cartouche SID"},
/* hu */ {IDCLS_ENABLE_SIDCART_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SIDCART_IT, "Attiva cartuccia SID"},
/* ko */ {IDCLS_ENABLE_SIDCART_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SIDCART_NL, "SID cartridge inschakelen"},
/* pl */ {IDCLS_ENABLE_SIDCART_PL, "W³±cz kartrid¿ SID"},
/* ru */ {IDCLS_ENABLE_SIDCART_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SIDCART_SV, "Aktivera SID-insticksmodul"},
/* tr */ {IDCLS_ENABLE_SIDCART_TR, "SID kartuþunu aktif et"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SIDCART,    N_("Disable the SID cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SIDCART_DA, "Deaktivér SID-cartridge"},
/* de */ {IDCLS_DISABLE_SIDCART_DE, "SID Modul deaktivieren"},
/* es */ {IDCLS_DISABLE_SIDCART_ES, "Deshabilitar cartucho SID"},
/* fr */ {IDCLS_DISABLE_SIDCART_FR, "Désactiver la cartouche SID"},
/* hu */ {IDCLS_DISABLE_SIDCART_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_SIDCART_IT, "Disattiva la cartuccia SID"},
/* ko */ {IDCLS_DISABLE_SIDCART_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SIDCART_NL, "SID cartridge uitschakelen"},
/* pl */ {IDCLS_DISABLE_SIDCART_PL, "Wy³±cz kartrid¿ SID"},
/* ru */ {IDCLS_DISABLE_SIDCART_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SIDCART_SV, "Inaktivera SID-insticksmodul"},
/* tr */ {IDCLS_DISABLE_SIDCART_TR, "SID kartuþunu pasifleþtir"},
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_PET_SIDCART_ADDRESS,    N_("SID cartridge address (0x8F00/0xE900)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET_SIDCART_ADDRESS_DA, ""},  /* fuzzy */
/* de */ {IDCLS_PET_SIDCART_ADDRESS_DE, "SID Modul Adresse (0x8F00/0xE900)"},
/* es */ {IDCLS_PET_SIDCART_ADDRESS_ES, "Dirección del cartucho SID (0x8F00/0xE900)"},
/* fr */ {IDCLS_PET_SIDCART_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_PET_SIDCART_ADDRESS_HU, ""},  /* fuzzy */
/* it */ {IDCLS_PET_SIDCART_ADDRESS_IT, "Indirizzo cartuccia SID (0x8F00/0xE900)"},
/* ko */ {IDCLS_PET_SIDCART_ADDRESS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PET_SIDCART_ADDRESS_NL, "SID cartridge adres (0x8F00/0xE900)"},
/* pl */ {IDCLS_PET_SIDCART_ADDRESS_PL, "Adres kartrid¿a SID (0x8F00/0xE900)"},
/* ru */ {IDCLS_PET_SIDCART_ADDRESS_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_PET_SIDCART_ADDRESS_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_PET_SIDCART_ADDRESS_TR, ""},  /* fuzzy */
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_PLUS4_SIDCART_ADDRESS,    N_("SID cartridge address (0xFD40/0xFE80)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PLUS4_SIDCART_ADDRESS_DA, ""},  /* fuzzy */
/* de */ {IDCLS_PLUS4_SIDCART_ADDRESS_DE, "SID Modul Adresse (0xFD40/0xFE80)"},
/* es */ {IDCLS_PLUS4_SIDCART_ADDRESS_ES, "Dirección del cartucho SID (0xFD40/0xFE80)"},
/* fr */ {IDCLS_PLUS4_SIDCART_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_PLUS4_SIDCART_ADDRESS_HU, ""},  /* fuzzy */
/* it */ {IDCLS_PLUS4_SIDCART_ADDRESS_IT, "Indirizzo cartuccia SID (0xFD40/0xFE80)"},
/* ko */ {IDCLS_PLUS4_SIDCART_ADDRESS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PLUS4_SIDCART_ADDRESS_NL, "SID cartridge adres (0xFD40/0xFE80)"},
/* pl */ {IDCLS_PLUS4_SIDCART_ADDRESS_PL, "Adres kartrid¿a SID (0xFD40/0xFE80)"},
/* ru */ {IDCLS_PLUS4_SIDCART_ADDRESS_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_PLUS4_SIDCART_ADDRESS_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_PLUS4_SIDCART_ADDRESS_TR, ""},  /* fuzzy */
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_VIC20_SIDCART_ADDRESS,    N_("SID cartridge address (0x9800/0x9C00)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_VIC20_SIDCART_ADDRESS_DA, ""},  /* fuzzy */
/* de */ {IDCLS_VIC20_SIDCART_ADDRESS_DE, "SID Module Adresse (0x9800/0x9C00)"},
/* es */ {IDCLS_VIC20_SIDCART_ADDRESS_ES, "Dirección del cartucho SID (0x9800/0x9C00)"},
/* fr */ {IDCLS_VIC20_SIDCART_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_VIC20_SIDCART_ADDRESS_HU, ""},  /* fuzzy */
/* it */ {IDCLS_VIC20_SIDCART_ADDRESS_IT, "Indirizzo cartuccia SID (0x9800/0x9C00)"},
/* ko */ {IDCLS_VIC20_SIDCART_ADDRESS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_VIC20_SIDCART_ADDRESS_NL, "SID cartridge adres (0x9800/0x9C00)"},
/* pl */ {IDCLS_VIC20_SIDCART_ADDRESS_PL, "Adres kartrid¿a SID (0x9800/0x9C00)"},
/* ru */ {IDCLS_VIC20_SIDCART_ADDRESS_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_VIC20_SIDCART_ADDRESS_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_VIC20_SIDCART_ADDRESS_TR, ""},  /* fuzzy */
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_PET_SIDCART_CLOCK,    N_("SID cartridge clock (0: C64 clock, 1: PET clock)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PET_SIDCART_CLOCK_DA, ""},  /* fuzzy */
/* de */ {IDCLS_PET_SIDCART_CLOCK_DE, "SID Module Takt (0: C64 Takt, 1: PET Takt)"},
/* es */ {IDCLS_PET_SIDCART_CLOCK_ES, "Reloj del cartucho SID (0: Reloj C64, 1: Reloj PET)"},
/* fr */ {IDCLS_PET_SIDCART_CLOCK_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_PET_SIDCART_CLOCK_HU, ""},  /* fuzzy */
/* it */ {IDCLS_PET_SIDCART_CLOCK_IT, "Clock cartuccia SID (0: clock C64 , 1: clock PET)"},
/* ko */ {IDCLS_PET_SIDCART_CLOCK_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PET_SIDCART_CLOCK_NL, "SID cartridge klok (0: C64 klok, 1: PET klok)"},
/* pl */ {IDCLS_PET_SIDCART_CLOCK_PL, "Zegar kartrid¿a SID (0: Zegar C64, 1: Zegar PET)"},
/* ru */ {IDCLS_PET_SIDCART_CLOCK_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_PET_SIDCART_CLOCK_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_PET_SIDCART_CLOCK_TR, ""},  /* fuzzy */
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_PLUS4_SIDCART_CLOCK,    N_("SID cartridge clock (0: C64 clock, 1: PLUS4 clock)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PLUS4_SIDCART_CLOCK_DA, ""},  /* fuzzy */
/* de */ {IDCLS_PLUS4_SIDCART_CLOCK_DE, "SID Modul Takt (0: C64 Takt, 1: PLUS4 Takt)"},
/* es */ {IDCLS_PLUS4_SIDCART_CLOCK_ES, "Reloj del cartucho SID (0: Reloj C64, 1: Reloj PLUS4)"},
/* fr */ {IDCLS_PLUS4_SIDCART_CLOCK_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_PLUS4_SIDCART_CLOCK_HU, ""},  /* fuzzy */
/* it */ {IDCLS_PLUS4_SIDCART_CLOCK_IT, "Clock cartuccia SID (0: clock C64 , 1: clock PLUS4)"},
/* ko */ {IDCLS_PLUS4_SIDCART_CLOCK_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PLUS4_SIDCART_CLOCK_NL, "SID cartridge klok (0: C64 klok, 1: PLUS4 klok)"},
/* pl */ {IDCLS_PLUS4_SIDCART_CLOCK_PL, "Zegar kartrid¿a SID (0: Zegar C64, 1: Zegar PLUS4)"},
/* ru */ {IDCLS_PLUS4_SIDCART_CLOCK_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_PLUS4_SIDCART_CLOCK_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_PLUS4_SIDCART_CLOCK_TR, ""},  /* fuzzy */
#endif

/* sid/sid-cmdline-options.c */
/* en */ {IDCLS_VIC20_SIDCART_CLOCK,    N_("SID cartridge clock (0: C64 clock, 1: VIC20 clock)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_VIC20_SIDCART_CLOCK_DA, ""},  /* fuzzy */
/* de */ {IDCLS_VIC20_SIDCART_CLOCK_DE, "SID Modul Takt (0: C64 Takt, 1: VIC20 Takt)"},
/* es */ {IDCLS_VIC20_SIDCART_CLOCK_ES, "Reloj del cartucho SID (0: Reloj C64, 1: Reloj VIC20)"},
/* fr */ {IDCLS_VIC20_SIDCART_CLOCK_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_VIC20_SIDCART_CLOCK_HU, ""},  /* fuzzy */
/* it */ {IDCLS_VIC20_SIDCART_CLOCK_IT, "Clock cartuccia SID (0: clock C64 , 1: clock VIC20)"},
/* ko */ {IDCLS_VIC20_SIDCART_CLOCK_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_VIC20_SIDCART_CLOCK_NL, "SID cartridge klok (0: C64 klok, 1: VIC20 klok)"},
/* pl */ {IDCLS_VIC20_SIDCART_CLOCK_PL, "Zegar kartrid¿a SID (0: Zegar C64, 1: Zegar VIC20)"},
/* ru */ {IDCLS_VIC20_SIDCART_CLOCK_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_VIC20_SIDCART_CLOCK_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_VIC20_SIDCART_CLOCK_TR, ""},  /* fuzzy */
#endif

/* vdc/vdc-cmdline-options.c, crtc/crtc-cmdline-options.c */
/* en */ {IDCLS_ENABLE_STRETCH_VERTICAL,    N_("Enable additional vertical stretching.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_STRETCH_VERTICAL_DA, "Aktivér yderligere vertikal udstrækning"},
/* de */ {IDCLS_ENABLE_STRETCH_VERTICAL_DE, "Aktiviere zusätzliche Vertikalstreckung."},
/* es */ {IDCLS_ENABLE_STRETCH_VERTICAL_ES, "Permitir extensión vertical adicional"},
/* fr */ {IDCLS_ENABLE_STRETCH_VERTICAL_FR, "Activer l'ajout d'étirement vertical."},
/* hu */ {IDCLS_ENABLE_STRETCH_VERTICAL_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_STRETCH_VERTICAL_IT, "Attiva allungamento verticale aggiuntivo."},
/* ko */ {IDCLS_ENABLE_STRETCH_VERTICAL_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_STRETCH_VERTICAL_NL, "Additionele verticale rekking inschakelen."},
/* pl */ {IDCLS_ENABLE_STRETCH_VERTICAL_PL, "W³±cz dodatkowe rozci±ganie w pionie"},
/* ru */ {IDCLS_ENABLE_STRETCH_VERTICAL_RU, "Enable additional vertical stretching."},
/* sv */ {IDCLS_ENABLE_STRETCH_VERTICAL_SV, "Aktivera ytterligare vertikal utdragning."},
/* tr */ {IDCLS_ENABLE_STRETCH_VERTICAL_TR, "Ek dikey uzatmayý aktif et."},
#endif

/* vdc/vdc-cmdline-options.c, crtc/crtc-cmdline-options.c */
/* en */ {IDCLS_DISABLE_STRETCH_VERTICAL,    N_("Disable additional vertical stretching.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_STRETCH_VERTICAL_DA, "Deaktivér yderligere vertikal udstrækning"},
/* de */ {IDCLS_DISABLE_STRETCH_VERTICAL_DE, "Deaktiviere zusätzliche Vertikalstreckung."},
/* es */ {IDCLS_DISABLE_STRETCH_VERTICAL_ES, "Inhabilitar extensión vertical adicional"},
/* fr */ {IDCLS_DISABLE_STRETCH_VERTICAL_FR, "Désactiver l'ajout d'étirement vertical."},
/* hu */ {IDCLS_DISABLE_STRETCH_VERTICAL_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_STRETCH_VERTICAL_IT, "Disattiva allungamento verticale aggiuntivo."},
/* ko */ {IDCLS_DISABLE_STRETCH_VERTICAL_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_STRETCH_VERTICAL_NL, "Additionele verticale rekking uitschakelen."},
/* pl */ {IDCLS_DISABLE_STRETCH_VERTICAL_PL, "Wy³±cz dodatkowe rozci±ganie w pionie"},
/* ru */ {IDCLS_DISABLE_STRETCH_VERTICAL_RU, "Disable additional vertical stretching."},
/* sv */ {IDCLS_DISABLE_STRETCH_VERTICAL_SV, "Inaktivera ytterligare vertikal utdragning."},
/* tr */ {IDCLS_DISABLE_STRETCH_VERTICAL_TR, "Ek dikey uzatmayý pasifleþtir."},
#endif

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_MEMORY_16KB,    N_("Set the VDC memory size to 16KB")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_VDC_MEMORY_16KB_DA, "Sæt VDC-hukommelsesstørrelse til 16KB"},
/* de */ {IDCLS_SET_VDC_MEMORY_16KB_DE, "VDC Speichgröße auf 16KB setzen"},
/* es */ {IDCLS_SET_VDC_MEMORY_16KB_ES, "Seleccionar tamaño memoria VDC en 16KB"},
/* fr */ {IDCLS_SET_VDC_MEMORY_16KB_FR, "Régler la taille de la mémoire VDC à 16KO"},
/* hu */ {IDCLS_SET_VDC_MEMORY_16KB_HU, "VDC memória méret beállítása 16KB-ra"},
/* it */ {IDCLS_SET_VDC_MEMORY_16KB_IT, "Imposta la dimensione della memoria del VDC a 16KB"},
/* ko */ {IDCLS_SET_VDC_MEMORY_16KB_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_VDC_MEMORY_16KB_NL, "Stel de VDC geheugengrootte in op 16KB"},
/* pl */ {IDCLS_SET_VDC_MEMORY_16KB_PL, "Ustaw rozmiar pamiêci VDC na 16 KB"},
/* ru */ {IDCLS_SET_VDC_MEMORY_16KB_RU, "Set the VDC memory size to 16KB"},
/* sv */ {IDCLS_SET_VDC_MEMORY_16KB_SV, "Sätt VDC-minnesstorlek till 16KB"},
/* tr */ {IDCLS_SET_VDC_MEMORY_16KB_TR, "VDC bellek boyutunu 16KB'a ayarla"},
#endif

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_MEMORY_64KB,    N_("Set the VDC memory size to 64KB")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_VDC_MEMORY_64KB_DA, "Sæt VDC-hukommelsesstørrelse til 64KB"},
/* de */ {IDCLS_SET_VDC_MEMORY_64KB_DE, "VDC Speichgröße auf 64KB setzen"},
/* es */ {IDCLS_SET_VDC_MEMORY_64KB_ES, "Seleccionar tamaño memoria VDC en 64KB"},
/* fr */ {IDCLS_SET_VDC_MEMORY_64KB_FR, "Régler la taille de la mémoire VDC à 64KO"},
/* hu */ {IDCLS_SET_VDC_MEMORY_64KB_HU, "VDC memória méret beállítása 64KB-ra"},
/* it */ {IDCLS_SET_VDC_MEMORY_64KB_IT, "Imposta la dimensione della memoria del VDC a 64KB"},
/* ko */ {IDCLS_SET_VDC_MEMORY_64KB_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_VDC_MEMORY_64KB_NL, "Stel de VDC geheugengrootte in op 64KB"},
/* pl */ {IDCLS_SET_VDC_MEMORY_64KB_PL, "Ustaw rozmiar pamiêci VDC na 64 KB"},
/* ru */ {IDCLS_SET_VDC_MEMORY_64KB_RU, "Set the VDC memory size to 64KB"},
/* sv */ {IDCLS_SET_VDC_MEMORY_64KB_SV, "Sätt VDC-minnesstorlek till 64KB"},
/* tr */ {IDCLS_SET_VDC_MEMORY_64KB_TR, "VDC bellek boyutunu 64KB'a ayarla"},
#endif

/* vdc/vdc-cmdline-options.c */
/* en */ {IDCLS_SET_VDC_REVISION,    N_("Set VDC revision (0..2)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_VDC_REVISION_DA, "Angiv VDC-revision (0..2)"},
/* de */ {IDCLS_SET_VDC_REVISION_DE, "VDC Revision setzen (0..2)"},
/* es */ {IDCLS_SET_VDC_REVISION_ES, "Poner revisión VDC (0..2)"},
/* fr */ {IDCLS_SET_VDC_REVISION_FR, "Régler la révision VDC (0..2)"},
/* hu */ {IDCLS_SET_VDC_REVISION_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_VDC_REVISION_IT, "Imposta la revisione del VDC (0..2)"},
/* ko */ {IDCLS_SET_VDC_REVISION_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_VDC_REVISION_NL, "Stel de VDC revisie in (0..2)"},
/* pl */ {IDCLS_SET_VDC_REVISION_PL, "Okre¶l wersjê VDC (0..2)"},
/* ru */ {IDCLS_SET_VDC_REVISION_RU, "Set VDC revision (0..2)"},
/* sv */ {IDCLS_SET_VDC_REVISION_SV, "Ange VDC-revision (0..2)"},
/* tr */ {IDCLS_SET_VDC_REVISION_TR, "VDC revizyonunu ayarla (0..2)"},
#endif

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_ENABLE_VIC1112_IEEE488,    N_("Enable VIC-1112 IEEE488 interface")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_VIC1112_IEEE488_DA, "Aktivér VIC-1112-IEEE488-interface"},
/* de */ {IDCLS_ENABLE_VIC1112_IEEE488_DE, "VIC-1112 IEEE488 Schnittstelle aktivieren"},
/* es */ {IDCLS_ENABLE_VIC1112_IEEE488_ES, "Permitir interface VIC-1112 IEEE488"},
/* fr */ {IDCLS_ENABLE_VIC1112_IEEE488_FR, "Activer l'interface VIC-1112 IEEE488"},
/* hu */ {IDCLS_ENABLE_VIC1112_IEEE488_HU, "VIC-1112 IEEE488 interfész engedélyezése"},
/* it */ {IDCLS_ENABLE_VIC1112_IEEE488_IT, "Attiva interfaccia IEEE488 VIC-1112"},
/* ko */ {IDCLS_ENABLE_VIC1112_IEEE488_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_VIC1112_IEEE488_NL, "VIC-1112 IEEE488 interface inschakelen"},
/* pl */ {IDCLS_ENABLE_VIC1112_IEEE488_PL, "W³±cz interfejs IEEE488 VIC-1112"},
/* ru */ {IDCLS_ENABLE_VIC1112_IEEE488_RU, "Enable VIC-1112 IEEE488 interface"},
/* sv */ {IDCLS_ENABLE_VIC1112_IEEE488_SV, "Aktivera VIC-1112-IEEE488-gränssnitt"},
/* tr */ {IDCLS_ENABLE_VIC1112_IEEE488_TR, "VIC-1112 IEEE488 arabirimini aktif et"},
#endif

/* vic20/vic20-cmdline-options.c */
/* en */ {IDCLS_DISABLE_VIC1112_IEEE488,    N_("Disable VIC-1112 IEEE488 interface")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_VIC1112_IEEE488_DA, "Deaktivér VIC-1112-IEEE488-interface"},
/* de */ {IDCLS_DISABLE_VIC1112_IEEE488_DE, "VIC-1112 IEEE488 Schnittstelle deaktivieren"},
/* es */ {IDCLS_DISABLE_VIC1112_IEEE488_ES, "Deshabilitar interface VIC-1112 IEEE488"},
/* fr */ {IDCLS_DISABLE_VIC1112_IEEE488_FR, "Désactiver l'interface VIC-1112 IEEE488"},
/* hu */ {IDCLS_DISABLE_VIC1112_IEEE488_HU, "VIC-1112 IEEE488 interfész tiltása"},
/* it */ {IDCLS_DISABLE_VIC1112_IEEE488_IT, "Disattiva interfaccia IEEE488 VIC-1112"},
/* ko */ {IDCLS_DISABLE_VIC1112_IEEE488_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_VIC1112_IEEE488_NL, "VIC-1112 IEEE488 interface uitschakelen"},
/* pl */ {IDCLS_DISABLE_VIC1112_IEEE488_PL, "Wy³±cz interfejs IEEE488 VIC-1112"},
/* ru */ {IDCLS_DISABLE_VIC1112_IEEE488_RU, "Disable VIC-1112 IEEE488 interface"},
/* sv */ {IDCLS_DISABLE_VIC1112_IEEE488_SV, "Inaktivera VIC-1112-IEEE488-gränssnitt"},
/* tr */ {IDCLS_DISABLE_VIC1112_IEEE488_TR, "VIC-1112 IEEE488 arabirimini pasifleþtir"},
#endif

/* vic20/cart/vic20cart.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME,    N_("Specify 4/8/16K extension ROM name at $2000")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_DA, "Angiv navn for 4/8/16k-udviddelses-ROM på $2000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_DE, "4/8/16K Erweiterungs ROM Dateiname für $2000 wählen"},
/* es */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_ES, "Especificar nombre ROM extendida de 4/8/16KB en $2000"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_FR, "Spécifier le nom de l'extension ROM 4/8/16K à $2000"},
/* hu */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_HU, "Adja meg a $2000 címû 4/8/16K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_IT, "Specifica il nome della ROM di estensione di 4/8/16K a $2000"},
/* ko */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_NL, "Geef de naam van het bestand voor de 4/8/16K ROM op $2000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_PL, "Okre¶l nazwê ROM rozszerzenia 4/8/16K w $2000"},
/* ru */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_RU, "Specify 4/8/16K extension ROM name at $2000"},
/* sv */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_SV, "Ange namn för 4/8/16K-utöknings-ROM på $2000"},
/* tr */ {IDCLS_SPECIFY_EXT_ROM_2000_NAME_TR, "$2000 adresindeki 4/8/16K uzantýsý ROM ismini belirt"},
#endif

/* vic20/cart/vic20cart.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME,    N_("Specify 4/8/16K extension ROM name at $4000")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_DA, "Angiv navn for 4/8/16k-udviddelses-ROM på $4000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_DE, "4/8/16K Erweiterungs ROM Dateiname für $4000 wählen"},
/* es */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_ES, "Especificar nombre ROM extendida de 4/8/16KB en $4000"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_FR, "Spécifier le nom de l'extension ROM 4/8/16K à $4000"},
/* hu */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_HU, "Adja meg a $4000 címû 4/8/16K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_IT, "Specifica il nome della ROM di estensione di 4/8/16K a $4000"},
/* ko */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_NL, "Geef de naam van het bestand voor de 4/8/16K ROM op $4000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_PL, "Okre¶l nazwê ROM rozszerzenia 4/8/16K w $4000"},
/* ru */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_RU, "Specify 4/8/16K extension ROM name at $4000"},
/* sv */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_SV, "Ange namn för 4/8/16K-utöknings-ROM på $4000"},
/* tr */ {IDCLS_SPECIFY_EXT_ROM_4000_NAME_TR, "$4000 adresindeki 4/8/16K uzantýsý ROM ismini belirt"},
#endif

/* vic20/cart/vic20cart.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME,    N_("Specify 4/8/16K extension ROM name at $6000")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_DA, "Angiv navn for 4/8/16k-udviddelses-ROM på $6000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_DE, "4/8/16K Erweiterungs ROM Dateiname für $6000 wählen"},
/* es */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_ES, "Especificar nombre ROM extendida de 4/8/16KB en $6000"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_FR, "Spécifier le nom de l'extension ROM 4/8/16K à $6000"},
/* hu */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_HU, "Adja meg a $6000 címû 4/8/16K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_IT, "Specifica il nome della ROM di estensione di 4/8/16K a $6000"},
/* ko */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_NL, "Geef de naam van het bestand voor de 4/8/16K ROM op $6000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_PL, "Okre¶l nazwê ROM rozszerzenia 4/8/16K w $6000"},
/* ru */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_RU, "Specify 4/8/16K extension ROM name at $6000"},
/* sv */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_SV, "Ange namn för 4/8/16K-utöknings-ROM på $6000"},
/* tr */ {IDCLS_SPECIFY_EXT_ROM_6000_NAME_TR, "$6000 adresindeki 4/8/16K uzantýsý ROM ismini belirt"},
#endif

/* vic20/cart/vic20cart.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME,    N_("Specify 4/8K extension ROM name at $A000")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_DA, "Angiv navn for 4/8k-udviddelses-ROM på $A000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_DE, "4/8/16K Erweiterungs ROM Dateiname für $A000 wählen"},
/* es */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_ES, "Especificar nombre ROM extendida de 4/8KB en $A000"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_FR, "Spécifier le nom de l'extension ROM 4/8K à $A000"},
/* hu */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_HU, "Adja meg a $A000 címû 4/8K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_IT, "Specifica il nome della ROM di estensione di 4/8K a $A000"},
/* ko */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_NL, "Geef de naam van het bestand voor de 4/8K ROM op $A000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_PL, "Okre¶l nazwê ROM rozszerzenia 4/8/16K w $A000"},
/* ru */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_RU, "Specify 4/8K extension ROM name at $A000"},
/* sv */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_SV, "Ange namn för 4/8K-utöknings-ROM på $A000"},
/* tr */ {IDCLS_SPECIFY_EXT_ROM_A000_NAME_TR, "$A000 adresindeki 4/8K uzantýsý ROM ismini belirt"},
#endif

/* vic20/cart/vic20cart.c */
/* en */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME,    N_("Specify 4K extension ROM name at $B000")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_DA, "Angiv navn for 4k-udviddelses-ROM på $B000"},
/* de */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_DE, "4/8/16K Erweiterungs ROM Dateiname für $B000 wählen"},
/* es */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_ES, "Especificar nombre ROM extendida de 4KB en $B000"},
/* fr */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_FR, "Spécifier le nom de l'extension ROM 4K à $B000"},
/* hu */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_HU, "Adja meg a $B000 címû 4K ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_IT, "Specifica il nome della ROM di estensione di 4 a $B000"},
/* ko */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_NL, "Geef de naam van het bestand voor de 4K ROM op $B000"},
/* pl */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_PL, "Okre¶l nazwê ROM rozszerzenia 4/8/16K w $B000"},
/* ru */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_RU, "Specify 4K extension ROM name at $B000"},
/* sv */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_SV, "Ange namn för 4K-utöknings-ROM på $B000"},
/* tr */ {IDCLS_SPECIFY_EXT_ROM_B000_NAME_TR, "$B000 adresindeki 4K uzantýsý ROM ismini belirt"},
#endif

/* vic20/cart/behrbonz.c */
/* en */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME,    N_("Specify Behr Bonz extension ROM name")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME_DE, "Behr Bonz Erweiterungs ROM Dateiname wählen"},
/* es */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME_NL, "Geef de naam van het bestand voor de Behr Bonz uitbreidings ROM"},
/* pl */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_BEHRBONZ_ROM_NAME_TR, ""},  /* fuzzy */
#endif

/* vic20/cart/vic20cart.c */
/* en */ {IDCLS_SPECIFY_GENERIC_ROM_NAME,    N_("Specify generic extension ROM name")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_GENERIC_ROM_NAME_DA, "Angiv navn på standard-udviddelses-ROM"},
/* de */ {IDCLS_SPECIFY_GENERIC_ROM_NAME_DE, "Generisches Erweiterungs ROM wählen"},
/* es */ {IDCLS_SPECIFY_GENERIC_ROM_NAME_ES, "Especificar nombre ROM extendida generica"},
/* fr */ {IDCLS_SPECIFY_GENERIC_ROM_NAME_FR, "Spécifier le nom de l'extension ROM générique"},
/* hu */ {IDCLS_SPECIFY_GENERIC_ROM_NAME_HU, "Adja meg az általános ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_GENERIC_ROM_NAME_IT, "Specifica il nome dell'estensione ROM generica"},
/* ko */ {IDCLS_SPECIFY_GENERIC_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_GENERIC_ROM_NAME_NL, "Geef de naam van het bestand voor de algemene ROM"},
/* pl */ {IDCLS_SPECIFY_GENERIC_ROM_NAME_PL, "Okre¶l nazwê typowego ROM-u rozszerzeñ"},
/* ru */ {IDCLS_SPECIFY_GENERIC_ROM_NAME_RU, "Specify generic extension ROM name"},
/* sv */ {IDCLS_SPECIFY_GENERIC_ROM_NAME_SV, "Ange namn på generiskt utöknings-ROM"},
/* tr */ {IDCLS_SPECIFY_GENERIC_ROM_NAME_TR, "Jenerik uzantý ROM ismini belirt"},
#endif

/* vic20/cart/vic20cart.c */
/* en */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME,    N_("Specify Mega-Cart extension ROM name")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME_DA, "Angiv navn på MegaCart-udviddelses-ROM"},
/* de */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME_DE, "Mega-Cart Erweiterungs ROM wählen"},
/* es */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME_ES, "Especificar nombre ROM extendida Mega Cart"},
/* fr */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME_FR, "Spécifier le nom de l'extension ROM Mega-Cart"},
/* hu */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME_HU, "Adja meg a Mega-Cart ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME_IT, "Specifica il nome dell'estensione ROM Mega-Cart"},
/* ko */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME_NL, "Geef de naam van het bestand voor de Mega-Cart ROM"},
/* pl */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME_PL, "Okre¶l nazwê ROM rozszerzenia Mega-Cart"},
/* ru */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME_RU, "Specify Mega-Cart extension ROM name"},
/* sv */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME_SV, "Ange namn på Mega-Cart-utöknings-ROM"},
/* tr */ {IDCLS_SPECIFY_MEGA_CART_ROM_NAME_TR, "Mega-Cart uzantýsý ROM ismini belirt"},
#endif

/* vic20/cart/vic20cart.c */
/* en */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME,    N_("Specify Final Expansion extension ROM name")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME_DA, "Angiv navn på Final Expansion udviddelses-ROM"},
/* de */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME_DE, "Name für Final Expansion Erweiterungs ROM wählen"},
/* es */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME_ES, "Especificar nombre ROM extendida Final Expansion"},
/* fr */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME_FR, "Spécifier le nom de l'extension ROM Final Expansion"},
/* hu */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME_HU, "Adja meg a Final Expansion ROM kiterjesztés nevét"},
/* it */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME_IT, "Specifica il nome dell'estensione ROM Final Expansion"},
/* ko */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME_NL, "Geef de naam van het bestand voor de Final Expansion ROM"},
/* pl */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME_PL, "Okre¶l nazwê rozszerzenia ROM Final Expansion"},
/* ru */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME_RU, "Specify Final Expansion extension ROM name"},
/* sv */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME_SV, "Ange namn på Final Expansion utöknings-ROM"},
/* tr */ {IDCLS_SPECIFY_FINAL_EXPANSION_ROM_NAME_TR, "Final Expansion uzantýsý ROM ismini belirt"},
#endif

/* vic20/cart/vic20cart.c */
/* en */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME,    N_("Specify Vic Flash Plugin extension ROM name")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME_DA, "Angiv navn på Vic Flash Plugin udviddelses-ROM"},
/* de */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME_DE, "Name für Vic Flash Plugin Erweiterungs ROM wählen"},
/* es */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME_ES, "Especificar nombre ROM extendida Vic Flash Plugin"},
/* fr */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME_FR, "Spécifier le nom de l'extension ROM Vic Flash Plugin"},
/* hu */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME_IT, "Specifica il nome dell'estensione ROM Vic Flash Plugin"},
/* ko */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME_NL, "Geef de naam van het bestand voor de Vic Flash Plugin ROM"},
/* pl */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME_PL, "Okre¶l nazwê rozszerzenia ROM Vic Flash Plugin"},
/* ru */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME_RU, "Specify Vic Flash Plugin extension ROM name"},
/* sv */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME_SV, "Ange namn på Vic Flash-insticksmodulutöknings-ROM"},
/* tr */ {IDCLS_SPECIFY_VIC_FP_ROM_NAME_TR, "Vic Flash Plugin uzantýsý ROM ismini belirt"},
#endif

/* vic20/cart/vic20cart.c */
/* en */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME,    N_("Specify UltiMem extension ROM name")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME_DE, "UltiMem Erweiterungs ROM wählen"},
/* es */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME_NL, "Geef de naam van het bestand voor de UltiMem ROM"},
/* pl */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_VIC_UM_ROM_NAME_TR, ""},  /* fuzzy */
#endif

/* vic20/cart/finalexpansion.c */
/* en */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK,    N_("Enable Final Expansion write back to ROM file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK_DA, "Aktivér Final Expansion tilbageskrivning til ROM-fil"},
/* de */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK_DE, "Final Expansion ROM Schreibzugriff aktivieren"},
/* es */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK_ES, "Permitir grabación en fichero ROM Final Expansion"},
/* fr */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK_FR, "Activer la réécriture du fichier ROM Final Expansion"},
/* hu */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK_HU, "Final Expansion ROM fájlba visszaírás engedélyezése"},
/* it */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK_IT, "Attiva write back per Final Expansion su file ROM"},
/* ko */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK_NL, "Terugschrijven naar ROM bestand voor Final Expansion inschakelen"},
/* pl */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK_PL, "W³±cz zapis do pliku ROM Final Expansion"},
/* ru */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK_RU, "Enable Final Expansion write back to ROM file"},
/* sv */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK_SV, "Aktivera återskrivning till ROM-fil för Final Expansion"},
/* tr */ {IDCLS_ENABLE_FINALEXPANSION_WRITEBACK_TR, "Final Expansion ROM dosyasýna geri yazmayý aktif et"},
#endif

/* vic20/cart/finalexpansion.c */
/* en */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK,    N_("Disable Final Expansion write back to ROM file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK_DA, "Deaktivér Final Expansion tilbageskrivning til ROM-fil"},
/* de */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK_DE, "Final Expansion ROM Schreibzugriff deaktivieren"},
/* es */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK_ES, "Deshabilitar grabación en fichero ROM Final Expansion"},
/* fr */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK_FR, "Désactiver la réécriture du fichier ROM Final Expansion"},
/* hu */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK_HU, "Final Expansion ROM fájlba visszaírás tiltása"},
/* it */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK_IT, "Disattiva write back per Final Expansion su file ROM"},
/* ko */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK_NL, "Deactiveer terug schrijven naar ROM bestand voor Final Expansion"},
/* pl */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK_PL, "Wy³±cz zapis do pliku ROM Final Expansion"},
/* ru */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK_RU, "Disable Final Expansion write back to ROM file"},
/* sv */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK_SV, "Inaktivera återskrivning till ROM-fil för Final Expansion"},
/* tr */ {IDCLS_DISABLE_FINALEXPANSION_WRITEBACK_TR, "Final Expansion ROM dosyasýna geri yazmayý pasifleþtir"},
#endif

/* vic20/cart/megacart.c */
/* en */ {IDCLS_SET_MEGACART_NVRAM_NAME,    N_("Set Mega-Cart NvRAM filename")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_MEGACART_NVRAM_NAME_DA, "Vælg Mega-Cart NvRAM filnavn"},
/* de */ {IDCLS_SET_MEGACART_NVRAM_NAME_DE, "Mega-Cart NvRAM Imagename wählen"},
/* es */ {IDCLS_SET_MEGACART_NVRAM_NAME_ES, "Seleccionar nombre fichero  Mega-Cart NvRAM"},
/* fr */ {IDCLS_SET_MEGACART_NVRAM_NAME_FR, "Nom du fichier NvRAM Mega-Cart"},
/* hu */ {IDCLS_SET_MEGACART_NVRAM_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_MEGACART_NVRAM_NAME_IT, "Imposta nome file NvRAM Mega-Cart"},
/* ko */ {IDCLS_SET_MEGACART_NVRAM_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_MEGACART_NVRAM_NAME_NL, "Zet Mega-Cart NvRAM bestandsnaam"},
/* pl */ {IDCLS_SET_MEGACART_NVRAM_NAME_PL, "Okre¶l nazwê pliku NvRAM Mega-Cart"},
/* ru */ {IDCLS_SET_MEGACART_NVRAM_NAME_RU, "Set Mega-Cart NvRAM filename"},
/* sv */ {IDCLS_SET_MEGACART_NVRAM_NAME_SV, "Välj  Mega-Cart NvRAM-filnamn"},
/* tr */ {IDCLS_SET_MEGACART_NVRAM_NAME_TR, "Mega-Cart NvRAM dosyaismini gir"},
#endif

/* vic20/cart/megacart.c */
/* en */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE,    N_("Enable Mega-Cart NvRAM writeback")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE_DA, "Aktivér Mega-Cart NvRAM tilbageskrivning"},
/* de */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE_DE, "Mega-Cart NvRAM Rückschreiben aktivieren"},
/* es */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE_ES, "Permitir respuesta Mega-Cart NvRAM"},
/* fr */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE_FR, "Permettre la réécriture de l'image Mega-Cart"},
/* hu */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE_IT, "Write back NvRAM Mega-Cart"},
/* ko */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE_NL, "Mega-Cart NvRAM terugschrijven inschakelem"},
/* pl */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE_PL, "W³±cz zapis do NvRAM Mega-Cart"},
/* ru */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE_RU, "Enable Mega-Cart NvRAM writeback"},
/* sv */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE_SV, "Aktivera Mega-Cart NvRAM-återskrivning"},
/* tr */ {IDCLS_ENABLE_MEGACART_NVRAM_WRITE_TR, "Mega-Cart NvRAM geri yazmayý aktif et"},
#endif

/* vic20/cart/megacart.c */
/* en */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE,    N_("Disable Mega-Cart NvRAM writeback")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE_DA, "Deaktivér Mega-Cart NvRAM tilbageskrivning"},
/* de */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE_DE, "Mega-Cart NvRAM Rückschreiben deaktivieren"},
/* es */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE_ES, "Deshabilitar grabación en Mega-Cart NvRAM"},
/* fr */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE_FR, "Interdire la réécriture NvRAM Mega-Cart"},
/* hu */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE_IT, "Disattiva write back su NvRAM Mega-Cart"},
/* ko */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE_NL, "Mega-Cart NvRAM terugschrijven uitschakelen"},
/* pl */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE_PL, "Wy³±cz zapis do NvRAM Mega-Cart"},
/* ru */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE_RU, "Disable Mega-Cart NvRAM writeback"},
/* sv */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE_SV, "Inaktivera Mega-Cart NvRAM-återskrivning"},
/* tr */ {IDCLS_DISABLE_MEGACART_NVRAM_WRITE_TR, "Mega-Cart NvRAM geri yazmayý pasifleþtir"},
#endif

/* vic20/cart/vic-fp.c */
/* en */ {IDCLS_ENABLE_VICFP_ROM_WRITE,    N_("Enable Vic Flash Plugin write back to ROM file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_VICFP_ROM_WRITE_DA, "Aktivér Vic Flash Plugin tilbageskrivning til ROM-fil"},
/* de */ {IDCLS_ENABLE_VICFP_ROM_WRITE_DE, "Vic Flash Plugin Rückschreiben auf ROM Datei aktivieren"},
/* es */ {IDCLS_ENABLE_VICFP_ROM_WRITE_ES, "Permitir grabación en ROM Vic Flash Plugin"},
/* fr */ {IDCLS_ENABLE_VICFP_ROM_WRITE_FR, "Activer la réécriture du fichier ROM Vic Flash Plugin"},
/* hu */ {IDCLS_ENABLE_VICFP_ROM_WRITE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_VICFP_ROM_WRITE_IT, "Attiva write back per Vic Flash Plugin su file ROM"},
/* ko */ {IDCLS_ENABLE_VICFP_ROM_WRITE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_VICFP_ROM_WRITE_NL, "Terugschrijven naar ROM bestand voor Vic Flash Plugin inschakelen"},
/* pl */ {IDCLS_ENABLE_VICFP_ROM_WRITE_PL, "W³±cz zapis do pliku ROM Vic Flash Plugin"},
/* ru */ {IDCLS_ENABLE_VICFP_ROM_WRITE_RU, "Enable Vic Flash Plugin write back to ROM file"},
/* sv */ {IDCLS_ENABLE_VICFP_ROM_WRITE_SV, "Aktivera återskrivning till ROM-fil för Vic Flash-insticksmodul"},
/* tr */ {IDCLS_ENABLE_VICFP_ROM_WRITE_TR, "Flash Plugin ROM dosyasýna geri yazmayý aktif et"},
#endif

/* vic20/cart/vic-fp.c */
/* en */ {IDCLS_DISABLE_VICFP_ROM_WRITE,    N_("Disable Vic Flash Plugin write back to ROM file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_VICFP_ROM_WRITE_DA, "Deaktivér Vic Flash Plugin tilbageskrivning til ROM-fil"},
/* de */ {IDCLS_DISABLE_VICFP_ROM_WRITE_DE, "Vic Flash Plugin Rückschreiben auf ROM Datei deaktivieren"},
/* es */ {IDCLS_DISABLE_VICFP_ROM_WRITE_ES, "Deshabilitar grabación en ROM Vic Flash Plugin"},
/* fr */ {IDCLS_DISABLE_VICFP_ROM_WRITE_FR, "Désactiver la réécriture du fichier ROM Vic Flash Plugin"},
/* hu */ {IDCLS_DISABLE_VICFP_ROM_WRITE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_VICFP_ROM_WRITE_IT, "Disattiva write back per Vic Flash Plugin su file ROM"},
/* ko */ {IDCLS_DISABLE_VICFP_ROM_WRITE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_VICFP_ROM_WRITE_NL, "Terugschrijven naar ROM bestand voor Vic Flash Plugin uitschakelen"},
/* pl */ {IDCLS_DISABLE_VICFP_ROM_WRITE_PL, "Wy³±cz zapis do pliku ROM Vic Flash Plugin"},
/* ru */ {IDCLS_DISABLE_VICFP_ROM_WRITE_RU, "Disable Vic Flash Plugin write back to ROM file"},
/* sv */ {IDCLS_DISABLE_VICFP_ROM_WRITE_SV, "Inaktivera återskrivning till ROM-fil för Vic Flash-insticksmodul"},
/* tr */ {IDCLS_DISABLE_VICFP_ROM_WRITE_TR, "Flash Plugin ROM dosyasýna geri yazmayý pasifleþtir"},
#endif

/* vic20/cart/ultimem.c */
/* en */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE,    N_("Enable UltiMem write back to ROM file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE_DE, "UltiMem ROM Schreibzugriff aktivieren"},
/* es */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE_NL, "Terugschrijven naar ROM bestand voor UltiMem inschakelen"},
/* pl */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_VIC_UM_ROM_WRITE_TR, ""},  /* fuzzy */
#endif

/* vic20/cart/ultimem.c */
/* en */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE,    N_("Disable UltiMem write back to ROM file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE_DE, "UltiMem ROM Schreibzugriff deaktivieren"},
/* es */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE_NL, "Terugschrijven naar ROM bestand voor UltiMem uitschakelen"},
/* pl */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_VIC_UM_ROM_WRITE_TR, ""},  /* fuzzy */
#endif

/* vic20/cart/ioramcart.c */
/* en */ {IDCLS_ENABLE_IO2_RAM,    N_("Enable I/O-2 RAM")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IO2_RAM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_IO2_RAM_DE, "I/O-2 RAM aktivieren"},
/* es */ {IDCLS_ENABLE_IO2_RAM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_IO2_RAM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_IO2_RAM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_IO2_RAM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_IO2_RAM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IO2_RAM_NL, "I/O-2 RAM inschakelen"},
/* pl */ {IDCLS_ENABLE_IO2_RAM_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_ENABLE_IO2_RAM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IO2_RAM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_IO2_RAM_TR, ""},  /* fuzzy */
#endif

/* vic20/cart/ioramcart.c */
/* en */ {IDCLS_DISABLE_IO2_RAM,    N_("Disable I/O-2 RAM")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IO2_RAM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_IO2_RAM_DE, "I/O-2 RAM deaktivieren"},
/* es */ {IDCLS_DISABLE_IO2_RAM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_IO2_RAM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_IO2_RAM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_IO2_RAM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_IO2_RAM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IO2_RAM_NL, "I/O-2 RAM uitschakelen"},
/* pl */ {IDCLS_DISABLE_IO2_RAM_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_DISABLE_IO2_RAM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IO2_RAM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_IO2_RAM_TR, ""},  /* fuzzy */
#endif

/* vic20/cart/ioramcart.c */
/* en */ {IDCLS_ENABLE_IO3_RAM,    N_("Enable I/O-3 RAM")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_IO3_RAM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_IO3_RAM_DE, "I/O-3 RAM aktivieren"},
/* es */ {IDCLS_ENABLE_IO3_RAM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_IO3_RAM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_IO3_RAM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_IO3_RAM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_IO3_RAM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_IO3_RAM_NL, "I/O-3 RAM inschakelen"},
/* pl */ {IDCLS_ENABLE_IO3_RAM_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_ENABLE_IO3_RAM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_IO3_RAM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_IO3_RAM_TR, ""},  /* fuzzy */
#endif

/* vic20/cart/ioramcart.c */
/* en */ {IDCLS_DISABLE_IO3_RAM,    N_("Disable I/O-3 RAM")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_IO3_RAM_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_IO3_RAM_DE, "I/O-3 RAM deaktivieren"},
/* es */ {IDCLS_DISABLE_IO3_RAM_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_IO3_RAM_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_IO3_RAM_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_IO3_RAM_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_IO3_RAM_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_IO3_RAM_NL, "I/O-3 RAM uitschakelen"},
/* pl */ {IDCLS_DISABLE_IO3_RAM_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_DISABLE_IO3_RAM_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_IO3_RAM_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_IO3_RAM_TR, ""},  /* fuzzy */
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_SET_BORDER_MODE,    N_("Set border display mode (0: normal, 1: full, 2: debug, 3: none)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_BORDER_MODE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_BORDER_MODE_DE, "Rahmen Darstellung Modus (0: Normal, 1: Voll, 2: Debug, 3: kein)"},
/* es */ {IDCLS_SET_BORDER_MODE_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_BORDER_MODE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_BORDER_MODE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_BORDER_MODE_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SET_BORDER_MODE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_BORDER_MODE_NL, ""},  /* fuzzy */
/* pl */ {IDCLS_SET_BORDER_MODE_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_SET_BORDER_MODE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_BORDER_MODE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SET_BORDER_MODE_TR, ""},  /* fuzzy */
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SPRITE_BACKGROUND,    N_("Enable sprite-background collision registers")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SPRITE_BACKGROUND_DA, "Aktivér sprite-til-baggrunds-kollisionsregistre"},
/* de */ {IDCLS_ENABLE_SPRITE_BACKGROUND_DE, "Sprite-Hintergrund Kollisionen aktivieren"},
/* es */ {IDCLS_ENABLE_SPRITE_BACKGROUND_ES, "Habilitar registros colisión sprite-fondo"},
/* fr */ {IDCLS_ENABLE_SPRITE_BACKGROUND_FR, "Activer les registres de collisions de sprite avec arrière-plan"},
/* hu */ {IDCLS_ENABLE_SPRITE_BACKGROUND_HU, "Sprite-háttér ütközési regiszterek engedélyezése"},
/* it */ {IDCLS_ENABLE_SPRITE_BACKGROUND_IT, "Attiva registri di collisione sprite-sfondo"},
/* ko */ {IDCLS_ENABLE_SPRITE_BACKGROUND_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SPRITE_BACKGROUND_NL, "Sprite-achtergrond botsing registers inschakelen"},
/* pl */ {IDCLS_ENABLE_SPRITE_BACKGROUND_PL, "W³±cz rejestry kolizji duszków z t³em"},
/* ru */ {IDCLS_ENABLE_SPRITE_BACKGROUND_RU, "Enable sprite-background collision registers"},
/* sv */ {IDCLS_ENABLE_SPRITE_BACKGROUND_SV, "Aktivera sprite-till-bakgrund-kollisionsregister"},
/* tr */ {IDCLS_ENABLE_SPRITE_BACKGROUND_TR, "Yaratýk-arka plan çarpýþma registerlarýný aktif et"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SPRITE_BACKGROUND,    N_("Disable sprite-background collision registers")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SPRITE_BACKGROUND_DA, "Deaktivér sprite-til-baggrunds-kollisionsregistre"},
/* de */ {IDCLS_DISABLE_SPRITE_BACKGROUND_DE, "Sprite-Hintergrund Kollisionen deaktivieren"},
/* es */ {IDCLS_DISABLE_SPRITE_BACKGROUND_ES, "Deshabilitar registros colisión sprite-fondo"},
/* fr */ {IDCLS_DISABLE_SPRITE_BACKGROUND_FR, "Désactiver les registres de collisions de sprite avec arrière-plan"},
/* hu */ {IDCLS_DISABLE_SPRITE_BACKGROUND_HU, "Sprite-háttér ütközési regiszterek tiltása"},
/* it */ {IDCLS_DISABLE_SPRITE_BACKGROUND_IT, "Disattiva registri di collisione sprite-sfondo"},
/* ko */ {IDCLS_DISABLE_SPRITE_BACKGROUND_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SPRITE_BACKGROUND_NL, "Sprite-achtergrond botsing registers uitschakelen"},
/* pl */ {IDCLS_DISABLE_SPRITE_BACKGROUND_PL, "Wy³±cz rejestry kolizji duszków z t³em"},
/* ru */ {IDCLS_DISABLE_SPRITE_BACKGROUND_RU, "Disable sprite-background collision registers"},
/* sv */ {IDCLS_DISABLE_SPRITE_BACKGROUND_SV, "Inaktivera sprite-till-bakgrund-kollisionsregister"},
/* tr */ {IDCLS_DISABLE_SPRITE_BACKGROUND_TR, "Yaratýk-arka plan çarpýþma registerlarýný pasifleþtir"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SPRITE_SPRITE,    N_("Enable sprite-sprite collision registers")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SPRITE_SPRITE_DA, "Aktivér sprite-til-sprite-kollisionsregistre"},
/* de */ {IDCLS_ENABLE_SPRITE_SPRITE_DE, "Sprite-Sprite Kollisionen aktivieren"},
/* es */ {IDCLS_ENABLE_SPRITE_SPRITE_ES, "Habilitar registros colisión sprite-sprite"},
/* fr */ {IDCLS_ENABLE_SPRITE_SPRITE_FR, "Activer les registres de collisions de sprite avec sprite"},
/* hu */ {IDCLS_ENABLE_SPRITE_SPRITE_HU, "Sprite-sprite ütközési regiszterek engedélyezése"},
/* it */ {IDCLS_ENABLE_SPRITE_SPRITE_IT, "Attiva registri di collisione sprite-sprite"},
/* ko */ {IDCLS_ENABLE_SPRITE_SPRITE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SPRITE_SPRITE_NL, "Sprite-sprite botsing registers inschakelen"},
/* pl */ {IDCLS_ENABLE_SPRITE_SPRITE_PL, "W³±cz rejestry kolizji duszków"},
/* ru */ {IDCLS_ENABLE_SPRITE_SPRITE_RU, "Enable sprite-sprite collision registers"},
/* sv */ {IDCLS_ENABLE_SPRITE_SPRITE_SV, "Aktivera sprite-till-sprite-kollisionsregister"},
/* tr */ {IDCLS_ENABLE_SPRITE_SPRITE_TR, "Yaratýk-yaratýk çarpýþma registerlarýný aktif et"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SPRITE_SPRITE,    N_("Disable sprite-sprite collision registers")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SPRITE_SPRITE_DA, "Deaktivér sprite-til-sprite-kollisionsregistre"},
/* de */ {IDCLS_DISABLE_SPRITE_SPRITE_DE, "Sprite-Sprite Kollisionen deaktivieren"},
/* es */ {IDCLS_DISABLE_SPRITE_SPRITE_ES, "Deshabilitar registros colisión sprite-sprite"},
/* fr */ {IDCLS_DISABLE_SPRITE_SPRITE_FR, "Désactiver les registres de collisions de sprite avec sprite"},
/* hu */ {IDCLS_DISABLE_SPRITE_SPRITE_HU, "Sprite-sprite ütközési regiszterek tiltása"},
/* it */ {IDCLS_DISABLE_SPRITE_SPRITE_IT, "Disattiva registri di collisione sprite-sprite"},
/* ko */ {IDCLS_DISABLE_SPRITE_SPRITE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SPRITE_SPRITE_NL, "Sprite-sprite botsing registers uitschakelen"},
/* pl */ {IDCLS_DISABLE_SPRITE_SPRITE_PL, "Wy³±cz rejestry kolizji duszków"},
/* ru */ {IDCLS_DISABLE_SPRITE_SPRITE_RU, "Disable sprite-sprite collision registers"},
/* sv */ {IDCLS_DISABLE_SPRITE_SPRITE_SV, "Inaktivera sprite-till-sprite-kollisionsregister"},
/* tr */ {IDCLS_DISABLE_SPRITE_SPRITE_TR, "Yaratýk-yaratýk çarpýþma registerlarýný pasifleþtir"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_ENABLE_VSPBUG,    N_("Enable VSP bug emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_VSPBUG_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_VSPBUG_DE, "VSP Fehler Emulation aktivieren"},
/* es */ {IDCLS_ENABLE_VSPBUG_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_VSPBUG_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_VSPBUG_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_VSPBUG_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_VSPBUG_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_VSPBUG_NL, "VSP bug emulatie inschakelen"},
/* pl */ {IDCLS_ENABLE_VSPBUG_PL, "W³±cz emulacjê blêdu VSP"},
/* ru */ {IDCLS_ENABLE_VSPBUG_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_VSPBUG_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_VSPBUG_TR, ""},  /* fuzzy */
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_DISABLE_VSPBUG,    N_("Disable VSP bug emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_VSPBUG_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_VSPBUG_DE, "VSP Fehler Emulation deaktivieren"},
/* es */ {IDCLS_DISABLE_VSPBUG_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_VSPBUG_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_VSPBUG_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_VSPBUG_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_VSPBUG_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_VSPBUG_NL, "VSP bug emulatie uitschakelen"},
/* pl */ {IDCLS_DISABLE_VSPBUG_PL, "Wy³±cz emulacjê b³êdu VSP"},
/* ru */ {IDCLS_DISABLE_VSPBUG_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_VSPBUG_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_VSPBUG_TR, ""},  /* fuzzy */
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_USE_NEW_LUMINANCES,    N_("Use new luminances")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_NEW_LUMINANCES_DA, "Brug nye lysstyrker"},
/* de */ {IDCLS_USE_NEW_LUMINANCES_DE, "Neue Helligkeitsemulation"},
/* es */ {IDCLS_USE_NEW_LUMINANCES_ES, "Usar luminancias nuevas"},
/* fr */ {IDCLS_USE_NEW_LUMINANCES_FR, "Utiliser les nouvelles luminescences"},
/* hu */ {IDCLS_USE_NEW_LUMINANCES_HU, "Az új fényerõket használja"},
/* it */ {IDCLS_USE_NEW_LUMINANCES_IT, "Usa nuove luminanze"},
/* ko */ {IDCLS_USE_NEW_LUMINANCES_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_NEW_LUMINANCES_NL, "Gebruik nieuwe luminantie"},
/* pl */ {IDCLS_USE_NEW_LUMINANCES_PL, "U¿yj nowego typu jaskrawo¶ci"},
/* ru */ {IDCLS_USE_NEW_LUMINANCES_RU, "Use new luminances"},
/* sv */ {IDCLS_USE_NEW_LUMINANCES_SV, "Använd nya ljusstyrkor"},
/* tr */ {IDCLS_USE_NEW_LUMINANCES_TR, "Yeni parlaklýklarý kullan"},
#endif

/* vicii/vicii-cmdline-options.c */
/* en */ {IDCLS_USE_OLD_LUMINANCES,    N_("Use old luminances")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_OLD_LUMINANCES_DA, "Brug gamle lysstyrker"},
/* de */ {IDCLS_USE_OLD_LUMINANCES_DE, "Alte Helligkeitsemulation"},
/* es */ {IDCLS_USE_OLD_LUMINANCES_ES, "Usar luminancias antiguas"},
/* fr */ {IDCLS_USE_OLD_LUMINANCES_FR, "Utiliser les anciennes luminescences"},
/* hu */ {IDCLS_USE_OLD_LUMINANCES_HU, "A régi fényerõket használja"},
/* it */ {IDCLS_USE_OLD_LUMINANCES_IT, "Usa vecchie luminanze"},
/* ko */ {IDCLS_USE_OLD_LUMINANCES_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_OLD_LUMINANCES_NL, "Gebruik oude luminantie"},
/* pl */ {IDCLS_USE_OLD_LUMINANCES_PL, "U¿yj starego typu jaskrawo¶ci"},
/* ru */ {IDCLS_USE_OLD_LUMINANCES_RU, "Use old luminances"},
/* sv */ {IDCLS_USE_OLD_LUMINANCES_SV, "Använd gamla ljusstyrkor"},
/* tr */ {IDCLS_USE_OLD_LUMINANCES_TR, "Eski parlaklýklarý kullan"},
#endif

/* monitor.c */
/* en */ {IDCLS_EXECUTE_MONITOR_FROM_FILE,    N_("Execute monitor commands from file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_DA, "Udfør monitorkommandoer fra fil"},
/* de */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_DE, "Monitor Kommandos aus Datei ausführen"},
/* es */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_ES, "Ejecutar comandos del monitor desde fichero"},
/* fr */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_FR, "Exécuter des commandes moniteur depuis un fichier"},
/* hu */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_HU, "Monitorprogram parancsok futtatása fájlból"},
/* it */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_IT, "Esegui i comandi del monitor da un file"},
/* ko */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_NL, "Uitvoeren van monitor commando's uit bestand"},
/* pl */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_PL, "Wykonaj polecenia monitora z pliku"},
/* ru */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_RU, "Execute monitor commands from file"},
/* sv */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_SV, "Exekvera monitorkommandon från fil"},
/* tr */ {IDCLS_EXECUTE_MONITOR_FROM_FILE_TR, "Dosyadan monitör komutlarý çalýþtýr"},
#endif

/* monitor.c */
/* en */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE,    N_("Write monitor output also to file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE_DE, "Monitor Ausgabe auch auf Datei schreiben"},
/* es */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE_ES, "Grabar salida del monitor tambien en fichero"},
/* fr */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE_IT, "Scrivi anche su file l'output del monitor"},
/* ko */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE_NL, "Schrijf uitvoer van de monitor ook naar een bestand"},
/* pl */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE_PL, "Zapisuj wyj¶cie monitora równie¿ do pliku"},
/* ru */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE_SV, "Skriv monitorutdata även till fil"},
/* tr */ {IDCLS_WRITE_MONITOR_LOG_TO_FILE_TR, ""},  /* fuzzy */
#endif

/* monitor.c */
/* en */ {IDCLS_SET_INITIAL_BREAKPOINT,    N_("Set an initial breakpoint for the monitor")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_INITIAL_BREAKPOINT_DA, "Sæt et initielt breakpoint for monitoren"},
/* de */ {IDCLS_SET_INITIAL_BREAKPOINT_DE, "Setze initialen Breakpoint für Monitor"},
/* es */ {IDCLS_SET_INITIAL_BREAKPOINT_ES, "Seleccionar un punto inicial de ruptura para el monitor"},
/* fr */ {IDCLS_SET_INITIAL_BREAKPOINT_FR, "Régler un point d’arrêt initial pour le moniteur"},
/* hu */ {IDCLS_SET_INITIAL_BREAKPOINT_HU, "Kezdeti töréspont megadása a monitorprogramnak"},
/* it */ {IDCLS_SET_INITIAL_BREAKPOINT_IT, "Imposta un breakpoint iniziale per il monitor"},
/* ko */ {IDCLS_SET_INITIAL_BREAKPOINT_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_INITIAL_BREAKPOINT_NL, "Stel een initieel breekpunt in voor de monitor"},
/* pl */ {IDCLS_SET_INITIAL_BREAKPOINT_PL, "Okre¶l pocz±tkowy punkt wstrzymania w monitorze"},
/* ru */ {IDCLS_SET_INITIAL_BREAKPOINT_RU, "Set an initial breakpoint for the monitor"},
/* sv */ {IDCLS_SET_INITIAL_BREAKPOINT_SV, "Sätt en ursprunglig brytpunkt för monitorn"},
/* tr */ {IDCLS_SET_INITIAL_BREAKPOINT_TR, "Monitör için dahili bir kesme noktasý ayarla"},
#endif

/* monitor.c */
/* en */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN,    N_("Keep the monitor open")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN_DE, "Monitor göffnet halten"},
/* es */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN_NL, "Laat de monitor geopend blijven"},
/* pl */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN_PL, "Nie zamykaj monitora"},
/* ru */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_KEEP_MONITOR_OPEN_TR, ""},  /* fuzzy */
#endif

/* monitor.c */
/* en */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN,    N_("Do not keep the monitor open")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN_DE, "Monitor nicht göffnet halten"},
/* es */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN_NL, "Laat de monitor niet geopend blijven"},
/* pl */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN_PL, "Zamykaj monitor"},
/* ru */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_KEEP_MONITOR_OPEN_TR, ""},  /* fuzzy */
#endif

/* monitor_network.c */
/* en */ {IDCLS_ENABLE_REMOTE_MONITOR,    N_("Enable remote monitor")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_REMOTE_MONITOR_DA, "Aktivér fjern-monitor"},
/* de */ {IDCLS_ENABLE_REMOTE_MONITOR_DE, "Monitor Fernsteuerung aktivieren"},
/* es */ {IDCLS_ENABLE_REMOTE_MONITOR_ES, "Permitir monitor remoto"},
/* fr */ {IDCLS_ENABLE_REMOTE_MONITOR_FR, "Activer le moniteur distant"},
/* hu */ {IDCLS_ENABLE_REMOTE_MONITOR_HU, "Távoli monitor engedélyezése"},
/* it */ {IDCLS_ENABLE_REMOTE_MONITOR_IT, "Attiva monitor remoto"},
/* ko */ {IDCLS_ENABLE_REMOTE_MONITOR_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_REMOTE_MONITOR_NL, "Monitor afstandsbediening inschakelen"},
/* pl */ {IDCLS_ENABLE_REMOTE_MONITOR_PL, "W³±cz zdalny monitor"},
/* ru */ {IDCLS_ENABLE_REMOTE_MONITOR_RU, "Enable remote monitor"},
/* sv */ {IDCLS_ENABLE_REMOTE_MONITOR_SV, "Aktivera fjärrmonitor"},
/* tr */ {IDCLS_ENABLE_REMOTE_MONITOR_TR, "Uzak monitörü aktif et"},
#endif

/* monitor_network.c */
/* en */ {IDCLS_DISABLE_REMOTE_MONITOR,    N_("Disable remote monitor")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_REMOTE_MONITOR_DA, "Deaktivér fjern-monitor"},
/* de */ {IDCLS_DISABLE_REMOTE_MONITOR_DE, "Monitor Fernsteuerung deaktivieren"},
/* es */ {IDCLS_DISABLE_REMOTE_MONITOR_ES, "Deshabilitar monitor remoto"},
/* fr */ {IDCLS_DISABLE_REMOTE_MONITOR_FR, "Désactiver le moniteur distant"},
/* hu */ {IDCLS_DISABLE_REMOTE_MONITOR_HU, "Távoli monitor tiltása"},
/* it */ {IDCLS_DISABLE_REMOTE_MONITOR_IT, "Disattiva monitor remoto"},
/* ko */ {IDCLS_DISABLE_REMOTE_MONITOR_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_REMOTE_MONITOR_NL, "Monitor afstandsbediening uitschakelen"},
/* pl */ {IDCLS_DISABLE_REMOTE_MONITOR_PL, "Wy³±cz zdalny monitor"},
/* ru */ {IDCLS_DISABLE_REMOTE_MONITOR_RU, "Disable remote monitor"},
/* sv */ {IDCLS_DISABLE_REMOTE_MONITOR_SV, "Inaktivera fjärrmonitor"},
/* tr */ {IDCLS_DISABLE_REMOTE_MONITOR_TR, "Uzak monitörü pasifleþtir"},
#endif

/* monitor_network.c */
/* en */ {IDCLS_REMOTE_MONITOR_ADDRESS,    N_("The local address the remote monitor should bind to")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_REMOTE_MONITOR_ADDRESS_DA, "Lokal adresse fjern-monitoren skal binde til"},
/* de */ {IDCLS_REMOTE_MONITOR_ADDRESS_DE, "Lokale Adresse für die Monitorfernsteuerung"},
/* es */ {IDCLS_REMOTE_MONITOR_ADDRESS_ES, "Dirección del monitor remoto al que se debe unir"},
/* fr */ {IDCLS_REMOTE_MONITOR_ADDRESS_FR, "L'adresse locale à laquelle le moniteur distant devrait se brancher"},
/* hu */ {IDCLS_REMOTE_MONITOR_ADDRESS_HU, "A helyi cím, amelyet a monitorprogram lefoglal"},
/* it */ {IDCLS_REMOTE_MONITOR_ADDRESS_IT, "Indirizzo locale a cui il monitor remoto dovrebbe collegarsi"},
/* ko */ {IDCLS_REMOTE_MONITOR_ADDRESS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_REMOTE_MONITOR_ADDRESS_NL, "Lokaal adres waar de afstandsbediening voor de monitor aan gebonden moet worden"},
/* pl */ {IDCLS_REMOTE_MONITOR_ADDRESS_PL, "Lokalny adres, z którym ma siê po³±czyæ zdalny monitor"},
/* ru */ {IDCLS_REMOTE_MONITOR_ADDRESS_RU, "The local address the remote monitor should bind to"},
/* sv */ {IDCLS_REMOTE_MONITOR_ADDRESS_SV, "Lokal adress för fjärrmonitor att ansluta till"},
/* tr */ {IDCLS_REMOTE_MONITOR_ADDRESS_TR, "Uzak monitörün baðlanmasý gereken lokal adres"},
#endif

/* fliplist.c */
/* en */ {IDCLS_SPECIFY_FLIP_LIST_NAME,    N_("Specify name of the flip list file image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_FLIP_LIST_NAME_DA, "Angiv navn på bladreliste"},
/* de */ {IDCLS_SPECIFY_FLIP_LIST_NAME_DE, "Namen für Fliplist Datei wählen"},
/* es */ {IDCLS_SPECIFY_FLIP_LIST_NAME_ES, "Especificar nombre fichero lista de intercambio imagenes"},
/* fr */ {IDCLS_SPECIFY_FLIP_LIST_NAME_FR, "Spécifier le nom de l'image du fichier de groupement de disques"},
/* hu */ {IDCLS_SPECIFY_FLIP_LIST_NAME_HU, "Adja meg a lemezlista fájl nevét"},
/* it */ {IDCLS_SPECIFY_FLIP_LIST_NAME_IT, "Specifica il nome immagine del file della flip list"},
/* ko */ {IDCLS_SPECIFY_FLIP_LIST_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_FLIP_LIST_NAME_NL, "Geef de naam van het fliplijstbestand"},
/* pl */ {IDCLS_SPECIFY_FLIP_LIST_NAME_PL, "Okre¶l nazwê pliku obrazu listy podmiany no¶ników"},
/* ru */ {IDCLS_SPECIFY_FLIP_LIST_NAME_RU, "Specify name of the flip list file image"},
/* sv */ {IDCLS_SPECIFY_FLIP_LIST_NAME_SV, "Ange namn på vallistefilavbildningen"},
/* tr */ {IDCLS_SPECIFY_FLIP_LIST_NAME_TR, "Sýralama listesi dosya imajý ismi belirt"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS,    N_("Show a list of the available options and exit normally")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_DA, "Vis en liste over tilgængelige parametre og afslut"},
/* de */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_DE, "Liste von verfügbaren Optionen zeigen und beenden"},
/* es */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_ES, "Muestra lista de las opciones disponibles al salir normalmente"},
/* fr */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_FR, "Montrer une liste des options disponibles et terminer normalement"},
/* hu */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_HU, "Kilistázza a lehetséges opciókat és kilép"},
/* it */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_IT, "Mostra un elenco delle opzioni disponibili ed esce"},
/* ko */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_NL, "Toon een lijst van de beschikbare opties en sluit normaal af"},
/* pl */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_PL, "Poka¿ listê dostêpnych poleceñ i normalnie wyjd¼"},
/* ru */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_RU, "Show a list of the available options and exit normally"},
/* sv */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_SV, "Visa lista över tillgängliga flaggor och avsluta normalt"},
/* tr */ {IDCLS_SHOW_COMMAND_LINE_OPTIONS_TR, "Mevcut seçeneklerin listesini göster ve normal çýk"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_SHOW_COMPILETIME_FEATURES,    N_("Show a list of the available compile-time options and their configuration.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SHOW_COMPILETIME_FEATURES_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SHOW_COMPILETIME_FEATURES_DE, "Liste von verfügbaren kompilierten Optionen und deren Konfiguration zeigen."},
/* es */ {IDCLS_SHOW_COMPILETIME_FEATURES_ES, "Muestra lista de las opciones disponibles en tiempo de compilación y su configuración."},
/* fr */ {IDCLS_SHOW_COMPILETIME_FEATURES_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SHOW_COMPILETIME_FEATURES_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SHOW_COMPILETIME_FEATURES_IT, "Mostra un elenco delle opzioni di compilazione disponibili e la loro configurazione."},
/* ko */ {IDCLS_SHOW_COMPILETIME_FEATURES_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SHOW_COMPILETIME_FEATURES_NL, "Toon een lijst van de beschikbare 'compile-time' opties en de bijbehorende configuratie."},
/* pl */ {IDCLS_SHOW_COMPILETIME_FEATURES_PL, "Poka¿ listê dostêpnych opcji kompilacji i ich konfiguracjê."},
/* ru */ {IDCLS_SHOW_COMPILETIME_FEATURES_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SHOW_COMPILETIME_FEATURES_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SHOW_COMPILETIME_FEATURES_TR, ""},  /* fuzzy */
#endif

#if (!defined  __OS2__ && !defined __BEOS__)
/* initcmdline.c */
/* en */ {IDCLS_CONSOLE_MODE,    N_("Console mode (for music playback)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_CONSOLE_MODE_DA, "Konsoltilstand (for musikafspilning)"},
/* de */ {IDCLS_CONSOLE_MODE_DE, "Konsolenmodus (für Musikwiedergabe)"},
/* es */ {IDCLS_CONSOLE_MODE_ES, "Modo consola (para reproducción música)"},
/* fr */ {IDCLS_CONSOLE_MODE_FR, "Mode console (pour l'écoute de musique)"},
/* hu */ {IDCLS_CONSOLE_MODE_HU, "Konzol mód (zene lejátszáshoz)"},
/* it */ {IDCLS_CONSOLE_MODE_IT, "Modalità console (per la riproduzione musicale)"},
/* ko */ {IDCLS_CONSOLE_MODE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_CONSOLE_MODE_NL, "Consolemodus (voor afspelen van muziek)"},
/* pl */ {IDCLS_CONSOLE_MODE_PL, "Tryb konsolowy (do odtwarzania muzyki)"},
/* ru */ {IDCLS_CONSOLE_MODE_RU, "Console mode (for music playback)"},
/* sv */ {IDCLS_CONSOLE_MODE_SV, "Konsolläge (för musikspelning)"},
/* tr */ {IDCLS_CONSOLE_MODE_TR, "Konsol modu (müzik çalmak için)"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ALLOW_CORE_DUMPS,    N_("Allow production of core dumps")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ALLOW_CORE_DUMPS_DA, "Tillad generering af core-dumps"},
/* de */ {IDCLS_ALLOW_CORE_DUMPS_DE, "Core Dumps ermöglichen"},
/* es */ {IDCLS_ALLOW_CORE_DUMPS_ES, "Permitir producción volcados de nucleo"},
/* fr */ {IDCLS_ALLOW_CORE_DUMPS_FR, "Permettre la production de \"core dumps\""},
/* hu */ {IDCLS_ALLOW_CORE_DUMPS_HU, "Core dump készítés engedélyezése"},
/* it */ {IDCLS_ALLOW_CORE_DUMPS_IT, "Consente la produzione di core dump"},
/* ko */ {IDCLS_ALLOW_CORE_DUMPS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ALLOW_CORE_DUMPS_NL, "Maken van core dumps toestaan"},
/* pl */ {IDCLS_ALLOW_CORE_DUMPS_PL, "Pozwól tworzyæ zrzuty rdzenia"},
/* ru */ {IDCLS_ALLOW_CORE_DUMPS_RU, "Allow production of core dumps"},
/* sv */ {IDCLS_ALLOW_CORE_DUMPS_SV, "Tillåt skapa minnesutskrift"},
/* tr */ {IDCLS_ALLOW_CORE_DUMPS_TR, "Bellek durumunun kayýtlarýnýn üretilmesine izin ver"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_DONT_ALLOW_CORE_DUMPS,    N_("Do not produce core dumps")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DONT_ALLOW_CORE_DUMPS_DA, "Lav ikke core-dumps"},
/* de */ {IDCLS_DONT_ALLOW_CORE_DUMPS_DE, "Core Dumps verhindern"},
/* es */ {IDCLS_DONT_ALLOW_CORE_DUMPS_ES, "No permitir producción volcados de nucleo"},
/* fr */ {IDCLS_DONT_ALLOW_CORE_DUMPS_FR, "Ne pas produire de \"core dumps\""},
/* hu */ {IDCLS_DONT_ALLOW_CORE_DUMPS_HU, "Core dump készítés tiltása"},
/* it */ {IDCLS_DONT_ALLOW_CORE_DUMPS_IT, "Non produce core dump"},
/* ko */ {IDCLS_DONT_ALLOW_CORE_DUMPS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DONT_ALLOW_CORE_DUMPS_NL, "Maken van core dumps niet toestaan"},
/* pl */ {IDCLS_DONT_ALLOW_CORE_DUMPS_PL, "Nie twórz zrzutów rdzenia"},
/* ru */ {IDCLS_DONT_ALLOW_CORE_DUMPS_RU, "Do not produce core dumps"},
/* sv */ {IDCLS_DONT_ALLOW_CORE_DUMPS_SV, "Skapa inte minnesutskrift"},
/* tr */ {IDCLS_DONT_ALLOW_CORE_DUMPS_TR, "Bellek durumunun kayýtlarýný üretme"},
#endif

#else

/* initcmdline.c */
/* en */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER,    N_("Don't call exception handler")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_DA, "Kald ikke exception-handler"},
/* de */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_DE, "Ausnahmebehandlung vermeiden"},
/* es */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_ES, "No llamar al manipulador de excepciones"},
/* fr */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_FR, "Ne pas utiliser l'assistant d'exception"},
/* hu */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_HU, "Ne hívja a kivétel kezelõt"},
/* it */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_IT, "Richiama il gestore delle eccezioni"},
/* ko */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_NL, "Geen gebruik maken van de exception handler"},
/* pl */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_PL, "Nie zajmuj siê wyj±tkami"},
/* ru */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_RU, "Don't call exception handler"},
/* sv */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_SV, "Anropa inte undantagshanterare"},
/* tr */ {IDCLS_DONT_CALL_EXCEPTION_HANDLER_TR, "Kural dýþý iþleyiciyi çaðýrma"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_CALL_EXCEPTION_HANDLER,    N_("Call exception handler (default)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_CALL_EXCEPTION_HANDLER_DA, "Kald exception-handler (standard)"},
/* de */ {IDCLS_CALL_EXCEPTION_HANDLER_DE, "Ausnahmebehandlung aktivieren (Default)"},
/* es */ {IDCLS_CALL_EXCEPTION_HANDLER_ES, "Llamar al manipulador de excepciones (por defecto)"},
/* fr */ {IDCLS_CALL_EXCEPTION_HANDLER_FR, "Utiliser l'assistant d'exception (par défaut)"},
/* hu */ {IDCLS_CALL_EXCEPTION_HANDLER_HU, "Kivétel kezelõ hívása (alapértelmezés)"},
/* it */ {IDCLS_CALL_EXCEPTION_HANDLER_IT, "Richiama il gestore delle eccezioni (predefinito)"},
/* ko */ {IDCLS_CALL_EXCEPTION_HANDLER_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_CALL_EXCEPTION_HANDLER_NL, "Gebruik maken van de exception handler (standaard)"},
/* pl */ {IDCLS_CALL_EXCEPTION_HANDLER_PL, "Zajmij siê wyj±tkami (domy¶lnie)"},
/* ru */ {IDCLS_CALL_EXCEPTION_HANDLER_RU, "Call exception handler (default)"},
/* sv */ {IDCLS_CALL_EXCEPTION_HANDLER_SV, "Anropa undantagshanterare (standard)"},
/* tr */ {IDCLS_CALL_EXCEPTION_HANDLER_TR, "Kural dýþý iþleyiciyi çaðýr (varsayýlan)"},
#endif
#endif

/* initcmdline.c */
/* en */ {IDCLS_RESTORE_DEFAULT_SETTINGS,    N_("Restore default settings")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_RESTORE_DEFAULT_SETTINGS_DA, "Gendan standardindstillinger"},
/* de */ {IDCLS_RESTORE_DEFAULT_SETTINGS_DE, "Standard Einstellungen wiederherstellen"},
/* es */ {IDCLS_RESTORE_DEFAULT_SETTINGS_ES, "Restaurar valores por defecto"},
/* fr */ {IDCLS_RESTORE_DEFAULT_SETTINGS_FR, "Rétablir les paramètres par défaut"},
/* hu */ {IDCLS_RESTORE_DEFAULT_SETTINGS_HU, "Alapértelmezés visszaállítása"},
/* it */ {IDCLS_RESTORE_DEFAULT_SETTINGS_IT, "Ripristina impostazioni predefinite"},
/* ko */ {IDCLS_RESTORE_DEFAULT_SETTINGS_KO, "µðÆúÆ® ¼³Á¤ º¹¿ø½ÃÅ°±â"},
/* nl */ {IDCLS_RESTORE_DEFAULT_SETTINGS_NL, "Herstel standaard instellingen"},
/* pl */ {IDCLS_RESTORE_DEFAULT_SETTINGS_PL, "Przywróæ domy¶lne ustawienia"},
/* ru */ {IDCLS_RESTORE_DEFAULT_SETTINGS_RU, "²ÞááâÐÝÞÒØâì ÝÐáâàÞÙÚØ ßÞ ãÜÞÛçÐÝØî"},
/* sv */ {IDCLS_RESTORE_DEFAULT_SETTINGS_SV, "Återställ förvalda inställningar"},
/* tr */ {IDCLS_RESTORE_DEFAULT_SETTINGS_TR, "Varsayýlan ayarlarý geri yükle"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_SPECIFY_CONFIG_FILE,    N_("Specify config file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_CONFIG_FILE_DA, "Angiv konfigurationsfil"},
/* de */ {IDCLS_SPECIFY_CONFIG_FILE_DE, "Konfigurationsdateiname definieren"},
/* es */ {IDCLS_SPECIFY_CONFIG_FILE_ES, "Especificar fichero de configuración"},
/* fr */ {IDCLS_SPECIFY_CONFIG_FILE_FR, "Spécifier le fichier de configuration"},
/* hu */ {IDCLS_SPECIFY_CONFIG_FILE_HU, "Adja meg a konfigurációs fájl nevét"},
/* it */ {IDCLS_SPECIFY_CONFIG_FILE_IT, "Specifica il nome del file di configurazione"},
/* ko */ {IDCLS_SPECIFY_CONFIG_FILE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_CONFIG_FILE_NL, "Geef configuratie bestandsnaam"},
/* pl */ {IDCLS_SPECIFY_CONFIG_FILE_PL, "Okre¶l plik konfiguracji"},
/* ru */ {IDCLS_SPECIFY_CONFIG_FILE_RU, "Specify config file"},
/* sv */ {IDCLS_SPECIFY_CONFIG_FILE_SV, "Ange inställningsfil"},
/* tr */ {IDCLS_SPECIFY_CONFIG_FILE_TR, "Konfigürasyon dosyasýný belirt"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_SPECIFY_DUMPCONFIG_FILE,    N_("Dump all resources to specified config file")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_DUMPCONFIG_FILE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_DUMPCONFIG_FILE_DE, "Exportiere sämtliche Resourcen in angegebene Datei"},
/* es */ {IDCLS_SPECIFY_DUMPCONFIG_FILE_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_DUMPCONFIG_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_DUMPCONFIG_FILE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_DUMPCONFIG_FILE_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SPECIFY_DUMPCONFIG_FILE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_DUMPCONFIG_FILE_NL, "Sla alle huidige instellingen op naar opgegeven configuratiebestand"},
/* pl */ {IDCLS_SPECIFY_DUMPCONFIG_FILE_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_SPECIFY_DUMPCONFIG_FILE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_DUMPCONFIG_FILE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_DUMPCONFIG_FILE_TR, ""},  /* fuzzy */
#endif

/* initcmdline.c */
/* en */ {IDCLS_LIMIT_CYCLES,    N_("Specify number of cycles to run before quitting with an error.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_LIMIT_CYCLES_DA, ""},  /* fuzzy */
/* de */ {IDCLS_LIMIT_CYCLES_DE, "Definiere die Zyklenanzahl, welche vor dem Beenden der Emulation noch\nausgeführt wird."},
/* es */ {IDCLS_LIMIT_CYCLES_ES, "Especificar número de ciclos a ejecutar antes de salir con un error"},
/* fr */ {IDCLS_LIMIT_CYCLES_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_LIMIT_CYCLES_HU, ""},  /* fuzzy */
/* it */ {IDCLS_LIMIT_CYCLES_IT, "Specifica il numero di cicli da eseguire prima di uscire con un errore."},
/* ko */ {IDCLS_LIMIT_CYCLES_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_LIMIT_CYCLES_NL, "Geef het aantal uit te voeren cycli voor het afsluiten met een fout."},
/* pl */ {IDCLS_LIMIT_CYCLES_PL, "Ustal, ile cykli pu¶ciæ przed zatrzymaniem i wyrzuceniem b³êdu."},
/* ru */ {IDCLS_LIMIT_CYCLES_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_LIMIT_CYCLES_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_LIMIT_CYCLES_TR, ""},  /* fuzzy */
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AND_AUTOSTART,    N_("Attach and autostart tape/disk image <name>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AND_AUTOSTART_DA, "Tilslut og autostart bånd-/disk-image <navn>"},
/* de */ {IDCLS_ATTACH_AND_AUTOSTART_DE, "Einlegen und Autostart Disk/Band Imagedatei <Name>"},
/* es */ {IDCLS_ATTACH_AND_AUTOSTART_ES, "Insertar y autoarrancar imagen de cinta/disco <nombre>"},
/* fr */ {IDCLS_ATTACH_AND_AUTOSTART_FR, "Insérer et autocharger l'image de disque/datassette <nom>"},
/* hu */ {IDCLS_ATTACH_AND_AUTOSTART_HU, "<név> lemez/szalag képmás csatolása és automatikus elindítása"},
/* it */ {IDCLS_ATTACH_AND_AUTOSTART_IT, "Seleziona ed avvia l'immagine di una cassetta/disco <nome>"},
/* ko */ {IDCLS_ATTACH_AND_AUTOSTART_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AND_AUTOSTART_NL, "Koppel en autostart tape/diskbestand <naam>"},
/* pl */ {IDCLS_ATTACH_AND_AUTOSTART_PL, "Zamontuj i automatycznie uruchom obraz ta¶my lub dysku <nazwa>"},
/* ru */ {IDCLS_ATTACH_AND_AUTOSTART_RU, "Attach and autostart tape/disk image <name>"},
/* sv */ {IDCLS_ATTACH_AND_AUTOSTART_SV, "Anslut och starta band-/diskettavbildning <namn>"},
/* tr */ {IDCLS_ATTACH_AND_AUTOSTART_TR, "Teyp/Disk imajý <isim> yerleþtir ve otomatik baþlat"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AND_AUTOLOAD,    N_("Attach and autoload tape/disk image <name>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AND_AUTOLOAD_DA, "Tilslut og læs bånd-/disk-image <navn>"},
/* de */ {IDCLS_ATTACH_AND_AUTOLOAD_DE, "Einlegen und Autoload Disk/Band Imagedatei <Name>"},
/* es */ {IDCLS_ATTACH_AND_AUTOLOAD_ES, "Insertar y autocargar imagen de cinta/disco <nombre>"},
/* fr */ {IDCLS_ATTACH_AND_AUTOLOAD_FR, "Insérer et charger l'image de disque/datassette <nom>"},
/* hu */ {IDCLS_ATTACH_AND_AUTOLOAD_HU, "<név> lemez/szalag képmás csatolása és automatikus betöltése"},
/* it */ {IDCLS_ATTACH_AND_AUTOLOAD_IT, "Seleziona e carica l'immagine di una cassetta/disco <nome>"},
/* ko */ {IDCLS_ATTACH_AND_AUTOLOAD_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AND_AUTOLOAD_NL, "Koppel en autolaad tape-/diskbestand <naam>"},
/* pl */ {IDCLS_ATTACH_AND_AUTOLOAD_PL, "Zamontuj i automatycznie wczytaj obraz ta¶my lub dysku <nazwa>"},
/* ru */ {IDCLS_ATTACH_AND_AUTOLOAD_RU, "Attach and autoload tape/disk image <name>"},
/* sv */ {IDCLS_ATTACH_AND_AUTOLOAD_SV, "Anslut och läs in band-/diskettavbildning <namn>"},
/* tr */ {IDCLS_ATTACH_AND_AUTOLOAD_TR, "Teyp/Disk imajý <isim> yerleþtir ve otomatik yükle"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_TAPE,    N_("Attach <name> as a tape image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AS_TAPE_DA, "Tilslut <navn> som bånd-image"},
/* de */ {IDCLS_ATTACH_AS_TAPE_DE, "Imagedatei <Name> einlegen"},
/* es */ {IDCLS_ATTACH_AS_TAPE_ES, "Insertar <nombre> como imagen de cinta"},
/* fr */ {IDCLS_ATTACH_AS_TAPE_FR, "Insérer <nom> comme image de datassette"},
/* hu */ {IDCLS_ATTACH_AS_TAPE_HU, "<név> szalag képmás csatolása"},
/* it */ {IDCLS_ATTACH_AS_TAPE_IT, "Seleziona <nome> come un'immagine di una cassetta"},
/* ko */ {IDCLS_ATTACH_AS_TAPE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AS_TAPE_NL, "Koppel <naam> als een tapebestand"},
/* pl */ {IDCLS_ATTACH_AS_TAPE_PL, "Zamontuj <nazwa> jako obraz ta¶my"},
/* ru */ {IDCLS_ATTACH_AS_TAPE_RU, "Attach <name> as a tape image"},
/* sv */ {IDCLS_ATTACH_AS_TAPE_SV, "Anslut <namn> som bandavbildning"},
/* tr */ {IDCLS_ATTACH_AS_TAPE_TR, "Teyp imajý olarak <isim> yerleþtir"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_8,    N_("Attach <name> as a disk image in drive #8")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AS_DISK_8_DA, "Tilslut <navn> som disk-image i enhed #8"},
/* de */ {IDCLS_ATTACH_AS_DISK_8_DE, "Imagedatei <Name> im Laufwerk #8 einlegen"},
/* es */ {IDCLS_ATTACH_AS_DISK_8_ES, "Insertar <nombre> como imagen disco en unidad disco #8"},
/* fr */ {IDCLS_ATTACH_AS_DISK_8_FR, "Insérer <nom> comme image de disque dans le lecteur #8"},
/* hu */ {IDCLS_ATTACH_AS_DISK_8_HU, "<név> lemezképmás csatolása #8-as egységként"},
/* it */ {IDCLS_ATTACH_AS_DISK_8_IT, "Seleziona <nome> come un'immagine di un disco nel drive #8"},
/* ko */ {IDCLS_ATTACH_AS_DISK_8_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AS_DISK_8_NL, "Koppel <naam> aan als een schijfbestand in drive #8"},
/* pl */ {IDCLS_ATTACH_AS_DISK_8_PL, "Zamontuj <nazwa> jako obraz dyskietki w stacji #8"},
/* ru */ {IDCLS_ATTACH_AS_DISK_8_RU, "Attach <name> as a disk image in drive #8"},
/* sv */ {IDCLS_ATTACH_AS_DISK_8_SV, "Anslut <namn> som diskettavbildning i enhet 8"},
/* tr */ {IDCLS_ATTACH_AS_DISK_8_TR, "Sürücü #8 disk imajý olarak <isim> yerleþtir"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_9,    N_("Attach <name> as a disk image in drive #9")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AS_DISK_9_DA, "Tilslut <navn> som disk-image i enhed #9"},
/* de */ {IDCLS_ATTACH_AS_DISK_9_DE, "Imagedatei <Name> im Laufwerk #9 einlegen"},
/* es */ {IDCLS_ATTACH_AS_DISK_9_ES, "Insertar <nombre> como imagen disco en unidad disco #9"},
/* fr */ {IDCLS_ATTACH_AS_DISK_9_FR, "Insérer <nom> comme image de disque dans le lecteur #9"},
/* hu */ {IDCLS_ATTACH_AS_DISK_9_HU, "<név> lemezképmás csatolása #9-es egységként"},
/* it */ {IDCLS_ATTACH_AS_DISK_9_IT, "Seleziona <nome> come un'immagine di un disco nel drive #9"},
/* ko */ {IDCLS_ATTACH_AS_DISK_9_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AS_DISK_9_NL, "Koppel <naam> aan als een schijfbestand in drive #9"},
/* pl */ {IDCLS_ATTACH_AS_DISK_9_PL, "Zamontuj <nazwa> jako obraz dyskietki w stacji #9"},
/* ru */ {IDCLS_ATTACH_AS_DISK_9_RU, "Attach <name> as a disk image in drive #9"},
/* sv */ {IDCLS_ATTACH_AS_DISK_9_SV, "Anslut <namn> som diskettavbildning i enhet 9"},
/* tr */ {IDCLS_ATTACH_AS_DISK_9_TR, "Sürücü #9 disk imajý olarak <isim> yerleþtir"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_10,    N_("Attach <name> as a disk image in drive #10")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AS_DISK_10_DA, "Tilslut <navn> som disk-image i enhed #10"},
/* de */ {IDCLS_ATTACH_AS_DISK_10_DE, "Imagedatei <Name> im Laufwerk #10 einlegen"},
/* es */ {IDCLS_ATTACH_AS_DISK_10_ES, "Insertar <nombre> como imagen disco en unidad disco #10"},
/* fr */ {IDCLS_ATTACH_AS_DISK_10_FR, "Insérer <nom> comme image de disque dans le lecteur #10"},
/* hu */ {IDCLS_ATTACH_AS_DISK_10_HU, "<név> lemezképmás csatolása #10-es egységként"},
/* it */ {IDCLS_ATTACH_AS_DISK_10_IT, "Seleziona <nome> come un'immagine di un disco nel drive #10"},
/* ko */ {IDCLS_ATTACH_AS_DISK_10_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AS_DISK_10_NL, "Koppel <naam> aan als een schijfbestand in drive #10"},
/* pl */ {IDCLS_ATTACH_AS_DISK_10_PL, "Zamontuj <nazwa> jako obraz dyskietki w stacji #10"},
/* ru */ {IDCLS_ATTACH_AS_DISK_10_RU, "Attach <name> as a disk image in drive #10"},
/* sv */ {IDCLS_ATTACH_AS_DISK_10_SV, "Anslut <namn> som diskettavbildning i enhet 10"},
/* tr */ {IDCLS_ATTACH_AS_DISK_10_TR, "Sürücü #10 disk imajý olarak <isim> yerleþtir"},
#endif

/* initcmdline.c */
/* en */ {IDCLS_ATTACH_AS_DISK_11,    N_("Attach <name> as a disk image in drive #11")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_AS_DISK_11_DA, "Tilslut <navn> som disk-image i enhed #11"},
/* de */ {IDCLS_ATTACH_AS_DISK_11_DE, "Imagedatei <Name> im Laufwerk #11 einlegen"},
/* es */ {IDCLS_ATTACH_AS_DISK_11_ES, "Insertar <nombre> como imagen disco en unidad disco #11"},
/* fr */ {IDCLS_ATTACH_AS_DISK_11_FR, "Insérer <nom> comme image de disque dans le lecteur #11"},
/* hu */ {IDCLS_ATTACH_AS_DISK_11_HU, "<név> lemezképmás csatolása #11-es egységként"},
/* it */ {IDCLS_ATTACH_AS_DISK_11_IT, "Seleziona <nome> come un'immagine di un disco nel drive #11"},
/* ko */ {IDCLS_ATTACH_AS_DISK_11_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_AS_DISK_11_NL, "Koppel <naam> aan als een schijfbestand in drive #11"},
/* pl */ {IDCLS_ATTACH_AS_DISK_11_PL, "Zamontuj <nazwa> jako obraz dyskietki w stacji #11"},
/* ru */ {IDCLS_ATTACH_AS_DISK_11_RU, "Attach <name> as a disk image in drive #11"},
/* sv */ {IDCLS_ATTACH_AS_DISK_11_SV, "Anslut <namn> som diskettavbildning i enhet 11"},
/* tr */ {IDCLS_ATTACH_AS_DISK_11_TR, "Sürücü #11 disk imajý olarak <isim> yerleþtir"},
#endif

/* kbdbuf.c */
/* en */ {IDCLS_P_STRING,    N_("<string>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_STRING_DA, "<streng>"},
/* de */ {IDCLS_P_STRING_DE, "<String>"},
/* es */ {IDCLS_P_STRING_ES, "<cadena>"},
/* fr */ {IDCLS_P_STRING_FR, "<chaine>"},
/* hu */ {IDCLS_P_STRING_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_STRING_IT, "<stringa>"},
/* ko */ {IDCLS_P_STRING_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_STRING_NL, "<tekenreeks>"},
/* pl */ {IDCLS_P_STRING_PL, "<string>"},
/* ru */ {IDCLS_P_STRING_RU, "<string>"},
/* sv */ {IDCLS_P_STRING_SV, "<sträng>"},
/* tr */ {IDCLS_P_STRING_TR, "<yazý>"},
#endif

/* kbdbuf.c */
/* en */ {IDCLS_PUT_STRING_INTO_KEYBUF,    N_("Put the specified string into the keyboard buffer.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PUT_STRING_INTO_KEYBUF_DA, "Indsæt den angivne streng i tastatur-bufferen."},
/* de */ {IDCLS_PUT_STRING_INTO_KEYBUF_DE, "Definierte Eingabe in Tastaturpuffer bereitstellen."},
/* es */ {IDCLS_PUT_STRING_INTO_KEYBUF_ES, "Introduce la cadena indicada en el búfer de teclado."},
/* fr */ {IDCLS_PUT_STRING_INTO_KEYBUF_FR, "Placer la chaîne spécifiée dans le tampon clavier."},
/* hu */ {IDCLS_PUT_STRING_INTO_KEYBUF_HU, "A megadott sztring elhelyezése a billentyûzet pufferben."},
/* it */ {IDCLS_PUT_STRING_INTO_KEYBUF_IT, "Metti la stringa specificata nel buffer di tastiera."},
/* ko */ {IDCLS_PUT_STRING_INTO_KEYBUF_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PUT_STRING_INTO_KEYBUF_NL, "Plaats de opgegeven tekenreeks in de toetsenbordbuffer."},
/* pl */ {IDCLS_PUT_STRING_INTO_KEYBUF_PL, "Umie¶æ dany ci±g znaków w buforze klawiatury"},
/* ru */ {IDCLS_PUT_STRING_INTO_KEYBUF_RU, "Put the specified string into the keyboard buffer."},
/* sv */ {IDCLS_PUT_STRING_INTO_KEYBUF_SV, "Lägg den angivna strängen i tangentbordsbufferten."},
/* tr */ {IDCLS_PUT_STRING_INTO_KEYBUF_TR, "Klavye arabelleðine belirtilen yazýyý koy."},
#endif

/* autostart.c */
/* en */ {IDCLS_SET_KEYBUF_DELAY,    N_("Set additional keyboard buffer delay (0: use default)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_KEYBUF_DELAY_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_KEYBUF_DELAY_DE, "Setze zusätzliche Tastaturbuffer Verzögerung (0: benutze Standard)"},
/* es */ {IDCLS_SET_KEYBUF_DELAY_ES, "Seleccionar retardo adicional buffer de teclado (0: Por defecto)"},
/* fr */ {IDCLS_SET_KEYBUF_DELAY_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_KEYBUF_DELAY_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_KEYBUF_DELAY_IT, "Imposta il ritardo supplementare del buffer di tastiera (0: usa predefinito)"},
/* ko */ {IDCLS_SET_KEYBUF_DELAY_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_KEYBUF_DELAY_NL, "Stel extra toetsenbord buffer vertraging in (0: gebruik de standaard waarde)"},
/* pl */ {IDCLS_SET_KEYBUF_DELAY_PL, "Okre¶l dodatkowe opó¼nienie bufora klawiatury (0: domy¶lne)"},
/* ru */ {IDCLS_SET_KEYBUF_DELAY_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_KEYBUF_DELAY_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SET_KEYBUF_DELAY_TR, ""},  /* fuzzy */
#endif

/* log.c */
/* en */ {IDCLS_SPECIFY_LOG_FILE_NAME,    N_("Specify log file name")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_LOG_FILE_NAME_DA, "Angiv navn på logfil"},
/* de */ {IDCLS_SPECIFY_LOG_FILE_NAME_DE, "Logdateiname wählen"},
/* es */ {IDCLS_SPECIFY_LOG_FILE_NAME_ES, "Especificar nombre fichero registro"},
/* fr */ {IDCLS_SPECIFY_LOG_FILE_NAME_FR, "Spécifier le nom du fichier log"},
/* hu */ {IDCLS_SPECIFY_LOG_FILE_NAME_HU, "Adja meg a naplófájl nevét"},
/* it */ {IDCLS_SPECIFY_LOG_FILE_NAME_IT, "Specifica il nome del file di log"},
/* ko */ {IDCLS_SPECIFY_LOG_FILE_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_LOG_FILE_NAME_NL, "Geef de naam van het logbestand"},
/* pl */ {IDCLS_SPECIFY_LOG_FILE_NAME_PL, "Okre¶l nazwê pliku z logiem"},
/* ru */ {IDCLS_SPECIFY_LOG_FILE_NAME_RU, "Specify log file name"},
/* sv */ {IDCLS_SPECIFY_LOG_FILE_NAME_SV, "Ange namn på loggfil"},
/* tr */ {IDCLS_SPECIFY_LOG_FILE_NAME_TR, "Log dosyasý ismini belirt"},
#endif

/* log.c */
/* en */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT,    N_("Enable verbose log output.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT_DA, "Aktivér omstændeligt log-output"},
/* de */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT_DE, "Ausführliche Diagnose aktivieren."},
/* es */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT_ES, "Permitir salida escrita registro."},
/* fr */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT_FR, "Activer la journalisation verbeuse"},
/* hu */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT_IT, "Attiva output dei log verboso."},
/* ko */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT_NL, "Uitgebreide log uitvoer inschakelen."},
/* pl */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT_PL, "W³±cz szczegó³owy zapis do loga"},
/* ru */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT_RU, "Enable verbose log output."},
/* sv */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT_SV, "Aktivera pratsam loggutdata."},
/* tr */ {IDCLS_ENABLE_VERBOSE_LOG_OUTPUT_TR, "Verbose log çýktýsýný aktif et."},
#endif

/* log.c */
/* en */ {IDCLS_DISABLE_LOG_OUTPUT,    N_("Disable verbose log output.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_LOG_OUTPUT_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_LOG_OUTPUT_DE, "Ausführliche Diagnose deaktivieren."},
/* es */ {IDCLS_DISABLE_LOG_OUTPUT_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_LOG_OUTPUT_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_LOG_OUTPUT_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_LOG_OUTPUT_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_LOG_OUTPUT_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_LOG_OUTPUT_NL, ""},  /* fuzzy */
/* pl */ {IDCLS_DISABLE_LOG_OUTPUT_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_DISABLE_LOG_OUTPUT_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_LOG_OUTPUT_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_LOG_OUTPUT_TR, ""},  /* fuzzy */
#endif

/* ps2mouse.c */
/* en */ {IDCLS_ENABLE_PS2MOUSE,    N_("Enable PS/2 mouse on userport")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_PS2MOUSE_DA, "Aktivér PS/2-mus på brugerporten"},
/* de */ {IDCLS_ENABLE_PS2MOUSE_DE, "PS/2 Userport Emulation aktivieren"},
/* es */ {IDCLS_ENABLE_PS2MOUSE_ES, "Permitir ratón PS/2 en puerto usuario"},
/* fr */ {IDCLS_ENABLE_PS2MOUSE_FR, "Activer souris PS/2 sur port utilisateur"},
/* hu */ {IDCLS_ENABLE_PS2MOUSE_HU, "PS/2 egér engedélyezése a userporton"},
/* it */ {IDCLS_ENABLE_PS2MOUSE_IT, "Attiva mouse PS/2 su userport"},
/* ko */ {IDCLS_ENABLE_PS2MOUSE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_PS2MOUSE_NL, "Emulatie van een PS/2 muis op de userport inschakelen"},
/* pl */ {IDCLS_ENABLE_PS2MOUSE_PL, "W³±cz mysz PS/2 w userporcie"},
/* ru */ {IDCLS_ENABLE_PS2MOUSE_RU, "Enable PS/2 mouse on userport"},
/* sv */ {IDCLS_ENABLE_PS2MOUSE_SV, "Aktivera PS/2-mus på användarport"},
/* tr */ {IDCLS_ENABLE_PS2MOUSE_TR, "Userport üzerinde PS/2 mouse'u aktif et"},
#endif

/* ps2mouse.c */
/* en */ {IDCLS_DISABLE_PS2MOUSE,    N_("Disable PS/2 mouse on userport")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_PS2MOUSE_DA, "Deaktivér PS/2-mus på brugerporten"},
/* de */ {IDCLS_DISABLE_PS2MOUSE_DE, "PS/2 Userport Emulation deaktivieren\""},
/* es */ {IDCLS_DISABLE_PS2MOUSE_ES, "Deshabilitar ratón PS/2 en puerto usuario"},
/* fr */ {IDCLS_DISABLE_PS2MOUSE_FR, "Désactiver la souris PS/2 sur port utilisateur"},
/* hu */ {IDCLS_DISABLE_PS2MOUSE_HU, "PS/2 egér tiltása a userporton"},
/* it */ {IDCLS_DISABLE_PS2MOUSE_IT, "Disattiva mouse PS/2 su userport"},
/* ko */ {IDCLS_DISABLE_PS2MOUSE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_PS2MOUSE_NL, "Emulatie van een PS/2 muis op de userport uitschakelen"},
/* pl */ {IDCLS_DISABLE_PS2MOUSE_PL, "Wy³±cz mysz PS/2 w userporcie"},
/* ru */ {IDCLS_DISABLE_PS2MOUSE_RU, "Disable PS/2 mouse on userport"},
/* sv */ {IDCLS_DISABLE_PS2MOUSE_SV, "Inaktivera PS/2-mus på användarport"},
/* tr */ {IDCLS_DISABLE_PS2MOUSE_TR, "Userport üzerinde PS/2 mouse'u pasifleþtir"},
#endif

/* ram.c */
/* en */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE,    N_("Set the value for the very first RAM address after powerup")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_DA, "Angiv værdien for den allerførste RAM adresse efter power-up"},
/* de */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_DE, "Wert für erstes Byte im RAM nach Kaltstart setzen"},
/* es */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_ES, "Seleccionar el valor para la primera dirección RAM después del encendido"},
/* fr */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_FR, "Spécifier la valeur de la première adresse RAM après la mise sous tension"},
/* hu */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_HU, "Adja meg a legelsõ RAM cím értékét bekapcsolás után"},
/* it */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_IT, "Imposta il valore del primissimo indirizzo della RAM dopo l'accensione"},
/* ko */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_NL, "Stel de waarde in voor het allereerste RAM adres na koude start"},
/* pl */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_PL, "Okre¶l warto¶æ pierwszego adresu RAM po uruchomieniu"},
/* ru */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_RU, "Set the value for the very first RAM address after powerup"},
/* sv */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_SV, "Ange värdet på den allra första RAM-adressen efter strömpåslag"},
/* tr */ {IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_TR, "Güç verildiðinde baþlangýçtaki RAM adreslerine atanacak deðeri belirle"},
#endif

/* ram.c */
/* en */ {IDCLS_P_NUM_OF_BYTES,    N_("<num of bytes>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_NUM_OF_BYTES_DA, "<antal bytes>"},
/* de */ {IDCLS_P_NUM_OF_BYTES_DE, "<Anzahl an Bytes>"},
/* es */ {IDCLS_P_NUM_OF_BYTES_ES, "<número de bytes>"},
/* fr */ {IDCLS_P_NUM_OF_BYTES_FR, "<nb. d'octets>"},
/* hu */ {IDCLS_P_NUM_OF_BYTES_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_NUM_OF_BYTES_IT, "<numero di byte>"},
/* ko */ {IDCLS_P_NUM_OF_BYTES_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_NUM_OF_BYTES_NL, "<aantal bytes>"},
/* pl */ {IDCLS_P_NUM_OF_BYTES_PL, "<num of bytes>"},
/* ru */ {IDCLS_P_NUM_OF_BYTES_RU, "<num of bytes>"},
/* sv */ {IDCLS_P_NUM_OF_BYTES_SV, "<antal byte>"},
/* tr */ {IDCLS_P_NUM_OF_BYTES_TR, "<byte sayýsý>"},
#endif

/* ram.c */
/* en */ {IDCLS_LENGTH_BLOCK_SAME_VALUE,    N_("Length of memory block initialized with the same value")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_DA, "Længde på hukommelsesblok som initialiseres med samme værdi"},
/* de */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_DE, "Länge des Speicherblocks der mit dem gleichen Wert initialisiert ist"},
/* es */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_ES, "Longitud del bloque de memoria inicializado con el mismo valor"},
/* fr */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_FR, "Longeur du premier bloc mémoire initialisé avec la même valeur"},
/* hu */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_HU, "Azonos értékkel feltöltött memória blokkoknak a hossza"},
/* it */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_IT, "Lunghezza del blocco di memoria inizializzato con lo stesso valore"},
/* ko */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_NL, "Geheugenblokgrootte die dezelfde waarde krijgt bij initialisatie"},
/* pl */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_PL, "D³ugo¶æ bloku pamiêci zainicjowanego z t± sam± warto¶ci±"},
/* ru */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_RU, "Length of memory block initialized with the same value"},
/* sv */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_SV, "Längd på minnesblock som initierats med samma värde"},
/* tr */ {IDCLS_LENGTH_BLOCK_SAME_VALUE_TR, "Ayný deðerle balangýç ayarlarý yapýlan bellek bloðu uzunluðu"},
#endif

/* ram.c */
/* en */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN,    N_("Length of memory block initialized with the same pattern")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_DA, "Længde på hukommelsesblok som initialiseres med samme mønster"},
/* de */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_DE, "Länge des Speicherblocks der mit dem gleichen Muster initialisiert ist"},
/* es */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_ES, "Longitud bloque memoria inicializado con el mismo patrón"},
/* fr */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_FR, "Longeur du premier bloc mémoire initialisé avec le même pattern"},
/* hu */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_HU, "Azonos mintával feltöltött memória blokkoknak a hossza"},
/* it */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_IT, "Lunghezza del blocco di memoria inizializzato con lo stesso pattern"},
/* ko */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_NL, "Geheugenblokgrootte met hetzelfde patroon bij initialisatie"},
/* pl */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_PL, "D³ugo¶æ bloku pamiêci zainicjowanego z tym samym wzorcem"},
/* ru */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_RU, "Length of memory block initialized with the same pattern"},
/* sv */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_SV, "Längd på minnesblock som initierats med samma mönster"},
/* tr */ {IDCLS_LENGTH_BLOCK_SAME_PATTERN_TR, "Ayný desenle balangýç ayarlarý yapýlan bellek bloðu uzunluðu"},
#endif

/* sampler/sampler.c */
/* en */ {IDCLS_P_GAIN,    N_("<Sampler gain>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_GAIN_DA, ""},  /* fuzzy */
/* de */ {IDCLS_P_GAIN_DE, "<Sampler Gain>"},
/* es */ {IDCLS_P_GAIN_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_P_GAIN_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_P_GAIN_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_GAIN_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_P_GAIN_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_GAIN_NL, "<Sampler toename>"},
/* pl */ {IDCLS_P_GAIN_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_P_GAIN_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_P_GAIN_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_P_GAIN_TR, ""},  /* fuzzy */
#endif

/* sampler/sampler.c */
/* en */ {IDCLS_SAMPLER_GAIN_IN_PERCENT,    N_("Specify sampler gain in percent (1%-200%)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SAMPLER_GAIN_IN_PERCENT_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SAMPLER_GAIN_IN_PERCENT_DE, "Sampler Gain in Prozent (1% - 200%)"},
/* es */ {IDCLS_SAMPLER_GAIN_IN_PERCENT_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SAMPLER_GAIN_IN_PERCENT_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SAMPLER_GAIN_IN_PERCENT_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SAMPLER_GAIN_IN_PERCENT_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SAMPLER_GAIN_IN_PERCENT_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SAMPLER_GAIN_IN_PERCENT_NL, "Geef sampler versterking in procent (1% - 200%)"},
/* pl */ {IDCLS_SAMPLER_GAIN_IN_PERCENT_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_SAMPLER_GAIN_IN_PERCENT_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SAMPLER_GAIN_IN_PERCENT_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SAMPLER_GAIN_IN_PERCENT_TR, ""},  /* fuzzy */
#endif

/* sysfile.c */
/* en */ {IDCLS_P_PATH,    N_("<Path>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_PATH_DA, "<Sti>"},
/* de */ {IDCLS_P_PATH_DE, "<Pfad>"},
/* es */ {IDCLS_P_PATH_ES, "<Camino>"},
/* fr */ {IDCLS_P_PATH_FR, "<Emplacement>"},
/* hu */ {IDCLS_P_PATH_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_PATH_IT, "<Percorso>"},
/* ko */ {IDCLS_P_PATH_KO, "<±æ>"},
/* nl */ {IDCLS_P_PATH_NL, "<Pad>"},
/* pl */ {IDCLS_P_PATH_PL, "<¦cie¿ka>"},
/* ru */ {IDCLS_P_PATH_RU, "<¿ãâì>"},
/* sv */ {IDCLS_P_PATH_SV, "<Sökväg>"},
/* tr */ {IDCLS_P_PATH_TR, "<Yol>"},
#endif

/* sysfile.c */
/* en */ {IDCLS_DEFINE_SYSTEM_FILES_PATH,    N_("Define search path to locate system files")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_DA, "Angiv sti til systemfiler"},
/* de */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_DE, "Suchpfad für Systemdateien wählen"},
/* es */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_ES, "Definir camino de búsqueda para los ficheros locales de sistema"},
/* fr */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_FR, "Définir le chemin de recherche pour trouver les fichiers systèmes"},
/* hu */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_HU, "Keresési útvonal megadása a rendszerfájlok megtalálására"},
/* it */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_IT, "Definisci il path di ricerca per cercare i file di sistema"},
/* ko */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_NL, "Geef het zoek pad waar de systeem bestanden te vinden zijn"},
/* pl */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_PL, "Definiuj ¶cie¿kê wyszukiwania plików systemowych"},
/* ru */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_RU, "Define search path to locate system files"},
/* sv */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_SV, "Ange sökväg för att hitta systemfiler"},
/* tr */ {IDCLS_DEFINE_SYSTEM_FILES_PATH_TR, "Sistem dosyalarýnýn konumunu belirlemek için arama yolu tanýmlayýn"},
#endif

/* translate.c */
/* en */ {IDCLS_P_ISO_LANGUAGE_CODE,    N_("<iso language code>")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_P_ISO_LANGUAGE_CODE_DA, "<iso-sprogkode>"},
/* de */ {IDCLS_P_ISO_LANGUAGE_CODE_DE, "<iso Sprachcode>"},
/* es */ {IDCLS_P_ISO_LANGUAGE_CODE_ES, "<código ISO idioma>"},
/* fr */ {IDCLS_P_ISO_LANGUAGE_CODE_FR, "<code language ISO>"},
/* hu */ {IDCLS_P_ISO_LANGUAGE_CODE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_P_ISO_LANGUAGE_CODE_IT, "<codice iso lingua>"},
/* ko */ {IDCLS_P_ISO_LANGUAGE_CODE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_P_ISO_LANGUAGE_CODE_NL, "<iso taalcode>"},
/* pl */ {IDCLS_P_ISO_LANGUAGE_CODE_PL, "<Kod jêzykowy iso>"},
/* ru */ {IDCLS_P_ISO_LANGUAGE_CODE_RU, "<iso language code>"},
/* sv */ {IDCLS_P_ISO_LANGUAGE_CODE_SV, "<iso-språkkod>"},
/* tr */ {IDCLS_P_ISO_LANGUAGE_CODE_TR, "<iso dil kodu>"},
#endif

/* c64/plus256k.c */
/* en */ {IDCLS_SPECIFY_PLUS256K_NAME,    N_("Specify name of PLUS256K image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PLUS256K_NAME_DA, "Angiv navn på PLUS256K-image"},
/* de */ {IDCLS_SPECIFY_PLUS256K_NAME_DE, "Name der PLUS256K Imagedatei wählen"},
/* es */ {IDCLS_SPECIFY_PLUS256K_NAME_ES, "Especificar nombre imagen RAM PLUS256K"},
/* fr */ {IDCLS_SPECIFY_PLUS256K_NAME_FR, "Spécifier le nom de l'image PLUS256K RAM"},
/* hu */ {IDCLS_SPECIFY_PLUS256K_NAME_HU, "Adja meg a PLUS256K-s képmás nevét"},
/* it */ {IDCLS_SPECIFY_PLUS256K_NAME_IT, "Specifica il nome immagine PLUS256K"},
/* ko */ {IDCLS_SPECIFY_PLUS256K_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PLUS256K_NAME_NL, "Geef de naam van het PLUS256K bestand"},
/* pl */ {IDCLS_SPECIFY_PLUS256K_NAME_PL, "Okre¶l nazwê obrazu PLUS256K"},
/* ru */ {IDCLS_SPECIFY_PLUS256K_NAME_RU, "Specify name of PLUS256K image"},
/* sv */ {IDCLS_SPECIFY_PLUS256K_NAME_SV, "Ange namn på PLUS256K-avbildning"},
/* tr */ {IDCLS_SPECIFY_PLUS256K_NAME_TR, "PLUS256K imajýnýn ismini belirt"},
#endif

/* c64/plus60k.c */
/* en */ {IDCLS_SPECIFY_PLUS60K_NAME,    N_("Specify name of PLUS60K image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_PLUS60K_NAME_DA, "Angiv navn på PLUS60K-image"},
/* de */ {IDCLS_SPECIFY_PLUS60K_NAME_DE, "Name der PLUS60K Imagedatei wählen"},
/* es */ {IDCLS_SPECIFY_PLUS60K_NAME_ES, "Especificar nombre imagen PLUS60K"},
/* fr */ {IDCLS_SPECIFY_PLUS60K_NAME_FR, "Spécifier le nom de l'image PLUS60K RAM"},
/* hu */ {IDCLS_SPECIFY_PLUS60K_NAME_HU, "Adja meg a nevét a PLUS60K képmásnak"},
/* it */ {IDCLS_SPECIFY_PLUS60K_NAME_IT, "Specifica il nome immagine PLUS60K"},
/* ko */ {IDCLS_SPECIFY_PLUS60K_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_PLUS60K_NAME_NL, "Geef de naam van het PLUS60K bestand"},
/* pl */ {IDCLS_SPECIFY_PLUS60K_NAME_PL, "Okre¶l nazwê obrazu PLUS60K"},
/* ru */ {IDCLS_SPECIFY_PLUS60K_NAME_RU, "Specify name of PLUS60K image"},
/* sv */ {IDCLS_SPECIFY_PLUS60K_NAME_SV, "Ange namn på plUS60K-avbildning"},
/* tr */ {IDCLS_SPECIFY_PLUS60K_NAME_TR, "PLUS60K imajýnýn ismini belirt"},
#endif

/* c64/plus60k.c */
/* en */ {IDCLS_PLUS60K_BASE,    N_("Base address of the PLUS60K expansion. (0xD040/0xD100)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_PLUS60K_BASE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_PLUS60K_BASE_DE, "Basis Adresse für PLUS60K Erweiterung. (0xD040/0xD100)"},
/* es */ {IDCLS_PLUS60K_BASE_ES, "Dirección base de la expansión PLUS60K. (0xD040/0xD100)"},
/* fr */ {IDCLS_PLUS60K_BASE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_PLUS60K_BASE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_PLUS60K_BASE_IT, "Indirizzo base espansione PLUS60K. (0xD040/0xD100)"},
/* ko */ {IDCLS_PLUS60K_BASE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_PLUS60K_BASE_NL, "Basisadres van de PLUS60K geheugenuitbreiding. (0xD040/0xD100)"},
/* pl */ {IDCLS_PLUS60K_BASE_PL, "Adres bazowy rozszerzenia PLUS60K. (0xD040/0xD100)"},
/* ru */ {IDCLS_PLUS60K_BASE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_PLUS60K_BASE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_PLUS60K_BASE_TR, ""},  /* fuzzy */
#endif

/* c64/c64_256k.c */
/* en */ {IDCLS_SPECIFY_C64_256K_NAME,    N_("Specify name of 256K image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_C64_256K_NAME_DA, "Angiv navn på 256k-image"},
/* de */ {IDCLS_SPECIFY_C64_256K_NAME_DE, "Name der 256K Imagedatei wählen"},
/* es */ {IDCLS_SPECIFY_C64_256K_NAME_ES, "Especificar nombre imagen 256K"},
/* fr */ {IDCLS_SPECIFY_C64_256K_NAME_FR, "Spécifier le nom de l'image 256K"},
/* hu */ {IDCLS_SPECIFY_C64_256K_NAME_HU, "Adja meg a 256K-s képmás nevét"},
/* it */ {IDCLS_SPECIFY_C64_256K_NAME_IT, "Specifica il nome immagine 256K"},
/* ko */ {IDCLS_SPECIFY_C64_256K_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_C64_256K_NAME_NL, "Geef de naam van het 256K bestand"},
/* pl */ {IDCLS_SPECIFY_C64_256K_NAME_PL, "Okre¶l nazwê obrazu 256K"},
/* ru */ {IDCLS_SPECIFY_C64_256K_NAME_RU, "Specify name of 256K image"},
/* sv */ {IDCLS_SPECIFY_C64_256K_NAME_SV, "Ange namn på 256K-avbildning"},
/* tr */ {IDCLS_SPECIFY_C64_256K_NAME_TR, "256K imajýnýn ismini belirt"},
#endif

/* c64/c64_256k.c */
/* en */ {IDCLS_C64_256K_BASE,    N_("Base address of the 256K expansion. (0xDE00/0xDE80/0xDF00/0xDF80)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_C64_256K_BASE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_C64_256K_BASE_DE, "Basis Adresse für 256K RAM Erweiterung. (0xDE00/0xDE80/0xDF00/0xDF80)"},
/* es */ {IDCLS_C64_256K_BASE_ES, "Dirección base de la expansión 256K. (0xDE00/0xDE80/0xDF00/0xDF80)"},
/* fr */ {IDCLS_C64_256K_BASE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_C64_256K_BASE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_C64_256K_BASE_IT, "Indirizzo base dell'espansione 256K. (0xDE00/0xDE80/0xDF00/0xDF80)"},
/* ko */ {IDCLS_C64_256K_BASE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_C64_256K_BASE_NL, "Basisadres van de 256K geheugenuitbreiding. (0xDE00/0xDE80/0xDF00/0xDF80)"},
/* pl */ {IDCLS_C64_256K_BASE_PL, "Adres bazowy rozszerzenia 256K. (0xDE00/0xDE80/0xDF00/0xDF80)"},
/* ru */ {IDCLS_C64_256K_BASE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_C64_256K_BASE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_C64_256K_BASE_TR, ""},  /* fuzzy */
#endif

/* c64/c64-memory-hacks.c */
/* en */ {IDCLS_SET_C64_MEMORY_HACK,    N_("Set the 'memory expansion hack' device (0: None, 1: C64 256K, 2: +60K, 3: +256K)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_C64_MEMORY_HACK_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_C64_MEMORY_HACK_DE, "Speichererweiterungshack Gerät (0: kein, 1: C64 256K, 2: +60K, 3: +256K)"},
/* es */ {IDCLS_SET_C64_MEMORY_HACK_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_C64_MEMORY_HACK_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_C64_MEMORY_HACK_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_C64_MEMORY_HACK_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SET_C64_MEMORY_HACK_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_C64_MEMORY_HACK_NL, "Stel het 'geheugen uitbreidingen hack' apparaat in (0: Geen, 1: C64 256K, 2: +60K, 3: +256K)"},
/* pl */ {IDCLS_SET_C64_MEMORY_HACK_PL, "Ustaw urz±dzenie 'hacka' rozszerzenia pamiêci (0: Brak, 1: 256K C64, 2: +60K, 3: +256K)"},
/* ru */ {IDCLS_SET_C64_MEMORY_HACK_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_C64_MEMORY_HACK_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SET_C64_MEMORY_HACK_TR, ""},  /* fuzzy */
#endif

/* plus4/plus4memhacks.c */
/* en */ {IDCLS_SET_PLUS4_MEMORY_HACK,    N_("Set the 'memory expansion hack' device (0: None, 1: CSORY 256K, 2: HANNES 256K, 3: HANNES 1024K, 4: HANNES 4096K)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_PLUS4_MEMORY_HACK_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_PLUS4_MEMORY_HACK_DE, "Speicher Erweiterungshack Gerät (0: Kein, 1: CSORY 256K, 2: HANNES 256K, 3: HANNES 1024K, 4: HANNES 4096K)"},
/* es */ {IDCLS_SET_PLUS4_MEMORY_HACK_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_PLUS4_MEMORY_HACK_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_PLUS4_MEMORY_HACK_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_PLUS4_MEMORY_HACK_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SET_PLUS4_MEMORY_HACK_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_PLUS4_MEMORY_HACK_NL, "Stel het 'geheugen uitbreidingen hack' apparaat in (0: Geen, 1: CSORY 256K, 2: HANNES 256K, 3: HANNES 1024K, 4: HANNES 4096K)"},
/* pl */ {IDCLS_SET_PLUS4_MEMORY_HACK_PL, "Ustaw urz±dzenie 'hacka' rozszerzenia pamiêci (0: Brak, 1: 256K CSORY, 2: 256K HANNES, 3: 1024K HANNES, 4: 4096K HANNES)"},
/* ru */ {IDCLS_SET_PLUS4_MEMORY_HACK_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_PLUS4_MEMORY_HACK_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SET_PLUS4_MEMORY_HACK_TR, ""},  /* fuzzy */
#endif

/* c64dtv/c64dtvblitter.c */
/* en */ {IDCLS_SPECIFY_DTV_REVISION,    N_("Specify DTV Revision (2: DTV2, 3: DTV3)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_DTV_REVISION_DA, "Angiv DTV-revision (2: DTV2, 3: DTV3)"},
/* de */ {IDCLS_SPECIFY_DTV_REVISION_DE, "DTV Revision wählen (2: DTV2, 3: DTV3)"},
/* es */ {IDCLS_SPECIFY_DTV_REVISION_ES, "Especificar revisión DTV (2: DTV2, 3: DTV3)"},
/* fr */ {IDCLS_SPECIFY_DTV_REVISION_FR, "Spécifier la révision DTV (2: DTV2, 3: DTV3)"},
/* hu */ {IDCLS_SPECIFY_DTV_REVISION_HU, "Adja meg a DTV verzióját (2: DTV2, 3: DTV3)"},
/* it */ {IDCLS_SPECIFY_DTV_REVISION_IT, "Specifica la revisione DTV (2: DTV2, 3: DTV3)"},
/* ko */ {IDCLS_SPECIFY_DTV_REVISION_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_DTV_REVISION_NL, "Geef de DTV revisie (2: DTV2, 3: DTV3)"},
/* pl */ {IDCLS_SPECIFY_DTV_REVISION_PL, "Okre¶l wersjê DTV (2: DTV2, 3: DTV3)"},
/* ru */ {IDCLS_SPECIFY_DTV_REVISION_RU, "Specify DTV Revision (2: DTV2, 3: DTV3)"},
/* sv */ {IDCLS_SPECIFY_DTV_REVISION_SV, "Ange DTV-revision (2: DTV2, 3: DTV3)"},
/* tr */ {IDCLS_SPECIFY_DTV_REVISION_TR, "DTV Revizyonunu belirt (2: DTV2, 3: DTV3)"},
#endif

/* c64dtv/c64dtvblitter.c */
/* en */ {IDCLS_ENABLE_DTV_BLITTER_LOG,    N_("Enable DTV blitter logs.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DTV_BLITTER_LOG_DA, "Aktivér DTV-blitterlog."},
/* de */ {IDCLS_ENABLE_DTV_BLITTER_LOG_DE, "DTV Blitter Log aktivieren."},
/* es */ {IDCLS_ENABLE_DTV_BLITTER_LOG_ES, "Permitir informes blitter DTV"},
/* fr */ {IDCLS_ENABLE_DTV_BLITTER_LOG_FR, "Activer la journalisation pour DTV blitter"},
/* hu */ {IDCLS_ENABLE_DTV_BLITTER_LOG_HU, "DTV blit naplók engedélyezése."},
/* it */ {IDCLS_ENABLE_DTV_BLITTER_LOG_IT, "Attiva log del blitter DTV."},
/* ko */ {IDCLS_ENABLE_DTV_BLITTER_LOG_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DTV_BLITTER_LOG_NL, "DTV blitter logs inschakelen."},
/* pl */ {IDCLS_ENABLE_DTV_BLITTER_LOG_PL, "W³±cz logi blittera DTV."},
/* ru */ {IDCLS_ENABLE_DTV_BLITTER_LOG_RU, "Enable DTV blitter logs."},
/* sv */ {IDCLS_ENABLE_DTV_BLITTER_LOG_SV, "Aktivera DTV-blitterloggar."},
/* tr */ {IDCLS_ENABLE_DTV_BLITTER_LOG_TR, "DTV blitter loglarýný aktif et"},
#endif

/* c64dtv/c64dtvblitter.c */
/* en */ {IDCLS_DISABLE_DTV_BLITTER_LOG,    N_("Disable DTV blitter logs.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DTV_BLITTER_LOG_DA, "Deaktivér DTV-blitterlog."},
/* de */ {IDCLS_DISABLE_DTV_BLITTER_LOG_DE, "DTV Blitter Log deaktivieren."},
/* es */ {IDCLS_DISABLE_DTV_BLITTER_LOG_ES, "Deshabilitar informes blitter DTV"},
/* fr */ {IDCLS_DISABLE_DTV_BLITTER_LOG_FR, "Désactiver la journalisation pour DTV blitter"},
/* hu */ {IDCLS_DISABLE_DTV_BLITTER_LOG_HU, "DTV blit naplók tiltása."},
/* it */ {IDCLS_DISABLE_DTV_BLITTER_LOG_IT, "Disattiva log del blitter DTV."},
/* ko */ {IDCLS_DISABLE_DTV_BLITTER_LOG_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DTV_BLITTER_LOG_NL, "DTV Blitter logs uitschakelen."},
/* pl */ {IDCLS_DISABLE_DTV_BLITTER_LOG_PL, "Wy³±cz logi blittera DTV"},
/* ru */ {IDCLS_DISABLE_DTV_BLITTER_LOG_RU, "Disable DTV blitter logs."},
/* sv */ {IDCLS_DISABLE_DTV_BLITTER_LOG_SV, "Inaktivera DTV-blitterloggar."},
/* tr */ {IDCLS_DISABLE_DTV_BLITTER_LOG_TR, "DTV blitter loglarýný pasifleþtir"},
#endif

/* c64dtv/c64dtvdma.c */
/* en */ {IDCLS_ENABLE_DTV_DMA_LOG,    N_("Enable DTV DMA logs.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DTV_DMA_LOG_DA, "Aktivér DTV DMA-log."},
/* de */ {IDCLS_ENABLE_DTV_DMA_LOG_DE, "DTV DMA Logs aktivieren."},
/* es */ {IDCLS_ENABLE_DTV_DMA_LOG_ES, "Permitir informes DTV DMA."},
/* fr */ {IDCLS_ENABLE_DTV_DMA_LOG_FR, "Activer la journalisation DMA DTV."},
/* hu */ {IDCLS_ENABLE_DTV_DMA_LOG_HU, "DTV DMA naplók engedélyezése."},
/* it */ {IDCLS_ENABLE_DTV_DMA_LOG_IT, "Attiva log DMA DTV."},
/* ko */ {IDCLS_ENABLE_DTV_DMA_LOG_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DTV_DMA_LOG_NL, "DTV DMA logs inschakelen."},
/* pl */ {IDCLS_ENABLE_DTV_DMA_LOG_PL, "W³±cz logowanie DTV DMA"},
/* ru */ {IDCLS_ENABLE_DTV_DMA_LOG_RU, "Enable DTV DMA logs."},
/* sv */ {IDCLS_ENABLE_DTV_DMA_LOG_SV, "Aktivera DTV DMA-loggning."},
/* tr */ {IDCLS_ENABLE_DTV_DMA_LOG_TR, "DTV DMA loglarýný aktif et"},
#endif

/* c64dtv/c64dtvdma.c */
/* en */ {IDCLS_DISABLE_DTV_DMA_LOG,    N_("Disable DTV DMA logs.")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DTV_DMA_LOG_DA, "Deaktivér DTV DMA-log."},
/* de */ {IDCLS_DISABLE_DTV_DMA_LOG_DE, "DTV DMA Logs deaktivieren."},
/* es */ {IDCLS_DISABLE_DTV_DMA_LOG_ES, "Deshabilitar informes DTV DMA."},
/* fr */ {IDCLS_DISABLE_DTV_DMA_LOG_FR, "Désactiver la journalisation DTV DMA."},
/* hu */ {IDCLS_DISABLE_DTV_DMA_LOG_HU, "DTV DMA naplók tiltása."},
/* it */ {IDCLS_DISABLE_DTV_DMA_LOG_IT, "Disattiva log DMA DTV."},
/* ko */ {IDCLS_DISABLE_DTV_DMA_LOG_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DTV_DMA_LOG_NL, "DTV DMA logs uitschakelen."},
/* pl */ {IDCLS_DISABLE_DTV_DMA_LOG_PL, "Wy³±cz logowanie DTV DMA"},
/* ru */ {IDCLS_DISABLE_DTV_DMA_LOG_RU, "Disable DTV DMA logs."},
/* sv */ {IDCLS_DISABLE_DTV_DMA_LOG_SV, "Inaktivera DTV DMA-loggning."},
/* tr */ {IDCLS_DISABLE_DTV_DMA_LOG_TR, "DTV DMA loglarýný pasifleþtir"},
#endif

/* c64dtv/flash-trap.c */
/* en */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS,    N_("Use <name> as directory for flash file system device")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_DA, "Brug <navn> som katalog for filsystembaseret flashenhed"},
/* de */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_DE, "Benutze <Name> für Verzeichnis Gerät Flash Dateisystem"},
/* es */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_ES, "Usar <nombre> como directorio para periférico flash files system"},
/* fr */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_FR, "Utiliser le répertoire <nom> comme système de fichiers flash"},
/* hu */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_HU, "<név> könyvtár használata a flash fájlrendszer eszközhöz"},
/* it */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_IT, "Una <nome> come directory per il file system flash"},
/* ko */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_NL, "Gebruik <naam> als directory voor het flash bestandssysteem apparaat"},
/* pl */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_PL, "U¿yj <nazwa> jako katalogu urz±dzenia systemu plików flash"},
/* ru */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_RU, "Use <name> as directory for flash file system device"},
/* sv */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_SV, "Ange <namn> som katalog för filsystemsbaserad flashenhet"},
/* tr */ {IDCLS_USE_AS_DIRECTORY_FLASH_FS_TR, "Flash dosya sistem aygýtý için <isim>'i dizin olarak kullan"},
#endif

/* c64dtv/flash-trap.c */
/* en */ {IDCLS_ENABLE_TRUE_FLASH_FS,    N_("Enable true hardware flash file system")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_TRUE_FLASH_FS_DA, "Aktivér ægte hardwareemulering af flashfilsystem"},
/* de */ {IDCLS_ENABLE_TRUE_FLASH_FS_DE, "Präzises Hardware Flashdateisystem aktivieren"},
/* es */ {IDCLS_ENABLE_TRUE_FLASH_FS_ES, "Permitir flash file system verdadero (hardware)"},
/* fr */ {IDCLS_ENABLE_TRUE_FLASH_FS_FR, "Activer le système de fichier flash matériel réel"},
/* hu */ {IDCLS_ENABLE_TRUE_FLASH_FS_HU, "Igazi hardver flash fájlrendszer engedélyezése"},
/* it */ {IDCLS_ENABLE_TRUE_FLASH_FS_IT, "Attiva emulazione hardware del file system flash"},
/* ko */ {IDCLS_ENABLE_TRUE_FLASH_FS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_TRUE_FLASH_FS_NL, "Activeer hardwarmatige flash-bestandssysteem"},
/* pl */ {IDCLS_ENABLE_TRUE_FLASH_FS_PL, "W³±cz prawdziwy sprzêtowy system plików flash"},
/* ru */ {IDCLS_ENABLE_TRUE_FLASH_FS_RU, "Enable true hardware flash file system"},
/* sv */ {IDCLS_ENABLE_TRUE_FLASH_FS_SV, "Aktivera äkta maskinvaru-flashfilsystem"},
/* tr */ {IDCLS_ENABLE_TRUE_FLASH_FS_TR, "Gerçek donaným flash dosya sistemini aktif et"},
#endif

/* c64dtv/flash-trap.c */
/* en */ {IDCLS_DISABLE_TRUE_FLASH_FS,    N_("Disable true hardware flash file system")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_TRUE_FLASH_FS_DA, "Deaktivér ægte hardwareemulering af flashfilsystem"},
/* de */ {IDCLS_DISABLE_TRUE_FLASH_FS_DE, "Präzises Hardware Flashdateisystem deaktivieren"},
/* es */ {IDCLS_DISABLE_TRUE_FLASH_FS_ES, "Deshabilitar flash file system verdadero (hardware)"},
/* fr */ {IDCLS_DISABLE_TRUE_FLASH_FS_FR, "Désactiver le système de fichiers matériel réel"},
/* hu */ {IDCLS_DISABLE_TRUE_FLASH_FS_HU, "Igazi hardver flash fájlrendszer tiltása"},
/* it */ {IDCLS_DISABLE_TRUE_FLASH_FS_IT, "Disattiva emulazione hardware del file system flash"},
/* ko */ {IDCLS_DISABLE_TRUE_FLASH_FS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_TRUE_FLASH_FS_NL, "Hardwarematig flash bestandssysteem uitschakelen"},
/* pl */ {IDCLS_DISABLE_TRUE_FLASH_FS_PL, "Wy³±cz prawdziwy system plików flash"},
/* ru */ {IDCLS_DISABLE_TRUE_FLASH_FS_RU, "Disable true hardware flash file system"},
/* sv */ {IDCLS_DISABLE_TRUE_FLASH_FS_SV, "Inaktivera maskinvaruemelring för flashfilsystem"},
/* tr */ {IDCLS_DISABLE_TRUE_FLASH_FS_TR, "Gerçek donaným flash dosya sistemini pasifleþtir"},
#endif

#ifdef HAVE_MIDI
/* midi.c */
/* en */ {IDCLS_ENABLE_MIDI_EMU,    N_("Enable MIDI emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_MIDI_EMU_DA, "Aktivér MIDI-emulering"},
/* de */ {IDCLS_ENABLE_MIDI_EMU_DE, "MIDI Emulation aktivieren"},
/* es */ {IDCLS_ENABLE_MIDI_EMU_ES, "Habilitar emulación MIDI"},
/* fr */ {IDCLS_ENABLE_MIDI_EMU_FR, "Activer l'émulation MIDI"},
/* hu */ {IDCLS_ENABLE_MIDI_EMU_HU, "MIDI emuláció engedélyezése"},
/* it */ {IDCLS_ENABLE_MIDI_EMU_IT, "Attiva emulazione MIDI"},
/* ko */ {IDCLS_ENABLE_MIDI_EMU_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_MIDI_EMU_NL, "MIDI emulatie inschakelen"},
/* pl */ {IDCLS_ENABLE_MIDI_EMU_PL, "W³±cz emulacjê MIDI"},
/* ru */ {IDCLS_ENABLE_MIDI_EMU_RU, "²ÚÛîçØâì íÜãÛïæØî MIDI"},
/* sv */ {IDCLS_ENABLE_MIDI_EMU_SV, "Aktivera MIDI-emulering"},
/* tr */ {IDCLS_ENABLE_MIDI_EMU_TR, "MIDI emülasyonunu aktif et"},
#endif

/* midi.c */
/* en */ {IDCLS_DISABLE_MIDI_EMU,    N_("Disable MIDI emulation")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_MIDI_EMU_DA, "Deaktivér MIDI-emulering"},
/* de */ {IDCLS_DISABLE_MIDI_EMU_DE, "MIDI Emulation deaktivieren"},
/* es */ {IDCLS_DISABLE_MIDI_EMU_ES, "Deshabilitar emulación MIDI"},
/* fr */ {IDCLS_DISABLE_MIDI_EMU_FR, "Désactiver l'émulation MIDI"},
/* hu */ {IDCLS_DISABLE_MIDI_EMU_HU, "MIDI emuláció tiltása"},
/* it */ {IDCLS_DISABLE_MIDI_EMU_IT, "Disattiva emulazione MIDI"},
/* ko */ {IDCLS_DISABLE_MIDI_EMU_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_MIDI_EMU_NL, "MIDI emulatie uitschakelen"},
/* pl */ {IDCLS_DISABLE_MIDI_EMU_PL, "Wy³±cz emulacjê MIDI"},
/* ru */ {IDCLS_DISABLE_MIDI_EMU_RU, "Disable MIDI emulation"},
/* sv */ {IDCLS_DISABLE_MIDI_EMU_SV, "Inaktivera MIDI-emulering"},
/* tr */ {IDCLS_DISABLE_MIDI_EMU_TR, "MIDI emülasyonunu pasifleþtir"},
#endif
#endif

/* plus4/digiblaster.c */
/* en */ {IDCLS_ENABLE_DIGIBLASTER,    N_("Enable the digiblaster add-on")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_DIGIBLASTER_DA, "Aktivér digiblaster-tilføjelse"},
/* de */ {IDCLS_ENABLE_DIGIBLASTER_DE, "Digiblaster add-on aktivieren"},
/* es */ {IDCLS_ENABLE_DIGIBLASTER_ES, "Permitir complementos digiblaster"},
/* fr */ {IDCLS_ENABLE_DIGIBLASTER_FR, "Activer le add-on digiblaster"},
/* hu */ {IDCLS_ENABLE_DIGIBLASTER_HU, "Digiblaster engedélyezése"},
/* it */ {IDCLS_ENABLE_DIGIBLASTER_IT, "Attiva l'estensione digiblaster"},
/* ko */ {IDCLS_ENABLE_DIGIBLASTER_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_DIGIBLASTER_NL, "Digiblaster add-on inschakelen"},
/* pl */ {IDCLS_ENABLE_DIGIBLASTER_PL, "W³±cz dodatek digiblaster"},
/* ru */ {IDCLS_ENABLE_DIGIBLASTER_RU, "Enable the digiblaster add-on"},
/* sv */ {IDCLS_ENABLE_DIGIBLASTER_SV, "Aktivera digiblaster-tillägg"},
/* tr */ {IDCLS_ENABLE_DIGIBLASTER_TR, "Digiblaster eklentisini aktif et"},
#endif

/* plus4/digiblaster.c */
/* en */ {IDCLS_DISABLE_DIGIBLASTER,    N_("Disable the digiblaster add-on")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_DIGIBLASTER_DA, "Deaktivér digiblaster-tilføjelse"},
/* de */ {IDCLS_DISABLE_DIGIBLASTER_DE, "Digiblaster add-on deaktivieren"},
/* es */ {IDCLS_DISABLE_DIGIBLASTER_ES, "Deshabilitar complementos digiblaster"},
/* fr */ {IDCLS_DISABLE_DIGIBLASTER_FR, "Désactiver le add-on digiblaster"},
/* hu */ {IDCLS_DISABLE_DIGIBLASTER_HU, "Digiblaster tiltása"},
/* it */ {IDCLS_DISABLE_DIGIBLASTER_IT, "Disattiva l'estensione digiblaster"},
/* ko */ {IDCLS_DISABLE_DIGIBLASTER_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_DIGIBLASTER_NL, "Digiblaster add-on uitschakelen"},
/* pl */ {IDCLS_DISABLE_DIGIBLASTER_PL, "Wy³±cz dodatek digiblaster"},
/* ru */ {IDCLS_DISABLE_DIGIBLASTER_RU, "Disable the digiblaster add-on"},
/* sv */ {IDCLS_DISABLE_DIGIBLASTER_SV, "Inaktivera digiblaster-tillägg"},
/* tr */ {IDCLS_DISABLE_DIGIBLASTER_TR, "Digiblaster eklentisini pasifleþtir"},
#endif

/* plus4/plus4speech.c */
/* en */ {IDCLS_ENABLE_PLUS4SPEECH,    N_("Enable the v364 speech add-on")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_PLUS4SPEECH_DA, "Aktivér v364 Speech-tilføjelse"},
/* de */ {IDCLS_ENABLE_PLUS4SPEECH_DE, "v364 Spracherweiterung aktivieren"},
/* es */ {IDCLS_ENABLE_PLUS4SPEECH_ES, "Permitir complementos v364 speech"},
/* fr */ {IDCLS_ENABLE_PLUS4SPEECH_FR, "Activer le add-on V364 Speech"},
/* hu */ {IDCLS_ENABLE_PLUS4SPEECH_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_PLUS4SPEECH_IT, "Attiva l'estensione v364 speech"},
/* ko */ {IDCLS_ENABLE_PLUS4SPEECH_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_PLUS4SPEECH_NL, "v364 spraak add-on inschakelen"},
/* pl */ {IDCLS_ENABLE_PLUS4SPEECH_PL, "W³±cz rozszerzenie mowy v364"},
/* ru */ {IDCLS_ENABLE_PLUS4SPEECH_RU, "Enable the v364 speech add-on"},
/* sv */ {IDCLS_ENABLE_PLUS4SPEECH_SV, "Aktivera v364-rösttillägg"},
/* tr */ {IDCLS_ENABLE_PLUS4SPEECH_TR, "V364 speech eklentisini aktif et"},
#endif

/* plus4/plus4speech.c */
/* en */ {IDCLS_DISABLE_PLUS4SPEECH,    N_("Disable the v364 speech add-on")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_PLUS4SPEECH_DA, "Deaktivér v364 Speech-tilføjelse"},
/* de */ {IDCLS_DISABLE_PLUS4SPEECH_DE, "v364 Spracherweiterung deaktivieren"},
/* es */ {IDCLS_DISABLE_PLUS4SPEECH_ES, "Deshabilitar complementos v364 speech"},
/* fr */ {IDCLS_DISABLE_PLUS4SPEECH_FR, "Désactiver le add-on V364 Speech"},
/* hu */ {IDCLS_DISABLE_PLUS4SPEECH_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_PLUS4SPEECH_IT, "Disattiva l'estensione v364 speech"},
/* ko */ {IDCLS_DISABLE_PLUS4SPEECH_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_PLUS4SPEECH_NL, "v364 spraak add-on uitschakelen"},
/* pl */ {IDCLS_DISABLE_PLUS4SPEECH_PL, "Wy³±cz rozszerzenie mowy v364"},
/* ru */ {IDCLS_DISABLE_PLUS4SPEECH_RU, "Disable the v364 speech add-on"},
/* sv */ {IDCLS_DISABLE_PLUS4SPEECH_SV, "Inaktivera v364-rösttillägg"},
/* tr */ {IDCLS_DISABLE_PLUS4SPEECH_TR, "V364 speech eklentisini pasifleþtir"},
#endif

/* plus4/plus4speech.c */
/* en */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE,    N_("Attach Speech ROM image")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE_DA, "Tilslut Speech ROM image"},
/* de */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE_DE, "Spracherweiterung ROM Imagedatei einlegen"},
/* es */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE_ES, "Insertar imagen ROM Speech"},
/* fr */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE_FR, "Attacher une image Speech RO"},
/* hu */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE_IT, "Seleziona immagine Speech ROM"},
/* ko */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE_NL, "Koppel spraak ROM bestand"},
/* pl */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE_PL, "Zamontuj obraz ROM mowy"},
/* ru */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE_RU, "Attach Speech ROM image"},
/* sv */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE_SV, "Anslut röst-ROM-avbildning"},
/* tr */ {IDCLS_ATTACH_SPEECH_ROM_IMAGE_TR, "Speech ROM imajýný yerleþtir"},
#endif

/* plus4/sidcartjoy.c */
/* en */ {IDCLS_ENABLE_SIDCARTJOY,    N_("Enable SID cartridge joystick")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SIDCARTJOY_DA, "Aktivér SID-cartridge joystick"},
/* de */ {IDCLS_ENABLE_SIDCARTJOY_DE, "SIDcart Modul aktivieren"},
/* es */ {IDCLS_ENABLE_SIDCARTJOY_ES, "Permitir joystick cartucho SID"},
/* fr */ {IDCLS_ENABLE_SIDCARTJOY_FR, "Activer Joystick cartouche SID"},
/* hu */ {IDCLS_ENABLE_SIDCARTJOY_HU, "SID cartridge botkormány engedélyezése"},
/* it */ {IDCLS_ENABLE_SIDCARTJOY_IT, "Attiva joystick su cartuccia SID"},
/* ko */ {IDCLS_ENABLE_SIDCARTJOY_KO, "SID Ä«Æ®¸®Áö Á¶ÀÌ½ºÆ½ »ç¿ëÇÏ±â"},
/* nl */ {IDCLS_ENABLE_SIDCARTJOY_NL, "SID cartridge joystick inschakelen"},
/* pl */ {IDCLS_ENABLE_SIDCARTJOY_PL, "W³±cz joystick kartrid¿a SID"},
/* ru */ {IDCLS_ENABLE_SIDCARTJOY_RU, "Enable SID cartridge joystick"},
/* sv */ {IDCLS_ENABLE_SIDCARTJOY_SV, "Aktivera SID-insticksmodul-styrspak"},
/* tr */ {IDCLS_ENABLE_SIDCARTJOY_TR, "SID kartuþu joystick'ini aktif et"},
#endif

/* plus4/sidcartjoy.c */
/* en */ {IDCLS_DISABLE_SIDCARTJOY,    N_("Disable SID cartridge joystick")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SIDCARTJOY_DA, "Deaktivér SID-cartridge joystick"},
/* de */ {IDCLS_DISABLE_SIDCARTJOY_DE, "SIDcart Modul deaktivieren"},
/* es */ {IDCLS_DISABLE_SIDCARTJOY_ES, "Deshabilitar joystick cartucho SID"},
/* fr */ {IDCLS_DISABLE_SIDCARTJOY_FR, "Désactiver Joystick cartouche SID"},
/* hu */ {IDCLS_DISABLE_SIDCARTJOY_HU, "SID cartridge botkormány tiltása"},
/* it */ {IDCLS_DISABLE_SIDCARTJOY_IT, "Disattiva joystick su cartuccia SID"},
/* ko */ {IDCLS_DISABLE_SIDCARTJOY_KO, "SID Ä«Æ®¸®Áö Á¶ÀÌ½ºÆ½ »ç¿ëÇÏÁö ¾Ê±â"},
/* nl */ {IDCLS_DISABLE_SIDCARTJOY_NL, "SID cartridge joystick uitschakelen"},
/* pl */ {IDCLS_DISABLE_SIDCARTJOY_PL, "Wy³±cz joystick kartrid¿a SID"},
/* ru */ {IDCLS_DISABLE_SIDCARTJOY_RU, "Disable SID cartridge joystick"},
/* sv */ {IDCLS_DISABLE_SIDCARTJOY_SV, "Inaktivera SID-insticksmodul-styrspak"},
/* tr */ {IDCLS_DISABLE_SIDCARTJOY_TR, "SID kartuþu joystick'ini pasifleþtir"},
#endif

/* c64/cart/sfx_soundsampler.c */
/* en */ {IDCLS_ENABLE_SFX_SS,    N_("Enable the SFX Sound Sampler cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SFX_SS_DA, "Aktivér SFX Sound Sampler cartridge"},
/* de */ {IDCLS_ENABLE_SFX_SS_DE, "SFX Sound Sampler Modul aktivieren"},
/* es */ {IDCLS_ENABLE_SFX_SS_ES, "Permitir cartucho SFX Sound Sampler"},
/* fr */ {IDCLS_ENABLE_SFX_SS_FR, "Activer la cartouche SFX Sound Sampler"},
/* hu */ {IDCLS_ENABLE_SFX_SS_HU, "SFX Sound Sampler engedélyezése"},
/* it */ {IDCLS_ENABLE_SFX_SS_IT, "Attiva cartuccia SFX Sound Sampler"},
/* ko */ {IDCLS_ENABLE_SFX_SS_KO, "SFX Sound Sampler Ä«Æ®¸®Áö ¸¦ ÀÛµ¿½ÃÅ²´Ù"},
/* nl */ {IDCLS_ENABLE_SFX_SS_NL, "SFX Sound Sampler cartridge inschakelen"},
/* pl */ {IDCLS_ENABLE_SFX_SS_PL, "W³±cz kartrid¿ SFX Sound Sampler"},
/* ru */ {IDCLS_ENABLE_SFX_SS_RU, "Enable the SFX Sound Sampler cartridge"},
/* sv */ {IDCLS_ENABLE_SFX_SS_SV, "Aktivera SFX Sound Sampler-insticksmodul"},
/* tr */ {IDCLS_ENABLE_SFX_SS_TR, "SFX Sound Sampler kartuþunu aktif et"},
#endif

/* c64/cart/sfx_soundsampler.c */
/* en */ {IDCLS_DISABLE_SFX_SS,    N_("Disable the SFX Sound Sampler cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SFX_SS_DA, "Deaktivér SFX Sound Sampler cartridge"},
/* de */ {IDCLS_DISABLE_SFX_SS_DE, "SFX Sound Sampler Modul  deaktivieren"},
/* es */ {IDCLS_DISABLE_SFX_SS_ES, "Deshabilitar cartucho SFX Sound Sampler"},
/* fr */ {IDCLS_DISABLE_SFX_SS_FR, "Désactiver la cartouche SFX Sound Sampler"},
/* hu */ {IDCLS_DISABLE_SFX_SS_HU, "SFX Sound Sampler tiltása"},
/* it */ {IDCLS_DISABLE_SFX_SS_IT, "Disattiva cartuccia SFX Sound Sampler"},
/* ko */ {IDCLS_DISABLE_SFX_SS_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SFX_SS_NL, "SFX Sound Sampler cartridge uitschakelen"},
/* pl */ {IDCLS_DISABLE_SFX_SS_PL, "Wy³±cz kartrid¿ SFX Sound Sampler"},
/* ru */ {IDCLS_DISABLE_SFX_SS_RU, "Disable the SFX Sound Sampler cartridge"},
/* sv */ {IDCLS_DISABLE_SFX_SS_SV, "Inaktivera SFX Sound Sampler-insticksmodul"},
/* tr */ {IDCLS_DISABLE_SFX_SS_TR, "SFX Sound Sampler kartuþunu pasifleþtir"},
#endif

/* c64/cart/turbomaster.c */
/* en */ {IDCLS_ENABLE_TURBOMASTER,    N_("Enable the TurboMaster cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_TURBOMASTER_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_TURBOMASTER_DE, "TurboMaster Modul aktivieren"},
/* es */ {IDCLS_ENABLE_TURBOMASTER_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_TURBOMASTER_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_TURBOMASTER_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_TURBOMASTER_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_TURBOMASTER_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_TURBOMASTER_NL, "TurboMaster cartridge inschakelen"},
/* pl */ {IDCLS_ENABLE_TURBOMASTER_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_ENABLE_TURBOMASTER_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_TURBOMASTER_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_TURBOMASTER_TR, ""},  /* fuzzy */
#endif

/* c64/cart/turbomaster.c */
/* en */ {IDCLS_DISABLE_TURBOMASTER,    N_("Disable the TurboMaster cartridge")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_TURBOMASTER_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_TURBOMASTER_DE, "TurboMaster Modul deaktivieren"},
/* es */ {IDCLS_DISABLE_TURBOMASTER_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_TURBOMASTER_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_TURBOMASTER_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_TURBOMASTER_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_TURBOMASTER_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_TURBOMASTER_NL, "TurboMaster cartridge uitschakelen"},
/* pl */ {IDCLS_DISABLE_TURBOMASTER_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_DISABLE_TURBOMASTER_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_TURBOMASTER_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_TURBOMASTER_TR, ""},  /* fuzzy */
#endif

/* c64/cart/turbomaster.c */
/* en */ {IDCLS_TURBOMASTER_SWITCH_ROM_1,    N_("Set TurboMaster ROM switch to ROM 1")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TURBOMASTER_SWITCH_ROM_1_DA, ""},  /* fuzzy */
/* de */ {IDCLS_TURBOMASTER_SWITCH_ROM_1_DE, "TurboMaster ROM Schalter auf ROM 1 setzen"},
/* es */ {IDCLS_TURBOMASTER_SWITCH_ROM_1_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_TURBOMASTER_SWITCH_ROM_1_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_TURBOMASTER_SWITCH_ROM_1_HU, ""},  /* fuzzy */
/* it */ {IDCLS_TURBOMASTER_SWITCH_ROM_1_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_TURBOMASTER_SWITCH_ROM_1_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_TURBOMASTER_SWITCH_ROM_1_NL, "Schakel TurboMaster ROM schakelaar naar ROM 1"},
/* pl */ {IDCLS_TURBOMASTER_SWITCH_ROM_1_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_TURBOMASTER_SWITCH_ROM_1_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_TURBOMASTER_SWITCH_ROM_1_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_TURBOMASTER_SWITCH_ROM_1_TR, ""},  /* fuzzy */
#endif

/* c64/cart/turbomaster.c */
/* en */ {IDCLS_TURBOMASTER_SWITCH_ROM_2,    N_("Set TurboMaster ROM switch to ROM 2")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TURBOMASTER_SWITCH_ROM_2_DA, ""},  /* fuzzy */
/* de */ {IDCLS_TURBOMASTER_SWITCH_ROM_2_DE, "TurboMaster ROM Schalter auf ROM 2 setzen"},
/* es */ {IDCLS_TURBOMASTER_SWITCH_ROM_2_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_TURBOMASTER_SWITCH_ROM_2_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_TURBOMASTER_SWITCH_ROM_2_HU, ""},  /* fuzzy */
/* it */ {IDCLS_TURBOMASTER_SWITCH_ROM_2_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_TURBOMASTER_SWITCH_ROM_2_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_TURBOMASTER_SWITCH_ROM_2_NL, "Schakel TurboMaster ROM schakelaar naar ROM 2"},
/* pl */ {IDCLS_TURBOMASTER_SWITCH_ROM_2_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_TURBOMASTER_SWITCH_ROM_2_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_TURBOMASTER_SWITCH_ROM_2_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_TURBOMASTER_SWITCH_ROM_2_TR, ""},  /* fuzzy */
#endif

/* c64/cart/turbomaster.c */
/* en */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT,    N_("Set TurboMaster speed control switch to software")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT_DA, ""},  /* fuzzy */
/* de */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT_DE, "TurboMaster Geschwindigkeits Kontrollschalter auf Software setzen"},
/* es */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT_HU, ""},  /* fuzzy */
/* it */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT_NL, "Schakel TurboMaster snelheidscontrole schakelaar naar software"},
/* pl */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT_TR, ""},  /* fuzzy */
#endif

/* c64/cart/turbomaster.c */
/* en */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL,    N_("Set TurboMaster speed control switch to manual")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL_DA, ""},  /* fuzzy */
/* de */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL_DE, "TurboMaster Geschwindigkeits Kontrollschalter auf Manuell setzen"},
/* es */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL_HU, ""},  /* fuzzy */
/* it */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL_NL, "Schakel TurboMaster snelheidscontrole schakelaar naar handmatig"},
/* pl */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL_TR, ""},  /* fuzzy */
#endif

/* c64/cart/turbomaster.c */
/* en */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ,    N_("Set TurboMaster clock speed switch to 4 Mhz")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ_DA, ""},  /* fuzzy */
/* de */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ_DE, "TurboMaster Taktgeschwindigkeitsschalter auf 4Mhz setzen"},
/* es */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ_HU, ""},  /* fuzzy */
/* it */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ_NL, "Schakel TurboMaster klok snelheid schakelaar naar 4 Mhz"},
/* pl */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_TURBOMASTER_SWITCH_4_MHZ_TR, ""},  /* fuzzy */
#endif

/* c64/cart/turbomaster.c */
/* en */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ,    N_("Set TurboMaster clock speed switch to 1 Mhz")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ_DA, ""},  /* fuzzy */
/* de */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ_DE, "TurboMaster Taktgeschwindigkeitsschalter auf 1Mhz setzen"},
/* es */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ_HU, ""},  /* fuzzy */
/* it */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ_NL, "Schakel TurboMaster klok snelheid schakelaar naar 1 Mhz"},
/* pl */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_TURBOMASTER_SWITCH_1_MHZ_TR, ""},  /* fuzzy */
#endif

/* c64/cart/turbomaster.c */
/* en */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME,    N_("Specify TurboMaster ROM 1 filename")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME_DE, "TurboMaster ROM 1 wählen"},
/* es */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME_NL, "Geef de naam van het TurboMaster ROM 1 bestand"},
/* pl */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME_TR, ""},  /* fuzzy */
#endif

/* c64/cart/turbomaster.c */
/* en */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME,    N_("Specify TurboMaster ROM 2 filename")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME_DE, "TurboMaster ROM 2 wählen"},
/* es */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME_NL, "Geef de naam van het TurboMaster ROM 2 bestand"},
/* pl */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME_TR, ""},  /* fuzzy */
#endif

/* c64/cart/shortbus_digimax.c */
/* en */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX,    N_("Enable the Short Bus DigiMAX expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX_DE, "Shortbus DigiMAX Erweiterung aktivieren"},
/* es */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX_NL, "Short Bus DigiMAX uitbreiding inschakelen"},
/* pl */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_SHORTBUS_DIGIMAX_TR, ""},  /* fuzzy */
#endif

/* c64/cart/shortbus_digimax.c */
/* en */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX,    N_("Disable the Short Bus DigiMAX expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX_DE, "Shortbus DigiMAX Erweiterung deaktivieren"},
/* es */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX_NL, "Short Bus DigiMAX uitbreiding uitschakelen"},
/* pl */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_SHORTBUS_DIGIMAX_TR, ""},  /* fuzzy */
#endif

/* c64/cart/shortbus_etfe.c */
/* en */ {IDCLS_ENABLE_SHORTBUS_ETFE,    N_("Enable the Short Bus ETFE expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SHORTBUS_ETFE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_SHORTBUS_ETFE_DE, "Short Bus ETFE Erweiterung aktivieren"},
/* es */ {IDCLS_ENABLE_SHORTBUS_ETFE_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_ENABLE_SHORTBUS_ETFE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_SHORTBUS_ETFE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SHORTBUS_ETFE_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_ENABLE_SHORTBUS_ETFE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SHORTBUS_ETFE_NL, "Short Bus ETFE uitbreiding inschakelen"},
/* pl */ {IDCLS_ENABLE_SHORTBUS_ETFE_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_ENABLE_SHORTBUS_ETFE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SHORTBUS_ETFE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_ENABLE_SHORTBUS_ETFE_TR, ""},  /* fuzzy */
#endif

/* c64/cart/shortbus_etfe.c */
/* en */ {IDCLS_DISABLE_SHORTBUS_ETFE,    N_("Disable the Short Bus ETFE expansion")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SHORTBUS_ETFE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_SHORTBUS_ETFE_DE, "Short Bus ETFE Erweiterung deaktivieren"},
/* es */ {IDCLS_DISABLE_SHORTBUS_ETFE_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_DISABLE_SHORTBUS_ETFE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_SHORTBUS_ETFE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_SHORTBUS_ETFE_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_DISABLE_SHORTBUS_ETFE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SHORTBUS_ETFE_NL, "Short Bus ETFE uitbreiding uitschakelen"},
/* pl */ {IDCLS_DISABLE_SHORTBUS_ETFE_PL, ""},  /* fuzzy */
/* ru */ {IDCLS_DISABLE_SHORTBUS_ETFE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SHORTBUS_ETFE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_DISABLE_SHORTBUS_ETFE_TR, ""},  /* fuzzy */
#endif

/* scpu64/scpu64-cmdline-options.c */
/* en */ {IDCLS_SET_SIMM_SIZE,    N_("Size of the SIMM RAM (0/1/4/8/16 MB)")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_SIMM_SIZE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_SIMM_SIZE_DE, "Größe des SIMM RAM (0/1/4/8/16 MB)"},
/* es */ {IDCLS_SET_SIMM_SIZE_ES, "Tamaño de la RAM SIMM (0/1/4/8/16 MB)"},
/* fr */ {IDCLS_SET_SIMM_SIZE_FR, "Grandeur de la RAM SIMM (0/1/4/8/16 mo)"},
/* hu */ {IDCLS_SET_SIMM_SIZE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_SIMM_SIZE_IT, "Dimensione della RAM SIMM (0/1/4/8/16 MB)"},
/* ko */ {IDCLS_SET_SIMM_SIZE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_SIMM_SIZE_NL, "Grootte van het SIMM RAM (0/1/4/8/16 MB)"},
/* pl */ {IDCLS_SET_SIMM_SIZE_PL, "Ilo¶æ SIMM RAM-u (0/1/4/8/16 MB)"},
/* ru */ {IDCLS_SET_SIMM_SIZE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_SIMM_SIZE_SV, "Storlek på SIMM-RAM (0/1/4/8/16 MB)"},
/* tr */ {IDCLS_SET_SIMM_SIZE_TR, ""},  /* fuzzy */
#endif

/* scpu64/scpu64-cmdline-options.c */
/* en */ {IDCLS_ENABLE_JIFFY_SWITCH,    N_("Turn on Jiffy switch")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_JIFFY_SWITCH_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_JIFFY_SWITCH_DE, "Jiffy Schalter einschalten"},
/* es */ {IDCLS_ENABLE_JIFFY_SWITCH_ES, "Encender conmutador Jiffy"},
/* fr */ {IDCLS_ENABLE_JIFFY_SWITCH_FR, "Activer la jiffy Switch"},
/* hu */ {IDCLS_ENABLE_JIFFY_SWITCH_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_JIFFY_SWITCH_IT, "Attiva pulsante Jiffy"},
/* ko */ {IDCLS_ENABLE_JIFFY_SWITCH_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_JIFFY_SWITCH_NL, "Jiffy schakelaar inschakelen"},
/* pl */ {IDCLS_ENABLE_JIFFY_SWITCH_PL, "W³±cz prze³±cznik Jiffy"},
/* ru */ {IDCLS_ENABLE_JIFFY_SWITCH_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_JIFFY_SWITCH_SV, "Slå på Jiffyomkopplare"},
/* tr */ {IDCLS_ENABLE_JIFFY_SWITCH_TR, ""},  /* fuzzy */
#endif

/* scpu64/scpu64-cmdline-options.c */
/* en */ {IDCLS_DISABLE_JIFFY_SWITCH,    N_("Turn off Jiffy switch")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_JIFFY_SWITCH_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_JIFFY_SWITCH_DE, "Jiffy Schalter ausschalten"},
/* es */ {IDCLS_DISABLE_JIFFY_SWITCH_ES, "Apagar conmutador Jiffy"},
/* fr */ {IDCLS_DISABLE_JIFFY_SWITCH_FR, "Désactiver Jiffy Switch"},
/* hu */ {IDCLS_DISABLE_JIFFY_SWITCH_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_JIFFY_SWITCH_IT, "Disattiva pulsante Jiffy"},
/* ko */ {IDCLS_DISABLE_JIFFY_SWITCH_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_JIFFY_SWITCH_NL, "Jiffy schakelaar uitschakelen"},
/* pl */ {IDCLS_DISABLE_JIFFY_SWITCH_PL, "Wy³±cz prze³±cznik Jiffy"},
/* ru */ {IDCLS_DISABLE_JIFFY_SWITCH_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_JIFFY_SWITCH_SV, "Slå av Jiffyomkopplare"},
/* tr */ {IDCLS_DISABLE_JIFFY_SWITCH_TR, ""},  /* fuzzy */
#endif

/* scpu64/scpu64-cmdline-options.c */
/* en */ {IDCLS_ENABLE_SPEED_SWITCH,    N_("Turn on Speed switch")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_SPEED_SWITCH_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_SPEED_SWITCH_DE, "Speed Schalter einschalten"},
/* es */ {IDCLS_ENABLE_SPEED_SWITCH_ES, "Encender conmutador de velocidad"},
/* fr */ {IDCLS_ENABLE_SPEED_SWITCH_FR, "Activer Speed Switch"},
/* hu */ {IDCLS_ENABLE_SPEED_SWITCH_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_SPEED_SWITCH_IT, "Attiva pulsante di velocità"},
/* ko */ {IDCLS_ENABLE_SPEED_SWITCH_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_SPEED_SWITCH_NL, "Snelheid schakelaar inschakelen"},
/* pl */ {IDCLS_ENABLE_SPEED_SWITCH_PL, "W³±cz prze³±cznik Speed"},
/* ru */ {IDCLS_ENABLE_SPEED_SWITCH_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_SPEED_SWITCH_SV, "Slå på hastighetsomkopplare"},
/* tr */ {IDCLS_ENABLE_SPEED_SWITCH_TR, ""},  /* fuzzy */
#endif

/* scpu64/scpu64-cmdline-options.c */
/* en */ {IDCLS_DISABLE_SPEED_SWITCH,    N_("Turn off Speed switch")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_SPEED_SWITCH_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_SPEED_SWITCH_DE, "Speed Schalter ausschalten"},
/* es */ {IDCLS_DISABLE_SPEED_SWITCH_ES, "Apagar conmutador de velocidad"},
/* fr */ {IDCLS_DISABLE_SPEED_SWITCH_FR, "Désactiver la Speed Switch"},
/* hu */ {IDCLS_DISABLE_SPEED_SWITCH_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_SPEED_SWITCH_IT, "Disattiva pulsante di velocità"},
/* ko */ {IDCLS_DISABLE_SPEED_SWITCH_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_SPEED_SWITCH_NL, "Snelheid schakelaar uitschakelen"},
/* pl */ {IDCLS_DISABLE_SPEED_SWITCH_PL, "Wy³±cz prze³±cznik Speed"},
/* ru */ {IDCLS_DISABLE_SPEED_SWITCH_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_SPEED_SWITCH_SV, "Slå av hastighetsomkopplare"},
/* tr */ {IDCLS_DISABLE_SPEED_SWITCH_TR, ""},  /* fuzzy */
#endif

/* pet/pethre.c */
/* en */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD,    N_("Enable HiRes Emulation Board")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD_DA, ""},  /* fuzzy */
/* de */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD_DE, "HiRes Emulation Board aktivieren"},
/* es */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD_ES, "Permitir emulación tableta de alta resolución"},
/* fr */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD_HU, ""},  /* fuzzy */
/* it */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD_IT, "Attiva scheda HiRes Emulation"},
/* ko */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD_NL, "PET High Res Emulation bord inschakelen"},
/* pl */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD_PL, "W³±cz kartê emulacji HiRes"},
/* ru */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD_SV, "Aktivera högupplösningsemuleringskort"},
/* tr */ {IDCLS_ENABLE_HIRES_EMULATION_BOARD_TR, ""},  /* fuzzy */
#endif

/* pet/pethre.c */
/* en */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD,    N_("Disable HiRes Emulation Board")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD_DA, ""},  /* fuzzy */
/* de */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD_DE, "HiRes Emulation Board aktivieren"},
/* es */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD_ES, "Deshabilitar emulación tableta de alta resolución"},
/* fr */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD_HU, ""},  /* fuzzy */
/* it */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD_IT, "Disattiva scheda HiRes Emulation"},
/* ko */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD_NL, "PET High Res Emulation bord uitschakelen"},
/* pl */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD_PL, "Wy³±cz kartê emulacji HiRes"},
/* ru */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD_SV, "högupplösningsemuleringskort Inaktivera"},
/* tr */ {IDCLS_DISABLE_HIRES_EMULATION_BOARD_TR, ""},  /* fuzzy */
#endif

#ifdef HAVE_RAWDRIVE
/* diskimage/rawimage.c */
/* en */ {IDCLS_SET_RAW_DRIVE_DEVICE,    N_("Set raw drive device name")},
#ifdef HAS_TRANSLATION
/* da */ {IDCLS_SET_RAW_DRIVE_DEVICE_DA, ""},  /* fuzzy */
/* de */ {IDCLS_SET_RAW_DRIVE_DEVICE_DE, "Raw Laufwerk Gerätename setzen"},
/* es */ {IDCLS_SET_RAW_DRIVE_DEVICE_ES, ""},  /* fuzzy */
/* fr */ {IDCLS_SET_RAW_DRIVE_DEVICE_FR, ""},  /* fuzzy */
/* hu */ {IDCLS_SET_RAW_DRIVE_DEVICE_HU, ""},  /* fuzzy */
/* it */ {IDCLS_SET_RAW_DRIVE_DEVICE_IT, ""},  /* fuzzy */
/* ko */ {IDCLS_SET_RAW_DRIVE_DEVICE_KO, ""},  /* fuzzy */
/* nl */ {IDCLS_SET_RAW_DRIVE_DEVICE_NL, "Stel binair drive apparaatnaam in"},
/* pl */ {IDCLS_SET_RAW_DRIVE_DEVICE_PL, "Wprowad¼ nazwê urz±dzenia surowego napêdu"},
/* ru */ {IDCLS_SET_RAW_DRIVE_DEVICE_RU, ""},  /* fuzzy */
/* sv */ {IDCLS_SET_RAW_DRIVE_DEVICE_SV, ""},  /* fuzzy */
/* tr */ {IDCLS_SET_RAW_DRIVE_DEVICE_TR, ""},  /* fuzzy */
#endif
#endif

};