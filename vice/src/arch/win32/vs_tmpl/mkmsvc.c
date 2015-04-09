/*
 * mkmsvc - win32 project files generation program.
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

#include <ctype.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

/* Tested and confirmed working:
 *
 * MSVC 4.0 (mips)
 * VS98/MSVC 6.0 (x86)
 * VS2002/MSVC 7.0 (x86)
 * VS2003/MSVC 7.1 (x86)
 * VS2005/MSVC 8.0 (x86/x64/ia64)
 * VS2008/MSVC 9.0 (x86/x64/ia64)
 * VS2010/MSVC 10.0 (x86/x64)
 * VS2012/MSVC 11.0 (x86/x64)
 * VS2013/MSVC 12.0 (x86/x64)
 */

/* Enable debug options */
/* #define MKMSVC_DEBUG 1 */

#define CP_PROJECT_NAMES_STRING       "PROJECTS ="
#define CP_PROJECT_NAMES_SIZE         sizeof(CP_PROJECT_NAMES_STRING) - 1

#define CP_PROJECT_NAMES_NAT_STRING   "PROJECTS_NATIVE ="
#define CP_PROJECT_NAMES_NAT_SIZE     sizeof(CP_PROJECT_NAMES_NAT_STRING) - 1

#define CP_PROJECT_NAMES_SDL_STRING   "PROJECTS_SDL ="
#define CP_PROJECT_NAMES_SDL_SIZE     sizeof(CP_PROJECT_NAMES_SDL_STRING) - 1

#define CP_NAME_STRING                "PROJECTNAME = "
#define CP_NAME_SIZE                  sizeof(CP_NAME_STRING) - 1

#define CP_TYPE_STRING                "PROJECTTYPE = "
#define CP_TYPE_SIZE                  sizeof(CP_TYPE_STRING) - 1

#define CP_DEPS_STRING                "PROJECTDEPS ="
#define CP_DEPS_SIZE                  sizeof(CP_DEPS_STRING) - 1

#define CP_LIBS_STRING                "PROJECTLIBS ="
#define CP_LIBS_SIZE                  sizeof(CP_LIBS_STRING) - 1

#define CP_INCLUDES_STRING            "INCLUDEDIRS ="
#define CP_INCLUDES_SIZE              sizeof(CP_INCLUDES_STRING) - 1

#define CP_SOURCES_STRING             "SOURCES ="
#define CP_SOURCES_SIZE               sizeof(CP_SOURCES_STRING) - 1

#define CP_SDL_SOURCES_STRING         "SDL_SOURCES ="
#define CP_SDL_SOURCES_SIZE           sizeof(CP_SDL_SOURCES_STRING) - 1

#define CP_NATIVE_SOURCES_STRING      "NATIVE_SOURCES ="
#define CP_NATIVE_SOURCES_SIZE        sizeof(CP_NATIVE_SOURCES_STRING) - 1

#define CP_CPUSOURCES_STRING          "CPUSOURCES ="
#define CP_CPUSOURCES_SIZE            sizeof(CP_CPUSOURCES_STRING) - 1

#define CP_CUSTOM_MSG_STRING          "CUSTOMMSG = "
#define CP_CUSTOM_MSG_SIZE            sizeof(CP_CUSTOM_MSG_STRING) - 1

#define CP_CUSTOM_SRC_STRING          "CUSTOMSOURCE = "
#define CP_CUSTOM_SRC_SIZE            sizeof(CP_CUSTOM_SRC_STRING) - 1

#define CP_CUSTOM_DEPS_STRING         "CUSTOMDEPS = "
#define CP_CUSTOM_DEPS_SIZE           sizeof(CP_CUSTOM_DEPS_STRING) - 1

#define CP_CUSTOM_OUTPUT_STRING       "CUSTOMOUTPUT = "
#define CP_CUSTOM_OUTPUT_SIZE         sizeof(CP_CUSTOM_OUTPUT_STRING) - 1

#define CP_CUSTOM_COMMAND_STRING      "CUSTOMCOMMAND = "
#define CP_CUSTOM_COMMAND_SIZE        sizeof(CP_CUSTOM_COMMAND_STRING) - 1

#define CP_POST_CUSTOM_MSG_STRING     "POST_CUSTOMMSG = "
#define CP_POST_CUSTOM_MSG_SIZE       sizeof(CP_POST_CUSTOM_MSG_STRING) - 1

#define CP_POST_CUSTOM_SRC_STRING     "POST_CUSTOMSOURCE = "
#define CP_POST_CUSTOM_SRC_SIZE       sizeof(CP_POST_CUSTOM_SRC_STRING) - 1

#define CP_POST_CUSTOM_OUTPUT_STRING  "POST_CUSTOMOUTPUT = "
#define CP_POST_CUSTOM_OUTPUT_SIZE    sizeof(CP_POST_CUSTOM_OUTPUT_STRING) - 1

#define CP_POST_CUSTOM_COMMAND_STRING "POST_CUSTOMCOMMAND = "
#define CP_POST_CUSTOM_COMMAND_SIZE   sizeof(CP_POST_CUSTOM_COMMAND_STRING) - 1

#define CP_CC_SOURCE_PATH_STRING      "CCSOURCEPATH = "
#define CP_CC_SOURCE_PATH_SIZE        sizeof(CP_CC_SOURCE_PATH_STRING) - 1

#define CP_CC_SOURCES_STRING          "CCSOURCES ="
#define CP_CC_SOURCES_SIZE            sizeof(CP_CC_SOURCES_STRING) - 1

#define CP_RES_SOURCE_STRING		"RESSOURCE = "
#define CP_RES_SOURCE_SIZE			sizeof(CP_RES_SOURCE_STRING) - 1

#define CP_RES_DEPS_STRING			"RESDEPS = "
#define CP_RES_DEPS_SIZE			sizeof(CP_RES_DEPS_STRING) - 1

#define CP_RES_OUTPUT_STRING		"RESOUTPUT = "
#define CP_RES_OUTPUT_SIZE			sizeof(CP_RES_OUTPUT_STRING) - 1

#define CP_TYPE_LIBRARY   1
#define CP_TYPE_CONSOLE   2
#define CP_TYPE_GUI       3

#define MAX_DIRS        400
#define MAX_DEP_NAMES   400
#define MAX_NAMES       800
#define MAX_CPU_NAMES   10
#define MAX_LIBS        50

/* include ffmpeg support */
static int ffmpeg = 0;

/* Treat cpu sources as normal sources for indicated level and up */
static int cpu_source_level = 70;

/* Current project level */
static int current_level = 0;

/* Main project file handle */
static FILE *mainfile = NULL;

/* project names */
static char *project_names[MAX_NAMES];

/* sdl specific project names */
static char *project_names_sdl[MAX_NAMES];

/* native specific project names */
static char *project_names_native[MAX_NAMES];

/* current project file parameters */
static char *cp_name = NULL;
static int cp_type = -1;
static char *cp_dep_names[MAX_DEP_NAMES];
static char *cp_include_dirs[MAX_DIRS];
static char *cp_source_names[MAX_NAMES];
static char *cp_sdl_source_names[MAX_NAMES];
static char *cp_native_source_names[MAX_NAMES];
static char *cp_cpusource_names[MAX_CPU_NAMES];
static char *cp_libs = NULL;
static char *cp_libs_elements[MAX_LIBS];
static char *cp_custom_message = NULL;
static char *cp_custom_source = NULL;
static char *cp_custom_deps[MAX_DEP_NAMES];
static char *cp_custom_output = NULL;
static char *cp_custom_command = NULL;
static char *cp_post_custom_message = NULL;
static char *cp_post_custom_source = NULL;
static char *cp_post_custom_output = NULL;
static char *cp_post_custom_command = NULL;
static char *cp_cc_source_path = NULL;
static char *cp_cc_source_names[MAX_NAMES];
static char *cp_res_source_name = NULL;
static char *cp_res_deps[MAX_DEP_NAMES];
static char *cp_res_output_name = NULL;

/* read buffer related */
static char *names_buffer = NULL;
static char *read_buffer = NULL;
static int read_buffer_line = 0;
static int read_buffer_pos = 0;
static int read_buffer_len = 0;


/* Solution file information structure */
typedef struct project_info_s {
    char *name;
    char *dep[MAX_DEP_NAMES];
} project_info_t;

static project_info_t project_info[MAX_DEP_NAMES];

/* ---------------------------------------------------------------------- */

static size_t vs_strnlen(const char *string, size_t maxlen)
{
    const char *end = memchr(string, '\0', maxlen);
    return end ? end - string : maxlen;
}

static char *vs_strndup(const char *s, size_t n)
{
    size_t len = vs_strnlen(s, n);
    char *new = malloc(len + 1);

    if (new == NULL) {
        return NULL;
    }

    new[len] = '\0';
    return memcpy(new, s, len);
}

/* ---------------------------------------------------------------------- */

void pi_init(void)
{
    int i, j;

    /* init project_info */
    for (i = 0; i < MAX_DEP_NAMES; i++) {
        project_info[i].name = NULL;
        for (j = 0; j < MAX_DEP_NAMES; j++) {
            project_info[i].dep[j] = NULL;
        }
    }
}

static void pi_exit(void)
{
    int i, j;

    /* free all names */
    for (i = 0; project_info[i].name; i++) {
        if (project_info[i].dep[0]) {
            for (j = 0; project_info[i].dep[j]; j++) {
                free(project_info[i].dep[j]);
            }
        }
        free(project_info[i].name);
    }
}

static int pi_get_index_of_name(char *name)
{
    int i;
    int retval = -1;

    for (i = 0; project_info[i].name && retval == -1; i++) {
        if (!strcmp(name, project_info[i].name)) {
            retval = i;
        }
    }
    return retval;
}

static int pi_insert_name(char *name)
{
    int i = 0;
    int found = 0;

    while (found == 0) {
        if (project_info[i].name) {
            if (!strcmp(project_info[i].name, name)) {
                found = 2;
            } else {
                i++;
            }
        } else {
            found = 1;
        }
    }

    if (found == 1) {
        project_info[i].name = strdup(name);
    }

    return i;
}

static void pi_insert_deps(char **names, int index)
{
    int i;

    for (i = 0; names[i]; i++) {
        project_info[index].dep[i] = strdup(names[i]);
    }
}

static int test_win32_exception(char *name)
{
    if (!strcmp(name, "geninfocontrib")) {
        return 1;
    }

    if (!strcmp(name, "genmsvcver")) {
        return 1;
    }

    if (!strcmp(name, "gentranslate")) {
        return 1;
    }

    if (!strcmp(name, "genwinres")) {
        return 1;
    }

    return 0;
}

static int ffmpeg_file(char *name)
{
    if (!strcmp(name, "avcodec")) {
        return 1;
    }

    if (!strcmp(name, "avcodec-x86")) {
        return 1;
    }
	
    if (!strcmp(name, "avfilter")) {
        return 1;
    }

    if (!strcmp(name, "avformat")) {
        return 1;
    }
	
    if (!strcmp(name, "avutil")) {
        return 1;
    }

    if (!strcmp(name, "avutil-x86")) {
        return 1;
    }

    if (!strcmp(name, "compat")) {
        return 1;
    }

    if (!strcmp(name, "lame")) {
        return 1;
    }

    if (!strcmp(name, "postproc")) {
        return 1;
    }

    if (!strcmp(name, "swresample")) {
        return 1;
    }

    if (!strcmp(name, "swresample-x86")) {
        return 1;
    }

    if (!strcmp(name, "swscale")) {
        return 1;
    }

    if (!strcmp(name, "swscale-x86")) {
        return 1;
    }

    if (!strcmp(name, "x264")) {
        return 1;
    }

    if (!strcmp(name, "x264common")) {
        return 1;
    }

    if (!strcmp(name, "x264encoder")) {
        return 1;
    }

    if (!strcmp(name, "x264filter")) {
        return 1;
    }

    if (!strcmp(name, "x264input")) {
        return 1;
    }

    if (!strcmp(name, "x264output")) {
        return 1;
    }

    return 0;
}

/* ---------------------------------------------------------------------- */

static int read_template_file(char *fname, int sdl);

static char *project_lib_type[MAX_NAMES];

static int is_lib_type(char *name)
{
    int i;

    if (project_lib_type[0]) {
        for (i = 0; project_lib_type[i]; i++) {
            if (!strcmp(project_lib_type[i], name)) {
                return 1;
            }
        }
    }
    return 0;
}

static int insert_lib_type(char *name)
{
    int i = 0;

    /* No resid* projects or x64dtv or xscpu64 projects for winmips */
    if (!strncmp(name, "resid", 5) || !strcmp(name, "x64dtv") || !strcmp(name, "xscpu64")) {
        return 0;
    }

    if (project_lib_type[0]) {
        for (i = 0; project_lib_type[i]; i++) {
        }
    } 
    project_lib_type[i] = strdup(name);
    project_lib_type[i + 1] = NULL;

    return 0;
}

static void free_preparse(void)
{
    int i;

    if (project_lib_type[0]) {
        for (i = 0; project_lib_type[i]; i++) {
            free(project_lib_type[i]);
        }
    }
}

static int preparse_project(char *name)
{
    int error = read_template_file(name, 1);

    if (!error && (cp_type == CP_TYPE_LIBRARY)) {
        error = insert_lib_type(cp_name);
    }

    if (cp_libs) {
        free(cp_libs);
        cp_libs = NULL;
    }

    if (read_buffer) {
        free(read_buffer);
        read_buffer = NULL;
        read_buffer_line = 0;
        read_buffer_pos = 0;
        read_buffer_len = 0;
    }

    return error;
}

/* ---------------------------------------------------------------------- */

static char *msvc_cc_inc = "/I &quot;..\\msvc&quot;";

static char *msvc_cc_inc_sid = "/I &quot;..\\msvc&quot; /I &quot;..\\..\\../&quot; /I &quot;../&quot; /I &quot;..\\..\\..\\sid&quot;";

static char *msvc_cc_inc_sdl = "/I &quot;./&quot; /I &quot;../&quot;";

static char *msvc_cc_inc_sid_sdl = "/I &quot;./&quot; /I &quot;..\\..\\../&quot; /I &quot;../&quot; /I &quot;..\\..\\..\\sid&quot;";

static char *msvc_file_end = "\t\t</File>\r\n";

static char *msvc_libs_console = "version.lib wsock32.lib";

static char *msvc_libs_gui[2] = {
    "comctl32.lib dsound.lib dxguid.lib winmm.lib version.lib wsock32.lib",
    "comctl32.lib version.lib winmm.lib wsock32.lib"
};

static char *msvc_libs_console_sdl = "version.lib wsock32.lib SDLmain.lib SDL.lib opengl32.lib";

static char *msvc_libs_gui_sdl = "comctl32.lib version.lib winmm.lib wsock32.lib SDLmain.lib SDL.lib opengl32.lib";

static char *msvc_preprodefs[2] = {
    "_DEBUG",
    "NDEBUG"
};

static char *msvc_cc_extra_noffmpeg[4] = {
    "/D &quot;_DEBUG&quot;",
    "/D &quot;NDEBUG&quot;",
    "/D &quot;NODIRECTX&quot; /D &quot;_DEBUG&quot;",
    "/D &quot;NODIRECTX&quot; /D &quot;NDEBUG&quot;"
};

static char *msvc_cc_extra_ffmpeg[4] = {
    "/D &quot;_DEBUG&quot; /D &quot;STATIC_FFMPEG&quot;",
    "/D &quot;NDEBUG&quot; /D &quot;STATIC_FFMPEG&quot;",
    "/D &quot;NODIRECTX&quot; /D &quot;_DEBUG&quot; /D &quot;STATIC_FFMPEG&quot;",
    "/D &quot;NODIRECTX&quot; /D &quot;NDEBUG&quot; /D &quot;STATIC_FFMPEG&quot;"
};

static char *msvc_flags[2] = {
    "/MTd /W3 /EHsc /Z7 /Od",
    "/MT /W3 /EHsc"
};

static char *msvc_flags_sdl[2] = {
    "/MDd /W3 /EHsc /Z7 /Od",
    "/MD /W3 /EHsc"
};

static char *msvc_platform[3] = {
    "Win32",
    "Itanium",
    "x64"
};

static char *msvc_platform_cap_start[3] = {
    "Win32",
    "Itanium",
    "X64"
};

static char *msvc_type_name[4] = {
    "DX Debug",
    "DX Release",
    "Debug",
    "Release"
};

static char *msvc_type[4] = {
    "DXDebug",
    "DXRelease",
    "Debug",
    "Release"
};

static char *msvc_type_sdl[2] = {
    "Debug",
    "Release"
};

static char *msvc_predefs_noffmpeg[4] = {
    "_DEBUG",
    "NDEBUG",
    "NODIRECTX,_DEBUG",
    "NODIRECTX,NDEBUG"
};

static char *msvc_predefs_ffmpeg[4] = {
    "_DEBUG,STATIC_FFMPEG",
    "NDEBUG,STATIC_FFMPEG",
    "NODIRECTX,_DEBUG,STATIC_FFMPEG",
    "NODIRECTX,NDEBUG,STATIC_FFMPEG"
};

static char *msvc_winid_copy[3] = {
    "winid_x86.bat",
    "winid_ia64.bat",
    "winid_x64.bat"
};

/* ---------------------------------------------------------------------- */

static char *msvc12_project_start = "﻿<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
                                    "<Project DefaultTargets=\"Build\" ToolsVersion=\"12.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\r\n"
                                    "  <ItemGroup Label=\"ProjectConfigurations\">\r\n";

static char *msvc12_pgc_gui = "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" Label=\"Configuration\">\r\n"
                              "    <ConfigurationType>Application</ConfigurationType>\r\n"
                              "    <UseOfMfc>false</UseOfMfc>\r\n"
                              "    <PlatformToolset>v120</PlatformToolset>\r\n"
                              "  </PropertyGroup>\r\n";

static char *msvc12_pgc_console = "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" Label=\"Configuration\">\r\n"
                                  "    <ConfigurationType>Application</ConfigurationType>\r\n"
                                  "    <UseOfMfc>false</UseOfMfc>\r\n"
                                  "    <PlatformToolset>v120</PlatformToolset>\r\n"
                                  "    <CharacterSet>MultiByte</CharacterSet>\r\n"
                                  "  </PropertyGroup>\r\n";

static char *msvc12_pgc_library = "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" Label=\"Configuration\">\r\n"
                                  "    <ConfigurationType>StaticLibrary</ConfigurationType>\r\n"
                                  "    <UseOfMfc>false</UseOfMfc>\r\n"
                                  "    <PlatformToolset>v120</PlatformToolset>\r\n"
                                  "  </PropertyGroup>\r\n";

/* ---------------------------------------------------------------------- */

static char *msvc11_platform[3] = {
    "Win32",
    "ARM",
    "x64"
};

static char *msvc11_platform_cap_start[3] = {
    "Win32",
    "ARM",
    "X64"
};

static char *msvc11_winid_copy[3] = {
    "winid_x86.bat",
    "winid_arm.bat",
    "winid_x64.bat"
};

static char *msvc11_pgc_gui = "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" Label=\"Configuration\">\r\n"
                              "    <ConfigurationType>Application</ConfigurationType>\r\n"
                              "    <UseOfMfc>false</UseOfMfc>\r\n"
                              "    <PlatformToolset>v110</PlatformToolset>\r\n"
                              "  </PropertyGroup>\r\n";

static char *msvc11_pgc_console = "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" Label=\"Configuration\">\r\n"
                                  "    <ConfigurationType>Application</ConfigurationType>\r\n"
                                  "    <UseOfMfc>false</UseOfMfc>\r\n"
                                  "    <PlatformToolset>v110</PlatformToolset>\r\n"
                                  "    <CharacterSet>MultiByte</CharacterSet>\r\n"
                                  "  </PropertyGroup>\r\n";

static char *msvc11_pgc_library = "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" Label=\"Configuration\">\r\n"
                                  "    <ConfigurationType>StaticLibrary</ConfigurationType>\r\n"
                                  "    <UseOfMfc>false</UseOfMfc>\r\n"
                                  "    <PlatformToolset>v110</PlatformToolset>\r\n"
                                  "  </PropertyGroup>\r\n";

/* ---------------------------------------------------------------------- */

static char *msvc10_cc_extra_noffmpeg[4] = {
    "/D \"_DEBUG\"",
    "/D \"NDEBUG\"",
    "/D \"NODIRECTX\" /D \"_DEBUG\"",
    "/D \"NODIRECTX\" /D \"NDEBUG\""
};

static char *msvc10_cc_extra_ffmpeg[4] = {
    "/D \"_DEBUG\" /D \"STATIC_FFMPEG\"",
    "/D \"NDEBUG\" /D \"STATIC_FFMPEG\"",
    "/D \"NODIRECTX\" /D \"_DEBUG\" /D \"STATIC_FFMPEG\"",
    "/D \"NODIRECTX\" /D \"NDEBUG\" /D \"STATIC_FFMPEG\""
};

static char *msvc10_cc_inc = "/I \"..\\msvc\"";

static char *msvc10_cc_inc_sdl = "/I \"./\" /I \"../\"";

static char *msvc10_cc_inc_sid = "/I \"..\\msvc\" /I \"..\\..\\../\" /I \"../\" /I \"..\\..\\..\\sid\"";

static char *msvc10_cc_inc_sid_sdl = "/I \"./\" /I \"..\\..\\../\" /I \"../\" /I \"..\\..\\..\\sid\"";

static char *msvc10_libs_console = "version.lib;wsock32.lib;advapi32.lib";

static char *msvc10_libs_console_sdl = "version.lib;wsock32.lib;SDLmain.lib;SDL.lib;opengl32.lib";

static char *msvc10_libs_gui[2] = {
    "comctl32.lib;dsound.lib;dxguid.lib;winmm.lib;version.lib;wsock32.lib;shell32.lib;gdi32.lib;comdlg32.lib;advapi32.lib",
    "comctl32.lib;version.lib;winmm.lib;wsock32.lib;shell32.lib;gdi32.lib;comdlg32.lib;advapi32.lib"
};

static char *msvc10_libs_gui_sdl = "comctl32.lib;version.lib;winmm.lib;wsock32.lib;SDLmain.lib;SDL.lib;opengl32.lib";

static char *msvc10_project_start = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
                                    "<Project DefaultTargets=\"Build\" ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\r\n"
                                    "  <ItemGroup Label=\"ProjectConfigurations\">\r\n";

static char *msvc10_pci = "    <ProjectConfiguration Include=\"%s|%s\">\r\n"
                          "      <Configuration>%s</Configuration>\r\n"
                          "      <Platform>%s</Platform>\r\n"
                          "    </ProjectConfiguration>\r\n";

static char *msvc10_item_group_end = "  </ItemGroup>\r\n";

static char *msvc10_pgl = "  <PropertyGroup Label=\"Globals\">\r\n"
                          "    <ProjectGuid>{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}</ProjectGuid>\r\n"
                          "  </PropertyGroup>\r\n"
                          "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\r\n";

static char *msvc10_pgc_gui = "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" Label=\"Configuration\">\r\n"
                              "    <ConfigurationType>Application</ConfigurationType>\r\n"
                              "    <UseOfMfc>false</UseOfMfc>\r\n"
                              "  </PropertyGroup>\r\n";

static char *msvc10_pgc_console = "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" Label=\"Configuration\">\r\n"
                                  "    <ConfigurationType>Application</ConfigurationType>\r\n"
                                  "    <UseOfMfc>false</UseOfMfc>\r\n"
                                  "    <CharacterSet>MultiByte</CharacterSet>\r\n"
                                  "  </PropertyGroup>\r\n";

static char *msvc10_pgc_library = "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" Label=\"Configuration\">\r\n"
                                  "    <ConfigurationType>StaticLibrary</ConfigurationType>\r\n"
                                  "    <UseOfMfc>false</UseOfMfc>\r\n"
                                  "  </PropertyGroup>\r\n";

static char *msvc10_import_project = "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\r\n"
                                     "  <ImportGroup Label=\"ExtensionSettings\">\r\n"
                                     "  </ImportGroup>\r\n";

static char *msvc10_import_cc = "  <ImportGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" Label=\"PropertySheets\">\r\n"
                                "    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />\r\n"
                                "    <Import Project=\"$(VCTargetsPath)Microsoft.CPP.UpgradeFromVC71.props\" />\r\n"
                                "  </ImportGroup>\r\n";

static char *msvc10_user_macros_start = "  <PropertyGroup Label=\"UserMacros\" />\r\n"
                                        "  <PropertyGroup>\r\n"
                                        "    <_ProjectFileVersion>10.0.30319.1</_ProjectFileVersion>\r\n";

static char *msvc10_outdir_cond_app = "    <OutDir Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">.\\..\\..\\..\\..\\data\\</OutDir>\r\n";

static char *msvc10_outdir_cond_lib = "    <OutDir Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">.\\libs\\%s\\%s%s\\</OutDir>\r\n";

static char *msvc10_intdir_cond = "    <IntDir Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">.\\libs\\%s\\%s%s\\</IntDir>\r\n";

static char *msvc10_lic = "    <LinkIncremental Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">%s</LinkIncremental>\r\n";

static char *msvc10_car = "    <CodeAnalysisRuleSet Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">AllRules.ruleset</CodeAnalysisRuleSet>\r\n"
                          "    <CodeAnalysisRules Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" />\r\n"
                          "    <CodeAnalysisRuleAssemblies Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" />\r\n";

static char *msvc10_pg_end = "  </PropertyGroup>\r\n";

static char *msvc10_idg_cond = "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">\r\n";

static char *msvc10_post_custom = "    <CustomBuildStep>\r\n"
                                  "      <Command>%s\r\n"
                                  "</Command>\r\n"
                                  "      <Outputs>%s;%%(Outputs)</Outputs>\r\n"
                                  "    </CustomBuildStep>\r\n";

static char *msvc10_midl_gui = "    <Midl>\r\n"
                               "      <PreprocessorDefinitions>%s;%%(PreprocessorDefinitions)</PreprocessorDefinitions>\r\n"
                               "      <MkTypLibCompatible>true</MkTypLibCompatible>\r\n"
                               "      <SuppressStartupBanner>true</SuppressStartupBanner>\r\n"
                               "      <TargetEnvironment>%s</TargetEnvironment>\r\n"
                               "      <TypeLibraryName>.\\..\\..\\..\\..\\data\\%s.tlb</TypeLibraryName>\r\n"
                               "    </Midl>\r\n";

static char *msvc10_midl_console = "    <Midl>\r\n"
                                   "%s"
                                   "      <TypeLibraryName>.\\..\\..\\..\\..\\data\\%s.tlb</TypeLibraryName>\r\n"
                                   "    </Midl>\r\n";

static char *msvc10_midl_lib = "    <Midl>\r\n"
                               "      <TargetEnvironment>%s</TargetEnvironment>\r\n"
                               "    </Midl>\r\n";

static char *msvc10_opti = "      <Optimization>Disabled</Optimization>\r\n";

static char *msvc10_ife = "      <InlineFunctionExpansion>OnlyExplicitInline</InlineFunctionExpansion>\r\n";

static char *msvc10_clcompile_part1 = "    <ClCompile>\r\n"
                                      "%s";

static char *msvc10_aid = "      <AdditionalIncludeDirectories>..\\msvc;..\\;..\\..\\..\\;";

static char *msvc10_aid_sdl = "      <AdditionalIncludeDirectories>.\\;..\\;..\\..\\..\\;";

static char *msvc10_aid_cc = "      <AdditionalIncludeDirectories>..\\msvc;%%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\r\n";

static char *msvc10_aid_cc_sdl = "      <AdditionalIncludeDirectories>.\\;..\\;%%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\r\n";

static char *msvc10_aid_sid = "      <AdditionalIncludeDirectories>..\\msvc;..\\;..\\..\\..\\;..\\..\\..\\resid;%%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\r\n";

static char *msvc10_aid_sid_sdl = "      <AdditionalIncludeDirectories>.\\;..\\;..\\..\\..\\;..\\..\\..\\resid;%%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\r\n";

static char *msvc10_aid_end = "%%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\r\n";

static char *msvc10_predefs_noffmpeg[4] = {
    "_DEBUG",
    "NDEBUG",
    "NODIRECTX;_DEBUG",
    "NODIRECTX;NDEBUG"
};

static char *msvc10_predefs_ffmpeg[4] = {
    "_DEBUG;STATIC_FFMPEG",
    "NDEBUG;STATIC_FFMPEG",
    "NODIRECTX;_DEBUG;STATIC_FFMPEG",
    "NODIRECTX;NDEBUG;STATIC_FFMPEG"
};

static char *msvc10_preprodefs = "      <PreprocessorDefinitions>WIN32;_WINDOWS;IDE_COMPILE;DONT_USE_UNISTD_H;%s;%%(PreprocessorDefinitions)</PreprocessorDefinitions>\r\n";

static char *msvc10_preprodefs_cc = "      <PreprocessorDefinitions>WIN32;_WINDOWS;IDE_COMPILE;DONT_USE_UNISTD_H;%s;PACKAGE=\"resid\";VERSION=\"0.7\";SIZEOF_INT=4;%%(PreprocessorDefinitions)</PreprocessorDefinitions>\r\n";

static char *msvc10_string_pooling = "      <StringPooling>true</StringPooling>\r\n"
                                     "      <RuntimeLibrary>MultiThreaded</RuntimeLibrary>\r\n"
                                     "      <FunctionLevelLinking>true</FunctionLevelLinking>\r\n";

static char *msvc10_string_pooling_sdl = "      <StringPooling>true</StringPooling>\r\n"
                                         "      <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>\r\n"
                                         "      <FunctionLevelLinking>true</FunctionLevelLinking>\r\n";

static char *msvc10_rtl_mtd = "      <RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>\r\n";

static char *msvc10_rtl_mtd_sdl = "      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>\r\n";

static char *msvc10_dif = "      <DebugInformationFormat>OldStyle</DebugInformationFormat>\r\n";

static char *msvc10_clcompile_part2 = "      <PrecompiledHeader>\r\n"
                                      "      </PrecompiledHeader>\r\n"
                                      "      <PrecompiledHeaderOutputFile>.\\libs\\%s\\%s%s\\%s.pch</PrecompiledHeaderOutputFile>\r\n"
                                      "      <AssemblerListingLocation>.\\libs\\%s\\%s%s\\</AssemblerListingLocation>\r\n"
                                      "      <ObjectFileName>.\\libs\\%s\\%s%s\\</ObjectFileName>\r\n"
                                      "      <ProgramDataBaseFileName>.\\libs\\%s\\%s%s\\</ProgramDataBaseFileName>\r\n"
                                      "      <WarningLevel>Level3</WarningLevel>\r\n"
                                      "      <SuppressStartupBanner>true</SuppressStartupBanner>\r\n"
                                      "%s"
                                      "      <CompileAs>Default</CompileAs>\r\n"
                                      "    </ClCompile>\r\n";

static char *msvc10_res_compiler_gui = "    <ResourceCompile>\r\n"
                                       "      <PreprocessorDefinitions>WIN32;_WINDOWS;IDE_COMPILE;DONT_USE_UNISTD_H;%s;%%(PreprocessorDefinitions)</PreprocessorDefinitions>\r\n"
                                       "      <Culture>0x0409</Culture>\r\n"
                                       "      <AdditionalIncludeDirectories>..\\msvc;..\\;..\\..\\..\\;%%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\r\n"
                                       "    </ResourceCompile>\r\n";

static char *msvc10_res_compiler_console = "    <ResourceCompile>\r\n"
                                           "      <PreprocessorDefinitions>%s;%%(PreprocessorDefinitions)</PreprocessorDefinitions>\r\n"
                                           "      <Culture>0x0409</Culture>\r\n"
                                           "    </ResourceCompile>\r\n";

static char *msvc10_res_compiler_lib = "    <ResourceCompile>\r\n"
                                       "      <Culture>0x0409</Culture>\r\n"
                                       "    </ResourceCompile>\r\n";

static char *msvc10_gdi = "      <GenerateDebugInformation>true</GenerateDebugInformation>\r\n";

static char *msvc10_lib = "    <Lib>\r\n"
                          "      <OutputFile>.\\libs\\%s\\%s%s\\%s.lib</OutputFile>\r\n"
                          "      <SuppressStartupBanner>true</SuppressStartupBanner>\r\n"
                          "    </Lib>\r\n";

static char *msvc10_tm_arm = "      <TargetMachine>MachineARM</TargetMachine>\r\n";

static char *msvc10_tm_ia64 = "      <TargetMachine>MachineIA64</TargetMachine>\r\n";

static char *msvc10_tm_x64 = "      <TargetMachine>MachineX64</TargetMachine>\r\n";

static char *msvc10_link = "    <Link>\r\n"
                           "      <AdditionalDependencies>%s;%%(AdditionalDependencies)</AdditionalDependencies>\r\n"
                           "      <OutputFile>.\\..\\..\\..\\..\\data\\%s.exe</OutputFile>\r\n"
                           "      <SuppressStartupBanner>true</SuppressStartupBanner>\r\n"
                           "%s"
                           "      <ProgramDatabaseFile>.\\..\\..\\..\\..\\data\\%s.pdb</ProgramDatabaseFile>\r\n"
                           "      <SubSystem>%s</SubSystem>\r\n"
                           "      <RandomizedBaseAddress>false</RandomizedBaseAddress>\r\n"
                           "      <DataExecutionPrevention>\r\n"
                           "      </DataExecutionPrevention>\r\n"
                           "%s"
                           "    </Link>\r\n";

static char *msvc10_idg_end = "  </ItemDefinitionGroup>\r\n";

static char *msvc10_ig_start = "  <ItemGroup>\r\n";

static char *msvc10_ig_end = "  </ItemGroup>\r\n";

static char *msvc10_source = "    <ClCompile Include=\"..\\..\\..\\%s\" />\r\n";

static char *msvc10_cc_cbi = "    <CustomBuild Include=\"..\\..\\..\\%s\\%s.cc\">\r\n";

static char *msvc10_cc_cc = "      <Command Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">cl /nologo %s %s /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" %s /D PACKAGE=\\\"resid\\\" /D VERSION=\\\"0.7\\\" /D SIZEOF_INT=4 /Fp\"libs\\%s\\%s%s\\%s.pch\" /Fo\"libs\\%s\\%s%s/\" /Fd\"libs\\%s\\%s%s/\" /FD /TP /c \"%%(FullPath)\"\r\n"
                            "</Command>\r\n";

static char *msvc10_cc_oc = "      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">libs\\%s\\%s%s\\%%(Filename).obj;%%(Outputs)</Outputs>\r\n";

static char *msvc10_cc_cbi_end = "    </CustomBuild>\r\n";

static char *msvc10_res_cbi = "    <CustomBuild Include=\"..\\%s\">\r\n";

static char *msvc10_res_cc = "      <Command Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">copy /b ";

static char *msvc10_res_cc_end = "%s /b\r\n"
                                 "</Command>\r\n";

static char *msvc10_res_aic = "      <AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">..\\..\\..\\debug.h;";

static char *msvc10_res_aic_end = "%%(AdditionalInputs)</AdditionalInputs>\r\n";

static char *msvc10_res_oc = "      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">%s;%%(Outputs)</Outputs>\r\n";

static char *msvc10_res_manifest = "    </CustomBuild>\r\n"
                                   "    <ResourceCompile Include=\"%s\" />\r\n"
                                   "  </ItemGroup>\r\n"
                                   "  <ItemGroup>\r\n"
                                   "    <CustomBuildStep Include=\"..\\vice.manifest\">\r\n";

static char *msvc10_efbc = "      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">true</ExcludedFromBuild>\r\n";

static char *msvc10_res_end = "    </CustomBuildStep>\r\n"
                              "  </ItemGroup>\r\n";

static char *msvc10_custom_start = "  <ItemGroup>\r\n"
                                   "    <CustomBuild Include=\"%s\">\r\n";

static char *msvc10_custom_mc = "      <Message Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">%s</Message>\r\n";

static char *msvc10_custom_cc = "      <Command Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">%s\r\n"
                                "</Command>\r\n";

static char *msvc10_custom_aic = "      <AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">";

static char *msvc10_custom_oc = "%%(AdditionalInputs)</AdditionalInputs>\r\n"
                                "      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">%s;%%(Outputs)</Outputs>\r\n";

static char *msvc10_winid_start = "    <CustomBuild Include=\"winid.bat\">\r\n";

static char *msvc10_winid_middle = "      <Message Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">Generating winid.bat</Message>\r\n"
                                   "      <Command Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">copy %s winid.bat\r\n"
                                   "</Command>\r\n"
                                   "      <AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">.\\%s;%%(AdditionalInputs)</AdditionalInputs>\r\n"
                                   "      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\">.\\winid.bat;%%(Outputs)</Outputs>\r\n";

static char *msvc10_pri = "    <ProjectReference Include=\"%s.vcxproj\">\r\n"
                          "      <Project>{8bc9ceb8-8b4a-11d0-8d12-00a0c91bc9%02x}</Project>\r\n"
                          "      <ReferenceOutputAssembly>false</ReferenceOutputAssembly>\r\n"
                          "    </ProjectReference>\r\n";

static char *msvc10_project_end = "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\r\n"
                                  "  <ImportGroup Label=\"ExtensionTargets\">\r\n"
                                  "  </ImportGroup>\r\n"
                                  "</Project>\r\n";

static char *msvc10_main_project = "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"%s\", \"%s.vcxproj\", \"{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}\"\r\n"
                                   "EndProject\r\n";

static char *msvc10_main_global_start = "Global\r\n"
                                        "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\r\n";

static char *msvc10_main_confs = "\t\t%s|%s = %s|%s\r\n";

static char *msvc10_main_global_middle = "\tEndGlobalSection\r\n"
                                         "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\r\n";

static char *msvc10_main_configs = "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.%s|%s.ActiveCfg = %s|%s\r\n"
                                   "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.%s|%s.Build.0 = %s|%s\r\n";

static char *msvc10_main_end = "\tEndGlobalSection\r\n"
                               "\tGlobalSection(SolutionProperties) = preSolution\r\n"
                               "\t\tHideSolutionNode = FALSE\r\n"
                               "\tEndGlobalSection\r\n"
                               "EndGlobal\r\n";

static void generate_msvc10_11_12_sln(int msvc11, int msvc12, int sdl)
{
    int i, j, k;
    int exc = 0;
    int max_k = 3;
    int max_i = (sdl) ? 2 : 4;
    char **msvc10_11_platform = (msvc11) ? msvc11_platform : msvc_platform;
    char **type_name = (sdl) ? msvc_type_sdl : msvc_type_name;

    for (j = 0; project_info[j].name; j++) {
        fprintf(mainfile, msvc10_main_project, project_info[j].name, project_info[j].name, j);
    }
    fprintf(mainfile, msvc10_main_global_start);
    for (i = 0; i < max_i; i++) {
        for (k = 0; k < max_k; k++) {
            fprintf(mainfile, msvc10_main_confs, type_name[i], msvc10_11_platform[k], type_name[i], msvc10_11_platform[k]);
        }
    }
    fprintf(mainfile, msvc10_main_global_middle);
    for (j = 0; project_info[j].name; j++) {
        exc = test_win32_exception(project_info[j].name);
        for (i = 0; i < max_i; i++) {
            for (k = 0; k < max_k; k++) {
                fprintf(mainfile, msvc10_main_configs, j, type_name[i], msvc10_11_platform[k], type_name[i], (exc) ? "Win32" : msvc10_11_platform[k], j, type_name[i], msvc10_11_platform[k], type_name[i], (exc) ? "Win32" : msvc10_11_platform[k]);
            }
        }
    }
    fprintf(mainfile, msvc10_main_end);
}

static int open_msvc10_11_12_main_project(int msvc11, int msvc12, int sdl)
{
    pi_init();

    if (msvc12) {
        if (sdl) {
            if (ffmpeg) {
                mainfile = fopen("../../sdl/win32-msvc12-ffmpeg/vice.sln", "wb");
            } else {
                mainfile = fopen("../../sdl/win32-msvc12/vice.sln", "wb");
            }
        } else {
            if (ffmpeg) {
                mainfile = fopen("../vs12-ffmpeg/vice.sln", "wb");
            } else {
                mainfile = fopen("../vs12/vice.sln", "wb");

			}
        }
	} else if (msvc11) {
        if (sdl) {
            if (ffmpeg) {
                mainfile = fopen("../../sdl/win32-msvc11-ffmpeg/vice.sln", "wb");
            } else {
                mainfile = fopen("../../sdl/win32-msvc11/vice.sln", "wb");
            }
        } else {
            if (ffmpeg) {
                mainfile = fopen("../vs11-ffmpeg/vice.sln", "wb");
            } else {
                mainfile = fopen("../vs11/vice.sln", "wb");
            }
        }
    } else {
        if (sdl) {
            if (ffmpeg) {
                mainfile = fopen("../../sdl/win32-msvc10-ffmpeg/vice.sln", "wb");
            } else {
                mainfile = fopen("../../sdl/win32-msvc10/vice.sln", "wb");
            }
        } else {
            if (ffmpeg) {
                mainfile = fopen("../vs10-ffmpeg/vice.sln", "wb");
            } else {
                mainfile = fopen("../vs10/vice.sln", "wb");
            }
        }
    }

    if (!mainfile) {
        printf("Cannot open 'vice.sln' for output\n");
        return 1;
    }

    if (msvc12) {
        fprintf(mainfile, "Microsoft Visual Studio Solution File, Format Version 12.00\r\n");
        fprintf(mainfile, "# Visual Studio 2013\r\n");
    } else {
        if (msvc11) {
            fprintf(mainfile, "Microsoft Visual Studio Solution File, Format Version 12.00\r\n");
            fprintf(mainfile, "# Visual Studio 2012\r\n");
        } else {
            fprintf(mainfile, "Microsoft Visual Studio Solution File, Format Version 11.00\r\n");
            fprintf(mainfile, "# Visual Studio 2010\r\n");
        }
    }

    return 0;
}

static void close_msvc10_11_12_main_project(int msvc11, int msvc12, int sdl)
{
    generate_msvc10_11_12_sln(msvc11, msvc12, sdl);
    pi_exit();
    fclose(mainfile);
}

static int output_msvc10_11_12_file(char *fname, int filelist, int msvc11, int msvc12, int sdl)
{
    char *filename;
    int retval = 0;
    FILE *outfile = NULL;
    int i, j, k;
    int index = 0;
    char *temp_string;
    int max_k = 3;
    int max_i = (sdl) ? 2 : 4;
    char **msvc10_11_platform = (msvc11) ? msvc11_platform : msvc_platform;
    char **msvc10_11_platform_cap_start = (msvc11) ? msvc11_platform_cap_start : msvc_platform_cap_start;
    char **msvc10_11_winid_copy = (msvc11) ? msvc11_winid_copy : msvc_winid_copy;
    char *rfname;
    char **type_name = (sdl) ? msvc_type_sdl : msvc_type_name;
    char **type = (sdl) ? msvc_type_sdl : msvc_type;
    char *libs;
    char **msvc10_cc_extra = (ffmpeg) ? msvc10_cc_extra_ffmpeg : msvc10_cc_extra_noffmpeg;
    char **msvc10_predefs = (ffmpeg) ? msvc10_predefs_ffmpeg : msvc10_predefs_noffmpeg;
    char *msvc10_tm_extra = (msvc11) ? msvc10_tm_arm : msvc10_tm_ia64;

    if (!strcmp(fname, "arch_native") || !strcmp(fname, "arch_sdl")) {
        rfname = "arch";
    } else {
        rfname = fname;
    }

    if (filelist) {
        index = pi_insert_name(cp_name);
        if (cp_dep_names[0]) {
            pi_insert_deps(cp_dep_names, index);
        }
        if (sdl) {
            if (ffmpeg) {
                filename = malloc(strlen(rfname) + sizeof("../../sdl/win32-msvc10-ffmpeg/.vcxproj"));
            } else {
                filename = malloc(strlen(rfname) + sizeof("../../sdl/win32-msvc10/.vcxproj"));
            }
        } else {
            if (ffmpeg) {
                filename = malloc(strlen(rfname) + sizeof("../vs10-ffmpeg/.vcxproj"));
            } else {
                filename = malloc(strlen(rfname) + sizeof("../vs10/.vcxproj"));
            }
        }
        if (msvc12) {
            if (sdl) {
                if (ffmpeg) {
                    sprintf(filename, "../../sdl/win32-msvc12-ffmpeg/%s.vcxproj", rfname);
                } else {
                    sprintf(filename, "../../sdl/win32-msvc12/%s.vcxproj", rfname);
                }
            } else {
                if (ffmpeg) {
                    sprintf(filename, "../vs12-ffmpeg/%s.vcxproj", rfname);
                } else {
                    sprintf(filename, "../vs12/%s.vcxproj", rfname);
                }
            }
        } else if (msvc11) {
            if (sdl) {
                if (ffmpeg) {
                    sprintf(filename, "../../sdl/win32-msvc11-ffmpeg/%s.vcxproj", rfname);
                } else {
                    sprintf(filename, "../../sdl/win32-msvc11/%s.vcxproj", rfname);
                }
            } else {
                if (ffmpeg) {
                    sprintf(filename, "../vs11-ffmpeg/%s.vcxproj", rfname);
                } else {
                    sprintf(filename, "../vs11/%s.vcxproj", rfname);
                }
            }
        } else {
            if (sdl) {
                if (ffmpeg) {
                    sprintf(filename, "../../sdl/win32-msvc10-ffmpeg/%s.vcxproj", rfname);
                } else {
                    sprintf(filename, "../../sdl/win32-msvc10/%s.vcxproj", rfname);
                }
            } else {
                if (ffmpeg) {
                    sprintf(filename, "../vs10-ffmpeg/%s.vcxproj", rfname);
                } else {
                    sprintf(filename, "../vs10/%s.vcxproj", rfname);
                }
            }
        }
    } else {
        filename = malloc(strlen(rfname) + sizeof(".vcxproj"));
        sprintf(filename, "%s.vcxproj", rfname);
    }

    outfile = fopen(filename, "wb");
    if (!outfile) {
        printf("Cannot open %s for output\n", filename);
        retval = 1;
    }

    if (!retval) {
        if (msvc12) {
            fprintf(outfile, msvc12_project_start);
        } else {
            fprintf(outfile, msvc10_project_start);
        }
        for (i = 0; i < max_i; i++) {
            for (k = 0; k < max_k; k++) {
                fprintf(outfile, msvc10_pci, type_name[i], msvc10_11_platform[k], type_name[i], msvc10_11_platform[k]);
            }
        }
        fprintf(outfile, msvc10_item_group_end);
        fprintf(outfile, msvc10_pgl, index);
        for (k = 0; k < max_k; k++) {
            for (i = 0; i < max_i; i++) {
                switch (cp_type) {
                    default:
                    case CP_TYPE_GUI:
                        if (msvc11) {
                            if (msvc12) {
                                fprintf(outfile, msvc12_pgc_gui, type_name[i], msvc10_11_platform[k]);
                            } else {
                                fprintf(outfile, msvc11_pgc_gui, type_name[i], msvc10_11_platform[k]);
                            }
                        } else {
                            fprintf(outfile, msvc10_pgc_gui, type_name[i], msvc10_11_platform[k]);
                        }
                        break;
                    case CP_TYPE_CONSOLE:
                        if (msvc11) {
                            if (msvc12) {
                                fprintf(outfile, msvc12_pgc_console, type_name[i], msvc10_11_platform[k]);
                            } else {
                                fprintf(outfile, msvc11_pgc_console, type_name[i], msvc10_11_platform[k]);
                            }
                        } else {
                            fprintf(outfile, msvc10_pgc_console, type_name[i], msvc10_11_platform[k]);
                        }
                        break;
                    case CP_TYPE_LIBRARY:
                        if (msvc11) {
                            if (msvc12) {
                                fprintf(outfile, msvc12_pgc_library, type_name[i], msvc10_11_platform[k]);
                            } else {
								fprintf(outfile, msvc11_pgc_library, type_name[i], msvc10_11_platform[k]);
                            }
                        } else {
                            fprintf(outfile, msvc10_pgc_library, type_name[i], msvc10_11_platform[k]);
                        }
                        break;
                }
            }
        }
        fprintf(outfile, msvc10_import_project);
        for (k = 0; k < max_k; k++) {
            for (i = 0; i < max_i; i++) {
                fprintf(outfile, msvc10_import_cc, type_name[i], msvc10_11_platform[k]);
            }
        }
        fprintf(outfile, msvc10_user_macros_start);
        for (i = 0; i < max_i; i++) {
            for (k = 0; k < max_k; k++) {
                if (cp_type != CP_TYPE_LIBRARY) {
                    fprintf(outfile, msvc10_outdir_cond_app, type_name[i], msvc10_11_platform[k]);
                } else {
                    fprintf(outfile, msvc10_outdir_cond_lib, type_name[i], msvc10_11_platform[k], cp_name, msvc10_11_platform[k], type[i]);
                }
                fprintf(outfile, msvc10_intdir_cond, type_name[i], msvc10_11_platform[k], cp_name, msvc10_11_platform[k], type[i]);
                if (cp_type != CP_TYPE_LIBRARY) {
                    fprintf(outfile, msvc10_lic, type_name[i], msvc10_11_platform[k], (i & 1) ? "false" : "true");
                }
            }
        }
        for (i = 0; i < max_i; i++) {
            for (k = 0; k < max_k; k++) {
                fprintf(outfile, msvc10_car, type_name[i], msvc10_11_platform[k], type_name[i], msvc10_11_platform[k], type_name[i], msvc10_11_platform[k]);
            }
        }
        fprintf(outfile, msvc10_pg_end);
        for (i = 0; i < max_i; i++) {
            for (k = 0; k < max_k; k++) {
                fprintf(outfile, msvc10_idg_cond, type_name[i], msvc10_11_platform[k]);
                if (cp_post_custom_message) {
                    fprintf(outfile, msvc10_post_custom, cp_post_custom_command, cp_post_custom_output);
                }
                switch (k) {
                    default:
                    case 0:
                        temp_string = "";
                        break;
                    case 1:
                        if (msvc11) {
                            temp_string = "      <TargetEnvironment>Itanium</TargetEnvironment>\r\n";
                        } else {
                            temp_string = "      <TargetEnvironment>ARM</TargetEnvironment>\r\n";
                        }
                        break;
                    case 2:
                        temp_string = "      <TargetEnvironment>X64</TargetEnvironment>\r\n";
                        break;
                }
                switch (cp_type) {
                    default:
                    case CP_TYPE_GUI:
                        fprintf(outfile, msvc10_midl_gui, msvc_preprodefs[i & 1], msvc10_11_platform_cap_start[k], cp_name);
                        break;
                    case CP_TYPE_CONSOLE:
                        fprintf(outfile, msvc10_midl_console, temp_string, cp_name);
                        break;
                    case CP_TYPE_LIBRARY:
                        if (k) {
                            fprintf(outfile, msvc10_midl_lib, msvc10_11_platform_cap_start[k]);
                        }
                        break;
                }
                fprintf(outfile, msvc10_clcompile_part1, (i & 1) ? msvc10_ife : msvc10_opti);
                if (cp_cc_source_path) {
                    if (cp_source_names[0]) {
                        if (sdl) {
                            fprintf(outfile, msvc10_aid_sid_sdl);
                        } else {
                            fprintf(outfile, msvc10_aid_sid);
                        }
                    } else {
                        if (sdl) {
                            fprintf(outfile, msvc10_aid_cc_sdl);
                        } else {
                            fprintf(outfile, msvc10_aid_cc);
                        }
                    }
                } else {
                    if (sdl) {
                        fprintf(outfile, msvc10_aid_sdl);
                    } else {
                        fprintf(outfile, msvc10_aid);
                    }
                    if (cp_include_dirs[0]) {
                        for (j = 0; cp_include_dirs[j]; j++) {
                            fprintf(outfile, "..\\..\\..\\%s;", cp_include_dirs[j]);
                        }
                    }
                    fprintf(outfile, msvc10_aid_end);
                }
                if (!cp_cc_source_path) {
                    fprintf(outfile, msvc10_preprodefs, msvc10_predefs[i]);
                } else {
                    fprintf(outfile, msvc10_preprodefs_cc, msvc10_predefs[i]);
                }
                if (i & 1) {
                    if (sdl) {
                        fprintf(outfile, msvc10_string_pooling_sdl);
                    } else {
                        fprintf(outfile, msvc10_string_pooling);
                    }
                } else {
                    if (sdl) {
                        fprintf(outfile, msvc10_rtl_mtd_sdl);
                    } else {
                        fprintf(outfile, msvc10_rtl_mtd);
                    }
                }
                fprintf(outfile, msvc10_clcompile_part2, cp_name, msvc10_11_platform[k], type[i], cp_name, cp_name, msvc10_11_platform[k], type[i], cp_name, msvc10_11_platform[k], type[i], cp_name, msvc10_11_platform[k], type[i], (i & 1) ? "" : msvc10_dif);
                switch (cp_type) {
                    default:
                    case CP_TYPE_GUI:
                        fprintf(outfile, msvc10_res_compiler_gui, msvc10_predefs[i]);
                        break;
                    case CP_TYPE_CONSOLE:
                        fprintf(outfile, msvc10_res_compiler_console, msvc10_predefs[i]);
                        break;
                    case CP_TYPE_LIBRARY:
                        fprintf(outfile, msvc10_res_compiler_lib);
                        break;
                }
                switch (k) {
                    default:
                    case 0:
                        temp_string = "";
                        break;
                    case 1:
                        temp_string = msvc10_tm_extra;
                        break;
                    case 2:
                        temp_string = msvc10_tm_x64;
                        break;
                }
                if (cp_type == CP_TYPE_LIBRARY) {
                    fprintf(outfile, msvc10_lib, cp_name, msvc10_11_platform[k], type[i], cp_name);
                } else {
                    if (sdl) {
                        if (cp_type == CP_TYPE_CONSOLE) {
                            libs = msvc10_libs_console_sdl;
                        } else {
                            libs = msvc10_libs_gui_sdl;
                        }
                    } else {
                        if (cp_type == CP_TYPE_CONSOLE) {
                            libs = msvc10_libs_console;
                        } else {
                            libs = msvc10_libs_gui[i >> 1];
                        }
                    }
                    fprintf(outfile, msvc10_link, libs, cp_name, (i & 1) ? "" : msvc10_gdi, cp_name, (cp_type == CP_TYPE_GUI) ? "Windows" : "Console", temp_string);
                }
                fprintf(outfile, msvc10_idg_end);
            }
        }
        if (cp_source_names[0] || cp_cc_source_names[0]) {
            fprintf(outfile, msvc10_ig_start);
            if (cp_source_names[0]) {
                for (j = 0; cp_source_names[j]; j++) {
                    fprintf(outfile, msvc10_source, cp_source_names[j]);
                }
            }
            if (cp_sdl_source_names[0] && sdl) {
                for (j = 0; cp_sdl_source_names[j]; j++) {
                    fprintf(outfile, msvc10_source, cp_sdl_source_names[j]);
                }
            }
            if (cp_native_source_names[0] && !sdl) {
                for (j = 0; cp_native_source_names[j]; j++) {
                    fprintf(outfile, msvc10_source, cp_native_source_names[j]);
                }
            }
            if (cp_cc_source_names[0]) {
                for (j = 0; cp_cc_source_names[j]; j++) {
                    fprintf(outfile, msvc10_cc_cbi, cp_cc_source_path, cp_cc_source_names[j]);
                    for (i = 0; i < max_i; i++) {
                        for (k = 0; k < max_k; k++) {
                            if (sdl) {
                                fprintf(outfile, msvc10_cc_cc, type_name[i], msvc10_11_platform[k], msvc_flags_sdl[i & 1], (cp_source_names[0]) ? msvc10_cc_inc_sid_sdl : msvc10_cc_inc_sdl, msvc10_cc_extra[i], cp_name, msvc10_11_platform[k], type[i], cp_name, cp_name, msvc10_11_platform[k], type[i], cp_name, msvc10_11_platform[k], type[i]);
                                fprintf(outfile, msvc10_cc_oc, type_name[i], msvc10_11_platform[k], cp_name, msvc10_11_platform[k], type[i]);
                            } else {
                                fprintf(outfile, msvc10_cc_cc, type_name[i], msvc10_11_platform[k], msvc_flags[i & 1], (cp_source_names[0]) ? msvc10_cc_inc_sid : msvc10_cc_inc, msvc10_cc_extra[i], cp_name, msvc10_11_platform[k], type[i], cp_name, cp_name, msvc10_11_platform[k], type[i], cp_name, msvc10_11_platform[k], type[i]);
                                fprintf(outfile, msvc10_cc_oc, type_name[i], msvc10_11_platform[k], cp_name, msvc10_11_platform[k], type[i]);
                            }
                        }
                    }
                    fprintf(outfile, msvc10_cc_cbi_end);
                }
            }
            fprintf(outfile, msvc10_ig_end);
        }
        if (cp_res_source_name && !sdl) {
            fprintf(outfile, msvc10_ig_start);
            fprintf(outfile, msvc10_res_cbi, cp_res_source_name);
            for (i = 0; i < max_i; i++) {
                for (k = 0; k < max_k; k++) {
                    fprintf(outfile, msvc10_res_cc, type_name[i], msvc10_11_platform[k]);
                    for (j = 0; cp_res_deps[j]; j++) {
                        fprintf(outfile, "..\\%s ", cp_res_deps[j]);
                        if (cp_res_deps[j + 1]) {
                            fprintf(outfile, "+ ");
                        }
                    }
                    fprintf(outfile, msvc10_res_cc_end, cp_res_output_name);
                    fprintf(outfile, msvc10_res_aic, type_name[i], msvc10_11_platform[k]);
                    for (j = 0; cp_res_deps[j]; j++) {
                        fprintf(outfile, "..\\%s;", cp_res_deps[j]);
                    }
                    fprintf(outfile, msvc10_res_aic_end);
                    fprintf(outfile, msvc10_res_oc, type_name[i], msvc10_11_platform[k], cp_res_output_name);
                }
            }
            fprintf(outfile, msvc10_res_manifest, cp_res_output_name);
            for (i = 0; i < max_i; i++) {
                for (k = 0; k < max_k; k++) {
                    fprintf(outfile, msvc10_efbc, type_name[i], msvc10_11_platform[k]);
                }
            }
            fprintf(outfile, msvc10_res_end);
        }
        if (cp_custom_source) {
            fprintf(outfile, msvc10_custom_start, cp_custom_source);
            for (i = 0; i < max_i; i++) {
                for (k = 0; k < max_k; k++) {
                    fprintf(outfile, msvc10_custom_mc, type_name[i], msvc10_11_platform[k], cp_custom_message);
                    fprintf(outfile, msvc10_custom_cc, type_name[i], msvc10_11_platform[k], cp_custom_command);
                    fprintf(outfile, msvc10_custom_aic, type_name[i], msvc10_11_platform[k]);
                    for (j = 0; cp_custom_deps[j]; j++) {
                        fprintf(outfile, "%s;", cp_custom_deps[j]);
                    }
                    fprintf(outfile, msvc10_custom_oc, type_name[i], msvc10_11_platform[k], cp_custom_output);
                }
            }
            fprintf(outfile, msvc10_cc_cbi_end);
            if (!strcmp(cp_name, "base")) {
                fprintf(outfile, msvc10_winid_start);
                for (i = 0; i < max_i; i++) {
                    for (k = 0; k < max_k; k++) {
                        fprintf(outfile, msvc10_winid_middle, type_name[i], msvc10_11_platform[k], type_name[i], msvc10_11_platform[k], msvc10_11_winid_copy[k], type_name[i], msvc10_11_platform[k], msvc10_11_winid_copy[k], type_name[i], msvc10_11_platform[k]);
                    }
                }
                fprintf(outfile, msvc10_cc_cbi_end);
            }
            fprintf(outfile, msvc10_ig_end);
        }
        if (cp_dep_names[0]) {
            fprintf(outfile, msvc10_ig_start);
            for (j = 0; cp_dep_names[j]; j++) {
                if (filelist) {
                    index = pi_insert_name(cp_dep_names[j]);
                }
                fprintf(outfile, msvc10_pri, cp_dep_names[j], index);
            }
            fprintf(outfile, msvc10_ig_end);
        }
        fprintf(outfile, msvc10_project_end);
    }

    if (outfile) {
        fclose(outfile);
    }

    if (filename) {
        free(filename);
    }

    if (cp_libs) {
        free(cp_libs);
        cp_libs = NULL;
    }

    if (read_buffer) {
        free(read_buffer);
        read_buffer = NULL;
        read_buffer_line = 0;
        read_buffer_pos = 0;
        read_buffer_len = 0;
    }

    return retval;
}

/* ---------------------------------------------------------------------- */

static char *msvc9_project_start = "<?xml version=\"1.0\" encoding=\"Windows-1252\"?>\r\n"
                                   "<VisualStudioProject\r\n"
                                   "\tProjectType=\"Visual C++\"\r\n"
                                   "\tVersion=\"9.00\"\r\n"
                                   "\tName=\"%s\"\r\n"
                                   "\tProjectGUID=\"{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}\"\r\n"
                                   "\tTargetFrameworkVersion=\"131072\"\r\n"
                                   "\t>\r\n"
                                   "\t<Platforms>\r\n"
                                   "\t\t<Platform\r\n"
                                   "\t\t\tName=\"Win32\"\r\n"
                                   "\t\t/>\r\n"
                                   "\t\t<Platform\r\n"
                                   "\t\t\tName=\"Itanium\"\r\n"
                                   "\t\t/>\r\n"
                                   "\t\t<Platform\r\n"
                                   "\t\t\tName=\"x64\"\r\n"
                                   "\t\t/>\r\n"
                                   "\t</Platforms>\r\n"
                                   "\t<ToolFiles>\r\n"
                                   "\t</ToolFiles>\r\n"
                                   "\t<Configurations>\r\n";

static char *msvc9_config_part1 = "\t\t<Configuration\r\n"
                                  "\t\t\tName=\"%s|%s\"\r\n";

static char *msvc9_output_dir_app = "\t\t\tOutputDirectory=\".\\..\\..\\..\\..\\data\"\r\n";

static char *msvc9_output_dir_lib = "\t\t\tOutputDirectory=\".\\libs\\%s\\%s%s\"\r\n";

static char *msvc9_config_part2 = "\t\t\tIntermediateDirectory=\".\\libs\\%s\\%s%s\"\r\n"
                                  "\t\t\tConfigurationType=\"%d\"\r\n"
                                  "\t\t\tInheritedPropertySheets=\"$(VCInstallDir)VCProjectDefaults\\UpgradeFromVC71.vsprops\"\r\n"
                                  "\t\t\tUseOfMFC=\"0\"\r\n"
                                  "\t\t\tATLMinimizesCRunTimeLibraryUsage=\"false\"\r\n";

static char *msvc9_charset = "\t\t\tCharacterSet=\"2\"\r\n";

static char *msvc9_config_part3 = "\t\t\t>\r\n"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCPreBuildEventTool\"\r\n"
                                  "\t\t\t/>\r\n";

static char *msvc9_cbt_custom = "\t\t\t<Tool\r\n"
                                "\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                                "\t\t\t\tCommandLine=\"%s&#x0D;&#x0A;\"\r\n"
                                "\t\t\t\tOutputs=\"%s\"\r\n"
                                "\t\t\t/>\r\n";

static char *msvc9_cbt = "\t\t\t<Tool\r\n"
                         "\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                         "\t\t\t/>\r\n";

static char *msvc9_config_part4 = "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCXMLDataGeneratorTool\"\r\n"
                                  "\t\t\t/>\r\n"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCWebServiceProxyGeneratorTool\"\r\n"
                                  "\t\t\t/>\r\n";

static char *msvc9_midl_tool_gui = "\t\t\t<Tool\r\n"
                                   "\t\t\t\tName=\"VCMIDLTool\"\r\n"
                                   "\t\t\t\tPreprocessorDefinitions=\"%s\"\r\n"
                                   "\t\t\t\tMkTypLibCompatible=\"true\"\r\n"
                                   "\t\t\t\tSuppressStartupBanner=\"true\"\r\n"
                                   "\t\t\t\tTargetEnvironment=\"%d\"\r\n"
                                   "\t\t\t\tTypeLibraryName=\".\\..\\..\\..\\..\\data\\%s.tlb\"\r\n"
                                   "\t\t\t/>\r\n";

static char *msvc9_midl_tool_console = "\t\t\t<Tool\r\n"
                                       "\t\t\t\tName=\"VCMIDLTool\"\r\n"
                                       "%s"
                                       "\t\t\t\tTypeLibraryName=\".\\..\\..\\..\\..\\data\\%s.tlb\"\r\n"
                                       "\t\t\t/>\r\n";

static char *msvc9_midl_tool_lib = "\t\t\t<Tool\r\n"
                                   "\t\t\t\tName=\"VCMIDLTool\"\r\n"
                                   "%s"
                                   "\t\t\t/>\r\n";

static char *msvc9_ct_part1 = "\t\t\t<Tool\r\n"
                              "\t\t\t\tName=\"VCCLCompilerTool\"\r\n"
                              "%s\r\n";

static char *msvc9_aid = "\t\t\t\tAdditionalIncludeDirectories=\"..\\msvc,..\\,..\\..\\..\\";

static char *msvc9_aid_sdl = "\t\t\t\tAdditionalIncludeDirectories=\".\\,..\\,..\\..\\..\\";

static char *msvc9_aid_cc = "\t\t\t\tAdditionalIncludeDirectories=\"..\\msvc\"\r\n";

static char *msvc9_aid_cc_sdl = "\t\t\t\tAdditionalIncludeDirectories=\".\\,..\\\"\r\n";

static char *msvc9_aid_sid = "\t\t\t\tAdditionalIncludeDirectories=\"..\\msvc,..\\,..\\..\\..\\,..\\..\\..\\resid\"\r\n";

static char *msvc9_aid_sid_sdl = "\t\t\t\tAdditionalIncludeDirectories=\".\\,..\\,..\\..\\..\\,..\\..\\..\\resid\"\r\n";

static char *msvc9_predef = "\t\t\t\tPreprocessorDefinitions=\"WIN32,_WINDOWS,IDE_COMPILE,DONT_USE_UNISTD_H,";

static char *msvc9_predef_cc = ",PACKAGE=\\&quot;%s\\&quot;,VERSION=\\&quot;0.7\\&quot;,SIZEOF_INT=4";

static char *msvc9_string_pooling = "	\t\t\tStringPooling=\"true\"\r\n";

static char *msvc9_rtl = "\t\t\t\tRuntimeLibrary=\"%d\"\r\n";

static char *msvc9_efll = "\t\t\t\tEnableFunctionLevelLinking=\"true\"\r\n";

static char *msvc9_ct_part2 = "\t\t\t\tUsePrecompiledHeader=\"0\"\r\n"
                              "\t\t\t\tPrecompiledHeaderFile=\".\\libs\\%s\\%s%s\\%s.pch\"\r\n"
                              "\t\t\t\tAssemblerListingLocation=\".\\libs\\%s\\%s%s\\\"\r\n"
                              "\t\t\t\tObjectFile=\".\\libs\\%s\\%s%s\\\"\r\n"
                              "\t\t\t\tProgramDataBaseFileName=\".\\libs\\%s\\%s%s\\\"\r\n"
                              "\t\t\t\tWarningLevel=\"3\"\r\n"
                              "\t\t\t\tSuppressStartupBanner=\"true\"\r\n";

static char *msvc9_dif = "\t\t\t\tDebugInformationFormat=\"1\"\r\n";

static char *msvc9_config_part5 = "\t\t\t\tCompileAs=\"0\"\r\n"
                                  "\t\t\t/>\r\n"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCManagedResourceCompilerTool\"\r\n"
                                  "\t\t\t/>\r\n";

static char *msvc9_rct_gui = "\t\t\t<Tool\r\n"
                             "\t\t\t\tName=\"VCResourceCompilerTool\"\r\n"
                             "\t\t\t\tPreprocessorDefinitions=\"WIN32,_WINDOWS,IDE_COMPILE,DONT_USE_UNISTD_H,%s\"\r\n"
                             "\t\t\t\tCulture=\"1033\"\r\n"
                             "\t\t\t\tAdditionalIncludeDirectories=\"..\\msvc;..\\;..\\..\\..\\\"\r\n"
                             "\t\t\t/>\r\n";

static char *msvc9_rct_console = "\t\t\t<Tool\r\n"
                                 "\t\t\t\tName=\"VCResourceCompilerTool\"\r\n"
                                 "\t\t\t\tPreprocessorDefinitions=\"%s\"\r\n"
                                 "\t\t\t\tCulture=\"1033\"\r\n"
                                 "\t\t\t/>\r\n";

static char *msvc9_rct_lib = "\t\t\t<Tool\r\n"
                             "\t\t\t\tName=\"VCResourceCompilerTool\"\r\n"
                             "\t\t\t\tCulture=\"1033\"\r\n"
                             "\t\t\t/>\r\n";

static char *msvc9_plet = "\t\t\t<Tool\r\n"
                          "\t\t\t\tName=\"VCPreLinkEventTool\"\r\n"
                          "\t\t\t/>\r\n";

static char *msvc9_gdi = "\t\t\t\tGenerateDebugInformation=\"true\"\r\n";

static char *msvc9_linker_tool = "\t\t\t<Tool\r\n"
                                 "\t\t\t\tName=\"VCLinkerTool\"\r\n"
                                 "\t\t\t\tAdditionalDependencies=\"%s\"\r\n"
                                 "\t\t\t\tOutputFile=\".\\..\\..\\..\\..\\data\\%s.exe\"\r\n"
                                 "\t\t\t\tLinkIncremental=\"%d\"\r\n"
                                 "\t\t\t\tSuppressStartupBanner=\"true\"\r\n"
                                 "%s"
                                 "\t\t\t\tProgramDatabaseFile=\".\\..\\..\\..\\..\\data\\%s.pdb\"\r\n"
                                 "\t\t\t\tSubSystem=\"%d\"\r\n"
                                 "\t\t\t\tRandomizedBaseAddress=\"1\"\r\n"
                                 "\t\t\t\tDataExecutionPrevention=\"0\"\r\n";

static char *msvc9_target_machine = "\t\t\t\tTargetMachine=\"%d\"\r\n";

static char *msvc9_lib_tool = "\t\t\t<Tool\r\n"
                              "\t\t\t\tName=\"VCLibrarianTool\"\r\n"
                              "\t\t\t\tOutputFile=\".\\libs\\%s\\%s%s\\%s.lib\"\r\n"
                              "\t\t\t\tSuppressStartupBanner=\"true\"\r\n";

static char *msvc9_manifest_tool = "\t\t\t<Tool\r\n"
                                   "\t\t\t\tName=\"VCManifestTool\"\r\n"
                                   "\t\t\t/>\r\n";

static char *msvc9_app_ver_tool = "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCAppVerifierTool\"\r\n"
                                  "\t\t\t/>\r\n";

static char *msvc9_config_part6 = "\t\t\t/>\r\n"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCALinkTool\"\r\n"
                                  "\t\t\t/>\r\n"
                                  "%s"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCXDCMakeTool\"\r\n"
                                  "\t\t\t/>\r\n"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCBscMakeTool\"\r\n"
                                  "\t\t\t/>\r\n"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCFxCopTool\"\r\n"
                                  "\t\t\t/>\r\n"
                                  "%s"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCPostBuildEventTool\"\r\n"
                                  "\t\t\t/>\r\n"
                                  "\t\t</Configuration>\r\n";

static char *msvc9_configs_files = "\t</Configurations>\r\n"
                                   "\t<References>\r\n"
                                   "\t</References>\r\n"
                                   "\t<Files>\r\n";

static char *msvc9_file = "\t\t<File\r\n"
                          "\t\t\tRelativePath=\"..\\..\\..\\%s\"\r\n"
                          "\t\t\t>\r\n"
                          "\t\t</File>\r\n";

static char *msvc9_res_file = "\t\t<File\r\n"
                              "\t\t\tRelativePath=\"..\\%s\"\r\n"
                              "\t\t\t>\r\n";

static char *msvc9_res_conf = "\t\t\t<FileConfiguration\r\n"
                              "\t\t\t\tName=\"%s|%s\"\r\n"
                              "\t\t\t\t>\r\n"
                              "\t\t\t\t<Tool\r\n"
                              "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                              "\t\t\t\t\tCommandLine=\"copy /b ";

static char *msvc9_res_conf_end = "\t\t\t\t\tOutputs=\"%s\"\r\n"
                                  "\t\t\t\t/>\r\n"
                                  "\t\t\t</FileConfiguration>\r\n";

static char *msvc9_res_file_end = "\t\t</File>\r\n"
                                  "\t\t<File\r\n"
                                  "\t\t\tRelativePath=\".\\%s\"\r\n"
                                  "\t\t\t>\r\n"
                                  "\t\t</File>\r\n"
                                  "\t\t<File\r\n"
                                  "\t\t\tRelativePath=\"..\\vice.manifest\"\r\n"
                                  "\t\t\t>\r\n";

static char *msvc9_res_file_conf = "\t\t\t<FileConfiguration\r\n"
                                   "\t\t\t\tName=\"%s|%s\"\r\n"
                                   "\t\t\t\tExcludedFromBuild=\"true\"\r\n"
                                   "\t\t\t\t>\r\n"
                                   "\t\t\t\t<Tool\r\n"
                                   "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                                   "\t\t\t\t/>\r\n"
                                   "\t\t\t</FileConfiguration>\r\n";


static char *msvc9_custom_file = "\t\t<File\r\n"
                                 "\t\t\tRelativePath=\"%s\"\r\n"
                                 "\t\t\t>\r\n";

static char *msvc9_custom_file_conf = "\t\t\t<FileConfiguration\r\n"
                                      "\t\t\t\tName=\"%s|%s\"\r\n"
                                      "\t\t\t\t>\r\n"
                                      "\t\t\t\t<Tool\r\n"
                                      "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                                      "\t\t\t\t\tDescription=\"%s\"\r\n"
                                      "\t\t\t\t\tCommandLine=\"%s&#x0D;&#x0A;\"\r\n"
                                      "\t\t\t\t\tAdditionalDependencies=\"";

static char *msvc9_custom_file_end = "\t\t\t\t\tOutputs=\"%s\"\r\n"
                                     "\t\t\t\t/>\r\n"
                                     "\t\t\t</FileConfiguration>\r\n";

static char *msvc9_winid_file = "\t\t<File\r\n"
                                "\t\t\tRelativePath=\".\\winid.bat\"\r\n"
                                "\t\t\t>\r\n";

static char *msvc9_winid_conf = "\t\t\t<FileConfiguration\r\n"
                                "\t\t\t\tName=\"%s|%s\"\r\n"
                                "\t\t\t\t>\r\n"
                                "\t\t\t\t<Tool\r\n"
                                "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                                "\t\t\t\t\tDescription=\"Generating winid.bat\"\r\n"
                                "\t\t\t\t\tCommandLine=\"copy %s winid.bat&#x0D;&#x0A;\"\r\n"
                                "\t\t\t\t\tAdditionalDependencies=\".\\%s;\"\r\n"
                                "\t\t\t\t\tOutputs=\".\\winid.bat\"\r\n"
                                "\t\t\t\t/>\r\n"
                                "\t\t\t</FileConfiguration>\r\n";

static char *msvc9_cc_file = "\t\t<File\r\n"
                             "\t\t\tRelativePath=\"..\\..\\..\\%s\\%s.cc\"\r\n"
                             "\t\t\t>\r\n";

static char *msvc9_cc_conf = "\t\t\t<FileConfiguration\r\n"
                             "\t\t\t\tName=\"%s|%s\"\r\n"
                             "\t\t\t\t>\r\n"
                             "\t\t\t\t<Tool\r\n"
                             "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                             "\t\t\t\t\tCommandLine=\"cl /nologo %s %s /D &quot;WIN32&quot; /D &quot;_WINDOWS&quot; /D &quot;IDE_COMPILE&quot; /D &quot;DONT_USE_UNISTD_H&quot; %s /D PACKAGE=\\&quot;%s\\&quot; /D VERSION=\\&quot;0.7\\&quot; /D SIZEOF_INT=4 /Fp&quot;libs\\%s\\%s%s\\%s.pch&quot; /Fo&quot;libs\\%s\\%s%s/&quot; /Fd&quot;libs\\%s\\%s%s/&quot; /FD /TP /c &quot;$(InputPath)&quot;&#x0D;&#x0A;\"\r\n"
                             "\t\t\t\t\tOutputs=\"libs\\%s\\%s%s\\$(InputName).obj\"\r\n"
                             "\t\t\t\t/>\r\n"
                             "\t\t\t</FileConfiguration>\r\n";

static char *msvc9_project_end = "\t</Files>\r\n"
                                 "\t<Globals>\r\n"
                                 "\t</Globals>\r\n"
                                 "</VisualStudioProject>\r\n";

static char *msvc9_main_project_start = "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"%s\", \"%s.vcproj\", \"{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}\"\r\n";

static char *msvc9_main_project_deps_start = "\tProjectSection(ProjectDependencies) = postProject\r\n";

static char *msvc9_main_project_deps = "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X} = {8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}\r\n";

static char *msvc9_main_project_deps_end = "\tEndProjectSection\r\n";

static char *msvc9_main_project_end = "EndProject\r\n";

static char *msvc9_main_global_start = "Global\r\n"
                                       "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\r\n";

static char *msvc9_main_confs = "\t\t%s|%s = %s|%s\r\n";

static char *msvc9_global_mid = "\tEndGlobalSection\r\n"
                                "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\r\n";

static char *msvc9_global_confs1 = "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.%s|%s.ActiveCfg = %s|%s\r\n";

static char *msvc9_global_confs2 = "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.%s|%s.Build.0 = %s|%s\r\n";

static char *msvc9_global_end = "\tEndGlobalSection\r\n"
                                "\tGlobalSection(SolutionProperties) = preSolution\r\n"
                                "\t\tHideSolutionNode = FALSE\r\n"
                                "\tEndGlobalSection\r\n"
                                "EndGlobal\r\n";

static void generate_msvc9_sln(int sdl)
{
    int i, j, k;
    int index;
    int exc = 0;
    int max_i = (sdl) ? 2 : 4;
    char **type_name = (sdl) ? msvc_type_sdl : msvc_type_name;

    for (i = 0; project_info[i].name; i++) {
        fprintf(mainfile, msvc9_main_project_start, project_info[i].name, project_info[i].name, i);
        if (project_info[i].dep[0]) {
            fprintf(mainfile, msvc9_main_project_deps_start);
            for (j = 0; project_info[i].dep[j]; j++) {
                index = pi_get_index_of_name(project_info[i].dep[j]);
                fprintf(mainfile, msvc9_main_project_deps, index, index);
            }
            fprintf(mainfile, msvc9_main_project_deps_end);
        }
        fprintf(mainfile, msvc9_main_project_end);
    }
    fprintf(mainfile, msvc9_main_global_start);
    for (i = 0; i < max_i; i++) {
        for (k = 0; k < 3; k++) {
            fprintf(mainfile, msvc9_main_confs, type_name[i], msvc_platform[k], type_name[i], msvc_platform[k]);
        }
    }
    fprintf(mainfile, msvc9_global_mid);
    for (j = 0; project_info[j].name; j++) {
        for (i = 0; i < max_i; i++) {
            for (k = 0; k < 3; k++) {
                exc = test_win32_exception(project_info[j].name);
                fprintf(mainfile, msvc9_global_confs1, j, type_name[i], msvc_platform[k], type_name[i], (exc) ? "Win32" : msvc_platform[k]);
                fprintf(mainfile, msvc9_global_confs2, j, type_name[i], msvc_platform[k], type_name[i], (exc) ? "Win32" : msvc_platform[k]);
            }
        }
    }
    fprintf(mainfile, msvc9_global_end);
}

static int open_msvc9_main_project(int sdl)
{
    pi_init();

    if (sdl) {
        if (ffmpeg) {
            mainfile = fopen("../../sdl/win32-msvc9-ffmpeg/vice.sln", "wb");
        } else {
            mainfile = fopen("../../sdl/win32-msvc9/vice.sln", "wb");
        }
    } else {
        if (ffmpeg) {
            mainfile = fopen("../vs9-ffmpeg/vice.sln", "wb");
        } else {
            mainfile = fopen("../vs9/vice.sln", "wb");
        }
    }

    if (!mainfile) {
        printf("Cannot open 'vice.sln' for output\n");
        return 1;
    }

    fprintf(mainfile, "Microsoft Visual Studio Solution File, Format Version 10.00\r\n");
    fprintf(mainfile, "# Visual Studio 2008\r\n");

    return 0;
}

static void close_msvc9_main_project(int sdl)
{
    generate_msvc9_sln(sdl);
    pi_exit();
    fclose(mainfile);
}

static int output_msvc9_file(char *fname, int filelist, int sdl)
{
    char *filename;
    int retval = 0;
    FILE *outfile = NULL;
    int i = 0;
    int j, k;
    int max_i = (sdl) ? 2 : 4;
    char *temp_string;
    char *rfname;
    char **type_name = (sdl) ? msvc_type_sdl : msvc_type_name;
    char **type = (sdl) ? msvc_type_sdl : msvc_type;
    char *libs;
    char **msvc_cc_extra = (ffmpeg) ? msvc_cc_extra_ffmpeg : msvc_cc_extra_noffmpeg;
    char **msvc_predefs = (ffmpeg) ? msvc_predefs_ffmpeg : msvc_predefs_noffmpeg;

    if (!strcmp(fname, "arch_native") || !strcmp(fname, "arch_sdl")) {
        rfname = "arch";
    } else {
        rfname = fname;
    }

    if (filelist) {
        i = pi_insert_name(cp_name);
        if (cp_dep_names[0]) {
            pi_insert_deps(cp_dep_names, i);
        }
        if (sdl) {
            if (ffmpeg) {
                filename = malloc(strlen(rfname) + sizeof("../../sdl/win32-msvc9-ffmpeg/.vcproj"));
                sprintf(filename, "../../sdl/win32-msvc9-ffmpeg/%s.vcproj", rfname);
            } else {
                filename = malloc(strlen(rfname) + sizeof("../../sdl/win32-msvc9/.vcproj"));
                sprintf(filename, "../../sdl/win32-msvc9/%s.vcproj", rfname);
            }
        } else {
            if (ffmpeg) {
                filename = malloc(strlen(rfname) + sizeof("../vs9-ffmpeg/.vcproj"));
                sprintf(filename, "../vs9-ffmpeg/%s.vcproj", rfname);
            } else {
                filename = malloc(strlen(rfname) + sizeof("../vs9/.vcproj"));
                sprintf(filename, "../vs9/%s.vcproj", rfname);
            }
        }
    } else {
        filename = malloc(strlen(rfname) + sizeof(".vcproj"));
        sprintf(filename, "%s.vcproj", rfname);
    }

    outfile = fopen(filename, "wb");
    if (!outfile) {
        printf("Cannot open %s for output\n", filename);
        retval = 1;
    }

    if (!retval) {
        fprintf(outfile, msvc9_project_start, cp_name, i);
        for (i = 0; i < max_i; i++) {
            for (k = 0; k < 3; k++) {
                fprintf(outfile, msvc9_config_part1, type_name[i], msvc_platform[k]);
                if (cp_type != CP_TYPE_LIBRARY) {
                    fprintf(outfile, msvc9_output_dir_app);
                } else {
                    fprintf(outfile, msvc9_output_dir_lib, cp_name, msvc_platform[k], type[i]);
                }
                fprintf(outfile, msvc9_config_part2, cp_name, msvc_platform[k], type[i], (cp_type == CP_TYPE_LIBRARY) ? 4 : 1);
                if (cp_type == CP_TYPE_CONSOLE) {
                    fprintf(outfile, msvc9_charset);
                }
                fprintf(outfile, msvc9_config_part3);
                if (cp_post_custom_message) {
                    fprintf(outfile, msvc9_cbt_custom, cp_post_custom_command, cp_post_custom_output);
                } else {
                    fprintf(outfile, msvc9_cbt);
                }
                fprintf(outfile, msvc9_config_part4);
                switch (k) {
                    default:
                    case 0:
                        temp_string = "";
                        break;
                    case 1:
                        temp_string = "\t\t\t\tTargetEnvironment=\"2\"\r\n";
                        break;
                    case 2:
                        temp_string = "\t\t\t\tTargetEnvironment=\"3\"\r\n";
                        break;
                }
                switch (cp_type) {
                    default:
                    case CP_TYPE_GUI:
                        fprintf(outfile, msvc9_midl_tool_gui, msvc_preprodefs[i & 1], k + 1, cp_name);
                        break;
                    case CP_TYPE_CONSOLE:
                        fprintf(outfile, msvc9_midl_tool_console, temp_string, cp_name);
                        break;
                    case CP_TYPE_LIBRARY:
                        fprintf(outfile, msvc9_midl_tool_lib, temp_string);
                        break;
                }
                if (i & 1) {
                    temp_string = "\t\t\t\tInlineFunctionExpansion=\"1\"";
                } else {
                    temp_string = "\t\t\t\tOptimization=\"0\"";
                }
                fprintf(outfile, msvc9_ct_part1, temp_string);
                if (cp_cc_source_names[0]) {
                    if (cp_source_names[0]) {
                        if (sdl) {
                            fprintf(outfile, msvc9_aid_sid_sdl);
                        } else {
                            fprintf(outfile, msvc9_aid_sid);
                        }
                    } else {
                        if (sdl) {
                            fprintf(outfile, msvc9_aid_cc_sdl);
                        } else {
                            fprintf(outfile, msvc9_aid_cc);
                        }
                    }
                } else {
                    if (sdl) {
                        fprintf(outfile, msvc9_aid_sdl);
                    } else {
                        fprintf(outfile, msvc9_aid);
                    }
                    if (cp_include_dirs[0]) {
                        for (j = 0; cp_include_dirs[j]; j++) {
                            fprintf(outfile, ",..\\..\\..\\%s", cp_include_dirs[j]);
                        }
                    }
                    fprintf(outfile, "\"\r\n");
                }
                fprintf(outfile, msvc9_predef);
                fprintf(outfile, msvc_predefs[i]);
                if (cp_cc_source_names[0]) {
                    fprintf(outfile, msvc9_predef_cc, cp_name);
                }
                fprintf(outfile, "\"\r\n");
                if (i & 1) {
                    fprintf(outfile, msvc9_string_pooling);
                }
                fprintf(outfile, msvc9_rtl, (!(i & 1)) + (sdl * 2));
                if (i & 1) {
                    fprintf(outfile, msvc9_efll);
                }
                fprintf(outfile, msvc9_ct_part2, cp_name, msvc_platform[k], type[i], cp_name, cp_name, msvc_platform[k], type[i], cp_name, msvc_platform[k], type[i], cp_name, msvc_platform[k], type[i]);
                if (!(i & 1)) {
                    fprintf(outfile, msvc9_dif);
                }
                fprintf(outfile, msvc9_config_part5);
                switch (cp_type) {
                    default:
                    case CP_TYPE_GUI:
                        fprintf(outfile, msvc9_rct_gui, msvc_predefs[i]);
                        break;
                    case CP_TYPE_CONSOLE:
                        fprintf(outfile, msvc9_rct_console, msvc_predefs[i]);
                        break;
                    case CP_TYPE_LIBRARY:
                        fprintf(outfile, msvc9_rct_lib);
                        break;
                }
                fprintf(outfile, msvc9_plet);
                if (cp_type != CP_TYPE_LIBRARY) {
                    if (sdl) {
                        if (cp_type == CP_TYPE_CONSOLE) {
                            libs = msvc_libs_console_sdl;
                        } else {
                            libs = msvc_libs_gui_sdl;
                        }
                    } else {
                        if (cp_type == CP_TYPE_CONSOLE) {
                            libs = msvc_libs_console;
                        } else {
                            libs = msvc_libs_gui[i >> 1];
                        }
                    }
                    fprintf(outfile, msvc9_linker_tool, libs, cp_name, (!(i & 1)) + 1, (i & 1) ? "" : msvc9_gdi, cp_name, (cp_type == CP_TYPE_GUI) ? 2 : 1);
                    if (k) {
                        fprintf(outfile, msvc9_target_machine, (k == 1) ? 5 : 17);
                    }
                } else {
                    fprintf(outfile, msvc9_lib_tool, cp_name, msvc_platform[k], type[i], cp_name);
                }
                fprintf(outfile, msvc9_config_part6, (cp_type == CP_TYPE_LIBRARY) ? "" : msvc9_manifest_tool, (cp_type == CP_TYPE_LIBRARY) ? "" : msvc9_app_ver_tool);
            }
        }
        fprintf(outfile, msvc9_configs_files);
        if (cp_source_names[0]) {
            for (j = 0; cp_source_names[j]; j++) {
                fprintf(outfile, msvc9_file, cp_source_names[j]);
            }
        }
        if (cp_sdl_source_names[0] && sdl) {
            for (j = 0; cp_sdl_source_names[j]; j++) {
                fprintf(outfile, msvc9_file, cp_sdl_source_names[j]);
            }
        }
        if (cp_native_source_names[0] && !sdl) {
            for (j = 0; cp_native_source_names[j]; j++) {
                fprintf(outfile, msvc9_file, cp_native_source_names[j]);
            }
        }
        if (cp_res_source_name && !sdl) {
            fprintf(outfile, msvc9_res_file, cp_res_source_name);
            for (i = 0; i < max_i; i++) {
                for (k = 0; k < 3; k++) {
                    fprintf(outfile, msvc9_res_conf, type_name[i], msvc_platform[k]);
                    for (j = 0; cp_res_deps[j]; j++) {
                        fprintf(outfile, "..\\%s ", cp_res_deps[j]);
                        if (cp_res_deps[j + 1]) {
                            fprintf(outfile, "+ ");
                        }
                    }
                    fprintf(outfile, "%s /b&#x0D;&#x0A;\"\r\n", cp_res_output_name);
                    fprintf(outfile, "\t\t\t\t\tAdditionalDependencies=\"..\\..\\..\\debug.h;");
                    for (j = 0; cp_res_deps[j]; j++) {
                        fprintf(outfile, "..\\%s;", cp_res_deps[j]);
                    }
                    fprintf(outfile, "\"\r\n");
                    fprintf(outfile, msvc9_res_conf_end, cp_res_output_name);
                }
            }
            fprintf(outfile, msvc9_res_file_end, cp_res_output_name);
            for (i = 0; i < max_i; i++) {
                for (k = 0; k < 3; k++) {
                    fprintf(outfile, msvc9_res_file_conf, type_name[i], msvc_platform[k]);
                }
            }
            fprintf(outfile, msvc_file_end);
        }
        if (cp_custom_message) {
            fprintf(outfile, msvc9_custom_file, cp_custom_source);
            for (i = 0; i < max_i; i++) {
                for (k = 0; k < 3; k++) {
                    fprintf(outfile, msvc9_custom_file_conf, type_name[i], msvc_platform[k], cp_custom_message, cp_custom_command);
                    for (j = 0; cp_custom_deps[j]; j++) {
                        fprintf(outfile, "%s;", cp_custom_deps[j]);
                    }
                    fprintf(outfile, "\"\r\n");
                    fprintf(outfile, msvc9_custom_file_end, cp_custom_output);
                }
            }
            fprintf(outfile, msvc_file_end);
        }
        if (!strcmp(cp_name, "base")) {
            fprintf(outfile, msvc9_winid_file);
            for (i = 0; i < max_i; i++) {
                for (k = 0; k < 3; k++) {
                    fprintf(outfile, msvc9_winid_conf, type_name[i], msvc_platform[k], msvc_winid_copy[k], msvc_winid_copy[k]);
                }
            }
            fprintf(outfile, msvc_file_end);
        }
        if (cp_cc_source_path) {
            for (j = 0; cp_cc_source_names[j]; j++) {
                fprintf(outfile, msvc9_cc_file, cp_cc_source_path, cp_cc_source_names[j]);
                for (i = 0; i < max_i; i++) {
                    for (k = 0; k < 3; k++) {
                        if (sdl) {
                            fprintf(outfile, msvc9_cc_conf, type_name[i], msvc_platform[k], msvc_flags_sdl[i & 1], (cp_source_names[0]) ? msvc_cc_inc_sid_sdl : msvc_cc_inc_sdl, msvc_cc_extra[i], cp_name, cp_name, msvc_platform[k], type[i], cp_name, cp_name, msvc_platform[k], type[i], cp_name, msvc_platform[k], type[i], cp_name, msvc_platform[k], type[i]);
                        } else {
                            fprintf(outfile, msvc9_cc_conf, type_name[i], msvc_platform[k], msvc_flags[i & 1], (cp_source_names[0]) ? msvc_cc_inc_sid : msvc_cc_inc, msvc_cc_extra[i], cp_name, cp_name, msvc_platform[k], type[i], cp_name, cp_name, msvc_platform[k], type[i], cp_name, msvc_platform[k], type[i], cp_name, msvc_platform[k], type[i]);
                        }
                    }
                }
                fprintf(outfile, msvc_file_end);
            }
        }
        fprintf(outfile, msvc9_project_end);
    }

    if (outfile) {
        fclose(outfile);
    }

    if (filename) {
        free(filename);
    }

    if (cp_libs) {
        free(cp_libs);
        cp_libs = NULL;
    }

    if (read_buffer) {
        free(read_buffer);
        read_buffer = NULL;
        read_buffer_line = 0;
        read_buffer_pos = 0;
        read_buffer_len = 0;
    }

    return retval;
}

/* ---------------------------------------------------------------------- */

static char *msvc8_main_platform[3] = {
    "Itanium",
    "Win32",
    "x64"
};

static char *msvc8_main_type[4] = {
    "Debug",
    "DX Debug",
    "DX Release",
    "Release"
};

static char *msvc8_main_type_sdl[2] = {
    "Debug",
    "Release"
};

static char *msvc8_project_start = "<?xml version=\"1.0\" encoding=\"Windows-1252\"?>\r\n"
                                   "<VisualStudioProject\r\n"
                                   "\tProjectType=\"Visual C++\"\r\n"
                                   "\tVersion=\"8.00\"\r\n"
                                   "\tName=\"%s\"\r\n"
                                   "\tProjectGUID=\"{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}\"\r\n"
                                   "\t>\r\n"
                                   "\t<Platforms>\r\n"
                                   "\t\t<Platform\r\n"
                                   "\t\t\tName=\"Win32\"\r\n"
                                   "\t\t/>\r\n"
                                   "\t\t<Platform\r\n"
                                   "\t\t\tName=\"Itanium\"\r\n"
                                   "\t\t/>\r\n"
                                   "\t\t<Platform\r\n"
                                   "\t\t\tName=\"x64\"\r\n"
                                   "\t\t/>\r\n"
                                   "\t</Platforms>\r\n"
                                   "\t<ToolFiles>\r\n"
                                   "\t</ToolFiles>\r\n"
                                   "\t<Configurations>\r\n";

static char *msvc8_config_start = "\t\t<Configuration\r\n"
                                  "\t\t\tName=\"%s|%s\"\r\n";

static char *msvc8_outputdir_app = "\t\t\tOutputDirectory=\".\\..\\..\\..\\..\\data\"\r\n";

static char *msvc8_outputdir_lib = "\t\t\tOutputDirectory=\".\\libs\\%s\\%s%s\"\r\n";

static char *msvc8_config_part2 = "\t\t\tIntermediateDirectory=\".\\libs\\%s\\%s%s\"\r\n"
                                  "\t\t\tConfigurationType=\"%d\"\r\n"
                                  "\t\t\tInheritedPropertySheets=\"$(VCInstallDir)VCProjectDefaults\\UpgradeFromVC71.vsprops\"\r\n"
                                  "\t\t\tUseOfMFC=\"0\"\r\n"
                                  "\t\t\tATLMinimizesCRunTimeLibraryUsage=\"false\"\r\n";

static char *msvc8_charset = "\t\t\tCharacterSet=\"2\"\r\n";

static char *msvc8_config_part3 = "\t\t\t>\r\n"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCPreBuildEventTool\"\r\n"
                                  "\t\t\t/>\r\n";

static char *msvc8_cbt_helper = "\t\t\t<Tool\r\n"
                                "\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                                "\t\t\t\tCommandLine=\"%s&#x0D;&#x0A;\"\r\n"
                                "\t\t\t\tOutputs=\"%s\"\r\n"
                                "\t\t\t/>\r\n";

static char *msvc8_cbt = "\t\t\t<Tool\r\n"
                         "\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                         "\t\t\t/>\r\n";

static char *msvc8_config_part4 = "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCXMLDataGeneratorTool\"\r\n"
                                  "\t\t\t/>\r\n"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCWebServiceProxyGeneratorTool\"\r\n"
                                  "\t\t\t/>\r\n";

static char *msvc8_midl_tool_gui = "\t\t\t<Tool\r\n"
                                   "\t\t\t\tName=\"VCMIDLTool\"\r\n"
                                   "\t\t\t\tPreprocessorDefinitions=\"%s\"\r\n"
                                   "\t\t\t\tMkTypLibCompatible=\"true\"\r\n"
                                   "\t\t\t\tSuppressStartupBanner=\"true\"\r\n"
                                   "\t\t\t\tTargetEnvironment=\"%d\"\r\n"
                                   "\t\t\t\tTypeLibraryName=\".\\..\\..\\..\\..\\data\\%s.tlb\"\r\n"
                                   "\t\t\t/>\r\n";

static char *msvc8_midl_tool_console_x86 = "\t\t\t<Tool\r\n"
                                           "\t\t\t\tName=\"VCMIDLTool\"\r\n"
                                           "\t\t\t\tTypeLibraryName=\".\\..\\..\\..\\..\\data\\%s.tlb\"\r\n"
                                           "\t\t\t/>\r\n";

static char *msvc8_midl_tool_console = "\t\t\t<Tool\r\n"
                                       "\t\t\t\tName=\"VCMIDLTool\"\r\n"
                                       "\t\t\t\tTargetEnvironment=\"%d\"\r\n"
                                       "\t\t\t\tTypeLibraryName=\".\\..\\..\\..\\..\\data\\%s.tlb\"\r\n"
                                       "\t\t\t/>\r\n";

static char *msvc8_midl_tool_lib_x86 = "\t\t\t<Tool\r\n"
                                       "\t\t\t\tName=\"VCMIDLTool\"\r\n"
                                       "\t\t\t/>\r\n";

static char *msvc8_midl_tool_lib = "\t\t\t<Tool\r\n"
                                   "\t\t\t\tName=\"VCMIDLTool\"\r\n"
                                   "\t\t\t\tTargetEnvironment=\"%d\"\r\n"
                                   "\t\t\t/>\r\n";

static char *msvc8_compiler_tool_start = "\t\t\t<Tool\r\n"
                                         "\t\t\t\tName=\"VCCLCompilerTool\"\r\n";

static char *msvc8_opt0 = "\t\t\t\tOptimization=\"0\"\r\n";

static char *msvc8_ife = "\t\t\t\tInlineFunctionExpansion=\"1\"\r\n";

static char *msvc8_includes = "\t\t\t\tAdditionalIncludeDirectories=\"..\\msvc,..\\,..\\..\\..\\";

static char *msvc8_includes_sdl = "\t\t\t\tAdditionalIncludeDirectories=\".\\,..\\,..\\..\\..\\";

static char *msvc8_includes_cc = "\t\t\t\tAdditionalIncludeDirectories=\"..\\msvc\"\r\n";

static char *msvc8_includes_cc_sdl = "\t\t\t\tAdditionalIncludeDirectories=\".\\,..\\\"\r\n";

static char *msvc8_includes_cc_mixed = "\t\t\t\tAdditionalIncludeDirectories=\"..\\msvc,..\\,..\\..\\..\\,..\\..\\..\\resid\"\r\n";

static char *msvc8_includes_cc_mixed_sdl = "\t\t\t\tAdditionalIncludeDirectories=\".\\,..\\,..\\..\\..\\,..\\..\\..\\resid\"\r\n";

static char *msvc8_defs = "\t\t\t\tPreprocessorDefinitions=\"WIN32,_WINDOWS,IDE_COMPILE,DONT_USE_UNISTD_H,%s\"\r\n";

static char *msvc8_defs_cc = "\t\t\t\tPreprocessorDefinitions=\"WIN32,_WINDOWS,IDE_COMPILE,DONT_USE_UNISTD_H,%s,PACKAGE=\\&quot;%s\\&quot;,VERSION=\\&quot;0.7\\&quot;,SIZEOF_INT=4\"\r\n";

static char *msvc8_string_pooling = "\t\t\t\tStringPooling=\"true\"\r\n";

static char *msvc8_ct_part2 = "\t\t\t\tRuntimeLibrary=\"%d\"\r\n";

static char *msvc8_efll = "\t\t\t\tEnableFunctionLevelLinking=\"true\"\r\n";

static char *msvc8_ct_part3 = "\t\t\t\tUsePrecompiledHeader=\"0\"\r\n"
                              "\t\t\t\tPrecompiledHeaderFile=\".\\libs\\%s\\%s%s\\%s.pch\"\r\n"
                              "\t\t\t\tAssemblerListingLocation=\".\\libs\\%s\\%s%s\\\"\r\n"
                              "\t\t\t\tObjectFile=\".\\libs\\%s\\%s%s\\\"\r\n"
                              "\t\t\t\tProgramDataBaseFileName=\".\\libs\\%s\\%s%s\\\"\r\n"
                              "\t\t\t\tWarningLevel=\"3\"\r\n"
                              "\t\t\t\tSuppressStartupBanner=\"true\"\r\n";

static char *msvc8_dif = "\t\t\t\tDebugInformationFormat=\"1\"\r\n";

static char *msvc8_config_part5 = "\t\t\t\tCompileAs=\"0\"\r\n"
                                  "\t\t\t/>\r\n"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCManagedResourceCompilerTool\"\r\n"
                                  "\t\t\t/>\r\n";

static char *msvc8_rct_gui = "\t\t\t<Tool\r\n"
                             "\t\t\t\tName=\"VCResourceCompilerTool\"\r\n"
                             "\t\t\t\tPreprocessorDefinitions=\"WIN32,_WINDOWS,IDE_COMPILE,DONT_USE_UNISTD_H,%s\"\r\n"
                             "\t\t\t\tCulture=\"1033\"\r\n"
                             "\t\t\t\tAdditionalIncludeDirectories=\"..\\msvc;..\\;..\\..\\..\\\"\r\n"
                             "\t\t\t/>\r\n";

static char *msvc8_rct_console = "\t\t\t<Tool\r\n"
                                 "\t\t\t\tName=\"VCResourceCompilerTool\"\r\n"
                                 "\t\t\t\tPreprocessorDefinitions=\"%s\"\r\n"
                                 "\t\t\t\tCulture=\"1033\"\r\n"
                                 "\t\t\t/>\r\n";

static char *msvc8_rct_lib = "\t\t\t<Tool\r\n"
                             "\t\t\t\tName=\"VCResourceCompilerTool\"\r\n"
                             "\t\t\t\tCulture=\"1033\"\r\n"
                             "\t\t\t/>\r\n";

static char *msvc8_config_part6 = "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCPreLinkEventTool\"\r\n"
                                  "\t\t\t/>\r\n";

static char *msvc8_lib_tool = "\t\t\t<Tool\r\n"
                              "\t\t\t\tName=\"VCLibrarianTool\"\r\n"
                              "\t\t\t\tOutputFile=\".\\libs\\%s\\%s%s\\%s.lib\"\r\n"
                              "\t\t\t\tSuppressStartupBanner=\"true\"\r\n"
                              "\t\t\t/>\r\n";

static char *msvc8_linker_tool_part1 = "\t\t\t<Tool\r\n"
                                       "\t\t\t\tName=\"VCLinkerTool\"\r\n"
                                       "\t\t\t\tAdditionalDependencies=\"%s\"\r\n"
                                       "\t\t\t\tOutputFile=\".\\..\\..\\..\\..\\data\\%s.exe\"\r\n"
                                       "\t\t\t\tLinkIncremental=\"%d\"\r\n"
                                       "\t\t\t\tSuppressStartupBanner=\"true\"\r\n";

static char *msvc8_idln = "\t\t\t\tIgnoreDefaultLibraryNames=\"msvcrt.lib\"\r\n";


static char *msvc8_linker_tool_debug = "\t\t\t\tGenerateDebugInformation=\"true\"\r\n";

static char *msvc8_linker_tool_part2 = "\t\t\t\tProgramDatabaseFile=\".\\..\\..\\..\\..\\data\\%s.pdb\"\r\n"
                                       "\t\t\t\tSubSystem=\"%d\"\r\n";

static char *msvc8_link_tool = "\t\t\t<Tool\r\n"
                               "\t\t\t\tName=\"VCALinkTool\"\r\n"
                               "\t\t\t/>\r\n";

static char *msvc8_manifest_tool = "\t\t\t<Tool\r\n"
                                   "\t\t\t\tName=\"VCManifestTool\"\r\n"
                                   "\t\t\t/>\r\n";

static char *msvc8_config_part7 = "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCXDCMakeTool\"\r\n"
                                  "\t\t\t/>\r\n"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCBscMakeTool\"\r\n"
                                  "\t\t\t/>\r\n"
                                  "\t\t\t<Tool\r\n"
                                  "\t\t\t\tName=\"VCFxCopTool\"\r\n"
                                  "\t\t\t/>\r\n";

static char *msvc8_app_tools = "\t\t\t<Tool\r\n"
                               "\t\t\t\tName=\"VCAppVerifierTool\"\r\n"
                               "\t\t\t/>\r\n"
                               "\t\t\t<Tool\r\n"
                               "\t\t\t\tName=\"VCWebDeploymentTool\"\r\n"
                               "\t\t\t/>\r\n";

static char *msvc8_conf_end = "\t\t\t<Tool\r\n"
                              "\t\t\t\tName=\"VCPostBuildEventTool\"\r\n"
                              "\t\t\t/>\r\n"
                              "\t\t</Configuration>\r\n";

static char *msvc8_confs_files = "\t</Configurations>\r\n"
                                 "\t<References>\r\n"
                                 "\t</References>\r\n"
                                 "\t<Files>\r\n";

static char *msvc8_file = "\t\t<File\r\n"
                          "\t\t\tRelativePath=\"..\\..\\..\\%s\"\r\n"
                          "\t\t\t>\r\n"
                          "\t\t</File>\r\n";

static char *msvc8_cpu_file = "\t\t<File\r\n"
                              "\t\t\tRelativePath=\"..\\..\\..\\%s\"\r\n"
                              "\t\t\t>\r\n";

static char *msvc8_cpu_fileconf = "\t\t\t<FileConfiguration\r\n"
                                  "\t\t\t\tName=\"%s|%s\"\r\n"
                                  "\t\t\t\t>\r\n"
                                  "\t\t\t\t<Tool\r\n"
                                  "\t\t\t\t\tName=\"VCCLCompilerTool\"\r\n"
                                  "\t\t\t\t\tOptimization=\"4\"\r\n"
                                  "\t\t\t\t\tEnableIntrinsicFunctions=\"true\"\r\n"
                                  "\t\t\t\t\tFavorSizeOrSpeed=\"0\"\r\n"
                                  "\t\t\t\t\tOmitFramePointers=\"true\"\r\n"
                                  "\t\t\t\t/>\r\n"
                                  "\t\t\t</FileConfiguration>\r\n";

static char *msvc8_custom_file = "\t\t<File\r\n"
                                 "\t\t\tRelativePath=\"%s\"\r\n"
                                 "\t\t\t>\r\n";

static char *msvc8_custom_conf = "\t\t\t<FileConfiguration\r\n"
                                 "\t\t\t\tName=\"%s|%s\"\r\n"
                                 "\t\t\t\t>\r\n"
                                 "\t\t\t\t<Tool\r\n"
                                 "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                                 "\t\t\t\t\tDescription=\"%s\"\r\n"
                                 "\t\t\t\t\tCommandLine=\"%s&#x0D;&#x0A;\"\r\n"
                                 "\t\t\t\t\tAdditionalDependencies=\"";

static char *msvc8_custom_conf2 = "\"\r\n"
                                  "\t\t\t\t\tOutputs=\"%s\"\r\n"
                                  "\t\t\t\t/>\r\n"
                                  "\t\t\t</FileConfiguration>\r\n";

static char *msvc8_winid_start = "\t\t<File\r\n"
                                 "\t\t\tRelativePath=\".\\winid.bat\"\r\n"
                                 "\t\t\t>\r\n";

static char *msvc8_winid_conf = "\t\t\t<FileConfiguration\r\n"
                                "\t\t\t\tName=\"%s|%s\"\r\n"
                                "\t\t\t\t>\r\n"
                                "\t\t\t\t<Tool\r\n"
                                "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                                "\t\t\t\t\tDescription=\"Generating winid.bat\"\r\n"
                                "\t\t\t\t\tCommandLine=\"copy %s winid.bat&#x0D;&#x0A;\"\r\n"
                                "\t\t\t\t\tAdditionalDependencies=\".\\%s;\"\r\n"
                                "\t\t\t\t\tOutputs=\".\\winid.bat\"\r\n"
                                "\t\t\t\t/>\r\n"
                                "\t\t\t</FileConfiguration>\r\n";

static char *msvc8_cc_file = "\t\t<File\r\n"
                             "\t\t\tRelativePath=\"..\\..\\..\\%s\\%s.cc\"\r\n"
                             "\t\t\t>\r\n";

static char *msvc8_cc_conf = "\t\t\t<FileConfiguration\r\n"
                             "\t\t\t\tName=\"%s|%s\"\r\n"
                             "\t\t\t\t>\r\n"
                             "\t\t\t\t<Tool\r\n"
                             "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                             "\t\t\t\t\tCommandLine=\"cl /nologo %s %s /D &quot;WIN32&quot; /D &quot;_WINDOWS&quot; /D &quot;IDE_COMPILE&quot; /D &quot;DONT_USE_UNISTD_H&quot; %s /D PACKAGE=\\&quot;%s\\&quot; /D VERSION=\\&quot;0.7\\&quot; /D SIZEOF_INT=4 /Fp&quot;libs\\%s\\%s%s\\%s.pch&quot; /Fo&quot;libs\\%s\\%s%s/&quot; /Fd&quot;libs\\%s\\%s%s/&quot; /FD /TP /c &quot;$(InputPath)&quot;&#x0D;&#x0A;\"\r\n"
                             "\t\t\t\t\tOutputs=\"libs\\%s\\%s%s\\$(InputName).obj\"\r\n"
                             "\t\t\t\t/>\r\n"
                             "\t\t\t</FileConfiguration>\r\n";

static char *msvc8_res_file = "\t\t<File\r\n"
                              "\t\t\tRelativePath=\"..\\%s\"\r\n"
                              "\t\t\t>\r\n";

static char *msvc8_res_conf = "\t\t\t<FileConfiguration\r\n"
                              "\t\t\t\tName=\"%s|%s\"\r\n"
                              "\t\t\t\t>\r\n"
                              "\t\t\t\t<Tool\r\n"
                              "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                              "\t\t\t\t\tCommandLine=\"copy /b ";

static char *msvc8_res_conf2 = "%s /b&#x0D;&#x0A;\"\r\n"
                               "\t\t\t\t\tAdditionalDependencies=\"..\\..\\..\\debug.h;";

static char *msvc8_res_conf3 = "\"\r\n"
                               "\t\t\t\t\tOutputs=\"%s\"\r\n"
                               "\t\t\t\t/>\r\n"
                               "\t\t\t</FileConfiguration>\r\n";

static char *msvc8_res_end = "\t\t</File>\r\n"
                             "\t\t<File\r\n"
                             "\t\t\tRelativePath=\".\\%s\"\r\n"
                             "\t\t\t>\r\n"
                             "\t\t</File>\r\n"
                             "\t\t<File\r\n"
                             "\t\t\tRelativePath=\"..\\vice.manifest\"\r\n"
                             "\t\t\t>\r\n";

static char *msvc8_manifest_conf = "\t\t\t<FileConfiguration\r\n"
                                   "\t\t\t\tName=\"%s|%s\"\r\n"
                                   "\t\t\t\tExcludedFromBuild=\"true\"\r\n"
                                   "\t\t\t\t>\r\n"
                                   "\t\t\t\t<Tool\r\n"
                                   "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                                   "\t\t\t\t/>\r\n"
                                   "\t\t\t</FileConfiguration>\r\n";

static char *msvc8_end_project = "\t</Files>\r\n"
                                 "\t<Globals>\r\n"
                                 "\t</Globals>\r\n"
                                 "</VisualStudioProject>\r\n";

static char *msvc8_main_project_deps = "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"%s\", \"%s.vcproj\", \"{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}\"\r\n";

static char *msvc8_main_dep_cond = "\tProjectSection(ProjectDependencies) = postProject\r\n";

static char *msvc8_main_dep_end_cond = "\tEndProjectSection\r\n";

static char *msvc8_main_dep_end = "EndProject\r\n";

static char *msvc8_main_global_start = "Global\r\n"
                                       "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\r\n";

static char *msvc8_main_post = "\tEndGlobalSection\r\n"
                               "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\r\n";

static char *msvc8_main_end = "\tEndGlobalSection\r\n"
                              "\tGlobalSection(SolutionProperties) = preSolution\r\n"
                              "\t\tHideSolutionNode = FALSE\r\n"
                              "\tEndGlobalSection\r\n"
                              "EndGlobal\r\n";

static void generate_msvc8_sln(int sdl)
{
    int i, j, k;
    int index;
    int exc = 0;
    int max_i = (sdl) ? 2 : 4;
    char **main_type = (sdl) ? msvc8_main_type_sdl : msvc8_main_type;

    for (i = 0; project_info[i].name; i++) {
        fprintf(mainfile, msvc8_main_project_deps, project_info[i].name, project_info[i].name, i);
        if (project_info[i].dep[0]) {
            fprintf(mainfile, msvc8_main_dep_cond);
            for (j = 0; project_info[i].dep[j]; j++) {
                index = pi_get_index_of_name(project_info[i].dep[j]);
                fprintf(mainfile, "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X} = {8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}\r\n", index, index);
            }
            fprintf(mainfile, msvc8_main_dep_end_cond);
        }
        fprintf(mainfile, msvc8_main_dep_end);
    }
    fprintf(mainfile, msvc8_main_global_start);
    for (i = 0; i < max_i; i++) {
        for (j = 0; j < 3; j++) {
            fprintf(mainfile, "\t\t%s|%s = %s|%s\r\n", main_type[i], msvc8_main_platform[j], main_type[i], msvc8_main_platform[j]);
        }
    }
    fprintf(mainfile, msvc8_main_post);
    for (k = 0; project_info[k].name; k++) {
        exc = test_win32_exception(project_info[k].name);
        for (i = 0; i < max_i; i++) {
            for (j = 0; j < 3; j++) {
                if (exc) {
                    fprintf(mainfile, "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.%s|%s.ActiveCfg = %s|Win32\r\n", k, main_type[i], msvc8_main_platform[j], main_type[i]);
                    fprintf(mainfile, "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.%s|%s.Build.0 = %s|Win32\r\n", k, main_type[i], msvc8_main_platform[j], main_type[i]);
                } else {
                    fprintf(mainfile, "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.%s|%s.ActiveCfg = %s|%s\r\n", k, main_type[i], msvc8_main_platform[j], main_type[i], msvc8_main_platform[j]);
                    fprintf(mainfile, "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.%s|%s.Build.0 = %s|%s\r\n", k, main_type[i], msvc8_main_platform[j], main_type[i], msvc8_main_platform[j]);
                }
            }
        }
    }
    fprintf(mainfile, msvc8_main_end);
}

static int open_msvc8_main_project(int sdl)
{
    pi_init();

    if (sdl) {
        if (ffmpeg) {
            mainfile = fopen("../../sdl/win32-msvc8-ffmpeg/vice.sln", "wb");
        } else {
            mainfile = fopen("../../sdl/win32-msvc8/vice.sln", "wb");
        }
    } else {
        if (ffmpeg) {
            mainfile = fopen("../vs8-ffmpeg/vice.sln", "wb");
        } else {
            mainfile = fopen("../vs8/vice.sln", "wb");
        }
    }

    if (!mainfile) {
        printf("Cannot open 'vice.sln' for output\n");
        return 1;
    }

    fprintf(mainfile, "Microsoft Visual Studio Solution File, Format Version 9.00\r\n");
    fprintf(mainfile, "# Visual Studio 2005\r\n");

    return 0;
}

static void close_msvc8_main_project(int sdl)
{
    generate_msvc8_sln(sdl);
    pi_exit();
    fclose(mainfile);
}

static int output_msvc8_file(char *fname, int filelist, int sdl)
{
    char *filename;
    int retval = 0;
    FILE *outfile = NULL;
    int i = 0;
    int j, k;
    int max_i = (sdl) ? 2 : 4;
    char *rfname;
    char **type_name = (sdl) ? msvc_type_sdl : msvc_type_name;
    char **type = (sdl) ? msvc_type_sdl : msvc_type;
    char *libs;
    char **msvc_cc_extra = (ffmpeg) ? msvc_cc_extra_ffmpeg : msvc_cc_extra_noffmpeg;
    char **msvc_predefs = (ffmpeg) ? msvc_predefs_ffmpeg : msvc_predefs_noffmpeg;

    if (!strcmp(fname, "arch_native") || !strcmp(fname, "arch_sdl")) {
        rfname = "arch";
    } else {
        rfname = fname;
    }

    if (filelist) {
        i = pi_insert_name(cp_name);
        if (cp_dep_names[0]) {
            pi_insert_deps(cp_dep_names, i);
        }
        if (sdl) {
            if (ffmpeg) {
                filename = malloc(strlen(rfname) + sizeof("../../sdl/win32-msvc8-ffmpeg/.vcproj"));
                sprintf(filename, "../../sdl/win32-msvc8-ffmpeg/%s.vcproj", rfname);
            } else {
                filename = malloc(strlen(rfname) + sizeof("../../sdl/win32-msvc8/.vcproj"));
                sprintf(filename, "../../sdl/win32-msvc8/%s.vcproj", rfname);
            }
        } else {
            if (ffmpeg) {
                filename = malloc(strlen(rfname) + sizeof("../vs8-ffmpeg/.vcproj"));
                sprintf(filename, "../vs8-ffmpeg/%s.vcproj", rfname);
            } else {
                filename = malloc(strlen(rfname) + sizeof("../vs8/.vcproj"));
                sprintf(filename, "../vs8/%s.vcproj", rfname);
            }
        }
    } else {
        filename = malloc(strlen(rfname) + sizeof(".vcproj"));
        sprintf(filename, "%s.vcproj", rfname);
    }

    outfile = fopen(filename, "wb");
    if (!outfile) {
        printf("Cannot open %s for output\n", filename);
        retval = 1;
    }

    if (!retval) {
        fprintf(outfile, msvc8_project_start, cp_name, i);
        for (k = 0; k < 3; k++) {
            for (i = 0; i < max_i; i++) {
                fprintf(outfile, msvc8_config_start, type_name[i], msvc_platform[k]);
                if (cp_type != CP_TYPE_LIBRARY) {
                    fprintf(outfile, msvc8_outputdir_app);
                } else {
                    fprintf(outfile, msvc8_outputdir_lib, cp_name, msvc_platform[k], type[i]);
                }
                fprintf(outfile, msvc8_config_part2, cp_name, msvc_platform[k], type[i], (cp_type == CP_TYPE_LIBRARY) ? 4 : 1);
                if (cp_type == CP_TYPE_CONSOLE) {
                    fprintf(outfile, msvc8_charset);
                }
                fprintf(outfile, msvc8_config_part3);
                if (cp_post_custom_message) {
                    fprintf(outfile, msvc8_cbt_helper, cp_post_custom_command, cp_post_custom_output);
                } else {
                    fprintf(outfile, msvc8_cbt);
                }
                fprintf(outfile, msvc8_config_part4);
                switch (cp_type) {
                    default:
                    case CP_TYPE_GUI:
                        fprintf(outfile, msvc8_midl_tool_gui, msvc_preprodefs[i & 1], k + 1, cp_name);
                        break;
                    case CP_TYPE_CONSOLE:
                        if (k == 0) {
                            fprintf(outfile, msvc8_midl_tool_console_x86, cp_name);
                        } else {
                            fprintf(outfile, msvc8_midl_tool_console, k + 1, cp_name);
                        }
                        break;
                    case CP_TYPE_LIBRARY:
                        if (k == 0) {
                            fprintf(outfile, msvc8_midl_tool_lib_x86);
                        } else {
                            fprintf(outfile, msvc8_midl_tool_lib, k + 1);
                        }
                        break;
                }
                fprintf(outfile, msvc8_compiler_tool_start);
                if (!(i & 1)) {
                    fprintf(outfile, msvc8_opt0);
                } else {
                    fprintf(outfile, msvc8_ife);
                }
                if (cp_cc_source_path) {
                    if (cp_source_names[0]) {
                        if (sdl) {
                            fprintf(outfile, msvc8_includes_cc_mixed_sdl);
                        } else {
                            fprintf(outfile, msvc8_includes_cc_mixed);
                        }
                    } else {
                        if (sdl) {
                            fprintf(outfile, msvc8_includes_cc_sdl);
                        } else {
                            fprintf(outfile, msvc8_includes_cc);
                        }
                    }
                } else {
                    if (sdl) {
                        fprintf(outfile, msvc8_includes_sdl);
                    } else {
                        fprintf(outfile, msvc8_includes);
                    }
                    if (cp_include_dirs[0]) {
                        for (j = 0; cp_include_dirs[j]; j++) {
                            fprintf(outfile, ",..\\..\\..\\%s", cp_include_dirs[j]);
                        }
                    }
                    fprintf(outfile, "\"\r\n");
                }
                if (!cp_cc_source_path) {
                    fprintf(outfile, msvc8_defs, msvc_predefs[i]);
                } else {
                    fprintf(outfile, msvc8_defs_cc, msvc_predefs[i], cp_name);
                }
                if (i & 1) {
                    fprintf(outfile, msvc8_string_pooling);
                }
                fprintf(outfile, msvc8_ct_part2, (!(i & 1)) + (sdl * 2));
                if (i & 1) {
                    fprintf(outfile, msvc8_efll);
                }
                fprintf(outfile, msvc8_ct_part3, cp_name, msvc_platform[k], type[i], cp_name, cp_name, msvc_platform[k], type[i], cp_name, msvc_platform[k], type[i], cp_name, msvc_platform[k], type[i]);
                if (!(i & 1)) {
                    fprintf(outfile, msvc8_dif);
                }
                fprintf(outfile, msvc8_config_part5);
                switch (cp_type) {
                    default:
                    case CP_TYPE_GUI:
                        fprintf(outfile, msvc8_rct_gui, msvc_predefs[i]);
                        break;
                    case CP_TYPE_CONSOLE:
                        fprintf(outfile, msvc8_rct_console, msvc_predefs[i]);
                        break;
                    case CP_TYPE_LIBRARY:
                        fprintf(outfile, msvc8_rct_lib);
                        break;
                }
                fprintf(outfile, msvc8_config_part6);
                if (cp_type == CP_TYPE_LIBRARY) {
                    fprintf(outfile, msvc8_lib_tool, cp_name, msvc_platform[k], type[i], cp_name);
                } else {
                    if (sdl) {
                        if (cp_type == CP_TYPE_CONSOLE) {
                            libs = msvc_libs_console_sdl;
                        } else {
                            libs = msvc_libs_gui_sdl;
                        }
                    } else {
                        if (cp_type == CP_TYPE_CONSOLE) {
                            libs = msvc_libs_console;
                        } else {
                            libs = msvc_libs_gui[i >> 1];
                        }
                    }
                    fprintf(outfile, msvc8_linker_tool_part1, libs, cp_name, (!(i & 1)) + 1);
                    if (!(i & 1)) {
                        if (sdl) {
                            fprintf(outfile, msvc8_idln);
                        }
                        fprintf(outfile, msvc8_linker_tool_debug);
                    }
                    fprintf(outfile, msvc8_linker_tool_part2, cp_name, (cp_type == CP_TYPE_GUI) ? 2 : 1);
                }
                if (cp_type != CP_TYPE_LIBRARY) {
                    if (k == 1) {
                        fprintf(outfile, "\t\t\t\tTargetMachine=\"5\"\r\n");
                    } else if (k == 2) {
                        fprintf(outfile, "\t\t\t\tTargetMachine=\"17\"\r\n");
                    }
                    fprintf(outfile, "\t\t\t/>\r\n");
                }
                fprintf(outfile, msvc8_link_tool);
                if (cp_type != CP_TYPE_LIBRARY) {
                    fprintf(outfile, msvc8_manifest_tool);
                }
                fprintf(outfile, msvc8_config_part7);
                if (cp_type != CP_TYPE_LIBRARY) {
                    fprintf(outfile, msvc8_app_tools);
                }
                fprintf(outfile, msvc8_conf_end);
            }
        }
        fprintf(outfile, msvc8_confs_files);
        if (cp_source_names[0]) {
            for (j = 0; cp_source_names[j]; j++) {
                fprintf(outfile, msvc8_file, cp_source_names[j]);
            }
        }
        if (cp_sdl_source_names[0] && sdl) {
            for (j = 0; cp_sdl_source_names[j]; j++) {
                fprintf(outfile, msvc8_file, cp_sdl_source_names[j]);
            }
        }
        if (cp_native_source_names[0] && !sdl) {
            for (j = 0; cp_native_source_names[j]; j++) {
                fprintf(outfile, msvc8_file, cp_native_source_names[j]);
            }
        }
        if (cp_cpusource_names[0]) {
            for (j = 0; cp_cpusource_names[j]; j++) {
                fprintf(outfile, msvc8_cpu_file, cp_cpusource_names[j]);
                for (k = 0; k < 3; k++) {
                    for (i = 0; i < 2; i++) {
                        fprintf(outfile, msvc8_cpu_fileconf, msvc_type_name[(i * 2) + 1], msvc_platform[k]);
                    }
                }
                fprintf(outfile, msvc_file_end);
            }
        }
        if (cp_custom_source) {
            fprintf(outfile, msvc8_custom_file, cp_custom_source);
            for (k = 0; k < 3; k++) {
                for (i = 0; i < max_i; i++) {
                    fprintf(outfile, msvc8_custom_conf, type_name[i], msvc_platform[k], cp_custom_message, cp_custom_command);
                    for (j = 0; cp_custom_deps[j]; j++) {
                        fprintf(outfile, "%s;", cp_custom_deps[j]);
                    }
                    fprintf(outfile, msvc8_custom_conf2, cp_custom_output);
                }
            }
            fprintf(outfile, msvc_file_end);
        }
        if (!strcmp(cp_name, "base")) {
            fprintf(outfile, msvc8_winid_start);
            for (k = 0; k < 3; k++) {
                for (i = 0; i < max_i; i++) {
                    fprintf(outfile, msvc8_winid_conf, type_name[i], msvc_platform[k], msvc_winid_copy[k], msvc_winid_copy[k]);
                }
            }
            fprintf(outfile, msvc_file_end);
        }

        if (cp_cc_source_names[0]) {
            for (j = 0; cp_cc_source_names[j]; j++) {
                fprintf(outfile, msvc8_cc_file, cp_cc_source_path, cp_cc_source_names[j]);
                for (k = 0; k < 3; k++) {
                    for (i = 0; i < max_i; i++) {
                        if (sdl) {
                            fprintf(outfile, msvc8_cc_conf, type_name[i], msvc_platform[k], msvc_flags_sdl[i & 1], (cp_source_names[0]) ? msvc_cc_inc_sid_sdl : msvc_cc_inc_sdl, msvc_cc_extra[i], cp_name, cp_name, msvc_platform[k], type[i], cp_name, cp_name, msvc_platform[k], type[i], cp_name, msvc_platform[k], type[i], cp_name, msvc_platform[k], type[i]);
                        } else {
                            fprintf(outfile, msvc8_cc_conf, type_name[i], msvc_platform[k], msvc_flags[i & 1], (cp_source_names[0]) ? msvc_cc_inc_sid : msvc_cc_inc, msvc_cc_extra[i], cp_name, cp_name, msvc_platform[k], type[i], cp_name, cp_name, msvc_platform[k], type[i], cp_name, msvc_platform[k], type[i], cp_name, msvc_platform[k], type[i]);
                        }
                    }
                }
                fprintf(outfile, msvc_file_end);
            }
        }
        if (cp_res_source_name && !sdl) {
            fprintf(outfile, msvc8_res_file, cp_res_source_name);
            for (k = 0; k < 3; k++) {
                for (i = 0; i < max_i; i++) {
                    fprintf(outfile, msvc8_res_conf, type_name[i], msvc_platform[k]);
                    for (j = 0; cp_res_deps[j]; j++) {
                        fprintf(outfile, "..\\%s ", cp_res_deps[j]);
                        if (cp_res_deps[j + 1]) {
                            fprintf(outfile, "+ ");
                        }
                    }
                    fprintf(outfile, msvc8_res_conf2, cp_res_output_name);
                    for (j = 0; cp_res_deps[j]; j++) {
                        fprintf(outfile, "..\\%s;", cp_res_deps[j]);
                    }
                    fprintf(outfile, msvc8_res_conf3, cp_res_output_name);
                }
            }
            fprintf(outfile, msvc8_res_end, cp_res_output_name);
            for (k = 0; k < 3; k++) {
                for (i = 0; i < max_i; i++) {
                    fprintf(outfile, msvc8_manifest_conf, type_name[i], msvc_platform[k]);
                }
            }
            fprintf(outfile, msvc_file_end);
        }
        fprintf(outfile, msvc8_end_project);
    }

    if (outfile) {
        fclose(outfile);
    }

    if (filename) {
        free(filename);
    }

    if (cp_libs) {
        free(cp_libs);
        cp_libs = NULL;
    }

    if (read_buffer) {
        free(read_buffer);
        read_buffer = NULL;
        read_buffer_line = 0;
        read_buffer_pos = 0;
        read_buffer_len = 0;
    }

    return retval;
}

/* ---------------------------------------------------------------------- */

static char *msvc7x_main_project_end = "\tEndGlobalSection\r\n"
                                       "\tGlobalSection(ExtensibilityGlobals) = postSolution\r\n"
                                       "\tEndGlobalSection\r\n"
                                       "\tGlobalSection(ExtensibilityAddIns) = postSolution\r\n"
                                       "\tEndGlobalSection\r\n"
                                       "EndGlobal\r\n";

static char *msvc7x_main_project_confs = "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.Debug.ActiveCfg = Debug|Win32\r\n"
                                         "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.Debug.Build.0 = Debug|Win32\r\n"
                                         "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.DX Debug.ActiveCfg = DX Debug|Win32\r\n"
                                         "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.DX Debug.Build.0 = DX Debug|Win32\r\n"
                                         "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.DX Release.ActiveCfg = DX Release|Win32\r\n"
                                         "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.DX Release.Build.0 = DX Release|Win32\r\n"
                                         "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.Release.ActiveCfg = Release|Win32\r\n"
                                         "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.Release.Build.0 = Release|Win32\r\n";

static char *msvc7x_main_project_confs_sdl = "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.Debug.ActiveCfg = Debug|Win32\r\n"
                                             "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.Debug.Build.0 = Debug|Win32\r\n"
                                             "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.Release.ActiveCfg = Release|Win32\r\n"
                                             "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.Release.Build.0 = Release|Win32\r\n";

static char *msvc71_xml_header = "<?xml version=\"1.0\" encoding=\"Windows-1252\"?>\r\n";

static char *msvc71_xmldgt = "\t\t\t<Tool\r\n"
                             "\t\t\t\tName=\"VCXMLDataGeneratorTool\"/>\r\n";

static char *msvc71_wrapper_tools = "\t\t\t<Tool\r\n"
                                    "\t\t\t\tName=\"VCManagedWrapperGeneratorTool\"/>\r\n"
                                    "\t\t\t<Tool\r\n"
                                    "\t\t\t\tName=\"VCAuxiliaryManagedWrapperGeneratorTool\"/>\r\n";

static char *msvc71_configs_files = "\t</Configurations>\r\n"
                                    "\t<References>\r\n"
                                    "\t</References>\r\n"
                                    "\t<Files>\r\n";

static char *msvc71_project_deps_start = "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"%s\", \"%s.vcproj\", \"{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}\"\r\n"
                                         "\tProjectSection(ProjectDependencies) = postProject\r\n";

static char *msvc71_project_deps = "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X} = {8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}\r\n";

static char *msvc71_project_deps_end = "\tEndProjectSection\r\n"
                                       "EndProject\r\n";

static char *msvc71_project_global_start = "Global\r\n"
                                           "\tGlobalSection(SolutionConfiguration) = preSolution\r\n"
                                           "\t\tDebug = Debug\r\n"
                                           "\t\tDX Debug = DX Debug\r\n"
                                           "\t\tDX Release = DX Release\r\n"
                                           "\t\tRelease = Release\r\n"
                                           "\tEndGlobalSection\r\n"
                                           "\tGlobalSection(ProjectConfiguration) = postSolution\r\n";

static char *msvc71_project_global_start_sdl = "Global\r\n"
                                               "\tGlobalSection(SolutionConfiguration) = preSolution\r\n"
                                               "\t\tDebug = Debug\r\n"
                                               "\t\tRelease = Release\r\n"
                                               "\tEndGlobalSection\r\n"
                                               "\tGlobalSection(ProjectConfiguration) = postSolution\r\n";

static void generate_msvc71_sln(int sdl)
{
    int i, j;
    int index;

    for (i = 0; project_info[i].name; i++) {
        fprintf(mainfile, msvc71_project_deps_start, project_info[i].name, project_info[i].name, i);
        if (project_info[i].dep[0]) {
            for (j = 0; project_info[i].dep[j]; j++) {
                index = pi_get_index_of_name(project_info[i].dep[j]);
                fprintf(mainfile, msvc71_project_deps, index, index);
            }
        }
        fprintf(mainfile, msvc71_project_deps_end);
    }
    if (sdl) {
        fprintf(mainfile, msvc71_project_global_start_sdl);
    } else {
        fprintf(mainfile, msvc71_project_global_start);
    }
    for (i = 0; project_info[i].name; i++) {
        if (sdl) {
            fprintf(mainfile, msvc7x_main_project_confs_sdl, i, i, i, i);
        } else {
            fprintf(mainfile, msvc7x_main_project_confs, i, i, i, i, i, i, i, i);
        }
    }
    fprintf(mainfile, msvc7x_main_project_end);
}

static int open_msvc71_main_project(int sdl)
{
    pi_init();

    if (sdl) {
        if (ffmpeg) {
            mainfile = fopen("../../sdl/win32-msvc71-ffmpeg/vice.sln", "wb");
        } else {
            mainfile = fopen("../../sdl/win32-msvc71/vice.sln", "wb");
        }
    } else {
        if (ffmpeg) {
            mainfile = fopen("../vs71-ffmpeg/vice.sln", "wb");
        } else {
            mainfile = fopen("../vs71/vice.sln", "wb");
        }
    }

    if (!mainfile) {
        printf("Cannot open 'vice.sln' for output\n");
        return 1;
    }
    fprintf(mainfile, "Microsoft Visual Studio Solution File, Format Version 7.10\r\n");
    return 0;
}

static void close_msvc71_main_project(int sdl)
{
    generate_msvc71_sln(sdl);
    pi_exit();
    fclose(mainfile);
}

/* ---------------------------------------------------------------------- */


static char *msvc70_compiler_tool_type[2] = {
    "Optimization=\"0\"",
    "InlineFunctionExpansion=\"1\""
};

static char *msvc70_cc_predefs = ",PACKAGE=\\&quot;%s\\&quot;,VERSION=\\&quot;0.7\\&quot;,SIZEOF_INT=4";

static char *msvc70_console_libs = "version.lib wsock32.lib";

static char *msvc70_gui_libs[2] = {
    "comctl32.lib dsound.lib dxguid.lib winmm.lib version.lib wsock32.lib",
    "comctl32.lib version.lib winmm.lib wsock32.lib"
};

static char *msvc70_console_libs_sdl = "version.lib wsock32.lib SDLmain.lib SDL.lib opengl32.lib";

static char *msvc70_gui_libs_sdl = "comctl32.lib version.lib winmm.lib wsock32.lib SDLmain.lib SDL.lib opengl32.lib";

static char *msvc70_xml_header = "<?xml version=\"1.0\" encoding = \"Windows-1252\"?>\r\n";

static char *msvc70_project_start = "<VisualStudioProject\r\n"
                                    "\tProjectType=\"Visual C++\"\r\n"
                                    "\tVersion=\"%s\"\r\n"
                                    "\tName=\"%s\"\r\n"
                                    "\tSccProjectName=\"\"\r\n"
                                    "\tSccLocalPath=\"\">\r\n"
                                    "\t<Platforms>\r\n"
                                    "\t\t<Platform\r\n"
                                    "\t\t\tName=\"Win32\"/>\r\n"
                                    "\t</Platforms>\r\n";

static char *msvc70_configurations = "\t<Configurations>\r\n";

static char *msvc70_config_part1 = "\t\t<Configuration\r\n"
                                   "\t\t\tName=\"%s|Win32\"\r\n";

static char *msvc70_config_part2_lib = "\t\t\tOutputDirectory=\".\\libs\\%s\\%s\"\r\n";

static char *msvc70_config_part2_app = "\t\t\tOutputDirectory=\".\\..\\..\\..\\..\\data\"\r\n";

static char *msvc70_config_part3 = "\t\t\tIntermediateDirectory=\".\\libs\\%s\\%s\"\r\n"
                                   "\t\t\tConfigurationType=\"%d\"\r\n"
                                   "\t\t\tUseOfMFC=\"0\"\r\n"
                                   "\t\t\tATLMinimizesCRunTimeLibraryUsage=\"FALSE\"";

static char *msvc70_charset = "\r\n\t\t\tCharacterSet=\"2\"";

static char *msvc70_compiler_tool_part1 = ">\r\n"
                                          "\t\t\t<Tool\r\n"
                                          "\t\t\t\tName=\"VCCLCompilerTool\"\r\n"
                                          "\t\t\t\t%s\r\n";

static char *msvc70_aid = "\t\t\t\tAdditionalIncludeDirectories=\"..\\msvc,..\\,..\\..\\..\\";

static char *msvc70_aid_sdl = "\t\t\t\tAdditionalIncludeDirectories=\".\\,..\\,..\\..\\..\\";

static char *msvc70_aid_cc = "\t\t\t\tAdditionalIncludeDirectories=\"..\\msvc";

static char *msvc70_aid_cc_sdl = "\t\t\t\tAdditionalIncludeDirectories=\".\\;..\\";

static char *msvc70_compiler_tool_part2 = "\t\t\t\tPreprocessorDefinitions=\"WIN32,_WINDOWS,IDE_COMPILE,DONT_USE_UNISTD_H,";

static char *msvc70_predefs_end = "\"\r\n";

static char *msvc70_string_pooling = "\t\t\t\tStringPooling=\"TRUE\"\r\n";

static char *msvc70_compiler_tool_part3 = "\t\t\t\tRuntimeLibrary=\"%d\"\r\n";

static char *msvc70_fll = "\t\t\t\tEnableFunctionLevelLinking=\"TRUE\"\r\n";

static char *msvc70_compiler_tool_part4 = "\t\t\t\tUsePrecompiledHeader=\"2\"\r\n"
                                          "\t\t\t\tPrecompiledHeaderFile=\".\\libs\\%s\\%s\\%s.pch\"\r\n"
                                          "\t\t\t\tAssemblerListingLocation=\".\\libs\\%s\\%s\\\"\r\n"
                                          "\t\t\t\tObjectFile=\".\\libs\\%s\\%s\\\"\r\n"
                                          "\t\t\t\tProgramDataBaseFileName=\".\\libs\\%s\\%s\\\"\r\n"
                                          "\t\t\t\tWarningLevel=\"3\"\r\n"
                                          "\t\t\t\tSuppressStartupBanner=\"TRUE\"\r\n";

static char *msvc70_dif = "\t\t\t\tDebugInformationFormat=\"1\"\r\n";

static char *msvc70_compiler_tool_part5 = "\t\t\t\tCompileAs=\"0\"/>\r\n";

static char *msvc70_cbt_normal = "\t\t\t<Tool\r\n"
                                 "\t\t\t\tName=\"VCCustomBuildTool\"/>\r\n";

static char *msvc70_cbt_custom = "\t\t\t<Tool\r\n"
                                 "\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                                 "\t\t\t\tCommandLine=\"%s\r\n\"\r\n"
                                 "\t\t\t\tOutputs=\"%s\"/>\r\n";

static char *msvc70_lib_tool = "\t\t\t<Tool\r\n"
                               "\t\t\t\tName=\"VCLibrarianTool\"\r\n"
                               "\t\t\t\tOutputFile=\".\\libs\\%s\\%s\\%s.lib\"\r\n"
                               "\t\t\t\tSuppressStartupBanner=\"TRUE\"/>\r\n";

static char *msvc70_linker_tool_part1 = "\t\t\t<Tool\r\n"
                                        "\t\t\t\tName=\"VCLinkerTool\"\r\n"
                                        "\t\t\t\tAdditionalOptions=\"/MACHINE:I386\"\r\n"
                                        "\t\t\t\tAdditionalDependencies=\"%s\"\r\n"
                                        "\t\t\t\tOutputFile=\".\\..\\..\\..\\..\\data\\%s.exe\"\r\n"
                                        "\t\t\t\tLinkIncremental=\"%d\"\r\n"
                                        "\t\t\t\tSuppressStartupBanner=\"TRUE\"\r\n";

static char *msvc70_gdi = "\t\t\t\tGenerateDebugInformation=\"TRUE\"\r\n";

static char *msvc70_linker_tool_part2 = "\t\t\t\tProgramDatabaseFile=\".\\..\\..\\..\\..\\data\\%s.pdb\"\r\n"
                                        "\t\t\t\tSubSystem=\"%d\"/>\r\n";

static char *msvc70_vcmidl_tool_console = "\t\t\t<Tool\r\n"
                                          "\t\t\t\tName=\"VCMIDLTool\"\r\n"
                                          "\t\t\t\tTypeLibraryName=\".\\..\\..\\..\\..\\data\\%s.tlb\"/>\r\n";

static char *msvc70_vcmidl_tool_gui = "\t\t\t<Tool\r\n"
                                      "\t\t\t\tName=\"VCMIDLTool\"\r\n"
                                      "\t\t\t\tPreprocessorDefinitions=\"%s\"\r\n"
                                      "\t\t\t\tMkTypLibCompatible=\"TRUE\"\r\n"
                                      "\t\t\t\tSuppressStartupBanner=\"TRUE\"\r\n"
                                      "\t\t\t\tTargetEnvironment=\"1\"\r\n"
                                      "\t\t\t\tTypeLibraryName=\".\\..\\..\\..\\..\\data\\%s.tlb\"/>\r\n";


static char *msvc70_vcmidl_tool_lib = "\t\t\t<Tool\r\n"
                                      "\t\t\t\tName=\"VCMIDLTool\"/>\r\n";

static char *msvc70_link_event_tool = "\t\t\t<Tool\r\n"
                                      "\t\t\t\tName=\"VCPostBuildEventTool\"/>\r\n"
                                      "\t\t\t<Tool\r\n"
                                      "\t\t\t\tName=\"VCPreBuildEventTool\"/>\r\n"
                                      "\t\t\t<Tool\r\n"
                                      "\t\t\t\tName=\"VCPreLinkEventTool\"/>\r\n";

static char *msvc70_resource_tool_lib = "\t\t\t<Tool\r\n"
                                        "\t\t\t\tName=\"VCResourceCompilerTool\"\r\n"
                                        "\t\t\t\tCulture=\"1033\"/>\r\n";

static char *msvc70_resource_tool_console = "\t\t\t<Tool\r\n"
                                            "\t\t\t\tName=\"VCResourceCompilerTool\"\r\n"
                                            "\t\t\t\tPreprocessorDefinitions=\"%s\"\r\n"
                                            "\t\t\t\tCulture=\"1033\"/>\r\n";

static char *msvc70_resource_tool_gui = "\t\t\t<Tool\r\n"
                                        "\t\t\t\tName=\"VCResourceCompilerTool\"\r\n"
                                        "\t\t\t\tPreprocessorDefinitions=\"WIN32,_WINDOWS,IDE_COMPILE,DONT_USE_UNISTD_H,%s\"\r\n"
                                        "\t\t\t\tCulture=\"1033\"\r\n"
                                        "\t\t\t\tAdditionalIncludeDirectories=\"..\\msvc;..\\;..\\..\\..\\\"/>\r\n";

static char *msvc70_wspgt = "\t\t\t<Tool\r\n"
                            "\t\t\t\tName=\"VCWebServiceProxyGeneratorTool\"/>\r\n";

static char *msvc70_wdt = "\t\t\t<Tool\r\n"
                          "\t\t\t\tName=\"VCWebDeploymentTool\"/>\r\n";

static char *msvc70_config_end = "\t\t</Configuration>\r\n";

static char *msvc70_configs_files = "\t</Configurations>\r\n"
                                    "\t<Files>\r\n";

static char *msvc70_file = "\t\t<File\r\n"
                           "\t\t\tRelativePath=\"..\\..\\..\\%s\">\r\n"
                           "\t\t</File>\r\n";

static char *msvc70_file_start = "\t\t<File\r\n"
                                 "\t\t\tRelativePath=\"%s\">\r\n";

static char *msvc70_custom_fc = "\t\t\t<FileConfiguration\r\n"
                                "\t\t\t\tName=\"%s|Win32\">\r\n"
                                "\t\t\t\t<Tool\r\n"
                                "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                                "\t\t\t\t\tDescription=\"%s\"\r\n"
                                "\t\t\t\t\tCommandLine=\"%s\r\n\"\r\n"
                                "\t\t\t\t\tAdditionalDependencies=\"";

static char *msvc70_custom_output = "\"\r\n"
                                    "\t\t\t\t\tOutputs=\"%s\"/>\r\n"
                                    "\t\t\t</FileConfiguration>\r\n";

static char *msvc70_end_file = "\t\t</File>\r\n";

static char *msvc70_cpu_file = "\t\t<File\r\n"
                               "\t\t\tRelativePath=\"..\\..\\..\\%s\">\r\n"
                               "\t\t\t<FileConfiguration\r\n"
                               "\t\t\t\tName=\"DX Release|Win32\">\r\n"
                               "\t\t\t\t<Tool\r\n"
                               "\t\t\t\t\tName=\"VCCLCompilerTool\"\r\n"
                               "\t\t\t\t\tOptimization=\"4\"\r\n"
                               "\t\t\t\t\tEnableIntrinsicFunctions=\"TRUE\"\r\n"
                               "\t\t\t\t\tImproveFloatingPointConsistency=\"TRUE\"\r\n"
                               "\t\t\t\t\tFavorSizeOrSpeed=\"0\"\r\n"
                               "\t\t\t\t\tOmitFramePointers=\"TRUE\"/>\r\n"
                               "\t\t\t</FileConfiguration>\r\n"
                               "\t\t\t<FileConfiguration\r\n"
                               "\t\t\t\tName=\"Release|Win32\">\r\n"
                               "\t\t\t\t<Tool\r\n"
                               "\t\t\t\t\tName=\"VCCLCompilerTool\"\r\n"
                               "\t\t\t\t\tOptimization=\"4\"\r\n"
                               "\t\t\t\t\tEnableIntrinsicFunctions=\"TRUE\"\r\n"
                               "\t\t\t\t\tImproveFloatingPointConsistency=\"TRUE\"\r\n"
                               "\t\t\t\t\tFavorSizeOrSpeed=\"0\"\r\n"
                               "\t\t\t\t\tOmitFramePointers=\"TRUE\"/>\r\n"
                               "\t\t\t</FileConfiguration>\r\n"
                               "\t\t</File>\r\n";

static char *msvc70_cpu_file_sdl = "\t\t<File\r\n"
                                   "\t\t\tRelativePath=\"..\\..\\..\\%s\">\r\n"
                                   "\t\t\t<FileConfiguration\r\n"
                                   "\t\t\t\tName=\"Release|Win32\">\r\n"
                                   "\t\t\t\t<Tool\r\n"
                                   "\t\t\t\t\tName=\"VCCLCompilerTool\"\r\n"
                                   "\t\t\t\t\tOptimization=\"4\"\r\n"
                                   "\t\t\t\t\tEnableIntrinsicFunctions=\"TRUE\"\r\n"
                                   "\t\t\t\t\tImproveFloatingPointConsistency=\"TRUE\"\r\n"
                                   "\t\t\t\t\tFavorSizeOrSpeed=\"0\"\r\n"
                                   "\t\t\t\t\tOmitFramePointers=\"TRUE\"/>\r\n"
                                   "\t\t\t</FileConfiguration>\r\n"
                                   "\t\t</File>\r\n";

static char *msvc70_cc_file_start = "\t\t<File\r\n"
                                    "\t\t\tRelativePath=\"..\\..\\..\\%s\\%s.cc\">\r\n";

static char *msvc70_cc_fc = "\t\t\t<FileConfiguration\r\n"
                            "\t\t\t\tName=\"%s|Win32\">\r\n"
                            "\t\t\t\t<Tool\r\n"
                            "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n";

static char *msvc70_cc_command = "\t\t\t\t\tCommandLine=\"cl /nologo %s /I &quot;..\\msvc&quot; /D &quot;WIN32&quot; /D &quot;_WINDOWS&quot; /D &quot;IDE_COMPILE&quot; /D &quot;DONT_USE_UNISTD_H&quot; %s /D PACKAGE=\\&quot;%s\\&quot; /D VERSION=\\&quot;0.7\\&quot; /D SIZEOF_INT=4 /Fp&quot;libs\\%s\\%s\\%s.pch&quot; /Fo&quot;libs\\%s\\%s/&quot; /Fd&quot;libs\\%s\\%s/&quot; /FD /TP /c &quot;$(InputPath)&quot;\r\n\"\r\n"
                                 "\t\t\t\t\tOutputs=\"libs\\%s\\%s\\$(InputName).obj\"/>\r\n"
                                 "\t\t\t</FileConfiguration>\r\n";

static char *msvc70_cc_command_sdl = "\t\t\t\t\tCommandLine=\"cl /nologo %s /I &quot;./&quot; /I &quot;../&quot; /D &quot;WIN32&quot; /D &quot;_WINDOWS&quot; /D &quot;IDE_COMPILE&quot; /D &quot;DONT_USE_UNISTD_H&quot; %s /D PACKAGE=\\&quot;%s\\&quot; /D VERSION=\\&quot;0.7\\&quot; /D SIZEOF_INT=4 /Fp&quot;libs\\%s\\%s\\%s.pch&quot; /Fo&quot;libs\\%s\\%s/&quot; /Fd&quot;libs\\%s\\%s/&quot; /FD /TP /c &quot;$(InputPath)&quot;\r\n\"\r\n"
                                 "\t\t\t\t\tOutputs=\"libs\\%s\\%s\\$(InputName).obj\"/>\r\n"
                                 "\t\t\t</FileConfiguration>\r\n";

static char *msvc70_cc_command_sid = "\t\t\t\t\tCommandLine=\"cl /nologo %s /I &quot;..\\msvc&quot; /I &quot;..\\..\\../&quot; /I &quot;../&quot; /I &quot;..\\..\\..\\sid&quot; /D &quot;WIN32&quot; /D &quot;_WINDOWS&quot; /D &quot;IDE_COMPILE&quot; /D &quot;DONT_USE_UNISTD_H&quot; %s /D PACKAGE=\\&quot;%s\\&quot; /D VERSION=\\&quot;0.7\\&quot; /D SIZEOF_INT=4 /Fp&quot;libs\\%s\\%s\\%s.pch&quot; /Fo&quot;libs\\%s\\%s/&quot; /Fd&quot;libs\\%s\\%s/&quot; /FD /TP /c &quot;$(InputPath)&quot;\r\n\"\r\n"
                                     "\t\t\t\t\tOutputs=\"libs\\%s\\%s\\$(InputName).obj\"/>\r\n"
                                     "\t\t\t</FileConfiguration>\r\n";

static char *msvc70_cc_command_sid_sdl = "\t\t\t\t\tCommandLine=\"cl /nologo %s /I &quot;./&quot; /I &quot;..\\..\\../&quot; /I &quot;../&quot; /I &quot;..\\..\\..\\sid&quot; /D &quot;WIN32&quot; /D &quot;_WINDOWS&quot; /D &quot;IDE_COMPILE&quot; /D &quot;DONT_USE_UNISTD_H&quot; %s /D PACKAGE=\\&quot;%s\\&quot; /D VERSION=\\&quot;0.7\\&quot; /D SIZEOF_INT=4 /Fp&quot;libs\\%s\\%s\\%s.pch&quot; /Fo&quot;libs\\%s\\%s/&quot; /Fd&quot;libs\\%s\\%s/&quot; /FD /TP /c &quot;$(InputPath)&quot;\r\n\"\r\n"
                                     "\t\t\t\t\tOutputs=\"libs\\%s\\%s\\$(InputName).obj\"/>\r\n"
                                     "\t\t\t</FileConfiguration>\r\n";

static char *msvc70_res_file = "\t\t<File\r\n"
                               "\t\t\tRelativePath=\"..\\%s\">\r\n";

static char *msvc70_res_fc = "\t\t\t<FileConfiguration\r\n"
                             "\t\t\t\tName=\"%s|Win32\">\r\n"
                             "\t\t\t\t<Tool\r\n"
                             "\t\t\t\t\tName=\"VCCustomBuildTool\"\r\n"
                             "\t\t\t\t\tCommandLine=\"copy /b ";

static char *msvc70_res_deps = "\t\t\t\t\tAdditionalDependencies=\"..\\..\\..\\debug.h;";

static char *msvc70_res_output = "\"\r\n"
                                 "\t\t\t\t\tOutputs=\"%s\"/>\r\n"
                                 "\t\t\t</FileConfiguration>\r\n";

static char *msvc70_res_end = "\t\t</File>\r\n"
                              "\t\t<File\r\n"
                              "\t\t\tRelativePath=\".\\%s\">\r\n"
                              "\t\t</File>\r\n"
                              "\t\t<File\r\n"
                              "\t\t\tRelativePath=\"..\\vice.manifest\">\r\n"
                              "\t\t</File>\r\n";

static char *msvc70_end_project = "\t</Files>\r\n"
                                  "\t<Globals>\r\n"
                                  "\t</Globals>\r\n"
                                  "</VisualStudioProject>\r\n";

static char *msvc70_main_projects = "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"%s\", \"%s.vcproj\", \"{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}\"\r\n"
                                    "EndProject\r\n";

static char *msvc70_main_project_global = "Global\r\n"
                                          "\tGlobalSection(SolutionConfiguration) = preSolution\r\n"
                                          "\t\tConfigName.0 = Debug\r\n"
                                          "\t\tConfigName.1 = DX Debug\r\n"
                                          "\t\tConfigName.2 = DX Release\r\n"
                                          "\t\tConfigName.3 = Release\r\n"
                                          "\tEndGlobalSection\r\n"
                                          "\tGlobalSection(ProjectDependencies) = postSolution\r\n";

static char *msvc70_main_project_global_sdl = "Global\r\n"
                                            "\tGlobalSection(SolutionConfiguration) = preSolution\r\n"
                                            "\t\tConfigName.0 = Debug\r\n"
                                            "\t\tConfigName.1 = Release\r\n"
                                            "\tEndGlobalSection\r\n"
                                            "\tGlobalSection(ProjectDependencies) = postSolution\r\n";

static char *msvc70_main_project_deps = "\t\t{8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}.%d = {8BC9CEB8-8B4A-11D0-8D12-00A0C91BC9%02X}\r\n";

static char *msvc70_main_project_conf = "\tEndGlobalSection\r\n"
                                        "\tGlobalSection(ProjectConfiguration) = postSolution\r\n";

static void generate_msvc70_sln(int sdl)
{
    int i, j;

    for (i = 0; project_info[i].name; i++) {
        fprintf(mainfile, msvc70_main_projects, project_info[i].name, project_info[i].name, i);
    }
    if (sdl) {
        fprintf(mainfile, msvc70_main_project_global_sdl);
    } else {
        fprintf(mainfile, msvc70_main_project_global);
    }
    for (i = 0; project_info[i].name; i++) {
        if (project_info[i].dep[0]) {
            for (j = 0; project_info[i].dep[j]; j++) {
                fprintf(mainfile, msvc70_main_project_deps, i, j, pi_get_index_of_name(project_info[i].dep[j]));
            }
        }
    }
    fprintf(mainfile, msvc70_main_project_conf);
    for (i = 0; project_info[i].name; i++) {
        if (sdl) {
            fprintf(mainfile, msvc7x_main_project_confs_sdl, i, i, i, i);
        } else {
            fprintf(mainfile, msvc7x_main_project_confs, i, i, i, i, i, i, i, i);
        }
    }
    fprintf(mainfile, msvc7x_main_project_end);
}

static int open_msvc70_main_project(int sdl)
{
    pi_init();

    if (sdl) {
        if (ffmpeg) {
            mainfile = fopen("../../sdl/win32-msvc70-ffmpeg/vice.sln", "wb");
        } else {
            mainfile = fopen("../../sdl/win32-msvc70/vice.sln", "wb");
        }
    } else {
        if (ffmpeg) {
            mainfile = fopen("../vs70-ffmpeg/vice.sln", "wb");
        } else {
            mainfile = fopen("../vs70/vice.sln", "wb");
        }
    }

    if (!mainfile) {
        printf("Cannot open 'vice.sln' for output\n");
        return 1;
    }
    fprintf(mainfile, "Microsoft Visual Studio Solution File, Format Version 7.00\r\n");
    return 0;
}

static void close_msvc70_main_project(int sdl)
{
    generate_msvc70_sln(sdl);
    pi_exit();
    fclose(mainfile);
}

static int output_msvc7_file(char *fname, int filelist, int version, int sdl)
{
    char *filename;
    char *rfname;
    int retval = 0;
    FILE *outfile = NULL;
    int i, j;
    int max_i = (sdl) ? 2 : 4;
    char **type_name = (sdl) ? msvc_type_sdl : msvc_type_name;
    char **type = (sdl) ? msvc_type_sdl : msvc_type;
    char *libs;
    char **msvc_cc_extra = (ffmpeg) ? msvc_cc_extra_ffmpeg : msvc_cc_extra_noffmpeg;
    char **msvc_predefs = (ffmpeg) ? msvc_predefs_ffmpeg : msvc_predefs_noffmpeg;

    if (!strcmp(fname, "arch_native") || !strcmp(fname, "arch_sdl")) {
        rfname = "arch";
    } else {
        rfname = fname;
    }

    if (filelist) {
        i = pi_insert_name(cp_name);
        if (cp_dep_names[0]) {
            pi_insert_deps(cp_dep_names, i);
        }
        if (sdl) {
            if (ffmpeg) {
                filename = malloc(strlen(rfname) + sizeof("../../sdl/win32-msvc70-ffmpeg/.vcproj"));
                if (version == 70) {
                    sprintf(filename, "../../sdl/win32-msvc70-ffmpeg/%s.vcproj", rfname);
                } else {
                    sprintf(filename, "../../sdl/win32-msvc71-ffmpeg/%s.vcproj", rfname);
                }
            } else {
                filename = malloc(strlen(rfname) + sizeof("../../sdl/win32-msvc70/.vcproj"));
                if (version == 70) {
                    sprintf(filename, "../../sdl/win32-msvc70/%s.vcproj", rfname);
                } else {
                    sprintf(filename, "../../sdl/win32-msvc71/%s.vcproj", rfname);
                }
            }
        } else {
            if (ffmpeg) {
                filename = malloc(strlen(rfname) + sizeof("../vs70-ffmpeg/.vcproj"));
                if (version == 70) {
                    sprintf(filename, "../vs70-ffmpeg/%s.vcproj", rfname);
                } else {
                    sprintf(filename, "../vs71-ffmpeg/%s.vcproj", rfname);
                }
            } else {
                filename = malloc(strlen(rfname) + sizeof("../vs70/.vcproj"));
                if (version == 70) {
                    sprintf(filename, "../vs70/%s.vcproj", rfname);
                } else {
                    sprintf(filename, "../vs71/%s.vcproj", rfname);
                }
            }
        }
    } else {
        filename = malloc(strlen(fname) + sizeof(".vcproj"));
        sprintf(filename, "%s.vcproj", rfname);
    }

    outfile = fopen(filename, "wb");
    if (!outfile) {
        printf("Cannot open %s for output\n", filename);
        retval = 1;
    }

    if (!retval) {
        if (version == 70) {
            fprintf(outfile, msvc70_xml_header);
        } else {
            fprintf(outfile, msvc71_xml_header);
        }
        fprintf(outfile, msvc70_project_start, (version == 70) ? "7.00" : "7.10", cp_name);
        fprintf(outfile, msvc70_configurations);
        for (i = 0; i < max_i; i++) {
            fprintf(outfile, msvc70_config_part1, type_name[i]);
            if (cp_type == CP_TYPE_LIBRARY) {
                fprintf(outfile, msvc70_config_part2_lib, cp_name, type[i]);
            } else {
                fprintf(outfile, msvc70_config_part2_app);
            }
            fprintf(outfile, msvc70_config_part3, cp_name, type[i], (cp_type == CP_TYPE_LIBRARY) ? 4 : 1);
            if (cp_type == CP_TYPE_CONSOLE) {
                fprintf(outfile, msvc70_charset);
            }
            fprintf(outfile, msvc70_compiler_tool_part1, msvc70_compiler_tool_type[i & 1]);
            if (cp_cc_source_path && !cp_source_names[0]) {
                if (sdl) {
                    fprintf(outfile, msvc70_aid_cc_sdl);
                } else {
                    fprintf(outfile, msvc70_aid_cc);
                }
            } else {
                if (sdl) {
                    fprintf(outfile, msvc70_aid_sdl);
                } else {
                    fprintf(outfile, msvc70_aid);
                }
            }
            if (cp_include_dirs[0]) {
                for (j = 0; cp_include_dirs[j]; j++) {
                    fprintf(outfile, ",..\\..\\..\\%s", cp_include_dirs[j]);
                }
            }
            fprintf(outfile, "\"\r\n");
            fprintf(outfile, msvc70_compiler_tool_part2);
            fprintf(outfile, msvc_predefs[i]);
            if (cp_cc_source_path) {
                fprintf(outfile, msvc70_cc_predefs, cp_name);
            }
            fprintf(outfile, msvc70_predefs_end);
            if (i & 1) {
                fprintf(outfile, msvc70_string_pooling);
            }
            fprintf(outfile, msvc70_compiler_tool_part3, !(i & 1) + (sdl * 2));
            if (i & 1) {
                fprintf(outfile, msvc70_fll);
            }
            fprintf(outfile, msvc70_compiler_tool_part4, cp_name, type[i], cp_name, cp_name, type[i], cp_name, type[i], cp_name, type[i]);
            if (!(i & 1)) {
                fprintf(outfile, msvc70_dif);
            }
            fprintf(outfile, msvc70_compiler_tool_part5);
            if (cp_post_custom_command) {
                fprintf(outfile, msvc70_cbt_custom, cp_post_custom_command, cp_post_custom_output);
            } else {
                fprintf(outfile, msvc70_cbt_normal);
            }
            if (cp_type == CP_TYPE_LIBRARY) {
                fprintf(outfile, msvc70_lib_tool, cp_name, type[i], cp_name);
            } else {
                if (sdl) {
                    if (cp_type == CP_TYPE_CONSOLE) {
                        libs = msvc70_console_libs_sdl;
                    } else {
                        libs = msvc70_gui_libs_sdl;
                    }
                } else {
                    if (cp_type == CP_TYPE_CONSOLE) {
                        libs = msvc70_console_libs;
                    } else {
                        libs = msvc70_gui_libs[i >> 1];
                    }
                }
                fprintf(outfile, msvc70_linker_tool_part1, libs, cp_name, (!(i & 1)) + 1);
                if (!(i & 1)) {
                    fprintf(outfile, msvc70_gdi);
                }
                fprintf(outfile, msvc70_linker_tool_part2, cp_name, (cp_type == CP_TYPE_GUI) ? 2 : 1);
            }
            switch (cp_type) {
                default:
                case CP_TYPE_CONSOLE:
                    fprintf(outfile, msvc70_vcmidl_tool_console, cp_name);
                    break;
                case CP_TYPE_GUI:
                    fprintf(outfile, msvc70_vcmidl_tool_gui, msvc_predefs[i & 1], cp_name);
                    break;
                case CP_TYPE_LIBRARY:
                    fprintf(outfile, msvc70_vcmidl_tool_lib);
                    break;
            }
            fprintf(outfile, msvc70_link_event_tool);
            switch (cp_type) {
                default:
                case CP_TYPE_LIBRARY:
                    fprintf(outfile, msvc70_resource_tool_lib);
                    break;
                case CP_TYPE_CONSOLE:
                    fprintf(outfile, msvc70_resource_tool_console, msvc_predefs[i & 1]);
                    break;
                case CP_TYPE_GUI:
                    fprintf(outfile, msvc70_resource_tool_gui, msvc_predefs[i]);
                    break;
            }
            fprintf(outfile, msvc70_wspgt);
            if (version == 71) {
                fprintf(outfile, msvc71_xmldgt);
            }
            if (cp_type != CP_TYPE_LIBRARY) {
                fprintf(outfile, msvc70_wdt);
            }
            if (version == 71) {
                fprintf(outfile, msvc71_wrapper_tools);
            }
            fprintf(outfile, msvc70_config_end);
        }
        if (version == 70) {
            fprintf(outfile, msvc70_configs_files);
        } else {
            fprintf(outfile, msvc71_configs_files);
        }
        if (cp_source_names[0]) {
            for (i = 0; cp_source_names[i]; i++) {
                fprintf(outfile, msvc70_file, cp_source_names[i]);
            }
        }
        if (cp_sdl_source_names[0] && sdl) {
            for (i = 0; cp_sdl_source_names[i]; i++) {
                fprintf(outfile, msvc70_file, cp_sdl_source_names[i]);
            }
        }
        if (cp_native_source_names[0] && !sdl) {
            for (i = 0; cp_native_source_names[i]; i++) {
                fprintf(outfile, msvc70_file, cp_native_source_names[i]);
            }
        }
        if (cp_custom_message) {
            fprintf(outfile, msvc70_file_start, cp_custom_source);
            for (i = 0; i < max_i; i++) {
                fprintf(outfile, msvc70_custom_fc, type_name[i], cp_custom_message, cp_custom_command);
                for (j = 0; cp_custom_deps[j]; j++) {
                    fprintf(outfile, "%s;", cp_custom_deps[j]);
                }
                fprintf(outfile, msvc70_custom_output, cp_custom_output);
            }
            fprintf(outfile, msvc70_end_file);
        }

        if (cp_cpusource_names[0]) {
            for (j = 0; cp_cpusource_names[j]; j++) {
                if (sdl) {
                    fprintf(outfile, msvc70_cpu_file_sdl, cp_cpusource_names[j]);
                } else {
                    fprintf(outfile, msvc70_cpu_file, cp_cpusource_names[j]);
                }
            }
        }

        if (cp_cc_source_path) {
            for (j = 0; cp_cc_source_names[j]; j++) {
                fprintf(outfile, msvc70_cc_file_start, cp_cc_source_path, cp_cc_source_names[j]);
                for (i = 0; i < max_i; i++) {
                    fprintf(outfile, msvc70_cc_fc, type_name[i]);
                    if (cp_source_names[0]) {
                        if (sdl) {
                            fprintf(outfile, msvc70_cc_command_sid_sdl, msvc_flags_sdl[i & 1], msvc_cc_extra[i], cp_name, cp_name, type[i], cp_name, cp_name, type[i], cp_name, type[i], cp_name, type[i]);
                        } else {
                            fprintf(outfile, msvc70_cc_command_sid, msvc_flags[i & 1], msvc_cc_extra[i], cp_name, cp_name, type[i], cp_name, cp_name, type[i], cp_name, type[i], cp_name, type[i]);
                        }
                    } else {
                        if (sdl) {
                            fprintf(outfile, msvc70_cc_command_sdl, msvc_flags_sdl[i & 1], msvc_cc_extra[i], cp_name, cp_name, type[i], cp_name, cp_name, type[i], cp_name, type[i], cp_name, type[i]);
                        } else {
                            fprintf(outfile, msvc70_cc_command, msvc_flags[i & 1], msvc_cc_extra[i], cp_name, cp_name, type[i], cp_name, cp_name, type[i], cp_name, type[i], cp_name, type[i]);
                        }
                    }
                }
                fprintf(outfile, msvc70_end_file);
            }
        }

        if (cp_res_source_name && !sdl) {
            fprintf(outfile, msvc70_res_file, cp_res_source_name);
            for (i = 0; i < max_i; i++) {
                fprintf(outfile, msvc70_res_fc, type_name[i]);
                for (j = 0; cp_res_deps[j]; j++) {
                    fprintf(outfile, "..\\%s ", cp_res_deps[j]);
                    if (cp_res_deps[j + 1]) {
                        fprintf(outfile, "+ ");
                    }
                }
                fprintf(outfile, "%s /b\r\n\"\r\n", cp_res_output_name);
                fprintf(outfile, msvc70_res_deps);
                for (j = 0; cp_res_deps[j]; j++) {
                    fprintf(outfile, "..\\%s;", cp_res_deps[j]);
                }
                fprintf(outfile, msvc70_res_output, cp_res_output_name);
            }
            fprintf(outfile, msvc70_res_end, cp_res_output_name);
        }
        fprintf(outfile, msvc70_end_project);
    }

    if (outfile) {
        fclose(outfile);
    }

    if (filename) {
        free(filename);
    }

    if (cp_libs) {
        free(cp_libs);
        cp_libs = NULL;
    }

    if (read_buffer) {
        free(read_buffer);
        read_buffer = NULL;
        read_buffer_line = 0;
        read_buffer_pos = 0;
        read_buffer_len = 0;
    }

    return retval;
}

/* ---------------------------------------------------------------------- */

/* MSVC 6 types */
static char *msvc6_console_type = "Console Application";
static char *msvc6_library_type = "Static Library";
static char *msvc6_gui_type =     "Application";

/* MSVC 6 ids */
static char *msvc6_console_id = "0x0103";
static char *msvc6_library_id = "0x0104";
static char *msvc6_gui_id =     "0x0101";

/* MSVC6 header */
static char *msvc6_header1 = "# Microsoft Developer Studio Project File - Name=\"%s\" - Package Owner=<4>\r\n"
                           "# Microsoft Developer Studio Generated Build File, Format Version 6.00\r\n"
                           "# ** DO NOT EDIT **\r\n"
                           "\r\n"
                           "# TARGTYPE \"Win32 (x86) %s\" %s\r\n\r\n"
                           "CFG=%s - Win32 Debug\r\n"
                           "!MESSAGE This is not a valid makefile. To build this project using NMAKE,\r\n"
                           "!MESSAGE use the Export Makefile command and run\r\n"
                           "!MESSAGE \r\n"
                           "!MESSAGE NMAKE /f \"%s.mak\".\r\n"
                           "!MESSAGE \r\n"
                           "!MESSAGE You can specify a configuration when running NMAKE\r\n"
                           "!MESSAGE by defining the macro CFG on the command line. For example:\r\n"
                           "!MESSAGE \r\n"
                           "!MESSAGE NMAKE /f \"%s.mak\" CFG=\"%s - Win32 Debug\"\r\n"
                           "!MESSAGE \r\n"
                           "!MESSAGE Possible choices for configuration are:\r\n"
                           "!MESSAGE \r\n"
                           "!MESSAGE \"%s - Win32 Release\" (based on \"Win32 (x86) %s\")\r\n"
                           "!MESSAGE \"%s - Win32 Debug\" (based on \"Win32 (x86) %s\")\r\n";

static char *msvc6_header_native = "!MESSAGE \"%s - Win32 DX Release\" (based on \"Win32 (x86) %s\")\r\n"
                                   "!MESSAGE \"%s - Win32 DX Debug\" (based on \"Win32 (x86) %s\")\r\n";

static char *msvc6_header2 = "!MESSAGE \r\n"
                             "\r\n";

/* MSVC6 begin project section */
static char *msvc6_section1 = "# Begin Project\r\n"
                              "# PROP AllowPerConfigDependencies 0\r\n"
                              "# PROP Scc_ProjName \"\"\r\n"
                              "# PROP Scc_LocalPath \"\"\r\n"
                              "CPP=cl.exe\r\n"
                              "%s"
                              "RSC=rc.exe\r\n"
                              "\r\n";

static char *msvc6_begin_ifs_native[4] = {
    "!IF  \"$(CFG)\" == \"%s - Win32 Release\"\r\n\r\n",
    "!ELSEIF  \"$(CFG)\" == \"%s - Win32 Debug\"\r\n\r\n",
    "!ELSEIF  \"$(CFG)\" == \"%s - Win32 DX Release\"\r\n\r\n",
    "!ELSEIF  \"$(CFG)\" == \"%s - Win32 DX Debug\"\r\n\r\n"
};

static char *msvc6_begin_ifs_sdl[2] = {
    "!IF  \"$(CFG)\" == \"%s - Win32 Release\"\r\n\r\n",
    "!ELSEIF  \"$(CFG)\" == \"%s - Win32 Debug\"\r\n\r\n"
};

static char *msvc6_releases[4] = {
    "Release",
    "Debug",
    "DXRelease",
    "DXDebug"
};

static int msvc6_enable_debug[4] = { 0, 1, 0, 1 };

/* begin sections */
static char *msvc6_begin_section = "# PROP BASE Use_MFC 0\r\n"
                                   "# PROP BASE Use_Debug_Libraries %d\r\n"
                                   "# PROP BASE Output_Dir \"%s\"\r\n"
                                   "# PROP BASE Intermediate_Dir \"%s\"\r\n"
                                   "# PROP BASE Target_Dir \"\"\r\n"
                                   "# PROP Use_MFC 0\r\n"
                                   "# PROP Use_Debug_Libraries %d\r\n";

static char *msvc6_middle_section_lib = "# PROP Output_Dir \"libs\\%s\\%s\"\r\n";
static char *msvc6_middle_section_app = "# PROP Output_Dir \"..\\..\\..\\..\\data\"\r\n";

static char *msvc6_section2 = "# PROP Intermediate_Dir \"libs\\%s\\%s\"\r\n";

static char *msvc6_app_section1 = "# PROP Ignore_Export_Lib 0\r\n";

static char *msvc6_section3 = "# PROP Target_Dir \"\"\r\n";

static char *msvc6_base_cpp_lib_gui_part1_native[4] = {
    "/MT /W3 /GX /O2",
    "/MTd /W3 /GX /Z7 /Od",
    "/MT /W3 /GX /O2",
    "/MTd /W3 /GX /Z7 /Od"
};

static char *msvc6_base_cpp_lib_gui_part1_sdl[2] = {
    "/MD /W3 /GX /O2",
    "/MDd /W3 /GX /Z7 /Od",
};

static char *msvc6_base_cpp_lib_gui_part2_native_noffmpeg[4] = {
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"NODIRECTX\" /D \"NDEBUG\" /YX /FD /c",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"NODIRECTX\" /D \"_DEBUG\" /YX /FD /c",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\"  /D \"NDEBUG\" /YX /FD /c",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"_DEBUG\" /YX /FD /c"
};

static char *msvc6_base_cpp_lib_gui_part2_native_ffmpeg[4] = {
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"STATIC_FFMPEG\" /D \"DONT_USE_UNISTD_H\" /D \"NODIRECTX\" /D \"NDEBUG\" /YX /FD /c",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"STATIC_FFMPEG\" /D \"DONT_USE_UNISTD_H\" /D \"NODIRECTX\" /D \"_DEBUG\" /YX /FD /c",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"STATIC_FFMPEG\" /D \"DONT_USE_UNISTD_H\"  /D \"NDEBUG\" /YX /FD /c",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"STATIC_FFMPEG\" /D \"DONT_USE_UNISTD_H\" /D \"_DEBUG\" /YX /FD /c"
};

static char *msvc6_base_cpp_lib_gui_part2_sdl[2] = {
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\"  /D \"NDEBUG\" /YX /FD /c",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"_DEBUG\" /YX /FD /c"
};

static char *msvc6_base_cpp_cc_native_noffmpeg[4] = {
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"NODIRECTX\" /D \"NDEBUG\" /D PACKAGE=\\\"%s\\\" /D VERSION=\\\"0.7\\\" /D SIZEOF_INT=4",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"NODIRECTX\" /D \"_DEBUG\" /D PACKAGE=\\\"%s\\\" /D VERSION=\\\"0.7\\\" /D SIZEOF_INT=4",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\"  /D \"NDEBUG\" /D PACKAGE=\\\"%s\\\" /D VERSION=\\\"0.7\\\" /D SIZEOF_INT=4",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"_DEBUG\" /D PACKAGE=\\\"%s\\\" /D VERSION=\\\"0.7\\\" /D SIZEOF_INT=4"
};

static char *msvc6_base_cpp_cc_native_ffmpeg[4] = {
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"STATIC_FFMPEG\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"NODIRECTX\" /D \"NDEBUG\" /D PACKAGE=\\\"%s\\\" /D VERSION=\\\"0.7\\\" /D SIZEOF_INT=4",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"STATIC_FFMPEG\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"NODIRECTX\" /D \"_DEBUG\" /D PACKAGE=\\\"%s\\\" /D VERSION=\\\"0.7\\\" /D SIZEOF_INT=4",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"STATIC_FFMPEG\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\"  /D \"NDEBUG\" /D PACKAGE=\\\"%s\\\" /D VERSION=\\\"0.7\\\" /D SIZEOF_INT=4",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"STATIC_FFMPEG\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"_DEBUG\" /D PACKAGE=\\\"%s\\\" /D VERSION=\\\"0.7\\\" /D SIZEOF_INT=4"
};

static char *msvc6_base_cpp_cc_sdl[2] = {
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\"  /D \"NDEBUG\" /D PACKAGE=\\\"%s\\\" /D VERSION=\\\"0.7\\\" /D SIZEOF_INT=4",
    " /D \"WIN32\" /D \"_WINDOWS\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"_DEBUG\" /D PACKAGE=\\\"%s\\\" /D VERSION=\\\"0.7\\\" /D SIZEOF_INT=4"
};

static char *msvc6_base_cpp_cc_end = " /YX /FD /c\r\n";

static char *msvc6_base_cpp_console_part1_native[4] = {
    "/MT /W3 /GX /O2",
    "/MTd /W3 /Gm /GX /Zi /Od",
    "/MT /W3 /GX /O2",
    "/MTd /W3 /GX /Z7 /Od"
};

static char *msvc6_base_cpp_console_part1_sdl[2] = {
    "/MD /W3 /GX /O2",
    "/MDd /W3 /GX /Z7 /Od"
};

static char *msvc6_base_cpp_console_part2[4] = {
    " /D \"WIN32\" /D \"_CONSOLE\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"_MBCS\" /D \"NDEBUG\" /YX /FD /c",
    " /D \"WIN32\" /D \"_CONSOLE\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H \"/D \"_MBCS\" /D \"_DEBUG\" /YX /FD /c",
    " /D \"WIN32\" /D \"_CONSOLE\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"_MBCS\" /D \"NDEBUG\" /YX /FD /c",
    " /D \"WIN32\" /D \"_CONSOLE\" /D \"IDE_COMPILE\" /D \"DONT_USE_UNISTD_H\" /D \"_MBCS\" /D \"_DEBUG\" /YX /FD /c"
};

static char *msvc6_base_mtl[2] = {
    "# ADD BASE MTL /nologo /D \"NDEBUG\" /mktyplib203 /o \"NUL\" /win32\r\n"
    "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /o \"NUL\" /win32\r\n",
    "# ADD BASE MTL /nologo /D \"_DEBUG\" /mktyplib203 /o \"NUL\" /win32\r\n"
    "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /o \"NUL\" /win32\r\n"
};

static char *msvc6_base_rsc_native_noffmpeg[4] = {
    "# ADD BASE RSC /l 0x409 /d \"NDEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\" /d \"NODIRECTX\"\r\n"
    "# ADD RSC /l 0x409 /i \"..\\msvc\" /i \"..\\\\\" /i \"..\\..\\..\\\\\" /d \"NDEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\" /d \"NODIRECTX\"\r\n",
    "# ADD BASE RSC /l 0x409 /d \"_DEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\" /d \"NODIRECTX\"\r\n"
    "# ADD RSC /l 0x409 /i \"..\\msvc\" /i \"..\\\\\" /i \"..\\..\\..\\\\\" /d \"_DEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\" /d \"NODIRECTX\"\r\n",
    "# ADD BASE RSC /l 0x409 /d \"NDEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\"\r\n"
    "# ADD RSC /l 0x409 /i \"..\\msvc\" /i \"..\\\\\" /i \"..\\..\\..\\\\\" /d \"NDEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\"\r\n",
    "# ADD BASE RSC /l 0x409 /d \"_DEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\"\r\n"
    "# ADD RSC /l 0x409 /i \"..\\msvc\" /i \"..\\\\\" /i \"..\\..\\..\\\\\" /d \"_DEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\"\r\n"
};

static char *msvc6_base_rsc_native_ffmpeg[4] = {
    "# ADD BASE RSC /l 0x409 /d \"NDEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\" /d \"STATIC_FFMPEG\" /d \"NODIRECTX\"\r\n"
    "# ADD RSC /l 0x409 /i \"..\\msvc\" /i \"..\\\\\" /i \"..\\..\\..\\\\\" /d \"NDEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\" /d \"STATIC_FFMPEG\" /d \"NODIRECTX\"\r\n",
    "# ADD BASE RSC /l 0x409 /d \"_DEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\" /d \"NODIRECTX\"\r\n"
    "# ADD RSC /l 0x409 /i \"..\\msvc\" /i \"..\\\\\" /i \"..\\..\\..\\\\\" /d \"_DEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\" /d \"STATIC_FFMPEG\" /d \"NODIRECTX\"\r\n",
    "# ADD BASE RSC /l 0x409 /d \"NDEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\"\r\n"
    "# ADD RSC /l 0x409 /i \"..\\msvc\" /i \"..\\\\\" /i \"..\\..\\..\\\\\" /d \"NDEBUG\" /d \"WIN32\" /d \"STATIC_FFMPEG\" /d \"IDE_COMPILE\"\r\n",
    "# ADD BASE RSC /l 0x409 /d \"_DEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\"\r\n"
    "# ADD RSC /l 0x409 /i \"..\\msvc\" /i \"..\\\\\" /i \"..\\..\\..\\\\\" /d \"_DEBUG\" /d \"WIN32\" /d \"STATIC_FFMPEG\" /d \"IDE_COMPILE\"\r\n"
};

static char *msvc6_base_rsc_sdl[2] = {
    "# ADD BASE RSC /l 0x409 /d \"NDEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\"\r\n"
    "# ADD RSC /l 0x409 /i \"..\\msvc\" /i \"..\\\\\" /i \"..\\..\\..\\\\\" /d \"NDEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\"\r\n",
    "# ADD BASE RSC /l 0x409 /d \"_DEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\"\r\n"
    "# ADD RSC /l 0x409 /i \"..\\msvc\" /i \"..\\\\\" /i \"..\\..\\..\\\\\" /d \"_DEBUG\" /d \"WIN32\" /d \"IDE_COMPILE\"\r\n"
};

static char *msvc6_bsc32 = "BSC32=bscmake.exe\r\n"
                           "# ADD BASE BSC32 /nologo\r\n"
                           "# ADD BSC32 /nologo\r\n";

static char *msvc6_lib32 = "LIB32=link.exe -lib\r\n"
                           "# ADD BASE LIB32 /nologo\r\n"
                           "# ADD LIB32 /nologo\r\n\r\n";

static char *msvc6_link32_console = "LINK32=link.exe\r\n"
                                    "# ADD BASE LINK32 %s /nologo /subsystem:console /machine:I386\r\n"
                                    "# ADD LINK32 %s /nologo /subsystem:console /machine:I386 /IGNORE:4089 \r\n\r\n";

static char *msvc6_link32_console_sdl_debug = "LINK32=link.exe\r\n"
                                              "# ADD BASE LINK32 %s %s /nologo /subsystem:console /machine:I386 /NODEFAULTLIB:msvcrt.lib\r\n"
                                              "# ADD LINK32 %s %s /nologo /subsystem:console /machine:I386 /NODEFAULTLIB:msvcrt.lib /IGNORE:4089\r\n\r\n";

static char *msvc6_link32_console_sdl_release = "LINK32=link.exe\r\n"
                                                "# ADD BASE LINK32 %s %s /nologo /subsystem:console /machine:I386\r\n"
                                                "# ADD LINK32 %s %s /nologo /subsystem:console /machine:I386 /IGNORE:4089\r\n\r\n";

static char *msvc6_link32_gui = "LINK32=link.exe\r\n"
                                "# ADD BASE LINK32 %s%s /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept\r\n"
                                "# ADD LINK32 %s%s /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept\r\n\r\n";

static char *msvc6_link32_gui_sdl_debug = "LINK32=link.exe\r\n"
                                          "# ADD BASE LINK32 %s%s /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /NODEFAULTLIB:msvcrt.lib\r\n"
                                          "# ADD LINK32 %s%s /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /NODEFAULTLIB:msvcrt.lib\r\n\r\n";

static char *msvc6_dx_libs[2] = {
    "",
    " dsound.lib dxguid.lib"
};

static char *msvc6_sdl_libs = " SDLmain.lib SDL.lib opengl32.lib";

static char *msvc6_endif = "!ENDIF\r\n"
                           "\r\n";

static char *msvc6_begin_target = "# Begin Target\r\n"
                                  "\r\n"
                                  "# Name \"%s - Win32 Release\"\r\n"
                                  "# Name \"%s - Win32 Debug\"\r\n";

static char *msvc6_dx_targets = "# Name \"%s - Win32 DX Release\"\r\n"
                                "# Name \"%s - Win32 DX Debug\"\r\n";

static char *msvc6_source = "# Begin Source File\r\n"
                            "\r\n"
                            "SOURCE=\"..\\..\\..\\%s\"\r\n"
                            "# End Source File\r\n";

static char *msvc6_end_target = "# End Target\r\n"
                                "# End Project\r\n";

static char *msvc6_custom_source = "# Begin Source File\r\n"
                                   "\r\n"
                                   "SOURCE=\"%s\"\r\n"
                                   "\r\n";

static char *msvc6_custom_section_part1 = "# PROP Ignore_Default_Tool 1\r\n"
                                          "USERDEP__CUSTOM=";

static char *msvc6_custom_section_part2 = "\r\n"
                                    "# Begin Custom Build - %s\r\n"
                                    "InputDir=.\r\n"
                                    "InputPath=\"%s\"\r\n"
                                    "\r\n"
                                    "\"$(InputDir)\\%s\" : $(SOURCE) \"$(INTDIR)\" \"$(OUTDIR)\"\r\n"
                                    "\t%s\r\n"
                                    "\r\n"
                                    "# End Custom Build\r\n"
                                    "\r\n";

static char *msvc6_post_custom_section = "# Begin Custom Build - %s\r\n"
                                         "InputPath=\"%s\"\r\n"
                                         "SOURCE=\"$(InputPath)\"\r\n"
                                         "\r\n"
                                         "\"%s\" : $(SOURCE) \"$(INTDIR)\" \"$(OUTDIR)\"\r\n"
                                         "\t%s\r\n"
                                         "\r\n"
                                         "# End Custom Build\r\n\r\n";

static char *msvc6_end_custom_source = "# End Source File\r\n";

static char *msvc6_custom_cpu_source = "# Begin Source File\r\n"
                                       "\r\n"
                                       "SOURCE=\"..\\..\\..\\%s\"\r\n"
                                       "\r\n"
                                       "!IF  \"$(CFG)\" == \"%s - Win32 Release\"\r\n"
                                       "\r\n"
                                       "# ADD CPP /Ot /Oa /Ow /Oi /Op /Oy\r\n"
                                       "# SUBTRACT CPP /Os\r\n"
                                       "\r\n"
                                       "!ELSEIF  \"$(CFG)\" == \"%s - Win32 Debug\"\r\n"
                                       "\r\n";

static char *msvc6_custom_cpu_native = "!ELSEIF  \"$(CFG)\" == \"%s - Win32 DX Release\"\r\n"
                                       "\r\n"
                                       "# ADD BASE CPP /Ot /Oa /Ow /Oi /Op /Oy\r\n"
                                       "# SUBTRACT BASE CPP /Os\r\n"
                                       "# ADD CPP /Ot /Oa /Ow /Oi /Op /Oy\r\n"
                                       "# SUBTRACT CPP /Os\r\n"
                                       "\r\n"
                                       "!ELSEIF  \"$(CFG)\" == \"%s - Win32 DX Debug\"\r\n"
                                       "\r\n";

static char *msvc6_custom_cpu_end = "!ENDIF\r\n"
                                    "\r\n"
                                    "# End Source File\r\n";

static char *msvc6_begin_cc_source = "# Begin Source File\r\n"
                                     "\r\n"
                                     "SOURCE=\"..\\..\\..\\%s\\%s.cc\"\r\n"
                                     "\r\n";

static char *msvc6_cc_custom_build_part1 = "# Begin Custom Build\r\n"
                                           "InputPath=\"..\\..\\..\\%s\\%s.cc\"\r\n"
                                           "InputName=%s\r\n"
                                           "\r\n"
                                           "\"libs\\%s\\%s\\$(InputName).obj\" : $(SOURCE) \"$(INTDIR)\" \"$(OUTDIR)\"\r\n";

static char *msvc6_cc_custom_build_part2a_native = "\tcl /nologo %s /EHsc /I \"..\\msvc\" ";

static char *msvc6_cc_custom_build_part2a_sdl = "\tcl /nologo %s /EHsc /I \".\\\\\" /I \"..\\\\\" ";

static char *msvc6_cc_custom_build_part2b_native = "\tcl /nologo %s /EHsc /I \"..\\msvc\" /I \"..\\\\\" /I \"..\\..\\..\\\\\" ";

static char *msvc6_cc_custom_build_part2b_sdl = "\tcl /nologo %s /EHsc /I \".\\\\\" /I \"..\\\\\" /I \"..\\..\\..\\\\\" ";

static char *msvc6_cc_custom_build_part3 = "%s /Fp\"libs\\%s\\%s\\%s.pch\" /Fo\"libs\\%s\\%s\\\\\" /Fd\"libs\\%s\\%s\\\\\" /FD /TP /c \"$(InputPath)\"\r\n"
                                           "\r\n"
                                           "# End Custom Build\r\n"
                                           "\r\n";

static char *msvc6_res_source_start = "# Begin Source File\r\n"
                                      "\r\n"
                                      "SOURCE=\"..\\%s\"\r\n"
                                      "\r\n";

static char *msvc6_res_source_part1 = "# PROP Ignore_Default_Tool 1\r\n"
                                      "USERDEP__RESC6=\"..\\..\\..\\debug.h\"";

static char *msvc6_res_source_part2 = "\r\n"
                                      "# Begin Custom Build\r\n"
                                      "InputPath=\"..\\%s\"\r\n"
                                      "\r\n"
                                      "\"%s\" : $(SOURCE) \"$(INTDIR)\" \"$(OUTDIR)\"\r\n"
                                      "\tcopy /b";

static char *msvc6_res_source_part3 = "!ENDIF\r\n"
                                      "\r\n"
                                      "# End Source File\r\n"
                                      "# Begin Source File\r\n"
                                      "\r\n"
                                      "SOURCE=\".\\%s\"\r\n"
                                      "# End Source File\r\n"
                                      "# Begin Source File\r\n"
                                      "\r\n"
                                      "SOURCE=\"..\\vice.manifest\"\r\n"
                                      "# End Source File\r\n";

static int open_msvc6_main_project(int sdl)
{
    if (sdl) {
        if (ffmpeg) {
            mainfile = fopen("../../sdl/win32-msvc6-ffmpeg/vice.dsw", "wb");
        } else {
            mainfile = fopen("../../sdl/win32-msvc6/vice.dsw", "wb");
        }
    } else {
        if (ffmpeg) {
            mainfile = fopen("../vs6-ffmpeg/vice.dsw", "wb");
        } else {
            mainfile = fopen("../vs6/vice.dsw", "wb");
        }
    }

    if (!mainfile) {
        printf("Cannot open 'vice.dsw' for output\n");
        return 1;
    }
    fprintf(mainfile, "Microsoft Developer Studio Workspace File, Format Version 6.00\r\n");
    fprintf(mainfile, "# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!\r\n\r\n");
    fprintf(mainfile, "###############################################################################\r\n\r\n");
    return 0;
}

static void close_msvc6_main_project(int sdl)
{
    fprintf(mainfile, "Global:\r\n\r\n");
    fprintf(mainfile, "Package=<5>\r\n{{{\r\n}}}\r\n\r\n");
    fprintf(mainfile, "Package=<3>\r\n{{{\r\n}}}\r\n\r\n");
    fprintf(mainfile, "###############################################################################\r\n\r\n");
    fclose(mainfile);
}

static int output_msvc6_file(char *fname, int filelist, int sdl)
{
    char *filename;
    int retval = 0;
    FILE *outfile = NULL;
    char *real_type = NULL;
    char *real_id = NULL;
    char *mtl = (cp_type == CP_TYPE_GUI) ? "MTL=midl.exe\r\n" : "";
    int i, j;
    int max_i = (sdl) ? 2 : 4;
    char *rfname;
    char **msvc6_base_cpp_lib_gui_part2_native = (ffmpeg) ? msvc6_base_cpp_lib_gui_part2_native_ffmpeg : msvc6_base_cpp_lib_gui_part2_native_noffmpeg;
    char **msvc6_base_cpp_cc_native = (ffmpeg) ? msvc6_base_cpp_cc_native_ffmpeg : msvc6_base_cpp_cc_native_noffmpeg;
    char **msvc6_base_rsc_native = (ffmpeg) ? msvc6_base_rsc_native_ffmpeg : msvc6_base_rsc_native_noffmpeg;

    if (!strcmp(fname, "arch_native") || !strcmp(fname, "arch_sdl")) {
        rfname = "arch";
    } else {
        rfname = fname;
    }

    if (filelist) {
        if (sdl) {
            if (ffmpeg) {
                filename = malloc(strlen(rfname) + sizeof("../../sdl/win32-msvc6-ffmpeg/.dsp"));
                sprintf(filename, "../../sdl/win32-msvc6-ffmpeg/%s.dsp", rfname);
            } else {
                filename = malloc(strlen(rfname) + sizeof("../../sdl/win32-msvc6/.dsp"));
                sprintf(filename, "../../sdl/win32-msvc6/%s.dsp", rfname);
            }
        } else {
            if (ffmpeg) {
                filename = malloc(strlen(rfname) + sizeof("../vs6-ffmpeg/.dsp"));
                sprintf(filename, "../vs6-ffmpeg/%s.dsp", rfname);
            } else {
                filename = malloc(strlen(rfname) + sizeof("../vs6/.dsp"));
                sprintf(filename, "../vs6/%s.dsp", rfname);
            }
        }
    } else {
        filename = malloc(strlen(rfname) + sizeof(".dsp"));
        sprintf(filename, "%s.dsp", rfname);
    }

    outfile = fopen(filename, "wb");
    if (!outfile) {
        printf("Cannot open %s for output\n", filename);
        retval = 1;
    }

    if (!retval && filelist) {
        fprintf(mainfile, "Project: \"%s\"=\".\\%s.dsp\" - Package Owner=<4>\r\n\r\n", cp_name, cp_name);
        fprintf(mainfile, "Package=<5>\r\n{{{\r\n}}}\r\n\r\nPackage=<4>\r\n{{{\r\n");
        if (cp_dep_names[0]) {
            for (i = 0; cp_dep_names[i]; i++) {
                fprintf(mainfile, "    Begin Project Dependency\r\n");
                fprintf(mainfile, "    Project_Dep_Name %s\r\n", cp_dep_names[i]);
                fprintf(mainfile, "    End Project Dependency\r\n");
            }
        }
        fprintf(mainfile, "}}}\r\n\r\n###############################################################################\r\n\r\n");
    }

    if (!retval) {
        switch(cp_type) {
            default:
            case CP_TYPE_LIBRARY:
                real_type = msvc6_library_type;
                real_id = msvc6_library_id;
                break;
            case CP_TYPE_CONSOLE:
                real_type = msvc6_console_type;
                real_id = msvc6_console_id;
                break;
            case CP_TYPE_GUI:
                real_type = msvc6_gui_type;
                real_id = msvc6_gui_id;
                break;
        }
        fprintf(outfile, msvc6_header1, cp_name, real_type, real_id, cp_name, cp_name, cp_name, cp_name, cp_name, real_type, cp_name, real_type);
        if (!sdl) {
            fprintf(outfile, msvc6_header_native, cp_name, real_type, cp_name, real_type);
        }
        fprintf(outfile, msvc6_header2);
        fprintf(outfile, msvc6_section1, mtl, cp_name);
        for (i = 0; i < max_i; i++) {
            if (sdl) {
                fprintf(outfile, msvc6_begin_ifs_sdl[i], cp_name);
            } else {
                fprintf(outfile, msvc6_begin_ifs_native[i], cp_name);
            }
            fprintf(outfile, msvc6_begin_section, msvc6_enable_debug[i], msvc6_releases[i], msvc6_releases[i], msvc6_enable_debug[i]);
            if (cp_type != CP_TYPE_LIBRARY) {
                fprintf(outfile, msvc6_middle_section_app);
            } else {
                fprintf(outfile, msvc6_middle_section_lib, cp_name, msvc6_releases[i]);
            }
            fprintf(outfile, msvc6_section2, cp_name, msvc6_releases[i]);
            if (cp_type != CP_TYPE_LIBRARY) {
                fprintf(outfile, msvc6_app_section1);
            }
            fprintf(outfile, msvc6_section3);
            if (cp_type != CP_TYPE_CONSOLE) {
                if (!cp_source_names[0] && cp_cc_source_names[0]) {
                    if (sdl) {
                        fprintf(outfile, "# ADD BASE CPP /nologo %s ", msvc6_base_cpp_lib_gui_part1_sdl[i]);
                        fprintf(outfile, msvc6_base_cpp_cc_sdl[i], cp_name);
                    } else {
                        fprintf(outfile, "# ADD BASE CPP /nologo %s ", msvc6_base_cpp_lib_gui_part1_native[i]);
                        fprintf(outfile, msvc6_base_cpp_cc_native[i], cp_name);
                    }
                    fprintf(outfile, msvc6_base_cpp_cc_end);
                } else {
                    if (sdl) {
                        fprintf(outfile, "# ADD BASE CPP /nologo %s %s\r\n", msvc6_base_cpp_lib_gui_part1_sdl[i], msvc6_base_cpp_lib_gui_part2_sdl[i]);
                    } else {
                        fprintf(outfile, "# ADD BASE CPP /nologo %s %s\r\n", msvc6_base_cpp_lib_gui_part1_native[i], msvc6_base_cpp_lib_gui_part2_native[i]);
                    }
                }
                if (sdl) {
                    fprintf(outfile, "# ADD CPP /nologo %s", msvc6_base_cpp_lib_gui_part1_sdl[i]);
                } else {
                    fprintf(outfile, "# ADD CPP /nologo %s", msvc6_base_cpp_lib_gui_part1_native[i]);
                }
            } else {
                if (sdl) {
                    fprintf(outfile, "# ADD BASE CPP /nologo %s %s\r\n", msvc6_base_cpp_console_part1_sdl[i], msvc6_base_cpp_console_part2[i]);
                    fprintf(outfile, "# ADD CPP /nologo %s", msvc6_base_cpp_console_part1_sdl[i]);
                } else {
                    fprintf(outfile, "# ADD BASE CPP /nologo %s %s\r\n", msvc6_base_cpp_console_part1_native[i], msvc6_base_cpp_console_part2[i]);
                    fprintf(outfile, "# ADD CPP /nologo %s", msvc6_base_cpp_console_part1_native[i]);
                }
            }
            if (!cp_source_names[0] && cp_cc_source_names[0]) {
                if (sdl) {
                    fprintf(outfile, " /I \".\\\"");
                } else {
                    fprintf(outfile, " /I \"..\\msvc\"");
                }
            } else {
                if (sdl) {
                    fprintf(outfile, " /I \".\\\" /I \"..\\\\\" /I \"..\\..\\..\\\\\"");
                } else {
                    fprintf(outfile, " /I \"..\\msvc\" /I \"..\\\\\" /I \"..\\..\\..\\\\\"");
                }
                if (cp_include_dirs[0]) {
                    for (j = 0; cp_include_dirs[j]; j++) {
                        fprintf(outfile, " /I \"..\\..\\..\\%s\"", cp_include_dirs[j]);
                    }
                }
            }
            if (cp_type != CP_TYPE_CONSOLE) {
                if (!cp_source_names[0] && cp_cc_source_names[0]) {
                    if (sdl) {
                        fprintf(outfile, msvc6_base_cpp_cc_sdl[i], cp_name);
                    } else {
                        fprintf(outfile, msvc6_base_cpp_cc_native[i], cp_name);
                    }
                    fprintf(outfile, msvc6_base_cpp_cc_end);
                } else {
                    if (sdl) {
                        fprintf(outfile, " %s\r\n", msvc6_base_cpp_lib_gui_part2_sdl[i]);
                    } else {
                        fprintf(outfile, " %s\r\n", msvc6_base_cpp_lib_gui_part2_native[i]);
                    }
                }
            } else {
                fprintf(outfile, " %s\r\n", msvc6_base_cpp_console_part2[i]);
            }
            if (cp_type == CP_TYPE_GUI) {
                fprintf(outfile, msvc6_base_mtl[i & 1]);
            }
            if (sdl) {
                fprintf(outfile, msvc6_base_rsc_sdl[i]);
            } else {
                fprintf(outfile, msvc6_base_rsc_native[i]);
            }
            fprintf(outfile, msvc6_bsc32);
            switch (cp_type) {
                default:
                case CP_TYPE_LIBRARY:
                    fprintf(outfile, msvc6_lib32);
                    break;
                case CP_TYPE_CONSOLE:
                    if (sdl) {
                        if (i & 1) {
                            fprintf(outfile, msvc6_link32_console_sdl_debug, cp_libs, msvc6_sdl_libs, cp_libs, msvc6_sdl_libs);
                        } else {
                            fprintf(outfile, msvc6_link32_console_sdl_release, cp_libs, msvc6_sdl_libs, cp_libs, msvc6_sdl_libs);
                        }
                    } else {
                        fprintf(outfile, msvc6_link32_console, cp_libs, cp_libs);
                    }
                    break;
                case CP_TYPE_GUI:
                    if (sdl) {
                        if (i & 1) {
                            fprintf(outfile, msvc6_link32_gui_sdl_debug, cp_libs, msvc6_sdl_libs, cp_libs, msvc6_sdl_libs);
                        } else {
                            fprintf(outfile, msvc6_link32_gui, cp_libs, msvc6_sdl_libs, cp_libs, msvc6_sdl_libs);
                        }
                    } else {
                        fprintf(outfile, msvc6_link32_gui, cp_libs, msvc6_dx_libs[i >> 1], cp_libs, msvc6_dx_libs[i >> 1]);
                    }
                    break;
            }
            if (cp_post_custom_message) {
                fprintf(outfile, msvc6_post_custom_section, cp_post_custom_message, cp_post_custom_source, cp_post_custom_output, cp_post_custom_command);
            }
        }
        fprintf(outfile, msvc6_endif);
        fprintf(outfile, msvc6_begin_target, cp_name, cp_name);
        if (!sdl) {
            fprintf(outfile, msvc6_dx_targets, cp_name, cp_name);
        }
        if (cp_cc_source_names[0]) {
            for (j = 0; cp_cc_source_names[j]; j++) {
                fprintf(outfile, msvc6_begin_cc_source, cp_cc_source_path, cp_cc_source_names[j]);
                for (i = 0; i < max_i; i++) {
                    if (sdl) {
                        fprintf(outfile, msvc6_begin_ifs_sdl[i], cp_name);
                    } else {
                        fprintf(outfile, msvc6_begin_ifs_native[i], cp_name);
                    }
                    fprintf(outfile, msvc6_cc_custom_build_part1, cp_cc_source_path, cp_cc_source_names[j], cp_cc_source_names[j], cp_name, msvc6_releases[i]);
                    if (cp_source_names[0]) {
                        if (sdl) {
                            fprintf(outfile, msvc6_cc_custom_build_part2b_sdl, msvc6_base_cpp_lib_gui_part1_sdl[i]);
                        } else {
                            fprintf(outfile, msvc6_cc_custom_build_part2b_native, msvc6_base_cpp_lib_gui_part1_native[i]);
                        }
                    } else {
                        if (sdl) {
                            fprintf(outfile, msvc6_cc_custom_build_part2a_sdl, msvc6_base_cpp_lib_gui_part1_sdl[i]);
                        } else {
                            fprintf(outfile, msvc6_cc_custom_build_part2a_native, msvc6_base_cpp_lib_gui_part1_native[i]);
                        }
                    }
                    if (sdl) {
                        fprintf(outfile, msvc6_cc_custom_build_part3, msvc6_base_cpp_cc_sdl[i], cp_name, msvc6_releases[i], cp_name, cp_name, msvc6_releases[i], cp_name, msvc6_releases[i]);
                    } else {
                        fprintf(outfile, msvc6_cc_custom_build_part3, msvc6_base_cpp_cc_native[i], cp_name, msvc6_releases[i], cp_name, cp_name, msvc6_releases[i], cp_name, msvc6_releases[i]);
                    }
                }
                fprintf(outfile, "%s# End Source File\r\n", msvc6_endif);
            }
        }
        if (cp_source_names[0]) {
            for (j = 0; cp_source_names[j]; j++) {
                fprintf(outfile, msvc6_source, cp_source_names[j]);
            }
        }
        if (cp_sdl_source_names[0] && sdl) {
            for (j = 0; cp_sdl_source_names[j]; j++) {
                fprintf(outfile, msvc6_source, cp_sdl_source_names[j]);
            }
        }
        if (cp_native_source_names[0] && !sdl) {
            for (j = 0; cp_native_source_names[j]; j++) {
                fprintf(outfile, msvc6_source, cp_native_source_names[j]);
            }
        }
        if (cp_cpusource_names[0]) {
            for (j = 0; cp_cpusource_names[j]; j++) {
                fprintf(outfile, msvc6_custom_cpu_source, cp_cpusource_names[j], cp_name, cp_name);
                if (!sdl) {
                    fprintf(outfile, msvc6_custom_cpu_native, cp_name, cp_name);
                }
                fprintf(outfile, msvc6_custom_cpu_end);
            }
        }
        if (cp_custom_message) {
            fprintf(outfile, msvc6_custom_source, cp_custom_source);
            for (i = 0; i < max_i; i++) {
                if (sdl) {
                    fprintf(outfile, msvc6_begin_ifs_sdl[i], cp_name);
                } else {
                    fprintf(outfile, msvc6_begin_ifs_native[i], cp_name);
                }
                fprintf(outfile, msvc6_custom_section_part1);
                for (j = 0; cp_custom_deps[j]; j++) {
                    fprintf(outfile, "\"%s\"\t", cp_custom_deps[j]);
                }
                fprintf(outfile, msvc6_custom_section_part2, cp_custom_message, cp_custom_source, cp_custom_output, cp_custom_command);
            }
            fprintf(outfile, msvc6_endif);
            fprintf(outfile, msvc6_end_custom_source);
        }
        if (cp_type == CP_TYPE_GUI) {
            if (cp_res_source_name && !sdl) {
                fprintf(outfile, msvc6_res_source_start, cp_res_source_name);
                for (i = 0; i < max_i; i++) {
                    if (sdl) {
                        fprintf(outfile, msvc6_begin_ifs_sdl[i], cp_name);
                    } else {
                        fprintf(outfile, msvc6_begin_ifs_native[i], cp_name);
                    }
                    fprintf(outfile, msvc6_res_source_part1);
                    for (j = 0; cp_res_deps[j]; j++) {
                        fprintf(outfile, "\t\"..\\%s\"", cp_res_deps[j]);
                    }
                    fprintf(outfile, msvc6_res_source_part2, cp_res_source_name, cp_res_output_name);
                    for (j = 0; cp_res_deps[j]; j++) {
                        fprintf(outfile, " ..\\%s", cp_res_deps[j]);
                        if (cp_res_deps[j + 1]) {
                            fprintf(outfile, " +");
                        }
                    }
                    fprintf(outfile, " %s /b\r\n\r\n# End Custom Build\r\n\r\n", cp_res_output_name);
                }
                fprintf(outfile, msvc6_res_source_part3, cp_res_output_name);
            }
        }
        fprintf(outfile, msvc6_end_target);
    }

    if (outfile) {
        fclose(outfile);
    }

    if (filename) {
        free(filename);
    }

    if (cp_libs) {
        free(cp_libs);
        cp_libs = NULL;
    }

    if (read_buffer) {
        free(read_buffer);
        read_buffer = NULL;
        read_buffer_line = 0;
        read_buffer_pos = 0;
        read_buffer_len = 0;
    }

    return retval;
}

/* ---------------------------------------------------------------------- */

static char *msvc4_project_start = "# Microsoft Developer Studio Generated NMAKE File, Based on %s.dsp\r\n"
                                   "!IF \"$(CFG)\" == \"\"\r\n"
                                   "CFG=%s - Win32 Release\r\n"
                                   "!MESSAGE No configuration specified. Defaulting to %s - Win32 Release.\r\n"
                                   "!ENDIF \r\n\r\n"
                                   "!IF \"$(CFG)\" != \"%s - Win32 Release\" && \"$(CFG)\" != \"%s - Win32 Debug\"\r\n"
                                   "!MESSAGE Invalid configuration \"$(CFG)\" specified.\r\n"
                                   "!MESSAGE You can specify a configuration when running NMAKE\r\n"
                                   "!MESSAGE by defining the macro CFG on the command line. For example:\r\n"
                                   "!MESSAGE \r\n"
                                   "!MESSAGE NMAKE /f \"%s.mak\" CFG=\"%s - Win32 Debug\"\r\n"
                                   "!MESSAGE \r\n"
                                   "!MESSAGE Possible choices for configuration are:\r\n"
                                   "!MESSAGE \r\n";

static char *msvc4_based_on_gui = "!MESSAGE \"%s - Win32 Release\" (based on \"Win32 (x86) Application\")\r\n"
                                  "!MESSAGE \"%s - Win32 Debug\" (based on \"Win32 (x86) Application\")\r\n";

static char *msvc4_based_on_console = "!MESSAGE \"%s - Win32 Release\" (based on \"Win32 (x86) Console Application\")\r\n"
                                      "!MESSAGE \"%s - Win32 Debug\" (based on \"Win32 (x86) Console Application\")\r\n";

static char *msvc4_based_on_lib = "!MESSAGE \"%s - Win32 Release\" (based on \"Win32 (x86) Static Library\")\r\n"
                                  "!MESSAGE \"%s - Win32 Debug\" (based on \"Win32 (x86) Static Library\")\r\n";

static char *msvc4_project_part2 = "!MESSAGE \r\n"
                                   "!ERROR An invalid configuration is specified.\r\n"
                                   "!ENDIF \r\n"
                                   "\r\n"
                                   "!IF \"$(OS)\" == \"Windows_NT\"\r\n"
                                   "NULL=\r\n"
                                   "!ELSE \r\n"
                                   "NULL=nul\r\n"
                                   "!ENDIF \r\n"
                                   "\r\n";

static char *msvc4_ifs[2] = {
    "!IF  \"$(CFG)\" == \"%s - Win32 Release\"\r\n\r\n",
    "!ELSEIF  \"$(CFG)\" == \"%s - Win32 Debug\"\r\n\r\n"
};

static char *msvc4_type[2] = {
    "Release",
    "Debug"
};

static char *msvc4_outdir_app = "OUTDIR=.\\..\\..\\..\\..\\data\r\n";

static char *msvc4_outdir_lib = "OUTDIR=.\\libs\\%s\\%s\r\n";

static char *msvc4_intdir = "INTDIR=.\\libs\\%s\\%s\r\n"
                            "# Begin Custom Macros\r\n";

static char *msvc4_outdir_app_small = "OutDir=.\\..\\..\\..\\..\\data\r\n";

static char *msvc4_outdir_lib_small = "OutDir=.\\libs\\%s\\%s\r\n";

static char *msvc4_ecm = "# End Custom Macros\r\n"
                         "\r\n";

static char *msvc4_rec_if_0 = "!IF \"$(RECURSE)\" == \"0\"\r\n"
                              "\r\n";

static char *msvc4_all = "ALL : ";

static char *msvc4_rec_else = "\r\n\r\n"
                              "!ELSE \r\n"
                              "\r\n";

static char *msvc4_double_end = "\r\n\r\n";

static char *msvc4_rec_endif = "\r\n"
                               "\r\n"
                               "!ENDIF \r\n"
                               "\r\n";

static char *msvc4_rec_if_1 = "!IF \"$(RECURSE)\" == \"1\"\r\n";

static char *msvc4_clean = "CLEAN :";

static char *msvc4_else_clean_endif = "\r\n"
                                      "!ELSE \r\n"
                                      "CLEAN :\r\n"
                                      "!ENDIF \r\n";

static char *msvc4_clean_newline = "CLEAN :\r\n";

static char *msvc4_erase = "\t-@erase \r\n";

static char *msvc4_erase_object = "\t-@erase \"$(INTDIR)\\%s.obj\"\r\n";

static char *msvc4_erase_lib = "\t-@erase \"$(OUTDIR)\\%s.lib\"\r\n";

static char *msvc4_erase_exe = "\t-@erase \"$(OUTDIR)\\%s.exe\"\r\n";

static char *msvc4_erase_exe_debug = "\t-@erase \"$(OUTDIR)\\%s.ilk\"\r\n"
                                     "\t-@erase \"$(OUTDIR)\\%s.pdb\"\r\n";

static char *msvc4_erase_custom = "\t-@erase \"%s\"\r\n";

static char *msvc4_mkdir_outdir = "\r\n"
                                  "\"$(OUTDIR)\" :\r\n"
                                  "    if not exist \"$(OUTDIR)/$(NULL)\" mkdir \"$(OUTDIR)\"\r\n"
                                  "\r\n";

static char *msvc4_mkdir_intdir = "\"$(INTDIR)\" :\r\n"
                                  "    if not exist \"$(INTDIR)/$(NULL)\" mkdir \"$(INTDIR)\"\r\n"
                                  "\r\n";

static char *msvc4_cpp_start = "CPP=cl.exe\r\n"
                               "CPP_PROJ=/nologo ";

static char *msvc4_flags[2] = {
    "/MD /W3 /GX /O2 ",
    "/MDd /W3 /GX /Z7 /Od "
};

static char *msvc4_standard_inc = "/I \".\\\\\" /I \"..\\\\\" /I \"..\\..\\..\\\\\" ";

static char *msvc4_standard_defs = "/D \"WIN32\" /D \"WINMIPS\" /D \"IDE_COMPILE\" /D \"_WINDOWS\" /D \"DONT_USE_UNISTD_H\" ";

static char *msvc4_console_defs = "/D \"_CONSOLE\" /D \"_MBCS\" ";

static char *msvc4_debug_defs[2] = {
    "/D \"NDEBUG\" ",
    "/D \"_DEBUG\" "
};

static char *msvc4_other_flags = "/Fp\"$(INTDIR)\\%s.pch\" /YX /Fo\"$(INTDIR)\\\\\" /Fd\"$(INTDIR)\\\\\"  /c \r\n"
                                 "\r\n"
                                 ".c{$(INTDIR)}.obj :\r\n"
                                 "   $(CPP) @<<\r\n"
                                 "   $(CPP_PROJ) $< \r\n"
                                 "<<\r\n"
                                 "\r\n"
                                 ".cpp{$(INTDIR)}.obj :\r\n"
                                 "   $(CPP) @<<\r\n"
                                 "   $(CPP_PROJ) $< \r\n"
                                 "<<\r\n"
                                 "\r\n"
                                 ".cxx{$(INTDIR)}.obj :\r\n"
                                 "   $(CPP) @<<\r\n"
                                 "   $(CPP_PROJ) $< \r\n"
                                 "<<\r\n"
                                 "\r\n"
                                 ".c{$(INTDIR)}.sbr :\r\n"
                                 "   $(CPP) @<<\r\n"
                                 "   $(CPP_PROJ) $< \r\n"
                                 "<<\r\n"
                                 "\r\n"
                                 ".cpp{$(INTDIR)}.sbr :\r\n"
                                 "   $(CPP) @<<\r\n"
                                 "   $(CPP_PROJ) $< \r\n"
                                 "<<\r\n"
                                 "\r\n"
                                 ".cxx{$(INTDIR)}.sbr :\r\n"
                                 "   $(CPP) @<<\r\n"
                                 "   $(CPP_PROJ) $< \r\n"
                                 "<<\r\n"
                                 "\r\n";

static char *msvc4_mtl_gui = "MTL=midl.exe\r\n"
                             "MTL_PROJ=/nologo %s/mktyplib203 /o \"NUL\" /win32 \r\n";


static char *msvc4_rsc_bsc32 = "RSC=rc.exe\r\n"
                               "BSC32=bscmake.exe\r\n"
                               "BSC32_FLAGS=/nologo /o\"$(OUTDIR)\\%s.bsc\" \r\n"
                               "BSC32_SBRS= \\\r\n"
                               "\r\n";

static char *msvc4_lib32 = "LIB32=link.exe -lib\r\n"
                           "LIB32_FLAGS=/nologo /out:\"$(OUTDIR)\\%s.lib\" \r\n"
                           "LIB32_OBJS= \\\r\n";

static char *msvc4_lib32_end = "\r\n"
                               "\r\n"
                               "\"$(OUTDIR)\\%s.lib\" : \"$(OUTDIR)\" $(DEF_FILE) $(LIB32_OBJS)\r\n"
                               "    $(LIB32) @<<\r\n"
                               "  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)\r\n"
                               "<<\r\n"
                               "\r\n";

static char *msvc4_link32 = "LINK32=link.exe\r\n"
                            "LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib comctl32.lib winmm.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib version.lib SDLmain.lib SDL.lib opengl32.lib /nologo ";

static char *msvc4_subsystem_gui = "/subsystem:windows ";

static char *msvc4_subsystem_console = "/subsystem:console ";

static char *msvc4_incremental_debug[2] = {
    "/incremental:no ",
    "/incremental:yes "
};

static char *msvc4_pdb = "/pdb:\"$(OUTDIR)\\%s.pdb\" %s/machine:MIPS %s/out:\"$(OUTDIR)\\%s.exe\" %s\r\n"
                         "LINK32_OBJS= \\\r\n";

static char *msvc4_exe32 = "\"$(OUTDIR)\\%s.exe\" : \"$(OUTDIR)\" $(DEF_FILE) $(LINK32_OBJS)\r\n"
                           "    $(LINK32) @<<\r\n"
                           "  $(LINK32_FLAGS) $(LINK32_OBJS)\r\n"
                           "<<\r\n"
                           "\r\n";

static char *msvc4_post_custom = "InputPath=%s\r\n"
                                 "SOURCE=\"$(InputPath)\"\r\n"
                                 "\r\n"
                                 "\"%s\" : $(SOURCE) \"$(INTDIR)\" \"$(OUTDIR)\"\r\n"
                                 "\t<<tempfile.bat \r\n"
                                 "\t@echo off \r\n"
                                 "\t%s\r\n"
                                 "<< \r\n"
                                 "\r\n"
                                 "\r\n";

static char *msvc4_endif_double_end = "!ENDIF \r\n"
                                      "\r\n"
                                      "\r\n";

static char *msvc4_global_if = "!IF \"$(CFG)\" == \"%s - Win32 Release\" || \"$(CFG)\" == \"%s - Win32 Debug\"\r\n"
                               "\r\n";

static char *msvc4_global_make = "\"%s - Win32 %s\" : \r\n"
                                 "   cd \".\"\r\n"
                                 "   $(MAKE) /$(MAKEFLAGS) /F \".\\%s.mak\" CFG=\"%s - Win32 %s\" \r\n"
                                 "   cd \".\"\r\n"
                                 "\r\n"
                                 "\"%s - Win32 %sCLEAN\" : \r\n"
                                 "   cd \".\"\r\n"
                                 "   $(MAKE) /$(MAKEFLAGS) /F \".\\%s.mak\" CFG=\"%s - Win32 %s\" RECURSE=1 CLEAN \r\n"
                                 "   cd \".\"\r\n"
                                 "\r\n";

static char *msvc4_endif_single_end = "!ENDIF \r\n"
                                      "\r\n";

static char *msvc4_source_build = "SOURCE=..\\..\\..\\%s\r\n"
                                  "\r\n"
                                  "\"$(INTDIR)\\%s.obj\" : $(SOURCE) \"$(INTDIR)\"\r\n"
                                  "\t$(CPP) $(CPP_PROJ) $(SOURCE)\r\n"
                                  "\r\n";

static char *msvc4_cpu_source_start = "SOURCE=..\\..\\..\\%s\r\n"
                                      "\r\n";

static char *msvc4_custom_source_start = "SOURCE=%s\r\n"
                                         "\r\n";

static char *msvc4_cpp_switch_start = "\r\n"
                                      "CPP_SWITCHES=/nologo ";

static char *msvc4_cpp_switch_flags[2] = {
    "/MD /W3 /GX /Ot /Oa /Ow /Oi /Op /Oy /Ob2 ",
    "/MDd /W3 /GX /Z7 /Od "
};

static char *msvc4_cpu_source_flags = "/Fp\"$(INTDIR)\\%s.pch\" /YX /Fo\"$(INTDIR)\\\\\" /Fd\"$(INTDIR)\\\\\"  /c \r\n"
                                      "\r\n"
                                      "\"$(INTDIR)\\%s.obj\" : $(SOURCE) \"$(INTDIR)\"\r\n"
                                      "\t$(CPP) @<<\r\n"
                                      "  $(CPP_SWITCHES) $(SOURCE)\r\n"
                                      "<<\r\n"
                                      "\r\n";

static char *msvc4_custom_source = "InputDir=.\\\r\n"
                                   "InputPath=\"%s\"\r\n"
                                   "USERDEP__SRC=\"%s\"\r\n"
                                   "\r\n"
                                   "\"%s\" : $(SOURCE) \"$(INTDIR)\" \"$(OUTDIR)\" $(USERDEP__SRC)\r\n"
                                   "\t<<tempfile.bat \r\n"
                                   "\t@echo off \r\n"
                                   "\t%s\r\n"
                                   "<< \r\n"
                                   "\r\n"
                                   "\r\n";

static char *msvc4_strip_c_extension(char *name)
{
    char *new_name = NULL;
    int i;
    int len = 0;
    int slash_pos = 0;

    if (!name) {
        return NULL;
    }

    len = strlen(name);

    if (len < 3) {
        return NULL;
    }

    if (name[len - 2] != '.' || name[len - 1] != 'c') {
        return NULL;
    }

    for (i = 0; name[i] != 0; i++) {
        if (name[i] == '\\') {
            slash_pos = i + 1;
        }
    }

    new_name = vs_strndup(name + slash_pos, len - (2 + slash_pos));

    return new_name;
}

static void msvc4_output_all_phase1(FILE *outfile)
{
    if (cp_custom_output) {
        fprintf(outfile, "\"%s\" ", cp_custom_output);
    }
    if (cp_type == CP_TYPE_LIBRARY) {
        fprintf(outfile, "\"$(OUTDIR)\\%s.lib\" ", cp_name);
    } else {
        if (cp_source_names[0] || cp_sdl_source_names[0]) {
            fprintf(outfile, "\"$(OUTDIR)\\%s.exe\" ", cp_name);
        }
    }
    if (cp_post_custom_output) {
        fprintf(outfile, "\"%s\" ", cp_post_custom_output);
    }
}

static int output_msvc4_file(char *fname, int filelist)
{
    char *filename;
    int retval = 0;
    FILE *outfile = NULL;
    int i, j, k;
    int libs_dep_count = 0;
    char *rfname;
    char *new_name = NULL;

    if (!strcmp(fname, "arch_native") || !strcmp(fname, "arch_sdl")) {
        rfname = "arch";
    } else {
        rfname = fname;
    }

    if (filelist) {
        filename = malloc(strlen(rfname) + sizeof("../../sdl/winmips-msvc/.mak"));
        sprintf(filename, "../../sdl/winmips-msvc/%s.mak", rfname);
    } else {
        filename = malloc(strlen(rfname) + sizeof(".mak"));
        sprintf(filename, "%s.mak", rfname);
    }

    outfile = fopen(filename, "wb");
    if (!outfile) {
        printf("Cannot open %s for output\n", filename);
        retval = 1;
    }

    if (!retval) {
        if (cp_dep_names[0]) {
            for (j = 0; cp_dep_names[j]; j++) {
                libs_dep_count += is_lib_type(cp_dep_names[j]);
            }
        }
        fprintf(outfile, msvc4_project_start, cp_name, cp_name, cp_name, cp_name, cp_name, cp_name, cp_name);
        switch (cp_type) {
            default:
            case CP_TYPE_GUI:
                fprintf(outfile, msvc4_based_on_gui, cp_name, cp_name);
                break;
            case CP_TYPE_CONSOLE:
                fprintf(outfile, msvc4_based_on_console, cp_name, cp_name);
                break;
            case CP_TYPE_LIBRARY:
                fprintf(outfile, msvc4_based_on_lib, cp_name, cp_name);
                break;
        }
        fprintf(outfile, msvc4_project_part2);
        for (i = 0; i < 2; i++) {
            fprintf(outfile, msvc4_ifs[i], cp_name);
            if (cp_type == CP_TYPE_LIBRARY) {
                fprintf(outfile, msvc4_outdir_lib, cp_name, msvc4_type[i]);
            } else {
                fprintf(outfile, msvc4_outdir_app);
            }
            fprintf(outfile, msvc4_intdir, cp_name, msvc4_type[i]);
            if (cp_type == CP_TYPE_LIBRARY) {
                fprintf(outfile, msvc4_outdir_lib_small, cp_name, msvc4_type[i]);
            } else {
                fprintf(outfile, msvc4_outdir_app_small);
            }
            fprintf(outfile, msvc4_ecm);
            if (cp_dep_names[0]) {
                fprintf(outfile, msvc4_rec_if_0);
                fprintf(outfile, msvc4_all);
                msvc4_output_all_phase1(outfile);
                fprintf(outfile, msvc4_rec_else);
                fprintf(outfile, msvc4_all);
                for (j = 0; cp_dep_names[j]; j++) {
                    if (strncmp(cp_dep_names[j], "resid", 5) && strcmp(cp_dep_names[j], "x64dtv") && strcmp(cp_dep_names[j], "xscpu64")) {
                        fprintf(outfile, "\"%s - Win32 %s\" ", cp_dep_names[j], msvc4_type[i]);
                    }
                }
                msvc4_output_all_phase1(outfile);
                fprintf(outfile, msvc4_rec_endif);
                fprintf(outfile, msvc4_rec_if_1);
                fprintf(outfile, msvc4_clean);
                j--;
                while (j != -1) {
                    if (strncmp(cp_dep_names[j], "resid", 5) && strcmp(cp_dep_names[j], "x64dtv") && strcmp(cp_dep_names[j], "xscpu64")) {
                        fprintf(outfile, "\"%s - Win32 %sCLEAN\" ", cp_dep_names[j], msvc4_type[i]);
                    }
                    j--;
                }
                fprintf(outfile, msvc4_else_clean_endif);
            } else {
                fprintf(outfile, msvc4_all);
                msvc4_output_all_phase1(outfile);
                fprintf(outfile, msvc4_double_end);
                fprintf(outfile, msvc4_clean_newline);
            }
            if (cp_source_names[0] || cp_sdl_source_names[0]) {
                if (cp_source_names[0]) {
                    for (j = 0; cp_source_names[j]; j++) {
                        new_name = msvc4_strip_c_extension(cp_source_names[j]);
                        if (new_name) {
                            fprintf(outfile, msvc4_erase_object, new_name);
                            free(new_name);
                        }
                    }
                }
                if (cp_sdl_source_names[0]) {
                    for (j = 0; cp_sdl_source_names[j]; j++) {
                        new_name = msvc4_strip_c_extension(cp_sdl_source_names[j]);
                        if (new_name) {
                            fprintf(outfile, msvc4_erase_object, new_name);
                            free(new_name);
                        }
                    }
                }
                if (cp_cpusource_names[0]) {
                    for (j = 0; cp_cpusource_names[j]; j++) {
                        new_name = msvc4_strip_c_extension(cp_cpusource_names[j]);
                        if (new_name) {
                            fprintf(outfile, msvc4_erase_object, new_name);
                            free(new_name);
                        }
                    }
                }
                if (cp_type == CP_TYPE_LIBRARY) {
                    fprintf(outfile, msvc4_erase_lib, cp_name);
                } else {
                    fprintf(outfile, msvc4_erase_exe, cp_name);
                    if (i & 1) {
                        fprintf(outfile, msvc4_erase_exe_debug, cp_name, cp_name);
                    }
                }
                if (cp_custom_output) {
                    fprintf(outfile, msvc4_erase_custom, cp_custom_output);
                }
                if (cp_post_custom_output) {
                    fprintf(outfile, msvc4_erase_custom, cp_post_custom_output);
                }
            } else {
                fprintf(outfile, msvc4_erase);
            }
            fprintf(outfile, msvc4_mkdir_outdir);
            if (cp_type != CP_TYPE_LIBRARY && cp_source_names[0]) {
                fprintf(outfile, msvc4_mkdir_intdir);
            }
            fprintf(outfile, msvc4_cpp_start);
            fprintf(outfile, msvc4_flags[i]);
            fprintf(outfile, msvc4_standard_inc);
            if (cp_include_dirs[0]) {
                for (j = 0; cp_include_dirs[j]; j++) {
                    fprintf(outfile, "/I \"..\\..\\..\\%s\" ", cp_include_dirs[j]);
                }
            }
            fprintf(outfile, msvc4_standard_defs);
            if (cp_type == CP_TYPE_CONSOLE) {
                fprintf(outfile, msvc4_console_defs);
            }
            fprintf(outfile, msvc4_debug_defs[i]);
            fprintf(outfile, msvc4_other_flags, cp_name);
            if (cp_type == CP_TYPE_GUI) {
                fprintf(outfile, msvc4_mtl_gui, msvc4_debug_defs[i]);
            }
            fprintf(outfile, msvc4_rsc_bsc32, cp_name);
            if (cp_type == CP_TYPE_LIBRARY) {
                fprintf(outfile, msvc4_lib32, cp_name);
            } else {
                fprintf(outfile, msvc4_link32);
                if (cp_type == CP_TYPE_GUI) {
                    fprintf(outfile, msvc4_subsystem_gui);
                } else {
                    fprintf(outfile, msvc4_subsystem_console);
                }
                fprintf(outfile, msvc4_incremental_debug[i]);
                fprintf(outfile, msvc4_pdb, cp_name, (i & 1) ? "/debug " : "", (i & 1) ? "/nodefaultlib:\"msvcrt.lib\" " : "", cp_name, (i & 1) ? "/pdbtype:sept" : "");
            }
            if (cp_source_names[0]) {
                for (j = 0; cp_source_names[j]; j++) {
                new_name = msvc4_strip_c_extension(cp_source_names[j]);
                if (new_name) {
                        fprintf(outfile, "\t\"$(INTDIR)\\%s.obj\" \\\r\n", new_name);
                        free(new_name);
                    }
                }
                if (cp_sdl_source_names[0]) {
                    for (j = 0; cp_sdl_source_names[j]; j++) {
                        new_name = msvc4_strip_c_extension(cp_sdl_source_names[j]);
                        if (new_name) {
                            fprintf(outfile, "\t\"$(INTDIR)\\%s.obj\" \\\r\n", new_name);
                            free(new_name);
                        }
                    }
                }
                if (cp_cpusource_names[0]) {
                    for (j = 0; cp_cpusource_names[j]; j++) {
                        new_name = msvc4_strip_c_extension(cp_cpusource_names[j]);
                        if (new_name) {
                            fprintf(outfile, "\t\"$(INTDIR)\\%s.obj\" \\\r\n", new_name);
                            free(new_name);
                        }
                    }
                }
                if (libs_dep_count) {
                    for (j = 0; cp_dep_names[j]; j++) {
                        if (is_lib_type(cp_dep_names[j])) {
                            fprintf(outfile, "\t\".\\libs\\%s\\%s\\%s.lib\" \\\r\n", cp_dep_names[j], msvc4_type[i], cp_dep_names[j]);
                        }
                    }
                }
                if (cp_type == CP_TYPE_LIBRARY) {
                    fprintf(outfile, msvc4_lib32_end, cp_name, msvc4_type[i], cp_name, cp_name);
                } else {
                    fprintf(outfile, msvc4_double_end);
                    if (cp_source_names[0]) {
                        fprintf(outfile, msvc4_exe32, cp_name);
                    }
                    if (cp_post_custom_source) {
                        fprintf(outfile, msvc4_post_custom, cp_post_custom_source, cp_post_custom_output, cp_post_custom_command);
                    }
                }
            }
        }
        fprintf(outfile, msvc4_endif_double_end);
        if (cp_source_names[0] || cp_dep_names[0]) {
            fprintf(outfile, msvc4_global_if, cp_name, cp_name);
            if (cp_dep_names[0]) {
                for (j = 0; cp_dep_names[j]; j++) {
                    if (strncmp(cp_dep_names[j], "resid", 5) && strcmp(cp_dep_names[j], "x64dtv") && strcmp(cp_dep_names[j], "xscpu64")) {
                        for (i = 0; i < 2; i++) {
                            fprintf(outfile, msvc4_ifs[i], cp_name);
                            fprintf(outfile, msvc4_global_make, cp_dep_names[j], msvc4_type[i], cp_dep_names[j], cp_dep_names[j], msvc4_type[i], cp_dep_names[j], msvc4_type[i], cp_dep_names[j], cp_dep_names[j], msvc4_type[i]);
                        }
                        fprintf(outfile, msvc4_endif_single_end);
                    }
                }
            }
            if (cp_source_names[0]) {
                for (j = 0; cp_source_names[j]; j++) {
                    new_name = msvc4_strip_c_extension(cp_source_names[j]);
                    if (new_name) {
                        fprintf(outfile, msvc4_source_build, cp_source_names[j], new_name);
                        free(new_name);
                    }
                }
            }
            if (cp_sdl_source_names[0]) {
                for (j = 0; cp_sdl_source_names[j]; j++) {
                    new_name = msvc4_strip_c_extension(cp_sdl_source_names[j]);
                    if (new_name) {
                        fprintf(outfile, msvc4_source_build, cp_sdl_source_names[j], new_name);
                        free(new_name);
                    }
                }
            }
            if (cp_cpusource_names[0]) {
                for (j = 0; cp_cpusource_names[j]; j++) {
                    fprintf(outfile, msvc4_cpu_source_start, cp_cpusource_names[j]);
                    for (i = 0; i < 2; i++) {
                        fprintf(outfile, msvc4_ifs[i], cp_name);
                        fprintf(outfile, msvc4_cpp_switch_start);
                        fprintf(outfile, msvc4_cpp_switch_flags[i]);
                        fprintf(outfile, msvc4_standard_inc);
                        if (cp_include_dirs[0]) {
                            for (k = 0; cp_include_dirs[k]; k++) {
                                fprintf(outfile, "/I \"..\\..\\..\\%s\" ", cp_include_dirs[k]);
                            }
                        }
                        fprintf(outfile, msvc4_standard_defs);
                        fprintf(outfile, msvc4_debug_defs[i]);
                        new_name = msvc4_strip_c_extension(cp_cpusource_names[j]);
                        if (new_name) {
                            fprintf(outfile, msvc4_cpu_source_flags, cp_name, new_name);
                            free(new_name);
                        }
                    }
                    fprintf(outfile, msvc4_endif_single_end);
                }
            }
            if (cp_custom_source) {
                fprintf(outfile, msvc4_custom_source_start, cp_custom_source);
                for (i = 0; i < 2; i++) {
                    fprintf(outfile, msvc4_ifs[i], cp_name);
                    fprintf(outfile, msvc4_custom_source, cp_custom_source, cp_custom_source, cp_custom_output, cp_custom_command);
                }
                fprintf(outfile, msvc4_endif_single_end);
            }
        }
        fprintf(outfile, msvc4_rec_endif);
    }

    if (outfile) {
        fclose(outfile);
    }

    if (filename) {
        free(filename);
    }

    if (cp_libs) {
        free(cp_libs);
        cp_libs = NULL;
    }

    if (read_buffer) {
        free(read_buffer);
        read_buffer = NULL;
        read_buffer_line = 0;
        read_buffer_pos = 0;
        read_buffer_len = 0;
    }

    return retval;
}

/* ---------------------------------------------------------------------- */

static char *make_cp_libs(void)
{
    int i;
	unsigned int j;
    char *retval = NULL;
    int length = 0;
    int k = 0;

    for (i = 0; cp_libs_elements[i]; i++) {
        length += (strlen(cp_libs_elements[i]) + 1);
    }

    retval = malloc(length);

    if (retval) {
        for (i = 0; cp_libs_elements[i]; i++) {
            for (j = 0; j < strlen(cp_libs_elements[i]); j++) {
                retval[k++] = cp_libs_elements[i][j];
            }
            if (cp_libs_elements[i + 1]) {
                retval[k++] = ' ';
            } else {
                retval[k] = 0;
            }
        }
    }

    return retval;
}

static int split_line_names(char *line, char **names, int max_names)
{
    int count = 0;
    int i = 0;

    names[count++] = line;
    while (line[i]) {
        while (!isspace(line[i]) && line[i]) {
            i++;
        }
        if (line[i]) {
            line[i++] = 0;
            names[count++] = line + i;
            if (count > max_names) {
                printf("name list overflow\n");
                return 1;
            }
        }
    }
    names[count] = NULL;

#if MKMSVC_DEBUG
    for (i = 0; names[i]; i++) {
        printf("NAME %d: %s\n", i, names[i]);
    }
#endif

    return 0;
}

static void strip_trailing_spaces(char *line)
{
    int pos = strlen(line);

    while (isspace(line[pos - 1]) && pos) {
        line[pos - 1] = 0;
        pos--;
    }
}

static char *get_next_line_from_buffer(void)
{
    char *retval = NULL;

    if (read_buffer_pos >= read_buffer_len) {
        return NULL;
    }

    retval = read_buffer + read_buffer_pos;

    while ((read_buffer_pos < read_buffer_len) && (read_buffer[read_buffer_pos] != '\n')) {
        read_buffer_pos++;
    }
    if (read_buffer[read_buffer_pos - 1] == '\r') {
        read_buffer[read_buffer_pos - 1] = 0;
    } else {
        read_buffer[read_buffer_pos] = 0;
    }
    read_buffer_pos++;
    read_buffer_line++;

    if (strlen(retval) && isspace(retval[strlen(retval) - 1])) {
        strip_trailing_spaces(retval);
    }

    return retval;
}

static int fill_line_names(char **names, int max_names)
{
    int count = 0;
    char *line = get_next_line_from_buffer();
#if MKMSVC_DEBUG
    int i;
#endif

    if (names[count]) {
        while (names[count]) {
            count++;
        }
    }

    while (line && strlen(line)) {
        if (ffmpeg) {
            names[count++] = line + 1;
        } else {
            if (ffmpeg_file(line + 1) == 0) {
                names[count++] = line + 1;
            }
        }
        if (count > max_names) {
            printf("name list overflow\n");
            return 1;
        }
        line = get_next_line_from_buffer();
    }
    names[count] = NULL;

#if MKMSVC_DEBUG
    for (i = 0; names[i]; i++) {
        printf("NAME %d: %s\n", i, names[i]);
    }
#endif

    return 0;
}

static void free_buffers(void)
{
    if (names_buffer) {
        free(names_buffer);
    }
    if (read_buffer) {
        free(read_buffer);
    }
    if (cp_libs) {
        free(cp_libs);
    }
}

/* ---------------------------------------------------------------------- */

static int parse_template(char *filename)
{
    char *line = NULL;
    int parsed;
    int is_main_project_template = 0;

    /* set current project parameters to 'empty' */
    cp_name = NULL;
    cp_type = -1;
    cp_dep_names[0] = NULL;
    cp_include_dirs[0] = NULL;
    cp_source_names[0] = NULL;
    cp_sdl_source_names[0] = NULL;
    cp_native_source_names[0] = NULL;
    cp_custom_message = NULL;
    cp_custom_source = NULL;
    cp_custom_deps[0] = NULL;
    cp_custom_output = NULL;
    cp_custom_command = NULL;
    cp_post_custom_message = NULL;
    cp_post_custom_source = NULL;
    cp_post_custom_output = NULL;
    cp_post_custom_command = NULL;
    cp_cc_source_path = NULL;
    cp_cc_source_names[0] = NULL;
    cp_res_source_name = NULL;
    cp_res_deps[0] = NULL;
    cp_res_output_name = NULL;
    cp_cpusource_names[0] = NULL;
    cp_libs_elements[0] = NULL;

    line = get_next_line_from_buffer();
    while (line) {
        parsed = 0;
        if (!strlen(line)) {
#if MKMSVC_DEBUG
            printf("Line %d is empty\n", read_buffer_line);
#endif
            parsed = 1;
        }
        if (!parsed && line[0] == '#') {
#if MKMSVC_DEBUG
            printf("Line %d is a comment line\n", read_buffer_line);
#endif
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_NAME_STRING, CP_NAME_SIZE)) {
            cp_name = line + CP_NAME_SIZE;
#if MKMSVC_DEBUG
            printf("Line %d is a project name line: %s\n", read_buffer_line, cp_name);
#endif
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_TYPE_STRING, CP_TYPE_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is a project type line: %s\n", read_buffer_line, line + CP_TYPE_SIZE);
#endif
            if (!strcmp(line + CP_TYPE_SIZE, "library")) {
                cp_type = CP_TYPE_LIBRARY;
                parsed = 1;
            }
            if (!parsed && !strcmp(line + CP_TYPE_SIZE, "console")) {
                cp_type = CP_TYPE_CONSOLE;
                parsed = 1;
            }
            if (!parsed && !strcmp(line + CP_TYPE_SIZE, "gui")) {
                cp_type = CP_TYPE_GUI;
                parsed = 1;
            }
            if (!parsed) {
                printf("Unknown project type '%s' in line %d of %s\n", line + CP_TYPE_SIZE, read_buffer_line, filename);
                return 1;
            }
        }
        if (!parsed && !strncmp(line, CP_PROJECT_NAMES_STRING, CP_PROJECT_NAMES_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is a project name line\n", read_buffer_line);
#endif
            if (fill_line_names(project_names, MAX_NAMES)) {
                printf("Error parsing project names in line %d of %s\n", read_buffer_line, filename);
                return 1;
            }
            is_main_project_template = 1;
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_PROJECT_NAMES_NAT_STRING, CP_PROJECT_NAMES_NAT_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is a native project name line\n", read_buffer_line);
#endif
            if (fill_line_names(project_names_native, MAX_NAMES)) {
                printf("Error parsing native project names in line %d of %s\n", read_buffer_line, filename);
                return 1;
            }
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_PROJECT_NAMES_SDL_STRING, CP_PROJECT_NAMES_SDL_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is an sdl project name line\n", read_buffer_line);
#endif
            if (fill_line_names(project_names_sdl, MAX_NAMES)) {
                printf("Error parsing sdl project names in line %d of %s\n", read_buffer_line, filename);
                return 1;
            }
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_DEPS_STRING, CP_DEPS_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is a project deps line: %s\n", read_buffer_line, line + CP_DEPS_SIZE);
#endif
            if (fill_line_names(cp_dep_names, MAX_DEP_NAMES)) {
                printf("Error parsing names in line %d of %s\n", read_buffer_line, filename);
                return 1;
            }
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_INCLUDES_STRING, CP_INCLUDES_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is a project include dirs line\n", read_buffer_line);
#endif
            if (fill_line_names(cp_include_dirs, MAX_DIRS)) {
                printf("Error parsing include dir name in line %d of %s\n", read_buffer_line, filename);
                return 1;
            }
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_SOURCES_STRING, CP_SOURCES_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is a project sources line\n", read_buffer_line);
#endif
            if (fill_line_names(cp_source_names, MAX_NAMES)) {
                printf("Error parsing source name in line %d of %s\n", read_buffer_line, filename);
                return 1;
            }
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_SDL_SOURCES_STRING, CP_SDL_SOURCES_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is an sdl project sources line\n", read_buffer_line);
#endif
            if (fill_line_names(cp_sdl_source_names, MAX_NAMES)) {
                printf("Error parsing sdl source name in line %d of %s\n", read_buffer_line, filename);
                return 1;
            }
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_NATIVE_SOURCES_STRING, CP_NATIVE_SOURCES_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is a native project sources line\n", read_buffer_line);
#endif
            if (fill_line_names(cp_native_source_names, MAX_NAMES)) {
                printf("Error parsing native source name in line %d of %s\n", read_buffer_line, filename);
                return 1;
            }
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_CPUSOURCES_STRING, CP_CPUSOURCES_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is a project cpu sources line\n", read_buffer_line);
#endif
            if (current_level >= cpu_source_level) {
                if (fill_line_names(cp_source_names, MAX_NAMES)) {
                    printf("Error parsing cpu source name in line %d of %s\n", read_buffer_line, filename);
                    return 1;
                }
            } else {
                if (fill_line_names(cp_cpusource_names, MAX_CPU_NAMES)) {
                    printf("Error parsing cpu source name in line %d of %s\n", read_buffer_line, filename);
                    return 1;
                }
           }
           parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_LIBS_STRING, CP_LIBS_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is a project link libs line\n", read_buffer_line);
#endif
            if (fill_line_names(cp_libs_elements, MAX_LIBS)) {
                printf("Error parsing link library name in line %d of %s\n", read_buffer_line, filename);
                return 1;
            }
            parsed = 1;
            cp_libs = make_cp_libs();
        }
        if (!parsed && !strncmp(line, CP_CUSTOM_MSG_STRING, CP_CUSTOM_MSG_SIZE)) {
            cp_custom_message = line + CP_CUSTOM_MSG_SIZE;
#if MKMSVC_DEBUG
            printf("Line %d is a project custom message line: %s\n", read_buffer_line, cp_custom_message);
#endif
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_CUSTOM_SRC_STRING, CP_CUSTOM_SRC_SIZE)) {
            cp_custom_source = line + CP_CUSTOM_SRC_SIZE;
#if MKMSVC_DEBUG
            printf("Line %d is a project custom source line: %s\n", read_buffer_line, cp_custom_source);
#endif
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_CUSTOM_DEPS_STRING, CP_CUSTOM_DEPS_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is a custom deps line: %s\n", read_buffer_line, line + CP_CUSTOM_DEPS_SIZE);
#endif
            if (split_line_names(line + CP_CUSTOM_DEPS_SIZE, cp_custom_deps, MAX_DEP_NAMES)) {
                printf("Error parsing custom deps in line %d of %s\n", read_buffer_line, filename);
                return 1;
            }
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_CUSTOM_OUTPUT_STRING, CP_CUSTOM_OUTPUT_SIZE)) {
            cp_custom_output = line + CP_CUSTOM_OUTPUT_SIZE;
#if MKMSVC_DEBUG
            printf("Line %d is a project custom output line: %s\n", read_buffer_line, cp_custom_output);
#endif
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_CUSTOM_COMMAND_STRING, CP_CUSTOM_COMMAND_SIZE)) {
            cp_custom_command = line + CP_CUSTOM_COMMAND_SIZE;
#if MKMSVC_DEBUG
            printf("Line %d is a project custom command line: %s\n", read_buffer_line, cp_custom_command);
#endif
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_POST_CUSTOM_MSG_STRING, CP_POST_CUSTOM_MSG_SIZE)) {
            cp_post_custom_message = line + CP_POST_CUSTOM_MSG_SIZE;
#if MKMSVC_DEBUG
            printf("Line %d is a project post custom message line: %s\n", read_buffer_line, cp_post_custom_message);
#endif
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_POST_CUSTOM_SRC_STRING, CP_POST_CUSTOM_SRC_SIZE)) {
            cp_post_custom_source = line + CP_POST_CUSTOM_SRC_SIZE;
#if MKMSVC_DEBUG
            printf("Line %d is a project post custom source line: %s\n", read_buffer_line, cp_post_custom_source);
#endif
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_POST_CUSTOM_OUTPUT_STRING, CP_POST_CUSTOM_OUTPUT_SIZE)) {
            cp_post_custom_output = line + CP_POST_CUSTOM_OUTPUT_SIZE;
#if MKMSVC_DEBUG
            printf("Line %d is a project post custom output line: %s\n", read_buffer_line, cp_post_custom_output);
#endif
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_POST_CUSTOM_COMMAND_STRING, CP_POST_CUSTOM_COMMAND_SIZE)) {
            cp_post_custom_command = line + CP_POST_CUSTOM_COMMAND_SIZE;
#if MKMSVC_DEBUG
            printf("Line %d is a project post custom command line: %s\n", read_buffer_line, cp_post_custom_command);
#endif
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_CC_SOURCES_STRING, CP_CC_SOURCES_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is a project cc sources line\n", read_buffer_line);
#endif
            if (fill_line_names(cp_cc_source_names, MAX_NAMES)) {
                printf("Error parsing cc source name in line %d of %s\n", read_buffer_line, filename);
                return 1;
            }
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_CC_SOURCE_PATH_STRING, CP_CC_SOURCE_PATH_SIZE)) {
            cp_cc_source_path = line + CP_CC_SOURCE_PATH_SIZE;
#if MKMSVC_DEBUG
            printf("Line %d is a project cc source path line: %s\n", read_buffer_line, cp_cc_source_path);
#endif
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_RES_SOURCE_STRING, CP_RES_SOURCE_SIZE)) {
            cp_res_source_name = line + CP_RES_SOURCE_SIZE;
#if MKMSVC_DEBUG
            printf("Line %d is a project res source name line: %s\n", read_buffer_line, cp_res_source_name);
#endif
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_RES_DEPS_STRING, CP_RES_DEPS_SIZE)) {
#if MKMSVC_DEBUG
            printf("Line %d is a res deps line: %s\n", read_buffer_line, line + CP_RES_DEPS_SIZE);
#endif
            if (split_line_names(line + CP_RES_DEPS_SIZE, cp_res_deps, MAX_DEP_NAMES)) {
                printf("Error parsing resource deps in line %d of %s\n", read_buffer_line, filename);
                return 1;
            }
            parsed = 1;
        }
        if (!parsed && !strncmp(line, CP_RES_OUTPUT_STRING, CP_RES_OUTPUT_SIZE)) {
            cp_res_output_name = line + CP_RES_OUTPUT_SIZE;
#if MKMSVC_DEBUG
            printf("Line %d is a project res output name line: %s\n", read_buffer_line, cp_res_output_name);
#endif
            parsed = 1;
        }
        if (!parsed) {
#if MKMSVC_DEBUG
            printf("Line %d is something else: %s\n", read_buffer_line, line);
#endif
            printf("Unknown command in line %d in %s\n", read_buffer_line, filename);
            return 1;
        }
        line = get_next_line_from_buffer();
    }

    if (!is_main_project_template) {

        /* Some sanity checks on the custom section */
        if (cp_custom_message || cp_custom_source || cp_custom_deps[0] || cp_custom_output || cp_custom_command) {
            if (!(cp_custom_message && cp_custom_source && cp_custom_deps[0] && cp_custom_output && cp_custom_command)) {
                printf("Missing custom section elements in %s\n", filename);
                return 1;
            }
        }

        /* Name always has to be given */
        if (!cp_name) {
            printf("Missing project name in %s\n", filename);
            return 1;
        }

        /* type always has to be given */
        if (cp_type == -1) {
            printf("Missing project type in %s\n", filename);
            return 1;
        }

        /* for console and gui types, libs have to be given */
        if (cp_type != CP_TYPE_LIBRARY) {
            if (!cp_libs) {
                printf("Missing link libs in %s\n", filename);
                return 1;
            }
        }
    }

    return 0;
}

static int read_template_file(char *fname, int sdl)
{
    char *filename;
    struct stat statbuf;
    FILE *infile = NULL;
    int retval = 0;

    filename = malloc(strlen(fname) + sizeof(".tmpl"));
    sprintf(filename, "%s.tmpl", fname);

    if (stat(filename, &statbuf) < 0) {
        printf("%s doesn't exist\n", filename);
        retval = 1;
    }

    if (!retval) {
        read_buffer_len = statbuf.st_size;

        if (read_buffer_len == 0) {
            printf("%s has no size\n", filename);
            retval = 1;
        }
    }

    if (!retval) {
        infile = fopen(filename, "rb");
        if (!infile) {
            printf("%s cannot be opened for reading\n", filename);
            retval = 1;
        }
    }

    if (!retval) {
        read_buffer = malloc(read_buffer_len);

        if (fread(read_buffer, 1, read_buffer_len, infile) < (unsigned int)read_buffer_len) {
            printf("Cannot read from %s\n", filename);
            retval = 1;
        }
    }

    if (!retval) {
        if (parse_template(filename)) {
            printf("Parse error in %s\n", filename);
            retval = 1;
        }
    }

    if (infile) {
        fclose(infile);
    }

    if (filename) {
        free(filename);
    }

    return retval;
}

/* ---------------------------------------------------------------------- */

static void usage(void)
{
    printf("Usage: mkmsvc <basename> <generation options> <type>\n\n");
    printf("basename is the name of a template file, without the '.tmpl' extension.\n");
    printf("generation options are:\n");
    printf("-4 = msvc4 mips makefile generation\n");
    printf("-6 = msvc6 (98) project file generation\n");
    printf("-7[0] = msvc7.0 (2002) project file generation\n");
    printf("-71 = msvc7.1 (2003) project file generation\n");
    printf("-8 = msvc8 (2005) project file generation\n");
    printf("-9 = msvc9 (2008) project file generation\n");
    printf("-10 = msvc10 (2010) project file generation\n");
    printf("-11 = msvc11 (2012) project file generation\n\n");
    printf("types are:\n");
    printf("-native = generate native project files.\n");
    printf("-sdl = generate SDL project files.\n");
    printf("-ffmpeg = include static ffmpeg support.\n");
}

int main(int argc, char *argv[])
{
    int i;
    int msvc4 = 0;
    int msvc6 = 0;
    int msvc70 = 0;
    int msvc71 = 0;
    int msvc8 = 0;
    int msvc9 = 0;
    int msvc10 = 0;
    int msvc11 = 0;
    int msvc12 = 0;
    int native = 0;
    int sdl = 0;
    int error = 0;
    char *filename = NULL;

    if (argc == 1) {
        usage();
    } else {
        for (i = 1; i < argc; i++) {
            if (argv[i][0] != '-') {
                if (!filename) {
                    filename = argv[i];
                } else {
                    printf("Error: Only one basename can be given.\n\n");
                    usage();
                    exit(1);
                }
            } else {
                if (!strcmp(argv[i], "-4")) {
                    msvc4 = 1;
                }
                if (!strcmp(argv[i], "-6")) {
                    msvc6 = 1;
                }
                if (!strcmp(argv[i], "-7") || !strcmp(argv[i], "-70")) {
                    msvc70 = 1;
                }
                if (!strcmp(argv[i], "-71")) {
                    msvc71 = 1;
                }
                if (!strcmp(argv[i], "-8")) {
                    msvc8 = 1;
                }
                if (!strcmp(argv[i], "-9")) {
                    msvc9 = 1;
                }
                if (!strcmp(argv[i], "-10")) {
                    msvc10 = 1;
                }
                if (!strcmp(argv[i], "-11")) {
                    msvc11 = 1;
                }
                if (!strcmp(argv[i], "-12")) {
                    msvc12 = 1;
                }
                if (!strcmp(argv[i], "-native")) {
                    native = 1;
                }
                if (!strcmp(argv[i], "-sdl")) {
                    sdl = 1;
                }
                if (!strcmp(argv[i], "-ffmpeg")) {
                    ffmpeg = 1;
                }
            }
        }

        /* No filename, so use the default */
        if (!filename) {
            filename = "vice";
        }

        /* at least ONE version has to be given */
        if (!msvc4 && !msvc6 && !msvc70 && !msvc71 && !msvc8 && !msvc9 && !msvc10 && !msvc11 && !msvc12) {
            printf("Error: No generation option(s) given\n");
            error = 1;
        }

        /* ONE type has to be given */
        if (native + sdl != 1) {
            printf("Error: One type needs to be given\n");
            error = 1;
        }

        /* msvc4 has to have sdl defined too */
        if (msvc4 && !sdl) {
            printf("Error: mips nt msvc4 can only be compiled for sdl\n");
            error = 1;
        }

        project_names[0] = NULL;
        project_names_sdl[0] = NULL;
        project_names_native[0] = NULL;
        project_lib_type[0] = NULL;

        if (!error) {
            if (read_template_file(filename, sdl)) {
                printf("Generation error.\n");
            } else {
                names_buffer = read_buffer;
                read_buffer = NULL;
                read_buffer_line = 0;
                read_buffer_pos = 0;
                read_buffer_len = 0;
                if (!error && msvc4) {
                    current_level = 40;
                    if (project_names[0]) {
                        for (i = 0; project_names[i] && !error; i++) {
                            if (strncmp(project_names[i], "resid", 5) && strcmp(project_names[i], "x64dtv") && strcmp(project_names[i], "xscpu64")) {
                                error = preparse_project(project_names[i]);
                            }
                        }
                        if (project_names_sdl[0] && !error) {
                            for (i = 0; project_names_sdl[i] && !error; i++) {
                                error = preparse_project(project_names_sdl[i]);
                            }
                        }
                        for (i = 0; project_names[i] && !error; i++) {
                            if (strncmp(project_names[i], "resid", 5) && strcmp(project_names[i], "x64dtv") && strcmp(project_names[i], "xscpu64")) {
                                error = read_template_file(project_names[i], 1);
                                if (!error) {
                                    error = output_msvc4_file(project_names[i], 1);
                                }
                            }
                        }
                        if (project_names_sdl[0] && !error) {
                            for (i = 0; project_names_sdl[i] && !error; i++) {
                                error = read_template_file(project_names_sdl[i], 1);
                                if (!error) {
                                    error = output_msvc4_file(project_names_sdl[i], 1);
                                }
                            }
                        }
                        free_preparse();
                    } else {
                        error = output_msvc4_file(filename, 0);
                    }
                    if (!error) {
                        free_buffers();
                    }
                }
                if (!error && msvc6) {
                    current_level = 60;
                    if (project_names[0]) {
                        error = open_msvc6_main_project(sdl);
                        for (i = 0; project_names[i] && !error; i++) {
                            error = read_template_file(project_names[i], sdl);
                            if (!error) {
                                error = output_msvc6_file(project_names[i], 1, sdl);
                            }
                        }
                        if (sdl) {
                            if (project_names_sdl[0] && !error) {
                                for (i = 0; project_names_sdl[i] && !error; i++) {
                                    error = read_template_file(project_names_sdl[i], sdl);
                                    if (!error) {
                                        error = output_msvc6_file(project_names_sdl[i], 1, sdl);
                                    }
                                }
                            }
                        } else {
                            if (project_names_native[0] && !error) {
                                for (i = 0; project_names_native[i] && !error; i++) {
                                    error = read_template_file(project_names_native[i], sdl);
                                    if (!error) {
                                        error = output_msvc6_file(project_names_native[i], 1, sdl);
                                    }
                                }
                            }
                        }
                        close_msvc6_main_project(sdl);
                    } else {
                        error = output_msvc6_file(filename, 0, sdl);
                    }
                    if (!error) {
                        free_buffers();
                    }
                }
                if (!error && msvc70) {
                    current_level = 70;
                    if (project_names[0]) {
                        error = open_msvc70_main_project(sdl);
                        for (i = 0; project_names[i] && !error; i++) {
                            error = read_template_file(project_names[i], sdl);
#if MKMSVC_DEBUG
                            printf("Parse done\n");
#endif
                            if (!error) {
                                error = output_msvc7_file(project_names[i], 1, 70, sdl);
#if MKMSVC_DEBUG
                                printf("Output done\n");
#endif
                            }
                        }
                        if (sdl) {
                            if (project_names_sdl[0] && !error) {
                                for (i = 0; project_names_sdl[i] && !error; i++) {
                                    error = read_template_file(project_names_sdl[i], sdl);
                                    if (!error) {
                                        error = output_msvc7_file(project_names_sdl[i], 1, 70, sdl);
                                    }
                                }
                            }
                        } else {
                            if (project_names_native[0] && !error) {
                                for (i = 0; project_names_native[i] && !error; i++) {
                                    error = read_template_file(project_names_native[i], sdl);
                                    if (!error) {
                                        error = output_msvc7_file(project_names_native[i], 1, 70, sdl);
                                    }
                                }
                            }
                        }
                        close_msvc70_main_project(sdl);
                    } else {
                        error = output_msvc7_file(filename, 0, 70, sdl);
                    }
                    if (!error) {
                        free_buffers();
                    }
                }
                if (!error && msvc71) {
                    current_level = 71;
                    if (project_names[0]) {
                        error = open_msvc71_main_project(sdl);
                        for (i = 0; project_names[i] && !error; i++) {
                            error = read_template_file(project_names[i], sdl);
#if MKMSVC_DEBUG
                            printf("Parse done\n");
#endif
                            if (!error) {
                                error = output_msvc7_file(project_names[i], 1, 71, sdl);
#if MKMSVC_DEBUG
                                printf("Output done\n");
#endif
                            }
                        }
                        if (sdl) {
                            if (project_names_sdl[0] && !error) {
                                for (i = 0; project_names_sdl[i] && !error; i++) {
                                    error = read_template_file(project_names_sdl[i], sdl);
                                    if (!error) {
                                        error = output_msvc7_file(project_names_sdl[i], 1, 71, sdl);
                                    }
                                }
                            }
                        } else {
                            if (project_names_native[0] && !error) {
                                for (i = 0; project_names_native[i] && !error; i++) {
                                    error = read_template_file(project_names_native[i], sdl);
                                    if (!error) {
                                        error = output_msvc7_file(project_names_native[i], 1, 71, sdl);
                                    }
                                }
                            }
                        }
                        close_msvc71_main_project(sdl);
                    } else {
                        error = output_msvc7_file(filename, 0, 71, sdl);
                    }
                    if (!error) {
                        free_buffers();
                    }
                }
                if (!error && msvc8) {
                    current_level = 80;
                    if (project_names[0]) {
                        error = open_msvc8_main_project(sdl);
                        for (i = 0; project_names[i] && !error; i++) {
                            error = read_template_file(project_names[i], sdl);
#if MKMSVC_DEBUG
                            printf("Parse done\n");
#endif
                            if (!error) {
                                error = output_msvc8_file(project_names[i], 1, sdl);
#if MKMSVC_DEBUG
                                printf("Output done\n");
#endif
                            }
                        }
                        if (sdl) {
                            if (project_names_sdl[0] && !error) {
                                for (i = 0; project_names_sdl[i] && !error; i++) {
                                    error = read_template_file(project_names_sdl[i], sdl);
                                    if (!error) {
                                        error = output_msvc8_file(project_names_sdl[i], 1, sdl);
                                    }
                                }
                            }
                        } else {
                            if (project_names_native[0] && !error) {
                                for (i = 0; project_names_native[i] && !error; i++) {
                                    error = read_template_file(project_names_native[i], sdl);
                                    if (!error) {
                                        error = output_msvc8_file(project_names_native[i], 1, sdl);
                                    }
                                }
                            }
                        }
                        close_msvc8_main_project(sdl);
                    } else {
                        error = output_msvc8_file(filename, 0, sdl);
                    }
                    if (!error) {
                        free_buffers();
                    }
                }
                if (!error && msvc9) {
                    current_level = 90;
                    if (project_names[0]) {
                        error = open_msvc9_main_project(sdl);
                        for (i = 0; project_names[i] && !error; i++) {
                            error = read_template_file(project_names[i], sdl);
#if MKMSVC_DEBUG
                            printf("Parse done\n");
#endif
                            if (!error) {
                                error = output_msvc9_file(project_names[i], 1, sdl);
#if MKMSVC_DEBUG
                                printf("Output done\n");
#endif
                            }
                        }
                        if (sdl) {
                            if (project_names_sdl[0] && !error) {
                                for (i = 0; project_names_sdl[i] && !error; i++) {
                                    error = read_template_file(project_names_sdl[i], sdl);
                                    if (!error) {
                                        error = output_msvc9_file(project_names_sdl[i], 1, sdl);
                                    }
                                }
                            }
                        } else {
                            if (project_names_native[0] && !error) {
                                for (i = 0; project_names_native[i] && !error; i++) {
                                    error = read_template_file(project_names_native[i], sdl);
                                    if (!error) {
                                        error = output_msvc9_file(project_names_native[i], 1, sdl);
                                    }
                                }
                            }
                        }
                        close_msvc9_main_project(sdl);
                    } else {
                        error = output_msvc9_file(filename, 0, sdl);
                    }
                    if (!error) {
                        free_buffers();
                    }
                }
                if (!error && msvc10) {
                    current_level = 100;
                    if (project_names[0]) {
                        error = open_msvc10_11_12_main_project(0, 0, sdl);
                        for (i = 0; project_names[i] && !error; i++) {
                            error = read_template_file(project_names[i], sdl);
#if MKMSVC_DEBUG
                            printf("Parse done\n");
#endif
                            if (!error) {
                                error = output_msvc10_11_12_file(project_names[i], 1, 0, 0, sdl);
#if MKMSVC_DEBUG
                                printf("Output done\n");
#endif
                            }
                        }
                        if (sdl) {
                            if (project_names_sdl[0] && !error) {
                                for (i = 0; project_names_sdl[i] && !error; i++) {
                                    error = read_template_file(project_names_sdl[i], sdl);
                                    if (!error) {
                                        error = output_msvc10_11_12_file(project_names_sdl[i], 1, 0, 0, sdl);
                                    }
                                }
                            }
                        } else {
                            if (project_names_native[0] && !error) {
                                for (i = 0; project_names_native[i] && !error; i++) {
                                    error = read_template_file(project_names_native[i], sdl);
                                    if (!error) {
                                        error = output_msvc10_11_12_file(project_names_native[i], 1, 0, 0, sdl);
                                    }
                                }
                            }
                        }
                        close_msvc10_11_12_main_project(0, 0, sdl);
                    } else {
                        error = output_msvc10_11_12_file(filename, 0, 0, 0, sdl);
                    }
                    if (!error) {
                        free_buffers();
                    }
                }
                if (!error && msvc11) {
                    current_level = 110;
                    if (project_names[0]) {
                        error = open_msvc10_11_12_main_project(1, 0, sdl);
                        for (i = 0; project_names[i] && !error; i++) {
                            error = read_template_file(project_names[i], sdl);
#if MKMSVC_DEBUG
                            printf("Parse done\n");
#endif
                            if (!error) {
                                error = output_msvc10_11_12_file(project_names[i], 1, 1, 0, sdl);
#if MKMSVC_DEBUG
                                printf("Output done\n");
#endif
                            }
                        }
                        if (sdl) {
                            if (project_names_sdl[0] && !error) {
                                for (i = 0; project_names_sdl[i] && !error; i++) {
                                    error = read_template_file(project_names_sdl[i], sdl);
                                    if (!error) {
                                        error = output_msvc10_11_12_file(project_names_sdl[i], 1, 1, 0, sdl);
                                    }
                                }
                            }
                        } else {
                            if (project_names_native[0] && !error) {
                                for (i = 0; project_names_native[i] && !error; i++) {
                                    error = read_template_file(project_names_native[i], sdl);
                                    if (!error) {
                                        error = output_msvc10_11_12_file(project_names_native[i], 1, 1, 0, sdl);
                                    }
                                }
                            }
                        }
                        close_msvc10_11_12_main_project(1, 0, sdl);
                    } else {
                        error = output_msvc10_11_12_file(filename, 0, 1, 0, sdl);
                    }
                    if (!error) {
                        free_buffers();
                    }
                }
                if (!error && msvc12) {
                    current_level = 120;
                    if (project_names[0]) {
                        error = open_msvc10_11_12_main_project(1, 1, sdl);
                        for (i = 0; project_names[i] && !error; i++) {
                            error = read_template_file(project_names[i], sdl);
#if MKMSVC_DEBUG
                            printf("Parse done\n");
#endif
                            if (!error) {
                                error = output_msvc10_11_12_file(project_names[i], 1, 1, 1, sdl);
#if MKMSVC_DEBUG
                                printf("Output done\n");
#endif
                            }
                        }
                        if (sdl) {
                            if (project_names_sdl[0] && !error) {
                                for (i = 0; project_names_sdl[i] && !error; i++) {
                                    error = read_template_file(project_names_sdl[i], sdl);
                                    if (!error) {
                                        error = output_msvc10_11_12_file(project_names_sdl[i], 1, 1, 1, sdl);
                                    }
                                }
                            }
                        } else {
                            if (project_names_native[0] && !error) {
                                for (i = 0; project_names_native[i] && !error; i++) {
                                    error = read_template_file(project_names_native[i], sdl);
                                    if (!error) {
                                        error = output_msvc10_11_12_file(project_names_native[i], 1, 1, 1, sdl);
                                    }
                                }
                            }
                        }
                        close_msvc10_11_12_main_project(1, 1, sdl);
                    } else {
                        error = output_msvc10_11_12_file(filename, 0, 1, 1, sdl);
                    }
                    if (!error) {
                        free_buffers();
                    }
                }
                if (error) {
                    printf("Generation error.\n");
                } else {
                    printf("Generation complete.\n");
                }
            }
        }
    }
}
