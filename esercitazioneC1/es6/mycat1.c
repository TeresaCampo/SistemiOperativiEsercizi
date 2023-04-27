#include <stdlib.h>	//exit
#include <unistd.h>	//close
#include <fcntl.h>	//FLAGS open
#include <stdio.h>	//printf

int main(int argc, char ** argv){
//servono per ogni scrittura, per non scrivere un carattere alla volta
int nread;
char buffer[BUFSIZ];
int N=argc-1;

//se ha 0 parametri, leggo da input e scrivo su otuput
int fdIn=0;
int fdOut=1;
if(N==0){
	while( (nread=read(fdIn, buffer, BUFSIZ)) > 0){
                write(fdOut, buffer, nread);
	}
}

//se ha almeno 1 parametro, leggo da parametro e scrivo su output
//mentre apro file controllo che il parametro sia un file
else{
	for(int i=0; i < N; i++){
		fdIn=open(argv[i+1], O_RDONLY);
			//controllo che ogni parametro sia effettivamente un file leggibile
			if(fdIn<0) {
			printf("Errore: il parametro numero %d non è un file o non ho diritti di lettura", i+1);
			exit (1);
			}
	
		while( (nread=read(fdIn, buffer, BUFSIZ)) > 0){
			write(fdOut, buffer, nread);
		}
	}
}

exit (0);
}
