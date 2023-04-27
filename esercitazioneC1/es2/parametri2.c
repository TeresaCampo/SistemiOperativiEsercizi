#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

//i numero di parametri deve essere 3, quindi argv=4
int main(int argc,char ** argv){
if(argc!=4){
	printf("Errore: il numero di parametri deve essere 3(argc = 4) ma argv=%d \n", argc);
	}

//il primo parametro deve essere un file, provo ad aprirlo
int file1=open(argv[1], O_RDONLY);
if(file1<0){
	printf("Errore: il primo parametro non è un file \n"); 
	exit (1); //non lo chiudo tanto si chiude automaticamente alla fine del processo
	}

//il secondo parametro deve essere positivo e intero
int N=atoi(argv[2]);
double Nf=atof(argv[2]);

if(N<=0){
	printf("Errore: il secondo parametro deve essere un numero positivo \n");
	exit (2);
	}
if((Nf-N)>0){
	printf("Errore: il secondo parametro deve essere un numero intero \n");
	exit (2);
	}

//il terzo parametro deve essere un singolo carattere C
char * C=argv[3];
int len=strlen(argv[3]);

if(len!=1){
	printf("Errore: il terzo parametro non è un singolo carattere \n");
	exit (3);
	}

printf("Il nome dell'eseguibile è %s \n", argv[0]);
printf("Parametro 1) %s, file con diritto di lettura\nParametro 2) %d, numero intero positivo\nParametro 3) %s, carattere singolo\n", argv[0], N, C);
exit (0); 
}
