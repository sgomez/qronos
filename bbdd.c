/*
 * $Log: bbdd.c,v $
 * Revision 1.8  1999/04/18 13:17:12  sergio
 * Extraido el código que se encargaba del fichero de configuración. Errores
 * menores corregidos.
 *
 * Revision 1.7  1999/04/14 05:02:01  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.6  1999/04/06 02:08:18  sergio
 * Las entradas caducadas del fichero de registro se eliminan.
 * En el caso de que un programa no deba ser matado, se detecta si se ha
 * pasado de quota para establecer cuando puede volver a ejecutar un
 * programa.
 *
 * Revision 1.5  1999/04/04 23:54:42  sergio
 * El acceso a la bbdd se ha protegido por medio de semáforos
 *
 * Revision 1.4  1999/04/04 14:00:42  sergio
 * Modificada la estructura del módulo. Añadidos parse_entry y parse_conf.
 * Se han creado o modificado las estructuras entr y conf.
 *
 * Revision 1.3  1999/04/04 00:39:09  sergio
 * Acabada la funcion check_bbdd que mira en el registro si un fichero 
 * puede ser ejecutado
 *
 * Revision 1.2  1999/03/30 10:35:17  sergio
 * Finalizada la funcion parse_bbdd que lee el fichero de configuración
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

#if HAVE_CONFIG_H
# include <conf.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <qronos.h>
#include <qronosd.h>
#include <bbdd.h>
#include <check.h>
#include <auth.h>
#include <log.h>

int
start_program (conection conec, program **prog) 
{
    
    program *auth;
    int semid, key;
    struct sembuf sops;

    /* Copiando el semáforo mutex */
    key = ftok (CONF_FILE, 0);
    if (key==-1) return -1;
    semid = semget (key, 1, 0700);
    if (semid==-1) return -1;

    /* Buscamos los permisos para este usuario */
    auth=check_auth(conec);

    /* wait(mutex) */
    sops.sem_flg=0;
    sops.sem_num=MUTEX;
    sops.sem_op=WAIT;
    semop(semid, &sops,1);
	
    /* Miramos que clase de permisos tiene */
    if (auth->timeleft.time!=AUTH_NO_AUTH)
	*prog=check_bbdd(conec, auth);

    /* signal (mutex) */
    sops.sem_num=MUTEX;
    sops.sem_op=SIGNAL;
    semop(semid, &sops,1);

    return 0;    
}
    
void
stop_program (program prog) 
{   
    int fdlog, fdbak;
    entry *line;
    char buffer[512];
    int semid, key;
    struct sembuf sops;
    time_t t_act=time(NULL), t_acum=0;

    /* Copiando el semáforo mutex */
    key = ftok (CONF_FILE, 0);
    semid = semget (key, 1, 0700);

    /* wait(mutes) */
    sops.sem_flg=0;
    sops.sem_num=MUTEX;
    sops.sem_op=WAIT;
    semop(semid, &sops,1);

    fdlog=open(LOG_FILE, O_RDONLY);
    if (fdlog==-1)
	{
	    // error
	}
    fdbak=open(LOG_FILE".bak", O_CREAT|O_WRONLY|O_TRUNC,S_IREAD|S_IWRITE);
    if (fdbak==-1)
	{
	    // error
	}

    /* buscamos la entrada del programa */
    while((line=read_entry(fdlog))!=NULL) 
	{

	    /* Si la línea contiene información caducada se ignora */
	    if (!strcmp(line->user, prog.user) && 
		!strcmp(line->prog, prog.prog) && 
		((t_act - line->begin) > prog.timeleft.interval) &&
		line->end)
		continue;

	    if (line->id == prog.id) 
		line->end=line->begin+prog.timeleft.time;

	    /* Para el caso de que el programa no tenía que ser interrumpido */
	    if (!prog.sigint) 
		{
		    if (!strcmp(line->user, prog.user) && 
			!strcmp(line->prog, prog.prog))
			t_acum+=line->end-line->begin;
	    
		    if (line->id == prog.id && t_acum >= prog.timeleft.quota) 
			line->begin=line->begin+prog.timeleft.interval-
			    prog.timeleft.quota;
		}

	    sprintf(buffer,"%d:%s:%s:%s:%d:%d\n",
		    line->id,
		    line->user,
		    line->prog,
		    line->host,
		    (int)line->begin,
		    (int)line->end
		    );
	    /* actualizamos los valores */
		write(fdbak, buffer, strlen(buffer));
	}
    close(fdlog);
    close(fdbak);
    unlink(LOG_FILE);
    rename(LOG_FILE".bak",LOG_FILE);

    /* signal (mutex) */
    sops.sem_num=MUTEX;
    sops.sem_op=SIGNAL;
    semop(semid, &sops,1);
}

program *
check_bbdd(conection conec, program *auth)
{
    int fd;
    char buffer[512];
    entry *line;
    int renove=0;
    time_t t_cons=0,t_act=time(NULL);
    program *prog=(program *)malloc(sizeof(program));

    *prog=*auth;
    prog->user=conec.username;


    if ((fd=open(LOG_FILE, O_RDONLY|O_CREAT, S_IREAD|S_IWRITE))==-1)
	{
	    log("Error al abrir el log.");
	}
    /* Miramos línea a línea */
    while((line=read_entry(fd))!=NULL) 
	{	    
	    /* Miramos si esta línea pertenece al usuario y programa adecuado*/
	    if (!strcmp(line->user, conec.username) &&
		!strcmp(line->prog, auth->prog))
		{
		    /* Si no tiene valor de finalización 
		       es que no puede ejecutar más copias */
		    if (!line->end) 
			{
			    prog->timeleft.time=AUTH_NO_MORE;
			    prog->timeleft.renove=0;
			    return prog;
			}
		    /* Averiguamos cuando puede renovar la quota */
		    if (!renove) 
			renove=auth->timeleft.interval+line->begin-t_act;
		    /* Si el valor es negativo esta línea no se tiene
		       en cuenta. */
		    if (renove<=0) 
			{
			    renove=0; 
			    continue;
			}
		    /* Contamos el tiempo consumido */
		    t_cons+=line->end-line->begin;
		    
		    /* Sin el tiempo consumido es mayor que la
		       quota no se le permite seguir */
		    if (t_cons >= auth->timeleft.quota)
			{
			    prog->timeleft.time=AUTH_NO_TIME;
			    prog->timeleft.renove=renove;
			    return prog;
			}
		}
	}

    /* Le indicamos el tiempo del que dispone */
    prog->timeleft.time=auth->timeleft.quota-t_cons;
    prog->timeleft.renove=renove?renove:auth->timeleft.interval;

    close(fd);
    if ((fd=open(LOG_FILE, O_WRONLY|O_APPEND|O_SYNC))==-1) {
	// Error
    }

    sprintf(buffer,"%d:%s:%s:%s:%d:0\n",
	    prog->id,
	    conec.username,
	    auth->prog,
	    conec.hostname,
	    (int)t_act
	    );
    write(fd,buffer,strlen(buffer));
    close(fd);

    return prog;
}

entry *
read_entry (int fd)
{
    char buffer[512], c[1];
    int i=0;
    entry *stuff=(entry *)malloc(sizeof(entry));
    char delimiter[2]=":";
    char *msg;
    msg=buffer;

    while(read(fd,c, 1) && ((char) c[0] != (char) '\n'))
	buffer[i++]=*c;

    buffer[i]='\0';
    if (!i) return NULL;
    
    stuff->id=atoi(strtok(msg, delimiter));
    stuff->user=strdup(strtok(NULL, delimiter));
    stuff->prog=strdup(strtok(NULL, delimiter));
    stuff->host=strdup(strtok(NULL, delimiter));
    stuff->begin=(time_t)atoi(strtok(NULL, delimiter));
    stuff->end=(time_t)atoi(strtok(NULL, delimiter));

    return stuff;
}
