/*
 * $Log: log.c,v $
 * Revision 1.3  1999/04/18 13:26:20  sergio
 * Corregida la ruta del fichero de registro
 *
 * Revision 1.2  1999/04/14 05:02:02  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.1  1999/03/29 21:34:11  sergio
 * Fichero que crea un fichero de registro
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

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <exec.h>

void 
log (char *mens)
{
    FILE *fd;
    time_t local_t = time (NULL);
    char *date = ctime (&local_t);
    fd = fopen (PATH_QRONOS"/var/log/quota.log", "a");
    fprintf (fd, "[%-19.19s] :: %s\n", date, mens);
    fclose (fd);
}
