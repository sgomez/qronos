/* 
 * $Log: msg.h,v $
 * Revision 1.4  1999/04/14 05:02:02  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.3  1999/04/06 02:14:35  sergio
 * Corregida la ruta de los includes
 *
 * Revision 1.2  1999/03/28 19:38:55  sergio
 * Breves modificaciones en el código para optimizar el uso de autoheader
 *
 * Revision 1.1 1999/02/18 11:43:47  sergio
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

#if !defined(MSG_H)
#define MSG_H

#include <qronos.h>

#define DELIMITER ":"

char *packmsg( program foo );
program *unpackmsg( char *msg );

#endif
    
