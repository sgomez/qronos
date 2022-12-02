/*
 * $Log: qronos.c,v $
 * Revision 1.13  1999/04/14 05:28:29  sergio
 * Version 0.8 released
 *
 * Revision 1.12  1999/04/14 05:02:02  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.11  1999/04/06 02:05:57  sergio
 * Se detecta el nombre del usuario para que pueda ser enviado al servidor.
 * Se detecta si qronos existe.
 * Modificada la salida de un mensaje de error.
 *
 * Revision 1.10  1999/04/04 23:55:46  sergio
 * Se detecta si el programa no tiene como euid a el uid de qronos.
 *
 * Revision 1.9  1999/04/04 14:07:38  sergio
 * Añadido el caso AUTH_NO_MORE. Interceptadas las señales de interrupción.
 *
 * Revision 1.8  1999/03/29 23:38:58  sergio
 * Corregido un bug del gettext
 * Se ha modificado el paso de los parámetros a las funciones
 *
 * Revision 1.1  1998/02/18 00:00:05 sergio
 * Módulo principal del cliente (qronos) 
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
# include <conf.h>
#endif

#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>
#include <signal.h>
#include <stdio.h>
#include <exec.h>
#include <qronos.h>
#include <auth.h>
#include <exec.h>

#ifndef HAVE_STRCHR
#define strrchr rindex
#endif

#if STDC_HEADERS
# include <string.h>
#else
# ifndef HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
char *strchr (), *strrchr ();
#endif


#if ENABLE_NLS
#include <libintl.h>
#define _(String) gettext (String)
#else
#define _(String) String
#endif
#define N_(String) String

#if HAVE_SETLOCALE
# include <locale.h>
#endif



int
main (int argc, char **argv)
{

    program *foo=(program *)malloc(sizeof(program));
    const char *qronos = QRONOS;

    /* Datos de qronos y del usuario */
    struct passwd *pw_qronos, *pw_user;
    uid_t uid_qronos;

    signal(SIGINT,SIG_IGN);
    signal(SIGQUIT,SIG_IGN);

#if HAVE_SETLOCALE
    setlocale (LC_ALL, "");
#endif
#if ENABLE_NLS
    bindtextdomain (PACKAGE, LOCALEDIR);
    textdomain (PACKAGE);
#endif

    /* Leemos el uid de qronos */
    if ((pw_qronos = getpwnam (qronos)) == NULL)
	{
	    fprintf (stderr, _("Error: %s don't exist.\n"), QRONOS);
	    exit (-1);
	}
    uid_qronos = pw_qronos->pw_uid;
    if (geteuid()!=uid_qronos) 
	{
	    fprintf (stderr, _("Error: the euid must be the %s uid.\n"), QRONOS);
	    exit (-1);
	}

    /* Guardamos el nombre del programa, los argumentos y el nombre 
       del usuario*/
    foo->prog = (foo->prog = (char *) strrchr (argv[0], '/')) ? ++(foo->prog) : argv[0];
    pw_user = getpwuid(getuid());
    foo->user = strdup(pw_user->pw_name);

    if (!strcmp("qronos",foo->prog)) {
	fprintf (stderr, _(
"    Qronos version %s, Copyright (C) 1999 Sergio Gómez <sergio@cartero.com>\n"
"    Qronos comes with ABSOLUTELY NO WARRANTY; for details read the GPL.\n"
"    This is free software, and you are welcome to redistribute it under \n"
"    certain conditions.\n\n"), VERSION);
	return 0;
    }

    switch (qronos_auth (foo))
	{
	case AUTH_NO_TIME:
	    fprintf (stderr, _("Service unavailable, try again in %d %s.\n"), foo->timeleft.renove / 60 + 1, (foo->timeleft.renove/60)?_("minutes"):_("minute") );
	    break;
	case AUTH_NO_MORE:
	case AUTH_NO_AUTH:
	    fprintf (stderr, _("Permission denied.\n"));
	    break;
	case AUTH_SERVER_ERROR:
	case AUTH_MSG_ERROR:
	    fprintf (stderr, _("Service unavailable, try again later.\n"));
	    break;
	case AUTH_OK:
	    foo->argv = argv;
	    if (qronos_exec (foo))
		fprintf (stderr, _("An error had been happen while executing the program. Please contact with your System Administrator.\n"));
	    qronos_close (*foo);
	    break;
	}

    return 0;
}
