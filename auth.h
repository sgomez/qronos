/* 
 * $Log: auth.h,v $
 * Revision 1.7  1999/04/14 05:02:02  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.6  1999/04/04 13:58:41  sergio
 * Error AUTH_NO_MORE añadido en caso de que ya se esté ejecutando una
 * copia del programa.
 *
 * Revision 1.5  1999/03/29 23:40:26  sergio
 * Detectado un nuevo caso de error
 *
 * Revision 1.4  1999/03/28 19:38:55  sergio
 * Breves modificaciones en el código para optimizar el uso de autoheader
 *
 * Revision 1.1 1998/12/24 00:54:50  sergio
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

#if !defined(AUTH_H)
#define AUTH_H

#include <qronos.h>

#define PET_AUTH    -1
#define PET_TIME    -2

#define AUTH_NO_MORE     -2
#define AUTH_NO_AUTH     -1
#define AUTH_NO_TIME      0
#define AUTH_OK           1
#define AUTH_SERVER_ERROR 2
#define AUTH_MSG_ERROR    3

#define COMM_OK             0
#define COMM_SOCKET_ERROR   1
#define COMM_SERVER_UNKNOWN 2
#define COMM_CONNECT_ERROR  3
#define COMM_SEND_ERROR     4
#define COMM_RECV_ERROR     5

#if !defined(SERVER)
#define SERVER "localhost"
#endif

#if !defined(PORT)
#define PORT   6051
#endif

int qronos_auth(program *foo);
void qronos_close(program foo);
int call_qronosd(char *msg);

#endif
