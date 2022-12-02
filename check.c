/*
 * $Log: check.c,v $
 * Revision 1.1  1999/04/18 13:00:22  sergio
 * División de bbdd.c, módulo encargado de leer el fichero de configuración
 * y establecer los permisos para la ejecución y la quota de tiempo.
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

#include <fnmatch.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/stat.h>
#include <fcntl.h>  
#include <stdio.h>
#include <qronos.h>
#include <qronosd.h>
#include <exec.h>
#include <check.h>
#include <auth.h>

program *
check_auth (conection conec)
{
    program *auth;
    int fd, fd_list, i;
    conf *line;
    char *token, *cad;
    const char delimiter[2]=",";
    int locate=0;
    char list[512];

    auth=(program *)malloc(sizeof(program));
    if (auth==NULL) return NULL;

    auth->id=conec.id;
    auth->user=conec.username;
    auth->prog=conec.progname;
    auth->timeleft.time=AUTH_NO_AUTH;

    fd=open(CONF_FILE, O_RDONLY);
    if (fd==-1)	return NULL;

    /* reading the configuration file */
    while((line=read_conf(fd))!=NULL) 
	{
	    /* 1st field: user */
	    cad=line->users;	    
	    token=strtok(cad, delimiter);
	    if (token==NULL) continue;
	    do 
		{
		    switch((char) token[0])
			{
			case '|': /* file */
			    sprintf(list,"%s/etc/%s",PATH_QRONOS,token+1);
			    fd_list=open(list, O_RDONLY); 
			    if (fd_list==-1) continue;
			    do 
				{
				    i=0;
				    while(read(fd_list,(char *)list+i, 1) &&
					  ((char) list[i] != (char) '\n') &&
					  (i < 510)) i++;
				    list[i]='\0';
				    locate=!fnmatch(list, conec.username,0);
				} 
			    while(i && !locate);
			    close(fd_list);			
			    break;
			case '%': /* group */
			    locate=!fnmatch(token+1, conec.groupname, 0);
			    break;
			default: /* user */
			    locate=!fnmatch(token, conec.username, 0);
			    break;
			}
		} 
	    while ((token=strtok(NULL, delimiter))!=NULL && !locate);
	    
	    if (!locate) continue;
	    locate=0;

	    /* 2nd field: program */
	    cad=line->programs;	    
	    token=strtok(cad, delimiter);
	    if (token==NULL) continue;
	    do 
		locate=!fnmatch(token, conec.progname, 0);
	    while ((token=strtok(NULL, delimiter))!=NULL && !locate);

	    if (!locate) continue;
	    locate=0;

	    /* 3th field: host */
	    cad=line->hosts;	    
	    token=strtok(cad, delimiter);
	    if (token==NULL) continue;
	    do 
		locate=!fnmatch(token, conec.hostname, 0);
	    while ((token=strtok(NULL, delimiter))!=NULL && !locate);

	    if (!locate) continue;

	    /* the user, program and host are defined correctly,
	       we return the time quota */

	    auth->timeleft.time=AUTH_OK;
	    auth->timeleft.quota=line->quota;
	    auth->timeleft.interval=line->interval;
	    auth->sigint=line->sigint;
	}
    close(fd);
    return auth;
}


conf *
read_conf (int fd)
{
    char buffer[512], c[1];
    int i=0;
    conf *stuff=(conf *)malloc(sizeof(conf));
    const char delimiter[2]="\t";
    char *msg, *token;
    msg=buffer;

    /* reading a config file line */
    while(read(fd,c, 1) && ((char) c[0] != (char) '\n') && (i < 510))
	buffer[i++]=*c;

    buffer[i]='\0';
    if (!i) return NULL;
    
    /* parsing the line */
    token=strdup(strtok(msg, delimiter));
    if (token==NULL) token=strdup("nobody");
    stuff->users=token;
    token=strdup(strtok(NULL, delimiter));
    if (token==NULL) token=strdup("nogroup");
    stuff->programs=token;
    token=strdup(strtok(NULL, delimiter));
    if (token==NULL) token=strdup("nohost");
    stuff->hosts=token;
    token=strdup(strtok(NULL, delimiter));
    if (token==NULL) token=strdup("0");
    stuff->quota=(time_t)atoi(token)*60;
    token=strdup(strtok(NULL, delimiter));
    if (token==NULL) token=strdup("0");
    stuff->interval=(time_t)atoi(token)*60;
    token=strdup(strtok(NULL, delimiter));
    if (token==NULL) token=strdup("1");
    stuff->sigint=atoi(token);

    return stuff;
}

