/* 
 * $Log: qronos.h,v $
 * Revision 1.8  1999/04/14 05:02:02  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.7  1999/04/06 02:14:11  sergio
 * Modificada la estructura program. Añadidos el nombre del usuario
 * y valor booleano que indica si el programa debe ser interrumpido
 * al final de la quota.
 *
 * Revision 1.6  1999/04/05 23:40:39  sergio
 * Modificación de las rutas de los includes.
 *
 * Revision 1.5  1999/03/29 23:39:29  sergio
 * *** empty log message ***
 *
 * Revision 1.4  1999/03/28 19:38:55  sergio
 * Breves modificaciones en el código para optimizar el uso de autoheader
 *
 * Revision 1.1 1998/12/23 16:17:05  sergio
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

#if !defined(QRONOS_H)
#define QRONOS_H

#if HAVE_CONFIG_H
# include <conf.h>
#endif

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

typedef struct {
    time_t time;        /* tiempo de ejecucion restante */
    time_t renove;      /* tiempo que queda para la renovacion */
    time_t quota;       /* quota para ese programa */
    time_t interval;    /* intervalo entre quotas */
} qronos_t;

typedef struct {
    char *user;         /* Nombre del usuario */
    char *prog;         /* Programa a ejecutar */    
    char **argv;        /* Argumentos del programa */
    int id;             /* Identificador */
    qronos_t timeleft;  /* Informacion de la cuota temporal del programa */
    int sigint;         /* Determina si el programa debe ser interrumpido */
} program;

#endif /* QRONOS_H */
