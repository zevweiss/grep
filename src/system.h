/* Portability cruft.  Include after config.h and sys/types.h.
   Copyright 1996, 1998-2000, 2007, 2009-2011 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef GREP_SYSTEM_H
#define GREP_SYSTEM_H 1

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "binary-io.h"
#include "configmake.h"
#include "dirname.h"
#include "minmax.h"
#include "same-inode.h"

#if O_BINARY
# define HAVE_DOS_FILE_CONTENTS 1
#endif

#ifdef EISDIR
# define is_EISDIR(e, f) ((e) == EISDIR)
#else
# define is_EISDIR(e, f) 0
#endif

#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

enum { EXIT_TROUBLE = 2 };

#include <gettext.h>
#define N_(String) gettext_noop(String)
#define _(String) gettext(String)

#include <locale.h>

#ifndef initialize_main
# define initialize_main(argcp, argvp)
#endif

/* Do struct stat *S, *T have the same file attributes?

   POSIX says that two files are identical if st_ino and st_dev are
   the same, but many file systems incorrectly assign the same (device,
   inode) pair to two distinct files, including:

   - GNU/Linux NFS servers that export all local file systems as a
     single NFS file system, if a local device number (st_dev) exceeds
     255, or if a local inode number (st_ino) exceeds 16777215.

   - Network Appliance NFS servers in snapshot directories; see
     Network Appliance bug #195.

   - ClearCase MVFS; see bug id ATRia04618.

   Check whether two files that purport to be the same have the same
   attributes, to work around instances of this common bug.  Do not
   inspect all attributes, only attributes useful in checking for this
   bug.

   It's possible for two distinct files on a buggy file system to have
   the same attributes, but it's not worth slowing down all
   implementations (or complicating the configuration) to cater to
   these rare cases in buggy implementations.  */

#ifndef same_file_attributes
# define same_file_attributes(s, t) \
   ((s)->st_mode == (t)->st_mode \
    && (s)->st_nlink == (t)->st_nlink \
    && (s)->st_uid == (t)->st_uid \
    && (s)->st_gid == (t)->st_gid \
    && (s)->st_size == (t)->st_size \
    && (s)->st_mtime == (t)->st_mtime \
    && (s)->st_ctime == (t)->st_ctime)
#endif

#define SAME_REGULAR_FILE(s, t) \
  (SAME_INODE (s, t) && same_file_attributes (&s, &t))

#include "unlocked-io.h"
#endif
