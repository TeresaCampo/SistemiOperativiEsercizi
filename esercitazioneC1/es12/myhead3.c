#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv){
//controllo parametri
	//deve avere un soloparametro
	if(argc>3){
	printf("Errore: devono essere passati 0, 1 o 2 parametri, non %d\n", argc-1);
	exit (1);
	}

	//il num di default è 10
	int num=10;

//se c'è un parametro controllo se è un file, se non lo è controllo se è un numero
int F=-1;
if(argc==2){
	if( (F=open(argv[1], O_RDONLY)) < 0){		//se non è un file deve essere un numero
		if(argv[1][0]!='-'){
		printf("Errore: il primo parametro deve essere o un file o un numero in forma -n\n");
		exit (2);}
		else{
		char *n=&argv[1][1];
		num=atoi(n);
			if(num<=0){
			printf("Errore: il primo parametro deve essere in forma -n, dove n deve essere un numero strettamente positivo\n");
			exit (3);}
		}
	}
}

if(argc==3){
	if(argv[1][0]!='-'){
        printf("Errore: il primo parametro deve essere un numero in forma -n\n");
        exit (3);}
        else{
        char *n=&argv[1][1];
        num=atoi(n);
        	if(num<=0){
                printf("Errore: il primo parametro deve essere in forma -n, dove n deve essere un numero strettamente positivo\n");
              	exit (3);}

		if( (F=open(argv[2],O_RDONLY)) < 0){
		printf("Errore: il secondo parametro deve essere il nome di un file leggibile");
		exit (4);}
	}
}

//arrivato qui i parametri sono giusti
int counter=1;
char buf;
int stdIn=0;
if(F>0){stdIn=F;}

while(read(stdIn, &buf, 1) ==1){ 
      	 write(1, &buf, 1);
	 if(buf=='\n') {++counter;}
	if(counter==num+1) break;
}

exit (0);
}
