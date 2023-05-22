#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char ** argv){
int N; 									//per contenere il numero di file
int piped[2][2];						//per contenere 2 pipe
int fdFile;								//per contenere i fd dei parametri, quando li apro nei figli
char letto;								//per contenter ultimo valore letto da un figlio
char chPari, chDispari;					//per contenere i valori letti dalle due pipe
int nrPari, nrDispari, tot;				//per contenere i numeri di valori letti dalle due pipe
int pidFiglio, status, ritorno;			//usati nella fork e nella wait

//numero parametri: almeno 2 parametri
if(argc<3){
printf("Errore: numero di parametri deve essere almeno 2, invece %d\n", argc-1);
exit (1);
}
N=argc-1;

//il processo padre crea 2 pipe: piped[0] per file pari, piped[1] per file dispari
if(pipe(piped[0])<0){
	printf("Errore: creazione pipe fallita\n");
	exit (2);
}
if(pipe(piped[1])<0){
        printf("Errore: creazione pipe fallita\n");
        exit (3);
}

//il processo padre crea N processi figli
for(int i=0; i<N; ++i){
	pidFiglio=fork();
	if(pidFiglio<0){
	printf("Errore: fork fallita\n");
	exit (4);
	}
	if(pidFiglio==0){
		//printf("Sono il processo figlio con pd %d\n", getpid());
		//controllo che il parametro sia un file leggibile
		fdFile=open(argv[i+1], O_RDONLY);
		if( fdFile<0){
		printf("Errore: il parametro %s non è un file apribile in lettura\n", argv[i+1]);
		exit (-1);
		}

		//chiudo il lato di lettura della pipe
		close(piped[0][0]); close(piped[1][0]);

		//se file dispari, uso piped[1]
		if((i%2)==0){ 
			close(piped[0][1]); 
			//deve scrivere sulla pipe solo i caratteri alfabetici
			while(read(fdFile, &letto, 1)){
				if(isalpha(letto)!=0){
				write(piped[1][1], &letto,1);}
			}
		}
		else{ //file pari, quindi usa piped[0]
			close(piped[1][1]);
			//deve scrivere sulla pipe solo i caratteri numerici
			while(read(fdFile, &letto, 1)){
                if(isalnum(letto)!=0){
                write(piped[0][1], &letto,1);}
			}         	
			}
		exit(letto);
	}
}

//il padre chiude i lati di scrittura sulle pipe
close(piped[0][1]); close(piped[1][1]);
nrPari=read(piped[0][0], &chPari, 1);
nrDispari=read(piped[1][0], &chDispari, 1);
while(nrPari!=0 || nrDispari!=0){
	write(1 ,&chDispari, 1);
	write(1 ,&chPari, 1);
	nrPari=read(piped[0][0], &chPari, 1);
	nrDispari=read(piped[1][0], &chDispari, 1);
	tot=tot+nrPari+nrDispari;
}
printf("\nIl numero totale di caratteri scritti sullo standard output è %d\n", tot);

//il padre aspetta i figli
for(int i=0; i< N; ++i){
	pidFiglio=wait(&status);
	/* padre aspetta il figlio */
        if ((pidFiglio=wait(&status)) < 0)
        {
                printf("Errore wait\n");
                exit(5);
        }
        if ((status & 0xFF) != 0)
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else
        {
                ritorno=(int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %c (%d se 255 problemi!)\n", pidFiglio, ritorno, ritorno);
        }
}
exit (0);
}
