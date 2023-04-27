#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char ** argv){

	//devono essere passati esattamente 2 parametri
	if(argc!=3){
	printf("Errore: devono essere passati esattamente 2 parametri, non %d\n", argc-1);
	exit (1);
	}

	//il primo parametro deve essere un file con diritto di lettura e scrittura, F
	int F=open(argv[1], O_RDWR);
	if(F<0){
	printf("Errore: il primo parametro non è un file oppure non ho diritto di lettura e scrittura\n");
	exit (2);
	}

	//il secondo parametro deve essere un singolo carattere, Cx
	size_t lenCar=strlen(argv[2]);
	if(lenCar != 1){
	printf("Errore: il secondo parametro non è un singolo carattere\n");
	}
	char Cx=*argv[2];
	
//arrivato qui i parametri sono ok
int nread;
int nwrite;
char letto;
char spazio=' ';

while( (nread=read(F, &letto, 1))==1){
	if(letto==Cx){
	lseek(F, -1, SEEK_CUR);
		if( (nwrite=write(F,&spazio , 1))!=1){
		printf("Errore: problemi durante la sostituzione del carattere\n");
		exit (3);
		}
	}
}

exit (0);
}
