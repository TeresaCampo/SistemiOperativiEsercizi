#include <stdlib.h>
#include <stdio.>

int main(int agrc, char ** argv[]){
	//numero parametri: almeno 2 parametri
	if(argc<3){
	printf("Errore: numero di parametri deve essere almeno 2, invece %d\n", argc-1);
	exit (1);
	}
	//numer parametri ok
	N=argc-1;

int pidFiglio;
typedef int pipe_t[2];					//per contenere le due pipe che creo
pipe_t piped=(pipe_t *) malloc(2* sizeof(pipe_t));
int * fdFile;						//per contenere fd dei file, in quanto faccio i controllo subito
fdFile= (int *) malloc(N*sizeof(int));

char letto;
//controllo che tutti i parametri siano file
for(int i=0; i<N; ++i){
	fdFile[i]=open(argv[i+1], O_RDONLY);
	if(fdFile[i]<0){
	printf("Errore: non è possibile aprire il file %s\n", argv[i+1]);
	exit (2); 
	}
}
//tutti i parametri sono file apribili in lettura

//il processo padre crea 2 pipe: piped[0] per file pari, piped[1] per file dispari
if(pipe(piped[0])<0){
	printf("Errore: creazione pipe fallita\n");
	exit (3);
}
if(pipe(piped[1])<0){
        printf("Errore: creazione pipe fallita\n");
        exit (4);
}

//il processo padre chea N processi figli
for(int i=0; i<N; ++i){
pidFiglio=fork();
	if(pidFiglio<0){
	printf("Errore: fork fallita\n");
	exit (2);
	}
	if(pidFiglio==0){
		if((i%2)==0){ //file dispari, quindi usa piped[1]
		close(piped[0][0]); close(piped[0][1]); close(piped[1][0]);
		//deve scrivere sulla pipe solo i caratteri alfabetici
			while(read(fdFile(i), letto, 1){
				if(isalpha(letto)!=0){
				write(piped[1][1], letto,1);}
			}
		}
		else{ //file pari, quindi usa piped[0]
		close(piped[0][0]); close(piped[1][0]); close(piped[1][1]);
		//deve scrivere sulla pipe solo i caratteri numerici
			while(read(fdFile(i), letto, 1){
                                if(isalnum(letto)!=0){
                                write(piped[0][1], letto,1);}
                        }
		}
	}

	

	}

}

}
