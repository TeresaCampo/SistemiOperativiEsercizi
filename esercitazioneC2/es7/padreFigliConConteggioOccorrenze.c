#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char ** argv){
int N;
//almeno 3 parametri
if(argc<4){
printf("Errore: il numero di parametri deve essere almeno 3, non %d\n", argc-1);
exit (1);
}
N=argc-2;
 
//l'ultimo parametro deve essere un singolo carattere
if(strlen(argv[N+1])!=1){
printf("Errore: l'ultimo parametro deve essere un singolo carattere\n");
exit (2);
}
char Cx=*argv[N+1];


//creo i processi figli
int pid, status, ritorno;
int fd, trovati;
char letto;

for(int i=0; i<N; ++i){
pid=fork();
	if(pid<0){
	printf("Errore: problemi nella fork\n");
	exit (3);
	}
	if(pid==0){
		if( (fd=open(argv[i+1], O_RDONLY)) <0){
		printf("Errore: il parametro numero %d non è un file apribile in lettura\n", i+1);
		exit (-1);
		}

		trovati=0;
		while( read(fd, &letto, 1)==1){
		if(letto==Cx){++trovati;}
		}
		exit (trovati);
	}
}

for(int i=0; i<N; ++i){
pid=wait(&status);
	if(pid<0){
	printf("Errore: problemi nella wait\n");
	exit (4);
	}
	if( (status & 0xFF)!=0 ){
	printf("Il processo figlio con pid %d è terminato in modo involontario\n", pid);
	}
	else{
	ritorno=(int) ((status >> 8) &0xFF);
	printf("Il processo figlio con pid %d ha ritornato %d -->se ha ritornato 255 significa che il file non esisteva o non era leggibile\n", pid, ritorno);
	}
}

exit (0);
}
