/** \file   src/arch/gtk3/opengl_renderer.h
 * \brief   OpenGL-based renderer for the GTK3 backend.
 *
 *  Michael C. Martin <mcmartin@gmail.com>
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
#ifndef VICE_OPENGL_RENDERER_H
#define VICE_OPENGL_RENDERER_H

#include "videoarch.h"

#ifdef MACOSX_SUPPORT
/* XXX: GdkGLContext is not implemented for Macs */
#undef HAVE_GTK3_OPENGL
#endif

#ifdef HAVE_GTK3_OPENGL
extern vice_renderer_backend_t vice_opengl_backend;
#endif

#endif
