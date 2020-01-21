#
# Override the usual am__v_CC_0 and friends for extra silent builds.
#
# Written by David Hogan.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

AC_DEFUN([VICE_QUIET_CC],
[
	am__v_CXX_0="@"
	am__v_CC_0="@"
	am__v_CXXLD_0="@"
	am__v_CCLD_0="@"
	am__v_GEN_0="@"
	am__v_AR_0="@"

	AC_SUBST([am__v_CXX_0])
	AC_SUBST([am__v_CC_0])
	AC_SUBST([am__v_CCLD_0])
	AC_SUBST([am__v_CXXLD_0])
	AC_SUBST([am__v_GEN_0])
	AC_SUBST([am__v_AR_0])
	
])# VICE_QUIET_CC

