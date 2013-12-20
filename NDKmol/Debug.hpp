/*  NDKmol - Molecular Viewer on Android NDK

 (C) Copyright 2011 - 2012, biochem_fan

 This file is part of NDKmol.

 NDKmol is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef DEBUG_INCLUDED
#define DEBUG_INCLUDED

#ifdef __ANDROID__
#include <android/log.h>
#define DEBUG_PRINTF(...) \
	__android_log_print(ANDROID_LOG_DEBUG,"NDKmol", __VA_ARGS__)
#else
#include <stdio.h>
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#endif

#endif
