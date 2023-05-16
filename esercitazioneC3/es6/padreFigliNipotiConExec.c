#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define PERMESSI 0666

int main(int argc, char ** argv){
//controllo il numero di parametri N: N maggiore o uguale a 3
int N=argc-1;
if(N<3) {
	printf("Errore: devono essere passati almeno 3 parametri, non %d\n", N);
	exit (1);
}

//il processo padre crea N processi figli
int pidFiglio, pidNipote, fdSort, status, ritorno;
char *nomeFile;

for(int i=0; i<N; ++i){
	//il processo padre crea un processo figlio
	pidFiglio=fork();
		if(pidFiglio<0){ //se fallisce la fork
		printf("Errore: problemi nella fork\n");
		exit (2);
		}

		if(pidFiglio==0){ //se il processo figlio è creato esegue il codice---------- 
		nomeFile= (char *) malloc(strlen(argv[i+1])+6);
		nomeFile=strcat(nomeFile, argv[i+1]);
		nomeFile=strcat(nomeFile,  ".sort");
		fdSort=creat(nomeFile, PERMESSI);	//crea il file e controllo l'effettiva creazione
			if(fdSort<0){
			printf("Errore: problemi nella creazione del file %s\n", nomeFile);
			exit (3);
			}
		printf("Creato il file %s\n", nomeFile);

		pidNipote= fork();			//crea processo figlio, processo nipote dal punto di vista iniziale
			if(pidNipote<0){ //se falllisce la fork
			printf("Errore: problemi nella fork\n");
	        	exit (4);
			}

			if(pidNipote==0){ //se il processo nipote è creato esegue il codice++++
			printf("Provo a eseguire il comando sort < %s > %s\n", argv[i+1],nomeFile);
			close(0); 				//ridireziona input: nome file da ordinare
				if( open(argv[i+1], O_RDONLY)<0){
				printf("Errore: problemi nell'apertura del file %s passato come parametro\n", argv[i+1]);
				exit (5);
				}
			close(fdSort); 				//ridireziona output: file.sort
			close(1);
				if( open(nomeFile, O_WRONLY)<0){
				exit (5);
				}
			execlp("sort", "sort", (char *) 0);	//provo a fare exec, il nuovo programma eseguirebbe il comando filtro sort e restituirebbe il valore di ritorno della sort

			exit (-1); 				//se fallisce la exec ritorno -1 al processo che per noi è il figlio
		}//in ogni caso qui è terminato il processo nipote+++++++++++++++++++++++++++++

	//di nuovo codice del processo figlio--------------------------------------------------
	pidNipote=wait(&status);			//processo figlio aspetta il processo nipote
		if(pidNipote<0){
		printf("Errore nella wait di un processo nipote\n");
		exit (6);
		}
		if( (status & 0xFF) !=0){
		printf("Processo nipote terminato in modo anomalo\n");
		exit (7);
		}
		else{
		ritorno=(int) ((status>>8) & 0xFF);
		exit (ritorno);				//ritorno il valore di ritorno del processo nipote al processo padre
		}
	//fine processo figlio--------------------------------------------------------
	}
}

//tutti gli N processi figi sono stati creati, ora il processo padre fa wait e riporta il valore ritornato da ogni figlio e il suo pid
for(int i=0; i<N; ++i){
pidFiglio=wait(&status);
	if(pidFiglio<0){
         printf("Errore nella wait di un processo figlio\n");
         exit (8);
         }
         if( (status & 0xFF) !=0){
         printf("Processo figlio con pid %d terminato in modo anomalo\n", pidFiglio);
         exit (9);
         }
         else{
         ritorno=(int) ((status>>8) & 0xFF);
         printf("Processo figlio con pid %d ha ritornoato %d\n", pidFiglio, ritorno);
         }
}

exit (0);
}
