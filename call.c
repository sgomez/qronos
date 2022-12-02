/*
 * $Log: call.c,v $
 * Revision 1.8  1999/04/14 05:02:01  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.7  1999/04/06 02:10:00  sergio
 * Adaptadas las funciones de comunicaciones al estandar POSIX
 *
 * Revision 1.6  1999/04/04 14:12:01  sergio
 * Corregido un error leve de compilación por declaración
 * implicita (string.h añadido).
 *
 * Revision 1.5  1999/03/29 23:41:13  sergio
 * Corregido un error al enviar datos al servidor
 *
 * Revision 1.4  1999/03/28 19:38:27  sergio
 * Breves modificaciones en el código para optimizar el uso de autoheader
 *
 * Revision 1.1 1999/02/15 00:20:50  sergio
 * Función que se encarga de comunicarse con el servidor 
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

#if HAVE_CONFIG_H
# include <conf.h>
#endif

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <qronos.h>
#include <auth.h>

int 
call_qronosd (char *msg)
{
    int sfd;
    struct sockaddr_in qserver;
    struct hostent *hp;
    char buffer[512];

    /* creamos y configuramos el socket para la conexión con el servidor */
    if ((sfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	return COMM_SOCKET_ERROR;
    if ((hp = gethostbyname (SERVER)) == NULL)
	return COMM_SERVER_UNKNOWN;
    bcopy (hp->h_addr, &qserver.sin_addr, hp->h_length);

    qserver.sin_family = AF_INET;
    qserver.sin_port = htons (PORT);

    /* conectamos */
    if (connect (sfd, &qserver, sizeof (qserver)) == -1)
	return COMM_CONNECT_ERROR;

    /* Enviamos el programa a comprobar */
    if ((write (sfd, msg, strlen(msg)+1)) == -1)
	{
	    close (sfd);
	    return COMM_SEND_ERROR;
	}

    /* Recibimos la respuesta del servidor */
    if ((read (sfd, buffer, sizeof (buffer))) == -1)
	{
	    close (sfd);
	    return COMM_RECV_ERROR;
	}

    sprintf(msg,"%s",buffer);

    close (sfd);
    return COMM_OK;
}
