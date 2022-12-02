/*
 * $Log: auth.c,v $
 * Revision 1.9  1999/04/18 13:23:38  sergio
 * Pequeñas modificaciones en la estructura del código
 *
 * Revision 1.8  1999/04/14 05:02:01  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.7  1999/04/06 02:10:43  sergio
 * Corregida la ruta de los includes
 *
 * Revision 1.6  1999/04/04 13:57:30  sergio
 * Error AUTH_NO_MORE añadido en caso de que ya se esté ejecutando una
 * copia del programa.
 *
 * Revision 1.5  1999/03/29 23:40:17  sergio
 * Detectado un nuevo caso de error
 *
 * Revision 1.4  1999/03/28 19:38:27  sergio
 * Breves modificaciones en el código para optimizar el uso de autoheader
 *
 * Revision 1.1 1999/02/15 00:20:50 sergio
 * Módulo que determina si se tiene permiso para ejecutar un
 * programa 
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

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <qronos.h>
#include <auth.h>
#include <msg.h>

int 
qronos_auth (program * foo)
{
    int error = 1;
    char *msg;

    /* Preparamos el paquete a enviar */
    foo->id = PET_AUTH;
    msg = packmsg (*foo);

    /* Este modulo llama a call_qronosd() que es la que envia el paquete */
    while (error)
	{
	    switch (call_qronosd (msg))
		{
		case COMM_OK:
		    error = 0;
		    break;
		case COMM_SOCKET_ERROR:
		case COMM_SERVER_UNKNOWN:
		    return AUTH_SERVER_ERROR;
		case COMM_CONNECT_ERROR:
		    if (++error > 5)
			return AUTH_SERVER_ERROR;
		    break;
		}
	}

    /* Desempaquetamos la respuesta */
    free (foo);
    foo = unpackmsg(msg);
    if (foo==NULL) return AUTH_MSG_ERROR;

    /* Detecta si tiene permisos */
    switch (error = foo->timeleft.time)
	{
	case AUTH_NO_MORE:
	case AUTH_NO_TIME:
	case AUTH_NO_AUTH:
	    return error;
	default:
	    return AUTH_OK;
	}
}
