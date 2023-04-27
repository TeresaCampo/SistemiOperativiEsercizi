#include <stdlib.h>	//per exit
#include <unistd.h>	//per close
#include <fcntl.h>	//per FLAGS della open
#include <stdio.h>	//per printf
#include <string.h>	//per strlen

int main(int argc, char ** argv){
//deve avere 2 prametri, quindi argc=3
if(argc!=3) {
	printf("Errore: devono essere passati 2 parametri, non %d\n", argc-1);
	exit (1);
	}

//il primo parametro deve essere il nome di un file F
char * F=argv[1];
int fd;
if( (fd=open(F, O_RDONLY)) <0){
	printf("Errore: il primo parametro non è un file con diritto di lettura\n");
	exit (2);
	}

//il secondo parametro deve essere un singolo carattere Cx
char * Cx=argv[2];
size_t len=strlen(Cx);
if(len!=1){
	printf("Errore: il secondo parametro non è un singolo carattere\n");
	exit (3);
	}

//tutto ok, visualizzo nome file eseguibile e parametri
printf("Il file eseguibile è %s, il primo parametro è il file %s, il secondo parametro è il carattere %s\n", argv[0], F, Cx);

//scorro il file e controllo carattere per carattere se trovo Cx
char letto;
long trovati=0;
int nread;
while( (nread=read(fd, &letto, 1)) > 0 ){
	if(letto==*Cx){
	++trovati;
	}
}

//finito il ciclo ho trovato tutte le occorrenze e mostro il numero 
printf("Il numero di occorrenze di Cx=%s in F=%s è %ld\n", Cx, F, trovati);
close(fd);
}

