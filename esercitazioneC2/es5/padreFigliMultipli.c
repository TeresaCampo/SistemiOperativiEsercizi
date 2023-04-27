#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **  argv){
//deve avere 1 parametro
if(argc!=2){
printf("Errore: deve essere passato 1 parametro, non %d", argc-1);
exit (1);
}

int N=atoi(argv[1]);
double check=atof(argv[1]);
if(check-N>0){
printf("Errore: il parmetro deve essere un numero intero\n");
exit (2);
}

if(N<=0 || N>=255){
printf("Errore: il parametro deve essere un numero compreso tra 1 e 254\n");
exit (2);
}

//pid processo corrente
int pid;
int pidFiglio, status, ritorno;
printf("Il pid del processo corrente è %d e il numero passato è %d\n", getpid(), N);

//crea i processi figli
for(int i=1; i<=N; ++i){
pid=fork();
	if( pid<0){
	printf("Errore: non è stato creato il processo numero %d\n", i);
	exit (3);
	}
	if(pid==0){
	//codice eseguito dal figlio numero i
	printf("Processo figlio numero %d ha pid %d\n", i, getpid());
	exit (i);
	}
}
//aspetta i processi figli
for(int i=1; i<=N; ++i){
pidFiglio=wait(&status);
	if( pidFiglio < 0){
	printf("Errore: problema nella wait\n");
	exit (4);
	}
	if( (status & 0xFF) != 0){
	printf("Figlio con pid %d ha terminato in modo anomalo\n", pidFiglio);
	}
	else{
	ritorno=(int)((status >> 8) & 0xFF);
	printf("Figlio con pid %d ha terminato con valore di ritorno %d\n", pidFiglio, ritorno); 
	}
}

exit (0);
}
