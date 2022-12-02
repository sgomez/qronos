/*
 * $Log: qronosd.c,v $
 * Revision 1.7  1999/04/18 13:30:13  sergio
 * Correciones menores en el código y en la captura de errores.
 *
 * Revision 1.6  1999/04/14 05:02:02  sergio
 * Modificado el copyright a partir de la versión 0.8
 *
 * Revision 1.5  1999/04/06 02:09:17  sergio
 * Adaptadas las funciones de comunicaciones al estandar POSIX
 *
 * Revision 1.4  1999/04/04 23:58:16  sergio
 * Insertadas rutinas para la creación de un grupo de semáforos.
 *
 * Revision 1.3  1999/04/04 14:09:33  sergio
 * Corregidos errores leves de compilación. Corregido el momento en el
 * que se incrementa el identificador de conexión.
 *
 * Revision 1.2  1999/03/29 23:42:51  sergio
 * Version final de la funcion server
 *
 * Revision 1.1  1999/03/29 21:31:04  sergio
 * Versión inicial del demonio de qronos
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

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ident.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <qronosd.h>
#include <qronos.h>
#include <msg.h>
#include <auth.h>
#include <log.h>
#include <bbdd.h>
#include <check.h>


void main(int argc, char *argv[]) {
    int ls;   /*Identificador del socket de recepción de peticiones*/
    int s;    /*Identificador del socket de servicio*/
    int addrlen;
    int id;   /*Identificador de la petición*/
    struct sockaddr_in myaddr_in; 
    int semid;
    int key;
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
    /* union semun is defined by including <sys/sem.h> */
#else
    /* according to X/OPEN we have to define it ourselves */
    union semun {
	int val;                    /* value for SETVAL */
	struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
	unsigned short int *array;  /* array for GETALL, SETALL */
	struct seminfo *__buf;      /* buffer for IPC_INFO */
    };
#endif
     
    /* Creando el semáforo */
    if ((key = ftok (CONF_FILE, 0))==-1) 
	{
	    perror(argv[0]);
	    fprintf(stderr, "%s: error in ftok\n", argv[0]);
	    exit(1);
	}
    if ((semid = semget (key, 1, 0700 | IPC_CREAT))==-1)
	{
	    perror(argv[0]);
	    fprintf(stderr, "%s: unable to create the semaphore\n", argv[0]);
	    exit(1);
	}

    /* Iniciando el semáforo a abierto*/
    if (semctl (semid, MUTEX, SETVAL, (union semun) 1)==-1)
	{
	    perror(argv[0]);
	    fprintf(stderr, "%s: error in ftok\n", argv[0]);
	    exit(1);
	}

    /* Definimos el tipo de conexión */
    myaddr_in.sin_family=AF_INET; 
    myaddr_in.sin_addr.s_addr=INADDR_ANY;
    myaddr_in.sin_port=htons(PORT);

    /* Configuramos y preparamos el socket */
    ls = socket (AF_INET, SOCK_STREAM, 0);
    if (ls == -1) {
	perror(argv[0]);
	fprintf(stderr, "%s: unable to create a socket\n", argv[0]);
	exit(1);
    }
  
    if(bind(ls,&myaddr_in,sizeof(struct sockaddr_in))<0) {
	perror(argv[0]);
	fprintf(stderr,"%s: unable to connect\n",argv[0]);
	exit(0);
    }

    if (listen(ls, 5) == -1) {
	perror(argv[0]);
	fprintf(stderr, "%s: unable to listen on socket\n", argv[0]);
	exit(1);
    }

    /* Eliminamos el vínculo con el terminal */
    setpgid(0,0);

    id=0;

    /* Se inicia el demonio propiamente dicho */

    switch(fork()) {
    case -1:
	perror(argv[0]);
	fprintf(stderr, "%s: unable to fork daemon\n", argv[0]);
	exit(1);
    case 0:
	close((int)stdin);
	close((int)stderr);
	signal(SIGCLD, SIG_IGN);
	addrlen=sizeof(struct sockaddr_in);

	for (;;) {
	    s=accept(ls, &myaddr_in, &addrlen);
	    if (s==-1) exit(1);
	    id++;
	    switch(fork()) {
	    case -1:
		exit(1);
	    case 0:
		close(ls);
		server(s,myaddr_in, id);
		exit(0);
	    default:
		close(s);
	    }
	}
    default:
	exit(0);
    }
}

void server(int s, struct sockaddr_in peeraddr_in, int id)
{
    char buffer[512];
    char *msg;
    struct hostent *hp;
    conection conec;	
    program *prog;
    struct passwd *pw_user;
    struct group *gr_user;

    msg=buffer;

    /* Determinamos el nombre del host que se ha conectado */
    hp=gethostbyaddr((char *)&peeraddr_in.sin_addr,
		     sizeof(struct in_addr),
		     peeraddr_in.sin_family);

    if (hp) conec.hostname = hp->h_name;
    else conec.hostname = (char *)inet_ntoa(peeraddr_in.sin_addr);
    
    /* Determinamos el nombre del usuario que se conecta */
    if ((conec.username = ident_id(s,0))) 
	{

	    /* Determinamos su grupo */
	    if ((pw_user = getpwnam(conec.username))!=NULL) 
		{
		    gr_user=getgrgid(pw_user->pw_gid);
		    conec.groupname=gr_user->gr_name;
		}
	    else
		conec.groupname=NULL;
    	    
	    /* Miramos lo que nos envía */
	    read(s, &buffer, sizeof(buffer));
	    prog=unpackmsg(buffer);
	    conec.progname=prog->prog;
	    
	    /* Si el usuario no es qronos, se está abriendo un prog */
	    if (strcmp(conec.username,"qronos")) 
		{
		    conec.id=id++;
		    start_program(conec,&prog);
		}
	    else /* si es qronos se está cerrando */
		{
		    stop_program(*prog);
		}	
	    msg=packmsg(*prog);
	}
    else /* Si no se ha podido determinar devolvemos un error */
	{
	    msg="-1";
	}
    write(s, msg, strlen(msg)+1);
    close(s);
}
