#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

//calcola numero random tra 0 e n-1
int mia_random(int n){
srand(time(NULL));
int casuale=rand()%n;
return casuale;
}


int main(int argc, char ** argv){
//controllo sui parametri
	//deve avere un parametro
	if(argc!=2){
	printf("Errore: il numero di parametri deve essere 1, non %d\n", argc-1);
	exit (1);
	}

	//il parametro deve essere un numero intero compreso tra 1 e 154
	int N=atoi(argv[1]);
	double check=atof(argv[1]);
	if( (check-N)>0 ){
	printf("Errore: il parametro deve essere un numero intero\n");
	exit (2);
	}
	if(N<=0 || N>=155){
	printf("Errore: il parametro deve essere un numero intero compreso tra 1 e 154\n");
	exit (2);
	}

//preparo quello che mi serve
int pid;
int status, indice, ritorno;
int *vettorePid= malloc(sizeof(int)*N);
if(vettorePid==NULL){
printf("Errore: problemi nella malloc\n");
exit (3);
}

//il padre riporta il suo pid
printf("Il pid del processo corrente è %d\n", getpid());

for(int i=1; i<=N; ++i){
pid=fork();
vettorePid[i-1]=pid;
	if(pid<0){
	printf("Errore: problemi nella fork\n");
	exit (4);
	}
	if(pid==0){
	printf("Il mio pid è %d, sono il processo con numero d'ordine %d\n", getpid(), i);
	exit (mia_random(100+i+1));
	}
}

for(int i=1; i<=N; ++i){
pid=wait(&status);
	if(pid<0){
	printf("Errore: problemi nella wait\n");
	exit (5);
	}
	if((status & 0xFF)!=0){
	printf("Figlio terminato in modo involontario\n");
	}
	else{
	//cerco il pid nel vettore così recupero il numero do'ordine
		for(int j=0; j<N; ++j){
			if(pid==vettorePid[j]){
			indice=j+1; break;
			}
		}

		ritorno=(int) ((status >> 8) & 0xFF);
		printf("Terminato processo figlio con pid %d, numero d'ordine %d e valore di ritorno %d\n", pid, indice, ritorno);
	}
}

exit (0);
}
