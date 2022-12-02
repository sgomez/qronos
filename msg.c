/*
 * $Log: msg.c,v $
 * Revision 1.6  1999/04/14 05:02:02  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.5  1999/04/06 02:12:57  sergio
 * Modificaciones en el (des)empaquetado debido a una modificación de la
 * estructura program.
 *
 * Revision 1.4  1999/03/29 23:41:57  sergio
 * Se detecta si el paquete está en mal estado
 *
 * Revision 1.3  1999/03/28 19:38:27  sergio
 * Breves modificaciones en el código para optimizar el uso de autoheader
 *
 * Revision 1.1 1999/02/15 00:20:50  sergio
 * Función que empaqueta y desempaqueta el contenido de la
   estructura 'program'
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

#if HAVE_CONFIG_H
# include "conf.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "qronos.h"
#include "msg.h"


char *
packmsg (program foo)
{
    char cad[512];
    sprintf (cad, "%s:%s:%d:%d:%d:%d:%d:%d",
	     foo.user,
	     foo.prog,
	     foo.id,
	     (int) foo.timeleft.time,
	     (int) foo.timeleft.renove,
	     (int) foo.timeleft.quota,
	     (int) foo.timeleft.interval,
	     foo.sigint);
    return strdup (cad);
}

program *
unpackmsg (char *msg)
{
    program *foo = (program *) malloc (sizeof (program));
    char *buffer = msg;
    char *token;

    if((token=strtok(buffer,DELIMITER))==NULL) goto pack_error;
    foo->user = strdup (token);
    if((token=strtok(NULL,DELIMITER))==NULL) goto pack_error;
    foo->prog = strdup (token);
    if((token=strtok(NULL,DELIMITER))==NULL) goto pack_error;
    foo->id = atoi (token);
    if((token=strtok(NULL,DELIMITER))==NULL) goto pack_error;
    foo->timeleft.time = atoi (token);
    if((token=strtok(NULL,DELIMITER))==NULL) goto pack_error;
    foo->timeleft.renove = atoi (token);
    if((token=strtok(NULL,DELIMITER))==NULL) goto pack_error;
    foo->timeleft.quota = atoi (token);
    if((token=strtok(NULL,DELIMITER))==NULL) goto pack_error;
    foo->timeleft.interval = atoi (token);
    if((token=strtok(NULL,DELIMITER))==NULL) goto pack_error;
    foo->sigint = atoi (token);
    return foo;

 pack_error:
    free(foo);
    return NULL;
}
