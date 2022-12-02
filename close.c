/*
 * $Log: close.c,v $
 * Revision 1.6  1999/04/14 05:02:02  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.5  1999/04/04 23:59:37  sergio
 * Corregidas las rutas de los includes.
 *
 * Revision 1.4  1999/03/28 19:38:27  sergio
 * Breves modificaciones en el código para optimizar el uso de autoheader
 *
 * Revision 1.1 1999/02/15 00:20:50  sergio
 * Función que informa al servidor que la ejecución a finalizado
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

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <qronos.h>
#include <auth.h>
#include <msg.h>

void 
qronos_close (program foo)
{
    int error = 1;
    char *buffer;

    /* Preparamos el paquete a enviar */
    buffer = packmsg (foo);

    /* Se llama a call_qronosd() que es la que envia el paquete */
    while (error)
	{
	    switch (call_qronosd (buffer))
		{
		case COMM_OK:
		    error = 0;
		    break;
		case COMM_CONNECT_ERROR:
		    if (++error > 5)
			return;
		    break;
		}
	}
    /* la memoria de buffer debe ser liberada */
    free (buffer);
}
