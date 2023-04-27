#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

//calcola un numero random compreso tra 0 e n-1
int mia_random(int n){
int casuale;

srand(time(NULL));
casuale=rand()%n;
return casuale;
}

int main(int argc, char ** argv){
int pid;			//per fork
int status, ritorno;	//per wait
	printf("pid del processo corrente: %d\n", getpid());
	if( (pid=fork()) < 0){
	printf("Errore: fork è fallita\n");
	}

	if(pid==0){
	printf("Il pid del processo figlio(il mio pid) è %d\n", getpid());
	printf("Il pid del processo padre è %d\n", getppid());
	exit(mia_random(100));
	}

pid =wait(&status);
 	//controllo se il figlio è terminato
	if(pid < 0){
	printf("Errore: problemi nella wait\n");
	exit (2);
	}

	else{
	printf("Terminato processo figlio con pid %d\n", pid);
	}

	//controllo se il figlio è terminato in modo anomalo
	if( (status & 0xFF) !=0){
	printf("Figlio terminato in modo involontario");
	}
	else{
	ritorno= status >> 8;
	ritorno &= 0xFF;
	printf("Figlio terminato con lo stato di EXIT %d\n", ritorno);
	}
exit (0);
}


