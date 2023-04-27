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
int len=lseek(F, 0, SEEK_END);
char buff;
int nread;

for(int i=1; i*n<=len; ++i){
	lseek(F,n*i-1, SEEK_SET);
	if( (nread=read(F, &buff, 1))==1) {
		printf("Il carattere multiplo %d-esimo all'interno del file %s è %c\n", i*n, argv[0], buff);}
	else {
	printf("Errore: errore nella lettura del file"); 
	exit (4);}
}

exit (0);
}
