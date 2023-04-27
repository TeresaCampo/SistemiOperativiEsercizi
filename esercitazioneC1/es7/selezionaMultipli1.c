#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char ** argv){
	//deve avere esattamente 2 prametri
	if(argc!=3){
	printf("Errore: devono essere passati esattamente 2 parametri, non %d\n", argc-1);
	exit (1);}

	//il primo parametro deve essere un file leggibile
	int F;
	if( (F=open(argv[1],O_RDONLY)) < 0) {
	printf("Errore: il primo parametro deve essere un file apribile in lettura, %s non lo è\n", argv[1]);
	exit (2);}

	//il secondo parametro deve essere un numero intero strettamente positivo
	int n=atoi(argv[2]);
	double check=atof(argv[2]);
	if( (check-(double) n)>0 || n <= 0){
	printf("Errore: il secondo parametro deve essere un numero intero e strettamente positivo\n");
	exit (3);}

//se arrivo fino a qui tutti i parametri sono ok
char * buffer= malloc(n);
int nread;
int c=1;

while( (nread=read(F, buffer,n)) ==n){
	printf("Il carattere multiplo %d-esimo all'interno del file %s è %c\n", c*n, argv[0], buffer[n-1]);
	++c;}

exit (0);
}
