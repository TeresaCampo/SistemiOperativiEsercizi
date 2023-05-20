#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>

typedef int pipe_t[2];

int main(int argc, char ** argv){
int N;                              //numero di parametri passati
int pidFiglio, pidNipote, status, ritorno;     //per fork e wait
pipe_t *pipedFiglio;                      //array in cui tenere le N pipe create
pipe_t pipedNipote;
char letto[BUFSIZ];                         //dove scrivere i valori letti dalla pipe del nipote
int len;                            //dove scrivere la lunghezza della linea in int
long int tot;                       //dove scrivere delle linee nel padre

//deve avere almeno 2 parametri
if(argc<3){
    printf("Errore: devono essere passati almeno 2 parametri\n");
    exit (1);
}
N=argc-1;
pipedFiglio= malloc(N*sizeof(pipe_t));
//devono essere nomi assoluti di file, lo controllo nei figli

//il padre crea N pipe
for(int i=0; i<N; ++i){
    if(pipe(pipedFiglio[i])<0){
        printf("Errore: problemi nella creazione della pipe\n");
        exit (2);
    }
}
//il padre crea N processi figli
printf("Sono il processo padre con pid %d e sto per creare %d figli\n", getpid(), N);
for(int i=0; i<N; ++i){
    pidFiglio=fork();
    if(pidFiglio<0){
        printf("Errore nella fork\n");
        exit (-1);
    }
    if(pidFiglio==0){
        //chiudo tutte le pipe in lettura, tutte in scrittura tranne la mia e poi creo una nuova pipe con il nipote
        for(int j=0; j<N; ++j){
            close(pipedFiglio[j][0]); 
            if(j!=i) close(pipedFiglio[j][1]);
        }
        if(pipe(pipedNipote)<0){
        printf("Errore: problemi nella creazione della pipe\n");
        exit (3);
        }
        
        //creo il processo nipote
       printf("Sono il processo figlio di pid %d e sto per creare il mio processo nipote\n", getpid());
       pidNipote=fork();

       //controllo che il parametro sia un file e in valore assoluto
       if(open(argv[i+1], O_RDONLY )<0){
            printf("Errore: impossibile aprire in lettura il file %s\n", argv[i+1]);
            exit (-1);
       } 

            if(pidNipote<0){
            printf("Errore nella fork\n");
            exit (-1);        
            }
            if(pidNipote==0){
                printf("Sono il processo nipote di pid %d e sto per eseguire il comando filtro wc\n", getpid());
                //chiudo la pipe in lettura
                close(pipedNipote[0]);
                //ridireziono imput e oputput
                close(0);
                open(argv[i+1], O_RDONLY);
                close(1);
                dup(pipedNipote[1]);
                close(pipedNipote[1]);

                //provo a eseguire il comando filtro wc
                execlp("wc", "wc", "-l", (char *)0);

                exit (-1);                
            }
        if ((pidNipote=wait(&status)) < 0)
        {
                printf("Errore wait\n");
                exit(5);
        }
        if ((status & 0xFF) != 0)
                printf("Nipote con pid %d terminato in modo anomalo\n", pidNipote);
        else
        {
                ritorno=(int)((status >> 8) & 0xFF);
                printf("Il nipote con pid=%d ha ritornato %c (%d se 255 problemi!)\n", pidNipote, ritorno, ritorno);
        }
        close(pipedNipote[1]);
        int j=0;
        while(read(pipedNipote[0], &letto[j], 1)){
            if(letto[j]=='\n'){
                letto[j]=0;
            }
            ++j;
        }        
        len=atoi(letto);
        write(pipedFiglio[i][1], &len, sizeof(int));
        exit(ritorno);
    }
}
    //il padre legge le pipe e somma i valori
    for(int i=0; i<N; ++i){
        //chiudo la parte della pipe in scrittura
        close(pipedFiglio[i][1]);
        read(pipedFiglio[i][0], &len, sizeof(int));

        tot=tot+len;
    }

    //il padre aspetta tutti i figli
    for(int i=0; i<N; ++i){
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
    printf("Il numero totale di righe è %ld\n", tot);
    exit (0);
}