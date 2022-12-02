/*
 * $Log: bbdd.h,v $
 * Revision 1.6  1999/04/18 13:14:57  sergio
 * Eliminación de la parte del fichero de configuración
 *
 * Revision 1.5  1999/04/14 05:02:02  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.4  1999/04/06 02:16:08  sergio
 * Modificada la estructura conf. En el fichero de configuración ahora
 * se puede indicar si un programa debe ser interrumpido o no.
 *
 * Revision 1.3  1999/04/04 14:01:01  sergio
 * Modificada la estructura del módulo. Añadidos parse_entry y parse_conf.
 * Se han creado o modificado las estructuras entr y conf.
 *
 * Revision 1.2  1999/04/04 00:39:09  sergio
 * Acabada la funcion check_bbdd que mira en el registro si un fichero 
 * puede ser ejecutado
 *
 * Revision 1.1  1999/03/29 23:33:57  sergio
 * Modulo de lectura de la BBDD, version inicial
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

#if !(BBDD_H)
# define BBDD_H

#include <qronos.h>
#include <qronosd.h>
#include <exec.h>

# if !(LOG_FILE)
#  define LOG_FILE PATH_QRONOS"/var/log/qronos.log"
# endif

typedef struct {
    int id;
    char *user;
    char *prog;
    char *host;
    time_t begin;
    time_t end;
} entry;

int start_program(conection conec, program **prog);
void stop_program(program prog);
program *check_bbdd(conection conec, program *prog);
entry *read_entry (int fd);

#endif /* BBDD_H */
