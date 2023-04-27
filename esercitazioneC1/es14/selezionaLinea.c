#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char ** argv){
//controllo sui parametri
	//devono essere passati 2 parametri
	if(argc!=3){
	printf("errore: devono essere passati 2 parametri, non %d\n", argc-1);
	exit (1);}

	//il primo parametro deve essere un file leggibile, F
	int F;
	if( (F=open(argv[1], O_RDONLY)) < 0){
	printf("Errore: il primo parameto non è un file o non posso aprirlo in lettura\n");
	exit (2);}

	//il secondo parametro deve essere un numero intero strett positivo, n
	int n=atoi(argv[2]);
	double check=atof(argv[2]);
	if((check-n)>0 || n <=0){
	printf("Errore: il secondo carattere non è un numero intero o non è strettamente positivo\n");
	exit (3);}

//svolgo il programma
int nl=1;
char buffer[256];
int j=0;

while(read (F, &buffer[j], 1) !=0 ){	//mi fermo dove devo iniziare a leggere
	if(buffer[j]=='\n') ++nl;
	if(nl==n) break;
}

if(nl==n){				//se la linea c'è
	while(read (F, &buffer[j], 1) !=0 ){
	if(buffer[j]=='\n') ++nl;
	if(nl>n) break;
	++j;
	}
	++j; buffer[j]=0;
	write(1, &buffer,j+1);
	exit (0);
}

if(nl< n){
printf("Il file non è lungo %d righe\n", n);
exit (0);
}

}
