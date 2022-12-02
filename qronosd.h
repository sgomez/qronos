/*
 * $Log: qronosd.h,v $
 * Revision 1.7  1999/04/18 13:31:08  sergio
 * Modificada la estructura conection para poder guardar el nombre del programa
 *
 * Revision 1.6  1999/04/14 05:02:02  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.5  1999/04/05 00:00:53  sergio
 * Definición de macros para la manipulación de semáforos.
 *
 * Revision 1.4  1999/04/04 14:10:31  sergio
 * Modificada la función server para que acepte el identificador de conexión.
 *
 * Revision 1.3  1999/03/30 10:39:32  sergio
 * Corregido un error en los includes de que depende este archivo
 *
 * Revision 1.2  1999/03/29 23:42:51  sergio
 * Version final de la funcion server
 *
 * Revision 1.1  1999/03/29 21:33:19  sergio
 * Fichero de cabecera de qronosd.c
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

#if !defined(QRONOSD_H)
# define QRONOSD_H

#include <netinet/in.h>

#define MUTEX    0
#define WAIT    -1
#define SIGNAL   1

typedef struct {
    char *username;
    char *groupname;
    char *hostname;
    char *progname;
    int id;
} conection;    

void server(int s, struct sockaddr_in peeraddr_in, int id);

#endif /* QRONOSD_H */
