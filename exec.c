/* 
 * $Log: exec.c,v $
 * Revision 1.14  1999/04/18 13:11:47  sergio
 * Pequeños cambios en la estructura del código
 *
 * Revision 1.13  1999/04/14 05:06:15  sergio
 * Eliminada la función que desvinculaba al padre de la consola.
 * En la versión anterior se cambio la forma de instalar señales al estandar
 * POSIX.
 *
 * Revision 1.12  1999/04/14 05:02:02  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.11  1999/04/06 02:12:03  sergio
 * La señal de alarma para interrumpir el programa solo se instala
 * en caso de que así se lo haya especificado el servidor.
 *
 * Revision 1.10  1999/04/04 23:57:24  sergio
 * Corregida la ruta de los includes
 *
 * Revision 1.9  1999/04/04 14:03:19  sergio
 * Desvinculado el padre con el terminal. Corregidas dos entradas de includes.
 *
 * Revision 1.8  1999/03/29 23:36:32  sergio
 * Corregido el momento en el que se instala la alarma
 *
 * Revision 1.7  1999/03/28 19:14:26  sergio
 * Breves modificaciones en el código para optimizar el uso de autoheader
 *
 * Revision 1.1  1998/12/23 16:17:05 sergio
 * Función que se encarga de ejecutar un programa, así como de 
 * controlar el tiempo de ejecución del mismo 
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <exec.h>
#include <qronos.h>

/* variables globales */

int pid;			/* pid del hijo que ejecuta el programa */
qronos_t counter;		/* quota para este programa */

int 
qronos_exec (program * foo)
{

    extern char **environ;	/* Variables de entorno del sistema UNIX */

    /* Estructura para manejar las señales */
    struct sigaction act;

    /* Datos para la ejecución de los programas */
    char *prog, *source, *target, *dirname;
    const char *path = PATH_QRONOS;
    const char *qronos = QRONOS;

    /* Datos de qronos y del usuario */
    struct passwd *pw_qronos;
    uid_t uid_qronos;
    uid_t uid_user;

    time_t inicio;
    struct stat buf;
    int linkOK;

    /* Cogemos el límite temporal */
    counter = foo->timeleft;
    /* Guardamos los uid de qronos y del usuario */
    if ((pw_qronos = getpwnam (qronos)) == NULL)
	return EXEC_QRONOS_DONT_EXIST;
    uid_qronos = pw_qronos->pw_uid;
    uid_user = getuid ();


    /* Creamos la ruta del programa original y del enlace */
    source = (char *) malloc ((strlen (path) + strlen (foo->prog) + 10) * 
			      sizeof (char));
    if (source==NULL) return EXEC_OUT_OF_MEMORY;
    dirname = (char *) malloc ((strlen (path) + 18) * sizeof (char));
    if (dirname==NULL) return EXEC_OUT_OF_MEMORY;
    target = (char *) malloc ((strlen (path) + strlen (foo->prog) + 18) * 
			      sizeof (char));
    if (target==NULL) return EXEC_OUT_OF_MEMORY;
    sprintf (source, "%s/bin/%s", path, foo->prog);
    sprintf (dirname, "%s%s", path, tmpnam (NULL));
    sprintf (target, "%s/%s", dirname, foo->prog);

    /* Si el programa a ejecutar tiene activado el suid o el sgid lo ejecutamos
       directamente */
    if (stat (source, &buf) == -1)
	{
	    perror (foo->argv[0]);
	    return EXEC_PROG_DONT_EXIST;
	}

    if (buf.st_mode & (S_ISUID | S_ISGID))
	{
	    prog = strdup (source);
	    linkOK = FALSE;
	}
    else
	{
	    /* si no creamos un enlace a ese programa en un directorio 
	       accesible */
	    if (mkdir (dirname, S_IRWXU | S_IXGRP | S_IXOTH) == -1)
		{
		    return EXEC_LINK_ERROR;
		}
	    if (link (source, target) == -1)
		{
		    return EXEC_LINK_ERROR;
		}
	    prog = strdup (target);
	    linkOK = TRUE;
	}

    /* Creamos un proceso hijo que se encargará de ejecutar el programa */
    pid = fork ();
    switch (pid)
	{
	case -1:			/* Error en el fork */
	    return EXEC_FORK_FAILED;
	case 0:			/* Fork correcto ejecutamos el programa */
	    /* Si estamos ejecutando un enlace, necesitamos establecer quien 
	       somos. Como uid real la de qronos, como uid efectiva la 
	       del usuario */
	    if (linkOK)
		setreuid (uid_qronos, uid_user);
	    /* Para que funcione con el write de Linux */
	    /* CUIDADO CON ESTO */
	    else
		setreuid (uid_user, uid_qronos);
	    execve (prog, foo->argv, environ);
	    perror ("Error en execve");
	    exit (127);
	default:
	    /* Aseguramos primero al padre */
	    setreuid (uid_qronos, uid_qronos);
	    /* El padre espera que el hijo termine e informa del tiempo */
	    /* Si se creó el enlace se borra ahora */
	    if (linkOK)
		{
		    sleep (1);
		    unlink (prog);
		    rmdir (dirname);
		}
	    /* Controlamos el tiempo exacto de ejecucion */
	    inicio = time (NULL);

	    /* Si el programa debe ser detenido instalamos la alarma */
	    if (foo->sigint) {
		/* cucu() es la funcion manejadora de la alarma */
		act.sa_handler=cucu;
		sigaction(SIGALRM, &act, NULL);
		/* Instalamos una alarma que se activa cada TICTAC segundos */
		alarm(TICTAC);
	    }
	    /* Esperamos que el programa acabe */
	    while(waitpid (pid, NULL, 0)==-1);
	    /* Eliminamos el manejador */
	    act.sa_handler=SIG_IGN;
	    sigaction (SIGALRM, &act, NULL);
	    /* Calculamos el tiempo exacto de ejecución */
	    foo->timeleft.time = time(NULL) - inicio;
	    return EXEC_OK;
	}
}

void 
cucu ()
{
    counter.time -= TICTAC;
    counter.renove -= TICTAC;
    /* Comprobamos si puede renovar la quota */
    if (counter.renove <= 0)
	{
	    counter.time = counter.quota;
	    /* Supuestamente solo se renovaría una vez */
	    counter.renove = counter.interval;	
	}
    /* Comprobamos si tiene aun tiempo */
    if (counter.time <= 0)
	kill (pid, SIGTERM);	/* Si nos pasamos FIN!!! :P */
    alarm(TICTAC);
}
