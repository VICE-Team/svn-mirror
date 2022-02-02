/** \file   atomic.c
 * \brief   Atomic operations
 *
 * \author  David Hogan <david.q.hogan@gmail.com>
 */

/*
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

#ifndef VICE_ATOMIC_H
#define VICE_ATOMIC_H

#include <stdint.h>

typedef int32_t atomic_flag_t;

#if defined(_MSC_VER) && !defined(__clang__)
#error "You'll need to implement these using msvc intrinsics"
#else
#define atomic_flag_set(x) __atomic_store_n((x), 1, __ATOMIC_RELEASE)
#define atomic_flag_clear(x) __atomic_store_n((x), 0, __ATOMIC_RELEASE)
#define atomic_flag_check(x) __atomic_load_n((x), __ATOMIC_ACQUIRE)
#endif

#endif /* #ifndef VICE_ATOMIC_H */