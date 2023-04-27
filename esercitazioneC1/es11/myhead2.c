#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv){
//controllo parametri
	//deve avere un soloparametro
	if(argc>2){
	printf("Errore: deve essere passato un solo parametro oppure nessuno, non %d\n", argc-1);
	exit (1);
	}

	//il parametro deve essere -n, con n numero
	int num=10;
if(argc==2){
	if(argv[1][0]!='-'){
	printf("Errore: il parametro deve essere in forma -n\n");
	exit (2);
	}
	else{
	char *n=&argv[1][1];
	num=atoi(n);
		if(num<=0){
		printf("Errore: il primo parametro deve essere in forma -n, dove n deve essere un numero strettamente positivo\n");
		exit (3);
		}
	}
}
//arrivato qui i parametri sono giusti
int counter=1;
char buf;

while(read(0, &buf, 1) ==1){ 
      	 write(1, &buf, 1);
	 if(buf=='\n') {++counter;}
	if(counter==num+1) break;
}

exit(0);
}
