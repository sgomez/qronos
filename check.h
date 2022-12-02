/*
 * $Log: check.h,v $
 * Revision 1.1  1999/04/18 12:56:08  sergio
 * División del módulo bbdd.h, para check.c
 *
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
   License along with this program; see the file COPYING.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA. */

#if !defined(CHECK_H)
#define CHECK_H

#include <qronos.h>
#include <qronosd.h>

#if !(CONF_FILE)
# define CONF_FILE PATH_QRONOS"/etc/qronos.cfg"
#endif

typedef struct {
    char *users;
    char *programs;
    char *hosts;
    int quota;
    int interval;
    int sigint;
} conf;

program *check_auth (conection conec);
conf *read_conf (int fd);

#endif /* CHECK_H */

