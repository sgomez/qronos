/* 
 * $Log: exec.h,v $
 * Revision 1.6  1999/04/14 05:02:02  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.5  1999/04/04 23:56:44  sergio
 * Corregido la ruta de los includes
 *
 * Revision 1.4  1999/03/28 19:38:55  sergio
 * Breves modificaciones en el código para optimizar el uso de autoheader
 *
 * Revision 1.1 1998/12/23 16:17:05  sergio
 */

/* Copyright (C) 1999 Sergio Gomez <sergio@cartero.com>
   This file is part of the qronos program

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA. */

#if !defined(QRONOS_EXEC_H)
#define QRONOS_EXEC_H

#include <qronos.h>

#if !defined(PATH_QRONOS)
#define PATH_QRONOS "/usr/local/lib/qronos"
#endif

#if !defined(QRONOS)
#define QRONOS "qronos"
#endif

#if !defined(TICTAC)
#define TICTAC 30
#endif

#define EXEC_OK                0
#define EXEC_QRONOS_DONT_EXIST 1
#define EXEC_PROG_DONT_EXIST   2
#define EXEC_OUT_OF_MEMORY     3
#define EXEC_FORK_FAILED       4
#define EXEC_LINK_ERROR        5

#define TRUE        (1==1)
#define FALSE       (1==0)

typedef void (*sighandler_t)(int);

void cucu();
int qronos_exec (program *foo);

#endif
