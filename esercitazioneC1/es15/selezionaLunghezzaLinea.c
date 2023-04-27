#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char ** argv){
//controllo i parametri
	//2 parametri
	if(argc!=3){
	printf("Errore: il numero di parametri deve essere 2, non %d", argc - 1);
	exit (1);
	}

	//il primo parametro deve essere un file con diritto di lettura
	int F;
	if( (F=open(argv[1], O_RDONLY))< 0) {
	printf("Errore: il primo parametro non è un file leggibile");
	exit (2);
	}

	//il secondo parametro deve esser eun numero intero strettamente positivo
	int n=atoi(argv[2]);
	double check=atof(argv[2]);
	if( (check-n)>0 || n<0){
	printf("Errore: il secondo parametro deve essere un numero intero strettamente positivo");
	exit (3);
	}

//eseguo il programma
char buffer[n];
char intestazione[BUFSIZ];

int nl=1;
int trovate=0;
int j=0; 

while( (read(F, &buffer[j], 1))!=0 ){
	if(buffer[j]=='\n'){
		if((j+1)==n){
		++trovate;
		sprintf(intestazione, "Linea %d:", nl);
		write(1, intestazione, strlen(intestazione));
		write(1, buffer, j+1);}
	++nl; j=-1;
	}
++j;
}

if(trovate==0){
printf("Non sono state trovate linee lunghe %d\n", n);
}

exit (0);
}
